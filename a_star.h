#pragma once
#ifndef A_STAR_H
#define A_STAR_H

#include <vector>
#include <utility>

using namespace std;

class a_star {
public:
    a_star(const vector<vector<short>>& grid);

    vector<pair<short, short>> findPath(pair<short, short> start, pair<short, short> goal);

private:
    struct Node {
        short x, y;
        double gCost, hCost;
        Node* parent;

        Node(short x, short y, double g = 0, double h = 0, Node* p = nullptr)
            : x(x), y(y), gCost(g), hCost(h), parent(p) {
        }

        double fCost() const {
            return gCost + hCost;
        }

        bool operator<(const Node& other) const {
            return fCost() > other.fCost();
        }
    };

    vector<pair<short, short>> directions;
    const vector<vector<short>>& grid;

    bool isValid(short x, short y);
    double heuristic(short x1, short y1, short x2, short y2);

    
};

#endif


