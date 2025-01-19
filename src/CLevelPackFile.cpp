#include <stdio.h>
#include <ctype.h>
#include <cstring>
#include <sys/stat.h>
#include "GameFuncs.h"
#include "CLevelPackFile.h"
#include "Defines.h"

CLevelPackFile::CLevelPackFile()
{
	Loaded = false;
	LevelCount = 0;
	memset(author, 0, MAXAUTHORLEN);
	memset(set, 0, MAXSETLEN);
}

CLevelPackFile::~CLevelPackFile()
{

}

bool CLevelPackFile::loadFile(char* filename, int maxWidth, int maxHeight, bool MetaOnly)
{
	bool Result = false;
	LevelCount = 0;
	Loaded = false;
	memset(author, 0, MAXAUTHORLEN);
	memset(set, 0, MAXSETLEN);
    struct stat statbuf;
    stat(filename, &statbuf);
    // test for a regular file
    if (S_ISREG(statbuf.st_mode))
	{
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
	}
	return Result;
}

bool CLevelPackFile::parseText(char* text, int maxWidth, int maxHeight, bool MetaOnly)
{
	char line[MAXLINELEN] = "";
	char levelField[MAXLEVELFIELDLEN] = "";
	char levelFieldValue[MAXLEVELFIELDDATALEN] = "";
	int linepos;
	char* pchar = text;
	char* pdoublepoint, *pset, *pauthor;
	int y;
	bool inlevel = false;
	LevelCount = 0;
	memset(author, 0, MAXAUTHORLEN);
	memset(set, 0, MAXSETLEN);
	LevelMeta* levelMeta = &LevelsMeta[LevelCount];
	while(*pchar)
	{
		linepos = 0;
		while((*pchar != '\n') && (*pchar != '\0'))
		{
			if((*pchar != '\r') && (linepos < MAXLINELEN-1))
			{
				if((*pchar >= 'A') && (*pchar <= 'Z'))
				{
					line[linepos++] = *pchar + 32;
				}
				else
					line[linepos++] = *pchar;
            }
            pchar++;
		}

		if(*pchar == '\0')
			break;

		pchar++;

		line[linepos] = '\0';

		if(LevelCount == 0)
		{
			if(!set[0])
			{
				pset = strstr(line, "set:");
				if(pset)
				{
					pset+= 4;
					while(*pset == ' ')
						pset++;
					strncpy(set, pset, MAXSETLEN-1);
				}
			}

			if(!author[0])
			{
				pauthor = strstr(line, "author:");
				if(pauthor)
				{
					pauthor+= 7;
					while(*pauthor == ' ')
						pauthor++;
					strncpy(author, pauthor, MAXAUTHORLEN-1);
				}
			}
		}

		//found double point while in a level start a metadata field
		pdoublepoint = strstr(line, ":");			
		if(inlevel && pdoublepoint)
		{
			if(levelField[0])
			{
				char* ptmp = levelFieldValue;
				while(*ptmp == ' ')
					ptmp++;
				if (strcmp(levelField, "title") == 0)
				{
					strncpy(levelMeta->title, ptmp, MAXTITLELEN-1);
				}
				else
				{
					if (strcmp(levelField, "author") == 0)
					{
						strncpy(levelMeta->author, ptmp, MAXAUTHORLEN-1);
					}
					else
					{
						if (strcmp(levelField, "comment") == 0)
						{
							strncpy(levelMeta->comments, ptmp, MAXCOMMENTLEN-1);
						}
					}
				}
			}	
			memset(levelFieldValue, 0, MAXLEVELFIELDDATALEN);
			memset(levelField, 0, MAXLEVELFIELDLEN);
			strncpy(levelField, line, pdoublepoint - &line[0]);
			strncpy(levelFieldValue, pdoublepoint +1, MAXLEVELFIELDDATALEN - 1);
			continue;
		}
		
		//we are in a level but found no empty line and no doublepoint then we are then in a multiline metadata field just append its value
		if(inlevel && linepos && !pdoublepoint && (levelField[0]))
		{
			if(levelFieldValue[0])
				strcat(levelFieldValue, "\n");
			strcat(levelFieldValue, line);
			continue;
		}

		//we are in a level and found a empty line then assume level end
		if(inlevel && !linepos)
		{
			if(levelField[0])
			{
				char* ptmp = levelFieldValue;
				while(*ptmp == ' ')
					ptmp++;
				if (strcmp(levelField, "title") == 0)
				{
					strncpy(levelMeta->title, ptmp, MAXTITLELEN-1);
				}
				else
				{
					if (strcmp(levelField, "author") == 0)
					{
						strncpy(levelMeta->author, ptmp, MAXAUTHORLEN-1);
					}
					else
					{
						if (strcmp(levelField, "comment") == 0)
						{
							strncpy(levelMeta->comments, ptmp, MAXCOMMENTLEN-1);
						}
					}
				}
			}
			//clear them for if condition above conerning level start
			memset(levelFieldValue, 0, MAXLEVELFIELDDATALEN);
			memset(levelField, 0, MAXLEVELFIELDLEN);
			inlevel = false;
			if((levelMeta->maxx+1 <= maxWidth) && 
				(levelMeta->maxy+1 <= maxHeight))
			{
				LevelCount++;
				levelMeta = &LevelsMeta[LevelCount];
			}
			continue;
		}

		//we are not in a level and found a wall and no doublepoint and we are not in a levelfield then assume levelstart
		if (!inlevel && !pdoublepoint && (!levelField[0]))
		{
			if (strchr(line, LPWall))
			{
				if (MetaOnly)
					return true;
				inlevel=true;
				y = 0;
				levelMeta->minx = NrOfCols;
				levelMeta->miny = NrOfRows;
				levelMeta->maxx = 0;
				levelMeta->maxy = 0;
				memset(levelMeta->author, 0, MAXAUTHORLEN);
				memset(levelMeta->title, 0, MAXTITLELEN);
				memset(levelMeta->comments, 0, MAXCOMMENTLEN);
				levelMeta->parts = 0;
			}
		}

		//we are in level and not in a level meta field
		if(inlevel && (!levelField[0]))
		{			
			for(int x = 0; x < linepos; x++)
			{
				if (line[x] == LPFloor)
					continue;
				//DON'T EXCEED MAX ITEMCOUNT!
				if(levelMeta->parts+2 >= MAXITEMCOUNT)
				{
					levelMeta->maxx = 1000;
					break;
				}
				LevelPart* levelPart = &(Levels[LevelCount][LevelsMeta[LevelCount].parts]);
				
				switch(line[x])
				{
					case LPWall:
						if(x < levelMeta->minx)
							levelMeta->minx = x;
						if(x > levelMeta->maxx)
							levelMeta->maxx = x;
						if(y < levelMeta->miny)
							levelMeta->miny = y;
						if(y > levelMeta->maxy)
							levelMeta->maxy = y;
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDWall;
						levelMeta->parts++;
						break;
					case LPBox:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBox;
						levelMeta->parts++;
						break;
					case LPBoxOnSpot:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDSpot;
						levelMeta->parts++;
						
						levelPart = &Levels[LevelCount][levelMeta->parts];
						
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDBox;
						levelMeta->parts++;
						break;
					case LPSpot:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDSpot;
						levelMeta->parts++;
						break;
					case LPPlayerOnSpot:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDSpot;

						levelPart = &Levels[LevelCount][levelMeta->parts];
						
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDPlayer;
						levelMeta->parts++;
						break;
					case LPPlayer:
						levelPart->x = x;
						levelPart->y = y;
						levelPart->id = IDPlayer;
						levelMeta->parts++;
						break;
				}
			}
			y++;
		}
	}
	return LevelCount > 0;
}

