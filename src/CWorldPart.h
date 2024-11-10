#ifndef CWORLDPART_H
#define CWORLDPART_H

#include <SDL.h>
#include "CHistory.h"
#include "CWorldParts.h"
#include <stdbool.h>

typedef struct CWorldParts CWorldParts;
typedef struct CHistory CHistory;

typedef struct CWorldPart CWorldPart;
struct CWorldPart
{
	int MoveDelayCounter;
	bool FirstArriveEventFired;
	int AnimCounter,AnimBase,AnimDelay,AnimDelayCounter,AnimPhases;
	int Type,MoveSpeed,MoveDelay,Xi,Yi,X,Y,AnimPhase,PlayFieldX,PlayFieldY,Z;
	bool BHistory;
	SDL_Surface * Image;
	CWorldParts *ParentList;
	bool IsMoving;
	CHistory *History;
	bool Selected;
};

CWorldPart* CWorldPart_Create(const int PlayFieldXin,const int PlayFieldYin,bool CreateHistory, const int aType);
void CWorldPart_Destroy(CWorldPart* WorldPart);
int CWorldPart_GetType(CWorldPart* WorldPart); 
int CWorldPart_GetX(CWorldPart* WorldPart);
int CWorldPart_GetY(CWorldPart* WorldPart);
int CWorldPart_GetPlayFieldX(CWorldPart* WorldPart);
int CWorldPart_GetPlayFieldY(CWorldPart* WorldPart);
int CWorldPart_GetZ(CWorldPart* WorldPart);
int CWorldPart_GetAnimPhase(CWorldPart* WorldPart);
bool CWorldPart_HasHistory(CWorldPart* WorldPart);
void CWorldPart_HistoryAdd(CWorldPart* WorldPart);
void CWorldPart_HistoryGoBack(CWorldPart* WorldPart);
void CWorldPart_SetAnimPhase(CWorldPart* WorldPart, int AnimPhaseIn);
void CWorldPart_Event_ArrivedOnNewSpot(CWorldPart* WorldPart);
void CWorldPart_Event_BeforeDraw(CWorldPart* WorldPart);
void CWorldPart_Event_LeaveCurrentSpot(CWorldPart* WorldPart);
void CWorldPart_SetPosition(CWorldPart* WorldPart,const int PlayFieldXin,const int PlayFieldYin);
void CWorldPart_MoveTo(CWorldPart* WorldPart,const int PlayFieldXin,const int PlayFieldYin,bool BackWards);
bool CWorldPart_CanMoveTo(CWorldPart* WorldPart,const int PlayFieldXin,const int PlayFieldYin);
void CWorldPart_Move(CWorldPart* WorldPart);
void CWorldPart_Draw(CWorldPart* WorldPart,SDL_Surface* Surface);


#endif