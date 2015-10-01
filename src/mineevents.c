#include "main.h"

//standard SDL Timer function, does not do more than pushing SDL_TICK event every ms time
Uint32 timer(Uint32 ms, void *param)
{
    SDL_Event ev;
    ev.type = SDL_TICK;
    SDL_PushEvent(&ev);
    return ms;
}

void SDL_MyDelay(Uint32 ms)
{
    SDL_Event ev;
    while( SDL_PollEvent(&ev) )
    {
        if(ev.type == SDL_QUIT)
        {
            SDL_Quit();
            exit(0);
        }
    }
    //SDL_PumpEvents();
    SDL_Delay(ms);
}

/* GetClickedCell(...) function ****************************************************************************

Description:    Converts coordinates of clicked px to coords of clicked Cell
Parameters:         Uint16          x,          (the x coordinate of clicked px)
                    Uint16          y,          (the y coordinate of clicked px)
                    Point           SP          (the starting point of cells)
Returns:        coordinates of clicked cell, (-1; -1) if not any cell clicked (2ND MEANING OF POINT)
********************************************************************************************************* */
Point GetClickedCell(Uint16 x, Uint16 y, Point SP)
{
    return (x < SP.x || y < SP.y) ? (Point){-1, -1} : (Point) {(x-SP.x) / CELL_SIZE, (y-SP.y) / CELL_SIZE};
}

/* GetClickedButton(...) function **************************************************************************

Description:    Converts coordinates of clicked px to index of clicked button in Buttons array
Parameters:         Uint16          x,          (the x coordinate of clicked px)
                    Uint16          y,          (the y coordinate of clicked px)
                    Button          *Buttons,   (the buttons' array)
                    unsigned        No          (the number of buttons)
Returns:        coordinates of clicked cell, (-1; -1) if not any cell clicked (2ND MEANING OF POINT)
********************************************************************************************************* */
int GetClickedButton(Uint16 x, Uint16 y, Button *Buttons, unsigned No)
{
    int i;
    for(i=0; i < No; i++)
        if( x > Buttons[i].SP.x && x < Buttons[i].SP.x + Buttons[i].Dim.x && y > Buttons[i].SP.y && y < Buttons[i].SP.y + Buttons[i].Dim.y )
            return i;
    return -1;
}

/* NextState(...) function *********************************************************************************

Description:    Sets the next state of a cell, called at Right Click
Parameters:         State           *st,        (the state to be changed)
                    int             *leftmines, (the number of unflagged mines, changes here)
                    bool            q           (whether ? marks are on or off)
Returns:        void
********************************************************************************************************* */
void NextState(State * st, int * leftmines, bool q)
{
    switch(*st)
    {
        case empty: *st = flag;  *leftmines -= 1; break;
        case flag:  *st = q ? qmark : empty; *leftmines += 1; break;
        case qmark: *st = empty; break;
    }
}

/* IsInDim(...) function ***********************************************************************************

Description:    Returns if P point is in dimensions
Parameters:         Point           P,          (the point to check)
                    Size            dimensions  (the dimensions)
Returns:        boolean value if P point is in dimensions
********************************************************************************************************* */
bool IsInDim(Point P, Size dimensions)
{
     return P.x >= 0 && P.x < dimensions.x && P.y >= 0 && P.y < dimensions.y ? true : false;
}

/* Uncover(...) function (THIS IS A KEY FUNCTION OF THE GAME, IT IS WORTH READING) *************************

Description:    Uncovers empty area around an empty cell
Parameters:         unsigned        x,          (Dimensions.x)
                    unsigned        y,          (Dimensions.y)
                    Cell            **Cells     (Cells 2D array)
                    Point           C           (coordinates of clicked cell) (2ND MEANING OF POINT)
                    SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *numbers    (source image)
                    Point           *SP         (starting point of cells)
                    unsigned        *uncovered  (number of uncovered cells, to change)
                    int             *leftmines  (number of mines to flag, to change)
                    bool            bnw         (drawn in black and white, or not)
Returns:       void && uncovered, leftmines, Cells as parameters
********************************************************************************************************* */
void Uncover(unsigned x, unsigned y, Cell **Cells, Point C, SDL_Surface *screen, SDL_Surface *numbers, Point *SP, unsigned *uncovered, int *leftmines, bool bnw)
{
    int k, l;

    //processing clicked cell
    DrawNumber(screen, numbers, C, *SP, Cells[C.x][C.y].preset, bnw);
    Cells[C.x][C.y].clickable = false;
    *uncovered -= 1;

    int kmax = Cap(C.x+1, 0, x-1);
    int lmax = Cap(C.y+1, 0, y-1);

    //looping through cells around
    for(k = Cap(C.x-1, 0, x-1); k <= kmax ; k++)
        for(l = Cap(C.y-1, 0, y-1); l <= lmax; l++)
            if(Cells[k][l].clickable)
            {
                //processing current cell
                Cells[k][l].clickable = false;

                if(SLOW_MO_TOGGLE)
                {
                    SDL_Flip(screen);
                    SDL_MyDelay(SLOW_MO_MS);
                }
                DrawNumber(screen, numbers, (Point){k,l}, *SP, Cells[k][l].preset, bnw);

                /** IMPORTANT NOTE
                At this point, my game differs from Windows XP Minesweeper. In the original version if an empty cell
                was flagged, uncovering algorythm didn't take effect on it, so it was just left flagged in the middle
                of empty area. I felt this would cause obvious and unnecessary clicks, so I modified this part.
                */
                if(Cells[k][l].state == flag)   //counting mines to flag
                    *leftmines += 1;

                if(Cells[k][l].preset == 0)
                    Uncover(x, y, Cells, (Point){k,l}, screen, numbers, SP, uncovered, leftmines, bnw);
                else
                    *uncovered -= 1;
                /*
                You can find the recursion here: if a cell nearby is empty as well, it calls Uncover function again.
                Note that clickable flag provides the condition to stop recursion set in the beginning. For non-zero
                cells that flag provides efficiency, too: if a cell is drawn, its flag is set to false, so it won't
                be drawn again.
                */
            }
}

/* PushCells(...) function (THIS IS A KEY FUNCTION OF THE GAME, IT IS WORTH READING) ***********************

Description:    Pushes click event on all nearby cells, called by Middle Mouse Button
Parameters:         bool            cover       (boolean value if nearby cells should be uncovered)
                    BtnState        st,         (this func is for pressing && releasing, this value sets it)
                    unsigned        x,          (Dimensions.x)
                    unsigned        y,          (Dimensions.y)
                    Cell            **Cells     (Cells 2D array)
                    Point           C           (coordinates of clicked cell) (2ND MEANING OF POINT)
                    SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *cells      (source image)
                    SDL_Surface     *numbers    (source image)
                    Point           *SP         (starting point of cells)
                    unsigned        *uncovered  (number of uncovered cells, to change)
                    int             *leftmines  (number of mines to flag, to change)
                    bool            bnw         (drawn in black and white, or not)
Returns:       void && uncovered, leftmines, Cells as parameters
********************************************************************************************************* */
void PushCells(bool cover, BtnState st, unsigned x, unsigned y, Cell **Cells, Point C, SDL_Surface *screen, SDL_Surface *cells, SDL_Surface *numbers, Point *SP, unsigned *uncovered, int *leftmines, bool bnw)
{
    int i, k, l;
    int kmax = Cap(C.x+1, 0, x-1);
    int lmax = Cap(C.y+1, 0, y-1);
    int markedaround = 0;
    int lost = 0;

    //looping through nearby cells, and drawing them
    for(k = Cap(C.x-1, 0, x-1); k <= kmax ; k++)
        for(l = Cap(C.y-1, 0, y-1); l <= lmax; l++)
        {
            if(Cells[k][l].clickable)
            {
                if(Cells[k][l].state == flag)       //we need to count if we can really uncover cells nearby
                    markedaround++;
                else
                {
                    if(st == pressed)               //we need to press now
                        DrawCell(screen, cells, (Point){k,l}, *SP, Cells[k][l].state+1, bnw);
                    else if(st == normal)           //we need to release now
                        DrawCell(screen, cells, (Point){k,l}, *SP, Cells[k][l].state, bnw);
                }
            }
        }

    //we need 2 similar loops, before first we need to count markaround to check if we can uncover them
    if( !Cells[C.x][C.y].clickable && markedaround == Cells[C.x][C.y].preset && st == normal && cover)
    {
        //looping through nearby cells again
        for(i=0, k = Cap(C.x-1, 0, x-1); k <= kmax ; k++)
            for(l = Cap(C.y-1, 0, y-1); l <= lmax; l++, i++)
                if(Cells[k][l].clickable && Cells[k][l].state != flag)
                {
                    switch(Cells[k][l].preset)
                    {
                        case -1:
                            lost += 1 << i;
                            break;

                        case 0:
                            Uncover(x, y, Cells, (Point){k,l}, screen, numbers, SP, uncovered, leftmines, bnw);
                            Cells[k][l].clickable = false;
                            break;

                        default:
                            DrawNumber(screen, numbers, (Point){k,l}, *SP, Cells[k][l].preset, bnw);
                            *uncovered -= 1;
                            Cells[k][l].clickable = false;
                            break;
                    }
                }
        if(lost)
        {   //we need to push LOST event if marking was not fine
            SDL_Event ev;
            ev.type = SDL_LOST;
            ev.button.x = lost;
            SDL_PushEvent(&ev);
        }
    }
}

/* GenerateMines(...) function (THIS IS A KEY FUNCTION OF THE GAME, IT IS WORTH READING)********************

Description:    Generates mines to Cells array
Parameters:         Cell            **Cells,    (the array to generate to)
                    Config          cfg,        (the config that contains conditions of generating)
                    Point           C,          (the clicked cell) (2ND MEANING OF POINT)
Returns:        void && Cells array as parameter
********************************************************************************************************* */
void GenerateMines(Cell** Cells, Config cfg, Point C)
{
    /** IMPORTANT NOTE
    This program differs fron original minesweeper at this point. Originally mines were generated at new
    game event, but I generate it at the first click. You can check it by xyzzy cheat. I didn't feel this
    was so important, and this version was easier to code for me.
    */
    int i, j, k, l;
    int genmines=0;

    //Creating a single array containing all possible coordinates to generate
    Point *Coords;
    Coords = (Point*) malloc( cfg.Dim.x * cfg.Dim.y * sizeof(Point));
    debugmalloc(Coords, "Coords");
    for(i=0; i < cfg.Dim.x; i++)
        for(j=0; j < cfg.Dim.y; j++)
            Coords[i + cfg.Dim.x * j] = (Point) {i, j};

    //configuring the mode of generating, read after actual generating
    switch(cfg.mm)
    {
        case lucker:
            cfg.Mines++;
            // fallthrough
        case badass:
            Cells[C.x][C.y].preset = -1;
            Coords[C.x + cfg.Dim.x * C.y] = Coords[cfg.Dim.x * cfg.Dim.y - 1];
            genmines++;
        default: break;
    }

    //Generating a random point of new array, then adding a mine to generated coordinates
    //and removing generated coordinates from array, in a fast and tricky way
    while(genmines < cfg.Mines)
    {
        k = rand() % (cfg.Dim.x * cfg.Dim.y - genmines);
        Cells[Coords[k].x][Coords[k].y].preset = -1;
        genmines++;
        Coords[k] = Coords[cfg.Dim.x * cfg.Dim.y - genmines];
    }
    //still part of the mode configuring
    if(cfg.mm == lucker)
        Cells[C.x][C.y].preset = 0;

    free(Coords);

/* old generating, not efficient at many mines

    switch(cfg.mm)
    {
        case badass:
            genmines++;
            // fallthrough
        case lucker:
            Cells[C.x][C.y].preset = -1;
        default: break;
    }

    while(genmines < cfg.Mines)
    {
        i = rand() % cfg.Dim.x;
        j = rand() % cfg.Dim.y;

        if(Cells[i][j].preset != -1)
        {
            Cells[i][j].preset = -1;
            genmines++;
        }
    }

    if(cfg.mm == lucker)
        Cells[C.x][C.y].preset = 0;
*/

    int kmax, lmax;

    //counting numbers, that mean number of mines around that cell
    //looping through all cells
    for(i=0; i < cfg.Dim.x; i++)
        for(j=0; j < cfg.Dim.y; j++)
        {
            if(Cells[i][j].preset != -1)
            {
                kmax = Cap(i+1, 0, cfg.Dim.x-1);
                lmax = Cap(j+1, 0, cfg.Dim.y-1);
                Cells[i][j].preset = 0;
                //if we found a mine, add 1 to all nearby cells' preset
                for(k = Cap(i-1, 0, cfg.Dim.x-1); k <= kmax; k++)
                    for(l = Cap(j-1, 0, cfg.Dim.y-1); l <= lmax; l++)
                        if(Cells[k][l].preset == -1)
                            Cells[i][j].preset++;
            }
        }
}

