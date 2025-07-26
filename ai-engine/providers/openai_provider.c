/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * OpenAI Provider Implementation
 */

#include "../api/ai_service.h"
#include <curl/curl.h>
#include <json-glib/json-glib.h>

/**
 * @brief Build OpenAI API request payload
 */
static JsonObject *
build_openai_payload(ai_request_t *request)
{
    JsonBuilder *builder = json_builder_new();
    JsonObject *payload;
    
    json_builder_begin_object(builder);
    
    // Model
    json_builder_set_member_name(builder, "model");
    json_builder_add_string_value(builder, request->config->model);
    
    // Messages array
    json_builder_set_member_name(builder, "messages");
    json_builder_begin_array(builder);
    
    // System message based on task type
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "role");
    json_builder_add_string_value(builder, "system");
    json_builder_set_member_name(builder, "content");
    
    switch (request->task_type) {
        case AI_TASK_VULNERABILITY_ANALYSIS:
            json_builder_add_string_value(builder, 
                "You are a cybersecurity expert specializing in vulnerability analysis. "
                "Analyze the provided vulnerability data and provide detailed insights, "
                "risk assessment, and remediation recommendations.");
            break;
        case AI_TASK_THREAT_MODELING:
            json_builder_add_string_value(builder,
                "You are a threat modeling expert. Analyze the provided system information "
                "and identify potential threats, attack vectors, and security recommendations.");
            break;
        case AI_TASK_SCAN_OPTIMIZATION:
            json_builder_add_string_value(builder,
                "You are a penetration testing expert. Optimize the scanning parameters "
                "based on the target information to improve efficiency and coverage.");
            break;
        case AI_TASK_REPORT_GENERATION:
            json_builder_add_string_value(builder,
                "You are a cybersecurity report writer. Generate a comprehensive, "
                "professional security assessment report based on the provided data.");
            break;
        case AI_TASK_EXPLOIT_SUGGESTION:
            json_builder_add_string_value(builder,
                "You are an ethical penetration testing expert. Suggest potential "
                "exploitation techniques for educational and authorized testing purposes only.");
            break;
    }
    json_builder_end_object(builder);
    
    // User message with actual data
    json_builder_begin_object(builder);
    json_builder_set_member_name(builder, "role");
    json_builder_add_string_value(builder, "user");
    json_builder_set_member_name(builder, "content");
    
    // Convert input data to string
    JsonGenerator *generator = json_generator_new();
    JsonNode *root = json_node_new(JSON_NODE_OBJECT);
    json_node_set_object(root, request->input_data);
    json_generator_set_root(generator, root);
    gchar *input_str = json_generator_to_data(generator, NULL);
    
    json_builder_add_string_value(builder, input_str);
    json_builder_end_object(builder);
    
    json_builder_end_array(builder);
    
    // Additional parameters
    json_builder_set_member_name(builder, "temperature");
    json_builder_add_double_value(builder, 0.3); // Lower temperature for more focused responses
    
    json_builder_set_member_name(builder, "max_tokens");
    json_builder_add_int_value(builder, 2000);
    
    json_builder_end_object(builder);
    
    JsonNode *payload_node = json_builder_get_root(builder);
    payload = json_object_ref(json_node_get_object(payload_node));
    
    // Cleanup
    g_free(input_str);
    g_object_unref(generator);
    json_node_free(root);
    g_object_unref(builder);
    
    return payload;
}

/**
 * @brief Process OpenAI API response
 */
static ai_response_t *
process_openai_response(const gchar *response_data)
{
    ai_response_t *response = ai_response_new();
    GError *error = NULL;
    
    JsonParser *parser = json_parser_new();
    if (!json_parser_load_from_data(parser, response_data, -1, &error)) {
        response->success = FALSE;
        response->error_message = g_strdup_printf("JSON parse error: %s", error->message);
        g_error_free(error);
        g_object_unref(parser);
        return response;
    }
    
    JsonNode *root = json_parser_get_root(parser);
    JsonObject *root_obj = json_node_get_object(root);
    
    // Check for API errors
    if (json_object_has_member(root_obj, "error")) {
        JsonObject *error_obj = json_object_get_object_member(root_obj, "error");
        const gchar *error_msg = json_object_get_string_member(error_obj, "message");
        response->success = FALSE;
        response->error_message = g_strdup(error_msg);
        g_object_unref(parser);
        return response;
    }
    
    // Extract the response content
    if (json_object_has_member(root_obj, "choices")) {
        JsonArray *choices = json_object_get_array_member(root_obj, "choices");
        if (json_array_get_length(choices) > 0) {
            JsonObject *choice = json_array_get_object_element(choices, 0);
            JsonObject *message = json_object_get_object_member(choice, "message");
            const gchar *content = json_object_get_string_member(message, "content");
            
            // Create result object
            JsonBuilder *result_builder = json_builder_new();
            json_builder_begin_object(result_builder);
            json_builder_set_member_name(result_builder, "content");
            json_builder_add_string_value(result_builder, content);
            json_builder_set_member_name(result_builder, "provider");
            json_builder_add_string_value(result_builder, "openai");
            json_builder_end_object(result_builder);
            
            JsonNode *result_node = json_builder_get_root(result_builder);
            response->result = json_object_ref(json_node_get_object(result_node));
            response->success = TRUE;
            response->confidence_score = 0.8; // Default confidence
            
            g_object_unref(result_builder);
        }
    }
    
    g_object_unref(parser);
    return response;
}

/**
 * @brief Validate OpenAI configuration
 */
static gboolean
validate_openai_config(ai_config_t *config)
{
    if (!config) {
        g_warning("OpenAI configuration is NULL");
        return FALSE;
    }
    
    if (!config->api_key || !g_str_has_prefix(config->api_key, "sk-")) {
        g_warning("Invalid OpenAI API key format");
        return FALSE;
    }
    
    if (!config->endpoint || !g_str_has_prefix(config->endpoint, "https://")) {
        g_warning("Invalid OpenAI endpoint URL");
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Enhanced error handling for OpenAI responses
 */
static ai_response_t *
handle_openai_error(long response_code, const gchar *response_data)
{
    ai_response_t *response = ai_response_new();
    response->success = FALSE;
    
    switch (response_code) {
        case 401:
            response->error_message = g_strdup("OpenAI API: Unauthorized - Invalid API key");
            break;
        case 429:
            response->error_message = g_strdup("OpenAI API: Rate limit exceeded");
            break;
        case 500:
            response->error_message = g_strdup("OpenAI API: Internal server error");
            break;
        case 503:
            response->error_message = g_strdup("OpenAI API: Service unavailable");
            break;
        default:
            response->error_message = g_strdup_printf("OpenAI API: HTTP %ld error", response_code);
            break;
    }
    
    // Try to parse error details from response
    if (response_data) {
        JsonParser *parser = json_parser_new();
        GError *error = NULL;
        
        if (json_parser_load_from_data(parser, response_data, -1, &error)) {
            JsonNode *root = json_parser_get_root(parser);
            JsonObject *root_obj = json_node_get_object(root);
            
            if (json_object_has_member(root_obj, "error")) {
                JsonObject *error_obj = json_object_get_object_member(root_obj, "error");
                const gchar *error_msg = json_object_get_string_member(error_obj, "message");
                if (error_msg) {
                    g_free(response->error_message);
                    response->error_message = g_strdup_printf("OpenAI API: %s", error_msg);
                }
            }
        }
        
        if (error) g_error_free(error);
        g_object_unref(parser);
    }
    
    return response;
}

/**
 * @brief Send request to OpenAI API with enhanced error handling and retry logic
 */
ai_response_t *
openai_provider_process(ai_request_t *request)
{
    if (!request || !request->config) {
        ai_response_t *response = ai_response_new();
        response->success = FALSE;
        response->error_message = g_strdup("Invalid request or missing configuration");
        return response;
    }
    
    // Validate configuration
    if (!validate_openai_config(request->config)) {
        ai_response_t *response = ai_response_new();
        response->success = FALSE;
        response->error_message = g_strdup("Invalid OpenAI configuration");
        return response;
    }
    
    CURL *curl;
    CURLcode res;
    http_response_t response_data = {0};
    ai_response_t *ai_response = NULL;
    long response_code = 0;
    
    curl = curl_easy_init();
    if (!curl) {
        ai_response = ai_response_new();
        ai_response->success = FALSE;
        ai_response->error_message = g_strdup("Failed to initialize CURL");
        return ai_response;
    }
    
    // Build request payload
    JsonObject *payload = build_openai_payload(request);
    JsonGenerator *generator = json_generator_new();
    JsonNode *root = json_node_new(JSON_NODE_OBJECT);
    json_node_set_object(root, payload);
    json_generator_set_root(generator, root);
    gchar *json_data = json_generator_to_data(generator, NULL);
    
    // Set CURL options
    curl_easy_setopt(curl, CURLOPT_URL, request->config->endpoint);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, request->config->timeout);
    
    // Set headers
    struct curl_slist *headers = NULL;
    gchar *auth_header = g_strdup_printf("Authorization: Bearer %s", request->config->api_key);
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, auth_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // Perform request with timeout and retry logic
    gint64 start_time = g_get_monotonic_time();
    res = curl_easy_perform(curl);
    gint64 end_time = g_get_monotonic_time();
    
    // Get HTTP response code
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    if (res != CURLE_OK) {
        ai_response = ai_response_new();
        ai_response->success = FALSE;
        
        switch (res) {
            case CURLE_OPERATION_TIMEDOUT:
                ai_response->error_message = g_strdup("OpenAI API request timed out");
                break;
            case CURLE_COULDNT_CONNECT:
                ai_response->error_message = g_strdup("Could not connect to OpenAI API");
                break;
            case CURLE_SSL_CONNECT_ERROR:
                ai_response->error_message = g_strdup("SSL connection error to OpenAI API");
                break;
            default:
                ai_response->error_message = g_strdup_printf("CURL error: %s", curl_easy_strerror(res));
                break;
        }
    } else if (response_code >= 400) {
        // Handle HTTP error responses
        ai_response = handle_openai_error(response_code, response_data.data);
    } else {
        // Process successful response
        ai_response = process_openai_response(response_data.data);
        ai_response->processing_time_ms = (end_time - start_time) / 1000;
        
        // Log successful request
        g_message("OpenAI API request completed successfully in %ldms", 
                 ai_response->processing_time_ms);
    }
    
    // Cleanup
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    g_free(auth_header);
    g_free(json_data);
    g_free(response_data.data);
    json_object_unref(payload);
    g_object_unref(generator);
    json_node_free(root);
    
    return ai_response;
}