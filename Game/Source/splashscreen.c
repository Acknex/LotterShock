#include "splashscreen.h"
#include "input.h"
#include "settings.h"

BMAP * splashscreen_image_map = "splashscreen.png";

PANEL * splashscreen_image =
{
    bmap = splashscreen_image_map;
    size_x = 1280;
    size_y = 720;
    flags = TRANSLUCENT;
}

var splashscreen_timer;

void splashscreen_resize()
{
    splashscreen_image.scale_x = screen_size.x / splashscreen_image.size_x;
    splashscreen_image.scale_y = screen_size.y / splashscreen_image.size_y;
}

void splashscreen_init()
{
    splashscreen_resize();
    settings_register_signal(splashscreen_resize);
}

bool splashscreen_is_done()
{
    return (splashscreen_timer >= 70);
}

void splashscreen_open()
{
    set(splashscreen_image, SHOW);
    splashscreen_image.alpha = 0;
    splashscreen_timer = 0;
}

void splashscreen_update()
{
    if(input_hit(INPUT_USE) || input_hit(INPUT_ATTACK) || input_hit(INPUT_JUMP))
        splashscreen_timer = 70;

    if(splashscreen_timer < 20)
    {
        splashscreen_image->alpha = 5 * splashscreen_timer;
    }
    else if(splashscreen_timer < 50)
    {
        splashscreen_image->alpha = 100;
    }
    else if(splashscreen_timer < 70)
    {
        splashscreen_image->alpha = 100 - 5 * (splashscreen_timer - 50);
    }
    else
    {
        splashscreen_image->alpha = 0;
    }
    splashscreen_timer += time_step;
}

void splashscreen_close()
{
    reset(splashscreen_image, SHOW);
}
