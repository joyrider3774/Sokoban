#include <stdbool.h>
#include "CInput.h"


CInput* CInput_Create(int UpdateCounterDelay) 
{
    CInput* Result = (CInput*) malloc(sizeof(CInput));
	CInput_Reset(Result);
    Result->PNumJoysticks = SDL_NumJoysticks();
    for (int teller=0;teller< Result->PNumJoysticks;teller++)
	{
    	SDL_JoystickOpen(teller);
	}
	if (Result->PNumJoysticks > 0)
	{
    	SDL_JoystickEventState(SDL_ENABLE);
	}
    Result->PUpdateCounterDelay = UpdateCounterDelay;
    Result->UpdateCounter = 0;
	return Result;
}

void CInput_Destroy(CInput* Input) 
{
	if(Input)
	{
		free(Input);
		Input = NULL;
	}
}


void CInput_Update(CInput* Input) 
{
    SDL_Event Event;
    if (Input->UpdateCounter > 0)
        Input->UpdateCounter--;
    while(SDL_PollEvent(&Event))
    {
        if(Event.type == SDL_QUIT)
            Input->SpecialsHeld[SPECIAL_QUIT_EV] = true;
        
		if(Event.type == SDL_KEYDOWN)
            Input->KeyboardHeld[Event.key.keysym.sym] = true;
        
		if(Event.type == SDL_KEYUP)
            Input->KeyboardHeld[Event.key.keysym.sym] = false;
        
		if (Event.type == SDL_JOYHATMOTION)
		{
			switch (Event.jhat.value)
			{
				case 1: 
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_UP] = true;
					break;
				case 2: 
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_RIGHT] = true;
					break;
				case 4: 
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_DOWN] = true;
					break;
				case 8: 
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_LEFT] = true;
					break;
				default:
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_UP] = false;
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_RIGHT] = false;
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_DOWN] = false;
					Input->JoystickHeld[Event.jhat.which][JOYSTICK_LEFT] = false;
					break;			
			}
		}
		
		if(Event.type == SDL_JOYAXISMOTION)
		{
            if(Event.jaxis.axis == 0)
            {
                if(Event.jaxis.value > JOYSTICKDEADZONE)
                {
                    if(Event.jaxis.which < MAXJOYSTICKS)
                        Input->JoystickHeld[Event.jaxis.which][JOYSTICK_RIGHT] = true;

                }
                else
				{
                    if(Event.jaxis.value < -JOYSTICKDEADZONE)
                    {
                        if(Event.jaxis.which < MAXJOYSTICKS)
                            Input->JoystickHeld[Event.jaxis.which][JOYSTICK_LEFT] = true;
                    }
                    else
                    {
                       Input->JoystickHeld[Event.jaxis.which][JOYSTICK_LEFT] = false;
                       Input->JoystickHeld[Event.jaxis.which][JOYSTICK_RIGHT] = false;

                    }
				}
            }
            else
			{
                if(Event.jaxis.axis == 1)
                {
                    if(Event.jaxis.value > JOYSTICKDEADZONE)
                    {
                        if(Event.jaxis.which < MAXJOYSTICKS)
                            Input->JoystickHeld[Event.jaxis.which][JOYSTICK_DOWN] = true;

                    }
                    else
					{
                        if(Event.jaxis.value < -JOYSTICKDEADZONE)
                        {
                            if(Event.jaxis.which < MAXJOYSTICKS)
                                Input->JoystickHeld[Event.jaxis.which][JOYSTICK_UP] = true;
                        }
                        else
                        {
                            if(Event.jaxis.which < MAXJOYSTICKS)
                            {
                                Input->JoystickHeld[Event.jaxis.which][JOYSTICK_DOWN] = false;
                                Input->JoystickHeld[Event.jaxis.which][JOYSTICK_UP] = false;
                            }
                        }
					}
                }
			}
		}
		
        if(Event.type == SDL_JOYBUTTONDOWN)
            if(Event.jbutton.button < MAXJOYSTICKBUTTONS)
            {
                if(Event.jbutton.which < MAXJOYSTICKS)
                    Input->JoystickHeld[Event.jbutton.which][Event.jbutton.button] = true;
            }

        if(Event.type == SDL_JOYBUTTONUP)
            if(Event.jbutton.button < MAXJOYSTICKBUTTONS)
            {
                if(Event.jbutton.which < MAXJOYSTICKS)
                    Input->JoystickHeld[Event.jbutton.which][Event.jbutton.button] = false;
            }

        if(Event.type == SDL_MOUSEBUTTONDOWN)
            if(Event.button.which < MAXMOUSES)
                if(Event.button.button < MAXMOUSEBUTTONS)
                    Input->MouseHeld[Event.button.which][Event.button.button] = true;

        if(Event.type == SDL_MOUSEBUTTONUP)
            if(Event.button.which < MAXMOUSES)
                if(Event.button.button < MAXMOUSEBUTTONS)
                    Input->MouseHeld[Event.button.which][Event.button.button] = false;
    }
}

void CInput_Reset(CInput* Input) 
{
    int x,y;
    for (x=0;x<MAXJOYSTICKS;x++)
        for (y=0;y<MAXJOYSTICKBUTTONS;y++)
            Input->JoystickHeld[x][y] = false;
    for (x=0;x<MAXMOUSES;x++)
        for (y=0;y<MAXMOUSEBUTTONS;y++)
            Input->MouseHeld[x][y] = false;
    for (x=0;x<SDLK_LAST;x++)
        Input->KeyboardHeld[x] = false;
    for (x=0;x<MAXSPECIALKEYS;x++)
        Input->SpecialsHeld[x] = false;
}

bool CInput_Ready(CInput* Input)
{
	return (Input->UpdateCounter == 0);
}

void CInput_Delay(CInput* Input)
{
	Input->UpdateCounter = Input->PUpdateCounterDelay;
}

int CInput_NumJoysticks(CInput* Input)
{
	return Input->PNumJoysticks;
}