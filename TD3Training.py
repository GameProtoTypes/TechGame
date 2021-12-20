import numpy as np
import torch
import argparse
import os
import time
from GYMServer import *
import utils
import TD3



# Runs policy for X episodes and returns average reward
def evaluate_policy(policy, eval_episodes=10):
	avg_reward = 0.
	for i in range(eval_episodes):
		obs = gym.Reset(1)
		done = False
		while not done:
			action = policy.select_action(np.array(obs))
			action = np.resize(action,(1,np.shape(action)[0]))
			
			obs, reward, done = gym.TakeActions(action)
			avg_reward += reward

	avg_reward /= eval_episodes

	print("---------------------------------------")
	print("Evaluation over %d episodes: %f" % (eval_episodes, avg_reward))
	print("---------------------------------------")
	return avg_reward


if __name__ == "__main__":
	
	parser = argparse.ArgumentParser()
	parser.add_argument("--policy_name", default="TD3")					# Policy name
	parser.add_argument("--env_name", default="ATRT")			    # gym environment name
	parser.add_argument("--seed", default=0, type=int)					# Sets Gym, PyTorch and Numpy seeds
	parser.add_argument("--start_timesteps", default=1e4, type=int)		# How many time steps purely random policy is run for
	parser.add_argument("--eval_freq", default=5e3, type=float)			# How often (time steps) we evaluate
	parser.add_argument("--max_timesteps", default=1e7, type=float)		# Max time steps to run environment for
	parser.add_argument("--save_models", default=True, type=bool)		# Whether or not models are saved
	parser.add_argument("--expl_noise", default=0.1, type=float)		# Std of Gaussian exploration noise
	parser.add_argument("--batch_size", default=100, type=int)			# Batch size for both actor and critic
	parser.add_argument("--discount", default=0.99, type=float)			# Discount factor
	parser.add_argument("--tau", default=0.005, type=float)				# Target network update rate
	parser.add_argument("--policy_noise", default=0.2, type=float)		# Noise added to target policy during critic update
	parser.add_argument("--noise_clip", default=0.5, type=float)		# Range to clip target policy noise
	parser.add_argument("--policy_freq", default=2, type=int)			# Frequency of delayed policy updates
	parser.add_argument("--num_gyms", default=50, type=int)				# Number of Gyms Expected to connect to
	args = parser.parse_args()

	file_name = "%s_%s_%s" % (args.policy_name, args.env_name, str(args.seed))
	file_name_baseline = "%s_%s_baseline_%s" % (args.policy_name, args.env_name, str(args.seed))
	print("---------------------------------------")
	print("Settings: %s" % (file_name))
	print("---------------------------------------")

	if not os.path.exists("./results"):
		os.makedirs("./results")
	if args.save_models and not os.path.exists("./pytorch_models"):
		os.makedirs("./pytorch_models")

	if(args.save_models):
		print("Model Saving Enabled")


	#env = gym.make(args.env_name)
	gym = GYMInterface()
	gym.ConnectToGYMS(1)


	# Set seeds
	#env.seed(args.seed)
	torch.manual_seed(args.seed)
	np.random.seed(args.seed)
	
	state_dim = gym.stateSize
	action_dim = gym.actionSize
	max_action = 1.0

	# Initialize policy
	if args.policy_name == "TD3": policy = TD3.TD3(state_dim, action_dim, max_action)
	elif args.policy_name == "OurDDPG": policy = OurDDPG.DDPG(state_dim, action_dim, max_action)
	elif args.policy_name == "DDPG": policy = DDPG.DDPG(state_dim, action_dim, max_action)


	# load baseline policy
	#policy.load(file_name_baseline, directory="./pytorch_models")

	replay_buffer = utils.ReplayBuffer()
	
	# Evaluate untrained policy
	evaluations = [evaluate_policy(policy)] 

	total_timesteps = 0
	timesteps_since_eval = 0
	episode_num = 0
	done = True

	t0 = time.time()

	while total_timesteps < args.max_timesteps:
		
		if done: 

			if total_timesteps != 0: 
				print(f"Total T: {total_timesteps} Episode Num: {episode_num} Episode T: {episode_timesteps} Reward: {episode_reward}  --  Wallclk T: {int(time.time() - t0)} sec")
				if args.policy_name == "TD3":
					policy.train(replay_buffer, episode_timesteps, args.batch_size, args.discount, args.tau, args.policy_noise, args.noise_clip, args.policy_freq)
				else: 
					policy.train(replay_buffer, episode_timesteps, args.batch_size, args.discount, args.tau)
			
			# Evaluate episode
			if timesteps_since_eval >= args.eval_freq:
				timesteps_since_eval %= args.eval_freq
				evaluations.append(evaluate_policy(policy))
				
				if args.save_models: 
					policy.save(file_name, directory="./pytorch_models")
				
				np.save(f"./results/{(file_name)}", evaluations) 
			
			# Reset environment
			obs = gym.Reset(1)
			done = False
			episode_reward = 0
			episode_timesteps = 0
			episode_num += 1 
		
		# Select action randomly or according to policy
		if total_timesteps < args.start_timesteps:
			action = np.random.randn(gym.actionSize)
		else:
			action = policy.select_action(np.array(obs))
			if args.expl_noise != 0: 
				action = (action + np.random.normal(0, args.expl_noise, size=gym.actionSize)).clip(-1.0, 1.0)

		# Perform action
		action = np.resize(action,(1,np.shape(action)[0]))

		new_obs, reward, done = gym.TakeActions(action) 
		
		episode_reward += reward

		# Store data in replay buffer
		replay_buffer.add((obs[0], new_obs[0], action[0], reward[0], done[0]))

		obs = new_obs

		episode_timesteps += 1
		total_timesteps += 1
		timesteps_since_eval += 1
		
	# Final evaluation 
	evaluations.append(evaluate_policy(policy))
	policy.save("%s" % (file_name), directory="./pytorch_models")
	np.save(f"./results/{(file_name)}", evaluations)  
