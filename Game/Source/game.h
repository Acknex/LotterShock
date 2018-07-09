#ifndef GAME_H
#define GAME_H

var story_enginesEnabled = 0;
var story_powercoreEnabled = 0;

// 0 = init
// 1 = server room was entered
// 2 = matrix terminal first time activated
// 3 = matrix terminal second time activated, matrix is now enabled
var story_serverRoomState = 0;

bool game_hidehud;

void game_init();

void game_open();

void game_update();

void game_close();

bool game_is_done();

// Starts the final cutscene
void game_set_complete();

#endif // GAME_H
