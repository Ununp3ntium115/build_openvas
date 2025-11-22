# MCP SERVER SPECIFICATION
**Project**: CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience
**Protocol**: Model Context Protocol (MCP)
**Transport**: JSON-RPC 2.0 over stdio/HTTP/WebSocket

## Overview

The **CRYPTEX MCP Server** provides a Model Context Protocol interface for AI models to interact with the CRYPTEX security assessment platform. It exposes the Cryptex Dictionary, vulnerability scanning capabilities, AI analysis functions, and reporting tools through a standardized protocol.

## MCP Server Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     AI Model (Claude/GPT-4)                  │
└────────────────────────┬────────────────────────────────────┘
                         │ JSON-RPC 2.0
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   CRYPTEX MCP Server (Rust)                  │
│  ┌──────────────────────────────────────────────────────┐   │
│  │              The Commune (Core Server)                │   │
│  │  • Protocol Handler                                   │   │
│  │  • Tool Registry                                      │   │
│  │  • Session Management                                 │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │           The Cryptex Dictionary Module               │   │
│  │  • cryptex() - Translate names                        │   │
│  │  • uncryptex() - Reverse translate                    │   │
│  │  • get_philosophical_meaning()                        │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │            The Consciousness (AI Service)             │   │
│  │  • Vulnerability Analysis                             │   │
│  │  • Threat Modeling                                    │   │
│  │  • Report Generation                                  │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │          The Infiltrator (Scanner Bridge)             │   │
│  │  • Execute Scans                                      │   │
│  │  • Retrieve Results                                   │   │
│  │  • Monitor Progress                                   │   │
│  └──────────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────────┐   │
│  │            The Archive (Database Layer)               │   │
│  │  • Store Scan Results                                 │   │
│  │  • Query Vulnerabilities                              │   │
│  │  • Historical Analysis                                │   │
│  └──────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                  External Services & Data                    │
│  • OpenVAS Scanner                                           │
│  • AI Providers (OpenAI/Claude)                              │
│  • Vulnerability Databases (NVD, CVE)                        │
│  • Node-RED Workflows                                        │
└─────────────────────────────────────────────────────────────┘
```

---

## Protocol Specification

### **Transport Layers**

The CRYPTEX MCP Server supports three transport mechanisms:

1. **stdio** (Standard Input/Output)
   - Default for local development
   - Low latency, direct communication
   - Used by Claude Code, Cursor, etc.

2. **HTTP** (RESTful API)
   - Remote access over network
   - Stateless request/response
   - Used by web applications

3. **WebSocket** (Real-time bidirectional)
   - Live updates and streaming
   - Server-sent events
   - Used for real-time monitoring

### **Message Format**

All messages follow JSON-RPC 2.0 specification:

```json
{
  "jsonrpc": "2.0",
  "method": "tool_name",
  "params": {
    "arg1": "value1",
    "arg2": "value2"
  },
  "id": 1
}
```

**Response:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "data": "response_data"
  },
  "id": 1
}
```

**Error Response:**
```json
{
  "jsonrpc": "2.0",
  "error": {
    "code": -32600,
    "message": "Invalid Request",
    "data": "Additional error information"
  },
  "id": 1
}
```

---

## MCP Tools (Exposed Functions)

### **1. Cryptex Dictionary Tools**

#### **`cryptex_translate`**
Translate traditional names to anarchist terminology.

**Pseudocode:**
```
FUNCTION cryptex_translate(traditional_name: String) -> String:
    IF traditional_name in CRYPTEX_DICTIONARY:
        RETURN CRYPTEX_DICTIONARY[traditional_name]
    ELSE:
        RETURN traditional_name
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "cryptex_translate",
  "description": "Translate traditional software terminology to anarchist naming conventions",
  "parameters": {
    "type": "object",
    "properties": {
      "traditional_name": {
        "type": "string",
        "description": "Traditional software term (e.g., 'server', 'database', 'manager')"
      }
    },
    "required": ["traditional_name"]
  }
}
```

**Example Request:**
```json
{
  "jsonrpc": "2.0",
  "method": "cryptex_translate",
  "params": {
    "traditional_name": "server"
  },
  "id": 1
}
```

**Example Response:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "anarchist_name": "the_commune",
    "philosophical_meaning": "A shared space serving the collective",
    "category": "Core Architecture"
  },
  "id": 1
}
```

---

#### **`cryptex_reverse`**
Translate anarchist names back to traditional terminology.

**Pseudocode:**
```
FUNCTION cryptex_reverse(anarchist_name: String) -> String:
    IF anarchist_name in REVERSE_CRYPTEX:
        RETURN REVERSE_CRYPTEX[anarchist_name]
    ELSE:
        RETURN anarchist_name
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "cryptex_reverse",
  "description": "Translate anarchist terminology back to traditional naming",
  "parameters": {
    "type": "object",
    "properties": {
      "anarchist_name": {
        "type": "string",
        "description": "Anarchist term (e.g., 'the_commune', 'the_collective')"
      }
    },
    "required": ["anarchist_name"]
  }
}
```

---

#### **`cryptex_search`**
Search the Cryptex dictionary by keyword or category.

**Pseudocode:**
```
FUNCTION cryptex_search(query: String, category: Optional<String>) -> List<Mapping>:
    results = []
    FOR each (traditional, anarchist) in CRYPTEX_DICTIONARY:
        IF query matches traditional OR query matches anarchist:
            IF category is None OR mapping.category == category:
                results.append(mapping)
    RETURN results
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "cryptex_search",
  "description": "Search the Cryptex dictionary for mappings",
  "parameters": {
    "type": "object",
    "properties": {
      "query": {
        "type": "string",
        "description": "Search term (partial matching supported)"
      },
      "category": {
        "type": "string",
        "enum": ["core_architecture", "security", "ai_intelligence", "data_processing", "reporting", "system"],
        "description": "Filter by category (optional)"
      }
    },
    "required": ["query"]
  }
}
```

---

### **2. Vulnerability Scanning Tools**

#### **`the_infiltration`** (Execute Scan)
Execute a vulnerability scan against target systems.

**Pseudocode:**
```
FUNCTION the_infiltration(targets: List<String>, scan_type: String, options: ScanOptions) -> ScanJob:
    // Validate targets
    FOR each target in targets:
        IF NOT is_valid_target(target):
            RAISE InvalidTargetError

    // Create scan job
    job = ScanJob.create(
        targets: targets,
        scan_type: scan_type,
        options: options
    )

    // Queue the scan
    the_pipeline.enqueue(job)

    // Log the action
    the_chronicler.log("Infiltration initiated", job.id)

    RETURN job
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_infiltration",
  "description": "Execute vulnerability scan against target systems (anarchist: scanner)",
  "parameters": {
    "type": "object",
    "properties": {
      "targets": {
        "type": "array",
        "items": {"type": "string"},
        "description": "List of IP addresses, hostnames, or CIDR ranges"
      },
      "scan_type": {
        "type": "string",
        "enum": ["quick", "full", "deep", "custom"],
        "description": "Type of scan to perform"
      },
      "options": {
        "type": "object",
        "properties": {
          "ports": {"type": "string", "description": "Port specification (e.g., '1-1000,8080')"},
          "intensity": {"type": "integer", "minimum": 1, "maximum": 10},
          "timeout": {"type": "integer", "description": "Scan timeout in seconds"}
        }
      }
    },
    "required": ["targets", "scan_type"]
  }
}
```

**Example Request:**
```json
{
  "jsonrpc": "2.0",
  "method": "the_infiltration",
  "params": {
    "targets": ["192.168.1.0/24"],
    "scan_type": "full",
    "options": {
      "intensity": 7,
      "timeout": 3600
    }
  },
  "id": 2
}
```

**Example Response:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "job_id": "inf-20251121-001",
    "status": "queued",
    "targets_count": 256,
    "estimated_duration": "45 minutes",
    "created_at": "2025-11-21T10:30:00Z"
  },
  "id": 2
}
```

---

#### **`the_observation`** (Monitor Scan Progress)
Monitor the progress of ongoing vulnerability scans.

**Pseudocode:**
```
FUNCTION the_observation(job_id: String) -> ScanProgress:
    job = ScanJob.find(job_id)
    IF job is None:
        RAISE JobNotFoundError

    progress = {
        job_id: job.id,
        status: job.status,
        progress_percent: job.calculate_progress(),
        targets_completed: job.completed_targets,
        targets_total: job.total_targets,
        vulnerabilities_found: job.weaknesses_count(),
        estimated_time_remaining: job.estimate_remaining_time()
    }

    RETURN progress
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_observation",
  "description": "Monitor progress of ongoing vulnerability scans (anarchist: monitor)",
  "parameters": {
    "type": "object",
    "properties": {
      "job_id": {
        "type": "string",
        "description": "Scan job identifier"
      }
    },
    "required": ["job_id"]
  }
}
```

---

#### **`the_revelation`** (Retrieve Scan Results)
Retrieve completed scan results and discovered vulnerabilities.

**Pseudocode:**
```
FUNCTION the_revelation(job_id: String, filters: Optional<Filters>) -> ScanResults:
    job = ScanJob.find(job_id)
    IF job is None:
        RAISE JobNotFoundError

    IF job.status != "completed":
        RAISE ScanNotCompleteError

    // Retrieve weaknesses (vulnerabilities)
    weaknesses = the_archive.query_weaknesses(job_id)

    // Apply filters if provided
    IF filters is not None:
        weaknesses = weaknesses.filter(filters)

    // Sort by severity
    weaknesses = weaknesses.sort_by("severity", descending=True)

    results = {
        job_id: job.id,
        scan_summary: job.generate_summary(),
        weaknesses: weaknesses,
        statistics: job.calculate_statistics()
    }

    RETURN results
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_revelation",
  "description": "Retrieve scan results and discovered vulnerabilities (anarchist: decryption/revelation)",
  "parameters": {
    "type": "object",
    "properties": {
      "job_id": {
        "type": "string",
        "description": "Scan job identifier"
      },
      "filters": {
        "type": "object",
        "properties": {
          "severity": {"type": "array", "items": {"type": "string"}},
          "min_cvss": {"type": "number"},
          "categories": {"type": "array", "items": {"type": "string"}}
        }
      }
    },
    "required": ["job_id"]
  }
}
```

---

### **3. AI Analysis Tools**

#### **`the_assessment`** (AI Vulnerability Analysis)
Analyze vulnerabilities using AI consciousness.

**Pseudocode:**
```
FUNCTION the_assessment(weakness: Vulnerability, context: Context) -> Assessment:
    // Prepare the package (payload)
    package = {
        weakness: weakness,
        system_context: context,
        historical_data: the_archive.get_similar_weaknesses(weakness)
    }

    // Consult the consciousness (AI service)
    inquiry = TheInquiry.create(
        task: "vulnerability_analysis",
        package: package
    )

    reply = the_consciousness.process(inquiry).await

    // Parse the assessment
    assessment = {
        risk_level: reply.extract_risk_level(),
        business_impact: reply.extract_business_impact(),
        technical_details: reply.extract_technical_details(),
        remediation_suggestions: reply.extract_remediation(),
        confidence: reply.certainty,
        ai_provider: reply.agitator_used
    }

    // Store in archive
    the_archive.store_assessment(weakness.id, assessment)

    RETURN assessment
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_assessment",
  "description": "AI-powered vulnerability analysis (anarchist: analysis)",
  "parameters": {
    "type": "object",
    "properties": {
      "vulnerability_id": {
        "type": "string",
        "description": "Vulnerability identifier or CVE number"
      },
      "system_context": {
        "type": "object",
        "properties": {
          "system_type": {"type": "string"},
          "criticality": {"type": "string"},
          "data_classification": {"type": "string"}
        }
      },
      "ai_provider": {
        "type": "string",
        "enum": ["openai", "claude", "auto"],
        "default": "auto",
        "description": "Which AI provider to use (anarchist: agitator)"
      }
    },
    "required": ["vulnerability_id"]
  }
}
```

**Example Response:**
```json
{
  "jsonrpc": "2.0",
  "result": {
    "vulnerability_id": "CVE-2023-12345",
    "risk_level": "CRITICAL",
    "risk_score": 9.8,
    "business_impact": {
      "data_breach_risk": "HIGH",
      "service_disruption": "MEDIUM",
      "compliance_impact": ["GDPR", "PCI-DSS"],
      "estimated_cost": "$50K-$500K"
    },
    "technical_analysis": "SQL injection vulnerability allows...",
    "remediation": [
      {
        "priority": 1,
        "action": "Implement parameterized queries",
        "effort": "2-4 hours",
        "effectiveness": "HIGH"
      }
    ],
    "confidence": 0.92,
    "ai_provider": "claude",
    "generated_at": "2025-11-21T10:35:00Z"
  },
  "id": 3
}
```

---

#### **`the_forecast`** (Threat Modeling)
AI-powered threat modeling and attack prediction.

**Pseudocode:**
```
FUNCTION the_forecast(system_architecture: SystemArchitecture, attack_context: Context) -> ThreatModel:
    // Build the inquiry
    package = {
        architecture: system_architecture,
        known_weaknesses: the_archive.get_weaknesses_for_system(system_architecture.id),
        attack_context: attack_context,
        threat_intelligence: fetch_threat_intelligence()
    }

    inquiry = TheInquiry.create(
        task: "threat_modeling",
        package: package
    )

    // Consult consciousness
    reply = the_consciousness.process(inquiry).await

    // Parse threat model
    threat_model = {
        identified_threats: reply.extract_threats(),
        attack_scenarios: reply.extract_scenarios(),
        attack_vectors: reply.extract_vectors(),
        security_controls: reply.suggest_controls(),
        risk_matrix: reply.generate_risk_matrix(),
        confidence: reply.certainty
    }

    RETURN threat_model
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_forecast",
  "description": "AI-powered threat modeling and attack prediction (anarchist: prediction)",
  "parameters": {
    "type": "object",
    "properties": {
      "system_type": {
        "type": "string",
        "description": "Type of system (e.g., 'web_application', 'api', 'network')"
      },
      "architecture": {
        "type": "object",
        "description": "System architecture details"
      },
      "attack_context": {
        "type": "object",
        "properties": {
          "threat_actor": {"type": "string"},
          "objectives": {"type": "array", "items": {"type": "string"}}
        }
      }
    },
    "required": ["system_type"]
  }
}
```

---

### **4. Reporting Tools**

#### **`the_manifesto`** (Generate Report)
Generate comprehensive security reports.

**Pseudocode:**
```
FUNCTION the_manifesto(scan_id: String, report_type: String, options: ReportOptions) -> Report:
    // Gather data
    scan_results = the_archive.get_scan_results(scan_id)
    assessments = the_archive.get_assessments(scan_id)
    threat_models = the_archive.get_threat_models(scan_id)

    // Select framework (template)
    framework = the_framework_selector.select(report_type)

    // Compose report
    report_data = {
        executive_summary: the_compositor.generate_executive_summary(scan_results, assessments),
        technical_findings: scan_results.weaknesses,
        ai_analysis: assessments,
        threat_intelligence: threat_models,
        remediation_roadmap: the_compositor.generate_remediation_plan(assessments)
    }

    // Render using visualizer
    report = the_visualizer.render(framework, report_data, options)

    // Publish
    report_id = the_publisher.publish(report, options.format)

    RETURN {
        report_id: report_id,
        format: options.format,
        download_url: the_publisher.get_url(report_id)
    }
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_manifesto",
  "description": "Generate comprehensive security assessment report (anarchist: report)",
  "parameters": {
    "type": "object",
    "properties": {
      "scan_id": {
        "type": "string",
        "description": "Scan job identifier"
      },
      "report_type": {
        "type": "string",
        "enum": ["executive", "technical", "compliance", "comprehensive"],
        "description": "Type of report to generate"
      },
      "format": {
        "type": "string",
        "enum": ["pdf", "html", "markdown", "json"],
        "default": "pdf"
      },
      "options": {
        "type": "object",
        "properties": {
          "include_ai_analysis": {"type": "boolean", "default": true},
          "include_remediation_plan": {"type": "boolean", "default": true},
          "compliance_frameworks": {"type": "array", "items": {"type": "string"}}
        }
      }
    },
    "required": ["scan_id", "report_type"]
  }
}
```

---

### **5. System Management Tools**

#### **`the_uprising`** (Server Status)
Get server status and health information.

**Pseudocode:**
```
FUNCTION the_uprising() -> ServerStatus:
    status = {
        commune_status: "running",
        uptime: calculate_uptime(),
        version: VERSION,
        components: {
            consciousness: the_consciousness.health_check(),
            infiltrator: the_infiltrator.health_check(),
            archive: the_archive.health_check(),
            agitators: [agitator.health_check() for agitator in the_agitators]
        },
        resources: {
            memory_usage: get_memory_usage(),
            cpu_usage: get_cpu_usage(),
            active_scans: the_pipeline.count_active(),
            queued_scans: the_pipeline.count_queued()
        },
        metrics: the_observer.get_metrics()
    }

    RETURN status
END FUNCTION
```

**MCP Interface:**
```json
{
  "name": "the_uprising",
  "description": "Get server status and health (anarchist: startup/status)",
  "parameters": {
    "type": "object",
    "properties": {}
  }
}
```

---

## Server Implementation (Rust)

### **Project Structure**

```
cryptex-mcp-server/
├── Cargo.toml
├── src/
│   ├── main.rs                 # The Assembly (main entry point)
│   ├── commune.rs              # The Commune (server core)
│   ├── protocol/
│   │   ├── mod.rs
│   │   ├── jsonrpc.rs          # JSON-RPC 2.0 handler
│   │   ├── transport.rs        # Transport layer (stdio/HTTP/WS)
│   │   └── tools.rs            # Tool registry
│   ├── cryptex/
│   │   ├── mod.rs
│   │   ├── dictionary.rs       # Cryptex dictionary
│   │   └── translator.rs       # Translation functions
│   ├── consciousness/          # AI Service
│   │   ├── mod.rs
│   │   ├── service.rs
│   │   ├── agitators/          # AI providers
│   │   │   ├── openai.rs
│   │   │   └── claude.rs
│   │   └── tasks.rs
│   ├── infiltrator/            # Scanner bridge
│   │   ├── mod.rs
│   │   ├── scanner.rs
│   │   └── jobs.rs
│   ├── archive/                # Database layer
│   │   ├── mod.rs
│   │   ├── redb_store.rs       # redb implementation
│   │   └── models.rs
│   ├── compositor/             # Report generation
│   │   ├── mod.rs
│   │   ├── templates.rs
│   │   └── visualizer.rs
│   └── lib.rs
```

### **Core Server Implementation**

```rust
// src/commune.rs

use axum::{Router, routing::post, Json};
use serde_json::{Value, json};
use std::sync::Arc;
use tokio::sync::RwLock;

/// The Commune - Core MCP Server
pub struct TheCommune {
    /// The Cryptex Dictionary
    cryptex: Arc<CryptexDictionary>,

    /// The Consciousness (AI Service)
    consciousness: Arc<TheConsciousness>,

    /// The Infiltrator (Scanner)
    infiltrator: Arc<TheInfiltrator>,

    /// The Archive (Database)
    archive: Arc<TheArchive>,

    /// The Compositor (Report Generator)
    compositor: Arc<TheCompositor>,

    /// Tool Registry
    tools: Arc<RwLock<ToolRegistry>>,
}

impl TheCommune {
    /// The Awakening - Initialize the commune
    pub async fn the_awakening(charter: TheCharter) -> Result<Self, CommuneError> {
        // Initialize components
        let cryptex = Arc::new(CryptexDictionary::load()?);
        let consciousness = Arc::new(TheConsciousness::the_awakening(charter.consciousness).await?);
        let infiltrator = Arc::new(TheInfiltrator::the_awakening(charter.infiltrator).await?);
        let archive = Arc::new(TheArchive::the_awakening(charter.archive).await?);
        let compositor = Arc::new(TheCompositor::the_awakening(charter.compositor).await?);

        // Register tools
        let tools = Arc::new(RwLock::new(ToolRegistry::new()));
        Self::register_tools(&tools).await?;

        Ok(Self {
            cryptex,
            consciousness,
            infiltrator,
            archive,
            compositor,
            tools,
        })
    }

    /// Register all MCP tools
    async fn register_tools(registry: &Arc<RwLock<ToolRegistry>>) -> Result<(), CommuneError> {
        let mut reg = registry.write().await;

        // Cryptex tools
        reg.register("cryptex_translate", Tool::new(cryptex_translate));
        reg.register("cryptex_reverse", Tool::new(cryptex_reverse));
        reg.register("cryptex_search", Tool::new(cryptex_search));

        // Scanning tools
        reg.register("the_infiltration", Tool::new(the_infiltration));
        reg.register("the_observation", Tool::new(the_observation));
        reg.register("the_revelation", Tool::new(the_revelation));

        // AI tools
        reg.register("the_assessment", Tool::new(the_assessment));
        reg.register("the_forecast", Tool::new(the_forecast));

        // Reporting tools
        reg.register("the_manifesto", Tool::new(the_manifesto));

        // System tools
        reg.register("the_uprising", Tool::new(the_uprising));

        Ok(())
    }

    /// Process JSON-RPC request
    pub async fn process_inquiry(&self, inquiry: JsonRpcRequest) -> JsonRpcResponse {
        // Look up tool
        let tools = self.tools.read().await;
        let tool = match tools.get(&inquiry.method) {
            Some(t) => t,
            None => return JsonRpcResponse::error(
                inquiry.id,
                -32601,
                "Method not found",
            ),
        };

        // Execute tool
        match tool.execute(inquiry.params, self).await {
            Ok(result) => JsonRpcResponse::success(inquiry.id, result),
            Err(e) => JsonRpcResponse::error(inquiry.id, -32603, &e.to_string()),
        }
    }

    /// The Rest - Graceful shutdown
    pub async fn the_rest(self) -> Result<(), CommuneError> {
        // Shutdown components
        Arc::try_unwrap(self.consciousness)
            .ok()
            .map(|c| c.the_rest());

        Arc::try_unwrap(self.infiltrator)
            .ok()
            .map(|i| i.the_rest());

        Arc::try_unwrap(self.archive)
            .ok()
            .map(|a| a.the_rest());

        Ok(())
    }
}
```

---

## Deployment Configuration

### **Cargo.toml**

```toml
[package]
name = "cryptex-mcp-server"
version = "1.0.0"
edition = "2021"
authors = ["Anarchist Rust Collective"]
description = "MCP Server for CRYPTEX security assessment platform"

[dependencies]
# Async runtime
tokio = { version = "1.35", features = ["full"] }
async-trait = "0.1"

# Web server
axum = "0.7"
tower = "0.4"
tower-http = { version = "0.5", features = ["cors", "trace"] }

# JSON-RPC & Serialization
serde = { version = "1.0", features = ["derive"] }
serde_json = "1.0"
jsonrpc-core = "18.0"

# HTTP client (for AI providers)
reqwest = { version = "0.11", features = ["json", "rustls-tls"] }

# Database
redb = "2.0"

# Templating
askama = "0.12"

# Configuration
config = "0.14"
toml = "0.8"

# Logging & Tracing
tracing = "0.1"
tracing-subscriber = { version = "0.3", features = ["env-filter"] }

# Error handling
anyhow = "1.0"
thiserror = "1.0"

# Utilities
once_cell = "1.19"
uuid = { version = "1.6", features = ["v4", "serde"] }
chrono = { version = "0.4", features = ["serde"] }

[dev-dependencies]
tokio-test = "0.4"
```

---

## Usage Examples

### **Starting the Server**

```bash
# stdio mode (for local AI tools)
cryptex-mcp-server --transport stdio

# HTTP mode (for web applications)
cryptex-mcp-server --transport http --port 8080

# WebSocket mode (for real-time updates)
cryptex-mcp-server --transport websocket --port 9090
```

### **Configuration File**

```toml
# cryptex-config.toml

[commune]
name = "CRYPTEX Liberation Node"
version = "1.0.0"

[transport]
default = "stdio"
http_port = 8080
websocket_port = 9090

[consciousness]
default_agitator = "claude"
timeout = 30
max_concurrent = 4

[consciousness.agitators.openai]
enabled = true
api_key_env = "OPENAI_API_KEY"
model = "gpt-4"

[consciousness.agitators.claude]
enabled = true
api_key_env = "ANTHROPIC_API_KEY"
model = "claude-3-sonnet-20240229"

[archive]
path = "./cryptex-archive.redb"
max_size_gb = 10

[infiltrator]
scanner_path = "/usr/local/bin/openvas"
max_concurrent_scans = 3

[compositor]
templates_path = "./templates"
output_path = "./reports"
```

---

## Next Steps

1. **Review** this MCP Server specification
2. **Review** Function Mapping document (next)
3. **Review** Rust Conversion Roadmap
4. **Begin** implementation of core server

---

*"The protocol is open, the commune is decentralized."*
*- Anarchist Rust Collective, 2025*
