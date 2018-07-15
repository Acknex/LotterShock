#ifndef UI_H
#define UI_H

SOUND * ui_swap_snd = "mainmenu_click.wav";
SOUND * ui_accept_snd = "mainmenu_accept.wav";

BMAP * ui_bmp_background = "ui_background.png";

BMAP * ui_bmp_selection = "ui_selector.png";

#define UIDIR_UP 0
#define UIDIR_LEFT 1
#define UIDIR_DOWN 2
#define UIDIR_RIGHT 3

#define UISYSTEM_SELECTOR (1<<0)
#define UISYSTEM_LIGHT    (1<<1)

typedef struct uibutton_t
{
    // optical
    var pos_x, pos_y;
    BMAP * bmap;
    int group;

    // event stuff
    void * callback;

    // property stuff
    var skill[10];

    // navigation
    struct uibutton_t * neighbour[4];

    // private section:
    struct uibutton_t * next; // linked list
    PANEL * pan;
} uibutton_t;

typedef struct uisystem_t
{
    // global offset for all buttons
    var pos_x, pos_y;
    int layer;
    int flags;

    // private section;
    PANEL * selector;
    uibutton_t * first;
    uibutton_t * selected;
} uisystem_t;

uisystem_t * uisystem_new(int layer);
void uisystem_delete(uisystem_t * system);

void uisystem_update(uisystem_t * system);

void uisystem_show_all(uisystem_t * system);
void uisystem_hide_all(uisystem_t * system);

void uisystem_show_group(uisystem_t * system, int group);
void uisystem_hide_group(uisystem_t * system, int group);

uibutton_t * uisystem_add_button(uisystem_t * system, var x, var y, BMAP * bmap, void * callback);

#endif // UI_H
