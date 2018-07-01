#ifndef SPUTNIK_H
#define SPUTNIK_H

void SPUTNIK_GlobalInit();
void SPUTNIK_Init();
void SPUTNIK_Update();
void SPUTNIK_Destroy();

// local
var SPUTNIK__turnToPlayer(ENTITY* ptr);
void SPUTNIK__inactive(ENTITY* ptr);
void SPUTNIK__wait(ENTITY* ptr);
void SPUTNIK__follow(ENTITY* ptr);
void SPUTNIK__die(ENTITY* ptr);


//SPUTNIK_STATE_INACTIVE 0
//SPUTNIK_STATE_WAIT 1
//SPUTNIK_STATE_FOLLOW 2
//SPUTNIK_STATE_ATTACK 3
//SPUTNIK_STATE_DIE 4
//SPUTNIK_STATE_DEAD 5

#endif