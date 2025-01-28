#pragma once
#include "gameConst.h"

using namespace std;

extern float totalmoney;
extern float moneyincrement;
extern float frametime;

extern short int chosengrid;
extern long long int quota;
extern short int chosenperson;

class gameFunction
{
public:
	pair<bool, char*> NameGenerator();

};

