#pragma once
#include <vector>
#include <iostream>
#include "gameconst.h"
#include "food.h"
#include "gameFunction.h"

using namespace std;

class Inventory {
public:
    struct Item {
        Food food;
        int quantity;
    };

    vector<Item> items;

    Inventory() {}

    void addItem(Food food, int quantity) {
        for (auto& item : items) {
            if (item.food.foodstuff == food.foodstuff) {
                item.quantity += quantity;
                return;
            }
        }
        items.push_back({ food, quantity });
    }

    bool removeItem(Food food, int quantity) {
        for (auto& item : items) {
            if (item.food.foodstuff == food.foodstuff) {
                if (item.quantity >= quantity) {
                    item.quantity -= quantity;
                    if (item.quantity == 0) {
                        items.erase(remove(items.begin(), items.end(), item), items.end());
                    }
                    return true;
                }
                return false;
            }
        }
        return false;
    }

    double getCost(Food::Foodstuff type) const {
        for (const auto& item : items) {
            if (item.food.foodstuff == type) {
                return item.food.cost;
            }
        }
        return -1;
    }

    double getProductivity(Food::Foodstuff type) const {
        for (const auto& item : items) {
            if (item.food.foodstuff == type) {
                return item.food.productivity;
            }
        }
        return -1;
    }

    void displayInventory() const {
        for (const auto& item : items) {
            cout << "Food: " << foodName(item.food.foodstuff)
                << ", Quantity: " << item.quantity
                << ", Cost: " << item.food.cost
                << ", Productivity: " << item.food.productivity
                << endl;
        }
    }

private:
    string foodName(Food::Foodstuff type) const {
        switch (type) {
        case Food::Foodstuff::Donut: return "Donut";
        case Food::Foodstuff::Coffee: return "Coffee";
        case Food::Foodstuff::Pizza: return "Pizza";
        default: return "Unknown";
        }
    }
};
