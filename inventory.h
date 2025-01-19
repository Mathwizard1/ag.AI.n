#pragma once
#include "gameconst.h"
#include "food.h"
#include "gameFunction.h"


class Inventory {
public:

	std::unordered_map<string, Food> foods;
	std::unordered_map<string, int> worker_types;
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

		//Initializing foods
		foods["Donut"] = Food(50, 0.5, 50, 0);
		foods["Pizza"] = Food(70, 0.5, 70, 1);
		foods["Coffee"] = Food(60, 0.6, 50, 2);

		//Initializing workers_types
		worker_types["Boss"] = 0; 
		worker_types["Software_Engineer"] = 1;
		worker_types["Hardware_Engineer"] = 2; 
		worker_types["Worker"] = 3;

	}

	//Food Functions
	bool food_into_inventory(int id, int q) {
		if (foodcount + q > 10) return 0;
		else {
			foodinv[id] += q;
			foodcount += q;
			return 1;
		}
	}
	bool food_from_inventory(int id, int q) {
		if (foodinv[id] > q) return 0;
		else {
			foodinv[id] -= q;
			foodcount -= q;
			return 1;
		}
	}

	//Work Functions
	bool work_into_inventory(int id, int q) {
		if (workcount + q > 10) return 0;
		else {
			workinv[id] += q;
			workcount += q;
			return 1;
		}
	}
	bool work_from_inventory(int id, int q) {
		if (workinv[id] > q) return 0;
		else {
			workinv[id] -= q;
			workcount -= q;
			return 1;
		}
	}
};	
