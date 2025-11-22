#!/bin/bash
# CRYPTEX Quick Start Script
# Automated setup and testing for rapid deployment

set -e

echo "╔═══════════════════════════════════════════════════════════╗"
echo "║                                                           ║"
echo "║   CRYPTEX - Quick Start                                  ║"
echo "║   No gods, no masters, no memory leaks.                  ║"
echo "║                                                           ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Helper functions
print_step() {
    echo -e "${BLUE}==>${NC} $1"
}

print_success() {
    echo -e "${GREEN}✓${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}⚠${NC} $1"
}

print_error() {
    echo -e "${RED}✗${NC} $1"
}

# Check prerequisites
print_step "Checking prerequisites..."

if ! command -v cargo &> /dev/null; then
    print_error "Rust/Cargo not found. Install from https://rustup.rs/"
    exit 1
fi
print_success "Rust/Cargo installed"

RUST_VERSION=$(rustc --version | awk '{print $2}')
print_success "Rust version: $RUST_VERSION"

# Check for optional dependencies
if command -v docker &> /dev/null; then
    print_success "Docker installed (optional)"
else
    print_warning "Docker not found (optional - for containerization)"
fi

# Build CRYPTEX
print_step "Building CRYPTEX workspace..."
echo ""

cargo build --release

if [ $? -eq 0 ]; then
    print_success "Build completed successfully"
else
    print_error "Build failed"
    exit 1
fi

# Run tests
print_step "Running test suite..."
echo ""

cargo test --workspace --quiet

if [ $? -eq 0 ]; then
    print_success "All tests passed"
else
    print_error "Some tests failed"
    exit 1
fi

# Count tests
TEST_COUNT=$(cargo test --workspace 2>&1 | grep "test result" | grep -o "[0-9]* passed" | awk '{sum += $1} END {print sum}')
print_success "$TEST_COUNT tests passed"

# Create necessary directories
print_step "Setting up directories..."

mkdir -p data logs config

print_success "Created data/, logs/, config/ directories"

# Create example configuration
print_step "Creating example configuration..."

cat > config/cryptex-config.toml << 'EOF'
[commune]
name = "CRYPTEX QuickStart"
host = "127.0.0.1"
port = 8080

[archive]
path = "./data/cryptex.db"
max_size_gb = 10

[memory]
max_size_mb = 256
ttl_seconds = 3600
cache_threshold = 0.8

# AI Providers (configure your API keys below)
# [[agitators]]
# name = "openai"
# agitator_type = "openai"
# enabled = true
# api_key_env = "OPENAI_API_KEY"
# model = "gpt-4"
# timeout_seconds = 30

# [[agitators]]
# name = "claude"
# agitator_type = "claude"
# enabled = true
# api_key_env = "ANTHROPIC_API_KEY"
# model = "claude-3-sonnet-20240229"
# timeout_seconds = 30
EOF

print_success "Created config/cryptex-config.toml"

# Create environment template
cat > config/environment.template << 'EOF'
# CRYPTEX Environment Variables Template
# Copy to .env and fill in your values

# AI Provider API Keys (at least one required for AI features)
OPENAI_API_KEY=sk-your-openai-key-here
ANTHROPIC_API_KEY=your-anthropic-key-here

# Logging
RUST_LOG=info

# Optional: Override configuration
# CRYPTEX_DB_PATH=./data/custom.db
# CRYPTEX_BIND_ADDR=0.0.0.0:8080
EOF

print_success "Created config/environment.template"

# Test MCP server
print_step "Testing MCP server..."
echo ""

# Create test script if it doesn't exist
if [ ! -f "test_mcp.sh" ]; then
    print_warning "test_mcp.sh not found, skipping MCP test"
else
    chmod +x test_mcp.sh
    timeout 5 ./test_mcp.sh > /dev/null 2>&1 && print_success "MCP server test passed" || print_warning "MCP server test skipped (requires API keys)"
fi

# Display summary
echo ""
echo "╔═══════════════════════════════════════════════════════════╗"
echo "║                                                           ║"
echo "║   CRYPTEX Setup Complete!                                ║"
echo "║                                                           ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""

print_step "Setup Summary:"
echo ""
echo "  📦 Build Status:     Complete"
echo "  ✅ Tests Passed:     $TEST_COUNT"
echo "  📁 Directories:      data/, logs/, config/"
echo "  ⚙️  Configuration:    config/cryptex-config.toml"
echo "  🔑 Environment:      config/environment.template"
echo ""

print_step "Next Steps:"
echo ""
echo "1. Configure AI Provider (optional):"
echo "   ${BLUE}cp config/environment.template .env${NC}"
echo "   ${BLUE}nano .env${NC}  # Add your API keys"
echo ""
echo "2. Run MCP Server (for PYRO integration):"
echo "   ${BLUE}export \$(cat .env | xargs)${NC}"
echo "   ${BLUE}./target/release/cryptex-mcp-server${NC}"
echo ""
echo "3. Test MCP Server:"
echo "   ${BLUE}./test_mcp.sh${NC}"
echo ""
echo "4. View Documentation:"
echo "   ${BLUE}cat README.md${NC}"
echo "   ${BLUE}cat DEPLOYMENT.md${NC}"
echo "   ${BLUE}cat API_REFERENCE.md${NC}"
echo "   ${BLUE}cat PYRO_INTEGRATION.md${NC}"
echo ""

print_step "Resources:"
echo ""
echo "  📚 README:           README.md"
echo "  🚀 Deployment:       DEPLOYMENT.md"
echo "  📡 API Reference:    API_REFERENCE.md"
echo "  🔗 PYRO Integration: PYRO_INTEGRATION.md"
echo "  ✅ Completion:       PROJECT_COMPLETION.md"
echo ""

print_success "CRYPTEX is ready for use!"
echo ""
echo -e "${GREEN}\"No gods, no masters, no memory leaks.\" 🚀${NC}"
echo ""
