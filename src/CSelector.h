#ifndef CSELECTOR_H
#define CSELECTOR_H

#include <SDL.h>
#include "CWorldPart.h"

typedef struct CSelector CSelector;
struct CSelector
{
	CWorldPart *Part;
	int Selection;
};

CSelector* CSelector_Create();
void CSelector_MoveUp(CSelector* Selector);
void CSelector_MoveDown(CSelector* Selector);
void CSelector_MoveLeft(CSelector* Selector);
void CSelector_MoveRight(CSelector* Selector);
void CSelector_IncSelection(CSelector* Selector);
void CSelector_DecSelection(CSelector* Selector);
void CSelector_Draw(CSelector* Selector, SDL_Surface *Surface);
int CSelector_GetSelection(CSelector* Selector);
int CSelector_GetPlayFieldX(CSelector* Selector);
int CSelector_GetPlayFieldY(CSelector* Selector);
void CSelector_Destroy(CSelector* Selector);


#endif