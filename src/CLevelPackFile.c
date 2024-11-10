#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include "GameFuncs.h"
#include "CLevelPackFile.h"
#include "Defines.h"

CLevelPackFile* CLevelPackFile_Create()
{
	CLevelPackFile* Result = (CLevelPackFile*)malloc(sizeof(CLevelPackFile));
	Result->Loaded = false;
	Result->LevelCount = 0;
	memset(Result->author, 0, 250);
	memset(Result->set, 0, 250);
	return Result;
}

void CLevelPackFile_Destroy(CLevelPackFile* LevelPackFile)
{
	if(LevelPackFile)
	{
		free(LevelPackFile);
		LevelPackFile = NULL;
	}
}

bool CLevelPackFile_loadFile(CLevelPackFile* LevelPackFile, char* filename, int maxWidth, int maxHeight, bool MetaOnly)
{
	bool Result = false;
	LevelPackFile->LevelCount = 0;
	LevelPackFile->Loaded = false;
	memset(LevelPackFile->author, 0, 250);
	memset(LevelPackFile->set, 0, 250);
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
			char* tmp =(char*) malloc(sizeof(char) *(FileSize + 2));
			fread(tmp,1,FileSize,Fp);		
			tmp[FileSize] = '\0';
			Result = CLevelPackFile_parseText(LevelPackFile, tmp, maxWidth, maxHeight, MetaOnly);		
			free(tmp);
			fclose(Fp);
			LevelPackFile->Loaded = true;
		}
	}
	return Result;
}

bool CLevelPackFile_parseText(CLevelPackFile *LevelPackFile, char* text, int maxWidth, int maxHeight, bool MetaOnly)
{
	char line[2000] = "";
	char levelField[100] = "";
	char levelFieldValue[2000] = "";
	int linepos;
	char* pchar = text;
	char* pdoublepoint, *pset, *pauthor;
	int y;
	bool inlevel = false;
	LevelPackFile->LevelCount = 0;
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

		if(LevelPackFile->LevelCount == 0)
		{
			pset = strstr(line, "set:");
			if(pset)
			{
				pset+= 4;
				while(*pset == ' ')
					pset++;
				strncpy(LevelPackFile->set, pset, strlen(pset));
			}

			pauthor = strstr(line, "author:");
			if(pauthor)
			{
				pauthor+= 7;
				while(*pauthor == ' ')
					pauthor++;
				strncpy(LevelPackFile->author, pauthor, strlen(pauthor));
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
					strcpy(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].title, ptmp);

				if (strcasecmp(levelField, "author") == 0)
					strcpy(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].author, ptmp);
			
				if (strcasecmp(levelField, "comment") == 0)
					strcpy(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].comments, ptmp);
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
					strcpy(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].title, ptmp);

				if (strcasecmp(levelField, "author") == 0)
					strcpy(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].author, ptmp);
			
				if (strcasecmp(levelField, "comment") == 0)
					strcpy(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].comments, ptmp);
			}
			//clear them for if condition above conerning level start
			memset(levelFieldValue, 0, 2000);
			memset(levelField, 0, 100);
			inlevel = false;
			if((LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].width <= maxWidth) && 
				(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].height <= maxHeight))
				LevelPackFile->LevelCount++;
			continue;
		}

		//we are not in a level and found a wall and no doublepoint and we are not in a levelfield then assume levelstart
		if (!inlevel && (strchr(line, LPWall)) && !pdoublepoint && (levelField[0] == '\0'))
		{
			if (MetaOnly)
				return true;
			inlevel=true;
			y = 0;
			LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].width = 0;
			LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].height = 0;
			memset(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].author, 0, 100);
			memset(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].title, 0, 100);
			memset(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].author, 0, 100);
			LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts = 0;
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
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDBox;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						break;
					case LPPlayer:
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDPlayer;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						break;
					case LPBoxOnSpot:
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDSpot;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDBox;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						break;
					case LPSpot:
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDSpot;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						break;
					case LPPlayerOnSpot:
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDSpot;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDPlayer;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						break;
					case LPWall:
						if(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].width < x+1)
							LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].width = x+1;
						if(LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].height < y+1)
							LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].height = y+1;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].x = x;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].y = y;
						LevelPackFile->Levels[LevelPackFile->LevelCount][LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts].id = IDWall;
						LevelPackFile->LevelsMeta[LevelPackFile->LevelCount].parts++;
						break;
				}
			}
			y++;
		}
	}
	return LevelPackFile->LevelCount > 0;
}

