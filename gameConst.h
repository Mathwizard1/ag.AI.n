#pragma once

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <string>
#include <math.h>
#include <unordered_set>
#include <unordered_map>
#include <random>

//Worker Constants
constexpr short int worktime = 20;
constexpr short int eattime = 10;
constexpr short int talkingtime = 5;

//Window
constexpr int windowwidth = 1600;
constexpr int windowheight = 900;

//Screen
constexpr int screenbuffer = 5;
constexpr int sidebarwidth =520;
constexpr int sidebarbuttonheight = 50;

//Moneybar
constexpr int moneybarwidth=150;

//Code
constexpr int namebannersize = 30;
constexpr int textinputheight = 60;
constexpr int codeblocks = 10;
constexpr int textinputsize = 35;
constexpr int textinputfontsize = 22;
constexpr int textsavebuttonheight = 40;

//Shop
constexpr int shopbuttonheight = 50;
constexpr int areashopboundarywidth = 10;

//Map
constexpr int mapdims = 10;
constexpr int zoneLimit = 10;

//Stocks
constexpr int stockperiod = 20;
constexpr int updatetime = 5;

//Bank
constexpr int fullreturn = 60;
constexpr int forwarddeposittime = 20;
constexpr int forwarddepositmaxterm = 800;
constexpr int maxforwarddeposits = 5;
constexpr int loaninteresttime = 120;
constexpr int bankbuttonheight = 50;

const std::string templateFolder = "./template";