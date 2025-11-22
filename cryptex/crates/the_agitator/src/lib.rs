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
//! use the_foundation::{ThePackage, AgitatorType};
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

// Re-export from the_foundation
pub use the_foundation::{
    AgitatorType, CryptexError, CryptexResult, TheAgitator, ThePackage, TheReply,
};

// Module declarations for specific providers
// These will be implemented in Phase 1, Week 5
pub mod openai;
pub mod claude;

// Re-exports
pub use openai::OpenAIAgitator;
pub use claude::ClaudeAgitator;
