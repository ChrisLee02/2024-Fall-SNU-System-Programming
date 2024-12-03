#!/bin/bash

# Array to hold commands for each client, ensuring newline at the end
commands=(
"CREATE key1 value1"
  "CREATE key1 value2"
  "READ key1"
  "UPDATE key1 updated_value1"
  "DELETE key1"
  "READ key1"
)

# Start each client with the respective command
for cmd in "${commands[@]}"; do
  (echo -e "${cmd}\n" | ../src/client) &
  sleep 1  # Ensure commands don't collide
done

# Wait for all background processes to finish
wait
