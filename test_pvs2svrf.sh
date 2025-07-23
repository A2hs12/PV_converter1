#!/bin/bash
# Test script for PVS to SVRF conversion

# Navigate to the project directory
cd "$(dirname "$0")"

# Platform detection
PLATFORM=$(./pvconvertor/config/guess-platform)
CONVERTER="./pvconvertor/platform/$PLATFORM/bin/pvconverter"

# Check if sample files exist
if [ ! -f "test_pvs_file.pvs" ]; then
  echo "Creating sample PVS test file..."
  cat > test_pvs_file.pvs << 'EOF'
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
EOF
  echo "Sample PVS file created: test_pvs_file.pvs"
fi

# Run the conversion
echo "Converting PVS to SVRF..."
$CONVERTER -pvs2svrf test_pvs_file.pvs -o test_output.svrf

# Check result
if [ -f "test_output.svrf" ]; then
  echo "Conversion successful!"
  echo "Output file: test_output.svrf"
  echo "Contents:"
  cat test_output.svrf
else
  echo "Conversion failed! No output file was generated."
fi
