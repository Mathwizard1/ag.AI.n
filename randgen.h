#pragma once
#include "gameFunction.h"

extern random_device rd;
extern uniform_int_distribution<int> cent;
extern mt19937 engine;

class RandomGenerator {
public:
	vector<double> player;
	vector<double> competitor;

	int momentum;
	int consistency;

	RandomGenerator();
	double randgen();
};