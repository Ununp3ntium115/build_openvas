//! # The Coordinator - Configuration Management
//!
//! The Coordinator manages the charter (configuration) for the entire CRYPTEX system.
//! It handles loading configuration from files, environment variables, and provides
//! a unified configuration interface.
//!
//! Traditional name: `ConfigManager` or `ConfigurationService`
//!
//! ## Philosophy
//!
//! Configuration is consensus - not control. The Charter represents agreed-upon
//! rules of engagement, not imposed restrictions.

use config::{Config, ConfigError, Environment, File};
use serde::{Deserialize, Serialize};
use std::path::PathBuf;

/// The Charter - Complete configuration for CRYPTEX
///
/// Traditional name: `Configuration` or `Config`
///
/// Represents the collective agreement on how the system should operate.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct TheCharter {
    /// Commune (server) configuration
    pub commune: CommuneConfig,

    /// Agitators (AI providers) configuration
    #[serde(default)]
    pub agitators: Vec<AgitatorConfig>,

    /// Archive (database) configuration
    pub archive: ArchiveConfig,

    /// Memory (cache) configuration
    pub memory: MemoryConfig,

    /// Infiltrator (scanner) configuration
    #[serde(default)]
    pub infiltrator: Option<InfiltratorConfig>,

    /// Cache threshold for storing AI replies
    #[serde(default = "default_cache_threshold")]
    pub cache_threshold: f64,
}

fn default_cache_threshold() -> f64 {
    0.8
}

/// Commune Configuration
///
/// Traditional name: `ServerConfig`
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CommuneConfig {
    /// Name of this commune/node
    #[serde(default = "default_commune_name")]
    pub name: String,

    /// HTTP server host
    #[serde(default = "default_host")]
    pub host: String,

    /// HTTP server port
    #[serde(default = "default_port")]
    pub port: u16,

    /// WebSocket port
    #[serde(default = "default_ws_port")]
    pub ws_port: u16,
}

fn default_commune_name() -> String {
    "CRYPTEX Liberation Node".to_string()
}

fn default_host() -> String {
    "127.0.0.1".to_string()
}

fn default_port() -> u16 {
    8080
}

fn default_ws_port() -> u16 {
    9090
}

/// Agitator Configuration
///
/// Traditional name: `AIProviderConfig`
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct AgitatorConfig {
    /// Name of this agitator
    pub name: String,

    /// Type of agitator (openai, claude, etc.)
    pub agitator_type: String,

    /// Whether this agitator is enabled
    #[serde(default = "default_true")]
    pub enabled: bool,

    /// API key (can be set via environment variable)
    #[serde(skip_serializing_if = "Option::is_none")]
    pub api_key: Option<String>,

    /// Environment variable name for API key
    #[serde(skip_serializing_if = "Option::is_none")]
    pub api_key_env: Option<String>,

    /// Model/mind to use
    #[serde(skip_serializing_if = "Option::is_none")]
    pub model: Option<String>,

    /// Request timeout in seconds
    #[serde(default = "default_timeout")]
    pub timeout_seconds: u64,
}

fn default_true() -> bool {
    true
}

fn default_timeout() -> u64 {
    30
}

/// Archive Configuration
///
/// Traditional name: `DatabaseConfig`
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ArchiveConfig {
    /// Path to the redb database file
    #[serde(default = "default_archive_path")]
    pub path: PathBuf,

    /// Maximum database size in GB
    #[serde(default = "default_max_size_gb")]
    pub max_size_gb: u64,
}

fn default_archive_path() -> PathBuf {
    PathBuf::from("./cryptex-archive.redb")
}

fn default_max_size_gb() -> u64 {
    10
}

/// Memory Configuration
///
/// Traditional name: `CacheConfig`
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct MemoryConfig {
    /// Maximum cache size in MB
    #[serde(default = "default_cache_size_mb")]
    pub max_size_mb: usize,

    /// Time-to-live for cached items in seconds
    #[serde(default = "default_ttl_seconds")]
    pub ttl_seconds: u64,
}

fn default_cache_size_mb() -> usize {
    256
}

fn default_ttl_seconds() -> u64 {
    3600
}

/// Infiltrator Configuration
///
/// Traditional name: `ScannerConfig`
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct InfiltratorConfig {
    /// Path to scanner executable
    pub scanner_path: PathBuf,

    /// Maximum concurrent scans
    #[serde(default = "default_max_concurrent_scans")]
    pub max_concurrent_scans: usize,
}

fn default_max_concurrent_scans() -> usize {
    3
}

impl TheCharter {
    /// The Charter Loading - Load configuration from file and environment
    ///
    /// Traditional name: `load_config` or `load_configuration`
    ///
    /// Loads configuration with the following precedence (highest to lowest):
    /// 1. Environment variables (CRYPTEX_*)
    /// 2. User-specified config file
    /// 3. Default config locations
    /// 4. Built-in defaults
    pub fn the_charter_loading(config_path: Option<PathBuf>) -> Result<Self, ConfigError> {
        let mut builder = Config::builder();

        // Start with defaults
        builder = builder.add_source(File::from_str(DEFAULT_CONFIG, config::FileFormat::Toml));

        // Try user-specified path or default paths
        if let Some(path) = config_path {
            builder = builder.add_source(File::from(path));
        } else {
            // Try default paths in order
            for path in DEFAULT_CONFIG_PATHS {
                let p = PathBuf::from(path);
                if p.exists() {
                    builder = builder.add_source(File::from(p));
                    break;
                }
            }
        }

        // Environment variables have highest precedence
        builder = builder.add_source(
            Environment::with_prefix("CRYPTEX")
                .separator("__")
                .try_parsing(true),
        );

        // Build and deserialize
        let config = builder.build()?;
        let mut charter: TheCharter = config.try_deserialize()?;

        // Load API keys from environment if specified
        for agitator in &mut charter.agitators {
            if agitator.api_key.is_none() {
                if let Some(ref env_var) = agitator.api_key_env {
                    if let Ok(key) = std::env::var(env_var) {
                        agitator.api_key = Some(key);
                    }
                }
            }
        }

        Ok(charter)
    }

    /// Validate the charter
    pub fn validate(&self) -> Result<(), String> {
        // Note: Empty agitators list is allowed - AI features will be disabled
        // but other functionality (like scanning) may still work

        // If agitators are configured, validate at least one is enabled
        if !self.agitators.is_empty() && !self.agitators.iter().any(|a| a.enabled) {
            return Err("Agitators are configured but none are enabled".to_string());
        }

        // Validate enabled agitators have API keys configured
        for agitator in &self.agitators {
            if agitator.enabled && agitator.api_key.is_none() && agitator.api_key_env.is_none() {
                return Err(format!(
                    "Agitator '{}' is enabled but has no API key or API key environment variable configured",
                    agitator.name
                ));
            }
        }

        Ok(())
    }
}

/// Default configuration paths to try
const DEFAULT_CONFIG_PATHS: &[&str] = &[
    "./cryptex-config.toml",
    "~/.config/cryptex/config.toml",
    "/etc/cryptex/config.toml",
];

/// Default built-in configuration
const DEFAULT_CONFIG: &str = r#"
[commune]
name = "CRYPTEX Liberation Node"
host = "127.0.0.1"
port = 8080
ws_port = 9090

[archive]
path = "./cryptex-archive.redb"
max_size_gb = 10

[memory]
max_size_mb = 256
ttl_seconds = 3600

cache_threshold = 0.8
"#;

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_charter_loading_defaults() {
        let charter = TheCharter::the_charter_loading(None);
        // May fail if config file exists, which is fine
        assert!(charter.is_ok() || charter.is_err());
    }

    #[test]
    fn test_commune_config_defaults() {
        let commune = CommuneConfig {
            name: default_commune_name(),
            host: default_host(),
            port: default_port(),
            ws_port: default_ws_port(),
        };

        assert_eq!(commune.port, 8080);
        assert_eq!(commune.ws_port, 9090);
        assert_eq!(commune.host, "127.0.0.1");
    }
}
