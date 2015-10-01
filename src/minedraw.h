#include "main.h"
#ifndef MINEDRAW_H_INCLUDED
#define MINEDRAW_H_INCLUDED

void DefaultCells(SDL_Surface*, SDL_Surface*, Point, Size, bool);
void DefaultBorders(SDL_Surface*, Size, Size, bool);
void DrawBorder(SDL_Surface*, Point, Size, unsigned, bool, bool);
void DrawDigits(SDL_Surface*, SDL_Surface*, int, Point, bool);
void DrawNumber(SDL_Surface*, SDL_Surface*, Point, Point, Field, bool);
void DrawSmiley(SDL_Surface*, SDL_Surface*, SmileyType, Point, bool);
void DrawCell(SDL_Surface*, SDL_Surface*, Point, Point, Field, bool);
void DrawButton(SDL_Surface*, SDL_Surface*, Button, BtnState, bool);
void DrawCounters(SDL_Surface*, SDL_Surface*, Config);
void DrawLabel(SDL_Surface*, TTF_Font*, Point, Uint16*, bool);

void DrawOld(SDL_Surface*, SDL_Surface*, SDL_Surface*, Cell**, Point, Size, bool);

#endif // MINEDRAW_H_INCLUDED
