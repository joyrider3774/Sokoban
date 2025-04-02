#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_framerate.h>
#include <SDL_mixer.h>
#include "Common.h"
#include "Defines.h"
#include "CWorldParts.h"
#include "CUsbJoystickSetup.h"
#include "CLevelPackFile.h"

SDL_Surface *IMGBackground=NULL,*IMGFloor=NULL,*IMGPlayer=NULL,*IMGBox=NULL,*IMGSpot=NULL,*IMGEmpty=NULL,*IMGWall=NULL,*IMGTitleScreen=NULL;
SDL_Surface* Screen,*Buffer,*Icon;
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
FPSmanager Fpsman;
int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 360;
Uint32 LastFps = 0;
Uint32 FrameCount = 0;
Uint32 FrameTicks = 0;
bool ShowFps = false;
bool noDelay = false;