/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * AI Metrics and Monitoring System - Production-grade observability
 */

#ifndef AI_METRICS_H
#define AI_METRICS_H

#include <glib.h>
#include <json-glib/json-glib.h>
#include "../api/ai_service.h"

/**
 * @brief Metric types for different measurements
 */
typedef enum {
    AI_METRIC_COUNTER,      // Monotonically increasing counter
    AI_METRIC_GAUGE,        // Current value that can go up or down
    AI_METRIC_HISTOGRAM,    // Distribution of values
    AI_METRIC_SUMMARY       // Summary statistics (percentiles)
} ai_metric_type_t;

/**
 * @brief Performance metrics structure
 */
typedef struct {
    // Request metrics
    guint64 total_requests;
    guint64 successful_requests;
    guint64 failed_requests;
    guint64 cached_requests;
    guint64 rate_limited_requests;
    
    // Timing metrics
    gdouble total_response_time_ms;
    gdouble min_response_time_ms;
    gdouble max_response_time_ms;
    gdouble avg_response_time_ms;
    gdouble p50_response_time_ms;
    gdouble p95_response_time_ms;
    gdouble p99_response_time_ms;
    
    // Provider-specific metrics
    GHashTable *provider_metrics;
    
    // Error metrics
    GHashTable *error_counts;
    
    // Resource metrics
    guint64 memory_usage_bytes;
    guint active_threads;
    guint queue_size;
    
    // Business metrics
    guint64 vulnerabilities_analyzed;
    guint64 threats_modeled;
    guint64 scans_optimized;
    guint64 reports_generated;
    
    // Time tracking
    gint64 start_time;
    gint64 last_update_time;
    
} ai_performance_metrics_t;

/**
 * @brief Provider-specific metrics
 */
typedef struct {
    gchar *provider_name;
    guint64 requests_sent;
    guint64 requests_successful;
    guint64 requests_failed;
    gdouble total_cost_usd;
    guint64 tokens_consumed;
    gdouble avg_response_time_ms;
    gint64 last_request_time;
    guint current_rate_limit;
} ai_provider_metrics_t;

/**
 * @brief Health check status
 */
typedef enum {
    AI_HEALTH_HEALTHY,
    AI_HEALTH_DEGRADED,
    AI_HEALTH_UNHEALTHY,
    AI_HEALTH_UNKNOWN
} ai_health_status_t;

/**
 * @brief Health check result
 */
typedef struct {
    ai_health_status_t status;
    gchar *message;
    gdouble response_time_ms;
    gint64 timestamp;
    JsonObject *details;
} ai_health_check_t;

/* Metrics System Management */
gboolean ai_metrics_init(void);
void ai_metrics_cleanup(void);
void ai_metrics_reset(void);

/* Core Metrics Functions */
void ai_metrics_increment_counter(const gchar *metric_name);
void ai_metrics_set_gauge(const gchar *metric_name, gdouble value);
void ai_metrics_record_histogram(const gchar *metric_name, gdouble value);
void ai_metrics_record_timing(const gchar *metric_name, gdouble duration_ms);

/* Request/Response Metrics */
void ai_metrics_record_request(ai_provider_t provider, ai_task_type_t task_type);
void ai_metrics_record_response(ai_provider_t provider, gboolean success, gdouble response_time_ms);
void ai_metrics_record_error(ai_provider_t provider, const gchar *error_type);
void ai_metrics_record_cache_hit(ai_provider_t provider);
void ai_metrics_record_cache_miss(ai_provider_t provider);

/* Provider Metrics */
ai_provider_metrics_t *ai_metrics_get_provider(ai_provider_t provider);
void ai_metrics_update_provider_cost(ai_provider_t provider, gdouble cost_usd);
void ai_metrics_update_provider_tokens(ai_provider_t provider, guint64 tokens);

/* Performance Metrics */
ai_performance_metrics_t *ai_metrics_get_performance(void);
gdouble ai_metrics_get_success_rate(void);
gdouble ai_metrics_get_cache_hit_rate(void);
gdouble ai_metrics_get_avg_response_time(void);
guint64 ai_metrics_get_requests_per_minute(void);

/* Health Monitoring */
gboolean ai_health_init(void);
void ai_health_cleanup(void);
ai_health_check_t *ai_health_check_provider(ai_provider_t provider);
ai_health_check_t *ai_health_check_system(void);
ai_health_status_t ai_health_get_overall_status(void);

/* Alerting System */
typedef void (*ai_alert_callback_t)(const gchar *alert_type, const gchar *message, JsonObject *data);

gboolean ai_alerts_init(void);
void ai_alerts_cleanup(void);
void ai_alerts_register_callback(ai_alert_callback_t callback);
void ai_alerts_check_thresholds(void);
void ai_alerts_trigger(const gchar *alert_type, const gchar *message, JsonObject *data);

/* Threshold Configuration */
void ai_alerts_set_error_rate_threshold(gdouble threshold);
void ai_alerts_set_response_time_threshold(gdouble threshold_ms);
void ai_alerts_set_memory_usage_threshold(guint64 threshold_bytes);
void ai_alerts_set_queue_size_threshold(guint threshold);

/* Metrics Export */
gchar *ai_metrics_export_json(void);
gchar *ai_metrics_export_prometheus(void);
gboolean ai_metrics_export_to_file(const gchar *filename, const gchar *format);

/* Dashboard Data */
JsonObject *ai_metrics_get_dashboard_data(void);
JsonArray *ai_metrics_get_recent_requests(guint limit);
JsonArray *ai_metrics_get_error_summary(void);
JsonObject *ai_metrics_get_provider_comparison(void);

/* Real-time Monitoring */
typedef void (*ai_metrics_callback_t)(const gchar *metric_name, gdouble value, gpointer user_data);

void ai_metrics_register_callback(const gchar *metric_name, ai_metrics_callback_t callback, gpointer user_data);
void ai_metrics_unregister_callback(const gchar *metric_name);
void ai_metrics_start_real_time_monitoring(guint interval_seconds);
void ai_metrics_stop_real_time_monitoring(void);

/* Performance Profiling */
typedef struct {
    gchar *operation_name;
    gint64 start_time;
    gint64 end_time;
    gdouble duration_ms;
    JsonObject *metadata;
} ai_profile_entry_t;

void ai_profiler_start(const gchar *operation_name);
void ai_profiler_end(const gchar *operation_name);
ai_profile_entry_t *ai_profiler_get_entry(const gchar *operation_name);
JsonArray *ai_profiler_get_all_entries(void);
void ai_profiler_clear(void);

/* Resource Monitoring */
void ai_metrics_update_memory_usage(void);
void ai_metrics_update_thread_count(void);
void ai_metrics_update_queue_size(guint size);
JsonObject *ai_metrics_get_resource_usage(void);

/* Business Intelligence */
void ai_metrics_record_vulnerability_analyzed(const gchar *severity, const gchar *type);
void ai_metrics_record_threat_modeled(const gchar *threat_type, gdouble confidence);
void ai_metrics_record_scan_optimized(gdouble improvement_percent);
void ai_metrics_record_report_generated(const gchar *report_type, guint page_count);

JsonObject *ai_metrics_get_business_intelligence(void);
JsonArray *ai_metrics_get_vulnerability_trends(guint days);
JsonArray *ai_metrics_get_threat_analysis_summary(void);

/* Utility Functions */
gchar *ai_metrics_format_duration(gdouble duration_ms);
gchar *ai_metrics_format_bytes(guint64 bytes);
gchar *ai_metrics_format_timestamp(gint64 timestamp);
gdouble ai_metrics_calculate_percentile(GArray *values, gdouble percentile);

#endif /* AI_METRICS_H */