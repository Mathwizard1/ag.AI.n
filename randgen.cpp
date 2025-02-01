#include "randgen.h"

random_device rd;
uniform_int_distribution<int> cent(1, 100);
mt19937 engine(rd());

RandomGenerator::RandomGenerator() {
	this->competitor = {};
    this->momentum = 0;
    this->consistency = 0;
}

double RandomGenerator::randgen() {
    int n = playerstock.size();

    double player_new =(n>=1)? playerstock[n-1]:0;
    double player_val = (n>=2)? playerstock[n - 2]:0;
    double comp_val = (n>=2)?competitor[n - 2]:0;
    double prev_inc = player_val - ((n >= 3) ? playerstock[n - 3] : 0);
    double cur_diff = comp_val - player_val;
    double pl_inc = player_new - player_val;

    //momentum
    if (pl_inc >= prev_inc) {
        momentum++;
    }
    else {
        momentum = max(1, momentum - 1);
    }

    //consistency
    if (pl_inc >= 0) {
        consistency++;
    }
    else {
        consistency = 0;
    }

    //calculation
    double inc = 0.8 * pl_inc;
    inc += (momentum * 4 + consistency * 2) * inc / 100;
    inc += ((int)(cent(engine) / 20) - 2) * inc / 100;
    competitor.push_back(comp_val + inc);

    return competitor.back();
}