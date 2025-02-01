#ifndef GARBAGE_MAZE_H
#define GARBAGE_MAZE_H

#include <vector>

void ensureConnectivity(std::vector<std::vector<int>>& grid);
void placeEmptyBlock(std::vector<std::vector<int>>& grid, int blockWidth, int blockHeight);
std::vector<std::vector<int>> generateMaze(int rows, int cols, int density);

#endif 

