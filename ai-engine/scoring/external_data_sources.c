/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * External Data Sources Integration
 * Fetches vulnerability data from NVD, CISA KEV, EPSS, and other sources
 */

#include "vulnerability_scoring.h"
#include <curl/curl.h>
#include <json-glib/json-glib.h>

/* API endpoints */
#define NVD_API_BASE "https://services.nvd.nist.gov/rest/json/cves/2.0"
#define KEV_API_BASE "https://www.cisa.gov/sites/default/files/feeds/known_exploited_vulnerabilities.json"
#define EPSS_API_BASE "https://api.first.org/data/v1/epss"
#define FIRST_SSVC_BASE "https://api.first.org/data/v1/ssvc"

/* Rate limiting */
#define NVD_RATE_LIMIT_DELAY_MS 6000  // NVD allows 5 requests per 30 seconds without API key
#define EPSS_RATE_LIMIT_DELAY_MS 1000 // FIRST.org rate limiting

static GMutex rate_limit_mutex;
static gint64 last_nvd_request = 0;
static gint64 last_epss_request = 0;

/**
 * @brief Rate limiting for API requests
 */
static void
rate_limit_wait(gint64 *last_request, gint64 delay_ms)
{
    g_mutex_lock(&rate_limit_mutex);
    
    gint64 current_time = g_get_monotonic_time() / 1000; // Convert to milliseconds
    gint64 time_since_last = current_time - *last_request;
    
    if (time_since_last < delay_ms) {
        gint64 wait_time = delay_ms - time_since_last;
        g_mutex_unlock(&rate_limit_mutex);
        g_usleep(wait_time * 1000); // Convert to microseconds
        g_mutex_lock(&rate_limit_mutex);
    }
    
    *last_request = g_get_monotonic_time() / 1000;
    g_mutex_unlock(&rate_limit_mutex);
}

/**
 * @brief Make HTTP request with error handling
 */
static gchar *
make_http_request(const gchar *url, const gchar *headers[])
{
    CURL *curl;
    CURLcode res;
    http_response_t response = {0};
    
    curl = curl_easy_init();
    if (!curl) {
        g_warning("Failed to initialize CURL for %s", url);
        return NULL;
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "AI-Enhanced-OpenVAS/1.0");
    
    // Set headers if provided
    struct curl_slist *header_list = NULL;
    if (headers) {
        for (int i = 0; headers[i]; i++) {
            header_list = curl_slist_append(header_list, headers[i]);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    }
    
    res = curl_easy_perform(curl);
    
    if (header_list) {
        curl_slist_free_all(header_list);
    }
    
    if (res != CURLE_OK) {
        g_warning("CURL request failed for %s: %s", url, curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        g_free(response.data);
        return NULL;
    }
    
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    curl_easy_cleanup(curl);
    
    if (response_code != 200) {
        g_warning("HTTP request failed for %s: %ld", url, response_code);
        g_free(response.data);
        return NULL;
    }
    
    return response.data;
}

/**
 * @brief Fetch vulnerability data from NVD
 */
gboolean
fetch_nvd_data(const gchar *cve_id, vulnerability_score_t *score)
{
    if (!cve_id || !score) {
        return FALSE;
    }
    
    // Rate limiting for NVD API
    rate_limit_wait(&last_nvd_request, NVD_RATE_LIMIT_DELAY_MS);
    
    gchar *url = g_strdup_printf("%s?cveId=%s", NVD_API_BASE, cve_id);
    gchar *response_data = make_http_request(url, NULL);
    g_free(url);
    
    if (!response_data) {
        return FALSE;
    }
    
    // Parse JSON response
    JsonParser *parser = json_parser_new();
    GError *error = NULL;
    
    if (!json_parser_load_from_data(parser, response_data, -1, &error)) {
        g_warning("Failed to parse NVD response for %s: %s", cve_id, error->message);
        g_error_free(error);
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonNode *root = json_parser_get_root(parser);
    JsonObject *root_obj = json_node_get_object(root);
    
    if (!json_object_has_member(root_obj, "vulnerabilities")) {
        g_warning("No vulnerabilities found in NVD response for %s", cve_id);
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonArray *vulnerabilities = json_object_get_array_member(root_obj, "vulnerabilities");
    if (json_array_get_length(vulnerabilities) == 0) {
        g_warning("Empty vulnerabilities array in NVD response for %s", cve_id);
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonObject *vuln = json_array_get_object_element(vulnerabilities, 0);
    JsonObject *cve = json_object_get_object_member(vuln, "cve");
    
    // Extract basic information
    if (json_object_has_member(cve, "descriptions")) {
        JsonArray *descriptions = json_object_get_array_member(cve, "descriptions");
        if (json_array_get_length(descriptions) > 0) {
            JsonObject *desc = json_array_get_object_element(descriptions, 0);
            score->description = g_strdup(json_object_get_string_member(desc, "value"));
        }
    }
    
    if (json_object_has_member(cve, "published")) {
        score->published_date = g_strdup(json_object_get_string_member(cve, "published"));
    }
    
    if (json_object_has_member(cve, "lastModified")) {
        score->last_modified = g_strdup(json_object_get_string_member(cve, "lastModified"));
    }
    
    // Extract CVSS scores
    if (json_object_has_member(cve, "metrics")) {
        JsonObject *metrics = json_object_get_object_member(cve, "metrics");
        
        // CVSS v3.1
        if (json_object_has_member(metrics, "cvssMetricV31")) {
            JsonArray *cvss_v31_array = json_object_get_array_member(metrics, "cvssMetricV31");
            if (json_array_get_length(cvss_v31_array) > 0) {
                JsonObject *cvss_v31_obj = json_array_get_object_element(cvss_v31_array, 0);
                JsonObject *cvss_data = json_object_get_object_member(cvss_v31_obj, "cvssData");
                
                score->cvss_v3_1 = g_malloc0(sizeof(cvss_v3_t));
                score->cvss_v3_1->base_score = json_object_get_double_member(cvss_data, "baseScore");
                score->cvss_v3_1->attack_vector = g_strdup(json_object_get_string_member(cvss_data, "attackVector"));
                score->cvss_v3_1->attack_complexity = g_strdup(json_object_get_string_member(cvss_data, "attackComplexity"));
                score->cvss_v3_1->privileges_required = g_strdup(json_object_get_string_member(cvss_data, "privilegesRequired"));
                score->cvss_v3_1->user_interaction = g_strdup(json_object_get_string_member(cvss_data, "userInteraction"));
                score->cvss_v3_1->scope = g_strdup(json_object_get_string_member(cvss_data, "scope"));
                score->cvss_v3_1->confidentiality = g_strdup(json_object_get_string_member(cvss_data, "confidentialityImpact"));
                score->cvss_v3_1->integrity = g_strdup(json_object_get_string_member(cvss_data, "integrityImpact"));
                score->cvss_v3_1->availability = g_strdup(json_object_get_string_member(cvss_data, "availabilityImpact"));
                score->cvss_v3_1->severity = cvss_v3_get_severity(score->cvss_v3_1->base_score);
            }
        }
        
        // CVSS v3.0
        if (json_object_has_member(metrics, "cvssMetricV30")) {
            JsonArray *cvss_v30_array = json_object_get_array_member(metrics, "cvssMetricV30");
            if (json_array_get_length(cvss_v30_array) > 0) {
                JsonObject *cvss_v30_obj = json_array_get_object_element(cvss_v30_array, 0);
                JsonObject *cvss_data = json_object_get_object_member(cvss_v30_obj, "cvssData");
                
                score->cvss_v3_0 = g_malloc0(sizeof(cvss_v3_t));
                score->cvss_v3_0->base_score = json_object_get_double_member(cvss_data, "baseScore");
                // ... similar parsing for v3.0
            }
        }
        
        // CVSS v2
        if (json_object_has_member(metrics, "cvssMetricV2")) {
            JsonArray *cvss_v2_array = json_object_get_array_member(metrics, "cvssMetricV2");
            if (json_array_get_length(cvss_v2_array) > 0) {
                JsonObject *cvss_v2_obj = json_array_get_object_element(cvss_v2_array, 0);
                JsonObject *cvss_data = json_object_get_object_member(cvss_v2_obj, "cvssData");
                
                score->cvss_v2 = g_malloc0(sizeof(cvss_v2_t));
                score->cvss_v2->base_score = json_object_get_double_member(cvss_data, "baseScore");
                // ... similar parsing for v2
            }
        }
    }
    
    // Extract CWE information
    if (json_object_has_member(cve, "weaknesses")) {
        JsonArray *weaknesses = json_object_get_array_member(cve, "weaknesses");
        GPtrArray *cwe_array = g_ptr_array_new();
        
        for (guint i = 0; i < json_array_get_length(weaknesses); i++) {
            JsonObject *weakness = json_array_get_object_element(weaknesses, i);
            JsonArray *descriptions = json_object_get_array_member(weakness, "description");
            
            for (guint j = 0; j < json_array_get_length(descriptions); j++) {
                JsonObject *desc = json_array_get_object_element(descriptions, j);
                const gchar *cwe_id = json_object_get_string_member(desc, "value");
                g_ptr_array_add(cwe_array, g_strdup(cwe_id));
            }
        }
        
        g_ptr_array_add(cwe_array, NULL);
        score->cwe_ids = (gchar **)g_ptr_array_free(cwe_array, FALSE);
    }
    
    g_object_unref(parser);
    g_free(response_data);
    
    g_message("Successfully fetched NVD data for %s", cve_id);
    return TRUE;
}/**
 *
 @brief Fetch KEV data from CISA
 */
gboolean
fetch_kev_data(const gchar *cve_id, kev_info_t *kev)
{
    if (!cve_id || !kev) {
        return FALSE;
    }
    
    // Fetch the entire KEV catalog (it's updated daily)
    gchar *response_data = make_http_request(KEV_API_BASE, NULL);
    if (!response_data) {
        return FALSE;
    }
    
    JsonParser *parser = json_parser_new();
    GError *error = NULL;
    
    if (!json_parser_load_from_data(parser, response_data, -1, &error)) {
        g_warning("Failed to parse KEV response: %s", error->message);
        g_error_free(error);
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonNode *root = json_parser_get_root(parser);
    JsonObject *root_obj = json_node_get_object(root);
    
    if (!json_object_has_member(root_obj, "vulnerabilities")) {
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonArray *vulnerabilities = json_object_get_array_member(root_obj, "vulnerabilities");
    gboolean found = FALSE;
    
    // Search for the CVE in the KEV catalog
    for (guint i = 0; i < json_array_get_length(vulnerabilities); i++) {
        JsonObject *vuln = json_array_get_object_element(vulnerabilities, i);
        const gchar *vuln_cve = json_object_get_string_member(vuln, "cveID");
        
        if (g_strcmp0(vuln_cve, cve_id) == 0) {
            kev->is_kev = TRUE;
            kev->date_added = g_strdup(json_object_get_string_member(vuln, "dateAdded"));
            kev->due_date = g_strdup(json_object_get_string_member(vuln, "dueDate"));
            kev->required_action = g_strdup(json_object_get_string_member(vuln, "requiredAction"));
            
            if (json_object_has_member(vuln, "knownRansomwareCampaignUse")) {
                kev->known_ransomware_use = g_strdup(json_object_get_string_member(vuln, "knownRansomwareCampaignUse"));
            }
            
            if (json_object_has_member(vuln, "notes")) {
                kev->notes = g_strdup(json_object_get_string_member(vuln, "notes"));
            }
            
            kev->last_updated = g_get_real_time();
            found = TRUE;
            break;
        }
    }
    
    if (!found) {
        kev->is_kev = FALSE;
        kev->last_updated = g_get_real_time();
    }
    
    g_object_unref(parser);
    g_free(response_data);
    
    g_message("KEV lookup for %s: %s", cve_id, found ? "Found" : "Not found");
    return TRUE;
}

/**
 * @brief Fetch EPSS data from FIRST.org
 */
gboolean
fetch_epss_data(const gchar *cve_id, epss_info_t *epss)
{
    if (!cve_id || !epss) {
        return FALSE;
    }
    
    // Rate limiting for EPSS API
    rate_limit_wait(&last_epss_request, EPSS_RATE_LIMIT_DELAY_MS);
    
    gchar *url = g_strdup_printf("%s?cve=%s", EPSS_API_BASE, cve_id);
    gchar *response_data = make_http_request(url, NULL);
    g_free(url);
    
    if (!response_data) {
        return FALSE;
    }
    
    JsonParser *parser = json_parser_new();
    GError *error = NULL;
    
    if (!json_parser_load_from_data(parser, response_data, -1, &error)) {
        g_warning("Failed to parse EPSS response for %s: %s", cve_id, error->message);
        g_error_free(error);
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonNode *root = json_parser_get_root(parser);
    JsonObject *root_obj = json_node_get_object(root);
    
    if (!json_object_has_member(root_obj, "data")) {
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonArray *data_array = json_object_get_array_member(root_obj, "data");
    if (json_array_get_length(data_array) == 0) {
        g_object_unref(parser);
        g_free(response_data);
        return FALSE;
    }
    
    JsonObject *epss_data = json_array_get_object_element(data_array, 0);
    
    epss->score = json_object_get_double_member(epss_data, "epss");
    epss->percentile = json_object_get_double_member(epss_data, "percentile");
    epss->date = g_strdup(json_object_get_string_member(epss_data, "date"));
    epss->last_updated = g_get_real_time();
    
    // Get model version from response metadata if available
    if (json_object_has_member(root_obj, "model_version")) {
        epss->model_version = g_strdup(json_object_get_string_member(root_obj, "model_version"));
    } else {
        epss->model_version = g_strdup("unknown");
    }
    
    g_object_unref(parser);
    g_free(response_data);
    
    g_message("Successfully fetched EPSS data for %s: score=%.5f, percentile=%.3f", 
              cve_id, epss->score, epss->percentile);
    return TRUE;
}

/**
 * @brief Fetch SSVC data (placeholder - would integrate with FIRST.org or custom implementation)
 */
gboolean
fetch_first_data(const gchar *cve_id, ssvc_info_t *ssvc)
{
    if (!cve_id || !ssvc) {
        return FALSE;
    }
    
    // For now, implement a basic SSVC decision logic based on available data
    // In production, this would integrate with FIRST.org SSVC API or custom SSVC implementation
    
    ssvc->exploitation = g_strdup("none");  // Default to no known exploitation
    ssvc->automatable = g_strdup("no");     // Default to not automatable
    ssvc->technical_impact = g_strdup("partial"); // Default technical impact
    ssvc->mission_impact = g_strdup("medium");     // Default mission impact
    ssvc->safety_impact = g_strdup("none");        // Default safety impact
    ssvc->public_safety_impact = g_strdup("minimal"); // Default public safety impact
    ssvc->situated_safety_impact = g_strdup("minimal"); // Default situated safety impact
    ssvc->decision = SSVC_TRACK;  // Default decision
    ssvc->last_updated = g_get_real_time();
    
    g_message("Generated default SSVC data for %s", cve_id);
    return TRUE;
}

/**
 * @brief Get comprehensive vulnerability score from all sources
 */
vulnerability_score_t *
get_comprehensive_score(const gchar *cve_id)
{
    if (!cve_id) {
        return NULL;
    }
    
    vulnerability_score_t *score = vulnerability_score_new(cve_id);
    
    // Fetch data from all sources
    gboolean nvd_success = fetch_nvd_data(cve_id, score);
    
    if (nvd_success) {
        score->kev = g_malloc0(sizeof(kev_info_t));
        fetch_kev_data(cve_id, score->kev);
        
        score->epss = g_malloc0(sizeof(epss_info_t));
        fetch_epss_data(cve_id, score->epss);
        
        score->ssvc = g_malloc0(sizeof(ssvc_info_t));
        fetch_first_data(cve_id, score->ssvc);
        
        // AI-enhance the score
        ai_enhance_vulnerability_score(score);
    } else {
        g_warning("Failed to fetch NVD data for %s", cve_id);
        vulnerability_score_free(score);
        return NULL;
    }
    
    return score;
}

/**
 * @brief AI-enhance vulnerability score
 */
gboolean
ai_enhance_vulnerability_score(vulnerability_score_t *score)
{
    if (!score) {
        return FALSE;
    }
    
    // Calculate AI composite risk score
    score->ai_risk_score = ai_calculate_composite_risk_score(score);
    
    // Determine AI-suggested priority
    score->ai_priority = ai_determine_remediation_priority(score);
    
    // Set remediation urgency based on multiple factors
    if (score->kev && score->kev->is_kev) {
        score->ai_remediation_urgency = g_strdup("IMMEDIATE");
    } else if (score->cvss_v3_1 && score->cvss_v3_1->severity == CVSS_SEVERITY_CRITICAL) {
        if (score->epss && score->epss->score > 0.1) {
            score->ai_remediation_urgency = g_strdup("HIGH");
        } else {
            score->ai_remediation_urgency = g_strdup("MEDIUM");
        }
    } else if (score->cvss_v3_1 && score->cvss_v3_1->severity == CVSS_SEVERITY_HIGH) {
        score->ai_remediation_urgency = g_strdup("MEDIUM");
    } else {
        score->ai_remediation_urgency = g_strdup("LOW");
    }
    
    // Create AI context object
    score->ai_context = json_object_new();
    json_object_set_double_member(score->ai_context, "composite_risk_score", score->ai_risk_score);
    json_object_set_string_member(score->ai_context, "priority_reasoning", 
        "Based on CVSS score, KEV status, EPSS probability, and SSVC decision");
    
    return TRUE;
}

/**
 * @brief Calculate AI composite risk score
 */
gdouble
ai_calculate_composite_risk_score(vulnerability_score_t *score)
{
    if (!score) {
        return 0.0;
    }
    
    gdouble composite_score = 0.0;
    gdouble weight_sum = 0.0;
    
    // CVSS base score (weight: 0.4)
    if (score->cvss_v3_1) {
        composite_score += score->cvss_v3_1->base_score * 0.4;
        weight_sum += 0.4;
    } else if (score->cvss_v3_0) {
        composite_score += score->cvss_v3_0->base_score * 0.4;
        weight_sum += 0.4;
    } else if (score->cvss_v2) {
        composite_score += score->cvss_v2->base_score * 0.4;
        weight_sum += 0.4;
    }
    
    // KEV status (weight: 0.3)
    if (score->kev) {
        if (score->kev->is_kev) {
            composite_score += 10.0 * 0.3;  // Maximum impact for KEV
        }
        weight_sum += 0.3;
    }
    
    // EPSS score (weight: 0.2)
    if (score->epss) {
        // Convert EPSS (0-1) to 0-10 scale
        composite_score += (score->epss->score * 10.0) * 0.2;
        weight_sum += 0.2;
    }
    
    // SSVC decision (weight: 0.1)
    if (score->ssvc) {
        gdouble ssvc_score = 0.0;
        switch (score->ssvc->decision) {
            case SSVC_TRACK: ssvc_score = 2.5; break;
            case SSVC_TRACK_STAR: ssvc_score = 5.0; break;
            case SSVC_ATTEND: ssvc_score = 7.5; break;
            case SSVC_ACT: ssvc_score = 10.0; break;
        }
        composite_score += ssvc_score * 0.1;
        weight_sum += 0.1;
    }
    
    // Normalize by actual weights used
    if (weight_sum > 0.0) {
        composite_score = composite_score / weight_sum * 10.0;
    }
    
    return fmin(composite_score, 10.0);
}

/**
 * @brief Determine AI remediation priority
 */
gchar *
ai_determine_remediation_priority(vulnerability_score_t *score)
{
    if (!score) {
        return g_strdup("UNKNOWN");
    }
    
    // KEV vulnerabilities are always critical priority
    if (score->kev && score->kev->is_kev) {
        return g_strdup("CRITICAL");
    }
    
    // High CVSS + High EPSS = Critical
    if (score->cvss_v3_1 && score->cvss_v3_1->severity >= CVSS_SEVERITY_HIGH &&
        score->epss && score->epss->score > 0.1) {
        return g_strdup("CRITICAL");
    }
    
    // Critical CVSS alone = High priority
    if (score->cvss_v3_1 && score->cvss_v3_1->severity == CVSS_SEVERITY_CRITICAL) {
        return g_strdup("HIGH");
    }
    
    // High CVSS = Medium priority
    if (score->cvss_v3_1 && score->cvss_v3_1->severity == CVSS_SEVERITY_HIGH) {
        return g_strdup("MEDIUM");
    }
    
    // SSVC Act decision = High priority
    if (score->ssvc && score->ssvc->decision == SSVC_ACT) {
        return g_strdup("HIGH");
    }
    
    // Default based on CVSS severity
    if (score->cvss_v3_1) {
        switch (score->cvss_v3_1->severity) {
            case CVSS_SEVERITY_CRITICAL: return g_strdup("HIGH");
            case CVSS_SEVERITY_HIGH: return g_strdup("MEDIUM");
            case CVSS_SEVERITY_MEDIUM: return g_strdup("LOW");
            default: return g_strdup("INFO");
        }
    }
    
    return g_strdup("LOW");
}