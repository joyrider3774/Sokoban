#include <unistd.h>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_gfxPrimitives.h>
#include <SDL_rotozoom.h>
#include <sys/stat.h>
#include "LevelEditorMenu.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"


void LevelEditorMenu()
{
    CInput *Input = new CInput(InputDelay);
	FILE *ColorsFile,*Fp;
	int Teller, Selection = 1;
	SDL_Event Event;
	char *PackName,*CreatorName;
	char FileName[FILENAME_MAX];
	SDL_PollEvent(&Event);
	char *Tekst = new char[300];
	if(MusicCount > 0)
		if (! Mix_PlayingMusic())
			if(GlobalSoundEnabled)
			{
				SelectedMusic = 0;
				Mix_PlayMusic(Music[SelectedMusic],0);
				//Mix_HookMusicFinished(MusicFinished);
				SetVolume(Volume);
			}
	while (GameState == GSLevelEditorMenu)
	{
		if (ReloadMusic)
		{
			ReloadMusic=false;
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
		SDL_BlitSurface(IMGTitleScreen,NULL,Buffer,NULL);

        Input->Update();

        if(Input->SpecialsHeld[SPECIAL_QUIT_EV])
            GameState = GSQuit;

		if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_B)] || Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_B)]))
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
			GameState = GSTitleScreen;
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_LEFT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_LEFT)]))
		{
			if(Selection==4)
				if (InstalledLevelPacksCount > 0)
					if(SelectedLevelPack > 0)
					{									
						SelectedLevelPack--;
						sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
						sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
						if(!FileExists(FileName))
							sprintf(FileName,"./levelpacks/%s",LevelPackName);
						LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);													
						LoadGraphics();
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_MENU],0);
					}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_RIGHT)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_RIGHT)]))
		{
			if (Selection==4)
				if (InstalledLevelPacksCount > 0)
					if(SelectedLevelPack < InstalledLevelPacksCount-1)
					{
						SelectedLevelPack++;
						sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
						sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
						if(!FileExists(FileName))
							sprintf(FileName,"./levelpacks/%s",LevelPackName);
						LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);						
						LoadGraphics();
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_MENU],0);
					}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_UP)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_UP)]))
		{
			if (Selection > 1)
			{
				Selection--;
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_DOWN)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_DOWN)]))
		{
			if (Selection < 5)
			{
				Selection++;
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			}
			Input->Delay();
		}

		if ((GameState == GSLevelEditorMenu) && Input->Ready() && (Input->KeyboardHeld[JoystickSetup->GetKeyValue(BUT_A)]|| Input->JoystickHeld[0][JoystickSetup->GetButtonValue(BUT_A)]))
		{
			switch(Selection)
			{
				case 1:
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					PackName = GetString("","Enter the Levelpack name:");
					if (strlen(PackName) > 0)
					{
						Input->Reset();
						SDL_BlitSurface(IMGTitleScreen,NULL,Buffer,NULL);
						CreatorName = GetString("","Enter the Levelpack Creator name:");
						if(strlen(CreatorName)>0)
						{
							Input->Reset();
							sprintf(LevelPackName,"%s",PackName);
							sprintf(LevelPackName,"%s",PackName);
							sprintf(Tekst,"%s", getenv("HOME") == NULL ? ".": getenv("HOME"));
#ifdef WIN32
							mkdir(Tekst);
#else
							mkdir(Tekst,S_IRWXO|S_IRWXU|S_IRWXG);
#endif

							sprintf(Tekst,"%s/.sokoban_levelpacks", getenv("HOME") == NULL ? ".": getenv("HOME"));
#ifdef WIN32
							mkdir(Tekst);
#else
							mkdir(Tekst,S_IRWXO|S_IRWXU|S_IRWXG);
#endif

							sprintf(Tekst,"%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
#ifdef WIN32
							mkdir(Tekst);
#else
							mkdir(Tekst,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
							sprintf(FileName,"./.sokoban_levelpacks/%s/colors.txt",LevelPackName);
							ColorsFile = fopen(FileName,"wt");
							if (ColorsFile)
							{
								fprintf(ColorsFile,"[TextColor]\nR=79\nG=135\nB=194\n");
								fprintf(ColorsFile,"[MenuBoxColor]\nR=197\nG=215\nB=235\nA=255\n");
								fprintf(ColorsFile,"[MenuBoxBorderColor]\nR=52\nG=102\nB=148\nA=255\n");
								fclose(ColorsFile);
							}
							sprintf(FileName,"%s/.sokoban_levelpacks/%s/credits.dat", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
							Fp = fopen(FileName,"wt");
							if (Fp)
							{
								fprintf(Fp,"[Credits]\nCreator='%s'\n",CreatorName);
								fclose(Fp);
							}
							SearchForLevelPacks();
							for (Teller=0;Teller<InstalledLevelPacksCount;Teller++)
								if(strcmp(PackName,InstalledLevelPacks[Teller]) == 0)
								{
									SelectedLevelPack = Teller;
									sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
									sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
								}
						}
						LoadGraphics();									
						delete[] CreatorName;
					}
					delete[] PackName;
					Input->Reset();
					break;
				case 2:
					if (InstalledLevelPacksCount >0)
					{
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", getenv("HOME") == NULL ? ".": getenv("HOME"),LevelPackName);
						if(!FileExists(FileName))
							sprintf(FileName,"./levelpacks/%s",LevelPackName);
						LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, false);						
						FindLevels();
						SelectedLevel=0;
						GameState=GSStageSelect;
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					}
					break;
				case 3:
					if (InstalledLevelPacksCount >0)
					{
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
						sprintf(Tekst,"do you want to delete the selected level pack:\n\"%s\"\nAll Levels in Levelpack will be deleted !!!\n\nPress A to Delete, X to Cancel",InstalledLevelPacks[SelectedLevelPack]);
						if(AskQuestion(Tekst))
						{
							FindLevels();
							for(Teller=1;Teller<=InstalledLevels;Teller++)
							{
								sprintf(Tekst,"%s/.sokoban_levelpacks/%s/level%d.lev",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName,Teller);
								if(FileExists(Tekst))
								{
									remove(Tekst);
								}
							}
							sprintf(Tekst,"%s/.sokoban_levelpacks/%s/credits.dat", getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
							if(FileExists(Tekst))
							{
								remove(Tekst);
							}
							sprintf(Tekst,"%s/.sokoban_levelpacks/%s",getenv("HOME") == NULL ? ".": getenv("HOME"), LevelPackName);
							rmdir(Tekst);
							SearchForLevelPacks();
							LoadGraphics();
						}
						Input->Reset();

					}
					break;
				case 5:
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_SELECT],0);
					GameState = GSTitleScreen;
					break;
			}
			Input->Delay();
		}
		
		printTitleInfo(Buffer);

		boxRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.unused);
		rectangleRGBA(Buffer,50*UI_WIDTH_SCALE,80*UI_HEIGHT_SCALE,270*UI_WIDTH_SCALE,160*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		rectangleRGBA(Buffer,51*UI_WIDTH_SCALE,81.5*UI_HEIGHT_SCALE,269*UI_WIDTH_SCALE,159*UI_HEIGHT_SCALE,MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.unused);
		sprintf(Tekst,"Create New Levelpack\nLoad Selected LevelPack\nDelete Selected Levelpack\n<%s>\nMain Menu",LevelPackName);
		WriteText(Buffer,BigFont,Tekst,strlen(Tekst),80*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2,MenuTextColor);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		WriteText(Buffer,BigFont,Tekst,strlen(Tekst),55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2,MenuTextColor);
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
	delete Input;
	delete[] Tekst;
}

