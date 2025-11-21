# CRYPTEX Steering Documentation
**Project**: CRYPTEX - Cross-Platform Rust Yielding Penetration Testing Execution eXperience
**Status**: Planning & Architecture Phase Complete
**Date**: 2025-11-21

## Overview

Welcome to the CRYPTEX steering documentation. This directory contains comprehensive architectural documentation for converting the AI-Enhanced OpenVAS project into a fully Rust-based, cross-platform security assessment platform with anarchist-inspired terminology.

---

## 📚 Documentation Index

### **Read in This Order:**

1. **[00_GAP_ANALYSIS.md](./00_GAP_ANALYSIS.md)** ⭐ START HERE
   - Current technology stack analysis
   - Rust vs Non-Rust component identification
   - Gap identification and conversion priorities
   - Risk assessment and success metrics
   - **Estimated reading time**: 15 minutes

2. **[01_CRYPTEX_DICTIONARY.md](./01_CRYPTEX_DICTIONARY.md)** ⭐ CORE CONCEPT
   - Anarchist terminology system
   - Complete naming convention mappings
   - The Cryptex function implementation
   - Philosophical guidelines
   - **Estimated reading time**: 20 minutes

3. **[02_MCP_SERVER_SPECIFICATION.md](./02_MCP_SERVER_SPECIFICATION.md)**
   - Model Context Protocol server design
   - Tool specifications with pseudocode
   - JSON-RPC 2.0 protocol implementation
   - Transport layer options (stdio/HTTP/WebSocket)
   - **Estimated reading time**: 25 minutes

4. **[03_FUNCTION_MAPPING_CRYPTEX.md](./03_FUNCTION_MAPPING_CRYPTEX.md)**
   - Complete function-by-function mapping
   - Pseudocode for all 21+ core functions
   - Traditional name → Anarchist name conversions
   - Implementation details for each module
   - **Estimated reading time**: 30 minutes

5. **[04_RUST_CONVERSION_ROADMAP.md](./04_RUST_CONVERSION_ROADMAP.md)**
   - 20-week implementation timeline
   - Phase-by-phase breakdown
   - Detailed implementation examples
   - Testing and deployment strategies
   - **Estimated reading time**: 25 minutes

6. **[05_FRONTEND_ARCHITECTURE.md](./05_FRONTEND_ARCHITECTURE.md)**
   - Svelte/TypeScript frontend design
   - Component structure and state management
   - API integration patterns
   - Electron wrapper (optional desktop app)
   - **Estimated reading time**: 20 minutes

7. **[06_DATA_ARCHITECTURE.md](./06_DATA_ARCHITECTURE.md)**
   - redb embedded database implementation
   - Node-RED workflow automation
   - Custom REST API specification
   - Complete integration examples
   - **Estimated reading time**: 20 minutes

---

## 🎯 Quick Start Guide

### **For First-Time Readers:**

1. **Read the Gap Analysis** to understand what we're converting and why
2. **Study the Cryptex Dictionary** to learn the anarchist terminology system
3. **Review the MCP Server Specification** to see how AI models will interact with CRYPTEX
4. **Scan the Function Mapping** to understand the conversion approach
5. **Check the Roadmap** to see the implementation timeline

### **For Implementers:**

1. Start with **00_GAP_ANALYSIS.md** for context
2. Reference **01_CRYPTEX_DICTIONARY.md** for naming conventions
3. Use **03_FUNCTION_MAPPING_CRYPTEX.md** as your implementation guide
4. Follow **04_RUST_CONVERSION_ROADMAP.md** phase by phase
5. Refer to architecture docs (05, 06) for specific subsystem implementation

---

## 📊 Project Statistics

### **Documentation Metrics**

| Metric | Value |
|--------|-------|
| **Total Documents** | 7 |
| **Total Pages (est.)** | ~150 |
| **Functions Mapped** | 21+ |
| **Cryptex Mappings** | 60+ |
| **Estimated Reading Time** | ~2.5 hours |
| **Implementation Timeline** | 18-20 weeks |

### **Conversion Scope**

| Component | From | To | LOC | Priority |
|-----------|------|-----|-----|----------|
| AI Service Core | C | Rust | ~850 | P0 |
| AI Providers | C | Rust | ~600 | P0 |
| Vulnerability Scoring | C | Rust | ~900 | P0 |
| GUI Server | Python | Rust | ~330 | P0 |
| Frontend | JavaScript | Svelte/TS | ~2,310 | P1 |
| Scanner Bridge | C | Rust | ~450 | P1 |
| Report Engine | C | Rust | ~1,200 | P1 |

---

## 🏗️ Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                      CRYPTEX Architecture                        │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              The Visualizer (Svelte/TypeScript)            │ │
│  │              Frontend + Electron (optional)                │ │
│  └───────────────────────┬────────────────────────────────────┘ │
│                          │                                       │
│  ┌───────────────────────┴────────────────────────────────────┐ │
│  │           The Interface (Rust/Axum REST API)               │ │
│  │           HTTP/WebSocket Gateway                           │ │
│  └───────────────────────┬────────────────────────────────────┘ │
│                          │                                       │
│  ┌───────────────────────┴────────────────────────────────────┐ │
│  │              The Collective (AI Service Core)              │ │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │ │
│  │  │Agitator  │  │Assessor  │  │Infiltrator│  │Propagandist│ │
│  │  │(AI Prov) │  │(Scoring) │  │(Scanner) │  │(Reports) │  │ │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘  │ │
│  └───────────────────────┬────────────────────────────────────┘ │
│                          │                                       │
│  ┌───────────────────────┴────────────────────────────────────┐ │
│  │          The Archive (redb Embedded Database)              │ │
│  │          • Weaknesses  • Infiltrations  • Reports          │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │              The Flow (Node-RED Automation)                │ │
│  │              Visual Workflow Integration                   │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │        The Commune (MCP Server - JSON-RPC 2.0)            │ │
│  │        AI Model Integration Protocol                       │ │
│  └────────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────────┘
```

---

## 🛠️ Technology Stack

### **Core Runtime**
- **Language**: Rust 2021 Edition
- **Async Runtime**: Tokio
- **HTTP Server**: Axum
- **Database**: redb (embedded)

### **AI Integration**
- **Providers**: OpenAI, Claude (Anthropic)
- **HTTP Client**: reqwest with rustls
- **Protocol**: MCP (Model Context Protocol)

### **Frontend**
- **Framework**: Svelte 4/5
- **Language**: TypeScript
- **Build Tool**: Vite
- **Styling**: TailwindCSS
- **Desktop**: Electron (optional)

### **Automation & Integration**
- **Workflow**: Node-RED
- **API**: RESTful HTTP + WebSocket
- **Data Format**: JSON, CBOR

---

## 🚀 Next Steps

### **Phase 0: Immediate Actions (Week 1)**

1. **Review All Documentation**
   - [ ] Read all 7 documents in order
   - [ ] Understand the Cryptex terminology system
   - [ ] Familiarize with the architecture

2. **Set Up Development Environment**
   - [ ] Install Rust toolchain (rustup)
   - [ ] Set up IDE (VS Code + rust-analyzer)
   - [ ] Install Node.js for frontend development
   - [ ] Install Node-RED for workflow testing

3. **Create Initial Project Structure**
   ```bash
   # Create Rust workspace
   cargo new --bin cryptex
   cd cryptex

   # Create all crate directories
   mkdir -p crates/{the_collective,the_agitator,the_assessor,the_infiltrator,the_propagandist,the_archive,the_interface,the_coordinator}

   # Create frontend project
   npm create vite@latest cryptex-visualizer -- --template svelte-ts

   # Copy steering docs to project
   cp -r /path/to/steering ./docs/steering
   ```

4. **Set Up CI/CD**
   - [ ] Create GitHub Actions workflow
   - [ ] Set up automated testing
   - [ ] Configure cross-platform builds

### **Phase 1: Core Implementation (Weeks 2-6)**

Follow the detailed roadmap in [04_RUST_CONVERSION_ROADMAP.md](./04_RUST_CONVERSION_ROADMAP.md).

---

## 📖 Key Concepts

### **The Cryptex Function**

The heart of the naming system:

```rust
/// Transform traditional name to anarchist terminology
pub fn cryptex(traditional: &str) -> &str {
    CRYPTEX_DICTIONARY.get(traditional)
        .copied()
        .unwrap_or(traditional)
}

// Examples:
cryptex("server")      // → "the_commune"
cryptex("database")    // → "the_archive"
cryptex("ai_service")  // → "the_collective"
```

### **Core Modules (Anarchist Names)**

| Traditional | Anarchist | Purpose |
|------------|-----------|---------|
| `ai_service` | `the_collective` | AI service core |
| `provider` | `the_agitator` | AI provider interface |
| `scanner` | `the_infiltrator` | Vulnerability scanner |
| `vulnerability_scoring` | `the_assessor` | Risk evaluation |
| `report_generator` | `the_propagandist` | Report creation |
| `database` | `the_archive` | Data storage |
| `config_manager` | `the_coordinator` | Configuration |
| `server` | `the_interface` | HTTP API server |
| `frontend` | `the_visualizer` | User interface |

---

## 🎓 Learning Path

### **For Rust Beginners:**

1. Complete [The Rust Book](https://doc.rust-lang.org/book/)
2. Read [Rust by Example](https://doc.rust-lang.org/rust-by-example/)
3. Study [Tokio Tutorial](https://tokio.rs/tokio/tutorial) for async programming
4. Review this project's documentation

### **For Experienced Rust Developers:**

1. Read **00_GAP_ANALYSIS.md** for context
2. Dive into **03_FUNCTION_MAPPING_CRYPTEX.md** for implementation details
3. Start implementing following **04_RUST_CONVERSION_ROADMAP.md**

### **For Frontend Developers:**

1. Review **05_FRONTEND_ARCHITECTURE.md**
2. Study the Svelte API documentation
3. Understand the state management patterns
4. Begin implementing UI components

---

## 🤝 Contributing

### **Code Style**

- Follow Rust standard formatting (`cargo fmt`)
- Use Clippy for linting (`cargo clippy`)
- Write comprehensive tests
- Document public APIs with rustdoc

### **Naming Conventions**

- Always use Cryptex anarchist names in code
- Include traditional name in documentation
- Example:
  ```rust
  /// The Awakening - Initialize the collective
  ///
  /// Traditional name: `init` or `initialize`
  pub async fn the_awakening(charter: TheCharter) -> Result<Self> {
      // Implementation
  }
  ```

### **Documentation**

- Every public function must have documentation
- Include usage examples
- Link to relevant steering documents

---

## 📞 Support & Questions

For questions about this documentation:
1. Review the specific document thoroughly
2. Check related documents for context
3. Search for similar concepts in other documents
4. Create an issue with specific questions

---

## 🎯 Success Criteria

### **Phase 0: Planning** ✅ COMPLETE
- [x] Gap analysis completed
- [x] Architecture designed
- [x] Functions mapped
- [x] Roadmap created

### **Phase 1-5: Implementation** (18-20 weeks)
- [ ] All Rust modules implemented
- [ ] Frontend completed
- [ ] Tests passing on all platforms
- [ ] Documentation complete
- [ ] Cross-platform builds working

### **Phase 6: Deployment**
- [ ] Single-binary distribution
- [ ] Docker images published
- [ ] Package repositories setup
- [ ] Production deployment guide

---

## 📜 License

This project is licensed under GPL-2.0-or-later, consistent with the original OpenVAS project.

---

## 🙏 Acknowledgments

- **Greenbone Networks** - Original OpenVAS platform
- **Rust Community** - Excellent tools and libraries
- **Anarchist Philosophy** - Inspiration for terminology and design principles

---

## 🔥 Motivation

```
"We reject the hierarchy of traditional naming.
We embrace the collective power of Rust.
We build security tools that serve the people, not the powerful.

No gods, no masters, no memory leaks."

- Anarchist Rust Collective, 2025
```

---

## 📚 Additional Resources

- [Rust Book](https://doc.rust-lang.org/book/)
- [Tokio Documentation](https://tokio.rs/)
- [Axum Web Framework](https://github.com/tokio-rs/axum)
- [Svelte Documentation](https://svelte.dev/)
- [redb Database](https://github.com/cberner/redb)
- [Node-RED Documentation](https://nodered.org/docs/)
- [MCP Specification](https://modelcontextprotocol.org/)

---

**Now go forth and build CRYPTEX - the liberated security assessment platform!** 🚀

*"The revolution will not be interpreted, it will be compiled."*
