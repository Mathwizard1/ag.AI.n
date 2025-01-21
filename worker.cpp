#include "worker.h"
#include "food.h"
#include "a_star.h"

a_star search_object;

void Worker::pathfind()
{
	pair<short int, short int > end = this->end_point;
	vector<pair<short int, short int>> temppath = { end };
	pair<short int, short int > start=this->pos;

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