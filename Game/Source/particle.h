#ifndef PARTICLE_H
#define PARTICLE_H

void PARTICLE_explode(var count, VECTOR* pos);

/* local */
void PARTICLE__explode_fade(PARTICLE* p);
void PARTICLE__explode_init(PARTICLE* p);

#endif