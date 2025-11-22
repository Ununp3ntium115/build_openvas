//! Scanner Bridge - Main API

use crate::context::*;
use crate::types::*;
use std::collections::HashMap;
use std::sync::Arc;
use std::time::Instant;
use tokio::sync::RwLock;
use the_assessor::TheAssessor;
use the_foundation::{CryptexError, CryptexResult};

/// The Infiltrator - Scanner Integration Bridge
///
/// Traditional name: `ScannerBridge`
///
/// Coordinates between OpenVAS scanner core and CRYPTEX's vulnerability assessment.
pub struct TheInfiltrator {
    /// Active scan contexts
    scans: Arc<RwLock<HashMap<String, ScanContext>>>,

    /// Vulnerability assessor
    assessor: Arc<TheAssessor>,

    /// Bridge statistics
    stats: Arc<RwLock<BridgeStatistics>>,

    /// Enable AI enhancement
    enable_ai_enhancement: bool,
}

impl TheInfiltrator {
    /// The Awakening - Initialize The Infiltrator
    ///
    /// Traditional name: `init` or `new`
    pub async fn the_awakening() -> CryptexResult<Self> {
        tracing::info!("The Infiltrator is awakening...");

        let assessor = TheAssessor::the_awakening().await?;

        Ok(Self {
            scans: Arc::new(RwLock::new(HashMap::new())),
            assessor: Arc::new(assessor),
            stats: Arc::new(RwLock::new(BridgeStatistics::default())),
            enable_ai_enhancement: true,
        })
    }

    /// Start a new scan
    ///
    /// Traditional name: `start_scan`
    pub async fn start_scan(&self, target: &str) -> CryptexResult<String> {
        let scan_id = uuid::Uuid::new_v4().to_string();

        tracing::info!("Starting scan {} for target: {}", scan_id, target);

        let context = ScanContext::new(scan_id.clone(), target.to_string());

        {
            let mut scans = self.scans.write().await;
            scans.insert(scan_id.clone(), context);

            let mut stats = self.stats.write().await;
            stats.total_scans += 1;
            stats.active_scans += 1;
        }

        Ok(scan_id)
    }

    /// Report a vulnerability detection
    ///
    /// Traditional name: `on_vulnerability_detected`
    ///
    /// This is called by the scanner when a vulnerability is detected.
    /// The result is enhanced with CVSS, KEV, EPSS, and AI-generated guidance.
    pub async fn on_vulnerability_detected(
        &self,
        scan_id: &str,
        cve_id: &str,
        host: &str,
        port: u16,
    ) -> CryptexResult<()> {
        let start = Instant::now();

        tracing::debug!(
            "Vulnerability detected in scan {}: {} on {}:{}",
            scan_id,
            cve_id,
            host,
            port
        );

        // Create base scan result
        let mut result = ScanResult::new(
            cve_id.to_string(),
            host.to_string(),
            port,
            "unknown".to_string(), // Would be provided by scanner
            format!("Vulnerability {} detected", cve_id),
        );

        // Enhance with vulnerability assessment
        match self.assessor.assess_vulnerability(cve_id).await {
            Ok(score) => {
                result.vulnerability_score = Some(score.clone());

                // Generate AI remediation guidance if enabled
                if self.enable_ai_enhancement {
                    result.remediation_guidance = Some(self.generate_remediation_guidance(&score).await);

                    let mut stats = self.stats.write().await;
                    stats.ai_enhanced_results += 1;
                }

                // Update statistics
                {
                    let mut stats = self.stats.write().await;
                    stats.total_vulnerabilities_detected += 1;

                    if score.is_kev() {
                        stats.kev_vulnerabilities_detected += 1;
                    }

                    use the_assessor::CvssSeverity;
                    if score.severity() == CvssSeverity::Critical {
                        stats.critical_vulnerabilities_detected += 1;
                    }

                    // Update average enhancement time
                    let elapsed = start.elapsed().as_millis() as f64;
                    let total = stats.ai_enhanced_results as f64;
                    stats.avg_enhancement_time_ms =
                        (stats.avg_enhancement_time_ms * (total - 1.0) + elapsed) / total;
                }

                tracing::info!(
                    "Enhanced {} - CVSS: {:.1}, Severity: {}, KEV: {}",
                    cve_id,
                    score.cvss_base_score(),
                    score.severity().as_str(),
                    score.is_kev()
                );
            }
            Err(e) => {
                tracing::warn!("Failed to assess {}: {}", cve_id, e);
            }
        }

        // Add result to scan context
        {
            let mut scans = self.scans.write().await;
            if let Some(ctx) = scans.get_mut(scan_id) {
                ctx.add_result(result);
            } else {
                return Err(CryptexError::not_found(format!("Scan not found: {}", scan_id)));
            }
        }

        Ok(())
    }

    /// End a scan and generate report
    ///
    /// Traditional name: `end_scan`
    pub async fn end_scan(&self, scan_id: &str) -> CryptexResult<ScanReport> {
        tracing::info!("Ending scan: {}", scan_id);

        let report = {
            let mut scans = self.scans.write().await;
            let ctx = scans
                .remove(scan_id)
                .ok_or_else(|| CryptexError::not_found(format!("Scan not found: {}", scan_id)))?;

            ctx.generate_report()
        };

        // Update statistics
        {
            let mut stats = self.stats.write().await;
            stats.active_scans = stats.active_scans.saturating_sub(1);
        }

        tracing::info!(
            "Scan {} complete: {} vulnerabilities ({} critical, {} KEV)",
            scan_id,
            report.total_vulnerabilities,
            report.critical_count,
            report.kev_count
        );

        Ok(report)
    }

    /// Get active scan context
    pub async fn get_scan_context(&self, scan_id: &str) -> CryptexResult<ScanReport> {
        let scans = self.scans.read().await;
        let ctx = scans
            .get(scan_id)
            .ok_or_else(|| CryptexError::not_found(format!("Scan not found: {}", scan_id)))?;

        Ok(ctx.generate_report())
    }

    /// Get bridge statistics
    pub async fn get_statistics(&self) -> BridgeStatistics {
        let stats = self.stats.read().await;
        stats.clone()
    }

    /// Reset statistics
    pub async fn reset_statistics(&self) {
        let mut stats = self.stats.write().await;
        *stats = BridgeStatistics::default();
        tracing::info!("Bridge statistics reset");
    }

    // Private methods

    /// Generate AI remediation guidance
    async fn generate_remediation_guidance(&self, score: &the_assessor::VulnerabilityScore) -> String {
        // In a real implementation, this would call The Collective for AI-generated guidance
        // For now, use template-based guidance

        let mut guidance = String::new();

        guidance.push_str(&format!(
            "Vulnerability: {}\n",
            score.vulnerability_name.as_deref().unwrap_or(&score.cve_id)
        ));

        guidance.push_str(&format!(
            "Severity: {} (CVSS {:.1})\n\n",
            score.severity().as_str(),
            score.cvss_base_score()
        ));

        if score.is_kev() {
            guidance.push_str("⚠️  This is a Known Exploited Vulnerability (KEV)\n");
            if let Some(kev) = &score.kev {
                if let Some(due_date) = &kev.due_date {
                    guidance.push_str(&format!("   Required action by: {}\n", due_date));
                }
                if let Some(action) = &kev.required_action {
                    guidance.push_str(&format!("   Action: {}\n", action));
                }
            }
            guidance.push_str("\n");
        }

        if let Some(priority) = &score.ai_priority {
            guidance.push_str(&format!("Priority: {}\n", priority));
        }

        if let Some(urgency) = &score.ai_remediation_urgency {
            guidance.push_str(&format!("Remediation: {}\n", urgency));
        }

        if let Some(epss) = &score.epss {
            guidance.push_str(&format!(
                "\nExploit Likelihood: {:.1}% (EPSS: {:.4})\n",
                epss.percentile * 100.0,
                epss.score
            ));
        }

        guidance.push_str("\nRecommended Actions:\n");
        match score.severity() {
            the_assessor::CvssSeverity::Critical | the_assessor::CvssSeverity::High => {
                guidance.push_str("1. Apply vendor patches immediately\n");
                guidance.push_str("2. Implement network segmentation if patching is delayed\n");
                guidance.push_str("3. Monitor for exploitation attempts\n");
            }
            the_assessor::CvssSeverity::Medium => {
                guidance.push_str("1. Schedule patching within 30 days\n");
                guidance.push_str("2. Verify patch availability\n");
            }
            _ => {
                guidance.push_str("1. Include in regular patch cycle\n");
            }
        }

        guidance
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_infiltrator_awakening() {
        let infiltrator = TheInfiltrator::the_awakening().await.unwrap();
        assert!(infiltrator.enable_ai_enhancement);
    }

    #[tokio::test]
    async fn test_start_end_scan() {
        let infiltrator = TheInfiltrator::the_awakening().await.unwrap();

        let scan_id = infiltrator.start_scan("192.168.1.0/24").await.unwrap();
        assert!(!scan_id.is_empty());

        let report = infiltrator.end_scan(&scan_id).await.unwrap();
        assert_eq!(report.scan_id, scan_id);
        assert_eq!(report.target, "192.168.1.0/24");
    }

    #[tokio::test]
    async fn test_vulnerability_detection() {
        let infiltrator = TheInfiltrator::the_awakening().await.unwrap();

        let scan_id = infiltrator.start_scan("192.168.1.0/24").await.unwrap();

        infiltrator
            .on_vulnerability_detected(&scan_id, "CVE-2021-44228", "192.168.1.100", 443)
            .await
            .unwrap();

        let report = infiltrator.end_scan(&scan_id).await.unwrap();
        assert_eq!(report.total_vulnerabilities, 1);
        assert!(report.kev_count > 0); // Log4Shell is KEV
    }

    #[tokio::test]
    async fn test_statistics() {
        let infiltrator = TheInfiltrator::the_awakening().await.unwrap();

        let scan_id = infiltrator.start_scan("192.168.1.0/24").await.unwrap();

        infiltrator
            .on_vulnerability_detected(&scan_id, "CVE-2021-44228", "192.168.1.100", 443)
            .await
            .unwrap();

        let stats = infiltrator.get_statistics().await;
        assert_eq!(stats.total_scans, 1);
        assert!(stats.total_vulnerabilities_detected > 0);
    }

    #[tokio::test]
    async fn test_scan_not_found() {
        let infiltrator = TheInfiltrator::the_awakening().await.unwrap();

        let result = infiltrator.end_scan("non-existent-scan").await;
        assert!(result.is_err());
    }
}
