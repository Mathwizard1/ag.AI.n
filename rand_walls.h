#pragma once
#ifndef RAND_WALLS_H
#define RAND_WALLS_H

#include <vector>
class MazeGenerator {
private:
    int n, m, min_width;
    std::vector<std::vector<int>> maze;

    void divideMaze(int x1, int y1, int x2, int y2);
    void addBoundaryWalls();
    void addEntryExit();

public:
    MazeGenerator(int rows, int cols, int minSize);
    std::vector<std::vector<int>> generateMaze();
    void printMaze(const std::vector<std::vector<int>>& maze) const;
};
#endif 
