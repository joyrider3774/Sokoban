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
	int Teller,Moves=0;
	char Msg[300];
	char FileName[FILENAME_MAX];
	SDL_Event Event;
	SDL_PollEvent(&Event);
	if (GlobalSoundEnabled)
		Mix_HaltMusic();
	Moves=0;
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
							sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName,SelectedLevel);
							if(!FileExists(FileName))
								sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackFileName,SelectedLevel);
							WorldParts.Load(FileName, true);
							Moves=0;
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
						WorldParts.Draw(Buffer);
						sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d\n\nPress (A) To continue playing",LevelPackName,SelectedLevel,InstalledLevels,Moves);
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
				if (Moves > 0)
					Moves--;
			}

			if ( Input->Ready() && !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) &&
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() + 1, Player->GetPlayFieldY()))
				{
					WorldParts.HistoryAdd();
					Moves++;
				}
				Player->MoveTo(Player->GetPlayFieldX() + 1, Player->GetPlayFieldY(),false);
			}

			if ( Input->Ready() &&  !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) && 
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() - 1, Player->GetPlayFieldY()))
				{
					WorldParts.HistoryAdd();
					Moves++;
				}
				Player->MoveTo(Player->GetPlayFieldX() - 1, Player->GetPlayFieldY(),false);
			}

			if ( Input->Ready() && !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) &&
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_UP)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_UP)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() , Player->GetPlayFieldY()-1))
				{
					WorldParts.HistoryAdd();
					Moves++;
				}
				Player->MoveTo(Player->GetPlayFieldX(), Player->GetPlayFieldY() - 1,false);
			}

			if ( Input->Ready() && !(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]) &&
				(Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_DOWN)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_DOWN)]))
			{
				if (Player->CanMoveTo(Player->GetPlayFieldX() , Player->GetPlayFieldY()+1))
				{
					WorldParts.HistoryAdd();
					Moves++;
				}
				Player->MoveTo(Player->GetPlayFieldX(), Player->GetPlayFieldY() +1,false);
			}
		}
		SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
		WorldParts.Move();
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
				if (AskQuestion("Congratulations !\nYou Succesfully Solved this level\nDo you want to return to the\nlevel editor ?\n(A) Leveleditor (X) Play Again"))
				{
					GameState = GSLevelEditor;
					Input->Reset();
				}
				else
				{
					sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
					WorldParts.RemoveAll();
					WorldParts.Load(FileName, true);
					Moves=0;
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
						sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d\nThe next level has now been unlocked!\n\nPress (A) to continue",SelectedLevel,InstalledLevels);
						PrintForm(Msg);
						UnlockedLevels++;
						SelectedLevel++;
						SaveUnlockData();
						GameState = GSStageSelect;
						Input->Reset();
					}
					else
					{
						sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d\nlevelpack %s\nis now finished, try out another one!\n\nPress (A) to continue",SelectedLevel,InstalledLevels,LevelPackName);
						PrintForm(Msg);
						GameState = GSTitleScreen;
						Input->Reset();
					}
				}
				else
				{
					sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d\n\nPress (A) to continue",SelectedLevel,InstalledLevels);
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
