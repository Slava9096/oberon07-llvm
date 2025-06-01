#!/bin/bash
COMPILER=$1
INPUT=$2
TEST_NAME=$3
EXPECTED=$4

TMP_DIR=$(mktemp -d)
cd "$TMP_DIR" || {
    echo "Failed to enter temporary directory"
    exit 1
}

trap 'rm -rf "$TMP_DIR"' EXIT

"${COMPILER}" -i "${INPUT}" -o "${TEST_NAME}"
if [ $? -ne 0 ]; then
    echo "Compilation failed for ${INPUT}"
    exit 1
fi

"./${TEST_NAME}" > output.log 2>&1
EXIT_CODE=$?

if [ $EXIT_CODE -ne 0 ]; then
    echo "Execution failed for ${INPUT}, exit code $EXIT_CODE"
    cat output.log
    echo "Test failed. Temporary directory preserved: $TMP_DIR"
    trap - EXIT  # Disable cleanup
    exit 1
fi

diff -q output.log "${EXPECTED}" > /dev/null
if [ $? -ne 0 ]; then
    echo "Output mismatch for ${INPUT}"
    diff -u "${EXPECTED}" output.log
    echo "Test failed. Temporary directory preserved: $TMP_DIR"
    trap - EXIT
    exit 1
fi

echo "Test passed: ${INPUT}"
