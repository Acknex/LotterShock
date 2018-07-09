#ifndef GAME_H
#define GAME_H

bool game_hidehud;

void game_init();

void game_open();

void game_update();

void game_close();

bool game_is_done();

// Starts the final cutscene
void game_set_complete();

#endif // GAME_H
