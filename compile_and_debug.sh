#!/bin/bash

# configures the repo
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug &&

# builds the repo
cmake --build build &&

# Run the app
./build/apps/app
