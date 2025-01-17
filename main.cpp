#include <raylib.h>
#include "Raygui.h"

#include "gameConst.h"

Font codingfont;
Font codingfontbold;

enum sidebarstate {
	Code,
	Shop,
	Hire
} SidebarState;

std::vector<char*> textboxes;

Vector2 mousepos;


void DrawCodeTab()
{
	static bool editing = false;
	static char text[textinputsize] = "";
	static short int startpos = 0;
	static short int chosenblock = 0;
	static short int chosentext = 0;
	static double codeblockheight = (windowheight - sidebarbuttonheight - textinputheight) / (double)codeblocks;

	
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
		Rectangle codeblock = { windowwidth - sidebarwidth, sidebarbuttonheight + i * codeblockheight, sidebarwidth, codeblockheight };
		DrawRectangleRec(codeblock,blockcolor);
		DrawRectangle(codeblock.x, codeblock.y, 65, codeblockheight, { 255, 71, 26,255});
		DrawRectangleLinesEx(codeblock, 2, DARKBLUE);
		if (textboxes.size() > startpos + i)
		{
			DrawTextEx(codingfontbold, textboxes[startpos + i], { codeblock.x + sidebarwidth / 7, (float)(codeblock.y + codeblockheight*0.4) }, textinputfontsize,3, WHITE);
		}
		DrawTextEx(codingfontbold, TextFormat("%d", (startpos + i)), { (float)codeblock.x + 10, (float)(codeblock.y + codeblockheight / 3) }, textinputfontsize,3, WHITE);

		if (CheckCollisionPointRec(mousepos, codeblock) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			chosenblock = i;
			chosentext = i + startpos;
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
	}
}


int main()
{
	InitWindow(windowwidth, windowheight, "ag.AI.n");
	SetTargetFPS(144);
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
	GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 1);

	codingfont = LoadFont("./fonts/dina10px.ttf");
	codingfontbold = LoadFont("./fonts/dina10pxbold.ttf");
	GuiSetFont(codingfont);

	while (!WindowShouldClose())
	{
		mousepos = GetMousePosition();
		BeginDrawing();
		ClearBackground(WHITE);

		DrawSidebar();


		EndDrawing();
	}
}