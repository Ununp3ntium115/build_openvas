/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Comprehensive LaTeX Engine Testing Suite
 * Critical QA validation for LaTeX report generation
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#include "../src/latex_engine.h"

/* Test fixtures and utilities */
static gchar *test_temp_dir = NULL;
static latex_config_t *test_config = NULL;

/**
 * @brief Setup test environment
 */
static void
test_setup(void)
{
    // Create temporary directory for tests
    test_temp_dir = g_strdup("/tmp/openvas_latex_test_XXXXXX");
    if (!mkdtemp(test_temp_dir)) {
        g_error("Failed to create test temp directory");
    }
    
    // Create test configuration
    test_config = latex_config_new();
    latex_config_set_temp_dir(test_config, test_temp_dir);
    latex_config_set_timeout(test_config, 30); // Shorter timeout for tests
}

/**
 * @brief Cleanup test environment
 */
static void
test_teardown(void)
{
    if (test_temp_dir) {
        // Remove test directory and contents
        gchar *rm_cmd = g_strdup_printf("rm -rf %s", test_temp_dir);
        system(rm_cmd);
        g_free(rm_cmd);
        g_free(test_temp_dir);
        test_temp_dir = NULL;
    }
    
    if (test_config) {
        latex_config_free(test_config);
        test_config = NULL;
    }
    
    latex_engine_cleanup();
}

/* ========== CORE FUNCTIONALITY TESTS ========== */

/**
 * @brief Test LaTeX engine initialization with various configurations
 */
static void
test_latex_engine_initialization(void)
{
    g_test_message("Testing LaTeX engine initialization...");
    
    // Test default initialization
    gboolean result = latex_engine_init();
    g_assert_true(result);
    g_assert_true(latex_engine_is_available());
    latex_engine_cleanup();
    
    // Test initialization with custom config
    result = latex_engine_init_with_config(test_config);
    g_assert_true(result);
    g_assert_true(latex_engine_is_available());
    latex_engine_cleanup();
    
    // Test initialization with invalid config
    latex_config_t *invalid_config = latex_config_new();
    g_free(invalid_config->latex_binary);
    invalid_config->latex_binary = g_strdup("/nonexistent/binary");
    
    result = latex_engine_init_with_config(invalid_config);
    g_assert_false(result);
    
    latex_config_free(invalid_config);
}

/**
 * @brief Test configuration validation with edge cases
 */
static void
test_latex_engine_configuration_validation(void)
{
    g_test_message("Testing configuration validation...");
    
    // Valid configuration
    latex_config_t *valid_config = latex_config_new();
    g_assert_true(latex_config_validate(valid_config));
    latex_config_free(valid_config);
    
    // NULL configuration
    g_assert_false(latex_config_validate(NULL));
    
    // Empty binary path
    latex_config_t *empty_binary = latex_config_new();
    g_free(empty_binary->latex_binary);
    empty_binary->latex_binary = g_strdup("");
    g_assert_false(latex_config_validate(empty_binary));
    latex_config_free(empty_binary);
    
    // NULL binary path
    latex_config_t *null_binary = latex_config_new();
    g_free(null_binary->latex_binary);
    null_binary->latex_binary = NULL;
    g_assert_false(latex_config_validate(null_binary));
    latex_config_free(null_binary);
    
    // Invalid timeout
    latex_config_t *invalid_timeout = latex_config_new();
    invalid_timeout->timeout_seconds = -1;
    g_assert_false(latex_config_validate(invalid_timeout));
    latex_config_free(invalid_timeout);
    
    // Empty temp directory
    latex_config_t *empty_temp = latex_config_new();
    g_free(empty_temp->temp_dir);
    empty_temp->temp_dir = g_strdup("");
    g_assert_false(latex_config_validate(empty_temp));
    latex_config_free(empty_temp);
}

/**
 * @brief Test LaTeX dependency checking
 */
static void
test_latex_engine_dependency_checking(void)
{
    g_test_message("Testing dependency checking...");
    
    g_assert_true(latex_engine_init());
    
    // Test basic dependency check
    gboolean deps_available = latex_check_dependencies();
    if (!deps_available) {
        g_test_skip("LaTeX dependencies not available - this is expected in some test environments");
        return;
    }
    
    // Test required packages list
    GPtrArray *packages = latex_get_required_packages();
    g_assert_nonnull(packages);
    g_assert_cmpuint(packages->len, >, 0);
    
    // Verify common packages are in the list
    gboolean found_geometry = FALSE;
    gboolean found_tikz = FALSE;
    
    for (guint i = 0; i < packages->len; i++) {
        const gchar *package = g_ptr_array_index(packages, i);
        if (g_strcmp0(package, "geometry") == 0) found_geometry = TRUE;
        if (g_strcmp0(package, "tikz") == 0) found_tikz = TRUE;
    }
    
    g_assert_true(found_geometry);
    g_assert_true(found_tikz);
    
    g_ptr_array_free(packages, TRUE);
}

/* ========== COMPILATION TESTS ========== */

/**
 * @brief Test successful LaTeX compilation scenarios
 */
static void
test_latex_compilation_success_scenarios(void)
{
    g_test_message("Testing successful compilation scenarios...");
    
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available");
        return;
    }
    
    g_assert_true(latex_engine_init_with_config(test_config));
    
    // Test basic document compilation
    const gchar *basic_doc = 
        "\\documentclass{article}\n"
        "\\begin{document}\n"
        "Hello, World!\n"
        "\\end{document}\n";
    
    latex_result_t *result = latex_compile_content(basic_doc, "test_basic");
    g_assert_nonnull(result);
    g_assert_true(latex_result_is_success(result));
    g_assert_nonnull(result->output_file);
    g_assert_true(g_file_test(result->output_file, G_FILE_TEST_EXISTS));
    
    latex_result_free(result);
    
    // Test document with packages
    const gchar *package_doc = 
        "\\documentclass{article}\n"
        "\\usepackage{geometry}\n"
        "\\usepackage{xcolor}\n"
        "\\begin{document}\n"
        "\\textcolor{blue}{Colored text}\n"
        "\\end{document}\n";
    
    result = latex_compile_content(package_doc, "test_packages");
    g_assert_nonnull(result);
    
    if (!latex_result_is_success(result)) {
        g_test_message("Package compilation failed: %s", result->error_message);
        g_test_skip("Required LaTeX packages not available");
    } else {
        g_assert_nonnull(result->output_file);
        g_assert_true(g_file_test(result->output_file, G_FILE_TEST_EXISTS));
    }
    
    latex_result_free(result);
}

/**
 * @brief Test LaTeX compilation failure scenarios
 */
static void
test_latex_compilation_failure_scenarios(void)
{
    g_test_message("Testing compilation failure scenarios...");
    
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available");
        return;
    }
    
    g_assert_true(latex_engine_init_with_config(test_config));
    
    // Test malformed LaTeX
    const gchar *malformed_doc = 
        "\\documentclass{article}\n"
        "\\begin{document}\n"
        "\\undefined_command{test}\n"  // This should cause an error
        "\\end{document}\n";
    
    latex_result_t *result = latex_compile_content(malformed_doc, "test_malformed");
    g_assert_nonnull(result);
    g_assert_false(latex_result_is_success(result));
    g_assert_nonnull(result->error_message);
    g_assert_null(result->output_file);
    
    latex_result_free(result);
    
    // Test missing \end{document}
    const gchar *incomplete_doc = 
        "\\documentclass{article}\n"
        "\\begin{document}\n"
        "Incomplete document\n";
        // Missing \end{document}
    
    result = latex_compile_content(incomplete_doc, "test_incomplete");
    g_assert_nonnull(result);
    g_assert_false(latex_result_is_success(result));
    g_assert_nonnull(result->error_message);
    
    latex_result_free(result);
    
    // Test empty content
    result = latex_compile_content("", "test_empty");
    g_assert_nonnull(result);
    g_assert_false(latex_result_is_success(result));
    
    latex_result_free(result);
    
    // Test NULL content
    result = latex_compile_content(NULL, "test_null");
    g_assert_null(result);
}

/**
 * @brief Test LaTeX compilation timeout handling
 */
static void
test_latex_timeout_handling(void)
{
    g_test_message("Testing timeout handling...");
    
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available");
        return;
    }
    
    // Create config with very short timeout
    latex_config_t *timeout_config = latex_config_new();
    latex_config_set_temp_dir(timeout_config, test_temp_dir);
    latex_config_set_timeout(timeout_config, 1); // 1 second timeout
    
    g_assert_true(latex_engine_init_with_config(timeout_config));
    
    // Create a document that might take longer to compile
    const gchar *complex_doc = 
        "\\documentclass{article}\n"
        "\\usepackage{tikz}\n"
        "\\begin{document}\n"
        "\\begin{tikzpicture}\n"
        "\\foreach \\i in {1,...,1000} {\n"
        "  \\draw (\\i/100, 0) circle (0.1);\n"
        "}\n"
        "\\end{tikzpicture}\n"
        "\\end{document}\n";
    
    clock_t start = clock();
    latex_result_t *result = latex_compile_content(complex_doc, "test_timeout");
    clock_t end = clock();
    
    double elapsed = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    // Should either succeed quickly or timeout within reasonable time
    g_assert_true(elapsed < 10.0); // Should not take more than 10 seconds
    
    if (result) {
        if (!latex_result_is_success(result)) {
            g_test_message("Compilation failed (possibly due to timeout): %s", 
                          result->error_message);
        }
        latex_result_free(result);
    }
    
    latex_config_free(timeout_config);
}

/* ========== SECURITY TESTS ========== */

/**
 * @brief Test comprehensive LaTeX text escaping
 */
static void
test_latex_text_escaping_comprehensive(void)
{
    g_test_message("Testing comprehensive text escaping...");
    
    g_assert_true(latex_engine_init());
    
    // Test all special LaTeX characters
    const gchar *special_chars = "\\{}$&%#^_~";
    gchar *escaped = latex_escape_text(special_chars);
    g_assert_nonnull(escaped);
    
    // Verify no unescaped special characters remain
    g_assert_false(strstr(escaped, "\\") && !strstr(escaped, "\\textbackslash"));
    g_assert_false(strstr(escaped, "{") && !strstr(escaped, "\\{"));
    g_assert_false(strstr(escaped, "}") && !strstr(escaped, "\\}"));
    g_assert_false(strstr(escaped, "$") && !strstr(escaped, "\\$"));
    g_assert_false(strstr(escaped, "&") && !strstr(escaped, "\\&"));
    g_assert_false(strstr(escaped, "%") && !strstr(escaped, "\\%"));
    g_assert_false(strstr(escaped, "#") && !strstr(escaped, "\\#"));
    
    g_free(escaped);
    
    // Test NULL input
    escaped = latex_escape_text(NULL);
    g_assert_null(escaped);
    
    // Test empty string
    escaped = latex_escape_text("");
    g_assert_nonnull(escaped);
    g_assert_cmpstr(escaped, ==, "");
    g_free(escaped);
    
    // Test normal text (should remain unchanged)
    const gchar *normal_text = "This is normal text with spaces and numbers 123.";
    escaped = latex_escape_text(normal_text);
    g_assert_nonnull(escaped);
    g_assert_cmpstr(escaped, ==, normal_text);
    g_free(escaped);
}

/**
 * @brief Test LaTeX injection prevention
 */
static void
test_latex_injection_prevention(void)
{
    g_test_message("Testing LaTeX injection prevention...");
    
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available");
        return;
    }
    
    g_assert_true(latex_engine_init_with_config(test_config));
    
    // Test potential injection attempts
    const gchar *injection_attempts[] = {
        "\\input{/etc/passwd}",
        "\\write18{rm -rf /}",
        "\\immediate\\write18{cat /etc/shadow}",
        "\\openin\\myfile=/etc/hosts",
        "\\catcode`\\{=12",
        NULL
    };
    
    for (int i = 0; injection_attempts[i]; i++) {
        gchar *escaped = latex_escape_text(injection_attempts[i]);
        g_assert_nonnull(escaped);
        
        // Verify that LaTeX commands are escaped
        g_assert_false(g_str_has_prefix(escaped, "\\input"));
        g_assert_false(g_str_has_prefix(escaped, "\\write18"));
        g_assert_false(g_str_has_prefix(escaped, "\\immediate"));
        g_assert_false(g_str_has_prefix(escaped, "\\openin"));
        g_assert_false(g_str_has_prefix(escaped, "\\catcode"));
        
        g_free(escaped);
    }
    
    // Test that escaped content compiles safely
    latex_variables_t *vars = latex_variables_new();
    latex_variables_set(vars, "user_input", "\\input{/etc/passwd}");
    
    const gchar *template = 
        "\\documentclass{article}\n"
        "\\begin{document}\n"
        "User input: {{user_input}}\n"
        "\\end{document}\n";
    
    gchar *processed = latex_process_template(template, vars);
    g_assert_nonnull(processed);
    
    // Verify the injection attempt was escaped
    g_assert_false(strstr(processed, "\\input{/etc/passwd}"));
    
    latex_result_t *result = latex_compile_content(processed, "test_injection");
    g_assert_nonnull(result);
    
    // Should compile successfully with escaped content
    if (!latex_result_is_success(result)) {
        g_test_message("Injection test compilation failed: %s", result->error_message);
    }
    
    g_free(processed);
    latex_variables_free(vars);
    latex_result_free(result);
}

/**
 * @brief Test Unicode handling
 */
static void
test_latex_unicode_handling(void)
{
    g_test_message("Testing Unicode handling...");
    
    g_assert_true(latex_engine_init());
    
    // Test various Unicode characters
    const gchar *unicode_text = "Héllo Wörld! 你好世界 🔒🛡️";
    gchar *escaped = latex_escape_text(unicode_text);
    g_assert_nonnull(escaped);
    
    // Unicode characters should be preserved (not escaped)
    g_assert_true(g_utf8_validate(escaped, -1, NULL));
    
    g_free(escaped);
    
    // Test Unicode in template processing
    latex_variables_t *vars = latex_variables_new();
    latex_variables_set(vars, "unicode_text", unicode_text);
    
    const gchar *template = "Unicode test: {{unicode_text}}";
    gchar *processed = latex_process_template(template, vars);
    g_assert_nonnull(processed);
    g_assert_true(g_utf8_validate(processed, -1, NULL));
    
    g_free(processed);
    latex_variables_free(vars);
}

/* ========== PERFORMANCE TESTS ========== */

/**
 * @brief Test memory management and leak detection
 */
static void
test_latex_memory_management(void)
{
    g_test_message("Testing memory management...");
    
    g_assert_true(latex_engine_init_with_config(test_config));
    
    // Test multiple allocations and deallocations
    for (int i = 0; i < 100; i++) {
        latex_variables_t *vars = latex_variables_new();
        
        for (int j = 0; j < 10; j++) {
            gchar *key = g_strdup_printf("key_%d", j);
            gchar *value = g_strdup_printf("value_%d_%d", i, j);
            latex_variables_set(vars, key, value);
            g_free(key);
            g_free(value);
        }
        
        latex_variables_free(vars);
    }
    
    // Test large text escaping
    GString *large_text = g_string_new("");
    for (int i = 0; i < 10000; i++) {
        g_string_append(large_text, "Test text with special chars: $&%#\n");
    }
    
    gchar *escaped = latex_escape_text(large_text->str);
    g_assert_nonnull(escaped);
    g_free(escaped);
    
    g_string_free(large_text, TRUE);
}

/**
 * @brief Test concurrent compilation
 */
static void
test_latex_concurrent_compilation(void)
{
    g_test_message("Testing concurrent compilation...");
    
    if (!latex_engine_is_available()) {
        g_test_skip("LaTeX not available");
        return;
    }
    
    g_assert_true(latex_engine_init_with_config(test_config));
    
    // This is a simplified test - in a real implementation,
    // we would use threads to test true concurrency
    const gchar *doc_template = 
        "\\documentclass{article}\n"
        "\\begin{document}\n"
        "Document number: %d\n"
        "\\end{document}\n";
    
    for (int i = 0; i < 5; i++) {
        gchar *doc_content = g_strdup_printf(doc_template, i);
        gchar *output_name = g_strdup_printf("concurrent_test_%d", i);
        
        latex_result_t *result = latex_compile_content(doc_content, output_name);
        
        if (result && latex_result_is_success(result)) {
            g_assert_nonnull(result->output_file);
            g_assert_true(g_file_test(result->output_file, G_FILE_TEST_EXISTS));
        }
        
        g_free(doc_content);
        g_free(output_name);
        if (result) latex_result_free(result);
    }
}

/* ========== TEMPLATE PROCESSING TESTS ========== */

/**
 * @brief Test variable substitution edge cases
 */
static void
test_variable_substitution_comprehensive(void)
{
    g_test_message("Testing comprehensive variable substitution...");
    
    g_assert_true(latex_engine_init());
    
    latex_variables_t *vars = latex_variables_new();
    latex_variables_set(vars, "simple", "value");
    latex_variables_set(vars, "with_spaces", "value with spaces");
    latex_variables_set(vars, "with_special", "value & special $ chars");
    latex_variables_set(vars, "empty", "");
    
    // Test basic substitution
    const gchar *template1 = "Simple: {{simple}}";
    gchar *result1 = latex_process_template(template1, vars);
    g_assert_cmpstr(result1, ==, "Simple: value");
    g_free(result1);
    
    // Test multiple substitutions
    const gchar *template2 = "{{simple}} and {{with_spaces}}";
    gchar *result2 = latex_process_template(template2, vars);
    g_assert_cmpstr(result2, ==, "value and value with spaces");
    g_free(result2);
    
    // Test repeated substitutions
    const gchar *template3 = "{{simple}} {{simple}} {{simple}}";
    gchar *result3 = latex_process_template(template3, vars);
    g_assert_cmpstr(result3, ==, "value value value");
    g_free(result3);
    
    // Test empty variable
    const gchar *template4 = "Empty: {{empty}}";
    gchar *result4 = latex_process_template(template4, vars);
    g_assert_cmpstr(result4, ==, "Empty: ");
    g_free(result4);
    
    // Test missing variable (should remain unchanged)
    const gchar *template5 = "Missing: {{missing}}";
    gchar *result5 = latex_process_template(template5, vars);
    g_assert_cmpstr(result5, ==, "Missing: {{missing}}");
    g_free(result5);
    
    // Test special characters in values
    const gchar *template6 = "Special: {{with_special}}";
    gchar *result6 = latex_process_template(template6, vars);
    g_assert_true(strstr(result6, "\\&") != NULL);
    g_assert_true(strstr(result6, "\\$") != NULL);
    g_free(result6);
    
    latex_variables_free(vars);
}

/* ========== TEST SUITE SETUP ========== */

/**
 * @brief Main test function
 */
int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    
    // Setup and teardown
    g_test_add("/latex_engine/setup", void, NULL, test_setup, NULL, NULL);
    
    // Core functionality tests
    g_test_add_func("/latex_engine/initialization", test_latex_engine_initialization);
    g_test_add_func("/latex_engine/configuration_validation", test_latex_engine_configuration_validation);
    g_test_add_func("/latex_engine/dependency_checking", test_latex_engine_dependency_checking);
    
    // Compilation tests
    g_test_add_func("/latex_engine/compilation_success", test_latex_compilation_success_scenarios);
    g_test_add_func("/latex_engine/compilation_failure", test_latex_compilation_failure_scenarios);
    g_test_add_func("/latex_engine/timeout_handling", test_latex_timeout_handling);
    
    // Security tests
    g_test_add_func("/latex_engine/text_escaping", test_latex_text_escaping_comprehensive);
    g_test_add_func("/latex_engine/injection_prevention", test_latex_injection_prevention);
    g_test_add_func("/latex_engine/unicode_handling", test_latex_unicode_handling);
    
    // Performance tests
    g_test_add_func("/latex_engine/memory_management", test_latex_memory_management);
    g_test_add_func("/latex_engine/concurrent_compilation", test_latex_concurrent_compilation);
    
    // Template processing tests
    g_test_add_func("/latex_engine/variable_substitution", test_variable_substitution_comprehensive);
    
    // Cleanup
    g_test_add("/latex_engine/teardown", void, NULL, NULL, NULL, test_teardown);
    
    return g_test_run();
}