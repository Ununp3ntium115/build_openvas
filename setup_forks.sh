#!/bin/bash
#
# Setup script to fork and clone all OpenVAS/Greenbone repositories
#

set -e

echo "Setting up AI-Enhanced OpenVAS project structure..."

# Create core directory structure
mkdir -p core
mkdir -p ai-engine/{providers,agents,models,api}
mkdir -p plugins/{vulnerability-analysis,threat-modeling,report-generation,scan-optimization}
mkdir -p deployment/{docker,kubernetes,scripts}
mkdir -p docs
mkdir -p tests
mkdir -p tools

# Greenbone Community repositories to fork
declare -A REPOS=(
    ["gvm-libs"]="https://github.com/greenbone/gvm-libs.git"
    ["gvmd"]="https://github.com/greenbone/gvmd.git"
    ["gsa"]="https://github.com/greenbone/gsa.git"
    ["openvas-scanner"]="https://github.com/greenbone/openvas-scanner.git"
    ["ospd-openvas"]="https://github.com/greenbone/ospd-openvas.git"
    ["pg-gvm"]="https://github.com/greenbone/pg-gvm.git"
)

echo "Cloning Greenbone repositories into core/ directory..."

cd core

for repo in "${!REPOS[@]}"; do
    if [ ! -d "$repo" ]; then
        echo "Cloning $repo..."
        git clone "${REPOS[$repo]}" "$repo"
        cd "$repo"
        # Create a development branch for our modifications
        git checkout -b ai-enhanced-dev
        cd ..
    else
        echo "$repo already exists, skipping..."
    fi
done

cd ..

echo "Repository structure created successfully!"
echo "Next steps:"
echo "1. Fork these repositories on GitHub to your account"
echo "2. Update remote origins to point to your forks"
echo "3. Begin Phase 1 development"