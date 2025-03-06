#pragma once
#include "gameConst.h"
#include <queue>
using namespace std;

class Pathfinding
{
	public:

		struct Node {
			short int x, y;
			float fcost;
			float gcost;
			vector<pair<short int, short int>> path;
		};
		struct NodeComparison {
			bool operator()(Node const& node1, Node const& node2)
			{
				return (node1.fcost > node2.fcost);
			}
		};

		void pathfind(pair<short int, short int> end,pair<short int,short int> pos, vector<vector<short int>>* grid, vector<pair<short int, short int>>* path);
		vector<pair<short int, short int>> getneighbors(pair<short int, short int> node, vector<vector<short int>>* grid);
};

