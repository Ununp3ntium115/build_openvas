#!/bin/bash
# Test script for The Commune MCP Server

BINARY="./target/release/cryptex-mcp-server"

echo "Testing The Commune MCP Server"
echo "================================"

# Test 1: Initialize
echo ""
echo "Test 1: Initialize"
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test-client","version":"1.0.0"}}}' | $BINARY

# Test 2: List Tools
echo ""
echo "Test 2: List Tools"
echo '{"jsonrpc":"2.0","id":2,"method":"tools/list","params":{}}' | $BINARY

# Test 3: Call assess_vulnerability tool
echo ""
echo "Test 3: Assess Vulnerability (CVE-2021-44228)"
echo '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"assess_vulnerability","arguments":{"cve_id":"CVE-2021-44228"}}}' | $BINARY
