//! # The Assessor - Vulnerability Scoring System
//!
//! The Assessor provides comprehensive vulnerability scoring and risk assessment,
//! integrating multiple industry-standard frameworks with AI-enhanced analysis.
//!
//! Traditional name: `VulnerabilityScorer` or `RiskAssessor`
//!
//! ## Supported Frameworks
//!
//! - **CVSS v2.0** - Common Vulnerability Scoring System v2
//! - **CVSS v3.0/3.1** - Modern CVSS with improved granularity
//! - **CVSS v4.0** - Latest CVSS with enhanced threat metrics
//! - **KEV** - CISA Known Exploited Vulnerabilities catalog
//! - **EPSS** - Exploit Prediction Scoring System
//! - **SSVC** - Stakeholder-Specific Vulnerability Categorization
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_assessor::{TheAssessor, VulnerabilityScore};
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     let assessor = TheAssessor::the_awakening().await?;
//!
//!     // Get comprehensive score for a CVE
//!     let score = assessor
//!         .assess_vulnerability("CVE-2024-12345")
//!         .await?;
//!
//!     println!("CVSS Base Score: {}", score.cvss_base_score());
//!     println!("AI Risk Score: {}", score.ai_risk_score.unwrap_or(0.0));
//!     println!("Severity: {:?}", score.severity());
//!
//!     Ok(())
//! }
//! ```

pub mod types;
pub mod cvss;
pub mod scoring;

pub use types::*;
pub use cvss::*;
pub use scoring::TheAssessor;

// Re-export from foundation
pub use the_foundation::CryptexResult;
