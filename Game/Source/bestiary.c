#include "bestiary.h"
#include "input.h"
#include "music_player.h"
#include "ui.h"
#include "global.h"
#include "framework.h"
#include "music_player.h"

#include "settings.h"

#include <acknex.h>

#define BESTIARY_ROTSPEED  skill1
#define BESTIARY_INDEX     skill2
#define BESTIARY_ISTURRET  skill3
#define BESTIARY_ANIMSPEED skill4
#define BESTIARY_ANIMATION skill21
#define BESTIARY_INITPOS   skill22
// #define BESTIARY_INITPOS   skill23
// #define BESTIARY_INITPOS   skill24
#define BESTIARY_BEAST_UNLOCKED  skill25
#define BESTIARY_INITSCALE   skill26
// #define BESTIARY_INITSCALE   skill27
// #define BESTIARY_INITSCALE   skill28
#define BESTIARY_SPRITEPTR  skill29

typedef struct beast_t
{
    char const * name;
    char const * flavour;
    char const * mediaFile;
} beast_t;

BMAP * bestiary_bmp_next = "best_next.png";
BMAP * bestiary_bmp_prev = "best_prev.png";
BMAP * bestiary_bmp_back = "best_back.png";
BMAP * bestiary_bmp_panl = "best_panel.png";

BMAP * bestiary_selection_bmap = "best_sel.png";

PANEL * bestiary_pan_back =
{
    bmap = bestiary_bmp_back;
    layer = 2;
}

PANEL * bestiary_pan_next =
{
    bmap = bestiary_bmp_next;
        layer = 2;
}

PANEL * bestiary_pan_prev =
{
    bmap = bestiary_bmp_prev;
        layer = 2;
}

PANEL * bestiary_pan_sel =
{
    bmap = mainmenu_selection_bmap;
    layer = 3;
    flags = UNTOUCHABLE;
}

PANEL * bestiary_pan_info =
{
    bmap = bestiary_bmp_panl;
    layer = 2;
    flags = UNTOUCHABLE;
}

STRING * bestiary_beast_name = "#1024";
STRING * bestiary_beast_desc = "#1024";
STRING * bestiary_beast_media = "#1024";

FONT * bestiary_fnt_name = "Arial#32b";
FONT * bestiary_fnt_desc = "Arial#24";
FONT * bestiary_fnt_help = "Arial#16ib";

TEXT * bestiary_txt_name =
{
    layer = 3;
    string = (bestiary_beast_name);
    font = bestiary_fnt_name;
    red = 208;
    green = 61;
    blue = 34;
}

TEXT * bestiary_txt_desc =
{
    layer = 3;
    string = (bestiary_beast_desc);
    font = bestiary_fnt_desc;
    red = 200;
    green = 200;
    blue = 200;
    flags = WWRAP;
}

TEXT * bestiary_txt_help =
{
    layer = 3;
    string = ("Kill this enemy to obtain more information!");
    font = bestiary_fnt_help;
    red = 208;
    green = 61;
    blue = 34;
}

struct
{
    int position;
    bool done;
    PANEL * lastPan;
    beast_t beasts[BEAST_COUNT];
    var mediaHandle;
    var mediaTimer;
} bestiary;

void bestiary_init()
{
    bestiary.beasts[0].name    = "J.C. 'Star Lotti' Lotter";
    bestiary.beasts[0].flavour = "Lone ranger, protector of the Earth.\nLast line of defense against the Unity Union.";
    bestiary.beasts[0].mediaFile = "Media/beastiary_lotti.mp3";

    bestiary.beasts[1].name    = "Acktana";
    bestiary.beasts[1].flavour = "A powerful and unique AI, created in the year 2018.\nBased on a Horstmann-Pohl personality construct.\nMay be corrupted and/or eccentric.";
    bestiary.beasts[1].mediaFile = "Media/beastiary_acktana.mp3";

    bestiary.beasts[2].name    = "Donkey-Lark";
    bestiary.beasts[2].flavour = "Originally a peaceful herbivore, this gentile creature\nwas transformed by excessive DNA modification.\nNitroglycerine makes up over 80 percent of its biomass.\nExplodes violently when agitated.";
    bestiary.beasts[2].mediaFile = "Media/beastiary_donkeylark.mp3";

    bestiary.beasts[3].name    = "Sputnik";
    bestiary.beasts[3].flavour = "Originally a haul-droid, now a dangerous military-grade combat machine.\nEquipped with sword-arms by Dr. Horstmann for \"additional coolness\"";
    bestiary.beasts[3].mediaFile = "Media/beastiary_sputnik.mp3";

    bestiary.beasts[4].name    = "Whiskas Skull";
    bestiary.beasts[4].flavour = "Originally intended to clean the ship by eating waste. With laser eyes. Because, why not.";
    bestiary.beasts[4].mediaFile = "Media/beastiary_whiskas.mp3";

    bestiary.beasts[5].name    = "Eye of the Acknex";
    bestiary.beasts[5].flavour = "Remains of the Giant Titans.\nEquipped with brains they can be very dangerous.";
    bestiary.beasts[5].mediaFile = "Media/beastiary_eye.mp3";

    bestiary.beasts[6].name    = "Turret";
    bestiary.beasts[6].flavour = "Your very personal bullet hell in a box in a wall in a spaceship.";
    bestiary.beasts[6].mediaFile = "Media/beastiary_turret.mp3";
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

    bestiary.mediaHandle = 0;
    bestiary.mediaTimer = 0;

    music_start("Media/beastiary.mp3", 0.5, true);

    set(bestiary_pan_back, SHOW);
    set(bestiary_pan_prev, SHOW);
    set(bestiary_pan_next, SHOW);

    set(bestiary_pan_info, SHOW);
    set(bestiary_txt_name, SHOW);
    set(bestiary_txt_desc, SHOW);
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

    bestiary.position = clamp(bestiary.position, 0, BEAST_COUNT - 1);

    bestiary_pan_back->pos_x = 16;
    bestiary_pan_back->pos_y = screen_size.y - 16 - bmap_height(bestiary_pan_back->bmap);

    bestiary_pan_prev->pos_x = 16;
    bestiary_pan_prev->pos_y = (screen_size.y - bmap_height(bestiary_pan_prev->bmap)) / 2;

    bestiary_pan_next->pos_x = screen_size.x - 16 - bmap_width(bestiary_pan_next->bmap);
    bestiary_pan_next->pos_y = (screen_size.y - bmap_height(bestiary_pan_next->bmap)) / 2;

    bestiary_pan_info->pos_x = screen_size.x - bmap_width(bestiary_pan_info->bmap);
    bestiary_pan_info->pos_y = screen_size.y - bmap_height(bestiary_pan_info->bmap);

    bestiary_txt_name->pos_x = bestiary_pan_info->pos_x + 10;
    bestiary_txt_name->pos_y = bestiary_pan_info->pos_y +  6;

    bestiary_txt_desc->pos_x = bestiary_pan_info->pos_x + 22;
    bestiary_txt_desc->pos_y = bestiary_pan_info->pos_y + 50;
    bestiary_txt_desc->size_x = bmap_width(bestiary_pan_info->bmap) - 44;

    bestiary_txt_help->pos_x = bestiary_pan_info->pos_x + 6;
    bestiary_txt_help->pos_y = bestiary_pan_info->pos_y + bmap_height(bestiary_pan_info->bmap) - 20;

    if(bestiary.position > 0)
        set(bestiary_pan_prev, SHOW);
    else
        reset(bestiary_pan_prev, SHOW);

    if(bestiary.position < (BEAST_COUNT - 1))
        set(bestiary_pan_next, SHOW);
    else
        reset(bestiary_pan_next, SHOW);

    if(mouse_panel != bestiary.lastPan)
        snd_play(ui_swap_snd, 100, 0);
    if(bestiary.done)
        snd_play(ui_accept_snd, 100, 0);

    if(inital != bestiary.position)
    {
        if(media_playing(bestiary.mediaHandle))
            media_stop(bestiary.mediaHandle);
        bestiary.mediaHandle = 0;
        bestiary.mediaTimer = 0;

        snd_play(ui_accept_snd, 100, 0);
    }

    bestiary.lastPan = mouse_panel;

#ifdef DEBUG
    if(def_camera == 0)
#endif
    {
        vec_lerp(camera->x, camera->x, vector(0, -800 * bestiary.position, 180), 0.1);
    }

    str_cpy(bestiary_beast_name, bestiary.beasts[bestiary.position].name);
    str_cpy(bestiary_beast_desc, bestiary.beasts[bestiary.position].flavour);

    if(achievements.bestiary_unlocked[bestiary.position])
    {
        bestiary.mediaTimer += time_step;
        if(bestiary.mediaHandle == 0 && bestiary.mediaTimer > 32) // 2 sekunden
        {
            bestiary.mediaHandle = media_play(bestiary.beasts[bestiary.position].mediaFile, NULL, 100);
        }

        reset(bestiary_txt_help, SHOW);
    }
    else
    {
        if(bestiary.mediaHandle != 0)
        {
            media_stop(bestiary.mediaHandle);
            bestiary.mediaHandle = 0;
        }

        set(bestiary_txt_help, SHOW);

        int i;
        char * mixup = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvxyz012345689!%&/?=-#";
        int mixup_len = strlen(mixup);
        srand((total_frames / 16) % 10 + 1); // fixed seed
        for(i = 0; i < str_len(bestiary_beast_name); i++)
        {
            if((bestiary_beast_name->chars)[i] != ' ')
                (bestiary_beast_name->chars)[i] = mixup[rand() % mixup_len];
        }
        for(i = 0; i < str_len(bestiary_beast_desc); i++)
        {
            if((bestiary_beast_desc->chars)[i] != ' ')
                (bestiary_beast_desc->chars)[i] = mixup[rand() % mixup_len];
        }
    }

    /*
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
    */

    var speed = 1;
    if(input_down(INPUT_JUMP))
        speed = 10;

    SUBSYSTEM_LOOP(you, SUBSYSTEM_BESTIARY)
    {
        ENTITY * spr = you->BESTIARY_SPRITEPTR;
        if(achievements.bestiary_unlocked[you->BESTIARY_INDEX] != you->BESTIARY_BEAST_UNLOCKED)
        {
            if(achievements.bestiary_unlocked[you->BESTIARY_INDEX])
            {
                set(spr, INVISIBLE);
                reset(you, INVISIBLE);
            }
            else
            {
                reset(spr, INVISIBLE);
                set(you, INVISIBLE);
            }

            you->BESTIARY_BEAST_UNLOCKED = achievements.bestiary_unlocked[you->BESTIARY_INDEX];
        }

        // Standardverhalten
        you->pan += speed * you->BESTIARY_ROTSPEED * time_step;

        you->BESTIARY_ANIMATION += you->BESTIARY_ANIMSPEED * time_step;

        if(you->string1 != NULL)
        {
            if(strlen(you->string1) > 0)
            {
                ent_animate(you, you->string1, you->BESTIARY_ANIMATION, ANM_CYCLE);
            }
        }

        if(you->BESTIARY_ISTURRET)
            ent_bonerotate(you, "Bone1", vector(speed * time_step, 0, 0));
    }
}

void bestiary_close()
{
    SUBSYSTEM_LOOP(you, SUBSYSTEM_BESTIARY)
    {
        if(you->string2 != NULL)
            free(you->string2);
    }
    level_load(NULL);

    if(bestiary.mediaHandle != 0)
    {
        media_stop(bestiary.mediaHandle);
        bestiary.mediaHandle = 0;
    }

    reset(bestiary_pan_back, SHOW);
    reset(bestiary_pan_prev, SHOW);
    reset(bestiary_pan_next, SHOW);
    reset(bestiary_pan_sel, SHOW);
    reset(bestiary_pan_info, SHOW);
    reset(bestiary_txt_name, SHOW);
    reset(bestiary_txt_desc, SHOW);
    reset(bestiary_txt_help, SHOW);
}

bool bestiary_is_done()
{
    return bestiary.done;
}

// skill1: Rotspeed 1.0
// skill2: Bestiary-ID 0
// skill3: IsTurret 0
// skill4: Animspeed 1.0
// string1: Animation "stand"
action BeastiaryEntry()
{
    // didn't to nothing
    framework_setup(me, SUBSYSTEM_BESTIARY);

    // Force update of beast state in next update
    my->BESTIARY_BEAST_UNLOCKED = !achievements.bestiary_unlocked[my->BESTIARY_INDEX];

    vec_set(my->BESTIARY_INITPOS, my->x);
    vec_set(my->BESTIARY_INITSCALE, my->scale_x);

    my->string2 = malloc(strlen(my->type) + 1);
    strcpy(my->string2, my->type);

    if(my->BESTIARY_ROTSPEED == 0)
        my->BESTIARY_ROTSPEED = 1;

    if(my->BESTIARY_ANIMSPEED == 0)
        my->BESTIARY_ANIMSPEED = my->BESTIARY_ROTSPEED;

    you = ent_create("best_noise.tga", my->x, NULL);
    you->y = -800 * my->BESTIARY_INDEX; // hard align sprite
    you->z = 200;
    you->material = matNoiseSprite;
    vec_fill(you->scale_x, 0.561);
    my->BESTIARY_SPRITEPTR = you;
}
