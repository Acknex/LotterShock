#ifndef PAUSEMENU_H
#define PAUSEMENU_H

#define PAUSEMENU_RESPONSE_NONE     0
#define PAUSEMENU_RESPONSE_CONTINUE 1
#define PAUSEMENU_RESPONSE_QUIT     3

void pausemenu_init();

void pausemenu_open();

void pausemenu_update();

void pausemenu_close();

int pausemenu_get_response();

void pausemenu_reset_response();

#endif // PAUSEMENU_H
