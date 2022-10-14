# udppingserver_no_loss.py
from socket import *
# Create a UDP socket
serverSocket = socket(AF_INET, SOCK_DGRAM)
# Assign IP address and port number to socket
serverSocket.bind(('', 12000))
serverSocket.settimeout(10)
while True:
	try:
	# Receive the client packet along with the address it is coming from
		message, address = serverSocket.recvfrom(1024)
	# The server responds
	#serverSocket.sendto(message, address)
		print("Server received heartbeat pulse " + message.decode() + " pulse interval was 5 seconds")
	except:
		print("No pulse after 10 seconds. Server quits.")
		print("Server Stops.")
		break
