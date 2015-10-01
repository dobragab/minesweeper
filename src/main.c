#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

/*
átvett modulok: debugmalloc, dictionary, iniparser együtt 2116 sor
2014. 11. 30. minimális kommentekkel, csak a saját kód: (átvett részek nélkül) 1560+250 sor
2014. 12. 04. saját modulok: 2260 sor
*/

/*
main.c actually contains the game part, nothing else
The reason of that segmentation fault can be found in main.c at line 126.
*/
int main(int argc, char *argv[])
{
    #ifdef MYDEBUG
    debugmalloc_naplofajl("dump.txt");   //debugging, not included in release
    #endif

    srand(time(NULL));

    SDL_Event event;                        //the main event i will use at event handling
    SDL_Surface *screen;                    //the visible screen surface

    BestTime times[3];                      //will contain the best times and names
    Config cfg = {{9, 9}, 10, true, false, false, beginner, lucker};    //default configuration

    bool goodcfg = LoadConfig(&cfg);        //loads config
    bool changedcfg = false;
    if(goodcfg)
        changedcfg = TrimConfig(&cfg);      //checks if config is valid, and changes if it isnt

    bool goodtimes = GetBestTimes(times);   //does same for best times
    bool changedtimes = false;
    if(goodtimes)
        changedtimes = TrimTimes(times);

    if(changedcfg || !goodcfg || changedtimes || !goodtimes)
        SaveConfig(&cfg, times);            //if config is changed, it saves it to config file

    Size Dim = {cfg.Dim.x, cfg.Dim.y};
    int MinesNo = cfg.Mines;

    unsigned uncovered;                     //contains number of uncovered non-mine cells, won if its 0
    int leftmines;                          //contains the value of the left counter, MinesNo minus flagged cels

//deklaraciok
    Size WindowSize, DIGITBDIM;             //important points of the game screen - B for Border, SP for Starting Point
    Point CellsSP, MINESSP, MINESBSP, TIMESP, TIMEBSP, SMILEYSP;
    SDL_Surface *cells, *digits, *numbers, *smileys;    //main images
        debugmalloc(cells = IMG_Load("cells.png"), "cells.png");
        debugmalloc(digits = IMG_Load("digits.png"), "digits.png");
        debugmalloc(numbers = IMG_Load("numbers.png"), "numbers.png");
        debugmalloc(smileys = IMG_Load("smileys.png"), "smileys.png");
    SDL_TimerID stw = stw;                  //stopwatch, set to NULL at first to avoid warning, not important
    unsigned sec = 0;                       //value of time in seconds

//inicializalas
    screen=NULL;
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

    Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,4096);
    Mix_Music *tick, *won, *lost;           //sounds used in game
    debugmalloc(tick = Mix_LoadMUS("tick.mp3"), "tick.mp3");
    debugmalloc(won = Mix_LoadMUS("won.mp3"), "won.mp3");
    debugmalloc(lost = Mix_LoadMUS("lost.mp3"), "lost.mp3");

    //Declarations of most local variables used in main module
    Cell ** Cells = NULL;       //the Cells table, 2D dinamic array
    bool quit = false;          //indicates exit from game
    bool mouseldown = false;    //contains if mouse buttons are pressed
    bool mousemdown = false;
    bool mouserdown = false;
    bool simpleclick = false;   //true if one mouse btn is pressed, and wasnt released since 2 btns
    bool screenmodified = false;//true if screen needs to be flipped
    bool blocked = false;       //game is blocked only if it is won or lost, you cant click on cells then
    Button Buttons[3];          //contains data of buttons
    unsigned ButtonsNo = 3;     //was easier than end sign inside Buttons
    int btn = -1, btn2 = -1;    //contains index of pressed button of Buttons[] array, 2 for safe buttons
    Point C, CNext;             //contains coords of pressed cells
    bool started = false;       //true if game is already started, but not finished yet. necessary for starting stw
    bool cheat = false;         //true if cheat mode activated
    char word[6] = {0};         //contains 4 last pressed keys

    SDL_EnableUNICODE(1);       //necessary, because event.key.keysym.sym confuses Y and Z

    SetCustomCursor();

    SDL_Event ev;               //pushes INIT event
    ev.type = SDL_INIT;
    SDL_PushEvent(&ev);


    while (!quit)
    {
        screenmodified = false;

        SDL_WaitEvent(&event);

        switch(event.type)
        {   //esemenyek valogatasa

            case SDL_INIT:
            {
                /*
                Sets all the local variables, needs to be pushed if dimensions are changed,
                and in the beginning. Not elegant, a function would be better, but in that
                case, these local variables must have been passed as arguments, or set as
                global variables. This was the simplest of all.
                */
                if(Cells != NULL)       //frees previously allocated cells
                {
                    free(Cells[0]);
                    free(Cells);
                }

                Dim = cfg.Dim;
                MinesNo = cfg.Mines;

                {   //setting the position variables
                    WindowSize.x = 2 * (BORDER_WINDOW + BORDER_CELLS) + Dim.x * CELL_SIZE;
                    WindowSize.y = DIGIT_Y + Dim.y*CELL_SIZE + 2*(BORDER_DIGITS + BORDER_HEADER +  BORDER_CELLS + BORDER_SMILEY) + 3*BORDER_WINDOW;
                    CellsSP.x = BORDER_WINDOW + BORDER_CELLS;
                    CellsSP.y = DIGIT_Y + BORDER_CELLS + 2*(BORDER_DIGITS + BORDER_HEADER + BORDER_SMILEY + BORDER_WINDOW);
                    MINESSP.x = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER + BORDER_DIGITS;
                    MINESSP.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER + BORDER_DIGITS;
                    MINESBSP.x = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;
                    MINESBSP.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;
                    TIMESP.x = WindowSize.x - BORDER_WINDOW - BORDER_HEADER - BORDER_SMILEY - BORDER_DIGITS - 3*DIGIT_X;
                    TIMESP.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER + BORDER_DIGITS;
                    TIMEBSP.x = WindowSize.x - BORDER_WINDOW - BORDER_HEADER - BORDER_SMILEY - 2*BORDER_DIGITS - 3*DIGIT_X;
                    TIMEBSP.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;
                    SMILEYSP.x = (WindowSize.x - SMILEY_SIZE) / 2;
                    SMILEYSP.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;
                    DIGITBDIM.x = 3*DIGIT_X + 2*BORDER_DIGITS;
                    DIGITBDIM.y =   DIGIT_Y + 2*BORDER_DIGITS;

                    Button asdButtons[3] =
                    {   { Smiley, Smiley, SMILEYSP, {SMILEY_SIZE, SMILEY_SIZE} },
                        { Settings, Settings, MINESSP, {DIGIT_X*3, DIGIT_Y} },
                        { Besttimes, Besttimes, TIMESP, {DIGIT_X*3, DIGIT_Y} }
                    };
                    int i;
                    for(i=0; i < 3; i++)
                        Buttons[i] = asdButtons[i];
                }

                screen=SDL_SetVideoMode(WindowSize.x, WindowSize.y, 0, SDL_ANYFORMAT);  //resizing window
                debugmalloc(screen, "screen");
                SDL_WM_SetCaption("Minesweeper - dobragab", "Minesweeper - dobragab");

                //allocating new array for cells
                Cells = (Cell**)malloc(Dim.x * sizeof(Cell*));
                debugmalloc(Cells, "Cells");

                Cells[0] = (Cell*) malloc( Dim.x * Dim.y * sizeof(Cell) );
                debugmalloc(Cells[0], "Cells[0]");

                int i;
                for(i=1; i < Dim.x; i++)
                    Cells[i] = Cells[0] + i * Dim.y;

                /** IMPORTANT NOTE
                If you look carefully, you can see that I allocated the pointer-array for the columns but
                the rows, unlike the conventions. In this case it makes our life easier: since I work
                with coordinates, the indexing will be in the correct order: Cells[x][y].
                Or Cells[column][row], but I will never use this.
                */

                //backround and borders
                boxColor(screen, 0, 0, WindowSize.x, WindowSize.y, C_GREY25);
                DefaultBorders(screen, WindowSize, Dim, cfg.bBnw);
            }

            /* HUGE FALLTHROUGH*/ /* HUGE FALLTHROUGH*/
            /* HUGE FALLTHROUGH*/ /* HUGE FALLTHROUGH*/
            //because there must be a new game at every init :)

            case SDL_NEWGAME:
            {
                sec = 0;

                //cleaning mine field
                int i, j;
                for(i=0; i < Dim.x; i++)
                    for(j=0; j < Dim.y; j++)
                        {
                            Cells[i][j].preset = 0;
                            Cells[i][j].state = empty;
                            Cells[i][j].clickable = true;
                        }
                //draws starting, blank cells
                DefaultCells(screen, cells, CellsSP, Dim, cfg.bBnw);
                leftmines = MinesNo;
                uncovered = Dim.x*Dim.y-MinesNo;
                started = false;
                blocked = false;

                //refreshing buttons
                DrawDigits(screen, digits, leftmines, MINESSP, cfg.bBnw);
                DrawDigits(screen, digits, sec, TIMESP, cfg.bBnw);
                DrawSmiley(screen, smileys, happy, SMILEYSP, cfg.bBnw);

                screenmodified = true;
                break;
            }

            case SDL_MOUSEBUTTONDOWN:  //clicked mouse
            {
                //Getting clicked cell and button. Only one of them can be valid at the same time.
                C = GetClickedCell(event.button.x, event.button.y, CellsSP);
                btn = GetClickedButton(event.button.x, event.button.y, Buttons, ButtonsNo);

                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:

                        //animating smiley
                        if(!blocked)
                            DrawSmiley(screen, smileys, surprised, SMILEYSP, cfg.bBnw);
                        screenmodified = true;

                        if(btn != -1)
                        {   //animating button press
                            switch(Buttons[btn].type)
                            {
                                case Smiley:
                                    DrawSmiley(screen, smileys, clickedhappy, SMILEYSP, cfg.bBnw);
                                    SDL_RemoveTimer(stw);
                                    break;
                                case Settings:
                                    DrawBorder(screen, MINESBSP, DIGITBDIM, BORDER_DIGITS, false, cfg.bBnw);
                                    break;
                                case Besttimes:
                                    DrawBorder(screen, TIMEBSP, DIGITBDIM, BORDER_DIGITS, false, cfg.bBnw);
                                    break;
                                default:
                                    break;
                            }
                            screenmodified = true;
                        }

                        else if(!blocked)
                        {
                            if(IsInDim(C, Dim))
                            {
                                mouseldown = true;
                                if(!mouserdown) simpleclick = true;
                                else            simpleclick = false;
                            }
                            //animating pushing of cell
                            if(IsInDim(C, Dim) && Cells[C.x][C.y].state != flag &&  Cells[C.x][C.y].clickable)
                                DrawCell(screen, cells, C, CellsSP, Cells[C.x][C.y].state + 1, cfg.bBnw);
                        }
                        if(mouserdown)
                        {   //pushing MMB in case "both buttons pressed"
                            mousemdown = true;
                            event.button.button = SDL_BUTTON_MIDDLE;
                            SDL_PushEvent(&event);
                        }
                        break;

                    case SDL_BUTTON_RIGHT:
                        if( !blocked && IsInDim(C, Dim))
                        {
                            mouserdown = true;
                            if(!mouseldown) simpleclick = true;
                            else            simpleclick = false;
                        }

                        if( !blocked && IsInDim(C, Dim) && Cells[C.x][C.y].clickable && !mouseldown)
                        {   //animate flagging of cell
                            NextState( & Cells[C.x][C.y].state, & leftmines, cfg.bQmark );
                            DrawCell(screen, cells, C, CellsSP, Cells[C.x][C.y].state, cfg.bBnw);
                            DrawDigits(screen, digits, leftmines, MINESSP, cfg.bBnw);
                            screenmodified = true;
                        }
                        /* //this is set for debug reasons, you can win the game immediately if you right click on smiley :)
                        else if(Buttons[btn].name == Smiley)
                        {
                            SDL_Event ev;
                            ev.type = SDL_WON;
                            SDL_PushEvent(&ev);
                        }*/

                        if(mouseldown)
                        {   //pushing MMB in case "both buttons pressed"
                            mousemdown = true;
                            event.button.button = SDL_BUTTON_MIDDLE;
                            SDL_PushEvent(&event);
                        }

                        break;

                    case SDL_BUTTON_MIDDLE:
                        if(!blocked && IsInDim(C, Dim))
                        {   //animating pushing nearby cells
                            mousemdown = true;

                            PushCells(false, pressed, Dim.x, Dim.y, Cells, C, screen, cells, numbers, &CellsSP, &uncovered, &leftmines, cfg.bBnw);
                            screenmodified = true;
                        }
                        break;
                    default:
                        break;
                }
                break;
            }

            case SDL_MOUSEBUTTONUP:
            {
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                    {
                        if(!blocked)        //animating smiley
                            DrawSmiley(screen, smileys, happy, SMILEYSP, cfg.bBnw);
                        screenmodified = true;

                        mouseldown = false;
                        mousemdown = false;

                        btn2 = GetClickedButton(event.button.x, event.button.y, Buttons, ButtonsNo);
                        if(btn != -1)
                        {   //processing and animating button push
                            screenmodified = true;
                            switch(Buttons[btn].type)
                            {
                                case Settings:
                                    DrawBorder(screen, MINESBSP, DIGITBDIM, BORDER_DIGITS, true, cfg.bBnw);
                                    break;
                                case Besttimes:
                                    DrawBorder(screen, TIMEBSP, DIGITBDIM, BORDER_DIGITS, true, cfg.bBnw);
                                    break;
                                default:
                                    break;
                            }

                            if(btn == btn2)    //for safe buttons
                            {
                                switch(Buttons[btn].type)
                                {
                                    case Smiley:
                                    {   //new game event
                                        SDL_Event ev;
                                        ev.type = SDL_NEWGAME;
                                        SDL_PushEvent(&ev);

                                        btn = -1;
                                        blocked = false;
                                        break;
                                    }
                                    case Settings:
                                    {   //opening menu "window"
                                        Point MenuSP = { (WindowSize.x-162)/2, (WindowSize.y-205)/2 };

                                        SDL_Surface *backup;
                                        backup = SDL_CreateRGBSurface(SDL_SWSURFACE, WindowSize.x, WindowSize.y, 32, 0, 0, 0, 0);
                                        SDL_BlitSurface(screen, NULL, backup, NULL);

                                        if(started)
                                            SDL_RemoveTimer(stw);
                                        MenuResult rslt = menu(screen, cells, smileys, digits, numbers, MenuSP, &cfg, &quit);
                                        if(started && rslt != huge)
                                            stw = SDL_AddTimer(1000, timer, NULL);

                                        switch(rslt)
                                        {   //saving config and reinit if necessary
                                            case huge:
                                            {
                                                SDL_Event ev;
                                                ev.type = SDL_INIT;
                                                SDL_PushEvent(&ev);
                                            }
                                            /* fallthrough */
                                            case Small:
                                                SaveConfig(&cfg, times);
                                                break;
                                            default:
                                                break;
                                        }

                                        SDL_BlitSurface(backup, NULL, screen, NULL);
                                        SDL_FreeSurface(backup);

                                        break;
                                    }
                                    case Besttimes:
                                    {   //opening best times "window"
                                        SDL_Surface *backup;
                                        backup = SDL_CreateRGBSurface(SDL_SWSURFACE, WindowSize.x, WindowSize.y, 32, 0, 0, 0, 0);
                                        SDL_BlitSurface(screen, NULL, backup, NULL);

                                        Point BestSP = { (WindowSize.x-162)/2, (WindowSize.y-108)/2 };

                                        if(started)
                                            SDL_RemoveTimer(stw);
                                        bool newbest = best(screen, smileys, digits, BestSP, times, custom, &quit, cfg.bBnw);
                                        if(started)
                                            stw = SDL_AddTimer(1000, timer, NULL);

                                        //saving times if necessary
                                        if(newbest)
                                            SaveConfig(&cfg, times);

                                        SDL_BlitSurface(backup, NULL, screen, NULL);
                                        SDL_FreeSurface(backup);

                                        break;
                                    }
                                    default:
                                        break;
                                }
                            }
                        }

                        else if(!blocked)
                        {
                            screenmodified = true;
                            C = GetClickedCell(event.button.x, event.button.y, CellsSP);

                            if(IsInDim(C, Dim) && Cells[C.x][C.y].clickable && Cells[C.x][C.y].state != flag && simpleclick)
                            {   //PROCESSING CLICKED CELL
                                if(!started)
                                {   //starting timer and generating mines
                                    GenerateMines(Cells, cfg, C);
    /** IMPORTANT NOTE
    This program differs fron original minesweeper at this point. Originally mines were generated at new
    game event, but I generate it at the first click. You can check it by xyzzy cheat. I didn't feel this
    was so important, and this version was easier to code for me.
    */
                                    sec = 0;

                                    stw = SDL_AddTimer(1000, timer, NULL);
                                    started = true;

                                    //generating another tick to set cntr to 1 to avoid 0 sec wins
                                    SDL_Event ev;
                                    ev.type = SDL_TICK;
                                    SDL_PushEvent(&ev);
                                }

                                Cells[C.x][C.y].clickable = false;

                                switch(Cells[C.x][C.y].preset)
                                {
                                    case -1:
                                    {   //game is lost
                                        SDL_Event ev;
                                        ev.type = SDL_LOST;
                                        ev.button.x = 0;
                                        SDL_PushEvent(&ev);

                                        break;
                                    }
                                    case 0:
                                    {
                                        clock_t mettol, meddig;
                                        if(UNCOVER_LOG)
                                            mettol = clock();
                                        Uncover(Dim.x, Dim.y, Cells, C, screen, numbers, &CellsSP, &uncovered, &leftmines, cfg.bBnw);
                                        if(UNCOVER_LOG)
                                        {
                                            meddig = clock();
                                            fprintf(stderr, "%.4f masodperc.\n", (meddig-mettol)/(double)CLOCKS_PER_SEC);
                                        }

                                        DrawDigits(screen, digits, leftmines, MINESSP, cfg.bBnw);
                                        break;
                                    }
                                    default:

                                        DrawNumber(screen, numbers, C, CellsSP, Cells[C.x][C.y].preset, cfg.bBnw);
                                        uncovered -= 1;
                                        Cells[C.x][C.y].clickable = false;
                                        break;
                                }
                            }
                        }

                        if(mouserdown)
                        {
                            event.button.button = SDL_BUTTON_MIDDLE;
                            SDL_PushEvent(&event);
                        }
                        if(!mouserdown)
                            simpleclick = true;

                        break;
                    }

                    case SDL_BUTTON_RIGHT:
                        mouserdown = false;
                        mousemdown = false;
                        if(mouseldown)
                        {   //releasing MMB in case "both buttons arent pressed anymore"
                            event.button.button = SDL_BUTTON_MIDDLE;
                            SDL_PushEvent(&event);
                        }
                        else
                            simpleclick = true;

                        break;

                    case SDL_BUTTON_MIDDLE:
                    {   //processing pushing nearby cells
                        C = GetClickedCell(event.button.x, event.button.y, CellsSP);
                        if(!blocked && IsInDim(C, Dim))
                        {
                            PushCells(true, normal, Dim.x, Dim.y, Cells, C, screen, cells, numbers, &CellsSP, &uncovered, &leftmines, cfg.bBnw);
                            DrawDigits(screen, digits, leftmines, MINESSP, cfg.bBnw);
                            mousemdown = false;
                            screenmodified = true;
                        }
                        break;
                    }
                    default:
                        break;
                }

                if(uncovered == 0 && !blocked && started)
                {   //checking if we've won the game
                    SDL_Event ev;
                    ev.type = SDL_WON;
                    SDL_PushEvent(&ev);
                    mouseldown = false;
                    mouserdown = false;
                    mousemdown = false;
                }
                break;
            }

            case SDL_MOUSEMOTION:
            {
                if(mousemdown && !blocked)
                {   //if you hold MMB and move the mouse, the pushed area moves with the cursor.
                    CNext = GetClickedCell(event.motion.x, event.motion.y, CellsSP);

                    if( CNext.x != C.x || CNext.y != C.y)
                    {
                        if(IsInDim(C, Dim))
                            PushCells(false, normal, Dim.x, Dim.y, Cells, C, screen, cells, numbers, &CellsSP, &uncovered, &leftmines, cfg.bBnw);
                        if(IsInDim(CNext, Dim))
                            PushCells(false, pressed, Dim.x, Dim.y, Cells, CNext, screen, cells, numbers, &CellsSP, &uncovered, &leftmines, cfg.bBnw);
                        C = CNext;
                    }
                    screenmodified = true;
                }
                else if(mouseldown && !blocked)
                {   //if you hold LMB and move the mouse, the pushed cell moves with the cursor.
                    CNext = GetClickedCell(event.motion.x, event.motion.y, CellsSP);

                    if( CNext.x != C.x || CNext.y != C.y)
                    {
                        if( IsInDim(C, Dim) && Cells[C.x][C.y].clickable )
                            DrawCell(screen, cells, C, CellsSP, Cells[C.x][C.y].state, cfg.bBnw);

                        if( IsInDim(CNext, Dim) && Cells[CNext.x][CNext.y].state != flag && Cells[CNext.x][CNext.y].clickable )
                            DrawCell(screen, cells, CNext, CellsSP, Cells[CNext.x][CNext.y].state + 1, cfg.bBnw);

                        C = CNext;
                    }
                    screenmodified = true;
                }
                else if( cheat )
                {   //this is for the cheat mode, at the top left px of the window you can see a black px if
                    //there's a mine under your cursor.
                    Point D = GetClickedCell(event.motion.x, event.motion.y, CellsSP);
                    if(IsInDim(D, Dim) && Cells[D.x][D.y].preset == -1)
                        pixelColor(screen, 0, 0, C_BLACK);
                    else
                        pixelColor(screen, 0, 0, C_WHITE);
                    screenmodified = true;
                }
                break;
            }

            case SDL_KEYDOWN:
            {   //this event is required only to enter cheat mode
                switch(event.key.keysym.unicode)
                {
                    case 'x': //puts 5 last pressed keys in word array, only if it's x, y or z.
                    case 'y': //others arent necessary since code is xyzzy
                    case 'z':
                    {
                        int i;
                        for(i=0; i < 4; i++)
                            word[i] = word[i+1];
                        word[4] = event.key.keysym.unicode;
                        puts(word);
                        break;
                    }
                    default:
                        break;
                }

                //activating cheat by pressing shift
                if(event.key.keysym.sym == SDLK_LSHIFT || event.key.keysym.sym == SDLK_RSHIFT)
                {
                    if(strcmp(word, "xyzzy") == 0)
                        cheat = true;
                }
                else if(event.key.keysym.sym == SDLK_ESCAPE)
                    quit = true;

                break;
            }

            case SDL_WON:
            {   //event for won game, pushed at mousebuttonup if uncovered is 0
                if(cfg.bSound)
                    Mix_PlayMusic(won,1);

                //draws flags on all mines, even if they arent flagged
                int i, j;
                for(i=0; i < Dim.x; i++)
                    for(j=0; j < Dim.y; j++)
                        if(Cells[i][j].preset == -1 && Cells[i][j].state != flag)
                            DrawCell(screen, cells, (Point){i,j}, CellsSP, flag, cfg.bBnw);

                leftmines = 0;
                DrawDigits(screen, digits, leftmines, MINESSP, cfg.bBnw);
                DrawSmiley(screen, smileys, sunglass, SMILEYSP, cfg.bBnw);
                blocked = true;
                screenmodified = true;
                SDL_RemoveTimer(stw);
                started = false;

                //if this is the best time of this preset, starts besttimes function with typing
                if(cfg.prst && times[cfg.prst-1].Time > sec)
                {
                    times[cfg.prst-1].Time = sec;

                    SDL_Surface *backup;
                    backup = SDL_CreateRGBSurface(SDL_SWSURFACE, WindowSize.x, WindowSize.y, 32, 0, 0, 0, 0);
                    SDL_BlitSurface(screen, NULL, backup, NULL);

                    Point BestSP = { (WindowSize.x-162)/2, (WindowSize.y-108)/2 };
                    bool newbest = best(screen, smileys, digits, BestSP, times, cfg.prst, &quit, cfg.bBnw);
                    if(newbest)
                        SaveConfig(&cfg, times);

                    SDL_BlitSurface(backup, NULL, screen, NULL);
                    SDL_FreeSurface(backup);
                }
                break;
            }

            case SDL_LOST:
            {   //event for lost game, pushed when user clicks not flagged mine, or mistaken middlemousebutton trick
                if(cfg.bSound)
                    Mix_PlayMusic(lost,1);

                //shows all non flagged mines
                int i, j;
                for(i=0; i < Dim.x; i++)
                    for(j=0; j < Dim.y; j++)
                    {
                        if(Cells[i][j].preset == -1 && Cells[i][j].state != flag)
                            DrawCell(screen, cells, (Point){i,j}, CellsSP, mine, cfg.bBnw);
                    }

                /*
                this part of the code is not obvious. event.button.x (Uint16) is used for flagging
                which surrounding mines were clicked. 0 if only C. Multiple mines can be "clicked"
                if you middleclick on a covered mine containing a number (e.g. 3), and exactly 3
                surrounding cells were flagged. If you flag wrong cells, you will lose, and all
                3 real mines will be considered as clicked mines. I always walk through surrounding
                mines using the following loop:
                    for(k = Cap(C.x-1, 0, Dim.x-1); k <= kmax ; k++)
                        for(l = Cap(C.y-1, 0, Dim.y-1); l <= lmax; l++)
                so at any clicked cell, the surrounding cells are walked through always in the same
                order, no matter if they are at the borders, or corner. this order is counted by i.
                so the bits of event.button.x contain which surrounding cells were clicked, 0th cell
                is the LSB, 1st cell is 2nd bit, etc. (max number is 9 so Uint16 can store it)
                */

                if(event.button.x == 0)
                    DrawCell(screen, cells, C, CellsSP, clickedmine, cfg.bBnw);
                else
                {
                    //draws clicked mines
                    int k, l, i=0;
                    int kmax = Cap(C.x+1, 0, Dim.x-1);
                    int lmax = Cap(C.y+1, 0, Dim.y-1);
                    for(k = Cap(C.x-1, 0, Dim.x-1); k <= kmax ; k++)
                        for(l = Cap(C.y-1, 0, Dim.y-1); l <= lmax; l++, i++)
                        {
                            if(event.button.x & (1<<i))
                                DrawCell(screen, cells, (Point){k,l}, CellsSP, clickedmine, cfg.bBnw);

                            Cells[k][l].clickable = true;
                            /*
                            necessary for drawing fake mines
                            When middleclick is released at a valid cell, processing is done one by one
                            at surrounding cells. If one of them are empty, Uncover function makes all
                            cells unclickable around empty cells. A flagged cell might be uncovered as
                            well, but at middleclick those should be marked as fake mines, too.
                            But we must check for clickability at drawing fake mines, because when i uncover
                            a flagged cell, its state isnt changed but the clickable flag.
                            */
                        }
                    //printf("%x\n", event.button.x);
                }

                //draws fake mines
                for(i=0; i < Dim.x; i++)
                    for(j=0; j < Dim.y; j++)
                        if(Cells[i][j].clickable && Cells[i][j].state == flag && Cells[i][j].preset != -1)
                            DrawCell(screen, cells, (Point){i,j}, CellsSP, fakemine, cfg.bBnw);

                DrawSmiley(screen, smileys, dead, SMILEYSP, cfg.bBnw);
                blocked = true;
                screenmodified = true;
                SDL_RemoveTimer(stw);
                started = false;
                break;
            }

            case SDL_TICK:
            {   //timer tick event: playing sound and increasing counter
                if(cfg.bSound)
                    Mix_PlayMusic(tick, 1);
                if(sec < 999)
                {
                    sec += 1;
                    DrawDigits(screen, digits, sec, TIMESP, cfg.bBnw);
                    screenmodified = true;
                }
                break;
            }

            case oMark: //I hope you will find my easter egg, Mr Kaszó ;)
                        //Actually I think this is worse than any "dad jokes", but I couldn't resist.
                        //#include <joke.h>
                        //bool shit;
                break;

            case SDL_QUIT:
                //exits from event handling loop
                quit = true;
                break;
        }
        if(screenmodified && !quit)
            SDL_Flip(screen);
    }

    free(Cells[0]);
    free(Cells);

    #ifdef MYDEBUG
    debugmalloc_dump();
    #endif

    SDL_Quit();

    return 0;
}
