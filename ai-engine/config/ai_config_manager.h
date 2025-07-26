/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Configuration Manager - Production-ready configuration system
 */

#ifndef AI_CONFIG_MANAGER_H
#define AI_CONFIG_MANAGER_H

#include <glib.h>
#include "../api/ai_service.h"

/**
 * @brief Configuration file locations
 */
#define AI_SYSTEM_CONFIG_FILE "/etc/ai-enhanced-openvas/ai-config.conf"
#define AI_USER_CONFIG_FILE "~/.config/ai-enhanced-openvas/ai-config.conf"
#define AI_LOCAL_CONFIG_FILE "./ai-config.conf"

/**
 * @brief Global configuration structure
 */
typedef struct {
    // Service settings
    gboolean service_enabled;
    guint thread_pool_size;
    gint default_timeout;
    
    // Cache settings
    gboolean cache_enabled;
    guint cache_max_entries;
    gint64 cache_default_ttl;
    
    // Rate limiting
    gboolean rate_limit_enabled;
    guint rate_limit_requests_per_minute;
    
    // Security settings
    gboolean encrypt_config;
    gboolean sanitize_data;
    gboolean audit_enabled;
    gchar *log_level;
    
    // Provider configurations
    GHashTable *provider_configs;
    
    // Feature flags
    gboolean vulnerability_analysis_enabled;
    gboolean threat_modeling_enabled;
    gboolean scan_optimization_enabled;
    gboolean report_generation_enabled;
    gboolean exploit_suggestion_enabled;
    
} ai_global_config_t;

/* Configuration Management Functions */
gboolean ai_config_manager_init(void);
void ai_config_manager_cleanup(void);

gboolean ai_config_load_from_file(const gchar *config_file);
gboolean ai_config_load_from_env(void);
gboolean ai_config_load_defaults(void);
gboolean ai_config_save_to_file(const gchar *config_file);

ai_global_config_t *ai_config_get_global(void);
gboolean ai_config_validate_global(ai_global_config_t *config);

/* Provider Configuration */
gboolean ai_config_add_provider(ai_provider_t provider, ai_config_t *config);
ai_config_t *ai_config_get_provider(ai_provider_t provider);
gboolean ai_config_remove_provider(ai_provider_t provider);
GList *ai_config_list_providers(void);

/* Configuration Utilities */
gchar *ai_config_to_json(void);
gboolean ai_config_from_json(const gchar *json_data);
gboolean ai_config_merge(ai_global_config_t *base, ai_global_config_t *override);

/* Environment Variable Helpers */
gchar *ai_config_get_env_string(const gchar *var_name, const gchar *default_value);
gint ai_config_get_env_int(const gchar *var_name, gint default_value);
gboolean ai_config_get_env_boolean(const gchar *var_name, gboolean default_value);

/* Configuration Validation */
gboolean ai_config_validate_api_key(const gchar *api_key, ai_provider_t provider);
gboolean ai_config_validate_endpoint(const gchar *endpoint);
gboolean ai_config_validate_model(const gchar *model, ai_provider_t provider);

/* Security Functions */
gchar *ai_config_encrypt_sensitive_data(const gchar *data);
gchar *ai_config_decrypt_sensitive_data(const gchar *encrypted_data);
gboolean ai_config_secure_store_api_key(ai_provider_t provider, const gchar *api_key);
gchar *ai_config_secure_retrieve_api_key(ai_provider_t provider);

#endif /* AI_CONFIG_MANAGER_H */