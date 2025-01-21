#include "worker.h"
#include "food.h"
#include "a_star.h"
#include "w_grid.h"



void Worker::pathfind()
{
	a_star search_object;
	pair<short int, short int > end = this->end_point;
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