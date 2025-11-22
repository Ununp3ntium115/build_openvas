//! Core types for scanner integration

use serde::{Deserialize, Serialize};
use the_assessor::VulnerabilityScore;

/// Scan Result - Enhanced vulnerability detection
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScanResult {
    pub cve_id: String,
    pub host: String,
    pub port: u16,
    pub plugin_oid: String,
    pub description: String,

    // Enhanced with external data
    pub vulnerability_score: Option<VulnerabilityScore>,

    // AI-enhanced fields
    pub remediation_guidance: Option<String>,

    // Metadata
    pub detection_time: u64,
    pub scanner_version: String,
}

impl ScanResult {
    /// Create a new scan result
    pub fn new(
        cve_id: String,
        host: String,
        port: u16,
        plugin_oid: String,
        description: String,
    ) -> Self {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Self {
            cve_id,
            host,
            port,
            plugin_oid,
            description,
            vulnerability_score: None,
            remediation_guidance: None,
            detection_time: now,
            scanner_version: "CRYPTEX 1.0.0".to_string(),
        }
    }

    /// Get CVSS base score if available
    pub fn cvss_base_score(&self) -> Option<f64> {
        self.vulnerability_score
            .as_ref()
            .map(|s| s.cvss_base_score())
    }

    /// Check if this is a KEV vulnerability
    pub fn is_kev(&self) -> bool {
        self.vulnerability_score
            .as_ref()
            .map(|s| s.is_kev())
            .unwrap_or(false)
    }

    /// Get composite risk score
    pub fn risk_score(&self) -> f64 {
        self.vulnerability_score
            .as_ref()
            .map(|s| s.composite_risk_score())
            .unwrap_or(0.0)
    }
}

/// Scan Report - Comprehensive scan results
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScanReport {
    pub scan_id: String,
    pub target: String,
    pub start_time: u64,
    pub end_time: Option<u64>,
    pub status: ScanStatus,

    // Results
    pub scan_results: Vec<ScanResult>,
    pub total_vulnerabilities: usize,
    pub critical_count: usize,
    pub high_count: usize,
    pub medium_count: usize,
    pub low_count: usize,
    pub kev_count: usize,

    // Statistics
    pub total_hosts: usize,
    pub ai_enhanced_count: usize,

    // Summary
    pub summary: Option<serde_json::Value>,
}

impl ScanReport {
    /// Create a new scan report
    pub fn new(scan_id: String, target: String) -> Self {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Self {
            scan_id,
            target,
            start_time: now,
            end_time: None,
            status: ScanStatus::Running,
            scan_results: Vec::new(),
            total_vulnerabilities: 0,
            critical_count: 0,
            high_count: 0,
            medium_count: 0,
            low_count: 0,
            kev_count: 0,
            total_hosts: 0,
            ai_enhanced_count: 0,
            summary: None,
        }
    }

    /// Add a scan result to the report
    pub fn add_result(&mut self, result: ScanResult) {
        // Update severity counts
        if let Some(ref score) = result.vulnerability_score {
            use the_assessor::CvssSeverity;
            match score.severity() {
                CvssSeverity::Critical => self.critical_count += 1,
                CvssSeverity::High => self.high_count += 1,
                CvssSeverity::Medium => self.medium_count += 1,
                CvssSeverity::Low => self.low_count += 1,
                CvssSeverity::None => {}
            }
        }

        // Update KEV count
        if result.is_kev() {
            self.kev_count += 1;
        }

        // Track AI enhancement
        if result.remediation_guidance.is_some() {
            self.ai_enhanced_count += 1;
        }

        self.total_vulnerabilities += 1;
        self.scan_results.push(result);
    }

    /// Mark scan as completed
    pub fn complete(&mut self) {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        self.end_time = Some(now);
        self.status = ScanStatus::Completed;
    }

    /// Get scan duration in seconds
    pub fn duration_seconds(&self) -> Option<u64> {
        self.end_time.map(|end| end - self.start_time)
    }

    /// Get top N vulnerabilities by risk score
    pub fn top_vulnerabilities(&self, count: usize) -> Vec<&ScanResult> {
        let mut results: Vec<&ScanResult> = self.scan_results.iter().collect();
        results.sort_by(|a, b| {
            b.risk_score()
                .partial_cmp(&a.risk_score())
                .unwrap_or(std::cmp::Ordering::Equal)
        });
        results.into_iter().take(count).collect()
    }

    /// Get only KEV vulnerabilities
    pub fn kev_vulnerabilities(&self) -> Vec<&ScanResult> {
        self.scan_results.iter().filter(|r| r.is_kev()).collect()
    }
}

/// Scan Status
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum ScanStatus {
    Pending,
    Running,
    Completed,
    Failed,
}

/// Bridge Statistics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct BridgeStatistics {
    pub total_scans: u64,
    pub active_scans: u64,
    pub total_vulnerabilities_detected: u64,
    pub kev_vulnerabilities_detected: u64,
    pub critical_vulnerabilities_detected: u64,
    pub ai_enhanced_results: u64,
    pub avg_enhancement_time_ms: f64,
    pub cache_hits: u64,
    pub cache_misses: u64,
}

impl Default for BridgeStatistics {
    fn default() -> Self {
        Self {
            total_scans: 0,
            active_scans: 0,
            total_vulnerabilities_detected: 0,
            kev_vulnerabilities_detected: 0,
            critical_vulnerabilities_detected: 0,
            ai_enhanced_results: 0,
            avg_enhancement_time_ms: 0.0,
            cache_hits: 0,
            cache_misses: 0,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scan_result_creation() {
        let result = ScanResult::new(
            "CVE-2024-0001".to_string(),
            "192.168.1.100".to_string(),
            443,
            "1.3.6.1.4.1.25623.1.0.12345".to_string(),
            "Test vulnerability".to_string(),
        );

        assert_eq!(result.cve_id, "CVE-2024-0001");
        assert_eq!(result.host, "192.168.1.100");
        assert_eq!(result.port, 443);
        assert!(result.cvss_base_score().is_none());
    }

    #[test]
    fn test_scan_report_creation() {
        let report = ScanReport::new(
            "scan-001".to_string(),
            "192.168.1.0/24".to_string(),
        );

        assert_eq!(report.scan_id, "scan-001");
        assert_eq!(report.status, ScanStatus::Running);
        assert_eq!(report.total_vulnerabilities, 0);
    }

    #[test]
    fn test_scan_report_add_result() {
        let mut report = ScanReport::new(
            "scan-001".to_string(),
            "192.168.1.0/24".to_string(),
        );

        let result = ScanResult::new(
            "CVE-2024-0001".to_string(),
            "192.168.1.100".to_string(),
            443,
            "1.3.6.1.4.1.25623.1.0.12345".to_string(),
            "Test vulnerability".to_string(),
        );

        report.add_result(result);

        assert_eq!(report.total_vulnerabilities, 1);
        assert_eq!(report.scan_results.len(), 1);
    }

    #[test]
    fn test_scan_report_completion() {
        let mut report = ScanReport::new(
            "scan-001".to_string(),
            "192.168.1.0/24".to_string(),
        );

        report.complete();

        assert_eq!(report.status, ScanStatus::Completed);
        assert!(report.end_time.is_some());
        assert!(report.duration_seconds().is_some());
    }
}
