#include "worker.h"

#include <queue>

std::vector<vector<Worker>> workers;
std::vector<Worker> bosses;
std::vector<Worker> receptionists;


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

float euclid(int x1, int y1, int x2, int y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void Worker::pathfind(pair<short int,short int> end)
{
	thread t1(&Pathfinding::pathfind, &pathfinder, end, pos, grid, &path);
	t1.detach();
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

	errorFound = false;
	bool errorFlag = false;

	int codeSize = code.size();
	for (int i = 0; i < codeSize; i++)
	{
		tokenizer(code[i], linesize[i], tokens);
		if (nonPairedEntity)
		{
			std::cout << "non paired brackets on " << i << '\n';
			errorFound = true;
			linecounter = i;
			return;
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
					errorFound = true;
					linecounter = i;
					return;
				}
			}
			else
			{
				for (const std::string& t : tokens)
				{
					std::cout << t << '\n';
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

		errorFound = true;
		linecounter = paranIndex.back();
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

std::pair<gameObjects, int> Worker::getExpression(std::string& atr)
{
	bool foundValue = false;
	int foundIndex = -1;

	if (atr.empty())
	{
		return std::make_pair(gameObjects::None, -1);
	}

	// check in me
	gameObjects gameObj = gameObjects::Me;
	int gameVar = -1;

	if (atr == "energy")
	{
		gameVar = energy;
		return std::make_pair(gameObj, gameVar);
	}
	else if (atr == "productivity")
	{
		gameVar = productivity;
		return std::make_pair(gameObj, gameVar);
	}
	else if (atr == "mood")
	{
		gameVar = mood;
		return std::make_pair(gameObj, gameVar);
	}
	else if (atr == "health")
	{
		gameVar = health;
		return std::make_pair(gameObj, gameVar);
	}
	else if (atr == "money")
	{
		gameVar = money;
		return std::make_pair(gameObj, gameVar);
	}

	atr[0] = std::toupper(atr[0]);
	if (foods.find(atr) != foods.end())
	{
		auto foodIt = inventory.foodinv.begin();
		std::advance(foodIt, std::distance(foods.begin(), foods.find(atr)));
		std::cout << (*foodIt) << " :food iterator\n";

		return std::make_pair(gameObjects::Food, *foodIt);
	}

	return std::make_pair(gameObjects::None, -1);
}

std::pair<gameObjects, int> Worker::genericProcess(std::string genericVal)
{
	std::string value = "";
	std::string attribute = "";

	bool attributePresent = false;
	gameObjects gameObj = gameObjects::None;

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
				return std::make_pair(gameObjects::Food, i);
			}
		}
		else
		{
			return std::make_pair(gameObjects::None, -1);
		}
	}

	if (value == "work")
	{
		gameObj = gameObjects::Work;
		if (attribute.empty())
		{
			if (lastTalkingto != -1)
			{
				return std::make_pair(gameObj, worker_types[workers[gridnumber][lastTalkingto].jobType]);
			}
		}
		else
		{
			if (attribute == "number")
			{
				return std::make_pair(gameObj, workVals.first);
			}
			else if (attribute == "pending")
			{
				return std::make_pair(gameObj, workVals.second);
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
				gameObj = gameObjects::Zone;
				return std::make_pair(gameObj, i);
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

				if (attribute == "near")
				{
					float minDist = 10.0 * (grid->size());
					for (int w = 0, len = workers[gridnumber].size(); w < len; w++)
					{
						workerDist = euclid(this->pos.first, this->pos.second, workers[gridnumber][w].pos.first, workers[gridnumber][w].pos.second);
						//std::cout << w << ":dist = " << workerDist << '\n';
						if (workerDist != 0 && workerDist < minDist)
						{
							minDist = workerDist;
							workerIndex = w;
						}
					}

					lastTalkingto = workerIndex;
				}
				else if (attribute == "far")
				{
					float maxDist = -10.0 * (grid->size());
					for (int w = 0, len = workers[gridnumber].size(); w < len; w++)
					{
						workerDist = euclid(this->pos.first, this->pos.second, workers[gridnumber][w].pos.first, workers[gridnumber][w].pos.second);
						//std::cout << w << ":dist = " << workerDist << '\n';
						if (workerDist > maxDist)
						{
							maxDist = workerDist;
							workerIndex = w;
						}
					}

					lastTalkingto = workerIndex;
				}

				return std::make_pair(gameObjects::Person, lastTalkingto);
			}
			else
			{
				return std::make_pair(gameObjects::None, -1);
			}
		}


		value[0] = std::toupper(value[0]);
		for (auto& worker : workers[gridnumber])
		{
			if (worker.name == value)
			{
				if (attribute.empty())
				{
					return std::make_pair(gameObjects::Person, worker.index);
				}
				else
				{
					std::pair<gameObjects, int> tempObj = worker.getExpression(attribute);
					tempObj.first = gameObjects::Value;
					return tempObj;
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

		return std::make_pair(gameObjects::None, -1);
	}

	return getExpression(attribute);
}

std::pair<gameObjects, int> Worker::expressionProcess(int lhs, int rhs, std::string Opr)
{
	if (lhs == -1 || rhs == -1)
	{
		return std::make_pair(gameObjects::None, -1);
	}


	if (Opr == "==")
	{
		return std::make_pair(gameObjects::Value, lhs == rhs);
	}
	else if (Opr == "!=")
	{
		return std::make_pair(gameObjects::Value, lhs != rhs);
	}
	else if (Opr == "<=")
	{
		return std::make_pair(gameObjects::Value, lhs <= rhs);
	}
	else if (Opr == ">=")
	{
		return std::make_pair(gameObjects::Value, lhs >= rhs);
	}
	else if (Opr == "<")
	{
		return std::make_pair(gameObjects::Value, lhs < rhs);
	}
	else if (Opr == ">")
	{
		return std::make_pair(gameObjects::Value, lhs > rhs);
	}

	return std::make_pair(gameObjects::None, -1);
}

int Worker::literalProcess(std::string intLiteral)
{
	try {
		size_t pos;
		int num = std::stoi(intLiteral, &pos);  // Convert string to int

		// Check if there are no leftover characters after the integer
		if (pos == intLiteral.length()) {
			return num;
		}

		std::cerr << "Extra characters after number" << '\n';
	}
	catch (const std::invalid_argument& e) {
		std::cerr << "Invalid input: " << e.what() << '\n';
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Out of range: " << e.what() << '\n';
	}

	return -1;  // Return 0 or handle as needed
}

void Worker::callFunction()
{
	int codeSize = code.size();

	if (linecounter < codeSize and !errorFound)
	{
		std::vector<std::string> tokens;
		tokenizer(code[linecounter], linesize[linecounter], tokens);

		// empty line skip
		while (tokens.empty())
		{
			linecounter++;

			if (linecounter == codeSize) return;
			tokenizer(code[linecounter], linesize[linecounter], tokens);
		}

		gameLexers expectToken = gameLexers::none;
		bool bracketCheck = false;
		bool actionCheck = false;
		std::unordered_set<gameObjects> gameObjs;

		std::pair<gameObjects, int> tempVal = { gameObjects::None, -1 };
		std::string tempToken = "";

		int tcount = tokens.size();

		// first token is keyword
		if (gameLanguage.find(tokens[0]) != gameLanguage.end())
		{
			expectToken = gameLanguage[tokens[0]].expected;
			bracketCheck = gameLanguage[tokens[0]].bracketd;
			actionCheck = gameLanguage[tokens[0]].action;

			gameObjs = gameLanguage[tokens[0]].gameObjs;
		}
		else if(tokens[0] == labelIdentifier || tokens[0] == paranFlower.second)
		{
			linecounter++;
			return;
		}
		else
		{
			errorFound = true;
			return;
		}

		if (expectToken != gameLexers::none)
		{
			// check cases for next tokens
			if (expectToken == gameLexers::generic && tcount >= 2 && tokens[1] == genericIdentifier)
			{
				tempVal = genericProcess(tokens[2]);
			}
			else if (expectToken == gameLexers::literal && tcount == 2)
			{
				tempVal.first = gameObjects::Value;
				tempVal.second = literalProcess(tokens[1]);
			}
			else if (expectToken == gameLexers::label && tokens[1] == labelIdentifier)
			{
				std::cout << "labelIdentifier found\n";
				if (labelMap.find(tokens[2]) != labelMap.end())
				{
					tempVal.first = gameObjects::Value;
					tempVal.second = labelMap[tokens[2]];
				}

				std::cout << tempVal.second << '-' << tokens[2] << '\n';
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
							lhs = genericProcess(tokens[2]).second;
							tempToken = tokens[3];
							rhs = literalProcess(tokens[4]);
						}
						else if (tokens[1] != genericIdentifier && tokens[3] == genericIdentifier)
						{
							lhs = literalProcess(tokens[1]);
							tempToken = tokens[2];
							rhs = genericProcess(tokens[4]).second;
						}
						else if (tokens[1] == genericIdentifier && tokens[4] == genericIdentifier)
						{
							lhs = genericProcess(tokens[2]).second;
							tempToken = tokens[3];
							rhs = genericProcess(tokens[5]).second;
						}
					}
				}

				if(bracketCheck && tokens.back() != paranFlower.first) 
				{
					lhs = rhs = -1;
				}

				std::cout << lhs << '_' << tempToken << '_' << rhs << '\n';
				tempVal = expressionProcess(lhs, rhs, tempToken);
			}

			// some other method corrections
			if (gameObjs.find(tempVal.first) == gameObjs.end() || tempVal.second < 0)
			{
				// idle call
				std::cout << tempVal.second << "<-value\n";
				std::cout << code[linecounter] << " :error in expression\n";

				// If needed to stop the line counter
				errorFound = true;
				linecounter++;
				return;
			}
		}

		if (tokens[0] == "if")
		{
			//tcount = linecounter;
			std::cout << "if : " << tempVal.second << '\n';

			if (tempVal.second == 0)
			{
				linecounter = bracketMaps[linecounter]; // start from end bracket
			}
		}
		else if (tokens[0] == "jump")
		{
			std::cout << "jump" << '\n';
			linecounter = tempVal.second;
			return;
		}

		else if (tokens[0] == "submit")
		{

		}
		else if (tokens[0] == "exit")
		{
			std::cout << "exit" << '\n';
		}
		else if (tokens[0] == "work")
		{
			pathfind(workspace);
			activitycounter = path.size()+worktime;
			activity = Working;
		}

		else if (tokens[0] == "break")
		{
			activitycounter = tempVal.second;
			/*
			if(lag == -1)
			{
				lag = tempVal;
				// do break changes
			}
			*/
		}

		else if (tokens[0] == "moveto")
		{
			std::pair<int, int> dir = pos;

			//Pathfind
			if (tempVal.first == gameObjects::Person)
			{
				//cout << tempVal.second << '\n';
				dir = workers[gridnumber][tempVal.second].pos;

				//Sync Conditions needed?
				//if (workers[gridnumber][tempVal.second].path.size() > 0)
				//{
				//	pathfind(workers[gridnumber][tempVal.second].path.front());
				//	workers[gridnumber][tempVal.second].activitycounter = max(path.size(), workers[gridnumber][tempVal.second].path.size()) + talkingtime;
				//	activitycounter = max(path.size(), workers[gridnumber][tempVal.second].path.size()) + talkingtime;
				//}
				//else
				//{
				//	pathfind(workers[gridnumber][tempVal.second].pos);
				//	workers[gridnumber][tempVal.second].activitycounter = path.size() + talkingtime;
				//	activitycounter = path.size() + talkingtime;
				//}
			}
			else
			{

			}


			// Activity counter check
			pathfind(dir);
			activitycounter = path.size();
			activity = Moving;
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

				auto it = foods.begin();
				std::advance(it, tempVal.second);

				buy(it->second, 1);

				inventory.display_inventory(this->name);
				lunchpositions[occupiedbench].second = false;
				occupiedbench = -1;
			}

			if (lag == -1)
			{
				lag = 2;
			}
		}

		else if (tokens[0] == "give")
		{
			//std::cout << tempVal << '\n';
			if (lastTalkingto != index)
			{
				if (euclid(this->pos.first, this->pos.second, workers[gridnumber][lastTalkingto].pos.first, workers[gridnumber][lastTalkingto].pos.second) <= sqrt(2))
				{
					activity = Giving;
					std::cout << this->name << " giving " << workers[gridnumber][lastTalkingto].name << '\n';
					if (tempVal.first == gameObjects::Food)
					{
						std::cout << "food index " << tempVal.second << '\n';
						give(lastTalkingto, tempVal.second, 1);
					}
					else if(tempVal.first == gameObjects::Work)
					{
						std::cout << "work type " << tempVal.second << '\n';
						give(lastTalkingto, 0, 0, tempVal.second, 1);
					}
				}

			}
			inventory.display_inventory(this->name);
			activitycounter = 1;
		}
		else if (tokens[0] == "take")
		{
			inventory.display_inventory(this->name);
			//std::cout << tempVal << '\n';
			if (lastTalkingto != index)
			{
				if (euclid(this->pos.first, this->pos.second, workers[gridnumber][lastTalkingto].pos.first, workers[gridnumber][lastTalkingto].pos.second) <= sqrt(2))
				{
					activity = Giving;
					std::cout << this->name << " taking " << workers[gridnumber][lastTalkingto].name << '\n';
					if (tempVal.first == gameObjects::Food)
					{
						std::cout << "food index " << tempVal.second << '\n';
						take(lastTalkingto, tempVal.second, 1);
					}
					else if (tempVal.first == gameObjects::Work)
					{
						std::cout << "work type " << tempVal.second << '\n';
						take(lastTalkingto, 0, 0, tempVal.second, 1);
					}
				}

			}
			inventory.display_inventory(this->name);
			activitycounter = 1;
		}
		else if (tokens[0] == "talk")
		{
			int wSize = workers[gridnumber].size();
			if (lastTalkingto != index)
			{
				std::cout << name << " talking with " << workers[gridnumber][tempVal.second].name << '\n';
				//TODO REPLACE THIS CODE
				if (workers[gridnumber][tempVal.second].path.size() > 0)
				{
					pathfind(workers[gridnumber][tempVal.second].path.front());
					workers[gridnumber][tempVal.second].activitycounter = max(path.size(), workers[gridnumber][tempVal.second].path.size()) + talkingtime;
					activitycounter = max(path.size(), workers[gridnumber][tempVal.second].path.size()) + talkingtime;
				}
				else
				{
					pathfind(workers[gridnumber][tempVal.second].pos);
					workers[gridnumber][tempVal.second].activitycounter = path.size() + talkingtime;
					activitycounter = path.size() + talkingtime;
				}
				lastTalkingto = tempVal.second;

				if (euclid(this->pos.first, this->pos.second, workers[gridnumber][tempVal.second].pos.first, workers[gridnumber][tempVal.second].pos.second) <= sqrt(2))
				{
					std::cout << this->name << " talking with " << workers[gridnumber][tempVal.second].name << '\n';
					//workers[gridnumber][tempVal].activity = talking;
					activity = Talking;
				}
			}

		}


		// Cannot talk and perform action at same time
		if (actionCheck)
		{
			lastTalkingto = index;
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
