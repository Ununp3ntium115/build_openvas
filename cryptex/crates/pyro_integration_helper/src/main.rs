#!/usr/bin/env rust
//! PYRO Integration Helper MCP Server
//!
//! This MCP server provides tools to help understand and integrate
//! openFireVulnerability/CRYPTEX with the PYRO Platform Ignition framework.

use anyhow::{Context, Result};
use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use std::collections::HashMap;
use std::fs;
use std::io::{self, BufRead, Write};
use std::path::{Path, PathBuf};
use std::process::{Command, Stdio};
use walkdir::WalkDir;

/// JSON-RPC request structure
#[derive(Debug, Deserialize)]
struct JsonRpcRequest {
    jsonrpc: String,
    id: Value,
    method: String,
    params: Option<Value>,
}

/// JSON-RPC response structure
#[derive(Debug, Serialize)]
struct JsonRpcResponse {
    jsonrpc: String,
    id: Value,
    #[serde(skip_serializing_if = "Option::is_none")]
    result: Option<Value>,
    #[serde(skip_serializing_if = "Option::is_none")]
    error: Option<JsonRpcError>,
}

/// JSON-RPC error structure
#[derive(Debug, Serialize)]
struct JsonRpcError {
    code: i32,
    message: String,
}

/// PYRO Integration Helper
struct PyroIntegrationHelper {
    workspace_root: PathBuf,
}

impl PyroIntegrationHelper {
    /// Create a new integration helper
    fn new() -> Result<Self> {
        // Try to find workspace root
        let current_dir = std::env::current_dir()?;
        let workspace_root = Self::find_workspace_root(&current_dir)
            .unwrap_or(current_dir);

        Ok(Self { workspace_root })
    }

    /// Find the workspace root by looking for Cargo.toml with [workspace]
    fn find_workspace_root(start: &Path) -> Option<PathBuf> {
        let mut current = start;
        loop {
            let cargo_toml = current.join("Cargo.toml");
            if cargo_toml.exists() {
                if let Ok(content) = fs::read_to_string(&cargo_toml) {
                    if content.contains("[workspace]") {
                        return Some(current.to_path_buf());
                    }
                }
            }
            current = current.parent()?;
        }
    }

    /// Handle MCP initialize request
    fn handle_initialize(&self, _params: Option<Value>) -> Result<Value> {
        Ok(json!({
            "protocolVersion": "2024-11-05",
            "serverInfo": {
                "name": "pyro-integration-helper",
                "version": "1.0.0"
            },
            "capabilities": {
                "tools": {
                    "listChanged": false
                }
            }
        }))
    }

    /// Handle tools/list request
    fn handle_list_tools(&self) -> Result<Value> {
        Ok(json!({
            "tools": [
                {
                    "name": "analyze_codebase",
                    "description": "Analyze the openFireVulnerability/CRYPTEX codebase structure",
                    "inputSchema": {
                        "type": "object",
                        "properties": {
                            "path": {
                                "type": "string",
                                "description": "Optional relative path to analyze (defaults to entire workspace)"
                            }
                        }
                    }
                },
                {
                    "name": "read_integration_docs",
                    "description": "Read and summarize PYRO integration documentation",
                    "inputSchema": {
                        "type": "object",
                        "properties": {
                            "doc_type": {
                                "type": "string",
                                "enum": ["pyro_integration", "mcp_integration", "api_reference", "stack_architecture"],
                                "description": "Type of documentation to read"
                            }
                        },
                        "required": ["doc_type"]
                    }
                },
                {
                    "name": "generate_pyro_config",
                    "description": "Generate PYRO Platform configuration files",
                    "inputSchema": {
                        "type": "object",
                        "properties": {
                            "pyro_path": {
                                "type": "string",
                                "description": "Path to PYRO Platform installation"
                            },
                            "mcp_server_path": {
                                "type": "string",
                                "description": "Path to cryptex-mcp-server binary (optional)"
                            }
                        }
                    }
                },
                {
                    "name": "test_mcp_connection",
                    "description": "Test the CRYPTEX MCP server connection",
                    "inputSchema": {
                        "type": "object",
                        "properties": {
                            "mcp_server_path": {
                                "type": "string",
                                "description": "Path to cryptex-mcp-server binary (optional)"
                            }
                        }
                    }
                },
                {
                    "name": "validate_integration",
                    "description": "Validate the PYRO integration setup",
                    "inputSchema": {
                        "type": "object",
                        "properties": {}
                    }
                },
                {
                    "name": "get_api_endpoints",
                    "description": "List all available CRYPTEX API endpoints",
                    "inputSchema": {
                        "type": "object",
                        "properties": {}
                    }
                },
                {
                    "name": "check_dependencies",
                    "description": "Check if all PYRO integration dependencies are met",
                    "inputSchema": {
                        "type": "object",
                        "properties": {}
                    }
                },
                {
                    "name": "get_mcp_tools",
                    "description": "List all available MCP tools from CRYPTEX MCP server",
                    "inputSchema": {
                        "type": "object",
                        "properties": {}
                    }
                }
            ]
        }))
    }

    /// Handle tools/call request
    fn handle_tool_call(&self, params: Value) -> Result<Value> {
        let tool_name = params["name"].as_str()
            .ok_or_else(|| anyhow::anyhow!("Tool name is required"))?;

        let empty_map = serde_json::Map::new();
        let args = params.get("arguments")
            .and_then(|v| v.as_object())
            .unwrap_or(&empty_map);

        let result = match tool_name {
            "analyze_codebase" => self.analyze_codebase(args)?,
            "read_integration_docs" => self.read_integration_docs(args)?,
            "generate_pyro_config" => self.generate_pyro_config(args)?,
            "test_mcp_connection" => self.test_mcp_connection(args)?,
            "validate_integration" => self.validate_integration()?,
            "get_api_endpoints" => self.get_api_endpoints()?,
            "check_dependencies" => self.check_dependencies()?,
            "get_mcp_tools" => self.get_mcp_tools(args)?,
            _ => return Err(anyhow::anyhow!("Unknown tool: {}", tool_name)),
        };

        Ok(json!({
            "content": [{
                "type": "text",
                "text": serde_json::to_string_pretty(&result)?
            }]
        }))
    }

    /// Analyze codebase structure
    fn analyze_codebase(&self, args: &serde_json::Map<String, Value>) -> Result<Value> {
        let rel_path = args.get("path")
            .and_then(|v| v.as_str())
            .unwrap_or("");

        let analyze_path = if rel_path.is_empty() {
            self.workspace_root.clone()
        } else {
            self.workspace_root.join(rel_path)
        };

        let mut file_count = 0;
        let mut dir_count = 0;

        // Walk the directory tree
        for entry in WalkDir::new(&analyze_path)
            .max_depth(4)
            .into_iter()
            .filter_map(|e| e.ok())
        {
            if entry.file_type().is_dir() {
                dir_count += 1;
            } else {
                file_count += 1;
            }
        }

        // Find key directories
        let key_dirs = vec![
            "cryptex/crates",
            "cryptex-desktop",
            "node-red-cryptex",
            "cryptex-visualizer",
        ];

        let mut found_dirs = HashMap::new();
        for key_dir in key_dirs {
            let path = self.workspace_root.join(key_dir);
            found_dirs.insert(key_dir, path.exists());
        }

        // Find important files
        let important_files = vec![
            "mcp_servers.json",
            "cryptex/openfire-config.toml",
            "PYRO_MCP_INTEGRATION.md",
            "cryptex/PYRO_INTEGRATION.md",
            "cryptex/API_REFERENCE.md",
            "cryptex-desktop/STACK_ARCHITECTURE.md",
        ];

        let mut found_files = HashMap::new();
        for file in important_files {
            let path = self.workspace_root.join(file);
            found_files.insert(file, path.exists());
        }

        Ok(json!({
            "workspace_root": self.workspace_root,
            "analyzed_path": analyze_path,
            "statistics": {
                "total_directories": dir_count,
                "total_files": file_count
            },
            "key_directories": found_dirs,
            "important_files": found_files,
            "integration_status": "ready"
        }))
    }

    /// Read integration documentation
    fn read_integration_docs(&self, args: &serde_json::Map<String, Value>) -> Result<Value> {
        let doc_type = args.get("doc_type")
            .and_then(|v| v.as_str())
            .ok_or_else(|| anyhow::anyhow!("doc_type is required"))?;

        let doc_path = match doc_type {
            "pyro_integration" => "cryptex/PYRO_INTEGRATION.md",
            "mcp_integration" => "PYRO_MCP_INTEGRATION.md",
            "api_reference" => "cryptex/API_REFERENCE.md",
            "stack_architecture" => "cryptex-desktop/STACK_ARCHITECTURE.md",
            _ => return Err(anyhow::anyhow!("Unknown doc_type: {}", doc_type)),
        };

        let full_path = self.workspace_root.join(doc_path);
        let content = fs::read_to_string(&full_path)
            .context(format!("Failed to read documentation: {}", doc_path))?;

        // Extract key sections
        let lines: Vec<&str> = content.lines().collect();
        let summary = lines.iter()
            .take(50)
            .cloned()
            .collect::<Vec<&str>>()
            .join("\n");

        Ok(json!({
            "doc_type": doc_type,
            "path": full_path,
            "size_bytes": content.len(),
            "line_count": lines.len(),
            "summary": summary,
            "full_content": content
        }))
    }

    /// Generate PYRO configuration
    fn generate_pyro_config(&self, args: &serde_json::Map<String, Value>) -> Result<Value> {
        let pyro_path = args.get("pyro_path")
            .and_then(|v| v.as_str());

        let mcp_server_path = args.get("mcp_server_path")
            .and_then(|v| v.as_str())
            .unwrap_or("/home/user/build_openvas/cryptex/target/release/cryptex-mcp-server");

        // Generate mcp_servers.json configuration
        let mcp_config = json!({
            "mcpServers": {
                "openfire": {
                    "command": mcp_server_path,
                    "args": [],
                    "env": {
                        "RUST_LOG": "info",
                        "CRYPTEX_DB_PATH": "/home/user/.local/share/openFireVulnerability/openfire.redb",
                        "CRYPTEX_CONFIG": "/home/user/build_openvas/cryptex/openfire-config.toml"
                    },
                    "description": "openFireVulnerability Security Assessment Platform - redb-based vulnerability scanner",
                    "metadata": {
                        "version": "1.0.0",
                        "stack": ["Rust", "redb", "Node-RED", "Svelte"],
                        "database": "redb (embedded ACID database)"
                    }
                }
            }
        });

        let mut output = HashMap::new();
        output.insert("mcp_servers_json", serde_json::to_string_pretty(&mcp_config)?);

        // If PYRO path provided, suggest where to place it
        if let Some(pyro_path) = pyro_path {
            let suggested_path = format!("{}/config/mcp_servers.json", pyro_path);
            output.insert("suggested_path", suggested_path);
        }

        Ok(json!(output))
    }

    /// Test MCP connection
    fn test_mcp_connection(&self, args: &serde_json::Map<String, Value>) -> Result<Value> {
        let mcp_server_path = args.get("mcp_server_path")
            .and_then(|v| v.as_str())
            .unwrap_or("/home/user/build_openvas/cryptex/target/release/cryptex-mcp-server");

        let mcp_path = Path::new(mcp_server_path);

        if !mcp_path.exists() {
            return Ok(json!({
                "status": "error",
                "message": format!("MCP server not found at: {}", mcp_server_path),
                "suggestion": "Run: cd /home/user/build_openvas/cryptex && cargo build --release -p the_commune"
            }));
        }

        // Test initialize
        let init_request = json!({
            "jsonrpc": "2.0",
            "id": 1,
            "method": "initialize",
            "params": {
                "protocolVersion": "2024-11-05",
                "capabilities": {},
                "clientInfo": {"name": "pyro-helper", "version": "1.0"}
            }
        });

        let output = Command::new(mcp_server_path)
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()
            .and_then(|mut child| {
                if let Some(mut stdin) = child.stdin.take() {
                    let request_str = serde_json::to_string(&init_request)? + "\n";
                    stdin.write_all(request_str.as_bytes())?;
                }
                child.wait_with_output()
            });

        match output {
            Ok(output) => {
                let stdout = String::from_utf8_lossy(&output.stdout);
                let stderr = String::from_utf8_lossy(&output.stderr);

                Ok(json!({
                    "status": "success",
                    "mcp_server_path": mcp_server_path,
                    "response": stdout,
                    "stderr": stderr
                }))
            }
            Err(e) => Ok(json!({
                "status": "error",
                "message": format!("Failed to execute MCP server: {}", e)
            }))
        }
    }

    /// Validate integration
    fn validate_integration(&self) -> Result<Value> {
        let mut checks = Vec::new();

        // Check 1: MCP server binary exists
        let mcp_server = self.workspace_root.join("cryptex/target/release/cryptex-mcp-server");
        checks.push(json!({
            "check": "MCP Server Binary",
            "status": mcp_server.exists(),
            "path": mcp_server,
            "message": if mcp_server.exists() { "✅ Found" } else { "❌ Not built - run: cargo build --release -p the_commune" }
        }));

        // Check 2: Configuration files exist
        let config_file = self.workspace_root.join("mcp_servers.json");
        checks.push(json!({
            "check": "MCP Configuration",
            "status": config_file.exists(),
            "path": config_file,
            "message": if config_file.exists() { "✅ Found" } else { "❌ Missing mcp_servers.json" }
        }));

        // Check 3: Integration docs exist
        let pyro_docs = self.workspace_root.join("cryptex/PYRO_INTEGRATION.md");
        checks.push(json!({
            "check": "PYRO Integration Docs",
            "status": pyro_docs.exists(),
            "path": pyro_docs,
            "message": if pyro_docs.exists() { "✅ Found" } else { "❌ Missing PYRO_INTEGRATION.md" }
        }));

        // Check 4: Desktop app built
        let desktop_dist = self.workspace_root.join("cryptex-desktop/dist");
        checks.push(json!({
            "check": "Desktop UI Built",
            "status": desktop_dist.exists(),
            "path": desktop_dist,
            "message": if desktop_dist.exists() { "✅ Built" } else { "⚠️  Not built - run: cd cryptex-desktop && npm run build" }
        }));

        let all_passed = checks.iter().all(|check| check["status"].as_bool().unwrap_or(false));

        Ok(json!({
            "overall_status": if all_passed { "ready" } else { "incomplete" },
            "checks": checks,
            "summary": format!("{}/{} checks passed",
                checks.iter().filter(|c| c["status"].as_bool().unwrap_or(false)).count(),
                checks.len()
            )
        }))
    }

    /// Get API endpoints
    fn get_api_endpoints(&self) -> Result<Value> {
        let api_doc_path = self.workspace_root.join("cryptex/API_REFERENCE.md");

        if !api_doc_path.exists() {
            return Ok(json!({
                "error": "API_REFERENCE.md not found",
                "path": api_doc_path
            }));
        }

        let content = fs::read_to_string(&api_doc_path)?;

        // Extract endpoint patterns
        let endpoint_regex = regex::Regex::new(r"(GET|POST|PUT|DELETE|PATCH)\s+([/\w\-:]+)")?;
        let mut endpoints = Vec::new();

        for cap in endpoint_regex.captures_iter(&content) {
            endpoints.push(json!({
                "method": &cap[1],
                "path": &cap[2]
            }));
        }

        Ok(json!({
            "total_endpoints": endpoints.len(),
            "endpoints": endpoints,
            "documentation": api_doc_path
        }))
    }

    /// Check dependencies
    fn check_dependencies(&self) -> Result<Value> {
        let mut deps = Vec::new();

        // Check Rust
        let rust_version = Command::new("rustc")
            .arg("--version")
            .output()
            .ok()
            .and_then(|o| String::from_utf8(o.stdout).ok());

        deps.push(json!({
            "name": "Rust",
            "required": "1.70+",
            "installed": rust_version.is_some(),
            "version": rust_version.unwrap_or_default().trim()
        }));

        // Check Cargo
        let cargo_version = Command::new("cargo")
            .arg("--version")
            .output()
            .ok()
            .and_then(|o| String::from_utf8(o.stdout).ok());

        deps.push(json!({
            "name": "Cargo",
            "required": "latest",
            "installed": cargo_version.is_some(),
            "version": cargo_version.unwrap_or_default().trim()
        }));

        // Check Node.js (for desktop app)
        let node_version = Command::new("node")
            .arg("--version")
            .output()
            .ok()
            .and_then(|o| String::from_utf8(o.stdout).ok());

        deps.push(json!({
            "name": "Node.js",
            "required": "18+",
            "installed": node_version.is_some(),
            "version": node_version.unwrap_or_default().trim()
        }));

        // Check npm
        let npm_version = Command::new("npm")
            .arg("--version")
            .output()
            .ok()
            .and_then(|o| String::from_utf8(o.stdout).ok());

        deps.push(json!({
            "name": "npm",
            "required": "9+",
            "installed": npm_version.is_some(),
            "version": npm_version.unwrap_or_default().trim()
        }));

        let all_installed = deps.iter().all(|d| d["installed"].as_bool().unwrap_or(false));

        Ok(json!({
            "status": if all_installed { "all_dependencies_met" } else { "missing_dependencies" },
            "dependencies": deps
        }))
    }

    /// Get MCP tools from CRYPTEX server
    fn get_mcp_tools(&self, args: &serde_json::Map<String, Value>) -> Result<Value> {
        let mcp_server_path = args.get("mcp_server_path")
            .and_then(|v| v.as_str())
            .unwrap_or("/home/user/build_openvas/cryptex/target/release/cryptex-mcp-server");

        let tools_request = json!({
            "jsonrpc": "2.0",
            "id": 2,
            "method": "tools/list",
            "params": {}
        });

        let output = Command::new(mcp_server_path)
            .stdin(Stdio::piped())
            .stdout(Stdio::piped())
            .stderr(Stdio::piped())
            .spawn()
            .and_then(|mut child| {
                if let Some(mut stdin) = child.stdin.take() {
                    let request_str = serde_json::to_string(&tools_request)? + "\n";
                    stdin.write_all(request_str.as_bytes())?;
                }
                child.wait_with_output()
            });

        match output {
            Ok(output) => {
                let stdout = String::from_utf8_lossy(&output.stdout);

                // Parse the JSON-RPC response
                if let Some(last_line) = stdout.lines().last() {
                    if let Ok(response) = serde_json::from_str::<Value>(last_line) {
                        return Ok(json!({
                            "status": "success",
                            "tools": response["result"]["tools"],
                            "raw_response": response
                        }));
                    }
                }

                Ok(json!({
                    "status": "error",
                    "message": "Failed to parse MCP response",
                    "raw_output": stdout
                }))
            }
            Err(e) => Ok(json!({
                "status": "error",
                "message": format!("Failed to execute MCP server: {}", e)
            }))
        }
    }
}

fn main() -> Result<()> {
    eprintln!("PYRO Integration Helper MCP Server initialized");

    let helper = PyroIntegrationHelper::new()?;
    let stdin = io::stdin();
    let mut stdout = io::stdout();

    for line in stdin.lock().lines() {
        let line = line?;
        let request: JsonRpcRequest = serde_json::from_str(&line)
            .context("Failed to parse JSON-RPC request")?;

        let result = match request.method.as_str() {
            "initialize" => helper.handle_initialize(request.params),
            "tools/list" => helper.handle_list_tools(),
            "tools/call" => {
                let params = request.params.ok_or_else(|| anyhow::anyhow!("Params required"))?;
                helper.handle_tool_call(params)
            }
            _ => Err(anyhow::anyhow!("Unknown method: {}", request.method)),
        };

        let response = match result {
            Ok(result) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id: request.id,
                result: Some(result),
                error: None,
            },
            Err(e) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id: request.id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32603,
                    message: e.to_string(),
                }),
            },
        };

        let response_json = serde_json::to_string(&response)?;
        writeln!(stdout, "{}", response_json)?;
        stdout.flush()?;
    }

    Ok(())
}
