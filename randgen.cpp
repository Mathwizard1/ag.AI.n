#include "randgen.h"

random_device rd;
uniform_int_distribution<int> cent(1, 100);
mt19937 engine(rd());

RandomGenerator::RandomGenerator() {
	this->competitor = {};
    this->momentum = 0;
    this->phi = 0.25 + 0.25 * static_cast<double>(std::rand()) / RAND_MAX;
}

double RandomGenerator::randgen() {
    int n = playerstock.size();
    double comp_val_f = 0.0;
    double r = static_cast<double>(std::rand()) / RAND_MAX;

    if (n >= 2) {
        double player_dt = 1.5 * (playerstock[n - 1] - playerstock[n - 2]);
        double delta_factor = 0.0;

        // Update momentum:
        momentum += phi * r * (2.0 * (r > 0.5) - 1.0);
        if (std::abs(momentum) > 2) {
            momentum *= 0.8; // Reduce momentum if it gets too high
        }

        if (r < 0.4) { // 40% chance
            delta_factor = (2.5 * player_dt) * r / 0.4;
        }
        else if (r < 0.6) { // 20% chance
            delta_factor = 1.25 * player_dt * (r - 0.4) / 0.2;
        }
        else if (r < 0.75) { // 15% chance
            delta_factor = -1.5 * player_dt * (r - 0.6) / 0.15;
        }
        else { // 25% chance
            delta_factor = -1.75 * player_dt * (r - 0.75) / 0.25;
        }

        delta_factor += momentum * 0.4 * player_dt;

        comp_val_f = competitor.back() + delta_factor;
    }
    else if (n == 1) 
    {
        if (r < 0.5) {
            comp_val_f = playerstock[0] * (1 + phi); // Increase
        }
        else {
            comp_val_f = playerstock[0] * (1 - phi); // Decrease
        }
    }

    competitor.push_back(comp_val_f);
    return competitor.back();
}