#!/bin/bash
set -e

echo "==========================================="
echo "        Building LoomIndex (C++20)"
echo "==========================================="
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

echo ""
echo "==========================================="
echo "        Running Unit Tests (GTest)"
echo "==========================================="
cd build
ctest --output-on-failure
cd ..

echo ""
echo "==========================================="
echo "        Running Crawler Demo"
echo "==========================================="
# Ausführen der kompilierten Hauptanwendung
./build/LoomIndex

echo ""
echo "==========================================="
echo "        Success! LoomIndex Finished."
echo "==========================================="
