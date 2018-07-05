#define TURRET_ATTACKRANGE 1500
#define TURRET_ATTACK_STOP_RANGE 1800
#define TURRET_SHOOTDELAY 3
#define TURRET_TURNSPEED 5
#define TURRET_ANIMOPENSPEED 5
#define TURRET_ANIMCLOSESPEED 7
#define TURRET_ANIMDIESPEED 8
#define TURRET_ALTERNATIONTIME 64

#define ANIM_COUNTER skill14
#define DELAY_COUNTER skill15

#define SHOOT_ANGLE skill24
#define TURRET_STATE skill25
#define TURRET_TOGGLE skill26
#define TURRET_ROTMODE skill27
#define TURRET_ROTTIMER skill28
#define TURRET_HITLOCKCNT skill29
#define TURRET_LASTTILT skill30
#define SHOOT_ELEVATION skill31

#define TURRETOPEN 0
#define TURRETCLOSE 1
#define TURRETACTIVE 2
#define TURRETDIE 3
#define TURRETSLEEP 4
#define TURRETDIE 5
#define TURRETDEAD 6

#define TURRETTURNCCW 0
#define TURRETTURNCW 1
#define TURRETTURNALT 2
#define TURRETTURNAIM 3

#include "framework.h"
#include "global.h"
#include "splatter.h"
#include "settings.h"
#include "scan.h"

void TURRET__init();
void TURRET__shoot(ENTITY* ptr);
void TURRET__hitcheck(ENTITY* ptr);
void TURRET__turnOn(ENTITY* ptr);
void TURRET__turnOff(ENTITY* ptr);
void TURRET__active(ENTITY* ptr);
void TURRET__die(ENTITY* ptr);
void TURRET__dead(ENTITY* ptr);
void TURRET__sleep(ENTITY* ptr);

SOUND* sndTurretUp = "turret_up.wav";
SOUND* sndTurretDown = "turret_down.wav";
SOUND* sndTurretDestroyed = "turret_destroyed.wav";
SOUND* sndTurretShot = "turret_shot.wav";


action turret_rotccw()
{
	TURRET__init();
	my->TURRET_ROTMODE = TURRETTURNCCW;
}

action turret_rotcw()
{
	TURRET__init();
	my->TURRET_ROTMODE = TURRETTURNCW;
}

action turret_alternate()
{
	TURRET__init();
	my->TURRET_ROTMODE = TURRETTURNALT;
}

action turret_aim()
{
	TURRET__init();
	my->TURRET_ROTMODE = TURRETTURNAIM;
}

void TURRET__init()
{
   framework_setup(my, SUBSYSTEM_ENEMY_TURRET);
	ENEMY_HIT_init(my);
	my->HEALTH = HEALTH_TURRET;
	my->DELAY_COUNTER = 0;
	my->TURRET_LASTTILT = 0;
	my->TURRET_STATE = TURRETSLEEP;
	set(my, PASSABLE | POLYGON | FLAG1);
	ent_animate(my, "closed", 0, 0);
	my->material = matObject;
	my->group = GROUP_ENEMY;
}

void TURRET_Init()
{
	//DEBUG
	//ENTITY* ptr = ent_create("tile-floor-turret.mdl", vector(1288,0,10), turret_rotccw);
	//ENTITY* ptr = ent_create("tile-floor-turret.mdl", vector(1288,0,440), turret_rotccw);
	// ENTITY* ptr = ent_create("tile-floor-turret.mdl", vector(1288,0,10), turret_rotccw);
	//ptr->tilt=0;
}

void TURRET_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_TURRET)
	{
		if (player != NULL)
    	{

			if (ptr->HEALTH > 0)
			{
				TURRET__hitcheck(ptr);
			}
			
			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				SPLATTER_splat(&ptr->x, vector(1.0, 1.0, 0.0));
				ptr->TURRET_HITLOCKCNT = 3;
				ptr->event = NULL;
				
				if (ptr->HEALTH <= 0)
				{
                    achievement_kill_beast(BEAST_TURRET);
					ptr->TURRET_STATE = TURRETDIE;
					ptr.skill44 = floatv(0);
					snd_play(sndTurretDestroyed, 100, 0);
					ptr->event = NULL;
					ptr->ANIM_COUNTER = 0;
				}
			}

			switch(ptr->TURRET_STATE)
			{
				case TURRETOPEN:
					TURRET__turnOn(ptr);
					break;
				
				case TURRETCLOSE:
					TURRET__turnOff(ptr);
					break;
				
				case TURRETACTIVE:
					TURRET__active(ptr);
					break;
				
				case TURRETSLEEP:
					TURRET__sleep(ptr);
					break;
				
				case TURRETDIE:
					TURRET__die(ptr);
					break;
				
				case TURRETDEAD:
					TURRET__dead(ptr);
					break;
				
				default:
					break;
			}
		}
	}
}

void TURRET__hitcheck(ENTITY* ptr)
{
	if (ptr->TURRET_HITLOCKCNT > 0)
	{
		ptr->TURRET_HITLOCKCNT -= time_step;
		
		if (ptr->TURRET_HITLOCKCNT <= 0)
		{
			ptr->TURRET_HITLOCKCNT = 0;
			ptr->event = ENEMY_HIT_event;
			vec_zero(ptr->DAMAGE_VEC);
		}
	}
}

void TURRET__turnOn(ENTITY* ptr)
{
	reset(ptr, PASSABLE); // Active turrets must be damageable
	ptr->ANIM_COUNTER += TURRET_ANIMOPENSPEED * time_step;
	if (ptr->ANIM_COUNTER >= 100)
	{
		ptr->TURRET_STATE = TURRETACTIVE;
		ent_bonereset(ptr, "Bone1");
		ptr->SHOOT_ANGLE = 0;
	}
	ptr->ANIM_COUNTER = minv(100, ptr->ANIM_COUNTER);
	ent_animate(ptr, "open", ptr->ANIM_COUNTER, 0);
	ptr->skill41 = floatv(0.01 * ptr->ANIM_COUNTER);
}

void TURRET__turnOff(ENTITY* ptr)
{
	ptr->ANIM_COUNTER += TURRET_ANIMCLOSESPEED * time_step;
	if (ptr->ANIM_COUNTER >= 100)
	{
		set(ptr, PASSABLE); // Inactive turrets are immune
		ptr->TURRET_STATE = TURRETSLEEP;
	}
	ptr->ANIM_COUNTER = minv(100, ptr->ANIM_COUNTER);
	ent_animate(ptr, "open", 100 - ptr->ANIM_COUNTER, 0);
	if (vec_dist(player->x, ptr->x) < TURRET_ATTACKRANGE)
	{
		ptr->TURRET_STATE = TURRETOPEN;
		snd_play(sndTurretUp, 100, 0);
		ptr->ANIM_COUNTER = 100 - ptr->ANIM_COUNTER;
	}
	ptr->skill41 = floatv(0.01 * (100 - ptr->ANIM_COUNTER));
}

void TURRET__active(ENTITY* ptr)
{
	ptr->DELAY_COUNTER += time_step;
	if (vec_dist(player->x, ptr->x) > TURRET_ATTACK_STOP_RANGE)
	{
		snd_play(sndTurretDown, 100, 0);
		ptr->TURRET_STATE = TURRETCLOSE;
		ptr->ANIM_COUNTER = 0;
	}
	else 
	{
		if (ptr->DELAY_COUNTER >= TURRET_SHOOTDELAY)
		{
			TURRET__shoot(ptr);
		}
		var vTurnStep = TURRET_TURNSPEED * time_step;
		switch (ptr->TURRET_ROTMODE)
		{
			case TURRETTURNCCW:
				break;

			case TURRETTURNCW:
				vTurnStep *= -1;
				break;

			case TURRETTURNALT:
				ptr->TURRET_ROTTIMER = cycle(ptr->TURRET_ROTTIMER + time_step, 0, TURRET_ALTERNATIONTIME);
				if (ptr->TURRET_ROTTIMER > TURRET_ALTERNATIONTIME * 0.5)
				{
					vTurnStep *= -2;
				}
				else
				{
					vTurnStep *= 2;
				}
				break;

			case TURRETTURNAIM:
				break;

			default:
				break;
		}

		VECTOR vecdir;
		vec_diff(&vecdir, &player->x, &ptr->x);
		ANGLE vecang;
		vec_to_angle(&vecang, &vecdir);
		ptr->SHOOT_ELEVATION = -vecang.tilt;
		ent_bonereset(ptr, "Bone3");
		ent_bonerotate(ptr, "Bone3", vector(0, ptr->SHOOT_ELEVATION, 0));

		if (ptr->TURRET_ROTMODE == TURRETTURNAIM)
		{
			VECTOR vecDist;
			vec_set (&vecDist, player->x);
			vec_sub(&vecDist, ptr->x);
			vecDist.z = ptr->z;
			ANGLE angTemp;
			vec_to_angle(&angTemp, &vecDist);
			ptr->SHOOT_ANGLE = 180 + angTemp.pan;
			ent_bonereset(ptr, "Bone1");
			ent_bonerotate(ptr, "Bone1", vector(ptr->SHOOT_ANGLE, 0, 0));
		}
		else
		{
			ptr->SHOOT_ANGLE += vTurnStep;
			ent_bonerotate(ptr, "Bone1", vector(vTurnStep, 0, 0));
		}
		c_updatehull(ptr,1);
		ptr->DELAY_COUNTER = cycle(ptr->DELAY_COUNTER, 0, TURRET_SHOOTDELAY);		
	}
}

void TURRET__die(ENTITY* ptr)
{
		
	effect(p_spark, 1, ptr.x, vector(6,6,6));
	ptr->ANIM_COUNTER += TURRET_ANIMDIESPEED * time_step;
	ptr->ANIM_COUNTER = minv(100, ptr->ANIM_COUNTER);
	ent_animate(ptr, "die", ptr->ANIM_COUNTER, 0);	
	ent_bonerotate(ptr, "Bone1", vector(ptr->SHOOT_ANGLE, 0, 0));
	
	if (ptr->ANIM_COUNTER >= 100)
	{
		set(ptr, PASSABLE);
		ptr->TURRET_STATE = TURRETDEAD;
	}		
}

void TURRET__dead(ENTITY* ptr)
{
	if(random(100) < 5) 
	{
		effect(p_spark, 1, ptr.x, vector(6,6,6));
		ptr->skill41 = floatv(0.0);
	} 
	else 
	{
		ptr->skill41 = floatv(1.0);
	}
	wait(1);		
}

void TURRET__sleep(ENTITY* ptr)
{
	ptr->event = NULL;
	//if (vec_dist(player->x, ptr->x) < TURRET_ATTACKRANGE)
	if(SCAN_IsPlayerInSight(ptr,TURRET_ATTACKRANGE,360))
	{
		ptr->TURRET_STATE = TURRETOPEN;
		snd_play(sndTurretUp, 100, 0);
		ptr->ANIM_COUNTER = 0;
		ptr->event = ENEMY_HIT_event;
	}
	
	ptr->skill41 = floatv(clamp(0.5 * sinv(10 * total_ticks), 0, 1));
}

void TURRET__shoot(ENTITY* ptr)
{
	if (ptr->TURRET_TOGGLE != 584)
		ptr->TURRET_TOGGLE = 584;
	else
		ptr->TURRET_TOGGLE = 592;
	
	VECTOR vecPos;
	vec_for_vertex(&vecPos, ptr, ptr->TURRET_TOGGLE);
	VECTOR* vecDist = vector(-30, 0, 0);
	ANGLE* angRot = vector(ptr->SHOOT_ANGLE, ptr->SHOOT_ELEVATION, 0);
	vec_rotate(vecDist, angRot);
	vec_add (vecDist, &vecPos);
	snd_play(sndTurretShot, 100, 0);

	ENTITY* ent = ent_create(SPHERE_MDL, vecDist, enemy_projectile);	
	ent->pan = ptr->SHOOT_ANGLE;	
}

// effects
var p_spark_size_factor = 2;

void p_spark_fade(PARTICLE* p)
{
	p.vel_x *=1-0.09 * time_step;
	p.vel_z -=3 * time_step;
	p.x +=0.8 * p.vel_x * time_step;
	p.y -= 2 * time_step;
	p.z += 0.8 * p.vel_z*time_step;
}

void p_spark(PARTICLE* p) {
	p.skill_a = random(360);
	p.skill_b = 17 + random(12);
	p.vel_x = sinv(p.skill_a) * p.skill_b;
	p.vel_z = cosv(p.skill_a) * p.skill_b;
	vec_set(p.blue, vector(255, 140, 160 * random(80)));
	if (random(2) > 1) {
		set(p,BEAM);
	} else {
		set(p,STREAK);
	}
	p.bmap = bmapSpark;
	p.size = (1 + random(2)) * p_spark_size_factor;
	p.alpha = 100;
	p.lifespan = 4+random(3);
	p.event = p_spark_fade;
}

