#ifndef SETTINGS_H
#define SETTINGS_H

typedef struct settings_t
{
    // Resolution
    int resolution_x;
    int resolution_y;
    bool fullscreen;

    // Game
    int fps_limit;
    int anisotropy;
    bool vsync;

    // Audio
    int game_volume;
} settings_t;

settings_t settings;

void settings_init();

void settings_load();

void settings_save();

#endif // SETTINGS_H
