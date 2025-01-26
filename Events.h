#pragma once
#include "gameConst.h"
#include "Events.h"


class Events
{
private:
	int *timeFrame;
	std::unordered_map<std::string, std::pair<bool, std::string>> pastRecords;

public:
	Events();

};

