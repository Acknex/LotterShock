#include "bestiary.h"
#include "input.h"
#include "music_player.h"
#include "mainmenu.h"
#include "global.h"
#include "framework.h"
#include "music_player.h"

#include <acknex.h>

#define BESTIARY_COUNT 7

typedef struct beast_t
{
    char const * name;
    char const * flavour;
} beast_t;

BMAP * bestiary_bmp_next = "best_next.png";
BMAP * bestiary_bmp_prev = "best_prev.png";
BMAP * bestiary_bmp_back = "best_back.png";

BMAP * bestiary_selection_bmap = "best_sel.png";

PANEL * bestiary_pan_back =
{
    bmap = bestiary_bmp_back;
}

PANEL * bestiary_pan_next =
{
    bmap = bestiary_bmp_next;
}

PANEL * bestiary_pan_prev =
{
    bmap = bestiary_bmp_prev;
}

PANEL * bestiary_pan_sel =
{
    bmap = mainmenu_selection_bmap;
    flags = UNTOUCHABLE;
}

struct
{
    int position;
    bool done;
    PANEL * lastPan;
    beast_t beasts[BESTIARY_COUNT];
} bestiary;

void bestiary_init()
{
    bestiary.beasts[0].name    = "Star Lotti";
    bestiary.beasts[0].flavour = "A mysterious adventurer from space. Follower of the Great Acknex.";

    bestiary.beasts[1].name    = "Acktana";
    bestiary.beasts[1].flavour = "Star Lottis faithful companion AI.\nCreated by the Great Acknex itself, it was written in Lite-C\nto fully unleash its mighty intellect upon the unity union.";

    bestiary.beasts[2].name    = "Donkey-Lark";
    bestiary.beasts[2].flavour = "A small animal from the far lands of Bielefeld.\nLikes to graze fresh flowers, but has to puke afterwards.\nBe careful, as donkey-larks might explode when beeing startled!";

    bestiary.beasts[3].name    = "Sputnik";
    bestiary.beasts[3].flavour = "A machine embedded with a human brain and a TV as a head.\nIt was meant to be a super soldier, but it is just super mad.";

    bestiary.beasts[4].name    = "Whiskas Skull";
    bestiary.beasts[4].flavour = "An evil floating skull that wants to eat your soul.";

    bestiary.beasts[5].name    = "Eye of the Acknex";
    bestiary.beasts[5].flavour = "It's a floating, ever watching eye.\nIf it crosses your way you have been blessed by the Great Acknex itself.";

    bestiary.beasts[6].name    = "Turret";
    bestiary.beasts[6].flavour = "Your very personal bullet hell.";
}

void bestiary_open()
{
	fog_color = 2;
	camera.fog_end = 20000.0;
    level_load("bestiary.wmb");

    vec_set(camera->x, vector(0, 0, 180));
    vec_zero(camera->pan);
    camera->arc = 90;

    bestiary.done = false;
    bestiary.position = 0;

    music_start("Media/blirg.mp3", 0.5, true);

    set(bestiary_pan_back, SHOW);
    set(bestiary_pan_prev, SHOW);
    set(bestiary_pan_next, SHOW);
}

void bestiary_update()
{
    int inital = bestiary.position;

    if(input_hit(INPUT_ATTACK) && mouse_panel != NULL)
    {
        if(mouse_panel == bestiary_pan_back)
            bestiary.done = true;
        if(mouse_panel == bestiary_pan_next)
            bestiary.position += 1;
        if(mouse_panel == bestiary_pan_prev)
            bestiary.position -= 1;
    }

    if(mouse_panel != NULL)
    {
        set(bestiary_pan_sel, SHOW);
        bestiary_pan_sel->pos_x = mouse_panel->pos_x;
        bestiary_pan_sel->pos_y = mouse_panel->pos_y;

        if(mouse_panel == bestiary_pan_back)
        {
            bestiary_pan_sel->bmap = mainmenu_selection_bmap;
        }
        else
        {
            bestiary_pan_sel->bmap = bestiary_selection_bmap;
        }

        bestiary_pan_sel->size_x = bmap_width(bestiary_pan_sel->bmap);
        bestiary_pan_sel->size_y = bmap_height(bestiary_pan_sel->bmap);
    }
    else
    {
        reset(bestiary_pan_sel, SHOW);
    }

    // Pure input
    if(input_hit(INPUT_LEFT))
        bestiary.position -= 1;
    if(input_hit(INPUT_RIGHT))
        bestiary.position += 1;
    if(input_hit(INPUT_NAVBACK) || (input_hit(INPUT_ATTACK) && !mouse_left))
        bestiary.done = true;

    bestiary.position = clamp(bestiary.position, 0, BESTIARY_COUNT - 1);

    bestiary_pan_back->pos_x = 16;
    bestiary_pan_back->pos_y = screen_size.y - 16 - bmap_height(bestiary_pan_back->bmap);

    bestiary_pan_prev->pos_x = 16;
    bestiary_pan_prev->pos_y = (screen_size.y - bmap_height(bestiary_pan_prev->bmap)) / 2;

    bestiary_pan_next->pos_x = screen_size.x - 16 - bmap_width(bestiary_pan_next->bmap);
    bestiary_pan_next->pos_y = (screen_size.y - bmap_height(bestiary_pan_next->bmap)) / 2;

    if(bestiary.position > 0)
        set(bestiary_pan_prev, SHOW);
    else
        reset(bestiary_pan_prev, SHOW);

    if(bestiary.position < (BESTIARY_COUNT - 1))
        set(bestiary_pan_next, SHOW);
    else
        reset(bestiary_pan_next, SHOW);

    if(mouse_panel != bestiary.lastPan)
        snd_play(mainmenu_swap_snd, 100, 0);
    if(bestiary.done)
        snd_play(mainmenu_accept_snd, 100, 0);
    if(inital != bestiary.position)
        snd_play(mainmenu_accept_snd, 100, 0);

    bestiary.lastPan = mouse_panel;

    vec_lerp(camera->x, camera->x, vector(0, -800 * bestiary.position, 180), 0.1);

    draw_text(
        bestiary.beasts[bestiary.position].name,
        16,
        16,
        vector(34, 61, 208));

    draw_text(
        bestiary.beasts[bestiary.position].flavour,
        16,
        32,
        vector(200, 200, 200));

    var speed = 1;
    if(input_down(INPUT_JUMP))
        speed = 10;

    SUBSYSTEM_LOOP(you, SUBSYSTEM_BESTIARY)
    {
        you->pan += speed * time_step;
    }

    SUBSYSTEM_LOOP(you, SUBSYSTEM_BESTIARYTURRET)
    {
		ent_bonerotate(you, "Bone1", vector(speed * time_step, 0, 0));
    }

}

void bestiary_close()
{
    level_load(NULL);
    music_start("Media/intro.mp3", 1, 0);
    camera->arc = 60;
    reset(bestiary_pan_back, SHOW);
    reset(bestiary_pan_prev, SHOW);
    reset(bestiary_pan_next, SHOW);
    reset(bestiary_pan_sel, SHOW);
}

bool bestiary_is_done()
{
    return bestiary.done;
}

action BeastiaryEntry()
{
    // didn't to nothing
    framework_setup(me, SUBSYSTEM_BESTIARY);
}

action BeastiaryTEntry()
{
    // didn't to nothing
    framework_setup(me, SUBSYSTEM_BESTIARYTURRET);
}
