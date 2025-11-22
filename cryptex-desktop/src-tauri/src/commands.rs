// Tauri Commands
// Exposed to frontend via Tauri IPC

use crate::{AppState, api_server::ApiServer};
use tauri::State;
use std::sync::MutexGuard;

#[tauri::command]
pub fn start_api_server(state: State<AppState>) -> Result<String, String> {
    let mut server_guard: MutexGuard<Option<ApiServer>> = state.api_server.lock()
        .map_err(|e| format!("Lock error: {}", e))?;

    if server_guard.is_none() {
        let mut server = ApiServer::new()
            .map_err(|e| format!("Failed to create server: {}", e))?;

        let url = server.start()
            .map_err(|e| format!("Failed to start server: {}", e))?;

        *server_guard = Some(server);
        Ok(url)
    } else {
        Err("API server already running".to_string())
    }
}

#[tauri::command]
pub fn stop_api_server(state: State<AppState>) {
    if let Ok(mut server_guard) = state.api_server.lock() {
        if let Some(mut server) = server_guard.take() {
            server.stop();
        }
    }
}

#[tauri::command]
pub fn get_api_status(state: State<AppState>) -> Result<bool, String> {
    let server_guard = state.api_server.lock()
        .map_err(|e| format!("Lock error: {}", e))?;

    Ok(server_guard.as_ref().map_or(false, |s| s.is_running()))
}

#[tauri::command]
pub async fn assess_vulnerability(cve_id: String) -> Result<serde_json::Value, String> {
    let url = format!("http://127.0.0.1:8080/api/v1/vulnerabilities/{}", cve_id);
    let response = reqwest::get(&url).await
        .map_err(|e| format!("Request failed: {}", e))?;

    response.json().await
        .map_err(|e| format!("JSON parse failed: {}", e))
}

#[tauri::command]
pub async fn start_scan(target: String) -> Result<serde_json::Value, String> {
    let client = reqwest::Client::new();
    let response = client
        .post("http://127.0.0.1:8080/api/v1/scans")
        .json(&serde_json::json!({"target": target}))
        .send()
        .await
        .map_err(|e| format!("Request failed: {}", e))?;

    response.json().await
        .map_err(|e| format!("JSON parse failed: {}", e))
}

#[tauri::command]
pub async fn get_scan_results(scan_id: String) -> Result<serde_json::Value, String> {
    let url = format!("http://127.0.0.1:8080/api/v1/scans/{}/results", scan_id);
    let response = reqwest::get(&url).await
        .map_err(|e| format!("Request failed: {}", e))?;

    response.json().await
        .map_err(|e| format!("JSON parse failed: {}", e))
}

#[tauri::command]
pub async fn generate_report(scan_id: String, format: String) -> Result<String, String> {
    let client = reqwest::Client::new();
    let response = client
        .post(format!("http://127.0.0.1:8080/api/v1/reports/{}", scan_id))
        .json(&serde_json::json!({"format": format}))
        .send()
        .await
        .map_err(|e| format!("Request failed: {}", e))?;

    response.text().await
        .map_err(|e| format!("Text parse failed: {}", e))
}

#[tauri::command]
pub async fn get_archive_stats() -> Result<serde_json::Value, String> {
    let response = reqwest::get("http://127.0.0.1:8080/api/v1/archive/stats").await
        .map_err(|e| format!("Request failed: {}", e))?;

    response.json().await
        .map_err(|e| format!("JSON parse failed: {}", e))
}

#[tauri::command]
pub fn open_node_red() -> Result<(), String> {
    open::that("http://localhost:1880")
        .map_err(|e| format!("Failed to open Node-RED: {}", e))
}

#[tauri::command]
pub fn connect_to_pyro() -> Result<(), String> {
    // Open PYRO platform in browser
    open::that("http://localhost:3000")
        .map_err(|e| format!("Failed to open PYRO: {}", e))
}
