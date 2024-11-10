#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <stdbool.h>
#include "SetupUSBJoysticks.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"


void SetupUsbJoystickButtons()
{
    SDL_Surface *Tmp,*Tmp1;
	int Teller, Selection = 0;
	Tmp1 = SDL_CreateRGBSurface(SDL_SWSURFACE,ORIG_WINDOW_WIDTH,ORIG_WINDOW_HEIGHT,SCREEN_BPP,Screen->format->Rmask,Screen->format->Gmask,Screen->format->Bmask,Screen->format->Amask);
	Tmp = SDL_DisplayFormat(Tmp1);
	SDL_FreeSurface(Tmp1);
	bool done = false;
	bool keyboard = true;
	CInput *Input = CInput_Create(10);
	while (GameState == GSJoystickSetup)
	{
        if(MusicCount > 0)
		{
            if(GlobalSoundEnabled)
			{
                if(!Mix_PlayingMusic())
                {
                    SelectedMusic = 0;
                    Mix_PlayMusic(Music[SelectedMusic],0);
                    SetVolume(Volume);
                }
			}
		}
		CInput_Update(Input);

		if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;

		if(Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_B)]||  Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_B)])
		{
			char FileName[FILENAME_MAX];
			sprintf(FileName,"%s/.sokoban_joystick.def", getenv("HOME") == NULL ? ".": getenv("HOME"));
			CUsbJoystickSetup_SaveCurrentButtonValues(JoystickSetup, FileName);
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
            GameState = GSTitleScreen;
		}

        if(CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_UP)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_UP)]))
        {
            Selection--;
            if( Selection < 0)
                Selection = 11;
            if (GlobalSoundEnabled)
                    Mix_PlayChannel(-1,Sounds[SND_MENU],0);
            CInput_Delay(Input);
        }

		if(CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_LEFT)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_LEFT)] ||
			Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_RIGHT)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_RIGHT)]))
		{
			if (GlobalSoundEnabled)
            	Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			keyboard = !keyboard;
			CInput_Delay(Input);
		}

        if ((Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_R)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_R)]) && 
	        (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_L)] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_L)]))
            CUsbJoystickSetup_ResetToDefaults(JoystickSetup);

        if(CInput_Ready(Input) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_DOWN)]|| Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_DOWN)]))
        {
            Selection++;
            if( Selection >=12)
                Selection = 0;
            if (GlobalSoundEnabled)
                    Mix_PlayChannel(-1,Sounds[SND_MENU],0);
            CInput_Delay(Input);
        }

        SDL_BlitSurface(IMGTitleScreen,NULL,Tmp,NULL);

		printTitleInfo(Tmp);

        boxRGBA(Tmp,45*UI_WIDTH_SCALE,50*UI_HEIGHT_SCALE,295*UI_WIDTH_SCALE,173*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Tmp,45*UI_WIDTH_SCALE,50*UI_HEIGHT_SCALE,295*UI_WIDTH_SCALE,173*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		rectangleRGBA(Tmp,46*UI_WIDTH_SCALE,51*UI_HEIGHT_SCALE,294*UI_WIDTH_SCALE,172*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
        if(keyboard)
		{
			WriteText(Tmp,font, "Keyboard <<", 11, 115*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuBoxBorderColor);
			WriteText(Tmp,font, "Joypad", 6, 170*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuTextColor);
		}
		else
		{
			WriteText(Tmp,font, "Keyboard", 8, 115*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuTextColor);
			WriteText(Tmp,font, "Joypad <<", 9, 170*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuBoxBorderColor);
		}

        CUsbJoystickSetup_DrawCurrentSetup(JoystickSetup,Tmp,font,55*UI_WIDTH_SCALE,61*UI_HEIGHT_SCALE,155*UI_WIDTH_SCALE,9*UI_HEIGHT_SCALE,Selection,MenuTextColor,MenuBoxBorderColor,keyboard);
        SDL_BlitSurface(Tmp,NULL,Buffer,NULL);
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

        if(CInput_Ready(Input) && (Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_A)] ||  Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_A)]))
        {
               if (GlobalSoundEnabled)
                    Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
                CInput_Reset(Input);
                while (!done)
                {
                    CInput_Update(Input);
                    SDL_BlitSurface(IMGTitleScreen,NULL,Tmp,NULL);
                    printTitleInfo(Tmp);

					boxRGBA(Tmp,45*UI_WIDTH_SCALE,50*UI_HEIGHT_SCALE,295*UI_WIDTH_SCALE,173*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
					rectangleRGBA(Tmp,45*UI_WIDTH_SCALE,50*UI_HEIGHT_SCALE,295*UI_WIDTH_SCALE,173*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
					rectangleRGBA(Tmp,46*UI_WIDTH_SCALE,51*UI_HEIGHT_SCALE,294*UI_WIDTH_SCALE,172*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
					if(keyboard)
					{
						WriteText(Tmp,font, "Keyboard <<", 11, 115*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuBoxBorderColor);
						WriteText(Tmp,font, "Joypad", 6, 170*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuTextColor);
					}
					else
					{
						WriteText(Tmp,font, "Keyboard", 8, 115*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuTextColor);
						WriteText(Tmp,font, "Joypad <<", 9, 170*UI_WIDTH_SCALE, 53*UI_HEIGHT_SCALE, 2, MenuBoxBorderColor);
					}

					CUsbJoystickSetup_DrawCurrentSetup(JoystickSetup,Tmp,font,55*UI_WIDTH_SCALE,61*UI_HEIGHT_SCALE,155*UI_WIDTH_SCALE,9*UI_HEIGHT_SCALE,Selection,MenuTextColor,MenuBoxBorderColor,keyboard);
					SDL_BlitSurface(Tmp,NULL,Buffer,NULL);
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
					if(keyboard)
                      for (Teller = 0;Teller<SDLK_LAST;Teller++)
                        if(Input->KeyboardHeld[Teller])
                        {
							if (GlobalSoundEnabled)
                				Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
                            done = true;
                            CUsbJoystickSetup_SetKeyValue(JoystickSetup,Selection,(SDLKey)Teller);
                            break;
                        }

                    if(!keyboard)  
					  for (Teller = 0;Teller<MAXJOYSTICKBUTTONS;Teller++)
                        if(Input->JoystickHeld[0][Teller])
                        {
							if (GlobalSoundEnabled)
                				Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
                            done = true;
                            CUsbJoystickSetup_SetButtonValue(JoystickSetup,Selection,Teller);
                            break;
                        }

                    if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
                    {
                        GameState = GSQuit;
                        done = true;
                    }

                    if((!done) && (Input->KeyboardHeld[CUsbJoystickSetup_GetKeyValue(JoystickSetup,BUT_B)] || Input->SpecialsHeld[SPECIAL_QUIT_EV] || Input->JoystickHeld[0][CUsbJoystickSetup_GetButtonValue(JoystickSetup,BUT_B)]))
					{
						if (GlobalSoundEnabled)
                			Mix_PlayChannel(-1,Sounds[SND_BACK],0);
                        done= true;
					}
                    SDL_framerateDelay(&Fpsman);
                }
                CInput_Reset(Input);
                CInput_Delay(Input);
                done= false;

        }
        SDL_framerateDelay(&Fpsman);

	}
	CInput_Destroy(Input);
	SDL_FreeSurface(Tmp);
}
