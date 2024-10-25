#include <SDL_gfxPrimitives.h>
#include <SDL.h>
#include "CWorldParts.h"
#include "CWorldPart.h"
#include "CHistory.h"
#include "Common.h"

CWorldPart::CWorldPart(const int PlayFieldXin,const int PlayFieldYin,bool CreateHistory)
{
	BHistory = CreateHistory;
	if (BHistory)
		History = new CHistory(this);
	PlayFieldX=PlayFieldXin;
	PlayFieldY=PlayFieldYin;
	Xi=0;
	Yi=0;
	X=PlayFieldXin*TileWidth;
	Y=PlayFieldYin*TileHeight;
	Type=0;
	MoveDelay=0;
	MoveDelayCounter=0;
	IsMoving = false;
	MoveSpeed=0;
	Image = NULL;
	ParentList = 0;
	AnimPhase=0;
	Selected = false;
	FirstArriveEventFired = false;
	Z=0;
}

int CWorldPart::GetType() 
{
	return Type;
}

int CWorldPart::GetX() 
{
	return X;
}

int CWorldPart::GetY() 
{
	return Y;
}

int CWorldPart::GetPlayFieldX() 
{
	return PlayFieldX;
}

int CWorldPart::GetPlayFieldY() 
{
	return PlayFieldY;
}

int CWorldPart::GetZ() 
{
	return Z;
}

int CWorldPart::GetAnimPhase() 
{
	return AnimPhase;
}

bool CWorldPart::HasHistory() 
{ 
	return BHistory;
}

void CWorldPart::HistoryAdd() 
{ 
	if (BHistory) 
		History->Add(PlayFieldX,PlayFieldY); 
}

void CWorldPart::HistoryGoBack() 
{ 
	if(BHistory) 
		History->GoBack();
}

void CWorldPart::SetAnimPhase(int AnimPhaseIn) 
{
	AnimPhase = AnimPhaseIn;
}

void CWorldPart::Event_ArrivedOnNewSpot() 
{
	
}

void CWorldPart::Event_BeforeDraw() 
{

}

void CWorldPart::Event_LeaveCurrentSpot() 
{

}

void CWorldPart::SetPosition(const int PlayFieldXin,const int PlayFieldYin)
{
	if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
	{
		PlayFieldX=PlayFieldXin;
		PlayFieldY=PlayFieldYin;
		X=PlayFieldXin*TileWidth;
		Y=PlayFieldYin*TileHeight;
		Event_ArrivedOnNewSpot();
	}
}

void CWorldPart::MoveTo(const int PlayFieldXin,const int PlayFieldYin,bool BackWards)
{
	if(!IsMoving)
	{
		if((PlayFieldXin != PlayFieldX) || (PlayFieldYin != PlayFieldY))
			if(this->CanMoveTo(PlayFieldXin,PlayFieldYin) || BackWards)
			{
				PlayFieldX = PlayFieldXin;
				PlayFieldY = PlayFieldYin;
				if(X < PlayFieldX*TileWidth)
					Xi = MoveSpeed;
				if(X > PlayFieldX*TileWidth)
					Xi = -MoveSpeed;
				if(Y > PlayFieldY*TileHeight)
					Yi = -MoveSpeed;
				if(Y < PlayFieldY*TileHeight)
					Yi = MoveSpeed;
				IsMoving = true;
				Event_LeaveCurrentSpot();
			}
	}
}

bool CWorldPart::CanMoveTo(const int PlayFieldXin,const int PlayFieldYin) 
{
	return false;
}

void CWorldPart::Move()
{
	if (!FirstArriveEventFired)
	{
		Event_ArrivedOnNewSpot();
		FirstArriveEventFired=true;
	}
	if (IsMoving)
	{
		if (MoveDelayCounter == MoveDelay)
		{
			X += Xi;
			Y += Yi;
			if ((X == PlayFieldX * TileWidth) && (Y == PlayFieldY * TileHeight))
			{
				IsMoving = false;
				Xi = 0;
				Yi = 0;
				Event_ArrivedOnNewSpot();
			}
			MoveDelayCounter = -1;
		}
		MoveDelayCounter++;
	}
}

void CWorldPart::Draw(SDL_Surface* Surface)
{

	if (Image)
	{
		Event_BeforeDraw();
		SDL_Rect SrcRect,DstRect;
		SrcRect.x = AnimPhase * TileWidth;
		SrcRect.y = 0;
		SrcRect.w = TileWidth;
		SrcRect.h = TileHeight;
		DstRect.x = X;
		DstRect.y = Y;
		DstRect.w = TileWidth;
		DstRect.h = TileHeight;
		SDL_BlitSurface(Image,&SrcRect,Surface,&DstRect);
	}
	if (Selected)
	{
		boxRGBA(Surface,X,Y,X+TileWidth-1,Y+TileHeight-1,0,0,200,125);
		rectangleRGBA(Surface,X,Y,X+TileWidth-1,Y+TileHeight-1,0,0,255,125);
	}

}

CWorldPart::~CWorldPart()
{
	if(BHistory)
		delete History;
}


CEmpty::CEmpty(const int PlayFieldXin,const int PlayFieldYin) : CWorldPart(PlayFieldXin,PlayFieldYin,false)
{
	Image = IMGEmpty;
	Type = IDEmpty;
	Z = ZEmpty;
}

CFloor::CFloor(const int PlayFieldXin,const int PlayFieldYin) : CWorldPart(PlayFieldXin,PlayFieldYin,false)
{
	Image = IMGFloor;
	Type = IDFloor;
	Z = ZFloor;
}

CWall::CWall(const int PlayFieldXin,const int PlayFieldYin) : CWorldPart(PlayFieldXin,PlayFieldYin,false)
{
	Image = IMGWall;
	Type = IDWall;
	Z = ZWall;
}



CBox::CBox(const int PlayFieldXin,const int PlayFieldYin) : CWorldPart(PlayFieldXin,PlayFieldYin,true)
{
	Image = IMGBox;
	Type = IDBox;
	MoveDelay = 0;
	MoveSpeed = 2;
	Z = ZBox;
}

void CBox::Event_LeaveCurrentSpot()
{
	if (ParentList)
	{
		int Teller;
		for (Teller=0;Teller< ParentList->ItemCount;Teller++)
		{
			if( ParentList->Items[Teller]->GetType() == IDSpot)
			{
				if ((ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldX) && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldY))
				{
					AnimPhase = 1;
					break;
				}
			}
		}
	}
}

bool CBox::CanMoveTo(const int PlayFieldXin,const int PlayFieldYin)
{
	int Teller;
	bool Result = true;
	if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
	{
		if (ParentList)
		{
			for (Teller=0;Teller<ParentList->ItemCount;Teller++)
				if((ParentList->Items[Teller]->GetType() == IDWall) || (ParentList->Items[Teller]->GetType() == IDBox))
					if((ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldXin) && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldYin))
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

void CBox::Event_ArrivedOnNewSpot()
{
	int Teller;
	AnimPhase = 0;
	if (ParentList)
	{
		for (Teller=0;Teller< ParentList->ItemCount;Teller++)
		{
			if( ParentList->Items[Teller]->GetType() == IDSpot)
			{
				if ((ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldX) && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldY))
				{
					AnimPhase = 1;
					break;
				}
			}
		}
	}
}

CSpot::CSpot(const int PlayFieldXin,const int PlayFieldYin) : CWorldPart(PlayFieldXin,PlayFieldYin,false)
{
	Image = IMGSpot;
	Type = IDSpot;
	Z = ZSpot;
}

void CSpot::Event_ArrivedOnNewSpot()
{
	int Teller;
	if (ParentList)
	{
		for (Teller=0;Teller< ParentList->ItemCount;Teller++)
		{
			if( ParentList->Items[Teller]->GetType() == IDBox)
			{
				if ((ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldX) && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldY))
				{
					ParentList->Items[Teller]->SetAnimPhase(1);
					break;
				}
			}
		}
	}
}

CPlayer::CPlayer(const int PlayFieldXin,const int PlayFieldYin) : CWorldPart(PlayFieldXin,PlayFieldYin,true)
{
	Image=IMGPlayer;
	AnimBase=4;
	AnimPhase=4;
	AnimPhases=4;
	AnimCounter = 1;
	AnimDelay = 8;
	MoveDelay = 0;
	MoveSpeed = 2;
	AnimDelayCounter =0;
	Type = IDPlayer;
	Z = ZPlayer;
}

bool CPlayer::CanMoveTo(const int PlayFieldXin,const int PlayFieldYin)
{
	int Teller;
	bool Result = true;
	if ((PlayFieldXin >= 0) && (PlayFieldXin < NrOfCols) && (PlayFieldYin >= 0) && (PlayFieldYin < NrOfRows))
	{
		if (ParentList)
		{
			for (Teller=0;Teller<ParentList->ItemCount;Teller++)
				if((ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldXin) && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldYin))
				{
					if((ParentList->Items[Teller]->GetType() == IDWall))
					{
						Result = false;
						break;
					}
					if((ParentList->Items[Teller]->GetType() == IDBox))
					{
						if (PlayFieldX > PlayFieldXin)
						{
							Result = ParentList->Items[Teller]->CanMoveTo(PlayFieldXin-1,PlayFieldYin);
						}
						if (PlayFieldX < PlayFieldXin)
						{
							Result = ParentList->Items[Teller]->CanMoveTo(PlayFieldXin+1,PlayFieldYin);
						}
						if (PlayFieldY > PlayFieldYin)
						{
							Result = ParentList->Items[Teller]->CanMoveTo(PlayFieldXin,PlayFieldYin-1);
						}
						if (PlayFieldY < PlayFieldYin)
						{
							Result = ParentList->Items[Teller]->CanMoveTo(PlayFieldXin,PlayFieldYin+1);
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

void CPlayer::Event_BeforeDraw()
{
	if (IsMoving)
	{
		AnimPhase = AnimBase + AnimCounter;
		AnimDelayCounter++;
		if (AnimDelayCounter ==AnimDelay)
		{
			AnimDelayCounter = 0;
			AnimCounter++;
			if (AnimCounter >= AnimPhases)
				AnimCounter = 0;
		}
	}
}

void CPlayer::MoveTo(const int PlayFieldXin,const int PlayFieldYin,bool BackWards)
{
 	int Teller;
 	if(!IsMoving)
	{
		if(this->CanMoveTo(PlayFieldXin,PlayFieldYin) || BackWards)
		{
			AnimPhases = 4;
			PlayFieldX = PlayFieldXin;
			PlayFieldY = PlayFieldYin;
			if(X < PlayFieldX*TileWidth)
			{
				Xi = MoveSpeed;
				if(BackWards)
				{
					AnimBase = 0;
				}
				else
				{
					AnimBase = 4;
					if (ParentList)
					{
						for(Teller=0;Teller<ParentList->ItemCount;Teller++)
						{
							if(((ParentList->Items[Teller]->GetType() == IDBox) || (ParentList->Items[Teller]->GetType() == IDWall)) && ((ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldX) && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldY)))
							{
								ParentList->Items[Teller]->MoveTo(PlayFieldX+1,PlayFieldY,false);
								break;
							}
						}
					}
				}
				
			}
			if(X > PlayFieldX*TileWidth)
			{
				Xi = -MoveSpeed;
				if(BackWards)
				{
					AnimBase = 4;
				}
				else
				{
					AnimBase = 0;
					if (ParentList)
					{
						for(Teller=0;Teller<ParentList->ItemCount;Teller++)
						{
							if(((ParentList->Items[Teller]->GetType() == IDBox) || (ParentList->Items[Teller]->GetType() == IDWall)) && ((PlayFieldX == ParentList->Items[Teller]->GetPlayFieldX() )  && (ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldY)))
							{
								ParentList->Items[Teller]->MoveTo(PlayFieldX-1,PlayFieldY,false);

								break;
							}
						}
					}
					
				}
			}
			if(Y > PlayFieldY*TileHeight)
			{
				Yi = -MoveSpeed;
				if(BackWards)
				{
					AnimBase = 12;
				}
				else
				{
					AnimBase = 8;
					if (ParentList)
					{
						for(Teller=0;Teller<ParentList->ItemCount;Teller++)
						{
							if(((ParentList->Items[Teller]->GetType() == IDBox) || (ParentList->Items[Teller]->GetType() == IDWall)) && ((PlayFieldY == ParentList->Items[Teller]->GetPlayFieldY())  && (ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldX)))
							{
								ParentList->Items[Teller]->MoveTo(PlayFieldX,PlayFieldY-1,false);
								break;
							}						
						}
					}
				}
			}
			if(Y < PlayFieldY*TileHeight)
			{
				Yi = MoveSpeed;
				if(BackWards)
				{
					AnimBase = 8;
				}
				else
				{
					AnimBase = 12;
					if(ParentList)
					{
						for(Teller=0;Teller<ParentList->ItemCount;Teller++)
						{
							if(((ParentList->Items[Teller]->GetType() == IDBox) || (ParentList->Items[Teller]->GetType() == IDWall)) && ((ParentList->Items[Teller]->GetPlayFieldY() == PlayFieldY)  && (ParentList->Items[Teller]->GetPlayFieldX() == PlayFieldX )))
							{
								ParentList->Items[Teller]->MoveTo(PlayFieldX,PlayFieldY+1,false);
								break;
							}
						}
					}
				}
			}
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_MOVE],0);
			IsMoving = true;
		}
		else
		{
			AnimPhases = 0;
			AnimCounter = 0;

			if (PlayFieldXin > PlayFieldX)
			{
				AnimBase= 4;
			}
			if (PlayFieldXin < PlayFieldX)
			{
				AnimBase = 0;
			}
			if (PlayFieldYin > PlayFieldY)
			{
				AnimBase = 12;
			}
			if (PlayFieldYin < PlayFieldY)
			{
				AnimBase = 8;
			}
			AnimPhase = AnimBase + AnimCounter;
			AnimDelayCounter++;
			if (AnimDelayCounter ==AnimDelay)
			{
				AnimDelayCounter = 0;
				AnimCounter++;
				if (AnimCounter >= AnimPhases)
					AnimCounter = 0;
			}
		}
 	}
 }
