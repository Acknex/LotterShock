#ifndef FRAMEWORK_H
#define FRAMEWORK_H

//! Initialisiert das Spiel und so
void framework_init();

//! Initialisiert eine Entity für das gegebene Subsystem
void framework_setup(ENTITY * ent, int subsystem);

//! Aktualisiert alles.
void framework_update();

#endif // FRAMEWORK_H
