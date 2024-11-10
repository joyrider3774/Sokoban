#include <stdbool.h>
#include "CSelector.h"
#include "CWorldPart.h"


CSelector *CSelector_Create()
{
	CSelector* Result = (CSelector*) malloc(sizeof(CSelector));
	Result->Selection = IDEmpty;
	Result->Part = CWorldPart_Create(9,7, false, IDEmpty);
	Result->Part->Selected = true;
	return Result;
}

void CSelector_MoveUp(CSelector* Selector)
{
	CWorldPart_SetPosition(Selector->Part, Selector->Part->PlayFieldX,Selector->Part->PlayFieldY-1);
}

void CSelector_MoveDown(CSelector* Selector)
{
	CWorldPart_SetPosition(Selector->Part,Selector->Part->PlayFieldX,Selector->Part->PlayFieldY+1);
}

void CSelector_MoveRight(CSelector* Selector)
{
	CWorldPart_SetPosition(Selector->Part,Selector->Part->PlayFieldX +1,Selector->Part->PlayFieldY);
}

void CSelector_MoveLeft(CSelector* Selector)
{
	CWorldPart_SetPosition(Selector->Part,Selector->Part->PlayFieldX -1,Selector->Part->PlayFieldY);
}

void CSelector_IncSelection(CSelector* Selector)
{
	int X,Y;
	Selector->Selection++;
	if (Selector->Selection > 5)
		Selector->Selection = 1;
	X = Selector->Part->PlayFieldX;
	Y = Selector->Part->PlayFieldY;
	CWorldPart_Destroy(Selector->Part);
	Selector->Part = CWorldPart_Create(X,Y, false, Selector->Selection);
	Selector->Part->Selected = true;
}

void CSelector_DecSelection(CSelector* Selector)
{
	int X,Y;
	Selector->Selection--;
	if (Selector->Selection < 1)
		Selector->Selection = 5;
	X = Selector->Part->PlayFieldX;
	Y = Selector->Part->PlayFieldY;
	CWorldPart_Destroy(Selector->Part);
	Selector->Part = CWorldPart_Create(X,Y, false, Selector->Selection);
	Selector->Part->Selected = true;
}

void CSelector_Draw(CSelector* Selector, SDL_Surface *Surface)
{
	CWorldPart_Draw(Selector->Part, Surface);
}

int CSelector_GetSelection(CSelector* Selector)
{
	return Selector->Selection;
}

int CSelector_GetPlayFieldX(CSelector* Selector)
{
	return Selector->Part->PlayFieldX;
}

int CSelector_GetPlayFieldY(CSelector* Selector)
{
	return Selector->Part->PlayFieldY;
}

void CSelector_Destroy(CSelector* Selector)
{
	if(Selector)
	{
		CWorldPart_Destroy(Selector->Part);
		free(Selector);
		Selector = NULL;
	}
}
