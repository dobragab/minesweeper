#include "main.h"

#include "menu.h"           //contains declaration of menu()
#include "menuconstants.h"  //contains the following global variables used in menu, and timer settings
#include "menuevents.h"     //contains button clicked events

Point SIZESP, PRESETSP, SETSP, SETCELLSBSP, SETCELLSSP, WIDTHBSP, HEIGHTBSP, MINESBSP, WIDTHSP, HEIGHTSP, MINESSP, OWLSP, XSP, XMINE, LEFTEYE, RIGHTEYE, MIDEYE;
Size SIZESIZE, PRESETSIZE, SETSIZE, SETCELLSBSIZE, SETCELLSSIZE, DIGITBDIM;

void SetMenuConstants(Point);               //sets values of global variables, using SP of menu on the screen
void DrawMenuBorders(SDL_Surface*, bool);   //draws all the borders seen in menu

/* menu(...) function **************************************************************************************

Description:    Draws a "window" in the middle of the screen, and displays the Menu, with a possibility of
                changing the main settings of the game
Parameters:         SDL_Surface     *screen,    (destination surface)
                    SDL_Surface     *cells,     (source image)
                    SDL_Surface     *smileys,   (source image)
                    SDL_Surface     *digits,    (source image)
                    SDL_Surface     *numbers,   (source image)
                    Point           SP,         (starting point of the "window")
                    Config          *cfg,       (address of config from main() )
                    bool            *quit,      (address of quit condition of game, changed at SDL_QUIT ev.)
Returns:        a boolean value if config must be saved again, && config and quit as parameters
********************************************************************************************************* */

MenuResult menu(SDL_Surface *screen, SDL_Surface *cells, SDL_Surface *smileys, SDL_Surface *digits, SDL_Surface *numbers,
                Point SP, Config *cfg, bool *quit)
{   //the menu module, return value varies by config values changed
    Size MenuDim = {162, 205};      //the dimensions of the menu could not be bigger than game window
                                    //at smallest size, could have been bigger for better design :)
    Config asdcfg = *cfg;           //creates a copy of original config to avoid unwanted overwriting
    MenuResult rslt = original;

    SetMenuConstants(SP);

    SDL_Surface *owls, *btns;       //loading special images used only in menu (and best)
    debugmalloc(owls = IMG_Load("owl.png"), "owl.png");
    debugmalloc(btns = IMG_Load("btns.png"), "btns.png");

    Point BEGSP[6] =                //sets SPs of 'flag' symbols for 3 levels
    {   {8+SP.x, 145+SP.y},
        {8+SP.x, 163+SP.y},
        {8+SP.x, 181+SP.y},
        {24+SP.x, 163+SP.y},
        {24+SP.x, 181+SP.y},
        {40+SP.x, 181+SP.y}     };

    unsigned ButtonsNo = 14;
    Button MenuButtons[] =          //contains data of all the buttons saw at menu
    {   { left,  WIDTHDOWN,       {63+SP.x,  13+SP.y},  {CELL_SIZE, CELL_SIZE}, fast, WidthDown },
        { right, WIDTHUP,         {126+SP.x, 13+SP.y},  {CELL_SIZE, CELL_SIZE}, fast, WidthUp },
        { up,    HEIGHTUP,        {21+SP.x,  41+SP.y},  {CELL_SIZE, CELL_SIZE}, fast, HeightUp },
        { down,  HEIGHTDOWN,      {21+SP.x,  84+SP.y},  {CELL_SIZE, CELL_SIZE}, fast, HeightDown },
        { left,  MINEMINUS,       {15+SP.x,  115+SP.y}, {CELL_SIZE, CELL_SIZE}, fast, MinesDown },
        { right, MINEPLUS,        {78+SP.x,  115+SP.y}, {CELL_SIZE, CELL_SIZE}, fast, MinesUp },
        { checkmark,BEGINNER,     {59+SP.x,  145+SP.y}, {CELL_SIZE, CELL_SIZE}, safe, BeginnerSet },
        { checkmark,INTERMEDIATE, {59+SP.x,  163+SP.y}, {CELL_SIZE, CELL_SIZE}, safe, IntermediateSet },
        { checkmark,EXPERT,       {59+SP.x,  181+SP.y}, {CELL_SIZE, CELL_SIZE}, safe, ExpertSet },
        { asdcfg.bQmark ? qmarkk: xqmark,  MARKQ, {93+SP.x,  153+SP.y}, {CELL_SIZE, CELL_SIZE}, safe, QmarkClicked },
        { asdcfg.bBnw ? colored : bnw,     BNW,   {112+SP.x, 153+SP.y}, {CELL_SIZE, CELL_SIZE}, safe, BnwClicked   },
        { asdcfg.bSound ? sound : xsound,  SOUND, {131+SP.x, 153+SP.y}, {CELL_SIZE, CELL_SIZE}, safe, SoundClicked },
        { checkmark, OK,          {122+SP.x, 184+SP.y}, {CELL_SIZE, CELL_SIZE}, safe },
        { redx, CANCEL,           {141+SP.x, 184+SP.y}, {CELL_SIZE, CELL_SIZE}, safe }         };

    SDL_TimerID id = NULL;          //this timer is used for fast menu buttons

    asdcfg.prst = GetPreset(asdcfg);//counts if currently set config is any preset

    SDL_Event event;                //pushes init event, this must be an event because whole menu is
    event.type = SDL_INIT;          //redrawn if B&W changed
    SDL_PushEvent(&event);

    int btn = -1, btn2 = -1;        //same as in game buttons, and btn2 is required for safe buttons
    bool exit = false;
    bool screenmodified = false;
    bool mouseldown = false;        //necessary for eyes of the owl
    unsigned timercnt = 0;

    while(!exit)
    {
        screenmodified = false,
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_INIT:
            {   //drawing whole menu panel
                boxColor(screen, SP.x, SP.y, SP.x+MenuDim.x, SP.y+MenuDim.y, C_GREY25);
                DrawBorder(screen, SP, MenuDim, 3, true, asdcfg.bBnw);
                DrawMenuBorders(screen, asdcfg.bBnw);

                DefaultCells(screen, cells, SETCELLSSP, SETCELLSSIZE, asdcfg.bBnw);
                DrawCounters(screen, digits, asdcfg);

                int i;
                for(i=0; i < ButtonsNo; i++)
                    DrawButton(screen, btns, MenuButtons[i], normal, asdcfg.bBnw);
                for(i=0; i < 6; i++)
                    DrawCell(screen, cells, (Point){0,0}, BEGSP[i], redflag, asdcfg.bBnw);

                DrawSmiley(screen, owls, happy, OWLSP, asdcfg.bBnw );

                Button xbtn = {redx, CANCEL, XSP, {CELL_SIZE, CELL_SIZE}, safe };
                DrawButton(screen, btns, xbtn, clicked, asdcfg.bBnw);
                DrawCell(screen, cells, (Point){0,0}, XMINE, clickedmine, asdcfg.bBnw );

                if(asdcfg.prst != custom)
                    DrawButton(screen, btns, MenuButtons[asdcfg.prst + 5], pressed, asdcfg.bBnw);
                                                        //sorry for this index
                screenmodified = true;
                break;
            }
            case SDL_TICK:
                //actually fast button event, when 5th timer is ticked, further fast button clicks
                //are sent at each tick until left mouse btn is released
                /** IMPORTANT NOTE
                The reason of the segmentation fault can be found here. When I started menu when I was
                playing, the timer of game wasn't stopped, but handling started here, so after 6 seconds
                MenuButtons[-1].click(screen, digits, &asdcfg, NULL);
                was called... :( It couldn't be a problem when game timer wasnt on, because menu timer was
                only running when btn was set.
                */

                if(timercnt >= TICKS_BEFORE_START && btn != -1)
                {
                    MenuButtons[btn].click(screen, digits, &asdcfg, NULL);
                    screenmodified = true;
                }
                else
                    timercnt++;
                break;


            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseldown = true;
                    DrawSmiley(screen, owls, surprised, OWLSP, asdcfg.bBnw );
                    screenmodified = true;
                    btn = GetClickedButton(event.button.x, event.button.y, MenuButtons, ButtonsNo);

                    if(btn != -1)
                    {
                        DrawButton(screen, btns, MenuButtons[btn], pressed, asdcfg.bBnw);
                        if(MenuButtons[btn].mode == fast)
                        {       //starts fast button counting
                            MenuButtons[btn].click(screen, digits, &asdcfg, NULL);
                            id = SDL_AddTimer(REPEAT_RATE, timer, NULL);
                        }
                    }
                }

                break;

            case SDL_MOUSEBUTTONUP:
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseldown = false;
                    timercnt = 0;
                    SDL_RemoveTimer(id);

                    int i;
                    for(i=6; i < 9; i++) //sorry for these indexes as well... indexes 6-8 are preset checkmarks
                        DrawButton(screen, btns, MenuButtons[i], normal, asdcfg.bBnw);

                    btn2 = GetClickedButton(event.button.x, event.button.y, MenuButtons, ButtonsNo);

                    if(btn != -1 && btn2 == btn && MenuButtons[btn].mode == safe)
                    {   //checks if mousebuttonup is sent at the same btn as mousebuttondown
                        switch(MenuButtons[btn].name)
                        {
                            case CANCEL:
                                exit = true;
                                rslt = original;
                                break;

                            case OK:

                                exit = true;
                                rslt = original;
                                if(cfg->Mines != asdcfg.Mines || cfg->Dim.x != asdcfg.Dim.x || cfg->Dim.y != asdcfg.Dim.y || cfg->bBnw != asdcfg.bBnw)
                                    rslt = huge;
                                else if(cfg->bSound != asdcfg.bSound || cfg->bQmark != asdcfg.bQmark)
                                    rslt = Small;

                                *cfg = asdcfg;
                                cfg->mm = GetMineMode(*cfg);
                                cfg->prst = GetPreset(*cfg);
                                exit = true;
                                break;

                            case BNW:
                                MenuButtons[btn].click(screen, digits, &asdcfg, &(MenuButtons[btn].type));

                                SDL_Event ev;                   //sends mousemotion event because eye must
                                ev.type = SDL_MOUSEMOTION;      //be redrawn at b&W change, too
                                ev.motion.x = event.button.x;
                                ev.motion.y = event.button.y;
                                SDL_PushEvent(&ev);
                                break;

                            default:
                                MenuButtons[btn].click(screen, digits, &asdcfg, &(MenuButtons[btn].type));
                                break;
                        }
                    }

                    if(btn != -1)
                        DrawButton(screen, btns, MenuButtons[btn], normal, asdcfg.bBnw);

                    asdcfg.prst = GetPreset(asdcfg);
                    if(asdcfg.prst != custom)
                        DrawButton(screen, btns, MenuButtons[asdcfg.prst + 5], pressed, asdcfg.bBnw);
                                                            //srsly i regret this
                    screenmodified = true;
                }

                /* HUGE FALLTHROUGH*/ /* HUGE FALLTHROUGH*/
                /* HUGE FALLTHROUGH*/ /* HUGE FALLTHROUGH*/
                //because owl needs to be redrawn everytime it stops drinking

            case SDL_MOUSEMOTION:
                //only contains stuff for moving eye of the owl
                if(!mouseldown)     //draw eye only if it isn't drinking beer atm
                {
                    screenmodified = true;
                    DrawSmiley(screen, owls, happy, OWLSP, asdcfg.bBnw );

                    if(event.motion.x == MIDEYE.x && event.motion.y == MIDEYE.y)
                    {   //event.motion works fine even at fallthrough, i have no idea why :))
                        pixelColor(screen, LEFTEYE.x,  LEFTEYE.y,  C_BLACK);
                        pixelColor(screen, RIGHTEYE.x, RIGHTEYE.y, C_BLACK);
                    }

                    else
                    {   //counts position of eyes
                        double lambda = (double) 2 / sqrt( (event.motion.x-MIDEYE.x)*(event.motion.x-MIDEYE.x)  +  (event.motion.y-MIDEYE.y)*(event.motion.y-MIDEYE.y) );
                        double p = lambda * (event.motion.x-MIDEYE.x);
                        double q = lambda * (event.motion.y-MIDEYE.y);

                        pixelColor(screen, LEFTEYE.x +p, LEFTEYE.y +q, C_BLACK);
                        pixelColor(screen, RIGHTEYE.x+p, RIGHTEYE.y+q, C_BLACK);
                    }
                }
                break;

            case SDL_QUIT:
                exit = true;        //exits from event handling loop
                *quit = true;       //quits from game as well
                break;

            default:
                break;
        }

        if(screenmodified)
            SDL_Flip(screen);
    }

    SDL_FreeSurface(owls);
    SDL_FreeSurface(btns);

    return rslt;
}

/* SetMenuConstants(...) function **************************************************************************

Description:    Sets the values of the constants used in menu(...) function depending from SP
Parameters:         Point           SP,         (starting point of the menu "window")
Returns:        void
********************************************************************************************************* */

void SetMenuConstants(Point SP)
{
    SIZESP =        (Point) {SP.x+4, SP.y+4};
    SIZESIZE =      (Size)  {154, 135};

    PRESETSP =      (Point) {SP.x+4, SP.y+141};
    PRESETSIZE =    (Size)  {75, 60};

    SETSP =         (Point) {SP.x+88, SP.y+148};
    SETSIZE =       (Size)  {64, 26};

    SETCELLSBSP =   (Point) {SP.x+51, SP.y+36};
    SETCELLSBSIZE = (Size)  {102, 70};

    SETCELLSSP =    (Point) {SP.x+54, SP.y+39};
    SETCELLSSIZE =  (Size)  {6, 4};

    WIDTHBSP  =     (Point) {SP.x+82, SP.y+9};
    HEIGHTBSP =     (Point) {SP.x+8, SP.y+58};
    MINESBSP  =     (Point) {SP.x+34, SP.y+111};

    DIGITBDIM =     (Size)  {3*DIGIT_X + 2*BORDER_DIGITS, DIGIT_Y + 2*BORDER_DIGITS};

    WIDTHSP  =      (Point) {SP.x+83, SP.y+10};
    HEIGHTSP =      (Point) {SP.x+9, SP.y+59};
    MINESSP  =      (Point) {SP.x+35, SP.y+112};

    OWLSP =         (Point) {SP.x+16, SP.y+8};
    XSP =           (Point) {104+SP.x, 115+SP.y};
    XMINE =         (Point) {130+SP.x, 115+SP.y};

    LEFTEYE =       (Point) {SP.x+25, SP.y+16};
    RIGHTEYE =      (Point) {SP.x+32, SP.y+16};
    MIDEYE =        (Point) {SP.x+28, SP.y+16};
}

/* DrawMenuBorders(...) function ***************************************************************************

Description:    Draws the default borders of menu(...) function on screen
Parameters:         SDL_Surface     *screen,    (destination surface)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */

void DrawMenuBorders(SDL_Surface *screen, bool bnw)
{
    DrawBorder(screen, SIZESP,   SIZESIZE,   2, false, bnw);
    DrawBorder(screen, PRESETSP, PRESETSIZE, 2, false, bnw);
    DrawBorder(screen, SETSP,    SETSIZE,    2, false, bnw);
    DrawBorder(screen, SETCELLSBSP, SETCELLSBSIZE, 3, false, bnw);
    DrawBorder(screen, WIDTHBSP,  DIGITBDIM, 1, false, bnw);
    DrawBorder(screen, HEIGHTBSP, DIGITBDIM, 1, false, bnw);
    DrawBorder(screen, MINESBSP,  DIGITBDIM, 1, false, bnw);
}
