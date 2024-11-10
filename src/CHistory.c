#include <stdbool.h>
#include "CHistory.h"
#include "CWorldPart.h"

CHistory* CHistory_Create(CWorldPart *Partin)
{
	CHistory* Result = (CHistory*)malloc(sizeof(CHistory));
	Result->ItemCount = 0;
	Result->Part = Partin;
	return Result;
}

void CHistory_Destroy(CHistory* History)
{
	if(History)
	{
		free(History);
		History = NULL;
	}
}

void CHistory_Add(CHistory* History, int X, int Y)
{
	SPrevPoint Temp;
	int Teller;
	Temp.X = X;
	Temp.Y = Y;
	if (History->ItemCount < MaxHistory)
	{
		History->Items[History->ItemCount] = Temp;
		History->ItemCount++;
	}
	else
	{
		for (Teller=0;Teller < History->ItemCount - 1 ;Teller++)
		{
			History->Items[Teller] = History->Items[Teller+1];
		}
		History->Items[History->ItemCount-1] = Temp;
	}
}

void CHistory_GoBack(CHistory* History)
{
	if(History->ItemCount > 0)
	{
		History->ItemCount--;
		CWorldPart_MoveTo(History->Part, History->Items[History->ItemCount].X,History->Items[History->ItemCount].Y,true);
	}
}
