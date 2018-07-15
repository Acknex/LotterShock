#ifndef INTRO_H
#define INTRO_H

void intro_init();

void intro_open();

void intro_close();

void intro_update();

bool intro_is_done();

//! ends the distress call sequence.
//! is callback for journals.
void intro_step_distress_end();

//! ends the flyby sequence.
//! is callback for journals
void intro_step_flyby_end();

#endif // INTRO_H
