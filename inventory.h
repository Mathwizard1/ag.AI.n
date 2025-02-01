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

    // Food Functions
    bool food_into_inventory(int id, int quantity);
    bool food_from_inventory(int id, int quantity);

    // Work Functions
    bool work_into_inventory(int id, int quantity);
    bool work_from_inventory(int id, int quantity);


    // Transfer Operations
    bool transfer_food(Inventory& other, int id, int quantity);
    bool transfer_worker(Inventory& other, int id, int quantity);

    // Utility Functions
    void display_inventory();
    void reset_inventory();
};
