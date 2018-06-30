#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

// starts new music, cross fades with old one over crossFadeTime (in seconds)
// if file is NULL, the current music is faded out
// set loop to 1 to loop music
void startMusic(STRING* file, var crossFadeTime, var loop);

// Change music speed. 100 for normal
void speedupMusic(var speed);

// stops all music immediately
void stopMusic();

// returns 1 if music is still played (even fading out)
var playingMusic();

// gets current play position
var getMusicPosition();

// updates volumes. run in main loop
void updateMusic();

// 0..100
void setMusicMasterVolume(var vol);

#include "music_player.c"

#endif
