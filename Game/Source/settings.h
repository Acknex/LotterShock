#ifndef SETTINGS_H
#define SETTINGS_H

#include "bestiary.h"

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

    // Input
    var input_sensitivity;
} settings_t;

typedef struct achievements_t
{
    bool bestiary_unlocked[BEAST_COUNT];
} achievements_t;

settings_t settings;

achievements_t achievements;

void settings_init();

void settings_load();

void settings_save();

void achievements_load();

void achievements_save();

#endif // SETTINGS_H
