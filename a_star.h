#pragma once
#ifndef A_STAR_H
#define A_STAR_H

#include <vector>
#include <utility>

using namespace std;

class a_star {
public:
    a_star(const vector<vector<int>>& grid);

    vector<pair<int, int>> findPath(pair<int, int> start, pair<int, int> goal);

private:
    struct Node {
        int x, y;
        double gCost, hCost;
        Node* parent;

        Node(int x, int y, double g = 0, double h = 0, Node* p = nullptr)
            : x(x), y(y), gCost(g), hCost(h), parent(p) {
        }

        double fCost() const {
            return gCost + hCost;
        }

        bool operator<(const Node& other) const {
            return fCost() > other.fCost();
        }
    };

    vector<pair<int, int>> directions;
    const vector<vector<int>>& grid;

    bool isValid(int x, int y);
    double heuristic(int x1, int y1, int x2, int y2);
};

#endif


