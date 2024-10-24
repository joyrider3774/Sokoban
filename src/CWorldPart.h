#ifndef CWORLDPART_H
#define CWORLDPART_H

#include <SDL.h>
#include "CHistory.h"
#include "CWorldParts.h"

class CWorldParts;

class CWorldPart
{
	private:
		int MoveDelayCounter;
		bool FirstArriveEventFired;
	
	protected:
		int Type,MoveSpeed,MoveDelay,Xi,Yi,X,Y,AnimPhase,PlayFieldX,PlayFieldY,Z;
		bool BHistory;
		SDL_Surface * Image;

	public:
		CWorldParts *ParentList;
		bool IsMoving;
		CHistory *History;
		bool Selected;
		CWorldPart(const int PlayFieldXin,const int PlayFieldYin,bool CreateHistory);
		int GetType(); 
		int GetX();
		int GetY();
		int GetPlayFieldX();
		int GetPlayFieldY();
		int GetZ();
		int GetAnimPhase();
		bool HasHistory();
		void HistoryAdd();
		void HistoryGoBack();
		void SetAnimPhase(int AnimPhaseIn);
		virtual void Event_ArrivedOnNewSpot();
		virtual void Event_BeforeDraw();
		virtual void Event_LeaveCurrentSpot();
		void SetPosition(const int PlayFieldXin,const int PlayFieldYin);
		virtual void MoveTo(const int PlayFieldXin,const int PlayFieldYin,bool BackWards);
		virtual bool CanMoveTo(const int PlayFieldXin,const int PlayFieldYin);
		virtual void Move();
		virtual void Draw(SDL_Surface* Surface);
		virtual ~CWorldPart();
};
 	
class CEmpty : public CWorldPart
{
	public:
		CEmpty(const int PlayFieldXin,const int PlayFieldYin);
};

class CFloor : public CWorldPart
{
	public:
		CFloor(const int PlayFieldXin,const int PlayFieldYin);
};

class CWall : public CWorldPart
{
	public:
 		CWall(const int PlayFieldXin,const int PlayFieldYin);

};

class CBox : public CWorldPart
{
	private:
		void Event_ArrivedOnNewSpot();
		bool CanMoveTo(const int PlayFieldXin,const int PlayFieldYin);
		void Event_LeaveCurrentSpot();
	
	public:
		CBox(const int PlayFieldXin,const int PlayFieldYin);
};

class CSpot : public CWorldPart
{
	private:
 		void Event_ArrivedOnNewSpot();
 	public:
 		CSpot(const int PlayFieldXin,const int PlayFieldYin);
};

class CPlayer : public CWorldPart
{
	private:
 		int AnimCounter,AnimBase,AnimDelay,AnimDelayCounter,AnimPhases;
 		void Event_BeforeDraw();

 	public:
 		bool CanMoveTo(const int PlayFieldXin,const int PlayFieldYin);
 		void MoveTo(const int PlayFieldXin,const int PlayFieldYin,bool BackWards);
 		CPlayer(const int PlayFieldXin,const int PlayFieldYin);
 		void MoveTo(const int PlayFieldXin,const int PlayFieldYin);
};


#endif