#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <SDL3_image/SDL_image.h>

#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"

bool StageDone()
{
	int Teller,FilledSpots=0,Spots=0;
	for (Teller=0;Teller<WorldParts.ItemCount;Teller++)
		if (WorldParts.Items[Teller]->GetType() == IDBox)
		{
			if (WorldParts.Items[Teller]->GetAnimPhase() == 1)
                FilledSpots++;
		}
		else
            if (WorldParts.Items[Teller]->GetType() == IDSpot)
                Spots++;
    if (FilledSpots >= Spots)
        return true;
    else
        return false;
}

void Game()
{
    Input->SetInputDelay(InputDelay);
	int Teller;
	char Msg[300];
	char FileName[FILENAME_MAX];
	SDL_Event Event;
	SDL_PollEvent(&Event);
	if (GlobalSoundEnabled)
		Mix_HaltMusic();
	if (MusicCount > 1)
 	{
 		SelectedMusic =	1+SDL_rand(MusicCount-1);
		if(GlobalSoundEnabled)
		{
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
	}
	CWorldPart *Player=0;
	for (Teller=0;Teller<WorldParts.ItemCount;Teller++)
	{
		if (WorldParts.Items[Teller]->GetType() == IDPlayer)
		{
			Player = WorldParts.Items[Teller];
			break;
		}
	}
	//should never happen
	if(!Player)
	{
		Player = new CPlayer(0,0);
		WorldParts.Add(Player);
	}

	while (GameState == GSGame)
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

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_B)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_B))) )
		{
			if(!LevelEditorMode)
			{
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_BACK],0);
		
				if (AskQuestion("Are you sure you want to return to the stage selector?\n\nPress (A) to confirm (X) to Cancel"))
				{
					GameState = GSStageSelect;
				}
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				Input->Reset();
			}
		}

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_START))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_START))))
		{
			if(LevelEditorMode)
			{
				if (AskQuestion("Are you sure you want to return to level editor?\n\nPress (A) to confirm (X) to Cancel"))
				{
					GameState=GSLevelEditor;
				}
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				Input->Reset();
			}
		}

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_R))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_R))))
		{
			SelectedMusic++;
			if (SelectedMusic >= MusicCount)
				SelectedMusic = 1;
			if(GlobalSoundEnabled)
			{
				Mix_HaltMusic();
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
			Input->Delay();
		}

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_L))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_L))))
		{
			SelectedMusic--;
			if (SelectedMusic <= 0)
				SelectedMusic = MusicCount-1;

			if(GlobalSoundEnabled)
			{
				Mix_HaltMusic();
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
			Input->Delay();
		}

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_Y))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_Y))))
		{
			SDL_RenderTexture(Renderer, IMGBackground,NULL,NULL);
			WorldParts.Draw();
			if(AskQuestion("You are about to restart this level\nAre you sure you want to restart?\n\nPress (A) to Restart (X) to Cancel"))
			{
				sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
				if(!FileExists(FileName))
				{
					sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, SelectedLevel);
					if(!FileExists(FileName))
					{
						char *TmpPath = assetPath("levelpacks");
						sprintf(FileName,"%s/%s/level%d.lev",TmpPath,LevelPackName,SelectedLevel);
						SDL_free(TmpPath);
					}
				}
				if(FileExists(FileName))
					WorldParts.Load(FileName, true);
				else
					WorldParts.LoadFromLevelPackFile(LevelPackFile, SelectedLevel, true);

				for (Teller=0;Teller<WorldParts.ItemCount;Teller++)
				{
					if (WorldParts.Items[Teller]->GetType() == IDPlayer)
					{
						Player = WorldParts.Items[Teller];
						break;
					}
				}
			}
			Input->Reset();
			Input->Delay();
		}

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_SELECT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_SELECT))))
		{
			SDL_RenderTexture(Renderer,IMGBackground,NULL,NULL);
			WorldParts.DrawFloor(Player);
			WorldParts.Draw();
			if(LevelPackFile->Loaded)
			{
				if(strlen(LevelPackFile->LevelsMeta[SelectedLevel-1].author) > 0)
					sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s\nComments: %s",LevelPackName,SelectedLevel,InstalledLevels,WorldParts.Moves,WorldParts.Pushes,LevelPackFile->LevelsMeta[SelectedLevel-1].author,LevelPackFile->LevelsMeta[SelectedLevel-1].comments);
				else
					sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s\nComments: %s",LevelPackName,SelectedLevel,InstalledLevels,WorldParts.Moves,WorldParts.Pushes,LevelPackFile->author,LevelPackFile->LevelsMeta[SelectedLevel-1].comments);
			}
			else
				sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d",LevelPackName,SelectedLevel,InstalledLevels,WorldParts.Moves,WorldParts.Pushes);
			PrintForm(Msg);
			SDL_Delay(250);
			Input->Delay();
			Input->Reset();
		}


		if (!Player->IsMoving)
		{
			if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))))
			{
				WorldParts.HistoryGoBack();
			}

			if ( Input->Ready() && !(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))) &&
				(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_RIGHT))))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() + 1, Player->GetPlayFieldY()))
				{
					WorldParts.HistoryAdd();
				}
				Player->MoveTo(Player->GetPlayFieldX() + 1, Player->GetPlayFieldY(),false);
			}
			else
			{
				if ( Input->Ready() &&  !(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))) && 
					(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT))))
				{
					if (Player->CanMoveTo(Player->GetPlayFieldX() - 1, Player->GetPlayFieldY()))
					{
						WorldParts.HistoryAdd();
					}
					Player->MoveTo(Player->GetPlayFieldX() - 1, Player->GetPlayFieldY(),false);
				}
				else
				{
					if ( Input->Ready() && !(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))) &&
						(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_UP))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_UP))))
					{
						if (Player->CanMoveTo(Player->GetPlayFieldX() , Player->GetPlayFieldY()-1))
						{
							WorldParts.HistoryAdd();
						}
						Player->MoveTo(Player->GetPlayFieldX(), Player->GetPlayFieldY() - 1,false);
					}
					else
					{
						if ( Input->Ready() && !(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))) &&
							(Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_DOWN))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_DOWN))))
						{
							if (Player->CanMoveTo(Player->GetPlayFieldX() , Player->GetPlayFieldY()+1))
							{
								WorldParts.HistoryAdd();
							}
							Player->MoveTo(Player->GetPlayFieldX(), Player->GetPlayFieldY() +1,false);
						}
					}
				}
			}
		}
		SDL_RenderTexture(Renderer, IMGBackground,NULL,NULL);
		WorldParts.Move();
		WorldParts.DrawFloor(WorldParts.Player);
		WorldParts.Draw();
		
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
		if (!Player->IsMoving && (WorldParts.Moves >0) && StageDone())
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_STAGEEND],0);
			SDL_Delay(250);
			if (LevelEditorMode)
			{
				sprintf(Msg, "Congratulations !\nYou Succesfully Solved this level in\n%d moves and %d pushes\nDo you want to return to the\nlevel editor ?\n(A) Leveleditor (X) Play Again", WorldParts.Moves, WorldParts.Pushes);
				if (AskQuestion(Msg))
				{
					GameState = GSLevelEditor;
					Input->Reset();
				}
				else
				{
					sprintf(FileName,"%s/.sokoban_temp.lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
					WorldParts.RemoveAll();
					WorldParts.Load(FileName, true);
					for (Teller=0;Teller<WorldParts.ItemCount;Teller++)
					{
						if (WorldParts.Items[Teller]->GetType() == IDPlayer)
						{
							Player = WorldParts.Items[Teller];
							break;
						}
					}
					Input->Reset();
				}

			}
			else
			{
				if (SelectedLevel == UnlockedLevels)
				{
					if ( UnlockedLevels < InstalledLevels)
					{
						sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\nThe next level has now been unlocked!\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts.Moves, WorldParts.Pushes);
						PrintForm(Msg);
						UnlockedLevels++;
						SelectedLevel++;
						SaveUnlockData();
						GameState = GSStageSelect;
						Input->Reset();
					}
					else
					{
						sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\nlevelpack %s\nis now finished, try out another one!\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts.Moves, WorldParts.Pushes,LevelPackName);
						PrintForm(Msg);
						GameState = GSTitleScreen;
						Input->Reset();
					}
				}
				else
				{
					sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\n\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts.Moves, WorldParts.Pushes);
					PrintForm(Msg);
					GameState = GSStageSelect;
					Input->Reset();

				}
			}
		}
	}
	if (GlobalSoundEnabled)
		Mix_HaltMusic();
	SelectedMusic++;
	if (SelectedMusic >= MusicCount)
		SelectedMusic = 1;
}
