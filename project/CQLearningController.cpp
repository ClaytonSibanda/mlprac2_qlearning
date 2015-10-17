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
	printf("gx: %d   gy: %d\n", _grid_size_x, _grid_size_y);

	//InitializeLearningAlgorithm();

	//for (uint y = 0; y < _grid_size_y; y++){
	//	for (uint x = 0; x < _grid_size_x; x++){
	//		for (uint action = 0; action < 4; action++){
	//			printf("%.0f ", _Q_sx_sy_a[x][y][action]);
	//		}
	//	}
	//	printf("\n");
	//}
}
/**
 The update method should allocate a Q table for each sweeper (this can
 be allocated in one shot - use an offset to store the tables one after the other)

 You can also use a boost multiarray if you wish
*/
void CQLearningController::InitializeLearningAlgorithm(void)
{
	//TODO

	//initialize the Q-table with all 0's
	for (uint x = 0; x < _grid_size_x; x++){
		std::vector<std::vector<double> > state_y;
		for (uint y = 0; y < _grid_size_y; y++){
			state_y.push_back({0.0, 0.0, 0.0, 0.0});
		}

		_Q_sx_sy_a.push_back(state_y);
	}

	resetDeadCheck();
}

void CQLearningController::resetDeadCheck(){
	deadCheck.clear();
	for (int i = 0; i < m_NumSweepers; i++){
		deadCheck.push_back(false);
	}
}
/**
 The immediate reward function. This computes a reward upon achieving the goal state of
 collecting all the mines on the field. It may also penalize movement to encourage exploring all directions and 
 of course for hitting supermines/rocks!
*/
double CQLearningController::R(uint x, uint y, uint sweeper_no){
	//TODO: roll your own here!
	//return 0;

	double reward = 0.0f; //neutral reward

	//see what the minesweeper hit
	int hit_index = ((m_vecSweepers[sweeper_no])->CheckForObject(m_vecObjects, CParams::dMineScale));
	if (hit_index >= 0)
	{
		switch (m_vecObjects[hit_index]->getType()){
			case CDiscCollisionObject::Mine:{
				if (!m_vecObjects[hit_index]->isDead()){
					printf("NOTDEAD");
					reward = 100.0;
				}
				break;
			}
			case CDiscCollisionObject::Rock:{
				reward = -100.0;
				break;
			}
			case CDiscCollisionObject::SuperMine:{
				reward = -100.0;
				break;
			}
		}
	}
	//if (reward != 0)
	//	printf("%d reward = %f\n", sweeper_no, reward);
	return reward;
}

int getMaxAct(std::vector<double> actions){
	int max_act = 0;
	double max_val = actions[max_act];
	for (uint a = 1; a < 4; a++){
		if (actions[a] > max_val){
			max_act = a;
			max_val = actions[a];
		}
	}

	std::vector<int> maxs;
	for (uint i = 0; i < 4; i++){
		if (actions[i] == max_val){
		//if (actions[i] == max_val){
			maxs.push_back(i);
		}
	}

	int ri = RandInt(0, maxs.size() - 1);
	return maxs[ri];
	//return max_act;
}

int getMaxActVal(std::vector<double> actions){
	double max_val = actions[0];
	for (uint a = 1; a < 4; a++){
		if (actions[a] > max_val){
			max_val = actions[a];
		}
	}

	std::vector<int> maxs;
	for (uint i = 0; i < 4; i++){
		if (actions[i] == max_val){
			maxs.push_back(actions[i]);
		}
	}

	int ri = RandInt(0, maxs.size() - 1);
	return maxs[ri];
	//return max_val;
}

/**
The update method. Main loop body of our Q Learning implementation
See: Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
*/

bool CQLearningController::Update(void)
{
	//m_vecSweepers is the array of minesweepers
	//everything you need will be m_[something] ;)
	uint cDead = std::count_if(m_vecSweepers.begin(), m_vecSweepers.end(), [](CDiscMinesweeper * s)->bool{return s->isDead();});
	if (cDead == CParams::iNumSweepers){
		printf("All dead ... skipping to next iteration\n");
		m_iTicks = CParams::iNumTicks;

		resetDeadCheck();
	}

	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) continue;
		/**
		Q-learning algorithm according to:
		Watkins, Christopher JCH, and Peter Dayan. "Q-learning." Machine learning 8. 3-4 (1992): 279-292
		*/
		//1:::Observe the current state:
		//TODO
		SVector2D<int> pos = m_vecSweepers[sw]->Position();
		pos.x /= (CParams::iGridCellDim);
		pos.y /= (CParams::iGridCellDim);

		//2:::Select action with highest historic return:
		//TODO
		int max_act = getMaxAct(_Q_sx_sy_a[pos.x][pos.y]);
		m_vecSweepers[sw]->setRotation((ROTATION_DIRECTION)max_act);

		//now call the parents update, so all the sweepers fulfill their chosen action
	}
	
	CDiscController::Update(); //call the parent's class update. Do not delete this.
	
	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		//TODO: just use bool cached from the first loop
		if (m_vecSweepers[sw]->isDead() && deadCheck[sw]){
			continue; 
		}
		else if(m_vecSweepers[sw]->isDead()){
			deadCheck[sw] = true;
		}

		//TODO:compute your indexes.. it may also be necessary to keep track of the previous state
		//3:::Observe new state:
		//TODO
		SVector2D<int> prev_pos = m_vecSweepers[sw]->PrevPosition();
		prev_pos.x /= (CParams::iGridCellDim);
		prev_pos.y /= (CParams::iGridCellDim);

		SVector2D<int> pos = m_vecSweepers[sw]->Position();
		pos.x /= (CParams::iGridCellDim);
		pos.y /= (CParams::iGridCellDim);

		int action = (int)m_vecSweepers[sw]->getRotation();

		//4:::Update _Q_s_a accordingly:
		//TODO
		//_Q_sx_sy_a[sw][prev_pos.x][prev_pos.y][action] = R(pos.x, pos.y, sw)
		//	+ (gamma_discount_rt * (
		//	((1 - lambda_learn_rt) * getMaxActVal(_Q_sx_sy_a[sw][pos.x][pos.y]))
		//	+ (lambda_learn_rt * _Q_sx_sy_a[sw][pos.x][pos.y][action])
		//	)
		//	);

		//_Q_sx_sy_a[prev_pos.x][prev_pos.y][action] += (lambda_learn_rt * (R(pos.x, pos.y, sw) + (gamma_discount_rt * getMaxActVal(_Q_sx_sy_a[pos.x][pos.y])) - getMaxActVal(_Q_sx_sy_a[prev_pos.x][prev_pos.y])));

		_Q_sx_sy_a[prev_pos.x][prev_pos.y][action] += (lambda_learn_rt * (R(pos.x, pos.y, sw) + (gamma_discount_rt * getMaxActVal(_Q_sx_sy_a[pos.x][pos.y])) - _Q_sx_sy_a[prev_pos.x][prev_pos.y][action]));
		//printf("QVAL: %f\n", _Q_sx_sy_a[prev_pos.x][prev_pos.y][action]);
	}

	if (m_iTicks == CParams::iNumTicks){
		resetDeadCheck();
	}

	/*printf("====================================================================================\n");
	for (uint y = 0; y < _grid_size_y; y++){
		for (uint x = 0; x < _grid_size_x; x++){
			printf("%.2f ", getMaxActVal(_Q_sx_sy_a[x][y]));
		}
		printf("\n");
	}
	printf("====================================================================================\n");*/
	return true;
}

CQLearningController::~CQLearningController(void)
{
	//TODO: dealloc stuff here if you need to	
}
