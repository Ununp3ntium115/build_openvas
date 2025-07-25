#!/bin/bash
# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Cross-platform installation script for AI-Enhanced OpenVAS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
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

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if [ -f /etc/debian_version ]; then
            OS="debian"
        elif [ -f /etc/redhat-release ]; then
            OS="redhat"
        else
            OS="linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="windows"
    else
        OS="unknown"
    fi
    
    log_info "Detected OS: $OS"
}

# Install dependencies based on OS
install_dependencies() {
    log_info "Installing dependencies for $OS..."
    
    case $OS in
        "debian")
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                pkg-config \
                libglib2.0-dev \
                libjson-glib-dev \
                libcurl4-openssl-dev \
                git \
                curl
            ;;
        "redhat")
            sudo yum install -y \
                gcc \
                gcc-c++ \
                cmake \
                pkgconfig \
                glib2-devel \
                json-glib-devel \
                libcurl-devel \
                git \
                curl
            ;;
        "macos")
            if ! command -v brew &> /dev/null; then
                log_error "Homebrew not found. Please install Homebrew first."
                exit 1
            fi
            brew install \
                cmake \
                pkg-config \
                glib \
                json-glib \
                curl \
                git
            ;;
        "windows")
            log_warning "Windows detected. Please ensure you have:"
            log_warning "- Visual Studio or MinGW-w64"
            log_warning "- CMake"
            log_warning "- vcpkg or similar package manager"
            log_warning "- Required libraries: glib, json-glib, curl"
            ;;
        *)
            log_error "Unsupported OS: $OS"
            exit 1
            ;;
    esac
    
    log_success "Dependencies installed successfully"
}

# Check if required tools are available
check_requirements() {
    log_info "Checking requirements..."
    
    local missing_tools=()
    
    if ! command -v cmake &> /dev/null; then
        missing_tools+=("cmake")
    fi
    
    if ! command -v git &> /dev/null; then
        missing_tools+=("git")
    fi
    
    if ! command -v pkg-config &> /dev/null; then
        missing_tools+=("pkg-config")
    fi
    
    if [ ${#missing_tools[@]} -ne 0 ]; then
        log_error "Missing required tools: ${missing_tools[*]}"
        log_info "Please install missing tools or run with --install-deps"
        exit 1
    fi
    
    log_success "All required tools are available"
}

# Build the project
build_project() {
    log_info "Building AI-Enhanced OpenVAS..."
    
    # Create build directory
    if [ -d "build" ]; then
        log_warning "Build directory exists, cleaning..."
        rm -rf build
    fi
    
    mkdir build
    cd build
    
    # Configure with CMake
    log_info "Configuring with CMake..."
    cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_AI_ENGINE=ON \
        -DBUILD_AI_PLUGINS=ON \
        -DBUILD_CORE_COMPONENTS=OFF \
        -DBUILD_TESTS=ON \
        ..
    
    # Build
    log_info "Compiling..."
    if command -v nproc &> /dev/null; then
        make -j$(nproc)
    else
        make -j4
    fi
    
    cd ..
    log_success "Build completed successfully"
}

# Run tests
run_tests() {
    log_info "Running tests..."
    
    cd build
    if [ -f "tests/test_ai_service" ]; then
        ./tests/test_ai_service
        log_success "Tests completed"
    else
        log_warning "Test binary not found, skipping tests"
    fi
    cd ..
}

# Install the project
install_project() {
    log_info "Installing AI-Enhanced OpenVAS..."
    
    cd build
    sudo make install
    cd ..
    
    log_success "Installation completed"
}

# Create sample configuration
create_sample_config() {
    log_info "Creating sample configuration..."
    
    local config_dir="$HOME/.config/ai-enhanced-openvas"
    mkdir -p "$config_dir"
    
    cat > "$config_dir/ai-config.conf" << EOF
# AI-Enhanced OpenVAS Configuration
# Copy this file and customize for your environment

[ai_providers]
# OpenAI Configuration
openai_enabled = true
openai_api_key = YOUR_OPENAI_API_KEY_HERE
openai_model = gpt-4
openai_timeout = 30

# Claude Configuration  
claude_enabled = false
claude_api_key = YOUR_CLAUDE_API_KEY_HERE
claude_model = claude-3-sonnet-20240229
claude_timeout = 30

# Local AI Configuration
local_ai_enabled = false
local_ai_endpoint = http://localhost:8080/v1/chat/completions
local_ai_model = local-model

[ai_features]
# Enable/disable AI features
vulnerability_analysis = true
threat_modeling = true
scan_optimization = true
report_generation = true
exploit_suggestion = false

[security]
# Security settings
sanitize_data = true
log_api_calls = true
max_concurrent_requests = 4
EOF
    
    log_success "Sample configuration created at $config_dir/ai-config.conf"
    log_warning "Please edit the configuration file and add your API keys"
}

# Main installation function
main() {
    local install_deps=false
    local run_tests_flag=true
    local install_flag=false
    
    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --install-deps)
                install_deps=true
                shift
                ;;
            --no-tests)
                run_tests_flag=false
                shift
                ;;
            --install)
                install_flag=true
                shift
                ;;
            -h|--help)
                echo "Usage: $0 [OPTIONS]"
                echo "Options:"
                echo "  --install-deps    Install system dependencies"
                echo "  --no-tests        Skip running tests"
                echo "  --install         Install after building"
                echo "  -h, --help        Show this help message"
                exit 0
                ;;
            *)
                log_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    log_info "Starting AI-Enhanced OpenVAS installation..."
    
    # Detect OS
    detect_os
    
    # Install dependencies if requested
    if [ "$install_deps" = true ]; then
        install_dependencies
    fi
    
    # Check requirements
    check_requirements
    
    # Build project
    build_project
    
    # Run tests
    if [ "$run_tests_flag" = true ]; then
        run_tests
    fi
    
    # Install if requested
    if [ "$install_flag" = true ]; then
        install_project
    fi
    
    # Create sample configuration
    create_sample_config
    
    log_success "AI-Enhanced OpenVAS setup completed!"
    log_info "Next steps:"
    log_info "1. Edit ~/.config/ai-enhanced-openvas/ai-config.conf"
    log_info "2. Add your AI provider API keys"
    log_info "3. Test the AI integration with your OpenVAS installation"
}

# Run main function
main "$@"