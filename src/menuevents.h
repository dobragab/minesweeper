#include "main.h"
#ifndef MENUEVENTS_H_INCLUDED
#define MENUEVENTS_H_INCLUDED

void WidthUp    (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void WidthDown  (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void HeightUp   (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void HeightDown (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void MinesUp    (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void MinesDown  (SDL_Surface*, SDL_Surface*, Config*, unsigned*);

void BeginnerSet     (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void IntermediateSet (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void ExpertSet       (SDL_Surface*, SDL_Surface*, Config*, unsigned*);

void QmarkClicked (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void BnwClicked   (SDL_Surface*, SDL_Surface*, Config*, unsigned*);
void SoundClicked (SDL_Surface*, SDL_Surface*, Config*, unsigned*);

#endif // MENUEVENTS_H_INCLUDED
