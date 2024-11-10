#ifndef CHISTORY_H
#define CHISTORY_H

#include "Defines.h"
#include "CWorldPart.h"

typedef struct CWorldPart CWorldPart;

typedef struct SPrevPoint SPrevPoint;
struct SPrevPoint
{
	int X,Y;
};


typedef struct CHistory CHistory;
struct CHistory
{
	SPrevPoint Items[MaxHistory];
	CWorldPart *Part;
	int ItemCount;
};

CHistory* CHistory_Create(CWorldPart *Partin);
void CHistory_Add (CHistory* History, int X, int Y);
void CHistory_GoBack(CHistory* History);
void CHistory_Destroy(CHistory* History);

#endif