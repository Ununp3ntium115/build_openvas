@echo off
echo 🚀 Deploying OpenVAS Professional LaTeX Reporting System
echo ============================================================

REM Add all files
echo ℹ️  Adding files to git...
git add .

REM Commit changes
echo ℹ️  Committing changes...
git commit -m "feat: Add OpenVAS Professional LaTeX Reporting System - Complete implementation with LaTeX engine, templates, AI integration, and UAT testing plans for Kali and Ubuntu"

REM Push to repository
echo ℹ️  Pushing to repository...
git push origin main

if %ERRORLEVEL% NEQ 0 (
    echo ❌ Normal push failed, trying force push...
    set /p confirm="Force push? This will overwrite remote history. (y/N): "
    if /i "%confirm%"=="y" (
        git push --force origin main
        if %ERRORLEVEL% EQU 0 (
            echo ✅ Force push successful
        ) else (
            echo ❌ Force push failed
        )
    ) else (
        echo ℹ️  Push cancelled
    )
) else (
    echo ✅ Push successful
)

echo.
echo 🐉 Kali Linux WSL Setup Commands:
echo git clone https://github.com/Ununp3ntium115/build_openvas.git openvas-latex-testing
echo cd openvas-latex-testing
echo cat docs/UAT_KALI_WSL_TESTING_PLAN.md
echo sudo apt update ^&^& sudo apt install -y texlive-full libglib2.0-dev libjson-glib-dev cmake build-essential
echo.
echo 🐧 Ubuntu Setup Commands:
echo git clone https://github.com/Ununp3ntium115/build_openvas.git openvas-latex-testing
echo cd openvas-latex-testing  
echo cat docs/UAT_UBUNTU_TESTING_PLAN.md
echo sudo apt update ^&^& sudo apt install -y texlive-full libglib2.0-dev libjson-glib-dev cmake build-essential
echo.
echo ✅ Deployment complete! Use the commands above to start UAT testing.
pause