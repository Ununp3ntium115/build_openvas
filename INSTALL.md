# CRYPTEX Installation Guide

Complete installation instructions for all platforms: **Linux**, **macOS**, and **Windows**.

## 📋 Prerequisites

### All Platforms

- **Rust toolchain** (1.70+): https://rustup.rs/
- **Node.js** (18+): https://nodejs.org/ (for Svelte UI)
- **Git**: For cloning the repository

### Optional

- **Node-RED** (3.0+): For workflow automation
- **Docker**: For containerized deployment

## 🚀 Quick Install

### Linux / macOS

```bash
# Clone repository
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas

# Run installer
chmod +x install-cryptex.sh
./install-cryptex.sh

# Reload shell
exec $SHELL

# Start CRYPTEX
cryptex-api
```

### Windows (PowerShell)

```powershell
# Clone repository
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas

# Run installer
.\install.ps1

# Restart PowerShell

# Start CRYPTEX
cryptex-api
```

## 📦 Detailed Installation

### Step 1: Clone Repository

```bash
git clone https://github.com/Ununp3ntium115/build_openvas.git
cd build_openvas
```

### Step 2: Build from Source

#### Linux / macOS

```bash
cd cryptex
cargo build --release
cd ..
```

#### Windows

```powershell
cd cryptex
cargo build --release
cd ..
```

Build artifacts will be in `cryptex/target/release/`

### Step 3: Install Binaries

#### Option A: Automatic Installation

**Linux/macOS:**
```bash
chmod +x install-cryptex.sh
./install-cryptex.sh
```

**Windows:**
```powershell
.\install.ps1
```

This installs to:
- **Linux/macOS**: `~/.cryptex/`
- **Windows**: `%USERPROFILE%\.cryptex\`

#### Option B: Manual Installation

**Linux/macOS:**
```bash
mkdir -p ~/.cryptex/bin ~/.cryptex/data
cp cryptex/target/release/the_interface_server ~/.cryptex/bin/
cp cryptex/target/release/the_commune ~/.cryptex/bin/
chmod +x ~/.cryptex/bin/*

# Add to PATH
echo 'export PATH="$PATH:$HOME/.cryptex/bin"' >> ~/.bashrc
source ~/.bashrc
```

**Windows:**
```powershell
New-Item -ItemType Directory -Force -Path "$env:USERPROFILE\.cryptex\bin"
New-Item -ItemType Directory -Force -Path "$env:USERPROFILE\.cryptex\data"
Copy-Item "cryptex\target\release\the_interface_server.exe" "$env:USERPROFILE\.cryptex\bin\"
Copy-Item "cryptex\target\release\the_commune.exe" "$env:USERPROFILE\.cryptex\bin\"

# Add to PATH
$path = [Environment]::GetEnvironmentVariable("Path", "User")
[Environment]::SetEnvironmentVariable("Path", "$path;$env:USERPROFILE\.cryptex\bin", "User")
```

### Step 4: Install Frontend (Optional)

```bash
cd cryptex-visualizer
npm install
npm run build
cd ..
```

### Step 5: Install Node-RED Nodes (Optional)

```bash
cd ~/.node-red  # Windows: cd $env:USERPROFILE\.node-red
npm install /path/to/build_openvas/node-red-cryptex
```

## 🏃 Running CRYPTEX

### Standalone Mode

#### Linux/macOS

```bash
# Using launcher
./cryptex-launcher.sh start

# Or directly
~/.cryptex/bin/the_interface_server

# Or using installed command
cryptex-api
```

#### Windows

```powershell
# Using launcher
.\cryptex-launcher.ps1 start

# Or directly
& "$env:USERPROFILE\.cryptex\bin\the_interface_server.exe"

# Or using installed command
cryptex-api
```

### Docker Mode

```bash
# Full stack
docker-compose up -d

# API only
docker-compose up -d cryptex-api
```

### As MCP Server (PYRO Integration)

```bash
# Linux/macOS
~/.cryptex/bin/the_commune

# Windows
& "$env:USERPROFILE\.cryptex\bin\the_commune.exe"
```

## 🔧 Configuration

### Environment Variables

Create a `.env` file or set environment variables:

```bash
# Database location
CRYPTEX_DB_PATH=/path/to/cryptex.db

# API server
CRYPTEX_BIND_ADDR=127.0.0.1:8080
RUST_LOG=info

# Optional: AI integration
OPENAI_API_KEY=your_key_here
ANTHROPIC_API_KEY=your_key_here
```

**Linux/macOS:**
```bash
export CRYPTEX_DB_PATH=~/.cryptex/data/cryptex.db
export RUST_LOG=info
```

**Windows:**
```powershell
$env:CRYPTEX_DB_PATH="$env:USERPROFILE\.cryptex\data\cryptex.db"
$env:RUST_LOG="info"
```

### Configuration File

Located at:
- **Linux**: `~/.cryptex/config/config.toml`
- **macOS**: `~/.cryptex/config/config.toml`
- **Windows**: `%USERPROFILE%\.cryptex\config\config.toml`

```toml
[cryptex]
data_dir = "/home/user/.cryptex/data"
db_path = "/home/user/.cryptex/data/cryptex.db"

[api]
bind_addr = "127.0.0.1:8080"
log_level = "info"

[mcp]
log_level = "info"
```

## 🧪 Verify Installation

### Test API Server

**Linux/macOS:**
```bash
# Start server
cryptex-api &

# Test health endpoint
curl http://localhost:8080/health

# Test vulnerability assessment
curl http://localhost:8080/api/v1/vulnerabilities/CVE-2024-1234
```

**Windows:**
```powershell
# Start server
Start-Process cryptex-api

# Test health endpoint
Invoke-WebRequest http://localhost:8080/health

# Test vulnerability assessment
Invoke-WebRequest http://localhost:8080/api/v1/vulnerabilities/CVE-2024-1234
```

### Test MCP Server

```bash
# Echo test (stdin/stdout)
echo '{"jsonrpc":"2.0","id":1,"method":"tools/list"}' | cryptex-mcp
```

### Test Frontend

```bash
cd cryptex-visualizer
npm run dev
# Open http://localhost:5173
```

## 📱 Platform-Specific Notes

### Linux

**Supported Distributions:**
- Ubuntu 20.04+ (x86_64, ARM64)
- Debian 11+ (x86_64, ARM64)
- Fedora 36+ (x86_64)
- Arch Linux (x86_64)
- Any distribution with glibc 2.31+

**Dependencies:**
```bash
# Ubuntu/Debian
sudo apt-get install build-essential pkg-config libssl-dev

# Fedora
sudo dnf install gcc openssl-devel

# Arch
sudo pacman -S base-devel openssl
```

**Systemd Service (Optional):**
```bash
sudo tee /etc/systemd/system/cryptex.service << EOF
[Unit]
Description=CRYPTEX API Server
After=network.target

[Service]
Type=simple
User=$USER
ExecStart=$HOME/.cryptex/bin/the_interface_server
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOF

sudo systemctl daemon-reload
sudo systemctl enable cryptex
sudo systemctl start cryptex
```

### macOS

**Supported Versions:**
- macOS 12 (Monterey) or newer
- Intel (x86_64) and Apple Silicon (ARM64)

**Dependencies:**
```bash
# Using Homebrew
brew install openssl@3
```

**Launch Agent (Optional):**
```bash
mkdir -p ~/Library/LaunchAgents

tee ~/Library/LaunchAgents/com.cryptex.api.plist << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.cryptex.api</string>
    <key>ProgramArguments</key>
    <array>
        <string>$HOME/.cryptex/bin/the_interface_server</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>KeepAlive</key>
    <true/>
</dict>
</plist>
EOF

launchctl load ~/Library/LaunchAgents/com.cryptex.api.plist
```

### Windows

**Supported Versions:**
- Windows 10 (1809+)
- Windows 11
- Windows Server 2019+

**Build Toolchains:**
- **MSVC** (recommended): Visual Studio Build Tools
- **GNU**: MinGW-w64

**Installing Rust (MSVC):**
```powershell
# Download and run rustup-init.exe from https://rustup.rs/
# Select option 1 (default installation)
```

**Windows Service (Optional):**

Use NSSM (Non-Sucking Service Manager):
```powershell
# Download NSSM from https://nssm.cc/
nssm install CRYPTEX "$env:USERPROFILE\.cryptex\bin\the_interface_server.exe"
nssm set CRYPTEX AppDirectory "$env:USERPROFILE\.cryptex\data"
nssm start CRYPTEX
```

## 🐳 Docker Installation

### Build Images

```bash
# API server
cd cryptex
docker build -f Dockerfile.api -t cryptex-api .

# MCP server
docker build -f Dockerfile.mcp -t cryptex-mcp .

# Visualizer
cd ../cryptex-visualizer
docker build -t cryptex-visualizer .
```

### Run with Docker Compose

```bash
docker-compose up -d
```

Access services:
- API: http://localhost:8080
- Visualizer: http://localhost:5173
- Node-RED: http://localhost:1880

## 🔄 Updates

### Pull Latest Changes

```bash
cd build_openvas
git pull origin main
```

### Rebuild

```bash
cd cryptex
cargo build --release
cd ..
./install-cryptex.sh  # Or install.ps1 on Windows
```

## 🗑️ Uninstallation

### Linux/macOS

```bash
rm -rf ~/.cryptex
# Remove from shell RC file manually
```

### Windows

```powershell
Remove-Item -Recurse -Force "$env:USERPROFILE\.cryptex"
# Remove from PATH manually via System Properties
```

## ❓ Troubleshooting

### Binary Not Found

**Linux/macOS:**
```bash
which cryptex-api
echo $PATH
export PATH="$PATH:$HOME/.cryptex/bin"
```

**Windows:**
```powershell
Get-Command cryptex-api
$env:Path
$env:Path += ";$env:USERPROFILE\.cryptex\bin"
```

### Permission Denied (Linux/macOS)

```bash
chmod +x ~/.cryptex/bin/*
```

### Port Already in Use

```bash
# Check what's using port 8080
lsof -i :8080  # Linux/macOS
netstat -ano | findstr :8080  # Windows

# Use different port
CRYPTEX_BIND_ADDR=127.0.0.1:9090 cryptex-api
```

### Database Errors

```bash
# Reset database
rm ~/.cryptex/data/cryptex.db  # Linux/macOS
Remove-Item "$env:USERPROFILE\.cryptex\data\cryptex.db"  # Windows
```

### Build Errors

**OpenSSL not found:**
```bash
# Linux
sudo apt-get install libssl-dev

# macOS
brew install openssl@3
export OPENSSL_DIR=$(brew --prefix openssl@3)

# Windows
# Use rustup default x86_64-pc-windows-msvc
```

## 📚 Additional Resources

- **README**: Main project documentation
- **API_REFERENCE.md**: REST API endpoints
- **DEPLOYMENT.md**: Production deployment guide
- **Node-RED README**: node-red-cryptex/README.md

## 🆘 Support

- GitHub Issues: https://github.com/Ununp3ntium115/build_openvas/issues
- Documentation: See docs/ directory
- PYRO Integration: https://github.com/Ununp3ntium115/PYRO_Platform_Ignition.git
