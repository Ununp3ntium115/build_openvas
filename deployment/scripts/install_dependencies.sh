#!/bin/bash
# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Dependency installation script for AI-Enhanced OpenVAS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

log_info() { echo -e "${BLUE}[INFO]${NC} $1"; }
log_success() { echo -e "${GREEN}[SUCCESS]${NC} $1"; }
log_warning() { echo -e "${YELLOW}[WARNING]${NC} $1"; }
log_error() { echo -e "${RED}[ERROR]${NC} $1"; }

# Ubuntu/Debian dependencies
install_debian_deps() {
    log_info "Installing dependencies for Debian/Ubuntu..."
    
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        pkg-config \
        git \
        curl \
        wget \
        libglib2.0-dev \
        libjson-glib-dev \
        libcurl4-openssl-dev \
        libssl-dev \
        libxml2-dev \
        libsqlite3-dev \
        uuid-dev \
        libgpgme-dev \
        libgnutls28-dev \
        libpcap-dev \
        libksba-dev \
        libldap2-dev \
        libhiredis-dev \
        libpq-dev \
        postgresql-server-dev-all \
        python3-dev \
        python3-pip \
        python3-setuptools \
        python3-wheel \
        nodejs \
        npm \
        yarn
    
    log_success "Debian/Ubuntu dependencies installed"
}

# CentOS/RHEL/Fedora dependencies
install_redhat_deps() {
    log_info "Installing dependencies for RedHat/CentOS/Fedora..."
    
    # Detect package manager
    if command -v dnf &> /dev/null; then
        PKG_MGR="dnf"
    elif command -v yum &> /dev/null; then
        PKG_MGR="yum"
    else
        log_error "No supported package manager found"
        exit 1
    fi
    
    sudo $PKG_MGR install -y \
        gcc \
        gcc-c++ \
        cmake \
        pkgconfig \
        git \
        curl \
        wget \
        glib2-devel \
        json-glib-devel \
        libcurl-devel \
        openssl-devel \
        libxml2-devel \
        sqlite-devel \
        libuuid-devel \
        gpgme-devel \
        gnutls-devel \
        libpcap-devel \
        libksba-devel \
        openldap-devel \
        hiredis-devel \
        postgresql-devel \
        python3-devel \
        python3-pip \
        nodejs \
        npm
    
    log_success "RedHat/CentOS/Fedora dependencies installed"
}

# macOS dependencies
install_macos_deps() {
    log_info "Installing dependencies for macOS..."
    
    if ! command -v brew &> /dev/null; then
        log_error "Homebrew not found. Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    fi
    
    brew install \
        cmake \
        pkg-config \
        git \
        curl \
        wget \
        glib \
        json-glib \
        libxml2 \
        sqlite \
        gpgme \
        gnutls \
        libpcap \
        libksba \
        openldap \
        hiredis \
        postgresql \
        python3 \
        node \
        yarn
    
    log_success "macOS dependencies installed"
}

# Python dependencies
install_python_deps() {
    log_info "Installing Python dependencies..."
    
    pip3 install --user \
        python-gvm \
        ospd \
        ospd-openvas \
        psutil \
        redis \
        paramiko \
        lxml \
        defusedxml \
        packaging \
        deprecated \
        pontos
    
    log_success "Python dependencies installed"
}

# Node.js dependencies for GSA
install_nodejs_deps() {
    log_info "Installing Node.js dependencies for GSA..."
    
    if [ -d "core/gsa" ]; then
        cd core/gsa
        if [ -f "package.json" ]; then
            npm install
            log_success "GSA Node.js dependencies installed"
        else
            log_warning "GSA package.json not found, skipping Node.js dependencies"
        fi
        cd ../..
    else
        log_warning "GSA directory not found, skipping Node.js dependencies"
    fi
}

# Detect OS and install appropriate dependencies
main() {
    log_info "Detecting operating system..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f /etc/debian_version ]; then
            install_debian_deps
        elif [ -f /etc/redhat-release ] || [ -f /etc/fedora-release ]; then
            install_redhat_deps
        else
            log_error "Unsupported Linux distribution"
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        install_macos_deps
    else
        log_error "Unsupported operating system: $OSTYPE"
        log_info "Supported systems: Linux (Debian/Ubuntu, RHEL/CentOS/Fedora), macOS"
        exit 1
    fi
    
    # Install Python and Node.js dependencies
    install_python_deps
    install_nodejs_deps
    
    log_success "All dependencies installed successfully!"
    log_info "You can now run: ./install_cross_platform.sh"
}

main "$@"