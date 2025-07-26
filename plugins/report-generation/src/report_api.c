/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Professional LaTeX Report Generation API Implementation
 */

#include "report_api.h"
#include "latex_engine.h"
#include "template_manager.h"
#include <time.h>
#include <string.h>

/* Global state */
static gboolean g_report_api_initialized = FALSE;
static report_metrics_t g_report_metrics = {0};
static gchar *g_last_error = NULL;
static void (*g_error_callback)(const gchar *error, gpointer user_data) = NULL;
static gpointer g_error_callback_data = NULL;

/* Internal helper functions */
static void report_set_last_error(const gchar *error);
static latex_variables_t *report_prepare_variables(vulnerability_score_t **vulnerabilities,
                                                   guint vulnerability_count,
                                                   report_config_t *config);
static gchar *report_generate_vulnerability_table(vulnerability_score_t **vulnerabilities,
                                                 guint vulnerability_count,
                                                 guint max_entries);

/**
 * @brief Initialize the report API system
 */
gboolean
report_api_init(void)
{
    if (g_report_api_initialized) {
        return TRUE;
    }
    
    // Initialize LaTeX engine
    if (!latex_engine_init()) {
        report_set_last_error("Failed to initialize LaTeX engine");
        return FALSE;
    }
    
    // Initialize template manager
    gchar *templates_dir = g_build_filename(g_get_current_dir(), 
                                          "plugins", "report-generation", "templates", NULL);
    if (!template_manager_init(templates_dir)) {
        report_set_last_error("Failed to initialize template manager");
        g_free(templates_dir);
        return FALSE;
    }
    g_free(templates_dir);
    
    // Initialize metrics
    memset(&g_report_metrics, 0, sizeof(report_metrics_t));
    g_report_metrics.most_used_template = g_strdup("executive_summary");
    
    g_report_api_initialized = TRUE;
    g_info("Report API initialized successfully");
    
    return TRUE;
}

/**
 * @brief Cleanup report API resources
 */
void
report_api_cleanup(void)
{
    if (!g_report_api_initialized) {
        return;
    }
    
    latex_engine_cleanup();
    template_manager_cleanup();
    
    g_free(g_report_metrics.most_used_template);
    g_free(g_last_error);
    
    g_report_api_initialized = FALSE;
}

/**
 * @brief Check if report API is initialized
 */
gboolean
report_api_is_initialized(void)
{
    return g_report_api_initialized;
}

/**
 * @brief Create new report configuration
 */
report_config_t *
report_config_new(void)
{
    report_config_t *config = g_new0(report_config_t, 1);
    
    // Set defaults
    config->template_name = g_strdup("executive_summary");
    config->output_format = REPORT_FORMAT_PDF;
    config->output_path = g_strdup("/tmp/openvas_report.pdf");
    config->company_name = g_strdup("OpenVAS Security Assessment");
    config->client_name = g_strdup("Client Organization");
    config->report_title = g_strdup("Vulnerability Assessment Report");
    config->include_charts = TRUE;
    config->include_ai_insights = TRUE;
    config->language = g_strdup("en");
    config->custom_variables = json_object_new();
    
    return config;
}

/**
 * @brief Free report configuration
 */
void
report_config_free(report_config_t *config)
{
    if (!config) return;
    
    g_free(config->template_name);
    g_free(config->output_path);
    g_free(config->company_name);
    g_free(config->company_logo);
    g_free(config->client_name);
    g_free(config->report_title);
    g_free(config->language);
    
    if (config->custom_variables) {
        json_object_unref(config->custom_variables);
    }
    
    g_free(config);
}

/**
 * @brief Validate report configuration
 */
gboolean
report_config_validate(report_config_t *config)
{
    if (!config) {
        report_set_last_error("Report configuration is NULL");
        return FALSE;
    }
    
    if (!config->template_name || strlen(config->template_name) == 0) {
        report_set_last_error("Template name not specified");
        return FALSE;
    }
    
    if (!report_template_exists(config->template_name)) {
        report_set_last_error("Specified template does not exist");
        return FALSE;
    }
    
    if (!config->output_path || strlen(config->output_path) == 0) {
        report_set_last_error("Output path not specified");
        return FALSE;
    }
    
    if (!config->company_name || strlen(config->company_name) == 0) {
        report_set_last_error("Company name not specified");
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Generate main vulnerability report
 */
report_result_t *
report_generate(vulnerability_score_t **vulnerabilities, 
                guint vulnerability_count,
                report_config_t *config)
{
    if (!g_report_api_initialized) {
        report_set_last_error("Report API not initialized");
        return NULL;
    }
    
    if (!vulnerabilities || vulnerability_count == 0) {
        report_set_last_error("No vulnerability data provided");
        return NULL;
    }
    
    if (!report_config_validate(config)) {
        return NULL;
    }
    
    clock_t start_time = clock();
    report_result_t *result = report_result_new();
    
    // Load template
    report_template_t *template = template_load(config->template_name);
    if (!template) {
        result->status = REPORT_STATUS_TEMPLATE_ERROR;
        result->error_message = g_strdup("Failed to load template");
        goto cleanup;
    }
    
    // Prepare template variables
    latex_variables_t *vars = report_prepare_variables(vulnerabilities, 
                                                      vulnerability_count, 
                                                      config);
    if (!vars) {
        result->status = REPORT_STATUS_DATA_ERROR;
        result->error_message = g_strdup("Failed to prepare template variables");
        goto cleanup;
    }
    
    // Process template
    gchar *processed_content = template_process(template, vars);
    if (!processed_content) {
        result->status = REPORT_STATUS_TEMPLATE_ERROR;
        result->error_message = g_strdup("Failed to process template");
        latex_variables_free(vars);
        goto cleanup;
    }
    
    // Compile LaTeX
    latex_result_t *latex_result = latex_compile_content(processed_content, 
                                                        config->output_path);
    if (!latex_result_is_success(latex_result)) {
        result->status = REPORT_STATUS_COMPILATION_ERROR;
        result->error_message = g_strdup_printf("LaTeX compilation failed: %s",
                                              latex_result->error_message);
        g_free(processed_content);
        latex_variables_free(vars);
        latex_result_free(latex_result);
        goto cleanup;
    }
    
    // Success
    result->status = REPORT_STATUS_SUCCESS;
    result->output_file = g_strdup(latex_result->output_file);
    result->generation_time = ((double)(clock() - start_time)) / CLOCKS_PER_SEC;
    result->template_used = g_strdup(config->template_name);
    
    // Update metrics
    g_report_metrics.total_reports_generated++;
    g_report_metrics.successful_reports++;
    g_report_metrics.average_generation_time = 
        (g_report_metrics.average_generation_time * (g_report_metrics.successful_reports - 1) + 
         result->generation_time) / g_report_metrics.successful_reports;
    g_report_metrics.last_generation_timestamp = time(NULL);
    
    // Cleanup
    g_free(processed_content);
    latex_variables_free(vars);
    latex_result_free(latex_result);

cleanup:
    if (template) {
        template_free(template);
    }
    
    if (result->status != REPORT_STATUS_SUCCESS) {
        g_report_metrics.total_reports_generated++;
        g_report_metrics.failed_reports++;
    }
    
    return result;
}

/**
 * @brief Calculate vulnerability statistics
 */
report_statistics_t *
report_calculate_statistics(vulnerability_score_t **vulnerabilities,
                           guint vulnerability_count)
{
    if (!vulnerabilities || vulnerability_count == 0) {
        return NULL;
    }
    
    report_statistics_t *stats = g_new0(report_statistics_t, 1);
    stats->total_vulnerabilities = vulnerability_count;
    
    gdouble total_cvss = 0.0;
    gdouble max_epss = 0.0;
    GHashTable *cwe_counts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    GHashTable *hosts = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    
    for (guint i = 0; i < vulnerability_count; i++) {
        vulnerability_score_t *vuln = vulnerabilities[i];
        
        // Count by severity (using CVSS v3.1 if available, otherwise v3.0, then v2)
        gdouble cvss_score = 0.0;
        if (vuln->cvss_v3_1) {
            cvss_score = vuln->cvss_v3_1->base_score;
            switch (vuln->cvss_v3_1->severity) {
                case CVSS_SEVERITY_CRITICAL:
                    stats->critical_count++;
                    break;
                case CVSS_SEVERITY_HIGH:
                    stats->high_count++;
                    break;
                case CVSS_SEVERITY_MEDIUM:
                    stats->medium_count++;
                    break;
                case CVSS_SEVERITY_LOW:
                    stats->low_count++;
                    break;
                default:
                    break;
            }
        } else if (vuln->cvss_v3_0) {
            cvss_score = vuln->cvss_v3_0->base_score;
            switch (vuln->cvss_v3_0->severity) {
                case CVSS_SEVERITY_CRITICAL:
                    stats->critical_count++;
                    break;
                case CVSS_SEVERITY_HIGH:
                    stats->high_count++;
                    break;
                case CVSS_SEVERITY_MEDIUM:
                    stats->medium_count++;
                    break;
                case CVSS_SEVERITY_LOW:
                    stats->low_count++;
                    break;
                default:
                    break;
            }
        } else if (vuln->cvss_v2) {
            cvss_score = vuln->cvss_v2->base_score;
            // Map CVSS v2 scores to severity levels
            if (cvss_score >= 9.0) stats->critical_count++;
            else if (cvss_score >= 7.0) stats->high_count++;
            else if (cvss_score >= 4.0) stats->medium_count++;
            else stats->low_count++;
        }
        
        total_cvss += cvss_score;
        
        // Check KEV status
        if (vuln->kev && vuln->kev->is_kev) {
            stats->kev_vulnerabilities++;
        }
        
        // Track highest EPSS score
        if (vuln->epss && vuln->epss->score > max_epss) {
            max_epss = vuln->epss->score;
        }
        
        // Count CWE occurrences
        if (vuln->cwe_ids) {
            for (guint j = 0; vuln->cwe_ids[j]; j++) {
                gpointer count = g_hash_table_lookup(cwe_counts, vuln->cwe_ids[j]);
                g_hash_table_insert(cwe_counts, g_strdup(vuln->cwe_ids[j]), 
                                  GINT_TO_POINTER(GPOINTER_TO_INT(count) + 1));
            }
        }
    }
    
    stats->average_cvss_score = total_cvss / vulnerability_count;
    stats->highest_epss_score = max_epss;
    stats->total_hosts = g_hash_table_size(hosts);
    
    // Find most common CWE
    gchar *most_common_cwe = NULL;
    gint max_cwe_count = 0;
    GHashTableIter iter;
    gpointer key, value;
    
    g_hash_table_iter_init(&iter, cwe_counts);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        gint count = GPOINTER_TO_INT(value);
        if (count > max_cwe_count) {
            max_cwe_count = count;
            most_common_cwe = (gchar *)key;
        }
    }
    
    if (most_common_cwe) {
        stats->most_common_cwe = g_strdup(most_common_cwe);
    }
    
    g_hash_table_destroy(cwe_counts);
    g_hash_table_destroy(hosts);
    
    return stats;
}

/**
 * @brief Create new report result
 */
report_result_t *
report_result_new(void)
{
    report_result_t *result = g_new0(report_result_t, 1);
    result->status = REPORT_STATUS_SUCCESS;
    result->metadata = json_object_new();
    return result;
}

/**
 * @brief Free report result
 */
void
report_result_free(report_result_t *result)
{
    if (!result) return;
    
    g_free(result->output_file);
    g_free(result->error_message);
    g_free(result->template_used);
    
    if (result->metadata) {
        json_object_unref(result->metadata);
    }
    
    g_free(result);
}

/**
 * @brief Check if report generation was successful
 */
gboolean
report_result_is_success(report_result_t *result)
{
    return result && result->status == REPORT_STATUS_SUCCESS;
}

/**
 * @brief Convert report status to string
 */
const gchar *
report_status_to_string(report_status_t status)
{
    switch (status) {
        case REPORT_STATUS_SUCCESS:
            return "Success";
        case REPORT_STATUS_TEMPLATE_ERROR:
            return "Template Error";
        case REPORT_STATUS_DATA_ERROR:
            return "Data Error";
        case REPORT_STATUS_COMPILATION_ERROR:
            return "Compilation Error";
        case REPORT_STATUS_IO_ERROR:
            return "I/O Error";
        case REPORT_STATUS_VALIDATION_ERROR:
            return "Validation Error";
        default:
            return "Unknown Status";
    }
}

/* Internal helper functions */

/**
 * @brief Set last error message
 */
static void
report_set_last_error(const gchar *error)
{
    g_free(g_last_error);
    g_last_error = g_strdup(error);
    
    if (g_error_callback) {
        g_error_callback(error, g_error_callback_data);
    }
    
    g_warning("Report API Error: %s", error);
}

/**
 * @brief Prepare template variables from vulnerability data
 */
static latex_variables_t *
report_prepare_variables(vulnerability_score_t **vulnerabilities,
                        guint vulnerability_count,
                        report_config_t *config)
{
    latex_variables_t *vars = latex_variables_new();
    
    // Calculate statistics
    report_statistics_t *stats = report_calculate_statistics(vulnerabilities, vulnerability_count);
    if (!stats) {
        latex_variables_free(vars);
        return NULL;
    }
    
    // Set basic variables
    latex_variables_set(vars, "company_name", config->company_name);
    latex_variables_set(vars, "client_name", config->client_name);
    latex_variables_set(vars, "report_title", config->report_title);
    
    // Set current date if not specified
    GDateTime *now = g_date_time_new_now_local();
    gchar *date_str = g_date_time_format(now, "%B %d, %Y");
    latex_variables_set(vars, "report_date", date_str);
    g_free(date_str);
    g_date_time_unref(now);
    
    // Set statistics
    gchar *temp_str;
    
    temp_str = g_strdup_printf("%u", stats->total_vulnerabilities);
    latex_variables_set(vars, "total_vulnerabilities", temp_str);
    g_free(temp_str);
    
    temp_str = g_strdup_printf("%u", stats->total_hosts);
    latex_variables_set(vars, "total_hosts", temp_str);
    g_free(temp_str);
    
    temp_str = g_strdup_printf("%u", stats->critical_count);
    latex_variables_set(vars, "critical_count", temp_str);
    g_free(temp_str);
    
    temp_str = g_strdup_printf("%u", stats->high_count);
    latex_variables_set(vars, "high_count", temp_str);
    g_free(temp_str);
    
    temp_str = g_strdup_printf("%u", stats->medium_count);
    latex_variables_set(vars, "medium_count", temp_str);
    g_free(temp_str);
    
    temp_str = g_strdup_printf("%u", stats->low_count);
    latex_variables_set(vars, "low_count", temp_str);
    g_free(temp_str);
    
    // Set company logo path
    if (config->company_logo) {
        latex_variables_set(vars, "company_logo", config->company_logo);
    } else {
        latex_variables_set(vars, "company_logo", "assets/images/default_logo.png");
    }
    
    // Generate top vulnerabilities table
    gchar *vuln_table = report_generate_vulnerability_table(vulnerabilities, 
                                                           vulnerability_count, 10);
    latex_variables_set(vars, "top_vulnerabilities_table", vuln_table);
    g_free(vuln_table);
    
    // Add custom variables from config
    if (config->custom_variables) {
        JsonObjectIter iter;
        const gchar *key;
        JsonNode *value_node;
        
        json_object_iter_init(&iter, config->custom_variables);
        while (json_object_iter_next(&iter, &key, &value_node)) {
            if (JSON_NODE_HOLDS_VALUE(value_node)) {
                const gchar *value = json_node_get_string(value_node);
                if (value) {
                    latex_variables_set(vars, key, value);
                }
            }
        }
    }
    
    report_statistics_free(stats);
    return vars;
}

/**
 * @brief Generate LaTeX table for top vulnerabilities
 */
static gchar *
report_generate_vulnerability_table(vulnerability_score_t **vulnerabilities,
                                   guint vulnerability_count,
                                   guint max_entries)
{
    if (!vulnerabilities || vulnerability_count == 0) {
        return g_strdup("No vulnerabilities found & N/A & N/A & System appears secure \\\\\\hline");
    }
    
    GString *table = g_string_new("");
    guint entries_added = 0;
    
    // Sort vulnerabilities by severity (this is simplified - in real implementation,
    // we would sort by CVSS score or AI risk score)
    for (guint i = 0; i < vulnerability_count && entries_added < max_entries; i++) {
        vulnerability_score_t *vuln = vulnerabilities[i];
        
        if (!vuln->cve_id) continue;
        
        // Get CVSS score and severity
        gdouble cvss_score = 0.0;
        const gchar *severity = "Unknown";
        
        if (vuln->cvss_v3_1) {
            cvss_score = vuln->cvss_v3_1->base_score;
            severity = cvss_severity_to_string(vuln->cvss_v3_1->severity);
        } else if (vuln->cvss_v3_0) {
            cvss_score = vuln->cvss_v3_0->base_score;
            severity = cvss_severity_to_string(vuln->cvss_v3_0->severity);
        } else if (vuln->cvss_v2) {
            cvss_score = vuln->cvss_v2->base_score;
            if (cvss_score >= 9.0) severity = "Critical";
            else if (cvss_score >= 7.0) severity = "High";
            else if (cvss_score >= 4.0) severity = "Medium";
            else severity = "Low";
        }
        
        // Escape and truncate description
        gchar *escaped_desc = latex_escape_text(vuln->description ? vuln->description : "No description available");
        if (strlen(escaped_desc) > 100) {
            escaped_desc[97] = '.';
            escaped_desc[98] = '.';
            escaped_desc[99] = '.';
            escaped_desc[100] = '\0';
        }
        
        g_string_append_printf(table, "%s & %s & %.1f & %s \\\\\\hline\n",
                              vuln->cve_id,
                              severity,
                              cvss_score,
                              escaped_desc);
        
        g_free(escaped_desc);
        entries_added++;
    }
    
    if (entries_added == 0) {
        g_string_append(table, "No vulnerabilities found & N/A & N/A & System appears secure \\\\\\hline");
    }
    
    return g_string_free(table, FALSE);
}

/**
 * @brief Free report statistics
 */
void
report_statistics_free(report_statistics_t *stats)
{
    if (!stats) return;
    
    g_free(stats->most_common_cwe);
    g_free(stats);
}