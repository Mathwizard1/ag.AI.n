#pragma once
#include <vector>
#include <utility>

using namespace std;


extern vector<vector<short>> worker_pos;

bool is_0bstacle(pair<short int, short int> coordinate);

void init_worker_grid(short int width, short int height);