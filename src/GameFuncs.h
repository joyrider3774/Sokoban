#ifndef GAMEFUNCS_H
#define GAMEFUNCS_H

void SetVolume(const int VolumeIn);
void IncVolume();
void DecVolume();
void MusicFinished();
void LoadSettings();
void SaveSettings();
void SearchForMusic();
void SearchForLevelPacks();
void LoadUnlockData();
void SaveUnlockData();
void PrintForm(char *msg);
bool AskQuestion(char *Msg);
char *GetString(char *NameIn,char *Msg);
void LoadGraphics();
void FindLevels();
void UnloadSounds();
void LoadSounds();
void UnloadMusic();
void UnLoadGraphics();
void LoadJoystickSettings();
#endif