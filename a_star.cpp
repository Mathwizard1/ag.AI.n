#include "a_star.h"
#include <queue>
#include <cmath>
#include <algorithm>


double a_star::heuristic(int x1, int y1, int x2, int y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

vector<pair<int, int>> a_star::findPath(pair<int, int> start, pair<int, int> goal) {
    int rows = grid.size(), cols = grid[0].size();
    priority_queue<Node> front;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    front.push(Node(start.first, start.second, 0, heuristic(start.first, start.second, goal.first, goal.second)));

    while (!front.empty()) {
        Node current = front.top();
        front.pop();
        vector<pair<int, int>> path;
        if (current.x == goal.first && current.y == goal.second) {
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
            int nx = current.x + dir.first, ny = current.y + dir.second;
            if (nx >= 0 && nx < grid.size() && ny >= 0 && ny < grid[0].size() && grid[nx][ny] == 0 && !visited[nx][ny]) {
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
