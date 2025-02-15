#include "inventory.h"

std::unordered_map<std::string, Food> foods = {
    {"Donut", Food(50, 0.5, 10, 0)},
    {"Pizza", Food(70, 0.5, 70, 1)},
    {"Coffee", Food(60, 0.6, 50, 2)}
};

int foodLimit = foods.size();

std::unordered_map<std::string, int> worker_types = {
    {"Worker", 0},
    {"Boss", 1},
    {"Software_Engineer", 2},
    {"Hardware_Engineer", 3},
    {"Network_Engineer", 4},
    {"Receptionist", 5},
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
// Display ka dekh lena , if you want to change the display format
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

// Transfer Functions
bool Inventory::transfer_food(Inventory& other, int id, int quantity) {
    if (food_from_inventory(id, quantity)) {
        return other.food_into_inventory(id, quantity);
    }
    return false; // not sufficient
}

bool Inventory::transfer_worker(Inventory& other, int id, int quantity) {
    if (work_from_inventory(id, quantity)) {
        return other.work_into_inventory(id, quantity);
    }
    return false;
}

// Availability Check Functions
bool Inventory::is_food_available(int id, int quantity) {
    return (id >= 0 && id < foodinv.size() && foodinv[id] >= quantity);
}

bool Inventory::is_worker_available(int id, int quantity) {
    return (id >= 0 && id < workinv.size() && workinv[id] >= quantity);
}

// Finding workers or foods with extreme quantities(max/min) 
int Inventory::find_food_with_lowest_quantity() {
    int min_id = -1, min_qty = INT_MAX;
    for (int i = 0; i < foodinv.size(); i++) {
        if (foodinv[i] < min_qty) {
            min_qty = foodinv[i];
            min_id = i;
        }
    }
    return min_id;
}

int Inventory::find_worker_with_lowest_quantity() {
    int min_id = -1, min_qty = INT_MAX;
    for (int i = 0; i < workinv.size(); i++) {
        if (workinv[i] < min_qty) {
            min_qty = workinv[i];
            min_id = i;
        }
    }
    return min_id;
}

int Inventory::find_food_with_highest_quantity() {
    int max_id = -1, max_qty = 0;
    for (int i = 0; i < foodinv.size(); i++) {
        if (foodinv[i] > max_qty) {
            max_qty = foodinv[i];
            max_id = i;
        }
    }
    return max_id;
}

int Inventory::find_worker_with_highest_quantity() {
    int max_id = -1, max_qty = 0;
    for (int i = 0; i < workinv.size(); i++) {
        if (workinv[i] > max_qty) {
            max_qty = workinv[i];
            max_id = i;
        }
    }
    return max_id;
}

// Range searching Functions
vector<int> Inventory::get_foods_in_quantity_range(int min_qty, int max_qty) {
    vector<int> result;
    for (int i = 0; i < foodinv.size(); i++) {
        if (foodinv[i] >= min_qty && foodinv[i] <= max_qty) {
            result.push_back(i);
        }
    }
    return result;
}

vector<int> Inventory::get_workers_in_quantity_range(int min_qty, int max_qty) {
    vector<int> result;
    for (int i = 0; i < workinv.size(); i++) {
        if (workinv[i] >= min_qty && workinv[i] <= max_qty) {
            result.push_back(i);
        }
    }
    return result;
}
