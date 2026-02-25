#!/bin/bash

## This script builds the program according to the instructions
## found in the CMakeLists.txt configuration file. To be run
## from the root directory of the project. If an argument is
## provided, it will be passed to the switch.sh script to change
## the configuration before building. The script will remove
## the build and bin directories before building. The script
## will also create a build directory if it does not exist.

# Home directory
HOMEDIR=$(pwd)

# Check if an argument is provided
if [ $# -eq 1 ]; then
    # Run switch.sh with the provided argument
    ./dev/switch.sh "$1"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to switch configuration."
        exit 1
    fi
fi

# Remove build and bin
rm -rf build
rm -rf bin

# Make a build directory
mkdir -p build

# Go there
cd build

# Prepare
cmake ..

# Build
cmake --build .

# Back to home
cd $HOMEDIR