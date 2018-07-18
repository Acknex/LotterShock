#include "global.h"
#include "framework.h"
#include "skull.h"
#include "scan.h"
#include "enemy_hit.h"
#include "ang.h"
#include "particle.h"
#include "settings.h"

#define SKL_RUNSPEED skill1
#define SKL_TURNSPEED skill2
#define SKL_ANIMSPEED skill3
#define SKL_ATTACKDIST skill4
#define SKL_ACTIVEDIST skill5

#define SKL_COUNTER skill22
#define SKL_STATE skill23
#define SKL_RUNSPEEDCUR skill24
#define SKL_ANIMSTATE skill25
#define SKL_COUNTER2 skill28
#define SKL_HITDIR skill30
#define SK_LASTPOS skill32
//#define SK_LASTPOS skill33
//#define SK_LASTPOS skill34
#define SKL_ZOFFSET skill35
#define SKL_BASEZ skill36

#define SKL_RUNANIM "stand"

#define SKL_STATE_INACTIVE 0
#define SKL_STATE_WAIT 1
#define SKL_STATE_RUN 2
#define SKL_STATE_ATTACK 3
#define SKL_STATE_DIE 4
#define SKL_STATE_DEAD 5
#define SKL_STATE_HIT 6
#define SKL_STATE_RETREAT 7

SOUND* skull_snd_shoot = "skull_shoot.wav";
SOUND* skull_snd_death = "skull_death.wav";

// uses: SKL_RUNSPEED, SKL_TURNSPEED, SKL_ATTACKDIST, SKL_ACTIVEDIST
action Skull()
{
   framework_setup(my, SUBSYSTEM_ENEMY_SKULL);
	if(my->SKL_RUNSPEED == 0) my->SKL_RUNSPEED = 25;
	if(my->SKL_TURNSPEED == 0) my->SKL_TURNSPEED = 20;
	if(my->SKL_ANIMSPEED == 0) my->SKL_ANIMSPEED = 8;
	if(my->SKL_ATTACKDIST == 0) my->SKL_ATTACKDIST = 300;
	if(my->SKL_ACTIVEDIST == 0) my->SKL_ACTIVEDIST = 3000;
	my->HEALTH = HEALTH_SKULL;
	ENEMY_HIT_init(my);
	vec_scale(&my->scale_x, 1.5);
	set(my, SHADOW);
	c_setminmax(me);
	my->material = matSkull;
	my->group = GROUP_ENEMY;
	
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_BOX;
	VECTOR* from = vector(my->x, my->y, my->z + 10);
	VECTOR* to = vector(my->x, my->y, my->z - 1000);
	c_ignore(GROUP_ENEMY, GROUP_PLAYER, 0);
	if (c_trace(from, to, mode))
		my->z = hit.z + 150 + my->SKL_ZOFFSET;
		
	my->SKL_BASEZ = my->z;
}

void SKULL_GlobalInit()
{
}

void SKULL_Init()
{
	// ENTITY* ptr = ent_create("whiskas.mdl", vector(5800,-6050,250), Skull); // biodome
}

void SKULL_Update()
{
	ENTITY * ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_ENEMY_SKULL)
	{
		if (player != NULL)
    	{
			ptr->SKL_ANIMSTATE += ptr->SKL_ANIMSPEED * time_step;
			ptr->SKL_ANIMSTATE = cycle(ptr->SKL_ANIMSTATE, 0, 100);
			
    		//DEBUG_VAR(ptr->SKL_STATE, 50);
			if (ptr->DAMAGE_HIT > 0)
			{
				ptr->roll = 0;
				ptr->HEALTH = maxv(0, ptr->HEALTH - ptr->DAMAGE_HIT);
				ptr->DAMAGE_HIT = 0;
				ptr->event = NULL;
				ptr->SKL_STATE = SKL_STATE_HIT;
				SPLATTER_splat(&ptr->x, vector(0.8,0.0,0.2));
				set(ptr, TRANSLUCENT);
			}
			
			ptr->SKL_ZOFFSET = 0;
			
			switch(ptr->SKL_STATE)    	
			{
				case SKL_STATE_INACTIVE:
				{
					SKULL__inactive(ptr);
					break;
				}

				case SKL_STATE_WAIT:
				{
					SKULL__wait(ptr);
					break;
				}

				case SKL_STATE_RUN:
				{
					SKULL__run(ptr);
					break;
				}

				case SKL_STATE_ATTACK:
				{
					SKULL__attack(ptr);
					break;
				}

				case SKL_STATE_DIE:
				{
					SKULL__die(ptr);
					break;
				}

				case SKL_STATE_HIT:
				{
					SKULL__hit(ptr);
					break;
				}

				case SKL_STATE_RETREAT:
				{
					SKULL__retreat(ptr);
					break;
				}

				default:
				{
					break;
				}

			}	

			if (ptr->SKL_STATE != SKL_STATE_DIE && ptr->SKL_STATE != SKL_STATE_DEAD)
			{
				SKULL__toFloor(ptr);
				int vertices[] = {73, 200, 67, 201, 68};
				CONTACT contact;
				int i;
				for(i = 0; i < 5; i++)
				{
					ent_getvertex(ptr, &contact, vertices[i]);
					vec_scale(contact.x, ptr->scale_x);
					vec_rotate(contact.x, ptr->pan);
					vec_add(contact.x, ptr->x);
					
					VECTOR velocity;
					vec_set(velocity, nullvector);
					velocity.x = -20 - random(20);
					velocity.z = 20 + random(30);
					vec_rotate(velocity, ptr->pan);
					if (ptr->SKL_STATE == SKL_STATE_HIT)
					{
						effect(SKULL__smokeEffect, 6*time_step, contact.x, velocity);
					}
					else
					{
						effect(SKULL__fireEffect, 6*time_step, contact.x, velocity);
					}
				}
			}
		}
	
	}	
}

var SKULL__toFloor(ENTITY* ptr)
{
	VECTOR* from = vector(ptr->x, ptr->y, ptr->z + 10);
	VECTOR* to = vector(ptr->x, ptr->y, ptr->z - 1000);
	me = ptr;
	var mode = IGNORE_ME | IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_PUSH | IGNORE_SPRITES | IGNORE_CONTENT | USE_BOX;
	ptr.min_x += 8;
	ptr.min_y += 8;
	ptr.max_x -= 8;
	ptr.max_y -= 8;
	var vmin, vmax;

	c_ignore(GROUP_ENEMY, GROUP_PLAYER, 0);
	c_trace(from, to, mode);
	if (HIT_TARGET)
	{
		vmin = hit->z;
	}
	else
	{
		vmin = to->z;
	}
	//30: max z-sway
	vmin = vmin - ptr->min_z + 30;
	to = vector(ptr->x, ptr->y, ptr->z + 1000);	
	c_ignore(GROUP_ENEMY, GROUP_PLAYER, 0);
	c_trace(from, to, mode);
	if (HIT_TARGET)
	{
		vmax = hit->z;
	}
	else
	{
		vmax = to->z;
	}
	//30: max z-sway
	vmax = vmax - ptr->max_z - 30;
	ptr.min_x -= 8;
	ptr.min_y -= 8;
	ptr.max_x += 8;
	ptr.max_y += 8;

	var zOffs = 130;
	if (ptr->SKL_BASEZ > player->z + zOffs)
	{
		ptr->SKL_BASEZ = clamp(ptr->SKL_BASEZ - 5*time_step, player->z + zOffs, vmax);
	}
	else
	{
		ptr->SKL_BASEZ = clamp(ptr->SKL_BASEZ + 5*time_step, vmin, player->z + zOffs);
	}
	ptr->z = ptr->SKL_BASEZ + ptr->SKL_ZOFFSET;
	
	/*if (HIT_TARGET)
	{
		var newZ = hit.z + 150 + ptr->SKL_ZOFFSET;
		ptr->z = ptr->z*0.5 + newZ*0.5;
		if (absv(newZ - ptr->z) < 2)
			ptr->z = newZ;
	}*/
}

void SKULL__inactive(ENTITY* ptr)
{
	/* transitions */
	if(SCAN_IsPlayerNear(ptr, ptr->SKL_ACTIVEDIST))
	{
		ptr->SKL_ZOFFSET = 15 * sinv(total_ticks * 50);
		if (SCAN_IsPlayerInSight(ptr, ptr->SKL_ACTIVEDIST, 360) || SCAN_IsPlayerNear(ptr, ptr->SKL_ACTIVEDIST * 0.3))
		{
			ptr->SKL_STATE = SKL_STATE_WAIT;
		}
	}
}

void SKULL__wait(ENTITY* ptr)
{
	ptr->SKL_ZOFFSET = 30 * sinv(total_ticks * 20);

	/* transitions */
	if(ANG_turnToPlayer(ptr, ptr->SKL_TURNSPEED, 5) != 0)
	{
		ptr->SKL_RUNSPEEDCUR = 0;
		ptr->SKL_STATE = SKL_STATE_RUN;
	}
	else if(!SCAN_IsPlayerNear(ptr, ptr->SKL_ACTIVEDIST + 100))
	{
		ptr->SKL_STATE = SKL_STATE_INACTIVE;
	}
	else
	{
	}
	
}

void SKULL__run(ENTITY* ptr)
{
	ent_animate(ptr, SKL_RUNANIM, ptr->SKL_ANIMSTATE, ANM_CYCLE);
	ptr->SKL_RUNSPEEDCUR = minv(ptr->SKL_RUNSPEEDCUR + 6*time_step, ptr->SKL_RUNSPEED);
	VECTOR* to = vector(ptr->SKL_RUNSPEEDCUR + 2, 0, 0);
	vec_rotate(to, vector(ptr->pan,0,0));
	vec_add (to, &ptr->x);
	var tracemode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_CONTENT;
	c_ignore (GROUP_ENEMY, GROUP_PLAYER, 0);
	if (c_trace(&ptr->x, to, tracemode) <= 0)
	{
		var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | /*IGNORE_PUSH |*/ GLIDE | USE_POLYGON;
		ptr.min_x -= 2;
		ptr.min_y -= 2;
		ptr.max_x += 2;
		ptr.max_y += 2;
		c_move(ptr, vector(ptr->SKL_RUNSPEEDCUR, 0, 0), nullvector, mode);
		ptr.min_x += 2;
		ptr.min_y += 2;
		ptr.max_x -= 2;
		ptr.max_y -= 2;
	}
	
	/* transitions */
	if (SCAN_IsPlayerInSight(ptr, ptr->SKL_ATTACKDIST, 75))
	{
		ptr->SKL_COUNTER = 0;
		ptr->SKL_COUNTER2 = 0;
		ptr->SKL_STATE = SKL_STATE_ATTACK;
		playerAddHealth(-DAMAGE_SKULL);
		snd_play(skull_snd_shoot, 100, 0);
	}
	else if (/*!SCAN_IsPlayerInSight(ptr, ptr->SKL_ATTACKDIST, 75) && */SCAN_IsPlayerBehind(ptr, 1200))
	{
		ptr->SKL_STATE = SKL_STATE_WAIT;
	}
	else
	{
		
	}

}

void SKULL__attack(ENTITY* ptr)
{
	ent_animate(ptr, SKL_RUNANIM, 0, 0);
	ptr->SKL_COUNTER += time_step/16;
	if(ptr->SKL_COUNTER > 0.05)
	{
		int i;
		for(i = 0; i < 5; i++)
		{	
			VECTOR velocity;
			vec_set(velocity, &player->x);
			vec_sub(velocity, &ptr->x);
			velocity.z = 5;
			vec_normalize(velocity, 80);
			 vec_rotate(velocity, vector(random(10)-5, random(10)-5, 0));
			effect(SKULL__shootEffect, 1, &ptr->x, velocity);
		}
		ptr->SKL_COUNTER2 += 1;
		ptr->SKL_COUNTER -= 0.05;
		if(ptr->SKL_COUNTER2 > 5)
		{
			ptr->SKL_STATE = SKL_STATE_RETREAT;	
		}
	}	
}

void SKULL__die(ENTITY* ptr)
{
	ptr->SKL_COUNTER = minv(ptr->SKL_COUNTER + 4*time_step, 100);
	var animState = (100 - ptr->SKL_COUNTER ) / 100;	
	vec_set(&ptr->scale_x, vector(animState, animState, animState));
	VECTOR* pos = vector(ptr->x+random(10)-5, ptr->y+random(10)-5, ptr->z+random(10)-5);
	VECTOR* vel = vector(-5-random(10), -2-random(4), 2+random(4));
	effect(PARTICLE_smoke, 6*time_step, pos, vel);

	/* transitions */
	if(animState <= 0)
	{
		reset(ptr, SHADOW);
		ptr->SKL_COUNTER = 0;
		ptr->SKL_STATE = SKL_STATE_DEAD;
	}
}

void SKULL__hit(ENTITY* ptr)
{
	ent_animate(ptr, SKL_RUNANIM, 100, 0);
	ptr->SKL_COUNTER = minv(ptr->SKL_COUNTER + 4*time_step, 40);
	
	VECTOR dir;
	vec_set(&dir, ptr->DAMAGE_VEC);
	vec_scale(&dir, 5*time_step);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, nullvector, dir, mode);

	/* transitions */
	if (ptr->HEALTH <= 0)
    {
        achievement_kill_beast(BEAST_SKULL);
		reset(ptr, TRANSLUCENT);
		snd_play(skull_snd_death, 100, 0);
		set(ptr, PASSABLE);
		ptr->SKL_STATE = SKL_STATE_DIE;
	}
	else if (ptr->SKL_COUNTER >= 40)
	{
		reset(ptr, TRANSLUCENT);
		ptr->SKL_STATE = SKL_STATE_WAIT;			
		ptr->event = ENEMY_HIT_event;
		vec_zero(ptr->DAMAGE_VEC);
		ptr->SKL_COUNTER = 0;
	}
	else if (ptr->SKL_COUNTER >= 30)
	{
		ptr->event = ENEMY_HIT_event;		
	}
	else
	{
	}
}

void SKULL__retreat(ENTITY* ptr)
{
	ptr->SKL_COUNTER = minv(ptr->SKL_COUNTER + 4*time_step, 120);
	var mode = IGNORE_PASSABLE | IGNORE_PASSENTS | IGNORE_SPRITES | IGNORE_PUSH | GLIDE | USE_POLYGON;
	c_move(ptr, vector(ptr->SKL_RUNSPEED*-0.4, 0, 0), nullvector, mode);

	/* transitions */
	if (ptr->SKL_COUNTER > 20)
	{
		ptr->SKL_STATE = SKL_STATE_WAIT;	
		ptr->SKL_COUNTER = 0;
	}
}

void SKULL__fireParticle(PARTICLE *p)
{
	p.size -= time_step;
	p.alpha -= p.skill_a*time_step;
	if(p.alpha <= 0) p.lifespan = 0;
}

void SKULL__fireEffect(PARTICLE *p)
{
	set(p, MOVE | BRIGHT | TRANSLUCENT);
	p.red = 255;
	p.green = 0;
	p.blue = 0;
	p.alpha = 100;
	p.lifespan = 100;
	p.size = 50;
	p.vel_z = 20 + random(30);
	p.gravity = -20.0;
	p.skill_a = 20.0; // fade factor
	p.event = SKULL__fireParticle;
}

void SKULL__smokeEffect(PARTICLE *p)
{
	set(p, MOVE | TRANSLUCENT);
	p.red = 70;
	p.green = 70;
	p.blue = 70;
	p.alpha = 100;
	p.lifespan = 100;
	p.size = 50;
	p.vel_z = 20 + random(30);
	p.gravity = -20.0;
	p.skill_a = 20.0; // fade factor
	p.event = SKULL__fireParticle;
}

void SKULL__shootParticle(PARTICLE *p)
{
	p.size += 50*time_step;
	p.alpha -= p.skill_a*time_step;
	if(p.alpha <= 0) p.lifespan = 0;
}

void SKULL__shootEffect(PARTICLE *p)
{
	set(p, MOVE | BRIGHT | TRANSLUCENT);
	p.red = 255;
	p.green = 0;
	p.blue = 0;
	p.alpha = 100;
	p.lifespan = 100;
	p.size = 100;
	p.skill_a = 20.0; // fade factor
	p.event = SKULL__fireParticle;
}
