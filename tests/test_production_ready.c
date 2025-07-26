/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * Production-Ready AI Integration Test Suite
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <json-glib/json-glib.h>
#include "../ai-engine/api/ai_service.h"
#include "../ai-engine/config/ai_config_manager.h"
#include "../ai-engine/security/ai_security.h"
#include "../ai-engine/monitoring/ai_metrics.h"

/* Test configuration */
#define TEST_CONFIG_FILE "test_ai_config.conf"
#define TEST_ITERATIONS 100
#define TEST_CONCURRENT_REQUESTS 10

/* Test counters */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macros */
#define TEST_START(name) \
    do { \
        tests_run++; \
        g_print("🧪 Testing %s... ", name); \
    } while(0)

#define TEST_PASS() \
    do { \
        tests_passed++; \
        g_print("✅ PASS\n"); \
    } while(0)

#define TEST_FAIL(reason) \
    do { \
        tests_failed++; \
        g_print("❌ FAIL: %s\n", reason); \
    } while(0)

#define ASSERT_TRUE(condition, message) \
    do { \
        if (!(condition)) { \
            TEST_FAIL(message); \
            return FALSE; \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr, message) \
    do { \
        if ((ptr) == NULL) { \
            TEST_FAIL(message); \
            return FALSE; \
        } \
    } while(0)

/**
 * @brief Create test configuration file
 */
static gboolean
create_test_config_file(void)
{
    GKeyFile *keyfile = g_key_file_new();
    
    // Service configuration
    g_key_file_set_boolean(keyfile, "service", "enabled", TRUE);
    g_key_file_set_integer(keyfile, "service", "thread_pool_size", 8);
    g_key_file_set_integer(keyfile, "service", "default_timeout", 30);
    
    // Cache configuration
    g_key_file_set_boolean(keyfile, "cache", "enabled", TRUE);
    g_key_file_set_integer(keyfile, "cache", "max_entries", 1000);
    g_key_file_set_int64(keyfile, "cache", "default_ttl", 3600);
    
    // Rate limiting
    g_key_file_set_boolean(keyfile, "rate_limiting", "enabled", TRUE);
    g_key_file_set_integer(keyfile, "rate_limiting", "requests_per_minute", 60);
    
    // Security settings
    g_key_file_set_boolean(keyfile, "security", "encrypt_config", FALSE);
    g_key_file_set_boolean(keyfile, "security", "sanitize_data", TRUE);
    g_key_file_set_boolean(keyfile, "security", "audit_enabled", TRUE);
    g_key_file_set_string(keyfile, "security", "log_level", "INFO");
    
    // Feature flags
    g_key_file_set_boolean(keyfile, "features", "vulnerability_analysis", TRUE);
    g_key_file_set_boolean(keyfile, "features", "threat_modeling", TRUE);
    g_key_file_set_boolean(keyfile, "features", "scan_optimization", TRUE);
    g_key_file_set_boolean(keyfile, "features", "report_generation", TRUE);
    g_key_file_set_boolean(keyfile, "features", "exploit_suggestion", FALSE);
    
    // AI providers (test configuration)
    g_key_file_set_boolean(keyfile, "ai_providers", "openai_enabled", TRUE);
    g_key_file_set_string(keyfile, "ai_providers", "openai_api_key", "sk-test-key-for-testing");
    g_key_file_set_string(keyfile, "ai_providers", "openai_model", "gpt-4");
    g_key_file_set_integer(keyfile, "ai_providers", "openai_timeout", 30);
    
    // Save to file
    GError *error = NULL;
    gchar *data = g_key_file_to_data(keyfile, NULL, &error);
    if (error) {
        g_error_free(error);
        g_key_file_free(keyfile);
        return FALSE;
    }
    
    gboolean success = g_file_set_contents(TEST_CONFIG_FILE, data, -1, &error);
    if (error) {
        g_error_free(error);
        success = FALSE;
    }
    
    g_free(data);
    g_key_file_free(keyfile);
    return success;
}

/**
 * @brief Test configuration manager
 */
static gboolean
test_configuration_manager(void)
{
    TEST_START("configuration manager");
    
    // Initialize configuration manager
    ASSERT_TRUE(ai_config_manager_init(), "Failed to initialize config manager");
    
    // Test loading from file
    ASSERT_TRUE(create_test_config_file(), "Failed to create test config file");
    ASSERT_TRUE(ai_config_load_from_file(TEST_CONFIG_FILE), "Failed to load config from file");
    
    // Test global configuration
    ai_global_config_t *config = ai_config_get_global();
    ASSERT_NOT_NULL(config, "Global configuration is NULL");
    ASSERT_TRUE(config->service_enabled, "Service should be enabled");
    ASSERT_TRUE(config->cache_enabled, "Cache should be enabled");
    ASSERT_TRUE(config->vulnerability_analysis_enabled, "Vulnerability analysis should be enabled");
    
    // Test provider configuration
    ai_config_t *openai_config = ai_config_get_provider(AI_PROVIDER_OPENAI);
    ASSERT_NOT_NULL(openai_config, "OpenAI configuration not found");
    ASSERT_TRUE(openai_config->provider == AI_PROVIDER_OPENAI, "Incorrect provider type");
    
    // Cleanup
    ai_config_manager_cleanup();
    g_unlink(TEST_CONFIG_FILE);
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test enhanced AI service with production features
 */
static gboolean
test_enhanced_ai_service(void)
{
    TEST_START("enhanced AI service");
    
    // Initialize with configuration
    ASSERT_TRUE(create_test_config_file(), "Failed to create test config");
    ASSERT_TRUE(ai_service_init_with_config(TEST_CONFIG_FILE), "Failed to initialize AI service with config");
    
    // Test service availability
    ASSERT_TRUE(ai_service_is_available(AI_PROVIDER_OPENAI), "OpenAI provider should be available");
    
    // Test cache functionality
    ASSERT_TRUE(ai_cache_init(100, 300), "Failed to initialize cache");
    ASSERT_TRUE(ai_cache_size() == 0, "Cache should be empty initially");
    
    // Test rate limiting
    ASSERT_TRUE(ai_rate_limit_init(60), "Failed to initialize rate limiting");
    ASSERT_TRUE(ai_rate_limit_check(AI_PROVIDER_OPENAI), "Rate limit check should pass initially");
    
    // Test metrics
    ai_metrics_t *metrics = ai_metrics_get();
    ASSERT_NOT_NULL(metrics, "Metrics should be available");
    ASSERT_TRUE(metrics->total_requests == 0, "Initial request count should be 0");
    
    // Cleanup
    ai_cache_cleanup();
    ai_rate_limit_cleanup();
    ai_service_cleanup();
    g_unlink(TEST_CONFIG_FILE);
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test caching system
 */
static gboolean
test_caching_system(void)
{
    TEST_START("caching system");
    
    // Initialize cache
    ASSERT_TRUE(ai_cache_init(10, 60), "Failed to initialize cache");
    
    // Create test response
    ai_response_t *response = ai_response_new();
    response->success = TRUE;
    response->confidence_score = 0.95;
    
    JsonObject *result = json_object_new();
    json_object_set_string_member(result, "content", "Test AI response");
    response->result = result;
    
    // Test cache set/get
    const gchar *test_key = "test_cache_key";
    ai_cache_set(test_key, response, 60);
    ASSERT_TRUE(ai_cache_size() == 1, "Cache size should be 1 after adding entry");
    
    ai_response_t *cached_response = ai_cache_get(test_key);
    ASSERT_NOT_NULL(cached_response, "Should retrieve cached response");
    ASSERT_TRUE(cached_response->success, "Cached response should be successful");
    ASSERT_TRUE(cached_response->confidence_score == 0.95, "Cached confidence score should match");
    
    // Test cache invalidation
    ai_cache_invalidate(test_key);
    ai_response_t *invalidated_response = ai_cache_get(test_key);
    ASSERT_TRUE(invalidated_response == NULL, "Response should be NULL after invalidation");
    
    // Test cache clear
    ai_cache_set("key1", response, 60);
    ai_cache_set("key2", response, 60);
    ASSERT_TRUE(ai_cache_size() == 2, "Cache should have 2 entries");
    
    ai_cache_clear();
    ASSERT_TRUE(ai_cache_size() == 0, "Cache should be empty after clear");
    
    // Cleanup
    ai_response_free(response);
    ai_response_free(cached_response);
    ai_cache_cleanup();
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test rate limiting system
 */
static gboolean
test_rate_limiting(void)
{
    TEST_START("rate limiting system");
    
    // Initialize rate limiting with low limit for testing
    ASSERT_TRUE(ai_rate_limit_init(5), "Failed to initialize rate limiting");
    
    // Test initial state
    ASSERT_TRUE(ai_rate_limit_remaining(AI_PROVIDER_OPENAI) == 5, "Should have 5 requests remaining initially");
    
    // Test rate limit checks
    for (int i = 0; i < 5; i++) {
        ASSERT_TRUE(ai_rate_limit_check(AI_PROVIDER_OPENAI), "Rate limit check should pass");
    }
    
    // Should be rate limited now
    ASSERT_TRUE(!ai_rate_limit_check(AI_PROVIDER_OPENAI), "Should be rate limited after 5 requests");
    ASSERT_TRUE(ai_rate_limit_remaining(AI_PROVIDER_OPENAI) == 0, "Should have 0 requests remaining");
    
    // Test rate limit reset
    ai_rate_limit_reset(AI_PROVIDER_OPENAI);
    ASSERT_TRUE(ai_rate_limit_check(AI_PROVIDER_OPENAI), "Rate limit check should pass after reset");
    
    // Cleanup
    ai_rate_limit_cleanup();
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test request processing with caching and rate limiting
 */
static gboolean
test_request_processing_integration(void)
{
    TEST_START("request processing integration");
    
    // Initialize all systems
    ASSERT_TRUE(create_test_config_file(), "Failed to create test config");
    ASSERT_TRUE(ai_service_init_with_config(TEST_CONFIG_FILE), "Failed to initialize AI service");
    
    // Create test request
    JsonObject *input_data = json_object_new();
    json_object_set_string_member(input_data, "vulnerability", "SQL Injection");
    json_object_set_string_member(input_data, "severity", "High");
    
    ai_request_t *request = ai_request_new(AI_TASK_VULNERABILITY_ANALYSIS, input_data);
    ASSERT_NOT_NULL(request, "Failed to create AI request");
    
    // Set configuration
    ai_config_t *config = ai_config_get_provider(AI_PROVIDER_OPENAI);
    request->config = config;
    
    // Test cache key generation
    gchar *cache_key = ai_request_generate_cache_key(request);
    ASSERT_NOT_NULL(cache_key, "Failed to generate cache key");
    ASSERT_TRUE(strlen(cache_key) > 0, "Cache key should not be empty");
    
    // Test request validation
    ASSERT_TRUE(ai_config_validate(request->config), "Request configuration should be valid");
    
    // Note: We can't test actual API calls without real API keys,
    // but we can test the framework components
    
    // Cleanup
    g_free(cache_key);
    ai_request_free(request);
    json_object_unref(input_data);
    ai_service_cleanup();
    g_unlink(TEST_CONFIG_FILE);
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test concurrent request handling
 */
static gboolean
test_concurrent_requests(void)
{
    TEST_START("concurrent request handling");
    
    // Initialize service
    ASSERT_TRUE(create_test_config_file(), "Failed to create test config");
    ASSERT_TRUE(ai_service_init_with_config(TEST_CONFIG_FILE), "Failed to initialize AI service");
    
    // Create multiple requests
    GPtrArray *requests = g_ptr_array_new_with_free_func((GDestroyNotify)ai_request_free);
    
    for (int i = 0; i < TEST_CONCURRENT_REQUESTS; i++) {
        JsonObject *input_data = json_object_new();
        json_object_set_string_member(input_data, "test_id", g_strdup_printf("test_%d", i));
        json_object_set_string_member(input_data, "vulnerability", "Test Vulnerability");
        
        ai_request_t *request = ai_request_new(AI_TASK_VULNERABILITY_ANALYSIS, input_data);
        ai_config_t *config = ai_config_get_provider(AI_PROVIDER_OPENAI);
        request->config = config;
        
        g_ptr_array_add(requests, request);
        json_object_unref(input_data);
    }
    
    ASSERT_TRUE(requests->len == TEST_CONCURRENT_REQUESTS, "Should have created all test requests");
    
    // Test cache key uniqueness
    GHashTable *cache_keys = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, NULL);
    
    for (guint i = 0; i < requests->len; i++) {
        ai_request_t *request = g_ptr_array_index(requests, i);
        gchar *cache_key = ai_request_generate_cache_key(request);
        
        ASSERT_TRUE(!g_hash_table_contains(cache_keys, cache_key), "Cache keys should be unique");
        g_hash_table_insert(cache_keys, cache_key, GINT_TO_POINTER(1));
    }
    
    ASSERT_TRUE(g_hash_table_size(cache_keys) == TEST_CONCURRENT_REQUESTS, 
               "Should have unique cache keys for all requests");
    
    // Cleanup
    g_hash_table_destroy(cache_keys);
    g_ptr_array_free(requests, TRUE);
    ai_service_cleanup();
    g_unlink(TEST_CONFIG_FILE);
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test error handling and recovery
 */
static gboolean
test_error_handling(void)
{
    TEST_START("error handling and recovery");
    
    // Test invalid configuration
    ai_config_t *invalid_config = ai_config_new(AI_PROVIDER_OPENAI, "invalid-key");
    ASSERT_TRUE(!ai_config_validate(invalid_config), "Invalid config should fail validation");
    ai_config_free(invalid_config);
    
    // Test NULL parameter handling
    ai_response_t *null_response = ai_response_copy(NULL);
    ASSERT_TRUE(null_response == NULL, "Copying NULL response should return NULL");
    
    gchar *null_cache_key = ai_request_generate_cache_key(NULL);
    ASSERT_TRUE(null_cache_key == NULL, "Cache key for NULL request should be NULL");
    
    // Test service operations without initialization
    ASSERT_TRUE(!ai_service_is_available(AI_PROVIDER_OPENAI), "Service should not be available without init");
    
    // Test cache operations without initialization
    ai_response_t *no_cache_response = ai_cache_get("test_key");
    ASSERT_TRUE(no_cache_response == NULL, "Cache get should return NULL without initialization");
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Test memory management and cleanup
 */
static gboolean
test_memory_management(void)
{
    TEST_START("memory management and cleanup");
    
    // Test multiple init/cleanup cycles
    for (int i = 0; i < 5; i++) {
        ASSERT_TRUE(ai_service_init(), "Service init should succeed");
        ASSERT_TRUE(ai_cache_init(100, 300), "Cache init should succeed");
        ASSERT_TRUE(ai_rate_limit_init(60), "Rate limit init should succeed");
        
        // Create and destroy some objects
        ai_response_t *response = ai_response_new();
        response->success = TRUE;
        JsonObject *result = json_object_new();
        json_object_set_string_member(result, "test", "data");
        response->result = result;
        
        ai_response_t *copy = ai_response_copy(response);
        ASSERT_NOT_NULL(copy, "Response copy should succeed");
        
        ai_response_free(response);
        ai_response_free(copy);
        
        // Cleanup
        ai_cache_cleanup();
        ai_rate_limit_cleanup();
        ai_service_cleanup();
    }
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Performance benchmark test
 */
static gboolean
test_performance_benchmark(void)
{
    TEST_START("performance benchmark");
    
    // Initialize service
    ASSERT_TRUE(create_test_config_file(), "Failed to create test config");
    ASSERT_TRUE(ai_service_init_with_config(TEST_CONFIG_FILE), "Failed to initialize AI service");
    
    // Benchmark cache operations
    gint64 start_time = g_get_monotonic_time();
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        ai_response_t *response = ai_response_new();
        response->success = TRUE;
        response->confidence_score = 0.9;
        
        JsonObject *result = json_object_new();
        json_object_set_string_member(result, "content", g_strdup_printf("Test response %d", i));
        response->result = result;
        
        gchar *cache_key = g_strdup_printf("benchmark_key_%d", i);
        ai_cache_set(cache_key, response, 300);
        
        ai_response_t *cached = ai_cache_get(cache_key);
        ASSERT_NOT_NULL(cached, "Should retrieve cached response");
        
        ai_response_free(response);
        ai_response_free(cached);
        g_free(cache_key);
    }
    
    gint64 end_time = g_get_monotonic_time();
    gdouble duration_ms = (end_time - start_time) / 1000.0;
    gdouble ops_per_second = (TEST_ITERATIONS * 2) / (duration_ms / 1000.0);  // 2 ops per iteration (set + get)
    
    g_print("(%.1f ops/sec) ", ops_per_second);
    
    // Benchmark should achieve reasonable performance
    ASSERT_TRUE(ops_per_second > 1000, "Should achieve > 1000 cache ops/sec");
    
    // Cleanup
    ai_service_cleanup();
    g_unlink(TEST_CONFIG_FILE);
    
    TEST_PASS();
    return TRUE;
}

/**
 * @brief Print test summary
 */
static void
print_test_summary(void)
{
    g_print("\n" "=" * 60 "\n");
    g_print("🧪 Production-Ready AI Integration Test Summary\n");
    g_print("=" * 60 "\n");
    g_print("Total tests run: %d\n", tests_run);
    g_print("Tests passed: %d ✅\n", tests_passed);
    g_print("Tests failed: %d ❌\n", tests_failed);
    g_print("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100);
    g_print("=" * 60 "\n");
    
    if (tests_failed == 0) {
        g_print("🎉 All production-ready tests passed!\n");
        g_print("✅ AI Integration Framework is production-ready\n");
        g_print("✅ Configuration management working\n");
        g_print("✅ Caching system operational\n");
        g_print("✅ Rate limiting functional\n");
        g_print("✅ Error handling robust\n");
        g_print("✅ Memory management clean\n");
        g_print("✅ Performance benchmarks met\n");
    } else {
        g_print("⚠️  Some tests failed. Please review the output above.\n");
    }
}

/**
 * @brief Main test function
 */
int
main(int argc, char *argv[])
{
    g_print("🚀 AI-Enhanced OpenVAS Production-Ready Test Suite\n");
    g_print("=" * 60 "\n");
    g_print("Testing complete AI integration framework with production features\n\n");
    
    // Run all production tests
    test_configuration_manager();
    test_enhanced_ai_service();
    test_caching_system();
    test_rate_limiting();
    test_request_processing_integration();
    test_concurrent_requests();
    test_error_handling();
    test_memory_management();
    test_performance_benchmark();
    
    // Print summary
    print_test_summary();
    
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}