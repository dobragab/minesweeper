#include "main.h"

void input_text(SDL_Surface*, TTF_Font*, char*, Point, Size, bool);

/* best(...) function **************************************************************************************

Description:    Writes a "window" in the middle of the screen, and displays Best Times, with a possibility
                of resetting the times, and typing name of the record holder if necessary
Parameters:         SDL_Surface     *screen,    (destination surface)
                    SDL_Surface     *smileys,   (source image)
                    SDL_Surface     *digits,    (source image)
                    Point           SP,         (starting point of the "window")
                    BestTime        *times,     (array containing best times and names, and new values)
                    Preset          newrecord,  (custom (0) if only displaying if times, otherwise preset
                                                of new record)
                    bool            *quit,      (address of quit condition of game, changed at SDL_QUIT ev.)
                    bool            bnw         (drawn in black and white, or not)
Returns:        a boolean value if config must be saved again, && times and quit as parameters
********************************************************************************************************* */

bool best(SDL_Surface *screen, SDL_Surface *smileys, SDL_Surface *digits, Point SP, BestTime *times, Preset newrecord, bool *quit, bool bnw)
{   //this function is the best times module of the game.

    Size BestDim = {162, 108};          //width cant be bigger than width of game window at 9×9 table

    boxColor(screen, SP.x, SP.y, SP.x+BestDim.x, SP.y+BestDim.y, C_GREY25);     //background
    DrawBorder(screen, SP, BestDim, 3, true, bnw);          //main border

    //position variables for best times, I was lazy to put these into a function
    Point BEGBSP = {SP.x+117, SP.y+25};
    Point INTBSP = {SP.x+117, SP.y+52};
    Point EXPBSP = {SP.x+117, SP.y+79};

    Size DIGITBDIM = {3*DIGIT_X + 2*BORDER_DIGITS, DIGIT_Y + 2*BORDER_DIGITS};

    DrawBorder(screen, BEGBSP, DIGITBDIM, 1, false, bnw);
    DrawBorder(screen, INTBSP, DIGITBDIM, 1, false, bnw);
    DrawBorder(screen, EXPBSP, DIGITBDIM, 1, false, bnw);

    Point BEGSP = {SP.x+118, SP.y+26};
    Point INTSP = {SP.x+118, SP.y+53};
    Point EXPSP = {SP.x+118, SP.y+80};

    DrawDigits(screen, digits, times[0].Time, BEGSP, bnw);
    DrawDigits(screen, digits, times[1].Time, INTSP, bnw);
    DrawDigits(screen, digits, times[2].Time, EXPSP, bnw);

    TTF_Font *font;
    TTF_Init();
    debugmalloc(font = TTF_OpenFont("consolab.ttf", 10), "consolab.ttf");

    Point TEXTSP[4] =           //contains SPs of labels
    {   {SP.x+8, SP.y+11},      //this is for "ENTER to finish" label
        {SP.x+8, SP.y+33},      //and these 3 are the 3 names
        {SP.x+8, SP.y+61},
        {SP.x+8, SP.y+88}   };

    Uint16 text[4][18];         //unicode arrays for label texts, same indexing as above
    ///unfortunately these local variables contain names at indexes 1-3 unlike times array,
    ///where they are at 0-2. I'm sorry again, this will cause unpleasant +1-s at indexing

    int i;
    for(i=0; i < 3; i++)
        utf8_2_unicode(times[i].Name, text[i+1]);   //converting names to unicode array
    utf8_2_unicode( "ENTER to finish", text[0] );   //and other label as well

    /*
    newrecord stands for multiple values: its type is Preset, so it can be custom, and 3 levels.
    if it's one of 3 pre-set levels, it contains which record is new, where the textbox should
    appear. If it's custom (value: 0) it means there is no new record, the user clicked right
    counter to check best times
    */
    if(newrecord)
    {
        for(i=1; i < 4; i++)                        //drawing non-changed
            if(i != newrecord)
                DrawLabel(screen, font, TEXTSP[i], text[i], bnw);

        DrawLabel(screen, font, TEXTSP[0], text[0], bnw);

        //TextBox variables
        Point TBBSP = {TEXTSP[newrecord].x - 3, TEXTSP[newrecord].y - 2};   //position of textbox
        Size TBBDIM = {110, 14};
        Size TBDIM = {104, 9};
        DrawBorder(screen, TBBSP, TBBDIM, 2, false, bnw);       //border of textbox

        SDL_Flip(screen);

        SDL_EnableKeyRepeat(500, 30);

        //getting input text, actually the name of the new record holder
        input_text(screen, font, times[newrecord-1].Name, TEXTSP[newrecord], TBDIM, bnw);
        utf8_2_unicode(times[newrecord-1].Name, text[newrecord]);
    }

    //erases previous labels set for input text
    Point ERASESP = {SP.x+5, SP.y+11};
    Size ERASEDIM = {110, 90};

    boxColor(screen, ERASESP.x, ERASESP.y, ERASESP.x+ERASEDIM.x, ERASESP.y+ERASEDIM.y, C_GREY25);

    //redraws current ones
    for(i=1; i < 4; i++)
        DrawLabel(screen, font, TEXTSP[i], text[i], bnw);

    SDL_Surface *btns;
    debugmalloc(btns = IMG_Load("btns.png"), "btns.png");

    unsigned ButtonsNo = 2;
    Button BestButtons[] =      //buttons array just like in game
    {   { happy,  RESET, {55+SP.x, 4+SP.y},  {SMILEY_SIZE, SMILEY_SIZE} },
        { redx,  CLOSEBEST, {142+SP.x, 4+SP.y},  {CELL_SIZE, CELL_SIZE} }      };

    DrawSmiley(screen, smileys, happy, BestButtons[0].SP, bnw);
    DrawButton(screen, btns, BestButtons[1], normal, bnw);
    SDL_Flip(screen);

    bool result = false;        //true if user changed the names and times, will be part of return value
    bool exit = false;
    bool screenchanged = false;
    SDL_Event event;
    int btn = -1, btn2 = -1;

    while(!exit)
    {
        screenchanged = false;
        SDL_WaitEvent(&event);

        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                {   //really similar event handling as in game, but a bit shorter :)
                    btn = GetClickedButton(event.button.x, event.button.y, BestButtons, ButtonsNo);

                    DrawSmiley(screen, smileys, surprised, BestButtons[0].SP, bnw);
                    screenchanged = true;

                    if(btn != -1)           //if a button was clicked
                    {
                        switch(BestButtons[btn].name)
                        {                   //draws pushed buttons
                            case CLOSEBEST:
                            {
                                DrawButton(screen, btns, BestButtons[1], clicked, bnw);
                                break;
                            }
                            case RESET:
                            {
                                DrawSmiley(screen, smileys, clickedhappy, BestButtons[0].SP, bnw);
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                if(event.button.button == SDL_BUTTON_LEFT)
                {
                    DrawSmiley(screen, smileys, happy, BestButtons[0].SP, bnw);
                    screenchanged = true;

                    //same as in main module, buttons are safe here
                    btn2 = GetClickedButton(event.button.x, event.button.y, BestButtons, ButtonsNo);

                    if(btn != -1)           //if a button was pressed before
                    {
                        switch(BestButtons[btn].name)
                        {
                            case CLOSEBEST:
                            {   //closing best times
                                DrawButton(screen, btns, BestButtons[1], normal, bnw);
                                if(btn == btn2)
                                    exit = true;
                                break;
                            }
                            case RESET:
                            {   //reset times and names to default
                                DrawSmiley(screen, smileys, happy, BestButtons[0].SP, bnw);

                                if(btn == btn2)
                                {
                                    result = true;

                                    //erasing previous names
                                    ERASESP = (Point) {SP.x+3, SP.y+30};
                                    ERASEDIM = (Size) {113, 74};
                                    boxColor(screen, ERASESP.x, ERASESP.y, ERASESP.x+ERASEDIM.x, ERASESP.y+ERASEDIM.y, C_GREY25);

                                    //resetting everything in times
                                    int i;
                                    for(i=0; i < 3; i++)
                                    {
                                        strcpy(times[i].Name, "Anonymous");
                                        times[i].Time = 999;
                                        utf8_2_unicode(times[i].Name, text[i+1]);
                                        DrawLabel(screen, font, TEXTSP[i+1], text[i+1], bnw);
                                    }

                                    //updating counters
                                    DrawDigits(screen, digits, times[0].Time, BEGSP, bnw);
                                    DrawDigits(screen, digits, times[1].Time, INTSP, bnw);
                                    DrawDigits(screen, digits, times[2].Time, EXPSP, bnw);
                                }

                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
                break;
            }

            case SDL_QUIT:
                exit = true;        //quitting from event loop
                *quit = true;       //quitting from game as well
                break;

            default:
                break;
        }

        if(screenchanged)
            SDL_Flip(screen);
    }

    SDL_FreeSurface(btns);

    //returns if times were reset, or there is a new record holder. config must be saved in these cases
    return result || newrecord;
}

/* input_text(...) function ********************************************************************************

Description:    handles a textbox on screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    TTF_Font        *font       (font used in textbox)
                    Point           SP          (starting point of textbox)
                    Size            Dim         (dimensions)
                    bool            bnw         (drawn in black and white, or not)
Returns:        input finishes at enter, returns *text as result, as parameter as well
Source:         this function was copied from http://infoc.eet.bme.hu and modified to fit to my purposes.
********************************************************************************************************* */

void input_text(SDL_Surface *screen, TTF_Font *font, char *text, Point SP, Size Dim, bool bnw)
{   //this function handles textbox events
    Uint16 dest[18];
    SDL_Event event;
    int length;
    SDL_EnableUNICODE(1);

    length = 0;
    dest[length] = 0x0000;   /* lezaro 0 */

    bool enter = false;
    while (!enter)
    {
        /* szoveg kirajzolasa */
        boxColor(screen, SP.x, SP.y, SP.x+Dim.x, SP.y+Dim.y, C_GREY25);
        DrawLabel(screen, font, SP, dest, bnw);
        SDL_UpdateRect(screen, SP.x, SP.y, Dim.x, Dim.y);

        SDL_WaitEvent(&event);
        switch (event.type)
        {
            case SDL_KEYDOWN:
                switch (event.key.keysym.unicode)
                {
                    case 0x0000:
                        /* nincs neki megfelelo karakter (pl. shift gomb) */
                        break;
                    case '\r':
                    case '\n':
                        /* enter: bevitel vege */
                        if(length > 0)
                            enter = true;
                        break;
                    case '\b':
                        /* backspace: torles visszafele, ha van karakter */
                        if (length>0)
                            dest[--length] = 0x0000;
                        break;
                    default:
                        if(length < 17 && event.key.keysym.unicode <= 0x007F)
                        {
                            /* karakter: tombbe vele, plusz lezaro nulla */
                            dest[length++] = event.key.keysym.unicode;
                            dest[length] = 0x0000;
                        }
                        break;
                }
                break;
        }
        SDL_Flip(screen);
    }

    unicode_2_utf8(dest, text);
}
