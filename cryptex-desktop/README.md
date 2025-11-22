# openFireVulnerability Desktop

**Professional native desktop application for vulnerability assessment and risk analysis**

Built with **Tauri** (Rust backend) + **Svelte** (OpenVAS/Greenbone-inspired professional UI).

---

## 🔥 Features

✅ **Native Desktop App** - Cross-platform (Windows, macOS, Linux)
✅ **Embedded Rust Backend** - Built-in API server, no external dependencies
✅ **Professional Security UI** - OpenVAS/Greenbone-style interface
✅ **System Tray Integration** - Background operation with quick actions
✅ **Offline Capable** - Works without internet connection
✅ **Node-RED Integration** - Visual workflow automation
✅ **PYRO Platform** - Connect to PYRO security ecosystem
✅ **Cross-Platform Database** - Embedded redb for persistent storage

---

## 📦 Tech Stack

### Backend (Rust)
- **Tauri 1.5** - Secure native wrapper
- **Embedded API Server** - CRYPTEX REST API (Axum framework)
- **redb Database** - Embedded ACID-compliant database
- **Platform-agnostic** - XDG Base Directory compliant

### Frontend (Svelte + TypeScript)
- **Svelte 4** - Reactive, compile-time optimized UI
- **TypeScript 5** - Type-safe development
- **Vite** - Lightning-fast build tool
- **Lucide Icons** - Professional iconography
- **Dark Theme** - Greenbone Security Assistant styling

---

## 🚀 Quick Start

### Prerequisites

**System Dependencies (Linux only):**
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
  libwebkit2gtk-4.1-dev \
  libsoup-3.0-dev \
  libjavascriptcoregtk-4.1-dev \
  libsoup2.4-dev \
  build-essential \
  curl \
  wget \
  file \
  libssl-dev \
  libgtk-3-dev \
  libayatana-appindicator3-dev \
  librsvg2-dev

# Fedora/RHEL
sudo dnf install webkit2gtk4.1-devel \
  openssl-devel \
  gtk3-devel \
  libappindicator-gtk3-devel \
  librsvg2-devel

# Arch Linux
sudo pacman -S webkit2gtk \
  gtk3 \
  libappindicator-gtk3 \
  librsvg
```

**Node.js & Rust:**
- Node.js 18+ ([nodejs.org](https://nodejs.org))
- Rust 1.70+ ([rustup.rs](https://rustup.rs))

### Installation

```bash
# Clone repository (if not already)
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas/cryptex-desktop

# Install npm dependencies
npm install

# Build Rust backend (optional, tauri:dev does this automatically)
cd src-tauri && cargo build
```

### Development

```bash
# Start development server (hot reload)
npm run tauri:dev
```

This will:
1. Start Vite dev server (frontend)
2. Compile Rust backend
3. Launch desktop application
4. Enable hot module replacement

### Production Build

```bash
# Build production-ready application
npm run tauri:build
```

**Build Outputs:**

| Platform | Output Location | File Types |
|----------|----------------|------------|
| **Windows** | `src-tauri/target/release/bundle/msi/` | `.exe`, `.msi` |
| **macOS** | `src-tauri/target/release/bundle/macos/` | `.app`, `.dmg` |
| **Linux** | `src-tauri/target/release/bundle/` | `.deb`, `.AppImage`, `.rpm` |

---

## 📂 Project Structure

```
cryptex-desktop/
├── src/                      # Svelte frontend source
│   ├── components/           # UI components
│   │   ├── Sidebar.svelte   # Navigation sidebar
│   │   └── TopBar.svelte    # Custom titlebar
│   ├── views/                # Main application views
│   │   ├── Dashboard.svelte # Vulnerability dashboard
│   │   ├── Scans.svelte     # Scan management
│   │   ├── Targets.svelte   # Target configuration
│   │   ├── Results.svelte   # Scan results viewer
│   │   ├── Reports.svelte   # Report generation
│   │   └── Configuration.svelte # Settings
│   ├── App.svelte            # Root component
│   ├── main.ts               # Frontend entry point
│   └── app.css               # Global styles
│
├── src-tauri/                # Tauri Rust backend
│   ├── src/
│   │   ├── main.rs           # Tauri app entry point
│   │   ├── api_server.rs     # Embedded REST API
│   │   ├── commands.rs       # IPC command handlers
│   │   └── database.rs       # Database utilities
│   ├── icons/                # Application icons
│   │   └── icon.svg          # Source icon
│   ├── Cargo.toml            # Rust dependencies
│   └── tauri.conf.json       # Tauri configuration
│
├── package.json              # Node.js dependencies
├── vite.config.ts            # Vite build config
└── README.md                 # This file
```

---

## 🖥️ Application Views

### Dashboard
- Real-time vulnerability statistics
- Severity distribution charts
- Quick scan launcher
- KEV (Known Exploited Vulnerabilities) tracking
- Recent activity timeline

### Scans
- Active and completed scan list
- Scan status monitoring
- Vulnerability count by severity
- Start new scans with target selection

### Targets
- Manage scan targets (hosts, IP ranges, CIDR notation)
- Add/Edit/Delete target configurations
- Target description and metadata

### Results
- Vulnerability scan results grid
- Severity filtering (Critical, High, Medium, Low)
- Search functionality (CVE ID, title, host)
- Detailed vulnerability modal views

### Reports
- Generate reports in multiple formats
  - HTML - Rich formatted reports
  - Markdown - Documentation-friendly
  - JSON - Machine-readable
  - Plain Text - Simple output
- Report history with metadata
- Download and view actions

### Configuration
- API server settings (URL, port, auto-start)
- Database path configuration
- Scan behavior settings (timeout, concurrency, retention)
- Desktop notification preferences

---

## 🔧 Configuration

### Database Location

Default locations:
- **Linux:** `~/.local/share/openFireVulnerability/openfire.db`
- **macOS:** `~/Library/Application Support/openFireVulnerability/openfire.db`
- **Windows:** `%APPDATA%\openFireVulnerability\openfire.db`

Override with environment variable:
```bash
export OPENFIRE_DATA_DIR=/custom/path
```

### API Server

The embedded API server runs at `http://localhost:8080` by default.

Configure in-app via **Configuration** view or manually:
```json
{
  "apiUrl": "http://localhost",
  "apiPort": 8080,
  "autoStartServer": true
}
```

---

## 🎨 UI Theme

The application uses a professional dark theme inspired by **Greenbone Security Assistant** (OpenVAS):

**Color Palette:**
- Primary: `#66c430` (Greenbone Green)
- Background: `#1a1a1a` (Dark)
- Secondary: `#2a2a2a`
- Severity Colors:
  - Critical: `#dc2626` (Red)
  - High: `#fb923c` (Orange)
  - Medium: `#facc15` (Yellow)
  - Low: `#4ade80` (Green)

---

## 🔌 Integrations

### Node-RED

Launch Node-RED workflows from the top bar:
- Custom CRYPTEX nodes available
- Visual automation editor
- Integration with scan workflow

### PYRO Platform

Connect to PYRO security ecosystem:
- Embeddable UI components
- MCP (Model Context Protocol) server
- Shared vulnerability intelligence

---

## 🛠️ Development Tips

### Hot Reload

Frontend changes hot reload automatically in dev mode. Rust changes require recompilation (Tauri handles this).

### Debug Logging

Enable Tauri debug logs:
```bash
RUST_LOG=debug npm run tauri:dev
```

### Browser DevTools

Press `F12` in the application to open DevTools.

### IPC Commands

Test Tauri commands from browser console:
```javascript
await window.__TAURI__.invoke('assess_vulnerability', { cveId: 'CVE-2024-1234' })
```

---

## 📋 Build Requirements

### Minimum Versions
- **Node.js:** 18.0.0+
- **Rust:** 1.70.0+
- **npm:** 9.0.0+
- **Cargo:** 1.70.0+

### Platform-Specific Notes

**Windows:**
- Windows 10 version 1809 or later
- Visual Studio 2019 Build Tools or newer
- WebView2 Runtime (usually pre-installed)

**macOS:**
- macOS 10.15 (Catalina) or later
- Xcode Command Line Tools

**Linux:**
- GTK 3.24+
- WebKitGTK 2.36+
- glibc 2.31+ (Ubuntu 20.04+, Debian 11+, Fedora 33+)

---

## 🐛 Troubleshooting

### Build Failures

**Missing system dependencies (Linux):**
```bash
# Check error message for missing -dev packages
sudo apt-get install <missing-package>-dev
```

**Rust compilation errors:**
```bash
# Update Rust toolchain
rustup update stable
```

**Node module issues:**
```bash
# Clean install
rm -rf node_modules package-lock.json
npm install
```

### Runtime Issues

**API server won't start:**
- Check if port 8080 is already in use
- Verify database directory is writable
- Check logs for detailed error messages

**UI not loading:**
- Clear browser cache (in dev mode)
- Rebuild frontend: `npm run build`

**System tray icon missing:**
- Verify `icons/icon.png` exists
- Check system tray is enabled in OS

---

## 📄 License

See main project LICENSE file.

---

## 🤝 Contributing

This desktop application is part of the larger openFireVulnerability/CRYPTEX project.

**Development Workflow:**
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on all platforms
5. Submit a pull request

---

## 🔗 Related Projects

- **CRYPTEX Core** - Rust backend crates (`../cryptex/`)
- **Node-RED Nodes** - Custom automation nodes (`../node-red-cryptex/`)
- **PYRO Platform** - Security intelligence platform

---

## 📞 Support

For issues specific to the desktop application:
- Open an issue on GitHub
- Provide platform info and error logs
- Include reproduction steps

---

**Built with ❤️ using Rust and Svelte**
