
/* File: misc.c */

/* Purpose: misc code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"




/*
 * Converts stat num into a six-char (right justified) string
 */
void cnv_stat(int val, char *out_val)
{
	/* Above 18 */
	if (val > 18)
	{
		int bonus = (val - 18);

		if (bonus >= 220)
		{
			sprintf(out_val, "18/%3s", "***");
		}
		else if (bonus >= 100)
		{
			sprintf(out_val, "18/%03d", bonus);
		}
		else
		{
			sprintf(out_val, " 18/%02d", bonus);
		}
	}

	/* From 3 to 18 */
	else
	{
		sprintf(out_val, "    %2d", val);
	}
}



/*
 * Modify a stat value by a "modifier", return new value
 *
 * Stats go up: 3,4,...,17,18,18/10,18/20,...,18/220
 * Or even: 18/13, 18/23, 18/33, ..., 18/220
 *
 * Stats go down: 18/220, 18/210,..., 18/10, 18, 17, ..., 3
 * Or even: 18/13, 18/03, 18, 17, ..., 3
 */
s16b modify_stat_value(int value, int amount)
{
	int    i;

	/* Reward */
	if (amount > 0)
	{
		/* Apply each point */
		for (i = 0; i < amount; i++)
		{
			/* One point at a time */
			if (value < 18) value++;

			/* Ten "points" at a time */
			else value += 10;
		}
	}

	/* Penalty */
	else if (amount < 0)
	{
		/* Apply each point */
		for (i = 0; i < (0 - amount); i++)
		{
			/* Ten points at a time */
			if (value >= 18+10) value -= 10;

			/* Hack -- prevent weirdness */
			else if (value > 18) value = 18;

			/* One point at a time */
			else if (value > 3) value--;
		}
	}

	/* Return new value */
	return (value);
}



/*
 * Print character info at given row, column in a 13 char field
 */
static void prt_field(cptr info, int row, int col)
{
	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", row, col);

	/* Dump the info itself */
	c_put_str(TERM_L_BLUE, info, row, col);
}


/*
 * Print time
 */
void prt_time(void)
{
	s32b len = 20L * TOWN_DAWN;
	s32b tick = turn % len + len / 4;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DAY, COL_DAY);

	/* Dump the info itself */
#ifdef JP
	c_put_str(TERM_WHITE, format("%2d����",
#else
	c_put_str(TERM_WHITE, format("Day %-2d",
#endif
		  ((p_ptr->prace == RACE_VAMPIRE) ||
		   (p_ptr->prace == RACE_SKELETON) ||
		   (p_ptr->prace == RACE_ZOMBIE) ||
		   (p_ptr->prace == RACE_SPECTRE))
		   ? (turn - (15L * TOWN_DAWN)) / len + 1
		   : (turn + (5L * TOWN_DAWN))/ len + 1),
		  ROW_DAY, COL_DAY);

	c_put_str(TERM_WHITE, format("%2d:%02d",
				      (24 * tick / len) % 24,
				      (1440 * tick / len) % 60),
		  ROW_DAY, COL_DAY+7);
}




/*
 * Print dungeon
 */
static void prt_dungeon(void)
{
	cptr dungeon_name = d_name+d_info[dungeon_type].name;
	int col;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DUNGEON, COL_DUNGEON);

	if (p_ptr->inside_quest && (p_ptr->inside_quest < MIN_RANDOM_QUEST) && (quest[p_ptr->inside_quest].flags & QUEST_FLAG_PRESET))

#ifdef JP
	dungeon_name = "��������";
#else
	dungeon_name = "Quest";
#endif
	else if (p_ptr->wild_mode)
#ifdef JP
		dungeon_name = "�Ͼ�";
#else
		dungeon_name = "Surface";
#endif
	else if (p_ptr->inside_arena)
#ifdef JP
		dungeon_name = "���꡼��";
#else
		dungeon_name = "Monster Arena";
#endif
	else if (p_ptr->inside_battle)
#ifdef JP
		dungeon_name = "Ʈ����";
#else
		dungeon_name = "Arena";
#endif
	else if (!dun_level && p_ptr->town_num)
		dungeon_name = town[p_ptr->town_num].name;
	col = COL_DUNGEON + 6 - strlen(dungeon_name)/2;
	if (col < 0) col = 0;

	/* Dump the info itself */
	c_put_str(TERM_L_UMBER, format("%s",dungeon_name),
		  ROW_DUNGEON, col);
}




/*
 * Print character stat in given row, column
 */
static void prt_stat(int stat)
{
	char tmp[32];

	/* Display "injured" stat */
	if (p_ptr->stat_cur[stat] < p_ptr->stat_max[stat])
	{
		put_str(stat_names_reduced[stat], ROW_STAT + stat, 0);
		cnv_stat(p_ptr->stat_use[stat], tmp);
		c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Display "healthy" stat */
	else
	{
		put_str(stat_names[stat], ROW_STAT + stat, 0);
		cnv_stat(p_ptr->stat_use[stat], tmp);
		c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Indicate natural maximum */
	if (p_ptr->stat_max[stat] == p_ptr->stat_max_max[stat])
	{
#ifdef JP
		/* ���ܸ�ˤ��֤�ʤ��褦��ɽ�����֤��ѹ� */
		put_str("!", ROW_STAT + stat, 5);
#else
		put_str("!", ROW_STAT + stat, 3);
#endif

	}
}


/* Show status bar */

static void prt_status(void)
{
	/* Tsuyoshi  */
	if (p_ptr->tsuyoshi)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "��", ROW_STATBAR, COL_STATBAR);
#else
		c_put_str(TERM_YELLOW, "Ts", ROW_STATBAR, COL_STATBAR);
#endif
	}

	/* Hallucinating */
	else if (p_ptr->image)
	{
#ifdef JP
		c_put_str(TERM_VIOLET, "��", ROW_STATBAR, COL_STATBAR);
#else
		c_put_str(TERM_VIOLET, "Hu", ROW_STATBAR, COL_STATBAR);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
#ifdef JP
		c_put_str(TERM_L_DARK, "��", ROW_STATBAR, COL_STATBAR+2);
#else
		c_put_str(TERM_L_DARK, "Bl", ROW_STATBAR, COL_STATBAR+2);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+2);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
#ifdef JP
		c_put_str(TERM_RED, "��", ROW_STATBAR, COL_STATBAR+4);
#else
		c_put_str(TERM_RED, "Pa", ROW_STATBAR, COL_STATBAR+4);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+4);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
#ifdef JP
		c_put_str(TERM_VIOLET, "��", ROW_STATBAR, COL_STATBAR+6);
#else
		c_put_str(TERM_VIOLET, "Cf", ROW_STATBAR, COL_STATBAR+6);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+6);
	}

	/* Afraid */
	if (p_ptr->poisoned)
	{
#ifdef JP
		c_put_str(TERM_GREEN, "��", ROW_STATBAR, COL_STATBAR+8);
#else
		c_put_str(TERM_GREEN, "Po", ROW_STATBAR, COL_STATBAR+8);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+8);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "��", ROW_STATBAR, COL_STATBAR+10);
#else
		c_put_str(TERM_L_BLUE, "Se", ROW_STATBAR, COL_STATBAR+10);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+10);
	}

	/* Timed esp */
	if (p_ptr->tim_esp || music_singing(MUSIC_MIND))
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "��", ROW_STATBAR, COL_STATBAR+12);
#else
		c_put_str(TERM_ORANGE, "Te", ROW_STATBAR, COL_STATBAR+12);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+12);
	}

	/* Timed regenerate */
	if (p_ptr->tim_regen)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "��", ROW_STATBAR, COL_STATBAR+14);
#else
		c_put_str(TERM_L_BLUE, "Rg", ROW_STATBAR, COL_STATBAR+14);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+14);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "��", ROW_STATBAR, COL_STATBAR+16);
#else
		c_put_str(TERM_L_RED, "If", ROW_STATBAR, COL_STATBAR+16);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+16);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
#ifdef JP
		c_put_str(TERM_SLATE, "��", ROW_STATBAR, COL_STATBAR+18);
#else
		c_put_str(TERM_SLATE, "Ev", ROW_STATBAR, COL_STATBAR+18);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+18);
	}

	/* Invulnerability */
	if (p_ptr->invuln || music_singing(MUSIC_INVULN))
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "̵", ROW_STATBAR, COL_STATBAR+20);
#else
		c_put_str(TERM_YELLOW, "Iv", ROW_STATBAR, COL_STATBAR+20);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+20);
	}

	/* Wraith form */
	if (p_ptr->wraith_form)
	{
#ifdef JP
		c_put_str(TERM_L_DARK, "ͩ", ROW_STATBAR, COL_STATBAR+22);
#else
		c_put_str(TERM_L_DARK, "Gh", ROW_STATBAR, COL_STATBAR+22);
#endif
	}
	/* Kabenuke */
	else if (p_ptr->kabenuke)
	{
#ifdef JP
		c_put_str(TERM_SLATE, "��", ROW_STATBAR, COL_STATBAR+22);
#else
		c_put_str(TERM_SLATE, "Wp", ROW_STATBAR, COL_STATBAR+22);
#endif
	}
	else if (p_ptr->tim_reflect)
	{
#ifdef JP
		c_put_str(TERM_SLATE, "ȿ", ROW_STATBAR, COL_STATBAR+22);
#else
		c_put_str(TERM_SLATE, "Rf", ROW_STATBAR, COL_STATBAR+22);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+22);
	}

	/* Heroism */
	if (p_ptr->hero || music_singing(MUSIC_HERO) || music_singing(MUSIC_SHERO))
	{
#ifdef JP
		c_put_str(TERM_WHITE, "ͦ", ROW_STATBAR, COL_STATBAR+24);
#else
		c_put_str(TERM_WHITE, "He", ROW_STATBAR, COL_STATBAR+24);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+24);
	}

	/* Super Heroism / berserk */
	if (p_ptr->shero)
	{
#ifdef JP
		c_put_str(TERM_RED, "��", ROW_STATBAR, COL_STATBAR+26);
#else
		c_put_str(TERM_RED, "Br", ROW_STATBAR, COL_STATBAR+26);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+26);
	}

	/* Blessed */
	if (p_ptr->blessed || music_singing(MUSIC_BLESS))
	{
#ifdef JP
		c_put_str(TERM_WHITE, "��", ROW_STATBAR, COL_STATBAR+28);
#else
		c_put_str(TERM_WHITE, "Bs", ROW_STATBAR, COL_STATBAR+28);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+28);
	}

	/* Shield */
	if (p_ptr->magicdef)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "��", ROW_STATBAR, COL_STATBAR+30);
#else
		c_put_str(TERM_YELLOW, "Md", ROW_STATBAR, COL_STATBAR+30);
#endif
	}
	else if (p_ptr->tsubureru)
	{
#ifdef JP
		c_put_str(TERM_L_UMBER, "��", ROW_STATBAR, COL_STATBAR+30);
#else
		c_put_str(TERM_L_UMBER, "Eh", ROW_STATBAR, COL_STATBAR+30);
#endif
	}
	else if (p_ptr->shield)
	{
#ifdef JP
		c_put_str(TERM_WHITE, "��", ROW_STATBAR, COL_STATBAR+30);
#else
		c_put_str(TERM_WHITE, "Ss", ROW_STATBAR, COL_STATBAR+30);
#endif
	}
	else if (p_ptr->special_defense & NINJA_KAWARIMI)
	{
#ifdef JP
		c_put_str(TERM_VIOLET, "��", ROW_STATBAR, COL_STATBAR+30);
#else
		c_put_str(TERM_VIOLET, "Qa", ROW_STATBAR, COL_STATBAR+30);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+30);
	}

	/* Oppose Acid */
	if (p_ptr->special_defense & DEFENSE_ACID)
	{
#ifdef JP
		c_put_str(TERM_L_GREEN, "��", ROW_STATBAR, COL_STATBAR+32);
#else
		c_put_str(TERM_L_GREEN, "Ac", ROW_STATBAR, COL_STATBAR+32);
#endif
	}
	else if (p_ptr->oppose_acid || music_singing(MUSIC_RESIST) || (p_ptr->special_defense & KATA_MUSOU))
	{
#ifdef JP
		c_put_str(TERM_GREEN, "��", ROW_STATBAR, COL_STATBAR+32);
#else
		c_put_str(TERM_GREEN, "Ac", ROW_STATBAR, COL_STATBAR+32);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+32);
	}

	/* Oppose Lightning */
	if (p_ptr->special_defense & DEFENSE_ELEC)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "��", ROW_STATBAR, COL_STATBAR+34);
#else
		c_put_str(TERM_L_BLUE, "El", ROW_STATBAR, COL_STATBAR+34);
#endif
	}
	else if (p_ptr->oppose_elec || music_singing(MUSIC_RESIST) || (p_ptr->special_defense & KATA_MUSOU))
	{
#ifdef JP
		c_put_str(TERM_BLUE, "��", ROW_STATBAR, COL_STATBAR+34);
#else
		c_put_str(TERM_BLUE, "El", ROW_STATBAR, COL_STATBAR+34);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+34);
	}

	/* Oppose Fire */
	if (p_ptr->special_defense & DEFENSE_FIRE)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "��", ROW_STATBAR, COL_STATBAR+36);
#else
		c_put_str(TERM_L_RED, "Fi", ROW_STATBAR, COL_STATBAR+36);
#endif
	}
	else if (p_ptr->oppose_fire || music_singing(MUSIC_RESIST) || (p_ptr->special_defense & KATA_MUSOU))
	{
#ifdef JP
		c_put_str(TERM_RED, "��", ROW_STATBAR, COL_STATBAR+36);
#else
		c_put_str(TERM_RED, "Fi", ROW_STATBAR, COL_STATBAR+36);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+36);
	}

	/* Oppose Cold */
	if (p_ptr->special_defense & DEFENSE_COLD)
	{
#ifdef JP
		c_put_str(TERM_WHITE, "��", ROW_STATBAR, COL_STATBAR+38);
#else
		c_put_str(TERM_WHITE, "Co", ROW_STATBAR, COL_STATBAR+38);
#endif
	}
	else if (p_ptr->oppose_cold || music_singing(MUSIC_RESIST) || (p_ptr->special_defense & KATA_MUSOU))
	{
#ifdef JP
		c_put_str(TERM_SLATE, "��", ROW_STATBAR, COL_STATBAR+38);
#else
		c_put_str(TERM_SLATE, "Co", ROW_STATBAR, COL_STATBAR+38);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+38);
	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois || music_singing(MUSIC_RESIST) || (p_ptr->special_defense & KATA_MUSOU))
	{
#ifdef JP
		c_put_str(TERM_GREEN, "��", ROW_STATBAR, COL_STATBAR+40);
#else
		c_put_str(TERM_GREEN, "Po", ROW_STATBAR, COL_STATBAR+40);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+40);
	}

	/* Word of Recall */
	if (p_ptr->word_recall)
	{
#ifdef JP
		c_put_str(TERM_WHITE, "��", ROW_STATBAR, COL_STATBAR+42);
#else
		c_put_str(TERM_WHITE, "Wr", ROW_STATBAR, COL_STATBAR+42);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+42);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
#ifdef JP
		c_put_str(TERM_BLUE, "��", ROW_STATBAR, COL_STATBAR+44);
#else
		c_put_str(TERM_BLUE, "Fe", ROW_STATBAR, COL_STATBAR+44);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+44);
	}

	/* Blindness */
	if (p_ptr->tim_res_time)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "��", ROW_STATBAR, COL_STATBAR+46);
#else
		c_put_str(TERM_L_BLUE, "Ti", ROW_STATBAR, COL_STATBAR+46);
#endif
	}
	else if (p_ptr->multishadow)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "ʬ", ROW_STATBAR, COL_STATBAR+46);
#else
		c_put_str(TERM_L_BLUE, "Ms", ROW_STATBAR, COL_STATBAR+46);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+46);
	}

	/* Confusing Hands */
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
#ifdef JP
		c_put_str(TERM_RED, "��", ROW_STATBAR, COL_STATBAR+48);
#else
		c_put_str(TERM_RED, "Cf", ROW_STATBAR, COL_STATBAR+48);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+48);
	}

	if (p_ptr->resist_magic)
	{
#ifdef JP
		c_put_str(TERM_SLATE, "��", ROW_STATBAR, COL_STATBAR+50);
#else
		c_put_str(TERM_SLATE, "Rm", ROW_STATBAR, COL_STATBAR+50);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+50);
	}

	/* Ultimate-resistance */
	if (p_ptr->ult_res)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "��", ROW_STATBAR, COL_STATBAR+52);
#else
		c_put_str(TERM_YELLOW, "Ul", ROW_STATBAR, COL_STATBAR+52);
#endif
	}
	/* tim levitation */
	else if (p_ptr->tim_ffall)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "��", ROW_STATBAR, COL_STATBAR+52);
#else
		c_put_str(TERM_L_BLUE, "Lv", ROW_STATBAR, COL_STATBAR+52);
#endif
	}
	else if (p_ptr->tim_res_nether)
	{
#ifdef JP
		c_put_str(TERM_L_DARK, "��", ROW_STATBAR, COL_STATBAR+52);
#else
		c_put_str(TERM_L_DARK, "Nt", ROW_STATBAR, COL_STATBAR+52);
#endif
	}
	else if (p_ptr->dustrobe)
	{
#ifdef JP
		c_put_str(TERM_L_DARK, "��", ROW_STATBAR, COL_STATBAR+52);
#else
		c_put_str(TERM_L_DARK, "Am", ROW_STATBAR, COL_STATBAR+52);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+52);
	}

	/* Mahouken */
	if (p_ptr->special_attack & ATTACK_FIRE)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "��", ROW_STATBAR, COL_STATBAR+54);
#else
		c_put_str(TERM_L_RED, "Fi", ROW_STATBAR, COL_STATBAR+54);
#endif
	}
	else if (p_ptr->special_attack & ATTACK_COLD)
	{
#ifdef JP
		c_put_str(TERM_WHITE, "��", ROW_STATBAR, COL_STATBAR+54);
#else
		c_put_str(TERM_WHITE, "Co", ROW_STATBAR, COL_STATBAR+54);
#endif
	}
	else if (p_ptr->special_attack & ATTACK_ELEC)
	{
#ifdef JP
		c_put_str(TERM_L_BLUE, "��", ROW_STATBAR, COL_STATBAR+54);
#else
		c_put_str(TERM_L_BLUE, "El", ROW_STATBAR, COL_STATBAR+54);
#endif
	}
	else if (p_ptr->special_attack & ATTACK_ACID)
	{
#ifdef JP
		c_put_str(TERM_SLATE, "��", ROW_STATBAR, COL_STATBAR+54);
#else
		c_put_str(TERM_SLATE, "Ac", ROW_STATBAR, COL_STATBAR+54);
#endif
	}
	else if (p_ptr->special_attack & ATTACK_POIS)
	{
#ifdef JP
		c_put_str(TERM_L_GREEN, "��", ROW_STATBAR, COL_STATBAR+54);
#else
		c_put_str(TERM_L_GREEN, "Po", ROW_STATBAR, COL_STATBAR+54);
#endif
	}
	else if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "Ķ", ROW_STATBAR, COL_STATBAR+54);
#else
		c_put_str(TERM_YELLOW, "St", ROW_STATBAR, COL_STATBAR+54);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+54);
	}

	/* tim stealth */
	if (p_ptr->tim_sh_fire)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "��", ROW_STATBAR, COL_STATBAR+56);
#else
		c_put_str(TERM_L_RED, "Sf", ROW_STATBAR, COL_STATBAR+56);
#endif
	}
	else if (p_ptr->tim_stealth || music_singing(MUSIC_STEALTH))
	{
#ifdef JP
		c_put_str(TERM_UMBER, "��", ROW_STATBAR, COL_STATBAR+56);
#else
		c_put_str(TERM_UMBER, "Sl", ROW_STATBAR, COL_STATBAR+56);
#endif
	}
	else if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "��", ROW_STATBAR, COL_STATBAR+56);
#else
		c_put_str(TERM_YELLOW, "lt", ROW_STATBAR, COL_STATBAR+56);
#endif
	}
	else if (p_ptr->tim_sh_touki)
	{
#ifdef JP
		c_put_str(TERM_WHITE, "Ʈ", ROW_STATBAR, COL_STATBAR+56);
#else
		c_put_str(TERM_WHITE, "Ae", ROW_STATBAR, COL_STATBAR+56);
#endif
	}
	else
	{
		put_str("  ", ROW_STATBAR, COL_STATBAR+56);
	}
}



/*
 * Prints "title", including "wizard" or "winner" as needed.
 */
static void prt_title(void)
{
	cptr p = "";

	/* Wizard */
	if (wizard)
	{
#ifdef JP
                /* �����ڤ��ؤ���ǽ �ι� */
		p = "[����������]";
#else
		p = "[=-WIZARD-=]";
#endif

	}

	/* Winner */
	else if (total_winner || (p_ptr->lev > PY_MAX_LEVEL))
	{
		if ((p_ptr->arena_number > MAX_ARENA_MONS+2) && (p_ptr->arena_number < 99))
		{
#ifdef JP
			/* �����ڤ��ؤ���ǽ �ι� */
			p = "*����������*";

#else
			p = "*TRUEWINNER*";
#endif
		}
		else
		{
#ifdef JP
	                /* �����ڤ��ؤ���ǽ �ι� */
			p = "***������***";

#else
			p = "***WINNER***";
#endif
		}
	}

	/* Normal */
	else
	{
		p = player_title[p_ptr->pclass][(p_ptr->lev - 1) / 5];


	}

	prt_field(p, ROW_TITLE, COL_TITLE);
}


/*
 * Prints level
 */
static void prt_level(void)
{
	char tmp[32];

#ifdef JP
	sprintf(tmp, "%5d", p_ptr->lev);
#else
	sprintf(tmp, "%6d", p_ptr->lev);
#endif


	if (p_ptr->lev >= p_ptr->max_plv)
	{
#ifdef JP
		put_str("��٥� ", ROW_LEVEL, 0);
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 7);
#else
		put_str("LEVEL ", ROW_LEVEL, 0);
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 6);
#endif

	}
	else
	{
#ifdef JP
		put_str("x��٥�", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 7);
#else
		put_str("Level ", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 6);
#endif

	}
}


/*
 * Display the experience
 */
static void prt_exp(void)
{
	char out_val[32];

	if ((p_ptr->prace == RACE_ANDROID) && !cheat_xtra)
		(void)strcpy(out_val, "*******");
	else
#ifdef JP
(void)sprintf(out_val, "%7ld", (long)p_ptr->exp);
#else
	(void)sprintf(out_val, "%8ld", (long)p_ptr->exp);
#endif


	if (p_ptr->exp >= p_ptr->max_exp)
	{
#ifdef JP
		put_str("�и� ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 5);
#else
		put_str("EXP ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 4);
#endif

	}
	else
	{
#ifdef JP
		put_str("x�и�", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 5);
#else
		put_str("Exp ", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 4);
#endif

	}
}


/*
 * Prints current gold
 */
static void prt_gold(void)
{
	char tmp[32];

#ifdef JP
	put_str("�� ", ROW_GOLD, COL_GOLD);
#else
	put_str("AU ", ROW_GOLD, COL_GOLD);
#endif

	sprintf(tmp, "%9ld", (long)p_ptr->au);
	c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_GOLD + 3);
}



/*
 * Prints current AC
 */
static void prt_ac(void)
{
	char tmp[32];

#ifdef JP
/* AC ��ɽ���������ѹ����Ƥ��� */
	put_str(" ����(     )", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 6);
#else
	put_str("Cur AC ", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 7);
#endif

}


/*
 * Prints Cur/Max hit points
 */
static void prt_hp(void)
{
/* �ҥåȥݥ���Ȥ�ɽ����ˡ���ѹ� */
	char tmp[32];
  
	byte color;
  
	/* �����ȥ� */
/*	put_str(" �ȣС��ͣ�", ROW_HPMP, COL_HPMP); */

	put_str("HP", ROW_CURHP, COL_CURHP);

	/* ���ߤΥҥåȥݥ���� */
	sprintf(tmp, "%4d", p_ptr->chp);

	if (p_ptr->chp >= p_ptr->mhp)
	{
		color = TERM_L_GREEN;
	}
	else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP+3);

	/* ���ڤ� */
	put_str( "/", ROW_CURHP, COL_CURHP + 7 );

	/* ����ҥåȥݥ���� */
	sprintf(tmp, "%4d", p_ptr->mhp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 8 );
}


/*
 * Prints players max/cur spell points
 */
static void prt_sp(void)
{
/* �ޥ��å��ݥ���Ȥ�ɽ����ˡ���ѹ����Ƥ��� */
	char tmp[32];
	byte color;


	/* Do not show mana unless it matters */
	if (!mp_ptr->spell_book) return;

	/* �����ȥ� */
/*	put_str(" �ͣ� / ����", ROW_MAXSP, COL_MAXSP); */

#ifdef JP
	put_str("MP", ROW_CURSP, COL_CURSP);
#else
	put_str("SP", ROW_CURSP, COL_CURSP);
#endif

	/* ���ߤΥޥ��å��ݥ���� */
	sprintf(tmp, "%4d", p_ptr->csp);

	if (p_ptr->csp >= p_ptr->msp)
	{
		color = TERM_L_GREEN;
	}
	else if (p_ptr->csp > (p_ptr->msp * hitpoint_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP+3);

	/* ���ڤ� */
	put_str( "/", ROW_CURSP, COL_CURSP + 7 );

	/* ����ޥ��å��ݥ���� */
	sprintf(tmp, "%4d", p_ptr->msp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 8);
}


/*
 * Prints depth in stat area
 */
static void prt_depth(void)
{
	char depths[32];

	if (!dun_level)
	{
#ifdef JP
		strcpy(depths, "�Ͼ�");
#else
		strcpy(depths, "Surf.");
#endif
	}
	else if (p_ptr->inside_quest && !dungeon_type)
	{
#ifdef JP
strcpy(depths, "�Ͼ�");
#else
		strcpy(depths, "Quest");
#endif

	}
	else if (depth_in_feet)
	{
#ifdef JP
(void)sprintf(depths, "%d ft", dun_level * 50);
#else
		(void)sprintf(depths, "%d ft", dun_level * 50);
#endif

	}
	else
	{
#ifdef JP
sprintf(depths, "%d ��", dun_level);
#else
		(void)sprintf(depths, "Lev %d", dun_level);
#endif

	}

	/* Right-Adjust the "depth", and clear old values */
	prt(format("%7s", depths), ROW_DEPTH, COL_DEPTH);
}


/*
 * Prints status of hunger
 */
static void prt_hunger(void)
{
	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
#ifdef JP
		c_put_str(TERM_RED, "���  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_RED, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "���  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_ORANGE, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "��ʢ  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_YELLOW, "Hungry", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Normal */
	else if (p_ptr->food < PY_FOOD_FULL)
	{
		c_put_str(TERM_L_GREEN, "      ", ROW_HUNGRY, COL_HUNGRY);
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
#ifdef JP
		c_put_str(TERM_L_GREEN, "��ʢ  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_L_GREEN, "Full  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Gorged */
	else
	{
#ifdef JP
		c_put_str(TERM_GREEN, "���᤮", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_GREEN, "Gorged", ROW_HUNGRY, COL_HUNGRY);
#endif

	}
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static void prt_state(void)
{
	byte attr = TERM_WHITE;

	char text[5];

	/* Repeating */
	if (command_rep)
	{
		if (command_rep > 999)
		{
#ifdef JP
sprintf(text, "%2d00", command_rep / 100);
#else
			(void)sprintf(text, "%2d00", command_rep / 100);
#endif

		}
		else
		{
#ifdef JP
sprintf(text, "  %2d", command_rep);
#else
			(void)sprintf(text, "  %2d", command_rep);
#endif

		}
	}

	/* Action */
	else
	{
		switch(p_ptr->action)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				strcpy(text, "õ��");
#else
				strcpy(text, "Sear");
#endif
				break;
			}
			case ACTION_REST:
			{
				int i;

				/* Start with "Rest" */
#ifdef JP
				strcpy(text, "    ");
#else
				strcpy(text, "    ");
#endif


				/* Extensive (timed) rest */
				if (resting >= 1000)
				{
					i = resting / 100;
					text[3] = '0';
					text[2] = '0';
					text[1] = '0' + (i % 10);
					text[0] = '0' + (i / 10);
				}

				/* Long (timed) rest */
				else if (resting >= 100)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					i = i / 10;
					text[2] = '0' + (i % 10);
					text[1] = '0' + (i / 10);
				}

				/* Medium (timed) rest */
				else if (resting >= 10)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					text[2] = '0' + (i / 10);
				}

				/* Short (timed) rest */
				else if (resting > 0)
				{
					i = resting;
					text[3] = '0' + (i);
				}

				/* Rest until healed */
				else if (resting == -1)
				{
					text[0] = text[1] = text[2] = text[3] = '*';
				}

				/* Rest until done */
				else if (resting == -2)
				{
					text[0] = text[1] = text[2] = text[3] = '&';
				}
				break;
			}
			case ACTION_LEARN:
			{
#ifdef JP
				strcpy(text, "�ؽ�");
#else
				strcpy(text, "lear");
#endif
				if (new_mane) attr = TERM_L_RED;
				break;
			}
			case ACTION_FISH:
			{
#ifdef JP
				strcpy(text, "���");
#else
				strcpy(text, "fish");
#endif
				break;
			}
			case ACTION_KAMAE:
			{
				int i;
				for (i = 0; i < MAX_KAMAE; i++)
					if (p_ptr->special_defense & (KAMAE_GENBU << i)) break;
				switch (i)
				{
					case 0: attr = TERM_GREEN;break;
					case 1: attr = TERM_WHITE;break;
					case 2: attr = TERM_L_BLUE;break;
					case 3: attr = TERM_L_RED;break;
				}
				strcpy(text, kamae_shurui[i].desc);
				break;
			}
			case ACTION_KATA:
			{
				int i;
				for (i = 0; i < MAX_KATA; i++)
					if (p_ptr->special_defense & (KATA_IAI << i)) break;
				strcpy(text, kata_shurui[i].desc);
				break;
			}
			case ACTION_SING:
			{
#ifdef JP
				strcpy(text, "��  ");
#else
				strcpy(text, "Sing");
#endif
				break;
			}
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				strcpy(text, "®��");
#else
				strcpy(text, "Fast");
#endif
				break;
			}
			default:
			{
				strcpy(text, "    ");
				break;
			}
		}
	}

	/* Display the info (or blanks) */
	c_put_str(attr, format("%5.5s",text), ROW_STATE, COL_STATE);
}


/*
 * Prints the speed of a character.			-CJS-
 */
static void prt_speed(void)
{
	int i = p_ptr->pspeed;
	bool is_fast = (p_ptr->fast || music_singing(MUSIC_SPEED) || music_singing(MUSIC_SHERO));

	byte attr = TERM_WHITE;
	char buf[32] = "";

	/* Hack -- Visually "undo" the Search Mode Slowdown */
	if (p_ptr->action == ACTION_SEARCH) i += 10;

	/* Fast */
	if (i > 110)
	{
		if (p_ptr->riding)
		{
			if (m_list[p_ptr->riding].fast && !m_list[p_ptr->riding].slow) attr = TERM_L_BLUE;
			else if (m_list[p_ptr->riding].slow && !m_list[p_ptr->riding].fast) attr = TERM_VIOLET;
			else attr = TERM_GREEN;
		}
		else if ((is_fast && !p_ptr->slow) || p_ptr->lightspeed) attr = TERM_YELLOW;
		else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_GREEN;
#ifdef JP
		sprintf(buf, "%s(+%d)", (p_ptr->riding ? "����" : "��®"), (i - 110));
#else
		sprintf(buf, "Fast(+%d)", (i - 110));
#endif

	}

	/* Slow */
	else if (i < 110)
	{
		if (p_ptr->riding)
		{
			if (m_list[p_ptr->riding].fast && !m_list[p_ptr->riding].slow) attr = TERM_L_BLUE;
			else if (m_list[p_ptr->riding].slow && !m_list[p_ptr->riding].fast) attr = TERM_VIOLET;
			else attr = TERM_RED;
		}
		else if (is_fast && !p_ptr->slow) attr = TERM_YELLOW;
		else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_UMBER;
#ifdef JP
		sprintf(buf, "%s(-%d)", (p_ptr->riding ? "����" : "��®"), (110 - i));
#else
		sprintf(buf, "Slow(-%d)", (110 - i));
#endif
	}
	else if (p_ptr->riding)
	{
		attr = TERM_GREEN;
#ifdef JP
		strcpy(buf, "������");
#else
		strcpy(buf, "Riding");
#endif
	}

	/* Display the speed */
	c_put_str(attr, format("%-9s", buf), ROW_SPEED, COL_SPEED);
}


static void prt_study(void)
{
	if (p_ptr->new_spells)
	{
#ifdef JP
		put_str("�ؽ�", ROW_STUDY, COL_STUDY);
#else
		put_str("Stud", ROW_STUDY, COL_STUDY);
#endif

	}
	else
	{
		put_str("    ", ROW_STUDY, COL_STUDY);
	}
}


static void prt_mane(void)
{
	if (p_ptr->pclass == CLASS_IMITATOR)
	{
		if (mane_num)
		{
			byte attr;
			if (new_mane) attr = TERM_L_RED;
			else attr = TERM_WHITE;
#ifdef JP
			c_put_str(attr, "�ޤ�", ROW_STUDY, COL_STUDY);
#else
			c_put_str(attr, "Mane", ROW_STUDY, COL_STUDY);
#endif
		}
		else
		{
			put_str("    ", ROW_STUDY, COL_STUDY);
		}
	}
}


static void prt_cut(void)
{
	int c = p_ptr->cut;

	if (c > 1000)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "��̿��      ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_L_RED, "Mortal wound", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 200)
	{
#ifdef JP
		c_put_str(TERM_RED, "�Ҥɤ�����  ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Deep gash   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "�Ž�        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Severe cut  ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "���Ѥʽ�    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Nasty cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 25)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "�Ҥɤ���    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Bad cut     ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 10)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "�ڽ�        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Light cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "�������    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Graze       ", ROW_CUT, COL_CUT);
#endif

	}
	else
	{
		put_str("            ", ROW_CUT, COL_CUT);
	}
}



static void prt_stun(void)
{
	int s = p_ptr->stun;

	if (s > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "�ռ�������  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "�Ҥɤ�ۯ۰  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ۯ۰        ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Stun        ", ROW_STUN, COL_STUN);
#endif

	}
	else
	{
		put_str("            ", ROW_STUN, COL_STUN);
	}
}



/*
 * Redraw the "monster health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "monster health bar" provides visual feedback on the "health"
 * of the monster currently being "tracked".  There are several ways
 * to "track" a monster, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the monster health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target monster when bored.  Note that the
 * health-bar stops tracking any monster that "disappears".
 */
static void health_redraw(void)
{

#ifdef DRS_SHOW_HEALTH_BAR

	/* Not tracking */
	if (!p_ptr->health_who)
	{
		/* Erase the health bar */
		Term_erase(COL_INFO, ROW_INFO, 12);
	}

	/* Tracking an unseen monster */
	else if (!m_list[p_ptr->health_who].ml)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a hallucinatory monster */
	else if (p_ptr->image)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a dead monster (???) */
	else if (!m_list[p_ptr->health_who].hp < 0)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a visible monster */
	else
	{
		int pct, pct2, len;

		monster_type *m_ptr = &m_list[p_ptr->health_who];

		/* Default to almost dead */
		byte attr = TERM_RED;

		/* Extract the "percent" of health */
		pct = 100L * m_ptr->hp / m_ptr->maxhp;
		pct2 = 100L * m_ptr->hp / m_ptr->max_maxhp;

		/* Badly wounded */
		if (pct >= 10) attr = TERM_L_RED;

		/* Wounded */
		if (pct >= 25) attr = TERM_ORANGE;

		/* Somewhat Wounded */
		if (pct >= 60) attr = TERM_YELLOW;

		/* Healthy */
		if (pct >= 100) attr = TERM_L_GREEN;

		/* Afraid */
		if (m_ptr->monfear) attr = TERM_VIOLET;

		/* Asleep */
		if (m_ptr->csleep) attr = TERM_BLUE;

		/* Invulnerable */
		if (m_ptr->invulner) attr = TERM_WHITE;

		/* Convert percent into "health" */
		len = (pct2 < 10) ? 1 : (pct2 < 90) ? (pct2 / 10 + 1) : 10;

		/* Default to "unknown" */
		Term_putstr(COL_INFO, ROW_INFO, 12, TERM_WHITE, "[----------]");

		/* Dump the current "health" (use '*' symbols) */
		Term_putstr(COL_INFO + 1, ROW_INFO, len, attr, "**********");
	}

#endif

}



static void riding_health_redraw(void)
{

#ifdef DRS_SHOW_HEALTH_BAR

	/* Not tracking */
	if (!p_ptr->riding)
	{
		/* Erase the health bar */
		Term_erase(COL_RIDING_INFO, ROW_RIDING_INFO, 12);
	}

	/* Tracking a hallucinatory monster */
	else if (p_ptr->image)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(COL_RIDING_INFO, ROW_RIDING_INFO, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a dead monster (???) */
	else if (!m_list[p_ptr->health_who].hp < 0)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(COL_RIDING_INFO, ROW_RIDING_INFO, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a visible monster */
	else
	{
		int pct, pct2, len;

		monster_type *m_ptr = &m_list[p_ptr->riding];

		/* Default to almost dead */
		byte attr = TERM_RED;

		/* Extract the "percent" of health */
		pct = 100L * m_ptr->hp / m_ptr->maxhp;
		pct2 = 100L * m_ptr->hp / m_ptr->max_maxhp;

		/* Badly wounded */
		if (pct >= 10) attr = TERM_L_RED;

		/* Wounded */
		if (pct >= 25) attr = TERM_ORANGE;

		/* Somewhat Wounded */
		if (pct >= 60) attr = TERM_YELLOW;

		/* Healthy */
		if (pct >= 100) attr = TERM_L_GREEN;

		/* Afraid */
		if (m_ptr->monfear) attr = TERM_VIOLET;

		/* Asleep */
		if (m_ptr->csleep) attr = TERM_BLUE;

		/* Invulnerable */
		if (m_ptr->invulner) attr = TERM_WHITE;

		/* Convert percent into "health" */
		len = (pct2 < 10) ? 1 : (pct2 < 90) ? (pct2 / 10 + 1) : 10;

		/* Default to "unknown" */
		Term_putstr(COL_RIDING_INFO, ROW_RIDING_INFO, 12, TERM_WHITE, "[----------]");

		/* Dump the current "health" (use '*' symbols) */
		Term_putstr(COL_RIDING_INFO + 1, ROW_RIDING_INFO, len, attr, "**********");
	}

#endif

}



/*
 * Display basic info (mostly left of map)
 */
static void prt_frame_basic(void)
{
	int i;

	/* Race and Class */
	if (p_ptr->mimic_form)
		prt_field(mimic_info[p_ptr->mimic_form].title, ROW_RACE, COL_RACE);
	else
		prt_field(rp_ptr->title, ROW_RACE, COL_RACE);
/*	prt_field(cp_ptr->title, ROW_CLASS, COL_CLASS); */
/*	prt_field(ap_ptr->title, ROW_SEIKAKU, COL_SEIKAKU); */


	/* Title */
	prt_title();

	/* Level/Experience */
	prt_level();
	prt_exp();

	/* All Stats */
	for (i = 0; i < 6; i++) prt_stat(i);

	/* Armor */
	prt_ac();

	/* Hitpoints */
	prt_hp();

	/* Spellpoints */
	prt_sp();

	/* Gold */
	prt_gold();

	/* Current depth */
	prt_depth();

	/* Special */
	health_redraw();
	riding_health_redraw();
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra(void)
{
	/* Cut/Stun */
	prt_cut();
	prt_stun();

	/* Food */
	prt_hunger();

	/* State */
	prt_state();

	/* Speed */
	prt_speed();

	/* Study spells */
	prt_study();

	prt_mane();

	prt_status();
}


/*
 * Hack -- display inventory in sub-windows
 */
static void fix_inven(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_INVEN))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display inventory */
		display_inven();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display equipment in sub-windows
 */
static void fix_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_EQUIP))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display equipment */
		display_equip();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display equipment in sub-windows
 */
static void fix_spell(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_SPELL))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display spell list */
		display_spell_list();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display character in sub-windows
 */
static void fix_player(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_PLAYER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		update_playtime();

		/* Display player */
		display_player(0);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display recent messages in sub-windows
 *
 * XXX XXX XXX Adjust for width and split messages
 */
static void fix_message(void)
{
	int j, i;
	int w, h;
	int x, y;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MESSAGE))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Get size */
		Term_get_size(&w, &h);

		/* Dump messages */
		for (i = 0; i < h; i++)
		{
			/* Dump the message on the appropriate line */
			Term_putstr(0, (h - 1) - i, -1, (i < now_message) ? TERM_WHITE : TERM_SLATE, message_str((s16b)i));

			/* Cursor */
			Term_locate(&x, &y);

			/* Clear to end of line */
			Term_erase(x, y, 255);
		}

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display overhead view in sub-windows
 *
 * Note that the "player" symbol does NOT appear on the map.
 */
static void fix_overhead(void)
{
	int j;

	int cy, cx;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OVERHEAD))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw map */
		display_map(&cy, &cx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display dungeon view in sub-windows
 */
static void fix_dungeon(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_DUNGEON))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw dungeon view */
		display_dungeon();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display monster recall in sub-windows
 */
static void fix_monster(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MONSTER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->monster_race_idx) display_roff(p_ptr->monster_race_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display object recall in sub-windows
 */
static void fix_object(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OBJECT))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->object_kind_idx) display_koff(p_ptr->object_kind_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
static void calc_spells(void)
{
	int			i, j, k, levels;
	int			num_allowed;
	int                     num_boukyaku = 0;

	magic_type		*s_ptr;
	int use_realm1 = p_ptr->realm1 - 1;
	int use_realm2 = p_ptr->realm2 - 1;
	int which;
	int bonus = 0;


	cptr p;

	/* Hack -- must be literate */
	if (!mp_ptr->spell_book) return;

	/* Hack -- wait for creation */
	if (!character_generated) return;

	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
	{
		p_ptr->new_spells = 0;
		return;
	}

	p = spell_categoly_name(mp_ptr->spell_book);

	/* Determine the number of spells allowed */
	levels = p_ptr->lev - mp_ptr->spell_first + 1;

	/* Hack -- no negative spells */
	if (levels < 0) levels = 0;

	/* Extract total allowed spells */
	num_allowed = (adj_mag_study[p_ptr->stat_ind[mp_ptr->spell_stat]] * levels / 2);

	if ((p_ptr->pclass != CLASS_SAMURAI) && (mp_ptr->spell_book != TV_LIFE_BOOK))
	{
		bonus = 4;
	}
	if (p_ptr->pclass == CLASS_SAMURAI)
	{
		num_allowed = 32;
	}
	else if (p_ptr->realm2 == REALM_NONE)
	{
		num_allowed = (num_allowed+1)/2;
		if (num_allowed>(32+bonus)) num_allowed = 32+bonus;
	}
	else if ((p_ptr->pclass == CLASS_MAGE) || (p_ptr->pclass == CLASS_PRIEST))
	{
		if (num_allowed>(96+bonus)) num_allowed = 96+bonus;
	}
	else
	{
		if (num_allowed>(80+bonus)) num_allowed = 80+bonus;
	}

        /* Count the number of spells we know */
        for (j = 0; j < 64; j++)
	{
		/* Count known spells */
		if ((j < 32) ?
                    (spell_forgotten1 & (1L << j)) :
                    (spell_forgotten2 & (1L << (j - 32))))
		{
			num_boukyaku++;
                }
        }

	/* See how many spells we must forget or may learn */
	p_ptr->new_spells = num_allowed + p_ptr->add_spells + num_boukyaku - p_ptr->learned_spells;

	/* Forget spells which are too hard */
	for (i = 63; i >= 0; i--)
	{
		/* Efficiency -- all done */
		if (!spell_learned1 && !spell_learned2) break;

		/* Access the spell */
		j = spell_order[i];

		/* Skip non-spells */
		if (j >= 99) continue;


		/* Get the spell */
		if (!is_magic(((j < 32) ? use_realm1 : use_realm2)+1))
		{
			if (j < 32)
				s_ptr = &technic_info[use_realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[use_realm2 - MIN_TECHNIC][j%32];
		}
		else if (j < 32)
			s_ptr = &mp_ptr->info[use_realm1][j];
		else
			s_ptr = &mp_ptr->info[use_realm2][j%32];

		/* Skip spells we are allowed to know */
		if (s_ptr->slevel <= p_ptr->lev) continue;

		/* Is it known? */
		if ((j < 32) ?
		    (spell_learned1 & (1L << j)) :
		    (spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				spell_forgotten1 |= (1L << j);
				which = use_realm1;
			}
			else
			{
				spell_forgotten2 |= (1L << (j - 32));
				which = use_realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				spell_learned1 &= ~(1L << j);
				which = use_realm1;
			}
			else
			{
				spell_learned2 &= ~(1L << (j - 32));
				which = use_realm2;
			}

			/* Message */
#ifdef JP
                        msg_format("%s��%s��˺��Ƥ��ޤä���",
				   spell_names[technic2magic(which+1)-1][j%32], p );
#else
			msg_format("You have forgotten the %s of %s.", p,
			spell_names[technic2magic(which+1)-1][j%32]);
#endif


			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Forget spells if we know too many spells */
	for (i = 63; i >= 0; i--)
	{
		/* Stop when possible */
		if (p_ptr->new_spells >= 0) break;

		/* Efficiency -- all done */
		if (!spell_learned1 && !spell_learned2) break;

		/* Get the (i+1)th spell learned */
		j = spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) continue;

		/* Forget it (if learned) */
		if ((j < 32) ?
		    (spell_learned1 & (1L << j)) :
		    (spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				spell_forgotten1 |= (1L << j);
				which = use_realm1;
			}
			else
			{
				spell_forgotten2 |= (1L << (j - 32));
				which = use_realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				spell_learned1 &= ~(1L << j);
				which = use_realm1;
			}
			else
			{
				spell_learned2 &= ~(1L << (j - 32));
				which = use_realm2;
			}

			/* Message */
#ifdef JP
                        msg_format("%s��%s��˺��Ƥ��ޤä���",
				   spell_names[technic2magic(which+1)-1][j%32], p );
#else
			msg_format("You have forgotten the %s of %s.", p,
			           spell_names[technic2magic(which+1)-1][j%32]);
#endif


			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Check for spells to remember */
	for (i = 0; i < 64; i++)
	{
		/* None left to remember */
		if (p_ptr->new_spells <= 0) break;

		/* Efficiency -- all done */
		if (!spell_forgotten1 && !spell_forgotten2) break;

		/* Get the next spell we learned */
		j = spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) break;

		/* Access the spell */
		if (!is_magic(((j < 32) ? use_realm1 : use_realm2)+1))
		{
			if (j < 32)
				s_ptr = &technic_info[use_realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[use_realm2 - MIN_TECHNIC][j%32];
		}
		else if (j<32)
			s_ptr = &mp_ptr->info[use_realm1][j];
		else
			s_ptr = &mp_ptr->info[use_realm2][j%32];

		/* Skip spells we cannot remember */
		if (s_ptr->slevel > p_ptr->lev) continue;

		/* First set of spells */
		if ((j < 32) ?
		    (spell_forgotten1 & (1L << j)) :
		    (spell_forgotten2 & (1L << (j - 32))))
		{
			/* No longer forgotten */
			if (j < 32)
			{
				spell_forgotten1 &= ~(1L << j);
				which = use_realm1;
			}
			else
			{
				spell_forgotten2 &= ~(1L << (j - 32));
				which = use_realm2;
			}

			/* Known once more */
			if (j < 32)
			{
				spell_learned1 |= (1L << j);
				which = use_realm1;
			}
			else
			{
				spell_learned2 |= (1L << (j - 32));
				which = use_realm2;
			}

			/* Message */
#ifdef JP
                        msg_format("%s��%s��פ��Ф�����",
				   spell_names[technic2magic(which+1)-1][j%32], p );
#else
			msg_format("You have remembered the %s of %s.",
			           p, spell_names[technic2magic(which+1)-1][j%32]);
#endif


			/* One less can be learned */
			p_ptr->new_spells--;
		}
	}

	k = 0;

	if (p_ptr->realm2 == REALM_NONE)
	{
		/* Count spells that can be learned */
		for (j = 0; j < 32; j++)
		{
			if (!is_magic(use_realm1+1)) s_ptr = &technic_info[use_realm1-MIN_TECHNIC][j];
			else s_ptr = &mp_ptr->info[use_realm1][j];

			/* Skip spells we cannot remember */
			if (s_ptr->slevel > p_ptr->lev) continue;

			/* Skip spells we already know */
			if (spell_learned1 & (1L << j))
			{
				continue;
			}

			/* Count it */
			k++;
		}
		if (k>32) k = 32;
		if ((p_ptr->new_spells > k) && ((mp_ptr->spell_book == TV_LIFE_BOOK) || (mp_ptr->spell_book == TV_HISSATSU_BOOK))) p_ptr->new_spells = k;
	}

	if (p_ptr->new_spells < 0) p_ptr->new_spells = 0;

	/* Spell count changed */
	if (p_ptr->old_spells != p_ptr->new_spells)
	{
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

		/* Save the new_spells value */
		p_ptr->old_spells = p_ptr->new_spells;

		/* Redraw Study Status */
		p_ptr->redraw |= (PR_STUDY);
	}
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static void calc_mana(void)
{
	int		msp, levels, cur_wgt, max_wgt;

	object_type	*o_ptr;


	/* Hack -- Must be literate */
	if (!mp_ptr->spell_book) return;

	if ((p_ptr->pclass == CLASS_MINDCRAFTER) ||
	    (p_ptr->pclass == CLASS_MIRROR_MASTER) ||
	    (p_ptr->pclass == CLASS_BLUE_MAGE))
	{
		levels = p_ptr->lev;
	}
	else
	{
		if(mp_ptr->spell_first > p_ptr->lev)
		{
			/* Save new mana */
			p_ptr->msp = 0;

			/* Display mana later */
			p_ptr->redraw |= (PR_MANA);
			return;
		}

		/* Extract "effective" player level */
		levels = (p_ptr->lev - mp_ptr->spell_first) + 1;
	}

	if (p_ptr->pclass == CLASS_SAMURAI)
	{
		msp = (adj_mag_mana[p_ptr->stat_ind[mp_ptr->spell_stat]] + 10) * 2;
		if (msp) msp += (msp * rp_ptr->r_adj[mp_ptr->spell_stat] / 20);
	}
	else
	{
		/* Extract total mana */
		msp = adj_mag_mana[p_ptr->stat_ind[mp_ptr->spell_stat]] * (levels+3) / 4;

		/* Hack -- usually add one mana */
		if (msp) msp++;

		if (msp) msp += (msp * rp_ptr->r_adj[mp_ptr->spell_stat] / 20);

		if (msp && (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)) msp += msp/2;

		/* Hack: High mages have a 25% mana bonus */
		if (msp && (p_ptr->pclass == CLASS_HIGH_MAGE)) msp += msp / 4;

		if (msp && (p_ptr->pclass == CLASS_SORCERER)) msp += msp*(25+p_ptr->lev)/100;
	}

	/* Only mages are affected */
	if (mp_ptr->spell_xtra & MAGIC_GLOVE_REDUCE_MANA)
	{
		u32b f1, f2, f3;

		/* Assume player is not encumbered by gloves */
		p_ptr->cumber_glove = FALSE;

		/* Get the gloves */
		o_ptr = &inventory[INVEN_HANDS];

		/* Examine the gloves */
		object_flags(o_ptr, &f1, &f2, &f3);

		/* Normal gloves hurt mage-type spells */
		if (o_ptr->k_idx &&
		    !(f2 & (TR2_FREE_ACT)) &&
		    !(f1 & (TR1_MAGIC_MASTERY)) &&
		    !((f1 & (TR1_DEX)) && (o_ptr->pval > 0)))
		{
			/* Encumbered */
			p_ptr->cumber_glove = TRUE;

			/* Reduce mana */
			msp = (3 * msp) / 4;
		}
	}


	/* Assume player not encumbered by armor */
	p_ptr->cumber_armor = FALSE;

	/* Weigh the armor */
	cur_wgt = 0;
	if(inventory[INVEN_RARM].tval> TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight;
	if(inventory[INVEN_LARM].tval> TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight;
	cur_wgt += inventory[INVEN_BODY].weight;
	cur_wgt += inventory[INVEN_HEAD].weight;
	cur_wgt += inventory[INVEN_OUTER].weight;
	cur_wgt += inventory[INVEN_HANDS].weight;
	cur_wgt += inventory[INVEN_FEET].weight;

	/* Subtract a percentage of maximum mana. */
	switch (p_ptr->pclass)
	{
		/* For these classes, mana is halved if armour 
		 * is 30 pounds over their weight limit. */
		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_BLUE_MAGE:
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		case CLASS_SORCERER:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight;
			break;
		}

		/* Mana halved if armour is 40 pounds over weight limit. */
		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_TOURIST:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight*2/3;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight*2/3;
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_BEASTMASTER:
		case CLASS_MIRROR_MASTER:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/2;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/2;
			break;
		}

		/* Mana halved if armour is 50 pounds over weight limit. */
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_RED_MAGE:
		case CLASS_WARRIOR_MAGE:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/3;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/3;
			break;
		}

		/* Mana halved if armour is 60 pounds over weight limit. */
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/5;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/5;
			break;
		}

		/* For new classes created, but not yet added to this formula. */
		default:
		{
			break;
		}
	}

	/* Determine the weight allowance */
	max_wgt = mp_ptr->spell_weight;

	/* Heavy armor penalizes mana by a percentage.  -LM- */
	if ((cur_wgt - max_wgt) > 0)
	{
		/* Encumbered */
		p_ptr->cumber_armor = TRUE;

		/* Subtract a percentage of maximum mana. */
		switch (p_ptr->pclass)
		{
			/* For these classes, mana is halved if armour 
			 * is 30 pounds over their weight limit. */
			case CLASS_MAGE:
			case CLASS_HIGH_MAGE:
			case CLASS_BLUE_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 600;
				break;
			}

			/* Mana halved if armour is 40 pounds over weight limit. */
			case CLASS_PRIEST:
			case CLASS_MINDCRAFTER:
			case CLASS_BEASTMASTER:
			case CLASS_BARD:
			case CLASS_FORCETRAINER:
			case CLASS_TOURIST:
			case CLASS_MIRROR_MASTER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}

			case CLASS_SORCERER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 900;
				break;
			}

			/* Mana halved if armour is 50 pounds over weight limit. */
			case CLASS_ROGUE:
			case CLASS_RANGER:
			case CLASS_MONK:
			case CLASS_RED_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1000;
				break;
			}

			/* Mana halved if armour is 60 pounds over weight limit. */
			case CLASS_PALADIN:
			case CLASS_CHAOS_WARRIOR:
			case CLASS_WARRIOR_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1200;
				break;
			}

			case CLASS_SAMURAI:
			{
				p_ptr->cumber_armor = FALSE;
				break;
			}

			/* For new classes created, but not yet added to this formula. */
			default:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}
		}
	}

	/* Mana can never be negative */
	if (msp < 0) msp = 0;


	/* Maximum mana has changed */
	if (p_ptr->msp != msp)
	{
		/* Enforce maximum */
		if ((p_ptr->csp >= msp) && (p_ptr->pclass != CLASS_SAMURAI))
		{
			p_ptr->csp = msp;
			p_ptr->csp_frac = 0;
		}

#ifdef JP
                /* ��٥륢�åפλ��Ͼ徺�̤�ɽ������ */
                if ((level_up == 1) && (msp > p_ptr->msp))
                {
                        msg_format("����ޥ��å����ݥ���Ȥ� %d ���ä�����",
                                   (msp - p_ptr->msp));
                }
#endif
		/* Save new mana */
		p_ptr->msp = msp;

		/* Display mana later */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
	}


	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	/* Take note when "glove state" changes */
	if (p_ptr->old_cumber_glove != p_ptr->cumber_glove)
	{
		/* Message */
		if (p_ptr->cumber_glove)
		{
#ifdef JP
			msg_print("�꤬ʤ���Ƽ�ʸ�������ˤ������������롣");
#else
			msg_print("Your covered hands feel unsuitable for spellcasting.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("���μ�ξ��֤ʤ顢���äȼ�ʸ�������䤹����������");
#else
			msg_print("Your hands feel more suitable for spellcasting.");
#endif

		}

		/* Save it */
		p_ptr->old_cumber_glove = p_ptr->cumber_glove;
	}


	/* Take note when "armor state" changes */
	if (p_ptr->old_cumber_armor != p_ptr->cumber_armor)
	{
		/* Message */
		if (p_ptr->cumber_armor)
		{
#ifdef JP
			msg_print("�����νŤ���ư�����ߤ��ʤäƤ��ޤäƤ��롣");
#else
			msg_print("The weight of your equipment encumbers your movement.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("���äȳڤ��Τ�ư������褦�ˤʤä���");
#else
			msg_print("You feel able to move more freely.");
#endif

		}

		/* Save it */
		p_ptr->old_cumber_armor = p_ptr->cumber_armor;
	}
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
static void calc_hitpoints(void)
{
	int bonus, mhp;
	byte tmp_hitdie;

	/* Un-inflate "half-hitpoint bonus per level" value */
	bonus = ((int)(adj_con_mhp[p_ptr->stat_ind[A_CON]]) - 128) * p_ptr->lev / 4;

	/* Calculate hitpoints */
	mhp = player_hp[p_ptr->lev - 1];

	if (p_ptr->mimic_form)
	{
		if (p_ptr->pclass == CLASS_SORCERER)
			tmp_hitdie = mimic_info[p_ptr->mimic_form].r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
		else
			tmp_hitdie = mimic_info[p_ptr->mimic_form].r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;
		mhp = mhp * tmp_hitdie / p_ptr->hitdie;
	}

	if (p_ptr->pclass == CLASS_SORCERER)
	{
		if (p_ptr->lev < 30)
			mhp = (mhp * (45+p_ptr->lev) / 100);
		else
			mhp = (mhp * 75 / 100);
		bonus = (bonus * 65 / 100);
	}

	mhp += bonus;

	if (p_ptr->pclass == CLASS_BERSERKER)
	{
		mhp = mhp*(110+(((p_ptr->lev + 40) * (p_ptr->lev + 40) - 1550) / 110))/100;
	}

	/* Always have at least one hitpoint per level */
	if (mhp < p_ptr->lev + 1) mhp = p_ptr->lev + 1;

	/* Factor in the hero / superhero settings */
	if (p_ptr->hero || music_singing(MUSIC_HERO) || music_singing(MUSIC_SHERO))
	if (p_ptr->shero) mhp += 30;
	if (p_ptr->tsuyoshi) mhp += 50;

	/* New maximum hitpoints */
	if (p_ptr->mhp != mhp)
	{
		/* Enforce maximum */
		if (p_ptr->chp >= mhp)
		{
			p_ptr->chp = mhp;
			p_ptr->chp_frac = 0;
		}

#ifdef JP
                /* ��٥륢�åפλ��Ͼ徺�̤�ɽ������ */
                if ((level_up == 1) && (mhp > p_ptr->mhp))
                {
                        msg_format("����ҥåȡ��ݥ���Ȥ� %d ���ä�����",
                                   (mhp - p_ptr->mhp) );
                }
#endif
		/* Save the new max-hitpoints */
		p_ptr->mhp = mhp;

		/* Display hitpoints (later) */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}



/*
 * Extract and set the current "lite radius"
 *
 * SWD: Experimental modification: multiple light sources have additive effect.
 *
 */
static void calc_torch(void)
{
	int i;
	object_type *o_ptr;
	u32b f1, f2, f3;

	/* Assume no light */
	p_ptr->cur_lite = 0;

	/* Loop through all wielded items */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Examine actual lites */
		if ((i == INVEN_LITE) && (o_ptr->k_idx) && (o_ptr->tval == TV_LITE))
		{
			if (o_ptr->name2 == EGO_LITE_DARKNESS)
			{
				if (o_ptr->sval == SV_LITE_TORCH)
				{
					p_ptr->cur_lite -= 1;
				}

				/* Lanterns (with fuel) provide more lite */
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
					p_ptr->cur_lite -= 2;
				}

				else if (o_ptr->sval == SV_LITE_FEANOR)
				{
					p_ptr->cur_lite -= 3;
				}
			}
			/* Torches (with fuel) provide some lite */
			else if ((o_ptr->sval == SV_LITE_TORCH) && (o_ptr->xtra4 > 0))
			{
				p_ptr->cur_lite += 1;
			}

			/* Lanterns (with fuel) provide more lite */
			else if ((o_ptr->sval == SV_LITE_LANTERN) && (o_ptr->xtra4 > 0))
			{
				p_ptr->cur_lite += 2;
			}

			else if (o_ptr->sval == SV_LITE_FEANOR)
			{
				p_ptr->cur_lite += 2;
			}

			/* Artifact Lites provide permanent, bright, lite */
			else if (artifact_p(o_ptr))
			{
				p_ptr->cur_lite += 3;
			}

			if (o_ptr->name2 == EGO_LITE_SHINE) p_ptr->cur_lite++;
		}
		else
		{
			/* Skip empty slots */
			if (!o_ptr->k_idx) continue;

			/* Extract the flags */
			object_flags(o_ptr, &f1, &f2, &f3);

			/* does this item glow? */
			if (f3 & TR3_LITE)
			{
				if ((o_ptr->name2 == EGO_DARK) || (o_ptr->name1 == ART_NIGHT)) p_ptr->cur_lite--;
				else p_ptr->cur_lite++;
			}
		}

	}

	/* max radius is 5 without rewriting other code -- */
	/* see cave.c:update_lite() and defines.h:LITE_MAX */
	if (d_info[dungeon_type].flags1 & DF1_DARKNESS && p_ptr->cur_lite > 1)
		p_ptr->cur_lite = 1;
	if (p_ptr->cur_lite > 5) p_ptr->cur_lite = 5;
	if (p_ptr->cur_lite < 0) p_ptr->cur_lite = 0;

	/* check if the player doesn't have a lite source, */
	/* but does glow as an intrinsic.                  */
	if (p_ptr->cur_lite == 0 && p_ptr->lite) p_ptr->cur_lite = 1;

	/* end experimental mods */

	/* Reduce lite when running if requested */
	if (running && view_reduce_lite)
	{
		/* Reduce the lite radius if needed */
		if (p_ptr->cur_lite > 1) p_ptr->cur_lite = 1;
	}

	/* Notice changes in the "lite radius" */
	if (p_ptr->old_lite != p_ptr->cur_lite)
	{
		/* Update the lite */
		p_ptr->update |= (PU_LITE);

		/* Update the monsters */
		p_ptr->update |= (PU_MONSTERS);

		/* Remember the old lite */
		p_ptr->old_lite = p_ptr->cur_lite;

		if ((p_ptr->cur_lite > 0) && (p_ptr->special_defense & NINJA_S_STEALTH))
			set_superstealth(FALSE);
	}
}



/*
 * Computes current weight limit.
 */
static int weight_limit(void)
{
	int i;

	/* Weight limit based only on strength */
	i = adj_str_wgt[p_ptr->stat_ind[A_STR]] * 100;
	if (p_ptr->pclass == CLASS_BERSERKER) i = i*3/2;

	/* Return the result */
	return (i);
}


bool buki_motteruka(int i)
{
	return ((inventory[i].k_idx && inventory[i].tval >= TV_DIGGING && inventory[i].tval <= TV_SWORD) ? TRUE : FALSE);
}

/*
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * This function induces various "status" messages.
 */
void calc_bonuses(void)
{
	int             i, j, hold, neutral[2];
	int             old_speed;
	int             old_telepathy;
	int             old_see_inv;
	int             old_dis_ac;
	int             old_dis_to_a;
	int             extra_blows[2];
	int             extra_shots;
	object_type     *o_ptr;
	u32b            f1, f2, f3;
	bool            omoi = FALSE;
	bool            yoiyami = FALSE;
	bool            down_saving = FALSE;
	bool            have_dd_s, have_dd_t, have_sw, have_kabe;
	bool            easy_2hand = FALSE;
	s16b this_o_idx, next_o_idx = 0;
	player_race *tmp_rp_ptr;


	/* Save the old speed */
	old_speed = p_ptr->pspeed;

	/* Save the old vision stuff */
	old_telepathy = p_ptr->telepathy;
	old_see_inv = p_ptr->see_inv;

	/* Save the old armor class */
	old_dis_ac = p_ptr->dis_ac;
	old_dis_to_a = p_ptr->dis_to_a;


	/* Clear extra blows/shots */
	extra_blows[0] = extra_blows[1] = extra_shots = 0;

	/* Clear the stat modifiers */
	for (i = 0; i < 6; i++) p_ptr->stat_add[i] = 0;


	/* Clear the Displayed/Real armor class */
	p_ptr->dis_ac = p_ptr->ac = 0;

	/* Clear the Displayed/Real Bonuses */
	p_ptr->dis_to_h[0] = p_ptr->to_h[0] = 0;
	p_ptr->dis_to_h[1] = p_ptr->to_h[1] = 0;
	p_ptr->dis_to_d[0] = p_ptr->to_d[0] = 0;
	p_ptr->dis_to_d[1] = p_ptr->to_d[1] = 0;
	p_ptr->dis_to_h_b = p_ptr->to_h_b = 0;
	p_ptr->dis_to_a = p_ptr->to_a = 0;
	p_ptr->to_h_m = 0;
	p_ptr->to_d_m = 0;

	/* Start with "normal" speed */
	p_ptr->pspeed = 110;

	/* Start with a single blow per turn */
	p_ptr->num_blow[0] = 1;
	p_ptr->num_blow[1] = 1;

	/* Start with a single shot per turn */
	p_ptr->num_fire = 100;

	/* Reset the "xtra" tval */
	p_ptr->tval_xtra = 0;

	/* Reset the "ammo" tval */
	p_ptr->tval_ammo = 0;

	/* Clear all the flags */
	p_ptr->aggravate = FALSE;
	p_ptr->teleport = FALSE;
	p_ptr->exp_drain = FALSE;
	p_ptr->bless_blade = FALSE;
	p_ptr->xtra_might = FALSE;
	p_ptr->impact[0] = FALSE;
	p_ptr->impact[1] = FALSE;
	p_ptr->pass_wall = FALSE;
	p_ptr->kill_wall = FALSE;
	p_ptr->dec_mana = FALSE;
	p_ptr->easy_spell = FALSE;
	p_ptr->heavy_spell = FALSE;
	p_ptr->see_inv = FALSE;
	p_ptr->free_act = FALSE;
	p_ptr->slow_digest = FALSE;
	p_ptr->regenerate = FALSE;
	p_ptr->can_swim = FALSE;
	p_ptr->ffall = FALSE;
	p_ptr->hold_life = FALSE;
	p_ptr->telepathy = FALSE;
	p_ptr->lite = FALSE;
	p_ptr->sustain_str = FALSE;
	p_ptr->sustain_int = FALSE;
	p_ptr->sustain_wis = FALSE;
	p_ptr->sustain_con = FALSE;
	p_ptr->sustain_dex = FALSE;
	p_ptr->sustain_chr = FALSE;
	p_ptr->resist_acid = FALSE;
	p_ptr->resist_elec = FALSE;
	p_ptr->resist_fire = FALSE;
	p_ptr->resist_cold = FALSE;
	p_ptr->resist_pois = FALSE;
	p_ptr->resist_conf = FALSE;
	p_ptr->resist_sound = FALSE;
	p_ptr->resist_lite = FALSE;
	p_ptr->resist_dark = FALSE;
	p_ptr->resist_chaos = FALSE;
	p_ptr->resist_disen = FALSE;
	p_ptr->resist_shard = FALSE;
	p_ptr->resist_nexus = FALSE;
	p_ptr->resist_blind = FALSE;
	p_ptr->resist_neth = FALSE;
	p_ptr->resist_time = FALSE;
	p_ptr->resist_fear = FALSE;
	p_ptr->reflect = FALSE;
	p_ptr->sh_fire = FALSE;
	p_ptr->sh_elec = FALSE;
	p_ptr->sh_cold = FALSE;
	p_ptr->anti_magic = FALSE;
	p_ptr->anti_tele = FALSE;
	p_ptr->warning = FALSE;
	p_ptr->mighty_throw = FALSE;

	p_ptr->immune_acid = FALSE;
	p_ptr->immune_elec = FALSE;
	p_ptr->immune_fire = FALSE;
	p_ptr->immune_cold = FALSE;

	p_ptr->ryoute = FALSE;
	p_ptr->migite = FALSE;
	p_ptr->hidarite = FALSE;
	p_ptr->no_flowed = FALSE;

	p_ptr->align = 0;

	if (p_ptr->mimic_form) tmp_rp_ptr = &mimic_info[p_ptr->mimic_form];
	else tmp_rp_ptr = &race_info[p_ptr->prace];

	/* Base infravision (purely racial) */
	p_ptr->see_infra = tmp_rp_ptr->infra;

	/* Base skill -- disarming */
	p_ptr->skill_dis = tmp_rp_ptr->r_dis + cp_ptr->c_dis + ap_ptr->a_dis;

	/* Base skill -- magic devices */
	p_ptr->skill_dev = tmp_rp_ptr->r_dev + cp_ptr->c_dev + ap_ptr->a_dev;

	/* Base skill -- saving throw */
	p_ptr->skill_sav = tmp_rp_ptr->r_sav + cp_ptr->c_sav + ap_ptr->a_sav;

	/* Base skill -- stealth */
	p_ptr->skill_stl = tmp_rp_ptr->r_stl + cp_ptr->c_stl + ap_ptr->a_stl;

	/* Base skill -- searching ability */
	p_ptr->skill_srh = tmp_rp_ptr->r_srh + cp_ptr->c_srh + ap_ptr->a_srh;

	/* Base skill -- searching frequency */
	p_ptr->skill_fos = tmp_rp_ptr->r_fos + cp_ptr->c_fos + ap_ptr->a_fos;

	/* Base skill -- combat (normal) */
	p_ptr->skill_thn = tmp_rp_ptr->r_thn + cp_ptr->c_thn + ap_ptr->a_thn;

	/* Base skill -- combat (shooting) */
	p_ptr->skill_thb = tmp_rp_ptr->r_thb + cp_ptr->c_thb + ap_ptr->a_thb;

	/* Base skill -- combat (throwing) */
	p_ptr->skill_tht = tmp_rp_ptr->r_thb + cp_ptr->c_thb + ap_ptr->a_thb;

	/* Base skill -- digging */
	p_ptr->skill_dig = 0;

	if (buki_motteruka(INVEN_RARM) && (empty_hands(FALSE) & 0x00000001) && ((inventory[INVEN_RARM].weight > 99) || (inventory[INVEN_RARM].tval == TV_POLEARM)) && (!p_ptr->riding || (p_ptr->pet_extra_flags & PF_RYOUTE))) p_ptr->ryoute = TRUE;
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(TRUE) == 3) && (!p_ptr->riding || (p_ptr->pet_extra_flags & PF_RYOUTE))) p_ptr->ryoute = TRUE;
	if (buki_motteruka(INVEN_RARM) || !buki_motteruka(INVEN_LARM)) p_ptr->migite = TRUE;
	if (buki_motteruka(INVEN_LARM)) p_ptr->hidarite = TRUE;

	if (p_ptr->special_defense & KAMAE_MASK)
	{
		if (empty_hands(TRUE) < 2)
		{
			set_action(ACTION_NONE);
		}
	}

#ifdef USE_SCRIPT

	if (!get_player_flags_callback())

#endif /* USE_SCRIPT */

	{
		switch (p_ptr->pclass)
		{
			case CLASS_WARRIOR:
				if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
				if (p_ptr->lev > 44) p_ptr->regenerate = TRUE;
				break;
			case CLASS_PALADIN:
				if (p_ptr->lev > 39) p_ptr->resist_fear = TRUE;
				break;
			case CLASS_CHAOS_WARRIOR:
				if (p_ptr->lev > 29) p_ptr->resist_chaos = TRUE;
				if (p_ptr->lev > 39) p_ptr->resist_fear = TRUE;
				break;
			case CLASS_MINDCRAFTER:
				if (p_ptr->lev >  9) p_ptr->resist_fear = TRUE;
				if (p_ptr->lev > 19) p_ptr->sustain_wis = TRUE;
				if (p_ptr->lev > 29) p_ptr->resist_conf = TRUE;
				if (p_ptr->lev > 39) p_ptr->telepathy = TRUE;
				break;
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
				/* Unencumbered Monks become faster every 10 levels */
				if (!(heavy_armor()))
				{
					if (!((p_ptr->prace == RACE_KLACKON) ||
						(p_ptr->prace == RACE_SPRITE) ||
						(p_ptr->pseikaku == SEIKAKU_MUNCHKIN)))
						p_ptr->pspeed += (p_ptr->lev) / 10;

					/* Free action if unencumbered at level 25 */
					if  (p_ptr->lev > 24)
						p_ptr->free_act = TRUE;
				}
				break;
			case CLASS_SORCERER:
				p_ptr->to_a -= 50;
				p_ptr->dis_to_a -= 50;
				break;
			case CLASS_BARD:
				p_ptr->resist_sound = TRUE;
				break;
			case CLASS_SAMURAI:
				if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
				break;
			case CLASS_BERSERKER:
				p_ptr->shero = 1;
				p_ptr->sustain_str = TRUE;
				p_ptr->sustain_dex = TRUE;
				p_ptr->sustain_con = TRUE;
				p_ptr->regenerate = TRUE;
				p_ptr->free_act = TRUE;
				p_ptr->pspeed += 2;
				if (p_ptr->lev > 29) p_ptr->pspeed++;
				if (p_ptr->lev > 39) p_ptr->pspeed++;
				if (p_ptr->lev > 44) p_ptr->pspeed++;
				if (p_ptr->lev > 49) p_ptr->pspeed++;
				p_ptr->to_a += 10+p_ptr->lev/2;
				p_ptr->dis_to_a += 10+p_ptr->lev/2;
				p_ptr->skill_dig += (100+p_ptr->lev*8);
				if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
				p_ptr->redraw |= PR_STATUS;
				break;
			case CLASS_MIRROR_MASTER:
				if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
				break;
			case CLASS_NINJA:
				/* Unencumbered Monks become faster every 10 levels */
				if (heavy_armor())
				{
					p_ptr->pspeed -= (p_ptr->lev) / 10;
					p_ptr->skill_stl -= (p_ptr->lev)/10;
				}
				else
				{
					if (!inventory[INVEN_LARM].tval || p_ptr->hidarite)
					{
						p_ptr->pspeed += 3;
					if (!((p_ptr->prace == RACE_KLACKON) ||
						(p_ptr->prace == RACE_SPRITE) ||
						(p_ptr->pseikaku == SEIKAKU_MUNCHKIN)))
						p_ptr->pspeed += (p_ptr->lev) / 10;

					p_ptr->skill_stl += (p_ptr->lev)/10;
					
					/* Free action if unencumbered at level 25 */
					if  (p_ptr->lev > 24)
						p_ptr->free_act = TRUE;
					}
				}
				if (!inventory[INVEN_LARM].tval || p_ptr->hidarite)
				{
					p_ptr->to_a += p_ptr->lev/2+5;
					p_ptr->dis_to_a += p_ptr->lev/2+5;
				}
				p_ptr->slow_digest = TRUE;
				p_ptr->resist_fear = TRUE;
				if (p_ptr->lev > 19) p_ptr->resist_pois = TRUE;
				if (p_ptr->lev > 24) p_ptr->sustain_dex = TRUE;
				if (p_ptr->lev > 29) p_ptr->see_inv = TRUE;
				if (p_ptr->lev > 44)
				{
					p_ptr->oppose_pois = 1;
					p_ptr->redraw |= PR_STATUS;
				}
				break;
		}
	}

	/***** Races ****/
	if (p_ptr->mimic_form)
	{
		switch(p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
                        p_ptr->hold_life=TRUE;
                        p_ptr->resist_chaos=TRUE;
                        p_ptr->resist_neth=TRUE;
                        p_ptr->resist_fire=TRUE;
			p_ptr->oppose_fire = 1;
                        p_ptr->see_inv=TRUE;
			p_ptr->pspeed += 3;
			p_ptr->redraw |= PR_STATUS;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
			break;
		case MIMIC_DEMON_LORD:
                        p_ptr->hold_life=TRUE;
                        p_ptr->resist_chaos=TRUE;
                        p_ptr->resist_neth=TRUE;
                        p_ptr->immune_fire=TRUE;
                        p_ptr->resist_acid = TRUE;
                        p_ptr->resist_fire=TRUE;
                        p_ptr->resist_cold = TRUE;
                        p_ptr->resist_elec = TRUE;
                        p_ptr->resist_pois = TRUE;
                        p_ptr->resist_conf = TRUE;
                        p_ptr->resist_disen = TRUE;
                        p_ptr->resist_nexus = TRUE;
                        p_ptr->resist_fear = TRUE;
                        p_ptr->sh_fire = TRUE;
                        p_ptr->see_inv = TRUE;
			p_ptr->telepathy = TRUE;
			p_ptr->ffall = TRUE;
			p_ptr->kill_wall = TRUE;
			p_ptr->pspeed += 5;
			p_ptr->to_a += 20;
			p_ptr->dis_to_a += 20;
			break;
		case MIMIC_VAMPIRE:
			p_ptr->resist_dark = TRUE;
			p_ptr->hold_life = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->pspeed += 3;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
			if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;
			break;
		}
	}
	else
	{
	switch (p_ptr->prace)
	{
		case RACE_ELF:
			p_ptr->resist_lite = TRUE;
			break;
		case RACE_HOBBIT:
			p_ptr->sustain_dex = TRUE;
			break;
		case RACE_GNOME:
			p_ptr->free_act = TRUE;
			break;
		case RACE_DWARF:
			p_ptr->resist_blind = TRUE;
			break;
		case RACE_HALF_ORC:
			p_ptr->resist_dark = TRUE;
			break;
		case RACE_HALF_TROLL:
			p_ptr->sustain_str = TRUE;

			if (p_ptr->lev > 14)
			{
				/* High level trolls heal fast... */
				p_ptr->regenerate = TRUE;

				if (p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_BERSERKER)
				{
					p_ptr->slow_digest = TRUE;
					/* Let's not make Regeneration
					 * a disadvantage for the poor warriors who can
					 * never learn a spell that satisfies hunger (actually
					 * neither can rogues, but half-trolls are not
					 * supposed to play rogues) */
				}
			}
			break;
		case RACE_AMBERITE:
			p_ptr->sustain_con = TRUE;
			p_ptr->regenerate = TRUE;  /* Amberites heal fast... */
			break;
		case RACE_HIGH_ELF:
			p_ptr->resist_lite = TRUE;
			p_ptr->see_inv = TRUE;
			break;
		case RACE_BARBARIAN:
			p_ptr->resist_fear = TRUE;
			break;
		case RACE_HALF_OGRE:
			p_ptr->resist_dark = TRUE;
			p_ptr->sustain_str = TRUE;
			break;
		case RACE_HALF_GIANT:
			p_ptr->sustain_str = TRUE;
			p_ptr->resist_shard = TRUE;
			break;
		case RACE_HALF_TITAN:
			p_ptr->resist_chaos = TRUE;
			break;
		case RACE_CYCLOPS:
			p_ptr->resist_sound = TRUE;
			break;
		case RACE_YEEK:
			p_ptr->resist_acid = TRUE;
			if (p_ptr->lev > 19) p_ptr->immune_acid = TRUE;
			break;
		case RACE_KLACKON:
			p_ptr->resist_conf = TRUE;
			p_ptr->resist_acid = TRUE;

			/* Klackons become faster */
			p_ptr->pspeed += (p_ptr->lev) / 10;
			break;
		case RACE_KOBOLD:
			p_ptr->resist_pois = TRUE;
			break;
		case RACE_NIBELUNG:
			p_ptr->resist_disen = TRUE;
			p_ptr->resist_dark = TRUE;
			break;
		case RACE_DARK_ELF:
			p_ptr->resist_dark = TRUE;
			if (p_ptr->lev > 19) p_ptr->see_inv = TRUE;
			break;
		case RACE_DRACONIAN:
			p_ptr->ffall = TRUE;
			if (p_ptr->lev >  4) p_ptr->resist_fire = TRUE;
			if (p_ptr->lev >  9) p_ptr->resist_cold = TRUE;
			if (p_ptr->lev > 14) p_ptr->resist_acid = TRUE;
			if (p_ptr->lev > 19) p_ptr->resist_elec = TRUE;
			if (p_ptr->lev > 34) p_ptr->resist_pois = TRUE;
			break;
		case RACE_MIND_FLAYER:
			p_ptr->sustain_int = TRUE;
			p_ptr->sustain_wis = TRUE;
			if (p_ptr->lev > 14) p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 29) p_ptr->telepathy = TRUE;
			break;
		case RACE_IMP:
			p_ptr->resist_fire = TRUE;
			if (p_ptr->lev > 9) p_ptr->see_inv = TRUE;
			break;
		case RACE_GOLEM:
			p_ptr->slow_digest = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			if (p_ptr->lev > 34) p_ptr->hold_life = TRUE;
			break;
		case RACE_SKELETON:
			p_ptr->resist_shard = TRUE;
			p_ptr->hold_life = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			if (p_ptr->lev > 9) p_ptr->resist_cold = TRUE;
			break;
		case RACE_ZOMBIE:
			p_ptr->resist_neth = TRUE;
			p_ptr->hold_life = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->slow_digest = TRUE;
			if (p_ptr->lev > 4) p_ptr->resist_cold = TRUE;
			break;
		case RACE_VAMPIRE:
			p_ptr->resist_dark = TRUE;
			p_ptr->hold_life = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;
			break;
		case RACE_SPECTRE:
			p_ptr->ffall = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_neth = TRUE;
			p_ptr->hold_life = TRUE;
			p_ptr->see_inv = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->slow_digest = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->pass_wall = TRUE;
			if (p_ptr->lev > 34) p_ptr->telepathy = TRUE;
			break;
		case RACE_SPRITE:
			p_ptr->ffall = TRUE;
			p_ptr->resist_lite = TRUE;

			/* Sprites become faster */
			p_ptr->pspeed += (p_ptr->lev) / 10;
			break;
		case RACE_BEASTMAN:
			p_ptr->resist_conf  = TRUE;
			p_ptr->resist_sound = TRUE;
			break;
		case RACE_ENT:
			/* Ents dig like maniacs, but only with their hands. */
			if (!inventory[INVEN_RARM].k_idx) 
				p_ptr->skill_dig += p_ptr->lev * 10;
			/* Ents get tougher and stronger as they age, but lose dexterity. */
			if (p_ptr->lev > 25) p_ptr->stat_add[A_STR]++;
			if (p_ptr->lev > 40) p_ptr->stat_add[A_STR]++;
			if (p_ptr->lev > 45) p_ptr->stat_add[A_STR]++;

			if (p_ptr->lev > 25) p_ptr->stat_add[A_DEX]--;
			if (p_ptr->lev > 40) p_ptr->stat_add[A_DEX]--;
			if (p_ptr->lev > 45) p_ptr->stat_add[A_DEX]--;

			if (p_ptr->lev > 25) p_ptr->stat_add[A_CON]++;
			if (p_ptr->lev > 40) p_ptr->stat_add[A_CON]++;
			if (p_ptr->lev > 45) p_ptr->stat_add[A_CON]++;
			break;
		case RACE_ANGEL:
			p_ptr->ffall = TRUE;
			p_ptr->see_inv = TRUE;
			break;
		case RACE_DEMON:
			p_ptr->resist_fire  = TRUE;
			p_ptr->resist_neth  = TRUE;
			p_ptr->hold_life = TRUE;
			if (p_ptr->lev > 9)
				p_ptr->see_inv = TRUE;
			if (p_ptr->lev > 44)
			{
				p_ptr->oppose_fire = 1;
				p_ptr->redraw |= PR_STATUS;
			}
			break;
		case RACE_DUNADAN:
			p_ptr->sustain_con = TRUE;
			break;
		case RACE_S_FAIRY:
			p_ptr->ffall = TRUE;
			break;
		case RACE_KUTA:
			p_ptr->resist_conf = TRUE;
			break;
		case RACE_ANDROID:
			p_ptr->slow_digest = TRUE;
			p_ptr->free_act = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->hold_life = TRUE;
			break;
		default:
			/* Do nothing */
			;
	}
	}

	if (p_ptr->ult_res || (p_ptr->special_defense & KATA_MUSOU))
	{
		p_ptr->see_inv = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->slow_digest = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->ffall = TRUE;
		p_ptr->hold_life = TRUE;
		p_ptr->telepathy = TRUE;
		p_ptr->lite = TRUE;
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_int = TRUE;
		p_ptr->sustain_wis = TRUE;
		p_ptr->sustain_con = TRUE;
		p_ptr->sustain_dex = TRUE;
		p_ptr->sustain_chr = TRUE;
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_elec = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_pois = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_sound = TRUE;
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_disen = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_nexus = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->reflect = TRUE;
		p_ptr->sh_fire = TRUE;
		p_ptr->sh_elec = TRUE;
		p_ptr->sh_cold = TRUE;
		p_ptr->to_a += 100;
		p_ptr->dis_to_a += 100;
	}
	if (p_ptr->tim_res_nether)
	{
		p_ptr->resist_neth = TRUE;
	}
	if (p_ptr->tim_sh_fire)
	{
		p_ptr->sh_fire = TRUE;
	}
	if (p_ptr->tim_res_time)
	{
		p_ptr->resist_time = TRUE;
	}

	/* Sexy Gal */
	if (p_ptr->pseikaku == SEIKAKU_SEXY) p_ptr->aggravate = TRUE;

	/* Lucky man */
	if (p_ptr->pseikaku == SEIKAKU_LUCKY) p_ptr->muta3 |= MUT3_GOOD_LUCK;

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_conf  = TRUE;
		p_ptr->hold_life = TRUE;
		if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;

		if ((p_ptr->prace != RACE_KLACKON) && (p_ptr->prace != RACE_SPRITE))
			/* Munchkin become faster */
			p_ptr->pspeed += (p_ptr->lev) / 10 + 5;
	}

	if (p_ptr->riding)
	{
		if (!(r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_PASS_WALL))
			p_ptr->pass_wall = FALSE;
		if (r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_KILL_WALL)
			p_ptr->pass_wall = TRUE;
	}
	if (music_singing(MUSIC_WALL)) p_ptr->kill_wall = TRUE;

	if (p_ptr->kill_wall) p_ptr->pass_wall = TRUE;

	/* Hack -- apply racial/class stat maxes */
	/* Apply the racial modifiers */
	for (i = 0; i < 6; i++)
	{
		/* Modify the stats for "race" */
		p_ptr->stat_add[i] += (tmp_rp_ptr->r_adj[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i]);
	}


	/* I'm adding the mutations here for the lack of a better place... */
	if (p_ptr->muta3)
	{
		/* Hyper Strength */
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
			p_ptr->stat_add[A_STR] += 4;
		}

		/* Puny */
		if (p_ptr->muta3 & MUT3_PUNY)
		{
			p_ptr->stat_add[A_STR] -= 4;
		}

		/* Living computer */
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
			p_ptr->stat_add[A_INT] += 4;
			p_ptr->stat_add[A_WIS] += 4;
		}

		/* Moronic */
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
			p_ptr->stat_add[A_INT] -= 4;
			p_ptr->stat_add[A_WIS] -= 4;
		}

		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
			p_ptr->stat_add[A_CON] += 4;
		}

		if (p_ptr->muta3 & MUT3_XTRA_FAT)
		{
			p_ptr->stat_add[A_CON] += 2;
			p_ptr->pspeed -= 2;
		}

		if (p_ptr->muta3 & MUT3_ALBINO)
		{
			p_ptr->stat_add[A_CON] -= 4;
		}

		if (p_ptr->muta3 & MUT3_FLESH_ROT)
		{
			p_ptr->stat_add[A_CON] -= 2;
			p_ptr->stat_add[A_CHR] -= 1;
			p_ptr->regenerate = FALSE;
			/* Cancel innate regeneration */
		}

		if (p_ptr->muta3 & MUT3_SILLY_VOI)
		{
			p_ptr->stat_add[A_CHR] -= 4;
		}

		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
			p_ptr->stat_add[A_CHR] -= 1;
		}

		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
			p_ptr->skill_fos += 15;
			p_ptr->skill_srh += 15;
		}

		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
			p_ptr->skill_sav += (15 + (p_ptr->lev / 5));
		}

		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
			p_ptr->skill_stl -= 3;
		}

		if (p_ptr->muta3 & MUT3_INFRAVIS)
		{
			p_ptr->see_infra += 3;
		}

		if (p_ptr->muta3 & MUT3_XTRA_LEGS)
		{
			p_ptr->pspeed += 3;
		}

		if (p_ptr->muta3 & MUT3_SHORT_LEG)
		{
			p_ptr->pspeed -= 3;
		}

		if (p_ptr->muta3 & MUT3_ELEC_TOUC)
		{
			p_ptr->sh_elec = TRUE;
		}

		if (p_ptr->muta3 & MUT3_FIRE_BODY)
		{
			p_ptr->sh_fire = TRUE;
			p_ptr->lite = TRUE;
		}

		if (p_ptr->muta3 & MUT3_WART_SKIN)
		{
			p_ptr->stat_add[A_CHR] -= 2;
			p_ptr->to_a += 5;
			p_ptr->dis_to_a += 5;
		}

		if (p_ptr->muta3 & MUT3_SCALES)
		{
			p_ptr->stat_add[A_CHR] -= 1;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
		}

		if (p_ptr->muta3 & MUT3_IRON_SKIN)
		{
			p_ptr->stat_add[A_DEX] -= 1;
			p_ptr->to_a += 25;
			p_ptr->dis_to_a += 25;
		}

		if (p_ptr->muta3 & MUT3_WINGS)
		{
			p_ptr->ffall = TRUE;
		}

		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			p_ptr->resist_fear = TRUE;
		}

		if (p_ptr->muta3 & MUT3_REGEN)
		{
			p_ptr->regenerate = TRUE;
		}

		if (p_ptr->muta3 & MUT3_ESP)
		{
			p_ptr->telepathy = TRUE;
		}

		if (p_ptr->muta3 & MUT3_LIMBER)
		{
			p_ptr->stat_add[A_DEX] += 3;
		}

		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
			p_ptr->stat_add[A_DEX] -= 3;
		}

		if (p_ptr->muta3 & MUT3_MOTION)
		{
			p_ptr->free_act = TRUE;
			p_ptr->skill_stl += 1;
		}

		if (p_ptr->muta3 & MUT3_ILL_NORM)
		{
			p_ptr->stat_add[A_CHR] = 0;
		}
	}

	if (p_ptr->tsuyoshi)
	{
		p_ptr->stat_add[A_STR] += 4;
		p_ptr->stat_add[A_CON] += 4;
	}

	/* Scan the usable inventory */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		int bonus_to_h, bonus_to_d;
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval == TV_CAPTURE) continue;

		/* Extract the item flags */
		object_flags(o_ptr, &f1, &f2, &f3);

		if (((o_ptr->tval == TV_RING) && (o_ptr->sval == SV_RING_WARNING)) || (o_ptr->name1 == ART_BARAHIR)) p_ptr->warning = TRUE;

		/* Affect stats */
		if (f1 & (TR1_STR)) p_ptr->stat_add[A_STR] += o_ptr->pval;
		if (f1 & (TR1_INT)) p_ptr->stat_add[A_INT] += o_ptr->pval;
		if (f1 & (TR1_WIS)) p_ptr->stat_add[A_WIS] += o_ptr->pval;
		if (f1 & (TR1_DEX)) p_ptr->stat_add[A_DEX] += o_ptr->pval;
		if (f1 & (TR1_CON)) p_ptr->stat_add[A_CON] += o_ptr->pval;
		if (f1 & (TR1_CHR)) p_ptr->stat_add[A_CHR] += o_ptr->pval;

		if (f1 & (TR1_MAGIC_MASTERY))    p_ptr->skill_dev += 8*o_ptr->pval;

		/* Affect stealth */
		if (f1 & (TR1_STEALTH)) p_ptr->skill_stl += o_ptr->pval;

		/* Affect searching ability (factor of five) */
		if (f1 & (TR1_SEARCH)) p_ptr->skill_srh += (o_ptr->pval * 5);

		/* Affect searching frequency (factor of five) */
		if (f1 & (TR1_SEARCH)) p_ptr->skill_fos += (o_ptr->pval * 5);

		/* Affect infravision */
		if (f1 & (TR1_INFRA)) p_ptr->see_infra += o_ptr->pval;

		/* Affect digging (factor of 20) */
		if (f1 & (TR1_TUNNEL)) p_ptr->skill_dig += (o_ptr->pval * 20);

		/* Affect speed */
		if (f1 & (TR1_SPEED)) p_ptr->pspeed += o_ptr->pval;

		/* Affect blows */
		if (f1 & (TR1_BLOWS))
		{
			if((i == INVEN_RARM || i == INVEN_RIGHT) && !p_ptr->ryoute) extra_blows[0] += o_ptr->pval;
			else if((i == INVEN_LARM || i == INVEN_LEFT) && !p_ptr->ryoute) extra_blows[1] += o_ptr->pval;
			else {extra_blows[0] += o_ptr->pval; extra_blows[1] += o_ptr->pval;}
		}

		/* Hack -- cause earthquakes */
		if (f1 & (TR1_IMPACT)) p_ptr->impact[(i == INVEN_RARM) ? 0 : 1] = TRUE;

		/* Boost shots */
		if (f3 & (TR3_XTRA_SHOTS)) extra_shots++;

		/* Various flags */
		if (f3 & (TR3_DEC_MANA))    p_ptr->dec_mana = TRUE;
		if (f3 & (TR3_AGGRAVATE))   p_ptr->aggravate = TRUE;
		if (f3 & (TR3_TELEPORT))    p_ptr->teleport = TRUE;
		if (f3 & (TR3_DRAIN_EXP))   p_ptr->exp_drain = TRUE;
		if (f3 & (TR3_BLESSED))     p_ptr->bless_blade = TRUE;
		if (f3 & (TR3_XTRA_MIGHT))  p_ptr->xtra_might = TRUE;
		if (f3 & (TR3_SLOW_DIGEST)) p_ptr->slow_digest = TRUE;
		if (f3 & (TR3_REGEN))       p_ptr->regenerate = TRUE;
		if (f3 & (TR3_TELEPATHY))   p_ptr->telepathy = TRUE;
		if (f3 & (TR3_SEE_INVIS))   p_ptr->see_inv = TRUE;
		if (f3 & (TR3_FEATHER))     p_ptr->ffall = TRUE;
		if (f2 & (TR2_FREE_ACT))    p_ptr->free_act = TRUE;
		if (f2 & (TR2_HOLD_LIFE))   p_ptr->hold_life = TRUE;

		/* Immunity flags */
		if (f2 & (TR2_IM_FIRE)) p_ptr->immune_fire = TRUE;
		if (f2 & (TR2_IM_ACID)) p_ptr->immune_acid = TRUE;
		if (f2 & (TR2_IM_COLD)) p_ptr->immune_cold = TRUE;
		if (f2 & (TR2_IM_ELEC)) p_ptr->immune_elec = TRUE;

		/* Resistance flags */
		if (f2 & (TR2_RES_ACID))   p_ptr->resist_acid = TRUE;
		if (f2 & (TR2_RES_ELEC))   p_ptr->resist_elec = TRUE;
		if (f2 & (TR2_RES_FIRE))   p_ptr->resist_fire = TRUE;
		if (f2 & (TR2_RES_COLD))   p_ptr->resist_cold = TRUE;
		if (f2 & (TR2_RES_POIS))   p_ptr->resist_pois = TRUE;
		if (f2 & (TR2_RES_FEAR))   p_ptr->resist_fear = TRUE;
		if (f2 & (TR2_RES_CONF))   p_ptr->resist_conf = TRUE;
		if (f2 & (TR2_RES_SOUND))  p_ptr->resist_sound = TRUE;
		if (f2 & (TR2_RES_LITE))   p_ptr->resist_lite = TRUE;
		if (f2 & (TR2_RES_DARK))   p_ptr->resist_dark = TRUE;
		if (f2 & (TR2_RES_CHAOS))  p_ptr->resist_chaos = TRUE;
		if (f2 & (TR2_RES_DISEN))  p_ptr->resist_disen = TRUE;
		if (f2 & (TR2_RES_SHARDS)) p_ptr->resist_shard = TRUE;
		if (f2 & (TR2_RES_NEXUS))  p_ptr->resist_nexus = TRUE;
		if (f2 & (TR2_RES_BLIND))  p_ptr->resist_blind = TRUE;
		if (f2 & (TR2_RES_NETHER)) p_ptr->resist_neth = TRUE;

		if (f2 & (TR2_REFLECT))  p_ptr->reflect = TRUE;
		if (f3 & (TR3_SH_FIRE))  p_ptr->sh_fire = TRUE;
		if (f3 & (TR3_SH_ELEC))  p_ptr->sh_elec = TRUE;
		if (f3 & (TR3_SH_COLD))  p_ptr->sh_cold = TRUE;
		if (f3 & (TR3_NO_MAGIC)) p_ptr->anti_magic = TRUE;
		if (f3 & (TR3_NO_TELE))  p_ptr->anti_tele = TRUE;

		/* Sustain flags */
		if (f2 & (TR2_SUST_STR)) p_ptr->sustain_str = TRUE;
		if (f2 & (TR2_SUST_INT)) p_ptr->sustain_int = TRUE;
		if (f2 & (TR2_SUST_WIS)) p_ptr->sustain_wis = TRUE;
		if (f2 & (TR2_SUST_DEX)) p_ptr->sustain_dex = TRUE;
		if (f2 & (TR2_SUST_CON)) p_ptr->sustain_con = TRUE;
		if (f2 & (TR2_SUST_CHR)) p_ptr->sustain_chr = TRUE;

		if (o_ptr->name2 == EGO_YOIYAMI) yoiyami = TRUE;
		if (o_ptr->name2 == EGO_2HAND) easy_2hand = TRUE;
		if (o_ptr->name2 == EGO_RING_RES_TIME) p_ptr->resist_time = TRUE;
		if (o_ptr->name2 == EGO_RING_THROW) p_ptr->mighty_throw = TRUE;
		if (o_ptr->name2 == EGO_RING_WIZARD) p_ptr->easy_spell = TRUE;
		if (o_ptr->name2 == EGO_AMU_FOOL) p_ptr->heavy_spell = TRUE;
		if (o_ptr->name2 == EGO_AMU_NAIVETY) down_saving = TRUE;

		/* Modify the base armor class */
		p_ptr->ac += o_ptr->ac;

		/* The base armor class is always known */
		p_ptr->dis_ac += o_ptr->ac;

		/* Apply the bonuses to armor class */
		p_ptr->to_a += o_ptr->to_a;

		/* Apply the mental bonuses to armor class, if known */
		if (object_known_p(o_ptr)) p_ptr->dis_to_a += o_ptr->to_a;

		/* Hack -- do not apply "weapon" bonuses */
		if (i == INVEN_RARM && buki_motteruka(i)) continue;
		if (i == INVEN_LARM && buki_motteruka(i)) continue;

		/* Hack -- do not apply "bow" bonuses */
		if (i == INVEN_BOW) continue;

		if (p_ptr->pclass == CLASS_NINJA)
		{
			if (o_ptr->to_h > 0) bonus_to_h = (o_ptr->to_h+1)/2;
			else bonus_to_h = o_ptr->to_h;
			if (o_ptr->to_d > 0) bonus_to_d = (o_ptr->to_d+1)/2;
			else bonus_to_d = o_ptr->to_d;
		}
		else
		{
			bonus_to_h = o_ptr->to_h;
			bonus_to_d = o_ptr->to_d;
		}

		if ((i == INVEN_LEFT || i == INVEN_RIGHT) && !p_ptr->ryoute)
		{
			p_ptr->to_h[i-INVEN_RIGHT] += bonus_to_h;
			p_ptr->to_h_b += bonus_to_h;
			p_ptr->to_h_m += bonus_to_h;
			p_ptr->to_d[i-INVEN_RIGHT] += bonus_to_d;
			p_ptr->to_d_m += bonus_to_d;

			if (object_known_p(o_ptr)) p_ptr->dis_to_h[i-INVEN_RIGHT] += bonus_to_h;
			if (object_known_p(o_ptr)) p_ptr->dis_to_h_b += bonus_to_h;
			if (object_known_p(o_ptr)) p_ptr->dis_to_d[i-INVEN_RIGHT] += bonus_to_d;
		}
		else if (p_ptr->migite && p_ptr->hidarite)
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->to_h[0] += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
			p_ptr->to_h[1] += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
			p_ptr->to_h_b  += bonus_to_h;
			p_ptr->to_h_m  += bonus_to_h;
			p_ptr->to_d[0] += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
			p_ptr->to_d[1] += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;
			p_ptr->to_d_m  += bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_known_p(o_ptr)) p_ptr->dis_to_h[0] += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
			if (object_known_p(o_ptr)) p_ptr->dis_to_h[1] += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
			if (object_known_p(o_ptr)) p_ptr->dis_to_h_b  += bonus_to_h;
			if (object_known_p(o_ptr)) p_ptr->dis_to_d[0] += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
			if (object_known_p(o_ptr)) p_ptr->dis_to_d[1] += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;
		}
		else
		{
			/* Apply the bonuses to hit/damage */
			if(i != INVEN_LEFT || p_ptr->ryoute) p_ptr->to_h[0] += bonus_to_h;
			if(i != INVEN_RIGHT) p_ptr->to_h[1] += bonus_to_h;
			p_ptr->to_h_b  += bonus_to_h;
			p_ptr->to_h_m  += bonus_to_h;
			if(i != INVEN_LEFT || p_ptr->ryoute) p_ptr->to_d[0] += bonus_to_d;
			if(i != INVEN_RIGHT) p_ptr->to_d[1] += bonus_to_d;
			p_ptr->to_d_m  += bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if ((i != INVEN_LEFT || p_ptr->ryoute) && object_known_p(o_ptr)) p_ptr->dis_to_h[0] += bonus_to_h;
			if ((i != INVEN_RIGHT) && object_known_p(o_ptr)) p_ptr->dis_to_h[1] += bonus_to_h;
			if (object_known_p(o_ptr)) p_ptr->dis_to_h_b  += bonus_to_h;
			if ((i != INVEN_LEFT || p_ptr->ryoute) && object_known_p(o_ptr)) p_ptr->dis_to_d[0] += bonus_to_d;
			if ((i != INVEN_RIGHT) && object_known_p(o_ptr)) p_ptr->dis_to_d[1] += bonus_to_d;
		}
	}

	/* Monks get extra ac for armour _not worn_ */
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER)) && !heavy_armor())
	{
		if (!(inventory[INVEN_BODY].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev * 3) / 2;
			p_ptr->dis_to_a += (p_ptr->lev * 3) / 2;
		}
		if (!(inventory[INVEN_OUTER].k_idx) && (p_ptr->lev > 15))
		{
			p_ptr->to_a += ((p_ptr->lev - 13) / 3);
			p_ptr->dis_to_a += ((p_ptr->lev - 13) / 3);
		}
		if (!(inventory[INVEN_LARM].k_idx) && (p_ptr->lev > 10))
		{
			p_ptr->to_a += ((p_ptr->lev - 8) / 3);
			p_ptr->dis_to_a += ((p_ptr->lev - 8) / 3);
		}
		if (!(inventory[INVEN_HEAD].k_idx) && (p_ptr->lev > 4))
		{
			p_ptr->to_a += (p_ptr->lev - 2) / 3;
			p_ptr->dis_to_a += (p_ptr->lev -2) / 3;
		}
		if (!(inventory[INVEN_HANDS].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev / 2);
			p_ptr->dis_to_a += (p_ptr->lev / 2);
		}
		if (!(inventory[INVEN_FEET].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev / 3);
			p_ptr->dis_to_a += (p_ptr->lev / 3);
		}
		if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_DEX] += 2;
			p_ptr->stat_add[A_CON] -= 3;
		}
		else if (p_ptr->special_defense & KAMAE_SEIRYU)
		{
		}
		else if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->stat_add[A_INT] -= 1;
			p_ptr->stat_add[A_WIS] -= 1;
			p_ptr->stat_add[A_DEX] -= 2;
			p_ptr->stat_add[A_CON] += 3;
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			p_ptr->stat_add[A_STR] -= 2;
			p_ptr->stat_add[A_INT] += 1;
			p_ptr->stat_add[A_WIS] += 1;
			p_ptr->stat_add[A_DEX] += 2;
			p_ptr->stat_add[A_CON] -= 2;
		}
	}

	if (p_ptr->special_defense & KATA_KOUKIJIN)
	{
		for (i = 0; i < 6; i++)
			p_ptr->stat_add[i] += 5;
		p_ptr->ac -= 50;
	}

	/* Hack -- aura of fire also provides light */
	if (p_ptr->sh_fire) p_ptr->lite = TRUE;

	/* Golems also get an intrinsic AC bonus */
	if ((p_ptr->prace == RACE_GOLEM) || (p_ptr->prace == RACE_ANDROID))
	{
		p_ptr->to_a += 10 + (p_ptr->lev * 2 / 5);
		p_ptr->dis_to_a += 10 + (p_ptr->lev * 2 / 5);
	}

	/* Calculate stats */
	for (i = 0; i < 6; i++)
	{
		int top, use, ind;

		/* Extract the new "stat_use" value for the stat */
		top = modify_stat_value(p_ptr->stat_max[i], p_ptr->stat_add[i]);

		/* Notice changes */
		if (p_ptr->stat_top[i] != top)
		{
			/* Save the new value */
			p_ptr->stat_top[i] = top;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Extract the new "stat_use" value for the stat */
		use = modify_stat_value(p_ptr->stat_cur[i], p_ptr->stat_add[i]);

		if ((i == A_CHR) && (p_ptr->muta3 & MUT3_ILL_NORM))
		{
			/* 10 to 18/90 charisma, guaranteed, based on level */
			if (use < 8 + 2 * p_ptr->lev)
			{
				use = 8 + 2 * p_ptr->lev;
			}
		}

		/* Notice changes */
		if (p_ptr->stat_use[i] != use)
		{
			/* Save the new value */
			p_ptr->stat_use[i] = use;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Values: 3, 4, ..., 17 */
		if (use <= 18) ind = (use - 3);

		/* Ranges: 18/00-18/09, ..., 18/210-18/219 */
		else if (use <= 18+219) ind = (15 + (use - 18) / 10);

		/* Range: 18/220+ */
		else ind = (37);

		/* Notice changes */
		if (p_ptr->stat_ind[i] != ind)
		{
			/* Save the new index */
			p_ptr->stat_ind[i] = ind;

			/* Change in CON affects Hitpoints */
			if (i == A_CON)
			{
				p_ptr->update |= (PU_HP);
			}

			/* Change in INT may affect Mana/Spells */
			else if (i == A_INT)
			{
				if (mp_ptr->spell_stat == A_INT)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_WIS)
			{
				if (mp_ptr->spell_stat == A_WIS)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_CHR)
			{
				if (mp_ptr->spell_stat == A_CHR)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}
	}


	/* Apply temporary "stun" */
	if (p_ptr->stun > 50)
	{
		p_ptr->to_h[0] -= 20;
		p_ptr->to_h[1] -= 20;
		p_ptr->to_h_b  -= 20;
		p_ptr->to_h_m  -= 20;
		p_ptr->dis_to_h[0] -= 20;
		p_ptr->dis_to_h[1] -= 20;
		p_ptr->dis_to_h_b  -= 20;
		p_ptr->to_d[0] -= 20;
		p_ptr->to_d[1] -= 20;
		p_ptr->to_d_m -= 20;
		p_ptr->dis_to_d[0] -= 20;
		p_ptr->dis_to_d[1] -= 20;
	}
	else if (p_ptr->stun)
	{
		p_ptr->to_h[0] -= 5;
		p_ptr->to_h[1] -= 5;
		p_ptr->to_h_b -= 5;
		p_ptr->to_h_m -= 5;
		p_ptr->dis_to_h[0] -= 5;
		p_ptr->dis_to_h[1] -= 5;
		p_ptr->dis_to_h_b -= 5;
		p_ptr->to_d[0] -= 5;
		p_ptr->to_d[1] -= 5;
		p_ptr->to_d_m -= 5;
		p_ptr->dis_to_d[0] -= 5;
		p_ptr->dis_to_d[1] -= 5;
	}

	/* wraith_form */
	if (p_ptr->wraith_form)
	{
		p_ptr->reflect = TRUE;
	}

	/* Temporary blessing */
	if (p_ptr->blessed || music_singing(MUSIC_BLESS))
	{
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
		p_ptr->to_h[0] += 10;
		p_ptr->to_h[1] += 10;
		p_ptr->to_h_b  += 10;
		p_ptr->to_h_m  += 10;
		p_ptr->dis_to_h[0] += 10;
		p_ptr->dis_to_h[1] += 10;
		p_ptr->dis_to_h_b += 10;
	}

	/* Temporary shield */
	if (p_ptr->tsubureru || p_ptr->shield || p_ptr->magicdef)
	{
		p_ptr->to_a += 50;
		p_ptr->dis_to_a += 50;
	}
	if (p_ptr->magicdef)
	{
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->reflect = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->ffall = TRUE;
	}

	/* Temporary "Hero" */
	if (p_ptr->hero || music_singing(MUSIC_HERO) || music_singing(MUSIC_SHERO))
	{
		p_ptr->to_h[0] += 12;
		p_ptr->to_h[1] += 12;
		p_ptr->to_h_b  += 12;
		p_ptr->to_h_m  += 12;
		p_ptr->dis_to_h[0] += 12;
		p_ptr->dis_to_h[1] += 12;
		p_ptr->dis_to_h_b  += 12;
	}

	/* Temporary "Beserk" */
	if (p_ptr->shero)
	{
		p_ptr->to_h[0] += 12;
		p_ptr->to_h[1] += 12;
		p_ptr->to_h_b  -= 12;
		p_ptr->to_h_m  += 12;
		p_ptr->to_d[0] += 3+(p_ptr->lev/5);
		p_ptr->to_d[1] += 3+(p_ptr->lev/5);
		p_ptr->to_d_m  += 3+(p_ptr->lev/5);
		p_ptr->dis_to_h[0] += 12;
		p_ptr->dis_to_h[1] += 12;
		p_ptr->dis_to_h_b  -= 12;
		p_ptr->dis_to_d[0] += 3+(p_ptr->lev/5);
		p_ptr->dis_to_d[1] += 3+(p_ptr->lev/5);
		p_ptr->to_a -= 10;
		p_ptr->dis_to_a -= 10;
		p_ptr->skill_stl -= 7;
		p_ptr->skill_dev -= 20;
		p_ptr->skill_sav -= 30;
		p_ptr->skill_srh -= 15;
		p_ptr->skill_fos -= 15;
		p_ptr->skill_tht -= 20;
		p_ptr->skill_dig += 30;
	}

/* Temporary "fast" */
	if (p_ptr->fast || music_singing(MUSIC_SPEED) || music_singing(MUSIC_SHERO))
	{
		p_ptr->pspeed += 10;
	}

	/* Temporary "slow" */
	if (p_ptr->slow)
	{
		p_ptr->pspeed -= 10;
	}

	/* Temporary "telepathy" */
	if (p_ptr->tim_esp || music_singing(MUSIC_MIND))
	{
		p_ptr->telepathy = TRUE;
	}

	if (p_ptr->ele_immune)
	{
		if (p_ptr->special_defense & DEFENSE_ACID)
			p_ptr->immune_acid = TRUE;
		else if (p_ptr->special_defense & DEFENSE_ELEC)
			p_ptr->immune_elec = TRUE;
		else if (p_ptr->special_defense & DEFENSE_FIRE)
			p_ptr->immune_fire = TRUE;
		else if (p_ptr->special_defense & DEFENSE_COLD)
			p_ptr->immune_cold = TRUE;
	}

	/* Temporary see invisible */
	if (p_ptr->tim_invis)
	{
		p_ptr->see_inv = TRUE;
	}

	/* Temporary infravision boost */
	if (p_ptr->tim_infra)
	{
		p_ptr->see_infra+=3;
	}

	/* Temporary regeneration boost */
	if (p_ptr->tim_regen)
	{
		p_ptr->regenerate = TRUE;
	}

	/* Temporary levitation */
	if (p_ptr->tim_ffall)
	{
		p_ptr->ffall = TRUE;
	}

	/* Hack -- Hero/Shero -> Res fear */
	if (p_ptr->hero || p_ptr->shero || music_singing(MUSIC_HERO) || music_singing(MUSIC_SHERO))
	{
		p_ptr->resist_fear = TRUE;
	}


	/* Hack -- Telepathy Change */
	if (p_ptr->telepathy != old_telepathy)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Hack -- See Invis Change */
	if (p_ptr->see_inv != old_see_inv)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Bloating slows the player down (a little) */
	if (p_ptr->food >= PY_FOOD_MAX) p_ptr->pspeed -= 10;

	if (p_ptr->special_defense & KAMAE_SUZAKU) p_ptr->pspeed += 10;

	if (!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
	{
		p_ptr->to_h[0] += (skill_exp[GINOU_SUDE]-4000)/200;
		p_ptr->dis_to_h[0] += (skill_exp[GINOU_SUDE]-4000)/200;
	}

	if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
	{
		int penalty1, penalty2;
		penalty1 = ((100-skill_exp[GINOU_NITOURYU]/160) - (130-inventory[INVEN_RARM].weight)/8);
		penalty2 = ((100-skill_exp[GINOU_NITOURYU]/160) - (130-inventory[INVEN_LARM].weight)/8);
		if ((inventory[INVEN_RARM].name1 == ART_QUICKTHORN) && (inventory[INVEN_LARM].name1 == ART_TINYTHORN))
		{
			penalty1 = penalty1 / 2 - 5;
			penalty2 = penalty2 / 2 - 5;
			p_ptr->pspeed += 7;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
		}
		if (easy_2hand)
		{
			if (penalty1 > 0) penalty1 /= 2;
			if (penalty2 > 0) penalty2 /= 2;
		}
		else if ((inventory[INVEN_LARM].tval == TV_SWORD) && ((inventory[INVEN_LARM].sval == SV_MAIN_GAUCHE) || (inventory[INVEN_LARM].sval == SV_WAKIZASHI)))
		{
			penalty1 = MAX(0, penalty1 - 10);
			penalty2 = MAX(0, penalty2 - 10);
		}
		if ((inventory[INVEN_RARM].name1 == ART_MUSASI_KATANA) && (inventory[INVEN_LARM].name1 == ART_MUSASI_WAKIZASI))
		{
			penalty1 = MIN(0, penalty1);
			penalty2 = MIN(0, penalty2);
		}
		if (inventory[INVEN_RARM].tval == TV_POLEARM) penalty1 += 10;
		if (inventory[INVEN_LARM].tval == TV_POLEARM) penalty2 += 10;
		p_ptr->to_h[0] -= penalty1;
		p_ptr->to_h[1] -= penalty2;
		p_ptr->dis_to_h[0] -= penalty1;
		p_ptr->dis_to_h[1] -= penalty2;
	}

	/* Extract the current weight (in tenth pounds) */
	j = p_ptr->total_weight;

	/* Extract the "weight limit" (in tenth pounds) */
	i = weight_limit();

	if (p_ptr->riding)
	{
		int speed = m_list[p_ptr->riding].mspeed;
		if (m_list[p_ptr->riding].mspeed > 110)
		{
			p_ptr->pspeed = 110 + ((speed-110)*(skill_exp[GINOU_RIDING]*3 + p_ptr->lev*160L - 10000L)/(22000L));
			if (p_ptr->pspeed < 110) p_ptr->pspeed = 110;
		}
		else
		{
			p_ptr->pspeed = speed;
		}
		if (m_list[p_ptr->riding].fast) p_ptr->pspeed += 10;
		if (m_list[p_ptr->riding].slow) p_ptr->pspeed -= 10;
		if (r_info[m_list[p_ptr->riding].r_idx].flags7 & RF7_CAN_FLY) p_ptr->ffall = TRUE;
		if (r_info[m_list[p_ptr->riding].r_idx].flags7 & (RF7_CAN_SWIM | RF7_AQUATIC)) p_ptr->can_swim = TRUE;

		if (skill_exp[GINOU_RIDING] < 2000) j += (p_ptr->wt*3*(2000 - skill_exp[GINOU_RIDING]))/2000;

		i = 3000 + r_info[m_list[p_ptr->riding].r_idx].level * 50;
	}

	/* XXX XXX XXX Apply "encumbrance" from weight */
	if (j > i/2) p_ptr->pspeed -= ((j - (i/2)) / (i / 10));

	/* Searching slows the player down */
	if (p_ptr->action == ACTION_SEARCH) p_ptr->pspeed -= 10;

	/* Actual Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_d[0] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_d[1] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_d_m  += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h[0] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h[1] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h_b  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h_m  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h[0] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h[1] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h_b  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h_m  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);

	/* Displayed Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->dis_to_a += ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_d[0] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_d[1] += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h[0] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h[1] += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h_b  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h[0] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h[1] += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h_b  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);


	/* Obtain the "hold" value */
	hold = adj_str_hold[p_ptr->stat_ind[A_STR]];


	/* Examine the "current bow" */
	o_ptr = &inventory[INVEN_BOW];


	/* Assume not heavy */
	p_ptr->heavy_shoot = FALSE;

	/* It is hard to carholdry a heavy bow */
	if (hold < o_ptr->weight / 10)
	{
		/* Hard to wield a heavy bow */
		p_ptr->to_h_b  += 2 * (hold - o_ptr->weight / 10);
		p_ptr->dis_to_h_b  += 2 * (hold - o_ptr->weight / 10);

		/* Heavy Bow */
		p_ptr->heavy_shoot = TRUE;
	}


	/* Compute "extra shots" if needed */
	if (o_ptr->k_idx)
	{
		/* Analyze the launcher */
		switch (o_ptr->sval)
		{
			case SV_SLING:
			{
				p_ptr->tval_ammo = TV_SHOT;
				break;
			}

			case SV_SHORT_BOW:
			case SV_LONG_BOW:
			case SV_NAMAKE_BOW:
			{
				p_ptr->tval_ammo = TV_ARROW;
				break;
			}

			case SV_LIGHT_XBOW:
			case SV_HEAVY_XBOW:
			{
				p_ptr->tval_ammo = TV_BOLT;
				break;
			}
			case SV_CRIMSON:
			{
				p_ptr->tval_ammo = TV_NO_AMMO;
				break;
			}
		}

		/* Apply special flags */
		if (o_ptr->k_idx && !p_ptr->heavy_shoot)
		{
			/* Extra shots */
			p_ptr->num_fire += (extra_shots * 100);

			/* Hack -- Rangers love Bows */
			if ((p_ptr->pclass == CLASS_RANGER) &&
			    (p_ptr->tval_ammo == TV_ARROW))
			{
				p_ptr->num_fire += (p_ptr->lev * 4);
			}

			if ((p_ptr->pclass == CLASS_CAVALRY) &&
			    (p_ptr->tval_ammo == TV_ARROW))
			{
				p_ptr->num_fire += (p_ptr->lev * 3);
			}

			if (p_ptr->pclass == CLASS_ARCHER)
			{
				p_ptr->num_fire += (p_ptr->lev * 6);
			}

			/*
			 * Addendum -- also "Reward" high level warriors,
			 * with _any_ missile weapon -- TY
			 */
			if (p_ptr->pclass == CLASS_WARRIOR &&
			   (p_ptr->tval_ammo <= TV_BOLT) &&
			   (p_ptr->tval_ammo >= TV_SHOT))
			{
				p_ptr->num_fire += (p_ptr->lev * 2);
			}
			if ((p_ptr->pclass == CLASS_ROGUE) &&
			    (p_ptr->tval_ammo == TV_SHOT))
			{
				p_ptr->num_fire += (p_ptr->lev * 4);
			}
		}
	}

	if (p_ptr->ryoute)
		hold *= 2;

	for(i = 0 ; i < 2 ; i++)
	{
		/* Examine the "main weapon" */
		o_ptr = &inventory[INVEN_RARM+i];

		object_flags(o_ptr, &f1, &f2, &f3);

		/* Assume not heavy */
		p_ptr->heavy_wield[i] = FALSE;
		p_ptr->icky_wield[i] = FALSE;
		p_ptr->riding_wield[i] = FALSE;

		if (!buki_motteruka(INVEN_RARM+i)) {p_ptr->num_blow[i]=1;continue;}
		/* It is hard to hold a heavy weapon */
		if (hold < o_ptr->weight / 10)
		{
			/* Hard to wield a heavy weapon */
			p_ptr->to_h[i] += 2 * (hold - o_ptr->weight / 10);
			p_ptr->dis_to_h[i] += 2 * (hold - o_ptr->weight / 10);

			/* Heavy weapon */
			p_ptr->heavy_wield[i] = TRUE;
		}
		else if (p_ptr->ryoute && (hold < o_ptr->weight/5)) omoi = TRUE;

		if ((i == 1) && (o_ptr->tval == TV_SWORD) && ((o_ptr->sval == SV_MAIN_GAUCHE) || (o_ptr->sval == SV_WAKIZASHI)))
		{
			p_ptr->to_a += 5;
			p_ptr->dis_to_a += 5;
		}

		/* Normal weapons */
		if (o_ptr->k_idx && !p_ptr->heavy_wield[i])
		{
			int str_index, dex_index;

			int num = 0, wgt = 0, mul = 0, div = 0;

			/* Analyze the class */
			switch (p_ptr->pclass)
			{
				/* Warrior */
				case CLASS_WARRIOR:
					num = 6; wgt = 70; mul = 5; break;

				/* Berserker */
				case CLASS_BERSERKER:
					num = 6; wgt = 70; mul = 7; break;

				/* Mage */
				case CLASS_MAGE:
				case CLASS_HIGH_MAGE:
				case CLASS_BLUE_MAGE:
					num = 3; wgt = 100; mul = 2; break;

				/* Priest, Mindcrafter */
				case CLASS_PRIEST:
				case CLASS_MAGIC_EATER:
				case CLASS_MINDCRAFTER:
					num = 5; wgt = 100; mul = 3; break;

				/* Rogue */
				case CLASS_ROGUE:
					num = 5; wgt = 40; mul = 3; break;

				/* Ranger */
				case CLASS_RANGER:
					num = 5; wgt = 70; mul = 4; break;

				/* Paladin */
				case CLASS_PALADIN:
				case CLASS_SAMURAI:
					num = 5; wgt = 70; mul = 4; break;

				/* Kaji */
				case CLASS_SMITH:
					num = 5; wgt = 150; mul = 5; break;

				/* Warrior-Mage */
				case CLASS_WARRIOR_MAGE:
				case CLASS_RED_MAGE:
					num = 5; wgt = 70; mul = 3; break;

				/* Chaos Warrior */
				case CLASS_CHAOS_WARRIOR:
					num = 5; wgt = 70; mul = 4; break;

				/* Monk */
				case CLASS_MONK:
					num = 5; wgt = 60; mul = 3; break;

				/* Tourist */
				case CLASS_TOURIST:
					num = 4; wgt = 100; mul = 3; break;

				/* Imitator */
				case CLASS_IMITATOR:
					num = 5; wgt = 70; mul = 4; break;

				/* Beastmaster */
				case CLASS_BEASTMASTER:
					num = 5; wgt = 70; mul = 3; break;

				case CLASS_CAVALRY:
					if ((p_ptr->riding) && (f2 & TR2_RIDING)) {num = 5; wgt = 70; mul = 4;}
					else {num = 5; wgt = 100; mul = 3;}
					break;

				/* Sorcerer */
				case CLASS_SORCERER:
					num = 1; wgt = 1; mul = 1; break;

				/* Archer, Magic eater */
				case CLASS_ARCHER:
				case CLASS_BARD:
					num = 4; wgt = 70; mul = 2; break;

				/* ForceTrainer */
				case CLASS_FORCETRAINER:
					num = 4; wgt = 60; mul = 2; break;

				/* Mirror Master */
				case CLASS_MIRROR_MASTER:
					num = 3; wgt = 100; mul = 3; break;

				/* Ninja */
				case CLASS_NINJA:
					num = 4; wgt = 20; mul = 1; break;
			}

			/* Enforce a minimum "weight" (tenth pounds) */
			div = ((o_ptr->weight < wgt) ? wgt : o_ptr->weight);

			/* Access the strength vs weight */
			str_index = (adj_str_blow[p_ptr->stat_ind[A_STR]] * mul / div);

			if (p_ptr->ryoute && !omoi) str_index++;
			if (p_ptr->pclass == CLASS_NINJA) str_index = MAX(0, str_index-1);

			/* Maximal value */
			if (str_index > 11) str_index = 11;

			/* Index by dexterity */
			dex_index = (adj_dex_blow[p_ptr->stat_ind[A_DEX]]);

			/* Maximal value */
			if (dex_index > 11) dex_index = 11;

			/* Use the blows table */
			p_ptr->num_blow[i] = blows_table[str_index][dex_index];

			/* Maximal value */
			if (p_ptr->num_blow[i] > num) p_ptr->num_blow[i] = num;

			/* Add in the "bonus blows" */
			p_ptr->num_blow[i] += extra_blows[i];


			if (p_ptr->pclass == CLASS_WARRIOR) p_ptr->num_blow[i] += (p_ptr->lev / 40);
			else if (p_ptr->pclass == CLASS_BERSERKER)
			{
				p_ptr->num_blow[i] += (p_ptr->lev / 23);
			}
			else if ((p_ptr->pclass == CLASS_ROGUE) && (o_ptr->weight < 50) && (p_ptr->stat_ind[A_DEX] >= 30)) p_ptr->num_blow[i] ++;

			if (p_ptr->special_defense & KATA_FUUJIN) p_ptr->num_blow[i] -= 1;

			if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) p_ptr->num_blow[i] = 1;


			/* Require at least one blow */
			if (p_ptr->num_blow[i] < 1) p_ptr->num_blow[i] = 1;

			/* Boost digging skill by weapon weight */
			p_ptr->skill_dig += (o_ptr->weight / 10);
		}

		/* Assume okay */
		/* Priest weapon penalty for non-blessed edged weapons */
		if ((p_ptr->pclass == CLASS_PRIEST) && (!(f3 & (TR3_BLESSED))) &&
		    ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM)))
		{
			/* Reduce the real bonuses */
			p_ptr->to_h[i] -= 2;
			p_ptr->to_d[i] -= 2;

			/* Reduce the mental bonuses */
			p_ptr->dis_to_h[i] -= 2;
			p_ptr->dis_to_d[i] -= 2;

			/* Icky weapon */
			p_ptr->icky_wield[i] = TRUE;
		}
		else if (p_ptr->pclass == CLASS_BERSERKER)
		{
			p_ptr->to_h[i] += p_ptr->lev/5;
			p_ptr->to_d[i] += p_ptr->lev/6;
			p_ptr->dis_to_h[i] += p_ptr->lev/5;
			p_ptr->dis_to_d[i] += p_ptr->lev/6;
			if (!p_ptr->hidarite || p_ptr->ryoute)
			{
				p_ptr->to_h[i] += p_ptr->lev/5;
				p_ptr->to_d[i] += p_ptr->lev/6;
				p_ptr->dis_to_h[i] += p_ptr->lev/5;
				p_ptr->dis_to_d[i] += p_ptr->lev/6;
			}
		}
		else if (p_ptr->pclass == CLASS_SORCERER)
		{
			if (!((o_ptr->tval == TV_HAFTED) && ((o_ptr->sval == SV_WIZSTAFF) || (o_ptr->sval == SV_NAMAKE_HAMMER))))
			{
				/* Reduce the real bonuses */
				p_ptr->to_h[i] -= 200;
				p_ptr->to_d[i] -= 200;

				/* Reduce the mental bonuses */
				p_ptr->dis_to_h[i] -= 200;
				p_ptr->dis_to_d[i] -= 200;

				/* Icky weapon */
				p_ptr->icky_wield[i] = TRUE;
			}
			else
			{
				/* Reduce the real bonuses */
				p_ptr->to_h[i] -= 30;
				p_ptr->to_d[i] -= 10;

				/* Reduce the mental bonuses */
				p_ptr->dis_to_h[i] -= 30;
				p_ptr->dis_to_d[i] -= 10;
			}
		}
		if (p_ptr->riding)
		{
			if ((o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
			{
				p_ptr->to_h[i] +=15;
				p_ptr->dis_to_h[i] +=15;
			}
			else if (!(f2 & TR2_RIDING))
			{
				int penalty;
				if ((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY))
				{
					penalty = 5;
				}
				else
				{
					penalty = r_info[m_list[p_ptr->riding].r_idx].level - skill_exp[GINOU_RIDING] / 80;
					penalty += 30;
					if (penalty < 30) penalty = 30;
				}
				p_ptr->to_h[i] -= penalty;
				p_ptr->dis_to_h[i] -= penalty;

				/* Riding weapon */
				p_ptr->riding_wield[i] = TRUE;
			}
		}
	}

	if (p_ptr->riding)
	{
		int penalty = 0;

		p_ptr->riding_ryoute = FALSE;
		if (p_ptr->ryoute || !empty_hands(FALSE)) p_ptr->riding_ryoute = TRUE;

		if ((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY))
		{
			if (p_ptr->tval_ammo != TV_ARROW) penalty = 5;
		}
		else
		{
			penalty = r_info[m_list[p_ptr->riding].r_idx].level - skill_exp[GINOU_RIDING] / 80;
			penalty += 30;
			if (penalty < 30) penalty = 30;
		}
		if (p_ptr->tval_ammo == TV_BOLT) penalty *= 2;
		p_ptr->to_h_b -= penalty;
		p_ptr->dis_to_h_b -= penalty;
	}

	/* Different calculation for monks with empty hands */
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(TRUE) > 1))
	{
		int blow_base = p_ptr->lev + adj_dex_blow[p_ptr->stat_ind[A_DEX]];
		p_ptr->num_blow[0] = 0;

		if (p_ptr->pclass == CLASS_FORCETRAINER)
		{
			if (blow_base > 18) p_ptr->num_blow[0]++;
			if (blow_base > 31) p_ptr->num_blow[0]++;
			if (blow_base > 44) p_ptr->num_blow[0]++;
			if (blow_base > 58) p_ptr->num_blow[0]++;
		}
		else
		{
			if (blow_base > 12) p_ptr->num_blow[0]++;
			if (blow_base > 22) p_ptr->num_blow[0]++;
			if (blow_base > 31) p_ptr->num_blow[0]++;
			if (blow_base > 39) p_ptr->num_blow[0]++;
			if (blow_base > 46) p_ptr->num_blow[0]++;
			if (blow_base > 53) p_ptr->num_blow[0]++;
			if (blow_base > 59) p_ptr->num_blow[0]++;
		}

		if (heavy_armor() && (p_ptr->pclass != CLASS_BERSERKER))
			p_ptr->num_blow[0] /= 2;
		else
		{
			p_ptr->to_h[0] += (p_ptr->lev / 3);
			p_ptr->dis_to_h[0] += (p_ptr->lev / 3);

			p_ptr->to_d[0] += (p_ptr->lev / 6);
			p_ptr->dis_to_d[0] += (p_ptr->lev / 6);
		}

		if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			p_ptr->to_a -= 40;
			p_ptr->dis_to_a -= 40;
			
		}
		else if (p_ptr->special_defense & KAMAE_SEIRYU)
		{
			p_ptr->to_a -= 50;
			p_ptr->dis_to_a -= 50;
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->sh_fire = TRUE;
			p_ptr->sh_elec = TRUE;
			p_ptr->sh_cold = TRUE;
			p_ptr->ffall = TRUE;
		}
		else if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->to_a += (p_ptr->lev*p_ptr->lev)/50;
			p_ptr->dis_to_a += (p_ptr->lev*p_ptr->lev)/50;
			p_ptr->reflect = TRUE;
			p_ptr->num_blow[0] -= 2;
			if ((p_ptr->pclass == CLASS_MONK) && (p_ptr->lev > 42)) p_ptr->num_blow[0]--;
			if (p_ptr->num_blow[0] < 0) p_ptr->num_blow[0] = 0;
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			p_ptr->to_h[0] -= (p_ptr->lev / 3);
			p_ptr->to_d[0] -= (p_ptr->lev / 6);

			p_ptr->dis_to_h[0] -= (p_ptr->lev / 3);
			p_ptr->dis_to_d[0] -= (p_ptr->lev / 6);
			p_ptr->num_blow[0] /= 2;
			p_ptr->ffall = TRUE;
		}

		p_ptr->num_blow[0] += 1+extra_blows[0];
	}

	monk_armour_aux = FALSE;

	if (heavy_armor())
	{
		monk_armour_aux = TRUE;
	}

	for (i = 0 ; i < 2 ; i++)
	{
		if(buki_motteruka(INVEN_RARM+i))
		{
			p_ptr->to_h[i] += (weapon_exp[inventory[INVEN_RARM+i].tval-TV_BOW][inventory[INVEN_RARM+i].sval]-4000)/200;
			p_ptr->dis_to_h[i] += (weapon_exp[inventory[INVEN_RARM+i].tval-TV_BOW][inventory[INVEN_RARM+i].sval]-4000)/200;
			if ((p_ptr->pclass == CLASS_MONK) && !(weapon_exp_settei[CLASS_MONK][inventory[INVEN_RARM+i].tval-TV_BOW][inventory[INVEN_RARM+i].sval][1]))
			{
				p_ptr->to_h[i] -= 40;
				p_ptr->dis_to_h[i] -= 40;
				p_ptr->icky_wield[i] = TRUE;
			}
			else if ((p_ptr->pclass == CLASS_FORCETRAINER) && !(weapon_exp_settei[CLASS_FORCETRAINER][inventory[INVEN_RARM+i].tval-TV_BOW][inventory[INVEN_RARM+i].sval][1]))
			{
				p_ptr->to_h[i] -= 40;
				p_ptr->dis_to_h[i] -= 40;
				p_ptr->icky_wield[i] = TRUE;
			}
			else if (p_ptr->pclass == CLASS_NINJA)
			{
				if ((weapon_exp_settei[CLASS_NINJA][inventory[INVEN_RARM+i].tval-TV_BOW][inventory[INVEN_RARM+i].sval][1] <= 4000) || (inventory[INVEN_LARM-i].tval == TV_SHIELD))
				{
					p_ptr->to_h[i] -= 40;
					p_ptr->dis_to_h[i] -= 40;
					p_ptr->icky_wield[i] = TRUE;
					p_ptr->num_blow[i] /= 2;
					if (p_ptr->num_blow[i] < 1) p_ptr->num_blow[i] = 1;
				}
			}
		}
	}

	/* Temporary lightspeed */
	if ((p_ptr->lightspeed && !p_ptr->riding) || (p_ptr->pspeed > 209))
	{
		p_ptr->pspeed = 209;
	}
	if (p_ptr->pspeed < 11) p_ptr->pspeed = 11;

	/* Display the speed (if needed) */
	if (p_ptr->pspeed != old_speed) p_ptr->redraw |= (PR_SPEED);

	if (yoiyami)
	{
		if (p_ptr->to_a > (0 - p_ptr->ac))
			p_ptr->to_a = 0 - p_ptr->ac;
		if (p_ptr->dis_to_a > (0 - p_ptr->dis_ac))
			p_ptr->dis_to_a = 0 - p_ptr->dis_ac;
	}

	/* Redraw armor (if needed) */
	if ((p_ptr->dis_ac != old_dis_ac) || (p_ptr->dis_to_a != old_dis_to_a))
	{
		/* Redraw */
		p_ptr->redraw |= (PR_ARMOR);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}


	if (p_ptr->ryoute && !omoi)
	{
		int bonus_to_h=0, bonus_to_d=0;
		bonus_to_d = ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		bonus_to_h = ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128) + ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);

		p_ptr->to_h[0] += MAX(bonus_to_h,1);
		p_ptr->dis_to_h[0] += MAX(bonus_to_h,1);
		p_ptr->to_d[0] += MAX(bonus_to_d,1);
		p_ptr->dis_to_d[0] += MAX(bonus_to_d,1);
	}

	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(TRUE) == 3)) p_ptr->ryoute = FALSE;

	/* Affect Skill -- stealth (bonus one) */
	p_ptr->skill_stl += 1;

	if (p_ptr->tim_stealth || music_singing(MUSIC_STEALTH))
		p_ptr->skill_stl += 99;

	/* Affect Skill -- disarming (DEX and INT) */
	p_ptr->skill_dis += adj_dex_dis[p_ptr->stat_ind[A_DEX]];
	p_ptr->skill_dis += adj_int_dis[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- magic devices (INT) */
	p_ptr->skill_dev += adj_int_dev[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- saving throw (WIS) */
	p_ptr->skill_sav += adj_wis_sav[p_ptr->stat_ind[A_WIS]];

	/* Affect Skill -- digging (STR) */
	p_ptr->skill_dig += adj_str_dig[p_ptr->stat_ind[A_STR]];

	/* Affect Skill -- disarming (Level, by Class) */
	p_ptr->skill_dis += ((cp_ptr->x_dis * p_ptr->lev / 10) + (ap_ptr->a_dis * p_ptr->lev / 50));

	/* Affect Skill -- magic devices (Level, by Class) */
	p_ptr->skill_dev += ((cp_ptr->x_dev * p_ptr->lev / 10) + (ap_ptr->a_dev * p_ptr->lev / 50));

	/* Affect Skill -- saving throw (Level, by Class) */
	p_ptr->skill_sav += ((cp_ptr->x_sav * p_ptr->lev / 10) + (ap_ptr->a_sav * p_ptr->lev / 50));

	/* Affect Skill -- stealth (Level, by Class) */
	p_ptr->skill_stl += (cp_ptr->x_stl * p_ptr->lev / 10);

	/* Affect Skill -- search ability (Level, by Class) */
	p_ptr->skill_srh += (cp_ptr->x_srh * p_ptr->lev / 10);

	/* Affect Skill -- search frequency (Level, by Class) */
	p_ptr->skill_fos += (cp_ptr->x_fos * p_ptr->lev / 10);

	/* Affect Skill -- combat (normal) (Level, by Class) */
	p_ptr->skill_thn += ((cp_ptr->x_thn * p_ptr->lev / 10) + (ap_ptr->a_thn * p_ptr->lev / 50));

	/* Affect Skill -- combat (shooting) (Level, by Class) */
	p_ptr->skill_thb += ((cp_ptr->x_thb * p_ptr->lev / 10) + (ap_ptr->a_thb * p_ptr->lev / 50));

	/* Affect Skill -- combat (throwing) (Level, by Class) */
	p_ptr->skill_tht += ((cp_ptr->x_thb * p_ptr->lev / 10) + (ap_ptr->a_thb * p_ptr->lev / 50));


	if ((prace_is_(RACE_S_FAIRY)) && (p_ptr->pseikaku != SEIKAKU_SEXY) && p_ptr->aggravate)
	{
		p_ptr->aggravate = FALSE;
		p_ptr->skill_stl = MIN(p_ptr->skill_stl - 3, (p_ptr->skill_stl + 2) / 2);
	}

	/* Limit Skill -- stealth from 0 to 30 */
	if (p_ptr->skill_stl > 30) p_ptr->skill_stl = 30;
	if (p_ptr->skill_stl < 0) p_ptr->skill_stl = 0;

	/* Limit Skill -- digging from 1 up */
	if (p_ptr->skill_dig < 1) p_ptr->skill_dig = 1;

	if (p_ptr->anti_magic && (p_ptr->skill_sav < (90 + p_ptr->lev))) p_ptr->skill_sav = 90 + p_ptr->lev;

	if (p_ptr->tsubureru) p_ptr->skill_sav = 10;

	if ((p_ptr->ult_res || p_ptr->resist_magic || p_ptr->magicdef) && (p_ptr->skill_sav < (95 + p_ptr->lev))) p_ptr->skill_sav = 95 + p_ptr->lev;

	if (down_saving) p_ptr->skill_sav /= 2;

	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	/* Take note when "heavy bow" changes */
	if (p_ptr->old_heavy_shoot != p_ptr->heavy_shoot)
	{
		/* Message */
		if (p_ptr->heavy_shoot)
		{
#ifdef JP
			msg_print("����ʽŤ��ݤ��������Ƥ���Τ����Ѥ���");
#else
			msg_print("You have trouble wielding such a heavy bow.");
#endif

		}
		else if (inventory[INVEN_BOW].k_idx)
		{
#ifdef JP
			msg_print("���εݤʤ��������Ƥ��Ƥ�ɤ��ʤ���");
#else
			msg_print("You have no trouble wielding your bow.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("�Ť��ݤ���������Ϥ������Τ��ڤˤʤä���");
#else
			msg_print("You feel relieved to put down your heavy bow.");
#endif

		}

		/* Save it */
		p_ptr->old_heavy_shoot = p_ptr->heavy_shoot;
	}

	for(i = 0 ; i < 2 ; i++)
	{
		/* Take note when "heavy weapon" changes */
		if (p_ptr->old_heavy_wield[i] != p_ptr->heavy_wield[i])
		{
			/* Message */
			if (p_ptr->heavy_wield[i])
			{
#ifdef JP
				msg_print("����ʽŤ������������Ƥ���Τ����Ѥ���");
#else
				msg_print("You have trouble wielding such a heavy weapon.");
#endif

			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("����ʤ��������Ƥ��Ƥ�ɤ��ʤ���");
#else
				msg_print("You have no trouble wielding your weapon.");
#endif

			}
			else if (p_ptr->heavy_wield[1-i])
			{
#ifdef JP
				msg_print("�ޤ���郎�Ť���");
#else
				msg_print("You have still trouble wielding a heavy weapon.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("�Ť�������������Ϥ������Τ��ڤˤʤä���");
#else
				msg_print("You feel relieved to put down your heavy weapon.");
#endif

			}

			/* Save it */
			p_ptr->old_heavy_wield[i] = p_ptr->heavy_wield[i];
		}

		/* Take note when "heavy weapon" changes */
		if (p_ptr->old_riding_wield[i] != p_ptr->riding_wield[i])
		{
			/* Message */
			if (p_ptr->riding_wield[i])
			{
#ifdef JP
				msg_print("�������Ͼ�����˻Ȥ��ˤϤफ�ʤ��褦����");
#else
				msg_print("This weapon is not suitable for riding.");
#endif

			}
			else if (!p_ptr->riding)
			{
#ifdef JP
				msg_print("������������ǻȤ��䤹����");
#else
				msg_print("This weapon was not suitable for riding.");
#endif

			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("����ʤ������ˤԤä������");
#else
				msg_print("This weapon is suitable for riding.");
#endif

			}
			/* Save it */
			p_ptr->old_riding_wield[i] = p_ptr->riding_wield[i];
		}

		/* Take note when "illegal weapon" changes */
		if (p_ptr->old_icky_wield[i] != p_ptr->icky_wield[i])
		{
			/* Message */
			if (p_ptr->icky_wield[i])
			{
#ifdef JP
				msg_print("���������Ϥɤ��⼫ʬ�ˤդ��路���ʤ��������롣");
#else
				msg_print("You do not feel comfortable with your weapon.");
#endif
				if (hack_mind)
				{
					chg_virtue(V_FAITH, -1);
				}
			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("���������ϼ�ʬ�ˤդ��路���������롣");
#else
				msg_print("You feel comfortable with your weapon.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("������Ϥ��������ʬ�ȵ����ڤˤʤä���");
#else
				msg_print("You feel more comfortable after removing your weapon.");
#endif

			}

			/* Save it */
			p_ptr->old_icky_wield[i] = p_ptr->icky_wield[i];
		}
	}

	if (p_ptr->riding && (p_ptr->old_riding_ryoute != p_ptr->riding_ryoute))
	{
		/* Message */
		if (p_ptr->riding_ryoute)
		{
#ifdef JP
			msg_print("ξ�꤬�դ����äƤ����Ϥ����ʤ���");
#else
			msg_print("You are using both hand for fighting, and you can't control a riding pet.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("�꤬�������Ϥ�����褦�ˤʤä���");
#else
			msg_print("You began to control riding pet with one hand.");
#endif
		}

		p_ptr->old_riding_ryoute = p_ptr->riding_ryoute;
	}

	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_NINJA)) && (monk_armour_aux != monk_notify_aux))
	{
		if (heavy_armor())
		{
#ifdef JP
msg_print("�������Ť��ƥХ�󥹤���ʤ���");
#else
			msg_print("The weight of your armor disrupts your balance.");
#endif

			if (hack_mind)
			{
				chg_virtue(V_HARMONY, -1);
			}
		}
		else
#ifdef JP
msg_print("�Х�󥹤��Ȥ��褦�ˤʤä���");
#else
			msg_print("You regain your balance.");
#endif

		monk_notify_aux = monk_armour_aux;
	}

	j = 0;
	p_ptr->align = friend_align;

        /* Determine player alignment */
        for (i = 0; i < 8; i++)
        {
		if ((p_ptr->vir_types[i] == V_HARMONY) || (p_ptr->vir_types[i] == V_NATURE))
		{
			neutral[j] = i;
			j++;
		}
		else if (p_ptr->vir_types[i] == V_UNLIFE) p_ptr->align -= p_ptr->virtues[i];
		else if (p_ptr->vir_types[i] == V_JUSTICE) p_ptr->align += (p_ptr->virtues[i]*2);
		else if (p_ptr->vir_types[i] != V_CHANCE) p_ptr->align += p_ptr->virtues[i];
        }
	while (j)
	{
		if (p_ptr->align > 0)
		{
			p_ptr->align -= (p_ptr->virtues[j]/2);
			if (p_ptr->align < 0) p_ptr->align = 0;
		}
		else if (p_ptr->align < 0)
		{
			p_ptr->align += (p_ptr->virtues[j]/2);
			if (p_ptr->align > 0) p_ptr->align = 0;
		}
		j--;
	}
	if ((inventory[INVEN_RARM].name1 == ART_IRON_BALL) || (inventory[INVEN_LARM].name1 == ART_IRON_BALL)) p_ptr->align -= 1000;
	if (prace_is_(RACE_ANGEL)) p_ptr->align += 200;
	if ((prace_is_(RACE_DEMON)) || (p_ptr->mimic_form == MIMIC_DEMON_LORD) || (p_ptr->mimic_form == MIMIC_DEMON)) p_ptr->align -= 200;

	have_dd_s = FALSE;
	have_dd_t = FALSE;
	have_sw = FALSE;
	have_kabe = FALSE;
	for (i = 0; i < INVEN_PACK; i++)
	{
		if ((inventory[i].tval == TV_SORCERY_BOOK) && (inventory[i].sval == 3)) have_dd_s = TRUE;
		if ((inventory[i].tval == TV_TRUMP_BOOK) && (inventory[i].sval == 1)) have_dd_t = TRUE;
		if ((inventory[i].tval == TV_NATURE_BOOK) && (inventory[i].sval == 2)) have_sw = TRUE;
		if ((inventory[i].tval == TV_ENCHANT_BOOK) && (inventory[i].sval == 2)) have_kabe = TRUE;
	}
	for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		if ((o_ptr->tval == TV_SORCERY_BOOK) && (o_ptr->sval == 3)) have_dd_s = TRUE;
		if ((o_ptr->tval == TV_TRUMP_BOOK) && (o_ptr->sval == 1)) have_dd_t = TRUE;
		if ((o_ptr->tval == TV_NATURE_BOOK) && (o_ptr->sval == 2)) have_sw = TRUE;
		if ((o_ptr->tval == TV_ENCHANT_BOOK) && (o_ptr->sval == 2)) have_kabe = TRUE;
	}

	if ((p_ptr->pass_wall && !p_ptr->kill_wall) || p_ptr->kabenuke || p_ptr->wraith_form) p_ptr->no_flowed = TRUE;
#if 0
	if (have_dd_s && ((p_ptr->realm1 == REALM_SORCERY) || (p_ptr->realm2 == REALM_SORCERY) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_SORCERY-1][SPELL_DD_S];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	if (have_dd_t && ((p_ptr->realm1 == REALM_TRUMP) || (p_ptr->realm2 == REALM_TRUMP) || (p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_TRUMP-1][SPELL_DD_T];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}
#endif
	if (have_sw && ((p_ptr->realm1 == REALM_NATURE) || (p_ptr->realm2 == REALM_NATURE) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_NATURE-1][SPELL_SW];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	if (have_kabe && ((p_ptr->realm1 == REALM_ENCHANT) || (p_ptr->realm2 == REALM_ENCHANT) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_ENCHANT-1][SPELL_KABE];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}
}



/*
 * Handle "p_ptr->notice"
 */
void notice_stuff(void)
{
	/* Notice stuff */
	if (!p_ptr->notice) return;


	/* Combine the pack */
	if (p_ptr->notice & (PN_COMBINE))
	{
		p_ptr->notice &= ~(PN_COMBINE);
		combine_pack();
	}

	/* Reorder the pack */
	if (p_ptr->notice & (PN_REORDER))
	{
		p_ptr->notice &= ~(PN_REORDER);
		reorder_pack();
	}
}


/*
 * Handle "p_ptr->update"
 */
void update_stuff(void)
{
	/* Update stuff */
	if (!p_ptr->update) return;


	if (p_ptr->update & (PU_BONUS))
	{
		p_ptr->update &= ~(PU_BONUS);
		calc_bonuses();
	}

	if (p_ptr->update & (PU_TORCH))
	{
		p_ptr->update &= ~(PU_TORCH);
		calc_torch();
	}

	if (p_ptr->update & (PU_HP))
	{
		p_ptr->update &= ~(PU_HP);
		calc_hitpoints();
	}

	if (p_ptr->update & (PU_MANA))
	{
		p_ptr->update &= ~(PU_MANA);
		calc_mana();
	}

	if (p_ptr->update & (PU_SPELLS))
	{
		p_ptr->update &= ~(PU_SPELLS);
		calc_spells();
	}


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;


	if (p_ptr->update & (PU_UN_LITE))
	{
		p_ptr->update &= ~(PU_UN_LITE);
		forget_lite();
	}

	if (p_ptr->update & (PU_UN_VIEW))
	{
		p_ptr->update &= ~(PU_UN_VIEW);
		forget_view();
	}

	if (p_ptr->update & (PU_VIEW))
	{
		p_ptr->update &= ~(PU_VIEW);
		update_view();
	}

	if (p_ptr->update & (PU_LITE))
	{
		p_ptr->update &= ~(PU_LITE);
		update_lite();
	}


	if (p_ptr->update & (PU_FLOW))
	{
		p_ptr->update &= ~(PU_FLOW);
		update_flow();
	}

	if (p_ptr->update & (PU_MON_LITE))
	{
		p_ptr->update &= ~(PU_MON_LITE);
		update_mon_lite();
	}

	if (p_ptr->update & (PU_DISTANCE))
	{
		p_ptr->update &= ~(PU_DISTANCE);
		p_ptr->update &= ~(PU_MONSTERS);
		update_monsters(TRUE);
	}

	if (p_ptr->update & (PU_MONSTERS))
	{
		p_ptr->update &= ~(PU_MONSTERS);
		update_monsters(FALSE);
	}
}


/*
 * Handle "p_ptr->redraw"
 */
void redraw_stuff(void)
{
	/* Redraw stuff */
	if (!p_ptr->redraw) return;


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;



	/* Hack -- clear the screen */
	if (p_ptr->redraw & (PR_WIPE))
	{
		p_ptr->redraw &= ~(PR_WIPE);
		msg_print(NULL);
		Term_clear();
	}


	if (p_ptr->redraw & (PR_MAP))
	{
		p_ptr->redraw &= ~(PR_MAP);
		prt_map();
	}


	if (p_ptr->redraw & (PR_BASIC))
	{
		p_ptr->redraw &= ~(PR_BASIC);
		p_ptr->redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
		p_ptr->redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
		p_ptr->redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
		p_ptr->redraw &= ~(PR_DEPTH | PR_HEALTH | PR_UHEALTH);
		prt_frame_basic();
		prt_time();
		prt_dungeon();
	}

	if (p_ptr->redraw & (PR_DUNGEON))
	{
		p_ptr->redraw &= ~(PR_DUNGEON);
	}

	if (p_ptr->redraw & (PR_EQUIPPY))
	{
		p_ptr->redraw &= ~(PR_EQUIPPY);
		print_equippy(); /* To draw / delete equippy chars */
	}

	if (p_ptr->redraw & (PR_MISC))
	{
		p_ptr->redraw &= ~(PR_MISC);
		prt_field(rp_ptr->title, ROW_RACE, COL_RACE);
/*		prt_field(cp_ptr->title, ROW_CLASS, COL_CLASS); */

	}

	if (p_ptr->redraw & (PR_TITLE))
	{
		p_ptr->redraw &= ~(PR_TITLE);
		prt_title();
	}

	if (p_ptr->redraw & (PR_LEV))
	{
		p_ptr->redraw &= ~(PR_LEV);
		prt_level();
	}

	if (p_ptr->redraw & (PR_EXP))
	{
		p_ptr->redraw &= ~(PR_EXP);
		prt_exp();
	}

	if (p_ptr->redraw & (PR_STATS))
	{
		p_ptr->redraw &= ~(PR_STATS);
		prt_stat(A_STR);
		prt_stat(A_INT);
		prt_stat(A_WIS);
		prt_stat(A_DEX);
		prt_stat(A_CON);
		prt_stat(A_CHR);
	}

	if (p_ptr->redraw & (PR_STATUS))
	{
		p_ptr->redraw &= ~(PR_STATUS);
		prt_status();
	}

	if (p_ptr->redraw & (PR_ARMOR))
	{
		p_ptr->redraw &= ~(PR_ARMOR);
		prt_ac();
	}

	if (p_ptr->redraw & (PR_HP))
	{
		p_ptr->redraw &= ~(PR_HP);
		prt_hp();
	}

	if (p_ptr->redraw & (PR_MANA))
	{
		p_ptr->redraw &= ~(PR_MANA);
		prt_sp();
	}

	if (p_ptr->redraw & (PR_GOLD))
	{
		p_ptr->redraw &= ~(PR_GOLD);
		prt_gold();
	}

	if (p_ptr->redraw & (PR_DEPTH))
	{
		p_ptr->redraw &= ~(PR_DEPTH);
		prt_depth();
	}

	if (p_ptr->redraw & (PR_HEALTH))
	{
		p_ptr->redraw &= ~(PR_HEALTH);
		health_redraw();
	}

	if (p_ptr->redraw & (PR_UHEALTH))
	{
		p_ptr->redraw &= ~(PR_UHEALTH);
		riding_health_redraw();
	}


	if (p_ptr->redraw & (PR_EXTRA))
	{
		p_ptr->redraw &= ~(PR_EXTRA);
		p_ptr->redraw &= ~(PR_CUT | PR_STUN);
		p_ptr->redraw &= ~(PR_HUNGER);
		p_ptr->redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY | PR_MANE | PR_STATUS);
		prt_frame_extra();
	}

	if (p_ptr->redraw & (PR_CUT))
	{
		p_ptr->redraw &= ~(PR_CUT);
		prt_cut();
	}

	if (p_ptr->redraw & (PR_STUN))
	{
		p_ptr->redraw &= ~(PR_STUN);
		prt_stun();
	}

	if (p_ptr->redraw & (PR_HUNGER))
	{
		p_ptr->redraw &= ~(PR_HUNGER);
		prt_hunger();
	}

	if (p_ptr->redraw & (PR_STATE))
	{
		p_ptr->redraw &= ~(PR_STATE);
		prt_state();
	}

	if (p_ptr->redraw & (PR_SPEED))
	{
		p_ptr->redraw &= ~(PR_SPEED);
		prt_speed();
	}

	if (p_ptr->pclass == CLASS_IMITATOR)
	{
		if (p_ptr->redraw & (PR_MANE))
		{
			p_ptr->redraw &= ~(PR_MANE);
			prt_mane();
		}
	}
	else if (p_ptr->redraw & (PR_STUDY))
	{
		p_ptr->redraw &= ~(PR_STUDY);
		prt_study();
	}
}


/*
 * Handle "p_ptr->window"
 */
void window_stuff(void)
{
	int j;

	u32b mask = 0L;


	/* Nothing to do */
	if (!p_ptr->window) return;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		/* Save usable flags */
		if (angband_term[j]) mask |= window_flag[j];
	}

	/* Apply usable flags */
	p_ptr->window &= mask;

	/* Nothing to do */
	if (!p_ptr->window) return;


	/* Display inventory */
	if (p_ptr->window & (PW_INVEN))
	{
		p_ptr->window &= ~(PW_INVEN);
		fix_inven();
	}

	/* Display equipment */
	if (p_ptr->window & (PW_EQUIP))
	{
		p_ptr->window &= ~(PW_EQUIP);
		fix_equip();
	}

	/* Display spell list */
	if (p_ptr->window & (PW_SPELL))
	{
		p_ptr->window &= ~(PW_SPELL);
		fix_spell();
	}

	/* Display player */
	if (p_ptr->window & (PW_PLAYER))
	{
		p_ptr->window &= ~(PW_PLAYER);
		fix_player();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_MESSAGE))
	{
		p_ptr->window &= ~(PW_MESSAGE);
		fix_message();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_OVERHEAD))
	{
		p_ptr->window &= ~(PW_OVERHEAD);
		fix_overhead();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_DUNGEON))
	{
		p_ptr->window &= ~(PW_DUNGEON);
		fix_dungeon();
	}

	/* Display monster recall */
	if (p_ptr->window & (PW_MONSTER))
	{
		p_ptr->window &= ~(PW_MONSTER);
		fix_monster();
	}

	/* Display object recall */
	if (p_ptr->window & (PW_OBJECT))
	{
		p_ptr->window &= ~(PW_OBJECT);
		fix_object();
	}
}


/*
 * Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window"
 */
void handle_stuff(void)
{
	/* Update stuff */
	if (p_ptr->update) update_stuff();

	/* Redraw stuff */
	if (p_ptr->redraw) redraw_stuff();

	/* Window stuff */
	if (p_ptr->window) window_stuff();
}


s16b empty_hands(bool is_monk)
{
	s16b kaerichi = 0;
	if (is_monk && (p_ptr->pclass != CLASS_MONK) && (p_ptr->pclass != CLASS_FORCETRAINER) && (p_ptr->pclass != CLASS_BERSERKER)) return FALSE;

	if (!(inventory[INVEN_RARM].k_idx)) kaerichi +=2;
	if (!(inventory[INVEN_LARM].k_idx)) kaerichi +=1;
	return kaerichi;
}


bool heavy_armor(void)
{
	u16b monk_arm_wgt = 0;

	if ((p_ptr->pclass != CLASS_MONK) && (p_ptr->pclass != CLASS_FORCETRAINER) && (p_ptr->pclass != CLASS_NINJA)) return FALSE;

	/* Weight the armor */
	if(inventory[INVEN_RARM].tval > TV_SWORD) monk_arm_wgt += inventory[INVEN_RARM].weight;
	if(inventory[INVEN_LARM].tval > TV_SWORD) monk_arm_wgt += inventory[INVEN_LARM].weight;
	monk_arm_wgt += inventory[INVEN_BODY].weight;
	monk_arm_wgt += inventory[INVEN_HEAD].weight;
	monk_arm_wgt += inventory[INVEN_OUTER].weight;
	monk_arm_wgt += inventory[INVEN_HANDS].weight;
	monk_arm_wgt += inventory[INVEN_FEET].weight;

	return (monk_arm_wgt > (100 + (p_ptr->lev * 4)));
}

int number_of_quests(void)
{
        int i, j;

        /* Clear the counter */
        i = 0;

        for (j = MIN_RANDOM_QUEST; j < MAX_RANDOM_QUEST+1; j++)
        {
                if (quest[j].status != QUEST_STATUS_UNTAKEN)
                {
                        /* Increment count of quests taken. */
                        i++;
                }
        }

        /* Return the number of quests taken */
        return (i);
}
