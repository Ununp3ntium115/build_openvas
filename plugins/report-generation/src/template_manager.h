/**
 * SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
 * SPDX-License-Identifier: GPL-2.0-or-later
 * 
 * LaTeX Template Management System
 */

#ifndef TEMPLATE_MANAGER_H
#define TEMPLATE_MANAGER_H

#include <glib.h>
#include <json-glib/json-glib.h>
#include "latex_engine.h"

/**
 * @brief Template types
 */
typedef enum {
    TEMPLATE_TYPE_EXECUTIVE,
    TEMPLATE_TYPE_TECHNICAL,
    TEMPLATE_TYPE_COMPLIANCE,
    TEMPLATE_TYPE_CUSTOM
} template_type_t;

/**
 * @brief Template metadata
 */
typedef struct {
    gchar *name;
    gchar *display_name;
    gchar *description;
    gchar *author;
    gchar *version;
    template_type_t type;
    gchar *created_date;
    gchar *modified_date;
    GPtrArray *required_variables;
    GPtrArray *optional_variables;
    JsonObject *custom_metadata;
} template_metadata_t;

/**
 * @brief Report template structure
 */
typedef struct {
    template_metadata_t *metadata;
    gchar *template_path;
    gchar *style_path;
    gchar *template_content;
    gchar *style_content;
    GPtrArray *asset_files;
    GHashTable *default_variables;
} report_template_t;

/**
 * @brief Template variable definition
 */
typedef struct {
    gchar *name;
    gchar *display_name;
    gchar *description;
    gchar *data_type;
    gchar *default_value;
    gboolean required;
    GPtrArray *allowed_values;
} template_variable_t;

/**
 * @brief Template validation result
 */
typedef struct {
    gboolean is_valid;
    GPtrArray *errors;
    GPtrArray *warnings;
} template_validation_t;

/* Template Manager Initialization */
gboolean template_manager_init(const gchar *templates_dir);
void template_manager_cleanup(void);
gboolean template_manager_reload(void);

/* Template Discovery and Loading */
GPtrArray *template_list_available(void);
GPtrArray *template_list_by_type(template_type_t type);
report_template_t *template_load(const gchar *template_name);
report_template_t *template_load_from_file(const gchar *template_path);
gboolean template_exists(const gchar *template_name);

/* Template Management */
report_template_t *template_new(const gchar *name, template_type_t type);
void template_free(report_template_t *template);
gboolean template_save(report_template_t *template, const gchar *output_dir);
gboolean template_delete(const gchar *template_name);
gboolean template_copy(const gchar *source_name, const gchar *dest_name);

/* Template Metadata */
template_metadata_t *template_metadata_new(void);
void template_metadata_free(template_metadata_t *metadata);
gboolean template_metadata_load_from_json(template_metadata_t *metadata, const gchar *json_content);
gchar *template_metadata_to_json(template_metadata_t *metadata);

/* Template Variables */
template_variable_t *template_variable_new(const gchar *name, const gchar *data_type);
void template_variable_free(template_variable_t *variable);
void template_add_variable(report_template_t *template, template_variable_t *variable);
template_variable_t *template_get_variable(report_template_t *template, const gchar *name);
GPtrArray *template_get_required_variables(report_template_t *template);
GPtrArray *template_get_optional_variables(report_template_t *template);

/* Template Validation */
template_validation_t *template_validate(report_template_t *template);
template_validation_t *template_validate_content(const gchar *template_content);
void template_validation_free(template_validation_t *validation);
gboolean template_check_syntax(const gchar *template_content);
gboolean template_check_variables(report_template_t *template, latex_variables_t *vars);

/* Template Processing */
gchar *template_process(report_template_t *template, latex_variables_t *vars);
gchar *template_process_section(report_template_t *template, const gchar *section_name, latex_variables_t *vars);
gboolean template_has_section(report_template_t *template, const gchar *section_name);
GPtrArray *template_get_sections(report_template_t *template);

/* Template Assets */
gboolean template_add_asset(report_template_t *template, const gchar *asset_path);
gboolean template_remove_asset(report_template_t *template, const gchar *asset_name);
GPtrArray *template_get_assets(report_template_t *template);
gchar *template_get_asset_path(report_template_t *template, const gchar *asset_name);

/* Template Customization */
gboolean template_set_style(report_template_t *template, const gchar *style_content);
gboolean template_set_logo(report_template_t *template, const gchar *logo_path);
gboolean template_set_colors(report_template_t *template, const gchar *primary_color, const gchar *secondary_color);
gboolean template_set_fonts(report_template_t *template, const gchar *main_font, const gchar *mono_font);

/* Built-in Templates */
gboolean template_install_builtin_templates(const gchar *install_dir);
report_template_t *template_create_executive_template(void);
report_template_t *template_create_technical_template(void);
report_template_t *template_create_compliance_template(const gchar *framework);

/* Template Import/Export */
gboolean template_export(const gchar *template_name, const gchar *export_path);
gboolean template_import(const gchar *import_path, const gchar *template_name);
gboolean template_export_as_zip(const gchar *template_name, const gchar *zip_path);
gboolean template_import_from_zip(const gchar *zip_path);

/* Utility Functions */
const gchar *template_type_to_string(template_type_t type);
template_type_t template_type_from_string(const gchar *type_str);
gchar *template_generate_unique_name(const gchar *base_name);
gboolean template_is_name_valid(const gchar *name);

#endif /* TEMPLATE_MANAGER_H */