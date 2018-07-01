#include "particle.h"
#include <particles.c>

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

void PARTICLE_laser(VECTOR* pos, VECTOR* dist)
{
	effect(PARTICLE__laser_init, 1, pos, dist);	
}

void PARTICLE__laser_init(PARTICLE* p)
{
	vec_set(&p->blue,vector(0,0,255));
	set(p, STREAK);
	p->lifespan = 20;
	p->alpha = 100;
	p->size = 10;
	p->event = PARTICLE__explode_fade;
}

void PARTICLE__laser_fade(PARTICLE* p)
{
	//p->alpha = maxv(p->alpha - 5* time_step, 0);
}
