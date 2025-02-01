#include "gameFunction.h"

float totalmoney = 0;
float moneyincrement = 1000;
float frametime;

short int chosengrid = 0;
long long int quota = 100000;
short int chosenperson = 0;

vector<string> malenames;
vector<string> femalenames;

pair<bool, char*> gameFunction::NameGenerator() {
	if (malenames.size() == 0)
	{
		string name;
		ifstream file("./names/malenames.txt");
		while (getline(file, name)) {
			malenames.push_back(name);
		}
		file.close();
		ifstream file2("./names/femalenames.txt");
		while (getline(file2, name)) {
			femalenames.push_back(name);
		}
		file2.close();

	}

	char* name = (char*)malloc(sizeof(char) * 20);

	if (rand() / (float)(RAND_MAX) > 0.5) {
		int ind =rand()%femalenames.size();
		return { 1, strcpy(name, femalenames[ind].c_str()) };
	}
	int ind = rand() % malenames.size();
	return { 0, strcpy(name, malenames[ind].c_str()) };
}
