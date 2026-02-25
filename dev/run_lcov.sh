#!/bin/bash

## Use this script to analyze code coverage across tests. Will only
## work in the coverage configuration (see documentation).

# Path to the lcov output folder
LCOV_DIR="./lcov"
TESTS_DIR="./bin/tests"

# Check if the tests directory exists
if [ ! -d "$TESTS_DIR" ]; then
    echo "Error: Tests directory '$TESTS_DIR' does not exist. Please ensure the tests are compiled."
    exit 1
fi

# Clean existing coverage data
echo "Cleaning existing coverage data..."
find . -name "*.gcda" -exec rm {} +

# Run all test executables in a temporary directory
echo "Running test executables..."
for TEST_EXECUTABLE in "$TESTS_DIR"/*tests; do
    if [ -x "$TEST_EXECUTABLE" ]; then
        echo "Running $TEST_EXECUTABLE..."
        TMPDIR=$(mktemp -d)
        (
            cd "$TMPDIR"
            "$OLDPWD/$TEST_EXECUTABLE"
        )
        rm -rf "$TMPDIR"
    else
        echo "Warning: Skipping $TEST_EXECUTABLE (not executable or not found)."
    fi
done

# Create the lcov output folder if it doesn't exist
echo "Creating lcov output folder..."
mkdir -p "$LCOV_DIR"

# Capture coverage data and save it in the lcov directory
echo "Capturing coverage data..."
lcov --capture --directory build/ --output-file "$LCOV_DIR/lcov.info" --ignore-errors mismatch

# Check if lcov.info was generated
if [ ! -f "$LCOV_DIR/lcov.info" ]; then
    echo "Error: Failed to generate lcov.info. Please check your configuration."
    exit 1
fi

# Filter out unnecessary files and save the filtered data in the lcov directory
echo "Filtering unnecessary files..."
lcov --remove "$LCOV_DIR/lcov.info" '/usr/*' 'tests/*' 'build/*/boost/*' --output-file "$LCOV_DIR/lcov.info"

# Generate HTML report in the lcov directory
echo "Generating HTML report..."
genhtml "$LCOV_DIR/lcov.info" --output-directory "$LCOV_DIR"

# Check if the HTML report was generated
if [ ! -f "$LCOV_DIR/index.html" ]; then
    echo "Error: Failed to generate HTML report. Please check your configuration."
    exit 1
fi

# Final message to the user
echo "Code coverage analysis completed! All output files are in the '$LCOV_DIR' directory."
echo "You can view the coverage report by opening '$LCOV_DIR/index.html' in your browser."