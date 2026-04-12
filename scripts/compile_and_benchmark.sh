#!/bin/bash

# Build the repo
cmake --build build --config Release -j$(nproc) &&

# Run the benchmark(s)
./build/benchmarks/mathbenchmark
./build/benchmarks/solverbenchmark
