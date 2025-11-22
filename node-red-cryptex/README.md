# Node-RED CRYPTEX Nodes

Custom Node-RED nodes for integrating CRYPTEX vulnerability assessment platform into Node-RED workflows.

## Installation

### Via npm (when published)

```bash
cd ~/.node-red
npm install @cryptex/node-red-cryptex
```

### Local Installation

```bash
cd ~/.node-red
npm install /path/to/build_openvas/node-red-cryptex
```

### In Node-RED UI

1. Open Node-RED (http://localhost:1880)
2. Click the menu icon (top right) → Manage palette
3. Go to the "Install" tab
4. Search for "@cryptex/node-red-cryptex"
5. Click "Install"

## Available Nodes

### 1. CRYPTEX Config

Configuration node that stores CRYPTEX connection settings.

**Properties:**
- **Mode**: `api` (REST API) or `binary` (local binary)
- **API URL**: CRYPTEX REST API endpoint (default: http://localhost:8080)
- **Binary Path**: Path to CRYPTEX binary executable
- **Data Directory**: Path to CRYPTEX data directory

### 2. CRYPTEX Assess

Assess vulnerability risk for a given CVE ID.

**Input:**
```json
{
  "cveId": "CVE-2024-1234"
}
```

**Output:**
```json
{
  "payload": {
    "cve_id": "CVE-2024-1234",
    "severity": "Critical",
    "cvss_base_score": 9.8,
    "is_kev": true,
    "composite_risk_score": 95.0,
    "description": "..."
  },
  "cveId": "CVE-2024-1234",
  "riskScore": 95.0,
  "severity": "Critical",
  "isKEV": true
}
```

**Properties:**
- **CRYPTEX Config**: Reference to CRYPTEX Config node
- **CVE ID**: Default CVE ID (can be overridden by msg.cveId)

### 3. CRYPTEX Scan

Start and monitor vulnerability scans.

**Actions:**

**Start Scan:**
```json
{
  "action": "start",
  "target": "192.168.1.0/24"
}
```

Output:
```json
{
  "payload": {
    "scan_id": "abc123"
  },
  "scanId": "abc123"
}
```

**Get Status:**
```json
{
  "action": "status",
  "scanId": "abc123"
}
```

**End Scan:**
```json
{
  "action": "end",
  "scanId": "abc123"
}
```

**Get Results:**
```json
{
  "action": "results",
  "scanId": "abc123"
}
```

**Properties:**
- **CRYPTEX Config**: Reference to CRYPTEX Config node
- **Action**: Default action (start/status/end/results)
- **Target**: Default scan target

### 4. CRYPTEX Report

Generate vulnerability reports.

**Input:**
```json
{
  "scanId": "abc123",
  "format": "html"
}
```

**Output:**
```json
{
  "payload": "<!DOCTYPE html>...",
  "format": "html",
  "scanId": "abc123"
}
```

**Properties:**
- **CRYPTEX Config**: Reference to CRYPTEX Config node
- **Format**: Report format (json/html/markdown)

### 5. CRYPTEX KEV

Fetch and monitor CISA Known Exploited Vulnerabilities catalog.

**Input:**
```json
{
  "trigger": true
}
```

**Output:**
```json
{
  "payload": [
    {
      "cveId": "CVE-2024-1234",
      "vendorProject": "Microsoft",
      "product": "Windows",
      "vulnerabilityName": "...",
      "dateAdded": "2024-01-15",
      "requiredAction": "...",
      "dueDate": "2024-02-15"
    }
  ],
  "totalKEV": 1000,
  "catalogVersion": "2024.01.15",
  "dateReleased": "2024-01-15T00:00:00.000Z"
}
```

## Example Flows

### Basic Vulnerability Assessment

```json
[
  {
    "id": "inject1",
    "type": "inject",
    "name": "Trigger",
    "payload": "{\"cveId\":\"CVE-2024-1234\"}",
    "payloadType": "json",
    "wires": [["assess1"]]
  },
  {
    "id": "assess1",
    "type": "cryptex-assess",
    "name": "Assess CVE",
    "cryptex": "config1",
    "wires": [["debug1"]]
  },
  {
    "id": "debug1",
    "type": "debug",
    "name": "Show Result"
  },
  {
    "id": "config1",
    "type": "cryptex-config",
    "mode": "api",
    "apiUrl": "http://localhost:8080"
  }
]
```

### Scheduled Network Scan

```json
[
  {
    "id": "schedule1",
    "type": "inject",
    "name": "Daily Scan",
    "repeat": "86400",
    "payload": "{\"target\":\"192.168.1.0/24\"}",
    "payloadType": "json",
    "wires": [["scan1"]]
  },
  {
    "id": "scan1",
    "type": "cryptex-scan",
    "name": "Start Scan",
    "cryptex": "config1",
    "action": "start",
    "wires": [["delay1"]]
  },
  {
    "id": "delay1",
    "type": "delay",
    "name": "Wait 5min",
    "pauseType": "delay",
    "timeout": "300",
    "timeoutUnits": "seconds",
    "wires": [["scan2"]]
  },
  {
    "id": "scan2",
    "type": "cryptex-scan",
    "name": "Get Results",
    "cryptex": "config1",
    "action": "results",
    "wires": [["report1"]]
  },
  {
    "id": "report1",
    "type": "cryptex-report",
    "name": "Generate Report",
    "cryptex": "config1",
    "format": "html",
    "wires": [["file1"]]
  },
  {
    "id": "file1",
    "type": "file",
    "name": "Save Report",
    "filename": "/reports/scan-report.html"
  }
]
```

### KEV Monitoring with Alerts

```json
[
  {
    "id": "schedule1",
    "type": "inject",
    "name": "Daily KEV Check",
    "repeat": "86400",
    "wires": [["kev1"]]
  },
  {
    "id": "kev1",
    "type": "cryptex-kev",
    "name": "Fetch KEV Catalog",
    "cryptex": "config1",
    "wires": [["filter1"]]
  },
  {
    "id": "filter1",
    "type": "function",
    "name": "Filter Critical",
    "func": "const critical = msg.payload.filter(v => v.requiredAction.includes('patch'));\nmsg.payload = critical;\nreturn msg;",
    "wires": [["notify1"]]
  },
  {
    "id": "notify1",
    "type": "email",
    "name": "Send Alert Email",
    "wires": []
  }
]
```

## Integration with CRYPTEX API

### Start the CRYPTEX REST API

**Using Docker:**
```bash
docker-compose up -d cryptex-api
```

**Using Binary:**
```bash
cd cryptex
cargo build --release --bin the_interface_server
./target/release/the_interface_server
```

**Windows:**
```powershell
cd cryptex
cargo build --release --bin the_interface_server
.\target\release\the_interface_server.exe
```

### Configure Node-RED

1. Drag a "CRYPTEX Config" node onto the flow
2. Double-click to configure:
   - Mode: `api`
   - API URL: `http://localhost:8080`
3. Deploy the flow

## Platform Support

- **Linux**: Full support (x86_64, ARM64)
- **macOS**: Full support (Intel, Apple Silicon)
- **Windows**: Full support (x86_64)

## Requirements

- Node.js 14.0 or higher
- Node-RED 3.0 or higher
- CRYPTEX REST API running (or binary in PATH)

## Troubleshooting

### Connection Errors

```
Error: connect ECONNREFUSED 127.0.0.1:8080
```

**Solution**: Ensure CRYPTEX API is running:
```bash
# Check if API is running
curl http://localhost:8080/health

# Start API if needed
docker-compose up -d cryptex-api
```

### Binary Not Found

```
Error: spawn the_interface_server ENOENT
```

**Solution**: Set correct binary path in CRYPTEX Config node, or add to PATH:

**Linux/macOS:**
```bash
export PATH=$PATH:$HOME/.cryptex/bin
```

**Windows:**
```powershell
$env:Path += ";C:\Program Files\CRYPTEX\bin"
```

## Development

### Running Tests

```bash
npm test
```

### Publishing

```bash
npm version patch
npm publish
```

## License

MIT - See LICENSE file in repository root

## Support

- GitHub Issues: https://github.com/Ununp3ntium115/build_openvas/issues
- CRYPTEX Documentation: See main README.md
