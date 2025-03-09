#include <raylib.h>
#include "Raygui.h"
#include <chrono>
#include "randgen.h"
#include "worker.h"
#include "Bank.h"
#include "garbage_maze.h"
#include "rand_walls.h"

using namespace std;

short int gridheight;
short int gridwidth;
float linewidth;
float lineheight;

Font codingfont;
Font codingfontbold;

//BANK
Bank bank(0.06, 0.1);

enum sidebarstate {
	Code,
	Shop,
	Hire,
	Money,
} SidebarState;

enum codestate {
	MainCode,
	Template
} CodeState;

enum shopstate {
	Area,
	Misc
} ShopState;

enum bankstate {
	Invest,
	FD,
	Loan
} BankState;

enum screenmode {
	View,
	Map,
	Stock,
	SkillTree
} ScreenMode;

enum marketcondition {
	Competitive,
	Monopoly,
	Oligopoly
} MarketCondition;

typedef struct skilltreenode {
	const char* name;
	const char* description;
	long long int cost;
	bool unlocked;
	int key;
	vector<skilltreenode*> children;


	skilltreenode(const char* iname, long int icost, bool iunlocked, int ikey,const char* idesc)
	{
		name = iname;
		cost = icost;
		unlocked = iunlocked;
		key = ikey;
		description = idesc;
	}

}SkillTreeNode;

//Skill Tree
SkillTreeNode dummynode{ "Dummy",0,true,-1,""};
SkillTreeNode skilltreeroot{"New Beginnings",10000,false,0,"Unlock The Skill Tree"};
SkillTreeNode* chosenskilltreenode = &skilltreeroot;
vector<SkillTreeNode> skilltreenodes;
vector<float> skilltreenodedistances;
short int skilltreekey = 0;

//Code Variables
std::vector<vector<char*>> textboxesnetwork;
std::vector<vector<short int>> textsizesnetwork;
std::vector<char*> textboxes;
std::vector<short int> textsizes;

//Stock Variables
std::vector<double> playerstock = {};

//Grid Variables
std::vector<short int> gridpurchased;
std::vector<long long int> gridprices;
std::vector<std::vector<std::vector<short int>>> pooledgridnetwork;
std::vector<std::vector<std::vector<short int>>> gridnetwork;
std::vector<std::vector<short int>> grid;

//Area Variables
std::vector<tuple<Color, char*, int>> areaitems;
std::vector<short int> areacolors;

//Misc Variables
std::vector<tuple<Color, char*, int>> miscitems;
std::vector<short int> misccolors;
std::vector<pair<pair<short int, short int>,bool>> lunchpositions;

std:: vector<tuple< Color,char*, int>> workertypes;

std::vector<RandomGenerator> competitors;

short int workbenchsize;
Color workbenchcolor;

Vector2 mousepos;

Texture officetile;
Texture bankicon;
Texture stock_background;
Texture screw_head;
Texture sticky_note;

/*
Colors:
1->Lunch Area (Yellow)
2->Work Area (Green)
3->Reception (Blue)
4->Boss (Red)
5->Chef (Light Yellow)
6->Boss (Maroon)
7->Receptionist (Dark Blue)

-1->Boundary
-2->Workspace
*/

const char* GetEnumEquivalent(Worker::activitytype act)
{
	switch (act)
	{
	case Worker::Working:
		return "Working";
	case Worker::Moving:
		return "Moving";
	case Worker::Eating:
		return "Eating";
	case Worker::Idle:
		return "Idle";
	case Worker::Talking:
		return "Talking";
	case Worker::Taking:
		return "Taking";
	case Worker::Giving:
		return "Giving";
	case Worker::Buying:
		return "Buying";
	default:
		return "No Expression Found";
	}
}

const char* GetMarketEnumEquivalent()
{
	switch (MarketCondition)
	{
	case Monopoly:
		return "Monopoly";
	case Oligopoly:
		return "Oligopoly";
	case Competitive:
		return "Competitive";
	}
}

vector<float> GetFDStats(float input)
{
	return { 4+4*(input/quota),(input / quota) * forwarddepositmaxterm};
}

void BankUpdate()
{
	//Interest
	bank.update_investment();

	//Loan Interest
	bank.apply_loan_interest();

	//Forward Deposits
	for (int x = 0; x < bank.forward_deposit_amount.size(); x++)
	{
		if (bank.forward_deposit_term[x] > 0)
		{
			bank.forward_deposit_term[x]--;
		}
	}
}

void ChangeWorkerPositions()
{
	for(int y=0;y<gridnetwork.size();y++)
	for (int x = 0; x < workers[y].size(); x++)
	{
		if (workers[y][x].path.size() > 0)
		{
			workers[y][x].pos = workers[y][x].path[workers[y][x].path.size() - 1];
			workers[y][x].path.pop_back();
		}
	}
}

void WorkerCodeUpdate()
{
	for (int i = 0; i < workers.size(); i++)
	{
		for (int j = 0; j < workers[i].size(); j++)
		{
			if (workers[i][j].activitycounter > 0)
			{
				//Decrement Activity Counter
				workers[i][j].activitycounter--;

				//If Activity Ends
				if (workers[i][j].activitycounter == 0)
				{
					switch (workers[i][j].activity)
					{
					case Worker::Working:
						totalmoney += 100;
							workers[i][j].energy -= 20;
							workers[i][j].productivity -= 40;
							break;
					case Worker::Eating:
						workers[i][j].energy += 60;
						workers[i][j].productivity += 10;
						lunchpositions[workers[i][j].occupiedbench].second =false ;
						workers[i][j].occupiedbench = -1;
						break;
					case Worker::Moving:
						break;
					}

					workers[i][j].activity = Worker::Idle;
					//workers[i][j].linecounter++;
				}

			}
			else
			{
				//Interpret Next Command
				workers[i][j].callFunction();
			}
		}
	}
}

void InitializeSkillTree()
{
	SkillTreeNode* node1 = new SkillTreeNode("No Hunger!",2000,false,1,"Worker Productivity +5");
	SkillTreeNode* node2 = new SkillTreeNode("Node2", 20000, false, 2,"Yap1");
	SkillTreeNode* node3 = new SkillTreeNode("Node3", 20000, false, 3,"Yap2");
	SkillTreeNode* node4 = new SkillTreeNode("Node4", 20000, false, 4,"Yap3");
	SkillTreeNode* node9 = new SkillTreeNode("Node9", 80000, false, 9, "Yap7");
	skilltreeroot.children = { node1,node2,node3,node9 };
	skilltreenodedistances = { 50,50,50,50 };

	SkillTreeNode* node5 = new SkillTreeNode("Node5", 50000, false, 5,"Yap4");
	SkillTreeNode* node6 = new SkillTreeNode("Node6", 50000, false, 6,"Yap5");
	SkillTreeNode* node7 = new SkillTreeNode("Node7", 50000, false, 7,"Yap6");
	SkillTreeNode* node8 = new SkillTreeNode("Node8", 80000, false, 8,"Yap7");
	node1->children = { node5 };
	node5->children = { node8 };
	node2->children = { node6,node7};

}

void InitializeSprites()
{
	officetile = LoadTextureFromImage(LoadImage( "sprites/office_tile.png"));
	bankicon = LoadTextureFromImage(LoadImage("sprites/Bank.png"));
	stock_background= LoadTextureFromImage(LoadImage("sprites/stock_background.jpg"));
	screw_head= LoadTextureFromImage(LoadImage("sprites/screw_head.png"));
	sticky_note= LoadTextureFromImage(LoadImage("sprites/sticky_note.png"));
}

void InitializeHire()
{
	workertypes.push_back({ DARKPURPLE,(char*)"Software Developer",20 });
	workertypes.push_back({ DARKGREEN,(char*)"Hardware Developer",30 });
	workertypes.push_back({ ORANGE,(char*)"Network Engineer",50 });
}

void InitializeShop()
{
	areaitems.push_back({ YELLOW,(char*)"Lunch Area",30 });
	areacolors.push_back(1);
	areaitems.push_back({ RED,(char*)"Boss Area",100 });
	areacolors.push_back(4);
	areaitems.push_back({ GREEN,(char*)"Work Area",10});
	areacolors.push_back(2);
	areaitems.push_back({ BLUE,(char*)"Reception Area",50 });
	areacolors.push_back(3);

	miscitems.push_back({ {225,225,50,255},(char*)"Chef",200 });
	misccolors.push_back(5);
	miscitems.push_back({ MAROON,(char*)"Boss",500 });
	misccolors.push_back(6);
	miscitems.push_back({ DARKBLUE,(char*)"Receptionist",500 });
	misccolors.push_back(7);
	miscitems.push_back({ MAROON,(char*)"____",500 });
	misccolors.push_back(8);
}

void InitializeGridPrices()
{
	static int dims = sqrt(mapsize);
	static int midx = dims/2;
	static int midy = dims/ 2;

	int x;
	int y;

	for (int i = 0; i < mapsize; i++)
	{
		x = i % dims;
		y = i / dims;

		switch (abs(midx - x) + abs(midy - y))
		{
		case 0:
			gridprices[x + y * dims] = 0;
			break;
		case 1:
			gridprices[x + y * dims] = 500;
			break;
		case 2:
			gridprices[x + y * dims] = 5000;
			break;
		case 3:
			gridprices[x + y * dims] = 10000;
			break;
		case 4:
			gridprices[x + y * dims] = 50000;
			break;
		case 5:
			gridprices[x + y * dims] = 100000;
			break;
		}
	}
}

void InitializeGrid(short int width, short int height, int type)
{
	static int mapseed = 1;
	static int dims = sqrt(mapsize);
	static int midx = dims / 2;
	static int midy = dims / 2;

	workbenchcolor = { 120,255,120,255 };
	workbenchsize = 3;

	//grid = generateMaze(width, height,1 );
	MazeGenerator generator(width, height, 5);

	for (int z = 0; z < mapsize ; z++)
	{
		int x = z % dims;
		int y = z / dims;
		int walldensity = abs(midx - x) + abs(midy - y);

		generator.generateMaze(grid, generator.randOrient(), walldensity, width, height);

		vector<char*> textboxes;
		vector<short int> textsizes;
		vector<Worker> gridworkers;
		gridprices.push_back(0);
		gridpurchased.push_back(0);
		gridnetwork.push_back(grid);
		textboxesnetwork.push_back(textboxes);
		textsizesnetwork.push_back(textsizes);
		workers.push_back(gridworkers);
		 
		vector<vector<short int>> pooledgrid;
		for (int y = 0; y < grid.size() - 1; y += grid.size()/mapdims)
		{
			vector<short int> row;
			for (int x = 0; x <grid[0].size() -1; x += grid[0].size() / mapdims)
			{
				vector<int> numarray(9);

				for (int q = 0; q < grid.size() / mapdims; q++)
				{
					for (int r = 0; r < grid[0].size() / mapdims; r++)
					{
						//cout << grid[y + q][x + r] << endl;
						if(grid[y+q][x+r]>=-2)
						numarray[grid[y+q][x+r]+2]++;
					}
				}
				
				int max = 0;
				if (numarray[1] > 0)
					max = 1;
				else
				for (int q = 0; q < 9; q++)
				{
					if (numarray[q] > numarray[max])
						max = q;
				}
				row.push_back(max-2);
			}

			pooledgrid.push_back(row);
		}
		pooledgridnetwork.push_back(pooledgrid);
	}
	gridpurchased[mapsize / 2] = 1;
	grid = gridnetwork[mapsize / 2];

	gridheight = grid.size() + 2 * screenbuffer;
	gridwidth = grid[0].size() + 2 * screenbuffer;
	linewidth = (windowwidth - sidebarwidth-moneybarwidth) / (float)gridwidth;
	lineheight = (windowheight) / (float)gridheight;
}

void DrawStocks()
{
	static float rectx = windowwidth * 0.2;
	static float recty = windowheight * 0.2;
	static float axessize = 450;
	Vector2 startpoint = { rectx + 100, windowheight * 0.8f - 50 };
	int xstart = (totalticks/updatetime-stockperiod<0)?0:totalticks/updatetime - stockperiod;


	//Draw Background
	DrawRectangleLinesEx({ 0,0,windowwidth,windowheight }, 20, GRAY);
	DrawTextureEx(stock_background, { 0,0, }, 0, windowwidth / (double)1920, WHITE);

	//Draw Screws
	DrawTextureEx(screw_head, { windowwidth*0.12, windowheight*0.1 },0,0.2, WHITE);
	DrawTextureEx(screw_head, { windowwidth*0.12, windowheight*0.8}, 0, 0.2, WHITE);
	DrawTextureEx(screw_head, { windowwidth*0.85, windowheight*0.1 }, 0, 0.2, WHITE);
	DrawTextureEx(screw_head, { windowwidth * 0.85, windowheight*0.8 }, 0, 0.2, WHITE);

	//Draw Sticky Note
	DrawTextureEx(sticky_note, {windowwidth*0.52,windowheight*0.27},-10,1.5, WHITE);
	DrawTextEx(codingfontbold, "Competitors", { windowwidth * 0.7,windowheight * 0.34 }, 28, 1, BLACK);

	//Draw Rectangles
	DrawRectangle(rectx-20, recty, windowwidth*0.4, windowheight * 0.6+20, BLUE);
	DrawRectangle(rectx, recty + 20, windowwidth*0.4-40 , windowheight * 0.6-20 , WHITE);

	//Draw Market Condition
	DrawTextEx(codingfontbold, TextFormat("Market Condition: "), { windowwidth*0.35,windowheight*0.14 }, 30, 1, BLACK);
	DrawTextEx(codingfontbold, TextFormat("%s",GetMarketEnumEquivalent()), { windowwidth * 0.55,windowheight * 0.14 }, 30, 1, RED);

	//Draw Competitors Stocks
	for (int i = 0; i < competitors.size(); i++)
	{
		if (competitors[i].competitor.size() > 1)
		{
			for (int j = xstart; j < competitors[i].competitor.size() - 1; j++)
			{
				//STOCK LINES
				DrawLineEx({ startpoint.x + (j - xstart) * axessize / stockperiod,startpoint.y - (float)((competitors[i].competitor[j] - minstock) / (maxstock - minstock)) *axessize }, { startpoint.x + (j - xstart + 1) * (axessize) / stockperiod,startpoint.y - (float)((competitors[i].competitor[j + 1] - minstock) / (maxstock - minstock)) * axessize }, 5, RED);
			}
			if (competitors[i].competitor[competitors[i].competitor.size() - 1] > maxstock)
			{
				maxstock = competitors[i].competitor[competitors[i].competitor.size() - 1]*1.4;
			}

			DrawTextEx(codingfontbold, TextFormat("Competitor %d: $%0.f", i + 1, competitors[i].competitor[competitors[i].competitor.size() - 1]), { windowwidth * 0.67,windowheight*0.35f+windowheight * 0.05f * (i + 1) }, 22, 1, BLACK);
		}
	}

	//Draw Player Stocks
	if(playerstock.size()>0)
	for (int j = xstart; j < playerstock.size() - 1; j++)
	{
		//STOCK LINES
		DrawLineEx({ startpoint.x + (j - xstart) * axessize / stockperiod,startpoint.y - (float)((playerstock[j] - minstock) / (maxstock - minstock)) * axessize}, {startpoint.x + (j - xstart + 1) * axessize / stockperiod,startpoint.y - (float)((playerstock[j+1] - minstock) / (maxstock - minstock)) * axessize}, 5, GREEN);
	}

	//Labels
	DrawTextEx(codingfontbold, TextFormat("$%0.f", maxstock), { startpoint.x-80 , startpoint.y-axessize }, 22,1, GREEN);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", minstock), {startpoint.x-80 , startpoint.y }, 22, 1, GREEN);
	DrawTextEx(codingfont, "$", {startpoint.x-80, startpoint.y-axessize*0.6f}, 50, 1, GREEN);

	//Draw Axes
	DrawCircle(startpoint.x, startpoint.y,10, BLACK);
	DrawLineEx(startpoint, { startpoint.x,startpoint.y - axessize }, 10, BLACK);
	DrawLineEx(startpoint, { startpoint.x+axessize,startpoint.y }, 10, BLACK);
	
	GuiDrawIcon(ICON_ARROW_UP_FILL, startpoint.x-22, startpoint.y - axessize-20, 3, BLACK);
	GuiDrawIcon(ICON_ARROW_RIGHT_FILL, startpoint.x + axessize-20, startpoint.y-25, 3, BLACK);


	//Draw Circle
	DrawCircle(80, 80, 50, BLACK);
	if (mousepos.x < 130 && mousepos.y < 130)
		if (CheckCollisionPointCircle(mousepos, { 80,80 }, 50))
		{
			DrawCircle(80, 80, 45, { 0,171,255,255 });
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				ScreenMode = Map;
				return;
			}
		}
		else
			DrawCircle(80, 80, 45, { 0,151,241,255 });
	else
		DrawCircle(80, 80, 45, { 0,151,241,255 });

	GuiDrawIcon(ICON_EXIT, 48, 48, 4, WHITE);

}

void DrawMap()
{
	static int mapnumber = gridnetwork.size();
	static int dims = sqrt(mapnumber);

	//Draw Background and Boundary
	DrawRectangleRec({ 0, 0, windowwidth, windowheight }, RAYWHITE);
	DrawRectangleLinesEx({ 0, 0, windowwidth, windowheight },20, GRAY);

	//Draw Total Money
	DrawTextEx(codingfontbold, TextFormat("$%0.f", totalmoney), { windowwidth - windowwidth/12, 50 }, 30, 1, GREEN);

	//Draw Lines
	for (int y = 0; y < dims; y++)
	{

		DrawLineEx({ windowwidth / 6,windowheight / 6 + (float)y * (2 * windowheight / 3) / (dims - 1) }, { 5 * windowwidth / 6, windowheight / 6 + (float)y * (2 * windowheight / 3) / (dims - 1) }, 8, DARKBLUE);
		DrawLineEx({ windowwidth / 6 + (float)(y) * (2 * windowwidth / 3) / (dims - 1),windowheight / 6 }, { windowwidth / 6 + (float)y * (2 * windowwidth / 3) / (dims - 1) ,5 * windowheight / 6 }, 8, DARKBLUE);
	}

	//Stock Button
	DrawCircle(75, 75, 50, BLACK);
	if (CheckCollisionPointCircle(mousepos, { 75,75 }, 45))
	{
		DrawCircle(75, 75, 45, { 0,171,255,255 });
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			ScreenMode = Stock;
		}
	}
	else
	DrawCircle(75, 75, 45, { 0,151,241,255 });
	GuiDrawIcon(ICON_WAVE_TRIANGULAR, 43, 43, 4, WHITE);

	//Skill Tree Button
	DrawCircle(75, 180, 50, BLACK);
	if (CheckCollisionPointCircle(mousepos, { 75,180 }, 45))
	{
		DrawCircle(75, 180, 45, { 0,171,255,255 });
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			ScreenMode = SkillTree;
		}
	}
	else
		DrawCircle(75, 180, 45, { 0,151,241,255 });
	GuiDrawIcon(ICON_ARROW_UP_FILL,45, 148, 4, GREEN);


	float rad = (windowheight - 200) / (2 * dims);
	float radmap = rad / mapdims;
	//Draw Circles and Grids
	for (int y = 0; y < dims; y++)
	{
		for (int x = 0; x < dims; x++)
		{
			float xpoint = windowwidth / 6 + x * (2 * windowwidth / 3) / (dims - 1);
			float ypoint = windowheight / 6 + y * (2 * windowheight / 3) / (dims - 1);

			//CIRCLES
			if (gridpurchased[x + y * (dims)] == 1)
			{
				DrawCircle(xpoint, ypoint, rad + 100 / mapsize, BLUE);
				DrawCircle(xpoint, ypoint, rad, {0,245,245,245});
				DrawCircle(xpoint, ypoint, rad - 5, {0,205,205,205});
			}
			else
			{
				DrawCircle(xpoint, ypoint, rad + 100 / mapsize, RED);
				DrawCircle(xpoint, ypoint, rad, {250,250,0,255});
				DrawCircle(xpoint, ypoint, rad - 5, {200,20,0,255});
			}

			if (CheckCollisionPointCircle(mousepos, {xpoint,ypoint },rad))
			{
				short int gridnum = x + y * dims;
				if (gridpurchased[gridnum] == 1)
					DrawCircle(xpoint,ypoint,rad-5, { 0,225,225,225 });
				else
					DrawCircle(xpoint, ypoint, rad - 5, { 250,40,0,255 });
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					if (gridpurchased[gridnum] == 0)
					{
						if (gridprices[gridnum] <= totalmoney)
						{
							totalmoney -= gridprices[gridnum];
							gridpurchased[gridnum] = 1;
						}
					}
					else
					{
						grid = gridnetwork[gridnum];
						textboxes = textboxesnetwork[gridnum];
						textsizes = textsizesnetwork[gridnum];
						chosengrid =gridnum;
						ScreenMode = View;
					}
				}
			}


			//GRIDS
			if(gridpurchased[x+y*dims]==1)
			{
				Color blockcolor = WHITE;
				for (int j = 0; j < mapdims; j++)
				{
					for (int i = 0; i < mapdims; i++)
					{

						switch (pooledgridnetwork[x + y * dims][j][i])
						{
						case -1:
							blockcolor = BLACK;
							break;
						case 0:
							blockcolor = WHITE;
							break;
						case 1:
							blockcolor = YELLOW;
							break;
						case 2:
							blockcolor = GREEN;
							break;
						case 3:
							blockcolor = BLUE;
							break;
						case 4:
							blockcolor = RED;
							break;
						}
						//DONOT USE ROUNDED REC
						DrawRectangleRec({ xpoint - rad*0.5f + i * radmap+1,ypoint - rad*0.5f + j * radmap+1,radmap-2,radmap-2 }, blockcolor);
					}
				}
			}
			//PRICES
			else
			{
				DrawTextEx(codingfontbold, TextFormat("$%d", gridprices[x + y * dims]), { xpoint - rad / 3,ypoint - rad / 10 }, 20, 1, YELLOW);
			}
		}
	}

}

void DrawSkillTreeNode(Vector2 pos, SkillTreeNode* node,SkillTreeNode* parent)
{
	DrawCircle(pos.x, pos.y, skilltreenoderadius, BLACK);
	if(node->unlocked==true)
		DrawCircle(pos.x, pos.y, skilltreenoderadius*0.8,GREEN);
	else
		DrawCircle(pos.x, pos.y, skilltreenoderadius * 0.8, RED);

	if (CheckCollisionPointCircle(mousepos, pos, skilltreenoderadius))
	{
		chosenskilltreenode = node;
		if (node->unlocked == false)
		{
			DrawCircle(pos.x, pos.y, skilltreenoderadius * 0.8, ORANGE);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && node->cost < totalmoney&&parent->unlocked==true)
			{
				totalmoney -= node->cost;
				node->unlocked = true;
				skilltreekey = node->key;
				return;
			}
		}
		else
			DrawCircle(pos.x, pos.y, skilltreenoderadius * 0.8, { 0,255,48,255 });
	}
}

void TraverseTree(Vector2 pos,SkillTreeNode* node,float inputangle)
{
	if (node->children.empty())return;

	float angleincrement = skilltreenodeangle/ ((node->children.size()+1)* 57.2958);
	float angle = inputangle + angleincrement;

	for (int i = 0; i < node->children.size(); i++)
	{
		float newx = pos.x + skilltreenodedistances[i] * cos(angle);
		float newy = pos.y + skilltreenodedistances[i] * sin(angle);
		angle += angleincrement;

		DrawLineEx(pos, { newx,newy }, skilltreeedgethickness, BLACK);
		TraverseTree({newx,newy}, node->children[i], inputangle);
		DrawSkillTreeNode({ newx,newy }, node->children[i],node);
	}
}

void DrawSkillTree()
{
	//Exit Button
	DrawCircle(70, 70, 50, BLACK);
	if (mousepos.x < 120 && mousepos.y < 120)
	{
		if (CheckCollisionPointCircle(mousepos, { 70,70 }, 50))
		{
			DrawCircle(70, 70, 45, { 0,171,255,255 });
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				ScreenMode = Map;
			}
		}
		else
			DrawCircle(70, 70, 45, { 0,151,241,255 });
	}
	else
		DrawCircle(70, 70, 45, { 0,151,241,255 });
	GuiDrawIcon(ICON_EXIT, 38, 38, 4, WHITE);

	//Draw Tree
	Vector2 rootnodepos = {windowwidth *0.5, windowheight*0.4};

	float angle = 0;
	float angleincrement = 360 / ((double)skilltreeroot.children.size() * 57.2958);
	for (int i=0;i<skilltreeroot.children.size();i++)
	{
		float newx = rootnodepos.x+ skilltreenodedistances[i] * cos(angle);
		float newy = rootnodepos.y+ skilltreenodedistances[i] * sin(angle);

		DrawLineEx(rootnodepos, { newx,newy }, skilltreeedgethickness, BLACK);
		TraverseTree({newx,newy}, skilltreeroot.children[i], (-90 + i * 90) / 57.2958);
		DrawSkillTreeNode({ newx,newy }, skilltreeroot.children[i], &skilltreeroot);

		angle += angleincrement;
	}
	DrawSkillTreeNode(rootnodepos,&skilltreeroot,&dummynode);

	//Draw Description Box
	DrawRectangle(0, windowheight * 0.85, windowwidth, windowheight * 0.15, RED);

	//Draw Descriptions
	DrawTextEx(codingfontbold, chosenskilltreenode->name, { windowwidth * 0.5-60,windowheight * 0.87 }, 28, 1, YELLOW);
	DrawTextEx(codingfontbold, chosenskilltreenode->description, { windowwidth * 0.2,windowheight * 0.92 }, 24, 1, ORANGE);

	//Draw Icon
	GuiDrawIcon(ICON_ARROW_UP_FILL, windowwidth * 0.13, windowheight * 0.89, 5, GREEN);

	//Draw Cost
	DrawTextEx(codingfontbold, TextFormat("Cost: $%d", chosenskilltreenode->cost), { windowwidth * 0.7f,windowheight * 0.9f }, 27, 1, GREEN);
	DrawTextEx(codingfontbold, TextFormat("Available Money: $%0.f", totalmoney), { windowwidth * 0.7f,windowheight * 0.94f }, 27, 1, GREEN);
}

void DrawWorkers(float linewidth,float lineheight)
{
	for (int x = 0; x < workers[chosengrid].size(); x++)
	{
		Vector2 workerpos = { (float)(workers[chosengrid][x].pos.first * linewidth + linewidth / 2),(float)(workers[chosengrid][x].pos.second * lineheight + lineheight / 2) };
		DrawCircleV(workerpos, min(linewidth / 2, lineheight / 2) - 2, { 100,100,255,255 });
		DrawCircleLinesV(workerpos, min(linewidth / 2, lineheight / 2) - 2, BLACK);
		DrawTextEx(codingfontbold, TextFormat("%s(%d)", workers[chosengrid][x].name,workers[chosengrid][x].gender), {workerpos.x + linewidth / 3,workerpos.y - lineheight * 0.5f - 10.0f}, 18, 0, BLACK);

		if (CheckCollisionPointCircle(mousepos, workerpos, min(linewidth, lineheight)))
		{
			DrawCircleV(workerpos, min(linewidth / 2, lineheight / 2) - 2, { 200,100,255,255 });
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				chosenperson = x;
				SidebarState = Code;
			}
		}
	}
}

void DrawProgressBar()
{
	static Rectangle outerrect = { windowwidth - sidebarwidth - moneybarwidth+50, 50,50,windowheight-100 };
	static Rectangle boundary = { windowwidth - sidebarwidth - moneybarwidth, 0, moneybarwidth, windowheight };
	float fraction = totalmoney/ (float)quota;
	Rectangle innerrect = {outerrect.x+10,outerrect.y+(outerrect.height*(1-fraction))+10,outerrect.width -20 ,outerrect.height*fraction-20 };
	Rectangle shinerect = { outerrect.x +15,outerrect.y + (outerrect.height * (1 - fraction)) + 20,innerrect.width/10 ,outerrect.height * fraction - 40 };

	DrawRectangleRec(boundary, DARKGRAY);

	DrawRectangleRounded(outerrect,10,10,BLACK);
	DrawRectangleRounded(innerrect, 10, 10, GOLD);
	DrawRectangleRounded(shinerect, 10, 10, WHITE);
	DrawCircle(boundary.x + 10, boundary.y + 10, 5, GRAY);
	DrawCircle(boundary.x +moneybarwidth- 10, boundary.y + 10, 5, GRAY);
	DrawCircle(boundary.x + 10, windowheight - 10, 5, GRAY);
	DrawCircle(boundary.x + moneybarwidth - 10, windowheight- 10, 5, GRAY);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", totalmoney), { outerrect.x-15,outerrect.y+outerrect.height+10 }, 20, 4, YELLOW);
	DrawTextEx(codingfontbold, TextFormat("$%d", quota), { outerrect.x-20 ,outerrect.y - 20 }, 20, 4, YELLOW);
}

void DrawMainScreen()
{
	static float tilesize = linewidth / (float)40;

	//Draw Cells
	Color color=WHITE;
	for (int y = 0; y < gridheight; y++)
	{
		for (int x = 0; x < gridwidth; x++)
		{
			if ((x >= 0&&x<screenbuffer-1) || (x >= gridwidth-screenbuffer+1&&x<gridwidth) || (y >= 0 && y < screenbuffer-1) || (y >= gridheight -screenbuffer+1 &&y<gridheight))
				color = BROWN;
			else if (( x == screenbuffer-1) || (x == gridwidth - screenbuffer) || (y == screenbuffer-1) || (y == gridheight - screenbuffer))
					color =DARKBROWN;
			else
			switch (grid[y-screenbuffer][x-screenbuffer])
			{
			case 0:
				color = WHITE;
				break;
			case 1:
				color = YELLOW;
				break;
			case 2:
				color = GREEN;
				break;
			case 3:
				color = BLUE;
				break;
			case 4:
				color = RED;
				break;
			case 5:
				color = { 225,225,50,255 };
				break;
			case 6:
				color = MAROON;
				break;
			case 7:
				color = DARKBLUE;
				break;
			case -1:
				color = DARKBROWN;
				break;
			case -2:
				color = workbenchcolor;
				break;
			}

			if (((x >= 0 && x < screenbuffer)|| (x >= gridwidth - screenbuffer && x < gridwidth)) && y > (int)(gridheight * 0.4) && y < (int)(gridheight * 0.6))
				color = {0,205,255,255};

			//DrawTextureEx(officetile, { x * linewidth, y * lineheight }, 0, tilesize, color);
			DrawRectangleRec({ x * linewidth, y * lineheight, linewidth, lineheight }, color);
			
		}
	}

	//Draw Grid
	for (int x = 0; x < gridwidth; x++)
	{
		DrawLineEx({ x * linewidth, 0 }, { x * linewidth, windowheight }, 1, BLACK);
	}
	for (int y = 0; y < gridheight; y++)
	{
		DrawLineEx({ 0,y * lineheight }, { windowwidth - sidebarwidth-moneybarwidth,y * lineheight }, 1, BLACK);
	}

	//DRAW WORKERS
	DrawWorkers(linewidth,lineheight);

	//Draw Circle
	DrawCircle(70, 70, 50, BLACK);
	if (mousepos.x < 120 && mousepos.y < 120)
		if (CheckCollisionPointCircle(mousepos, { 70,70 }, 50))
		{
			DrawCircle(70, 70, 45, { 0,171,255,255 });
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				gridnetwork[chosengrid] = grid;

				//RECREATE POOLED MAP
				
				vector<vector<short int>> pooledgrid;
				for (int l = 0; l < grid.size() -1; l += grid.size() / mapdims)
				{
					vector<short int> row;
					for (int m = 0; m < grid[0].size()-1 ; m += grid[0].size() / mapdims)
					{
						//UPDATE
						vector<int> numarray(10);

						for (int q = 0; q < grid.size() / mapdims; q++)
						{
							for (int r = 0; r < grid[0].size() / mapdims; r++)
							{
								numarray[grid[l + q][m + r]+2]++;
							}
						}


						int min = 0;
						if (numarray[1] > 0)
							min = 1;
						else
						{
							for (int q = 0; q < 10; q++)
							{
								if (numarray[q] > numarray[min])
									min = q;
							}
						}

						row.push_back(min-2);
					}
					pooledgrid.push_back(row);
				}
				pooledgridnetwork[chosengrid] = pooledgrid;
				textboxesnetwork[chosengrid]=textboxes;
				textsizesnetwork[chosengrid] = textsizes;
				ScreenMode = Map;
				return;
			}
		}
		else
		DrawCircle(70, 70, 45, { 0,151,241,255 });
	else
	DrawCircle(70, 70, 45, { 0,151,241,255 });

	GuiDrawIcon(ICON_EXIT, 38, 38, 4, WHITE);

}

void DrawTemplateTab()
{
	//BOUNDARY
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), { 0,100,50,255 });
	DrawRectangleLinesEx({ windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight) }, areashopboundarywidth, { 220,220,220,220 });

	if (GuiButton({ windowwidth - sidebarwidth ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "Back"))
		CodeState = MainCode;
}

void DrawMainCodeTab()
{
	if (workers[chosengrid].empty())
	{
		DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), GRAY);
		DrawRectangle(windowwidth - sidebarwidth + 80, windowheight * 0.43, sidebarwidth - 160, 130, BLACK);
		DrawRectangle(windowwidth - sidebarwidth + 100, windowheight*0.45, sidebarwidth - 200, 100, RED);
		DrawTextEx(codingfontbold, "NO CODE SELECTED", { windowwidth - sidebarwidth + 120, windowheight * 0.5 }, 30, 2, WHITE);
		return;
	}

	static bool editing = false;
	static char text[textinputsize] = "";
	static short int textsize = 0;
	static short int startpos = 0;
	static short int chosenblock = 0;
	static short int chosentext = 0;
	static short int chosencode = 0;
	static double codeblockheight = (windowheight - sidebarbuttonheight - textinputheight-textsavebuttonheight-namebannersize) / (double)codeblocks;
	
	short int interpreterblock = workers[chosengrid][chosenperson].linecounter;

	//CODE CHANGE FOR DIFFERENT PERSON
	if (chosencode != chosenperson)
	{
		chosenblock = 0;
		startpos = 0;
		chosentext = 0;
		chosencode = chosenperson;
		textboxes = workers[chosengrid][chosenperson].code;
		textsizes = workers[chosengrid][chosenperson].linesize;
	}
	
	//CODE SUBMISSION
	if (GuiTextBox({ windowwidth - sidebarwidth,windowheight - textinputheight,sidebarwidth,textinputheight }, text, textinputsize, true))
	{
		char* sendtext=(char*)malloc(sizeof(char)*textinputsize);

		//Copy text to sendtext buffer
		for (int x = 0; x < textinputsize; x++)
		{
			sendtext[x] = text[x];
			if (text[x] == NULL)
			{
				textsize = x;
				break;
			}
			text[x] = NULL;
		}

		//Choose to edit text or enter new text
		if (chosentext < textboxes.size())
		{
			textboxes[chosentext] = sendtext;
			textsizes[chosentext] = textsize;
		}
		else
		{
			for (int x = textboxes.size(); x <=chosentext; x++)
			{
				char* temptext = (char*)malloc(sizeof(char) * textinputsize);
				temptext[0]='\0';
				textboxes.push_back(temptext);
				textsizes.push_back(0);
			}
			textboxes[chosentext] = sendtext;
			textsizes[chosentext] = textsize;
		}

		//Iterate Codeblocks
		if (chosenblock < codeblocks - 1)
		{
			editing = false;
			chosentext++;
			chosenblock++;
		}
		else if (textboxes.size() + 1 > codeblocks)
		{
			editing = false;
			chosentext++;
			startpos++;
		}
	}

	//EDITING MODE TOGGLE
	if (editing == false)
	{
		if (chosentext < textboxes.size())
		{
			for (int x = 0; x < textinputsize; x++)
			{
				text[x] = textboxes[chosentext][x];
			}
			editing = true;
		}
	}

	//CHANGING CURRENT BLOCK
	if (IsKeyPressed(KEY_UP) || (GetMouseWheelMove() > 0))
	{
		if (chosenblock == 0 && startpos > 0)
		{
			text[0] = '\0';
			editing = false;
			startpos -= 1;
			chosentext--;
		}
		else if (chosenblock > 0)
		{
			text[0] = '\0';
			editing = false;
			chosentext--;
			chosenblock--;
		}
	}
	else if (IsKeyPressed(KEY_DOWN) || (GetMouseWheelMove() < 0))
	{
		if (chosenblock == codeblocks - 1)
		{
			text[0] = '\0';
			editing = false;
			chosentext++;
			startpos += 1;
		}
		else
		{
			text[0] = '\0';
			editing = false;
			chosentext++;
			chosenblock++;
		}
	}

	//DRAWING CODEBLOCKS
	for (int i = 0; i < codeblocks; i++)
	{
		Color blockcolor = (i == chosenblock) ? GREEN : Color{0,150,255,255};

		if (startpos+i+1 == interpreterblock)
			blockcolor = DARKBLUE;
		if (startpos + i  == interpreterblock&& workers[chosengrid][chosenperson].errorFound)
			blockcolor = RED;

		Rectangle codeblock = { windowwidth - sidebarwidth, sidebarbuttonheight +namebannersize+ i * codeblockheight, sidebarwidth, codeblockheight };
		DrawRectangleRec(codeblock,blockcolor);
		DrawRectangle(codeblock.x, codeblock.y, 55, codeblockheight, GRAY);
		DrawRectangleLinesEx(codeblock, 2, DARKBLUE);
		if (textboxes.size() > startpos + i)
		{
			DrawTextEx(codingfontbold, textboxes[startpos + i], { codeblock.x + sidebarwidth / 7, (float)(codeblock.y + codeblockheight*0.4) }, textinputfontsize-2*(textsizes[startpos+i]/12), 3, WHITE);
		}
		DrawTextEx(codingfontbold, TextFormat("%d", (startpos + i)), { (float)codeblock.x + 10, (float)(codeblock.y + codeblockheight / 2.5) }, textinputfontsize-5,3, WHITE);

		if (CheckCollisionPointRec(mousepos, codeblock) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			text[0] = '\0';
			chosenblock = i;
			chosentext = i + startpos;
			editing = false;
		}
	}

	//DRAWING SAVE BUTTON
	if (GuiButton({ windowwidth - sidebarwidth,windowheight - textinputheight - textsavebuttonheight,sidebarwidth / 5,textsavebuttonheight }, "SAVE"))
	{
		workers[chosengrid][chosenperson].code = textboxes;
		workers[chosengrid][chosenperson].linesize = textsizes;
		workers[chosengrid][chosenperson].getCode();
	}
	//DRAWING TEMPLATE BUTTON
	else if (GuiButton({ windowwidth - sidebarwidth * 2 / 5,windowheight - textinputheight - textsavebuttonheight,sidebarwidth * 2 / 5,textsavebuttonheight }, "TEMPLATE"))
	{
		CodeState = Template;

		// Clean up the textboxes // NEEDS SERIOUS CHECK AND REWORK
		chosenblock = 0;
		textsizes.clear();
		textboxes.clear();

		// hard coded system for now
		std::cout << "template button\n";
		std::ifstream tempFile(templateFolder + "/template.txt");

		if (tempFile.is_open()) {
			std::string tempLine;
			while (std::getline(tempFile, tempLine)) {
				int lineSize = tempLine.size() + 1; // include the '\0'

				lineSize = (lineSize < textinputsize) ? lineSize : textinputsize;

				char* lineBuffer = new char[lineSize];  // Dynamically allocate memory
				std::memcpy(lineBuffer, tempLine.c_str(), lineSize);  // Copy line to char* buffer

				textboxes.push_back(lineBuffer);  // Store in vector
				textsizes.push_back(lineSize);
			}
			tempFile.close();  // Close the file
		}
	}
	//DRAWING SAVE TEMPLATE BUTTON
	else if (GuiButton({ windowwidth - sidebarwidth * 4 / 5,windowheight - textinputheight - textsavebuttonheight,sidebarwidth * 2 / 5,textsavebuttonheight }, "SAVE TEMPLATE"))
	{
		// hard coded system for now
		std::cout << "save template button\n";
		std::ofstream tempFile(templateFolder + "/template.txt");
	
		if (tempFile.is_open()) {
			std::cout << "file opened for wrtiting\n";
			for (const char *line : textboxes)
			{
				tempFile << line << '\n';
			}
			tempFile.close();
		}
	}

	//DRAWING NAME BANNER
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight, sidebarwidth, namebannersize, DARKGRAY);
	DrawTextEx(codingfontbold, workers[chosengrid][chosenperson].name, { windowwidth - 0.6 * sidebarwidth,sidebarbuttonheight + 10 }, 20, 5, WHITE);

	//Drawing Worker State
	DrawTextEx(codingfontbold,GetEnumEquivalent(workers[chosengrid][chosenperson].activity), {windowwidth - 0.2 * sidebarwidth, sidebarbuttonheight + 10}, 20, 1, GREEN);
	DrawTextEx(codingfontbold, TextFormat("E: %d", workers[chosengrid][chosenperson].energy), {windowwidth - sidebarwidth, sidebarbuttonheight + 10}, 17, 1, YELLOW);
	DrawTextEx(codingfontbold, TextFormat("P: %d", workers[chosengrid][chosenperson].productivity), { windowwidth - 0.85*sidebarwidth, sidebarbuttonheight + 10 }, 17, 1, YELLOW);


}

void DrawCodeTab()
{
	switch (CodeState)
	{
	case MainCode:
		DrawMainCodeTab();
		break;
	case Template:
		DrawTemplateTab();
		break;
	}
}

void DrawMiscTab()
{
	static short int workernum = workertypes.size();
	static bool drawmode = false;
	static short int chosenworkertype = 0;
	static Rectangle CancelButton = { windowwidth - sidebarwidth - moneybarwidth - 180,10,150,60 };
	static int buttonnum = miscitems.size();

	static vector<float> buttondims = { sidebarwidth,(windowheight - shopbuttonheight - sidebarbuttonheight) / (float)buttonnum };

	int cost = 0;

	//BOUNDARY
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), { 0,121,181,255 });
	DrawRectangleLinesEx({ windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight) }, areashopboundarywidth, { 220,220,220,220 });


	//BUTTONS
	for (int x = 0; x < buttonnum; x++)
	{
		Rectangle button = { windowwidth - sidebarwidth + 3 * areashopboundarywidth, sidebarbuttonheight + shopbuttonheight + x * buttondims[1] + (8 - buttonnum) * areashopboundarywidth, buttondims[0] - 6 * areashopboundarywidth, buttondims[1] - 2 * (8 - buttonnum) * areashopboundarywidth };
		if (CheckCollisionPointRec(mousepos, button) && drawmode == false)
		{
			DrawRectangleRounded(button, 4, 10, { 0,180,180,255 });
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				drawmode = true;
				chosenworkertype = x;
			}
		}
		else
			DrawRectangleRounded(button, 4, 10, DARKBLUE);


		DrawRectangleRoundedLinesEx(button, 4, 10, 5, { 220,220,220,220 });
		DrawCircle(button.x + button.width * 0.15, button.y + button.height * 0.5, button.height * 0.32, WHITE);
		DrawCircle(button.x + button.width * 0.15, button.y + button.height * 0.5, button.height * 0.3, get<0>(miscitems[x]));
		DrawTextEx(codingfontbold, get<1>(miscitems[x]), { button.x + button.width * 0.3f ,button.y + button.height * 0.4f }, 25, 2, WHITE);
		DrawTextEx(codingfontbold, TextFormat("$%d", get<2>(miscitems[x])), { button.x + button.width * 0.8f ,button.y + button.height * 0.4f }, 25, 2, GREEN);
	}

	if (drawmode == true)
	{
		//CANCEL BUTTON
		DrawRectangleRounded(CancelButton, 5, 10, MAROON);
		DrawRectangleRoundedLinesEx(CancelButton, 5, 10, 5, WHITE);
		DrawTextEx(codingfontbold, "Exit", { CancelButton.x + CancelButton.width / 3,CancelButton.y + CancelButton.height / 3 }, 23, 2, WHITE);
		if (CheckCollisionPointRec(mousepos, CancelButton))
		{
			DrawRectangleRounded(CancelButton, 5, 10, RED);
			DrawTextEx(codingfontbold, "Exit", { CancelButton.x + CancelButton.width / 3,CancelButton.y + CancelButton.height / 3 }, 23, 2, WHITE);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				drawmode = false;
				return;
			}
		}

		for (int y = workbenchsize / 2; y < grid.size() - workbenchsize / 2; y++)
		{
			for (int x = workbenchsize / 2; x < grid[0].size() - workbenchsize / 2; x++)
			{
				Rectangle chosenblock = { (x + screenbuffer) * linewidth,(y + screenbuffer) * lineheight,linewidth,lineheight };
				Rectangle block = { (x + screenbuffer - workbenchsize / 2) * linewidth,(y + screenbuffer - workbenchsize / 2) * lineheight,linewidth * (workbenchsize),lineheight * (workbenchsize) };
				if (CheckCollisionPointRec(mousepos, chosenblock))
				{
					DrawRectangleRec(block, get<0>(miscitems[chosenworkertype]));
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						int requiredground;
						switch (chosenworkertype)
						{
						case 0:
							requiredground = 1;
							break;
						case 1:
							requiredground = 4;
							break;
						case 2:
							requiredground = 3;
							break;
						}

						for (int i = y - workbenchsize / 2; i <= y + workbenchsize / 2; i++)
						{
							for (int j = x - workbenchsize / 2; j <= x + workbenchsize / 2; j++)
							{
								if (grid[i][j] != requiredground || grid[i][j] < 0)
								{
									drawmode = false;
									return;
								}
							}
						}

						//TODO:::IMPLEMENT WORKERPOSITIONS
						for (int i = y - workbenchsize / 2; i < y + workbenchsize - 1; i++)
						{
							for (int j = x - workbenchsize / 2; j < x + workbenchsize - 1; j++)
							{
								grid[i][j] = misccolors[chosenworkertype];
							}
						}

						//Calculate Cost
						int workernetcost = get<2>(miscitems[chosenworkertype]);

						//Subtract Cost
						if (workernetcost <= totalmoney)totalmoney -= workernetcost;

						//Spawn Worker
						if (chosenworkertype == 1)
						{
							workers[chosengrid].push_back(Worker(screenbuffer + 10, gridheight / 2));
							workers[chosengrid].back().grid = &grid;
							workers[chosengrid].back().workspace = { x + screenbuffer,y + screenbuffer };
							workers[chosengrid].back().pathfind({x + screenbuffer,y + screenbuffer});
						}
						else
							lunchpositions.push_back( {{ x + screenbuffer,y + screenbuffer },0} );
					}
				}
			}
		}
	}

}

void DrawAreaTab()
{
	static bool drawmode = false;
	static short int drawzone = -1;
	static pair<short int, short int> startpos = { 0,0 };
	static pair<short int, short int> endpos = { 0,0 };
	static long long int totalcost = 0;
	static short int clicks = 0;

	static int buttonnum = areaitems.size();
	static vector<float> buttondims = { sidebarwidth,(windowheight - shopbuttonheight - sidebarbuttonheight) / (float)buttonnum };

	static Rectangle CancelButton = { windowwidth - sidebarwidth -moneybarwidth- 180,10,150,60 };
	static Rectangle AcceptButton = { windowwidth - sidebarwidth-moneybarwidth - 380,10,150,60 };

	//BOUNDARY
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight),{0,121,181,255});
	DrawRectangleLinesEx({ windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight) },areashopboundarywidth, {220,220,220,220});

	//BUTTONS
	for (int x = 0; x < buttonnum; x++)
	{
		Rectangle button = { windowwidth - sidebarwidth + 3 * areashopboundarywidth, sidebarbuttonheight + shopbuttonheight + x * buttondims[1] + (8 - buttonnum) * areashopboundarywidth, buttondims[0] - 6 * areashopboundarywidth, buttondims[1] - 2 * (8 - buttonnum) * areashopboundarywidth };
		if (CheckCollisionPointRec(mousepos, button)&&drawmode==false)
		{
			DrawRectangleRounded(button, 4, 10, { 0,180,180,255 });
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				drawmode = true;
				drawzone = x;
			}
		}
		else
		DrawRectangleRounded(button, 4, 10, DARKBLUE);

		DrawRectangleRoundedLinesEx(button,4,10,5, { 220,220,220,220 });
		DrawCircle(button.x + button.width * 0.15, button.y + button.height * 0.5, button.height * 0.32, WHITE);
		DrawCircle(button.x + button.width * 0.15 , button.y + button.height*0.5, button.height*0.3, get<0>(areaitems[x]));
		DrawTextEx(codingfontbold, get<1>(areaitems[x]), { button.x + button.width * 0.3f ,button.y+button.height*0.4f},25,2,WHITE);
		DrawTextEx(codingfontbold,TextFormat( "$%d",get<2>(areaitems[x])), {button.x + button.width * 0.8f ,button.y + button.height * 0.4f}, 25, 2, GREEN);
	}


	//DRAW MODE
	if (drawmode == true)
	{
		DrawCircleV(mousepos, 5, get<0>(areaitems[drawzone]));

		//CANCEL BUTTON
		DrawRectangleRounded(CancelButton, 5, 10, MAROON);
		DrawRectangleRoundedLinesEx(CancelButton, 5, 10,5, WHITE);
		DrawTextEx(codingfontbold, "Cancel", { CancelButton.x + CancelButton.width/5,CancelButton.y + CancelButton.height/3 }, 23, 2, WHITE);
		if (CheckCollisionPointRec(mousepos, CancelButton))
		{
			DrawRectangleRounded(CancelButton, 5, 10, RED);
			DrawTextEx(codingfontbold, "Cancel", { CancelButton.x + CancelButton.width / 5,CancelButton.y + CancelButton.height / 3 }, 23, 2, WHITE);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				drawmode = false;
				startpos = { 0,0 };
				endpos = { 0,0 };
				totalcost = 0;
				return;
			}
		}

		//ACCEPT BUTTON
		DrawRectangleRounded(AcceptButton, 5, 10, DARKGREEN);
		DrawRectangleRoundedLinesEx(AcceptButton, 5, 10, 5, WHITE);
		DrawTextEx(codingfontbold, "Accept", { AcceptButton.x + AcceptButton.width / 5,AcceptButton.y + AcceptButton.height / 3 }, 23, 2, WHITE);
		if (CheckCollisionPointRec(mousepos, AcceptButton))
		{
			DrawRectangleRounded(AcceptButton, 5, 10, GREEN);
			DrawTextEx(codingfontbold, "Accept", { AcceptButton.x + AcceptButton.width / 5,AcceptButton.y + AcceptButton.height / 3 }, 23, 2, WHITE);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				drawmode = false;
				if (startpos != endpos)
				{
					if (totalcost > totalmoney)
					{
						drawmode = false;
						startpos = { 0,0 };
						endpos = { 0,0 };
						totalcost = 0;
						return;
					}

					for (int y = startpos.second; y < endpos.second; y++)
					{
						for (int x = startpos.first; x < endpos.first; x++)
						{
							if (grid[y][x] > 0)
							{
								drawmode = false;
								startpos = { 0,0 };
								endpos = { 0,0 };
								totalcost = 0;
								return;
							}
						}
					}

					for (int y = startpos.second; y < endpos.second; y++)
					{
						for (int x = startpos.first; x < endpos.first; x++)
						{
							grid[y][x] = areacolors[drawzone];
						}
					}
				}
				startpos = { 0,0 };
				endpos = { 0,0 };
				totalmoney -= totalcost;
				totalcost = 0;
				return;
			}
		}

		//DRAWING
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			for (int y = 0; y < grid.size(); y++)
			{
				for (int x = 0; x < grid[0].size(); x++)
				{
					if (CheckCollisionPointRec(mousepos, { (x+screenbuffer) * linewidth,(y+screenbuffer) * lineheight,linewidth,lineheight }))
					{
						clicks++;
							if (clicks % 2 == 1)
							{
								startpos = { x,y };
								return;
							}
							else
							{
								if (abs(x - startpos.first)+ abs(y - startpos.second)>0)
								{
									endpos = { x + 1 ,y + 1 };
									totalcost = abs(startpos.first - endpos.first) * abs(startpos.second - endpos.second) * get<2>(areaitems[drawzone]);
									return;
								}
								else
								{
									startpos = { 0,0 };
									endpos = { 0,0 };
									drawmode = false;
									totalcost = 0;
									return;
								}
							}
					}
				}
			}
		}

		//WHILE DRAWING
		if (clicks % 2 == 1)
			DrawRectangle((startpos.first + screenbuffer) * linewidth, (startpos.second + screenbuffer) * lineheight, mousepos.x - ((startpos.first + screenbuffer) * linewidth), mousepos.y - (startpos.second + screenbuffer) * lineheight, get<0>(areaitems[drawzone]));
		//AFTER DRAWING
		else if (clicks % 2 == 0)
		{
			Vector2 pos = { (windowwidth - sidebarwidth) / 2-80,windowheight - 50 };
			DrawRectangle((startpos.first + screenbuffer) * linewidth, (startpos.second + screenbuffer) * lineheight, (endpos.first - startpos.first) * linewidth, (endpos.second - startpos.second) * lineheight, get<0>(areaitems[drawzone]));
			DrawTextEx(codingfontbold, TextFormat("$%d", totalcost) , pos, 40, 5, GREEN);
		}
	}
}

void DrawShopTab()
{

	if (GuiButton({ windowwidth - sidebarwidth ,sidebarbuttonheight,sidebarwidth / 2,shopbuttonheight }, "Area"))
		ShopState = Area;
	if (GuiButton({ windowwidth - sidebarwidth / 2,sidebarbuttonheight,sidebarwidth / 2,shopbuttonheight }, "Misc"))
		ShopState = Misc;

	switch (ShopState)
	{
	case Area:
		DrawAreaTab();
		break;
	case Misc:
		DrawMiscTab();
	}

}

void DrawHireTab()
{
	static short int workernum = workertypes.size();
	static short int iconsize = (workernum < 4) ? 5 : 3;
	static float iconspacing = (workernum < 4) ? 0.28 : 0.36;
	static float bannerheight = (workernum < 4) ? 0.5 : 0.47;
	static bool drawmode = false;
	static short int chosenworkertype = 0;
	static short int chosenposition = 0;
	static Rectangle CancelButton = { windowwidth - sidebarwidth - moneybarwidth - 180,10,150,60 };

	//BackGround
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight, sidebarwidth, windowheight - sidebarbuttonheight, BLUE);

	Vector2 buttondims = { sidebarwidth /3, (windowheight - sidebarbuttonheight) /workernum};
	for (int x = 0; x < workernum; x++)
	{
		for (int y = 3; y >=1; y--)
		{
			int cost=0;
			Rectangle button = { windowwidth - y * buttondims.x + 6, sidebarbuttonheight + x * buttondims.y + 15, buttondims.x - 12, buttondims.y - 30 };
			
			//DRAW BUTTON AND BOUNDARIES
			if (CheckCollisionPointRec(mousepos, button)&&drawmode==false)
			{
				DrawRectangleRec(button, {0,131,255,255});
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					drawmode = true;
					chosenworkertype = x;
					chosenposition = y;
				}
			}
			else
			DrawRectangleRec(button, DARKBLUE);
			DrawRectangleLinesEx(button, 4, WHITE);


			//DRAW MONEY AND POSITION
			Color iconcolor = WHITE;
			if (y == 3)
			{
				iconcolor = { 180,180,180,255 };
				cost = get<2>(workertypes[workernum-1 - x]);
				DrawTextEx(codingfontbold, "Junior", { button.x + button.width * 0.33f,button.y + button.height * bannerheight }, 15, 1, iconcolor);
			}
			else if (y == 2)
			{
				iconcolor = { 220,220,0,255 };
				cost = get<2>(workertypes[workernum-1 - x]) * 20;
				DrawTextEx(codingfontbold, "Senior", { button.x + button.width * 0.33f,button.y + button.height * bannerheight }, 15, 1, iconcolor);
			}
			else if (y == 1)
			{
				iconcolor = PURPLE;
				cost = get<2>(workertypes[workernum-1 - x]) * 80;
				DrawTextEx(codingfontbold, "Specialist", { button.x + button.width * 0.23f,button.y + button.height * bannerheight }, 15, 1, iconcolor);
			}
			DrawTextEx(codingfontbold, TextFormat("$%d", cost), { button.x + button.width * 0.3f,button.y + button.height * 0.8f }, 25, 1, GREEN);

			
			//DRAW NAME AND ICON
			switch (x)
			{
			case 0:
				GuiDrawIcon(ICON_MONITOR, button.x + button.width *iconspacing, button.y + button.height/15, iconsize, iconcolor);
				DrawTextEx(codingfontbold, "Software", { button.x + button.width*0.13f,button.y + button.height *0.6f }, 22, 3, WHITE);
				break;
			case 1:
				GuiDrawIcon(ICON_GEAR, button.x + button.width *iconspacing, button.y + button.height / 15, iconsize, iconcolor);
				DrawTextEx(codingfontbold, "Hardware", { button.x + button.width * 0.13f,button.y + button.height * 0.6f }, 22, 3, WHITE);
				break;
			case 2:
				GuiDrawIcon(ICON_WAVE, button.x + button.width *iconspacing, button.y + button.height / 15, iconsize, iconcolor);
				DrawTextEx(codingfontbold, "Network", { button.x + button.width * 0.18f,button.y + button.height * 0.6f }, 22, 3, WHITE);
				break;
			default:break;
			}
		}
	}

	if (drawmode == true)
	{
		//CANCEL BUTTON
		DrawRectangleRounded(CancelButton, 5, 10, MAROON);
		DrawRectangleRoundedLinesEx(CancelButton, 5, 10, 5, WHITE);
		DrawTextEx(codingfontbold, "Exit", { CancelButton.x + CancelButton.width / 3,CancelButton.y + CancelButton.height / 3 }, 23, 2, WHITE);
		if (CheckCollisionPointRec(mousepos, CancelButton))
		{
			DrawRectangleRounded(CancelButton, 5, 10, RED);
			DrawTextEx(codingfontbold, "Exit", { CancelButton.x + CancelButton.width / 3,CancelButton.y + CancelButton.height / 3 }, 23, 2, WHITE);
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				drawmode = false;
				return;
			}
		}

		for (int y = workbenchsize/2; y < grid.size()-workbenchsize/2; y++)
		{
			for (int x = workbenchsize/2; x < grid[0].size()-workbenchsize/2; x++)
			{
				Rectangle chosenblock = { (x + screenbuffer) * linewidth,(y + screenbuffer) * lineheight,linewidth,lineheight };
				Rectangle block = { (x + screenbuffer- workbenchsize / 2) * linewidth,(y + screenbuffer- workbenchsize / 2) * lineheight,linewidth*(workbenchsize),lineheight*(workbenchsize)};
				if (CheckCollisionPointRec(mousepos, chosenblock))
				{
					DrawRectangleRec(block, {40,255,40,255});
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						for (int i = y - workbenchsize / 2; i <= y + workbenchsize / 2; i++)
						{
							for (int j = x - workbenchsize / 2; j <= x + workbenchsize / 2; j++)
							{
								if (grid[i][j] != 2||grid[i][j]<0)
								{
									drawmode = false;
									return;
								}
							}
						}

						//TODO:::IMPLEMENT WORKERPOSITIONS
						for (int i = y - workbenchsize / 2; i < y + workbenchsize-1; i++)
						{
							for (int j = x - workbenchsize / 2; j < x + workbenchsize-1; j++)
							{
								grid[i][j] = -2;
							}
						}

						//Calculate Cost
						int workernetcost=0;
						if (chosenposition == 3)workernetcost= get<2>(workertypes[workernum-1-chosenworkertype]);
						else if (chosenposition == 2)workernetcost= get<2>(workertypes[workernum-1-chosenworkertype])*20;
						else if (chosenposition == 1)workernetcost= get<2>(workertypes[workernum-1-chosenworkertype])*80;

						//Subtract Cost
						if (workernetcost <= totalmoney)totalmoney -= workernetcost;

						//Spawn Worker
						workers[chosengrid].push_back(Worker(screenbuffer, gridheight / 2));
						workers[chosengrid].back().grid = &grid;
						workers[chosengrid].back().workspace = { x + screenbuffer,y + screenbuffer };
						workers[chosengrid].back().pathfind({x +screenbuffer ,y+screenbuffer});
					}
				}
			}
		}
	}

}

void DrawInvestTab()
{
	static char inputamount[10] = "";
	static float outputamount = 0;

	//BOUNDARY
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), {0,100,50,255});
	DrawRectangleLinesEx({ windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight) }, areashopboundarywidth, { 220,220,220,220 });

	//Bank Icon
	DrawTextureEx(bankicon, { windowwidth - sidebarwidth * 0.75f, windowheight *0.2 },0,1, WHITE);

	//Money Invested
	DrawTextEx(codingfontbold, "Money Invested", { windowwidth - sidebarwidth * 0.8,windowheight*0.55 }, 38, 1, WHITE);
	DrawTextEx(codingfontbold, TextFormat("$%0.f",bank.player_savings), { windowwidth - sidebarwidth * (0.55f+((bank.player_savings/1000>0)?0.06f:0.0f)),windowheight * 0.61}, 30, 1, GREEN);

	//Invest Amount
	DrawTextEx(codingfontbold, "Invest:", { windowwidth - sidebarwidth * 0.75f ,windowheight * 0.72 }, 25, 1, WHITE);
	if (GuiTextBox({ windowwidth - sidebarwidth * 0.52f ,windowheight * 0.7,sidebarwidth * 0.4f,windowheight * 0.06f }, inputamount, 10, true))
	{
		int amount=0;
		try
		{
			amount = stoi(inputamount);
		}
		catch (exception e) {};

		if (amount <= totalmoney&&amount>=0)
		{
			bank.invest(amount);
			totalmoney -= amount;
		}
		inputamount[0] = '\0';
	}

	//Extract Amount
	DrawTextEx(codingfontbold, "Take Out:", { windowwidth - sidebarwidth * 0.68f ,windowheight * 0.81 }, 25, 1, WHITE);
	DrawTextEx(codingfontbold, TextFormat("$%0.f",outputamount), {windowwidth - sidebarwidth * 0.43f ,windowheight * 0.81}, 25, 1, GREEN);
	GuiSlider({ windowwidth - sidebarwidth * 0.8f ,windowheight * 0.85,sidebarwidth * 0.6f,40 }, "0", TextFormat("%0.f", bank.player_savings), &outputamount, 0, bank.player_savings);
	if (GuiButton({ windowwidth - sidebarwidth,windowheight - bankbuttonheight,sidebarwidth,bankbuttonheight }, "Extract"))
	{
		if (outputamount < bank.player_savings)
		{
			bank.player_savings -= outputamount;
			totalmoney += outputamount;
			outputamount = 0;
		}
	}
}

void DrawFDTab()
{
	static int forwarddepositnum = 0;
	static float inputamount = 0;
	static float rate = 0;
	static float term = 20;


	//BOUNDARY
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), { 0,100,50,255 });
	DrawRectangleLinesEx({ windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight) }, areashopboundarywidth, { 220,220,220,220 });


	//Deposit Amount
	DrawRectangle(windowwidth - sidebarwidth+areashopboundarywidth, windowheight * 0.77, sidebarwidth-2*areashopboundarywidth, windowheight * 0.23, { 0,80,50,255 });
	DrawTextEx(codingfontbold, "Deposit:", { windowwidth - sidebarwidth * 0.65f ,windowheight * 0.81 }, 25, 1, WHITE);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", inputamount), { windowwidth - sidebarwidth * 0.43f ,windowheight * 0.81 }, 25, 1, GREEN);
	DrawTextEx(codingfont, TextFormat("Rate:%0.f%%",rate), { windowwidth - sidebarwidth * 0.78f ,windowheight * 0.9 }, 22, 1, YELLOW);
	DrawTextEx(codingfont, TextFormat("Term:%0.f ticks", term), { windowwidth - sidebarwidth * 0.45f ,windowheight * 0.9 }, 22, 1, YELLOW);
	GuiSlider({ windowwidth - sidebarwidth * 0.8f ,windowheight * 0.85,sidebarwidth * 0.6f,30 }, "0", TextFormat("%0.f", totalmoney), &inputamount, 0, totalmoney);

	vector<float> fdstats = GetFDStats(inputamount);
	rate = fdstats[0];
	term = fdstats[1];

	if (GuiButton({ windowwidth - sidebarwidth,windowheight - bankbuttonheight,sidebarwidth,bankbuttonheight }, "Deposit"))
	{
		if (forwarddepositnum < maxforwarddeposits&&inputamount>0)
		{
			bank.add_forward_deposit(inputamount, term, rate);
			forwarddepositnum++;
			totalmoney -= inputamount;
			inputamount = 0;
		}
	}

	//Draw Deposits
	for (int x = 0; x < forwarddepositnum; x++)
	{
		//Block
		float blockheight = 10+shopbuttonheight + bankbuttonheight + x*(windowheight * 0.77 - shopbuttonheight - bankbuttonheight)/ (float)maxforwarddeposits;
		Rectangle block = { windowwidth - sidebarwidth + areashopboundarywidth,blockheight,sidebarwidth - 2 * areashopboundarywidth,(windowheight * 0.77 - shopbuttonheight - bankbuttonheight) / (float)maxforwarddeposits };
		DrawRectangleRec(block, {140,140,140,255});
		DrawRectangle(block.x, block.y, block.width * 0.1f, block.height, {120,120,120,255});
		DrawRectangleLinesEx(block, 5, {100,100,100,100});
		DrawTextEx(codingfontbold, TextFormat("%d", x+1), { block.x + block.width * 0.03f ,block.y + block.height * 0.43f }, 20, 0, { 140,140,140,255 });

		//Draw Text
		if(bank.forward_deposit_term[x]>0)
			DrawTextEx(codingfontbold, "MATURING", { block.x + block.width * 0.13f,block.y + block.height * 0.43f }, 25, 3, GOLD);
		else
			DrawTextEx(codingfontbold, "MATURED", { block.x + block.width * 0.13f,block.y + block.height * 0.43f }, 25, 3, GREEN);

		//Draw Progress Bar
		float val = bank.forward_deposit_totalterm[x] - bank.forward_deposit_term[x];
		DrawTextEx(codingfontbold, TextFormat("$%0.f", bank.forward_deposit_amount[x]*(1+0.01*bank.forward_deposit_rate[x])), {block.x + block.width * 0.65f ,block.y + block.height * 0.4f}, 26, 1, GOLD);
		//GuiProgressBar({ block.x + block.width * 0.5f,block.y + block.height * 0.45f,block.width * 0.25f,block.height * 0.1f }, "0", TextFormat("%0.f", bank.forward_deposit_totalterm[x]), &val, 0, bank.forward_deposit_totalterm[x]);
		GuiProgressBar({ block.x + block.width * 0.1f,block.y + block.height * 0.9f,block.width * 0.8f,block.height * 0.1f }, "", "", &val, 0, bank.forward_deposit_totalterm[x]);

		if (GuiButton({ block.x + block.width * 0.9f,block.y,block.width * 0.9f,block.height }, "")&& bank.forward_deposit_term[x]<=0.2)
		{
			totalmoney+=bank.mature_forward_deposits(x);
			forwarddepositnum--;
			break;
		}
		GuiDrawIcon(ICON_ARROW_DOWN_FILL, block.x + block.width * 0.935f, block.y+block.height*0.4, 2, WHITE);
	}

}

void DrawBankTab()
{
	if (GuiButton({ windowwidth - sidebarwidth ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "Invest"))
		BankState = Invest;
	if (GuiButton({ windowwidth - 2*sidebarwidth/3 ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "FD"))
		BankState = FD;
	if (GuiButton({ windowwidth - sidebarwidth / 3 ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "Loan"))
		BankState = Loan;

	switch (BankState)
	{
	case Invest:
		DrawInvestTab();
		break;
	case FD:
		DrawFDTab();
		break;
	case Loan:
		break;
	}
}

void DrawSidebar()
{
	//Drawing 3 Sidebar Buttons
	DrawRectangle(windowwidth - sidebarwidth, 0, sidebarwidth, windowheight, DARKGRAY);
	if (GuiButton({ windowwidth - sidebarwidth,0,sidebarwidth / 4,sidebarbuttonheight }, "Code"))
		SidebarState = Code;
	else if (GuiButton({ windowwidth - 3 * sidebarwidth / 4,0,sidebarwidth / 4,sidebarbuttonheight }, "Shop"))
		SidebarState = Shop;
	else if (GuiButton({ windowwidth - sidebarwidth / 2,0,sidebarwidth / 4,sidebarbuttonheight }, "Hire"))
		SidebarState = Hire;
	else if (GuiButton({ windowwidth - sidebarwidth / 4,0,sidebarwidth / 4,sidebarbuttonheight }, "Bank"))
		SidebarState = Money;

	switch (SidebarState)
	{
	case Code:
		DrawCodeTab();
		break;
	case Shop:
		DrawShopTab();
		break;
	case Hire:
		DrawHireTab();
		break;
	case Money:
		DrawBankTab();
		break;
	}
}

void UpdateStocks()
{
	//UPDATE STOCKS
	int minval = INT_MAX;
	for (int i = 0; i < competitors.size(); i++)
	{
		if (competitors[i].competitor.size() > 1)
		{
			if (competitors[i].competitor[competitors[i].competitor.size() - 1] > maxstock)
			{
				maxstock = competitors[i].competitor[competitors[i].competitor.size() - 1] * 1.4;
			}
			for (int j = (totalticks/updatetime - stockperiod < 0) ? 0 : totalticks/updatetime - stockperiod; j < competitors[i].competitor.size() - 1; j++)
			{
				if (competitors[i].competitor[j] < minval)
				{
					minval = competitors[i].competitor[j];
				}
			}
		}
	}
	if (playerstock[playerstock.size() - 1] > maxstock)
		maxstock = playerstock[playerstock.size() - 1];

	minstock = minval;

}

int main()
{
	InitWindow(windowwidth, windowheight, "ag.AI.n");
	SetTargetFPS(144);

	srand(time(NULL));

	//Flags
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 1);

	//Create folder dir
	gameSetup();

	//Set Font
	codingfont = LoadFont("./fonts/dina10px.ttf");
	codingfontbold = LoadFont("./fonts/dina10pxbold.ttf");
	GuiSetFont(codingfont);

	//Initialize Grid
	InitializeGrid(60,60,0);
	InitializeGridPrices();
	InitializeShop();
	InitializeHire();
	InitializeSprites();
	InitializeSkillTree();

	ScreenMode = Map; //DEBUG
	BankState = FD;
	SidebarState = Money;

	//Add Competitors
	competitors.push_back(RandomGenerator());
	competitors.push_back(RandomGenerator());
	competitors.push_back(RandomGenerator());
	competitors.push_back(RandomGenerator());
	competitors.push_back(RandomGenerator());

	bool clockswitch = false;

	while (!WindowShouldClose())
	{
		mousepos = GetMousePosition();
		frametime = GetFrameTime();

		//GAMETIME
		gametime = std::chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count()%ticksize;
		if (gametime <= 20 && clockswitch == false)
		{
			clockswitch = true;
			totalticks++;

			//Tick Functions
			ChangeWorkerPositions();
			BankUpdate();
			WorkerCodeUpdate();


			if (totalticks % updatetime == 0)
			{
				playerstock.push_back(totalmoney / 100);
				for (int j = 0; j < competitors.size(); j++)
				{
					competitors[j].randgen();
				}
				UpdateStocks();
			}
		}
		else if (gametime > 50 && clockswitch == true)clockswitch = false;

		//RAYLIB
		BeginDrawing();
		ClearBackground(WHITE);

		//Money Increment
		if (totalmoney < quota)
		{
			if (totalmoney + moneyincrement * frametime > quota)
				totalmoney = quota;
			else
				totalmoney += moneyincrement * frametime;
		}
		
		//DRAWING
		switch (ScreenMode)
		{
		case View:
			DrawMainScreen();
			DrawSidebar();
			DrawProgressBar();
			break;
		case Map:
			DrawMap();
			break;
		case Stock:
			DrawStocks();
			break; 
		case SkillTree:
			DrawSkillTree();
			break;
		}
		DrawText(TextFormat("%d", GetFPS()), 10, 10, 25, BLACK);

		EndDrawing();
	}
}