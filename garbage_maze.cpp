#include "garbage_maze.h"
#include <iostream>
#include <queue>
#include <random>

void ensureConnectivity(std::vector<std::vector<int>>& grid) {
    int rows = grid.size(), cols = grid[0].size();
    std::queue<std::pair<int, int>> q;
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));

    int startX = -1, startY = -1;
    for (int y = 1; y < rows; y += 2) {
        for (int x = 1; x < cols; x += 2) {
            if (grid[y][x] == 0) {
                startX = x;
                startY = y;
                break;
            }
        }
        if (startX != -1) break;
    }

    q.push({ startX, startY });
    visited[startY][startX] = true;
    std::vector<int> dx = { 0, 0, -1, 1 };
    std::vector<int> dy = { -1, 1, 0, 0 };

    while (!q.empty()) {
        std::pair<int, int> current = q.front();
        q.pop();
        int x = current.first;
        int y = current.second;
        for (int i = 0; i < 4; ++i) {
            int nx = x + dx[i], ny = y + dy[i];
            if (nx >= 0 && ny >= 0 && nx < cols && ny < rows && grid[ny][nx] == 0 && !visited[ny][nx]) {
                visited[ny][nx] = true;
                q.push({ nx, ny });
            }
        }
    }

    for (int y = 1; y < rows; y += 2) {
        for (int x = 1; x < cols; x += 2) {
            if (grid[y][x] == 0 && !visited[y][x]) {
                for (int i = 0; i < 4; ++i) {
                    int nx = x + dx[i], ny = y + dy[i];
                    if (nx >= 0 && ny >= 0 && nx < cols && ny < rows && visited[ny][nx]) {
                        grid[(y + ny) / 2][(x + nx) / 2] = 0;
                        visited[y][x] = true;
                        break;
                    }
                }
            }
        }
    }
}
//some empt block(relaxation)
void placeEmptyBlock(std::vector<std::vector<int>>& grid, int blockWidth, int blockHeight) {
    int rows = grid.size(), cols = grid[0].size();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distX(1, (cols - blockWidth) / 2);
    std::uniform_int_distribution<int> distY(1, (rows - blockHeight) / 2);

    int startX = distX(gen) * 2;
    int startY = distY(gen) * 2;

    for (int y = startY; y < startY + blockHeight; y += 2) {
        for (int x = startX; x < startX + blockWidth; x += 2) {
            grid[y][x] = 0;
        }
    }
}

std::vector<std::vector<int>> generateMaze(int rows, int cols, int density) {
    std::vector<std::vector<int>> grid(rows, std::vector<int>(cols, 0));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (y % 2 == 0 || x % 2 == 0) {
                if (rand() % 100 < density) grid[y][x] = -1;
            }
        }
    }

    ensureConnectivity(grid);

    int blockWidth = (cols / 8) * 2;
    int blockHeight = (rows / 8) * 2;

    for (int i = 0; i < 4; ++i) {
        placeEmptyBlock(grid, blockWidth, blockHeight);
    }

    for (int i = 0; i < grid.size(); ++i) {
        if (i != 1 && i != grid.size() - 2) {
            grid[i][0] = -1;
            grid[i][grid[0].size() - 1] = -1;
        }
    }
    for (int i = 0; i < grid[0].size(); ++i) {
        if (i != 1 && i != grid[0].size() - 2) {
            grid[0][i] = -1;
            grid[grid.size() - 1][i] = -1;
        }
    }

    grid[1][0] = 0;
    grid[grid.size() - 2][grid[0].size() - 1] = 0;

    return grid;
}
