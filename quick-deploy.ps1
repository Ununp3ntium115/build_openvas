#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Quick Deploy OpenVAS LaTeX Reporting System
.DESCRIPTION
    Simple script to commit and push the OpenVAS Professional LaTeX Reporting implementation
.PARAMETER RepoUrl
    Your git repository URL
#>

param(
    [Parameter(Mandatory=$false)]
    [string]$RepoUrl = "https://github.com/Ununp3ntium115/build_openvas.git"
)

# Color functions
function Write-Success { param($Message) Write-Host "✅ $Message" -ForegroundColor Green }
function Write-Error { param($Message) Write-Host "❌ $Message" -ForegroundColor Red }
function Write-Info { param($Message) Write-Host "ℹ️  $Message" -ForegroundColor Cyan }

Write-Host "🚀 Quick Deploy OpenVAS Professional LaTeX Reporting System" -ForegroundColor Magenta
Write-Host "Repository: $RepoUrl" -ForegroundColor Yellow
Write-Host "=" * 70

# Check git status
Write-Info "Checking git status..."
try {
    git status --short
} catch {
    Write-Error "Git command failed"
    exit 1
}

# Add all files
Write-Info "Adding all files..."
git add .

# Check what's staged
Write-Info "Files staged for commit:"
git diff --cached --name-only

# Commit with simple message
Write-Info "Committing changes..."
$commitMsg = "feat: Add OpenVAS Professional LaTeX Reporting System

Complete implementation with:
- LaTeX report generation engine
- Professional templates and AI integration  
- Comprehensive UAT testing plans for Kali and Ubuntu
- Full documentation and examples

Ready for UAT testing."

try {
    git commit -m $commitMsg
    Write-Success "Commit successful"
} catch {
    Write-Error "Commit failed: $($_.Exception.Message)"
}

# Handle push issues
Write-Info "Attempting to push..."
try {
    # Try normal push first
    git push origin main
    Write-Success "Push successful"
} catch {
    Write-Error "Normal push failed, trying force push..."
    
    # Ask for confirmation before force push
    $confirm = Read-Host "Force push? This will overwrite remote history. (y/N)"
    if ($confirm -eq "y" -or $confirm -eq "Y") {
        try {
            git push --force origin main
            Write-Success "Force push successful"
        } catch {
            Write-Error "Force push also failed: $($_.Exception.Message)"
            Write-Info "Manual resolution may be required"
        }
    } else {
        Write-Info "Push cancelled. You may need to resolve conflicts manually."
    }
}

# Generate Kali setup commands
Write-Host "`n🐉 Kali Linux WSL Setup Commands:" -ForegroundColor Green
Write-Host @"
git clone $RepoUrl openvas-latex-testing
cd openvas-latex-testing
cat docs/UAT_KALI_WSL_TESTING_PLAN.md
sudo apt update && sudo apt install -y texlive-full libglib2.0-dev libjson-glib-dev cmake build-essential
"@ -ForegroundColor Cyan

# Generate Ubuntu setup commands  
Write-Host "`n🐧 Ubuntu Setup Commands:" -ForegroundColor Green
Write-Host @"
git clone $RepoUrl openvas-latex-testing
cd openvas-latex-testing
cat docs/UAT_UBUNTU_TESTING_PLAN.md
sudo apt update && sudo apt install -y texlive-full libglib2.0-dev libjson-glib-dev cmake build-essential
"@ -ForegroundColor Yellow

Write-Host "`n✅ Deployment complete! Use the commands above to start UAT testing." -ForegroundColor Green