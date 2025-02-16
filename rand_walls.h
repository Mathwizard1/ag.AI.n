#pragma once
#ifndef RAND_WALLS_H
#define RAND_WALLS_H

#include "gameFunction.h"

class MazeGenerator {
private:
    enum class orientation
    {
        Horizontal,
        Vertical
    };

    std::vector<std::pair<short, short>> gridPath = {};

    orientation chooseOrientation(int width, int height);
    short int fullWidth, fullHeight, fraq, entrSize;
public:
    MazeGenerator(short int fullWidth, short int fullHeight, short int fraq, short entrSize = 0);

    orientation randOrient();
    void generateMaze(std::vector<std::vector<short int>> &maze, orientation wallOrientation, short int maxDepth, short int width, short int height, short int x = 0, short int y = 0, short int depth = 0);
};
#endif 
