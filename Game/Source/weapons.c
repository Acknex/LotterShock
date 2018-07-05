#include "global.h"
#include "weapons.h"
#include "input.h"
#include "math.h"
#include "dmgsys.h"
#include "movement.h"
#include "projectiles.h"
#include "environmentals.h"

#define WEAPONS_COUNT 5

#define WEAPONS_LERP_SPEED 0.35

#define WEAPONS_CURRENT (weapons.weapon[weapons.current])

#define WEAPONS_SHOTGUN_RANGE  8000
#define WEAPONS_SHOTGUN_SPREAD 6 // *2 grad
#define WEAPONS_SHOTGUN_DAMAGE 10

#define WEAPONS_FLAME_VEL 100
#define WEAPONS_FLAME_SPREAD 13
#define WEAPONS_FLAME_COUNT 25
#define WEAPONS_FLAME_DAMPING 2.0
#define WEAPONS_FLAME_GRAVITY 5.0

#define WEAPONS_FLAME_MINSIZE 5
#define WEAPONS_FLAME_MAXSIZE 75

#define WEAPONS_CELLGUN_DAMAGE 7

#define WEAPONS_DOUBLE_FLAME_EFFECT

typedef struct weapons_data_t
{
	ENTITY * ent;
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
	var spearpower;
	var speartimer;
	var electro;
	weapons_data_t weapon[WEAPONS_COUNT];
	VECTOR flamedir;
} weapons_t;

weapons_t weapons;

ENTITY * weapons_wp_sword =
{
	type = "gipsy_sword.mdl";
	material = matWeaponBasic;
	view = camera;
}

ENTITY * weapons_wp_shotgun =
{
	type = "triple_shotgun.mdl";
	material = matWeaponBasic;
	view = camera;
}

ENTITY * weapons_wp_cellgun =
{
	type = "trident_of_lightning.mdl";
	material = matWeaponBasic;
	view = camera;
}

ENTITY * weapons_wp_flamethrower =
{
	type = "flamemother.mdl";
	material = matWeaponBasic;
	view = camera;
}

BMAP * weapons_bullethole_decal = "bullet_hole.tga";

BMAP * weapons_fire_01 = "fire.pcx";
BMAP * weapons_fire_02 = "fire.pcx";

SOUND * weapons_snd_sword1 = "sword_swing1.wav";
SOUND * weapons_snd_sword2 = "sword_swing2.wav";
SOUND * weapons_snd_sword3 = "sword_swing3.wav";
SOUND * weapons_snd_shotgun = "shotgun_snd.wav";
SOUND * weapons_snd_cellgun = "cellgun_snd.wav";
SOUND * weapons_snd_cellgun_loop = "cellgun_hum.wav";
SOUND * weapons_snd_flamethrower = "flamethrower_snd.wav";
SOUND * weapons_snd_flamethrower_start = "flamethrower_start_snd.wav";
SOUND * weapons_snd_flamethrower_end = "flamethrower_end_snd.wav";


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

#define WEAPONS_SHOTGUN_SIGHT_STANCE_POS rel_for_screen(vector(screen_size.x/2 +150, 2*screen_size.y/3 + 350, 15), camera)
#define WEAPONS_SHOTGUN_SIGHT_STANCE_ANG vector(0,0,0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WEAPONS_FLAME_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x, screen_size.y - 20, 400), camera)
#define WEAPONS_FLAME_DEFAULT_STANCE_ANG vector(0,0,0)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define WEAPONS_CELLGUN_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x + 20, screen_size.y, 40), camera)
#define WEAPONS_CELLGUN_DEFAULT_STANCE_ANG vector(0,0,160)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void weapons_add(int id)
{
    int i, len = 0;
    for(i = 1; i < WEAPONS_COUNT; i++)
    {
        if(weapons.weapon[i].unlocked)
            len += 1;
    }
	weapons.weapon[id].unlocked = true;
    if(len == 0)
        weapons.current = id;
}

void weapons_erect_sword()
{
	weapons.swordLength += 1;
}

int weapons_get_max_ammo(int id)
{
	if(id > 0 && id < WEAPONS_COUNT)
	return (int)(weapons.weapon[id].max_ammo);
	else
	return 0;
}
int weapons_get_max_ammo()
{
	if(weapons.current > 0)
	return weapons_get_max_ammo(weapons.current);
	else
	return 0;
}

int weapons_get_ammo(int id)
{
	if(id > 0 && id < WEAPONS_COUNT)
	return (int)(weapons.weapon[id].ammo);
	else
	return 0;
}
int weapons_get_ammo()
{
	if(weapons.current > 0)
	return weapons_get_ammo(weapons.current);
	else
	return 0;
}
void weapons_add_ammo(int weaponType, var amount)
{
	weapons.weapon[weaponType].ammo = clamp(weapons.weapon[weaponType].ammo + amount, 0, weapons_get_max_ammo(weaponType));
}

int weapons_get_current()
{
	return weapons.current;
}

BMAP* skyCute_cubemap = "skyCute+6.tga";

MATERIAL* trident_sphere_mat =
{
	effect = "tridentSphere.fx";
	flags = AUTORELOAD;
}
VECTOR vRotatedDirection;
float rotateMatrix[16];

void weapons_init()
{
	on_o = weapons_erect_sword;

	memset(&weapons, 0, sizeof(weapons_t));
	weapons.weapon[WEAPON_SWORD].ent = weapons_wp_sword;
	weapons.weapon[WEAPON_SHOTGUN].ent = weapons_wp_shotgun;
	weapons.weapon[WEAPON_CELLGUN].ent = weapons_wp_cellgun;
	weapons.weapon[WEAPON_FLAMETHROWER].ent = weapons_wp_flamethrower;

	weapons.weapon[WEAPON_SWORD].attackspeed = 40;
	weapons.weapon[WEAPON_SHOTGUN].attackspeed = 10;
	weapons.weapon[WEAPON_FLAMETHROWER].streaming = true;
	weapons.weapon[WEAPON_CELLGUN].streaming = true;
	weapons.weapon[WEAPON_CELLGUN].attackspeed = 20.0;

	weapons.weapon[WEAPON_SWORD].max_ammo        = 0;
	weapons.weapon[WEAPON_SHOTGUN].max_ammo      = 36;
	weapons.weapon[WEAPON_CELLGUN].max_ammo      = 150;
	weapons.weapon[WEAPON_FLAMETHROWER].max_ammo = 300;


	//weapons_wp_cellgun.material = mtl_model;
	//ent_mtlset(weapons_wp_cellgun,trident_sphere_mat,2);
	weapons_wp_cellgun.material = trident_sphere_mat;
 	ent_mtlset(weapons_wp_cellgun,mtl_model,1);
	bmap_to_cubemap(skyCute_cubemap); 
	trident_sphere_mat.skin1 = skyCute_cubemap;

	on_o = weapons_erect_sword;
}

void weapons_open()
{
	weapons.current = 0;
	
	int id;
	for(id = 1; id <= WEAPONS_COUNT; id++)
	{
		weapons.weapon[id].unlocked = false;
		weapons.weapon[id].ammo = weapons.weapon[id].max_ammo;
	}
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

bool weapons_draw_ammo(var amount)
{
	if(WEAPONS_CURRENT.max_ammo <= 0)
	return true;
	if(WEAPONS_CURRENT.ammo < amount)
	return false;
	WEAPONS_CURRENT.ammo -= amount;
	return true;
}


var weaponShotgunSmokeCounter = 0;
var weaponShotgunSmokeCounter2 = 0;

void p_shotgun_muzzle_smoke_fade(PARTICLE* p)
{
	p.vel_x *= 1-0.35*time_step;
	p.vel_y *= 1-0.35*time_step;
	p.vel_z *= 1-0.35*time_step;
	p.alpha -= 0.125*time_step;
	p.size += 3*time_step;
	if(p.alpha <= 0) p.lifespan = 0;
}

void p_shotgun_muzzle_smoke(PARTICLE* p)
{
	p.bmap = smokeSprite1_bmp;
	set(p,MOVE);
	vec_scale(p.vel_x,0.25);
	//p.gravity = -0.2;
	p.alpha = 4+random(1);
	vec_fill(p.blue,110+random(10));
	p.size = 5+random(2);
	p.event = p_shotgun_muzzle_smoke_fade;
}


VECTOR shotgun_muzzle_pos;
void p_shotgun_muzzle_fire_fade(PARTICLE* p)
{
	p.size = p.skill_a*p.lifespan;
	vec_set(p.x,shotgun_muzzle_pos);
}

void p_shotgun_muzzle_fire(PARTICLE* p)
{
	set(p,BRIGHT);
	//set(p,BRIGHT);
	p.alpha = 10+random(10);
	vec_set(p.blue,vector(20,190,255));
	p.skill_a = p.size = 2*(48+random(32));
	p.lifespan = 1;
	p.event = p_shotgun_muzzle_fire_fade;
}


void weapons_shoot_shotgun()
{
	int i;
	playerSetLight(vector(50,190,255), 3000, 2);
	for(i = 0; i < 10; i++)
	{
		VECTOR dir;
		vec_for_angle(dir, vector(camera.pan, camera.tilt, 0));
		vec_rotate(dir, vector(
		random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD,
		random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD,
		random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD));
		vec_normalize(dir, WEAPONS_SHOTGUN_RANGE);

		
		VECTOR pos,speed;
		vec_set(speed,dir);
		vec_normalize(speed,800);
		vec_set(pos,vector(camera.x,camera.y,camera.z-2));
		
		projectileCreate(PROJECTILE_TYPE_SHOTGUN, 1, pos, speed);
		
		vec_add(dir, camera.x);
		dmgsys_set_src(DMGSYS_PLAYER, player, WEAPONS_SHOTGUN_DAMAGE);
		var dist = c_trace(camera.x, dir, IGNORE_PASSABLE | IGNORE_PASSENTS | USE_POLYGON | SCAN_TEXTURE | ACTIVATE_SHOOT);
		/*if(HIT_TARGET)
		{
			PARTICLE *p = ent_decal(you, weapons_bullethole_decal, 2 + random(3) + 0.002 * dist, random(360));
			p->material = matDecalBasic;
			p->lifespan = 640;
		}*/
	}
	weaponShotgunSmokeCounter = 24;
}

void weapons_secondary_flame_effect_event(PARTICLE *p)
{
	p->skill_a += time_step;
	
	if(p->skill_a < 30)
		p->alpha += time_step;
	else
	{
		if (p->skill_a > 60)
			p->alpha -= 0.5*time_step;
		p->alpha -= 0.3*time_step;
	}
	
	var r = 10;
	var o = r/2;
	/*
	vec_set(p->x, p->skill_x);
	
	vec_add(p->x, vector(random(r)-o,random(r)-o,random(r)-o));
	*/
	
	VECTOR tmp;
	vec_set(&tmp, p->skill_x);
	vec_add(&tmp, vector(random(r)-o,random(r)-o,random(r)-o));
	vec_lerp(p->skill_x, p->x, &tmp, 0.1);
	vec_set(p->x, p->skill_x);
	
	
	if(p->alpha <= 0)
		p->lifespan = 0;
}

void weapons_secondary_flame_effect(PARTICLE *p)
{
	vec_set(p->skill_x, p->x);
	
	p->bmap = weapons_fire_02;
	p->flags = LIGHT|TRANSLUCENT|BRIGHT;
	p->alpha = 0;
	vec_set(p->blue, vector(32, 70, 255));
	p->lifespan = 100;
	p->size = 35 + random(35);
	p->event = weapons_secondary_flame_effect_event;
}

void weapons_flame_effect_event(PARTICLE *p)
{
	if(p->skill_x == 0)
	{
		p->skill_a -= minv(abs(p->skill_a), p->skill[3]) * sign(p->skill_a) * time_step;
		p->skill_b -= minv(abs(p->skill_b), p->skill[3]) * sign(p->skill_b) * time_step;
		p->skill_c -= minv(abs(p->skill_c), p->skill[3]) * sign(p->skill_c) * time_step;
		p->skill_c -= WEAPONS_FLAME_GRAVITY * time_step;

		VECTOR src, dest;
		vec_set(dest, p->skill_a);
		vec_normalize(dest, maxv(1, vec_length(dest)));

		vec_set(src, dest);
		vec_scale(src, -1);

		vec_add(src, p->x);
		vec_add(dest, p->x);

        you = player;
        if(c_trace(src, dest, IGNORE_YOU | IGNORE_PASSABLE | USE_POLYGON | IGNORE_PASSENTS))
		{
			vec_set(p->x, normal);
			vec_normalize(p->x,7.1 +random(5));
			vec_add(p->x, target);
			p->skill_x = 1;
			p->flags &= ~STREAK;
			
			
			#ifdef WEAPONS_DOUBLE_FLAME_EFFECT     
				VECTOR secondary_position;
				vec_set(&secondary_position, &normal);
				vec_normalize(&secondary_position,10 + random(10));
				vec_add(&secondary_position, &p->x);      
				effect (weapons_secondary_flame_effect, 1, &secondary_position, nullvector);
			#endif
			// p->lifespan = 0;
			
		}
		else
		{
			VECTOR dist;
			vec_set(dist, p->skill_a);
			vec_scale(dist, time_step);
			vec_add(p->x, dist);
		}
		#ifdef WEAPONS_DOUBLE_FLAME_EFFECT     
			p->size = clamp(p->size + 8 * time_step, 5, 40);
		#endif
	}
	else
	{
		//p->size = clamp(p->size - time_step, 35, 100);
	}

    if(p->skill_z <= 0)
	{
        // no entity -> no pushback
        dmgsys_set_src(DMGSYS_PLAYER, NULL, 0.5);
		// c_scan(p->x, vector(0,0,0), vector(360, 360, p->size), IGNORE_PASSABLE | IGNORE_PASSENTS | SCAN_ENTS);
		ENTITY * it;
		for(it = ent_next(NULL); it != NULL; it = ent_next(it))
        {
            // NEVER FORGET 100ms!
            // don't check non-subsystem entities, they aren't relevant for us!
            if(it->SK_SUBSYSTEM == 0)
                continue;

			VECTOR tmp;
			vec_diff(tmp, p->x, it->x);
			vec_rotateback(tmp, it->pan);

            if((tmp.x + p->size) < it->min_x || (tmp.y + p->size) < it->min_y || (tmp.z + p->size) < it->min_z)
                continue;
            if((tmp.x - p->size) > it->max_x || (tmp.y - p->size) > it->max_y || (tmp.z - p->size) > it->max_z)
                continue;

            if(it->SK_SUBSYSTEM == SUBSYSTEM_ENVIRONMENT && it->ENVIRONMENTALS_TYPE == ENVIRONMENTAL_ICE)
            {
                it->ENVIROMENTALS_ICE_DAMAGE -= 0.01;
            }

            if(it->SK_SUBSYSTEM >= 1000)
            {
                if((it->emask & ENABLE_SHOOT) && (it->event))
                {
                    my = it;
                    event_type = EVENT_SHOOT;
                    function fo();
                    fo = it->event;
                    fo();
                    my = NULL;
                }
            }
		}

		p->skill_z = 1;
	}
    else
    {
        p->skill_z -= time_step;
    }

	if(p->skill_y > 40)
		p->flags &= ~STREAK;
	
	
	var r = 2;
	var o = r/2;
	vec_add(p->x, vector(random(r)-o,random(r)-o,random(r)-o));
	
	p->red   = maxv(128, 255 - p->skill_y);// - random(32);
	p->green = maxv(110,  180 - p->skill_y*1.5);// + random(64);
	p->blue  = maxv(64,  128 - p->skill_y*80);

	p->skill_y += 10 * time_step;
    #ifndef WEAPONS_DOUBLE_FLAME_EFFECT
		p->size += 10*time_step;
        if(p->size >= WEAPONS_FLAME_MAXSIZE)
            p->size = WEAPONS_FLAME_MAXSIZE;
	#endif  

	p->alpha = p->lifespan/2;
	if(p->lifespan < 30)
	{
		p->alpha = 2*p->lifespan - 45;
		if(p->alpha <= 0)
		p->lifespan = 0;
	}	
}

void weapons_flame_effect(PARTICLE *p)
{
	p->bmap = weapons_fire_02;
	p->flags = TRANSLUCENT | LIGHT | BRIGHT | STREAK;
	vec_set(p->blue, vector(255, 192, 192));

	vec_set(p->skill_a, weapons.flamedir);
	
	VECTOR spread;
	spread.x = random(1)-0.5;
	spread.y = random(1)-0.5;
	spread.z = random(1)-0.5;
	vec_normalize(spread, random(WEAPONS_FLAME_SPREAD));
	vec_rotate(p->skill_a, spread);
	
	p->lifespan = 100;
    p->size = WEAPONS_FLAME_MINSIZE + random(WEAPONS_FLAME_MINSIZE);
	p->event = weapons_flame_effect_event;
	p->alpha = 80 + random(10);
	p->skill[3] = (0.5 + random(1)/2) * WEAPONS_FLAME_DAMPING;
    p->skill_z = random(1);
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

	vec_set(weapons.flamedir, dir);

	vec_add(weapons.flamedir, playerGetSpeedVec());

	effect (weapons_flame_effect, maxv(1, time_frame * WEAPONS_FLAME_COUNT), pos, dir);
	playerSetLight(vector(30,180,255), 3000, 16);
	
}

void weapons_shoot_sword(VECTOR * _pos, VECTOR * _ang)
{
	VECTOR pos;
	vec_set(pos, _pos);
	vec_scale(pos, 0.1);

	vec_rotate(pos, camera.pan);
	vec_add(pos, camera.x);

	VECTOR end;
	vec_set(end, vector(0, 0, 5.7 + 0.9 * weapons.swordLength));

	vec_scale(end, 20); // sword is kinda 10 times longer

	vec_rotate(end, _ang);
	vec_rotate(end, camera.pan);
	vec_add(end, pos);

	dmgsys_set_src(DMGSYS_PLAYER, player, 1);
	var dist = c_trace(pos, end, IGNORE_PASSABLE | IGNORE_PASSENTS | USE_POLYGON | SCAN_TEXTURE | ACTIVATE_SHOOT);
	if(HIT_TARGET)
	{
		PARTICLE *p = ent_decal(you, weapons_bullethole_decal, 2 + random(3) + 0.002 * dist, random(360));
		if(p)
		{
			p->material = matDecalBasic;
			p->lifespan = 640;
		}
	}

	/*
	draw_line3d(pos, NULL, 100);
	draw_line3d(pos, COLOR_GREEN, 100);
	draw_line3d(end, COLOR_GREEN, 100);

	draw_point3d(end, COLOR_BLUE, 100, 1);
	*/
}

void weapons_shoot_cellgun()
{
	VECTOR pos;
	vec_for_vertex (pos, weapons_wp_cellgun, 393);
	vec_add(pos, weapons_wp_cellgun.x);
	vec_scale(pos, 1);

	vec_rotate(pos, camera.pan);
	vec_add(pos, camera.x);

	vec_set(target, screen_size);
	vec_scale(target, 0.5);
	target.z = 10000;
	vec_for_screen(target, camera);


	VECTOR speed;
	vec_diff(speed, target, pos);
	vec_normalize(speed, 600);
	PROJECTILE * pr0 = projectileCreate(PROJECTILE_TYPE_CELL, 1, pos, speed);
	pr0->source = player;
	pr0->dmg = WEAPONS_CELLGUN_DAMAGE;
	
	playerSetLight(vector(255,255,30), 3000, 2);
	
}

var weaponGetKickbackFac(var progress, var kickPoint)
{
	progress = progress*1.8;
	kickPoint = kickPoint*1.8;
	if(progress <= kickPoint) return sqrt(progress/kickPoint);
	progress = (progress-kickPoint)/(180-kickPoint)*180;
	return (cosv(progress)*0.5+0.5);
}

var weaponGetAttackProgress()
{
	if(weapons.current == WEAPON_CELLGUN) return weapons.spearpower;
	return weapons.attackprogress;
}

VECTOR* weapons_shotgun_get_muzzle_pos(ENTITY* ent, VECTOR* v, VECTOR* vdir)
{
	VECTOR pos;
	vec_set(pos, vector(107,0,4));
	vec_rotate(pos, ent.pan);
	vec_add(pos, ent.x);
	vec_rotate(pos, camera.pan);
	vec_add(pos, camera.x);
	
	if(vdir)
	{
		vec_set(vdir, vector(1,0,0));
		vec_rotate(vdir, ent.pan);
		vec_rotate(vdir, camera.pan);
	}
	if(v) vec_set(v,pos);
	return vector(pos.x,pos.y,pos.z);
}

void weapons_update()
{
	int i;
#ifdef DEBUG
	if(key_1) weapons_add(WEAPON_SWORD);
	if(key_2) weapons_add(WEAPON_SHOTGUN);
	if(key_3) weapons_add(WEAPON_CELLGUN);
	if(key_4) weapons_add(WEAPON_FLAMETHROWER);
	if(key_5)
	{
		for(i = 1; i <= WEAPONS_COUNT; i++)
		weapons.weapon[i].ammo = weapons.weapon[i].max_ammo;
	}
#endif

    // Keine Leben, keine Waffen.
    if(playerGetHealth() <= 0)
        return;

	if(!weapons.attacking && input_hit(INPUT_WEAPON_UP))
        weapons_select_next(1);
	if(!weapons.attacking && input_hit(INPUT_WEAPON_DOWN))
        weapons_select_next(-1);

	ent_animate(weapons_wp_sword, "Erect", 10 * clamp(weapons.swordLength, 0, 10), ANM_SKIP);

	// ignore dummy weapon for visuals
	for(i = 1; i < WEAPONS_COUNT; i++)
	{
        if(i == weapons.current) {
            weapons.weapon[i].ent.flags2 |= SHOW;
        }
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
			var isdown = input_down(INPUT_ATTACK);
			if(isdown)
			{
				if(weapons.current == WEAPON_CELLGUN)
				isdown = WEAPONS_CURRENT.ammo >= 3;
				else if(weapons.current == WEAPON_FLAMETHROWER)
				isdown = weapons_draw_ammo(1.5 * time_step);
			}
			if(weapons.attacking != isdown)
			{
				if(isdown)
				{
					//					if(weapons.current == WEAPON_FLAMETHROWER)
					//                        snd_play(weapons_snd_flamethrower_start, 100, 0);
					weapons.attackprogress = 0;
					weapons.attackstate = 0;
				}
				else
				{
					if(weapons.current == WEAPON_FLAMETHROWER)
					{
						if(weapons.flamefade == 100)
						snd_play(weapons_snd_flamethrower_end, 30, 0);
					}
					else if(weapons.current == WEAPON_CELLGUN)
					{
						snd_stop(weapons.electro);
						weapons.electro = 0;
					}
				}
				weapons.attacking = isdown;
			}
		}
		else
		{
			if(!weapons.attacking && input_hit(INPUT_ATTACK))
			{
				if(weapons_draw_ammo(1))
				{
					weapons.attacking = 1;
					weapons.attackprogress = 0;
					weapons.attackstate = 0;
				}
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
				switch(integer(random(3)))
				{
					case 0: snd_play(weapons_snd_sword1, 100, 0); break;
					case 1: snd_play(weapons_snd_sword2, 100, 0); break;
					case 2: snd_play(weapons_snd_sword3, 100, 0); break;
				}
			}

			if(weapons.attacking)
			{
				VECTOR pos, ang;
				var i;
				for(i = 0; i < 5; i++)
				{
					vec_lerp(pos, sourcePosePos, targetPosePos, 0.01 * (weapons.attackprogress + 0.2 * i * WEAPONS_CURRENT.attackspeed));
					ang_lerp(ang, sourcePoseAng, targetPoseAng, 0.01 * (weapons.attackprogress + 0.2 * i * WEAPONS_CURRENT.attackspeed));
					weapons_shoot_sword(pos, ang);
				}
			}

			break;

			case WEAPON_SHOTGUN:
			if(input_down(INPUT_BLOCK) || 1)
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
			var kickbackFac = weaponGetKickbackFac(weapons.attackprogress, 10);
            targetPosePos.x += sinv(playerGetCameraBob()) * 0.5 - 2 * kickbackFac - 3;
            targetPosePos.y += 2 * kickbackFac;
            targetPosePos.z -= 10 * kickbackFac + 2;
			ang_rotate(targetPoseAng,playerGetWeaponSway());
			//draw_text(str_printf(NULL,"kick %.1f atprogress %.1f",(double)weaponGetKickbackFac(weapons.attackprogress, 10),(double)weapons.attackprogress),400,240,COLOR_RED);

			if(weapons.attacking)
			{
				if(weapons.attackstate == 0)
				{
					VECTOR temp,vdir;
					weapons_shotgun_get_muzzle_pos(WEAPONS_CURRENT.ent, temp, vdir);
					effect(p_shotgun_muzzle_fire,36,temp,vdir);
					
					var id = snd_play(weapons_snd_shotgun, 100, 0);
					snd_tune(id, 0, 90 + random(20), 0);
					weapons.attackstate = 3;
					weapons_shoot_shotgun();
					vec_set(temp,vector(-55,0,0));
					vec_rotate(temp,vector(camera.pan,0,0));
					playerAddSpeed(temp);
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
			case WEAPON_CELLGUN:
			vec_set(sourcePosePos, WEAPONS_CELLGUN_DEFAULT_STANCE_POS);
			vec_set(sourcePoseAng, WEAPONS_CELLGUN_DEFAULT_STANCE_ANG);

			vec_set(targetPosePos, sourcePosePos);
			vec_set(targetPoseAng, sourcePoseAng);
			
			//weapons_wp_cellgun.
			vec_set(vRotatedDirection,vector(1,0,0));
			vec_rotate(vRotatedDirection,vector(total_ticks*3,total_ticks*5.1,total_ticks*3.7));
			ANGLE angle;
			vec_to_angle(angle,vRotatedDirection);
ang_to_matrix(angle,rotateMatrix);
			//trident_sphere_mat.skill1 = floatv(temp.x);
			//trident_sphere_mat.skill2 = floatv(temp.z);
			//trident_sphere_mat.skill3 = floatv(temp.y);
weapons_wp_cellgun.skill41 = floatd(weapons.spearpower,100);

			if((weapons.attacking || weapons.attackstate != 0) && WEAPONS_CURRENT.ammo > 0)
			{
				if(weapons.spearpower < 100)
				{
					ent_animate(weapons_wp_cellgun, "PowerUp", weapons.spearpower, 0);
					weapons.spearpower += WEAPONS_CURRENT.attackspeed * time_step;
					weapons.speartimer = weapons.spearpower % 100;
					weapons.attackstate = 0;
				}
				else
				{
					int spearshots[3] = { 0, 33, 66, 999 };

					if(weapons.speartimer >= spearshots[weapons.attackstate])
					{
						if(weapons.attackstate < 3)
						{
							if(weapons_draw_ammo(1))
							{
								snd_play(weapons_snd_cellgun, 100, 0);
								weapons_shoot_cellgun();
							}
							weapons.attackstate += 1;
						}
					}

					ent_animate(weapons_wp_cellgun, "ShootStuff", weapons.speartimer, ANM_CYCLE);
					weapons.speartimer += WEAPONS_CURRENT.attackspeed * time_step;
					if(weapons.speartimer >= 100) {
						weapons.speartimer -= 100;
						weapons.attackstate = 0;
					}

					weapons.spearpower = 100;
				}
			}
			else
			{
				ent_animate(weapons_wp_cellgun, "PowerDown", 100 - weapons.spearpower, 0);
				weapons.spearpower -= WEAPONS_CURRENT.attackspeed * time_step;
				if(weapons.spearpower < 0)
				weapons.spearpower = 0;
			}

			if(weapons.spearpower <= 0)
			{
				if(weapons.electro != 0)
				snd_stop(weapons.electro);
				weapons.electro = 0;
			}
			else
			{
				if(weapons.electro == 0)
				weapons.electro = snd_loop(weapons_snd_cellgun_loop, 100, 0);
				//                snd_tune(weapons.electro, weapons.spearpower, 100, 0);
			}

			var kickbackFac = weaponGetKickbackFac(weapons.speartimer, 10)*0.85;
			targetPosePos.x += sinv(playerGetCameraBob())*0.5-2*kickbackFac;
			targetPosePos.y += 2*kickbackFac;
			targetPosePos.z -= 5*kickbackFac;
			ang_rotate(targetPoseAng,playerGetWeaponSway());



			break;
		}

		if(weapons.attacking && weapons.current != WEAPON_SHOTGUN)
		{
			vec_lerp(WEAPONS_CURRENT.ent.x, sourcePosePos, targetPosePos, 0.01 * weapons.attackprogress);
			ang_lerp(WEAPONS_CURRENT.ent.pan, sourcePoseAng, targetPoseAng, 0.01 * weapons.attackprogress);
		}
		else
		{
			vec_lerp(WEAPONS_CURRENT.ent.x, WEAPONS_CURRENT.ent.x, targetPosePos, WEAPONS_LERP_SPEED);
			ang_lerp(WEAPONS_CURRENT.ent.pan, WEAPONS_CURRENT.ent.pan, targetPoseAng, WEAPONS_LERP_SPEED);
			if(weapons.attacking && weapons.current == WEAPON_SHOTGUN)
			{
				vec_set(WEAPONS_CURRENT.ent.pan,targetPoseAng);
				var kickbackFac = 0;
				if(weapons.current == WEAPON_SHOTGUN) kickbackFac = weaponGetKickbackFac(weapons.attackprogress, 10);
				else kickbackFac = weaponGetKickbackFac(weapons.speartimer, 10)*0.25;
				//var recoilSide = sinv(total_ticks*10);
				ang_rotate(WEAPONS_CURRENT.ent.pan, vector(10*kickbackFac,12*kickbackFac,-10*kickbackFac));
			}
		}
		

		weaponShotgunSmokeCounter = maxv(weaponShotgunSmokeCounter-time_step,0);
		if(weapons.current == WEAPON_SHOTGUN && weaponShotgunSmokeCounter > 0)
		{
			weaponShotgunSmokeCounter2 += pow((1+weaponShotgunSmokeCounter*0.1)*0.5,0.5)*time_step;
			while(weaponShotgunSmokeCounter2 > 0.5)
			{
				weaponShotgunSmokeCounter2 -= 0.5;
				weapons_shotgun_get_muzzle_pos(WEAPONS_CURRENT.ent, shotgun_muzzle_pos, NULL);
				effect(p_shotgun_muzzle_smoke,1,shotgun_muzzle_pos,playerGetSpeedVec());
			}
		}
		
		if(weapons.attacking && weapons.current != WEAPON_CELLGUN) // whoopsie
		{
			weapons.attackprogress += WEAPONS_CURRENT.attackspeed * time_step;
			if(weapons.attackprogress >= 100)
			{
				weapons.attackprogress = 0;
				weapons.attacking = 0;
			}
		}
	}
	else weapons.attackprogress = 0;

	if(weapons.current != WEAPON_CELLGUN)
	{
		if(weapons.electro != 0)
		{
			snd_stop(weapons.electro);
			weapons.electro = 0;
		}
	}

}

void weapons_obscure_fix(int i)
{
    /* this fixes the loop in weapons_close() which would not execute when the function was not called */
}

void weapons_close()
{
    weapons_obscure_fix(0);
    int i;
	for(i = 1; i < WEAPONS_COUNT; i++)
    {
        weapons.weapon[i].ent.flags2 &= ~SHOW;
    }
	weapons.attackprogress = 0; // no more head swaying
	on_o = NULL;
	if(weapons.electro != 0)
	{
		snd_stop(weapons.electro);
		weapons.electro = 0;
    }
    if(weapons.flamesound != 0)
    {
        snd_stop(weapons.flamesound);
        weapons.flamesound = 0;
    }
}
