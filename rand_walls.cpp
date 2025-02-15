#include "rand_walls.h"

std::vector<std::vector<std::pair<short, short>>> gridPaths;

MazeGenerator::orientation MazeGenerator::chooseOrientation(int width, int height)
{
    if (width < height)
    {
        return orientation::Horizontal;
    }
    else if (height < width)
    {
        return orientation::Vertical;
    }
    
    return ((rand() % 2) ? orientation::Vertical : orientation::Horizontal);
}

MazeGenerator::MazeGenerator(short int fullWidth, short int fullHeight, short int fraq)
{
    this->fullWidth = fullWidth;
    this->fullHeight = fullHeight;
    this->fraq = fraq;
}

MazeGenerator::orientation MazeGenerator::randOrient()
{
    return chooseOrientation(1, 1);
}

void MazeGenerator::generateMaze(std::vector<std::vector<short int>> &maze, orientation wallOrientation, short int maxDepth, short int width, short int height, short int x, short int y, short int depth)
{
    if (depth == 0)
    {
        maze.clear();
        gridPath.clear();
        maze.resize(width, std::vector<short int>(height, 0));
    }
    else
    {
        if (width <= fullWidth / fraq || height <= fullHeight / fraq) { return; }
    }

    if (depth < maxDepth)
    {
        short int wx, wy, nx, ny, w, h;
        short int dx, dy, rx, ry;

        // the line filler
        dx = (wallOrientation == orientation::Horizontal) ? 1 : 0;
        dy = (wallOrientation == orientation::Horizontal) ? 0 : 1;

        // draw walls at wx,wy
        rx = (width - 1) / 2 + (1 + -2 * (rand() % 2)) * ( rand() % ((width - 1) / 4 + 1));
        ry = (height - 1) / 2 + (1 + -2 * (rand() % 2)) * ( rand() % ((height - 1) / 4 + 1));

        wx = x + ((wallOrientation == orientation::Horizontal) ? 0 : rx);
        wy = y + ((wallOrientation == orientation::Horizontal) ? ry : 0);

        // where will the passage through the wall
        short int px, py;
        px = wx + ((wallOrientation == orientation::Horizontal) ? 0 : 0);   // needs changes
        py = wy + ((wallOrientation == orientation::Horizontal) ? 0 : 0);

        int entrSize = 1;

        short int length = (wallOrientation == orientation::Horizontal) ? width : height;
        while (length)
        {
            if ((wx <= (px - entrSize) || wx >= (px + entrSize)) && (wallOrientation == orientation::Horizontal))
            {
                maze[wx][wy] = -1;
            }
            else if ((wy <= (py - entrSize) || wy >= (py + entrSize)) && (wallOrientation == orientation::Vertical))
            {
                maze[wx][wy] = -1;
            }
            else
            {
                maze[wx][wy] = 1;
            }
            wx += dx;
            wy += dy;
            length--;
        }

        nx = x;
        ny = y;

        w = (wallOrientation == orientation::Horizontal) ? width : wx - x + 1;
        h = (wallOrientation == orientation::Horizontal) ? wy - y + 1 : height;
        generateMaze(maze, chooseOrientation(w, h), maxDepth, w, h, nx, ny, depth + 1);

        nx = (wallOrientation == orientation::Horizontal) ? x : wx + 1;
        ny = (wallOrientation == orientation::Horizontal) ? wy + 1 : y;

        w = (wallOrientation == orientation::Horizontal) ? width : x + width - wx - 1;
        h = (wallOrientation == orientation::Horizontal) ? y + height - wy - 1 : height;
        generateMaze(maze, chooseOrientation(w, h), maxDepth, w, h, nx, ny, depth + 1);
    }
    else
    {
        gridPaths.push_back(gridPath);
        return;
    }
}

