#!/bin/bash
IP=192.168.0.250
PORT=10001
TIMEOUT=5  # Per command

# Command sequence (add delays, queries, etc.)
COMMANDS=(
  "*IDN?"
  "SYSTEM:STACK?"
)

for cmd in "${COMMANDS[@]}"; do
  echo "Sending: $cmd"
  (echo "$cmd"; sleep 1) | socat -T$TIMEOUT - TCP:$IP:$PORT
done
