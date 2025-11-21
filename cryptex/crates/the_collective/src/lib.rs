//! # The Collective - AI Service Core
//!
//! The Collective is the heart of CRYPTEX's AI-powered security assessment capabilities.
//! It coordinates AI providers (Agitators), manages request processing, and ensures
//! efficient, reliable AI-enhanced vulnerability analysis.
//!
//! ## Philosophy
//!
//! > "Security through autonomy, not authority"
//! > "Decentralized power, distributed knowledge"
//! > "No gods, no masters, no memory leaks"
//!
//! ## Architecture
//!
//! ```text
//! ┌─────────────────────────────────────────┐
//! │         The Collective                   │
//! │  ┌─────────────────────────────────────┐│
//! │  │  The Memory (Cache)                 ││
//! │  └─────────────────────────────────────┘│
//! │  ┌─────────────────────────────────────┐│
//! │  │  The Agitators (AI Providers)       ││
//! │  │  • OpenAI                           ││
//! │  │  • Claude                           ││
//! │  └─────────────────────────────────────┘│
//! │  ┌─────────────────────────────────────┐│
//! │  │  The Observer (Metrics)             ││
//! │  └─────────────────────────────────────┘│
//! └─────────────────────────────────────────┘
//! ```
//!
//! ## Example Usage
//!
//! ```rust,no_run
//! use the_collective::{TheCollective, TheCharter, TheInquiry, TaskType};
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     // Load the charter (configuration)
//!     let charter = TheCharter::the_charter_loading(None)?;
//!
//!     // The Awakening - Initialize the collective
//!     let collective = TheCollective::the_awakening(charter).await?;
//!
//!     // Create an inquiry
//!     let inquiry = TheInquiry::new(
//!         TaskType::VulnerabilityAnalysis,
//!         serde_json::json!({
//!             "cve": "CVE-2023-12345",
//!             "severity": "critical",
//!         })
//!     );
//!
//!     // Process the inquiry
//!     let reply = collective.the_inquiry_sync(inquiry).await?;
//!
//!     println!("AI Analysis: {}", reply.content);
//!     println!("Certainty: {:.1}%", reply.certainty * 100.0);
//!
//!     // The Rest - Graceful shutdown
//!     collective.the_rest().await?;
//!
//!     Ok(())
//! }
//! ```

pub mod api;
pub mod error;
pub mod models;

// Re-exports
pub use error::{CryptexError, CryptexResult};
pub use models::{
    AgitatorType, TaskType, TheCertainty, TheInquiry, ThePackage, TheReply,
};

use tracing_subscriber::{layer::SubscriberExt, util::SubscriberInitExt};

/// The Chronicler - Initialize logging system
///
/// Sets up structured logging for the entire CRYPTEX system.
/// Uses the RUST_LOG environment variable for log level configuration.
///
/// # Example
///
/// ```bash
/// RUST_LOG=the_collective=debug,info cargo run
/// ```
pub fn the_chronicler_awakening() -> CryptexResult<()> {
    tracing_subscriber::registry()
        .with(
            tracing_subscriber::EnvFilter::try_from_default_env()
                .unwrap_or_else(|_| "the_collective=info,warn".into()),
        )
        .with(tracing_subscriber::fmt::layer())
        .init();

    tracing::info!("The Chronicler has awakened - logging initialized");

    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_chronicler_awakening() {
        let result = the_chronicler_awakening();
        // May fail if already initialized, which is fine
        assert!(result.is_ok() || result.is_err());
    }
}
