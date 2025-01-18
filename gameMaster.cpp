#include "gameMaster.h"

#include <ctype.h>

void gameMaster::tokenPrinter(std::vector<std::string>& tokens)
{
	for (auto &token : tokens)
	{
		std::cout << token << "|";
	}
	std::cout << '\n';
	std::cout << tokens.size() << '\n';
}

void gameMaster::tokenizer(std::string& instruction, std::vector<std::string>& tokensList)
{
	std::string cleanInstruction = "";
	for (char &c : instruction)
	{
		if (c != delimiter)
		{
			if (invalidChar.find(c) == invalidChar.end())
			{
				cleanInstruction += tolower(c);
			}
		}
		else
		{
			std::cout << "\n";
			if (! cleanInstruction.empty())
			{
				tokensList.push_back(cleanInstruction);
				cleanInstruction = "";
			}
		}
	}

	if (! cleanInstruction.empty())
	{
		tokensList.push_back(cleanInstruction);
	}
}

gameMaster::gameMaster()
{

}

void gameMaster::getCode(std::vector<std::string> InstructionList)
{
	for (auto &instruction : InstructionList)
	{
		std::vector<std::string> tokens;

		tokenizer(instruction, tokens);
		tokenPrinter(tokens);

	}
}

void gameMaster::transfer()
{

}