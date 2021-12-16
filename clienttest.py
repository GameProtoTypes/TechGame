import socket
import sys
 
# Create a connection to the server application on port 81
tcp_socket = socket.create_connection(('localhost', 5041))
 
try:
    data = 'Hi. I am a TCP client sending data to the server'
    tcp_socket.sendall(bytes(data, 'utf-8') )
 
finally:
    print("Closing socket")
    tcp_socket.close()