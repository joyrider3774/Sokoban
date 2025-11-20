#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include "Titlescreen.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"


void Options()
{
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
	
	while (GameState == GSOptions)
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

		if ( Input->Ready() &&  (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_B))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_B))))
		{
			GameState= GSTitleScreen;
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
			Input->Delay();
		}

		if (Input->Ready() && ((Input->KeyboardHeld(SDLK_LALT) || Input->KeyboardHeld(SDLK_RALT)) && Input->KeyboardHeld(SDLK_RETURN)))
        {
			if(!fullScreen)
				SDL_GetWindowSize(SdlWindow, &WINDOW_WIDTH, &WINDOW_HEIGHT);
            fullScreen = !fullScreen;
			SaveSettings();
            SDL_SetWindowFullscreen(SdlWindow, fullScreen);
            Input->Delay();
            continue;
        }

					
		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT))))
		{
			if(Selection==1)
			{
            	if(!fullScreen)
					SDL_GetWindowSize(SdlWindow, &WINDOW_WIDTH, &WINDOW_HEIGHT);
				fullScreen = !fullScreen;
				SDL_SetWindowFullscreen(SdlWindow, fullScreen);
				SaveSettings();
				Input->Delay();
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);					
            	continue;
			}

			if(Selection == 2)
			{
				fullScreen = false;
				WINDOW_WIDTH = 640;
				WINDOW_HEIGHT = 360;
				SDL_SetWindowFullscreen(SdlWindow, fullScreen);
				SDL_SetWindowSize(SdlWindow, WINDOW_WIDTH, WINDOW_HEIGHT);
				SDL_SetWindowPosition(SdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				Input->Delay();
			}
			
			if(Selection == 3)
			{
				showfps = !showfps;
				Input->Delay();
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				continue;
			}
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_RIGHT))))
		{
			if (Selection==1)
			{
            	if(!fullScreen)
					SDL_GetWindowSize(SdlWindow, &WINDOW_WIDTH, &WINDOW_HEIGHT);
				fullScreen = !fullScreen;
				SDL_SetWindowFullscreen(SdlWindow, fullScreen);
				SaveSettings();
				Input->Delay();
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);					
            	continue;
			}

			if(Selection == 2)
			{
				fullScreen = false;
				WINDOW_WIDTH = 640;
				WINDOW_HEIGHT = 360;
				SDL_SetWindowFullscreen(SdlWindow, fullScreen);
				SDL_SetWindowSize(SdlWindow, WINDOW_WIDTH, WINDOW_HEIGHT);
				SDL_SetWindowPosition(SdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				Input->Delay();
			}

			if(Selection == 3)
			{
				showfps = !showfps;
				Input->Delay();
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
				continue;
			}
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
			if (Selection < 4)
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
					if(!fullScreen)
						SDL_GetWindowSize(SdlWindow, &WINDOW_WIDTH, &WINDOW_HEIGHT);
					fullScreen = !fullScreen;
					SDL_SetWindowFullscreen(SdlWindow, fullScreen);
					SaveSettings();
					Input->Delay();
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					break;
				case 2:
					fullScreen = false;
					WINDOW_WIDTH = 640;
					WINDOW_HEIGHT = 360;
					SDL_SetWindowFullscreen(SdlWindow, fullScreen);
					SDL_SetWindowSize(SdlWindow, WINDOW_WIDTH, WINDOW_HEIGHT);
					SDL_SetWindowPosition(SdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					Input->Delay();
					break;
				case 3:
					showfps = !showfps;
					Input->Delay();
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					break;
				case 4:
					GameState = GSTitleScreen;
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					break;
			}
			Input->Delay();
		}

		printTitleInfo();


        SDL_FRect Rect;
        Rect.x = 50.0f*UI_WIDTH_SCALE;
        Rect.y = 94.0f*UI_HEIGHT_SCALE;
        Rect.w = 220.0f*UI_WIDTH_SCALE;
        Rect.h = 64.0f*UI_HEIGHT_SCALE;
        SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
   		SDL_RenderFillRect(Renderer, &Rect);
        SDL_SetRenderDrawColor(Renderer,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
   		SDL_RenderRect(Renderer, &Rect);      
        SDL_FRect Rect2;
        Rect2.x = 52.0f*UI_WIDTH_SCALE;
        Rect2.y = 96.0f*UI_HEIGHT_SCALE;
        Rect2.w = 216.0f*UI_WIDTH_SCALE;
        Rect2.h = 60.0f*UI_HEIGHT_SCALE;
        SDL_RenderRect(Renderer, &Rect2);
		int w,h;
		SDL_GetWindowSize(SdlWindow, &w, &h);
		sprintf(Tekst,"Fullscreen:%i\nReset Window (Current: %ix%i)\nShow FPS:%i\nBack\n", fullScreen, w, h, showfps);
		WriteText(BigFont,Tekst,strlen(Tekst),80*UI_WIDTH_SCALE,97*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		WriteText(BigFont,Tekst,strlen(Tekst),55*UI_WIDTH_SCALE,97*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
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
