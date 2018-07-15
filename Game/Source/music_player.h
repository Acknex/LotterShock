#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

// initializes the music library
void music_init();

// starts new music, cross fades with old one over crossFadeTime (in seconds)
// if file is NULL, the current music is faded out
// set loop to 1 to loop music
void music_start(STRING* file, var crossFadeTime, var loop);

// Change music speed. 100 for normal
void music_speedup(var speed);

// stops all music immediately
void music_stop();

// returns 1 if music is still played (even fading out)
var music_is_playing();

// gets current play position
var music_get_position();

// gets the time offset to the start of the current song
var music_get_time();

// updates volumes. run in main loop
void music_update();

// 0..100
void music_set_master_volume(var vol);

#endif
