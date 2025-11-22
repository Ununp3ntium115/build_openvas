#!/bin/bash
# CRYPTEX Installation Script for Linux/macOS
# Installs CRYPTEX binaries and sets up environment

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Installation directories
INSTALL_DIR="${INSTALL_DIR:-$HOME/.cryptex}"
BIN_DIR="$INSTALL_DIR/bin"
DATA_DIR="$INSTALL_DIR/data"
CONFIG_DIR="$INSTALL_DIR/config"

echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║      CRYPTEX Installation Script       ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
echo ""

# Detect platform
PLATFORM=$(uname -s | tr '[:upper:]' '[:lower:]')
ARCH=$(uname -m)

echo -e "${YELLOW}Platform:${NC} $PLATFORM"
echo -e "${YELLOW}Architecture:${NC} $ARCH"
echo -e "${YELLOW}Install directory:${NC} $INSTALL_DIR"
echo ""

# Create directories
echo -e "${YELLOW}Creating directories...${NC}"
mkdir -p "$BIN_DIR" "$DATA_DIR" "$CONFIG_DIR"

# Check if running from source or release
if [ -d "cryptex/target/release" ]; then
    echo -e "${GREEN}✓ Found compiled binaries${NC}"
    BINARY_SOURCE="cryptex/target/release"
elif [ -d "dist/$PLATFORM" ]; then
    echo -e "${GREEN}✓ Found distribution binaries${NC}"
    BINARY_SOURCE="dist/$PLATFORM"
else
    echo -e "${RED}✗ No binaries found${NC}"
    echo -e "${YELLOW}Building from source...${NC}"

    if ! command -v cargo &> /dev/null; then
        echo -e "${RED}Error: Rust/Cargo not installed${NC}"
        echo "Install Rust from https://rustup.rs/"
        exit 1
    fi

    cd cryptex
    cargo build --release
    cd ..
    BINARY_SOURCE="cryptex/target/release"
fi

# Copy binaries
echo -e "${YELLOW}Installing binaries...${NC}"
for binary in "the_commune" "the_interface_server"; do
    if [ -f "$BINARY_SOURCE/$binary" ]; then
        cp "$BINARY_SOURCE/$binary" "$BIN_DIR/"
        chmod +x "$BIN_DIR/$binary"
        echo -e "${GREEN}✓ Installed $binary${NC}"
    else
        echo -e "${YELLOW}⚠ Skipping $binary (not found)${NC}"
    fi
done

# Install Svelte visualizer (if npm is available)
if command -v npm &> /dev/null; then
    echo -e "${YELLOW}Installing Svelte visualizer...${NC}"
    if [ -d "cryptex-visualizer" ]; then
        cd cryptex-visualizer
        npm install
        npm run build
        cp -r dist "$INSTALL_DIR/visualizer"
        cd ..
        echo -e "${GREEN}✓ Installed visualizer${NC}"
    fi
fi

# Install Node-RED nodes (if npm is available)
if command -v npm &> /dev/null; then
    echo -e "${YELLOW}Installing Node-RED nodes...${NC}"
    if [ -d "node-red-cryptex" ] && [ -d "$HOME/.node-red" ]; then
        cd "$HOME/.node-red"
        npm install "$(pwd | sed 's|/.node-red||')/node-red-cryptex"
        echo -e "${GREEN}✓ Installed Node-RED nodes${NC}"
        cd - > /dev/null
    fi
fi

# Setup PATH
echo -e "${YELLOW}Setting up PATH...${NC}"

SHELL_RC=""
if [ -n "$BASH_VERSION" ]; then
    SHELL_RC="$HOME/.bashrc"
elif [ -n "$ZSH_VERSION" ]; then
    SHELL_RC="$HOME/.zshrc"
fi

if [ -n "$SHELL_RC" ] && [ -f "$SHELL_RC" ]; then
    if ! grep -q "CRYPTEX" "$SHELL_RC"; then
        echo "" >> "$SHELL_RC"
        echo "# CRYPTEX Environment" >> "$SHELL_RC"
        echo "export PATH=\"\$PATH:$BIN_DIR\"" >> "$SHELL_RC"
        echo "export CRYPTEX_DATA_DIR=\"$DATA_DIR\"" >> "$SHELL_RC"
        echo "export CRYPTEX_CONFIG_DIR=\"$CONFIG_DIR\"" >> "$SHELL_RC"
        echo -e "${GREEN}✓ Added to $SHELL_RC${NC}"
    else
        echo -e "${YELLOW}⚠ Already in $SHELL_RC${NC}"
    fi
fi

# Create launcher scripts
echo -e "${YELLOW}Creating launcher scripts...${NC}"

cat > "$BIN_DIR/cryptex-api" << 'EOF'
#!/bin/bash
# CRYPTEX REST API Launcher
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
"$SCRIPT_DIR/the_interface_server" "$@"
EOF

cat > "$BIN_DIR/cryptex-mcp" << 'EOF'
#!/bin/bash
# CRYPTEX MCP Server Launcher
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
"$SCRIPT_DIR/the_commune" "$@"
EOF

chmod +x "$BIN_DIR/cryptex-api" "$BIN_DIR/cryptex-mcp"

echo -e "${GREEN}✓ Created launcher scripts${NC}"

# Create config file
if [ ! -f "$CONFIG_DIR/config.toml" ]; then
    cat > "$CONFIG_DIR/config.toml" << EOF
[cryptex]
# CRYPTEX Configuration

# Data directory
data_dir = "$DATA_DIR"

# Database path
db_path = "$DATA_DIR/cryptex.db"

# API server settings
[api]
bind_addr = "127.0.0.1:8080"
log_level = "info"

# MCP server settings
[mcp]
log_level = "info"
EOF
    echo -e "${GREEN}✓ Created config file${NC}"
fi

# Print summary
echo ""
echo -e "${GREEN}╔════════════════════════════════════════╗${NC}"
echo -e "${GREEN}║     Installation Complete! 🎉          ║${NC}"
echo -e "${GREEN}╚════════════════════════════════════════╝${NC}"
echo ""
echo -e "${YELLOW}Installed to:${NC} $INSTALL_DIR"
echo ""
echo -e "${YELLOW}Available commands:${NC}"
echo -e "  ${BLUE}cryptex-api${NC}        Start REST API server"
echo -e "  ${BLUE}cryptex-mcp${NC}        Start MCP server"
echo ""
echo -e "${YELLOW}To use these commands, either:${NC}"
echo -e "  1. Restart your shell: ${BLUE}exec \$SHELL${NC}"
echo -e "  2. Source your shell config: ${BLUE}source $SHELL_RC${NC}"
echo -e "  3. Use full path: ${BLUE}$BIN_DIR/cryptex-api${NC}"
echo ""
echo -e "${YELLOW}Quick start:${NC}"
echo -e "  ${BLUE}cryptex-api${NC}                   # Start API server"
echo -e "  ${BLUE}curl http://localhost:8080/health${NC}  # Test connection"
echo ""
echo -e "${YELLOW}Documentation:${NC}"
echo -e "  README: $(pwd)/README.md"
echo -e "  API Reference: $(pwd)/API_REFERENCE.md"
echo -e "  Deployment Guide: $(pwd)/DEPLOYMENT.md"
echo ""
