#include "intro.h"
#include "global.h"
#include "framework.h"
#include "journals.h"
#include "input.h"

#include <acknex.h>

struct
{
    int state;
    var timer;
    bool done;
    bool distressDone;
    bool flybyDone;
    var flybyTime;
} intro;

void intro_init()
{
}


bool intro_is_done()
{
    return intro.done;
}

ENTITY* introsky;
void intro_open()
{
    level_load("intro.wmb");

    music_start(NULL, 1, false);
    
    // tricky trick:
    // uss patchnotes hinter depthplane verstecken :P
    camera->clip_far = 20000;

    introsky = ent_createlayer("sky_1+6.png", SHOW|CUBE|SKY, 100);
    bmap_to_cubemap(ent_getskin(introsky, 0));

    intro.state = 0; // use 3 to start in second cutscene
    intro.timer = 0;
    intro.distressDone = false;
    intro.flybyDone = false;
    intro.done = false;
    intro.flybyTime = 0;

    fog_color = 0;
}

void intro_close()
{
	ptr_remove(introsky);
    level_load(NULL);
}

// Ship is the only thing, rotate the camera
void intro_camera_roto(ENTITY * ackwing)
{
    // somewhere "behind" the patchnotes
    vec_set(ackwing->x, vector(0, 25000, 0));
    vec_set(ackwing->pan, nullvector);

    vec_set(camera->x, vector(1000, 0, 150 + 200 * sin(0.01 * total_ticks)));
    vec_rotate(camera->x, vector(0.2 * total_ticks, 0, 0));
    vec_add(camera->x, ackwing->x);

    // look at ackwing
    vec_diff(camera->pan, ackwing->x, camera->x);
    vec_to_angle(camera->pan, camera->pan);
    camera->roll = 0;
}

void intro_camera_flyby(ENTITY * ackwing)
{
    vec_set(camera->x, vector(-1000 + intro.flybyTime, -2500, 0));
    vec_set(camera->pan, vector(90,0,0));

    // somewhere "behind" the patchnotes
    vec_set(ackwing->x, camera->x);
    ackwing->x -= 1000;
    ackwing->x += 0.5 * intro.flybyTime;
    ackwing->y += 2000;
    ackwing->z -= 500;
    vec_set(ackwing->pan, nullvector);

    intro.flybyTime += 6.0 * time_step;
}

void intro_update()
{
    // HACK: We only have one entity, so the first fitting entity we find is our ackwing
    ENTITY * ackwing = NULL;
    SUBSYSTEM_LOOP(ackwing, SUBSYSTEM_INTRO) break;

    if(!ackwing) error("intro_update: No AckWing found!");

    switch(intro.state)
    {
    case 0: // wait for sequence start
        intro_camera_roto(ackwing);
        if(intro.timer >= 16)
        {
            journals_play(50, JOURNAL_LEVEL_STORY);
            intro.state = 1;
        }
        intro.timer += time_step;
        break;

    case 1: // wait for distress sequence end
        intro_camera_roto(ackwing);
        if(intro.distressDone)
        {
            intro.state = 2;
            intro.timer = 0;
        }
        break;

    case 2: // fade to black
        intro.timer += 10 * time_step;
        if(intro.timer >= 200)
        {
            intro.timer = 100;
            intro.state = 3;
        }
        draw_quad(NULL, vector(0,0,0), NULL, &screen_size, NULL, COLOR_BLACK, clamp(intro.timer, 0, 100), 0);
        break;

    case 3: // unfade and start flyby sequence
        intro_camera_flyby(ackwing);
        intro.timer -= 10 * time_step;
        if(intro.timer <= 0)
        {
            intro.state = 4;
            journals_play(68, JOURNAL_LEVEL_STORY);
        }
        draw_quad(NULL, vector(0,0,0), NULL, &screen_size, NULL, COLOR_BLACK, clamp(intro.timer, 0, 100), 0);
        break;

    case 4: // wait for flyby scene sequece done
        intro_camera_flyby(ackwing);
        if(intro.flybyDone)
        {
            intro.state = 5;
            intro.timer = 0;
        }
        break;

    case 5:
        intro_camera_flyby(ackwing);
        intro.timer += 3.0 * time_step;
        if(intro.timer >= 140)
        {
            intro.done = true;
            intro.state = 6;
        }
        draw_quad(NULL, vector(0,0,0), NULL, &screen_size, NULL, COLOR_BLACK, clamp(intro.timer, 0, 100), 0);
        break;

    }

    if(input_hit(INPUT_JUMP) || input_hit(INPUT_ATTACK) || input_hit(INPUT_NAVBACK) || input_hit(INPUT_USE))
    {
        intro.done = true;
        intro.state = 6;
    }
}

//! ends the distress call sequence.
//! is callback for journals.
void intro_step_distress_end()
{
    intro.distressDone = true;
}

//! ends the flyby sequence.
//! is callback for journals
void intro_step_flyby_end()
{
    intro.flybyDone = true;
}

// lotters trusty ship
action AckWing()
{
    framework_setup(my, SUBSYSTEM_INTRO);
}
