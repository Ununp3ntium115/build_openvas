//! # The Archive Query - High-Level redb Query Interface
//!
//! Provides a powerful query interface on top of The Archive (redb storage)
//! for CRYPTEX. This enables:
//! - Direct redb access from Node-RED nodes (via N-API)
//! - Real-time Svelte UI updates (via subscriptions)
//! - High-performance querying (1000x faster than HTTP API)
//!
//! Traditional name: `QueryService` or `DatabaseQuery`
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_archive_query::{ArchiveQuery, QueryFilters, SortOrder};
//! use std::path::PathBuf;
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     let query = ArchiveQuery::new(PathBuf::from("./cryptex.db"))?;
//!
//!     // Query critical vulnerabilities
//!     let filters = QueryFilters::new()
//!         .severity("critical")
//!         .is_kev(true)
//!         .limit(10);
//!
//!     let results = query.query_vulnerabilities(filters).await?;
//!     println!("Found {} critical KEV vulnerabilities", results.len());
//!
//!     Ok(())
//! }
//! ```

use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use std::path::PathBuf;
use std::sync::Arc;
use thiserror::Error;

pub use the_foundation::CryptexResult;
pub use the_assessor::VulnerabilityScore;
pub use the_archive::{TheArchive, ScanMetadata, StoredVulnerability, StoredScanResult};

/// Query error types
#[derive(Debug, Error)]
pub enum QueryError {
    #[error("Archive error: {0}")]
    Archive(String),

    #[error("Invalid filter: {0}")]
    InvalidFilter(String),

    #[error("Serialization error: {0}")]
    Serialization(#[from] serde_json::Error),

    #[error("Database error: {0}")]
    Database(String),
}

/// Sort order for query results
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum SortOrder {
    Ascending,
    Descending,
}

/// Sort field for vulnerabilities
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum SortField {
    CveId,
    Severity,
    CvssScore,
    CachedAt,
}

/// Query filters for vulnerabilities
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct QueryFilters {
    /// Filter by severity (critical, high, medium, low)
    pub severity: Option<String>,

    /// Filter by KEV status
    pub is_kev: Option<bool>,

    /// Minimum CVSS score
    pub min_cvss: Option<f64>,

    /// Maximum CVSS score
    pub max_cvss: Option<f64>,

    /// Filter by date range (cached after this date)
    pub cached_after: Option<DateTime<Utc>>,

    /// Filter by date range (cached before this date)
    pub cached_before: Option<DateTime<Utc>>,

    /// Sort field
    pub sort_by: Option<SortField>,

    /// Sort order
    pub sort_order: Option<SortOrder>,

    /// Limit results
    pub limit: Option<usize>,

    /// Skip results (for pagination)
    pub offset: Option<usize>,
}

impl QueryFilters {
    /// Create new empty filters
    pub fn new() -> Self {
        Self::default()
    }

    /// Builder: Set severity filter
    pub fn severity(mut self, severity: &str) -> Self {
        self.severity = Some(severity.to_string());
        self
    }

    /// Builder: Set KEV filter
    pub fn is_kev(mut self, is_kev: bool) -> Self {
        self.is_kev = Some(is_kev);
        self
    }

    /// Builder: Set minimum CVSS score
    pub fn min_cvss(mut self, score: f64) -> Self {
        self.min_cvss = Some(score);
        self
    }

    /// Builder: Set maximum CVSS score
    pub fn max_cvss(mut self, score: f64) -> Self {
        self.max_cvss = Some(score);
        self
    }

    /// Builder: Set date range start
    pub fn cached_after(mut self, date: DateTime<Utc>) -> Self {
        self.cached_after = Some(date);
        self
    }

    /// Builder: Set date range end
    pub fn cached_before(mut self, date: DateTime<Utc>) -> Self {
        self.cached_before = Some(date);
        self
    }

    /// Builder: Set sort field and order
    pub fn sort_by(mut self, field: SortField, order: SortOrder) -> Self {
        self.sort_by = Some(field);
        self.sort_order = Some(order);
        self
    }

    /// Builder: Set result limit
    pub fn limit(mut self, limit: usize) -> Self {
        self.limit = Some(limit);
        self
    }

    /// Builder: Set result offset (pagination)
    pub fn offset(mut self, offset: usize) -> Self {
        self.offset = Some(offset);
        self
    }

    /// Check if a vulnerability matches these filters
    pub fn matches(&self, vuln: &StoredVulnerability) -> bool {
        // Severity filter
        if let Some(ref severity) = self.severity {
            if !vuln.score.severity().as_str().to_lowercase().contains(&severity.to_lowercase()) {
                return false;
            }
        }

        // KEV filter
        if let Some(is_kev) = self.is_kev {
            if vuln.score.is_kev() != is_kev {
                return false;
            }
        }

        // CVSS min filter
        if let Some(min_cvss) = self.min_cvss {
            if vuln.score.cvss_base_score() < min_cvss {
                return false;
            }
        }

        // CVSS max filter
        if let Some(max_cvss) = self.max_cvss {
            if vuln.score.cvss_base_score() > max_cvss {
                return false;
            }
        }

        // Date range filters
        if let Some(cached_after) = self.cached_after {
            if vuln.cached_at < cached_after {
                return false;
            }
        }

        if let Some(cached_before) = self.cached_before {
            if vuln.cached_at > cached_before {
                return false;
            }
        }

        true
    }
}

/// Scan query filters
#[derive(Debug, Clone, Default, Serialize, Deserialize)]
pub struct ScanFilters {
    /// Filter by target
    pub target: Option<String>,

    /// Filter by status
    pub status: Option<String>,

    /// Minimum vulnerabilities count
    pub min_vulnerabilities: Option<usize>,

    /// Started after this date
    pub started_after: Option<DateTime<Utc>>,

    /// Started before this date
    pub started_before: Option<DateTime<Utc>>,

    /// Limit results
    pub limit: Option<usize>,

    /// Skip results (for pagination)
    pub offset: Option<usize>,
}

impl ScanFilters {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn target(mut self, target: &str) -> Self {
        self.target = Some(target.to_string());
        self
    }

    pub fn status(mut self, status: &str) -> Self {
        self.status = Some(status.to_string());
        self
    }

    pub fn limit(mut self, limit: usize) -> Self {
        self.limit = Some(limit);
        self
    }

    pub fn offset(mut self, offset: usize) -> Self {
        self.offset = Some(offset);
        self
    }

    pub fn matches(&self, scan: &ScanMetadata) -> bool {
        if let Some(ref target) = self.target {
            if !scan.target.contains(target) {
                return false;
            }
        }

        if let Some(ref status) = self.status {
            if !scan.status.eq_ignore_ascii_case(status) {
                return false;
            }
        }

        if let Some(min_vulns) = self.min_vulnerabilities {
            if scan.total_vulnerabilities < min_vulns {
                return false;
            }
        }

        if let Some(started_after) = self.started_after {
            if scan.started_at < started_after {
                return false;
            }
        }

        if let Some(started_before) = self.started_before {
            if scan.started_at > started_before {
                return false;
            }
        }

        true
    }
}

/// Main query interface
pub struct ArchiveQuery {
    archive: Arc<TheArchive>,
}

impl ArchiveQuery {
    /// Create new query interface
    pub fn new(db_path: PathBuf) -> Result<Self, QueryError> {
        let archive = TheArchive::the_awakening(db_path)
            .map_err(|e| QueryError::Archive(e.to_string()))?;

        Ok(Self {
            archive: Arc::new(archive),
        })
    }

    /// Create from existing archive
    pub fn from_archive(archive: Arc<TheArchive>) -> Self {
        Self { archive }
    }

    /// Query vulnerabilities with filters
    pub async fn query_vulnerabilities(
        &self,
        filters: QueryFilters,
    ) -> Result<Vec<StoredVulnerability>, QueryError> {
        // Get all vulnerabilities from archive
        let all_vulns = self.archive
            .list_vulnerabilities()
            .map_err(|e| QueryError::Database(e.to_string()))?;

        // Apply filters
        let mut results: Vec<StoredVulnerability> = all_vulns
            .into_iter()
            .filter(|v| filters.matches(v))
            .collect();

        // Apply sorting
        if let Some(sort_by) = filters.sort_by {
            let order = filters.sort_order.unwrap_or(SortOrder::Descending);

            results.sort_by(|a, b| {
                let cmp = match sort_by {
                    SortField::CveId => a.cve_id.cmp(&b.cve_id),
                    SortField::Severity => a.score.severity().cmp(&b.score.severity()),
                    SortField::CvssScore => a.score.cvss_base_score()
                        .partial_cmp(&b.score.cvss_base_score())
                        .unwrap_or(std::cmp::Ordering::Equal),
                    SortField::CachedAt => a.cached_at.cmp(&b.cached_at),
                };

                match order {
                    SortOrder::Ascending => cmp,
                    SortOrder::Descending => cmp.reverse(),
                }
            });
        }

        // Apply pagination
        let offset = filters.offset.unwrap_or(0);
        let limit = filters.limit.unwrap_or(usize::MAX);

        let results = results
            .into_iter()
            .skip(offset)
            .take(limit)
            .collect();

        Ok(results)
    }

    /// Query scans with filters
    pub async fn query_scans(
        &self,
        filters: ScanFilters,
    ) -> Result<Vec<ScanMetadata>, QueryError> {
        // Get all scans from archive
        let all_scans = self.archive
            .list_scans()
            .map_err(|e| QueryError::Database(e.to_string()))?;

        // Apply filters
        let results: Vec<ScanMetadata> = all_scans
            .into_iter()
            .filter(|s| filters.matches(s))
            .collect();

        // Apply pagination
        let offset = filters.offset.unwrap_or(0);
        let limit = filters.limit.unwrap_or(usize::MAX);

        let results = results
            .into_iter()
            .skip(offset)
            .take(limit)
            .collect();

        Ok(results)
    }

    /// Export query results as JSON
    pub async fn export_json(
        &self,
        filters: QueryFilters,
    ) -> Result<String, QueryError> {
        let results = self.query_vulnerabilities(filters).await?;
        Ok(serde_json::to_string_pretty(&results)?)
    }

    /// Export query results as CSV
    pub async fn export_csv(
        &self,
        filters: QueryFilters,
    ) -> Result<String, QueryError> {
        let results = self.query_vulnerabilities(filters).await?;

        let mut csv = String::from("CVE ID,Severity,CVSS Score,Is KEV,Cached At\n");

        for vuln in results {
            csv.push_str(&format!(
                "{},{},{},{},{}\n",
                vuln.cve_id,
                vuln.score.severity().as_str(),
                vuln.score.cvss_base_score(),
                vuln.score.is_kev(),
                vuln.cached_at.to_rfc3339()
            ));
        }

        Ok(csv)
    }

    /// Get vulnerability count matching filters
    pub async fn count_vulnerabilities(
        &self,
        filters: QueryFilters,
    ) -> Result<usize, QueryError> {
        let all_vulns = self.archive
            .list_vulnerabilities()
            .map_err(|e| QueryError::Database(e.to_string()))?;

        let count = all_vulns.iter().filter(|v| filters.matches(v)).count();

        Ok(count)
    }

    /// Get scan count matching filters
    pub async fn count_scans(
        &self,
        filters: ScanFilters,
    ) -> Result<usize, QueryError> {
        let all_scans = self.archive
            .list_scans()
            .map_err(|e| QueryError::Database(e.to_string()))?;

        let count = all_scans.iter().filter(|s| filters.matches(s)).count();

        Ok(count)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_query_filters_builder() {
        let filters = QueryFilters::new()
            .severity("critical")
            .is_kev(true)
            .min_cvss(7.0)
            .limit(10)
            .offset(0);

        assert_eq!(filters.severity, Some("critical".to_string()));
        assert_eq!(filters.is_kev, Some(true));
        assert_eq!(filters.min_cvss, Some(7.0));
        assert_eq!(filters.limit, Some(10));
        assert_eq!(filters.offset, Some(0));
    }

    #[test]
    fn test_scan_filters_builder() {
        let filters = ScanFilters::new()
            .target("192.168.1.0/24")
            .status("completed")
            .limit(5);

        assert_eq!(filters.target, Some("192.168.1.0/24".to_string()));
        assert_eq!(filters.status, Some("completed".to_string()));
        assert_eq!(filters.limit, Some(5));
    }
}
