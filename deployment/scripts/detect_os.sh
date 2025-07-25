#!/bin/bash
# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Cross-platform OS detection and configuration

set -e

# Global variables for OS detection
OS_ID=""
OS_VERSION=""
OS_CODENAME=""
PACKAGE_MANAGER=""
INSTALL_CMD=""
UPDATE_CMD=""
PYTHON_CMD=""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect operating system
detect_os() {
    log_info "Detecting operating system..."
    
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        OS_ID="$ID"
        OS_VERSION="$VERSION_ID"
        OS_CODENAME="$VERSION_CODENAME"
    elif [ -f /etc/lsb-release ]; then
        . /etc/lsb-release
        OS_ID="$DISTRIB_ID"
        OS_VERSION="$DISTRIB_RELEASE"
        OS_CODENAME="$DISTRIB_CODENAME"
    else
        log_error "Cannot detect operating system"
        exit 1
    fi
    
    # Convert to lowercase for consistency
    OS_ID=$(echo "$OS_ID" | tr '[:upper:]' '[:lower:]')
    
    log_info "Detected OS: $OS_ID $OS_VERSION ($OS_CODENAME)"
}

# Configure package manager based on OS
configure_package_manager() {
    case "$OS_ID" in
        ubuntu|debian)
            PACKAGE_MANAGER="apt"
            INSTALL_CMD="apt-get install -y"
            UPDATE_CMD="apt-get update"
            PYTHON_CMD="python3"
            ;;
        kali)
            PACKAGE_MANAGER="apt"
            INSTALL_CMD="apt-get install -y"
            UPDATE_CMD="apt-get update"
            PYTHON_CMD="python3"
            ;;
        arch|manjaro)
            PACKAGE_MANAGER="pacman"
            INSTALL_CMD="pacman -S --noconfirm"
            UPDATE_CMD="pacman -Sy"
            PYTHON_CMD="python"
            ;;
        fedora|rhel|centos)
            PACKAGE_MANAGER="dnf"
            INSTALL_CMD="dnf install -y"
            UPDATE_CMD="dnf update"
            PYTHON_CMD="python3"
            ;;
        *)
            log_error "Unsupported operating system: $OS_ID"
            log_info "Supported systems: Ubuntu, Kali Linux, Arch Linux"
            exit 1
            ;;
    esac
    
    log_success "Package manager configured: $PACKAGE_MANAGER"
}

# Check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        log_error "This script must be run as root"
        log_info "Please run: sudo $0"
        exit 1
    fi
}

# Update package database
update_packages() {
    log_info "Updating package database..."
    case "$PACKAGE_MANAGER" in
        apt)
            $UPDATE_CMD
            ;;
        pacman)
            $UPDATE_CMD
            ;;
        dnf)
            $UPDATE_CMD
            ;;
    esac
    log_success "Package database updated"
}

# Check system requirements
check_requirements() {
    log_info "Checking system requirements..."
    
    # Check available memory (minimum 4GB recommended)
    MEMORY_KB=$(grep MemTotal /proc/meminfo | awk '{print $2}')
    MEMORY_GB=$((MEMORY_KB / 1024 / 1024))
    
    if [ $MEMORY_GB -lt 4 ]; then
        log_warning "System has ${MEMORY_GB}GB RAM. Minimum 4GB recommended for OpenVAS"
    else
        log_success "Memory check passed: ${MEMORY_GB}GB RAM available"
    fi
    
    # Check available disk space (minimum 20GB recommended)
    DISK_SPACE_GB=$(df / | awk 'NR==2 {print int($4/1024/1024)}')
    if [ $DISK_SPACE_GB -lt 20 ]; then
        log_warning "Available disk space: ${DISK_SPACE_GB}GB. Minimum 20GB recommended"
    else
        log_success "Disk space check passed: ${DISK_SPACE_GB}GB available"
    fi
    
    # Check CPU cores
    CPU_CORES=$(nproc)
    if [ $CPU_CORES -lt 2 ]; then
        log_warning "System has ${CPU_CORES} CPU core(s). 2+ cores recommended"
    else
        log_success "CPU check passed: ${CPU_CORES} cores available"
    fi
}

# Export variables for use by other scripts
export_variables() {
    cat > /tmp/os_config.env << EOF
# OS Configuration for AI-Enhanced OpenVAS
export OS_ID="$OS_ID"
export OS_VERSION="$OS_VERSION"
export OS_CODENAME="$OS_CODENAME"
export PACKAGE_MANAGER="$PACKAGE_MANAGER"
export INSTALL_CMD="$INSTALL_CMD"
export UPDATE_CMD="$UPDATE_CMD"
export PYTHON_CMD="$PYTHON_CMD"
EOF
    
    log_success "OS configuration exported to /tmp/os_config.env"
}

# Main function
main() {
    echo "=== AI-Enhanced OpenVAS OS Detection ==="
    echo
    
    check_root
    detect_os
    configure_package_manager
    check_requirements
    update_packages
    export_variables
    
    echo
    log_success "OS detection and configuration complete!"
    log_info "You can now proceed with the installation"
}

# Run main function if script is executed directly
if [ "${BASH_SOURCE[0]}" == "${0}" ]; then
    main "$@"
fi