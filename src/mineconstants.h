#ifndef MINECONSTANTS_H_INCLUDED
#define MINECONSTANTS_H_INCLUDED

enum // size constants
{
    CELL_SIZE =   16,   //px size of one cell (must be square)
    SMILEY_SIZE = 26,   //px size of a smiley (must be square)
    DIGIT_X =     13,   //px width  of one digit
    DIGIT_Y =     23,   //px height of one digit
};


#define C_WHITE  0xFFFFFFFF
#define C_GREY50 0x808080FF
#define C_GREY25 0xC0C0C0FF
#define C_BLACK  0x000000FF
/*
enum // color constants
{
    C_WHITE =  0xFFFFFFFF,  //white color, for borders
    C_GREY50 = 0x808080FF,  //dark grey color, used for borders if colored
    C_GREY25 = 0xC0C0C0FF,  //light grey color, used for backgrounds
    C_BLACK =  0x000000FF,  //black color, used for borders if B&W
};
*/
enum // widths of borders
{
    BORDER_DIGITS = 1,      //border around digits
    BORDER_HEADER = 2,      //border around "header" (includes digits, and smiley)
    BORDER_CELLS  = 3,      //border around mine field
    BORDER_SMILEY = 4,      //blank "border" between border of header, and digits
    BORDER_WINDOW = 6,      //blank "border" around window
};

enum // SDL_USEREVENT constants
{
    SDL_NEWGAME =   SDL_USEREVENT,      //these are obvious, description at main event loop
    SDL_TICK =      SDL_USEREVENT+1,
    SDL_WON =       SDL_USEREVENT+2,
    SDL_LOST =      SDL_USEREVENT+3,
    SDL_UNCOVER =   SDL_USEREVENT+4,
    SDL_INIT =      SDL_USEREVENT+5,
    oMark =         SDL_USEREVENT+6,
};

enum // limits of width, height and mines
{
    MIN_WIDTH = 9,
    MIN_HEIGHT = 9,
    MIN_MINES = 10,

    MAX_WIDTH = 200,
    MAX_HEIGHT = 100,
};

enum // sets slow motion of uncovering
{
    SLOW_MO_TOGGLE = 1,
    SLOW_MO_MS = 0,
    UNCOVER_LOG = 1,
};

#endif // MINECONSTANTS_H_INCLUDED
