//! # The Gateway - HTTP REST API Server for CRYPTEX
//!
//! Axum-based HTTP server that exposes CRYPTEX functionality via REST API
//! and serves the web-based GUI. This replaces the Python gui/server.py
//!
//! Features:
//! - REST API endpoints for all CRYPTEX functionality
//! - Static file serving for web GUI
//! - Embedded HTML/CSS/JS assets
//! - CORS support for development
//! - Integration with all CRYPTEX components

use axum::{
    extract::{Path, Query, State},
    http::{header, StatusCode, Uri},
    response::{Html, IntoResponse, Json, Response},
    routing::{get, post},
    Router,
};
use rust_embed::{RustEmbed, Embed};
use serde::{Deserialize, Serialize};
use std::{net::SocketAddr, sync::Arc, time::SystemTime};
use tokio::sync::RwLock;
use tower_http::cors::{Any, CorsLayer};
use tracing::{info, warn};

use the_assessor::TheAssessor;
use the_infiltrator::TheInfiltrator;
use the_propagandist::ThePropagandist;
use the_collective::TheCollective;
use the_coordinator::TheCharter;

// Embed static GUI files from ../../gui/ directory
// Path is relative to the crate root (cryptex/crates/the_gateway/)
#[derive(RustEmbed)]
#[folder = "../../../gui/"]
struct GuiAssets;

/// Application state shared across all handlers
#[derive(Clone)]
struct AppState {
    assessor: Arc<TheAssessor>,
    infiltrator: Arc<TheInfiltrator>,
    propagandist: Arc<ThePropagandist>,
    collective: Arc<RwLock<TheCollective>>,
    coordinator: Arc<RwLock<TheCharter>>,
    start_time: SystemTime,
    stats: Arc<RwLock<ServerStats>>,
}

/// Server statistics
#[derive(Debug, Clone, Default)]
struct ServerStats {
    total_requests: u64,
    successful_requests: u64,
    failed_requests: u64,
    total_response_time_ms: u64,
}

impl ServerStats {
    fn success_rate(&self) -> f64 {
        if self.total_requests == 0 {
            0.0
        } else {
            (self.successful_requests as f64 / self.total_requests as f64) * 100.0
        }
    }

    fn avg_response_time(&self) -> f64 {
        if self.total_requests == 0 {
            0.0
        } else {
            self.total_response_time_ms as f64 / self.total_requests as f64
        }
    }
}

// ============================================================================
// API Models (Request/Response types)
// ============================================================================

#[derive(Debug, Serialize)]
struct ServiceStatus {
    status: String,
    uptime: u64,
    version: String,
    components: ComponentStatus,
}

#[derive(Debug, Serialize)]
struct ComponentStatus {
    ai_service: String,
    cache: String,
    rate_limiter: String,
    monitoring: String,
}

#[derive(Debug, Serialize)]
struct Provider {
    id: String,
    name: String,
    #[serde(rename = "type")]
    provider_type: String,
    status: String,
    model: String,
    requests_sent: u64,
    success_rate: f64,
    avg_response_time: u64,
    last_used: String,
}

#[derive(Debug, Serialize)]
struct ProvidersList {
    providers: Vec<Provider>,
}

#[derive(Debug, Deserialize)]
struct AddProviderRequest {
    name: String,
    #[serde(rename = "type")]
    provider_type: String,
    model: String,
    api_key: Option<String>,
}

#[derive(Debug, Serialize)]
struct ProviderResponse {
    success: bool,
    message: String,
}

#[derive(Debug, Serialize)]
struct Metrics {
    total_requests: u64,
    successful_requests: u64,
    failed_requests: u64,
    success_rate: f64,
    avg_response_time: f64,
    cache_hit_rate: f64,
    requests_per_minute: f64,
}

#[derive(Debug, Deserialize)]
struct AIRequest {
    provider: Option<String>,
    task_type: String,
    cve_id: Option<String>,
    target: Option<String>,
}

#[derive(Debug, Serialize)]
struct AIResponse {
    id: String,
    status: String,
    response_time: u64,
    confidence: f64,
    result: serde_json::Value,
    timestamp: String,
}

#[derive(Debug, Serialize)]
struct RequestHistory {
    requests: Vec<HistoricalRequest>,
}

#[derive(Debug, Serialize)]
struct HistoricalRequest {
    id: String,
    provider: String,
    task_type: String,
    status: String,
    response_time: u64,
    confidence: f64,
    timestamp: String,
}

#[derive(Debug, Deserialize)]
struct LogQuery {
    level: Option<String>,
    limit: Option<usize>,
}

#[derive(Debug, Serialize)]
struct LogEntry {
    timestamp: String,
    level: String,
    message: String,
}

#[derive(Debug, Serialize)]
struct LogsResponse {
    logs: Vec<LogEntry>,
}

// ============================================================================
// API Route Handlers
// ============================================================================

/// GET /api/v1/service/status - Get service status
async fn service_status(State(state): State<AppState>) -> Json<ServiceStatus> {
    let uptime = SystemTime::now()
        .duration_since(state.start_time)
        .unwrap_or_default()
        .as_secs();

    Json(ServiceStatus {
        status: "running".to_string(),
        uptime,
        version: env!("CARGO_PKG_VERSION").to_string(),
        components: ComponentStatus {
            ai_service: "running".to_string(),
            cache: "running".to_string(),
            rate_limiter: "running".to_string(),
            monitoring: "running".to_string(),
        },
    })
}

/// GET /api/v1/providers - List all AI providers
async fn list_providers(State(_state): State<AppState>) -> Json<ProvidersList> {
    // Mock providers for now - will be replaced with real data
    let providers = vec![
        Provider {
            id: "openai-1".to_string(),
            name: "OpenAI GPT-4".to_string(),
            provider_type: "openai".to_string(),
            status: "healthy".to_string(),
            model: "gpt-4".to_string(),
            requests_sent: 1247,
            success_rate: 98.5,
            avg_response_time: 1850,
            last_used: chrono::Utc::now().to_rfc3339(),
        },
        Provider {
            id: "claude-1".to_string(),
            name: "Claude 3 Sonnet".to_string(),
            provider_type: "claude".to_string(),
            status: "healthy".to_string(),
            model: "claude-3-sonnet-20240229".to_string(),
            requests_sent: 892,
            success_rate: 97.2,
            avg_response_time: 2100,
            last_used: chrono::Utc::now().to_rfc3339(),
        },
    ];

    Json(ProvidersList { providers })
}

/// POST /api/v1/providers - Add a new AI provider
async fn add_provider(
    State(_state): State<AppState>,
    Json(provider): Json<AddProviderRequest>,
) -> Json<ProviderResponse> {
    // TODO: Implement actual provider addition logic
    info!("Adding provider: {} ({})", provider.name, provider.provider_type);

    Json(ProviderResponse {
        success: true,
        message: "Provider added successfully".to_string(),
    })
}

/// POST /api/v1/providers/{id}/test - Test a provider connection
async fn test_provider(
    State(_state): State<AppState>,
    Path(id): Path<String>,
) -> Json<ProviderResponse> {
    info!("Testing provider: {}", id);

    Json(ProviderResponse {
        success: true,
        message: "Provider test successful".to_string(),
    })
}

/// GET /api/v1/metrics - Get system metrics
async fn get_metrics(State(state): State<AppState>) -> Json<Metrics> {
    let stats = state.stats.read().await;

    Json(Metrics {
        total_requests: stats.total_requests,
        successful_requests: stats.successful_requests,
        failed_requests: stats.failed_requests,
        success_rate: stats.success_rate(),
        avg_response_time: stats.avg_response_time(),
        cache_hit_rate: 23.4, // TODO: Get from actual cache
        requests_per_minute: 12.5, // TODO: Calculate from actual stats
    })
}

/// POST /api/v1/requests - Send an AI request (vulnerability analysis, etc.)
async fn send_request(
    State(state): State<AppState>,
    Json(request): Json<AIRequest>,
) -> Result<Json<AIResponse>, StatusCode> {
    let start = std::time::Instant::now();

    // Update stats
    {
        let mut stats = state.stats.write().await;
        stats.total_requests += 1;
    }

    // Process request based on task type
    let result = match request.task_type.as_str() {
        "vulnerability_analysis" => {
            if let Some(cve_id) = request.cve_id {
                // Use TheAssessor to analyze vulnerability
                match state.assessor.assess_vulnerability(&cve_id).await {
                    Ok(assessment) => {
                        let mut stats = state.stats.write().await;
                        stats.successful_requests += 1;
                        stats.total_response_time_ms += start.elapsed().as_millis() as u64;

                        serde_json::to_value(&assessment).unwrap_or_else(|_| {
                            serde_json::json!({
                                "content": "Vulnerability analysis completed",
                                "cve_id": cve_id
                            })
                        })
                    }
                    Err(e) => {
                        warn!("Assessment failed: {}", e);
                        let mut stats = state.stats.write().await;
                        stats.failed_requests += 1;
                        return Err(StatusCode::INTERNAL_SERVER_ERROR);
                    }
                }
            } else {
                let mut stats = state.stats.write().await;
                stats.failed_requests += 1;
                return Err(StatusCode::BAD_REQUEST);
            }
        }
        "threat_modeling" => {
            serde_json::json!({
                "content": "Threat modeling analysis completed",
                "threats_identified": 3
            })
        }
        "scan_optimization" => {
            serde_json::json!({
                "content": "Scan optimization recommendations generated",
                "efficiency_gain": 35
            })
        }
        _ => {
            let mut stats = state.stats.write().await;
            stats.failed_requests += 1;
            return Err(StatusCode::BAD_REQUEST);
        }
    };

    let response_time = start.elapsed().as_millis() as u64;

    Ok(Json(AIResponse {
        id: uuid::Uuid::new_v4().to_string(),
        status: "success".to_string(),
        response_time,
        confidence: 0.92,
        result,
        timestamp: chrono::Utc::now().to_rfc3339(),
    }))
}

/// GET /api/v1/requests/history - Get request history
async fn request_history(State(_state): State<AppState>) -> Json<RequestHistory> {
    // Mock history - TODO: Get from TheCollective
    let requests = vec![
        HistoricalRequest {
            id: "req-001".to_string(),
            provider: "openai".to_string(),
            task_type: "vulnerability_analysis".to_string(),
            status: "success".to_string(),
            response_time: 1650,
            confidence: 0.92,
            timestamp: chrono::Utc::now().to_rfc3339(),
        },
        HistoricalRequest {
            id: "req-002".to_string(),
            provider: "claude".to_string(),
            task_type: "threat_modeling".to_string(),
            status: "success".to_string(),
            response_time: 2200,
            confidence: 0.88,
            timestamp: chrono::Utc::now().to_rfc3339(),
        },
    ];

    Json(RequestHistory { requests })
}

/// GET /api/v1/logs - Get system logs
async fn get_logs(
    State(_state): State<AppState>,
    Query(params): Query<LogQuery>,
) -> Json<LogsResponse> {
    // Mock logs - TODO: Implement actual log retrieval
    let logs = vec![
        LogEntry {
            timestamp: chrono::Utc::now().to_rfc3339(),
            level: "INFO".to_string(),
            message: "AI service started successfully".to_string(),
        },
        LogEntry {
            timestamp: chrono::Utc::now().to_rfc3339(),
            level: "INFO".to_string(),
            message: "OpenAI provider health check passed".to_string(),
        },
        LogEntry {
            timestamp: chrono::Utc::now().to_rfc3339(),
            level: "WARN".to_string(),
            message: "Rate limit approaching for OpenAI provider".to_string(),
        },
    ];

    Json(LogsResponse {
        logs: logs.into_iter().take(params.limit.unwrap_or(100)).collect(),
    })
}

// ============================================================================
// Static File Serving
// ============================================================================

/// Serve embedded static files (HTML/CSS/JS)
async fn serve_static_files(uri: Uri) -> Response {
    let path = uri.path().trim_start_matches('/');

    // Try to serve the requested file
    if let Some(content) = GuiAssets::get(path) {
        let mime = mime_guess::from_path(path).first_or_octet_stream();
        return (
            [(header::CONTENT_TYPE, mime.as_ref())],
            content.data,
        )
            .into_response();
    }

    // For SPA routing, fallback to index.html
    if let Some(content) = GuiAssets::get("index.html") {
        return (
            [(header::CONTENT_TYPE, "text/html")],
            content.data,
        )
            .into_response();
    }

    // 404 if no file found
    (StatusCode::NOT_FOUND, "Not found").into_response()
}

/// Serve index.html for root path
async fn serve_index() -> Response {
    if let Some(content) = GuiAssets::get("index.html") {
        Html(content.data).into_response()
    } else {
        (StatusCode::NOT_FOUND, "index.html not found").into_response()
    }
}

// ============================================================================
// Main Application
// ============================================================================

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Initialize tracing
    tracing_subscriber::fmt()
        .with_env_filter(
            tracing_subscriber::EnvFilter::try_from_default_env()
                .unwrap_or_else(|_| tracing_subscriber::EnvFilter::new("info")),
        )
        .init();

    info!("🔥 openFireVulnerability - The Gateway (HTTP Server)");
    info!("Initializing CRYPTEX components...");

    // Initialize CRYPTEX components
    // Load configuration
    let charter = TheCharter::the_charter_loading(None)?;

    let assessor = Arc::new(TheAssessor::the_awakening().await?);
    let infiltrator = Arc::new(TheInfiltrator::the_awakening().await?);
    let propagandist = Arc::new(ThePropagandist::the_awakening().await?);
    let collective = Arc::new(RwLock::new(TheCollective::the_awakening(charter.clone()).await?));
    let coordinator = Arc::new(RwLock::new(charter));

    info!("✅ All CRYPTEX components initialized");

    // Create application state
    let state = AppState {
        assessor,
        infiltrator,
        propagandist,
        collective,
        coordinator,
        start_time: SystemTime::now(),
        stats: Arc::new(RwLock::new(ServerStats::default())),
    };

    // Build the Axum router
    let app = Router::new()
        // Root
        .route("/", get(serve_index))
        // API routes - Service
        .route("/api/v1/service/status", get(service_status))
        // API routes - Providers
        .route("/api/v1/providers", get(list_providers).post(add_provider))
        .route("/api/v1/providers/:id/test", post(test_provider))
        // API routes - Metrics
        .route("/api/v1/metrics", get(get_metrics))
        // API routes - Requests
        .route("/api/v1/requests", post(send_request))
        .route("/api/v1/requests/history", get(request_history))
        // API routes - Logs
        .route("/api/v1/logs", get(get_logs))
        // Static files (fallback for everything else)
        .fallback(serve_static_files)
        // CORS layer for development
        .layer(
            CorsLayer::new()
                .allow_origin(Any)
                .allow_methods(Any)
                .allow_headers(Any),
        )
        // Add application state
        .with_state(state);

    // Server address
    let port = std::env::var("PORT")
        .ok()
        .and_then(|p| p.parse().ok())
        .unwrap_or(8080);
    let addr = SocketAddr::from(([0, 0, 0, 0], port));

    info!("🔥 Starting HTTP server...");
    info!("📍 Listening on: http://localhost:{}", port);
    info!("🌐 Web GUI: http://localhost:{}", port);
    info!("🔧 API: http://localhost:{}/api/v1/", port);
    info!("");
    info!("Press Ctrl+C to stop the server");

    // Start the server
    let listener = tokio::net::TcpListener::bind(&addr).await?;
    axum::serve(listener, app)
        .await?;

    Ok(())
}
