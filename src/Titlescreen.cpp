#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
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
    Input->SetInputDelay(InputDelay);
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
		sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s",LevelPackName);
		LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);	
	}
	while (GameState == GSTitleScreen)
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
		SDL_RenderTexture(Renderer,IMGTitleScreen,NULL,NULL);

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

					
		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT))))
		{
			if(Selection==3)
				if (InstalledLevelPacksCount > 0)
				{
					SelectedLevelPack--;
					if(SelectedLevelPack < 0)
						SelectedLevelPack = InstalledLevelPacksCount - 1;
					sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);								
					sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
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

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_RIGHT))))
		{
			if (Selection==3)
				if (InstalledLevelPacksCount > 0)
				{
					SelectedLevelPack++;
					if(SelectedLevelPack > InstalledLevelPacksCount-1)
						SelectedLevelPack = 0;
					sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
					sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
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

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_UP)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_UP))))
		{
			if (Selection > 1)
			{
				Selection--;
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_DOWN))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_DOWN))))
		{
			if (Selection < 6)
			{
				Selection++;
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))))
		{
			switch(Selection)
			{
				case 1:
					if (InstalledLevelPacksCount >0)
					{
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
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
							SDL_RenderTexture(Renderer, IMGTitleScreen,NULL,NULL);
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
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
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

		printTitleInfo();


		SDL_FRect Rect;
        Rect.x = 50.0f*UI_WIDTH_SCALE;
        Rect.y = 70.0f*UI_HEIGHT_SCALE;
        Rect.w = 220.0f*UI_WIDTH_SCALE;
        Rect.h = 100.0f*UI_HEIGHT_SCALE;
        SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
   		SDL_RenderFillRect(Renderer, &Rect);
        SDL_SetRenderDrawColor(Renderer,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
   		SDL_RenderRect(Renderer, &Rect);      
        SDL_FRect Rect2;
        Rect2.x = 52.0f*UI_WIDTH_SCALE;
        Rect2.y = 72.0f*UI_HEIGHT_SCALE;
        Rect2.w = 216.0f*UI_WIDTH_SCALE;
        Rect2.h = 96.0f*UI_HEIGHT_SCALE;
        SDL_RenderRect(Renderer, &Rect2);		
		sprintf(Tekst,"Play Selected LevelPack\nLevel Editor\n<%s>\nControls\nCredits\nQuit",LevelPackName);
		WriteText(BigFont,Tekst,strlen(Tekst),80*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		WriteText(BigFont,Tekst,strlen(Tekst),55*UI_WIDTH_SCALE,77*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
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
	delete[] Tekst;
}
