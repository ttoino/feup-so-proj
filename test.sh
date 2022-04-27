#!/bin/bash

check_error() {
    $1 2>/dev/null 1>/dev/null && echo "Expected '$1' to generate error" >&2
}

check_val() {
    out=$($1 2>/dev/null)
    expected=$(cat "$2")

    [ "$out" != "$expected" ] && echo "Unexpected output for '$1'" >&2
}

p="./phrases"
echo "Testing $p"
cd "$p" || exit 1

# Invalid arguments
check_error "$p"
check_error "$p a"
check_error "$p a a"
check_error "$p a a a"

# Working example
check_val "$p tests/quote.txt" "tests/quote.out.txt"
check_val "$p -l tests/quote.txt" "tests/quote.l.out.txt"

cd ".."

p="./addmx"
echo "Testing $p"
cd "$p" || exit 1

# Invalid arguments
check_error "$p"
check_error "$p a"
check_error "$p a a b"
check_error "$p a a"

# Working example
check_val "$p tests/matrix1.txt tests/matrix2.txt" "tests/matrix1+2.txt"

check_error "$p tests/matrix1.error1.txt tests/matrix1.txt"
check_error "$p tests/matrix1.txt tests/matrix1.error1.txt"

check_error "$p tests/matrix1.error2.txt tests/matrix1.txt"
check_error "$p tests/matrix1.txt tests/matrix1.error2.txt"

check_error "$p tests/matrix1.error3.txt tests/matrix1.txt"
check_error "$p tests/matrix1.txt tests/matrix1.error3.txt"

cd ".."
