#!/bin/bash

set -e

GREEN='\033[32m'
RED='\033[31m'
NC='\033[0m'

echo "=== Runtime Tests ==="

for testfile in tests/exec/*.goe
do
    name=$(basename "$testfile" .goe)

    expected="tests/exec/${name}.expected"
    bytecode="tests/exec/${name}.gb"

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

echo
echo "=== AST Tests ==="

for testfile in tests/ast/*.goe
do
    name=$(basename "$testfile" .goe)

    expected="tests/ast/${name}.expected"
    bytecode="tests/ast/${name}.gb"

    echo "Testing AST $name"

    ./kama-c --ast "$testfile" "$bytecode" > actual.txt

    if diff actual.txt "$expected" > /dev/null; then
        echo -e "${GREEN}✓ [PASS]${NC} $name"
    else
        echo -e "${RED}✗ [FAIL]${NC} $name"
        exit 1
    fi
done

rm -f actual.txt