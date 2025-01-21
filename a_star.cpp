#include "a_star.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include "w_grid.h"



a_star::a_star() {
    grid = worker_pos;
}

a_star::a_star(const vector<vector<short int>>& grid) : grid(grid) {
    directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1} };
}

bool a_star::isValid(short int x, short int y) {
    return x >= 0 && x < grid.size() && y >= 0 && y < grid[0].size() && grid[x][y] == 0;
}

double a_star::heuristic(short x1, short y1, short x2, short y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));//euclidian distance
}
//global worker grid 
//local worker grid
vector<pair<short int, short int>> a_star::findPath(pair<short int, short int> start, pair<short int, short int> goal) {
    short rows = grid.size(), cols = grid[0].size();
    priority_queue<Node> front;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    front.push(Node(start.first, start.second, 0, heuristic(start.first, start.second, goal.first, goal.second)));

    while (!front.empty()) {
        Node current = front.top();
        front.pop();

        if (current.x == goal.first && current.y == goal.second) {
            vector<pair<short int, short int>> path;
            while (current.parent != nullptr) {
                path.push_back({ current.x, current.y });
                current = *current.parent;
            }
            path.push_back({ current.x, current.y });
            reverse(path.begin(), path.end());
            return path;
        }

        if (visited[current.x][current.y]) continue;
        visited[current.x][current.y] = true;

        for (const auto& dir : directions) {
            short nx = current.x + dir.first, ny = current.y + dir.second;
            if (isValid(nx, ny) && !visited[nx][ny]) {
                double gCost = current.gCost + 1;
                if (dir.first != 0 && dir.second != 0) {
                    gCost += 0.414; //to compnsate diagonal
                }
                double hCost = heuristic(nx, ny, goal.first, goal.second);
                front.push(Node(nx, ny, gCost, hCost, new Node(current)));
            }
        }
    }
    return {};
}


bool operator==(const pair<short int, short int>& a, const pair<short int, short int>& b) {
    return a.first == b.first && a.second == b.second;
}