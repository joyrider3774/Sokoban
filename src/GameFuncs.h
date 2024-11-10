#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

void SetVolume(const int VolumeIn);
void IncVolume();
void DecVolume();
void MusicFinished();
void LoadSettings();
void SaveSettings();
void SearchForMusic();
void SearchForLevelPacks();
void LoadUnlockData();
void SaveUnlockData();
void PrintForm(char *msg);
bool AskQuestion(char *Msg);
char *GetString(char *NameIn,char *Msg);
void LoadGraphics();
void FindLevels();
void UnloadSounds();
void LoadSounds();
void UnloadMusic();
void UnLoadGraphics();
void LoadJoystickSettings();
void printTitleInfo(SDL_Surface *Surface);	
void WriteText(SDL_Surface* Surface,TTF_Font* FontIn,char* Tekst,int NrOfChars,int X,int Y,int YSpacing,SDL_Color ColorIn);
SDL_Surface* ScaleSurface(SDL_Surface* Surface,int ScaleFactor);
bool FileExists(char * FileName);
char chr(int ascii);
int ord(char chr);
#endif