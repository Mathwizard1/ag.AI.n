#include "senior_worker.h"



//SpecialSoftware
SeniorSoftware::SeniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}
SeniorSoftware::SeniorSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

//SeniorHardware
SeniorHardware::SeniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorHardware::SeniorHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

// SeniorNetwork 
SeniorNetwork::SeniorNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SeniorNetwork::SeniorNetwork(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}
