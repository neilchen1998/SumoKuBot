#!/bin/bash

# builds the repo
cmake --build build --config Debug -j$(nproc &&

# Run the app
./build/apps/app
