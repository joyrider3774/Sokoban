#include <unistd.h>
#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_image/SDL_image.h>
#include <sys/stat.h>
#include "LevelEditorMenu.h"
#include "Common.h"
#include "GameFuncs.h"
#include "CInput.h"


void LevelEditorMenu()
{
    Input->SetInputDelay(InputDelay);
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
	    frameticks = SDL_GetPerformanceCounter();
        SDL_SetRenderTarget(Renderer, Buffer);
		if (ReloadMusic)
		{
			ReloadMusic=false;
			Mix_PlayMusic(Music[SelectedMusic],0);
			//Mix_HookMusicFinished(MusicFinished);
			SetVolume(Volume);
		}
		SDL_RenderTexture(Renderer,IMGTitleScreen,NULL,NULL);
		printTitleInfo();
        Input->Update();

        if(Input->SpecialsHeld(SPECIAL_QUIT_EV))
            GameState = GSQuit;

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_B)) || Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_B))))
		{
			if (GlobalSoundEnabled)
				Mix_PlayChannel(-1,Sounds[SND_BACK],0);
			GameState = GSTitleScreen;
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_LEFT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_LEFT))))
		{
			if(Selection==4)
				if (InstalledLevelPacksCount > 0)
				{
					SelectedLevelPack--;
					if(SelectedLevelPack < 0)
						SelectedLevelPack = InstalledLevelPacksCount - 1;
					sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
					sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
					if(!FileExists(FileName))
						sprintf(FileName,"./levelpacks/%s",LevelPackName);
					LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);
					LoadGraphics();
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_MENU],0);
					SaveSettings();
				}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_RIGHT))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_RIGHT))))
		{
			if (Selection==4)
				if (InstalledLevelPacksCount > 0)
				{
					SelectedLevelPack++;
					if(SelectedLevelPack > InstalledLevelPacksCount-1)
						SelectedLevelPack = 0;
					sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
					sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
					if(!FileExists(FileName))
						sprintf(FileName,"./levelpacks/%s",LevelPackName);
					LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);
					LoadGraphics();
					if (GlobalSoundEnabled)
						Mix_PlayChannel(-1,Sounds[SND_MENU],0);
					SaveSettings();
				}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_UP))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_UP))))
		{
			if (Selection > 1)
			{
				Selection--;
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			}
			Input->Delay();
		}

		if (Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_DOWN))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_DOWN))))
		{
			if (Selection < 5)
			{
				Selection++;
				if (GlobalSoundEnabled)
					Mix_PlayChannel(-1,Sounds[SND_MENU],0);
			}
			Input->Delay();
		}

		if ((GameState == GSLevelEditorMenu) && Input->Ready() && (Input->KeyboardHeld(JoystickSetup->GetKeyValue(BUT_A))|| Input->JoystickHeld(0,JoystickSetup->GetButtonValue(BUT_A))))
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
						SDL_RenderTexture(Renderer, IMGTitleScreen,NULL,NULL);
						printTitleInfo();
						CreatorName = GetString("","Enter the Levelpack Creator name:");
						if(strlen(CreatorName)>0)
						{
							Input->Reset();
							sprintf(LevelPackName,"%s",PackName);
							sprintf(Tekst,"%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
#ifdef WIN32
							mkdir(Tekst);
#else
							mkdir(Tekst,S_IRWXO|S_IRWXU|S_IRWXG);
#endif

							sprintf(Tekst,"%s/.sokoban_levelpacks", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"));
#ifdef WIN32
							mkdir(Tekst);
#else
							mkdir(Tekst,S_IRWXO|S_IRWXU|S_IRWXG);
#endif

							sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
#ifdef WIN32
							mkdir(Tekst);
#else
							mkdir(Tekst,S_IRWXO|S_IRWXU|S_IRWXG);
#endif
							sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/colors.txt", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
							ColorsFile = fopen(FileName,"wt");
							if (ColorsFile)
							{
								fprintf(ColorsFile,"[TextColor]\nR=79\nG=135\nB=194\n");
								fprintf(ColorsFile,"[MenuBoxColor]\nR=197\nG=215\nB=235\nA=255\n");
								fprintf(ColorsFile,"[MenuBoxBorderColor]\nR=52\nG=102\nB=148\nA=255\n");
								fclose(ColorsFile);
							}
							sprintf(FileName,"%s/.sokoban_levelpacks/%s._lev/credits.dat", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
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
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
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
								sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev/level%d.lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName,Teller);
								if(FileExists(Tekst))
								{
									remove(Tekst);
								}
							}
							sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev/credits.dat", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
							if(FileExists(Tekst))
							{
								remove(Tekst);
							}
							sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev/colors.txt", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
							if(FileExists(Tekst))
							{
								remove(Tekst);
							}
							sprintf(Tekst,"%s/.sokoban_levelpacks/%s._lev",SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"), LevelPackName);
							rmdir(Tekst);
							SearchForLevelPacks();
							LoadGraphics();
						}
						Input->Reset();

					}
					break;
				case 4:
					if (InstalledLevelPacksCount > 0)
					{
						SelectedLevelPack++;
						if(SelectedLevelPack > InstalledLevelPacksCount-1)
							SelectedLevelPack = 0;
						sprintf(LevelPackName,"%s",InstalledLevelPacks[SelectedLevelPack]);
						sprintf(FileName, "%s/.sokoban_levelpacks/%s", SDL_getenv("HOME") == NULL ? ".": SDL_getenv("HOME"),LevelPackName);
						if(!FileExists(FileName))
							sprintf(FileName,"./levelpacks/%s",LevelPackName);
						LevelPackFile->loadFile(FileName, NrOfCols, NrOfRows, true);
						LoadGraphics();
						if (GlobalSoundEnabled)
							Mix_PlayChannel(-1,Sounds[SND_MENU],0);
						SaveSettings();
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
		
		
        SDL_SetRenderTarget(Renderer, Buffer);
        SDL_FRect Rect1;
        Rect1.x = 50.0f*UI_WIDTH_SCALE;
        Rect1.y = 80.0f*UI_HEIGHT_SCALE;
        Rect1.w = 220.0f*UI_WIDTH_SCALE;
        Rect1.h = 85.0f*UI_HEIGHT_SCALE;
        SDL_SetRenderDrawColor(Renderer, MenuBoxColor.r,MenuBoxColor.g,MenuBoxColor.b,MenuBoxColor.a);
        SDL_RenderFillRect(Renderer, &Rect1);
        SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
        SDL_RenderRect(Renderer, &Rect1);
        SDL_FRect Rect2;
        Rect2.x = 52.0f*UI_WIDTH_SCALE;
        Rect2.y = 82.0f*UI_HEIGHT_SCALE;
        Rect2.w = 216.0f*UI_WIDTH_SCALE;
        Rect2.h = 81.0f*UI_HEIGHT_SCALE;
        SDL_SetRenderDrawColor(Renderer, MenuBoxBorderColor.r,MenuBoxBorderColor.g,MenuBoxBorderColor.b,MenuBoxBorderColor.a);
        SDL_RenderRect(Renderer, &Rect2);

		sprintf(Tekst,"Create New Levelpack\nLoad Selected LevelPack\nDelete Selected Levelpack\n<%s>\nMain Menu",LevelPackName);
		WriteText(BigFont,Tekst,strlen(Tekst),80*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
		if (Selection > 1)
		{
			strcpy(Tekst,"\n");
			for(Teller=2;Teller<Selection;Teller++)
				strcat(Tekst,"\n");
			strcat(Tekst,">>");
		}
		else
			strcpy(Tekst,">>");
		WriteText(BigFont,Tekst,strlen(Tekst),55*UI_WIDTH_SCALE,85*UI_HEIGHT_SCALE,2*UI_HEIGHT_SCALE,MenuTextColor,false);
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
	delete[] Tekst;
}

