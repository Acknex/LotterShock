#ifndef EYE_H
#define EYE_H

void EYE_GlobalInit();
void EYE_Init();
void EYE_Update();

//local
void EYE__inactive(ENTITY* ptr);
void EYE__patrol(ENTITY* ptr);
void EYE__attack(ENTITY* ptr);
void EYE__die(ptr);
var EYE__toFloor(ENTITY* ptr);

#endif
