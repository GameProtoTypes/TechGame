import socket
import sys
import time
import torch
from GYMServer import *

gym = GYMInterface()
gym.ConnectToGYMS(1)

print("Connected.")
numGYMS = 10
gym.Reset(numGYMS)


while True:
    actions = np.random.rand(gym.numGYMS,gym.actionSize)*2 - 1

    states, rewards, ends = gym.TakeActions(actions)
    
    #print(ends)
    if(ends[0] == 1):
        gym.Reset(numGYMS)
        numGYMS+=1
        print("ENDS:")
        print(ends)



