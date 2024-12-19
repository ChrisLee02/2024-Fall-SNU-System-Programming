#!/bin/bash

# Default port number
PORT=8080
TEST_CASE=1  # Default test case

# Parse arguments for port number and test case
while getopts "p:c:" opt; do
    case $opt in
        p) PORT=$OPTARG ;;
        c) TEST_CASE=$OPTARG ;;
        *) echo "Usage: $0 [-p port] [-c test_case]"; exit 1 ;;
    esac
done

# Initialize output directory
OUTPUT_DIR="./output"
if [[ -d $OUTPUT_DIR ]]; then
    rm -rf $OUTPUT_DIR  # Delete the directory if it exists
fi
mkdir -p $OUTPUT_DIR    # Create a new directory

# Define test cases
case $TEST_CASE in
    1)
        REQUESTS=("CREATE hello 1" "CREATE hello 2" "CREATE hello 3" "CREATE hello 4")
        RESPONSES=("CREATE OK" "COLLISION" "COLLISION" "COLLISION")
        INTERVALS=(5 5 5 5)
        ;;
    2)
        REQUESTS=("CREATE hello world" "READ hello" "READ hello" "READ hello")
        RESPONSES=("CREATE OK" "world" "world" "world")
        INTERVALS=(5 5 0 0)
        ;;
    3)
        REQUESTS=("CREATE hello world" "READ hello" "CREATE bye snu" "READ bye")
        RESPONSES=("CREATE OK" "world" "CREATE OK" "snu")
        INTERVALS=(5 5 -5 0)
        ;;
    4)
        REQUESTS=("CREATE hello world" "DELETE bye" "READ hello" "UPDATE hello snu" "DELETE hello" "READ hello")
        RESPONSES=("CREATE OK" "NOT FOUND" "world" "UPDATE OK" "DELETE OK" "NOT FOUND")
        INTERVALS=(5 0 5 5 5 -10)
        ;;
    *)
        echo "Invalid test case number: $TEST_CASE"
        exit 1
        ;;
esac

echo "=== Starting Requests and Responses for Test Case $TEST_CASE ==="

START_TIME=$(date +%s)
RESPONSE_TIMES=()

for i in "${!REQUESTS[@]}"; do
    echo "Sending Request $i: '${REQUESTS[$i]}'"
    
    # Send request and capture the response time
    { 
        echo "${REQUESTS[$i]}" | ./client -p $PORT | ts '[%Y-%m-%d %H:%M:%S]' > \
        "$OUTPUT_DIR/response_$i.log"
        RESPONSE_TIMES[$i]=$(date +%s)  # Record the time when response is received
        echo "DEBUG: RESPONSE_TIMES[$i] = ${RESPONSE_TIMES[$i]}"
    } &
    
    # Small delay to ensure proper sequence
    sleep 0.1
done

# Wait for all background processes to finish
wait


echo "=== Verifying Responses for Test Case $TEST_CASE ==="

# Verify the responses
for i in "${!REQUESTS[@]}"; do
    echo "Checking response for Request $i..."
    if ! grep -q "${RESPONSES[$i]}" "$OUTPUT_DIR/response_$i.log"; then
        echo -e "\033[31mTest Failed: '${RESPONSES[$i]}' missing in response_$i.log\033[0m"
        exit 1
    fi
    echo "Response for Request $i: '${RESPONSES[$i]}' verified successfully."
done

echo "=== Calculating Response Time Differences for Test Case $TEST_CASE ==="

# Verify the response time intervals
for ((i = 1; i < ${#RESPONSE_TIMES[@]}; i++)); do
    diff=$((RESPONSE_TIMES[$i] - RESPONSE_TIMES[$((i - 1))]))
    expected_interval="${INTERVALS[$i]}"
    echo "Response $((i - 1)) to Response $i: $diff seconds (expected: $expected_interval seconds)"
    
    # Allow small deviations (±1 second)
    if [[ $diff -lt $((expected_interval - 1)) || $diff -gt $((expected_interval + 1)) ]]; then
        echo -e "\033[31mTest Failed: Time difference between Response $((i - 1)) and Response $i is not in range $((expected_interval - 1))-$((expected_interval + 1)) seconds\033[0m"
        exit 1
    fi
done

echo -e "\033[32mTest Passed for Test Case $TEST_CASE: All conditions satisfied.\033[0m"
exit 0
