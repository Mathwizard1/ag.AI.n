#include "worker.h"

#include <queue>

std::vector<vector<Worker>> workers;
std::vector<Worker> bosses;
std::vector<Worker> receptionists;

std::vector<std::pair<short, short>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1} };


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

float heuristic(int x1, int y1, int x2, int y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

vector<pair<short int, short int>> Worker::getneighbors(pair<short int, short int> node)
{
	vector<pair<short int, short int>> neighbors;
	
	for (const auto& i : directions)
	{
		if (node.first + i.first < screenbuffer || (node.first + i.first) >= (*grid)[0].size()+screenbuffer )continue;
		if (node.second + i.second < screenbuffer || (node.second + i.second) >= grid->size()+screenbuffer )continue;

		if ((*grid)[node.second + i.second-screenbuffer][node.first + i.first-screenbuffer] !=-1)
		{
			neighbors.push_back({ node.first + i.first ,node.second + i.second });

		}
	}

	return neighbors;
}

void Worker::pathfind(pair<short int,short int> end)
{
	vector<pair<short int,short int>> neighbors;
	priority_queue<Node,vector<Node>,NodeComparison> open;
	vector<Node> closed;

	Node temp;
	temp.x = end.first;
	temp.y = end.second;
	temp.fcost = heuristic(end.first, end.second, pos.first, pos.second);
	temp.path = { end };
	open.push(temp);

	int counter = 0;
	while(true)
	{
		if (open.empty())
		{
			cout << "PATH NOT FOUND" << endl;
			return;
		}

		Node node= open.top();
		open.pop();

		if (node.x == pos.first && node.y == pos.second)
		{
			//cout << "NODE FOUND" << endl;
			temp = node;
			break;
		}

		neighbors = getneighbors({ node.x,node.y });
		for (const auto& i : neighbors)
		{
			float f = heuristic(pos.first, pos.second, i.first, i.second) + heuristic(end.first, end.second, i.first, i.second);

			bool contflag = false;
			for (int j=0;j<closed.size();j++)
			{
				if (i.first == closed[j].x && i.second==closed[j].y)
				{
					contflag = true;
					break;
					
				}
			}
			if (contflag)continue;
			else
			{
				Node n;
				n.x = i.first;
				n.y = i.second;
				n.fcost = f;
				n.path = node.path;
				n.path.push_back({ node.x,node.y });
				open.push(n);
				closed.push_back(n);
			}
		}
	}

	path = temp.path;
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
	std::vector<int> paranIndex;
	bool errorFlag = false;

	int codeSize = code.size();
	for (int i = 0; i < codeSize; i++)
	{
		tokenizer(code[i], linesize[i], tokens);
		if (nonPairedEntity)
		{
			std::cout << "non paired brackets on " << i << '\n';
		}


		if (!tokens.empty())
		{
			if (tokens[0] == labelIdentifier)
			{
				if(labelMap.find(tokens[1]) == labelMap.end())
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
			else
			{
				for (const std::string& t : tokens)
				{
					if (t == paranFlower.second)
					{
						if (! paranIndex.empty())
						{
							bracketMaps[paranIndex.back()] = i;
							paranIndex.pop_back();
						}
						else
						{
							paranIndex.push_back(i);
							errorFlag = true;
							break;
						}
					}
					else if (t == paranFlower.first)
					{
						paranIndex.push_back(i);
					}
				}

				if (errorFlag) break;
			}
		}

		tokens.clear();
	}

	if (! paranIndex.empty())
	{
		std::cout << code[paranIndex.back()] << " non paired brackets in code" << '\n';
		linecounter = codeSize;
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

				//tokens for { }
				if (specialString == paranFlower.first || specialString == paranFlower.second)
				{
					tokensList.push_back(specialString);
					specialString.clear();
				}
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

int Worker::getExpression(std::string& atr)
{
	bool foundValue = false;
	int foundIndex = -1;

	if (atr.empty())
	{
		return -1;
	}

	// check in me
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

	atr[0] = std::toupper(atr[0]);
	if (foods.find(atr) != foods.end())
	{
		auto foodIt = inventory.foodinv.begin();
		std::advance(foodIt, std::distance(foods.begin(), foods.find(atr)));
		std::cout << (*foodIt) << " :food iterator\n";
		return (*foodIt);
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

	//std::cout << value << '.' << attribute << '\n';

	if (value == "food")
	{
		if (!attribute.empty())
		{
			attribute[0] = std::toupper(attribute[0]);
			if (foods.find(attribute) != foods.end())
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

	if (value != "me" && value != "zone" && value != "work" && value != "food")
	{
		if (value == "worker")
		{

			if (!attribute.empty())
			{
				int workerIndex = 0;
				float workerDist = 0;

				if (attribute == "recent")
				{
					if (lastTalkingto != -1)
					{
						return -(lastTalkingto + 1) * zoneLimit;
					}
					else
					{
						return -1;
					}
				}
				else if (attribute == "near")
				{
					float minDist = 10.0 * (grid->size());
					for (int w = 0, len = workers[gridnumber].size(); w < len; w++)
					{
						workerDist = heuristic(this->pos.first, this->pos.second, workers[gridnumber][w].pos.first, workers[gridnumber][w].pos.second);
						//std::cout << w << ":dist = " << workerDist << '\n';
						if (workerDist != 0 && workerDist < minDist)
						{
							minDist = workerDist;
							workerIndex = w;
						}
					}

					lastTalkingto = workerIndex;
					return -(workerIndex + 1) * zoneLimit;
				}
				else if (attribute == "far")
				{
					float maxDist = -10.0 * (grid->size());
					for (int w = 0, len = workers[gridnumber].size(); w < len; w++)
					{
						workerDist = heuristic(this->pos.first, this->pos.second, workers[gridnumber][w].pos.first, workers[gridnumber][w].pos.second);
						//std::cout << w << ":dist = " << workerDist << '\n';
						if (workerDist > maxDist)
						{
							maxDist = workerDist;
							workerIndex = w;
						}
					}

					lastTalkingto = workerIndex;
					return -(workerIndex + 1) * zoneLimit;
				}
			}
			else
			{
				return -1;
			}
		}


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
					return worker.getExpression(attribute);
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

	return getExpression(attribute);
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
			int wSize = workers[gridnumber].size();
			if (tempVal <= -zoneLimit && tempVal >= -(wSize + 1) * zoneLimit)
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
			//tcount = linecounter;
			std::cout << "if : " << tempVal << '\n';

			if (tempVal == 0)
			{
				linecounter = bracketMaps[linecounter]; // start from end
			}
		}
		else if (tokens[0] == "work")
		{
			pathfind(workspace);
			activitycounter = path.size()+worktime;
			activity = Working;
		}
		else if (tokens[0] == "talk")
		{
			int wSize = workers[gridnumber].size();
			if (tempVal <= -zoneLimit && tempVal >= -(wSize + 1) * zoneLimit)
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
			inventory.display_inventory(this->name);
			std::cout << tempVal << '\n';
			if (lastTalkingto != -1)
			{
				if (heuristic(this->pos.first, this->pos.second, workers[gridnumber][lastTalkingto].pos.first, workers[gridnumber][lastTalkingto].pos.second) <= sqrt(2))
				{
					activity = Giving;
					std::cout << this->name << " giving " << workers[gridnumber][lastTalkingto].name << '\n';
					if (tempVal <= -foodLimit && tempVal >= -(foodLimit + 1) * foodLimit)
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
				inventory.display_inventory(this->name);
			}
			activitycounter = 1;
		}
		else if (tokens[0] == "take")
		{
			inventory.display_inventory(this->name);
			std::cout << tempVal << '\n';
			if (lastTalkingto != -1)
			{
				if (heuristic(this->pos.first, this->pos.second, workers[gridnumber][lastTalkingto].pos.first, workers[gridnumber][lastTalkingto].pos.second) <= sqrt(2))
				{
					activity = Taking;
					std::cout << this->name << " taking " << workers[gridnumber][lastTalkingto].name << '\n';
					if (tempVal <= -foodLimit && tempVal >= -(foodLimit + 1) * foodLimit)
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
				inventory.display_inventory(this->name);
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
				if (tempVal <= -foodLimit && tempVal >= -(foodLimit + 1) * foodLimit)
				{
					std::cout << tempVal;
					tempVal = -(tempVal / foodLimit + 1);
					auto it = foods.begin();
					std::advance(it, tempVal);

					buy(it->second, 1);
				}
				inventory.display_inventory(this->name);
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

Worker::~Worker()
{
	// TO DO
	// clean up the workers own char * which was either malloced or new
}
