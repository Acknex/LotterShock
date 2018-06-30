#ifndef SKULL_H
#define SKULL_H

void SKULL_GlobalInit();
void SKULL_Init();
void SKULL_Update();
void SKULL_Destroy();

// local
var SKULL__turnToPlayer(ENTITY* ptr);
void SKULL__inactive(ENTITY* ptr);
void SKULL__wait(ENTITY* ptr);
void SKULL__run(ENTITY* ptr);
void SKULL__explode(ENTITY* ptr);
void SKULL__die(ENTITY* ptr);
void SKULL__hit(ENTITY* ptr);

#endif