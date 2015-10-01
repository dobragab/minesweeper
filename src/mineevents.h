#include "main.h"
#ifndef MINEEVENTS_H_INCLUDED
#define MINEEVENTS_H_INCLUDED

void NextState(State*, int*, bool);
void Uncover(unsigned, unsigned, Cell**, Point, SDL_Surface*, SDL_Surface*, Point*, unsigned*, int*, bool);
void PushCells(bool, BtnState, unsigned, unsigned, Cell**, Point, SDL_Surface*, SDL_Surface*, SDL_Surface*, Point*, unsigned*, int*, bool);
Point GetClickedCell(Uint16, Uint16, Point);
int GetClickedButton(Uint16, Uint16, Button*, unsigned);
bool IsInDim(Point, Size);
Uint32 timer(Uint32, void*);
void GenerateMines(Cell**, Config, Point);

#endif // MINEEVENTS_H_INCLUDED
