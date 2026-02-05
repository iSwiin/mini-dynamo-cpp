#!/usr/bin/env bash
set -euo pipefail

cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_ASAN=ON -DENABLE_UBSAN=ON
cmake --build build -j
ctest --test-dir build --output-on-failure

echo
echo "Try:"
echo "  ./build/dynamoctl health"
echo "  ./build/dynamoctl put hello world"
echo "  ./build/dynamoctl get hello"
