#include "particle.h"
#include <particles.c>

#define EYE_LASERDIST skill24
//#define EYE_LASERDIST skill25
//#define EYE_LASERDIST skill26

void PARTICLE_explode(var count, VECTOR* pos)
{
	effect(PARTICLE__explode_init, count, pos, nullvector);	
}

void PARTICLE__explode_fade(PARTICLE* p)
{
	p->alpha = maxv(p->alpha - 10* time_step, 0);
}

void PARTICLE__explode_init(PARTICLE* p)
{
	vec_randomize(&p->vel_x,40);
	vec_set(&p->blue,vector(5,174,86));
	set(p,MOVE | BRIGHT | BEAM);
	p->lifespan = 20+random(1);
	p->alpha = 90 + random(10);
	p->gravity = 1;
	p->size = 3;
	p->event = PARTICLE__explode_fade;
}

void PARTICLE_laser(VECTOR* pos, ENTITY* ent)
{
	var h = handle(ent);
	effect(PARTICLE__laser_init, 1, pos, vector(h,0,0));	
}

void PARTICLE__laser_init(PARTICLE* p)
{
	p->skill_a = p->vel_x;
	ENTITY* ent = ptr_for_handle(p->skill_a);
	vec_set(&p->vel_x,&ent->EYE_LASERDIST);
	vec_set(&p->blue,vector(0,0,255));
	set(p, STREAK);
	p->lifespan = 200;
	p->alpha = 100;
	p->size = 10;
	p->event = PARTICLE__laser_fade;
}

void PARTICLE__laser_fade(PARTICLE* p)
{
	ENTITY* ent = ptr_for_handle(p->skill_a);
	if (ent)
		vec_lerp(&p->vel_x,&p->vel_x,&ent->EYE_LASERDIST,0.5);
	p->alpha = maxv(p->alpha - 5* time_step, 0);
}
