/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Professional LaTeX Report Generation API
 * Public interface for generating professional vulnerability reports
 */

#ifndef REPORT_API_H
#define REPORT_API_H

#include <glib.h>
#include <json-glib/json-glib.h>
#include "../../ai-engine/scoring/vulnerability_scoring.h"

/**
 * @brief Report output formats
 */
typedef enum {
    REPORT_FORMAT_PDF,
    REPORT_FORMAT_HTML,
    REPORT_FORMAT_LATEX_SOURCE,
    REPORT_FORMAT_PRINT_READY
} report_format_t;

/**
 * @brief Report generation status
 */
typedef enum {
    REPORT_STATUS_SUCCESS,
    REPORT_STATUS_TEMPLATE_ERROR,
    REPORT_STATUS_DATA_ERROR,
    REPORT_STATUS_COMPILATION_ERROR,
    REPORT_STATUS_IO_ERROR,
    REPORT_STATUS_VALIDATION_ERROR
} report_status_t;

/**
 * @brief Report configuration
 */
typedef struct {
    gchar *template_name;           // Template to use
    report_format_t output_format;  // Output format
    gchar *output_path;            // Output file path
    gchar *company_name;           // Company name
    gchar *company_logo;           // Path to company logo
    gchar *client_name;            // Client organization name
    gchar *report_title;           // Report title
    gboolean include_charts;       // Include charts and graphs
    gboolean include_ai_insights;  // Include AI-generated insights
    gchar *language;               // Report language (ISO 639-1)
    JsonObject *custom_variables;  // Additional template variables
} report_config_t;

/**
 * @brief Report generation result
 */
typedef struct {
    report_status_t status;
    gchar *output_file;            // Path to generated report
    gchar *error_message;          // Error message if failed
    gdouble generation_time;       // Time taken in seconds
    guint total_pages;             // Number of pages generated
    gchar *template_used;          // Template that was used
    JsonObject *metadata;         // Additional metadata
} report_result_t;

/**
 * @brief Report statistics
 */
typedef struct {
    guint total_vulnerabilities;
    guint critical_count;
    guint high_count;
    guint medium_count;
    guint low_count;
    guint total_hosts;
    gdouble average_cvss_score;
    guint kev_vulnerabilities;
    gdouble highest_epss_score;
    gchar *most_common_cwe;
} report_statistics_t;

/* Core Report Generation Functions */
gboolean report_api_init(void);
void report_api_cleanup(void);
gboolean report_api_is_initialized(void);

/* Report Configuration */
report_config_t *report_config_new(void);
void report_config_free(report_config_t *config);
gboolean report_config_validate(report_config_t *config);
void report_config_set_template(report_config_t *config, const gchar *template_name);
void report_config_set_output_format(report_config_t *config, report_format_t format);
void report_config_set_company_info(report_config_t *config, const gchar *name, const gchar *logo);
void report_config_add_variable(report_config_t *config, const gchar *key, const gchar *value);

/* Main Report Generation */
report_result_t *report_generate(vulnerability_score_t **vulnerabilities, 
                                guint vulnerability_count,
                                report_config_t *config);

report_result_t *report_generate_executive_summary(vulnerability_score_t **vulnerabilities,
                                                  guint vulnerability_count,
                                                  report_config_t *config);

report_result_t *report_generate_technical_details(vulnerability_score_t **vulnerabilities,
                                                  guint vulnerability_count,
                                                  report_config_t *config);

report_result_t *report_generate_compliance_report(vulnerability_score_t **vulnerabilities,
                                                  guint vulnerability_count,
                                                  const gchar *framework,
                                                  report_config_t *config);

/* Batch Report Generation */
report_result_t **report_generate_batch(vulnerability_score_t **vulnerabilities,
                                       guint vulnerability_count,
                                       report_config_t **configs,
                                       guint config_count);

/* Report Statistics */
report_statistics_t *report_calculate_statistics(vulnerability_score_t **vulnerabilities,
                                                guint vulnerability_count);
void report_statistics_free(report_statistics_t *stats);
gchar *report_statistics_to_json(report_statistics_t *stats);

/* AI-Enhanced Report Generation */
report_result_t *report_generate_with_ai_insights(vulnerability_score_t **vulnerabilities,
                                                 guint vulnerability_count,
                                                 report_config_t *config);

gchar *report_generate_ai_executive_summary(vulnerability_score_t **vulnerabilities,
                                          guint vulnerability_count);

gchar *report_generate_ai_recommendations(vulnerability_score_t **vulnerabilities,
                                        guint vulnerability_count);

GPtrArray *report_generate_ai_prioritized_actions(vulnerability_score_t **vulnerabilities,
                                                 guint vulnerability_count);

/* Report Result Management */
report_result_t *report_result_new(void);
void report_result_free(report_result_t *result);
gboolean report_result_is_success(report_result_t *result);
const gchar *report_status_to_string(report_status_t status);

/* Template Management Integration */
GPtrArray *report_list_available_templates(void);
gboolean report_template_exists(const gchar *template_name);
JsonObject *report_get_template_info(const gchar *template_name);
GPtrArray *report_get_template_variables(const gchar *template_name);

/* Report Customization */
gboolean report_customize_colors(report_config_t *config, 
                                const gchar *primary_color,
                                const gchar *secondary_color);

gboolean report_customize_fonts(report_config_t *config,
                               const gchar *main_font,
                               const gchar *mono_font);

gboolean report_add_custom_section(report_config_t *config,
                                  const gchar *section_name,
                                  const gchar *section_content);

/* Multi-format Output */
gboolean report_convert_to_html(const gchar *pdf_path, const gchar *html_path);
gboolean report_convert_to_docx(const gchar *pdf_path, const gchar *docx_path);
gboolean report_optimize_for_print(const gchar *pdf_path, const gchar *print_path);

/* Report Validation */
gboolean report_validate_data(vulnerability_score_t **vulnerabilities, guint count);
gboolean report_validate_template_compatibility(const gchar *template_name, 
                                               vulnerability_score_t **vulnerabilities,
                                               guint count);

/* Performance and Monitoring */
typedef struct {
    guint64 total_reports_generated;
    guint64 successful_reports;
    guint64 failed_reports;
    gdouble average_generation_time;
    gchar *most_used_template;
    gint64 last_generation_timestamp;
} report_metrics_t;

report_metrics_t *report_get_metrics(void);
void report_reset_metrics(void);
gchar *report_metrics_to_json(report_metrics_t *metrics);

/* Utility Functions */
const gchar *report_format_to_string(report_format_t format);
report_format_t report_format_from_string(const gchar *format_str);
gchar *report_generate_filename(report_config_t *config);
gboolean report_is_format_supported(report_format_t format);

/* Error Handling */
void report_set_error_callback(void (*callback)(const gchar *error, gpointer user_data), 
                              gpointer user_data);
gchar *report_get_last_error(void);
void report_clear_last_error(void);

#endif /* REPORT_API_H */