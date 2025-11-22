//! # The Commune - MCP Server for CRYPTEX
//!
//! Model Context Protocol (MCP) server that exposes CRYPTEX functionality
//! as tools for integration with PYRO_Platform_Ignition and other AI systems.
//!
//! Traditional name: `MCPServer` or `RPCServer`

use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use std::io::{self, BufRead, Write};
use the_assessor::TheAssessor;
use the_collective::TheCollective;
use the_coordinator::TheCharter;
use the_infiltrator::TheInfiltrator;
use the_propagandist::ThePropagandist;
use tracing_subscriber;

/// JSON-RPC 2.0 Request
#[derive(Debug, Deserialize)]
struct JsonRpcRequest {
    jsonrpc: String,
    id: Option<Value>,
    method: String,
    params: Option<Value>,
}

/// JSON-RPC 2.0 Response
#[derive(Debug, Serialize)]
struct JsonRpcResponse {
    jsonrpc: String,
    id: Option<Value>,
    #[serde(skip_serializing_if = "Option::is_none")]
    result: Option<Value>,
    #[serde(skip_serializing_if = "Option::is_none")]
    error: Option<JsonRpcError>,
}

/// JSON-RPC 2.0 Error
#[derive(Debug, Serialize)]
struct JsonRpcError {
    code: i32,
    message: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    data: Option<Value>,
}

/// MCP Server
struct MCPServer {
    assessor: TheAssessor,
    infiltrator: TheInfiltrator,
    propagandist: ThePropagandist,
}

impl MCPServer {
    /// Initialize the MCP server with all CRYPTEX components
    async fn new() -> Result<Self, Box<dyn std::error::Error>> {
        tracing::info!("Initializing CRYPTEX MCP Server...");

        let assessor = TheAssessor::the_awakening().await?;
        let infiltrator = TheInfiltrator::the_awakening().await?;
        let propagandist = ThePropagandist::the_awakening().await?;

        tracing::info!("CRYPTEX MCP Server initialized successfully");

        Ok(Self {
            assessor,
            infiltrator,
            propagandist,
        })
    }

    /// Handle JSON-RPC request
    async fn handle_request(&self, request: JsonRpcRequest) -> JsonRpcResponse {
        match request.method.as_str() {
            "initialize" => self.handle_initialize(request.id).await,
            "tools/list" => self.handle_list_tools(request.id).await,
            "tools/call" => self.handle_tool_call(request.id, request.params).await,
            _ => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id: request.id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32601,
                    message: "Method not found".to_string(),
                    data: None,
                }),
            },
        }
    }

    /// Handle MCP initialize request
    async fn handle_initialize(&self, id: Option<Value>) -> JsonRpcResponse {
        JsonRpcResponse {
            jsonrpc: "2.0".to_string(),
            id,
            result: Some(json!({
                "protocolVersion": "2024-11-05",
                "serverInfo": {
                    "name": "cryptex-mcp-server",
                    "version": "1.0.0"
                },
                "capabilities": {
                    "tools": {
                        "listChanged": false
                    }
                }
            })),
            error: None,
        }
    }

    /// Handle tools/list request
    async fn handle_list_tools(&self, id: Option<Value>) -> JsonRpcResponse {
        let tools = vec![
            json!({
                "name": "assess_vulnerability",
                "description": "Assess a CVE vulnerability with comprehensive CVSS, KEV, EPSS, and AI-enhanced scoring",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "cve_id": {
                            "type": "string",
                            "description": "CVE identifier (e.g., CVE-2021-44228)"
                        }
                    },
                    "required": ["cve_id"]
                }
            }),
            json!({
                "name": "start_scan",
                "description": "Start a new vulnerability scan on a target",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "target": {
                            "type": "string",
                            "description": "Scan target (IP, CIDR, or hostname)"
                        }
                    },
                    "required": ["target"]
                }
            }),
            json!({
                "name": "end_scan",
                "description": "End a scan and generate comprehensive report",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "scan_id": {
                            "type": "string",
                            "description": "Scan identifier"
                        }
                    },
                    "required": ["scan_id"]
                }
            }),
            json!({
                "name": "generate_report",
                "description": "Generate a vulnerability assessment report in various formats",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "scan_id": {
                            "type": "string",
                            "description": "Scan identifier"
                        },
                        "format": {
                            "type": "string",
                            "enum": ["json", "html", "markdown", "text"],
                            "description": "Report format"
                        }
                    },
                    "required": ["scan_id", "format"]
                }
            }),
            json!({
                "name": "get_executive_summary",
                "description": "Generate executive-level vulnerability summary",
                "inputSchema": {
                    "type": "object",
                    "properties": {
                        "scan_id": {
                            "type": "string",
                            "description": "Scan identifier"
                        }
                    },
                    "required": ["scan_id"]
                }
            }),
        ];

        JsonRpcResponse {
            jsonrpc: "2.0".to_string(),
            id,
            result: Some(json!({ "tools": tools })),
            error: None,
        }
    }

    /// Handle tools/call request
    async fn handle_tool_call(&self, id: Option<Value>, params: Option<Value>) -> JsonRpcResponse {
        let params = match params {
            Some(p) => p,
            None => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Invalid params".to_string(),
                        data: None,
                    }),
                };
            }
        };

        let tool_name = params["name"].as_str().unwrap_or("");
        let arguments = &params["arguments"];

        match tool_name {
            "assess_vulnerability" => self.call_assess_vulnerability(id, arguments).await,
            "start_scan" => self.call_start_scan(id, arguments).await,
            "end_scan" => self.call_end_scan(id, arguments).await,
            "generate_report" => self.call_generate_report(id, arguments).await,
            "get_executive_summary" => self.call_executive_summary(id, arguments).await,
            _ => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32602,
                    message: format!("Unknown tool: {}", tool_name),
                    data: None,
                }),
            },
        }
    }

    /// Call assess_vulnerability tool
    async fn call_assess_vulnerability(
        &self,
        id: Option<Value>,
        args: &Value,
    ) -> JsonRpcResponse {
        let cve_id = match args["cve_id"].as_str() {
            Some(id) => id,
            None => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Missing cve_id parameter".to_string(),
                        data: None,
                    }),
                };
            }
        };

        match self.assessor.assess_vulnerability(cve_id).await {
            Ok(score) => {
                let result = json!({
                    "cve_id": score.cve_id,
                    "cvss_base_score": score.cvss_base_score(),
                    "severity": score.severity().as_str(),
                    "is_kev": score.is_kev(),
                    "composite_risk_score": score.composite_risk_score(),
                    "ai_risk_score": score.ai_risk_score,
                    "ai_priority": score.ai_priority,
                    "remediation_urgency": score.ai_remediation_urgency,
                });

                JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: Some(json!({ "content": [{ "type": "text", "text": serde_json::to_string_pretty(&result).unwrap() }] })),
                    error: None,
                }
            }
            Err(e) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32000,
                    message: format!("Assessment failed: {}", e),
                    data: None,
                }),
            },
        }
    }

    /// Call start_scan tool
    async fn call_start_scan(&self, id: Option<Value>, args: &Value) -> JsonRpcResponse {
        let target = match args["target"].as_str() {
            Some(t) => t,
            None => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Missing target parameter".to_string(),
                        data: None,
                    }),
                };
            }
        };

        match self.infiltrator.start_scan(target).await {
            Ok(scan_id) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id,
                result: Some(json!({ "content": [{ "type": "text", "text": format!("Scan started: {}", scan_id) }] })),
                error: None,
            },
            Err(e) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32000,
                    message: format!("Scan start failed: {}", e),
                    data: None,
                }),
            },
        }
    }

    /// Call end_scan tool
    async fn call_end_scan(&self, id: Option<Value>, args: &Value) -> JsonRpcResponse {
        let scan_id = match args["scan_id"].as_str() {
            Some(s) => s,
            None => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Missing scan_id parameter".to_string(),
                        data: None,
                    }),
                };
            }
        };

        match self.infiltrator.end_scan(scan_id).await {
            Ok(report) => {
                let summary = format!(
                    "Scan {} completed\nVulnerabilities: {}\nCritical: {}\nHigh: {}\nMedium: {}\nLow: {}\nKEV: {}",
                    report.scan_id,
                    report.total_vulnerabilities,
                    report.critical_count,
                    report.high_count,
                    report.medium_count,
                    report.low_count,
                    report.kev_count
                );

                JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: Some(json!({ "content": [{ "type": "text", "text": summary }] })),
                    error: None,
                }
            }
            Err(e) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32000,
                    message: format!("Scan end failed: {}", e),
                    data: None,
                }),
            },
        }
    }

    /// Call generate_report tool
    async fn call_generate_report(&self, id: Option<Value>, args: &Value) -> JsonRpcResponse {
        let scan_id = match args["scan_id"].as_str() {
            Some(s) => s,
            None => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Missing scan_id parameter".to_string(),
                        data: None,
                    }),
                };
            }
        };

        let format = match args["format"].as_str() {
            Some("json") => the_propagandist::ReportFormat::Json,
            Some("html") => the_propagandist::ReportFormat::Html,
            Some("markdown") => the_propagandist::ReportFormat::Markdown,
            Some("text") => the_propagandist::ReportFormat::Text,
            _ => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Invalid format parameter".to_string(),
                        data: None,
                    }),
                };
            }
        };

        // Get scan report first
        match self.infiltrator.get_scan_context(scan_id).await {
            Ok(scan_report) => {
                match self.propagandist.generate_report(&scan_report, format).await {
                    Ok(report) => JsonRpcResponse {
                        jsonrpc: "2.0".to_string(),
                        id,
                        result: Some(json!({ "content": [{ "type": "text", "text": report }] })),
                        error: None,
                    },
                    Err(e) => JsonRpcResponse {
                        jsonrpc: "2.0".to_string(),
                        id,
                        result: None,
                        error: Some(JsonRpcError {
                            code: -32000,
                            message: format!("Report generation failed: {}", e),
                            data: None,
                        }),
                    },
                }
            }
            Err(e) => JsonRpcResponse {
                jsonrpc: "2.0".to_string(),
                id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32000,
                    message: format!("Scan not found: {}", e),
                    data: None,
                }),
            },
        }
    }

    /// Call get_executive_summary tool
    async fn call_executive_summary(&self, id: Option<Value>, args: &Value) -> JsonRpcResponse {
        let scan_id = match args["scan_id"].as_str() {
            Some(s) => s,
            None => {
                return JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32602,
                        message: "Missing scan_id parameter".to_string(),
                        data: None,
                    }),
                };
            }
        };

        match self.infiltrator.get_scan_context(scan_id).await {
            Ok(scan_report) => {
                match self.propagandist.generate_executive_summary(&scan_report).await {
                    Ok(summary) => JsonRpcResponse {
                        jsonrpc: "2.0".to_string(),
                        id,
                        result: Some(json!({ "content": [{ "type": "text", "text": summary }] })),
                        error: None,
                    },
                    Err(e) => JsonRpcResponse {
                        jsonrpc: "2.0".to_string(),
                        id,
                        result: None,
                        error: Some(JsonRpcError {
                            code: -32000,
                            message: format!("Summary generation failed: {}", e),
                            data: None,
                        }),
                    },
                }
            }
            Err(e) => JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                id,
                result: None,
                error: Some(JsonRpcError {
                    code: -32000,
                    message: format!("Scan not found: {}", e),
                    data: None,
                }),
            },
        }
    }
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Initialize logging
    tracing_subscriber::fmt::init();

    tracing::info!("Starting CRYPTEX MCP Server for PYRO integration...");

    // Initialize MCP server
    let server = MCPServer::new().await?;

    tracing::info!("CRYPTEX MCP Server ready - listening on stdin/stdout");
    eprintln!("CRYPTEX MCP Server initialized - ready for PYRO integration");

    // Process JSON-RPC requests from stdin
    let stdin = io::stdin();
    let mut stdout = io::stdout();

    for line in stdin.lock().lines() {
        let line = line?;

        // Parse JSON-RPC request
        let request: JsonRpcRequest = match serde_json::from_str(&line) {
            Ok(req) => req,
            Err(e) => {
                let error_response = JsonRpcResponse {
                    jsonrpc: "2.0".to_string(),
                    id: None,
                    result: None,
                    error: Some(JsonRpcError {
                        code: -32700,
                        message: format!("Parse error: {}", e),
                        data: None,
                    }),
                };

                let response_json = serde_json::to_string(&error_response)?;
                writeln!(stdout, "{}", response_json)?;
                stdout.flush()?;
                continue;
            }
        };

        // Handle request
        let response = server.handle_request(request).await;

        // Send response
        let response_json = serde_json::to_string(&response)?;
        writeln!(stdout, "{}", response_json)?;
        stdout.flush()?;
    }

    Ok(())
}
