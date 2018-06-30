#include "game.h"
#include "weapons.h"

void game_init()
{
    weapons_init();
}

void game_open()
{
    weapons_open();
}

void game_update()
{
    weapons_update();
}

void game_close()
{
    weapons_close();
}
