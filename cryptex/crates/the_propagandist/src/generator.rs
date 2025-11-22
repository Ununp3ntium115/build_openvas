//! Report Generator - Main API

use crate::formats::*;
use chrono::{DateTime, Utc};
use the_foundation::{CryptexError, CryptexResult};
use the_infiltrator::ScanReport;

/// The Propagandist - Report Generation System
///
/// Traditional name: `ReportGenerator`
pub struct ThePropagandist {
    /// Default configuration
    default_config: ReportConfig,
}

impl ThePropagandist {
    /// The Awakening - Initialize The Propagandist
    ///
    /// Traditional name: `init` or `new`
    pub async fn the_awakening() -> CryptexResult<Self> {
        tracing::info!("The Propagandist is awakening...");

        Ok(Self {
            default_config: ReportConfig::default(),
        })
    }

    /// Generate a report in the specified format
    ///
    /// Traditional name: `generate_report`
    pub async fn generate_report(
        &self,
        scan_report: &ScanReport,
        format: ReportFormat,
    ) -> CryptexResult<String> {
        let mut config = self.default_config.clone();
        config.format = format;

        self.generate_with_config(scan_report, &config).await
    }

    /// Generate a report with custom configuration
    pub async fn generate_with_config(
        &self,
        scan_report: &ScanReport,
        config: &ReportConfig,
    ) -> CryptexResult<String> {
        tracing::debug!(
            "Generating {:?} report for scan: {}",
            config.format,
            scan_report.scan_id
        );

        match config.format {
            ReportFormat::Json => self.generate_json(scan_report, config).await,
            ReportFormat::Html => self.generate_html(scan_report, config).await,
            ReportFormat::Markdown => self.generate_markdown(scan_report, config).await,
            ReportFormat::Text => self.generate_text(scan_report, config).await,
        }
    }

    /// Generate executive summary
    ///
    /// Traditional name: `generate_summary`
    pub async fn generate_executive_summary(
        &self,
        scan_report: &ScanReport,
    ) -> CryptexResult<String> {
        let mut summary = String::new();

        summary.push_str("# CRYPTEX Vulnerability Assessment - Executive Summary\n\n");

        // Overview
        summary.push_str("## Scan Overview\n\n");
        summary.push_str(&format!("**Scan ID**: {}\n", scan_report.scan_id));
        summary.push_str(&format!("**Target**: {}\n", scan_report.target));

        if let Some(duration) = scan_report.duration_seconds() {
            summary.push_str(&format!("**Duration**: {} seconds\n", duration));
        }

        summary.push_str(&format!("**Total Hosts**: {}\n\n", scan_report.total_hosts));

        // Key Findings
        summary.push_str("## Key Findings\n\n");
        summary.push_str(&format!(
            "- **Total Vulnerabilities**: {}\n",
            scan_report.total_vulnerabilities
        ));
        summary.push_str(&format!(
            "- **Critical**: {} vulnerabilities requiring immediate action\n",
            scan_report.critical_count
        ));
        summary.push_str(&format!(
            "- **High**: {} vulnerabilities requiring prompt attention\n",
            scan_report.high_count
        ));
        summary.push_str(&format!("- **Medium**: {}\n", scan_report.medium_count));
        summary.push_str(&format!("- **Low**: {}\n\n", scan_report.low_count));

        // KEV Alert
        if scan_report.kev_count > 0 {
            summary.push_str("### ⚠️ Known Exploited Vulnerabilities (KEV)\n\n");
            summary.push_str(&format!(
                "**{}** vulnerabilities detected are on the CISA KEV catalog, \
                 indicating active exploitation in the wild. \
                 These require immediate remediation.\n\n",
                scan_report.kev_count
            ));
        }

        // Risk Assessment
        summary.push_str("## Risk Assessment\n\n");
        let risk_level = if scan_report.critical_count > 0 || scan_report.kev_count > 0 {
            "**CRITICAL** - Immediate action required"
        } else if scan_report.high_count > 0 {
            "**HIGH** - Prompt remediation needed"
        } else if scan_report.medium_count > 0 {
            "**MEDIUM** - Address in regular patch cycle"
        } else {
            "**LOW** - Minimal risk"
        };

        summary.push_str(&format!("Overall Risk Level: {}\n\n", risk_level));

        // Top Vulnerabilities
        if !scan_report.scan_results.is_empty() {
            summary.push_str("## Top Vulnerabilities (By Risk)\n\n");
            let top = scan_report.top_vulnerabilities(5);
            for (i, result) in top.iter().enumerate() {
                summary.push_str(&format!("{}. **{}** - {}\n", i + 1, result.cve_id, result.host));
                if let Some(score) = result.cvss_base_score() {
                    summary.push_str(&format!("   - CVSS Score: {:.1}\n", score));
                }
                if result.is_kev() {
                    summary.push_str("   - ⚠️ Known Exploited Vulnerability\n");
                }
                summary.push('\n');
            }
        }

        // Recommendations
        summary.push_str("## Recommended Actions\n\n");
        if scan_report.kev_count > 0 {
            summary.push_str("1. **Immediate**: Patch all KEV vulnerabilities within 24-48 hours\n");
        }
        if scan_report.critical_count > 0 {
            summary.push_str("2. **High Priority**: Address critical vulnerabilities within 7 days\n");
        }
        if scan_report.high_count > 0 {
            summary.push_str("3. **Standard**: Remediate high severity issues within 30 days\n");
        }
        summary.push_str("4. **Ongoing**: Implement continuous vulnerability scanning\n");
        summary.push_str("5. **Process**: Review and update security policies based on findings\n\n");

        summary.push_str("---\n");
        summary.push_str(&format!(
            "Report generated: {}\n",
            Utc::now().format("%Y-%m-%d %H:%M:%S UTC")
        ));

        Ok(summary)
    }

    // Private format generators

    /// Generate JSON format report
    async fn generate_json(
        &self,
        scan_report: &ScanReport,
        _config: &ReportConfig,
    ) -> CryptexResult<String> {
        serde_json::to_string_pretty(scan_report)
            .map_err(|e| CryptexError::validation(format!("JSON serialization failed: {}", e)))
    }

    /// Generate HTML format report
    async fn generate_html(
        &self,
        scan_report: &ScanReport,
        config: &ReportConfig,
    ) -> CryptexResult<String> {
        let mut html = String::new();

        // HTML header
        html.push_str("<!DOCTYPE html>\n");
        html.push_str("<html lang=\"en\">\n<head>\n");
        html.push_str("  <meta charset=\"UTF-8\">\n");
        html.push_str("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n");
        html.push_str("  <title>CRYPTEX Vulnerability Report</title>\n");
        html.push_str("  <style>\n");
        html.push_str("    body { font-family: sans-serif; margin: 20px; background: #f5f5f5; }\n");
        html.push_str("    .container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; }\n");
        html.push_str("    h1 { color: #333; border-bottom: 3px solid #e74c3c; }\n");
        html.push_str("    .critical { color: #e74c3c; font-weight: bold; }\n");
        html.push_str("    .high { color: #f39c12; font-weight: bold; }\n");
        html.push_str("    .medium { color: #3498db; }\n");
        html.push_str("    .kev-badge { background: #e74c3c; color: white; padding: 2px 8px; border-radius: 4px; font-size: 0.8em; }\n");
        html.push_str("    table { width: 100%; border-collapse: collapse; margin: 20px 0; }\n");
        html.push_str("    th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }\n");
        html.push_str("    th { background: #34495e; color: white; }\n");
        html.push_str("  </style>\n</head>\n<body>\n");
        html.push_str("<div class=\"container\">\n");

        // Title
        html.push_str("  <h1>CRYPTEX Vulnerability Assessment Report</h1>\n");

        // Overview
        html.push_str("  <h2>Scan Overview</h2>\n");
        html.push_str("  <table>\n");
        html.push_str(&format!("    <tr><td><strong>Scan ID</strong></td><td>{}</td></tr>\n", scan_report.scan_id));
        html.push_str(&format!("    <tr><td><strong>Target</strong></td><td>{}</td></tr>\n", scan_report.target));
        html.push_str(&format!("    <tr><td><strong>Total Hosts</strong></td><td>{}</td></tr>\n", scan_report.total_hosts));
        if let Some(duration) = scan_report.duration_seconds() {
            html.push_str(&format!("    <tr><td><strong>Duration</strong></td><td>{} seconds</td></tr>\n", duration));
        }
        html.push_str("  </table>\n");

        // Statistics
        html.push_str("  <h2>Vulnerability Statistics</h2>\n");
        html.push_str("  <table>\n");
        html.push_str(&format!("    <tr><td><strong>Total Vulnerabilities</strong></td><td>{}</td></tr>\n", scan_report.total_vulnerabilities));
        html.push_str(&format!("    <tr><td><strong>Critical</strong></td><td class=\"critical\">{}</td></tr>\n", scan_report.critical_count));
        html.push_str(&format!("    <tr><td><strong>High</strong></td><td class=\"high\">{}</td></tr>\n", scan_report.high_count));
        html.push_str(&format!("    <tr><td><strong>Medium</strong></td><td class=\"medium\">{}</td></tr>\n", scan_report.medium_count));
        html.push_str(&format!("    <tr><td><strong>Low</strong></td><td>{}</td></tr>\n", scan_report.low_count));
        if scan_report.kev_count > 0 {
            html.push_str(&format!("    <tr><td><strong>KEV Vulnerabilities</strong></td><td><span class=\"kev-badge\">{} KEV</span></td></tr>\n", scan_report.kev_count));
        }
        html.push_str("  </table>\n");

        // Top vulnerabilities (if executive)
        if config.audience == ReportAudience::Executive {
            let top = scan_report.top_vulnerabilities(10);
            if !top.is_empty() {
                html.push_str("  <h2>Top Vulnerabilities by Risk</h2>\n");
                html.push_str("  <table>\n");
                html.push_str("    <tr><th>CVE</th><th>Host</th><th>Port</th><th>CVSS</th><th>Status</th></tr>\n");
                for result in top {
                    html.push_str("    <tr>");
                    html.push_str(&format!("<td>{}</td>", result.cve_id));
                    html.push_str(&format!("<td>{}</td>", result.host));
                    html.push_str(&format!("<td>{}</td>", result.port));
                    if let Some(score) = result.cvss_base_score() {
                        let class = if score >= 9.0 { "critical" } else if score >= 7.0 { "high" } else { "medium" };
                        html.push_str(&format!("<td class=\"{}\">{:.1}</td>", class, score));
                    } else {
                        html.push_str("<td>-</td>");
                    }
                    if result.is_kev() {
                        html.push_str("<td><span class=\"kev-badge\">KEV</span></td>");
                    } else {
                        html.push_str("<td>-</td>");
                    }
                    html.push_str("</tr>\n");
                }
                html.push_str("  </table>\n");
            }
        }

        // Footer
        html.push_str(&format!("  <p style=\"margin-top: 40px; color: #7f8c8d; font-size: 0.9em;\">Report generated: {}</p>\n",
                              Utc::now().format("%Y-%m-%d %H:%M:%S UTC")));
        html.push_str("</div>\n</body>\n</html>");

        Ok(html)
    }

    /// Generate Markdown format report
    async fn generate_markdown(
        &self,
        scan_report: &ScanReport,
        _config: &ReportConfig,
    ) -> CryptexResult<String> {
        // Similar to executive summary but in pure markdown
        self.generate_executive_summary(scan_report).await
    }

    /// Generate plain text format report
    async fn generate_text(
        &self,
        scan_report: &ScanReport,
        _config: &ReportConfig,
    ) -> CryptexResult<String> {
        let mut text = String::new();

        text.push_str("CRYPTEX VULNERABILITY ASSESSMENT REPORT\n");
        text.push_str("=======================================\n\n");

        text.push_str(&format!("Scan ID:      {}\n", scan_report.scan_id));
        text.push_str(&format!("Target:       {}\n", scan_report.target));
        text.push_str(&format!("Total Hosts:  {}\n", scan_report.total_hosts));
        if let Some(duration) = scan_report.duration_seconds() {
            text.push_str(&format!("Duration:     {} seconds\n", duration));
        }
        text.push_str("\n");

        text.push_str("VULNERABILITY SUMMARY\n");
        text.push_str("---------------------\n");
        text.push_str(&format!("Total:     {}\n", scan_report.total_vulnerabilities));
        text.push_str(&format!("Critical:  {}\n", scan_report.critical_count));
        text.push_str(&format!("High:      {}\n", scan_report.high_count));
        text.push_str(&format!("Medium:    {}\n", scan_report.medium_count));
        text.push_str(&format!("Low:       {}\n", scan_report.low_count));
        if scan_report.kev_count > 0 {
            text.push_str(&format!("KEV:       {} (Known Exploited Vulnerabilities)\n", scan_report.kev_count));
        }
        text.push_str("\n");

        text.push_str(&format!("Report generated: {}\n",
                              Utc::now().format("%Y-%m-%d %H:%M:%S UTC")));

        Ok(text)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use the_infiltrator::ScanReport;

    #[tokio::test]
    async fn test_propagandist_awakening() {
        let propagandist = ThePropagandist::the_awakening().await.unwrap();
        assert_eq!(propagandist.default_config.format, ReportFormat::Json);
    }

    #[tokio::test]
    async fn test_generate_json_report() {
        let propagandist = ThePropagandist::the_awakening().await.unwrap();
        let report = ScanReport::new("scan-001".to_string(), "192.168.1.0/24".to_string());

        let json = propagandist
            .generate_report(&report, ReportFormat::Json)
            .await
            .unwrap();

        assert!(json.contains("scan-001"));
        assert!(json.contains("192.168.1.0/24"));
    }

    #[tokio::test]
    async fn test_generate_executive_summary() {
        let propagandist = ThePropagandist::the_awakening().await.unwrap();
        let report = ScanReport::new("scan-001".to_string(), "192.168.1.0/24".to_string());

        let summary = propagandist
            .generate_executive_summary(&report)
            .await
            .unwrap();

        assert!(summary.contains("Executive Summary"));
        assert!(summary.contains("scan-001"));
    }

    #[tokio::test]
    async fn test_generate_html_report() {
        let propagandist = ThePropagandist::the_awakening().await.unwrap();
        let report = ScanReport::new("scan-001".to_string(), "192.168.1.0/24".to_string());

        let html = propagandist
            .generate_report(&report, ReportFormat::Html)
            .await
            .unwrap();

        assert!(html.contains("<!DOCTYPE html>"));
        assert!(html.contains("scan-001"));
    }

    #[tokio::test]
    async fn test_generate_text_report() {
        let propagandist = ThePropagandist::the_awakening().await.unwrap();
        let report = ScanReport::new("scan-001".to_string(), "192.168.1.0/24".to_string());

        let text = propagandist
            .generate_report(&report, ReportFormat::Text)
            .await
            .unwrap();

        assert!(text.contains("CRYPTEX VULNERABILITY ASSESSMENT REPORT"));
        assert!(text.contains("scan-001"));
    }
}
