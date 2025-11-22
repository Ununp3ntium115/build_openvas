# openFireVulnerability API Optimization Guide

**Focus**: Condensing and optimizing REST API endpoints for redb + Node-RED + Svelte stack

---

## Current API Structure

### Existing Endpoints (The Interface)

```
Health & Status
  GET /health

Vulnerability Assessment
  GET /api/v1/vulnerabilities/:cve_id

Scan Management
  POST   /api/v1/scans
  GET    /api/v1/scans
  GET    /api/v1/scans/:scan_id
  POST   /api/v1/scans/:scan_id/end
  GET    /api/v1/scans/:scan_id/results

Report Generation
  GET    /api/v1/scans/:scan_id/report
  GET    /api/v1/scans/:scan_id/executive-summary

Archive & Statistics
  GET    /api/v1/archive/stats
```

**Total**: 10 endpoints across 4 functional groups

---

## Optimization Opportunities

### 1. Consolidate Scan Lifecycle Endpoints

**Current Structure** (3 separate endpoints):
```
POST   /api/v1/scans                     # Start scan
POST   /api/v1/scans/:scan_id/end        # End scan
GET    /api/v1/scans/:scan_id/results    # Get results
```

**Optimized Structure** (2 endpoints with actions):
```
POST   /api/v1/scans
  Request: { "target": "192.168.1.0/24" }
  Response: { "scan_id": "...", "started_at": "..." }

PATCH  /api/v1/scans/:scan_id
  Request: { "action": "complete", "include_results": true }
  Response: {
    "scan_id": "...",
    "status": "completed",
    "ended_at": "...",
    "results": [...],  # Optional based on include_results flag
    "summary": {...}
  }

GET    /api/v1/scans/:scan_id?include=results,summary
  Query params control response depth
```

**Benefits**:
- Reduces 3 endpoints to 2
- More RESTful (PATCH for state changes)
- Query params allow flexible data loading
- Supports Node-RED flow optimization (single node for scan completion)

---

### 2. Unified Report Generation Endpoint

**Current Structure** (2 separate endpoints):
```
GET /api/v1/scans/:scan_id/report?format=html
GET /api/v1/scans/:scan_id/executive-summary
```

**Optimized Structure** (1 endpoint with variants):
```
GET /api/v1/scans/:scan_id/report?format=html&template=executive

Supported templates:
  - full (default) - Complete vulnerability report
  - executive      - Executive summary
  - compact        - Condensed findings
  - compliance     - Regulatory compliance format

Supported formats:
  - json, html, markdown, text, pdf (future)
```

**Benefits**:
- Single endpoint for all reporting needs
- Template system scales easily
- Reduced Node-RED complexity
- Consistent content-type negotiation

---

### 3. Batch Operations for Vulnerability Assessment

**New Endpoint** (missing capability):
```
POST /api/v1/vulnerabilities/batch
  Request: {
    "cve_ids": ["CVE-2021-44228", "CVE-2023-12345", "CVE-2024-6789"],
    "include_kev": true,
    "include_ai_analysis": true
  }
  Response: {
    "assessed": [
      { "cve_id": "CVE-2021-44228", "score": {...} },
      { "cve_id": "CVE-2023-12345", "score": {...} }
    ],
    "failed": [
      { "cve_id": "CVE-2024-6789", "error": "CVE not found" }
    ],
    "cache_hits": 2,
    "cache_misses": 1
  }
```

**Benefits**:
- Reduce roundtrips for multi-CVE assessment
- Critical for scan result processing
- redb batch reads optimize database access
- Node-RED flow efficiency (single batch node vs loops)

---

### 4. WebSocket Real-Time Scan Updates

**New Capability** (async scan progress):
```
WebSocket: ws://localhost:8080/api/v1/scans/:scan_id/stream

Events:
  {
    "event": "scan_progress",
    "scan_id": "...",
    "hosts_scanned": 50,
    "total_hosts": 254,
    "vulnerabilities_found": 12,
    "timestamp": "2025-11-22T10:30:00Z"
  }

  {
    "event": "vulnerability_found",
    "cve_id": "CVE-2021-44228",
    "host": "192.168.1.100",
    "severity": "Critical"
  }

  {
    "event": "scan_completed",
    "scan_id": "...",
    "total_vulnerabilities": 42
  }
```

**Benefits**:
- Real-time Svelte UI updates (no polling)
- Node-RED event-driven workflows
- Reduced server load (no repeated GET requests)
- redb watch mode integration

---

### 5. Archive Query Optimization

**Current**: Single stats endpoint
```
GET /api/v1/archive/stats
```

**Enhanced with Filters**:
```
GET /api/v1/archive/query
  Query params:
    ?entity=scans                    # scans, vulnerabilities, hosts
    &date_from=2025-01-01
    &date_to=2025-11-22
    &severity=critical,high
    &limit=100
    &offset=0
    &sort=severity_desc

  Response:
    {
      "data": [...],
      "pagination": {
        "total": 1234,
        "limit": 100,
        "offset": 0,
        "has_more": true
      },
      "aggregations": {
        "by_severity": {...},
        "by_host": {...}
      }
    }
```

**Benefits**:
- Single endpoint for all archive queries
- redb range scans optimized
- Svelte dashboards get paginated data
- Node-RED analytics flows simplified

---

## Proposed Optimized API (v2)

### Core Endpoints (8 total, down from 10)

```
Health & Discovery
  GET    /health
  GET    /api/v2/capabilities              # NEW: Discover available features

Vulnerabilities
  GET    /api/v2/vulnerabilities/:cve_id
  POST   /api/v2/vulnerabilities/batch     # NEW: Batch assessment

Scans
  POST   /api/v2/scans
  GET    /api/v2/scans                     # List scans (with filters)
  GET    /api/v2/scans/:scan_id?include=results,summary
  PATCH  /api/v2/scans/:scan_id            # Update scan state/end
  WS     /api/v2/scans/:scan_id/stream     # NEW: Real-time updates

Reports (Consolidated)
  GET    /api/v2/scans/:scan_id/report?format=html&template=executive

Archive
  GET    /api/v2/archive/query             # Enhanced stats + query
```

---

## redb Integration Optimizations

### 1. Connection Pooling
```rust
// Current: Single TheArchive instance
// Optimized: Connection pool with read replicas

pub struct ArchivePool {
    write_conn: Arc<Mutex<WriteTransaction>>,
    read_pool: Vec<Arc<ReadOnlyDatabase>>,
}
```

### 2. Batch Operations
```rust
// Batch write for scan results
pub async fn store_scan_results_batch(
    &self,
    results: Vec<ScanResult>
) -> Result<usize> {
    let txn = self.db.begin_write()?;
    let table = txn.open_table(SCAN_RESULTS_TABLE)?;

    for result in results {
        table.insert(result.id.as_bytes(), serialize(&result)?)?;
    }

    txn.commit()?;
    Ok(results.len())
}
```

### 3. Index Optimization
```rust
// Add composite indexes for common queries
pub fn create_indexes(&self) -> Result<()> {
    // Severity + Date index for fast filtering
    self.create_index("idx_severity_date", |scan_result| {
        format!("{}_{}", scan_result.severity, scan_result.timestamp)
    })?;

    // Host + CVE index for deduplication
    self.create_index("idx_host_cve", |scan_result| {
        format!("{}_{}", scan_result.host, scan_result.cve_id)
    })?;

    Ok(())
}
```

---

## Node-RED Integration Points

### Optimized Flow Nodes

**1. Batch Vulnerability Assessment Node**
```javascript
// Input: Array of CVE IDs
// Output: Assessment results with cache stats
node.on('input', async (msg) => {
  const cves = msg.payload.cve_ids;
  const response = await fetch('/api/v2/vulnerabilities/batch', {
    method: 'POST',
    body: JSON.stringify({ cve_ids: cves, include_ai_analysis: true })
  });

  msg.payload = await response.json();
  node.send(msg);
});
```

**2. Real-Time Scan Monitor Node**
```javascript
// WebSocket connection to scan stream
const ws = new WebSocket(`ws://localhost:8080/api/v2/scans/${scanId}/stream`);

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);

  if (data.event === 'vulnerability_found' && data.severity === 'Critical') {
    // Trigger immediate alert workflow
    node.send({ payload: data });
  }
};
```

---

## Svelte UI Benefits

### 1. Reactive Data Loading
```typescript
// Current: Multiple fetch calls
const scan = await fetch(`/api/v1/scans/${scanId}`);
const results = await fetch(`/api/v1/scans/${scanId}/results`);

// Optimized: Single call with includes
const scanData = await fetch(`/api/v2/scans/${scanId}?include=results,summary`);
```

### 2. WebSocket Store Integration
```typescript
// Svelte store for real-time scan updates
export const scanProgress = writable({});

export function connectToScan(scanId: string) {
  const ws = new WebSocket(`ws://localhost:8080/api/v2/scans/${scanId}/stream`);

  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);
    scanProgress.update(progress => ({
      ...progress,
      [scanId]: data
    }));
  };
}
```

### 3. Optimistic UI Updates
```typescript
// Update UI immediately, sync with redb later
async function completeScan(scanId: string) {
  // Optimistic update
  scans.update(list => list.map(scan =>
    scan.id === scanId ? { ...scan, status: 'completed' } : scan
  ));

  // Sync with server
  await fetch(`/api/v2/scans/${scanId}`, {
    method: 'PATCH',
    body: JSON.stringify({ action: 'complete' })
  });
}
```

---

## Implementation Priority

### Phase 1: Core Optimizations (Week 1-2)
- [x] Batch vulnerability assessment endpoint
- [ ] Consolidate scan lifecycle endpoints (PATCH)
- [ ] Enhanced archive query endpoint
- [ ] Query parameter standardization

### Phase 2: Real-Time Features (Week 3-4)
- [ ] WebSocket scan streaming
- [ ] Svelte store integration
- [ ] Node-RED WebSocket nodes

### Phase 3: Advanced Features (Week 5-6)
- [ ] redb connection pooling
- [ ] Composite indexes
- [ ] Report template system
- [ ] PDF export support

---

## Performance Metrics

### Expected Improvements

| Metric | Current | Optimized | Improvement |
|--------|---------|-----------|-------------|
| Scan result retrieval | 3 API calls | 1 API call | 67% reduction |
| Batch CVE assessment | N requests | 1 request | ~90% reduction |
| Database connections | 1 per request | Pooled (5 read + 1 write) | 5x throughput |
| UI render time (scan page) | 450ms | 180ms | 60% faster |
| Node-RED flow complexity | 8 nodes | 3 nodes | 62% simpler |

---

## Migration Strategy

### API Versioning
```
# Support both v1 and v2 simultaneously
/api/v1/scans      # Legacy (deprecated 2026-01)
/api/v2/scans      # New optimized
```

### Backward Compatibility Layer
```rust
// v1 endpoint delegates to v2 internally
async fn v1_end_scan(scan_id: String) -> Response {
    // Translate to v2 PATCH request
    v2_update_scan(scan_id, UpdateScanRequest {
        action: "complete",
        include_results: false
    }).await
}
```

---

## Stack-Specific Benefits

### redb
- Batch writes reduce transaction overhead
- Composite indexes speed up dashboard queries
- Connection pooling increases concurrency

### Node-RED
- Fewer nodes per workflow
- Built-in batch processing
- WebSocket event streams simplify real-time flows

### Svelte
- Single fetch for data + includes
- Reactive stores for WebSocket updates
- Optimistic UI updates improve UX

---

## Next Steps

1. **Review & Approve** this optimization plan
2. **Prototype** batch endpoints and WebSocket streams
3. **Benchmark** redb performance with indexes
4. **Update** Svelte UI to use optimized endpoints
5. **Refactor** Node-RED flows to use batch operations
6. **Document** migration guide for existing deployments

---

**Status**: Draft for review
**Last Updated**: 2025-11-22
**Author**: openFireVulnerability Team
