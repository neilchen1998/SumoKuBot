#!/bin/bash

# builds the repo
cmake --build build -j$(nproc) &&

# runs all the tests
cmake --build build --target test
