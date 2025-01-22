#include "specialist_worker.h"



//SpecialSoftware
SpecialistSoftware::SpecialistSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}
SpecialistSoftware::SpecialistSoftware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

//SpecialistHardware
SpecialistHardware::SpecialistHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber){
}

SpecialistHardware::SpecialistHardware(int index, bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}

// SpecialistNetwork 
SpecialistNetwork::SpecialistNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber) {
}

SpecialistNetwork::SpecialistNetwork(int index, bool gender,char* name, short int x, short int y)
    : Worker(index, gender, name, x, y) {
}
