#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <sys/stat.h>
#include <dirent.h>
#include "GameFuncs.h"
#include "Common.h"
#include "CInput.h"

void SetVolume(const int VolumeIn)
{
	if (GlobalSoundEnabled)
	{
		Mix_Volume(0,VolumeIn);
		Mix_Volume(1,VolumeIn);
		Mix_VolumeMusic(VolumeIn);
	}
}

void IncVolume()
{
	if (Volume < 128)
	{
		Volume += 8;
		SetVolume(Volume);
	}
}

void DecVolume()
{
	if (Volume > 0)
	{
		Volume-= 8;
		SetVolume(Volume);
	}
}

void MusicFinished()
{
	ReloadMusic=true;
}

void LoadSettings()
{
	FILE *Fp;
	char Filename[FILENAME_MAX];
	sprintf(Filename, "%s/.sokoban_settings.dat", getenv("HOME") == NULL ? ".": getenv("HOME"));
	Fp = fopen(Filename,"rt");
	if (Fp)
	{
		char* tmpName = (char*)malloc(sizeof(char) * 100);
		fscanf(Fp,"SelectedLevelPack=%100[^\n]\n",tmpName);
		for (int i = 0; i < InstalledLevelPacksCount; i++)
			if(strcasecmp(tmpName, InstalledLevelPacks[i]) == 0)
			{
				SelectedLevelPack = i;
				sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
				break;
			}
		fclose(Fp);
		free(tmpName);
	}

}

void SaveSettings()
{
	FILE *Fp;
	char Filename[FILENAME_MAX];
	sprintf(Filename, "%s/.sokoban_settings.dat", getenv("HOME") == NULL ? ".": getenv("HOME"));
	Fp = fopen(Filename,"wt");
	if (Fp)
	{
		fprintf(Fp,"SelectedLevelPack=%s\n",LevelPackName);
		fclose(Fp);
	}
}


void SearchForMusic()
{
	struct dirent *Entry;
	DIR *Directory;
	struct stat Stats;
	int Teller;
	char FileName[FILENAME_MAX];
	if (GlobalSoundEnabled)
		Music[0] = Mix_LoadMUS("./music/title.wav");
	Teller=1;
	Directory = opendir("./music");
	if (Directory)
	{
		Entry=readdir(Directory);
		while(Entry)
		{
			sprintf(FileName,"./music/%s",Entry->d_name);
			stat(FileName,&Stats);
			if(!S_ISDIR(Stats.st_mode))
			{
				if(strncmp(".", Entry->d_name, 1) && (strcmp("title.wav",Entry->d_name) != 0) && (Teller< MaxMusicFiles))
				{
					if (GlobalSoundEnabled)
					{
						Music[Teller] = Mix_LoadMUS(FileName);
						if(Music[Teller])
							Teller++;
					}
				}
			}
			Entry=readdir(Directory);
		}
		closedir(Directory);
	}
	MusicCount = Teller;
}

void DoSearchForLevelPacks(char* Path)
{
	struct dirent *Entry;
	DIR *Directory;
	struct stat Stats;
	char FileName[FILENAME_MAX];
	char Name[MaxLevelPackNameLength];
	Directory = opendir(Path);
	if (Directory)
	{
		Entry=readdir(Directory);
		while(Entry)
		{
			sprintf(FileName,"%s/%s",Path,Entry->d_name);
			stat(FileName,&Stats);
			if(S_ISDIR(Stats.st_mode))
			{
				if(strncmp(".", Entry->d_name, 1)  && (InstalledLevelPacksCount< MaxLevelPacks) && (strlen(Entry->d_name) < MaxLevelPackNameLength))
				{
					sprintf(Name,"%s",Entry->d_name);
					bool found = false;
					for (int i = 0; i < InstalledLevelPacksCount; i++)
					{
						if(strcmp(Name, InstalledLevelPacks[i]) == 0)
						{
							found = true;
							break;
						}
					}
					
					if(!found)
					{
						sprintf(InstalledLevelPacks[InstalledLevelPacksCount],"%s",Entry->d_name);
						InstalledLevelPacksCount++;
					}
				}
			}
			//levelpackfiles
			else
			{
				if((InstalledLevelPacksCount< MaxLevelPacks) && (strlen(Entry->d_name) < MaxLevelPackNameLength))
				{
					char *ext;
					char *dot = strrchr(Entry->d_name, '.');
					if(dot &&  !(dot == Entry->d_name)) 
					{
						ext = dot +1;
						if((strcasecmp(ext, "sok") == 0) || (strcasecmp(ext, "txt") == 0))
						{
							sprintf(Name,"%s",Entry->d_name);
							bool found = false;
							for (int i = 0; i < InstalledLevelPacksCount; i++)
							{
								if(strcmp(Name, InstalledLevelPacks[i]) == 0)
								{
									found = true;
									break;
								}
							}
							
							if(!found)
							{
								sprintf(InstalledLevelPacks[InstalledLevelPacksCount],"%s",Entry->d_name);
								InstalledLevelPacksCount++;
							}
						}
					}
				}
			}
			Entry=readdir(Directory);
		}
		closedir(Directory);
	}
}

void SearchForLevelPacks()
{
	InstalledLevelPacksCount = 0;
	DoSearchForLevelPacks("./levelpacks");
	char Path[FILENAME_MAX];
	sprintf(Path, "%s/.sokoban_levelpacks", getenv("HOME") == NULL ? ".": getenv("HOME"));
	DoSearchForLevelPacks(Path);
	SelectedLevelPack=0;
	if (InstalledLevelPacksCount > 0)
	{
		sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
	}
	else
	{
		sprintf(LevelPackName,"%s","");
	}
}


bool AskQuestion(char *Msg)
{
	bool Result = false;
	CInput *Input = new CInput(InputDelay);
	boxRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
	rectangleRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
	rectangleRGBA(Buffer,51*UI_WIDTH_SCALE,81.5*UI_HEIGHT_SCALE,269*UI_WIDTH_SCALE,159*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
	WriteText(Buffer,font,Msg,strlen(Msg),55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
	SDL_FillRect(Screen,NULL,SDL_MapRGB(Screen->format,0,0,0));
    if ((WINDOW_WIDTH != ORIG_WINDOW_WIDTH) || (WINDOW_HEIGHT != ORIG_WINDOW_HEIGHT))
	{
		SDL_Surface *ScreenBufferZoom = zoomSurface(Buffer,(double)WINDOW_WIDTH / ORIG_WINDOW_WIDTH,(double)WINDOW_HEIGHT / ORIG_WINDOW_HEIGHT,0);
		SDL_BlitSurface(ScreenBufferZoom,NULL,Screen,NULL);
		SDL_FreeSurface(ScreenBufferZoom);
	}
	else
	{
		SDL_BlitSurface(Buffer, NULL, Screen, NULL);
	}
    SDL_Flip(Screen);
	{
		while (!( Input->SpecialsHeld[SPECIAL_QUIT_EV] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_X)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_X)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]))
		{
		    Input->Update();
			if(GlobalSoundEnabled)
			if(!Mix_PlayingMusic())
			{
				Mix_PlayMusic(Music[SelectedMusic],0);
				SetVolume(Volume);
			}
			SDL_framerateDelay(&Fpsman);
		}
		if (Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;
		if (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)])
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
			Result = true;
		}
		else
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
			Result = false;
		}

	}
	delete Input;
	return Result;
}

void PrintForm(char *msg)
{
    CInput *Input = new CInput(InputDelay);
	boxRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
	rectangleRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
	rectangleRGBA(Buffer,51*UI_WIDTH_SCALE,81.5*UI_HEIGHT_SCALE,269*UI_WIDTH_SCALE,159*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
	WriteText(Buffer,font,msg,strlen(msg),55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
	SDL_FillRect(Screen,NULL,SDL_MapRGB(Screen->format,0,0,0));
    if ((WINDOW_WIDTH != ORIG_WINDOW_WIDTH) || (WINDOW_HEIGHT != ORIG_WINDOW_HEIGHT))
	{
		SDL_Surface *ScreenBufferZoom = zoomSurface(Buffer,(double)WINDOW_WIDTH / ORIG_WINDOW_WIDTH,(double)WINDOW_HEIGHT / ORIG_WINDOW_HEIGHT,0);
		SDL_BlitSurface(ScreenBufferZoom,NULL,Screen,NULL);
		SDL_FreeSurface(ScreenBufferZoom);
	}
	else
	{
		SDL_BlitSurface(Buffer, NULL, Screen, NULL);
	}
    SDL_Flip(Screen);
    while (!( Input->SpecialsHeld[SPECIAL_QUIT_EV] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]))
    {
        Input->Update();
        if(GlobalSoundEnabled)
        if(!Mix_PlayingMusic())
        {
            Mix_PlayMusic(Music[SelectedMusic],0);
            //Mix_HookMusicFinished(MusicFinished);
            SetVolume(Volume);
        }
        SDL_framerateDelay(&Fpsman);
    }
	if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
	    GameState = GSQuit;
	if (GlobalSoundEnabled)
		Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
	delete Input;
}

void SaveUnlockData()
{
	FILE *Fp;
	char Filename[FILENAME_MAX];
	sprintf(Filename,"%s/%s.dat",getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
	Fp = fopen(Filename,"wb");
	if (Fp)
	{
		fwrite(&UnlockedLevels,sizeof(int),1,Fp);
		fclose(Fp);
	}
}

void LoadUnlockData()
{
	FILE *Fp;
 	UnlockedLevels = 1;
	char Filename[FILENAME_MAX];
	sprintf(Filename,"%s/%s.dat",getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
	Fp = fopen(Filename,"rb");
	if (Fp)
	{
		fread(&UnlockedLevels,sizeof(int),1,Fp);
		fclose(Fp);
		if ((UnlockedLevels > InstalledLevels) || (UnlockedLevels < 1))
			UnlockedLevels = 1;
	}
}


char *GetString(char *NameIn,char *Msg)
{
	char *PackName = new char[21];
	bool End=false,SubmitChanges=false;
	int Teller,MaxSelection=0, Selection = 0,asci=97;
	CInput *Input = new CInput(InputDelay);
	sprintf(PackName,"%s",NameIn);
	MaxSelection = strlen(NameIn);
	PackName[Selection+1]='\0';
	if (MaxSelection == 0)
		PackName[MaxSelection]=chr(asci);
	char Tekst[100];
	while (!End)
	{
	    if(GlobalSoundEnabled)
		if(!Mix_PlayingMusic())
        {
            Mix_PlayMusic(Music[SelectedMusic],0);
            SetVolume(Volume);
        }
        Input->Update();

        if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
		{
			End = true;
            SubmitChanges=false;
            GameState = GSQuit;
		}

        if(Input->Ready() && (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)]))
        {
            if (Selection > 0)
            {	Selection--;
                asci = ord(PackName[Selection]);
            }
            Input->Delay();
        }

        if(Input->Ready() && (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)]))
        {
            if (Selection < 19)
            {
                Selection++;
                if (Selection > MaxSelection)
                {
                    PackName[Selection] = chr(97);
                    PackName[Selection+1] = '\0';
                    MaxSelection=Selection;
                }
                asci = ord(PackName[Selection]);
            }
            Input->Delay();
        }

        if(Input->Ready() && (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_UP)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_UP)]))
        {
            asci++;
            if (asci==123)
			{
                asci=32;
			}
            if (asci==33)
			{
                (asci=48);
			}
            if (asci==58)
			{
                asci=97;
			}
            PackName[Selection] = chr(asci);
            Input->Delay();
        }

        if(Input->Ready() && (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_DOWN)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_DOWN)]))
        {
            asci--;
            if(asci==96)
			{
            	asci=57;
			}
            if(asci==47)
			{
                asci=32;
			}
            if(asci==31)
			{
                asci=122;
			}
            PackName[Selection] = chr(asci);
            Input->Delay();
        }

        if(Input->Ready() && (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)] ||Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]))
        {
            if (GlobalSoundEnabled)
                Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
            End = true;
            SubmitChanges=true;
        }

        if(Input->Ready() && (Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_X)] || Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_X)] ))
        {
			if (GlobalSoundEnabled)
                Mix_PlayChannel(-1,Sounds[SND_BACK],0);
            End=true;
            SubmitChanges=false;
        }


		SDL_BlitSurface(IMGTitleScreen,NULL,Buffer,NULL);
		boxRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		rectangleRGBA(Buffer,51*UI_WIDTH_SCALE,81.5*UI_HEIGHT_SCALE,269*UI_WIDTH_SCALE,159*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		WriteText(Buffer,font,Msg,strlen(Msg),55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
	
		WriteText(Buffer,MonoFont,PackName,strlen(PackName),75*UI_WIDTH_SCALE,110*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
		if (Selection > 0)
		{
			sprintf(Tekst," ");
			for (Teller=1;Teller<Selection;Teller++)
				sprintf(Tekst,"%s ",Tekst);
			sprintf(Tekst,"%s_",Tekst);
		}
		else
			sprintf(Tekst,"_");
		WriteText(Buffer,MonoFont,Tekst,strlen(Tekst),75*UI_WIDTH_SCALE,112*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
		sprintf(Tekst,"Use Up, Down, Left, Right, %s=Ok %s=Cancel",JoystickSetup->GetKeyNameForDefinition(BUT_A),JoystickSetup->GetKeyNameForDefinition(BUT_X) );
		WriteText(Buffer,font,Tekst,strlen(Tekst),65*UI_WIDTH_SCALE,135*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor);
        SDL_FillRect(Screen,NULL,SDL_MapRGB(Screen->format,0,0,0));
        if ((WINDOW_WIDTH != ORIG_WINDOW_WIDTH) || (WINDOW_HEIGHT != ORIG_WINDOW_HEIGHT))
		{
			SDL_Surface *ScreenBufferZoom = zoomSurface(Buffer,(double)WINDOW_WIDTH / ORIG_WINDOW_WIDTH,(double)WINDOW_HEIGHT / ORIG_WINDOW_HEIGHT,0);
			SDL_BlitSurface(ScreenBufferZoom,NULL,Screen,NULL);
			SDL_FreeSurface(ScreenBufferZoom);
		}
		else
		{
			SDL_BlitSurface(Buffer, NULL, Screen, NULL);
		}
        SDL_Flip(Screen);
        SDL_framerateDelay(&Fpsman);
	}
	PackName[MaxSelection+1] = '\0';
	while ((PackName[0] == ' ') && (MaxSelection>-1))
	{
		for (Teller=0;Teller<MaxSelection;Teller++)
			PackName[Teller] = PackName[Teller+1];
		MaxSelection--;
	}
	if (MaxSelection>-1)
		while ((PackName[MaxSelection] == ' ') && (MaxSelection>0))
		{
			PackName[MaxSelection] = '\0';
			MaxSelection--;
		}
	if (!SubmitChanges)
		sprintf(PackName,"%s","\0");
    delete Input;
	return PackName;
}

void FindLevels()
{
	int InstalledLevelsFile = LevelPackFile->LevelCount;
	int Teller=InstalledLevelsFile + 1;
	char *FileName = new char[FILENAME_MAX];
	char *FileName2 = new char[FILENAME_MAX];
	
	sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName, Teller);
	sprintf(FileName2,"./levelpacks/%s/level%d.lev",LevelPackName,Teller);		
	while (FileExists(FileName) || FileExists(FileName2))
	{
		Teller+=30;
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName, Teller);
		sprintf(FileName2,"./levelpacks/%s/level%d.lev",LevelPackName,Teller);
	}
	while (!FileExists(FileName) && !FileExists(FileName2) && (Teller > InstalledLevelsFile) )
	{
		Teller--;
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName, Teller);
		sprintf(FileName2,"./levelpacks/%s/level%d.lev",LevelPackName,Teller);
	}
	InstalledLevels=Teller;
	delete[] FileName;
	delete[] FileName2;
}

void LoadGraphics()
{
	FILE *ColorsFile;
	SDL_Surface *Tmp;
	int R,G,B,A;
	char FileName[FILENAME_MAX];
	if(IMGBackground)
		SDL_FreeSurface(IMGBackground);
	if(IMGFloor)
		SDL_FreeSurface(IMGFloor);
	if(IMGPlayer)
		SDL_FreeSurface(IMGPlayer);
	if(IMGBox)
		SDL_FreeSurface(IMGBox);
	if(IMGSpot)
		SDL_FreeSurface(IMGSpot);
	if(IMGEmpty)
		SDL_FreeSurface(IMGEmpty);
	if(IMGWall)
		SDL_FreeSurface(IMGWall);
	if(IMGTitleScreen)
		SDL_FreeSurface(IMGTitleScreen);



	sprintf(FileName,"%s/.sokoban_levelpacks/%s/floor.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/floor.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/floor.png");
	SDL_SetColorKey(Tmp,SDL_SRCCOLORKEY |SDL_RLEACCEL,SDL_MapRGB(Tmp->format,255,0,255));
	IMGFloor = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/wall.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/wall.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/wall.png");
	SDL_SetColorKey(Tmp,SDL_SRCCOLORKEY |SDL_RLEACCEL,SDL_MapRGB(Tmp->format,255,0,255));
	IMGWall = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/box.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/box.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/box.png");
	SDL_SetColorKey(Tmp,SDL_SRCCOLORKEY |SDL_RLEACCEL,SDL_MapRGB(Tmp->format,255,0,255));
	IMGBox = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/spot.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/spot.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/spot.png");
	SDL_SetColorKey(Tmp,SDL_SRCCOLORKEY |SDL_RLEACCEL,SDL_MapRGB(Tmp->format,255,0,255));
	IMGSpot = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/player.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/player.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/player.png");
	SDL_SetColorKey(Tmp,SDL_SRCCOLORKEY |SDL_RLEACCEL,SDL_MapRGB(Tmp->format,255,0,255));
	IMGPlayer = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/empty.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/empty.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/empty.png");
	SDL_SetColorKey(Tmp,SDL_SRCCOLORKEY |SDL_RLEACCEL,SDL_MapRGB(Tmp->format,255,0,255));
	IMGEmpty = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/background.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/background.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/background.png");
	IMGBackground = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	isCustomnTitleScreen = true;
	sprintf(FileName,"%s/.sokoban_levelpacks/%s/titlescreen.png", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/titlescreen.png",LevelPackName);
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
	{
		isCustomnTitleScreen = false;
		Tmp = IMG_Load("./graphics/titlescreen.png");
	}
	IMGTitleScreen = SDL_DisplayFormat(Tmp);
	SDL_FreeSurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s/colors.txt", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
		sprintf(FileName,"./levelpacks/%s/colors.txt",LevelPackName);
	ColorsFile = fopen(FileName,"rt");
	if (ColorsFile)
	{
		fscanf(ColorsFile,"[TextColor]\nR=%d\nG=%d\nB=%d\n",&R,&G,&B);
		MenuTextColor.r = R;
		MenuTextColor.g = G;
		MenuTextColor.b = B;
		fscanf(ColorsFile,"[MenuBoxColor]\nR=%d\nG=%d\nB=%d\nA=%d\n",&R,&G,&B,&A);
		MenuBoxColor.r = R;
		MenuBoxColor.g = G;
		MenuBoxColor.b = B;
		MenuBoxColor.unused = A;
		fscanf(ColorsFile,"[MenuBoxBorderColor]\nR=%d\nG=%d\nB=%d\nA=%d\n",&R,&G,&B,&A);
		MenuBoxBorderColor.r = R;
		MenuBoxBorderColor.g = G;
		MenuBoxBorderColor.b = B;
		MenuBoxBorderColor.unused = A;
		fclose(ColorsFile);
	}
	else
	{
		MenuTextColor.r = 79;
		MenuTextColor.g = 135;
		MenuTextColor.b = 194;
		MenuBoxColor.r = 197;
		MenuBoxColor.g = 215;
		MenuBoxColor.b = 235;
		MenuBoxColor.unused = 255;
		MenuBoxBorderColor.r = 52;
		MenuBoxBorderColor.g = 102;
		MenuBoxBorderColor.b = 148;
		MenuBoxBorderColor.unused = 255;
	}
}

void UnLoadGraphics()
{
	if(IMGBackground)
		SDL_FreeSurface(IMGBackground);
	if(IMGFloor)
		SDL_FreeSurface(IMGFloor);
	if(IMGPlayer)
		SDL_FreeSurface(IMGPlayer);
	if(IMGBox)
		SDL_FreeSurface(IMGBox);
	if(IMGSpot)
		SDL_FreeSurface(IMGSpot);
	if(IMGEmpty)
		SDL_FreeSurface(IMGEmpty);
	if(IMGWall)
		SDL_FreeSurface(IMGWall);
	if(IMGTitleScreen)
		SDL_FreeSurface(IMGTitleScreen);
}

void UnloadMusic()
{
	int Teller;
	if (GlobalSoundEnabled)
	{
		Mix_HaltMusic();
		for (Teller=0;Teller < MusicCount;Teller++)
			if (Music[Teller])
				Mix_FreeMusic(Music[Teller]);
	}
}

void LoadSounds()
{
	if (GlobalSoundEnabled)
	{
		Sounds[SND_MENU] = Mix_LoadWAV("./sound/menu.wav");
		Sounds[SND_SELECT] = Mix_LoadWAV("./sound/select.wav");
		Sounds[SND_ERROR] = Mix_LoadWAV("./sound/error.wav");
		Sounds[SND_STAGEEND] = Mix_LoadWAV("./sound/stageend.wav");
		Sounds[SND_BACK] = Mix_LoadWAV("./sound/back.wav");
		Sounds[SND_MOVE] = Mix_LoadWAV("./sound/move.wav");
	}
}



void UnloadSounds()
{
	int Teller;
	for (Teller=0;Teller<NrOfSounds;Teller++)
		if(Sounds[Teller])
			Mix_FreeChunk(Sounds[Teller]);
}

void printTitleInfo(SDL_Surface *Surface)
{
	char Tekst[250];
	int w;
	if(LevelPackFile->Loaded)
	{
		strcpy(Tekst, "Sokoban");
		TTF_SizeText(RobotoBig, Tekst, &w, NULL);
		SDL_Color TitleColor = {174,198,234,255};
		WriteText(Surface, RobotoBig, Tekst, strlen(Tekst), (ORIG_WINDOW_WIDTH - w) / 2, 10, 0, TitleColor);

		if(strlen(LevelPackFile->author) > 0)
		{
			strcpy(Tekst, "Levels By ");
			strcat(Tekst, LevelPackFile->author);
			TTF_SizeText(RobotoMedium, Tekst, &w, NULL);
			WriteText(Surface, RobotoMedium, Tekst, strlen(Tekst), (ORIG_WINDOW_WIDTH - w) / 2, 290, 0, TitleColor);
		}
	}
	else
	{
		if(!isCustomnTitleScreen)
		{
			strcpy(Tekst, "Sokoban");
			TTF_SizeText(RobotoBig, Tekst, &w, NULL);
			SDL_Color TitleColor = {174,198,234,255};
			WriteText(Surface, RobotoBig, Tekst, strlen(Tekst), (ORIG_WINDOW_WIDTH - w) / 2, 10, 0, TitleColor);
		}
	}
}

void LoadJoystickSettings()
{
    JoystickSetup->AddDefinition(BUT_B,"Go back / Quit",1,1,SDLK_ESCAPE,SDLK_ESCAPE,"B");
    JoystickSetup->AddDefinition(BUT_R,"Next Music / Inc Level +5",5,5,SDLK_PAGEUP,SDLK_PAGEUP,"R");
    JoystickSetup->AddDefinition(BUT_L,"Prev Music / Dec Level -5",4,4,SDLK_PAGEDOWN,SDLK_PAGEDOWN,"L");
    JoystickSetup->AddDefinition(BUT_LEFT,"Left",JOYSTICK_LEFT,JOYSTICK_LEFT,SDLK_LEFT,SDLK_LEFT,"LEFT");
    JoystickSetup->AddDefinition(BUT_RIGHT,"Right",JOYSTICK_RIGHT,JOYSTICK_RIGHT,SDLK_RIGHT,SDLK_RIGHT,"RIGHT");
    JoystickSetup->AddDefinition(BUT_DOWN,"Down",JOYSTICK_DOWN,JOYSTICK_DOWN,SDLK_DOWN,SDLK_DOWN,"DOWN");
    JoystickSetup->AddDefinition(BUT_UP,"Up",JOYSTICK_UP,JOYSTICK_UP,SDLK_UP,SDLK_UP,"UP");
    JoystickSetup->AddDefinition(BUT_A,"Select / Place part",0,0,SDLK_SPACE,SDLK_SPACE,"A");
    JoystickSetup->AddDefinition(BUT_X,"Center level",2,2,SDLK_x,SDLK_x,"X");
    JoystickSetup->AddDefinition(BUT_Y,"New Level / Erase all parts",3,3,SDLK_y,SDLK_y,"Y");
    JoystickSetup->AddDefinition(BUT_SELECT,"Hide position / Statistics",6,6,SDLK_b,SDLK_b,"SELECT");
    JoystickSetup->AddDefinition(BUT_START,"Restart / Test level",7,7,SDLK_RETURN,SDLK_RETURN,"START");
	char FileName[FILENAME_MAX];
	sprintf(FileName,"%s/.sokoban_joystick.def", getenv("HOME") == NULL ? ".": getenv("HOME"));
	JoystickSetup->LoadCurrentButtonValues(FileName);
}


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
			TmpSurface1 = TTF_RenderUTF8_Blended(FontIn,List[Teller],ColorIn);
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
	struct stat statbuf;
	stat(FileName, &statbuf);
    // test for a regular file
    if (S_ISREG(statbuf.st_mode))
	{
		Fp = fopen(FileName,"rb");
		if (Fp)
		{
			fclose(Fp);
			return true;
		}
		else
			return false;
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