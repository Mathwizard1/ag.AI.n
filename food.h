#pragma once
#include "gameConst.h"

class Food {
public:
	int energy=0;
	double productivity=0;
	int cost=0;
	int id=-1;
	Food()
	{

	}
	Food(int energy, double productivity, int cost, int id) {
		this->energy = energy;
		this->productivity = productivity;
		this->cost = cost;
		this->id = id;
	}
};