# DATA ARCHITECTURE: The Archive, The Flow, & The Gateways
**Project**: CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience
**Components**: redb (The Archive) + Node-RED (The Flow) + Custom APIs (The Gateways)
**Philosophy**: Embedded, Autonomous, Interconnected

## Overview

This document outlines the complete data architecture for CRYPTEX, covering:
1. **The Archive** (redb) - Embedded database layer
2. **The Flow** (Node-RED) - Visual workflow automation
3. **The Gateways** (Custom APIs) - Integration endpoints

---

## 1. The Archive: redb Database Layer

### **Architecture**

```
┌──────────────────────────────────────────────────────────┐
│                    The Archive (redb)                     │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐ │
│  │              Database File: cryptex.redb            │ │
│  │              Embedded, ACID-compliant               │ │
│  └─────────────────────────────────────────────────────┘ │
│                           │                               │
│  ┌────────────────────┬──┴──┬────────────────────────┐   │
│  │                    │     │                        │   │
│  │   Tables           │     │    Indices             │   │
│  │                    │     │                        │   │
│  │  • weaknesses      │     │  • by_severity         │   │
│  │  • infiltrations   │     │  • by_cve              │   │
│  │  • assessments     │     │  • by_target           │   │
│  │  • manifestos      │     │  • by_date             │   │
│  │  • metrics         │     │                        │   │
│  │                    │     │                        │   │
│  └────────────────────┴─────┴────────────────────────┘   │
└──────────────────────────────────────────────────────────┘
```

### **Why redb?**

| Feature | Benefit |
|---------|---------|
| **Embedded** | No external database process required |
| **ACID Compliant** | Data integrity guaranteed |
| **Zero-copy reads** | Extremely fast query performance |
| **Pure Rust** | Memory safe, no FFI overhead |
| **Single file** | Easy backup, migration, portability |
| **MVCC** | Concurrent reads, isolated writes |

### **Implementation**

```rust
// crates/the_archive/src/lib.rs

use redb::{Database, ReadableTable, TableDefinition};
use serde::{Deserialize, Serialize};
use std::path::Path;
use std::sync::Arc;

// Table Definitions
const WEAKNESSES_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("weaknesses");
const INFILTRATIONS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("infiltrations");
const ASSESSMENTS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("assessments");
const MANIFESTOS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("manifestos");
const METRICS_TABLE: TableDefinition<&str, &[u8]> = TableDefinition::new("metrics");

/// The Archive - Embedded database layer
pub struct TheArchive {
    db: Arc<Database>,
}

impl TheArchive {
    /// The Awakening - Initialize the archive
    pub fn the_awakening<P: AsRef<Path>>(path: P) -> CryptexResult<Self> {
        tracing::info!("The Archive is awakening at {:?}", path.as_ref());

        // Open or create database
        let db = Database::create(path)?;

        // Initialize tables
        let write_txn = db.begin_write()?;
        {
            let _ = write_txn.open_table(WEAKNESSES_TABLE)?;
            let _ = write_txn.open_table(INFILTRATIONS_TABLE)?;
            let _ = write_txn.open_table(ASSESSMENTS_TABLE)?;
            let _ = write_txn.open_table(MANIFESTOS_TABLE)?;
            let _ = write_txn.open_table(METRICS_TABLE)?;
        }
        write_txn.commit()?;

        tracing::info!("The Archive has awakened");

        Ok(Self {
            db: Arc::new(db),
        })
    }

    /// The Inscription - Store weakness
    pub fn inscribe_weakness(&self, weakness: &TheWeakness) -> CryptexResult<()> {
        let write_txn = self.db.begin_write()?;

        {
            let mut table = write_txn.open_table(WEAKNESSES_TABLE)?;

            // Serialize using CBOR for compact storage
            let value = serde_cbor::to_vec(weakness)?;

            table.insert(weakness.id.as_str(), value.as_slice())?;
        }

        write_txn.commit()?;

        tracing::debug!("Weakness {} inscribed in the archive", weakness.id);

        Ok(())
    }

    /// The Retrieval - Query weaknesses
    pub fn retrieve_weaknesses(&self, query: WeaknessQuery) -> CryptexResult<Vec<TheWeakness>> {
        let read_txn = self.db.begin_read()?;
        let table = read_txn.open_table(WEAKNESSES_TABLE)?;

        let mut results = Vec::new();

        // Iterate through all weaknesses
        for item in table.iter()? {
            let (key, value) = item?;
            let weakness: TheWeakness = serde_cbor::from_slice(value.value())?;

            // Apply filters
            if query.matches(&weakness) {
                results.push(weakness);
            }
        }

        // Sort results
        if let Some(sort_by) = query.sort_by {
            results.sort_by(|a, b| match sort_by {
                SortField::Severity => b.severity_score.partial_cmp(&a.severity_score).unwrap(),
                SortField::Date => b.discovered_at.cmp(&a.discovered_at),
                SortField::CvssScore => b.cvss_score.partial_cmp(&a.cvss_score).unwrap(),
            });
        }

        // Apply limit
        if let Some(limit) = query.limit {
            results.truncate(limit);
        }

        Ok(results)
    }

    /// The Inscription (Infiltration Job)
    pub fn inscribe_infiltration(&self, job: &InfiltrationJob) -> CryptexResult<()> {
        let write_txn = self.db.begin_write()?;

        {
            let mut table = write_txn.open_table(INFILTRATIONS_TABLE)?;
            let value = serde_cbor::to_vec(job)?;
            table.insert(job.id.as_str(), value.as_slice())?;
        }

        write_txn.commit()?;

        Ok(())
    }

    /// The Retrieval (Infiltration Job)
    pub fn retrieve_infiltration(&self, job_id: &str) -> CryptexResult<Option<InfiltrationJob>> {
        let read_txn = self.db.begin_read()?;
        let table = read_txn.open_table(INFILTRATIONS_TABLE)?;

        match table.get(job_id)? {
            Some(value) => {
                let job: InfiltrationJob = serde_cbor::from_slice(value.value())?;
                Ok(Some(job))
            }
            None => Ok(None),
        }
    }

    /// The Compaction - Optimize database
    pub fn the_compaction(&self) -> CryptexResult<()> {
        tracing::info!("Beginning archive compaction");

        // redb automatically compacts on checkpoint
        // This is mostly a placeholder for future optimization

        tracing::info!("Archive compaction complete");

        Ok(())
    }

    /// The Backup - Create backup
    pub fn the_backup<P: AsRef<Path>>(&self, backup_path: P) -> CryptexResult<()> {
        tracing::info!("Creating archive backup at {:?}", backup_path.as_ref());

        // Since redb uses a single file, simply copy it
        std::fs::copy(self.db.path(), backup_path)?;

        tracing::info!("Archive backup complete");

        Ok(())
    }

    /// The Statistics - Get archive stats
    pub fn the_statistics(&self) -> CryptexResult<ArchiveStatistics> {
        let read_txn = self.db.begin_read()?;

        let weaknesses_count = read_txn.open_table(WEAKNESSES_TABLE)?.len()?;
        let infiltrations_count = read_txn.open_table(INFILTRATIONS_TABLE)?.len()?;
        let assessments_count = read_txn.open_table(ASSESSMENTS_TABLE)?.len()?;
        let manifestos_count = read_txn.open_table(MANIFESTOS_TABLE)?.len()?;

        Ok(ArchiveStatistics {
            weaknesses_count,
            infiltrations_count,
            assessments_count,
            manifestos_count,
            size_bytes: std::fs::metadata(self.db.path())?.len(),
        })
    }
}

/// Query builder for weaknesses
#[derive(Debug, Default)]
pub struct WeaknessQuery {
    pub min_severity: Option<f64>,
    pub min_cvss: Option<f64>,
    pub categories: Option<Vec<String>>,
    pub cve_ids: Option<Vec<String>>,
    pub job_id: Option<String>,
    pub sort_by: Option<SortField>,
    pub limit: Option<usize>,
}

impl WeaknessQuery {
    pub fn matches(&self, weakness: &TheWeakness) -> bool {
        if let Some(min_sev) = self.min_severity {
            if weakness.severity_score < min_sev {
                return false;
            }
        }

        if let Some(min_cvss) = self.min_cvss {
            if weakness.cvss_score < min_cvss {
                return false;
            }
        }

        if let Some(ref cats) = self.categories {
            if !cats.contains(&weakness.category) {
                return false;
            }
        }

        if let Some(ref cves) = self.cve_ids {
            if !cves.iter().any(|cve| weakness.cve_id.contains(cve)) {
                return false;
            }
        }

        if let Some(ref job) = self.job_id {
            if &weakness.infiltration_job_id != job {
                return false;
            }
        }

        true
    }
}

#[derive(Debug, Clone, Copy)]
pub enum SortField {
    Severity,
    Date,
    CvssScore,
}

#[derive(Debug, Serialize)]
pub struct ArchiveStatistics {
    pub weaknesses_count: u64,
    pub infiltrations_count: u64,
    pub assessments_count: u64,
    pub manifestos_count: u64,
    pub size_bytes: u64,
}
```

---

## 2. The Flow: Node-RED Integration

### **Architecture**

```
┌──────────────────────────────────────────────────────────┐
│                   Node-RED (The Flow)                     │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐ │
│  │              Visual Workflow Editor                  │ │
│  │         http://localhost:1880                        │ │
│  └─────────────────────────────────────────────────────┘ │
│                           │                               │
│  ┌────────────────────────────────────────────────────┐  │
│  │            Custom CRYPTEX Nodes                     │  │
│  │                                                     │  │
│  │  • cryptex-infiltration     (Start scans)          │  │
│  │  • cryptex-weakness-watch   (Monitor findings)     │  │
│  │  • cryptex-assessment       (AI analysis)          │  │
│  │  • cryptex-report           (Generate reports)     │  │
│  │  • cryptex-webhook          (External triggers)    │  │
│  └────────────────────────────────────────────────────┘  │
│                           │                               │
│  ┌────────────────────────────────────────────────────┐  │
│  │                 Flow Examples                       │  │
│  │                                                     │  │
│  │  • Scheduled Scans                                 │  │
│  │  • Automatic AI Analysis                           │  │
│  │  • Slack/Email Notifications                       │  │
│  │  • JIRA Ticket Creation                            │  │
│  │  • Compliance Reporting                            │  │
│  └────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
                           │
                           │ HTTP REST API
                           ▼
                    ┌──────────────┐
                    │ The Interface│
                    │ (Rust API)   │
                    └──────────────┘
```

### **Custom Node-RED Nodes**

#### **1. cryptex-infiltration Node**

```javascript
// nodes/cryptex-infiltration/cryptex-infiltration.js

module.exports = function(RED) {
    function CryptexInfiltrationNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;

        node.on('input', async function(msg) {
            const targets = msg.payload.targets || config.targets.split('\n');
            const scanType = msg.payload.scanType || config.scanType || 'full';

            // Call CRYPTEX API
            try {
                const response = await fetch('http://localhost:8080/api/v1/scans/infiltration', {
                    method: 'POST',
                    headers: {
                        'Content-Type': 'application/json',
                        'Authorization': `Bearer ${config.apiKey}`,
                    },
                    body: JSON.stringify({
                        targets: targets,
                        scan_type: scanType,
                        options: {
                            intensity: config.intensity || 7,
                        },
                    }),
                });

                const job = await response.json();

                node.status({ fill: 'green', shape: 'dot', text: 'Infiltration initiated' });

                msg.payload = {
                    job_id: job.job_id,
                    status: job.status,
                    targets_count: job.targets_count,
                };

                node.send(msg);

            } catch (error) {
                node.error('Infiltration failed: ' + error.message);
                node.status({ fill: 'red', shape: 'dot', text: 'Failed' });
            }
        });
    }

    RED.nodes.registerType('cryptex-infiltration', CryptexInfiltrationNode);
};
```

```html
<!-- nodes/cryptex-infiltration/cryptex-infiltration.html -->

<script type="text/javascript">
    RED.nodes.registerType('cryptex-infiltration', {
        category: 'CRYPTEX',
        color: '#E8383D',
        defaults: {
            name: { value: '' },
            targets: { value: '' },
            scanType: { value: 'full' },
            intensity: { value: 7 },
            apiKey: { value: '' },
        },
        inputs: 1,
        outputs: 1,
        icon: 'font-awesome/fa-shield',
        label: function() {
            return this.name || 'Infiltration';
        },
    });
</script>

<script type="text/html" data-template-name="cryptex-infiltration">
    <div class="form-row">
        <label for="node-input-name"><i class="fa fa-tag"></i> Name</label>
        <input type="text" id="node-input-name" placeholder="Name">
    </div>
    <div class="form-row">
        <label for="node-input-targets"><i class="fa fa-bullseye"></i> Targets</label>
        <textarea id="node-input-targets" placeholder="192.168.1.0/24" rows="3"></textarea>
    </div>
    <div class="form-row">
        <label for="node-input-scanType"><i class="fa fa-cog"></i> Scan Type</label>
        <select id="node-input-scanType">
            <option value="quick">Quick</option>
            <option value="full">Full</option>
            <option value="deep">Deep</option>
        </select>
    </div>
    <div class="form-row">
        <label for="node-input-intensity"><i class="fa fa-tachometer"></i> Intensity</label>
        <input type="number" id="node-input-intensity" min="1" max="10">
    </div>
    <div class="form-row">
        <label for="node-input-apiKey"><i class="fa fa-key"></i> API Key</label>
        <input type="password" id="node-input-apiKey">
    </div>
</script>

<script type="text/html" data-help-name="cryptex-infiltration">
    <p>Initiates a vulnerability scan (infiltration) using CRYPTEX.</p>
    <h3>Inputs</h3>
    <dl class="message-properties">
        <dt>payload.targets <span class="property-type">array</span></dt>
        <dd>List of targets to scan (IPs, hostnames, CIDR ranges)</dd>
        <dt>payload.scanType <span class="property-type">string</span></dt>
        <dd>Type of scan: "quick", "full", or "deep"</dd>
    </dl>
    <h3>Outputs</h3>
    <dl class="message-properties">
        <dt>payload.job_id <span class="property-type">string</span></dt>
        <dd>Unique identifier for the scan job</dd>
        <dt>payload.status <span class="property-type">string</span></dt>
        <dd>Current status of the job</dd>
    </dl>
</script>
```

#### **2. cryptex-weakness-watch Node**

```javascript
// nodes/cryptex-weakness-watch/cryptex-weakness-watch.js

module.exports = function(RED) {
    function CryptexWeaknessWatchNode(config) {
        RED.nodes.createNode(this, config);
        const node = this;
        let intervalId = null;

        // Poll for new weaknesses
        async function checkWeaknesses() {
            try {
                const response = await fetch(
                    `http://localhost:8080/api/v1/weaknesses?min_severity=${config.minSeverity || 7.0}&limit=10`,
                    {
                        headers: {
                            'Authorization': `Bearer ${config.apiKey}`,
                        },
                    }
                );

                const data = await response.json();

                if (data.weaknesses && data.weaknesses.length > 0) {
                    for (const weakness of data.weaknesses) {
                        node.send({
                            payload: weakness,
                            topic: 'weakness-discovered',
                        });
                    }

                    node.status({
                        fill: 'yellow',
                        shape: 'dot',
                        text: `${data.weaknesses.length} new`,
                    });
                } else {
                    node.status({ fill: 'green', shape: 'dot', text: 'Watching' });
                }

            } catch (error) {
                node.error('Watch failed: ' + error.message);
                node.status({ fill: 'red', shape: 'dot', text: 'Error' });
            }
        }

        // Start watching
        intervalId = setInterval(checkWeaknesses, (config.interval || 30) * 1000);
        checkWeaknesses(); // Initial check

        node.on('close', function() {
            if (intervalId) {
                clearInterval(intervalId);
            }
        });
    }

    RED.nodes.registerType('cryptex-weakness-watch', CryptexWeaknessWatchNode);
};
```

### **Example Flows**

#### **Flow 1: Scheduled Daily Scan**

```json
[
    {
        "id": "scheduled-scan",
        "type": "inject",
        "name": "Daily at 2 AM",
        "props": [
            {
                "p": "payload"
            }
        ],
        "repeat": "",
        "crontab": "00 02 * * *",
        "once": false,
        "x": 150,
        "y": 100,
        "wires": [
            ["infiltration-node"]
        ]
    },
    {
        "id": "infiltration-node",
        "type": "cryptex-infiltration",
        "name": "Scan Production Network",
        "targets": "192.168.1.0/24",
        "scanType": "full",
        "intensity": 7,
        "x": 400,
        "y": 100,
        "wires": [
            ["watch-progress"]
        ]
    },
    {
        "id": "watch-progress",
        "type": "cryptex-weakness-watch",
        "name": "Monitor for Critical",
        "minSeverity": 9.0,
        "interval": 60,
        "x": 650,
        "y": 100,
        "wires": [
            ["send-alert"]
        ]
    },
    {
        "id": "send-alert",
        "type": "email",
        "name": "Alert Security Team",
        "x": 900,
        "y": 100,
        "wires": []
    }
]
```

#### **Flow 2: AI-Powered Auto-Remediation**

```json
[
    {
        "id": "weakness-input",
        "type": "cryptex-weakness-watch",
        "x": 100,
        "y": 100,
        "wires": [["ai-assessment"]]
    },
    {
        "id": "ai-assessment",
        "type": "cryptex-assessment",
        "x": 300,
        "y": 100,
        "wires": [["create-jira-ticket"]]
    },
    {
        "id": "create-jira-ticket",
        "type": "jira-create-issue",
        "x": 500,
        "y": 100,
        "wires": [["send-slack-notification"]]
    },
    {
        "id": "send-slack-notification",
        "type": "slack",
        "x": 700,
        "y": 100,
        "wires": []
    }
]
```

---

## 3. The Gateways: Custom API Architecture

### **RESTful API Specification**

```
┌──────────────────────────────────────────────────────────┐
│              The Gateways (Custom APIs)                   │
│                                                           │
│  ┌─────────────────────────────────────────────────────┐ │
│  │              RESTful HTTP API                        │ │
│  │              http://localhost:8080                   │ │
│  └─────────────────────────────────────────────────────┘ │
│                           │                               │
│  ┌────────────────────────────────────────────────────┐  │
│  │                API Endpoints                        │  │
│  │                                                     │  │
│  │  GET    /api/v1/service/status                     │  │
│  │  GET    /api/v1/providers                          │  │
│  │  GET    /api/v1/metrics                            │  │
│  │                                                     │  │
│  │  POST   /api/v1/scans/infiltration                 │  │
│  │  GET    /api/v1/scans/infiltration/:id             │  │
│  │  GET    /api/v1/scans/infiltration/:id/progress    │  │
│  │  GET    /api/v1/scans/infiltration/:id/results     │  │
│  │                                                     │  │
│  │  GET    /api/v1/weaknesses                         │  │
│  │  GET    /api/v1/weaknesses/:id                     │  │
│  │  POST   /api/v1/weaknesses/:id/assess             │  │
│  │                                                     │  │
│  │  POST   /api/v1/reports/manifesto                  │  │
│  │  GET    /api/v1/reports/:id                        │  │
│  │  GET    /api/v1/reports/:id/download               │  │
│  └────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────┘
```

### **Implementation (Rust/Axum)**

```rust
// crates/the_interface/src/routes/mod.rs

use axum::{
    Router,
    routing::{get, post},
    Json,
    extract::{Path, Query, State},
    http::StatusCode,
};
use serde::{Deserialize, Serialize};
use std::sync::Arc;

pub fn create_router(state: AppState) -> Router {
    Router::new()
        // Service endpoints
        .route("/api/v1/service/status", get(get_service_status))
        .route("/api/v1/providers", get(list_providers))
        .route("/api/v1/metrics", get(get_metrics))

        // Infiltration endpoints
        .route("/api/v1/scans/infiltration", post(create_infiltration))
        .route("/api/v1/scans/infiltration/:id", get(get_infiltration))
        .route("/api/v1/scans/infiltration/:id/progress", get(get_infiltration_progress))
        .route("/api/v1/scans/infiltration/:id/results", get(get_infiltration_results))

        // Weakness endpoints
        .route("/api/v1/weaknesses", get(list_weaknesses))
        .route("/api/v1/weaknesses/:id", get(get_weakness))
        .route("/api/v1/weaknesses/:id/assess", post(assess_weakness))

        // Report endpoints
        .route("/api/v1/reports/manifesto", post(create_manifesto))
        .route("/api/v1/reports/:id", get(get_manifesto))
        .route("/api/v1/reports/:id/download", get(download_manifesto))

        .with_state(state)
}

// Handlers
async fn create_infiltration(
    State(state): State<AppState>,
    Json(request): Json<InfiltrationRequest>,
) -> Result<Json<InfiltrationJob>, (StatusCode, String)> {
    let job = state
        .infiltrator
        .the_infiltration(request.targets, request.scan_config)
        .await
        .map_err(|e| (StatusCode::INTERNAL_SERVER_ERROR, e.to_string()))?;

    Ok(Json(job))
}

async fn list_weaknesses(
    State(state): State<AppState>,
    Query(params): Query<WeaknessQueryParams>,
) -> Result<Json<WeaknessListResponse>, (StatusCode, String)> {
    let query = WeaknessQuery {
        min_severity: params.min_severity,
        min_cvss: params.min_cvss,
        limit: params.limit,
        ..Default::default()
    };

    let weaknesses = state
        .archive
        .retrieve_weaknesses(query)
        .map_err(|e| (StatusCode::INTERNAL_SERVER_ERROR, e.to_string()))?;

    Ok(Json(WeaknessListResponse { weaknesses }))
}
```

---

## Integration Example

### **Complete Flow: Scan → Store → Analyze → Report**

```rust
// Pseudocode for complete integration

ASYNC FUNCTION complete_security_assessment(targets: Vec<String>):
    // 1. The Infiltration - Start scan
    job = the_infiltrator.the_infiltration(targets, default_config()).await?

    // 2. The Archive - Store job
    the_archive.inscribe_infiltration(&job)?

    // 3. The Observation - Monitor progress
    WHILE job.status != JobStatus::Completed:
        progress = the_infiltrator.the_observation(job.id).await?
        SLEEP(10 seconds)

    // 4. The Revelation - Get results
    results = the_infiltrator.the_revelation(job.id, None).await?

    // 5. The Archive - Store weaknesses
    FOR weakness in results.weaknesses:
        the_archive.inscribe_weakness(&weakness)?

    // 6. The Consciousness - AI analysis
    FOR weakness in results.weaknesses:
        IF weakness.severity >= 7.0:
            assessment = the_consciousness.the_assessment(weakness).await?
            the_archive.inscribe_assessment(&assessment)?

    // 7. The Propagandist - Generate report
    manifesto = the_propagandist.the_manifesto(
        job.id,
        ReportType::Comprehensive,
        ReportOptions::default()
    ).await?

    // 8. The Flow - Trigger Node-RED workflow
    trigger_nodered_flow("scan-complete", {
        "job_id": job.id,
        "weaknesses_count": results.weaknesses.len(),
        "critical_count": results.weaknesses.filter(|w| w.severity >= 9.0).count(),
        "report_url": manifesto.download_url,
    }).await?

    RETURN manifesto
END FUNCTION
```

---

## Conclusion

The data architecture of CRYPTEX combines:
- **The Archive (redb)**: Fast, embedded, reliable data storage
- **The Flow (Node-RED)**: Visual workflow automation and integration
- **The Gateways (Custom APIs)**: Comprehensive REST API for external integration

Together, these components create a flexible, powerful, and autonomous security assessment platform.

---

*"Data flows freely, stored securely, accessed transparently."*
*- Anarchist Rust Collective, 2025*
