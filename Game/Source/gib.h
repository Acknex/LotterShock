#ifndef GIB_H
#define GIB_H

void GIB_Spawn(VECTOR* pos);

/* local */
void GIB_Update();
void GIB_GlobalInit();
void GIB__fall(ENTITY* ptr);
void GIB__floor(ENTITY* ptr);

#endif