#include "cheats.h"
#include "game.h"
#include "settings.h"
#include "materials.h"

#define INPUT_CHEAT_COUNT 9

SOUND * input_snd_cheat_unlocked = "snd_jingle.ogg";
SOUND * input_snd_cheat_tap = "snd_button_tap.wav";

typedef struct cheatcode_t
{
    int position;
    char const * text;
    void * trigger;
} cheatcode_t;

cheatcode_t cheatcodes[INPUT_CHEAT_COUNT];

void input_reset_cheats()
{
    int i;
    for(i = 0; i < INPUT_CHEAT_COUNT; i++)
    {
        cheatcodes[i].position = 0;
    }
}

void input_put_cheat_char(char c)
{
    if(!input_cheats_enabled)
    {
        input_reset_cheats();
        return;
    }

    int i;
    bool any = false;
    for(i = 0; i < INPUT_CHEAT_COUNT; i++)
    {
        if((cheatcodes[i].text)[cheatcodes[i].position] == c && cheatcodes[i].position < strlen(cheatcodes[i].text))
        {
            cheatcodes[i].position += 1;
            if((cheatcodes[i].text)[cheatcodes[i].position] == '\0')
            {
                void (*trigger)();
                trigger = cheatcodes[i].trigger;
                if(trigger == NULL)
                    error("input(cheats): cheat code does not have a trigger!");
                trigger();

                // cheat war erfolgreich: alle cheateingaben zurücksetzen
                input_reset_cheats();
                snd_play(input_snd_cheat_unlocked, 100, 0);
                break;
            }
            else
            {
                any = true;
            }
        }
        else
        {
            cheatcodes[i].position = 0;
        }
    }
    if(any)
        snd_play(input_snd_cheat_tap, 20, random(200) - 100);
}

LRESULT CALLBACK input_original_proc_message(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK input_proc_message(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   if (WM_CHAR == message)
   {
       input_put_cheat_char((char)wParam);
   }
   return input_original_proc_message(hwnd,message,wParam,lParam);
}

void input_cheat_indestructible()
{
    movement_cheat_invincibility = !movement_cheat_invincibility;
}

void input_cheat_all_weapons()
{
    weapons_add(WEAPON_SWORD);
    weapons_add(WEAPON_SHOTGUN);
    weapons_add(WEAPON_CELLGUN);
    weapons_add(WEAPON_FLAMETHROWER);
}

void input_cheat_weapons_and_keys()
{
    weapons_add(WEAPON_SWORD);
    weapons_add(WEAPON_SHOTGUN);
    weapons_add(WEAPON_CELLGUN);
    weapons_add(WEAPON_FLAMETHROWER);
    var i;
    for(i = 0; i < 5; ++i)
    {
        keys[i] = 1;
    }
    story_enginesEnabled = 1;
}

void input_cheat_suicide()
{
    playerAddHealth(-999);
}

void input_cheat_all_die()
{
    ENTITY * it;
    for(it = ent_next(NULL); it != NULL; it = ent_next(it))
    {
        if(it->SK_SUBSYSTEM < 1000)
            continue;

        it->DAMAGE_HIT += 25000;
    }
}

void input_cheat_wallhack()
{
    ENTITY * it;
    for(it = ent_next(NULL); it != NULL; it = ent_next(it))
    {
        if(it->SK_SUBSYSTEM < 1000)
            continue;

        toggle(it, ZNEAR);
    }
}

void input_cheat_clipmode()
{
    movement_cheat_clipmode = !movement_cheat_clipmode;
}

void cheat_killhud()
{
    game_hidehud = !game_hidehud;
}

void cheat_retromode()
{
    achievements.retro_unlocked = true;
    settings.retroshader = true;
    materials_reinit(); // reload shaders to enable retro mode
    achievements_save();
}

void cheats_init()
{
    memset(cheatcodes, 0, sizeof(cheatcode_t) * INPUT_CHEAT_COUNT);

    if(on_message != input_proc_message)
    {
        input_original_proc_message = on_message;	// store the original message loop
        on_message = input_proc_message; // and replace it by your own
    }

    cheatcodes[0].text = "iddqd";
    cheatcodes[0].trigger = input_cheat_indestructible;

    cheatcodes[1].text = "idfa";
    cheatcodes[1].trigger = input_cheat_all_weapons;

    cheatcodes[2].text = "idkill";
    cheatcodes[2].trigger = input_cheat_all_die;

    cheatcodes[3].text = "idfly";
    cheatcodes[3].trigger = input_cheat_clipmode;

    cheatcodes[4].text = "idnoclip";
    cheatcodes[4].trigger = input_cheat_wallhack;

    cheatcodes[5].text = "idkfa";
    cheatcodes[5].trigger = input_cheat_weapons_and_keys;

    cheatcodes[6].text = "idseppuku";
    cheatcodes[6].trigger = input_cheat_suicide;

    cheatcodes[7].text = "idnohud";
    cheatcodes[7].trigger = cheat_killhud;

    cheatcodes[8].text = "idretro";
    cheatcodes[8].trigger = cheat_retromode;
}
