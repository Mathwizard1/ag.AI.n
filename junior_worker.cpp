#include "junior_worker.h"

JuniorSoftware::JuniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber), programmingSkills(50) {
}

JuniorSoftware::JuniorSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y), programmingSkills(50) {
}

void JuniorSoftware::makeObedient() {
    obedient = true;  // add on
}

void JuniorSoftware::makeNonObedient() {
    if (obedient == true) {
        obedient = false;  //remove to get some money in need
        money += 50;
    }
}

JuniorHardware::JuniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber), technicalSkills(50) {
}

JuniorHardware::JuniorHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y), technicalSkills(50) {
}

void JuniorHardware::makeObedient() {
    obedient = true;  
}

void JuniorHardware::makeNonObedient() {
    if (obedient == true) {
        obedient = false; \
        money += 50;
    }
}

JuniorNetwork::JuniorNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber), networkingSkills(50) {
}

JuniorNetwork::JuniorNetwork(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y), networkingSkills(50) {
}

void JuniorNetwork::makeObedient() {
    obedient = true; 
}

void JuniorNetwork::makeNonObedient() {
    if (obedient == true) {
        obedient = false; 
        money += 50;
    }
}
