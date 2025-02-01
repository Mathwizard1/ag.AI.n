#pragma once
#include "worker.h"
#include <vector>
#include "juniorWorker.h"


class SeniorSoftware : public Worker {
public:
    SeniorSoftware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorSoftware(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // power -up can be purchased
    bool isgambler = false;
    void juniorStatsBoostByPercentage(std::vector<JuniorSoftware*>& juniors, int percentage, std::string& stat);
    void juniorstatsboostbyval(std::vector<JuniorSoftware*>& juniors, const std::string& stat, int val);
    void applySelfStatBoost(Worker& target, const std::string& stat, int val);
    void applyLeadershipBoost(std::vector<JuniorSoftware*>& juniors);
    void medassistance(JuniorSoftware* junior, Worker& target);
    void gamble();
    void applyGamble(std::vector<Worker*>& workers);
    void callgamble();
};

class SeniorHardware : public Worker {
public:
    SeniorHardware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorHardware(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // power -up can be purchased
    bool isgambler = false;
    void juniorStatsBoostByPercentage(std::vector<JuniorHardware*>& juniors, int percentage, std::string& stat);
    void juniorstatsboostbyval(std::vector<JuniorHardware*>& juniors, const std::string& stat, int val);
    void applySelfStatBoost(Worker& target, const std::string& stat, int val);
    void applyLeadershipBoost(std::vector<JuniorHardware*>& juniors);
    void medassistance(JuniorHardware* junior, Worker& target);
    void gamble();
    void applyGamble(std::vector<Worker*>& workers);
    void callgamble();
};

class SeniorNetwork : public Worker {
public:
    SeniorNetwork(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorNetwork(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // power -up can be purchased
    bool isgambler = false;
    void juniorStatsBoostByPercentage(std::vector<JuniorNetwork*>& juniors, int percentage, std::string& stat);
    void juniorstatsboostbyval(std::vector<JuniorNetwork*>& juniors, const std::string& stat, int val);
    void applySelfStatBoost(Worker& target, const std::string& stat, int val);
    void applyLeadershipBoost(std::vector<JuniorNetwork*>& juniors);
    void medassistance(JuniorNetwork* junior, Worker& target);
    void gamble();
    void applyGamble(std::vector<Worker*>& workers);
    void callgamble();
};






