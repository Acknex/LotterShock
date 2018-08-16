#ifndef MAP_H
#define MAP_H

VECTOR map_levelboundary_min;
VECTOR map_levelboundary_max;

void map_init();

void map_open();

void map_lock();

void map_update();

void map_close();

#endif // MAP_H
