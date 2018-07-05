#ifndef SKULL_H
#define SKULL_H

void SKULL_GlobalInit();
void SKULL_Init();
void SKULL_Update();

// local
void SKULL__toFloor(ENTITY* ptr);
//var SKULL__turnToPlayer(ENTITY* ptr);
void SKULL__inactive(ENTITY* ptr);
void SKULL__wait(ENTITY* ptr);
void SKULL__run(ENTITY* ptr);
void SKULL__attack(ENTITY* ptr);
void SKULL__die(ENTITY* ptr);
void SKULL__hit(ENTITY* ptr);
void SKULL__retreat(ENTITY* ptr);
void SKULL__fireParticle(PARTICLE *p);
void SKULL__fireEffect(PARTICLE *p);
void SKULL__smokeEffect(PARTICLE *p);
void SKULL__shootParticle(PARTICLE *p);
void SKULL__shootEffect(PARTICLE *p);

#endif