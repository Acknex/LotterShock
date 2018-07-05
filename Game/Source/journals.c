void journals_init()
{
	journals[0].text = "This is only my 3rd Tour with the Patchnotes but it doesn't seem to get any less crazy around here. The Science guys are working on some out of this world computer stuff. Don't understand a word of it. I hope I can make friends here. I miss my family.";
	journals[0].media = "Media/pda_cpt_kappes1.mp3";
}

void show_journal() {
	journal_pan.pos_x = 0;
	journal_pan.pos_y = 0;
	
	journal_txt.pos_x = journal_pan.pos_x + 100;
	journal_txt.pos_y = journal_pan.pos_y + 10;
	
	journal_txt.size_x = bmap_width(journal_pan.bmap) - 100;
	
	set(journal_pan, SHOW);
	set(journal_txt, SHOW);
}

void hide_journal() {
	reset(journal_pan, SHOW);
	reset(journal_txt, SHOW);
}

action journal() {	
	framework_setup(my, SUBSYSTEM_JOURNAL);
}

void journals_update() {
	ENTITY *ptr;
	SUBSYSTEM_LOOP(ptr, SUBSYSTEM_JOURNAL) {
		
		
		if (mouse_ent == ptr) {
			if (input_hit(INPUT_USE)) {
				
				if (is(journal_pan, SHOW) == 0) {
					if(ptr.JOURNAL_ID >= MAX_JOURNALS)
						return;
					
					str_cpy((journal_txt.pstring)[0], journals[ptr.JOURNAL_ID].text);
					media_play(journals[ptr.JOURNAL_ID].media, NULL, 100);
					
					show_journal();
					journal_delay = 400;
				}
			}
		}
		
		if (is(journal_pan, SHOW) && (journal_delay > 0)) {
			journal_delay -=1*time_step;
			if (journal_delay <= 0) {
				hide_journal();
			}
			//draw_text(str_cat("Journal show time", str_for_num(NULL, journal_delay)), 100, 10, COLOR_RED);
			
		}
	}
}