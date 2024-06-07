import socket

# Configuration
HOST = "0.0.0.0"  # Bind to all available IP addresses
PORT = 5005       # Port to listen on
BUFFER_SIZE = 1024  # Buffer size for receiving packets

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the address and port
sock.bind((HOST, PORT))

print(f"Listening on port {PORT}...")

while True:
    # Receive UDP packet
    data, addr = sock.recvfrom(BUFFER_SIZE)
    data = data.decode("utf-8")  # Assuming received data is in text format

    # Print received data and address
    print(f"Received from {addr}: {data}")

# Close the socket when done (though the script will typically not exit)
sock.close()
