#ifndef PARTICLE_H
#define PARTICLE_H

void PARTICLE_explode(var count, VECTOR* pos);
void PARTICLE_laser(VECTOR* pos, ENTITY* ent);

/* local */
void PARTICLE__explode_fade(PARTICLE* p);
void PARTICLE__explode_init(PARTICLE* p);
void PARTICLE__laser_init(PARTICLE* p);
void PARTICLE__laser_fade(PARTICLE* p);

#endif