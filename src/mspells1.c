/* File: mspells1.c */

/* Purpose: Monster spells (attack player) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


#ifdef DRS_SMART_OPTIONS

/*
 * And now for Intelligent monster attacks (including spells).
 *
 * Original idea and code by "DRS" (David Reeves Sward).
 * Major modifications by "BEN" (Ben Harrison).
 *
 * Give monsters more intelligent attack/spell selection based on
 * observations of previous attacks on the player, and/or by allowing
 * the monster to "cheat" and know the player status.
 *
 * Maintain an idea of the player status, and use that information
 * to occasionally eliminate "ineffective" spell attacks.  We could
 * also eliminate ineffective normal attacks, but there is no reason
 * for the monster to do this, since he gains no benefit.
 * Note that MINDLESS monsters are not allowed to use this code.
 * And non-INTELLIGENT monsters only use it partially effectively.
 *
 * Actually learn what the player resists, and use that information
 * to remove attacks or spells before using them.  This will require
 * much less space, if I am not mistaken.  Thus, each monster gets a
 * set of 32 bit flags, "smart", build from the various "SM_*" flags.
 *
 * This has the added advantage that attacks and spells are related.
 * The "smart_learn" option means that the monster "learns" the flags
 * that should be set, and "smart_cheat" means that he "knows" them.
 * So "smart_cheat" means that the "smart" field is always up to date,
 * while "smart_learn" means that the "smart" field is slowly learned.
 * Both of them have the same effect on the "choose spell" routine.
 */



/*
 * Internal probability routine
 */
static bool int_outof(monster_race *r_ptr, int prob)
{
	/* Non-Smart monsters are half as "smart" */
	if (!(r_ptr->flags2 & RF2_SMART)) prob = prob / 2;

	/* Roll the dice */
	return (randint0(100) < prob);
}



/*
 * Remove the "bad" spells from a spell list
 */
static void remove_bad_spells(int m_idx, u32b *f4p, u32b *f5p, u32b *f6p)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u32b f4 = (*f4p);
	u32b f5 = (*f5p);
	u32b f6 = (*f6p);

	u32b smart = 0L;


	/* Too stupid to know anything */
	if (r_ptr->flags2 & RF2_STUPID) return;


	/* Must be cheating or learning */
	if (!smart_cheat && !smart_learn) return;


	/* Update acquired knowledge */
	if (smart_learn)
	{
		/* Hack -- Occasionally forget player status */
		/* Only save SM_FRIENDLY, SM_PET or SM_CLONED */
		if (m_ptr->smart && (randint0(100) < 1)) m_ptr->smart &= (SM_FRIENDLY | SM_PET | SM_CLONED);

		/* Use the memorized flags */
		smart = m_ptr->smart;
	}


	/* Cheat if requested */
	if (smart_cheat)
	{
		/* Know basic info */
		if (p_ptr->resist_acid) smart |= (SM_RES_ACID);
		if (p_ptr->oppose_acid) smart |= (SM_OPP_ACID);
		if (p_ptr->immune_acid) smart |= (SM_IMM_ACID);
		if (p_ptr->resist_elec) smart |= (SM_RES_ELEC);
		if (p_ptr->oppose_elec) smart |= (SM_OPP_ELEC);
		if (p_ptr->immune_elec) smart |= (SM_IMM_ELEC);
		if (p_ptr->resist_fire) smart |= (SM_RES_FIRE);
		if (p_ptr->oppose_fire) smart |= (SM_OPP_FIRE);
		if (p_ptr->immune_fire) smart |= (SM_IMM_FIRE);
		if (p_ptr->resist_cold) smart |= (SM_RES_COLD);
		if (p_ptr->oppose_cold) smart |= (SM_OPP_COLD);
		if (p_ptr->immune_cold) smart |= (SM_IMM_COLD);

		/* Know poison info */
		if (p_ptr->resist_pois) smart |= (SM_RES_POIS);
		if (p_ptr->oppose_pois) smart |= (SM_OPP_POIS);

		/* Know special resistances */
		if (p_ptr->resist_neth) smart |= (SM_RES_NETH);
		if (p_ptr->resist_lite) smart |= (SM_RES_LITE);
		if (p_ptr->resist_dark) smart |= (SM_RES_DARK);
		if (p_ptr->resist_fear) smart |= (SM_RES_FEAR);
		if (p_ptr->resist_conf) smart |= (SM_RES_CONF);
		if (p_ptr->resist_chaos) smart |= (SM_RES_CHAOS);
		if (p_ptr->resist_disen) smart |= (SM_RES_DISEN);
		if (p_ptr->resist_blind) smart |= (SM_RES_BLIND);
		if (p_ptr->resist_nexus) smart |= (SM_RES_NEXUS);
		if (p_ptr->resist_sound) smart |= (SM_RES_SOUND);
		if (p_ptr->resist_shard) smart |= (SM_RES_SHARD);
		if (p_ptr->reflect) smart |= (SM_IMM_REFLECT);

		/* Know bizarre "resistances" */
		if (p_ptr->free_act) smart |= (SM_IMM_FREE);
		if (!p_ptr->msp) smart |= (SM_IMM_MANA);
	}


	/* Nothing known */
	if (!smart) return;


	if (smart & SM_IMM_ACID)
	{
		f4 &= ~(RF4_BR_ACID);
		f5 &= ~(RF5_BA_ACID);
		f5 &= ~(RF5_BO_ACID);
	}
	else if ((smart & (SM_OPP_ACID)) && (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ACID);
	}
	else if ((smart & (SM_OPP_ACID)) || (smart & (SM_RES_ACID)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ACID);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ACID);
	}


	if (smart & (SM_IMM_ELEC))
	{
		f4 &= ~(RF4_BR_ELEC);
		f5 &= ~(RF5_BA_ELEC);
		f5 &= ~(RF5_BO_ELEC);
	}
	else if ((smart & (SM_OPP_ELEC)) && (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ELEC);
	}
	else if ((smart & (SM_OPP_ELEC)) || (smart & (SM_RES_ELEC)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_ELEC);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_ELEC);
	}


	if (smart & (SM_IMM_FIRE))
	{
		f4 &= ~(RF4_BR_FIRE);
		f5 &= ~(RF5_BA_FIRE);
		f5 &= ~(RF5_BO_FIRE);
	}
	else if ((smart & (SM_OPP_FIRE)) && (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_FIRE);
	}
	else if ((smart & (SM_OPP_FIRE)) || (smart & (SM_RES_FIRE)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_FIRE);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_FIRE);
	}


	if (smart & (SM_IMM_COLD))
	{
		f4 &= ~(RF4_BR_COLD);
		f5 &= ~(RF5_BA_COLD);
		f5 &= ~(RF5_BO_COLD);
		f5 &= ~(RF5_BO_ICEE);
	}
	else if ((smart & (SM_OPP_COLD)) && (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BO_ICEE);
	}
	else if ((smart & (SM_OPP_COLD)) || (smart & (SM_RES_COLD)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_COLD);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 20)) f5 &= ~(RF5_BO_ICEE);
	}


	if ((smart & (SM_OPP_POIS)) && (smart & (SM_RES_POIS)))
	{
		if (int_outof(r_ptr, 80)) f4 &= ~(RF4_BR_POIS);
		if (int_outof(r_ptr, 80)) f5 &= ~(RF5_BA_POIS);
		if (int_outof(r_ptr, 60)) f4 &= ~(RF4_BA_NUKE);
		if (int_outof(r_ptr, 60)) f4 &= ~(RF4_BR_NUKE);
	}
	else if ((smart & (SM_OPP_POIS)) || (smart & (SM_RES_POIS)))
	{
		if (int_outof(r_ptr, 30)) f4 &= ~(RF4_BR_POIS);
		if (int_outof(r_ptr, 30)) f5 &= ~(RF5_BA_POIS);
	}


	if (smart & (SM_RES_NETH))
	{
		if (prace_is_(RACE_SPECTRE))
		{
			f4 &= ~(RF4_BR_NETH);
			f5 &= ~(RF5_BA_NETH);
			f5 &= ~(RF5_BO_NETH);
		}
		else
		{
			if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_NETH);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BO_NETH);
		}
	}

	if (smart & (SM_RES_LITE))
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_LITE);
		if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_LITE);
	}

	if (smart & (SM_RES_DARK))
	{
		if (prace_is_(RACE_VAMPIRE))
		{
			f4 &= ~(RF4_BR_DARK);
			f5 &= ~(RF5_BA_DARK);
		}
		else
		{
			if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_DARK);
			if (int_outof(r_ptr, 50)) f5 &= ~(RF5_BA_DARK);
		}
	}

	if (smart & (SM_RES_FEAR))
	{
		f5 &= ~(RF5_SCARE);
	}

	if (smart & (SM_RES_CONF))
	{
		f5 &= ~(RF5_CONF);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_CONF);
	}

	if (smart & (SM_RES_CHAOS))
	{
		if (int_outof(r_ptr, 20)) f4 &= ~(RF4_BR_CHAO);
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BA_CHAO);
	}

	if (smart & (SM_RES_DISEN))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_DISE);
	}

	if (smart & (SM_RES_BLIND))
	{
		f5 &= ~(RF5_BLIND);
	}

	if (smart & (SM_RES_NEXUS))
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_NEXU);
		f6 &= ~(RF6_TELE_LEVEL);
	}

	if (smart & (SM_RES_SOUND))
	{
		if (int_outof(r_ptr, 50)) f4 &= ~(RF4_BR_SOUN);
	}

	if (smart & (SM_RES_SHARD))
	{
		if (int_outof(r_ptr, 40)) f4 &= ~(RF4_BR_SHAR);
	}

	if (smart & (SM_IMM_REFLECT))
	{
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_COLD);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_FIRE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ACID);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ELEC);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_NETH);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_WATE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_MANA);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_PLAS);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_BO_ICEE);
		if (int_outof(r_ptr, 150)) f5 &= ~(RF5_MISSILE);
		if (int_outof(r_ptr, 150)) f4 &= ~(RF4_SHOOT);
	}

	if (smart & (SM_IMM_FREE))
	{
		f5 &= ~(RF5_HOLD);
		f5 &= ~(RF5_SLOW);
	}

	if (smart & (SM_IMM_MANA))
	{
		f5 &= ~(RF5_DRAIN_MANA);
	}

	/* XXX XXX XXX No spells left? */
	/* if (!f4 && !f5 && !f6) ... */

	(*f4p) = f4;
	(*f5p) = f5;
	(*f6p) = f6;
}

#endif /* DRS_SMART_OPTIONS */


/*
 * Determine if there is a space near the player in which
 * a summoned creature can appear
 */
bool summon_possible(int y1, int x1)
{
	int y, x;

	/* Start at the player's location, and check 2 grids in each dir */
	for (y = y1 - 2; y <= y1 + 2; y++)
	{
		for (x = x1 - 2; x <= x1 + 2; x++)
		{
			/* Ignore illegal locations */
			if (!in_bounds(y, x)) continue;

			/* Only check a circular area */
			if (distance(y1, x1, y, x)>2) continue;

			/* ...nor on the Pattern */
			if ((cave[y][x].feat >= FEAT_PATTERN_START)
				&& (cave[y][x].feat <= FEAT_PATTERN_XTRA2)) continue;

			/* Require empty floor grid in line of sight */
			if ((cave_empty_bold(y, x) || (cave[y][x].feat == FEAT_TREES)) && los(y1, x1, y, x) && los(y, x, y1, x1)) return (TRUE);
		}
	}

	return FALSE;
}


static bool raise_possible(int y, int x)
{
	int xx, yy;
	s16b this_o_idx, next_o_idx = 0;
	cave_type *c_ptr;

	for (xx = x - 5; xx <= x + 5; xx++)
	{
		for (yy = y - 5; yy <= y + 5; yy++)
		{
			if (distance(y, x, yy, xx) > 5) continue;
			if (!los(y, x, yy, xx)) continue;

			c_ptr = &cave[yy][xx];
			/* Scan the pile of objects */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				/* Acquire object */
				object_type *o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Known to be worthless? */
				if (o_ptr->tval == TV_CORPSE)
					return TRUE;
			}
		}
	}
	return FALSE;
}


/*
 * Originally, it was possible for a friendly to shoot another friendly.
 * Change it so a "clean shot" means no equally friendly monster is
 * between the attacker and target.
 */
/*
 * Determine if a bolt spell will hit the player.
 *
 * This is exactly like "projectable", but it will
 * return FALSE if a monster is in the way.
 * no equally friendly monster is
 * between the attacker and target.
 */
bool clean_shot(int y1, int x1, int y2, int x2, bool friend)
{
	/* Must be the same as projectable() */

	int i, y, x;

	int grid_n = 0;
	u16b grid_g[512];

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, 0);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	/* Final grid */
	y = GRID_Y(grid_g[grid_n-1]);
	x = GRID_X(grid_g[grid_n-1]);

	/* May not end in an unrequested grid */
	if ((y != y2) || (x != x2)) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if ((cave[y][x].m_idx > 0) && !((y == y2) && (x == x2)))
		{
			monster_type *m_ptr = &m_list[cave[y][x].m_idx];
			if (friend == is_pet(m_ptr))
			{
				return (FALSE);
			}
		}
		/* Pets may not shoot through the character - TNB */
		if ((y == py) && (x == px))
		{
			if (friend) return (FALSE);
		}
	}

	return (TRUE);
}

/*
 * Cast a bolt at the player
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void bolt(int m_idx, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_PLAYER | PROJECT_REFLECTABLE;

	/* Target the player with a bolt attack */
	(void)project(m_idx, 0, py, px, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void beam(int m_idx, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_PLAYER;

	/* Target the player with a bolt attack */
	(void)project(m_idx, 0, py, px, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Cast a breath (or ball) attack at the player
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
static void breath(int y, int x, int m_idx, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_PLAYER;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Determine the radius of the blast */
	if ((rad < 1) && breath) rad = (r_ptr->flags2 & (RF2_POWERFUL)) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

	if (typ == GF_ROCKET) flg |= PROJECT_STOP;
	if (typ == GF_MIND_BLAST || typ == GF_BRAIN_SMASH ||
	    typ == GF_CAUSE_1 || typ == GF_CAUSE_2 || typ == GF_CAUSE_3 ||
	    typ == GF_CAUSE_4 || typ == GF_HAND_DOOM) flg |= PROJECT_HIDE;

	/* Target the player with a ball attack */
	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


u32b get_curse(int power, object_type *o_ptr)
{
	u32b new_curse;

	while(1)
	{
		new_curse = (1 << (randint0(MAX_CURSE)+4));
		if (power == 2)
		{
			if (!(new_curse & TRC_HEAVY_MASK)) continue;
		}
		else if (power == 1)
		{
			if (new_curse & TRC_SPECIAL_MASK) continue;
		}
		else if (power == 0)
		{
			if (new_curse & TRC_HEAVY_MASK) continue;
		}
		if (((o_ptr->tval < TV_BOW) || (o_ptr->tval > TV_SWORD)) && (new_curse == TRC_LOW_MELEE)) continue;
		if (((o_ptr->tval < TV_BOOTS) || (o_ptr->tval > TV_DRAG_ARMOR)) && (new_curse == TRC_LOW_AC)) continue;
		break;
	}
	return new_curse;
}

void curse_equipment(int chance, int heavy_chance)
{
	bool        changed = FALSE;
	int         curse_power = 0;
	u32b        new_curse;
	u32b oflgs[TR_FLAG_SIZE];
	object_type *o_ptr = &inventory[INVEN_RARM + randint0(12)];
	char o_name[MAX_NLEN];

	if (randint1(100) > chance) return;

	if (!o_ptr->k_idx) return;

	object_flags(o_ptr, oflgs);

	object_desc(o_name, o_ptr, FALSE, 0);

	/* Extra, biased saving throw for blessed items */
	if (have_flag(oflgs, TR_BLESSED) && (randint1(888) > chance))
	{
#ifdef JP
msg_format("%s�ϼ�����ķ���֤�����", o_name,
#else
		msg_format("Your %s resist%s cursing!", o_name,
#endif

			((o_ptr->number > 1) ? "" : "s"));
		/* Hmmm -- can we wear multiple items? If not, this is unnecessary */
		return;
	}

	if ((randint1(100) <= heavy_chance) &&
		(o_ptr->name1 || o_ptr->name2 || o_ptr->art_name))
	{
		if (!(o_ptr->curse_flags & TRC_HEAVY_CURSE))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_HEAVY_CURSE;
		o_ptr->curse_flags |= TRC_CURSED;
		curse_power++;
	}
	else
	{
		if (!cursed_p(o_ptr))
			changed = TRUE;
		o_ptr->curse_flags |= TRC_CURSED;
	}
	if (heavy_chance >= 50) curse_power++;

	new_curse = get_curse(curse_power, o_ptr);
	if (!(o_ptr->curse_flags & new_curse))
	{
		changed = TRUE;
		o_ptr->curse_flags |= new_curse;
	}

	if (changed)
	{
#ifdef JP
msg_format("���դ����������������餬%s��Ȥ�ޤ���...", o_name);
#else
		msg_format("There is a malignant black aura surrounding %s...", o_name);
#endif

		o_ptr->feeling = FEEL_NONE;
	}
	p_ptr->update |= (PU_BONUS);
}


/*
 * Return TRUE if a spell is good for hurting the player (directly).
 */
static bool spell_attack(byte spell)
{
	/* All RF4 spells hurt (except for shriek and dispel) */
	if (spell < 128 && spell > 98) return (TRUE);

	/* Various "ball" spells */
	if (spell >= 128 && spell <= 128 + 8) return (TRUE);

	/* "Cause wounds" and "bolt" spells */
	if (spell >= 128 + 12 && spell < 128 + 27) return (TRUE);

	/* Hand of Doom */
	if (spell == 160 + 1) return (TRUE);

	/* Psycho-Spear */
	if (spell == 160 + 11) return (TRUE);

	/* Doesn't hurt */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for escaping.
 */
static bool spell_escape(byte spell)
{
	/* Blink or Teleport */
	if (spell == 160 + 4 || spell == 160 + 5) return (TRUE);

	/* Teleport the player away */
	if (spell == 160 + 9 || spell == 160 + 10) return (TRUE);

	/* Isn't good for escaping */
	return (FALSE);
}

/*
 * Return TRUE if a spell is good for annoying the player.
 */
static bool spell_annoy(byte spell)
{
	/* Shriek */
	if (spell == 96 + 0) return (TRUE);

	/* Brain smash, et al (added curses) */
	if (spell >= 128 + 9 && spell <= 128 + 14) return (TRUE);

	/* Scare, confuse, blind, slow, paralyze */
	if (spell >= 128 + 27 && spell <= 128 + 31) return (TRUE);

	/* Teleport to */
	if (spell == 160 + 8) return (TRUE);

	/* Teleport level */
	if (spell == 160 + 10) return (TRUE);

	/* Darkness, make traps, cause amnesia */
	if (spell >= 160 + 12 && spell <= 160 + 14) return (TRUE);

	/* Doesn't annoy */
	return (FALSE);
}

/*
 * Return TRUE if a spell summons help.
 */
static bool spell_summon(byte spell)
{
	/* All summon spells */
	if (spell >= 160 + 16) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}


/*
 * Return TRUE if a spell raise-dead.
 */
static bool spell_raise(byte spell)
{
	/* All raise-dead spells */
	if (spell == 160 + 15) return (TRUE);

	/* Doesn't summon */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good in a tactical situation.
 */
static bool spell_tactic(byte spell)
{
	/* Blink */
	if (spell == 160 + 4) return (TRUE);

	/* Not good */
	return (FALSE);
}

/*
 * Return TRUE if a spell makes invulnerable.
 */
static bool spell_invulner(byte spell)
{
	/* Invulnerability */
	if (spell == 160 + 3) return (TRUE);

	/* No invulnerability */
	return (FALSE);
}

/*
 * Return TRUE if a spell hastes.
 */
static bool spell_haste(byte spell)
{
	/* Haste self */
	if (spell == 160 + 0) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell world.
 */
static bool spell_world(byte spell)
{
	/* world */
	if (spell == 160 + 6) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell special.
 */
static bool spell_special(byte spell)
{
	if (p_ptr->inside_battle) return FALSE;

	/* world */
	if (spell == 160 + 7) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell psycho-spear.
 */
static bool spell_psy_spe(byte spell)
{
	/* world */
	if (spell == 160 + 11) return (TRUE);

	/* Not a haste spell */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for healing.
 */
static bool spell_heal(byte spell)
{
	/* Heal */
	if (spell == 160 + 2) return (TRUE);

	/* No healing */
	return (FALSE);
}


/*
 * Return TRUE if a spell is good for dispel.
 */
static bool spell_dispel(byte spell)
{
	/* Dispel */
	if (spell == 96 + 2) return (TRUE);

	/* No dispel */
	return (FALSE);
}


/*
 * Check should monster cast dispel spell.
 */
static bool dispel_check(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Invulnabilty */
	if (p_ptr->invuln) return (TRUE);

	/* Wraith form */
	if (p_ptr->wraith_form) return (TRUE);

	/* Shield */
	if (p_ptr->shield) return (TRUE);

	/* Magic defence */
	if (p_ptr->magicdef) return (TRUE);

	/* Multi Shadow */
	if (p_ptr->multishadow) return (TRUE);

	/* Robe of dust */
	if (p_ptr->dustrobe) return (TRUE);

	/* Berserk Strength */
	if (p_ptr->shero && (p_ptr->pclass != CLASS_BERSERKER)) return (TRUE);

	/* Invulnability song */
	if (music_singing(MUSIC_INVULN)) return (TRUE);

	/* Demon Lord */
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD) return (TRUE);

	/* Elemental resistances */
	if (r_ptr->flags4 & RF4_BR_ACID)
	{
		if (!p_ptr->immune_acid && (p_ptr->oppose_acid || music_singing(MUSIC_RESIST))) return (TRUE);
		if (p_ptr->special_defense & DEFENSE_ACID) return (TRUE);
	}

	if (r_ptr->flags4 & RF4_BR_FIRE)
	{
		if (!((p_ptr->prace == RACE_DEMON) && p_ptr->lev > 44))
		{
			if (!p_ptr->immune_fire && (p_ptr->oppose_fire || music_singing(MUSIC_RESIST))) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_FIRE) return(TRUE);
		}
	}

	if (r_ptr->flags4 & RF4_BR_ELEC)
	{
		if (!p_ptr->immune_elec && (p_ptr->oppose_elec || music_singing(MUSIC_RESIST))) return (TRUE);
		if (p_ptr->special_defense & DEFENSE_ELEC) return (TRUE);
	}

	if (r_ptr->flags4 & RF4_BR_COLD)
	{
		if (!p_ptr->immune_cold && (p_ptr->oppose_cold || music_singing(MUSIC_RESIST))) return (TRUE);
		if (p_ptr->special_defense & DEFENSE_COLD) return (TRUE);
	}

	if (r_ptr->flags4 & (RF4_BR_POIS | RF4_BR_NUKE))
	{
		if (!((p_ptr->pclass == CLASS_NINJA) && p_ptr->lev > 44))
		{
			if (p_ptr->oppose_pois || music_singing(MUSIC_RESIST)) return (TRUE);
			if (p_ptr->special_defense & DEFENSE_POIS) return (TRUE);
		}
	}

	/* Ultimate resistance */
	if (p_ptr->ult_res) return (TRUE);

	/* Potion of Neo Tsuyosi special */
	if (p_ptr->tsuyoshi) return (TRUE);

	/* Elemental Brands */
	if ((p_ptr->special_attack & ATTACK_ACID) && !(r_ptr->flags3 & RF3_IM_ACID)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_FIRE) && !(r_ptr->flags3 & RF3_IM_FIRE)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_ELEC) && !(r_ptr->flags3 & RF3_IM_ELEC)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_COLD) && !(r_ptr->flags3 & RF3_IM_COLD)) return (TRUE);
	if ((p_ptr->special_attack & ATTACK_POIS) && !(r_ptr->flags3 & RF3_IM_POIS)) return (TRUE);

	/* Speed */
	if (p_ptr->pspeed < 145)
	{
		if (p_ptr->fast) return (TRUE);

		if (music_singing(MUSIC_SPEED)) return (TRUE);

		if (music_singing(MUSIC_SHERO)) return (TRUE);
	}

	/* Light speed */
	if (p_ptr->lightspeed && (m_ptr->mspeed < 136)) return (TRUE);

	if (p_ptr->riding && (m_list[p_ptr->riding].mspeed < 135))
	{
		if (m_list[p_ptr->riding].fast) return (TRUE);
	}

	/* No need to cast dispel spell */
	return (FALSE);
}


/*
 * Have a monster choose a spell from a list of "useful" spells.
 *
 * Note that this list does NOT include spells that will just hit
 * other monsters, and the list is restricted when the monster is
 * "desperate".  Should that be the job of this function instead?
 *
 * Stupid monsters will just pick a spell randomly.  Smart monsters
 * will choose more "intelligently".
 *
 * Use the helper functions above to put spells into categories.
 *
 * This function may well be an efficiency bottleneck.
 */
static int choose_attack_spell(int m_idx, byte spells[], byte num)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	byte escape[96], escape_num = 0;
	byte attack[96], attack_num = 0;
	byte summon[96], summon_num = 0;
	byte tactic[96], tactic_num = 0;
	byte annoy[96], annoy_num = 0;
	byte invul[96], invul_num = 0;
	byte haste[96], haste_num = 0;
	byte world[96], world_num = 0;
	byte special[96], special_num = 0;
	byte psy_spe[96], psy_spe_num = 0;
	byte raise[96], raise_num = 0;
	byte heal[96], heal_num = 0;
	byte dispel[96], dispel_num = 0;

	int i;

	/* Stupid monsters choose randomly */
	if (r_ptr->flags2 & (RF2_STUPID))
	{
		/* Pick at random */
		return (spells[randint0(num)]);
	}

	/* Categorize spells */
	for (i = 0; i < num; i++)
	{
		/* Escape spell? */
		if (spell_escape(spells[i])) escape[escape_num++] = spells[i];

		/* Attack spell? */
		if (spell_attack(spells[i])) attack[attack_num++] = spells[i];

		/* Summon spell? */
		if (spell_summon(spells[i])) summon[summon_num++] = spells[i];

		/* Tactical spell? */
		if (spell_tactic(spells[i])) tactic[tactic_num++] = spells[i];

		/* Annoyance spell? */
		if (spell_annoy(spells[i])) annoy[annoy_num++] = spells[i];

		/* Invulnerability spell? */
		if (spell_invulner(spells[i])) invul[invul_num++] = spells[i];

		/* Haste spell? */
		if (spell_haste(spells[i])) haste[haste_num++] = spells[i];

		/* World spell? */
		if (spell_world(spells[i])) world[world_num++] = spells[i];

		/* Special spell? */
		if (spell_special(spells[i])) special[special_num++] = spells[i];

		/* Psycho-spear spell? */
		if (spell_psy_spe(spells[i])) psy_spe[psy_spe_num++] = spells[i];

		/* Raise-dead spell? */
		if (spell_raise(spells[i])) raise[raise_num++] = spells[i];

		/* Heal spell? */
		if (spell_heal(spells[i])) heal[heal_num++] = spells[i];

		/* Dispel spell? */
		if (spell_dispel(spells[i])) dispel[dispel_num++] = spells[i];
	}

	/*** Try to pick an appropriate spell type ***/

	/* world */
	if (world_num && (randint0(100) < 15) && !world_monster)
	{
		/* Choose haste spell */
		return (world[randint0(world_num)]);
	}

	/* special */
	if (special_num)
	{
		bool success = FALSE;
		switch(m_ptr->r_idx)
		{
			case MON_BANOR:
			case MON_LUPART:
				if ((m_ptr->hp < m_ptr->maxhp / 2) && r_info[MON_BANOR].max_num && r_info[MON_LUPART].max_num) success = TRUE;
				break;
			default: break;
		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Still hurt badly, couldn't flee, attempt to heal */
	if (m_ptr->hp < m_ptr->maxhp / 3 && one_in_(2))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Hurt badly or afraid, attempt to flee */
	if (((m_ptr->hp < m_ptr->maxhp / 3) || m_ptr->monfear) && one_in_(2))
	{
		/* Choose escape spell if possible */
		if (escape_num) return (escape[randint0(escape_num)]);
	}

	/* special */
	if (special_num)
	{
		bool success = FALSE;
		switch(m_ptr->r_idx)
		{
			case MON_OHMU:
				if (randint0(100) < 50) success = TRUE;
				break;
			case MON_BANORLUPART:
				if (randint0(100) < 70) success = TRUE;
				break;
			case MON_BANOR:
			case MON_LUPART:
				break;
			default:
				if (randint0(100) < 50) success = TRUE;
				break;
		}
		if (success) return (special[randint0(special_num)]);
	}

	/* Player is close and we have attack spells, blink away */
	if ((distance(py, px, m_ptr->fy, m_ptr->fx) < 4) && (attack_num || (r_ptr->flags6 & RF6_TRAPS)) && (randint0(100) < 75) && !world_monster)
	{
		/* Choose tactical spell */
		if (tactic_num) return (tactic[randint0(tactic_num)]);
	}

	/* Summon if possible (sometimes) */
	if (summon_num && (randint0(100) < 40))
	{
		/* Choose summon spell */
		return (summon[randint0(summon_num)]);
	}

	/* dispel */
	if (dispel_num && one_in_(2))
	{
		/* Choose dispel spell if possible */
		if (dispel_check(m_idx))
		{
			return (dispel[randint0(dispel_num)]);
		}
	}

	/* Raise-dead if possible (sometimes) */
	if (raise_num && (randint0(100) < 40) && raise_possible(m_ptr->fy, m_ptr->fx))
	{
		/* Choose raise-dead spell */
		return (raise[randint0(raise_num)]);
	}

	/* Attack spell (most of the time) */
	if (p_ptr->invuln)
	{
		if (psy_spe_num && (randint0(100) < 50))
		{
			/* Choose attack spell */
			return (psy_spe[randint0(psy_spe_num)]);
		}
		else if (attack_num && (randint0(100) < 40))
		{
			/* Choose attack spell */
			return (attack[randint0(attack_num)]);
		}
	}
	else if (attack_num && (randint0(100) < 85))
	{
		/* Choose attack spell */
		return (attack[randint0(attack_num)]);
	}

	/* Try another tactical spell (sometimes) */
	if (tactic_num && (randint0(100) < 50) && !world_monster)
	{
		/* Choose tactic spell */
		return (tactic[randint0(tactic_num)]);
	}

	/* Cast globe of invulnerability if not already in effect */
	if (invul_num && !(m_ptr->invulner) && (randint0(100) < 50))
	{
		/* Choose Globe of Invulnerability */
		return (invul[randint0(invul_num)]);
	}

	/* We're hurt (not badly), try to heal */
	if ((m_ptr->hp < m_ptr->maxhp * 3 / 4) && (randint0(100) < 25))
	{
		/* Choose heal spell if possible */
		if (heal_num) return (heal[randint0(heal_num)]);
	}

	/* Haste self if we aren't already somewhat hasted (rarely) */
	if (haste_num && (randint0(100) < 20) && !(m_ptr->fast))
	{
		/* Choose haste spell */
		return (haste[randint0(haste_num)]);
	}

	/* Annoy player (most of the time) */
	if (annoy_num && (randint0(100) < 80))
	{
		/* Choose annoyance spell */
		return (annoy[randint0(annoy_num)]);
	}

	/* Choose no spell */
	return (0);
}


/*
 * Creatures can cast spells, shoot missiles, and breathe.
 *
 * Returns "TRUE" if a spell (or whatever) was (successfully) cast.
 *
 * XXX XXX XXX This function could use some work, but remember to
 * keep it as optimized as possible, while retaining generic code.
 *
 * Verify the various "blind-ness" checks in the code.
 *
 * XXX XXX XXX Note that several effects should really not be "seen"
 * if the player is blind.  See also "effects.c" for other "mistakes".
 *
 * Perhaps monsters should breathe at locations *near* the player,
 * since this would allow them to inflict "partial" damage.
 *
 * Perhaps smart monsters should decline to use "bolt" spells if
 * there is a monster in the way, unless they wish to kill it.
 *
 * Note that, to allow the use of the "track_target" option at some
 * later time, certain non-optimal things are done in the code below,
 * including explicit checks against the "direct" variable, which is
 * currently always true by the time it is checked, but which should
 * really be set according to an explicit "projectable()" test, and
 * the use of generic "x,y" locations instead of the player location,
 * with those values being initialized with the player location.
 *
 * It will not be possible to "correctly" handle the case in which a
 * monster attempts to attack a location which is thought to contain
 * the player, but which in fact is nowhere near the player, since this
 * might induce all sorts of messages about the attack itself, and about
 * the effects of the attack, which the player might or might not be in
 * a position to observe.  Thus, for simplicity, it is probably best to
 * only allow "faulty" attacks by a monster if one of the important grids
 * (probably the initial or final grid) is in fact in view of the player.
 * It may be necessary to actually prevent spell attacks except when the
 * monster actually has line of sight to the player.  Note that a monster
 * could be left in a bizarre situation after the player ducked behind a
 * pillar and then teleported away, for example.
 *
 * Note that certain spell attacks do not use the "project()" function
 * but "simulate" it via the "direct" variable, which is always at least
 * as restrictive as the "project()" function.  This is necessary to
 * prevent "blindness" attacks and such from bending around walls, etc,
 * and to allow the use of the "track_target" option in the future.
 *
 * Note that this function attempts to optimize the use of spells for the
 * cases in which the monster has no spells, or has spells but cannot use
 * them, or has spells but they will have no "useful" effect.  Note that
 * this function has been an efficiency bottleneck in the past.
 *
 * Note the special "MFLAG_NICE" flag, which prevents a monster from using
 * any spell attacks until the player has had a single chance to move.
 */
bool make_attack_spell(int m_idx)
{
	int             k, chance, thrown_spell = 0, rlev, failrate;
	byte            spell[96], num = 0;
	u32b            f4, f5, f6;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];
	char            m_poss[80];
	char            ddesc[80];
	bool            no_inate = FALSE;
	bool            do_disi = FALSE;
	int             dam = 0;
	u32b mode = 0L;
	int s_num_6 = (easy_band ? 2 : 6);
	int s_num_4 = (easy_band ? 1 : 4);

	/* Target location */
	int x = px;
	int y = py;

	/* Summon count */
	int count = 0;

	/* Extract the blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Extract the "see-able-ness" */
	bool seen = (!blind && m_ptr->ml);

	bool maneable = player_has_los_bold(m_ptr->fy, m_ptr->fx);
	bool learnable = (seen && maneable && !world_monster);

	/* Assume "normal" target */
	bool normal = TRUE;

	/* Assume "projectable" */
	bool direct = TRUE;

	/* Cannot cast spells when confused */
	if (m_ptr->confused)
	{
		reset_target(m_ptr);
		return (FALSE);
	}

	/* Cannot cast spells when nice */
	if (m_ptr->mflag & MFLAG_NICE) return (FALSE);
	if (!is_hostile(m_ptr)) return (FALSE);

	/* Hack -- Extract the spell probability */
	chance = (r_ptr->freq_inate + r_ptr->freq_spell) / 2;

	/* Not allowed to cast spells */
	if (!chance) return (FALSE);


	/* Only do spells occasionally */
	if (randint0(100) >=  chance) return (FALSE);

	/* Sometimes forbid inate attacks (breaths) */
	if (randint0(100) >= (chance * 2)) no_inate = TRUE;

	/* XXX XXX XXX Handle "track_target" option (?) */


	/* Extract the racial spell flags */
	f4 = r_ptr->flags4;
	f5 = r_ptr->flags5;
	f6 = r_ptr->flags6;

	/* Hack -- require projectable player */
	if (normal)
	{
		/* Check range */
		if ((m_ptr->cdis > MAX_RANGE) && !m_ptr->target_y) return (FALSE);

		/* Check path */
		if (projectable(m_ptr->fy, m_ptr->fx, y, x))
		{
			/* Breath disintegration to the glyph */
			if ((!cave_floor_bold(y,x)) && (r_ptr->flags4 & RF4_BR_DISI) && one_in_(2)) do_disi = TRUE;
		}

		/* Check path to next grid */
		else
		{
			bool success = FALSE;

			if ((r_ptr->flags4 & RF4_BR_DISI) &&
			    (m_ptr->cdis < MAX_RANGE/2) &&
			    in_disintegration_range(m_ptr->fy, m_ptr->fx, y, x) &&
			    (one_in_(10) || (projectable(y, x, m_ptr->fy, m_ptr->fx) && one_in_(2))))
			{
				do_disi = TRUE;
				success = TRUE;
			}
			else
			{
				int i;
				int tonari;
				int tonari_y[4][8] = {{-1,-1,-1,0,0,1,1,1},
						      {-1,-1,-1,0,0,1,1,1},
						      {1,1,1,0,0,-1,-1,-1},
						      {1,1,1,0,0,-1,-1,-1}};
				int tonari_x[4][8] = {{-1,0,1,-1,1,-1,0,1},
						      {1,0,-1,1,-1,1,0,-1},
						      {-1,0,1,-1,1,-1,0,1},
						      {1,0,-1,1,-1,1,0,-1}};

				if (m_ptr->fy < py && m_ptr->fx < px) tonari = 0;
				else if (m_ptr->fy < py) tonari = 1;
				else if (m_ptr->fx < px) tonari = 2;
				else tonari = 3;

				for (i = 0; i < 8; i++)
				{
					int next_x = x + tonari_x[tonari][i];
					int next_y = y + tonari_y[tonari][i];
					cave_type *c_ptr;

					/* Access the next grid */
					c_ptr = &cave[next_y][next_x];

					/* Skip door, rubble, wall */
					if ((c_ptr->feat >= FEAT_DOOR_HEAD) && (c_ptr->feat <= FEAT_PERM_SOLID)) continue;

					/* Skip tree */
					if (c_ptr->feat == FEAT_TREES) continue;

					/* Skip mountain */
					if (c_ptr->feat == FEAT_MOUNTAIN) continue;

					if (projectable(m_ptr->fy, m_ptr->fx, next_y, next_x))
					{
						y = next_y;
						x = next_x;
						success = TRUE;
						break;
					}
				}
			}

			if (!success)
			{
				if (m_ptr->target_y && m_ptr->target_x)
				{
					y = m_ptr->target_y;
					x = m_ptr->target_x;
					f4 &= (RF4_INDIRECT_MASK);
					f5 &= (RF5_INDIRECT_MASK);
					f6 &= (RF6_INDIRECT_MASK);
					success = TRUE;
				}
			}

			/* No spells */
			if (!success) return FALSE;
		}
	}

	reset_target(m_ptr);

	/* Extract the monster level */
	rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

	/* Forbid inate attacks sometimes */
	if (no_inate) f4 &= 0x500000FF;

	if (!p_ptr->csp)
	{
		f5 &= ~(RF5_DRAIN_MANA);
	}
	if ((p_ptr->pclass == CLASS_NINJA) && (r_ptr->flags3 & (RF3_UNDEAD | RF3_HURT_LITE)))
	{
		f6 &= ~(RF6_DARKNESS);
	}

	if (dun_level && (!p_ptr->inside_quest || (p_ptr->inside_quest < MIN_RANDOM_QUEST)) && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
	{
		f4 &= (RF4_NOMAGIC_MASK);
		f5 &= (RF5_NOMAGIC_MASK);
		f6 &= (RF6_NOMAGIC_MASK);
	}

	/* Hack -- allow "desperate" spells */
	if ((r_ptr->flags2 & (RF2_SMART)) &&
		(m_ptr->hp < m_ptr->maxhp / 10) &&
		(randint0(100) < 50))
	{
		/* Require intelligent spells */
		f4 &= (RF4_INT_MASK);
		f5 &= (RF5_INT_MASK);
		f6 &= (RF6_INT_MASK);

		/* No spells left */
		if (!f4 && !f5 && !f6) return (FALSE);
	}

	/* Remove the "ineffective" spells */
	remove_bad_spells(m_idx, &f4, &f5, &f6);

	if (p_ptr->inside_arena)
	{
		f4 &= ~(RF4_SUMMON_MASK);
		f5 &= ~(RF5_SUMMON_MASK);
		f6 &= ~(RF6_SUMMON_MASK);
	}

	/* No spells left */
	if (!f4 && !f5 && !f6) return (FALSE);

	/* Check for a clean bolt shot */
	if (((f4 & RF4_BOLT_MASK) ||
	     (f5 & RF5_BOLT_MASK) ||
	     (f6 & RF6_BOLT_MASK)) &&
	    !(r_ptr->flags2 & RF2_STUPID) &&
	    !clean_shot(m_ptr->fy, m_ptr->fx, py, px, FALSE))
	{
		/* Remove spells that will only hurt friends */
		f4 &= ~(RF4_BOLT_MASK);
		f5 &= ~(RF5_BOLT_MASK);
		f6 &= ~(RF6_BOLT_MASK);
	}

	/* Check for a possible summon */
	if (((f4 & RF4_SUMMON_MASK) ||
	     (f5 & RF5_SUMMON_MASK) ||
	     (f6 & RF6_SUMMON_MASK)) &&
	    !(r_ptr->flags2 & RF2_STUPID) &&
	    !(summon_possible(y, x)))
	{
		/* Remove summoning spells */
		f4 &= ~(RF4_SUMMON_MASK);
		f5 &= ~(RF5_SUMMON_MASK);
		f6 &= ~(RF6_SUMMON_MASK);
	}

	/* No spells left */
	if (!f4 && !f5 && !f6) return (FALSE);

	/* Extract the "inate" spells */
	for (k = 0; k < 32; k++)
	{
		if (f4 & (1L << k)) spell[num++] = k + 32 * 3;
	}

	/* Extract the "normal" spells */
	for (k = 0; k < 32; k++)
	{
		if (f5 & (1L << k)) spell[num++] = k + 32 * 4;
	}

	/* Extract the "bizarre" spells */
	for (k = 0; k < 32; k++)
	{
		if (f6 & (1L << k)) spell[num++] = k + 32 * 5;
	}

	/* No spells left */
	if (!num) return (FALSE);

	/* Stop if player is dead or gone */
	if (!p_ptr->playing || p_ptr->is_dead) return (FALSE);

	/* Stop if player is leaving */
	if (p_ptr->leaving) return (FALSE);

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0x00);

	/* Get the monster possessive ("his"/"her"/"its") */
	monster_desc(m_poss, m_ptr, 0x22);

	/* Hack -- Get the "died from" name */
	monster_desc(ddesc, m_ptr, 0x288);

	if (do_disi)
		thrown_spell = 96+31;
	else
	{
		int attempt = 10;
		while(attempt--)
		{
			thrown_spell = choose_attack_spell(m_idx, spell, num);
			if (thrown_spell) break;
		}
	}

	/* Abort if no spell was chosen */
	if (!thrown_spell) return (FALSE);

	/* Calculate spell failure rate */
	failrate = 25 - (rlev + 3) / 4;

	/* Hack -- Stupid monsters will never fail (for jellies and such) */
	if (r_ptr->flags2 & RF2_STUPID) failrate = 0;

	/* Check for spell failure (inate attacks never fail) */
	if ((thrown_spell >= 128) && ((m_ptr->stunned && one_in_(2)) || (randint0(100) < failrate)))
	{
		disturb(1, 0);
		/* Message */
		if (thrown_spell != (160+7)) /* Not RF6_SPECIAL */
		{
#ifdef JP
			msg_format("%^s�ϼ�ʸ�򾧤��褦�Ȥ��������Ԥ�����", m_name);
#else
			msg_format("%^s tries to cast a spell, but fails.", m_name);
#endif
		}

		return (TRUE);
	}


	/* Cast the spell. */
	switch (thrown_spell)
	{
		/* RF4_SHRIEK */
		case 96+0:
		{
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
msg_format("%^s������⤤���ڤ����򤢤�����", m_name);
#else
			msg_format("%^s makes a high pitched shriek.", m_name);
#endif

			aggravate_monsters(m_idx);
			break;
		}

		/* RF4_XXX1 */
		case 96+1:
		{
			/* XXX XXX XXX */
			break;
		}

		/* RF4_DISPEL */
		case 96+2:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
			if (blind) msg_format("%^s���������϶����Ĥ֤䤤����", m_name);
			else msg_format("%^s�����Ͼõ�μ�ʸ��ǰ������", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
			else msg_format("%^s invokes a dispel magic.", m_name);
#endif
			set_fast(0, TRUE);
			set_lightspeed(0, TRUE);
			set_slow(0, TRUE);
			set_shield(0, TRUE);
			set_blessed(0, TRUE);
			set_tsuyoshi(0, TRUE);
			set_hero(0, TRUE);
			set_shero(0, TRUE);
			set_protevil(0, TRUE);
			set_invuln(0, TRUE);
			set_wraith_form(0, TRUE);
			set_kabenuke(0, TRUE);
			set_tim_res_nether(0, TRUE);
			set_tim_res_time(0, TRUE);
			/* by henkma */
			set_tim_reflect(0,TRUE);
			set_multishadow(0,TRUE);
			set_dustrobe(0,TRUE);

			set_tim_invis(0, TRUE);
			set_tim_infra(0, TRUE);
			set_tim_esp(0, TRUE);
			set_tim_regen(0, TRUE);
			set_tim_stealth(0, TRUE);
			set_tim_ffall(0, TRUE);
			set_tim_sh_touki(0, TRUE);
			set_tim_sh_fire(0, TRUE);
			set_tim_sh_holy(0, TRUE);
			set_tim_eyeeye(0, TRUE);
			set_magicdef(0, TRUE);
			set_resist_magic(0, TRUE);
			set_oppose_acid(0, TRUE);
			set_oppose_elec(0, TRUE);
			set_oppose_fire(0, TRUE);
			set_oppose_cold(0, TRUE);
			set_oppose_pois(0, TRUE);
			set_ultimate_res(0, TRUE);
			set_mimic(0, 0, TRUE);
			set_ele_attack(0, 0);
			set_ele_immune(0, 0);
			/* Cancel glowing hands */
			if (p_ptr->special_attack & ATTACK_CONFUSE)
			{
				p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
				msg_print("��ε������ʤ��ʤä���");
#else
				msg_print("Your hands stop glowing.");
#endif

			}
			if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0]))
			{
				p_ptr->magic_num1[1] = p_ptr->magic_num1[0];
				p_ptr->magic_num1[0] = 0;
#ifdef JP
				msg_print("�Τ����ڤ줿��");
#else
				msg_print("Your singing is interrupted.");
#endif
				p_ptr->action = ACTION_NONE;

				/* Recalculate bonuses */
				p_ptr->update |= (PU_BONUS | PU_HP);

				/* Redraw map */
				p_ptr->redraw |= (PR_MAP | PR_STATUS | PR_STATE);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

				p_ptr->energy_need += ENERGY_NEED();
			}
			if (p_ptr->riding)
			{
				m_list[p_ptr->riding].invulner = 0;
				m_list[p_ptr->riding].fast = 0;
				m_list[p_ptr->riding].slow = 0;
				p_ptr->update |= PU_BONUS;
				if (p_ptr->health_who == p_ptr->riding) p_ptr->redraw |= PR_HEALTH;
				p_ptr->redraw |= (PR_UHEALTH);
			}

#ifdef JP
			if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				msg_print("���䤬�ä��ʡ�");
#endif
			learn_spell(MS_DISPEL);
			break;
		}

		/* RF4_XXX4X4 */
		case 96+3:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������ͤä���", m_name);
#else
			if (blind) msg_format("%^s shoots something.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����åȤ�ȯ�ͤ�����", m_name);
#else
			else msg_format("%^s fires a rocket.", m_name);
#endif

			dam = ((m_ptr->hp / 4) > 800 ? 800 : (m_ptr->hp / 4));
			breath(y, x, m_idx, GF_ROCKET,
				dam, 2, FALSE, MS_ROCKET, learnable);
			update_smart_learn(m_idx, DRS_SHARD);
			break;
		}

		/* RF4_SHOOT */
		case 96+4:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s����̯�ʲ���ȯ������", m_name);
#else
			if (blind) msg_format("%^s makes a strange noise.", m_name);
#endif

#ifdef JP
else msg_format("%^s��������ä���", m_name);
#else
			else msg_format("%^s fires an arrow.", m_name);
#endif

			dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);
			bolt(m_idx, GF_ARROW, dam, MS_SHOOT, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF4_XXX2 */
		case 96+5:
		{
			/* XXX XXX XXX */
			break;
		}

		/* RF4_XXX3 */
		case 96+6:
		{
			/* XXX XXX XXX */
			break;
		}

		/* RF4_XXX4 */
		case 96+7:
		{
			/* XXX XXX XXX */
			break;
		}

		/* RF4_BR_ACID */
		case 96+8:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes acid.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_ACID, dam, 0, TRUE, MS_BR_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF4_BR_ELEC */
		case 96+9:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ʤΥ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes lightning.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_ELEC, dam,0, TRUE, MS_BR_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF4_BR_FIRE */
		case 96+10:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���б�Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes fire.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_FIRE, dam,0, TRUE, MS_BR_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}

		/* RF4_BR_COLD */
		case 96+11:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���䵤�Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes frost.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_COLD, dam,0, TRUE, MS_BR_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}

		/* RF4_BR_POIS */
		case 96+12:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes gas.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_POIS, dam, 0, TRUE, MS_BR_POIS, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}


		/* RF4_BR_NETH */
		case 96+13:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ϲ��Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes nether.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_NETHER, dam,0, TRUE, MS_BR_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF4_BR_LITE */
		case 96+14:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes light.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_LITE, dam,0, TRUE, MS_BR_LITE, learnable);
			update_smart_learn(m_idx, DRS_LITE);
			break;
		}

		/* RF4_BR_DARK */
		case 96+15:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ź��Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes darkness.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DARK, dam,0, TRUE, MS_BR_DARK, learnable);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF4_BR_CONF */
		case 96+16:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s������Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes confusion.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_CONFUSION, dam,0, TRUE, MS_BR_CONF, learnable);
			update_smart_learn(m_idx, DRS_CONF);
			break;
		}

		/* RF4_BR_SOUN */
		case 96+17:
		{
			disturb(1, 0);
			if (m_ptr->r_idx == MON_JAIAN)
#ifdef JP
				msg_format("�֥ܥ�����������������");
#else
				msg_format("'Booooeeeeee'");
#endif
#ifdef JP
else if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���첻�Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes sound.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_SOUND, dam,0, TRUE, MS_BR_SOUND, learnable);
			update_smart_learn(m_idx, DRS_SOUND);
			break;
		}

		/* RF4_BR_CHAO */
		case 96+18:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes chaos.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_CHAOS, dam,0, TRUE, MS_BR_CHAOS, learnable);
			update_smart_learn(m_idx, DRS_CHAOS);
			break;
		}

		/* RF4_BR_DISE */
		case 96+19:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes disenchantment.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DISENCHANT, dam,0, TRUE, MS_BR_DISEN, learnable);
			update_smart_learn(m_idx, DRS_DISEN);
			break;
		}

		/* RF4_BR_NEXU */
		case 96+20:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����̺���Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes nexus.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_NEXUS, dam,0, TRUE, MS_BR_NEXUS, learnable);
			update_smart_learn(m_idx, DRS_NEXUS);
			break;
		}

		/* RF4_BR_TIME */
		case 96+21:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ֵ�ž�Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes time.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_TIME, dam,0, TRUE, MS_BR_TIME, learnable);
			break;
		}

		/* RF4_BR_INER */
		case 96+22:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ߤΥ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes inertia.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_INERTIA, dam,0, TRUE, MS_BR_INERTIA, learnable);
			break;
		}

		/* RF4_BR_GRAV */
		case 96+23:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ϤΥ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes gravity.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_GRAVITY, dam,0, TRUE, MS_BR_GRAVITY, learnable);
			break;
		}

		/* RF4_BR_SHAR */
		case 96+24:
		{
			disturb(1, 0);
			if (m_ptr->r_idx == MON_BOTEI)
#ifdef JP
				msg_format("�֥���ӥ륫�å�����������");
#else
				msg_format("'Boty-Build cutter!!!'");
#endif
#ifdef JP
else if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			else if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ҤΥ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes shards.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_SHARDS, dam,0, TRUE, MS_BR_SHARDS, learnable);
			update_smart_learn(m_idx, DRS_SHARD);
			break;
		}

		/* RF4_BR_PLAS */
		case 96+25:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���ץ饺�ޤΥ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes plasma.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_PLASMA, dam,0, TRUE, MS_BR_PLASMA, learnable);
			break;
		}

		/* RF4_BR_WALL */
		case 96+26:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���ե������Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes force.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_FORCE, dam,0, TRUE, MS_BR_FORCE, learnable);
			break;
		}

		/* RF4_BR_MANA */
		case 96+27:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ϤΥ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes mana.", m_name);
#endif
			dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_MANA, dam,0, TRUE, MS_BR_MANA, learnable);
			break;
		}

		/* RF4_BA_NUKE */
		case 96+28:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s������ǽ������ä���", m_name);
#else
			else msg_format("%^s casts a ball of radiation.", m_name);
#endif

			dam = (rlev + damroll(10, 6)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_NUKE, dam, 2, FALSE, MS_BALL_NUKE, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF4_BR_NUKE */
		case 96+29:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������Ѵ�ʪ�Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes toxic waste.", m_name);
#endif

			dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
			breath(y, x, m_idx, GF_NUKE, dam,0, TRUE, MS_BR_NUKE, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF4_BA_CHAO */
		case 96+30:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s���������ˤĤ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles frighteningly.", m_name);
#endif

#ifdef JP
else msg_format("%^s������륹�����ä���", m_name);/*nuke me*/
#else
			else msg_format("%^s invokes a raw Logrus.", m_name);
#endif

			dam = ((r_ptr->flags2 & RF2_POWERFUL) ? (rlev * 3) : (rlev * 2))+ damroll(10, 10);
			breath(y, x, m_idx, GF_CHAOS, dam, 4, FALSE, MS_BALL_CHAOS, learnable);
			update_smart_learn(m_idx, DRS_CHAOS);
			break;
		}

		/* RF4_BR_DISI */
		case 96+31:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
			if (blind) msg_format("%^s breathes.", m_name);
#endif

#ifdef JP
else msg_format("%^s��ʬ��Υ֥쥹���Ǥ�����", m_name);
#else
			else msg_format("%^s breathes disintegration.", m_name);
#endif

			dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
			breath(y, x, m_idx, GF_DISINTEGRATE, dam,0, TRUE, MS_BR_DISI, learnable);
			break;
		}



		/* RF5_BA_ACID */
		case 128+0:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������åɡ��ܡ���μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts an acid ball.", m_name);
#endif

			dam = (randint1(rlev * 3) + 15) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_ACID, dam, 2, FALSE, MS_BALL_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			break;
		}

		/* RF5_BA_ELEC */
		case 128+1:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s������������ܡ���μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a lightning ball.", m_name);
#endif

			dam = (randint1(rlev * 3 / 2) + 8) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_ELEC, dam, 2, FALSE, MS_BALL_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			break;
		}

		/* RF5_BA_FIRE */
		case 128+2:
		{
			disturb(1, 0);

			if (m_ptr->r_idx == MON_ROLENTO)
			{
#ifdef JP
				if (blind)
					msg_format("%s���������ꤲ����", m_name);
				else 
					msg_format("%s�ϼ����Ƥ��ꤲ����", m_name);
#else
				if (blind)
					msg_format("%^s throws something.", m_name);
				else
					msg_format("%^s throws a hand grenade.", m_name);
#endif
			}
			else
			{
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���ե��������ܡ���μ�ʸ�򾧤�����", m_name);
#else
				else msg_format("%^s casts a fire ball.", m_name);
#endif
			}

			dam = (randint1(rlev * 7 / 2) + 10) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_FIRE, dam, 2, FALSE, MS_BALL_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			break;
		}

		/* RF5_BA_COLD */
		case 128+3:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����������ܡ���μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a frost ball.", m_name);
#endif

			dam = (randint1(rlev * 3 / 2) + 10) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_COLD, dam, 2, FALSE, MS_BALL_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			break;
		}

		/* RF5_BA_POIS */
		case 128+4:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a stinking cloud.", m_name);
#endif

			dam = damroll(12, 2) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			breath(y, x, m_idx, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);
			update_smart_learn(m_idx, DRS_POIS);
			break;
		}

		/* RF5_BA_NETH */
		case 128+5:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ϲ���μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a nether ball.", m_name);
#endif

			dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
			breath(y, x, m_idx, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			break;
		}

		/* RF5_BA_WATE */
		case 128+6:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s��ή���褦�ʿȿ���򤷤���", m_name);
#else
			else msg_format("%^s gestures fluidly.", m_name);
#endif

#ifdef JP
msg_print("���ʤ��ϱ������˰��߹��ޤ줿��");
#else
			msg_print("You are engulfed in a whirlpool.");
#endif

			dam = ((r_ptr->flags2 & RF2_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
			breath(y, x, m_idx, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);
			break;
		}

		/* RF5_BA_MANA */
		case 128+7:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s���������϶����Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ϥ���μ�ʸ��ǰ������", m_name);
#else
			else msg_format("%^s invokes a mana storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);
			break;
		}

		/* RF5_BA_DARK */
		case 128+8:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s���������϶����Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ź�����μ�ʸ��ǰ������", m_name);
#else
			else msg_format("%^s invokes a darkness storm.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);
			update_smart_learn(m_idx, DRS_DARK);
			break;
		}

		/* RF5_DRAIN_MANA */
		case 128+9:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
			if (p_ptr->csp)
			{
				int r1;

				/* Basic message */
#ifdef JP
msg_format("%^s���������ͥ륮����ۤ�����Ƥ��ޤä���", m_name);
#else
				msg_format("%^s draws psychic energy from you!", m_name);
#endif


				/* Attack power */
				r1 = (randint1(rlev) / 2) + 1;

				/* Full drain */
				if (r1 >= p_ptr->csp)
				{
					r1 = p_ptr->csp;
					p_ptr->csp = 0;
					p_ptr->csp_frac = 0;
				}

				/* Partial drain */
				else
				{
					p_ptr->csp -= r1;
				}

				learn_spell(MS_DRAIN_MANA);

				/* Redraw mana */
				p_ptr->redraw |= (PR_MANA);

				/* Window stuff */
				p_ptr->window |= (PW_PLAYER);
				p_ptr->window |= (PW_SPELL);

				/* Heal the monster */
				if (m_ptr->hp < m_ptr->maxhp)
				{
					/* Heal */
					m_ptr->hp += (6 * r1);
					if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

					/* Redraw (later) if needed */
					if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
					if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

					/* Special message */
					if (seen)
					{
#ifdef JP
msg_format("%^s�ϵ�ʬ���ɤ���������", m_name);
#else
						msg_format("%^s appears healthier.", m_name);
#endif

					}
				}
			}
			update_smart_learn(m_idx, DRS_MANA);
			break;
		}

		/* RF5_MIND_BLAST */
		case 128+10:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
			if (!seen)
			{
#ifdef JP
msg_print("���������ʤ���������ǰ�����äƤ���褦����");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s�����ʤ���Ʒ�򤸤äȤˤ��Ǥ��롣", m_name);
#else
				msg_format("%^s gazes deep into your eyes.", m_name);
#endif

			}

			dam = damroll(7, 7);
			breath(y, x, m_idx, GF_MIND_BLAST, dam, 0, FALSE, MS_MIND_BLAST, learnable);
			break;
		}

		/* RF5_BRAIN_SMASH */
		case 128+11:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
			if (!seen)
			{
#ifdef JP
msg_print("���������ʤ���������ǰ�����äƤ���褦����");
#else
				msg_print("You feel something focusing on your mind.");
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s�����ʤ���Ʒ�򤸤äȸ��Ƥ��롣", m_name);
#else
				msg_format("%^s looks deep into your eyes.", m_name);
#endif

			}

			dam = damroll(12, 12);
			breath(y, x, m_idx, GF_BRAIN_SMASH, dam, 0, FALSE, MS_BRAIN_SMASH, learnable);
			break;
		}

		/* RF5_CAUSE_1 */
		case 128+12:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʤ���ؤ����Ƽ��ä���", m_name);
#else
			else msg_format("%^s points at you and curses.", m_name);
#endif

			dam = damroll(3, 8);
			breath(y, x, m_idx, GF_CAUSE_1, dam, 0, FALSE, MS_CAUSE_1, learnable);
			break;
		}

		/* RF5_CAUSE_2 */
		case 128+13:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʤ���ؤ����ƶ������˼��ä���", m_name);
#else
			else msg_format("%^s points at you and curses horribly.", m_name);
#endif

			dam = damroll(8, 8);
			breath(y, x, m_idx, GF_CAUSE_2, dam, 0, FALSE, MS_CAUSE_2, learnable);
			break;
		}

		/* RF5_CAUSE_3 */
		case 128+14:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s�������������Ƕ������", m_name);
#else
			if (blind) msg_format("%^s mumbles loudly.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʤ���ؤ����ƶ������˼�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s points at you, incanting terribly!", m_name);
#endif

			dam = damroll(10, 15);
			breath(y, x, m_idx, GF_CAUSE_3, dam, 0, FALSE, MS_CAUSE_3, learnable);
			break;
		}

		/* RF5_CAUSE_4 */
		case 128+15:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s���֤����ϴ��˻��Ǥ���פȶ������", m_name);
#else
			if (blind) msg_format("%^s screams the word 'DIE!'", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʤ����빦���ͤ��ơ֤����ϴ��˻��Ǥ���פȶ������", m_name);
#else
			else msg_format("%^s points at you, screaming the word DIE!", m_name);
#endif

			dam = damroll(15, 15);
			breath(y, x, m_idx, GF_CAUSE_4, dam, 0, FALSE, MS_CAUSE_4, learnable);
			break;
		}

		/* RF5_BO_ACID */
		case 128+16:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������åɡ��ܥ�Ȥμ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a acid bolt.", m_name);
#endif

			dam = (damroll(7, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_ACID, dam, MS_BOLT_ACID, learnable);
			update_smart_learn(m_idx, DRS_ACID);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_ELEC */
		case 128+17:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s������������ܥ�Ȥμ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a lightning bolt.", m_name);
#endif

			dam = (damroll(4, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_ELEC, dam, MS_BOLT_ELEC, learnable);
			update_smart_learn(m_idx, DRS_ELEC);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_FIRE */
		case 128+18:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���ե��������ܥ�Ȥμ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a fire bolt.", m_name);
#endif

			dam = (damroll(9, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_FIRE, dam, MS_BOLT_FIRE, learnable);
			update_smart_learn(m_idx, DRS_FIRE);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_COLD */
		case 128+19:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����������ܥ�Ȥμ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a frost bolt.", m_name);
#endif

			dam = (damroll(6, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
			bolt(m_idx, GF_COLD, dam, MS_BOLT_COLD, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BA_LITE */
		case 128+20:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s���������϶����Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles powerfully.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������С����Ȥμ�ʸ��ǰ������", m_name);
#else
			else msg_format("%^s invokes a starburst.", m_name);
#endif

			dam = (rlev * 4) + 50 + damroll(10, 10);
			breath(y, x, m_idx, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);
			update_smart_learn(m_idx, DRS_LITE);
			break;
		}

		/* RF5_BO_NETH */
		case 128+21:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���Ϲ�����μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a nether bolt.", m_name);
#endif

			dam = 30 + damroll(5, 5) + (rlev * 4) / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3);
			bolt(m_idx, GF_NETHER, dam, MS_BOLT_NETHER, learnable);
			update_smart_learn(m_idx, DRS_NETH);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_WATE */
		case 128+22:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������������ܥ�Ȥμ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a water bolt.", m_name);
#endif

			dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_WATER, dam, MS_BOLT_WATER, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_MANA */
		case 128+23:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����Ϥ���μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a mana bolt.", m_name);
#endif

			dam = randint1(rlev * 7 / 2) + 50;
			bolt(m_idx, GF_MANA, dam, MS_BOLT_MANA, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_PLAS */
		case 128+24:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���ץ饺�ޡ��ܥ�Ȥμ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a plasma bolt.", m_name);
#endif

			dam = 10 + damroll(8, 7) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_PLASMA, dam, MS_BOLT_PLASMA, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_BO_ICEE */
		case 128+25:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���˴�����μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts an ice bolt.", m_name);
#endif

			dam = damroll(6, 6) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
			bolt(m_idx, GF_ICE, dam, MS_BOLT_ICE, learnable);
			update_smart_learn(m_idx, DRS_COLD);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_MISSILE */
		case 128+26:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���ޥ��å����ߥ�����μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a magic missile.", m_name);
#endif

			dam = damroll(2, 6) + (rlev / 3);
			bolt(m_idx, GF_MISSILE, dam, MS_MAGIC_MISSILE, learnable);
			update_smart_learn(m_idx, DRS_REFLECT);
			break;
		}

		/* RF5_SCARE */
		case 128+27:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤯�ȡ��������ʲ���ʹ��������", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear scary noises.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������ʸ��Ф���Ф�����", m_name);
#else
			else msg_format("%^s casts a fearful illusion.", m_name);
#endif

			if (p_ptr->resist_fear)
			{
#ifdef JP
msg_print("���������ݤ˿�����ʤ��ä���");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������ݤ˿�����ʤ��ä���");
#else
				msg_print("You refuse to be frightened.");
#endif

			}
			else
			{
				(void)set_afraid(p_ptr->afraid + randint0(4) + 4);
			}
			learn_spell(MS_SCARE);
			update_smart_learn(m_idx, DRS_FEAR);
			break;
		}

		/* RF5_BLIND */
		case 128+28:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ʸ�򾧤��Ƥ��ʤ����ܤ򤯤�ޤ�����", m_name);
#else
			else msg_format("%^s casts a spell, burning your eyes!", m_name);
#endif

			if (p_ptr->resist_blind)
			{
#ifdef JP
msg_print("���������̤��ʤ��ä���");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������Ϥ�ķ���֤�����");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_blind(12 + randint0(4));
			}
			learn_spell(MS_BLIND);
			update_smart_learn(m_idx, DRS_BLIND);
			break;
		}

		/* RF5_CONF */
		case 128+29:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤯�ȡ�Ƭ��Ǻ�ޤ�����������", m_name);
#else
			if (blind) msg_format("%^s mumbles, and you hear puzzling noises.", m_name);
#endif

#ifdef JP
else msg_format("%^s��Ͷ��Ū�ʸ��Ф���Ф�����", m_name);
#else
			else msg_format("%^s creates a mesmerising illusion.", m_name);
#endif

			if (p_ptr->resist_conf)
			{
#ifdef JP
msg_print("���������ФˤϤ��ޤ���ʤ��ä���");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������ФˤϤ��ޤ���ʤ��ä���");
#else
				msg_print("You disbelieve the feeble spell.");
#endif

			}
			else
			{
				(void)set_confused(p_ptr->confused + randint0(4) + 4);
			}
			learn_spell(MS_CONF);
			update_smart_learn(m_idx, DRS_CONF);
			break;
		}

		/* RF5_SLOW */
		case 128+30:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
msg_format("%^s�����ʤ��ζ��Ϥ�ۤ�����Ȥ�����", m_name);
#else
			msg_format("%^s drains power from your muscles!", m_name);
#endif

			if (p_ptr->free_act)
			{
#ifdef JP
msg_print("���������̤��ʤ��ä���");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������Ϥ�ķ���֤�����");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				(void)set_slow(p_ptr->slow + randint0(4) + 4, FALSE);
			}
			learn_spell(MS_SLOW);
			update_smart_learn(m_idx, DRS_FREE);
			break;
		}

		/* RF5_HOLD */
		case 128+31:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʤ����ܤ򤸤äȸ��Ĥ᤿��", m_name);
#else
			else msg_format("%^s stares deep into your eyes!", m_name);
#endif

			if (p_ptr->free_act)
			{
#ifdef JP
msg_print("���������̤��ʤ��ä���");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_format("���������Ϥ�ķ���֤�����");
#else
				msg_format("You resist the effects!");
#endif

			}
			else
			{
				(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
			}
			learn_spell(MS_SLEEP);
			update_smart_learn(m_idx, DRS_FREE);
			break;
		}

		/* RF6_HASTE */
		case 160+0:
		{
			disturb(1, 0);
			if (blind)
			{
#ifdef JP
msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s����ʬ���Τ�ǰ�����ä���", m_name, m_poss);
#else
				msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

			}

			/* Allow quick speed increases to base+10 */
			if (!m_ptr->fast)
			{
#ifdef JP
msg_format("%^s��ư����®���ʤä���", m_name);
#else
				msg_format("%^s starts moving faster.", m_name);
#endif
			}
			m_ptr->fast = MIN(200, m_ptr->fast + 100);
			if (p_ptr->riding == m_idx) p_ptr->update |= PU_BONUS;
			break;
		}

		/* RF6_HAND_DOOM */
		case 160+1:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
msg_format("%^s�����Ǥμ�����ä���", m_name);
#else
			msg_format("%^s invokes the Hand of Doom!", m_name);
#endif
			dam = (((s32b) ((40 + randint1(20)) * (p_ptr->chp))) / 100);
			breath(y, x, m_idx, GF_HAND_DOOM, dam, 0, FALSE, MS_HAND_DOOM, learnable);
			break;
		}

		/* RF6_HEAL */
		case 160+2:
		{
			disturb(1, 0);

			/* Message */
			if (blind)
			{
#ifdef JP
msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
				msg_format("%^s mumbles.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%^s����ʬ�ν��˽��椷����", m_name);
#else
				msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

			}

			/* Heal some */
			m_ptr->hp += (rlev * 6);

			/* Fully healed */
			if (m_ptr->hp >= m_ptr->maxhp)
			{
				/* Fully healed */
				m_ptr->hp = m_ptr->maxhp;

				/* Message */
				if (seen)
				{
#ifdef JP
msg_format("%^s�ϴ����˼��ä���", m_name);
#else
					msg_format("%^s looks completely healed!", m_name);
#endif

				}
				else
				{
#ifdef JP
msg_format("%^s�ϴ����˼��ä��褦����", m_name);
#else
					msg_format("%^s sounds completely healed!", m_name);
#endif

				}
			}

			/* Partially healed */
			else
			{
				/* Message */
				if (seen)
				{
#ifdef JP
msg_format("%^s�����Ϥ���������褦����", m_name);
#else
					msg_format("%^s looks healthier.", m_name);
#endif

				}
				else
				{
#ifdef JP
msg_format("%^s�����Ϥ���������褦����", m_name);
#else
					msg_format("%^s sounds healthier.", m_name);
#endif

				}
			}

			/* Redraw (later) if needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Cancel fear */
			if (m_ptr->monfear)
			{
				/* Cancel fear */
				m_ptr->monfear = 0;

				/* Message */
#ifdef JP
msg_format("%^s��ͦ�������ᤷ����", m_name, m_poss);
#else
				msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif

			}
			break;
		}

		/* RF6_INVULNER */
		case 160+3:
		{
			disturb(1, 0);

			/* Message */
			if (!seen)
			{
#ifdef JP
msg_format("%^s���������϶����Ĥ֤䤤����", m_name);
#else
				msg_format("%^s mumbles powerfully.", m_name);
#endif

			}
			else
			{
#ifdef JP
msg_format("%s��̵���ε�μ�ʸ�򾧤�����", m_name);
#else
				msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif

			}

			if (!(m_ptr->invulner))
				m_ptr->invulner = randint1(4) + 4;

			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);
			break;
		}

		/* RF6_BLINK */
		case 160+4:
		{
			disturb(1, 0);
#ifdef JP
msg_format("%^s���ֻ��˾ä�����", m_name);
#else
			msg_format("%^s blinks away.", m_name);
#endif

			teleport_away(m_idx, 10, FALSE);
			p_ptr->update |= (PU_MONSTERS | PU_MON_LITE);
			break;
		}

		/* RF6_TPORT */
		case 160+5:
		{
			int i, oldfy, oldfx;
			u32b flgs[TR_FLAG_SIZE];
			object_type *o_ptr;

			oldfy = m_ptr->fy;
			oldfx = m_ptr->fx;

			disturb(1, 0);
#ifdef JP
msg_format("%^s���ƥ�ݡ��Ȥ�����", m_name);
#else
			msg_format("%^s teleports away.", m_name);
#endif

			teleport_away(m_idx, MAX_SIGHT * 2 + 5, FALSE);

			if (los(py, px, oldfy, oldfx) && !world_monster)
			{
				for (i=INVEN_RARM;i<INVEN_TOTAL;i++)
				{
					o_ptr = &inventory[i];
					if(!cursed_p(o_ptr))
					{
						object_flags(o_ptr, flgs);

						if((have_flag(flgs, TR_TELEPORT)) || (p_ptr->muta1 & MUT1_VTELEPORT) || (p_ptr->pclass == CLASS_IMITATOR))
						{
#ifdef JP
							if(get_check_strict("�Ĥ��Ƥ����ޤ�����", CHECK_OKAY_CANCEL))
#else
							if(get_check_strict("Do you follow it? ", CHECK_OKAY_CANCEL))
#endif
							{
								if (one_in_(3))
								{
									teleport_player(200);
#ifdef JP
									msg_print("���ԡ�");
#else
									msg_print("Failed!");
#endif
								}
								else teleport_player_to(m_ptr->fy, m_ptr->fx, TRUE);
								p_ptr->energy_need += ENERGY_NEED();
							}
							break;
						}
					}
				}
			}
			break;
		}

		/* RF6_WORLD */
		case 160+6:
		{
			int who = 0;
			disturb(1, 0);
			if(m_ptr->r_idx == MON_DIO) who = 1;
			else if(m_ptr->r_idx == MON_WONG) who = 3;
			dam = who;
			if (!process_the_world(randint1(2)+2, who, TRUE)) return (FALSE);
			break;
		}

		/* RF6_SPECIAL */
		case 160+7:
		{
			int k;

			disturb(1, 0);
			switch(m_ptr->r_idx)
			{
			case MON_OHMU:
				if (p_ptr->inside_arena || p_ptr->inside_battle) return FALSE;
				for (k = 0; k < 6; k++)
				{
					count += summon_specific(m_idx, m_ptr->fy, m_ptr->fx, rlev, SUMMON_BIZARRE1, PM_ALLOW_GROUP);
				}
				return FALSE;
				
			case MON_BANORLUPART:
				{
					int dummy_hp = (m_ptr->hp + 1) / 2;
					int dummy_maxhp = m_ptr->maxhp/2;
					int dummy_y = m_ptr->fy;
					int dummy_x = m_ptr->fx;

					if (p_ptr->inside_arena || p_ptr->inside_battle || !summon_possible(m_ptr->fy, m_ptr->fx)) return FALSE;
					delete_monster_idx(cave[m_ptr->fy][m_ptr->fx].m_idx);
					summon_named_creature(0, dummy_y, dummy_x, MON_BANOR, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;
					summon_named_creature(0, dummy_y, dummy_x, MON_LUPART, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;

#ifdef JP
					msg_print("�إС��Ρ��롦��ѡ��ȡ٤�ʬ��������");
#else
					msg_print("Banor=Rupart splits in two person!");
#endif

					break;
				}
				case MON_BANOR:
				case MON_LUPART:
				{
					int dummy_hp = 0;
					int dummy_maxhp = 0;
					int dummy_y = m_ptr->fy;
					int dummy_x = m_ptr->fx;

					if (!r_info[MON_BANOR].cur_num || !r_info[MON_LUPART].cur_num) return (FALSE);
					for (k = 1; k < m_max; k++)
					{
						if (m_list[k].r_idx == MON_BANOR || m_list[k].r_idx == MON_LUPART)
						{
							dummy_hp += m_list[k].hp;
							dummy_maxhp += m_list[k].maxhp;
							if (m_list[k].r_idx != m_ptr->r_idx)
							{
								dummy_y = m_list[k].fy;
								dummy_x = m_list[k].fx;
							}
							delete_monster_idx(k);
						}
					}
					summon_named_creature(0, dummy_y, dummy_x, MON_BANORLUPART, mode);
					m_list[hack_m_idx_ii].hp = dummy_hp;
					m_list[hack_m_idx_ii].maxhp = dummy_maxhp;

#ifdef JP
					msg_print("�إС��Ρ���٤ȡإ�ѡ��ȡ٤����Τ�����");
#else
					msg_print("Banor and Rupart combine into one!");
#endif

					break;
				}

			default:
				if (r_ptr->d_char == 'B')
				{
					if (!direct) break;
					disturb(1, 0);
					if (one_in_(3) || x!=px || y!=py)
					{
#ifdef JP
						msg_format("%^s�������볦����ä���!", m_name);
#else
						msg_format("%^s suddenly go out of your sight!", m_name);
#endif
						teleport_away(m_idx, 10, FALSE);
						p_ptr->update |= (PU_MONSTERS | PU_MON_LITE);
						break;
					}
					else
					{
						int dam = damroll(4, 8);
						int get_damage = 0;
#ifdef JP
						msg_format("%^s�����ʤ����Ϥ�Ƕ��椫���ꤲ�����", m_name);
#else
						msg_format("%^s holds you, and drops from the sky.", m_name);
#endif
						teleport_player_to(m_ptr->fy, m_ptr->fx, FALSE);

						sound(SOUND_FALL);

						if (p_ptr->ffall)
						{
#ifdef JP
							msg_print("���ʤ����Ť������Ϥ�����");
#else
							msg_print("You float gently down to the ground.");
#endif
						}
						else
						{
#ifdef JP
							msg_print("���ʤ������̤�á���Ĥ���줿��");
#else
							msg_print("You crashed into the ground.");
#endif
							dam += damroll(6, 8);
						}

						/* Mega hack -- this special action deals damage to the player. Therefore the code of "eyeeye" is necessary.
						   -- henkma
						 */
						get_damage = take_hit(DAMAGE_NOESCAPE, dam, m_name, -1);
						if (p_ptr->tim_eyeeye && get_damage > 0 && !p_ptr->is_dead)
						{
#ifdef JP
							msg_format("���⤬%s���Ȥ���Ĥ�����", m_name);
#else
							char m_name_self[80];
		
							/* hisself */
							monster_desc(m_name_self, m_ptr, 0x23);

							msg_format("The attack of %s has wounded %s!", m_name, m_name_self);
#endif
							project(0, 0, m_ptr->fy, m_ptr->fx, get_damage, GF_MISSILE, PROJECT_KILL, -1);
							set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);
						}
					}
					break;
				}

				/* Something is wrong */
				else return FALSE;
			}
			break;
		}

		/* RF6_TELE_TO */
		case 160+8:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
msg_format("%^s�����ʤ�������ᤷ����", m_name);
#else
			msg_format("%^s commands you to return.", m_name);
#endif

			teleport_player_to(m_ptr->fy, m_ptr->fx, TRUE);
			learn_spell(MS_TELE_TO);
			break;
		}

		/* RF6_TELE_AWAY */
		case 160+9:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
msg_format("%^s�˥ƥ�ݡ��Ȥ�����줿��", m_name);
			if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				msg_print("���ä���");
#else
			msg_format("%^s teleports you away.", m_name);
#endif

			learn_spell(MS_TELE_AWAY);
			teleport_player(100);
			break;
		}

		/* RF6_TELE_LEVEL */
		case 160+10:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������̯�ʸ��դ�Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles strangely.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����ʤ���­��ؤ�������", m_name);
#else
			else msg_format("%^s gestures at your feet.", m_name);
#endif

			if (p_ptr->resist_nexus)
			{
#ifdef JP
msg_print("���������̤��ʤ��ä���");
#else
				msg_print("You are unaffected!");
#endif

			}
			else if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������Ϥ�ķ���֤�����");
#else
				msg_print("You resist the effects!");
#endif

			}
			else
			{
				teleport_player_level();
			}
			learn_spell(MS_TELE_LEVEL);
			update_smart_learn(m_idx, DRS_NEXUS);
			break;
		}

		/* RF6_PSY_SPEAR */
		case 160+11:
		{
			if (x!=px || y!=py) return (FALSE);
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�����η������ä���", m_name);
#else
			else msg_format("%^s throw a Psycho-Spear.", m_name);
#endif

			dam = (r_ptr->flags2 & RF2_POWERFUL) ? (randint1(rlev * 2) + 150) : (randint1(rlev * 3 / 2) + 100);
			beam(m_idx, GF_PSY_SPEAR, dam, MS_PSY_SPEAR, learnable);
			break;
		}

		/* RF6_DARKNESS */
		case 160+12:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else if (p_ptr->pclass == CLASS_NINJA) msg_format("%^s���դ�����뤯�Ȥ餷����", m_name);
else msg_format("%^s���ŰǤ���Ǽ�򿶤ä���", m_name);
#else
			else if (p_ptr->pclass == CLASS_NINJA)
				msg_format("%^s cast a spell to light up.", m_name);
			else msg_format("%^s gestures in shadow.", m_name);
#endif

			learn_spell(MS_DARKNESS);
			if (p_ptr->pclass == CLASS_NINJA)
				(void)lite_area(0, 3);
			else
				(void)unlite_area(0, 3);
			break;
		}

		/* RF6_TRAPS */
		case 160+13:
		{
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤�Ƽٰ������Ф����", m_name);
#else
			if (blind) msg_format("%^s mumbles, and then cackles evilly.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ʸ�򾧤��Ƽٰ������Ф����", m_name);
#else
			else msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif

			learn_spell(MS_MAKE_TRAP);
			(void)trap_creation(y, x);
			break;
		}

		/* RF6_FORGET */
		case 160+14:
		{
			if (x!=px || y!=py) return (FALSE);
			if (!direct) break;
			disturb(1, 0);
#ifdef JP
msg_format("%^s�����ʤ��ε�����õ�褦�Ȥ��Ƥ��롣", m_name);
#else
			msg_format("%^s tries to blank your mind.", m_name);
#endif


			if (randint0(100 + rlev/2) < p_ptr->skill_sav)
			{
#ifdef JP
msg_print("���������Ϥ�ķ���֤�����");
#else
				msg_print("You resist the effects!");
#endif

			}
			else if (lose_all_info())
			{
#ifdef JP
msg_print("����������Ƥ��ޤä���");
#else
				msg_print("Your memories fade away.");
#endif

			}
			learn_spell(MS_FORGET);
			break;
		}

		/* RF6_RAISE_DEAD */
		case 160+15:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s���������μ�ʸ�򾧤�����", m_name);
#else
			else msg_format("%^s casts a spell to revive corpses.", m_name);
#endif
			animate_dead(m_idx, m_ptr->fy, m_ptr->fx);
			break;
		}

		/* RF6_SUMMON_KIN */
		case 160+16:
		{
			disturb(1, 0);
			if (m_ptr->r_idx == MON_ROLENTO)
			{
#ifdef JP
				if (blind)
					msg_format("%^s���������̤��ꤲ����", m_name);
				else 
					msg_format("%^s�ϼ����Ƥ�Ф�ޤ�����", m_name);
#else
				if (blind)
					msg_format("%^s spreads something.", m_name);
				else
					msg_format("%^s throws some hand grenades.", m_name);
#endif
			}
			else if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
			{
#ifdef JP
				if (blind)
					msg_format("%^s��������Ĥ֤䤤����", m_name);
				else
					msg_format("%^s�����󥸥��μ�򾤴�������", m_name);
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons guardians of dungeons.", m_name);
#endif
			}
			else
			{
#ifdef JP
				if (blind)
					msg_format("%^s��������Ĥ֤䤤����", m_name);
				else
					msg_format("%^s����ˡ��%s�򾤴�������",
					m_name,
					((r_ptr->flags1) & RF1_UNIQUE ?
					"�겼" : "���"));
#else
				if (blind)
					msg_format("%^s mumbles.", m_name);
				else
					msg_format("%^s magically summons %s %s.",
					m_name, m_poss,
					((r_ptr->flags1) & RF1_UNIQUE ?
					"minions" : "kin"));
#endif
			}

			if(m_ptr->r_idx == MON_ROLENTO)
			{
				int num = 1 + randint1(3);

				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, MON_SHURYUUDAN, mode);
				}
			}
			else if(m_ptr->r_idx == MON_THORONDOR ||
				m_ptr->r_idx == MON_GWAIHIR ||
				m_ptr->r_idx == MON_MENELDOR)
			{
				int num = 4 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_EAGLES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			else if(m_ptr->r_idx == MON_LOUSY)
			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_LOUSE, PM_ALLOW_GROUP);
				}
			}
			else if(m_ptr->r_idx == MON_BULLGATES)
			{
				int num = 2 + randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, 921, mode);
				}
			}
			else if (m_ptr->r_idx == MON_CALDARM)
			{
				int num = randint1(3);
				for (k = 0; k < num; k++)
				{
					count += summon_named_creature(m_idx, y, x, 930, mode);
				}
			}
			else if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
			{
				int num = 2 + randint1(3);

				if (r_info[MON_JORMUNGAND].cur_num < r_info[MON_JORMUNGAND].max_num && one_in_(6))
				{
#ifdef JP
					msg_print("���̤���夬�᤭�Ф�����");
#else
					msg_print("Water blew off from the ground!");
#endif
					fire_ball_hide(GF_WATER_FLOW, 0, 3, 8);
				}

				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			else
			{

				summon_kin_type = r_ptr->d_char; /* Big hack */

				for (k = 0; k < 4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_KIN, PM_ALLOW_GROUP);
				}
			}
#ifdef JP
if (blind && count) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif


			break;
		}

		/* RF6_S_CYBER */
		case 160+17:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s�������С��ǡ����򾤴�������", m_name);
#else
			else msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

#ifdef JP
if (blind && count) msg_print("�Ÿ���­�����᤯��ʹ�����롣");
#else
			if (blind && count) msg_print("You hear heavy steps nearby.");
#endif

			summon_cyber(m_idx, y, x);
			break;
		}

		/* RF6_S_MONSTER */
		case 160+18:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ����֤򾤴�������", m_name);
#else
			else msg_format("%^s magically summons help!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
if (blind && count) msg_print("�������ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		/* RF6_S_MONSTERS */
		case 160+19:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥ�󥹥����򾤴�������", m_name);
#else
			else msg_format("%^s magically summons monsters!", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
#ifdef JP
if (blind && count) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		/* RF6_S_ANT */
		case 160+20:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥ���򾤴�������", m_name);
#else
			else msg_format("%^s magically summons ants.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANT, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		/* RF6_S_SPIDER */
		case 160+21:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥ���򾤴�������", m_name);
#else
			else msg_format("%^s magically summons spiders.", m_name);
#endif

			for (k = 0; k < s_num_6; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_SPIDER, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		/* RF6_S_HOUND */
		case 160+22:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥϥ���ɤ򾤴�������", m_name);
#else
			else msg_format("%^s magically summons hounds.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HOUND, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		/* RF6_S_HYDRA */
		case 160+23:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥҥɥ�򾤴�������", m_name);
#else
			else msg_format("%^s magically summons hydras.", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HYDRA, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear many things appear nearby.");
#endif

			break;
		}

		/* RF6_S_ANGEL */
		case 160+24:
		{
			int num = 1;

			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ��ŷ�Ȥ򾤴�������", m_name);
#else
			else msg_format("%^s magically summons an angel!", m_name);
#endif

			if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band)
			{
				num += r_ptr->level/40;
			}

			for (k = 0; k < num; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, PM_ALLOW_GROUP);
			}

			if (count < 2)
			{
#ifdef JP
if (blind && count) msg_print("�������ֶ�˸��줿�������롣");
#else
				if (blind && count) msg_print("You hear something appear nearby.");
#endif
			}
			else
			{
#ifdef JP
if (blind) msg_print("¿���Τ�Τ��ֶ�˸��줿�������롣");
#else
				if (blind) msg_print("You hear many things appear nearby.");
#endif
			}

			break;
		}

		/* RF6_S_DEMON */
		case 160+25:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�Ǻ��٤ε���鰭��򾤴�������", m_name);
#else
			else msg_format("%^s magically summons a demon from the Courts of Chaos!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DEMON, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�������ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		/* RF6_S_UNDEAD */
		case 160+26:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥ���ǥåɤζ�Ũ�򾤴�������", m_name);
#else
			else msg_format("%^s magically summons an undead adversary!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_UNDEAD, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�������ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		/* RF6_S_DRAGON */
		case 160+27:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�ǥɥ饴��򾤴�������", m_name);
#else
			else msg_format("%^s magically summons a dragon!", m_name);
#endif

			for (k = 0; k < 1; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_DRAGON, PM_ALLOW_GROUP);
			}
#ifdef JP
if (blind && count) msg_print("�������ֶ�˸��줿�������롣");
#else
			if (blind && count) msg_print("You hear something appear nearby.");
#endif

			break;
		}

		/* RF6_S_HI_UNDEAD */
		case 160+28:
		{
			disturb(1, 0);

			if (((m_ptr->r_idx == MON_MORGOTH) || (m_ptr->r_idx == MON_SAURON) || (m_ptr->r_idx == MON_ANGMAR)) && ((r_info[MON_NAZGUL].cur_num+2) < r_info[MON_NAZGUL].max_num))
			{
				int cy = y;
				int cx = x;

#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ��ͩ������򾤴�������", m_name);
#else
				else msg_format("%^s magically summons rangers of Nazgul!", m_name);
#endif
				msg_print(NULL);

				for (k = 0; k < 30; k++)
				{
					if (!summon_possible(cy, cx) || !cave_floor_bold(cy, cx))
					{
						int j;
						for (j = 100; j > 0; j--)
						{
							scatter(&cy, &cx, y, x, 2, 0);
							if (cave_floor_bold(cy, cx)) break;
						}
						if (!j) break;
					}
					if (!cave_floor_bold(cy, cx)) continue;

					if (summon_named_creature(m_idx, cy, cx, MON_NAZGUL, mode))
					{
						y = cy;
						x = cx;
						count++;
						if (count == 1)
#ifdef JP
msg_format("��ͩ������%d�桢�ʥ����롦�֥�å�����", count);
#else
							msg_format("A Nazgul says 'Nazgul-Rangers Number %d, Nazgul-Black!'",count);
#endif
						else
#ifdef JP
msg_format("��Ʊ����%d�桢�ʥ����롦�֥�å�����", count);
#else
							msg_format("Another one says 'Number %d, Nazgul-Black!'",count);
#endif
						msg_print(NULL);
					}
				}
#ifdef JP
msg_format("��%dɤ����äơ���󥰥�󥸥㡼����", count);
#else
msg_format("They say 'The %d meets! We are the Ring-Ranger!'.", count);
#endif
				msg_print(NULL);
			}
			else
			{
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
				if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�Ƕ��Ϥʥ���ǥåɤ򾤴�������", m_name);
#else
				else msg_format("%^s magically summons greater undead!", m_name);
#endif

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			if (blind && count)
			{
#ifdef JP
msg_print("�ֶ�ǲ���¿���Τ�Τ��礤��벻��ʹ�����롣");
#else
				msg_print("You hear many creepy things appear nearby.");
#endif

			}
			break;
		}

		/* RF6_S_HI_DRAGON */
		case 160+29:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�Ǹ���ɥ饴��򾤴�������", m_name);
#else
			else msg_format("%^s magically summons ancient dragons!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("¿�����϶�����Τ��ֶ�˸��줿����ʹ�����롣");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			break;
		}

		/* RF6_S_AMBERITES */
		case 160+30:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s������С��β�²�򾤴�������", m_name);
#else
			else msg_format("%^s magically summons Lords of Amber!", m_name);
#endif



			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (blind && count)
			{
#ifdef JP
msg_print("�Ի�μԤ��᤯�˸����Τ�ʹ��������");
#else
				msg_print("You hear immortal beings appear nearby.");
#endif

			}
			break;
		}

		/* RF6_S_UNIQUE */
		case 160+31:
		{
			disturb(1, 0);
#ifdef JP
if (blind) msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
			if (blind) msg_format("%^s mumbles.", m_name);
#endif

#ifdef JP
else msg_format("%^s����ˡ�����̤ʶ�Ũ�򾤴�������", m_name);
#else
			else msg_format("%^s magically summons special opponents!", m_name);
#endif

			for (k = 0; k < s_num_4; k++)
			{
				count += summon_specific(m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
			}
			if (r_ptr->flags3 & RF3_GOOD)
			{
				for (k = count; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			else
			{
				for (k = count; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE));
				}
			}
			if (blind && count)
			{
#ifdef JP
msg_print("¿�����϶�����Τ��ֶ�˸��줿����ʹ�����롣");
#else
				msg_print("You hear many powerful things appear nearby.");
#endif

			}
			break;
		}
	}

	if ((p_ptr->action == ACTION_LEARN) && thrown_spell > 175)
	{
		learn_spell(thrown_spell - 96);
	}

	if (seen && maneable && !world_monster && (p_ptr->pclass == CLASS_IMITATOR))
	{
		if (thrown_spell != 167)
		{
			if (p_ptr->mane_num == MAX_MANE)
			{
				int i;
				p_ptr->mane_num--;
				for (i = 0;i < p_ptr->mane_num;i++)
				{
					p_ptr->mane_spell[i] = p_ptr->mane_spell[i+1];
					p_ptr->mane_dam[i] = p_ptr->mane_dam[i+1];
				}
			}
			p_ptr->mane_spell[p_ptr->mane_num] = thrown_spell - 96;
			p_ptr->mane_dam[p_ptr->mane_num] = dam;
			p_ptr->mane_num++;
			new_mane = TRUE;

			p_ptr->redraw |= (PR_MANE);
		}
	}

	/* Remember what the monster did to us */
	if (seen)
	{
		/* Inate spell */
		if (thrown_spell < 32 * 4)
		{
			r_ptr->r_flags4 |= (1L << (thrown_spell - 32 * 3));
			if (r_ptr->r_cast_inate < MAX_UCHAR) r_ptr->r_cast_inate++;
		}

		/* Bolt or Ball */
		else if (thrown_spell < 32 * 5)
		{
			r_ptr->r_flags5 |= (1L << (thrown_spell - 32 * 4));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}

		/* Special spell */
		else if (thrown_spell < 32 * 6)
		{
			r_ptr->r_flags6 |= (1L << (thrown_spell - 32 * 5));
			if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
		}
	}


	/* Always take note of monsters that kill you */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++;
	}

	/* A spell was cast */
	return (TRUE);
}
