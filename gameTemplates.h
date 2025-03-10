#pragma once
#include "gameConst.h"
#include <filesystem>

// Templates
constexpr int filenamesize = 20;
const std::string templateFolder = ".\\template";
const int templateblocks = 10;

class gameTemplates
{
private:
	std::filesystem::path folderPath = templateFolder;
	const char* defFilename = "defaultTemplate.txt";

	std::unordered_map<std::string, bool> fileSavingHandler;

	void gameSetup();
	void fileSaver(const std::string &filename, const std::vector<char*>& codeData);
public:
	char* NameBuffer;
	char* c_filename;
	int i, len;

	gameTemplates();
	~gameTemplates();

	void updateFileName();
	void updateFileName(const char* filename);

	std::vector<std::string> loadTemplateNames();
	void loadTemplate(const std::string filename, std::vector<char*> &textboxes, std::vector<short> &textsizes);
	void saveTemplate(const std::vector<char *> &codeData);
};

