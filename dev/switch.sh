#!/bin/bash

## Run this script to switch from one CMake configuration to
## another. To be run from the root directory of the project.
## The name of the configuration to use must be provided as
## e.g. --release, --tests, --profile or --coverage.

# Check if an argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 --release | --tests | --profile | --coverage"
    exit 1
fi

# Folder with templates
TEMPLATE_DIR="dev/cmake"

# Strip the "--" from the argument and construct the configuration file name
CONFIG_NAME="${1#--}"
CONFIG_FILE="$TEMPLATE_DIR/$CONFIG_NAME.cmake"

# Check if the configuration file exists
if [ ! -f "$CONFIG_FILE" ]; then
    echo "Error: Configuration file '$CONFIG_FILE' does not exist."
    exit 1
fi

# Replace the root CMakeLists.txt file
cp "$CONFIG_FILE" CMakeLists.txt
if [ $? -eq 0 ]; then
    echo "Switched to configuration: $1"
else
    echo "Error: Failed to switch configuration."
    exit 1
fi