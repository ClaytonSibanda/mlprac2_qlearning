##Machine Learning: Prac2 - Q-Learning

**Author** : Muhummad Yunus Patel  
**Student#** : PTLMUH006  
**Date**  : 29-October-2015

###Introduction:
Q-learning is a reinforcement learning technique. Agents learn a function Q that gives the expected reward of taking some action a in a given state s. Agents learn this Q function by exploring the actions in each state and observing the reward they receive for that particular action. They then use this reward to inform future decisions.

In the given minesweeper framework, the agents are the minesweepers and the ultimate goal is to have the minesweepers learn to stay alive (by not hitting rocks and supermines) and collect the mines. The state of each agent is represented as the agent’s x-y position in the simulation grid. For each state, there are four possible actions that the agent could take, viz. East, North, West, or South. Each action corresponds to the minesweeper agent turning and moving in that particular direction.

This report serves to document the algorithms used to implement the Q-learning algorithm that controls the minesweepers and present the results obtained. The implementation of the algorithm can be found in the CQLearningController.cpp and CQLearningController.h files.

###Results and Conclusions:
The CQLearningController used a shared Q-table to allow the minesweeper agents to learn to collect mines while avoiding rocks and supermines. A shared table was used instead of individual Q-tables to allow for each minesweeper to update the Q-table with the section of the map that they are currently exploring. This approach is valid because the minesweepers are all in the same environment and the rewards for their actions in each of the states will be the same. The stochasticity introduced in the getMaxAct method will avoid the agents’ behaviour becoming entirely deterministic. The shared Q-table also allows for much faster parallel learning and speeds up the convergence.

The algorithm was tested in three test environments, described below, and the average performance of the minesweeper agents in each of these environments is tabulated below. In each case, the simulation was left to run for 15 iterations to allow the agents to learn and the following 20 iterations were then averaged to obtain the results tabulated in the report handed in (available on request).

From the tabulated results, we can clearly see that the minesweeper agents have learned the required behaviour. In each environment, after learning for 15 iterations, the minesweepers collect all the mines in the environment and then continue to move about, but they do not die at all! It was noted that the algorithm took longer to converge in the tougher environments with more rocks and supermines. This is probably because the agents would die earlier on in the iteration thereby taking longer to fully explore the environment.



###Usage:
 * Compile and run as a Visual Studio 2013 project.
 * Only works on Windows.