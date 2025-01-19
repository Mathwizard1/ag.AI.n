#include "worker.h"
#include "food.h"
#include "a_star.h"

a_star search_object;

void Worker::pathfind(pair<short int,short int> end)
{
	vector<pair<short int, short int>> temppath = { end };
	pair<short int, short int > start=this->pos;
	/*while (end != pos)
	{
		if (end.first < pos.first)end.first++;
		if (end.first > pos.first)end.first--;
		if (end.second < pos.second)end.second++;
		if (end.second > pos.second)end.second--;
		temppath.push_back(end);
	}*/
	path = search_object.findPath(start, end);
}



void Worker::eat(Food food) {
	this->energy += food.energy;
	this->productivity += food.productivity;
	this->money -= food.cost;
}

void Worker::give() {

}

void Worker::take() {

}