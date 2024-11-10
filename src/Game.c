#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_rotozoom.h>
#include <stdbool.h>
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"

bool StageDone()
{
	int Teller,FilledSpots=0,Spots=0;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		if (WorldParts->Items[Teller]->Type == IDBox)
		{
			if (WorldParts->Items[Teller]->AnimPhase == 1)
                FilledSpots++;
		}
		else
            if (WorldParts->Items[Teller]->Type == IDSpot)
                Spots++;
    if (FilledSpots >= Spots)
        return true;
    else
        return false;
}

void Game()
{
    CInput *Input = CInput_Create(InputDelay);
	int Teller;
	char Msg[1500];
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
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if (WorldParts->Items[Teller]->Type == IDPlayer)
		{
			Player = WorldParts->Items[Teller];
			break;
		}
	}
	//should never happen
	if(!Player)
	{
		Player = CWorldPart_Create(0,0, true, IDPlayer);
		CWorldParts_Add(WorldParts,Player);
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
        CInput_Update(Input);

        if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;


                    if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_B)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_B)]) )
					{
						if(!LevelEditorMode)
						{
							GameState = GSStageSelect;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_BACK],0);
						}
                    }

					if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_START)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_START)]))
					{
						if(LevelEditorMode)
						{
							GameState=GSLevelEditor;
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_BACK],0);
						}
					}

					if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_R)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_R)]))
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
                        CInput_Delay(Input);
					}

					if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_L)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_L)]))
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
						CInput_Delay(Input);
					}

					if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_Y)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_Y)]))
					{
						SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
						CWorldParts_Draw(WorldParts,Buffer);
						if(AskQuestion("You are about to restart this level\nAre you sure you want to restart?\n\nPress (A) to Restart (X) to Cancel"))
						{
							sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName, SelectedLevel);
							if(!FileExists(FileName))
								sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackName,SelectedLevel);
							if(FileExists(FileName))
								CWorldParts_Load(WorldParts,FileName, true);
							else
								CWorldParts_LoadFromLevelPackFile(WorldParts,LevelPackFile, SelectedLevel, true);

							for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
							{
								if (WorldParts->Items[Teller]->Type == IDPlayer)
								{
									Player = WorldParts->Items[Teller];
									break;
								}
							}
						}
						CInput_Reset(Input);
						CInput_Delay(Input);
					}

					if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_SELECT)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_SELECT)]))
					{
						SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
						CWorldParts_DrawFloor(WorldParts,Buffer, Player);
						CWorldParts_Draw(WorldParts,Buffer);
						if(LevelPackFile->Loaded)
						{
							if(strlen(LevelPackFile->LevelsMeta[SelectedLevel-1].author) > 0)
								sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s\nComments: %s",LevelPackName,SelectedLevel,InstalledLevels,WorldParts->Moves,WorldParts->Pushes,LevelPackFile->LevelsMeta[SelectedLevel-1].author,LevelPackFile->LevelsMeta[SelectedLevel-1].comments);
							else
								sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d\nAuthor: %s\nComments: %s",LevelPackName,SelectedLevel,InstalledLevels,WorldParts->Moves,WorldParts->Pushes,LevelPackFile->author,LevelPackFile->LevelsMeta[SelectedLevel-1].comments);
						}
						else
							sprintf(Msg,"Level Pack: %s\nLevel: %d/%d - Moves: %d - Pushes: %d",LevelPackName,SelectedLevel,InstalledLevels,WorldParts->Moves,WorldParts->Pushes);
						PrintForm(Msg);
						SDL_Delay(250);
						CInput_Delay(Input);
						CInput_Reset(Input);
					}


		if (!Player->IsMoving)
		{
			if ( CInput_Ready(Input) &&  (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)]))
			{
				CWorldParts_HistoryGoBack(WorldParts);
			}

			if ( CInput_Ready(Input) && !(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)]) &&
				(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_RIGHT)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_RIGHT)]))
			{
				if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX + 1, Player->PlayFieldY))
				{
					CWorldParts_HistoryAdd(WorldParts);
				}
				CWorldPart_MoveTo(Player, Player->PlayFieldX + 1, Player->PlayFieldY,false);
			}

			if ( CInput_Ready(Input) &&  !(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)]) && 
				(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_LEFT)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_LEFT)]))
			{
				if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX - 1, Player->PlayFieldY))
				{
					CWorldParts_HistoryAdd(WorldParts);
				}
				CWorldPart_MoveTo(Player, Player->PlayFieldX - 1, Player->PlayFieldY,false);
			}

			if ( CInput_Ready(Input) && !(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)]) &&
				(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_UP)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_UP)]))
			{
				if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX , Player->PlayFieldY-1))
				{
					CWorldParts_HistoryAdd(WorldParts);
				}
				CWorldPart_MoveTo(Player, Player->PlayFieldX, Player->PlayFieldY - 1,false);
			}

			if ( CInput_Ready(Input) && !(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)]) &&
				(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_DOWN)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_DOWN)]))
			{
				if (CWorldPart_CanMoveTo(Player, Player->PlayFieldX , Player->PlayFieldY+1))
				{
					CWorldParts_HistoryAdd(WorldParts);
				}
				CWorldPart_MoveTo(Player, Player->PlayFieldX, Player->PlayFieldY +1,false);
			}
		}
		SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
		CWorldParts_Move(WorldParts);
		CWorldParts_DrawFloor(WorldParts,Buffer, WorldParts->Player);
		CWorldParts_Draw(WorldParts,Buffer);
		
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
		if (!Player->IsMoving && (WorldParts->Moves >0) && StageDone())
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_STAGEEND],0);
			SDL_Delay(250);
			if (LevelEditorMode)
			{
				sprintf(Msg, "Congratulations !\nYou Succesfully Solved this level in\n%d moves and %d pushes\nDo you want to return to the\nlevel editor ?\n(A) Leveleditor (X) Play Again", WorldParts->Moves, WorldParts->Pushes);
				if (AskQuestion(Msg))
				{
					GameState = GSLevelEditor;
					CInput_Reset(Input);
				}
				else
				{
					sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
					CWorldParts_RemoveAll(WorldParts);
					CWorldParts_Load(WorldParts,FileName, true);
					for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
					{
						if (WorldParts->Items[Teller]->Type == IDPlayer)
						{
							Player = WorldParts->Items[Teller];
							break;
						}
					}
					CInput_Reset(Input);
				}

			}
			else
			{
				if (SelectedLevel == UnlockedLevels)
				{
					if ( UnlockedLevels < InstalledLevels)
					{
						sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\nThe next level has now been unlocked!\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts->Moves, WorldParts->Pushes);
						PrintForm(Msg);
						UnlockedLevels++;
						SelectedLevel++;
						SaveUnlockData();
						GameState = GSStageSelect;
						CInput_Reset(Input);
					}
					else
					{
						sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\nlevelpack %s\nis now finished, try out another one!\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts->Moves, WorldParts->Pushes,LevelPackName);
						PrintForm(Msg);
						GameState = GSTitleScreen;
						CInput_Reset(Input);
					}
				}
				else
				{
					sprintf(Msg,"Congratulations !\nYou Succesfully Solved Level %d/%d in\n%d moves and %d pushes\n\n\nPress (A) to continue",SelectedLevel,InstalledLevels,WorldParts->Moves, WorldParts->Pushes);
					PrintForm(Msg);
					GameState = GSStageSelect;
					CInput_Reset(Input);

				}
			}
		}
		SDL_framerateDelay(&Fpsman);
	}
	CInput_Destroy(Input);
	if (GlobalSoundEnabled)
		Mix_HaltMusic();
	SelectedMusic++;
	if (SelectedMusic >= MusicCount)
		SelectedMusic = 1;
}
