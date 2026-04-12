#!/bin/bash

# Build the repo
cmake --build build -j$(nproc) &&

# Run the benchmark(s)
./build/benchmarks/mathbenchmark
./build/benchmarks/solverbenchmark
