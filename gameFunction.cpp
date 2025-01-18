#include "gameFunction.h"
using namespace std;


pair<bool, string> gameFunction::NameGenerator() {
	static vector<string> malenames;
	static vector<string> femalenames;
	string name;
	ifstream file("malenames.txt");
	while (getline(file, name)) {
		malenames.push_back(name);
	}
	file.close();
	ifstream file("femalenames.txt");
	while (getline(file, name)) {
		femalenames.push_back(name);
	}
	if (rand() / (float)(RAND_MAX) > 0.5) {
		int ind = (int)(rand() / (double)RAND_MAX * femalenames.size());
		return { 1, femalenames[ind] };
	}
	int ind = (int)(rand() / (double)RAND_MAX * malenames.size());
	return { 0, malenames[ind] };
}
