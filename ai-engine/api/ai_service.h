/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Service Interface for OpenVAS Integration
 */

#ifndef AI_SERVICE_H
#define AI_SERVICE_H

#include <glib.h>
#include <json-glib/json-glib.h>

/**
 * @brief AI Provider types supported
 */
typedef enum {
    AI_PROVIDER_OPENAI,
    AI_PROVIDER_CLAUDE,
    AI_PROVIDER_CUSTOM,
    AI_PROVIDER_LOCAL
} ai_provider_t;

/**
 * @brief AI Task types
 */
typedef enum {
    AI_TASK_VULNERABILITY_ANALYSIS,
    AI_TASK_THREAT_MODELING,
    AI_TASK_SCAN_OPTIMIZATION,
    AI_TASK_REPORT_GENERATION,
    AI_TASK_EXPLOIT_SUGGESTION
} ai_task_type_t;

/**
 * @brief AI Service configuration
 */
typedef struct {
    ai_provider_t provider;
    gchar *api_key;
    gchar *endpoint;
    gchar *model;
    gint timeout;
    gboolean enabled;
} ai_config_t;

/**
 * @brief AI Request structure
 */
typedef struct {
    ai_task_type_t task_type;
    JsonObject *input_data;
    gchar *context;
    ai_config_t *config;
} ai_request_t;

/**
 * @brief AI Response structure
 */
typedef struct {
    gboolean success;
    JsonObject *result;
    gchar *error_message;
    gdouble confidence_score;
    gint64 processing_time_ms;
} ai_response_t;

/**
 * @brief AI Service callback function type
 */
typedef void (*ai_callback_t)(ai_response_t *response, gpointer user_data);

/**
 * @brief AI Service metrics structure
 */
typedef struct {
    guint64 total_requests;
    guint64 successful_requests;
    guint64 failed_requests;
    gdouble average_response_time_ms;
    gdouble total_processing_time_ms;
    guint64 cache_hits;
    guint64 cache_misses;
    gint64 last_request_timestamp;
} ai_metrics_t;

/**
 * @brief AI Service rate limiting structure
 */
typedef struct {
    guint requests_per_minute;
    guint current_requests;
    gint64 window_start_time;
    gboolean enabled;
} ai_rate_limit_t;

/**
 * @brief AI Service cache entry
 */
typedef struct {
    gchar *key;
    ai_response_t *response;
    gint64 timestamp;
    gint64 ttl_seconds;
} ai_cache_entry_t;

/* Core AI Service Functions */
ai_config_t *ai_config_new(ai_provider_t provider, const gchar *api_key);
void ai_config_free(ai_config_t *config);
gboolean ai_config_validate(ai_config_t *config);
void ai_config_set_timeout(ai_config_t *config, gint timeout_seconds);
void ai_config_set_model(ai_config_t *config, const gchar *model);
void ai_config_set_endpoint(ai_config_t *config, const gchar *endpoint);

ai_request_t *ai_request_new(ai_task_type_t task_type, JsonObject *input_data);
void ai_request_free(ai_request_t *request);
void ai_request_set_context(ai_request_t *request, const gchar *context);
void ai_request_set_priority(ai_request_t *request, gint priority);
gchar *ai_request_generate_cache_key(ai_request_t *request);

ai_response_t *ai_response_new(void);
void ai_response_free(ai_response_t *response);
ai_response_t *ai_response_copy(ai_response_t *response);
gboolean ai_response_is_valid(ai_response_t *response);

/* Enhanced AI Processing */
void ai_service_process_async(ai_request_t *request, ai_callback_t callback, gpointer user_data);
ai_response_t *ai_service_process_sync(ai_request_t *request);
ai_response_t *ai_service_process_with_retry(ai_request_t *request, guint max_retries);
ai_response_t *ai_service_process_batch(ai_request_t **requests, guint count);

/* AI Service Management */
gboolean ai_service_init(void);
gboolean ai_service_init_with_config(const gchar *config_file);
void ai_service_cleanup(void);
gboolean ai_service_is_available(ai_provider_t provider);
gboolean ai_service_health_check(ai_provider_t provider);
void ai_service_register_provider(ai_provider_t provider, ai_config_t *config);

/* Caching System */
gboolean ai_cache_init(guint max_entries, gint64 default_ttl);
void ai_cache_cleanup(void);
ai_response_t *ai_cache_get(const gchar *key);
void ai_cache_set(const gchar *key, ai_response_t *response, gint64 ttl);
void ai_cache_invalidate(const gchar *key);
void ai_cache_clear(void);
guint ai_cache_size(void);

/* Rate Limiting */
gboolean ai_rate_limit_init(guint requests_per_minute);
void ai_rate_limit_cleanup(void);
gboolean ai_rate_limit_check(ai_provider_t provider);
void ai_rate_limit_reset(ai_provider_t provider);
guint ai_rate_limit_remaining(ai_provider_t provider);

/* Metrics and Monitoring */
ai_metrics_t *ai_metrics_get(void);
void ai_metrics_reset(void);
void ai_metrics_record_request(ai_provider_t provider, gboolean success, gdouble response_time);
gchar *ai_metrics_to_json(void);

/* Security Functions */
gboolean ai_security_sanitize_data(JsonObject *data);
gboolean ai_security_validate_api_key(const gchar *api_key);
gchar *ai_security_encrypt_config(const gchar *config_data);
gchar *ai_security_decrypt_config(const gchar *encrypted_data);

/* Logging and Audit */
void ai_audit_log_request(ai_request_t *request, ai_response_t *response);
void ai_audit_log_error(const gchar *error_message, ai_provider_t provider);
void ai_audit_set_log_level(const gchar *level);

/* Utility Functions */
const gchar *ai_provider_to_string(ai_provider_t provider);
const gchar *ai_task_type_to_string(ai_task_type_t task_type);
ai_provider_t ai_provider_from_string(const gchar *provider_str);
ai_task_type_t ai_task_type_from_string(const gchar *task_str);
gchar *ai_generate_request_id(void);
gint64 ai_get_timestamp_ms(void);

#endif /* AI_SERVICE_H */