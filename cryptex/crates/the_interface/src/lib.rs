//! # The Interface - REST API Server
//!
//! The Interface provides a RESTful API for interacting with CRYPTEX.
//!
//! Traditional name: `Server` or `APIServer`
//!
//! ## Features
//!
//! - **RESTful API**: Standard HTTP endpoints for all CRYPTEX operations
//! - **Vulnerability Assessment**: CVE scoring and analysis
//! - **Scan Management**: Start, monitor, and complete scans
//! - **Report Generation**: Multi-format report export
//! - **Historical Data**: Query archived scan results
//! - **CORS Support**: Cross-origin resource sharing
//! - **Request Tracing**: Comprehensive logging
//!
//! ## Example
//!
//! ```rust,no_run
//! use the_interface::TheInterface;
//! use std::path::PathBuf;
//!
//! #[tokio::main]
//! async fn main() -> Result<(), Box<dyn std::error::Error>> {
//!     // Initialize the interface
//!     let interface = TheInterface::the_awakening(
//!         "0.0.0.0:8080".to_string(),
//!         PathBuf::from("./cryptex.db")
//!     ).await?;
//!
//!     // Start the server
//!     interface.the_manifestation().await?;
//!
//!     Ok(())
//! }
//! ```

use axum::{
    extract::{Path, Query, State},
    http::StatusCode,
    response::{IntoResponse, Response},
    routing::{get, post},
    Json, Router,
};
use serde::{Deserialize, Serialize};
use std::path::PathBuf;
use std::sync::Arc;
use tower_http::cors::{Any, CorsLayer};
use tower_http::trace::TraceLayer;

pub use the_foundation::{CryptexError, CryptexResult};
use the_archive::{ArchiveStats, ScanMetadata, TheArchive};
use the_assessor::{TheAssessor, VulnerabilityScore};
use the_infiltrator::{ScanReport, TheInfiltrator};
use the_propagandist::{ReportFormat, ThePropagandist};

/// Application state shared across handlers
#[derive(Clone)]
pub struct AppState {
    assessor: Arc<TheAssessor>,
    infiltrator: Arc<TheInfiltrator>,
    propagandist: Arc<ThePropagandist>,
    archive: Arc<TheArchive>,
}

/// The Interface - Main REST API server
pub struct TheInterface {
    bind_address: String,
    state: AppState,
}

impl TheInterface {
    /// The Awakening - Initialize The Interface
    pub async fn the_awakening(
        bind_address: String,
        archive_path: PathBuf,
    ) -> CryptexResult<Self> {
        tracing::info!("The Interface awakening on {}", bind_address);

        // Initialize components
        let assessor = Arc::new(TheAssessor::the_awakening().await?);
        let infiltrator = Arc::new(TheInfiltrator::the_awakening().await?);
        let propagandist = Arc::new(ThePropagandist::the_awakening().await?);
        let archive = Arc::new(TheArchive::the_awakening(archive_path)?);

        let state = AppState {
            assessor,
            infiltrator,
            propagandist,
            archive,
        };

        Ok(Self {
            bind_address,
            state,
        })
    }

    /// The Manifestation - Start the HTTP server
    ///
    /// Traditional name: `serve` or `run`
    pub async fn the_manifestation(self) -> CryptexResult<()> {
        tracing::info!("The Interface manifesting at {}", self.bind_address);

        let app = create_router(self.state);

        let listener = tokio::net::TcpListener::bind(&self.bind_address)
            .await
            .map_err(|e| {
                CryptexError::ArchiveError(format!("Failed to bind to {}: {}", self.bind_address, e))
            })?;

        tracing::info!("The Interface ready at {}", self.bind_address);

        axum::serve(listener, app).await.map_err(|e| {
            CryptexError::ArchiveError(format!("Server error: {}", e))
        })?;

        Ok(())
    }
}

/// Create the API router
fn create_router(state: AppState) -> Router {
    Router::new()
        // Health check
        .route("/health", get(health_check))
        // Vulnerability assessment
        .route("/api/v1/vulnerabilities/:cve_id", get(assess_vulnerability))
        // Scans
        .route("/api/v1/scans", post(start_scan))
        .route("/api/v1/scans", get(list_scans))
        .route("/api/v1/scans/:scan_id", get(get_scan))
        .route("/api/v1/scans/:scan_id/end", post(end_scan))
        .route("/api/v1/scans/:scan_id/results", get(get_scan_results))
        // Reports
        .route("/api/v1/scans/:scan_id/report", get(generate_report))
        .route("/api/v1/scans/:scan_id/executive-summary", get(get_executive_summary))
        // Archive
        .route("/api/v1/archive/stats", get(get_archive_stats))
        .with_state(state)
        .layer(CorsLayer::new().allow_origin(Any))
        .layer(TraceLayer::new_for_http())
}

// ============================================================================
// Request/Response Types
// ============================================================================

#[derive(Debug, Serialize)]
struct HealthResponse {
    status: String,
    version: String,
}

#[derive(Debug, Deserialize)]
struct StartScanRequest {
    target: String,
}

#[derive(Debug, Serialize)]
struct StartScanResponse {
    scan_id: String,
    target: String,
    started_at: String,
}

#[derive(Debug, Deserialize)]
struct ReportQuery {
    format: Option<String>,
}

// ============================================================================
// Handler Functions
// ============================================================================

/// Health check endpoint
async fn health_check() -> Json<HealthResponse> {
    Json(HealthResponse {
        status: "healthy".to_string(),
        version: env!("CARGO_PKG_VERSION").to_string(),
    })
}

/// Assess a CVE vulnerability
async fn assess_vulnerability(
    State(state): State<AppState>,
    Path(cve_id): Path<String>,
) -> Result<Json<VulnerabilityScore>, ApiError> {
    tracing::info!("Assessing vulnerability: {}", cve_id);

    // Check archive first
    if let Some(stored) = state.archive.get_vulnerability(&cve_id)? {
        tracing::debug!("Vulnerability {} found in archive", cve_id);
        return Ok(Json(stored.score));
    }

    // Assess and store
    let score = state.assessor.assess_vulnerability(&cve_id).await?;
    state.archive.store_vulnerability(&score)?;

    Ok(Json(score))
}

/// Start a new scan
async fn start_scan(
    State(state): State<AppState>,
    Json(request): Json<StartScanRequest>,
) -> Result<Json<StartScanResponse>, ApiError> {
    tracing::info!("Starting scan on target: {}", request.target);

    let scan_id = state.infiltrator.start_scan(&request.target).await?;

    // Store scan metadata
    let metadata = ScanMetadata::new(scan_id.clone(), request.target.clone());
    state.archive.store_scan_metadata(&metadata)?;

    Ok(Json(StartScanResponse {
        scan_id: scan_id.clone(),
        target: request.target,
        started_at: chrono::Utc::now().to_rfc3339(),
    }))
}

/// List all scans
async fn list_scans(
    State(state): State<AppState>,
) -> Result<Json<Vec<ScanMetadata>>, ApiError> {
    tracing::info!("Listing all scans");

    let scans = state.archive.list_scans()?;

    Ok(Json(scans))
}

/// Get scan metadata
async fn get_scan(
    State(state): State<AppState>,
    Path(scan_id): Path<String>,
) -> Result<Json<ScanMetadata>, ApiError> {
    tracing::info!("Getting scan: {}", scan_id);

    let metadata = state
        .archive
        .get_scan_metadata(&scan_id)?
        .ok_or_else(|| ApiError::not_found("Scan not found"))?;

    Ok(Json(metadata))
}

/// End a scan
async fn end_scan(
    State(state): State<AppState>,
    Path(scan_id): Path<String>,
) -> Result<Json<ScanReport>, ApiError> {
    tracing::info!("Ending scan: {}", scan_id);

    let report = state.infiltrator.end_scan(&scan_id).await?;

    // Update metadata in archive
    let mut metadata = state
        .archive
        .get_scan_metadata(&scan_id)?
        .ok_or_else(|| ApiError::not_found("Scan not found"))?;

    metadata.status = "completed".to_string();
    metadata.ended_at = Some(chrono::Utc::now());
    metadata.total_vulnerabilities = report.total_vulnerabilities;
    metadata.critical = report.critical_count;
    metadata.high = report.high_count;
    metadata.medium = report.medium_count;
    metadata.low = report.low_count;

    state.archive.store_scan_metadata(&metadata)?;

    Ok(Json(report))
}

/// Get scan results
async fn get_scan_results(
    State(state): State<AppState>,
    Path(scan_id): Path<String>,
) -> Result<Json<Vec<the_infiltrator::ScanResult>>, ApiError> {
    tracing::info!("Getting results for scan: {}", scan_id);

    let results = state.archive.get_scan_results(&scan_id)?;

    Ok(Json(results))
}

/// Generate report
async fn generate_report(
    State(state): State<AppState>,
    Path(scan_id): Path<String>,
    Query(query): Query<ReportQuery>,
) -> Result<Response, ApiError> {
    tracing::info!("Generating report for scan: {}", scan_id);

    // Get scan report
    let report = state.infiltrator.end_scan(&scan_id).await?;

    // Determine format
    let format_str = query.format.as_deref().unwrap_or("json");
    let format = match format_str {
        "html" => ReportFormat::Html,
        "markdown" | "md" => ReportFormat::Markdown,
        "text" | "txt" => ReportFormat::Text,
        _ => ReportFormat::Json,
    };

    // Generate report
    let content = state
        .propagandist
        .generate_report(&report, format)
        .await?;

    // Set appropriate content type
    let content_type = match format {
        ReportFormat::Html => "text/html",
        ReportFormat::Markdown => "text/markdown",
        ReportFormat::Text => "text/plain",
        ReportFormat::Json => "application/json",
    };

    Ok((
        StatusCode::OK,
        [(axum::http::header::CONTENT_TYPE, content_type)],
        content,
    )
        .into_response())
}

/// Get executive summary
async fn get_executive_summary(
    State(state): State<AppState>,
    Path(scan_id): Path<String>,
) -> Result<Response, ApiError> {
    tracing::info!("Generating executive summary for scan: {}", scan_id);

    // Get scan report
    let report = state.infiltrator.end_scan(&scan_id).await?;

    // Generate executive summary
    let summary = state
        .propagandist
        .generate_executive_summary(&report)
        .await?;

    Ok((StatusCode::OK, summary).into_response())
}

/// Get archive statistics
async fn get_archive_stats(
    State(state): State<AppState>,
) -> Result<Json<ArchiveStats>, ApiError> {
    tracing::info!("Getting archive statistics");

    let stats = state.archive.get_stats()?;

    Ok(Json(stats))
}

// ============================================================================
// Error Handling
// ============================================================================

/// API error response
#[derive(Debug, Serialize)]
struct ErrorResponse {
    error: String,
    message: String,
}

/// API error wrapper
struct ApiError {
    status: StatusCode,
    message: String,
}

impl ApiError {
    fn not_found(message: &str) -> Self {
        Self {
            status: StatusCode::NOT_FOUND,
            message: message.to_string(),
        }
    }
}

impl From<CryptexError> for ApiError {
    fn from(err: CryptexError) -> Self {
        tracing::error!("API error: {:?}", err);
        Self {
            status: StatusCode::INTERNAL_SERVER_ERROR,
            message: err.to_string(),
        }
    }
}

impl IntoResponse for ApiError {
    fn into_response(self) -> Response {
        let body = Json(ErrorResponse {
            error: self.status.to_string(),
            message: self.message,
        });

        (self.status, body).into_response()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_health_response_creation() {
        let response = HealthResponse {
            status: "healthy".to_string(),
            version: "1.0.0".to_string(),
        };

        assert_eq!(response.status, "healthy");
        assert_eq!(response.version, "1.0.0");
    }

    #[test]
    fn test_start_scan_request_deserialization() {
        let json = r#"{"target": "192.168.1.0/24"}"#;
        let request: StartScanRequest = serde_json::from_str(json).unwrap();

        assert_eq!(request.target, "192.168.1.0/24");
    }

    #[test]
    fn test_report_query_deserialization() {
        let query = ReportQuery {
            format: Some("html".to_string()),
        };

        assert_eq!(query.format.unwrap(), "html");
    }
}
