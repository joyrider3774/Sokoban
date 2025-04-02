#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <sys/stat.h>
#include <dirent.h>
#include "GameFuncs.h"
#include "Common.h"
#include "CInput.h"

char* assetPath(const char* assetSubPath)
{
	char* Result = (char*) SDL_malloc(FILENAME_MAX);

	snprintf(Result, FILENAME_MAX, "%s/%s", basePath, assetSubPath);
	
	return Result;
}

void logMessage(SDL_PRINTF_FORMAT_STRING const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
#if defined _WIN32 || defined __CYGWIN__
    vprintf(fmt, ap);
#else
    SDL_LogMessageV(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, fmt, ap);
#endif    
    va_end(ap);   
}

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
	sprintf(Filename, "%s/.sokoban_settings.dat", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
	Fp = fopen(Filename,"rt");
	if (Fp)
	{
		char* tmpName = (char*)SDL_malloc(sizeof(char) * 100);
		fscanf(Fp,"SelectedLevelPack=%100[^\n]\n",tmpName);
		for (int i = 0; i < InstalledLevelPacksCount; i++)
			if(strcasecmp(tmpName, InstalledLevelPacks[i]) == 0)
			{
				SelectedLevelPack = i;
				sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
				break;
			}
		fclose(Fp);
		SDL_free(tmpName);
	}

}

void SaveSettings()
{
	FILE *Fp;
	char Filename[FILENAME_MAX];
	sprintf(Filename, "%s/.sokoban_settings.dat", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
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

void DoSearchForLevelPacks(const char* Path)
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
					char* ext = strstr(Name,"._lev");
					if(ext)
						*ext = 0;
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
						sprintf(InstalledLevelPacks[InstalledLevelPacksCount],"%s",Name);
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
	sprintf(Path, "%s/.sokoban_levelpacks", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
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


bool AskQuestion(const char *Msg)
{
	bool Result = false;
	SDL_Texture * PrevTarget = SDL_GetRenderTarget(Renderer);
	Input->Reset();
	Input->SetInputDelay(InputDelay);
	SDL_FRect Rect1;
	Rect1.x = 60.0f*UI_WIDTH_SCALE;
	Rect1.y = 80.0f*UI_HEIGHT_SCALE;
	Rect1.w = 200.0f*UI_WIDTH_SCALE;
	Rect1.h = 85.0f*UI_HEIGHT_SCALE;
	SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
	SDL_RenderFillRect(Renderer, &Rect1);
	SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
	SDL_RenderRect(Renderer, &Rect1);
	SDL_FRect Rect2;
	Rect2.x = 62.0f*UI_WIDTH_SCALE;
	Rect2.y = 82.0f*UI_HEIGHT_SCALE;
	Rect2.w = 196.0f*UI_WIDTH_SCALE;
	Rect2.h = 81.0f*UI_HEIGHT_SCALE;
	SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
	SDL_RenderRect(Renderer, &Rect2);
	WriteText(font,Msg,strlen(Msg),65*UI_WIDTH_SCALE,83*UI_HEIGHT_SCALE,2,MenuTextColor,false);
	SDL_SetRenderTarget(Renderer, NULL);
	SDL_SetRenderDrawColor(Renderer, 0,0,0,255);
	SDL_RenderClear(Renderer);
	SDL_SetRenderLogicalPresentation(Renderer, ORIG_WINDOW_WIDTH, ORIG_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);        
	SDL_RenderTexture(Renderer, Buffer, NULL, NULL);
	SDL_RenderPresent(Renderer);
	while (!( Input->SpecialsHeld(SPECIAL_QUIT_EV) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A)) || Input->JoystickHeld(0, JoystickSetup->GetButtonValue(BUT_X)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_X)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))))
	{
		frameticks = SDL_GetPerformanceCounter();
		Input->Update();
		if(GlobalSoundEnabled)
		if(!Mix_PlayingMusic())
		{
			Mix_PlayMusic(Music[SelectedMusic],0);
			SetVolume(Volume);
		}
		if(showfps)
		{

			char fpsText[100];
			sprintf(fpsText, "FPS: %.2f\n", avgfps);
			SDL_FRect Rect;
			Rect.x = 0.0f;
			Rect.y = 0.0f;
			Rect.w = 100.0f;
			Rect.h = (float)TTF_GetFontHeight(font);
			SDL_SetRenderDrawColor(Renderer, 255,255,255,255);
			SDL_RenderFillRect(Renderer, &Rect);
			SDL_Color col = {0,0,0,255};
			WriteText(font, fpsText, strlen(fpsText), 0, 0, 0, col, false);
		}
		SDL_SetRenderTarget(Renderer, NULL);
		SDL_SetRenderDrawColor(Renderer, 0,0,0,255);
		SDL_RenderClear(Renderer);
		SDL_SetRenderLogicalPresentation(Renderer, ORIG_WINDOW_WIDTH, ORIG_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);        
		SDL_RenderTexture(Renderer, Buffer, NULL, NULL);
		SDL_RenderPresent(Renderer);
		Uint64 frameEndTicks = SDL_GetPerformanceCounter();
		Uint64 FramePerf = frameEndTicks - frameticks;
		frameTime = (double)FramePerf / (double)SDL_GetPerformanceFrequency() * 1000.0f;
		double delay = 1000.0f / FPS - frameTime;
		if (!nodelay && (delay > 0.0f))
		SDL_Delay((Uint32)(delay)); 
		if(skipCounter > 0)
		{
			skipCounter--;
			lastfpstime = SDL_GetTicks();
		}
		else
		{
			framecount++;
			if(SDL_GetTicks() - lastfpstime >= 1000)
			{
				for (int i = FPS_SAMPLES-1; i > 0; i--)
					fpsSamples[i] = fpsSamples[i-1];
				fpsSamples[0] = framecount;
				fpsAvgCount++;
				if(fpsAvgCount > FPS_SAMPLES)
					fpsAvgCount = FPS_SAMPLES;
				int fpsSum = 0;
				for (int i = 0; i < fpsAvgCount; i++)
					fpsSum += fpsSamples[i];
				avgfps = (double)fpsSum / (double)fpsAvgCount;
				framecount = 0;
				lastfpstime = SDL_GetTicks();
			}
		}
	}
	if (Input->SpecialsHeld(SPECIAL_QUIT_EV))
		GameState = GSQuit;
	if (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A)))
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

	Input->Reset();
	SDL_SetRenderTarget(Renderer, PrevTarget);
	return Result;
}

void PrintForm(const char *msg)
{
	SDL_Texture * PrevTarget = SDL_GetRenderTarget(Renderer);
	Input->Reset();
    Input->SetInputDelay(InputDelay);
	SDL_FRect Rect1;
	Rect1.x = 60.0f*UI_WIDTH_SCALE;
	Rect1.y = 80.0f*UI_HEIGHT_SCALE;
	Rect1.w = 200.0f*UI_WIDTH_SCALE;
	Rect1.h = 85.0f*UI_HEIGHT_SCALE;
	SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
	SDL_RenderFillRect(Renderer, &Rect1);
	SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
	SDL_RenderRect(Renderer, &Rect1);
	SDL_FRect Rect2;
	Rect2.x = 62.0f*UI_WIDTH_SCALE;
	Rect2.y = 82.0f*UI_HEIGHT_SCALE;
	Rect2.w = 196.0f*UI_WIDTH_SCALE;
	Rect2.h = 81.0f*UI_HEIGHT_SCALE;
	SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
	SDL_RenderRect(Renderer, &Rect2);
	WriteText(font,msg,strlen(msg),65*UI_WIDTH_SCALE,83*UI_HEIGHT_SCALE,2,MenuTextColor,false);
	SDL_SetRenderTarget(Renderer, NULL);
	SDL_SetRenderDrawColor(Renderer, 0,0,0,255);
	SDL_RenderClear(Renderer);
	SDL_SetRenderLogicalPresentation(Renderer, ORIG_WINDOW_WIDTH, ORIG_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);        
	SDL_RenderTexture(Renderer, Buffer, NULL, NULL);
	SDL_RenderPresent(Renderer);
    while (!( Input->SpecialsHeld(SPECIAL_QUIT_EV) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))))
    {
		frameticks = SDL_GetPerformanceCounter();
        Input->Update();
        if(GlobalSoundEnabled)
        if(!Mix_PlayingMusic())
        {
            Mix_PlayMusic(Music[SelectedMusic],0);
            //Mix_HookMusicFinished(MusicFinished);
            SetVolume(Volume);
        }
        if(showfps)
        {
            char fpsText[100];
            sprintf(fpsText, "FPS: %.2f\n", avgfps);
            SDL_FRect Rect;
			Rect.x = 0.0f;
			Rect.y = 0.0f;
			Rect.w = 100.0f;
			Rect.h = (float)TTF_GetFontHeight(font);
            SDL_SetRenderDrawColor(Renderer, 255,255,255,255);
            SDL_RenderFillRect(Renderer, &Rect);
            SDL_Color col = {0,0,0,255};
            WriteText(font, fpsText, strlen(fpsText), 0, 0, 0, col, false);
        }
        SDL_SetRenderTarget(Renderer, NULL);
        SDL_SetRenderDrawColor(Renderer, 0,0,0,255);
        SDL_RenderClear(Renderer);
        SDL_SetRenderLogicalPresentation(Renderer, ORIG_WINDOW_WIDTH, ORIG_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);        
        SDL_RenderTexture(Renderer, Buffer, NULL, NULL);
        SDL_RenderPresent(Renderer);
        Uint64 frameEndTicks = SDL_GetPerformanceCounter();
        Uint64 FramePerf = frameEndTicks - frameticks;
        frameTime = (double)FramePerf / (double)SDL_GetPerformanceFrequency() * 1000.0f;
        double delay = 1000.0f / FPS - frameTime;
        if (!nodelay && (delay > 0.0f))
            SDL_Delay((Uint32)(delay)); 
		if (showfps)
		{
			if(skipCounter > 0)
			{
				skipCounter--;
				lastfpstime = SDL_GetTicks();
			}
			else
			{
				framecount++;
				if(SDL_GetTicks() - lastfpstime >= 1000)
				{
					for (int i = FPS_SAMPLES-1; i > 0; i--)
						fpsSamples[i] = fpsSamples[i-1];
					fpsSamples[0] = framecount;
					fpsAvgCount++;
					if(fpsAvgCount > FPS_SAMPLES)
						fpsAvgCount = FPS_SAMPLES;
					int fpsSum = 0;
					for (int i = 0; i < fpsAvgCount; i++)
						fpsSum += fpsSamples[i];
					avgfps = (double)fpsSum / (double)fpsAvgCount;
					framecount = 0;
					lastfpstime = SDL_GetTicks();
				}
			}
		}
    }
	if(Input->SpecialsHeld(SPECIAL_QUIT_EV))
	    GameState = GSQuit;
	if (GlobalSoundEnabled)
		Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
	Input->Reset();
	SDL_SetRenderTarget(Renderer, PrevTarget);
}

void SaveUnlockData()
{
	FILE *Fp;
	char Filename[FILENAME_MAX];
	sprintf(Filename,"%s/%s.dat",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
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
	sprintf(Filename,"%s/%s.dat",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
	Fp = fopen(Filename,"rb");
	if (Fp)
	{
		fread(&UnlockedLevels,sizeof(int),1,Fp);
		fclose(Fp);
		if ((UnlockedLevels > InstalledLevels) || (UnlockedLevels < 1))
			UnlockedLevels = 1;
	}
}


char *GetString(const char *NameIn,const char *Msg)
{
	Input->Reset();
	char *PackName = new char[21];
	bool End=false,SubmitChanges=false;
	int Teller,MaxSelection=0, Selection = 0,asci=97;
	Input->SetInputDelay(InputDelay);
	sprintf(PackName,"%s",NameIn);
	MaxSelection = strlen(NameIn);
	PackName[Selection+1]='\0';
	if (MaxSelection == 0)
		PackName[MaxSelection]=chr(asci);
	char Tekst[100];
	SDL_Texture * PrevTarget = SDL_GetRenderTarget(Renderer);
	while (!End)
	{
		frameticks = SDL_GetPerformanceCounter();
		SDL_SetRenderTarget(Renderer, Buffer);
	    if(GlobalSoundEnabled)
			if(!Mix_PlayingMusic())
			{
				Mix_PlayMusic(Music[SelectedMusic],0);
				SetVolume(Volume);
			}
        Input->Update();

        if(Input->SpecialsHeld(SPECIAL_QUIT_EV))
		{
			End = true;
            SubmitChanges=false;
            GameState = GSQuit;
		}

        if(Input->Ready() && (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT))))
        {
            if (Selection > 0)
            {	Selection--;
                asci = ord(PackName[Selection]);
            }
            Input->Delay();
        }

        if(Input->Ready() && (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_RIGHT)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT))))
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

        if(Input->Ready() && (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_UP)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_UP))))
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

        if(Input->Ready() && (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_DOWN)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_DOWN))))
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

        if(Input->Ready() && (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A)) ||Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))))
        {
            if (GlobalSoundEnabled)
                Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
            End = true;
            SubmitChanges=true;
        }

        if(Input->Ready() && (Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_X)) || Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_X)) ))
        {
			if (GlobalSoundEnabled)
                Mix_PlayChannel(-1,Sounds[SND_BACK],0);
            End=true;
            SubmitChanges=false;
        }


		SDL_RenderTexture(Renderer, IMGTitleScreen,NULL,NULL);
		printTitleInfo();
		SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
		SDL_FRect Rect;
		Rect.x = 50.0f*UI_WIDTH_SCALE;
		Rect.y = 80.0f*UI_HEIGHT_SCALE;
		Rect.w = 220.0f*UI_WIDTH_SCALE;
		Rect.h = 85.0f*UI_HEIGHT_SCALE;
		SDL_RenderFillRect(Renderer, &Rect);
		SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
		SDL_RenderRect(Renderer, &Rect);
		Rect.x = 52.0f*UI_WIDTH_SCALE;
		Rect.y = 82.0f*UI_HEIGHT_SCALE;
		Rect.w = 215.0f*UI_WIDTH_SCALE;
		Rect.h = 81.0f*UI_HEIGHT_SCALE;
		WriteText(font,Msg,strlen(Msg),55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
	
		WriteText(MonoFont,PackName,strlen(PackName),75*UI_WIDTH_SCALE,110*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
		if (Selection > 0)
		{
			strcpy(Tekst," ");
			for (Teller=1;Teller<Selection;Teller++)
				strcat(Tekst," ");
			strcat(Tekst,"_");
		}
		else
			strcpy(Tekst,"_");
		WriteText(MonoFont,Tekst,strlen(Tekst),75*UI_WIDTH_SCALE,112*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor, false);
		sprintf(Tekst,"Use Up, Down, Left, Right, %s=Ok %s=Cancel",JoystickSetup->GetKeyNameForDefinition(BUT_A),JoystickSetup->GetKeyNameForDefinition(BUT_X) );
		WriteText(font,Tekst,strlen(Tekst),65*UI_WIDTH_SCALE,135*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
        if(showfps)
		{
			char fpsText[100];
			sprintf(fpsText, "FPS: %.2f\n", avgfps);
			SDL_FRect Rect;
			Rect.x = 0.0f;
			Rect.y = 0.0f;
			Rect.w = 100.0f;
			Rect.h = (float)TTF_GetFontHeight(font);
			SDL_SetRenderDrawColor(Renderer, 255,255,255,255);
			SDL_RenderFillRect(Renderer, &Rect);
			SDL_Color col = {0,0,0,255};
			WriteText(font, fpsText, strlen(fpsText), 0, 0, 0, col, false);
		}		
		SDL_SetRenderTarget(Renderer, NULL);
		SDL_SetRenderDrawColor(Renderer, 0,0,0,255);
		SDL_RenderClear(Renderer);
		SDL_SetRenderLogicalPresentation(Renderer, ORIG_WINDOW_WIDTH, ORIG_WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);        
		SDL_RenderTexture(Renderer, Buffer, NULL, NULL);
		SDL_RenderPresent(Renderer);
		Uint64 frameEndTicks = SDL_GetPerformanceCounter();
		Uint64 FramePerf = frameEndTicks - frameticks;
		frameTime = (double)FramePerf / (double)SDL_GetPerformanceFrequency() * 1000.0f;
		double delay = 1000.0f / FPS - frameTime;
		if (!nodelay && (delay > 0.0f))
		SDL_Delay((Uint32)(delay)); 
		if(skipCounter > 0)
		{
			skipCounter--;
			lastfpstime = SDL_GetTicks();
		}
		else
		{
			framecount++;
			if(SDL_GetTicks() - lastfpstime >= 1000)
			{
				for (int i = FPS_SAMPLES-1; i > 0; i--)
					fpsSamples[i] = fpsSamples[i-1];
				fpsSamples[0] = framecount;
				fpsAvgCount++;
				if(fpsAvgCount > FPS_SAMPLES)
					fpsAvgCount = FPS_SAMPLES;
				int fpsSum = 0;
				for (int i = 0; i < fpsAvgCount; i++)
					fpsSum += fpsSamples[i];
				avgfps = (double)fpsSum / (double)fpsAvgCount;
				framecount = 0;
				lastfpstime = SDL_GetTicks();
			}
		}
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
	Input->Reset();
	SDL_SetRenderTarget(Renderer, PrevTarget);
	return PackName;
}

void FindLevels()
{
	int InstalledLevelsFile = LevelPackFile->LevelCount;
	int Teller=InstalledLevelsFile + 1;
	char *FileName = new char[FILENAME_MAX];
	char *FileName2 = new char[FILENAME_MAX];
	char *FileName3 = new char[FILENAME_MAX];
	sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller);
	sprintf(FileName2,"./levelpacks/%s/level%d.lev",LevelPackName,Teller);
	sprintf(FileName3,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller);
	while (FileExists(FileName) || FileExists(FileName2) || FileExists(FileName3))
	{
		Teller+=30;
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller);
		sprintf(FileName2,"./levelpacks/%s/level%d.lev",LevelPackName,Teller);
		sprintf(FileName3,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller);
	}
	while (!FileExists(FileName) && !FileExists(FileName2) && !FileExists(FileName3) && (Teller > InstalledLevelsFile) )
	{
		Teller--;
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller);
		sprintf(FileName2,"./levelpacks/%s/level%d.lev",LevelPackName,Teller);
		sprintf(FileName3,"%s/.sokoban_levelpacks/%s._lev/level%d.lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName, Teller);
	}
	InstalledLevels=Teller;
	delete[] FileName;
	delete[] FileName2;
	delete[] FileName3;
}

void LoadGraphics()
{
	FILE *ColorsFile;
	SDL_Surface *Tmp;
	int R,G,B,A;
	char FileName[FILENAME_MAX];
	if(IMGBackground)
		SDL_DestroyTexture(IMGBackground);
	if(IMGFloor)
		SDL_DestroyTexture(IMGFloor);
	if(IMGPlayer)
		SDL_DestroyTexture(IMGPlayer);
	if(IMGBox)
		SDL_DestroyTexture(IMGBox);
	if(IMGSpot)
		SDL_DestroyTexture(IMGSpot);
	if(IMGEmpty)
		SDL_DestroyTexture(IMGEmpty);
	if(IMGWall)
		SDL_DestroyTexture(IMGWall);
	if(IMGTitleScreen)
		SDL_DestroyTexture(IMGTitleScreen);



	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/floor.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/floor.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/floor.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/floor.png");
	IMGFloor = SDL_CreateTextureFromSurface(Renderer, Tmp);
	

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/wall.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/wall.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/wall.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/wall.png");
	IMGWall =  SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/box.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/box.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/box.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/box.png");
	IMGBox =  SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/spot.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/spot.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/spot.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/spot.png");
	IMGSpot = SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/player.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/player.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/player.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/player.png");
	IMGPlayer = SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/empty.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/empty.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/empty.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/empty.png");
	IMGEmpty = SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/background.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/background.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/background.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
		Tmp = IMG_Load("./graphics/background.png");
	IMGBackground = SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	isCustomnTitleScreen = true;
	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/titlescreen.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/titlescreen.png", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/titlescreen.png",LevelPackName);
	}
	if (FileExists(FileName))
		Tmp = IMG_Load(FileName);
	else
	{
		isCustomnTitleScreen = false;
		Tmp = IMG_Load("./graphics/titlescreen.png");
	}
	IMGTitleScreen = SDL_CreateTextureFromSurface(Renderer, Tmp);
	SDL_DestroySurface(Tmp);

	sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/colors.txt", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
	if(!FileExists(FileName))
	{
		sprintf(FileName,"%s/.sokoban_levelpacks/%s/colors.txt", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
		if(!FileExists(FileName))
			sprintf(FileName,"./levelpacks/%s/colors.txt",LevelPackName);
	}
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
		MenuBoxColor.a = A;
		fscanf(ColorsFile,"[MenuBoxBorderColor]\nR=%d\nG=%d\nB=%d\nA=%d\n",&R,&G,&B,&A);
		MenuBoxBorderColor.r = R;
		MenuBoxBorderColor.g = G;
		MenuBoxBorderColor.b = B;
		MenuBoxBorderColor.a = A;
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
		MenuBoxColor.a = 255;
		MenuBoxBorderColor.r = 52;
		MenuBoxBorderColor.g = 102;
		MenuBoxBorderColor.b = 148;
		MenuBoxBorderColor.a = 255;
	}
}

void UnLoadGraphics()
{
	if(IMGBackground)
		SDL_DestroyTexture(IMGBackground);
	if(IMGFloor)
		SDL_DestroyTexture(IMGFloor);
	if(IMGPlayer)
		SDL_DestroyTexture(IMGPlayer);
	if(IMGBox)
		SDL_DestroyTexture(IMGBox);
	if(IMGSpot)
		SDL_DestroyTexture(IMGSpot);
	if(IMGEmpty)
		SDL_DestroyTexture(IMGEmpty);
	if(IMGWall)
		SDL_DestroyTexture(IMGWall);
	if(IMGTitleScreen)
		SDL_DestroyTexture(IMGTitleScreen);
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

void printTitleInfo()
{
	char Tekst[250];
	int w;
	if(LevelPackFile->Loaded)
	{
		strcpy(Tekst, "Sokoban");
		TTF_GetStringSize(RobotoBig, Tekst, strlen(Tekst), &w, NULL);
		SDL_Color TitleColor = {174,198,234,255};
		WriteText(RobotoBig, Tekst, strlen(Tekst), (ORIG_WINDOW_WIDTH - w) / 2, 10, 0, TitleColor, false);

		if(strlen(LevelPackFile->author) > 0)
		{
			strcpy(Tekst, "Levels by ");
			strcat(Tekst, LevelPackFile->author);
			TTF_GetStringSize(RobotoMedium, Tekst, strlen(Tekst), &w, NULL);
			WriteText(RobotoMedium, Tekst, strlen(Tekst), (ORIG_WINDOW_WIDTH - w) / 2, 290, 0, TitleColor, false);
		}
	}
	else
	{
		if(!isCustomnTitleScreen)
		{
			strcpy(Tekst, "Sokoban");
			TTF_GetStringSize(RobotoBig, Tekst, strlen(Tekst), &w, NULL);
			SDL_Color TitleColor = {174,198,234,255};
			WriteText(RobotoBig, Tekst, strlen(Tekst), (ORIG_WINDOW_WIDTH - w) / 2, 10, 0, TitleColor, false);
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
    JoystickSetup->AddDefinition(BUT_X,"Center level",2,2,SDLK_X,SDLK_X,"X");
    JoystickSetup->AddDefinition(BUT_Y,"New Level / Erase all parts",3,3,SDLK_Y,SDLK_Y,"Y");
    JoystickSetup->AddDefinition(BUT_SELECT,"Hide position / Statistics",6,6,SDLK_B,SDLK_B,"SELECT");
    JoystickSetup->AddDefinition(BUT_START,"Restart / Test level",7,7,SDLK_RETURN,SDLK_RETURN,"START");
	char FileName[FILENAME_MAX];
	sprintf(FileName,"%s/.sokoban_joystick.def", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
	JoystickSetup->LoadCurrentButtonValues(FileName);
}


void WriteText(TTF_Font* FontIn,const char* Tekst,int NrOfChars,int X,int Y,int YSpacing,SDL_Color ColorIn,bool Centered)
{
	char List[100][255];
	int Lines,Teller,Chars;
	SDL_FRect DstRect;
	SDL_Surface* TmpSurface1;
	SDL_Texture *TmpTexture;
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
			TmpSurface1 = TTF_RenderText_Blended(FontIn,List[Teller],strlen(List[Teller]) * sizeof(char), ColorIn);
			if(Centered)
			{
				int w;
				SDL_GetCurrentRenderOutputSize(Renderer, &w, NULL);
                DstRect.x = (w /2) - (TmpSurface1->w / 2);
			}
			else
                DstRect.x = X;
			DstRect.y = Y + (Teller) * TTF_GetFontLineSkip(FontIn) + (Teller*YSpacing);
			DstRect.w = TmpSurface1->w;
			DstRect.h = TmpSurface1->h;
			TmpTexture = SDL_CreateTextureFromSurface(Renderer, TmpSurface1);
			SDL_RenderTexture(Renderer, TmpTexture,NULL,&DstRect);
			SDL_DestroyTexture(TmpTexture);
			SDL_DestroySurface(TmpSurface1);
		}
	}
}

bool FileExists(const char * FileName)
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