/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Scanner Integration Bridge Header
 * Connects external vulnerability data sources with OpenVAS scanner core
 */

#ifndef SCANNER_BRIDGE_H
#define SCANNER_BRIDGE_H

#include <glib.h>
#include <json-glib/json-glib.h>
#include "../scoring/vulnerability_scoring.h"
#include "../api/ai_service.h"

/**
 * @brief Scan result structure (enhanced)
 */
typedef struct {
    gchar *cve_id;
    gchar *host;
    gint port;
    gchar *plugin_oid;
    gchar *description;
    
    // Enhanced with external data
    gdouble cvss_base_score;
    gchar *cvss_severity;
    gboolean is_kev;
    gchar *kev_due_date;
    gdouble epss_score;
    gdouble epss_percentile;
    gchar *ssvc_decision;
    
    // AI-enhanced fields
    gdouble ai_risk_score;
    gchar *ai_priority;
    gchar *remediation_guidance;
    
    // Metadata
    gint64 detection_time;
    gchar *scanner_version;
} scan_result_t;

/**
 * @brief Report structure (enhanced)
 */
typedef struct {
    gchar *scan_id;
    gint64 start_time;
    gint64 end_time;
    GPtrArray *hosts;           // Array of host_context_t*
    GPtrArray *vulnerabilities; // Array of vulnerability_score_t*
    JsonObject *summary;
    JsonObject *ai_analysis;
    gboolean ai_enhanced;
} report_t;

/* Forward declarations */
typedef struct scan_context_t scan_context_t;
typedef struct host_context_t host_context_t;
typedef struct service_info_t service_info_t;

/* Core bridge functions */
gboolean scanner_bridge_init(void);
void scanner_bridge_cleanup(void);

/* Scan lifecycle management */
gboolean scanner_bridge_start_scan(const gchar *scan_id);
void scanner_bridge_end_scan(const gchar *scan_id);

/* Hook functions called by OpenVAS scanner */
void on_vulnerability_detected(const gchar *cve_id, const gchar *host, gint port);
void on_scan_result_enhancement(scan_result_t *result);
void on_report_generation(report_t *report);

/* NASL plugin integration */
void nasl_vulnerability_detected(const gchar *cve_id, const gchar *host, gint port, 
                                const gchar *plugin_oid, const gchar *description);

/* Vulnerability data management */
vulnerability_score_t *get_cached_or_fetch_vulnerability_score(scan_context_t *ctx, const gchar *cve_id);
void update_host_risk_score(host_context_t *host_ctx);

/* Context management */
scan_context_t *get_active_scan_context(void);
host_context_t *get_or_create_host_context(scan_context_t *ctx, const gchar *host);
void scan_context_free(scan_context_t *ctx);
void host_context_free(host_context_t *ctx);
void service_info_free(service_info_t *info);

/* Report generation */
void generate_enhanced_scan_report(scan_context_t *ctx);
JsonArray *get_top_vulnerabilities_by_cvss(GPtrArray *vulnerabilities, guint count);
JsonArray *get_top_vulnerabilities_by_epss(GPtrArray *vulnerabilities, guint count);
JsonArray *get_kev_vulnerabilities(GPtrArray *vulnerabilities);
JsonArray *get_ssvc_act_vulnerabilities(GPtrArray *vulnerabilities);
void save_enhanced_report(const gchar *scan_id, JsonObject *report);

/* AI integration helpers */
gchar *ai_generate_contextual_guidance(vulnerability_score_t *score, 
                                      host_context_t *host_ctx, 
                                      ai_config_t *ai_config);
void update_service_vulnerability_info(host_context_t *host_ctx, gint port, 
                                      vulnerability_score_t *score);

/* Configuration helpers */
gboolean is_ai_enhancement_enabled(void);
gchar *get_asset_criticality(const gchar *host);
gint get_scan_priority_for_vulnerability(vulnerability_score_t *score);

/* Utility functions */
scan_result_t *scan_result_new(void);
void scan_result_free(scan_result_t *result);
report_t *report_new(const gchar *scan_id);
void report_free(report_t *report);

/* Statistics and monitoring */
typedef struct {
    guint total_vulnerabilities_detected;
    guint kev_vulnerabilities_detected;
    guint critical_vulnerabilities_detected;
    guint ai_enhanced_results;
    gdouble avg_enhancement_time_ms;
    guint external_api_calls;
    guint cache_hits;
    guint cache_misses;
} bridge_stats_t;

bridge_stats_t *get_bridge_statistics(void);
void reset_bridge_statistics(void);

#endif /* SCANNER_BRIDGE_H */