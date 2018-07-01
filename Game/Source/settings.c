#include "settings.h"
#include "ini.h"

#include <windows.h>
#include <acknex.h>

STRING * settings_file = "settings.ini";

#define CSIDL_APPDATA 0x001A
#define SHGFP_TYPE_CURRENT 0

HRESULT SHGetFolderPath(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
#define PRAGMA_API SHGetFolderPath;shell32!SHGetFolderPathA

char settings_path[MAX_PATH];

void settings_init()
{
    // Set default values here
    settings.resolution_x = 1280;
    settings.resolution_y = 720;
    settings.fullscreen = false;

    settings.fps_limit = 60;
    settings.anisotropy = 5;
    settings.vsync = true;

    settings.game_volume = 100;

    if(ini_write("settings.ini", "Dummy", "dummy", "dummy"))
    {
        ini_write("settings.ini", "Dummy", NULL, NULL);
        str_cpy(settings_file, "settings.ini");
    }
    else
    {
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
    }
}

void settings_load()
{
    settings.fps_limit    = ini_read_int(settings_file, "Game", "fps_limit", settings.fps_limit);
    settings.anisotropy   = ini_read_int(settings_file, "Game", "anisotropy", settings.anisotropy);
    settings.vsync        = ini_read_int(settings_file, "Game", "vsync", settings.vsync);

    settings.resolution_x = ini_read_int(settings_file, "Resolution", "width", settings.resolution_x);
    settings.resolution_y = ini_read_int(settings_file, "Resolution", "height", settings.resolution_y);
    settings.fullscreen   = ini_read_int(settings_file, "Resolution", "fullscreen", settings.fullscreen);

    settings.game_volume  = ini_read_int(settings_file, "Audio", "volume", settings.game_volume);
}

void settings_save()
{
    ini_write_int(settings_file, "Game", "fps_limit", settings.fps_limit);
    ini_write_int(settings_file, "Game", "anisotropy", settings.anisotropy);
    ini_write_int(settings_file, "Game", "vsync", settings.vsync);

    ini_write_int(settings_file, "Resolution", "width", settings.resolution_x);
    ini_write_int(settings_file, "Resolution", "height", settings.resolution_y);
    ini_write_int(settings_file, "Resolution", "fullscreen", settings.fullscreen);

    ini_write_int(settings_file, "Audio", "volume", settings.game_volume);
}
