# udppingserver_no_loss.py
from socket import *
# Import random and create randome a variable between 20 and 40 for Modulus calculation
import random
R3 = 0
R4 = 0
# Setup sleep function.
import time
# Create a UDP socket
serverSocket = socket(AF_INET, SOCK_DGRAM)
# Assign IP address and port number to socket
serverSocket.bind(('', 12000))
while True:
	# Receive the client packet along with the address it is coming from
	message, address = serverSocket.recvfrom(1024)
	# The server responds with random delay
	R1 = random.randrange(10,40)
	R2 = R1 % 100
	time.sleep(R2/1000)
	R3 = random.randint(1,100)
	if R3 < 80 and R4 < 80:
		serverSocket.sendto(message, address)
		R4 = R4+1
	else:
		continue
