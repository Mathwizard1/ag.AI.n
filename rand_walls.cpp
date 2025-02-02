#include "rand_walls.h"

using namespace std;

MazeGenerator::MazeGenerator(int rows, int cols, int minSize) {
    n = rows;
    m = cols;
    min_width = minSize;
    maze.resize(n, vector<short int>(m, 0));
}

void MazeGenerator::divideMaze(int x1, int y1, int x2, int y2) {

    if ((x2 - x1 < min_width) || (y2 - y1 < min_width)) {
        return;
    }

    bool horizDiv = (x2 - x1) > (y2 - y1);

    if (horizDiv) {
        int wallX = x1 + rand() % (x2 - x1 - 1) + 1;
        int passageY = y1 + rand() % (y2 - y1);

        for (int y = y1; y < y2; y++) {
            if (y != passageY) {
                maze[wallX][y] = -1;
            }
        }

        divideMaze(x1, y1, wallX, y2);
        divideMaze(wallX + 1, y1, x2, y2);
    }
    else {
        int wallY = y1 + rand() % (y2 - y1 - 1) + 1;
        int passageX = x1 + rand() % (x2 - x1);

        for (int x = x1; x < x2; x++) {
            if (x != passageX) {
                maze[x][wallY] = -1;
            }
        }

        divideMaze(x1, y1, x2, wallY);
        divideMaze(x1, wallY + 1, x2, y2);
    }
}

void MazeGenerator::addEntryExit() {
    vector<pair<int, int>> edges;
    for (int i = 1; i < n - 1; i++) {
        edges.push_back({ i, 0 });
        edges.push_back({ i, m - 1 });
    }
    for (int j = 1; j < m - 1; j++) {
        edges.push_back({ 0, j });
        edges.push_back({ n - 1, j });
    }

    int idx1 = rand() % edges.size();
    int idx2;
    do {
        idx2 = rand() % edges.size();
    } while (idx1 == idx2);

    maze[edges[idx1].first][edges[idx1].second] = 0;
    maze[edges[idx2].first][edges[idx2].second] = 0;
}

vector<vector<short int>> MazeGenerator::generateMaze(int seed) {
    srand(seed);
    divideMaze(0, 0, n , m );
    addEntryExit();

    return maze;
}