#!/bin/bash

## Use this script to perform memory check. Will only work if the tests have been
## compiled.

# Ensure the script exits on errors
set -e

# Path to the tests folder
TESTS_DIR="./bin/tests"

# Path to the Valgrind logs folder (always in root)
LOGS_DIR="./valgrind"

# Root directory
ROOT_DIR=$(pwd)

# Check if the tests directory exists
if [ ! -d "$TESTS_DIR" ]; then
    echo "Error: Tests directory '$TESTS_DIR' does not exist."
    exit 1
fi

# Create the Valgrind logs directory in the root if it doesn't exist
mkdir -p "$LOGS_DIR"

# Loop through all executables in the tests directory
for TEST_EXECUTABLE in "$TESTS_DIR"/*; do
    if [ -x "$TEST_EXECUTABLE" ] && [ ! -d "$TEST_EXECUTABLE" ]; then

        EXEC_BASENAME=$(basename "$TEST_EXECUTABLE")
        echo "Running Valgrind on $EXEC_BASENAME..."
        LOG_FILE="$LOGS_DIR/${EXEC_BASENAME}.valgrind.log"

        # Create a temporary directory for this test run
        TMPDIR=$(mktemp -d)
        # Run valgrind with the temporary directory as CWD
        (
            cd "$TMPDIR"
            valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
                --log-file="$ROOT_DIR/$LOG_FILE" "$ROOT_DIR/$TEST_EXECUTABLE"
        )
        # Remove the temporary directory and its contents
        rm -rf "$TMPDIR"

        echo "Valgrind log saved to $LOG_FILE"
        SUMMARY_LINE=$(tail -n 1 "$LOG_FILE")
        echo "$SUMMARY_LINE"

    else
        [ -d "$TEST_EXECUTABLE" ] || echo "Skipping $(basename "$TEST_EXECUTABLE") (not executable)"
    fi
done

echo "All tests completed! Valgrind logs are in the '$LOGS_DIR' directory."