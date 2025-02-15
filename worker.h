#pragma once
#include "gameFunction.h"
#include "food.h"
#include "inventory.h"
#include "gameAST.h"

//#include "gameMaster.h"

struct Node {
	short int x, y;
	float fcost;
	vector<pair<short int, short int>> path;
};
struct NodeComparison {
	bool operator()(Node const& node1, Node const& node2)
	{
		return (node1.fcost > node2.fcost);
	}
};

class Worker
{
protected:
	// gameMaster gM;
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
	//std::unordered_set<int, int> bracketMaps;

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
	short int activitycounter = 0;


	//POSITION AND WORKSPACE
	pair<short int, short int> pos;
	pair<short int, short int> workspace;
	short int occupiedbench = -1;

	//ASSEMBLER STUFF
	short int lag = -1;
	int linecounter = 0;

	//CODE STUFF
	vector<char*> code;
	vector<short int> linesize;
	vector<pair<short int, short int>> path;
	vector<vector<short int>>* grid = nullptr;

	pair<int, int>workVals = {10, 10};

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
	vector<pair<short int, short int>> getneighbors(pair<short int, short int> node);
	bool give(int index, int fid, int fq, int wid, int wq);
	bool take(int index, int fid, int fq, int wid, int wq);
	bool give(int index, int fid = 0, int fq = 0, int wid = 0, int wq = 0);
	bool take(int index, int fid = 0, int fq = 0, int wid = 0, int wq = 0);
	int buy(Food food, int q);

	void getCode();
	void tokenizer(char* instruction, short int instructionSize, std::vector<std::string>& tokensList);

	int getExpression(const std::string& var, const std::string& atr);

	int genericProcess(std::string genericVal);
	int expressionProcess(int lhs, int rhs, std::string Opr);
	int literalProcess(std::string intLiteral);

	void callFunction();

	// Spec functions
	virtual void incSkills(int amt);
	virtual void updateObedience();
};

extern std::vector<std::vector<Worker>> workers;
extern std::vector<Worker> bosses;
extern std::vector<Worker> receptionists;
