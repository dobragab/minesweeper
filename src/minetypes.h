#include "main.h"
#ifndef MINETYPES_H_INCLUDED
#define MINETYPES_H_INCLUDED

typedef enum // Field enum, in the order of cells.png
{
    virgin, clicked, redflag, questmark, clickedqmark, mine, clickedmine, fakemine
} Field;

typedef enum // State enum, they stand for 3 possible states of a cell.
{   //int values correspond to places in cells.png as well
    empty=0, flag=2, qmark=3
} State;

typedef enum //SmileyType enum, in the order of smileys.png and owls.png
{
    happy, surprised, dead, sunglass, clickedhappy
} SmileyType;

typedef enum //BtnMode enum, 2 values: safe and fast
{
    safe, fast
    /*
    safe button: if you click on it, but you don't release mousebtn on it, the click doesnt take effect
    fast button: if you click on it and hold mousebtn, after some ms it will start repeating click
    */
} BtnMode;

typedef enum //BtnState enum, a btn is pressed or not
{
    normal, pressed
} BtnState;

typedef enum //BtnType enum, for 3 buttons used in main window
{
    Smiley, Settings, Besttimes
} BtnType;

typedef enum //MtnMType enum, for types of buttons in menu, in the order of btns.png
{
    left, right, up, down, checkmark, redx, qmarkk, xqmark, bnw, colored, sound, xsound
} BtnMType;

typedef enum //BtnMName enum, for names of menu buttons
{
    noname,
    WIDTHDOWN, WIDTHUP,
    HEIGHTUP, HEIGHTDOWN,
    MINEMINUS, MINEPLUS,
    BEGINNER, INTERMEDIATE, EXPERT,
    MARKQ, BNW, SOUND,
    OK, CANCEL

} BtnMName;

typedef enum //BtnBType enum, for names of buttons in BestTimes
{
    nonameB,
    RESET, CLOSEBEST
} BtnBType;

typedef enum //Preset enum, for level presets
{
    custom, beginner, intermediate, expert
    /*
    custom: not any of the presets, best times aren't saved for these games
    others: the 3 stock presets
    */
} Preset;

typedef enum //MineMode enum, for the mode of generating mines
{
    ran, badass, lucker
    /*
    lucker: used at stock presets, this means first click does not hit any mines
    badass: when user sets too few mines for a level, badass mode is activated: his first click will
            always hit a mine, anywhere he clicks
    ran: every other cases, only at custom preset
    */
} MineMode;

typedef enum //MenuResult enum, used only for the return value of menu(...) function
{
    original, Small, huge
    /*
    original: no values changed at menu
    small:    some values changed at menu (qmark or sound), so it requires saving config, but reinit.
    huge:     important values changed at menu (B&W, Dim or MinesNo), requires saving and reinit
    */
} MenuResult;

typedef struct //Point struct for coordinates
{
    int x, y;
} Point;

/** IMPORTANT NOTE
Actually the Point structure stands for two meanings, practically both are Points, but in a bit different
meaning. Generally it is the pixel with (x; y) coordinates of the screen. But due to the structure of the
game, it can mean another thing: for example the 6th cell from the left of the mine field in the 3rd row
refers to coordinates (5; 2).
The variables that use this 2nd meaning: C, CNext, D.
The functions that use this 2nd meaning: DrawCell, DrawNumber, GetClickedCell, Uncover, PushCells, GenerateMines.
(There may be further ones I forgot to mention, I will note them in place as well.)
*/

typedef Point Size; //Size struct for sizes and dimensions

typedef struct //Cell struct for cells array in game
{
    State state;    //flag, qmark or nothing
    int preset;     //-1 is mine, others are mines around that cell
    bool clickable; //if clickable or uncovered
} Cell;

typedef struct //Config struct
{
    Size Dim;   //Dimensions
    int Mines;  //number of mines
    bool bQmark, bBnw, bSound;  // boolean values for ?mark, b&w, sound
    Preset prst;
    MineMode mm;
} Config;

//function pointer for fast menu buttons click event
typedef void (*FastClicked) (SDL_Surface*, SDL_Surface*, Config*, unsigned*);

typedef struct //Button struct
{
    unsigned type, name;    //unsigned becasue actually used for numerous types
    Point SP;               //starting point of the button
    Size Dim;               //dimensions of the button
    BtnMode mode;           //mode (safe or fast)
    FastClicked click;      //click event (function) for the button
} Button;

typedef struct //BestTime struct for 1 name and time
{
    unsigned Time;
    char Name[19];  //num of chars can be limited cuz there's no more space on the BestTimes window
} BestTime;

#endif // MINETYPES_H_INCLUDED
