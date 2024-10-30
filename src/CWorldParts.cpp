#include <SDL.h>

#include "CWorldParts.h"
#include "CWorldPart.h"
#include "Common.h"
#include "Defines.h"
#include "CLevelPackFile.h"

CWorldParts::CWorldParts()
{
	ItemCount = 0;
	DisableSorting = false;
	Player = NULL;
}

void CWorldParts::RemoveAll()
{
	int Teller;
	for (Teller=0;Teller<ItemCount;Teller++)
	{
		delete Items[Teller];
		Items[Teller] = NULL;
	}
	ItemCount=0;
}
void CWorldParts::Remove(int PlayFieldXin,int PlayFieldYin)
{
	int Teller1,Teller2;
	for (Teller1=0;Teller1<ItemCount;Teller1++)
	{
		if ((Items[Teller1]->GetPlayFieldX() == PlayFieldXin) && (Items[Teller1]->GetPlayFieldY() == PlayFieldYin))
		{
			delete Items[Teller1];
			for (Teller2=Teller1;Teller2<ItemCount-1;Teller2++)
				Items[Teller2] = Items[Teller2+1];
			ItemCount--;
			Teller1--;
		}
	}
}

void CWorldParts::Remove(int PlayFieldXin,int PlayFieldYin,int Type)
{
	int Teller1,Teller2;
	for (Teller1=0;Teller1<ItemCount;Teller1++)
	{
		if ((Items[Teller1]->GetPlayFieldX() == PlayFieldXin) && (Items[Teller1]->GetPlayFieldY() == PlayFieldYin) && (Items[Teller1]->GetType() == Type))
		{
			delete Items[Teller1];
			for (Teller2=Teller1;Teller2<ItemCount-1;Teller2++)
				Items[Teller2] = Items[Teller2+1];
			ItemCount--;
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

	if(aWorldParts->ItemExists(X, Y, IDWall))
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

void  CWorldParts::DrawFloor(SDL_Surface* Surface, CWorldPart* Player)
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

	FloodFill(this, Surface, visited, Player->GetPlayFieldX(), Player->GetPlayFieldY());

    // Free the allocated memory for the visited array
    for (int i = 0; i < NrOfRows; ++i)
        free(visited[i]);
    free(visited);
}


bool CWorldParts::ItemExists(int PlayFieldXin,int PlayFieldYin, int Type)
{
	for (int Teller1=0;Teller1<ItemCount;Teller1++)
	{
		if ((Items[Teller1]->GetPlayFieldX() == PlayFieldXin) && (Items[Teller1]->GetPlayFieldY() == PlayFieldYin) && (Items[Teller1]->GetType() == Type))
			return true;
	}
	return false;
}

void CWorldParts::HistoryAdd()
{
	int Teller;
	for (Teller=0;Teller<ItemCount;Teller++)
		Items[Teller]->HistoryAdd();
}

void CWorldParts::HistoryGoBack()
{
	int Teller;
	for (Teller=0;Teller<ItemCount;Teller++)
		Items[Teller]->HistoryGoBack();
}

void CWorldParts::Add(CWorldPart *WorldPart)
{
	if( ItemCount < NrOfRows*NrOfCols*3 )
	{
		WorldPart->ParentList = this;
		Items[ItemCount] = WorldPart;
		ItemCount++;
		Sort();
	}
}


void CWorldParts::Sort()
{
	int Teller1,Teller2,Index;
	CWorldPart *Part;
	if (!DisableSorting)
	{
		for (Teller1 = 1; Teller1 <ItemCount;Teller1++)
		{
			Index = Items[Teller1]->GetZ();
			Part = Items[Teller1];
			Teller2 = Teller1;
			while ((Teller2 > 0) && (Items[Teller2-1]->GetZ() > Index))
			{
				Items[Teller2] = Items[Teller2 - 1];
				Teller2--;
			}
			Items[Teller2] = Part;
		}
	}

}

void CWorldParts::Save(char *Filename)
{
	FILE *Fp;
	int Teller,BufferPosition=0;
	char *Buffer;
	Buffer = new char[3*ItemCount];
	for (Teller=0;Teller<ItemCount;Teller++)
	{
		Buffer[BufferPosition]= (char) Items[Teller]->GetType();
		Buffer[BufferPosition+1] = (char)Items[Teller]->GetPlayFieldX();
		Buffer[BufferPosition+2]= (char)Items[Teller]->GetPlayFieldY();
		BufferPosition += 3;
	}
	Fp = fopen(Filename,"wb");
	if (Fp)
	{
		fwrite(Buffer,1,3*ItemCount,Fp);
		fclose(Fp);
	}
	delete[] Buffer;
}

bool CWorldParts::LoadFromLevelPackFile(CLevelPackFile* LPFile, int level, bool doCenterLevel)
{
	isLevelPackFileLevel = false;
	RemoveAll();
	if(level <= LPFile->LevelCount)
	{
		isLevelPackFileLevel = true;
		DisableSorting=true;
		Pushes = 0;
		Moves = 0;
		for (int i=0; i< LPFile->LevelsMeta[level-1].parts; i++ )
		{
			int Type = LPFile->Levels[level-1][i].id;
			int X = LPFile->Levels[level-1][i].x;
			int Y = LPFile->Levels[level-1][i].y;
			switch(Type)
			{
				case IDPlayer:
					Player = new CPlayer(X,Y);
					Add(Player);
					break;
				case IDBox:
					Add(new CBox(X,Y));
					break;
				case IDSpot:
					Add(new CSpot(X,Y));
					break;
				case IDWall:
					Add(new CWall(X,Y));
					break;
				//case IDFloor:
				//	Add(new CFloor(X,Y));
				//	break;

			}
		}
		DisableSorting=false;
		Sort();
		if(doCenterLevel)
			CenterLevel();
		return true;
	}
	return false;
}

void CWorldParts::Load(char *Filename, bool doCenterLevel)
{
	isLevelPackFileLevel = false;
	int X,Y,Type;
	FILE *Fp;
	int BufferPosition=0;
	long FileSize;
	char *Buffer;
	Fp = fopen(Filename,"rb");
	if(Fp)
	{
		RemoveAll();
		DisableSorting=true;
		Pushes = 0;
		Moves = 0;
		fseek (Fp , 0 , SEEK_END);
  		FileSize = ftell (Fp);
  		rewind (Fp);
		Buffer = new char[FileSize];
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
					Player = new CPlayer(X,Y);
					Add(Player);
					break;
				case IDBox:
					Add(new CBox(X,Y));
					break;
				case IDSpot:
					Add(new CSpot(X,Y));
					break;
				case IDWall:
					Add(new CWall(X,Y));
					break;
				//case IDFloor:
				//	Add(new CFloor(X,Y));
				//	break;

			}
		}
		delete[] Buffer;
		fclose(Fp);
		DisableSorting=false;
		Sort();
		if(doCenterLevel)
			CenterLevel();
	}

}

bool CWorldParts::CenterLevel()
{
	int MinX = NrOfCols-1;
	int MinY = NrOfRows-1;
	int MaxX = 0;
	int MaxY = 0;
	for (int Teller = 0;Teller<ItemCount;Teller++)
	{
		if (Items[Teller]->GetPlayFieldX() < MinX)
			MinX = Items[Teller]->GetPlayFieldX();
		if (Items[Teller]->GetPlayFieldY() < MinY)
			MinY = Items[Teller]->GetPlayFieldY();
		if (Items[Teller]->GetPlayFieldX() > MaxX)
			MaxX = Items[Teller]->GetPlayFieldX();
		if (Items[Teller]->GetPlayFieldY() > MaxY)
			MaxY = WorldParts.Items[Teller]->GetPlayFieldY();
	}
	int Xi = ((NrOfCols-1) / 2) - (MaxX + MinX) / 2;
	int Yi = ((NrOfRows-1) / 2) - (MaxY + MinY) / 2;
	for (int Teller = 0;Teller<WorldParts.ItemCount;Teller++)
	{
		Items[Teller]->SetPosition(Items[Teller]->GetPlayFieldX() + Xi,Items[Teller]->GetPlayFieldY() + Yi);
	}
	if (Xi != 0 || Yi!=0)
		return true;
	return false;
}

void CWorldParts::Move()
{
	int Teller;
	for (Teller=0;Teller<ItemCount;Teller++)
		Items[Teller]->Move();
}

void CWorldParts::Draw(SDL_Surface *Surface)
{
	int Teller;
	for (Teller=0;Teller<ItemCount;Teller++)
		Items[Teller]->Draw(Surface);
}

CWorldParts::~CWorldParts()
{
	int Teller;
	for (Teller=0;Teller<ItemCount;Teller++)
	{
		delete Items[Teller];
		Items[Teller] = NULL;
	}
}
