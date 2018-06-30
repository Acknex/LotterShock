#include "weapons.h"
#include "input.h"
#include "math.h"

#define WEAPONS_COUNT 5

#define WEAPONS_LERP_SPEED 0.2

#define WEAPONS_CURRENT (weapons.weapon[weapons.current])

typedef struct weapons_data_t
{
    ENTITY * ent;
    bool unlocked;
    var cooldown;
    var attackspeed;
} weapons_data_t;

typedef struct weapons_t
{
    int current;
    bool attacking;
    var attackprogress;
    int swordLength;
    weapons_data_t weapon[WEAPONS_COUNT];
} weapons_t;

weapons_t weapons;

ENTITY * weapons_wp_sword =
{
    type = "gipsy_sword.mdl";
    view = camera;
    x = 100;
    y = -25;
    z = -25;
}

ENTITY * weapons_wp_shotgun =
{
    type = "gipsy_sword.mdl";
    view = camera;
}

ENTITY * weapons_wp_cellgun =
{
    type = "gipsy_sword.mdl";
    view = camera;
}

ENTITY * weapons_wp_flamethrower =
{
    type = "gipsy_sword.mdl";
    view = camera;
}

VECTOR debugVec;

#define WEAPONS_SWORD_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x - 280, screen_size.y - 30, 100), camera)
#define WEAPONS_SWORD_DEFAULT_STANCE_ANG vector(-70, -20, 0)

#define WEAPONS_SWORD_BLOCK_STANCE_POS rel_for_screen(vector(screen_size.x - 80, screen_size.y - 20, 100), camera)
#define WEAPONS_SWORD_BLOCK_STANCE_ANG vector(0, 0, -60)

#define WEAPONS_SWORD_ATTACK_STANCE_POS rel_for_screen(vector(screen_size.x/3, screen_size.y + 10, 90), camera)
#define WEAPONS_SWORD_ATTACK_STANCE_ANG vector(00, -100, -30)

void weapons_add(int id)
{
    weapons.weapon[id].unlocked = true;
}

void weapons_erect_sword()
{
    weapons.swordLength += 1;
}

void weapons_init()
{
    memset(&weapons, 0, sizeof(weapons_t));
    weapons.weapon[WEAPON_SWORD].ent = weapons_wp_sword;
    weapons.weapon[WEAPON_SHOTGUN].ent = weapons_wp_shotgun;
    weapons.weapon[WEAPON_CELLGUN].ent = weapons_wp_cellgun;
    weapons.weapon[WEAPON_FLAMETHROWER].ent = weapons_wp_flamethrower;

    weapons.weapon[WEAPON_SWORD].attackspeed = 30;

    on_o = weapons_erect_sword;
}

void weapons_open()
{
    weapons.current = 0;
}

//! search next unlocked weapon in the given direction.
//! if none found, keep the current one
void weapons_select_next(int dir)
{
    int start = weapons.current;
    do
    {
        weapons.current = cycle(weapons.current + dir, 0, WEAPONS_COUNT);
        if(weapons.weapon[weapons.current].unlocked)
            return;
    } while(start != weapons.current);
}

void weapons_update()
{
    int i;
    if(key_1) weapons_add(WEAPON_SWORD);
    if(key_2) weapons_add(WEAPON_SHOTGUN);
    if(key_3) weapons_add(WEAPON_CELLGUN);
    if(key_4) weapons_add(WEAPON_FLAMETHROWER);

    if(!weapons.attacking && input_hit(INPUT_WEAPON_UP))
        weapons_select_next(1);
    if(!weapons.attacking && input_hit(INPUT_WEAPON_DOWN))
        weapons_select_next(-1);

    DEBUG_VAR(weapons.attacking, 16);

    ent_animate(weapons_wp_sword, "Erect", 10 * clamp(weapons.swordLength, 0, 10), ANM_SKIP);

    if(weapons.current > 0)
    {
        draw_text(
            str_printf(
                NULL,
                "x=%f y=%f z=%f  pan=%f tilt=%f roll=%f",
                (double)WEAPONS_CURRENT.ent.x,
                (double)WEAPONS_CURRENT.ent.y,
                (double)WEAPONS_CURRENT.ent.z,
                (double)WEAPONS_CURRENT.ent.pan,
                (double)WEAPONS_CURRENT.ent.tilt,
                (double)WEAPONS_CURRENT.ent.roll),
            64, 16,
            COLOR_RED);
    }

    // ignore dummy weapon for visuals
    for(i = 1; i < WEAPONS_COUNT; i++)
    {
        if(i == weapons.current)
            weapons.weapon[i].ent.flags2 |= SHOW;
        else
            weapons.weapon[i].ent.flags2 &= ~SHOW;
    }

    if(weapons.current > 0)
    {
        VECTOR targetPosePos;
        ANGLE targetPoseAng;

        VECTOR sourcePosePos;
        ANGLE sourcePoseAng;

        if(!weapons.attacking && input_hit(INPUT_ATTACK))
        {
            weapons.attacking = 1;
            weapons.attackprogress = 0;
        }

        switch(weapons.current)
        {
        case WEAPON_SWORD:
            vec_set(sourcePosePos, WEAPONS_SWORD_DEFAULT_STANCE_POS);
            vec_set(sourcePoseAng, WEAPONS_SWORD_DEFAULT_STANCE_ANG);

            if(weapons.attacking)
            {
                vec_set(targetPosePos, WEAPONS_SWORD_ATTACK_STANCE_POS);
                vec_set(targetPoseAng, WEAPONS_SWORD_ATTACK_STANCE_ANG);
            }
            else if(input_down(INPUT_BLOCK))
            {
                vec_set(targetPosePos, WEAPONS_SWORD_BLOCK_STANCE_POS);
                vec_set(targetPoseAng, WEAPONS_SWORD_BLOCK_STANCE_ANG);
            }
            else
            {
                vec_set(targetPosePos, sourcePosePos);
                vec_set(targetPoseAng, sourcePoseAng);
            }
            break;
        }

        if(weapons.attacking)
        {
            vec_lerp(WEAPONS_CURRENT.ent.x, sourcePosePos, targetPosePos, 0.01 * weapons.attackprogress);
            ang_lerp(WEAPONS_CURRENT.ent.pan, sourcePoseAng, targetPoseAng, 0.01 * weapons.attackprogress);
        }
        else
        {
            vec_lerp(WEAPONS_CURRENT.ent.x, WEAPONS_CURRENT.ent.x, targetPosePos, WEAPONS_LERP_SPEED);
            ang_lerp(WEAPONS_CURRENT.ent.pan, WEAPONS_CURRENT.ent.pan, targetPoseAng, WEAPONS_LERP_SPEED);
        }

        weapons.attackprogress += WEAPONS_CURRENT.attackspeed * time_step;
        if(weapons.attackprogress >= 100)
        {
            weapons.attackprogress = 0;
            weapons.attacking = 0;
        }
    }
}

void weapons_close()
{
    int i;
    for(i = 1; i < WEAPONS_COUNT; i++)
    {
        weapons.weapon[i].ent.flags2 &= ~SHOW;
    }
    on_o = NULL;
}
