#include "gameMaster.h"

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
		if (delimiter.find(c) == delimiter.end())
		{
			// not special characters
			if (specialOp.find(c) == specialOp.end())
			{
				// Add the previous specialString first
				if (!specialString.empty())
				{
					tokensList.push_back(specialString);
					specialString.clear();
				}

				cleanString += std::tolower(c);
			}
			else
			{
				// Add the previous cleanString first
				if (!cleanString.empty())
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
			if (c == paranSmooth.second)
			{
				int count = paired_paran.size();
				if (count > 0 && paired_paran.back() == paranSmooth.first)
				{
					paired_paran.pop_back();
				}
				else if (count == 0)
				{
					nonPairedEntity = true;
				}
			}
			else if (c == paranSmooth.first)
			{
				paired_paran += c;
			}

			if (!specialString.empty())
			{
				tokensList.push_back(specialString);
				specialString.clear();
			}

			if (!cleanString.empty())
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
	if (!specialString.empty())
	{
		tokensList.push_back(specialString);
		specialString.clear();
	}

	// others
	if (!cleanString.empty())
	{
		tokensList.push_back(cleanString);
		cleanString.clear();
	}
}

void gameMaster::getCode(std::vector<const char*> instructionList, int listSize, std::vector<short int> instructionSizes)
{
	for (int i = 0; i < listSize; i++)
	{
		tokenizer(instructionList[i], instructionSizes[i], parsedTokens);
		if (nonPairedEntity)
		{
			std::cout << "non paired brackets on " << i << '\n';
			nonPairedEntity = false;
		}
	}
}

int gameMaster::process(int tokenS, int tokenE)
{
	gameLexers expectToken = gameLexers::none;

	for (int t = tokenS; t < tokenE; t++)
	{

	}
}

void gameMaster::processCode()
{
	int tokenE = parsedTokens.size();
	process(0, tokenE);
}