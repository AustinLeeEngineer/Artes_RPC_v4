import socket

DISCOVERY_PORT = 30718
DISCOVERY_QUERY = b'\x00\x00\x00\xf8'
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.settimeout(5)
sock.sendto(DISCOVERY_QUERY, ('192.168.0.255', DISCOVERY_PORT))  # Use your subnet broadcast

try:
    while True:
        data, addr = sock.recvfrom(1024)
        if data.startswith(b'\x00\x00\x00\xf9'):
            ip = addr[0]
            mac = ':'.join(f'{byte:02x}' for byte in data[4:10])
            print(f'Discovered XPort at IP: {ip}, MAC: {mac}')
except socket.timeout:
    print('No response')
