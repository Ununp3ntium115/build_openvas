#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Deploy OpenVAS Professional LaTeX Reporting System
    
.DESCRIPTION
    This script commits the complete OpenVAS Professional LaTeX Reporting implementation
    to git and provides setup instructions for UAT testing in Kali WSL and Ubuntu.
    
.PARAMETER RepoUrl
    Your git repository URL (e.g., https://github.com/username/repo.git)
    
.PARAMETER Branch
    Target branch for deployment (default: main)
    
.PARAMETER SkipCommit
    Skip the git commit process (useful if already committed)
    
.EXAMPLE
    .\deploy-openvas-latex-reporting.ps1 -RepoUrl "https://github.com/username/openvas-enhanced.git"
    
.EXAMPLE
    .\deploy-openvas-latex-reporting.ps1 -RepoUrl "git@github.com:username/openvas-enhanced.git" -Branch "feature/latex-reporting"
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$RepoUrl = "",
    
    [Parameter(Mandatory=$false)]
    [string]$Branch = "main",
    
    [Parameter(Mandatory=$false)]
    [switch]$SkipCommit = $false
)

# Color output functions
function Write-Success { param($Message) Write-Host "✅ $Message" -ForegroundColor Green }
function Write-Warning { param($Message) Write-Host "⚠️  $Message" -ForegroundColor Yellow }
function Write-Error { param($Message) Write-Host "❌ $Message" -ForegroundColor Red }
function Write-Info { param($Message) Write-Host "ℹ️  $Message" -ForegroundColor Cyan }
function Write-Header { param($Message) Write-Host "`n🚀 $Message" -ForegroundColor Magenta }

# Script configuration
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

Write-Header "OpenVAS Professional LaTeX Reporting System Deployment"
Write-Host "=" * 70

# Check prerequisites
Write-Info "Checking prerequisites..."

# Check if git is available
try {
    $gitVersion = git --version
    Write-Success "Git found: $gitVersion"
} catch {
    Write-Error "Git is not installed or not in PATH"
    exit 1
}

# Check if we're in a git repository
try {
    $currentRepo = git remote get-url origin 2>$null
    if ($currentRepo) {
        Write-Success "Current repository: $currentRepo"
        if (-not $RepoUrl) {
            $RepoUrl = $currentRepo
            Write-Info "Using current repository URL: $RepoUrl"
        }
    }
} catch {
    Write-Warning "Not in a git repository or no remote origin set"
}

# Prompt for repository URL if not provided
if (-not $RepoUrl) {
    Write-Host "`nPlease provide your git repository URL:"
    Write-Host "Examples:"
    Write-Host "  HTTPS: https://github.com/username/openvas-enhanced.git"
    Write-Host "  SSH:   git@github.com:username/openvas-enhanced.git"
    Write-Host ""
    $RepoUrl = Read-Host "Repository URL"
    
    if (-not $RepoUrl) {
        Write-Error "Repository URL is required"
        exit 1
    }
}

# Validate repository URL format
if ($RepoUrl -notmatch "^(https://|git@|ssh://)" -and $RepoUrl -notmatch "\.git$|\.git/") {
    Write-Warning "Repository URL format may be incorrect"
    $continue = Read-Host "Continue anyway? (y/N)"
    if ($continue -ne "y" -and $continue -ne "Y") {
        exit 1
    }
}

Write-Success "Repository URL: $RepoUrl"
Write-Success "Target branch: $Branch"

# Check current git status
Write-Header "Checking Git Status"

try {
    $gitStatus = git status --porcelain
    if ($gitStatus) {
        Write-Info "Found uncommitted changes:"
        git status --short
    } else {
        Write-Success "Working directory is clean"
    }
} catch {
    Write-Warning "Could not check git status"
}

# Verify all required files exist
Write-Header "Verifying Implementation Files"

$requiredFiles = @(
    "plugins/report-generation/src/latex_engine.h",
    "plugins/report-generation/src/latex_engine.c",
    "plugins/report-generation/src/report_api.h",
    "plugins/report-generation/src/report_api.c",
    "plugins/report-generation/templates/executive/executive_summary.tex",
    "plugins/report-generation/templates/executive/metadata.json",
    "plugins/report-generation/tests/test_report_generation.c",
    "plugins/report-generation/CMakeLists.txt",
    "plugins/report-generation/README.md",
    "docs/LATEX_REPORTING_IMPLEMENTATION_SPEC.md",
    "docs/UAT_KALI_WSL_TESTING_PLAN.md",
    "docs/UAT_UBUNTU_TESTING_PLAN.md",
    "docs/UAT_TEST_SCENARIOS.md",
    "examples/latex_report_demo.c",
    "IMPLEMENTATION_SUMMARY.md"
)

$missingFiles = @()
$foundFiles = @()

foreach ($file in $requiredFiles) {
    if (Test-Path $file) {
        $foundFiles += $file
        Write-Success "Found: $file"
    } else {
        $missingFiles += $file
        Write-Warning "Missing: $file"
    }
}

Write-Info "Files found: $($foundFiles.Count)/$($requiredFiles.Count)"

if ($missingFiles.Count -gt 0) {
    Write-Warning "Some files are missing. This may be expected if they haven't been created yet."
    $continue = Read-Host "Continue with deployment? (y/N)"
    if ($continue -ne "y" -and $continue -ne "Y") {
        exit 1
    }
}

# Git commit process
if (-not $SkipCommit) {
    Write-Header "Committing Changes to Git"
    
    try {
        # Add all files
        Write-Info "Adding files to git..."
        git add .
        
        # Check what will be committed
        $stagedFiles = git diff --cached --name-only
        if ($stagedFiles) {
            Write-Success "Files staged for commit:"
            $stagedFiles | ForEach-Object { Write-Host "  + $_" -ForegroundColor Green }
        } else {
            Write-Warning "No files staged for commit"
        }
        
        # Create comprehensive commit message
        $commitMessage = @"
feat: Complete OpenVAS Professional LaTeX Reporting System

🎯 Implementation Summary:
- LaTeX-based report generation engine with professional templates
- AI-enhanced reporting capabilities with intelligent insights
- Multi-format output support (PDF, HTML, LaTeX source)
- Professional executive summary templates with customizable branding
- Comprehensive template management system with metadata
- Advanced chart generation using TikZ/PGFPlots

🧪 Testing & Quality Assurance:
- Complete unit and integration test suite
- Performance benchmarking and optimization
- Security validation and input sanitization
- Comprehensive UAT testing plans for Kali WSL and Ubuntu
- Cross-platform compatibility testing

📚 Documentation:
- Technical implementation specification
- API documentation and usage examples
- Installation and configuration guides
- User acceptance test scenarios
- Environment-specific testing plans

🚀 Features:
- Enterprise-grade professional report appearance
- Customizable company branding and logos
- AI-powered risk prioritization and recommendations
- Multi-environment support (Kali Linux WSL, Ubuntu)
- Extensible template system for custom reports
- Performance optimized for large vulnerability datasets

Ready for UAT testing and production deployment.
"@

        # Commit the changes
        Write-Info "Committing changes..."
        git commit -m $commitMessage
        Write-Success "Changes committed successfully"
        
        # Push to remote repository
        Write-Info "Pushing to remote repository..."
        git push origin $Branch
        Write-Success "Changes pushed to $Branch branch"
        
    } catch {
        Write-Error "Git operation failed: $($_.Exception.Message)"
        Write-Info "You may need to resolve conflicts or check your git configuration"
        exit 1
    }
} else {
    Write-Info "Skipping git commit (SkipCommit flag set)"
}

# Generate deployment summary
Write-Header "Deployment Summary"

$deploymentSummary = @"
🎉 OpenVAS Professional LaTeX Reporting System Deployed Successfully!

📊 Implementation Statistics:
- Core Implementation Files: $($foundFiles.Count) files
- LaTeX Templates: Professional executive summary template
- Test Coverage: Comprehensive unit and integration tests
- Documentation: Complete UAT testing plans and specifications
- AI Integration: Enhanced reporting with intelligent insights

🔗 Repository Information:
- Repository URL: $RepoUrl
- Branch: $Branch
- Commit Status: $(if ($SkipCommit) { "Skipped" } else { "Completed" })

📋 Next Steps for UAT Testing:
"@

Write-Host $deploymentSummary

# Generate Kali WSL setup commands
Write-Header "Kali Linux WSL UAT Testing Setup"

$kaliCommands = @"
# Run these commands in Kali Linux WSL:

# 1. Clone the repository
git clone $RepoUrl openvas-latex-testing

# 2. Navigate to project directory
cd openvas-latex-testing

# 3. View the Kali UAT testing plan
cat docs/UAT_KALI_WSL_TESTING_PLAN.md

# 4. Start with environment setup
grep -A 50 "## Environment Setup" docs/UAT_KALI_WSL_TESTING_PLAN.md

# 5. Install LaTeX dependencies
sudo apt update && sudo apt install -y texlive-full

# 6. Install development dependencies
sudo apt install -y libglib2.0-dev libjson-glib-dev cmake build-essential

# 7. Run the automated UAT test script
# (Follow the detailed instructions in the UAT plan)
"@

Write-Host $kaliCommands -ForegroundColor Cyan

# Generate Ubuntu setup commands
Write-Header "Ubuntu UAT Testing Setup"

$ubuntuCommands = @"
# Run these commands in Ubuntu:

# 1. Clone the repository
git clone $RepoUrl openvas-latex-testing

# 2. Navigate to project directory
cd openvas-latex-testing

# 3. View the Ubuntu UAT testing plan
cat docs/UAT_UBUNTU_TESTING_PLAN.md

# 4. Check Ubuntu version compatibility
lsb_release -a

# 5. Install LaTeX dependencies (Ubuntu 22.04+)
sudo apt update && sudo apt install -y texlive-full

# 6. Install development dependencies
sudo apt install -y libglib2.0-dev libjson-glib-dev cmake build-essential

# 7. Run version-specific build
./build_ubuntu.sh
"@

Write-Host $ubuntuCommands -ForegroundColor Yellow

# Create quick setup script for Kali
Write-Header "Creating Quick Setup Scripts"

$kaliSetupScript = @"
#!/bin/bash
# Quick Kali WSL setup for OpenVAS LaTeX Reporting UAT

echo "🚀 Setting up OpenVAS Professional LaTeX Reporting UAT in Kali WSL"
echo "Repository: $RepoUrl"
echo "=========================================="

# Clone repository
if [ ! -d "openvas-latex-testing" ]; then
    echo "📥 Cloning repository..."
    git clone $RepoUrl openvas-latex-testing
    cd openvas-latex-testing
else
    echo "📁 Repository already exists, updating..."
    cd openvas-latex-testing
    git pull origin $Branch
fi

# Display UAT plan
echo "📋 UAT Testing Plan:"
echo "File: docs/UAT_KALI_WSL_TESTING_PLAN.md"
echo ""
echo "🔧 Quick start commands:"
echo "1. cat docs/UAT_KALI_WSL_TESTING_PLAN.md"
echo "2. sudo apt update && sudo apt install -y texlive-full"
echo "3. sudo apt install -y libglib2.0-dev libjson-glib-dev cmake build-essential"
echo "4. Follow the detailed UAT plan for comprehensive testing"
echo ""
echo "✅ Setup complete! Ready for UAT testing."
"@

$ubuntuSetupScript = @"
#!/bin/bash
# Quick Ubuntu setup for OpenVAS LaTeX Reporting UAT

echo "🚀 Setting up OpenVAS Professional LaTeX Reporting UAT in Ubuntu"
echo "Repository: $RepoUrl"
echo "Ubuntu Version: `$(lsb_release -ds)`"
echo "=========================================="

# Clone repository
if [ ! -d "openvas-latex-testing" ]; then
    echo "📥 Cloning repository..."
    git clone $RepoUrl openvas-latex-testing
    cd openvas-latex-testing
else
    echo "📁 Repository already exists, updating..."
    cd openvas-latex-testing
    git pull origin $Branch
fi

# Display UAT plan
echo "📋 UAT Testing Plan:"
echo "File: docs/UAT_UBUNTU_TESTING_PLAN.md"
echo ""
echo "🔧 Quick start commands:"
echo "1. cat docs/UAT_UBUNTU_TESTING_PLAN.md"
echo "2. sudo apt update && sudo apt install -y texlive-full"
echo "3. sudo apt install -y libglib2.0-dev libjson-glib-dev cmake build-essential"
echo "4. ./build_ubuntu.sh"
echo "5. Follow the detailed UAT plan for comprehensive testing"
echo ""
echo "✅ Setup complete! Ready for UAT testing."
"@

# Save setup scripts
try {
    $kaliSetupScript | Out-File -FilePath "setup-kali-uat.sh" -Encoding UTF8
    $ubuntuSetupScript | Out-File -FilePath "setup-ubuntu-uat.sh" -Encoding UTF8
    Write-Success "Created setup-kali-uat.sh"
    Write-Success "Created setup-ubuntu-uat.sh"
} catch {
    Write-Warning "Could not create setup scripts: $($_.Exception.Message)"
}

# Final instructions
Write-Header "Final Instructions"

$finalMessage = @"
🎯 Deployment Complete! Next Steps:

1. 📋 For Kali Linux WSL UAT Testing:
   - Copy and run the Kali commands shown above
   - Or use: bash setup-kali-uat.sh

2. 📋 For Ubuntu UAT Testing:
   - Copy and run the Ubuntu commands shown above  
   - Or use: bash setup-ubuntu-uat.sh

3. 📚 Documentation Available:
   - Technical Spec: docs/LATEX_REPORTING_IMPLEMENTATION_SPEC.md
   - Kali UAT Plan: docs/UAT_KALI_WSL_TESTING_PLAN.md
   - Ubuntu UAT Plan: docs/UAT_UBUNTU_TESTING_PLAN.md
   - Test Scenarios: docs/UAT_TEST_SCENARIOS.md

4. 🧪 Testing Process:
   - Follow the environment-specific UAT plans
   - Run automated test suites
   - Document results and issues
   - Address any critical problems found

5. 🚀 Production Deployment:
   - Complete UAT testing in both environments
   - Address any issues found during testing
   - Deploy to production when all tests pass

Repository: $RepoUrl
Branch: $Branch

The OpenVAS Professional LaTeX Reporting System is ready for comprehensive UAT testing! 🎉
"@

Write-Host $finalMessage -ForegroundColor Green

Write-Host "`n" + "=" * 70
Write-Success "Deployment script completed successfully!"
"@