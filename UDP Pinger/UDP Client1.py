from socket import *
import datetime
import time
# Time object and timer
now = datetime.datetime.now()
# Timer list
Timelist = []
# ServerName
serverName = ''
# Port Number
serverPort = 12000
# Create UPD socket and assign IP address and socket number
clientSocket = socket(AF_INET, SOCK_DGRAM)
# Ping variable
pings = 0
# Timeout variable
timeout = 0
# Try and Except for timeout
#clientSocket.settimeout(1)
# Messages / pings in a for loop of 10 
while True:
	#try:
	pings = pings + 1
	message = str(pings)
# Send messages or pings
	time.sleep(5)
	clientSocket.sendto(message.encode(),(serverName, serverPort))
		#T1 = time.time()
# Receieve respondes from server and capture time elapsed
	#message2, serverAddress = clientSocket.recvfrom(1024)
		#T2 = time.time()
		#Timelist.append(T2-T1)
# Print received messages
		#print('Ping ' + str(pings) +': host replied: seq ' + message2.decode() + ' ' + time.asctime() + ", RTT = " + str(round((T2-T1)*1000,2)) + " ms")
	print("heartbeat pluse " + str(pings))
	#except: 
	#print('Request timed out, message was lost')
	#timeout = timeout + 1
# Print Min, Max, Avg values
#print("Min RTT: " + str(round(min(Timelist)*1000,2)) + ' ms')
#print("Max RTT: " + str(round(max(Timelist)*1000,2)) + ' ms')
#print("Avg RTT: " + str(round(sum(Timelist)/len(Timelist)*1000,2)) + ' ms')
#print("Packet lost: " + str(round(100*(1-((pings-timeout)/pings)),2)) + ' %'  )
clientSocket.close()
 
