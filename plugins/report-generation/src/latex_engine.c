/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * LaTeX Report Generation Engine Implementation
 */

#include "latex_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/* Global configuration */
static latex_config_t *g_latex_config = NULL;
static void (*g_error_callback)(const gchar *error, gpointer user_data) = NULL;
static gpointer g_error_callback_data = NULL;

/* Required LaTeX packages */
static const gchar *REQUIRED_PACKAGES[] = {
    "geometry",
    "fancyhdr", 
    "titlesec",
    "graphicx",
    "xcolor",
    "booktabs",
    "longtable",
    "tikz",
    "pgfplots",
    "fontspec",
    "microtype",
    NULL
};

/**
 * @brief Initialize LaTeX engine with default configuration
 */
gboolean
latex_engine_init(void)
{
    latex_config_t *config = latex_config_new();
    return latex_engine_init_with_config(config);
}

/**
 * @brief Initialize LaTeX engine with custom configuration
 */
gboolean
latex_engine_init_with_config(latex_config_t *config)
{
    if (!config || !latex_config_validate(config)) {
        g_warning("Invalid LaTeX configuration provided");
        return FALSE;
    }
    
    if (g_latex_config) {
        latex_config_free(g_latex_config);
    }
    
    g_latex_config = config;
    
    // Check if LaTeX is available
    if (!latex_engine_is_available()) {
        g_warning("LaTeX engine not available - check installation");
        return FALSE;
    }
    
    // Create necessary directories
    if (g_mkdir_with_parents(config->temp_dir, 0755) != 0) {
        g_warning("Failed to create temp directory: %s", config->temp_dir);
        return FALSE;
    }
    
    if (g_mkdir_with_parents(config->output_dir, 0755) != 0) {
        g_warning("Failed to create output directory: %s", config->output_dir);
        return FALSE;
    }
    
    g_info("LaTeX engine initialized successfully");
    return TRUE;
}

/**
 * @brief Cleanup LaTeX engine resources
 */
void
latex_engine_cleanup(void)
{
    if (g_latex_config) {
        latex_config_free(g_latex_config);
        g_latex_config = NULL;
    }
    
    g_error_callback = NULL;
    g_error_callback_data = NULL;
}

/**
 * @brief Check if LaTeX engine is available
 */
gboolean
latex_engine_is_available(void)
{
    if (!g_latex_config) {
        return FALSE;
    }
    
    // Test if LaTeX binary exists and is executable
    gchar *test_cmd = g_strdup_printf("%s --version", g_latex_config->latex_binary);
    gint exit_status;
    gboolean result = g_spawn_command_line_sync(test_cmd, NULL, NULL, &exit_status, NULL);
    
    g_free(test_cmd);
    return result && exit_status == 0;
}

/**
 * @brief Create new LaTeX configuration with defaults
 */
latex_config_t *
latex_config_new(void)
{
    latex_config_t *config = g_new0(latex_config_t, 1);
    
    // Set default values
    config->latex_binary = g_strdup("pdflatex");
    config->temp_dir = g_strdup("/tmp/openvas-latex");
    config->output_dir = g_strdup("/var/lib/openvas/reports");
    config->timeout_seconds = 60;
    config->cleanup_temp = TRUE;
    config->verbose_logging = FALSE;
    
    return config;
}

/**
 * @brief Free LaTeX configuration
 */
void
latex_config_free(latex_config_t *config)
{
    if (!config) return;
    
    g_free(config->latex_binary);
    g_free(config->temp_dir);
    g_free(config->output_dir);
    g_free(config);
}

/**
 * @brief Validate LaTeX configuration
 */
gboolean
latex_config_validate(latex_config_t *config)
{
    if (!config) return FALSE;
    
    if (!config->latex_binary || strlen(config->latex_binary) == 0) {
        g_warning("LaTeX binary path not specified");
        return FALSE;
    }
    
    if (!config->temp_dir || strlen(config->temp_dir) == 0) {
        g_warning("Temp directory not specified");
        return FALSE;
    }
    
    if (!config->output_dir || strlen(config->output_dir) == 0) {
        g_warning("Output directory not specified");
        return FALSE;
    }
    
    if (config->timeout_seconds <= 0) {
        g_warning("Invalid timeout value: %d", config->timeout_seconds);
        return FALSE;
    }
    
    return TRUE;
}

/**
 * @brief Create new LaTeX variables container
 */
latex_variables_t *
latex_variables_new(void)
{
    latex_variables_t *vars = g_new0(latex_variables_t, 1);
    
    vars->variables = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    vars->sections = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    vars->includes = g_ptr_array_new_with_free_func(g_free);
    
    return vars;
}

/**
 * @brief Free LaTeX variables container
 */
void
latex_variables_free(latex_variables_t *vars)
{
    if (!vars) return;
    
    if (vars->variables) {
        g_hash_table_destroy(vars->variables);
    }
    
    if (vars->sections) {
        g_hash_table_destroy(vars->sections);
    }
    
    if (vars->includes) {
        g_ptr_array_free(vars->includes, TRUE);
    }
    
    g_free(vars);
}

/**
 * @brief Set a template variable
 */
void
latex_variables_set(latex_variables_t *vars, const gchar *key, const gchar *value)
{
    if (!vars || !key || !value) return;
    
    g_hash_table_insert(vars->variables, g_strdup(key), g_strdup(value));
}

/**
 * @brief Get a template variable value
 */
const gchar *
latex_variables_get(latex_variables_t *vars, const gchar *key)
{
    if (!vars || !key) return NULL;
    
    return g_hash_table_lookup(vars->variables, key);
}

/**
 * @brief Process template with variable substitution
 */
gchar *
latex_process_template(const gchar *template_content, latex_variables_t *vars)
{
    if (!template_content) return NULL;
    
    GString *result = g_string_new(template_content);
    
    if (vars && vars->variables) {
        GHashTableIter iter;
        gpointer key, value;
        
        g_hash_table_iter_init(&iter, vars->variables);
        while (g_hash_table_iter_next(&iter, &key, &value)) {
            gchar *placeholder = g_strdup_printf("{{%s}}", (gchar *)key);
            gchar *escaped_value = latex_escape_text((gchar *)value);
            
            // Replace all occurrences
            gchar *temp = g_string_free(result, FALSE);
            gchar **parts = g_strsplit(temp, placeholder, -1);
            result = g_string_new(g_strjoinv(escaped_value, parts));
            
            g_free(temp);
            g_free(placeholder);
            g_free(escaped_value);
            g_strfreev(parts);
        }
    }
    
    return g_string_free(result, FALSE);
}

/**
 * @brief Escape special LaTeX characters
 */
gchar *
latex_escape_text(const gchar *text)
{
    if (!text) return NULL;
    
    GString *escaped = g_string_new("");
    
    for (const gchar *p = text; *p; p++) {
        switch (*p) {
            case '\\':
                g_string_append(escaped, "\\textbackslash{}");
                break;
            case '{':
                g_string_append(escaped, "\\{");
                break;
            case '}':
                g_string_append(escaped, "\\}");
                break;
            case '$':
                g_string_append(escaped, "\\$");
                break;
            case '&':
                g_string_append(escaped, "\\&");
                break;
            case '%':
                g_string_append(escaped, "\\%");
                break;
            case '#':
                g_string_append(escaped, "\\#");
                break;
            case '^':
                g_string_append(escaped, "\\textasciicircum{}");
                break;
            case '_':
                g_string_append(escaped, "\\_");
                break;
            case '~':
                g_string_append(escaped, "\\textasciitilde{}");
                break;
            default:
                g_string_append_c(escaped, *p);
                break;
        }
    }
    
    return g_string_free(escaped, FALSE);
}

/**
 * @brief Compile LaTeX file to PDF
 */
latex_result_t *
latex_compile_file(const gchar *tex_file_path)
{
    if (!tex_file_path || !g_latex_config) {
        return NULL;
    }
    
    latex_result_t *result = latex_result_new();
    clock_t start_time = clock();
    
    // Build compilation command
    gchar *output_dir = g_path_get_dirname(tex_file_path);
    gchar *cmd = g_strdup_printf("%s -output-directory=%s -interaction=nonstopmode %s",
                                g_latex_config->latex_binary,
                                output_dir,
                                tex_file_path);
    
    // Execute compilation
    gchar *stdout_output = NULL;
    gchar *stderr_output = NULL;
    gint exit_status;
    GError *error = NULL;
    
    gboolean success = g_spawn_command_line_sync(cmd, &stdout_output, &stderr_output, 
                                               &exit_status, &error);
    
    // Calculate compilation time
    result->compilation_time = ((double)(clock() - start_time)) / CLOCKS_PER_SEC;
    
    if (!success || exit_status != 0) {
        result->status = LATEX_STATUS_COMPILE_ERROR;
        result->error_message = g_strdup_printf("LaTeX compilation failed: %s", 
                                              error ? error->message : "Unknown error");
        if (error) g_error_free(error);
    } else {
        result->status = LATEX_STATUS_SUCCESS;
        
        // Set output file path
        gchar *base_name = g_path_get_basename(tex_file_path);
        gchar *pdf_name = g_strdup_printf("%.*s.pdf", 
                                        (int)(strlen(base_name) - 4), base_name);
        result->output_file = g_build_filename(output_dir, pdf_name, NULL);
        
        g_free(base_name);
        g_free(pdf_name);
    }
    
    // Store log output
    if (stdout_output || stderr_output) {
        result->log_output = g_strdup_printf("STDOUT:\n%s\nSTDERR:\n%s",
                                           stdout_output ? stdout_output : "",
                                           stderr_output ? stderr_output : "");
    }
    
    g_free(cmd);
    g_free(output_dir);
    g_free(stdout_output);
    g_free(stderr_output);
    
    return result;
}

/**
 * @brief Create new LaTeX result
 */
latex_result_t *
latex_result_new(void)
{
    latex_result_t *result = g_new0(latex_result_t, 1);
    result->status = LATEX_STATUS_SUCCESS;
    return result;
}

/**
 * @brief Free LaTeX result
 */
void
latex_result_free(latex_result_t *result)
{
    if (!result) return;
    
    g_free(result->output_file);
    g_free(result->log_output);
    g_free(result->error_message);
    g_free(result);
}

/**
 * @brief Check if LaTeX result indicates success
 */
gboolean
latex_result_is_success(latex_result_t *result)
{
    return result && result->status == LATEX_STATUS_SUCCESS;
}

/**
 * @brief Convert LaTeX status to string
 */
const gchar *
latex_status_to_string(latex_status_t status)
{
    switch (status) {
        case LATEX_STATUS_SUCCESS:
            return "Success";
        case LATEX_STATUS_COMPILE_ERROR:
            return "Compilation Error";
        case LATEX_STATUS_TEMPLATE_ERROR:
            return "Template Error";
        case LATEX_STATUS_IO_ERROR:
            return "I/O Error";
        case LATEX_STATUS_DEPENDENCY_ERROR:
            return "Dependency Error";
        default:
            return "Unknown Status";
    }
}

/**
 * @brief Check LaTeX dependencies
 */
gboolean
latex_check_dependencies(void)
{
    // Check if required packages are available
    for (int i = 0; REQUIRED_PACKAGES[i]; i++) {
        gchar *cmd = g_strdup_printf("kpsewhich %s.sty", REQUIRED_PACKAGES[i]);
        gint exit_status;
        gboolean found = g_spawn_command_line_sync(cmd, NULL, NULL, &exit_status, NULL);
        
        g_free(cmd);
        
        if (!found || exit_status != 0) {
            g_warning("Required LaTeX package not found: %s", REQUIRED_PACKAGES[i]);
            return FALSE;
        }
    }
    
    return TRUE;
}

/**
 * @brief Create temporary file with content
 */
gchar *
latex_create_temp_file(const gchar *content, const gchar *extension)
{
    if (!content || !g_latex_config) return NULL;
    
    gchar *temp_file = g_strdup_printf("%s/openvas_report_XXXXXX.%s",
                                     g_latex_config->temp_dir,
                                     extension ? extension : "tex");
    
    gint fd = g_mkstemp(temp_file);
    if (fd == -1) {
        g_free(temp_file);
        return NULL;
    }
    
    // Write content to file
    if (write(fd, content, strlen(content)) == -1) {
        close(fd);
        g_unlink(temp_file);
        g_free(temp_file);
        return NULL;
    }
    
    close(fd);
    return temp_file;
}