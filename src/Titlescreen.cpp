#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include "Titlescreen.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"


void TitleScreen()
{
    CInput *Input = new CInput(InputDelay);
	int Teller, Selection = 1;
	SDL_Event Event;
	SDL_PollEvent(&Event);
	char *Tekst = new char[300];
	if(MusicCount > 0)
		if (! Mix_PlayingMusic())
			if(GlobalSoundEnabled)
			{
				SelectedMusic = 0;
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
	while (GameState == GSTitleScreen)
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

					
					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)]))
					{
						if(Selection==3)
						 	if (InstalledLevelPacksCount > 0)
								if(SelectedLevelPack > 0)
								{
									SelectedLevelPack--;
									sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
									sprintf(LevelPackFileName,"%s",InstalledLevelPacks[SelectedLevelPack]);
									AddUnderScores(LevelPackFileName);
									LoadGraphics();
									if (GlobalSoundEnabled)
										Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								}
                        Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
					{
						if (Selection==3)
							if (InstalledLevelPacksCount > 0)
								if(SelectedLevelPack < InstalledLevelPacksCount-1)
								{
									SelectedLevelPack++;
									sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
									sprintf(LevelPackFileName,"%s",InstalledLevelPacks[SelectedLevelPack]);
									AddUnderScores(LevelPackFileName);
									LoadGraphics();
									if (GlobalSoundEnabled)
										Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								}
                        Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_UP)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_UP)]))
					{
						if (Selection > 1)
						{
							Selection--;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_MENU],0);
						}
						Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_DOWN)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_DOWN)]))
					{
						if (Selection < 5)
						{
							Selection++;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_MENU],0);
						}
						Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]))
					{
						switch(Selection)
						{
							case 1:
								if (InstalledLevelPacksCount >0)
								{
									FindLevels();
								    if(InstalledLevels > 0)
									{										
										LoadUnlockData();
										SelectedLevel=UnlockedLevels;
										LevelEditorMode=false;
										GameState=GSStageSelect;
										if (GlobalSoundEnabled)
											Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
									}
									else
									{
										if (GlobalSoundEnabled)
											Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
										SDL_BlitSurface(IMGTitleScreen,NULL,Buffer,NULL);
										sprintf(Tekst,"There are no levels found in levelpack\n%s\n\nPlease create a level for this level pack\nfirst!",LevelPackName);
										PrintForm(Tekst);
										Input->Reset();
                        			}
								}
								break;
							case 2:
								GameState=GSLevelEditorMenu;
								LevelEditorMode=true;
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
								break;
							case 4:
								GameState=GSCredits;
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
								break;
							case 5:
								GameState = GSQuit;
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
								break;
						}
						Input->Delay();
					}


		boxRGBA(Buffer,60*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,260*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Buffer,60*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,260*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		rectangleRGBA(Buffer,61*UI_WIDTH_SCALE,81.5*UI_HEIGHT_SCALE,259*UI_WIDTH_SCALE,159*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		sprintf(Tekst,"Play Selected LevelPack\nLevel Editor\n<%s>\nCredits\nQuit",LevelPackName);
		WriteText(Buffer,BigFont,Tekst,strlen(Tekst),90*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		WriteText(Buffer,BigFont,Tekst,strlen(Tekst),65*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2,MenuTextColor);
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
}
