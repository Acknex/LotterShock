#ifndef JOURNALS_H
#define JOURNALS_H

#define JOURNAL_TYPE_DEFAULT 0
#define JOURNAL_TYPE_SUBTITLE 1

#define JOURNAL_LEVEL_DEFAULT 0
#define JOURNAL_LEVEL_STORY 100

void show_journal();
void hide_journal();
void journal();
void journals_update();

void journals_init();
void journals_quit();

void journals_pause();
void journals_resume();

// stops playing any journals
void journals_stop();

// plays the journal with the given ID
void journals_play(int id, int level);

typedef struct journal_t
{
	STRING* text;
	STRING* name;
	STRING* media;
    int followup; // when set to non-zero, the followup journal is played next right after the current journal entry
    void * event; // event callback is triggered when the journal is  completed
    int type;
} journal_t;

STRING* crew_name0 = "Lt. Woll";
STRING* crew_name1 = "Lt. Cmdr. Queissner";
STRING* crew_name2 = "Dr. Schwarzfeuer";
STRING* crew_name3 = "Dr. Pohl";
STRING* crew_name4 = "Cmdr. Daumann";
STRING* crew_name5 = "1st Office Truempler";
STRING* crew_name6 = "Cpt. Kappes";
STRING* crew_name7 = "Ens. Schwarz";
STRING* crew_name8 = "Ens. Jaeger";
STRING* crew_name9 = "Lt. Freiknecht";

STRING* lotter_name = "Star Lotti";
STRING* acktana_name = "Acktana";
STRING * patchnotes_name = "Unknown Voice";

#endif
