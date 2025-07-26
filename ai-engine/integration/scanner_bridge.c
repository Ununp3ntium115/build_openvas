/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Scanner Integration Bridge
 * Connects external vulnerability data sources with OpenVAS scanner core
 */

#include "scanner_bridge.h"
#include "../scoring/vulnerability_scoring.h"
#include "../api/ai_service.h"
#include <gvm/base/prefs.h>
#include <gvm/util/kb.h>

/* Integration state */
static gboolean bridge_initialized = FALSE;
static GHashTable *scan_contexts = NULL;
static GMutex bridge_mutex;

/* OpenVAS integration hooks */
extern void (*vulnerability_detected_hook)(const gchar *cve_id, const gchar *host, gint port);
extern void (*scan_result_enhancement_hook)(scan_result_t *result);
extern void (*report_generation_hook)(report_t *report);

/**
 * @brief Scan context for tracking vulnerability data per scan
 */
typedef struct {
    gchar *scan_id;
    GHashTable *vulnerability_cache;    // CVE -> vulnerability_score_t
    GHashTable *host_contexts;          // host -> host_context_t
    ai_config_t *ai_config;
    gint64 scan_start_time;
    gboolean ai_enhancement_enabled;
} scan_context_t;

/**
 * @brief Host context for per-host vulnerability analysis
 */
typedef struct {
    gchar *host_ip;
    gchar *hostname;
    GPtrArray *detected_vulnerabilities; // Array of vulnerability_score_t*
    GHashTable *service_info;           // port -> service_info_t
    gchar *asset_criticality;           // For SSVC calculation
    gdouble composite_risk_score;
} host_context_t;

/**
 * @brief Service information for context-aware analysis
 */
typedef struct {
    gint port;
    gchar *service_name;
    gchar *service_version;
    gchar *cpe;                         // Common Platform Enumeration
    GPtrArray *vulnerabilities;         // CVEs affecting this service
} service_info_t;

/**
 * @brief Initialize scanner bridge
 */
gboolean
scanner_bridge_init(void)
{
    if (bridge_initialized) {
        return TRUE;
    }
    
    g_mutex_init(&bridge_mutex);
    scan_contexts = g_hash_table_new_full(g_str_hash, g_str_equal,
                                         g_free, (GDestroyNotify)scan_context_free);
    
    // Initialize vulnerability scoring system
    if (!vulnerability_scoring_init()) {
        g_critical(\"Failed to initialize vulnerability scoring system\");\n        return FALSE;
    }
    
    // Register OpenVAS hooks
    vulnerability_detected_hook = on_vulnerability_detected;
    scan_result_enhancement_hook = on_scan_result_enhancement;
    report_generation_hook = on_report_generation;
    
    bridge_initialized = TRUE;
    g_message(\"Scanner bridge initialized successfully\");
    return TRUE;
}

/**
 * @brief Cleanup scanner bridge
 */
void
scanner_bridge_cleanup(void)
{
    if (!bridge_initialized) {
        return;
    }
    
    g_mutex_lock(&bridge_mutex);
    
    if (scan_contexts) {
        g_hash_table_destroy(scan_contexts);
        scan_contexts = NULL;
    }
    
    vulnerability_scoring_cleanup();
    
    bridge_initialized = FALSE;
    g_mutex_unlock(&bridge_mutex);
    g_mutex_clear(&bridge_mutex);
    
    g_message(\"Scanner bridge cleaned up\");
}

/**
 * @brief Create new scan context
 */
scan_context_t *
scan_context_new(const gchar *scan_id)
{
    scan_context_t *ctx = g_malloc0(sizeof(scan_context_t));
    ctx->scan_id = g_strdup(scan_id);
    ctx->vulnerability_cache = g_hash_table_new_full(g_str_hash, g_str_equal,
                                                    g_free, (GDestroyNotify)vulnerability_score_free);
    ctx->host_contexts = g_hash_table_new_full(g_str_hash, g_str_equal,
                                              g_free, (GDestroyNotify)host_context_free);
    ctx->scan_start_time = g_get_real_time();
    
    // Check if AI enhancement is enabled
    ctx->ai_enhancement_enabled = prefs_get_bool(\"ai_enable_vulnerability_analysis\");
    
    if (ctx->ai_enhancement_enabled) {
        // Initialize AI configuration
        const gchar *ai_provider = prefs_get(\"ai_provider\");
        const gchar *ai_api_key = prefs_get(\"ai_api_key\");
        
        if (ai_provider && ai_api_key) {
            ai_provider_t provider = AI_PROVIDER_OPENAI; // Default
            if (g_strcmp0(ai_provider, \"claude\") == 0) {
                provider = AI_PROVIDER_CLAUDE;
            } else if (g_strcmp0(ai_provider, \"local\") == 0) {
                provider = AI_PROVIDER_LOCAL;
            }
            
            ctx->ai_config = ai_config_new(provider, ai_api_key);
        }
    }
    
    return ctx;
}

/**
 * @brief Free scan context
 */
void
scan_context_free(scan_context_t *ctx)
{
    if (!ctx) return;
    
    g_free(ctx->scan_id);
    if (ctx->vulnerability_cache) {
        g_hash_table_destroy(ctx->vulnerability_cache);
    }
    if (ctx->host_contexts) {
        g_hash_table_destroy(ctx->host_contexts);
    }
    if (ctx->ai_config) {
        ai_config_free(ctx->ai_config);
    }
    g_free(ctx);
}

/**
 * @brief Start scan context tracking
 */
gboolean
scanner_bridge_start_scan(const gchar *scan_id)
{
    if (!bridge_initialized) {
        g_warning(\"Scanner bridge not initialized\");
        return FALSE;
    }
    
    g_mutex_lock(&bridge_mutex);
    
    scan_context_t *ctx = scan_context_new(scan_id);
    g_hash_table_insert(scan_contexts, g_strdup(scan_id), ctx);
    
    g_mutex_unlock(&bridge_mutex);
    
    g_message(\"Started scan context for scan %s\", scan_id);
    return TRUE;
}

/**
 * @brief End scan context tracking
 */
void
scanner_bridge_end_scan(const gchar *scan_id)
{
    if (!bridge_initialized) {
        return;
    }
    
    g_mutex_lock(&bridge_mutex);
    
    scan_context_t *ctx = g_hash_table_lookup(scan_contexts, scan_id);
    if (ctx) {
        // Generate final scan report with comprehensive vulnerability data
        generate_enhanced_scan_report(ctx);
        
        // Remove context
        g_hash_table_remove(scan_contexts, scan_id);
    }
    
    g_mutex_unlock(&bridge_mutex);
    
    g_message(\"Ended scan context for scan %s\", scan_id);
}

/**
 * @brief Hook called when vulnerability is detected by NASL plugin
 */
void
on_vulnerability_detected(const gchar *cve_id, const gchar *host, gint port)
{
    if (!cve_id || !host) {
        return;
    }
    
    g_message(\"Vulnerability detected: %s on %s:%d\", cve_id, host, port);
    
    // Find active scan context (simplified - in real implementation, 
    // we'd track which scan is currently processing this host)
    scan_context_t *ctx = get_active_scan_context();
    if (!ctx) {
        g_warning(\"No active scan context for vulnerability %s\", cve_id);
        return;
    }
    
    // Get or create host context
    host_context_t *host_ctx = get_or_create_host_context(ctx, host);
    
    // Fetch comprehensive vulnerability data
    vulnerability_score_t *score = get_cached_or_fetch_vulnerability_score(ctx, cve_id);
    if (!score) {
        g_warning(\"Failed to fetch vulnerability data for %s\", cve_id);
        return;
    }
    
    // Add to host's vulnerability list
    g_ptr_array_add(host_ctx->detected_vulnerabilities, vulnerability_score_copy(score));
    
    // Update service information if port is specified
    if (port > 0) {
        update_service_vulnerability_info(host_ctx, port, score);
    }
    
    // AI enhancement if enabled
    if (ctx->ai_enhancement_enabled && ctx->ai_config) {
        ai_enhance_vulnerability_score(score);
        
        // Generate AI-powered remediation guidance
        gchar *guidance = ai_generate_contextual_guidance(score, host_ctx, ctx->ai_config);
        if (guidance) {
            // Store guidance in vulnerability score
            if (!score->ai_context) {
                score->ai_context = json_object_new();
            }
            json_object_set_string_member(score->ai_context, \"remediation_guidance\", guidance);
            g_free(guidance);
        }
    }
    
    // Update composite risk score for host
    update_host_risk_score(host_ctx);
    
    g_message(\"Enhanced vulnerability %s: CVSS=%.1f, KEV=%s, EPSS=%.3f, SSVC=%s\",
              cve_id,
              score->cvss_v3_1 ? score->cvss_v3_1->base_score : 0.0,
              score->kev && score->kev->is_kev ? \"YES\" : \"NO\",
              score->epss ? score->epss->score : 0.0,
              score->ssvc ? ssvc_decision_to_string(score->ssvc->decision) : \"UNKNOWN\");
}

/**
 * @brief Get cached vulnerability score or fetch from external sources
 */
vulnerability_score_t *
get_cached_or_fetch_vulnerability_score(scan_context_t *ctx, const gchar *cve_id)
{
    // Check cache first
    vulnerability_score_t *score = g_hash_table_lookup(ctx->vulnerability_cache, cve_id);
    if (score) {
        return score;
    }
    
    // Fetch comprehensive score from all external sources
    score = get_comprehensive_score(cve_id);
    if (score) {
        // Cache for future use in this scan
        g_hash_table_insert(ctx->vulnerability_cache, g_strdup(cve_id), score);
    }
    
    return score;
}

/**
 * @brief Hook called to enhance scan results
 */
void
on_scan_result_enhancement(scan_result_t *result)
{
    if (!result || !result->cve_id) {
        return;
    }
    
    scan_context_t *ctx = get_active_scan_context();
    if (!ctx) {
        return;
    }
    
    vulnerability_score_t *score = get_cached_or_fetch_vulnerability_score(ctx, result->cve_id);
    if (!score) {
        return;
    }
    
    // Enhance scan result with external data
    if (score->cvss_v3_1) {
        result->cvss_base_score = score->cvss_v3_1->base_score;
        result->cvss_severity = cvss_severity_to_string(score->cvss_v3_1->severity);
    }
    
    if (score->kev) {
        result->is_kev = score->kev->is_kev;
        result->kev_due_date = g_strdup(score->kev->due_date);
    }
    
    if (score->epss) {
        result->epss_score = score->epss->score;
        result->epss_percentile = score->epss->percentile;
    }
    
    if (score->ssvc) {
        result->ssvc_decision = ssvc_decision_to_string(score->ssvc->decision);
    }
    
    // AI-enhanced fields
    result->ai_risk_score = score->ai_risk_score;
    result->ai_priority = g_strdup(score->ai_priority);
    
    // Extract remediation guidance from AI context
    if (score->ai_context) {
        const gchar *guidance = json_object_get_string_member(score->ai_context, \"remediation_guidance\");
        if (guidance) {
            result->remediation_guidance = g_strdup(guidance);
        }
    }
}

/**
 * @brief Generate enhanced scan report with comprehensive vulnerability data
 */
void
generate_enhanced_scan_report(scan_context_t *ctx)
{
    g_message(\"Generating enhanced scan report for scan %s\", ctx->scan_id);
    
    // Collect all vulnerabilities from all hosts
    GPtrArray *all_vulnerabilities = g_ptr_array_new();
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, ctx->host_contexts);
    
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        host_context_t *host_ctx = (host_context_t *)value;
        
        for (guint i = 0; i < host_ctx->detected_vulnerabilities->len; i++) {
            vulnerability_score_t *score = g_ptr_array_index(host_ctx->detected_vulnerabilities, i);
            g_ptr_array_add(all_vulnerabilities, score);
        }
    }
    
    // Generate comprehensive vulnerability report
    JsonObject *report = generate_vulnerability_report(all_vulnerabilities);
    
    // Add scan-specific metadata
    json_object_set_string_member(report, \"scan_id\", ctx->scan_id);
    json_object_set_int_member(report, \"scan_start_time\", ctx->scan_start_time);
    json_object_set_int_member(report, \"scan_end_time\", g_get_real_time());
    json_object_set_boolean_member(report, \"ai_enhancement_enabled\", ctx->ai_enhancement_enabled);
    
    // Generate executive summary
    gchar *executive_summary = generate_executive_summary(all_vulnerabilities);
    json_object_set_string_member(report, \"executive_summary\", executive_summary);
    
    // Get top vulnerabilities by different criteria
    JsonArray *top_by_cvss = get_top_vulnerabilities_by_cvss(all_vulnerabilities, 10);
    JsonArray *top_by_epss = get_top_vulnerabilities_by_epss(all_vulnerabilities, 10);
    JsonArray *kev_vulnerabilities = get_kev_vulnerabilities(all_vulnerabilities);
    JsonArray *act_vulnerabilities = get_ssvc_act_vulnerabilities(all_vulnerabilities);
    
    json_object_set_array_member(report, \"top_by_cvss\", top_by_cvss);
    json_object_set_array_member(report, \"top_by_epss\", top_by_epss);
    json_object_set_array_member(report, \"kev_vulnerabilities\", kev_vulnerabilities);
    json_object_set_array_member(report, \"ssvc_act_vulnerabilities\", act_vulnerabilities);
    
    // Save enhanced report
    save_enhanced_report(ctx->scan_id, report);
    
    // Cleanup
    g_ptr_array_free(all_vulnerabilities, FALSE); // Don't free elements, they're owned by contexts
    g_free(executive_summary);
    json_object_unref(report);
}

/**
 * @brief Update host risk score based on detected vulnerabilities
 */
void
update_host_risk_score(host_context_t *host_ctx)
{
    if (!host_ctx->detected_vulnerabilities || host_ctx->detected_vulnerabilities->len == 0) {
        host_ctx->composite_risk_score = 0.0;
        return;
    }
    
    gdouble total_risk = 0.0;
    gdouble max_cvss = 0.0;
    gint kev_count = 0;
    gdouble avg_epss = 0.0;
    gint act_count = 0;
    
    for (guint i = 0; i < host_ctx->detected_vulnerabilities->len; i++) {
        vulnerability_score_t *score = g_ptr_array_index(host_ctx->detected_vulnerabilities, i);
        
        // CVSS contribution
        if (score->cvss_v3_1) {
            max_cvss = MAX(max_cvss, score->cvss_v3_1->base_score);
            total_risk += score->cvss_v3_1->base_score;
        }
        
        // KEV contribution (high weight)
        if (score->kev && score->kev->is_kev) {
            kev_count++;
            total_risk += 15.0; // High penalty for KEV
        }
        
        // EPSS contribution
        if (score->epss) {
            avg_epss += score->epss->score;
            total_risk += score->epss->score * 10.0; // Scale EPSS to CVSS range
        }
        
        // SSVC contribution
        if (score->ssvc && score->ssvc->decision == SSVC_ACT) {
            act_count++;
            total_risk += 12.0; // High penalty for Act decision
        }
        
        // AI risk contribution
        if (score->ai_risk_score > 0.0) {
            total_risk += score->ai_risk_score;
        }
    }
    
    // Calculate composite score (normalized to 0-100 range)
    gdouble vuln_count = (gdouble)host_ctx->detected_vulnerabilities->len;
    avg_epss /= vuln_count;
    
    host_ctx->composite_risk_score = MIN(100.0, total_risk / vuln_count);
    
    g_message(\"Host %s risk score: %.1f (vulns=%d, max_cvss=%.1f, kev=%d, avg_epss=%.3f, act=%d)\",
              host_ctx->host_ip, host_ctx->composite_risk_score,
              (gint)vuln_count, max_cvss, kev_count, avg_epss, act_count);
}

/**
 * @brief Integration with OpenVAS NASL plugins
 */
void
nasl_vulnerability_detected(const gchar *cve_id, const gchar *host, gint port, 
                           const gchar *plugin_oid, const gchar *description)
{
    // This function would be called from NASL plugins when they detect vulnerabilities
    // It bridges the gap between NASL script execution and our enhanced scoring system
    
    g_message(\"NASL plugin %s detected %s on %s:%d\", plugin_oid, cve_id, host, port);
    
    // Call our vulnerability detection hook
    on_vulnerability_detected(cve_id, host, port);
    
    // Store additional context from NASL plugin
    scan_context_t *ctx = get_active_scan_context();
    if (ctx) {
        host_context_t *host_ctx = get_or_create_host_context(ctx, host);
        
        // Store plugin information
        if (!host_ctx->service_info) {
            host_ctx->service_info = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                                          NULL, (GDestroyNotify)service_info_free);
        }
        
        service_info_t *service = g_hash_table_lookup(host_ctx->service_info, GINT_TO_POINTER(port));
        if (!service) {
            service = g_malloc0(sizeof(service_info_t));
            service->port = port;
            service->vulnerabilities = g_ptr_array_new_with_free_func(g_free);
            g_hash_table_insert(host_ctx->service_info, GINT_TO_POINTER(port), service);
        }
        
        g_ptr_array_add(service->vulnerabilities, g_strdup(cve_id));
    }
}

/**
 * @brief Get active scan context (simplified implementation)
 */
scan_context_t *
get_active_scan_context(void)
{
    // In a real implementation, this would track which scan is currently active
    // For now, return the first context (assuming single scan at a time)
    
    g_mutex_lock(&bridge_mutex);
    
    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, scan_contexts);
    
    if (g_hash_table_iter_next(&iter, &key, &value)) {
        g_mutex_unlock(&bridge_mutex);
        return (scan_context_t *)value;
    }
    
    g_mutex_unlock(&bridge_mutex);
    return NULL;
}