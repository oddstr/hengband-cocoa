/* File: cmd5.c */

/* Purpose: Spell/Prayer commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#include "spellstips.h"

cptr spell_categoly_name(int tval)
{
	switch (tval)
	{
#ifdef JP
	case TV_HISSATSU_BOOK:
		return "ɬ����";
	case TV_LIFE_BOOK:
		return "����";
	case TV_MUSIC_BOOK:
		return "��";
	default:
		return "��ʸ";
#else
	case TV_HISSATSU_BOOK:
		return "arts";
	case TV_LIFE_BOOK:
		return "prayer";
	case TV_MUSIC_BOOK:
		return "song";
	default:
		return "spell";
#endif
	}
}

/*
 * Allow user to choose a spell/prayer from the given book.
 *
 * If a valid spell is chosen, saves it in '*sn' and returns TRUE
 * If the user hits escape, returns FALSE, and set '*sn' to -1
 * If there are no legal choices, returns FALSE, and sets '*sn' to -2
 *
 * The "prompt" should be "cast", "recite", or "study"
 * The "known" should be TRUE for cast/pray, FALSE for study
 */

bool select_spellbook=FALSE;
bool select_the_force=FALSE;

static int get_spell(int *sn, cptr prompt, int sval, bool learned, int use_realm)
{
	int         i;
	int         spell = -1;
	int         num = 0;
	int         ask = TRUE;
	int         shouhimana;
	byte        spells[64];
	bool        flag, redraw, okay;
	char        choice;
	magic_type  *s_ptr;
	char        out_val[160];
	cptr        p;
#ifdef JP
        char jverb_buf[128];
#endif
	int menu_line = (use_menu ? 1 : 0);

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the spell, if available */
	if (repeat_pull(sn))
	{
		/* Verify the spell */
		if (spell_okay(*sn, learned, FALSE, use_realm - 1))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	p = spell_categoly_name(mp_ptr->spell_book);

	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}

	/* Assume no usable spells */
	okay = FALSE;

	/* Assume no spells available */
	(*sn) = -2;

	/* Check for "okay" spells */
	for (i = 0; i < num; i++)
	{
		/* Look for "okay" spells */
		if (spell_okay(spells[i], learned, FALSE, use_realm - 1)) okay = TRUE;
	}

	/* No "okay" spells */
	if (!okay) return (FALSE);
	if (((use_realm) != p_ptr->realm1) && ((use_realm) != p_ptr->realm2) && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE)) return FALSE;
	if (((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE)) && !is_magic(use_realm)) return FALSE;
	if ((p_ptr->pclass == CLASS_RED_MAGE) && ((use_realm) != REALM_ARCANE) && (sval > 1)) return FALSE;

	/* Assume cancelled */
	*sn = (-1);

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Show choices */
	if (show_choices)
	{
		/* Update */
		p_ptr->window |= (PW_SPELL);

		/* Window stuff */
		window_stuff();
	}

	/* Build a prompt (accept all spells) */
#ifdef JP
	jverb1( prompt, jverb_buf );
	(void) strnfmt(out_val, 78, "(%^s:%c-%c, '*'�ǰ���, ESC������) �ɤ�%s��%^s�ޤ���? ",
	        p, I2A(0), I2A(num - 1), p, jverb_buf );
#else
	(void)strnfmt(out_val, 78, "(%^ss %c-%c, *=List, ESC=exit) %^s which %s? ",
		p, I2A(0), I2A(num - 1), prompt, p);
#endif

	/* Get a spell from the user */

        choice = (always_show_list || use_menu) ? ESCAPE:1;
        while (!flag)
        {
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, TRUE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					return (FALSE);
					break;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (num - 1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > num) menu_line -= num;
			/* Display a list of spells */
			print_spells(menu_line, spells, num, 1, 15, use_realm - 1);
			if (ask) continue;
		}
		else
		{
			/* Request redraw */
			if ((choice == ' ') || (choice == '*') || (choice == '?'))
			{
				/* Show the list */
				if (!redraw)
				{
					/* Show list */
					redraw = TRUE;

					/* Save the screen */
					screen_save();

					/* Display a list of spells */
					print_spells(menu_line, spells, num, 1, 15, use_realm - 1);
				}

				/* Hide the list */
				else
				{
					if (use_menu) continue;

					/* Hide list */
					redraw = FALSE;

					/* Restore the screen */
					screen_load();
				}

				/* Redo asking */
				continue;
			}


			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Save the spell index */
		spell = spells[i];

		/* Require "okay" spells */
		if (!spell_okay(spell, learned, FALSE, use_realm - 1))
		{
			bell();
#ifdef JP
                        msg_format("����%s��%s���ȤϤǤ��ޤ���", p, prompt);
#else
			msg_format("You may not %s that %s.", prompt, p);
#endif

			continue;
		}

		/* Verify it */
		if (ask)
		{
			char tmp_val[160];

			/* Access the spell */
			if (!is_magic(use_realm))
			{
				s_ptr = &technic_info[use_realm - MIN_TECHNIC - 1][spell];
			}
			else
			{
				s_ptr = &mp_ptr->info[use_realm - 1][spell % 32];
			}

			if (use_realm == REALM_HISSATSU)
			{
				shouhimana = s_ptr->smana;
			}
			else
			{
				if (p_ptr->pclass == CLASS_SORCERER)
					shouhimana = s_ptr->smana*2200 + 2399;
				else if (p_ptr->pclass == CLASS_RED_MAGE)
					shouhimana = s_ptr->smana*2600 + 2399;
				else
					shouhimana = (s_ptr->smana*(3800-spell_exp[spell])+2399);
				if(p_ptr->dec_mana)
					shouhimana *= 3;
				else shouhimana *= 4;
				shouhimana /= 9600;
				if(shouhimana < 1) shouhimana = 1;
			}

			/* Prompt */
#ifdef JP
			jverb1( prompt, jverb_buf );
                        /* �����ڤ��ؤ���ǽ���б� */
                        (void) strnfmt(tmp_val, 78, "%s(MP%d, ����Ψ%d%%)��%s�ޤ���? ",
                                spell_names[technic2magic(use_realm)-1][spell % 32], shouhimana,
				       spell_chance(spell, use_realm -1),jverb_buf);
#else
			(void)strnfmt(tmp_val, 78, "%^s %s (%d mana, %d%% fail)? ",
				prompt, spell_names[technic2magic(use_realm)-1][spell % 32], shouhimana,
				spell_chance(spell, use_realm - 1));
#endif


			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}


	/* Restore the screen */
	if (redraw) screen_load();


	/* Show choices */
	if (show_choices)
	{
		/* Update */
		p_ptr->window |= (PW_SPELL);

		/* Window stuff */
		window_stuff();
	}


	/* Abort if needed */
	if (!flag) return (FALSE);

	/* Save the choice */
	(*sn) = spell;

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*sn);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


static bool item_tester_learn_spell(object_type *o_ptr)
{
	s32b choices = realm_choices2[p_ptr->pclass];

	if (p_ptr->pclass == CLASS_PRIEST)
	{
		if (p_ptr->realm1 == REALM_LIFE)
		{
			choices &= ~(CH_DEATH | CH_DAEMON);
		}
		else if ((p_ptr->realm1 == REALM_DEATH) || (p_ptr->realm1 == REALM_DAEMON))
		{
			choices &= ~(CH_LIFE);
		}
	}

	if ((o_ptr->tval < TV_LIFE_BOOK) || (o_ptr->tval > (TV_LIFE_BOOK + MAX_REALM - 1))) return (FALSE);
	if ((o_ptr->tval == TV_MUSIC_BOOK) && (p_ptr->pclass == CLASS_BARD)) return (TRUE);
	else if (!is_magic(tval2realm(o_ptr->tval))) return FALSE;
	if ((REALM1_BOOK == o_ptr->tval) || (REALM2_BOOK == o_ptr->tval)) return (TRUE);
	if (choices & (0x0001 << (tval2realm(o_ptr->tval) - 1))) return (TRUE);
	return (FALSE);
}


/*
 * Peruse the spells/prayers in a book
 *
 * Note that *all* spells in the book are listed
 *
 * Note that browsing is allowed while confused or blind,
 * and in the dark, primarily to allow browsing in stores.
 */
void do_cmd_browse(void)
{
	int		item, sval, use_realm = 0, j, line;
	int		spell = -1;
	int		num = 0;
	int             increment = 0;

	byte		spells[64];
	char            temp[62*4];

	object_type	*o_ptr;
	magic_type      *s_ptr;

	cptr q, s;

	/* Warriors are illiterate */
	if (!(p_ptr->realm1 || p_ptr->realm2) && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE))
	{
#ifdef JP
msg_print("�ܤ��ɤळ�Ȥ��Ǥ��ʤ���");
#else
		msg_print("You cannot read books!");
#endif

		return;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Restrict choices to "useful" books */
	if (p_ptr->realm2 == REALM_NONE) item_tester_tval = mp_ptr->spell_book;
	else item_tester_hook = item_tester_learn_spell;

	/* Get an item */
#ifdef JP
q = "�ɤ��ܤ��ɤߤޤ���? ";
#else
	q = "Browse which book? ";
#endif

#ifdef JP
s = "�ɤ���ܤ��ʤ���";
#else
	s = "You have no books that you can read.";
#endif

        select_spellbook=TRUE;
	if (p_ptr->pclass == CLASS_FORCETRAINER)
		select_the_force = TRUE;
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))){
            select_spellbook = FALSE;
	    select_the_force = FALSE;
            return;
        }
        select_spellbook = FALSE;
	select_the_force = FALSE;

	if (item == 1111) { /* the_force */
	    do_cmd_mind_browse();
	    return;
	} else
	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

#ifdef USE_SCRIPT
	if (object_browse_callback(o_ptr)) return;
#endif /* USE_SCRIPT */

	/* Access the item's sval */
	sval = o_ptr->sval;

	use_realm = tval2realm(o_ptr->tval);
	if ((p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE) && is_magic(use_realm))
	{
		if (o_ptr->tval == REALM2_BOOK) increment = 32;
		else if (o_ptr->tval != REALM1_BOOK) increment = 64;
	}

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();


	/* Extract spells */
	for (spell = 0; spell < 32; spell++)
	{
		/* Check for this spell */
		if ((fake_spell_flags[sval] & (1L << spell)))
		{
			/* Collect this spell */
			spells[num++] = spell;
		}
	}


	/* Save the screen */
	screen_save();

	/* Clear the top line */
	prt("", 0, 0);

	/* Keep browsing spells.  Exit browsing on cancel. */
	while(TRUE)
	{
		/* Ask for a spell, allow cancel */
#ifdef JP
		if (!get_spell(&spell, "�ɤ�", o_ptr->sval, TRUE, use_realm))
#else
		if (!get_spell(&spell, "browse", o_ptr->sval, TRUE, use_realm))
#endif
		{
			/* If cancelled, leave immediately. */
			if (spell == -1) break;

			/* Display a list of spells */
			print_spells(0, spells, num, 1, 15, use_realm - 1);

			/* Notify that there's nothing to see, and wait. */
			if (use_realm == REALM_HISSATSU)
#ifdef JP
				prt("�ɤ�뵻���ʤ���", 0, 0);
#else
				prt("No techniques to browse.", 0, 0);
#endif
			else
#ifdef JP
				prt("�ɤ���ʸ���ʤ���", 0, 0);
#else
				prt("No spells to browse.", 0, 0);
#endif
			(void)inkey();
			

			/* Restore the screen */
			screen_load();

			return;
		}				  

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(14, 14, 255);
		Term_erase(14, 13, 255);
		Term_erase(14, 12, 255);
		Term_erase(14, 11, 255);

		/* Access the spell */
		if (!is_magic(use_realm))
		{
			s_ptr = &technic_info[use_realm - MIN_TECHNIC - 1][spell];
		}
		else
		{
			s_ptr = &mp_ptr->info[use_realm - 1][spell];
		}

		roff_to_buf( spell_tips[technic2magic(use_realm)-1][spell] ,62,temp);
		for(j=0, line = 11;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
	}

	/* Restore the screen */
	screen_load();
}


static void change_realm2(int next_realm)
{
	int i, j=0;
	char tmp[80];

	for (i = 0; i < 64; i++)
	{
		spell_order[j] = spell_order[i];
		if(spell_order[i] < 32) j++;
	}
	for (; j < 64; j++)
		spell_order[j] = 99;

	for (i = 32; i < 64; i++)
	{
		spell_exp[i] = 0;
	}
	spell_learned2 = 0L;
	spell_worked2 = 0L;
	spell_forgotten2 = 0L;	

#ifdef JP
	sprintf(tmp,"��ˡ���ΰ��%s����%s���ѹ�������", realm_names[p_ptr->realm2], realm_names[next_realm]);
#else
	sprintf(tmp,"change magic realm from %s to %s.", realm_names[p_ptr->realm2], realm_names[next_realm]);
#endif
	do_cmd_write_nikki(NIKKI_BUNSHOU, 0, tmp);
	p_ptr->old_realm |= 1 << (p_ptr->realm2-1);
	p_ptr->realm2 = next_realm;

	p_ptr->notice |= (PN_REORDER);
	p_ptr->update |= (PU_SPELLS);
	handle_stuff();
}


/*
 * Study a book to gain a new spell/prayer
 */
void do_cmd_study(void)
{
	int	i, item, sval;
	int	increment = 0;
	bool    learned = FALSE;

	/* Spells of realm2 will have an increment of +32 */
	int	spell = -1;

	cptr p = spell_categoly_name(mp_ptr->spell_book);

	object_type *o_ptr;

	cptr q, s;

	if (!p_ptr->realm1)
	{
#ifdef JP
msg_print("�ܤ��ɤळ�Ȥ��Ǥ��ʤ���");
#else
		msg_print("You cannot read books!");
#endif

		return;
	}

	if (p_ptr->blind || no_lite())
	{
#ifdef JP
msg_print("�ܤ������ʤ���");
#else
		msg_print("You cannot see!");
#endif

		return;
	}

	if (p_ptr->confused)
	{
#ifdef JP
msg_print("���𤷤Ƥ����ɤ�ʤ���");
#else
		msg_print("You are too confused!");
#endif

		return;
	}

	if (!(p_ptr->new_spells))
	{
#ifdef JP
msg_format("������%s��Ф��뤳�ȤϤǤ��ʤ���", p);
#else
		msg_format("You cannot learn any new %ss!", p);
#endif

		return;
	}

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	p = spell_categoly_name(mp_ptr->spell_book);

#ifdef JP
	if( p_ptr->new_spells < 10 ){
		msg_format("���� %d �Ĥ�%s��ؤ٤롣", p_ptr->new_spells, p);
	}else{
		msg_format("���� %d �Ĥ�%s��ؤ٤롣", p_ptr->new_spells, p);
	}
#else
	msg_format("You can learn %d new %s%s.", p_ptr->new_spells, p,
		(p_ptr->new_spells == 1?"":"s"));
#endif

	msg_print(NULL);


	/* Restrict choices to "useful" books */
	if (p_ptr->realm2 == REALM_NONE) item_tester_tval = mp_ptr->spell_book;
	else item_tester_hook = item_tester_learn_spell;

	/* Get an item */
#ifdef JP
q = "�ɤ��ܤ���ؤӤޤ���? ";
#else
	q = "Study which book? ";
#endif

#ifdef JP
s = "�ɤ���ܤ��ʤ���";
#else
	s = "You have no books that you can read.";
#endif

        select_spellbook=TRUE;
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;
        select_spellbook=FALSE;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

	/* Access the item's sval */
	sval = o_ptr->sval;

	if (o_ptr->tval == REALM2_BOOK) increment = 32;
	else if (o_ptr->tval != REALM1_BOOK)
	{
#ifdef JP
		if (!get_check("��������ˡ���ΰ���ѹ����ޤ�����")) return;
#else
		if (!get_check("Really, change magic realm? ")) return;
#endif
		change_realm2(tval2realm(o_ptr->tval));
		increment = 32;
	}

	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Mage -- Learn a selected spell */
	if (mp_ptr->spell_book != TV_LIFE_BOOK)
	{
		/* Ask for a spell, allow cancel */
#ifdef JP
                if (!get_spell(&spell, "�ؤ�", sval, FALSE, o_ptr->tval - TV_LIFE_BOOK + 1)
			&& (spell == -1)) return;
#else
		if (!get_spell(&spell, "study", sval, FALSE, o_ptr->tval - TV_LIFE_BOOK + 1)
			&& (spell == -1)) return;
#endif

	}

	/* Priest -- Learn a random prayer */
	else
	{
		int k = 0;

		int gift = -1;

		/* Extract spells */
		for (spell = 0; spell < 32; spell++)
		{
			/* Check spells in the book */
			if ((fake_spell_flags[sval] & (1L << spell)))
			{
				/* Skip non "okay" prayers */
				if (!spell_okay(spell, FALSE, TRUE,
					(increment ? p_ptr->realm2 - 1 : p_ptr->realm1 - 1))) continue;

				/* Hack -- Prepare the randomizer */
				k++;

				/* Hack -- Apply the randomizer */
				if (rand_int(k) == 0) gift = spell;
			}
		}

		/* Accept gift */
		spell = gift;
	}

	/* Nothing to study */
	if (spell < 0)
	{
		/* Message */
#ifdef JP
msg_format("�����ܤˤϳؤ֤٤�%s���ʤ���", p);
#else
		msg_format("You cannot learn any %ss in that book.", p);
#endif


		/* Abort */
		return;
	}


	if (increment) spell += increment;

	/* Learn the spell */
	if (spell < 32)
	{
		if (spell_learned1 & (1L << spell)) learned = TRUE;
		else spell_learned1 |= (1L << spell);
	}
	else
	{
		if (spell_learned2 & (1L << (spell - 32))) learned = TRUE;
		else spell_learned2 |= (1L << (spell - 32));
	}

	if (learned)
	{
		int max_exp = (spell < 32) ? 1600 : 1400;
		int old_exp = spell_exp[spell];
		int new_rank = 0;
		cptr name = spell_names[technic2magic(increment ? p_ptr->realm2 : p_ptr->realm1)-1][spell%32];

		if (old_exp >= max_exp)
		{
#ifdef JP
			msg_format("����%s�ϴ����˻Ȥ����ʤ���Τǳؤ�ɬ�פϤʤ���", spell_categoly_name(mp_ptr->spell_book));
#else
			msg_format("You don't need to study this %s anymore.", spell_categoly_name(mp_ptr->spell_book));
#endif
			return;
		}
#ifdef JP
		if (!get_check(format("%s��%s�򤵤�˳ؤӤޤ���������Ǥ�����", name, spell_categoly_name(mp_ptr->spell_book))))
#else
		if (!get_check(format("You will study a %s of %s again. Are you sure? ", spell_categoly_name(mp_ptr->spell_book), name)))
#endif
		{
			return;
		}
		else if (old_exp >= 1400)
		{
			spell_exp[spell] = 1600;
			new_rank = 4;
		}
		else if (old_exp >= 1200)
		{
			if (spell >= 32) spell_exp[spell] = 1400;
			else spell_exp[spell] += 200;
			new_rank = 3;
		}
		else if (old_exp >= 900)
		{
			spell_exp[spell] = 1200+(old_exp-900)*2/3;
			new_rank = 2;
		}
		else
		{
			spell_exp[spell] = 900+(old_exp)/3;
			new_rank = 1;
		}
#ifdef JP
		msg_format("%s�ν����٤�%s�˾夬�ä���", name, shougou_moji[new_rank]);
#else
		msg_format("Your proficiency of %s is now %s rank.", name, shougou_moji[new_rank]);
#endif
	}
	else
	{
		/* Find the next open entry in "spell_order[]" */
		for (i = 0; i < 64; i++)
		{
			/* Stop at the first empty space */
			if (spell_order[i] == 99) break;
		}

		/* Add the spell to the known list */
		spell_order[i++] = spell;

		/* Mention the result */
#ifdef JP
	        /* �����ڤ��ؤ���ǽ���б� */
		if (mp_ptr->spell_book == TV_MUSIC_BOOK)
		{
        	        msg_format("%s��ؤ����",
				    spell_names[technic2magic(increment ? p_ptr->realm2 : p_ptr->realm1)-1][spell % 32]);
		}
		else
		{
        	        msg_format("%s��%s��ؤ����",
				    spell_names[technic2magic(increment ? p_ptr->realm2 : p_ptr->realm1)-1][spell % 32] ,p);
		}
#else
		msg_format("You have learned the %s of %s.",
			p, spell_names[technic2magic(increment ? p_ptr->realm2 : p_ptr->realm1)-1][spell % 32]);
#endif
	}

	/* Take a turn */
	energy_use = 100;

	if (mp_ptr->spell_book == TV_LIFE_BOOK)
		chg_virtue(V_FAITH, 1);
	else if (mp_ptr->spell_book == TV_DEATH_BOOK)
		chg_virtue(V_UNLIFE, 1);
	else if (mp_ptr->spell_book == TV_NATURE_BOOK)
		chg_virtue(V_NATURE, 1);
	else
		chg_virtue(V_KNOWLEDGE, 1);

	/* Sound */
	sound(SOUND_STUDY);

	/* One less spell available */
	p_ptr->learned_spells++;
#if 0
	/* Message if needed */
	if (p_ptr->new_spells)
	{
		/* Message */
#ifdef JP
                        if( p_ptr->new_spells < 10 ){
                                msg_format("���� %d �Ĥ�%s��ؤ٤롣", p_ptr->new_spells, p);
                        }else{
                                msg_format("���� %d �Ĥ�%s��ؤ٤롣", p_ptr->new_spells, p);
                        }
#else
		msg_format("You can learn %d more %s%s.",
			p_ptr->new_spells, p,
			(p_ptr->new_spells != 1) ? "s" : "");
#endif

	}
#endif

	/* Update Study */
	p_ptr->update |= (PU_SPELLS);
	update_stuff();
}


void wild_magic(int spell)
{
	int counter = 0;
	int type = SUMMON_BIZARRE1 + rand_int(6);

	if (type < SUMMON_BIZARRE1) type = SUMMON_BIZARRE1;
	else if (type > SUMMON_BIZARRE6) type = SUMMON_BIZARRE6;

	switch (randint(spell) + randint(8) + 1)
	{
	case 1:
	case 2:
	case 3:
		teleport_player(10);
		break;
	case 4:
	case 5:
	case 6:
		teleport_player(100);
		break;
	case 7:
	case 8:
		teleport_player(200);
		break;
	case 9:
	case 10:
	case 11:
		unlite_area(10, 3);
		break;
	case 12:
	case 13:
	case 14:
		lite_area(damroll(2, 3), 2);
		break;
	case 15:
		destroy_doors_touch();
		break;
	case 16: case 17:
		wall_breaker();
	case 18:
		sleep_monsters_touch();
		break;
	case 19:
	case 20:
		trap_creation(py, px);
		break;
	case 21:
	case 22:
		door_creation();
		break;
	case 23:
	case 24:
	case 25:
		aggravate_monsters(0);
		break;
	case 26:
		earthquake(py, px, 5);
		break;
	case 27:
	case 28:
		(void)gain_random_mutation(0);
		break;
	case 29:
	case 30:
		apply_disenchant(0);
		break;
	case 31:
		lose_all_info();
		break;
	case 32:
		fire_ball(GF_CHAOS, 0, spell + 5, 1 + (spell / 10));
		break;
	case 33:
		wall_stone();
		break;
	case 34:
	case 35:
		while (counter++ < 8)
		{
			(void)summon_specific(0, py, px, (dun_level * 3) / 2, type, TRUE, FALSE, FALSE, FALSE, TRUE);
		}
		break;
	case 36:
	case 37:
		activate_hi_summon(py, px, FALSE);
		break;
	case 38:
		(void)summon_cyber(-1, py, px);
		break;
	default:
		{
			int count = 0;
			(void)activate_ty_curse(FALSE, &count);
			break;
		}
	}

	return;
}


static bool cast_life_spell(int spell)
{
	int	dir;
	int	plev = p_ptr->lev;

	switch (spell)
	{
	case 0: /* Detect Evil */
		(void)detect_monsters_evil(DETECT_RAD_DEFAULT);
		break;
	case 1: /* Cure Light Wounds */
		(void)hp_player(damroll(2, 10));
		(void)set_cut(p_ptr->cut - 10);
		break;
	case 2: /* Bless */
		(void)set_blessed(randint(12) + 12, FALSE);
		break;
	case 3: /* Remove Fear */
		(void)set_afraid(0);
		break;
	case 4: /* Call Light */
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 5: /* Detect Traps + Secret Doors */
		(void)detect_traps(DETECT_RAD_DEFAULT);
		(void)detect_doors(DETECT_RAD_DEFAULT);
		(void)detect_stairs(DETECT_RAD_DEFAULT);
		break;
	case 6: /* Cure Medium Wounds */
		(void)hp_player(damroll(4, 10));
		(void)set_cut((p_ptr->cut / 2) - 20);
		break;
	case 7: /* Satisfy Hunger */
		(void)set_food(PY_FOOD_MAX - 1);
		break;
	case 8: /* Remove Curse */
		if (remove_curse())
		{
#ifdef JP
			msg_print("ï���˸�����Ƥ���褦�ʵ������롣");
#else
			msg_print("You feel as if someone is watching over you.");
#endif
		}
		break;
	case 9: /* Cure Poison */
		(void)set_poisoned(0);
		break;
	case 10: /* Cure Critical Wounds */
		(void)hp_player(damroll(8, 10));
		(void)set_stun(0);
		(void)set_cut(0);
		break;
	case 11: /* Sense Unseen */
		(void)set_tim_invis(randint(24) + 24, FALSE);
		break;
	case 12: /* Holy Orb */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_HOLY_FIRE, dir,
		          (damroll(3, 6) + plev +
		          (plev / ((p_ptr->pclass == CLASS_PRIEST ||
		             p_ptr->pclass == CLASS_HIGH_MAGE ||
			     p_ptr->pclass == CLASS_SORCERER) ? 2 : 4))),
		          ((plev < 30) ? 2 : 3));

		break;
	case 13: /* Protection from Evil */
		(void)set_protevil(randint(25) + 3 * p_ptr->lev, FALSE);
		break;
	case 14: /* Healing */
		(void)hp_player(300);
		(void)set_stun(0);
		(void)set_cut(0);
		break;
	case 15: /* Glyph of Warding */
		warding_glyph();
		break;
	case 16: /* Exorcism */
		(void)dispel_undead(randint(plev));
		(void)dispel_demons(randint(plev));
		(void)turn_evil(plev);
		break;
	case 17: /* Dispel Curse */
		if (remove_all_curse())
		{
#ifdef JP
			msg_print("ï���˸�����Ƥ���褦�ʵ������롣");
#else
			msg_print("You feel as if someone is watching over you.");
#endif
		}
		break;
	case 18: /* Dispel Undead + Demons */
		(void)dispel_undead(randint(plev * 3));
		(void)dispel_demons(randint(plev * 3));
		break;
	case 19: /* 'Day of the Dove' */
		charm_monsters(plev * 2);
		break;
	case 20: /* Dispel Evil */
		(void)dispel_evil(randint(plev * 4));
		break;
	case 21: /* Banishment */
		if (banish_evil(100))
		{
#ifdef JP
msg_print("���θ��Ϥ��ٰ����Ǥ�ʧ�ä���");
#else
			msg_print("The power of your god banishes evil!");
#endif

		}
		break;
	case 22: /* Holy Word */
		(void)dispel_evil(randint(plev * 4));
		(void)hp_player(1000);
		(void)set_afraid(0);
		(void)set_poisoned(0);
		(void)set_stun(0);
		(void)set_cut(0);
		break;
	case 23: /* Warding True */
		warding_glyph();
		glyph_creation();
		break;
	case 24: /* Heroism */
		(void)set_hero(randint(25) + 25, FALSE);
		(void)hp_player(10);
		(void)set_afraid(0);
		break;
	case 25: /* Prayer */
		(void)set_blessed(randint(48) + 48, FALSE);
		break;
	case 26: /* Turn Undead */
		(void)mass_genocide_undead(plev+50,TRUE);
		break;
	case 27: /* Restoration */
		(void)do_res_stat(A_STR);
		(void)do_res_stat(A_INT);
		(void)do_res_stat(A_WIS);
		(void)do_res_stat(A_DEX);
		(void)do_res_stat(A_CON);
		(void)do_res_stat(A_CHR);
		(void)restore_level();
		break;
	case 28: /* Healing True */
		(void)hp_player(2000);
		(void)set_stun(0);
		(void)set_cut(0);
		break;
	case 29: /* Holy Vision */
		return identify_fully(FALSE);
	case 30: /* Divine Intervention */
		project(0, 1, py, px, 500, GF_HOLY_FIRE, PROJECT_KILL, -1);
		dispel_monsters(plev * 4);
		slow_monsters();
		stun_monsters(plev * 4);
		confuse_monsters(plev * 4);
		turn_monsters(plev * 4);
		stasis_monsters(plev * 4);
		summon_specific(-1, py, px, plev, SUMMON_ANGEL, TRUE, TRUE, TRUE, FALSE, FALSE);
		(void)set_hero(randint(25) + 25, FALSE);
		(void)hp_player(300);
		(void)set_fast(randint(20 + plev) + plev, FALSE);
		(void)set_afraid(0);
		break;
	case 31: /* Ultimate resistance */
	{
		int v = randint(plev/2)+plev/2;
		(void)set_fast(v, FALSE);
		set_oppose_acid(v, FALSE);
		set_oppose_elec(v, FALSE);
		set_oppose_fire(v, FALSE);
		set_oppose_cold(v, FALSE);
		set_oppose_pois(v, FALSE);
		set_ultimate_res(v, FALSE);
		break;
	}
	default:
#ifdef JP
msg_format("���ʤ��������ʥ饤�դμ�ʸ %d �򾧤�����", spell);
#else
		msg_format("You cast an unknown Life spell: %d.", spell);
#endif

		msg_print(NULL);
	}

	return TRUE;
}



static bool cast_sorcery_spell(int spell)
{
	int	dir;
	int	plev = p_ptr->lev;

	switch (spell)
	{
	case 0: /* Detect Monsters */
		(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
		break;
	case 1: /* Phase Door */
		teleport_player(10);
		break;
	case 2: /* Detect Doors and Traps */
		(void)detect_traps(DETECT_RAD_DEFAULT);
		(void)detect_doors(DETECT_RAD_DEFAULT);
		(void)detect_stairs(DETECT_RAD_DEFAULT);
		break;
	case 3: /* Light Area */
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 4: /* Confuse Monster */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)confuse_monster(dir, (plev * 3) / 2);
		break;
	case 5: /* Teleport */
		teleport_player(plev * 5);
		break;
	case 6: /* Sleep Monster */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)sleep_monster(dir);
		break;
	case 7: /* Recharging */
		return recharge(plev * 4);
	case 8: /* Magic Mapping */
		map_area(DETECT_RAD_MAP);
		break;
	case 9: /* Identify */
		return ident_spell(FALSE);
	case 10: /* Slow Monster */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)slow_monster(dir);
		break;
	case 11: /* Mass Sleep */
		(void)sleep_monsters();
		break;
	case 12: /* Teleport Away */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)fire_beam(GF_AWAY_ALL, dir, plev);
		break;
	case 13: /* Haste Self */
		(void)set_fast(randint(20 + plev) + plev, FALSE);
		break;
	case 14: /* Detection True */
		(void)detect_all(DETECT_RAD_DEFAULT);
		break;
	case 15: /* Identify True */
		return identify_fully(FALSE);
	case 16: /* Detect Objects and Treasure*/
		(void)detect_objects_normal(DETECT_RAD_DEFAULT);
		(void)detect_treasure(DETECT_RAD_DEFAULT);
		(void)detect_objects_gold(DETECT_RAD_DEFAULT);
		break;
	case 17: /* Charm Monster */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)charm_monster(dir, plev);
		break;
	case 18: /* Sense Minds */
		(void)set_tim_esp(randint(30) + 25, FALSE);
		break;
	case 19: /* Teleport to town */
		return tele_town();
		break;
	case 20: /* Self knowledge */
		(void)self_knowledge();
		break;
	case 21: /* Teleport Level */
		if (!get_check("������¾�γ��˥ƥ�ݡ��Ȥ��ޤ�����")) return FALSE;
		(void)teleport_player_level();
		break;
	case 22: /* Word of Recall */
		if (!word_of_recall()) return FALSE;
		break;
	case 23: /* Explosive Rune */
		explosive_rune();
		break;
	case 24: /* Probing */
		(void)probing();
		break;
	case 25: /* Telekinesis */
		if (!get_aim_dir(&dir)) return FALSE;

		fetch(dir, plev * 15, FALSE);
		break;
	case 26: /* Clairvoyance */
		chg_virtue(V_KNOWLEDGE, 1);
		chg_virtue(V_ENLIGHTEN, 1);

		wiz_lite(FALSE, FALSE);
		if (!(p_ptr->telepathy))
		{
			(void)set_tim_esp(randint(30) + 25, FALSE);
		}
		break;
	case 27: /* Charm Monsters */
		charm_monsters(plev * 2);
		break;
	case 28: /* Dimension Door */
#ifdef JP
msg_print("�������⤬����������Ū�Ϥ�����ǲ�������");
#else
		msg_print("You open a dimensional gate. Choose a destination.");
#endif

		return dimension_door();
	case 29: /* Alchemy */
		return alchemy();
	case 30: /* Banish */
		banish_monsters(plev * 4);
		break;
	case 31: /* Globe of Invulnerability */
		(void)set_invuln(randint(4) + 4, FALSE);
		break;
	default:
#ifdef JP
msg_format("���ʤ��������ʥ������꡼�μ�ʸ %d �򾧤�����", spell);
#else
		msg_format("You cast an unknown Sorcery spell: %d.", spell);
#endif

		msg_print(NULL);
	}

	return TRUE;
}


static bool cast_nature_spell(int spell)
{
	int	    dir;
	int	    beam;
	int	    plev = p_ptr->lev;
	bool    no_trump = FALSE;

	if (p_ptr->pclass == CLASS_MAGE) beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER) beam = plev + 10;
	else beam = plev / 2;

	switch (spell)
	{
	case 0: /* Detect Creatures */
		(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
		break;
	case 1: /* Lightning Bolt */
		project_length = plev / 6 + 2;
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_ELEC, dir,
			damroll(3 + ((plev - 1) / 5), 4));
		break;
	case 2: /* Detect Doors & Traps */
		(void)detect_traps(DETECT_RAD_DEFAULT);
		(void)detect_doors(DETECT_RAD_DEFAULT);
		(void)detect_stairs(DETECT_RAD_DEFAULT);
		break;
	case 3: /* Produce Food */
		(void)set_food(PY_FOOD_MAX - 1);
		break;
	case 4: /* Daylight */
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
		{
#ifdef JP
msg_print("���θ������ʤ������Τ�Ǥ�������");
#else
			msg_print("The daylight scorches your flesh!");
#endif

#ifdef JP
take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "���θ�", -1);
#else
			take_hit(DAMAGE_NOESCAPE, damroll(2, 2), "daylight", -1);
#endif

		}
		break;
	case 5: /* Animal Taming */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)charm_animal(dir, plev);
		break;
	case 6: /* Resist Environment */
		(void)set_oppose_cold(randint(20) + 20, FALSE);
		(void)set_oppose_fire(randint(20) + 20, FALSE);
		(void)set_oppose_elec(randint(20) + 20, FALSE);
		break;
	case 7: /* Cure Wounds & Poison */
		(void)hp_player(damroll(2, 8));
		(void)set_cut(0);
		(void)set_poisoned(0);
		break;
	case 8: /* Stone to Mud */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)wall_to_mud(dir);
		break;
	case 9: /* Frost Bolt */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_bolt_or_beam(beam - 10, GF_COLD, dir,
			damroll(3 + ((plev - 5) / 4), 8));
		break;
	case 10: /* Nature Awareness -- downgraded */
		map_area(DETECT_RAD_MAP);
		(void)detect_traps(DETECT_RAD_DEFAULT);
		(void)detect_doors(DETECT_RAD_DEFAULT);
		(void)detect_stairs(DETECT_RAD_DEFAULT);
		(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
		break;
	case 11: /* Fire Bolt */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_bolt_or_beam(beam - 10, GF_FIRE, dir,
			damroll(5 + ((plev - 5) / 4), 8));
		break;
	case 12: /* Ray of Sunlight */
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
msg_print("���۸��������줿��");
#else
		msg_print("A line of sunlight appears.");
#endif

		(void)lite_line(dir);
		break;
	case 13: /* Entangle */
		slow_monsters();
		break;
	case 14: /* Summon Animals */
		if (!(summon_specific(-1, py, px, plev, SUMMON_ANIMAL_RANGER, TRUE, TRUE, TRUE, FALSE, FALSE)))
			no_trump = TRUE;
		break;
	case 15: /* Herbal Healing */
		(void)hp_player(500);
		(void)set_stun(0);
		(void)set_cut(0);
		(void)set_poisoned(0);
		break;
	case 16: /* Stair Building */
		(void)stair_creation();
		break;
	case 17: /* Stone Skin */
		(void)set_shield(randint(20) + 30, FALSE);
		break;
	case 18: /* Resistance True */
		(void)set_oppose_acid(randint(20) + 20, FALSE);
		(void)set_oppose_elec(randint(20) + 20, FALSE);
		(void)set_oppose_fire(randint(20) + 20, FALSE);
		(void)set_oppose_cold(randint(20) + 20, FALSE);
		(void)set_oppose_pois(randint(20) + 20, FALSE);
		break;
	case 19: /* Tree Creation */
		(void)tree_creation();
		break;
	case 20: /* Animal Friendship */
		(void)charm_animals(plev * 2);
		break;
	case 21: /* Stone Tell */
		return identify_fully(FALSE);
	case 22: /* Wall of Stone */
		(void)wall_stone();
		break;
	case 23: /* Protection from Corrosion */
		return rustproof();
	case 24: /* Earthquake */
		earthquake(py, px, 10);
		break;
	case 25: /* Whirlwind Attack */
		{
			int y = 0, x = 0;
			cave_type       *c_ptr;
			monster_type    *m_ptr;

			for (dir = 0; dir < 8; dir++)
			{
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				/* Get the monster */
				m_ptr = &m_list[c_ptr->m_idx];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx && (m_ptr->ml || cave_floor_bold(y, x)))
					py_attack(y, x, 0);
			}
		}
		break;
	case 26: /* Blizzard */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_COLD, dir, 70 + plev * 3 / 2, (plev / 12) + 1);
		break;
	case 27: /* Lightning Storm */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_ELEC, dir, 90 + plev * 3 / 2, (plev / 12) + 1);
		break;
	case 28: /* Whirlpool */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_WATER, dir, 100 + plev * 3 / 2, (plev / 12) + 1);
		break;
	case 29: /* Call Sunlight */
		fire_ball(GF_LITE, 0, 150, 8);
		chg_virtue(V_KNOWLEDGE, 1);
		chg_virtue(V_ENLIGHTEN, 1);
		wiz_lite(FALSE, FALSE);
		if ((prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE)) && !p_ptr->resist_lite)
		{
#ifdef JP
msg_print("���������ʤ������Τ�Ǥ�������");
#else
			msg_print("The sunlight scorches your flesh!");
#endif

#ifdef JP
take_hit(DAMAGE_NOESCAPE, 50, "����", -1);
#else
			take_hit(DAMAGE_NOESCAPE, 50, "sunlight", -1);
#endif

		}
		break;
	case 30: /* Elemental Branding */
		brand_weapon(rand_int(2));
		break;
	case 31: /* Nature's Wrath */
		(void)dispel_monsters(plev * 4);
		earthquake(py, px, 20 + (plev / 2));
		project(0, 1 + plev / 12, py, px,
			(100 + plev) * 2, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
		break;
	default:
#ifdef JP
msg_format("���ʤ��������ʥͥ����㡼�μ�ʸ %d �򾧤�����", spell);
#else
		msg_format("You cast an unknown Nature spell: %d.", spell);
#endif

		msg_print(NULL);
	}

	if (no_trump)
#ifdef JP
msg_print("ưʪ�ϸ���ʤ��ä���");
#else
		msg_print("No animals arrive.");
#endif


	return TRUE;
}


static bool cast_chaos_spell(int spell)
{
	int	dir, i, beam;
	int	plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_MAGE) beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER) beam = plev + 10;
	else beam = plev / 2;

	switch (spell)
	{
	case 0: /* Magic Missile */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
			damroll(3 + ((plev - 1) / 5), 4));
		break;
	case 1: /* Trap / Door destruction */
		(void)destroy_doors_touch();
		break;
	case 2: /* Flash of Light == Light Area */
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 3: /* Touch of Confusion */
		if (!(p_ptr->special_attack & ATTACK_CONFUSE))
		{
#ifdef JP
msg_print("���ʤ��μ�ϸ���Ϥ᤿��");
#else
			msg_print("Your hands start glowing.");
#endif

			p_ptr->special_attack |= ATTACK_CONFUSE;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	case 4: /* Mana Burst */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_MISSILE, dir,
			(damroll(3, 5) + plev +
			(plev / (((p_ptr->pclass == CLASS_MAGE)
			|| (p_ptr->pclass == CLASS_HIGH_MAGE)
			|| (p_ptr->pclass == CLASS_SORCERER)) ? 2 : 4))),
			((plev < 30) ? 2 : 3));
			/* Shouldn't actually use GF_MANA, as it will destroy all
			 * items on the floor */
		break;
	case 5: /* Fire Bolt */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam, GF_FIRE, dir,
			damroll(8 + ((plev - 5) / 4), 8));
		break;
	case 6: /* Fist of Force ("Fist of Fun") */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_DISINTEGRATE, dir,
			damroll(8 + ((plev - 5) / 4), 8), 0);
		break;
	case 7: /* Teleport Self */
		teleport_player(plev * 5);
		break;
	case 8: /* Wonder */
		{
		/* This spell should become more useful (more
		controlled) as the player gains experience levels.
		Thus, add 1/5 of the player's level to the die roll.
		This eliminates the worst effects later on, while
		keeping the results quite random.  It also allows
			some potent effects only at high level. */

			int die = randint(100) + plev / 5;
			int vir = virtue_number(V_CHANCE);
			if (vir)
			{
				if (p_ptr->virtues[vir - 1] > 0)
				{
					while (randint(400) < p_ptr->virtues[vir - 1]) die++;
				}
				else
				{
					while (randint(400) < (0-p_ptr->virtues[vir - 1])) die--;
				}
			}

			if (die < 26)
				chg_virtue(V_CHANCE, 1);

			if (!get_aim_dir(&dir)) return FALSE;
			if (die > 100)
#ifdef JP
msg_print("���ʤ����Ϥ��ߤʤ���Τ򴶤�����");
#else
				msg_print("You feel a surge of power!");
#endif

			if (die < 8) clone_monster(dir);
			else if (die < 14) speed_monster(dir);
			else if (die < 26) heal_monster(dir, damroll(4, 6));
			else if (die < 31) poly_monster(dir);
			else if (die < 36)
				fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
				    damroll(3 + ((plev - 1) / 5), 4));
			else if (die < 41) confuse_monster(dir, plev);
			else if (die < 46) fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
			else if (die < 51) (void)lite_line(dir);
			else if (die < 56)
				fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
				    damroll(3 + ((plev - 5) / 4), 8));
			else if (die < 61)
				fire_bolt_or_beam(beam - 10, GF_COLD, dir,
				    damroll(5 + ((plev - 5) / 4), 8));
			else if (die < 66)
				fire_bolt_or_beam(beam, GF_ACID, dir,
				    damroll(6 + ((plev - 5) / 4), 8));
			else if (die < 71)
				fire_bolt_or_beam(beam, GF_FIRE, dir,
				    damroll(8 + ((plev - 5) / 4), 8));
			else if (die < 76) drain_life(dir, 75);
			else if (die < 81) fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
			else if (die < 86) fire_ball(GF_ACID, dir, 40 + plev, 2);
			else if (die < 91) fire_ball(GF_ICE, dir, 70 + plev, 3);
			else if (die < 96) fire_ball(GF_FIRE, dir, 80 + plev, 3);
			else if (die < 101) drain_life(dir, 100 + plev);
			else if (die < 104)
			{
				earthquake(py, px, 12);
			}
			else if (die < 106)
			{
				destroy_area(py, px, 13+rand_int(5), TRUE);
			}
			else if (die < 108)
			{
				symbol_genocide(plev+50, TRUE);
			}
			else if (die < 110) dispel_monsters(120);
			else /* RARE */
			{
				dispel_monsters(150);
				slow_monsters();
				sleep_monsters();
				hp_player(300);
			}
			break;
		}
		break;
	case 9: /* Chaos Bolt */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam, GF_CHAOS, dir,
			damroll(10 + ((plev - 5) / 4), 8));
		break;
	case 10: /* Sonic Boom */
#ifdef JP
msg_print("�ɡ����������ɤ줿��");
#else
		msg_print("BOOM! Shake the room!");
#endif

		project(0, plev / 10 + 2, py, px,
			(60 + plev), GF_SOUND, PROJECT_KILL | PROJECT_ITEM, -1);
		break;
	case 11: /* Doom Bolt -- always beam in 2.0.7 or later */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_MANA, dir, damroll(11 + ((plev - 5) / 4), 8));
		break;
	case 12: /* Fire Ball */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_FIRE, dir, plev + 55, 2);
		break;
	case 13: /* Teleport Other */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)fire_beam(GF_AWAY_ALL, dir, plev);
		break;
	case 14: /* Word of Destruction */
		destroy_area(py, px, 13+rand_int(5), TRUE);
		break;
	case 15: /* Invoke Logrus */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_CHAOS, dir, plev*2 + 99, plev / 5);
		break;
	case 16: /* Polymorph Other */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)poly_monster(dir);
		break;
	case 17: /* Chain Lightning */
		for (dir = 0; dir <= 9; dir++)
			fire_beam(GF_ELEC, dir, damroll(5 + (plev / 10), 8));
		break;
	case 18: /* Arcane Binding == Charging */
		return recharge(90);
	case 19: /* Disintegration */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_DISINTEGRATE, dir, plev + 70, 3 + plev / 40);
		break;
	case 20: /* Alter Reality */
		alter_reality();
		break;
	case 21: /* Magic Rocket */
		if (!get_aim_dir(&dir)) return FALSE;

#ifdef JP
msg_print("���å�ȯ�͡�");
#else
		msg_print("You launch a rocket!");
#endif

		fire_rocket(GF_ROCKET, dir, 120 + plev*2, 2);
		break;
	case 22: /* Chaos Branding */
		brand_weapon(2);
		break;
	case 23: /* Summon monster, demon */
		{
			bool pet = (randint(3) == 1);
			bool group = !(pet && (plev < 50));

			if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, SUMMON_DEMON, group, FALSE, pet, FALSE, !pet))
			{
#ifdef JP
msg_print("β���ΰ���������������");
#else
				msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


				if (pet)
#ifdef JP
msg_print("�֤��ѤǤ������ޤ�����������͡�");
#else
					msg_print("'What is thy bidding... Master?'");
#endif

				else
#ifdef JP
msg_print("���ܤ����Ԥ衢�����β��ͤˤ��餺�� �����κ���ĺ��������");
#else
					msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif

			}
			break;
		}
	case 24: /* Beam of Gravity */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_beam(GF_GRAVITY, dir, damroll(9 + ((plev - 5) / 4), 8));
		break;
	case 25: /* Meteor Swarm  */
		{
			int x, y, dx, dy;
			int b = 10 + randint(10);
			for (i = 0; i < b; i++)
			{
				int count = 0, d = 0;

				while (1)
				{
					count++;
					if (count > 20) break;
					x = px - 8 + rand_int(17);
					y = py - 8 + rand_int(17);

					if (!in_bounds(y,x) || (!cave_floor_bold(y,x) && (cave[y][x].feat != FEAT_TREES)) || !player_has_los_bold(y, x)) continue;

					dx = (px > x) ? (px - x) : (x - px);
					dy = (py > y) ? (py - y) : (y - py);

					/* Approximate distance */
					d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
					if (d < 9) break;
				}

				if (count > 20) continue;

				project(0, 2, y, x, plev * 2, GF_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
			}
		}
		break;
	case 26: /* Flame Strike */
		fire_ball(GF_FIRE, 0, 300 + (3 * plev), 8);
		break;
	case 27: /* Call Chaos */
		call_chaos();
		break;
	case 28: /* Polymorph Self */
#ifdef JP
		if (!get_check("�ѿȤ��ޤ���������Ǥ�����")) return FALSE;
#else
		if (!get_check("You will polymorph yourself. Are you sure? ")) return FALSE;
#endif
		do_poly_self();
		break;
	case 29: /* Mana Storm */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_MANA, dir, 300 + (plev * 4), 4);
		break;
	case 30: /* Breathe Logrus */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_CHAOS, dir, p_ptr->chp, 2);
		break;
	case 31: /* Call the Void */
		call_the_();
		break;
	default:
#ifdef JP
msg_format("���ʤ��������ʥ������μ�ʸ %d �򾧤�����", spell);
#else
		msg_format("You cast an unknown Chaos spell: %d.", spell);
#endif

		msg_print(NULL);
	}

	return TRUE;
}


static bool cast_death_spell(int spell)
{
	int	dir;
	int	beam;
	int	plev = p_ptr->lev;
	int	dummy = 0;

	if (p_ptr->pclass == CLASS_MAGE) beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER) beam = plev + 10;
	else beam = plev / 2;

	switch (spell)
	{
	case 0: /* Detect Undead & Demons -> Unlife */
		(void)detect_monsters_nonliving(DETECT_RAD_DEFAULT);
		break;
	case 1: /* Malediction */
		if (!get_aim_dir(&dir)) return FALSE;
		/* A radius-0 ball may (1) be aimed at objects etc.,
		 * and will affect them; (2) may be aimed at ANY
		 * visible monster, unlike a 'bolt' which must travel
		 * to the monster. */

		fire_ball(GF_HELL_FIRE, dir,
			damroll(3 + ((plev - 1) / 5), 4), 0);

		if (randint(5) == 1)
		{   /* Special effect first */
			dummy = randint(1000);
			if (dummy == 666)
				fire_ball_hide(GF_DEATH_RAY, dir, plev * 200, 0);
			else if (dummy < 500)
				fire_ball_hide(GF_TURN_ALL, dir, plev, 0);
			else if (dummy < 800)
				fire_ball_hide(GF_OLD_CONF, dir, plev, 0);
			else
				fire_ball_hide(GF_STUN, dir, plev, 0);
		}
		break;
	case 2: /* Detect Evil */
		(void)detect_monsters_evil(DETECT_RAD_DEFAULT);
		break;
	case 3: /* Stinking Cloud */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_POIS, dir, 10 + (plev / 2), 2);
		break;
	case 4: /* Black Sleep */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)sleep_monster(dir);
		break;
	case 5: /* Resist Poison */
		(void)set_oppose_pois(randint(20) + 20, FALSE);
		break;
	case 6: /* Horrify */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)fear_monster(dir, plev);
		(void)stun_monster(dir, plev);
		break;
	case 7: /* Enslave Undead */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)control_one_undead(dir, plev);
		break;
	case 8: /* Orb of Entropy */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_OLD_DRAIN, dir,
			(damroll(3, 6) + plev +
			(plev / (((p_ptr->pclass == CLASS_MAGE) ||
			(p_ptr->pclass == CLASS_HIGH_MAGE) ||
			(p_ptr->pclass == CLASS_SORCERER)) ? 2 : 4))),
			((plev < 30) ? 2 : 3));
		break;
	case 9: /* Nether Bolt */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam, GF_NETHER, dir,
		    damroll(8 + ((plev - 5) / 4), 8));
		break;
	case 10: /* Cloud kill */
		project(0, plev / 10 + 2, py, px,
			(30 + plev) * 2, GF_POIS, PROJECT_KILL | PROJECT_ITEM, -1);
		break;
	case 11: /* Genocide One */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball_hide(GF_GENOCIDE, dir, plev + 50, 0);
		break;
	case 12: /* Poison Branding */
		brand_weapon(3);
		break;
	case 13: /* Vampiric Drain */
		if (!get_aim_dir(&dir)) return FALSE;

		dummy = plev * 2 + randint(plev * 2);   /* Dmg */
		if (drain_life(dir, dummy))
		{
			chg_virtue(V_SACRIFICE, -1);
			chg_virtue(V_VITALITY, -1);

			(void)hp_player(dummy);
			/* Gain nutritional sustenance: 150/hp drained */
			/* A Food ration gives 5000 food points (by contrast) */
			/* Don't ever get more than "Full" this way */
			/* But if we ARE Gorged,  it won't cure us */
			dummy = p_ptr->food + MIN(5000, 100 * dummy);
			if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
				(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
		}
		break;
	case 14: /* Animate Dead */
		animate_dead(0, py, px);
		break;
	case 15: /* Genocide */
		(void)symbol_genocide(plev+50, TRUE);
		break;
	case 16: /* Berserk */
		(void)set_shero(randint(25) + 25, FALSE);
		(void)hp_player(30);
		(void)set_afraid(0);
		break;
	case 17: /* Invoke Spirits */
		{
			int die = randint(100) + plev / 5;
			int vir = virtue_number(V_CHANCE);
			if (vir)
			{
				if (p_ptr->virtues[vir - 1] > 0)
				{
					while (randint(400) < p_ptr->virtues[vir - 1]) die++;
				}
				else
				{
					while (randint(400) < (0-p_ptr->virtues[vir - 1])) die--;
				}
			}

			if (!get_aim_dir(&dir)) return FALSE;

#ifdef JP
msg_print("���ʤ��ϻ�Ԥ������Ϥ򾷽�����...");
#else
			msg_print("You call on the power of the dead...");
#endif
			if (die < 26)
				chg_virtue(V_CHANCE, 1);

			if (die > 100)
#ifdef JP
msg_print("���ʤ��Ϥ��ɤ��ɤ����ϤΤ��ͤ�򴶤�����");
#else
				msg_print("You feel a surge of eldritch force!");
#endif


			if (die < 8)
			{
#ifdef JP
msg_print("�ʤ�Ƥ��ä������ʤ��μ�������̤��������ͱƤ�Ω���夬�äƤ�����");
#else
				msg_print("Oh no! Mouldering forms rise from the earth around you!");
#endif

				(void)summon_specific(0, py, px, dun_level, SUMMON_UNDEAD, TRUE, FALSE, FALSE, TRUE, TRUE);
				chg_virtue(V_UNLIFE, 1);
			}
			else if (die < 14)
			{
#ifdef JP
msg_print("̾�����񤤼ٰ���¸�ߤ����ʤ��ο����̤�᤮�ƹԤä�...");
#else
				msg_print("An unnamable evil brushes against your mind...");
#endif

				set_afraid(p_ptr->afraid + randint(4) + 4);
			}
			else if (die < 26)
			{
#ifdef JP
msg_print("���ʤ���Ƭ�����̤�ͩ����������������������󤻤Ƥ���...");
#else
				msg_print("Your head is invaded by a horde of gibbering spectral voices...");
#endif

				set_confused(p_ptr->confused + randint(4) + 4);
			}
			else if (die < 31)
			{
				poly_monster(dir);
			}
			else if (die < 36)
			{
				fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
					damroll(3 + ((plev - 1) / 5), 4));
			}
			else if (die < 41)
			{
				confuse_monster (dir, plev);
			}
			else if (die < 46)
			{
				fire_ball(GF_POIS, dir, 20 + (plev / 2), 3);
			}
			else if (die < 51)
			{
				(void)lite_line(dir);
			}
			else if (die < 56)
			{
				fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
					damroll(3+((plev-5)/4),8));
			}
			else if (die < 61)
			{
				fire_bolt_or_beam(beam - 10, GF_COLD, dir,
					damroll(5+((plev-5)/4),8));
			}
			else if (die < 66)
			{
				fire_bolt_or_beam(beam, GF_ACID, dir,
					damroll(6+((plev-5)/4),8));
			}
			else if (die < 71)
			{
				fire_bolt_or_beam(beam, GF_FIRE, dir,
					damroll(8+((plev-5)/4),8));
			}
			else if (die < 76)
			{
				drain_life(dir, 75);
			}
			else if (die < 81)
			{
				fire_ball(GF_ELEC, dir, 30 + plev / 2, 2);
			}
			else if (die < 86)
			{
				fire_ball(GF_ACID, dir, 40 + plev, 2);
			}
			else if (die < 91)
			{
				fire_ball(GF_ICE, dir, 70 + plev, 3);
			}
			else if (die < 96)
			{
				fire_ball(GF_FIRE, dir, 80 + plev, 3);
			}
			else if (die < 101)
			{
				drain_life(dir, 100 + plev);
			}
			else if (die < 104)
			{
				earthquake(py, px, 12);
			}
			else if (die < 106)
			{
				destroy_area(py, px, 13+rand_int(5), TRUE);
			}
			else if (die < 108)
			{
				symbol_genocide(plev+50, TRUE);
			}
			else if (die < 110)
			{
				dispel_monsters(120);
			}
			else
			{ /* RARE */
				dispel_monsters(150);
				slow_monsters();
				sleep_monsters();
				hp_player(300);
			}

			if (die < 31)
#ifdef JP
msg_print("�������������������Ф����֤⤦�������ޤ��ϲ桹����֤ˤʤ�������夭�Ԥ衣��");
#else
				msg_print("Sepulchral voices chuckle. 'Soon you will join us, mortal.'");
#endif

			break;
		}
	case 18: /* Dark Bolt */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam, GF_DARK, dir,
			damroll(4 + ((plev - 5) / 4), 8));
		break;
	case 19: /* Battle Frenzy */
		(void)set_shero(randint(25) + 25, FALSE);
		(void)hp_player(30);
		(void)set_afraid(0);
		(void)set_fast(randint(20 + (plev / 2)) + (plev / 2), FALSE);
		break;
	case 20: /* Vampiric Branding */
		brand_weapon(4);
		break;
	case 21: /* Vampirism True */
		if (!get_aim_dir(&dir)) return FALSE;

		chg_virtue(V_SACRIFICE, -1);
		chg_virtue(V_VITALITY, -1);

		for (dummy = 0; dummy < 3; dummy++)
		{
			if (drain_life(dir, 100))
				hp_player(100);
		}
		break;
	case 22: /* Word of Death */
		(void)dispel_living(randint(plev * 3));
		break;
	case 23: /* Darkness Storm */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_DARK, dir, 100+plev*2, 4);
		break;
	case 24: /* Death Ray */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)death_ray(dir, plev);
		break;
	case 25: /* Raise the Dead */
		{
			bool pet = (randint(3) == 1);
			bool group;
			int type;

			type = (plev > 47 ? SUMMON_HI_UNDEAD : SUMMON_UNDEAD);
			if (pet)
			{
				group = (((plev > 24) && (randint(3) == 1)) ? TRUE : FALSE);
			}
			else
			{
				group = TRUE;
			}

			if (summon_specific((pet ? -1 : 0), py, px, (plev * 3) / 2, type, group, FALSE, pet, !pet, !pet))
			{
#ifdef JP
msg_print("�䤿���������ʤ��μ���˿᤭�Ϥ᤿����������Խ��򱿤�Ǥ���...");
#else
				msg_print("Cold winds begin to blow around you, carrying with them the stench of decay...");
#endif


				if (pet)
#ifdef JP
msg_print("�Ť��λऻ��Զ������ʤ��˻Ť��뤿���ڤ���ᴤä���");
#else
					msg_print("Ancient, long-dead forms arise from the ground to serve you!");
#endif

				else
#ifdef JP
msg_print("��Ԥ�ᴤä���̲���˸���뤢�ʤ���ȳ���뤿��ˡ�");
#else
					msg_print("'The dead arise... to punish you for disturbing them!'");
#endif

			chg_virtue(V_UNLIFE, 1);
			}

			break;
		}
	case 26: /* Esoteria */
		if (randint(50) > plev)
			return ident_spell(FALSE);
		else
			return identify_fully(FALSE);
		break;
	case 27: /* Mimic vampire */
		(void)set_mimic(10+plev/2 + randint(10+plev/2), MIMIC_VAMPIRE, FALSE);
		break;
	case 28: /* Restore Life */
		(void)restore_level();
		break;
	case 29: /* Mass Genocide */
		(void)mass_genocide(plev+50, TRUE);
		break;
	case 30: /* Hellfire */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_HELL_FIRE, dir, 666, 3);
#ifdef JP
take_hit(DAMAGE_USELIFE, 20 + randint(30), "�Ϲ��ι�Фμ�ʸ�򾧤�����ϫ", -1);
#else
		take_hit(DAMAGE_USELIFE, 20 + randint(30), "the strain of casting Hellfire", -1);
#endif

		break;
	case 31: /* Wraithform */
		set_wraith_form(randint(plev / 2) + (plev / 2), FALSE);
		break;
	default:
		msg_format("You cast an unknown Death spell: %d.", spell);
		msg_print(NULL);
	}

	return TRUE;
}


static bool cast_trump_spell(int spell, bool success)
{
	int     dir;
	int     beam;
	int     plev = p_ptr->lev;
	int     summon_lev = plev * 2 / 3 + randint(plev/2);
	int     dummy = 0;
	bool	no_trump = FALSE;
	bool    unique_okay = FALSE;


	if (p_ptr->pclass == CLASS_MAGE) beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER) beam = plev + 10;
	else beam = plev / 2;

	if (summon_lev < 1) summon_lev = 1;
	if (!success || (randint(50+plev) < plev/10)) unique_okay = TRUE;
	switch (spell)
	{
		case 0: /* Phase Door */
			if (success)
			{
				teleport_player(10);
			}
			break;
		case 1: /* Trump Spiders */
		{
			bool pet = success; /* (randint(5) > 2) */

#ifdef JP
msg_print("���ʤ�������Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of an spider...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_SPIDER, TRUE, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿������ܤäƤ��롪");
#else
					msg_print("The summoned spiders get angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 2: /* Shuffle */
			if (success)
			{
				/* A limited power 'wonder' spell */
				int die = randint(120);
				int vir = virtue_number(V_CHANCE);

				if ((p_ptr->pclass == CLASS_ROGUE) ||
					(p_ptr->pclass == CLASS_HIGH_MAGE) ||
				        (p_ptr->pclass == CLASS_SORCERER))
					die = (randint(110)) + plev / 5;
				/* Card sharks and high mages get a level bonus */

				if (vir)
				{
					if (p_ptr->virtues[vir - 1] > 0)
					{
						while (randint(400) < p_ptr->virtues[vir - 1]) die++;
					}
					else
					{
						while (randint(400) < (0-p_ptr->virtues[vir - 1])) die--;
					}
				}

#ifdef JP
msg_print("���ʤ��ϥ����ɤ��ڤäư��������...");
#else
				msg_print("You shuffle the deck and draw a card...");
#endif

				if (die < 30)
					chg_virtue(V_CHANCE, 1);

				if (die < 7)
				{
#ifdef JP
msg_print("�ʤ�Ƥ��ä����Ի�դ���");
#else
					msg_print("Oh no! It's Death!");
#endif

					for (dummy = 0; dummy < randint(3); dummy++)
						(void)activate_hi_summon(py, px, FALSE);
				}
				else if (die < 14)
				{
#ifdef JP
msg_print("�ʤ�Ƥ��ä����԰���դ���");
#else
					msg_print("Oh no! It's the Devil!");
#endif

					(void)summon_specific(0, py, px, dun_level, SUMMON_DEMON, TRUE, FALSE, FALSE, TRUE, TRUE);
				}
				else if (die < 18)
				{
					int count = 0;
#ifdef JP
msg_print("�ʤ�Ƥ��ä������ߤ�줿�ˡդ���");
#else
					msg_print("Oh no! It's the Hanged Man.");
#endif

					(void)activate_ty_curse(FALSE, &count);
				}
				else if (die < 22)
				{
#ifdef JP
msg_print("����Ĵ�¤η��դ���");
#else
					msg_print("It's the swords of discord.");
#endif

					aggravate_monsters(0);
				}
				else if (die < 26)
				{
#ifdef JP
msg_print("�Զ�ԡդ���");
#else
					msg_print("It's the Fool.");
#endif

					(void)do_dec_stat(A_INT);
					(void)do_dec_stat(A_WIS);
				}
				else if (die < 30)
				{
#ifdef JP
msg_print("��̯�ʥ�󥹥����γ�����");
#else
					msg_print("It's the picture of a strange monster.");
#endif

					if (!(summon_specific(0, py, px, (dun_level * 3) / 2, 32 + randint(6), TRUE, FALSE, FALSE, TRUE, TRUE)))
						no_trump = TRUE;
				}
				else if (die < 33)
				{
#ifdef JP
msg_print("�Է�դ���");
#else
					msg_print("It's the Moon.");
#endif

					unlite_area(10, 3);
				}
				else if (die < 38)
				{
#ifdef JP
msg_print("�Ա�̿���ءդ���");
#else
					msg_print("It's the Wheel of Fortune.");
#endif

					wild_magic(rand_int(32));
				}
				else if (die < 40)
				{
#ifdef JP
msg_print("�ƥ�ݡ��ȡ������ɤ���");
#else
					msg_print("It's a teleport trump card.");
#endif

					teleport_player(10);
				}
				else if (die < 42)
				{
#ifdef JP
msg_print("�������դ���");
#else
					msg_print("It's Justice.");
#endif

					set_blessed(p_ptr->lev, FALSE);
				}
				else if (die < 47)
				{
#ifdef JP
msg_print("�ƥ�ݡ��ȡ������ɤ���");
#else
					msg_print("It's a teleport trump card.");
#endif

					teleport_player(100);
				}
				else if (die < 52)
				{
#ifdef JP
msg_print("�ƥ�ݡ��ȡ������ɤ���");
#else
					msg_print("It's a teleport trump card.");
#endif

					teleport_player(200);
				}
				else if (die < 60)
				{
#ifdef JP
msg_print("����դ���");
#else
					msg_print("It's the Tower.");
#endif

					wall_breaker();
				}
				else if (die < 72)
				{
#ifdef JP
msg_print("�������դ���");
#else
					msg_print("It's Temperance.");
#endif

					sleep_monsters_touch();
				}
				else if (die < 80)
				{
#ifdef JP
msg_print("����դ���");
#else
					msg_print("It's the Tower.");
#endif

					earthquake(py, px, 5);
				}
				else if (die < 82)
				{
#ifdef JP
msg_print("ͧ��Ū�ʥ�󥹥����γ�����");
#else
					msg_print("It's the picture of a friendly monster.");
#endif

					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2, SUMMON_BIZARRE1, FALSE, TRUE, TRUE, FALSE, FALSE)))
						no_trump = TRUE;
				}
				else if (die < 84)
				{
#ifdef JP
msg_print("ͧ��Ū�ʥ�󥹥����γ�����");
#else
					msg_print("It's the picture of a friendly monster.");
#endif

					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2, SUMMON_BIZARRE2, FALSE, TRUE, TRUE, FALSE, FALSE)))
						no_trump = TRUE;
				}
				else if (die < 86)
				{
#ifdef JP
msg_print("ͧ��Ū�ʥ�󥹥����γ�����");
#else
					msg_print("It's the picture of a friendly monster.");
#endif

					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2, SUMMON_BIZARRE4, FALSE, TRUE, TRUE, FALSE, FALSE)))
						no_trump = TRUE;
				}
				else if (die < 88)
				{
#ifdef JP
msg_print("ͧ��Ū�ʥ�󥹥����γ�����");
#else
					msg_print("It's the picture of a friendly monster.");
#endif

					if (!(summon_specific(-1, py, px, (dun_level * 3) / 2, SUMMON_BIZARRE5, FALSE, TRUE, TRUE, FALSE, FALSE)))
						no_trump = TRUE;
				}
				else if (die < 96)
				{
#ifdef JP
msg_print("�����͡դ���");
#else
					msg_print("It's the Lovers.");
#endif

					if (get_aim_dir(&dir))
						(void)charm_monster(dir, MIN(p_ptr->lev, 20));
				}
				else if (die < 101)
				{
#ifdef JP
msg_print("�Ա��ԡդ���");
#else
					msg_print("It's the Hermit.");
#endif

					wall_stone();
				}
				else if (die < 111)
				{
#ifdef JP
msg_print("�Կ�Ƚ�դ���");
#else
					msg_print("It's the Judgement.");
#endif

					do_cmd_rerate(FALSE);
					if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
					{
#ifdef JP
msg_print("���Ƥ������Ѱۤ����ä���");
#else
						msg_print("You are cured of all mutations.");
#endif

						p_ptr->muta1 = p_ptr->muta2 = p_ptr->muta3 = 0;
						p_ptr->update |= PU_BONUS;
						handle_stuff();
					}
				}
				else if (die < 120)
				{
#ifdef JP
msg_print("�����ۡդ���");
#else
					msg_print("It's the Sun.");
#endif

					chg_virtue(V_KNOWLEDGE, 1);
					chg_virtue(V_ENLIGHTEN, 1);
					wiz_lite(FALSE, FALSE);
				}
				else
				{
#ifdef JP
msg_print("�������դ���");
#else
					msg_print("It's the World.");
#endif

					if (p_ptr->exp < PY_MAX_EXP)
					{
						s32b ee = (p_ptr->exp / 25) + 1;
						if (ee > 5000) ee = 5000;
#ifdef JP
msg_print("���˷и����Ѥ���褦�ʵ������롣");
#else
						msg_print("You feel more experienced.");
#endif

						gain_exp(ee);
					}
				}
			}
			break;
		case 3: /* Reset Recall */
			if (success)
			{
				if (!reset_recall()) return FALSE;
			}
			break;
		case 4: /* Teleport Self */
			if (success)
			{
				teleport_player(plev * 4);
			}
			break;
		case 5: /* Trump Spying */
			if (success)
			{
				(void)set_tim_esp(randint(30) + 25, FALSE);
			}
			break;
		case 6: /* Teleport Away */
			if (success)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				(void)fire_beam(GF_AWAY_ALL, dir, plev);
			}
			break;
		case 7: /* Trump Animals */
		{
			bool pet = success; /* was (randint(5) > 2) */
			int type = (pet ? SUMMON_ANIMAL_RANGER : SUMMON_ANIMAL);
			bool group = (pet ? FALSE : TRUE);

#ifdef JP
msg_print("���ʤ���ưʪ�Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of an animal...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, type, group, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("�������줿ưʪ���ܤäƤ��롪");
#else
					msg_print("The summoned animal gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 8: /* Trump Reach */
			if (success)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fetch(dir, plev * 15, TRUE);
			}
			break;
		case 9: /* Trump Kamikaze */
		{
			int x = px, y = py;
			if (success)
			{
				if (!target_set(TARGET_KILL)) return FALSE;
				x = target_col;
				y = target_row;
			}
			no_trump = TRUE;

#ifdef JP
msg_print("���ʤ��ϥ��ߥ����Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on several trumps at once...");
#endif


			for (dummy = 2 + rand_int(plev / 7); dummy > 0; dummy--)
			{
				bool pet = success; /* was (randint(10) > 3) */
				bool group = (pet ? FALSE : TRUE);
				int type;

				if (p_ptr->pclass == CLASS_BEASTMASTER)
				{
					type = SUMMON_KAMIKAZE_LIVING;
				}
				else
				{
					type = SUMMON_KAMIKAZE;
				}

				if (summon_specific((pet ? -1 : 0), y, x, summon_lev, type, group, FALSE, pet, FALSE, !pet))
				{
					if (!pet)
#ifdef JP
msg_print("���Ԥ��줿��󥹥������ܤäƤ��롪");
#else
						msg_print("The summoned creatures get angry!");
#endif

					no_trump = FALSE;
				}
			}
			break;
		}
		case 10: /* Phantasmal Servant */
			if (success)
			{
				if (summon_specific(-1, py, px, (summon_lev * 3) / 2, SUMMON_PHANTOM, FALSE, TRUE, TRUE, FALSE, FALSE))
				{
#ifdef JP
msg_print("���ѤǤ������ޤ����������͡�");
#else
					msg_print("'Your wish, master?'");
#endif

				}
				else
				{
					no_trump = TRUE;
				}
			}
			break;
		case 11: /* Speed Monster */
			if (success)
			{
				bool old_target_pet = target_pet;
				target_pet = TRUE;
				if (!get_aim_dir(&dir))
				{
					target_pet = old_target_pet;
					return (FALSE);
				}
				target_pet = old_target_pet;
				(void)speed_monster(dir);
			}
			break;
		case 12: /* Teleport Level */
			if (success)
			{
#ifdef JP
				if (!get_check("������¾�γ��˥ƥ�ݡ��Ȥ��ޤ�����")) return FALSE;
#else
				if (!get_check("Are you sure? (Teleport Level)")) return FALSE;
#endif
				(void)teleport_player_level();
			}
			break;
		case 13: /* Dimension Door */
			if (success)
			{
#ifdef JP
msg_print("�������⤬����������Ū�Ϥ�����ǲ�������");
#else
				msg_print("You open a dimensional gate. Choose a destination.");
#endif

				return dimension_door();
			}
			break;
		case 14: /* Word of Recall */
			if (success)
			{
				if (!word_of_recall()) return FALSE;
			}
			break;
		case 15: /* Banish */
			if (success)
			{
				banish_monsters(plev * 4);
			}
			break;
		case 16: /* Swap Position */
		{
			if (success)
			{
				project_length = -1;
				if (!get_aim_dir(&dir))
				{
					project_length = 0;
					return FALSE;
				}
				project_length = 0;

				(void)teleport_swap(dir);
			}
			break;
		}
		case 17: /* Trump Undead */
		{
			bool pet = success; /* (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

#ifdef JP
msg_print("���ʤ��ϥ���ǥåɤΥ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of an undead creature...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_UNDEAD, group, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿����ǥåɤ��ܤäƤ��롪");
#else
					msg_print("The summoned undead creature gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 18: /* Trump Reptiles */
		{
			bool pet = success; /* was (randint(5) > 2) */
			bool group = !pet;

#ifdef JP
msg_print("���ʤ��������Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of a reptile...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HYDRA, group, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿�������ܤäƤ��롪");
#else
					msg_print("The summoned reptile gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 19: /* Trump Monsters */
		{
			no_trump = TRUE;

#ifdef JP
msg_print("���ʤ��ϥ�󥹥����Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on several trumps at once...");
#endif


			for (dummy = 0; dummy < 1 + ((plev - 15)/ 10); dummy++)
			{
				bool pet = success; /* was (randint(10) > 3) */
				bool group = (pet ? FALSE : TRUE);
				int type;

				if (p_ptr->pclass == CLASS_BEASTMASTER)
				{
					type = SUMMON_LIVING;
				}
				else
				{
					type = 0;
				}

				if (summon_specific((pet ? -1 : 0), py, px, summon_lev, type, group, FALSE, pet, unique_okay, !pet))
				{
					if (!pet)
#ifdef JP
msg_print("���Ԥ��줿��󥹥������ܤäƤ��롪");
#else
						msg_print("The summoned creatures get angry!");
#endif

					no_trump = FALSE;
				}
			}
			break;
		}
		case 20: /* Trump Hounds */
		{
			bool pet = success; /* was (randint(5) > 2) */

#ifdef JP
msg_print("���ʤ��ϥϥ���ɤΥ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of a hound...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HOUND, TRUE, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿�ϥ���ɤ��ܤäƤ��롪");
#else
					msg_print("The summoned hounds get angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 21: /* Trump Branding */
			if (success)
			{
				brand_weapon(5);
			}
			break;
		case 22: /* Living Trump */
			if (success)
			{
				if (randint(7) == 1)
					/* Teleport control */
					dummy = 12;
				else
					/* Random teleportation (uncontrolled) */
					dummy = 77;
				/* Gain the mutation */
				if (gain_random_mutation(dummy))
#ifdef JP
msg_print("���ʤ��������Ƥ��륫���ɤ��Ѥ�ä���");
#else
					msg_print("You have turned into a Living Trump.");
#endif

			}
			break;
		case 23: /* Trump Cyberdemon */
		{
			bool pet = success; /* was (randint(10) > 3) */

#ifdef JP
msg_print("���ʤ��ϥ����С��ǡ����Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of a Cyberdemon...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_CYBER, FALSE, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿�����С��ǡ������ܤäƤ��롪");
#else
					msg_print("The summoned Cyberdemon gets angry!");
#endif

			}
			else
			{
					no_trump = TRUE;
			}

			break;
		}
		case 24: /* Trump Divination */
			if (success)
			{
				(void)detect_all(DETECT_RAD_DEFAULT);
			}
			break;
		case 25: /* Trump Lore */
			if (success)
			{
				return identify_fully(FALSE);
			}
			break;
		case 26: /* Heal Monster */
			if (success)
			{
				bool old_target_pet = target_pet;
				target_pet = TRUE;
				if (!get_aim_dir(&dir))
				{
					target_pet = old_target_pet;
					return (FALSE);
				}
				target_pet = old_target_pet;

				(void)heal_monster(dir, p_ptr->lev * 10 + 200);
			}
			break;
		case 27: /* Trump Dragon */
		{
			bool pet = success; /* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

#ifdef JP
msg_print("���ʤ��ϥɥ饴��Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of a dragon...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_DRAGON, group, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿�ɥ饴����ܤäƤ��롪");
#else
					msg_print("The summoned dragon gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 28: /* Trump Meteor */
			if (success)
			{
				int x, y, dx, dy, i;
				int b = 10 + randint(10);
				for (i = 0; i < b; i++)
				{
					int count = 0, d = 0;

					while (1)
					{
						count++;
						if (count > 20) break;
						x = px - 8 + rand_int(17);
						y = py - 8 + rand_int(17);

						if (!in_bounds(y,x) || (!cave_floor_bold(y,x) && (cave[y][x].feat != FEAT_TREES)) || !player_has_los_bold(y, x)) continue;

						dx = (px > x) ? (px - x) : (x - px);
						dy = (py > y) ? (py - y) : (y - py);

						/* Approximate distance */
						d = (dy > dx) ? (dy + (dx >> 1)) : (dx + (dy >> 1));
						if (d < 9) break;
					}

					if (count > 20) continue;

					project(0, 2, y, x, plev * 2, GF_METEOR, PROJECT_KILL | PROJECT_JUMP | PROJECT_ITEM, -1);
				}
			}
			break;
		case 29: /* Trump Demon */
		{
			bool pet = success; /* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

#ifdef JP
msg_print("���ʤ��ϥǡ����Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of a demon...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_DEMON, group, FALSE, pet, FALSE, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿�ǡ������ܤäƤ��롪");
#else
					msg_print("The summoned demon gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 30: /* Trump Greater Undead */
		{
			bool pet = success; /* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);

#ifdef JP
msg_print("���ʤ��϶��Ϥʥ���ǥåɤΥ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of a greater undead being...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, SUMMON_HI_UNDEAD, group, FALSE, pet, unique_okay, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿��饢��ǥåɤ��ܤäƤ��롪");
#else
					msg_print("The summoned greater undead creature gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		case 31: /* Trump Ancient Dragon */
		{
			bool pet = success; /* was (randint(10) > 3) */
			bool group = (pet ? FALSE : TRUE);
			int type;
			if (p_ptr->pclass == CLASS_BEASTMASTER)
			{
				type = SUMMON_HI_DRAGON_LIVING;
			}
			else
			{
				type = SUMMON_HI_DRAGON;
			}

#ifdef JP
msg_print("���ʤ��ϸ���ɥ饴��Υ����ɤ˽��椹��...");
#else
			msg_print("You concentrate on the trump of an ancient dragon...");
#endif


			if (summon_specific((pet ? -1 : 0), py, px, summon_lev, type, group, FALSE, pet, unique_okay, !pet))
			{
				if (!pet)
#ifdef JP
msg_print("���Ԥ��줿����ɥ饴����ܤäƤ��롪");
#else
					msg_print("The summoned ancient dragon gets angry!");
#endif

			}
			else
			{
				no_trump = TRUE;
			}

			break;
		}
		default:
#ifdef JP
msg_format("̤�ΤΥ����ɤμ�ʸ�Ǥ�: %d", spell);
#else
			msg_format("You cast an unknown Trump spell: %d.", spell);
#endif

			msg_print(NULL);
	}

	if (no_trump)
	{
#ifdef JP
msg_print("ï�⤢�ʤ��Υ����ɤθƤ����������ʤ���");
#else
		msg_print("Nobody answers to your Trump call.");
#endif

	}

	return TRUE;
}


static bool cast_arcane_spell(int spell)
{
	int	dir;
	int	beam;
	int	plev = p_ptr->lev;
	int	dummy = 0;
	bool	no_trump = FALSE;

	if (p_ptr->pclass == CLASS_MAGE) beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER) beam = plev + 10;
	else beam = plev / 2;

	switch (spell)
	{
	case 0: /* Zap */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam - 10, GF_ELEC, dir,
			damroll(3 + ((plev - 1) / 5), 3));
		break;
	case 1: /* Wizard Lock */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)wizard_lock(dir);
		break;
	case 2: /* Detect Invisibility */
		(void)detect_monsters_invis(DETECT_RAD_DEFAULT);
		break;
	case 3: /* Detect Monsters */
		(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
		break;
	case 4: /* Blink */
		teleport_player(10);
		break;
	case 5: /* Light Area */
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 6: /* Trap & Door Destruction */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)destroy_door(dir);
		break;
	case 7: /* Cure Light Wounds */
		(void)hp_player(damroll(2, 8));
		(void)set_cut(p_ptr->cut - 10);
		break;
	case 8: /* Detect Doors & Traps */
		(void)detect_traps(DETECT_RAD_DEFAULT);
		(void)detect_doors(DETECT_RAD_DEFAULT);
		(void)detect_stairs(DETECT_RAD_DEFAULT);
		break;
	case 9: /* Phlogiston */
		phlogiston();
		break;
	case 10: /* Detect Treasure */
		(void)detect_treasure(DETECT_RAD_DEFAULT);
		(void)detect_objects_gold(DETECT_RAD_DEFAULT);
		break;
	case 11: /* Detect Enchantment */
		(void)detect_objects_magic(DETECT_RAD_DEFAULT);
		break;
	case 12: /* Detect Objects */
		(void)detect_objects_normal(DETECT_RAD_DEFAULT);
		break;
	case 13: /* Cure Poison */
		(void)set_poisoned(0);
		break;
	case 14: /* Resist Cold */
		(void)set_oppose_cold(randint(20) + 20, FALSE);
		break;
	case 15: /* Resist Fire */
		(void)set_oppose_fire(randint(20) + 20, FALSE);
		break;
	case 16: /* Resist Lightning */
		(void)set_oppose_elec(randint(20) + 20, FALSE);
		break;
	case 17: /* Resist Acid */
		(void)set_oppose_acid(randint(20) + 20, FALSE);
		break;
	case 18: /* Cure Medium Wounds */
		(void)hp_player(damroll(4, 8));
		(void)set_cut((p_ptr->cut / 2) - 50);
		break;
	case 19: /* Teleport */
		teleport_player(plev * 5);
		break;
	case 20: /* Identify */
		return ident_spell(FALSE);
	case 21: /* Stone to Mud */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)wall_to_mud(dir);
		break;
	case 22: /* Ray of Light */
		if (!get_aim_dir(&dir)) return FALSE;

#ifdef JP
msg_print("�����������줿��");
#else
		msg_print("A line of light appears.");
#endif

		(void)lite_line(dir);
		break;
	case 23: /* Satisfy Hunger */
		(void)set_food(PY_FOOD_MAX - 1);
		break;
	case 24: /* See Invisible */
		(void)set_tim_invis(randint(24) + 24, FALSE);
		break;
	case 25: /* Conjure Elemental */
		if (!summon_specific(-1, py, px, plev, SUMMON_ELEMENTAL, TRUE, TRUE, TRUE, FALSE, FALSE))
			no_trump = TRUE;
		break;
	case 26: /* Teleport Level */
		if (!get_check("������¾�γ��˥ƥ�ݡ��Ȥ��ޤ�����")) return FALSE;
		(void)teleport_player_level();
		break;
	case 27: /* Teleport Away */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)fire_beam(GF_AWAY_ALL, dir, plev);
		break;
	case 28: /* Elemental Ball */
		if (!get_aim_dir(&dir)) return FALSE;

		switch (randint(4))
		{
			case 1:  dummy = GF_FIRE;break;
			case 2:  dummy = GF_ELEC;break;
			case 3:  dummy = GF_COLD;break;
			default: dummy = GF_ACID;break;
		}
		fire_ball(dummy, dir, 75 + (plev), 2);
		break;
	case 29: /* Detection */
		(void)detect_all(DETECT_RAD_DEFAULT);
		break;
	case 30: /* Word of Recall */
		if (!word_of_recall()) return FALSE;
		break;
	case 31: /* Clairvoyance */
		chg_virtue(V_KNOWLEDGE, 1);
		chg_virtue(V_ENLIGHTEN, 1);
		wiz_lite(FALSE, FALSE);
		if (!p_ptr->telepathy)
		{
			(void)set_tim_esp(randint(30) + 25, FALSE);
		}
		break;
	default:
		msg_format("You cast an unknown Arcane spell: %d.", spell);
		msg_print(NULL);
	}

	if (no_trump)
#ifdef JP
msg_print("�����󥿥�ϸ���ʤ��ä���");
#else
		msg_print("No Elementals arrive.");
#endif

	return TRUE;
}


static bool cast_enchant_spell(int spell)
{
	int	plev = p_ptr->lev;
	int	dummy = 0;
	bool	no_trump = FALSE;

	switch (spell)
	{
	case 0: /* Infravision */
		set_tim_infra(100 + randint(100), FALSE);
		break;
	case 1: /* Regeneration */
		set_tim_regen(80 + randint(80), FALSE);
		break;
	case 2: /* Satisfy Hunger */
		(void)set_food(PY_FOOD_MAX - 1);
		break;
	case 3: /* Resist Cold */
		(void)set_oppose_cold(randint(20) + 20, FALSE);
		break;
	case 4: /* Resist Fire */
		(void)set_oppose_fire(randint(20) + 20, FALSE);
		break;
	case 5: /* Heroism */
		(void)set_hero(randint(25) + 25, FALSE);
		(void)hp_player(10);
		(void)set_afraid(0);
		break;
	case 6: /* Resist Lightning */
		(void)set_oppose_elec(randint(20) + 20, FALSE);
		break;
	case 7: /* Resist Acid */
		(void)set_oppose_acid(randint(20) + 20, FALSE);
		break;
	case 8: /* See Invisibility */
		(void)set_tim_invis(randint(24) + 24, FALSE);
		break;
	case 9: /* Remove Curse */
		if (remove_curse())
		{
#ifdef JP
			msg_print("ï���˸�����Ƥ���褦�ʵ������롣");
#else
			msg_print("You feel as if someone is watching over you.");
#endif
		}
		break;
	case 10: /* Resist Poison */
		(void)set_oppose_pois(randint(20) + 20, FALSE);
		break;
	case 11: /* Berserk */
		(void)set_shero(randint(25) + 25, FALSE);
		(void)hp_player(30);
		(void)set_afraid(0);
		break;
	case 12: /* Self Knowledge */
		(void)self_knowledge();
		break;
	case 13: /* Protection from Evil */
		(void)set_protevil(randint(25) + 3 * p_ptr->lev, FALSE);
		break;
	case 14: /* Healing */
		set_poisoned(0);
		set_stun(0);
		set_cut(0);
		set_image(0);
		break;
	case 15: /* Mana Branding */
		return choose_ele_attack();
		break;
	case 16: /* Telepathy */
		(void)set_tim_esp(randint(30) + 25, FALSE);
		break;
	case 17: /* Stone Skin */
		(void)set_shield(randint(20) + 30, FALSE);
		break;
	case 18: /* Resistance */
		(void)set_oppose_acid(randint(20) + 20, FALSE);
		(void)set_oppose_elec(randint(20) + 20, FALSE);
		(void)set_oppose_fire(randint(20) + 20, FALSE);
		(void)set_oppose_cold(randint(20) + 20, FALSE);
		(void)set_oppose_pois(randint(20) + 20, FALSE);
		break;
	case 19: /* Haste */
		(void)set_fast(randint(20 + plev) + plev, FALSE);
		break;
	case 20: /* Walk through Wall */
		(void)set_kabenuke(randint(plev/2) + plev/2, FALSE);
		break;
	case 21: /* Pulish Shield */
		(void)pulish_shield();
		break;
	case 22: /* Create Golem */
		if (summon_specific(-1, py, px, plev, SUMMON_GOLEM, FALSE, TRUE, TRUE, FALSE, FALSE))
		{
#ifdef JP
msg_print("���������ä���");
#else
		msg_print("You make a golem.");
#endif
		}
		else
		{
			no_trump = TRUE;
		}
		break;
	case 23: /* Magic armor */
		(void)set_magicdef(randint(20) + 20, FALSE);
		break;
	case 24: /* Remove Enchantment */
		if (!mundane_spell(TRUE)) return FALSE;
		break;
	case 25: /* Remove All Curse */
		if (remove_all_curse())
		{
#ifdef JP
			msg_print("ï���˸�����Ƥ���褦�ʵ������롣");
#else
			msg_print("You feel as if someone is watching over you.");
#endif
		}
		break;
	case 26: /* Total Knowledge */
		return identify_fully(FALSE);
		break;
	case 27: /* Enchant Weapon */
		return enchant_spell(rand_int(4) + 1, rand_int(4) + 1, 0);
		break;
	case 28: /* Enchant Armor */
		return enchant_spell(0, 0, rand_int(3) + 2);
		break;
	case 29: /* Brand Weapon */
		brand_weapon(rand_int(17));
		break;
	case 30: /* Living Trump */
		if (randint(7) == 1)
			/* Teleport control */
			dummy = 12;
		else
			/* Random teleportation (uncontrolled) */
			dummy = 77;
		/* Gain the mutation */
		if (gain_random_mutation(dummy))
#ifdef JP
msg_print("���ʤ��������Ƥ��륫���ɤ��Ѥ�ä���");
#else
			msg_print("You have turned into a Living Trump.");
#endif
		break;
	case 31: /* Immune */
		return (choose_ele_immune(13 + randint(13)));
		break;
	default:
		msg_format("You cast an unknown Craft spell: %d.", spell);
		msg_print(NULL);
	}

	if (no_trump)
#ifdef JP
msg_print("���ޤ�����������ʤ��ä���");
#else
		msg_print("No Golems arrive.");
#endif

	return TRUE;
}


/*
 * An "item_tester_hook" for offer
 */
static bool item_tester_offer(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval != TV_CORPSE) return (FALSE);

	if (o_ptr->sval != SV_CORPSE) return (FALSE);

	if (strchr("pht", r_info[o_ptr->pval].d_char)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


static bool cast_daemon_spell(int spell)
{
	int	dir, beam;
	int	plev = p_ptr->lev;

	if (p_ptr->pclass == CLASS_MAGE) beam = plev;
	else if (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_SORCERER) beam = plev + 10;
	else beam = plev / 2;

	switch (spell)
	{
	case 0: /* Magic Missile */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam - 10, GF_MISSILE, dir,
			damroll(3 + ((plev - 1) / 5), 4));
		break;
	case 1: /* Detect Undead & Demons -> Unlife */
		(void)detect_monsters_nonliving(DETECT_RAD_DEFAULT);
		break;
	case 2: /* Bless */
		(void)set_blessed(randint(12) + 12, FALSE);
		break;
	case 3: /* Resist Fire */
		(void)set_oppose_fire(randint(20) + 20, FALSE);
		break;
	case 4: /* Horrify */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)fear_monster(dir, plev);
		(void)stun_monster(dir, plev);
		break;
	case 5: /* Nether Bolt */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam, GF_NETHER, dir,
		    damroll(6 + ((plev - 5) / 4), 8));
		break;
	case 6: /* Summon monster, demon */
		if (!summon_specific(-1, py, px, (plev * 3) / 2, SUMMON_MANES, TRUE, FALSE, TRUE, FALSE, FALSE))
		{
#ifdef JP
msg_print("����λ���ϸ���ʤ��ä���");
#else
			msg_print("No Manes arrive.");
#endif
		}
		break;
	case 7: /* Mini Hellfire */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_HELL_FIRE, dir,
			(damroll(3, 6) + plev +
			(plev / (((p_ptr->pclass == CLASS_MAGE) ||
			(p_ptr->pclass == CLASS_HIGH_MAGE) ||
			(p_ptr->pclass == CLASS_SORCERER)) ? 2 : 4))),
			((plev < 30) ? 2 : 3));
		break;
	case 8: /* Enslave Demon */
		if (!get_aim_dir(&dir)) return FALSE;

		(void)control_one_demon(dir, plev);
		break;
	case 9: /* Vision */
		map_area(DETECT_RAD_MAP);
		break;
	case 10: /* Resist Nether */
		(void)set_tim_res_nether(randint(20) + 20, FALSE);
		break;
	case 11: /* Plasma Bolt */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_bolt_or_beam(beam, GF_PLASMA, dir,
		    damroll(11 + ((plev - 5) / 4), 8));
		break;
	case 12: /* Fire Ball */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_FIRE, dir, plev + 55, 2);
		break;
	case 13: /* Fire Branding */
		brand_weapon(1);
		break;
	case 14: /* Nether Ball */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_NETHER, dir, plev*3/2 + 100, plev / 20+2);
		break;
	case 15: /* Summon monster, demon */
	{
		bool pet = (randint(3) != 1);
		bool group = !(pet && (plev < 50));

		if (summon_specific((pet ? -1 : 0), py, px, plev*2/3+randint(plev/2), SUMMON_DEMON, group, FALSE, pet, FALSE, !pet))
		{
#ifdef JP
msg_print("β���ΰ���������������");
#else
			msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


			if (pet)
#ifdef JP
msg_print("�֤��ѤǤ������ޤ�����������͡�");
#else
				msg_print("'What is thy bidding... Master?'");
#endif

			else
#ifdef JP
msg_print("���ܤ����Ԥ衢�����β��ͤˤ��餺�� �����κ���ĺ��������");
#else
				msg_print("'NON SERVIAM! Wretch! I shall feast on thy mortal soul!'");
#endif

		}
		else
		{
#ifdef JP
msg_print("����ϸ���ʤ��ä���");
#else
			msg_print("No Greater Demon arrive.");
#endif
		}
		break;
	}
	case 16: /* Telepathy */
		(void)set_tim_esp(randint(30) + 25, FALSE);
		break;
	case 17: /* Demoncloak */
	{
		int dur=randint(20) + 20;
			
		set_oppose_fire(dur, FALSE);
		set_oppose_cold(dur, FALSE);
		set_tim_sh_fire(dur, FALSE);
		set_afraid(0);
		break;
	}
	case 18: /* Rain of Lava */
		fire_ball(GF_FIRE, 0, (55 + plev)*2, 3);
		fire_ball_hide(GF_LAVA_FLOW, 0, 2+randint(2), 3);
		break;
	case 19: /* Plasma ball */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_PLASMA, dir, plev*3/2 + 80, 2 + plev/40);
		break;
	case 20: /* Mimic demon */
		(void)set_mimic(10+plev/2 + randint(10+plev/2), MIMIC_DEMON, FALSE);
		break;
	case 21: /* Nether Wave == Dispel Good */
		(void)dispel_monsters(randint(plev * 2));
		(void)dispel_good(randint(plev * 2));
		break;
	case 22: /*  */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_NEXUS, dir, 100 + plev*2, 4);
		break;
	case 23: /* Hand Doom */
		if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
else msg_print("<���Ǥμ�>�����ä���");
#else
		else msg_print("You invokes the Hand of Doom!");
#endif

		fire_ball_hide(GF_HAND_DOOM, dir, plev * 2, 0);
		break;
	case 24: /* Heroism */
		(void)set_hero(randint(25) + 25, FALSE);
		(void)hp_player(10);
		(void)set_afraid(0);
		break;
	case 25: /* Tim resist time */
		(void)set_tim_res_time(randint(20)+20, FALSE);
		break;
	case 26: /* Circle of Madness */
		fire_ball(GF_CHAOS, 0, 50+plev, 3+plev/20);
		fire_ball(GF_CONFUSION, 0, 50+plev, 3+plev/20);
		fire_ball(GF_CHARM, 0, 20+plev, 3+plev/20);
		break;
	case 27: /* True Discharge Minion */
		discharge_minion(TRUE);
		break;
	case 28: /* Summon Greater Demon */
	{
		int item;
		cptr q, s;
		int summon_lev;
		object_type *o_ptr;

		item_tester_hook = item_tester_offer;
#ifdef JP
		q = "�ɤλ��Τ������ޤ���? ";
		s = "����������Τ���äƤ��ʤ���";
#else
		q = "Sacrifice which corpse? ";
		s = "You have nothing to scrifice.";
#endif
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			o_ptr = &inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			o_ptr = &o_list[0 - item];
		}

		summon_lev = p_ptr->lev * 2 / 3 + r_info[o_ptr->pval].level;
		if (summon_specific(-1, py, px, summon_lev, SUMMON_HI_DEMON, TRUE, FALSE, TRUE, FALSE, FALSE))
		{
#ifdef JP
msg_print("β���ΰ���������������");
#else
			msg_print("The area fills with a stench of sulphur and brimstone.");
#endif


#ifdef JP
msg_print("�֤��ѤǤ������ޤ�����������͡�");
#else
			msg_print("'What is thy bidding... Master?'");
#endif

			/* Decrease the item (from the pack) */
			if (item >= 0)
			{
				inven_item_increase(item, -1);
				inven_item_describe(item);
				inven_item_optimize(item);
			}

			/* Decrease the item (from the floor) */
			else
			{
				floor_item_increase(0 - item, -1);
				floor_item_describe(0 - item);
				floor_item_optimize(0 - item);
			}
		}
		else
		{
#ifdef JP
msg_print("����ϸ���ʤ��ä���");
#else
			msg_print("No Greater Demon arrive.");
#endif
		}
		break;
	}
	case 29: /* Nether Storm */
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball(GF_NETHER, dir, plev*15, plev / 5);
		break;
	case 30: /* Blood curse */
	{
		if (!get_aim_dir(&dir)) return FALSE;

		fire_ball_hide(GF_BLOOD_CURSE, dir, 600, 0);
#ifdef JP
take_hit(DAMAGE_USELIFE, 20 + randint(30), "��μ���", -1);
#else
		take_hit(DAMAGE_USELIFE, 20 + randint(30), "Blood curse", -1);
#endif
		break;
	}
	case 31: /* Mimic Demon lord */
		(void)set_mimic(15 + randint(15), MIMIC_DEMON_LORD, FALSE);
		break;
	default:
		msg_format("You cast an unknown Daemon spell: %d.", spell);
		msg_print(NULL);
	}

	return TRUE;
}


void stop_singing(void)
{
	if (p_ptr->pclass != CLASS_BARD) return;

	if (p_ptr->magic_num1[1])
	{
		p_ptr->magic_num1[1] = 0;
		return;
	}
	if (!p_ptr->magic_num1[0]) return;

	set_action(ACTION_NONE);

        switch(p_ptr->magic_num1[0])
        {
                case MUSIC_BLESS:
                        if (!p_ptr->blessed)
#ifdef JP
msg_print("���ʵ�ʬ���ä���������");
#else
				msg_print("The prayer has expired.");
#endif
                        break;
                case MUSIC_HERO:
                        if (!p_ptr->hero)
			{
#ifdef JP
msg_print("�ҡ����ε�ʬ���ä���������");
#else
				msg_print("The heroism wears off.");
#endif
				/* Recalculate hitpoints */
				p_ptr->update |= (PU_HP);
			}
                        break;
                case MUSIC_MIND:
                        if (!p_ptr->tim_esp)
			{
#ifdef JP
msg_print("�ռ��ϸ�����ä���");
#else
				msg_print("Your consciousness contracts again.");
#endif
				/* Update the monsters */
				p_ptr->update |= (PU_MONSTERS);
			}
                        break;
                case MUSIC_STEALTH:
                        if (!p_ptr->tim_stealth)
#ifdef JP
msg_print("�Ѥ��Ϥä���ȸ�����褦�ˤʤä���");
#else
				msg_print("You are no longer hided.");
#endif
                        break;
                case MUSIC_RESIST:
                        if (!p_ptr->oppose_acid)
#ifdef JP
msg_print("���ؤ����������줿�������롣");
#else
				msg_print("You feel less resistant to acid.");
#endif
                        if (!p_ptr->oppose_elec)
#ifdef JP
msg_print("�ŷ�ؤ����������줿�������롣");
#else
				msg_print("You feel less resistant to elec.");
#endif
                        if (!p_ptr->oppose_fire)
#ifdef JP
msg_print("�Фؤ����������줿�������롣");
#else
				msg_print("You feel less resistant to fire.");
#endif
                        if (!p_ptr->oppose_cold)
#ifdef JP
msg_print("�䵤�ؤ����������줿�������롣");
#else
				msg_print("You feel less resistant to cold.");
#endif
                        if (!p_ptr->oppose_pois)
#ifdef JP
msg_print("�Ǥؤ����������줿�������롣");
#else
				msg_print("You feel less resistant to pois.");
#endif
                        break;
                case MUSIC_SPEED:
                        if (!p_ptr->fast)
#ifdef JP
msg_print("ư�������ᤵ���ʤ��ʤä��褦����");
#else
				msg_print("You feel yourself slow down.");
#endif
                        break;
                case MUSIC_SHERO:
                        if (!p_ptr->hero)
			{
#ifdef JP
msg_print("�ҡ����ε�ʬ���ä���������");
#else
				msg_print("The heroism wears off.");
#endif
				/* Recalculate hitpoints */
				p_ptr->update |= (PU_HP);
			}

                        if (!p_ptr->fast)
#ifdef JP
msg_print("ư�������ᤵ���ʤ��ʤä��褦����");
#else
				msg_print("You feel yourself slow down.");
#endif
                        break;
                case MUSIC_INVULN:
                        if (!p_ptr->invuln)
			{
#ifdef JP
msg_print("̵Ũ�ǤϤʤ��ʤä���");
#else
				msg_print("The invulnerability wears off.");
#endif
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
                        break;
        }
	p_ptr->magic_num1[0] = MUSIC_NONE;
	p_ptr->magic_num2[0] = 0;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
}


static bool cast_music_spell(int spell)
{
	int	plev = p_ptr->lev;
	int dir;

        if(p_ptr->magic_num1[0])
        {
                stop_singing();
        }

        p_ptr->magic_num2[0] = spell;

	switch (spell)
	{
	case 0: /* Song of Holding ���ߤβ�*/
#ifdef JP
		msg_print("��ä���Ȥ������ǥ���������߻Ϥ᤿������");
#else
		msg_print("You start humming a slow, steady melody...");
#endif
		p_ptr->magic_num1[0] = MUSIC_SLOW;
		break;
	case 1:  /* Song of Blessing ��ʡ�β� */
#ifdef JP
		msg_print("�����ʥ��ǥ����դǻϤ᤿������");
#else
		msg_print("The holy power of the Music of the Ainur enters you...");
#endif
		p_ptr->magic_num1[0] = MUSIC_BLESS;
		break;
		
	case 2:  /* Wrecking Note �����β��� */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_bolt(GF_SOUND, dir,
			  damroll(4 + ((plev - 1) / 5), 4));
		break;
	case 3:  /* Stun Pattern ۯ۰����Χ */
#ifdef JP
		msg_print("���Ǥ�������ǥ����դǻϤ᤿������");
#else
		msg_print("You weave a pattern of sounds to bewilder and daze...");
#endif
		p_ptr->magic_num1[0] = MUSIC_STUN;
		break;
	case 4:  /* Flow of life ��̿��ή�� */
#ifdef JP
		msg_print("�Τ��̤����Τ˳赤����äƤ���������");
#else
		msg_print("Life flows through you as you sing a song of healing...");
#endif
		p_ptr->magic_num1[0] = MUSIC_L_LIFE;
		break;
	case 5:  /* Song of the Sun ���ۤβ� */
#ifdef JP
		msg_print("���굱���Τ��դ��Ȥ餷����");
#else
		msg_print("Your uplifting song brings brightness to dark places...");
#endif
		(void)lite_area(damroll(2, (plev / 2)), (plev / 10) + 1);
		break;
	case 6:  /* Song of fear ���ݤβ� */
#ifdef JP
		msg_print("���ɤ��ɤ������ǥ����դǻϤ᤿������");
#else
		msg_print("You start weaving a fearful pattern...");
#endif
		p_ptr->magic_num1[0] = MUSIC_FEAR;
		break;
	case 7:  /* Heroic Ballad �襤�β� */
#ifdef JP
		msg_print("�㤷���襤�βΤ�Τä�������");
#else
		msg_print("You start singing a song of intense fighting...");
#endif
		p_ptr->magic_num1[0] = MUSIC_HERO;
		break;
	case 8:  /* Clairaudience ��Ū�γ� */
#ifdef JP
		msg_print("�Ť��ʲ��ڤ����Ф򸦤����ޤ�����������");
#else
		msg_print("Your quiet music sharpens your sense of hearing...");
#endif
		p_ptr->magic_num1[0] = MUSIC_DETECT;
		break;
	case 9: /* ���β� */
#ifdef JP
		msg_print("������Ǳ���ʤ���Τ�Τä�������");
#else
		msg_print("You start singing a song of soul in pain...");
#endif
		p_ptr->magic_num1[0] = MUSIC_PSI;
		break;
	case 10:  /* Song of Lore �μ��β� */
#ifdef JP
		msg_print("�����������μ���ή�����Ǥ���������");
#else
		msg_print("You recall the rich lore of the world...");
#endif
		p_ptr->magic_num1[0] = MUSIC_ID;
		break;
	case 11:  /* hidding song ���ۤβ� */
#ifdef JP
		msg_print("���ʤ��λѤ��ʿ��ˤȤ�����Ǥ��ä�������");
#else
		msg_print("Your song carries you beyond the sight of mortal eyes...");
#endif
		p_ptr->magic_num1[0] = MUSIC_STEALTH;
		break;
	case 12:  /* Illusion Pattern ���Ƥ���Χ */
#ifdef JP
		msg_print("�դ���̤˸��Ƥ����줿������");
#else
		msg_print("You weave a pattern of sounds to beguile and confuse...");
#endif
		p_ptr->magic_num1[0] = MUSIC_CONF;
		break;
	case 13:  /* Doomcall (vibration song) ���Ǥζ��� */
#ifdef JP
		msg_print("�첻��������������");
#else
		msg_print("The fury of the Downfall of Numenor lashes out...");
#endif
		p_ptr->magic_num1[0] = MUSIC_SOUND;
		break;
	case 14:  /* Firiel's Song (song of the Undeads) �ե��ꥨ��β� */
#ifdef JP
		msg_print("��̿������Υơ��ޤ��դǻϤ᤿������");
#else
		msg_print("The themes of life and revival are woven into your song...");
#endif
		animate_dead(0, py, px);
		break;
	case 15:  /* Fellowship Chant (charming song) ι����� */
#ifdef JP
		msg_print("�¤餫�ʥ��ǥ����դǻϤ᤿������");
#else
		msg_print("You weave a slow, soothing melody of imploration...");
#endif
		p_ptr->magic_num1[0] = MUSIC_CHARM;
		break;
	case 16:  /* (wall breaking song) ʬ���� */
#ifdef JP
		msg_print("ʴ�դ�����ǥ����դǻϤ᤿������");
#else
		msg_print("You weave a violent pattern of sounds to break wall.");
#endif
		p_ptr->magic_num1[0] = MUSIC_WALL;
		project(0, 0, py, px,
			0, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE, -1);
		break;
	case 17:  /* Finrod's Resistance (song of resistance) �������� */
#ifdef JP
		msg_print("���Ǥ��Ϥ��Ф���Ǧ�ѤβΤ�Τä���");
#else
		msg_print("You sing a song of perseverance against powers...");
#endif
		p_ptr->magic_num1[0] = MUSIC_RESIST;
		break;
	case 18:  /* Hobbit Melodies (song of time) �ۥӥåȤΥ��ǥ� */
#ifdef JP
		msg_print("�ڲ��ʲΤ�������߻Ϥ᤿������");
#else
		msg_print("You start singing joyful pop song...");
#endif
		p_ptr->magic_num1[0] = MUSIC_SPEED;
		break;
	case 19:  /* World Contortion �Ĥ������ */
#ifdef JP
		msg_print("�Τ����֤��Ĥ᤿������");
#else
		msg_print("Reality whirls wildly as you sing a dizzying melody...");
#endif
		project(0, plev/15 + 1, py, px, plev * 3 + 1, GF_AWAY_ALL , PROJECT_KILL, -1);
		break;
	case 20: /* �໶�β� */
#ifdef JP
		msg_print("�Ѥ����ʤ��Զ��²���Ũ���դ�Ω�Ƥ�������");
#else
		msg_print("You cry out in an ear-wracking voice...");
#endif
		p_ptr->magic_num1[0] = MUSIC_DISPEL;
		break;
	case 21: /* The Voice of Saruman ����ޥ�δŸ� */
#ifdef JP
		msg_print("ͥ������̥��Ū�ʲΤ�������߻Ϥ᤿������");
#else
		msg_print("You start humming a gentle and attractive song...");
#endif
		p_ptr->magic_num1[0] = MUSIC_SARUMAN;
		break;
	case 22:  /* Song of Tempest (song of death) ��β��� */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_beam(GF_SOUND, dir,
			  damroll(15 + ((plev - 1) / 2), 10));
		break;
	case 23:  /* (song of disruption) �⤦��Ĥ����� */
#ifdef JP
		msg_print("���Ϥ��Ѳ����Ϥ᤿������");
#else
		msg_print("You sing of the primeval shaping of Middle-earth...");
#endif
		alter_reality();
		break;
	case 24:  /* Wrecking Pattern (destruction shriek) �˲�����Χ */
#ifdef JP
		msg_print("�˲�Ū�ʲΤ������錄�ä�������");
#else
		msg_print("You weave a pattern of sounds to contort and shatter...");
#endif
		p_ptr->magic_num1[0] = MUSIC_QUAKE;
		break;
	case 25: /* ���ڤβ�  */
#ifdef JP
		msg_print("��ä���Ȥ������ǥ����դǻϤ᤿������");
#else
		msg_print("You weave a very slow pattern which is almost likely to stop...");
#endif
		p_ptr->magic_num1[0] = MUSIC_STASIS;
		break;
	case 26: /* ���β� */
#ifdef JP
		msg_print("�Τ������ʾ����Ф���������");
#else
		msg_print("The holy power of the Music is creating sacred field...");
#endif
		warding_glyph();
		break;
	case 27: /* ��ͺ�λ� */
#ifdef JP
		msg_print("��ͺ�βΤ���������������");
#else
		msg_print("You chant a powerful, heroic call to arms...");
#endif
		p_ptr->magic_num1[0] = MUSIC_SHERO;
		(void)hp_player(10);
		(void)set_afraid(0);
		break;
	case 28: /* �������ʤν��� */
#ifdef JP
		msg_print("�Τ��̤����Τ˳赤����äƤ���������");
#else
		msg_print("Life flows through you as you sing the song...");
#endif
		p_ptr->magic_num1[0] = MUSIC_H_LIFE;
		break;
	case 29: /* �����β� */
#ifdef JP
		msg_print("�Ź�����˸�������դ�ޤ������Τ����γ��Ϥ����ᤷ����");
#else
		msg_print("You strewed light and beauty in the dark as you sing. You feel refreshed.");
#endif
		(void)do_res_stat(A_STR);
		(void)do_res_stat(A_INT);
		(void)do_res_stat(A_WIS);
		(void)do_res_stat(A_DEX);
		(void)do_res_stat(A_CON);
		(void)do_res_stat(A_CHR);
		(void)restore_level();
		break;
	case 30:  /* shriek of death ����������� */
		if (!get_aim_dir(&dir)) return FALSE;
		fire_ball(GF_SOUND, dir, damroll(50 + plev, 10), 0);
		break;
	case 31:  /* song of liberty �ե��󥴥�ե����ĩ�� */
#ifdef JP
		msg_print("�ե��󥴥�ե����̽���ؤ�ĩ���Τä�������");
#else
		msg_print("You recall the valor of Fingolfin's challenge to the Dark Lord...");
#endif
		p_ptr->magic_num1[0] = MUSIC_INVULN;
		
		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);
		
		/* Update monsters */
		p_ptr->update |= (PU_MONSTERS);
		
		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		break;
	default:
#ifdef JP
		msg_format("̤�Τβ�(%d)��Τä���", spell);
#else
		msg_format("You sing an unknown song: %d.", spell);
#endif
		msg_print(NULL);
	}

	if (p_ptr->magic_num1[0]) set_action(ACTION_SING);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_HP);

	/* Redraw status bar */
	p_ptr->redraw |= (PR_STATUS);
	return TRUE;
}


/*
 * Cast a spell
 */
void do_cmd_cast(void)
{
#ifdef USE_SCRIPT
	use_skill_callback();
#else /* USE_SCRIPT */
	int	item, sval, spell, realm;
	int	chance;
	int	increment = 0;
	int	use_realm;
	int	shouhimana;
	bool cast;

	cptr prayer;

	object_type	*o_ptr;

	magic_type	*s_ptr;

	cptr q, s;

	/* Require spell ability */
	if (!p_ptr->realm1 && (p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE))
	{
#ifdef JP
msg_print("��ʸ�򾧤����ʤ���");
#else
		msg_print("You cannot cast spells!");
#endif

		return;
	}

	/* Require lite */
	if (p_ptr->blind || no_lite())
	{
#ifdef JP
msg_print("�ܤ������ʤ���");
#else
		msg_print("You cannot see!");
#endif
		if (p_ptr->pclass == CLASS_FORCETRAINER)
		    do_cmd_mind();
		else
			flush();
		return;
	}

	/* Not when confused */
	if (p_ptr->confused)
	{
#ifdef JP
msg_print("���𤷤Ƥ��ƾ������ʤ���");
#else
		msg_print("You are too confused!");
#endif
		flush();
		return;
	}

	prayer = spell_categoly_name(mp_ptr->spell_book);

	/* Restrict choices to spell books */
	item_tester_tval = mp_ptr->spell_book;

	/* Get an item */
#ifdef JP
q = "�ɤμ�ʸ���Ȥ��ޤ���? ";
#else
	q = "Use which book? ";
#endif

#ifdef JP
s = "��ʸ�񤬤ʤ���";
#else
	s = "You have no spell books!";
#endif

        select_spellbook=TRUE;
	if (p_ptr->pclass == CLASS_FORCETRAINER)
		select_the_force = TRUE;
	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))){
            select_spellbook = FALSE;
	    select_the_force = FALSE;
            return;
        }
        select_spellbook = FALSE;
	select_the_force = FALSE;

	if (item == 1111) { /* the_force */
	    do_cmd_mind();
	    return;
	} else
	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}

#ifdef USE_SCRIPT
	if (object_cast_callback(o_ptr)) return;
#endif /* USE_SCRIPT */

	/* Access the item's sval */
	sval = o_ptr->sval;

	if ((p_ptr->pclass != CLASS_SORCERER) && (p_ptr->pclass != CLASS_RED_MAGE) && (o_ptr->tval == REALM2_BOOK)) increment = 32;


	/* Track the object kind */
	object_kind_track(o_ptr->k_idx);

	/* Hack -- Handle stuff */
	handle_stuff();

	if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
		realm = o_ptr->tval - TV_LIFE_BOOK + 1;
	else if (increment) realm = p_ptr->realm2;
	else realm = p_ptr->realm1;

	/* Ask for a spell */
#ifdef JP
        if (!get_spell(&spell,  
		                ((mp_ptr->spell_book == TV_LIFE_BOOK) ? "�Ӿ�����" : (mp_ptr->spell_book == TV_MUSIC_BOOK) ? "�Τ�" : "������"), 
		       sval, TRUE, realm))
        {
                if (spell == -2) msg_format("�����ܤˤ��ΤäƤ���%s���ʤ���", prayer);
                return;
        }
#else
	if (!get_spell(&spell, ((mp_ptr->spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
		sval, TRUE, realm))
	{
		if (spell == -2)
			msg_format("You don't know any %ss in that book.", prayer);
		return;
	}
#endif


	use_realm = tval2realm(o_ptr->tval);

	if (!is_magic(use_realm))
	{
		s_ptr = &technic_info[use_realm - MIN_TECHNIC - 1][spell];
	}
	else
	{
		s_ptr = &mp_ptr->info[realm - 1][spell];
	}

	if (p_ptr->pclass == CLASS_SORCERER)
		shouhimana = s_ptr->smana*2200 + 2399;
	else if (p_ptr->pclass == CLASS_RED_MAGE)
		shouhimana = s_ptr->smana*2600 + 2399;
	else
		shouhimana = (s_ptr->smana*(3800-spell_exp[(increment ? spell+32 : spell)])+2399);
	if(p_ptr->dec_mana)
		shouhimana *= 3;
	else shouhimana *= 4;
	shouhimana /= 9600;
	if(shouhimana < 1) shouhimana = 1;

	/* Verify "dangerous" spells */
	if (shouhimana > p_ptr->csp)
	{
		/* Warning */
#ifdef JP
msg_format("����%s��%s�Τ˽�ʬ�ʥޥ��å��ݥ���Ȥ��ʤ���",prayer,
((mp_ptr->spell_book == TV_LIFE_BOOK) ? "�Ӿ�����" : (mp_ptr->spell_book == TV_LIFE_BOOK) ? "�Τ�" : "������"));
#else
		msg_format("You do not have enough mana to %s this %s.",
			((mp_ptr->spell_book == TV_LIFE_BOOK) ? "recite" : "cast"),
			prayer);
#endif


		if (!over_exert) return;

		/* Verify */
#ifdef JP
if (!get_check("����Ǥ�ĩ�路�ޤ���? ")) return;
#else
		if (!get_check("Attempt it anyway? ")) return;
#endif

	}


	/* Spell failure chance */
	chance = spell_chance(spell, use_realm - 1);

	/* Failed spell */
	if (rand_int(100) < chance)
	{
		if (flush_failure) flush();

#ifdef JP
msg_format("%s�򤦤ޤ��������ʤ��ä���", prayer);
#else
		msg_format("You failed to get the %s off!", prayer);
#endif

		sound(SOUND_FAIL);

		if (realm == REALM_LIFE)
		{
			if (randint(100) < chance)
				chg_virtue(V_FAITH, -1);
		}
		else if (realm == REALM_DEATH)
		{
			if (randint(100) < chance)
				chg_virtue(V_UNLIFE, -1);
		}
		else if (realm == REALM_NATURE)
		{
			if (randint(100) < chance)
				chg_virtue(V_NATURE, -1);
		}
		else if (realm == REALM_DAEMON)
		{
			if (randint(100) < chance)
				chg_virtue(V_JUSTICE, 1);
		}
		else if (randint(100) < chance)
		{
			chg_virtue(V_KNOWLEDGE, -1);
		}

		if (realm == REALM_TRUMP)
		{
			cast_trump_spell(spell, FALSE);
		}
		else if ((o_ptr->tval == TV_CHAOS_BOOK) && (randint(100) < spell))
		{
#ifdef JP
msg_print("������Ū�ʸ��̤�ȯ��������");
#else
			msg_print("You produce a chaotic effect!");
#endif

			wild_magic(spell);
		}
		else if ((o_ptr->tval == TV_DEATH_BOOK) && (randint(100) < spell))
		{
			if ((sval == 3) && (randint(2) == 1))
			{
				sanity_blast(0, TRUE);
			}
			else
			{
#ifdef JP
				msg_print("�ˤ���");
#else
				msg_print("It hurts!");
#endif

#ifdef JP
				take_hit(DAMAGE_LOSELIFE, damroll(o_ptr->sval + 1, 6), "�Ź���ˡ�ε�ή", -1);
#else
				take_hit(DAMAGE_LOSELIFE, damroll(o_ptr->sval + 1, 6), "a miscast Death spell", -1);
#endif

				if ((spell > 15) && (randint(6) == 1) && !p_ptr->hold_life)
					lose_exp(spell * 250);
			}
		}
		else if ((o_ptr->tval == TV_MUSIC_BOOK) && (randint(200) < spell))
		{
msg_print("����ʲ���������");

			aggravate_monsters(0);
		}
		if (randint(100) >= chance)
			chg_virtue(V_CHANCE,-1);
	}

	/* Process spell */
	else
	{
		/* Spells.  */
		switch (realm)
		{
		case REALM_LIFE: /* * LIFE * */
			cast = cast_life_spell(spell);
			break;
		case REALM_SORCERY: /* * SORCERY * */
			cast = cast_sorcery_spell(spell);
			break;
		case REALM_NATURE: /* * NATURE * */
			cast = cast_nature_spell(spell);
			break;
		case REALM_CHAOS: /* * CHAOS * */
			cast = cast_chaos_spell(spell);
			break;
		case REALM_DEATH: /* * DEATH * */
			cast = cast_death_spell(spell);
			break;
		case REALM_TRUMP: /* TRUMP */
			cast = cast_trump_spell(spell, TRUE);
			break;
		case REALM_ARCANE: /* ARCANE */
			cast = cast_arcane_spell(spell);
			break;
		case REALM_ENCHANT: /* ENCHANT */
			cast = cast_enchant_spell(spell);
			break;
		case REALM_DAEMON: /* DAEMON */
			cast = cast_daemon_spell(spell);
			break;
		case REALM_MUSIC: /* MUSIC */
			cast = cast_music_spell(spell);
			break;
		default:
			cast = FALSE;
			msg_format("You cast a spell from an unknown realm: realm %d, spell %d.", realm, spell);
			msg_print(NULL);
		}

		/* Canceled spells cost neither a turn nor mana */
		if (!cast) return;

		if (randint(100) < chance)
			chg_virtue(V_CHANCE,1);

		/* A spell was cast */
		if (!(increment ?
		    (spell_worked2 & (1L << spell)) :
		    (spell_worked1 & (1L << spell)))
		    && (p_ptr->pclass != CLASS_SORCERER)
		    && (p_ptr->pclass != CLASS_RED_MAGE))
		{
			int e = s_ptr->sexp;

			/* The spell worked */
			if (realm == p_ptr->realm1)
			{
				spell_worked1 |= (1L << spell);
			}
			else
			{
				spell_worked2 |= (1L << spell);
			}

			/* Gain experience */
			gain_exp(e * s_ptr->slevel);

			if (realm == REALM_LIFE)
			{
				chg_virtue(V_FAITH, 1);
				chg_virtue(V_COMPASSION, 1);
				chg_virtue(V_VITALITY, 1);
				chg_virtue(V_HONOUR, 1);
			}
			else if (realm == REALM_DEATH)
			{
				chg_virtue(V_UNLIFE, 1);
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_FAITH, -1);
				chg_virtue(V_VITALITY, -1);
			}
			else if (realm == REALM_DAEMON)
			{
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_FAITH, -1);
				chg_virtue(V_HONOUR, -1);
				chg_virtue(V_TEMPERANCE, -1);
			}
			else if (realm == REALM_NATURE)
			{
				chg_virtue(V_NATURE, 1);
				chg_virtue(V_HARMONY, 1);
			}
			else
				chg_virtue(V_KNOWLEDGE, 1);
		}
		if (realm == REALM_LIFE)
		{
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_FAITH, 1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_COMPASSION, 1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_VITALITY, 1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_HONOUR, 1);
		}
		else if (realm == REALM_DEATH)
		{
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_UNLIFE, 1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_JUSTICE, -1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_FAITH, -1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_VITALITY, -1);
		}
		else if (realm == REALM_DAEMON)
		{
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_JUSTICE, -1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_FAITH, -1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_HONOUR, -1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_TEMPERANCE, -1);
		}
		else if (realm == REALM_NATURE)
		{
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_NATURE, 1);
			if (randint(100 + p_ptr->lev) < shouhimana) chg_virtue(V_HARMONY, 1);
		}
		if (mp_ptr->spell_xtra & MAGIC_GAIN_EXP)
		{
			if (spell_exp[(increment ? 32 : 0)+spell] < 900)
				spell_exp[(increment ? 32 : 0)+spell]+=60;
			else if(spell_exp[(increment ? 32 : 0)+spell] < 1200)
				{if ((dun_level > 4) && ((dun_level + 10) > p_ptr->lev)) spell_exp[(increment ? 32 : 0)+spell]+=8;}
			else if(spell_exp[(increment ? 32 : 0)+spell] < 1400)
				{if (((dun_level + 5) > p_ptr->lev) && ((dun_level + 5) > s_ptr->slevel)) spell_exp[(increment ? 32 : 0)+spell]+=2;}
			else if((spell_exp[(increment ? 32 : 0)+spell] < 1600) && !increment)
				{if (((dun_level + 5) > p_ptr->lev) && (dun_level > s_ptr->slevel)) spell_exp[(increment ? 32 : 0)+spell]+=1;}
		}
	}

	/* Take a turn */
	energy_use = 100;

	/* Sufficient mana */
	if (shouhimana <= p_ptr->csp)
	{
		/* Use some mana */
		p_ptr->csp -= shouhimana;
	}

	/* Over-exert the player */
	else
	{
		int oops = shouhimana;

		/* No mana left */
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;

		/* Message */
#ifdef JP
msg_print("�������椷�����Ƶ��򼺤äƤ��ޤä���");
#else
		msg_print("You faint from the effort!");
#endif


		/* Hack -- Bypass free action */
		(void)set_paralyzed(p_ptr->paralyzed + randint(5 * oops + 1));

		if (realm == REALM_LIFE)
			chg_virtue(V_FAITH, -10);
		else if (realm == REALM_DEATH)
			chg_virtue(V_UNLIFE, -10);
		else if (realm == REALM_DAEMON)
			chg_virtue(V_JUSTICE, 10);
		else if (realm == REALM_NATURE)
			chg_virtue(V_NATURE, -10);
		else
			chg_virtue(V_KNOWLEDGE, -10);

		/* Damage CON (possibly permanently) */
		if (rand_int(100) < 50)
		{
			bool perm = (rand_int(100) < 25);

			/* Message */
#ifdef JP
msg_print("�Τ򰭤����Ƥ��ޤä���");
#else
			msg_print("You have damaged your health!");
#endif


			/* Reduce constitution */
			(void)dec_stat(A_CON, 15 + randint(10), perm);
		}
	}

	/* Redraw mana */
	p_ptr->redraw |= (PR_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);
	p_ptr->window |= (PW_SPELL);
#endif /* USE_SCRIPT */
}


/*
 * Pray a prayer -- Unused in Hengband
 */
void do_cmd_pray(void)
{
	msg_print("Praying is not used in Hengband. Use magic spell casting instead.");
}

static bool ang_sort_comp_pet_dismiss(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	int w1 = who[a];
	int w2 = who[b];

	monster_type *m_ptr1 = &m_list[w1];
	monster_type *m_ptr2 = &m_list[w2];
	monster_race *r_ptr1 = &r_info[m_ptr1->r_idx];
	monster_race *r_ptr2 = &r_info[m_ptr2->r_idx];

	if (w1 == p_ptr->riding) return TRUE;
	if (w2 == p_ptr->riding) return FALSE;

	if (m_ptr1->nickname && !m_ptr2->nickname) return TRUE;
	if (m_ptr2->nickname && !m_ptr1->nickname) return FALSE;

	if ((r_ptr1->flags1 & RF1_UNIQUE) && !(r_ptr2->flags1 & RF1_UNIQUE)) return TRUE;
	if ((r_ptr2->flags1 & RF1_UNIQUE) && !(r_ptr1->flags1 & RF1_UNIQUE)) return FALSE;

	if (r_ptr1->level > r_ptr2->level) return TRUE;
	if (r_ptr2->level > r_ptr1->level) return FALSE;

	if (m_ptr1->hp > m_ptr2->hp) return TRUE;
	if (m_ptr2->hp > m_ptr1->hp) return FALSE;
	
	return w1 <= w2;
}

int calculate_upkeep(void)
{
	s32b old_friend_align = friend_align;
	int m_idx;
	bool have_a_unique = FALSE;

	total_friends = 0;
	total_friend_levels = 0;
	friend_align = 0;

	for (m_idx = m_max - 1; m_idx >=1; m_idx--)
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		
		m_ptr = &m_list[m_idx];
		if (!m_ptr->r_idx) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		if (is_pet(m_ptr))
		{
			total_friends++;
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				if (p_ptr->pclass == CLASS_CAVALRY)
				{
					if (p_ptr->riding == m_idx)
						total_friend_levels += (r_ptr->level+5)*2;
					else if (!have_a_unique && (r_info[m_ptr->r_idx].flags7 & RF7_RIDING))
						total_friend_levels += (r_ptr->level+5)*7/2;
					else
						total_friend_levels += (r_ptr->level+5)*10;
					have_a_unique = TRUE;
				}
				else
					total_friend_levels += (r_ptr->level+5)*10;
			}
			else
				total_friend_levels += r_ptr->level;
			
			/* Determine pet alignment */
			if (r_ptr->flags3 & RF3_GOOD)
			{
				friend_align += r_ptr->level;
			}
			else if (r_ptr->flags3 & RF3_EVIL)
			{
				friend_align -= r_ptr->level;
			}
		}
	}
	if (old_friend_align != friend_align) p_ptr->update |= (PU_BONUS);
	if (total_friends)
	{
		int upkeep_factor;
		upkeep_factor = (total_friend_levels - (p_ptr->lev * 80 / (cp_ptr->pet_upkeep_div)));
		if (upkeep_factor < 0) upkeep_factor = 0;
		if (upkeep_factor > 1000) upkeep_factor = 1000;
		return upkeep_factor;
	}
	else
		return 0;
}

void do_cmd_pet_dismiss(void)
{
	monster_type	*m_ptr;
	bool		all_pets = FALSE;
	int pet_ctr, i;
	int Dismissed = 0;

	u16b *who;
	u16b dummy_why;
	int max_pet = 0;
	int cu, cv;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Allocate the "who" array */
	C_MAKE(who, max_m_idx, u16b);

	/* Process the monsters (backwards) */
	for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
	{
		if (is_pet(&m_list[pet_ctr]))
			who[max_pet++] = pet_ctr;
	}

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_pet_dismiss;
	ang_sort_swap = ang_sort_swap_hook;

	ang_sort(who, &dummy_why, max_pet);

	/* Process the monsters (backwards) */
	for (i = 0; i < max_pet; i++)
	{
		/* Access the monster */
		pet_ctr = who[i];
		m_ptr = &m_list[pet_ctr];

		{
			bool delete_this = FALSE;
			char friend_name[80];
			bool kakunin = ((pet_ctr == p_ptr->riding) || (m_ptr->nickname));
			monster_desc(friend_name, m_ptr, 0x80);

			if (all_pets && !kakunin)
				delete_this = TRUE;
			else
			{
				char ch;
				char check_friend[80];
				if (all_pets)
				{
#ifdef JP
sprintf(check_friend, "%s�������ޤ����� [Yes/No]", friend_name);
#else
 sprintf(check_friend, "Dismiss %s? [Yes/No]", friend_name);
#endif
				}
				else
				{
#ifdef JP
sprintf(check_friend, "%s�������ޤ����� [Yes/No/All(%dɤ)]", friend_name, max_pet-i);
#else
 sprintf(check_friend, "Dismiss %s? [Yes/No/All(%d remain)]", friend_name, max_pet-i);
#endif
				}
				/* Hack -- health bar for this monster */
				health_track(pet_ctr);

				/* Hack -- handle stuff */
				handle_stuff();

				prt(check_friend, 0, 0);

				if (m_ptr->ml)
					move_cursor_relative(m_ptr->fy, m_ptr->fx);
				while (TRUE)
				{
					ch = inkey();
					if (ch == ESCAPE) break;
					if (strchr("YyNnAa", ch)) break;
					bell();
				}

				if (ch == 'A' || ch == 'a') all_pets = TRUE;

				if (ch == 'Y' || ch == 'y' || ch == 'A' || ch == 'a')
				{
					if (kakunin)
					{
#ifdef JP
						sprintf(check_friend, "�����ˤ�����Ǥ����� (%s) ", friend_name);
#else
						sprintf(check_friend, "Really? (%s) ", friend_name);
#endif
						if (!get_check(check_friend)) continue;
					}
					if (record_named_pet && m_ptr->nickname)
					{
						char m_name[80];

						monster_desc(m_name, m_ptr, 0x08);
						do_cmd_write_nikki(NIKKI_NAMED_PET, 2, m_name);
					}
					delete_this = TRUE;
				}
			}

			if (delete_this)
			{
				if (pet_ctr == p_ptr->riding)
				{
#ifdef JP
msg_format("%s����ߤꤿ��", friend_name);
#else
					msg_format("You have got off %s. ", friend_name);
#endif

					p_ptr->riding = 0;

					/* Update the monsters */
					p_ptr->update |= (PU_BONUS | PU_MONSTERS);
					p_ptr->redraw |= (PR_EXTRA);
				}
				delete_monster_idx(pet_ctr);
				Dismissed++;
			}
		}
	}

	Term->scr->cu = cu;
	Term->scr->cv = cv;
	Term_fresh();

	C_KILL(who, max_m_idx, u16b);

#ifdef JP
msg_format("%d ɤ�ΥڥåȤ������ޤ�����", Dismissed);
#else
	msg_format("You have dismissed %d pet%s.", Dismissed,
		(Dismissed == 1 ? "" : "s"));
#endif

	p_ptr->update |= (PU_MON_LITE);
}

bool rakuba(int dam, bool force)
{
	int i, y, x, oy, ox;
	int sn = 0, sy = 0, sx = 0;
	char m_name[80];
	monster_type *m_ptr = &m_list[p_ptr->riding];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (!p_ptr->riding) return FALSE;
	if (p_ptr->wild_mode) return FALSE;

	if (dam >= 0 || force)
	{
		if (!force)
		{
			int level = r_ptr->level;
			if (p_ptr->riding_ryoute) level += 20;
			if ((dam/2 + r_ptr->level) > (skill_exp[GINOU_RIDING]/30+10))
			{
				if((skill_exp[GINOU_RIDING] < skill_exp_settei[p_ptr->pclass][GINOU_RIDING][1]) && skill_exp_settei[p_ptr->pclass][GINOU_RIDING][1] > 1000)
				{
					if (r_ptr->level*100 > (skill_exp[GINOU_RIDING] + 1500))
						skill_exp[GINOU_RIDING] += (1+(r_ptr->level - skill_exp[GINOU_RIDING]/100 - 15));
					else skill_exp[GINOU_RIDING]++;
				}
			}
			if (rand_int(dam/2 + level*2) < (skill_exp[GINOU_RIDING]/30+10))
			{
				if ((((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY)) && !p_ptr->riding_ryoute) || !one_in_(p_ptr->lev*(p_ptr->riding_ryoute ? 2 : 3)+30))
				{
					return FALSE;
				}
			}
		}
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			cave_type *c_ptr;

			/* Access the location */
			y = py + ddy_ddd[i];
			x = px + ddx_ddd[i];

			c_ptr = &cave[y][x];

			/* Skip non-empty grids */
			if (cave_perma_grid(c_ptr)) continue;
			if (!cave_empty_grid2(c_ptr)) continue;

			if (c_ptr->m_idx) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if (rand_int(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}
		if (!sn)
		{
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%s���鿶����Ȥ��줽���ˤʤäơ��ɤˤ֤Ĥ��ä���",m_name);
			take_hit(DAMAGE_NOESCAPE, r_ptr->level+3, "�ɤؤξ���", -1);
#else
			msg_format("You have nearly fallen from %s, but bumped into wall.",m_name);
			take_hit(DAMAGE_NOESCAPE, r_ptr->level+3, "bumping into wall", -1);
#endif
			return FALSE;
		}

		oy = py;
		ox = px;

		py = sy;
		px = sx;

		/* Redraw the old spot */
		lite_spot(oy, ox);

		/* Redraw the new spot */
		lite_spot(py, px);

		/* Check for new panel */
		verify_panel();
	}

	p_ptr->riding = 0;
	p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
	p_ptr->riding_ryoute = p_ptr->old_riding_ryoute = FALSE;

	calc_bonuses();

	p_ptr->update |= (PU_BONUS);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW);

	/* Update the monsters */
	p_ptr->update |= (PU_DISTANCE);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	p_ptr->redraw |= (PR_EXTRA);

	if (p_ptr->ffall && !force)
	{
		monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%s�����������������Ǥ��ޤ�������Ω��ľ�������Ϥ�����",m_name);
#else
		msg_format("You are thrown from %s, but make a good landing.",m_name);
#endif
		return FALSE;
	}
#ifdef JP
	take_hit(DAMAGE_NOESCAPE, r_ptr->level+3, "����", -1);
#else
	take_hit(DAMAGE_NOESCAPE, r_ptr->level+3, "Falling from riding", -1);
#endif
	p_ptr->redraw |= (PR_UHEALTH);

	return TRUE;
}

bool do_riding(bool force)
{
	int oy, ox, x, y, dir = 0;
	cave_type *c_ptr;
	monster_type *m_ptr;

	if (!get_rep_dir2(&dir)) return FALSE;
	y = py + ddy[dir];
	x = px + ddx[dir];
	c_ptr = &cave[y][x];

	if (p_ptr->riding)
	{
		/* Skip non-empty grids */
		if (!cave_empty_bold2(y, x) || c_ptr->m_idx)
		{
#ifdef JP
msg_print("������ˤϹߤ���ޤ���");
#else
			msg_print("You cannot go to that direction.");
#endif
			return FALSE;
		}
		p_ptr->riding = 0;
		p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
		p_ptr->riding_ryoute = p_ptr->old_riding_ryoute = FALSE;
	}
	else
	{
		if (p_ptr->confused)
		{
#ifdef JP
msg_print("���𤷤Ƥ��ƾ��ʤ���");
#else
			msg_print("You are too confused!");
#endif
			return FALSE;
		}
		if (!(c_ptr->m_idx))
		{
#ifdef JP
msg_print("���ξ��ˤϥ�󥹥����Ϥ��ޤ���");
#else
			msg_print("Here is no pet.");
#endif

			return FALSE;
		}

		m_ptr = &m_list[c_ptr->m_idx];

		if (!is_pet(m_ptr) && !force)
		{
#ifdef JP
msg_print("���Υ�󥹥����ϥڥåȤǤϤ���ޤ���");
#else
			msg_print("That monster is no a pet.");
#endif

			return FALSE;
		}
		if (!(r_info[m_ptr->r_idx].flags7 & RF7_RIDING))
		{
#ifdef JP
msg_print("���Υ�󥹥����ˤϾ��ʤ���������");
#else
			msg_print("This monster doesn't seem suitable for riding.");
#endif

			return FALSE;
		}
		if (!(p_ptr->pass_wall) && (c_ptr->feat >= FEAT_RUBBLE) && (c_ptr->feat <= FEAT_PERM_SOLID))
		{
#ifdef JP
msg_print("���Υ�󥹥������ɤ���ˤ��롣");
#else
			msg_print("This monster is in the wall.");
#endif

			return FALSE;
		}
		if ((cave[py][px].feat >= FEAT_PATTERN_START) && (cave[py][px].feat <= FEAT_PATTERN_XTRA2) && ((cave[y][x].feat < FEAT_PATTERN_START) || (cave[y][x].feat > FEAT_PATTERN_XTRA2)))
		{
#ifdef JP
msg_print("�ѥ�����ξ夫��Ͼ��ޤ���");
#else
			msg_print("You cannot ride from on Pattern.");
#endif

			return FALSE;
		}
		if (!m_ptr->ml)
		{
#ifdef JP
msg_print("���ξ��ˤϥ�󥹥����Ϥ��ޤ���");
#else
			msg_print("Here is no monster.");
#endif

			return FALSE;
		}
		if (r_info[m_ptr->r_idx].level > randint((skill_exp[GINOU_RIDING]/50 + p_ptr->lev/2 +20)))
		{
#ifdef JP
msg_print("���ޤ����ʤ��ä���");
#else
			msg_print("You failed to ride.");
#endif

			energy_use = 100;

			return FALSE;
		}
		if (m_ptr->csleep)
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);
			m_ptr->csleep = 0;
#ifdef JP
msg_format("%s�򵯤�������", m_name);
#else
			msg_format("You have waked %s up.", m_name);
#endif
		}

		p_ptr->riding = c_ptr->m_idx;
	}

	/* Save the old location */
	oy = py;
	ox = px;

	/* Move the player to the safe location */
	py = y;
	px = x;

	/* Redraw the old spot */
	lite_spot(oy, ox);

	/* Redraw the new spot */
	lite_spot(py, px);

	/* Check for new panel */
	verify_panel();

	energy_use = 100;

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW);

	/* Update the monsters */
	p_ptr->update |= (PU_DISTANCE);

	/* Update the monsters */
	p_ptr->update |= (PU_BONUS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP | PR_EXTRA);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	p_ptr->redraw |= (PR_UHEALTH);

	handle_stuff();
	return TRUE;
}

void do_name_pet(void)
{
	monster_type *m_ptr;
	char out_val[20];
	char m_name[80];
	bool old_name = FALSE;
	bool old_target_pet = target_pet;

	target_pet = TRUE;
	if (!target_set(TARGET_KILL))
	{
		target_pet = old_target_pet;
		return;
	}
	target_pet = old_target_pet;

	if (cave[target_row][target_col].m_idx)
	{
		m_ptr = &m_list[cave[target_row][target_col].m_idx];

		if (!is_pet(m_ptr))
		{
			/* Message */
#ifdef JP
			msg_print("���Υ�󥹥����ϥڥåȤǤϤʤ���");
#else
			msg_format("This monster is not a pet.");
#endif
			return;
		}
		if (r_info[m_ptr->r_idx].flags1 & RF1_UNIQUE)
		{
#ifdef JP
			msg_print("���Υ�󥹥�����̾�����Ѥ����ʤ���");
#else
			msg_format("You cannot change name of this monster!");
#endif
			return;
		}
		monster_desc(m_name, m_ptr, 0);

		/* Message */
#ifdef JP
		msg_format("%s��̾����Ĥ��롣", m_name);
#else
		msg_format("Name %s.", m_name);
#endif

		msg_print(NULL);

		/* Start with nothing */
		strcpy(out_val, "");

		/* Use old inscription */
		if (m_ptr->nickname)
		{
			/* Start with the old inscription */
			strcpy(out_val, quark_str(m_ptr->nickname));
			old_name = TRUE;
		}

		/* Get a new inscription (possibly empty) */
#ifdef JP
		if (get_string("̾��: ", out_val, 15))
#else
		if (get_string("Name: ", out_val, 15))
#endif

		{
			if (out_val[0])
			{
				/* Save the inscription */
				m_ptr->nickname = quark_add(out_val);
				if (record_named_pet)
				{
					char m_name[80];

					monster_desc(m_name, m_ptr, 0x08);
					do_cmd_write_nikki(NIKKI_NAMED_PET, 0, m_name);
				}
			}
			else
			{
				if (record_named_pet && old_name)
				{
					char m_name[80];

					monster_desc(m_name, m_ptr, 0x08);
					do_cmd_write_nikki(NIKKI_NAMED_PET, 1, m_name);
				}
				m_ptr->nickname = 0;
			}
		}
	}
}

/*
 * Issue a pet command
 */
void do_cmd_pet(void)
{
	int			i = 0;
	int			num;
	int			powers[36];
	cptr			power_desc[36];
	bool			flag, redraw;
	int			ask;
	char			choice;
	char			out_val[160];
	int			pets = 0, pet_ctr;
	monster_type	*m_ptr;

	int mode = 0;

	byte y = 1, x = 0;
	int ctr = 0;
	char buf[160];

	num = 0;

	/* Calculate pets */
	/* Process the monsters (backwards) */
	for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
	{
		/* Access the monster */
		m_ptr = &m_list[pet_ctr];

		if (is_pet(m_ptr)) pets++;
	}

#ifdef JP
	power_desc[num] = "�ڥåȤ�����";
#else
	power_desc[num] = "dismiss pets";
#endif

	powers[num++] = PET_DISMISS;

#ifdef JP
	sprintf(buf,"�ڥåȤΥ������åȤ���� (���ߡ�%s)",
		(pet_t_m_idx ? r_name + r_info[m_list[pet_t_m_idx].r_idx].name : "����ʤ�"));
#else
	sprintf(buf,"specify a targert of pet (now:%s)",
		(pet_t_m_idx ? r_name + r_info[m_list[pet_t_m_idx].r_idx].name : "nothing"));
#endif
	power_desc[num] = buf;

	powers[num++] = PET_TARGET;

#ifdef JP
power_desc[num] = "�᤯�ˤ���";
#else
	power_desc[num] = "stay close";
#endif

	if (p_ptr->pet_follow_distance == PET_CLOSE_DIST) mode = num;
	powers[num++] = PET_STAY_CLOSE;

#ifdef JP
	power_desc[num] = "�Ĥ����褤";
#else
	power_desc[num] = "follow me";
#endif

	if (p_ptr->pet_follow_distance == PET_FOLLOW_DIST) mode = num;
	powers[num++] = PET_FOLLOW_ME;

#ifdef JP
power_desc[num] = "Ũ�򸫤Ĥ����ݤ�";
#else
	power_desc[num] = "seek and destroy";
#endif

	if (p_ptr->pet_follow_distance == PET_DESTROY_DIST) mode = num;
	powers[num++] = PET_SEEK_AND_DESTROY;

#ifdef JP
power_desc[num] = "����Υ��Ƥ���";
#else
	power_desc[num] = "give me space";
#endif

	if (p_ptr->pet_follow_distance == PET_SPACE_DIST) mode = num;
	powers[num++] = PET_ALLOW_SPACE;

#ifdef JP
power_desc[num] = "Υ��Ƥ���";
#else
	power_desc[num] = "stay away";
#endif

	if (p_ptr->pet_follow_distance == PET_AWAY_DIST) mode = num;
	powers[num++] = PET_STAY_AWAY;

	if (p_ptr->pet_extra_flags & PF_OPEN_DOORS)
	{
#ifdef JP
		power_desc[num] = "�ɥ��򳫤��� (����:ON)";
#else
		power_desc[num] = "pets open doors (now On)";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "�ɥ��򳫤��� (����:OFF)";
#else
		power_desc[num] = "pets open doors (now Off)";
#endif

	}
	powers[num++] = PET_OPEN_DOORS;

	if (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
	{
#ifdef JP
		power_desc[num] = "�����ƥ�򽦤� (����:ON)";
#else
		power_desc[num] = "pets pick up items (now On)";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "�����ƥ�򽦤� (����:OFF)";
#else
		power_desc[num] = "pets pick up items (now Off)";
#endif

	}
	powers[num++] = PET_TAKE_ITEMS;

	if (p_ptr->pet_extra_flags & PF_TELEPORT)
	{
#ifdef JP
		power_desc[num] = "�ƥ�ݡ��ȷ���ˡ��Ȥ� (����:ON)";
#else
		power_desc[num] = "allow teleport (now On)";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "�ƥ�ݡ��ȷ���ˡ��Ȥ� (����:OFF)";
#else
		power_desc[num] = "allow teleport (now Off)";
#endif

	}
	powers[num++] = PET_TELEPORT;

	if (p_ptr->pet_extra_flags & PF_ATTACK_SPELL)
	{
#ifdef JP
		power_desc[num] = "������ˡ��Ȥ� (����:ON)";
#else
		power_desc[num] = "allow cast attack spell (now On)";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "������ˡ��Ȥ� (����:OFF)";
#else
		power_desc[num] = "allow cast attack spell (now Off)";
#endif

	}
	powers[num++] = PET_ATTACK_SPELL;

	if (p_ptr->pet_extra_flags & PF_SUMMON_SPELL)
	{
#ifdef JP
		power_desc[num] = "������ˡ��Ȥ� (����:ON)";
#else
		power_desc[num] = "allow cast summon spell (now On)";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "������ˡ��Ȥ� (����:OFF)";
#else
		power_desc[num] = "allow cast summon spell (now Off)";
#endif

	}
	powers[num++] = PET_SUMMON_SPELL;

	if (p_ptr->pet_extra_flags & PF_BALL_SPELL)
	{
#ifdef JP
		power_desc[num] = "�ץ쥤�䡼�򴬤������ϰ���ˡ��Ȥ� (����:ON)";
#else
		power_desc[num] = "allow involve player in area spell (now On)";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "�ץ쥤�䡼�򴬤������ϰ���ˡ��Ȥ� (����:OFF)";
#else
		power_desc[num] = "allow involve player in area spell (now Off)";
#endif

	}
	powers[num++] = PET_BALL_SPELL;

	if (p_ptr->riding)
	{
#ifdef JP
		power_desc[num] = "�ڥåȤ���ߤ��";
#else
		power_desc[num] = "get off a pet";
#endif

	}
	else
	{
#ifdef JP
		power_desc[num] = "�ڥåȤ˾��";
#else
		power_desc[num] = "ride a pet";
#endif

	}
	powers[num++] = PET_RIDING;

#ifdef JP
	power_desc[num] = "�ڥåȤ�̾����Ĥ��롣";
#else
	power_desc[num] = "name pets";
#endif

	powers[num++] = PET_NAME;

	if (p_ptr->riding && buki_motteruka(INVEN_RARM) && (empty_hands(FALSE) & 0x00000001) && ((inventory[INVEN_RARM].weight > 99) || (inventory[INVEN_RARM].tval == TV_POLEARM)))
	{
		if (p_ptr->pet_extra_flags & PF_RYOUTE)
		{
#ifdef JP
			power_desc[num] = "�����Ҽ�ǻ���";
#else
			power_desc[num] = "use one hand to control a riding pet";
#endif

		}
		else
		{
#ifdef JP
			power_desc[num] = "����ξ��ǻ���";
#else
			power_desc[num] = "use both hands for a weapon.";
#endif

		}

		powers[num++] = PET_RYOUTE;
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* Build a prompt (accept all spells) */
	if (num <= 26)
	{
		/* Build a prompt (accept all spells) */
#ifdef JP
strnfmt(out_val, 78, "(���ޥ�� %c-%c��'*'=������ESC=��λ) ���ޥ�ɤ�����Ǥ�������:",
#else
		strnfmt(out_val, 78, "(Command %c-%c, *=List, ESC=exit) Select a command: ",
#endif

			I2A(0), I2A(num - 1));
	}
	else
	{
#ifdef JP
strnfmt(out_val, 78, "(���ޥ�� %c-%c��'*'=������ESC=��λ) ���ޥ�ɤ�����Ǥ�������:",
#else
		strnfmt(out_val, 78, "(Command %c-%c, *=List, ESC=exit) Select a command: ",
#endif

			I2A(0), '0' + num - 27);
	}

	/* Show list */
	redraw = TRUE;

	/* Save the screen */
	Term_save();

	prt("", y++, x);

	while (ctr < num)
	{
		prt(format("%s%c) %s", (ctr == mode) ? "*" : " ", I2A(ctr), power_desc[ctr]), y + ctr, x);
		ctr++;
	}

	if (ctr < 17)
	{
		prt("", y + ctr, x);
	}
	else
	{
		prt("", y + 17, x);
	}

	/* Get a command from the user */
	while (!flag && get_com(out_val, &choice, TRUE))
	{
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?'))
		{
			/* Show the list */
			if (!redraw)
			{
				y = 1;
				x = 0;
				ctr = 0;

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				Term_save();

				prt("", y++, x);

				while (ctr < num)
				{
					sprintf(buf, "%s%c) %s", (ctr == mode) ? "*" : " ", I2A(ctr), power_desc[ctr]);
					prt(buf, y + ctr, x);
					ctr++;
				}

				if (ctr < 17)
				{
					prt("", y + ctr, x);
				}
				else
				{
					prt("", y + 17, x);
				}
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				Term_load();
			}

			/* Redo asking */
			continue;
		}

		if (choice == '\r' && num == 1)
		{
			choice = 'a';
		}

		if (isalpha(choice))
		{
			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}
		else
		{
			ask = FALSE; /* Can't uppercase digits */

			i = choice - '0' + 26;
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= num))
		{
			bell();
			continue;
		}

		/* Verify it */
		if (ask)
		{
			/* Prompt */
#ifdef JP
			strnfmt(buf, 78, "%s��Ȥ��ޤ����� ", power_desc[i]);
#else
			strnfmt(buf, 78, "Use %s? ", power_desc[i]);
#endif


			/* Belay that order */
			if (!get_check(buf)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) Term_load();

	/* Abort if needed */
	if (!flag)
	{
		energy_use = 0;
		return;
	}

	switch (powers[i])
	{
		case PET_DISMISS: /* Dismiss pets */
		{
			if (!pets)
			{
#ifdef JP
				msg_print("�ڥåȤ����ʤ���");
#else
				msg_print("You have no pets!");
#endif
				break;
			}
			do_cmd_pet_dismiss();
			(void)calculate_upkeep();
			break;
		}
		case PET_TARGET:
		{
			project_length = -1;
			if (!target_set(TARGET_KILL)) pet_t_m_idx = 0;
			else
			{
				cave_type *c_ptr = &cave[target_row][target_col];
				if (c_ptr->m_idx && (m_list[c_ptr->m_idx].ml))
				{
					pet_t_m_idx = cave[target_row][target_col].m_idx;
					p_ptr->pet_follow_distance = PET_DESTROY_DIST;
				}
				else pet_t_m_idx = 0;
			}
			project_length = 0;

			break;
		}
		/* Call pets */
		case PET_STAY_CLOSE:
		{
			p_ptr->pet_follow_distance = PET_CLOSE_DIST;
			pet_t_m_idx = 0;
			break;
		}
		/* "Follow Me" */
		case PET_FOLLOW_ME:
		{
			p_ptr->pet_follow_distance = PET_FOLLOW_DIST;
			pet_t_m_idx = 0;
			break;
		}
		/* "Seek and destoy" */
		case PET_SEEK_AND_DESTROY:
		{
			p_ptr->pet_follow_distance = PET_DESTROY_DIST;
			break;
		}
		/* "Give me space" */
		case PET_ALLOW_SPACE:
		{
			p_ptr->pet_follow_distance = PET_SPACE_DIST;
			break;
		}
		/* "Stay away" */
		case PET_STAY_AWAY:
		{
			p_ptr->pet_follow_distance = PET_AWAY_DIST;
			break;
		}
		/* flag - allow pets to open doors */
		case PET_OPEN_DOORS:
		{
			if (p_ptr->pet_extra_flags & PF_OPEN_DOORS) p_ptr->pet_extra_flags &= ~(PF_OPEN_DOORS);
			else p_ptr->pet_extra_flags |= (PF_OPEN_DOORS);
			break;
		}
		/* flag - allow pets to pickup items */
		case PET_TAKE_ITEMS:
		{
			if (p_ptr->pet_extra_flags & PF_PICKUP_ITEMS)
			{
				p_ptr->pet_extra_flags &= ~(PF_PICKUP_ITEMS);
				for (pet_ctr = m_max - 1; pet_ctr >= 1; pet_ctr--)
				{
					/* Access the monster */
					m_ptr = &m_list[pet_ctr];

					if (is_pet(m_ptr))
					{
						monster_drop_carried_objects(m_ptr);
					}
				}
			}
			else p_ptr->pet_extra_flags |= (PF_PICKUP_ITEMS);

			break;
		}
		/* flag - allow pets to teleport */
		case PET_TELEPORT:
		{
			if (p_ptr->pet_extra_flags & PF_TELEPORT) p_ptr->pet_extra_flags &= ~(PF_TELEPORT);
			else p_ptr->pet_extra_flags |= (PF_TELEPORT);
			break;
		}
		/* flag - allow pets to cast attack spell */
		case PET_ATTACK_SPELL:
		{
			if (p_ptr->pet_extra_flags & PF_ATTACK_SPELL) p_ptr->pet_extra_flags &= ~(PF_ATTACK_SPELL);
			else p_ptr->pet_extra_flags |= (PF_ATTACK_SPELL);
			break;
		}
		/* flag - allow pets to cast attack spell */
		case PET_SUMMON_SPELL:
		{
			if (p_ptr->pet_extra_flags & PF_SUMMON_SPELL) p_ptr->pet_extra_flags &= ~(PF_SUMMON_SPELL);
			else p_ptr->pet_extra_flags |= (PF_SUMMON_SPELL);
			break;
		}
		/* flag - allow pets to cast attack spell */
		case PET_BALL_SPELL:
		{
			if (p_ptr->pet_extra_flags & PF_BALL_SPELL) p_ptr->pet_extra_flags &= ~(PF_BALL_SPELL);
			else p_ptr->pet_extra_flags |= (PF_BALL_SPELL);
			break;
		}

		case PET_RIDING:
		{
			do_riding(FALSE);
			break;
		}

		case PET_NAME:
		{
			do_name_pet();
			break;
		}

		case PET_RYOUTE:
		{
			if (p_ptr->pet_extra_flags & PF_RYOUTE) p_ptr->pet_extra_flags &= ~(PF_RYOUTE);
			else p_ptr->pet_extra_flags |= (PF_RYOUTE);
			p_ptr->update |= (PU_BONUS);
			handle_stuff();
			break;
		}
	}
}
