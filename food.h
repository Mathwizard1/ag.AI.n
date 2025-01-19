#pragma once

class Food {
public:
    double energy;
    double productivity;
    double cost;

    enum class Foodstuff {
        Donut,
        Coffee,
        Pizza,
    } foodstuff;
};
