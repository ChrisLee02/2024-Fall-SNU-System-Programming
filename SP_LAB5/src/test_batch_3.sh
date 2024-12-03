#!/bin/bash

# Array to hold commands for each client, ensuring newline at the end
commands=(
"CREATE user alice"
  "CREATE user bob"
  "READ user"
  "UPDATE user alice_updated"
  "DELETE user"
  "READ user"
)

# Start each client with the respective command
for cmd in "${commands[@]}"; do
  (echo -e "${cmd}\n" | ../src/client) &
  sleep 1  # Ensure commands don't collide
done

# Wait for all background processes to finish
wait
