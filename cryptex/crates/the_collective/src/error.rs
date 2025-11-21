//! Error types for The Collective
//!
//! Comprehensive error handling using the anarchist naming philosophy.

use thiserror::Error;

/// CryptexError - All possible errors in the CRYPTEX system
///
/// Follows the principle of transparent error reporting - no hidden failures,
/// all errors are explicit and informative.
#[derive(Error, Debug)]
pub enum CryptexError {
    /// Configuration error - The Charter is invalid or incomplete
    #[error("Charter error: {0}")]
    CharterError(String),

    /// Agitator (AI Provider) error
    #[error("Agitator error: {0}")]
    AgitatorError(String),

    /// Network/HTTP error
    #[error("Network error: {0}")]
    NetworkError(#[from] reqwest::Error),

    /// JSON serialization/deserialization error
    #[error("Serialization error: {0}")]
    SerializationError(#[from] serde_json::Error),

    /// Archive (database) error
    #[error("Archive error: {0}")]
    ArchiveError(String),

    /// Invalid inquiry structure
    #[error("Invalid inquiry: {0}")]
    InvalidInquiry(String),

    /// Timeout - operation took too long
    #[error("Timeout: operation exceeded time limit")]
    Timeout,

    /// Authentication/authorization error
    #[error("Validation failed: {0}")]
    ValidationError(String),

    /// Resource not found
    #[error("Not found: {0}")]
    NotFound(String),

    /// IO error
    #[error("IO error: {0}")]
    IoError(#[from] std::io::Error),

    /// Generic error for unexpected situations
    #[error("Unknown error: {0}")]
    Unknown(String),
}

/// CryptexResult - Standard Result type for CRYPTEX operations
///
/// All fallible operations should return this type.
pub type CryptexResult<T> = Result<T, CryptexError>;

impl CryptexError {
    /// Create a Charter error
    pub fn charter<S: Into<String>>(msg: S) -> Self {
        CryptexError::CharterError(msg.into())
    }

    /// Create an Agitator error
    pub fn agitator<S: Into<String>>(msg: S) -> Self {
        CryptexError::AgitatorError(msg.into())
    }

    /// Create an Archive error
    pub fn archive<S: Into<String>>(msg: S) -> Self {
        CryptexError::ArchiveError(msg.into())
    }

    /// Create an Invalid Inquiry error
    pub fn invalid_inquiry<S: Into<String>>(msg: S) -> Self {
        CryptexError::InvalidInquiry(msg.into())
    }

    /// Create a Not Found error
    pub fn not_found<S: Into<String>>(msg: S) -> Self {
        CryptexError::NotFound(msg.into())
    }

    /// Create a Validation error
    pub fn validation<S: Into<String>>(msg: S) -> Self {
        CryptexError::ValidationError(msg.into())
    }

    /// Create an Unknown error
    pub fn unknown<S: Into<String>>(msg: S) -> Self {
        CryptexError::Unknown(msg.into())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_error_creation() {
        let err = CryptexError::charter("Invalid configuration");
        assert!(matches!(err, CryptexError::CharterError(_)));
        assert_eq!(err.to_string(), "Charter error: Invalid configuration");
    }

    #[test]
    fn test_agitator_error() {
        let err = CryptexError::agitator("OpenAI API key missing");
        assert!(matches!(err, CryptexError::AgitatorError(_)));
    }

    #[test]
    fn test_result_type() {
        fn returns_result() -> CryptexResult<String> {
            Ok("success".to_string())
        }

        assert!(returns_result().is_ok());
    }
}
