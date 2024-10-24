#include "CHistory.h"
#include "CWorldPart.h"

CHistory::CHistory(CWorldPart *Partin)
{
	ItemCount = 0;
	Part = Partin;
}

void CHistory::Add(int X, int Y)
{
	SPrevPoint Temp;
	int Teller;
	Temp.X = X;
	Temp.Y = Y;
	if (ItemCount < MaxHistory)
	{
		Items[ItemCount] = Temp;
		ItemCount++;
	}
	else
	{
		for (Teller=0;Teller < ItemCount - 1 ;Teller++)
		{
			Items[Teller] = Items[Teller+1];
		}
		Items[ItemCount-1] = Temp;
	}
}

void CHistory::GoBack()
{
	if(ItemCount > 0)
	{
		ItemCount--;
		Part->MoveTo(Items[ItemCount].X,Items[ItemCount].Y,true);
	}
}
