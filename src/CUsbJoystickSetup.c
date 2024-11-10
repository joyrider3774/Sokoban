#include <unistd.h>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>
#include <stdbool.h>
#include "GameFuncs.h"
#include "CUsbJoystickSetup.h"
#include "CInput.h"
#include "Defines.h"

CUsbJoystickSetup *CUsbJoystickSetup_Create() 
{
	CUsbJoystickSetup * Result = (CUsbJoystickSetup*) malloc(sizeof(CUsbJoystickSetup));
    for (int Teller=0; Teller < MAXDEFINITIONS; Teller++) {
          Result->PJoystickButtons[Teller].CurrentButtonValue = JOYSTICK_NONE;
          Result->PJoystickButtons[Teller].DefaultButtonValue = JOYSTICK_NONE;
          memset(Result->PJoystickButtons[Teller].ButtonDescription,0,DESCRIPTIONSIZE);
    }
    Result->PNrOfDefinitions = 0;
	return Result;
}

void CUsbJoystickSetup_Destroy(CUsbJoystickSetup * UsbJoystickSetup) 
{
	if(UsbJoystickSetup)
	{
		free(UsbJoystickSetup);
		UsbJoystickSetup = NULL;
	}
}

bool CUsbJoystickSetup_AddDefinition(CUsbJoystickSetup * UsbJoystickSetup, int Button, char* Definition, int Value, int DefaultValue, SDLKey keyValue, SDLKey defaultKeyValue, char *DisplayValue) 
{
    if (Button >=0 && Button < NROFBUTTONS)
    {
        memset(UsbJoystickSetup->PJoystickButtons[Button].ButtonDescription,0,DESCRIPTIONSIZE);
        strncpy(UsbJoystickSetup->PJoystickButtons[Button].ButtonDescription,Definition,DESCRIPTIONSIZE);
        UsbJoystickSetup->PJoystickButtons[Button].CurrentButtonValue = Value;
        UsbJoystickSetup->PJoystickButtons[Button].DefaultButtonValue = DefaultValue;
        UsbJoystickSetup->PJoystickButtons[Button].CurrentKeyValue = keyValue;
        UsbJoystickSetup->PJoystickButtons[Button].DefaultKeyValue = defaultKeyValue;
        sprintf(UsbJoystickSetup->PJoystickButtons[Button].KeyboardDisplayValue,"%s",DisplayValue);
        sprintf(UsbJoystickSetup->PJoystickButtons[Button].DefaultKeyboardDisplayValue,"%s",DisplayValue);
        UsbJoystickSetup->PNrOfDefinitions++;
        return true;
    }
    return false;

}

int CUsbJoystickSetup_GetButtonValue(CUsbJoystickSetup * UsbJoystickSetup, int Button) 
{
    if (Button >= 0 && Button <NROFBUTTONS)
        return UsbJoystickSetup->PJoystickButtons[Button].CurrentButtonValue;
    return -1;
}

int CUsbJoystickSetup_GetKeyValue(CUsbJoystickSetup * UsbJoystickSetup, int Button) 
{
    if (Button >= 0 && Button <NROFBUTTONS)
        return UsbJoystickSetup->PJoystickButtons[Button].CurrentKeyValue;
    return -1;
}


void CUsbJoystickSetup_DrawCurrentSetup(CUsbJoystickSetup * UsbJoystickSetup, SDL_Surface *Surface,TTF_Font* FontIn,int X, int Y, int XSpacing,int YSpacing,int Selection,  SDL_Color TextColor,SDL_Color SelectedColor, bool Keyboard) 
{
    char ButtonText[100];
    char SelectedDescText[DESCRIPTIONSIZE+4];
    for (int Teller=0; Teller < NROFBUTTONS; Teller++)
    {
        if (strlen(UsbJoystickSetup->PJoystickButtons[Teller].ButtonDescription) > 0)
        {
            WriteText(Surface,FontIn,UsbJoystickSetup->PJoystickButtons[Teller].ButtonDescription,strlen(UsbJoystickSetup->PJoystickButtons[Teller].ButtonDescription),X ,Y + (YSpacing*Teller),0,TextColor);

            if(Keyboard)
            {
                sprintf(ButtonText, "Button: %s",SDL_GetKeyName(UsbJoystickSetup->PJoystickButtons[Teller].CurrentKeyValue));
            }
            else
            if(UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue >= -1 && UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue < MAXJOYSTICKBUTTONS)
                switch (UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue)
                {
                    case JOYSTICK_NONE : sprintf(ButtonText,"Button: None"); break;
                    case JOYSTICK_LEFT : sprintf(ButtonText,"Button: Left"); break;
                    case JOYSTICK_UP : sprintf(ButtonText,"Button: Up"); break;
                    case JOYSTICK_RIGHT : sprintf(ButtonText,"Button: Right"); break;
                    case JOYSTICK_DOWN : sprintf(ButtonText,"Button: Down"); break;
                    default: sprintf(ButtonText,"Button: %d", UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue+1); break;
                }

            if(Selection == Teller)
            {
                sprintf(SelectedDescText,"%s <<",ButtonText);
                WriteText(Surface,FontIn,SelectedDescText,strlen(SelectedDescText),X + XSpacing ,Y + (YSpacing*Teller),YSpacing,SelectedColor);
            }
            else
                WriteText(Surface,FontIn,ButtonText,strlen(ButtonText),X + XSpacing ,Y + (YSpacing*Teller),0,TextColor);
        }
    }
}


void CUsbJoystickSetup_SetButtonValue(CUsbJoystickSetup * UsbJoystickSetup, int Button, int Value) 
{
    int Teller;
    if(Button >= 0 && Button < NROFBUTTONS)
    {
        for(Teller=0;Teller<NROFBUTTONS;Teller++)
            if(UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue == Value)
                UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue = JOYSTICK_NONE;
        UsbJoystickSetup->PJoystickButtons[Button].CurrentButtonValue = Value;
    }
}

void CUsbJoystickSetup_SetKeyValue(CUsbJoystickSetup * UsbJoystickSetup, int Button, SDLKey Value) 
{
    int Teller;
    char Tmp[15];
    if(Button >= 0 && Button < NROFBUTTONS)
    {
        for(Teller=0;Teller<NROFBUTTONS;Teller++)
            if(UsbJoystickSetup->PJoystickButtons[Teller].CurrentKeyValue == Value)
            {
                UsbJoystickSetup->PJoystickButtons[Teller].CurrentKeyValue = UsbJoystickSetup->PJoystickButtons[Button].CurrentKeyValue;
                sprintf(Tmp,"%s",UsbJoystickSetup->PJoystickButtons[Button].KeyboardDisplayValue);
                sprintf(UsbJoystickSetup->PJoystickButtons[Button].KeyboardDisplayValue,"%s",UsbJoystickSetup->PJoystickButtons[Teller].KeyboardDisplayValue);
                sprintf(UsbJoystickSetup->PJoystickButtons[Teller].KeyboardDisplayValue,"%s",Tmp);               
            }
		 UsbJoystickSetup->PJoystickButtons[Button].CurrentKeyValue = Value;
    }
}


bool CUsbJoystickSetup_SaveCurrentButtonValues(CUsbJoystickSetup * UsbJoystickSetup, char *Filename) 
{
    FILE *f;
    int Teller;
    f = fopen(Filename,"wb");
    if(f)
    {
        for(Teller = 0;Teller < NROFBUTTONS;Teller++)
        {
            fprintf(f,"%d %d %s\n",UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue,UsbJoystickSetup->PJoystickButtons[Teller].CurrentKeyValue,UsbJoystickSetup->PJoystickButtons[Teller].KeyboardDisplayValue);
        }
        fclose(f);
        return true;
    }
    return false;
}

bool CUsbJoystickSetup_LoadCurrentButtonValues(CUsbJoystickSetup * UsbJoystickSetup, char *Filename) 
{
    FILE *f;
    int Teller;
    f = fopen(Filename,"rt");
    if(f)
    {
        for(Teller = 0;Teller < NROFBUTTONS;Teller++)
        {
            fscanf(f,"%d %d %s\n",&(UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue),(int*)&(UsbJoystickSetup->PJoystickButtons[Teller].CurrentKeyValue), &(UsbJoystickSetup->PJoystickButtons[Teller].KeyboardDisplayValue[0]));
        }
        fclose(f);
        return true;
    }
    return false;
}

char *CUsbJoystickSetup_GetKeyNameForDefinition(CUsbJoystickSetup * UsbJoystickSetup, int Definition)
{
    return UsbJoystickSetup->PJoystickButtons[Definition].KeyboardDisplayValue;
}

void CUsbJoystickSetup_ResetToDefaults(CUsbJoystickSetup * UsbJoystickSetup) 
{
    for (int Teller=0; Teller < NROFBUTTONS; Teller++)
    {
        UsbJoystickSetup->PJoystickButtons[Teller].CurrentButtonValue = UsbJoystickSetup->PJoystickButtons[Teller].DefaultButtonValue;
        UsbJoystickSetup->PJoystickButtons[Teller].CurrentKeyValue = UsbJoystickSetup->PJoystickButtons[Teller].DefaultKeyValue;
        sprintf(UsbJoystickSetup->PJoystickButtons[Teller].KeyboardDisplayValue,"%s",UsbJoystickSetup->PJoystickButtons[Teller].DefaultKeyboardDisplayValue);
    }
}
