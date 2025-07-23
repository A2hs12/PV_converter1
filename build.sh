#!/bin/bash
# Build script for PV Converter

# Navigate to the project directory
cd "$(dirname "$0")"

# Clean any previous build artifacts
make clean

# Build the project
make

echo "Build complete. Executable is in platform/<platform>/bin/pvconverter"
