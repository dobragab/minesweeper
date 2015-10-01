#include "main.h"
#ifndef MINECONFIG_H_INCLUDED
#define MINECONFIG_H_INCLUDED

bool LoadConfig(Config*);
bool TrimConfig(Config*);
void SaveConfig(const Config*, const BestTime*);
bool GetBestTimes(BestTime*);
bool TrimTimes(BestTime*);

Preset GetPreset(Config);
MineMode GetMineMode(Config);
void CopyPreset(Config*, Config);

#endif // MINECONFIG_H_INCLUDED
