/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Security Module - Production-grade security for AI integration
 */

#ifndef AI_SECURITY_H
#define AI_SECURITY_H

#include <glib.h>
#include <json-glib/json-glib.h>

/**
 * @brief Security levels for data sanitization
 */
typedef enum {
    AI_SECURITY_LEVEL_NONE,      // No sanitization
    AI_SECURITY_LEVEL_BASIC,     // Basic PII removal
    AI_SECURITY_LEVEL_STANDARD,  // Standard enterprise sanitization
    AI_SECURITY_LEVEL_STRICT,    // Strict government/healthcare level
    AI_SECURITY_LEVEL_PARANOID   // Maximum security, minimal data
} ai_security_level_t;

/**
 * @brief Data classification levels
 */
typedef enum {
    AI_DATA_PUBLIC,
    AI_DATA_INTERNAL,
    AI_DATA_CONFIDENTIAL,
    AI_DATA_RESTRICTED,
    AI_DATA_TOP_SECRET
} ai_data_classification_t;

/**
 * @brief Security audit event types
 */
typedef enum {
    AI_AUDIT_REQUEST_SENT,
    AI_AUDIT_RESPONSE_RECEIVED,
    AI_AUDIT_DATA_SANITIZED,
    AI_AUDIT_SECURITY_VIOLATION,
    AI_AUDIT_CONFIG_CHANGED,
    AI_AUDIT_ERROR_OCCURRED
} ai_audit_event_t;

/**
 * @brief Security audit entry
 */
typedef struct {
    gint64 timestamp;
    ai_audit_event_t event_type;
    gchar *user_id;
    gchar *session_id;
    gchar *provider;
    gchar *event_description;
    gchar *ip_address;
    JsonObject *event_data;
} ai_audit_entry_t;

/* Data Sanitization Functions */
gboolean ai_security_init(ai_security_level_t level);
void ai_security_cleanup(void);

gboolean ai_security_sanitize_json(JsonObject *data, ai_data_classification_t classification);
gchar *ai_security_sanitize_string(const gchar *input, ai_data_classification_t classification);
gboolean ai_security_validate_input_size(const gchar *input, gsize max_size);
gboolean ai_security_validate_json_depth(JsonObject *data, guint max_depth);

/* Pattern-based sanitization */
gchar *ai_security_remove_ip_addresses(const gchar *text);
gchar *ai_security_remove_email_addresses(const gchar *text);
gchar *ai_security_remove_phone_numbers(const gchar *text);
gchar *ai_security_remove_credit_cards(const gchar *text);
gchar *ai_security_remove_ssn(const gchar *text);
gchar *ai_security_remove_hostnames(const gchar *text);
gchar *ai_security_remove_file_paths(const gchar *text);
gchar *ai_security_remove_urls(const gchar *text);

/* API Key Security */
gboolean ai_security_validate_api_key_format(const gchar *api_key, const gchar *provider);
gchar *ai_security_mask_api_key(const gchar *api_key);
gboolean ai_security_store_api_key_secure(const gchar *provider, const gchar *api_key);
gchar *ai_security_retrieve_api_key_secure(const gchar *provider);

/* Encryption/Decryption */
gchar *ai_security_encrypt_data(const gchar *plaintext, const gchar *key);
gchar *ai_security_decrypt_data(const gchar *ciphertext, const gchar *key);
gchar *ai_security_generate_encryption_key(void);
gboolean ai_security_secure_delete(gchar *sensitive_data, gsize length);

/* Request/Response Security */
gboolean ai_security_validate_request(JsonObject *request_data);
gboolean ai_security_validate_response(JsonObject *response_data);
gboolean ai_security_check_content_policy(const gchar *content);
gboolean ai_security_rate_limit_check(const gchar *user_id, const gchar *provider);

/* Audit Logging */
gboolean ai_audit_init(const gchar *log_file);
void ai_audit_cleanup(void);
void ai_audit_log_event(ai_audit_event_t event_type, const gchar *description, JsonObject *data);
void ai_audit_log_request(const gchar *provider, JsonObject *request, const gchar *user_id);
void ai_audit_log_response(const gchar *provider, JsonObject *response, gboolean success);
void ai_audit_log_security_violation(const gchar *violation_type, const gchar *details);

/* Security Monitoring */
gboolean ai_security_monitor_init(void);
void ai_security_monitor_cleanup(void);
void ai_security_monitor_request(const gchar *provider, gsize request_size);
void ai_security_monitor_response(const gchar *provider, gsize response_size, gboolean success);
gboolean ai_security_monitor_check_anomaly(const gchar *provider);

/* Compliance Functions */
gboolean ai_security_gdpr_compliant_processing(JsonObject *data);
gboolean ai_security_hipaa_compliant_processing(JsonObject *data);
gboolean ai_security_pci_compliant_processing(JsonObject *data);
gboolean ai_security_sox_compliant_processing(JsonObject *data);

/* Security Utilities */
gchar *ai_security_generate_session_id(void);
gchar *ai_security_hash_data(const gchar *data);
gboolean ai_security_constant_time_compare(const gchar *a, const gchar *b);
void ai_security_clear_memory(void *ptr, gsize size);

/* Security Configuration */
void ai_security_set_level(ai_security_level_t level);
ai_security_level_t ai_security_get_level(void);
void ai_security_set_max_request_size(gsize max_size);
void ai_security_set_max_response_size(gsize max_size);
void ai_security_enable_audit(gboolean enabled);
void ai_security_enable_monitoring(gboolean enabled);

#endif /* AI_SECURITY_H */