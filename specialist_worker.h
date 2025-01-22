#pragma once
#include "worker.h"

class SpecialistSoftware: public Worker {
public:
    SpecialistSoftware(int index,short int x = 0, short int y = 0, short int gridnumber = 0);
    SpecialistSoftware(int index, bool gender,char* name, short int x = 0, short int y = 0);
};

class SpecialistHardware :public Worker {
public:
    SpecialistHardware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SpecialistHardware(int index,bool gender, char* name, short int x = 0, short int y = 0);
};

class SpecialistNetwork : public Worker {
public:
    SpecialistNetwork(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SpecialistNetwork(int index, bool gender,char* name, short int x = 0, short int y = 0);
};
