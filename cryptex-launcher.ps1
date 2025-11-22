# Standalone CRYPTEX Launcher for Windows (PowerShell)
# Starts all CRYPTEX services in standalone mode

param(
    [Parameter(Position=0)]
    [ValidateSet('start', 'stop', 'restart', 'status', 'logs')]
    [string]$Command = 'start',

    [Parameter(Position=1)]
    [string]$Service = 'cryptex-api'
)

$ErrorActionPreference = "Stop"

# Find CRYPTEX installation
$CryptexDir = $null
if ($env:CRYPTEX_HOME) {
    $CryptexDir = $env:CRYPTEX_HOME
} elseif (Test-Path "$env:USERPROFILE\.cryptex") {
    $CryptexDir = "$env:USERPROFILE\.cryptex"
} elseif (Test-Path "$PSScriptRoot\cryptex") {
    $CryptexDir = "$PSScriptRoot\cryptex"
} else {
    Write-Host "Error: CRYPTEX installation not found" -ForegroundColor Red
    Write-Host "Set CRYPTEX_HOME environment variable or run install script"
    exit 1
}

$BinDir = Join-Path $CryptexDir "bin"
$DataDir = Join-Path $CryptexDir "data"
$RunDir = Join-Path $CryptexDir "run"

New-Item -ItemType Directory -Force -Path $RunDir | Out-Null

# Banner
Write-Host @"
╔═══════════════════════════════════════════════════╗
║   _____ ______   ______ _______ _______ _    _   ║
║  / ____|  _ \ \ / /  _ \__   __|__   __| |  | |  ║
║ | |    | |_) \ V /| |_) | | |     | |  | |  | |  ║
║ | |    |  _ < | | |  __/  | |     | |  | |  | |  ║
║ | |____| |_) || | | |     | |     | |  | |__| |  ║
║  \_____|____/ |_| |_|     |_|     |_|   \____/   ║
║                                                   ║
║        The Anarchist Vulnerability Platform      ║
╚═══════════════════════════════════════════════════╝
"@ -ForegroundColor Blue

function Start-CryptexService {
    param([string]$ServiceName, [string]$Binary)

    $pidFile = Join-Path $RunDir "$ServiceName.pid"

    if ((Test-Path $pidFile)) {
        $pid = Get-Content $pidFile
        if (Get-Process -Id $pid -ErrorAction SilentlyContinue) {
            Write-Host "⚠ $ServiceName already running (PID: $pid)" -ForegroundColor Yellow
            return
        }
    }

    Write-Host "Starting $ServiceName..." -ForegroundColor Yellow

    $binaryPath = Join-Path $BinDir $Binary
    if (-not (Test-Path $binaryPath)) {
        Write-Host "✗ Binary not found: $binaryPath" -ForegroundColor Red
        return
    }

    $logFile = Join-Path $RunDir "$ServiceName.log"
    $process = Start-Process -FilePath $binaryPath -NoNewWindow -PassThru `
        -RedirectStandardOutput $logFile -RedirectStandardError $logFile

    $process.Id | Out-File -FilePath $pidFile

    Start-Sleep -Seconds 1

    if (Get-Process -Id $process.Id -ErrorAction SilentlyContinue) {
        Write-Host "✓ $ServiceName started (PID: $($process.Id))" -ForegroundColor Green
    } else {
        Write-Host "✗ Failed to start $ServiceName" -ForegroundColor Red
        Get-Content $logFile
    }
}

function Stop-CryptexService {
    param([string]$ServiceName)

    $pidFile = Join-Path $RunDir "$ServiceName.pid"

    if (-not (Test-Path $pidFile)) {
        Write-Host "⚠ $ServiceName not running" -ForegroundColor Yellow
        return
    }

    $pid = Get-Content $pidFile
    $process = Get-Process -Id $pid -ErrorAction SilentlyContinue

    if (-not $process) {
        Write-Host "⚠ $ServiceName not running (stale PID)" -ForegroundColor Yellow
        Remove-Item $pidFile -Force
        return
    }

    Write-Host "Stopping $ServiceName (PID: $pid)..." -ForegroundColor Yellow
    Stop-Process -Id $pid -Force
    Remove-Item $pidFile -Force
    Write-Host "✓ $ServiceName stopped" -ForegroundColor Green
}

function Get-CryptexServiceStatus {
    param([string]$ServiceName)

    $pidFile = Join-Path $RunDir "$ServiceName.pid"

    if (Test-Path $pidFile) {
        $pid = Get-Content $pidFile
        if (Get-Process -Id $pid -ErrorAction SilentlyContinue) {
            Write-Host "● $ServiceName running (PID: $pid)" -ForegroundColor Green
        } else {
            Write-Host "● $ServiceName dead (stale PID)" -ForegroundColor Red
        }
    } else {
        Write-Host "● $ServiceName stopped" -ForegroundColor Yellow
    }
}

switch ($Command) {
    'start' {
        Write-Host "Starting CRYPTEX services..."
        Start-CryptexService -ServiceName "cryptex-api" -Binary "the_interface_server.exe"
        Write-Host ""
        Write-Host "CRYPTEX is running!" -ForegroundColor Green
        Write-Host "API: http://localhost:8080" -ForegroundColor Blue
        Write-Host "Health check: Invoke-WebRequest http://localhost:8080/health" -ForegroundColor Blue
    }

    'stop' {
        Write-Host "Stopping CRYPTEX services..."
        Stop-CryptexService -ServiceName "cryptex-api"
    }

    'restart' {
        Write-Host "Restarting CRYPTEX services..."
        Stop-CryptexService -ServiceName "cryptex-api"
        Start-Sleep -Seconds 2
        Start-CryptexService -ServiceName "cryptex-api" -Binary "the_interface_server.exe"
    }

    'status' {
        Write-Host "CRYPTEX Service Status:"
        Get-CryptexServiceStatus -ServiceName "cryptex-api"
    }

    'logs' {
        $logFile = Join-Path $RunDir "$Service.log"
        if (Test-Path $logFile) {
            Get-Content $logFile -Wait
        } else {
            Write-Host "Log file not found: $logFile" -ForegroundColor Red
        }
    }
}
