#!/bin/bash
# PYRO Platform + openFireVulnerability Integration Setup
# Automated integration script for Fire Marshal

set -e

echo "🔥 openFireVulnerability → PYRO Platform Integration Setup"
echo "============================================================"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Paths
OPENFIRE_ROOT="/home/user/build_openvas"
PYRO_ROOT="/home/user/PYRO_Platform_Ignition"
MCP_SERVER="${OPENFIRE_ROOT}/cryptex/target/release/cryptex-mcp-server"
INTEGRATION_HELPER="${OPENFIRE_ROOT}/cryptex/target/release/pyro-integration-mcp"

# Step 1: Validate openFireVulnerability installation
echo "📋 Step 1: Validating openFireVulnerability installation..."
if [ ! -f "$INTEGRATION_HELPER" ]; then
    echo -e "${RED}❌ PYRO Integration Helper not found${NC}"
    echo "Building integration helper..."
    cd "${OPENFIRE_ROOT}/cryptex"
    cargo build --release -p pyro_integration_helper
fi

# Run validation
VALIDATION=$(echo '{"jsonrpc":"2.0","id":1,"method":"tools/call","params":{"name":"validate_integration","arguments":{}}}' | \
    "$INTEGRATION_HELPER" 2>&1 | grep -v "^PYRO" | jq '.result.content[0].text' | jq -r '.')

OVERALL_STATUS=$(echo "$VALIDATION" | jq -r '.overall_status')

if [ "$OVERALL_STATUS" = "ready" ]; then
    echo -e "${GREEN}✅ openFireVulnerability is ready for integration${NC}"
    echo "$VALIDATION" | jq '.summary'
else
    echo -e "${YELLOW}⚠️  Some checks failed:${NC}"
    echo "$VALIDATION" | jq '.checks[] | select(.status == false) | .message'
    echo ""
    echo "Please fix the issues above before continuing."
    exit 1
fi

echo ""

# Step 2: Check PYRO Platform installation
echo "📋 Step 2: Checking PYRO Platform installation..."
if [ ! -d "$PYRO_ROOT" ]; then
    echo -e "${YELLOW}⚠️  PYRO Platform not found at: $PYRO_ROOT${NC}"
    echo ""
    read -p "Would you like to clone PYRO Platform? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Cloning PYRO Platform..."
        git clone https://github.com/Ununp3ntium115/PYRO_Platform_Ignition.git "$PYRO_ROOT"
    else
        echo -e "${RED}❌ PYRO Platform is required for integration${NC}"
        exit 1
    fi
fi

echo -e "${GREEN}✅ PYRO Platform found${NC}"
echo ""

# Step 3: Generate and install MCP configuration
echo "📋 Step 3: Generating MCP server configuration..."

# Create config directory if it doesn't exist
mkdir -p "${PYRO_ROOT}/config"

# Generate configuration
MCP_CONFIG=$(echo '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"generate_pyro_config","arguments":{"pyro_path":"'"$PYRO_ROOT"'"}}}' | \
    "$INTEGRATION_HELPER" 2>&1 | grep -v "^PYRO" | jq '.result.content[0].text' | jq -r '.mcp_servers_json')

# Write configuration
echo "$MCP_CONFIG" > "${PYRO_ROOT}/config/openfire_mcp.json"
echo -e "${GREEN}✅ MCP configuration written to: ${PYRO_ROOT}/config/openfire_mcp.json${NC}"

# Also update the main mcp_servers.json if it exists
if [ -f "${PYRO_ROOT}/mcp_servers.json" ]; then
    echo "Merging with existing mcp_servers.json..."
    # Backup existing
    cp "${PYRO_ROOT}/mcp_servers.json" "${PYRO_ROOT}/mcp_servers.json.backup"

    # Merge configurations
    jq -s '.[0] * .[1]' "${PYRO_ROOT}/mcp_servers.json" <(echo "$MCP_CONFIG") > "${PYRO_ROOT}/mcp_servers.json.tmp"
    mv "${PYRO_ROOT}/mcp_servers.json.tmp" "${PYRO_ROOT}/mcp_servers.json"
    echo -e "${GREEN}✅ Merged with existing mcp_servers.json${NC}"
fi

echo ""

# Step 4: Test MCP connection
echo "📋 Step 4: Testing MCP server connection..."
TEST_RESULT=$(echo '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"test_mcp_connection","arguments":{}}}' | \
    "$INTEGRATION_HELPER" 2>&1 | grep -v "^PYRO\|^CRYPTEX" | jq '.result.content[0].text' | jq -r '.')

TEST_STATUS=$(echo "$TEST_RESULT" | jq -r '.status')

if [ "$TEST_STATUS" = "success" ]; then
    echo -e "${GREEN}✅ MCP server connection successful${NC}"
else
    echo -e "${RED}❌ MCP server connection failed${NC}"
    echo "$TEST_RESULT" | jq '.message'
    exit 1
fi

echo ""

# Step 5: Get MCP tools
echo "📋 Step 5: Listing available MCP tools..."
TOOLS=$(echo '{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"get_mcp_tools","arguments":{}}}' | \
    "$INTEGRATION_HELPER" 2>&1 | grep -v "^PYRO\|^CRYPTEX" | jq '.result.content[0].text' | jq -r '.')

TOOLS_STATUS=$(echo "$TOOLS" | jq -r '.status')

if [ "$TOOLS_STATUS" = "success" ]; then
    echo -e "${GREEN}✅ Successfully retrieved MCP tools:${NC}"
    echo "$TOOLS" | jq -r '.tools[] | "   • \(.name) - \(.description)"'
else
    echo -e "${RED}❌ Failed to retrieve MCP tools${NC}"
fi

echo ""

# Step 6: Copy documentation
echo "📋 Step 6: Copying integration documentation..."
mkdir -p "${PYRO_ROOT}/docs/integrations"

cp "${OPENFIRE_ROOT}/PYRO_MCP_INTEGRATION.md" "${PYRO_ROOT}/docs/integrations/openFireVulnerability.md"
cp "${OPENFIRE_ROOT}/cryptex/PYRO_INTEGRATION.md" "${PYRO_ROOT}/docs/integrations/openFireVulnerability_detailed.md"
cp "${OPENFIRE_ROOT}/cryptex/API_REFERENCE.md" "${PYRO_ROOT}/docs/integrations/openFireVulnerability_api.md"

echo -e "${GREEN}✅ Documentation copied to: ${PYRO_ROOT}/docs/integrations/${NC}"
echo ""

# Step 7: Create database directory
echo "📋 Step 7: Setting up database directory..."
mkdir -p ~/.local/share/openFireVulnerability
echo -e "${GREEN}✅ Database directory created: ~/.local/share/openFireVulnerability${NC}"
echo ""

# Step 8: Summary
echo "============================================================"
echo -e "${GREEN}🔥 Integration Complete!${NC}"
echo "============================================================"
echo ""
echo "Configuration files:"
echo "  • MCP Config: ${PYRO_ROOT}/config/openfire_mcp.json"
echo "  • System Config: ${OPENFIRE_ROOT}/cryptex/openfire-config.toml"
echo "  • Database: ~/.local/share/openFireVulnerability/openfire.redb"
echo ""
echo "MCP Servers:"
echo "  • openfire - openFireVulnerability (5 tools)"
echo "  • Binary: $MCP_SERVER"
echo ""
echo "Documentation:"
echo "  • ${PYRO_ROOT}/docs/integrations/openFireVulnerability.md"
echo "  • ${PYRO_ROOT}/docs/integrations/openFireVulnerability_detailed.md"
echo "  • ${PYRO_ROOT}/docs/integrations/openFireVulnerability_api.md"
echo ""
echo "Next steps:"
echo "  1. Review configuration in ${PYRO_ROOT}/config/openfire_mcp.json"
echo "  2. Start PYRO Platform"
echo "  3. Test openFireVulnerability tools from Fire Marshal"
echo ""
echo "Example Fire Marshal usage:"
echo "  python3 -c \\"
echo "    from pyro import Agent"
echo "    agent = Agent(name='SecurityAnalyst', mcp_servers=['openfire'])"
echo "    result = await agent.use_tool("
echo "        server='openfire',"
echo "        tool='assess_vulnerability',"
echo "        arguments={'cve_id': 'CVE-2021-44228'}"
echo "    )"
echo "  \\""
echo ""
echo -e "${GREEN}Integration ready! 🔥${NC}"
