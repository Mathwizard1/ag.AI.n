#pragma once
#include "gameConst.h"
#include <raylib.h>
#include <filesystem>

using namespace std;

class animationHandler
{
public:
	vector<vector<Texture>> animations;
	vector<int> current_frame;
	vector<float> animation_time;
	vector<float> increment_time;
	vector<int> fps;
	vector<int> enable;
	vector<int> repetition;
	vector<vector<float>> animation_information;

	float prevtime=0;
	
	unordered_map<string, int> animation_names;
	
	void loadAnimation(string path,string name,int speed);
	void playStaticAnimation(string name, float x, float y, float scale, float rotation,bool repetition,float screenmode);
	void stopAnimation(string name);
	void stopAnimation(int i);
	void update(float newtime,int screenmode);
};

