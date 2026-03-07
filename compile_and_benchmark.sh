#!/bin/bash

# Configure the repo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Benchmark &&

# Build the repo
cmake --build build &&

# Run the benchmark(s)
./build/benchmarks/solverbenchmark
