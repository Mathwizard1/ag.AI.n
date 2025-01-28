#include "inventory.h"

std::unordered_map<std::string, Food> foods = {
    {"Donut", Food(50, 0.5, 10, 0)},
    {"Pizza", Food(70, 0.5, 70, 1)},
    {"Coffee", Food(60, 0.6, 50, 2)}
};

std::unordered_map<std::string, int> worker_types = {
    {"Boss", 0},
    {"Software_Engineer", 1},
    {"Hardware_Engineer", 2},
    {"Worker", 3}
};

// Food into-from
bool Inventory::food_into_inventory(int id, int q) {
    if (foodcount + q > 10) return false;
    foodinv[id] += q;
    foodcount += q;
    return 1;
}

bool Inventory::food_from_inventory(int id, int q) {
    if (foodinv[id] < q) return false;
    foodinv[id] -= q;
    foodcount -= q;
    return 1;
}

// Work into-from
bool Inventory::work_into_inventory(int id, int q) {
    if (workcount + q > 10) return false;
    workinv[id] += q;
    workcount += q;
    return 1;
}

bool Inventory::work_from_inventory(int id, int q) {
    if (workinv[id] < q) return false;
    workinv[id] -= q;
    workcount -= q;
    return 1;
}

// Utility display-reset
void Inventory::display_inventory() {
    std::cout << "Food Inventory:\n";
    for (auto it = foods.begin(); it != foods.end(); ++it) {
        const std::string& name = it->first;
        const Food& food = it->second;
        std::cout << "- " << name << ": " << foodinv[food.id] << "\n";
    }

    std::cout << "Work Inventory:\n";
    for (auto it = worker_types.begin(); it != worker_types.end(); ++it) {
        const std::string& name = it->first;
        int id = it->second;
        std::cout << "- " << name << ": " << workinv[id] << "\n";
    }
}


void Inventory::reset_inventory() {
    std::fill(foodinv.begin(), foodinv.end(), 0);
    std::fill(workinv.begin(), workinv.end(), 0);
    foodcount = 0;
    workcount = 0;
}
