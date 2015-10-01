#include <stdio.h>
#include <string.h>
#include <math.h>

#include "main.h"
#include "iniparser.h"

/* LoadConfig(...) function *******************************************************************************

Description:    Loads the config from config.ini to cfg
Parameters:         Config          *cfg        (the pointer to the struct to store the config)
Returns:        boolean value if the loading was successful && loaded config as parameter
********************************************************************************************************* */
bool LoadConfig(Config *cfg)
{
    char * ini_name = "config.ini";
    dictionary  *   ini ;
    bool result;

    ini = iniparser_load(ini_name);
    if(ini == NULL)
        result = false;
    else
    {
        cfg->Dim.x = iniparser_getint(ini, "Config:Width",  0);
        cfg->Dim.y = iniparser_getint(ini, "Config:Height", 0);
        cfg->Mines = iniparser_getint(ini, "Config:Mines", 0);
        cfg->mm = GetMineMode(*cfg);
        cfg->prst = GetPreset(*cfg);
        cfg->bQmark = iniparser_getboolean(ini, "Config:Qmark", true);
        cfg->bBnw   = iniparser_getboolean(ini, "Config:Bnw",   false);
        cfg->bSound = iniparser_getboolean(ini, "Config:Sound", false);
        result = true;
    }

    iniparser_freedict(ini);
    return result;
}

/* TrimConfig(...) function *******************************************************************************

Description:    Trims the config *cfg: limits dimensions, number of mines && counts preset and minemode
Parameters:         Config          *cfg        (the pointer to the struct to trim)
Returns:        boolean value if trimming was necessary && trimmed config as parameter
********************************************************************************************************* */
bool TrimConfig(Config *cfg)
{
    bool a, b, c;
    a = CapR(&cfg->Dim.x, MIN_WIDTH, MAX_WIDTH);
    b = CapR(&cfg->Dim.y, MIN_HEIGHT, MAX_HEIGHT);
    c = CapR(&cfg->Mines, MIN_MINES, (cfg->Dim.x - 1) * (cfg->Dim.y - 1));

    cfg->prst = GetPreset(*cfg);
    cfg->mm = GetMineMode(*cfg);

    return a || b || c;
}

/* GetPreset(...) function *********************************************************************************

Description:    Checks if the currently set config equals to any stock preset
Parameters:         Config          currentprst     (the struct to check)
Returns:        the stock preset, or custom
********************************************************************************************************* */
Preset GetPreset(Config currentprst)
{
    Config prsts[3] =
    {   { { 9, 9 }, 10 },
        { { 16, 16 }, 40 },
        { { 30, 16 }, 99 }  };

    int i;
    for(i=0; i < 3; i++)
        if(currentprst.Dim.x == prsts[i].Dim.x && currentprst.Dim.y == prsts[i].Dim.y && currentprst.Mines == prsts[i].Mines)
            return i+1;
    return custom;
}

/* GetMineMode(...) function *******************************************************************************

Description:    Counts the mode of generating mines of cfg
Parameters:         Config          cfg         (the struct to check)
Returns:        the mine mode
********************************************************************************************************* */
MineMode GetMineMode(Config cfg)
{
    if(cfg.prst != custom)
        cfg.mm = lucker;
    else if(cfg.Mines <= sqrt( (cfg.Dim.x*cfg.Dim.x + cfg.Dim.y*cfg.Dim.y) / 2) )
        cfg.mm = badass;
        //cfg.mm = lucker;
    else
        cfg.mm = ran;
        //cfg.mm = badass;

    return cfg.mm;
}

/* SaveConfig(...) function *******************************************************************************

Description:    Saves the config and best times from *cfg and *times to config.ini
Parameters:         const Config        *cfg        (the pointer to the struct to save to config.ini)
                    const BestTime      *times      (the array of best times and names to save)
Returns:        void
NOTE:           config and times must be saved together due to the way iniparser works :(
********************************************************************************************************* */
void SaveConfig(const Config *cfg, const BestTime *times)
{
    char ini_name[] = "config.ini";

    dictionary * ini;
    ini = iniparser_load(ini_name);

    FILE * fp;
    fp = fopen(ini_name, "wt");
/*
    FILE * dfp = fopen("bullshit.txt", "wt");
    iniparser_dump(ini, stdout);
    fclose(dfp);
*/
    char number[4];

    iniparser_set(ini, "Config", "");

    sprintf(number, "%d", cfg->Dim.x);
    iniparser_set(ini, "Config:Width", number);
    sprintf(number, "%d", cfg->Dim.y);
    iniparser_set(ini, "Config:Height", number);
    sprintf(number, "%d", cfg->Mines);
    iniparser_set(ini, "Config:Mines", number);

    iniparser_set(ini, "Config:Qmark", cfg->bQmark ? "true" : "false");
    iniparser_set(ini, "Config:Bnw", cfg->bBnw ? "true" : "false");
    iniparser_set(ini, "Config:Sound", cfg->bSound ? "true" : "false");

    iniparser_set(ini, "Besttimes", "");

    //sorry for ugly code duplication, I was lazy to use another sprintf for Time%d and Name%d
    sprintf(number, "%d", times[0].Time);
    iniparser_set(ini, "Besttimes:Time1", number);
    iniparser_set(ini, "Besttimes:Name1", times[0].Name);
    sprintf(number, "%d", times[1].Time);
    iniparser_set(ini, "Besttimes:Time2", number);
    iniparser_set(ini, "Besttimes:Name2", times[1].Name);
    sprintf(number, "%d", times[2].Time);
    iniparser_set(ini, "Besttimes:Time3", number);
    iniparser_set(ini, "Besttimes:Name3", times[2].Name);

    iniparser_dump_ini(ini, fp);

    iniparser_freedict(ini);
    fclose(fp);
}

/* GetBestTimes(...) function ******************************************************************************

Description:    Loads the best times from config.ini to *times, the pair of GetConfig(...)
Parameters:         BestTime      *times      (the array of best times and names to fill)
Returns:        a boolean value if loading was successful
********************************************************************************************************* */
bool GetBestTimes(BestTime *times)
{
    char * ini_name = "config.ini";
    dictionary  *   ini ;
    bool result;

    ini = iniparser_load(ini_name);

    if(ini == NULL)
    {
        result = false;
        int i;
        for(i=0; i < 3; i++)
        {
            strcpy(times[i].Name, "Anonymous");
            times[i].Time = 999;
        }
    }
    else
    {   //code duplication again :((
        result = true;
        times[0].Time = iniparser_getint   (ini, "Besttimes:Time1",  0);
        strncpy(times[0].Name, iniparser_getstring(ini, "Besttimes:Name1", ""), 17);
        times[0].Name[17] = '\0';
        times[1].Time = iniparser_getint   (ini, "Besttimes:Time2",  0);
        strncpy(times[1].Name, iniparser_getstring(ini, "Besttimes:Name2", ""), 17);
        times[1].Name[17] = '\0';
        times[2].Time = iniparser_getint   (ini, "Besttimes:Time3",  0);
        strncpy(times[2].Name, iniparser_getstring(ini, "Besttimes:Name3", ""), 17);
        times[2].Name[17] = '\0';
    }

    iniparser_freedict(ini);

    return result;
}

/* TrimTimes(...) function *********************************************************************************

Description:    Checks if all names and times are valid, and resets it to default in case not
                (the pair of TrimConfig)
Parameters:         BestTime        *times      (the best times array to check)
Returns:        boolean value if trimming was necessary && trimmed times as parameter
********************************************************************************************************* */
bool TrimTimes(BestTime *times)
{
    bool newtimes = false;
    int i;
    for(i=0; i < 3; i++)
        if(times[i].Time < 1 || times[i].Time > 999 || times[i].Name[0] == '\0')
        {
            newtimes = true;
            times[i].Time = 999;
            strcpy(times[i].Name, "Anonymous");
        }
    return newtimes;
}

/* CopyPreset(...) function ********************************************************************************

Description:    Copies the size values of preset from src to *dest, used in
Parameters:         Config          *dest       (the pointer to the destination struct)
                    Config          src         (the source struct)
Returns:        void
********************************************************************************************************* */
void CopyPreset(Config *dest, Config src)
{
    dest->Dim.x = src.Dim.x;
    dest->Dim.y = src.Dim.y;
    dest->Mines = src.Mines;
}
