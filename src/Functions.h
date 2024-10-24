#ifndef FUNCTION_H
#define FUNCTION_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

void WriteText(SDL_Surface* Surface,TTF_Font* FontIn,char* Tekst,int NrOfChars,int X,int Y,int YSpacing,SDL_Color ColorIn);
SDL_Surface* ScaleSurface(SDL_Surface* Surface,int ScaleFactor);
bool FileExists(char * FileName);
void AddUnderScores (char *string);
void RemoveUnderScores (char *string);
char chr(int ascii);
int ord(char chr);
#endif
