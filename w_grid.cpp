#include "w_grid.h"

vector<vector<short>> worker_pos;

bool is_0bstacle(std::pair<short int, short int> coordinate) {
    return worker_pos[coordinate.first][coordinate.second] == 1;
}

void init_worker_grid(short width=65, short height=65) {
    worker_pos = vector<vector<short>>(width, vector<short>(height, 0));
}