# Build script for PV Converter (PowerShell)

# Navigate to the project directory
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptPath

# Clean any previous build artifacts
make clean

# Build the project
make

Write-Host "Build complete. Executable is in platform/<platform>/bin/pvconverter"
