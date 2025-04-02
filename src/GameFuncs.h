#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

char* assetPath(const char* assetSubPath);
void logMessage(SDL_PRINTF_FORMAT_STRING const char *fmt, ...);
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
void printTitleInfo();	
void WriteText(TTF_Font* FontIn,const char* Tekst,int NrOfChars,int X,int Y,int YSpacing,SDL_Color ColorIn,bool Centered);
bool FileExists(const char * FileName);
char chr(int ascii);
int ord(char chr);
#endif