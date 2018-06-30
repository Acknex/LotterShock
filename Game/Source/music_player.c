var handle_current = 0;
var handle_last = 0;
var volume_current = 0;
var volume_last = 0;
var crossTime = 0;
var maxVol = 100;
var musicCurrentPosition = 0;
STRING* currentMusic = NULL;

void startMusic(STRING* file, var crossFadeTime, var loop)
{
	if(currentMusic != NULL && file != NULL)
	{
		if(str_cmp(currentMusic, file) == 1)
			return;
	}
	//error(file);
	currentMusic = NULL;
	if(handle_current != 0)
	{
		if(handle_last != 0)
		{
			media_stop(handle_last);
		}
		
		handle_last = handle_current;
		volume_last = volume_current;
	}
	
	if(file != NULL)
	{
		currentMusic = file;
		if(loop == 1)
		{
			handle_current = media_loop(file, NULL, 0);
		}
		else
		{
			handle_current = media_play(file, NULL, 0);
		}
		musicCurrentPosition = 0;
	}
	else {
		handle_current = 0;
	}
	volume_current = 0;
	crossTime = crossFadeTime;
}

void stopMusic()
{
	if(handle_current != 0)
	{
		media_stop(handle_current);
	}
	if(handle_last != 0)
	{
		media_stop(handle_last);
	}
}

void speedupMusic(var speed)
{
	if (playingMusic())
	{
		media_tune(handle_current, 0, speed, 0);
	}
}

var playingMusic()
{
	if(handle_current != 0 && media_playing(handle_current) != 0)
		return 1;
		if(handle_last != 0 && media_playing(handle_last) != 0)
		return 1;
	return 0;
}

void setMusicMasterVolume(var vol)
{
	maxVol = vol;
}

var getMusicPosition() {
	return musicCurrentPosition;
}

void updateMusic()
{
	musicCurrentPosition += time_frame/16;
	if(handle_current != 0 && media_playing(handle_current) == 0)
	{
		startMusic(NULL, 0, 0);
	}
	if(crossTime == 0)
		return;
	//DEBUG_VAR(volume_current, 180);
	var crossfactor = time_step/16.0/crossTime*maxVol;
	//DEBUG_VAR(crossfactor, 200);
	//DEBUG_VAR(crossTime, 220);
	if(handle_current != 0 && volume_current < maxVol)
	{
		volume_current = minv(volume_current+crossfactor, maxVol);
		media_tune(handle_current, volume_current, 0, 0);
	}
	
	if(handle_last != 0)
	{
		volume_last -= minv(volume_last, crossfactor);
		if(volume_last == 0)
		{
			media_stop(handle_last);
			handle_last = 0;
		}
		else {
			media_tune(handle_last, volume_last, 0, 0);
		}
	}
}
