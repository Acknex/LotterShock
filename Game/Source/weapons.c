#include "weapons.h"
#include "input.h"
#include "math.h"
#include "dmgsys.h"

#define WEAPONS_COUNT 5

#define WEAPONS_LERP_SPEED 0.25

#define WEAPONS_CURRENT (weapons.weapon[weapons.current])

#define WEAPONS_SHOTGUN_RANGE  8000
#define WEAPONS_SHOTGUN_SPREAD 3 // *2 grad
#define WEAPONS_SHOTGUN_DAMAGE 5

#define WEAPONS_FLAME_VEL 100
#define WEAPONS_FLAME_SPREAD 13
#define WEAPONS_FLAME_COUNT 25
#define WEAPONS_FLAME_DAMPING 5.0

typedef struct weapons_data_t
{
    ENTITY * ent;
    SOUND  * snd;
    bool unlocked;
    bool autofire;
    bool streaming;
    var cooldown;
    var attackspeed;
    var ammo;
    var max_ammo;
} weapons_data_t;

typedef struct weapons_t
{
    int current;
    bool attacking;
    var attackprogress;
    int attackstate;
    int swordLength;
    var flamesound;
    var flamefade;
    weapons_data_t weapon[WEAPONS_COUNT];
} weapons_t;

weapons_t weapons;

ENTITY * weapons_wp_sword =
{
    type = "gipsy_sword.mdl";
    view = camera;
}

ENTITY * weapons_wp_shotgun =
{
    type = "triple_shotgun.mdl";
    view = camera;
}

ENTITY * weapons_wp_cellgun =
{
    type = "gipsy_sword.mdl";
    view = camera;
}

ENTITY * weapons_wp_flamethrower =
{
    type = "flamemother.mdl";
    view = camera;
}

BMAP * weapons_bullethole_decal = "bullet_hole.tga";

BMAP * weapons_fire_01 = "fire.pcx";

SOUND * weapons_snd_sword = "sword_snd.wav";
SOUND * weapons_snd_shotgun = "shotgun_snd.wav";
SOUND * weapons_snd_cellgun = "cellgun_snd.wav";
SOUND * weapons_snd_flamethrower = "flamethrowing.ogg";

VECTOR debugVec;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WEAPONS_SWORD_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x - 280, screen_size.y - 30, 100), camera)
#define WEAPONS_SWORD_DEFAULT_STANCE_ANG vector(-70, -20, 0)

#define WEAPONS_SWORD_BLOCK_STANCE_POS rel_for_screen(vector(screen_size.x - 80, screen_size.y - 20, 100), camera)
#define WEAPONS_SWORD_BLOCK_STANCE_ANG vector(0, 0, -60)

#define WEAPONS_SWORD_ATTACK_STANCE_POS rel_for_screen(vector(screen_size.x/3, screen_size.y + 10, 90), camera)
#define WEAPONS_SWORD_ATTACK_STANCE_ANG vector(00, -100, -30)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WEAPONS_SHOTGUN_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x - 200, screen_size.y - 20, 35), camera)
#define WEAPONS_SHOTGUN_DEFAULT_STANCE_ANG vector(0,0,0)

#define WEAPONS_SHOTGUN_SIGHT_STANCE_POS rel_for_screen(vector(screen_size.x/2, 2*screen_size.y/3 + 350, 15), camera)
#define WEAPONS_SHOTGUN_SIGHT_STANCE_ANG vector(0,0,0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WEAPONS_FLAME_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x, screen_size.y - 20, 400), camera)
#define WEAPONS_FLAME_DEFAULT_STANCE_ANG vector(0,0,0)

void weapons_add(int id)
{
    weapons.weapon[id].unlocked = true;
}

void weapons_erect_sword()
{
    weapons.swordLength += 1;
}

int weapons_get_max_ammo()
{
    if(weapons.current > 0)
        return (int)(WEAPONS_CURRENT.max_ammo);
    else
        return 0;
}

int weapons_get_ammo()
{
    if(weapons.current > 0)
        return (int)(WEAPONS_CURRENT.ammo);
    else
        return 0;
}

int weapons_get_current()
{
    return weapons.current;
}


void weapons_init()
{
    memset(&weapons, 0, sizeof(weapons_t));
    weapons.weapon[WEAPON_SWORD].ent = weapons_wp_sword;
    weapons.weapon[WEAPON_SHOTGUN].ent = weapons_wp_shotgun;
    weapons.weapon[WEAPON_CELLGUN].ent = weapons_wp_cellgun;
    weapons.weapon[WEAPON_FLAMETHROWER].ent = weapons_wp_flamethrower;

    weapons.weapon[WEAPON_SWORD].snd = weapons_snd_sword;
    weapons.weapon[WEAPON_SHOTGUN].snd = weapons_snd_shotgun;
    weapons.weapon[WEAPON_CELLGUN].snd = weapons_snd_cellgun;
    weapons.weapon[WEAPON_FLAMETHROWER].snd = weapons_snd_flamethrower;

    weapons.weapon[WEAPON_SWORD].attackspeed = 40;
    weapons.weapon[WEAPON_SHOTGUN].attackspeed = 10;
    weapons.weapon[WEAPON_FLAMETHROWER].streaming = true;

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

void weapons_shoot_shotgun()
{
    int i;
    for(i = 0; i < 10; i++)
    {
        VECTOR dir;
        vec_for_angle(dir, vector(camera.pan, camera.tilt, 0));
        vec_rotate(dir, vector(
            random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD,
            random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD,
            random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD));
        vec_normalize(dir, WEAPONS_SHOTGUN_RANGE);
        vec_add(dir, camera.x);

        dmgsys_set_src(DMGSYS_PLAYER, player, WEAPONS_SHOTGUN_DAMAGE);
        var dist = c_trace(camera.x, dir, IGNORE_PASSABLE | IGNORE_PASSENTS | USE_POLYGON | SCAN_TEXTURE | ACTIVATE_SHOOT);
        if(HIT_TARGET)
        {
            ent_decal(you, weapons_bullethole_decal, 2 + random(3) + 0.002 * dist, random(360));
        }
    }
}

void weapons_flame_effect_event(PARTICLE *p)
{
    if(p->skill_x == 0)
    {
        p->vel_x -= minv(abs(p->vel_x), WEAPONS_FLAME_DAMPING) * sign(p->vel_x) * time_step;
        p->vel_y -= minv(abs(p->vel_y), WEAPONS_FLAME_DAMPING) * sign(p->vel_y) * time_step;
        p->vel_z -= minv(abs(p->vel_z), WEAPONS_FLAME_DAMPING) * sign(p->vel_z) * time_step;
        p->vel_z -= 10 * time_step;

        VECTOR src, dest;
        vec_set(dest, p->vel_x);
        vec_normalize(dest, maxv(1, vec_length(dest)));

        vec_set(src, dest);
        vec_scale(src, -1);

        vec_add(src, p->x);
        vec_add(dest, p->x);

        if(c_trace(src, dest, USE_POLYGON))
        {
            vec_set(p->x, normal);
            vec_normalize(p->x,10.1);
            vec_add(p->x, target);
            p->skill_x = 1;
            p->flags &= ~STREAK;
        }
        else
        {
            VECTOR dist;
            vec_set(dist, p->vel_x);
            vec_scale(dist, time_step);
            vec_add(p->x, dist);
        }
    }

    if(p->skill_z <= 0)
    {
        dmgsys_set_src(DMGSYS_PLAYER, player, 1);
        c_scan(p->x, vector(0,0,0), vector(360, 360, p->size), ACTIVATE_SHOOT | IGNORE_PASSABLE | IGNORE_PASSENTS | SCAN_ENTS);
        p->skill_z = 1;
    }
    p->skill_z -= time_step;

    if(p->skill_y > 10)
        vec_set(p->blue, vector(64, 192 + random(64), 255 - random(32)));

    p->skill_y += 10 * time_step;

    p->size = clamp(p->size + 10 * time_step, 5, 50);
    p->alpha = 40 + random(10);

    if(p->lifespan < 25)
        p->alpha = 2.0 * p->lifespan;
}

void weapons_flame_effect(PARTICLE *p)
{
    p->bmap = weapons_fire_01;
    vec_rotate(p->vel_x, vector(
        random(2*WEAPONS_FLAME_SPREAD)-WEAPONS_FLAME_SPREAD,
        random(2*WEAPONS_FLAME_SPREAD)-WEAPONS_FLAME_SPREAD,
        random(2*WEAPONS_FLAME_SPREAD)-WEAPONS_FLAME_SPREAD ));
    p->flags = TRANSLUCENT | LIGHT | BRIGHT;
    vec_set(p->blue, vector(255, 192, 192));
    p->lifespan = 100;
    p->size = 5 + random(5);
    p->event = weapons_flame_effect_event;
}

void weapons_shoot_flamethrower()
{
    VECTOR pos;
    vec_set(pos, weapons_wp_flamethrower.x);
    vec_add(pos,  vector(370, 0, 30));

    vec_scale(pos, 0.1);

    vec_rotate(pos, camera.pan);
    vec_add(pos, camera.x);

    VECTOR dir;
    vec_for_angle(dir, camera.pan);

    vec_normalize(dir, WEAPONS_FLAME_VEL);

    effect (weapons_flame_effect, maxv(1, time_frame * WEAPONS_FLAME_COUNT), pos, dir);
}

void weapons_shoot_sword()
{
    VECTOR pos;
    vec_set(pos, weapons_wp_sword.x);
    vec_scale(pos, 0.1);

    vec_rotate(pos, camera.pan);
    vec_add(pos, camera.x);

    VECTOR end;
    vec_set(end, vector(0, 0, 25));
    vec_rotate(end, weapons_wp_sword.pan);
    vec_rotate(end, camera.pan);
    vec_add(end, pos);

    draw_line3d(pos, NULL, 100);
    draw_line3d(pos, COLOR_GREEN, 100);
    draw_line3d(end, COLOR_GREEN, 100);

    draw_point3d(end, COLOR_BLUE, 100, 5);

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

    ent_animate(weapons_wp_sword, "Erect", 10 * clamp(weapons.swordLength, 0, 10), ANM_SKIP);

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

        if(WEAPONS_CURRENT.streaming)
        {
            weapons.attacking = input_down(INPUT_ATTACK);
            if(!weapons.attacking)
            {
                weapons.attackprogress = 0;
                weapons.attackstate = 0;
            }
        }
        else
        {
            if(!weapons.attacking && input_hit(INPUT_ATTACK))
            {
                weapons.attacking = 1;
                weapons.attackprogress = 0;
                weapons.attackstate = 0;
            }
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

            if(weapons.attacking && weapons.attackstate++ == 0)
            {
                snd_play(WEAPONS_CURRENT.snd, 100, 0);
            }

            // if(weapons.attacking)
            {
                weapons_shoot_sword();
            }

            break;

        case WEAPON_SHOTGUN:
            if(input_down(INPUT_BLOCK))
            {
                vec_set(sourcePosePos, WEAPONS_SHOTGUN_SIGHT_STANCE_POS);
                vec_set(sourcePoseAng, WEAPONS_SHOTGUN_SIGHT_STANCE_ANG);
            }
            else
            {
                vec_set(sourcePosePos, WEAPONS_SHOTGUN_DEFAULT_STANCE_POS);
                vec_set(sourcePoseAng, WEAPONS_SHOTGUN_DEFAULT_STANCE_ANG);
            }
            vec_set(targetPosePos, sourcePosePos);
            vec_set(targetPoseAng, sourcePoseAng);

            if(weapons.attacking)
            {
                if(weapons.attackprogress >= 10 && weapons.attackstate == 0)
                {
                    var id = snd_play(WEAPONS_CURRENT.snd, 100, 0);
                    snd_tune(id, 0, 90 + random(20), 0);
                    weapons.attackstate = 1;
                    weapons_shoot_shotgun();
                }
                else if(weapons.attackprogress >= 30 && weapons.attackstate == 1)
                {
                    var id = snd_play(WEAPONS_CURRENT.snd, 100, 0);
                    snd_tune(id, 0, 90 + random(20), 0);
                    weapons.attackstate = 2;
                    weapons_shoot_shotgun();
                }
                else if(weapons.attackprogress >= 50 && weapons.attackstate == 2)
                {
                    var id = snd_play(WEAPONS_CURRENT.snd, 100, 0);
                    snd_tune(id, 0, 90 + random(20), 0);
                    weapons.attackstate = 3;
                    weapons_shoot_shotgun();
                }
            }

            break;
        case WEAPON_FLAMETHROWER:
            vec_set(sourcePosePos, WEAPONS_FLAME_DEFAULT_STANCE_POS);
            vec_set(sourcePoseAng, WEAPONS_FLAME_DEFAULT_STANCE_ANG);

            vec_set(targetPosePos, sourcePosePos);
            vec_set(targetPoseAng, sourcePoseAng);

            if(weapons.attacking)
            {
                if(!weapons.flamesound)
                    weapons.flamesound = snd_loop(weapons_snd_flamethrower, 0, 0);
                snd_tune(weapons.flamesound, weapons.flamefade, 0, 0);

                weapons.flamefade = clamp(weapons.flamefade + 20 * time_step, 0, 100);

                weapons_shoot_flamethrower();
            }
            else
            {
                if(weapons.flamesound)
                {
                    weapons.flamefade = clamp(weapons.flamefade - 20 * time_step, 0, 100);

                    snd_tune(weapons.flamesound, weapons.flamefade, 0, 0);

                    if(weapons.flamefade == 0)
                    {
                        snd_stop(weapons.flamesound);
                        weapons.flamesound = 0;
                    }
                }
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
