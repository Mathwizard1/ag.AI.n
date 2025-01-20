#pragma once
#include "gameConst.h"

class Food {
public:
	int energy;
	double productivity;
	int cost;
	int id;
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