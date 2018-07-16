#include "journals.h"
#include "global.h"
#include "framework.h"
#include "intro.h"

#include <acknex.h>

#define JOURNAL_ID skill1

#define JOURNAL_MINTIME_PER_CHAR (0.06 * 16) // measure for average text length designed by text[2]

FONT * journal_fnt = "Arial#16b";
FONT * journal_name_fnt = "Arial#24b";
FONT * journal_subtitle_fnt = "Arial#32b";

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
    red = 200;
    green = 200;
    blue = 200;
    alpha = 0;
}

TEXT *journal_txt_name =
{
    flags = TRANSLUCENT | CENTER_Y | CENTER_X;
    font = journal_name_fnt;
    strings = 1;
    layer = 2;
    red = 200;
    green = 200;
    blue = 200;
    alpha = 0;
}

TEXT *journal_subtitle_txt =
{
    flags = WWRAP | TRANSLUCENT | OUTLINE;
    font = journal_subtitle_fnt;
    strings = 1;
    layer = 999;
    red = 255;
    green = 255;
    blue = 255;
    alpha = 0;
}

int journals_current = 0;
int journals_current_level = 0;
var journals_mediahandle = 0;
var journals_timeout = 0;

#define MAX_JOURNALS 100
journal_t journals[MAX_JOURNALS];

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

	// Dr. Schwarzfeuer
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
	journals[8].media = "Media/pda_cmdr_daumann1.mp3";
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
    journals[16].media = "Media/pda_ensign_jaeger1.mp3";
	journals[16].name = crew_name8;

	// Lt. Officer Freiknecht
	journals[17].text = "Some idiot tried to fix the coolant lines with duct tape! DUCT TAPE! What the fuck? Who thought this was a good idea? Even worse: The hull breaches on level 2 reopened. Can you guess why? BECAUSE SOMEONE FIXED THEM WITH DUCT TAPE!!!!";
	journals[17].media = "";
	journals[17].name = crew_name9;

    ///////////////////////////////////////////////////////////////////////
    // PART 1
    ///////////////////////////////////////////////////////////////////////

    // Game Intro Sequence

    journals[49].text = "Docking procedure complete.";
    journals[49].type = JOURNAL_TYPE_SUBTITLE;
    journals[49].name = acktana_name;
    journals[49].event = framework_complete_intro;
    journals[49].media = "Media/story_part1_1.mp3";
    journals[49].followup = 18;

    journals[18].text = "Preliminary scans reveal that the ship seems to be running on emergency power.";
    journals[18].type = JOURNAL_TYPE_SUBTITLE;
    journals[18].name = acktana_name;
    journals[18].media = "Media/story_part1_2.mp3";
    journals[18].followup = 19;

    journals[19].text = "Can we fix it?";
    journals[19].type = JOURNAL_TYPE_SUBTITLE;
    journals[19].name = lotter_name;
    journals[19].media = "Media/story_part1_3.mp3";
    journals[19].followup = 20;

    journals[20].text = "Main ship system power is generated by the ship engines. Turning them back on would restore power to most systems.";
    journals[20].type = JOURNAL_TYPE_SUBTITLE;
    journals[20].name = acktana_name;
    journals[20].media = "Media/story_part1_4.mp3";
    journals[20].followup = 21;

    journals[21].text = "Alright, Let's code, motherfucker.";
    journals[21].type = JOURNAL_TYPE_SUBTITLE;
    journals[21].name = lotter_name;
    journals[21].media = "Media/story_part1_5.mp3";

    // Finding battery sequence
    journals[22].text = "Analysis of this battery reveals that it should have enough power to jumpstart the ion-containment field in order to restart the engine poston-field generator.";
    journals[22].name = acktana_name;
    journals[22].type = JOURNAL_TYPE_SUBTITLE;
    journals[22].media = "Media/story_part1_6.mp3";
    journals[22].followup = 23;

    journals[23].text = " In other words: Stick it into the engine and press the big red button?";
    journals[23].type = JOURNAL_TYPE_SUBTITLE;
    journals[23].name = lotter_name;
    journals[23].media = "Media/story_part1_7.mp3";
    journals[23].followup = 24;

    journals[24].text = "Yes, that would be another way to say it. You imbecile.";
    journals[24].type = JOURNAL_TYPE_SUBTITLE;
    journals[24].media = "Media/story_part1_8.mp3";
    journals[24].name = acktana_name;

    ///////////////////////////////////////////////////////////////////////
    // PART 2
    ///////////////////////////////////////////////////////////////////////

    // Insert battery into engine

    journals[25].text = "I've got the power! Acktana, how's it lookin'?";
    journals[25].type = JOURNAL_TYPE_SUBTITLE;
    journals[25].name = lotter_name;
    journals[25].media = "Media/story_part2_1.mp3";
    journals[25].followup = 26;

    journals[26].text = "That seems to have done it. Main power is coming back online... Connecting to ship information system, stand by.... Power is go. Engines are go. Lavatories are go. Life support is go. Gravity generators are online. Server systems are offline.";
    journals[26].type = JOURNAL_TYPE_SUBTITLE;
    journals[26].name = acktana_name;
    journals[26].media = "Media/story_part2_2.mp3";
    journals[26].followup = 27;

    journals[27].text = "That's strange, why are the servers offline?";
    journals[27].type = JOURNAL_TYPE_SUBTITLE;
    journals[27].name = lotter_name;
    journals[27].media = "Media/story_part2_3.mp3";
    journals[27].followup = 28;

    journals[28].text = "System information shows that the servers are powered independently. There seems to be a higher power draw than usual. It would be impossible to know why, until I get a good look at the servers.";
    journals[28].type = JOURNAL_TYPE_SUBTITLE;
    journals[28].name = acktana_name;
    journals[28].media = "Media/story_part2_4.mp3";
    journals[28].followup = 29;

    journals[29].text = "Let's get you there then!";
    journals[29].type = JOURNAL_TYPE_SUBTITLE;
    journals[29].name = lotter_name;
    journals[29].media = "Media/story_part2_5.mp3";
    journals[29].followup = 30;

    journals[30].text = "Powering up the generators seems to have opened up some additional doors. That should help. . . . . You're welcome.";
    journals[30].type = JOURNAL_TYPE_SUBTITLE;
    journals[30].media = "Media/story_part2_6.mp3";
    journals[30].name = acktana_name;

    // Finding the Flamethrower

    journals[31].text = "This should help with the frozen door.";
    journals[31].type = JOURNAL_TYPE_SUBTITLE;
    journals[31].media = "Media/story_part2_7.mp3";
    journals[31].name = lotter_name;

    // Finding Jetpackl/Jumpboots

    journals[32].text = "This is a prototype Silverman-Stolipin Gravity Assist module!";
    journals[32].type = JOURNAL_TYPE_SUBTITLE;
    journals[32].name = acktana_name;
    journals[32].media = "Media/story_part2_8.mp3";
    journals[32].followup = 33;

    journals[33].text = "In English please, Acktana";
    journals[33].type = JOURNAL_TYPE_SUBTITLE;
    journals[33].name = lotter_name;
    journals[33].media = "Media/story_part2_9.mp3";
    journals[33].followup = 34;

    journals[34].text = "Perform a mid-air jump";
    journals[34].type = JOURNAL_TYPE_SUBTITLE;
    journals[34].name = acktana_name;
    journals[34].media = "Media/story_part2_10.mp3";
    journals[34].followup = 35;

    journals[35].text = "So... double jumping?";
    journals[35].type = JOURNAL_TYPE_SUBTITLE;
    journals[35].name = lotter_name;
    journals[35].media = "Media/story_part2_11.mp3";
    journals[35].followup = 36;

    journals[36].text = "Yes, \"double jumping\".";
    journals[36].type = JOURNAL_TYPE_SUBTITLE;
    journals[36].media = "Media/story_part2_12.mp3";
    journals[36].name = acktana_name;

    ///////////////////////////////////////////////////////////////////////
    // PART 3
    ///////////////////////////////////////////////////////////////////////

    // Server room enter first time

    journals[37].text = "There's the terminal there at the end. Get me there and I'll take a look.";
    journals[37].media = "Media/story_part3_1.mp3";
    journals[37].type = JOURNAL_TYPE_SUBTITLE;
    journals[37].name = acktana_name;

    // Server use first time

    journals[38].text = "Connecting...\n[modem sounds]\nIt seems that the Patchnotes was working on digitization prior to the incident, and when it arrived they uploaded the crew into the computer to escape the Unity Union boarding party. Power requirements for the system increased exponentially with this many people. The system is running on a low power mode. Until we can get it back up, I can't get you any more information.";
    journals[38].type = JOURNAL_TYPE_SUBTITLE;
    journals[38].name = acktana_name;
    journals[38].media = "Media/story_part3_2.mp3";
    journals[38].followup = 39;

    journals[39].text = "Start the generator and talk to the computer people, got it.";
    journals[39].type = JOURNAL_TYPE_SUBTITLE;
    journals[39].name = lotter_name;
    journals[39].media = "Media/story_part3_3.mp3";

    ///////////////////////////////////////////////////////////////////////
    // PART 4
    ///////////////////////////////////////////////////////////////////////

    // Power core boot

    journals[40].text = "Power core output stable at 1.21 Gigawatts. Servers are resuming operation now. Take me back.";
    journals[40].type = JOURNAL_TYPE_SUBTITLE;
    journals[40].name = acktana_name;
    journals[40].media = "Media/story_part4_1.mp3";
    journals[40].followup = 41;

    journals[41].text = " You got it babe.";
    journals[41].type = JOURNAL_TYPE_SUBTITLE;
    journals[41].name = lotter_name;
    journals[41].media = "Media/story_part4_2.mp3";

    // Server terminal

    journals[42].text = "[Modem sounds] .... This is terrible. Apparently immediately after uploading themselves, they got attacked by a computer virus by the Unity union. They deployed countermeasures.... something called the... Guardian System? It defeated the Unity Union Virus, but it also trapped them in nullpointer space.";
    journals[42].type = JOURNAL_TYPE_SUBTITLE;
    journals[42].name = acktana_name;
    journals[42].media = "Media/story_part4_3.mp3";
    journals[42].followup = 43;

    journals[43].text = "Anything we can do?";
    journals[43].type = JOURNAL_TYPE_SUBTITLE;
    journals[43].name = lotter_name;
    journals[43].media = "Media/story_part4_4.mp3";
    journals[43].followup = 44;

    journals[44].text = "Jack in, turn off the Guardian and jack out?";
    journals[44].type = JOURNAL_TYPE_SUBTITLE;
    journals[44].name = acktana_name;
    journals[44].media = "Media/story_part4_5.mp3";
    journals[44].followup = 45;

    journals[45].text = " Alright! Let's jack in, kill that jack-off and jack out!";
    journals[45].type = JOURNAL_TYPE_SUBTITLE;
    journals[45].name = lotter_name;
    journals[45].media = "Media/story_part4_6.mp3";

    // interacting with matrix bed

    journals[46].text = "That's a weird place for a bed.";
    journals[46].type = JOURNAL_TYPE_SUBTITLE;
    journals[46].name = lotter_name;
    journals[46].media = "Media/lotter_part4_7.mp3";
    journals[46].followup = 47;

    journals[47].text = "It seems to be some kind of human-computer-interface, but it's powered off right now.";
    journals[47].type = JOURNAL_TYPE_SUBTITLE;
    journals[47].name = acktana_name;
    journals[47].media = "Media/acktana_part4_8.mp3";
    journals[47].followup = 48;

    journals[48].text = "Alright!";
    journals[48].media = "Media/lotter_part4_9.mp3";
    journals[48].type = JOURNAL_TYPE_SUBTITLE;
    journals[48].name = lotter_name;

    // 49 IS DECLARED ABOVE!!!

#define JOURNAL(_id,_name,_media,_text) do { journals[_id].name = _name; journals[_id].type = JOURNAL_TYPE_SUBTITLE; journals[_id].media = _media; journals[_id].text = _text; journals[_id].followup = ((_id)+1); } while(false)

    JOURNAL(50, NULL, NULL, "22 Hours after the death pebble incident...");

    JOURNAL(51, acktana_name, "Media/intro_chunk_51.mp3", "Star-Lotti! We have received a wide-range emergency broadcast. It Identifies the ship as the long-lost USS Patchnotes.");
    JOURNAL(52, lotter_name, "Media/intro_chunk_52.mp3", " Patch it through.");
    JOURNAL(53, acktana_name, "Media/intro_chunk_53.mp3", "Patching through...");

    JOURNAL(54, patchnotes_name, "Media/intro_chunk_54.mp3", "MAYDAY, MAYDAY, MAYDAY");
    JOURNAL(55, patchnotes_name, "Media/intro_chunk_55.mp3", "THIS IS USS PATCHNOTES, USS PATCHNOTES, USS PATCHNOTES");
    JOURNAL(56, patchnotes_name, "Media/intro_chunk_56.mp3", "CALL SIGN ACKPATCH");
    JOURNAL(57, patchnotes_name, "Media/intro_chunk_57.mp3", "MAYDAY");
    JOURNAL(58, patchnotes_name, "Media/intro_chunk_58.mp3", "USS PATCHNOTES CALL SIGN ACKPATCH,\nPOSITION 42, 69, 1024 AT 0745 HOURS, OCTOBER 17TH 2015");
    JOURNAL(59, patchnotes_name, "Media/intro_chunk_59.mp3", "WE HAVE BEEN ATTACKED BY A UNITY UNION BATTLECRUISER AND HAVE BEEN BOARDED");
    JOURNAL(60, patchnotes_name, "Media/intro_chunk_60.mp3", "IMMEDIATE ASSISTANCE REQUIRED");
    JOURNAL(61, patchnotes_name, "Media/intro_chunk_61.mp3", "WE HAVE 29 SOULS REMAINING ABOARD AS OF TIME OF THIS MESSAGE");
    JOURNAL(62, patchnotes_name, "Media/intro_chunk_62.mp3", "OVER");
    JOURNAL(63, acktana_name, "Media/intro_chunk_63.mp3", "Message repeats every two minutes.");
    JOURNAL(64, lotter_name, "Media/intro_chunk_64.mp3", "That distress signal dates back 3 years! About the time the Patchnotes vanished...\nideas?");
    JOURNAL(65, acktana_name, "Media/intro_chunk_65.mp3", "Acknation Galactic Laws require us to answer any-");
    JOURNAL(66, lotter_name, "Media/intro_chunk_66.mp3", "...friendly distress call, yes, yes... Fine. Set a course.");
    JOURNAL(67, acktana_name, "Media/intro_chunk_67.mp3", "As you command.");
    journals[67].event = intro_step_distress_end;
    journals[67].followup = 0; // breakup the sequence here

    JOURNAL(68, NULL, NULL, "3 Hours later...");
    JOURNAL(69, lotter_name, "Media/intro_chunk_69.mp3", "It really is her... the USS Patchnotes... What a sight...");
    JOURNAL(70, acktana_name, "Media/intro_chunk_70.mp3", "Sensors indicate heavy damage to internal systems and signs of boarding. Ship not responding to hails. No sign of the enemy.");
    JOURNAL(71, lotter_name, "Media/intro_chunk_71.mp3", "No reason for them to stick around after all this time... take us in. I want to take a look around inside.");
    JOURNAL(72, acktana_name, "Media/intro_chunk_72.mp3", "Commencing docking procedure.");
    journals[72].event = intro_step_flyby_end;
    journals[72].followup = 0; // END OF SEQUENCE!

#undef JOURNAL

    journals_mediahandle = 0;
    journals_current = -1;
}

void journals_quit()
{
    hide_journal();

    if(journals_mediahandle != 0)
    {
        media_stop(journals_mediahandle);
    }
    journals_mediahandle = 0;
    journals_current = -1;
}

void hide_journal()
{
    reset(journal_pan, SHOW);
    reset(journal_txt, SHOW);
    reset(journal_txt_name, SHOW);
    reset(journal_subtitle_txt, SHOW);
}

void show_journal()
{
    if(journals_current < 0)
        return; // nah
	journal_pan.pos_x = 0;
	journal_pan.pos_y = 0;
	
    journal_txt.pos_x = journal_pan.pos_x + 10;
	journal_txt.pos_y = journal_pan.pos_y + 10;
	
    journal_txt.size_x = bmap_width(journal_pan.bmap) - 20;
    journal_txt->size_y = bmap_height(journal_pan.bmap) - 20;

    journal_txt_name->pos_x = journal_pan->pos_x + 141;
    journal_txt_name->pos_y = journal_pan->pos_y + 194;

    journal_subtitle_txt->pos_x = 100;
    journal_subtitle_txt->size_x = (screen_size.x - 200);
    journal_subtitle_txt->pos_y = screen_size.y - 200; // TODO: Adjust this value

    hide_journal();
    if(journals[journals_current].type == JOURNAL_TYPE_SUBTITLE)
    {
        set(journal_subtitle_txt, SHOW);
    }
    else
    {
        set(journal_pan, SHOW);
        set(journal_txt, SHOW);
        set(journal_txt_name, SHOW);
    }
}

void journals_stop()
{
    journals_current = -1;
    hide_journal();
}

action journal()
{
	framework_setup(my, SUBSYSTEM_JOURNAL);
    my.INTERACTIBLE = 1;
    set(my, PASSABLE);
}

void journals_pause()
{
    if(journals_mediahandle != 0)
        media_pause(journals_mediahandle);
}

void journals_resume()
{
    if(journals_mediahandle != 0)
        media_start(journals_mediahandle);
}

void journals_play(int id, int level)
{
    if(id < 0 || id >= MAX_JOURNALS)
    {
        error("journals_play: id out of range!");
        return;
    }

    // we are already playing this journal
    if (id == journals_current)
        return;

    // we aren't powerful enough to override the current journal
    if(level < journals_current_level)
        return;

    journals_current = id;
    journals_current_level = level;
    journals_timeout = total_ticks + maxv(16, JOURNAL_MINTIME_PER_CHAR * str_len(journals[journals_current].text));

    if(journals[journals_current].name != NULL)
    {
        str_cpy((journal_txt.pstring)[0], journals[journals_current].text);
        str_cpy((journal_txt_name.pstring)[0], journals[journals_current].name);
        str_cpy((journal_subtitle_txt->pstring)[0], journals[journals_current].name);
        str_cat((journal_subtitle_txt->pstring)[0], ": ");
        str_cat((journal_subtitle_txt->pstring)[0], journals[journals_current].text);
    }
    else
    {
        str_cpy((journal_txt.pstring)[0], journals[journals_current].text);
        str_cpy((journal_txt_name.pstring)[0], "");
        str_cpy((journal_subtitle_txt->pstring)[0], journals[journals_current].text);
    }

    if(journals_mediahandle != 0)
        media_stop(journals_mediahandle);

    // yay, lite-c and it's beatiful "&&" operator!
    bool good = (journals[journals_current].media != NULL);
    if(good)
        good = (strcmp(journals[journals_current].media, "") != 0);

    if(good)
    {
        journals_mediahandle = media_play(journals[journals_current].media, NULL, 100);
    }
    else
    {
        journals_mediahandle = 0;
    }
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
            journals_play(ptr->JOURNAL_ID, JOURNAL_LEVEL_DEFAULT);
            break; // can break out of the loop here
        }
	}

    if(journals_current >= 0)
    {
        show_journal();
        journal_pan->alpha = clamp(journal_pan->alpha + 5 * time_step, 0, 100);

#ifdef DEBUG
        /*
        DEBUG_VAR(journals_mediahandle, 0);
        DEBUG_VAR(media_playing(journals_mediahandle), 16);
        DEBUG_VAR(journals_timeout, 32);
        DEBUG_VAR(journals[journals_current].followup, 48);
        DEBUG_VAR(journals_current, 64);
        DEBUG_VAR(total_ticks, 72);
        */
#endif

        // Probably a bad hack, but is a nice bug mitigation:
        // media_playing returned 1 when not beeing stopped and then restarted,
        // even after completing the sound
        media_pause(journals_mediahandle);
        media_start(journals_mediahandle);

        if(!media_playing(journals_mediahandle) && (total_ticks >= journals_timeout))
        {
            function callback();
            callback = journals[journals_current].event;
            if(callback)
                callback();

            if(journals[journals_current].followup > 0)
            {
                journals_play(journals[journals_current].followup, journals_current_level);
            }
            else
            {
                journals_current = -1;
            }
        }
    }
    else
    {
        journals_current_level = -9999;
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
    journal_txt_name->alpha = journal_pan->alpha;
    journal_subtitle_txt->alpha = journal_pan->alpha;
}
