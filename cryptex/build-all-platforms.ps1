# Cross-platform build script for CRYPTEX (PowerShell)
# Builds binaries for Linux, macOS, and Windows

$ErrorActionPreference = "Stop"

Write-Host "🔨 CRYPTEX Cross-Platform Build Script" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

# Get version from Cargo.toml
$version = (Get-Content Cargo.toml | Select-String '^version\s*=' | Select-Object -First 1) -replace '.*"(.*)".*', '$1'
Write-Host "Version: $version"
Write-Host ""

# Create dist directory
New-Item -ItemType Directory -Force -Path "dist/linux" | Out-Null
New-Item -ItemType Directory -Force -Path "dist/macos" | Out-Null
New-Item -ItemType Directory -Force -Path "dist/windows" | Out-Null

# Targets to build
$targets = @(
    "x86_64-unknown-linux-gnu",     # Linux x86_64
    "x86_64-apple-darwin",          # macOS x86_64
    "aarch64-apple-darwin",         # macOS ARM64 (M1/M2)
    "x86_64-pc-windows-gnu",        # Windows x86_64
    "x86_64-pc-windows-msvc"        # Windows x86_64 (MSVC)
)

# Binaries to build
$binaries = @(
    "the_commune",          # MCP server
    "the_interface_server"  # REST API server
)

# Check if cross is installed
if (-not (Get-Command cross -ErrorAction SilentlyContinue)) {
    Write-Host "⚠️  'cross' not found. Installing..." -ForegroundColor Yellow
    cargo install cross
}

# Function to build for a specific target
function Build-Target {
    param(
        [string]$Target,
        [string]$Binary
    )

    Write-Host "Building $Binary for $Target..." -ForegroundColor Yellow

    $result = cargo build --release --target $Target --bin $Binary 2>&1

    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Successfully built $Binary for $Target" -ForegroundColor Green

        # Determine platform directory
        $platformDir = ""
        if ($Target -like "*linux*") {
            $platformDir = "linux"
        } elseif ($Target -like "*darwin*") {
            $platformDir = "macos"
        } elseif ($Target -like "*windows*") {
            $platformDir = "windows"
        }

        # Copy binary to dist directory
        $exeName = $Binary
        if ($Target -like "*windows*") {
            $exeName = "$Binary.exe"
        }

        $sourcePath = "target/$Target/release/$exeName"
        $destPath = "dist/$platformDir/$exeName"

        if (Test-Path $sourcePath) {
            Copy-Item $sourcePath $destPath -Force
            Write-Host "✓ Copied to $destPath" -ForegroundColor Green
        }

        return $true
    } else {
        Write-Host "✗ Failed to build $Binary for $Target" -ForegroundColor Red
        Write-Host $result
        return $false
    }
}

# Build all combinations
Write-Host "Building binaries for all platforms..."
Write-Host ""

foreach ($binary in $binaries) {
    foreach ($target in $targets) {
        try {
            Build-Target -Target $target -Binary $binary | Out-Null
        } catch {
            Write-Host "Skipping $binary for $target" -ForegroundColor Red
        }
        Write-Host ""
    }
}

# Create release archives
Write-Host "Creating release archives..." -ForegroundColor Yellow

Set-Location dist

foreach ($platform in @("linux", "macos", "windows")) {
    if (Test-Path $platform) {
        $items = Get-ChildItem $platform
        if ($items.Count -gt 0) {
            $archiveName = "cryptex-$version-$platform"

            if ($platform -eq "windows") {
                Compress-Archive -Path "$platform/*" -DestinationPath "$archiveName.zip" -Force
                Write-Host "✓ Created $archiveName.zip" -ForegroundColor Green
            } else {
                # Use tar if available
                if (Get-Command tar -ErrorAction SilentlyContinue) {
                    tar czf "$archiveName.tar.gz" $platform/
                    Write-Host "✓ Created $archiveName.tar.gz" -ForegroundColor Green
                } else {
                    Compress-Archive -Path "$platform/*" -DestinationPath "$archiveName.zip" -Force
                    Write-Host "✓ Created $archiveName.zip" -ForegroundColor Green
                }
            }
        }
    }
}

Set-Location ..

Write-Host ""
Write-Host "═══════════════════════════════════" -ForegroundColor Green
Write-Host "Build complete!" -ForegroundColor Green
Write-Host "═══════════════════════════════════" -ForegroundColor Green
Write-Host ""
Write-Host "Distribution files are in the dist/ directory:"
Get-ChildItem dist -File | Format-Table Name, Length, LastWriteTime

Write-Host ""
Write-Host "Next steps:"
Write-Host "  1. Test the binaries on each platform"
Write-Host "  2. Create a GitHub release with the archives"
Write-Host "  3. Update installation documentation"
