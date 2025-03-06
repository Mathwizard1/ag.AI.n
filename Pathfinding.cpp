#include "Pathfinding.h"

std::vector<std::pair<short, short>> directions = { {-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1} };

float heuristic(int x1, int y1, int x2, int y2) {
	return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

vector<pair<short int, short int>> Pathfinding::getneighbors(pair<short int, short int> node, vector<vector<short int>>* grid)
{
	vector<pair<short int, short int>> neighbors;

	for (const auto& i : directions)
	{
		if (node.first + i.first < screenbuffer || (node.first + i.first) >= (*grid)[0].size() + screenbuffer)continue;
		if (node.second + i.second < screenbuffer || (node.second + i.second) >= grid->size() + screenbuffer)continue;

		if ((*grid)[node.second + i.second - screenbuffer][node.first + i.first - screenbuffer] != -1)
		{
			neighbors.push_back({ node.first + i.first ,node.second + i.second });

		}
	}

	return neighbors;

}

void Pathfinding::pathfind(pair<short int, short int> end,pair<short int,short int> pos, vector<vector<short int>>* grid, vector<pair<short int, short int>>* path)
{
	vector<pair<short int, short int>> neighbors;
	priority_queue<Node, vector<Node>, NodeComparison> open;
	vector<Node> closed;

	Node temp;
	temp.x = end.first;
	temp.y = end.second;
	temp.fcost = heuristic(end.first, end.second, pos.first, pos.second);
	temp.path = { end };
	open.push(temp);

	int counter = 0;
	while (true)
	{
		if (open.empty())
		{
			cout << "PATH NOT FOUND" << endl;
			return;
		}

		Node node = open.top();
		open.pop();

		if (node.x == pos.first && node.y == pos.second)
		{
			//cout << "NODE FOUND" << endl;
			temp = node;
			break;
		}

		neighbors = getneighbors({ node.x,node.y },grid);
		for (const auto& i : neighbors)
		{
			float f = heuristic(pos.first, pos.second, i.first, i.second) + heuristic(end.first, end.second, i.first, i.second);

			bool contflag = false;
			for (int j = 0; j < closed.size(); j++)
			{
				if (i.first == closed[j].x && i.second == closed[j].y)
				{
					if (f < closed[j].fcost)
					{
						Node n;
						n.x = i.first;
						n.y = i.second;
						n.fcost = f;
						n.gcost = node.gcost + 1;
						n.path = node.path;
						n.path.push_back({ node.x,node.y });
						open.push(n);
						closed.erase(closed.begin() + j);
						closed.push_back(n);

					}
					contflag = true;
					break;

				}
			}
			if (contflag)continue;
			else
			{
				Node n;
				n.x = i.first;
				n.y = i.second;
				n.fcost = f;
				n.gcost = node.gcost + 1;
				n.path = node.path;
				n.path.push_back({ node.x,node.y });
				open.push(n);
				closed.push_back(n);
			}
		}
	}

	*path = temp.path;
}