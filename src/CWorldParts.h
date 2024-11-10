#ifndef CWORLDPARTS_H
#define CWORLDPARTS_H

#include "CWorldPart.h"
#include "CWorldParts.h"
#include "CLevelPackFile.h"

typedef struct CWorldPart CWorldPart;

typedef struct CWorldParts CWorldParts;

struct CWorldParts
{
	bool DisableSorting;
	CWorldPart *Items[NrOfRows*NrOfCols*3];
	CWorldPart *Player;
	int Pushes, Moves;
	bool isLevelPackFileLevel;
	int ItemCount;
};

CWorldParts* CWorldParts_Create();
void CWorldParts_Destroy(CWorldParts* WorldParts);
bool CWorldParts_CenterLevel(CWorldParts* WorldParts);
void CWorldParts_Add(CWorldParts* WorldParts, CWorldPart *WorldPart);
void CWorldParts_Move(CWorldParts* WorldParts);
void CWorldParts_HistoryAdd(CWorldParts* WorldParts);
void CWorldParts_HistoryGoBack(CWorldParts* WorldParts);
void CWorldParts_Draw(CWorldParts* WorldParts, SDL_Surface *Surface);
void CWorldParts_Remove(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin);
void CWorldParts_RemoveType(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin,int Type);
void CWorldParts_RemoveAll(CWorldParts* WorldParts);
void CWorldParts_Save(CWorldParts* WorldParts, char *Filename);
void CWorldParts_Load(CWorldParts* WorldParts, char *Filename, bool doCenterLevel);
bool CWorldParts_LoadFromLevelPackFile(CWorldParts* WorldParts, CLevelPackFile* LPFile, int level, bool doCenterLevel);
bool CWorldParts_ItemExists(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin, int Type);
void CWorldParts_DrawFloor(CWorldParts* WorldParts, SDL_Surface* Surface, CWorldPart* Player);
void CWorldParts_Sort(CWorldParts* WorldParts);

#endif