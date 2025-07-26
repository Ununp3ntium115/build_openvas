/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * LaTeX Professional Reporting System Demo
 * Demonstrates the new professional report generation capabilities
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../plugins/report-generation/src/report_api.h"
#include "../ai-engine/scoring/vulnerability_scoring.h"

/**
 * @brief Create sample vulnerability data for demonstration
 */
static vulnerability_score_t **
create_sample_vulnerabilities(guint *count)
{
    *count = 15;
    vulnerability_score_t **vulns = g_new0(vulnerability_score_t *, *count);
    
    // Critical vulnerabilities
    vulns[0] = vulnerability_score_new("CVE-2024-1001");
    vulns[0]->vulnerability_name = g_strdup("Remote Code Execution in Web Server");
    vulns[0]->description = g_strdup("A critical buffer overflow vulnerability allows remote attackers to execute arbitrary code with system privileges.");
    vulns[0]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vulns[0]->cvss_v3_1->base_score = 9.8;
    vulns[0]->cvss_v3_1->severity = CVSS_SEVERITY_CRITICAL;
    
    vulns[1] = vulnerability_score_new("CVE-2024-1002");
    vulns[1]->vulnerability_name = g_strdup("SQL Injection in Authentication System");
    vulns[1]->description = g_strdup("SQL injection vulnerability in login form allows authentication bypass and data extraction.");
    vulns[1]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vulns[1]->cvss_v3_1->base_score = 9.1;
    vulns[1]->cvss_v3_1->severity = CVSS_SEVERITY_CRITICAL;
    
    // High severity vulnerabilities
    vulns[2] = vulnerability_score_new("CVE-2024-1003");
    vulns[2]->vulnerability_name = g_strdup("Cross-Site Scripting (XSS) in User Input");
    vulns[2]->description = g_strdup("Stored XSS vulnerability allows attackers to inject malicious scripts into web pages.");
    vulns[2]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vulns[2]->cvss_v3_1->base_score = 8.5;
    vulns[2]->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
    
    vulns[3] = vulnerability_score_new("CVE-2024-1004");
    vulns[3]->vulnerability_name = g_strdup("Privilege Escalation in System Service");
    vulns[3]->description = g_strdup("Local privilege escalation vulnerability allows standard users to gain administrative access.");
    vulns[3]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vulns[3]->cvss_v3_1->base_score = 7.8;
    vulns[3]->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
    
    vulns[4] = vulnerability_score_new("CVE-2024-1005");
    vulns[4]->vulnerability_name = g_strdup("Directory Traversal in File Upload");
    vulns[4]->description = g_strdup("Path traversal vulnerability allows attackers to access files outside the intended directory.");
    vulns[4]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vulns[4]->cvss_v3_1->base_score = 7.5;
    vulns[4]->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
    
    // Medium severity vulnerabilities
    for (int i = 5; i < 10; i++) {
        vulns[i] = vulnerability_score_new(g_strdup_printf("CVE-2024-10%02d", i));
        vulns[i]->vulnerability_name = g_strdup_printf("Medium Severity Issue #%d", i - 4);
        vulns[i]->description = g_strdup("Medium severity vulnerability requiring attention within 90 days.");
        vulns[i]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
        vulns[i]->cvss_v3_1->base_score = 4.0 + (i - 5) * 0.5;
        vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_MEDIUM;
    }
    
    // Low severity vulnerabilities
    for (int i = 10; i < 15; i++) {
        vulns[i] = vulnerability_score_new(g_strdup_printf("CVE-2024-20%02d", i));
        vulns[i]->vulnerability_name = g_strdup_printf("Low Severity Issue #%d", i - 9);
        vulns[i]->description = g_strdup("Low severity vulnerability for long-term remediation planning.");
        vulns[i]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
        vulns[i]->cvss_v3_1->base_score = 1.0 + (i - 10) * 0.5;
        vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_LOW;
    }
    
    return vulns;
}

/**
 * @brief Free sample vulnerability data
 */
static void
free_sample_vulnerabilities(vulnerability_score_t **vulns, guint count)
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
 * @brief Demonstrate executive summary report generation
 */
static void
demonstrate_executive_report(vulnerability_score_t **vulns, guint count)
{
    g_print("\n📊 Generating Executive Summary Report\n");
    g_print("=" * 50 "\n");
    
    // Create report configuration
    report_config_t *config = report_config_new();
    
    // Customize for executive audience
    g_free(config->template_name);
    config->template_name = g_strdup("executive_summary");
    
    g_free(config->company_name);
    config->company_name = g_strdup("OpenVAS Security Solutions");
    
    g_free(config->client_name);
    config->client_name = g_strdup("Acme Corporation");
    
    g_free(config->report_title);
    config->report_title = g_strdup("Quarterly Security Assessment Report");
    
    g_free(config->output_path);
    config->output_path = g_strdup("/tmp/executive_summary_report.pdf");
    
    // Add custom variables
    report_config_add_variable(config, "assessment_period", "January 1-31, 2025");
    report_config_add_variable(config, "company_address", "123 Security Street, Cyber City, CC 12345");
    report_config_add_variable(config, "company_contact", "Phone: +1-555-SECURE | Email: info@openvas-security.com");
    report_config_add_variable(config, "technical_lead_name", "Dr. Sarah Johnson");
    report_config_add_variable(config, "technical_lead_email", "sarah.johnson@openvas-security.com");
    report_config_add_variable(config, "account_manager_name", "Michael Chen");
    report_config_add_variable(config, "account_manager_email", "michael.chen@openvas-security.com");
    
    // Generate the report
    report_result_t *result = report_generate_executive_summary(vulns, count, config);
    
    if (result && report_result_is_success(result)) {
        g_print("✅ Executive report generated successfully!\n");
        g_print("   📄 Output file: %s\n", result->output_file);
        g_print("   ⏱️  Generation time: %.2f seconds\n", result->generation_time);
        g_print("   📊 Template used: %s\n", result->template_used);
        
        if (result->total_pages > 0) {
            g_print("   📖 Total pages: %u\n", result->total_pages);
        }
    } else {
        g_print("❌ Executive report generation failed\n");
        if (result && result->error_message) {
            g_print("   Error: %s\n", result->error_message);
        }
    }
    
    if (result) {
        report_result_free(result);
    }
    report_config_free(config);
}

/**
 * @brief Demonstrate technical detailed report generation
 */
static void
demonstrate_technical_report(vulnerability_score_t **vulns, guint count)
{
    g_print("\n🔧 Generating Technical Detailed Report\n");
    g_print("=" * 50 "\n");
    
    // Create report configuration for technical audience
    report_config_t *config = report_config_new();
    
    g_free(config->template_name);
    config->template_name = g_strdup("technical_details");
    
    g_free(config->company_name);
    config->company_name = g_strdup("OpenVAS Security Solutions");
    
    g_free(config->client_name);
    config->client_name = g_strdup("Acme Corporation - IT Department");
    
    g_free(config->report_title);
    config->report_title = g_strdup("Technical Vulnerability Assessment Report");
    
    g_free(config->output_path);
    config->output_path = g_strdup("/tmp/technical_detailed_report.pdf");
    
    // Enable all technical features
    config->include_charts = TRUE;
    config->include_ai_insights = TRUE;
    
    // Generate the report
    report_result_t *result = report_generate_technical_details(vulns, count, config);
    
    if (result && report_result_is_success(result)) {
        g_print("✅ Technical report generated successfully!\n");
        g_print("   📄 Output file: %s\n", result->output_file);
        g_print("   ⏱️  Generation time: %.2f seconds\n", result->generation_time);
        g_print("   📊 Template used: %s\n", result->template_used);
    } else {
        g_print("❌ Technical report generation failed\n");
        if (result && result->error_message) {
            g_print("   Error: %s\n", result->error_message);
        }
    }
    
    if (result) {
        report_result_free(result);
    }
    report_config_free(config);
}

/**
 * @brief Demonstrate compliance report generation
 */
static void
demonstrate_compliance_report(vulnerability_score_t **vulns, guint count)
{
    g_print("\n📋 Generating Compliance Report (SOC2)\n");
    g_print("=" * 50 "\n");
    
    // Create report configuration for compliance
    report_config_t *config = report_config_new();
    
    g_free(config->company_name);
    config->company_name = g_strdup("OpenVAS Security Solutions");
    
    g_free(config->client_name);
    config->client_name = g_strdup("Acme Corporation - Compliance Team");
    
    g_free(config->report_title);
    config->report_title = g_strdup("SOC2 Type II Vulnerability Assessment Report");
    
    g_free(config->output_path);
    config->output_path = g_strdup("/tmp/soc2_compliance_report.pdf");
    
    // Generate compliance-specific report
    report_result_t *result = report_generate_compliance_report(vulns, count, "SOC2", config);
    
    if (result && report_result_is_success(result)) {
        g_print("✅ Compliance report generated successfully!\n");
        g_print("   📄 Output file: %s\n", result->output_file);
        g_print("   ⏱️  Generation time: %.2f seconds\n", result->generation_time);
        g_print("   📊 Template used: %s\n", result->template_used);
    } else {
        g_print("❌ Compliance report generation failed\n");
        if (result && result->error_message) {
            g_print("   Error: %s\n", result->error_message);
        }
    }
    
    if (result) {
        report_result_free(result);
    }
    report_config_free(config);
}

/**
 * @brief Demonstrate AI-enhanced reporting
 */
static void
demonstrate_ai_enhanced_reporting(vulnerability_score_t **vulns, guint count)
{
    g_print("\n🤖 Generating AI-Enhanced Report\n");
    g_print("=" * 50 "\n");
    
    // Create report configuration with AI features
    report_config_t *config = report_config_new();
    
    g_free(config->template_name);
    config->template_name = g_strdup("executive_summary");
    
    g_free(config->company_name);
    config->company_name = g_strdup("OpenVAS AI-Enhanced Security");
    
    g_free(config->client_name);
    config->client_name = g_strdup("Acme Corporation");
    
    g_free(config->report_title);
    config->report_title = g_strdup("AI-Enhanced Vulnerability Assessment Report");
    
    g_free(config->output_path);
    config->output_path = g_strdup("/tmp/ai_enhanced_report.pdf");
    
    // Enable AI features
    config->include_ai_insights = TRUE;
    
    // Generate AI-enhanced report
    report_result_t *result = report_generate_with_ai_insights(vulns, count, config);
    
    if (result && report_result_is_success(result)) {
        g_print("✅ AI-enhanced report generated successfully!\n");
        g_print("   📄 Output file: %s\n", result->output_file);
        g_print("   ⏱️  Generation time: %.2f seconds\n", result->generation_time);
        g_print("   🧠 AI insights included\n");
        
        // Demonstrate AI-generated content
        gchar *ai_summary = report_generate_ai_executive_summary(vulns, count);
        if (ai_summary) {
            g_print("\n🧠 AI-Generated Executive Summary Preview:\n");
            g_print("   %s\n", ai_summary);
            g_free(ai_summary);
        }
        
    } else {
        g_print("❌ AI-enhanced report generation failed\n");
        if (result && result->error_message) {
            g_print("   Error: %s\n", result->error_message);
        }
    }
    
    if (result) {
        report_result_free(result);
    }
    report_config_free(config);
}

/**
 * @brief Display vulnerability statistics
 */
static void
display_vulnerability_statistics(vulnerability_score_t **vulns, guint count)
{
    g_print("\n📈 Vulnerability Assessment Statistics\n");
    g_print("=" * 50 "\n");
    
    report_statistics_t *stats = report_calculate_statistics(vulns, count);
    if (!stats) {
        g_print("❌ Failed to calculate statistics\n");
        return;
    }
    
    g_print("📊 Total Vulnerabilities: %u\n", stats->total_vulnerabilities);
    g_print("🔴 Critical: %u\n", stats->critical_count);
    g_print("🟠 High: %u\n", stats->high_count);
    g_print("🟡 Medium: %u\n", stats->medium_count);
    g_print("🟢 Low: %u\n", stats->low_count);
    g_print("🖥️  Total Hosts: %u\n", stats->total_hosts);
    g_print("📊 Average CVSS Score: %.1f\n", stats->average_cvss_score);
    g_print("⚡ KEV Vulnerabilities: %u\n", stats->kev_vulnerabilities);
    g_print("📈 Highest EPSS Score: %.3f\n", stats->highest_epss_score);
    
    if (stats->most_common_cwe) {
        g_print("🎯 Most Common CWE: %s\n", stats->most_common_cwe);
    }
    
    report_statistics_free(stats);
}

/**
 * @brief Display available templates
 */
static void
display_available_templates(void)
{
    g_print("\n📋 Available Report Templates\n");
    g_print("=" * 50 "\n");
    
    GPtrArray *templates = report_list_available_templates();
    if (!templates) {
        g_print("❌ No templates available\n");
        return;
    }
    
    for (guint i = 0; i < templates->len; i++) {
        const gchar *template_name = g_ptr_array_index(templates, i);
        JsonObject *info = report_get_template_info(template_name);
        
        g_print("📄 %s\n", template_name);
        
        if (info) {
            const gchar *display_name = json_object_get_string_member(info, "display_name");
            const gchar *description = json_object_get_string_member(info, "description");
            
            if (display_name) {
                g_print("   Name: %s\n", display_name);
            }
            if (description) {
                g_print("   Description: %s\n", description);
            }
            
            json_object_unref(info);
        }
        g_print("\n");
    }
    
    g_ptr_array_free(templates, TRUE);
}

/**
 * @brief Main demonstration function
 */
int
main(int argc, char *argv[])
{
    g_print("🚀 OpenVAS Professional LaTeX Reporting System Demo\n");
    g_print("=" * 60 "\n");
    
    // Initialize the report API
    if (!report_api_init()) {
        g_print("❌ Failed to initialize report API\n");
        g_print("   Make sure LaTeX is installed and templates are available\n");
        return 1;
    }
    
    g_print("✅ Report API initialized successfully\n");
    
    // Create sample vulnerability data
    guint vuln_count;
    vulnerability_score_t **vulns = create_sample_vulnerabilities(&vuln_count);
    
    // Display statistics
    display_vulnerability_statistics(vulns, vuln_count);
    
    // Display available templates
    display_available_templates();
    
    // Demonstrate different report types
    demonstrate_executive_report(vulns, vuln_count);
    demonstrate_technical_report(vulns, vuln_count);
    demonstrate_compliance_report(vulns, vuln_count);
    demonstrate_ai_enhanced_reporting(vulns, vuln_count);
    
    // Display final metrics
    g_print("\n📊 Report Generation Metrics\n");
    g_print("=" * 50 "\n");
    
    report_metrics_t *metrics = report_get_metrics();
    if (metrics) {
        g_print("📈 Total Reports Generated: %lu\n", metrics->total_reports_generated);
        g_print("✅ Successful Reports: %lu\n", metrics->successful_reports);
        g_print("❌ Failed Reports: %lu\n", metrics->failed_reports);
        g_print("⏱️  Average Generation Time: %.2f seconds\n", metrics->average_generation_time);
        if (metrics->most_used_template) {
            g_print("📊 Most Used Template: %s\n", metrics->most_used_template);
        }
    }
    
    // Cleanup
    free_sample_vulnerabilities(vulns, vuln_count);
    report_api_cleanup();
    
    g_print("\n🎉 Demo completed successfully!\n");
    g_print("Check the generated PDF reports in /tmp/\n");
    
    return 0;
}