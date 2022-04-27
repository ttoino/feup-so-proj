#!/bin/bash

expect_error() {
    $1 2>/dev/null 1>/dev/null && echo "Expected '$1' to generate error" >&2
}

expect_val() {
    out=$($1 2>/dev/null)
    expected=$(cat "$2")

    [ "$out" != "$expected" ] && echo "Unexpected output for '$1'" >&2
}

p="phrases"
echo "Testing $p"
cd "$p/tests" || exit 1
p="../$p"

# Invalid arguments
expect_error "$p"
expect_error "$p a"
expect_error "$p a a"
expect_error "$p a a a"

# Working example
expect_val "$p quote.txt" "quote.out.txt"
expect_val "$p -l quote.txt" "quote.l.out.txt"

cd "../.."

p="addmx"
echo "Testing $p"
cd "$p/tests" || exit 1
p="../$p"

# Invalid arguments
expect_error "$p"
expect_error "$p a"
expect_error "$p a a"
expect_error "$p a a a"

# Working example
expect_val "$p matrix1.txt matrix2.txt" "matrix1+2.txt"

# Malformed sizes
expect_error "$p matrix1.error1.txt matrix1.txt"
expect_error "$p matrix1.txt matrix1.error1.txt"

# Malformed row
expect_error "$p matrix1.error2.txt matrix1.txt"
expect_error "$p matrix1.txt matrix1.error2.txt"

# Extra row
expect_error "$p matrix1.error3.txt matrix1.txt"
expect_error "$p matrix1.txt matrix1.error3.txt"

cd "../.."
