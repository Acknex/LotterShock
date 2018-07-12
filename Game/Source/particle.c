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

void PARTICLE_laser(VECTOR* pos, VECTOR* vecDist)
{
//	var h = handle(ent);
//	effect(PARTICLE__laser_init, 1, pos, vector(h,0,0));	
	effect(PARTICLE__laser_init, 1, pos, vecDist);	
}

void PARTICLE__laser_init(PARTICLE* p)
{
	vec_set(&p->blue,vector(0,255,0));
	set(p, BEAM);
	p->lifespan = 0.1;
	p->alpha = 100;
	p->size = 10;
	p->event = NULL;
}

void PARTICLE__laser_fade(PARTICLE* p)
{
	ENTITY* ent = ptr_for_handle(p->skill_a);
	if (ent)
		vec_lerp(&p->vel_x,&p->vel_x,&ent->EYE_LASERDIST,0.5);
	p->alpha = maxv(p->alpha - 5* time_step, 0);
}

void smoke_fade_p(PARTICLE* p)
{
	p.alpha -= p.skill_a*time_step;
	if (p.alpha <= 0) p.lifespan = 0;
	
	p.size = minv(p.skill_b, p.size+time_step*3);
}

BMAP* bmp_smoke = "rauch2.tga";

void PARTICLE_smoke(PARTICLE* p)
{
	p.skill_a = 3;
	p.bmap = bmp_smoke;
	p.lifespan = 1200+random(600);
	p.skill_b = 100 + random(50);
	p.size = 20;
	p.alpha = 80;
	vec_scale(p.vel_x, 0.4);
	set(p, TRANSLUCENT | MOVE);
	p.event = smoke_fade_p;
}
