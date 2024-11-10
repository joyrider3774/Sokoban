#ifndef CINPUT_H
#define CINPUT_H
#define MAXJOYSTICKS 10
#define MAXJOYSTICKBUTTONS 37
#define MAXSPECIALKEYS 1
#define MAXMOUSEBUTTONS 32
#define MAXMOUSES 10

#define SPECIAL_QUIT_EV 0

#define JOYSTICKDEADZONE 15000

#define JOYSTICK_LEFT MAXJOYSTICKBUTTONS-2
#define JOYSTICK_UP MAXJOYSTICKBUTTONS-3
#define JOYSTICK_RIGHT MAXJOYSTICKBUTTONS-4
#define JOYSTICK_DOWN MAXJOYSTICKBUTTONS-5
#define JOYSTICK_NONE MAXJOYSTICKBUTTONS-1

#include <SDL/SDL_joystick.h>
#include <SDL/SDL_keysym.h>
#include <SDL/SDL.h>
#include <stdbool.h>

typedef struct CInput CInput;
struct CInput 
{
    bool JoystickHeld[MAXJOYSTICKS][MAXJOYSTICKBUTTONS];
    bool SpecialsHeld[MAXSPECIALKEYS];
    bool KeyboardHeld[SDLK_LAST];
    bool MouseHeld[MAXMOUSES][MAXMOUSEBUTTONS];
    int PNumJoysticks;
    int UpdateCounter;
    int PUpdateCounterDelay;
};

CInput* CInput_Create(int UpdateCounterDelay);
void CInput_Destroy(CInput* Input);
void CInput_Update(CInput* Input);
void CInput_Reset(CInput* Input);
bool CInput_Ready(CInput* Input);
void CInput_Delay(CInput* Input);
int CInput_NumJoysticks(CInput* Input);


#endif
