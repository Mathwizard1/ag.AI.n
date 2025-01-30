#include "senior_worker.h"
#include <iostream>


SeniorSoftware::SeniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorSoftware::SeniorSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

void SeniorSoftware::boostJuniorProductivity(std::vector<JuniorSoftware*>& juniors) {
    for (auto& junior : juniors) {
        junior->productivity += junior->productivity * 0.1;  //10% for base all numbers will change 
    }
}

void SeniorSoftware::applyGamble(std::vector<Worker*>& workers) {
    isgambler = true;
    gamble();
}

void SeniorSoftware::applyEnergyBoost(Worker& target) {
    target.energy += 20; 
}

void SeniorSoftware::applyLeadershipBoost(std::vector<JuniorSoftware*>& juniors) {
    for (auto& junior : juniors) {
        junior->productivity += 15;  
        junior->mood += 10;  
    }
}

void SeniorSoftware::applySelectiveStatBoost(std::vector<JuniorSoftware*>& juniors, const std::string& stat) {
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            junior->productivity += 10;
        }
    }
}

void SeniorSoftware::gamble() {
    
}

// Base constructor for Senior Hardware Worker
SeniorHardware::SeniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorHardware::SeniorHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

void SeniorHardware::boostHardwareSkills(std::vector<JuniorHardware*>& juniors) {
    for (auto& junior : juniors) {
        junior->productivity += junior->productivity * 0.1;  // Increase by 10%
    }
}

void SeniorHardware::applyGamble(std::vector<Worker*>& workers) {
    std::cout << "Applying Gambler Power: Boosting hardware skills for a random worker!" << std::endl;
    gamble();
}

void SeniorHardware::applyEnergyBoost(Worker& target) {
    target.energy += 20; // Boost energy by 20
}

void SeniorHardware::applyLeadershipBoost(std::vector<JuniorHardware*>& juniors) {
    for (auto& junior : juniors) {
        junior->productivity += 15;  // Boost productivity
        junior->mood += 10;  // Boost mood
    }
}

void SeniorHardware::applySelectiveStatBoost(std::vector<JuniorHardware*>& juniors, const std::string& stat) {
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            junior->productivity += 10;
        }
    }
}

void SeniorHardware::gamble() {
    std::cout << "Gambler is applying a random boost!" << std::endl;
    // Gambling logic to apply a random boost (example: boosting productivity)
    // In real game scenario, this might be more dynamic or random
}



// Base constructor for Senior Network Worker
SeniorNetwork::SeniorNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorNetwork::SeniorNetwork(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

void SeniorNetwork::boostNetworkSkills(std::vector<JuniorNetwork*>& juniors) {
    for (auto& junior : juniors) {
        junior->productivity += junior->productivity * 0.1;  // Increase by 10%
    }
}

void SeniorNetwork::applyGamble(std::vector<Worker*>& workers) {
    std::cout << "Applying Gambler Power: Boosting network skills for a random worker!" << std::endl;
    gamble();
}

void SeniorNetwork::applyEnergyBoost(Worker& target) {
    target.energy += 20; // Boost energy by 20
}

void SeniorNetwork::applyLeadershipBoost(std::vector<JuniorNetwork*>& juniors) {
    for (auto& junior : juniors) {
        junior->productivity += 15;  // Boost productivity
        junior->mood += 10;  // Boost mood
    }
}

void SeniorNetwork::applySelectiveStatBoost(std::vector<JuniorNetwork*>& juniors, const std::string& stat) {
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            junior->productivity += 10;
        }
    }
}

void SeniorNetwork::gamble() {
   //time based 

}
