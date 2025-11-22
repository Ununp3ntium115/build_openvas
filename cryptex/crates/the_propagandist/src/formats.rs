//! Report format types and utilities

use serde::{Deserialize, Serialize};

/// Report Format
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum ReportFormat {
    /// JSON format for API consumption
    Json,
    /// HTML format for web viewing
    Html,
    /// Markdown format for documentation
    Markdown,
    /// Plain text format
    Text,
}

impl ReportFormat {
    /// Get file extension for this format
    pub fn extension(&self) -> &'static str {
        match self {
            ReportFormat::Json => "json",
            ReportFormat::Html => "html",
            ReportFormat::Markdown => "md",
            ReportFormat::Text => "txt",
        }
    }

    /// Get MIME type for this format
    pub fn mime_type(&self) -> &'static str {
        match self {
            ReportFormat::Json => "application/json",
            ReportFormat::Html => "text/html",
            ReportFormat::Markdown => "text/markdown",
            ReportFormat::Text => "text/plain",
        }
    }
}

/// Report Audience - determines level of detail and technical depth
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum ReportAudience {
    /// Technical team - full details
    Technical,
    /// Management/Executive - high-level overview
    Executive,
    /// Compliance/Audit - policy-focused
    Compliance,
}

/// Report Configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ReportConfig {
    /// Report format
    pub format: ReportFormat,

    /// Target audience
    pub audience: ReportAudience,

    /// Include remediation guidance
    pub include_remediation: bool,

    /// Include compliance mappings
    pub include_compliance: bool,

    /// Maximum vulnerabilities to include in detail
    pub max_detailed_vulnerabilities: Option<usize>,

    /// Only include vulnerabilities above this severity
    pub min_severity: Option<the_assessor::CvssSeverity>,
}

impl Default for ReportConfig {
    fn default() -> Self {
        Self {
            format: ReportFormat::Json,
            audience: ReportAudience::Technical,
            include_remediation: true,
            include_compliance: false,
            max_detailed_vulnerabilities: None,
            min_severity: None,
        }
    }
}

impl ReportConfig {
    /// Create a technical report configuration
    pub fn technical() -> Self {
        Self {
            audience: ReportAudience::Technical,
            include_remediation: true,
            include_compliance: false,
            ..Default::default()
        }
    }

    /// Create an executive report configuration
    pub fn executive() -> Self {
        Self {
            audience: ReportAudience::Executive,
            include_remediation: false,
            include_compliance: false,
            max_detailed_vulnerabilities: Some(10),
            min_severity: Some(the_assessor::CvssSeverity::High),
            ..Default::default()
        }
    }

    /// Create a compliance report configuration
    pub fn compliance() -> Self {
        Self {
            audience: ReportAudience::Compliance,
            include_remediation: true,
            include_compliance: true,
            ..Default::default()
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_report_format_extension() {
        assert_eq!(ReportFormat::Json.extension(), "json");
        assert_eq!(ReportFormat::Html.extension(), "html");
        assert_eq!(ReportFormat::Markdown.extension(), "md");
    }

    #[test]
    fn test_report_config_default() {
        let config = ReportConfig::default();
        assert_eq!(config.format, ReportFormat::Json);
        assert_eq!(config.audience, ReportAudience::Technical);
        assert!(config.include_remediation);
    }

    #[test]
    fn test_executive_config() {
        let config = ReportConfig::executive();
        assert_eq!(config.audience, ReportAudience::Executive);
        assert_eq!(config.max_detailed_vulnerabilities, Some(10));
    }
}
