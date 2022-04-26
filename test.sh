#!/bin/bash

check_val() {
    out=$($1)
    exit_code=$?
    expected=$(cat "$2")
    expected_exit_code="$3"

    [ "$out" != "$expected" ] && echo "Unexpected output for '$1'" >&2
    [ "$exit_code" != "$expected_exit_code" ] && echo "Unexpected exit code for '$1'" >&2
}

p="./phrases"

echo "Testing $p"

cd "$p" || exit 1

check_val "$p" "tests/noargs.out.txt" 0
check_val "$p tests/quote.txt" "tests/quote.out.txt" 0
check_val "$p -l tests/quote.txt" "tests/quote.l.out.txt" 0

cd ".."
