#!/bin/bash

# Build the repo
cmake --build build --config Release -j$(nproc) &&

# Run the app
./build/apps/app
