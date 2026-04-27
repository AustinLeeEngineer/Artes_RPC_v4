#!/bin/bash

# Usage: ./script.sh [REPEATS] (default 10)
REPEATS=${1:-10}  # Default to 10 if no arg
IP=192.168.0.250
PORT=10001
TIMEOUT=5  # Per command

# Command sequence (add delays, queries, etc.)
COMMANDS=(
  "*IDN?"
  "SYSTEM:STACK?"
  "SYSTEM:FREQ?"
  "SYSTEM:WDT?"
)

# Repeat the sequence REPEATS times
i=1
while [ $i -le $REPEATS ]; do
  echo "Iteration $i of $REPEATS"
  for cmd in "${COMMANDS[@]}"; do
    echo "Sending: $cmd"
    (echo "$cmd"; sleep 1) | socat -T$TIMEOUT - TCP:$IP:$PORT || {
      echo "Error: socat failed for $cmd" >&2
      exit 1  # Exit on failure; remove for continue
    }
  done
  ((i++))
done
