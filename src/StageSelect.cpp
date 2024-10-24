#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include "StageSelect.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"

void StageSelect()
{
    CInput *Input = new CInput(InputDelay);
	SDL_Event Event;
	SDL_PollEvent(&Event);
	int Teller;
	char *FileName = new char[FILENAME_MAX];
	char Tekst[300];
	char Tekst1[300];
	SelectedMusic = 0;
	if(MusicCount > 0)
		if (! Mix_PlayingMusic())
			if(GlobalSoundEnabled)
			{
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
	if (SelectedLevel > 0)
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName, SelectedLevel);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
		WorldParts.Load(FileName, true);

	}
	else
		WorldParts.RemoveAll();
	while (GameState == GSStageSelect)
	{
		if (ReloadMusic)
		{
			ReloadMusic=false;
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
		SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
		WorldParts.Move();
		WorldParts.Draw(Buffer);
		boxRGBA(Buffer,0,0,320*UI_WIDTH_SCALE,13*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Buffer,0,-1,320*UI_WIDTH_SCALE,13*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		if(LevelEditorMode)
		{
			if (SelectedLevel == 0)
				sprintf(Tekst,"Level Pack: %s -> %d Levels - (A) Create New Level",LevelPackName,InstalledLevels);
			else
				sprintf(Tekst,"Level Pack: %s Level:%d/%d - (A) Edit Level (X) Delete Level",LevelPackName,SelectedLevel,InstalledLevels);
		}
		else
		{
			if(SelectedLevel <= UnlockedLevels)
				sprintf(Tekst,"Level Pack: %s Level:%d/%d - (A) Play Level",LevelPackName,SelectedLevel,InstalledLevels);
			else
				sprintf(Tekst,"Level Pack: %s Level:%d/%d - Level is locked!",LevelPackName,SelectedLevel,InstalledLevels);
		}
		WriteText(Buffer,font,Tekst,strlen(Tekst),2*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE,0,MenuTextColor);

        Input->Update();

        if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;


                    if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_Y)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_Y)]))
                    {
                        if(LevelEditorMode)
						{
							SelectedLevel = 0;
							WorldParts.RemoveAll();
							LevelHasChanged = false;
							GameState = GSLevelEditor;
						}
						Input->Delay();
                    }

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_B)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_B)]))
					{
						if(LevelEditorMode)
							GameState= GSLevelEditorMenu;
						else
							GameState= GSTitleScreen;
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_BACK],0);
						WorldParts.RemoveAll();
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_X)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_X)]))
					{
						if(LevelEditorMode && (SelectedLevel > 0))
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							sprintf(Tekst,"Are you sure you want to delete this level:\n%s - Level %d\n\nPress (A) to Delete (X) to Cancel",LevelPackName,SelectedLevel);
							if (AskQuestion(Tekst))
							{
								sprintf(Tekst,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName, SelectedLevel);
								if(FileExists(Tekst))
								{
									remove(Tekst);
									sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
									//only swap levels if we did not edit a level that also exists as a default level
									if(!FileExists(FileName))
									{
										for(Teller=SelectedLevel;Teller<InstalledLevels;Teller++)
										{
											sprintf(Tekst,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName, Teller+1);
											sprintf(Tekst1,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,Teller);                      
											rename(Tekst,Tekst1);
										}
									}
									FindLevels();
									if (SelectedLevel > InstalledLevels)
										SelectedLevel = InstalledLevels;
									if (SelectedLevel==0)
										WorldParts.RemoveAll();
									else
									{
										sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
										if(!FileExists(FileName))
											sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
										WorldParts.Load(FileName, true);
									}
								}
							}
							Input->Reset();
						}
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]))
					{
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
						if(LevelEditorMode)
						{
							LevelHasChanged = false;
							GameState = GSLevelEditor;
						}
						else
						{
							if (SelectedLevel <= UnlockedLevels)
								GameState = GSGame;
							else
							{
								sprintf(Tekst,"This Level Hasn't been unlocked yet!\nDo you want to play the last unlocked\nlevel %d/%d\n\nPress (A) to Play (X) to Cancel",UnlockedLevels,InstalledLevels);
								if	(AskQuestion(Tekst))
								{									
									SelectedLevel = UnlockedLevels;
									sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
									if(!FileExists(FileName))
										sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
									WorldParts.Load(FileName, true);
									GameState = GSGame;
								}
								Input->Reset();
							}
						}
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_L)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_L)]))
					{
						if(LevelEditorMode)
						{
							if(SelectedLevel != 0)
							{
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_MENU],0);
						
								SelectedLevel -= 5;								
								if (SelectedLevel <= 0)
								{
									SelectedLevel = 0;
									WorldParts.RemoveAll();
								}
								else
								{
									sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
									if(!FileExists(FileName))
										sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
									WorldParts.Load(FileName, true);
								}
							}
						}
						else
						{
							if(SelectedLevel != 1)
							{								
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								
								SelectedLevel -= 5;	
								if (SelectedLevel < 1)
									SelectedLevel = 1;
								sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
								if(!FileExists(FileName))
									sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
								WorldParts.Load(FileName, true);
							}
						}
                        Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_R)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_R)]))
					{
						if(SelectedLevel != InstalledLevels)
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_MENU],0);
							
							SelectedLevel +=5;
							if (SelectedLevel > InstalledLevels)
								SelectedLevel = InstalledLevels;
							sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
							if(!FileExists(FileName))
								sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
							WorldParts.Load(FileName, true);
						}
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)]))
					{
						if(LevelEditorMode)
						{
							if(SelectedLevel != 0)
							{
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								
								SelectedLevel--;
								if (SelectedLevel <= 0)
								{
									SelectedLevel = 0;
									WorldParts.RemoveAll();
								}
								else
								{
									sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
									if(!FileExists(FileName))
										sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
									WorldParts.Load(FileName,true);
								}
							}
						}
						else
						{
							if(SelectedLevel != 1)
							{
								if (GlobalSoundEnabled)
									Mix_PlayChannel(-1,Sounds[SND_MENU],0);
								
								SelectedLevel--;
								if (SelectedLevel < 1)
									SelectedLevel = 1;
								sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
								if(!FileExists(FileName))
									sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
								WorldParts.Load(FileName,true);
							}
						}
                        Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
					{
						if(SelectedLevel != InstalledLevels)
						{
							SelectedLevel++;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_MENU],0);
							if (SelectedLevel > InstalledLevels)
								SelectedLevel = InstalledLevels;
							sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
							if(!FileExists(FileName))
								sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
							WorldParts.Load(FileName,true);
						}
                        Input->Delay();
					}

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
	delete Input;
	delete[] FileName;
}
