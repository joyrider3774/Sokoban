#include <SDL.h>

#include "CWorldParts.h"
#include "CWorldPart.h"
#include <stdbool.h>
#include "Common.h"
#include "Defines.h"
#include "CLevelPackFile.h"

CWorldParts* CWorldParts_Create()
{
	CWorldParts* Result = (CWorldParts*) malloc(sizeof(CWorldParts));
	Result->ItemCount = 0;
	Result->DisableSorting = false;
	Result->Player = NULL;
	return Result;
}

void CWorldParts_RemoveAll(CWorldParts* WorldParts)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		CWorldPart_Destroy(WorldParts->Items[Teller]);
		WorldParts->Items[Teller] = NULL;
	}
	WorldParts->ItemCount=0;
}
void CWorldParts_Remove(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin)
{
	int Teller1,Teller2;
	for (Teller1=0;Teller1<WorldParts->ItemCount;Teller1++)
	{
		if ((WorldParts->Items[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller1]->PlayFieldY == PlayFieldYin))
		{
			CWorldPart_Destroy(WorldParts->Items[Teller1]);
			for (Teller2=Teller1;Teller2<WorldParts->ItemCount-1;Teller2++)
				WorldParts->Items[Teller2] = WorldParts->Items[Teller2+1];
			WorldParts->ItemCount--;
			Teller1--;
		}
	}
}

void CWorldParts_RemoveType(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin,int Type)
{
	int Teller1,Teller2;
	for (Teller1=0;Teller1<WorldParts->ItemCount;Teller1++)
	{
		if ((WorldParts->Items[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller1]->PlayFieldY == PlayFieldYin) && (WorldParts->Items[Teller1]->Type == Type))
		{
			CWorldPart_Destroy(WorldParts->Items[Teller1]);
			for (Teller2=Teller1;Teller2<WorldParts->ItemCount-1;Teller2++)
				WorldParts->Items[Teller2] = WorldParts->Items[Teller2+1];
			WorldParts->ItemCount--;
			Teller1--;
		}
	}
}


// Recursive Floodfill function
void FloodFill(CWorldParts *aWorldParts, SDL_Surface* Surface, bool **visited, int X, int Y)
{
    // Check bounds and whether the tile has been visited
    if (X < 0 || X >= NrOfCols || Y < 0 || Y >= NrOfRows || visited[Y][X])
	{
        return;
	}

	if(CWorldParts_ItemExists(aWorldParts, X, Y, IDWall))
	{
        return;
	}

    // Mark the tile as visited
    visited[Y][X] = true;

	SDL_Rect Dest;

	Dest.x = (X * TileWidth) + 8; // + 8 because 640/24 is not possible
	Dest.y = (Y * TileWidth);
	Dest.w = TileWidth;
	Dest.h = TileHeight;
    // Draw the floor tile
    SDL_BlitSurface(IMGFloor, NULL, Surface, &Dest );

    // Recur for neighboring tiles
    FloodFill(aWorldParts, Surface, visited, X + 1, Y);
    FloodFill(aWorldParts, Surface, visited, X - 1, Y);
    FloodFill(aWorldParts, Surface, visited, X, Y + 1);
    FloodFill(aWorldParts, Surface, visited, X, Y - 1);
}

void  CWorldParts_DrawFloor(CWorldParts* WorldParts, SDL_Surface* Surface, CWorldPart* Player)
{
	if(!Player)
		return;
    // Allocate memory for the visited array using malloc
    bool **visited = (bool**)malloc(NrOfRows * sizeof(bool*));
    for (int i = 0; i < NrOfRows; ++i)
    {
        visited[i] = (bool*)malloc(NrOfCols * sizeof(bool));
        for (int j = 0; j < NrOfCols; ++j)
            visited[i][j] = false; // Initialize the array to false
    }

	FloodFill(WorldParts, Surface, visited, Player->PlayFieldX, Player->PlayFieldY);

    // Free the allocated memory for the visited array
    for (int i = 0; i < NrOfRows; ++i)
        free(visited[i]);
    free(visited);
}


bool CWorldParts_ItemExists(CWorldParts* WorldParts, int PlayFieldXin,int PlayFieldYin, int Type)
{
	for (int Teller1=0;Teller1<WorldParts->ItemCount;Teller1++)
	{
		if ((WorldParts->Items[Teller1]->PlayFieldX == PlayFieldXin) && (WorldParts->Items[Teller1]->PlayFieldY == PlayFieldYin) && (WorldParts->Items[Teller1]->Type == Type))
			return true;
	}
	return false;
}

void CWorldParts_HistoryAdd(CWorldParts* WorldParts)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		CWorldPart_HistoryAdd(WorldParts->Items[Teller]);
}

void CWorldParts_HistoryGoBack(CWorldParts* WorldParts)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		CWorldPart_HistoryGoBack(WorldParts->Items[Teller]);
}

void CWorldParts_Add(CWorldParts* WorldParts, CWorldPart *WorldPart)
{
	if( WorldParts->ItemCount < NrOfRows*NrOfCols*3 )
	{
		WorldPart->ParentList = WorldParts;
		WorldParts->Items[WorldParts->ItemCount] = WorldPart;
		WorldParts->ItemCount++;
		CWorldParts_Sort(WorldParts);
	}
}


void CWorldParts_Sort(CWorldParts* WorldParts)
{
	int Teller1,Teller2,Index;
	CWorldPart *Part;
	if (!WorldParts->DisableSorting)
	{
		for (Teller1 = 1; Teller1 <WorldParts->ItemCount;Teller1++)
		{
			Index = WorldParts->Items[Teller1]->Z;
			Part = WorldParts->Items[Teller1];
			Teller2 = Teller1;
			while ((Teller2 > 0) && (WorldParts->Items[Teller2-1]->Z > Index))
			{
				WorldParts->Items[Teller2] = WorldParts->Items[Teller2 - 1];
				Teller2--;
			}
			WorldParts->Items[Teller2] = Part;
		}
	}

}

void CWorldParts_Save(CWorldParts* WorldParts, char *Filename)
{
	FILE *Fp;
	int Teller,BufferPosition=0;
	char Buffer[3*WorldParts->ItemCount];
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
	{
		Buffer[BufferPosition]= (char) WorldParts->Items[Teller]->Type;
		Buffer[BufferPosition+1] = (char)WorldParts->Items[Teller]->PlayFieldX;
		Buffer[BufferPosition+2]= (char)WorldParts->Items[Teller]->PlayFieldY;
		BufferPosition += 3;
	}
	Fp = fopen(Filename,"wb");
	if (Fp)
	{
		fwrite(Buffer,1,3*WorldParts->ItemCount,Fp);
		fclose(Fp);
	}
}

bool CWorldParts_LoadFromLevelPackFile(CWorldParts* WorldParts, CLevelPackFile* LPFile, int level, bool doCenterLevel)
{
	WorldParts->isLevelPackFileLevel = false;
	CWorldParts_RemoveAll(WorldParts);
	if(level <= LPFile->LevelCount)
	{
		WorldParts->isLevelPackFileLevel = true;
		WorldParts->DisableSorting=true;
		WorldParts->Pushes = 0;
		WorldParts->Moves = 0;
		for (int i=0; i< LPFile->LevelsMeta[level-1].parts; i++ )
		{
			int Type = LPFile->Levels[level-1][i].id;
			int X = LPFile->Levels[level-1][i].x;
			int Y = LPFile->Levels[level-1][i].y;
			switch(Type)
			{
				case IDPlayer:
					WorldParts->Player = CWorldPart_Create(X,Y,true,Type);
					CWorldParts_Add(WorldParts, WorldParts->Player);
					break;
				case IDBox:
					CWorldParts_Add(WorldParts, CWorldPart_Create(X,Y,true,Type));
					break;
				case IDSpot:
					CWorldParts_Add(WorldParts, CWorldPart_Create(X,Y,false,Type));
					break;
				case IDWall:
					CWorldParts_Add(WorldParts, CWorldPart_Create(X,Y,false,Type));
					break;
			}
		}
		WorldParts->DisableSorting=false;
		CWorldParts_Sort(WorldParts);
		if(doCenterLevel)
			CWorldParts_CenterLevel(WorldParts);
		return true;
	}
	return false;
}

void CWorldParts_Load(CWorldParts* WorldParts, char *Filename, bool doCenterLevel)
{
	WorldParts->isLevelPackFileLevel = false;
	int X,Y,Type;
	FILE *Fp;
	int BufferPosition=0;
	long FileSize;
	char *Buffer;
	Fp = fopen(Filename,"rb");
	if(Fp)
	{
		CWorldParts_RemoveAll(WorldParts);
		WorldParts->DisableSorting=true;
		WorldParts->Pushes = 0;
		WorldParts->Moves = 0;
		fseek (Fp , 0 , SEEK_END);
  		FileSize = ftell (Fp);
  		rewind (Fp);
		Buffer = (char*) malloc(sizeof(char) *FileSize);
		fread(Buffer,1,FileSize,Fp);
		while(BufferPosition < FileSize)
		{
			Type = (int)Buffer[BufferPosition];
			X =(int)Buffer[BufferPosition+1];
			Y = (int)Buffer[BufferPosition+2];
			BufferPosition +=3;
			switch(Type)
			{
				case IDPlayer:
					WorldParts->Player = CWorldPart_Create(X,Y,true,Type);
					CWorldParts_Add(WorldParts, WorldParts->Player);
					break;
				case IDBox:
					CWorldParts_Add(WorldParts, CWorldPart_Create(X,Y,true,Type));
					break;
				case IDSpot:
					CWorldParts_Add(WorldParts, CWorldPart_Create(X,Y,false,Type));
					break;
				case IDWall:
					CWorldParts_Add(WorldParts, CWorldPart_Create(X,Y,false,Type));
					break;
			}
		}
		fclose(Fp);
		free(Buffer);
		WorldParts->DisableSorting=false;
		CWorldParts_Sort(WorldParts);
		if(doCenterLevel)
			CWorldParts_CenterLevel(WorldParts);
	}

}

bool CWorldParts_CenterLevel(CWorldParts* WorldParts)
{
	int MinX = NrOfCols-1;
	int MinY = NrOfRows-1;
	int MaxX = 0;
	int MaxY = 0;
	for (int Teller = 0;Teller<WorldParts->ItemCount;Teller++)
	{
		if (WorldParts->Items[Teller]->PlayFieldX < MinX)
			MinX = WorldParts->Items[Teller]->PlayFieldX;
		if (WorldParts->Items[Teller]->PlayFieldY < MinY)
			MinY = WorldParts->Items[Teller]->PlayFieldY;
		if (WorldParts->Items[Teller]->PlayFieldX > MaxX)
			MaxX = WorldParts->Items[Teller]->PlayFieldX;
		if (WorldParts->Items[Teller]->PlayFieldY > MaxY)
			MaxY = WorldParts->Items[Teller]->PlayFieldY;
	}
	int Xi = ((NrOfCols-1) / 2) - (MaxX + MinX) / 2;
	int Yi = ((NrOfRows-1) / 2) - (MaxY + MinY) / 2;
	for (int Teller = 0;Teller<WorldParts->ItemCount;Teller++)
	{
		CWorldPart_SetPosition(WorldParts->Items[Teller], WorldParts->Items[Teller]->PlayFieldX + Xi, WorldParts->Items[Teller]->PlayFieldY + Yi);
	}
	if (Xi != 0 || Yi!=0)
		return true;
	return false;
}

void CWorldParts_Move(CWorldParts* WorldParts)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		CWorldPart_Move(WorldParts->Items[Teller]);
}

void CWorldParts_Draw(CWorldParts* WorldParts, SDL_Surface *Surface)
{
	int Teller;
	for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		CWorldPart_Draw(WorldParts->Items[Teller], Surface);
}

void CWorldParts_Destroy(CWorldParts* WorldParts)
{
	if(WorldParts)
	{
		int Teller;
		for (Teller=0;Teller<WorldParts->ItemCount;Teller++)
		{
			CWorldPart_Destroy(WorldParts->Items[Teller]);
			WorldParts->Items[Teller] = NULL;
		}
		free(WorldParts);
		WorldParts = NULL;
	}
}
