#ifndef FRAMEWORK_H
#define FRAMEWORK_H

#define MOUSEMODE_UI   1
#define MOUSEMODE_GAME 2

//! Initialisiert das Spiel und so
void framework_init();

//! Initialisiert eine Entity für das gegebene Subsystem
void framework_setup(ENTITY * ent, int subsystem);

//! "Friert" die Entity ein und entfernt ihre Subsystemzuweisung sowie das DYNAMIC-Flag
void framework_freeze(ENTITY * ent);

//! Aktualisiert alles.
void framework_update();

//! Setzt den Mausmodus auf UI oder GAME
void framework_set_mousemode(int mode);

//! Ends the loading sequence and starts the fade in
void framework_complete_intro();

#endif // FRAMEWORK_H
