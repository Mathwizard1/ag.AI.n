#include "juniorWorker.h"

JuniorSoftware::JuniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber), programmingSkills(50) {
}

JuniorSoftware::JuniorSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y), programmingSkills(50) {
}

void JuniorSoftware::makeObedient() {
    obedient = true;  // add on
    money -= 100;
}

void JuniorSoftware::makeNonObedient() {
    if (obedient == true) {
        obedient = false;  //remove to get some money in need
        money += 50;
    }
}
void JuniorSoftware::incProgSkills(int amt) {
    if (programmingSkills + amt >= 100) {
        int reqd = 100 - programmingSkills;
        programmingSkills = 100;
        money -= 2 * reqd;
        return;
    }

    programmingSkills += amt;
    money -= 2 * amt;
}

JuniorHardware::JuniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber), technicalSkills(50) {
}

JuniorHardware::JuniorHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y), technicalSkills(50) {
}

void JuniorHardware::makeObedient() {
    obedient = true;
    money -= 100;
}

void JuniorHardware::makeNonObedient() {
    if (obedient == true) {
        obedient = false;
        money += 50;
    }
}
void JuniorHardware::incTechSkills(int amt) {
    if (technicalSkills + amt >= 100) {
        int reqd = 100 - technicalSkills;
        technicalSkills = 100;
        money -= 2 * reqd;
        return;
    }

    technicalSkills += amt;
    money -= 2 * amt;
}

JuniorNetwork::JuniorNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber), networkingSkills(50) {
}

JuniorNetwork::JuniorNetwork(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y), networkingSkills(50) {
}

void JuniorNetwork::makeObedient() {
    obedient = true;
    money -= 100;
}

void JuniorNetwork::makeNonObedient() {
    if (obedient == true) {
        obedient = false;
        money += 50;
    }
}

void JuniorNetwork::incNetworkSkills(int amt) {
    if (networkingSkills + amt >= 100) {
        int reqd = 100 - networkingSkills;
        networkingSkills = 100;
        money -= 2 * reqd;
        return;
    }

    networkingSkills += amt;
    money -= 2 * amt;
}
