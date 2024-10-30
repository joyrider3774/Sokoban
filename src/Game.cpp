#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_rotozoom.h>
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
    CInput *Input = new CInput(InputDelay);
	int Teller;
	char Msg[300];
	char FileName[FILENAME_MAX];
	SDL_Event Event;
	SDL_PollEvent(&Event);
	if (GlobalSoundEnabled)
		Mix_HaltMusic();
	if (MusicCount > 1)
 	{
 		SelectedMusic =	1+rand()%(MusicCount-1);
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
		if (ReloadMusic)
		{
			ReloadMusic=false;
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
        Input->Update();

        if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;


                    if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_B)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_B)]) )
					{
						if(!LevelEditorMode)
						{
							GameState = GSStageSelect;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_BACK],0);
						}
                    }

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_START)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_START)]))
					{
						if(LevelEditorMode)
						{
							GameState=GSLevelEditor;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_BACK],0);
						}
					}

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_R)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_R)]))
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

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_L)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_L)]))
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

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_Y)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_Y)]))
					{
						SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
						WorldParts.Draw(Buffer);
						if(AskQuestion("You are about to restart this level\nAre you sure you want to restart?\n\nPress (A) to Restart (X) to Cancel"))
						{
							sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName, SelectedLevel);
							if(!FileExists(FileName))
								sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackName,SelectedLevel);
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

					if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_SELECT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_SELECT)]))
					{
						SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
						WorldParts.DrawFloor(Buffer, Player);
						WorldParts.Draw(Buffer);
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
			if ( Input->Ready() &&  (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]))
			{
				WorldParts.HistoryGoBack();
			}

			if ( Input->Ready() && !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) &&
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() + 1, Player->GetPlayFieldY()))
				{
					WorldParts.HistoryAdd();
				}
				Player->MoveTo(Player->GetPlayFieldX() + 1, Player->GetPlayFieldY(),false);
			}

			if ( Input->Ready() &&  !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) && 
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() - 1, Player->GetPlayFieldY()))
				{
					WorldParts.HistoryAdd();
				}
				Player->MoveTo(Player->GetPlayFieldX() - 1, Player->GetPlayFieldY(),false);
			}

			if ( Input->Ready() && !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) &&
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_UP)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_UP)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() , Player->GetPlayFieldY()-1))
				{
					WorldParts.HistoryAdd();
				}
				Player->MoveTo(Player->GetPlayFieldX(), Player->GetPlayFieldY() - 1,false);
			}

			if ( Input->Ready() && !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) &&
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_DOWN)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_DOWN)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() , Player->GetPlayFieldY()+1))
				{
					WorldParts.HistoryAdd();
				}
				Player->MoveTo(Player->GetPlayFieldX(), Player->GetPlayFieldY() +1,false);
			}
		}
		SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
		WorldParts.Move();
		WorldParts.DrawFloor(Buffer, WorldParts.Player);
		WorldParts.Draw(Buffer);
		
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
		if (!Player->IsMoving && StageDone())
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
					sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
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
		SDL_framerateDelay(&Fpsman);
	}
	delete Input;
	if (GlobalSoundEnabled)
		Mix_HaltMusic();
	SelectedMusic++;
	if (SelectedMusic >= MusicCount)
		SelectedMusic = 1;
}
