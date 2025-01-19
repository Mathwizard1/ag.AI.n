#include <raylib.h>
#include "Raygui.h"

#include "worker.h"
#include "gameConst.h"
#include "inventory.h"

using namespace std;

short int chosenperson = 0;
long long int gametime;

Font codingfont;
Font codingfontbold;

enum sidebarstate {
	Code,
	Shop,
	Hire
} SidebarState;

std::vector<char*> textboxes;
std::vector<short int> textsizes;
std::vector<std::vector<short int>> grid;
std::vector<Worker> workers;
std::vector<Worker> bosses;
std::vector<Worker> receptionists;

Vector2 mousepos;


/*
Colors:
1->Lunch Area (Yellow)
2->Work Area (Green)
3->Reception (Blue)
4->Boss (Red)

*/
void ChangeWorkerPositions()
{
	for (int x = 0; x < workers.size(); x++)
	{
		if (workers[x].path.size() > 0)
		{
			workers[x].pos = workers[x].path[workers[x].path.size() - 1];
			workers[x].path.pop_back();
		}
	}
}

void InitializeGrid(short int width, short int height)
{
	for (int y = 0; y < height; y++)
	{
		std::vector<short int> row;
		for (int x = 0; x < width; x++)
		{
			if (x >0.05 * width && x < 0.5 * width)
			{
				if (y < 0.2 * height)
				{
					row.push_back(1);
					continue;
				}
				if (y > 0.5 * height)
				{
					row.push_back(2);
					continue;
				}
			}

			else if (x > 0.6 * width && x < 0.9 * width)
			{
				if (y < 0.2 * height)
				{
					row.push_back(3);
					continue;
				}

				if (y > 0.7 * height)
				{
					row.push_back(4);
					continue;
				}
			}

			
			row.push_back(0);
		}
		grid.push_back(row);
	}
}

void DrawWorkers(float linewidth,float lineheight)
{
	for (int x = 0; x < workers.size(); x++)
	{
		Vector2 workerpos = { (float)(workers[x].pos.first * linewidth + linewidth / 2),(float)(workers[x].pos.second * lineheight + lineheight / 2) };
		DrawCircleV(workerpos, min(linewidth / 2, lineheight / 2) - 2, { 100,100,255,255 });
		DrawCircleLinesV(workerpos, min(linewidth / 2, lineheight / 2) - 2, BLACK);
		DrawTextEx(codingfontbold, TextFormat("%s(%d)", workers[x].name,workers[x].gender), {workerpos.x + linewidth / 3,workerpos.y - lineheight * 0.5f - 10.0f}, 18, 3, BLACK);

		if (CheckCollisionPointCircle(mousepos, workerpos, min(linewidth, lineheight)))
		{
			DrawCircleV(workerpos, min(linewidth / 2, lineheight / 2) - 2, { 200,100,255,255 });
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			chosenperson = x;
		}
	}
}

void DrawMainScreen()
{
	static short int gridheight = grid.size()+2*screenbuffer;
	static short int gridwidth = grid[0].size()+2*screenbuffer;
	static float linewidth = (windowwidth - sidebarwidth) / (float)gridwidth;
	static float lineheight = (windowheight) / (float)gridheight;

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
		DrawLineEx({ 0,y * lineheight }, { windowwidth - sidebarwidth,y * lineheight }, 1, BLACK);
	}

	DrawWorkers(linewidth,lineheight);

}

void DrawCodeTab()
{
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
		textboxes = workers[chosenperson].code;
		textsizes = workers[chosenperson].linesize;
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
		workers[chosenperson].code = textboxes;
		workers[chosenperson].linesize = textsizes;
	}

	//DRAWING NAME BANNER
	DrawRectangle(windowwidth - sidebarwidth, sidebarbuttonheight, sidebarwidth, namebannersize, DARKGRAY);
	DrawTextEx(codingfontbold, workers[chosenperson].name, { windowwidth - 0.6 * sidebarwidth,sidebarbuttonheight + 10 }, 20, 5, WHITE);
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
	}
}

int main()
{
	InitWindow(windowwidth, windowheight, "ag.AI.n");
	SetTargetFPS(144);

	srand(42);

	//Flags
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 1);

	//Set Font
	codingfont = LoadFont("./fonts/dina10px.ttf");
	codingfontbold = LoadFont("./fonts/dina10pxbold.ttf");
	GuiSetFont(codingfont);
	
	//Initialize Grid
	InitializeGrid(60,60);

	//Add random workers
	for (int x = 0; x < 2; x++)
	{
		workers.push_back(Worker((rand() * (grid[0].size() - 2*screenbuffer)) / RAND_MAX + screenbuffer, (rand() * (grid.size() - 2*screenbuffer) )/ RAND_MAX + screenbuffer));
		workers[x].pathfind({ (rand() * (grid[0].size()-2*screenbuffer)) / RAND_MAX + screenbuffer ,(rand() * (grid.size()-2*screenbuffer)) / RAND_MAX + screenbuffer });
	}

	//SET GAMETIME
	gametime = time(NULL);

	while (!WindowShouldClose())
	{
		mousepos = GetMousePosition();
		if (time(NULL)!=gametime)
		{
			gametime = time(NULL);
			ChangeWorkerPositions();
		}


		BeginDrawing();
		ClearBackground(WHITE);


		DrawMainScreen();
		DrawSidebar();
		DrawText(TextFormat("%d", GetFPS()), 10, 10, 25, WHITE);

		EndDrawing();
	}

}