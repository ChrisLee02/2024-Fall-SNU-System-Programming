#!/bin/bash

# Usage: create abc "value"

if [ "$#" -ne 3 ]; then
    echo "Usage: create <filename> <value>"
    exit 1
fi

command="$1"
filename="$2"
content="$3"

if [ "$command" != "create" ]; then
    echo "Unknown command: $command"
    exit 1
fi

# 시작 부분을 'create abc'로 고정
final_content="create $filename $content"

# Repeat content to make it approximately 4000 bytes
while [ "${#final_content}" -lt 4000 ]; do
    final_content="${final_content}${content}"  # Append more content
done

# Trim to exactly 4000 bytes
final_content=${final_content:0:4000}

# Write content to file
echo -n "$final_content" > "$filename"

echo "File '$filename' created with content starting with 'create $filename'."
