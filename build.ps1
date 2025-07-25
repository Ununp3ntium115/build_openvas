# PowerShell build script for AI-Enhanced OpenVAS

Write-Host "Building AI-Enhanced OpenVAS..." -ForegroundColor Green

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location build

# Configure with CMake
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
cmake -DCMAKE_BUILD_TYPE=Debug -DBUILD_CORE_COMPONENTS=OFF ..

if ($LASTEXITCODE -eq 0) {
    Write-Host "Configuration successful!" -ForegroundColor Green
    
    # Build the project
    Write-Host "Building project..." -ForegroundColor Yellow
    cmake --build . --config Debug
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "Build successful!" -ForegroundColor Green
        Write-Host "AI Engine library created in build/ai-engine/" -ForegroundColor Cyan
    } else {
        Write-Host "Build failed!" -ForegroundColor Red
    }
} else {
    Write-Host "Configuration failed!" -ForegroundColor Red
    Write-Host "Make sure you have the required dependencies installed:" -ForegroundColor Yellow
    Write-Host "- CMake 3.10+" -ForegroundColor White
    Write-Host "- GLib 2.42+" -ForegroundColor White
    Write-Host "- JSON-GLib" -ForegroundColor White
    Write-Host "- libcurl" -ForegroundColor White
}

Set-Location ..