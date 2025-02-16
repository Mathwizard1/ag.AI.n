#pragma once
#include "food.h"
#include "gameFunction.h"

extern std::unordered_map<string, Food> foods;
extern std::unordered_map<string, int> worker_types;
extern int foodLimit;

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

    // Availability Check
    bool is_food_available(int id, int quantity);
    bool is_worker_available(int id, int quantity);

    // Find Min/Max
    int find_food_with_lowest_quantity();
    int find_worker_with_lowest_quantity();
    int find_food_with_highest_quantity();
    int find_worker_with_highest_quantity();

    // Search by Range
    vector<int> get_foods_in_quantity_range(int min_qty, int max_qty);
    vector<int> get_workers_in_quantity_range(int min_qty, int max_qty);

    // Utility Functions
    void display_inventory(std::string name);
    void reset_inventory();
};
