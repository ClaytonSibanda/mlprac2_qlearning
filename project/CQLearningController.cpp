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

	//initialize the Q-tables with all 0's
	for (uint sweeper = 0; sweeper < CParams::iNumSweepers; sweeper++){
		std::vector<std::vector<std::vector<double > > > q_table;
			for (uint x = 0; x < _grid_size_x; x++){
				std::vector<std::vector<double> > state_y;
				for (uint y = 0; y < _grid_size_y; y++){
					state_y.push_back({500.0, 500.0, 500.0, 500.0});
				}

				q_table.push_back(state_y);
			}

			_Q_sx_sy_a.push_back(q_table);
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

	double reward = 500.0f; //neutral reward

	//see what the minesweeper hit
	int hit_index = ((m_vecSweepers[sweeper_no])->CheckForObject(m_vecObjects, CParams::dMineScale));
	if (hit_index >= 0)
	{
		switch (m_vecObjects[hit_index]->getType()){
			case CDiscCollisionObject::Mine:{
				reward = 1000.0;
				break;
			}
			case CDiscCollisionObject::Rock:{
				reward = 0.0;
				break;
			}
			case CDiscCollisionObject::SuperMine:{
				reward = 0.0;
				break;
			}
		}
	}
	
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

	return max_act;
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

		//2:::Select action with highest historic return:
		//TODO
		int max_act = getMaxAct(_Q_sx_sy_a[sw][pos.x / (CParams::iGridCellDim + 1)][pos.y / (CParams::iGridCellDim + 1)]);
		m_vecSweepers[sw]->setRotation((ROTATION_DIRECTION)max_act);

		//now call the parents update, so all the sweepers fulfill their chosen action
	}
	
	CDiscController::Update(); //call the parent's class update. Do not delete this.
	
	for (uint sw = 0; sw < CParams::iNumSweepers; ++sw){
		if (m_vecSweepers[sw]->isDead()) continue;
		//TODO:compute your indexes.. it may also be necessary to keep track of the previous state
		//3:::Observe new state:
		//TODO
		//4:::Update _Q_s_a accordingly:
		//TODO
	}
	return true;
}

CQLearningController::~CQLearningController(void)
{
	//TODO: dealloc stuff here if you need to	
}
