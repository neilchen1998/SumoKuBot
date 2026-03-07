#!/bin/bash

# Configure the repo
cmake -S . -B build &&

# Build the repo
cmake --build build &&

# Run the app
./build/apps/app
