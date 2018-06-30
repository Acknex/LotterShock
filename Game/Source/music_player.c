#include "music_player.h"

typedef struct
{
    var handle_current;
    var handle_last;
    var volume_current;
    var volume_last;
    var crossTime;
    var maxVol;
    var musicCurrentPosition;
    STRING* currentMusic;
} music_t;

music_t music;

void music_init()
{
    music.handle_current = 0;
    music.handle_last = 0;
    music.volume_current = 0;
    music.volume_last = 0;
    music.crossTime = 0;
    music.maxVol = 100;
    music.musicCurrentPosition = 0;
    music.currentMusic = NULL;
}

void music_start(STRING* file, var crossFadeTime, var loop)
{
    if(music.currentMusic != NULL && file != NULL)
	{
        if(str_cmp(music.currentMusic, file) == 1)
			return;
    }
    music.currentMusic = NULL;
    if(music.handle_current != 0)
	{
        if(music.handle_last != 0)
		{
            media_stop(music.handle_last);
		}
		
        music.handle_last = music.handle_current;
        music.volume_last = music.volume_current;
	}
	
	if(file != NULL)
	{
        music.currentMusic = file;
#ifndef DEBUG_NO_MUSIC
        if(loop != 0)
		{
            music.handle_current = media_loop(file, NULL, 0);
		}
		else
		{
            music.handle_current = media_play(file, NULL, 0);
        }
#else
        music.handle_current = 0;
#endif
        music.musicCurrentPosition = 0;
	}
	else {
        music.handle_current = 0;
	}
    music.volume_current = 0;
    music.crossTime = crossFadeTime;
}

void music_stop()
{
    if(music.handle_current != 0)
	{
        media_stop(music.handle_current);
	}
    if(music.handle_last != 0)
	{
        media_stop(music.handle_last);
	}
}

void music_speedup(var speed)
{
    if (music_is_playing())
	{
        media_tune(music.handle_current, 0, speed, 0);
	}
}

var music_is_playing()
{
    if(music.handle_current != 0 && media_playing(music.handle_current) != 0)
        return 1;
    if(music.handle_last != 0 && media_playing(music.handle_last) != 0)
        return 1;
	return 0;
}

void music_set_master_volume(var vol)
{
    music.maxVol = vol;
}

var music_get_position()
{
    return music.musicCurrentPosition;
}

void music_update()
{
    music.musicCurrentPosition += time_frame / 16;
    if(music.handle_current != 0 && media_playing(music.handle_current) == 0)
	{
        music_start(NULL, 0, 0);
	}

    if(music.crossTime == 0)
		return;

//  DEBUG_VAR(music.volume_current, 180);

    var crossfactor = music.maxVol * (time_step / 16.0) / music.crossTime;

//  DEBUG_VAR(crossfactor, 200);
//  DEBUG_VAR(music.crossTime, 220);
//  DEBUG_VAR(music.maxVol, 240);

    if(music.handle_current != 0 && music.volume_current < music.maxVol)
	{
        music.volume_current = minv(music.volume_current + crossfactor,music. maxVol);
        media_tune(music.handle_current, music.volume_current, 0, 0);
	}
	
    if(music.handle_last != 0)
	{
        music.volume_last -= minv(music.volume_last, crossfactor);
        if(music.volume_last == 0)
		{
            media_stop(music.handle_last);
            music.handle_last = 0;
		}
        else
        {
            media_tune(music.handle_last, music.volume_last, 0, 0);
		}
	}
}
