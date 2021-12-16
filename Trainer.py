import socket
import sys
import torch
import struct
import numpy as np


def ReadInt(connection):
  intdata = connection.recv(4)
  integer = struct.unpack('<i',intdata)[0]
  return integer

def ReadFloat(connection):
    floatdata = connection.recv(4)
    number = struct.unpack('<f',floatdata)[0]
    return number


class GYMInterface:
  def __init__(self):
    self.name = "TrainerInstance"
    self.connections = []
    self.numGYMS    = 0
    self.actionSize = 0
    self.stateSize  = 0



  #Allow GYMS to connect
  def ConnectToGYMS(self, expectedGYMS = 1):
    # Set up a TCP/IP server
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind the socket to server address and port 5000
    server_address = ('localhost', 5041)
    tcp_socket.bind(server_address)
    tcp_socket.setblocking(True);
     
    tcp_socket.listen(expectedGYMS)
    for i in range(0,expectedGYMS):
      print("Waiting for connection " + str(0))
      connection = tcp_socket.accept()
      self.connections.append(connection)
      print("Retrieving GYM Configuration..")
      numActions = ReadInt(connection[0])
      numStates  = ReadInt(connection[0])
      print(f'actionSize:{numActions}, stateSize:{numStates}')
      self.stateSize += numStates
      self.actionSize += numActions


  def Reset(self, numGYMS):
    self.numGYMS = numGYMS
    states = np.zeros((self.numGYMS, self.stateSize))
    connection = self.connections[0][0]
    connection.send(struct.pack('<i',1))
    connection.send(struct.pack('<i',self.numGYMS))


    for g in range(0,self.numGYMS):
        for v in range(0, self.stateSize):
          states[g][v] = ReadFloat(connection)
    print(f"Read {self.numGYMS * self.stateSize} Floats")


    verify = ReadInt(connection)
    print(f"Verify:{verify}, Bytes: {struct.pack('<i',verify)}")



    return states


  # tell all gyms to take a set of actions and return reward
  def TakeActions(self, actionSets):      
    states = np.zeros((self.numGYMS, self.stateSize))
    rewards = np.zeros((self.numGYMS, 1))
    ends = np.zeros((self.numGYMS))

    connection = self.connections[0][0]
    connection.send(struct.pack('<i',0))

    for g in range(0,self.numGYMS):
      for v in range(0, self.actionSize):#send the action set
        connection.send(struct.pack('<f',actionSets[g][v]))
  
    for g in range(0,self.numGYMS):

        for v in range(0, self.stateSize):
          states[g][v] = ReadFloat(connection)

        rewards[g] = ReadFloat(connection)

        ends[g] = ReadInt(connection)

    
    return states, rewards, ends
