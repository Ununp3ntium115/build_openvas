# PowerShell script to set up AI-Enhanced OpenVAS project structure

Write-Host "Setting up AI-Enhanced OpenVAS project structure..." -ForegroundColor Green

# Create core directory structure
$directories = @(
    "core",
    "ai-engine/providers",
    "ai-engine/agents", 
    "ai-engine/models",
    "ai-engine/api",
    "plugins/vulnerability-analysis",
    "plugins/threat-modeling",
    "plugins/report-generation", 
    "plugins/scan-optimization",
    "deployment/docker",
    "deployment/kubernetes",
    "deployment/scripts",
    "docs",
    "tests",
    "tools"
)

foreach ($dir in $directories) {
    if (!(Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "Created directory: $dir" -ForegroundColor Yellow
    }
}

# Greenbone Community repositories to clone
$repos = @{
    "gvm-libs" = "https://github.com/greenbone/gvm-libs.git"
    "gvmd" = "https://github.com/greenbone/gvmd.git"
    "gsa" = "https://github.com/greenbone/gsa.git"
    "openvas-scanner" = "https://github.com/greenbone/openvas-scanner.git"
    "ospd-openvas" = "https://github.com/greenbone/ospd-openvas.git"
    "pg-gvm" = "https://github.com/greenbone/pg-gvm.git"
}

Write-Host "Cloning Greenbone repositories into core/ directory..." -ForegroundColor Green

Set-Location core

foreach ($repo in $repos.Keys) {
    if (!(Test-Path $repo)) {
        Write-Host "Cloning $repo..." -ForegroundColor Cyan
        git clone $repos[$repo] $repo
        Set-Location $repo
        # Create a development branch for our modifications
        git checkout -b ai-enhanced-dev
        Set-Location ..
    } else {
        Write-Host "$repo already exists, skipping..." -ForegroundColor Yellow
    }
}

Set-Location ..

Write-Host "Repository structure created successfully!" -ForegroundColor Green
Write-Host "Next steps:" -ForegroundColor Cyan
Write-Host "1. Fork these repositories on GitHub to your account"
Write-Host "2. Update remote origins to point to your forks"
Write-Host "3. Begin Phase 1 development"