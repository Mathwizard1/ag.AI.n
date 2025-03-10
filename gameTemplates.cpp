#include "gameTemplates.h"
#include <chrono>

void gameTemplates::gameSetup()
{
	if (!std::filesystem::exists(folderPath))
	{
		std::filesystem::create_directory(folderPath);
	}
}

std::vector<std::string> gameTemplates::loadTemplateNames()
{
	std::vector<std::string> allTemplates;

	for (const auto &entry : std::filesystem::directory_iterator(folderPath))
	{
		// Filepath
		//std::cout << std::filesystem::relative(entry.path(), folderPath).string() << '\n';
		std::string filename = std::filesystem::relative(entry.path(), folderPath).string();
		allTemplates.push_back(filename);
	}
	return allTemplates;
}

void gameTemplates::loadTemplate(const std::string filename, std::vector<char*> &textboxes, std::vector<short> &textsizes)
{
	textsizes.clear();
	for (auto& t : textboxes)
	{
		free(t);
	}
	textboxes.clear();
	
	std::ifstream tempFile(templateFolder + '\\' + filename);
	if (tempFile.is_open()) {
		std::string tempLine;
		std::cout << "file opened for reading\n";
		while (std::getline(tempFile, tempLine)) {
			int lineSize = tempLine.size() + 1; // include the '\0'

			lineSize = (lineSize < textinputsize) ? lineSize : textinputsize;

			char* lineBuffer = (char *)malloc(lineSize * sizeof(char));  // Dynamically allocate memory
			std::memcpy(lineBuffer, tempLine.c_str(), lineSize);  // Copy line to char* buffer

			textboxes.push_back(lineBuffer);  // Store in vector
			textsizes.push_back(lineSize);
		}
		tempFile.close();  // Close the file
	}
}

void gameTemplates::fileSaver(const std::string &filename, const std::vector<char*>& codeData)
{
	try
	{
		std::ofstream tempFile(templateFolder + '\\' + filename);
		if (tempFile.is_open()) {
			std::cout << "file opened for writing\n";
			std::this_thread::sleep_for(std::chrono::seconds(10));
			for (const char* line : codeData)
			{
				tempFile << line << '\n';
			}
			tempFile.close();
			std::cout << "File SAVE\n";
		}
		else
		{
			std::cerr << "File not SAVE\n";
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	fileSavingHandler[filename] = false;
}

void gameTemplates::saveTemplate(const std::vector<char*>& codeData)
{
	const std::string filename = c_filename;
	if (fileSavingHandler.find(filename) != fileSavingHandler.end())
	{
		if (fileSavingHandler[filename])
		{
			std::cout << "File being saved\n";
			return;
		}
	}

	fileSavingHandler[filename] = true;
	std::thread(&gameTemplates::fileSaver, this, filename, codeData).detach();
}

gameTemplates::gameTemplates()
{
	gameSetup();

	c_filename = (char *)malloc(sizeof(char) * filenamesize + 1);
	NameBuffer = (char*)malloc(sizeof(char) * filenamesize + 1);

	NameBuffer[0] = '>';
	updateFileName();
}

void gameTemplates::updateFileName()
{
	strcpy(c_filename, defFilename);
}

void gameTemplates::updateFileName(const char* filename)
{
	len = (sizeof(filename) < filenamesize) ? sizeof(filename) : filenamesize;
	
	for (i = 0; i < len; i++)
	{
		c_filename[i] = filename[i];
	}
	c_filename[i] = '\n';
}

gameTemplates::~gameTemplates()
{
	free(this->c_filename);
	free(this->NameBuffer);
}

