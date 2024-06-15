import socket
import random
# Configuration
HOST = "0.0.0.0"  # Bind to all available IP addresses
PORT = 5005       # Port to listen on
BUFFER_SIZE = 20248  # Buffer size for receiving packets
random.seed()
def parse_ack_and_syn(data):
    return (data[0:3], int(data[3:]))

def syn_ack_response(addr, data, sock):
    ip, port = addr
    if(len(data) < 4):
        return False
    msg, isn = parse_ack_and_syn(data)
    isn += 1
    rp_isn = random.randint(isn, 100000)
    print(msg)
    print(isn)
    if (msg == "SYN"):
        message = "SYN-ACK" + str(isn) + str(rp_isn)
        server_address = (ip, port)
        print(server_address)
        sock.sendto(message.encode("utf-8"), (ip, port))
        print("SYN-ACK sent")
        data, addr = sock.recvfrom(BUFFER_SIZE)
        data = data.decode("utf-8")  # Assuming received data is in text format
        msg, isn = parse_ack_and_syn(data)
        # Print received data and address
        print(f"Received ACK from {addr}: {data}")
        if(isn == rp_isn + 1):
            print("Authenticated:", addr)
            return True
    return False
# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to the address and port
sock.bind((HOST, PORT))

print(f"Listening on port {PORT}...")
trusted_clients = []
while True:
    # Receive UDP packet
    data, addr = sock.recvfrom(BUFFER_SIZE)
    data = data.decode("utf-8")  # Assuming received data is in text format

    # Print received data and address
    print(f"Received from {addr}: {data}")
    if(syn_ack_response(addr, data, sock)):
        trusted_clients.append(addr)

    

# Close the socket when done (though the script will typically not exit)
sock.close()
