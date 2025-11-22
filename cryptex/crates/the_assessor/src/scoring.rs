//! Comprehensive Vulnerability Scoring and Assessment

use crate::types::*;
use crate::cvss::*;
use std::collections::HashMap;
use std::sync::Arc;
use tokio::sync::RwLock;
use the_foundation::CryptexResult;

/// The Assessor - Comprehensive Vulnerability Scoring System
///
/// Traditional name: `VulnerabilityScorer`
///
/// Coordinates CVSS scoring, external data enrichment, and AI-enhanced risk assessment.
pub struct TheAssessor {
    /// Cache of vulnerability scores
    score_cache: Arc<RwLock<HashMap<String, VulnerabilityScore>>>,

    /// Configuration
    enable_ai_enhancement: bool,
}

impl TheAssessor {
    /// The Awakening - Initialize The Assessor
    ///
    /// Traditional name: `init` or `new`
    pub async fn the_awakening() -> CryptexResult<Self> {
        tracing::info!("The Assessor is awakening...");

        Ok(Self {
            score_cache: Arc::new(RwLock::new(HashMap::new())),
            enable_ai_enhancement: true,
        })
    }

    /// Assess a vulnerability by CVE ID
    ///
    /// Traditional name: `assess` or `score_vulnerability`
    ///
    /// Returns a comprehensive vulnerability score with CVSS, KEV, EPSS, and AI-enhanced data.
    pub async fn assess_vulnerability(&self, cve_id: &str) -> CryptexResult<VulnerabilityScore> {
        tracing::debug!("Assessing vulnerability: {}", cve_id);

        // Check cache first
        {
            let cache = self.score_cache.read().await;
            if let Some(score) = cache.get(cve_id) {
                tracing::debug!("Cache hit for: {}", cve_id);
                return Ok(score.clone());
            }
        }

        // Create new score
        let mut score = VulnerabilityScore::new(cve_id.to_string());

        // Fetch from NVD (stub - would make real API call)
        self.fetch_nvd_data(&mut score).await?;

        // Fetch KEV data (stub)
        self.fetch_kev_data(&mut score).await?;

        // Fetch EPSS data (stub)
        self.fetch_epss_data(&mut score).await?;

        // AI enhancement
        if self.enable_ai_enhancement {
            self.ai_enhance_score(&mut score).await?;
        }

        // Cache the score
        {
            let mut cache = self.score_cache.write().await;
            cache.insert(cve_id.to_string(), score.clone());
        }

        tracing::info!(
            "Vulnerability {} assessed - CVSS: {:.1}, Severity: {}",
            cve_id,
            score.cvss_base_score(),
            score.severity().as_str()
        );

        Ok(score)
    }

    /// Assess multiple vulnerabilities
    ///
    /// Traditional name: `batch_assess`
    pub async fn assess_multiple(&self, cve_ids: &[String]) -> CryptexResult<Vec<VulnerabilityScore>> {
        let mut scores = Vec::new();

        for cve_id in cve_ids {
            match self.assess_vulnerability(cve_id).await {
                Ok(score) => scores.push(score),
                Err(e) => {
                    tracing::warn!("Failed to assess {}: {}", cve_id, e);
                }
            }
        }

        Ok(scores)
    }

    /// Parse and score a CVSS vector string
    ///
    /// Traditional name: `score_from_vector`
    pub async fn score_from_cvss_vector(&self, cve_id: &str, vector: &str) -> CryptexResult<VulnerabilityScore> {
        let cvss = cvss_v3_from_vector(vector)
            .map_err(|e| the_foundation::CryptexError::validation(e))?;

        let mut score = VulnerabilityScore::new(cve_id.to_string());
        score.cvss_v3 = Some(cvss);

        Ok(score)
    }

    /// Filter vulnerabilities by severity
    pub fn filter_by_severity(
        scores: &[VulnerabilityScore],
        min_severity: CvssSeverity,
    ) -> Vec<VulnerabilityScore> {
        scores
            .iter()
            .filter(|s| s.severity() >= min_severity)
            .cloned()
            .collect()
    }

    /// Filter KEV vulnerabilities
    pub fn filter_kev_only(scores: &[VulnerabilityScore]) -> Vec<VulnerabilityScore> {
        scores
            .iter()
            .filter(|s| s.is_kev())
            .cloned()
            .collect()
    }

    /// Sort vulnerabilities by composite risk score
    pub fn sort_by_risk(mut scores: Vec<VulnerabilityScore>) -> Vec<VulnerabilityScore> {
        scores.sort_by(|a, b| {
            b.composite_risk_score()
                .partial_cmp(&a.composite_risk_score())
                .unwrap_or(std::cmp::Ordering::Equal)
        });
        scores
    }

    /// Get top N vulnerabilities by risk
    pub fn get_top_vulnerabilities(
        scores: Vec<VulnerabilityScore>,
        count: usize,
    ) -> Vec<VulnerabilityScore> {
        let sorted = Self::sort_by_risk(scores);
        sorted.into_iter().take(count).collect()
    }

    // Private methods

    /// Fetch NVD data (stub - would make real API call)
    async fn fetch_nvd_data(&self, score: &mut VulnerabilityScore) -> CryptexResult<()> {
        // In a real implementation, this would call the NVD API
        // For now, provide example data for testing

        if score.cve_id.starts_with("CVE-2021-44228") {
            // Log4Shell example
            score.vulnerability_name = Some("Apache Log4j2 Remote Code Execution".to_string());
            score.description = Some(
                "Apache Log4j2 2.0-beta9 through 2.15.0 JNDI features used in configuration..."
                    .to_string(),
            );
            score.cvss_v3 = Some(cvss_v3_from_vector(
                "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H",
            ).unwrap());
            score.cwe_ids = vec!["CWE-502".to_string()];
        }

        Ok(())
    }

    /// Fetch KEV data (stub)
    async fn fetch_kev_data(&self, score: &mut VulnerabilityScore) -> CryptexResult<()> {
        // In real implementation, fetch from CISA KEV catalog
        // For now, mark Log4Shell as KEV
        if score.cve_id.starts_with("CVE-2021-44228") {
            score.kev = Some(KevInfo {
                is_kev: true,
                date_added: Some("2021-12-10".to_string()),
                due_date: Some("2021-12-24".to_string()),
                required_action: Some("Apply updates per vendor instructions".to_string()),
                known_ransomware_use: true,
            });
        }

        Ok(())
    }

    /// Fetch EPSS data (stub)
    async fn fetch_epss_data(&self, score: &mut VulnerabilityScore) -> CryptexResult<()> {
        // In real implementation, fetch from FIRST EPSS API
        // For now, provide example data
        if score.cve_id.starts_with("CVE-2021-44228") {
            score.epss = Some(EpssInfo {
                score: 0.975,
                percentile: 0.999,
                date: "2024-01-15".to_string(),
            });
        }

        Ok(())
    }

    /// AI-enhance the vulnerability score
    async fn ai_enhance_score(&self, score: &mut VulnerabilityScore) -> CryptexResult<()> {
        // In real implementation, this would call The Collective for AI analysis
        // For now, use heuristic scoring

        let base_score = score.cvss_base_score();
        let is_kev = score.is_kev();
        let epss_score = score.epss.as_ref().map(|e| e.score).unwrap_or(0.0);

        // Calculate AI risk score (0-10 scale)
        let mut ai_risk = base_score;

        // Boost for KEV
        if is_kev {
            ai_risk = (ai_risk * 1.3).min(10.0);
        }

        // Boost for high EPSS
        if epss_score > 0.5 {
            ai_risk = (ai_risk + (epss_score * 2.0)).min(10.0);
        }

        score.ai_risk_score = Some(ai_risk);

        // Determine priority
        score.ai_priority = Some(match ai_risk {
            r if r >= 9.0 => "IMMEDIATE".to_string(),
            r if r >= 7.0 => "HIGH".to_string(),
            r if r >= 4.0 => "MEDIUM".to_string(),
            _ => "LOW".to_string(),
        });

        // Remediation urgency
        score.ai_remediation_urgency = Some(if is_kev {
            "Patch within 24 hours".to_string()
        } else if ai_risk >= 9.0 {
            "Patch within 7 days".to_string()
        } else if ai_risk >= 7.0 {
            "Patch within 30 days".to_string()
        } else {
            "Patch as part of regular cycle".to_string()
        });

        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn test_assessor_awakening() {
        let assessor = TheAssessor::the_awakening().await.unwrap();
        assert!(assessor.enable_ai_enhancement);
    }

    #[tokio::test]
    async fn test_assess_vulnerability() {
        let assessor = TheAssessor::the_awakening().await.unwrap();
        let score = assessor
            .assess_vulnerability("CVE-2021-44228")
            .await
            .unwrap();

        assert_eq!(score.cve_id, "CVE-2021-44228");
        assert!(score.cvss_base_score() > 0.0);
        assert!(score.is_kev());
    }

    #[tokio::test]
    async fn test_score_from_vector() {
        let assessor = TheAssessor::the_awakening().await.unwrap();
        let score = assessor
            .score_from_cvss_vector(
                "CVE-2024-0001",
                "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H",
            )
            .await
            .unwrap();

        assert_eq!(score.cvss_base_score(), 10.0);
        assert_eq!(score.severity(), CvssSeverity::Critical);
    }

    #[tokio::test]
    async fn test_filter_by_severity() {
        let scores = vec![
            VulnerabilityScore {
                cve_id: "CVE-1".to_string(),
                cvss_v3: Some(CvssV3 {
                    base_metrics: CvssV3Base {
                        attack_vector: "N".to_string(),
                        attack_complexity: "L".to_string(),
                        privileges_required: "N".to_string(),
                        user_interaction: "N".to_string(),
                        scope: "U".to_string(),
                        confidentiality: "H".to_string(),
                        integrity: "H".to_string(),
                        availability: "H".to_string(),
                    },
                    base_score: 9.8,
                    temporal_score: None,
                    environmental_score: None,
                    severity: CvssSeverity::Critical,
                    vector_string: String::new(),
                }),
                ..VulnerabilityScore::new("CVE-1".to_string())
            },
            VulnerabilityScore {
                cve_id: "CVE-2".to_string(),
                cvss_v3: Some(CvssV3 {
                    base_metrics: CvssV3Base {
                        attack_vector: "L".to_string(),
                        attack_complexity: "L".to_string(),
                        privileges_required: "L".to_string(),
                        user_interaction: "N".to_string(),
                        scope: "U".to_string(),
                        confidentiality: "L".to_string(),
                        integrity: "L".to_string(),
                        availability: "L".to_string(),
                    },
                    base_score: 5.3,
                    temporal_score: None,
                    environmental_score: None,
                    severity: CvssSeverity::Medium,
                    vector_string: String::new(),
                }),
                ..VulnerabilityScore::new("CVE-2".to_string())
            },
        ];

        let filtered = TheAssessor::filter_by_severity(&scores, CvssSeverity::High);
        assert_eq!(filtered.len(), 1);
        assert_eq!(filtered[0].cve_id, "CVE-1");
    }

    #[tokio::test]
    async fn test_sort_by_risk() {
        let mut score1 = VulnerabilityScore::new("CVE-LOW".to_string());
        score1.ai_risk_score = Some(3.0);

        let mut score2 = VulnerabilityScore::new("CVE-HIGH".to_string());
        score2.ai_risk_score = Some(9.0);

        let scores = vec![score1, score2];
        let sorted = TheAssessor::sort_by_risk(scores);

        assert_eq!(sorted[0].cve_id, "CVE-HIGH");
        assert_eq!(sorted[1].cve_id, "CVE-LOW");
    }
}
