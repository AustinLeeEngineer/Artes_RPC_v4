import socket
import subprocess
import re

DISCOVERY_PORT = 30718
DISCOVERY_QUERY = b'\x00\x00\x00\xf8'
BROADCAST_ADDR = '192.168.0.255'  # Adjust to your subnet broadcast if needed

def get_mac_from_arp(ip):
    # Ping the IP to populate ARP cache (1 packet, quiet, 1s timeout)
    subprocess.run(['ping', '-c', '1', '-W', '1', ip], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    
    # Run arp -n and grep for the IP
    arp_output = subprocess.run(['arp', '-n'], capture_output=True, text=True).stdout
    mac_match = re.search(r'{}[^\n]*?((?:[0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}})'.format(re.escape(ip)), arp_output)
    
    return mac_match.group(1) if mac_match else None

# Set up UDP socket for broadcast
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.settimeout(5)

# Send discovery query
sock.sendto(DISCOVERY_QUERY, (BROADCAST_ADDR, DISCOVERY_PORT))

discovered = False

try:
    while True:
        data, addr = sock.recvfrom(1024)
        if data.startswith(b'\x00\x00\x00\xf9'):
            ip = addr[0]
            mac = get_mac_from_arp(ip)
            if mac:
                print(f'Discovered XPort at IP: {ip}, MAC: {mac}')
                discovered = True
            else:
                print(f'Discovered XPort at IP: {ip}, but no MAC found in ARP')
                discovered = True
except socket.timeout:
    if not discovered:
        print('No response')
finally:
    sock.close()
