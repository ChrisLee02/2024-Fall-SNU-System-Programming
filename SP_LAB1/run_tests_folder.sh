#!/bin/bash

# Directory containing test files
test_dir="./c_test_cases"

# Find all .c files in the test directory
test_files=($(find "$test_dir" -name "*.c"))

# Loop through each test file
for file in "${test_files[@]}"; do
  echo "Testing $file"

  # Run sampledecomment and decomment on the test file
  ./sampledecomment < "$file" > output1 2> errors1
  ./decomment < "$file" > output2 2> errors2

  # Compare the outputs
  diff -c output1 output2 > diff_output.txt
  diff -c errors1 errors2 >> diff_output.txt

  if [ -s diff_output.txt ]; then
    echo "Differences found for $file"
    cat diff_output.txt
  else
    echo "Success: No differences found for $file"
  fi

  # Clean up
  rm output1 errors1 output2 errors2 diff_output.txt
done
