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
    var hsensitivity;
    var vsensitivity;
    bool invertX, invertY;
    bool has_input_mapping;
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

//! registers notification for settings updates
void settings_register_signal(void * reload);

//! sends the update notification
void settings_send_signal();

void achievements_load();

void achievements_save();

void achievement_kill_beast(int beastid);

#endif // SETTINGS_H
