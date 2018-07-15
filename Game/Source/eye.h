#ifndef EYE_H
#define EYE_H

void EYE_GlobalInit();
void EYE_Init();
void EYE_Update();

//local
void EYE__inactive(ENTITY* ptr);
void EYE__patrol(ENTITY* ptr);
void EYE__turn(ENTITY* ptr);
void EYE__attack(ENTITY* ptr);
void EYE__die(ENTITY* ptr);
void EYE__dead(ENTITY* ptr);
void EYE__hit(ENTITY* ptr);
var EYE__toFloor(ENTITY* ptr);

#endif
