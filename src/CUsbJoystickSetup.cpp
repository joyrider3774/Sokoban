#include <unistd.h>
#include <SDL3/SDL.h>
#include "GameFuncs.h"
#include "CUsbJoystickSetup.h"
#include "CInput.h"
#include "Defines.h"

CUsbJoystickSetup::CUsbJoystickSetup() 
{
    for (int Teller=0; Teller < MAXDEFINITIONS; Teller++) {
          PJoystickButtons[Teller].CurrentButtonValue = JOYSTICK_NONE;
          PJoystickButtons[Teller].DefaultButtonValue = JOYSTICK_NONE;
          memset(PJoystickButtons[Teller].ButtonDescription,0,DESCRIPTIONSIZE);
    }
    PNrOfDefinitions = 0;
}

CUsbJoystickSetup::~CUsbJoystickSetup() 
{
}

bool CUsbJoystickSetup::AddDefinition(int Button,const char* Definition, int Value, int DefaultValue, SDL_Keycode keyValue, SDL_Keycode defaultKeyValue,const char *DisplayValue) 
{
    if (Button >=0 && Button < NROFBUTTONS)
    {
        memset( PJoystickButtons[Button].ButtonDescription,0,DESCRIPTIONSIZE);
        strncpy( PJoystickButtons[Button].ButtonDescription,Definition,DESCRIPTIONSIZE);
        PJoystickButtons[Button].CurrentButtonValue = Value;
        PJoystickButtons[Button].DefaultButtonValue = DefaultValue;
        PJoystickButtons[Button].CurrentKeyValue = keyValue;
        PJoystickButtons[Button].DefaultKeyValue = defaultKeyValue;
        sprintf(PJoystickButtons[Button].KeyboardDisplayValue,"%s",DisplayValue);
        sprintf(PJoystickButtons[Button].DefaultKeyboardDisplayValue,"%s",DisplayValue);
        PNrOfDefinitions++;
        return true;
    }
    return false;

}

int CUsbJoystickSetup::GetButtonValue(int Button) 
{
    if (Button >= 0 && Button <NROFBUTTONS)
        return PJoystickButtons[Button].CurrentButtonValue;
    return -1;
}

SDL_Keycode CUsbJoystickSetup::GetKeyValue(int Button) 
{
    if (Button >= 0 && Button <NROFBUTTONS)
        return PJoystickButtons[Button].CurrentKeyValue;
    return -1;
}


void CUsbJoystickSetup::DrawCurrentSetup(TTF_Font* FontIn,int X, int Y, int XSpacing,int YSpacing,int Selection,  SDL_Color TextColor,SDL_Color SelectedColor, bool Keyboard) 
{
    char ButtonText[100];
    char SelectedDescText[DESCRIPTIONSIZE+4];
    for (int Teller=0; Teller < NROFBUTTONS; Teller++)
    {
        if (strlen(PJoystickButtons[Teller].ButtonDescription) > 0)
        {
            WriteText(FontIn,PJoystickButtons[Teller].ButtonDescription,strlen(PJoystickButtons[Teller].ButtonDescription),X ,Y + (YSpacing*Teller),0,TextColor,false);

            if(Keyboard)
            {
                sprintf(ButtonText, "Button: %s",SDL_GetKeyName(PJoystickButtons[Teller].CurrentKeyValue));
            }
            else
            if(PJoystickButtons[Teller].CurrentButtonValue >= -1 && PJoystickButtons[Teller].CurrentButtonValue < MAXJOYSTICKBUTTONS)
                switch (PJoystickButtons[Teller].CurrentButtonValue)
                {
                    case JOYSTICK_NONE : sprintf(ButtonText,"Button: None"); break;
                    case JOYSTICK_LEFT : sprintf(ButtonText,"Button: Left"); break;
                    case JOYSTICK_UP : sprintf(ButtonText,"Button: Up"); break;
                    case JOYSTICK_RIGHT : sprintf(ButtonText,"Button: Right"); break;
                    case JOYSTICK_DOWN : sprintf(ButtonText,"Button: Down"); break;
                    default: sprintf(ButtonText,"Button: %d", PJoystickButtons[Teller].CurrentButtonValue+1); break;
                }

            if(Selection == Teller)
            {
                sprintf(SelectedDescText,"%s <<",ButtonText);
                WriteText(FontIn,SelectedDescText,strlen(SelectedDescText),X + XSpacing ,Y + (YSpacing*Teller),YSpacing,SelectedColor, false);
            }
            else
                WriteText(FontIn,ButtonText,strlen(ButtonText),X + XSpacing ,Y + (YSpacing*Teller),0,TextColor, false);
        }
    }
}


void CUsbJoystickSetup::SetButtonValue(int Button, int Value) 
{
    int Teller;
    if(Button >= 0 && Button < NROFBUTTONS)
    {
        for(Teller=0;Teller<NROFBUTTONS;Teller++)
            if(PJoystickButtons[Teller].CurrentButtonValue == Value)
                PJoystickButtons[Teller].CurrentButtonValue = JOYSTICK_NONE;
        PJoystickButtons[Button].CurrentButtonValue = Value;
    }
}

void CUsbJoystickSetup::SetKeyValue(int Button, SDL_Keycode Value) 
{
    int Teller;
    char Tmp[100];
    if(Button >= 0 && Button < NROFBUTTONS)
    {
        for(Teller=0;Teller<NROFBUTTONS;Teller++)
            if(PJoystickButtons[Teller].CurrentKeyValue == Value)
            {
                PJoystickButtons[Teller].CurrentKeyValue = PJoystickButtons[Button].CurrentKeyValue;
                sprintf(Tmp,"%s",PJoystickButtons[Button].KeyboardDisplayValue);
                sprintf(PJoystickButtons[Button].KeyboardDisplayValue,"%s",PJoystickButtons[Teller].KeyboardDisplayValue);
                sprintf(PJoystickButtons[Teller].KeyboardDisplayValue,"%s",Tmp);               
            }
		 PJoystickButtons[Button].CurrentKeyValue = Value;
    }
}


bool CUsbJoystickSetup::SaveCurrentButtonValues(const char *Filename) 
{
    FILE *f;
    int Teller;
    f = fopen(Filename,"wb");
    if(f)
    {
        for(Teller = 0;Teller < NROFBUTTONS;Teller++)
        {
            fprintf(f,"%d %d %s\n",PJoystickButtons[Teller].CurrentButtonValue,PJoystickButtons[Teller].CurrentKeyValue,PJoystickButtons[Teller].KeyboardDisplayValue);
        }
        fclose(f);
        return true;
    }
    return false;
}

bool CUsbJoystickSetup::LoadCurrentButtonValues(const char *Filename) 
{
    FILE *f;
    int Teller;
    f = fopen(Filename,"rt");
    if(f)
    {
        for(Teller = 0;Teller < NROFBUTTONS;Teller++)
        {
            fscanf(f,"%d %d %s\n",&PJoystickButtons[Teller].CurrentButtonValue,(int*)&PJoystickButtons[Teller].CurrentKeyValue, &PJoystickButtons[Teller].KeyboardDisplayValue[0]);
        }
        fclose(f);
        return true;
    }
    return false;
}

char *CUsbJoystickSetup::GetKeyNameForDefinition(int Definition)
{
    return PJoystickButtons[Definition].KeyboardDisplayValue;
}

void CUsbJoystickSetup::ResetToDefaults() 
{
    for (int Teller=0; Teller < NROFBUTTONS; Teller++)
    {
        PJoystickButtons[Teller].CurrentButtonValue = PJoystickButtons[Teller].DefaultButtonValue;
        PJoystickButtons[Teller].CurrentKeyValue = PJoystickButtons[Teller].DefaultKeyValue;
        sprintf(PJoystickButtons[Teller].KeyboardDisplayValue,"%s",PJoystickButtons[Teller].DefaultKeyboardDisplayValue);
    }
}
