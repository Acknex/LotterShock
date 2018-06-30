#include "game.h"
#include "weapons.h"
#include "movement.h"
#include "input.h"

bool game_done;

void game_init()
{
    weapons_init();
}

void game_open()
{
    game_done = false;
    weapons_open();
}

void game_update()
{
    movement_update();
    weapons_update();

    if(input_hit(INPUT_NAVBACK))
        game_done = true;
}

void game_close()
{
    weapons_close();
}

bool game_is_done()
{
    return game_done;
}
