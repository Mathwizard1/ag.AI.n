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
	Landing,   //channges made by me
	Settings,
	ISA,
	Help
} ScreenMode;

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
std::vector<pair<pair<short int, short int>, bool>> lunchpositions;

std::vector<tuple< Color, char*, int>> workertypes;

std::vector<RandomGenerator> competitors;

short int workbenchsize;
Color workbenchcolor;

Vector2 mousepos;

Texture officetile;
Texture bankicon;
Texture2D background;

/*
Colors:
1->Lunch Area (Yellow)
2->Work Area (Green)
3->Reception (Blue)
4->Boss (Red)
5->Chef (Light Yellow)
6->Boss (Maroon)

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
	default:
		return "No Expression Found";
	}
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
		if (--bank.forward_deposit_term[x] <= 0)
		{
			totalmoney += bank.mature_forward_deposits(x);
		}
	}


}

void ChangeWorkerPositions()
{
	for (int y = 0;y < gridnetwork.size();y++)
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
						lunchpositions[workers[i][j].occupiedbench].second = false;
						break;
					case Worker::Moving:
						break;
					}

					workers[i][j].activity = Worker::Idle;
					workers[i][j].linecounter++;
				}

			}
			else
			{
				//Interpret Line
				workers[i][j].callFunction();
			}
		}
	}
}

void InitializeSprites()
{
	officetile = LoadTextureFromImage(LoadImage("sprites/office_tile.png"));
	bankicon = LoadTextureFromImage(LoadImage("sprites/Bank.png"));
	background = LoadTexture("landing.png");

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
	areaitems.push_back({ GREEN,(char*)"Work Area",10 });
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
	static int midx = dims / 2;
	static int midy = dims / 2;

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

	for (int z = 0; z < mapsize; z++)
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
		for (int y = 0; y < grid.size() - 1; y += grid.size() / mapdims)
		{
			vector<short int> row;
			for (int x = 0; x < grid[0].size() - 1; x += grid[0].size() / mapdims)
			{
				vector<int> numarray(9);

				for (int q = 0; q < grid.size() / mapdims; q++)
				{
					for (int r = 0; r < grid[0].size() / mapdims; r++)
					{
						//cout << grid[y + q][x + r] << endl;
						if (grid[y + q][x + r] >= -2)
							numarray[grid[y + q][x + r] + 2]++;
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
				row.push_back(max - 2);
			}

			pooledgrid.push_back(row);
		}
		pooledgridnetwork.push_back(pooledgrid);
	}
	gridpurchased[mapsize / 2] = 1;
	grid = gridnetwork[mapsize / 2];

	gridheight = grid.size() + 2 * screenbuffer;
	gridwidth = grid[0].size() + 2 * screenbuffer;
	linewidth = (windowwidth - sidebarwidth - moneybarwidth) / (float)gridwidth;
	lineheight = (windowheight) / (float)gridheight;
}

void DrawStocks()
{
	static float rectx = windowwidth * 0.33;
	static float recty = windowheight * 0.2;
	int xstart = (totalticks / updatetime - stockperiod < 0) ? 0 : totalticks / updatetime - stockperiod;


	//Draw Rectangle
	DrawRectangleLinesEx({ 0,0,windowwidth,windowheight }, 20, DARKGRAY);
	DrawRectangleRec({ 20,20,windowwidth - 40,windowheight - 40 }, GRAY);
	DrawRectangle(rectx - 80, recty, rectx + 140, windowheight * 0.6 + 20, BLUE);
	DrawRectangle(rectx - 60, recty + 20, rectx + 100, windowheight * 0.6 - 20, WHITE);

	//Draw Competitors Stocks
	for (int i = 0; i < competitors.size(); i++)
	{
		if (competitors[i].competitor.size() > 1)
		{
			for (int j = xstart; j < competitors[i].competitor.size() - 1; j++)
			{
				//STOCK LINES
				DrawLineEx({ rectx + 40 + (j - xstart) * (rectx - 80) / stockperiod,windowheight * 0.8f - 50 - (float)((competitors[i].competitor[j] - minstock) / (maxstock - minstock)) * (windowheight * 0.6f - 80) }, { rectx + 40 + (j - xstart + 1) * (rectx - 80) / stockperiod,windowheight * 0.8f - 50 - (float)((competitors[i].competitor[j + 1] - minstock) / (maxstock - minstock)) * (windowheight * 0.6f - 80) }, 5, RED);
			}
			if (competitors[i].competitor[competitors[i].competitor.size() - 1] > maxstock)
			{
				maxstock = competitors[i].competitor[competitors[i].competitor.size() - 1] * 1.4;
			}

			DrawTextEx(codingfontbold, TextFormat("Competitor %d: $%0.f", i + 1, competitors[i].competitor[competitors[i].competitor.size() - 1]), { windowwidth * 0.75,windowheight * 0.1f * (i + 1) }, 30, 1, MAROON);
		}
	}

	//Draw Player Stocks
	if (playerstock.size() > 0)
		for (int j = xstart; j < playerstock.size() - 1; j++)
		{
			//STOCK LINES
			DrawLineEx({ rectx + 40 + (j - xstart) * (rectx - 80) / stockperiod,windowheight * 0.8f - 50 - (float)((playerstock[j] - minstock) / (maxstock - minstock)) * (windowheight * 0.6f - 80) }, { rectx + 40 + (j - xstart + 1) * (rectx - 80) / stockperiod,windowheight * 0.8f - 50 - (float)((playerstock[j + 1] - minstock) / (maxstock - minstock)) * (windowheight * 0.6f - 80) }, 5, GREEN);
		}

	//Labels
	DrawTextEx(codingfontbold, TextFormat("$%0.f", maxstock), { rectx - 40 , recty + 50 }, 22, 1, GREEN);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", minstock), { rectx - 40 , windowheight * 0.75f - 20 }, 22, 1, GREEN);
	DrawTextEx(codingfont, "$", { rectx - 30 , windowheight * 0.5f - 20 }, 50, 1, GREEN);

	//Draw Axes
	DrawRectangleRounded({ rectx + 35,recty + 40 ,12, windowheight * 0.6 - 80 }, 6, 1, BLACK);
	DrawRectangleRounded({ rectx + 35,windowheight * 0.8f - 50 , rectx - 70,12 }, 6, 1, BLACK);

	GuiDrawIcon(ICON_ARROW_UP_FILL, rectx + 18, recty + 15, 3, BLACK);
	GuiDrawIcon(ICON_ARROW_RIGHT_FILL, 2 * rectx - 60, windowheight * 0.8 - 69, 3, BLACK);


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
	DrawRectangleLinesEx({ 0, 0, windowwidth, windowheight }, 20, GRAY);

	//Draw Total Money
	DrawTextEx(codingfontbold, TextFormat("$%0.f", totalmoney), { windowwidth - windowwidth / 12, 50 }, 30, 1, GREEN);

	//Draw Lines
	for (int y = 0; y < dims; y++)
	{

		DrawLineEx({ windowwidth / 6,windowheight / 6 + (float)y * (2 * windowheight / 3) / (dims - 1) }, { 5 * windowwidth / 6, windowheight / 6 + (float)y * (2 * windowheight / 3) / (dims - 1) }, 8, DARKBLUE);
		DrawLineEx({ windowwidth / 6 + (float)(y) * (2 * windowwidth / 3) / (dims - 1),windowheight / 6 }, { windowwidth / 6 + (float)y * (2 * windowwidth / 3) / (dims - 1) ,5 * windowheight / 6 }, 8, DARKBLUE);
	}

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
				DrawCircle(xpoint, ypoint, rad, { 0,245,245,245 });
				DrawCircle(xpoint, ypoint, rad - 5, { 0,205,205,205 });
			}
			else
			{
				DrawCircle(xpoint, ypoint, rad + 100 / mapsize, RED);
				DrawCircle(xpoint, ypoint, rad, { 250,250,0,255 });
				DrawCircle(xpoint, ypoint, rad - 5, { 200,20,0,255 });
			}

			if (CheckCollisionPointCircle(mousepos, { xpoint,ypoint }, rad))
			{
				short int gridnum = x + y * dims;
				if (gridpurchased[gridnum] == 1)
					DrawCircle(xpoint, ypoint, rad - 5, { 0,225,225,225 });
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
						chosengrid = gridnum;
						ScreenMode = View;
					}
				}
			}


			//GRIDS
			if (gridpurchased[x + y * dims] == 1)
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
						DrawRectangleRec({ xpoint - rad * 0.5f + i * radmap + 1,ypoint - rad * 0.5f + j * radmap + 1,radmap - 2,radmap - 2 }, blockcolor);
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

void DrawWorkers(float linewidth, float lineheight)
{
	for (int x = 0; x < workers[chosengrid].size(); x++)
	{
		Vector2 workerpos = { (float)(workers[chosengrid][x].pos.first * linewidth + linewidth / 2),(float)(workers[chosengrid][x].pos.second * lineheight + lineheight / 2) };
		DrawCircleV(workerpos, min(linewidth / 2, lineheight / 2) - 2, { 100,100,255,255 });
		DrawCircleLinesV(workerpos, min(linewidth / 2, lineheight / 2) - 2, BLACK);
		DrawTextEx(codingfontbold, TextFormat("%s(%d)", workers[chosengrid][x].name, workers[chosengrid][x].gender), { workerpos.x + linewidth / 3,workerpos.y - lineheight * 0.5f - 10.0f }, 18, 0, BLACK);

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
	static Rectangle outerrect = { windowwidth - sidebarwidth - moneybarwidth + 50, 50,50,800 };
	static Rectangle boundary = { windowwidth - sidebarwidth - moneybarwidth, 0, moneybarwidth, windowheight };
	float fraction = totalmoney / (float)quota;
	Rectangle innerrect = { outerrect.x + 10,outerrect.y + (outerrect.height * (1 - fraction)) + 10,outerrect.width - 20 ,outerrect.height * fraction - 20 };
	Rectangle shinerect = { outerrect.x + 15,outerrect.y + (outerrect.height * (1 - fraction)) + 20,innerrect.width / 10 ,outerrect.height * fraction - 40 };

	DrawRectangleRec(boundary, DARKGRAY);

	DrawRectangleRounded(outerrect, 10, 10, BLACK);
	DrawRectangleRounded(innerrect, 10, 10, GOLD);
	DrawRectangleRounded(shinerect, 10, 10, WHITE);
	DrawCircle(boundary.x + 10, boundary.y + 10, 5, GRAY);
	DrawCircle(boundary.x + moneybarwidth - 10, boundary.y + 10, 5, GRAY);
	DrawCircle(boundary.x + 10, windowheight - 10, 5, GRAY);
	DrawCircle(boundary.x + moneybarwidth - 10, windowheight - 10, 5, GRAY);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", totalmoney), { outerrect.x - 15,outerrect.y + outerrect.height + 10 }, 20, 4, YELLOW);
	DrawTextEx(codingfontbold, TextFormat("$%d", quota), { outerrect.x - 20 ,outerrect.y - 20 }, 20, 4, YELLOW);
}

void DrawMainScreen()
{
	static float tilesize = linewidth / (float)40;

	//Draw Cells
	Color color = WHITE;
	for (int y = 0; y < gridheight; y++)
	{
		for (int x = 0; x < gridwidth; x++)
		{
			if ((x >= 0 && x < screenbuffer - 1) || (x >= gridwidth - screenbuffer + 1 && x < gridwidth) || (y >= 0 && y < screenbuffer - 1) || (y >= gridheight - screenbuffer + 1 && y < gridheight))
				color = BROWN;
			else if ((x == screenbuffer - 1) || (x == gridwidth - screenbuffer) || (y == screenbuffer - 1) || (y == gridheight - screenbuffer))
				color = DARKBROWN;
			else
				switch (grid[y - screenbuffer][x - screenbuffer])
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

			if (((x >= 0 && x < screenbuffer) || (x >= gridwidth - screenbuffer && x < gridwidth)) && y > (int)(gridheight * 0.4) && y < (int)(gridheight * 0.6))
				color = { 0,205,255,255 };

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
		DrawLineEx({ 0,y * lineheight }, { windowwidth - sidebarwidth - moneybarwidth,y * lineheight }, 1, BLACK);
	}

	//DRAW WORKERS
	DrawWorkers(linewidth, lineheight);

	//Draw Map button (existing)
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
				for (int l = 0; l < grid.size() - 1; l += grid.size() / mapdims)
				{
					vector<short int> row;
					for (int m = 0; m < grid[0].size() - 1; m += grid[0].size() / mapdims)
					{
						//UPDATE
						vector<int> numarray(9);

						for (int q = 0; q < grid.size() / mapdims; q++)
						{
							for (int r = 0; r < grid[0].size() / mapdims; r++)
							{
								numarray[grid[l + q][m + r] + 2]++;
							}
						}


						int min = 0;
						if (numarray[1] > 0)
							min = 1;
						else
						{
							for (int q = 0; q < 9; q++)
							{
								if (numarray[q] > numarray[min])
									min = q;
							}
						}

						row.push_back(min - 2);
					}
					pooledgrid.push_back(row);
				}
				pooledgridnetwork[chosengrid] = pooledgrid;
				textboxesnetwork[chosengrid] = textboxes;
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

	// New button for returning to landing page
	DrawCircle(70, 160, 50, BLACK);
	if (mousepos.x < 120 && mousepos.y > 110 && mousepos.y < 210)
		if (CheckCollisionPointCircle(mousepos, { 70, 160 }, 50))
		{
			DrawCircle(70, 160, 45, { 255, 215, 0, 255 }); // Gold color when hovering
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				// First save current grid state like the map button does
				gridnetwork[chosengrid] = grid;

				vector<vector<short int>> pooledgrid;
				for (int l = 0; l < grid.size() - 1; l += grid.size() / mapdims)
				{
					vector<short int> row;
					for (int m = 0; m < grid[0].size() - 1; m += grid[0].size() / mapdims)
					{
						vector<int> numarray(9);
						for (int q = 0; q < grid.size() / mapdims; q++)
						{
							for (int r = 0; r < grid[0].size() / mapdims; r++)
							{
								numarray[grid[l + q][m + r] + 2]++;
							}
						}

						int min = 0;
						if (numarray[1] > 0)
							min = 1;
						else
						{
							for (int q = 0; q < 9; q++)
							{
								if (numarray[q] > numarray[min])
									min = q;
							}
						}
						row.push_back(min - 2);
					}
					pooledgrid.push_back(row);
				}
				pooledgridnetwork[chosengrid] = pooledgrid;
				textboxesnetwork[chosengrid] = textboxes;
				textsizesnetwork[chosengrid] = textsizes;

				// Change screen mode to Landing
				ScreenMode = Landing;
				return;
			}
		}
		else
			DrawCircle(70, 160, 45, { 218, 165, 32, 255 }); // Default gold color
	else
		DrawCircle(70, 160, 45, { 218, 165, 32, 255 }); // Default gold color

	// Draw home icon
	GuiDrawIcon(ICON_HOUSE, 45, 140, 4, WHITE);
}


void DrawCodeTab()
{
	if (workers[chosengrid].empty())
	{
		DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), GRAY);
		DrawRectangle(windowwidth - sidebarwidth + 80, windowheight * 0.43, sidebarwidth - 160, 130, BLACK);
		DrawRectangle(windowwidth - sidebarwidth + 100, windowheight * 0.45, sidebarwidth - 200, 100, RED);
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
	static double codeblockheight = (windowheight - sidebarbuttonheight - textinputheight - textsavebuttonheight - namebannersize) / (double)codeblocks;

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
		char* sendtext = (char*)malloc(sizeof(char) * textinputsize);

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
			for (int x = textboxes.size(); x <= chosentext; x++)
			{
				char* temptext = (char*)malloc(sizeof(char) * textinputsize);
				temptext[0] = '\0';
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
		Color blockcolor = (i == chosenblock) ? GREEN : Color{ 0,150,255,255 };
		if (i == interpreterblock)
			blockcolor = DARKBLUE;

		Rectangle codeblock = { windowwidth - sidebarwidth, sidebarbuttonheight + namebannersize + i * codeblockheight, sidebarwidth, codeblockheight };
		DrawRectangleRec(codeblock, blockcolor);
		DrawRectangle(codeblock.x, codeblock.y, 55, codeblockheight, GRAY);
		DrawRectangleLinesEx(codeblock, 2, DARKBLUE);
		if (textboxes.size() > startpos + i)
		{
			DrawTextEx(codingfontbold, textboxes[startpos + i], { codeblock.x + sidebarwidth / 7, (float)(codeblock.y + codeblockheight * 0.4) }, textinputfontsize - 2 * (textsizes[startpos + i] / 12), 3, WHITE);
		}
		DrawTextEx(codingfontbold, TextFormat("%d", (startpos + i)), { (float)codeblock.x + 10, (float)(codeblock.y + codeblockheight / 2.5) }, textinputfontsize - 5, 3, WHITE);

		if (CheckCollisionPointRec(mousepos, codeblock) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			text[0] = '\0';
			chosenblock = i;
			chosentext = i + startpos;
			editing = false;
		}
	}

	//DRAWING SAVE BUTTON
	if (GuiButton({ windowwidth - sidebarwidth,windowheight - textinputheight - textsavebuttonheight,sidebarwidth,textsavebuttonheight }, "SAVE"))
	{
		workers[chosengrid][chosenperson].code = textboxes;
		workers[chosengrid][chosenperson].linesize = textsizes;
		workers[chosengrid][chosenperson].getCode();
	}

	//DRAWING NAME BANNER
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight, sidebarwidth, namebannersize, DARKGRAY);
	DrawTextEx(codingfontbold, workers[chosengrid][chosenperson].name, { windowwidth - 0.6 * sidebarwidth,sidebarbuttonheight + 10 }, 20, 5, WHITE);

	//Drawing Worker State
	DrawTextEx(codingfontbold, GetEnumEquivalent(workers[chosengrid][chosenperson].activity), { windowwidth - 0.2 * sidebarwidth, sidebarbuttonheight + 10 }, 20, 1, GREEN);
	DrawTextEx(codingfontbold, TextFormat("E: %d", workers[chosengrid][chosenperson].energy), { windowwidth - sidebarwidth, sidebarbuttonheight + 10 }, 17, 1, YELLOW);
	DrawTextEx(codingfontbold, TextFormat("P: %d", workers[chosengrid][chosenperson].productivity), { windowwidth - 0.85 * sidebarwidth, sidebarbuttonheight + 10 }, 17, 1, YELLOW);


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
							workers[chosengrid].push_back(Worker(screenbuffer, gridheight / 2));
							workers[chosengrid].back().grid = &grid;
							workers[chosengrid].back().workspace = { x + screenbuffer,y + screenbuffer };
							workers[chosengrid][workers[chosengrid].size() - 1].pathfind({ x + screenbuffer,y + screenbuffer });
						}
						else
							lunchpositions.push_back({ { x + screenbuffer,y + screenbuffer },0 });
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

	static Rectangle CancelButton = { windowwidth - sidebarwidth - moneybarwidth - 180,10,150,60 };
	static Rectangle AcceptButton = { windowwidth - sidebarwidth - moneybarwidth - 380,10,150,60 };

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
				drawzone = x;
			}
		}
		else
			DrawRectangleRounded(button, 4, 10, DARKBLUE);

		DrawRectangleRoundedLinesEx(button, 4, 10, 5, { 220,220,220,220 });
		DrawCircle(button.x + button.width * 0.15, button.y + button.height * 0.5, button.height * 0.32, WHITE);
		DrawCircle(button.x + button.width * 0.15, button.y + button.height * 0.5, button.height * 0.3, get<0>(areaitems[x]));
		DrawTextEx(codingfontbold, get<1>(areaitems[x]), { button.x + button.width * 0.3f ,button.y + button.height * 0.4f }, 25, 2, WHITE);
		DrawTextEx(codingfontbold, TextFormat("$%d", get<2>(areaitems[x])), { button.x + button.width * 0.8f ,button.y + button.height * 0.4f }, 25, 2, GREEN);
	}


	//DRAW MODE
	if (drawmode == true)
	{
		DrawCircleV(mousepos, 5, get<0>(areaitems[drawzone]));

		//CANCEL BUTTON
		DrawRectangleRounded(CancelButton, 5, 10, MAROON);
		DrawRectangleRoundedLinesEx(CancelButton, 5, 10, 5, WHITE);
		DrawTextEx(codingfontbold, "Cancel", { CancelButton.x + CancelButton.width / 5,CancelButton.y + CancelButton.height / 3 }, 23, 2, WHITE);
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
					if (CheckCollisionPointRec(mousepos, { (x + screenbuffer) * linewidth,(y + screenbuffer) * lineheight,linewidth,lineheight }))
					{
						clicks++;
						if (clicks % 2 == 1)
						{
							startpos = { x,y };
							return;
						}
						else
						{
							if (abs(x - startpos.first) + abs(y - startpos.second) > 0)
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
			Vector2 pos = { (windowwidth - sidebarwidth) / 2 - 80,windowheight - 50 };
			DrawRectangle((startpos.first + screenbuffer) * linewidth, (startpos.second + screenbuffer) * lineheight, (endpos.first - startpos.first) * linewidth, (endpos.second - startpos.second) * lineheight, get<0>(areaitems[drawzone]));
			DrawTextEx(codingfontbold, TextFormat("$%d", totalcost), pos, 40, 5, GREEN);
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

	Vector2 buttondims = { sidebarwidth / 3, (windowheight - sidebarbuttonheight) / workernum };
	for (int x = 0; x < workernum; x++)
	{
		for (int y = 3; y >= 1; y--)
		{
			int cost = 0;
			Rectangle button = { windowwidth - y * buttondims.x + 6, sidebarbuttonheight + x * buttondims.y + 15, buttondims.x - 12, buttondims.y - 30 };

			//DRAW BUTTON AND BOUNDARIES
			if (CheckCollisionPointRec(mousepos, button) && drawmode == false)
			{
				DrawRectangleRec(button, { 0,131,255,255 });
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
				cost = get<2>(workertypes[workernum - 1 - x]);
				DrawTextEx(codingfontbold, "Junior", { button.x + button.width * 0.33f,button.y + button.height * bannerheight }, 15, 1, iconcolor);
			}
			else if (y == 2)
			{
				iconcolor = { 220,220,0,255 };
				cost = get<2>(workertypes[workernum - 1 - x]) * 20;
				DrawTextEx(codingfontbold, "Senior", { button.x + button.width * 0.33f,button.y + button.height * bannerheight }, 15, 1, iconcolor);
			}
			else if (y == 1)
			{
				iconcolor = PURPLE;
				cost = get<2>(workertypes[workernum - 1 - x]) * 80;
				DrawTextEx(codingfontbold, "Specialist", { button.x + button.width * 0.23f,button.y + button.height * bannerheight }, 15, 1, iconcolor);
			}
			DrawTextEx(codingfontbold, TextFormat("$%d", cost), { button.x + button.width * 0.3f,button.y + button.height * 0.8f }, 25, 1, GREEN);


			//DRAW NAME AND ICON
			switch (x)
			{
			case 0:
				GuiDrawIcon(ICON_MONITOR, button.x + button.width * iconspacing, button.y + button.height / 15, iconsize, iconcolor);
				DrawTextEx(codingfontbold, "Software", { button.x + button.width * 0.13f,button.y + button.height * 0.6f }, 22, 3, WHITE);
				break;
			case 1:
				GuiDrawIcon(ICON_GEAR, button.x + button.width * iconspacing, button.y + button.height / 15, iconsize, iconcolor);
				DrawTextEx(codingfontbold, "Hardware", { button.x + button.width * 0.13f,button.y + button.height * 0.6f }, 22, 3, WHITE);
				break;
			case 2:
				GuiDrawIcon(ICON_WAVE, button.x + button.width * iconspacing, button.y + button.height / 15, iconsize, iconcolor);
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

		for (int y = workbenchsize / 2; y < grid.size() - workbenchsize / 2; y++)
		{
			for (int x = workbenchsize / 2; x < grid[0].size() - workbenchsize / 2; x++)
			{
				Rectangle chosenblock = { (x + screenbuffer) * linewidth,(y + screenbuffer) * lineheight,linewidth,lineheight };
				Rectangle block = { (x + screenbuffer - workbenchsize / 2) * linewidth,(y + screenbuffer - workbenchsize / 2) * lineheight,linewidth * (workbenchsize),lineheight * (workbenchsize) };
				if (CheckCollisionPointRec(mousepos, chosenblock))
				{
					DrawRectangleRec(block, { 40,255,40,255 });
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						for (int i = y - workbenchsize / 2; i <= y + workbenchsize / 2; i++)
						{
							for (int j = x - workbenchsize / 2; j <= x + workbenchsize / 2; j++)
							{
								if (grid[i][j] != 2 || grid[i][j] < 0)
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
								grid[i][j] = -2;
							}
						}

						//Calculate Cost
						int workernetcost = 0;
						if (chosenposition == 3)workernetcost = get<2>(workertypes[workernum - 1 - chosenworkertype]);
						else if (chosenposition == 2)workernetcost = get<2>(workertypes[workernum - 1 - chosenworkertype]) * 20;
						else if (chosenposition == 1)workernetcost = get<2>(workertypes[workernum - 1 - chosenworkertype]) * 80;

						//Subtract Cost
						if (workernetcost <= totalmoney)totalmoney -= workernetcost;

						//Spawn Worker
						workers[chosengrid].push_back(Worker(screenbuffer, gridheight / 2));
						workers[chosengrid].back().grid = &grid;
						workers[chosengrid].back().workspace = { x + screenbuffer,y + screenbuffer };
						workers[chosengrid][workers[chosengrid].size() - 1].pathfind({ x + screenbuffer ,y + screenbuffer });
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
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight), { 0,100,50,255 });
	DrawRectangleLinesEx({ windowwidth - sidebarwidth, sidebarbuttonheight + shopbuttonheight, sidebarwidth, windowheight - (sidebarbuttonheight + shopbuttonheight) }, areashopboundarywidth, { 220,220,220,220 });

	//Bank Icon
	DrawTextureEx(bankicon, { windowwidth - sidebarwidth * 0.75f, windowheight * 0.2 }, 0, 1, WHITE);

	//Money Invested
	DrawTextEx(codingfontbold, "Money Invested", { windowwidth - sidebarwidth * 0.8,windowheight * 0.55 }, 38, 1, WHITE);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", bank.player_savings), { windowwidth - sidebarwidth * (0.55f + ((bank.player_savings / 1000 > 0) ? 0.06f : 0.0f)),windowheight * 0.61 }, 30, 1, GREEN);

	//Invest Amount
	DrawTextEx(codingfontbold, "Invest:", { windowwidth - sidebarwidth * 0.75f ,windowheight * 0.72 }, 25, 1, WHITE);
	if (GuiTextBox({ windowwidth - sidebarwidth * 0.52f ,windowheight * 0.7,sidebarwidth * 0.4f,windowheight * 0.06f }, inputamount, 10, true))
	{
		int amount = 0;
		try
		{
			amount = stoi(inputamount);
		}
		catch (exception e) {};

		if (amount <= totalmoney && amount >= 0)
		{
			bank.invest(amount);
			totalmoney -= amount;
		}
		inputamount[0] = '\0';
	}

	//Extract Amount
	DrawTextEx(codingfontbold, "Take Out:", { windowwidth - sidebarwidth * 0.68f ,windowheight * 0.81 }, 25, 1, WHITE);
	DrawTextEx(codingfontbold, TextFormat("$%0.f", outputamount), { windowwidth - sidebarwidth * 0.43f ,windowheight * 0.81 }, 25, 1, GREEN);
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

void DrawBankTab()
{
	if (GuiButton({ windowwidth - sidebarwidth ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "Invest"))
		BankState = Invest;
	if (GuiButton({ windowwidth - 2 * sidebarwidth / 3 ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "FD"))
		BankState = FD;
	if (GuiButton({ windowwidth - sidebarwidth / 3 ,sidebarbuttonheight,sidebarwidth / 3,shopbuttonheight }, "Loan"))
		BankState = Loan;

	switch (BankState)
	{
	case Invest:
		DrawInvestTab();
		break;
	case FD:
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
			for (int j = (totalticks / updatetime - stockperiod < 0) ? 0 : totalticks / updatetime - stockperiod; j < competitors[i].competitor.size() - 1; j++)
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

void DrawLanding() {
	int backgroundWidth = background.width;
	int backgroundHeight = background.height;
	int destWidth = windowwidth;
	int destHeight = windowheight;

	Rectangle sourceRect;
	sourceRect.x = 0;
	sourceRect.y = 0;
	sourceRect.width = backgroundWidth;
	sourceRect.height = backgroundHeight;

	Rectangle destRect;
	destRect.x = 0;
	destRect.y = 0;
	destRect.width = destWidth;
	destRect.height = destHeight;

	Vector2 origin;
	origin.x = 0;
	origin.y = 0;

	DrawTexturePro(background, sourceRect, destRect, origin, 0.0f, WHITE);

	Color golden = { 255, 215, 0, 255 };

	int logoFontSize = 55;
	int logoTextWidth = MeasureText("WELCOME TO AG.AI.N", logoFontSize);
	int logoX = (windowwidth - logoTextWidth) / 2;
	int logoY = 80;

	DrawText("WELCOME TO AG.AI.N", logoX, logoY, logoFontSize, golden);

	// Video area dimensions
	int videoWidth = windowwidth / 2;
	int videoHeight = 200;
	int videoX = (windowwidth - videoWidth) / 2;
	int videoY = 160;

	// Draw the background for the video area
	DrawRectangle(videoX, videoY, videoWidth, videoHeight, WHITE);

	// Simple grid parameters for preview
	int gridCols = 15;
	int gridRows = 8;
	float cellWidth = (float)videoWidth / gridCols;
	float cellHeight = (float)videoHeight / gridRows;

	// Draw cells with some colors similar to the main screen
	for (int y = 0; y < gridRows; y++) {
		for (int x = 0; x < gridCols; x++) {
			Color cellColor = WHITE;

			// Create a border of brown cells
			if (x == 0 || x == gridCols - 1 || y == 0 || y == gridRows - 1) {
				cellColor = BROWN;
			}
			// Add some yellow cells as workers/items
			else if ((x == 3 && y == 2) || (x == 7 && y == 4) || (x == 10 && y == 5)) {
				cellColor = YELLOW;
			}
			// Add some green cells as workers/items
			else if ((x == 5 && y == 3) || (x == 8 && y == 2) || (x == 11 && y == 3)) {
				cellColor = GREEN;
			}
			// Add some blue cells as workers/items
			else if ((x == 4 && y == 5) || (x == 9 && y == 3)) {
				cellColor = BLUE;
			}
			// Add some red cells as workers/items
			else if ((x == 2 && y == 4) || (x == 6 && y == 5)) {
				cellColor = RED;
			}

			// Draw the cell
			DrawRectangle(
				videoX + x * cellWidth,
				videoY + y * cellHeight,
				cellWidth,
				cellHeight,
				cellColor
			);
		}
	}

	// Draw grid lines
	for (int x = 0; x <= gridCols; x++) {
		DrawLine(
			videoX + x * cellWidth,
			videoY,
			videoX + x * cellWidth,
			videoY + videoHeight,
			BLACK
		);
	}

	for (int y = 0; y <= gridRows; y++) {
		DrawLine(
			videoX,
			videoY + y * cellHeight,
			videoX + videoWidth,
			videoY + y * cellHeight,
			BLACK
		);
	}

	// Draw a few worker circles on some cells to simulate the game
	DrawCircle(videoX + 3 * cellWidth + cellWidth / 2, videoY + 2 * cellHeight + cellHeight / 2, cellWidth / 3, BLACK);
	DrawCircle(videoX + 7 * cellWidth + cellWidth / 2, videoY + 4 * cellHeight + cellHeight / 2, cellWidth / 3, BLACK);
	DrawCircle(videoX + 4 * cellWidth + cellWidth / 2, videoY + 5 * cellHeight + cellHeight / 2, cellWidth / 3, BLACK);

	// Draw border around video area
	DrawRectangleLines(videoX, videoY, videoWidth, videoHeight, BLACK);

	// Draw "PREVIEW" label
	DrawText("GAME PREVIEW", videoX + 10, videoY + 10, 16, BLACK);

	int buttonWidth = 260;
	int buttonHeight = 65;
	int buttonX = (windowwidth - buttonWidth) / 2;
	int startY = videoY + videoHeight + 50;
	int gap = 25;

	Rectangle playButton, settingsButton, isaButton, helpButton;

	playButton.x = buttonX;
	playButton.y = startY;
	playButton.width = buttonWidth;
	playButton.height = buttonHeight;

	settingsButton.x = buttonX;
	settingsButton.y = startY + (buttonHeight + gap);
	settingsButton.width = buttonWidth;
	settingsButton.height = buttonHeight;

	isaButton.x = buttonX;
	isaButton.y = startY + 2 * (buttonHeight + gap);
	isaButton.width = buttonWidth;
	isaButton.height = buttonHeight;

	helpButton.x = buttonX;
	helpButton.y = startY + 3 * (buttonHeight + gap);
	helpButton.width = buttonWidth;
	helpButton.height = buttonHeight;

	Color buttonColor = { 218, 165, 32, 255 };
	Color hoverColor = { 255, 223, 0, 255 };
	Vector2 mousePosition = GetMousePosition();

	float enlargeFactor = 1.1f;
	int liftAmount = 5;

	auto DrawButton = [&](Rectangle button, const char* text) {
		Rectangle modifiedButton = button;
		Color currentColor = buttonColor;

		if (CheckCollisionPointRec(mousePosition, button)) {
			modifiedButton.width *= enlargeFactor;
			modifiedButton.height *= enlargeFactor;
			modifiedButton.x -= (modifiedButton.width - button.width) / 2;
			modifiedButton.y -= liftAmount;
			currentColor = hoverColor;
		}

		DrawRectangleRounded(modifiedButton, 0.3f, 10, currentColor);
		DrawText(text, modifiedButton.x + (modifiedButton.width - MeasureText(text, 27)) / 2, modifiedButton.y + 20, 27, BLACK);

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, button)) {
			if (strcmp(text, "PLAY") == 0) ScreenMode = View;
			if (strcmp(text, "SETTINGS") == 0) ScreenMode = Settings;
			if (strcmp(text, "ISA") == 0) ScreenMode = ISA;
			if (strcmp(text, "HELP") == 0) ScreenMode = Help;
		}
		};

	DrawButton(playButton, "PLAY");
	DrawButton(settingsButton, "SETTINGS");
	DrawButton(isaButton, "ISA");
	DrawButton(helpButton, "HELP");
}



Rectangle back_button = { windowwidth - 210, 10, 200, 50 };


void DrawISAScreen() {
	Color second = { 40, 40, 40, 240 };
	int backgroundWidth = background.width;
	int backgroundHeight = background.height;
	int destWidth = windowwidth;
	int destHeight = windowheight;
	Rectangle sourceRect = { 0, 0, backgroundWidth, backgroundHeight };
	Rectangle destRect = { 0, 0, destWidth, destHeight };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(background, sourceRect, destRect, origin, 0.0f, WHITE);

	Color golden = { 255, 215, 0, 255 };
	Color buttonColor = { 50, 50, 50, 255 };
	Color hoverColor = { 50, 150, 50, 255 };

	int titleFontSize = 55;
	int titleTextWidth = MeasureText("INSTRUCTION SET ARCHITECTURE", titleFontSize);
	int titleX = (windowwidth - titleTextWidth) / 2;
	int titleY = 50;
	DrawText("INSTRUCTION SET ARCHITECTURE", titleX, titleY, titleFontSize, golden);

	struct ISAEntry {
		const char* symbol;
		const char* description;
	};

	ISAEntry isaInstructions[] = {
		{"$", "Generic token, $value.attribute"},
		{"!", "Label token, for jump label !j"},
		{"if", "Conditional statement"},
		{"jump", "Jump to label"},
		{"moveto", "Move to target"},
		{"eat", "Consume food"},
		{"buy", "Purchase item"},
		{"talk", "Interact with a character"},
		{"work", "Perform a job"},
		{"break", "Pause action"},
		{"exit", "Exit program"},
		{"submit", "Submit action"}
	};

	int contentX = 100;
	int contentWidth = windowwidth - 200;
	int contentStartY = 150;
	int contentHeight = windowheight - 300;
	Rectangle contentArea = { contentX, contentStartY, contentWidth, contentHeight };

	static int scrollOffset = 0;
	int maxScroll = 200;

	if (IsKeyDown(KEY_DOWN) && scrollOffset < maxScroll) scrollOffset += 10;
	if (IsKeyDown(KEY_UP) && scrollOffset > 0) scrollOffset -= 10;

	int mouseWheel = GetMouseWheelMove();
	if (mouseWheel < 0 && scrollOffset < maxScroll) scrollOffset += 20;
	if (mouseWheel > 0 && scrollOffset > 0) scrollOffset -= 20;

	if (scrollOffset < 0) scrollOffset = 0;
	if (scrollOffset > maxScroll) scrollOffset = maxScroll;

	BeginScissorMode(contentX, contentStartY, contentWidth, contentHeight);
	Color smth = { 30, 30, 30, 150 };
	DrawRectangleRec(contentArea, smth);

	Vector2 mousePosition = GetMousePosition();
	int columns = 4;
	int rows = 3;
	int boxWidth = (contentWidth - (columns - 1) * 30) / columns;
	int boxHeight = 120;
	int startX = contentX + 15;
	int startY = contentStartY - scrollOffset + 15;

	int hoveredIndex = -1;

	for (int i = 0; i < 12; i++) {
		int row = i / columns;
		int col = i % columns;
		int x = startX + col * (boxWidth + 30);
		int y = startY + row * (boxHeight + 30);

		Rectangle button = { x, y, boxWidth, boxHeight };
		bool hovered = CheckCollisionPointRec(mousePosition, button);

		if (hovered && mousePosition.y >= contentStartY && mousePosition.y <= contentStartY + contentHeight) {
			hoveredIndex = i;
		}

		Color currentColor = (hoveredIndex == i) ? hoverColor : buttonColor;
		DrawRectangleRounded(button, 0.3f, 10, currentColor);
		DrawText(isaInstructions[i].symbol, x + boxWidth / 2 - MeasureText(isaInstructions[i].symbol, 40) / 2, y + boxHeight / 2 - 20, 40, RAYWHITE);
	}

	EndScissorMode();

	if (hoveredIndex >= 0) {
		int tooltipWidth = 500;
		int tooltipHeight = 80;
		int tooltipX = (windowwidth - tooltipWidth) / 2;
		int tooltipY = windowheight - 200;

		Rectangle tooltipRect = { tooltipX, tooltipY, tooltipWidth, tooltipHeight };
		DrawRectangleRounded(tooltipRect, 0.2f, 10, second);
		DrawRectangleLinesEx(tooltipRect, 2, golden);
		DrawText(isaInstructions[hoveredIndex].description, tooltipX + 20, tooltipY + tooltipHeight / 2 - 16, 32, golden);
	}

	int buttonWidth = 260;
	int buttonHeight = 65;
	int buttonX = (windowwidth - buttonWidth) / 2;
	int buttonY = windowheight - 100;
	Rectangle backButton = { buttonX, buttonY, buttonWidth, buttonHeight };
	Color backButtonColor = { 218, 165, 32, 255 };
	Color backHoverColor = { 255, 223, 0, 255 };
	Color currentBackColor = CheckCollisionPointRec(mousePosition, backButton) ? backHoverColor : backButtonColor;

	DrawRectangleRounded(backButton, 0.3f, 10, currentBackColor);
	DrawText("BACK", buttonX + (buttonWidth - MeasureText("BACK", 27)) / 2, buttonY + 20, 27, BLACK);

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, backButton)) {
		ScreenMode = Landing;
	}
}





void DrawSettingsScreen() {
	Color second = { 40, 40, 40, 240 };
	int backgroundWidth = background.width;
	int backgroundHeight = background.height;
	int destWidth = windowwidth;
	int destHeight = windowheight;
	Rectangle sourceRect = { 0, 0, backgroundWidth, backgroundHeight };
	Rectangle destRect = { 0, 0, destWidth, destHeight };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(background, sourceRect, destRect, origin, 0.0f, WHITE);

	Color golden = { 255, 215, 0, 255 };
	Color buttonColor = { 50, 50, 50, 255 };
	Color hoverColor = { 50, 150, 50, 255 };
	Color sliderBgColor = { 100, 100, 100, 255 };
	Color sliderColor = { 218, 165, 32, 255 };

	int titleFontSize = 55;
	int titleTextWidth = MeasureText("SETTINGS", titleFontSize);
	int titleX = (windowwidth - titleTextWidth) / 2;
	int titleY = 50;
	DrawText("SETTINGS", titleX, titleY, titleFontSize, golden);

	struct SettingEntry {
		const char* name;
		const char* description;
		bool hasSlider;
	};

	SettingEntry settingsOptions[] = {
		{"SOUND", "Adjust sound volume level", true},
		{"MUSIC", "Adjust music volume level", true},
		{"DIFFICULTY", "Change game difficulty", true},
		{"GRAPHICS", "Toggle high quality graphics", false},
		{"FULLSCREEN", "Toggle fullscreen mode", false},
		{"CONTROLS", "Customize keyboard controls", false},
		{"LANGUAGE", "Change display language", false},
		{"CREDITS", "View game credits", false}
	};

	static int scrollOffset = 0;
	int maxScroll = 300;


	if (IsKeyDown(KEY_DOWN) && scrollOffset < maxScroll) scrollOffset += 10;
	if (IsKeyDown(KEY_UP) && scrollOffset > 0) scrollOffset -= 10;


	int mouseWheel = GetMouseWheelMove();
	if (mouseWheel < 0 && scrollOffset < maxScroll) scrollOffset += 20;
	if (mouseWheel > 0 && scrollOffset > 0) scrollOffset -= 20;


	if (scrollOffset < 0) scrollOffset = 0;
	if (scrollOffset > maxScroll) scrollOffset = maxScroll;

	Vector2 mousePosition = GetMousePosition();

	static float soundVolume = 0.8f;
	static float musicVolume = 0.7f;
	static float difficulty = 0.5f;

	int settingsCount = sizeof(settingsOptions) / sizeof(SettingEntry);
	int boxWidth = windowwidth - 300;
	int boxHeight = 80;
	int startX = 150;
	int startY = 150 - scrollOffset;
	int spacing = 20;

	for (int i = 0; i < settingsCount; i++) {
		int y = startY + i * (boxHeight + spacing);

		if (y + boxHeight < 0 || y > windowheight) continue;

		Rectangle button = { startX, y, boxWidth, boxHeight };
		bool hovered = CheckCollisionPointRec(mousePosition, button);
		Color currentColor = hovered ? hoverColor : buttonColor;

		DrawRectangleRounded(button, 0.3f, 10, currentColor);
		DrawText(settingsOptions[i].name, startX + 30, y + boxHeight / 2 - 15, 30, RAYWHITE);

		if (settingsOptions[i].hasSlider) {
			int sliderWidth = 300;
			int sliderHeight = 20;
			int sliderX = startX + boxWidth - sliderWidth - 30;
			int sliderY = y + (boxHeight - sliderHeight) / 2;

			float* sliderValue = NULL;
			if (i == 0) sliderValue = &soundVolume;
			else if (i == 1) sliderValue = &musicVolume;
			else if (i == 2) sliderValue = &difficulty;

			Rectangle sliderRect = { sliderX, sliderY, sliderWidth, sliderHeight };
			DrawRectangleRounded(sliderRect, 0.5f, 10, sliderBgColor);

			int knobX = sliderX + (int)(sliderWidth * (*sliderValue));
			int knobSize = 30;
			DrawCircle(knobX, sliderY + sliderHeight / 2, knobSize / 2, sliderColor);

			if (hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
				float mouseXRel = (mousePosition.x - sliderX) / sliderWidth;

				// Replace Clamp with direct min/max logic
				if (mouseXRel < 0.0f) mouseXRel = 0.0f;
				if (mouseXRel > 1.0f) mouseXRel = 1.0f;

				*sliderValue = mouseXRel;
			}

			char percentText[10];
			sprintf(percentText, "%d%%", (int)(*sliderValue * 100));
			DrawText(percentText, sliderX + sliderWidth + 10, sliderY, 20, golden);
		}
		else {
			int toggleSize = 30;
			int toggleX = startX + boxWidth - toggleSize - 30;
			int toggleY = y + (boxHeight - toggleSize) / 2;

			static bool toggleStates[8] = { false };
			bool* currentToggle = &toggleStates[i - 3];

			if (i >= 3) {
				Rectangle toggleRect = { toggleX, toggleY, toggleSize, toggleSize };
				bool toggleHovered = CheckCollisionPointRec(mousePosition, toggleRect);

				if (toggleHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					*currentToggle = !(*currentToggle);
				}

				DrawRectangleRounded(toggleRect, 0.5f, 8, *currentToggle ? golden : sliderBgColor);
				if (*currentToggle) {
					DrawText("?", toggleX + 8, toggleY + 5, 20, BLACK);
				}
			}
		}

		if (hovered) {
			int tooltipX = startX;
			int tooltipY = y + boxHeight + 5;
			int tooltipWidth = 400;
			int tooltipHeight = 60;
			DrawRectangle(tooltipX, tooltipY, tooltipWidth, tooltipHeight, second);
			DrawText(settingsOptions[i].description, tooltipX + 20, tooltipY + 20, 24, golden);
		}
	}

	int buttonWidth = 260;
	int buttonHeight = 65;
	int buttonX = (windowwidth - buttonWidth) / 2;
	int buttonY = windowheight - 100;
	Rectangle backButton = { buttonX, buttonY, buttonWidth, buttonHeight };
	Color backButtonColor = { 218, 165, 32, 255 };
	Color backHoverColor = { 255, 223, 0, 255 };
	Color currentBackColor = CheckCollisionPointRec(mousePosition, backButton) ? backHoverColor : backButtonColor;

	DrawRectangleRounded(backButton, 0.3f, 10, currentBackColor);
	DrawText("BACK", buttonX + (buttonWidth - MeasureText("BACK", 27)) / 2, buttonY + 20, 27, BLACK);

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, backButton)) {
		ScreenMode = Landing;
	}

	int scrollbarWidth = 10;
	int scrollbarHeight = windowheight - 200;
	int scrollbarX = windowwidth - 50;
	int scrollbarY = 100;

	Rectangle scrollbarBg = { scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight };
	DrawRectangleRounded(scrollbarBg, 0.5f, 5, sliderBgColor);

	float scrollPercentage = (float)scrollOffset / maxScroll;
	int handleHeight = scrollbarHeight / 3;
	int handleY = scrollbarY + (int)((scrollbarHeight - handleHeight) * scrollPercentage);

	Rectangle scrollHandle = { scrollbarX, handleY, scrollbarWidth, handleHeight };
	DrawRectangleRounded(scrollHandle, 0.5f, 5, sliderColor);

	Rectangle scrollbarRect = { scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight };
	if (CheckCollisionPointRec(mousePosition, scrollbarRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		float relativeY = (mousePosition.y - scrollbarY) / scrollbarHeight;


		if (relativeY < 0.0f) relativeY = 0.0f;
		if (relativeY > 1.0f) relativeY = 1.0f;

		scrollOffset = (int)(relativeY * maxScroll);


		if (scrollOffset < 0) scrollOffset = 0;
		if (scrollOffset > maxScroll) scrollOffset = maxScroll;
	}
}

void DrawHelpScreen() {
	Color background_color = { 40, 40, 40, 240 };
	Color golden = { 255, 215, 0, 255 };
	Color textColor = { 220, 220, 220, 255 };
	Color headerColor = { 218, 165, 32, 255 };
	Color dividerColor = { 150, 150, 150, 255 };

	int backgroundWidth = background.width;
	int backgroundHeight = background.height;
	int destWidth = windowwidth;
	int destHeight = windowheight;
	Rectangle sourceRect = { 0, 0, backgroundWidth, backgroundHeight };
	Rectangle destRect = { 0, 0, destWidth, destHeight };
	Vector2 origin = { 0, 0 };
	DrawTexturePro(background, sourceRect, destRect, origin, 0.0f, WHITE);

	int titleFontSize = 55;
	int titleTextWidth = MeasureText("HELP", titleFontSize);
	int titleX = (windowwidth - titleTextWidth) / 2;
	int titleY = 50;
	DrawText("HELP", titleX, titleY, titleFontSize, golden);

	int contentX = 100;
	int contentWidth = windowwidth - 200;
	int contentStartY = 150;
	int contentHeight = windowheight - 250;
	Rectangle contentArea = { contentX, contentStartY, contentWidth, contentHeight };

	static int scrollOffset = 0;
	int maxScroll = 1200;

	if (IsKeyDown(KEY_DOWN) && scrollOffset < maxScroll) scrollOffset += 10;
	if (IsKeyDown(KEY_UP) && scrollOffset > 0) scrollOffset -= 10;

	int mouseWheel = GetMouseWheelMove();
	if (mouseWheel < 0 && scrollOffset < maxScroll) scrollOffset += 20;
	if (mouseWheel > 0 && scrollOffset > 0) scrollOffset -= 20;

	if (scrollOffset < 0) scrollOffset = 0;
	if (scrollOffset > maxScroll) scrollOffset = maxScroll;

	BeginScissorMode(contentX, contentStartY, contentWidth, contentHeight);
	Color smth1 = { 30, 30, 30, 200 };
	DrawRectangleRec(contentArea, smth1);

	int currentY = contentStartY - scrollOffset;

	struct HelpSection {
		const char* question;
		const char* answer;
		int imageIndex;
	};

	HelpSection helpSections[] = {
		{
			"How do I start a new game?",
			"To start a new game, select 'NEW GAME' from the main menu. You'll be prompted to choose your difficulty level before the game begins. Higher difficulty means more challenges and fewer resources.",
			0
		},
		{
			"What are the basic controls?",
			"Move your character using WASD or arrow keys. Use the mouse to aim and left-click to attack. Press E to interact with objects and I to open inventory. Space bar is used for jumping and Shift for running.",
			1
		},
		{
			"How do I save my progress?",
			"The game automatically saves your progress when you reach checkpoints (indicated by a golden icon). You can also manually save by accessing the pause menu (ESC key) and selecting 'SAVE GAME'.",
			-1
		},
		{
			"What happens when I die?",
			"When you die, you'll respawn at the last checkpoint. You'll lose some collected resources but will keep all major items and progress. On higher difficulty levels, the penalty for death is more severe.",
			2
		},
		{
			"How do I change game settings?",
			"Access the Settings menu from either the main menu or the pause menu during gameplay. There you can adjust sound volume, music volume, difficulty, graphics quality, and more.",
			-1
		},
		{
			"What are the different difficulty levels?",
			"There are three difficulty levels: Easy (more resources, weaker enemies), Normal (balanced experience), and Hard (limited resources, stronger enemies, fewer checkpoints).",
			3
		},
		{
			"How do I unlock new abilities?",
			"New abilities are unlocked by finding special items throughout the game world, defeating certain boss enemies, or by spending skill points earned when leveling up. Check your abilities screen to see what's available.",
			-1
		},
		{
			"What are the crafting resources?",
			"The main crafting resources are Wood, Stone, Metal, and Crystal. Rarer resources include Ancient Artifacts and Magical Essence. Different combinations create different items and upgrades.",
			4
		}
	};

	int sectionsCount = sizeof(helpSections) / sizeof(HelpSection);

	Vector2 lineStart = { contentX, currentY };
	Vector2 lineEnd = { contentX + contentWidth, currentY };
	DrawLineEx(lineStart, lineEnd, 3, dividerColor);
	currentY += 20;

	for (int i = 0; i < sectionsCount; i++) {
		int questionFontSize = 28;
		DrawText(helpSections[i].question, contentX, currentY, questionFontSize, headerColor);
		currentY += questionFontSize + 15;

		int answerFontSize = 22;
		const char* text = helpSections[i].answer;
		int textLength = strlen(text);
		int startOfLine = 0;
		int lastSpace = -1;

		for (int j = 0; j < textLength; j++) {
			if (text[j] == ' ') {
				lastSpace = j;
			}

			int width = MeasureText(TextSubtext(text, startOfLine, j - startOfLine + 1), answerFontSize);
			if (width > contentWidth - 50) {
				DrawText(TextSubtext(text, startOfLine, lastSpace - startOfLine), contentX + 25, currentY, answerFontSize, textColor);
				currentY += answerFontSize + 5;
				startOfLine = lastSpace + 1;
			}
		}

		if (startOfLine < textLength) {
			DrawText(TextSubtext(text, startOfLine, textLength - startOfLine), contentX + 25, currentY, answerFontSize, textColor);
			currentY += answerFontSize + 5;
		}

		if (helpSections[i].imageIndex >= 0) {
			int imageHeight = 150;
			int imageWidth = 250;
			int imageX = contentX + (contentWidth - imageWidth) / 2;

			Color imageColors[] = {
				{ 150, 100, 50, 255 },
				{ 80, 120, 200, 255 },
				{ 200, 80, 80, 255 },
				{ 100, 180, 100, 255 },
				{ 180, 100, 180, 255 }
			};

			int colorIndex = helpSections[i].imageIndex % 5;

			Rectangle imageRect = { imageX, currentY, imageWidth, imageHeight };
			DrawRectangleRounded(imageRect, 0.2f, 10, imageColors[colorIndex]);
			DrawText("EXAMPLE IMAGE", imageX + 50, currentY + imageHeight / 2 - 10, 20, BLACK);

			currentY += imageHeight + 30;
		}
		else {
			currentY += 30;
		}

		Vector2 divLineStart = { contentX, currentY };
		Vector2 divLineEnd = { contentX + contentWidth, currentY };
		DrawLineEx(divLineStart, divLineEnd, 3, dividerColor);
		currentY += 30;
	}

	EndScissorMode();

	Vector2 mousePosition = GetMousePosition();
	int buttonWidth = 260;
	int buttonHeight = 65;
	int buttonX = (windowwidth - buttonWidth) / 2;
	int buttonY = windowheight - 100;
	Rectangle backButton = { buttonX, buttonY, buttonWidth, buttonHeight };
	Color backButtonColor = { 218, 165, 32, 255 };
	Color backHoverColor = { 255, 223, 0, 255 };
	Color currentBackColor = CheckCollisionPointRec(mousePosition, backButton) ? backHoverColor : backButtonColor;

	DrawRectangleRounded(backButton, 0.3f, 10, currentBackColor);
	DrawText("BACK", buttonX + (buttonWidth - MeasureText("BACK", 27)) / 2, buttonY + 20, 27, BLACK);

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePosition, backButton)) {
		ScreenMode = Landing;
	}

	int scrollbarWidth = 10;
	int scrollbarHeight = contentHeight;
	int scrollbarX = windowwidth - 50;
	int scrollbarY = contentStartY;

	Rectangle scrollbarBg = { scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight };
	Color smth2 = { 100, 100, 100, 255 };
	DrawRectangleRounded(scrollbarBg, 0.5f, 5, smth2);

	float scrollPercentage = (float)scrollOffset / maxScroll;
	int handleHeight = scrollbarHeight / 3;
	int handleY = scrollbarY + (int)((scrollbarHeight - handleHeight) * scrollPercentage);

	Rectangle scrollHandle = { scrollbarX, handleY, scrollbarWidth, handleHeight };
	smth2 = { 218, 165, 32, 255 };
	DrawRectangleRounded(scrollHandle, 0.5f, 5, smth2);

	Rectangle scrollbarRect = { scrollbarX, scrollbarY, scrollbarWidth, scrollbarHeight };
	if (CheckCollisionPointRec(mousePosition, scrollbarRect) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
		float relativeY = (mousePosition.y - scrollbarY) / scrollbarHeight;

		if (relativeY < 0.0f) relativeY = 0.0f;
		if (relativeY > 1.0f) relativeY = 1.0f;

		scrollOffset = (int)(relativeY * maxScroll);

		if (scrollOffset < 0) scrollOffset = 0;
		if (scrollOffset > maxScroll) scrollOffset = maxScroll;
	}
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

	//Set Font
	codingfont = LoadFont("./fonts/dina10px.ttf");
	codingfontbold = LoadFont("./fonts/dina10pxbold.ttf");
	GuiSetFont(codingfont);

	//Initialize Grid
	InitializeGrid(60, 60, 0);
	InitializeGridPrices();
	InitializeShop();
	InitializeHire();
	InitializeSprites();

	ScreenMode = Landing; //DEBUG
	BankState = Invest;
	SidebarState = Money;

	//Add Competitors
	competitors.push_back(RandomGenerator());
	competitors.push_back(RandomGenerator());
	competitors.push_back(RandomGenerator());

	bool clockswitch = false;

	while (!WindowShouldClose())
	{
		mousepos = GetMousePosition();
		frametime = GetFrameTime();

		//GAMETIME
		gametime = std::chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count() % ticksize;
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
		else if (gametime > 50 && clockswitch == true)
			clockswitch = false;

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
		case Landing:
			DrawLanding();
			break;
		case Settings:
			DrawSettingsScreen();
			break;
		case ISA:
			DrawISAScreen();
			break;
		case Help:
			DrawHelpScreen();
			break;
		}
		DrawText(TextFormat("%d", GetFPS()), 10, 10, 25, BLACK);

		EndDrawing();
	}
}