#pragma once
#include "gameFunction.h"

// Grammar of language
enum class gameLexers
{
	none,
	keywords,
	label,
	expression,
	generic,
	literal,
};

enum class gameObjects
{
	None = -1,
	Value,
	Person,
	Food,
	Work,
	Zone,
	Me,
};

typedef struct gameGrammar
{
	gameLexers expected;
	bool bracketd;
	bool action;
	std::unordered_set<gameObjects> gameObjs;

	gameGrammar(gameLexers expected = gameLexers::none, std::unordered_set<gameObjects> gameObjs = {gameObjects::None}, bool bracketed = false, bool action = true);
}gameGrammar;

// language
extern std::unordered_map<std::string, gameGrammar>  gameLanguage;