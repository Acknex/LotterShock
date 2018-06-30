#ifndef ESELSLERCHE_H
#define ESELSLERCHE_H

void ESELSLERCHE_Init();
void ESELSLERCHE_Update();
void ESELSLERCHE_Destroy();

// local
var ESELSLERCHE__turnToPlayer(ENTITY* ptr);
void ESELSLERCHE__inactive(ENTITY* ptr);
void ESELSLERCHE__wait(ENTITY* ptr);
void ESELSLERCHE__run(ENTITY* ptr);
void ESELSLERCHE__explode(ENTITY* ptr);
void ESELSLERCHE__die(ENTITY* ptr);
void ESELSLERCHE__hit(ENTITY* ptr);

#endif