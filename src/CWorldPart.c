#include <SDL_gfxPrimitives.h>
#include <SDL.h>
#include "CWorldParts.h"
#include "CWorldPart.h"
#include "CHistory.h"
#include "Common.h"

CWorldPart* CWorldPart_Create(const int PlayFieldXin,const int PlayFieldYin,bool CreateHistory, const int aType)
{
	CWorldPart* Result = (CWorldPart*) malloc(sizeof(CWorldPart));
	Result->BHistory = CreateHistory;
	if (Result->BHistory)
		Result->History = CHistory_Create(Result);
	Result->PlayFieldX=PlayFieldXin;
	Result->PlayFieldY=PlayFieldYin;
	Result->Xi=0;
	Result->Yi=0;
	Result->X=PlayFieldXin*TileWidth;
	Result->Y=PlayFieldYin*TileHeight;
	Result->Type=aType;
	Result->MoveDelay=0;
	Result->MoveDelayCounter=0;
	Result->IsMoving = false;
	Result->MoveSpeed=0;
	Result->Image = NULL;
	Result->ParentList = 0;
	Result->AnimPhase=0;
	Result->Selected = false;
	Result->FirstArriveEventFired = false;
	Result->Z=0;
	Result->AnimBase=0;
	Result->AnimPhases=0;
	Result->AnimCounter = 0;
	Result->AnimDelay = 0;
	Result->MoveSpeed = 0;

	switch(Result->Type)
	{
		case IDPlayer:
			Result->Image=IMGPlayer;
			Result->AnimBase=4;
			Result->AnimPhase=4;
			Result->AnimPhases=4;
			Result->AnimCounter = 1;
			Result->AnimDelay = 8;
			Result->MoveDelay = 0;
			Result->MoveSpeed = 2;
			Result->AnimDelayCounter =0;
			Result->Z = ZPlayer;
			break;
		case IDSpot:
			Result->Image = IMGSpot;
			Result->Z = ZSpot;
			break;
		case IDBox:
			Result->Image = IMGBox;
			Result->MoveDelay = 0;
			Result->MoveSpeed = 2;
			Result->Z = ZBox;
			break;
		case IDWall:
			Result->Image = IMGWall;
			Result->Z = ZWall;
			break;
		case IDFloor:
			Result->Image = IMGFloor;
			Result->Z = ZFloor;
			break;
		case IDEmpty:
			Result->Image = IMGEmpty;
			Result->Z = ZEmpty;
			break;
	}
	return Result;
}

int CWorldPart_GetType(CWorldPart* WorldPart) 
{
	return WorldPart->Type;
}

int CWorldPart_GetX(CWorldPart* WorldPart) 
{
	return WorldPart->X;
}

int CWorldPart_GetY(CWorldPart* WorldPart) 
{
	return WorldPart->Y;
}

int CWorldPart_GetPlayFieldX(CWorldPart* WorldPart) 
{
	return WorldPart->PlayFieldX;
}

int CWorldPart_GetPlayFieldY(CWorldPart* WorldPart) 
{
	return WorldPart->PlayFieldY;
}

int CWorldPart_GetZ(CWorldPart* WorldPart) 
{
	return WorldPart->Z;
}

int CWorldPart_GetAnimPhase(CWorldPart* WorldPart) 
{
	return WorldPart->AnimPhase;
}

bool CWorldPart_HasHistory(CWorldPart* WorldPart) 
{ 
	return WorldPart->BHistory;
}

void CWorldPart_HistoryAdd(CWorldPart* WorldPart) 
{ 
	if (WorldPart->BHistory) 
		CHistory_Add(WorldPart->History,WorldPart->PlayFieldX,WorldPart->PlayFieldY); 
}

void CWorldPart_HistoryGoBack(CWorldPart* WorldPart) 
{ 
	if(WorldPart->BHistory) 
		CHistory_GoBack(WorldPart->History);
}

void CWorldPart_SetAnimPhase(CWorldPart* WorldPart, int AnimPhaseIn) 
{
	WorldPart->AnimPhase = AnimPhaseIn;
}

void CWorldPart_Event_ArrivedOnNewSpot(CWorldPart* WorldPart) 
{
	int Teller;
	
	if (WorldPart->Type == IDBox)
	{
		WorldPart->AnimPhase = 0;
		if (WorldPart->ParentList)
		{
			for (Teller=0;Teller< WorldPart->ParentList->ItemCount;Teller++)
			{
				if( WorldPart->ParentList->Items[Teller]->Type == IDSpot)
				{
					if ((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
					{
						WorldPart->AnimPhase = 1;
						break;
					}
				}
			}
		}
	}

	if(WorldPart->Type == IDSpot)
	{
		if (WorldPart->ParentList)
		{
			for (Teller=0;Teller< WorldPart->ParentList->ItemCount;Teller++)
			{
				if( WorldPart->ParentList->Items[Teller]->Type == IDBox)
				{
					if ((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
					{
						WorldPart->ParentList->Items[Teller]->AnimPhase = 1;
						break;
					}
				}
			}
		}
	}
}

void CWorldPart_Event_BeforeDraw(CWorldPart* WorldPart) 
{
	if(WorldPart->Type == IDPlayer)
	{
		if (WorldPart->IsMoving)
		{
			WorldPart->AnimPhase = WorldPart->AnimBase + WorldPart->AnimCounter;
			WorldPart->AnimDelayCounter++;
			if (WorldPart->AnimDelayCounter == WorldPart->AnimDelay)
			{
				WorldPart->AnimDelayCounter = 0;
				WorldPart->AnimCounter++;
				if (WorldPart->AnimCounter >= WorldPart->AnimPhases)
					WorldPart->AnimCounter = 0;
			}
		}

	}
}

void CWorldPart_Event_LeaveCurrentSpot(CWorldPart* WorldPart) 
{
	if(WorldPart->Type == IDBox)
	{
		if (WorldPart->ParentList)
		{
			int Teller;
			for (Teller=0;Teller< WorldPart->ParentList->ItemCount;Teller++)
			{
				if( WorldPart->ParentList->Items[Teller]->Type == IDSpot)
				{
					if ((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY))
					{
						WorldPart->AnimPhase = 1;
						break;
					}
				}
			}
		}
	}
}

void CWorldPart_SetPosition(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin)
{
	if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
	{
		WorldPart->PlayFieldX=PlayFieldXin;
		WorldPart->PlayFieldY=PlayFieldYin;
		WorldPart->X=PlayFieldXin*TileWidth;
		WorldPart->Y=PlayFieldYin*TileHeight;
		CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
	}
}

void CWorldPart_MoveTo(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin,bool BackWards)
{
	if(WorldPart->Type== IDPlayer)
	{
		int Teller;
		if(!WorldPart->IsMoving)
		{
			if(CWorldPart_CanMoveTo(WorldPart, PlayFieldXin,PlayFieldYin) || BackWards)
			{
				if(WorldPart->ParentList)
				{
					if(BackWards)
						WorldPart->ParentList->Moves--;
					else
						WorldPart->ParentList->Moves++;
				}
				WorldPart->AnimPhases = 4;
				WorldPart->PlayFieldX = PlayFieldXin;
				WorldPart->PlayFieldY = PlayFieldYin;
				if(WorldPart->X < WorldPart->PlayFieldX*TileWidth)
				{
					WorldPart->Xi = WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 0;
					}
					else
					{
						WorldPart->AnimBase = 4;
						if (WorldPart->ParentList)
						{
							for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
							{
								if(((WorldPart->ParentList->Items[Teller]->Type == IDBox) || (WorldPart->ParentList->Items[Teller]->Type == IDWall)) && ((WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)))
								{
									CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX+1,WorldPart->PlayFieldY,false);
									break;
								}
							}
						}
					}
					
				}
				if(WorldPart->X > WorldPart->PlayFieldX*TileWidth)
				{
					WorldPart->Xi = -WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 4;
					}
					else
					{
						WorldPart->AnimBase = 0;
						if (WorldPart->ParentList)
						{
							for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
							{
								if(((WorldPart->ParentList->Items[Teller]->Type == IDBox) || (WorldPart->ParentList->Items[Teller]->Type == IDWall)) && ((WorldPart->PlayFieldX == WorldPart->ParentList->Items[Teller]->PlayFieldX )  && (WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)))
								{
									CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX-1,WorldPart->PlayFieldY,false);
									break;
								}
							}
						}
						
					}
				}

				if(WorldPart->Y > WorldPart->PlayFieldY*TileHeight)
				{
					WorldPart->Yi = -WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 12;
					}
					else
					{
						WorldPart->AnimBase = 8;
						if (WorldPart->ParentList)
						{
							for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
							{
								if(((WorldPart->ParentList->Items[Teller]->Type == IDBox) || (WorldPart->ParentList->Items[Teller]->Type == IDWall)) && ((WorldPart->PlayFieldY == WorldPart->ParentList->Items[Teller]->PlayFieldY)  && (WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX)))
								{
									CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX,WorldPart->PlayFieldY-1,false);
									break;
								}						
							}
						}
					}
				}
				if(WorldPart->Y < WorldPart->PlayFieldY*TileHeight)
				{
					WorldPart->Yi = WorldPart->MoveSpeed;
					if(BackWards)
					{
						WorldPart->AnimBase = 8;
					}
					else
					{
						WorldPart->AnimBase = 12;
						if(WorldPart->ParentList)
						{
							for(Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
							{
								if(((WorldPart->ParentList->Items[Teller]->Type == IDBox) || (WorldPart->ParentList->Items[Teller]->Type == IDWall)) && ((WorldPart->ParentList->Items[Teller]->PlayFieldY == WorldPart->PlayFieldY)  && (WorldPart->ParentList->Items[Teller]->PlayFieldX == WorldPart->PlayFieldX )))
								{
									CWorldPart_MoveTo(WorldPart->ParentList->Items[Teller], WorldPart->PlayFieldX,WorldPart->PlayFieldY+1,false);
									break;
								}
							}
						}
					}
				}
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MOVE],0);
				WorldPart->IsMoving = true;
			}
			else
			{
				WorldPart->AnimPhases = 0;
				WorldPart->AnimCounter = 0;

				if (PlayFieldXin > WorldPart->PlayFieldX)
				{
					WorldPart->AnimBase= 4;
				}
				if (PlayFieldXin < WorldPart->PlayFieldX)
				{
					WorldPart->AnimBase = 0;
				}
				if (PlayFieldYin > WorldPart->PlayFieldY)
				{
					WorldPart->AnimBase = 12;
				}
				if (PlayFieldYin < WorldPart->PlayFieldY)
				{
					WorldPart->AnimBase = 8;
				}
				WorldPart->AnimPhase = WorldPart->AnimBase + WorldPart->AnimCounter;
				WorldPart->AnimDelayCounter++;
				if (WorldPart->AnimDelayCounter == WorldPart->AnimDelay)
				{
					WorldPart->AnimDelayCounter = 0;
					WorldPart->AnimCounter++;
					if (WorldPart->AnimCounter >= WorldPart->AnimPhases)
						WorldPart->AnimCounter = 0;
				}
			}
		}
	}
	else
	{
		if(!WorldPart->IsMoving)
		{
			if((PlayFieldXin != WorldPart->PlayFieldX) || (PlayFieldYin != WorldPart->PlayFieldY))
				if(CWorldPart_CanMoveTo(WorldPart,PlayFieldXin,PlayFieldYin) || BackWards)
				{
					if(WorldPart->Type == IDBox)
					{
						if(WorldPart->ParentList)
						{
							if(BackWards)
								WorldPart->ParentList->Pushes--;
							else
								WorldPart->ParentList->Pushes++;
						}
					}
					WorldPart->PlayFieldX = PlayFieldXin;
					WorldPart->PlayFieldY = PlayFieldYin;
					if(WorldPart->X < WorldPart->PlayFieldX*TileWidth)
						WorldPart->Xi = WorldPart->MoveSpeed;
					if(WorldPart->X > WorldPart->PlayFieldX*TileWidth)
						WorldPart->Xi = -WorldPart->MoveSpeed;
					if(WorldPart->Y > WorldPart->PlayFieldY*TileHeight)
						WorldPart->Yi = -WorldPart->MoveSpeed;
					if(WorldPart->Y < WorldPart->PlayFieldY*TileHeight)
						WorldPart->Yi = WorldPart->MoveSpeed;
					WorldPart->IsMoving = true;
					CWorldPart_Event_LeaveCurrentSpot(WorldPart);
				}
		}
	}
}

bool CWorldPart_CanMoveTo(CWorldPart* WorldPart, const int PlayFieldXin,const int PlayFieldYin) 
{
	bool Result = true;
	int Teller;
	if(WorldPart->Type == IDBox)
	{
		if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
		{
			if (WorldPart->ParentList)
			{
				for (Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
					if((WorldPart->ParentList->Items[Teller]->Type == IDWall) || (WorldPart->ParentList->Items[Teller]->Type == IDBox))
						if((WorldPart->ParentList->Items[Teller]->PlayFieldX == PlayFieldXin) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == PlayFieldYin))
						{
							Result = false;
							break;
						}
			}
		}
		else
			Result = false;
		return Result;

	}
	else
	{
		if(WorldPart->Type == IDPlayer)
		{
			if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
			{
				if (WorldPart->ParentList)
				{
					for (Teller=0;Teller<WorldPart->ParentList->ItemCount;Teller++)
						if((WorldPart->ParentList->Items[Teller]->PlayFieldX == PlayFieldXin) && (WorldPart->ParentList->Items[Teller]->PlayFieldY == PlayFieldYin))
						{
							if((WorldPart->ParentList->Items[Teller]->Type == IDWall))
							{
								Result = false;
								break;
							}
							if((WorldPart->ParentList->Items[Teller]->Type == IDBox))
							{
								if (WorldPart->PlayFieldX > PlayFieldXin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin-1,PlayFieldYin);
								}
								if (WorldPart->PlayFieldX < PlayFieldXin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin+1,PlayFieldYin);
								}
								if (WorldPart->PlayFieldY > PlayFieldYin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller], PlayFieldXin,PlayFieldYin-1);
								}
								if (WorldPart->PlayFieldY < PlayFieldYin)
								{
									Result = CWorldPart_CanMoveTo(WorldPart->ParentList->Items[Teller],PlayFieldXin,PlayFieldYin+1);
								}
								break;
							}

						}
				}
			}
			else
				Result = false;
			return Result;

		}
		else
			return false;
	}
}

void CWorldPart_Move(CWorldPart* WorldPart)
{
	if (!WorldPart->FirstArriveEventFired)
	{
		CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
		WorldPart->FirstArriveEventFired=true;
	}
	if (WorldPart->IsMoving)
	{
		if (WorldPart->MoveDelayCounter == WorldPart->MoveDelay)
		{
			WorldPart->X += WorldPart->Xi;
			WorldPart->Y += WorldPart->Yi;
			if ((WorldPart->X == WorldPart->PlayFieldX * TileWidth) && (WorldPart->Y == WorldPart->PlayFieldY * TileHeight))
			{
				WorldPart->IsMoving = false;
				WorldPart->Xi = 0;
				WorldPart->Yi = 0;
				CWorldPart_Event_ArrivedOnNewSpot(WorldPart);
			}
			WorldPart->MoveDelayCounter = -1;
		}
		WorldPart->MoveDelayCounter++;
	}
}

void CWorldPart_Draw(CWorldPart* WorldPart, SDL_Surface* Surface)
{

	if (WorldPart->Image)
	{
		CWorldPart_Event_BeforeDraw(WorldPart);
		SDL_Rect SrcRect,DstRect;
		SrcRect.x = WorldPart->AnimPhase * TileWidth;
		SrcRect.y = 0;
		SrcRect.w = TileWidth;
		SrcRect.h = TileHeight;
		DstRect.x = WorldPart->X + 8; // + 8 because 640/24 is not possible
		DstRect.y = WorldPart->Y;
		DstRect.w = TileWidth;
		DstRect.h = TileHeight;
		SDL_BlitSurface(WorldPart->Image,&SrcRect,Surface,&DstRect);
	}
	if (WorldPart->Selected)
	{
		boxRGBA(Surface,WorldPart->X+8,WorldPart->Y,WorldPart->X+8+TileWidth-1,WorldPart->Y+TileHeight-1,0,0,200,125);
		rectangleRGBA(Surface,WorldPart->X+8,WorldPart->Y,WorldPart->X+8+TileWidth-1,WorldPart->Y+TileHeight-1,0,0,255,125);
	}

}

void CWorldPart_Destroy(CWorldPart* WorldPart)
{
	if(WorldPart)
	{
		if(WorldPart->BHistory)
			CHistory_Destroy(WorldPart->History);
	}
}
