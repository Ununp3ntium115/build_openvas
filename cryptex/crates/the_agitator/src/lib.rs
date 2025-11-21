//! # The Agitator - AI Provider Integrations
//!
//! The Agitator provides a unified interface to multiple AI providers,
//! allowing CRYPTEX to leverage different AI models for security analysis.
//!
//! Traditional name: `AIProvider` or `ModelProvider`
//!
//! ## Supported Providers
//!
//! - **OpenAI** - GPT-4 and other OpenAI models
//! - **Claude** - Anthropic's Claude models
//! - More to come...
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_agitator::{TheAgitator, OpenAIAgitator};
//! use the_collective::{ThePackage, AgitatorType};
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     // Initialize OpenAI agitator
//!     let agitator = OpenAIAgitator::the_awakening(
//!         std::env::var("OPENAI_API_KEY")?,
//!         "gpt-4".to_string(),
//!     ).await?;
//!
//!     // Create a package
//!     let package = ThePackage {
//!         data: serde_json::json!({
//!             "cve": "CVE-2023-12345",
//!             "description": "Critical SQL injection vulnerability"
//!         }),
//!         context: Some("Analyze this vulnerability".to_string()),
//!     };
//!
//!     // Process through agitator
//!     let reply = agitator.the_agitation(package).await?;
//!
//!     println!("AI Response: {}", reply.content);
//!
//!     Ok(())
//! }
//! ```

use async_trait::async_trait;
use the_collective::{AgitatorType, CryptexResult, ThePackage, TheReply};

/// The Agitator Trait - Common interface for all AI providers
///
/// All AI provider implementations must implement this trait to ensure
/// consistent behavior across different providers.
#[async_trait]
pub trait TheAgitator: Send + Sync {
    /// Get the type of this agitator
    fn agitator_type(&self) -> AgitatorType;

    /// Get the mind (model) name
    fn the_mind(&self) -> &str;

    /// The Agitation - Process a package and return a reply
    ///
    /// Traditional name: `process` or `send_request`
    async fn the_agitation(&self, package: ThePackage) -> CryptexResult<TheReply>;

    /// The Validation - Health check
    ///
    /// Traditional name: `health_check`
    async fn the_validation(&self) -> CryptexResult<bool>;
}

// Module declarations for specific providers
// These will be implemented in Phase 1, Week 5
pub mod openai;
pub mod claude;

// Re-exports
pub use openai::OpenAIAgitator;
pub use claude::ClaudeAgitator;
