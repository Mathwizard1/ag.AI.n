#include "inventory.h"

std::unordered_map<string, Food> foods = {
	{"Donut",Food(50, 0.5, 10, 0)}, 
	{"Pizza",Food(70, 0.5, 70, 1)},
	{"Coffee",Food(60, 0.6, 50, 2)}
};
std::unordered_map<string, int> worker_types = {
	{"Boss",0},
	{"Software_Engineer",1},
	{"Hardware_Engineer",2},
	{"Worker",3}
};

//Food Functions
bool Inventory::food_into_inventory(int id, int q) {
	if (foodcount + q > 10) return 0;
	else {
		foodinv[id] += q;
		foodcount += q;
		return 1;
	}
}
bool Inventory::food_from_inventory(int id, int q) {
	if (foodinv[id] > q) return 0;
	else {
		foodinv[id] -= q;
		foodcount -= q;
		return 1;
	}
}

//Work Functions
bool Inventory::work_into_inventory(int id, int q) {
	if (workcount + q > 10) return 0;
	else {
		workinv[id] += q;
		workcount += q;
		return 1;
	}
}
bool Inventory::work_from_inventory(int id, int q) {
	if (workinv[id] > q) return 0;
	else {
		workinv[id] -= q;
		workcount -= q;
		return 1;
	}
}