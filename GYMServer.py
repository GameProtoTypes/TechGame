import socket
import sys
import torch
import struct
import numpy as np
import win32pipe, win32file, pywintypes

def ReadInt(connection):

  intdata = win32file.ReadFile(connection, 4)
  integer = struct.unpack('<i',intdata[1])[0]

  return integer

def ReadFloat(connection):
    floatdata = win32file.ReadFile(connection, 4)
    number = struct.unpack('<f',floatdata[1])[0]
    return number


class GYMInterface:
  def __init__(self):
    self.name = "TrainerInstance"
    self.connection = []
    self.numGYMS    = 0
    self.actionSize = 0
    self.stateSize  = 0



  #Allow GYMS to connect
  def ConnectToGYMS(self, expectedGYMS = 1):
    self.connection = win32pipe.CreateNamedPipe(
        r'\\.\pipe\GYMPIPE',
        win32pipe.PIPE_ACCESS_DUPLEX,
        win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_WAIT,
        1, 65536, 65536,
        300,
        None)
     


    print("waiting for client")
    win32pipe.ConnectNamedPipe(self.connection, None)
    print("got client")



    print("Retrieving GYM Configuration..")
    numActions = ReadInt(self.connection)
    numStates  = ReadInt(self.connection)
    print(f'actionSize:{numActions}, stateSize:{numStates}')
    self.stateSize += numStates
    self.actionSize += numActions


  def Reset(self, numGYMS):
    self.numGYMS = numGYMS
    states = np.zeros((self.numGYMS, self.stateSize))

    #code for
    win32file.WriteFile(self.connection, struct.pack('<i',1))#code for reset
    win32file.WriteFile(self.connection, struct.pack('<i',self.numGYMS))


    for g in range(0,self.numGYMS):
        for v in range(0, self.stateSize):
          states[g][v] = ReadFloat(self.connection)
    print(f"Read {self.numGYMS * self.stateSize} Floats")


    verify = ReadInt(self.connection)
    print(f"Verify:{verify}, Bytes: {struct.pack('<i',verify)}")
    if(verify != 99999):
      print("SYNC ERROR")
      exit(1)

    return states


  # tell all gyms to take a set of actions and return reward
  def TakeActions(self, actionSets, render = False):      
    states = np.zeros((self.numGYMS, self.stateSize))
    rewards = np.zeros((self.numGYMS, 1))
    ends = np.zeros((self.numGYMS))



    win32file.WriteFile(self.connection, struct.pack('<i',0))#code for action
    win32file.WriteFile(self.connection, struct.pack('<i', int(render)))


    for g in range(0,self.numGYMS):
      for v in range(0, self.actionSize):#send the action set
        win32file.WriteFile(self.connection, struct.pack('<f',actionSets[g][v]))

  
    for g in range(0,self.numGYMS):

        for v in range(0, self.stateSize):
          states[g][v] = ReadFloat(self.connection)

        rewards[g] = ReadFloat(self.connection)

        ends[g] = ReadInt(self.connection)

    
    return states, rewards, ends
