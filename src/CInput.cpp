#include "CInput.h"


CInput::CInput(int UpdateCounterDelay) {
    Reset();
    PNumJoysticks = SDL_NumJoysticks();
    for (int teller=0;teller< PNumJoysticks;teller++)
	{
    	SDL_JoystickOpen(teller);
	}
	if (PNumJoysticks > 0)
	{
    	SDL_JoystickEventState(SDL_ENABLE);
	}
    PUpdateCounterDelay = UpdateCounterDelay;
    UpdateCounter = 0;
}

CInput::~CInput() {
}

void CInput::Update() {
    SDL_Event Event;
    if (UpdateCounter > 0)
        UpdateCounter--;
    while(SDL_PollEvent(&Event))
    {
        if(Event.type == SDL_QUIT)
            SpecialsHeld[SPECIAL_QUIT_EV] = true;
        
		if(Event.type == SDL_KEYDOWN)
            KeyboardHeld[Event.key.keysym.sym] = true;
        
		if(Event.type == SDL_KEYUP)
            KeyboardHeld[Event.key.keysym.sym] = false;
        
		if (Event.type == SDL_JOYHATMOTION)
		{
			switch (Event.jhat.value)
			{
				case 1: 
					JoystickHeld[Event.jhat.which][JOYSTICK_UP] = true;
					break;
				case 2: 
					JoystickHeld[Event.jhat.which][JOYSTICK_RIGHT] = true;
					break;
				case 4: 
					JoystickHeld[Event.jhat.which][JOYSTICK_DOWN] = true;
					break;
				case 8: 
					JoystickHeld[Event.jhat.which][JOYSTICK_LEFT] = true;
					break;
				default:
					JoystickHeld[Event.jhat.which][JOYSTICK_UP] = false;
					JoystickHeld[Event.jhat.which][JOYSTICK_RIGHT] = false;
					JoystickHeld[Event.jhat.which][JOYSTICK_DOWN] = false;
					JoystickHeld[Event.jhat.which][JOYSTICK_LEFT] = false;
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
                        JoystickHeld[Event.jaxis.which][JOYSTICK_RIGHT] = true;

                }
                else
				{
                    if(Event.jaxis.value < -JOYSTICKDEADZONE)
                    {
                        if(Event.jaxis.which < MAXJOYSTICKS)
                            JoystickHeld[Event.jaxis.which][JOYSTICK_LEFT] = true;
                    }
                    else
                    {
                       JoystickHeld[Event.jaxis.which][JOYSTICK_LEFT] = false;
                       JoystickHeld[Event.jaxis.which][JOYSTICK_RIGHT] = false;

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
                            JoystickHeld[Event.jaxis.which][JOYSTICK_DOWN] = true;

                    }
                    else
					{
                        if(Event.jaxis.value < -JOYSTICKDEADZONE)
                        {
                            if(Event.jaxis.which < MAXJOYSTICKS)
                                JoystickHeld[Event.jaxis.which][JOYSTICK_UP] = true;
                        }
                        else
                        {
                            if(Event.jaxis.which < MAXJOYSTICKS)
                            {
                                JoystickHeld[Event.jaxis.which][JOYSTICK_DOWN] = false;
                                JoystickHeld[Event.jaxis.which][JOYSTICK_UP] = false;
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
                    JoystickHeld[Event.jbutton.which][Event.jbutton.button] = true;
            }

        if(Event.type == SDL_JOYBUTTONUP)
            if(Event.jbutton.button < MAXJOYSTICKBUTTONS)
            {
                if(Event.jbutton.which < MAXJOYSTICKS)
                    JoystickHeld[Event.jbutton.which][Event.jbutton.button] = false;
            }

        if(Event.type == SDL_MOUSEBUTTONDOWN)
            if(Event.button.which < MAXMOUSES)
                if(Event.button.button < MAXMOUSEBUTTONS)
                    MouseHeld[Event.button.which][Event.button.button] = true;

        if(Event.type == SDL_MOUSEBUTTONUP)
            if(Event.button.which < MAXMOUSES)
                if(Event.button.button < MAXMOUSEBUTTONS)
                    MouseHeld[Event.button.which][Event.button.button] = false;
    }
}

void CInput::Reset() {
    int x,y;
    for (x=0;x<MAXJOYSTICKS;x++)
        for (y=0;y<MAXJOYSTICKBUTTONS;y++)
            JoystickHeld[x][y] = false;
    for (x=0;x<MAXMOUSES;x++)
        for (y=0;y<MAXMOUSEBUTTONS;y++)
            MouseHeld[x][y] = false;
    for (x=0;x<SDLK_LAST;x++)
        KeyboardHeld[x] = false;
    for (x=0;x<MAXSPECIALKEYS;x++)
        SpecialsHeld[x] = false;
}

