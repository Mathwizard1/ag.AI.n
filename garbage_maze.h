#ifndef GARBAGE_MAZE_H
#define GARBAGE_MAZE_H

#include <vector>

void ensureConnectivity(std::vector<std::vector<short int>>& grid);
void placeEmptyBlock(std::vector<std::vector<short int>>& grid, int blockWidth, int blockHeight);
std::vector<std::vector<short int>> generateMaze(int rows, int cols, int density);

#endif 

