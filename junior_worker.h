#pragma once
#include <string>
#include "worker.h"  

class JuniorSoftware : public Worker {
public:
    JuniorSoftware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    JuniorSoftware(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // Spec attr.
    int programmingSkills;
    bool obedient = false;

    // extra addons to buy
    void makeObedient();
    void makeNonObedient();
    void incProgSkills(int amt);
};

class JuniorHardware : public Worker {
public:
    JuniorHardware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    JuniorHardware(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // Specific attri.
    int technicalSkills;
    bool obedient = false;
    // add ons
    void makeObedient();
    void makeNonObedient();
    void incTechSkills(int amt);
};

class JuniorNetwork : public Worker {
public:
    JuniorNetwork(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    JuniorNetwork(int index, bool gender, char* name, short int x = 0, short int y = 0);

    // spec attr.
    int networkingSkills;
    bool obedient = false;
    // add ons
    void makeObedient();
    void makeNonObedient();
    void incNetworkSkills(int amt);
};
