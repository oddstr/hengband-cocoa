/* File: dungeonc */

/* Purpose: Angband game engine */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#define TY_CURSE_CHANCE 200
#define CHAINSWORD_NOISE 100

static bool load = TRUE;
static int wild_regen = 20;

/*
 * Return a "feeling" (or NULL) about an item.  Method 1 (Heavy).
 */
static byte value_check_aux1(object_type *o_ptr)
{
	/* Artifacts */
	if (artifact_p(o_ptr) || o_ptr->art_name)
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) return FEEL_TERRIBLE;

		/* Normal */
		return FEEL_SPECIAL;
	}

	/* Ego-Items */
	if (ego_item_p(o_ptr))
	{
		/* Cursed/Broken */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) return FEEL_WORTHLESS;

		/* Normal */
		return FEEL_EXCELLENT;
	}

	/* Cursed items */
	if (cursed_p(o_ptr)) return FEEL_CURSED;

	/* Broken items */
	if (broken_p(o_ptr)) return FEEL_BROKEN;

	if ((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET)) return FEEL_AVERAGE;

	/* Good "armor" bonus */
	if (o_ptr->to_a > 0) return FEEL_GOOD;

	/* Good "weapon" bonus */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_GOOD;

	/* Default to "average" */
	return FEEL_AVERAGE;
}


/*
 * Return a "feeling" (or NULL) about an item.  Method 2 (Light).
 */
static byte value_check_aux2(object_type *o_ptr)
{
	/* Cursed items (all of them) */
	if (cursed_p(o_ptr)) return FEEL_CURSED;

	/* Broken items (all of them) */
	if (broken_p(o_ptr)) return FEEL_BROKEN;

	/* Artifacts -- except cursed/broken ones */
	if (artifact_p(o_ptr) || o_ptr->art_name) return FEEL_UNCURSED;

	/* Ego-Items -- except cursed/broken ones */
	if (ego_item_p(o_ptr)) return FEEL_UNCURSED;

	/* Good armor bonus */
	if (o_ptr->to_a > 0) return FEEL_UNCURSED;

	/* Good weapon bonuses */
	if (o_ptr->to_h + o_ptr->to_d > 0) return FEEL_UNCURSED;

	/* No feeling */
	return FEEL_NONE;
}



static void sense_inventory_aux(int slot, bool heavy)
{
	byte        feel;
	object_type *o_ptr = &inventory[slot];
	char        o_name[MAX_NLEN];
	int idx;

	/* We know about it already, do not tell us again */
	if (o_ptr->ident & (IDENT_SENSE))return;

	/* It is fully known, no information needed */
	if (object_known_p(o_ptr)) return;

	/* Check for a feeling */
	feel = (heavy ? value_check_aux1(o_ptr) : value_check_aux2(o_ptr));

	/* Skip non-feelings */
	if (!feel) return;

	/* Bad luck */
	if ((p_ptr->muta3 & MUT3_BAD_LUCK) && !randint0(13))
	{
		switch (feel)
		{
			case FEEL_TERRIBLE:
			{
				feel = FEEL_SPECIAL;
				break;
			}
			case FEEL_WORTHLESS:
			{
				feel = FEEL_EXCELLENT;
				break;
			}
			case FEEL_CURSED:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_GOOD : FEEL_AVERAGE;
				else
					feel = FEEL_UNCURSED;
				break;
			}
			case FEEL_AVERAGE:
			{
				feel = randint0(2) ? FEEL_CURSED : FEEL_GOOD;
				break;
			}
			case FEEL_GOOD:
			{
				if (heavy)
					feel = randint0(3) ? FEEL_CURSED : FEEL_AVERAGE;
				else
					feel = FEEL_CURSED;
				break;
			}
			case FEEL_EXCELLENT:
			{
				feel = FEEL_WORTHLESS;
				break;
			}
			case FEEL_SPECIAL:
			{
				feel = FEEL_TERRIBLE;
				break;
			}
		}
	}

	/* Stop everything */
	if (disturb_minor) disturb(0, 0);

	/* Get an object description */
	object_desc(o_name, o_ptr, FALSE, 0);

	/* Message (equipment) */
	if (slot >= INVEN_RARM)
	{
#ifdef JP
msg_format("%s%s(%c)��%s�Ȥ�������������...",
describe_use(slot),o_name, index_to_label(slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) you are %s %s %s...",
			   o_name, index_to_label(slot), describe_use(slot),
			   ((o_ptr->number == 1) ? "is" : "are"),
				   game_inscriptions[feel]);
#endif

	}

	/* Message (inventory) */
	else
	{
#ifdef JP
msg_format("���å������%s(%c)��%s�Ȥ�������������...",
o_name, index_to_label(slot),game_inscriptions[feel]);
#else
		msg_format("You feel the %s (%c) in your pack %s %s...",
			   o_name, index_to_label(slot),
			   ((o_ptr->number == 1) ? "is" : "are"),
				   game_inscriptions[feel]);
#endif

	}

	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* Set the "inscription" */
	o_ptr->feeling = feel;

	/* Auto-inscription/destroy */
	idx = is_autopick(o_ptr);
	auto_inscribe_item(slot, idx);
	if (destroy_feeling)
		auto_destroy_item(slot, idx);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);
}



/*
 * Sense the inventory
 *
 *   Class 0 = Warrior --> fast and heavy
 *   Class 1 = Mage    --> slow and light
 *   Class 2 = Priest  --> fast but light
 *   Class 3 = Rogue   --> okay and heavy
 *   Class 4 = Ranger  --> slow but heavy  (changed!)
 *   Class 5 = Paladin --> slow but heavy
 */
static void sense_inventory1(void)
{
	int         i;
	int         plev = p_ptr->lev;
	bool        heavy = FALSE;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_SMITH:
		{
			/* Good sensing */
			if (0 != randint0(6000L / (plev * plev + 50))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		{
			/* Good (light) sensing */
			if (0 != randint0(10000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_ROGUE:
		case CLASS_NINJA:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_PALADIN:
		{
			/* Bad sensing */
			if (0 != randint0(77777L / (plev * plev + 40))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		{
			/* Bad sensing */
			if (0 != randint0(75000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_MIRROR_MASTER:
		{
			/* Bad sensing */
			if (0 != randint0(55000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_CHAOS_WARRIOR:
		{
			/* Bad sensing */
			if (0 != randint0(80000L / (plev * plev + 40))) return;

			/* Changed! */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		{
			/* Okay sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}

		case CLASS_IMITATOR:
		case CLASS_BLUE_MAGE:
		{
			/* Bad sensing */
			if (0 != randint0(55000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_BEASTMASTER:
		{
			/* Bad sensing */
			if (0 != randint0(65000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}
		case CLASS_BERSERKER:
		{
			/* Heavy sensing */
			heavy = TRUE;

			/* Done */
			break;
		}
	}

	if (compare_virtue(V_KNOWLEDGE, 100, VIRTUE_LARGE)) heavy = TRUE;

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_SHOT:
			case TV_ARROW:
			case TV_BOLT:
			case TV_BOW:
			case TV_DIGGING:
			case TV_HAFTED:
			case TV_POLEARM:
			case TV_SWORD:
			case TV_BOOTS:
			case TV_GLOVES:
			case TV_HELM:
			case TV_CROWN:
			case TV_SHIELD:
			case TV_CLOAK:
			case TV_SOFT_ARMOR:
			case TV_HARD_ARMOR:
			case TV_DRAG_ARMOR:
			case TV_CARD:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		/* Good luck */
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && !randint0(13))
		{
			heavy = TRUE;
		}

		sense_inventory_aux(i, heavy);
	}
}


static void sense_inventory2(void)
{
	int         i;
	int         plev = p_ptr->lev;
	object_type *o_ptr;


	/*** Check for "sensing" ***/

	/* No sensing when confused */
	if (p_ptr->confused) return;

	/* Analyze the class */
	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		case CLASS_ARCHER:
		case CLASS_SAMURAI:
		case CLASS_CAVALRY:
		case CLASS_BERSERKER:
		{
			return;
		}

		case CLASS_SMITH:
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_IMITATOR:
		case CLASS_BEASTMASTER:
		case CLASS_NINJA:
		{
			/* Very bad (light) sensing */
			if (0 != randint0(240000L / (plev + 5))) return;

			/* Done */
			break;
		}

		case CLASS_RANGER:
		case CLASS_WARRIOR_MAGE:
		case CLASS_RED_MAGE:
		case CLASS_MONK:
		{
			/* Bad sensing */
			if (0 != randint0(95000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_ROGUE:
		case CLASS_FORCETRAINER:
		case CLASS_MINDCRAFTER:
		{
			/* Good sensing */
			if (0 != randint0(20000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		case CLASS_MAGIC_EATER:
		case CLASS_MIRROR_MASTER:
		case CLASS_BLUE_MAGE:
		{
			/* Good sensing */
			if (0 != randint0(9000L / (plev * plev + 40))) return;

			/* Done */
			break;
		}

		case CLASS_TOURIST:
		{
			/* Good sensing */
			if (0 != randint0(20000L / ((plev+50)*(plev+50)))) return;

			/* Done */
			break;
		}
	}

	/*** Sense everything ***/

	/* Check everything */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		bool okay = FALSE;

		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Valid "tval" codes */
		switch (o_ptr->tval)
		{
			case TV_RING:
			case TV_AMULET:
			case TV_LITE:
			case TV_FIGURINE:
			{
				okay = TRUE;
				break;
			}
		}

		/* Skip non-sense machines */
		if (!okay) continue;

		/* Occasional failure on inventory items */
		if ((i < INVEN_RARM) && (0 != randint0(5))) continue;

		sense_inventory_aux(i, TRUE);
	}
}



/*
 * Go to any level (ripped off from wiz_jump)
 */
static void pattern_teleport(void)
{
	int min_level = 0;
	int max_level = 99;

	/* Ask for level */
#ifdef JP
if (get_check("¾�γ��˥ƥ�ݡ��Ȥ��ޤ�����"))
#else
	if (get_check("Teleport level? "))
#endif

	{
		char	ppp[80];
		char	tmp_val[160];

		/* Only downward in ironman mode */
		if (ironman_downward)
			min_level = dun_level;

		/* Maximum level */
		if (dungeon_type == DUNGEON_ANGBAND)
		{
			if (dun_level > 100)
				max_level = MAX_DEPTH - 1;
			else if (dun_level == 100)
				max_level = 100;
		}
		else
		{
			max_level = d_info[dungeon_type].maxdepth;
			min_level = d_info[dungeon_type].mindepth;
		}

		/* Prompt */
#ifdef JP
sprintf(ppp, "�ƥ�ݡ�����:(%d-%d)", min_level, max_level);
#else
		sprintf(ppp, "Teleport to level (%d-%d): ", min_level, max_level);
#endif


		/* Default */
		sprintf(tmp_val, "%d", dun_level);

		/* Ask for a level */
		if (!get_string(ppp, tmp_val, 10)) return;

		/* Extract request */
		command_arg = atoi(tmp_val);
	}
#ifdef JP
else if (get_check("�̾�ƥ�ݡ��ȡ�"))
#else
	else if (get_check("Normal teleport? "))
#endif

	{
		teleport_player(200);
		return;
	}
	else
	{
		return;
	}

	/* Paranoia */
	if (command_arg < min_level) command_arg = min_level;

	/* Paranoia */
	if (command_arg > max_level) command_arg = max_level;

	/* Accept request */
#ifdef JP
msg_format("%d ���˥ƥ�ݡ��Ȥ��ޤ�����", command_arg);
#else
	msg_format("You teleport to dungeon level %d.", command_arg);
#endif


	if (autosave_l) do_cmd_save_game(TRUE);

	/* Change level */
	dun_level = command_arg;

	leave_quest_check();

	if (record_stair) do_cmd_write_nikki(NIKKI_PAT_TELE,0,NULL);

	p_ptr->inside_quest = 0;
	p_ptr->leftbldg = FALSE;
	energy_use = 0;

	/* Leaving */
	p_ptr->leaving = TRUE;
}


static void wreck_the_pattern(void)
{
	int to_ruin = 0, r_y, r_x;

	if (cave[py][px].feat == FEAT_PATTERN_XTRA2)
	{
		/* Ruined already */
		return;
	}

#ifdef JP
msg_print("�ѥ�������Ǳ����Ƥ��ޤä���");
msg_print("�������������������ä���");
#else
	msg_print("You bleed on the Pattern!");
	msg_print("Something terrible happens!");
#endif


	if (!p_ptr->invuln)
#ifdef JP
take_hit(DAMAGE_NOESCAPE, damroll(10, 8), "�ѥ�����»��", -1);
#else
		take_hit(DAMAGE_NOESCAPE, damroll(10, 8), "corrupting the Pattern", -1);
#endif


	to_ruin = randint1(45) + 35;

	while (to_ruin--)
	{
		scatter(&r_y, &r_x, py, px, 4, 0);

		if ((cave[r_y][r_x].feat >= FEAT_PATTERN_START) &&
		    (cave[r_y][r_x].feat < FEAT_PATTERN_XTRA2))
		{
			cave_set_feat(r_y, r_x, FEAT_PATTERN_XTRA2);
		}
	}

	cave_set_feat(py, px, FEAT_PATTERN_XTRA2);
}


/* Returns TRUE if we are on the Pattern... */
static bool pattern_effect(void)
{
	if ((cave[py][px].feat < FEAT_PATTERN_START) ||
	    (cave[py][px].feat > FEAT_PATTERN_XTRA2))
		return FALSE;

	if ((prace_is_(RACE_AMBERITE)) &&
	    (p_ptr->cut > 0) && one_in_(10))
	{
		wreck_the_pattern();
	}

	if (cave[py][px].feat == FEAT_PATTERN_END)
	{
		(void)set_poisoned(0);
		(void)set_image(0);
		(void)set_stun(0);
		(void)set_cut(0);
		(void)set_blind(0);
		(void)set_afraid(0);
		(void)do_res_stat(A_STR);
		(void)do_res_stat(A_INT);
		(void)do_res_stat(A_WIS);
		(void)do_res_stat(A_DEX);
		(void)do_res_stat(A_CON);
		(void)do_res_stat(A_CHR);
		(void)restore_level();
		(void)hp_player(1000);
		cave_set_feat(py, px, FEAT_PATTERN_OLD);
#ifdef JP
msg_print("�֥ѥ�����פΤ�����ʬ��¾����ʬ��궯�ϤǤʤ��褦����");
#else
		msg_print("This section of the Pattern looks less powerful.");
#endif

	}


	/*
	 * We could make the healing effect of the
	 * Pattern center one-time only to avoid various kinds
	 * of abuse, like luring the win monster into fighting you
	 * in the middle of the pattern...
	 */

	else if (cave[py][px].feat == FEAT_PATTERN_OLD)
	{
		/* No effect */
	}
	else if (cave[py][px].feat == FEAT_PATTERN_XTRA1)
	{
		pattern_teleport();
	}
	else if (cave[py][px].feat == FEAT_PATTERN_XTRA2)
	{
		if (!p_ptr->invuln)
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 200, "���줿�֥ѥ�����פ��⤤�����᡼��", -1);
#else
		take_hit(DAMAGE_NOESCAPE, 200, "walking the corrupted Pattern", -1);
#endif

	}
	else
	{
		if ((prace_is_(RACE_AMBERITE)) && !one_in_(2))
			return TRUE;
		else if (!p_ptr->invuln)
#ifdef JP
take_hit(DAMAGE_NOESCAPE, damroll(1,3), "�֥ѥ�����פ��⤤�����᡼��", -1);
#else
			take_hit(DAMAGE_NOESCAPE, damroll(1, 3), "walking the Pattern", -1);
#endif

	}

	return TRUE;
}





/*
 * Regenerate hit points				-RAK-
 */
static void regenhp(int percent)
{
	s32b    new_chp, new_chp_frac;
	int     old_chp;

	if (p_ptr->special_defense & KATA_KOUKIJIN) return;
	if (p_ptr->action == ACTION_HAYAGAKE) return;
	/* Save the old hitpoints */
	old_chp = p_ptr->chp;

	/* Extract the new hitpoints */
	new_chp = ((long)p_ptr->mhp) * percent + PY_REGEN_HPBASE;
	p_ptr->chp += (s16b)(new_chp >> 16);   /* div 65536 */

	/* check for overflow */
	if ((p_ptr->chp < 0) && (old_chp > 0)) p_ptr->chp = MAX_SHORT;
	new_chp_frac = (new_chp & 0xFFFF) + p_ptr->chp_frac;	/* mod 65536 */
	if (new_chp_frac >= 0x10000L)
	{
		p_ptr->chp_frac = (u16b)(new_chp_frac - 0x10000L);
		p_ptr->chp++;
	}
	else
	{
		p_ptr->chp_frac = (u16b)new_chp_frac;
	}

	/* Fully healed */
	if (p_ptr->chp >= p_ptr->mhp)
	{
		p_ptr->chp = p_ptr->mhp;
		p_ptr->chp_frac = 0;
	}

	/* Notice changes */
	if (old_chp != p_ptr->chp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		wild_regen = 20;
	}
}


/*
 * Regenerate mana points				-RAK-
 */
static void regenmana(int percent)
{
	s32b        new_mana, new_mana_frac;
	int                   old_csp;
	bool    old_csp_msp = (p_ptr->csp > p_ptr->msp);

	if (p_ptr->special_defense & KATA_KOUKIJIN) return;
	if ((p_ptr->pclass == CLASS_SAMURAI) && (p_ptr->regenerate)) percent /= 2;
	old_csp = p_ptr->csp;
	new_mana = ((long)p_ptr->msp) * percent + PY_REGEN_MNBASE;
	if (old_csp_msp && (new_mana > 0))
	{
		new_mana *= 32;
		p_ptr->csp--;
		p_ptr->csp -= (s16b)(new_mana >> 16);	/* div 65536 */
		new_mana_frac = p_ptr->csp_frac + 0x10000L - (new_mana & 0xFFFF);
	}
	else
	{
		if (old_csp_msp) new_mana += ((((long)p_ptr->msp) * percent + PY_REGEN_MNBASE) * 32);
		p_ptr->csp += (s16b)(new_mana >> 16);	/* div 65536 */

		new_mana_frac = (new_mana & 0xFFFF) + p_ptr->csp_frac;	/* mod 65536 */
	}
	if (new_mana_frac >= 0x10000L)
	{
		p_ptr->csp_frac = (u16b)(new_mana_frac - 0x10000L);
		p_ptr->csp++;
	}
	else
	{
		p_ptr->csp_frac = (u16b)(new_mana_frac);
	}

	/* check for overflow */
	if (p_ptr->csp < 0)
	{
		p_ptr->csp = 0;
		p_ptr->csp_frac = 0;
	}

	/* Must set frac to zero even if equal */
	if ((old_csp_msp && p_ptr->csp < p_ptr->msp) || (!old_csp_msp && p_ptr->csp >= p_ptr->msp))
	{
		p_ptr->csp = p_ptr->msp;
		p_ptr->csp_frac = 0;
	}

	/* Redraw mana */
	if (old_csp != p_ptr->csp)
	{
		/* Redraw */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);

		wild_regen = 20;
	}
}



/*
 * Regenerate magic
 */
static void regenmagic(int percent)
{
	s32b        new_mana;
	int i;

	for (i = 0; i < EATER_EXT*2; i++)
	{
		if (!p_ptr->magic_num2[i]) continue;
		if (p_ptr->magic_num1[i] == ((long)p_ptr->magic_num2[i] << 16)) continue;
		new_mana = ((long)p_ptr->magic_num2[i]+adj_mag_mana[A_INT]+13) * percent / 8;
		p_ptr->magic_num1[i] += new_mana;

		/* Must set frac to zero even if equal */
		if (p_ptr->magic_num1[i] > (p_ptr->magic_num2[i] << 16))
		{
			p_ptr->magic_num1[i] = ((long)p_ptr->magic_num2[i] << 16);
		}
		wild_regen = 20;
	}
	for (i = EATER_EXT*2; i < EATER_EXT*3; i++)
	{
		if (!p_ptr->magic_num1[i]) continue;
		if (!p_ptr->magic_num2[i]) continue;
		p_ptr->magic_num1[i] -= (long)(p_ptr->magic_num2[i] * (adj_mag_mana[A_INT] + 10)) * EATER_ROD_CHARGE/16;
		if (p_ptr->magic_num1[i] < 0) p_ptr->magic_num1[i] = 0;
		wild_regen = 20;
	}
}






/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_monsters(void)
{
	int i, frac;


	/* Regenerate everyone */
	for (i = 1; i < m_max; i++)
	{
		/* Check the i'th monster */
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];


		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Allow regeneration (if needed) */
		if (m_ptr->hp < m_ptr->maxhp)
		{
			/* Hack -- Base regeneration */
			frac = m_ptr->maxhp / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			m_ptr->hp += frac;

			/* Do not over-regenerate */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);
		}
	}
}


/*
 * Regenerate the monsters (once per 100 game turns)
 *
 * XXX XXX XXX Should probably be done during monster turns.
 */
static void regen_captured_monsters(void)
{
	int i, frac;
	bool heal = FALSE;

	/* Regenerate everyone */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		monster_race *r_ptr;
		object_type *o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;
		if (o_ptr->tval != TV_CAPTURE) continue;
		if (!o_ptr->pval) continue;

		heal = TRUE;

		r_ptr = &r_info[o_ptr->pval];

		/* Allow regeneration (if needed) */
		if (o_ptr->xtra4 < o_ptr->xtra5)
		{
			/* Hack -- Base regeneration */
			frac = o_ptr->xtra5 / 100;

			/* Hack -- Minimal regeneration rate */
			if (!frac) if (one_in_(2)) frac = 1;

			/* Hack -- Some monsters regenerate quickly */
			if (r_ptr->flags2 & RF2_REGENERATE) frac *= 2;

			/* Hack -- Regenerate */
			o_ptr->xtra4 += frac;

			/* Do not over-regenerate */
			if (o_ptr->xtra4 > o_ptr->xtra5) o_ptr->xtra4 = o_ptr->xtra5;
		}
	}

	if (heal)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}
}


static void notice_lite_change(object_type *o_ptr)
{
	/* Hack -- notice interesting fuel steps */
	if ((o_ptr->xtra4 < 100) || (!(o_ptr->xtra4 % 100)))
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Hack -- Special treatment when blind */
	if (p_ptr->blind)
	{
		/* Hack -- save some light for later */
		if (o_ptr->xtra4 == 0) o_ptr->xtra4++;
	}

	/* The light is now out */
	else if (o_ptr->xtra4 == 0)
	{
		disturb(0, 0);
#ifdef JP
msg_print("�����꤬�ä��Ƥ��ޤä���");
#else
		msg_print("Your light has gone out!");
#endif
		p_ptr->update |= (PU_BONUS);
	}

	/* The light is getting dim */
	else if (o_ptr->name2 == EGO_LITE_LONG)
	{
		if ((o_ptr->xtra4 < 50) && (!(o_ptr->xtra4 % 5))
		    && (turn % (TURNS_PER_TICK*2)))
		{
			if (disturb_minor) disturb(0, 0);
#ifdef JP
msg_print("�����꤬�����ˤʤäƤ��Ƥ��롣");
#else
			msg_print("Your light is growing faint.");
#endif

		}
	}

	/* The light is getting dim */
	else if ((o_ptr->xtra4 < 100) && (!(o_ptr->xtra4 % 10)))
	{
		if (disturb_minor) disturb(0, 0);
#ifdef JP
msg_print("�����꤬�����ˤʤäƤ��Ƥ��롣");
#else
		msg_print("Your light is growing faint.");
#endif

	}
}


void leave_quest_check(void)
{
	/* Save quset number for dungeon pref file ($LEAVING_QUEST) */
	leaving_quest = p_ptr->inside_quest;

	/* Leaving an 'only once' quest marks it as failed */
	if (leaving_quest &&
	    ((quest[leaving_quest].flags & QUEST_FLAG_ONCE)  || (quest[leaving_quest].type == QUEST_TYPE_RANDOM)) &&
	    (quest[leaving_quest].status == QUEST_STATUS_TAKEN))
	{
		quest[leaving_quest].status = QUEST_STATUS_FAILED;
		quest[leaving_quest].complev = (byte)p_ptr->lev;
		if (quest[leaving_quest].type == QUEST_TYPE_RANDOM)
		{
			r_info[quest[leaving_quest].r_idx].flags1 &= ~(RF1_QUESTOR);
			if (record_rand_quest)
				do_cmd_write_nikki(NIKKI_RAND_QUEST_F, leaving_quest, NULL);
		}
		else if (record_fix_quest)
			do_cmd_write_nikki(NIKKI_FIX_QUEST_F, leaving_quest, NULL);
	}
}


/*
 * Forcibly pseudo-identify an object in the inventory
 * (or on the floor)
 *
 * note: currently this function allows pseudo-id of any object,
 * including silly ones like potions & scrolls, which always
 * get '{average}'. This should be changed, either to stop such
 * items from being pseudo-id'd, or to allow psychometry to
 * detect whether the unidentified potion/scroll/etc is
 * good (Cure Light Wounds, Restore Strength, etc) or
 * bad (Poison, Weakness etc) or 'useless' (Slime Mold Juice, etc).
 */
bool psychometry(void)
{
	int             item;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];
	byte            feel;
	cptr            q, s;
	bool okay = FALSE;
	int idx;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
q = "�ɤΥ����ƥ��Ĵ�٤ޤ�����";
s = "Ĵ�٤륢���ƥब����ޤ���";
#else
	q = "Meditate on which item? ";
	s = "You have nothing appropriate.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* It is fully known, no information needed */
	if (object_known_p(o_ptr))
	{
#ifdef JP
msg_print("���⿷�������Ȥ�Ƚ��ʤ��ä���");
#else
		msg_print("You cannot find out anything more about that.");
#endif

		return TRUE;
	}

	/* Check for a feeling */
	feel = value_check_aux1(o_ptr);

	/* Get an object description */
	object_desc(o_name, o_ptr, FALSE, 0);

	/* Skip non-feelings */
	if (!feel)
	{
#ifdef JP
msg_format("%s������ä��Ѥ�ä����ϴ����Ȥ�ʤ��ä���", o_name);
#else
		msg_format("You do not perceive anything unusual about the %s.", o_name);
#endif

		return TRUE;
	}

#ifdef JP
msg_format("%s��%s�Ȥ�������������...",
    o_name,  game_inscriptions[feel]);
#else
	msg_format("You feel that the %s %s %s...",
			   o_name, ((o_ptr->number == 1) ? "is" : "are"),
			   game_inscriptions[feel]);
#endif


	/* We have "felt" it */
	o_ptr->ident |= (IDENT_SENSE);

	/* "Inscribe" it */
	o_ptr->feeling = feel;

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	/* Valid "tval" codes */
	switch (o_ptr->tval)
	{
	case TV_SHOT:
	case TV_ARROW:
	case TV_BOLT:
	case TV_BOW:
	case TV_DIGGING:
	case TV_HAFTED:
	case TV_POLEARM:
	case TV_SWORD:
	case TV_BOOTS:
	case TV_GLOVES:
	case TV_HELM:
	case TV_CROWN:
	case TV_SHIELD:
	case TV_CLOAK:
	case TV_SOFT_ARMOR:
	case TV_HARD_ARMOR:
	case TV_DRAG_ARMOR:
	case TV_CARD:
	case TV_RING:
	case TV_AMULET:
	case TV_LITE:
	case TV_FIGURINE:
		okay = TRUE;
		break;
	}

	/* Auto-inscription/destroy */
	idx = is_autopick(o_ptr);
	auto_inscribe_item(item, idx);
	if (okay && destroy_feeling)
		auto_destroy_item(item, idx);

	/* Something happened */
	return (TRUE);
}


static void gere_music(s32b music)
{
	switch(music)
	{
		case MUSIC_SLOW:
			slow_monsters();
			break;
		case MUSIC_STUN:
			stun_monsters(damroll(p_ptr->lev/10,2));
			break;
		case MUSIC_L_LIFE:
			hp_player(damroll(2,6));
			break;
		case MUSIC_FEAR:
			project_hack(GF_TURN_ALL, p_ptr->lev);
			break;
		case MUSIC_PSI:
			project_hack(GF_PSI, randint1(p_ptr->lev * 3 / 2));
			break;
		case MUSIC_ID:
			project(0, 1, py, px, 0, GF_IDENTIFY, PROJECT_ITEM, -1);
			break;
		case MUSIC_CONF:
			confuse_monsters(p_ptr->lev * 2);
			break;
		case MUSIC_SOUND:
			project_hack(GF_SOUND, damroll(10 + p_ptr->lev/5,7));
			break;
		case MUSIC_CHARM:
			charm_monsters(damroll(10 + p_ptr->lev/15,6));
			break;
		case MUSIC_WALL:
			project(0, 0, py, px,
				0, GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM | PROJECT_HIDE, -1);
			break;
		case MUSIC_DISPEL:
			dispel_monsters(randint1(p_ptr->lev * 3));
			dispel_evil(randint1(p_ptr->lev * 3));
			break;
		case MUSIC_SARUMAN:
			slow_monsters();
			sleep_monsters();
			break;
		case MUSIC_QUAKE:
			earthquake(py, px, 10);
			break;
		case MUSIC_STASIS:
			stasis_monsters(p_ptr->lev * 4);
			break;
		case MUSIC_SHERO:
			dispel_monsters(randint1(p_ptr->lev * 3));
			break;
		case MUSIC_H_LIFE:
			hp_player(damroll(15,10));
			set_stun(0);
			set_cut(0);
			break;
		case MUSIC_DETECT+19:
			wiz_lite(FALSE, FALSE);
		case MUSIC_DETECT+11:
		case MUSIC_DETECT+12:
		case MUSIC_DETECT+13:
		case MUSIC_DETECT+14:
		case MUSIC_DETECT+15:
		case MUSIC_DETECT+16:
		case MUSIC_DETECT+17:
		case MUSIC_DETECT+18:
			map_area(DETECT_RAD_MAP);
			if ((p_ptr->lev > 39) && (music < MUSIC_DETECT+19)) p_ptr->magic_num1[0] = music+1;
		case MUSIC_DETECT+6:
		case MUSIC_DETECT+7:
		case MUSIC_DETECT+8:
		case MUSIC_DETECT+9:
		case MUSIC_DETECT+10:
			detect_treasure(DETECT_RAD_DEFAULT);
			detect_objects_gold(DETECT_RAD_DEFAULT);
			detect_objects_normal(DETECT_RAD_DEFAULT);
			if ((p_ptr->lev > 24) && (music < MUSIC_DETECT+11)) p_ptr->magic_num1[0] = music+1;
		case MUSIC_DETECT+3:
		case MUSIC_DETECT+4:
		case MUSIC_DETECT+5:
			detect_monsters_invis(DETECT_RAD_DEFAULT);
			detect_monsters_normal(DETECT_RAD_DEFAULT);
			if ((p_ptr->lev > 19) && (music < MUSIC_DETECT+6)) p_ptr->magic_num1[0] = music+1;
		case MUSIC_DETECT:
		case MUSIC_DETECT+1:
		case MUSIC_DETECT+2:
			detect_traps(DETECT_RAD_DEFAULT, TRUE);
			detect_doors(DETECT_RAD_DEFAULT);
			detect_stairs(DETECT_RAD_DEFAULT);
			if ((p_ptr->lev > 14)  && (music  < MUSIC_DETECT+3)) p_ptr->magic_num1[0] = music+1;
			break;
	}
}

/*
 * If player has inscribed the object with "!!", let him know when it's
 * recharged. -LM-
 */
static void recharged_notice(object_type *o_ptr)
{
	char o_name[MAX_NLEN];

	cptr s;

	/* No inscription */
	if (!o_ptr->inscription) return;

	/* Find a '!' */
	s = strchr(quark_str(o_ptr->inscription), '!');

	/* Process notification request. */
	while (s)
	{
		/* Find another '!' */
		if (s[1] == '!')
		{
			/* Describe (briefly) */
			object_desc(o_name, o_ptr, FALSE, 0);

			/* Notify the player */
#ifdef JP
			msg_format("%s�Ϻƽ�Ŷ���줿��", o_name);
#else
			if (o_ptr->number > 1)
				msg_format("Your %s are recharged.", o_name);
			else
				msg_format("Your %s is recharged.", o_name);
#endif

			disturb(1, 0);

			/* Done. */
			return;
		}

		/* Keep looking for '!'s */
		s = strchr(s + 1, '!');
	}
}


static void check_music(void)
{
	magic_type *s_ptr;
	u32b shouhimana;

	/* Music singed by player */
	if(p_ptr->pclass != CLASS_BARD) return;
	if(!p_ptr->magic_num1[0] && !p_ptr->magic_num1[1]) return;

	s_ptr = &technic_info[REALM_MUSIC - MIN_TECHNIC][p_ptr->magic_num2[0]];

	shouhimana = (s_ptr->smana*(3800-p_ptr->spell_exp[p_ptr->magic_num2[0]])+2399);
	if(p_ptr->dec_mana)
		shouhimana *= 3;
	else shouhimana *= 4;
	shouhimana /= 9600;
	if(shouhimana < 1) shouhimana = 1;
	shouhimana *= 0x8000;
	if (((u16b)(p_ptr->csp) < (shouhimana / 0x10000)) || (p_ptr->anti_magic))
	{
		stop_singing();
		return;
	}
	else
	{
			p_ptr->csp -= (u16b) (shouhimana / 0x10000);
			shouhimana = (shouhimana & 0xffff);
			if (p_ptr->csp_frac < shouhimana)
		{
			p_ptr->csp--;
			p_ptr->csp_frac += (u16b)(0x10000L - shouhimana);
		}
		else
		{
			p_ptr->csp_frac -= (u16b)shouhimana;
		}

		p_ptr->redraw |= PR_MANA;
		if (p_ptr->magic_num1[1])
		{
			p_ptr->magic_num1[0] = p_ptr->magic_num1[1];
			p_ptr->magic_num1[1] = 0;
#ifdef JP
			msg_print("�Τ�Ƴ�������");
#else
			msg_print("You restart singing.");
#endif
			p_ptr->action = ACTION_SING;

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS | PU_HP);

			/* Redraw status bar */
			p_ptr->redraw |= (PR_STATUS);
		}
	}
	if (p_ptr->spell_exp[p_ptr->magic_num2[0]] < 900)
		p_ptr->spell_exp[p_ptr->magic_num2[0]]+=5;
	else if(p_ptr->spell_exp[p_ptr->magic_num2[0]] < 1200)
		{if (one_in_(2) && (dun_level > 4) && ((dun_level + 10) > p_ptr->lev)) p_ptr->spell_exp[p_ptr->magic_num2[0]]+=1;}
	else if(p_ptr->spell_exp[p_ptr->magic_num2[0]] < 1400)
		{if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && ((dun_level + 5) > s_ptr->slevel)) p_ptr->spell_exp[p_ptr->magic_num2[0]]+=1;}
	else if(p_ptr->spell_exp[p_ptr->magic_num2[0]] < 1600)
		{if (one_in_(5) && ((dun_level + 5) > p_ptr->lev) && (dun_level > s_ptr->slevel)) p_ptr->spell_exp[p_ptr->magic_num2[0]]+=1;}

	gere_music(p_ptr->magic_num1[0]);
}

/* Choose one of items that have cursed flag */
static object_type *choose_cursed_obj_name(u32b flag)
{
	int i;
	int choices[INVEN_TOTAL-INVEN_RARM];
	int number = 0;

	/* Paranoia -- Player has no warning-item */
	if (!(p_ptr->cursed & flag)) return NULL;

	/* Search Inventry */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		if (o_ptr->curse_flags & flag)
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return (&inventory[choices[randint0(number)]]);
}


/*
 * Handle certain things once every 10 game turns
 */
static void process_world(void)
{
	int x, y, i, j;
	int regen_amount;
	bool cave_no_regen = FALSE;
	int upkeep_factor = 0;
	cave_type *c_ptr;
	object_type *o_ptr;
	int temp;
	object_kind *k_ptr;
	const int dec_count = (easy_band ? 2 : 1);

	int day, hour, min, prev_min;

	s32b len = TURNS_PER_TICK * TOWN_DAWN;
	s32b tick = turn % len + len / 4;

	extract_day_hour_min(&day, &hour, &min);
	prev_min = (1440 * (tick - TURNS_PER_TICK) / len) % 60;

	if ((turn - old_turn == (150 - dun_level) * TURNS_PER_TICK)
	    && (dun_level) &&
	    !(quest_number(dun_level) && ((quest_number(dun_level) < MIN_RANDOM_QUEST) && !(quest_number(dun_level) == QUEST_OBERON || quest_number(dun_level) == QUEST_SERPENT || !(quest[quest_number(dun_level)].flags & QUEST_FLAG_PRESET)))) &&
	    !(p_ptr->inside_battle))
		do_cmd_feeling();

	if (p_ptr->inside_battle && !p_ptr->leaving)
	{

		int i2, j2;
		int win_m_idx = 0;
		int number_mon = 0;

		/* Count all hostile monsters */
		for (i2 = 0; i2 < cur_wid; ++i2)
			for (j2 = 0; j2 < cur_hgt; j2++)
				if ((cave[j2][i2].m_idx > 0) && (cave[j2][i2].m_idx != p_ptr->riding))
				{
					number_mon++;
					win_m_idx = cave[j2][i2].m_idx;
				}

		if (number_mon == 0)
		{
#ifdef JP
msg_print("���Ǥ��˽����ޤ�����");
#else
			msg_print("They have kill each other at the same time.");
#endif
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else if ((number_mon-1) == 0)
		{
			char m_name[80];
			monster_type *wm_ptr;

			wm_ptr = &m_list[win_m_idx];

			monster_desc(m_name, wm_ptr, 0);
#ifdef JP
msg_format("%s������������", m_name);
#else
			msg_format("%s is winner!", m_name);
#endif
			msg_print(NULL);

			if (win_m_idx == (sel_monster+1))
			{
#ifdef JP
msg_print("����ǤȤ��������ޤ���");
#else
				msg_print("Congratulations.");
#endif
#ifdef JP
msg_format("%d��������ä���", battle_odds);
#else
				msg_format("You received %d gold.", battle_odds);
#endif
			p_ptr->au += battle_odds;
			}
			else
			{
#ifdef JP
msg_print("��ǰ�Ǥ�����");
#else
				msg_print("You lost gold.");
#endif
			}
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else if(turn - old_turn == 150*TURNS_PER_TICK)
		{
#ifdef JP
msg_print("����ʬ������ޤ��󤬡����ξ���ϰ���ʬ���Ȥ����Ƥ��������ޤ���");
#else
			msg_format("This battle have ended in a draw.");
#endif
			p_ptr->au += kakekin;
			msg_print(NULL);
			p_ptr->energy_need = 0;
			battle_monsters();
		}
	}

	/* Every 20 game turns */
	if (turn % TURNS_PER_TICK) return;

	/*** Check the Time and Load ***/

	if (!(turn % (50*TURNS_PER_TICK)))
	{
		/* Check time and load */
		if ((0 != check_time()) || (0 != check_load()))
		{
			/* Warning */
			if (closing_flag <= 2)
			{
				/* Disturb */
				disturb(0, 0);

				/* Count warnings */
				closing_flag++;

				/* Message */
#ifdef JP
msg_print("���󥰥Х�ɤؤ��礬�Ĥ������äƤ��ޤ�...");
msg_print("�������λ���뤫�����֤��뤫���Ʋ�������");
#else
				msg_print("The gates to ANGBAND are closing...");
				msg_print("Please finish up and/or save your game.");
#endif

			}

			/* Slam the gate */
			else
			{
				/* Message */
#ifdef JP
msg_print("�������󥰥Х�ɤؤ��礬�Ĥ�����ޤ�����");
#else
				msg_print("The gates to ANGBAND are now closed.");
#endif


				/* Stop playing */
				p_ptr->playing = FALSE;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
		}
	}

	/*** Attempt timed autosave ***/
	if (autosave_t && autosave_freq && !p_ptr->inside_battle)
	{
		if (!(turn % ((s32b)autosave_freq * TURNS_PER_TICK)))
			do_cmd_save_game(TRUE);
	}

	if (mon_fight)
	{
#ifdef JP
		msg_print("������ʹ��������");
#else
		msg_print("You hear noise.");
#endif
	}

	/*** Handle the wilderness/town (sunshine) ***/

	/* While in town/wilderness */
	if (!dun_level && !p_ptr->inside_quest && !p_ptr->inside_battle && !p_ptr->inside_arena && !p_ptr->wild_mode)
	{
		/* Hack -- Daybreak/Nighfall in town */
		if (!(turn % ((TURNS_PER_TICK * TOWN_DAWN) / 2)))
		{
			bool dawn;

			/* Check for dawn */
			dawn = (!(turn % (TURNS_PER_TICK * TOWN_DAWN)));

			/* Day breaks */
			if (dawn)
			{
				/* Message */
#ifdef JP
msg_print("�뤬��������");
#else
				msg_print("The sun has risen.");
#endif


				/* Hack -- Scan the town */
				for (y = 0; y < cur_hgt; y++)
				{
					for (x = 0; x < cur_wid; x++)
					{
						/* Get the cave grid */
						c_ptr = &cave[y][x];

						/* Assume lit */
						c_ptr->info |= (CAVE_GLOW);

						/* Hack -- Memorize lit grids if allowed */
						if (view_perma_grids) c_ptr->info |= (CAVE_MARK);

						/* Hack -- Notice spot */
						note_spot(y, x);
					}
				}
			}

			/* Night falls */
			else
			{
				/* Message */
#ifdef JP
msg_print("�����������");
#else
				msg_print("The sun has fallen.");
#endif


				/* Hack -- Scan the town */
				for (y = 0; y < cur_hgt; y++)
				{
					for (x = 0; x < cur_wid; x++)
					{
						/* Get the cave grid */
						c_ptr = &cave[y][x];

						/* Darken "boring" features */
						if ((c_ptr->feat <= FEAT_INVIS) ||
						    ((c_ptr->feat >= FEAT_DEEP_WATER) &&
							(c_ptr->feat <= FEAT_MOUNTAIN) &&
						     (c_ptr->feat != FEAT_MUSEUM)) ||
						    (x == 0) || (x == cur_wid-1) ||
						    (y == 0) || (y == cur_hgt-1))
						{
							/* Forget the grid */
							if (!is_mirror_grid(c_ptr)) c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

							/* Hack -- Notice spot */
							note_spot(y, x);
						}
					}
				}
			}

			/* Update the monsters */
			p_ptr->update |= (PU_MONSTERS | PU_MON_LITE);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);

			/* Window stuff */
			p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
		}
	}

	/* Set back the rewards once a day */
	if (!(turn % (TURNS_PER_TICK*10 * STORE_TURNS)))
	{
		int n;

		/* Reset the rewards */
		for (n = 0; n < MAX_BACT; n++)
		{
			p_ptr->rewards[n] = FALSE;
		}

		/* Message */
#ifdef JP
if (cheat_xtra) msg_print("�󽷤�ꥻ�å�");
#else
		if (cheat_xtra) msg_print("Rewards reset.");
#endif

	}


	/*** Process the monsters ***/

	/* Check for creature generation. */
	if (one_in_(d_info[dungeon_type].max_m_alloc_chance) &&
	    !p_ptr->inside_arena && !p_ptr->inside_quest && !p_ptr->inside_battle)
	{
		/* Make a new monster */
		(void)alloc_monster(MAX_SIGHT + 5, 0);
	}

	/* Hack -- Check for creature regeneration */
	if (!(turn % (TURNS_PER_TICK*10)) && !p_ptr->inside_battle) regen_monsters();
	if (!(turn % (TURNS_PER_TICK*3))) regen_captured_monsters();


	/*** Damage over Time ***/

	/* Take damage from poison */
	if (p_ptr->poisoned && !p_ptr->invuln)
	{
		/* Take damage */
#ifdef JP
take_hit(DAMAGE_NOESCAPE, 1, "��", -1);
#else
		take_hit(DAMAGE_NOESCAPE, 1, "poison", -1);
#endif

	}


	/* (Vampires) Take damage from sunlight */
	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
		if (!dun_level && !p_ptr->resist_lite && !p_ptr->invuln && is_daytime())
		{
			if (cave[py][px].info & CAVE_GLOW)
			{
				/* Take damage */
#ifdef JP
msg_print("���������ʤ��Υ���ǥåɤ����Τ�Ƥ��Ǥ�������");
take_hit(DAMAGE_NOESCAPE, 1, "����", -1);
#else
				msg_print("The sun's rays scorch your undead flesh!");
				take_hit(DAMAGE_NOESCAPE, 1, "sunlight", -1);
#endif

				cave_no_regen = TRUE;
			}
		}

		if (inventory[INVEN_LITE].tval && (inventory[INVEN_LITE].name2 != EGO_LITE_DARKNESS) &&
		    !p_ptr->resist_lite)
		{
			object_type * o_ptr = &inventory[INVEN_LITE];
			char o_name [MAX_NLEN];
			char ouch [MAX_NLEN+40];

			/* Get an object description */
			object_desc(o_name, o_ptr, FALSE, 0);

#ifdef JP
msg_format("%s�����ʤ��Υ���ǥåɤ����Τ�Ƥ��Ǥ�������", o_name);
#else
			msg_format("The %s scorches your undead flesh!", o_name);
#endif


			cave_no_regen = TRUE;

			/* Get an object description */
			object_desc(o_name, o_ptr, TRUE, 0);

#ifdef JP
sprintf(ouch, "%s�������������᡼��", o_name);
#else
			sprintf(ouch, "wielding %s", o_name);
#endif

			if (!p_ptr->invuln) take_hit(DAMAGE_NOESCAPE, 1, ouch, -1);
		}
	}

	if ((cave[py][px].feat == FEAT_SHAL_LAVA) &&
		!p_ptr->invuln && !p_ptr->immune_fire && !p_ptr->ffall)
	{
		int damage = 3000 + randint0(2000);

		if (prace_is_(RACE_ENT)) damage += damage/3;

		if (p_ptr->resist_fire) damage = damage / 3;
		if (p_ptr->oppose_fire) damage = damage / 3;
		damage = damage / 100 + (randint0(100) < (damage % 100));

		if (damage)
		{
			/* Take damage */
#ifdef JP
msg_print("�ϴ�ǲн�������");
take_hit(DAMAGE_NOESCAPE, damage, "�����ϴ�ή", -1);
#else
			msg_print("The lava burns you!");
			take_hit(DAMAGE_NOESCAPE, damage, "shallow lava", -1);
#endif

			cave_no_regen = TRUE;
		}
	}

	else if ((cave[py][px].feat == FEAT_DEEP_LAVA) &&
		!p_ptr->invuln && !p_ptr->immune_fire)
	{
		int damage = 6000 + randint0(4000);

		cptr message;
		cptr hit_from;

		if (p_ptr->resist_fire) damage = damage / 3;
		if (p_ptr->oppose_fire) damage = damage / 3;

		if (p_ptr->ffall)
		{
			damage = damage / 5;

#ifdef JP
message = "Ǯ�ǲн�������";
hit_from = "�����ϴ�ή�ξ����ͷ�������᡼��";
#else
			message = "The heat burns you!";
			hit_from = "flying over deep lava";
#endif

		}
		else
		{
#ifdef JP
message = "�ϴ�ǲн�������";
hit_from = "�����ϴ�ή";
#else
			message = "The lava burns you!";
			hit_from = "deep lava";
#endif

		}

		damage = damage / 100 + (randint0(100) < (damage % 100));
		if (damage)
		{
			/* Take damage */
			msg_print(message);
			take_hit(DAMAGE_NOESCAPE, damage, hit_from, -1);

			cave_no_regen = TRUE;
		}
	}

	else if ((cave[py][px].feat == FEAT_DEEP_WATER) && !p_ptr->ffall && !p_ptr->can_swim)
	{
		if (p_ptr->total_weight > (((u32b)adj_str_wgt[p_ptr->stat_ind[A_STR]] * (p_ptr->pclass == CLASS_BERSERKER ? 150 : 100)) / 2))
		{
			/* Take damage */
#ifdef JP
msg_print("Ů��Ƥ��롪");
take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->lev), "Ů��", -1);
#else
			msg_print("You are drowning!");
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->lev), "drowning", -1);
#endif

			cave_no_regen = TRUE;
		}
	}

	if (p_ptr->riding)
	{
		int damage;
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_FIRE) && !p_ptr->immune_fire)
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			if (prace_is_(RACE_ENT)) damage += damage/3;
			if (p_ptr->resist_fire) damage = damage / 3;
			if (p_ptr->oppose_fire) damage = damage / 3;
#ifdef JP
msg_print("Ǯ����");
take_hit(DAMAGE_NOESCAPE, damage, "��Υ�����", -1);
#else
			msg_print("It's hot!");
			take_hit(DAMAGE_NOESCAPE, damage, "Fire aura", -1);
#endif
		}
		if ((r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_AURA_ELEC) && !p_ptr->immune_elec)
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			if (p_ptr->resist_elec) damage = damage / 3;
			if (p_ptr->oppose_elec) damage = damage / 3;
#ifdef JP
msg_print("�ˤ���");
take_hit(DAMAGE_NOESCAPE, damage, "�ŵ��Υ�����", -1);
#else
			msg_print("It hurts!");
			take_hit(DAMAGE_NOESCAPE, damage, "Elec aura", -1);
#endif
		}
		if ((r_info[m_list[p_ptr->riding].r_idx].flags3 & RF3_AURA_COLD) && !p_ptr->immune_cold)
		{
			damage = r_info[m_list[p_ptr->riding].r_idx].level / 2;
			if (p_ptr->resist_cold) damage = damage / 3;
			if (p_ptr->oppose_cold) damage = damage / 3;
#ifdef JP
msg_print("�䤿����");
take_hit(DAMAGE_NOESCAPE, damage, "�䵤�Υ�����", -1);
#else
			msg_print("It's cold!");
			take_hit(DAMAGE_NOESCAPE, damage, "Cold aura", -1);
#endif
		}
	}

	/* Spectres -- take damage when moving through walls */
	/*
	 * Added: ANYBODY takes damage if inside through walls
	 * without wraith form -- NOTE: Spectres will never be
	 * reduced below 0 hp by being inside a stone wall; others
	 * WILL BE!
	 */
	if (!cave_floor_bold(py, px))
	{
		/* Player can walk through trees */
		if ((cave[py][px].feat == FEAT_TREES) || ((cave[py][px].feat == FEAT_MOUNTAIN) && !dun_level && p_ptr->ffall))
		{
			/* Do nothing */
		}
		else if (!p_ptr->invuln && !p_ptr->wraith_form && !p_ptr->kabenuke &&
		    ((p_ptr->chp > (p_ptr->lev / 5)) || !p_ptr->pass_wall))
		{
			cptr dam_desc;

			cave_no_regen = TRUE;

			if (p_ptr->pass_wall)
			{
#ifdef JP
msg_print("�Τ�ʬ�Ҥ�ʬ�򤷤��������롪");
dam_desc = "̩��";
#else
				msg_print("Your molecules feel disrupted!");
				dam_desc = "density";
#endif

			}
			else
			{
#ifdef JP
msg_print("���줿��˲����٤��줿��");
dam_desc = "�Ť���";
#else
				msg_print("You are being crushed!");
				dam_desc = "solid rock";
#endif

			}

			take_hit(DAMAGE_NOESCAPE, 1 + (p_ptr->lev / 5), dam_desc, -1);
		}
	}

	if (!hour && !min)
	{
		monster_race *r_ptr;

		if (min != prev_min)
		{
			int max_dl = 3;
			bool old_inside_battle = p_ptr->inside_battle;

			do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);

			p_ptr->inside_battle = TRUE;
			get_mon_num_prep(NULL,NULL);

			for (i = 0; i < max_d_idx; i++)
			{
				if (max_dlv[i] < d_info[i].mindepth) continue;
				if (max_dl < max_dlv[i]) max_dl = max_dlv[i];
			}
			while (1)
			{
				today_mon = get_mon_num(max_dl);
				r_ptr = &r_info[today_mon];

				if (r_ptr->flags1 & RF1_UNIQUE) continue;
				if (r_ptr->flags7 & (RF7_UNIQUE_7 | RF7_UNIQUE2)) continue;
				if (r_ptr->flags2 & (RF2_MULTIPLY)) continue;
				if (!(r_ptr->flags9 & RF9_DROP_CORPSE) || !(r_ptr->flags9 & RF9_DROP_SKELETON)) continue;
				if (r_ptr->level < MIN(max_dl/2, 40)) continue;
				if (r_ptr->rarity > 10) continue;
				if (r_ptr->level == 0) continue;
				break;
			}
			p_ptr->today_mon = 0;
			p_ptr->inside_battle = old_inside_battle;
		}
	}

	/* Nightmare mode activates the TY_CURSE at midnight */
	if (ironman_nightmare)
	{
		/* Require exact minute */
		if (min != prev_min)
		{
			/* Every 15 minutes after 11:00 pm */
			if ((hour == 23) && !(min % 15))
			{
				/* Disturbing */
				disturb(0, 0);

				switch (min / 15)
				{
					case 0:
					{
#ifdef JP
msg_print("�󤯤��Ե�̣�ʾ�β����Ĥä���");
#else
						msg_print("You hear a distant bell toll ominously.");
#endif

						break;
					}
					case 1:
					{
#ifdef JP
msg_print("�󤯤Ǿ⤬����Ĥä���");
#else
						msg_print("A distant bell sounds twice.");
#endif

						break;
					}
					case 2:
	{
#ifdef JP
msg_print("�󤯤Ǿ⤬�����Ĥä���");
#else
						msg_print("A distant bell sounds three times.");
#endif

						break;
					}
					case 3:
					{
#ifdef JP
msg_print("�󤯤Ǿ⤬�Ͳ��Ĥä���");
#else
						msg_print("A distant bell tolls four times.");
#endif

						break;
					}
				}
			}

			/* TY_CURSE activates at mignight! */
			if (!hour && !min)
			{
				int count = 0;

				disturb(1, 0);
#ifdef JP
msg_print("�󤯤Ǿ⤬������Ĥꡢ�����褦���Ť�������ؾä��Ƥ��ä���");
#else
				msg_print("A distant bell tolls many times, fading into an deathly silence.");
#endif

		activate_ty_curse(FALSE, &count);
	}
		}
	}

	/* Take damage from cuts */
	if (p_ptr->cut && !p_ptr->invuln)
	{
		/* Mortal wound or Deep Gash */
		if (p_ptr->cut > 1000)
		{
			i = 200;
		}

		else if (p_ptr->cut > 200)
		{
			i = 80;
		}

		/* Severe cut */
		else if (p_ptr->cut > 100)
		{
			i = 32;
		}

		else if (p_ptr->cut > 50)
		{
			i = 16;
		}

		else if (p_ptr->cut > 25)
		{
			i = 7;
		}

		else if (p_ptr->cut > 10)
		{
			i = 3;
		}

		/* Other cuts */
		else
		{
			i = 1;
		}

		/* Take damage */
#ifdef JP
take_hit(DAMAGE_NOESCAPE, i, "��̿��", -1);
#else
		take_hit(DAMAGE_NOESCAPE, i, "a fatal wound", -1);
#endif

	}


	/*** Check the Food, and Regenerate ***/

	if (!p_ptr->inside_battle)
	{
		/* Digest normally */
		if (p_ptr->food < PY_FOOD_MAX)
		{
			/* Every 100 game turns */
			if (!(turn % (TURNS_PER_TICK*5)))
			{
				/* Basic digestion rate based on speed */
				i = /* extract_energy[p_ptr->pspeed] * 2;*/
				((p_ptr->pspeed > 199) ? 49 : ((p_ptr->pspeed < 0) ?
				1 : extract_energy[p_ptr->pspeed]));

				/* Regeneration takes more food */
				if (p_ptr->regenerate) i += 20;
				if (p_ptr->special_defense & (KAMAE_MASK | KATA_MASK)) i+= 20;
				if (p_ptr->cursed & TRC_FAST_DIGEST) i += 30;

				/* Slow digestion takes less food */
				if (p_ptr->slow_digest) i -= 5;

				/* Minimal digestion */
				if (i < 1) i = 1;
				/* Maximal digestion */
				if (i > 100) i = 100;

				/* Digest some food */
				(void)set_food(p_ptr->food - i);
			}
		}

		/* Digest quickly when gorged */
		else
		{
			/* Digest a lot of food */
			(void)set_food(p_ptr->food - 100);
		}
	}

	/* Starve to death (slowly) */
	if (p_ptr->food < PY_FOOD_STARVE)
	{
		/* Calculate damage */
		i = (PY_FOOD_STARVE - p_ptr->food) / 10;

		/* Take damage */
#ifdef JP
if (!p_ptr->invuln) take_hit(DAMAGE_LOSELIFE, i, "��ʢ", -1);
#else
		if (!p_ptr->invuln) take_hit(DAMAGE_LOSELIFE, i, "starvation", -1);
#endif

	}

	/* Default regeneration */
	regen_amount = PY_REGEN_NORMAL;

	/* Getting Weak */
	if (p_ptr->food < PY_FOOD_WEAK)
	{
		/* Lower regeneration */
		if (p_ptr->food < PY_FOOD_STARVE)
		{
			regen_amount = 0;
		}
		else if (p_ptr->food < PY_FOOD_FAINT)
		{
			regen_amount = PY_REGEN_FAINT;
		}
		else
		{
			regen_amount = PY_REGEN_WEAK;
		}

		/* Getting Faint */
		if ((p_ptr->food < PY_FOOD_FAINT) && !p_ptr->inside_battle)
		{
			/* Faint occasionally */
			if (!p_ptr->paralyzed && (randint0(100) < 10))
			{
				/* Message */
#ifdef JP
msg_print("���ޤ�ˤ��ʢ�ǵ��䤷�Ƥ��ޤä���");
#else
				msg_print("You faint from the lack of food.");
#endif

				disturb(1, 0);

				/* Hack -- faint (bypass free action) */
				(void)set_paralyzed(p_ptr->paralyzed + 1 + randint0(5));
			}
		}
	}


	/* Are we walking the pattern? */
	if (pattern_effect())
	{
		cave_no_regen = TRUE;
	}
	else
	{
		/* Regeneration ability */
		if (p_ptr->regenerate)
		{
			regen_amount = regen_amount * 2;
		}
		if (p_ptr->special_defense & (KAMAE_MASK | KATA_MASK))
		{
			regen_amount /= 2;
		}
		if (p_ptr->cursed & TRC_SLOW_REGEN)
		{
			regen_amount /= 5;
		}
	}


	/* Searching or Resting */
	if ((p_ptr->action == ACTION_SEARCH) || (p_ptr->action == ACTION_REST))
	{
		regen_amount = regen_amount * 2;
	}

	upkeep_factor = calculate_upkeep();

	/* Regenerate the mana */
/*	if (p_ptr->csp < p_ptr->msp) */
	{
		if (upkeep_factor)
		{
			s32b upkeep_regen = ((100 - upkeep_factor) * regen_amount);
			if ((p_ptr->action == ACTION_LEARN) || (p_ptr->action == ACTION_HAYAGAKE)) upkeep_regen -= regen_amount;
			regenmana(upkeep_regen/100);

#ifdef TRACK_FRIENDS
			if (p_ptr->wizard)
			{
#ifdef JP
msg_format("�ͣв���: %d/%d", upkeep_regen, regen_amount);
#else
				msg_format("Regen: %d/%d", upkeep_regen, regen_amount);
#endif

			}
#endif /* TRACK_FRIENDS */

		}
		else if (p_ptr->action != ACTION_LEARN)
		{
			regenmana(regen_amount);
		}
	}
	if (p_ptr->pclass == CLASS_MAGIC_EATER)
	{
		regenmagic(regen_amount);
	}

	if ((p_ptr->csp == 0) && (p_ptr->csp_frac == 0))
	{
		while (upkeep_factor > 100)
		{
#ifdef JP
msg_print("����ʤ�¿���ΥڥåȤ�����Ǥ��ʤ���");
#else
			msg_print("Such much pets cannot be controled at once!");
#endif
			msg_print(NULL);
			do_cmd_pet_dismiss();

			upkeep_factor = calculate_upkeep();

#ifdef JP
			msg_format("�ݻ��ͣФ� %d%%", upkeep_factor);
#else
			msg_format("Upkeep: %d%% mana.", upkeep_factor);
#endif
			msg_print(NULL);
		}
	}

	/* Poisoned or cut yields no healing */
	if (p_ptr->poisoned) regen_amount = 0;
	if (p_ptr->cut) regen_amount = 0;

	/* Special floor -- Pattern, in a wall -- yields no healing */
	if (cave_no_regen) regen_amount = 0;

	regen_amount = (regen_amount * mutant_regenerate_mod) / 100;

	/* Regenerate Hit Points if needed */
	if ((p_ptr->chp < p_ptr->mhp) && !cave_no_regen)
	{
		if ((cave[py][px].feat < FEAT_PATTERN_END) &&
		    (cave[py][px].feat >= FEAT_PATTERN_START))
		{
			regenhp(regen_amount / 5); /* Hmmm. this should never happen? */
		}
		else
		{
			regenhp(regen_amount);
		}
	}


	/*** Timeout Various Things ***/

	/* Mimic */
	if (p_ptr->tim_mimic)
	{
		(void)set_mimic(p_ptr->tim_mimic - 1, p_ptr->mimic_form, TRUE);
	}

	/* Hack -- Hallucinating */
	if (p_ptr->image)
	{
		(void)set_image(p_ptr->image - dec_count);
	}

	/* Blindness */
	if (p_ptr->blind)
	{
		(void)set_blind(p_ptr->blind - dec_count);
	}

	/* Times see-invisible */
	if (p_ptr->tim_invis)
	{
		(void)set_tim_invis(p_ptr->tim_invis - 1, TRUE);
	}

	if (multi_rew)
	{
		multi_rew = FALSE;
	}

	/* Timed esp */
	if (p_ptr->tim_esp)
	{
		(void)set_tim_esp(p_ptr->tim_esp - 1, TRUE);
	}

	/* Timed temporary elemental brands. -LM- */
	if (p_ptr->ele_attack)
	{
		p_ptr->ele_attack--;

		/* Clear all temporary elemental brands. */
		if (!p_ptr->ele_attack) set_ele_attack(0, 0);
	}

	/* Timed temporary elemental immune. -LM- */
	if (p_ptr->ele_immune)
	{
		p_ptr->ele_immune--;

		/* Clear all temporary elemental brands. */
		if (!p_ptr->ele_immune) set_ele_immune(0, 0);
	}

	/* Timed infra-vision */
	if (p_ptr->tim_infra)
	{
		(void)set_tim_infra(p_ptr->tim_infra - 1, TRUE);
	}

	/* Timed stealth */
	if (p_ptr->tim_stealth)
	{
		(void)set_tim_stealth(p_ptr->tim_stealth - 1, TRUE);
	}

	/* Timed levitation */
	if (p_ptr->tim_ffall)
	{
		(void)set_tim_ffall(p_ptr->tim_ffall - 1, TRUE);
	}

	/* Timed sh_touki */
	if (p_ptr->tim_sh_touki)
	{
		(void)set_tim_sh_touki(p_ptr->tim_sh_touki - 1, TRUE);
	}

	/* Timed sh_fire */
	if (p_ptr->tim_sh_fire)
	{
		(void)set_tim_sh_fire(p_ptr->tim_sh_fire - 1, TRUE);
	}

	/* Timed sh_holy */
	if (p_ptr->tim_sh_holy)
	{
		(void)set_tim_sh_holy(p_ptr->tim_sh_holy - 1, TRUE);
	}

	/* Timed eyeeye */
	if (p_ptr->tim_eyeeye)
	{
		(void)set_tim_eyeeye(p_ptr->tim_eyeeye - 1, TRUE);
	}

	/* Timed resist-magic */
	if (p_ptr->resist_magic)
	{
		(void)set_resist_magic(p_ptr->resist_magic - 1, TRUE);
	}

	/* Timed regeneration */
	if (p_ptr->tim_regen)
	{
		(void)set_tim_regen(p_ptr->tim_regen - 1, TRUE);
	}

	/* Timed resist nether */
	if (p_ptr->tim_res_nether)
	{
		(void)set_tim_res_nether(p_ptr->tim_res_nether - 1, TRUE);
	}

	/* Timed resist time */
	if (p_ptr->tim_res_time)
	{
		(void)set_tim_res_time(p_ptr->tim_res_time - 1, TRUE);
	}

	/* Timed reflect */
	if (p_ptr->tim_reflect)
	{
		(void)set_tim_reflect(p_ptr->tim_reflect - 1, TRUE);
	}

	/* Multi-shadow */
	if (p_ptr->multishadow)
	{
		(void)set_multishadow(p_ptr->multishadow - 1, TRUE);
	}

	/* Timed Robe of dust */
	if (p_ptr->dustrobe)
	{
		(void)set_dustrobe(p_ptr->dustrobe - 1, TRUE);
	}

	/* Timed infra-vision */
	if (p_ptr->kabenuke)
	{
		(void)set_kabenuke(p_ptr->kabenuke - 1, TRUE);
	}

	/* Paralysis */
	if (p_ptr->paralyzed)
	{
		(void)set_paralyzed(p_ptr->paralyzed - dec_count);
	}

	/* Confusion */
	if (p_ptr->confused)
	{
		(void)set_confused(p_ptr->confused - dec_count);
	}

	/* Afraid */
	if (p_ptr->afraid)
	{
		(void)set_afraid(p_ptr->afraid - dec_count);
	}

	/* Fast */
	if (p_ptr->fast)
	{
		(void)set_fast(p_ptr->fast - 1, TRUE);
	}

	/* Slow */
	if (p_ptr->slow)
	{
		(void)set_slow(p_ptr->slow - dec_count, TRUE);
	}

	/* Protection from evil */
	if (p_ptr->protevil)
	{
		(void)set_protevil(p_ptr->protevil - 1, TRUE);
	}

	/* Invulnerability */
	if (p_ptr->invuln)
	{
		(void)set_invuln(p_ptr->invuln - 1, TRUE);
	}

	/* Wraith form */
	if (p_ptr->wraith_form)
	{
		(void)set_wraith_form(p_ptr->wraith_form - 1, TRUE);
	}

	/* Heroism */
	if (p_ptr->hero)
	{
		(void)set_hero(p_ptr->hero - 1, TRUE);
	}

	/* Super Heroism */
	if (p_ptr->shero)
	{
		(void)set_shero(p_ptr->shero - 1, TRUE);
	}

	/* Blessed */
	if (p_ptr->blessed)
	{
		(void)set_blessed(p_ptr->blessed - 1, TRUE);
	}

	/* Shield */
	if (p_ptr->shield)
	{
		(void)set_shield(p_ptr->shield - 1, TRUE);
	}

	/* Tsubureru */
	if (p_ptr->tsubureru)
	{
		(void)set_tsubureru(p_ptr->tsubureru - 1, TRUE);
	}

	/* Magicdef */
	if (p_ptr->magicdef)
	{
		(void)set_magicdef(p_ptr->magicdef - 1, TRUE);
	}

	/* Tsuyoshi */
	if (p_ptr->tsuyoshi)
	{
		(void)set_tsuyoshi(p_ptr->tsuyoshi - 1, TRUE);
	}

	/* Oppose Acid */
	if (p_ptr->oppose_acid)
	{
		(void)set_oppose_acid(p_ptr->oppose_acid - 1, TRUE);
	}

	/* Oppose Lightning */
	if (p_ptr->oppose_elec)
	{
		(void)set_oppose_elec(p_ptr->oppose_elec - 1, TRUE);
	}

	/* Oppose Fire */
	if (p_ptr->oppose_fire)
	{
		(void)set_oppose_fire(p_ptr->oppose_fire - 1, TRUE);
	}

	/* Oppose Cold */
	if (p_ptr->oppose_cold)
	{
		(void)set_oppose_cold(p_ptr->oppose_cold - 1, TRUE);
	}

	/* Oppose Poison */
	if (p_ptr->oppose_pois)
	{
		(void)set_oppose_pois(p_ptr->oppose_pois - 1, TRUE);
	}

	if (p_ptr->ult_res)
	{
		(void)set_ultimate_res(p_ptr->ult_res - 1, TRUE);
	}

	/*** Poison and Stun and Cut ***/

	/* Poison */
	if (p_ptr->poisoned)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_poisoned(p_ptr->poisoned - adjust);
	}

	/* Stun */
	if (p_ptr->stun)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Apply some healing */
		(void)set_stun(p_ptr->stun - adjust);
	}

	/* Cut */
	if (p_ptr->cut)
	{
		int adjust = adj_con_fix[p_ptr->stat_ind[A_CON]] + 1;

		/* Hack -- Truly "mortal" wound */
		if (p_ptr->cut > 1000) adjust = 0;

		/* Apply some healing */
		(void)set_cut(p_ptr->cut - adjust);
	}



	/*** Process Light ***/

	/* Check for light being wielded */
	o_ptr = &inventory[INVEN_LITE];

	/* Burn some fuel in the current lite */
	if (o_ptr->tval == TV_LITE)
	{
		/* Hack -- Use some fuel (except on artifacts) */
		if (!(artifact_p(o_ptr) || o_ptr->sval == SV_LITE_FEANOR) && (o_ptr->xtra4 > 0))
		{
			/* Decrease life-span */
			if (o_ptr->name2 == EGO_LITE_LONG)
			{
				if (turn % (TURNS_PER_TICK*2)) o_ptr->xtra4--;
			}
			else o_ptr->xtra4--;

			/* Notice interesting fuel steps */
			notice_lite_change(o_ptr);
		}
	}

	/* Calculate torch radius */
	p_ptr->update |= (PU_TORCH);


	/*** Process mutation effects ***/
	if (p_ptr->muta2 && !p_ptr->inside_battle && !p_ptr->wild_mode)
	{
		if ((p_ptr->muta2 & MUT2_BERS_RAGE) && one_in_(3000))
		{
			disturb(0, 0);
#ifdef JP
msg_print("������������");
msg_print("���ܤ�ȯ��˽���줿��");
#else
			msg_print("RAAAAGHH!");
			msg_print("You feel a fit of rage coming over you!");
#endif

			(void)set_shero(10 + randint1(p_ptr->lev), FALSE);
		}

		if ((p_ptr->muta2 & MUT2_COWARDICE) && (randint1(3000) == 13))
		{
			if (!(p_ptr->resist_fear || p_ptr->hero || p_ptr->shero))
			{
				disturb(0, 0);
#ifdef JP
msg_print("�ȤƤ�Ť�... �ȤƤⶲ����");
#else
				msg_print("It's so dark... so scary!");
#endif

				set_afraid(p_ptr->afraid + 13 + randint1(26));
			}
		}

		if ((p_ptr->muta2 & MUT2_RTELEPORT) && (randint1(5000) == 88))
		{
			if (!p_ptr->resist_nexus && !(p_ptr->muta1 & MUT1_VTELEPORT) &&
			    !p_ptr->anti_tele)
			{
				disturb(0, 0);

				/* Teleport player */
#ifdef JP
msg_print("���ʤ��ΰ��֤������Ҥ��礦���Գ���ˤʤä�...");
#else
				msg_print("Your position suddenly seems very uncertain...");
#endif

				msg_print(NULL);
				teleport_player(40);
			}
		}

		if ((p_ptr->muta2 & MUT2_ALCOHOL) && (randint1(6400) == 321))
		{
			if (!p_ptr->resist_conf && !p_ptr->resist_chaos)
			{
				disturb(0, 0);
				p_ptr->redraw |= PR_EXTRA;
#ifdef JP
msg_print("���Ҥ����⡼���ȤҤƤ��������դ�...�ҥå���");
#else
				msg_print("You feel a SSSCHtupor cOmINg over yOu... *HIC*!");
#endif

			}

			if (!p_ptr->resist_conf)
			{
				(void)set_confused(p_ptr->confused + randint0(20) + 15);
			}

			if (!p_ptr->resist_chaos)
			{
				if (one_in_(20))
				{
					msg_print(NULL);
					if (one_in_(3)) lose_all_info();
					else wiz_dark();
					teleport_player(100);
					wiz_dark();
#ifdef JP
msg_print("���ʤ��ϸ��Τ�̾����ܤ��ä᤿...Ƭ���ˤ���");
msg_print("����Ф��Ƥ��ʤ����ɤ���äƤ������褿����ʬ����ʤ���");
#else
					msg_print("You wake up somewhere with a sore head...");
					msg_print("You can't remember a thing, or how you got here!");
#endif

				}
				else
				{
					if (one_in_(3))
					{
#ifdef JP
msg_print("�����줤�ʤ��礪�����Ȥ�줤���");
#else
						msg_print("Thishcischs GooDSChtuff!");
#endif

						(void)set_image(p_ptr->image + randint0(150) + 150);
					}
				}
			}
		}

		if ((p_ptr->muta2 & MUT2_HALLU) && (randint1(6400) == 42))
		{
			if (!p_ptr->resist_chaos)
			{
				disturb(0, 0);
				p_ptr->redraw |= PR_EXTRA;
				(void)set_image(p_ptr->image + randint0(50) + 20);
			}
		}

		if ((p_ptr->muta2 & MUT2_FLATULENT) && (randint1(3000) == 13))
		{
			disturb(0, 0);

#ifdef JP
msg_print("�֥������á����äȡ�");
#else
			msg_print("BRRAAAP! Oops.");
#endif

			msg_print(NULL);
			fire_ball(GF_POIS, 0, p_ptr->lev, 3);
		}

		if ((p_ptr->muta2 & MUT2_PROD_MANA) &&
		    !p_ptr->anti_magic && one_in_(9000))
		{
			int dire = 0;
			disturb(0, 0);
#ifdef JP
msg_print("��ˡ�Υ��ͥ륮�����������ʤ������ή�����Ǥ��������ͥ륮����������ʤ���Фʤ�ʤ���");
#else
			msg_print("Magical energy flows through you! You must release it!");
#endif

			flush();
			msg_print(NULL);
			(void)get_hack_dir(&dire);
			fire_ball(GF_MANA, dire, p_ptr->lev * 2, 3);
		}

		if ((p_ptr->muta2 & MUT2_ATT_DEMON) &&
		    !p_ptr->anti_magic && (randint1(6666) == 666))
		{
			bool pet = one_in_(6);
			u32b mode = PM_ALLOW_GROUP;

			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px,
				    dun_level, SUMMON_DEMON, mode))
			{
#ifdef JP
msg_print("���ʤ��ϥǡ���������󤻤���");
#else
				msg_print("You have attracted a demon!");
#endif

				disturb(0, 0);
			}
		}

		if ((p_ptr->muta2 & MUT2_SPEED_FLUX) && one_in_(6000))
		{
			disturb(0, 0);
			if (one_in_(2))
			{
#ifdef JP
msg_print("����Ū�Ǥʤ��ʤä��������롣");
#else
				msg_print("You feel less energetic.");
#endif

				if (p_ptr->fast > 0)
				{
					set_fast(0, TRUE);
				}
				else
				{
					set_slow(randint1(30) + 10, FALSE);
				}
			}
			else
			{
#ifdef JP
msg_print("����Ū�ˤʤä��������롣");
#else
				msg_print("You feel more energetic.");
#endif

				if (p_ptr->slow > 0)
				{
					set_slow(0, TRUE);
				}
				else
				{
					set_fast(randint1(30) + 10, FALSE);
				}
			}
			msg_print(NULL);
		}
		if ((p_ptr->muta2 & MUT2_BANISH_ALL) && one_in_(9000))
		{
			disturb(0, 0);
#ifdef JP
msg_print("�����ۤȤ�ɸ��Ȥˤʤä��������롣");
#else
			msg_print("You suddenly feel almost lonely.");
#endif

			banish_monsters(100);
			if (!dun_level && p_ptr->town_num)
			{
#ifdef JP
msg_print("Ź�μ�ͤ��֤˸����ä����äƤ��롪");
#else
				msg_print("You see one of the shopkeepers running for the hills!");
#endif

				store_shuffle(randint0(MAX_STORES));
			}
			msg_print(NULL);
		}

		if ((p_ptr->muta2 & MUT2_EAT_LIGHT) && one_in_(3000))
		{
			object_type *o_ptr;

#ifdef JP
msg_print("�ƤˤĤĤޤ줿��");
#else
			msg_print("A shadow passes over you.");
#endif

			msg_print(NULL);

			/* Absorb light from the current possition */
			if (cave[py][px].info & CAVE_GLOW)
			{
				hp_player(10);
			}

			o_ptr = &inventory[INVEN_LITE];

			/* Absorb some fuel in the current lite */
			if (o_ptr->tval == TV_LITE)
			{
				/* Use some fuel (except on artifacts) */
				if (!artifact_p(o_ptr) && (o_ptr->xtra4 > 0))
				{
					/* Heal the player a bit */
					hp_player(o_ptr->xtra4 / 20);

					/* Decrease life-span of lite */
					o_ptr->xtra4 /= 2;

#ifdef JP
msg_print("�������饨�ͥ륮����ۼ�������");
#else
					msg_print("You absorb energy from your light!");
#endif


					/* Notice interesting fuel steps */
					notice_lite_change(o_ptr);
				}
			}

			/*
			 * Unlite the area (radius 10) around player and
			 * do 50 points damage to every affected monster
			 */
			unlite_area(50, 10);
		}

		if ((p_ptr->muta2 & MUT2_ATT_ANIMAL) &&
		   !p_ptr->anti_magic && one_in_(7000))
		{
			bool pet = one_in_(3);
			u32b mode = PM_ALLOW_GROUP;

			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_ANIMAL, mode))
			{
#ifdef JP
msg_print("ưʪ������󤻤���");
#else
				msg_print("You have attracted an animal!");
#endif

				disturb(0, 0);
			}
		}

		if ((p_ptr->muta2 & MUT2_RAW_CHAOS) &&
		    !p_ptr->anti_magic && one_in_(8000))
		{
			disturb(0, 0);
#ifdef JP
msg_print("����ζ��֤��Ĥ�Ǥ��뵤�����롪");
#else
			msg_print("You feel the world warping around you!");
#endif

			msg_print(NULL);
			fire_ball(GF_CHAOS, 0, p_ptr->lev, 8);
		}
		if ((p_ptr->muta2 & MUT2_NORMALITY) && one_in_(5000))
		{
			if (!lose_mutation(0))
#ifdef JP
msg_print("��̯�ʤ��餤���̤ˤʤä��������롣");
#else
				msg_print("You feel oddly normal.");
#endif

		}
		if ((p_ptr->muta2 & MUT2_WRAITH) && !p_ptr->anti_magic && one_in_(3000))
		{
			disturb(0, 0);
#ifdef JP
msg_print("��ʪ����������");
#else
			msg_print("You feel insubstantial!");
#endif

			msg_print(NULL);
			set_wraith_form(randint1(p_ptr->lev / 2) + (p_ptr->lev / 2), FALSE);
		}
		if ((p_ptr->muta2 & MUT2_POLY_WOUND) && one_in_(3000))
		{
			do_poly_wounds();
		}
		if ((p_ptr->muta2 & MUT2_WASTING) && one_in_(3000))
		{
			int which_stat = randint0(6);
			int sustained = FALSE;

			switch (which_stat)
			{
			case A_STR:
				if (p_ptr->sustain_str) sustained = TRUE;
				break;
			case A_INT:
				if (p_ptr->sustain_int) sustained = TRUE;
				break;
			case A_WIS:
				if (p_ptr->sustain_wis) sustained = TRUE;
				break;
			case A_DEX:
				if (p_ptr->sustain_dex) sustained = TRUE;
				break;
			case A_CON:
				if (p_ptr->sustain_con) sustained = TRUE;
				break;
			case A_CHR:
				if (p_ptr->sustain_chr) sustained = TRUE;
				break;
			default:
#ifdef JP
msg_print("�����ʾ��֡�");
#else
				msg_print("Invalid stat chosen!");
#endif

				sustained = TRUE;
			}

			if (!sustained)
			{
				disturb(0, 0);
#ifdef JP
msg_print("��ʬ����夷�Ƥ����Τ�ʬ���롪");
#else
				msg_print("You can feel yourself wasting away!");
#endif

				msg_print(NULL);
				(void)dec_stat(which_stat, randint1(6) + 6, one_in_(3));
			}
		}
		if ((p_ptr->muta2 & MUT2_ATT_DRAGON) &&
		   !p_ptr->anti_magic && one_in_(3000))
		{
			bool pet = one_in_(5);
			u32b mode = PM_ALLOW_GROUP;

			if (pet) mode |= PM_FORCE_PET;
			else mode |= (PM_ALLOW_UNIQUE | PM_NO_PET);

			if (summon_specific((pet ? -1 : 0), py, px, dun_level, SUMMON_DRAGON, mode))
			{
#ifdef JP
msg_print("�ɥ饴�������󤻤���");
#else
				msg_print("You have attracted a dragon!");
#endif

				disturb(0, 0);
			}
		}
		if ((p_ptr->muta2 & MUT2_WEIRD_MIND) && !p_ptr->anti_magic &&
			one_in_(3000))
		{
			if (p_ptr->tim_esp > 0)
			{
#ifdef JP
msg_print("�����ˤ�䤬�����ä���");
#else
				msg_print("Your mind feels cloudy!");
#endif

				set_tim_esp(0, TRUE);
			}
			else
			{
#ifdef JP
msg_print("�����������ä���");
#else
				msg_print("Your mind expands!");
#endif

				set_tim_esp(p_ptr->lev, FALSE);
			}
		}
		if ((p_ptr->muta2 & MUT2_NAUSEA) && !p_ptr->slow_digest &&
			one_in_(9000))
		{
			disturb(0, 0);
#ifdef JP
msg_print("�ߤ���ڻ���������򼺤ä���");
#else
			msg_print("Your stomach roils, and you lose your lunch!");
#endif

			msg_print(NULL);
			set_food(PY_FOOD_WEAK);
		}

		if ((p_ptr->muta2 & MUT2_WALK_SHAD) &&
		   !p_ptr->anti_magic && one_in_(12000) && !p_ptr->inside_arena)
		{
			alter_reality();
		}

		if ((p_ptr->muta2 & MUT2_WARNING) && one_in_(1000))
		{
			int danger_amount = 0;
			int monster;

			for (monster = 0; monster < m_max; monster++)
			{
				monster_type    *m_ptr = &m_list[monster];
				monster_race    *r_ptr = &r_info[m_ptr->r_idx];

				/* Paranoia -- Skip dead monsters */
				if (!m_ptr->r_idx) continue;

				if (r_ptr->level >= p_ptr->lev)
				{
					danger_amount += r_ptr->level - p_ptr->lev + 1;
				}
			}

			if (danger_amount > 100)
#ifdef JP
msg_print("���˶������������롪");
#else
				msg_print("You feel utterly terrified!");
#endif

			else if (danger_amount > 50)
#ifdef JP
msg_print("�������������롪");
#else
				msg_print("You feel terrified!");
#endif

			else if (danger_amount > 20)
#ifdef JP
msg_print("���˿��ۤʵ������롪");
#else
				msg_print("You feel very worried!");
#endif

			else if (danger_amount > 10)
#ifdef JP
msg_print("���ۤʵ������롪");
#else
				msg_print("You feel paranoid!");
#endif

			else if (danger_amount > 5)
#ifdef JP
msg_print("�ۤȤ�ɰ����ʵ������롣");
#else
				msg_print("You feel almost safe.");
#endif

			else
#ifdef JP
msg_print("�䤷���������롣");
#else
				msg_print("You feel lonely.");
#endif

		}
		if ((p_ptr->muta2 & MUT2_INVULN) && !p_ptr->anti_magic &&
			one_in_(5000))
		{
			disturb(0, 0);
#ifdef JP
msg_print("̵Ũ�ʵ������롪");
#else
			msg_print("You feel invincible!");
#endif

			msg_print(NULL);
			(void)set_invuln(randint1(8) + 8, FALSE);
		}
		if ((p_ptr->muta2 & MUT2_SP_TO_HP) && one_in_(2000))
		{
			int wounds = p_ptr->mhp - p_ptr->chp;

			if (wounds > 0)
			{
				int healing = p_ptr->csp;

				if (healing > wounds)
				{
					healing = wounds;
				}

				hp_player(healing);
				p_ptr->csp -= healing;
			}
		}
		if ((p_ptr->muta2 & MUT2_HP_TO_SP) && !p_ptr->anti_magic &&
			one_in_(4000))
		{
			int wounds = p_ptr->msp - p_ptr->csp;

			if (wounds > 0)
			{
				int healing = p_ptr->chp;

				if (healing > wounds)
				{
					healing = wounds;
				}

				p_ptr->csp += healing;
#ifdef JP
take_hit(DAMAGE_LOSELIFE, healing, "Ƭ�˾��ä���", -1);
#else
				take_hit(DAMAGE_LOSELIFE, healing, "blood rushing to the head", -1);
#endif

			}
		}
		if ((p_ptr->muta2 & MUT2_DISARM) && one_in_(10000))
		{
			object_type *o_ptr;

			disturb(0, 0);
#ifdef JP
msg_print("­����Ĥ��ž�����");
take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), "ž��", -1);
#else
			msg_print("You trip over your own feet!");
			take_hit(DAMAGE_NOESCAPE, randint1(p_ptr->wt / 6), "tripping", -1);
#endif


			msg_print(NULL);
			if (buki_motteruka(INVEN_RARM))
			{
				int slot = INVEN_RARM;
				o_ptr = &inventory[INVEN_RARM];
				if (buki_motteruka(INVEN_LARM) && one_in_(2))
				{
					o_ptr = &inventory[INVEN_LARM];
					slot = INVEN_LARM;
				}
				if (!cursed_p(o_ptr))
				{
#ifdef JP
msg_print("������Ƥ��ޤä���");
#else
					msg_print("You drop your weapon!");
#endif

					inven_drop(slot, 1);
				}
			}
		}
	}


	/*** Process Inventory ***/

	if ((p_ptr->cursed & TRC_P_FLAG_MASK) && !p_ptr->wild_mode)
	{
		/*
		 * Hack: Uncursed teleporting items (e.g. Trump Weapons)
		 * can actually be useful!
		 */
		if ((p_ptr->cursed & TRC_TELEPORT_SELF) && one_in_(200))
		{
#ifdef JP
if (get_check_strict("�ƥ�ݡ��Ȥ��ޤ�����", CHECK_OKAY_CANCEL))
#else
			if (get_check_strict("Teleport? ", CHECK_OKAY_CANCEL))
#endif
			{
				disturb(0, 0);
				teleport_player(50);
			}
		}
		/* Make a chainsword noise */
		if ((p_ptr->cursed & TRC_CHAINSWORD) && one_in_(CHAINSWORD_NOISE))
		{
			char noise[1024];
#ifdef JP
if (!get_rnd_line("chainswd_j.txt", 0, noise))
#else
			if (!get_rnd_line("chainswd.txt", 0, noise))
#endif
				msg_print(noise);
			disturb(FALSE, FALSE);
		}
		/* TY Curse */
		if ((p_ptr->cursed & TRC_TY_CURSE) && one_in_(TY_CURSE_CHANCE))
		{
			int count = 0;
			(void)activate_ty_curse(FALSE, &count);
		}
		/* Handle experience draining */
		if (p_ptr->prace != RACE_ANDROID && 
			((p_ptr->cursed & TRC_DRAIN_EXP) && one_in_(4)))
		{
			p_ptr->exp -= (p_ptr->lev+1)/2;
			if (p_ptr->exp < 0) p_ptr->exp = 0;
			p_ptr->max_exp -= (p_ptr->lev+1)/2;
			if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;
			check_experience();
		}
		/* Add light curse (Later) */
		if ((p_ptr->cursed & TRC_ADD_L_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			o_ptr = choose_cursed_obj_name(TRC_ADD_L_CURSE);

			new_curse = get_curse(0, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, FALSE, 0);

				o_ptr->curse_flags |= new_curse;
#ifdef JP
msg_format("���դ����������������餬%s��Ȥ�ޤ���...", o_name);
#else
				msg_format("There is a malignant black aura surrounding %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				p_ptr->update |= (PU_BONUS);
			}
		}
		/* Add heavy curse (Later) */
		if ((p_ptr->cursed & TRC_ADD_H_CURSE) && one_in_(2000))
		{
			u32b new_curse;
			object_type *o_ptr;

			o_ptr = choose_cursed_obj_name(TRC_ADD_H_CURSE);

			new_curse = get_curse(1, o_ptr);
			if (!(o_ptr->curse_flags & new_curse))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, o_ptr, FALSE, 0);

				o_ptr->curse_flags |= new_curse;
#ifdef JP
msg_format("���դ����������������餬%s��Ȥ�ޤ���...", o_name);
#else
				msg_format("There is a malignant black aura surrounding %s...", o_name);
#endif

				o_ptr->feeling = FEEL_NONE;

				p_ptr->update |= (PU_BONUS);
			}
		}
		/* Call animal */
		if ((p_ptr->cursed & TRC_CALL_ANIMAL) && one_in_(2500))
		{
			if (summon_specific(0, py, px, dun_level, SUMMON_ANIMAL,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_ANIMAL), FALSE, 0);
#ifdef JP
msg_format("%s��ưʪ������󤻤���", o_name);
#else
				msg_format("%s have attracted an animal!", o_name);
#endif

				disturb(0, 0);
			}
		}
		/* Call demon */
		if ((p_ptr->cursed & TRC_CALL_DEMON) && one_in_(1111))
		{
			if (summon_specific(0, py, px, dun_level, SUMMON_DEMON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DEMON), FALSE, 0);
#ifdef JP
msg_format("%s�����������󤻤���", o_name);
#else
				msg_format("%s have attracted a demon!", o_name);
#endif

				disturb(0, 0);
			}
		}
		/* Call dragon */
		if ((p_ptr->cursed & TRC_CALL_DRAGON) && one_in_(800))
		{
			if (summon_specific(0, py, px, dun_level, SUMMON_DRAGON,
			    (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET)))
			{
				char o_name[MAX_NLEN];

				object_desc(o_name, choose_cursed_obj_name(TRC_CALL_DRAGON), FALSE, 0);
#ifdef JP
msg_format("%s���ɥ饴�������󤻤���", o_name);
#else
				msg_format("%s have attracted an animal!", o_name);
#endif

				disturb(0, 0);
			}
		}
		if ((p_ptr->cursed & TRC_COWARDICE) && one_in_(1500))
		{
			if (!(p_ptr->resist_fear || p_ptr->hero || p_ptr->shero))
			{
				disturb(0, 0);
#ifdef JP
msg_print("�ȤƤ�Ť�... �ȤƤⶲ����");
#else
				msg_print("It's so dark... so scary!");
#endif

				set_afraid(p_ptr->afraid + 13 + randint1(26));
			}
		}
		/* Teleport player */
		if ((p_ptr->cursed & TRC_TELEPORT) && one_in_(200) && !p_ptr->anti_tele)
		{
			disturb(0, 0);

			/* Teleport player */
			teleport_player(40);
		}
		/* Handle HP draining */
		if ((p_ptr->cursed & TRC_DRAIN_HP) && one_in_(666))
		{
			char o_name[MAX_NLEN];

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_HP), FALSE, 0);
#ifdef JP
msg_format("%s�Ϥ��ʤ������Ϥ�ۼ�������", o_name);
#else
			msg_format("%s drains HP from you!", o_name);
#endif
			take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev*2, 100), o_name, -1);
		}
		/* Handle mana draining */
		if ((p_ptr->cursed & TRC_DRAIN_MANA) && one_in_(666))
		{
			char o_name[MAX_NLEN];

			object_desc(o_name, choose_cursed_obj_name(TRC_DRAIN_MANA), FALSE, 0);
#ifdef JP
msg_format("%s�Ϥ��ʤ������Ϥ�ۼ�������", o_name);
#else
			msg_format("%s drains mana from you!", o_name);
#endif
			p_ptr->csp -= MIN(p_ptr->lev, 50);
			if (p_ptr->csp < 0)
			{
				p_ptr->csp = 0;
				p_ptr->csp_frac = 0;
			}
			p_ptr->redraw |= PR_MANA;
		}
	}

	/* Rarely, take damage from the Jewel of Judgement */
	if (one_in_(999) && !p_ptr->anti_magic)
	{
		if ((inventory[INVEN_LITE].tval) &&
		    (inventory[INVEN_LITE].sval == SV_LITE_THRAIN))
		{
#ifdef JP
msg_print("�ؿ�Ƚ�����С٤Ϥ��ʤ������Ϥ�ۼ�������");
take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev, 50), "��Ƚ������", -1);
#else
			msg_print("The Jewel of Judgement drains life from you!");
			take_hit(DAMAGE_LOSELIFE, MIN(p_ptr->lev, 50), "the Jewel of Judgement", -1);
#endif

		}
	}


	/* Process equipment */
	for (j = 0, i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Get the object */
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge activatable objects */
		if (o_ptr->timeout > 0)
		{
			/* Recharge */
			o_ptr->timeout--;

			/* Notice changes */
			if (!o_ptr->timeout)
			{
				recharged_notice(o_ptr);
				j++;
			}
		}
	}

	/* Notice changes */
	if (j)
	{
		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
		wild_regen = 20;
	}

	/*
	 * Recharge rods.  Rods now use timeout to control charging status,
	 * and each charging rod in a stack decreases the stack's timeout by
	 * one per turn. -LM-
	 */
	for (j = 0, i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Examine all charging rods or stacks of charging rods. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Determine how many rods are charging. */
			temp = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
			if (temp > o_ptr->number) temp = o_ptr->number;

			/* Decrease timeout by that number. */
			o_ptr->timeout -= temp;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;

			/* Notice changes, provide message if object is inscribed. */
			if (!(o_ptr->timeout))
			{
				recharged_notice(o_ptr);
				j++;
			}
		}
	}

	/* Notice changes */
	if (j)
	{
		/* Combine pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
		wild_regen = 20;
	}

	/* Feel the inventory */
	sense_inventory1();
	sense_inventory2();


	/*** Process Objects ***/

	/* Process objects */
	for (i = 1; i < o_max; i++)
	{
		/* Access object */
		o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Recharge rods on the ground.  No messages. */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			/* Charge it */
			o_ptr->timeout -= o_ptr->number;

			/* Boundary control. */
			if (o_ptr->timeout < 0) o_ptr->timeout = 0;
		}
	}


	/*** Involuntary Movement ***/

	/* Delayed Word-of-Recall */
	if (p_ptr->word_recall)
	{
		/*
		 * HACK: Autosave BEFORE resetting the recall counter (rr9)
		 * The player is yanked up/down as soon as
		 * he loads the autosaved game.
		 */
		if (autosave_l && (p_ptr->word_recall == 1) && !p_ptr->inside_battle)
			do_cmd_save_game(TRUE);

		/* Count down towards recall */
		p_ptr->word_recall--;

		p_ptr->redraw |= (PR_STATUS);

		/* Activate the recall */
		if (!p_ptr->word_recall)
		{
			/* Disturbing! */
			disturb(0, 0);

			/* Determine the level */
			if (dun_level || p_ptr->inside_quest)
			{
#ifdef JP
msg_print("��˰���ĥ�ꤢ�����봶�������롪");
#else
				msg_print("You feel yourself yanked upwards!");
#endif

				p_ptr->recall_dungeon = dungeon_type;
				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				dun_level = 0;
				dungeon_type = 0;

				leave_quest_check();

				p_ptr->inside_quest = 0;
				p_ptr->leaving = TRUE;
			}
			else
			{
#ifdef JP
msg_print("���˰�������ߤ���봶�������롪");
#else
				msg_print("You feel yourself yanked downwards!");
#endif

				dungeon_type = p_ptr->recall_dungeon;

				if (record_stair)
					do_cmd_write_nikki(NIKKI_RECALL, dun_level, NULL);

				/* New depth */
				dun_level = max_dlv[dungeon_type];
				if (dun_level < 1) dun_level = 1;

				/* Nightmare mode makes recall more dangerous */
				if (ironman_nightmare && !randint0(666) && (dungeon_type == DUNGEON_ANGBAND))
				{
					if (dun_level < 50)
					{
						dun_level *= 2;
					}
					else if (dun_level < 99)
					{
						dun_level = (dun_level + 99) / 2;
					}
					else if (dun_level > 100)
					{
						dun_level = d_info[dungeon_type].maxdepth - 1;
					}
				}

				if (p_ptr->wild_mode)
				{
					p_ptr->wilderness_y = py;
					p_ptr->wilderness_x = px;
				}
				else
				{
					/* Save player position */
					p_ptr->oldpx = px;
					p_ptr->oldpy = py;
				}
				p_ptr->wild_mode = FALSE;

				/* Leaving */
				p_ptr->leaving = TRUE;

				if (dungeon_type == DUNGEON_ANGBAND)
				{
					for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
					{
						if ((quest[i].type == QUEST_TYPE_RANDOM) &&
						    (quest[i].status == QUEST_STATUS_TAKEN) &&
						    (quest[i].level < dun_level))
						{
							quest[i].status = QUEST_STATUS_FAILED;
							quest[i].complev = (byte)p_ptr->lev;
							r_info[quest[i].r_idx].flags1 &= ~(RF1_QUESTOR);
						}
					}
				}
			}

			/* Sound */
			sound(SOUND_TPLEVEL);
		}
	}
}



/*
 * Verify use of "wizard" mode
 */
static bool enter_wizard_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Wizard mode is not permitted */
		if (!allow_debug_opts)
		{
#ifdef JP
			msg_print("���������ɥ⡼�ɤϵ��Ĥ���Ƥ��ޤ��� ");
#else
			msg_print("Wizard mode is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
msg_print("���������ɥ⡼�ɤϥǥХ��ȼ¸��Τ���Υ⡼�ɤǤ��� ");
msg_print("���٥��������ɥ⡼�ɤ�����ȥ������ϵ�Ͽ����ޤ���");
#else
		msg_print("Wizard mode is for debugging and experimenting.");
		msg_print("The game will not be scored if you enter wizard mode.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
if (!get_check("�����˥��������ɥ⡼�ɤ����ꤿ���ΤǤ���? "))
#else
		if (!get_check("Are you sure you want to enter wizard mode? "))
#endif

		{
			return (FALSE);
		}

		/* Mark savefile */
		p_ptr->noscore |= 0x0002;
	}

	/* Success */
	return (TRUE);
}


#ifdef ALLOW_WIZARD

/*
 * Verify use of "debug" commands
 */
static bool enter_debug_mode(void)
{
	/* Ask first time */
	if (!p_ptr->noscore)
	{
		/* Debug mode is not permitted */
		if (!allow_debug_opts)
		{
#ifdef JP
			msg_print("�ǥХå����ޥ�ɤϵ��Ĥ���Ƥ��ޤ��� ");
#else
			msg_print("Use of debug command is not permitted.");
#endif
			return FALSE;
		}

		/* Mention effects */
#ifdef JP
msg_print("�ǥХ������ޥ�ɤϥǥХ��ȼ¸��Τ���Υ��ޥ�ɤǤ��� ");
msg_print("�ǥХ������ޥ�ɤ�Ȥ��ȥ������ϵ�Ͽ����ޤ���");
#else
		msg_print("The debug commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use debug commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
if (!get_check("�����˥ǥХ������ޥ�ɤ�Ȥ��ޤ���? "))
#else
		if (!get_check("Are you sure you want to use debug commands? "))
#endif

		{
			return (FALSE);
		}

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�ǥХå��⡼�ɤ��������ƥ�������Ĥ��ʤ��ʤä���");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up sending score to use debug commands.");
#endif
		/* Mark savefile */
		p_ptr->noscore |= 0x0008;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Debug Routines
 */
extern void do_cmd_debug(void);

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

/*
 * Verify use of "borg" commands
 */
static bool enter_borg_mode(void)
{
	/* Ask first time */
	if (!(p_ptr->noscore & 0x0010))
	{
		/* Mention effects */
#ifdef JP
msg_print("�ܡ��������ޥ�ɤϥǥХ��ȼ¸��Τ���Υ��ޥ�ɤǤ��� ");
msg_print("�ܡ��������ޥ�ɤ�Ȥ��ȥ������ϵ�Ͽ����ޤ���");
#else
		msg_print("The borg commands are for debugging and experimenting.");
		msg_print("The game will not be scored if you use borg commands.");
#endif

		msg_print(NULL);

		/* Verify request */
#ifdef JP
if (!get_check("�����˥ܡ��������ޥ�ɤ�Ȥ��ޤ���? "))
#else
		if (!get_check("Are you sure you want to use borg commands? "))
#endif

		{
			return (FALSE);
		}

		/* Mark savefile */
		p_ptr->noscore |= 0x0010;
	}

	/* Success */
	return (TRUE);
}

/*
 * Hack -- Declare the Ben Borg
 */
extern void do_cmd_borg(void);

#endif /* ALLOW_BORG */



/*
 * Parse and execute the current command
 * Give "Warning" on illegal commands.
 *
 * XXX XXX XXX Make some "blocks"
 */
static void process_command(void)
{
	int old_now_message = now_message;

#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	now_message = 0;

	/* Parse the command */
	switch (command_cmd)
	{
		/* Ignore */
		case ESCAPE:
		case ' ':
		{
			break;
		}

		/* Ignore return */
		case '\r':
		case '\n':
		{
			break;
		}

		/*** Wizard Commands ***/

		/* Toggle Wizard Mode */
		case KTRL('W'):
		{
			if (p_ptr->wizard)
			{
				p_ptr->wizard = FALSE;
#ifdef JP
msg_print("���������ɥ⡼�ɲ����");
#else
				msg_print("Wizard mode off.");
#endif

			}
			else if (enter_wizard_mode())
			{
				p_ptr->wizard = TRUE;
#ifdef JP
msg_print("���������ɥ⡼��������");
#else
				msg_print("Wizard mode on.");
#endif

			}

			/* Update monsters */
			p_ptr->update |= (PU_MONSTERS);

			/* Redraw "title" */
			p_ptr->redraw |= (PR_TITLE);

			break;
		}


#ifdef ALLOW_WIZARD

		/* Special "debug" commands */
		case KTRL('A'):
		{
			/* Enter debug mode */
			if (enter_debug_mode())
			{
				do_cmd_debug();
			}
			break;
		}

#endif /* ALLOW_WIZARD */


#ifdef ALLOW_BORG

		/* Special "borg" commands */
		case KTRL('Z'):
		{
			/* Enter borg mode */
			if (enter_borg_mode())
			{
				if (!p_ptr->wild_mode) do_cmd_borg();
			}

			break;
		}

#endif /* ALLOW_BORG */



		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			if (!p_ptr->wild_mode) do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			if (!p_ptr->wild_mode) do_cmd_takeoff();
			break;
		}

		/* Drop an item */
		case 'd':
		{
			if (!p_ptr->wild_mode) do_cmd_drop();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}


		/*** Standard "Movement" Commands ***/

		/* Alter a grid */
		case '+':
		{
			if (!p_ptr->wild_mode) do_cmd_alter();
			break;
		}

		/* Dig a tunnel */
		case 'T':
		{
			if (!p_ptr->wild_mode) do_cmd_tunnel();
			break;
		}

		/* Move (usually pick up things) */
		case ';':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}

		/* Move (usually do not pick up) */
		case '-':
		{
#ifdef ALLOW_EASY_DISARM /* TNB */

			do_cmd_walk(TRUE);

#else /* ALLOW_EASY_DISARM -- TNB */

			do_cmd_walk(!always_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

			break;
		}


		/*** Running, Resting, Searching, Staying */

		/* Begin Running -- Arg is Max Distance */
		case '.':
		{
			if (!p_ptr->wild_mode) do_cmd_run();
			break;
		}

		/* Stay still (usually pick things up) */
		case ',':
		{
			do_cmd_stay(always_pickup);
			break;
		}

		/* Stay still (usually do not pick up) */
		case 'g':
		{
			do_cmd_stay(!always_pickup);
			break;
		}

		/* Rest -- Arg is time */
		case 'R':
		{
			do_cmd_rest();
			break;
		}

		/* Search for traps/doors */
		case 's':
		{
			do_cmd_search();
			break;
		}

		/* Toggle search mode */
		case 'S':
		{
			if (p_ptr->action == ACTION_SEARCH) set_action(ACTION_NONE);
			else set_action(ACTION_SEARCH);
			break;
		}


		/*** Stairs and Doors and Chests and Traps ***/

		/* Enter store */
		case SPECIAL_KEY_STORE:
		{
			if (!p_ptr->wild_mode) do_cmd_store();
			break;
		}

		/* Enter building -KMW- */
		case SPECIAL_KEY_BUILDING:
		{
			if (!p_ptr->wild_mode) do_cmd_bldg();
			break;
		}

		/* Enter quest level -KMW- */
		case SPECIAL_KEY_QUEST:
		{
			if (!p_ptr->wild_mode) do_cmd_quest();
			break;
		}

		/* Go up staircase */
		case '<':
		{
			if(!p_ptr->wild_mode && !dun_level && !p_ptr->inside_arena && !p_ptr->inside_quest)
			{
				if (!vanilla_town)
				{
					if(ambush_flag)
					{
#ifdef JP
						msg_print("���⤫��ƨ����ˤϥޥåפ�ü�ޤǰ�ư���ʤ���Фʤ�ʤ���");
#else
						msg_print("To flee the ambush you have to reach the edge of the map.");
#endif
					}
					else if (p_ptr->food < PY_FOOD_WEAK)
					{
#ifdef JP
						msg_print("�������˿�����Ȥ�ʤ��ȡ�");
#else
						msg_print("You must eat something here.");
#endif
					}
					else
					{
						if (change_wild_mode())
						{
							p_ptr->oldpx = px;
							p_ptr->oldpy = py;
						}
					}
				}
			}
			else
				do_cmd_go_up();
			break;
		}

		/* Go down staircase */
		case '>':
		{
			if(!p_ptr->wild_mode) do_cmd_go_down();
			else
			{
				p_ptr->wilderness_x = px;
				p_ptr->wilderness_y = py;
				change_wild_mode();
			}
			break;
		}

		/* Open a door or chest */
		case 'o':
		{
			if (!p_ptr->wild_mode) do_cmd_open();
			break;
		}

		/* Close a door */
		case 'c':
		{
			if (!p_ptr->wild_mode) do_cmd_close();
			break;
		}

		/* Jam a door with spikes */
		case 'j':
		{
			if (!p_ptr->wild_mode) do_cmd_spike();
			break;
		}

		/* Bash a door */
		case 'B':
		{
			if (!p_ptr->wild_mode) do_cmd_bash();
			break;
		}

		/* Disarm a trap or chest */
		case 'D':
		{
			if (!p_ptr->wild_mode) do_cmd_disarm();
			break;
		}


		/*** Magic and Prayers ***/

		/* Gain new spells/prayers */
		case 'G':
		{
			if ((p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE))
#ifdef JP
				msg_print("��ʸ��ؽ�����ɬ�פϤʤ���");
#else
				msg_print("You don't have to learn spells!");
#endif
			else if (p_ptr->pclass == CLASS_SAMURAI)
				do_cmd_gain_hissatsu();
			else if (p_ptr->pclass == CLASS_MAGIC_EATER)
				gain_magic();
			else
				do_cmd_study();
			break;
		}

		/* Browse a book */
		case 'b':
		{
			if ( (p_ptr->pclass == CLASS_MINDCRAFTER) ||
			     (p_ptr->pclass == CLASS_BERSERKER) ||
			     (p_ptr->pclass == CLASS_NINJA) ||
			     (p_ptr->pclass == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse();
			else if (p_ptr->pclass == CLASS_SMITH)
				do_cmd_kaji(TRUE);
			else if (p_ptr->pclass == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(TRUE);
			else do_cmd_browse();
			break;
		}

		/* Cast a spell */
		case 'm':
		{
			/* -KMW- */
			if (!p_ptr->wild_mode)
			{
				if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_ARCHER) || (p_ptr->pclass == CLASS_CAVALRY))
				{
#ifdef JP
					msg_print("��ʸ�򾧤����ʤ���");
#else
					msg_print("You cannot cast spells!");
#endif
				}
				else if (dun_level && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC) && (p_ptr->pclass != CLASS_BERSERKER) && (p_ptr->pclass != CLASS_SMITH))
				{
#ifdef JP
					msg_print("���󥸥����ˡ��ۼ�������");
#else
					msg_print("The dungeon absorbs all attempted magic!");
#endif
					msg_print(NULL);
				}
				else if (p_ptr->anti_magic && (p_ptr->pclass != CLASS_BERSERKER) && (p_ptr->pclass != CLASS_SMITH))
				{
#ifdef JP

					cptr which_power = "��ˡ";
#else
					cptr which_power = "magic";
#endif
					if (p_ptr->pclass == CLASS_MINDCRAFTER)
#ifdef JP
						which_power = "Ķǽ��";
#else
						which_power = "psionic powers";
#endif
					else if (p_ptr->pclass == CLASS_IMITATOR)
#ifdef JP
						which_power = "��Τޤ�";
#else
						which_power = "imitation";
#endif
					else if (p_ptr->pclass == CLASS_SAMURAI)
#ifdef JP
						which_power = "ɬ����";
#else
						which_power = "hissatsu";
#endif
					else if (p_ptr->pclass == CLASS_MIRROR_MASTER)
#ifdef JP
						which_power = "����ˡ";
#else
						which_power = "mirror magic";
#endif
					else if (p_ptr->pclass == CLASS_NINJA)
#ifdef JP
						which_power = "Ǧ��";
#else
						which_power = "ninjutsu";
#endif
					else if (mp_ptr->spell_book == TV_LIFE_BOOK)
#ifdef JP
						which_power = "����";
#else
						which_power = "prayer";
#endif

#ifdef JP
					msg_format("ȿ��ˡ�Хꥢ��%s����⤷����", which_power);
#else
					msg_format("An anti-magic shell disrupts your %s!", which_power);
#endif
					energy_use = 0;
				}
				else if (p_ptr->shero && (p_ptr->pclass != CLASS_BERSERKER))
				{
#ifdef JP
					msg_format("����β����Ƥ���Ƭ�����ʤ���");
#else
					msg_format("You cannot think directly!");
#endif
					energy_use = 0;
				}
				else
				{
					if ((p_ptr->pclass == CLASS_MINDCRAFTER) ||
					    (p_ptr->pclass == CLASS_BERSERKER) ||
					    (p_ptr->pclass == CLASS_NINJA) ||
					    (p_ptr->pclass == CLASS_MIRROR_MASTER)
					    )
						do_cmd_mind();
					else if (p_ptr->pclass == CLASS_IMITATOR)
						do_cmd_mane(FALSE);
					else if (p_ptr->pclass == CLASS_MAGIC_EATER)
						do_cmd_magic_eater(FALSE);
					else if (p_ptr->pclass == CLASS_SAMURAI)
						do_cmd_hissatsu();
					else if (p_ptr->pclass == CLASS_BLUE_MAGE)
						do_cmd_cast_learned();
					else if (p_ptr->pclass == CLASS_SMITH)
						do_cmd_kaji(FALSE);
					else
						do_cmd_cast();
				}
			}
			break;
		}

		/* Issue a pet command */
		case 'p':
		{
			if (!p_ptr->wild_mode) do_cmd_pet();
			break;
		}

		/*** Use various objects ***/

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}

		/* Activate an artifact */
		case 'A':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_activate();
			else
			{
#ifdef JP
msg_print("���꡼�ʤ���ˡ��ۼ�������");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Eat some food */
		case 'E':
		{
			do_cmd_eat_food();
			break;
		}

		/* Fuel your lantern/torch */
		case 'F':
		{
			do_cmd_refill();
			break;
		}

		/* Fire an item */
		case 'f':
		{
			if (!p_ptr->wild_mode) do_cmd_fire();
			break;
		}

		/* Throw an item */
		case 'v':
		{
			if (!p_ptr->wild_mode)
			{
				do_cmd_throw();
			}
			break;
		}

		/* Aim a wand */
		case 'a':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_aim_wand();
			else
			{
#ifdef JP
msg_print("���꡼�ʤ���ˡ��ۼ�������");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Zap a rod */
		case 'z':
		{
			if (!p_ptr->wild_mode)
			{
			if (p_ptr->inside_arena)
			{
#ifdef JP
msg_print("���꡼�ʤ���ˡ��ۼ�������");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && rogue_like_commands)
			{
				do_cmd_use();
			}
			else
			{
				do_cmd_zap_rod();
			}
			}
			break;
		}

		/* Quaff a potion */
		case 'q':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_quaff_potion();
			else
			{
#ifdef JP
msg_print("���꡼�ʤ���ˡ��ۼ�������");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Read a scroll */
		case 'r':
		{
			if (!p_ptr->wild_mode)
			{
			if (!p_ptr->inside_arena)
				do_cmd_read_scroll();
			else
			{
#ifdef JP
msg_print("���꡼�ʤ���ˡ��ۼ�������");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			}
			break;
		}

		/* Use a staff */
		case 'u':
		{
			if (!p_ptr->wild_mode)
			{
			if (p_ptr->inside_arena)
			{
#ifdef JP
msg_print("���꡼�ʤ���ˡ��ۼ�������");
#else
				msg_print("The arena absorbs all attempted magic!");
#endif

				msg_print(NULL);
			}
			else if (use_command && !rogue_like_commands)
			{
				do_cmd_use();
			}
			else
				do_cmd_use_staff();
			}
			break;
		}

		/* Use racial power */
		case 'U':
		{
			if (!p_ptr->wild_mode) do_cmd_racial_power();
			break;
		}


		/*** Looking at Things (nearby or on map) ***/

		/* Full dungeon map */
		case 'M':
		{
			do_cmd_view_map();
			break;
		}

		/* Locate player on map */
		case 'L':
		{
			do_cmd_locate();
			break;
		}

		/* Look around */
		case 'l':
		{
			do_cmd_look();
			break;
		}

		/* Target monster or location */
		case '*':
		{
			if (!p_ptr->wild_mode) do_cmd_target();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		case '$':
		{
			do_cmd_pickpref();
			break;
		}

		case '_':
		{
			do_cmd_edit_autopick();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			do_cmd_redraw();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			do_cmd_redraw();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			do_cmd_redraw();
			break;
		}

		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			if (!p_ptr->wild_mode) do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(old_now_message);
			break;
		}

		/* Show quest status -KMW- */
		case KTRL('Q'):
		{
			do_cmd_checkquest();
			break;
		}

		/* Redraw the screen */
		case KTRL('R'):
		{
			now_message = old_now_message;
			do_cmd_redraw();
			break;
		}

#ifndef VERIFY_SAVEFILE

		/* Hack -- Save and don't quit */
		case KTRL('S'):
		{
			do_cmd_save_game(FALSE);
			break;
		}

#endif /* VERIFY_SAVEFILE */

		case KTRL('T'):
		{
			do_cmd_time();
			break;
		}

		/* Save and quit */
		case KTRL('X'):
		case SPECIAL_KEY_QUIT:
		{
			do_cmd_save_and_exit();
			break;
		}

		/* Quit (commit suicide) */
		case 'Q':
		{
			do_cmd_suicide();
			break;
		}

		case '|':
		{
			do_cmd_nikki();
			break;
		}

		/* Check artifacts, uniques, objects */
		case '~':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Make random artifact list */
		case KTRL('V'):
		{
			spoil_random_artifact("randifact.txt");
			break;
		}

		/* Hack -- Unknown command */
		default:
		{
			if (flush_failure) flush();
			if (one_in_(2))
			{
				char error_m[1024];
				sound(SOUND_ILLEGAL);
#ifdef JP
				if (!get_rnd_line("error_j.txt", 0, error_m))
#else
				if (!get_rnd_line("error.txt", 0, error_m))
#endif

					msg_print(error_m);
			}
			else
#ifdef JP
prt(" '?' �ǥإ�פ�ɽ������ޤ���", 0, 0);
#else
				prt("Type '?' for help.", 0, 0);
#endif

			break;
		}
	}
	if (!energy_use && !now_message)
		now_message = old_now_message;
}




static bool monster_tsuri(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if ((r_ptr->flags7 & RF7_AQUATIC) && !(r_ptr->flags1 & RF1_UNIQUE) && strchr("Jjlw", r_ptr->d_char))
		return TRUE;
	else
		return FALSE;
}


/*
 * Process the player
 *
 * Notice the annoying code to handle "pack overflow", which
 * must come first just in case somebody manages to corrupt
 * the savefiles by clever use of menu commands or something.
 */
static void process_player(void)
{
	int i;

	/*** Apply energy ***/

	if (hack_mutation)
	{
#ifdef JP
msg_print("�����Ѥ�ä��������롪");
#else
		msg_print("You feel different!");
#endif

		(void)gain_random_mutation(0);
		hack_mutation = FALSE;
	}

	if (p_ptr->inside_battle)
	{
		for(i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			if (!m_ptr->r_idx) continue;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);
		}
		prt_time();
	}

	/* Give the player some energy */
	else if (!(load && p_ptr->energy_need <= 0))
	{
		p_ptr->energy_need -= (p_ptr->pspeed > 199 ? 49 : (p_ptr->pspeed < 0 ? 1 : extract_energy[p_ptr->pspeed]));
	}

	/* No turn yet */
	if (p_ptr->energy_need > 0) return;
	if (!command_rep) prt_time();

	/*** Check for interupts ***/

	/* Complete resting */
	if (resting < 0)
	{
		/* Basic resting */
		if (resting == -1)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp))
			{
				set_action(ACTION_NONE);
			}
		}

		/* Complete resting */
		else if (resting == -2)
		{
			/* Stop resting */
			if ((p_ptr->chp == p_ptr->mhp) &&
			    (p_ptr->csp >= p_ptr->msp) &&
			    !p_ptr->blind && !p_ptr->confused &&
			    !p_ptr->poisoned && !p_ptr->afraid &&
			    !p_ptr->stun && !p_ptr->cut &&
			    !p_ptr->slow && !p_ptr->paralyzed &&
			    !p_ptr->image && !p_ptr->word_recall)
			{
				set_action(ACTION_NONE);
			}
		}
	}

	if (p_ptr->action == ACTION_FISH)
	{
		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, 10);
		if (one_in_(1000))
		{
			int r_idx;
			bool success = FALSE;
			get_mon_num_prep(monster_tsuri,NULL);
			r_idx = get_mon_num(dun_level ? dun_level : wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level);
			msg_print(NULL);
			if (r_idx && one_in_(2))
			{
				int y, x;
				y = py+ddy[tsuri_dir];
				x = px+ddx[tsuri_dir];
				if (place_monster_aux(0, y, x, r_idx, PM_NO_KAGE))
				{
					char m_name[80];
					monster_desc(m_name, &m_list[cave[y][x].m_idx], 0);
#ifdef JP
					msg_format("%s����줿��", m_name);
#else
					msg_format("You have a good catch!", m_name);
#endif
					success = TRUE;
				}
			}
			if (!success)
			{
#ifdef JP
				msg_print("�¤��������Ƥ��ޤä������ä�����");
#else
				msg_print("Damn!  The fish stole your bait!");
#endif
			}
			disturb(0, 0);
		}
	}

	/* Handle "abort" */
	if (check_abort)
	{
		/* Check for "player abort" (semi-efficiently for resting) */
		if (running || command_rep || (p_ptr->action == ACTION_REST) || (p_ptr->action == ACTION_FISH))
		{
			/* Do not wait */
			inkey_scan = TRUE;

			/* Check for a key */
			if (inkey())
			{
				/* Flush input */
				flush();

				/* Disturb */
				disturb(0, 0);

				/* Hack -- Show a Message */
#ifdef JP
msg_print("���Ǥ��ޤ�����");
#else
				msg_print("Canceled.");
#endif

			}
		}
	}

	if (p_ptr->riding && !p_ptr->confused && !p_ptr->blind)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];

		if (m_ptr->csleep)
		{
			char m_name[80];

			/* Recover fully */
			m_ptr->csleep = 0;

			/* Acquire the monster name */
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
msg_format("%^s�򵯤�������", m_name);
#else
			msg_format("You have waked %s up.", m_name);
#endif
			if (p_ptr->health_who == p_ptr->riding) p_ptr->redraw |= (PR_HEALTH);
			p_ptr->redraw |= (PR_UHEALTH);
		}

		if (m_ptr->stunned)
		{
			int d = 1;

			/* Make a "saving throw" against stun */
			if (randint0(r_info[m_ptr->r_idx].level) < p_ptr->skill_exp[GINOU_RIDING])
			{
				/* Recover fully */
				d = m_ptr->stunned;
			}

			/* Hack -- Recover from stun */
			if (m_ptr->stunned > d)
			{
				/* Recover somewhat */
				m_ptr->stunned -= d;
			}

			/* Fully recover */
			else
			{
				char m_name[80];

				/* Recover fully */
				m_ptr->stunned = 0;

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
msg_format("%^s��ۯ۰���֤���Ω��ľ�餻����", m_name);
#else
				msg_format("%^s is no longer stunned.", m_name);
#endif
				if (p_ptr->health_who == p_ptr->riding) p_ptr->redraw |= (PR_HEALTH);
				p_ptr->redraw |= (PR_UHEALTH);
			}
		}

		if (m_ptr->confused)
		{
			int d = 1;

			/* Make a "saving throw" against stun */
			if (randint0(r_info[m_ptr->r_idx].level) < p_ptr->skill_exp[GINOU_RIDING])
			{
				/* Recover fully */
				d = m_ptr->confused;
			}

			/* Hack -- Recover from stun */
			if (m_ptr->confused > d)
			{
				/* Recover somewhat */
				m_ptr->confused -= d;
			}

			/* Fully recover */
			else
			{
				char m_name[80];

				/* Recover fully */
				m_ptr->confused = 0;

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
msg_format("%^s������֤���Ω��ľ�餻����", m_name);
#else
				msg_format("%^s is no longer confused.", m_name);
#endif
				if (p_ptr->health_who == p_ptr->riding) p_ptr->redraw |= (PR_HEALTH);
				p_ptr->redraw |= (PR_UHEALTH);
			}
		}

		if (m_ptr->monfear)
		{
			int d = 1;

			/* Make a "saving throw" against stun */
			if (randint0(r_info[m_ptr->r_idx].level) < p_ptr->skill_exp[GINOU_RIDING])
			{
				/* Recover fully */
				d = m_ptr->monfear;
			}

			/* Hack -- Recover from stun */
			if (m_ptr->monfear > d)
			{
				/* Recover somewhat */
				m_ptr->monfear -= d;
			}

			/* Fully recover */
			else
			{
				char m_name[80];

				/* Recover fully */
				m_ptr->monfear = 0;

				/* Acquire the monster name */
				monster_desc(m_name, m_ptr, 0);

				/* Dump a message */
#ifdef JP
msg_format("%^s���ݤ���Ω��ľ�餻����", m_name);
#else
				msg_format("%^s is no longer fear.", m_name);
#endif
				if (p_ptr->health_who == p_ptr->riding) p_ptr->redraw |= (PR_HEALTH);
				p_ptr->redraw |= (PR_UHEALTH);
			}
		}

		handle_stuff();
	}

	/* Handle the player song */
	if (!load) check_music();

	load = FALSE;

	/* Fast */
	if (p_ptr->lightspeed)
	{
		(void)set_lightspeed(p_ptr->lightspeed - 1, TRUE);
	}
	if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
	{
		if (p_ptr->magic_num1[0] < 40)
		{
			p_ptr->magic_num1[0] = 0;
		}
		else p_ptr->magic_num1[0] -= 40;
		p_ptr->update |= (PU_BONUS);
	}
	if (p_ptr->action == ACTION_LEARN)
	{
		int hoge = ((p_ptr->msp * 0x10000L) / 256L)+7680L;
		if ((p_ptr->csp * 0x10000L + p_ptr->csp_frac) < hoge)
		{
			p_ptr->csp = 0;
			p_ptr->csp_frac = 0;
			set_action(ACTION_NONE);
		}
		else
		{
			p_ptr->csp -= (s16b)(hoge >> 16);
			hoge &= 0xFFFFL;
			if (p_ptr->csp_frac < hoge)
			{
				p_ptr->csp_frac += 0x10000L - hoge;
				p_ptr->csp--;
			}
			else
				p_ptr->csp_frac -= hoge;
		}
		p_ptr->redraw |= PR_MANA;
	}

	if (p_ptr->special_defense & KATA_MASK)
	{
		if (p_ptr->special_defense & KATA_MUSOU)
		{
			if (p_ptr->csp < 3)
			{
				set_action(ACTION_NONE);
			}
			else
			{
				p_ptr->csp -= 2;
				p_ptr->redraw |= (PR_MANA);
			}
		}
	}

	/*** Handle actual user input ***/

	/* Repeat until out of energy */
	while (p_ptr->energy_need <= 0)
	{
		p_ptr->window |= PW_PLAYER;
		p_ptr->sutemi = FALSE;
		p_ptr->counter = FALSE;
		now_damaged = FALSE;

		/* Notice stuff (if needed) */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff (if needed) */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff (if needed) */
		if (p_ptr->window) window_stuff();


		/* Place the cursor on the player */
		move_cursor_relative(py, px);

		/* Refresh (optional) */
		if (fresh_before) Term_fresh();


		/* Hack -- Pack Overflow */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			char o_name[MAX_NLEN];

			object_type *o_ptr;

			/* Access the slot to be dropped */
			o_ptr = &inventory[item];

			/* Disturbing */
			disturb(0, 0);

			/* Warning */
#ifdef JP
msg_print("���å����饢���ƥब���դ줿��");
#else
			msg_print("Your pack overflows!");
#endif


			/* Describe */
			object_desc(o_name, o_ptr, TRUE, 3);

			/* Message */
#ifdef JP
msg_format("%s(%c)����Ȥ�����", o_name, index_to_label(item));
#else
			msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif


			/* Drop it (carefully) near the player */
			(void)drop_near(o_ptr, 0, py, px);

			/* Modify, Describe, Optimize */
			inven_item_increase(item, -255);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Notice stuff (if needed) */
			if (p_ptr->notice) notice_stuff();

			/* Update stuff (if needed) */
			if (p_ptr->update) update_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->redraw) redraw_stuff();

			/* Redraw stuff (if needed) */
			if (p_ptr->window) window_stuff();
		}


		/* Hack -- cancel "lurking browse mode" */
		if (!command_new) command_see = FALSE;


		/* Assume free turn */
		energy_use = 0;


		if (p_ptr->inside_battle)
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			command_cmd = SPECIAL_KEY_BUILDING;

			/* Process the command */
			process_command();
		}

		/* Paralyzed or Knocked Out */
		else if (p_ptr->paralyzed || (p_ptr->stun >= 100))
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Resting */
		else if (p_ptr->action == ACTION_REST)
		{
			/* Timed rest */
			if (resting > 0)
			{
				/* Reduce rest count */
				resting--;

				if (!resting) set_action(ACTION_NONE);

				/* Redraw the state */
				p_ptr->redraw |= (PR_STATE);
			}

			/* Take a turn */
			energy_use = 100;
		}

		/* Fishing */
		else if (p_ptr->action == ACTION_FISH)
		{
			/* Take a turn */
			energy_use = 100;
		}

		/* Running */
		else if (running)
		{
			/* Take a step */
			run_step(0);
		}

		/* Repeated command */
		else if (command_rep)
		{
			/* Count this execution */
			command_rep--;

			/* Redraw the state */
			p_ptr->redraw |= (PR_STATE);

			/* Redraw stuff */
			redraw_stuff();

			/* Hack -- Assume messages were seen */
			msg_flag = FALSE;

			/* Clear the top line */
			prt("", 0, 0);

			/* Process the command */
			process_command();
		}

		/* Normal command */
		else
		{
			/* Place the cursor on the player */
			move_cursor_relative(py, px);

			can_save = TRUE;
			/* Get a command (normal) */
			request_command(FALSE);
			can_save = FALSE;

			/* Process the command */
			process_command();
		}


		/*** Clean up ***/

		/* Significant */
		if (energy_use)
		{
			/* Use some energy */
			if (world_player || energy_use > 400)
			{
				/* The Randomness is irrelevant */
				p_ptr->energy_need += energy_use * TURNS_PER_TICK / 10;
			}
			else
			{
				/* There is some randomness of needed energy */
				p_ptr->energy_need += (s16b)((s32b)energy_use * ENERGY_NEED() / 100L);
			}

			/* Hack -- constant hallucination */
			if (p_ptr->image) p_ptr->redraw |= (PR_MAP);


			/* Shimmer monsters if needed */
			if (shimmer_monsters)
			{
				/* Clear the flag */
				shimmer_monsters = FALSE;

				/* Shimmer multi-hued monsters */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;
					monster_race *r_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Access the monster race */
					r_ptr = &r_info[m_ptr->r_idx];

					/* Skip non-multi-hued monsters */
					if (!(r_ptr->flags1 & RF1_ATTR_MULTI)) continue;

					/* Reset the flag */
					shimmer_monsters = TRUE;

					/* Redraw regardless */
					lite_spot(m_ptr->fy, m_ptr->fx);
				}
			}


			/* Handle monster detection */
			if (repair_monsters)
			{
				/* Reset the flag */
				repair_monsters = FALSE;

				/* Rotate detection flags */
				for (i = 1; i < m_max; i++)
				{
					monster_type *m_ptr;

					/* Access monster */
					m_ptr = &m_list[i];

					/* Skip dead monsters */
					if (!m_ptr->r_idx) continue;

					/* Nice monsters get mean */
					if (m_ptr->mflag & MFLAG_NICE)
					{
						/* Nice monsters get mean */
						m_ptr->mflag &= ~(MFLAG_NICE);
					}

					/* Handle memorized monsters */
					if (m_ptr->mflag & MFLAG_MARK)
					{
						/* Maintain detection */
						if (m_ptr->mflag & MFLAG_SHOW)
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_SHOW);

							/* Still need repairs */
							repair_monsters = TRUE;
						}

						/* Remove detection */
						else
						{
							/* Forget flag */
							m_ptr->mflag &= ~(MFLAG_MARK);

							/* Assume invisible */
							m_ptr->ml = FALSE;

							/* Update the monster */
							update_mon(i, FALSE);

							if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == i) p_ptr->redraw |= (PR_UHEALTH);

							/* Redraw regardless */
							lite_spot(m_ptr->fy, m_ptr->fx);
						}
					}
				}
			}
			if (p_ptr->pclass == CLASS_IMITATOR)
			{
				if (p_ptr->mane_num > (p_ptr->lev > 44 ? 3 : p_ptr->lev > 29 ? 2 : 1))
				{
					p_ptr->mane_num--;
					for (i = 0; i < p_ptr->mane_num; i++)
					{
						p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
						p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
					}
				}
				new_mane = FALSE;
				p_ptr->redraw |= (PR_MANE);
			}
			if (p_ptr->action == ACTION_LEARN)
			{
				new_mane = FALSE;
				p_ptr->redraw |= (PR_STATE);
			}

			if (world_player && (p_ptr->energy_need > - 1000))
			{
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

#ifdef JP
				msg_print("�ֻ���ư�������ġ�");
#else
				msg_print("You feel time flowing around you once more.");
#endif
				msg_print(NULL);
				world_player = FALSE;
				p_ptr->energy_need = ENERGY_NEED();

				handle_stuff();
			}
		}

		/* Hack -- notice death */
		if (!p_ptr->playing || p_ptr->is_dead)
		{
			world_player = FALSE;
			break;
		}

		/* Handle "leaving" */
		if (p_ptr->leaving) break;
	}

	/* Update scent trail */
	update_smell();
}


/*
 * Interact with the current dungeon level.
 *
 * This function will not exit until the level is completed,
 * the user dies, or the game is terminated.
 */
static void dungeon(bool load_game)
{
	int quest_num = 0, i, num;

	/* Set the base level */
	base_level = dun_level;

	/* Reset various flags */
	hack_mind = FALSE;

	/* Not leaving */
	p_ptr->leaving = FALSE;

	/* Reset the "command" vars */
	command_cmd = 0;
	command_new = 0;
	command_rep = 0;
	command_arg = 0;
	command_dir = 0;


	/* Cancel the target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;
	ambush_flag = FALSE;

	/* Cancel the health bar */
	health_track(0);

	/* Check visual effects */
	shimmer_monsters = TRUE;
	shimmer_objects = TRUE;
	repair_monsters = TRUE;
	repair_objects = TRUE;


	/* Disturb */
	disturb(1, 0);

	/* Get index of current quest (if any) */
	quest_num = quest_number(dun_level);

	/* Inside a quest? */
	if (quest_num)
	{
		/* Mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 |= RF1_QUESTOR;
	}

	/* Track maximum player level */
	if (p_ptr->max_plv < p_ptr->lev)
	{
		p_ptr->max_plv = p_ptr->lev;
	}


	/* Track maximum dungeon level (if not in quest -KMW-) */
	if ((max_dlv[dungeon_type] < dun_level) && !p_ptr->inside_quest)
	{
		max_dlv[dungeon_type] = dun_level;
		if (record_maxdeapth) do_cmd_write_nikki(NIKKI_MAXDEAPTH, dun_level, NULL);
	}

	/* No stairs down from Quest */
	if (quest_number(dun_level))
	{
		create_down_stair = 0;
	}

	/* Paranoia -- no stairs from town or wilderness */
	if (!dun_level) create_down_stair = create_up_stair = 0;

	/* Option -- no connected stairs */
	if (!dungeon_stair) create_down_stair = create_up_stair = 0;

	/* Option -- no up stairs */
	if (ironman_downward) create_down_stair = create_up_stair = 0;

	/* Make a stairway. */
	if (create_up_stair || create_down_stair)
	{
		/* Place a stairway */
		if (cave_valid_bold(py, px))
		{
			/* XXX XXX XXX */
			delete_object(py, px);

			/* Make stairs */
			if (create_down_stair)
			{
				if (create_down_stair == 2) cave_set_feat(py, px, FEAT_MORE_MORE);
				else cave_set_feat(py, px, FEAT_MORE);
			}
			else
			{
				if (create_up_stair == 2) cave_set_feat(py, px, FEAT_LESS_LESS);
				else cave_set_feat(py, px, FEAT_LESS);
			}
		}

		/* Cancel the stair request */
		create_down_stair = create_up_stair = 0;
	}

	/* Validate the panel */
	panel_bounds_center();

	/* Verify the panel */
	verify_panel();

	/* Flush messages */
	msg_print(NULL);


	/* Enter "xtra" mode */
	character_xtra = TRUE;

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER | PW_MONSTER | PW_OVERHEAD | PW_DUNGEON);

	/* Redraw dungeon */
	p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Update lite/view */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE | PU_TORCH);

	/* Update monsters */
	p_ptr->update |= (PU_MONSTERS | PU_DISTANCE | PU_FLOW);

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Redraw stuff */
	window_stuff();

	/* Leave "xtra" mode */
	character_xtra = FALSE;

	/* Update stuff */
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

	/* Combine / Reorder the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Notice stuff */
	notice_stuff();

	/* Update stuff */
	update_stuff();

	/* Redraw stuff */
	redraw_stuff();

	/* Window stuff */
	window_stuff();

	/* Refresh */
	Term_fresh();

	if (quest_number(dun_level) && ((quest_number(dun_level) < MIN_RANDOM_QUEST) && !(quest_number(dun_level) == QUEST_OBERON || quest_number(dun_level) == QUEST_SERPENT || !(quest[quest_number(dun_level)].flags & QUEST_FLAG_PRESET)))) do_cmd_feeling();

	if (p_ptr->inside_battle)
	{
		if (load_game)
		{
			p_ptr->energy_need = 0;
			battle_monsters();
		}
		else
		{
			
#ifdef JP
msg_print("��糫�ϡ�");
#else
			msg_format("Ready..Fight!");
#endif
			msg_print(NULL);
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT))
		p_ptr->magic_num1[0] = MUSIC_DETECT;

	/* Hack -- notice death or departure */
	if (!p_ptr->playing || p_ptr->is_dead) return;

	/* Print quest message if appropriate */
	if (!p_ptr->inside_quest && (dungeon_type == DUNGEON_ANGBAND))
	{
		quest_discovery(random_quest_number(dun_level));
		p_ptr->inside_quest = random_quest_number(dun_level);
	}
	if ((dun_level == d_info[dungeon_type].maxdepth) && d_info[dungeon_type].final_guardian)
	{
		if (r_info[d_info[dungeon_type].final_guardian].max_num)
#ifdef JP
			msg_format("���γ��ˤ�%s�μ�Ǥ���%s������Ǥ��롣",
				   d_name+d_info[dungeon_type].name, 
				   r_name+r_info[d_info[dungeon_type].final_guardian].name);
#else
		msg_format("%^s lives in this level as the keeper of %s.",
				   r_name+r_info[d_info[dungeon_type].final_guardian].name, 
				   d_name+d_info[dungeon_type].name);
#endif
	}

	/*** Process this dungeon level ***/

	/* Reset the monster generation level */
	monster_level = base_level;

	/* Reset the object generation level */
	object_level = base_level;

	hack_mind = TRUE;

	if (p_ptr->energy_need > 0 && !p_ptr->inside_battle &&
	    (dun_level || p_ptr->leaving_dungeon || p_ptr->inside_arena))
		p_ptr->energy_need = 0;

	/* Not leaving dungeon */
	p_ptr->leaving_dungeon = FALSE;

	/* Main loop */
	while (TRUE)
	{
		int i;

		/* Hack -- Compact the monster list occasionally */
		if ((m_cnt + 32 > max_m_idx) && !p_ptr->inside_battle) compact_monsters(64);

		/* Hack -- Compress the monster list occasionally */
		if ((m_cnt + 32 < m_max) && !p_ptr->inside_battle) compact_monsters(0);


		/* Hack -- Compact the object list occasionally */
		if (o_cnt + 32 > max_o_idx) compact_objects(64);

		/* Hack -- Compress the object list occasionally */
		if (o_cnt + 32 < o_max) compact_objects(0);


		/* Process the player */
		process_player();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Process all of the monsters */
		process_monsters();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Redraw stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;


		/* Process the world */
		process_world();

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();

		/* Hack -- Hilite the player */
		move_cursor_relative(py, px);

		/* Optional fresh */
		if (fresh_after) Term_fresh();

		/* Hack -- Notice death or departure */
		if (!p_ptr->playing || p_ptr->is_dead) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		/* Count game turns */
		turn++;
		if (!p_ptr->wild_mode || wild_regen) dungeon_turn++;
		else if (p_ptr->wild_mode && !(turn % ((MAX_HGT + MAX_WID) / 2))) dungeon_turn++;
		if (wild_regen) wild_regen--;
	}

	/* Inside a quest and non-unique questor? */
	if (quest_num && !(r_info[quest[quest_num].r_idx].flags1 & RF1_UNIQUE))
	{
		/* Un-mark the quest monster */
		r_info[quest[quest_num].r_idx].flags1 &= ~RF1_QUESTOR;
	}

	/* Not save-and-quit and not dead? */
	if (p_ptr->playing && !p_ptr->is_dead)
	{
		for(num = 0; num < 21; num++)
		{
			party_mon[num].r_idx = 0;
		}

		if (p_ptr->riding)
		{
			COPY(&party_mon[0], &m_list[p_ptr->riding], monster_type);
		}

		for(i = m_max - 1, num = 1; (i >= 1 && num < 21); i--)
		{
			monster_type *m_ptr = &m_list[i];
			
			if (!m_ptr->r_idx) continue;
			if (!is_pet(m_ptr)) continue;
			if (i == p_ptr->riding) continue;

			if (reinit_wilderness)
			{
				/* Don't lose sight of pets when getting a Quest */
			}
			else
			{
				int dis = distance(py, px, m_ptr->fy, m_ptr->fx);

				/*
				 * Pets with nickname will follow even from 3 blocks away
				 * when you or the pet can see the other.
				 */
				if (m_ptr->nickname && 
				    (player_has_los_bold(m_ptr->fy, m_ptr->fx) ||
				     los(m_ptr->fy, m_ptr->fx, py, px)))
				{
					if (dis > 3) continue;
				}
				else
				{
					if (dis > 1) continue;
				}
				if (m_ptr->confused || m_ptr->stunned || m_ptr->csleep) continue;
			}

			COPY(&party_mon[num], &m_list[i], monster_type);
			num++;

			/* Mark as followed */
			delete_monster_idx(i);
		}

		/* Forget the flag */
		reinit_wilderness = FALSE;

		if (record_named_pet)
		{
			for (i = m_max - 1; i >=1; i--)
			{
				monster_type *m_ptr = &m_list[i];
				char m_name[80];
				
				if (!m_ptr->r_idx) continue;
				if (!is_pet(m_ptr)) continue;
				if (!m_ptr->nickname) continue;
				if (p_ptr->riding == i) continue;
				
				monster_desc(m_name, m_ptr, 0x88);
				do_cmd_write_nikki(NIKKI_NAMED_PET, 4, m_name);
			}
		}
	}

	write_level = TRUE;
}


/*
 * Load some "user pref files"
 *
 * Modified by Arcum Dagsson to support
 * separate macro files for different realms.
 */
static void load_all_pref_files(void)
{
	char buf[1024];
	errr err;

	/* Access the "user" pref file */
	sprintf(buf, "user.prf");

	/* Process that file */
	process_pref_file(buf);

	/* Access the "user" system pref file */
	sprintf(buf, "user-%s.prf", ANGBAND_SYS);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "race" pref file */
	sprintf(buf, "%s.prf", rp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "class" pref file */
	sprintf(buf, "%s.prf", cp_ptr->title);

	/* Process that file */
	process_pref_file(buf);

	/* Access the "character" pref file */
	sprintf(buf, "%s.prf", player_base);

	/* Process that file */
	process_pref_file(buf);

	/* Free old entries */
	init_autopicker();

#ifdef JP
	sprintf(buf, "picktype-%s.prf", player_base);
#else
	sprintf(buf, "pickpref-%s.prf", player_base);
#endif

	err = process_pickpref_file(buf);

	/* Process 'pick????.prf' if 'pick????-<name>.prf' doesn't exist */
	if (0 > err)
	{
#ifdef JP
		process_pickpref_file("picktype.prf");
#else
		process_pickpref_file("pickpref.prf");
#endif
	}

	/* Access the "realm 1" pref file */
	if (p_ptr->realm1 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm1]);

		/* Process that file */
		process_pref_file(buf);
	}

	/* Access the "realm 2" pref file */
	if (p_ptr->realm2 != REALM_NONE)
	{
		sprintf(buf, "%s.prf", realm_names[p_ptr->realm2]);

		/* Process that file */
		process_pref_file(buf);
	}
}


/*
 * Actually play a game
 *
 * If the "new_game" parameter is true, then, after loading the
 * savefile, we will commit suicide, if necessary, to allow the
 * player to start a new game.
 */
void play_game(bool new_game)
{
	int i;
	bool load_game = TRUE;

#ifdef CHUUKEI
	if(chuukei_client){
	  reset_visuals();
	  browse_chuukei();
	  return;
	}
#endif

	hack_mutation = FALSE;

	/* Hack -- Character is "icky" */
	character_icky = TRUE;

	/* Make sure main term is active */
	Term_activate(angband_term[0]);

	/* Initialise the resize hooks */
	angband_term[0]->resize_hook = resize_map;
	
	for (i = 1; i < 8; i++)
	{
		/* Does the term exist? */
		if (angband_term[i])
		{
			/* Add the redraw on resize hook */
			angband_term[i]->resize_hook = redraw_window;
		}
	}

	/* Hack -- turn off the cursor */
	(void)Term_set_cursor(0);


	/* Attempt to load */
	if (!load_player())
	{
		/* Oops */
#ifdef JP
quit("�����֥ե����뤬����Ƥ��ޤ�");
#else
		quit("broken savefile");
#endif

	}

	/* Extract the options */
	for (i = 0; option_info[i].o_desc; i++)
	{
		int os = option_info[i].o_set;
		int ob = option_info[i].o_bit;

		/* Set the "default" options */
		if (option_info[i].o_var)
		{
			/* Set */
			if (option_flag[os] & (1L << ob))
			{
				/* Set */
				(*option_info[i].o_var) = TRUE;
			}

			/* Clear */
			else
			{
				/* Clear */
				(*option_info[i].o_var) = FALSE;
			}
		}
	}

	/* Report waited score */
	if (p_ptr->wait_report_score)
	{
		char buf[1024];
		bool success;

#ifdef JP
		if (!get_check_strict("�Ե����Ƥ�����������Ͽ�򺣹Ԥʤ��ޤ�����", CHECK_NO_HISTORY))
#else
		if (!get_check_strict("Do you register score now? ", CHECK_NO_HISTORY))
#endif
			quit(0);

		/* Update stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Update stuff */
		update_stuff();

		p_ptr->is_dead = TRUE;

		start_time = time(NULL);

		/* No suspending now */
		signals_ignore_tstp();
		
		/* Hack -- Character is now "icky" */
		character_icky = TRUE;

		/* Build the filename */
		path_build(buf, sizeof(buf), ANGBAND_DIR_APEX, "scores.raw");

		/* Open the high score file, for reading/writing */
		highscore_fd = fd_open(buf, O_RDWR);

		/* Handle score, show Top scores */
		success = send_world_score(TRUE);

#ifdef JP
		if (!success && !get_check_strict("��������Ͽ������ޤ�����", CHECK_NO_HISTORY))
#else
		if (!success && !get_check_strict("Do you give up score registration? ", CHECK_NO_HISTORY))
#endif
		{
#ifdef JP
			prt("����³���Ե����ޤ���", 0, 0);
#else
			prt("standing by for future registration...", 0, 0);
#endif
			(void)inkey();
		}
		else
		{
			p_ptr->wait_report_score = FALSE;
			top_twenty();
#ifdef JP
			if (!save_player()) msg_print("�����ּ��ԡ�");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		/* Shut the high score file */
		(void)fd_close(highscore_fd);

		/* Forget the high score fd */
		highscore_fd = -1;
		
		/* Allow suspending now */
		signals_handle_tstp();

		quit(0);
	}

	/* Nothing loaded */
	if (!character_loaded)
	{
		/* Make new player */
		new_game = TRUE;

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Prepare to init the RNG */
		Rand_quick = TRUE;
	}

	/* Process old character */
	if (!new_game)
	{
		/* Process the player name */
		process_player_name(FALSE);
	}

	/* Init the RNG */
	if (Rand_quick)
	{
		u32b seed;

		/* Basic seed */
		seed = (time(NULL));

#ifdef SET_UID

		/* Mutate the seed on Unix machines */
		seed = ((seed >> 3) * (getpid() << 1));

#endif

		/* Use the complex RNG */
		Rand_quick = FALSE;

		/* Seed the "complex" RNG */
		Rand_state_init(seed);
	}

	/* Roll new character */
	if (new_game)
	{
		monster_race *r_ptr;

		/* The dungeon is not ready */
		character_dungeon = FALSE;

		/* Start in town */
		dun_level = 0;
		p_ptr->inside_quest = 0;
		p_ptr->inside_arena = FALSE;
		p_ptr->inside_battle = FALSE;

		write_level = TRUE;

		/* Hack -- seed for flavors */
		seed_flavor = randint0(0x10000000);

		/* Hack -- seed for town layout */
		seed_town = randint0(0x10000000);

		/* Roll up a new character */
		player_birth();

		counts_write(2,0);
		p_ptr->count = 0;

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�ն����Ϥ˹ߤ�Ω�ä���");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You are standing in the Outpost.");
#endif

		load = FALSE;
		get_mon_num_prep(NULL, NULL);
		for (i = 0; i < MAX_KUBI; i++)
		{
			while (1)
			{
				int j;

				kubi_r_idx[i] = get_mon_num(MAX_DEPTH - 1);
				r_ptr = &r_info[kubi_r_idx[i]];

				if(!(r_ptr->flags1 & RF1_UNIQUE)) continue;

				if(!(r_ptr->flags9 & RF9_DROP_CORPSE)) continue;
				if (r_ptr->rarity > 100) continue;

				if(r_ptr->flags6 & RF6_SPECIAL) continue;

				for (j = 0; j < i; j++)
					if (kubi_r_idx[i] == kubi_r_idx[j])break;

				if (j == i) break;
			}
		}
		for (i = 0; i < MAX_KUBI -1; i++)
		{
			int j,tmp;
			for (j = i; j < MAX_KUBI; j++)
			{
				if (r_info[kubi_r_idx[i]].level > r_info[kubi_r_idx[j]].level)
				{
					tmp = kubi_r_idx[i];
					kubi_r_idx[i] = kubi_r_idx[j];
					kubi_r_idx[j] = tmp;
				}
			}
		}

		p_ptr->inside_battle = TRUE;
		while (1)
		{
			today_mon = get_mon_num(3);
			r_ptr = &r_info[today_mon];

			if (r_ptr->flags1 & RF1_UNIQUE) continue;
			if (r_ptr->flags2 & (RF2_MULTIPLY)) continue;
			if (!(r_ptr->flags9 & RF9_DROP_CORPSE) || !(r_ptr->flags9 & RF9_DROP_SKELETON)) continue;
			if (r_ptr->rarity > 10) continue;
			if (r_ptr->level == 0) continue;
			break;
		}
		p_ptr->inside_battle = FALSE;
	}
	else
	{
		write_level = FALSE;

#ifdef JP
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ----������Ƴ�----");
#else
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "                            ---- Restart Game ----");
#endif

/*
 * 1.0.9 �����ϥ��������� p_ptr->riding = -1 �Ȥ��Ƥ����Τǡ������꤬ɬ�פ��ä���
 * �⤦���פ����������Υ����֥ե�����Ȥθߴ��Τ���˻Ĥ��Ƥ�����
 */
		if (p_ptr->riding == -1)
		{
			p_ptr->riding = 0;
			for(i = m_max; i > 0; i--)
			{
				if ((m_list[i].fy == py) && (m_list[i].fx == px))
				{
					p_ptr->riding = i;
					break;
				}
			}
		}
	}

	p_ptr->teleport_town = FALSE;
	p_ptr->sutemi = FALSE;
	world_monster = FALSE;
	now_damaged = FALSE;
	now_message = 0;
	start_time = time(NULL) - 1;
	record_o_name[0] = '\0';

	/* Reset map panel */
	panel_row_min = cur_hgt;
	panel_col_min = cur_wid;

	/* Sexy gal gets bonus to maximum weapon skill of whip */
	if(p_ptr->pseikaku == SEIKAKU_SEXY)
		s_info[p_ptr->pclass].w_max[TV_HAFTED-TV_BOW][SV_WHIP] = 8000;

	/* Fill the arrays of floors and walls in the good proportions */
	for (i = 0; i < 100; i++)
	{
		int lim1, lim2, lim3;

		lim1 = d_info[dungeon_type].floor_percent1;
		lim2 = lim1 + d_info[dungeon_type].floor_percent2;
		lim3 = lim2 + d_info[dungeon_type].floor_percent3;

		if (i < lim1)
			floor_type[i] = d_info[dungeon_type].floor1;
		else if (i < lim2)
			floor_type[i] = d_info[dungeon_type].floor2;
		else if (i < lim3)
			floor_type[i] = d_info[dungeon_type].floor3;

		lim1 = d_info[dungeon_type].fill_percent1;
		lim2 = lim1 + d_info[dungeon_type].fill_percent2;
		lim3 = lim2 + d_info[dungeon_type].fill_percent3;
		if (i < lim1)
			fill_type[i] = d_info[dungeon_type].fill_type1;
		else if (i < lim2)
			fill_type[i] = d_info[dungeon_type].fill_type2;
		else if (i < lim3)
			fill_type[i] = d_info[dungeon_type].fill_type3;
	}

	/* Flavor the objects */
	flavor_init();
	if (new_game)
	{
		wipe_o_list();
		player_outfit();
	}

	/* Flash a message */
#ifdef JP
prt("���Ԥ�������...", 0, 0);
#else
	prt("Please wait...", 0, 0);
#endif


	/* Flush the message */
	Term_fresh();


	/* Hack -- Enter wizard mode */
	if (arg_wizard)
	{
		if (enter_wizard_mode()) p_ptr->wizard = TRUE;
		else if (p_ptr->is_dead) quit("Already dead.");
	}

	/* Initialize the town-buildings if necessary */
	if (!dun_level && !p_ptr->inside_quest)
	{
		/* Init the wilderness */

		process_dungeon_file("w_info.txt", 0, 0, max_wild_y, max_wild_x);

		/* Init the town */
		init_flags = INIT_ONLY_BUILDINGS;

		process_dungeon_file("t_info.txt", 0, 0, MAX_HGT, MAX_WID);

	}


	/* Initialize vault info */
#ifdef JP
if (init_v_info()) quit("����ʪ�������ǽ");
#else
	if (init_v_info()) quit("Cannot initialize vaults");
#endif

	/* Generate a dungeon level if needed */
	if (!character_dungeon) generate_cave();


	/* Character is now "complete" */
	character_generated = TRUE;


	/* Hack -- Character is no longer "icky" */
	character_icky = FALSE;


	/* Start game */
	p_ptr->playing = TRUE;

	/* Reset the visual mappings */
	reset_visuals();

	/* Load the "pref" files */
	load_all_pref_files();

	/* React to changes */
	Term_xtra(TERM_XTRA_REACT, 0);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_SPELL | PW_PLAYER);

	/* Window stuff */
	p_ptr->window |= (PW_MESSAGE | PW_OVERHEAD | PW_DUNGEON | PW_MONSTER | PW_OBJECT);

	/* Window stuff */
	window_stuff();


	/* Set or clear "rogue_like_commands" if requested */
	if (arg_force_original) rogue_like_commands = FALSE;
	if (arg_force_roguelike) rogue_like_commands = TRUE;

	/* Hack -- Enforce "delayed death" */
	if (p_ptr->chp < 0) p_ptr->is_dead = TRUE;

	if (p_ptr->prace == RACE_ANDROID) calc_android_exp();

	if (new_game && ((p_ptr->pclass == CLASS_CAVALRY) || (p_ptr->pclass == CLASS_BEASTMASTER)))
	{
		monster_type *m_ptr;
		int pet_r_idx = ((p_ptr->pclass == CLASS_CAVALRY) ? MON_HORSE : MON_YASE_HORSE);
		monster_race *r_ptr = &r_info[pet_r_idx];
		place_monster_aux(0, py, px - 1, pet_r_idx,
				  (PM_FORCE_PET | PM_NO_KAGE));
		m_ptr = &m_list[hack_m_idx_ii];
		m_ptr->mspeed = r_ptr->speed;
		m_ptr->maxhp = r_ptr->hdice*(r_ptr->hside+1)/2;
		m_ptr->max_maxhp = m_ptr->maxhp;
		m_ptr->hp = r_ptr->hdice*(r_ptr->hside+1)/2;
		m_ptr->energy_need = ENERGY_NEED() + ENERGY_NEED();
	}

	/* Process */
	while (TRUE)
	{
		/* Process the level */
		dungeon(load_game);

		/* Notice stuff */
		if (p_ptr->notice) notice_stuff();

		/* Update stuff */
		if (p_ptr->update) update_stuff();

		/* Redraw stuff */
		if (p_ptr->redraw) redraw_stuff();

		/* Window stuff */
		if (p_ptr->window) window_stuff();


		/* Cancel the target */
		target_who = 0;

		/* Cancel the health bar */
		health_track(0);


		/* Forget the lite */
		forget_lite();

		/* Forget the view */
		forget_view();

		/* Forget the view */
		clear_mon_lite();

		/* Handle "quit and save" */
		if (!p_ptr->playing && !p_ptr->is_dead) break;

		/* Erase the old cave */
		wipe_o_list();
		if (!p_ptr->is_dead) wipe_m_list();


		/* XXX XXX XXX */
		msg_print(NULL);

		load_game = FALSE;

		/* Accidental Death */
		if (p_ptr->playing && p_ptr->is_dead)
		{
			if (p_ptr->inside_arena)
			{
				p_ptr->inside_arena = FALSE;
				if(p_ptr->arena_number > MAX_ARENA_MONS)
					p_ptr->arena_number++;
				else
					p_ptr->arena_number = 99;
				p_ptr->is_dead = FALSE;
				p_ptr->chp = 0;
				p_ptr->chp_frac = 0;
				p_ptr->exit_bldg = TRUE;
				reset_tim_flags();
			}
			else
			{
				/* Mega-Hack -- Allow player to cheat death */
#ifdef JP
if ((p_ptr->wizard || cheat_live) && !get_check("��ˤޤ���? "))
#else
				if ((p_ptr->wizard || cheat_live) && !get_check("Die? "))
#endif

				{
					/* Mark social class, reset age, if needed */
					if (p_ptr->sc) p_ptr->sc = p_ptr->age = 0;

					/* Increase age */
					p_ptr->age++;

					/* Mark savefile */
					p_ptr->noscore |= 0x0001;

					/* Message */
#ifdef JP
msg_print("���������ɥ⡼�ɤ�ǰ�����ꡢ��򵽤�����");
#else
					msg_print("You invoke wizard mode and cheat death.");
#endif
					wipe_m_list();

					msg_print(NULL);

					/* Restore hit points */
					p_ptr->chp = p_ptr->mhp;
					p_ptr->chp_frac = 0;

					if (p_ptr->pclass == CLASS_MAGIC_EATER)
					{
						for (i = 0; i < EATER_EXT*2; i++)
						{
							p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
						}
						for (; i < EATER_EXT*3; i++)
						{
							p_ptr->magic_num1[i] = 0;
						}
					}
					/* Restore spell points */
					p_ptr->csp = p_ptr->msp;
					p_ptr->csp_frac = 0;

					/* Hack -- Healing */
					(void)set_blind(0);
					(void)set_confused(0);
					(void)set_poisoned(0);
					(void)set_afraid(0);
					(void)set_paralyzed(0);
					(void)set_image(0);
					(void)set_stun(0);
					(void)set_cut(0);

					/* Hack -- Prevent starvation */
					(void)set_food(PY_FOOD_MAX - 1);

					/* Hack -- cancel recall */
					if (p_ptr->word_recall)
					{
						/* Message */
#ifdef JP
msg_print("ĥ��Ĥ᤿�絤��ή���ä�...");
#else
						msg_print("A tension leaves the air around you...");
#endif

						msg_print(NULL);

						/* Hack -- Prevent recall */
						p_ptr->word_recall = 0;
						p_ptr->redraw |= (PR_STATUS);
					}

					/* Note cause of death XXX XXX XXX */
#ifdef JP
(void)strcpy(p_ptr->died_from, "��ε���");
#else
					(void)strcpy(p_ptr->died_from, "Cheating death");
#endif


					/* Do not die */
					p_ptr->is_dead = FALSE;

					dun_level = 0;
					p_ptr->inside_arena = FALSE;
					p_ptr->inside_battle = FALSE;
					leaving_quest = 0;
					p_ptr->inside_quest = 0;
					p_ptr->recall_dungeon = dungeon_type;
					dungeon_type = 0;
					if (lite_town || vanilla_town)
					{
						p_ptr->wilderness_y = 1;
						p_ptr->wilderness_x = 1;
						if (vanilla_town)
						{
							p_ptr->oldpy = 10;
							p_ptr->oldpx = 34;
						}
						else
						{
							p_ptr->oldpy = 33;
							p_ptr->oldpx = 131;
						}
					}
					else
					{
						p_ptr->wilderness_y = 48;
						p_ptr->wilderness_x = 5;
						p_ptr->oldpy = 33;
						p_ptr->oldpx = 131;
					}

					/* Leaving */
					p_ptr->wild_mode = FALSE;
					p_ptr->leaving = TRUE;

#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            �������������֤ä���");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "                            but revived.");
#endif
				}
			}
		}

		/* Handle "death" */
		if (p_ptr->is_dead) break;

		/* Make a new level */
		generate_cave();
	}

	/* Close stuff */
	close_game();

	/* Quit */
	quit(NULL);
}

s32b turn_real(s32b hoge)
{
	if ((p_ptr->prace == RACE_VAMPIRE) ||
	    (p_ptr->prace == RACE_SKELETON) ||
	    (p_ptr->prace == RACE_ZOMBIE) ||
	    (p_ptr->prace == RACE_SPECTRE))
		return hoge-(TURNS_PER_TICK * TOWN_DAWN *3/ 4);
	else
		return hoge;
}
