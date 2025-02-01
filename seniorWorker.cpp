#include "seniorWorker.h"
#include <iostream>


SeniorSoftware::SeniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorSoftware::SeniorSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

void SeniorSoftware::juniorStatsBoostByPercentage(std::vector<JuniorSoftware*>& juniors, int percentage, std::string& stat) {
    money -= int((percentage / 100) * 3 / 2);
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->productivity += junior->productivity * percentage / 100;
            junior->productivity = min(100, junior->productivity);
        }
    }
    else if (stat == "energy") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->energy += junior->energy * percentage / 100;
            junior->energy = min(100, junior->energy);
        }
    }
    else if (stat == "mood") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->mood += junior->mood * percentage / 100;
            junior->mood = min(100, junior->mood);
        }
    }
    else if (stat == "health") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->health + junior->health * percentage / 100;
            junior->health = min(100, junior->health);
        }
    }
}

void SeniorSoftware::juniorstatsboostbyval(std::vector<JuniorSoftware*>& juniors, const std::string& stat, int val) {
    money -= val * 2;
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->productivity += val;
            junior->productivity = min(100, junior->productivity);
        }
    }
    else if (stat == "energy") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->energy += val;
            junior->energy = min(100, junior->energy);
        }
    }
    else if (stat == "mood") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->mood += val;
            junior->mood = min(100, junior->mood);
        }
    }
    else if (stat == "health") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->health += val;
            junior->health = min(100, junior->health);
        }
    }
}
//target --> curr senior worker
void SeniorSoftware::applySelfStatBoost(Worker& target, const std::string& stat, int val) {
    money -= val * 3;
    if (stat == "productivity") {
        target.productivity += val;
        target.productivity = max(target.productivity, 100);
    }
    else if (stat == "energy") {
        target.energy += val;
        target.energy = max(target.energy, 100);
    }
    else if (stat == "mood") {
        target.mood += val;
        target.mood = max(target.mood, 100);
    }
    else if (stat == "health") {
        target.health += val;
        target.health = max(target.health, 100);
    }

}

void SeniorSoftware::applyLeadershipBoost(std::vector<JuniorSoftware*>& juniors) {
    money -= 50;
    for (auto& junior : juniors) {
        if (junior->obedient)
            junior->productivity = max(100, junior->productivity + int((junior->productivity) * 0.2));
        junior->energy = max(100, junior->energy + int((junior->energy) * 0.2));
        junior->mood = max(100, junior->mood + int((junior->mood) * 0.2));
    }
}
//target --> curr senior worker
void SeniorSoftware::medassistance(JuniorSoftware* junior, Worker& target) {
    target.health -= 10;
    junior->health = 100;
}

void SeniorSoftware::gamble() {
    //need to implement//getting some errors
}
void SeniorSoftware::applyGamble(std::vector<Worker*>& workers) {
    isgambler = true;
    money -= 50;
    gamble();
}
void SeniorSoftware::callgamble() {
    //need to implement
}





SeniorHardware::SeniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorHardware::SeniorHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

void SeniorHardware::juniorStatsBoostByPercentage(std::vector<JuniorHardware*>& juniors, int percentage, std::string& stat) {
    money -= int((percentage / 100) * 3 / 2);
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->productivity += junior->productivity * percentage / 100;
            junior->productivity = min(100, junior->productivity);
        }
    }
    else if (stat == "energy") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->energy += junior->energy * percentage / 100;
            junior->energy = min(100, junior->energy);
        }
    }
    else if (stat == "mood") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->mood += junior->mood * percentage / 100;
            junior->mood = min(100, junior->mood);
        }
    }
    else if (stat == "health") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->health + junior->health * percentage / 100;
            junior->health = min(100, junior->health);
        }
    }
}

void SeniorHardware::juniorstatsboostbyval(std::vector<JuniorHardware*>& juniors, const std::string& stat, int val) {
    money -= val * 2;
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->productivity += val;
            junior->productivity = min(100, junior->productivity);
        }
    }
    else if (stat == "energy") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->energy += val;
            junior->energy = min(100, junior->energy);
        }
    }
    else if (stat == "mood") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->mood += val;
            junior->mood = min(100, junior->mood);
        }
    }
    else if (stat == "health") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->health += val;
            junior->health = min(100, junior->health);
        }
    }
}
//target --> curr senior worker
void SeniorHardware::applySelfStatBoost(Worker& target, const std::string& stat, int val) {
    money -= val * 3;
    if (stat == "productivity") {
        target.productivity += val;
        target.productivity = max(target.productivity, 100);
    }
    else if (stat == "energy") {
        target.energy += val;
        target.energy = max(target.energy, 100);
    }
    else if (stat == "mood") {
        target.mood += val;
        target.mood = max(target.mood, 100);
    }
    else if (stat == "health") {
        target.health += val;
        target.health = max(target.health, 100);
    }

}

void SeniorHardware::applyLeadershipBoost(std::vector<JuniorHardware*>& juniors) {
    money -= 50;
    for (auto& junior : juniors) {
        if (junior->obedient)
            junior->productivity = max(100, junior->productivity + int((junior->productivity) * 0.2));
        junior->energy = max(100, junior->energy + int((junior->energy) * 0.2));
        junior->mood = max(100, junior->mood + int((junior->mood) * 0.2));
    }
}
//target --> curr senior worker
void SeniorHardware::medassistance(JuniorHardware* junior, Worker& target) {
    target.health -= 10;
    junior->health = 100;
}

void SeniorHardware::gamble() {
    //need to implement//getting some errors
}
void SeniorHardware::applyGamble(std::vector<Worker*>& workers) {
    isgambler = true;
    money -= 50;
    gamble();
}
void SeniorHardware::callgamble() {
    //need to implement
}







SeniorNetwork::SeniorNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorNetwork::SeniorNetwork(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

void SeniorNetwork::juniorStatsBoostByPercentage(std::vector<JuniorNetwork*>& juniors, int percentage, std::string& stat) {
    money -= int((percentage / 100) * 3 / 2);
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->productivity += junior->productivity * percentage / 100;
            junior->productivity = min(100, junior->productivity);
        }
    }
    else if (stat == "energy") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->energy += junior->energy * percentage / 100;
            junior->energy = min(100, junior->energy);
        }
    }
    else if (stat == "mood") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->mood += junior->mood * percentage / 100;
            junior->mood = min(100, junior->mood);
        }
    }
    else if (stat == "health") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->health + junior->health * percentage / 100;
            junior->health = min(100, junior->health);
        }
    }
}

void SeniorNetwork::juniorstatsboostbyval(std::vector<JuniorNetwork*>& juniors, const std::string& stat, int val) {
    money -= val * 2;
    if (stat == "productivity") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->productivity += val;
            junior->productivity = min(100, junior->productivity);
        }
    }
    else if (stat == "energy") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->energy += val;
            junior->energy = min(100, junior->energy);
        }
    }
    else if (stat == "mood") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->mood += val;
            junior->mood = min(100, junior->mood);
        }
    }
    else if (stat == "health") {
        for (auto& junior : juniors) {
            if (junior->obedient)
                junior->health += val;
            junior->health = min(100, junior->health);
        }
    }
}
//target --> curr senior worker
void SeniorNetwork::applySelfStatBoost(Worker& target, const std::string& stat, int val) {
    money -= val * 3;
    if (stat == "productivity") {
        target.productivity += val;
        target.productivity = max(target.productivity, 100);
    }
    else if (stat == "energy") {
        target.energy += val;
        target.energy = max(target.energy, 100);
    }
    else if (stat == "mood") {
        target.mood += val;
        target.mood = max(target.mood, 100);
    }
    else if (stat == "health") {
        target.health += val;
        target.health = max(target.health, 100);
    }

}

void SeniorNetwork::applyLeadershipBoost(std::vector<JuniorNetwork*>& juniors) {
    money -= 50;
    for (auto& junior : juniors) {
        if (junior->obedient)
            junior->productivity = max(100, junior->productivity + int((junior->productivity) * 0.2));
        junior->energy = max(100, junior->energy + int((junior->energy) * 0.2));
        junior->mood = max(100, junior->mood + int((junior->mood) * 0.2));
    }
}
//target --> curr senior worker
void SeniorNetwork::medassistance(JuniorNetwork* junior, Worker& target) {
    target.health -= 10;
    junior->health = 100;
}

void SeniorNetwork::gamble() {
    //need to implement//getting some errors
}
void SeniorNetwork::applyGamble(std::vector<Worker*>& workers) {
    isgambler = true;
    money -= 50;
    gamble();
}
void SeniorNetwork::callgamble() {
    //need to implement
}