#include "main.h"
#include "menuconstants.h"
#include "menuevents.h"

Config Presets[3] = {           //array for presets, will be necessary here.
        { { 9, 9 }, 10 },       //I could have made it an extern variable, I was lazy :D
        { { 16, 16 }, 40 },
        { { 30, 16 }, 99 }  };

/*
These functions are all type FastClicked, they are called at clicking menu buttons.
They are actually obvious, comments not necessary.
*/

void WidthUp    (SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    if(asdcfg->Dim.x < MAX_WIDTH)
    {
        asdcfg->Dim.x++;
        DrawDigits(screen, digits, asdcfg->Dim.x, WIDTHSP, asdcfg->bBnw);
    }
}

void WidthDown  (SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    if(asdcfg->Dim.x > MIN_WIDTH)
    {
        asdcfg->Dim.x--;
        DrawDigits(screen, digits, asdcfg->Dim.x, WIDTHSP, asdcfg->bBnw);
        if(CapR(&asdcfg->Mines, 9, (asdcfg->Dim.x-1) * (asdcfg->Dim.y-1)))
            DrawDigits(screen, digits, asdcfg->Mines, MINESSP, asdcfg->bBnw);
    }
}

void HeightUp   (SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    if(asdcfg->Dim.y < MAX_HEIGHT)
    {
        asdcfg->Dim.y++;
        DrawDigits(screen, digits, asdcfg->Dim.y, HEIGHTSP, asdcfg->bBnw);
    }
}

void HeightDown (SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    if(asdcfg->Dim.y > MIN_HEIGHT)
    {
        asdcfg->Dim.y--;
        DrawDigits(screen, digits, asdcfg->Dim.y, HEIGHTSP, asdcfg->bBnw);
        if(CapR(&asdcfg->Mines, 9, (asdcfg->Dim.x-1) * (asdcfg->Dim.y-1)))
            DrawDigits(screen, digits, asdcfg->Mines, MINESSP, asdcfg->bBnw);
    }
}

void MinesUp    (SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    if(asdcfg->Mines < (asdcfg->Dim.x-1) * (asdcfg->Dim.y-1))
    {
        asdcfg->Mines++;
        DrawDigits(screen, digits, asdcfg->Mines, MINESSP, asdcfg->bBnw);
    }
}

void MinesDown  (SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    if(asdcfg->Mines > MIN_MINES)
    {
        asdcfg->Mines--;
        DrawDigits(screen, digits, asdcfg->Mines, MINESSP, asdcfg->bBnw);
    }
}

void BeginnerSet(SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    CopyPreset(asdcfg, Presets[0]);
    asdcfg->prst = beginner;
    DrawCounters(screen, digits, *asdcfg);
}

void IntermediateSet(SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    CopyPreset(asdcfg, Presets[1]);
    asdcfg->prst = intermediate;
    DrawCounters(screen, digits, *asdcfg);
}

void ExpertSet(SDL_Surface *screen, SDL_Surface *digits, Config *asdcfg, unsigned *unused1)
{
    CopyPreset(asdcfg, Presets[2]);
    asdcfg->prst = expert;
    DrawCounters(screen, digits, *asdcfg);
}

void QmarkClicked(SDL_Surface *screen, SDL_Surface *btns, Config *asdcfg, unsigned *type)
{
    asdcfg->bQmark = !asdcfg->bQmark;
    if(asdcfg->bQmark) *type = qmarkk;
    else               *type = xqmark;
}

void BnwClicked(SDL_Surface *screen, SDL_Surface *btns, Config *asdcfg, unsigned *type)
{
    asdcfg->bBnw = !asdcfg->bBnw;

    SDL_Event ev;
    ev.type = SDL_INIT;
    SDL_PushEvent(&ev);
}

void SoundClicked(SDL_Surface *screen, SDL_Surface *btns, Config *asdcfg, unsigned *type)
{
    asdcfg->bSound = !asdcfg->bSound;
    if(asdcfg->bSound) *type = sound;
    else               *type = xsound;
}

