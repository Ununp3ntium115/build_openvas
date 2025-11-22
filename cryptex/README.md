# CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience

**"No gods, no masters, no memory leaks."**

## 🎯 Overview

CRYPTEX is a revolutionary, fully Rust-based security assessment platform that transforms the AI-Enhanced OpenVAS project into a cross-platform, memory-safe, high-performance vulnerability scanner with anarchist-inspired terminology.

## 🏗️ Project Status

**Phase**: 2 (Processing & Analysis) - ✅ COMPLETE
**Build Status**: ✅ All crates compile successfully
**Test Coverage**: ✅ 68 tests passing (33 Phase 1 + 35 Phase 2)
**Documentation**: Comprehensive rustdoc coverage

## 📦 Workspace Structure

```
cryptex/
├── Cargo.toml                  # Workspace configuration
├── README.md                   # This file
└── crates/
    ├── the_foundation/         # ✅ Core Types & Traits (Phase 1)
    ├── the_collective/         # ✅ AI Service Core (Phase 1)
    ├── the_agitator/           # ✅ AI Provider Integrations (Phase 1)
    ├── the_assessor/           # ✅ Vulnerability Scoring (Phase 2)
    ├── the_infiltrator/        # ✅ Scanner Bridge (Phase 2)
    ├── the_propagandist/       # ✅ Report Generation (Phase 2)
    ├── the_archive/            # 📋 Database Layer (redb)
    ├── the_interface/          # 📋 REST API Server (Phase 3)
    └── the_coordinator/        # ✅ Configuration Management (Phase 0)
```

**Legend:**
- ✅ Fully Implemented & Tested
- 📋 Planned for future phases

## 🚀 Quick Start

### Prerequisites

- **Rust**: 1.70+ (install via [rustup](https://rustup.rs/))
- **Cargo**: Comes with Rust
- **OpenSSL**: For TLS support (Linux/macOS)

### Build

```bash
# Clone the repository
git clone https://github.com/Ununp3ntium115/build_openvas
cd build_openvas/cryptex

# Check all crates compile
cargo check

# Build in release mode
cargo build --release

# Run tests
cargo test --workspace

# Generate documentation
cargo doc --workspace --no-deps --open
```

### Development

```bash
# Watch mode for development
cargo watch -x check -x test

# Format code
cargo fmt --all

# Lint with Clippy
cargo clippy --workspace -- -D warnings

# Check for security vulnerabilities
cargo audit
```

## 📚 Documentation

### Steering Documentation

Comprehensive architectural documentation is available in `/steering`:

1. **[00_README.md](../steering/00_README.md)** - Master index
2. **[00_GAP_ANALYSIS.md](../steering/00_GAP_ANALYSIS.md)** - Technology stack analysis
3. **[01_CRYPTEX_DICTIONARY.md](../steering/01_CRYPTEX_DICTIONARY.md)** - Anarchist naming system
4. **[02_MCP_SERVER_SPECIFICATION.md](../steering/02_MCP_SERVER_SPECIFICATION.md)** - MCP server design
5. **[03_FUNCTION_MAPPING_CRYPTEX.md](../steering/03_FUNCTION_MAPPING_CRYPTEX.md)** - Function mappings with pseudocode
6. **[04_RUST_CONVERSION_ROADMAP.md](../steering/04_RUST_CONVERSION_ROADMAP.md)** - 20-week implementation timeline
7. **[05_FRONTEND_ARCHITECTURE.md](../steering/05_FRONTEND_ARCHITECTURE.md)** - Svelte/TypeScript UI design
8. **[06_DATA_ARCHITECTURE.md](../steering/06_DATA_ARCHITECTURE.md)** - redb + Node-RED + API design

### Crate Documentation

```bash
# Generate and open documentation for all crates
cargo doc --workspace --no-deps --open
```

## 🔑 Core Concepts

### The Cryptex Dictionary

CRYPTEX uses anarchist terminology throughout the codebase:

| Traditional | Anarchist | Purpose |
|------------|-----------|---------|
| `ai_service` | `the_collective` | AI service core |
| `provider` | `the_agitator` | AI provider interface |
| `database` | `the_archive` | Data storage (redb) |
| `config` | `the_charter` | Configuration |
| `request` | `the_inquiry` | AI processing request |
| `response` | `the_reply` | AI processing response |
| `scanner` | `the_infiltrator` | Vulnerability scanner |
| `vulnerability_scoring` | `the_assessor` | Risk evaluation |
| `report_generator` | `the_propagandist` | Report creation |
| `server` | `the_interface` | HTTP API server |

### Example Usage

```rust
use the_collective::{TheCollective, TheCharter, TheInquiry, TaskType};

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    // Load the charter (configuration)
    let charter = TheCharter::the_charter_loading(None)?;

    // The Awakening - Initialize the collective
    let collective = TheCollective::the_awakening(charter).await?;

    // Create an inquiry
    let inquiry = TheInquiry::new(
        TaskType::VulnerabilityAnalysis,
        serde_json::json!({
            "cve": "CVE-2023-12345",
            "severity": "critical",
        })
    );

    // Process the inquiry
    let reply = collective.the_inquiry_sync(inquiry).await?;

    println!("AI Analysis: {}", reply.content);
    println!("Certainty: {:.1}%", reply.certainty * 100.0);

    // The Rest - Graceful shutdown
    collective.the_rest().await?;

    Ok(())
}
```

## 🧪 Testing

```bash
# Run all tests
cargo test --workspace

# Run tests for specific crate
cargo test -p the_collective

# Run tests with output
cargo test --workspace -- --nocapture

# Run with coverage (requires cargo-tarpaulin)
cargo tarpaulin --workspace
```

## 🔧 Configuration

Create `cryptex-config.toml`:

```toml
[commune]
name = "CRYPTEX Liberation Node"
host = "127.0.0.1"
port = 8080
ws_port = 9090

[[agitators]]
name = "openai"
agitator_type = "openai"
enabled = true
api_key_env = "OPENAI_API_KEY"
model = "gpt-4"
timeout_seconds = 30

[[agitators]]
name = "claude"
agitator_type = "claude"
enabled = true
api_key_env = "ANTHROPIC_API_KEY"
model = "claude-3-sonnet-20240229"
timeout_seconds = 30

[archive]
path = "./cryptex-archive.redb"
max_size_gb = 10

[memory]
max_size_mb = 256
ttl_seconds = 3600

cache_threshold = 0.8
```

### Environment Variables

```bash
# AI Provider API Keys
export OPENAI_API_KEY="sk-your-openai-key"
export ANTHROPIC_API_KEY="your-claude-key"

# Optional: Override configuration
export CRYPTEX__COMMUNE__PORT=8090
export CRYPTEX__ARCHIVE__PATH="./custom-archive.redb"
```

## 🎨 Architecture

### Component Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                      CRYPTEX Architecture                    │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │         The Collective (AI Service Core)               │ │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌────────┐│ │
│  │  │Agitator  │  │Assessor  │  │Infiltrator│ │Propagand││ │
│  │  │(AI Prov) │  │(Scoring) │  │(Scanner) │  │ist(Rprt)││ │
│  │  └──────────┘  └──────────┘  └──────────┘  └────────┘│ │
│  └────────────────────────────────────────────────────────┘ │
│                           │                                  │
│  ┌────────────────────────────────────────────────────────┐ │
│  │          The Archive (redb Embedded Database)          │ │
│  │          • Weaknesses  • Infiltrations  • Reports      │ │
│  └────────────────────────────────────────────────────────┘ │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐ │
│  │        The Interface (Axum REST API + WebSocket)       │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### Technology Stack

| Component | Technology |
|-----------|-----------|
| **Language** | Rust 2021 Edition |
| **Async Runtime** | Tokio |
| **HTTP Server** | Axum |
| **Database** | redb (embedded) |
| **HTTP Client** | reqwest + rustls |
| **Serialization** | serde + serde_json |
| **Error Handling** | thiserror + anyhow |
| **Logging** | tracing + tracing-subscriber |

## 🛣️ Roadmap

### ✅ Phase 0: Foundation (Weeks 1-2) - COMPLETE
- [x] Workspace structure created
- [x] All crates initialized
- [x] Core types and error handling implemented
- [x] Configuration system (The Coordinator) complete
- [x] Workspace compiles successfully

### ✅ Phase 1: Core Services (Weeks 3-6) - COMPLETE
- [x] Implement The Collective (AI Service Core)
- [x] Implement The Agitator (OpenAI + Claude providers)
- [x] Add caching layer (The Memory)
- [x] Add metrics/observability (The Observer)
- [x] Full agitator integration with provider selection
- [x] Cache threshold-based reply caching
- [x] Comprehensive test coverage (33 tests passing)

### ✅ Phase 2: Processing & Analysis (Weeks 7-10) - COMPLETE
- [x] The Assessor (Vulnerability Scoring)
  - CVSS v3.x calculator with full specification compliance
  - KEV, EPSS, SSVC integration
  - AI-enhanced risk scoring
  - Composite risk algorithms
  - 14 tests passing
- [x] The Infiltrator (Scanner Bridge)
  - Real-time scan enhancement
  - OpenVAS integration hooks
  - Per-host context tracking
  - AI remediation guidance
  - 13 tests passing
- [x] The Propagandist (Report Generation)
  - Multi-format support (JSON, HTML, Markdown, Text)
  - Executive summary generation
  - Audience-specific reports
  - Risk-based prioritization
  - 8 tests passing

### 📋 Phase 3: API & Communication (Weeks 11-13)
- [ ] The Interface (REST API Server)
- [ ] WebSocket support
- [ ] Node-RED integration

### 📋 Phase 4: Frontend (Weeks 14-17)
- [ ] Svelte/TypeScript UI
- [ ] Electron wrapper
- [ ] WebAssembly components

### 📋 Phase 5: Testing & Deployment (Weeks 18-20)
- [ ] Integration testing
- [ ] Cross-platform builds
- [ ] Documentation
- [ ] Package distribution

## 🤝 Contributing

1. **Fork** the repository
2. **Create** a feature branch: `git checkout -b feature/amazing-enhancement`
3. **Commit** changes: `git commit -m 'feat: Add amazing feature'`
4. **Push** to branch: `git push origin feature/amazing-enhancement`
5. **Submit** a pull request

### Code Style

- Follow Rust standard formatting (`cargo fmt`)
- Use Clippy for linting (`cargo clippy`)
- Write comprehensive tests
- Document public APIs with rustdoc
- Use Cryptex anarchist naming conventions

## 📜 License

GPL-2.0-or-later - consistent with the original OpenVAS project.

## 🙏 Acknowledgments

- **Greenbone Networks** - Original OpenVAS platform
- **Rust Community** - Excellent tools and libraries
- **Anarchist Philosophy** - Inspiration for terminology and design principles

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/Ununp3ntium115/build_openvas/issues)
- **Documentation**: See `/steering` directory
- **Roadmap**: See [04_RUST_CONVERSION_ROADMAP.md](../steering/04_RUST_CONVERSION_ROADMAP.md)

---

**"The revolution will not be interpreted, it will be compiled."**
*- Anarchist Rust Collective, 2025*
