#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_gfxPrimitives.h>

#include <stdbool.h>

#include "menu.h"           //the menu module
#include "best.h"           //the best times module
#include "minecaps.h"       //module of general functions and algorythms
#include "mineconfig.h"     //contains all file operations, reading and writing
#include "mineconstants.h"  //includes the constant integers in enums
#include "minecursor.h"
#include "minedraw.h"       //all the drawing functions
#include "mineevents.h"     //event handling parts
#include "minetypes.h"      //data structures and enums i use

#ifdef MYDEBUG
#include "debugmalloc.h"
#endif // MYDEBUG

#endif // MAIN_H_INCLUDED
