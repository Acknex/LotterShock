#include "ui.h"
#include "input.h"
#include "math.h"

#include <acknex.h>

uisystem_t * uisystem_new(int layer)
{
    uisystem_t * sys = sys_malloc(sizeof(uisystem_t));
    sys->first = NULL;
    sys->selected = NULL;
    sys->layer = layer;
    sys->flags = UISYSTEM_LIGHT;

    sys->selector = pan_create("", layer + 1);
    sys->selector->flags = UNTOUCHABLE | LIGHT;
    sys->selector->bmap = ui_bmp_selection;
    vec_set(sys->selector->blue, COLOR_RED);

    return sys;
}

void uisystem_delete(uisystem_t * system)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * btn = system->first;
    while(btn != NULL)
    {
        uibutton_t * tmp = btn;
        btn = tmp->next;
        ptr_remove(btn->pan);
        sys_free(tmp);
    }
    ptr_remove(system->selector);
    sys_free(system);
}

void uisystem_move_selection(uisystem_t * system, int dir)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * trial = system->selected;
    while(trial->neighbour[dir] != NULL)
    {
        trial = trial->neighbour[dir];
        if(is(trial->pan, SHOW))
        {
            system->selected = trial;
            return;
        }
    }

}

void uisystem_get_button_edge(VECTOR * pos, uisystem_t * system, uibutton_t * btn, int edge)
{
    pos->x = system->pos_x + btn->pos_x;
    pos->y = system->pos_y + btn->pos_y;
    pos->z = 0;

    switch(edge)
    {
    case UIDIR_UP:
        pos->x += bmap_width(btn->bmap) / 2;
        return;
    case UIDIR_LEFT:
        pos->y += bmap_height(btn->bmap) / 2;
        return;
    case UIDIR_DOWN:
        pos->x += bmap_width(btn->bmap) / 2;
        pos->y += bmap_height(btn->bmap);
        return;
    case UIDIR_RIGHT:
        pos->x += bmap_width(btn->bmap);
        pos->y += bmap_height(btn->bmap) / 2;
        return;


    }
}

void uisystem_update(uisystem_t * system)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * last = system->selected;

    // refresh all button positions and stuff
    uibutton_t * btn;
    for(btn = system->first; btn != NULL; btn = btn->next)
    {
        btn->pan->bmap = btn->bmap;
        btn->pan->pos_x = system->pos_x + btn->pos_x;
        btn->pan->pos_y = system->pos_y + btn->pos_y;

        if(mouse_panel == btn->pan && mouse_moving)
            system->selected = btn;
    }

    if(system->flags & UISYSTEM_LIGHT)
    {
        for(btn = system->first; btn != NULL; btn = btn->next)
        {
            if(system->selected == btn)
                set(btn->pan, LIGHT);
            else
                reset(btn->pan, LIGHT);
        }
    }

    if(input_hit(INPUT_UP))    uisystem_move_selection(system, UIDIR_UP);
    if(input_hit(INPUT_DOWN))  uisystem_move_selection(system, UIDIR_DOWN);
    if(input_hit(INPUT_LEFT))  uisystem_move_selection(system, UIDIR_LEFT);
    if(input_hit(INPUT_RIGHT)) uisystem_move_selection(system, UIDIR_RIGHT);

    if(!is(system->selected->pan, SHOW))
    {
        // TODO: Handle this better!
        system->selected = NULL;
    }

    // play click sound
    if(last != system->selected)
        snd_play(ui_swap_snd, 100, 0);

    if(system->selected)
    {
        if(system->flags & UISYSTEM_SELECTOR)
        {
            set(system->selector, SHOW);
            system->selector->pos_x = system->pos_x + system->selected->pos_x;
            system->selector->pos_y = system->pos_y + system->selected->pos_y;

            system->selector->scale_x = bmap_width(system->selected->bmap) / bmap_width(system->selector->bmap);
            system->selector->scale_y = bmap_height(system->selected->bmap) / bmap_height(system->selector->bmap);
        }
        else
        {
            reset(system->selector, SHOW);
        }

        var attack = input_hit(INPUT_ATTACK) && (!mouse_left || (mouse_left && (mouse_panel == system->selected->pan)));
        if(input_hit(INPUT_USE) || attack || input_hit(INPUT_JUMP))
        {
            snd_play(ui_accept_snd, 100, 0);

            function callback(uibutton_t * btn);
            callback = system->selected->callback;
            if(callback != NULL)
            {
                callback(system->selected);
            }
        }
    }
    else
    {
        reset(system->selector, SHOW);
    }

#ifdef DEBUG_UI_PATHS
    { // draw debug lines

        uibutton_t * btn;
        for(btn = system->first; btn != NULL; btn = btn->next)
        {
            if(is(btn->pan, SHOW) == 0)
                continue;

            int i;
            for(i = 0; i < 4; i++)
            {
                if(btn->neighbour[i] == NULL)
                    continue;

                VECTOR start, end;
                uisystem_get_button_edge(start, system, btn, i);
                uisystem_get_button_edge(end,   system, btn->neighbour[i], (i + 2) % 4);


                draw_line(start, NULL, 100);
                draw_line(start, COLOR_GREEN, 100);
                draw_line(end,   COLOR_RED, 25);
            }
        }
    }
#endif
}

uibutton_t * uisystem_add_button(uisystem_t * system, var x, var y, BMAP * bmap, void * callback)
{
    if(system == NULL) error("uisystem: system must not be NULL!");


    uibutton_t * btn = sys_malloc(sizeof(uibutton_t));
    memset(btn, 0, sizeof(uibutton_t));

    btn->pos_x = x;
    btn->pos_y = y;
    btn->bmap = bmap;
    btn->callback = callback;

    btn->pan = pan_create("", system->layer + 1);
    btn->pan->bmap = bmap;

    btn->next = system->first;
    system->first = btn;

    if(system->selected == NULL)
        system->selected = btn;

    return btn;
}

void uisystem_show_all(uisystem_t * system)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * btn;
    for(btn = system->first; btn != NULL; btn = btn->next)
    {
        set(btn->pan, SHOW);
    }
}

void uisystem_hide_all(uisystem_t * system)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * btn;
    for(btn = system->first; btn != NULL; btn = btn->next)
    {
        reset(btn->pan, SHOW);
    }
    reset(system->selector, SHOW);
}

void uisystem_show_group(uisystem_t * system, int group)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * btn;
    for(btn = system->first; btn != NULL; btn = btn->next)
    {
        if(btn->group == group)
            set(btn->pan, SHOW);
    }
}

void uisystem_hide_group(uisystem_t * system, int group)
{
    if(system == NULL) error("uisystem: system must not be NULL!");

    uibutton_t * btn;
    for(btn = system->first; btn != NULL; btn = btn->next)
    {
        if(btn->group == group)
            reset(btn->pan, SHOW);
    }
}
