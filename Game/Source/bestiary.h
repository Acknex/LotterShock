#ifndef BESTIARY_H
#define BESTIARY_H

#define BEAST_STARLOTTI   0
#define BEAST_ACKTANA     1
#define BEAST_ESELSLERCHE 2
#define BEAST_SPUTNIK     3
#define BEAST_SKULL       4
#define BEAST_EYE         5
#define BEAST_TURRET      6
#define BEAST_COUNT       7

void bestiary_init();

void bestiary_open();

void bestiary_update();

void bestiary_close();

bool bestiary_is_done();

#endif // BESTIARY_H
