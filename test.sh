#!/bin/bash

expect_error() {
    $1 2>/dev/null 1>/dev/null && echo "Expected '$1' to generate error" >&2
}

expect_value() {
    out=$(eval "$1 2>/dev/null")
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
expect_value "$p quote.txt" "quote.out.txt"
expect_value "$p -l quote.txt" "quote.l.out.txt"

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
expect_value "$p matrix1.txt matrix2.txt" "matrix1+2.txt"

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

p="cypher"
echo "Testing $p"
cd "$p/tests" || exit 1
p="../$p"

# No cypher.txt
rm -f "cypher.txt"
expect_error "$p"

# Empty cypher.txt
touch "cypher.txt"
expect_value "$p < quote.txt" "quote.txt"

# Working example
cat "cypher1.txt" >|"cypher.txt"
expect_value "$p < quote.txt" "quote.cypher1.txt"
expect_value "$p < quote.cypher1.txt" "quote.txt"

# Spaces in cypher
cat "cypher2.txt" >|"cypher.txt"
expect_value "$p < quote.txt" "quote.cypher2.txt"
expect_value "$p < quote.cypher2.txt" "quote.txt"

# Broken cypher
cat "cypher3.txt" >|"cypher.txt"
expect_error "$p < quote.txt"

rm -f "cypher.txt"

cd "../.."
