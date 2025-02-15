#include "worker.h"

#include <queue>

std::vector<vector<Worker>> workers;
std::vector<Worker> bosses;
std::vector<Worker> receptionists;

std::vector<std::pair<short, short>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1} };

/////////////////////////////////////////////////////////////////

Node::Node(short x, short y, double g, double h, struct Node* parent)
{	
	this->x = x;
	this->y = y;

	this->parent = parent;

	this->gCost = g;
	this->fCost = g + h;
}

bool Node::operator<(const Node& other) const
{
	if (fCost == other.fCost)
	{
		return gCost > other.gCost;
	}

	return fCost > other.fCost;
}

/////////////////////////////////////////////////////////////////

Worker::Worker(short int x, short int y) {
	this->gridnumber = chosengrid;
	this->index = workers[chosengrid].size();
	this->Zone = Office;
	pair<bool, char*> p = NameGenerator();
	this->name = p.second;
	this->gender = p.first;
	this->pos = { x,y };
}

Worker::Worker(bool gender, char* name, short int x, short int y) {
	this->gridnumber = chosengrid;
	this->index = workers[chosengrid].size();
	this->Zone = Office;
	this->gender = gender;
	this->name = name;
	this->pos = { x,y };
}

double heuristic(int x1, int y1, int x2, int y2) {
	//return max(abs(x1 - x2), abs(y1 - y2));
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void Worker::pathfind(pair<short int,short int> end)
{
	vector<pair<short, short>> temppath = { end };
	while (end != pos)
	{
		if (end.first < pos.first)end.first++;
		if (end.first > pos.first)end.first--;
		if (end.second < pos.second)end.second++;
		if (end.second > pos.second)end.second--;
		(*grid)[end.second - screenbuffer][end.first - screenbuffer] = 1;
		temppath.push_back(end);
	}
	path = temppath;
}

void Worker::eat() {
}

bool Worker::give(int index, int fid, int fq, int wid, int wq) {
	if (fq > 0) {
		if (inventory.foodinv[fid] >= fq) {
			if (workers[gridnumber][index].inventory.foodcount + fq <= 10) {
				inventory.food_from_inventory(fid, fq);
				workers[gridnumber][index].inventory.food_into_inventory(fid, fq);
				return 1;
			}
			else return 0;
		}
		else return 0;
	}
	else {
		if (inventory.workinv[wid] >= wq) {
			if (workers[gridnumber][index].inventory.workcount + wq <= 10) {
				inventory.work_from_inventory(wid, wq);
				workers[gridnumber][index].inventory.work_into_inventory(wid, wq);
				return 1;
			}
			else return 0;
		}
		else return 0;
	}

}

bool Worker::take(int index, int fid, int fq, int wid, int wq) {
	return workers[gridnumber][index].give(this->index, fid, fq, wid, wq);
}

int Worker::buy(Food food, int q) {
	bool b = inventory.food_into_inventory(food.id, q);
	if (!b) return 0;
	return q*food.cost;
}

void Worker::getCode()
{
	linecounter = 0;
	labelMap.clear();

	std::vector<std::string> tokens;
	std::string paired_paran = "";

	int codeSize = code.size();
	for (int i = 0; i < codeSize; i++)
	{
		tokenizer(code[i], linesize[i], tokens);
		if (nonPairedEntity)
		{
			std::cout << "non paired brackets on " << i << '\n';
		}


		if (!tokens.empty() && tokens[0] == labelIdentifier)
		{
			if (labelMap.find(tokens[1]) == labelMap.end())
			{
				labelMap[tokens[1]] = i;
				std::cout << code[i] << "," << i << " token\n";
			}
			else
			{
				// idle call
				std::cout << code[i] << " common token\n";
				return;
			}
		}

		tokens.clear();
	}
}

void Worker::tokenizer(char* instruction, short int instructionSize, std::vector<std::string>& tokensList)
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

	if (! paired_paran.empty())
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

	return -1;
}

int Worker::genericProcess(std::string genericVal)
{
	std::string value = "";
	std::string attribute = "";

	bool attributePresent = false;

	for (int c = 0, len = genericVal.size(); c < len; c++)
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

	if (value == "food")
	{
		if (!attribute.empty())
		{
			attribute[0] = std::toupper(attribute[0]);

			if(foods.find(attribute) != foods.end())
			{

				int i = std::distance(foods.begin(), foods.find(attribute));
				return -(i + 1) * foodLimit;
			}
		}
		else
		{
			return -1;
		}
	}

	if (value == "work")
	{
		if (attribute.empty())
		{
			if (lastTalkingto != -1)
			{
				return worker_types[workers[gridnumber][lastTalkingto].jobType];
			}
		}
		else
		{
			if (attribute == "number")
			{
				return workVals.first;
			}
			else if (attribute == "pending")
			{
				return workVals.second;
			}
		}
	}

	if (value == "zone")
	{
		// check in zone
		for (int i = 0; i < zone.size(); i++)
		{
			if (attribute == zone[i])
			{
				return i;
			}
		}
	}

	if (value != "me" && value != "zone" && value != "work")
	{
		value[0] = std::toupper(value[0]);
		for (auto& worker : workers[gridnumber])
		{
			if (worker.name == value)
			{
				if (attribute.empty())
				{
					return -(worker.index + 1) * zoneLimit;
				}
				else
				{
					return worker.getExpression("me", attribute);
				}
			}
		}

		//for (auto& receptionist : receptionists[gridnumber])
		//{
		//	if (receptionist.name == value)
		//	{
		//		if (attribute.empty())
		//		{
		//			return -(receptionist.index + 1) * zoneLimit;
		//		}
		//		else
		//		{
		//			return receptionist.getExpression("me", attribute);
		//		}
		//	}
		//}

		return -1;
	}

	return getExpression(value, attribute);
}

int Worker::expressionProcess(int lhs, int rhs, std::string Opr)
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

		// empty line skip
		if (tokens.empty())
		{
			linecounter++;
			return;
		}

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
		else if(tokens[0] == labelIdentifier)
		{
			linecounter++;
			return;
		}

		if (expectToken != gameLexers::none)
		{
			// check cases for next tokens
			if (expectToken == gameLexers::generic && tcount >= 2 && tokens[1] == genericIdentifier)
			{
				tempVal = genericProcess(tokens[2]);
			}
			else if (expectToken == gameLexers::literal)
			{
				tempVal = literalProcess(tokens[1]);
			}
			else if (expectToken == gameLexers::label || tokens[1] == labelIdentifier)
			{
				std::cout << "label found\n";
				if (labelMap.find(tokens[2]) != labelMap.end())
				{
					tempVal = labelMap[tokens[2]];
				}
				else
				{
					tempVal = -1;
				}

				std::cout << tempVal << '-' << tokens[2] << '\n';
			}
			else if (expectToken == gameLexers::expression)
			{
				int lhs, rhs;
				lhs = rhs = -1;

				if (tokens[1] != genericIdentifier && tokens[3] != genericIdentifier)
				{
					if (tcount > 3)
					{
						lhs = literalProcess(tokens[1]);
						tempToken = tokens[2];
						rhs = literalProcess(tokens[3]);
					}
				}
				else
				{
					if (tcount > 4)
					{
						if (tokens[1] == genericIdentifier && tokens[4] != genericIdentifier)
						{
							lhs = genericProcess(tokens[2]);
							tempToken = tokens[3];
							rhs = literalProcess(tokens[4]);
						}
						else if (tokens[1] != genericIdentifier && tokens[3] == genericIdentifier)
						{
							lhs = literalProcess(tokens[1]);
							tempToken = tokens[2];
							rhs = genericProcess(tokens[4]);
						}
						else if (tokens[1] == genericIdentifier && tokens[4] == genericIdentifier)
						{
							lhs = genericProcess(tokens[2]);
							tempToken = tokens[3];
							rhs = genericProcess(tokens[5]);
						}
					}
				}

				if(bracketCheck && tokens.back() != paranFlower.first) 
				{
					lhs = rhs = -1;
				}

				tempVal = expressionProcess(lhs, rhs, tempToken);
			}

			// some other method corrections
			if (tempVal == -1)
			{
				// idle call
				std::cout << code[linecounter] << " :error in expression\n";
				linecounter++;
				return;
			}
			else
			{

			}
		}


		if (tokens[0] == "moveto")
		{
			std::pair<int, int> dir = pos;

			if (tempVal <= -zoneLimit)
			{
				//cout << tempVal << '\n';
				tempVal = -(tempVal / zoneLimit + 1);
				dir = workers[gridnumber][tempVal].pos;
			}
			else
			{
				switch (tempVal)
				{
				case 1:
					break;
				default:
					break;
				}
			}

			//Pathfind
			pathfind(dir);
			activitycounter = path.size();
			activity = Moving;

		}
		else if (tokens[0] == "break")
		{
			activitycounter = tempVal;
			/*
			if(lag == -1)
			{
				lag = tempVal;
				// do break changes
			}
			*/
		}
		else if (tokens[0] == "if")
		{
			std::string paired_paran = "{";
			nonPairedEntity = true;

			tcount = linecounter;
			//std::cout << "if : " << tempVal << '\n';
			
			if (tempVal == 0)
			{
				linecounter++; // start from next line

				while (linecounter < codeSize)
				{
					//std::cout << linecounter << '\n';

					tokens.clear();
					tokenizer(code[linecounter], linesize[linecounter], tokens);

					for (auto& token : tokens)
					{
						if (paired_paran.empty())
						{
							return;
						}
						else if (token == paranFlower.second)
						{
							paired_paran.pop_back();
						}
						else if (token == paranFlower.first)
						{
							paired_paran += token;
						}
					}

					linecounter++;
				}

				// idle call
				if (linecounter == codeSize)
				{
					std::cout << tcount << " No brackets found matching\n";
					activity = Idle;
				}
			}
			//else
			//{
			//	linecounter++;;
			//}
		}
		else if (tokens[0] == "work")
		{
			pathfind(workspace);
			activitycounter = path.size()+worktime;
			activity = Working;
		}
		else if (tokens[0] == "talk")
		{
			if (tempVal <= -zoneLimit)
			{
				tempVal = -(tempVal / zoneLimit + 1);

				//TODO REPLACE THIS CODE
				if (workers[gridnumber][tempVal].path.size() > 0)
				{
					pathfind(workers[gridnumber][tempVal].path.front());
					workers[gridnumber][tempVal].activitycounter = max(path.size(), workers[gridnumber][tempVal].path.size()) + talkingtime;
					activitycounter = max(path.size(), workers[gridnumber][tempVal].path.size()) + talkingtime;
				}
				else
				{
					pathfind(workers[gridnumber][tempVal].pos);
					workers[gridnumber][tempVal].activitycounter = path.size()+ talkingtime;
					activitycounter = path.size() + talkingtime;
				}
				lastTalkingto = tempVal;

				if (heuristic(this->pos.first, this->pos.second, workers[gridnumber][tempVal].pos.first, workers[gridnumber][tempVal].pos.second) <= sqrt(2))
				{
					std::cout << this->name << " talking with " << workers[gridnumber][tempVal].name << '\n';
					//workers[gridnumber][tempVal].activity = talking;
					activity = Talking;
				}
			}
			
		}
		else if (tokens[0] == "submit")
		{

		}
		else if (tokens[0] == "eat")
		{
			for (int i = 0; i < lunchpositions.size(); i++)
			{
				if (lunchpositions[i].second==false)
				{
					lunchpositions[i].second = true;
					pathfind(lunchpositions[i].first);
					activitycounter = path.size()+eattime;
					occupiedbench = i;
					activity = Eating;
					break;
				}
			}
		}
		else if (tokens[0] == "exit")
		{
			std::cout << "exit" << '\n';
		}
		else if (tokens[0] == "give")
		{
			inventory.display_inventory();
			std::cout << tempVal << '\n';
			if (lastTalkingto != -1)
			{
				if (heuristic(this->pos.first, this->pos.second, workers[gridnumber][lastTalkingto].pos.first, workers[gridnumber][lastTalkingto].pos.second) <= sqrt(2))
				{
					activity = Giving;
					std::cout << this->name << " giving " << workers[gridnumber][lastTalkingto].name << '\n';
					if (tempVal <= -foodLimit)
					{
						tempVal = -(tempVal / foodLimit + 1);
						std::cout << "food index " << tempVal << '\n';
						give(lastTalkingto, tempVal, 1);
					}
					else if(tempVal < worker_types.size())
					{
						std::cout << "work type " << tempVal << '\n';
						give(lastTalkingto, 0, 0, tempVal, 1);
					}
				}
				inventory.display_inventory();
			}
			activitycounter = 1;
		}
		else if (tokens[0] == "take")
		{
			inventory.display_inventory();
			std::cout << tempVal << '\n';
			if (lastTalkingto != -1)
			{
				if (heuristic(this->pos.first, this->pos.second, workers[gridnumber][lastTalkingto].pos.first, workers[gridnumber][lastTalkingto].pos.second) <= sqrt(2))
				{
					activity = Taking;
					std::cout << this->name << " taking " << workers[gridnumber][lastTalkingto].name << '\n';
					if (tempVal <= -foodLimit)
					{
						tempVal = -(tempVal / foodLimit + 1);
						std::cout << "food index " << tempVal << '\n';
						take(lastTalkingto, tempVal, 1);
					}
					else if (tempVal < worker_types.size())
					{
						std::cout << "work type " << tempVal << '\n';
						take(lastTalkingto, 0, 0, tempVal, 1);
					}
				}
				inventory.display_inventory();
			}
			activitycounter = 1;
		}
		else if (tokens[0] == "buy")
		{
			if (occupiedbench == -1)
			{
				for (int i = 0; i < lunchpositions.size(); i++)
				{
					if (lunchpositions[i].second == false)
					{
						lunchpositions[i].second = true;
						pathfind(lunchpositions[i].first);
						activitycounter = path.size();
						occupiedbench = i;
						activity = Moving;
						break;
					}
				}
			}
			else
			{
				activitycounter = 2;
				activity = Buying;
				if (tempVal <= foodLimit)
				{
					std::cout << tempVal;
					tempVal = -(tempVal / foodLimit + 1);
					auto it = foods.begin();
					std::advance(it, tempVal);

					buy(it->second, 1);
				}
				inventory.display_inventory();
				lunchpositions[occupiedbench].second = false;
				occupiedbench = -1;
			}

			if (lag == -1)
			{
				lag = 2;
			}
		}
		else if (tokens[0] == "jump")
		{
			std::cout << "jump" << '\n';
			linecounter = tempVal;
			return;
		}

		if (actionCheck)
		{
			lastTalkingto = -1;
		}

		// For repeated call functions
		if (lag > 1)
		{
			linecounter--;
			lag--;
		}
		else
		{
			lag = -1;
		}

		linecounter++;
	}
	//linecounter = 0;
}

void Worker::incSkills(int amt)
{
	if (skills + amt >= 100) {
		int reqd = 100 - skills;
		skills = 100;
		money -= 2 * reqd;
		return;
	}

	skills += amt;
	money -= 2 * amt;
}

void Worker::updateObedience()
{
	std::cout << "virtual function\n";
}
