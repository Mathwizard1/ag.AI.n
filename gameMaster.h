#pragma once
#include "gameFunction.h"
#include <unordered_set>


class gameMaster
{
private:
	void tokenPrinter(std::vector<std::string>& tokens);
    std::unordered_set<char> delimiter = {' ', '\n', '\r', '\t' , '(', ')' , '\0'};
    bool nonPairedEntity = false;

    std::unordered_set<char> specialOp = {'=', '>', '<', '!', '$', '{' ,'}' };

    std::pair<char, char> paran_smooth = { '(', ')' };
    std::pair<char, char> paran_flower = { '{', '}' };

	void tokenizer(const char* instruction, short int instructionSize, std::vector<std::string>& tokensList);

public:
	gameMaster();
	void getCode(std::vector<const char*> instructionList, int listSize, std::vector<short int> instructionSizes);

	void transfer();
};

