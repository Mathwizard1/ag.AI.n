#pragma once
#include "gameFunction.h"
#include "food.h"
#include "inventory.h"

#include "gameAST.h"

class Worker
{
protected:
	std::string genericIdentifier = "$";
	std::string labelIdentifier = "!";

	std::pair<std::string, std::string> paranFlower = { "{", "}" };

	std::unordered_set<char> delimiter = { ' ', '\n', '\r', '\t' , '(', ')' , '\0' };
	bool nonPairedEntity = false;

	std::unordered_set<char> specialOp = { '=', '>', '<', '!', '$', '{' ,'}' };
	char genericAttribute = '.';

	std::pair<char, char> paranSmooth = { '(', ')' };

public:

	int energy;
	int productivity;
	int mood;
	int health;
	int money;

	Inventory inventory;

	enum zone {
		Office,
		Lunch,
		Reception,
		Boss,
	} Zone;

	pair<short int, short int> pos;

	short int linecounter = 0;
	vector<char*> code;
	vector<short int> linesize;
	vector<pair<short int, short int>> path;

	int workProp[2] = {10, 0};

	std::vector<std::string> me = {
		"energy",
		"productivity",
		"mood",
		"health",
		"money"
	};

	std::vector<std::string> work = {
		"number",
		"pending",
	};

	std::vector<std::string> zone = {
		"office",
		"lunch",
		"reception",
		"boss"
	};

	char* name;

	bool gender;

	//Constructors
	Worker(short int x = 0, short int y = 0);
	Worker(bool gender, char* name, short int x = 0, short int y = 0);

	//Functions
	void eat();
	void pathfind( pair<short int, short int> end);
	void give();
	void take();
	int buy(Food food, int q);

	void tokenizer(const char* instruction, short int instructionSize, std::vector<std::string>& tokensList);

	int getExpression(const std::string& var, const std::string& atr);

	int genericProcess(std::string genericVal);
	int expressionProcess(int lhs, int rhs, std::string Opr);
	int literalProcess(std::string intLiteral);

	void callFunction();
};

extern std::vector<Worker> workers;
extern std::vector<Worker> bosses;
extern std::vector<Worker> receptionists;