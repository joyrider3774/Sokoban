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
#include "CLevelPackFile.h"

extern SDL_Surface *IMGBackground,*IMGFloor,*IMGPlayer,*IMGBox,*IMGSpot,*IMGEmpty,*IMGWall,*IMGTitleScreen;
extern SDL_Surface* Screen,*Buffer,*Icon;
extern TTF_Font* font,* BigFont,*MonoFont, *RobotoBig, *RobotoMedium;
extern SDL_Joystick *Joystick;
extern int GameState;
extern int MusicCount,SelectedMusic,InstalledLevelPacksCount,InstalledLevels,SelectedLevel,SelectedLevelPack,UnlockedLevels;
extern bool GlobalSoundEnabled,LevelEditorMode,LevelHasChanged,StageReload,ReloadMusic;
extern SDL_Color MenuBoxColor, MenuBoxBorderColor, MenuTextColor;
extern CWorldParts WorldParts;
extern Uint32 NextTime;
extern CLevelPackFile *LevelPackFile;
extern int Volume;
extern char LevelPackName[100];
extern char InstalledLevelPacks[MaxLevelPacks][100];
extern Mix_Music *Music[MaxMusicFiles];
extern Mix_Chunk *Sounds[NrOfSounds];
extern CUsbJoystickSetup *JoystickSetup;
extern bool isCustomnTitleScreen;
extern FPSmanager Fpsman;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern Uint32 LastFps;
extern Uint32 FrameCount;
extern Uint32 FrameTicks;
extern bool ShowFps;
extern bool noDelay;
#endif