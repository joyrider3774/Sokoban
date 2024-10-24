#include "Functions.h"

void WriteText(SDL_Surface* Surface,TTF_Font* FontIn,char* Tekst,int NrOfChars,int X,int Y,int YSpacing,SDL_Color ColorIn)
{
	char List[100][255];
	int Lines,Teller,Chars;
	SDL_Rect DstRect;
	SDL_Surface* TmpSurface1;
	Lines = 0;
	Chars = 0;
	for (Teller=0;Teller<NrOfChars;Teller++)
	{
		if(Lines > 100)
			break;
		if((Tekst[Teller] == '\n') || (Chars==255))
		{
			List[Lines][Chars]='\0';
			Lines++;
			Chars = 0;
		}
		else
		{
		 	List[Lines][Chars]=Tekst[Teller];
		 	Chars++;
		}
	}
	List[Lines][Chars] = '\0';
	for (Teller=0;Teller <= Lines;Teller++)
	{
		if(strlen(List[Teller]) > 0)
		{
			TmpSurface1 = TTF_RenderText_Blended(FontIn,List[Teller],ColorIn);
			DstRect.x = X;
			DstRect.y = Y + (Teller) * TTF_FontLineSkip(FontIn) + (Teller*YSpacing);
			DstRect.w = TmpSurface1->w;
			DstRect.h = TmpSurface1->h;
			SDL_BlitSurface(TmpSurface1,NULL,Surface,&DstRect);
			SDL_FreeSurface(TmpSurface1);
		}
	}
}

bool FileExists(char * FileName)
{
	FILE *Fp;
	Fp = fopen(FileName,"rb");
	if (Fp)
	{
		fclose(Fp);
		return true;
	}
	else
		return false;
}

char chr(int ascii)
{
	return((char)ascii);
}

int ord(char chr)
{
	return((int)chr);
}

void AddUnderScores (char *string)
{
	int Teller;
	for(Teller=0;Teller<strlen(string);Teller++)
		if(string[Teller] == ' ')
			string[Teller] = '_';
}

void RemoveUnderScores (char *string)
{
	int Teller;
	for(Teller=0;Teller<strlen(string);Teller++)
		if(string[Teller] == '_')
			string[Teller] = ' ';
}
