/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Performance Benchmark Testing Suite
 * Validates performance requirements for LaTeX report generation
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>
#include <unistd.h>

#include "../src/report_api.h"
#include "../../../ai-engine/scoring/vulnerability_scoring.h"

/* Performance benchmarks (in seconds) */
#define SMALL_DATASET_BENCHMARK    30.0   // < 100 vulnerabilities
#define MEDIUM_DATASET_BENCHMARK   120.0  // 100-1000 vulnerabilities  
#define LARGE_DATASET_BENCHMARK    300.0  // > 1000 vulnerabilities

/* Memory benchmarks (in MB) */
#define SMALL_MEMORY_BENCHMARK     256
#define LARGE_MEMORY_BENCHMARK     512

/* Test data generators */
static vulnerability_score_t **create_small_dataset(guint *count);
static vulnerability_score_t **create_medium_dataset(guint *count);
static vulnerability_score_t **create_large_dataset(guint *count);
static void free_vulnerability_dataset(vulnerability_score_t **vulns, guint count);

/* Performance measurement utilities */
static double get_elapsed_time(struct timespec start, struct timespec end);
static long get_memory_usage_mb(void);
static void log_performance_result(const gchar *test_name, double elapsed, long memory_mb, gboolean passed);

/**
 * @brief Create small vulnerability dataset (< 100 vulnerabilities)
 */
static vulnerability_score_t **
create_small_dataset(guint *count)
{
    *count = 75;
    vulnerability_score_t **vulns = g_new0(vulnerability_score_t *, *count);
    
    for (guint i = 0; i < *count; i++) {
        gchar *cve_id = g_strdup_printf("CVE-2024-%04d", i + 1);
        vulns[i] = vulnerability_score_new(cve_id);
        
        // Create CVSS v3.1 score with realistic distribution
        vulns[i]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
        
        if (i < 5) {
            // Critical vulnerabilities
            vulns[i]->cvss_v3_1->base_score = 9.0 + (i * 0.2);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_CRITICAL;
        } else if (i < 20) {
            // High vulnerabilities
            vulns[i]->cvss_v3_1->base_score = 7.0 + ((i - 5) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
        } else if (i < 50) {
            // Medium vulnerabilities
            vulns[i]->cvss_v3_1->base_score = 4.0 + ((i - 20) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_MEDIUM;
        } else {
            // Low vulnerabilities
            vulns[i]->cvss_v3_1->base_score = 1.0 + ((i - 50) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_LOW;
        }
        
        // Add realistic vulnerability details
        vulns[i]->vulnerability_name = g_strdup_printf("Test Vulnerability %d", i + 1);
        vulns[i]->description = g_strdup_printf(
            "This is a test vulnerability with ID %s. It represents a security issue "
            "that could potentially impact system security. The vulnerability affects "
            "multiple components and requires immediate attention for remediation. "
            "Additional details about the vulnerability impact and exploitation methods "
            "are provided in the technical documentation.", cve_id);
        
        g_free(cve_id);
    }
    
    return vulns;
}

/**
 * @brief Create medium vulnerability dataset (100-1000 vulnerabilities)
 */
static vulnerability_score_t **
create_medium_dataset(guint *count)
{
    *count = 500;
    vulnerability_score_t **vulns = g_new0(vulnerability_score_t *, *count);
    
    for (guint i = 0; i < *count; i++) {
        gchar *cve_id = g_strdup_printf("CVE-2024-%04d", i + 1);
        vulns[i] = vulnerability_score_new(cve_id);
        
        // Create CVSS v3.1 score with realistic enterprise distribution
        vulns[i]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
        
        if (i < 25) {
            // Critical vulnerabilities (5%)
            vulns[i]->cvss_v3_1->base_score = 9.0 + ((i % 10) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_CRITICAL;
        } else if (i < 100) {
            // High vulnerabilities (15%)
            vulns[i]->cvss_v3_1->base_score = 7.0 + ((i % 20) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
        } else if (i < 300) {
            // Medium vulnerabilities (40%)
            vulns[i]->cvss_v3_1->base_score = 4.0 + ((i % 30) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_MEDIUM;
        } else {
            // Low vulnerabilities (40%)
            vulns[i]->cvss_v3_1->base_score = 1.0 + ((i % 30) * 0.1);
            vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_LOW;
        }
        
        // Add comprehensive vulnerability details
        vulns[i]->vulnerability_name = g_strdup_printf("Enterprise Vulnerability %d", i + 1);
        vulns[i]->description = g_strdup_printf(
            "Enterprise vulnerability %s identified during comprehensive security assessment. "
            "This vulnerability affects critical business systems and requires coordinated "
            "remediation efforts across multiple teams. Impact analysis shows potential for "
            "data confidentiality, integrity, and availability concerns. Detailed technical "
            "analysis and remediation procedures are documented in the security advisory. "
            "Priority level has been assigned based on business risk assessment and "
            "exploitability factors. Coordination with vendor support may be required "
            "for complete resolution of this security issue.", cve_id);
        
        // Add some KEV and EPSS data for realism
        if (i % 20 == 0) {
            vulns[i]->kev = g_new0(kev_info_t, 1);
            vulns[i]->kev->is_kev = TRUE;
            vulns[i]->kev->date_added = g_strdup("2024-01-15");
            vulns[i]->kev->due_date = g_strdup("2024-02-15");
            vulns[i]->kev->required_action = g_strdup("Apply security patches");
        }
        
        if (i % 10 == 0) {
            vulns[i]->epss = g_new0(epss_info_t, 1);
            vulns[i]->epss->score = 0.1 + ((i % 100) * 0.008);
            vulns[i]->epss->percentile = 0.5 + ((i % 50) * 0.01);
            vulns[i]->epss->date = g_strdup("2024-01-20");
        }
        
        g_free(cve_id);
    }
    
    return vulns;
}

/**
 * @brief Create large vulnerability dataset (> 1000 vulnerabilities)
 */
static vulnerability_score_t **
create_large_dataset(guint *count)
{
    *count = 2500;
    vulnerability_score_t **vulns = g_new0(vulnerability_score_t *, *count);
    
    for (guint i = 0; i < *count; i++) {
        gchar *cve_id = g_strdup_printf("CVE-2024-%05d", i + 1);
        vulns[i] = vulnerability_score_new(cve_id);
        
        // Create mixed CVSS versions for large enterprise environment
        if (i % 4 == 0) {
            // CVSS v3.1 (most common)
            vulns[i]->cvss_v3_1 = g_new0(cvss_v3_t, 1);
            vulns[i]->cvss_v3_1->base_score = 1.0 + ((i % 90) * 0.1);
            
            if (vulns[i]->cvss_v3_1->base_score >= 9.0) {
                vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_CRITICAL;
            } else if (vulns[i]->cvss_v3_1->base_score >= 7.0) {
                vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_HIGH;
            } else if (vulns[i]->cvss_v3_1->base_score >= 4.0) {
                vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_MEDIUM;
            } else {
                vulns[i]->cvss_v3_1->severity = CVSS_SEVERITY_LOW;
            }
        } else if (i % 4 == 1) {
            // CVSS v3.0
            vulns[i]->cvss_v3_0 = g_new0(cvss_v3_t, 1);
            vulns[i]->cvss_v3_0->base_score = 1.0 + ((i % 90) * 0.1);
            
            if (vulns[i]->cvss_v3_0->base_score >= 9.0) {
                vulns[i]->cvss_v3_0->severity = CVSS_SEVERITY_CRITICAL;
            } else if (vulns[i]->cvss_v3_0->base_score >= 7.0) {
                vulns[i]->cvss_v3_0->severity = CVSS_SEVERITY_HIGH;
            } else if (vulns[i]->cvss_v3_0->base_score >= 4.0) {
                vulns[i]->cvss_v3_0->severity = CVSS_SEVERITY_MEDIUM;
            } else {
                vulns[i]->cvss_v3_0->severity = CVSS_SEVERITY_LOW;
            }
        } else {
            // CVSS v2 (legacy systems)
            vulns[i]->cvss_v2 = g_new0(cvss_v2_t, 1);
            vulns[i]->cvss_v2->base_score = 1.0 + ((i % 90) * 0.1);
        }
        
        // Add comprehensive vulnerability information
        vulns[i]->vulnerability_name = g_strdup_printf("Large Scale Vulnerability %d", i + 1);
        vulns[i]->description = g_strdup_printf(
            "Large-scale enterprise vulnerability %s discovered during extensive security "
            "assessment of complex IT infrastructure. This vulnerability represents one of "
            "many security issues identified across distributed systems, cloud environments, "
            "and legacy infrastructure components. The vulnerability requires systematic "
            "approach to remediation considering business continuity, system dependencies, "
            "and operational constraints. Risk assessment includes evaluation of threat "
            "landscape, exploitability factors, and potential business impact. Remediation "
            "strategy must account for change management processes, testing requirements, "
            "and coordination across multiple technical teams and business stakeholders.", cve_id);
        
        // Add realistic metadata
        if (i % 50 == 0) {
            vulns[i]->kev = g_new0(kev_info_t, 1);
            vulns[i]->kev->is_kev = TRUE;
            vulns[i]->kev->date_added = g_strdup("2024-01-10");
            vulns[i]->kev->due_date = g_strdup("2024-03-10");
            vulns[i]->kev->required_action = g_strdup("Apply vendor patches immediately");
        }
        
        if (i % 25 == 0) {
            vulns[i]->epss = g_new0(epss_info_t, 1);
            vulns[i]->epss->score = 0.01 + ((i % 1000) * 0.0009);
            vulns[i]->epss->percentile = 0.1 + ((i % 900) * 0.001);
            vulns[i]->epss->date = g_strdup("2024-01-25");
        }
        
        g_free(cve_id);
    }
    
    return vulns;
}

/**
 * @brief Free vulnerability dataset
 */
static void
free_vulnerability_dataset(vulnerability_score_t **vulns, guint count)
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
 * @brief Calculate elapsed time between two timespec structures
 */
static double
get_elapsed_time(struct timespec start, struct timespec end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
}

/**
 * @brief Get current memory usage in MB
 */
static long
get_memory_usage_mb(void)
{
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // ru_maxrss is in kilobytes on Linux, bytes on macOS
        #ifdef __APPLE__
        return usage.ru_maxrss / (1024 * 1024);
        #else
        return usage.ru_maxrss / 1024;
        #endif
    }
    return -1;
}

/**
 * @brief Log performance test result
 */
static void
log_performance_result(const gchar *test_name, double elapsed, long memory_mb, gboolean passed)
{
    const gchar *status = passed ? "PASS" : "FAIL";
    g_test_message("PERFORMANCE [%s] %s: %.2fs, %ldMB", status, test_name, elapsed, memory_mb);
    
    if (passed) {
        g_test_message("✅ %s completed within benchmark", test_name);
    } else {
        g_test_message("❌ %s exceeded performance benchmark", test_name);
    }
}

/* ========== PERFORMANCE BENCHMARK TESTS ========== */

/**
 * @brief Test small dataset performance (< 100 vulnerabilities, < 30 seconds)
 */
static void
test_small_dataset_performance(void)
{
    g_test_message("Testing small dataset performance benchmark...");
    
    if (!report_api_init()) {
        g_test_skip("Report API initialization failed");
        return;
    }
    
    // Create small dataset
    guint vuln_count;
    vulnerability_score_t **vulns = create_small_dataset(&vuln_count);
    g_assert_cmpuint(vuln_count, <, 100);
    
    // Create report configuration
    report_config_t *config = report_config_new();
    config->output_path = g_strdup("/tmp/small_dataset_performance_test.pdf");
    
    // Measure performance
    struct timespec start, end;
    long start_memory = get_memory_usage_mb();
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    report_result_t *result = report_generate(vulns, vuln_count, config);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    long end_memory = get_memory_usage_mb();
    double elapsed = get_elapsed_time(start, end);
    long memory_used = end_memory > start_memory ? end_memory - start_memory : end_memory;
    
    // Validate performance benchmarks
    gboolean time_passed = elapsed <= SMALL_DATASET_BENCHMARK;
    gboolean memory_passed = memory_used <= SMALL_MEMORY_BENCHMARK;
    gboolean generation_passed = result && report_result_is_success(result);
    
    log_performance_result("Small Dataset", elapsed, memory_used, 
                          time_passed && memory_passed && generation_passed);
    
    // Assert performance requirements
    g_assert_true(generation_passed);
    g_assert_cmpfloat(elapsed, <=, SMALL_DATASET_BENCHMARK);
    g_assert_cmpint(memory_used, <=, SMALL_MEMORY_BENCHMARK);
    
    // Cleanup
    if (result) report_result_free(result);
    report_config_free(config);
    free_vulnerability_dataset(vulns, vuln_count);
    report_api_cleanup();
}

/**
 * @brief Test medium dataset performance (100-1000 vulnerabilities, < 2 minutes)
 */
static void
test_medium_dataset_performance(void)
{
    g_test_message("Testing medium dataset performance benchmark...");
    
    if (!report_api_init()) {
        g_test_skip("Report API initialization failed");
        return;
    }
    
    // Create medium dataset
    guint vuln_count;
    vulnerability_score_t **vulns = create_medium_dataset(&vuln_count);
    g_assert_cmpuint(vuln_count, >=, 100);
    g_assert_cmpuint(vuln_count, <=, 1000);
    
    // Create report configuration
    report_config_t *config = report_config_new();
    config->output_path = g_strdup("/tmp/medium_dataset_performance_test.pdf");
    
    // Measure performance
    struct timespec start, end;
    long start_memory = get_memory_usage_mb();
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    report_result_t *result = report_generate(vulns, vuln_count, config);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    long end_memory = get_memory_usage_mb();
    double elapsed = get_elapsed_time(start, end);
    long memory_used = end_memory > start_memory ? end_memory - start_memory : end_memory;
    
    // Validate performance benchmarks
    gboolean time_passed = elapsed <= MEDIUM_DATASET_BENCHMARK;
    gboolean memory_passed = memory_used <= LARGE_MEMORY_BENCHMARK;
    gboolean generation_passed = result && report_result_is_success(result);
    
    log_performance_result("Medium Dataset", elapsed, memory_used, 
                          time_passed && memory_passed && generation_passed);
    
    // Assert performance requirements
    g_assert_true(generation_passed);
    g_assert_cmpfloat(elapsed, <=, MEDIUM_DATASET_BENCHMARK);
    g_assert_cmpint(memory_used, <=, LARGE_MEMORY_BENCHMARK);
    
    // Cleanup
    if (result) report_result_free(result);
    report_config_free(config);
    free_vulnerability_dataset(vulns, vuln_count);
    report_api_cleanup();
}

/**
 * @brief Test large dataset performance (> 1000 vulnerabilities, < 5 minutes)
 */
static void
test_large_dataset_performance(void)
{
    g_test_message("Testing large dataset performance benchmark...");
    
    if (!report_api_init()) {
        g_test_skip("Report API initialization failed");
        return;
    }
    
    // Create large dataset
    guint vuln_count;
    vulnerability_score_t **vulns = create_large_dataset(&vuln_count);
    g_assert_cmpuint(vuln_count, >, 1000);
    
    // Create report configuration
    report_config_t *config = report_config_new();
    config->output_path = g_strdup("/tmp/large_dataset_performance_test.pdf");
    
    // Measure performance
    struct timespec start, end;
    long start_memory = get_memory_usage_mb();
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    report_result_t *result = report_generate(vulns, vuln_count, config);
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    long end_memory = get_memory_usage_mb();
    double elapsed = get_elapsed_time(start, end);
    long memory_used = end_memory > start_memory ? end_memory - start_memory : end_memory;
    
    // Validate performance benchmarks
    gboolean time_passed = elapsed <= LARGE_DATASET_BENCHMARK;
    gboolean memory_passed = memory_used <= LARGE_MEMORY_BENCHMARK;
    gboolean generation_passed = result && report_result_is_success(result);
    
    log_performance_result("Large Dataset", elapsed, memory_used, 
                          time_passed && memory_passed && generation_passed);
    
    // Assert performance requirements
    g_assert_true(generation_passed);
    g_assert_cmpfloat(elapsed, <=, LARGE_DATASET_BENCHMARK);
    g_assert_cmpint(memory_used, <=, LARGE_MEMORY_BENCHMARK);
    
    // Cleanup
    if (result) report_result_free(result);
    report_config_free(config);
    free_vulnerability_dataset(vulns, vuln_count);
    report_api_cleanup();
}

/**
 * @brief Test concurrent report generation performance
 */
static void
test_concurrent_performance(void)
{
    g_test_message("Testing concurrent report generation performance...");
    
    if (!report_api_init()) {
        g_test_skip("Report API initialization failed");
        return;
    }
    
    // Create multiple small datasets for concurrent testing
    const int num_reports = 3;
    vulnerability_score_t **datasets[num_reports];
    guint counts[num_reports];
    report_config_t *configs[num_reports];
    
    for (int i = 0; i < num_reports; i++) {
        datasets[i] = create_small_dataset(&counts[i]);
        configs[i] = report_config_new();
        g_free(configs[i]->output_path);
        configs[i]->output_path = g_strdup_printf("/tmp/concurrent_test_%d.pdf", i);
    }
    
    // Measure concurrent generation time
    struct timespec start, end;
    long start_memory = get_memory_usage_mb();
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Generate reports sequentially (simulating concurrent load)
    report_result_t *results[num_reports];
    for (int i = 0; i < num_reports; i++) {
        results[i] = report_generate(datasets[i], counts[i], configs[i]);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    long end_memory = get_memory_usage_mb();
    double elapsed = get_elapsed_time(start, end);
    long memory_used = end_memory > start_memory ? end_memory - start_memory : end_memory;
    
    // Validate all reports generated successfully
    gboolean all_passed = TRUE;
    for (int i = 0; i < num_reports; i++) {
        if (!results[i] || !report_result_is_success(results[i])) {
            all_passed = FALSE;
            break;
        }
    }
    
    // Performance should scale reasonably with number of reports
    double expected_time = SMALL_DATASET_BENCHMARK * num_reports * 1.2; // 20% overhead allowance
    gboolean time_passed = elapsed <= expected_time;
    gboolean memory_passed = memory_used <= (SMALL_MEMORY_BENCHMARK * num_reports);
    
    log_performance_result("Concurrent Generation", elapsed, memory_used, 
                          time_passed && memory_passed && all_passed);
    
    g_assert_true(all_passed);
    g_assert_cmpfloat(elapsed, <=, expected_time);
    g_assert_cmpint(memory_used, <=, LARGE_MEMORY_BENCHMARK);
    
    // Cleanup
    for (int i = 0; i < num_reports; i++) {
        if (results[i]) report_result_free(results[i]);
        report_config_free(configs[i]);
        free_vulnerability_dataset(datasets[i], counts[i]);
    }
    
    report_api_cleanup();
}

/**
 * @brief Test memory usage patterns
 */
static void
test_memory_usage_patterns(void)
{
    g_test_message("Testing memory usage patterns...");
    
    if (!report_api_init()) {
        g_test_skip("Report API initialization failed");
        return;
    }
    
    long baseline_memory = get_memory_usage_mb();
    
    // Test multiple report generations to check for memory leaks
    for (int iteration = 0; iteration < 5; iteration++) {
        guint vuln_count;
        vulnerability_score_t **vulns = create_small_dataset(&vuln_count);
        
        report_config_t *config = report_config_new();
        g_free(config->output_path);
        config->output_path = g_strdup_printf("/tmp/memory_test_%d.pdf", iteration);
        
        report_result_t *result = report_generate(vulns, vuln_count, config);
        
        if (result) {
            g_assert_true(report_result_is_success(result));
            report_result_free(result);
        }
        
        report_config_free(config);
        free_vulnerability_dataset(vulns, vuln_count);
        
        // Check memory usage after each iteration
        long current_memory = get_memory_usage_mb();
        long memory_growth = current_memory - baseline_memory;
        
        g_test_message("Iteration %d: Memory usage %ldMB (growth: %ldMB)", 
                      iteration + 1, current_memory, memory_growth);
        
        // Memory growth should be reasonable (< 50MB per iteration)
        g_assert_cmpint(memory_growth, <, 50 * (iteration + 1));
    }
    
    report_api_cleanup();
}

/* ========== TEST SUITE SETUP ========== */

/**
 * @brief Main test function
 */
int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);
    
    // Performance benchmark tests
    g_test_add_func("/performance/small_dataset", test_small_dataset_performance);
    g_test_add_func("/performance/medium_dataset", test_medium_dataset_performance);
    g_test_add_func("/performance/large_dataset", test_large_dataset_performance);
    g_test_add_func("/performance/concurrent", test_concurrent_performance);
    g_test_add_func("/performance/memory_patterns", test_memory_usage_patterns);
    
    g_test_message("Starting performance benchmark tests...");
    g_test_message("Benchmarks: Small=%.0fs, Medium=%.0fs, Large=%.0fs", 
                   SMALL_DATASET_BENCHMARK, MEDIUM_DATASET_BENCHMARK, LARGE_DATASET_BENCHMARK);
    
    return g_test_run();
}