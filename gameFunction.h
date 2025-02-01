#pragma once
#include "gameConst.h"

using namespace std;

extern short int totalticks;

extern float totalmoney;
extern float moneyincrement;
extern float frametime;

extern short int chosengrid;
extern long long int quota;
extern short int chosenperson;

extern vector<double> playerstock;

extern float minstock;
extern float maxstock;

extern vector<pair<pair<short int, short int>,bool>> lunchpositions;

class gameFunction
{
public:
	pair<bool, char*> NameGenerator();

};

