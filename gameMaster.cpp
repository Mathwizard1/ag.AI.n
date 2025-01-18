#include "gameMaster.h"

void gameMaster::tokenPrinter(std::vector<std::string>& tokens)
{
	for (auto &token : tokens)
	{
		std::cout << token << "/";
	}
	std::cout << " size:" << tokens.size() << '\n';
	if (nonPairedEntity)
	{
		std::cout << "Not paired brackets\n";
	}
	std::cout << '\n';
}

void gameMaster::tokenizer(const char* instruction, short int instructionSize, std::vector<std::string>& tokensList)
{
	std::string cleanString = "";
	std::string specialString = "";

	std::string paired_paran = "";
	nonPairedEntity = false;


	for (int i = 0; i < instructionSize; i++)
	{
		char c = instruction[i];

		// delimiter not there
		if(delimiter.find(c) == delimiter.end())
		{
			// not special characters
			if (specialOp.find(c) == specialOp.end())
			{
				// Add the previous specialString first
				if (! specialString.empty())
				{
					tokensList.push_back(specialString);
					specialString.clear();
				}

				cleanString += std::tolower(c);
			}
			else
			{
				// Add the previous cleanString first
				if (! cleanString.empty())
				{
					tokensList.push_back(cleanString);
					cleanString.clear();
				}

				specialString += c;
			}
		}
		// delimiter there
		else
		{
			// only check for '(', ')'
			if (c == paran_smooth.second)
			{
				int count = paired_paran.size();
				if (count > 0 && paired_paran.back() == paran_smooth.first)
				{
					paired_paran.pop_back();
				}
				else if (count == 0)
				{
					nonPairedEntity = true;
				}
			}
			else if(c == paran_smooth.first)
			{
				paired_paran += c;
			}

			if (! specialString.empty())
			{
				tokensList.push_back(specialString);
				specialString.clear();
			}

			if (! cleanString.empty())
			{
				tokensList.push_back(cleanString);
				cleanString.clear();
			}
		}
	}

	if (paired_paran.size() > 0) 
	{ 
		nonPairedEntity = true; 
	}

	// Comparator will be in between
	if (! specialString.empty())
	{
		tokensList.push_back(specialString);
		specialString.clear();
	}

	// others
	if (! cleanString.empty())
	{
		tokensList.push_back(cleanString);
		cleanString.clear();
	}
}

gameMaster::gameMaster()
{

}

void gameMaster::getCode(std::vector<const char*> instructionList, int listSize, std::vector<short int> instructionSizes)
{
	std::vector<std::string> tokens;

	for (int i = 0; i < listSize; i++)
	{

		tokenizer(instructionList[i], instructionSizes[i], tokens);

		tokenPrinter(tokens);
		tokens.clear();
	}
}

void gameMaster::transfer()
{

}