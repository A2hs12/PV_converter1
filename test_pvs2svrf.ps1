# Test script for PVS to SVRF conversion (PowerShell)

# Navigate to the project directory
$scriptPath = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptPath

# Check if sample files exist
if (-not (Test-Path -Path "test_pvs_file.pvs")) {
  Write-Host "Creating sample PVS test file..."
  @'
VERSION 5.6 ;

GLOBAL {
  CELLNAME = "test_cell" ;
  DATE = "2023/05/15" ;
}

LAYER metal1 ;
  WIDTH 0.12 ;
  SPACING 0.12 ;

LAYER metal2 ;
  WIDTH 0.14 ;
  SPACING 0.14 ;

VIA via1 metal1 metal2 ;
  RECTANGLE 0.12 0.12 ;
  ENCLOSURE 0.04 0.04 ;
'@ | Out-File -FilePath "test_pvs_file.pvs" -Encoding ascii
  Write-Host "Sample PVS file created: test_pvs_file.pvs"
}

# Run the conversion
Write-Host "Converting PVS to SVRF..."

# Determine the script directory
$binDir = Join-Path $scriptPath "bin"

# Run the command using the available converter
& "$binDir\pvconverter.exe" -pvs2svrf -i test_pvs_file.pvs -o test_output.svrf

# Check result
if (Test-Path -Path "test_output.svrf") {
  Write-Host "Conversion successful!"
  Write-Host "Output file: test_output.svrf"
  Write-Host "Contents:"
  Get-Content -Path "test_output.svrf"
}
else {
  Write-Host "Conversion failed! No output file was generated."
}
