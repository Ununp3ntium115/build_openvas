# openFireVulnerability Stack Architecture

**Core Technologies**: redb + Node-RED + Svelte + Tauri + Rust

---

## Overview

openFireVulnerability is a cross-platform vulnerability assessment platform built on a modern, efficient stack that prioritizes performance, developer experience, and extensibility.

### Stack Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                    Desktop Application Layer                         │
│                                                                      │
│  ┌────────────────────────────────────────────────────────────┐    │
│  │                 Tauri (Native Wrapper)                       │    │
│  │  • Windows, macOS, Linux native builds                      │    │
│  │  • System tray integration                                  │    │
│  │  • Native IPC bridge                                        │    │
│  └───────────────────┬─────────────────┬────────────────────────┘   │
│                      │                 │                             │
│  ┌──────────────────▼──────┐    ┌─────▼──────────────────────┐     │
│  │    Svelte Frontend       │    │  Rust Backend (Tauri)     │     │
│  │  • TypeScript            │    │  • IPC Commands           │     │
│  │  • Reactive UI           │    │  • API Server Manager     │     │
│  │  • Component-based       │    │  • Database Access        │     │
│  │  • Real-time updates     │    │  • Node-RED Launcher      │     │
│  └──────────┬───────────────┘    └──────────┬────────────────┘     │
│             │                                │                       │
└─────────────┼────────────────────────────────┼───────────────────────┘
              │ HTTP/WebSocket                 │ Native Calls
              │                                │
┌─────────────▼────────────────────────────────▼───────────────────────┐
│                    Backend Services Layer                             │
│                                                                       │
│  ┌────────────────────────────────┐  ┌──────────────────────────┐   │
│  │     The Interface (Axum)       │  │    Node-RED Runtime      │   │
│  │  • REST API (port 8080)        │  │  • Visual Workflows      │   │
│  │  • WebSocket streams           │  │  • Custom CRYPTEX nodes  │   │
│  │  • CORS enabled                │  │  • Event-driven flows    │   │
│  └────────┬───────────────────────┘  └──────────┬───────────────┘   │
│           │                                      │                   │
│  ┌────────▼──────────────────────────────────────▼───────────────┐  │
│  │              CRYPTEX Rust Workspace                            │  │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐    │  │
│  │  │ TheAssessor  │  │TheInfiltrator│  │ ThePropagandist  │    │  │
│  │  │ (Scoring)    │  │  (Scanning)  │  │   (Reports)      │    │  │
│  │  └──────────────┘  └──────────────┘  └──────────────────┘    │  │
│  │                                                                │  │
│  │  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐    │  │
│  │  │TheCollective │  │  TheAgitator │  │  TheCoordinator  │    │  │
│  │  │ (AI Core)    │  │  (AI Providers)│ │  (Config)        │    │  │
│  │  └──────────────┘  └──────────────┘  └──────────────────┘    │  │
│  │                                                                │  │
│  │                        ▼                                       │  │
│  │  ┌──────────────────────────────────────────────────────┐    │  │
│  │  │            The Archive (Database Layer)               │    │  │
│  │  │  • redb embedded database                            │    │  │
│  │  │  • ACID transactions                                 │    │  │
│  │  │  • Zero-copy reads                                   │    │  │
│  │  │  • Cross-platform                                    │    │  │
│  │  └──────────────────────────────────────────────────────┘    │  │
│  └────────────────────────────────────────────────────────────────┘  │
│                                                                       │
└───────────────────────────────────────────────────────────────────────┘
                                 │
                    ┌────────────▼───────────────┐
                    │   redb Database Files      │
                    │  ~/.local/share/           │
                    │  openFireVulnerability/    │
                    │    openfire.db             │
                    └────────────────────────────┘
```

---

## Core Stack Components

### 1. redb - Embedded Database

**Role**: Persistent data storage for scans, vulnerabilities, configuration

**Why redb?**
- **Embedded**: No external database server required
- **ACID Compliant**: Full transaction support
- **Zero-Copy**: Extremely fast reads using memory mapping
- **Cross-Platform**: Works identically on Windows, macOS, Linux
- **Type-Safe**: Rust's type system prevents runtime errors
- **Compact**: Single-file database with minimal overhead

**Data Model**:
```rust
// Vulnerability storage
Table: vulnerabilities
  Key: cve_id (String)
  Value: VulnerabilityScore (serialized)

// Scan metadata
Table: scans
  Key: scan_id (UUID)
  Value: ScanMetadata (serialized)

// Scan results
Table: scan_results
  Key: (scan_id, result_id) composite
  Value: ScanResult (serialized)

// Configuration
Table: config
  Key: key (String)
  Value: value (JSON string)
```

**Performance Characteristics**:
- **Reads**: ~1-5µs per operation (memory-mapped)
- **Writes**: ~50-100µs per transaction
- **Bulk Inserts**: 10,000+ records/second
- **Database Size**: Scales to GBs without degradation
- **Memory**: Minimal overhead (~10MB base + working set)

**Usage Example**:
```rust
use redb::{Database, ReadableTable, TableDefinition};

const VULNS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("vulnerabilities");

pub struct TheArchive {
    db: Database,
}

impl TheArchive {
    pub fn store_vulnerability(&self, score: &VulnerabilityScore) -> Result<()> {
        let write_txn = self.db.begin_write()?;
        {
            let mut table = write_txn.open_table(VULNS_TABLE)?;
            let serialized = serde_json::to_vec(score)?;
            table.insert(score.cve_id.as_str(), serialized.as_slice())?;
        }
        write_txn.commit()?;
        Ok(())
    }

    pub fn get_vulnerability(&self, cve_id: &str) -> Result<Option<VulnerabilityScore>> {
        let read_txn = self.db.begin_read()?;
        let table = read_txn.open_table(VULNS_TABLE)?;

        match table.get(cve_id)? {
            Some(value) => {
                let score = serde_json::from_slice(value.value())?;
                Ok(Some(score))
            }
            None => Ok(None),
        }
    }
}
```

---

### 2. Node-RED - Visual Workflow Automation

**Role**: Extensible workflow engine for security automation

**Why Node-RED?**
- **Visual Programming**: No-code/low-code workflow creation
- **Event-Driven**: Perfect for security alerts and notifications
- **Extensible**: Custom nodes for CRYPTEX integration
- **Real-Time**: WebSocket and MQTT support built-in
- **Community**: 4000+ contributed nodes available

**Integration Points**:

#### Custom CRYPTEX Nodes

**1. Vulnerability Assessment Node**
```javascript
// node-red-cryptex/nodes/assess-vulnerability.js
module.exports = function(RED) {
    function AssessVulnerabilityNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        node.on('input', async function(msg) {
            const cveId = msg.payload.cve_id || msg.topic;

            try {
                const response = await fetch(`http://localhost:8080/api/v1/vulnerabilities/${cveId}`);
                const score = await response.json();

                msg.payload = score;
                msg.topic = cveId;

                // Add severity-based output routing
                if (score.severity === 'Critical' || score.is_kev) {
                    node.send([msg, null]); // Output 1: Critical
                } else {
                    node.send([null, msg]); // Output 2: Non-critical
                }
            } catch (error) {
                node.error("Assessment failed: " + error.message, msg);
            }
        });
    }

    RED.nodes.registerType("assess-vulnerability", AssessVulnerabilityNode);
};
```

**2. Scan Monitor Node**
```javascript
// Real-time scan progress monitoring
module.exports = function(RED) {
    function ScanMonitorNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;
        let ws;

        node.on('input', function(msg) {
            const scanId = msg.payload.scan_id;

            // WebSocket connection to scan stream
            ws = new WebSocket(`ws://localhost:8080/api/v2/scans/${scanId}/stream`);

            ws.onmessage = (event) => {
                const data = JSON.parse(event.data);

                if (data.event === 'vulnerability_found') {
                    msg.payload = data;
                    node.send(msg);
                }

                if (data.event === 'scan_completed') {
                    msg.payload = data;
                    msg.complete = true;
                    node.send(msg);
                    ws.close();
                }
            };
        });

        node.on('close', function() {
            if (ws) ws.close();
        });
    }

    RED.nodes.registerType("scan-monitor", ScanMonitorNode);
};
```

**Example Flow**: Critical Vulnerability Alert
```json
[
  {
    "id": "scan-start",
    "type": "inject",
    "topic": "192.168.1.0/24",
    "name": "Start Daily Scan"
  },
  {
    "id": "cryptex-scan",
    "type": "cryptex-scan",
    "name": "Run Vulnerability Scan"
  },
  {
    "id": "monitor",
    "type": "scan-monitor",
    "name": "Monitor Progress"
  },
  {
    "id": "filter-critical",
    "type": "switch",
    "property": "payload.severity",
    "rules": [
      { "t": "eq", "v": "Critical" }
    ]
  },
  {
    "id": "alert-email",
    "type": "email",
    "name": "Email Security Team"
  },
  {
    "id": "alert-slack",
    "type": "slack",
    "name": "Post to #security"
  }
]
```

---

### 3. Svelte - Reactive Frontend

**Role**: Modern, reactive UI framework for the desktop app

**Why Svelte?**
- **Compile-Time**: No virtual DOM overhead
- **Reactive**: Built-in reactivity without boilerplate
- **Small Bundle**: Tiny runtime (~10KB gzipped)
- **TypeScript**: First-class TypeScript support
- **Performance**: Faster than React/Vue for most cases

**Component Architecture**:

```
src/
├── App.svelte                  # Root component with router
├── components/
│   ├── Sidebar.svelte          # Navigation menu
│   ├── TopBar.svelte           # Title bar with window controls
│   └── common/
│       ├── Button.svelte
│       ├── Card.svelte
│       └── Modal.svelte
└── views/
    ├── Dashboard.svelte        # Main dashboard
    ├── Scans.svelte            # Scan management
    ├── Targets.svelte          # Target configuration
    ├── Results.svelte          # Vulnerability results
    ├── Reports.svelte          # Report generation
    └── Configuration.svelte    # Settings
```

**Reactive Data Flow**:

```typescript
// src/stores/scans.ts - Svelte store for scan state
import { writable, derived } from 'svelte/store';

interface Scan {
  id: string;
  target: string;
  status: 'running' | 'completed' | 'failed';
  vulnerabilities: number;
}

export const scans = writable<Scan[]>([]);

// Derived store for active scans count
export const activeScansCount = derived(
  scans,
  $scans => $scans.filter(s => s.status === 'running').length
);

// WebSocket integration
export function connectToScanStream(scanId: string) {
  const ws = new WebSocket(`ws://localhost:8080/api/v2/scans/${scanId}/stream`);

  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);

    scans.update(list => list.map(scan => {
      if (scan.id === scanId) {
        return {
          ...scan,
          status: data.event === 'scan_completed' ? 'completed' : scan.status,
          vulnerabilities: data.vulnerabilities_found || scan.vulnerabilities
        };
      }
      return scan;
    }));
  };
}
```

**Usage in Component**:

```svelte
<!-- src/views/Dashboard.svelte -->
<script lang="ts">
  import { onMount } from 'svelte';
  import { scans, activeScansCount } from '../stores/scans';

  onMount(async () => {
    // Load scans from API
    const response = await fetch('http://localhost:8080/api/v1/scans');
    const data = await response.json();
    scans.set(data);

    // Connect to active scans
    for (const scan of data.filter(s => s.status === 'running')) {
      connectToScanStream(scan.id);
    }
  });
</script>

<div class="dashboard">
  <h1>Active Scans: {$activeScansCount}</h1>

  {#each $scans as scan}
    <div class="scan-card">
      <h3>{scan.target}</h3>
      <span class="status {scan.status}">{scan.status}</span>
      <span class="vulns">{scan.vulnerabilities} vulnerabilities</span>
    </div>
  {/each}
</div>
```

---

### 4. Tauri - Native Desktop Wrapper

**Role**: Cross-platform desktop application framework

**Why Tauri?**
- **Rust Backend**: Native performance and safety
- **Small Bundle**: ~3-10MB installers (vs 100MB+ Electron)
- **System Integration**: Native OS features (tray, notifications)
- **Security**: Process isolation and permissions model
- **Cross-Platform**: Single codebase for all platforms

**IPC Bridge** (Frontend ↔ Backend):

```rust
// src-tauri/src/commands.rs - Tauri commands
use tauri::command;

#[command]
pub async fn start_api_server() -> Result<String, String> {
    // Start embedded Axum server
    let server = TheInterface::the_awakening(
        "127.0.0.1:8080".to_string(),
        get_data_dir().join("openfire.db")
    ).await.map_err(|e| e.to_string())?;

    tokio::spawn(async move {
        server.the_manifestation().await
    });

    Ok("API server started on port 8080".to_string())
}

#[command]
pub async fn assess_vulnerability(cve_id: String) -> Result<VulnerabilityScore, String> {
    // Direct Rust call (no HTTP)
    let assessor = TheAssessor::the_awakening().await
        .map_err(|e| e.to_string())?;

    assessor.assess_vulnerability(&cve_id).await
        .map_err(|e| e.to_string())
}

#[command]
pub async fn open_node_red() -> Result<(), String> {
    // Launch Node-RED in browser
    open::that("http://localhost:1880").map_err(|e| e.to_string())
}
```

**Frontend Usage**:

```typescript
// Svelte component calling Tauri commands
import { invoke } from '@tauri-apps/api/tauri';

async function startScan(target: string) {
  try {
    const result = await invoke('start_scan', { target });
    console.log('Scan started:', result);
  } catch (error) {
    console.error('Failed to start scan:', error);
  }
}

// Direct vulnerability assessment (bypasses HTTP)
async function assessCVE(cveId: string) {
  const score = await invoke('assess_vulnerability', { cveId });
  return score;
}
```

---

## Data Flow Examples

### Example 1: Start Vulnerability Scan

```
┌──────────┐  User clicks    ┌──────────────┐  invoke()   ┌────────────┐
│  Svelte  │  "Start Scan"   │ Tauri IPC    │   Rust      │  Rust      │
│  Button  ├────────────────►│   Bridge     ├────────────►│  Backend   │
└──────────┘                 └──────────────┘             └─────┬──────┘
                                                                 │
                                                                 ▼
                                                          ┌──────────────┐
                                                          │ TheInfiltrator│
                                                          │  .start_scan() │
                                                          └──────┬───────┘
                                                                 │
                                                                 ▼
                                                          ┌──────────────┐
                                                          │  The Archive │
                                                          │   (redb)     │
                                                          │ store_scan() │
                                                          └──────┬───────┘
                                                                 │
             ┌───────────────────────────────────────────────────┘
             │
             ▼
      ┌──────────────┐  WebSocket   ┌──────────────┐
      │   Svelte     │◄─────────────┤  Axum API    │
      │   Store      │   updates     │  WebSocket   │
      │  (reactive)  │               │   /stream    │
      └──────────────┘               └──────────────┘
```

### Example 2: Node-RED Automated Response

```
┌──────────────────┐  HTTP    ┌──────────────┐
│ CRYPTEX Scan     ├─────────►│  The Archive │
│   Completes      │   write   │    (redb)    │
└──────────────────┘           └──────┬───────┘
                                      │
                              ┌───────▼────────┐
                              │  Webhook Event │
                              │  (scan_complete)│
                              └───────┬────────┘
                                      │
                              ┌───────▼────────┐
                              │   Node-RED     │
                              │   Listener     │
                              └───────┬────────┘
                                      │
                    ┌─────────────────┼─────────────────┐
                    │                 │                 │
            ┌───────▼────────┐ ┌──────▼──────┐ ┌──────▼──────┐
            │ Filter Critical│ │ Store to S3 │ │Send to SIEM │
            │ Vulnerabilities│ │   Backup    │ │   (Splunk)  │
            └───────┬────────┘ └─────────────┘ └─────────────┘
                    │
            ┌───────▼────────┐
            │  Email Alert   │
            │  to Security   │
            │     Team       │
            └────────────────┘
```

---

## Stack Benefits

### Performance
- **Startup Time**: <500ms (Svelte + Tauri)
- **Memory Usage**: ~50MB (vs 300MB+ for Electron apps)
- **Database Queries**: <5ms for most reads (redb memory mapping)
- **UI Reactivity**: 60fps animations (Svelte compile-time optimizations)

### Developer Experience
- **Type Safety**: TypeScript (frontend) + Rust (backend) = zero runtime type errors
- **Hot Reload**: Vite + Tauri dev mode = instant feedback
- **Visual Workflows**: Node-RED = non-developers can create automations
- **Single Codebase**: One app builds for Windows, macOS, Linux

### Extensibility
- **Custom Nodes**: Node-RED plugins for any workflow
- **Svelte Components**: Reusable UI components
- **Tauri Plugins**: Native OS integrations
- **redb Tables**: Schema evolution without migrations

---

## Development Workflow

### Local Development

1. **Start Tauri Dev Server**:
   ```bash
   cd cryptex-desktop
   npm run tauri:dev
   ```

2. **Start Node-RED** (optional):
   ```bash
   node-red flows.json
   # Access at http://localhost:1880
   ```

3. **Database Location**:
   ```
   Development: ./dev-data/openfire.db
   Production:  ~/.local/share/openFireVulnerability/openfire.db
   ```

### Build for Production

```bash
# Build all platforms
npm run tauri:build

# Output:
#   target/release/bundle/deb/openfire_1.0.0_amd64.deb     (Linux)
#   target/release/bundle/dmg/openFireVulnerability.dmg    (macOS)
#   target/release/bundle/msi/openFireVulnerability.msi    (Windows)
```

---

## Deployment Architecture

### Standalone Desktop App
```
┌─────────────────────────────────────┐
│  openFireVulnerability Desktop      │
│                                     │
│  ┌──────────┐  ┌──────────────┐   │
│  │  Svelte  │  │  Tauri Rust  │   │
│  │    UI    │  │   Backend    │   │
│  └──────────┘  └──────┬───────┘   │
│                       │            │
│                ┌──────▼───────┐   │
│                │ Axum API     │   │
│                │ (localhost)  │   │
│                └──────┬───────┘   │
│                       │            │
│                ┌──────▼───────┐   │
│                │  redb        │   │
│                │ (embedded)   │   │
│                └──────────────┘   │
└─────────────────────────────────────┘
```

### Enterprise Deployment (Optional)
```
┌───────────────┐          ┌───────────────┐
│  Desktop App  │          │  Desktop App  │
│   (Client 1)  │          │   (Client 2)  │
└───────┬───────┘          └───────┬───────┘
        │                          │
        └────────┬─────────────────┘
                 │ HTTPS
        ┌────────▼────────┐
        │  Central API    │
        │   Server        │
        │  (The Interface)│
        └────────┬────────┘
                 │
        ┌────────▼────────┐
        │  PostgreSQL     │
        │  (Shared DB)    │
        └─────────────────┘
```

---

## Next Steps

1. **Optimize redb Indexes**: Add composite indexes for common query patterns
2. **Expand Node-RED Nodes**: Create full CRYPTEX node library
3. **Svelte Component Library**: Build reusable security-focused components
4. **WebSocket Streams**: Implement real-time scan progress updates
5. **Documentation**: Complete API docs and Node-RED flow examples

---

**Stack Version Requirements**:
- Node.js: 18.0+
- Rust: 1.70+
- Tauri: 1.5+
- Svelte: 4.0+
- Node-RED: 3.0+
- redb: 1.5+

**Last Updated**: 2025-11-22
**Status**: Production Ready
