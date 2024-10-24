#ifndef COMMON_H
#define COMMON_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_framerate.h>
#include <SDL_mixer.h>
#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CUsbJoystickSetup.h"

extern SDL_Surface *IMGBackground,*IMGFloor,*IMGPlayer,*IMGBox,*IMGSpot,*IMGEmpty,*IMGWall,*IMGTitleScreen;
extern unsigned char HashTable[1004];
extern SDL_Surface* Screen,*Buffer;
extern TTF_Font* font,* BigFont,*MonoFont;
extern SDL_Joystick *Joystick;
extern int GameState;
extern int MusicCount,SelectedMusic,InstalledLevelPacksCount,InstalledLevels,SelectedLevel,SelectedLevelPack,UnlockedLevels;
extern bool TvOutMode,GlobalSoundEnabled,LevelEditorMode,LevelHasChanged,StageReload,ReloadMusic;
extern SDL_Color MenuBoxColor, MenuBoxBorderColor, MenuTextColor;
extern CWorldParts WorldParts;
extern Uint32 NextTime;
extern int Volume;
extern char LevelPackName[21];
extern char LevelPackFileName[21];
extern char InstalledLevelPacks[MaxLevelPacks][21];
extern Mix_Music *Music[MaxMusicFiles];
extern Mix_Chunk *Sounds[NrOfSounds];
extern CUsbJoystickSetup *JoystickSetup;
extern FPSmanager Fpsman;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

#endif