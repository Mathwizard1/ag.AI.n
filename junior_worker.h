#pragma once
#include "worker.h"

class JuniorSoftware: public Worker {
public:
    JuniorSoftware(int index,short int x =0, short int y = 0, short int gridnumber = 0);
    JuniorSoftware(int index, bool gender, char* name, short int x = 0, short int y = 0);
};

class JuniorHardware :public Worker {
public:
    JuniorHardware(int index,short int x =0, short int y = 0, short int gridnumber = 0);
    JuniorHardware(int index, bool gender, char* name, short int x = 0, short int y = 0);
};

class JuniorNetwork : public Worker {
public:
    JuniorNetwork(int index,short int x =0, short int y = 0, short int gridnumber = 0);
    JuniorNetwork(int index, bool gender, char* name, short int x = 0, short int y = 0);
};
