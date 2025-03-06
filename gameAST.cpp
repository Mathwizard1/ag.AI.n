#include "gameAST.h"


gameGrammar::gameGrammar(gameLexers expected, std::unordered_set<gameObjects> gameObjs, bool bracketed, bool action)
{
    this->expected = expected;
    this->gameObjs = gameObjs;
    this->bracketd = bracketed;
    this->action = action;
}

// Definition and initialization of the language
std::unordered_map<std::string, gameGrammar> gameLanguage = {
    {"if", gameGrammar(gameLexers::expression, {gameObjects::Value}, true, false)},
    {"jump", gameGrammar(gameLexers::label, {gameObjects::Value}, false, false)},
   
    {"submit", gameGrammar()},
    {"exit", gameGrammar()},
    {"work", gameGrammar()},

    {"break", gameGrammar(gameLexers::literal, {gameObjects::Value})},

    {"moveto", gameGrammar(gameLexers::generic, {gameObjects::Person, gameObjects::Zone})},

    {"eat", gameGrammar(gameLexers::none, {gameObjects::Food})},
    {"buy", gameGrammar(gameLexers::generic, {gameObjects::Food})},

    {"give", gameGrammar(gameLexers::generic, {gameObjects::Food, gameObjects::Work}, false, false)},
    {"take", gameGrammar(gameLexers::generic, {gameObjects::Food, gameObjects::Work}, false, false)},
    {"talk", gameGrammar(gameLexers::generic, {gameObjects::Person}, false, false)},
};


