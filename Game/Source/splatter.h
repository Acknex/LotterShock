#ifndef SPALTTER_H
#define SPLATTER_H

void SPLATTER_explode(var count, ENTITY* ent, var distance, BMAP** decal, var bmapCount);
void SPLATTER_splat(VECTOR* pos, VECTOR* n);


//local
void SPLATTER__fade_effect(PARTICLE* p);

#endif