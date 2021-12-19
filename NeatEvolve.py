import socket
import sys
import time
import torch
import neat
import os
import pickle
from GYMServer import *





def eval_genomes(genomes, config):
    
    #create all the nets
    nets = []
    for genome_id, genome in genomes:
        nets.append(neat.nn.FeedForwardNetwork.create(genome, config))

            

    states = gym.Reset(len(genomes))
    fitnesses = np.zeros((gym.numGYMS,1))
    ends = np.zeros((gym.numGYMS,1), dtype=int)

    while ends[0] == 0:
        gymIdx = 0
        actions = []
        #print(genomes)
        for genome_id, genome in genomes:
            #print(gymIdx)
            actions.append(nets[gymIdx].activate(states[gymIdx]))
            gymIdx+=1

        
        states, rewards, ends = gym.TakeActions(actions)

        #print(np.shape(fitnesses))
        #print(np.shape(states))
        #print(np.shape(rewards))
        #print(ends)
        fitnesses = np.add(fitnesses,rewards)
        #print(np.shape(fitnesses))

    print(ends)
    gymIdx = 0
    for genome_id, genome in genomes:
        genome.fitness = fitnesses[gymIdx][0]
        #print(genome.fitness)
        gymIdx+=1



gym = GYMInterface()
gym.ConnectToGYMS(1)
print("Connected.")

# Load the config file, which is assumed to live in
# the same directory as this script.
local_dir = os.path.dirname(__file__)
config_path = os.path.join(local_dir, 'neatconfig')
config = neat.Config(neat.DefaultGenome, neat.DefaultReproduction,
                     neat.DefaultSpeciesSet, neat.DefaultStagnation,
                     config_path)

pop = neat.Population(config)
stats = neat.StatisticsReporter()
pop.add_reporter(stats)
pop.add_reporter(neat.StdOutReporter(True))

#pe = neat.ParallelEvaluator(multiprocessing.cpu_count()*2, eval_genome)
winner = pop.run(eval_genomes,250)

# Save the winner.
with open('winner', 'wb') as f:
    pickle.dump(winner, f)




