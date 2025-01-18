#pragma once
#include "gameConst.h"
#include "food.h"
using namespace std;
class Worker
{
public:
	double energy;
	double productivity;
	double mood;
	double health;
	double money;

	enum zone {
		Office,
		Lunch,
		Reception,
		Work,
		Boss,
	} Zone;

	pair<short int, short int> pos;

	string name;

	bool gender;



	Worker(bool gender, string name, short int x=0, short int y=0) {
		this->energy = 50;
		this->productivity = 80;
		this->health = 100;
		this->Zone = Office;	
		this->gender = gender;
		this->name = name;
		this->money = 0;
		this->mood = (3 * energy + 5 * health) / 8;
		this->pos = { x,y };
	}
	void eat(Food food);
	void pathfind();
	void give();
	void take();
};
