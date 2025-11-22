//! Scan and Host Context Management

use crate::types::*;
use std::collections::HashMap;
use the_assessor::VulnerabilityScore;

/// Scan Context - Tracks state for an active scan
#[derive(Debug)]
pub struct ScanContext {
    pub scan_id: String,
    pub target: String,
    pub start_time: u64,
    pub hosts: HashMap<String, HostContext>,
    pub results: Vec<ScanResult>,
}

impl ScanContext {
    /// Create a new scan context
    pub fn new(scan_id: String, target: String) -> Self {
        let now = std::time::SystemTime::now()
            .duration_since(std::time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        Self {
            scan_id,
            target,
            start_time: now,
            hosts: HashMap::new(),
            results: Vec::new(),
        }
    }

    /// Get or create host context
    pub fn get_or_create_host(&mut self, host: String) -> &mut HostContext {
        self.hosts.entry(host.clone()).or_insert_with(|| {
            tracing::debug!("Creating new host context for: {}", host);
            HostContext::new(host)
        })
    }

    /// Add a scan result
    pub fn add_result(&mut self, result: ScanResult) {
        // Update host context
        let host = result.host.clone();
        if let Some(host_ctx) = self.hosts.get_mut(&host) {
            host_ctx.add_vulnerability(result.cve_id.clone());
        }

        self.results.push(result);
    }

    /// Generate scan report
    pub fn generate_report(&self) -> ScanReport {
        let mut report = ScanReport::new(self.scan_id.clone(), self.target.clone());
        report.start_time = self.start_time;
        report.total_hosts = self.hosts.len();

        for result in &self.results {
            report.add_result(result.clone());
        }

        report.complete();
        report
    }
}

/// Host Context - Tracks vulnerability profile for a specific host
#[derive(Debug, Clone)]
pub struct HostContext {
    pub host: String,
    pub vulnerabilities: Vec<String>,
    pub services: HashMap<u16, ServiceInfo>,
    pub risk_score: f64,
}

impl HostContext {
    /// Create a new host context
    pub fn new(host: String) -> Self {
        Self {
            host,
            vulnerabilities: Vec::new(),
            services: HashMap::new(),
            risk_score: 0.0,
        }
    }

    /// Add a vulnerability to this host
    pub fn add_vulnerability(&mut self, cve_id: String) {
        if !self.vulnerabilities.contains(&cve_id) {
            self.vulnerabilities.push(cve_id);
        }
    }

    /// Update service information
    pub fn add_service(&mut self, port: u16, service: ServiceInfo) {
        self.services.insert(port, service);
    }

    /// Update overall risk score for this host
    pub fn update_risk_score(&mut self, scores: &[VulnerabilityScore]) {
        if scores.is_empty() {
            self.risk_score = 0.0;
            return;
        }

        // Calculate aggregate risk (average of composite risk scores)
        let total_risk: f64 = scores.iter().map(|s| s.composite_risk_score()).sum();
        self.risk_score = total_risk / scores.len() as f64;
    }
}

/// Service Information
#[derive(Debug, Clone)]
pub struct ServiceInfo {
    pub port: u16,
    pub protocol: String,
    pub service_name: String,
    pub version: Option<String>,
    pub vulnerabilities: Vec<String>,
}

impl ServiceInfo {
    /// Create a new service info
    pub fn new(port: u16, protocol: String, service_name: String) -> Self {
        Self {
            port,
            protocol,
            service_name,
            version: None,
            vulnerabilities: Vec::new(),
        }
    }

    /// Add vulnerability to this service
    pub fn add_vulnerability(&mut self, cve_id: String) {
        if !self.vulnerabilities.contains(&cve_id) {
            self.vulnerabilities.push(cve_id);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_scan_context_creation() {
        let ctx = ScanContext::new(
            "scan-001".to_string(),
            "192.168.1.0/24".to_string(),
        );

        assert_eq!(ctx.scan_id, "scan-001");
        assert_eq!(ctx.target, "192.168.1.0/24");
        assert!(ctx.hosts.is_empty());
        assert!(ctx.results.is_empty());
    }

    #[test]
    fn test_get_or_create_host() {
        let mut ctx = ScanContext::new(
            "scan-001".to_string(),
            "192.168.1.0/24".to_string(),
        );

        let host1 = ctx.get_or_create_host("192.168.1.100".to_string());
        assert_eq!(host1.host, "192.168.1.100");

        // Second call should return same context
        let host2 = ctx.get_or_create_host("192.168.1.100".to_string());
        assert_eq!(host2.host, "192.168.1.100");

        assert_eq!(ctx.hosts.len(), 1);
    }

    #[test]
    fn test_host_context_add_vulnerability() {
        let mut host = HostContext::new("192.168.1.100".to_string());

        host.add_vulnerability("CVE-2024-0001".to_string());
        host.add_vulnerability("CVE-2024-0002".to_string());
        host.add_vulnerability("CVE-2024-0001".to_string()); // Duplicate

        assert_eq!(host.vulnerabilities.len(), 2);
    }

    #[test]
    fn test_service_info() {
        let mut service = ServiceInfo::new(
            443,
            "tcp".to_string(),
            "https".to_string(),
        );

        service.version = Some("Apache/2.4.41".to_string());
        service.add_vulnerability("CVE-2024-0001".to_string());

        assert_eq!(service.port, 443);
        assert_eq!(service.vulnerabilities.len(), 1);
    }
}
