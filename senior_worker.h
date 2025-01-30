#pragma once
#include "worker.h"
#include <vector>
#include "junior_worker.h"


class SeniorSoftware : public Worker {
public:
    SeniorSoftware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorSoftware(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // power -up can be purchased
    bool isgambler = false;
    void boostJuniorProductivity(std::vector<JuniorSoftware*>& juniors);
    void applyGamble(std::vector<Worker*>& workers);
    void applyEnergyBoost(Worker& target);
    void applyLeadershipBoost(std::vector<JuniorSoftware*>& juniors);
    void applySelectiveStatBoost(std::vector<JuniorSoftware*>& juniors, const std::string& stat);
    void gamble();
    void callgamble();
};

class SeniorHardware : public Worker {
public:
    SeniorHardware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorHardware(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // power-up
    void boostHardwareSkills(std::vector<JuniorHardware*>& juniors);
    void applyGamble(std::vector<Worker*>& workers);
    void applyEnergyBoost(Worker& target);
    void applyLeadershipBoost(std::vector<JuniorHardware*>& juniors);
    void applySelectiveStatBoost(std::vector<JuniorHardware*>& juniors, const std::string& stat);

private:
    void gamble();
};

class SeniorNetwork : public Worker {
public:
    SeniorNetwork(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorNetwork(int index, bool gender, char* name, short int x = 0, short int y = 0);

    //power_up
    void boostNetworkSkills(std::vector<JuniorNetwork*>& juniors);
    void applyGamble(std::vector<Worker*>& workers);
    void applyEnergyBoost(Worker& target);
    void applyLeadershipBoost(std::vector<JuniorNetwork*>& juniors);
    void applySelectiveStatBoost(std::vector<JuniorNetwork*>& juniors, const std::string& stat);

private:
    void gamble();
};







