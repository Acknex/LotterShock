#ifndef JOURNALS_H
#define JOURNALS_H

#define JOURNAL_ID skill1

var journal_delay = 0;

PANEL *journal_pan = {
	bmap = "journal.tga";
	flags = OVERLAY;
	layer = 1;
}

TEXT *journal_txt = {
	flags = WWRAP;
	strings = 1;
	layer = 2;
}

void show_journal();
void hide_journal();
void journal();
void journals_update();
void journals_init();

typedef struct journal_t
{
	STRING* text;
	STRING* media;
} journal_t;

#define MAX_JOURNALS 1
journal_t journals[MAX_JOURNALS];

/*
STRING *journal_01_str = "This is only my 3rd Tour with the Patchnotes but it doesn't seem to get any less crazy around here. The Science guys are working on some out of this world computer stuff. Don't understand a word of it. I hope I can make friends here. I miss my family.";

STRING *journal_02_str = "Ok. So... you know that upper section in the mess hall? Yeah that one you can't actually get to. - WHO designs a starship like that? Why have it up there? It makes no sense!! -- Anyway, I threw my keycard up there, just for fun, I thought I could just climb up there and get it back. I couldn't. I think it might be easier if I just request a new one. I'm gonna need to get a new one anyway since my promotion.";
STRING *journal_03_str = "Shit!\n\nI lost my Keycard again! For the 2nd time this week! I accidentally dropped it into one of the garbage chutes. And by accidentally I mean accidentally. I was going to throw away a candy bar wrapper in my right hand but dropped the keycard in my left hand instead. Who knows where it ended up at. I sure as hell am not going to go look for it in Waste Recycling! Have you seen the place? I'm not even sure why they have radioactive barrels there, but no way in hell am I risking cancer for a keycard. The captain's gonna be so mad though.... I can probably kiss that promotion goodbye. sigh Back to simple Lieutenant again...";

STRING *journal_04_str = "Infusing the Donkey-Lark DNA with Nitroglycerine might have been a bad idea. These little buggers seem to have developed some... explosive tendencies. Whenever they get excited or agitated they violently explode. We've already lost 3 interns trying to feed them. I will have to look into reinforcing the cages lest they blow their way free. If they get loose, we'll have big problems on our hands.";
STRING *journal_05_str = "On second thought, maybe giving eyeballs some brains and an antigravity drive might also not have been the greatest move. But to be fair, how could we know what was going to happen. Maybe we can apply this tech for espionage or something.";
STRING *journal_06_str = "Look, I'm not going to stand around and pretend that giving those robots swords for arms was my best idea. But we can all agree on at least one thing: They look pretty, frickin' cool. And aside from that whole \"homicidal attitude problem\", they have pretty much filled every checkbox we set out for this project... Well, except for their tendency to get stuck in railings and behind chest-high obstacles. But other than that, complete success!";
STRING *journal_07_str = "My colleagues from university will be sooo jealous! I just spent the last 2 weeks re-coding the entire Öhler-Poston Engine Control System so that it can run on a Commodore 64. Yes, you heard right. Commodore. 64. Amazing, huh? Sometimes I even amaze myself! Additional plus: More computers for the digitization project! Dr. Pohl asked me where I got those computers from. I said \"It's a secret\". Between you and me? They used to run the Engine Control System. Ripped 'em right out. Put the Commodore 64 in there. Works just as well. And to think that they say I'm crazy!";

STRING *journal_08_str = "New research into the Real Life Compiler has netted us some great results. We finally cracked some of the chinese documentation on this tech. Yes we stole it, whatever. Science marches on. We think we might be able to apply the principles to an entire person!";

STRING *journal_09_str = "The guys from the engine room asked if they could store their spare batteries somewhere. \"Why not store them in the engine room?\" I asked. \"Because there is no storage room in the engine room!\" they say. Just chuck it in the corner! Why can't they just keep their engine stuff with the engines?! Anyway, the chefs said they can keep it in their storage closet. Because THAT makes sense.... Not!";

STRING *journal_10_str = "Everyone is so preoccupied with doing crazy science, they don't even notice the ship falling apart around them! This was supposed to be a state of the art vessel and now we've got exploding demon abominations running around and creepy flying eyeballs and coolant leaks all over the place! The guys in engineering promised to make me a flamethrower so we can get back into the cryo room but they had to give it to the Frankenstein scientists first to deal with their explosive monster problem. Now we have 3 dead interns and we still can't get into the meat storage. Idiots.";

STRING *journal_11_str = "This is great! Who knew that being captain of a space ship was this easy? I just hang out in the bridge all day and vaguely point in the direction I want to go and then we go there. This is awesome. I don't understand what everyone is complaining about! We are in SPACE! In. Friggin'. Space.\n\nYeah maybe we have some science experiments gone wrong, but who cares?! We'll ship them off to the military, I'm sure they can come up with some use for them.";

STRING *journal_12_str = "I asked one of the guys if they were playing some weird music at night but they denied it. Sure! Whatever! I know I can hear it. It wouldn't be so bad, if it wasn't such an earworm.";
STRING *journal_13_str = "I am SURE they are fucking with me! I stayed up all night just to check if it was them. They pretended they were sleeping and then AGAIN. That FUCKING MUSIC! They acted all surprised when I woke them up and demanded an explanation. Fuckers.";
STRING *journal_14_str = "I JUST HEARD IT AGAIN. They aren't even in the room! I fucking knew it! They put some hidden speaker somewhere to play the music at night! Frickin' hazing of the new guy! I'm gonna rip this room apart and find it!";
STRING *journal_15_str = "WHERE THE FUCK IS THIS MUSIC COMING FROM!!! I CAN'T STAND IT ANYMORE!!! IT'S DRIVING ME CRAZY! LALALALALALALA I CAN HEAR THIS MUSIC EVERYWHERE ITS IN MY DREAMS ITS ON MY MIND ALL HOURS OF THE DAY PLEASE MAKE IT STOP";
STRING *journal_16_str = "MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP MAKE IT STOP";

STRING *journal_17_str = "I fixed some of the coolant lines with duct tape. It's amazing. Even in space duct tape is still the best way to fix stuff. I even used it on some of the hull breaches. Works perfectly!";

STRING *journal_18_str = "Some idiot tried to fix the coolant lines with duct tape! DUCT TAPE! What the fuck? Who thought this was a good idea? Even worse: The hull breaches on level 2 reopened. Can you guess why? BECAUSE SOMEONE FIXED THEM WITH DUCT TAPE!!!!";
*/


#endif