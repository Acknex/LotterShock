#include "global.h"
#include "framework.h"
#include "eye.h"
#include "particle.h"
#include "scan.h"
#include "ang.h"
#include "splatter.h"
#include "gib.h"

#define EYE_PATHID skill1
#define EYE_PATHPROGRESS skill2
#define EYE_PATROLSPEED skill3
#define EYE_TURNSPEED skill4
#define EYE_ATTACKDIST skill5
#define EYE_ACTIVEDIST skill6
#define EYE_ANIMSPEED skill7
#define EYE_SHOTCOUNTER skill8

#define EYE_ANIMSTATE skill21
#define EYE_COUNTER skill22
#define EYE_STATE skill23
#define EYE_LASERDIST skill24
//#define EYE_LASERDIST skill25
//#define EYE_LASERDIST skill26
#define EYE_ACTIVEDIST skill27
#define EYE_LASTPOS skill28
//#define EYE_LASTPOS skill29
//#define EYE_LASTPOS skill30
#define EYE_PATROLLEN skill33
#define EYE_PATROLDIST skill34
#define EYE_ZOFFSET skill35
#define EYE_PATHENT skill36
#define EYE_BASESCALE skill37
//#define EYE_BASESCALE skill38
//#define EYE_BASESCALE skill39
#define EYE_LASTPAN skill40
#define EYE_ATKCOOLDOWN skill42
#define EYE_PROJECTILEENT skill43

#define EYE_STATE_INACTIVE 0
#define EYE_STATE_PATROL 1
#define EYE_STATE_TURN 2
#define EYE_STATE_ATTACK 3
#define EYE_STATE_DIE 4
#define EYE_STATE_DEAD 5
#define EYE_STATE_HIT 6

#define EYE_WALKANIM "stand"

BMAP* EYE_BmapDecal = "bulletHoleCool.tga";
BMAP* EYE_bmapSplatter[5];

SOUND* EYE_sndExplo = "eselslerche_explo.wav";
SOUND* eye_snd_hit1 = "skull_hit1.wav";
SOUND* eye_snd_hit2 = "skull_hit2.wav";


void EYE__attack(ENTITY* ptr);

// uses: EYE_PATROLSPEED, EYE_TURNSPEED, EYE_ATTACKDIST, EYE_ACTIVEDIST, EYE_PATHID, EYE_PATHPROGRESS
action Eye()
{
   framework_setup(my, SUBSYSTEM_ENEMY_EYE);

	if(my->EYE_TURNSPEED == 0) my->EYE_TURNSPEED = 15;
	if(my->EYE_PATROLSPEED == 0) my->EYE_PATROLSPEED = 15;
	if(my->EYE_ACTIVEDIST == 0) my->EYE_ACTIVEDIST = 6000;
	if(my->EYE_ATTACKDIST == 0) my->EYE_ATTACKDIST = 500;
	if(my->EYE_ANIMSPEED == 0) my->EYE_ANIMSPEED = 3;
	my->HEALTH = HEALTH_EYE;
	vec_set(&my->scale_x, vector(10,10,10));
	vec_set(&my->EYE_BASESCALE, &my->scale_x);
	my->material = matObject;
	ENTITY* ent = ent_create(NULL, nullvector, NULL);
	if (path_set(ent, str_for_num(NULL, my->EYE_PATHID)) == 0)
	{
		path_set(ent, str_for_num(NULL, 0));
	}
	my->EYE_PATROLLEN = path_length(ent);
	my->EYE_PATHPROGRESS = clamp(my->EYE_PATHPROGRESS,0,100) / 100;
	my->EYE_PATROLDIST = my->EYE_PATROLLEN * my->EYE_PATHPROGRESS;
	//error(str_for_num(NULL, my->EYE_PATROLLEN * my->EYE_PATHPROGRESS));
	path_spline (ent,&ent->x, my->EYE_PATROLDIST);
	vec_set(&my->x, ent->x);
	vec_set(&my->EYE_LASTPOS, &my->x);
	//set(my, PASSABLE);
	ENEMY_HIT_init(me);		
	my->EYE_PATHENT = handle(ent);
	my->group = GROUP_ENEMY;
	c_setminmax(me);
	set(my, SHADOW);
}	

//fix me
action Eye_shot()
{
	set (my, PASSABLE|LIGHT|TRANSLUCENT);
	my->EYE_COUNTER = 0;
	my->alpha = 100;
	VECTOR to;
	while(my->EYE_COUNTER < 6)
	{
		my->EYE_COUNTER += time_step;
		vec_add(&my->scale_x, vector(4*time_step, 4*time_step, 4*time_step));
		vec_set (&to, &player->x);
		vec_sub (&to, &my->x);
		vec_to_angle(&my->pan, &to);
		my->alpha = maxv(0, my->alpha -  5* time_step);
		wait(1);
	}
		ptr_remove(me);
}

void EYE_GlobalInit()
{
	EYE_bmapSplatter[0] = bmap_create("splatter_red_01.png");
	EYE_bmapSplatter[1] = bmap_create("splatter_red_02.png");
	EYE_bmapSplatter[2] = bmap_create("splatter_red_03.png");
	EYE_bmapSplatter[3] = bmap_create("splatter_red_04.png");
	EYE_bmapSplatter[4] = bmap_create("splatter_red_05.png");
	int i;
	for (i= 0; i < 5; i++)
	{
		bmap_preload(EYE_bmapSplatter[i]);
	}
}

void EYE_Init()
{
	//DEBUG
		//ENTITY* ptr = ent_create("enemy_eye.mdl", vector(400,1600,200), Eye);
//		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(5900,-6050,250), Eye);		
//		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(1200,6500,250), Eye);		
//		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(100000,100000,250), Eye);		
//		ENTITY* ptr = ent_create("enemy_eye.mdl", vector(1288,0,250), Eye);		
}

void EYE_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_EYE)
	{
		if (player)
		{
			ptr->EYE_ANIMSTATE += ptr->EYE_ANIMSPEED * time_step;
    		ptr->EYE_ZOFFSET = 0;

			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				ptr->event = NULL;
				ptr->EYE_STATE = EYE_STATE_HIT;
				switch(integer(random(2)))
				{
					case 0: snd_play(eye_snd_hit1, 100, 0); break;
					default: snd_play(eye_snd_hit2, 100, 0); break;
				}
				SPLATTER_splat(&ptr->x, vector(1.0,0.0,0.0));
				ptr->EYE_COUNTER = 0;
				ptr->EYE_LASTPAN = ptr->pan;
				SPLATTER_explode(20, ptr, 350, EYE_bmapSplatter, 5);
			}

			switch(ptr->EYE_STATE)    	
			{
				case EYE_STATE_INACTIVE:
				{
					EYE__inactive(ptr);
					break;
				}

				case EYE_STATE_PATROL:
				{
					EYE__patrol(ptr);
					break;
				}

				case EYE_STATE_TURN:
				{
					EYE__turn(ptr);
					break;
				}
				
				case EYE_STATE_ATTACK:
				{
					EYE__attack(ptr);
					break;
				}

				case EYE_STATE_HIT:
				{
					EYE__hit(ptr);
					break;
				}

				case EYE_STATE_DIE:
				{
					EYE__die(ptr);
					break;
				}

				case EYE_STATE_DEAD:
				{
					EYE__dead(ptr);
					break;
				}
				
				default:
				{
					break;
				}

			}
		}
		
	}
}

var EYE__toFloor(ENTITY* ptr)
{
	VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 10);
	VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
	me = ptr;
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_POLYGON;// | USE_BOX;
	c_trace(from, to, mode);
	if(HIT_TARGET)
		ptr->z = hit.z + 150 + ptr->EYE_ZOFFSET;
}

void EYE__inactive(ENTITY* ptr)
{
	//adjust eye position but do not do anything else
	ptr->EYE_PATROLDIST = cycle(ptr->EYE_PATROLDIST + ptr->EYE_PATROLSPEED*time_step,0,ptr->EYE_PATROLLEN);
	ENTITY* ent = ptr_for_handle(ptr->EYE_PATHENT);
	path_spline (ent,&ent->x,ptr->EYE_PATROLDIST);

	/* transitions */
	if(SCAN_IsPlayerNear(ent, ptr->EYE_ACTIVEDIST))
	{
		ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);
		if (SCAN_IsPlayerInSight(ent, ptr->EYE_ACTIVEDIST, 360) || SCAN_IsPlayerNear(ent, ptr->EYE_ACTIVEDIST * 0.3))
		{
			ptr->EYE_STATE = EYE_STATE_PATROL;
			vec_set(&ptr->x, &ent->x);
		}
	}
}

void EYE__patrol(ENTITY* ptr)
{
	ptr->EYE_ZOFFSET = 20 * sinv(total_ticks * 20);
	ent_animate(ptr, EYE_WALKANIM, ptr->EYE_ANIMSTATE, ANM_CYCLE);

	ENTITY* ent = ptr_for_handle(ptr->EYE_PATHENT);
	//eye stuck detection. do not progress path until eye caught up
	if(vec_dist(&ptr->x, &ent->x) < 100)
	{
		ptr->EYE_PATROLDIST = cycle(ptr->EYE_PATROLDIST + ptr->EYE_PATROLSPEED*time_step,0,ptr->EYE_PATROLLEN);
		path_spline (ent,&ent->x,ptr->EYE_PATROLDIST);
	}

	VECTOR vdiff;
	vec_diff(&vdiff, &ent->x, &ptr->x);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | GLIDE;
	c_ignore(GROUP_ENEMY,0);
	c_move(ptr, nullvector, vdiff, mode);

	ANGLE vecAngle;
	VECTOR vecDir;
	vec_diff(&vecDir,&ptr->x,ptr->EYE_LASTPOS);
	vec_to_angle(&vecAngle,&vecDir);
	vec_set(ptr->EYE_LASTPOS,&ptr->x);
	ANG_turnToAngle(ptr, vecAngle.pan, ptr->EYE_TURNSPEED, 1);

	/* transitions */
	if (SCAN_IsPlayerInSight(ptr, ptr->EYE_ATTACKDIST, 360))
	{
		ptr->EYE_COUNTER = 0;
		ptr->EYE_STATE = EYE_STATE_TURN;
	}
	else if (!SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST * 1.1))
	{
		ptr->EYE_STATE = EYE_STATE_INACTIVE;
	}
	else
	{
		
	}
}

void EYE__turn(ENTITY* ptr)
{
	ptr->EYE_ATKCOOLDOWN = maxv(0, ptr->EYE_ATKCOOLDOWN - time_step);
	//no hysteresis here. did not turn out well
	if (!SCAN_IsPlayerNear(ptr, ptr->EYE_ATTACKDIST))
	{
		ptr->EYE_COUNTER = 0;
		ptr->EYE_STATE = EYE_STATE_PATROL; 
	}
	else if (ANG_turnToPlayer(ptr, ptr->EYE_TURNSPEED, 0))
	{
		if (ptr->EYE_ATKCOOLDOWN <= 0)
		{
			ptr->EYE_COUNTER = 0;
			ptr->EYE_ATKCOOLDOWN = 0;
			ptr->EYE_STATE = EYE_STATE_ATTACK;
			VECTOR* dist = vector(20, 0,0);
			VECTOR to;
			vec_set(&to, &player->x);
			vec_sub(&to, &ptr->x);
			ANGLE dir;
			vec_to_angle(&dir, &to);
			dir.pan = ptr->pan;
			vec_rotate(dist, &dir);
			vec_add (dist, &ptr->x);
			ENTITY* ent = ent_create("eye_shot.mdl", &ptr->x/*dist*/, Eye_shot);
			//vec_set(&ent->scale_x, &ptr->scale_x);
			vec_scale(&ent->scale_x, 16);
		} 
	}
	else
	{
	}
	
}

void EYE__attack(ENTITY* ptr)
{
	ANG_turnToPlayer(ptr, ptr->EYE_TURNSPEED * 0.2, 0);
	ptr->EYE_COUNTER += time_step;
	ptr->EYE_ATKCOOLDOWN += time_step;
	
	VECTOR from;
	vec_for_vertex(from, ptr, 195);
	VECTOR to;
	vec_set(&to, &player->x);
	vec_sub(&to, &ptr->x);
	ANGLE dir;
	vec_to_angle(&dir, &to);
	//ptr->pan = dir.pan;
	dir.pan = ptr->pan;
	vec_set(&ptr->EYE_LASERDIST, vector(ptr->EYE_ATTACKDIST, 0, 0));
	vec_rotate(&ptr->EYE_LASERDIST, &dir);
	//vec_set(&to, &player->x);
	vec_set(&to, &ptr->EYE_LASERDIST);
	vec_add(&to, &from);
	me = ptr;
	var mode = IGNORE_ME | IGNORE_WORLD | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | SCAN_TEXTURE | USE_POLYGON;

	//laser is not stopped by player - new trace needed
	c_ignore(GROUP_ENEMY, GROUP_PLAYER,0);
	c_trace(&from, &to, mode);
	if(HIT_TARGET)
	{
		//eye cannot laser through walls
		vec_set(&ptr->EYE_LASERDIST, &hit.x);
		vec_sub(&ptr->EYE_LASERDIST, &ptr->x);
		//don't place decal every frame
		if (ptr->EYE_COUNTER > 2)
		{
	     	PARTICLE* p = ent_decal(you, EYE_BmapDecal, 10 + random(20), random(360));
			if(p) // ent_decal can fail!!
	     	{
	     		p->material = matDecalBlood;
	     		p->lifespan = 64;
	     		p->event = SPLATTER__fade_effect;
	     		p->flags |= TRANSLUCENT;
	     		p->alpha = 50 + random(30);
			}
  		}
	}

	//test if player was hit - this time do not ignore player
	if (ptr->EYE_COUNTER > 2)
	{
		c_ignore(GROUP_ENEMY, 0);
		c_trace(&from, &to, mode);
		if(HIT_TARGET)
		{
			if (vec_dist(&player->x, &hit->x) < 100)
			{
				playerAddHealth(-DAMAGE_EYE);
			}	
		}
		ptr->EYE_COUNTER = cycle(ptr->EYE_COUNTER,0,2);
	}
	
	PARTICLE_laser(from, &ptr->EYE_LASERDIST);

	
	if (ptr->EYE_ATKCOOLDOWN > 16)
	{
		ptr->EYE_COUNTER = 0;
		ptr->EYE_ATKCOOLDOWN = 32;
		ptr->EYE_STATE = EYE_STATE_TURN;
	}

/*		if (!SCAN_IsPlayerInSight(ptr, ptr->EYE_ACTIVEDIST, 100) && !SCAN_IsPlayerNear(ptr, ptr->EYE_ACTIVEDIST * 0.3))
		{
			ptr->EYE_STATE = EYE_STATE_INACTIVE;
		}
		*/
}

void EYE__hit(ENTITY* ptr)
{
	ptr->EYE_COUNTER += 10 * time_step;
	ptr->pan = ptr->EYE_LASTPAN + 4*ptr->EYE_COUNTER;

	/* transitions */
	if (ptr->HEALTH <= 0)
	{
        achievement_kill_beast(BEAST_EYE);
		ptr->EYE_COUNTER = 0;
		ptr->EYE_STATE = EYE_STATE_DIE;
		set(ptr, PASSABLE);
	}
	else if (ptr->EYE_COUNTER >= 90)
	{
		ptr->EYE_STATE = EYE_STATE_PATROL;			
		ptr->event = ENEMY_HIT_event;
		ptr->EYE_COUNTER = 0;
		vec_zero(ptr->DAMAGE_VEC);
	}
	else if (ptr->EYE_COUNTER>= 50)
	{
		ptr->event = ENEMY_HIT_event;		
	}
	else
	{
	}
}

void EYE__die(ENTITY* ptr)
{
	ptr->EYE_COUNTER += time_step;
	vec_set(&ptr->scale_x, &ptr->EYE_BASESCALE);
	//factor 18 * 20 should make this end with cos(360deg) = 1 = max scale before poof
	vec_scale(&ptr->scale_x, 1 + (0.1*ptr->EYE_COUNTER * absv(cosv(2*ptr->EYE_COUNTER * 18))));
	VECTOR diff;
	vec_diff(&diff, &player->x, &ptr->x);
	ANGLE dir;
	vec_to_angle(&dir, &diff);
	ptr->pan = dir.pan;
	
	/* transitions */
	if(ptr->EYE_COUNTER >= 20)
	{
		var i;
		for ( i = 0; i < 3; i++)
		{
			GIB_Spawn(&ptr->x);
		}
		SPLATTER_explode(50, ptr, 400, EYE_bmapSplatter, 5);
		ptr->EYE_STATE = EYE_STATE_DEAD;
		//ptr->SK_ENTITY_DEAD = 1; //keep entity alive a while longer for sound playback
		ent_playsound2(ptr, EYE_sndExplo, 10000, 10000);
		set(ptr, INVISIBLE); //pretend I'm gone
		ptr->EYE_COUNTER = 0;
	}

}

void EYE__dead(ENTITY* ptr)
{
	ptr->EYE_COUNTER += time_step;
	if (ptr->EYE_COUNTER > 32)
	{
		ptr->SK_ENTITY_DEAD = 1;		
	}
}
