#ifndef EYE_H
#define EYE_H

void EYE_GlobalInit();
void EYE_Init();
void EYE_Update();

//local
void EYE__inactive(ENTITY* ptr);
void EYE__wait(ENTITY* ptr);
void EYE__attack(ENTITY* ptr);
var EYE__toFloor(ENTITY* ptr);
var EYE__turnToPlayer(ENTITY* ptr);

#endif
