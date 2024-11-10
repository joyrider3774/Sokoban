#ifndef CLEVELPACKFILE_H
#define CLEVELPACKFILE_H

#include <stdbool.h>

#define MAXLEVELTEXTSIZE 500000
#define MAXLEVELS 2250
#define MAXITEMCOUNT 50*50

#define LPWall '#'
#define LPSpot '.'
#define LPPlayer '@'
#define LPBox '$'
#define LPPlayerOnSpot '+'
#define LPBoxOnSpot '*'


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
	int width;
	int height;
	int parts;
	char author[100];
	char comments[1000];
	char title[100];

};

typedef struct CLevelPackFile CLevelPackFile;
struct CLevelPackFile
{
	LevelPart Levels[MAXLEVELS][MAXITEMCOUNT];
	LevelMeta LevelsMeta[MAXLEVELS];
	char author[250];
	char set[250];
	int LevelCount;
	bool Loaded;
};

CLevelPackFile* CLevelPackFile_Create();
void CLevelPackFile_Destroy(CLevelPackFile* LevelPackFile);
bool CLevelPackFile_parseText(CLevelPackFile* LevelPackFile, char* text, int maxWidth, int maxHeight, bool MetaOnly);
bool CLevelPackFile_loadFile(CLevelPackFile* LevelPackFile, char* filename, int maxWidth, int maxHeight, bool MetaOnly);

#endif