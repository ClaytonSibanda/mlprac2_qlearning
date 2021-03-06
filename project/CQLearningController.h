#pragma once
#include "cdisccontroller.h"
#include "CParams.h"
#include "CDiscCollisionObject.h"
#include <cmath>

#include <vector>

typedef unsigned int uint;
class CQLearningController :
	public CDiscController
{
private:
	uint _grid_size_x;
	uint _grid_size_y;

	//Q-table
	//NOTE: one shared Q table to allow for faster learning and convergence(shared knowledge).
	std::vector<std::vector<std::vector<double> > > _Q_sx_sy_a; //right-up-left-down == 0-1-2-3

	//Q-learning paramaters (chosen experimentally)
	double gamma_discount_rt = 0.8;
	double lambda_learn_rt = 0.1;

	//To allow for updating the Q-table once after the sweeper has died
	std::vector<bool> deadCheck;
	void resetDeadCheck();


public:
	CQLearningController(HWND hwndMain);
	virtual void InitializeLearningAlgorithm(void);
	double R(uint x, uint y, uint sweeper_no);
	virtual bool Update(void);
	virtual ~CQLearningController(void);
};

