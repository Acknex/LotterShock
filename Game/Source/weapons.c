#include "weapons.h"
#include "input.h"
#include "math.h"
#include "dmgsys.h"

#define WEAPONS_COUNT 5

#define WEAPONS_LERP_SPEED 0.25

#define WEAPONS_CURRENT (weapons.weapon[weapons.current])

#define WEAPONS_SHOTGUN_RANGE  8000
#define WEAPONS_SHOTGUN_SPREAD 4 // *2 grad
#define WEAPONS_SHOTGUN_DAMAGE 5

typedef struct weapons_data_t
{
	ENTITY * ent;
	SOUND  * snd;
	bool unlocked;
	bool autofire;
	var cooldown;
	var attackspeed;
} weapons_data_t;

typedef struct weapons_t
{
	int current;
	bool attacking;
	var attackprogress;
	int attackstate;
	int swordLength;
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
	type = "gipsy_sword.mdl";
	view = camera;
}

ENTITY * weapons_wp_flamethrower =
{
	type = "gipsy_sword.mdl";
	view = camera;
}

BMAP * weapons_bullethole_decal = "bullet_hole.tga";

SOUND * weapons_snd_sword = "sword_snd.wav";
SOUND * weapons_snd_shotgun = "shotgun_snd.wav";
SOUND * weapons_snd_cellgun = "cellgun_snd.wav";
SOUND * weapons_snd_flamethrower = "flamethrower_snd.wav";

VECTOR debugVec;

#define WEAPONS_SWORD_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x - 280, screen_size.y - 30, 100), camera)
#define WEAPONS_SWORD_DEFAULT_STANCE_ANG vector(-70, -20, 0)

#define WEAPONS_SWORD_BLOCK_STANCE_POS rel_for_screen(vector(screen_size.x - 80, screen_size.y - 20, 100), camera)
#define WEAPONS_SWORD_BLOCK_STANCE_ANG vector(0, 0, -60)

#define WEAPONS_SWORD_ATTACK_STANCE_POS rel_for_screen(vector(screen_size.x/3, screen_size.y + 10, 90), camera)
#define WEAPONS_SWORD_ATTACK_STANCE_ANG vector(00, -100, -30)

#define WEAPONS_SHOTGUN_DEFAULT_STANCE_POS rel_for_screen(vector(screen_size.x - 200, screen_size.y - 20, 35), camera)
#define WEAPONS_SHOTGUN_DEFAULT_STANCE_ANG vector(0,0,0)

#define WEAPONS_SHOTGUN_SIGHT_STANCE_POS rel_for_screen(vector(screen_size.x/2, 2*screen_size.y/3 + 380, 16), camera)
#define WEAPONS_SHOTGUN_SIGHT_STANCE_ANG vector(0,0,0)

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

	weapons.weapon[WEAPON_SWORD].snd = weapons_snd_sword;
	weapons.weapon[WEAPON_SHOTGUN].snd = weapons_snd_shotgun;
	weapons.weapon[WEAPON_CELLGUN].snd = weapons_snd_cellgun;
	weapons.weapon[WEAPON_FLAMETHROWER].snd = weapons_snd_flamethrower;

	weapons.weapon[WEAPON_SWORD].attackspeed = 40;
	weapons.weapon[WEAPON_SHOTGUN].attackspeed = 10;

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
		vec_for_angle(dir, vector(camera.pan + random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD, camera.tilt  + random(2*WEAPONS_SHOTGUN_SPREAD) - WEAPONS_SHOTGUN_SPREAD, 0));
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
	return weapons.attackprogress;
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
		/*draw_text(
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
		COLOR_RED);*/
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
			weapons.attackstate = 0;
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
			targetPosePos.x += sinv(playerGetCameraBob())-2-2*kickbackFac;
			targetPosePos.y += 2*kickbackFac;
			targetPosePos.z -= 10*kickbackFac;
			ang_rotate(targetPoseAng,playerGetWeaponSway());
			//draw_text(str_printf(NULL,"kick %.1f atprogress %.1f",(double)weaponGetKickbackFac(weapons.attackprogress, 10),(double)weapons.attackprogress),400,240,COLOR_RED);

			if(weapons.attacking)
			{
				if(weapons.attackstate == 0)
				{
					var id = snd_play(WEAPONS_CURRENT.snd, 100, 0);
					snd_tune(id, 0, 90 + random(20), 0);
					weapons.attackstate = 3;
					weapons_shoot_shotgun();
					VECTOR temp;
					vec_set(temp,vector(-55,0,0));
					vec_rotate(temp,vector(camera.pan,0,0));
					playerAddSpeed(temp);
				}
				/*if(weapons.attackprogress >= 10 && weapons.attackstate == 0)
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
				}*/
			}

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
				//var recoilSide = sinv(total_ticks*10);
				ang_rotate(WEAPONS_CURRENT.ent.pan, vector(20*kickbackFac,12*kickbackFac,-25*kickbackFac));
			}
		}
	

		if(weapons.attacking)
		{
			weapons.attackprogress += WEAPONS_CURRENT.attackspeed * time_step;
			if(weapons.attackprogress >= 100)
			{
				weapons.attackprogress = 0;
				weapons.attacking = 0;
			}
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
