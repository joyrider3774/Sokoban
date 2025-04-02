#ifndef COMMON_H
#define COMMON_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Defines.h"
#include "CWorldParts.h"
#include "CUsbJoystickSetup.h"
#include "CLevelPackFile.h"
#include "CInput.h"

#define PIXELFORMAT SDL_PIXELFORMAT_RGBA32
#define FPS_SAMPLES 5

extern SDL_Texture *IMGBackground,*IMGFloor,*IMGPlayer,*IMGBox,*IMGSpot,*IMGEmpty,*IMGWall,*IMGTitleScreen;
extern SDL_Texture* Screen,*Buffer,*Icon;
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
extern bool disableJoysticks;;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern bool showfps;
extern bool nodelay;
extern SDL_Window *SdlWindow;
extern SDL_Renderer *Renderer;
extern Uint64 frameticks, lastfpstime;
extern double frameTime, avgfps;
extern Uint32 framecount;
extern double fpsSamples[FPS_SAMPLES];
extern int fpsAvgCount, skipCounter;
extern char basePath[FILENAME_MAX];
extern bool fullScreen;
extern CInput *Input;
#endif