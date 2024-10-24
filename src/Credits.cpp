#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include "Credits.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"


void Credits()
{
    CInput *Input = new CInput(InputDelay);
	SDL_Event Event;
	char *LevelPackCreator = new char[21];
	char FileName[FILENAME_MAX];
	FILE *Fp;
	SDL_PollEvent(&Event);
	char *Tekst = new char[500];
	sprintf(FileName,"%s/.sokoban_levelpacks/%s/credits.dat", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/credits.dat",LevelPackFileName);
	if(InstalledLevelPacksCount > 0)
	{
		Fp = fopen(FileName,"rt");
		if (Fp)
		{
			fscanf(Fp,"[Credits]\nCreator='%[^']'\n",LevelPackCreator);
			fclose(Fp);
			sprintf(Tekst,"Sokoban GP2X was created by\nWillems Davy - Willems Soft 2006-2024.\nHttps://joyrider3774.itch.io\n\nLevelpack %s was created\nby %s.",LevelPackName,LevelPackCreator);
		}
		else
			sprintf(Tekst,"Sokoban GP2X was created by\nWillems Davy - Willems Soft 2006-2024.\nHttps://joyrider3774.itch.io\n\nLevelpack %s was created\nby unknown person.",LevelPackName);
	}
	else
		sprintf(Tekst,"Sokoban GP2X was created by\nWillems Davy - Willems Soft 2006-2024\nHttps://joyrider3774.itch.io");
	while (GameState == GSCredits)
	{
		if (ReloadMusic)
		{
			ReloadMusic=false;
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
		SDL_BlitSurface(IMGTitleScreen,NULL,Buffer,NULL);

        Input->Update();

        if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;


        if (Input->Ready() && ((Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_X)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_Y)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_B)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_START)] ) ||
			(Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_X)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_Y)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_B)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_START)] )))
        {
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
		    GameState = GSTitleScreen;
		}

		boxRGBA(Buffer,60*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,260*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Buffer,60*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,260*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		rectangleRGBA(Buffer,61*UI_WIDTH_SCALE,81.5*UI_HEIGHT_SCALE,259*UI_WIDTH_SCALE,159*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		WriteText(Buffer,font,Tekst,strlen(Tekst),65*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
		SDL_FillRect(Screen,NULL,SDL_MapRGB(Screen->format,0,0,0));
        if ((WINDOW_WIDTH != ORIG_WINDOW_WIDTH) || (WINDOW_HEIGHT != ORIG_WINDOW_HEIGHT))
		{
			SDL_Surface *ScreenBufferZoom = zoomSurface(Buffer,(double)WINDOW_WIDTH / ORIG_WINDOW_WIDTH,(double)WINDOW_HEIGHT / ORIG_WINDOW_HEIGHT,0);
			SDL_BlitSurface(ScreenBufferZoom,NULL,Screen,NULL);
			SDL_FreeSurface(ScreenBufferZoom);
		}
		else
		{
			SDL_BlitSurface(Buffer, NULL, Screen, NULL);
		}
        SDL_Flip(Screen);
        SDL_framerateDelay(&Fpsman);
	}
	delete[] Tekst;
	delete[] LevelPackCreator;
	delete Input;
}
