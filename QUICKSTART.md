# CRYPTEX Quick Start Guide

Get up and running with CRYPTEX in under 5 minutes.

## 🚀 Installation (Pick Your Platform)

### Linux / macOS

```bash
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas
chmod +x install-cryptex.sh
./install-cryptex.sh
exec $SHELL
```

### Windows

```powershell
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas
.\install.ps1
# Restart PowerShell
```

## ✅ Verify Installation

```bash
# Check if installed
cryptex-api --version  # or: the_interface_server --version

# Check paths
which cryptex-api      # Linux/macOS
Get-Command cryptex-api  # Windows
```

## 🎯 Choose Your Mode

### Mode 1: Standalone API Server

**Start the server:**
```bash
cryptex-api
```

**Test it:**
```bash
# Health check
curl http://localhost:8080/health

# Assess a CVE
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2024-21413

# Start a scan
curl -X POST http://localhost:8080/api/v1/scans \
  -H "Content-Type: application/json" \
  -d '{"target":"192.168.1.1"}'

# Get archive stats
curl http://localhost:8080/api/v1/archive/stats
```

**Access the UI:**
```bash
cd cryptex-visualizer
npm install
npm run dev
# Open http://localhost:5173
```

### Mode 2: With Node-RED

**Install Node-RED (if needed):**
```bash
npm install -g node-red
```

**Install CRYPTEX nodes:**
```bash
cd ~/.node-red
npm install /path/to/build_openvas/node-red-cryptex
```

**Start Node-RED:**
```bash
node-red
# Open http://localhost:1880
```

**Create a simple flow:**

1. Drag **Inject** node onto canvas
2. Drag **cryptex-assess** node
3. Drag **debug** node
4. Connect: Inject → CRYPTEX Assess → Debug
5. Configure **cryptex-assess**:
   - CRYPTEX Config: Add new (API mode, http://localhost:8080)
   - CVE ID: `CVE-2024-21413`
6. Click **Deploy**
7. Click Inject button
8. Check debug panel for results

### Mode 3: Docker Stack

**Start everything:**
```bash
docker-compose up -d
```

**Access services:**
- API: http://localhost:8080
- UI: http://localhost:5173
- Node-RED: http://localhost:1880

**Check status:**
```bash
docker-compose ps
docker-compose logs -f cryptex-api
```

## 📝 Common Tasks

### Assess a Single Vulnerability

**Using API:**
```bash
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2024-21413 | jq
```

**Using Node-RED:**
1. Add **cryptex-assess** node
2. Set CVE ID
3. Connect to debug
4. Deploy and inject

**Expected Output:**
```json
{
  "cve_id": "CVE-2024-21413",
  "severity": "Critical",
  "cvss_base_score": 9.8,
  "is_kev": true,
  "composite_risk_score": 95.0,
  "description": "..."
}
```

### Run a Network Scan

**Using API:**
```bash
# Start scan
SCAN_ID=$(curl -X POST http://localhost:8080/api/v1/scans \
  -H "Content-Type: application/json" \
  -d '{"target":"192.168.1.0/24"}' | jq -r '.scan_id')

echo "Scan ID: $SCAN_ID"

# Check status
curl http://localhost:8080/api/v1/scans/$SCAN_ID | jq

# End scan (when done)
curl -X POST http://localhost:8080/api/v1/scans/$SCAN_ID/end | jq

# Get results
curl http://localhost:8080/api/v1/scans/$SCAN_ID/results | jq
```

**Using Node-RED:**
1. **Inject** → **cryptex-scan** (action: start, target: 192.168.1.0/24)
2. **cryptex-scan** → **delay** (5 minutes)
3. **delay** → **cryptex-scan** (action: results, scanId from msg)
4. **cryptex-scan** → **debug**

### Generate a Report

**Using API:**
```bash
# HTML report
curl -X POST http://localhost:8080/api/v1/reports/$SCAN_ID \
  -H "Content-Type: application/json" \
  -d '{"format":"html"}' > report.html

# JSON report
curl -X POST http://localhost:8080/api/v1/reports/$SCAN_ID \
  -H "Content-Type: application/json" \
  -d '{"format":"json"}' > report.json

# Markdown report
curl -X POST http://localhost:8080/api/v1/reports/$SCAN_ID \
  -H "Content-Type: application/json" \
  -d '{"format":"markdown"}' > report.md
```

**Using Node-RED:**
1. **Inject** with `msg.scanId`
2. **cryptex-report** (format: html)
3. **file** node (save to disk)

### Monitor CISA KEV Catalog

**Using Node-RED:**
1. **Inject** (repeat every 24 hours)
2. **cryptex-kev**
3. **function** (filter for critical)
4. **email** (send alerts)

**Flow Example:**
```javascript
// Function node: Filter critical KEVs
const critical = msg.payload.filter(kev =>
  kev.requiredAction.includes('patch immediately')
);

msg.payload = critical;
msg.subject = `${critical.length} Critical KEVs Require Attention`;
return msg;
```

## 🔧 Configuration

### Environment Variables

**Linux/macOS (~/.bashrc or ~/.zshrc):**
```bash
export CRYPTEX_DB_PATH="$HOME/.cryptex/data/cryptex.db"
export CRYPTEX_BIND_ADDR="127.0.0.1:8080"
export RUST_LOG="info"
```

**Windows (PowerShell profile):**
```powershell
$env:CRYPTEX_DB_PATH = "$env:USERPROFILE\.cryptex\data\cryptex.db"
$env:CRYPTEX_BIND_ADDR = "127.0.0.1:8080"
$env:RUST_LOG = "info"
```

### Config File

Edit `~/.cryptex/config/config.toml`:

```toml
[cryptex]
data_dir = "/home/user/.cryptex/data"
db_path = "/home/user/.cryptex/data/cryptex.db"

[api]
bind_addr = "127.0.0.1:8080"
log_level = "info"
```

## 🐛 Troubleshooting

### API Won't Start

**Check if port is in use:**
```bash
# Linux/macOS
lsof -i :8080

# Windows
netstat -ano | findstr :8080
```

**Use different port:**
```bash
CRYPTEX_BIND_ADDR=127.0.0.1:9090 cryptex-api
```

### Binary Not Found

**Add to PATH:**
```bash
# Linux/macOS
export PATH="$PATH:$HOME/.cryptex/bin"

# Windows
$env:Path += ";$env:USERPROFILE\.cryptex\bin"
```

### Database Errors

**Reset database:**
```bash
rm ~/.cryptex/data/cryptex.db  # Linux/macOS
Remove-Item "$env:USERPROFILE\.cryptex\data\cryptex.db"  # Windows
```

### Node-RED Nodes Not Showing

**Check installation:**
```bash
cd ~/.node-red
npm ls @cryptex/node-red-cryptex
```

**Reinstall:**
```bash
npm uninstall @cryptex/node-red-cryptex
npm install /path/to/node-red-cryptex
```

**Restart Node-RED:**
```bash
# Stop (Ctrl+C)
# Start again
node-red
```

## 📚 Next Steps

### Learn More
- [Full Installation Guide](INSTALL.md)
- [API Reference](API_REFERENCE.md)
- [Deployment Guide](DEPLOYMENT.md)
- [Node-RED Integration](node-red-cryptex/README.md)
- [Frontend Documentation](cryptex-visualizer/README.md)

### Example Workflows

**Daily Security Scan:**
```bash
# Add to crontab
0 2 * * * cryptex-scan-daily.sh
```

**Continuous KEV Monitoring:**
- Set up Node-RED flow with 24h inject
- Filter for new KEVs
- Send email alerts

**Integration with PYRO:**
- Deploy as MCP server
- Embed UI in PYRO dashboard
- Share vulnerability data

## 🎯 Common Use Cases

### Security Assessment

```bash
# Quick vulnerability check
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2024-21413

# Network scan
curl -X POST http://localhost:8080/api/v1/scans \
  -d '{"target":"192.168.1.0/24"}'

# Generate report
curl -X POST http://localhost:8080/api/v1/reports/$SCAN_ID \
  -d '{"format":"html"}' > report.html
```

### Compliance Monitoring

**Node-RED Flow:**
1. Daily KEV catalog check
2. Filter for industry-specific vulnerabilities
3. Generate compliance report
4. Email to security team

### CI/CD Integration

```yaml
# .github/workflows/security-scan.yml
- name: CRYPTEX Security Scan
  run: |
    cryptex-api &
    sleep 5
    SCAN_ID=$(curl -X POST http://localhost:8080/api/v1/scans \
      -d '{"target":"${{ github.repository }}"}' | jq -r '.scan_id')
    curl http://localhost:8080/api/v1/scans/$SCAN_ID/results
```

## 🆘 Getting Help

- **Documentation**: See README.md and docs/
- **Issues**: GitHub Issues
- **PYRO Integration**: https://github.com/Ununp3ntium115/PYRO_Platform_Ignition.git

---

**You're ready to use CRYPTEX!** 🎉

Start with assessing a single CVE, then explore scans and reports. For automation, try Node-RED flows.
