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
};

typedef struct gameGrammar
{
	gameLexers expected;
	bool bracketd;
	bool action;

	gameGrammar(gameLexers expected = gameLexers::none, bool bracketed = false, bool action = true);
}gameGrammar;

// language
extern std::unordered_map<std::string, gameGrammar>  gameLanguage;

