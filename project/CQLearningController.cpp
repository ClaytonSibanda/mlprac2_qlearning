/**
         (                                      
   (     )\ )                                   
 ( )\   (()/(   (    ) (        (        (  (   
 )((_)   /(_)) ))\( /( )(   (   )\  (    )\))(  
((_)_   (_))  /((_)(_)|()\  )\ |(_) )\ )((_))\  
 / _ \  | |  (_))((_)_ ((_)_(_/((_)_(_/( (()(_) 
| (_) | | |__/ -_) _` | '_| ' \)) | ' \)) _` |  
 \__\_\ |____\___\__,_|_| |_||_||_|_||_|\__, |  
                                        |___/   

Refer to Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
for a detailed discussion on Q Learning
*/
#include "CQLearningController.h"

#include <vector>


CQLearningController::CQLearningController(HWND hwndMain):
	CDiscController(hwndMain),
	_grid_size_x(CParams::WindowWidth / CParams::iGridCellDim + 1),
	_grid_size_y(CParams::WindowHeight / CParams::iGridCellDim + 1)
{
}

/**
 Allocates space for the Q Table and the dead table and initializes them appropriately.
*/
void CQLearningController::InitializeLearningAlgorithm(void)
{
	//initialize the Q-table with all 0's. Initially, every action in every state has neutral reard.
	for (uint x = 0; x < _grid_size_x; x++){
		std::vector<std::vector<double> > state_y;
		for (uint y = 0; y < _grid_size_y; y++){
			state_y.push_back({0.0, 0.0, 0.0, 0.0});
		}

		_Q_sx_sy_a.push_back(state_y);
	}

	//Allocate & initialize the dead table
	resetDeadCheck();
}

/**
 Initializes the deadCheck table. The dead Check table is used to ensure that when the agents die,
 They update the Qtable to reflect the negative reward resulting from that action in that particular state.
*/
void CQLearningController::resetDeadCheck(){
	deadCheck.clear();
	for (int i = 0; i < m_NumSweepers; i++){
		deadCheck.push_back(false);
	}
}

/**
 This is the reward function. It dispenses a reward to the given sweeper that has just transitioned into the state
 represented by x and y. If the new state is favourable (picked up a mine), a hihg positive rewasrd is given. If the
 state is negative, drove into a rock or supermine, then a negative reward is given to "punish" this bad action.
*/
double CQLearningController::R(uint x, uint y, uint sweeper_no){
	double reward = 0.0f; //neutral reward

	//see what the minesweeper hit and adjust the reward accordingly
	int hit_index = ((m_vecSweepers[sweeper_no])->CheckForObject(m_vecObjects, CParams::dMineScale));
	if (hit_index >= 0)
	{
		switch (m_vecObjects[hit_index]->getType()){
			case CDiscCollisionObject::Mine:{
				if (!m_vecObjects[hit_index]->isDead()){
					//If we hit a mine, return a nice positve reward
					reward = 100.0;
				}
				break;
			}
			case CDiscCollisionObject::Rock:{
				//we hit a rock and died, so return a negative reward
				reward = -100.0;
				break;
			}
			case CDiscCollisionObject::SuperMine:{
				//we hit a supermine and died, returna negative reward
				reward = -100.0;
				break;
			}
		}
	}

	return reward;
}

/**
 Takes in a list of actions and returns the action that has historically, 
 yielded the highest reward. Where there are mutiple maximum valued actions,
 one of these actions is randomly chosen. This allows the agents to explore 
 the environment before beginning to exploit.
*/
int getMaxAct(std::vector<double> actions){
	//Find the maximum.
	int max_act = 0;
	double max_val = actions[max_act];
	for (uint a = 1; a < 4; a++){
		if (actions[a] > max_val){
			max_act = a;
			max_val = actions[a];
		}
	}

	//find all the maximum valued actions
	std::vector<int> maxs;
	for (uint i = 0; i < 4; i++){
		if (actions[i] == max_val){
			maxs.push_back(i);
		}
	}

	//return one of the maximum valued actions
	int ri = RandInt(0, maxs.size() - 1);
	return maxs[ri];
}

/**
 Takes in a list of actions and returns the maximum value given by any of the actions.
 i.e. the expected reward that will be obtained if the agen behaves optimally.
*/
int getMaxActVal(std::vector<double> actions){
	double max_val = actions[0];
	for (uint a = 1; a < 4; a++){
		if (actions[a] > max_val){
			max_val = actions[a];
		}
	}

	return max_val;
}

/**
The update method. Main loop body of our Q Learning implementation
See: Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
*/
bool CQLearningController::Update(void)
{
	uint cDead = std::count_if(m_vecSweepers.begin(), m_vecSweepers.end(), [](CDiscMinesweeper * s)->bool{return s->isDead();});
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;

		//reset our deadCheck table
		resetDeadCheck();
	}

	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) continue;

		//1:::Observe the current state:
		SVector2D<int> pos = m_vecSweepers[sw]->Position();
		pos /= 10;

		//2:::Select action with highest historic return:
		int max_act = getMaxAct(_Q_sx_sy_a[pos.x][pos.y]);
		m_vecSweepers[sw]->setRotation((ROTATION_DIRECTION)max_act);
	}

	//now call the parents update, so all the sweepers fulfill their chosen action
	CDiscController::Update(); //call the parent's class update. Do not delete this.
	
	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead() && deadCheck[sw]){
			//if we died and already updated the Q table, just skip/continue
			continue; 
		}
		else if(m_vecSweepers[sw]->isDead()){
			//if we just died, then run  through the loop to update the Qtable with the
			//negative reward from dying. To ensure that this only happens once after we die,
			//set deadcheck to true.
			deadCheck[sw] = true;
		}

		//3:::Observe new state:
		SVector2D<int> prev_pos = m_vecSweepers[sw]->PrevPosition();
		prev_pos /= 10;

		SVector2D<int> pos = m_vecSweepers[sw]->Position();
		pos /= 10;

		int action = (int)m_vecSweepers[sw]->getRotation();

		//4:::Update _Q_s_a accordingly:
		_Q_sx_sy_a[prev_pos.x][prev_pos.y][action] += (lambda_learn_rt * (R(pos.x, pos.y, sw) + (gamma_discount_rt * getMaxActVal(_Q_sx_sy_a[pos.x][pos.y])) - _Q_sx_sy_a[prev_pos.x][prev_pos.y][action]));
	}

	if (m_iTicks == CParams::iNumTicks){
		//if the iteration is over, all the sweepers will be respawned in the next iteration, so reset
		//the deadcheck table so it will remain valid.
		resetDeadCheck();
	}

	return true;
}

CQLearningController::~CQLearningController(void)
{
	//TODO: dealloc stuff here if you need to	
}
