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
static GHashTable *response_cache = NULL;
static GHashTable *rate_limiters = NULL;
static ai_metrics_t *global_metrics = NULL;
static GMutex service_mutex;
static GMutex cache_mutex;
static GMutex metrics_mutex;

/* Configuration constants */
#define AI_DEFAULT_CACHE_SIZE 1000
#define AI_DEFAULT_CACHE_TTL 3600  // 1 hour
#define AI_DEFAULT_RATE_LIMIT 60   // 60 requests per minute
#define AI_MAX_RETRY_ATTEMPTS 3
#define AI_RETRY_DELAY_MS 1000

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
 * @brief Validate AI configuration
 */
gboolean
ai_config_validate(ai_config_t *config)
{
    if (!config) {
        g_warning("AI configuration is NULL");
        return FALSE;
    }
    
    if (!config->api_key || strlen(config->api_key) == 0) {
        g_warning("AI configuration missing API key");
        return FALSE;
    }
    
    if (!config->endpoint || strlen(config->endpoint) == 0) {
        g_warning("AI configuration missing endpoint");
        return FALSE;
    }
    
    if (!config->model || strlen(config->model) == 0) {
        g_warning("AI configuration missing model");
        return FALSE;
    }
    
    if (config->timeout <= 0) {
        g_warning("AI configuration has invalid timeout: %d", config->timeout);
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Set timeout for AI configuration
 */
void
ai_config_set_timeout(ai_config_t *config, gint timeout_seconds)
{
    if (config && timeout_seconds > 0) {
        config->timeout = timeout_seconds;
    }
}

/**
 * @brief Set model for AI configuration
 */
void
ai_config_set_model(ai_config_t *config, const gchar *model)
{
    if (config && model) {
        g_free(config->model);
        config->model = g_strdup(model);
    }
}

/**
 * @brief Set endpoint for AI configuration
 */
void
ai_config_set_endpoint(ai_config_t *config, const gchar *endpoint)
{
    if (config && endpoint) {
        g_free(config->endpoint);
        config->endpoint = g_strdup(endpoint);
    }
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
    
    // Initialize mutexes
    g_mutex_init(&service_mutex);
    g_mutex_init(&cache_mutex);
    g_mutex_init(&metrics_mutex);
    
    // Initialize CURL
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        g_warning("Failed to initialize CURL");
        return FALSE;
    }
    
    // Initialize provider configurations hash table
    provider_configs = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                           NULL, (GDestroyNotify)ai_config_free);
    
    // Initialize rate limiters
    rate_limiters = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                        NULL, g_free);
    
    // Initialize metrics
    global_metrics = g_malloc0(sizeof(ai_metrics_t));
    global_metrics->last_request_timestamp = ai_get_timestamp_ms();
    
    // Initialize cache
    if (!ai_cache_init(AI_DEFAULT_CACHE_SIZE, AI_DEFAULT_CACHE_TTL)) {
        g_warning("Failed to initialize AI response cache");
        return FALSE;
    }
    
    // Initialize rate limiting
    if (!ai_rate_limit_init(AI_DEFAULT_RATE_LIMIT)) {
        g_warning("Failed to initialize AI rate limiting");
        return FALSE;
    }
    
    // Create thread pool for async processing
    GError *error = NULL;
    ai_thread_pool = g_thread_pool_new((GFunc)ai_service_process_sync, NULL, 
                                      8, FALSE, &error);  // Increased to 8 threads
    if (error) {
        g_warning("Failed to create AI thread pool: %s", error->message);
        g_error_free(error);
        return FALSE;
    }
    
    ai_service_initialized = TRUE;
    g_message("AI Service initialized successfully with enhanced features");
    return TRUE;
}

/**
 * @brief Initialize AI service with configuration file
 */
gboolean
ai_service_init_with_config(const gchar *config_file)
{
    if (!ai_service_init()) {
        return FALSE;
    }
    
    // Load configuration from file
    GKeyFile *keyfile = g_key_file_new();
    GError *error = NULL;
    
    if (!g_key_file_load_from_file(keyfile, config_file, G_KEY_FILE_NONE, &error)) {
        g_warning("Failed to load AI config file %s: %s", config_file, error->message);
        g_error_free(error);
        g_key_file_free(keyfile);
        return FALSE;
    }
    
    // Configure OpenAI provider if enabled
    if (g_key_file_get_boolean(keyfile, "ai_providers", "openai_enabled", NULL)) {
        gchar *api_key = g_key_file_get_string(keyfile, "ai_providers", "openai_api_key", NULL);
        gchar *model = g_key_file_get_string(keyfile, "ai_providers", "openai_model", NULL);
        
        if (api_key) {
            ai_config_t *config = ai_config_new(AI_PROVIDER_OPENAI, api_key);
            if (model) {
                ai_config_set_model(config, model);
            }
            ai_service_register_provider(AI_PROVIDER_OPENAI, config);
            g_message("OpenAI provider configured from config file");
        }
        
        g_free(api_key);
        g_free(model);
    }
    
    // Configure Claude provider if enabled
    if (g_key_file_get_boolean(keyfile, "ai_providers", "claude_enabled", NULL)) {
        gchar *api_key = g_key_file_get_string(keyfile, "ai_providers", "claude_api_key", NULL);
        gchar *model = g_key_file_get_string(keyfile, "ai_providers", "claude_model", NULL);
        
        if (api_key) {
            ai_config_t *config = ai_config_new(AI_PROVIDER_CLAUDE, api_key);
            if (model) {
                ai_config_set_model(config, model);
            }
            ai_service_register_provider(AI_PROVIDER_CLAUDE, config);
            g_message("Claude provider configured from config file");
        }
        
        g_free(api_key);
        g_free(model);
    }
    
    g_key_file_free(keyfile);
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
/
**
 * @brief Initialize AI response cache
 */
gboolean
ai_cache_init(guint max_entries, gint64 default_ttl)
{
    g_mutex_lock(&cache_mutex);
    
    if (response_cache) {
        g_hash_table_destroy(response_cache);
    }
    
    response_cache = g_hash_table_new_full(g_str_hash, g_str_equal,
                                         g_free, (GDestroyNotify)ai_cache_entry_free);
    
    g_mutex_unlock(&cache_mutex);
    g_message("AI response cache initialized with max %u entries, TTL %ld seconds", 
              max_entries, default_ttl);
    return TRUE;
}

/**
 * @brief Cleanup AI response cache
 */
void
ai_cache_cleanup(void)
{
    g_mutex_lock(&cache_mutex);
    
    if (response_cache) {
        g_hash_table_destroy(response_cache);
        response_cache = NULL;
    }
    
    g_mutex_unlock(&cache_mutex);
    g_message("AI response cache cleaned up");
}

/**
 * @brief Free cache entry
 */
static void
ai_cache_entry_free(ai_cache_entry_t *entry)
{
    if (!entry) return;
    
    g_free(entry->key);
    ai_response_free(entry->response);
    g_free(entry);
}

/**
 * @brief Check if cache entry is expired
 */
static gboolean
ai_cache_entry_expired(ai_cache_entry_t *entry)
{
    if (!entry) return TRUE;
    
    gint64 current_time = ai_get_timestamp_ms() / 1000;  // Convert to seconds
    return (current_time - entry->timestamp) > entry->ttl_seconds;
}

/**
 * @brief Get response from cache
 */
ai_response_t *
ai_cache_get(const gchar *key)
{
    if (!key || !response_cache) return NULL;
    
    g_mutex_lock(&cache_mutex);
    
    ai_cache_entry_t *entry = g_hash_table_lookup(response_cache, key);
    if (!entry || ai_cache_entry_expired(entry)) {
        if (entry) {
            g_hash_table_remove(response_cache, key);
            g_mutex_lock(&metrics_mutex);
            global_metrics->cache_misses++;
            g_mutex_unlock(&metrics_mutex);
        }
        g_mutex_unlock(&cache_mutex);
        return NULL;
    }
    
    ai_response_t *response = ai_response_copy(entry->response);
    
    g_mutex_lock(&metrics_mutex);
    global_metrics->cache_hits++;
    g_mutex_unlock(&metrics_mutex);
    
    g_mutex_unlock(&cache_mutex);
    return response;
}

/**
 * @brief Set response in cache
 */
void
ai_cache_set(const gchar *key, ai_response_t *response, gint64 ttl)
{
    if (!key || !response || !response_cache) return;
    
    g_mutex_lock(&cache_mutex);
    
    ai_cache_entry_t *entry = g_malloc0(sizeof(ai_cache_entry_t));
    entry->key = g_strdup(key);
    entry->response = ai_response_copy(response);
    entry->timestamp = ai_get_timestamp_ms() / 1000;  // Convert to seconds
    entry->ttl_seconds = ttl > 0 ? ttl : AI_DEFAULT_CACHE_TTL;
    
    g_hash_table_replace(response_cache, g_strdup(key), entry);
    
    g_mutex_unlock(&cache_mutex);
}

/**
 * @brief Invalidate cache entry
 */
void
ai_cache_invalidate(const gchar *key)
{
    if (!key || !response_cache) return;
    
    g_mutex_lock(&cache_mutex);
    g_hash_table_remove(response_cache, key);
    g_mutex_unlock(&cache_mutex);
}

/**
 * @brief Clear entire cache
 */
void
ai_cache_clear(void)
{
    if (!response_cache) return;
    
    g_mutex_lock(&cache_mutex);
    g_hash_table_remove_all(response_cache);
    g_mutex_unlock(&cache_mutex);
    
    g_message("AI response cache cleared");
}

/**
 * @brief Get cache size
 */
guint
ai_cache_size(void)
{
    if (!response_cache) return 0;
    
    g_mutex_lock(&cache_mutex);
    guint size = g_hash_table_size(response_cache);
    g_mutex_unlock(&cache_mutex);
    
    return size;
}

/**
 * @brief Initialize rate limiting
 */
gboolean
ai_rate_limit_init(guint requests_per_minute)
{
    // Initialize rate limiters for each provider
    for (ai_provider_t provider = AI_PROVIDER_OPENAI; provider <= AI_PROVIDER_LOCAL; provider++) {
        ai_rate_limit_t *limiter = g_malloc0(sizeof(ai_rate_limit_t));
        limiter->requests_per_minute = requests_per_minute;
        limiter->current_requests = 0;
        limiter->window_start_time = ai_get_timestamp_ms();
        limiter->enabled = TRUE;
        
        g_hash_table_insert(rate_limiters, GINT_TO_POINTER(provider), limiter);
    }
    
    g_message("AI rate limiting initialized: %u requests per minute", requests_per_minute);
    return TRUE;
}

/**
 * @brief Cleanup rate limiting
 */
void
ai_rate_limit_cleanup(void)
{
    if (rate_limiters) {
        g_hash_table_destroy(rate_limiters);
        rate_limiters = NULL;
    }
    
    g_message("AI rate limiting cleaned up");
}

/**
 * @brief Check rate limit for provider
 */
gboolean
ai_rate_limit_check(ai_provider_t provider)
{
    if (!rate_limiters) return TRUE;
    
    ai_rate_limit_t *limiter = g_hash_table_lookup(rate_limiters, GINT_TO_POINTER(provider));
    if (!limiter || !limiter->enabled) return TRUE;
    
    gint64 current_time = ai_get_timestamp_ms();
    gint64 window_duration = 60 * 1000;  // 1 minute in milliseconds
    
    // Reset window if needed
    if (current_time - limiter->window_start_time >= window_duration) {
        limiter->current_requests = 0;
        limiter->window_start_time = current_time;
    }
    
    // Check if we're within limits
    if (limiter->current_requests >= limiter->requests_per_minute) {
        g_warning("Rate limit exceeded for provider %s: %u/%u requests",
                 ai_provider_to_string(provider), 
                 limiter->current_requests, 
                 limiter->requests_per_minute);
        return FALSE;
    }
    
    limiter->current_requests++;
    return TRUE;
}

/**
 * @brief Get remaining requests for provider
 */
guint
ai_rate_limit_remaining(ai_provider_t provider)
{
    if (!rate_limiters) return 0;
    
    ai_rate_limit_t *limiter = g_hash_table_lookup(rate_limiters, GINT_TO_POINTER(provider));
    if (!limiter) return 0;
    
    return limiter->requests_per_minute - limiter->current_requests;
}

/**
 * @brief Get current metrics
 */
ai_metrics_t *
ai_metrics_get(void)
{
    return global_metrics;
}

/**
 * @brief Record request metrics
 */
void
ai_metrics_record_request(ai_provider_t provider, gboolean success, gdouble response_time)
{
    if (!global_metrics) return;
    
    g_mutex_lock(&metrics_mutex);
    
    global_metrics->total_requests++;
    global_metrics->last_request_timestamp = ai_get_timestamp_ms();
    global_metrics->total_processing_time_ms += response_time;
    global_metrics->average_response_time_ms = 
        global_metrics->total_processing_time_ms / global_metrics->total_requests;
    
    if (success) {
        global_metrics->successful_requests++;
    } else {
        global_metrics->failed_requests++;
    }
    
    g_mutex_unlock(&metrics_mutex);
}

/**
 * @brief Generate request cache key
 */
gchar *
ai_request_generate_cache_key(ai_request_t *request)
{
    if (!request) return NULL;
    
    JsonGenerator *generator = json_generator_new();
    JsonNode *root = json_node_new(JSON_NODE_OBJECT);
    json_node_set_object(root, request->input_data);
    json_generator_set_root(generator, root);
    gchar *json_str = json_generator_to_data(generator, NULL);
    
    // Create hash of task type + input data + context
    GChecksum *checksum = g_checksum_new(G_CHECKSUM_SHA256);
    g_checksum_update(checksum, (const guchar*)&request->task_type, sizeof(request->task_type));
    g_checksum_update(checksum, (const guchar*)json_str, strlen(json_str));
    
    if (request->context) {
        g_checksum_update(checksum, (const guchar*)request->context, strlen(request->context));
    }
    
    gchar *cache_key = g_strdup(g_checksum_get_string(checksum));
    
    // Cleanup
    g_checksum_free(checksum);
    g_free(json_str);
    json_node_free(root);
    g_object_unref(generator);
    
    return cache_key;
}

/**
 * @brief Copy AI response
 */
ai_response_t *
ai_response_copy(ai_response_t *response)
{
    if (!response) return NULL;
    
    ai_response_t *copy = ai_response_new();
    copy->success = response->success;
    copy->confidence_score = response->confidence_score;
    copy->processing_time_ms = response->processing_time_ms;
    
    if (response->result) {
        copy->result = json_object_ref(response->result);
    }
    
    if (response->error_message) {
        copy->error_message = g_strdup(response->error_message);
    }
    
    return copy;
}

/**
 * @brief Enhanced sync processing with caching and rate limiting
 */
ai_response_t *
ai_service_process_sync(ai_request_t *request)
{
    if (!request || !ai_service_initialized) {
        ai_response_t *response = ai_response_new();
        response->success = FALSE;
        response->error_message = g_strdup("Invalid request or service not initialized");
        return response;
    }
    
    // Check rate limiting
    ai_provider_t provider = request->config ? request->config->provider : AI_PROVIDER_OPENAI;
    if (!ai_rate_limit_check(provider)) {
        ai_response_t *response = ai_response_new();
        response->success = FALSE;
        response->error_message = g_strdup("Rate limit exceeded");
        ai_metrics_record_request(provider, FALSE, 0);
        return response;
    }
    
    // Check cache first
    gchar *cache_key = ai_request_generate_cache_key(request);
    ai_response_t *cached_response = ai_cache_get(cache_key);
    if (cached_response) {
        g_message("AI request served from cache: %s", cache_key);
        g_free(cache_key);
        return cached_response;
    }
    
    // Process request
    gint64 start_time = ai_get_timestamp_ms();
    ai_response_t *response = NULL;
    
    switch (provider) {
        case AI_PROVIDER_OPENAI:
            response = openai_provider_process(request);
            break;
        case AI_PROVIDER_CLAUDE:
            // TODO: Implement Claude provider
            response = ai_response_new();
            response->success = FALSE;
            response->error_message = g_strdup("Claude provider not yet implemented");
            break;
        default:
            response = ai_response_new();
            response->success = FALSE;
            response->error_message = g_strdup("Unsupported AI provider");
            break;
    }
    
    gint64 end_time = ai_get_timestamp_ms();
    gdouble processing_time = (gdouble)(end_time - start_time);
    
    // Record metrics
    ai_metrics_record_request(provider, response->success, processing_time);
    
    // Cache successful responses
    if (response->success) {
        ai_cache_set(cache_key, response, AI_DEFAULT_CACHE_TTL);
    }
    
    g_free(cache_key);
    return response;
}

/**
 * @brief Get current timestamp in milliseconds
 */
gint64
ai_get_timestamp_ms(void)
{
    return g_get_monotonic_time() / 1000;
}

/**
 * @brief Register AI provider
 */
void
ai_service_register_provider(ai_provider_t provider, ai_config_t *config)
{
    if (!provider_configs || !config) return;
    
    g_mutex_lock(&service_mutex);
    
    if (ai_config_validate(config)) {
        g_hash_table_insert(provider_configs, GINT_TO_POINTER(provider), config);
        g_message("AI provider %s registered successfully", ai_provider_to_string(provider));
    } else {
        g_warning("Failed to register AI provider %s: invalid configuration", 
                 ai_provider_to_string(provider));
        ai_config_free(config);
    }
    
    g_mutex_unlock(&service_mutex);
}

/**
 * @brief Health check for AI provider
 */
gboolean
ai_service_health_check(ai_provider_t provider)
{
    if (!ai_service_is_available(provider)) {
        return FALSE;
    }
    
    // Create simple test request
    JsonObject *test_data = json_object_new();
    json_object_set_string_member(test_data, "test", "health_check");
    
    ai_request_t *request = ai_request_new(AI_TASK_VULNERABILITY_ANALYSIS, test_data);
    ai_config_t *config = g_hash_table_lookup(provider_configs, GINT_TO_POINTER(provider));
    request->config = config;
    
    ai_response_t *response = ai_service_process_sync(request);
    gboolean healthy = response && response->success;
    
    // Cleanup
    ai_request_free(request);
    ai_response_free(response);
    json_object_unref(test_data);
    
    return healthy;
}