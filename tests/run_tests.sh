#!/bin/bash

set -e

echo "=== Compile ==="
./kama-c tests/test.goe tests/test.gb > /dev/null

echo "=== Execute ==="
result=$(./kama-e tests/test.gb)

expected="VM Output: 7"

if [ "$result" = "$expected" ]; then
    echo "PASS"
else
    echo "FAIL"
    echo "expected: $expected"
    echo "actual:   $result"
    exit 1
fi