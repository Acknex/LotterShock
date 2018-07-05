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
					ent_playsound(ptr, snd_beep, 100);
					
					switch(ptr.JOURNAL_ID) {
						case 1: str_cpy((journal_txt.pstring)[0], journal_01_str); break;
						case 2: str_cpy((journal_txt.pstring)[0], journal_02_str); break;
						case 3: str_cpy((journal_txt.pstring)[0], journal_03_str); break;
						case 4: str_cpy((journal_txt.pstring)[0], journal_04_str); break;
						case 5: str_cpy((journal_txt.pstring)[0], journal_05_str); break;
						case 6: str_cpy((journal_txt.pstring)[0], journal_06_str); break;
						case 7: str_cpy((journal_txt.pstring)[0], journal_07_str); break;
						case 8: str_cpy((journal_txt.pstring)[0], journal_08_str); break;
						case 9: str_cpy((journal_txt.pstring)[0], journal_09_str); break;
						case 10: str_cpy((journal_txt.pstring)[0], journal_10_str); break;
						case 11: str_cpy((journal_txt.pstring)[0], journal_11_str); break;
						case 12: str_cpy((journal_txt.pstring)[0], journal_12_str); break;
						case 13: str_cpy((journal_txt.pstring)[0], journal_13_str); break;
						case 14: str_cpy((journal_txt.pstring)[0], journal_14_str); break;
						case 15: str_cpy((journal_txt.pstring)[0], journal_15_str); break;
						case 16: str_cpy((journal_txt.pstring)[0], journal_16_str); break;
					}
					
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