#include "settings.h"
#include "ini.h"
#include "input.h"

#include <windows.h>
#include <acknex.h>
#include <stdio.h>

STRING * settings_file = "settings.ini";

#define CSIDL_APPDATA 0x001A
#define SHGFP_TYPE_CURRENT 0

HRESULT SHGetFolderPath(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
#define PRAGMA_API SHGetFolderPath;shell32!SHGetFolderPathA

char settings_path[MAX_PATH];

void * settings_signals[1000];
int settings_signal_count = 0;

void settings_register_signal(void * reload)
{
    if(reload == NULL)
        error("settings_register_signal: reload was NULL");
    settings_signals[settings_signal_count] = reload;
    settings_signal_count += 1;
}

void settings_send_signal()
{
    function foo();
    int i;
    for(i = 0; i < settings_signal_count; i++)
    {
        foo = settings_signals[i];
        foo();
    }
}

void settings_init()
{
    // Set default values here
    settings.resolution_x = 1280;
    settings.resolution_y = 720;
    settings.fullscreen = false;

    settings.fps_limit = 60;
    settings.anisotropy = 5;
    settings.vsync = true;
    settings.gamma = 100;
    settings.skipIntro = false;
    settings.retroshader = false;
    settings.bloom = true;

    settings.game_volume = 100;

    settings.hsensitivity = 13;
    settings.vsensitivity = 13;

    // try to get "%APPDATA%\AckCon\2018\settings.ini"
    if(ExpandEnvironmentStrings("%APPDATA%", settings_path, MAX_PATH) == 0)
    {
        error("Failed to expand environment variable for settings.");
        return;
    }
    str_cpy(settings_file, settings_path);
    str_cat(settings_file, "\\AckCon");
    CreateDirectory(_chr(settings_file), NULL);
    str_cat(settings_file, "\\2018");
    CreateDirectory(_chr(settings_file), NULL);
    str_cat(settings_file, "\\settings.ini");

    memset(&achievements, 0, sizeof(achievements_t));
    achievements.bestiary_unlocked[BEAST_STARLOTTI] = 1;
    achievements.bestiary_unlocked[BEAST_ACKTANA] = 1;

    achievements_load();
}

void settings_load_from(STRING * fileName)
{
    diag(str_printf(NULL, "\nLoading settings from %s", _chr(fileName)));
    settings.fps_limit    = ini_read_int(fileName, "Game", "fps_limit", settings.fps_limit);
    settings.anisotropy   = ini_read_int(fileName, "Game", "anisotropy", settings.anisotropy);
    settings.vsync        = ini_read_int(fileName, "Game", "vsync", settings.vsync);
    settings.skipIntro    = ini_read_int(fileName, "Game", "skip_intro", settings.skipIntro);
    settings.gamma        = ini_read_var(fileName, "Game", "gamma", settings.gamma);

    settings.retroshader  = ini_read_int(fileName, "Game", "retro", settings.retroshader);
    settings.bloom        = ini_read_int(fileName, "Game", "bloom", settings.bloom);

    settings.resolution_x = ini_read_int(fileName, "Resolution", "width", settings.resolution_x);
    settings.resolution_y = ini_read_int(fileName, "Resolution", "height", settings.resolution_y);
    settings.fullscreen   = ini_read_int(fileName, "Resolution", "fullscreen", settings.fullscreen);

    settings.game_volume  = ini_read_int(fileName, "Audio", "volume", settings.game_volume);

    settings.hsensitivity = ini_read_var(fileName, "Input", "hsensitivity", settings.hsensitivity);
    settings.vsensitivity = ini_read_var(fileName, "Input", "vsensitivity", settings.vsensitivity);

    settings.invertX = ini_read_var(settings_file, "Input", "invert_h", settings.invertX);
    settings.invertY = ini_read_var(settings_file, "Input", "invert_v", settings.invertY);

    if(ini_read_int(settings_file, "Input", "config_written", 0) == 1)
    {
        settings.has_input_mapping = 1;

        int i,k;
        char buffer[64];
        for(i = 0; i < INPUT_MAX; i++)
        {
            for(k = 0; k < 4; k++)
            {
                sprintf(buffer, "btn%d_%d_type", i, k);
                input[i].configs[k].type = ini_read_int(settings_file, "Input", _str(buffer), 0);

                sprintf(buffer, "btn%d_%d_index", i, k);
                input[i].configs[k].index = ini_read_int(settings_file, "Input", _str(buffer), 0);

                sprintf(buffer, "btn%d_%d_invert", i, k);
                input[i].configs[k].inverted = ini_read_int(settings_file, "Input", _str(buffer), 0);

                sprintf(buffer, "btn%d_%d_deadzone", i, k);
                input[i].configs[k].deadZone = ini_read_var(settings_file, "Input", _str(buffer), 0.3);
            }
        }
    }
}


void settings_load()
{
    settings_load_from(settings_file);
}

void settings_save()
{
    diag(str_printf(NULL, "\nSaving settings to %s", _chr(settings_file)));
    ini_write_int(settings_file, "Game", "fps_limit", settings.fps_limit);
    ini_write_int(settings_file, "Game", "anisotropy", settings.anisotropy);
    ini_write_int(settings_file, "Game", "vsync", settings.vsync);
    ini_write_int(settings_file, "Game", "skip_intro", settings.skipIntro);
    ini_write_var(settings_file, "Game", "gamma", settings.gamma);
    if(achievements.retro_unlocked) // don't spoil them!
        ini_write_var(settings_file, "Game", "retro", settings.retroshader);
    ini_write_var(settings_file, "Game", "bloom", settings.bloom);

    ini_write_int(settings_file, "Resolution", "width", settings.resolution_x);
    ini_write_int(settings_file, "Resolution", "height", settings.resolution_y);
    ini_write_int(settings_file, "Resolution", "fullscreen", settings.fullscreen);

    ini_write_int(settings_file, "Audio", "volume", settings.game_volume);

    ini_write_var(settings_file, "Input", "hsensitivity", settings.hsensitivity);
    ini_write_var(settings_file, "Input", "vsensitivity", settings.vsensitivity);

    ini_write_var(settings_file, "Input", "invert_h", settings.invertX);
    ini_write_var(settings_file, "Input", "invert_v", settings.invertY);

    int i,k;
    char buffer[64];
    for(i = 0; i < INPUT_MAX; i++)
    {
        for(k = 0; k < 4; k++)
        {
            sprintf(buffer, "btn%d_%d_type", i, k);
            ini_write_int(settings_file, "Input", _str(buffer), input[i].configs[k].type);

            sprintf(buffer, "btn%d_%d_index", i, k);
            ini_write_int(settings_file, "Input", _str(buffer), input[i].configs[k].index);

            sprintf(buffer, "btn%d_%d_invert", i, k);
            ini_write_int(settings_file, "Input", _str(buffer), input[i].configs[k].inverted);

            sprintf(buffer, "btn%d_%d_deadzone", i, k);
            ini_write_var(settings_file, "Input", _str(buffer), input[i].configs[k].deadZone);
        }
    }

    ini_write_int(settings_file, "Input", "config_written", 1);
}


void achievements_load()
{
    diag(str_printf(NULL, "\nLoading achivements from %s", _chr(settings_file)));
    int i;
    char buffer[64];
    for(i = 0; i < BEAST_COUNT; i++)
    {
        sprintf(buffer, "beast_%d", i);
        achievements.bestiary_unlocked[i] = ini_read_var(settings_file, "Achievements", buffer, achievements.bestiary_unlocked[i]);
    }
    achievements.retro_unlocked = ini_read_int(settings_file, "Achievements", "retromode", achievements.retro_unlocked);
}

void achievements_save()
{
    diag(str_printf(NULL, "\nSaving achievements to %s", _chr(settings_file)));
    int i;
    char buffer[64];
    for(i = 0; i < BEAST_COUNT; i++)
    {
        sprintf(buffer, "beast_%d", i);
        ini_write_var(settings_file, "Achievements", _str(buffer), achievements.bestiary_unlocked[i]);
    }
    if(achievements.retro_unlocked)
        ini_write_int(settings_file, "Achievements", "retromode", achievements.retro_unlocked);
}


void achievement_kill_beast(int beastid)
{
    if(achievements.bestiary_unlocked[beastid] == 0)
    {
        achievements.bestiary_unlocked[beastid] = 1;
        achievements_save();
    }
}

void achievement_reset()
{
    int i;
    for(i = 0; i < BEAST_COUNT; i++)
    {
        if(i < 2)
            achievements.bestiary_unlocked[i] = 1;
        else
            achievements.bestiary_unlocked[i] = 0;
    }
    achievements_save();
}
