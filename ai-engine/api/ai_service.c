/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Service Implementation for OpenVAS Integration
 */

#include "ai_service.h"
#include <curl/curl.h>
#include <string.h>

/* Global AI service state */
static gboolean ai_service_initialized = FALSE;
static GHashTable *provider_configs = NULL;
static GThreadPool *ai_thread_pool = NULL;

/**
 * @brief HTTP response structure for CURL
 */
typedef struct {
    gchar *data;
    gsize size;
} http_response_t;

/**
 * @brief CURL write callback
 */
static size_t
curl_write_callback(void *contents, size_t size, size_t nmemb, http_response_t *response)
{
    size_t realsize = size * nmemb;
    response->data = g_realloc(response->data, response->size + realsize + 1);
    
    if (response->data) {
        memcpy(&(response->data[response->size]), contents, realsize);
        response->size += realsize;
        response->data[response->size] = 0;
    }
    
    return realsize;
}

/**
 * @brief Create new AI configuration
 */
ai_config_t *
ai_config_new(ai_provider_t provider, const gchar *api_key)
{
    ai_config_t *config = g_malloc0(sizeof(ai_config_t));
    config->provider = provider;
    config->api_key = g_strdup(api_key);
    config->timeout = 30; // Default 30 seconds
    config->enabled = TRUE;
    
    // Set default endpoints and models
    switch (provider) {
        case AI_PROVIDER_OPENAI:
            config->endpoint = g_strdup("https://api.openai.com/v1/chat/completions");
            config->model = g_strdup("gpt-4");
            break;
        case AI_PROVIDER_CLAUDE:
            config->endpoint = g_strdup("https://api.anthropic.com/v1/messages");
            config->model = g_strdup("claude-3-sonnet-20240229");
            break;
        case AI_PROVIDER_CUSTOM:
        case AI_PROVIDER_LOCAL:
            config->endpoint = g_strdup("http://localhost:8080/v1/chat/completions");
            config->model = g_strdup("local-model");
            break;
    }
    
    return config;
}

/**
 * @brief Free AI configuration
 */
void
ai_config_free(ai_config_t *config)
{
    if (!config) return;
    
    g_free(config->api_key);
    g_free(config->endpoint);
    g_free(config->model);
    g_free(config);
}

/**
 * @brief Create new AI request
 */
ai_request_t *
ai_request_new(ai_task_type_t task_type, JsonObject *input_data)
{
    ai_request_t *request = g_malloc0(sizeof(ai_request_t));
    request->task_type = task_type;
    request->input_data = json_object_ref(input_data);
    return request;
}

/**
 * @brief Free AI request
 */
void
ai_request_free(ai_request_t *request)
{
    if (!request) return;
    
    if (request->input_data)
        json_object_unref(request->input_data);
    g_free(request->context);
    g_free(request);
}

/**
 * @brief Create new AI response
 */
ai_response_t *
ai_response_new(void)
{
    ai_response_t *response = g_malloc0(sizeof(ai_response_t));
    response->success = FALSE;
    response->confidence_score = 0.0;
    response->processing_time_ms = 0;
    return response;
}

/**
 * @brief Free AI response
 */
void
ai_response_free(ai_response_t *response)
{
    if (!response) return;
    
    if (response->result)
        json_object_unref(response->result);
    g_free(response->error_message);
    g_free(response);
}

/**
 * @brief Initialize AI service
 */
gboolean
ai_service_init(void)
{
    if (ai_service_initialized)
        return TRUE;
    
    // Initialize CURL
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        g_warning("Failed to initialize CURL");
        return FALSE;
    }
    
    // Initialize provider configurations hash table
    provider_configs = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                           NULL, (GDestroyNotify)ai_config_free);
    
    // Create thread pool for async processing
    GError *error = NULL;
    ai_thread_pool = g_thread_pool_new((GFunc)ai_service_process_sync, NULL, 
                                      4, FALSE, &error);
    if (error) {
        g_warning("Failed to create AI thread pool: %s", error->message);
        g_error_free(error);
        return FALSE;
    }
    
    ai_service_initialized = TRUE;
    g_message("AI Service initialized successfully");
    return TRUE;
}

/**
 * @brief Cleanup AI service
 */
void
ai_service_cleanup(void)
{
    if (!ai_service_initialized)
        return;
    
    if (ai_thread_pool) {
        g_thread_pool_free(ai_thread_pool, FALSE, TRUE);
        ai_thread_pool = NULL;
    }
    
    if (provider_configs) {
        g_hash_table_destroy(provider_configs);
        provider_configs = NULL;
    }
    
    curl_global_cleanup();
    ai_service_initialized = FALSE;
    g_message("AI Service cleaned up");
}

/**
 * @brief Check if AI provider is available
 */
gboolean
ai_service_is_available(ai_provider_t provider)
{
    if (!ai_service_initialized)
        return FALSE;
    
    ai_config_t *config = g_hash_table_lookup(provider_configs, 
                                            GINT_TO_POINTER(provider));
    return config && config->enabled;
}

/**
 * @brief Convert AI provider to string
 */
const gchar *
ai_provider_to_string(ai_provider_t provider)
{
    switch (provider) {
        case AI_PROVIDER_OPENAI: return "OpenAI";
        case AI_PROVIDER_CLAUDE: return "Claude";
        case AI_PROVIDER_CUSTOM: return "Custom";
        case AI_PROVIDER_LOCAL: return "Local";
        default: return "Unknown";
    }
}

/**
 * @brief Convert AI task type to string
 */
const gchar *
ai_task_type_to_string(ai_task_type_t task_type)
{
    switch (task_type) {
        case AI_TASK_VULNERABILITY_ANALYSIS: return "Vulnerability Analysis";
        case AI_TASK_THREAT_MODELING: return "Threat Modeling";
        case AI_TASK_SCAN_OPTIMIZATION: return "Scan Optimization";
        case AI_TASK_REPORT_GENERATION: return "Report Generation";
        case AI_TASK_EXPLOIT_SUGGESTION: return "Exploit Suggestion";
        default: return "Unknown";
    }
}