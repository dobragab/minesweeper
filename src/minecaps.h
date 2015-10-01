#include <stdbool.h>
#include <SDL.h>
#ifndef MINECAPS_H_INCLUDED
#define MINECAPS_H_INCLUDED

int Cap(int, int, int);
bool CapR(int*, int, int);
void debugmalloc(void*, char*);
void unicode_2_utf8(Uint16 const*, char*);
void utf8_2_unicode(char const*, Uint16*);

#endif // MINECAPS_H_INCLUDED
