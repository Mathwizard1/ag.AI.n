#include "worker.h"
#include "food.h"

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

void Worker::eat(Food food) {
	this->energy += food.energy;
	this->productivity += food.productivity;

}

void Worker::give() {

}

void Worker::take() {

}