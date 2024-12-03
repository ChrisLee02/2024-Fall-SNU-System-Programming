#!/bin/bash

# Array to hold commands for each client, ensuring newline at the end
commands=(
  "CREATE hello world1"
  "CREATE hello world2"
  "CREATE hello world3"
  "CREATE hello world4"
)

# Start each client with the respective command
for cmd in "${commands[@]}"; do
  (echo -e "${cmd}\n" | ../src/client) &
  sleep 1  # Ensure commands don't collide
done

# Wait for all background processes to finish
wait
