#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <SDL/SDL_keysym.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_framerate.h>
#include "GameFuncs.h"
#include "Functions.h"
#include "CUsbJoystickSetup.h"
#include "CWorldParts.h"
#include "Common.h"
#include "Defines.h"
#include "Game.h"
#include "SetupUSBJoysticks.h"
#include "Game.h"
#include "LevelEditor.h"
#include "LevelEditorMenu.h"
#include "StageSelect.h"
#include "Credits.h"
#include "Titlescreen.h"

using namespace std;

int main(int argc, char **argv)
{
	srand((int) time(NULL));
	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO ) == 0)
	{
		bool fullscreen = false;
		for (int i=0; i < argc; i++)
		{
			if(strcasecmp(argv[i], "-f") == 0)
				fullscreen = true;
			if(strcasecmp(argv[i], "-s2") == 0)
			{
				WINDOW_WIDTH = 640 * 2;
				WINDOW_HEIGHT = 360 * 2;
			}

			if(strcasecmp(argv[i], "-s3") == 0)
			{
				WINDOW_WIDTH = 640 * 3;
				WINDOW_HEIGHT = 360 * 3;
			}

			if(strcasecmp(argv[i], "-s4") == 0)
			{
				WINDOW_WIDTH = 640 * 4;
				WINDOW_HEIGHT = 360 * 4;
			}

			if(strcasecmp(argv[i], "-s5") == 0)
			{
				WINDOW_WIDTH = 640 * 5;
				WINDOW_HEIGHT = 360 * 5;
			}
		}
		printf("SDL Succesfully initialized\n");
		Uint32 flags = SDL_SWSURFACE;
		if(fullscreen)
			flags |= SDL_FULLSCREEN;
        Screen = SDL_SetVideoMode( WINDOW_WIDTH, WINDOW_HEIGHT,16, flags);
		if(Screen)
		{
			Icon = IMG_Load("./graphics/sokoban.png");
        	SDL_WM_SetIcon(Icon,NULL);
			SDL_WM_SetCaption( "Sokoban GP2X - Created By Willems Davy", NULL );
		    Buffer = SDL_CreateRGBSurface(SDL_SWSURFACE,ORIG_WINDOW_WIDTH,ORIG_WINDOW_HEIGHT,SCREEN_BPP,Screen->format->Rmask,Screen->format->Gmask,Screen->format->Bmask,Screen->format->Amask);
			printf("Succesfully Set %dx%dx%d\n",WINDOW_WIDTH,WINDOW_HEIGHT,SCREEN_BPP);
			SDL_ShowCursor(SDL_DISABLE);
				if (Mix_OpenAudio(22050,AUDIO_S16,MIX_DEFAULT_CHANNELS,1024) < 0)
				{
					GlobalSoundEnabled = false;
					printf("Failed to initialise sound!\n");
					printf("%s\n",Mix_GetError());
				}
				else
				{
			 		printf("Audio Succesfully initialised!\n");
				}
				if (TTF_Init() == 0)
				{
					printf("Succesfully initialized TTF\n");
					font = TTF_OpenFont("./fonts/font.ttf",17*UI_HEIGHT_SCALE);
					BigFont = TTF_OpenFont("./fonts/font.ttf",20*UI_HEIGHT_SCALE);
					MonoFont = TTF_OpenFont("./fonts/font1.ttf",17*UI_HEIGHT_SCALE);
					if (font && BigFont && MonoFont)
					{
						printf("Succesfully Loaded fonts\n");
						JoystickSetup = new CUsbJoystickSetup();
                        LoadJoystickSettings();
                        SDL_initFramerate(&Fpsman);
                        SDL_setFramerate(&Fpsman,FPS);
						Mix_HookMusicFinished(MusicFinished);
						TTF_SetFontStyle(font,TTF_STYLE_NORMAL);
						SearchForLevelPacks();
						SearchForMusic();
						LoadSounds();
						LoadGraphics();
						LoadSettings();
						while (GameState != GSQuit)
						{
							switch(GameState)
							{
								case GSIntro :
									break;
								case GSTitleScreen :
									TitleScreen();
									break;
								case GSCredits :
									Credits();
									break;
								case GSGame :
									Game();
									break;
								case GSStageClear:
									break;
								case GSStageSelect:
									StageSelect();
									break;
								case GSLevelEditor:
									LevelEditor();
									break;
								case GSLevelEditorMenu:
									LevelEditorMode = true;
									LevelEditorMenu();
									break;
							}
						}
						SaveSettings();
						UnLoadGraphics();
						UnloadSounds();
						UnloadMusic();
						TTF_CloseFont(font);
						TTF_CloseFont(BigFont);
						TTF_CloseFont(MonoFont);
						MonoFont=NULL;
				    	font=NULL;
				    	BigFont=NULL;
                        delete JoystickSetup;
					}
					else
					{
						printf("Failed to Load fonts\n");
					}
					TTF_Quit();
				}
				else
				{
					printf("Failed to initialize TTF\n");
				}
				SDL_FreeSurface(Screen);
				Screen=NULL;
				SDL_FreeSurface(Icon);
				Icon=NULL;
				Mix_CloseAudio();

		}
		else
		{
			printf("Failed to Set Videomode %dx%dx%d\n",WINDOW_WIDTH, WINDOW_HEIGHT, SCREEN_BPP);
		}
		SDL_Quit();
	}
	else
	{
		printf("Couldn't initialise SDL!\n");
	}
	WorldParts.RemoveAll();
	return 0;

}
