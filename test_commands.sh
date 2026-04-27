#!/bin/bash
IP=${1:-192.168.1.250}
PORT=${2:-10001}
COUNT=${3:-200}

echo "Running high-speed SCPI stress test ($COUNT commands)..."

{
    for ((i=1; i<=COUNT; i++)); do
        case $((RANDOM % 6)) in
            0) echo "*IDN?" ;;
            1) echo "SYSTEM:STACK?" ;;
            2) echo "SYSTEM:FREQ?" ;;
            3) echo "SYSTEM:WDT?" ;;
        esac
        echo -e "\r"
    done
} | nc "$IP" "$PORT"
