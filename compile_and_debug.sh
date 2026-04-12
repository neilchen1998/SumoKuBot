#!/bin/bash

# Configure the repo in debug mode
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug &&

# builds the repo
cmake --build build -j$(nproc) &&

# Run the app
lldb ./build/apps/app
