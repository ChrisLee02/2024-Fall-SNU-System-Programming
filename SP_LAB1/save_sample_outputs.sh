#!/bin/bash

# Define the list of test files
test_files=("test0.c" "test1.c" "test2.c" "test3.c" "test4.c" "test5.c" "test_mine.c")

# Create a directory to store the results
mkdir -p sample_outputs

# Loop through each test file and save the outputs
for file in "${test_files[@]}"; do
  echo "Saving output for $file"

  # Run sampledecomment and save the outputs
  ./sampledecomment < "$file" > "sample_outputs/${file}_output" 2> "sample_outputs/${file}_errors"
done

echo "All sample outputs saved in the sample_outputs/ directory."
