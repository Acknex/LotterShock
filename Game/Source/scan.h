#ifndef SCAN_H
#define SCAN_H

bool SCAN_invisibility = 0;

var SCAN_IsPlayerNear(ENTITY* ent, var distance);
var SCAN_IsPlayerInSight(ENTITY* ent, var distance, var fov);
var SCAN_IsPlayerBehind(ENTITY* ent, var distance);

#endif
