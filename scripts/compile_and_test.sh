#!/bin/bash

# builds the repo
cmake --build build --config Release -j$(nproc) &&

# runs all the tests
cmake --build build --target test
