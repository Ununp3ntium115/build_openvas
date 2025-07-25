#!/bin/bash
# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Cross-platform directory creation

set -e

source ../exports.sh

# Source OS configuration if available
if [ -f /tmp/os_config.env ]; then
    source /tmp/os_config.env
fi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

echo "=== Creating Build Directories ==="

log_info "Creating source, build, and install directories..."

# Create core directories
mkdir -p "$SOURCE_DIR"
mkdir -p "$BUILD_DIR"
mkdir -p "$INSTALL_DIR"

# Create AI-specific directories
mkdir -p "$SOURCE_DIR/ai-engine"
mkdir -p "$BUILD_DIR/ai-engine"
mkdir -p "$INSTALL_DIR/ai-engine"

# Create log directories with proper permissions
mkdir -p /var/log/gvm
mkdir -p /var/log/ai-openvas
mkdir -p /var/lib/gvm
mkdir -p /var/lib/ai-openvas

# Set ownership
chown -R gvm:gvm /var/log/gvm /var/lib/gvm 2>/dev/null || true
chown -R gvm:gvm /var/log/ai-openvas /var/lib/ai-openvas 2>/dev/null || true

# Create run directories
mkdir -p /run/gvm
mkdir -p /run/ai-openvas
chown -R gvm:gvm /run/gvm /run/ai-openvas 2>/dev/null || true

log_success "Build directories created successfully"
log_info "Source directory: $SOURCE_DIR"
log_info "Build directory: $BUILD_DIR"
log_info "Install directory: $INSTALL_DIR"
