#include "weapons.h"
#include "input.h"
#include "math.h"
#include "dmgsys.h"
#include "movement.h"
#include "projectiles.h"

#define WEAPONS_COUNT 5

#define WEAPONS_LERP_SPEED 0.35

#define WEAPONS_CURRENT (weapons.weapon[weapons.current])

#define WEAPONS_SHOTGUN_RANGE  8000
#define WEAPONS_SHOTGUN_SPREAD 4 // *2 grad
#define WEAPONS_SHOTGUN_DAMAGE 5

#define WEAPONS_FLAME_VEL 100
#define WEAPONS_FLAME_SPREAD 13
#define WEAPONS_FLAME_COUNT 25
#define WEAPONS_FLAME_DAMPING 5.0

#define WEAPONS_CELLGUN_DAMAGE 7

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
} weapons_t;

weapons_t weapons;


MATERIAL* shotgunTest_mat =
{
	effect = "shotgunTest.fx";
	flags = AUTORELOAD;
}

ENTITY * weapons_wp_sword =
{
	type = "gipsy_sword.mdl";
	view = camera;
}

ENTITY * weapons_wp_shotgun =
{
	type = "triple_shotgun.mdl";
	material = shotgunTest_mat;
	view = camera;
}

ENTITY * weapons_wp_cellgun =
{
	type = "trident_of_lightning.mdl";
	view = camera;
}

ENTITY * weapons_wp_flamethrower =
{
	type = "flamemother.mdl";
	view = camera;
}

ENTITY * weapons_wp_cellgun_bzzt =
{
	type = "bzzt+4.png";
	view = camera;
}

BMAP * weapons_bullethole_decal = "bullet_hole.tga";

BMAP * weapons_fire_01 = "fire.pcx";

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

#define WEAPONS_SHOTGUN_SIGHT_STANCE_POS rel_for_screen(vector(screen_size.x/2, 2*screen_size.y/3 + 350, 15), camera)
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
	int i;
	for(i = 1; i <= WEAPONS_COUNT; i++)
	weapons.weapon[i].ammo = weapons.weapon[i].max_ammo;

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

	int i;
	for(i = 1; i <= WEAPONS_COUNT; i++)
	weapons.weapon[i].ammo = weapons.weapon[i].max_ammo;

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

bool weapons_draw_ammo(var amount)
{
	if(WEAPONS_CURRENT.max_ammo <= 0)
	return true;
	if(WEAPONS_CURRENT.ammo < amount)
	return false;
	WEAPONS_CURRENT.ammo -= amount;
	return true;
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
}

void weapons_secondary_flame_effect_event(PARTICLE *p)
{
	p->skill_a += time_step;
	
	if(p->skill_a < 30)
	p->alpha += time_step;
	else
	if (p->skill_a > 60)
	p->alpha -= 0.5*time_step;
	p->alpha -= 0.3*time_step;
	
	vec_set(p->x, p->skill_x);
	
	var r = 6;
	var o = r/2;
	vec_add(p->x, vector(random(r)-o,random(r)-o,random(r)-o));
	
	
	if(p->alpha <= 0)
	p->lifespan = 0;
}

void weapons_secondary_flame_effect(PARTICLE *p)
{
	vec_set(p->skill_x, p->x);
	
	p->bmap = weapons_fire_01;
	p->flags = LIGHT|TRANSLUCENT|BRIGHT;
	p->alpha = 0;
	vec_set(p->blue, vector(32, 32, 192));
	p->lifespan = 100;
	p->size = 25 + random(35);
	p->event = weapons_secondary_flame_effect_event;
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
			
			VECTOR secondary_position;
			vec_set(&secondary_position, &normal);
			vec_normalize(&secondary_position,25.);
			vec_add(&secondary_position, &p->x);
			
			effect (weapons_secondary_flame_effect, 1, &secondary_position, nullvector);
			//p->lifespan = 0;
			
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

	if(p->skill_y > 40)
	p->flags &= ~STREAK;
	
	
	p->red = maxv(255-p->skill_y, 128);// - random(32);
	p->green = maxv(90,180-p->skill_y*1.5);// + random(64);
	p->blue = maxv(64, 128-p->skill_y*80);

	p->skill_y += 10 * time_step;
	
	if(p->skill_y < 40)
	p->size = clamp(p->size + 10 * time_step, 5, 50);
	if(p->skill_y > 60)
	p->size = clamp(p->size - time_step, 35, 50);

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
	p->bmap = weapons_fire_01;
	vec_rotate(p->vel_x, vector(
	random(2*WEAPONS_FLAME_SPREAD)-WEAPONS_FLAME_SPREAD,
	random(2*WEAPONS_FLAME_SPREAD)-WEAPONS_FLAME_SPREAD,
	random(2*WEAPONS_FLAME_SPREAD)-WEAPONS_FLAME_SPREAD ));
	p->flags = TRANSLUCENT | LIGHT | BRIGHT | STREAK;
	vec_set(p->blue, vector(255, 192, 192));
	p->lifespan = 100;
	p->size = 5 + random(5);
	p->event = weapons_flame_effect_event;
	p->alpha = 80 + random(10);
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

	vec_add(dir, playerGetSpeedVec());

	effect (weapons_flame_effect, maxv(1, time_frame * WEAPONS_FLAME_COUNT), pos, dir);
	
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

	VECTOR speed;
	vec_set(speed,vector(600,0,0));
	vec_rotate(speed,camera.pan);
	PROJECTILE * pr0 = projectileCreate(PROJECTILE_TYPE_CELL, 1, pos, speed);
	pr0->source = player;
	pr0->dmg = WEAPONS_CELLGUN_DAMAGE;
}

var weaponGetKickbackFac(var progress, var kickPoint)
{
	progress = clamp(progress,0,100);
	progress = progress*1.8;
	kickPoint = kickPoint*1.8;
	if(progress <= kickPoint) return sqrt(progress/kickPoint);
	progress = (progress-kickPoint)/(180-kickPoint)*180;
	return (cosv(progress)*0.5+0.5);
}

var weaponGetAttackProgress()
{
	if(weapons.current == WEAPON_CELLGUN) return weapons.speartimer;
	return weapons.attackprogress;
}

void weapons_update()
{
	int i;
	if(key_1) weapons_add(WEAPON_SWORD);
	if(key_2) weapons_add(WEAPON_SHOTGUN);
	if(key_3) weapons_add(WEAPON_CELLGUN);
	if(key_4) weapons_add(WEAPON_FLAMETHROWER);

	if(!weapons_disabled)
	{
		if(!weapons.attacking && input_hit(INPUT_WEAPON_UP))
		weapons_select_next(1);
		if(!weapons.attacking && input_hit(INPUT_WEAPON_DOWN))
		weapons_select_next(-1);
	}

	ent_animate(weapons_wp_sword, "Erect", 10 * clamp(weapons.swordLength, 0, 10), ANM_SKIP);

	//draw_quad(NULL,vector(128,128,0),NULL,vector(128,128,0),NULL,COLOR_RED,100,0);

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
			targetPosePos.x += sinv(playerGetCameraBob())*0.5-2*kickbackFac-3;
			targetPosePos.y += 2*kickbackFac;
			targetPosePos.z -= 10*kickbackFac+2;
			ang_rotate(targetPoseAng,playerGetWeaponSway());
			//draw_text(str_printf(NULL,"kick %.1f atprogress %.1f",(double)weaponGetKickbackFac(weapons.attackprogress, 10),(double)weapons.attackprogress),400,240,COLOR_RED);

			if(weapons.attacking)
			{
				if(weapons.attackstate == 0)
				{
					var id = snd_play(weapons_snd_shotgun, 100, 0);
					snd_tune(id, 0, 90 + random(20), 0);
					weapons.attackstate = 3;
					weapons_shoot_shotgun();
					VECTOR temp;
					vec_set(temp,vector(-55,0,0));
					vec_rotate(temp,vector(camera.pan,0,0));
					playerAddSpeed(temp);
					playerSetLight(vector(32,160,255),2000,2);
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

			var kickbackFac = weaponGetKickbackFac(weapons.speartimer, 10)*0.25;
			targetPosePos.x += sinv(playerGetCameraBob())*0.5-2*kickbackFac;
			targetPosePos.y += 2*kickbackFac+5;
			targetPosePos.z -= 5*kickbackFac;
			ang_rotate(targetPoseAng,playerGetWeaponSway());

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

			DEBUG_VAR(weapons.spearpower, 16);
			DEBUG_VAR(weapons.speartimer, 32);
			DEBUG_VAR(weapons.electro, 48);

			DEBUG_VAR(weapons.attacking, 64);
			DEBUG_VAR(weapons.attackstate, 72);

			break;
		}

		if(weapons.attacking && weapons.current != WEAPON_SHOTGUN && weapons.current != WEAPON_CELLGUN)
		{
			vec_lerp(WEAPONS_CURRENT.ent.x, sourcePosePos, targetPosePos, 0.01 * weapons.attackprogress);
			ang_lerp(WEAPONS_CURRENT.ent.pan, sourcePoseAng, targetPoseAng, 0.01 * weapons.attackprogress);
		}
		else
		{
			vec_lerp(WEAPONS_CURRENT.ent.x, WEAPONS_CURRENT.ent.x, targetPosePos, WEAPONS_LERP_SPEED);
			ang_lerp(WEAPONS_CURRENT.ent.pan, WEAPONS_CURRENT.ent.pan, targetPoseAng, WEAPONS_LERP_SPEED);
			if(weapons.attacking && (weapons.current == WEAPON_SHOTGUN || weapons.current == WEAPON_CELLGUN))
			{
				vec_set(WEAPONS_CURRENT.ent.pan,targetPoseAng);
				//var recoilSide = sinv(total_ticks*10);
			var kickbackFac = 0;
			if(weapons.current == WEAPON_SHOTGUN) kickbackFac = weaponGetKickbackFac(weapons.attackprogress, 10);
			else kickbackFac = weaponGetKickbackFac(weapons.speartimer, 10);
				ang_rotate(WEAPONS_CURRENT.ent.pan, vector(10*kickbackFac,12*kickbackFac,-10*kickbackFac));
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

	if(weapons.current == WEAPON_CELLGUN)
	{
		weapons_wp_cellgun_bzzt.flags2 |= SHOW;
	}
	else
	{
		weapons_wp_cellgun_bzzt.flags2 &= ~SHOW;

		if(weapons.electro != 0)
		{
			snd_stop(weapons.electro);
			weapons.electro = 0;
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
	weapons.attackprogress = 0; // no more head swaying
	on_o = NULL;
}
