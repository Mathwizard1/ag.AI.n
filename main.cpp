#include <raylib.h>
#include "Raygui.h"

#include "worker.h"
#include "gameConst.h"
using namespace std;

long long int gametime;
float totalmoney = 0;
float moneyincrement = 2000;
float frametime;

short int chosengrid = 0;
long long int quota = 10000;
short int chosenperson = 0;
short int gridheight;
short int gridwidth;
float linewidth;
float lineheight;

Font codingfont;
Font codingfontbold;

enum sidebarstate {
	Code,
	Shop,
	Hire
} SidebarState;

enum shopstate {
	Area,
	Misc
} ShopState;

enum screenmode {
	View,
	Map
} ScreenMode;

std::vector<vector<char*>> textboxesnetwork;
std::vector<vector<short int>> textsizesnetwork;
std::vector<char*> textboxes;
std::vector<short int> textsizes;

std::vector < std::vector<std::vector<short int>>> pooledgridnetwork;
std::vector<std::vector<std::vector<short int>>> gridnetwork;
std::vector<std::vector<short int>> grid;

std::vector<tuple<Color, char*, int>> areaitems;
std::vector<short int> areacolors;

std:: vector<tuple< Color,char*, int>> workertypes;

short int workbenchsize;
Color workbenchcolor;

Vector2 mousepos;

/*
Colors:
1->Lunch Area (Yellow)
2->Work Area (Green)
3->Reception (Blue)
4->Boss (Red)

-1->Boundary
-2->Workspace
*/

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
		else
		{
			workers[y][x].callFunction();
		}
	}
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
}

void InitializeGrid(short int width, short int height, int type)
{
	workbenchcolor = { 120,255,120,255 };
	workbenchsize = 3;

	//Create Grid Network
	for (int y = 0; y < height; y++)
	{
		std::vector<short int> row;

		for (int x = 0; x < width; x++)
		{
				row.push_back(0);
		}
		grid.push_back(row);
	}

	for (int z = 0; z < 25; z++)
	{
		vector<char*> textboxes;
		vector<short int> textsizes;
		vector<Worker> gridworkers;
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
				int* numarray = (int*)malloc(sizeof(int) * 6);

				for (int q = 0; q < grid.size() / mapdims; q++)
				{
					for (int r = 0; r < grid[0].size() / mapdims; r++)
					{
						//cout << grid[y + q][x + r] << endl;
						if(grid[y+q][x+r]>-2)
						numarray[grid[y+q][x+r]+1]++;
					}
				}
				
				int max = 0;
				for (int q = 0; q < 6; q++)
				{
					if (numarray[q] > numarray[max])
						max = q;
				}
				row.push_back(max-1);
				free(numarray);
			}

			pooledgrid.push_back(row);
		}
		pooledgridnetwork.push_back(pooledgrid);
	}

	gridheight = grid.size() + 2 * screenbuffer;
	gridwidth = grid[0].size() + 2 * screenbuffer;
	linewidth = (windowwidth - sidebarwidth-moneybarwidth) / (float)gridwidth;
	lineheight = (windowheight) / (float)gridheight;
}

void DrawMap()
{
	static int mapnumber = gridnetwork.size();
	static int dims = sqrt(mapnumber);

	//Draw Boundary
	DrawRectangleLinesEx({ 0, 0, windowwidth, windowheight },20, GRAY);

	//Draw Lines
	for (int y = 0; y < dims; y++)
	{
		DrawLineEx({ windowwidth / 6,windowheight / 6 + (float)y * (2 * windowheight / 3) / (dims - 1) }, { 5 * windowwidth / 6, windowheight / 6 + (float)y * (2 * windowheight / 3) / (dims - 1) }, 5, BLUE);
		DrawLineEx({ windowwidth / 6 + (float)(y) * (2 * windowwidth / 3) / (dims - 1),windowheight / 6 }, { windowwidth / 6 + (float)y * (2 * windowwidth / 3) / (dims - 1) ,5 * windowheight / 6 }, 5, BLUE);
	}

	//Draw Circles and Grids
	for (int y = 0; y < dims; y++)
	{
		for (int x = 0; x < dims; x++)
		{
			float xpoint = windowwidth / 6 + x * (2 * windowwidth / 3) / (dims - 1);
			float ypoint = windowheight / 6 + y * (2 * windowheight / 3) / (dims - 1);
			float rad = (windowheight - 200) / (2 * dims);

			//CIRCLES
			DrawCircle(xpoint,ypoint,rad, BLUE);
			DrawCircle(xpoint,ypoint,rad-5, LIGHTGRAY);
			if (CheckCollisionPointCircle(mousepos, {xpoint,ypoint },rad))
			{
				DrawCircle(xpoint,ypoint,rad-5, {220,220,220,255});
				if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					grid = gridnetwork[x + y * dims];
					textboxes = textboxesnetwork[x + y * dims];
					textsizes = textsizesnetwork[x + y * dims];
					chosengrid = x + y * dims;
					ScreenMode = View;
				}
			}

			//GRIDS
			Color blockcolor = WHITE;
			for (int j = 0; j < mapdims;j++)
			{
				for (int i = 0; i <mapdims;i++)
				{
					
					switch (pooledgridnetwork[x+y*dims][j][i])
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
					DrawRectangle(xpoint-rad/2+i*rad/mapdims,ypoint-rad/2+j*rad/mapdims,rad/mapdims,rad/mapdims,blockcolor);
				}
			}
		}
	}


}

void DrawWorkers(float linewidth,float lineheight)
{
	for (int x = 0; x < workers[chosengrid].size(); x++)
	{
		Vector2 workerpos = { (float)(workers[chosengrid][x].pos.first * linewidth + linewidth / 2),(float)(workers[chosengrid][x].pos.second * lineheight + lineheight / 2) };
		DrawCircleV(workerpos, min(linewidth / 2, lineheight / 2) - 2, { 100,100,255,255 });
		DrawCircleLinesV(workerpos, min(linewidth / 2, lineheight / 2) - 2, BLACK);
		DrawTextEx(codingfontbold, TextFormat("%s(%d)", workers[chosengrid][x].name,workers[chosengrid][x].gender), {workerpos.x + linewidth / 3,workerpos.y - lineheight * 0.5f - 10.0f}, 18, 3, BLACK);

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
	static Rectangle outerrect = { windowwidth - sidebarwidth - moneybarwidth+35, 50,50,700 };
	static Rectangle boundary = { windowwidth - sidebarwidth - moneybarwidth, 0, moneybarwidth, windowheight };
	float fraction = totalmoney/ (float)quota;
	Rectangle innerrect = {outerrect.x+10,outerrect.y+(outerrect.height*(1-fraction))+10,outerrect.width -20 ,outerrect.height*fraction-20 };
	Rectangle shinerect = { outerrect.x +15,outerrect.y + (outerrect.height * (1 - fraction)) + 20,innerrect.width/10 ,outerrect.height * fraction - 40 };

	DrawRectangleRec(boundary, DARKGRAY);

	DrawRectangleRounded(outerrect,10,10,BLACK);
	DrawRectangleRounded(innerrect, 10, 10, GREEN);
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
	//Draw Cells
	Color color=WHITE;
	for (int y = 0; y < gridheight; y++)
	{
		for (int x = 0; x < gridwidth; x++)
		{
			if ((x >= 0&&x<screenbuffer) || (x >= gridwidth-screenbuffer&&x<gridwidth) || (y >= 0 && y < screenbuffer) || (y >= gridheight -screenbuffer&&y<gridheight))
				color = BROWN;
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
			case -2:
				color = workbenchcolor;
				break;
			}

			if (((x >= 0 && x < screenbuffer)|| (x >= gridwidth - screenbuffer && x < gridwidth)) && y > (int)(gridheight * 0.4) && y < (int)(gridheight * 0.6))
				color = {0,205,255,255};

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

	DrawWorkers(linewidth,lineheight);

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
						int* numarray = (int*)malloc(sizeof(int) * 6);

						for (int q = 0; q < grid.size() / mapdims; q++)
						{
							for (int r = 0; r < grid[0].size() / mapdims; r++)
							{
								if(grid[l + q][m + r]>-2)
								numarray[grid[l + q][m + r]+1]++;
							}
						}

						int min = 0;
						for (int q = 0; q < 6; q++)
						{
							if (numarray[q] > numarray[min])
								min = q;
						}
						row.push_back(min-1);
						free(numarray);
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

void DrawCodeTab()
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
	if (GuiButton({ windowwidth - sidebarwidth,windowheight - textinputheight - textsavebuttonheight,sidebarwidth,textsavebuttonheight }, "SAVE"))
	{
		workers[chosengrid][chosenperson].code = textboxes;
		workers[chosengrid][chosenperson].linesize = textsizes;
		workers[chosengrid][chosenperson].getCode();
	}

	//DRAWING NAME BANNER
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight, sidebarwidth, namebannersize, DARKGRAY);
	DrawTextEx(codingfontbold, workers[chosengrid][chosenperson].name, { windowwidth - 0.6 * sidebarwidth,sidebarbuttonheight + 10 }, 20, 5, WHITE);
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
		DrawCircle(button.x + button.width * 0.1, button.y + button.height * 0.5, button.height * 0.32, WHITE);
		DrawCircle(button.x + button.width * 0.1 , button.y + button.height*0.5, button.height*0.3, get<0>(areaitems[x]));
		DrawTextEx(codingfontbold, get<1>(areaitems[x]), { button.x + button.width * 0.2f ,button.y+button.height*0.4f},25,2,WHITE);
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
								if (x > startpos.first && y > startpos.second)
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
					DrawRectangleRec(block, workbenchcolor);
					if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
					{
						for (int i = y - workbenchsize / 2; i < y + workbenchsize / 2; i++)
						{
							for (int j = x - workbenchsize / 2; j < x + workbenchsize / 2; j++)
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
						workers[chosengrid].push_back(Worker(0, screenbuffer, gridheight / 2));
						workers[chosengrid][workers[chosengrid].size() - 1].pathfind({x + workbenchsize / 2 + 1,y + workbenchsize / 2 + 1});
					}
				}
			}
		}
	}

}

void DrawSidebar()
{
	//Drawing 3 Sidebar Buttons
	DrawRectangle(windowwidth - sidebarwidth, 0, sidebarwidth, windowheight, DARKGRAY);
	if (GuiButton({ windowwidth - sidebarwidth,0,sidebarwidth / 3,sidebarbuttonheight }, "Code"))
		SidebarState = Code;
	else if (GuiButton({ windowwidth - 2 * sidebarwidth / 3,0,sidebarwidth/3,sidebarbuttonheight }, "Shop"))
		SidebarState = Shop;
	else if (GuiButton({ windowwidth - sidebarwidth / 3,0,sidebarwidth / 3,sidebarbuttonheight }, "Hire"))
		SidebarState = Hire;

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
	InitializeGrid(50,50,0);
	InitializeShop();
	InitializeHire();

	//Add random workers
	//for (int x = 0; x < 5; x++)
	//{
		//workers.push_back(Worker((rand() * (grid[0].size() - 2*screenbuffer)) / RAND_MAX + screenbuffer, (rand() * (grid.size() - 2*screenbuffer) )/ RAND_MAX + screenbuffer));
		//workers[x].pathfind({ (rand() * (grid[0].size()-2*screenbuffer)) / RAND_MAX + screenbuffer ,(rand() * (grid.size()-2*screenbuffer)) / RAND_MAX + screenbuffer });
	//}

	ScreenMode = Map;
	//SET GAMETIME
	gametime = time(NULL);

	while (!WindowShouldClose())
	{
		mousepos = GetMousePosition();
		frametime = GetFrameTime();

		if (time(NULL)!=gametime)
		{
			gametime = time(NULL);

			//Tick Functions
			ChangeWorkerPositions();

		}

		BeginDrawing();
		ClearBackground(WHITE);

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
		}
		DrawText(TextFormat("%d", GetFPS()), 10, 10, 25, WHITE);

		EndDrawing();
	}

	
}