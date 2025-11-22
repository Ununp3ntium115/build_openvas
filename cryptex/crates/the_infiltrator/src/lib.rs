//! # The Infiltrator - Scanner Integration Bridge
//!
//! The Infiltrator provides seamless integration between OpenVAS scanner core
//! and CRYPTEX's AI-enhanced vulnerability assessment capabilities.
//!
//! Traditional name: `ScannerBridge` or `ScanIntegration`
//!
//! ## Features
//!
//! - **Real-time Enhancement**: Enriches scan results with CVSS, KEV, EPSS data
//! - **AI Integration**: Provides contextual remediation guidance
//! - **Scan Lifecycle Management**: Tracks active scans and their context
//! - **Performance Monitoring**: Comprehensive statistics and metrics
//! - **Host Context Tracking**: Maintains per-host vulnerability profiles
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_infiltrator::{TheInfiltrator, ScanResult};
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     let infiltrator = TheInfiltrator::the_awakening().await?;
//!
//!     // Start a new scan
//!     let scan_id = infiltrator.start_scan("192.168.1.0/24").await?;
//!
//!     // Report vulnerability detection
//!     infiltrator.on_vulnerability_detected(
//!         &scan_id,
//!         "CVE-2024-12345",
//!         "192.168.1.100",
//!         443,
//!     ).await?;
//!
//!     // End scan and generate report
//!     let report = infiltrator.end_scan(&scan_id).await?;
//!
//!     println!("Scan complete: {} vulnerabilities found",
//!              report.total_vulnerabilities);
//!
//!     Ok(())
//! }
//! ```

pub mod types;
pub mod scanner;
pub mod context;

pub use types::*;
pub use scanner::TheInfiltrator;
pub use context::*;

// Re-export from foundation
pub use the_foundation::CryptexResult;
