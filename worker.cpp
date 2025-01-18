#include "worker.h"
#include "food.h"

void Worker::pathfind()
{

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