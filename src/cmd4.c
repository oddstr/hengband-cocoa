/* File: cmd4.c */

/* Purpose: Interface commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

errr do_cmd_write_nikki(int type, int num, cptr note)
{
	int day, hour, min;
	FILE *fff = NULL;
	char file_name[80];
	char buf[1024];
	cptr note_level = "";
	bool do_level = TRUE;

	s32b len = 20L * TOWN_DAWN;
	s32b tick = turn % len + len / 4;

	static bool disable_nikki = FALSE;

	if (disable_nikki) return(-1);

	if ((p_ptr->prace == RACE_VAMPIRE) ||
	    (p_ptr->prace == RACE_SKELETON) ||
	    (p_ptr->prace == RACE_ZOMBIE) ||
	    (p_ptr->prace == RACE_SPECTRE))
		day = (turn - (15L * TOWN_DAWN))/ len + 1;
	else
		day = (turn + (5L * TOWN_DAWN))/ len + 1;

	hour = (24 * tick / len) % 24;
	min = (1440 * tick / len) % 60;

	if (type == NIKKI_FIX_QUEST_C ||
	    type == NIKKI_FIX_QUEST_F ||
	    type == NIKKI_RAND_QUEST_C ||
	    type == NIKKI_RAND_QUEST_F ||
	    type == NIKKI_TO_QUEST)
	{
		int old_quest;

		old_quest = p_ptr->inside_quest;
		p_ptr->inside_quest = (quest[num].type == QUEST_TYPE_RANDOM) ? 0 : num;

		/* Get the quest text */
		init_flags = INIT_ASSIGN;

		process_dungeon_file("q_info_j.txt", 0, 0, 0, 0);

		/* Reset the old quest number */
		p_ptr->inside_quest = old_quest;
	}

#ifdef JP
	sprintf(file_name,"playrecord-%s.txt",savefile_base);
#else
	/* different filne name to avoid mixing */
	sprintf(file_name,"playrec-%s.txt",savefile_base);
#endif

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, file_name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) {
#ifdef JP
		msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ������ץ쥤��Ͽ������ߤ��ޤ���", buf);
#else
		msg_format("Failed to open %s. Play-Record is disabled temporally.", buf);
#endif
		msg_format(NULL);
		disable_nikki=TRUE;
		return (-1);
	}

	if (write_level)
	{
		if (p_ptr->inside_arena)
#ifdef JP
			note_level = "���꡼��:";
#else
			note_level = "Arane:";
#endif
		else if (!dun_level)
#ifdef JP
			note_level = "�Ͼ�:";
#else
			note_level = "Surface:";
#endif
		else if (quest_number(dun_level) && ((quest_number(dun_level) < MIN_RANDOM_QUEST) && !(quest_number(dun_level) == QUEST_OBERON || quest_number(dun_level) == QUEST_SERPENT)))
#ifdef JP
			note_level = "��������:";
#else
			note_level = "Quest:";
#endif
		else
#ifdef JP
			note_level = format("%d��(%s):", dun_level, d_name+d_info[dungeon_type].name);
#else
			note_level = format("%s L%d:", d_name+d_info[dungeon_type].name, dun_level);
#endif
	}

	switch(type)
	{
		case NIKKI_HIGAWARI:
		{
#ifdef JP
			fprintf(fff, "%d����\n",day);
#else
			fprintf(fff, "Day %d\n",day);
#endif
			do_level = FALSE;
			break;
		}
		case NIKKI_BUNSHOU:
		{
			if (num)
			{
				fprintf(fff, "%s\n",note);
				do_level = FALSE;
			}
			else
				fprintf(fff, " %2d:%02d %20s %s\n",hour, min, note_level, note);
			break;
		}
		case NIKKI_ART:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s��ȯ��������\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s discover %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_UNIQUE:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s���ݤ�����\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s defeated %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_FIX_QUEST_C:
		{
			if (quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �������ȡ�%s�פ�ã��������\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s completed quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case NIKKI_FIX_QUEST_F:
		{
			if (quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �������ȡ�%s�פ���̿���餬��ƨ�����ä���\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s run away from quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case NIKKI_RAND_QUEST_C:
		{
			char name[80];
			strcpy(name, r_name+r_info[quest[num].r_idx].name);
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �����९������(%s)��ã��������\n", hour, min, note_level, name);
#else
			fprintf(fff, " %2d:%02d %20s completed randome quest '%s'\n", hour, min, note_level, name);
#endif
			break;
		}
		case NIKKI_RAND_QUEST_F:
		{
			char name[80];
			strcpy(name, r_name+r_info[quest[num].r_idx].name);
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �����९������(%s)����ƨ���Ф�����\n", hour, min, note_level, name);
#else
			fprintf(fff, " %2d:%02d %20s ran away from quest '%s'.\n", hour, min, note_level, name);
#endif
			break;
		}
		case NIKKI_MAXDEAPTH:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�κǿ���%d������ã������\n", hour, min, note_level, d_name+d_info[dungeon_type].name, num);
#else
			fprintf(fff, " %2d:%02d %20s reached level %d of %s for the first time.\n", hour, min, note_level, num, d_name+d_info[dungeon_type].name);
#endif
			break;
		}
		case NIKKI_TRUMP:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s%s�κǿ�����%d���˥��åȤ�����\n", hour, min, note_level, note, d_name + d_info[num].name, max_dlv[num]);
#else
			fprintf(fff, " %2d:%02d %20s reset recall level of %s to %d %s.\n", hour, min, note_level, d_name + d_info[num].name, max_dlv[num], note);
#endif
			break;
		}
		case NIKKI_STAIR:
		{
			cptr to;
			if (quest_number(dun_level) && ((quest_number(dun_level) < MIN_RANDOM_QUEST) && !(quest_number(dun_level) == QUEST_OBERON || quest_number(dun_level) == QUEST_SERPENT)))
			{
#ifdef JP
				to = "�Ͼ�";
#else
				to = "the surface";
#endif
			}
			else
			{
#ifdef JP
				if (!(dun_level+num)) to = "�Ͼ�";
				else to = format("%d��", dun_level+num);
#else
				if (!(dun_level+num)) to = "the surfice";
				else to = format("level %d", dun_level+num);
#endif
			}
				
#ifdef JP 
			fprintf(fff, " %2d:%02d %20s %s��%s��\n", hour, min, note_level, to, note);
#else
			fprintf(fff, " %2d:%02d %20s %s %s.\n", hour, min, note_level, note, to);
#endif
			break;
		}
		case NIKKI_RECALL:
		{
			if (!num)
#ifdef JP
				fprintf(fff, " %2d:%02d %20s ���Ԥ�Ȥä�%s��%d���ز��ꤿ��\n", hour, min, note_level, d_name+d_info[dungeon_type].name, max_dlv[dungeon_type]);
#else
				fprintf(fff, " %2d:%02d %20s recall to dungeon level %d of %s.\n", hour, min, note_level, max_dlv[dungeon_type], d_name+d_info[dungeon_type].name);
#endif
			else
#ifdef JP
				fprintf(fff, " %2d:%02d %20s ���Ԥ�Ȥä��Ͼ�ؤ���ä���\n", hour, min, note_level);
#else
				fprintf(fff, " %2d:%02d %20s recall from dungeon to surface.\n", hour, min, note_level);
#endif
			break;
		}
		case NIKKI_TO_QUEST:
		{
			if (quest[num].flags & QUEST_FLAG_SILENT) break;
#ifdef JP
			fprintf(fff, " %2d:%02d %20s �������ȡ�%s�פؤ�����������\n", hour, min, note_level, quest[num].name);
#else
			fprintf(fff, " %2d:%02d %20s enter quest '%s'.\n", hour, min, note_level, quest[num].name);
#endif
			break;
		}
		case NIKKI_TELE_LEV:
		{
			cptr to;
			if (!dun_level)
			{
#ifdef JP
				to = "1��";
#else
				to = "level 1";
#endif
			}
			else if (quest_number(dun_level) && ((quest_number(dun_level) < MIN_RANDOM_QUEST) && !(quest_number(dun_level) == QUEST_OBERON || quest_number(dun_level) == QUEST_SERPENT)))
			{
#ifdef JP
				to = "�Ͼ�";
#else
				to = "the surface";
#endif
			}
			else
			{
#ifdef JP
				if (!(dun_level+num)) to = "�Ͼ�";
				else to = format("%d��", dun_level+num);
#else
				if (!(dun_level+num)) to = "surface";
				else to = format("level %d", dun_level+num);
#endif
			}
				
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�ؤȥƥ�ݡ��Ȥǰ�ư������\n", hour, min, note_level, to);
#else
			fprintf(fff, " %2d:%02d %20s teleport level to %s.\n", hour, min, note_level, to);
#endif
			break;
		}
		case NIKKI_BUY:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�����������\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s buy %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_SELL:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s����Ѥ�����\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s sell %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_ARENA:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s Ʈ�����%d����(%s)�˾���������\n", hour, min, note_level, num, note);
#else
			fprintf(fff, " %2d:%02d %20s win the %d%s fight (%s).\n", hour, min, note_level, num, (num%10==1?"st":num%10==2?"nd":num%10==3?"rd":"th"), note);
#endif
			if (num == MAX_ARENA_MONS)
			{
#ifdef JP
				fprintf(fff, "                 Ʈ����Τ��٤Ƥ�Ũ�˾������������ԥ���Ȥʤä���\n");
#else
				fprintf(fff, "                 win all fight to become a Chanpion.\n");
#endif
				do_level = FALSE;
			}
			break;
		}
		case NIKKI_HANMEI:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s���̤�����\n", hour, min, note_level, note);
#else
			fprintf(fff, " %2d:%02d %20s identify %s.\n", hour, min, note_level, note);
#endif
			break;
		}
		case NIKKI_WIZ_TELE:
		{
			cptr to;
			if (!dun_level)
#ifdef JP
				to = "�Ͼ�";
#else
				to = "the surface";
#endif
			else
#ifdef JP
				to = format("%d��(%s)", dun_level, d_name+d_info[dungeon_type].name);
#else
				to = format("level %d of %s", dun_level, d_name+d_info[dungeon_type].name);
#endif
				
#ifdef JP
			fprintf(fff, " %2d:%02d %20s %s�ؤȥ��������ɡ��ƥ�ݡ��Ȥǰ�ư������\n", hour, min, note_level, to);
#else
			fprintf(fff, " %2d:%02d %20s wizard-teleport to %s.\n", hour, min, note_level, to);
#endif
			break;
		}
		case NIKKI_LEVELUP:
		{
#ifdef JP
			fprintf(fff, " %2d:%02d %20s ��٥뤬%d�˾夬�ä���\n", hour, min, note_level, num);
#else
			fprintf(fff, " %2d:%02d %20s reach player level %d.\n", hour, min, note_level, num);
#endif
			break;
		}
		case NIKKI_GAMESTART:
		{
			time_t ct = time((time_t*)0);
			do_level = FALSE;
			if (num)
			{
				fprintf(fff, "%s %s",note, ctime(&ct));
			}
			else
				fprintf(fff, " %2d:%02d %20s %s %s",hour, min, note_level, note, ctime(&ct));
			break;
		}
		case NIKKI_NAMED_PET:
		{
			fprintf(fff, " %2d:%02d %20s ", hour, min, note_level);
			switch (num)
			{
				case 0:
#ifdef JP
					fprintf(fff, "%s��ι��ͧ�ˤ��뤳�Ȥ˷�᤿��\n", note);
#else
					fprintf(fff, "decide to travel together with %s.\n", note);
#endif
					break;
				case 1:
#ifdef JP
					fprintf(fff, "%s��̾����ä�����\n", note);
#else
					fprintf(fff, "unname %s.\n", note);
#endif
					break;
				case 2:
#ifdef JP
					fprintf(fff, "%s�����������\n", note);
#else
					fprintf(fff, "dismiss %s.\n", note);
#endif
					break;
				case 3:
#ifdef JP
					fprintf(fff, "%s�����Ǥ��ޤä���\n", note);
#else
					fprintf(fff, "%s die.\n", note);
#endif
					break;
				case 4:
#ifdef JP
					fprintf(fff, "%s�򤪤����̤Υޥåפذ�ư������\n", note);
#else
					fprintf(fff, "move to other map leaving %s behind.\n", note);
#endif
					break;
				case 5:
#ifdef JP
					fprintf(fff, "%s�ȤϤ���Ƥ��ޤä���\n", note);
#else
					fprintf(fff, "lose sight of %s.\n", note);
#endif
					break;
				case 6:
#ifdef JP
					fprintf(fff, "%s��*�˲�*�ˤ�äƾä���ä���\n", note);
#else
					fprintf(fff, "%s is made disappeared by *destruction*.\n", note);
#endif
					break;
				case 7:
#ifdef JP
					fprintf(fff, "%s�����Ф˲����٤��줿��\n", note);
#else
					fprintf(fff, "%s is crushed by falling rocks.\n", note);
#endif
					break;
				default:
					fprintf(fff, "\n");
					break;
			}
			break;
		}
		default:
			break;
	}

	my_fclose(fff);

	if (do_level) write_level = FALSE;

	return (0);
}


#define MAX_SUBTITLE (sizeof(subtitle)/sizeof(subtitle[0]))

static void do_cmd_disp_nikki(void)
{
	char nikki_title[256];
	char file_name[80];
	char buf[1024];
	char tmp[80];
#ifdef JP
	static const char subtitle[][30] = {"�Ƕ������Τ����",
					   "��������ϤϤ��ʤ�",
					   "�����˸����ä�",
					   "ê����ܤ����",
					   "���Ȥκפ�",
					   "����Ϥ����ͤ���",
					   "���ȤǤ����",
					   "�Ƥˤ�Ѥˤ�",
					   "����������",
					   "��Ϥ䤳��ޤ�",
					   "�ʤ�Ǥ����ʤ��",
					   "�����̵����",
					   "�ݤ��٤�Ũ�ϥ�����",
					   "�����ʹ������ʤ�",
					   "�����̾����äƤߤ�",
					   "Ƭ���Ѥˤʤä���ä�",
					   "�ߴ����ޤ���",
					   "���ä���������",
					   "�ޤ��ޤ��Ť���",
					   "�ऴ���ऴ������",
					   "����ʤ�󤸤�ʤ�",
					   "����������",
					   "�����äƤߤ褦",
					   "����äȤ�����",
					   "������������",
					   "��˾�β̤�",
					   "̵���Ϲ�",
					   "���˷��ޤ�����",
					   "̤�Τ�������",
					   "�ǹ��ƬǾ�����"};
#else
	static const char subtitle[][51] ={"Quest of The World's Toughest Body",
					   "Attack is the best form of defence.",
					   "Might is right.",
					   "An unexpected windfall",
					   "A drowning man will catch at a straw",
					   "Don't count your chickens before they are hatched.",
					   "It is no use crying over spilt milk.",
					   "Seeing is believing.",
					   "Strike the iron while it is hot.",
					   "I don't care what follows.",
					   "To dig a well to put out a house on fire.",
					   "Tomorrow is another day.",
					   "Easy come, easy go.",
					   "The more haste, the less speed.",
					   "Where there is life, there is hope.",
					   "There is no royal road to *WINNER*.",
					   "Danger past, God forgotten.",
					   "The best thing to do now is to run away.",
					   "Life is but an empty dream.",
					   "Dead men tell no tales.",
					   "A book that remains shut is but a block.",
					   "Misfortunes never come singly.",
					   "A little knowledge is a dangerous thing.",
					   "History repeats itself.",
					   "*WINNER* was not built in a day.",
					   "Ignorance is bliss.",
					   "To lose is to win?",
					   "No medicine can cure folly.",
					   "All good things come to an end.",
					   "M$ Empire strikes back.",
					   "To see is to believe",
					   "Time is money.",
					   "Quest of The World's Greatest Brain"};
#endif
#ifdef JP
	sprintf(file_name,"playrecord-%s.txt",savefile_base);
#else
	sprintf(file_name,"playrec-%s.txt",savefile_base);
#endif

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, file_name);

	if (p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_MONK || p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_BERSERKER)
		strcpy(tmp,subtitle[rand_int(MAX_SUBTITLE-1)]);
	else if (p_ptr->pclass == CLASS_MAGE || p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER)
		strcpy(tmp,subtitle[rand_int(MAX_SUBTITLE-1)+1]);
	else strcpy(tmp,subtitle[rand_int(MAX_SUBTITLE-2)+1]);

#ifdef JP
	sprintf(nikki_title, "��%s%s%s������ -%s-��",
		ap_ptr->title, ap_ptr->no ? "��" : "", player_name, tmp);
#else
	sprintf(nikki_title, "Legend of %s %s '%s'",
		ap_ptr->title, player_name, tmp);
#endif

	/* Display the file contents */
	show_file(FALSE, buf, nikki_title, -1, 0);
}

static void do_cmd_bunshou(void)
{
	char tmp[80] = "\0";
	char bunshou[80] = "\0";

#ifdef JP
	if (get_string("����: ", tmp, 79))
#else
	if (get_string("diary note: ", tmp, 79))
#endif
	{
		strcpy(bunshou, tmp);

		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, bunshou);
	}
}

static void do_cmd_last_get(void)
{
	char buf[256];
	s32b turn_tmp;

	if (record_o_name[0] == '\0') return;

#ifdef JP
	sprintf(buf,"%s�������Ͽ���ޤ���",record_o_name);
#else
	sprintf(buf,"Do you really want to record getting %s? ",record_o_name);
#endif
	if (!get_check(buf)) return;

	turn_tmp = turn;
	turn = record_turn;
#ifdef JP
	sprintf(buf,"%s�������줿��", record_o_name);
#else
	sprintf(buf,"descover %s.", record_o_name);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 0, buf);
	turn = turn_tmp;
}

static void do_cmd_erase_nikki(void)
{
	char file_name[80];
	char buf[256];
	FILE *fff = NULL;

#ifdef JP
	if (!get_check("�����˵�Ͽ��õ�ޤ�����")) return;
#else
	if (!get_check("Do you really want to delete all your record? ")) return;
#endif

#ifdef JP
	sprintf(file_name,"playrecord-%s.txt",savefile_base);
#else
	sprintf(file_name,"playrec-%s.txt",savefile_base);
#endif

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, file_name);

	/* Remove the file */
	fd_kill(buf);

	fff = my_fopen(buf, "w");
	if(fff){
		my_fclose(fff);
#ifdef JP
		msg_format("��Ͽ��õ�ޤ�����");
#else
		msg_format("deleted record.");
#endif
	}else{
#ifdef JP
		msg_format("%s �ξõ�˼��Ԥ��ޤ�����", buf);
#else
		msg_format("failed to delete %s.", buf);
#endif
	}
	msg_print(NULL);
}

#if 0
void do_debug(void)
{
	msg_format("%d %d %d:%d",py,px, p_ptr->energy, p_ptr->skill_dis);
	msg_print(NULL);
	battle_monsters();
}
#endif

void do_cmd_nikki(void)
{
	int i;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("[ ��Ͽ������ ]", 2, 0);
#else
		prt("[ Play Record ]", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1) ��Ͽ�򸫤�", 4, 5);
		prt("(2) ʸ�Ϥ�Ͽ����", 5, 5);
		prt("(3) ľ�����������ϴ��ꤷ����Τ�Ͽ����", 6, 5);
		prt("(4) ��Ͽ��õ��", 7, 5);
#else
		prt("(1) Display your record", 4, 5);
		prt("(2) Add record", 5, 5);
		prt("(3) Record item you last get/identify", 6, 5);
		prt("(4) Delete your record", 7, 5);
#endif


		/* Prompt */
#ifdef JP
		prt("���ޥ��:", 18, 0);
#else
		prt("Command: ", 18, 0);
#endif


		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		switch (i)
		{
		case '1':
			do_cmd_disp_nikki();
			break;
		case '2':
			do_cmd_bunshou();
			break;
		case '3':
			do_cmd_last_get();
			break;
		case '4':
			do_cmd_erase_nikki();
			break;
#if 0
		case ':':
			do_debug();
			break;
#endif
		default: /* Unknown option */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();
}

/*
 * Hack -- redraw the screen
 *
 * This command performs various low level updates, clears all the "extra"
 * windows, does a total redraw of the main window, and requests all of the
 * interesting updates and redraws that I can think of.
 *
 * This command is also used to "instantiate" the results of the user
 * selecting various things, such as graphics mode, so it must call
 * the "TERM_XTRA_REACT" hook before redrawing the windows.
 */
void do_cmd_redraw(void)
{
	int j;

	term *old = Term;


	/* Hack -- react to changes */
	Term_xtra(TERM_XTRA_REACT, 0);


	/* Combine and Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);


	/* Update torch */
	p_ptr->update |= (PU_TORCH);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Forget lite/view */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	update_playtime();

	/* Hack -- update */
	handle_stuff();

	if (p_ptr->prace == RACE_ANDROID) calc_android_exp();


	/* Redraw every window */
	for (j = 0; j < 8; j++)
	{
		/* Dead window */
		if (!angband_term[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw */
		Term_redraw();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- change name
 */
void do_cmd_change_name(void)
{
	char	c;

	int		mode = 0;

	char	tmp[160];


	/* Save the screen */
	screen_save();

	/* Forever */
	while (1)
	{
		update_playtime();

		/* Display the player */
		display_player(mode);

		if (mode == 6)
		{
			mode = 0;
			display_player(mode);
		}

		/* Prompt */
#ifdef JP
		Term_putstr(2, 23, -1, TERM_WHITE,
			    "['c'��̾���ѹ�, 'f'�ǥե�����ؽ��, 'h'�ǥ⡼���ѹ�, ESC�ǽ�λ]");
#else
		Term_putstr(2, 23, -1, TERM_WHITE,
			"['c' to change name, 'f' to file, 'h' to change mode, or ESC]");
#endif


		/* Query */
		c = inkey();

		/* Exit */
		if (c == ESCAPE) break;

		/* Change name */
		if (c == 'c')
		{
			get_name();
		}

		/* File dump */
		else if (c == 'f')
		{
			sprintf(tmp, "%s.txt", player_base);
#ifdef JP
			if (get_string("�ե�����̾: ", tmp, 80))
#else
			if (get_string("File name: ", tmp, 80))
#endif

			{
				if (tmp[0] && (tmp[0] != ' '))
				{
					file_character(tmp, TRUE);
				}
			}
		}

		/* Toggle mode */
		else if (c == 'h')
		{
			mode++;
		}

		/* Oops */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();

	/* Redraw everything */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_MAP | PR_EQUIPPY);

	handle_stuff();
}


/*
 * Recall the most recent message
 */
void do_cmd_message_one(void)
{
	/* Recall one message XXX XXX XXX */
	prt(format("> %s", message_str(0)), 0, 0);
}


/*
 * Show previous messages to the user	-BEN-
 *
 * The screen format uses line 0 and 23 for headers and prompts,
 * skips line 1 and 22, and uses line 2 thru 21 for old messages.
 *
 * This command shows you which commands you are viewing, and allows
 * you to "search" for strings in the recall.
 *
 * Note that messages may be longer than 80 characters, but they are
 * displayed using "infinite" length, with a special sub-command to
 * "slide" the virtual display to the left or right.
 *
 * Attempt to only hilite the matching portions of the string.
 */
void do_cmd_messages(int num_now)
{
	int i, j, k, n;
	uint q;

	char shower[80];
	char finder[80];


	/* Wipe finder */
	strcpy(finder, "");

	/* Wipe shower */
	strcpy(shower, "");


	/* Total messages */
	n = message_num();

	/* Start on first message */
	i = 0;

	/* Start at leftmost edge */
	q = 0;

	/* Save the screen */
	screen_save();

	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Dump up to 20 lines of messages */
		for (j = 0; (j < 20) && (i + j < n); j++)
		{
			cptr msg = message_str(i+j);

			/* Apply horizontal scroll */
			msg = (strlen(msg) >= q) ? (msg + q) : "";

			/* Dump the messages, bottom to top */
			Term_putstr(0, 21-j, -1, (i+j < num_now ? TERM_WHITE : TERM_SLATE), msg);

			/* Hilite "shower" */
			if (shower[0])
			{
				cptr str = msg;

				/* Display matches */
				while ((str = strstr(str, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(str-msg, 21-j, len, TERM_YELLOW, shower);

					/* Advance */
					str += len;
				}
			}
		}

		/* Display header XXX XXX XXX */
#ifdef JP
		/* translation */
		prt(format("�����Υ�å����� %d-%d ������(%d) ���ե��å�(%d)",
		           i, i+j-1, n, q), 0, 0);
#else
		prt(format("Message Recall (%d-%d of %d), Offset %d",
		    i, i+j-1, n, q), 0, 0);
#endif


		/* Display prompt (not very informative) */
#ifdef JP
		prt("[ 'p' �ǹ��˸Ť����, 'n' �ǹ��˿��������, '/' �Ǹ���, ESC ������ ]", 23, 0);
#else
		prt("[Press 'p' for older, 'n' for newer, ..., or ESCAPE]", 23, 0);
#endif


		/* Get a command */
		k = inkey();

		/* Exit on Escape */
		if (k == ESCAPE) break;

		/* Hack -- Save the old index */
		j = i;

		/* Horizontal scroll */
		if (k == '4')
		{
			/* Scroll left */
			q = (q >= 40) ? (q - 40) : 0;

			/* Success */
			continue;
		}

		/* Horizontal scroll */
		if (k == '6')
		{
			/* Scroll right */
			q = q + 40;

			/* Success */
			continue;
		}

		/* Hack -- handle show */
		if (k == '=')
		{
			/* Prompt */
#ifdef JP
			prt("��Ĵ: ", 23, 0);
#else
			prt("Show: ", 23, 0);
#endif


			/* Get a "shower" string, or continue */
			if (!askfor_aux(shower, 80)) continue;

			/* Okay */
			continue;
		}

		/* Hack -- handle find */
		if (k == '/')
		{
			int z;

			/* Prompt */
#ifdef JP
			prt("����: ", 23, 0);
#else
			prt("Find: ", 23, 0);
#endif


			/* Get a "finder" string, or continue */
			if (!askfor_aux(finder, 80)) continue;

			/* Show it */
			strcpy(shower, finder);

			/* Scan messages */
			for (z = i + 1; z < n; z++)
			{
				cptr msg = message_str(z);

				/* Search for it */
				if (strstr(msg, finder))
				{
					/* New location */
					i = z;

					/* Done */
					break;
				}
			}
		}

		/* Recall 1 older message */
		if ((k == '8') || (k == '\n') || (k == '\r'))
		{
			/* Go newer if legal */
			if (i + 1 < n) i += 1;
		}

		/* Recall 10 older messages */
		if (k == '+')
		{
			/* Go older if legal */
			if (i + 10 < n) i += 10;
		}

		/* Recall 20 older messages */
		if ((k == 'p') || (k == KTRL('P')) || (k == ' '))
		{
			/* Go older if legal */
			if (i + 20 < n) i += 20;
		}

		/* Recall 20 newer messages */
		if ((k == 'n') || (k == KTRL('N')))
		{
			/* Go newer (if able) */
			i = (i >= 20) ? (i - 20) : 0;
		}

		/* Recall 10 newer messages */
		if (k == '-')
		{
			/* Go newer (if able) */
			i = (i >= 20) ? (i - 20) : 0;
		}

		/* Recall 1 newer messages */
		if (k == '2')
		{
			/* Go newer (if able) */
			i = (i >= 1) ? (i - 1) : 0;
		}

		/* Hack -- Error of some kind */
		if (i == j) bell();
	}

	/* Restore the screen */
	screen_load();
}



/*
 * Number of cheating options
 */
#define CHEAT_MAX 6

/*
 * Cheating options
 */
static option_type cheat_info[CHEAT_MAX] =
{
	{ &cheat_peek,		FALSE,	255,	0x01, 0x00,
#ifdef JP
	"cheat_peek",		"�����ƥ��������Τ�������" },
#else
	"cheat_peek",		"Peek into object creation" },
#endif


	{ &cheat_hear,		FALSE,	255,	0x02, 0x00,
#ifdef JP
	"cheat_hear",		"��󥹥�����������Τ�������" },
#else
	"cheat_hear",		"Peek into monster creation" },
#endif


	{ &cheat_room,		FALSE,	255,	0x04, 0x00,
#ifdef JP
	"cheat_room",		"���󥸥���������Τ�������" },
#else
	"cheat_room",		"Peek into dungeon creation" },
#endif


	{ &cheat_xtra,		FALSE,	255,	0x08, 0x00,
#ifdef JP
	"cheat_xtra",		"����¾�λ���Τ�������" },
#else
	"cheat_xtra",		"Peek into something else" },
#endif


	{ &cheat_know,		FALSE,	255,	0x10, 0x00,
#ifdef JP
	"cheat_know",		"�����ʥ�󥹥����λפ��Ф��Τ�" },
#else
	"cheat_know",		"Know complete monster info" },
#endif


	{ &cheat_live,		FALSE,	255,	0x20, 0x00,
#ifdef JP
	"cheat_live",		"�����򤹤뤳�Ȥ��ǽ�ˤ���" }
#else
	"cheat_live",		"Allow player to avoid death" }
#endif

};

/*
 * Interact with some options for cheating
 */
static void do_cmd_options_cheat(cptr info)
{
	char	ch;

	int		i, k = 0, n = CHEAT_MAX;

	char	buf[80];


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( �꥿����Ǽ���, y/n �ǥ��å�, ESC �Ƿ��� )", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

#ifdef JP
		/* �������ץ����򤦤ä��ꤤ���äƤ��ޤ��ͤ�����褦�ʤΤ���� */
		prt("                                 <<  ���  >>", 11, 0);
		prt("      �������ץ�������٤Ǥ����ꤹ��ȡ���������Ͽ���Ĥ�ʤ��ʤ�ޤ���", 12, 0);
		prt("      ��˲�����Ƥ����Ǥ��Τǡ������Ԥ��ܻؤ����Ϥ����Υ��ץ����Ϥ�", 13, 0);
		prt("      ����ʤ��褦�ˤ��Ʋ�������", 14, 0);
#endif
		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%s)",
			    cheat_info[i].o_desc,
#ifdef JP
			    (*cheat_info[i].o_var ? "�Ϥ�  " : "������"),
#else
			    (*cheat_info[i].o_var ? "yes" : "no "),
#endif

			    cheat_info[i].o_text);
			c_prt(a, buf, i + 2, 0);
		}

		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{
				if(!noscore)
#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�������ץ�����ON�ˤ��ơ���������Ĥ��ʤ��ʤä���");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use cheating options.");
#endif
				noscore |= (cheat_info[k].o_set * 256 + cheat_info[k].o_bit);
				(*cheat_info[k].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				(*cheat_info[k].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


static option_type autosave_info[2] =
{
	{ &autosave_l,      FALSE, 255, 0x01, 0x00,
#ifdef JP
	    "autosave_l",    "���������������٤˼�ư�����֤���" },
#else
	    "autosave_l",    "Autosave when entering new levels" },
#endif


	{ &autosave_t,      FALSE, 255, 0x02, 0x00,
#ifdef JP
	    "autosave_t",   "���꥿������˼�ư�����֤���" },
#else
	    "autosave_t",   "Timed autosave" },
#endif

};


static s16b toggle_frequency(s16b current)
{
	if (current == 0) return 50;
	if (current == 50) return 100;
	if (current == 100) return 250;
	if (current == 250) return 500;
	if (current == 500) return 1000;
	if (current == 1000) return 2500;
	if (current == 2500) return 5000;
	if (current == 5000) return 10000;
	if (current == 10000) return 25000;

	return 0;
}


/*
 * Interact with some options for cheating
 */
static void do_cmd_options_autosave(cptr info)
{
	char	ch;

	int     i, k = 0, n = 2;

	char	buf[80];


	/* Clear screen */
	Term_clear();

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( �꥿����Ǽ���, y/n �ǥ��å�, F �����٤�����, ESC �Ƿ��� ) ", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, 'F' for frequency, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%s)",
			    autosave_info[i].o_desc,
#ifdef JP
			    (*autosave_info[i].o_var ? "�Ϥ�  " : "������"),
#else
			    (*autosave_info[i].o_var ? "yes" : "no "),
#endif

			    autosave_info[i].o_text);
			c_prt(a, buf, i + 2, 0);
		}

#ifdef JP
		prt(format("��ư�����֤����١� %d ��������",  autosave_freq), 5, 0);
#else
		prt(format("Timed autosave frequency: every %d turns",  autosave_freq), 5, 0);
#endif



		/* Hilite current option */
		move_cursor(k + 2, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			{

				(*autosave_info[k].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			{
				(*autosave_info[k].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case 'f':
			case 'F':
			{
				autosave_freq = toggle_frequency(autosave_freq);
#ifdef JP
				prt(format("��ư�����֤����١� %d ��������", 
#else
				prt(format("Timed autosave frequency: every %d turns",
#endif

				    autosave_freq), 5, 0);
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


#define PAGE_AUTODESTROY 7

/*
 * Interact with some options
 */
void do_cmd_options_aux(int page, cptr info)
{
	char    ch;
	int     i, k = 0, n = 0, l;
	int     opt[24];
	char    buf[80];


	/* Lookup the options */
	for (i = 0; i < 24; i++) opt[i] = 0;

	/* Scan the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		/* Notice options on this "page" */
		if (option_info[i].o_page == page) opt[n++] = i;
	}


	/* Clear screen */
	Term_clear();

#ifdef JP
	if (page == PAGE_AUTODESTROY) c_prt(TERM_YELLOW, "�ʲ��Υ��ץ����ϡ���ư�˲�����Ѥ���Ȥ��Τ�ͭ��", 7, 6);
#else
	if (page == PAGE_AUTODESTROY) c_prt(TERM_YELLOW, "Following options will protect items from easy auto-destroyer.", 7, 3);
#endif

	/* Interact with the player */
	while (TRUE)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		sprintf(buf, "%s ( �꥿����Ǽ���, y/n �ǥ��å�, ESC �Ƿ��� ) ", info);
#else
		sprintf(buf, "%s (RET to advance, y/n to set, ESC to accept) ", info);
#endif

		prt(buf, 0, 0);

		/* Display the options */
		for (i = 0; i < n; i++)
		{
			byte a = TERM_WHITE;

			/* Color current option */
			if (i == k) a = TERM_L_BLUE;

			/* Display the option text */
			sprintf(buf, "%-48s: %s (%.19s)",
			        option_info[opt[i]].o_desc,
#ifdef JP
			        (*option_info[opt[i]].o_var ? "�Ϥ�  " : "������"),
#else
			        (*option_info[opt[i]].o_var ? "yes" : "no "),
#endif

			        option_info[opt[i]].o_text);
			if ((page == PAGE_AUTODESTROY) && i > 3) c_prt(a, buf, i + 5, 0);
			else c_prt(a, buf, i + 2, 0);
		}

		if ((page == PAGE_AUTODESTROY) && (k > 3)) l = 3;
		else l = 0;
		/* Hilite current option */
		move_cursor(k + 2 + l, 50);

		/* Get a key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				return;
			}

			case '-':
			case '8':
			case 'k':
			case 'K':
			{
				k = (n + k - 1) % n;
				break;
			}

			case ' ':
			case '\n':
			case '\r':
			case '2':
			case 'j':
			case 'J':
			{
				k = (k + 1) % n;
				break;
			}

			case 'y':
			case 'Y':
			case '6':
			case 'l':
			case 'L':
			{
				(*option_info[opt[k]].o_var) = TRUE;
				k = (k + 1) % n;
				break;
			}

			case 'n':
			case 'N':
			case '4':
			case 'h':
			case 'H':
			{
				(*option_info[opt[k]].o_var) = FALSE;
				k = (k + 1) % n;
				break;
			}

			case 't':
			case 'T':
			{
				(*option_info[opt[k]].o_var) = !(*option_info[opt[k]].o_var);
				break;
			}

			default:
			{
				bell();
				break;
			}
		}
	}
}


/*
 * Modify the "window" options
 */
static void do_cmd_options_win(void)
{
	int i, j, d;

	int y = 0;
	int x = 0;

	char ch;

	bool go = TRUE;

	u32b old_flag[8];


	/* Memorize old flags */
	for (j = 0; j < 8; j++)
	{
		/* Acquire current flags */
		old_flag[j] = window_flag[j];
	}


	/* Clear screen */
	Term_clear();

	/* Interact */
	while (go)
	{
		/* Prompt XXX XXX XXX */
#ifdef JP
		prt("������ɥ����ե饰 (<����>�ǰ�ư, t�ǥ�����, y/n �ǥ��å�, ESC)", 0, 0);
#else
		prt("Window Flags (<dir>, t, y, n, ESC) ", 0, 0);
#endif


		/* Display the windows */
		for (j = 0; j < 8; j++)
		{
			byte a = TERM_WHITE;

			cptr s = angband_term_name[j];

			/* Use color */
			if (j == x) a = TERM_L_BLUE;

			/* Window name, staggered, centered */
			Term_putstr(35 + j * 5 - strlen(s) / 2, 2 + j % 2, -1, a, s);
		}

		/* Display the options */
		for (i = 0; i < 16; i++)
		{
			byte a = TERM_WHITE;

			cptr str = window_flag_desc[i];

			/* Use color */
			if (i == y) a = TERM_L_BLUE;

			/* Unused option */
#ifdef JP
			if (!str) str = "(̤����)";
#else
			if (!str) str = "(Unused option)";
#endif


			/* Flag name */
			Term_putstr(0, i + 5, -1, a, str);

			/* Display the windows */
			for (j = 0; j < 8; j++)
			{
				byte a = TERM_WHITE;

				char c = '.';

				/* Use color */
				if ((i == y) && (j == x)) a = TERM_L_BLUE;

				/* Active flag */
				if (window_flag[j] & (1L << i)) c = 'X';

				/* Flag value */
				Term_putch(35 + j * 5, i + 5, a, c);
			}
		}

		/* Place Cursor */
		Term_gotoxy(35 + x * 5, y + 5);

		/* Get key */
		ch = inkey();

		/* Analyze */
		switch (ch)
		{
			case ESCAPE:
			{
				go = FALSE;
				break;
			}

			case 'T':
			case 't':
			{
				/* Clear windows */
				for (j = 0; j < 8; j++)
				{
					window_flag[j] &= ~(1L << y);
				}

				/* Clear flags */
				for (i = 0; i < 16; i++)
				{
					window_flag[x] &= ~(1L << i);
				}

				/* Fall through */
			}

			case 'y':
			case 'Y':
			{
				/* Ignore screen */
				if (x == 0) break;

				/* Set flag */
				window_flag[x] |= (1L << y);
				break;
			}

			case 'n':
			case 'N':
			{
				/* Clear flag */
				window_flag[x] &= ~(1L << y);
				break;
			}

			default:
			{
				d = get_keymap_dir(ch);

				x = (x + ddx[d] + 8) % 8;
				y = (y + ddy[d] + 16) % 16;

				if (!d) bell();
			}
		}
	}

	/* Notice changes */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* Dead window */
		if (!angband_term[j]) continue;

		/* Ignore non-changes */
		if (window_flag[j] == old_flag[j]) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Erase */
		Term_clear();

		/* Refresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}




/*
 * Set or unset various options.
 *
 * The user must use the "Ctrl-R" command to "adapt" to changes
 * in any options which control "visual" aspects of the game.
 */
void do_cmd_options(void)
{
	int k;


	/* Save the screen */
	screen_save();


	/* Interact */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Why are we here */
#ifdef JP
		prt("[ ���ץ��������� ]", 2, 0);
#else
		prt("Options", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1)     ��������          ���ץ����", 4, 5);
		prt("(2)     ���̽���          ���ץ����", 5, 5);
		prt("(3)   ������ץ쥤        ���ץ����", 6, 5);
		prt("(4)   ��ư��ߴط�        ���ץ����", 7, 5);
		prt("(5)      ��Ψ��           ���ץ����", 8, 5);
		prt("(6) �����ƥ༫ư����/�˲� ���ץ����", 9, 5);
		prt("(R)    �ץ쥤��Ͽ         ���ץ����", 10, 5);
		/* Special choices */
		prt("(D)  ���ܥ���������", 12, 5);
		prt("(H) ��ҥåȥݥ���ȷٹ�", 13, 5);
		prt("(A)    ��ư������         ���ץ����", 14, 5);
		/* Window flags */
		prt("(W) ������ɥ��ե饰", 15, 5);
		/* Cheating */
		prt("(C)       ����            ���ץ����", 16, 5);
#else
		prt("(1) Input Options", 4, 5);
		prt("(2) Output Options", 5, 5);
		prt("(3) Game-Play Options", 6, 5);
		prt("(4) Disturbance Options", 7, 5);
		prt("(5) Efficiency Options", 8, 5);
		prt("(6) Object Auto-Pick/Destroy Options", 9, 5);
		prt("(R) Play-record Options", 10, 5);

		/* Special choices */
		prt("(D) Base Delay Factor", 12, 5);
		prt("(H) Hitpoint Warning", 13, 5);
		prt("(A) Autosave Options", 14, 5);


		/* Window flags */
		prt("(W) Window Flags", 15, 5);

		/* Cheating */
		prt("(C) Cheating Options", 16, 5);
#endif


		/* Prompt */
#ifdef JP
		prt("���ޥ��:", 18, 0);
#else
		prt("Command: ", 18, 0);
#endif


		/* Get command */
		k = inkey();

		/* Exit */
		if (k == ESCAPE) break;

		/* Analyze */
		switch (k)
		{
			/* General Options */
			case '1':
			{
				/* Process the general options */
#ifdef JP
				do_cmd_options_aux(1, "�������ϥ��ץ����");
#else
				do_cmd_options_aux(1, "Input Options");
#endif

				break;
			}

			/* General Options */
			case '2':
			{
				/* Process the general options */
#ifdef JP
				do_cmd_options_aux(2, "���̽��ϥ��ץ����");
#else
				do_cmd_options_aux(2, "Output Options");
#endif

				break;
			}

			/* Inventory Options */
			case '3':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(3, "������ץ쥤�����ץ����");
#else
				do_cmd_options_aux(3, "Game-Play Options");
#endif

				break;
			}

			/* Disturbance Options */
			case '4':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(4, "��ư��ߴط��Υ��ץ����");
#else
				do_cmd_options_aux(4, "Disturbance Options");
#endif

				break;
			}

			/* Efficiency Options */
			case '5':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_aux(5, "��Ψ�����ץ����");
#else
				do_cmd_options_aux(5, "Efficiency Options");
#endif

				break;
			}

                        /* Object auto-destruction Options */
                        case '6':
                        {
                                /* Spawn */
#ifdef JP
                                do_cmd_options_aux(7, "�����ƥ༫ư����/�˲����ץ����");
#else
                                do_cmd_options_aux(7, "Object Auto-Pick/Destroy Options");
#endif
                                break;
                        }

                        /* Play-record Options */
                        case 'R':
                        case 'r':
                        {
                                /* Spawn */
#ifdef JP
                                do_cmd_options_aux(10, "�ץ쥤��Ͽ���ץ����");
#else
                                do_cmd_options_aux(10, "Play-record Option");
#endif
                                break;
                        }

			/* Cheating Options */
			case 'C':
			{
				/* Spawn */
#ifdef JP
				do_cmd_options_cheat("�����դϷ褷�ƾ����Ǥ��ʤ���");
#else
				do_cmd_options_cheat("Cheaters never win");
#endif

				break;
			}

			case 'a':
			case 'A':
			{
#ifdef JP
				do_cmd_options_autosave("��ư������");
#else
				do_cmd_options_autosave("Autosave");
#endif

				break;
			}

			/* Window flags */
			case 'W':
			case 'w':
			{
				/* Spawn */
				do_cmd_options_win();
				p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL |
				                  PW_PLAYER | PW_MESSAGE | PW_OVERHEAD |
				                  PW_MONSTER | PW_OBJECT | PW_SNAPSHOT |
				                  PW_BORG_1 | PW_BORG_2 | PW_DUNGEON);
				break;
			}

			/* Hack -- Delay Speed */
			case 'D':
			case 'd':
			{
				/* Prompt */
#ifdef JP
				prt("���ޥ��: ���ܥ���������", 18, 0);
#else
				prt("Command: Base Delay Factor", 18, 0);
#endif


				/* Get a new value */
				while (1)
				{
					int msec = delay_factor * delay_factor * delay_factor;
#ifdef JP
					prt(format("���ߤΥ�������: %d (%d�ߥ���)",
#else
					prt(format("Current base delay factor: %d (%d msec)",
#endif

					           delay_factor, msec), 22, 0);
#ifdef JP
					prt("�������� (0-9) ESC�Ƿ���: ", 20, 0);
#else
					prt("Delay Factor (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if (k == ESCAPE) break;
					if (isdigit(k)) delay_factor = D2I(k);
					else bell();
				}

				break;
			}

			/* Hack -- hitpoint warning factor */
			case 'H':
			case 'h':
			{
				/* Prompt */
#ifdef JP
				prt("���ޥ��: ��ҥåȥݥ���ȷٹ�", 18, 0);
#else
				prt("Command: Hitpoint Warning", 18, 0);
#endif


				/* Get a new value */
				while (1)
				{
#ifdef JP
	    prt(format("���ߤ���ҥåȥݥ���ȷٹ�: %d0%%",
		       hitpoint_warn), 22, 0);
#else
					prt(format("Current hitpoint warning: %d0%%",
					           hitpoint_warn), 22, 0);
#endif

#ifdef JP
			prt("��ҥåȥݥ���ȷٹ� (0-9) ESC�Ƿ���: ", 20, 0);
#else
					prt("Hitpoint Warning (0-9 or ESC to accept): ", 20, 0);
#endif

					k = inkey();
					if (k == ESCAPE) break;
					if (isdigit(k)) hitpoint_warn = D2I(k);
					else bell();
				}

				break;
			}

			/* Unknown option */
			default:
			{
				/* Oops */
				bell();
				break;
			}
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	screen_load();

	/* Hack - Redraw equippy chars */
	p_ptr->redraw |= (PR_EQUIPPY);
}



/*
 * Ask for a "user pref line" and process it
 *
 * XXX XXX XXX Allow absolute file names?
 */
void do_cmd_pref(void)
{
	char buf[80];

	/* Default */
	strcpy(buf, "");

	/* Ask for a "user pref command" */
#ifdef JP
	if (!get_string("�����ѹ����ޥ��: ", buf, 80)) return;
#else
	if (!get_string("Pref: ", buf, 80)) return;
#endif


	/* Process that pref command */
	(void)process_pref_file_command(buf);
}

void do_cmd_pickpref(void)
{
	char buf[80];
	errr err = -1; 
	int i;

#ifdef JP
	if(!get_check("��ư��������ե��������ɤ��ޤ���? ")) return;
#else
	if(!get_check("Reload auto-pick preference file? ")) return;
#endif
	/* ���ޤޤǻȤäƤ���������� */
	for( i = 0; i < max_autopick; i++){
		free(autopick_name[i]);
		free(autopick_insc[i]);
	}
	max_autopick = 0;

	/* ������������ե�������ɤ߹��� */
#ifdef JP
	sprintf(buf, "picktype-%s.prf", player_name);
#else
	sprintf(buf, "pickpref-%s.prf", player_name);
#endif
	if( process_pickpref_file(buf) == 0 ){
		err = 0;
#ifdef JP
		msg_format("%s���ɤ߹��ߤޤ�����", buf);
#else
		msg_format("loaded '%s'.", buf);
#endif
	}

	/* ���̤�����ե������ɤ߹��� */
#ifdef JP
	if( process_pickpref_file("picktype.prf") == 0 )
#else
	if( process_pickpref_file("pickpref.prf") == 0 )
#endif
	{
		err = 0;
#ifdef JP
		msg_print("picktype.prf���ɤ߹��ߤޤ�����");
#else
		msg_print("loaded 'pickpref.prf'.");
#endif
	}

#ifdef JP
	if(err) msg_print("��ư��������ե�������ɤ߹��ߤ˼��Ԥ��ޤ�����");
#else
	if(err) msg_print("Failed to reload autopick preference.");
#endif
}

#ifdef ALLOW_MACROS

/*
 * Hack -- append all current macros to the given file
 */
static errr macro_dump(cptr fname)
{
	int i;

	FILE *fff;

	char buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) {
#ifdef JP
		msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
		msg_format("Failed to open %s.", buf);
#endif
		msg_print(NULL);
		return (-1);
	}


	/* Skip space */
	fprintf(fff, "\n\n");

	/* Start dumping */
#ifdef JP
	fprintf(fff, "# ��ư�ޥ�������\n\n");
#else
	fprintf(fff, "# Automatic macro dump\n\n");
#endif


	/* Dump them */
	for (i = 0; i < macro__num; i++)
	{
		/* Start the macro */
#ifdef JP
	fprintf(fff, "# ��ư�������֥�����\n\n");
#else
		fprintf(fff, "# Macro '%d'\n\n", i);
#endif


		/* Extract the action */
		ascii_to_text(buf, macro__act[i]);

		/* Dump the macro */
		fprintf(fff, "A:%s\n", buf);

		/* Extract the action */
		ascii_to_text(buf, macro__pat[i]);

		/* Dump normal macros */
		fprintf(fff, "P:%s\n", buf);

		/* End the macro */
		fprintf(fff, "\n\n");
	}

	/* Start dumping */
	fprintf(fff, "\n\n\n\n");


	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}


/*
 * Hack -- ask for a "trigger" (see below)
 *
 * Note the complex use of the "inkey()" function from "util.c".
 *
 * Note that both "flush()" calls are extremely important.
 */
static void do_cmd_macro_aux(char *buf)
{
	int i, n = 0;

	char tmp[1024];


	/* Flush */
	flush();

	/* Do not process macros */
	inkey_base = TRUE;

	/* First key */
	i = inkey();

	/* Read the pattern */
	while (i)
	{
		/* Save the key */
		buf[n++] = i;

		/* Do not process macros */
		inkey_base = TRUE;

		/* Do not wait for keys */
		inkey_scan = TRUE;

		/* Attempt to read a key */
		i = inkey();
	}

	/* Terminate */
	buf[n] = '\0';

	/* Flush */
	flush();


	/* Convert the trigger */
	ascii_to_text(tmp, buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);
}

#endif


/*
 * Hack -- ask for a keymap "trigger" (see below)
 *
 * Note that both "flush()" calls are extremely important.  This may
 * no longer be true, since "util.c" is much simpler now.  XXX XXX XXX
 */
static void do_cmd_macro_aux_keymap(char *buf)
{
	char tmp[1024];


	/* Flush */
	flush();


	/* Get a key */
	buf[0] = inkey();
	buf[1] = '\0';


	/* Convert to ascii */
	ascii_to_text(tmp, buf);

	/* Hack -- display the trigger */
	Term_addstr(-1, TERM_WHITE, tmp);


	/* Flush */
	flush();
}


/*
 * Hack -- append all keymaps to the given file
 */
static errr keymap_dump(cptr fname)
{
	int i;

	FILE *fff;

	char key[1024];
	char buf[1024];

	int mode;


	/* Roguelike */
	if (rogue_like_commands)
	{
		mode = KEYMAP_MODE_ROGUE;
	}

	/* Original */
	else
	{
		mode = KEYMAP_MODE_ORIG;
	}


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, fname);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Append to the file */
	fff = my_fopen(buf, "a");

	/* Failure */
	if (!fff) {
#ifdef JP
		msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
		msg_format("Failed to open %s.", buf);
#endif
		msg_print(NULL);
		return (-1);
	}


	/* Skip space */
	fprintf(fff, "\n\n");

	/* Start dumping */
#ifdef JP
	fprintf(fff, "# ��ư�������֥�����\n\n");
#else
	fprintf(fff, "# Automatic keymap dump\n\n");
#endif


	/* Dump them */
	for (i = 0; i < 256; i++)
	{
		cptr act;

		/* Loop up the keymap */
		act = keymap_act[mode][i];

		/* Skip empty keymaps */
		if (!act) continue;

		/* Encode the key */
		buf[0] = i;
		buf[1] = '\0';
		ascii_to_text(key, buf);

		/* Encode the action */
		ascii_to_text(buf, act);

		/* Dump the macro */
		fprintf(fff, "A:%s\n", buf);
		fprintf(fff, "C:%d:%s\n", mode, key);
	}

	/* Start dumping */
	fprintf(fff, "\n\n\n");


	/* Close */
	my_fclose(fff);

	/* Success */
	return (0);
}



/*
 * Interact with "macros"
 *
 * Note that the macro "action" must be defined before the trigger.
 *
 * Could use some helpful instructions on this page.  XXX XXX XXX
 */
void do_cmd_macros(void)
{
	int i;

	char tmp[1024];

	char buf[1024];

	int mode;


	/* Roguelike */
	if (rogue_like_commands)
	{
		mode = KEYMAP_MODE_ROGUE;
	}

	/* Original */
	else
	{
		mode = KEYMAP_MODE_ORIG;
	}

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save screen */
	screen_save();


	/* Process requests until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Describe */
#ifdef JP
		prt("[ �ޥ�������� ]", 2, 0);
#else
		prt("Interact with Macros", 2, 0);
#endif



		/* Describe that action */
#ifdef JP
		prt("(1) �桼��������ե�����Υ���", 4, 5);
#else
		prt("Current action (if any) shown below:", 20, 0);
#endif


		/* Analyze the current action */
		ascii_to_text(buf, macro__buf);

		/* Display the current action */
		prt(buf, 22, 0);


		/* Selections */
#ifdef JP
		prt("(1) �桼��������ե�����Υ���", 4, 5);
#else
		prt("(1) Load a user pref file", 4, 5);
#endif

#ifdef ALLOW_MACROS
#ifdef JP
                prt("(2) �ե�����˥ޥ�����ɲ�", 5, 5);
                prt("(3) �ޥ���γ�ǧ", 6, 5);
                prt("(4) �ޥ���κ���", 7, 5);
                prt("(5) �ޥ���κ��", 8, 5);
                prt("(6) �ե�����˥������֤��ɲ�", 9, 5);
                prt("(7) �������֤γ�ǧ", 10, 5);
                prt("(8) �������֤κ���", 11, 5);
                prt("(9) �������֤κ��", 12, 5);
                prt("(0) �ޥ����ư������", 13, 5);
#else
		prt("(2) Append macros to a file", 5, 5);
		prt("(3) Query a macro", 6, 5);
		prt("(4) Create a macro", 7, 5);
		prt("(5) Remove a macro", 8, 5);
		prt("(6) Append keymaps to a file", 9, 5);
		prt("(7) Query a keymap", 10, 5);
		prt("(8) Create a keymap", 11, 5);
		prt("(9) Remove a keymap", 12, 5);
		prt("(0) Enter a new action", 13, 5);
#endif

#endif /* ALLOW_MACROS */

		/* Prompt */
#ifdef JP
		prt("���ޥ��: ", 16, 0);
#else
		prt("Command: ", 16, 0);
#endif


		/* Get a command */
		i = inkey();

		/* Leave */
		if (i == ESCAPE) break;

		/* Load a 'macro' file */
		else if (i == '1')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �桼��������ե�����Υ���", 16, 0);
#else
			prt("Command: Load a user pref file", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 18, 0);
#else
			prt("File: ", 18, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Ask for a file */
			if (!askfor_aux(tmp, 80)) continue;

			/* Process the given filename */
			if (0 != process_pref_file(tmp))
			{
				/* Prompt */
#ifdef JP
				msg_print("�ե�������ɤ߹���ޤ���Ǥ�����");
#else
				msg_print("Could not load file!");
#endif

			}
		}

#ifdef ALLOW_MACROS

		/* Save macros */
		else if (i == '2')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �ޥ����ե�������ɲä���", 16, 0);
#else
			prt("Command: Append macros to a file", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 18, 0);
#else
			prt("File: ", 18, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Ask for a file */
			if (!askfor_aux(tmp, 80)) continue;

			/* Drop priv's */
			safe_setuid_drop();

			/* Dump the macros */
			(void)macro_dump(tmp);

			/* Grab priv's */
			safe_setuid_grab();

			/* Prompt */
#ifdef JP
			msg_print("�ޥ�����ɲä��ޤ�����");
#else
			msg_print("Appended macros.");
#endif

		}

		/* Query a macro */
		else if (i == '3')
		{
			int k;

			/* Prompt */
#ifdef JP
			prt("���ޥ��: �ޥ���γ�ǧ", 16, 0);
#else
			prt("Command: Query a macro", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ȥꥬ������: ", 18, 0);
#else
			prt("Trigger: ", 18, 0);
#endif


			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Acquire action */
			k = macro_find_exact(buf);

			/* Nothing found */
			if (k < 0)
			{
				/* Prompt */
#ifdef JP
				msg_print("���Υ����ˤϥޥ�����������Ƥ��ޤ���");
#else
				msg_print("Found no macro.");
#endif

			}

			/* Found one */
			else
			{
				/* Obtain the action */
				strcpy(macro__buf, macro__act[k]);

				/* Analyze the current action */
				ascii_to_text(buf, macro__buf);

				/* Display the current action */
				prt(buf, 22, 0);

				/* Prompt */
#ifdef JP
				msg_print("�ޥ�����ǧ���ޤ�����");
#else
				msg_print("Found a macro.");
#endif

			}
		}

		/* Create a macro */
		else if (i == '4')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �ޥ���κ���", 16, 0);
#else
			prt("Command: Create a macro", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ȥꥬ������: ", 18, 0);
#else
			prt("Trigger: ", 18, 0);
#endif


			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Clear */
			clear_from(20);

			/* Prompt */
#ifdef JP
			prt("�ޥ����ư: ", 20, 0);
#else
			prt("Action: ", 20, 0);
#endif


			/* Convert to text */
			ascii_to_text(tmp, macro__buf);

			/* Get an encoded action */
			if (askfor_aux(tmp, 80))
			{
				/* Convert to ascii */
				text_to_ascii(macro__buf, tmp);

				/* Link the macro */
				macro_add(buf, macro__buf);

				/* Prompt */
#ifdef JP
				msg_print("�ޥ�����ɲä��ޤ�����");
#else
				msg_print("Added a macro.");
#endif

			}
		}

		/* Remove a macro */
		else if (i == '5')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �ޥ���κ��", 16, 0);
#else
			prt("Command: Remove a macro", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ȥꥬ������: ", 18, 0);
#else
			prt("Trigger: ", 18, 0);
#endif


			/* Get a macro trigger */
			do_cmd_macro_aux(buf);

			/* Link the macro */
			macro_add(buf, buf);

			/* Prompt */
#ifdef JP
			msg_print("�ޥ���������ޤ�����");
#else
			msg_print("Removed a macro.");
#endif

		}

		/* Save keymaps */
		else if (i == '6')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �������֤�ե�������ɲä���", 16, 0);
#else
			prt("Command: Append keymaps to a file", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 18, 0);
#else
			prt("File: ", 18, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Ask for a file */
			if (!askfor_aux(tmp, 80)) continue;

			/* Drop priv's */
			safe_setuid_drop();

			/* Dump the macros */
			(void)keymap_dump(tmp);

			/* Grab priv's */
			safe_setuid_grab();

			/* Prompt */
#ifdef JP
			msg_print("�������֤��ɲä��ޤ�����");
#else
			msg_print("Appended keymaps.");
#endif

		}

		/* Query a keymap */
		else if (i == '7')
		{
			cptr act;

			/* Prompt */
#ifdef JP
			prt("���ޥ��: �������֤γ�ǧ", 16, 0);
#else
			prt("Command: Query a keymap", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("��������: ", 18, 0);
#else
			prt("Keypress: ", 18, 0);
#endif


			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Look up the keymap */
			act = keymap_act[mode][(byte)(buf[0])];

			/* Nothing found */
			if (!act)
			{
				/* Prompt */
#ifdef JP
				msg_print("�������֤��������Ƥ��ޤ���");
#else
				msg_print("Found no keymap.");
#endif

			}

			/* Found one */
			else
			{
				/* Obtain the action */
				strcpy(macro__buf, act);

				/* Analyze the current action */
				ascii_to_text(buf, macro__buf);

				/* Display the current action */
				prt(buf, 22, 0);

				/* Prompt */
#ifdef JP
				msg_print("�������֤��ǧ���ޤ�����");
#else
				msg_print("Found a keymap.");
#endif

			}
		}

		/* Create a keymap */
		else if (i == '8')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �������֤κ���", 16, 0);
#else
			prt("Command: Create a keymap", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("��������: ", 18, 0);
#else
			prt("Keypress: ", 18, 0);
#endif


			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Clear */
			clear_from(20);

			/* Prompt */
#ifdef JP
			prt("��ư: ", 20, 0);
#else
			prt("Action: ", 20, 0);
#endif


			/* Convert to text */
			ascii_to_text(tmp, macro__buf);

			/* Get an encoded action */
			if (askfor_aux(tmp, 80))
			{
				/* Convert to ascii */
				text_to_ascii(macro__buf, tmp);

				/* Free old keymap */
				string_free(keymap_act[mode][(byte)(buf[0])]);

				/* Make new keymap */
				keymap_act[mode][(byte)(buf[0])] = string_make(macro__buf);

				/* Prompt */
#ifdef JP
				msg_print("�������֤��ɲä��ޤ�����");
#else
				msg_print("Added a keymap.");
#endif

			}
		}

		/* Remove a keymap */
		else if (i == '9')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �������֤κ��", 16, 0);
#else
			prt("Command: Remove a keymap", 16, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("��������: ", 18, 0);
#else
			prt("Keypress: ", 18, 0);
#endif


			/* Get a keymap trigger */
			do_cmd_macro_aux_keymap(buf);

			/* Free old keymap */
			string_free(keymap_act[mode][(byte)(buf[0])]);

			/* Make new keymap */
			keymap_act[mode][(byte)(buf[0])] = NULL;

			/* Prompt */
#ifdef JP
			msg_print("�������֤������ޤ�����");
#else
			msg_print("Removed a keymap.");
#endif

		}

		/* Enter a new action */
		else if (i == '0')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �ޥ����ư������", 16, 0);
#else
			prt("Command: Enter a new action", 16, 0);
#endif


			/* Go to the correct location */
			Term_gotoxy(0, 22);

			/* Hack -- limit the value */
			tmp[80] = '\0';

			/* Get an encoded action */
			if (!askfor_aux(buf, 80)) continue;

			/* Extract an action */
			text_to_ascii(macro__buf, buf);
		}

#endif /* ALLOW_MACROS */

		/* Oops */
		else
		{
			/* Oops */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Load screen */
	screen_load();
}


/*
 * Interact with "visuals"
 */
void do_cmd_visuals(void)
{
	int i;

	FILE *fff;

	char tmp[160];

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save the screen */
	screen_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("����ɽ��������", 2, 0);
#else
		prt("Interact with Visuals", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1) �桼��������ե�����Υ���", 4, 5);
#else
		prt("(1) Load a user pref file", 4, 5);
#endif

#ifdef ALLOW_VISUALS
#ifdef JP
		prt("(2) ��󥹥����� ��/ʸ�� ��ե�����˽񤭽Ф�", 5, 5);
		prt("(3) �����ƥ��   ��/ʸ�� ��ե�����˽񤭽Ф�", 6, 5);
		prt("(4) �Ϸ���       ��/ʸ�� ��ե�����˽񤭽Ф�", 7, 5);
		prt("(5) (̤����)", 8, 5);
		prt("(6) ��󥹥����� ��/ʸ�� ���ѹ�����", 9, 5);
		prt("(7) �����ƥ��   ��/ʸ�� ���ѹ�����", 10, 5);
		prt("(8) �Ϸ���       ��/ʸ�� ���ѹ�����", 11, 5);
		prt("(9) (̤����)", 12, 5);
#else
		prt("(2) Dump monster attr/chars", 5, 5);
		prt("(3) Dump object attr/chars", 6, 5);
		prt("(4) Dump feature attr/chars", 7, 5);
		prt("(5) (unused)", 8, 5);
		prt("(6) Change monster attr/chars", 9, 5);
		prt("(7) Change object attr/chars", 10, 5);
		prt("(8) Change feature attr/chars", 11, 5);
		prt("(9) (unused)", 12, 5);
#endif

#endif
#ifdef JP
		prt("(0) ����ɽ����ˡ�ν����", 13, 5);
#else
		prt("(0) Reset visuals", 13, 5);
#endif


		/* Prompt */
#ifdef JP
		prt("���ޥ��:", 18, 0);
#else
		prt("Command: ", 15, 0);
#endif


		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		/* Load a 'pref' file */
		else if (i == '1')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �桼��������ե�����Υ���", 15, 0);
#else
			prt("Command: Load a user pref file", 15, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 18, 0);
#else
			prt("File: ", 17, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Query */
			if (!askfor_aux(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);
		}

#ifdef ALLOW_VISUALS

		/* Dump monster attr/chars */
		else if (i == '2')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: ��󥹥�����[��/ʸ��]��ե�����˽񤭽Ф��ޤ�", 15, 0);
#else
			prt("Command: Dump monster attr/chars", 15, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);
			
			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) {
#ifdef JP
				msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
				msg_format("Failed to open %s.", buf);
#endif
				msg_print(NULL);
				continue;
			}

			/* Start dumping */
			fprintf(fff, "\n\n");
#ifdef JP
			fprintf(fff, "# ��󥹥�����[��/ʸ��]������\n\n");
#else
			fprintf(fff, "# Monster attr/char definitions\n\n");
#endif


			/* Dump monsters */
			for (i = 0; i < max_r_idx; i++)
			{
				monster_race *r_ptr = &r_info[i];

				/* Skip non-entries */
				if (!r_ptr->name) continue;

				/* Dump a comment */
				fprintf(fff, "# %s\n", (r_name + r_ptr->name));


				/* Dump the monster attr/char info */
				fprintf(fff, "R:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(r_ptr->x_attr), (byte)(r_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("��󥹥�����[��/ʸ��]��ե�����˽񤭽Ф��ޤ�����");
#else
			msg_print("Dumped monster attr/chars.");
#endif

		}

		/* Dump object attr/chars */
		else if (i == '3')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �����ƥ��[��/ʸ��]��ե�����˽񤭽Ф��ޤ�", 15, 0);
#else
			prt("Command: Dump object attr/chars", 15, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) {
#ifdef JP
				msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
				msg_format("Failed to open %s.", buf);
#endif
				msg_print(NULL);
				continue;
			}

			/* Start dumping */
			fprintf(fff, "\n\n");
#ifdef JP
			fprintf(fff, "# �����ƥ��[��/ʸ��]������\n\n");
#else
			fprintf(fff, "# Object attr/char definitions\n\n");
#endif


			/* Dump objects */
			for (i = 0; i < max_k_idx; i++)
			{
				object_kind *k_ptr = &k_info[i];

				/* Skip non-entries */
				if (!k_ptr->name) continue;

				/* Dump a comment */
				fprintf(fff, "# %s\n", (k_name + k_ptr->name));


				/* Dump the object attr/char info */
				fprintf(fff, "K:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(k_ptr->x_attr), (byte)(k_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("�����ƥ��[��/ʸ��]��ե�����˽񤭽Ф��ޤ�����");
#else
			msg_print("Dumped object attr/chars.");
#endif

		}

		/* Dump feature attr/chars */
		else if (i == '4')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �Ϸ���[��/ʸ��]��ե�����˽񤭽Ф��ޤ�", 15, 0);
#else
			prt("Command: Dump feature attr/chars", 15, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 17, 0);
#else
			prt("File: ", 17, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) {
#ifdef JP
				msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
				msg_format("Failed to open %s.", buf);
#endif
				msg_print(NULL);
				continue;
			}

			/* Start dumping */
			fprintf(fff, "\n\n");
#ifdef JP
			fprintf(fff, "# �Ϸ���[��/ʸ��]������\n\n");
#else
			fprintf(fff, "# Feature attr/char definitions\n\n");
#endif


			/* Dump features */
			for (i = 0; i < max_f_idx; i++)
			{
				feature_type *f_ptr = &f_info[i];

				/* Skip non-entries */
				if (!f_ptr->name) continue;

				/* Dump a comment */
				fprintf(fff, "# %s\n", (f_name + f_ptr->name));

				/* Dump the feature attr/char info */
				fprintf(fff, "F:%d:0x%02X:0x%02X\n\n", i,
				        (byte)(f_ptr->x_attr), (byte)(f_ptr->x_char));
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("�Ϸ���[��/ʸ��]��ե�����˽񤭽Ф��ޤ�����");
#else
			msg_print("Dumped feature attr/chars.");
#endif

		}

		/* Modify monster attr/chars */
		else if (i == '6')
		{
			static int r = 0;

			/* Prompt */
#ifdef JP
			prt("���ޥ��: ��󥹥�����[��/ʸ��]���ѹ����ޤ�", 15, 0);
#else
			prt("Command: Change monster attr/chars", 15, 0);
#endif


			/* Hack -- query until done */
			while (1)
			{
				monster_race *r_ptr = &r_info[r];

				byte da = (r_ptr->d_attr);
				char dc = (r_ptr->d_char);
				byte ca = (r_ptr->x_attr);
				char cc = (r_ptr->x_char);

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("��󥹥��� = %d, ̾�� = %-40.40s",
				                   r, (r_name + r_ptr->name)));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Monster = %d, Name = %-40.40s",
				                   r, (r_name + r_ptr->name)));
#endif


				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("�����  �� / ʸ�� = %3u / %3u", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3u / %3u", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("������  �� / ʸ�� = %3u / %3u", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3u / %3u", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 20, ca, cc);

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "���ޥ�� (n/N/a/A/c/C): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif


				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') r = (r + max_r_idx + 1) % max_r_idx;
				if (i == 'N') r = (r + max_r_idx - 1) % max_r_idx;
				if (i == 'a') r_ptr->x_attr = (byte)(ca + 1);
				if (i == 'A') r_ptr->x_attr = (byte)(ca - 1);
				if (i == 'c') r_ptr->x_char = (byte)(cc + 1);
				if (i == 'C') r_ptr->x_char = (byte)(cc - 1);
			}
		}

		/* Modify object attr/chars */
		else if (i == '7')
		{
			static int k = 0;

			/* Prompt */
#ifdef JP
			prt("���ޥ��: �����ƥ��[��/ʸ��]���ѹ����ޤ�", 15, 0);
#else
			prt("Command: Change object attr/chars", 15, 0);
#endif


			/* Hack -- query until done */
			while (1)
			{
				object_kind *k_ptr = &k_info[k];

				byte da = (byte)k_ptr->d_attr;
				char dc = (byte)k_ptr->d_char;
				byte ca = (byte)k_ptr->x_attr;
				char cc = (byte)k_ptr->x_char;

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("�����ƥ� = %d, ̾�� = %-40.40s",
				                   k, (k_name + k_ptr->name)));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Object = %d, Name = %-40.40s",
				                   k, (k_name + k_ptr->name)));
#endif


				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("�����  �� / ʸ�� = %3d / %3d", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3d / %3d", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("������  �� / ʸ�� = %3d / %3d", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3d / %3d", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 20, ca, cc);

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "���ޥ�� (n/N/a/A/c/C): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif


				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') k = (k + max_k_idx + 1) % max_k_idx;
				if (i == 'N') k = (k + max_k_idx - 1) % max_k_idx;
				if (i == 'a') k_info[k].x_attr = (byte)(ca + 1);
				if (i == 'A') k_info[k].x_attr = (byte)(ca - 1);
				if (i == 'c') k_info[k].x_char = (byte)(cc + 1);
				if (i == 'C') k_info[k].x_char = (byte)(cc - 1);
			}
		}

		/* Modify feature attr/chars */
		else if (i == '8')
		{
			static int f = 0;

			/* Prompt */
#ifdef JP
			prt("���ޥ��: �Ϸ���[��/ʸ��]���ѹ����ޤ�", 15, 0);
#else
			prt("Command: Change feature attr/chars", 15, 0);
#endif


			/* Hack -- query until done */
			while (1)
			{
				feature_type *f_ptr = &f_info[f];

				byte da = (byte)f_ptr->d_attr;
				char dc = (byte)f_ptr->d_char;
				byte ca = (byte)f_ptr->x_attr;
				char cc = (byte)f_ptr->x_char;

				/* Label the object */
#ifdef JP
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("�Ϸ� = %d, ̾�� = %-40.40s",
				                   f, (f_name + f_ptr->name)));
#else
				Term_putstr(5, 17, -1, TERM_WHITE,
				            format("Terrain = %d, Name = %-40.40s",
				                   f, (f_name + f_ptr->name)));
#endif


				/* Label the Default values */
#ifdef JP
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("�����  �� / ʸ�� = %3d / %3d", da, dc));
#else
				Term_putstr(10, 19, -1, TERM_WHITE,
				            format("Default attr/char = %3d / %3d", da, dc));
#endif

				Term_putstr(40, 19, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 19, da, dc);

				/* Label the Current values */
#ifdef JP
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("������  �� / ʸ�� = %3d / %3d", ca, cc));
#else
				Term_putstr(10, 20, -1, TERM_WHITE,
				            format("Current attr/char = %3d / %3d", ca, cc));
#endif

				Term_putstr(40, 20, -1, TERM_WHITE, "<< ? >>");
				Term_putch(43, 20, ca, cc);

				/* Prompt */
#ifdef JP
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "���ޥ�� (n/N/a/A/c/C): ");
#else
				Term_putstr(0, 22, -1, TERM_WHITE,
				            "Command (n/N/a/A/c/C): ");
#endif


				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') f = (f + max_f_idx + 1) % max_f_idx;
				if (i == 'N') f = (f + max_f_idx - 1) % max_f_idx;
				if (i == 'a') f_info[f].x_attr = (byte)(ca + 1);
				if (i == 'A') f_info[f].x_attr = (byte)(ca - 1);
				if (i == 'c') f_info[f].x_char = (byte)(cc + 1);
				if (i == 'C') f_info[f].x_char = (byte)(cc - 1);
			}
		}

#endif

		/* Reset visuals */
		else if (i == '0')
		{
			/* Reset */
			reset_visuals();

			/* Message */
#ifdef JP
			msg_print("���̾��[��/ʸ��]�����ͤ˥ꥻ�åȤ��ޤ�����");
#else
			msg_print("Visual attr/char tables reset.");
#endif

		}

		/* Unknown option */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	screen_load();
}


/*
 * Interact with "colors"
 */
void do_cmd_colors(void)
{
	int i;

	FILE *fff;

	char tmp[160];

	char buf[1024];


	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);


	/* Save the screen */
	screen_save();


	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("[ ���顼������ ]", 2, 0);
#else
		prt("Interact with Colors", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1) �桼��������ե�����Υ���", 4, 5);
#else
		prt("(1) Load a user pref file", 4, 5);
#endif

#ifdef ALLOW_COLORS
#ifdef JP
                prt("(2) ���顼�������ե�����˽񤭽Ф�", 5, 5);
                prt("(3) ���顼��������ѹ�����", 6, 5);
#else
		prt("(2) Dump colors", 5, 5);
		prt("(3) Modify colors", 6, 5);
#endif

#endif

		/* Prompt */
#ifdef JP
		prt("���ޥ��: ", 8, 0);
#else
		prt("Command: ", 8, 0);
#endif


		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		/* Load a 'pref' file */
		if (i == '1')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: �桼��������ե��������ɤ��ޤ�", 8, 0);
#else
			prt("Command: Load a user pref file", 8, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 10, 0);
#else
			prt("File: ", 10, 0);
#endif


			/* Default file */
			sprintf(tmp, "%s.prf", player_name);

			/* Query */
			if (!askfor_aux(tmp, 70)) continue;

			/* Process the given filename */
			(void)process_pref_file(tmp);

			/* Mega-Hack -- react to changes */
			Term_xtra(TERM_XTRA_REACT, 0);

			/* Mega-Hack -- redraw */
			Term_redraw();
		}

#ifdef ALLOW_COLORS

		/* Dump colors */
		else if (i == '2')
		{
			/* Prompt */
#ifdef JP
			prt("���ޥ��: ���顼�������ե�����˽񤭽Ф��ޤ�", 8, 0);
#else
			prt("Command: Dump colors", 8, 0);
#endif


			/* Prompt */
#ifdef JP
			prt("�ե�����: ", 10, 0);
#else
			prt("File: ", 10, 0);
#endif


			/* Default filename */
			sprintf(tmp, "%s.prf", player_name);

			/* Get a filename */
			if (!askfor_aux(tmp, 70)) continue;

			/* Build the filename */
			path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

			/* Drop priv's */
			safe_setuid_drop();

			/* Append to the file */
			fff = my_fopen(buf, "a");

			/* Grab priv's */
			safe_setuid_grab();

			/* Failure */
			if (!fff) {
#ifdef JP
				msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
				msg_format("Failed to open %s.", buf);
#endif
				msg_print(NULL);
				continue;
			}

			/* Start dumping */
			fprintf(fff, "\n\n");
#ifdef JP
			fprintf(fff, "# ���顼������\n\n");
#else
			fprintf(fff, "# Color redefinitions\n\n");
#endif


			/* Dump colors */
			for (i = 0; i < 256; i++)
			{
				int kv = angband_color_table[i][0];
				int rv = angband_color_table[i][1];
				int gv = angband_color_table[i][2];
				int bv = angband_color_table[i][3];

#ifdef JP
				cptr name = "̤��";
#else
				cptr name = "unknown";
#endif


				/* Skip non-entries */
				if (!kv && !rv && !gv && !bv) continue;

				/* Extract the color name */
				if (i < 16) name = color_names[i];

				/* Dump a comment */
#ifdef JP
				fprintf(fff, "# ���顼 '%s'\n", name);
#else
				fprintf(fff, "# Color '%s'\n", name);
#endif


				/* Dump the monster attr/char info */
				fprintf(fff, "V:%d:0x%02X:0x%02X:0x%02X:0x%02X\n\n",
				        i, kv, rv, gv, bv);
			}

			/* All done */
			fprintf(fff, "\n\n\n\n");

			/* Close */
			my_fclose(fff);

			/* Message */
#ifdef JP
			msg_print("���顼�������ե�����˽񤭽Ф��ޤ�����");
#else
			msg_print("Dumped color redefinitions.");
#endif

		}

		/* Edit colors */
		else if (i == '3')
		{
			static byte a = 0;

			/* Prompt */
#ifdef JP
			prt("���ޥ��: ���顼��������ѹ����ޤ�", 8, 0);
#else
			prt("Command: Modify colors", 8, 0);
#endif


			/* Hack -- query until done */
			while (1)
			{
				cptr name;
				byte j;

				/* Clear */
				clear_from(10);

				/* Exhibit the normal colors */
				for (j = 0; j < 16; j++)
				{
					/* Exhibit this color */
					Term_putstr(j*4, 20, -1, a, "###");

					/* Exhibit all colors */
					Term_putstr(j*4, 22, -1, j, format("%3d", j));
				}

				/* Describe the color */
#ifdef JP
				name = ((a < 16) ? color_names[a] : "̤���");
#else
				name = ((a < 16) ? color_names[a] : "undefined");
#endif


				/* Describe the color */
#ifdef JP
				Term_putstr(5, 10, -1, TERM_WHITE,
				            format("���顼 = %d, ̾�� = %s", a, name));
#else
				Term_putstr(5, 10, -1, TERM_WHITE,
				            format("Color = %d, Name = %s", a, name));
#endif


				/* Label the Current values */
				Term_putstr(5, 12, -1, TERM_WHITE,
				            format("K = 0x%02x / R,G,B = 0x%02x,0x%02x,0x%02x",
				                   angband_color_table[a][0],
				                   angband_color_table[a][1],
				                   angband_color_table[a][2],
				                   angband_color_table[a][3]));

				/* Prompt */
#ifdef JP
				Term_putstr(0, 14, -1, TERM_WHITE,
				            "���ޥ�� (n/N/k/K/r/R/g/G/b/B): ");
#else
				Term_putstr(0, 14, -1, TERM_WHITE,
				            "Command (n/N/k/K/r/R/g/G/b/B): ");
#endif


				/* Get a command */
				i = inkey();

				/* All done */
				if (i == ESCAPE) break;

				/* Analyze */
				if (i == 'n') a = (byte)(a + 1);
				if (i == 'N') a = (byte)(a - 1);
				if (i == 'k') angband_color_table[a][0] = (byte)(angband_color_table[a][0] + 1);
				if (i == 'K') angband_color_table[a][0] = (byte)(angband_color_table[a][0] - 1);
				if (i == 'r') angband_color_table[a][1] = (byte)(angband_color_table[a][1] + 1);
				if (i == 'R') angband_color_table[a][1] = (byte)(angband_color_table[a][1] - 1);
				if (i == 'g') angband_color_table[a][2] = (byte)(angband_color_table[a][2] + 1);
				if (i == 'G') angband_color_table[a][2] = (byte)(angband_color_table[a][2] - 1);
				if (i == 'b') angband_color_table[a][3] = (byte)(angband_color_table[a][3] + 1);
				if (i == 'B') angband_color_table[a][3] = (byte)(angband_color_table[a][3] - 1);

				/* Hack -- react to changes */
				Term_xtra(TERM_XTRA_REACT, 0);

				/* Hack -- redraw */
				Term_redraw();
			}
		}

#endif

		/* Unknown option */
		else
		{
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}


	/* Restore the screen */
	screen_load();
}


/*
 * Note something in the message recall
 */
void do_cmd_note(void)
{
	char buf[80];

	/* Default */
	strcpy(buf, "");

	/* Input */
#ifdef JP
	if (!get_string("���: ", buf, 60)) return;
#else
	if (!get_string("Note: ", buf, 60)) return;
#endif


	/* Ignore empty notes */
	if (!buf[0] || (buf[0] == ' ')) return;

	/* Add the note to the message recall */
#ifdef JP
	msg_format("���: %s", buf);
#else
	msg_format("Note: %s", buf);
#endif

}


/*
 * Mention the current version
 */
void do_cmd_version(void)
{

	/* Silly message */
#ifndef FAKE_VERSION
	msg_format("You are playing Angband %d.%d.%d.",
	           VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
#else
#ifdef JP
	msg_format("�Ѷ�����(Hengband) %d.%d.%d",
	            FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#else
	msg_format("You are playing Hengband %d.%d.%d.",
	            FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif

#endif

}



/*
 * Array of feeling strings
 */
static cptr do_cmd_feeling_text[11] =
{
#ifdef JP
	"���γ���ʷ�ϵ��򴶤��Ȥ�ʤ��ä�...",
#else
	"Looks like any other level.",
#endif

#ifdef JP
	"���γ��ˤϲ������̤ʤ�Τ�����褦�ʵ������롣",
#else
	"You feel there is something special about this level.",
#endif

#ifdef JP
	"��������θ����ܤ��⤫�ӡ����䤷�����ˤʤä���",
#else
	"You nearly faint as horrible visions of death fill your mind!",
#endif

#ifdef JP
	"���γ��ϤȤƤ���ʤ褦����",
#else
	"This level looks very dangerous.",
#endif

#ifdef JP
	"�ȤƤⰭ��ͽ��������...",
#else
	"You have a very bad feeling...",
#endif

#ifdef JP
	"����ͽ��������...",
#else
	"You have a bad feeling...",
#endif

#ifdef JP
	"������ĥ���롣",
#else
	"You feel nervous.",
#endif

#ifdef JP
	"�����Ա��ʵ�������...",
#else
	"You feel your luck is turning...",
#endif

#ifdef JP
	"���ξ��Ϲ����ˤʤ�ʤ���",
#else
	"You don't like the look of this place.",
#endif

#ifdef JP
	"���γ��Ϥ���ʤ�˰����ʤ褦����",
#else
	"This level looks reasonably safe.",
#endif

#ifdef JP
	"�ʤ������ʤȤ����..."
#else
	"What a boring place..."
#endif

};

static cptr do_cmd_feeling_text_combat[11] =
{
#ifdef JP
	"���γ���ʷ�ϵ��򴶤��Ȥ�ʤ��ä�...",
#else
	"Looks like any other level.",
#endif

#ifdef JP
	"���γ��ˤϲ������̤ʤ�Τ�����褦�ʵ������롣",
#else
	"You feel there is something special about this level.",
#endif

#ifdef JP
	"�����ޤ���ï����̿����Ȥ�...",
#else
	"You nearly faint as horrible visions of death fill your mind!",
#endif

#ifdef JP
	"���γ��ϤȤƤ���ʤ褦����",
#else
	"This level looks very dangerous.",
#endif

#ifdef JP
	"�ȤƤⰭ��ͽ��������...",
#else
	"You have a very bad feeling...",
#endif

#ifdef JP
	"����ͽ��������...",
#else
	"You have a bad feeling...",
#endif

#ifdef JP
	"������ĥ���롣",
#else
	"You feel nervous.",
#endif

#ifdef JP
	"�����Ա��ʵ�������...",
#else
	"You feel your luck is turning...",
#endif

#ifdef JP
	"���ξ��Ϲ����ˤʤ�ʤ���",
#else
	"You don't like the look of this place.",
#endif

#ifdef JP
	"���γ��Ϥ���ʤ�˰����ʤ褦����",
#else
	"This level looks reasonably safe.",
#endif

#ifdef JP
	"�ʤ������ʤȤ����..."
#else
	"What a boring place..."
#endif

};

static cptr do_cmd_feeling_text_lucky[11] =
{
#ifdef JP
	"���γ���ʷ�ϵ��򴶤��Ȥ�ʤ��ä�...",
	"���γ��ˤϲ������̤ʤ�Τ�����褦�ʵ������롣",
	"���γ��Ϥ��ξ�ʤ������餷�����������롣",
	"�����餷������������...",
	"�ȤƤ��ɤ�����������...",
	"�ɤ�����������...",
	"����äȹ����ʴ���������...",
	"¿���ϱ��������Ƥ�����...",
	"�������������Ϥʤ�...",
	"�������ܤȤ������ȤϤʤ���...",
	"�ʤ������ʤȤ����..."
#else
	"Looks like any other level.",
	"You feel there is something special about this level.",
	"You have a superb feeling about this level.",
	"You have an excellent feeling...",
	"You have a very good feeling...",
	"You have a good feeling...",
	"You feel strangely lucky...",
	"You feel your luck is turning...",
	"You like the look of this place...",
	"This level can't be all bad...",
	"What a boring place..."
#endif
};


/*
 * Note that "feeling" is set to zero unless some time has passed.
 * Note that this is done when the level is GENERATED, not entered.
 */
void do_cmd_feeling(void)
{
	/* Verify the feeling */
	if (feeling > 10) feeling = 10;

#ifdef USE_SCRIPT
	if (cmd_feeling_callback(feeling)) return;
#endif /* USE_SCRIPT */

	/* No useful feeling in quests */
	if (p_ptr->inside_quest && !random_quest_number(dun_level))
	{
#ifdef JP
		msg_print("ŵ��Ū�ʥ������ȤΥ��󥸥��Τ褦����");
#else
		msg_print("Looks like a typical quest level.");
#endif

		return;
	}

	/* No useful feeling in town */
	else if (p_ptr->town_num && !dun_level)
	{
#ifdef JP
		if (!strcmp(town[p_ptr->town_num].name, "����"))
#else
		if (!strcmp(town[p_ptr->town_num].name, "wilderness"))
#endif
		{
#ifdef JP
			msg_print("�������ꤽ���ʹ���Τ褦����");
#else
			msg_print("Looks like a strange wilderness.");
#endif

			return;
		}
		else
		{
#ifdef JP
			msg_print("ŵ��Ū��Į�Τ褦����");
#else
			msg_print("Looks like a typical town.");
#endif

			return;
		}
	}

	/* No useful feeling in the wilderness */
	else if (!dun_level)
	{
#ifdef JP
		msg_print("ŵ��Ū�ʹ���Τ褦����");
#else
		msg_print("Looks like a typical wilderness.");
#endif

		return;
	}

	/* Display the feeling */
        if (turn - old_turn >= (3000 - dun_level*20) || cheat_xtra)
        {
                if (p_ptr->muta3 & MUT3_GOOD_LUCK) msg_print(do_cmd_feeling_text_lucky[feeling]);
                else {
					if((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON)){
						msg_print(do_cmd_feeling_text_combat[feeling]);
					}else
						msg_print(do_cmd_feeling_text[feeling]);
				}
        }
        else
        {
                msg_print(do_cmd_feeling_text[0]);
        }
}





/*
 * Encode the screen colors
 */
static char hack[17] = "dwsorgbuDWvyRGBU";


static errr photo_fgets(FILE *fff, char *buf, huge n)
{
	huge i = 0;

	char *s;

	char tmp[1024];

	/* Read a line */
	if (fgets(tmp, 1024, fff))
	{
		/* Convert weirdness */
		for (s = tmp; *s; s++)
		{
			/* Handle newline */
			if (*s == '\n')
			{
				/* Terminate */
				buf[i] = '\0';

				/* Success */
				return (0);
			}

			/* Handle tabs */
			else if (*s == '\t')
			{
				/* Hack -- require room */
				if (i + 8 >= n) break;

				/* Append a space */
				buf[i++] = ' ';

				/* Append some more spaces */
				while (!(i % 8)) buf[i++] = ' ';
			}

#ifdef JP
			else if (iskanji(*s))
			{
				if (!s[1]) break;
				buf[i++] = *s++;
				buf[i++] = *s;
			}
# ifndef EUC
	/* Ⱦ�Ѥ��ʤ��б� */
			else if ((((int)*s & 0xff) > 0xa1) && (((int)*s & 0xff ) < 0xdf))
			{
				buf[i++] = *s;
				if (i >= n) break;
			}
# endif
#endif
			/* Handle printables */
			else
			{
				/* Copy */
				buf[i++] = *s;

				/* Check length */
				if (i >= n) break;
			}
		}
	}

	/* Nothing */
	buf[0] = '\0';

	/* Failure */
	return (1);
}


/*
 * Hack -- load a screen dump from a file
 */
void do_cmd_load_screen(void)
{
	int i, y, x;

	byte a = 0;
	char c = ' ';

	bool okay = TRUE;

	FILE *fff;

	char buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, "dump.txt");

	/* Append to the file */
	fff = my_fopen(buf, "r");

	/* Oops */
	if (!fff) {
#ifdef JP
		msg_format("%s �򳫤����Ȥ��Ǥ��ޤ���Ǥ�����", buf);
#else
		msg_format("Failed to open %s.", buf);
#endif
		msg_print(NULL);
		return;
	}


	/* Save the screen */
	screen_save();

	/* Clear the screen */
	Term_clear();


	/* Load the screen */
	for (y = 0; okay && (y < 24); y++)
	{
		/* Get a line of data */
		if (photo_fgets(fff, buf, 1024)) okay = FALSE;

		/* Show each row */
		for (x = 0; x < 79; x++)
		{
			/* Put the attr/char */
			Term_draw(x, y, TERM_WHITE, buf[x]);
		}
	}

	/* Get the blank line */
	if (my_fgets(fff, buf, 1024)) okay = FALSE;


	/* Dump the screen */
	for (y = 0; okay && (y < 24); y++)
	{
		/* Get a line of data */
		if (photo_fgets(fff, buf, 1024)) okay = FALSE;

		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Look up the attr */
			for (i = 0; i < 16; i++)
			{
				/* Use attr matches */
				if (hack[i] == buf[x]) a = i;
			}

			/* Put the attr/char */
			Term_draw(x, y, a, c);
		}
	}


	/* Get the blank line */
	if (my_fgets(fff, buf, 1024)) okay = FALSE;


	/* Close it */
	my_fclose(fff);


	/* Message */
#ifdef JP
	prt("�ե�����˽񤭽Ф��줿����(��ǰ����)����ɤ��ޤ�����", 0, 0);
#else
	msg_print("Screen dump loaded.");
#endif

	flush();
	inkey();


	/* Restore the screen */
	screen_load();
}




cptr inven_res_label = 
#ifdef JP
 "                               ���Ų����Ǹ����˹�������� ��������Ʃ̿��������";
#else
 "                               AcElFiCoPoLiDkShSoNtNxCaDi BlFeCfFaSeHlEpSdRgLv";
#endif

/* XTRA HACK RESLIST */
static void do_cmd_knowledge_inven_aux(FILE *fff, object_type *o_ptr, 
				       int *j, byte tval, char *where)
{
  char o_name[MAX_NLEN];
  u32b    f[3];

  if (!o_ptr->k_idx)return;
  if (o_ptr->tval != tval)return;

       /* 
	* HACK:Ring of Lordly protection and Dragon shield/helm
        * have random resistances.
        */
  if ( ((o_ptr->tval >= TV_BOW && o_ptr->tval<= TV_DRAG_ARMOR && o_ptr->name2)
       || (o_ptr->tval == TV_RING && o_ptr->sval == SV_RING_LORDLY) 
       || (o_ptr->tval == TV_SHIELD && o_ptr->sval == SV_DRAGON_SHIELD) 
       || (o_ptr->tval == TV_HELM && o_ptr->sval == SV_DRAGON_HELM) 
       || (o_ptr->tval == TV_GLOVES && o_ptr->sval == SV_SET_OF_DRAGON_GLOVES) 
       || (o_ptr->tval == TV_BOOTS && o_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE) 
       || o_ptr->art_name || o_ptr->name1) && object_known_p(o_ptr))
    {
      int i = 0;
      object_desc(o_name, o_ptr, TRUE, 0);

      while ( o_name[i] && i < 26 ){
#ifdef JP
	if (iskanji(o_name[i])) i++;
#endif
	i++;
      }
      if(i<28) while(i<28){o_name[i]=' ';i++;}
      o_name[i]=0;
      
      fprintf(fff,"%s %s", where, o_name);

      if (!(o_ptr->ident & (IDENT_MENTAL))) 
	{
#ifdef JP
	  fprintf(fff, "-------����--------------- -------����---------\n");
#else
	  fprintf(fff, "-------unknown------------ -------unknown------\n");
#endif
	}
      else {
	object_flags_known(o_ptr, &f[0], &f[1], &f[2]);
      
#ifdef JP
	if (f[1] & TR2_IM_ACID) fprintf(fff,"��");
	else if (f[1] & TR2_RES_ACID) fprintf(fff,"��");
	else fprintf(fff,"��");

	if (f[1] & TR2_IM_ELEC) fprintf(fff,"��");
	else if (f[1] & TR2_RES_ELEC) fprintf(fff,"��");
	else fprintf(fff,"��");

	if (f[1] & TR2_IM_FIRE) fprintf(fff,"��");
	else if (f[1] & TR2_RES_FIRE) fprintf(fff,"��");
	else fprintf(fff,"��");

	if (f[1] & TR2_IM_COLD) fprintf(fff,"��");
	else if (f[1] & TR2_RES_COLD) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_POIS) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_LITE) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_DARK) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_SHARDS) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_SOUND) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_NETHER) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_NEXUS) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_CHAOS) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_DISEN) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	fprintf(fff," ");
	
	if (f[1] & TR2_RES_BLIND) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_FEAR) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_RES_CONF) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_FREE_ACT) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[2] & TR3_SEE_INVIS) fprintf(fff,"��");
	else fprintf(fff,"��");
	
	if (f[1] & TR2_HOLD_LIFE) fprintf(fff,"��");
	else fprintf(fff,"��");

	if (f[2] & TR3_TELEPATHY) fprintf(fff,"��");
	else fprintf(fff,"��");

	if (f[2] & TR3_SLOW_DIGEST) fprintf(fff,"��");
	else fprintf(fff,"��");


	if (f[2] & TR3_REGEN) fprintf(fff,"��");
	else fprintf(fff,"��");

	if (f[2] & TR3_FEATHER) fprintf(fff,"��");
	else fprintf(fff,"��");
#else
	if (f[1] & TR2_IM_ACID) fprintf(fff,"* ");
	else if (f[1] & TR2_RES_ACID) fprintf(fff,"+ ");
	else fprintf(fff,". ");

	if (f[1] & TR2_IM_ELEC) fprintf(fff,"* ");
	else if (f[1] & TR2_RES_ELEC) fprintf(fff,"+ ");
	else fprintf(fff,". ");

	if (f[1] & TR2_IM_FIRE) fprintf(fff,"* ");
	else if (f[1] & TR2_RES_FIRE) fprintf(fff,"+ ");
	else fprintf(fff,". ");

	if (f[1] & TR2_IM_COLD) fprintf(fff,"* ");
	else if (f[1] & TR2_RES_COLD) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_POIS) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_LITE) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_DARK) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_SHARDS) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_SOUND) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_NETHER) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_NEXUS) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_CHAOS) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_DISEN) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	fprintf(fff," ");
	
	if (f[1] & TR2_RES_BLIND) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_FEAR) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_RES_CONF) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_FREE_ACT) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[2] & TR3_SEE_INVIS) fprintf(fff,"+ ");
	else fprintf(fff,". ");
	
	if (f[1] & TR2_HOLD_LIFE) fprintf(fff,"+ ");
	else fprintf(fff,". ");

	if (f[2] & TR3_TELEPATHY) fprintf(fff,"+ ");
	else fprintf(fff,". ");

	if (f[2] & TR3_SLOW_DIGEST) fprintf(fff,"+ ");
	else fprintf(fff,". ");


	if (f[2] & TR3_REGEN) fprintf(fff,"+ ");
	else fprintf(fff,". ");

	if (f[2] & TR3_FEATHER) fprintf(fff,"+ ");
	else fprintf(fff,". ");
#endif	
	fprintf(fff,"\n");
      }
      (*j)++;
      if(*j==9)
	{ 
	  *j=0;
	  fprintf(fff,"%s\n", inven_res_label);
	}
    }
}

/*
 * Display *ID* ed weapons/armors's resistances
 */
static void do_cmd_knowledge_inven(void)
{

	FILE *fff;

	char file_name[1024];
 
	store_type  *st_ptr;
	object_type *o_ptr;

	int tval;
	int i=0;
        int j=0;

	char  where[32];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporally file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	fprintf(fff,"%s\n",inven_res_label);

        for (tval=TV_BOW; tval <= TV_RING; tval++){

          if (j!=0) {
              for (;j<9;j++) fprintf(fff, "\n");
              j=0;
	      fprintf(fff,"%s\n",inven_res_label);              
	  }
	  
#ifdef JP
	  strcpy(where, "��");
#else
	  strcpy(where, "E ");
#endif
	  for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	    {
	      o_ptr = &inventory[i];
	      do_cmd_knowledge_inven_aux(fff, o_ptr, &j, tval, where);
	    }
	  
#ifdef JP
	  strcpy(where, "��");
#else
	  strcpy(where, "I ");
#endif
	  for (i = 0; i < INVEN_PACK; i++)
	    {
	      o_ptr = &inventory[i];
	      do_cmd_knowledge_inven_aux(fff, o_ptr, &j, tval, where);
	    }
	  
	  
	  /* Print all homes in the different towns */
	  st_ptr = &town[1].store[STORE_HOME];
#ifdef JP
	  strcpy(where, "��");
#else
	  strcpy(where, "H ");/*nanka*/
#endif
	      
	  /* Dump all available items */
	  for (i = 0; i < st_ptr->stock_num; i++)
	    {
	      o_ptr = &st_ptr->stock[i];
	      do_cmd_knowledge_inven_aux(fff, o_ptr, &j, tval, where);
	    }
	}
	  
	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "*����*�Ѥ����/�ɶ�������ꥹ��", 0, 0);
#else
	show_file(TRUE, file_name, "Resistances of *identifyed* equipment", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}


void do_cmd_save_screen_html_aux(char *filename, int message)
{
	int y, x, i;

	byte a = 0, old_a = 0;
	char c = ' ';

	FILE *fff, *tmpfff;
	char buf[2048];

	int yomikomu = 0;
	char *tags[4] = {
		"HEADER_START:",
		"HEADER_END:",
		"FOOTER_START:",
		"FOOTER_END:",
	};

	char *html_head[] = {
		"<html>\n<body text=\"#ffffff\" bgcolor=\"#000000\">\n",
		"<pre>",
		0,
	};
	char *html_foot[] = {
		"</pre>\n",
		"</body>\n</html>\n",
		0,
	};

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Hack -- drop permissions */
	safe_setuid_drop();

	/* Append to the file */
	fff = my_fopen(filename, "w");

	/* Hack -- grab permissions */
	safe_setuid_grab();

	/* Oops */
	if (!fff) {
		if (message) {
#ifdef JP
		    msg_format("�ե����� %s �򳫤��ޤ���Ǥ�����", filename);
#else
		    msg_format("Failed to open file %s.", filename);
#endif
		    msg_print(NULL);
		}
	    return;
	}

	/* Save the screen */
	if (message)
	screen_save();

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, "htmldump.prf");
	tmpfff = my_fopen(buf, "r");
	if (!tmpfff) {
		for (i = 0; html_head[i]; i++)
			fprintf(fff, html_head[i]);
	}
	else {
		yomikomu = 0;
		while (!my_fgets(tmpfff, buf, 1024)) {
			if (!yomikomu) {
				if (strncmp(buf, tags[0], strlen(tags[0])) == 0)
					yomikomu = 1;
			}
			else {
				if (strncmp(buf, tags[1], strlen(tags[1])) == 0)
					break;
				fprintf(fff, "%s\n", buf);
			}
		}
	}

	/* Dump the screen */
	for (y = 0; y < 24; y++)
	{
		/* Start the row */
		if (y != 0)
			fprintf(fff, "\n");

		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			int rv, gv, bv;
			char *cc = NULL;
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			switch (c)
			{
			case '&': cc = "&amp;"; break;
			case '<': cc = "&lt;"; break;
			case '>': cc = "&gt;"; break;
#ifdef WINDOWS
			case 0x1f: c = '.'; break;
			case 0x7f: c = (a == 0x09) ? '%' : '#'; break;
#endif
			}

			a = a & 0x0F;
			if ((y == 0 && x == 0) || a != old_a) {
				rv = angband_color_table[a][1];
				gv = angband_color_table[a][2];
				bv = angband_color_table[a][3];
				fprintf(fff, "%s<font color=\"#%02x%02x%02x\">", 
					((y == 0 && x == 0) ? "" : "</font>"), rv, gv, bv);
				old_a = a;
			}
			if (cc)
				fprintf(fff, "%s", cc);
			else
				fprintf(fff, "%c", c);
		}
	}
	fprintf(fff, "</font>");

	if (!tmpfff) {
		for (i = 0; html_foot[i]; i++)
			fprintf(fff, html_foot[i]);
		}
	else {
		rewind(tmpfff);
		yomikomu = 0;
		while (!my_fgets(tmpfff, buf, 1024)) {
			if (!yomikomu) {
				if (strncmp(buf, tags[2], strlen(tags[2])) == 0)
					yomikomu = 1;
			}
			else {
				if (strncmp(buf, tags[3], strlen(tags[3])) == 0)
					break;
				fprintf(fff, "%s\n", buf);
			}
		}
		my_fclose(tmpfff);
	}

	/* Skip a line */
	fprintf(fff, "\n");

	/* Close it */
	my_fclose(fff);

	/* Message */
	if (message) {
#ifdef JP
	msg_print("����(��ǰ����)��ե�����˽񤭽Ф��ޤ�����");
#else
		msg_print("Screen dump saved.");
#endif
		msg_print(NULL);
	}

	/* Restore the screen */
	if (message)
	screen_load();
}

/*
 * Hack -- save a screen dump to a file
 */
void do_cmd_save_screen_html(void)
{
	char buf[1024], tmp[256] = "screen.html";

	if (!get_string("�ե�����̾: ", tmp, 80))
		return;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, tmp);

	msg_print(NULL);

	do_cmd_save_screen_html_aux(buf, 1);
}


/*
 * Redefinable "save_screen" action
 */
void (*screendump_aux)(void) = NULL;


/*
 * Hack -- save a screen dump to a file
 */
void do_cmd_save_screen(void)
{
#ifdef JP
	if (get_check("HTML�ǽ��Ϥ��ޤ�����"))
#else
	if (get_check("Save screen dump as HTML? "))
#endif
	{
		do_cmd_save_screen_html();
		return;
	}

	/* Do we use a special screendump function ? */
	if (screendump_aux)
	{
		/* Dump the screen to a graphics file */
		(*screendump_aux)();
	}
	else /* Dump the screen as text */
	{
		int y, x;

		byte a = 0;
		char c = ' ';

		FILE *fff;

		char buf[1024];


		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_USER, "dump.txt");

		/* File type is "TEXT" */
		FILE_TYPE(FILE_TYPE_TEXT);

		/* Hack -- drop permissions */
		safe_setuid_drop();

		/* Append to the file */
		fff = my_fopen(buf, "w");

		/* Hack -- grab permissions */
		safe_setuid_grab();

		/* Oops */
		if (!fff) {
#ifdef JP
		    msg_format("�ե����� %s �򳫤��ޤ���Ǥ�����", buf);
#else
		    msg_format("Failed to open file %s.", buf);
#endif
		    msg_print(NULL);
		    return;
		}


		/* Save the screen */
		screen_save();


		/* Dump the screen */
		for (y = 0; y < 24; y++)
		{
			/* Dump each row */
			for (x = 0; x < 79; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x, y, &a, &c));

				/* Dump it */
				buf[x] = c;
			}

			/* Terminate */
			buf[x] = '\0';

			/* End the row */
			fprintf(fff, "%s\n", buf);
		}

		/* Skip a line */
		fprintf(fff, "\n");


		/* Dump the screen */
		for (y = 0; y < 24; y++)
		{
			/* Dump each row */
			for (x = 0; x < 79; x++)
			{
				/* Get the attr/char */
				(void)(Term_what(x, y, &a, &c));

				/* Dump it */
				buf[x] = hack[a&0x0F];
			}

			/* Terminate */
			buf[x] = '\0';

			/* End the row */
			fprintf(fff, "%s\n", buf);
		}

		/* Skip a line */
		fprintf(fff, "\n");


		/* Close it */
		my_fclose(fff);


		/* Message */
#ifdef JP
	msg_print("����(��ǰ����)��ե�����˽񤭽Ф��ޤ�����");
#else
		msg_print("Screen dump saved.");
#endif

		msg_print(NULL);


		/* Restore the screen */
		screen_load();
	}
}


/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
static bool ang_sort_art_comp(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = a_info[w1].tval;
		z2 = a_info[w2].tval;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = a_info[w1].sval;
		z2 = a_info[w2].sval;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = a_info[w1].level;
		z2 = a_info[w2].level;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
static void ang_sort_art_swap(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}


/*
 * Check the status of "artifacts"
 */
void do_cmd_knowledge_artifacts(void)
{
	int i, k, z, x, y, n = 0;
	u16b why = 3;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	char base_name[MAX_NLEN];

	bool *okay;

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Allocate the "okay" array */
	C_MAKE(okay, max_a_idx, bool);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Scan the artifacts */
	for (k = 0; k < max_a_idx; k++)
	{
		artifact_type *a_ptr = &a_info[k];

		/* Default */
		okay[k] = FALSE;

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Skip "uncreated" artifacts */
		if (!a_ptr->cur_num) continue;

		/* Assume okay */
		okay[k] = TRUE;
	}

	/* Check the dungeon */
	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			s16b this_o_idx, next_o_idx = 0;

			/* Scan all objects in the grid */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Ignore non-artifacts */
				if (!artifact_p(o_ptr)) continue;

				/* Ignore known items */
				if (object_known_p(o_ptr)) continue;

				/* Note the artifact */
				okay[o_ptr->name1] = FALSE;
			}
		}
	}

	/* Check the inventory and equipment */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		/* Ignore non-objects */
		if (!o_ptr->k_idx) continue;

		/* Ignore non-artifacts */
		if (!artifact_p(o_ptr)) continue;

		/* Ignore known items */
		if (object_known_p(o_ptr)) continue;

		/* Note the artifact */
		okay[o_ptr->name1] = FALSE;
	}

	for (k = 0; k < max_a_idx; k++)
	{
		if (okay[k]) who[n++] = k;
	}

        /* Select the sort method */
        ang_sort_comp = ang_sort_art_comp;
        ang_sort_swap = ang_sort_art_swap;

        /* Sort the array by dungeon depth of monsters */
        ang_sort(who, &why, n);

	/* Scan the artifacts */
	for (k = 0; k < n; k++)
	{
		artifact_type *a_ptr = &a_info[who[k]];

		/* Paranoia */
#ifdef JP
strcpy(base_name, "̤�Τ�����Υ����ƥ�");
#else
		strcpy(base_name, "Unknown Artifact");
#endif


		/* Obtain the base object type */
		z = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Real object */
		if (z)
		{
			object_type forge;
			object_type *q_ptr;

			/* Get local object */
			q_ptr = &forge;

			/* Create fake object */
			object_prep(q_ptr, z);

			/* Make it an artifact */
			q_ptr->name1 = who[k];

			/* Describe the artifact */
			object_desc_store(base_name, q_ptr, FALSE, 0);
		}

		/* Hack -- Build the artifact name */
#ifdef JP
		fprintf(fff, "     %s\n", base_name);
#else
		fprintf(fff, "     The %s\n", base_name);
#endif

	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���Τ�����Υ����ƥ�", 0, 0);
#else
	show_file(TRUE, file_name, "Artifacts Seen", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known uniques
 */
static void do_cmd_knowledge_uniques(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = i;
	}

        /* Select the sort method */
        ang_sort_comp = ang_sort_comp_hook;
        ang_sort_swap = ang_sort_swap_hook;

        /* Sort the array by dungeon depth of monsters */
        ang_sort(who, &why, n);

	/* Scan the monster races */
	for (k = 0; k < n; k++)
	{
		monster_race *r_ptr = &r_info[who[k]];

		/* Only print Uniques */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			bool dead = (r_ptr->max_num == 0);

			if (dead) continue;

			/* Only display "known" uniques */
			if (dead || cheat_know || r_ptr->r_sights)
			{
				/* Print a message */
#ifdef JP
				fprintf(fff, "     %s�Ϥޤ������Ƥ��롣\n",
				        (r_name + r_ptr->name));
#else
				fprintf(fff, "     %s is alive\n",
				        (r_name + r_ptr->name));
#endif

			}
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�ޤ������Ƥ����ˡ�������󥹥���", 0, 0);
#else
	show_file(TRUE, file_name, "Alive Uniques", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display dead uniques
 */
static void do_cmd_knowledge_uniques_dead(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = i;
	}

        /* Select the sort method */
        ang_sort_comp = ang_sort_comp_hook;
        ang_sort_swap = ang_sort_swap_hook;

        /* Sort the array by dungeon depth of monsters */
        ang_sort(who, &why, n);

	/* Scan the monster races */
	for (k = 0; k < n; k++)
	{
		monster_race *r_ptr = &r_info[who[k]];

		/* Only print Uniques */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			bool dead = (r_ptr->max_num == 0);

			if (!dead) continue;

			/* Only display "known" uniques */
			if (dead || cheat_know || r_ptr->r_sights)
			{
				/* Print a message */
#ifdef JP
				fprintf(fff, "     %s�ϴ��˻��Ǥ��롣\n",
				        (r_name + r_ptr->name));
#else
				fprintf(fff, "     %s is dead\n",
				        (r_name + r_ptr->name));
#endif

			}
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "�ݤ�����ˡ�������󥹥���", 0, 0);
#else
	show_file(TRUE, file_name, "Dead Uniques", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display weapon-exp
 */
static void do_cmd_knowledge_weapon_exp(void)
{
	int i,j, num, shougou;

	FILE *fff;

	char file_name[1024];
	char tmp[30];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	for(i = 0; i < 5; i++)
	{
		for (num = 0; num < 64; num++)
		{
			for (j = 0; j < max_k_idx; j++)
			{
				object_kind *k_ptr = &k_info[j];

				if ((k_ptr->tval == TV_SWORD-i) && (k_ptr->sval == num))
				{
					if((k_ptr->tval == TV_BOW) && (k_ptr->sval == SV_CRIMSON)) continue;

					if(weapon_exp[4-i][num]<4000) shougou=0;
					else if(weapon_exp[4-i][num]<6000) shougou=1;
					else if(weapon_exp[4-i][num]<7000) shougou=2;
					else if(weapon_exp[4-i][num]<8000) shougou=3;
					else shougou=4;
					strip_name(tmp, j);
					fprintf(fff,"%-25s ",tmp);
					if (weapon_exp[4-i][num] >= weapon_exp_settei[p_ptr->pclass][4-i][num][1]) fprintf(fff,"!");
					else fprintf(fff," ");
					fprintf(fff,"%s",shougou_moji[shougou]);
					if (cheat_xtra) fprintf(fff," %d",weapon_exp[4-i][num]);
					fprintf(fff,"\n");
					break;
				}
			}
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���ηи���", 0, 0);
#else
	show_file(TRUE, file_name, "Weapon Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display spell-exp
 */
static void do_cmd_knowledge_spell_exp(void)
{
	int i=0, shougou;

	FILE *fff;
	magic_type *s_ptr;

	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	if(p_ptr->realm1 != REALM_NONE)
	{
		fprintf(fff,"%s����ˡ��\n",realm_names[p_ptr->realm1]);
		for (i = 0; i < 32; i++)
		{
			if (!is_magic(p_ptr->realm1))
			{
				s_ptr = &technic_info[p_ptr->realm1 - MIN_TECHNIC - 1][i];
			}
			else
			{
				s_ptr = &mp_ptr->info[p_ptr->realm1 - 1][i];
			}
			if(s_ptr->slevel == 99) continue;
			if(spell_exp[i]<900) shougou=0;
			else if(spell_exp[i]<1200) shougou=1;
			else if(spell_exp[i]<1400) shougou=2;
			else if(spell_exp[i]<1600) shougou=3;
			else shougou=4;
			fprintf(fff,"%-25s ",spell_names[technic2magic(p_ptr->realm1)-1][i]);
			if (p_ptr->realm1 == REALM_HISSATSU)
				fprintf(fff,"[--]");
			else
			{
				if (shougou == 4) fprintf(fff,"!");
				else fprintf(fff," ");
				fprintf(fff,"%s",shougou_moji[shougou]);
			}
			if (cheat_xtra) fprintf(fff," %d",spell_exp[i]);
			fprintf(fff,"\n");
		}
	}

	if(p_ptr->realm2 != REALM_NONE)
	{
		fprintf(fff,"\n%s����ˡ��\n",realm_names[p_ptr->realm2]);
		for (i = 0; i < 32; i++)
		{
			if (!is_magic(p_ptr->realm1))
			{
				s_ptr = &technic_info[p_ptr->realm2 - MIN_TECHNIC - 1][i];
			}
			else
			{
				s_ptr = &mp_ptr->info[p_ptr->realm2 - 1][i];
			}
			if(s_ptr->slevel == 99) continue;

			if(spell_exp[i+32]<900) shougou=0;
			else if(spell_exp[i+32]<1200) shougou=1;
			else if(spell_exp[i+32]<1400) shougou=2;
			else shougou=3;
			fprintf(fff,"%-25s ",spell_names[technic2magic(p_ptr->realm2)-1][i]);
			if (shougou == 3) fprintf(fff,"!");
			else fprintf(fff," ");
			fprintf(fff,"%s",shougou_moji[shougou]);
			if (cheat_xtra) fprintf(fff," %d",spell_exp[i+32]);
			fprintf(fff,"\n");
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "��ˡ�ηи���", 0, 0);
#else
	show_file(TRUE, file_name, "Spell Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display skill-exp
 */
static void do_cmd_knowledge_skill_exp(void)
{
	int i=0, shougou;

	FILE *fff;

	char file_name[1024];
#ifdef JP
	char skill_name[3][17]={"�ޡ�����륢����", "����ή          ", "����            "};
#else
	char skill_name[3][20]={"Martial Arts    ", "Dual Wielding   ", "Riding          "};
#endif

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	for (i = 0; i < 3; i++)
	{
		if(i == GINOU_RIDING)
		{
			if(skill_exp[i]<500) shougou=0;
			else if(skill_exp[i]<2000) shougou=1;
			else if(skill_exp[i]<5000) shougou=2;
			else if(skill_exp[i]<8000) shougou=3;
			else shougou=4;
		}
		else
		{
			if(skill_exp[i]<4000) shougou=0;
			else if(skill_exp[i]<6000) shougou=1;
			else if(skill_exp[i]<7000) shougou=2;
			else if(skill_exp[i]<8000) shougou=3;
			else shougou=4;
		}
		fprintf(fff,"%-20s ",skill_name[i]);
		if (skill_exp[i] == skill_exp_settei[p_ptr->pclass][i][1]) fprintf(fff,"!");
		else fprintf(fff," ");
		fprintf(fff,"%s",shougou_moji[shougou]);
		if (cheat_xtra) fprintf(fff," %d",skill_exp[i]);
		fprintf(fff,"\n");
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "��ǽ�ηи���", 0, 0);
#else
	show_file(TRUE, file_name, "Miscellaneous Proficiency", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Pluralize a monster name
 */
void plural_aux(char *Name)
{
	int NameLen = strlen(Name);

	if (strstr(Name, "Disembodied hand"))
	{
		strcpy(Name, "Disembodied hands that strangled people");
	}
	else if (strstr(Name, "Colour out of space"))
	{
		strcpy(Name, "Colours out of space");
	}
	else if (strstr(Name, "stairway to hell"))
	{
		strcpy(Name, "stairways to hell");
	}
	else if (strstr(Name, "Dweller on the threshold"))
	{
		strcpy(Name, "Dwellers on the threshold");
	}
	else if (strstr(Name, " of "))
	{
		cptr aider = strstr(Name, " of ");
		char dummy[80];
		int i = 0;
		cptr ctr = Name;

		while (ctr < aider)
		{
			dummy[i] = *ctr;
			ctr++; i++;
		}

		if (dummy[i-1] == 's')
		{
			strcpy(&(dummy[i]), "es");
			i++;
		}
		else
		{
			strcpy(&(dummy[i]), "s");
		}

		strcpy(&(dummy[i+1]), aider);
		strcpy(Name, dummy);
	}
	else if (strstr(Name, "coins"))
	{
		char dummy[80];
		strcpy(dummy, "piles of ");
		strcat(dummy, Name);
		strcpy(Name, dummy);
		return;
	}
	else if (strstr(Name, "Manes"))
	{
		return;
	}
	else if (streq(&(Name[NameLen - 2]), "ey"))
	{
		strcpy(&(Name[NameLen - 2]), "eys");
	}
	else if (Name[NameLen - 1] == 'y')
	{
		strcpy(&(Name[NameLen - 1]), "ies");
	}
	else if (streq(&(Name[NameLen - 4]), "ouse"))
	{
		strcpy(&(Name[NameLen - 4]), "ice");
	}
	else if (streq(&(Name[NameLen - 2]), "us"))
	{
		strcpy(&(Name[NameLen - 2]), "i");
	}
	else if (streq(&(Name[NameLen - 6]), "kelman"))
	{
		strcpy(&(Name[NameLen - 6]), "kelmen");
	}
	else if (streq(&(Name[NameLen - 8]), "wordsman"))
	{
		strcpy(&(Name[NameLen - 8]), "wordsmen");
	}
	else if (streq(&(Name[NameLen - 7]), "oodsman"))
	{
		strcpy(&(Name[NameLen - 7]), "oodsmen");
	}
	else if (streq(&(Name[NameLen - 7]), "eastman"))
	{
		strcpy(&(Name[NameLen - 7]), "eastmen");
	}
	else if (streq(&(Name[NameLen - 8]), "izardman"))
	{
		strcpy(&(Name[NameLen - 8]), "izardmen");
	}
	else if (streq(&(Name[NameLen - 5]), "geist"))
	{
		strcpy(&(Name[NameLen - 5]), "geister");
	}
	else if (streq(&(Name[NameLen - 2]), "ex"))
	{
		strcpy(&(Name[NameLen - 2]), "ices");
	}
	else if (streq(&(Name[NameLen - 2]), "lf"))
	{
		strcpy(&(Name[NameLen - 2]), "lves");
	}
	else if (suffix(Name, "ch") ||
	         suffix(Name, "sh") ||
			 suffix(Name, "nx") ||
			 suffix(Name, "s") ||
			 suffix(Name, "o"))
	{
		strcpy(&(Name[NameLen]), "es");
	}
	else
	{
		strcpy(&(Name[NameLen]), "s");
	}
}

/*
 * Display current pets
 */
static void do_cmd_knowledge_pets(void)
{
	int             i;
	FILE            *fff;
	monster_type    *m_ptr;
	int             t_friends = 0;
	int             show_upkeep = 0;
	char            file_name[1024];


	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Process the monsters (backwards) */
	for (i = m_max - 1; i >= 1; i--)
	{
		monster_race *r_ptr;
		/* Access the monster */
		m_ptr = &m_list[i];

		/* Ignore "dead" monsters */
		if (!m_ptr->r_idx) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		/* Calculate "upkeep" for pets */
		if (is_pet(m_ptr))
		{
			char pet_name[80];
			t_friends++;
			monster_desc(pet_name, m_ptr, 0x88);
			fprintf(fff, "%s (%s)", pet_name, look_mon_desc(i));
			if (p_ptr->riding == i)
#ifdef JP
				fprintf(fff, " ������");
#else
				fprintf(fff, " Riding");
#endif
			fprintf(fff, "\n");
		}
	}

	show_upkeep = calculate_upkeep();

	fprintf(fff, "----------------------------------------------\n");
#ifdef JP
	fprintf(fff, "    ���: %d ɤ�Υڥå�\n", t_friends);
	fprintf(fff, " �ݻ�������: %d%% MP\n", show_upkeep);
#else
	fprintf(fff, "   Total: %d pet%s.\n",
	        t_friends, (t_friends == 1 ? "" : "s"));
	fprintf(fff, "   Upkeep: %d%% mana.\n", show_upkeep);
#endif



	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "���ߤΥڥå�", 0, 0);
#else
	show_file(TRUE, file_name, "Current Pets", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Total kill count
 *
 * Note that the player ghosts are ignored.  XXX XXX XXX
 */
static void do_cmd_knowledge_kill_count(void)
{
	int i, k, n = 0;
	u16b why = 2;
	s16b *who;

	FILE *fff;

	char file_name[1024];

	s32b Total = 0;


	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, s16b);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	{
		/* Monsters slain */
		int kk;

		for (kk = 1; kk < max_r_idx; kk++)
		{
			monster_race *r_ptr = &r_info[kk];

			if (r_ptr->flags1 & (RF1_UNIQUE))
			{
				bool dead = (r_ptr->max_num == 0);

				if (dead)
				{
					Total++;
				}
			}
			else
			{
				s16b This = r_ptr->r_pkills;

				if (This > 0)
				{
					Total += This;
				}
			}
		}

		if (Total < 1)
#ifdef JP
			fprintf(fff,"���ʤ��Ϥޤ�Ũ���ݤ��Ƥ��ʤ���\n\n");
#else
			fprintf(fff,"You have defeated no enemies yet.\n\n");
#endif

		else if (Total == 1)
#ifdef JP
			fprintf(fff,"���ʤ��ϰ�ɤ��Ũ���ݤ��Ƥ��롣\n\n");
#else
			fprintf(fff,"You have defeated one enemy.\n\n");
#endif

		else
#ifdef JP
			fprintf(fff,"���ʤ��� %lu ɤ��Ũ���ݤ��Ƥ��롣\n\n", Total);
#else
			fprintf(fff,"You have defeated %lu enemies.\n\n", Total);
#endif

	}

	Total = 0;

	/* Scan the monsters */
	for (i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Use that monster */
		if (r_ptr->name) who[n++] = i;
	}

        /* Select the sort method */
        ang_sort_comp = ang_sort_comp_hook;
        ang_sort_swap = ang_sort_swap_hook;

        /* Sort the array by dungeon depth of monsters */
        ang_sort(who, &why, n);

	/* Scan the monster races */
	for (k = 0; k < n; k++)
	{
		monster_race *r_ptr = &r_info[who[k]];

		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			bool dead = (r_ptr->max_num == 0);

			if (dead)
			{
				/* Print a message */
				fprintf(fff, "     %s\n",
				    (r_name + r_ptr->name));
				Total++;
			}
		}
		else
		{
			s16b This = r_ptr->r_pkills;

			if (This > 0)
			{
#ifdef JP
/* p,t�Ͽͤȿ����� by ita*/
if(strchr("pt",r_ptr->d_char))
fprintf(fff, "     %3d �ͤ� %s\n", This, r_name + r_ptr->name);
else
fprintf(fff, "     %3d ɤ�� %s\n", This, r_name + r_ptr->name);
#else
				if (This < 2)
				{
					if (strstr(r_name + r_ptr->name, "coins"))
					{
						fprintf(fff, "     1 pile of %s\n", (r_name + r_ptr->name));
					}
					else
					{
						fprintf(fff, "     1 %s\n", (r_name + r_ptr->name));
					}
				}
				else
				{
					char ToPlural[80];
					strcpy(ToPlural, (r_name + r_ptr->name));
					plural_aux(ToPlural);
					fprintf(fff, "     %d %s\n", This, ToPlural);
				}
#endif


				Total += This;
			}
		}
	}

	fprintf(fff,"----------------------------------------------\n");
#ifdef JP
	fprintf(fff,"    ���: %lu ɤ���ݤ�����\n", Total);
#else
	fprintf(fff,"   Total: %lu creature%s killed.\n",
	        Total, (Total == 1 ? "" : "s"));
#endif


	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "�ݤ���Ũ�ο�", 0, 0);
#else
	show_file(TRUE, file_name, "Kill Count", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
 * Display known objects
 */
static void do_cmd_knowledge_objects(void)
{
	int k;

	FILE *fff;

	char o_name[MAX_NLEN];

	char file_name[1024];


	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

	/* Scan the object kinds */
	for (k = 1; k < max_k_idx; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Hack -- skip artifacts */
		if (k_ptr->flags3 & (TR3_INSTA_ART)) continue;

		/* List known flavored objects */
		if (k_ptr->flavor && k_ptr->aware)
		{
			object_type *i_ptr;
			object_type object_type_body;

			/* Get local object */
			i_ptr = &object_type_body;

			/* Create fake object */
			object_prep(i_ptr, k);

			/* Describe the object */
			object_desc_store(o_name, i_ptr, FALSE, 0);

			/* Print a message */
			fprintf(fff, "     %s\n", o_name);
		}
	}

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "���ΤΥ����ƥ�", 0, 0);
#else
	show_file(TRUE, file_name, "Known Objects", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


/*
* List virtues & status
*
*/
void do_cmd_knowledge_kubi(void)
{
	int i;
	FILE *fff;
	
	char file_name[1024];
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
#ifdef JP
		fprintf(fff, "�����Υ������å� : %s\n", (p_ptr->today_mon ? r_name + r_info[p_ptr->today_mon].name : "����"));
		fprintf(fff, "\n");
		fprintf(fff, "�޶��ꥹ��\n");
#else
		fprintf(fff, "Today target : %s\n", (p_ptr->today_mon ? r_name + r_info[p_ptr->today_mon].name : "unknown"));
		fprintf(fff, "\n");
		fprintf(fff, "List of wanted monsters\n");
#endif
		for (i = 0; i < MAX_KUBI; i++)
		{
			fprintf(fff,"%-40s ---- ",r_name + r_info[(kubi_r_idx[i] > 10000 ? kubi_r_idx[i] - 10000 : kubi_r_idx[i])].name);
			if (kubi_r_idx[i] > 10000)
#ifdef JP
				fprintf(fff, "��\n");
#else
				fprintf(fff, "done\n");
#endif
			else
				fprintf(fff, "$%d\n", 300 * (r_info[kubi_r_idx[i]].level + 1));
		}
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "�޶��ΰ���", 0, 0);
#else
	show_file(TRUE, file_name, "Wanted monsters", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
* List virtues & status
*
*/
void do_cmd_knowledge_virtues(void)
{
	FILE *fff;
	
	char file_name[1024];
	cptr disp_align;
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
#ifdef JP
		if (p_ptr->align > 150) disp_align = "����";
		else if (p_ptr->align > 50) disp_align = "����";
		else if (p_ptr->align > 10) disp_align = "����";
		else if (p_ptr->align > -11) disp_align = "��Ω";
		else if (p_ptr->align > -51) disp_align = "����";
		else if (p_ptr->align > -151) disp_align = "�氭";
		else disp_align = "�簭";
		fprintf(fff, "���ߤ�°�� : %s\n", disp_align);
		fprintf(fff, "\n");
#else
		if (p_ptr->align > 150) disp_align = "lawful";
		else if (p_ptr->align > 50) disp_align = "good";
		else if (p_ptr->align > 10) disp_align = "neutral good";
		else if (p_ptr->align > -11) disp_align = "neutral";
		else if (p_ptr->align > -51) disp_align = "neutral evil";
		else if (p_ptr->align > -151) disp_align = "evil";
		else disp_align = "chaotic";
		fprintf(fff, "Your alighnment : %s\n", disp_align);
		fprintf(fff, "\n");
#endif
		dump_virtues(fff);
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "Ȭ�Ĥ���", 0, 0);
#else
	show_file(TRUE, file_name, "Virtues", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
* Dungeon
*
*/
void do_cmd_knowledge_dungeon(void)
{
	FILE *fff;
	
	char file_name[1024];
	int i;
	
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
		for (i = 1; i < max_d_idx; i++)
		{
			bool seiha = FALSE;

			if (!d_info[i].maxdepth) continue;
			if (!max_dlv[i]) continue;
			if (d_info[i].final_guardian)
			{
				if (!r_info[d_info[i].final_guardian].max_num) seiha = TRUE;
			}
			else if (max_dlv[i] == d_info[i].maxdepth) seiha = TRUE;
			fprintf(fff,"%c%-12s :  %3d ��\n", seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
		}
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "���ޤǤ����ä����󥸥��", 0, 0);
#else
	show_file(TRUE, file_name, "Dungeon", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
* List virtues & status
*
*/
static void do_cmd_knowledge_stat(void)
{
	FILE *fff;
	
	char file_name[1024];
	int percent, v_nr;
	
	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
		percent = (int)(((long)player_hp[PY_MAX_LEVEL - 1] * 200L) /
			(2 * p_ptr->hitdie +
			((PY_MAX_LEVEL - 1+3) * (p_ptr->hitdie + 1))));

#ifdef JP
if (p_ptr->knowledge & KNOW_HPRATE) fprintf(fff, "���ߤ����ϥ�� : %d/100\n\n", percent);
else fprintf(fff, "���ߤ����ϥ�� : ???\n\n");
fprintf(fff, "ǽ�Ϥκ�����\n\n");
#else
		if (p_ptr->knowledge & KNOW_HPRATE) fprintf(fff, "Your current Life Rating is %d/100.\n\n", percent);
		else fprintf(fff, "Your current Life Rating is ???.\n\n");
fprintf(fff, "Limits of maximum stats\n\n");
#endif
		for (v_nr = 0; v_nr < 6; v_nr++)
		{
			if ((p_ptr->knowledge & KNOW_STAT) || p_ptr->stat_max[v_nr] == p_ptr->stat_max_max[v_nr]) fprintf(fff, "%s 18/%d\n", stat_names[v_nr], p_ptr->stat_max_max[v_nr]-18);
			else fprintf(fff, "%s ???\n", stat_names[v_nr]);
		}
	}

	dump_yourself(fff);

	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "��ʬ�˴ؤ������", 0, 0);
#else
	show_file(TRUE, file_name, "HP-rate & Max stat", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
 * Print quest status of all active quests
 */
static void do_cmd_knowledge_quests(void)
{
	FILE *fff;
	char file_name[1024];
	char tmp_str[120];
	char rand_tmp_str[120] = "\0";
	char name[80];
	monster_race *r_ptr;
	int i;
	int rand_level = 100;
	int total = 0;

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}

#ifdef JP
	fprintf(fff, "�Կ����Υ������ȡ�\n");
#else
	fprintf(fff, "< Current Quest >\n");
#endif

	for (i = 1; i < max_quests; i++)
	{
		/* No info from "silent" quests */
		if (quest[i].flags & QUEST_FLAG_SILENT) continue;

		if (quest[i].status == QUEST_STATUS_TAKEN || quest[i].status == QUEST_STATUS_COMPLETED)
		{
			int old_quest;
			int j;

			/* Clear the text */
			for (j = 0; j < 10; j++)
			{
				quest_text[j][0] = '\0';
			}

			quest_text_line = 0;

			total++;

			/* Set the quest number temporary */
			old_quest = p_ptr->inside_quest;
			p_ptr->inside_quest = i;

			/* Get the quest text */
			init_flags = INIT_SHOW_TEXT;

			process_dungeon_file("q_info_j.txt", 0, 0, 0, 0);

			/* Reset the old quest number */
			p_ptr->inside_quest = old_quest;

			if (quest[i].type != QUEST_TYPE_RANDOM)
			{
				char note[80] = "\0";

				if (quest[i].status == QUEST_STATUS_TAKEN)
				{
					if (quest[i].type == QUEST_TYPE_KILL_LEVEL || quest[i].type == QUEST_TYPE_KILL_ANY_LEVEL)
					{
						r_ptr = &r_info[quest[i].r_idx];
						strcpy(name, r_name + r_ptr->name);
						if (quest[i].max_num > 1)
						{
#ifdef JP
							sprintf(note," - %d �Τ�%s���ݤ���(���� %d ��)",quest[i].max_num, name, quest[i].max_num-quest[i].cur_num);
#else
							plural_aux(name);
							sprintf(note," - kill %d %s, have killed %d.",quest[i].max_num, name, quest[i].cur_num);
#endif
						}
						else
#ifdef JP
							sprintf(note," - %s���ݤ���",name);
#else
							sprintf(note," - kill %s.",name);
#endif
					}
					else if (quest[i].type == QUEST_TYPE_KILL_NUMBER)
					{
#ifdef JP
						sprintf(note," - %d �ΤΥ�󥹥������ݤ���(���� %d ��)",quest[i].max_num, quest[i].max_num-quest[i].cur_num);
#else
						sprintf(note," - Kill %d monsters, have killed %d.",quest[i].max_num, quest[i].cur_num);
#endif
					}
					else if (quest[i].type == QUEST_TYPE_FIND_ARTIFACT)
					{
						strcpy(name, a_name + a_info[quest[i].k_idx].name);
#ifdef JP
						sprintf(note," - %s�򸫤Ĥ��Ф���", name);
#else
						sprintf(note," - Find out %s.", name);
#endif
					}
					else if (quest[i].type == QUEST_TYPE_FIND_EXIT)
#ifdef JP
						sprintf(note," - õ�����롣");
#else
						sprintf(note," - Search.");
#endif
					else if (quest[i].type == QUEST_TYPE_KILL_ALL)
#ifdef JP
						sprintf(note," - ���ƤΥ�󥹥������ݤ���");
#else
						sprintf(note," - Kill all monsters.");
#endif
				}

				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "%s (����:%d������)%s\n",
#else
				sprintf(tmp_str, "%s (Danger level: %d)%s\n",
#endif

					quest[i].name, quest[i].level, note);

				fprintf(fff, tmp_str);

				if (quest[i].status == QUEST_STATUS_COMPLETED)
				{
#ifdef JP
					sprintf(tmp_str, "  ��������ã�� - �ޤ��󽷤�����ȤäƤʤ���\n");
#else
					sprintf(tmp_str, "  Quest Completed - Unrewarded\n");
#endif


					fprintf(fff, tmp_str);
				}
				else
				{
					j = 0;

					while (quest_text[j][0] && j < 10)
					{
						fprintf(fff, "  %s\n", quest_text[j]);
						j++;
					}
				}
			}
			else if ((quest[i].type == QUEST_TYPE_RANDOM) &&
			         (quest[i].level < rand_level))
			{
				/* New random */
				rand_level = quest[i].level;

				if (max_dlv[DUNGEON_ANGBAND] >= rand_level)
				{
					/* Print the quest info */
					r_ptr = &r_info[quest[i].r_idx];
					strcpy(name, r_name + r_ptr->name);

					if (quest[i].max_num > 1)
					{
#ifdef JP
sprintf(rand_tmp_str,"%s (%d ��) - %d �Τ�%s���ݤ���(���� %d ��)\n",
	quest[i].name, quest[i].level,
	quest[i].max_num, name, quest[i].max_num-quest[i].cur_num);
#else
						plural_aux(name);

						sprintf(rand_tmp_str,"%s (Dungeon level: %d)\n  Kill %d %s, have killed %d.\n",
							quest[i].name, quest[i].level,
							quest[i].max_num, name, quest[i].cur_num);
#endif

					}
					else
					{
#ifdef JP
sprintf(rand_tmp_str,"%s (%d ��) - %s���ݤ���\n",
#else
						sprintf(rand_tmp_str,"%s (Dungeon level: %d)\n  Kill %s.\n",
#endif

							quest[i].name, quest[i].level, name);
					}
				}
			}
		}
	}

	/* Print the current random quest  */
	if (rand_tmp_str[0]) fprintf(fff, rand_tmp_str);

#ifdef JP
	if (!total) fprintf(fff, "�ʤ�\n");
#else
	if (!total) fprintf(fff, "Nothing.\n");
#endif

#ifdef JP
	fprintf(fff, "\n��ã�������������ȡ�\n");
#else
	fprintf(fff, "\n< Completed Quest >\n");
#endif
	total = 0;
	for (i = 1; i < max_quests; i++)
	{
		/* No info from "silent" quests */
		if (quest[i].flags & QUEST_FLAG_SILENT) continue;

		if (quest[i].status == QUEST_STATUS_FINISHED)
		{
			int old_quest;

			total++;

			if (i < MIN_RANDOM_QUEST)
			{
				/* Set the quest number temporary */
				old_quest = p_ptr->inside_quest;
				p_ptr->inside_quest = i;

				/* Get the quest */
				init_flags = INIT_ASSIGN;

				process_dungeon_file("q_info_j.txt", 0, 0, 0, 0);

				/* Reset the old quest number */
				p_ptr->inside_quest = old_quest;
			}

			if ((i >= MIN_RANDOM_QUEST) && quest[i].r_idx)
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "%s (%d��) - ��٥�%d\n",
#else
				sprintf(tmp_str, "%s (Dungeon level: %d) - level %d\n",
#endif

					r_name+r_info[quest[i].r_idx].name, quest[i].level, quest[i].complev);
			}
			else
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "%s (����:%d������) - ��٥�%d\n",
#else
				sprintf(tmp_str, "%s (Danger level: %d) - level %d\n",
#endif

					quest[i].name, quest[i].level, quest[i].complev);
			}

			fprintf(fff, tmp_str);
		}
	}
#ifdef JP
	if (!total) fprintf(fff, "�ʤ�\n");
#else
	if (!total) fprintf(fff, "Nothing.\n");
#endif

#ifdef JP
	fprintf(fff, "\n�Լ��Ԥ����������ȡ�\n");
#else
	fprintf(fff, "\n< Failed Quest >\n");
#endif
	total = 0;
	for (i = 1; i < max_quests; i++)
	{
		/* No info from "silent" quests */
		if (quest[i].flags & QUEST_FLAG_SILENT) continue;

		if ((quest[i].status == QUEST_STATUS_FAILED_DONE) || (quest[i].status == QUEST_STATUS_FAILED))
		{
			int old_quest;

			total++;

			if (i < MIN_RANDOM_QUEST)
			{
				/* Set the quest number temporary */
				old_quest = p_ptr->inside_quest;
				p_ptr->inside_quest = i;

				/* Get the quest text */
				init_flags = INIT_ASSIGN;

				process_dungeon_file("q_info_j.txt", 0, 0, 0, 0);

				/* Reset the old quest number */
				p_ptr->inside_quest = old_quest;
			}

			if ((i >= MIN_RANDOM_QUEST) && quest[i].r_idx)
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "%s (%d��) - ��٥�%d\n",
#else
				sprintf(tmp_str, "%s (Dungeon level: %d) - level %d\n",
#endif

					r_name+r_info[quest[i].r_idx].name, quest[i].level, quest[i].complev);
			}
			else
			{
				/* Print the quest info */
#ifdef JP
				sprintf(tmp_str, "%s (����:%d������) - ��٥�%d\n",
#else
				sprintf(tmp_str, "%s (Danger level: %d) - level %d\n",
#endif

					quest[i].name, quest[i].level, quest[i].complev);
			}
			fprintf(fff, tmp_str);
		}
	}
#ifdef JP
	if (!total) fprintf(fff, "�ʤ�\n");
#else
	if (!total) fprintf(fff, "Nothing.\n");
#endif

	if (wizard) {
#ifdef JP
	fprintf(fff, "\n�ԻĤ�Υ����९�����ȡ�\n");
#else
	fprintf(fff, "\n< Remaining Random Quest >\n");
#endif
	total = 0;
	for (i = 1; i < max_quests; i++)
	{
		/* No info from "silent" quests */
		if (quest[i].flags & QUEST_FLAG_SILENT) continue;

		if ((quest[i].type == QUEST_TYPE_RANDOM) && (quest[i].status == QUEST_STATUS_TAKEN))
		{
			total++;

			/* Print the quest info */
#ifdef JP
			sprintf(tmp_str, "%s (%d��, %s)\n",
#else
			sprintf(tmp_str, "%s (%d, %s)\n",
#endif

				quest[i].name, quest[i].level, r_name+r_info[quest[i].r_idx].name);
			fprintf(fff, tmp_str);
		}
	}
	if (!total) fprintf(fff, "�ʤ�\n");
	}	

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
	show_file(TRUE, file_name, "��������ã������", 0, 0);
#else
	show_file(TRUE, file_name, "Quest status", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}



/*
* List my home
*
*/
void do_cmd_knowledge_home(void)
{
	FILE *fff;
	
	int i, x;
	char file_name[1024];
	store_type  *st_ptr;
	char o_name[MAX_NLEN];
	cptr		paren = ")";

	process_dungeon_file("w_info_j.txt", 0, 0, max_wild_y, max_wild_x);

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);
	if (!fff) {
#ifdef JP
	    msg_format("����ե����� %s ������Ǥ��ޤ���Ǥ�����", file_name);
#else
	    msg_format("Failed to create temporary file %s.", file_name);
#endif
	    msg_print(NULL);
	    return;
	}
	
	if (fff)
	{
		/* Print all homes in the different towns */
		st_ptr = &town[1].store[STORE_HOME];

		/* Home -- if anything there */
		if (st_ptr->stock_num)
		{
			/* Header with name of the town */
#ifdef JP
			fprintf(fff, "  [ �椬�ȤΥ����ƥ� ]\n");
#else
			fprintf(fff, "  [Home Inventory]\n");
#endif
				x = 1;

			/* Dump all available items */
			for (i = 0; i < st_ptr->stock_num; i++)
			{
#ifdef JP
		                if ((i % 12) == 0) fprintf(fff, "\n ( %d �ڡ��� )\n", x++);
				object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
				if (strlen(o_name) <= 80-3)
				{
					fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
				}
				else
				{
					int n;
					char *t;
					for (n = 0, t = o_name; n < 80-3; n++, t++)
						if(iskanji(*t)) {t++; n++;}
					if (n == 81-3) n = 79-3; /* �Ǹ夬����Ⱦʬ */

					fprintf(fff, "%c%s %.*s\n", I2A(i%12), paren, n, o_name);
					fprintf(fff, "   %.77s\n", o_name+n);
				}
#else
				object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
				fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
#endif

			}

			/* Add an empty line */
			fprintf(fff, "\n\n");
		}
	}
	
	/* Close the file */
	my_fclose(fff);
	
	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "�椬�ȤΥ����ƥ�", 0, 0);
#else
	show_file(TRUE, file_name, "Home Inventory", 0, 0);
#endif

	
	/* Remove the file */
	fd_kill(file_name);
}

/*
 * Interact with "knowledge"
 */
void do_cmd_knowledge(void)
{
	int i;

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Interact until done */
	while (1)
	{
		/* Clear screen */
		Term_clear();

		/* Ask for a choice */
#ifdef JP
		prt("���ߤ��μ����ǧ����", 2, 0);
#else
		prt("Display current knowledge", 2, 0);
#endif


		/* Give some choices */
#ifdef JP
		prt("(1) ���Τ�����Υ����ƥ�                 �ΰ���", 4, 5);
		prt("(2) ���Τ������Ƥ����ˡ�������󥹥��� �ΰ���", 5, 5);
		prt("(3) �ݤ�����ˡ�������󥹥���           �ΰ���", 6, 5);
		prt("(4) ���ΤΥ����ƥ�                       �ΰ���", 7, 5);
		prt("(5) �ݤ���Ũ�ο�                         �ΰ���", 8, 5);
		prt("(6) �����Ѱ�                             �ΰ���", 9, 5);
		prt("(7) ���ߤΥڥå�                         �ΰ���", 10, 5);
		prt("(8) �¹���Υ�������                     �ΰ���", 11, 5);
		prt("(9) �ץ쥤�䡼����                       �ΰ���", 12, 5);
		prt("(a) ���ηи���                         �ΰ���", 13, 5);
		prt("(b) ��ˡ�ηи���                         �ΰ���", 14, 5);
		prt("(c) ��ǽ�ηи���                         �ΰ���", 15, 5);
		prt("(d) �޶��                               �ΰ���", 16, 5);
		prt("(e) �椬�ȤΥ����ƥ�                     �ΰ���", 17, 5);
		prt("(f) *����*�Ѥ�����������                 �ΰ���", 18, 5);
		prt("(g) ��ʬ�˴ؤ������                     �ΰ���", 19, 5);
		prt("(h) ���ä����󥸥��                     �ΰ���", 20, 5);
#else
		prt("(1) Display known artifacts", 4, 5);
		prt("(2) Display known uniques", 5, 5);
		prt("(3) Display dead uniques", 6, 5);
		prt("(4) Display known objects", 7, 5);
		prt("(5) Display kill count", 8, 5);
		prt("(6) Display mutations", 9, 5);
		prt("(7) Display current pets", 10, 5);
		prt("(8) Display current quests", 11, 5);
		prt("(9) Display virtues", 12, 5);
		prt("(a) Display weapon proficiency", 13, 5);
		prt("(b) Display spell proficiency", 14, 5);
		prt("(c) Display misc. proficiency", 15, 5);
		prt("(d) Display wanted monsters", 16, 5);
		prt("(e) Display home inventory", 17, 5);
		prt("(f) Display *identifyed* equip.", 18, 5);
		prt("(g) Display about yourself", 19, 5);
		prt("(h) Display dungeons", 20, 5);
#endif

		/* Prompt */
#ifdef JP
		prt("���ޥ��:", 22, 0);
#else
		prt("Command: ", 22, 0);
#endif


		/* Prompt */
		i = inkey();

		/* Done */
		if (i == ESCAPE) break;

		switch (i)
		{
		case '1': /* Artifacts */
			do_cmd_knowledge_artifacts();
			break;
		case '2': /* Uniques */
			do_cmd_knowledge_uniques();
			break;
		case '3': /* Uniques */
			do_cmd_knowledge_uniques_dead();
			break;
		case '4': /* Objects */
			do_cmd_knowledge_objects();
			break;
		case '5': /* Kill count  */
			do_cmd_knowledge_kill_count();
			break;
		case '6': /* Mutations */
			do_cmd_knowledge_mutations();
			break;
		case '7': /* Pets */
			do_cmd_knowledge_pets();
			break;
		case '8': /* Quests */
			do_cmd_knowledge_quests();
			break;
		case '9': /* Virtues */
			do_cmd_knowledge_virtues();
			break;
		case 'a': /* weapon-exp */
			do_cmd_knowledge_weapon_exp();
			break;
		case 'b': /* spell-exp */
			do_cmd_knowledge_spell_exp();
			break;
		case 'c': /* skill-exp */
			do_cmd_knowledge_skill_exp();
			break;
		case 'd': /* skill-exp */
			do_cmd_knowledge_kubi();
			break;
		case 'e': /* skill-exp */
			do_cmd_knowledge_home();
			break;
		case 'f': /* Resist list */
			do_cmd_knowledge_inven();
			break;
		case 'g': /* Max stat */
			do_cmd_knowledge_stat();
			break;
		case 'h': /* Dungeon */
			do_cmd_knowledge_dungeon();
			break;
		default: /* Unknown option */
			bell();
		}

		/* Flush messages */
		msg_print(NULL);
	}

	/* Restore the screen */
	screen_load();
}


/*
 * Check on the status of an active quest
 */
void do_cmd_checkquest(void)
{
	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Save the screen */
	screen_save();

	/* Quest info */
	do_cmd_knowledge_quests();

	/* Restore the screen */
	screen_load();
}


/*
 * Display the time and date
 */
void do_cmd_time(void)
{
	int day, hour, min, full, start, end, num;
	char desc[1024];

	char buf[1024];

	FILE *fff;

	s32b len = 20L * TOWN_DAWN;
	s32b tick = turn % len + len / 4;

	if ((p_ptr->prace == RACE_VAMPIRE) ||
	    (p_ptr->prace == RACE_SKELETON) ||
	    (p_ptr->prace == RACE_ZOMBIE) ||
	    (p_ptr->prace == RACE_SPECTRE))
		day = (turn - (15L * TOWN_DAWN))/ len + 1;
	else
		day = (turn + (5L * TOWN_DAWN))/ len + 1;
	hour = (24 * tick / len) % 24;
	min = (1440 * tick / len) % 60;
	full = hour * 100 + min;

	start = 9999;
	end = -9999;

	num = 0;

#ifdef JP
strcpy(desc, "�Ѥʻ������");
#else
	strcpy(desc, "It is a strange time.");
#endif


	/* Message */
#ifdef JP
msg_format("%d ����,�����%d:%02d %s�Ǥ���",
#else
	msg_format("This is day %d. The time is %d:%02d %s.",
#endif

				  day, (hour % 12 == 0) ? 12 : (hour % 12),
				  min, (hour < 12) ? "AM" : "PM");

	/* Find the path */
	if (!rand_int(10) || p_ptr->image)
		{
#ifdef JP
		path_build(buf, 1024, ANGBAND_DIR_FILE, "timefun_j.txt");
#else
		path_build(buf, 1024, ANGBAND_DIR_FILE, "timefun.txt");
#endif

		}
		else
		{
#ifdef JP
		path_build(buf, 1024, ANGBAND_DIR_FILE, "timenorm_j.txt");
#else
		path_build(buf, 1024, ANGBAND_DIR_FILE, "timenorm.txt");
#endif

		}

	/* Open this file */
	fff = my_fopen(buf, "rt");

	/* Oops */
	if (!fff) return;

	/* Find this time */
	while (!my_fgets(fff, buf, 1024))
	{
		/* Ignore comments */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Ignore invalid lines */
		if (buf[1] != ':') continue;

		/* Process 'Start' */
		if (buf[0] == 'S')
		{
			/* Extract the starting time */
			start = atoi(buf + 2);

			/* Assume valid for an hour */
			end = start + 59;

			/* Next... */
			continue;
		}

		/* Process 'End' */
		if (buf[0] == 'E')
		{
			/* Extract the ending time */
			end = atoi(buf + 2);

			/* Next... */
			continue;
		}

		/* Ignore incorrect range */
		if ((start > full) || (full > end)) continue;

		/* Process 'Description' */
		if (buf[0] == 'D')
		{
			num++;

			/* Apply the randomizer */
			if (!rand_int(num)) strcpy(desc, buf + 2);

			/* Next... */
			continue;
		}
	}

	/* Message */
	msg_print(desc);

	/* Close the file */
	my_fclose(fff);
}
