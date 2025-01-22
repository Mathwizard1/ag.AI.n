#include "junior_worker.h"



//SpecialSoftware
JuniorSoftware::JuniorSoftware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber){
}
JuniorSoftware::JuniorSoftware(int index,bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y){
}

//JuniorHardware
JuniorHardware::JuniorHardware(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber){
}

JuniorHardware::JuniorHardware(int index,bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y){
}

// JuniorNetwork 
JuniorNetwork::JuniorNetwork(int index, short int x, short int y, short int gridnumber)
    : Worker(index, x, y, gridnumber){
}

JuniorNetwork::JuniorNetwork(int index,bool gender, char* name, short int x, short int y)
    : Worker(index, gender, name, x, y){
}

