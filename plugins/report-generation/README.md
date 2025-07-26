# OpenVAS Professional LaTeX Report Generation Plugin

## Overview

The Professional LaTeX Report Generation Plugin transforms OpenVAS vulnerability assessment data into enterprise-grade, professional reports suitable for executive presentation, technical teams, and compliance requirements. This plugin replaces the basic PDF and CSV reports with sophisticated LaTeX-generated documents featuring professional typography, charts, and customizable branding.

## Features

### 🎯 Professional Report Templates
- **Executive Summary**: C-level focused reports with high-level insights
- **Technical Details**: Comprehensive technical reports for IT teams
- **Compliance Reports**: Framework-specific reports (SOC2, ISO27001, etc.)
- **Custom Templates**: Fully customizable template system

### 📊 Advanced Visualizations
- CVSS severity distribution charts
- Risk trend analysis graphs
- Vulnerability timeline visualizations
- Risk matrix heat maps
- Custom chart generation with TikZ/PGFPlots

### 🤖 AI-Enhanced Reporting
- AI-generated executive summaries
- Intelligent risk prioritization
- Automated remediation recommendations
- Context-aware insights and analysis

### 🎨 Professional Formatting
- Enterprise-grade typography with LaTeX
- Customizable company branding and logos
- Professional color schemes and layouts
- Multi-format output (PDF, HTML, print-ready)

## Installation

### Prerequisites

1. **LaTeX Distribution** (required):
   ```bash
   # Ubuntu/Debian
   sudo apt-get install texlive-latex-base texlive-latex-extra texlive-fonts-recommended texlive-fonts-extra texlive-pictures texlive-science
   
   # CentOS/RHEL
   sudo yum install texlive-latex texlive-collection-latexextra texlive-collection-fontsrecommended
   
   # macOS
   brew install --cask mactex
   ```

2. **Development Dependencies**:
   ```bash
   # Ubuntu/Debian
   sudo apt-get install libglib2.0-dev libjson-glib-dev cmake build-essential
   
   # CentOS/RHEL
   sudo yum install glib2-devel json-glib-devel cmake gcc
   ```

### Building the Plugin

```bash
# Navigate to the plugin directory
cd plugins/report-generation

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the plugin
make -j$(nproc)

# Install (optional)
sudo make install
```

### Verification

```bash
# Test LaTeX installation
pdflatex --version

# Run unit tests
make test

# Run the demo
./examples/latex_report_demo
```

## Quick Start

### Basic Usage

```c
#include "plugins/report-generation/src/report_api.h"

// Initialize the report API
if (!report_api_init()) {
    g_error("Failed to initialize report API");
}

// Create report configuration
report_config_t *config = report_config_new();
config->template_name = g_strdup("executive_summary");
config->company_name = g_strdup("Your Company Name");
config->client_name = g_strdup("Client Organization");
config->output_path = g_strdup("/path/to/output/report.pdf");

// Generate report from vulnerability data
report_result_t *result = report_generate(vulnerabilities, vuln_count, config);

if (report_result_is_success(result)) {
    g_print("Report generated: %s\n", result->output_file);
} else {
    g_print("Report generation failed: %s\n", result->error_message);
}

// Cleanup
report_result_free(result);
report_config_free(config);
report_api_cleanup();
```

### Executive Summary Report

```c
// Create executive-focused configuration
report_config_t *config = report_config_new();
report_config_set_template(config, "executive_summary");
report_config_set_company_info(config, "Security Corp", "/path/to/logo.png");

// Add custom variables
report_config_add_variable(config, "assessment_period", "Q1 2025");
report_config_add_variable(config, "technical_lead_name", "Dr. Jane Smith");

// Generate executive summary
report_result_t *result = report_generate_executive_summary(
    vulnerabilities, vuln_count, config);
```

### AI-Enhanced Reporting

```c
// Enable AI insights
report_config_t *config = report_config_new();
config->include_ai_insights = TRUE;

// Generate AI-enhanced report
report_result_t *result = report_generate_with_ai_insights(
    vulnerabilities, vuln_count, config);

// Get AI-generated summary
gchar *ai_summary = report_generate_ai_executive_summary(
    vulnerabilities, vuln_count);
```

## Template System

### Available Templates

1. **executive_summary**: Professional executive reports
   - Target audience: C-level executives, board members
   - Focus: High-level risk overview, business impact
   - Length: 8-12 pages

2. **technical_details**: Comprehensive technical reports
   - Target audience: IT teams, security engineers
   - Focus: Detailed vulnerability information, remediation steps
   - Length: 20-50 pages

3. **compliance_report**: Framework-specific compliance reports
   - Target audience: Compliance teams, auditors
   - Focus: Regulatory requirements, control mappings
   - Length: 15-30 pages

### Template Customization

Templates support extensive customization through variables:

```json
{
  "company_name": "Your Organization",
  "company_logo": "/path/to/logo.png",
  "primary_color": "#0066CC",
  "secondary_color": "#666666",
  "report_title": "Custom Report Title",
  "custom_sections": {
    "methodology": "Our assessment methodology...",
    "recommendations": "Strategic recommendations..."
  }
}
```

### Creating Custom Templates

1. Create template directory:
   ```bash
   mkdir templates/custom/my_template
   ```

2. Create LaTeX template file:
   ```latex
   % templates/custom/my_template/template.tex
   \documentclass{article}
   \begin{document}
   \title{{{report_title}}}
   \author{{{company_name}}}
   \maketitle
   
   \section{Executive Summary}
   {{executive_summary}}
   
   \end{document}
   ```

3. Create metadata file:
   ```json
   {
     "name": "my_template",
     "display_name": "My Custom Template",
     "description": "Custom template for specific needs",
     "required_variables": ["report_title", "company_name"],
     "optional_variables": ["executive_summary"]
   }
   ```

## Configuration

### Global Configuration

Create `/etc/openvas/report-generation/report-generation.conf`:

```ini
[latex]
binary_path = /usr/bin/pdflatex
temp_directory = /tmp/openvas-latex
timeout_seconds = 120
cleanup_temp_files = true

[templates]
template_directory = /usr/share/openvas/report-templates
custom_template_directory = /etc/openvas/report-templates

[output]
default_output_directory = /var/lib/openvas/reports
default_format = pdf
include_charts = true

[ai]
enable_ai_insights = true
ai_provider = openai
```

### Runtime Configuration

```c
// Configure LaTeX engine
latex_config_t *latex_config = latex_config_new();
latex_config_set_binary(latex_config, "/usr/bin/pdflatex");
latex_config_set_timeout(latex_config, 120);
latex_engine_init_with_config(latex_config);

// Configure template manager
template_manager_init("/usr/share/openvas/report-templates");
```

## API Reference

### Core Functions

```c
// Initialization
gboolean report_api_init(void);
void report_api_cleanup(void);

// Configuration
report_config_t *report_config_new(void);
void report_config_free(report_config_t *config);
void report_config_set_template(report_config_t *config, const gchar *template);

// Report Generation
report_result_t *report_generate(vulnerability_score_t **vulns, guint count, report_config_t *config);
report_result_t *report_generate_executive_summary(vulnerability_score_t **vulns, guint count, report_config_t *config);
report_result_t *report_generate_technical_details(vulnerability_score_t **vulns, guint count, report_config_t *config);

// AI Enhancement
report_result_t *report_generate_with_ai_insights(vulnerability_score_t **vulns, guint count, report_config_t *config);
gchar *report_generate_ai_executive_summary(vulnerability_score_t **vulns, guint count);

// Statistics
report_statistics_t *report_calculate_statistics(vulnerability_score_t **vulns, guint count);
```

### Data Structures

```c
typedef struct {
    gchar *template_name;
    report_format_t output_format;
    gchar *output_path;
    gchar *company_name;
    gchar *company_logo;
    gboolean include_charts;
    gboolean include_ai_insights;
    JsonObject *custom_variables;
} report_config_t;

typedef struct {
    report_status_t status;
    gchar *output_file;
    gchar *error_message;
    gdouble generation_time;
    guint total_pages;
} report_result_t;
```

## Performance

### Benchmarks

- **Small Dataset** (< 100 vulnerabilities): < 30 seconds
- **Medium Dataset** (100-1000 vulnerabilities): < 2 minutes  
- **Large Dataset** (> 1000 vulnerabilities): < 5 minutes
- **Memory Usage**: < 512MB for typical reports

### Optimization Tips

1. **Template Caching**: Templates are cached after first load
2. **Parallel Processing**: Multiple reports can be generated concurrently
3. **LaTeX Optimization**: Use `pdflatex` with `-interaction=nonstopmode`
4. **Asset Management**: Optimize images and graphics for faster compilation

## Troubleshooting

### Common Issues

1. **LaTeX Not Found**:
   ```
   Error: LaTeX engine not available
   Solution: Install texlive packages and verify pdflatex is in PATH
   ```

2. **Template Loading Failed**:
   ```
   Error: Failed to load template
   Solution: Check template directory permissions and file existence
   ```

3. **Compilation Timeout**:
   ```
   Error: LaTeX compilation timeout
   Solution: Increase timeout in configuration or optimize template
   ```

4. **Missing Fonts**:
   ```
   Error: Font not found
   Solution: Install required fonts or use system defaults
   ```

### Debug Mode

Enable verbose logging:

```c
latex_config_t *config = latex_config_new();
config->verbose_logging = TRUE;
latex_engine_init_with_config(config);
```

### Log Analysis

Check LaTeX compilation logs:

```bash
# View recent LaTeX logs
tail -f /tmp/openvas-latex/*.log

# Check for common errors
grep -i error /tmp/openvas-latex/*.log
```

## Contributing

### Development Setup

1. Fork the repository
2. Create feature branch: `git checkout -b feature/new-template`
3. Make changes and test thoroughly
4. Submit pull request with detailed description

### Template Contributions

1. Create template in `templates/` directory
2. Include metadata.json with complete variable definitions
3. Test with various vulnerability datasets
4. Document usage and customization options

### Code Style

- Follow GLib coding conventions
- Use descriptive variable names
- Include comprehensive error handling
- Add unit tests for new functionality

## License

This plugin is licensed under GPL-2.0-or-later. See COPYING file for details.

## Support

- **Documentation**: [OpenVAS Documentation](https://docs.openvas.org)
- **Issues**: [GitHub Issues](https://github.com/greenbone/openvas/issues)
- **Community**: [OpenVAS Community Forum](https://community.greenbone.net)
- **Commercial Support**: [Greenbone Networks](https://www.greenbone.net)

## Changelog

### Version 1.0.0 (2025-01-25)
- Initial release with LaTeX report generation
- Executive, technical, and compliance templates
- AI-enhanced reporting capabilities
- Professional chart generation
- Customizable branding and styling
- Multi-format output support