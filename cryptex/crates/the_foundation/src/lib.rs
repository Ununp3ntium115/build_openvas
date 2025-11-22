//! # The Foundation - Core Types and Traits for CRYPTEX
//!
//! The Foundation provides the shared types, traits, and error handling
//! used across all CRYPTEX crates, breaking circular dependencies.

pub mod error;
pub mod models;
pub mod traits;
pub mod platform;

// Re-exports
pub use error::{CryptexError, CryptexResult};
pub use models::{
    AgitatorType, TaskType, TheCertainty, TheInquiry, ThePackage, TheReply,
};
pub use traits::TheAgitator;
pub use platform::{
    get_data_dir, get_default_db_path, get_config_dir, get_cache_dir,
    ensure_dir_exists, get_platform, get_arch, is_windows, is_macos, is_linux,
};
