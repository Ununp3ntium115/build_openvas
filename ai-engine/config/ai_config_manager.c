/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Configuration Manager Implementation
 */

#include "ai_config_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global configuration instance */
static ai_global_config_t *global_config = NULL;
static GMutex config_mutex;
static gboolean config_manager_initialized = FALSE;

/**
 * @brief Initialize configuration manager
 */
gboolean
ai_config_manager_init(void)
{
    if (config_manager_initialized) {
        return TRUE;
    }
    
    g_mutex_init(&config_mutex);
    
    global_config = g_malloc0(sizeof(ai_global_config_t));
    global_config->provider_configs = g_hash_table_new_full(g_direct_hash, g_direct_equal,
                                                           NULL, (GDestroyNotify)ai_config_free);
    
    // Load default configuration
    ai_config_load_defaults();
    
    // Try to load from configuration files (system -> user -> local)
    ai_config_load_from_file(AI_SYSTEM_CONFIG_FILE);
    
    gchar *user_config = g_strdup_printf("%s/%s", g_get_home_dir(), 
                                        ".config/ai-enhanced-openvas/ai-config.conf");
    ai_config_load_from_file(user_config);
    g_free(user_config);
    
    ai_config_load_from_file(AI_LOCAL_CONFIG_FILE);
    
    // Override with environment variables
    ai_config_load_from_env();
    
    config_manager_initialized = TRUE;
    g_message("AI Configuration Manager initialized successfully");
    return TRUE;
}

/**
 * @brief Cleanup configuration manager
 */
void
ai_config_manager_cleanup(void)
{
    if (!config_manager_initialized) {
        return;
    }
    
    g_mutex_lock(&config_mutex);
    
    if (global_config) {
        if (global_config->provider_configs) {
            g_hash_table_destroy(global_config->provider_configs);
        }
        g_free(global_config->log_level);
        g_free(global_config);
        global_config = NULL;
    }
    
    g_mutex_unlock(&config_mutex);
    g_mutex_clear(&config_mutex);
    
    config_manager_initialized = FALSE;
    g_message("AI Configuration Manager cleaned up");
}

/**
 * @brief Load default configuration values
 */
gboolean
ai_config_load_defaults(void)
{
    if (!global_config) return FALSE;
    
    g_mutex_lock(&config_mutex);
    
    // Service defaults
    global_config->service_enabled = TRUE;
    global_config->thread_pool_size = 8;
    global_config->default_timeout = 30;
    
    // Cache defaults
    global_config->cache_enabled = TRUE;
    global_config->cache_max_entries = 1000;
    global_config->cache_default_ttl = 3600;  // 1 hour
    
    // Rate limiting defaults
    global_config->rate_limit_enabled = TRUE;
    global_config->rate_limit_requests_per_minute = 60;
    
    // Security defaults
    global_config->encrypt_config = FALSE;
    global_config->sanitize_data = TRUE;
    global_config->audit_enabled = TRUE;
    global_config->log_level = g_strdup("INFO");
    
    // Feature flags defaults
    global_config->vulnerability_analysis_enabled = TRUE;
    global_config->threat_modeling_enabled = TRUE;
    global_config->scan_optimization_enabled = TRUE;
    global_config->report_generation_enabled = TRUE;
    global_config->exploit_suggestion_enabled = FALSE;  // Disabled by default for security
    
    g_mutex_unlock(&config_mutex);
    
    g_message("Default AI configuration loaded");
    return TRUE;
}

/**
 * @brief Load configuration from file
 */
gboolean
ai_config_load_from_file(const gchar *config_file)
{
    if (!config_file || !global_config) return FALSE;
    
    GKeyFile *keyfile = g_key_file_new();
    GError *error = NULL;
    
    if (!g_key_file_load_from_file(keyfile, config_file, G_KEY_FILE_NONE, &error)) {
        if (error->code != G_FILE_ERROR_NOENT) {  // Don't warn about missing files
            g_warning("Failed to load config file %s: %s", config_file, error->message);
        }
        g_error_free(error);
        g_key_file_free(keyfile);
        return FALSE;
    }
    
    g_mutex_lock(&config_mutex);
    
    // Load service settings
    if (g_key_file_has_group(keyfile, "service")) {
        global_config->service_enabled = g_key_file_get_boolean(keyfile, "service", "enabled", NULL);
        global_config->thread_pool_size = g_key_file_get_integer(keyfile, "service", "thread_pool_size", NULL);
        global_config->default_timeout = g_key_file_get_integer(keyfile, "service", "default_timeout", NULL);
    }
    
    // Load cache settings
    if (g_key_file_has_group(keyfile, "cache")) {
        global_config->cache_enabled = g_key_file_get_boolean(keyfile, "cache", "enabled", NULL);
        global_config->cache_max_entries = g_key_file_get_integer(keyfile, "cache", "max_entries", NULL);
        global_config->cache_default_ttl = g_key_file_get_int64(keyfile, "cache", "default_ttl", NULL);
    }
    
    // Load rate limiting settings
    if (g_key_file_has_group(keyfile, "rate_limiting")) {
        global_config->rate_limit_enabled = g_key_file_get_boolean(keyfile, "rate_limiting", "enabled", NULL);
        global_config->rate_limit_requests_per_minute = g_key_file_get_integer(keyfile, "rate_limiting", "requests_per_minute", NULL);
    }
    
    // Load security settings
    if (g_key_file_has_group(keyfile, "security")) {
        global_config->encrypt_config = g_key_file_get_boolean(keyfile, "security", "encrypt_config", NULL);
        global_config->sanitize_data = g_key_file_get_boolean(keyfile, "security", "sanitize_data", NULL);
        global_config->audit_enabled = g_key_file_get_boolean(keyfile, "security", "audit_enabled", NULL);
        
        gchar *log_level = g_key_file_get_string(keyfile, "security", "log_level", NULL);
        if (log_level) {
            g_free(global_config->log_level);
            global_config->log_level = log_level;
        }
    }
    
    // Load feature flags
    if (g_key_file_has_group(keyfile, "features")) {
        global_config->vulnerability_analysis_enabled = g_key_file_get_boolean(keyfile, "features", "vulnerability_analysis", NULL);
        global_config->threat_modeling_enabled = g_key_file_get_boolean(keyfile, "features", "threat_modeling", NULL);
        global_config->scan_optimization_enabled = g_key_file_get_boolean(keyfile, "features", "scan_optimization", NULL);
        global_config->report_generation_enabled = g_key_file_get_boolean(keyfile, "features", "report_generation", NULL);
        global_config->exploit_suggestion_enabled = g_key_file_get_boolean(keyfile, "features", "exploit_suggestion", NULL);
    }
    
    // Load AI provider configurations
    if (g_key_file_has_group(keyfile, "ai_providers")) {
        // OpenAI configuration
        if (g_key_file_get_boolean(keyfile, "ai_providers", "openai_enabled", NULL)) {
            gchar *api_key = g_key_file_get_string(keyfile, "ai_providers", "openai_api_key", NULL);
            gchar *model = g_key_file_get_string(keyfile, "ai_providers", "openai_model", NULL);
            gchar *endpoint = g_key_file_get_string(keyfile, "ai_providers", "openai_endpoint", NULL);
            gint timeout = g_key_file_get_integer(keyfile, "ai_providers", "openai_timeout", NULL);
            
            if (api_key) {
                ai_config_t *config = ai_config_new(AI_PROVIDER_OPENAI, api_key);
                if (model) ai_config_set_model(config, model);
                if (endpoint) ai_config_set_endpoint(config, endpoint);
                if (timeout > 0) ai_config_set_timeout(config, timeout);
                
                g_hash_table_insert(global_config->provider_configs, 
                                  GINT_TO_POINTER(AI_PROVIDER_OPENAI), config);
            }
            
            g_free(api_key);
            g_free(model);
            g_free(endpoint);
        }
        
        // Claude configuration
        if (g_key_file_get_boolean(keyfile, "ai_providers", "claude_enabled", NULL)) {
            gchar *api_key = g_key_file_get_string(keyfile, "ai_providers", "claude_api_key", NULL);
            gchar *model = g_key_file_get_string(keyfile, "ai_providers", "claude_model", NULL);
            gchar *endpoint = g_key_file_get_string(keyfile, "ai_providers", "claude_endpoint", NULL);
            gint timeout = g_key_file_get_integer(keyfile, "ai_providers", "claude_timeout", NULL);
            
            if (api_key) {
                ai_config_t *config = ai_config_new(AI_PROVIDER_CLAUDE, api_key);
                if (model) ai_config_set_model(config, model);
                if (endpoint) ai_config_set_endpoint(config, endpoint);
                if (timeout > 0) ai_config_set_timeout(config, timeout);
                
                g_hash_table_insert(global_config->provider_configs, 
                                  GINT_TO_POINTER(AI_PROVIDER_CLAUDE), config);
            }
            
            g_free(api_key);
            g_free(model);
            g_free(endpoint);
        }
    }
    
    g_mutex_unlock(&config_mutex);
    g_key_file_free(keyfile);
    
    g_message("Configuration loaded from file: %s", config_file);
    return TRUE;
}

/**
 * @brief Load configuration from environment variables
 */
gboolean
ai_config_load_from_env(void)
{
    if (!global_config) return FALSE;
    
    g_mutex_lock(&config_mutex);
    
    // Service settings
    global_config->service_enabled = ai_config_get_env_boolean("AI_SERVICE_ENABLED", global_config->service_enabled);
    global_config->thread_pool_size = ai_config_get_env_int("AI_THREAD_POOL_SIZE", global_config->thread_pool_size);
    global_config->default_timeout = ai_config_get_env_int("AI_DEFAULT_TIMEOUT", global_config->default_timeout);
    
    // Cache settings
    global_config->cache_enabled = ai_config_get_env_boolean("AI_CACHE_ENABLED", global_config->cache_enabled);
    global_config->cache_max_entries = ai_config_get_env_int("AI_CACHE_MAX_ENTRIES", global_config->cache_max_entries);
    global_config->cache_default_ttl = ai_config_get_env_int("AI_CACHE_DEFAULT_TTL", global_config->cache_default_ttl);
    
    // Rate limiting
    global_config->rate_limit_enabled = ai_config_get_env_boolean("AI_RATE_LIMIT_ENABLED", global_config->rate_limit_enabled);
    global_config->rate_limit_requests_per_minute = ai_config_get_env_int("AI_RATE_LIMIT_RPM", global_config->rate_limit_requests_per_minute);
    
    // Security settings
    global_config->encrypt_config = ai_config_get_env_boolean("AI_ENCRYPT_CONFIG", global_config->encrypt_config);
    global_config->sanitize_data = ai_config_get_env_boolean("AI_SANITIZE_DATA", global_config->sanitize_data);
    global_config->audit_enabled = ai_config_get_env_boolean("AI_AUDIT_ENABLED", global_config->audit_enabled);
    
    gchar *log_level = ai_config_get_env_string("AI_LOG_LEVEL", NULL);
    if (log_level) {
        g_free(global_config->log_level);
        global_config->log_level = log_level;
    }
    
    // Feature flags
    global_config->vulnerability_analysis_enabled = ai_config_get_env_boolean("AI_VULN_ANALYSIS_ENABLED", global_config->vulnerability_analysis_enabled);
    global_config->threat_modeling_enabled = ai_config_get_env_boolean("AI_THREAT_MODELING_ENABLED", global_config->threat_modeling_enabled);
    global_config->scan_optimization_enabled = ai_config_get_env_boolean("AI_SCAN_OPTIMIZATION_ENABLED", global_config->scan_optimization_enabled);
    global_config->report_generation_enabled = ai_config_get_env_boolean("AI_REPORT_GENERATION_ENABLED", global_config->report_generation_enabled);
    global_config->exploit_suggestion_enabled = ai_config_get_env_boolean("AI_EXPLOIT_SUGGESTION_ENABLED", global_config->exploit_suggestion_enabled);
    
    // OpenAI provider from environment
    gchar *openai_key = ai_config_get_env_string("OPENAI_API_KEY", NULL);
    if (openai_key) {
        ai_config_t *config = ai_config_new(AI_PROVIDER_OPENAI, openai_key);
        
        gchar *model = ai_config_get_env_string("OPENAI_MODEL", NULL);
        if (model) ai_config_set_model(config, model);
        
        gchar *endpoint = ai_config_get_env_string("OPENAI_ENDPOINT", NULL);
        if (endpoint) ai_config_set_endpoint(config, endpoint);
        
        gint timeout = ai_config_get_env_int("OPENAI_TIMEOUT", 0);
        if (timeout > 0) ai_config_set_timeout(config, timeout);
        
        g_hash_table_insert(global_config->provider_configs, 
                          GINT_TO_POINTER(AI_PROVIDER_OPENAI), config);
        
        g_free(model);
        g_free(endpoint);
    }
    
    // Claude provider from environment
    gchar *claude_key = ai_config_get_env_string("ANTHROPIC_API_KEY", NULL);
    if (claude_key) {
        ai_config_t *config = ai_config_new(AI_PROVIDER_CLAUDE, claude_key);
        
        gchar *model = ai_config_get_env_string("CLAUDE_MODEL", NULL);
        if (model) ai_config_set_model(config, model);
        
        gchar *endpoint = ai_config_get_env_string("CLAUDE_ENDPOINT", NULL);
        if (endpoint) ai_config_set_endpoint(config, endpoint);
        
        gint timeout = ai_config_get_env_int("CLAUDE_TIMEOUT", 0);
        if (timeout > 0) ai_config_set_timeout(config, timeout);
        
        g_hash_table_insert(global_config->provider_configs, 
                          GINT_TO_POINTER(AI_PROVIDER_CLAUDE), config);
        
        g_free(model);
        g_free(endpoint);
    }
    
    g_mutex_unlock(&config_mutex);
    
    g_message("Configuration loaded from environment variables");
    return TRUE;
}

/**
 * @brief Get global configuration
 */
ai_global_config_t *
ai_config_get_global(void)
{
    return global_config;
}

/**
 * @brief Get environment variable as string
 */
gchar *
ai_config_get_env_string(const gchar *var_name, const gchar *default_value)
{
    const gchar *value = g_getenv(var_name);
    return g_strdup(value ? value : default_value);
}

/**
 * @brief Get environment variable as integer
 */
gint
ai_config_get_env_int(const gchar *var_name, gint default_value)
{
    const gchar *value = g_getenv(var_name);
    return value ? atoi(value) : default_value;
}

/**
 * @brief Get environment variable as boolean
 */
gboolean
ai_config_get_env_boolean(const gchar *var_name, gboolean default_value)
{
    const gchar *value = g_getenv(var_name);
    if (!value) return default_value;
    
    return (g_ascii_strcasecmp(value, "true") == 0 ||
            g_ascii_strcasecmp(value, "yes") == 0 ||
            g_ascii_strcasecmp(value, "1") == 0);
}

/**
 * @brief Validate API key format for specific provider
 */
gboolean
ai_config_validate_api_key(const gchar *api_key, ai_provider_t provider)
{
    if (!api_key || strlen(api_key) == 0) {
        return FALSE;
    }
    
    switch (provider) {
        case AI_PROVIDER_OPENAI:
            return g_str_has_prefix(api_key, "sk-") && strlen(api_key) > 10;
        case AI_PROVIDER_CLAUDE:
            return g_str_has_prefix(api_key, "sk-ant-") && strlen(api_key) > 20;
        case AI_PROVIDER_CUSTOM:
        case AI_PROVIDER_LOCAL:
            return strlen(api_key) > 0;  // Less strict for custom providers
        default:
            return FALSE;
    }
}

/**
 * @brief Validate endpoint URL
 */
gboolean
ai_config_validate_endpoint(const gchar *endpoint)
{
    if (!endpoint) return FALSE;
    
    return (g_str_has_prefix(endpoint, "https://") || 
            g_str_has_prefix(endpoint, "http://localhost") ||
            g_str_has_prefix(endpoint, "http://127.0.0.1"));
}

/**
 * @brief Add provider configuration
 */
gboolean
ai_config_add_provider(ai_provider_t provider, ai_config_t *config)
{
    if (!global_config || !config) return FALSE;
    
    g_mutex_lock(&config_mutex);
    
    if (ai_config_validate(config)) {
        g_hash_table_insert(global_config->provider_configs, 
                          GINT_TO_POINTER(provider), config);
        g_mutex_unlock(&config_mutex);
        return TRUE;
    }
    
    g_mutex_unlock(&config_mutex);
    return FALSE;
}

/**
 * @brief Get provider configuration
 */
ai_config_t *
ai_config_get_provider(ai_provider_t provider)
{
    if (!global_config) return NULL;
    
    g_mutex_lock(&config_mutex);
    ai_config_t *config = g_hash_table_lookup(global_config->provider_configs, 
                                            GINT_TO_POINTER(provider));
    g_mutex_unlock(&config_mutex);
    
    return config;
}