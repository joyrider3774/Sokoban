#include <stdio.h>
#include <ctype.h>
#include <cstring>
#include "CLevelPackFile.h"
#include "Defines.h"

CLevelPackFile::CLevelPackFile()
{
	Loaded = false;
	LevelCount = 0;
	memset(author, 0, 250);
	memset(set, 0, 250);
}

CLevelPackFile::~CLevelPackFile()
{

}

bool CLevelPackFile::loadFile(char* filename, int maxWidth, int maxHeight, bool MetaOnly)
{
	bool Result = false;
	LevelCount = 0;
	Loaded = false;
	memset(author, 0, 250);
	memset(set, 0, 250);
	FILE* Fp = fopen(filename,"rb");
	if(Fp)
	{
		fseek (Fp , 0 , SEEK_END);
		long FileSize = ftell (Fp);
		rewind (Fp);
		char* tmp = new char[FileSize + 2];
		fread(tmp,1,FileSize,Fp);		
		tmp[FileSize] = '\0';
		Result = parseText(tmp, maxWidth, maxHeight, MetaOnly);		
		delete[] tmp;
		fclose(Fp);
		Loaded = true;
	}
	return Result;
}

bool CLevelPackFile::parseText(char* text, int maxWidth, int maxHeight, bool MetaOnly)
{
	char line[2000] = "";
	char levelField[100] = "";
	char levelFieldValue[2000] = "";
	int linepos;
	char* pchar = text;
	char* pdoublepoint, *pset, *pauthor;
	int y;
	bool inlevel = false;
	LevelCount = 0;
	while(*pchar != '\0')
	{
		linepos = 0;
		while((*pchar != '\n') && (*pchar != '\0'))
		{
			if(*pchar != '\r')
				line[linepos++] = tolower(*pchar);
			pchar++;			
		}

		if(*pchar == '\0')
			break;

		pchar++;

		line[linepos] = '\0';
		char* pline = line;	

		if(LevelCount == 0)
		{
			pset = strstr(line, "set:");
			if(pset)
			{
				pset+= 4;
				while(*pset == ' ')
					pset++;
				strncpy(set, pset, strlen(pset));
			}

			pauthor = strstr(line, "author:");
			if(pauthor)
			{
				pauthor+= 7;
				while(*pauthor == ' ')
					pauthor++;
				strncpy(author, pauthor, strlen(pauthor));
			}
		}

		//found double point while in a level start a metadata field
		pdoublepoint = strstr(line, ":");			
		if(inlevel && pdoublepoint)
		{
			if(strlen(levelField) > 0)
			{
				char* ptmp = levelFieldValue;
				while(*ptmp == ' ')
					ptmp++;
				if (strcasecmp(levelField, "title") == 0)
					strcpy(LevelsMeta[LevelCount].title, ptmp);

				if (strcasecmp(levelField, "author") == 0)
					strcpy(LevelsMeta[LevelCount].author, ptmp);
			
				if (strcasecmp(levelField, "comment") == 0)
					strcpy(LevelsMeta[LevelCount].comments, ptmp);
			}	
			memset(levelFieldValue, 0, 2000);
			memset(levelField, 0, 100);
			strncpy(levelField, line, pdoublepoint - line);
			strncpy(levelFieldValue, pdoublepoint +1, strlen(line));
			continue;
		}
		
		//we are in a level but found no empty line and no doublepoint then we are then in a multiline metadata field just append its value
		if(inlevel && !(*pline == '\0') && !pdoublepoint && (levelField[0] != '\0'))
		{
			if(strlen(levelFieldValue) > 0)
				strcat(levelFieldValue, "\n");
			strcat(levelFieldValue, line);
			continue;
		}

		//we are in a level and found a empty line then assume level end
		if(inlevel && (*pline == '\0'))
		{
			if(strlen(levelField) > 0)
			{
				char* ptmp = levelFieldValue;
				while(*ptmp == ' ')
					ptmp++;
				if (strcasecmp(levelField, "title") == 0)
					strcpy(LevelsMeta[LevelCount].title, ptmp);

				if (strcasecmp(levelField, "author") == 0)
					strcpy(LevelsMeta[LevelCount].author, ptmp);
			
				if (strcasecmp(levelField, "comment") == 0)
					strcpy(LevelsMeta[LevelCount].comments, ptmp);
			}
			//clear them for if condition above conerning level start
			memset(levelFieldValue, 0, 2000);
			memset(levelField, 0, 100);
			inlevel = false;
			if((LevelsMeta[LevelCount].width <= maxWidth) && (LevelsMeta[LevelCount].height <= maxHeight))
				LevelCount++;
			continue;
		}

		//we are not in a level and found a wall and no doublepoint and we are not in a levelfield then assume levelstart
		if (!inlevel && (strchr(line, LPWall)) && !pdoublepoint && (levelField[0] == '\0'))
		{
			if (MetaOnly)
				return true;
			inlevel=true;
			y = 0;
			LevelsMeta[LevelCount].width = 0;
			LevelsMeta[LevelCount].height = 0;
			memset(LevelsMeta[LevelCount].author, 0, 100);
			memset(LevelsMeta[LevelCount].title, 0, 100);
			memset(LevelsMeta[LevelCount].author, 0, 100);
			LevelsMeta[LevelCount].parts = 0;
		}

		//we are in level and not in a level meta field
		if(inlevel && (levelField[0] == '\0'))
		{
			int linelen = strlen(line);
			for(int x = 0; x < linelen; x++)
			{
				switch(line[x])
				{
					case LPBox:
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDBox;
						LevelsMeta[LevelCount].parts++;
						break;
					case LPPlayer:
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDPlayer;
						LevelsMeta[LevelCount].parts++;
						break;
					case LPBoxOnSpot:
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDSpot;
						LevelsMeta[LevelCount].parts++;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDBox;
						LevelsMeta[LevelCount].parts++;
						break;
					case LPSpot:
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDSpot;
						LevelsMeta[LevelCount].parts++;
						break;
					case LPPlayerOnSpot:
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDSpot;
						LevelsMeta[LevelCount].parts++;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDPlayer;
						LevelsMeta[LevelCount].parts++;
						break;
					case LPWall:
						if(LevelsMeta[LevelCount].width < x+1)
							LevelsMeta[LevelCount].width = x+1;
						if(LevelsMeta[LevelCount].height < y+1)
							LevelsMeta[LevelCount].height = y+1;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].x = x;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].y = y;
						Levels[LevelCount][LevelsMeta[LevelCount].parts].id = IDWall;
						LevelsMeta[LevelCount].parts++;
						break;
				}
			}
			y++;
		}
	}
	return LevelCount > 0;
}

