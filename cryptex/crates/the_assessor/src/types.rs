//! Core types for vulnerability assessment

use serde::{Deserialize, Serialize};

/// CVSS Severity Levels
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Serialize, Deserialize)]
pub enum CvssSeverity {
    None,      // 0.0
    Low,       // 0.1-3.9
    Medium,    // 4.0-6.9
    High,      // 7.0-8.9
    Critical,  // 9.0-10.0
}

impl CvssSeverity {
    /// Get severity from CVSS base score
    pub fn from_score(score: f64) -> Self {
        match score {
            s if s == 0.0 => CvssSeverity::None,
            s if s < 4.0 => CvssSeverity::Low,
            s if s < 7.0 => CvssSeverity::Medium,
            s if s < 9.0 => CvssSeverity::High,
            _ => CvssSeverity::Critical,
        }
    }

    pub fn as_str(&self) -> &'static str {
        match self {
            CvssSeverity::None => "None",
            CvssSeverity::Low => "Low",
            CvssSeverity::Medium => "Medium",
            CvssSeverity::High => "High",
            CvssSeverity::Critical => "Critical",
        }
    }
}

/// CVSS v3.x Base Metrics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CvssV3Base {
    pub attack_vector: String,        // N/A/L/P
    pub attack_complexity: String,    // L/H
    pub privileges_required: String,  // N/L/H
    pub user_interaction: String,     // N/R
    pub scope: String,                // U/C
    pub confidentiality: String,      // N/L/H
    pub integrity: String,            // N/L/H
    pub availability: String,         // N/L/H
}

/// CVSS v3.x Score
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CvssV3 {
    pub base_metrics: CvssV3Base,
    pub base_score: f64,
    pub temporal_score: Option<f64>,
    pub environmental_score: Option<f64>,
    pub severity: CvssSeverity,
    pub vector_string: String,
}

/// KEV (Known Exploited Vulnerabilities) Information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct KevInfo {
    pub is_kev: bool,
    pub date_added: Option<String>,
    pub due_date: Option<String>,
    pub required_action: Option<String>,
    pub known_ransomware_use: bool,
}

/// EPSS (Exploit Prediction Scoring System) Information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct EpssInfo {
    pub score: f64,         // 0.0-1.0
    pub percentile: f64,    // 0.0-1.0
    pub date: String,
}

/// SSVC (Stakeholder-Specific Vulnerability Categorization) Decision
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum SsvcDecision {
    Track,      // Track vulnerability
    TrackStar,  // Track* (elevated)
    Attend,     // Attend to vulnerability
    Act,        // Act immediately
}

impl SsvcDecision {
    pub fn as_str(&self) -> &'static str {
        match self {
            SsvcDecision::Track => "Track",
            SsvcDecision::TrackStar => "Track*",
            SsvcDecision::Attend => "Attend",
            SsvcDecision::Act => "Act",
        }
    }
}

/// SSVC Information
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct SsvcInfo {
    pub decision: SsvcDecision,
    pub exploitation: String,      // none/poc/active
    pub automatable: bool,
    pub technical_impact: String,  // partial/total
}

/// Comprehensive Vulnerability Score
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct VulnerabilityScore {
    pub cve_id: String,
    pub vulnerability_name: Option<String>,
    pub description: Option<String>,

    // CVSS scores
    pub cvss_v3: Option<CvssV3>,

    // Additional scoring systems
    pub kev: Option<KevInfo>,
    pub epss: Option<EpssInfo>,
    pub ssvc: Option<SsvcInfo>,

    // Metadata
    pub cwe_ids: Vec<String>,
    pub references: Vec<String>,
    pub published_date: Option<String>,
    pub last_modified: Option<String>,

    // AI-enhanced fields
    pub ai_risk_score: Option<f64>,
    pub ai_priority: Option<String>,
    pub ai_remediation_urgency: Option<String>,
}

impl VulnerabilityScore {
    /// Create a new vulnerability score
    pub fn new(cve_id: String) -> Self {
        Self {
            cve_id,
            vulnerability_name: None,
            description: None,
            cvss_v3: None,
            kev: None,
            epss: None,
            ssvc: None,
            cwe_ids: Vec::new(),
            references: Vec::new(),
            published_date: None,
            last_modified: None,
            ai_risk_score: None,
            ai_priority: None,
            ai_remediation_urgency: None,
        }
    }

    /// Get the primary CVSS base score
    pub fn cvss_base_score(&self) -> f64 {
        self.cvss_v3.as_ref().map(|c| c.base_score).unwrap_or(0.0)
    }

    /// Get the severity level
    pub fn severity(&self) -> CvssSeverity {
        self.cvss_v3
            .as_ref()
            .map(|c| c.severity)
            .unwrap_or(CvssSeverity::None)
    }

    /// Check if this is a known exploited vulnerability
    pub fn is_kev(&self) -> bool {
        self.kev.as_ref().map(|k| k.is_kev).unwrap_or(false)
    }

    /// Get composite risk score (combines CVSS, EPSS, KEV, AI)
    pub fn composite_risk_score(&self) -> f64 {
        let mut score = self.cvss_base_score() / 10.0; // Normalize to 0-1

        // EPSS weight
        if let Some(epss) = &self.epss {
            score = score * 0.6 + epss.score * 0.4;
        }

        // KEV multiplier
        if self.is_kev() {
            score *= 1.5;
        }

        // AI risk score override if available
        if let Some(ai_score) = self.ai_risk_score {
            score = score * 0.7 + (ai_score / 10.0) * 0.3;
        }

        // Clamp to 0-1
        score.min(1.0).max(0.0)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_severity_from_score() {
        assert_eq!(CvssSeverity::from_score(0.0), CvssSeverity::None);
        assert_eq!(CvssSeverity::from_score(3.5), CvssSeverity::Low);
        assert_eq!(CvssSeverity::from_score(5.0), CvssSeverity::Medium);
        assert_eq!(CvssSeverity::from_score(7.5), CvssSeverity::High);
        assert_eq!(CvssSeverity::from_score(9.8), CvssSeverity::Critical);
    }

    #[test]
    fn test_vulnerability_score_creation() {
        let score = VulnerabilityScore::new("CVE-2024-0001".to_string());
        assert_eq!(score.cve_id, "CVE-2024-0001");
        assert_eq!(score.cvss_base_score(), 0.0);
        assert_eq!(score.severity(), CvssSeverity::None);
    }

    #[test]
    fn test_composite_risk_score() {
        let mut score = VulnerabilityScore::new("CVE-2024-0001".to_string());
        score.cvss_v3 = Some(CvssV3 {
            base_metrics: CvssV3Base {
                attack_vector: "N".to_string(),
                attack_complexity: "L".to_string(),
                privileges_required: "N".to_string(),
                user_interaction: "N".to_string(),
                scope: "C".to_string(),
                confidentiality: "H".to_string(),
                integrity: "H".to_string(),
                availability: "H".to_string(),
            },
            base_score: 9.8,
            temporal_score: None,
            environmental_score: None,
            severity: CvssSeverity::Critical,
            vector_string: "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H".to_string(),
        });

        let risk = score.composite_risk_score();
        assert!(risk > 0.9); // High risk for critical vuln
    }
}
