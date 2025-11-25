//! # openFireVulnerability - Unified Binary
//!
//! Single executable that supports multiple operation modes:
//! - MCP mode: STDIN/STDOUT for PYRO Platform integration
//! - HTTP mode: Web server for GUI
//! - Hybrid mode: Both MCP and HTTP simultaneously
//! - Desktop mode: Launch Tauri desktop application
//!
//! This eliminates the need for multiple binaries and provides
//! a unified interface for all deployment scenarios.

use clap::{Parser, Subcommand};
use std::process::{Command, Stdio};
use std::env;
use std::path::PathBuf;

#[derive(Parser)]
#[command(name = "openFireVulnerability")]
#[command(about = "🔥 AI-Enhanced Vulnerability Assessment Platform", long_about = None)]
#[command(version)]
struct Cli {
    #[command(subcommand)]
    mode: Mode,
}

#[derive(Subcommand)]
enum Mode {
    /// Run as MCP server (STDIN/STDOUT) for PYRO Platform integration
    ///
    /// This mode enables Fire Marshal agents to use CRYPTEX tools via the
    /// Model Context Protocol. Perfect for AI agent orchestration.
    Mcp {
        /// Optional configuration file path
        #[arg(short, long)]
        config: Option<PathBuf>,
    },

    /// Run as HTTP server for web-based GUI
    ///
    /// Starts the HTTP REST API server on the specified port, serving
    /// the embedded web GUI and providing API endpoints for programmatic access.
    Http {
        /// Port to listen on
        #[arg(short, long, default_value = "8080")]
        port: u16,

        /// Optional configuration file path
        #[arg(short, long)]
        config: Option<PathBuf>,
    },

    /// Run both MCP and HTTP modes simultaneously
    ///
    /// Enables full functionality - Fire Marshal integration via MCP
    /// AND web GUI access via HTTP. Best for comprehensive deployments.
    Hybrid {
        /// Port for HTTP server
        #[arg(short, long, default_value = "8080")]
        port: u16,

        /// Optional configuration file path
        #[arg(short, long)]
        config: Option<PathBuf>,
    },

    /// Launch desktop application (Tauri)
    ///
    /// Starts the native desktop GUI application. Provides the best
    /// user experience for local installations.
    Desktop {
        /// Optional configuration file path
        #[arg(short, long)]
        config: Option<PathBuf>,
    },
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();

    // Get the directory where this executable is located
    let exe_path = env::current_exe()?;
    let exe_dir = exe_path.parent().unwrap_or_else(|| std::path::Path::new("."));

    match cli.mode {
        Mode::Mcp { config } => {
            println!("🔥 openFireVulnerability - MCP Mode");
            println!("Starting MCP server for PYRO Platform integration...\n");

            run_mcp_server(exe_dir, config)?;
        }

        Mode::Http { port, config } => {
            println!("🔥 openFireVulnerability - HTTP Mode");
            println!("Starting HTTP server on port {}...\n", port);

            run_http_server(exe_dir, port, config)?;
        }

        Mode::Hybrid { port, config } => {
            println!("🔥 openFireVulnerability - Hybrid Mode");
            println!("Starting both MCP and HTTP servers...\n");

            run_hybrid_mode(exe_dir, port, config)?;
        }

        Mode::Desktop { config } => {
            println!("🔥 openFireVulnerability - Desktop Mode");
            println!("Launching desktop application...\n");

            run_desktop_app(exe_dir, config)?;
        }
    }

    Ok(())
}

/// Run MCP server mode (STDIN/STDOUT)
fn run_mcp_server(exe_dir: &std::path::Path, config: Option<PathBuf>) -> Result<(), Box<dyn std::error::Error>> {
    let mcp_binary = exe_dir.join("cryptex-mcp-server");

    if !mcp_binary.exists() {
        eprintln!("❌ Error: MCP server binary not found: {}", mcp_binary.display());
        eprintln!("Expected location: {}", mcp_binary.display());
        eprintln!("\nPlease ensure the CRYPTEX binaries are built and located in the same directory.");
        eprintln!("Run: cargo build --release");
        return Err("MCP server binary not found".into());
    }

    let mut cmd = Command::new(&mcp_binary);

    if let Some(config_path) = config {
        cmd.env("CRYPTEX_CONFIG", config_path);
    }

    // Execute MCP server with inherited STDIN/STDOUT
    let status = cmd
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status()?;

    if !status.success() {
        return Err(format!("MCP server exited with status: {}", status).into());
    }

    Ok(())
}

/// Run HTTP server mode
fn run_http_server(exe_dir: &std::path::Path, port: u16, config: Option<PathBuf>) -> Result<(), Box<dyn std::error::Error>> {
    let http_binary = exe_dir.join("the-gateway");

    if !http_binary.exists() {
        eprintln!("❌ Error: HTTP server binary not found: {}", http_binary.display());
        eprintln!("Expected location: {}", http_binary.display());
        eprintln!("\nPlease ensure the CRYPTEX binaries are built and located in the same directory.");
        eprintln!("Run: cargo build --release");
        return Err("HTTP server binary not found".into());
    }

    let mut cmd = Command::new(&http_binary);

    cmd.env("PORT", port.to_string());

    if let Some(config_path) = config {
        cmd.env("CRYPTEX_CONFIG", config_path);
    }

    // Execute HTTP server with inherited STDIO
    let status = cmd
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status()?;

    if !status.success() {
        return Err(format!("HTTP server exited with status: {}", status).into());
    }

    Ok(())
}

/// Run hybrid mode (both MCP and HTTP)
fn run_hybrid_mode(exe_dir: &std::path::Path, port: u16, config: Option<PathBuf>) -> Result<(), Box<dyn std::error::Error>> {
    let mcp_binary = exe_dir.join("cryptex-mcp-server");
    let http_binary = exe_dir.join("the-gateway");

    if !mcp_binary.exists() {
        eprintln!("❌ Error: MCP server binary not found: {}", mcp_binary.display());
        return Err("MCP server binary not found".into());
    }

    if !http_binary.exists() {
        eprintln!("❌ Error: HTTP server binary not found: {}", http_binary.display());
        return Err("HTTP server binary not found".into());
    }

    println!("Starting MCP server (background)...");

    // Start MCP server in background
    let mut mcp_cmd = Command::new(&mcp_binary);
    if let Some(ref config_path) = config {
        mcp_cmd.env("CRYPTEX_CONFIG", config_path);
    }

    let mcp_child = mcp_cmd
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::inherit())
        .spawn()?;

    println!("✅ MCP server started (PID: {})", mcp_child.id());
    println!("\nStarting HTTP server...");

    // Start HTTP server in foreground
    let mut http_cmd = Command::new(&http_binary);
    http_cmd.env("PORT", port.to_string());
    if let Some(config_path) = config {
        http_cmd.env("CRYPTEX_CONFIG", config_path);
    }

    let status = http_cmd
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status()?;

    if !status.success() {
        return Err(format!("HTTP server exited with status: {}", status).into());
    }

    Ok(())
}

/// Run desktop application
fn run_desktop_app(exe_dir: &std::path::Path, config: Option<PathBuf>) -> Result<(), Box<dyn std::error::Error>> {
    // Look for the Tauri desktop app
    let desktop_binary = exe_dir.join("cryptex-desktop");

    if !desktop_binary.exists() {
        eprintln!("❌ Error: Desktop application binary not found: {}", desktop_binary.display());
        eprintln!("Expected location: {}", desktop_binary.display());
        eprintln!("\nPlease build the desktop application:");
        eprintln!("  cd cryptex-desktop && npm run tauri build");
        return Err("Desktop application binary not found".into());
    }

    let mut cmd = Command::new(&desktop_binary);

    if let Some(config_path) = config {
        cmd.env("CRYPTEX_CONFIG", config_path);
    }

    // Execute desktop app
    let status = cmd
        .stdin(Stdio::inherit())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .status()?;

    if !status.success() {
        return Err(format!("Desktop application exited with status: {}", status).into());
    }

    Ok(())
}
