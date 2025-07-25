# SPDX-FileCopyrightText: 2025 AI-Enhanced OpenVAS Project
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Windows PowerShell dependency installation script for AI-Enhanced OpenVAS

param(
    [switch]$UseVcpkg,
    [switch]$UseChocolatey,
    [switch]$UseScoop,
    [string]$VcpkgRoot = "C:\vcpkg"
)

# Colors for output
function Write-Info { param($Message) Write-Host "[INFO] $Message" -ForegroundColor Blue }
function Write-Success { param($Message) Write-Host "[SUCCESS] $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "[WARNING] $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "[ERROR] $Message" -ForegroundColor Red }

# Check if running as administrator
function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

# Install using vcpkg
function Install-VcpkgDependencies {
    Write-Info "Installing dependencies using vcpkg..."
    
    # Check if vcpkg is installed
    if (!(Test-Path $VcpkgRoot)) {
        Write-Info "vcpkg not found at $VcpkgRoot. Installing vcpkg..."
        
        git clone https://github.com/Microsoft/vcpkg.git $VcpkgRoot
        Set-Location $VcpkgRoot
        .\bootstrap-vcpkg.bat
        .\vcpkg integrate install
        Set-Location $PSScriptRoot
    }
    
    # Install required packages
    $packages = @(
        "glib",
        "json-glib", 
        "curl",
        "openssl",
        "libxml2",
        "sqlite3",
        "postgresql"
    )
    
    foreach ($package in $packages) {
        Write-Info "Installing $package..."
        & "$VcpkgRoot\vcpkg.exe" install "$package`:x64-windows"
    }
    
    Write-Success "vcpkg dependencies installed"
}

# Install using Chocolatey
function Install-ChocolateyDependencies {
    Write-Info "Installing dependencies using Chocolatey..."
    
    # Check if Chocolatey is installed
    if (!(Get-Command choco -ErrorAction SilentlyContinue)) {
        Write-Info "Chocolatey not found. Installing Chocolatey..."
        Set-ExecutionPolicy Bypass -Scope Process -Force
        [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
        iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
    }
    
    # Install packages
    $packages = @(
        "cmake",
        "git",
        "curl",
        "nodejs",
        "python3",
        "postgresql",
        "visualstudio2022buildtools",
        "visualstudio2022-workload-vctools"
    )
    
    foreach ($package in $packages) {
        Write-Info "Installing $package..."
        choco install $package -y
    }
    
    Write-Success "Chocolatey dependencies installed"
}

# Install using Scoop
function Install-ScoopDependencies {
    Write-Info "Installing dependencies using Scoop..."
    
    # Check if Scoop is installed
    if (!(Get-Command scoop -ErrorAction SilentlyContinue)) {
        Write-Info "Scoop not found. Installing Scoop..."
        Set-ExecutionPolicy RemoteSigned -Scope CurrentUser
        irm get.scoop.sh | iex
    }
    
    # Add buckets
    scoop bucket add extras
    scoop bucket add versions
    
    # Install packages
    $packages = @(
        "cmake",
        "git",
        "curl",
        "nodejs",
        "python",
        "postgresql"
    )
    
    foreach ($package in $packages) {
        Write-Info "Installing $package..."
        scoop install $package
    }
    
    Write-Success "Scoop dependencies installed"
}

# Install Python dependencies
function Install-PythonDependencies {
    Write-Info "Installing Python dependencies..."
    
    if (Get-Command python -ErrorAction SilentlyContinue) {
        python -m pip install --upgrade pip
        python -m pip install --user `
            python-gvm `
            ospd `
            ospd-openvas `
            psutil `
            redis `
            paramiko `
            lxml `
            defusedxml `
            packaging `
            deprecated `
            pontos
        
        Write-Success "Python dependencies installed"
    } else {
        Write-Warning "Python not found. Please install Python first."
    }
}

# Install Node.js dependencies
function Install-NodejsDependencies {
    Write-Info "Installing Node.js dependencies..."
    
    if (Get-Command npm -ErrorAction SilentlyContinue) {
        if (Test-Path "core\gsa\package.json") {
            Set-Location "core\gsa"
            npm install
            Set-Location $PSScriptRoot
            Write-Success "GSA Node.js dependencies installed"
        } else {
            Write-Warning "GSA package.json not found, skipping Node.js dependencies"
        }
    } else {
        Write-Warning "npm not found. Please install Node.js first."
    }
}

# Check Visual Studio installation
function Test-VisualStudio {
    $vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    
    if (Test-Path $vsWhere) {
        $installations = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64
        if ($installations) {
            Write-Success "Visual Studio with C++ tools found"
            return $true
        }
    }
    
    Write-Warning "Visual Studio with C++ tools not found"
    Write-Info "Please install Visual Studio 2019 or later with C++ development tools"
    return $false
}

# Main installation function
function Main {
    Write-Info "Starting AI-Enhanced OpenVAS dependency installation for Windows..."
    
    # Check administrator privileges for some operations
    if (!(Test-Administrator)) {
        Write-Warning "Not running as administrator. Some operations may fail."
        Write-Info "Consider running PowerShell as administrator for full functionality."
    }
    
    # Check Visual Studio
    Test-VisualStudio
    
    # Install dependencies based on user preference
    if ($UseVcpkg) {
        Install-VcpkgDependencies
    } elseif ($UseChocolatey) {
        Install-ChocolateyDependencies
    } elseif ($UseScoop) {
        Install-ScoopDependencies
    } else {
        Write-Info "No package manager specified. Detecting available options..."
        
        if (Get-Command choco -ErrorAction SilentlyContinue) {
            Write-Info "Chocolatey detected. Using Chocolatey..."
            Install-ChocolateyDependencies
        } elseif (Get-Command scoop -ErrorAction SilentlyContinue) {
            Write-Info "Scoop detected. Using Scoop..."
            Install-ScoopDependencies
        } elseif (Test-Path $VcpkgRoot) {
            Write-Info "vcpkg detected. Using vcpkg..."
            Install-VcpkgDependencies
        } else {
            Write-Warning "No package manager found. Please install one of:"
            Write-Info "- Chocolatey: https://chocolatey.org/install"
            Write-Info "- Scoop: https://scoop.sh/"
            Write-Info "- vcpkg: https://github.com/Microsoft/vcpkg"
            Write-Info "Then run this script with the appropriate flag:"
            Write-Info "  -UseChocolatey, -UseScoop, or -UseVcpkg"
            exit 1
        }
    }
    
    # Install Python and Node.js dependencies
    Install-PythonDependencies
    Install-NodejsDependencies
    
    Write-Success "Windows dependencies installation completed!"
    Write-Info "Next steps:"
    Write-Info "1. Ensure all required libraries are in your PATH"
    Write-Info "2. Set up CMake to find the installed libraries"
    Write-Info "3. Run: .\build.ps1"
    
    # Create environment setup script
    $envScript = @"
# Environment setup for AI-Enhanced OpenVAS development
# Source this script or add to your PowerShell profile

# Add vcpkg to PATH if using vcpkg
if (Test-Path "$VcpkgRoot") {
    `$env:PATH += ";$VcpkgRoot"
    `$env:CMAKE_TOOLCHAIN_FILE = "$VcpkgRoot\scripts\buildsystems\vcpkg.cmake"
}

# Add common development paths
`$env:PATH += ";C:\Program Files\CMake\bin"
`$env:PATH += ";C:\Program Files\Git\bin"

Write-Host "AI-Enhanced OpenVAS development environment configured" -ForegroundColor Green
"@
    
    $envScript | Out-File -FilePath "setup_env.ps1" -Encoding UTF8
    Write-Info "Environment setup script created: setup_env.ps1"
}

# Show help
function Show-Help {
    Write-Host "AI-Enhanced OpenVAS Windows Dependency Installer" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Usage: .\install_dependencies.ps1 [OPTIONS]" -ForegroundColor White
    Write-Host ""
    Write-Host "Options:" -ForegroundColor Yellow
    Write-Host "  -UseVcpkg         Use vcpkg package manager"
    Write-Host "  -UseChocolatey    Use Chocolatey package manager"
    Write-Host "  -UseScoop         Use Scoop package manager"
    Write-Host "  -VcpkgRoot <path> Specify vcpkg installation path (default: C:\vcpkg)"
    Write-Host "  -Help             Show this help message"
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor Green
    Write-Host "  .\install_dependencies.ps1 -UseChocolatey"
    Write-Host "  .\install_dependencies.ps1 -UseVcpkg -VcpkgRoot D:\vcpkg"
}

# Handle help parameter
if ($args -contains "-Help" -or $args -contains "--help" -or $args -contains "-h") {
    Show-Help
    exit 0
}

# Run main function
Main