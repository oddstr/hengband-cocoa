/* File: wizard2.c */

/* Purpose: Wizard commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- Rerate Hitpoints
 */
void do_cmd_rerate(bool display)
{
	int min_value, max_value, i, percent, j;

	min_value = ((PY_MAX_LEVEL+2) * (p_ptr->hitdie + 1)) * 3 / 8;
	min_value += p_ptr->hitdie;

	max_value = ((PY_MAX_LEVEL+2) * (p_ptr->hitdie + 1)) * 5 / 8;
	max_value += p_ptr->hitdie;

	/* Rerate */
	while (1)
	{
		p_ptr->player_hp[0] = p_ptr->hitdie;

		for (i = 1; i < 4; i++)
		{
			j = randint1(p_ptr->hitdie);
			p_ptr->player_hp[0] += j;
		}

		/* Collect values */
		for (i = 1; i < PY_MAX_LEVEL; i++)
		{
			p_ptr->player_hp[i] = randint1(p_ptr->hitdie);
			p_ptr->player_hp[i] += p_ptr->player_hp[i - 1];
		}

		/* Legal values */
		if ((p_ptr->player_hp[PY_MAX_LEVEL - 1] >= min_value) &&
		    (p_ptr->player_hp[PY_MAX_LEVEL - 1] <= max_value)) break;
	}

	percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * p_ptr->hitdie +
		((PY_MAX_LEVEL - 1+3) * (p_ptr->hitdie + 1))));


	/* Update and redraw hitpoints */
	p_ptr->update |= (PU_HP);
	p_ptr->redraw |= (PR_HP);

	/* Window stuff */
	p_ptr->window |= (PW_PLAYER);

	/* Handle stuff */
	handle_stuff();

	/* Message */
	if (display)
	{
#ifdef JP
		msg_format("���ߤ����ϥ�󥯤� %d/100 �Ǥ���", percent);
#else
		msg_format("Your life rate is %d/100 now.", percent);
#endif
		p_ptr->knowledge |= KNOW_HPRATE;
	}
	else
	{
#ifdef JP
		msg_print("���ϥ�󥯤��Ѥ�ä���");
#else
		msg_print("Life rate is changed.");
#endif
		p_ptr->knowledge &= ~(KNOW_HPRATE);
	}
}


#ifdef ALLOW_WIZARD

/*
 * Dimension Door
 */
static bool wiz_dimension_door(void)
{
	int	x = 0, y = 0;

	if (!tgt_pt(&x, &y)) return FALSE;

	if (!cave_empty_bold(y, x))
	{
#ifdef JP
msg_print("�������ʪ�������������ޤ������ʤ��ä���");
#else
		msg_print("You fail to exit the astral plane correctly!");
#endif

		teleport_player(10);
	}
	else teleport_player_to(y, x, FALSE);

	return (TRUE);
}


/*
 * Create the artifact of the specified number -- DAN
 *
 */
static void wiz_create_named_art(int a_idx)
{
	/* Create the artifact */
	create_named_art(a_idx, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Hack -- quick debugging hook
 */
static void do_cmd_wiz_hack_ben(void)
{
	/* Oops */
	msg_print("Oops.");
	(void)probing();
}



#ifdef MONSTER_HORDES

/* Summon a horde of monsters */
static void do_cmd_summon_horde(void)
{
	int wy = py, wx = px;
	int attempts = 1000;

	while (--attempts)
	{
		scatter(&wy, &wx, py, px, 3, 0);
		if (cave_naked_bold(wy, wx)) break;
	}

	(void)alloc_horde(wy, wx);
}

#endif /* MONSTER_HORDES */


/*
 * Output a long int in binary format.
 */
static void prt_binary(u32b flags, int row, int col)
{
	int        	i;
	u32b        bitmask;

	/* Scan the flags */
	for (i = bitmask = 1; i <= 32; i++, bitmask *= 2)
	{
		/* Dump set bits */
		if (flags & bitmask)
		{
			Term_putch(col++, row, TERM_BLUE, '*');
		}

		/* Dump unset bits */
		else
		{
			Term_putch(col++, row, TERM_WHITE, '-');
		}
	}
}


#define K_MAX_DEPTH 110

/*
 * Output a rarity graph for a type of object.
 */
static void prt_alloc(byte tval, byte sval, int row, int col)
{
	int i, j;
	int home = 0;
	u32b maxr = 1, maxt = 1, ratio;
	u32b rarity[K_MAX_DEPTH];
	u32b total[K_MAX_DEPTH];
	s32b maxd = 1, display[22];
	byte c = TERM_WHITE;
	cptr r = "+--common--+";
	object_kind *k_ptr;


	/* Get the entry */
	alloc_entry *table = alloc_kind_table;

	/* Wipe the tables */
	(void)C_WIPE(rarity, K_MAX_DEPTH, u32b);
	(void)C_WIPE(total, K_MAX_DEPTH, u32b);
	(void)C_WIPE(display, 22, s32b);

	/* Scan all entries */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		int total_frac = 0;
		for (j = 0; j < alloc_kind_size; j++)
		{
			int prob = 0;

			if (table[j].level <= i)
			{
				prob = table[j].prob1 * GREAT_OBJ * K_MAX_DEPTH;
			}
			else if (table[j].level - 1 > 0)
			{
				prob = table[j].prob1 * i * K_MAX_DEPTH / (table[j].level - 1);
			}

			/* Acquire this kind */
			k_ptr = &k_info[table[j].index];

			/* Accumulate probabilities */
			total[i] += prob / (GREAT_OBJ * K_MAX_DEPTH);
			total_frac += prob % (GREAT_OBJ * K_MAX_DEPTH);

			/* Accumulate probabilities */
			if ((k_ptr->tval == tval) && (k_ptr->sval == sval))
			{
				home = k_ptr->level;
				rarity[i] += prob;
			}
		}
		total[i] += total_frac / (GREAT_OBJ * K_MAX_DEPTH);
	}

	/* Find maxima */
	for (i = 0; i < K_MAX_DEPTH; i++)
	{
		if (rarity[i] > maxr) maxr = rarity[i];
		if (total[i] > maxt) maxt = total[i];
	}

	if (maxr / (GREAT_OBJ * K_MAX_DEPTH) != 0)
		ratio = maxt / (maxr / (GREAT_OBJ * K_MAX_DEPTH));
	else
		ratio = 99999L;

	/* Simulate a log graph */
	if (ratio > 1000)
	{
		c = TERM_L_WHITE;
		r = "+-uncommon-+";
	}
	if (ratio > 3000)
	{
		c = TERM_SLATE;
		r = "+---rare---+";
	}
	if (ratio > 32768L)
	{
		c = TERM_L_DARK;
		r = "+-VeryRare-+";
	}

	/* Calculate probabilities for each range */
	for (i = 0; i < 22; i++)
	{
		/* Shift the values into view */

		int possibility = 0;
		for (j = i * K_MAX_DEPTH / 22; j < (i + 1) * K_MAX_DEPTH / 22; j++)
			possibility += rarity[j] * (100 * maxt / total[j]);

		possibility = possibility / maxr;

		/* display[i] = log_{sqrt(2)}(possibility) */
		display[i] = 0;
		while (possibility)
		{
			display[i]++;
			possibility = possibility * 1000 / 1414;
		}

		/* Track maximum */
		if (display[i] > maxd) maxd = display[i];
	}

	/* Normalize */
	if (maxd > 10) for (i = 0; i < 22; i++)
	{
		display[i] = display[i] - maxd + 10;
	}

	/* Graph the rarities */
	for (i = 0; i < 22; i++)
	{
		Term_putch(col, row + i + 1, TERM_WHITE,  '|');

		prt(format("%d", (i * K_MAX_DEPTH / 220) % 10), row + i + 1, col);

		if (display[i] <= 0) 
			continue;

		/* Note the level */
		if ((i * K_MAX_DEPTH / 22 <= home) && (home < (i + 1) * K_MAX_DEPTH / 22))
		{
			c_prt(TERM_RED, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
		else
		{
			c_prt(c, format("%.*s", display[i], "**********"), row + i + 1, col + 1);
		}
	}

	/* Make it look nice */
	prt(r, row, col);
}


/*
 * Hack -- Teleport to the target
 */
static void do_cmd_wiz_bamf(void)
{
	/* Must have a target */
	if (!target_who) return;

	/* Teleport to the target */
	teleport_player_to(target_row, target_col, FALSE);
}


/*
 * Aux function for "do_cmd_wiz_change()".	-RAK-
 */
static void do_cmd_wiz_change_aux(void)
{
	int i, j;
	int tmp_int;
	long tmp_long;
	s16b tmp_s16b;
	char tmp_val[160];
	char ppp[80];


	/* Query the stats */
	for (i = 0; i < 6; i++)
	{
		/* Prompt */
		sprintf(ppp, "%s (3-%d): ", stat_names[i], p_ptr->stat_max_max[i]);

		/* Default */
		sprintf(tmp_val, "%d", p_ptr->stat_max[i]);

		/* Query */
		if (!get_string(ppp, tmp_val, 3)) return;

		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Verify */
		if (tmp_int > p_ptr->stat_max_max[i]) tmp_int = p_ptr->stat_max_max[i];
		else if (tmp_int < 3) tmp_int = 3;

		/* Save it */
		p_ptr->stat_cur[i] = p_ptr->stat_max[i] = tmp_int;
	}


	/* Default */
	sprintf(tmp_val, "8000");

	/* Query */
#ifdef JP
	if (!get_string("������: ", tmp_val, 9)) return;
#else
	if (!get_string("Proficiency: ", tmp_val, 9)) return;
#endif

	/* Extract */
	tmp_s16b = atoi(tmp_val);

	/* Verify */
	if (tmp_s16b < 0) tmp_s16b = 0L;
	if (tmp_s16b > 8000) tmp_s16b = 8000L;

	for (j = 0; j <= TV_SWORD - TV_BOW;j++)
	{
		for (i = 0;i < 64;i++)
		{
			p_ptr->weapon_exp[j][i] = tmp_s16b;
			if (p_ptr->weapon_exp[j][i] > s_info[p_ptr->pclass].w_max[j][i]) p_ptr->weapon_exp[j][i] = s_info[p_ptr->pclass].w_max[j][i];
		}
	}

	for (j = 0; j < 10; j++)
	{
		p_ptr->skill_exp[j] = tmp_s16b;
		if (p_ptr->skill_exp[j] > s_info[p_ptr->pclass].s_max[j]) p_ptr->skill_exp[j] = s_info[p_ptr->pclass].s_max[j];
	}

	for (j = 0; j < 32; j++)
		p_ptr->spell_exp[j] = (tmp_s16b > 1600 ? 1600 : tmp_s16b);
	for (; j < 64; j++)
		p_ptr->spell_exp[j] = (tmp_s16b > 1400 ? 1400 : tmp_s16b);

	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->au));

	/* Query */
	if (!get_string("Gold: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	/* Save */
	p_ptr->au = tmp_long;


	/* Default */
	sprintf(tmp_val, "%ld", (long)(p_ptr->max_exp));

	/* Query */
	if (!get_string("Experience: ", tmp_val, 9)) return;

	/* Extract */
	tmp_long = atol(tmp_val);

	/* Verify */
	if (tmp_long < 0) tmp_long = 0L;

	if (p_ptr->prace != RACE_ANDROID)
	{
		/* Save */
		p_ptr->max_exp = tmp_long;
		p_ptr->exp = tmp_long;

		/* Update */
		check_experience();
	}
}


/*
 * Change various "permanent" player variables.
 */
static void do_cmd_wiz_change(void)
{
	/* Interact */
	do_cmd_wiz_change_aux();

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Wizard routines for creating objects		-RAK-
 * And for manipulating them!                   -Bernd-
 *
 * This has been rewritten to make the whole procedure
 * of debugging objects much easier and more comfortable.
 *
 * The following functions are meant to play with objects:
 * Create, modify, roll for them (for statistic purposes) and more.
 * The original functions were by RAK.
 * The function to show an item's debug information was written
 * by David Reeve Sward <sward+@CMU.EDU>.
 *                             Bernd (wiebelt@mathematik.hu-berlin.de)
 *
 * Here are the low-level functions
 * - wiz_display_item()
 *     display an item's debug-info
 * - wiz_create_itemtype()
 *     specify tval and sval (type and subtype of object)
 * - wiz_tweak_item()
 *     specify pval, +AC, +tohit, +todam
 *     Note that the wizard can leave this function anytime,
 *     thus accepting the default-values for the remaining values.
 *     pval comes first now, since it is most important.
 * - wiz_reroll_item()
 *     apply some magic to the item or turn it into an artifact.
 * - wiz_roll_item()
 *     Get some statistics about the rarity of an item:
 *     We create a lot of fake items and see if they are of the
 *     same type (tval and sval), then we compare pval and +AC.
 *     If the fake-item is better or equal it is counted.
 *     Note that cursed items that are better or equal (absolute values)
 *     are counted, too.
 *     HINT: This is *very* useful for balancing the game!
 * - wiz_quantity_item()
 *     change the quantity of an item, but be sane about it.
 *
 * And now the high-level functions
 * - do_cmd_wiz_play()
 *     play with an existing object
 * - wiz_create_item()
 *     create a new object
 *
 * Note -- You do not have to specify "pval" and other item-properties
 * directly. Just apply magic until you are satisfied with the item.
 *
 * Note -- For some items (such as wands, staffs, some rings, etc), you
 * must apply magic, or you will get "broken" or "uncharged" objects.
 *
 * Note -- Redefining artifacts via "do_cmd_wiz_play()" may destroy
 * the artifact.  Be careful.
 *
 * Hack -- this function will allow you to create multiple artifacts.
 * This "feature" may induce crashes or other nasty effects.
 */

/*
 * Just display an item's properties (debug-info)
 * Originally by David Reeve Sward <sward+@CMU.EDU>
 * Verbose item flags by -Bernd-
 */
static void wiz_display_item(object_type *o_ptr)
{
	int i, j = 13;
	u32b f1, f2, f3;
	char buf[256];

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Clear the screen */
	for (i = 1; i <= 23; i++) prt("", i, j - 2);

	prt_alloc(o_ptr->tval, o_ptr->sval, 1, 0);

	/* Describe fully */
	object_desc_store(buf, o_ptr, TRUE, 3);

	prt(buf, 2, j);

	prt(format("kind = %-5d  level = %-4d  tval = %-5d  sval = %-5d",
	           o_ptr->k_idx, get_object_level(o_ptr),
	           o_ptr->tval, o_ptr->sval), 4, j);

	prt(format("number = %-3d  wgt = %-6d  ac = %-5d    damage = %dd%d",
	           o_ptr->number, o_ptr->weight,
	           o_ptr->ac, o_ptr->dd, o_ptr->ds), 5, j);

	prt(format("pval = %-5d  toac = %-5d  tohit = %-4d  todam = %-4d",
	           o_ptr->pval, o_ptr->to_a, o_ptr->to_h, o_ptr->to_d), 6, j);

	prt(format("name1 = %-4d  name2 = %-4d  cost = %ld",
	           o_ptr->name1, o_ptr->name2, (long)object_value(o_ptr)), 7, j);

	prt(format("ident = %04x  xtra1 = %-4d  xtra2 = %-4d  timeout = %-d",
	           o_ptr->ident, o_ptr->xtra1, o_ptr->xtra2, o_ptr->timeout), 8, j);

	prt(format("xtra3 = %-4d  xtra4 = %-4d  xtra5 = %-4d  cursed  = %-d",
	           o_ptr->xtra3, o_ptr->xtra4, o_ptr->xtra5, o_ptr->curse_flags), 9, j);

	prt("+------------FLAGS1------------+", 10, j);
	prt("AFFECT........SLAY........BRAND.", 11, j);
	prt("      mf      cvae      xsqpaefc", 12, j);
	prt("siwdccsossidsahanvudotgddhuoclio", 13, j);
	prt("tnieohtctrnipttmiinmrrnrrraiierl", 14, j);
	prt("rtsxnarelcfgdkcpmldncltggpksdced", 15, j);
	prt_binary(f1, 16, j);

	prt("+------------FLAGS2------------+", 17, j);
	prt("SUST....IMMUN.RESIST............", 18, j);
	prt("      r aefctrpsaefcpfldbc sn   ", 19, j);
	prt("siwdcci clioheatcliooeialoshtncd", 20, j);
	prt("tnieohd ierlrfraierliatrnnnrhehi", 21, j);
	prt("rtsxnae.dcedwlatdcedsrekdfddrxss", 22, j);
	prt_binary(f2, 23, j);

	prt("+------------FLAGS3------------+", 10, j+32);
	prt("fe cnn t      stdrmsiiii d ab   ", 11, j+32);
	prt("aa aoomywhs lleeieihgggg rtgl   ", 12, j+32);
	prt("uu utmacaih eielgggonnnnaaere   ", 13, j+32);
	prt("rr reanurdo vtieeehtrrrrcilas   ", 14, j+32);
	prt("aa algarnew ienpsntsaefctnevs   ", 15, j+32);
	prt_binary(f3, 16, j+32);
}


/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc
{
	int        tval;
	cptr       desc;
} tval_desc;

/*
 * A list of tvals and their textual names
 */
static tval_desc tvals[] =
{
	{ TV_SWORD,             "Sword"                },
	{ TV_POLEARM,           "Polearm"              },
	{ TV_HAFTED,            "Hafted Weapon"        },
	{ TV_BOW,               "Bow"                  },
	{ TV_ARROW,             "Arrows"               },
	{ TV_BOLT,              "Bolts"                },
	{ TV_SHOT,              "Shots"                },
	{ TV_SHIELD,            "Shield"               },
	{ TV_CROWN,             "Crown"                },
	{ TV_HELM,              "Helm"                 },
	{ TV_GLOVES,            "Gloves"               },
	{ TV_BOOTS,             "Boots"                },
	{ TV_CLOAK,             "Cloak"                },
	{ TV_DRAG_ARMOR,        "Dragon Scale Mail"    },
	{ TV_HARD_ARMOR,        "Hard Armor"           },
	{ TV_SOFT_ARMOR,        "Soft Armor"           },
	{ TV_RING,              "Ring"                 },
	{ TV_AMULET,            "Amulet"               },
	{ TV_LITE,              "Lite"                 },
	{ TV_POTION,            "Potion"               },
	{ TV_SCROLL,            "Scroll"               },
	{ TV_WAND,              "Wand"                 },
	{ TV_STAFF,             "Staff"                },
	{ TV_ROD,               "Rod"                  },
	{ TV_LIFE_BOOK,         "Life Spellbook"       },
	{ TV_SORCERY_BOOK,      "Sorcery Spellbook"    },
	{ TV_NATURE_BOOK,       "Nature Spellbook"     },
	{ TV_CHAOS_BOOK,        "Chaos Spellbook"      },
	{ TV_DEATH_BOOK,        "Death Spellbook"      },
	{ TV_TRUMP_BOOK,        "Trump Spellbook"      },
	{ TV_ARCANE_BOOK,       "Arcane Spellbook"     },
	{ TV_ENCHANT_BOOK,      "Craft Spellbook"},
	{ TV_DAEMON_BOOK,       "Daemon Spellbook"},
	{ TV_CRUSADE_BOOK,         "Crusade Spellbook"},
	{ TV_MUSIC_BOOK,        "Music Spellbook"      },
	{ TV_HISSATSU_BOOK,     "Book of Kendo" },
	{ TV_PARCHEMENT,        "Parchement" },
	{ TV_WHISTLE,           "Whistle"	},
	{ TV_SPIKE,             "Spikes"               },
	{ TV_DIGGING,           "Digger"               },
	{ TV_CHEST,             "Chest"                },
	{ TV_CAPTURE,           "Capture Ball"         },
	{ TV_CARD,              "Express Card"         },
	{ TV_FIGURINE,          "Magical Figurine"     },
	{ TV_STATUE,            "Statue"               },
	{ TV_CORPSE,            "Corpse"               },
	{ TV_FOOD,              "Food"                 },
	{ TV_FLASK,             "Flask"                },
	{ TV_JUNK,              "Junk"                 },
	{ TV_SKELETON,          "Skeleton"             },
	{ 0,                    NULL                   }
};


/*
 * Strip an "object name" into a buffer
 */
void strip_name(char *buf, int k_idx)
{
	char *t;

	object_kind *k_ptr = &k_info[k_idx];

	cptr str = (k_name + k_ptr->name);


	/* Skip past leading characters */
	while ((*str == ' ') || (*str == '&')) str++;

	/* Copy useful chars */
	for (t = buf; *str; str++)
	{
#ifdef JP
		if (iskanji(*str)) {*t++ = *str++; *t++ = *str; continue;}
#endif
		if (*str != '~') *t++ = *str;
	}

	/* Terminate the new name */
	*t = '\0';
}


/*
 * Specify tval and sval (type and subtype of object) originally
 * by RAK, heavily modified by -Bernd-
 *
 * This function returns the k_idx of an object type, or zero if failed
 *
 * List up to 50 choices in three columns
 */
static int wiz_create_itemtype(void)
{
	int i, num, max_num;
	int col, row;
	int tval;

	cptr tval_desc;
	char ch;

	int choice[80];

	char buf[160];


	/* Clear screen */
	Term_clear();

	/* Print all tval's and their descriptions */
	for (num = 0; (num < 80) && tvals[num].tval; num++)
	{
		row = 2 + (num % 20);
		col = 20 * (num / 20);
		ch = listsym[num];
		prt(format("[%c] %s", ch, tvals[num].desc), row, col);
	}

	/* Me need to know the maximal possible tval_index */
	max_num = num;

	/* Choose! */
	if (!get_com("Get what type of object? ", &ch, FALSE)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is illegal */
	if ((num < 0) || (num >= max_num)) return (0);

	/* Base object type chosen, fill in tval */
	tval = tvals[num].tval;
	tval_desc = tvals[num].desc;


	/*** And now we go for k_idx ***/

	/* Clear screen */
	Term_clear();

	/* We have to search the whole itemlist. */
	for (num = 0, i = 1; (num < 80) && (i < max_k_idx); i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Analyze matching items */
		if (k_ptr->tval == tval)
		{
			/* Prepare it */
			row = 2 + (num % 20);
			col = 20 * (num / 20);
			ch = listsym[num];
			strcpy(buf,"                    ");

			/* Acquire the "name" of object "i" */
			strip_name(buf, i);

			/* Print it */
			prt(format("[%c] %s", ch, buf), row, col);

			/* Remember the object index */
			choice[num++] = i;
		}
	}

	/* Me need to know the maximal possible remembered object_index */
	max_num = num;

	/* Choose! */
	if (!get_com(format("What Kind of %s? ", tval_desc), &ch, FALSE)) return (0);

	/* Analyze choice */
	for (num = 0; num < max_num; num++)
	{
		if (listsym[num] == ch) break;
	}

	/* Bail out if choice is "illegal" */
	if ((num < 0) || (num >= max_num)) return (0);

	/* And return successful */
	return (choice[num]);
}


/*
 * Tweak an item
 */
static void wiz_tweak_item(object_type *o_ptr)
{
	cptr p;
	char tmp_val[80];


	/* Hack -- leave artifacts alone */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;

	p = "Enter new 'pval' setting: ";
	sprintf(tmp_val, "%d", o_ptr->pval);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->pval = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_a' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_a);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_a = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_h' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_h);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_h = atoi(tmp_val);
	wiz_display_item(o_ptr);

	p = "Enter new 'to_d' setting: ";
	sprintf(tmp_val, "%d", o_ptr->to_d);
	if (!get_string(p, tmp_val, 5)) return;
	o_ptr->to_d = atoi(tmp_val);
	wiz_display_item(o_ptr);
}


/*
 * Apply magic to an item or turn it into an artifact. -Bernd-
 */
static void wiz_reroll_item(object_type *o_ptr)
{
	object_type forge;
	object_type *q_ptr;

	char ch;

	bool changed = FALSE;


	/* Hack -- leave artifacts alone */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;


	/* Get local object */
	q_ptr = &forge;

	/* Copy the object */
	object_copy(q_ptr, o_ptr);


	/* Main loop. Ask for magification and artifactification */
	while (TRUE)
	{
		/* Display full item debug information */
		wiz_display_item(q_ptr);

		/* Ask wizard what to do. */
		if (!get_com("[a]ccept, [w]orthless, [c]ursed, [n]ormal, [g]ood, [e]xcellent, [s]pecial? ", &ch, FALSE))
		{
			/* Preserve wizard-generated artifacts */
			if (artifact_p(q_ptr))
			{
				a_info[q_ptr->name1].cur_num = 0;
				q_ptr->name1 = 0;
			}

			changed = FALSE;
			break;
		}

		/* Create/change it! */
		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		/* Preserve wizard-generated artifacts */
		if (artifact_p(q_ptr))
		{
			a_info[q_ptr->name1].cur_num = 0;
			q_ptr->name1 = 0;
		}

		switch(ch)
		{
			/* Apply bad magic, but first clear object */
			case 'w': case 'W':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, TRUE, TRUE);
				break;
			}
			/* Apply bad magic, but first clear object */
			case 'c': case 'C':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, FALSE, TRUE);
				break;
			}
			/* Apply normal magic, but first clear object */
			case 'n': case 'N':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE, FALSE);
				break;
			}
			/* Apply good magic, but first clear object */
			case 'g': case 'G':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, FALSE, FALSE);
				break;
			}
			/* Apply great magic, but first clear object */
			case 'e': case 'E':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, FALSE, TRUE, TRUE, FALSE);
				break;
			}
			case 's': case 'S':
			{
				object_prep(q_ptr, o_ptr->k_idx);
				apply_magic(q_ptr, dun_level, TRUE, TRUE, TRUE, FALSE);

				/* Failed to create normal artifact; make a random one */
				if (!artifact_p(q_ptr)) create_artifact(q_ptr, FALSE);
				break;
			}
		}
		q_ptr->iy = o_ptr->iy;
		q_ptr->ix = o_ptr->ix;
		q_ptr->next_o_idx = o_ptr->next_o_idx;
		q_ptr->marked = o_ptr->marked;
	}


	/* Notice change */
	if (changed)
	{
		/* Apply changes */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}
}



/*
 * Try to create an item again. Output some statistics.    -Bernd-
 *
 * The statistics are correct now.  We acquire a clean grid, and then
 * repeatedly place an object in this grid, copying it into an item
 * holder, and then deleting the object.  We fiddle with the artifact
 * counter flags to prevent weirdness.  We use the items to collect
 * statistics on item creation relative to the initial item.
 */
static void wiz_statistics(object_type *o_ptr)
{
	u32b i, matches, better, worse, other, correct;

	u32b test_roll = 1000000;

	char ch;
	cptr quality;

	bool good, great;

	object_type forge;
	object_type	*q_ptr;

	cptr q = "Rolls: %ld  Correct: %ld  Matches: %ld  Better: %ld  Worse: %ld  Other: %ld";

	cptr p = "Enter number of items to roll: ";
	char tmp_val[80];


	/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
	if (artifact_p(o_ptr)) a_info[o_ptr->name1].cur_num = 0;


	/* Interact */
	while (TRUE)
	{
		cptr pmt = "Roll for [n]ormal, [g]ood, or [e]xcellent treasure? ";

		/* Display item */
		wiz_display_item(o_ptr);

		/* Get choices */
		if (!get_com(pmt, &ch, FALSE)) break;

		if (ch == 'n' || ch == 'N')
		{
			good = FALSE;
			great = FALSE;
			quality = "normal";
		}
		else if (ch == 'g' || ch == 'G')
		{
			good = TRUE;
			great = FALSE;
			quality = "good";
		}
		else if (ch == 'e' || ch == 'E')
		{
			good = TRUE;
			great = TRUE;
			quality = "excellent";
		}
		else
		{
			good = FALSE;
			great = FALSE;
			break;
		}

		sprintf(tmp_val, "%ld", test_roll);
		if (get_string(p, tmp_val, 10)) test_roll = atol(tmp_val);
		test_roll = MAX(1, test_roll);

		/* Let us know what we are doing */
		msg_format("Creating a lot of %s items. Base level = %d.",
					  quality, dun_level);
		msg_print(NULL);

		/* Set counters to zero */
		correct = matches = better = worse = other = 0;

		/* Let's rock and roll */
		for (i = 0; i <= test_roll; i++)
		{
			/* Output every few rolls */
			if ((i < 100) || (i % 100 == 0))
			{
				/* Do not wait */
				inkey_scan = TRUE;

				/* Allow interupt */
				if (inkey())
				{
					/* Flush */
					flush();

					/* Stop rolling */
					break;
				}

				/* Dump the stats */
				prt(format(q, i, correct, matches, better, worse, other), 0, 0);
				Term_fresh();
			}


			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Create an object */
			make_object(q_ptr, good, great);


			/* XXX XXX XXX Mega-Hack -- allow multiple artifacts */
			if (artifact_p(q_ptr)) a_info[q_ptr->name1].cur_num = 0;


			/* Test for the same tval and sval. */
			if ((o_ptr->tval) != (q_ptr->tval)) continue;
			if ((o_ptr->sval) != (q_ptr->sval)) continue;

			/* One more correct item */
			correct++;

			/* Check for match */
			if ((q_ptr->pval == o_ptr->pval) &&
				 (q_ptr->to_a == o_ptr->to_a) &&
				 (q_ptr->to_h == o_ptr->to_h) &&
				 (q_ptr->to_d == o_ptr->to_d) &&
				 (q_ptr->name1 == o_ptr->name1))
			{
				matches++;
			}

			/* Check for better */
			else if ((q_ptr->pval >= o_ptr->pval) &&
						(q_ptr->to_a >= o_ptr->to_a) &&
						(q_ptr->to_h >= o_ptr->to_h) &&
						(q_ptr->to_d >= o_ptr->to_d))
			{
				better++;
			}

			/* Check for worse */
			else if ((q_ptr->pval <= o_ptr->pval) &&
						(q_ptr->to_a <= o_ptr->to_a) &&
						(q_ptr->to_h <= o_ptr->to_h) &&
						(q_ptr->to_d <= o_ptr->to_d))
			{
				worse++;
			}

			/* Assume different */
			else
			{
				other++;
			}
		}

		/* Final dump */
		msg_format(q, i, correct, matches, better, worse, other);
		msg_print(NULL);
	}


	/* Hack -- Normally only make a single artifact */
	if (artifact_p(o_ptr)) a_info[o_ptr->name1].cur_num = 1;
}


/*
 * Change the quantity of a the item
 */
static void wiz_quantity_item(object_type *o_ptr)
{
	int         tmp_int, tmp_qnt;

	char        tmp_val[100];


	/* Never duplicate artifacts */
	if (artifact_p(o_ptr) || o_ptr->art_name) return;

	/* Store old quantity. -LM- */
	tmp_qnt = o_ptr->number;

	/* Default */
	sprintf(tmp_val, "%d", o_ptr->number);

	/* Query */
	if (get_string("Quantity: ", tmp_val, 2))
	{
		/* Extract */
		tmp_int = atoi(tmp_val);

		/* Paranoia */
		if (tmp_int < 1) tmp_int = 1;
		if (tmp_int > 99) tmp_int = 99;

		/* Accept modifications */
		o_ptr->number = tmp_int;

		/* Hack -- rod pvals must change if the number in the stack does. -LM- */
		if (o_ptr->tval == TV_ROD)
			o_ptr->pval = o_ptr->pval * o_ptr->number / tmp_qnt;
	}
}



/*
 * Play with an item. Options include:
 *   - Output statistics (via wiz_roll_item)
 *   - Reroll item (via wiz_reroll_item)
 *   - Change properties (via wiz_tweak_item)
 *   - Change the number of items (via wiz_quantity_item)
 */
static void do_cmd_wiz_play(void)
{
	int item;

	object_type	forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char ch;

	bool changed;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
	q = "Play with which object? ";
	s = "You have nothing to play with.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

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


	/* The item was not changed */
	changed = FALSE;


	/* Save the screen */
	screen_save();


	/* Get local object */
	q_ptr = &forge;

	/* Copy object */
	object_copy(q_ptr, o_ptr);


	/* The main loop */
	while (TRUE)
	{
		/* Display the item */
		wiz_display_item(q_ptr);

		/* Get choice */
		if (!get_com("[a]ccept [s]tatistics [r]eroll [t]weak [q]uantity? ", &ch, FALSE))
		{
			changed = FALSE;
			break;
		}

		if (ch == 'A' || ch == 'a')
		{
			changed = TRUE;
			break;
		}

		if (ch == 's' || ch == 'S')
		{
			wiz_statistics(q_ptr);
		}

		if (ch == 'r' || ch == 'r')
		{
			wiz_reroll_item(q_ptr);
		}

		if (ch == 't' || ch == 'T')
		{
			wiz_tweak_item(q_ptr);
		}

		if (ch == 'q' || ch == 'Q')
		{
			wiz_quantity_item(q_ptr);
		}
	}


	/* Restore the screen */
	screen_load();


	/* Accept change */
	if (changed)
	{
		/* Message */
		msg_print("Changes accepted.");

		/* Change */
		object_copy(o_ptr, q_ptr);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);
	}

	/* Ignore change */
	else
	{
		msg_print("Changes ignored.");
	}
}


/*
 * Wizard routine for creating objects		-RAK-
 * Heavily modified to allow magification and artifactification  -Bernd-
 *
 * Note that wizards cannot create objects on top of other objects.
 *
 * Hack -- this routine always makes a "dungeon object", and applies
 * magic to it, and attempts to decline cursed items.
 */
static void wiz_create_item(void)
{
	object_type	forge;
	object_type *q_ptr;

	int k_idx;


	/* Save the screen */
	screen_save();

	/* Get object base type */
	k_idx = wiz_create_itemtype();

	/* Restore the screen */
	screen_load();


	/* Return if failed */
	if (!k_idx) return;

	if (k_info[k_idx].gen_flags & TRG_INSTA_ART)
	{
		int i;

		/* Artifactify */
		for (i = 1; i < max_a_idx; i++)
		{
			/* Ignore incorrect tval */
			if (a_info[i].tval != k_info[k_idx].tval) continue;

			/* Ignore incorrect sval */
			if (a_info[i].sval != k_info[k_idx].sval) continue;

			/* Create this artifact */
                        create_named_art(i, py, px);

                        /* All done */
                        msg_print("Allocated(INSTA_ART).");

                        return;
		}
	}

	/* Get local object */
	q_ptr = &forge;

	/* Create the item */
	object_prep(q_ptr, k_idx);

        /* Apply magic */
        apply_magic(q_ptr, dun_level, FALSE, FALSE, FALSE, FALSE);

	/* Drop the object from heaven */
	(void)drop_near(q_ptr, -1, py, px);

	/* All done */
	msg_print("Allocated.");
}


/*
 * Cure everything instantly
 */
static void do_cmd_wiz_cure_all(void)
{
	/* Restore stats */
	(void)res_stat(A_STR);
	(void)res_stat(A_INT);
	(void)res_stat(A_WIS);
	(void)res_stat(A_CON);
	(void)res_stat(A_DEX);
	(void)res_stat(A_CHR);

	/* Restore the level */
	(void)restore_level();

	/* Heal the player */
	p_ptr->chp = p_ptr->mhp;
	p_ptr->chp_frac = 0;

	/* Restore mana */
	if (p_ptr->csp < p_ptr->msp)
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;
	}

	/* Cure stuff */
	(void)set_blind(0);
	(void)set_confused(0);
	(void)set_poisoned(0);
	(void)set_afraid(0);
	(void)set_paralyzed(0);
	(void)set_image(0);
	(void)set_stun(0);
	(void)set_cut(0);
	(void)set_slow(0, TRUE);

	/* No longer hungry */
	(void)set_food(PY_FOOD_MAX - 1);

	/* Redraw everything */
	do_cmd_redraw();
}


/*
 * Go to any level
 */
static void do_cmd_wiz_jump(void)
{
	/* Ask for level */
	if (command_arg <= 0)
	{
		char	ppp[80];

		char	tmp_val[160];

		/* Prompt */
		sprintf(ppp, "Jump which dungeon : ");

		/* Default */
		sprintf(tmp_val, "%d", dungeon_type);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 2)) return;

		dungeon_type = atoi(tmp_val);
		if (!d_info[dungeon_type].maxdepth || (dungeon_type > max_d_idx)) dungeon_type = DUNGEON_ANGBAND;

		/* Prompt */
		sprintf(ppp, "Jump to level (0, %d-%d): ", d_info[dungeon_type].mindepth, d_info[dungeon_type].maxdepth);

		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}

	/* Paranoia */
	if (command_arg < d_info[dungeon_type].mindepth) command_arg = 0;

	/* Paranoia */
	if (command_arg > d_info[dungeon_type].maxdepth) command_arg = d_info[dungeon_type].maxdepth;

	/* Accept request */
	msg_format("You jump to dungeon level %d.", command_arg);

	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	if (!dun_level) dungeon_type = 0;
	p_ptr->inside_arena = FALSE;

	leave_quest_check();

	if (record_stair) do_cmd_write_nikki(NIKKI_WIZ_TELE,0,NULL);

	p_ptr->inside_quest = 0;
	p_ptr->leftbldg = FALSE;
	energy_use = 0;

        /* Prevent energy_need from being too lower than 0 */
        p_ptr->energy_need = 0;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


/*
 * Become aware of a lot of objects
 */
static void do_cmd_wiz_learn(void)
{
	int i;

	object_type forge;
	object_type *q_ptr;

	/* Scan every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Induce awareness */
		if (k_ptr->level <= command_arg)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare object */
			object_prep(q_ptr, i);

			/* Awareness */
			object_aware(q_ptr);
		}
	}
}


/*
 * Summon some creatures
 */
static void do_cmd_wiz_summon(int num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		(void)summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named(int r_idx)
{
	int i, x, y;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= max_r_idx) return;

	/* Try 10 times */
	for (i = 0; i < 10; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&y, &x, py, px, d, 0);

		/* Require empty grids */
		if (!cave_empty_bold(y, x)) continue;

		/* Place it (allow groups) */
		if (place_monster_aux(0, y, x, r_idx, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP))) break;
	}
}


/*
 * Summon a creature of the specified type
 *
 * XXX XXX XXX This function is rather dangerous
 */
static void do_cmd_wiz_named_friendly(int r_idx)
{
	(void) summon_named_creature(0, py, px, r_idx, (PM_ALLOW_SLEEP | PM_ALLOW_GROUP | PM_FORCE_PET));
}



/*
 * Hack -- Delete all nearby monsters
 */
static void do_cmd_wiz_zap(void)
{
	int i;


	/* Genocide everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Delete nearby monsters */
		if (m_ptr->cdis <= MAX_SIGHT)
		{
			if (i == p_ptr->riding)
			{
				rakuba(-1, FALSE);
				p_ptr->redraw |= (PR_EXTRA);
			}
			delete_monster_idx(i);
		}
	}
}


/*
 * Hack -- Delete all monsters
 */
static void do_cmd_wiz_zap_all(void)
{
	int i;

	/* Genocide everyone */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		if (i == p_ptr->riding)
		{
			rakuba(-1, FALSE);
			p_ptr->redraw |= (PR_EXTRA);
		}

		/* Delete this monster */
		delete_monster_idx(i);
	}
}


#ifdef ALLOW_SPOILERS

/*
 * External function
 */
extern void do_cmd_spoilers(void);

#endif /* ALLOW_SPOILERS */



/*
 * Hack -- declare external function
 */
extern void do_cmd_debug(void);



/*
 * Ask for and parse a "debug command"
 * The "command_arg" may have been set.
 */
void do_cmd_debug(void)
{
	int     x, y;
	char    cmd;


	/* Get a "debug command" */
	get_com("Debug Command: ", &cmd, FALSE);

	/* Analyze the command */
	switch (cmd)
	{
		/* Nothing */
		case ESCAPE:
		case ' ':
		case '\n':
		case '\r':
		break;


#ifdef ALLOW_SPOILERS

		/* Hack -- Generate Spoilers */
		case '"':
		do_cmd_spoilers();
		break;

#endif /* ALLOW_SPOILERS */


		/* Hack -- Help */
		case '?':
		do_cmd_help();
		break;


		/* Cure all maladies */
		case 'a':
		do_cmd_wiz_cure_all();
		break;

		/* Know alignment */
		case 'A':
		msg_format("Your alignment is %d.", p_ptr->align);
		break;

		/* Teleport to target */
		case 'b':
		do_cmd_wiz_bamf();
		break;

		case 'B':
		battle_monsters();
		break;

		/* Create any object */
		case 'c':
		wiz_create_item();
		break;

		/* Create a named artifact */
		case 'C':
		wiz_create_named_art(command_arg);
		break;

		/* Detect everything */
		case 'd':
		detect_all(DETECT_RAD_ALL*3);
		break;

		/* Dimension_door */
		case 'D':
		wiz_dimension_door();
		break;

		/* Edit character */
		case 'e':
		do_cmd_wiz_change();
		break;

		/* View item info */
		case 'f':
		identify_fully(FALSE, FALSE);
		break;

		/* Good Objects */
		case 'g':
		if (command_arg <= 0) command_arg = 1;
		acquirement(py, px, command_arg, FALSE, TRUE);
		break;

		/* Hitpoint rerating */
		case 'h':
		do_cmd_rerate(TRUE); break;

#ifdef MONSTER_HORDES
		case 'H':
		do_cmd_summon_horde(); break;
#endif /* MONSTER_HORDES */

		/* Identify */
		case 'i':
		(void)ident_spell(FALSE, FALSE);
		break;

		/* Go up or down in the dungeon */
		case 'j':
		do_cmd_wiz_jump();
		break;

		/* Self-Knowledge */
		case 'k':
			self_knowledge();
			break;

		/* Learn about objects */
		case 'l':
			do_cmd_wiz_learn();
			break;

		/* Magic Mapping */
		case 'm':
			map_area(DETECT_RAD_ALL);
			break;

		/* Mutation */
		case 'M':
			(void)gain_random_mutation(command_arg);
			break;

		/* Specific reward */
		case 'r':
			(void)gain_level_reward(command_arg);
			break;

		/* Summon _friendly_ named monster */
		case 'N':
			do_cmd_wiz_named_friendly(command_arg);
			break;

		/* Summon Named Monster */
		case 'n':
			do_cmd_wiz_named(command_arg);
			break;

		/* Object playing routines */
		case 'o':
			do_cmd_wiz_play();
			break;

		/* Phase Door */
		case 'p':
			teleport_player(10);
			break;

#if 0
		/* Complete a Quest -KMW- */
		case 'q':
		{
			for (i = 0; i < max_quests; i++)
			{
				if (p_ptr->quest[i].status == QUEST_STATUS_TAKEN)
				{
					p_ptr->quest[i].status++;
					msg_print("Completed Quest");
					msg_print(NULL);
					break;
				}
			}
			if (i == max_quests)
			{
				msg_print("No current quest");
				msg_print(NULL);
			}
			break;
		}
#endif

		/* Make every dungeon square "known" to test streamers -KMW- */
		case 'u':
		{
			for(y = 0; y < cur_hgt; y++)
			{
				for(x = 0; x < cur_wid; x++)
				{
					cave[y][x].info |= (CAVE_GLOW | CAVE_MARK);
				}
			}
			wiz_lite(TRUE, FALSE);
			break;
		}

		/* Summon Random Monster(s) */
		case 's':
			if (command_arg <= 0) command_arg = 1;
			do_cmd_wiz_summon(command_arg);
			break;

		/* Teleport */
		case 't':
			teleport_player(100);
			break;

		/* Very Good Objects */
		case 'v':
			if (command_arg <= 0) command_arg = 1;
			acquirement(py, px, command_arg, TRUE, TRUE);
			break;

		/* Wizard Light the Level */
		case 'w':
		wiz_lite(TRUE, (bool)(p_ptr->pclass == CLASS_NINJA));
		break;

		/* Increase Experience */
		case 'x':
		if (command_arg)
		{
			gain_exp(command_arg);
		}
		else
		{
			gain_exp(p_ptr->exp + 1);
		}
		break;

		/* Zap Monsters (Genocide) */
		case 'z':
		do_cmd_wiz_zap();
		break;

		case 'Z':
		do_cmd_wiz_zap_all();
		break;

		/* Hack -- whatever I desire */
		case '_':
		do_cmd_wiz_hack_ben();
		break;

		/* Not a Wizard Command */
		default:
		msg_print("That is not a valid debug command.");
		break;
	}
}


#else

#ifdef MACINTOSH
static int i = 0;
#endif

#endif

