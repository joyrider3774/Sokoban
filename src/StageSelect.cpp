#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "StageSelect.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"

void StageSelect()
{
    Input->SetInputDelay(InputDelay);
	SDL_Event Event;
	SDL_PollEvent(&Event);
	int Teller;
	char *FileName = new char[FILENAME_MAX];
	char *TmpPath;
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
		sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
		if(!FileExists(FileName))
		{
			sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
			if(!FileExists(FileName))
			{
				TmpPath = assetPath("levelpacks");
				sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
				SDL_free(TmpPath);
			}
		}
		if(FileExists(FileName))
			WorldParts.Load(FileName, true);
		else
			WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
		
	}
	else
		WorldParts.RemoveAll();
	while (GameState == GSStageSelect)
	{
        frameticks = SDL_GetPerformanceCounter();
        SDL_SetRenderTarget(Renderer, Buffer);
		if (ReloadMusic)
		{
			ReloadMusic=false;
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
		SDL_RenderTexture(Renderer, IMGBackground,NULL,NULL);
		SDL_FRect Rect;
		if (SelectedLevel > 0)
		{
			WorldParts.Move();
			WorldParts.DrawFloor(WorldParts.Player);
			WorldParts.Draw();
			if((InstalledLevels > 0))
			{
				
				Rect.x = -1.0f;
				Rect.y = 0.0f;
				Rect.w = (float)ORIG_WINDOW_WIDTH+1;
				Rect.h = 13.0f*UI_HEIGHT_SCALE;
				SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
				SDL_RenderFillRect(Renderer, &Rect);
				SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
				SDL_RenderRect(Renderer, &Rect);
				if(WorldParts.isLevelPackFileLevel)
				{
					if ((strlen(LevelPackFile->LevelsMeta[SelectedLevel-1].title) > 0) || (strlen(LevelPackFile->LevelsMeta[SelectedLevel-1].author) > 0))
					{
						if(strlen(LevelPackFile->LevelsMeta[SelectedLevel-1].author) > 0)
							sprintf(Tekst, "%s by %s", LevelPackFile->LevelsMeta[SelectedLevel-1].title, LevelPackFile->LevelsMeta[SelectedLevel-1].author);
						else
							sprintf(Tekst, "%s by %s", LevelPackFile->LevelsMeta[SelectedLevel-1].title, LevelPackFile->author);
						int w;
						TTF_GetStringSize(font, Tekst, strlen(Tekst), &w, NULL);
						WriteText(font,Tekst,strlen(Tekst),(ORIG_WINDOW_WIDTH - w) / 2,ORIG_WINDOW_HEIGHT- 11*UI_HEIGHT_SCALE,0,MenuTextColor, false);
					}					
				}
			}
		}
		Rect.x = -1.0f;
		Rect.y = 0.0f;
		Rect.w = (float)ORIG_WINDOW_WIDTH+1;
		Rect.h = 13.0f*UI_HEIGHT_SCALE;
		SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
		SDL_RenderFillRect(Renderer, &Rect);
		SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
		SDL_RenderRect(Renderer, &Rect);
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
		WriteText(font,Tekst,strlen(Tekst),2*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE,0,MenuTextColor,false);

        Input->Update();

        if(Input->SpecialsHeld(SPECIAL_QUIT_EV))
            GameState = GSQuit;

        if (Input->Ready() && ((Input->KeyboardHeld(SDLK_LALT) || Input->KeyboardHeld(SDLK_RALT)) && Input->KeyboardHeld(SDLK_RETURN)))
		{
			fullScreen = !fullScreen;
			SDL_SetWindowFullscreen(SdlWindow, fullScreen);
			Input->Delay();
			continue;
		}

                    if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_Y)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_Y))))
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

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_B))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_B))))
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

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_X))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_X))))
					{
						if(LevelEditorMode && (SelectedLevel > 0))
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							sprintf(Tekst,"Are you sure you want to delete this level:\n%s - Level %d\n\nPress (A) to Delete (X) to Cancel",LevelPackName,SelectedLevel);
							if (AskQuestion(Tekst))
							{
								sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
								//to edit default levels
								//sprintf(Tekst,"./levelpacks/%s/level%d.lev",LevelPackName,SelectedLevel);
								if(FileExists(Tekst))
								{
									remove(Tekst);
									TmpPath = assetPath("levelpacks");
									sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
									SDL_free(TmpPath);
									//only swap levels if we did not edit a level that also exists as a default level
									if(!FileExists(FileName) && (SelectedLevel > LevelPackFile->LevelCount))
									{
										for(Teller=SelectedLevel;Teller<InstalledLevels;Teller++)
										{
											sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller+1);
											sprintf(Tekst1,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName,Teller);                      
											rename(Tekst,Tekst1);
										}
									}
									//to edit default levels
									// for(Teller=SelectedLevel;Teller<InstalledLevels;Teller++)
									// {
									// 	sprintf(Tekst,"./levelpacks/%s/level%d.lev", LevelPackName, Teller+1);
									// 	sprintf(Tekst1,"./levelpacks/%s/level%d.lev", LevelPackName,Teller);                      
									// 	rename(Tekst,Tekst1);
									// }
									FindLevels();
									if (SelectedLevel > InstalledLevels)
										SelectedLevel = InstalledLevels;
									if (SelectedLevel==0)
										WorldParts.RemoveAll();
									else
									{
										sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
										if(!FileExists(FileName))
										{
											sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
											if(!FileExists(FileName))
											{
												TmpPath = assetPath("levelpacks");
												sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
												SDL_free(TmpPath);
											}
										}
										if(FileExists(FileName))
											WorldParts.Load(FileName, true);
										else
											WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
									}
								}
							}
							Input->Reset();
						}
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))))
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
									sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
									if(!FileExists(FileName))
									{
										sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
										if(!FileExists(FileName))
										{
											TmpPath = assetPath("levelpacks");
											sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
											SDL_free(TmpPath);
										}
									}
									if(FileExists(FileName))
										WorldParts.Load(FileName, true);
									else
										WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
									GameState = GSGame;
								}
								Input->Reset();
							}
						}
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_L))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_L))))
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
									sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
									if(!FileExists(FileName))
									{
										sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
										if(!FileExists(FileName))
										{
											TmpPath = assetPath("levelpacks");
											sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
											SDL_free(TmpPath);
										}
									}
									if(FileExists(FileName))
										WorldParts.Load(FileName, true);
									else
										WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
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
								sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
								if(!FileExists(FileName))
								{
									sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
									if(!FileExists(FileName))
									{
										TmpPath = assetPath("levelpacks");
										sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
										SDL_free(TmpPath);
									}
								}
								if(FileExists(FileName))
									WorldParts.Load(FileName, true);
								else
									WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
							}
						}
                        Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_R))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_R))))
					{
						if(SelectedLevel != InstalledLevels)
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_MENU],0);
							
							SelectedLevel +=5;
							if (SelectedLevel > InstalledLevels)
								SelectedLevel = InstalledLevels;
							sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
							if(!FileExists(FileName))
							{
								sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
								if(!FileExists(FileName))
								{
									TmpPath = assetPath("levelpacks");
									sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
									SDL_free(TmpPath);
								}
							}
							if(FileExists(FileName))
								WorldParts.Load(FileName, true);
							else
								WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
						}
						Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT))))
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
									sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
									if(!FileExists(FileName))
									{
										sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
										if(!FileExists(FileName))
										{
											TmpPath = assetPath("levelpacks");
											sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
											SDL_free(TmpPath);
										}
									}
									if(FileExists(FileName))
										WorldParts.Load(FileName, true);
									else
										WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
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
								sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
								if(!FileExists(FileName))
								{
									sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
									if(!FileExists(FileName))
									{
										TmpPath = assetPath("levelpacks");
										sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
										SDL_free(TmpPath);
									}
								}
								if(FileExists(FileName))
									WorldParts.Load(FileName, true);
								else
									WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
							}
						}
                        Input->Delay();
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_RIGHT))))
					{
						if(SelectedLevel != InstalledLevels)
						{
							SelectedLevel++;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_MENU],0);
							if (SelectedLevel > InstalledLevels)
								SelectedLevel = InstalledLevels;
							sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
							if(!FileExists(FileName))
							{
								sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
								if(!FileExists(FileName))
								{
									TmpPath = assetPath("levelpacks");
									sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
									SDL_free(TmpPath);
								}
							}
							if(FileExists(FileName))
								WorldParts.Load(FileName, true);
							else
								WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);
						}
                        Input->Delay();
					}

		if(showfps)
        {
            char fpsText[100];
            sprintf(fpsText, "FPS: %.2f\n", avgfps);
            SDL_FRect Rect;
			Rect.x = 0.0f;
			Rect.y = 0.0f;
			Rect.w = 100.0f;
			Rect.h = (float)TTF_GetFontHeight(font);
            SDL_SetRenderDrawColor(Renderer, 255,255,255,255);
            SDL_RenderFillRect(Renderer, &Rect);
            SDL_Color col = {0,0,0,255};
            WriteText(font, fpsText, strlen(fpsText), 0, 0, 0, col, false);
        }
        SDL_SetRenderTarget(Renderer, NULL);
        SDL_SetRenderDrawColor(Renderer, 0,0,0,255);
        SDL_RenderClear(Renderer);
        SDL_SetRenderLogicalPresentation(Renderer, ORIG_WINDOW_WIDTH, ORIG_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);        
        SDL_RenderTexture(Renderer, Buffer, NULL, NULL);
        SDL_RenderPresent(Renderer);
        Uint64 frameEndTicks = SDL_GetPerformanceCounter();
        Uint64 FramePerf = frameEndTicks - frameticks;
        frameTime = (double)FramePerf / (double)SDL_GetPerformanceFrequency() * 1000.0f;
        double delay = 1000.0f / FPS - frameTime;
        if (!nodelay && (delay > 0.0f))
            SDL_Delay((Uint32)(delay)); 
		if (showfps)
		{
			if(skipCounter > 0)
			{
				skipCounter--;
				lastfpstime = SDL_GetTicks();
			}
			else
			{
				framecount++;
				if(SDL_GetTicks() - lastfpstime >= 1000)
				{
					for (int i = FPS_SAMPLES-1; i > 0; i--)
						fpsSamples[i] = fpsSamples[i-1];
					fpsSamples[0] = framecount;
					fpsAvgCount++;
					if(fpsAvgCount > FPS_SAMPLES)
						fpsAvgCount = FPS_SAMPLES;
					int fpsSum = 0;
					for (int i = 0; i < fpsAvgCount; i++)
						fpsSum += fpsSamples[i];
					avgfps = (double)fpsSum / (double)fpsAvgCount;
					framecount = 0;
					lastfpstime = SDL_GetTicks();
				}
			}
		}


	}
	delete[] FileName;
}
