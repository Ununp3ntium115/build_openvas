# CRYPTEX REST API Reference

Complete API documentation for The Interface REST API server.

**Base URL**: `http://localhost:8080` (configurable)
**API Version**: v1
**Protocol**: HTTP/JSON
**Authentication**: Bearer token (when enabled)

---

## Table of Contents

- [Authentication](#authentication)
- [Error Handling](#error-handling)
- [Endpoints](#endpoints)
  - [Health Check](#health-check)
  - [Vulnerability Assessment](#vulnerability-assessment)
  - [Scan Management](#scan-management)
  - [Report Generation](#report-generation)
  - [Archive Statistics](#archive-statistics)

---

## Authentication

When authentication is enabled, include a Bearer token in the Authorization header:

```http
Authorization: Bearer <your-token>
```

Example:
```bash
curl -H "Authorization: Bearer eyJ0eXAiOiJKV1QiLCJhbGc..." \
  http://localhost:8080/api/v1/scans
```

---

## Error Handling

### Error Response Format

All errors return a standard JSON structure:

```json
{
  "error": "404 Not Found",
  "message": "Scan not found"
}
```

### HTTP Status Codes

| Code | Meaning | When It Occurs |
|------|---------|---------------|
| 200 | OK | Request succeeded |
| 201 | Created | Resource created successfully |
| 400 | Bad Request | Invalid request parameters |
| 401 | Unauthorized | Missing or invalid authentication |
| 404 | Not Found | Resource doesn't exist |
| 500 | Internal Server Error | Server-side error |

---

## Endpoints

### Health Check

Check if the server is running and healthy.

#### `GET /health`

**Response:**
```json
{
  "status": "healthy",
  "version": "1.0.0"
}
```

**Example:**
```bash
curl http://localhost:8080/health
```

---

### Vulnerability Assessment

Assess CVE vulnerabilities with comprehensive scoring.

#### `GET /api/v1/vulnerabilities/:cve_id`

Retrieve detailed vulnerability assessment for a CVE.

**Parameters:**
- `cve_id` (path) - CVE identifier (e.g., `CVE-2021-44228`)

**Response:**
```json
{
  "cve_id": "CVE-2021-44228",
  "severity": "Critical",
  "cvss_base_score": 10.0,
  "cvss_vector": "CVSS:3.1/AV:N/AC:L/PR:N/UI:N/S:C/C:H/I:H/A:H",
  "is_kev": true,
  "kev_date_added": "2021-12-10",
  "epss_score": 0.97568,
  "epss_percentile": 0.99999,
  "composite_risk_score": 1.0,
  "ai_risk_score": 10.0,
  "ai_priority": "IMMEDIATE",
  "remediation_urgency": "Patch within 24 hours",
  "description": "Apache Log4j2 2.0-beta9 through 2.15.0 JNDI features...",
  "ai_analysis": "This is a critical remote code execution vulnerability..."
}
```

**Example:**
```bash
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2021-44228
```

**Notes:**
- Results are cached in The Archive for performance
- First request may be slower (fetches from NVD/KEV/EPSS)
- AI analysis requires configured AI provider (OpenAI or Claude)

---

### Scan Management

Create, monitor, and manage vulnerability scans.

#### `POST /api/v1/scans`

Start a new vulnerability scan.

**Request Body:**
```json
{
  "target": "192.168.1.0/24"
}
```

**Response:** (201 Created)
```json
{
  "scan_id": "550e8400-e29b-41d4-a716-446655440000",
  "target": "192.168.1.0/24",
  "started_at": "2025-11-22T10:30:00Z"
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/api/v1/scans \
  -H "Content-Type: application/json" \
  -d '{"target": "192.168.1.0/24"}'
```

---

#### `GET /api/v1/scans`

List all scans (sorted by most recent).

**Response:**
```json
[
  {
    "scan_id": "550e8400-e29b-41d4-a716-446655440000",
    "target": "192.168.1.0/24",
    "started_at": "2025-11-22T10:30:00Z",
    "ended_at": "2025-11-22T11:15:00Z",
    "status": "completed",
    "total_vulnerabilities": 42,
    "critical": 3,
    "high": 12,
    "medium": 20,
    "low": 7
  },
  {
    "scan_id": "660e8400-e29b-41d4-a716-446655440001",
    "target": "10.0.0.0/8",
    "started_at": "2025-11-21T14:00:00Z",
    "ended_at": null,
    "status": "running",
    "total_vulnerabilities": 0,
    "critical": 0,
    "high": 0,
    "medium": 0,
    "low": 0
  }
]
```

**Example:**
```bash
curl http://localhost:8080/api/v1/scans
```

---

#### `GET /api/v1/scans/:scan_id`

Get metadata for a specific scan.

**Parameters:**
- `scan_id` (path) - Scan UUID

**Response:**
```json
{
  "scan_id": "550e8400-e29b-41d4-a716-446655440000",
  "target": "192.168.1.0/24",
  "started_at": "2025-11-22T10:30:00Z",
  "ended_at": "2025-11-22T11:15:00Z",
  "status": "completed",
  "total_vulnerabilities": 42,
  "critical": 3,
  "high": 12,
  "medium": 20,
  "low": 7
}
```

**Example:**
```bash
curl http://localhost:8080/api/v1/scans/550e8400-e29b-41d4-a716-446655440000
```

**Error Responses:**
- `404 Not Found` - Scan doesn't exist

---

#### `POST /api/v1/scans/:scan_id/end`

Complete a scan and generate final report.

**Parameters:**
- `scan_id` (path) - Scan UUID

**Response:**
```json
{
  "scan_id": "550e8400-e29b-41d4-a716-446655440000",
  "target": "192.168.1.0/24",
  "start_time": 1700654400,
  "end_time": 1700657100,
  "status": "Completed",
  "scan_results": [
    {
      "cve_id": "CVE-2021-44228",
      "host": "192.168.1.100",
      "port": 8080,
      "plugin_oid": "1.3.6.1.4.1.25623.1.0.12345",
      "description": "Apache Log4j RCE vulnerability detected",
      "vulnerability_score": {
        "cve_id": "CVE-2021-44228",
        "severity": "Critical",
        "cvss_base_score": 10.0,
        "is_kev": true
      },
      "remediation_guidance": "Update Log4j to version 2.17.1 or later...",
      "detection_time": 1700654500,
      "scanner_version": "CRYPTEX 1.0.0"
    }
  ],
  "total_vulnerabilities": 42,
  "critical_count": 3,
  "high_count": 12,
  "medium_count": 20,
  "low_count": 7,
  "kev_count": 5,
  "total_hosts": 254,
  "ai_enhanced_count": 42,
  "summary": null
}
```

**Example:**
```bash
curl -X POST http://localhost:8080/api/v1/scans/550e8400-e29b-41d4-a716-446655440000/end
```

---

#### `GET /api/v1/scans/:scan_id/results`

Get all vulnerability results for a scan.

**Parameters:**
- `scan_id` (path) - Scan UUID

**Response:**
```json
[
  {
    "cve_id": "CVE-2021-44228",
    "host": "192.168.1.100",
    "port": 8080,
    "plugin_oid": "1.3.6.1.4.1.25623.1.0.12345",
    "description": "Apache Log4j RCE vulnerability detected",
    "vulnerability_score": {
      "cve_id": "CVE-2021-44228",
      "severity": "Critical",
      "cvss_base_score": 10.0
    },
    "remediation_guidance": "Update Log4j to version 2.17.1 or later...",
    "detection_time": 1700654500,
    "scanner_version": "CRYPTEX 1.0.0"
  },
  {
    "cve_id": "CVE-2023-12345",
    "host": "192.168.1.101",
    "port": 443,
    "plugin_oid": "1.3.6.1.4.1.25623.1.0.67890",
    "description": "SSL/TLS vulnerability detected",
    "vulnerability_score": {
      "cve_id": "CVE-2023-12345",
      "severity": "High",
      "cvss_base_score": 7.5
    },
    "remediation_guidance": null,
    "detection_time": 1700654600,
    "scanner_version": "CRYPTEX 1.0.0"
  }
]
```

**Example:**
```bash
curl http://localhost:8080/api/v1/scans/550e8400-e29b-41d4-a716-446655440000/results
```

---

### Report Generation

Generate formatted vulnerability reports.

#### `GET /api/v1/scans/:scan_id/report`

Generate a vulnerability assessment report.

**Parameters:**
- `scan_id` (path) - Scan UUID
- `format` (query, optional) - Report format: `json` (default), `html`, `markdown`, `text`

**Response Headers:**
- `Content-Type`: Varies based on format
  - `application/json` for JSON
  - `text/html` for HTML
  - `text/markdown` for Markdown
  - `text/plain` for Text

**Response (JSON format):**
```json
{
  "scan_id": "550e8400-e29b-41d4-a716-446655440000",
  "target": "192.168.1.0/24",
  "generated_at": "2025-11-22T11:15:00Z",
  "summary": {
    "total_vulnerabilities": 42,
    "critical": 3,
    "high": 12,
    "medium": 20,
    "low": 7,
    "kev_vulnerabilities": 5
  },
  "findings": [
    {
      "cve_id": "CVE-2021-44228",
      "severity": "Critical",
      "affected_hosts": ["192.168.1.100", "192.168.1.105"],
      "cvss_score": 10.0,
      "is_kev": true,
      "remediation": "Update Apache Log4j to 2.17.1 or later"
    }
  ]
}
```

**Response (HTML format):**
```html
<!DOCTYPE html>
<html lang="en">
<head>
  <title>CRYPTEX Vulnerability Report</title>
  <style>
    .critical { color: #e74c3c; font-weight: bold; }
    .high { color: #e67e22; font-weight: bold; }
    ...
  </style>
</head>
<body>
  <h1>CRYPTEX Vulnerability Assessment Report</h1>
  <h2>Scan: 550e8400-e29b-41d4-a716-446655440000</h2>
  ...
</body>
</html>
```

**Examples:**
```bash
# JSON report
curl http://localhost:8080/api/v1/scans/550e8400.../report

# HTML report
curl http://localhost:8080/api/v1/scans/550e8400.../report?format=html

# Markdown report
curl http://localhost:8080/api/v1/scans/550e8400.../report?format=markdown

# Text report
curl http://localhost:8080/api/v1/scans/550e8400.../report?format=text
```

---

#### `GET /api/v1/scans/:scan_id/executive-summary`

Generate an executive-level summary of vulnerabilities.

**Parameters:**
- `scan_id` (path) - Scan UUID

**Response:**
```markdown
# Executive Summary

## Overview
- **Scan Target**: 192.168.1.0/24
- **Scan Date**: 2025-11-22
- **Total Vulnerabilities**: 42
- **Critical Risk Items**: 3

## Key Findings

### Critical Issues Requiring Immediate Attention

1. **CVE-2021-44228** (Log4Shell)
   - **Risk Level**: Critical
   - **CVSS Score**: 10.0
   - **Affected Systems**: 2 hosts
   - **KEV Status**: ⚠️ Known Exploited Vulnerability
   - **Action Required**: Patch within 24 hours

### High-Priority Items

- 12 high-severity vulnerabilities identified
- 5 vulnerabilities listed in CISA KEV catalog
- Average EPSS score: 0.65 (65% exploitation probability)

## Recommendations

1. **Immediate**: Address all KEV-listed vulnerabilities within 48 hours
2. **Short-term**: Remediate critical and high-severity issues within 2 weeks
3. **Long-term**: Implement continuous vulnerability scanning

## Risk Assessment

**Overall Security Posture**: Needs Improvement

The assessment identified several critical vulnerabilities that require
immediate attention. The presence of actively exploited vulnerabilities
(KEV) represents significant risk to the organization.
```

**Example:**
```bash
curl http://localhost:8080/api/v1/scans/550e8400.../executive-summary
```

---

### Archive Statistics

Query database statistics and metrics.

#### `GET /api/v1/archive/stats`

Get Archive database statistics.

**Response:**
```json
{
  "total_scans": 142,
  "total_vulnerabilities": 3582,
  "total_results": 8921
}
```

**Example:**
```bash
curl http://localhost:8080/api/v1/archive/stats
```

---

## Rate Limiting

When rate limiting is enabled:

**Limits:**
- 10 requests per second per IP
- 50 requests burst capacity

**Response Headers:**
```http
X-RateLimit-Limit: 10
X-RateLimit-Remaining: 7
X-RateLimit-Reset: 1700654500
```

**Error Response (429 Too Many Requests):**
```json
{
  "error": "429 Too Many Requests",
  "message": "Rate limit exceeded. Try again in 3 seconds."
}
```

---

## CORS

Cross-Origin Resource Sharing is enabled for all origins by default.

**Response Headers:**
```http
Access-Control-Allow-Origin: *
Access-Control-Allow-Methods: GET, POST, OPTIONS
Access-Control-Allow-Headers: Content-Type, Authorization
```

For production, configure specific origins:

```rust
let cors = CorsLayer::new()
    .allow_origin("https://dashboard.example.com".parse::<HeaderValue>().unwrap())
    .allow_methods([Method::GET, Method::POST])
    .allow_headers([CONTENT_TYPE, AUTHORIZATION]);
```

---

## Pagination

*Note: Pagination not yet implemented. All results returned in single response.*

Planned for future release:

```http
GET /api/v1/scans?page=2&per_page=50
```

---

## Filtering & Sorting

*Note: Filtering/sorting not yet implemented.*

Planned for future release:

```http
# Filter by severity
GET /api/v1/vulnerabilities?severity=critical

# Filter by KEV status
GET /api/v1/vulnerabilities?kev=true

# Sort by CVSS score
GET /api/v1/vulnerabilities?sort=cvss_desc
```

---

## WebSocket Support

*Note: WebSocket support planned for Phase 4.*

Real-time scan updates will be available via:

```javascript
const ws = new WebSocket('ws://localhost:8080/api/v1/scans/550e8400.../stream');

ws.onmessage = (event) => {
  const update = JSON.parse(event.data);
  console.log('Vulnerability found:', update);
};
```

---

## SDK Examples

### Python

```python
import requests

class CryptexClient:
    def __init__(self, base_url="http://localhost:8080"):
        self.base_url = base_url
        self.session = requests.Session()

    def assess_vulnerability(self, cve_id):
        """Assess a CVE vulnerability"""
        url = f"{self.base_url}/api/v1/vulnerabilities/{cve_id}"
        response = self.session.get(url)
        response.raise_for_status()
        return response.json()

    def start_scan(self, target):
        """Start a new scan"""
        url = f"{self.base_url}/api/v1/scans"
        response = self.session.post(url, json={"target": target})
        response.raise_for_status()
        return response.json()

    def get_scan_results(self, scan_id):
        """Get scan results"""
        url = f"{self.base_url}/api/v1/scans/{scan_id}/results"
        response = self.session.get(url)
        response.raise_for_status()
        return response.json()

# Usage
client = CryptexClient()

# Assess Log4Shell
vuln = client.assess_vulnerability("CVE-2021-44228")
print(f"Severity: {vuln['severity']}, CVSS: {vuln['cvss_base_score']}")

# Start scan
scan = client.start_scan("192.168.1.0/24")
print(f"Scan started: {scan['scan_id']}")

# Get results
results = client.get_scan_results(scan['scan_id'])
print(f"Found {len(results)} vulnerabilities")
```

### JavaScript/TypeScript

```typescript
class CryptexClient {
  constructor(private baseUrl: string = 'http://localhost:8080') {}

  async assessVulnerability(cveId: string) {
    const response = await fetch(
      `${this.baseUrl}/api/v1/vulnerabilities/${cveId}`
    );
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return response.json();
  }

  async startScan(target: string) {
    const response = await fetch(`${this.baseUrl}/api/v1/scans`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ target })
    });
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return response.json();
  }

  async getScanResults(scanId: string) {
    const response = await fetch(
      `${this.baseUrl}/api/v1/scans/${scanId}/results`
    );
    if (!response.ok) throw new Error(`HTTP ${response.status}`);
    return response.json();
  }
}

// Usage
const client = new CryptexClient();

const vuln = await client.assessVulnerability('CVE-2021-44228');
console.log(`Severity: ${vuln.severity}, CVSS: ${vuln.cvss_base_score}`);

const scan = await client.startScan('192.168.1.0/24');
console.log(`Scan started: ${scan.scan_id}`);
```

### Rust

```rust
use reqwest::Client;
use serde::{Deserialize, Serialize};

#[derive(Debug, Serialize)]
struct StartScanRequest {
    target: String,
}

#[derive(Debug, Deserialize)]
struct VulnerabilityScore {
    cve_id: String,
    severity: String,
    cvss_base_score: f64,
    is_kev: bool,
}

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    let client = Client::new();
    let base_url = "http://localhost:8080";

    // Assess vulnerability
    let vuln: VulnerabilityScore = client
        .get(format!("{}/api/v1/vulnerabilities/CVE-2021-44228", base_url))
        .send()
        .await?
        .json()
        .await?;

    println!("Severity: {}, CVSS: {}", vuln.severity, vuln.cvss_base_score);

    // Start scan
    let scan_request = StartScanRequest {
        target: "192.168.1.0/24".to_string(),
    };

    let scan_response = client
        .post(format!("{}/api/v1/scans", base_url))
        .json(&scan_request)
        .send()
        .await?
        .json::<serde_json::Value>()
        .await?;

    println!("Scan started: {}", scan_response["scan_id"]);

    Ok(())
}
```

---

## Additional Resources

- **MCP Integration**: See [PYRO_INTEGRATION.md](PYRO_INTEGRATION.md)
- **Deployment**: See [DEPLOYMENT.md](DEPLOYMENT.md)
- **Architecture**: See [README.md](README.md)

---

**"No gods, no masters, no memory leaks."**

*CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience*
