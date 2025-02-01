#include "randgen.h"

random_device rd;
uniform_int_distribution<int> cent(1, 100);
mt19937 engine(rd());

RandomGenerator::RandomGenerator() {
	this->competitor = {};
    this->momentum = 0;
    this->phi = (double)std::rand() / (double)RAND_MAX;
}

double RandomGenerator::randgen() {
    int n = playerstock.size();

    double r = (double)std::rand() / (double)RAND_MAX;
    double comp_val_f = 0.0;

    if(n >= 2)
    {
        double player_t = playerstock[n - 1];
        double player_dt = playerstock[n - 1] - playerstock[n - 2];

        momentum = momentum * r + player_dt / (player_t + 1.0e-6) * phi + (0.5 - r);

        comp_val_f = competitor[n - 1] + player_dt * momentum;
    }
    else if (n == 1)
    {
        if (r > 0.5)
        {
            momentum = phi;
        }
        else
        {
            momentum = -phi;
        }

        comp_val_f = playerstock[n - 1] + playerstock[n - 1] / 5 * momentum;
    }
    
    competitor.push_back(comp_val_f);
    return competitor.back();
}