#include <unistd.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <sys/stat.h>
#include "LevelEditor.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"
#include "CSelector.h"

bool LevelErrorsFound()
{
	int Teller,NumBoxes=0,NumSpots=0,NumPlayer=0,NumFilledSpots=0;
	for (Teller=0;Teller<WorldParts.ItemCount;Teller++)
	{
		if(WorldParts.Items[Teller]->GetType() == IDBox)
		{
			NumBoxes++;
			if (WorldParts.Items[Teller]->GetAnimPhase() == 1)
                NumFilledSpots++;
		}
		if(WorldParts.Items[Teller]->GetType() == IDSpot)
			NumSpots++;
		if(WorldParts.Items[Teller]->GetType() == IDPlayer)
			NumPlayer++;
	}
	if (NumPlayer==0)
	{
		if (GlobalSoundEnabled)
			Mix_PlayChannel(-1,Sounds[SND_ERROR],0);
		PrintForm("Can not save this level because there's\nno player in the level! Please add a Player\nand try again or cancel saving.\n\nPress (A) to continue");
		return true;
	}
	if (NumBoxes < NumSpots)
	{
		if (GlobalSoundEnabled)
			Mix_PlayChannel(-1,Sounds[SND_ERROR],0);
		PrintForm("Can not save this level because the number\nof spots is greater then the number of boxes!\nPlease Add some more boxes and try again\nor cancel saving.\nPress (A) to continue");
		return true;
	}
	// if (NumSpots-NumFilledSpots == 0)
	// {
	// 	if (GlobalSoundEnabled)
	// 		Mix_PlayChannel(-1,Sounds[SND_ERROR],0);
	// 	PrintForm("Can not save this level because there are\nno empty spots in this level! Please Add at least\none empty spot and try again or cancel saving.\n\nPress (A) to continue");
	// 	return true;
	// }
	return false;

}

void LevelEditor()
{
    Input->SetInputDelay(InputDelay >> 1);
	int Teller,Teller2;
	bool ShowPosition=true,AnotherPartFound,SamePartFound,SpotFound;
	char Tekst[200];
	char *FileName = new char[FILENAME_MAX];
	SDL_Event Event;
	SDL_PollEvent(&Event);
	CSelector Selector;
	if(MusicCount > 0)
		if(GlobalSoundEnabled)
			if (! Mix_PlayingMusic())
			{
				SelectedMusic = 0;
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
	if (StageReload)
	{
		sprintf(FileName,"%s/.sokoban_temp.lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
		WorldParts.Load(FileName, true);
		remove(FileName);
		StageReload=false;
	}
	while (GameState == GSLevelEditor)
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

		if (Input->Ready() && ((Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A)))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))))
		{
			SamePartFound = false;
			for(Teller=0;Teller<WorldParts.ItemCount;Teller++)
			{
				if((WorldParts.Items[Teller]->GetPlayFieldX() == Selector.GetPlayFieldX()) &&
					(WorldParts.Items[Teller]->GetPlayFieldY() == Selector.GetPlayFieldY()))
				{
					if(WorldParts.Items[Teller]->GetType() == Selector.GetSelection())
					{
						SamePartFound=true;
					}
					if(Selector.GetSelection() == IDEmpty)
					{
						LevelHasChanged=true;
						break;
					}
				}
			}
			if(Selector.GetSelection() != IDEmpty)
				if(!LevelHasChanged)
					LevelHasChanged=!SamePartFound;
			switch(Selector.GetSelection())
			{
				case IDEmpty:
					for(Teller=0;Teller<WorldParts.ItemCount;Teller++)
					{
						if((WorldParts.Items[Teller]->GetPlayFieldX() == Selector.GetPlayFieldX()) &&
							(WorldParts.Items[Teller]->GetPlayFieldY() == Selector.GetPlayFieldY()))
						{
							if(WorldParts.Items[Teller]->GetType() == IDFloor)
							{
								AnotherPartFound = false;
								SpotFound = false;
								for(Teller2=Teller+1;Teller2<WorldParts.ItemCount;Teller2++)
								{
									if((WorldParts.Items[Teller2]->GetPlayFieldX() == Selector.GetPlayFieldX()) &&
										(WorldParts.Items[Teller2]->GetPlayFieldY() == Selector.GetPlayFieldY()))
										{
											if(WorldParts.Items[Teller2]->GetType() == IDSpot)
												SpotFound = true;
											else
											{
												WorldParts.Remove(WorldParts.Items[Teller2]->GetPlayFieldX(),WorldParts.Items[Teller2]->GetPlayFieldY(),WorldParts.Items[Teller2]->GetType());
												AnotherPartFound = true;
												break;
											}

										}
								}
								if(SpotFound && !AnotherPartFound)
								{
									WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDSpot);
								}
								else
								{
									if (!SpotFound && !AnotherPartFound)
										WorldParts.Remove(WorldParts.Items[Teller]->GetPlayFieldX(),WorldParts.Items[Teller]->GetPlayFieldY(),IDFloor);
								}
								break;
							}
							else
							{
								if(WorldParts.Items[Teller]->GetType() == IDSpot)
								{
									AnotherPartFound = false;
									for(Teller2=Teller+1;Teller2<WorldParts.ItemCount;Teller2++)
									{
										if((WorldParts.Items[Teller2]->GetPlayFieldX() == Selector.GetPlayFieldX()) &&
											(WorldParts.Items[Teller2]->GetPlayFieldY() == Selector.GetPlayFieldY()) &&
											(WorldParts.Items[Teller2]->GetType() != IDFloor))
											{
												WorldParts.Remove(WorldParts.Items[Teller2]->GetPlayFieldX(),WorldParts.Items[Teller2]->GetPlayFieldY(),WorldParts.Items[Teller2]->GetType());
												AnotherPartFound = true;
												break;
											}
									}
									if (!AnotherPartFound)
										WorldParts.Remove(WorldParts.Items[Teller]->GetPlayFieldX(),WorldParts.Items[Teller]->GetPlayFieldY(),IDSpot);
									break;
								}
								else
								{
									if(WorldParts.Items[Teller]->GetType() == IDBox)
									{
										WorldParts.Remove(WorldParts.Items[Teller]->GetPlayFieldX(),WorldParts.Items[Teller]->GetPlayFieldY(),IDBox);
										break;
									}
									else
									{
										if(WorldParts.Items[Teller]->GetType() == IDPlayer)
										{
											WorldParts.Remove(WorldParts.Items[Teller]->GetPlayFieldX(),WorldParts.Items[Teller]->GetPlayFieldY(),IDPlayer);
											break;
										}
										else
										{
											WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY());
											break;
										}
									}
								}
							}
						}
					}
					
					break;
				case IDBox:
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDWall);
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDBox);
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDPlayer);
					WorldParts.Add(new CBox(Selector.GetPlayFieldX(),Selector.GetPlayFieldY()));
					break;
				case IDPlayer:
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDWall);
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDBox);
					for(Teller=0;Teller<WorldParts.ItemCount;Teller++)
					{
						if (WorldParts.Items[Teller]->GetType() == IDPlayer)
							WorldParts.Remove(WorldParts.Items[Teller]->GetPlayFieldX(),WorldParts.Items[Teller]->GetPlayFieldY(),IDPlayer);
					}
					WorldParts.Add(new CPlayer(Selector.GetPlayFieldX(),Selector.GetPlayFieldY()));
					break;
				case IDWall:
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY());
					WorldParts.Add(new CWall(Selector.GetPlayFieldX(),Selector.GetPlayFieldY()));
					break;
				case IDSpot:
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDWall);
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDSpot);
					WorldParts.Add(new CSpot(Selector.GetPlayFieldX(),Selector.GetPlayFieldY()));
					break;
				case IDFloor:
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDFloor);
					WorldParts.Remove(Selector.GetPlayFieldX(),Selector.GetPlayFieldY(),IDWall);
					WorldParts.Add(new CFloor(Selector.GetPlayFieldX(),Selector.GetPlayFieldY()));
					break;
			}
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_START))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_START))))
		{
			if(!LevelErrorsFound())
			{
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				sprintf(FileName,"%s/.sokoban_temp.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
				WorldParts.Save(FileName);
				StageReload = true;
				GameState=GSGame;
			}
			Input->Reset();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT)) || Input->JoystickHeld(0, JoystickSetup->GetButtonValue(BUT_RIGHT))))
		{
			Selector.MoveRight();
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT))))
		{
			Selector.MoveLeft();
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_UP)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_UP))))
		{
			Selector.MoveUp();
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_DOWN)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_DOWN))))
		{
			Selector.MoveDown();
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_L)) || Input->JoystickHeld(0, JoystickSetup->GetButtonValue(BUT_L))))
		{
			Selector.DecSelection();
			Input->Delay();
			SDL_Delay(100);
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_R)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_R))))
		{
			Selector.IncSelection();
			Input->Delay();
			SDL_Delay(100);
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_SELECT)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_SELECT)) ))
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			ShowPosition = !ShowPosition;
			Input->Delay();
			SDL_Delay(100);
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_Y)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_Y))))
		{
			if (WorldParts.ItemCount > 0)
			{
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				if(AskQuestion("You are about to delete all parts\nin this level, are you sure\nyou want to do this?\n\nPress (A) to delete, (X) to cancel"))
				{
					LevelHasChanged = true;
					WorldParts.RemoveAll();
				}
			}
			Input->Reset();
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_X)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_X))))
		{
			LevelHasChanged |= WorldParts.CenterLevel();
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_B)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_B))))
		{
			if (LevelHasChanged)
			{
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				if(AskQuestion("The current level isn't saved yet!\nDo you want to save it now ?\n\nPress (A) to save, (X) to cancel saving"))
				{
					SDL_RenderTexture(Renderer, IMGBackground,NULL,NULL);
					WorldParts.Move();
					WorldParts.Draw();
					Selector.Draw();
					if (ShowPosition)
					{
						sprintf(Tekst,"X: %d - Y: %d",Selector.GetPlayFieldX(),Selector.GetPlayFieldY());
						SDL_FRect Rect;
						Rect.x = ORIG_WINDOW_WIDTH-50*UI_WIDTH_SCALE;
						Rect.y = 0;
						Rect.w = ORIG_WINDOW_WIDTH;
						Rect.h = 13*UI_HEIGHT_SCALE;
						SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
						SDL_RenderFillRect(Renderer, &Rect);
						SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
						SDL_RenderRect(Renderer, &Rect);							
						WriteText(font,Tekst,strlen(Tekst),272*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE,0,MenuTextColor,false);
					}
					if (!LevelErrorsFound())
					{
						sprintf(FileName,"%s/.sokoban_levelpacks",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
#ifdef WIN32
						mkdir(FileName);
#else
						mkdir(FileName,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
						sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
#ifdef WIN32
						mkdir(FileName);
#else
						mkdir(FileName,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
						if (SelectedLevel==0)
							sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName,InstalledLevels+1);
						else
							sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName,SelectedLevel);

						//to edit default levels
						// if (SelectedLevel==0)
						// 	sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackName,InstalledLevels+1);
						// else
						// 	sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackName,SelectedLevel);

						WorldParts.Save(FileName);
						FindLevels();
						if (SelectedLevel==0)
							SelectedLevel = InstalledLevels;
						LevelHasChanged=false;
						GameState = GSStageSelect;									
					}

				}
				else
					GameState = GSStageSelect;							
				Input->Reset();
			}
			else
				GameState = GSStageSelect;
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
			Input->Delay();
		}


		SDL_RenderTexture(Renderer, IMGBackground,NULL,NULL);
		//if(ShowGrid)
        //    SDL_RenderTexture(Renderer, IMGGrid,NULL,NULL);
		WorldParts.Draw();
		Selector.Draw();
		if (ShowPosition)
		{
			sprintf(Tekst,"X: %d - Y: %d",Selector.GetPlayFieldX(),Selector.GetPlayFieldY());
			SDL_FRect Rect;
			Rect.x = ORIG_WINDOW_WIDTH-50*UI_WIDTH_SCALE;
			Rect.y = 0;
			Rect.w = ORIG_WINDOW_WIDTH;
			Rect.h = 13*UI_HEIGHT_SCALE;
			SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
			SDL_RenderFillRect(Renderer, &Rect);
			SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
			SDL_RenderRect(Renderer, &Rect);							
			WriteText(font,Tekst,strlen(Tekst),272*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE,0,MenuTextColor,false);
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
        SDL_SetRenderTarget(Renderer, Buffer);
	}
	delete[] FileName;
}
