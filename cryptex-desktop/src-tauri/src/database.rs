// Database utilities for desktop app

use the_foundation::get_default_db_path;
use std::path::PathBuf;

pub fn get_db_path() -> PathBuf {
    get_default_db_path()
}

pub fn ensure_db_exists() -> Result<(), Box<dyn std::error::Error>> {
    let db_path = get_db_path();

    if let Some(parent) = db_path.parent() {
        std::fs::create_dir_all(parent)?;
    }

    Ok(())
}
