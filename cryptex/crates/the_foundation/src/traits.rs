//! Core traits for CRYPTEX components

use async_trait::async_trait;
use crate::{AgitatorType, CryptexResult, ThePackage, TheReply};

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
