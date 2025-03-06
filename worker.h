#pragma once
#include "gameFunction.h"
#include "food.h"
#include "inventory.h"
#include "gameAST.h"
#include "Pathfinding.h"

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
	std::unordered_map<std::string, int> labelMap;
	std::unordered_map<short, short> bracketMaps;

	Pathfinding pathfinder;
	
	int index;
	int energy = 50;
	int productivity = 80;
	int mood = 80;
	int health = 100;
	int money = 100;
	string jobType = "Worker";

	Inventory inventory;

	enum zone {
		Office,
		Lunch,
		Reception,
		Boss,
	} Zone;


	//ACTIVITIES
	enum activitytype {
		Idle,
		Working,
		Eating,
		Moving,
		Talking,
		Taking,
		Giving,
		Buying,
	}activity=Idle;


	//POSITION AND WORKSPACE
	std::pair<short int, short int> pos;
	std::pair<short int, short int> workspace;
	short int occupiedbench = -1;

	//ASSEMBLER STUFF
	short int activitycounter = 0;
	short int lag = -1;
	int linecounter = 0;
	bool errorFound = false;

	//CODE STUFF
	std::vector<char*> code;
	std::vector<short int> linesize;
	std::vector<std::pair<short int, short int>> path;
	std::vector<std::vector<short int>>* grid = nullptr;

	pair<int, int>workVals = {10, 10};	// Number, Pending

	std::vector<std::string> me = {
		"energy",
		"productivity",
		"mood",
		"health",
		"money"
	};

	std::vector<std::string> zone = {
		"office",
		"lunch",
		"reception",
		"boss"
	};

	char* name;
	bool gender;

	int gridnumber;
	int lastTalkingto = -1;

	// Spec attr.
	int skills = 50;
	bool obedient = false;

	//Constructors
	Worker(short int x , short int y);
	Worker(bool gender, char* name, short int x = 0, short int y = 0);

	//Functions
	void eat();
	void pathfind(pair<short int, short int> end);
	bool give(int index, int fid = 0, int fq = 0, int wid = 0, int wq = 0);
	bool take(int index, int fid = 0, int fq = 0, int wid = 0, int wq = 0);
	int buy(Food food, int q);

	void getCode();
	void tokenizer(char* instruction, short int instructionSize, std::vector<std::string>& tokensList);

	std::pair<gameObjects, int> getExpression(std::string& atr);

	std::pair<gameObjects, int> genericProcess(std::string genericVal);
	std::pair<gameObjects, int> expressionProcess(int lhs, int rhs, std::string Opr);
	int literalProcess(std::string intLiteral);

	void callFunction();

	// Spec functions
	virtual void incSkills(int amt);
	virtual void updateObedience();

	~Worker();
};

extern std::vector<std::vector<Worker>> workers;
extern std::vector<Worker> bosses;
extern std::vector<Worker> receptionists;
