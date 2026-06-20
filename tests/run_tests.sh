#!/bin/bash

set -e

for testfile in tests/*.goe
do
    name=$(basename "$testfile" .goe)

    expected="tests/${name}.expected"
    bytecode="tests/${name}.gb"

    echo "Testing $name"

    ./kama-c "$testfile" "$bytecode" > /dev/null
    ./kama-e "$bytecode" > actual.txt

    if diff actual.txt "$expected" > /dev/null; then
        echo "[PASS] $name"
    else
        echo "[FAIL] $name"
        exit 1
    fi
done