#!/usr/bin/env python3
"""
SCPI Automated Test Script for ARTES RPC (XPort)
Sprint 2 requirements
"""

import socket
import time
import sys
from datetime import datetime

# ========================= CONFIG =========================
HOST = "192.168.0.250"      # ← Change to your XPort IP
PORT = 10001                # Common raw TCP port for XPort
TIMEOUT = 2.0               # seconds to wait for response
# =========================================================

def send_command(sock, cmd: str):
    """Send a command with proper CRLF termination"""
    full_cmd = cmd.strip() + "\r\n\0"
    sock.sendall(full_cmd.encode('ascii'))
    print(f">>> {cmd.strip()}")

def read_response(sock):
    """Read response with timeout"""
    try:
        response = sock.recv(4096).decode('ascii', errors='replace').strip()
        if response:
            print(f"<<< {response}")
            return response
        return ""
    except socket.timeout:
        print("<<< [Timeout - no response]")
        return ""

def run_test(commands, count=1):
    print(f"\n=== SCPI Test Started at {datetime.now()} ===\n")
    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.settimeout(TIMEOUT)
        try:
            sock.connect((HOST, PORT))
            print(f"Connected to {HOST}:{PORT}\n")
            
            for _ in range(count):
                for cmd in commands:
                    send_command(sock, cmd)
                    time.sleep(0.06)          # Small delay between commands
                    read_response(sock)
                    time.sleep(0.02)          # Brief pause
            
        except Exception as e:
            print(f"Connection error: {e}")
    
    print(f"\n=== Test Completed at {datetime.now()} ===")

# ====================== TEST COMMANDS ======================
test_commands = [
    "*IDN?",
]

if __name__ == "__main__":
    if len(sys.argv) > 1:
        HOST = sys.argv[1]
    if len(sys.argv) > 2:
        PORT = int(sys.argv[2])

    print("SCPI Automated Test Tool")
    print(f"Target: {HOST}:{PORT}")
    print("-" * 50)

    try:
        run_test(test_commands, count=100)   # Change count as needed
    except KeyboardInterrupt:
        print("\nTest interrupted by user.")
