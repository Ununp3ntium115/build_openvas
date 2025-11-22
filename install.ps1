# CRYPTEX Installation Script for Windows (PowerShell)
# Installs CRYPTEX binaries and sets up environment

param(
    [string]$InstallDir = "$env:USERPROFILE\.cryptex"
)

$ErrorActionPreference = "Stop"

Write-Host "╔════════════════════════════════════════╗" -ForegroundColor Blue
Write-Host "║      CRYPTEX Installation Script       ║" -ForegroundColor Blue
Write-Host "╚════════════════════════════════════════╝" -ForegroundColor Blue
Write-Host ""

# Installation directories
$BinDir = Join-Path $InstallDir "bin"
$DataDir = Join-Path $InstallDir "data"
$ConfigDir = Join-Path $InstallDir "config"

Write-Host "Platform: Windows" -ForegroundColor Yellow
Write-Host "Architecture: $env:PROCESSOR_ARCHITECTURE" -ForegroundColor Yellow
Write-Host "Install directory: $InstallDir" -ForegroundColor Yellow
Write-Host ""

# Create directories
Write-Host "Creating directories..." -ForegroundColor Yellow
New-Item -ItemType Directory -Force -Path $BinDir | Out-Null
New-Item -ItemType Directory -Force -Path $DataDir | Out-Null
New-Item -ItemType Directory -Force -Path $ConfigDir | Out-Null

# Determine binary source
$BinarySource = $null
if (Test-Path "cryptex\target\release") {
    Write-Host "✓ Found compiled binaries" -ForegroundColor Green
    $BinarySource = "cryptex\target\release"
} elseif (Test-Path "dist\windows") {
    Write-Host "✓ Found distribution binaries" -ForegroundColor Green
    $BinarySource = "dist\windows"
} else {
    Write-Host "✗ No binaries found" -ForegroundColor Red
    Write-Host "Building from source..." -ForegroundColor Yellow

    if (-not (Get-Command cargo -ErrorAction SilentlyContinue)) {
        Write-Host "Error: Rust/Cargo not installed" -ForegroundColor Red
        Write-Host "Install Rust from https://rustup.rs/"
        exit 1
    }

    Set-Location cryptex
    cargo build --release
    Set-Location ..
    $BinarySource = "cryptex\target\release"
}

# Copy binaries
Write-Host "Installing binaries..." -ForegroundColor Yellow
$binaries = @("the_commune.exe", "the_interface_server.exe")

foreach ($binary in $binaries) {
    $sourcePath = Join-Path $BinarySource $binary
    if (Test-Path $sourcePath) {
        Copy-Item $sourcePath $BinDir -Force
        Write-Host "✓ Installed $binary" -ForegroundColor Green
    } else {
        Write-Host "⚠ Skipping $binary (not found)" -ForegroundColor Yellow
    }
}

# Install Svelte visualizer (if npm is available)
if (Get-Command npm -ErrorAction SilentlyContinue) {
    Write-Host "Installing Svelte visualizer..." -ForegroundColor Yellow
    if (Test-Path "cryptex-visualizer") {
        Set-Location cryptex-visualizer
        npm install
        npm run build
        Copy-Item -Path dist -Destination (Join-Path $InstallDir "visualizer") -Recurse -Force
        Set-Location ..
        Write-Host "✓ Installed visualizer" -ForegroundColor Green
    }
}

# Install Node-RED nodes (if npm is available)
if (Get-Command npm -ErrorAction SilentlyContinue) {
    Write-Host "Installing Node-RED nodes..." -ForegroundColor Yellow
    $nodeRedDir = Join-Path $env:USERPROFILE ".node-red"
    if ((Test-Path "node-red-cryptex") -and (Test-Path $nodeRedDir)) {
        Set-Location $nodeRedDir
        npm install (Join-Path (Get-Location).Path "..\node-red-cryptex")
        Write-Host "✓ Installed Node-RED nodes" -ForegroundColor Green
        Set-Location $PSScriptRoot
    }
}

# Setup PATH
Write-Host "Setting up PATH..." -ForegroundColor Yellow

$currentPath = [Environment]::GetEnvironmentVariable("Path", "User")
if ($currentPath -notlike "*$BinDir*") {
    [Environment]::SetEnvironmentVariable(
        "Path",
        "$currentPath;$BinDir",
        "User"
    )
    Write-Host "✓ Added to PATH" -ForegroundColor Green
    Write-Host "  (You may need to restart your terminal)" -ForegroundColor Yellow
} else {
    Write-Host "⚠ Already in PATH" -ForegroundColor Yellow
}

# Set environment variables
[Environment]::SetEnvironmentVariable("CRYPTEX_DATA_DIR", $DataDir, "User")
[Environment]::SetEnvironmentVariable("CRYPTEX_CONFIG_DIR", $ConfigDir, "User")

# Create launcher scripts
Write-Host "Creating launcher scripts..." -ForegroundColor Yellow

@"
@echo off
REM CRYPTEX REST API Launcher
"%~dp0the_interface_server.exe" %*
"@ | Out-File -FilePath (Join-Path $BinDir "cryptex-api.bat") -Encoding ASCII

@"
@echo off
REM CRYPTEX MCP Server Launcher
"%~dp0the_commune.exe" %*
"@ | Out-File -FilePath (Join-Path $BinDir "cryptex-mcp.bat") -Encoding ASCII

Write-Host "✓ Created launcher scripts" -ForegroundColor Green

# Create PowerShell launchers
@"
# CRYPTEX REST API Launcher (PowerShell)
& "`$PSScriptRoot\the_interface_server.exe" `$args
"@ | Out-File -FilePath (Join-Path $BinDir "cryptex-api.ps1") -Encoding UTF8

@"
# CRYPTEX MCP Server Launcher (PowerShell)
& "`$PSScriptRoot\the_commune.exe" `$args
"@ | Out-File -FilePath (Join-Path $BinDir "cryptex-mcp.ps1") -Encoding UTF8

# Create config file
$configPath = Join-Path $ConfigDir "config.toml"
if (-not (Test-Path $configPath)) {
    @"
[cryptex]
# CRYPTEX Configuration

# Data directory
data_dir = "$($DataDir -replace '\\', '\\')"

# Database path
db_path = "$($DataDir -replace '\\', '\\')\\cryptex.db"

# API server settings
[api]
bind_addr = "127.0.0.1:8080"
log_level = "info"

# MCP server settings
[mcp]
log_level = "info"
"@ | Out-File -FilePath $configPath -Encoding UTF8
    Write-Host "✓ Created config file" -ForegroundColor Green
}

# Print summary
Write-Host ""
Write-Host "╔════════════════════════════════════════╗" -ForegroundColor Green
Write-Host "║     Installation Complete! 🎉          ║" -ForegroundColor Green
Write-Host "╚════════════════════════════════════════╝" -ForegroundColor Green
Write-Host ""
Write-Host "Installed to: $InstallDir" -ForegroundColor Yellow
Write-Host ""
Write-Host "Available commands:" -ForegroundColor Yellow
Write-Host "  cryptex-api        Start REST API server" -ForegroundColor Blue
Write-Host "  cryptex-mcp        Start MCP server" -ForegroundColor Blue
Write-Host ""
Write-Host "To use these commands:" -ForegroundColor Yellow
Write-Host "  1. Restart your terminal (PATH was updated)" -ForegroundColor White
Write-Host "  2. Or use full path: $BinDir\cryptex-api.bat" -ForegroundColor White
Write-Host ""
Write-Host "Quick start:" -ForegroundColor Yellow
Write-Host "  cryptex-api" -ForegroundColor Blue
Write-Host "  Invoke-WebRequest http://localhost:8080/health" -ForegroundColor Blue
Write-Host ""
Write-Host "Documentation:" -ForegroundColor Yellow
Write-Host "  README: $(Get-Location)\README.md"
Write-Host "  API Reference: $(Get-Location)\API_REFERENCE.md"
Write-Host "  Deployment Guide: $(Get-Location)\DEPLOYMENT.md"
Write-Host ""
