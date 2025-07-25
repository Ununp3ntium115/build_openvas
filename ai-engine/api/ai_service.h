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

/* Core AI Service Functions */
ai_config_t *ai_config_new(ai_provider_t provider, const gchar *api_key);
void ai_config_free(ai_config_t *config);

ai_request_t *ai_request_new(ai_task_type_t task_type, JsonObject *input_data);
void ai_request_free(ai_request_t *request);

ai_response_t *ai_response_new(void);
void ai_response_free(ai_response_t *response);

/* Async AI Processing */
void ai_service_process_async(ai_request_t *request, ai_callback_t callback, gpointer user_data);
ai_response_t *ai_service_process_sync(ai_request_t *request);

/* AI Service Management */
gboolean ai_service_init(void);
void ai_service_cleanup(void);
gboolean ai_service_is_available(ai_provider_t provider);

/* Utility Functions */
const gchar *ai_provider_to_string(ai_provider_t provider);
const gchar *ai_task_type_to_string(ai_task_type_t task_type);

#endif /* AI_SERVICE_H */