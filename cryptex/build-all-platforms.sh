#!/bin/bash
# Cross-platform build script for CRYPTEX
# Builds binaries for Linux, macOS, and Windows

set -e

echo "🔨 CRYPTEX Cross-Platform Build Script"
echo "========================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Get version from Cargo.toml
VERSION=$(grep '^version' Cargo.toml | head -1 | cut -d'"' -f2)
echo "Version: $VERSION"
echo ""

# Create dist directory
mkdir -p dist/{linux,macos,windows}

# Targets to build
TARGETS=(
    "x86_64-unknown-linux-gnu"      # Linux x86_64
    "x86_64-apple-darwin"           # macOS x86_64
    "aarch64-apple-darwin"          # macOS ARM64 (M1/M2)
    "x86_64-pc-windows-gnu"         # Windows x86_64
)

# Binaries to build
BINARIES=(
    "the_commune"           # MCP server
    "the_interface_server"  # REST API server
)

# Check if cross is installed
if ! command -v cross &> /dev/null; then
    echo -e "${YELLOW}⚠️  'cross' not found. Installing...${NC}"
    cargo install cross
fi

# Function to build for a specific target
build_target() {
    local target=$1
    local binary=$2

    echo -e "${YELLOW}Building $binary for $target...${NC}"

    if cross build --release --target $target --bin $binary; then
        echo -e "${GREEN}✓ Successfully built $binary for $target${NC}"

        # Determine platform directory
        local platform_dir=""
        case $target in
            *linux*)
                platform_dir="linux"
                ;;
            *darwin*)
                platform_dir="macos"
                ;;
            *windows*)
                platform_dir="windows"
                ;;
        esac

        # Copy binary to dist directory
        local exe_name=$binary
        if [[ $target == *"windows"* ]]; then
            exe_name="${binary}.exe"
        fi

        local source_path="target/$target/release/$exe_name"
        local dest_path="dist/$platform_dir/$exe_name"

        if [ -f "$source_path" ]; then
            cp "$source_path" "$dest_path"
            echo -e "${GREEN}✓ Copied to $dest_path${NC}"
        fi

        return 0
    else
        echo -e "${RED}✗ Failed to build $binary for $target${NC}"
        return 1
    fi
}

# Build all combinations
echo "Building binaries for all platforms..."
echo ""

for binary in "${BINARIES[@]}"; do
    for target in "${TARGETS[@]}"; do
        build_target "$target" "$binary" || echo -e "${RED}Skipping $binary for $target${NC}"
        echo ""
    done
done

# Create release archives
echo -e "${YELLOW}Creating release archives...${NC}"

cd dist

for platform in linux macos windows; do
    if [ -d "$platform" ] && [ "$(ls -A $platform)" ]; then
        archive_name="cryptex-$VERSION-$platform"

        case $platform in
            windows)
                zip -r "$archive_name.zip" $platform/
                echo -e "${GREEN}✓ Created $archive_name.zip${NC}"
                ;;
            *)
                tar czf "$archive_name.tar.gz" $platform/
                echo -e "${GREEN}✓ Created $archive_name.tar.gz${NC}"
                ;;
        esac
    fi
done

cd ..

echo ""
echo -e "${GREEN}════════════════════════════════════${NC}"
echo -e "${GREEN}Build complete!${NC}"
echo -e "${GREEN}════════════════════════════════════${NC}"
echo ""
echo "Distribution files are in the dist/ directory:"
ls -lh dist/

# Generate checksums
echo ""
echo -e "${YELLOW}Generating checksums...${NC}"
cd dist
sha256sum *.tar.gz *.zip 2>/dev/null > SHA256SUMS || shasum -a 256 *.tar.gz *.zip > SHA256SUMS
echo -e "${GREEN}✓ Checksums saved to dist/SHA256SUMS${NC}"
cd ..

echo ""
echo "Next steps:"
echo "  1. Test the binaries on each platform"
echo "  2. Create a GitHub release with the archives"
echo "  3. Update installation documentation"
