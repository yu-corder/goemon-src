#!/bin/bash

set -e

GREEN='\033[32m'
RED='\033[31m'
NC='\033[0m'

for testfile in tests/*.goe
do
    name=$(basename "$testfile" .goe)

    expected="tests/${name}.expected"
    bytecode="tests/${name}.gb"

    echo "Testing $name"

    ./kama-c "$testfile" "$bytecode" > /dev/null
    ./kama-e "$bytecode" > actual.txt

    if diff actual.txt "$expected" > /dev/null; then
        echo -e "${GREEN}✓ [PASS]${NC} $name"
    else
        echo -e "${RED}✗ [FAIL]${NC} $name"
        exit 1
    fi
done