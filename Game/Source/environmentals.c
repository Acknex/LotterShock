#include "environmentals.h"
#include <acknex.h>

SOUND* snd_terminal = "keys_engine_room.wav";

var environ_ribanna_music;

// skill1: SPEED
action environ_fake_cloud()
{	
    set(me, PASSABLE);
    set(me, TRANSLUCENT);
    my->flags2 |= UNTOUCHABLE;

    my.alpha = 50 + random(50);    
    my->ENVIRONMENTALS_TEMP = integer(random(1));

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_FAKECLOUD;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

// skill1: SPEED
action environ_ice_cloud()
{	
    set(me, PASSABLE);
    set(me, TRANSLUCENT);
    my->flags2 |= UNTOUCHABLE;

    my.alpha = 20 + random(80);    
    my->ENVIRONMENTALS_TEMP = integer(random(1));

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_ICECLOUD;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

// skill1: SPEED
action environ_toxic_cloud()
{	
    set(me, PASSABLE);  
    set(me, LIGHT);
    set(me, TRANSLUCENT);
    my->flags2 |= UNTOUCHABLE;

    vec_set(my->blue, vector(0,255,0)); 

    my.alpha = 30 + random(80);    
    my->ENVIRONMENTALS_TEMP = integer(random(1));

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_TOXICCLOUD;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

action environ_engine_beam()
{	
    set(me, PASSABLE);  
    set(me, LIGHT);
    set(me, TRANSLUCENT);
    my->flags2 |= UNTOUCHABLE;

    vec_set(my->blue, vector(255,0,0)); 

    my.alpha = 0;    
    my->ENVIRONMENTALS_TEMP = 0;
    my->ENVIRONMENTALS_TIMER = 60 + random(120);

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_ENGINE_BEAM;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}


action environ_engterm()
{	
    my->ENVIRONMENTALS_TEMP = ENVIRONMENTAL_TERMINAL_INACTIVE;
    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_ENGINE_TERMINAL;
    my->INTERACTIBLE = 1;
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

action environ_ice()
{
    my->ENVIRONMENTALS_TEMP = 0;
    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_ICE;
    my->ENVIROMENTALS_ICE_DAMAGE = 100; // you can melt me!
    my->ENVIROMENTALS_ICE_OFFSET = my->z;
    set(my, POLYGON);
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

action EnvironAirlock()
{
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_AIRLOCK;
    my->ENVIRONMENTALS_TEMP = 0;
}

action EnvironRadio()
{
    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_RADIO;

    if(environ_ribanna_music != 0)
        error("Two EnvironRadios are not allowed!");

    // midi can't be tuned!
    environ_ribanna_music = media_loop("media/ribanna.mp3", NULL, 0);
}

action environ_server()
{
    my->skin = 1 + integer(random(2));

    my->ENVIRONMENTALS_TIMER = 5 + random(10);
    my->ENVIRONMENTALS_TEMP = random(my->ENVIRONMENTALS_TIMER);

    my->ENVIRONMENTALS_TYPE = ENVIRONMENTAL_SERVER;

    framework_setup(my, SUBSYSTEM_ENVIRONMENT);
}

void environmentals_close()
{
    if(environ_ribanna_music != 0)
        media_stop(environ_ribanna_music);
    environ_ribanna_music = 0;
}

void environmentals_engine_terminal_starting(ENTITY *ptr)
{
	fog_color = 2;
	camera.fog_end = 20000.0;
}
void environmentals_terminal(ENTITY* ptr, void *starting_effect, var *flag)
{
    
    switch(ptr.ENVIRONMENTALS_TEMP)
	{
		case ENVIRONMENTAL_TERMINAL_INACTIVE:
    		if(mouse_ent == ptr) 
            {
                ptr.skin = 2;
                if(input_hit(INPUT_USE)) 
                {
                    snd_play(snd_terminal, 100, 0);
                    ptr.ENVIRONMENTALS_TEMP = ENVIRONMENTAL_TERMINAL_STARTING;
                }
            }
            else
            {
                ptr.skin = 1;
            }
			break;
		case ENVIRONMENTAL_TERMINAL_STARTING:
    		
    		ptr.skin = 3;
		    ptr.ENVIRONMENTALS_TIMER += 1 * time_step;
		
		    if(ptr.ENVIRONMENTALS_TIMER >= 30)
		    {
	    		function starting(ENTITY*);
	    		starting = starting_effect;
	    		starting(ptr);
    			ptr.ENVIRONMENTALS_TEMP = ENVIRONMENTAL_TERMINAL_ACTIVE;
    			(*flag) = 1;
    		}
			break;
		case ENVIRONMENTAL_TERMINAL_ACTIVE:
        	ptr.skin = 4;
			break;
	}
}

void environmentals_update()
{
    ENTITY *ptr;
    var dmgTest;
    var dist;

    SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENVIRONMENT) 
    {
        switch(ptr.ENVIRONMENTALS_TYPE) 
        {
            case ENVIRONMENTAL_SERVER:
                ptr->ENVIRONMENTALS_TEMP += time_step;
                if(ptr->ENVIRONMENTALS_TEMP >= ptr->ENVIRONMENTALS_TIMER)
                {
                    ptr->ENVIRONMENTALS_TEMP = 0;
                    ptr->skin = 1 + integer(random(2));
                }
                break;

            case ENVIRONMENTAL_RADIO:
                dist = vec_dist(ptr->x, camera->x);
                dist = (clamp(dist, RIBANNA_MINDIST, RIBANNA_MAXDIST) - RIBANNA_MINDIST) / ((RIBANNA_MAXDIST - RIBANNA_MINDIST) / 100);

                if(dist >= 98)
                {
                    media_pause(environ_ribanna_music);
                }
                else
                {
                    media_start(environ_ribanna_music);
                    media_tune(environ_ribanna_music, 100 - dist, 0, 0);
                }

                break;

            case ENVIRONMENTAL_AIRLOCK:
                ent_animate(ptr, "close", ptr->ENVIRONMENTALS_TEMP, 0);
                ptr->ENVIRONMENTALS_TEMP = clamp(ptr->ENVIRONMENTALS_TEMP + time_step, 0, 100);
                if(ptr->ENVIRONMENTALS_TEMP >= 100)
                    framework_freeze(ptr);
                break;

            case ENVIRONMENTAL_FAKECLOUD:
                ptr.roll += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * time_step;
                if(ptr.ENVIRONMENTALS_TEMP == 0)
                {
                    ptr.alpha -= ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 5 * time_step;
                    if(ptr.alpha <= 50)
                        ptr.ENVIRONMENTALS_TEMP = 1;
                }
                else
                {
                    ptr.alpha += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 5 * time_step;
                    if(ptr.alpha >= 100)
                        ptr.ENVIRONMENTALS_TEMP = 0;
                }
                break;	
            case ENVIRONMENTAL_ICECLOUD:
                ptr.roll += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * time_step;
                if(ptr.ENVIRONMENTALS_TEMP == 0)
                {
                    ptr.alpha -= ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 5 * time_step;
                    if(ptr.alpha <= 20)
                        ptr.ENVIRONMENTALS_TEMP = 1;
                }
                else
                {
                    ptr.alpha += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 5 * time_step;
                    if(ptr.alpha >= 100)
                        ptr.ENVIRONMENTALS_TEMP = 0;
                }
                break;	
            case ENVIRONMENTAL_TOXICCLOUD:
                ptr.roll += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * time_step;
                if(ptr.ENVIRONMENTALS_TEMP == 0)
                {
                    ptr.alpha -= ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 3 * time_step;
                    if(ptr.alpha <= 30)
                        ptr.ENVIRONMENTALS_TEMP = 1;
                }
                else
                {
                    ptr.alpha += ptr.ENVIRONMENTALS_FAKECLOUD_ROTATESPEED * 3 * time_step;
                    if(ptr.alpha >= 100)
                        ptr.ENVIRONMENTALS_TEMP = 0;
                }

                // A bit overkill to do it here but I'm lazy
                if(playerHasHazmat == 0)
                {
                    dmgTest = region_check("region_toxic", vector(player.x + player.min_x, player.y + player.min_y, player.z + player.min_z), vector(player.x + player.max_x, player.y + player.max_y, player.z + player.max_z));
                    if(dmgTest > 0)
                        playerAddHealth(-ENVIRONMENTAL_DAMAGE_TOXIC);
                }
                break;
            case ENVIRONMENTAL_ENGINE_BEAM:
                if(story_enginesEnabled == 1)
                {
                    if(ptr.ENVIRONMENTALS_TEMP > ptr.ENVIRONMENTALS_TIMER)
                    {
                        if(ptr.alpha < 100) 
                        {
                            ptr.alpha += 20 * time_step;
                            ptr.lightrange += 400 * time_step;
                        }
                        else if(random(100) > 95)
                        {
                            ptr.alpha = 75;
                            ptr.lightrange = ptr.lightrange * 0.75;
                        }

                        dmgTest = c_intersect(vector(ptr.x + (ptr.min_x * 0.77), ptr.y + (ptr.min_y * 0.77), ptr.z + (ptr.min_z * 0.77)), vector(ptr.x + (ptr.max_x * 0.77), ptr.y + (ptr.max_y * 0.77), ptr.z + (ptr.max_z * 0.77)), NULL, vector(player.x + player.min_x, player.y + player.min_y, player.z + player.min_z), vector(player.x + player.max_x, player.y + player.max_y, player.z + player.max_z), NULL);
                        if(dmgTest == -1)
                            playerAddHealth(-ENVIRONMENTAL_DAMAGE_BEAM);
                    }
                    else
                    {
                        ptr.ENVIRONMENTALS_TEMP += 2 * time_step;
                    }
                }
                break;
            
            case ENVIRONMENTAL_ENGINE_TERMINAL:
            	environmentals_terminal(ptr, environmentals_engine_terminal_starting, &story_enginesEnabled);
                break;

            case ENVIRONMENTAL_ICE:
                if(ptr->ENVIROMENTALS_ICE_DAMAGE <= 0)
                {
                    ptr->SK_ENTITY_DEAD = 1;
                }
                else
                {
                    ptr->z = ptr->ENVIROMENTALS_ICE_OFFSET - 4 * (100 - ptr->ENVIROMENTALS_ICE_DAMAGE);
                    ptr->scale_z = 0.75 + ptr->ENVIROMENTALS_ICE_DAMAGE / 400;
                    ptr->scale_x = 1.0  - ptr->ENVIROMENTALS_ICE_DAMAGE / 400;
                    ptr->v = -0.01 * ptr->ENVIROMENTALS_ICE_DAMAGE;
                }
                // TODO: i can haz logic?!
                break;
        }
    }	
}
