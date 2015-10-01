#include "main.h"

/* DefaultCells(...) function ******************************************************************************

Description:    draws default empty cells on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *cells      (source image)
                    Point           SP          (starting point of cells)
                    Size            Dim         (dimensions of cells)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DefaultCells(SDL_Surface *screen, SDL_Surface *cells, Point SP, Size Dim, bool bnw)
{
    SDL_Rect src_area = { 0, bnw * CELL_SIZE, CELL_SIZE, CELL_SIZE };

    int i, j;
    for(i=0; i < Dim.x; i++)
        for(j=0; j < Dim.y; j++)
        {
            SDL_Rect dest_position = { SP.x + i*CELL_SIZE, SP.y + j*CELL_SIZE, 0, 0 };
            SDL_BlitSurface(cells, &src_area, screen, &dest_position);
        }
}

/* DrawOld(...) function ***********************************************************************************

Description:    draws current state of Cells on screen (UNUSED)
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *cells      (source image)
                    SDL_Surface     *numbers    (source image)
                    Cell            **Cells     (Cells 2D array)
                    Point           SP          (starting point of cells)
                    Size            Dim         (dimensions of cells)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
NOTE:           NOT USED
********************************************************************************************************* */
void DrawOld(SDL_Surface *screen, SDL_Surface *cells, SDL_Surface *numbers, Cell **Cells, Point SP, Size Dim, bool bnw)
{
    int i, j;
    for(i=0; i < Dim.x; i++)
        for(j=0; j < Dim.y; j++)
        {
            if(!Cells[i][j].clickable)
                DrawNumber(screen, numbers, (Point){i,j}, SP, Cells[i][j].preset, bnw);
            else
                DrawCell(screen, cells, (Point){i,j}, SP, Cells[i][j].state, bnw);
        }
}

/* DefaultBorders(...) function ****************************************************************************

Description:    draws default borders of game window on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    Size            WindowSize  (size of the window)
                    Size            Dim         (dimensions of cells)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DefaultBorders(SDL_Surface *screen, Size WindowSize, Size Dim, bool bnw)
{
    Point szegelyKP;
    szegelyKP.x = BORDER_WINDOW;
    szegelyKP.y = BORDER_WINDOW;

    Size szegelyDim;
    szegelyDim.x = WindowSize.x - 2*BORDER_WINDOW;
    szegelyDim.y = 2*(BORDER_SMILEY + BORDER_HEADER + BORDER_DIGITS) + DIGIT_Y;

    Point DigitBorderSP;
    DigitBorderSP.x = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;
    DigitBorderSP.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;

    Point DigitBorderSP2;
    DigitBorderSP2.x = WindowSize.x - BORDER_WINDOW - BORDER_HEADER - BORDER_SMILEY - 2*BORDER_DIGITS - 3*DIGIT_X;
    DigitBorderSP2.y = BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER;

    Size DigitBorderDim;
    DigitBorderDim.x = 3*DIGIT_X + 2*BORDER_DIGITS;
    DigitBorderDim.y =   DIGIT_Y + 2*BORDER_DIGITS;

    Point CellBorderSP;
    CellBorderSP.x = BORDER_WINDOW;
    CellBorderSP.y = DIGIT_Y + 2*(BORDER_WINDOW + BORDER_SMILEY + BORDER_HEADER + BORDER_DIGITS);

    Size CellBorderDim;
    CellBorderDim.x = 2*BORDER_CELLS + Dim.x * CELL_SIZE;
    CellBorderDim.y = 2*BORDER_CELLS + Dim.y * CELL_SIZE;

    DrawBorder(screen, szegelyKP, szegelyDim, BORDER_HEADER, false, bnw);
    DrawBorder(screen, DigitBorderSP, DigitBorderDim, BORDER_DIGITS, true, bnw);
    DrawBorder(screen, DigitBorderSP2, DigitBorderDim, BORDER_DIGITS, true, bnw);
    DrawBorder(screen, CellBorderSP, CellBorderDim, BORDER_CELLS, false, bnw);
}

/* DrawBorder(...) function ********************************************************************************

Description:    draws one border on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    Point           SP          (starting point of border)
                    Size            Dim         (dimensions of border)
                    unsigned        width       (width of border)
                    bool            reversed    (true if sinks into, false if emerges from environment)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
NOTE:           DIMENSIONS ARE OUTER DIMENSIONS
********************************************************************************************************* */
void DrawBorder(SDL_Surface *screen, Point SP, Size Dim, unsigned width, bool reversed, bool bnw)
{   //Vigyazat! A dimenziokban a keret is benne foglaltatik!

    Dim.x--;
    Dim.y--;
    width--;
    const int DARK = bnw ? C_BLACK : C_GREY50;

    Sint16 points1x[6] = { SP.x, SP.x+Dim.x, SP.x+Dim.x-width, SP.x+width,   SP.x+width,       SP.x };
    Sint16 points1y[6] = { SP.y, SP.y,       SP.y+width+1,     SP.y+width+1, SP.y+Dim.y-width, SP.y+Dim.y };

    Sint16 points2x[6] = { SP.x+Dim.x, SP.x,       SP.x+width,       SP.x-width+Dim.x, SP.x-width+Dim.x, SP.x+Dim.x};
    Sint16 points2y[6] = { SP.y+Dim.y, SP.y+Dim.y, SP.y+Dim.y-width, SP.y+Dim.y-width, SP.y+width,       SP.y};

    filledPolygonColor(screen, points1x, points1y, 6, reversed ? C_WHITE : DARK);
    filledPolygonColor(screen, points2x, points2y, 6, reversed ? DARK : C_WHITE);

    if(width == 0)
        lineColor(screen, SP.x, SP.y+Dim.y, SP.x+Dim.x, SP.y+Dim.y, reversed ? DARK : C_WHITE);

    lineColor(screen, points1x[1], points1y[1], points1x[2], points1y[2]-1, C_GREY25 );
    lineColor(screen, points1x[4], points1y[4], points1x[5], points1y[5],   C_GREY25 );

}

/* DrawDigits(...) function ********************************************************************************

Description:    draws 3-digit number on screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *digits     (source image)
                    int             number      (the number to display)
                    Point           SP          (starting point of digits)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DrawDigits(SDL_Surface *screen, SDL_Surface *digits, int number, Point SP, bool bnw)
{
    unsigned bcd[3];
    bool neg = false;
    if(number < 0)
    {
        neg = true;
        number = -number;
    }
    int i;
    for(i=2; i >= 0; i--)
    {
        bcd[i] = number % 10;
        number /= 10;
    }
    for(i=0; i < 3; i++)
    {
        SDL_Rect src_area = { bcd[i] * DIGIT_X, bnw * DIGIT_Y, DIGIT_X, DIGIT_Y };
        SDL_Rect dest_position = { SP.x + i*DIGIT_X , SP.y, 0, 0 };

        SDL_BlitSurface(digits, &src_area, screen, &dest_position);
    }
    if(neg)
    {
        SDL_Rect src_area = { 10 * DIGIT_X, bnw * DIGIT_Y, DIGIT_X, DIGIT_Y };
        SDL_Rect dest_position = { SP.x , SP.y, 0, 0 };

        SDL_BlitSurface(digits, &src_area, screen, &dest_position);
    }
}

/* DrawSmiley(...) function ********************************************************************************

Description:    draws a smiley on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *smileys    (source image)
                    SmileyType      type        (type of smiley)
                    Point           SP          (starting point of smiley
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DrawSmiley(SDL_Surface *screen, SDL_Surface *smileys, SmileyType type, Point SP, bool bnw)
{
    SDL_Rect src_area = { type * SMILEY_SIZE, bnw * SMILEY_SIZE, SMILEY_SIZE, SMILEY_SIZE };
    SDL_Rect dest_position = { SP.x , SP.y, 0, 0 };

    SDL_BlitSurface(smileys, &src_area, screen, &dest_position);
}

/* DrawCell(...) function **********************************************************************************

Description:    draws one cell on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *cells      (source image)
                    Point           cell        (coordinates of current cell) 2ND MEANING OF POINT
                    Point           SP          (starting point of cells)
                    Field           type        (type of cell)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DrawCell(SDL_Surface *screen, SDL_Surface *cells, Point cell, Point SP, Field type, bool bnw)
{
    SDL_Rect src_area = { type * CELL_SIZE, bnw * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_Rect dest_position = { SP.x + cell.x*CELL_SIZE, SP.y + cell.y*CELL_SIZE, 0, 0 };

    SDL_BlitSurface(cells, &src_area, screen, &dest_position);
}

/* DrawNumber(...) function ********************************************************************************

Description:    draws one numbered cell on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *numbers    (source image)
                    Point           cell        (coordinates of current cell) 2ND MEANING OF POINT
                    Point           SP          (starting point of cells)
                    Field           type        (type of cell)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DrawNumber(SDL_Surface *screen, SDL_Surface *numbers, Point cell, Point SP, Field type, bool bnw)
{
    SDL_Rect src_area = { type * CELL_SIZE, bnw * CELL_SIZE, CELL_SIZE, CELL_SIZE };
    SDL_Rect dest_position = { SP.x + cell.x*CELL_SIZE, SP.y + cell.y*CELL_SIZE, 0, 0 };

    SDL_BlitSurface(numbers, &src_area, screen, &dest_position);
}

/* DrawButton(...) function ********************************************************************************

Description:    draws one button on the screen
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *btns       (source image)
                    Button          btn         (all data of buttons)
                    BtnState        st          (whether clicked or not)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DrawButton(SDL_Surface *screen, SDL_Surface *btns, Button btn, BtnState st, bool bnw)
{
    SDL_Rect src_area = { btn.type * btn.Dim.x, (st + 2*bnw) * btn.Dim.y, btn.Dim.x, btn.Dim.y };
    SDL_Rect dest_position = { btn.SP.x, btn.SP.y, 0, 0 };

    SDL_BlitSurface(btns, &src_area, screen, &dest_position);
}

/* DrawCounters(...) function ******************************************************************************

Description:    draws all the counters on the screen, used only in Menu
Parameters:         SDL_Surface     *screen     (destination surface)
                    SDL_Surface     *digits     (source image)
                    Config          cfg         (all data of counters)
Returns:        void
********************************************************************************************************* */
void DrawCounters(SDL_Surface *screen, SDL_Surface *digits, Config cfg)
{
    DrawDigits(screen, digits, cfg.Dim.x, WIDTHSP,  cfg.bBnw);
    DrawDigits(screen, digits, cfg.Dim.y, HEIGHTSP, cfg.bBnw);
    DrawDigits(screen, digits, cfg.Mines, MINESSP,  cfg.bBnw);
}

/* DrawLabel(...) function *********************************************************************************

Description:    draws a label on the screen, used only in BestTimes
Parameters:         SDL_Surface     *screen     (destination surface)
                    TTF_Font        *font       (font used in textbox)
                    Point           SP          (starting point of textbox)
                    Uint16          *text       (text to write)
                    bool            bnw         (drawn in black and white, or not)
Returns:        void
********************************************************************************************************* */
void DrawLabel(SDL_Surface *screen, TTF_Font *font, Point SP, Uint16 *text, bool bnw)
{
    SDL_Surface *felirat;
    SDL_Color grey50 = {128, 128, 128};
    SDL_Color black = {0, 0, 0};
    felirat = TTF_RenderUNICODE_Blended(font, text, bnw ? black : grey50);
    SDL_Rect hely;
    hely.x = SP.x;
    hely.y = SP.y;
    SDL_BlitSurface(felirat, NULL, screen, &hely);
    SDL_FreeSurface(felirat);
}

