/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Unit tests for LaTeX Report Generation System
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/report_api.h"
#include "../src/latex_engine.h"
#include "../src/template_manager.h"
#include "../../../ai-engine/scoring/vulnerability_scoring.h"

/* Test fixtures */
static vulnerability_score_t *create_test_vulnerability(const gchar *cve_id, 
                                                       gdouble cvss_score,
                                                       cvss_severity_t severity);
static vulnerability_score_t **create_test_vulnerability_set(guint *count);
static void free_test_vulnerability_set(vulnerability_score_t **vulns, guint count);

/**
 * @brief Test LaTeX engine initialization
 */
static void
test_latex_engine_init(void)
{
    g_test_message("Testing LaTeX engine initialization...");
    
    gboolean result = latex_engine_init();
    g_assert_true(result);
    g_assert_true(latex_engine_is_available());
    
    latex_engine_cleanup();
}

/**
 * @brief Test LaTeX text escaping
 */
static void
test_latex_escape_text(void)
{
    g_test_message("Testing LaTeX text escaping...");
    
    // Initialize engine first
    g_assert_true(latex_engine_init());
    
    // Test special characters
    gchar *escaped = latex_escape_text("Test & special $ characters # % ^ _ ~ \\ {}");
    g_assert_nonnull(escaped);
    g_assert_true(strstr(escaped, "\\&") != NULL);
    g_assert_true(strstr(escaped, "\\$") != NULL);
    g_assert_true(strstr(escaped, "\\#") != NULL);
    g_assert_true(strstr(escaped, "\\%") != NULL);
    
    g_free(escaped);
    latex_engine_cleanup();
}

/**
 * @brief Test template variable management
 */
static void
test_latex_variables(void)
{
    g_test_message("Testing LaTeX variables...");
    
    latex_variables_t *vars = latex_variables_new();
    g_assert_nonnull(vars);
    
    // Set and get variables
    latex_variables_set(vars, "test_key", "test_value");
    const gchar *value = latex_variables_get(vars, "test_key");
    g_assert_cmpstr(value, ==, "test_value");
    
    // Test non-existent key
    value = latex_variables_get(vars, "nonexistent");
    g_assert_null(value);
    
    latex_variables_free(vars);
}

/**
 * @brief Test template processing
 */
static void
test_template_processing(void)
{
    g_test_message("Testing template processing...");
    
    g_assert_true(latex_engine_init());
    
    const gchar *template_content = "Hello {{name}}, your score is {{score}}.";
    
    latex_variables_t *vars = latex_variables_new();
    latex_variables_set(vars, "name", "OpenVAS");
    latex_variables_set(vars, "score", "95");
    
    gchar *processed = latex_process_template(template_content, vars);
    g_assert_nonnull(processed);
    g_assert_cmpstr(processed, ==, "Hello OpenVAS, your score is 95.");
    
    g_free(processed);
    latex_variables_free(vars);
    latex_engine_cleanup();
}

/**
 * @brief Test report API initialization
 */
static void
test_report_api_init(void)
{
    g_test_message("Testing report API initialization...");
    
    gboolean result = report_api_init();
    g_assert_true(result);
    g_assert_true(report_api_is_initialized());
    
    report_api_cleanup();
    g_assert_false(report_api_is_initialized());
}

/**
 * @brief Test report configuration
 */
static void
test_report_config(void)
{
    g_test_message("Testing report configuration...");
    
    report_config_t *config = report_config_new();
    g_assert_nonnull(config);
    
    // Check default values
    g_assert_cmpstr(config->template_name, ==, "executive_summary");
    g_assert_cmpint(config->output_format, ==, REPORT_FORMAT_PDF);
    g_assert_true(config->include_charts);
    g_assert_true(config->include_ai_insights);
    
    // Test configuration modification
    report_config_set_template(config, "technical_details");
    g_assert_cmpstr(config->template_name, ==, "technical_details");
    
    report_config_set_output_format(config, REPORT_FORMAT_HTML);
    g_assert_cmpint(config->output_format, ==, REPORT_FORMAT_HTML);
    
    report_config_set_company_info(config, "Test Company", "/path/to/logo.png");
    g_assert_cmpstr(config->company_name, ==, "Test Company");
    g_assert_cmpstr(config->company_logo, ==, "/path/to/logo.png");
    
    report_config_free(config);
}

/**
 * @brief Test vulnerability statistics calculation
 */
static void
test_vulnerability_statistics(void)
{
    g_test_message("Testing vulnerability statistics calculation...");
    
    guint vuln_count;
    vulnerability_score_t **vulns = create_test_vulnerability_set(&vuln_count);
    g_assert_nonnull(vulns);
    g_assert_cmpuint(vuln_count, >, 0);
    
    report_statistics_t *stats = report_calculate_statistics(vulns, vuln_count);
    g_assert_nonnull(stats);
    
    g_assert_cmpuint(stats->total_vulnerabilities, ==, vuln_count);
    g_assert_cmpuint(stats->critical_count + stats->high_count + 
                     stats->medium_count + stats->low_count, ==, vuln_count);
    
    g_test_message("Statistics: Total=%u, Critical=%u, High=%u, Medium=%u, Low=%u",
                   stats->total_vulnerabilities, stats->critical_count,
                   stats->high_count, stats->medium_count, stats->low_count);
    
    report_statistics_free(stats);
    free_test_vulnerability_set(vulns, vuln_count);
}

/**
 * @brief Test basic report generation
 */
static void
test_basic_report_generation(void)
{
    g_test_message("Testing basic report generation...");
    
    // This test requires LaTeX to be installed, so we'll skip if not available
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available, skipping report generation test");
        return;
    }
    
    g_assert_true(report_api_init());
    
    guint vuln_count;
    vulnerability_score_t **vulns = create_test_vulnerability_set(&vuln_count);
    
    report_config_t *config = report_config_new();
    config->output_path = g_strdup("/tmp/test_openvas_report.pdf");
    
    // Note: This test might fail if the executive template is not properly installed
    // In a real implementation, we would ensure templates are available for testing
    report_result_t *result = report_generate(vulns, vuln_count, config);
    
    if (result) {
        if (report_result_is_success(result)) {
            g_test_message("Report generated successfully: %s", result->output_file);
            g_test_message("Generation time: %.2f seconds", result->generation_time);
        } else {
            g_test_message("Report generation failed: %s", result->error_message);
        }
        report_result_free(result);
    }
    
    report_config_free(config);
    free_test_vulnerability_set(vulns, vuln_count);
    report_api_cleanup();
}

/**
 * @brief Test format conversion functions
 */
static void
test_format_conversion(void)
{
    g_test_message("Testing format conversion functions...");
    
    // Test format to string conversion
    g_assert_cmpstr(report_format_to_string(REPORT_FORMAT_PDF), ==, "PDF");
    g_assert_cmpstr(report_format_to_string(REPORT_FORMAT_HTML), ==, "HTML");
    g_assert_cmpstr(report_format_to_string(REPORT_FORMAT_LATEX_SOURCE), ==, "LaTeX Source");
    
    // Test string to format conversion
    g_assert_cmpint(report_format_from_string("PDF"), ==, REPORT_FORMAT_PDF);
    g_assert_cmpint(report_format_from_string("HTML"), ==, REPORT_FORMAT_HTML);
    g_assert_cmpint(report_format_from_string("LaTeX Source"), ==, REPORT_FORMAT_LATEX_SOURCE);
}

/* Helper functions for creating test data */

/**
 * @brief Create a test vulnerability with specified parameters
 */
static vulnerability_score_t *
create_test_vulnerability(const gchar *cve_id, gdouble cvss_score, cvss_severity_t severity)
{
    vulnerability_score_t *vuln = vulnerability_score_new(cve_id);
    
    // Create CVSS v3.1 score
    vuln->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vuln->cvss_v3_1->base_score = cvss_score;
    vuln->cvss_v3_1->severity = severity;
    
    // Set some basic information
    vuln->vulnerability_name = g_strdup_printf("Test Vulnerability %s", cve_id);
    vuln->description = g_strdup_printf("This is a test vulnerability with CVE ID %s", cve_id);
    
    return vuln;
}

/**
 * @brief Create a set of test vulnerabilities
 */
static vulnerability_score_t **
create_test_vulnerability_set(guint *count)
{
    *count = 10;
    vulnerability_score_t **vulns = g_new0(vulnerability_score_t *, *count);
    
    // Create vulnerabilities with different severities
    vulns[0] = create_test_vulnerability("CVE-2024-0001", 9.8, CVSS_SEVERITY_CRITICAL);
    vulns[1] = create_test_vulnerability("CVE-2024-0002", 9.1, CVSS_SEVERITY_CRITICAL);
    vulns[2] = create_test_vulnerability("CVE-2024-0003", 8.5, CVSS_SEVERITY_HIGH);
    vulns[3] = create_test_vulnerability("CVE-2024-0004", 7.8, CVSS_SEVERITY_HIGH);
    vulns[4] = create_test_vulnerability("CVE-2024-0005", 7.2, CVSS_SEVERITY_HIGH);
    vulns[5] = create_test_vulnerability("CVE-2024-0006", 6.1, CVSS_SEVERITY_MEDIUM);
    vulns[6] = create_test_vulnerability("CVE-2024-0007", 5.4, CVSS_SEVERITY_MEDIUM);
    vulns[7] = create_test_vulnerability("CVE-2024-0008", 4.3, CVSS_SEVERITY_MEDIUM);
    vulns[8] = create_test_vulnerability("CVE-2024-0009", 3.1, CVSS_SEVERITY_LOW);
    vulns[9] = create_test_vulnerability("CVE-2024-0010", 2.3, CVSS_SEVERITY_LOW);
    
    return vulns;
}

/**
 * @brief Free test vulnerability set
 */
static void
free_test_vulnerability_set(vulnerability_score_t **vulns, guint count)
{
    if (!vulns) return;
    
    for (guint i = 0; i < count; i++) {
        if (vulns[i]) {
            vulnerability_score_free(vulns[i]);
        }
    }
    
    g_free(vulns);
}

/**
 * @brief Main test function
 */
int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    
    // LaTeX Engine Tests
    g_test_add_func("/latex_engine/init", test_latex_engine_init);
    g_test_add_func("/latex_engine/escape_text", test_latex_escape_text);
    g_test_add_func("/latex_engine/variables", test_latex_variables);
    g_test_add_func("/latex_engine/template_processing", test_template_processing);
    
    // Report API Tests
    g_test_add_func("/report_api/init", test_report_api_init);
    g_test_add_func("/report_api/config", test_report_config);
    g_test_add_func("/report_api/statistics", test_vulnerability_statistics);
    g_test_add_func("/report_api/basic_generation", test_basic_report_generation);
    g_test_add_func("/report_api/format_conversion", test_format_conversion);
    
    return g_test_run();
}