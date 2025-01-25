#include "gameMaster.h"
#include <stdexcept>

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

int gameMaster::getCode(std::vector<char*>& instructionList, int listSize, std::vector<short int>& instructionSizes)
{
	if (listSize > 0)
	{
		int errorCodes = 0;

		std::vector<std::string> parsedTokens;
		std::vector<int> TokenCount_lineWise;

		TokenCount_lineWise.resize(listSize);

		for (int i = 0; i < listSize; i++)
		{
			tokenizer(instructionList[i], instructionSizes[i], parsedTokens);
			TokenCount_lineWise[i] = parsedTokens.size();

			if (nonPairedEntity)
			{
				errorCodes += 1;
				nonPairedEntity = false;
			}
		}

		int tcount = TokenCount_lineWise[listSize - 1];
		for (int t = 0; t < tcount; t++)
		{
			std::cout << parsedTokens[t] << " : " << t;
			std::cout << '\n';
		}

		return errorCodes;
	}

	return 0;
}

int gameMaster::genericProcess(std::string genericVal)
{
	std::string value = "";
	std::string attribute = "";

	bool attributePresent = false;

	for (int c = 0; c < genericVal.size(); c++)
	{
		if (!attributePresent && genericVal[c] == genericAttribute)
		{
			attributePresent = true;
		}
		else if (attributePresent)
		{
			attribute += genericVal[c];
		}
		else
		{
			value += genericVal[c];
		}
	}



	
	return -1;
}

int gameMaster::expressionProcess(int lhs, int rhs, std::string Opr)
{
	if (lhs == -1 || rhs == -1)
	{
		return -1;
	}

	if (Opr == "==")
	{
		return (lhs == rhs);
	}
	else if (Opr == "!=")
	{
		return (lhs != rhs);
	}
	else if (Opr == "<=")
	{
		return (lhs <= rhs);
	}
	else if (Opr == ">=")
	{
		return (lhs >= rhs);
	}
	else if (Opr == "<")
	{
		return (lhs < rhs);
	}
	else if (Opr == ">")
	{
		return (lhs > rhs);
	}

	return -1;
}

int gameMaster::literalProcess(std::string intLiteral)
{
	try {
		size_t pos;
		int num = std::stoi(intLiteral, &pos);  // Convert string to int

		// Check if there are any leftover characters after the integer
		if (pos != intLiteral.length()) {
			throw std::invalid_argument("Extra characters after number");
		}

		return num;
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Invalid input: " << e.what() << std::endl;
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Out of range: " << e.what() << std::endl;
	}

	return -1;  // Return 0 or handle as needed
}
