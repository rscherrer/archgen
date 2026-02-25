#!/bin/bash

## Use this script to run all the test executables. Will only
## run if tests have been compiled.

# Ensure the script exits on errors
set -e

# Path to the tests folder
TESTS_DIR="./bin/tests"

# Check if the directory exists
if [ ! -d "$TESTS_DIR" ]; then
    echo "Error: Tests directory '$TESTS_DIR' does not exist."
    exit 1
fi

# Change to the tests directory
cd "$TESTS_DIR"

# Loop through all executables in the tests directory
for TEST_EXECUTABLE in *tests; do
    if [ -x "$TEST_EXECUTABLE" ]; then

        # Run tests
        echo "Running $TEST_EXECUTABLE..."
        ./"$TEST_EXECUTABLE"
        
    else
        echo "Skipping $TEST_EXECUTABLE (not executable)"
    fi
done

echo "All tests completed!"