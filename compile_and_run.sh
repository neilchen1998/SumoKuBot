#!/bin/bash

# Build the repo
cmake --build build -j$(nproc) &&

# Run the app
./build/apps/app
