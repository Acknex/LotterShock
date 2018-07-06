#include "journals.h"
#include "global.h"
#include "framework.h"

#include <acknex.h>

#define JOURNAL_ID skill1

#define JOURNAL_MINTIME_PER_CHAR (0.06 * 16) // measure for average text length designed by text[2]

FONT * journal_fnt = "Arial#16b";

PANEL *journal_pan =
{
    bmap = "journal.tga";
    flags = TRANSLUCENT;
    layer = 1;
    alpha = 0;
}

TEXT *journal_txt =
{
    flags = WWRAP | TRANSLUCENT;
    font = journal_fnt;
    strings = 1;
    layer = 2;
    red = 0;
    green = 0;
    blue = 0;
    alpha = 0;
}

int journals_current = 0;
var journals_mediahandle = 0;
var journals_mindelay = 0;

void journals_init()
{
	// Lt. Woll
	journals[0].text = "This is only my 3rd Tour with the Patchnotes but it doesn't seem to get any less crazy around here. The Science guys are working on some out of this world computer stuff. Don't understand a word of it. I hope I can make friends here. I miss my family.";
	journals[0].media = "";
	journals[0].name = crew_name0;

	// Lt. Cmdr. Queißner
	journals[1].text = "Ok. So... you know that upper section in the mess hall? Yeah that one you can't actually get to. - WHO designs a starship like that? Why have it up there? It makes no sense!! -- Anyway, I threw my keycard up there, just for fun, I thought I could just climb up there and get it back. I couldn't. I think it might be easier if I just request a new one. I'm gonna need to get a new one anyway since my promotion.";
	journals[1].media = "Media/pda_lt_cmd_queissner1.mp3";
	journals[1].name = crew_name1;

	journals[2].text = "Shit!\nI lost my Keycard again! For the 2nd time this week! I accidentally dropped it into one of the garbage chutes. And by accidentally I mean accidentally. I was going to throw away a candy bar wrapper in my right hand but dropped the keycard in my left hand instead. Who knows where it ended up at. I sure as hell am not going to go look for it in Waste Recycling! Have you seen the place? I'm not even sure why they have radioactive barrels there, but no way in hell am I risking cancer for a keycard. The captain's gonna be so mad though.... I can probably kiss that promotion goodbye. sigh Back to simple Lieutenant again...";
	journals[2].media = "Media/pda_lt_cmd_queissner2.mp3";
	journals[2].name = crew_name1;

	// Dr. Horstmann
	journals[3].text = "Infusing the Donkey-Lark DNA with Nitroglycerine might have been a bad idea. These little buggers seem to have developed some... explosive tendencies. Whenever they get excited or agitated they violently explode. We've already lost 3 interns trying to feed them. I will have to look into reinforcing the cages lest they blow their way free. If they get loose, we'll have big problems on our hands.";
	journals[3].media = "Media/pda_dr_horstmann1.mp3";
	journals[3].name = crew_name2;

	journals[4].text = "On second thought, maybe giving eyeballs some brains and an antigravity drive might also not have been the greatest move. But to be fair, how could we know what was going to happen. Maybe we can apply this tech for espionage or something.";
	journals[4].media = "Media/pda_dr_horstmann2.mp3";
	journals[4].name = crew_name2;

	journals[5].text = "Look, I'm not going to stand around and pretend that giving those robots swords for arms was my best idea. But we can all agree on at least one thing: They look pretty, frickin' cool. And aside from that whole \"homicidal attitude problem\", they have pretty much filled every checkbox we set out for this project... Well, except for their tendency to get stuck in railings and behind chest-high obstacles. But other than that, complete success!";
	journals[5].media = "Media/pda_dr_horstmann3.mp3";
	journals[5].name = crew_name2;

	journals[6].text = "My colleagues from university will be sooo jealous! I just spent the last 2 weeks re-coding the entire Öhler-Poston Engine Control System so that it can run on a Commodore 64. Yes, you heard right. Commodore. 64. Amazing, huh? Sometimes I even amaze myself! Additional plus: More computers for the digitization project! Dr. Pohl asked me where I got those computers from. I said \"It's a secret\". Between you and me? They used to run the Engine Control System. Ripped 'em right out. Put the Commodore 64 in there. Works just as well. And to think that they say I'm crazy!";
	journals[6].media = "Media/pda_dr_horstmann4.mp3";
	journals[6].name = crew_name2;

	// Dr. Pohl
	journals[7].text = "New research into the Real Life Compiler has netted us some great results. We finally cracked some of the chinese documentation on this tech. Yes we stole it, whatever. Science marches on. We think we might be able to apply the principles to an entire person!";
	journals[7].media = "Media/pda_dr_pohl1.mp3";
	journals[7].name = crew_name3;

	// Cmdr. Daumann
	journals[8].text = "The guys from the engine room asked if they could store their spare batteries somewhere. \"Why not store them in the engine room?\" I asked. \"Because there is no storage room in the engine room!\" they say. Just chuck it in the corner! Why can't they just keep their engine stuff with the engines?! Anyway, the chefs said they can keep it in their storage closet. Because THAT makes sense.... Not!";
	journals[8].media = "";
	journals[8].name = crew_name4;

	// 1st Officer Trümpler
	journals[9].text = "Everyone is so preoccupied with doing crazy science, they don't even notice the ship falling apart around them! This was supposed to be a state of the art vessel and now we've got exploding demon abominations running around and creepy flying eyeballs and coolant leaks all over the place! The guys in engineering promised to make me a flamethrower so we can get back into the cryo room but they had to give it to the Frankenstein scientists first to deal with their explosive monster problem. Now we have 3 dead interns and we still can't get into the meat storage. Idiots.";
	journals[9].media = "Media/pda_1st_off_truempler.mp3";
	journals[9].name = crew_name5;

	// Cpt. Kappes
	journals[10].text = "This is great! Who knew that being captain of a space ship was this easy? I just hang out in the bridge all day and vaguely point in the direction I want to go and then we go there. This is awesome. I don't understand what everyone is complaining about! We are in SPACE! In. Friggin'. Space.\nYeah maybe we have some science experiments gone wrong, but who cares?! We'll ship them off to the military, I'm sure they can come up with some use for them.";
	journals[10].media = "Media/pda_cpt_kappes1.mp3";
	journals[10].name = crew_name6;

	// Ensign Schwarz
	journals[11].text = "I asked one of the guys if they were playing some weird music at night but they denied it. Sure! Whatever! I know I can hear it. It wouldn't be so bad, if it wasn't such an earworm.";
	journals[11].media = "Media/pda_ensign_schwarz1.mp3";
	journals[11].name = crew_name7;

	journals[12].text = "I am SURE they are fucking with me! I stayed up all night just to check if it was them. They pretended they were sleeping and then AGAIN. That FUCKING MUSIC! They acted all surprised when I woke them up and demanded an explanation. Fuckers.";
	journals[12].media = "Media/pda_ensign_schwarz2.mp3";
	journals[12].name = crew_name7;

	journals[13].text = "I JUST HEARD IT AGAIN. They aren't even in the room! I fucking knew it! They put some hidden speaker somewhere to play the music at night! Frickin' hazing of the new guy! I'm gonna rip this room apart and find it!";
	journals[13].media = "Media/pda_ensign_schwarz3.mp3";
	journals[13].name = crew_name7;

	journals[14].text = "WHERE THE FUCK IS THIS MUSIC COMING FROM!!! I CAN'T STAND IT ANYMORE!!! IT'S DRIVING ME CRAZY! LALALALALALALA I CAN HEAR THIS MUSIC EVERYWHERE ITS IN MY DREAMS ITS ON MY MIND ALL HOURS OF THE DAY PLEASE MAKE IT STOP";
	journals[14].media = "Media/pda_ensign_schwarz4.mp3";
	journals[14].name = crew_name7;

	journals[15].text = "MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP ";
	journals[15].media = "Media/pda_ensign_schwarz5.mp3";
	journals[15].name = crew_name7;

	// Ensign Jäger
	journals[16].text = "I fixed some of the coolant lines with duct tape. It's amazing. Even in space duct tape is still the best way to fix stuff. I even used it on some of the hull breaches. Works perfectly!";
	journals[16].media = "";
	journals[16].name = crew_name8;

	// Lt. Officer Freiknecht
	journals[17].text = "Some idiot tried to fix the coolant lines with duct tape! DUCT TAPE! What the fuck? Who thought this was a good idea? Even worse: The hull breaches on level 2 reopened. Can you guess why? BECAUSE SOMEONE FIXED THEM WITH DUCT TAPE!!!!";
	journals[17].media = "";
	journals[17].name = crew_name9;

    journals_mediahandle = 0;
    journals_current = -1;
}

void journals_quit()
{
    reset(journal_pan, SHOW);
    reset(journal_txt, SHOW);
    if(journals_mediahandle != 0)
    {
        media_stop(journals_mediahandle);
    }
    journals_mediahandle = 0;
    journals_current = -1;
}

void show_journal()
{
	journal_pan.pos_x = 0;
	journal_pan.pos_y = 0;
	
    journal_txt.pos_x = journal_pan.pos_x + 10;
	journal_txt.pos_y = journal_pan.pos_y + 10;
	
    journal_txt.size_x = bmap_width(journal_pan.bmap) - 20;
    journal_txt->size_y = bmap_height(journal_pan.bmap) - 20;
	
	set(journal_pan, SHOW);
	set(journal_txt, SHOW);
}

void hide_journal()
{
	reset(journal_pan, SHOW);
	reset(journal_txt, SHOW);
}

action journal()
{
	framework_setup(my, SUBSYSTEM_JOURNAL);
    my.INTERACTIBLE = 1;
    set(my, PASSABLE);
}

void journals_update()
{
	ENTITY *ptr;
    SUBSYSTEM_LOOP(ptr, SUBSYSTEM_JOURNAL)
    {
        if (mouse_ent == ptr && input_hit(INPUT_USE))
        {
            if(ptr.JOURNAL_ID >= MAX_JOURNALS)
                return;
            if (ptr.JOURNAL_ID != journals_current)
            {
                journals_current = ptr.JOURNAL_ID;
                journals_mindelay = JOURNAL_MINTIME_PER_CHAR * str_len(journals[journals_current].text);

                str_cpy((journal_txt.pstring)[0], journals[journals_current].text);

                if(journals_mediahandle != 0)
                    media_stop(journals_mediahandle);

                if(strcmp(journals[journals_current].media, "") != 0)
                {
                    journals_mediahandle = media_play(journals[journals_current].media, NULL, 100);
                }
                else
                {
                    journals_mediahandle = 0;
                }
                break; // can break out of the loop here
            }
        }
	}

    if(journals_current >= 0)
    {
        show_journal();
        journal_pan->alpha = clamp(journal_pan->alpha + 5 * time_step, 0, 100);

        if(media_playing(journals_mediahandle) || (journals_mindelay >= 0))
        {
            journals_mindelay -= time_step;
        }
        else
        {
            journals_current = -1;
        }
#ifdef DEBUG
        DEBUG_VAR(journals_mindelay, 256);
#endif

    }
    else
    {
        if(journals_mediahandle != 0)
        {
                media_stop(journals_mediahandle);
                journals_mediahandle = 0;
        }
        journal_pan->alpha = clamp(journal_pan->alpha - 5 * time_step, 0, 100);
        if(journal_pan->alpha == 0)
            hide_journal();
    }
    journal_txt->alpha = journal_pan->alpha;
}
