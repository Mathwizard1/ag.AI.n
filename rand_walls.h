#pragma once
#ifndef RAND_WALLS_H
#define RAND_WALLS_H

#include <vector>
#include "gameConst.h"

class MazeGenerator {
private:
    std::vector<std::vector<short int>> maze;

    void divideMaze(int x1, int y1, int x2, int y2);
    void addEntryExit();

public:
    int n, m, min_width;
    MazeGenerator(int rows, int cols, int minSize);
    std::vector<std::vector<short int>> generateMaze(int seed);
};
#endif 
