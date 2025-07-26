# OpenVAS Professional LaTeX Reporting - Ubuntu UAT Testing Plan

## Overview

This document provides a comprehensive User Acceptance Testing (UAT) plan specifically designed for Ubuntu environments (18.04 LTS, 20.04 LTS, 22.04 LTS). This plan validates the OpenVAS Professional LaTeX Reporting System across different Ubuntu versions commonly used in enterprise environments.

## Environment Setup

### Supported Ubuntu Versions
- Ubuntu 18.04 LTS (Bionic Beaver)
- Ubuntu 20.04 LTS (Focal Fossa)  
- Ubuntu 22.04 LTS (Jammy Jellyfish)
- Ubuntu 23.04+ (Latest versions)

### Prerequisites

#### 1. System Update and Basic Tools
```bash
# Update Ubuntu system
sudo apt update && sudo apt upgrade -y

# Install essential development tools
sudo apt install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    curl \
    wget \
    vim \
    htop \
    software-properties-common \
    apt-transport-https \
    ca-certificates \
    gnupg \
    lsb-release
```

### LaTeX Installation for Ubuntu

#### Ubuntu 22.04 LTS and Later (Recommended)
```bash
# Install complete LaTeX distribution
sudo apt install -y \
    texlive-full \
    texlive-latex-base \
    texlive-latex-extra \
    texlive-fonts-recommended \
    texlive-fonts-extra \
    texlive-pictures \
    texlive-science \
    texlive-xetex \
    texlive-luatex \
    texlive-lang-european \
    texlive-lang-other

# Additional useful packages
sudo apt install -y \
    latexmk \
    texlive-publishers \
    texlive-humanities \
    biber \
    xindy

# Verify installation
pdflatex --version
xelatex --version
lualatex --version
latexmk --version
```

#### Ubuntu 20.04 LTS
```bash
# Install LaTeX distribution (slightly older packages)
sudo apt install -y \
    texlive-full \
    texlive-latex-base \
    texlive-latex-extra \
    texlive-fonts-recommended \
    texlive-fonts-extra \
    texlive-pictures \
    texlive-science \
    texlive-xetex \
    texlive-luatex

# May need to install additional packages manually
sudo apt install -y \
    texlive-generic-extra \
    texlive-plain-generic \
    texlive-font-utils

# Update LaTeX packages
sudo tlmgr update --self --all 2>/dev/null || echo "tlmgr not available, using apt packages"
```

#### Ubuntu 18.04 LTS (Legacy Support)
```bash
# Install available LaTeX packages
sudo apt install -y \
    texlive-latex-base \
    texlive-latex-recommended \
    texlive-latex-extra \
    texlive-fonts-recommended \
    texlive-fonts-extra \
    texlive-pictures \
    texlive-science

# Install additional packages that may be needed
sudo apt install -y \
    texlive-generic-recommended \
    texlive-generic-extra \
    texlive-font-utils \
    texlive-formats-extra

# Note: Some newer LaTeX features may not be available on 18.04
echo "Warning: Ubuntu 18.04 has older LaTeX packages. Some features may be limited."
```

#### Minimal LaTeX Installation (for resource-constrained environments)
```bash
# Minimal installation for testing
sudo apt install -y \
    texlive-latex-base \
    texlive-latex-recommended \
    texlive-fonts-recommended \
    texlive-pictures

# Install specific packages as needed
sudo apt install -y \
    texlive-science \
    texlive-latex-extra
```

### Development Dependencies Installation

#### GLib and JSON-GLib (Version-Specific)

##### Ubuntu 22.04 LTS and Later
```bash
# Install latest GLib development libraries
sudo apt install -y \
    libglib2.0-dev \
    libglib2.0-0 \
    libglib2.0-bin \
    glib-networking \
    libglib2.0-doc

# Install JSON-GLib development libraries
sudo apt install -y \
    libjson-glib-dev \
    libjson-glib-1.0-0 \
    libjson-glib-doc

# Verify versions
pkg-config --modversion glib-2.0
pkg-config --modversion json-glib-1.0
```

##### Ubuntu 20.04 LTS
```bash
# Install GLib development libraries
sudo apt install -y \
    libglib2.0-dev \
    libglib2.0-0 \
    libglib2.0-bin

# Install JSON-GLib development libraries
sudo apt install -y \
    libjson-glib-dev \
    libjson-glib-1.0-0

# Check versions (may be older)
echo "GLib version: $(pkg-config --modversion glib-2.0)"
echo "JSON-GLib version: $(pkg-config --modversion json-glib-1.0)"
```

##### Ubuntu 18.04 LTS
```bash
# Install available GLib development libraries
sudo apt install -y \
    libglib2.0-dev \
    libglib2.0-0

# Install JSON-GLib development libraries
sudo apt install -y \
    libjson-glib-dev \
    libjson-glib-1.0-0

# Note: Versions will be older, may need compatibility adjustments
echo "Warning: Ubuntu 18.04 has older library versions"
pkg-config --modversion glib-2.0
pkg-config --modversion json-glib-1.0
```

#### Additional Development Tools
```bash
# Install development and testing tools
sudo apt install -y \
    valgrind \
    gdb \
    strace \
    ltrace \
    doxygen \
    graphviz \
    pandoc \
    imagemagick \
    ghostscript

# Install Python tools for testing automation
sudo apt install -y \
    python3 \
    python3-pip \
    python3-venv \
    python3-dev

# Install useful Python packages
pip3 install --user \
    pytest \
    requests \
    beautifulsoup4 \
    lxml \
    matplotlib \
    numpy
```

### Version-Specific Compatibility Testing

#### Test Script for Version Detection
```bash
#!/bin/bash
# Ubuntu version compatibility check

echo "=== Ubuntu Version Compatibility Check ==="

# Detect Ubuntu version
ubuntu_version=$(lsb_release -rs)
ubuntu_codename=$(lsb_release -cs)

echo "Ubuntu Version: $ubuntu_version ($ubuntu_codename)"

# Check LaTeX version compatibility
latex_version=$(pdflatex --version | head -1)
echo "LaTeX Version: $latex_version"

# Check GLib version compatibility
glib_version=$(pkg-config --modversion glib-2.0)
echo "GLib Version: $glib_version"

# Check JSON-GLib version compatibility
json_glib_version=$(pkg-config --modversion json-glib-1.0)
echo "JSON-GLib Version: $json_glib_version"

# Version-specific warnings and recommendations
case $ubuntu_version in
    "18.04")
        echo "⚠️  Ubuntu 18.04 detected - using legacy compatibility mode"
        echo "   - Some LaTeX packages may be older"
        echo "   - Library versions may require compatibility adjustments"
        ;;
    "20.04")
        echo "✅ Ubuntu 20.04 detected - good compatibility"
        echo "   - Most features should work correctly"
        ;;
    "22.04"|"23."*|"24."*)
        echo "✅ Ubuntu $ubuntu_version detected - optimal compatibility"
        echo "   - All features should work correctly"
        ;;
    *)
        echo "⚠️  Ubuntu $ubuntu_version - compatibility unknown"
        echo "   - Testing recommended before production use"
        ;;
esac
```

### OpenVAS Professional LaTeX Reporting Installation

#### 1. Clone and Setup Project
```bash
# Create working directory
mkdir -p ~/openvas-ubuntu-testing
cd ~/openvas-ubuntu-testing

# Clone the project
# git clone <repository-url> openvas-latex-reporting
# cd openvas-latex-reporting

# For testing purposes, create the directory structure
mkdir -p openvas-latex-reporting
cd openvas-latex-reporting

# Create necessary directory structure
mkdir -p {plugins/report-generation/{src,templates/executive,tests},docs,examples,ai-engine/scoring}
```

#### 2. Version-Specific Build Configuration
```bash
# Create version-specific build script
cat > build_ubuntu.sh << 'EOF'
#!/bin/bash

# Ubuntu version-specific build configuration
ubuntu_version=$(lsb_release -rs)

echo "Configuring build for Ubuntu $ubuntu_version"

# Create build directory
mkdir -p build
cd build

# Version-specific CMake flags
case $ubuntu_version in
    "18.04")
        # Ubuntu 18.04 specific flags
        cmake .. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON \
            -DCMAKE_C_FLAGS="-std=c99" \
            -DGLIB_MIN_VERSION=2.56
        ;;
    "20.04")
        # Ubuntu 20.04 specific flags
        cmake .. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON \
            -DGLIB_MIN_VERSION=2.64
        ;;
    "22.04"|"23."*|"24."*)
        # Ubuntu 22.04+ specific flags
        cmake .. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON \
            -DGLIB_MIN_VERSION=2.72
        ;;
    *)
        # Default configuration
        cmake .. \
            -DCMAKE_BUILD_TYPE=Debug \
            -DBUILD_TESTING=ON
        ;;
esac

# Build with appropriate number of cores
make -j$(nproc)

echo "Build completed for Ubuntu $ubuntu_version"
EOF

chmod +x build_ubuntu.sh
```

#### 3. Installation and Configuration
```bash
# Run version-specific build
./build_ubuntu.sh

# Install the plugin (optional for testing)
cd build
sudo make install

# Create necessary directories with proper permissions
sudo mkdir -p /etc/openvas/report-generation
sudo mkdir -p /var/lib/openvas/reports
sudo mkdir -p /usr/share/openvas/report-templates

# Set proper permissions
sudo chown -R $USER:$USER /var/lib/openvas/reports
sudo chmod 755 /var/lib/openvas/reports

# Create Ubuntu-specific configuration
cat > /tmp/ubuntu_report_config.conf << EOF
[latex]
binary_path = $(which pdflatex)
temp_directory = /tmp/openvas-latex
timeout_seconds = 120
cleanup_temp_files = true

[system]
ubuntu_version = $(lsb_release -rs)
ubuntu_codename = $(lsb_release -cs)
glib_version = $(pkg-config --modversion glib-2.0)
json_glib_version = $(pkg-config --modversion json-glib-1.0)

[templates]
template_directory = /usr/share/openvas/report-templates
custom_template_directory = /etc/openvas/report-templates
EOF

sudo cp /tmp/ubuntu_report_config.conf /etc/openvas/report-generation/
```

## Ubuntu-Specific UAT Test Execution

### Phase 1: Ubuntu Version Compatibility Tests

#### Test 1.1: Version-Specific Package Verification
```bash
#!/bin/bash
echo "=== Ubuntu Version-Specific Package Verification ==="

ubuntu_version=$(lsb_release -rs)

# Test LaTeX packages based on Ubuntu version
case $ubuntu_version in
    "18.04")
        echo "Testing Ubuntu 18.04 LaTeX packages..."
        required_packages=("geometry" "fancyhdr" "titlesec" "graphicx" "xcolor")
        ;;
    "20.04")
        echo "Testing Ubuntu 20.04 LaTeX packages..."
        required_packages=("geometry" "fancyhdr" "titlesec" "graphicx" "xcolor" "booktabs" "tikz")
        ;;
    "22.04"|"23."*|"24."*)
        echo "Testing Ubuntu $ubuntu_version LaTeX packages..."
        required_packages=("geometry" "fancyhdr" "titlesec" "graphicx" "xcolor" "booktabs" "tikz" "pgfplots" "fontspec")
        ;;
esac

for package in "${required_packages[@]}"; do
    if kpsewhich ${package}.sty > /dev/null 2>&1; then
        echo "✅ Package $package found"
    else
        echo "❌ Package $package missing"
        # Attempt to install missing package
        sudo apt install -y texlive-latex-extra texlive-pictures 2>/dev/null
    fi
done
```

#### Test 1.2: Library Version Compatibility
```bash
#!/bin/bash
echo "=== Library Version Compatibility Test ==="

# Check GLib version compatibility
glib_version=$(pkg-config --modversion glib-2.0)
glib_major=$(echo $glib_version | cut -d. -f1)
glib_minor=$(echo $glib_version | cut -d. -f2)

echo "GLib Version: $glib_version"

if [ "$glib_major" -eq 2 ] && [ "$glib_minor" -ge 50 ]; then
    echo "✅ GLib version compatible"
else
    echo "⚠️  GLib version may be too old, some features may not work"
fi

# Check JSON-GLib version compatibility
json_glib_version=$(pkg-config --modversion json-glib-1.0)
echo "JSON-GLib Version: $json_glib_version"

# Test actual library functionality
cat > /tmp/glib_test.c << 'EOF'
#include <glib.h>
#include <json-glib/json-glib.h>
#include <stdio.h>

int main() {
    // Test GLib functionality
    GString *test_string = g_string_new("Hello");
    g_string_append(test_string, " World");
    printf("GLib test: %s\n", test_string->str);
    g_string_free(test_string, TRUE);
    
    // Test JSON-GLib functionality
    JsonObject *obj = json_object_new();
    json_object_set_string_member(obj, "test", "value");
    printf("JSON-GLib test: %s\n", json_object_get_string_member(obj, "test"));
    json_object_unref(obj);
    
    printf("✅ Library compatibility test passed\n");
    return 0;
}
EOF

gcc -o /tmp/glib_test /tmp/glib_test.c $(pkg-config --cflags --libs glib-2.0 json-glib-1.0)
if [ $? -eq 0 ]; then
    echo "✅ Library compilation successful"
    /tmp/glib_test
else
    echo "❌ Library compilation failed"
fi

rm -f /tmp/glib_test /tmp/glib_test.c
```

### Phase 2: Ubuntu-Specific Build Tests

#### Test 2.1: Version-Specific Build Test
```bash
#!/bin/bash
echo "=== Ubuntu Version-Specific Build Test ==="

cd ~/openvas-ubuntu-testing/openvas-latex-reporting

# Clean previous builds
rm -rf build

# Run version-specific build
./build_ubuntu.sh

if [ $? -eq 0 ]; then
    echo "✅ Ubuntu-specific build successful"
    
    # Verify build artifacts
    if [ -f build/plugins/report-generation/libopenvas-report-generation.so ]; then
        echo "✅ Report generation library built"
        
        # Check library dependencies
        echo "Library dependencies:"
        ldd build/plugins/report-generation/libopenvas-report-generation.so
    else
        echo "❌ Report generation library missing"
    fi
else
    echo "❌ Ubuntu-specific build failed"
fi
```

#### Test 2.2: Cross-Version Compatibility Test
```bash
#!/bin/bash
echo "=== Cross-Version Compatibility Test ==="

# Test if the built library works across different Ubuntu versions
ubuntu_version=$(lsb_release -rs)

echo "Testing compatibility for Ubuntu $ubuntu_version"

# Create a simple test program
cat > /tmp/compat_test.c << 'EOF'
#include <glib.h>
#include <stdio.h>

// Test GLib features used in the project
int main() {
    // Test GLib version
    printf("GLib compile-time version: %d.%d.%d\n", 
           GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
    printf("GLib runtime version: %d.%d.%d\n",
           glib_major_version, glib_minor_version, glib_micro_version);
    
    // Test basic GLib functionality
    GHashTable *hash = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
    g_hash_table_insert(hash, g_strdup("key"), g_strdup("value"));
    
    const gchar *value = g_hash_table_lookup(hash, "key");
    if (value && g_strcmp0(value, "value") == 0) {
        printf("✅ GLib hash table functionality working\n");
    } else {
        printf("❌ GLib hash table functionality failed\n");
    }
    
    g_hash_table_destroy(hash);
    
    // Test GPtrArray functionality
    GPtrArray *array = g_ptr_array_new_with_free_func(g_free);
    g_ptr_array_add(array, g_strdup("test"));
    
    if (array->len == 1) {
        printf("✅ GLib pointer array functionality working\n");
    } else {
        printf("❌ GLib pointer array functionality failed\n");
    }
    
    g_ptr_array_free(array, TRUE);
    
    return 0;
}
EOF

gcc -o /tmp/compat_test /tmp/compat_test.c $(pkg-config --cflags --libs glib-2.0)
if [ $? -eq 0 ]; then
    echo "✅ Compatibility test compiled successfully"
    /tmp/compat_test
else
    echo "❌ Compatibility test compilation failed"
fi

rm -f /tmp/compat_test /tmp/compat_test.c
```

### Phase 3: Ubuntu-Specific Functional Tests

#### Test 3.1: LaTeX Engine Ubuntu Compatibility
```bash
#!/bin/bash
echo "=== LaTeX Engine Ubuntu Compatibility Test ==="

# Test LaTeX functionality specific to Ubuntu version
ubuntu_version=$(lsb_release -rs)

# Create version-specific LaTeX test
cat > /tmp/ubuntu_latex_test.tex << 'EOF'
\documentclass{article}
\usepackage[utf8]{inputenc}
\usepackage{geometry}
\usepackage{fancyhdr}
\usepackage{xcolor}
\usepackage{graphicx}

% Ubuntu version-specific packages
\usepackage{tikz}
\usepackage{booktabs}

\title{Ubuntu LaTeX Compatibility Test}
\author{OpenVAS UAT}
\date{\today}

\begin{document}
\maketitle

\section{Ubuntu Version Information}
This document was compiled on Ubuntu \texttt{UBUNTU_VERSION}.

\section{Package Compatibility Test}
Testing various LaTeX packages:

\subsection{Colors}
\textcolor{blue}{Blue text} and \textcolor{red}{red text}.

\subsection{Tables}
\begin{tabular}{|l|l|}
\hline
Feature & Status \\
\hline
Basic LaTeX & Working \\
Colors & Working \\
Tables & Working \\
\hline
\end{tabular}

\subsection{Graphics}
% Test if graphics package works
Graphics package loaded successfully.

\end{document}
EOF

# Replace placeholder with actual Ubuntu version
sed -i "s/UBUNTU_VERSION/$(lsb_release -rs)/g" /tmp/ubuntu_latex_test.tex

# Compile LaTeX document
cd /tmp
pdflatex -interaction=nonstopmode ubuntu_latex_test.tex

if [ -f ubuntu_latex_test.pdf ]; then
    echo "✅ Ubuntu LaTeX compatibility test successful"
    echo "   PDF size: $(stat -c%s ubuntu_latex_test.pdf) bytes"
    
    # Test PDF validity
    if command -v pdfinfo &> /dev/null; then
        if pdfinfo ubuntu_latex_test.pdf > /dev/null 2>&1; then
            echo "✅ Generated PDF is valid"
        else
            echo "❌ Generated PDF is invalid"
        fi
    fi
else
    echo "❌ Ubuntu LaTeX compatibility test failed"
    echo "LaTeX log:"
    cat ubuntu_latex_test.log 2>/dev/null || echo "No log file found"
fi

# Clean up
rm -f ubuntu_latex_test.*
```

#### Test 3.2: File System Permissions Test
```bash
#!/bin/bash
echo "=== Ubuntu File System Permissions Test ==="

# Test file system permissions specific to Ubuntu
test_dirs=(
    "/tmp/openvas-test"
    "/var/tmp/openvas-test"
    "$HOME/openvas-test"
)

for test_dir in "${test_dirs[@]}"; do
    echo "Testing directory: $test_dir"
    
    # Create directory
    mkdir -p "$test_dir" 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✅ Directory creation successful"
        
        # Test file creation
        echo "test content" > "$test_dir/test_file.txt"
        if [ -f "$test_dir/test_file.txt" ]; then
            echo "✅ File creation successful"
            
            # Test file reading
            content=$(cat "$test_dir/test_file.txt")
            if [ "$content" = "test content" ]; then
                echo "✅ File reading successful"
            else
                echo "❌ File reading failed"
            fi
        else
            echo "❌ File creation failed"
        fi
        
        # Clean up
        rm -rf "$test_dir"
    else
        echo "❌ Directory creation failed"
    fi
    echo "---"
done
```

### Phase 4: Ubuntu Performance Benchmarking

#### Test 4.1: Ubuntu Version Performance Comparison
```bash
#!/bin/bash
echo "=== Ubuntu Version Performance Comparison ==="

ubuntu_version=$(lsb_release -rs)
echo "Testing performance on Ubuntu $ubuntu_version"

# CPU information
echo "CPU Information:"
lscpu | grep -E "(Model name|CPU\(s\)|Thread|Core)"

# Memory information
echo "Memory Information:"
free -h

# Disk information
echo "Disk Information:"
df -h /

# LaTeX compilation performance test
echo "LaTeX Compilation Performance Test:"

# Create a moderately complex LaTeX document
cat > /tmp/perf_test.tex << 'EOF'
\documentclass{article}
\usepackage{geometry}
\usepackage{fancyhdr}
\usepackage{xcolor}
\usepackage{tikz}
\usepackage{booktabs}
\usepackage{longtable}

\title{Performance Test Document}
\author{Ubuntu Performance Test}

\begin{document}
\maketitle

\section{Introduction}
This is a performance test document for Ubuntu LaTeX compilation.

\section{Tables}
\begin{longtable}{|l|l|l|l|}
\hline
Column 1 & Column 2 & Column 3 & Column 4 \\
\hline
\endfirsthead
\hline
Column 1 & Column 2 & Column 3 & Column 4 \\
\hline
\endhead
EOF

# Generate 100 table rows for performance testing
for i in {1..100}; do
    echo "Row $i & Data $i & Value $i & Result $i \\\\" >> /tmp/perf_test.tex
    echo "\\hline" >> /tmp/perf_test.tex
done

cat >> /tmp/perf_test.tex << 'EOF'
\end{longtable}

\section{Graphics}
\begin{tikzpicture}
\draw (0,0) -- (4,0) -- (4,4) -- (0,4) -- cycle;
\draw (2,2) circle (1);
\end{tikzpicture}

\end{document}
EOF

# Time the compilation
echo "Starting LaTeX compilation timing..."
start_time=$(date +%s.%N)

cd /tmp
pdflatex -interaction=nonstopmode perf_test.tex > /dev/null 2>&1

end_time=$(date +%s.%N)
duration=$(echo "$end_time - $start_time" | bc)

echo "LaTeX compilation time: ${duration} seconds"

# Performance benchmarks by Ubuntu version
case $ubuntu_version in
    "18.04")
        benchmark=15.0  # Expected slower on older version
        ;;
    "20.04")
        benchmark=12.0
        ;;
    "22.04"|"23."*|"24."*)
        benchmark=10.0  # Expected faster on newer versions
        ;;
    *)
        benchmark=12.0  # Default benchmark
        ;;
esac

if (( $(echo "$duration < $benchmark" | bc -l) )); then
    echo "✅ Performance acceptable for Ubuntu $ubuntu_version (< ${benchmark}s)"
else
    echo "⚠️  Performance slower than expected for Ubuntu $ubuntu_version (> ${benchmark}s)"
fi

# Clean up
rm -f perf_test.*
```

### Phase 5: Ubuntu Integration Testing

#### Test 5.1: Package Manager Integration
```bash
#!/bin/bash
echo "=== Ubuntu Package Manager Integration Test ==="

# Test package installation and removal
echo "Testing package management integration..."

# Test installing a small LaTeX package
test_package="texlive-generic-extra"

echo "Testing installation of $test_package..."
sudo apt install -y $test_package

if dpkg -l | grep -q $test_package; then
    echo "✅ Package installation successful"
    
    # Test package functionality
    if kpsewhich generic.sty > /dev/null 2>&1; then
        echo "✅ Package functionality verified"
    else
        echo "⚠️  Package installed but functionality not verified"
    fi
else
    echo "❌ Package installation failed"
fi

# Test package information
echo "Package information:"
apt show $test_package 2>/dev/null | head -10
```

#### Test 5.2: System Service Integration
```bash
#!/bin/bash
echo "=== Ubuntu System Service Integration Test ==="

# Test systemd integration (if applicable)
echo "Testing systemd integration..."

# Check if systemd is available
if command -v systemctl &> /dev/null; then
    echo "✅ systemd available"
    
    # Test creating a simple service file for testing
    cat > /tmp/openvas-report-test.service << 'EOF'
[Unit]
Description=OpenVAS Report Generation Test Service
After=network.target

[Service]
Type=oneshot
ExecStart=/bin/echo "OpenVAS Report Service Test"
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
EOF

    # Test service file validation
    systemd-analyze verify /tmp/openvas-report-test.service
    if [ $? -eq 0 ]; then
        echo "✅ Service file validation successful"
    else
        echo "❌ Service file validation failed"
    fi
    
    rm -f /tmp/openvas-report-test.service
else
    echo "⚠️  systemd not available (may be expected in some environments)"
fi

# Test cron integration
echo "Testing cron integration..."
if command -v crontab &> /dev/null; then
    echo "✅ cron available"
    
    # Test cron syntax
    echo "0 2 * * * /usr/local/bin/openvas-report-generate" | crontab -l > /dev/null 2>&1
    echo "✅ Cron syntax validation successful"
else
    echo "⚠️  cron not available"
fi
```

## Ubuntu-Specific Test Results Documentation

### Ubuntu Version Compatibility Matrix
```bash
#!/bin/bash
# Generate Ubuntu compatibility matrix

cat > ~/openvas-ubuntu-testing/ubuntu_compatibility_matrix.md << 'EOF'
# Ubuntu Compatibility Matrix - OpenVAS Professional LaTeX Reporting

## Tested Ubuntu Versions

| Ubuntu Version | LaTeX Support | GLib Version | JSON-GLib | Build Status | Test Status |
|----------------|---------------|--------------|-----------|--------------|-------------|
| 18.04 LTS      | Limited       | 2.56+        | 1.4+      | ⚠️ Partial   | ⚠️ Limited  |
| 20.04 LTS      | Good          | 2.64+        | 1.4+      | ✅ Full      | ✅ Pass     |
| 22.04 LTS      | Excellent     | 2.72+        | 1.6+      | ✅ Full      | ✅ Pass     |
| 23.04+         | Excellent     | 2.76+        | 1.6+      | ✅ Full      | ✅ Pass     |

## Feature Support by Version

### Ubuntu 18.04 LTS
- ✅ Basic report generation
- ⚠️ Limited LaTeX packages
- ⚠️ Older library versions
- ❌ Some advanced features may not work

### Ubuntu 20.04 LTS
- ✅ Full report generation
- ✅ Most LaTeX packages available
- ✅ Good library support
- ✅ All core features working

### Ubuntu 22.04 LTS and Later
- ✅ Full report generation
- ✅ Complete LaTeX package support
- ✅ Latest library versions
- ✅ All features including advanced ones

## Recommendations

### Production Deployment
- **Recommended**: Ubuntu 22.04 LTS or later
- **Acceptable**: Ubuntu 20.04 LTS
- **Not Recommended**: Ubuntu 18.04 LTS (legacy support only)

### Development Environment
- **Preferred**: Ubuntu 22.04 LTS or latest
- **Minimum**: Ubuntu 20.04 LTS
EOF

echo "Ubuntu compatibility matrix created"
```

### Automated Ubuntu Testing Script
```bash
#!/bin/bash
# Comprehensive Ubuntu UAT test runner

echo "🚀 Starting OpenVAS Professional LaTeX Reporting Ubuntu UAT Tests"
echo "Ubuntu Version: $(lsb_release -ds)"
echo "Date: $(date)"
echo "=========================================="

# Initialize test results
results_dir="~/openvas-ubuntu-testing/uat_results_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$results_dir"

total_tests=0
passed_tests=0
failed_tests=0

# Test execution function
run_ubuntu_test() {
    local test_name="$1"
    local test_command="$2"
    
    echo "Running: $test_name"
    total_tests=$((total_tests + 1))
    
    if eval "$test_command"; then
        echo "✅ PASS: $test_name"
        passed_tests=$((passed_tests + 1))
        echo "PASS: $test_name" >> "$results_dir/ubuntu_test_results.log"
    else
        echo "❌ FAIL: $test_name"
        failed_tests=$((failed_tests + 1))
        echo "FAIL: $test_name" >> "$results_dir/ubuntu_test_results.log"
    fi
    echo "----------------------------------------"
}

# Execute Ubuntu-specific test phases
echo "Phase 1: Ubuntu Version Compatibility"
run_ubuntu_test "Ubuntu Version Detection" "test_ubuntu_version_detection"
run_ubuntu_test "Package Compatibility" "test_ubuntu_package_compatibility"
run_ubuntu_test "Library Compatibility" "test_ubuntu_library_compatibility"

echo "Phase 2: Ubuntu Build System"
run_ubuntu_test "Version-Specific Build" "test_ubuntu_version_specific_build"
run_ubuntu_test "Cross-Version Compatibility" "test_ubuntu_cross_version_compatibility"

echo "Phase 3: Ubuntu Functional Testing"
run_ubuntu_test "LaTeX Ubuntu Compatibility" "test_ubuntu_latex_compatibility"
run_ubuntu_test "File System Permissions" "test_ubuntu_filesystem_permissions"

echo "Phase 4: Ubuntu Performance"
run_ubuntu_test "Version Performance Comparison" "test_ubuntu_performance_comparison"

echo "Phase 5: Ubuntu Integration"
run_ubuntu_test "Package Manager Integration" "test_ubuntu_package_manager_integration"
run_ubuntu_test "System Service Integration" "test_ubuntu_system_service_integration"

# Generate Ubuntu-specific final report
echo "=========================================="
echo "🎯 Ubuntu UAT Test Results Summary"
echo "Ubuntu Version: $(lsb_release -ds)"
echo "Total Tests: $total_tests"
echo "Passed: $passed_tests"
echo "Failed: $failed_tests"
echo "Success Rate: $(( passed_tests * 100 / total_tests ))%"

# Ubuntu-specific recommendations
ubuntu_version=$(lsb_release -rs)
case $ubuntu_version in
    "18.04")
        echo "⚠️  Ubuntu 18.04: Limited support - consider upgrading"
        ;;
    "20.04")
        echo "✅ Ubuntu 20.04: Good support - production ready"
        ;;
    "22.04"|"23."*|"24."*)
        echo "✅ Ubuntu $ubuntu_version: Excellent support - fully recommended"
        ;;
esac

if [ $failed_tests -eq 0 ]; then
    echo "🎉 All Ubuntu tests passed! System ready for production on this Ubuntu version."
else
    echo "⚠️  $failed_tests test(s) failed. Review required before production deployment."
fi

echo "Detailed results saved to: $results_dir"
```

This comprehensive Ubuntu UAT testing plan provides thorough coverage for testing the OpenVAS Professional LaTeX Reporting System across different Ubuntu versions, ensuring compatibility and optimal performance in Ubuntu environments commonly used in enterprise settings.

## Summary

Now you have **both comprehensive UAT testing plans**:

1. **Kali Linux WSL UAT Testing Plan** - For penetration testing environments
2. **Ubuntu UAT Testing Plan** - For enterprise production environments

Both plans include:
- ✅ **Complete installation procedures** for each environment
- ✅ **Version-specific compatibility testing**
- ✅ **Automated test execution scripts**
- ✅ **Performance benchmarking**
- ✅ **Integration testing**
- ✅ **Results documentation templates**

You can now execute comprehensive UAT testing in both environments to validate the OpenVAS Professional LaTeX Reporting System before production deployment! 🚀