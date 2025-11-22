# CRYPTEX Desktop

**Native desktop application for CRYPTEX vulnerability assessment platform**

Built with **Tauri** (Rust backend) + **Svelte** (professional UI inspired by OpenVAS/Greenbone Security Assistant).

## Features

✅ **Native Desktop App** - Windows, macOS, Linux
✅ **Embedded Rust Backend** - Built-in API server and database
✅ **Professional UI** - OpenVAS/Greenbone-style interface
✅ **System Tray** - Background operation support
✅ **Node-RED Integration** - Launch automation flows
✅ **PYRO Platform** - Connect to PYRO ecosystem
✅ **Offline Mode** - Works without internet

## Tech Stack

**Backend (Rust):**
- Tauri 1.5 - Native wrapper
- CRYPTEX API server (embedded)
- redb database (local storage)
- All CRYPTEX Rust crates integrated

**Frontend (Svelte):**
- Svelte 4 - Reactive UI
- TypeScript - Type safety
- Lucide icons - Clean iconography
- Professional security tool design

## Quick Start

### Development

```bash
npm install
npm run tauri:dev
```

### Build

```bash
npm run tauri:build
```

Outputs:
- Windows: `.exe` installer
- macOS: `.dmg` and `.app` bundle
- Linux: `.deb` and `.AppImage`

## Architecture

```
┌────────────────────────────────────┐
│        Tauri Native Window         │
│  (Rust wrapper with OS integration)│
└────────────┬───────────────────────┘
             │
     ┌───────▼────────┐
     │  Svelte UI     │
     │  (Web view)    │
     └───────┬────────┘
             │ IPC (invoke)
     ┌───────▼────────┐
     │ Rust Commands  │
     │ (Tauri backend)│
     └───────┬────────┘
             │
   ┌─────────┴─────────┐
   │                   │
┌──▼──────┐    ┌──────▼────┐
│Embedded │    │  CRYPTEX  │
│API      │    │  Database │
│Server   │    │  (redb)   │
└─────────┘    └───────────┘
```

## Features

### Dashboard
- Vulnerability statistics
- Scan overview
- Quick scan launcher
- System status

### Scans
- Scan management
- Real-time progress
- Vulnerability counts
- Export results

### Targets
- Target configuration
- Network discovery
- Target groups

### Results
- Vulnerability details
- CVE information
- KEV tracking
- Risk scoring

### Reports
- HTML reports
- JSON export
- Markdown format
- Custom templates

### Configuration
- API settings
- Database location
- Node-RED connection
- PYRO integration

## Desktop Features

### System Tray
- Background operation
- Quick access menu
- API server control
- Minimize to tray

### Native Dialogs
- File selection
- Save report dialogs
- Platform-native feel

### Notifications
- Scan completion
- Critical vulnerabilities
- System alerts

### Window Controls
- Minimize/maximize
- Custom title bar
- Drag-to-move

## Building from Source

### Prerequisites

**Rust:**
```bash
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

**Node.js:**
```bash
# Install Node 18+
```

**System Dependencies:**

**Linux:**
```bash
# Ubuntu/Debian
sudo apt install libwebkit2gtk-4.0-dev \
    build-essential \
    curl \
    wget \
    libssl-dev \
    libgtk-3-dev \
    libayatana-appindicator3-dev \
    librsvg2-dev

# Fedora
sudo dnf install webkit2gtk3-devel \
    openssl-devel \
    curl \
    wget \
    libappindicator-gtk3-devel \
    librsvg2-devel
```

**macOS:**
```bash
# Xcode Command Line Tools
xcode-select --install
```

**Windows:**
- Visual Studio C++ Build Tools
- WebView2 (usually pre-installed)

### Build Steps

```bash
# Clone repository
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas/cryptex-desktop

# Install dependencies
npm install

# Development
npm run tauri:dev

# Production build
npm run tauri:build
```

## Distribution

### Windows
- `.msi` installer
- `.exe` portable

### macOS
- `.dmg` disk image
- `.app` bundle
- Code signed (optional)

### Linux
- `.deb` package (Debian/Ubuntu)
- `.AppImage` (universal)
- `.rpm` (Fedora/RHEL)

## Integration

### Node-RED
Click "Node-RED" button in top bar to launch automation interface.

### PYRO Platform
Click "PYRO" button to connect to PYRO ecosystem.

### Standalone API
Desktop app runs embedded API server on `http://127.0.0.1:8080`

## Configuration

**Config file location:**
- Windows: `%APPDATA%\CRYPTEX\config.toml`
- macOS: `~/Library/Application Support/CRYPTEX/config.toml`
- Linux: `~/.config/CRYPTEX/config.toml`

**Database:**
- Windows: `%APPDATA%\CRYPTEX\cryptex.db`
- macOS: `~/Library/Application Support/CRYPTEX/cryptex.db`
- Linux: `~/.local/share/CRYPTEX/cryptex.db`

## Keyboard Shortcuts

- `Ctrl/Cmd + N` - New scan
- `Ctrl/Cmd + R` - Refresh
- `Ctrl/Cmd + W` - Close window
- `Ctrl/Cmd + Q` - Quit application
- `Ctrl/Cmd + ,` - Open settings

## Troubleshooting

### App Won't Start
- Check system requirements
- Verify WebView2 installed (Windows)
- Check console logs

### API Server Issues
- Check port 8080 availability
- Verify database permissions
- Review system tray icon status

### Build Errors
- Update Rust toolchain
- Clear node_modules and reinstall
- Check system dependencies

## Development

### Project Structure

```
cryptex-desktop/
├── src/                # Svelte frontend
│   ├── components/     # UI components
│   ├── views/          # Page views
│   ├── App.svelte      # Root component
│   └── main.ts         # Entry point
├── src-tauri/          # Rust backend
│   ├── src/
│   │   ├── main.rs     # Tauri app
│   │   ├── commands.rs # IPC commands
│   │   └── api_server.rs # Embedded server
│   ├── Cargo.toml      # Rust deps
│   └── tauri.conf.json # Tauri config
├── package.json        # Node deps
└── vite.config.ts      # Build config
```

### Adding Features

1. **New Rust command:**
   - Add to `src-tauri/src/commands.rs`
   - Register in `main.rs` invoke_handler
   - Call from Svelte via `invoke('command_name')`

2. **New UI view:**
   - Create in `src/views/`
   - Add route in `App.svelte`
   - Add navigation in `Sidebar.svelte`

## License

MIT - See LICENSE file

## Support

- GitHub Issues: https://github.com/Ununp3ntium115/build_openvas/issues
- Documentation: See main README.md
