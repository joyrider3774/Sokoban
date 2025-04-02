#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "CUsbJoystickSetup.h"
#include "CInput.h"

#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CUsbJoystickSetup.h"
#include "CLevelPackFile.h"

SDL_Texture *IMGBackground=NULL,*IMGFloor=NULL,*IMGPlayer=NULL,*IMGBox=NULL,*IMGSpot=NULL,*IMGEmpty=NULL,*IMGWall=NULL,*IMGTitleScreen=NULL;
SDL_Texture* Screen,*Buffer,*Icon;
TTF_Font* font,* BigFont,*MonoFont, *RobotoBig, *RobotoMedium;
SDL_Joystick *Joystick;
int GameState = GSTitleScreen;
int MusicCount=0,SelectedMusic=0,InstalledLevelPacksCount=0,InstalledLevels=0,SelectedLevel=0,SelectedLevelPack=0,UnlockedLevels=1;
bool GlobalSoundEnabled = true,LevelEditorMode=false,LevelHasChanged=false,StageReload=false,ReloadMusic=false;
SDL_Color MenuBoxColor = {132,155,189,255}, MenuBoxBorderColor = {82,117,156,255}, MenuTextColor = {33,75,123,0};
CWorldParts WorldParts;
Uint32 NextTime=0;
CLevelPackFile *LevelPackFile;
int Volume = 100;
char LevelPackName[MaxLevelPackNameLength]= "";
char InstalledLevelPacks[MaxLevelPacks][MaxLevelPackNameLength];
Mix_Music *Music[MaxMusicFiles];
Mix_Chunk *Sounds[NrOfSounds];
CUsbJoystickSetup *JoystickSetup;
bool isCustomnTitleScreen;
bool disableJoysticks = false;
bool nodelay = false;
int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 360;
SDL_Window *SdlWindow = NULL;
SDL_Renderer *Renderer = NULL;
Uint64 frameticks = 0, lastfpstime = 0;
double frameTime = 0, avgfps = 0;
Uint32 framecount = 0;
double fpsSamples[FPS_SAMPLES];
int fpsAvgCount = 0, skipCounter = 10;
char basePath[FILENAME_MAX];
bool fullScreen = false;
bool showfps = false;
CInput *Input = NULL;