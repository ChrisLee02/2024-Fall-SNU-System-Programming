#!/bin/bash

# Array to hold commands for each client, ensuring newline at the end
commands=(
  "CREATE item1 description1"
  "CREATE item2 description2"
  "UPDATE item1 description_updated"
  "READ item1"
  "READ item2"
  "DELETE item1"
  "DELETE item2"
)

# Start each client with the respective command
for cmd in "${commands[@]}"; do
  (echo -e "${cmd}\n" | ../src/client) &
  sleep 1  # Ensure commands don't collide
done

# Wait for all background processes to finish
wait
