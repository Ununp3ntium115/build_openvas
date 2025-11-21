//! # The Foundation - Core Types and Traits for CRYPTEX
//!
//! The Foundation provides the shared types, traits, and error handling
//! used across all CRYPTEX crates, breaking circular dependencies.

pub mod error;
pub mod models;
pub mod traits;

// Re-exports
pub use error::{CryptexError, CryptexResult};
pub use models::{
    AgitatorType, TaskType, TheCertainty, TheInquiry, ThePackage, TheReply,
};
pub use traits::TheAgitator;
