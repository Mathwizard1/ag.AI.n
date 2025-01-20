#pragma once
#include "gameAST.h"

class gameMaster
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
	std::vector<std::string> parsedTokens;

	void tokenizer(const char* instruction, short int instructionSize, std::vector<std::string>& tokensList);

	void getCode(std::vector<const char*> instructionList, int listSize, std::vector<short int> instructionSizes);

	//int getExpression(const std::string& var, const std::string& atr);

	int genericProcess(std::string genericVal);
	int expressionProcess(int lhs, int rhs, std::string Opr);
	int literalProcess(std::string intLiteral);
};

