#!/bin/bash
# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Cross-platform GVM user creation

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

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Create GVM user with OS-specific parameters
create_gvm_user() {
    local username="${USERNAME:-gvm}"
    
    if id "$username" &>/dev/null; then
        log_info "User '$username' already exists."
        return 0
    fi
    
    log_info "Creating GVM user '$username'..."
    
    case "${OS_ID:-ubuntu}" in
        ubuntu|debian|kali)
            # Debian-based systems
            useradd -r -M -U -G sudo -s /usr/sbin/nologin "$username"
            ;;
        arch|manjaro)
            # Arch-based systems
            useradd -r -M -U -G wheel -s /usr/bin/nologin "$username"
            ;;
        fedora|rhel|centos)
            # Red Hat-based systems
            useradd -r -M -U -G wheel -s /sbin/nologin "$username"
            ;;
        *)
            # Default fallback
            log_warning "Unknown OS, using default user creation"
            useradd -r -M -U -s /usr/sbin/nologin "$username" || \
            useradd -r -M -U -s /sbin/nologin "$username" || \
            useradd -r -M -U -s /usr/bin/nologin "$username"
            ;;
    esac
    
    log_success "User '$username' created successfully"
}

# Create additional groups if needed
create_additional_groups() {
    local username="${USERNAME:-gvm}"
    
    # Create redis group if it doesn't exist (some systems need this)
    if ! getent group redis >/dev/null 2>&1; then
        groupadd -r redis || true
    fi
    
    # Add gvm user to redis group for socket access
    usermod -a -G redis "$username" 2>/dev/null || true
    
    # Create mosquitto group if it doesn't exist
    if ! getent group mosquitto >/dev/null 2>&1; then
        groupadd -r mosquitto || true
    fi
    
    # Add gvm user to mosquitto group
    usermod -a -G mosquitto "$username" 2>/dev/null || true
    
    log_success "Additional group memberships configured"
}

# Main function
main() {
    echo "=== Creating GVM User ==="
    
    create_gvm_user
    create_additional_groups
    
    log_success "GVM user setup complete"
}

# Run main function
main "$@"
