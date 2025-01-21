#include "w_grid.h"

vector<vector<short>> worker_pos;

void init_worker_grid(short width, short height) {
    worker_pos = vector<vector<short>>(width, vector<short>(height, 0));
}

bool is_0bstacle(std::pair<short int, short int> coordinate) {
    if (coordinate.first >= 0 && coordinate.first < worker_pos.size() &&
        coordinate.second >= 0 && coordinate.second < worker_pos[0].size()) {
        return worker_pos[coordinate.first][coordinate.second] == 1;
    }
    return true; 
}

