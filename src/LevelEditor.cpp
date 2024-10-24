#include <unistd.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
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
	if (NumSpots-NumFilledSpots == 0)
	{
		if (GlobalSoundEnabled)
			Mix_PlayChannel(-1,Sounds[SND_ERROR],0);
		PrintForm("Can not save this level because there are\nno empty spots in this level! Please Add at least\none empty spot and try again or cancel saving.\n\nPress (A) to continue");
		return true;
	}
	return false;

}

void LevelEditor()
{
    CInput *Input = new CInput(InputDelay >> 1);
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
		sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
		WorldParts.Load(FileName, true);
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
        Input->Update();

		if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;

					if (Input->Ready() && ((Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)])|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]))
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

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_START)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_START)]))
					{
						if(!LevelErrorsFound())
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							sprintf(FileName,"%s/.sokoban_temp.lev",getenv("HOME") == NULL ? ".": getenv("HOME"));
							WorldParts.Save(FileName);
							StageReload = true;
							GameState=GSGame;
						}
						Input->Reset();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
					{
						Selector.MoveRight();
						Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)]))
					{
						Selector.MoveLeft();
						Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_UP)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_UP)]))
					{
						Selector.MoveUp();
						Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_DOWN)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_DOWN)]))
					{
						Selector.MoveDown();
						Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_L)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_L)]))
					{
						Selector.DecSelection();
						Input->Delay();
						SDL_Delay(100);
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_R)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_R)]))
					{
						Selector.IncSelection();
						Input->Delay();
						SDL_Delay(100);
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_SELECT)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_SELECT)] ))
					{
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_MENU],0);
						ShowPosition = !ShowPosition;
						Input->Delay();
						SDL_Delay(100);
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_Y)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_Y)]))
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

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_X)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_X)]))
					{
						LevelHasChanged |= WorldParts.CenterLevel();
                       	Input->Delay();
					}

					if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_B)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_B)]))
					{
						if (LevelHasChanged)
						{
							if (GlobalSoundEnabled)
								Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
							if(AskQuestion("The current level isn't saved yet!\nDo you want to save it now ?\n\nPress (A) to save, (X) to cancel saving"))
							{
								SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
								WorldParts.Move();
								WorldParts.Draw(Buffer);
								Selector.Draw(Buffer);
								if (ShowPosition)
								{
									sprintf(Tekst,"X: %d - Y: %d",Selector.GetPlayFieldX(),Selector.GetPlayFieldY());
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
									sprintf(FileName,"%s/.sokoban_levelpacks/%s",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackFileName);
#ifdef WIN32
                       				mkdir(FileName);
#else
                       				mkdir(FileName,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
                        			if (SelectedLevel==0)
                            			sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackFileName,InstalledLevels+1);
                        			else
                            			sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackFileName,SelectedLevel);

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


		SDL_BlitSurface(IMGBackground,NULL,Buffer,NULL);
		WorldParts.Move();
		WorldParts.Draw(Buffer);
		Selector.Draw(Buffer);
		if (ShowPosition)
		{			
			sprintf(Tekst,"X: %d - Y: %d",Selector.GetPlayFieldX(),Selector.GetPlayFieldY());
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
	delete Input;
	delete[] FileName;
}
