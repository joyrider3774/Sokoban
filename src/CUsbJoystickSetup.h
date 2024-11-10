#ifndef CUSBJOYSTICK_H
#define CUSBJOYSTICK_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdbool.h>

#define MAXDEFINITIONS 50
#define DESCRIPTIONSIZE 1024


typedef struct SJoystickButtonDefinition SJoystickButtonDefinition; 
struct SJoystickButtonDefinition {
    char ButtonDescription[DESCRIPTIONSIZE];
    int DefaultButtonValue;
    int CurrentButtonValue;
    SDLKey DefaultKeyValue;
    SDLKey CurrentKeyValue;
    char KeyboardDisplayValue[15];
    char DefaultKeyboardDisplayValue[15];
};

typedef struct CUsbJoystickSetup CUsbJoystickSetup;
struct CUsbJoystickSetup 
{
    int PNrOfDefinitions;
    SJoystickButtonDefinition PJoystickButtons[MAXDEFINITIONS];
};

CUsbJoystickSetup* CUsbJoystickSetup_Create();
void CUsbJoystickSetup_Destroy(CUsbJoystickSetup* UsbJoystickSetup);
bool CUsbJoystickSetup_AddDefinition(CUsbJoystickSetup* UsbJoystickSetup, int Button, char* Definition, int value, int defaultValue,SDLKey keyValue, SDLKey defaultKeyValue, char *DisplayValue);
int CUsbJoystickSetup_GetButtonValue(CUsbJoystickSetup* UsbJoystickSetup, int Button);
int CUsbJoystickSetup_GetKeyValue(CUsbJoystickSetup* UsbJoystickSetup, int Button);
bool CUsbJoystickSetup_SaveCurrentButtonValues(CUsbJoystickSetup* UsbJoystickSetup, char *Filename);
bool CUsbJoystickSetup_LoadCurrentButtonValues(CUsbJoystickSetup* UsbJoystickSetup, char *Filename);
void CUsbJoystickSetup_SetButtonValue(CUsbJoystickSetup* UsbJoystickSetup, int Button, int Value);
void CUsbJoystickSetup_SetKeyValue(CUsbJoystickSetup* UsbJoystickSetup, int Button, SDLKey Value);
char *CUsbJoystickSetup_GetKeyNameForDefinition(CUsbJoystickSetup* UsbJoystickSetup, int Definition);
void CUsbJoystickSetup_DrawCurrentSetup(CUsbJoystickSetup* UsbJoystickSetup, SDL_Surface *Surface,TTF_Font* FontIn,int X, int Y, int XSpacing, int YSpacing ,int Selection, SDL_Color TextColor,SDL_Color SelectedTextColor,bool Keyboard);
void CUsbJoystickSetup_ResetToDefaults(CUsbJoystickSetup* UsbJoystickSetup);


#endif
