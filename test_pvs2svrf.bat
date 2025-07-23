@echo off
REM Test script for PVS to SVRF conversion

REM Check if sample files exist
if not exist test_pvs_file.pvs (
  echo Creating sample PVS test file...
  (
    echo VERSION 5.6 ;
    echo.
    echo GLOBAL {
    echo   CELLNAME = "test_cell" ;
    echo   DATE = "2023/05/15" ;
    echo }
    echo.
    echo LAYER metal1 ;
    echo   WIDTH 0.12 ;
    echo   SPACING 0.12 ;
    echo.
    echo LAYER metal2 ;
    echo   WIDTH 0.14 ;
    echo   SPACING 0.14 ;
    echo.
    echo VIA via1 metal1 metal2 ;
    echo   RECTANGLE 0.12 0.12 ;
    echo   ENCLOSURE 0.04 0.04 ;
  ) > test_pvs_file.pvs
  echo Sample PVS file created: test_pvs_file.pvs
)

REM Run the conversion
echo Converting PVS to SVRF...

REM Run the command using the available converter
bin\pvconverter.exe -pvs2svrf -i test_pvs_file.pvs -o test_output.svrf

REM Check result
if exist test_output.svrf (
  echo Conversion successful!
  echo Output file: test_output.svrf
  echo Contents:
  type test_output.svrf
) else (
  echo Conversion failed! No output file was generated.
)

pause
