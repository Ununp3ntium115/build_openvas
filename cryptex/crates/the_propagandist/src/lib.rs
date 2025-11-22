//! # The Propagandist - Report Generation
//!
//! The Propagandist generates comprehensive vulnerability assessment reports
//! in multiple formats, designed for different audiences and use cases.
//!
//! Traditional name: `ReportGenerator` or `ReportingService`
//!
//! ## Features
//!
//! - **Multi-Format Support**: JSON, HTML, Markdown, Executive Summary
//! - **Audience-Specific**: Technical, executive, compliance-focused reports
//! - **Risk Prioritization**: Automatic vulnerability ranking
//! - **Actionable Insights**: Clear remediation recommendations
//! - **Compliance Mapping**: NIST, CIS, OWASP framework alignment
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_propagandist::{ThePropagandist, ReportFormat};
//! use the_infiltrator::ScanReport;
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     let propagandist = ThePropagandist::the_awakening().await?;
//!
//!     // Assume we have a scan report
//!     # let scan_report = ScanReport::new("scan-001".to_string(), "192.168.1.0/24".to_string());
//!
//!     // Generate JSON report
//!     let json = propagandist.generate_report(
//!         &scan_report,
//!         ReportFormat::Json
//!     ).await?;
//!
//!     // Generate executive summary
//!     let summary = propagandist.generate_executive_summary(
//!         &scan_report
//!     ).await?;
//!
//!     println!("{}", summary);
//!
//!     Ok(())
//! }
//! ```

pub mod formats;
pub mod generator;

pub use formats::*;
pub use generator::ThePropagandist;

// Re-export from foundation
pub use the_foundation::CryptexResult;
