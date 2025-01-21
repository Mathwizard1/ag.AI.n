#include "w_grid.h"

bool is_0bstacle(std::pair<short int, short int> coordinate) {
	if (worker_pos[coordinate.first][coordinate.second] == 1)
		return true;
}