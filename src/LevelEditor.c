#include <unistd.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "LevelEditor.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"
#include "CSelector.h"

bool LevelErrorsFound()
{
	int Teller,NumBoxes=0,NumSpots=0,NumPlayer=0,NumFilledSpots=0;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		if(WorldParts->Items[Teller]->Type == IDBox)
		{
			NumBoxes++;
			if (WorldParts->Items[Teller]->AnimPhase == 1)
                NumFilledSpots++;
		}
		if(WorldParts->Items[Teller]->Type == IDSpot)
			NumSpots++;
		if(WorldParts->Items[Teller]->Type == IDPlayer)
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
    CInput *Input = CInput_Create(InputDelay >> 1);
	int Teller,Teller2;
	bool ShowPosition=true,AnotherPartFound,SamePartFound,SpotFound;
	char Tekst[200];
	char FileName[FILENAME_MAX];
	SDL_Event Event;
	SDL_PollEvent(&Event);
	CSelector *Selector = CSelector_Create();
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
		sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
		CWorldParts_Load(WorldParts, FileName, true);
		remove(FileName);
		StageReload=false;
	}
	while (GameState == GSLevelEditor)
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

					if (CInput_Ready(Input) && ((Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)])|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)]))
					{
						SamePartFound = false;
						for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
						{
							if((WorldParts->Items[Teller]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
							   (WorldParts->Items[Teller]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
							{
							   	if(WorldParts->Items[Teller]->Type == CSelector_GetSelection(Selector))
								{
									SamePartFound=true;
								}
								if(CSelector_GetSelection(Selector) == IDEmpty)
								{
									LevelHasChanged=true;
									break;
								}
							}
						}
						if(CSelector_GetSelection(Selector) != IDEmpty)
							if(!LevelHasChanged)
								LevelHasChanged=!SamePartFound;
						switch(CSelector_GetSelection(Selector))
						{
							case IDEmpty:
								for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
								{
									if((WorldParts->Items[Teller]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
									   (WorldParts->Items[Teller]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
									{
									  	if(WorldParts->Items[Teller]->Type == IDFloor)
									  	{
									  		AnotherPartFound = false;
									  		SpotFound = false;
									  		for(Teller2=Teller+1;Teller2<WorldParts->ItemCount;Teller2++)
											{
									  			if((WorldParts->Items[Teller2]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
									   			   (WorldParts->Items[Teller2]->PlayFieldY == CSelector_GetPlayFieldY(Selector)))
									   			    {
														if(WorldParts->Items[Teller2]->Type == IDSpot)
															SpotFound = true;
														else
														{
															CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller2]->PlayFieldX,WorldParts->Items[Teller2]->PlayFieldY,WorldParts->Items[Teller2]->Type);
															AnotherPartFound = true;
															break;
														}

													}
											}
											if(SpotFound && !AnotherPartFound)
											{
												CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDSpot);
											}
											else
											{
												if (!SpotFound && !AnotherPartFound)
													CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller]->PlayFieldX,WorldParts->Items[Teller]->PlayFieldY,IDFloor);
											}
											break;
										}
									  	else
										{
									  		if(WorldParts->Items[Teller]->Type == IDSpot)
									  		{
									  			AnotherPartFound = false;
									  			for(Teller2=Teller+1;Teller2<WorldParts->ItemCount;Teller2++)
												{
									  				if((WorldParts->Items[Teller2]->PlayFieldX == CSelector_GetPlayFieldX(Selector)) &&
									   				   (WorldParts->Items[Teller2]->PlayFieldY == CSelector_GetPlayFieldY(Selector)) &&
									   				   (WorldParts->Items[Teller2]->Type != IDFloor))
														{
															CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller2]->PlayFieldX,WorldParts->Items[Teller2]->PlayFieldY,WorldParts->Items[Teller2]->Type);
															AnotherPartFound = true;
															break;
														}
												}
												if (!AnotherPartFound)
													CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller]->PlayFieldX,WorldParts->Items[Teller]->PlayFieldY,IDSpot);
												break;
											}
									  		else
											{
									  			if(WorldParts->Items[Teller]->Type == IDBox)
									  			{
													CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller]->PlayFieldX,WorldParts->Items[Teller]->PlayFieldY,IDBox);
													break;
												}
												else
												{
													if(WorldParts->Items[Teller]->Type == IDPlayer)
													{
														CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller]->PlayFieldX,WorldParts->Items[Teller]->PlayFieldY,IDPlayer);
														break;
													}
													else
													{
														CWorldParts_Remove(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector));
														break;
													}
												}
											}
										}
									}
								}
								
								break;
							case IDBox:
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDWall);
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDBox);
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDPlayer);
								CWorldParts_Add(WorldParts,CWorldPart_Create(CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),false, IDBox));
								break;
							case IDPlayer:
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDWall);
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDBox);
								for(Teller=0;Teller<WorldParts->ItemCount;Teller++)
								{
									if (WorldParts->Items[Teller]->Type == IDPlayer)
									   	CWorldParts_RemoveType(WorldParts,WorldParts->Items[Teller]->PlayFieldX,WorldParts->Items[Teller]->PlayFieldY,IDPlayer);
								}
								CWorldParts_Add(WorldParts,CWorldPart_Create(CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector), false, IDPlayer));
								break;
							case IDWall:
								CWorldParts_Remove(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector));
								CWorldParts_Add(WorldParts,CWorldPart_Create(CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector), false, IDWall));
								break;
							case IDSpot:
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDWall);
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDSpot);
								CWorldParts_Add(WorldParts,CWorldPart_Create(CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector), false, IDSpot));
								break;
							case IDFloor:
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDFloor);
								CWorldParts_RemoveType(WorldParts,CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector),IDWall);
								CWorldParts_Add(WorldParts,CWorldPart_Create(CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector), false, IDFloor));
								break;
						}
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_START)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_START)]))
					{
						if(!LevelErrorsFound())
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
							CWorldParts_Save(WorldParts,FileName);
							StageReload = true;
							GameState=GSGame;
						}
						CInput_Reset(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_RIGHT)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_RIGHT)]))
					{
						CSelector_MoveRight(Selector);
						CInput_Delay(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_LEFT)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_LEFT)]))
					{
						CSelector_MoveLeft(Selector);
						CInput_Delay(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_UP)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_UP)]))
					{
						CSelector_MoveUp(Selector);
						CInput_Delay(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_DOWN)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_DOWN)]))
					{
						CSelector_MoveDown(Selector);
						CInput_Delay(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_L)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_L)]))
					{
						CSelector_DecSelection(Selector);
						CInput_Delay(Input);
						SDL_Delay(100);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_R)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_R)]))
					{
						CSelector_IncSelection(Selector);
						CInput_Delay(Input);
						SDL_Delay(100);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_SELECT)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_SELECT)] ))
					{
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_MENU],0);
						ShowPosition = !ShowPosition;
						CInput_Delay(Input);
						SDL_Delay(100);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_Y)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_Y)]))
					{
						if (WorldParts->ItemCount > 0)
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							if(AskQuestion("You are about to delete all parts\nin this level, are you sure\nyou want to do this?\n\nPress (A) to delete, (X) to cancel"))
							{
								LevelHasChanged = true;
								CWorldParts_RemoveAll(WorldParts);
                            }
						}
                        CInput_Reset(Input);
                        CInput_Delay(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_X)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_X)]))
					{
						LevelHasChanged |= CWorldParts_CenterLevel(WorldParts);
                       	CInput_Delay(Input);
					}

					if (CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_B)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_B)]))
					{
						if (LevelHasChanged)
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							if(AskQuestion("The current level isn't saved yet!\nDo you want to save it now ?\n\nPress (A) to save, (X) to cancel saving"))
							{
								SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
								CWorldParts_Move(WorldParts);
								CWorldParts_Draw(WorldParts,Buffer);
								CSelector_Draw(Selector, Buffer);
								if (ShowPosition)
								{
									sprintf(Tekst,"X: %d - Y: %d",CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector));
									boxRGBA(Buffer,ORIG_WINDOW_WIDTH-50*UI_WIDTH_SCALE,0,ORIG_WINDOW_WIDTH, 13*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
									rectangleRGBA(Buffer,ORIG_WINDOW_WIDTH-50*UI_WIDTH_SCALE,-1,ORIG_WINDOW_WIDTH-1, 13*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
									WriteText(Buffer,font,Tekst,strlen(Tekst),272*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE,0,MenuTextColor);
								}
								if (!LevelErrorsFound())
								{
									sprintf(FileName,"%s/.sokoban_levelpacks",getenv("HOME") == NULL ? ".": getenv("HOME"));
#ifdef WIN32
                     			  	mkdir(FileName);
#else
                        			mkdir(FileName,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
									sprintf(FileName,"%s/.sokoban_levelpacks/%s",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
#ifdef WIN32
                       				mkdir(FileName);
#else
                       				mkdir(FileName,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
                        			if (SelectedLevel==0)
                            			sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName,InstalledLevels+1);
                        			else
                            			sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName,SelectedLevel);

									//to edit default levels
									// if (SelectedLevel==0)
                            		// 	sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackName,InstalledLevels+1);
                        			// else
                            		// 	sprintf(FileName,"./levelpacks/%s/level%d.lev",LevelPackName,SelectedLevel);

									CWorldParts_Save(WorldParts, FileName);
									FindLevels();
									if (SelectedLevel==0)
										SelectedLevel = InstalledLevels;
									LevelHasChanged=false;
									GameState = GSStageSelect;									
								}

							}
							else
								GameState = GSStageSelect;							
                            CInput_Reset(Input);
						}
						else
							GameState = GSStageSelect;
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_BACK],0);
                        CInput_Delay(Input);
					}


		SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
		CWorldParts_Move(WorldParts);
		CWorldParts_Draw(WorldParts,Buffer);
		CSelector_Draw(Selector, Buffer);
		if (ShowPosition)
		{			
			sprintf(Tekst,"X: %d - Y: %d",CSelector_GetPlayFieldX(Selector),CSelector_GetPlayFieldY(Selector));
			boxRGBA(Buffer,ORIG_WINDOW_WIDTH-50*UI_WIDTH_SCALE,0,ORIG_WINDOW_WIDTH, 13*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
			rectangleRGBA(Buffer,ORIG_WINDOW_WIDTH-50*UI_WIDTH_SCALE,-1,ORIG_WINDOW_WIDTH-1, 13*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
			WriteText(Buffer,font,Tekst,strlen(Tekst),272*UI_WIDTH_SCALE,2*UI_HEIGHT_SCALE,0,MenuTextColor);								
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
	CInput_Destroy(Input);
}
