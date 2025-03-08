#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include <SDL.h>
#include <SDL_ttf.h>

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
void PrintForm(const char *msg);
bool AskQuestion(const char *Msg);
char *GetString(const char *NameIn,const char *Msg);
void LoadGraphics();
void FindLevels();
void UnloadSounds();
void LoadSounds();
void UnloadMusic();
void UnLoadGraphics();
void LoadJoystickSettings();
void printTitleInfo(SDL_Surface *Surface);	
void WriteText(SDL_Surface* Surface,TTF_Font* FontIn,const char* Tekst,int NrOfChars,int X,int Y,int YSpacing,SDL_Color ColorIn);
SDL_Surface* ScaleSurface(SDL_Surface* Surface,int ScaleFactor);
bool FileExists(const char * FileName);
char chr(int ascii);
int ord(char chr);
#endif