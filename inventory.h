#pragma once
#include "food.h"
#include "gameFunction.h"

extern std::unordered_map<string, Food> foods;
extern std::unordered_map<string, int> worker_types;

class Inventory {
public:

	int foodcount;
	int workcount;
	vector<int> foodinv;
	vector<int> workinv;

	Inventory() {

		foodcount = 0;
		workcount = 0;

		for (int i = 0; i < foods.size(); i++) {
			foodinv.push_back(0);
		}
		for (int i = 0; i < worker_types.size(); i++) {
			workinv.push_back(0);
		}

	}

	//Food Functions
	bool food_into_inventory(int id, int q);
	bool food_from_inventory(int id, int q);

	//Work Functions
	bool work_into_inventory(int id, int q);
	bool work_from_inventory(int id, int q);
};
