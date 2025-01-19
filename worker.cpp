#include "worker.h"
#include "food.h"

std::vector<Worker> workers;
std::vector<Worker> bosses;
std::vector<Worker> receptionists;

Worker::Worker(short int x, short int y) {
	this->energy = 50;
	this->productivity = 80;
	this->health = 100;
	this->Zone = Office;
	pair<bool, char*> p = gameFunction().NameGenerator();
	this->name = p.second;
	this->gender = p.first;
	this->money = 0;
	this->mood = (3 * energy + 5 * health) / 8;
	this->pos = { x,y };
}

Worker::Worker(bool gender, char* name, short int x, short int y) {
	this->energy = 50;
	this->productivity = 80;
	this->health = 100;
	this->Zone = Office;
	this->gender = gender;
	this->name = name;
	this->money = 0;
	this->mood = (3 * energy + 5 * health) / 8;
	this->pos = { x,y };
}

void Worker::pathfind(pair<short int,short int> end)
{
	vector<pair<short int, short int>> temppath = { end };

	while (end != pos)
	{
		if (end.first < pos.first)end.first++;
		if (end.first > pos.first)end.first--;
		if (end.second < pos.second)end.second++;
		if (end.second > pos.second)end.second--;
		temppath.push_back(end);
	}
	path = temppath;
}

void Worker::eat() {
}

void Worker::give() {

}

void Worker::take() {

}

void Worker::tokenizer(const char* instruction, short int instructionSize, std::vector<std::string>& tokensList)
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

int Worker::getExpression(const std::string& var, const std::string& atr)
{
	bool foundValue = false;
	int foundIndex = -1;

	// check in me
	if (var == "me")
	{
		for (int i = 0; i < me.size(); i++)
		{
			if (me[i] == atr)
			{
				foundValue = true;
				foundIndex = i;
				break;
			}
		}

		if (foundValue)
		{
			switch (foundIndex)
			{
			case 0:
				return energy;
			case 1:
				return productivity;
			case 2:
				return mood;
			case 3:
				return health;
			case 4:
				return money;
			default:
				break;
			}
		}
	}
	else if (var == "work")
	{
		// check in work
		for (int i = 0; i < work.size(); i++)
		{
			if (work[i] == atr)
			{
				foundValue = true;
				foundIndex = i;
				break;
			}
		}

		if (foundValue)
		{
			switch (foundIndex)
			{
			case 0:
				return workProp[0];
			case 1:
				return workProp[1];
			default:
				break;
			}
		}
	}
	else if (var == "zone")
	{
		// check in zone
		for (int i = 0; i < zone.size(); i++)
		{
			if (zone[i] == atr)
			{
				foundValue = true;
				foundIndex = i;
				break;
			}
		}

		if (foundValue)
		{
			return foundIndex;
		}
	}
	else
	{
		// check in peoples

	}

	return -1;
}

int Worker::genericProcess(std::string genericVal)
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

	return getExpression(value, attribute);
}

int Worker::expressionProcess(int lhs, int rhs, std::string Opr)
{
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

int Worker::literalProcess(std::string intLiteral)
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

void Worker::callFunction()
{
	int codeSize = code.size();
	if (linecounter < codeSize)
	{
		std::vector<std::string> tokens;
		tokenizer(code[linecounter], linesize[linecounter], tokens);

		gameLexers expectToken = gameLexers::none;
		bool bracketCheck = false;
		bool actionCheck = false;

		int tempVal = -1;
		std::string tempToken = "";
		int tcount = tokens.size();

		// first token is keyword
		if (gameLanguage.find(tokens[0]) != gameLanguage.end())
		{
			expectToken = gameLanguage[tokens[0]].expected;
			bracketCheck = gameLanguage[tokens[0]].bracketd;
			actionCheck = gameLanguage[tokens[0]].action;
		}
		else
		{
			// idle call
			return;
		}

		if (expectToken != gameLexers::none)
		{
			// check cases for next tokens
			if (expectToken == gameLexers::generic && tokens[1] == "$")
			{
				if (tcount < 3)
				{
					// idle call
					return;
				}
				tempVal = genericProcess(tokens[2]);
			}
			else if (expectToken == gameLexers::literal)
			{
				tempVal = literalProcess(tokens[1]);
			}
			else if (expectToken == gameLexers::label && tokens[1] == "!")
			{
				tempVal = 9696;
			}
			else if (expectToken == gameLexers::expression)
			{
				int lhs, rhs;
				lhs = rhs = -1;

				if (tcount <= 3)
				{
					// idle call
					return;
				}

				if (tokens[1] != "$" && tokens[3] != "$")
				{
					lhs = literalProcess(tokens[1]);
					tempToken = tokens[2];
					rhs = literalProcess(tokens[3]);
				}
				else
				{
					if (tcount <= 4)
					{
						// idle call
						return;
					}

					if (tokens[1] == "$" && tokens[4] != "$")
					{
						lhs = genericProcess(tokens[2]);
						tempToken = tokens[3];
						rhs = literalProcess(tokens[4]);
					}
					else if (tokens[1] != "$" && tokens[3] == "$")
					{
						lhs = literalProcess(tokens[1]);
						tempToken = tokens[2];
						rhs = genericProcess(tokens[4]);
					}
					else if (tokens[1] == "$" && tokens[4] == "$")
					{
						lhs = literalProcess(tokens[2]);
						tempToken = tokens[3];
						rhs = genericProcess(tokens[5]);
					}
				}

				if(bracketCheck && tokens[tcount - 1] != paranFlower.first) 
				{
					lhs = rhs = -1;
				}

				if (lhs == -1 || rhs == -1)
				{
					// idle call
					return;
				}

				tempVal = expressionProcess(lhs, rhs, tempToken);
			}

			// some other method corrections
			if (tempVal == -1)
			{
				// idle call
				std::cout << "error in expression\n";
				return;
			}
			else
			{

			}
		}

		if (tokens[0] == "moveto")
		{
			std::pair<int, int> dir = pos;

			switch (tempVal)
			{
			case 0:
				dir = { 20, 20 };
				break;
			default:
				break;
			}

			pathfind(dir);
		}
		else if (tokens[0] == "break")
		{
			std::cout << "break" << tempVal << '\n';
		}
		else if (tokens[0] == "work")
		{
			std::cout << "work" << '\n';
		}
		else if (tokens[0] == "talk")
		{
			//std::cout << "talk" << '\n';
		}
		else if (tokens[0] == "submit")
		{

		}
		else if (tokens[0] == "eat")
		{

		}
		else if (tokens[0] == "exit")
		{
			std::cout << "exit" << '\n';
		}
		else if (tokens[0] == "give")
		{

		}
		else if (tokens[0] == "take")
		{

		}


		linecounter++;
		return;
	}

	//linecounter = 0;
}
