#pragma once
#include "gameconst.h"
#include "food.h"
#include "gameFunction.h"


class Worker
{
public:

	double energy;
	double productivity;
	double mood;
	double health;
	double money;
	pair<short int, short int> end_point;
	pair<short int, short int> pos;

	enum zone {
		Office,
		Lunch,
		Reception,
		Work,
		Boss,
	} Zone;



	vector<char*> code;
	vector<short int> linesize;
	vector<pair<short int, short int>> path;

	char* name;

	bool gender;

	//Constructors
	Worker(short int x = 0, short int y = 0) {
		this->energy = 50;
		this->productivity = 80;
		this->health = 100;
		this->Zone = Office;
		pair<bool, char*> p = gameFunction().NameGenerator();
		this->name = p.second;
		this->gender = p.first;
		this->money = 0;
		this->mood = (3 * energy + 5 * health) / 8;
		this->pos = { x,y };
	}
	Worker(bool gender, char* name, short int x=0, short int y=0) {
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



	//Functions
	void eat(Food food);
	void pathfind();//marked
	void give();
	void take();
};
