import socket
import random
# Configuration
HOST = "0.0.0.0"  # Bind to all available IP addresses
PORT = 5005       # Port to listen on
BUFFER_SIZE = 20248  # Buffer size for receiving packets
random.seed()
def syn_ack_response(addr, data, sock):
    ip, port = addr
    if(len(data) < 4):
        return False
    msg = data[0:3]
    isn = data[3:]
    isn = int(isn)
    isn += 1
    rp_isn = random.randint(isn, 100000)
    print(msg)
    print(isn)
    if (msg == "SYN"):
        message = "SYN-ACK" + str(isn) + str(rp_isn)
        server_address = (ip, port)
        print(server_address)
        sock.sendto(message.encode("utf-8"), (ip, port))
        print("Message sent")
        return True
def ack_response(addr, data, sock):
    ip, port = addr
    if (len(data) < 8):
        return False
    msg = data[0:3]
    isn = data[3:]
    isn = int(isn)
    isn += 1
    print(msg)
    print(isn)
    if (msg == "ACK"):
        return True;
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
    if(syn_ack_response(addr, data, sock)):
        print("syn ack complete")


    

# Close the socket when done (though the script will typically not exit)
sock.close()
