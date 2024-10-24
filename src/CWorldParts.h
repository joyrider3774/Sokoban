#ifndef CWORLDPARTS_H
#define CWORLDPARTS_H

#include "CWorldPart.h"
#include "CWorldParts.h"

class CWorldPart;

class CWorldParts
{
	private:
		void Sort();
		bool DisableSorting;
	public:
		CWorldPart *Items[NrOfRows*NrOfCols*3];
		int ItemCount;
		CWorldParts();
		bool CenterLevel();
		void Add(CWorldPart *WorldPart);
		void Move();
		void HistoryAdd();
		void HistoryGoBack();
		void Draw(SDL_Surface *Surface);
		void Remove(int PlayFieldXin,int PlayFieldYin);
		void Remove(int PlayFieldXin,int PlayFieldYin,int Type);
		void RemoveAll();
		void Save(char *Filename);
		void Load(char *Filename, bool doCenterLevel);
		~CWorldParts();
};

#endif