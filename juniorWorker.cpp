#include "juniorWorker.h"

JuniorSoftware::JuniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber){
    this->skills = 50;
}

JuniorSoftware::JuniorSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y){
    this->skills = 50;
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
void JuniorSoftware::incSkills(int amt)
{
    if (skills + amt >= 100) {
        int reqd = 100 - skills;
        skills = 100;
        money -= 2 * reqd;
        return;
    }

    skills += amt;
    money -= 2 * amt;
}

JuniorHardware::JuniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber){
    this->skills = 50;
}

JuniorHardware::JuniorHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y){
    this->skills = 50;
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
    : Worker(index, x, y, gridnumber){
    this->skills = 50;
}

JuniorNetwork::JuniorNetwork(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y){
    this->skills = 50;
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
