#ifndef CWORLDPARTS_H
#define CWORLDPARTS_H

#include "CWorldPart.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"

class CWorldPart;

class CWorldParts
{
	private:
		void Sort();
		bool DisableSorting;
	public:
		CWorldPart *Items[NrOfRows*NrOfCols*3];
		CWorldPart *Player;
		int Pushes, Moves;
		bool isLevelPackFileLevel;
		int ItemCount;
		CWorldParts();
		bool CenterLevel();
		void Add(CWorldPart *WorldPart);
		void Move();
		void HistoryAdd();
		void HistoryGoBack();
		void Draw();
		void Remove(int PlayFieldXin,int PlayFieldYin);
		void Remove(int PlayFieldXin,int PlayFieldYin,int Type);
		void RemoveAll();
		void Save(char *Filename);
		void Load(char *Filename, bool doCenterLevel);
		bool LoadFromLevelPackFile(CLevelPackFile* LPFile, int level, bool doCenterLevel);
		bool ItemExists(int PlayFieldXin,int PlayFieldYin, int Type);
		void DrawFloor(CWorldPart* Player);
		~CWorldParts();
};

#endif