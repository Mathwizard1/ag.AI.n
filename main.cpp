#include <iostream>
#include <vector>
#include <raylib.h>
#include "Raygui.h"

#include "gameConst.h"


enum sidebarstate {
	Code,
	Shop,
	Hire
} SidebarState;

std::vector<char*> textboxes;

void DrawCodeTab()
{
	static char text[textinputsize] = "";
	static short int startpos = 0;
	static short int chosenblock = 0;
	static short int chosentext = 0;
	static float codeblockheight = (windowheight - sidebarbuttonheight - textinputheight) / codeblocks;


	//CODE SUBMISSION
	if (GuiTextBox({ windowwidth - sidebarwidth,windowheight - textinputheight,sidebarwidth,textinputheight }, text, textinputsize, true))
	{
		char* sendtext=(char*)malloc(sizeof(char)*textinputsize);

		//Copy text to sendtext buffer
		for (int x = 0; x < textinputsize; x++)
		{
			sendtext[x] = text[x];
			text[x] = NULL;
		}

		//Choose to edit text or enter new text
		if (chosentext < textboxes.size())
			textboxes[chosentext] = sendtext;
		else
		{
			for (int x = textboxes.size(); x <=chosentext; x++)
			{
				char* temptext = (char*)malloc(sizeof(char) * textinputsize);
				temptext[0]='\0';
				textboxes.push_back(temptext);
			}
			textboxes[chosentext] = sendtext;
		}

		//Iterate Codeblocks
		if (chosenblock < codeblocks - 1)
		{
			chosentext++;
			chosenblock++;
		}
		else if (textboxes.size() + 1 > codeblocks)
		{
			chosentext++;
			startpos++;
		}
	}

	//CHANGING CURRENT BLOCK
	if (IsKeyPressed(KEY_UP) || (GetMouseWheelMove() > 0))
	{
		if (chosenblock == 0 && startpos > 0)
		{
			startpos -= 1;
			chosentext--;
		}
		else if (chosenblock > 0)
		{
			chosentext--;
			chosenblock--;
		}
	}
	else if (IsKeyPressed(KEY_DOWN) || (GetMouseWheelMove() < 0))
	{
		if (chosenblock == codeblocks - 1)
		{
			chosentext++;
			startpos += 1;
		}
		else
		{
			chosentext++;
			chosenblock++;
		}
	}

	//DRAWING CODEBLOCKS
	for (int i = 0; i < codeblocks; i++)
	{
		Color blockcolor = (i == chosenblock) ? GREEN : Color{0,150,255,255};
		Rectangle codeblock = { windowwidth - sidebarwidth, sidebarbuttonheight + i * codeblockheight, sidebarwidth, codeblockheight };
		DrawRectangleRec(codeblock,blockcolor);
		DrawRectangleLinesEx(codeblock, 2, DARKBLUE);
		if (textboxes.size() > startpos + i)
		{
			DrawText(textboxes[startpos + i], codeblock.x + sidebarwidth/10, codeblock.y + 5, textinputfontsize, BLACK);
		}
		DrawText(TextFormat("%d",(startpos + i)),codeblock.x+10, codeblock.y+codeblockheight/3, textinputfontsize / 2, WHITE);
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
	}
}


int main()
{
	InitWindow(windowwidth, windowheight, "ag.AI.n");
	SetTargetFPS(144);
	SetConfigFlags(FLAG_MSAA_4X_HINT);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(WHITE);

		DrawSidebar();


		EndDrawing();
	}
}