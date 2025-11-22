// Platform-agnostic utilities for CRYPTEX
// Handles path resolution, platform detection, and system-specific operations

use std::path::{Path, PathBuf};
use std::env;

/// Get the platform-specific data directory for CRYPTEX
/// - Windows: %APPDATA%\CRYPTEX
/// - macOS: ~/Library/Application Support/CRYPTEX
/// - Linux: ~/.local/share/CRYPTEX or $XDG_DATA_HOME/CRYPTEX
pub fn get_data_dir() -> PathBuf {
    if let Ok(custom_path) = env::var("CRYPTEX_DATA_DIR") {
        return PathBuf::from(custom_path);
    }

    let base_dir = match env::consts::OS {
        "windows" => {
            env::var("APPDATA")
                .map(PathBuf::from)
                .unwrap_or_else(|_| {
                    let home = env::var("USERPROFILE").unwrap_or_else(|_| ".".to_string());
                    PathBuf::from(home).join("AppData").join("Roaming")
                })
        }
        "macos" => {
            env::var("HOME")
                .map(PathBuf::from)
                .unwrap_or_else(|_| PathBuf::from("."))
                .join("Library")
                .join("Application Support")
        }
        _ => {
            // Linux and other Unix-like systems
            env::var("XDG_DATA_HOME")
                .map(PathBuf::from)
                .unwrap_or_else(|_| {
                    let home = env::var("HOME").unwrap_or_else(|_| ".".to_string());
                    PathBuf::from(home).join(".local").join("share")
                })
        }
    };

    base_dir.join("CRYPTEX")
}

/// Get the default database path for CRYPTEX
pub fn get_default_db_path() -> PathBuf {
    if let Ok(db_path) = env::var("CRYPTEX_DB_PATH") {
        return PathBuf::from(db_path);
    }

    get_data_dir().join("cryptex.db")
}

/// Get the platform-specific config directory
pub fn get_config_dir() -> PathBuf {
    if let Ok(custom_path) = env::var("CRYPTEX_CONFIG_DIR") {
        return PathBuf::from(custom_path);
    }

    let base_dir = match env::consts::OS {
        "windows" => {
            env::var("APPDATA")
                .map(PathBuf::from)
                .unwrap_or_else(|_| {
                    let home = env::var("USERPROFILE").unwrap_or_else(|_| ".".to_string());
                    PathBuf::from(home).join("AppData").join("Roaming")
                })
        }
        "macos" => {
            env::var("HOME")
                .map(PathBuf::from)
                .unwrap_or_else(|_| PathBuf::from("."))
                .join("Library")
                .join("Preferences")
        }
        _ => {
            // Linux and other Unix-like systems
            env::var("XDG_CONFIG_HOME")
                .map(PathBuf::from)
                .unwrap_or_else(|_| {
                    let home = env::var("HOME").unwrap_or_else(|_| ".".to_string());
                    PathBuf::from(home).join(".config")
                })
        }
    };

    base_dir.join("CRYPTEX")
}

/// Get the platform-specific cache directory
pub fn get_cache_dir() -> PathBuf {
    if let Ok(custom_path) = env::var("CRYPTEX_CACHE_DIR") {
        return PathBuf::from(custom_path);
    }

    let base_dir = match env::consts::OS {
        "windows" => {
            env::var("LOCALAPPDATA")
                .map(PathBuf::from)
                .unwrap_or_else(|_| {
                    let home = env::var("USERPROFILE").unwrap_or_else(|_| ".".to_string());
                    PathBuf::from(home).join("AppData").join("Local")
                })
        }
        "macos" => {
            env::var("HOME")
                .map(PathBuf::from)
                .unwrap_or_else(|_| PathBuf::from("."))
                .join("Library")
                .join("Caches")
        }
        _ => {
            // Linux and other Unix-like systems
            env::var("XDG_CACHE_HOME")
                .map(PathBuf::from)
                .unwrap_or_else(|_| {
                    let home = env::var("HOME").unwrap_or_else(|_| ".".to_string());
                    PathBuf::from(home).join(".cache")
                })
        }
    };

    base_dir.join("CRYPTEX")
}

/// Ensure a directory exists, creating it if necessary
pub fn ensure_dir_exists(path: &Path) -> std::io::Result<()> {
    if !path.exists() {
        std::fs::create_dir_all(path)?;
    }
    Ok(())
}

/// Get the current platform identifier
pub fn get_platform() -> &'static str {
    env::consts::OS
}

/// Get the platform architecture
pub fn get_arch() -> &'static str {
    env::consts::ARCH
}

/// Check if running on Windows
pub fn is_windows() -> bool {
    env::consts::OS == "windows"
}

/// Check if running on macOS
pub fn is_macos() -> bool {
    env::consts::OS == "macos"
}

/// Check if running on Linux
pub fn is_linux() -> bool {
    env::consts::OS == "linux"
}

/// Get the executable file extension for the current platform
pub fn get_exe_extension() -> &'static str {
    if is_windows() {
        ".exe"
    } else {
        ""
    }
}

/// Resolve a path relative to the executable location
pub fn resolve_relative_to_exe(relative_path: &str) -> Option<PathBuf> {
    env::current_exe()
        .ok()
        .and_then(|exe| exe.parent().map(|p| p.to_path_buf()))
        .map(|exe_dir| exe_dir.join(relative_path))
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_get_platform() {
        let platform = get_platform();
        assert!(!platform.is_empty());
        println!("Platform: {}", platform);
    }

    #[test]
    fn test_get_data_dir() {
        let data_dir = get_data_dir();
        assert!(data_dir.to_str().unwrap().contains("CRYPTEX"));
        println!("Data dir: {:?}", data_dir);
    }

    #[test]
    fn test_get_default_db_path() {
        let db_path = get_default_db_path();
        assert!(db_path.to_str().unwrap().ends_with("cryptex.db"));
        println!("DB path: {:?}", db_path);
    }

    #[test]
    fn test_platform_detection() {
        println!("Platform: {}", get_platform());
        println!("Architecture: {}", get_arch());
        println!("Windows: {}", is_windows());
        println!("macOS: {}", is_macos());
        println!("Linux: {}", is_linux());
    }
}
