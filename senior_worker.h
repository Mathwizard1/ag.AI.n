#pragma once
#include "worker.h"

class SeniorSoftware : public Worker {
public:
    SeniorSoftware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorSoftware(int index, bool gender, char* name, short int x = 0, short int y = 0);
};

class SeniorHardware :public Worker {
public:
    SeniorHardware(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorHardware(int index, bool gender, char* name, short int x = 0, short int y = 0);
};

class SeniorNetwork : public Worker {
public:
    SeniorNetwork(int index, short int x = 0, short int y = 0, short int gridnumber = 0);
    SeniorNetwork(int index, bool gender, char* name, short int x = 0, short int y = 0);
};
