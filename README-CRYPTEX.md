# CRYPTEX - The Anarchist Vulnerability Platform

**Cross-platform vulnerability assessment and risk analysis platform built in Rust with Svelte UI and Node-RED integration.**

[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux-blue)]()
[![Rust](https://img.shields.io/badge/rust-1.75%2B-orange)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()
[![Node-RED](https://img.shields.io/badge/Node--RED-3.0%2B-red)]()

## 🎯 Overview

CRYPTEX is a **fully cross-platform, standalone vulnerability assessment platform** that combines:

- 🦀 **Rust backend** - Fast, memory-safe, platform-agnostic
- 🎨 **Svelte frontend** - Modern reactive UI (standalone + embeddable)
- 🔄 **Node-RED integration** - Custom nodes for workflow automation
- 🔌 **MCP server** - AI agent integration via Model Context Protocol
- 📦 **redb database** - Embedded ACID-compliant storage
- 🐳 **Docker support** - Full-stack orchestration

### Key Capabilities

✅ **Platform Agnostic**: Runs natively on Windows, macOS, and Linux
✅ **Standalone**: No Docker required - single binary deployment
✅ **Node-RED Nodes**: 5 custom nodes for CRYPTEX workflows
✅ **Dual-Mode UI**: Standalone app OR embeddable in PYRO
✅ **MCP Protocol**: AI agent integration for PYRO Platform
✅ **KEV Monitoring**: CISA Known Exploited Vulnerabilities tracking
✅ **Composite Risk Scoring**: Advanced multi-factor risk analysis
✅ **Multi-Format Reports**: HTML, JSON, Markdown output

## 🚀 Quick Start

### Linux / macOS

```bash
# Clone and install
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas
./install-cryptex.sh

# Start CRYPTEX
cryptex-api

# Test
curl http://localhost:8080/health
```

### Windows

```powershell
# Clone and install
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas
.\install.ps1

# Start CRYPTEX
cryptex-api

# Test
Invoke-WebRequest http://localhost:8080/health
```

### Docker

```bash
docker-compose up -d
# Access at http://localhost:8080 (API)
# Access at http://localhost:5173 (UI)
```

## 📚 Deployment Modes

### Mode 1: Standalone Application

Run CRYPTEX as a native application on any platform.

**Use Cases:**
- Security assessment workstations
- Pentesting laptops
- CI/CD pipelines
- Air-gapped environments

**Features:**
- Single binary deployment
- Local database (no external dependencies)
- REST API server
- Web UI included
- System service integration

**Quick Start:**
```bash
cryptex-api &
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2024-1234
```

### Mode 2: Node-RED Integration

Use CRYPTEX as custom Node-RED nodes for automated workflows.

**Use Cases:**
- Scheduled vulnerability scans
- Automated reporting workflows
- KEV catalog monitoring
- Security operations automation
- Alert generation pipelines

**Features:**
- 5 custom Node-RED nodes
- Visual workflow builder
- Scheduled automation
- CISA KEV monitoring
- Report generation

**Quick Start:**
```bash
cd ~/.node-red
npm install /path/to/node-red-cryptex
node-red
# Access at http://localhost:1880
```

**Example Flow:**
```
[Inject: Hourly] → [CRYPTEX Scan] → [CRYPTEX Report] → [Email]
```

### Mode 3: PYRO Platform Integration

Embed CRYPTEX in the PYRO Platform as both an MCP server and UI component.

**Use Cases:**
- PYRO Platform deployment
- Multi-tool security platform
- AI-powered security analysis
- Integrated dashboards

**Features:**
- MCP server for AI agents
- Embeddable Svelte components
- Shared authentication
- Unified dashboard
- Cross-tool workflows

**Quick Start:**

**MCP Server (AI Agents):**
```json
{
  "mcpServers": {
    "cryptex": {
      "command": "cryptex-mcp"
    }
  }
}
```

**UI Integration (Svelte):**
```typescript
import { CryptexDashboard } from 'cryptex-visualizer';

new CryptexDashboard({
  target: document.getElementById('cryptex-widget'),
  props: {
    mode: 'embedded',
    apiUrl: 'http://cryptex-api:8080'
  }
});
```

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────────────┐
│                     CRYPTEX Platform                     │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │   Svelte UI  │  │  Node-RED    │  │  MCP Server  │  │
│  │  Visualizer  │  │  Nodes (5)   │  │  (PYRO)      │  │
│  │              │  │              │  │              │  │
│  │ • Dashboard  │  │ • Assess     │  │ • JSON-RPC   │  │
│  │ • Scans      │  │ • Scan       │  │ • STDIN/OUT  │  │
│  │ • Reports    │  │ • Report     │  │ • 5 Tools    │  │
│  │ • KEV List   │  │ • KEV        │  │              │  │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────┘  │
│         │                 │                  │          │
│         └─────────────────┴──────────────────┘          │
│                           │                             │
│                  ┌────────▼─────────┐                   │
│                  │   REST API       │                   │
│                  │   (Axum)         │                   │
│                  │                  │                   │
│                  │ • 10 Endpoints   │                   │
│                  │ • WebSocket      │                   │
│                  │ • CORS           │                   │
│                  └────────┬─────────┘                   │
│                           │                             │
│         ┌─────────────────┴─────────────────┐           │
│         │                                   │           │
│  ┌──────▼──────┐  ┌──────────┐  ┌─────────▼─────────┐ │
│  │ The Archive │  │   The    │  │  The Assessor     │ │
│  │   (redb)    │  │ Agitator │  │  (Risk Scoring)   │ │
│  │             │  │          │  │                   │ │
│  │ • Scans     │  │ • CVE    │  │ • CVSS Analysis   │ │
│  │ • Vulns     │  │ • Analyze│  │ • KEV Detection   │ │
│  │ • Results   │  │          │  │ • Composite Risk  │ │
│  └─────────────┘  └──────────┘  └───────────────────┘ │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

### Core Components (Rust Crates)

| Crate | Purpose | Status |
|-------|---------|--------|
| **the_foundation** | Core types, platform utils | ✅ Complete |
| **the_assessor** | Risk analysis engine | ✅ Complete |
| **the_infiltrator** | Vulnerability scanning | ✅ Complete |
| **the_propagandist** | Report generation | ✅ Complete |
| **the_archive** | Database layer (redb) | ✅ Complete |
| **the_interface** | REST API server | ✅ Complete |
| **the_commune** | MCP server | ✅ Complete |
| **the_agitator** | CVE analysis | ✅ Complete |
| **the_collective** | Scan orchestration | ✅ Complete |

### Frontend (Svelte + TypeScript)

| Component | Purpose | Status |
|-----------|---------|--------|
| **Dashboard** | Main overview | ✅ Complete |
| **ScanView** | Scan details | ✅ Complete |
| **VulnerabilityList** | Vuln grid | ✅ Complete |
| **ReportView** | Multi-format reports | ✅ Complete |
| **Stores** | State management | ✅ Complete |
| **API Client** | REST API wrapper | ✅ Complete |
| **WebSocket** | Real-time updates | ✅ Complete |

### Node-RED Nodes

| Node | Purpose | Status |
|------|---------|--------|
| **cryptex-config** | Configuration | ✅ Complete |
| **cryptex-assess** | Vulnerability assessment | ✅ Complete |
| **cryptex-scan** | Network scanning | ✅ Complete |
| **cryptex-report** | Report generation | ✅ Complete |
| **cryptex-kev** | KEV monitoring | ✅ Complete |

## 🔥 Features

### Vulnerability Assessment

- **CVE Analysis**: Comprehensive vulnerability analysis with CVSS scoring
- **KEV Tracking**: CISA Known Exploited Vulnerabilities monitoring
- **Composite Risk**: Multi-factor risk scoring algorithm
- **Network Scanning**: Target-based vulnerability discovery
- **Real-time Updates**: WebSocket-based scan progress

### Risk Analysis

- **CVSS Scoring**: Industry-standard vulnerability scoring
- **KEV Integration**: Known exploit detection
- **Temporal Metrics**: Time-based risk adjustment
- **Environmental Context**: Organization-specific risk factors
- **Composite Score**: Weighted multi-factor analysis

### Reporting

- **HTML Reports**: Styled, professional vulnerability reports
- **JSON Export**: Machine-readable data for integration
- **Markdown Reports**: Documentation-ready format
- **Archive Stats**: Historical analysis and trends
- **Scan History**: Complete audit trail

### Platform Support

| Platform | Architecture | Status |
|----------|--------------|--------|
| Linux | x86_64 | ✅ Tested |
| Linux | ARM64 | ✅ Supported |
| macOS | x86_64 (Intel) | ✅ Tested |
| macOS | ARM64 (Apple Silicon) | ✅ Tested |
| Windows | x86_64 | ✅ Tested |

## 📦 Installation

### Quick Install

See [INSTALL.md](INSTALL.md) for complete installation instructions.

**Linux/macOS:**
```bash
./install-cryptex.sh
```

**Windows:**
```powershell
.\install.ps1
```

### Build from Source

**Prerequisites:**
- Rust 1.75+
- Node.js 18+ (for UI)

**Build:**
```bash
cd cryptex
cargo build --release
```

**Cross-compile for all platforms:**
```bash
./build-all-platforms.sh  # Linux/macOS
.\build-all-platforms.ps1  # Windows
```

## 🎮 Usage Examples

### REST API

**Assess a vulnerability:**
```bash
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2024-1234
```

**Start a scan:**
```bash
curl -X POST http://localhost:8080/api/v1/scans \
  -H "Content-Type: application/json" \
  -d '{"target":"192.168.1.0/24"}'
```

**Generate report:**
```bash
curl -X POST http://localhost:8080/api/v1/reports/scan-123 \
  -H "Content-Type: application/json" \
  -d '{"format":"html"}'
```

### Node-RED Workflow

```javascript
// Scheduled vulnerability scan workflow
[
  {
    id: "schedule",
    type: "inject",
    repeat: "3600",  // Every hour
    wires: [["scan"]]
  },
  {
    id: "scan",
    type: "cryptex-scan",
    action: "start",
    target: "192.168.1.0/24",
    wires: [["report"]]
  },
  {
    id: "report",
    type: "cryptex-report",
    format: "html",
    wires: [["email"]]
  }
]
```

### Embedded in PYRO

```typescript
// PYRO dashboard integration
import { CryptexDashboard, CryptexAPI } from 'cryptex-visualizer';

const api = new CryptexAPI('http://cryptex-api:8080');

// Standalone widget
new CryptexDashboard({
  target: document.querySelector('#cryptex'),
  props: { mode: 'embedded' }
});

// Programmatic access
const vuln = await api.assessVulnerability('CVE-2024-1234');
console.log(`Risk: ${vuln.composite_risk_score}`);
```

## 📖 Documentation

- **[INSTALL.md](INSTALL.md)** - Complete installation guide for all platforms
- **[DEPLOYMENT.md](DEPLOYMENT.md)** - Production deployment and scaling
- **[API_REFERENCE.md](API_REFERENCE.md)** - REST API endpoint documentation
- **[node-red-cryptex/README.md](node-red-cryptex/README.md)** - Node-RED integration
- **[cryptex-visualizer/README.md](cryptex-visualizer/README.md)** - Frontend documentation

## 🛠️ Development

### Project Structure

```
build_openvas/
├── cryptex/                    # Rust backend
│   ├── crates/                 # 9 workspace crates
│   ├── Cargo.toml              # Workspace manifest
│   ├── Dockerfile.api          # API server image
│   └── Dockerfile.mcp          # MCP server image
├── cryptex-visualizer/         # Svelte frontend
│   ├── src/                    # Components, stores, services
│   ├── package.json            # NPM dependencies
│   └── vite.config.ts          # Dual build config
├── node-red-cryptex/           # Node-RED nodes
│   ├── nodes/                  # 5 custom nodes
│   └── package.json            # Node-RED plugin manifest
├── docker-compose.yml          # Full stack orchestration
├── install-cryptex.sh          # Linux/macOS installer
├── install.ps1                 # Windows installer
└── cryptex-launcher.sh         # Service manager
```

### Build Commands

```bash
# Build Rust backend
cd cryptex
cargo build --release

# Run tests
cargo test --workspace

# Build frontend
cd cryptex-visualizer
npm install
npm run build

# Build for all platforms
cd cryptex
./build-all-platforms.sh
```

### Run Development Servers

```bash
# Rust API (auto-reload)
cargo watch -x 'run --bin the_interface_server'

# Svelte UI (hot reload)
cd cryptex-visualizer
npm run dev

# Full stack
docker-compose up
```

## 🐳 Docker Deployment

### Services

| Service | Port | Purpose |
|---------|------|---------|
| cryptex-api | 8080 | REST API server |
| cryptex-mcp | - | MCP server (STDIN/STDOUT) |
| cryptex-visualizer | 5173 | Svelte frontend |
| node-red | 1880 | Workflow automation |
| nginx | 80/443 | Reverse proxy |

### Launch

```bash
# Full stack
docker-compose up -d

# Specific services
docker-compose up -d cryptex-api cryptex-visualizer

# With rebuild
docker-compose up -d --build
```

## 🔗 Integration

### PYRO Platform

CRYPTEX is designed to integrate with the [PYRO Platform](https://github.com/Ununp3ntium115/PYRO_Platform_Ignition.git):

- **MCP Server**: AI agent integration via JSON-RPC 2.0
- **Embeddable UI**: Svelte components for PYRO dashboard
- **Shared Network**: Docker network bridging
- **Unified Auth**: (planned) PYRO authentication

### Node-RED

Install CRYPTEX nodes in any Node-RED instance:

```bash
cd ~/.node-red
npm install /path/to/node-red-cryptex
```

Access 5 custom nodes:
- CRYPTEX Config
- CRYPTEX Assess
- CRYPTEX Scan
- CRYPTEX Report
- CRYPTEX KEV

## 🤝 Contributing

Contributions welcome! See the main repository for guidelines.

## 📄 License

MIT License - See LICENSE file for details.

## 🔒 Security

### Reporting Vulnerabilities

Please report security vulnerabilities via GitHub Issues with the `security` label.

### Security Features

- ✅ Memory-safe Rust implementation
- ✅ Input validation on all endpoints
- ✅ CORS configuration
- ✅ Database ACID compliance
- ✅ No credential storage
- ✅ API key support (optional)

## 📊 Project Status

| Component | Status | Version |
|-----------|--------|---------|
| Rust Backend | ✅ Complete | 1.0.0 |
| Svelte Frontend | ✅ Complete | 1.0.0 |
| Node-RED Nodes | ✅ Complete | 1.0.0 |
| MCP Server | ✅ Complete | 1.0.0 |
| Cross-Platform | ✅ Complete | 1.0.0 |
| Documentation | ✅ Complete | 1.0.0 |

## 🎯 Roadmap

### Completed ✅

- [x] Rust backend (9 crates)
- [x] REST API server
- [x] redb database integration
- [x] Svelte frontend (standalone + embeddable)
- [x] Node-RED custom nodes
- [x] MCP server for PYRO
- [x] Cross-platform support (Windows/macOS/Linux)
- [x] Docker orchestration
- [x] Complete documentation

### Future Enhancements 🚀

- [ ] WebSocket real-time scan updates
- [ ] Advanced filtering and search
- [ ] Export to PDF
- [ ] Multi-language support
- [ ] Plugin system for custom analyzers
- [ ] GraphQL API
- [ ] Kubernetes deployment
- [ ] Cloud-native scanning

## 🙏 Acknowledgments

- **OpenVAS** - Original vulnerability scanning foundation
- **PYRO Platform** - Integration architecture inspiration
- **Rust Community** - Amazing ecosystem
- **Svelte Team** - Excellent frontend framework
- **Node-RED** - Powerful automation platform

---

**Built with 🦀 Rust, ⚡ Svelte, and 🔄 Node-RED**

For questions, issues, or contributions, visit the [GitHub repository](https://github.com/Ununp3ntium115/build_openvas).
