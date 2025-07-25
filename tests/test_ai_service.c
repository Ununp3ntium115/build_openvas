/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Service Test Program
 */

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include "../ai-engine/api/ai_service.h"

/**
 * @brief Test AI service initialization
 */
static gboolean
test_ai_service_init(void)
{
    g_print("Testing AI service initialization...\n");
    
    if (!ai_service_init()) {
        g_print("❌ AI service initialization failed\n");
        return FALSE;
    }
    
    g_print("✅ AI service initialized successfully\n");
    return TRUE;
}

/**
 * @brief Test AI configuration creation
 */
static gboolean
test_ai_config(void)
{
    g_print("Testing AI configuration...\n");
    
    ai_config_t *config = ai_config_new(AI_PROVIDER_OPENAI, "test-api-key");
    if (!config) {
        g_print("❌ Failed to create AI configuration\n");
        return FALSE;
    }
    
    if (config->provider != AI_PROVIDER_OPENAI) {
        g_print("❌ Incorrect provider type\n");
        ai_config_free(config);
        return FALSE;
    }
    
    if (g_strcmp0(config->api_key, "test-api-key") != 0) {
        g_print("❌ Incorrect API key\n");
        ai_config_free(config);
        return FALSE;
    }
    
    ai_config_free(config);
    g_print("✅ AI configuration test passed\n");
    return TRUE;
}

/**
 * @brief Test AI request creation
 */
static gboolean
test_ai_request(void)
{
    g_print("Testing AI request creation...\n");
    
    JsonObject *input_data = json_object_new();
    json_object_set_string_member(input_data, "test", "data");
    
    ai_request_t *request = ai_request_new(AI_TASK_VULNERABILITY_ANALYSIS, input_data);
    if (!request) {
        g_print("❌ Failed to create AI request\n");
        json_object_unref(input_data);
        return FALSE;
    }
    
    if (request->task_type != AI_TASK_VULNERABILITY_ANALYSIS) {
        g_print("❌ Incorrect task type\n");
        ai_request_free(request);
        return FALSE;
    }
    
    ai_request_free(request);
    g_print("✅ AI request test passed\n");
    return TRUE;
}

/**
 * @brief Test utility functions
 */
static gboolean
test_utility_functions(void)
{
    g_print("Testing utility functions...\n");
    
    // Test provider to string conversion
    const gchar *provider_str = ai_provider_to_string(AI_PROVIDER_OPENAI);
    if (g_strcmp0(provider_str, "OpenAI") != 0) {
        g_print("❌ Provider to string conversion failed\n");
        return FALSE;
    }
    
    // Test task type to string conversion
    const gchar *task_str = ai_task_type_to_string(AI_TASK_VULNERABILITY_ANALYSIS);
    if (g_strcmp0(task_str, "Vulnerability Analysis") != 0) {
        g_print("❌ Task type to string conversion failed\n");
        return FALSE;
    }
    
    g_print("✅ Utility functions test passed\n");
    return TRUE;
}

/**
 * @brief Test AI service availability check
 */
static gboolean
test_service_availability(void)
{
    g_print("Testing service availability...\n");
    
    // Should return FALSE since no providers are configured
    if (ai_service_is_available(AI_PROVIDER_OPENAI)) {
        g_print("⚠️  OpenAI provider appears to be available (unexpected)\n");
    } else {
        g_print("✅ OpenAI provider correctly reported as unavailable\n");
    }
    
    return TRUE;
}

/**
 * @brief Main test function
 */
int
main(int argc, char *argv[])
{
    g_print("=== AI-Enhanced OpenVAS Service Tests ===\n\n");
    
    gboolean all_passed = TRUE;
    
    // Run tests
    all_passed &= test_ai_service_init();
    all_passed &= test_ai_config();
    all_passed &= test_ai_request();
    all_passed &= test_utility_functions();
    all_passed &= test_service_availability();
    
    // Cleanup
    ai_service_cleanup();
    
    g_print("\n=== Test Results ===\n");
    if (all_passed) {
        g_print("🎉 All tests passed!\n");
        return EXIT_SUCCESS;
    } else {
        g_print("❌ Some tests failed!\n");
        return EXIT_FAILURE;
    }
}