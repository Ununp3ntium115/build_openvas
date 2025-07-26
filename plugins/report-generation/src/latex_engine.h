/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * LaTeX Report Generation Engine
 * Core LaTeX processing and compilation system
 */

#ifndef LATEX_ENGINE_H
#define LATEX_ENGINE_H

#include <glib.h>
#include <json-glib/json-glib.h>

/**
 * @brief LaTeX compilation status
 */
typedef enum {
    LATEX_STATUS_SUCCESS,
    LATEX_STATUS_COMPILE_ERROR,
    LATEX_STATUS_TEMPLATE_ERROR,
    LATEX_STATUS_IO_ERROR,
    LATEX_STATUS_DEPENDENCY_ERROR
} latex_status_t;

/**
 * @brief LaTeX engine configuration
 */
typedef struct {
    gchar *latex_binary;        // Path to latex/pdflatex binary
    gchar *temp_dir;           // Temporary directory for compilation
    gchar *output_dir;         // Output directory for reports
    gint timeout_seconds;      // Compilation timeout
    gboolean cleanup_temp;     // Clean up temporary files
    gboolean verbose_logging;  // Enable verbose logging
} latex_config_t;

/**
 * @brief LaTeX compilation result
 */
typedef struct {
    latex_status_t status;
    gchar *output_file;        // Path to generated file
    gchar *log_output;         // LaTeX compilation log
    gchar *error_message;      // Error message if failed
    gdouble compilation_time;  // Time taken in seconds
} latex_result_t;

/**
 * @brief LaTeX template variables
 */
typedef struct {
    GHashTable *variables;     // Key-value pairs for substitution
    GHashTable *sections;      // Named sections content
    GPtrArray *includes;       // Additional files to include
} latex_variables_t;

/* Core LaTeX Engine Functions */
gboolean latex_engine_init(void);
gboolean latex_engine_init_with_config(latex_config_t *config);
void latex_engine_cleanup(void);
gboolean latex_engine_is_available(void);

/* Configuration Management */
latex_config_t *latex_config_new(void);
void latex_config_free(latex_config_t *config);
gboolean latex_config_validate(latex_config_t *config);
void latex_config_set_binary(latex_config_t *config, const gchar *binary_path);
void latex_config_set_temp_dir(latex_config_t *config, const gchar *temp_dir);
void latex_config_set_timeout(latex_config_t *config, gint timeout_seconds);

/* Template Variable Management */
latex_variables_t *latex_variables_new(void);
void latex_variables_free(latex_variables_t *vars);
void latex_variables_set(latex_variables_t *vars, const gchar *key, const gchar *value);
void latex_variables_set_section(latex_variables_t *vars, const gchar *name, const gchar *content);
void latex_variables_add_include(latex_variables_t *vars, const gchar *file_path);
const gchar *latex_variables_get(latex_variables_t *vars, const gchar *key);

/* Template Processing */
gchar *latex_process_template(const gchar *template_content, latex_variables_t *vars);
gboolean latex_validate_template(const gchar *template_content);
gchar *latex_escape_text(const gchar *text);
gchar *latex_format_date(const gchar *date_string);

/* Compilation Functions */
latex_result_t *latex_compile_file(const gchar *tex_file_path);
latex_result_t *latex_compile_content(const gchar *tex_content, const gchar *output_name);
latex_result_t *latex_compile_with_options(const gchar *tex_file_path, const gchar *options);

/* Result Management */
latex_result_t *latex_result_new(void);
void latex_result_free(latex_result_t *result);
gboolean latex_result_is_success(latex_result_t *result);
const gchar *latex_status_to_string(latex_status_t status);

/* Utility Functions */
gboolean latex_check_dependencies(void);
GPtrArray *latex_get_required_packages(void);
gchar *latex_get_version(void);
gboolean latex_install_package(const gchar *package_name);

/* Error Handling */
void latex_set_error_callback(void (*callback)(const gchar *error, gpointer user_data), gpointer user_data);
gchar *latex_parse_error_log(const gchar *log_content);
gboolean latex_is_recoverable_error(const gchar *error_message);

/* File Management */
gchar *latex_create_temp_file(const gchar *content, const gchar *extension);
gboolean latex_cleanup_temp_files(const gchar *base_name);
gchar *latex_get_temp_dir(void);

#endif /* LATEX_ENGINE_H */