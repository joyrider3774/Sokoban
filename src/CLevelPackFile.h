#ifndef CLEVELPACKFILE_H
#define CLEVELPACKFILE_H

#include "Defines.h"

#define MAXLEVELTEXTSIZE 500000

#define MAXLEVELS 2250
#define MAXITEMCOUNT (NrOfCols*NrOfRows) + 2

#define MAXCOMMENTLEN 150
#define MAXSETLEN 50
#define MAXAUTHORLEN 75
#define MAXTITLELEN 50
#define MAXLEVELFIELDDATALEN 2000
#define MAXLEVELFIELDLEN 100
#define MAXLINELEN 2000

#define LPWall 35          //'#' 
#define LPSpot 46          //'.'
#define LPPlayer 64        //'@'
#define LPBox 36           //'$'
#define LPPlayerOnSpot 43  //'+'
#define LPBoxOnSpot 42     //'*'
#define LPFloor 32         //' '

typedef struct LevelPart LevelPart;
struct LevelPart
{
	int id;
	int x;
	int y;
};

typedef struct LevelMeta LevelMeta;
struct LevelMeta
{
	int minx;
	int miny;
	int maxx;
	int maxy;
	int parts;
	char author[MAXAUTHORLEN];
	char comments[MAXCOMMENTLEN];
	char title[MAXTITLELEN];

};

class CLevelPackFile
{
	public:
		LevelPart Levels[MAXLEVELS][MAXITEMCOUNT];
		LevelMeta LevelsMeta[MAXLEVELS];
		char author[MAXAUTHORLEN];
		char set[MAXSETLEN];
		int LevelCount;
		bool Loaded;
		CLevelPackFile();
		~CLevelPackFile();
		bool parseText(char* text, int maxWidth, int maxHeight, bool MetaOnly);
		bool loadFile(char* filename, int maxWidth, int maxHeight, bool MetaOnly);
};

#endif