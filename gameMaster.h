#pragma once
#include "gameAST.h"

class gameMaster
{
private:
	std::unordered_set<char> delimiter = { ' ', '\n', '\r', '\t' , '(', ')' , '\0' };
	bool nonPairedEntity = false;

	std::unordered_set<char> specialOp = { '=', '>', '<', '!', '$', '{' ,'}' , '.'};

	std::pair<char, char> paranSmooth = { '(', ')' };
	std::pair<char, char> paranFlower = { '{', '}' };

	std::vector<std::string> parsedTokens;

	void tokenizer(const char* instruction, short int instructionSize, std::vector<std::string>& tokensList);

public:
	gameMaster();
	void getCode(std::vector<const char*> instructionList, int listSize, std::vector<short int> instructionSizes);

	void process(int tokenS, int tokenE);
	void processCode();
};

