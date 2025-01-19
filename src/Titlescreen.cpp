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
	//http://sokobano.de/en/levels.php

	//int total = 0;

	// char FileName[FILENAME_MAX];
	// if (InstalledLevelPacksCount > 0)
	// 	for(SelectedLevelPack = 0; SelectedLevelPack < InstalledLevelPacksCount; SelectedLevelPack++)
	// 	{
	// 		sprintf(FileName,"%s/.sokoban_levelpacks/%s",getenv("HOME") == NULL ? ".": getenv("HOME"), InstalledLevelPacks[SelectedLevelPack]);
	// 		if(!FileExists(FileName))
	// 			sprintf(FileName,"./levelpacks/%s",InstalledLevelPacks[SelectedLevelPack]);
	// 		LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows);									
	// 		//WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
	// 		total += LevelPackFile->LevelCount;
	// 	}
	// printf("total levels:%d\n", total);
    CInput *Input = new CInput(InputDelay);
	int Teller, Selection = 1;
	SDL_Event Event;
	SDL_PollEvent(&Event);
	char *Tekst = new char[300];
	char FileName[FILENAME_MAX];
	if(MusicCount > 0)
		if (! Mix_PlayingMusic())
			if(GlobalSoundEnabled)
			{
				SelectedMusic = 0;
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
	
	if (InstalledLevelPacksCount >0)
	{		
		sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s",LevelPackName);
		LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);	
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
							{
								SelectedLevelPack--;
								if(SelectedLevelPack < 0)
									SelectedLevelPack = InstalledLevelPacksCount - 1;
								sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);								
								sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
								if(!FileExists(FileName))
									sprintf(FileName,"./levelpacks/%s",LevelPackName);
								LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);									
								LoadGraphics();
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								SaveSettings();
							}
                        Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
					{
						if (Selection==3)
							if (InstalledLevelPacksCount > 0)
							{
								SelectedLevelPack++;
								if(SelectedLevelPack > InstalledLevelPacksCount-1)
									SelectedLevelPack = 0;
								sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
								sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
								if(!FileExists(FileName))
									sprintf(FileName,"./levelpacks/%s",LevelPackName);
								LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);
								LoadGraphics();
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								SaveSettings();
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
						if (Selection < 6)
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
									sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
									if(!FileExists(FileName))
										sprintf(FileName,"./levelpacks/%s",LevelPackName);
									LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, false);
								
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
							case 3:
								if (InstalledLevelPacksCount > 0)
								{
									SelectedLevelPack++;
									if(SelectedLevelPack > InstalledLevelPacksCount-1)
										SelectedLevelPack = 0;
									sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
									sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
									if(!FileExists(FileName))
										sprintf(FileName,"./levelpacks/%s",LevelPackName);
									LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);
									LoadGraphics();
									if (GlobalSoundEnabled)
										Mix_PlayChannel(-1,Sounds[SND_MENU],0);
									SaveSettings();
								}		
								break;
							case 4:
								GameState = GSJoystickSetup;
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
								break;
							case 5:
								GameState=GSCredits;
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
								break;
							case 6:
								GameState = GSQuit;
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
								break;
						}
						Input->Delay();
					}

		printTitleInfo(Buffer);


		boxRGBA(Buffer,50*UI_WIDTH_SCALE,70*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,170*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Buffer,50*UI_WIDTH_SCALE,70*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,170*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		rectangleRGBA(Buffer,51*UI_WIDTH_SCALE,71.5*UI_HEIGHT_SCALE,269*UI_WIDTH_SCALE,169*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		sprintf(Tekst,"Play Selected LevelPack\nLevel Editor\n<%s>\nControls\nCredits\nQuit",LevelPackName);
		WriteText(Buffer,BigFont,Tekst,strlen(Tekst),80*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		WriteText(Buffer,BigFont,Tekst,strlen(Tekst),55*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
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
	delete Input;
}
