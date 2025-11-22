//! # The Archive - Embedded Database Layer
//!
//! The Archive provides persistent storage using redb, an embedded
//! ACID-compliant database written in pure Rust.
//!
//! Traditional name: `Database` or `Storage`
//!
//! ## Features
//!
//! - **Scan Results**: Store and retrieve vulnerability scan data
//! - **Vulnerability Assessments**: Cache CVE assessments and scores
//! - **Historical Tracking**: Query scans by date range
//! - **ACID Compliance**: Guaranteed data consistency
//! - **Thread-Safe**: Concurrent read/write support
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_archive::TheArchive;
//! use std::path::PathBuf;
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     // Initialize the archive
//!     let archive = TheArchive::the_awakening(PathBuf::from("./cryptex.db"))?;
//!
//!     // Store scan results
//!     // archive.store_scan_result(...).await?;
//!
//!     // Query historical data
//!     // let results = archive.get_scan_history(scan_id).await?;
//!
//!     Ok(())
//! }
//! ```

use chrono::{DateTime, Utc};
use redb::{Database, ReadableTable, ReadableTableMetadata, TableDefinition};
use serde::{Deserialize, Serialize};
use std::path::PathBuf;
use std::sync::Arc;

pub use the_foundation::{CryptexError, CryptexResult};
pub use the_assessor::VulnerabilityScore;
pub use the_infiltrator::ScanResult;

// Table definitions
const SCANS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("scans");
const VULNERABILITIES_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("vulnerabilities");
const SCAN_RESULTS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("scan_results");

/// Stored scan metadata
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ScanMetadata {
    pub scan_id: String,
    pub target: String,
    pub started_at: DateTime<Utc>,
    pub ended_at: Option<DateTime<Utc>>,
    pub status: String,
    pub total_vulnerabilities: usize,
    pub critical: usize,
    pub high: usize,
    pub medium: usize,
    pub low: usize,
}

impl ScanMetadata {
    pub fn new(scan_id: String, target: String) -> Self {
        Self {
            scan_id,
            target,
            started_at: Utc::now(),
            ended_at: None,
            status: "running".to_string(),
            total_vulnerabilities: 0,
            critical: 0,
            high: 0,
            medium: 0,
            low: 0,
        }
    }
}

/// Stored scan result with scan_id
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StoredScanResult {
    pub scan_id: String,
    pub result: ScanResult,
}

/// Stored vulnerability assessment
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct StoredVulnerability {
    pub cve_id: String,
    pub score: VulnerabilityScore,
    pub cached_at: DateTime<Utc>,
}

/// The Archive - Main database interface
pub struct TheArchive {
    db: Arc<Database>,
}

impl TheArchive {
    /// The Awakening - Initialize the archive
    pub fn the_awakening(path: PathBuf) -> CryptexResult<Self> {
        tracing::info!("The Archive awakening at {:?}", path);

        // Create parent directory if it doesn't exist
        if let Some(parent) = path.parent() {
            std::fs::create_dir_all(parent).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to create archive directory: {}", e))
            })?;
        }

        // Open or create the database
        let db = Database::create(&path).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to create database: {}", e))
        })?;

        // Initialize tables
        let write_txn = db.begin_write().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin write transaction: {}", e))
        })?;

        {
            let _ = write_txn.open_table(SCANS_TABLE).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to open scans table: {}", e))
            })?;

            let _ = write_txn.open_table(VULNERABILITIES_TABLE).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to open vulnerabilities table: {}", e))
            })?;

            let _ = write_txn.open_table(SCAN_RESULTS_TABLE).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to open scan_results table: {}", e))
            })?;
        }

        write_txn.commit().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to commit table creation: {}", e))
        })?;

        tracing::info!("The Archive ready");

        Ok(Self { db: Arc::new(db) })
    }

    /// Store scan metadata
    pub fn store_scan_metadata(&self, metadata: &ScanMetadata) -> CryptexResult<()> {
        let write_txn = self.db.begin_write().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin write transaction: {}", e))
        })?;

        {
            let mut table = write_txn.open_table(SCANS_TABLE).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to open scans table: {}", e))
            })?;

            let data = serde_cbor::to_vec(metadata).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to serialize scan metadata: {}", e))
            })?;

            table
                .insert(metadata.scan_id.as_str(), data.as_slice())
                .map_err(|e| {
                    CryptexError::ArchiveError(format!("Failed to insert scan metadata: {}", e))
                })?;
        }

        write_txn.commit().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to commit scan metadata: {}", e))
        })?;

        tracing::debug!("Stored scan metadata for {}", metadata.scan_id);
        Ok(())
    }

    /// Retrieve scan metadata
    pub fn get_scan_metadata(&self, scan_id: &str) -> CryptexResult<Option<ScanMetadata>> {
        let read_txn = self.db.begin_read().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin read transaction: {}", e))
        })?;

        let table = read_txn.open_table(SCANS_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open scans table: {}", e))
        })?;

        let result = table.get(scan_id).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to read scan metadata: {}", e))
        })?;

        match result {
            Some(data) => {
                let metadata: ScanMetadata = serde_cbor::from_slice(data.value()).map_err(
                    |e| {
                        CryptexError::ArchiveError(format!(
                            "Failed to deserialize scan metadata: {}",
                            e
                        ))
                    },
                )?;
                Ok(Some(metadata))
            }
            None => Ok(None),
        }
    }

    /// Store scan result
    pub fn store_scan_result(&self, scan_id: &str, result: &ScanResult) -> CryptexResult<()> {
        let write_txn = self.db.begin_write().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin write transaction: {}", e))
        })?;

        {
            let mut table = write_txn.open_table(SCAN_RESULTS_TABLE).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to open scan_results table: {}", e))
            })?;

            let stored = StoredScanResult {
                scan_id: scan_id.to_string(),
                result: result.clone(),
            };

            let data = serde_cbor::to_vec(&stored).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to serialize scan result: {}", e))
            })?;

            // Key format: scan_id:cve_id:host:port
            let key = format!(
                "{}:{}:{}:{}",
                scan_id, result.cve_id, result.host, result.port
            );

            table.insert(key.as_str(), data.as_slice()).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to insert scan result: {}", e))
            })?;
        }

        write_txn.commit().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to commit scan result: {}", e))
        })?;

        tracing::debug!("Stored scan result for {} in scan {}", result.cve_id, scan_id);
        Ok(())
    }

    /// Get all scan results for a scan
    pub fn get_scan_results(&self, scan_id: &str) -> CryptexResult<Vec<ScanResult>> {
        let read_txn = self.db.begin_read().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin read transaction: {}", e))
        })?;

        let table = read_txn.open_table(SCAN_RESULTS_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open scan_results table: {}", e))
        })?;

        let mut results = Vec::new();
        let prefix = format!("{}:", scan_id);

        // Iterate through all entries
        let iter = table.iter().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to iterate scan results: {}", e))
        })?;

        for entry in iter {
            let (key, value) = entry.map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to read scan result entry: {}", e))
            })?;

            let key_str = key.value();
            if key_str.starts_with(&prefix) {
                let stored: StoredScanResult = serde_cbor::from_slice(value.value()).map_err(|e| {
                    CryptexError::ArchiveError(format!(
                        "Failed to deserialize scan result: {}",
                        e
                    ))
                })?;
                results.push(stored.result);
            }
        }

        Ok(results)
    }

    /// Store vulnerability assessment
    pub fn store_vulnerability(&self, score: &VulnerabilityScore) -> CryptexResult<()> {
        let write_txn = self.db.begin_write().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin write transaction: {}", e))
        })?;

        {
            let mut table = write_txn.open_table(VULNERABILITIES_TABLE).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to open vulnerabilities table: {}", e))
            })?;

            let stored = StoredVulnerability {
                cve_id: score.cve_id.clone(),
                score: score.clone(),
                cached_at: Utc::now(),
            };

            let data = serde_cbor::to_vec(&stored).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to serialize vulnerability: {}", e))
            })?;

            table
                .insert(score.cve_id.as_str(), data.as_slice())
                .map_err(|e| {
                    CryptexError::ArchiveError(format!("Failed to insert vulnerability: {}", e))
                })?;
        }

        write_txn.commit().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to commit vulnerability: {}", e))
        })?;

        tracing::debug!("Stored vulnerability assessment for {}", score.cve_id);
        Ok(())
    }

    /// Retrieve vulnerability assessment
    pub fn get_vulnerability(&self, cve_id: &str) -> CryptexResult<Option<StoredVulnerability>> {
        let read_txn = self.db.begin_read().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin read transaction: {}", e))
        })?;

        let table = read_txn.open_table(VULNERABILITIES_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open vulnerabilities table: {}", e))
        })?;

        let result = table.get(cve_id).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to read vulnerability: {}", e))
        })?;

        match result {
            Some(data) => {
                let stored: StoredVulnerability = serde_cbor::from_slice(data.value()).map_err(
                    |e| {
                        CryptexError::ArchiveError(format!(
                            "Failed to deserialize vulnerability: {}",
                            e
                        ))
                    },
                )?;
                Ok(Some(stored))
            }
            None => Ok(None),
        }
    }

    /// List all scans
    pub fn list_scans(&self) -> CryptexResult<Vec<ScanMetadata>> {
        let read_txn = self.db.begin_read().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin read transaction: {}", e))
        })?;

        let table = read_txn.open_table(SCANS_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open scans table: {}", e))
        })?;

        let mut scans = Vec::new();

        let iter = table.iter().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to iterate scans: {}", e))
        })?;

        for entry in iter {
            let (_key, value) = entry.map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to read scan entry: {}", e))
            })?;

            let metadata: ScanMetadata = serde_cbor::from_slice(value.value()).map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to deserialize scan metadata: {}", e))
            })?;
            scans.push(metadata);
        }

        // Sort by started_at descending (most recent first)
        scans.sort_by(|a, b| b.started_at.cmp(&a.started_at));

        Ok(scans)
    }

    /// Get database statistics
    pub fn get_stats(&self) -> CryptexResult<ArchiveStats> {
        let read_txn = self.db.begin_read().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to begin read transaction: {}", e))
        })?;

        let scans_table = read_txn.open_table(SCANS_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open scans table: {}", e))
        })?;

        let vulns_table = read_txn.open_table(VULNERABILITIES_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open vulnerabilities table: {}", e))
        })?;

        let results_table = read_txn.open_table(SCAN_RESULTS_TABLE).map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to open scan_results table: {}", e))
        })?;

        let total_scans = scans_table.len().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to count scans: {}", e))
        })?;

        let total_vulnerabilities = vulns_table.len().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to count vulnerabilities: {}", e))
        })?;

        let total_results = results_table.len().map_err(|e| {
            CryptexError::ArchiveError(format!("Failed to count results: {}", e))
        })?;

        Ok(ArchiveStats {
            total_scans,
            total_vulnerabilities,
            total_results,
        })
    }
}

/// Archive statistics
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ArchiveStats {
    pub total_scans: u64,
    pub total_vulnerabilities: u64,
    pub total_results: u64,
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::TempDir;

    #[test]
    fn test_archive_awakening() {
        let temp_dir = TempDir::new().unwrap();
        let db_path = temp_dir.path().join("test.db");

        let archive = TheArchive::the_awakening(db_path).unwrap();
        let stats = archive.get_stats().unwrap();

        assert_eq!(stats.total_scans, 0);
        assert_eq!(stats.total_vulnerabilities, 0);
        assert_eq!(stats.total_results, 0);
    }

    #[test]
    fn test_store_and_retrieve_scan_metadata() {
        let temp_dir = TempDir::new().unwrap();
        let db_path = temp_dir.path().join("test.db");
        let archive = TheArchive::the_awakening(db_path).unwrap();

        let metadata = ScanMetadata::new("scan_123".to_string(), "192.168.1.0/24".to_string());

        // Store metadata
        archive.store_scan_metadata(&metadata).unwrap();

        // Retrieve metadata
        let retrieved = archive.get_scan_metadata("scan_123").unwrap();
        assert!(retrieved.is_some());

        let retrieved = retrieved.unwrap();
        assert_eq!(retrieved.scan_id, "scan_123");
        assert_eq!(retrieved.target, "192.168.1.0/24");
        assert_eq!(retrieved.status, "running");
    }

    #[test]
    fn test_store_and_retrieve_vulnerability() {
        let temp_dir = TempDir::new().unwrap();
        let db_path = temp_dir.path().join("test.db");
        let archive = TheArchive::the_awakening(db_path).unwrap();

        let score = VulnerabilityScore::new("CVE-2021-44228".to_string());

        // Store vulnerability
        archive.store_vulnerability(&score).unwrap();

        // Retrieve vulnerability
        let retrieved = archive.get_vulnerability("CVE-2021-44228").unwrap();
        assert!(retrieved.is_some());

        let retrieved = retrieved.unwrap();
        assert_eq!(retrieved.cve_id, "CVE-2021-44228");
    }

    #[test]
    fn test_list_scans() {
        let temp_dir = TempDir::new().unwrap();
        let db_path = temp_dir.path().join("test.db");
        let archive = TheArchive::the_awakening(db_path).unwrap();

        // Store multiple scans
        let scan1 = ScanMetadata::new("scan_1".to_string(), "192.168.1.0/24".to_string());
        let scan2 = ScanMetadata::new("scan_2".to_string(), "10.0.0.0/8".to_string());

        archive.store_scan_metadata(&scan1).unwrap();
        archive.store_scan_metadata(&scan2).unwrap();

        // List scans
        let scans = archive.list_scans().unwrap();
        assert_eq!(scans.len(), 2);
    }

    #[test]
    fn test_get_stats() {
        let temp_dir = TempDir::new().unwrap();
        let db_path = temp_dir.path().join("test.db");
        let archive = TheArchive::the_awakening(db_path).unwrap();

        // Add some data
        let metadata = ScanMetadata::new("scan_1".to_string(), "192.168.1.0/24".to_string());
        archive.store_scan_metadata(&metadata).unwrap();

        let score = VulnerabilityScore::new("CVE-2021-44228".to_string());
        archive.store_vulnerability(&score).unwrap();

        // Check stats
        let stats = archive.get_stats().unwrap();
        assert_eq!(stats.total_scans, 1);
        assert_eq!(stats.total_vulnerabilities, 1);
    }

    #[test]
    fn test_nonexistent_scan() {
        let temp_dir = TempDir::new().unwrap();
        let db_path = temp_dir.path().join("test.db");
        let archive = TheArchive::the_awakening(db_path).unwrap();

        let result = archive.get_scan_metadata("nonexistent").unwrap();
        assert!(result.is_none());
    }
}
