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

void InitialiseCodeLine(int i)
{
	textboxes.push_back((char*)malloc(sizeof(char) * 100));
}

void DrawCodeTab()
{
	static char text[10000] = "";


	if (GuiTextBox({ windowwidth - sidebarwidth,sidebarbuttonheight,sidebarwidth,windowheight - sidebarbuttonheight }, text, 100, true))
		for (int x = 0; x < 10000; x++)
			text[x] = NULL;
}

void DrawSidebar()
{
	//Drawing 3 Sidebar Buttons
	DrawRectangle(windowwidth - sidebarwidth, 0, sidebarwidth, windowheight, DARKGRAY);
	if (GuiButton({ windowwidth - sidebarwidth,0,133,sidebarbuttonheight }, "Code"))
		SidebarState = Code;
	else if (GuiButton({ windowwidth - 2 * sidebarwidth / 3,0,133,sidebarbuttonheight }, "Shop"))
		SidebarState = Shop;
	else if (GuiButton({ windowwidth - sidebarwidth / 3,0,133,sidebarbuttonheight }, "Hire"))
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

	InitialiseCodeLine(0);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(WHITE);

		DrawSidebar();


		EndDrawing();
	}
}