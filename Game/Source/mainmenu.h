#ifndef MAINMENU_H
#define MAINMENU_H

#define MAINMENU_RESPONSE_STILLACTIVE 0
#define MAINMENU_RESPONSE_START       1
#define MAINMENU_RESPONSE_CREDITS     2
#define MAINMENU_RESPONSE_EXIT        3

void mainmenu_init();

void mainmenu_open();

void mainmenu_update();

void mainmenu_close();

int mainmenu_get_response();

#endif // MAINMENU_H
