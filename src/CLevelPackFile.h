#ifndef CLEVELPACKFILE_H
#define CLEVELPACKFILE_H

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

class CLevelPackFile
{
	public:
		LevelPart Levels[MAXLEVELS][MAXITEMCOUNT];
		LevelMeta LevelsMeta[MAXLEVELS];
		char author[250];
		char set[250];
		int LevelCount;
		bool Loaded;
		CLevelPackFile();
		~CLevelPackFile();
		bool parseText(char* text, int maxWidth, int maxHeight, bool MetaOnly);
		bool loadFile(char* filename, int maxWidth, int maxHeight, bool MetaOnly);
};

#endif