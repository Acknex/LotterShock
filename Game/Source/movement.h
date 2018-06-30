#ifndef MOVEMENT_H
#define MOVEMENT_H

void movement_update();

var playerGetCameraBob();

var playerGetSpeed();

ANGLE* playerGetWeaponSway();

void playerAddSpeed(VECTOR* v);

#endif // MOVEMENT_H
