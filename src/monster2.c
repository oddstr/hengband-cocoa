/* File: monster2.c */

/* Purpose: misc code for monsters */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#define HORDE_NOGOOD 0x01
#define HORDE_NOEVIL 0x02

bool is_kage = FALSE;

cptr horror_desc[MAX_SAN_HORROR] =
{
#ifdef JP
	"���ޤ路��",
	"���Τ��",
	"���äȤ���",
	"����Ū��",
	"����Ū��",

	"�����",
	"������",
	"�Է��",
	"�ƼϤΤʤ�",
	"�����ޤ���",

	"�Ϲ���",
	"�Ȥ��Ӥ�����",
	"�Ϲ���",
	"���ޤ路��",
	"��̴�Τ褦��",

	"�����򴶤���",
	"ȳ�������",
	"����",
	"�Ծ���",
	"�����⤪���ޤ���",
#else
	"abominable",
	"abysmal",
	"appalling",
	"baleful",
	"blasphemous",

	"disgusting",
	"dreadful",
	"filthy",
	"grisly",
	"hideous",

	"hellish",
	"horrible",
	"infernal",
	"loathsome",
	"nightmarish",

	"repulsive",
	"sacrilegious",
	"terrible",
	"unclean",
	"unspeakable",
#endif

};

cptr funny_desc[MAX_SAN_FUNNY] =
{
#ifdef JP
	"��ȴ����",
	"��Τ�",
	"�Ф��餷��",
	"̵̣�����",
	"�ϼ�����",

	"�Ф���",
	"�Ф��Ф�����",
	"�֤äȤ��",
	"��������",
	"�ݥ��ȥ�����",

	"�ե��󥿥��ƥ��å���",
	"����������Ū��",
	"����ӥ���Ū��",
	"����Ū��",
	"��ۤ���",

	"������ǽ��",
	"��Τ�����",
	"�ä��٤�",
	"�������ʤ�",
	"�����ƥ��å���",

	"����Ū��",
	"��Ｑ��",
#else
	"silly",
	"hilarious",
	"absurd",
	"insipid",
	"ridiculous",

	"laughable",
	"ludicrous",
	"far-out",
	"groovy",
	"postmodern",

	"fantastic",
	"dadaistic",
	"cubistic",
	"cosmic",
	"awesome",

	"incomprehensible",
	"fabulous",
	"amazing",
	"incredible",
	"chaotic",

	"wild",
	"preposterous",
#endif

};

cptr funny_comments[MAX_SAN_COMMENT] =
{
#ifdef JP
  /* nuke me */
	"�ǹ������",
	"���Ҥ硼��",
	"����������",
	"����Ф餷����",
	"�֤äȤӡ���"
#else
	"Wow, cosmic, man!",
	"Rad!",
	"Groovy!",
	"Cool!",
	"Far out!"
#endif

};


/*
 * Set the target of counter attack
 */
void set_target(monster_type *m_ptr, int y, int x)
{
	m_ptr->target_y = y;
	m_ptr->target_x = x;
}

/*
 * Reset the target of counter attack
 */
void reset_target(monster_type *m_ptr)
{
	set_target(m_ptr, 0, 0);
}

/*
 * Delete a monster by index.
 *
 * When a monster is deleted, all of its objects are deleted.
 */
void delete_monster_idx(int i)
{
	int x, y;

	monster_type *m_ptr = &m_list[i];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	s16b this_o_idx, next_o_idx = 0;


	/* Get location */
	y = m_ptr->fy;
	x = m_ptr->fx;


	/* Hack -- Reduce the racial counter */
	if (m_ptr->mflag2 & MFLAG_CHAMELEON)
	{
		if (r_ptr->flags1 & RF1_UNIQUE)
			r_info[MON_CHAMELEON_K].cur_num--;
		else
			r_info[MON_CHAMELEON].cur_num--;
	}
	else
	{
		r_ptr->cur_num--;
	}

	/* Hack -- count the number of "reproducers" */
	if (r_ptr->flags2 & (RF2_MULTIPLY)) num_repro--;


	/* Hack -- remove target monster */
	if (i == target_who) target_who = 0;

	/* Hack -- remove tracked monster */
	if (i == p_ptr->health_who) health_track(0);

	if (pet_t_m_idx == i ) pet_t_m_idx = 0;
	if (riding_t_m_idx == i) riding_t_m_idx = 0;
	if (p_ptr->riding == i) p_ptr->riding = 0;

	/* Monster is gone */
	cave[y][x].m_idx = 0;


	/* Delete objects */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- efficiency */
		o_ptr->held_m_idx = 0;

		/* Delete the object */
		delete_object_idx(this_o_idx);
	}


	/* Wipe the Monster */
	(void)WIPE(m_ptr, monster_type);

	/* Count monsters */
	m_cnt--;

	/* Visual update */
	lite_spot(y, x);

	/* Update some things */
	p_ptr->update |= (PU_MON_LITE);
}


/*
 * Delete the monster, if any, at a given location
 */
void delete_monster(int y, int x)
{
	cave_type *c_ptr;

	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Check the grid */
	c_ptr = &cave[y][x];

	/* Delete the monster (if any) */
	if (c_ptr->m_idx) delete_monster_idx(c_ptr->m_idx);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
static void compact_monsters_aux(int i1, int i2)
{
	int y, x;

	cave_type *c_ptr;

	monster_type *m_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Do nothing */
	if (i1 == i2) return;


	/* Old monster */
	m_ptr = &m_list[i1];

	/* Location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Cave grid */
	c_ptr = &cave[y][x];

	/* Update the cave */
	c_ptr->m_idx = i2;

	/* Repair objects being carried by monster */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Reset monster pointer */
		o_ptr->held_m_idx = i2;
	}

	/* Hack -- Update the target */
	if (target_who == i1) target_who = i2;

	/* Hack -- Update the target */
	if (pet_t_m_idx == i1) pet_t_m_idx = i2;
	if (riding_t_m_idx == i1) riding_t_m_idx = i2;

	/* Hack -- Update the riding */
	if (p_ptr->riding == i1) p_ptr->riding = i2;

	/* Hack -- Update the health bar */
	if (p_ptr->health_who == i1) health_track(i2);

	/* Structure copy */
	COPY(&m_list[i2], &m_list[i1], monster_type);

	/* Wipe the hole */
	(void)WIPE(&m_list[i1], monster_type);

}


/*
 * Compact and Reorder the monster list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" monsters, we base the saving throw
 * on a combination of monster level, distance from player, and
 * current "desperation".
 *
 * After "compacting" (if needed), we "reorder" the monsters into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
void compact_monsters(int size)
{
	int		i, num, cnt;
	int		cur_lev, cur_dis, chance;

	/* Message (only if compacting) */
#ifdef JP
	if (size) msg_print("��󥹥�������򰵽̤��Ƥ��ޤ�...");
#else
	if (size) msg_print("Compacting monsters...");
#endif


	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Check all the monsters */
		for (i = 1; i < m_max; i++)
		{
			monster_type *m_ptr = &m_list[i];

			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Paranoia -- skip "dead" monsters */
			if (!m_ptr->r_idx) continue;

			/* Hack -- High level monsters start out "immune" */
			if (r_ptr->level > cur_lev) continue;

			if (i == p_ptr->riding) continue;

			/* Ignore nearby monsters */
			if ((cur_dis > 0) && (m_ptr->cdis < cur_dis)) continue;

			/* Saving throw chance */
			chance = 90;

			/* Only compact "Quest" Monsters in emergencies */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) && (cnt < 1000)) chance = 100;

			/* Try not to compact Unique Monsters */
			if (r_ptr->flags1 & (RF1_UNIQUE)) chance = 100;

			/* All monsters get a saving throw */
			if (randint0(100) < chance) continue;

			/* Delete the monster */
			delete_monster_idx(i);

			/* Count the monster */
			num++;
		}
	}


	/* Excise dead monsters (backwards!) */
	for (i = m_max - 1; i >= 1; i--)
	{
		/* Get the i'th monster */
		monster_type *m_ptr = &m_list[i];

		/* Skip real monsters */
		if (m_ptr->r_idx) continue;

		/* Move last monster into open hole */
		compact_monsters_aux(m_max - 1, i);

		/* Compress "m_max" */
		m_max--;
	}
}


/*
 * Delete/Remove all the monsters when the player leaves the level
 *
 * This is an efficient method of simulating multiple calls to the
 * "delete_monster()" function, with no visual effects.
 */
void wipe_m_list(void)
{
	int i;

	/* Hack -- if Banor or Lupart dies, stay another dead */
	if (!r_info[MON_BANORLUPART].max_num)
	{
		if (r_info[MON_BANOR].max_num)
		{
			r_info[MON_BANOR].max_num = 0;
			r_info[MON_BANOR].r_pkills++;
			if (r_info[MON_BANOR].r_tkills < MAX_SHORT) r_info[MON_BANOR].r_tkills++;
		}
		if (r_info[MON_LUPART].max_num)
		{
			r_info[MON_LUPART].max_num = 0;
			r_info[MON_LUPART].r_pkills++;
			if (r_info[MON_LUPART].r_tkills < MAX_SHORT) r_info[MON_LUPART].r_tkills++;
		}
	}

	/* Delete all the monsters */
	for (i = m_max - 1; i >= 1; i--)
	{
		monster_type *m_ptr = &m_list[i];

		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Mega-Hack -- preserve Unique's XXX XXX XXX */

		/* Hack -- Reduce the racial counter */
		if (m_ptr->mflag2 & MFLAG_CHAMELEON)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				r_info[MON_CHAMELEON_K].cur_num = 0;
			else
				r_info[MON_CHAMELEON].cur_num = 0;
		}
		else
			r_ptr->cur_num = 0;

		/* Monster is gone */
		cave[m_ptr->fy][m_ptr->fx].m_idx = 0;

		/* Wipe the Monster */
		(void)WIPE(m_ptr, monster_type);

	}

	/* Reset "m_max" */
	m_max = 1;

	/* Reset "m_cnt" */
	m_cnt = 0;

	/* Hack -- reset "reproducer" count */
	num_repro = 0;

	/* Hack -- no more target */
	target_who = 0;
	pet_t_m_idx = 0;
	riding_t_m_idx = 0;

	/* Hack -- no more tracking */
	health_track(0);
}


/*
 * Acquires and returns the index of a "free" monster.
 *
 * This routine should almost never fail, but it *can* happen.
 */
s16b m_pop(void)
{
	int i;


	/* Normal allocation */
	if (m_max < max_m_idx)
	{
		/* Access the next hole */
		i = m_max;

		/* Expand the array */
		m_max++;

		/* Count monsters */
		m_cnt++;

		/* Return the index */
		return (i);
	}


	/* Recycle dead monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr;

		/* Acquire monster */
		m_ptr = &m_list[i];

		/* Skip live monsters */
		if (m_ptr->r_idx) continue;

		/* Count monsters */
		m_cnt++;

		/* Use this monster */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
#ifdef JP
	if (character_dungeon) msg_print("��󥹥�����¿�����롪");
#else
	if (character_dungeon) msg_print("Too many monsters!");
#endif


	/* Try not to crash */
	return (0);
}




/*
 * Hack -- the "type" of the current "summon specific"
 */
static int summon_specific_type = 0;


/*
 * Hack -- the index of the summoning monster
 */
static int summon_specific_who = -1;


/*
 * Hack -- the hostility of the summoned monster
 */
static bool summon_specific_hostile = TRUE;

static bool summon_unique_okay = FALSE;


static bool summon_specific_aux(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	int okay = FALSE;

	/* Check our requirements */
	switch (summon_specific_type)
	{
		case SUMMON_ANT:
		{
			okay = (r_ptr->d_char == 'a');
			break;
		}

		case SUMMON_SPIDER:
		{
			okay = (r_ptr->d_char == 'S');
			break;
		}

		case SUMMON_HOUND:
		{
			okay = ((r_ptr->d_char == 'C') || (r_ptr->d_char == 'Z'));
			break;
		}

		case SUMMON_HYDRA:
		{
			okay = (r_ptr->d_char == 'M');
			break;
		}

		case SUMMON_ANGEL:
		{
			okay = (r_ptr->d_char == 'A' && ((r_ptr->flags3 & RF3_EVIL) || (r_ptr->flags3 & RF3_GOOD)));
			break;
		}

		case SUMMON_DEMON:
		{
			okay = (r_ptr->flags3 & RF3_DEMON);
			break;
		}

		case SUMMON_UNDEAD:
		{
			okay = (r_ptr->flags3 & RF3_UNDEAD);
			break;
		}

		case SUMMON_DRAGON:
		{
			okay = (r_ptr->flags3 & RF3_DRAGON);
			break;
		}

		case SUMMON_HI_UNDEAD:
		{
			okay = ((r_ptr->d_char == 'L') ||
				(r_ptr->d_char == 'V') ||
				(r_ptr->d_char == 'W'));
			break;
		}

		case SUMMON_HI_DRAGON:
		{
			okay = (r_ptr->d_char == 'D');
			break;
		}

		case SUMMON_HI_DEMON:
		{
			okay = (((r_ptr->d_char == 'U') ||
				 (r_ptr->d_char == 'H') ||
				 (r_ptr->d_char == 'B')) &&
				(r_ptr->flags3 & RF3_DEMON)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_AMBERITES:
		{
			okay = (r_ptr->flags3 & (RF3_AMBERITE)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_UNIQUE:
		{
			okay = (r_ptr->flags1 & (RF1_UNIQUE)) ? TRUE : FALSE;
			break;
		}

		case SUMMON_BIZARRE1:
		{
			okay = (r_ptr->d_char == 'm');
			break;
		}
		case SUMMON_BIZARRE2:
		{
			okay = (r_ptr->d_char == 'b');
			break;
		}
		case SUMMON_BIZARRE3:
		{
			okay = (r_ptr->d_char == 'Q');
			break;
		}

		case SUMMON_BIZARRE4:
		{
			okay = (r_ptr->d_char == 'v');
			break;
		}

		case SUMMON_BIZARRE5:
		{
			okay = (r_ptr->d_char == '$');
			break;
		}

		case SUMMON_BIZARRE6:
		{
			okay = ((r_ptr->d_char == '!') ||
				 (r_ptr->d_char == '?') ||
				 (r_ptr->d_char == '=') ||
				 (r_ptr->d_char == '$') ||
				 (r_ptr->d_char == '|'));
			break;
		}

		case SUMMON_GOLEM:
		{
			okay = (r_ptr->d_char == 'g');
			break;
		}

		case SUMMON_CYBER:
		{
			okay = ((r_ptr->d_char == 'U') &&
				(r_ptr->flags4 & RF4_ROCKET));
			break;
		}


		case SUMMON_KIN:
		{
			okay = ((r_ptr->d_char == summon_kin_type) && (r_idx != MON_HAGURE));
			break;
		}

		case SUMMON_DAWN:
		{
			okay = (r_idx == MON_DAWN);
			break;
		}

		case SUMMON_ANIMAL:
		{
			okay = (r_ptr->flags3 & (RF3_ANIMAL));
			break;
		}

		case SUMMON_ANIMAL_RANGER:
		{
			okay = ((r_ptr->flags3 & (RF3_ANIMAL)) &&
			       (strchr("abcflqrwBCHIJKMRS", r_ptr->d_char)) &&
			       !(r_ptr->flags3 & (RF3_DRAGON)) &&
			       !(r_ptr->flags3 & (RF3_EVIL)) &&
			       !(r_ptr->flags3 & (RF3_UNDEAD)) &&
			       !(r_ptr->flags3 & (RF3_DEMON)) &&
			       !(r_ptr->flags2 & (RF2_MULTIPLY)) &&
			       !(r_ptr->flags4 || r_ptr->flags5 || r_ptr->flags6));
			break;
		}

		case SUMMON_HI_DRAGON_LIVING:
		{
			okay = ((r_ptr->d_char == 'D') &&
			       !(r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING)));
			break;
		}

		case SUMMON_LIVING:
		{
			okay = (!(r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING)));
			break;
		}

		case SUMMON_PHANTOM:
		{
			okay = (r_idx == MON_PHANTOM_B || r_idx == MON_PHANTOM_W);
			break;
		}

		case SUMMON_BLUE_HORROR:
		{
			okay = (r_idx == MON_BLUE_HORROR);
			break;
		}

		case SUMMON_ELEMENTAL:
		{
			okay = (r_ptr->d_char == 'E');
			break;
		}

		case SUMMON_VORTEX:
		{
			okay = (r_ptr->d_char == 'v');
			break;
		}

		case SUMMON_HYBRID:
		{
			okay = (r_ptr->d_char == 'H');
			break;
		}

		case SUMMON_BIRD:
		{
			okay = (r_ptr->d_char == 'B');
			break;
		}

		case SUMMON_KAMIKAZE:
		{
			int i;
			for (i = 0; i < 4; i++)
				if (r_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			break;
		}

		case SUMMON_KAMIKAZE_LIVING:
		{
			int i;

			for (i = 0; i < 4; i++)
				if (r_ptr->blow[i].method == RBM_EXPLODE) okay = TRUE;
			okay = (okay && (!(r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING))));
			break;
		}

		case SUMMON_MANES:
		{
			okay = (r_idx == MON_MANES);
			break;
		}

		case SUMMON_LOUSE:
		{
			okay = (r_idx == MON_LOUSE);
			break;
		}

		case SUMMON_GUARDIANS:
		{
			okay = (r_ptr->flags7 & RF7_GUARDIAN);
			break;
		}

		case SUMMON_KNIGHTS:
		{
			okay = ((r_idx == MON_NOV_PALADIN) ||
				(r_idx == MON_NOV_PALADIN_G) ||
				(r_idx == MON_PALADIN) ||
				(r_idx == MON_W_KNIGHT) ||
				(r_idx == MON_ULTRA_PALADIN) ||
				(r_idx == MON_KNI_TEMPLAR));
			break;
		}
		case SUMMON_EAGLES:
		{
			okay = (r_ptr->d_char == 'B' &&
				(r_ptr->flags8 & RF8_WILD_MOUNTAIN) &&
				(r_ptr->flags8 & RF8_WILD_ONLY));
			break;
		}
		case SUMMON_PIRANHAS:
		{
			okay = (r_idx == MON_PIRANHA);
			break;
		}
	}

	/* Result */
	/* Since okay is int, "return (okay);" is not correct. */
	return (bool)(okay ? TRUE : FALSE);
}


static int chameleon_change_m_idx = 0;


/*
 * Some dungeon types restrict the possible monsters.
 * Return TRUE is the monster is OK and FALSE otherwise
 */
static bool restrict_monster_to_dungeon(int r_idx)
{
	dungeon_info_type *d_ptr = &d_info[dungeon_type];
	monster_race *r_ptr = &r_info[r_idx];
	byte a;

	if (d_ptr->flags1 & DF1_CHAMELEON)
	{
		if (chameleon_change_m_idx) return TRUE;
	}
	if (d_ptr->flags1 & DF1_NO_MAGIC)
	{
		if (r_idx != MON_CHAMELEON &&
		    r_ptr->freq_spell && 
		    !(r_ptr->flags4 & RF4_NOMAGIC_MASK) &&
		    !(r_ptr->flags5 & RF5_NOMAGIC_MASK) &&
		    !(r_ptr->flags6 & RF6_NOMAGIC_MASK))
			return FALSE;
	}
	if (d_ptr->flags1 & DF1_NO_MELEE)
	{
		if (r_idx == MON_CHAMELEON) return TRUE;
		if (!(r_ptr->flags4 & (RF4_BOLT_MASK | RF4_BEAM_MASK | RF4_BALL_MASK)) &&
		    !(r_ptr->flags5 & (RF5_BOLT_MASK | RF5_BEAM_MASK | RF5_BALL_MASK | RF5_CAUSE_1 | RF5_CAUSE_2 | RF5_CAUSE_3 | RF5_CAUSE_4 | RF5_MIND_BLAST | RF5_BRAIN_SMASH)) &&
		    !(r_ptr->flags6 & (RF6_BOLT_MASK | RF6_BEAM_MASK | RF6_BALL_MASK)))
			return FALSE;
	}
	if (d_ptr->flags1 & DF1_BEGINNER)
	{
		if (r_ptr->level > dun_level)
			return FALSE;
	}

	if (d_ptr->special_div == 64) return TRUE;
	if (summon_specific_type && !(d_ptr->flags1 & DF1_CHAMELEON)) return TRUE;

	if(d_ptr->mode == DUNGEON_MODE_AND)
	{
		if (d_ptr->mflags1)
		{
			if((d_ptr->mflags1 & r_ptr->flags1) != d_ptr->mflags1)
				return FALSE;
		}
		if (d_ptr->mflags2)
		{
			if((d_ptr->mflags2 & r_ptr->flags2) != d_ptr->mflags2)
				return FALSE;
		}
		if (d_ptr->mflags3)
		{
			if((d_ptr->mflags3 & r_ptr->flags3) != d_ptr->mflags3)
				return FALSE;
		}
		if (d_ptr->mflags4)
		{
			if((d_ptr->mflags4 & r_ptr->flags4) != d_ptr->mflags4)
				return FALSE;
		}
		if (d_ptr->mflags5)
		{
			if((d_ptr->mflags5 & r_ptr->flags5) != d_ptr->mflags5)
				return FALSE;
		}
		if (d_ptr->mflags6)
		{
			if((d_ptr->mflags6 & r_ptr->flags6) != d_ptr->mflags6)
				return FALSE;
		}
		if (d_ptr->mflags7)
		{
			if((d_ptr->mflags7 & r_ptr->flags7) != d_ptr->mflags7)
				return FALSE;
		}
		if (d_ptr->mflags8)
		{
			if((d_ptr->mflags8 & r_ptr->flags8) != d_ptr->mflags8)
				return FALSE;
		}
		if (d_ptr->mflags9)
		{
			if((d_ptr->mflags9 & r_ptr->flags9) != d_ptr->mflags9)
				return FALSE;
		}
		for(a = 0; a < 5; a++)
			if(d_ptr->r_char[a] && (d_ptr->r_char[a] != r_ptr->d_char)) return FALSE;
	}
	else if(d_ptr->mode == DUNGEON_MODE_NAND)
	{
		byte ok[9 + 5], i = 0, j = 0;

		if (d_ptr->mflags1)
		{
			i++;
			if(d_ptr->mflags1 & r_ptr->flags1)
				ok[0] = 1;
		}
		if (d_ptr->mflags2)
		{
			i++;
			if(d_ptr->mflags2 & r_ptr->flags2)
				ok[1] = 1;
		}
		if (d_ptr->mflags3)
		{
			i++;
			if(d_ptr->mflags3 & r_ptr->flags3)
				ok[2] = 1;
		}
		if (d_ptr->mflags4)
		{
			i++;
			if(d_ptr->mflags4 & r_ptr->flags4)
				ok[3] = 1;
		}
		if (d_ptr->mflags5)
		{
			i++;
			if(d_ptr->mflags5 & r_ptr->flags5)
				ok[4] = 1;
		}
		if (d_ptr->mflags6)
		{
			i++;
			if(d_ptr->mflags6 & r_ptr->flags6)
				ok[5] = 1;
		}
		if (d_ptr->mflags7)
		{
			i++;
			if(d_ptr->mflags7 & r_ptr->flags7)
				ok[6] = 1;
		}
		if (d_ptr->mflags8)
		{
			i++;
			if(d_ptr->mflags8 & r_ptr->flags8)
				ok[7] = 1;
		}
		if (d_ptr->mflags9)
		{
			i++;
			if(d_ptr->mflags9 & r_ptr->flags9)
				ok[8] = 1;
		}

		for(a = 0; a < 5; a++)
		{
			if(d_ptr->r_char[a])
			{
				i++;
				if (d_ptr->r_char[a] != r_ptr->d_char) ok[9 + a] = 1;
			}
		}

		j = ok[0] + ok[1] + ok[2] + ok[3] + ok[4] + ok[5] + ok[6] + ok[7] + ok[8] + ok[9] + ok[10] + ok[11] + ok[12] + ok[13];

		if(i == j) return FALSE;
	}
	else if(d_ptr->mode == DUNGEON_MODE_OR)
	{
		byte ok = FALSE, i;
		s32b flag;

		for(i = 0; i < 32; i++)
		{
			flag = d_ptr->mflags1 & (1 << i);
			if(r_ptr->flags1 & flag) ok = TRUE;

			flag = d_ptr->mflags2 & (1 << i);
			if(r_ptr->flags2 & flag) ok = TRUE;

			flag = d_ptr->mflags3 & (1 << i);
			if(r_ptr->flags3 & flag) ok = TRUE;

			flag = d_ptr->mflags4 & (1 << i);
			if(r_ptr->flags4 & flag) ok = TRUE;

			flag = d_ptr->mflags5 & (1 << i);
			if(r_ptr->flags5 & flag) ok = TRUE;

			flag = d_ptr->mflags6 & (1 << i);
			if(r_ptr->flags6 & flag) ok = TRUE;

			flag = d_ptr->mflags7 & (1 << i);
			if(r_ptr->flags7 & flag) ok = TRUE;

			flag = d_ptr->mflags8 & (1 << i);
			if(r_ptr->flags8 & flag) ok = TRUE;

			flag = d_ptr->mflags9 & (1 << i);
			if(r_ptr->flags9 & flag) ok = TRUE;
		}
		for(a = 0; a < 5; a++)
			if(d_ptr->r_char[a] == r_ptr->d_char) ok = TRUE;

		return ok;
	}
	else if(d_ptr->mode == DUNGEON_MODE_NOR)
	{
		byte ok = TRUE, i;
		s32b flag;

		for(i = 0; i < 32; i++)
		{
			flag = d_ptr->mflags1 & (1 << i);
			if(r_ptr->flags1 & flag) ok = FALSE;

			flag = d_ptr->mflags2 & (1 << i);
			if(r_ptr->flags2 & flag) ok = FALSE;

			flag = d_ptr->mflags3 & (1 << i);
			if(r_ptr->flags3 & flag) ok = FALSE;

			flag = d_ptr->mflags4 & (1 << i);
			if(r_ptr->flags4 & flag) ok = FALSE;

			flag = d_ptr->mflags5 & (1 << i);
			if(r_ptr->flags5 & flag) ok = FALSE;

			flag = d_ptr->mflags6 & (1 << i);
			if(r_ptr->flags6 & flag) ok = FALSE;

			flag = d_ptr->mflags7 & (1 << i);
			if(r_ptr->flags7 & flag) ok = FALSE;

			flag = d_ptr->mflags8 & (1 << i);
			if(r_ptr->flags8 & flag) ok = FALSE;

			flag = d_ptr->mflags9 & (1 << i);
			if(r_ptr->flags9 & flag) ok = FALSE;
		}
		for(a = 0; a < 5; a++)
			if(d_ptr->r_char[a] == r_ptr->d_char) ok = FALSE;
		return ok;
	}

	return TRUE;
}

/*
 * Apply a "monster restriction function" to the "monster allocation table"
 */
errr get_mon_num_prep(monster_hook_type monster_hook,
					  monster_hook_type monster_hook2)
{
	int i;

	/* Todo: Check the hooks for non-changes */

	/* Set the new hooks */
	get_mon_num_hook = monster_hook;
	get_mon_num2_hook = monster_hook2;

	/* Scan the allocation table */
	for (i = 0; i < alloc_race_size; i++)
	{
		monster_race	*r_ptr;
		
		/* Get the entry */
		alloc_entry *entry = &alloc_race_table[i];

		entry->prob2 = 0;
		r_ptr = &r_info[entry->index];

		/* Skip monsters which don't pass the restriction */
		if ((get_mon_num_hook && !((*get_mon_num_hook)(entry->index))) ||
		    (get_mon_num2_hook && !((*get_mon_num2_hook)(entry->index))))
			continue;

		if (!p_ptr->inside_battle && !chameleon_change_m_idx &&
		    summon_specific_type != SUMMON_GUARDIANS)
		{
			/* Hack -- don't create questors */
			if (r_ptr->flags1 & RF1_QUESTOR)
				continue;

			if (r_ptr->flags7 & RF7_GUARDIAN)
				continue;
		
			/* Depth Monsters never appear out of depth */
			if ((r_ptr->flags1 & (RF1_FORCE_DEPTH)) &&
			    (r_ptr->level > dun_level))
				continue;
		}

		/* Accept this monster */
		entry->prob2 = entry->prob1;

		if (dun_level && (!p_ptr->inside_quest || p_ptr->inside_quest < MIN_RANDOM_QUEST) && !restrict_monster_to_dungeon(entry->index) && !p_ptr->inside_battle)
		{
			int hoge = entry->prob2 * d_info[dungeon_type].special_div;
			entry->prob2 = hoge / 64;
			if (randint0(64) < (hoge & 0x3f)) entry->prob2++;
		}
	}

	/* Success */
	return (0);
}


static int mysqrt(int n)
{
	int tmp = n>>1;
	int tasu = 10;
	int kaeriti = 1;

	if (!tmp)
	{
		if (n) return 1;
		else return 0;
	}

	while(tmp)
	{
		if ((n/tmp) < tmp)
		{
			tmp >>= 1;
		}
		else break;
	}
	kaeriti = tmp;
	while(tasu)
	{
		if ((n/tmp) < tmp)
		{
			tasu--;
			tmp = kaeriti;
		}
		else
		{
			kaeriti = tmp;
			tmp += tasu;
		}
	}
	return kaeriti;
}

/*
 * Choose a monster race that seems "appropriate" to the given level
 *
 * This function uses the "prob2" field of the "monster allocation table",
 * and various local information, to calculate the "prob3" field of the
 * same table, which is then used to choose an "appropriate" monster, in
 * a relatively efficient manner.
 *
 * Note that "town" monsters will *only* be created in the town, and
 * "normal" monsters will *never* be created in the town, unless the
 * "level" is "modified", for example, by polymorph or summoning.
 *
 * There is a small chance (1/50) of "boosting" the given depth by
 * a small amount (up to four levels), except in the town.
 *
 * It is (slightly) more likely to acquire a monster of the given level
 * than one of a lower level.  This is done by choosing several monsters
 * appropriate to the given level and keeping the "hardest" one.
 *
 * Note that if no monsters are "appropriate", then this function will
 * fail, and return zero, but this should *almost* never happen.
 */
s16b get_mon_num(int level)
{
	int			i, j, p;

	int			r_idx;

	long		value, total;

	monster_race	*r_ptr;

	alloc_entry		*table = alloc_race_table;

	int pls_kakuritu, pls_level;
	int hoge=mysqrt(level*10000L);

	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;

	if ((dungeon_turn > hoge*(TURNS_PER_TICK*500L)) && !level)
	{
		pls_kakuritu = MAX(2, NASTY_MON-((dungeon_turn/(TURNS_PER_TICK*2500L)-hoge/10)));
		pls_level = MIN(8,3 + dungeon_turn/(TURNS_PER_TICK*20000L)-hoge/40);
	}
	else
	{
		pls_kakuritu = NASTY_MON;
		pls_level = 2;
	}

	if (d_info[dungeon_type].flags1 & DF1_MAZE)
	{
		pls_kakuritu = MIN(pls_kakuritu/2, pls_kakuritu-10);
		if (pls_kakuritu < 2) pls_kakuritu = 2;
		pls_level += 2;
		level += 3;
	}

	/* Boost the level */
	if ((level > 0) && !p_ptr->inside_battle && !(d_info[dungeon_type].flags1 & DF1_BEGINNER))
	{
		/* Nightmare mode allows more out-of depth monsters */
		if (ironman_nightmare && !randint0(pls_kakuritu))
		{
			/* What a bizarre calculation */
			level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH));
		}
		else
		{
			/* Occasional "nasty" monster */
			if (!randint0(pls_kakuritu))
			{
				/* Pick a level bonus */
				int d = MIN(5, level/10) + pls_level;

				/* Boost the level */
				level += d;
			}

			/* Occasional "nasty" monster */
			if (!randint0(pls_kakuritu))
			{
				/* Pick a level bonus */
				int d = MIN(5, level/10) + pls_level;

				/* Boost the level */
				level += d;
			}
		}
	}


	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Monsters are sorted by depth */
		if (table[i].level > level) break;

		/* Default */
		table[i].prob3 = 0;

		/* Access the "r_idx" of the chosen monster */
		r_idx = table[i].index;

		/* Access the actual race */
		r_ptr = &r_info[r_idx];

		if (!p_ptr->inside_battle && !chameleon_change_m_idx)
		{
			/* Hack -- "unique" monsters must be "unique" */
			if (((r_ptr->flags1 & (RF1_UNIQUE)) ||
			     (r_ptr->flags7 & (RF7_UNIQUE_7))) &&
			    (r_ptr->cur_num >= r_ptr->max_num))
			{
				continue;
			}

			if ((r_ptr->flags7 & (RF7_UNIQUE2)) &&
			    (r_ptr->cur_num >= 1))
			{
				continue;
			}

			if (r_idx == MON_BANORLUPART)
			{
				if (r_info[MON_BANOR].cur_num > 0) continue;
				if (r_info[MON_LUPART].cur_num > 0) continue;
			}
		}

		/* Accept */
		table[i].prob3 = table[i].prob2;

		/* Total */
		total += table[i].prob3;
	}

	/* No legal monsters */
	if (total <= 0) return (0);


	/* Pick a monster */
	value = randint0(total);

	/* Find the monster */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}


	/* Power boost */
	p = randint0(100);

	/* Try for a "harder" monster once (50%) or twice (10%) */
	if (p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = randint0(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Try for a "harder" monster twice (10%) */
	if (p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = randint0(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Result */
	return (table[i].index);
}





/*
 * Build a string describing a monster in some way.
 *
 * We can correctly describe monsters based on their visibility.
 * We can force all monsters to be treated as visible or invisible.
 * We can build nominatives, objectives, possessives, or reflexives.
 * We can selectively pronominalize hidden, visible, or all monsters.
 * We can use definite or indefinite descriptions for hidden monsters.
 * We can use definite or indefinite descriptions for visible monsters.
 *
 * Pronominalization involves the gender whenever possible and allowed,
 * so that by cleverly requesting pronominalization / visibility, you
 * can get messages like "You hit someone.  She screams in agony!".
 *
 * Reflexives are acquired by requesting Objective plus Possessive.
 *
 * If no m_ptr arg is given (?), the monster is assumed to be hidden,
 * unless the "Assume Visible" mode is requested.
 *
 * If no r_ptr arg is given, it is extracted from m_ptr and r_info
 * If neither m_ptr nor r_ptr is given, the monster is assumed to
 * be neuter, singular, and hidden (unless "Assume Visible" is set),
 * in which case you may be in trouble... :-)
 *
 * I am assuming that no monster name is more than 70 characters long,
 * so that "char desc[80];" is sufficiently large for any result.
 *
 * Mode Flags:
 *   0x01 --> Objective (or Reflexive)
 *   0x02 --> Possessive (or Reflexive)
 *   0x04 --> Use indefinites for hidden monsters ("something")
 *   0x08 --> Use indefinites for visible monsters ("a kobold")
 *   0x10 --> Pronominalize hidden monsters
 *   0x20 --> Pronominalize visible monsters
 *   0x40 --> Assume the monster is hidden
 *   0x80 --> Assume the monster is visible
 *  0x100 --> Chameleon's true name
 *  0x200 --> Ignore hallucination, and penetrate shape change
 *
 * Useful Modes:
 *   0x00 --> Full nominative name ("the kobold") or "it"
 *   0x04 --> Full nominative name ("the kobold") or "something"
 *   0x80 --> Genocide resistance name ("the kobold")
 *   0x88 --> Killing name ("a kobold")
 *   0x22 --> Possessive, genderized if visable ("his") or "its"
 *   0x23 --> Reflexive, genderized if visable ("himself") or "itself"
 */
void monster_desc(char *desc, monster_type *m_ptr, int mode)
{
	cptr            res;
	monster_race    *r_ptr;

	cptr            name;
	char            buf[128];
	char            silly_name[1024];
	bool            seen, pron;
	bool            named = FALSE;

	r_ptr = &r_info[m_ptr->ap_r_idx];

	if ((mode & 0x100) && (m_ptr->mflag2 & MFLAG_CHAMELEON))
	{
		if (r_ptr->flags1 & RF1_UNIQUE) name = (r_name + r_info[MON_CHAMELEON_K].name);
		else name = (r_name + r_info[MON_CHAMELEON].name);
	}
	else name = (r_name + r_ptr->name);

	/* Are we hallucinating? (Idea from Nethack...) */
	if (p_ptr->image && !(mode & 0x200))
	{
		if (one_in_(2))
		{
#ifdef JP
if (!get_rnd_line("silly_j.txt", m_ptr->r_idx, silly_name))
#else
			if (!get_rnd_line("silly.txt", m_ptr->r_idx, silly_name))
#endif

				named = TRUE;
		}

		if (!named)
		{
			monster_race *hallu_race;

			do
			{
				hallu_race = &r_info[randint1(max_r_idx - 1)];
			}
			while (hallu_race->flags1 & RF1_UNIQUE);

			strcpy(silly_name, (r_name + hallu_race->name));
		}

		/* Better not strcpy it, or we could corrupt r_info... */
		name = silly_name;
	}

	/* Can we "see" it (exists + forced, or visible + not unforced) */
	seen = (m_ptr && ((mode & 0x80) || (!(mode & 0x40) && m_ptr->ml)));

	/* Sexed Pronouns (seen and allowed, or unseen and allowed) */
	pron = (m_ptr && ((seen && (mode & 0x20)) || (!seen && (mode & 0x10))));


	/* First, try using pronouns, or describing hidden monsters */
	if (!seen || pron)
	{
		/* an encoding of the monster "sex" */
		int kind = 0x00;

		/* Extract the gender (if applicable) */
		if (r_ptr->flags1 & (RF1_FEMALE)) kind = 0x20;
		else if (r_ptr->flags1 & (RF1_MALE)) kind = 0x10;

		/* Ignore the gender (if desired) */
		if (!m_ptr || !pron) kind = 0x00;


		/* Assume simple result */
#ifdef JP
		res = "����";
#else
		res = "it";
#endif


		/* Brute force: split on the possibilities */
		switch (kind + (mode & 0x07))
		{
			/* Neuter, or unknown */
#ifdef JP
			case 0x00: res = "����"; break;
			case 0x01: res = "����"; break;
			case 0x02: res = "������"; break;
			case 0x03: res = "��������"; break;
			case 0x04: res = "����"; break;
			case 0x05: res = "����"; break;
			case 0x06: res = "����"; break;
			case 0x07: res = "���켫��"; break;
#else
			case 0x00: res = "it"; break;
			case 0x01: res = "it"; break;
			case 0x02: res = "its"; break;
			case 0x03: res = "itself"; break;
			case 0x04: res = "something"; break;
			case 0x05: res = "something"; break;
			case 0x06: res = "something's"; break;
			case 0x07: res = "itself"; break;
#endif


			/* Male (assume human if vague) */
#ifdef JP
			case 0x10: res = "��"; break;
			case 0x11: res = "��"; break;
			case 0x12: res = "���"; break;
			case 0x13: res = "�༫��"; break;
			case 0x14: res = "ï��"; break;
			case 0x15: res = "ï��"; break;
			case 0x16: res = "ï����"; break;
			case 0x17: res = "�༫��"; break;
#else
			case 0x10: res = "he"; break;
			case 0x11: res = "him"; break;
			case 0x12: res = "his"; break;
			case 0x13: res = "himself"; break;
			case 0x14: res = "someone"; break;
			case 0x15: res = "someone"; break;
			case 0x16: res = "someone's"; break;
			case 0x17: res = "himself"; break;
#endif


			/* Female (assume human if vague) */
#ifdef JP
			case 0x20: res = "���"; break;
			case 0x21: res = "���"; break;
			case 0x22: res = "�����"; break;
			case 0x23: res = "�������"; break;
			case 0x24: res = "ï��"; break;
			case 0x25: res = "ï��"; break;
			case 0x26: res = "ï����"; break;
			case 0x27: res = "�������"; break;
#else
			case 0x20: res = "she"; break;
			case 0x21: res = "her"; break;
			case 0x22: res = "her"; break;
			case 0x23: res = "herself"; break;
			case 0x24: res = "someone"; break;
			case 0x25: res = "someone"; break;
			case 0x26: res = "someone's"; break;
			case 0x27: res = "herself"; break;
#endif

		}

		/* Copy the result */
		(void)strcpy(desc, res);
	}


	/* Handle visible monsters, "reflexive" request */
	else if ((mode & 0x02) && (mode & 0x01))
	{
		/* The monster is visible, so use its gender */
#ifdef JP
		if (r_ptr->flags1 & (RF1_FEMALE)) strcpy(desc, "�������");
		else if (r_ptr->flags1 & (RF1_MALE)) strcpy(desc, "�༫��");
		else strcpy(desc, "���켫��");
#else
		if (r_ptr->flags1 & RF1_FEMALE) strcpy(desc, "herself");
		else if (r_ptr->flags1 & RF1_MALE) strcpy(desc, "himself");
		else strcpy(desc, "itself");
#endif

	}


	/* Handle all other visible monster requests */
	else
	{
		/* Tanuki? */
		if (is_pet(m_ptr) && m_ptr->ap_r_idx != m_ptr->r_idx)
		{
#ifdef JP
				char *t;
				strcpy(buf, name);
				t = buf;
				while(strncmp(t, "��", 2) && *t) t++;
				if (*t)
				{
					*t = '\0';
					(void)sprintf(desc, "%s����", buf);
				}
				else
					(void)sprintf(desc, "%s��", name);
#else
				(void)sprintf(desc, "%s?", name);
#endif
		}
		else

		/* It could be a Unique */
		if ((r_ptr->flags1 & RF1_UNIQUE) && !(p_ptr->image && !(mode & 0x200)))
		{
			/* Start with the name (thus nominative and objective) */
			if ((m_ptr->mflag2 & MFLAG_CHAMELEON) && !(mode & 0x100))
			{
#ifdef JP
				char *t;
				strcpy(buf, name);
				t = buf;
				while(strncmp(t, "��", 2) && *t) t++;
				if (*t)
				{
					*t = '\0';
					(void)sprintf(desc, "%s����", buf);
				}
				else
					(void)sprintf(desc, "%s��", name);
#else
				(void)sprintf(desc, "%s?", name);
#endif
			}
			else if ((cave[m_ptr->fy][m_ptr->fx].m_idx == p_ptr->riding) || !p_ptr->inside_battle)
				(void)strcpy(desc, name);
			else
#ifdef JP
				(void)sprintf(desc, "%s��ɤ�", name);
#else
				(void)sprintf(desc, "fake %s", name);
#endif
		}

		/* It could be an indefinite monster */
		else if (mode & 0x08)
		{
			/* XXX Check plurality for "some" */

			/* Indefinite monsters need an indefinite article */
#ifdef JP
			(void)strcpy(desc, "");
#else
			(void)strcpy(desc, is_a_vowel(name[0]) ? "an " : "a ");
#endif

			(void)strcat(desc, name);
		}

		/* It could be a normal, definite, monster */
		else
		{
			/* Definite monsters need a definite article */
			if (is_pet(m_ptr))
#ifdef JP
				(void)strcpy(desc, "���ʤ���");
#else
				(void)strcpy(desc, "your ");
#endif

			else
#ifdef JP
				(void)strcpy(desc, "");
#else
				(void)strcpy(desc, "the ");
#endif

			(void)strcat(desc, name);
		}

		if (m_ptr->nickname)
		{
#ifdef JP
			sprintf(buf,"��%s��",quark_str(m_ptr->nickname));
#else
			sprintf(buf," called %s",quark_str(m_ptr->nickname));
#endif
			strcat(desc,buf);
		}

		if ((m_ptr->fy == py) && (m_ptr->fx == px))
		{
#ifdef JP
			strcat(desc,"(������)");
#else
			strcat(desc,"(riding)");
#endif
		}

		if ((mode & 0x200) && (m_ptr->mflag2 & MFLAG_CHAMELEON))
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
#ifdef JP
				strcat(desc,"(����쥪��β�)");
#else
				strcat(desc,"(Chameleon Lord)");
#endif
			}
			else
			{
#ifdef JP
				strcat(desc,"(����쥪��)");
#else
				strcat(desc,"(Chameleon)");
#endif
			}
		}

		if ((mode & 0x200) && m_ptr->ap_r_idx != m_ptr->r_idx)
		{
			strcat(desc, format("(%s)", r_name + r_info[m_ptr->r_idx].name));
		}

		/* Handle the Possessive as a special afterthought */
		if (mode & 0x02)
		{
			/* XXX Check for trailing "s" */
			
			/* Simply append "apostrophe" and "s" */
#ifdef JP
			(void)strcat(desc, "��");
#else
			(void)strcat(desc, "'s");
#endif

		}
	}
}




/*
 * Learn about a monster (by "probing" it)
 */
void lore_do_probe(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Hack -- Memorize some flags */
	r_ptr->r_flags1 = r_ptr->flags1;
	r_ptr->r_flags2 = r_ptr->flags2;
	r_ptr->r_flags3 = r_ptr->flags3;

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}
}


/*
 * Take note that the given monster just dropped some treasure
 *
 * Note that learning the "GOOD"/"GREAT" flags gives information
 * about the treasure (even when the monster is killed for the first
 * time, such as uniques, and the treasure has not been examined yet).
 *
 * This "indirect" method is used to prevent the player from learning
 * exactly how much treasure a monster can drop from observing only
 * a single example of a drop.  This method actually observes how much
 * gold and items are dropped, and remembers that information to be
 * described later by the monster recall code.
 */
void lore_treasure(int m_idx, int num_item, int num_gold)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Note the number of things dropped */
	if (num_item > r_ptr->r_drop_item) r_ptr->r_drop_item = num_item;
	if (num_gold > r_ptr->r_drop_gold) r_ptr->r_drop_gold = num_gold;

	/* Hack -- memorize the good/great flags */
	if (r_ptr->flags1 & (RF1_DROP_GOOD)) r_ptr->r_flags1 |= (RF1_DROP_GOOD);
	if (r_ptr->flags1 & (RF1_DROP_GREAT)) r_ptr->r_flags1 |= (RF1_DROP_GREAT);

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}
}



void sanity_blast(monster_type *m_ptr, bool necro)
{
	bool happened = FALSE;
	int power = 100;

	if (p_ptr->inside_battle || !character_dungeon) return;

	if (!necro)
	{
		char            m_name[80];
		monster_race    *r_ptr = &r_info[m_ptr->r_idx];

		power = r_ptr->level / 2;

		monster_desc(m_name, m_ptr, 0);

		if (!(r_ptr->flags1 & RF1_UNIQUE))
		{
			if (r_ptr->flags1 & RF1_FRIENDS)
			power /= 2;
		}
		else power *= 2;

		if (!hack_mind)
			return; /* No effect yet, just loaded... */

		if (!m_ptr->ml)
			return; /* Cannot see it for some reason */

		if (!(r_ptr->flags2 & RF2_ELDRITCH_HORROR))
			return; /* oops */



		if (is_pet(m_ptr))
			return; /* Pet eldritch horrors are safe most of the time */

		if (randint1(100) > power) return;

		if (saving_throw(p_ptr->skill_sav - power))
		{
			return; /* Save, no adverse effects */
		}

		if (p_ptr->image)
		{
			/* Something silly happens... */
#ifdef JP
msg_format("%s%s�δ�򸫤Ƥ��ޤä���",
#else
			msg_format("You behold the %s visage of %s!",
#endif

				funny_desc[randint0(MAX_SAN_FUNNY)], m_name);

			if (one_in_(3))
			{
				msg_print(funny_comments[randint0(MAX_SAN_COMMENT)]);
				p_ptr->image = p_ptr->image + randint1(r_ptr->level);
			}

			return; /* Never mind; we can't see it clearly enough */
		}

		/* Something frightening happens... */
#ifdef JP
msg_format("%s%s�δ�򸫤Ƥ��ޤä���",
#else
		msg_format("You behold the %s visage of %s!",
#endif

			horror_desc[randint0(MAX_SAN_HORROR)], m_name);

		r_ptr->r_flags2 |= RF2_ELDRITCH_HORROR;

		/* Demon characters are unaffected */
		if ((p_ptr->prace == RACE_IMP) || (p_ptr->prace == RACE_DEMON) || (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_DEMON)) return;
		if (p_ptr->wizard) return;

		/* Undead characters are 50% likely to be unaffected */
		if ((p_ptr->prace == RACE_SKELETON) || (p_ptr->prace == RACE_ZOMBIE)
			|| (p_ptr->prace == RACE_VAMPIRE) || (p_ptr->prace == RACE_SPECTRE) ||
		    (mimic_info[p_ptr->mimic_form].MIMIC_FLAGS & MIMIC_IS_UNDEAD))
		{
			if (saving_throw(25 + p_ptr->lev)) return;
		}
	}
	else
	{
#ifdef JP
msg_print("�ͥ���Υߥ�����ɤ�������򼺤ä���");
#else
		msg_print("Your sanity is shaken by reading the Necronomicon!");
#endif

	}

	if (!saving_throw(p_ptr->skill_sav - power)) /* Mind blast */
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->resist_chaos && one_in_(3))
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	if (!saving_throw(p_ptr->skill_sav - power)) /* Lose int & wis */
	{
		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
		return;
	}

	if (!saving_throw(p_ptr->skill_sav - power)) /* Brain smash */
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
		}
		while (randint0(100) > p_ptr->skill_sav)
			(void)do_dec_stat(A_INT);
		while (randint0(100) > p_ptr->skill_sav)
			(void)do_dec_stat(A_WIS);
		if (!p_ptr->resist_chaos)
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}

	if (!saving_throw(p_ptr->skill_sav - power)) /* Amnesia */
	{

		if (lose_all_info())
#ifdef JP
msg_print("���ޤ�ζ��ݤ����ƤΤ��Ȥ�˺��Ƥ��ޤä���");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		return;
	}

	if (saving_throw(p_ptr->skill_sav - power))
	{
		return;
	}

	/* Else gain permanent insanity */
	if ((p_ptr->muta3 & MUT3_MORONIC) && /*(p_ptr->muta2 & MUT2_BERS_RAGE) &&*/
		((p_ptr->muta2 & MUT2_COWARDICE) || (p_ptr->resist_fear)) &&
		((p_ptr->muta2 & MUT2_HALLU) || (p_ptr->resist_chaos)))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}

	while (!happened)
	{
		switch (randint1(21))
		{
			case 1:
				if (!(p_ptr->muta3 & MUT3_MORONIC) && one_in_(5))
				{
					if ((p_ptr->stat_use[A_INT] < 4) && (p_ptr->stat_use[A_WIS] < 4))
					{
#ifdef JP
msg_print("���ʤ��ϴ������ϼ��ˤʤä��褦�ʵ�������������������ϸ������ä���");
#else
						msg_print("You turn into an utter moron!");
#endif
					}
					else
					{
#ifdef JP
msg_print("���ʤ��ϴ������ϼ��ˤʤä���");
#else
						msg_print("You turn into an utter moron!");
#endif
					}

					if (p_ptr->muta3 & MUT3_HYPER_INT)
					{
#ifdef JP
msg_print("���ʤ���Ǿ�����Υ���ԥ塼���ǤϤʤ��ʤä���");
#else
						msg_print("Your brain is no longer a living computer.");
#endif

						p_ptr->muta3 &= ~(MUT3_HYPER_INT);
					}
					p_ptr->muta3 |= MUT3_MORONIC;
					happened = TRUE;
				}
				break;
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				if (!(p_ptr->muta2 & MUT2_COWARDICE) && !p_ptr->resist_fear)
				{
#ifdef JP
msg_print("���ʤ��ϥѥ�Υ����ˤʤä���");
#else
					msg_print("You become paranoid!");
#endif


					/* Duh, the following should never happen, but anyway... */
					if (p_ptr->muta3 & MUT3_FEARLESS)
					{
#ifdef JP
msg_print("���ʤ��Ϥ⤦�����Τ餺�ǤϤʤ��ʤä���");
#else
						msg_print("You are no longer fearless.");
#endif

						p_ptr->muta3 &= ~(MUT3_FEARLESS);
					}

					p_ptr->muta2 |= MUT2_COWARDICE;
					happened = TRUE;
				}
				break;
			case 12:
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
				if (!(p_ptr->muta2 & MUT2_HALLU) && !p_ptr->resist_chaos)
				{
#ifdef JP
msg_print("���Ф�Ҥ���������������˴٤ä���");
#else
					msg_print("You are afflicted by a hallucinatory insanity!");
#endif

					p_ptr->muta2 |= MUT2_HALLU;
					happened = TRUE;
				}
				break;
			default:
				if (!(p_ptr->muta2 & MUT2_BERS_RAGE))
				{
#ifdef JP
msg_print("�����ʴ����ȯ��ˤ�������褦�ˤʤä���");
#else
					msg_print("You become subject to fits of berserk rage!");
#endif

					p_ptr->muta2 |= MUT2_BERS_RAGE;
					happened = TRUE;
				}
				break;
		}
	}

	p_ptr->update |= PU_BONUS;
	handle_stuff();
}


/*
 * This function updates the monster record of the given monster
 *
 * This involves extracting the distance to the player (if requested),
 * and then checking for visibility (natural, infravision, see-invis,
 * telepathy), updating the monster visibility flag, redrawing (or
 * erasing) the monster when its visibility changes, and taking note
 * of any interesting monster flags (cold-blooded, invisible, etc).
 *
 * Note the new "mflag" field which encodes several monster state flags,
 * including "view" for when the monster is currently in line of sight,
 * and "mark" for when the monster is currently visible via detection.
 *
 * The only monster fields that are changed here are "cdis" (the
 * distance from the player), "ml" (visible to the player), and
 * "mflag" (to maintain the "MFLAG_VIEW" flag).
 *
 * Note the special "update_monsters()" function which can be used to
 * call this function once for every monster.
 *
 * Note the "full" flag which requests that the "cdis" field be updated,
 * this is only needed when the monster (or the player) has moved.
 *
 * Every time a monster moves, we must call this function for that
 * monster, and update the distance, and the visibility.  Every time
 * the player moves, we must call this function for every monster, and
 * update the distance, and the visibility.  Whenever the player "state"
 * changes in certain ways ("blindness", "infravision", "telepathy",
 * and "see invisible"), we must call this function for every monster,
 * and update the visibility.
 *
 * Routines that change the "illumination" of a grid must also call this
 * function for any monster in that grid, since the "visibility" of some
 * monsters may be based on the illumination of their grid.
 *
 * Note that this function is called once per monster every time the
 * player moves.  When the player is running, this function is one
 * of the primary bottlenecks, along with "update_view()" and the
 * "process_monsters()" code, so efficiency is important.
 *
 * Note the optimized "inline" version of the "distance()" function.
 *
 * A monster is "visible" to the player if (1) it has been detected
 * by the player, (2) it is close to the player and the player has
 * telepathy, or (3) it is close to the player, and in line of sight
 * of the player, and it is "illuminated" by some combination of
 * infravision, torch light, or permanent light (invisible monsters
 * are only affected by "light" if the player can see invisible).
 *
 * Monsters which are not on the current panel may be "visible" to
 * the player, and their descriptions will include an "offscreen"
 * reference.  Currently, offscreen monsters cannot be targetted
 * or viewed directly, but old targets will remain set.  XXX XXX
 *
 * The player can choose to be disturbed by several things, including
 * "disturb_move" (monster which is viewable moves in some way), and
 * "disturb_near" (monster which is "easily" viewable moves in some
 * way).  Note that "moves" includes "appears" and "disappears".
 */
void update_mon(int m_idx, bool full)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool do_disturb = disturb_move;

	int d;

	/* Current location */
	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	/* Seen at all */
	bool flag = FALSE;

	/* Seen by vision */
	bool easy = FALSE;

	/* Do disturb? */
	if (disturb_high)
	{
		monster_race *ap_r_ptr = &r_info[m_ptr->ap_r_idx];

		if (ap_r_ptr->r_tkills && ap_r_ptr->level >= p_ptr->lev)
			do_disturb = TRUE;
	}

	/* Compute distance */
	if (full)
	{
		/* Distance components */
		int dy = (py > fy) ? (py - fy) : (fy - py);
		int dx = (px > fx) ? (px - fx) : (fx - px);

		/* Approximate distance */
		d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

		/* Restrict distance */
		if (d > 255) d = 255;

		if (!d) d = 1;

		/* Save the distance */
		m_ptr->cdis = d;
	}

	/* Extract distance */
	else
	{
		/* Extract the distance */
		d = m_ptr->cdis;
	}


	/* Detected */
	if (m_ptr->mflag & (MFLAG_MARK)) flag = TRUE;


	/* Nearby */
	if (d <= ((d_info[dungeon_type].flags1 & DF1_DARKNESS) ? MAX_SIGHT / 2 : MAX_SIGHT))
	{
		if (!(d_info[dungeon_type].flags1 & DF1_DARKNESS) || (d <= MAX_SIGHT / 4))
		{
			if (p_ptr->special_defense & KATA_MUSOU)
			{
				/* Detectable */
				flag = TRUE;

				/* Hack -- Memorize mental flags */
				if (r_ptr->flags2 & (RF2_SMART)) r_ptr->r_flags2 |= (RF2_SMART);
				if (r_ptr->flags2 & (RF2_STUPID)) r_ptr->r_flags2 |= (RF2_STUPID);
			}

			/* Basic telepathy */
			else if (p_ptr->telepathy)
			{
				/* Empty mind, no telepathy */
				if (r_ptr->flags2 & (RF2_EMPTY_MIND))
				{
					/* Memorize flags */
					r_ptr->r_flags2 |= (RF2_EMPTY_MIND);
				}

				/* Weird mind, occasional telepathy */
				else if (r_ptr->flags2 & (RF2_WEIRD_MIND))
				{
					/* One in ten individuals are detectable */
					if ((m_idx % 10) == 5)
					{
						/* Detectable */
						flag = TRUE;

						/* Memorize flags */
						r_ptr->r_flags2 |= (RF2_WEIRD_MIND);

						/* Hack -- Memorize mental flags */
						if (r_ptr->flags2 & (RF2_SMART)) r_ptr->r_flags2 |= (RF2_SMART);
						if (r_ptr->flags2 & (RF2_STUPID)) r_ptr->r_flags2 |= (RF2_STUPID);
					}
				}

				/* Normal mind, allow telepathy */
				else
				{
					/* Detectable */
					flag = TRUE;

					/* Hack -- Memorize mental flags */
					if (r_ptr->flags2 & (RF2_SMART)) r_ptr->r_flags2 |= (RF2_SMART);
					if (r_ptr->flags2 & (RF2_STUPID)) r_ptr->r_flags2 |= (RF2_STUPID);
				}
			}

			/* Magical sensing */
			if ((p_ptr->esp_animal) && (r_ptr->flags3 & (RF3_ANIMAL)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_ANIMAL);

			}

			/* Magical sensing */
			if ((p_ptr->esp_undead) && (r_ptr->flags3 & (RF3_UNDEAD)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_UNDEAD);

			}

			/* Magical sensing */
			if ((p_ptr->esp_demon) && (r_ptr->flags3 & (RF3_DEMON)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_DEMON);

			}

			/* Magical sensing */
			if ((p_ptr->esp_orc) && (r_ptr->flags3 & (RF3_ORC)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_ORC);

			}

			/* Magical sensing */
			if ((p_ptr->esp_troll) && (r_ptr->flags3 & (RF3_TROLL)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_TROLL);

			}

			/* Magical sensing */
			if ((p_ptr->esp_giant) && (r_ptr->flags3 & (RF3_GIANT)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_GIANT);

			}

			/* Magical sensing */
			if ((p_ptr->esp_dragon) && (r_ptr->flags3 & (RF3_DRAGON)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_DRAGON);

			}

			/* Magical sensing */
			if ((p_ptr->esp_human) && (r_ptr->flags2 & (RF2_HUMAN)))
			{
				flag = TRUE;
				r_ptr->r_flags2 |= (RF2_HUMAN);

			}

			/* Magical sensing */
			if ((p_ptr->esp_evil) && (r_ptr->flags3 & (RF3_EVIL)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_EVIL);

			}

			/* Magical sensing */
			if ((p_ptr->esp_good) && (r_ptr->flags3 & (RF3_GOOD)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_GOOD);

			}

			/* Magical sensing */
			if ((p_ptr->esp_nonliving) && 
			    (r_ptr->flags3 & (RF3_NONLIVING)) &&
			    !(r_ptr->flags3 & (RF3_DEMON)) &&
			    !(r_ptr->flags3 & (RF3_UNDEAD)))
			{
				flag = TRUE;
				r_ptr->r_flags3 |= (RF3_NONLIVING);

			}

			/* Magical sensing */
			if ((p_ptr->esp_unique) && (r_ptr->flags1 & (RF1_UNIQUE)))
			{
				flag = TRUE;
				r_ptr->r_flags1 |= (RF1_UNIQUE);

			}
		}

		/* Normal line of sight, and not blind */
		if (player_has_los_bold(fy, fx) && !p_ptr->blind)
		{
			bool do_invisible = FALSE;
			bool do_cold_blood = FALSE;

			/* Use "infravision" */
			if (d <= p_ptr->see_infra)
			{
				/* Handle "cold blooded" monsters */
				if (r_ptr->flags2 & (RF2_COLD_BLOOD))
				{
					/* Take note */
					do_cold_blood = TRUE;
				}

				/* Handle "warm blooded" monsters */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Use "illumination" */
			if (player_can_see_bold(fy, fx))
			{
				/* Handle "invisible" monsters */
				if (r_ptr->flags2 & (RF2_INVISIBLE))
				{
					/* Take note */
					do_invisible = TRUE;

					/* See invisible */
					if (p_ptr->see_inv)
					{
						/* Easy to see */
						easy = flag = TRUE;
					}
				}

				/* Handle "normal" monsters */
				else
				{
					/* Easy to see */
					easy = flag = TRUE;
				}
			}

			/* Visible */
			if (flag)
			{
				/* Memorize flags */
				if (do_invisible) r_ptr->r_flags2 |= (RF2_INVISIBLE);
				if (do_cold_blood) r_ptr->r_flags2 |= (RF2_COLD_BLOOD);
			}
		}
	}


	/* The monster is now visible */
	if (flag)
	{
		/* It was previously unseen */
		if (!m_ptr->ml)
		{
			/* Mark as visible */
			m_ptr->ml = TRUE;

			/* Draw the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Hack -- Count "fresh" sightings */
			if ((m_ptr->ap_r_idx == MON_KAGE) && (r_info[MON_KAGE].r_sights < MAX_SHORT))
				r_info[MON_KAGE].r_sights++;
			else if (m_ptr->ap_r_idx == m_ptr->r_idx && 
				 r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;

			/* Eldritch Horror */
			if (r_ptr->flags2 & RF2_ELDRITCH_HORROR)
			{
				sanity_blast(m_ptr, FALSE);
			}

			/* Disturb on appearance */
			if (disturb_near && (projectable(m_ptr->fy, m_ptr->fx, py, px) && projectable(py, px, m_ptr->fy, m_ptr->fx)))
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 0);
			}
		}
	}

	/* The monster is not visible */
	else
	{
		/* It was previously seen */
		if (m_ptr->ml)
		{
			/* Mark as not visible */
			m_ptr->ml = FALSE;

			/* Erase the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
			if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

			/* Disturb on disappearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 0);
			}
		}
	}


	/* The monster is now easily visible */
	if (easy)
	{
		/* Change */
		if (!(m_ptr->mflag & (MFLAG_VIEW)))
		{
			/* Mark as easily visible */
			m_ptr->mflag |= (MFLAG_VIEW);

			/* Disturb on appearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 0);
			}
		}
	}

	/* The monster is not easily visible */
	else
	{
		/* Change */
		if (m_ptr->mflag & (MFLAG_VIEW))
		{
			/* Mark as not easily visible */
			m_ptr->mflag &= ~(MFLAG_VIEW);

			/* Disturb on disappearance */
			if (do_disturb)
			{
				if (disturb_pets || is_hostile(m_ptr))
					disturb(1, 0);
			}
		}
	}
}


/*
 * This function simply updates all the (non-dead) monsters (see above).
 */
void update_monsters(bool full)
{
	int i;

	/* Update each (live) monster */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Update the monster */
		update_mon(i, full);
	}
}


/*
 * Hack -- the index of the summoning monster
 */
static bool monster_hook_chameleon_lord(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	monster_type *m_ptr = &m_list[chameleon_change_m_idx];
	monster_race *old_r_ptr = &r_info[m_ptr->r_idx];

	if (!(r_ptr->flags1 & (RF1_UNIQUE))) return FALSE;
	if (r_ptr->flags7 & (RF7_FRIENDLY | RF7_CHAMELEON)) return FALSE;

	if (ABS(r_ptr->level - r_info[MON_CHAMELEON_K].level) > 5) return FALSE;

	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	if (!monster_can_cross_terrain(cave[m_ptr->fy][m_ptr->fx].feat, r_ptr)) return FALSE;

	/* Not born */
	if (!(old_r_ptr->flags7 & RF7_CHAMELEON))
	{
		if ((m_ptr->sub_align & SUB_ALIGN_EVIL) && (r_ptr->flags3 & RF3_GOOD)) return FALSE;
		if ((m_ptr->sub_align & SUB_ALIGN_GOOD) && (r_ptr->flags3 & RF3_EVIL)) return FALSE;
	}

	/* Born now */
	else if (summon_specific_who > 0)
	{
		monster_type *sm_ptr = &m_list[summon_specific_who];

		if ((sm_ptr->sub_align & SUB_ALIGN_EVIL) && (r_ptr->flags3 & RF3_GOOD)) return FALSE;
		if ((sm_ptr->sub_align & SUB_ALIGN_GOOD) && (r_ptr->flags3 & RF3_EVIL)) return FALSE;
	}

	return TRUE;
}

static bool monster_hook_chameleon(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];
	monster_type *m_ptr = &m_list[chameleon_change_m_idx];
	monster_race *old_r_ptr = &r_info[m_ptr->r_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;
	if (r_ptr->flags7 & (RF7_FRIENDLY | RF7_CHAMELEON)) return FALSE;
	
	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	if (!monster_can_cross_terrain(cave[m_ptr->fy][m_ptr->fx].feat, r_ptr)) return FALSE;

	/* Not born */
	if (!(old_r_ptr->flags7 & RF7_CHAMELEON))
	{
		if ((old_r_ptr->flags3 & RF3_GOOD) && !(r_ptr->flags3 & RF3_GOOD)) return FALSE;
		if ((old_r_ptr->flags3 & RF3_EVIL) && !(r_ptr->flags3 & RF3_EVIL)) return FALSE;
		if (!(old_r_ptr->flags3 & (RF3_GOOD | RF3_EVIL)) && (r_ptr->flags3 & (RF3_GOOD | RF3_EVIL))) return FALSE;
	}

	/* Born now */
	else if (summon_specific_who > 0)
	{
		monster_type *sm_ptr = &m_list[summon_specific_who];

		if ((sm_ptr->sub_align & SUB_ALIGN_EVIL) && (r_ptr->flags3 & RF3_GOOD)) return FALSE;
		if ((sm_ptr->sub_align & SUB_ALIGN_GOOD) && (r_ptr->flags3 & RF3_EVIL)) return FALSE;
	}

	return (*(get_monster_hook()))(r_idx);
}


void choose_new_monster(int m_idx, bool born, int r_idx)
{
	int oldmaxhp, i;
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr;
	char old_m_name[80];
	int old_r_idx = m_ptr->r_idx;
	bool old_unique = FALSE;

	if (r_info[m_ptr->r_idx].flags1 & RF1_UNIQUE)
		old_unique = TRUE;
	if (old_unique && (r_idx == MON_CHAMELEON)) r_idx = MON_CHAMELEON_K;
	r_ptr = &r_info[r_idx];

	monster_desc(old_m_name, m_ptr, 0);

	if (!r_idx)
	{
		int level;

		chameleon_change_m_idx = m_idx;
		if (old_unique)
			get_mon_num_prep(monster_hook_chameleon_lord, NULL);
		else
			get_mon_num_prep(monster_hook_chameleon, NULL);

		if (old_unique)
			level = r_info[MON_CHAMELEON_K].level;
		else if (!dun_level)
			level = wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].level;
		else
			level = dun_level;

		if (d_info[dungeon_type].flags1 & DF1_CHAMELEON) level+= 2+randint1(3);

		r_idx = get_mon_num(level);
		r_ptr = &r_info[r_idx];

		chameleon_change_m_idx = 0;

		/* Paranoia */
		if (!r_idx) return;
	}

	m_ptr->r_idx = r_idx;
	m_ptr->ap_r_idx = r_idx;
	update_mon(m_idx, FALSE);
	lite_spot(m_ptr->fy, m_ptr->fx);
	if (born)
	{
		/* Sub-alignment of a chameleon */
		if (r_ptr->flags3 & (RF3_EVIL | RF3_GOOD))
		{
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
			if (r_ptr->flags3 & RF3_EVIL) m_ptr->sub_align |= SUB_ALIGN_EVIL;
			if (r_ptr->flags3 & RF3_GOOD) m_ptr->sub_align |= SUB_ALIGN_GOOD;
		}
		return;
	}

	if (m_idx == p_ptr->riding)
	{
		char m_name[80];
		monster_desc(m_name, m_ptr, 0);
#ifdef JP
		msg_format("����%s���ѿȤ�����", old_m_name);
#else
		msg_format("Suddenly, %s transforms!", old_m_name);
#endif
		if (!(r_ptr->flags7 & RF7_RIDING))
#ifdef JP
			if (rakuba(0, TRUE)) msg_print("���̤���Ȥ��줿��");
#else
			if (rakuba(0, TRUE)) msg_format("You have fallen from %s.", m_name);
#endif
	}

	/* Extract the monster base speed */
	m_ptr->mspeed = r_ptr->speed;
	/* Hack -- small racial variety */
	/* Allow some small variation per monster */
	if(one_in_(4)){
		i = extract_energy[r_ptr->speed] / 3;
		if (i) m_ptr->mspeed += rand_spread(0, i);
	}
	else{
		i = extract_energy[r_ptr->speed] / 10;
		if (i) m_ptr->mspeed += rand_spread(0, i);
	}

	oldmaxhp = m_ptr->max_maxhp;
	/* Assign maximal hitpoints */
	if (r_ptr->flags1 & RF1_FORCE_MAXHP)
	{
		m_ptr->max_maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
	}
	else
	{
		m_ptr->max_maxhp = damroll(r_ptr->hdice, r_ptr->hside);
	}
	m_ptr->maxhp = (long)(m_ptr->maxhp * m_ptr->max_maxhp) / oldmaxhp;
	m_ptr->hp = (long)(m_ptr->hp * m_ptr->max_maxhp) / oldmaxhp;
}


/*
 *  Hook for Tanuki
 */
static bool monster_hook_tanuki(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	if (r_ptr->flags1 & (RF1_UNIQUE)) return FALSE;
	if (r_ptr->flags2 & RF2_MULTIPLY) return FALSE;
	if (r_ptr->flags7 & (RF7_FRIENDLY | RF7_CHAMELEON)) return FALSE;
	if (r_ptr->flags7 & RF7_AQUATIC) return FALSE;
	
	if ((r_ptr->blow[0].method == RBM_EXPLODE) || (r_ptr->blow[1].method == RBM_EXPLODE) || (r_ptr->blow[2].method == RBM_EXPLODE) || (r_ptr->blow[3].method == RBM_EXPLODE))
		return FALSE;

	return (*(get_monster_hook()))(r_idx);
}


/*
 *  Set initial racial appearance of a monster
 */
static int initial_r_appearance(int r_idx)
{
	int attempts = 1000;

	int ap_r_idx;
	int min = MIN(base_level-5, 50);

	if (!(r_info[r_idx].flags7 & RF7_TANUKI))
		return r_idx;

	get_mon_num_prep(monster_hook_tanuki, NULL);

	while (--attempts)
	{
		ap_r_idx = get_mon_num(base_level + 10);
		if (r_info[ap_r_idx].level >= min) return ap_r_idx;
	}

	return r_idx;
}


/*
 * Attempt to place a monster of the given race at the given location.
 *
 * To give the player a sporting chance, any monster that appears in
 * line-of-sight and is extremely dangerous can be marked as
 * "FORCE_SLEEP", which will cause them to be placed with low energy,
 * which often (but not always) lets the player move before they do.
 *
 * This routine refuses to place out-of-depth "FORCE_DEPTH" monsters.
 *
 * XXX XXX XXX Use special "here" and "dead" flags for unique monsters,
 * remove old "cur_num" and "max_num" fields.
 *
 * XXX XXX XXX Actually, do something similar for artifacts, to simplify
 * the "preserve" mode, and to make the "what artifacts" flag more useful.
 *
 * This is the only function which may place a monster in the dungeon,
 * except for the savefile loading code.
 */
bool place_monster_one(int who, int y, int x, int r_idx, u32b mode)
{
	int			i;
	int rune_dam = 0;

	cave_type		*c_ptr;

	monster_type	*m_ptr;

	monster_race	*r_ptr = &r_info[r_idx];

	cptr		name = (r_name + r_ptr->name);

	/* DO NOT PLACE A MONSTER IN THE SMALL SCALE WILDERNESS !!! */
	if(p_ptr->wild_mode) return FALSE;

	/* Verify location */
	if (!in_bounds(y, x)) return (FALSE);

	/* Require empty space (if not ghostly) */
	if (!(!dun_level && (cave[y][x].feat == FEAT_MOUNTAIN) && ((r_ptr->flags8 & RF8_WILD_MOUNTAIN) || (r_ptr->flags7 & RF7_CAN_FLY))) &&
	    !(cave_empty_bold2(y, x) || (mode & PM_IGNORE_TERRAIN)) &&
	    !((r_ptr->flags2 & RF2_PASS_WALL) &&
	      !(cave_perma_bold(y, x) || cave[y][x].m_idx ||
		((y == py) && (x == px))))) return (FALSE);

	/* Paranoia */
	if (!r_idx) return (FALSE);

	/* Paranoia */
	if (!r_ptr->name) return (FALSE);

	/* Nor on the Pattern */
	if ((cave[y][x].feat >= FEAT_PATTERN_START)
	 && (cave[y][x].feat <= FEAT_PATTERN_XTRA2))
		return (FALSE);

	if (!(mode & PM_IGNORE_TERRAIN) &&
	    !monster_can_cross_terrain(cave[y][x].feat, r_ptr))
	{
		return FALSE;
	}

	if (!p_ptr->inside_battle)
	{
		/* Hack -- "unique" monsters must be "unique" */
		if (((r_ptr->flags1 & (RF1_UNIQUE)) ||
		     (r_ptr->flags7 & (RF7_UNIQUE_7))) &&
		    (r_ptr->cur_num >= r_ptr->max_num))
		{
			/* Cannot create */
			return (FALSE);
		}
		
		if ((r_ptr->flags7 & (RF7_UNIQUE2)) &&
		    (r_ptr->cur_num >= 1))
		{
			return (FALSE);
		}

		if (r_idx == MON_BANORLUPART)
		{
			if (r_info[MON_BANOR].cur_num > 0) return FALSE;
			if (r_info[MON_LUPART].cur_num > 0) return FALSE;
		}
		
		/* Depth monsters may NOT be created out of depth, unless in Nightmare mode */
		if ((r_ptr->flags1 & (RF1_FORCE_DEPTH)) && (dun_level < r_ptr->level) &&
		    (!ironman_nightmare || (r_ptr->flags1 & (RF1_QUESTOR))))
		{
			/* Cannot create */
			return (FALSE);
		}
	}

	if(quest_number(dun_level))
	{
		int hoge = quest_number(dun_level);
		if((quest[hoge].type == QUEST_TYPE_KILL_LEVEL) || (quest[hoge].type == QUEST_TYPE_RANDOM))
		{
			if(r_idx == quest[hoge].r_idx)
			{
				int number_mon, i2, j2;
				number_mon = 0;

				/* Count all quest monsters */
				for (i2 = 0; i2 < cur_wid; ++i2)
					for (j2 = 0; j2 < cur_hgt; j2++)
						if (cave[j2][i2].m_idx > 0)
							if (m_list[cave[j2][i2].m_idx].r_idx == quest[hoge].r_idx)
								number_mon++;
				if(number_mon + quest[hoge].cur_num >= quest[hoge].max_num)
					return FALSE;
			}
		}
	}

	/* Access the location */
	c_ptr = &cave[y][x];

	if (is_glyph_grid(c_ptr))
	{
		if (randint1(BREAK_GLYPH) < (r_ptr->level+20))
		{
			/* Describe observable breakage */
			if (c_ptr->info & CAVE_MARK)
			{
#ifdef JP
msg_print("���Υ롼�󤬲��줿��");
#else
				msg_print("The rune of protection is broken!");
#endif

			}

			/* Forget the rune */
			c_ptr->info &= ~(CAVE_MARK);

			/* Break the rune */
			c_ptr->info &= ~(CAVE_OBJECT);
			c_ptr->mimic = 0;

			/* Notice */
			note_spot(y, x);
			rune_dam = 1000;
		}
		else return FALSE;
	}

	/* Powerful monster */
	if (r_ptr->level > dun_level)
	{
		/* Unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			/* Message for cheaters */
#ifdef JP
			if (cheat_hear) msg_format("���ؤΥ�ˡ�������󥹥��� (%s)��", name);
#else
			if (cheat_hear) msg_format("Deep Unique (%s).", name);
#endif


			/* Boost rating by twice delta-depth */
			rating += (r_ptr->level - dun_level) * 2;
		}

		/* Normal monsters */
		else
		{
			/* Message for cheaters */
#ifdef JP
			if (cheat_hear) msg_format("���ؤΥ�󥹥��� (%s)��", name);
#else
			if (cheat_hear) msg_format("Deep Monster (%s).", name);
#endif


			/* Boost rating by delta-depth */
			rating += (r_ptr->level - dun_level);
		}
	}

	/* Note the monster */
	else if (r_ptr->flags1 & (RF1_UNIQUE))
	{
		/* Unique monsters induce message */
#ifdef JP
		if (cheat_hear) msg_format("��ˡ�������󥹥��� (%s)��", name);
#else
		if (cheat_hear) msg_format("Unique (%s).", name);
#endif

	}

	if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE_7) || (r_ptr->level < 10)) is_kage = FALSE;

	/* Make a new monster */
	c_ptr->m_idx = m_pop();
	hack_m_idx_ii = c_ptr->m_idx;

	/* Mega-Hack -- catch "failure" */
	if (!c_ptr->m_idx) return (FALSE);


	/* Get a new monster record */
	m_ptr = &m_list[c_ptr->m_idx];

	/* Save the race */
	m_ptr->r_idx = r_idx;
	m_ptr->ap_r_idx = initial_r_appearance(r_idx);

	/* Sub-alignment of a monster */
	if ((who > 0) && !(r_ptr->flags3 & (RF3_EVIL | RF3_GOOD)))
		m_ptr->sub_align = m_list[who].sub_align;
	else
	{
		m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
		if (r_ptr->flags3 & RF3_EVIL) m_ptr->sub_align |= SUB_ALIGN_EVIL;
		if (r_ptr->flags3 & RF3_GOOD) m_ptr->sub_align |= SUB_ALIGN_GOOD;
	}

	/* Place the monster at the location */
	m_ptr->fy = y;
	m_ptr->fx = x;


	/* No "damage" yet */
	m_ptr->stunned = 0;
	m_ptr->confused = 0;
	m_ptr->monfear = 0;

	/* Unknown distance */
	m_ptr->cdis = 0;

	reset_target(m_ptr);

	m_ptr->nickname = 0;

	m_ptr->exp = 0;

	/* No flags */
	m_ptr->mflag = 0;
	m_ptr->mflag2 = 0;

	if (r_ptr->flags7 & RF7_CHAMELEON)
	{
		choose_new_monster(c_ptr->m_idx, TRUE, 0);
		r_ptr = &r_info[m_ptr->r_idx];
		m_ptr->mflag2 |= MFLAG_CHAMELEON;
		rating++;

		/* Hack - Set sub_align to neutral when the Chameleon Lord is generated as "GUARDIAN" */
		if ((r_ptr->flags1 & RF1_UNIQUE) && (who <= 0))
			m_ptr->sub_align = SUB_ALIGN_NEUTRAL;
	}
	else if (is_kage)
	{
		m_ptr->ap_r_idx = MON_KAGE;
		m_ptr->mflag2 |= MFLAG_KAGE;
	}

	if (mode & PM_NO_PET) m_ptr->mflag2 |= MFLAG_NOPET;

	/* Not visible */
	m_ptr->ml = FALSE;

	/* Pet? */
	if (mode & PM_FORCE_PET)
	{
		set_pet(m_ptr);
	}
	/* Friendly? */
	else if ((r_ptr->flags7 & RF7_FRIENDLY) ||
		 (mode & PM_FORCE_FRIENDLY) || is_friendly_idx(who))
	{
		if (!(p_ptr->align >= 0 && (r_ptr->flags3 & RF3_EVIL)) &&
		    !(p_ptr->align < 0 && (r_ptr->flags3 & RF3_GOOD)))
		{
			set_friendly(m_ptr);
		}
	}

	/* Assume no sleeping */
	m_ptr->csleep = 0;

	/* Enforce sleeping if needed */
	if ((mode & PM_ALLOW_SLEEP) && r_ptr->sleep && !ironman_nightmare)
	{
		int val = r_ptr->sleep;
		m_ptr->csleep = ((val * 2) + randint1(val * 10));
	}

	/* Assign maximal hitpoints */
	if (r_ptr->flags1 & RF1_FORCE_MAXHP)
	{
		m_ptr->max_maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
	}
	else
	{
		m_ptr->max_maxhp = damroll(r_ptr->hdice, r_ptr->hside);
	}

	/* Monsters have double hitpoints in Nightmare mode */
	if (ironman_nightmare)
	{
		u32b hp = m_ptr->max_maxhp * 2L;

		m_ptr->max_maxhp = (s16b)MIN(30000, hp);
	}

	m_ptr->maxhp = m_ptr->max_maxhp;

	/* And start out fully healthy */
	if (m_ptr->r_idx == MON_WOUNDED_BEAR)
		m_ptr->hp = m_ptr->maxhp / 2;
	else m_ptr->hp = m_ptr->maxhp;


	/* Extract the monster base speed */
	m_ptr->mspeed = r_ptr->speed;

	/* Hack -- small racial variety */
	if (!(r_ptr->flags1 & RF1_UNIQUE) && !p_ptr->inside_arena)
	{
		/* Allow some small variation per monster */
	  if(one_in_(4)){
		i = extract_energy[r_ptr->speed] / 3;
		if (i) m_ptr->mspeed += rand_spread(0, i);
	  }
	  else{
		i = extract_energy[r_ptr->speed] / 10;
		if (i) m_ptr->mspeed += rand_spread(0, i);
	  }
	}

	if (mode & PM_HASTE) m_ptr->fast = 100;

	if (m_ptr->mspeed > 199) m_ptr->mspeed = 199;

	/* Give a random starting energy */
	if (!ironman_nightmare)
	{
		m_ptr->energy_need = ENERGY_NEED() - (s16b)randint0(100);
	}
	else
	{
		/* Nightmare monsters are more prepared */
		m_ptr->energy_need = ENERGY_NEED() - (s16b)randint0(100) * 2;
	}

	/* Force monster to wait for player, unless in Nightmare mode */
	if ((r_ptr->flags1 & RF1_FORCE_SLEEP) && !ironman_nightmare)
	{
		/* Monster is still being nice */
		m_ptr->mflag |= (MFLAG_NICE);

		/* Must repair monsters */
		repair_monsters = TRUE;
	}

	/* Hack -- see "process_monsters()" */
	if (c_ptr->m_idx < hack_m_idx)
	{
		/* Monster is still being born */
		m_ptr->mflag |= (MFLAG_BORN);
	}


	if (r_ptr->flags7 & (RF7_SELF_LITE_1 | RF7_SELF_LITE_2))
		p_ptr->update |= (PU_MON_LITE);
	else if ((r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) && !m_ptr->csleep)
		p_ptr->update |= (PU_MON_LITE);

	/* Update the monster */
	update_mon(c_ptr->m_idx, TRUE);


	/* Hack -- Count the monsters on the level */
	if (m_ptr->mflag2 & MFLAG_CHAMELEON)
		r_info[r_idx].cur_num++;
	else
		r_ptr->cur_num++;


	/* Hack -- Count the number of "reproducers" */
	if (r_ptr->flags2 & RF2_MULTIPLY) num_repro++;


	/* Hack -- Notice new multi-hued monsters */
	if (r_ptr->flags1 & RF1_ATTR_MULTI) shimmer_monsters = TRUE;

	if (p_ptr->warning && character_dungeon)
	{
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			cptr color;
			object_type *o_ptr;
			char o_name[MAX_NLEN];

			if (r_ptr->level > p_ptr->lev + 30)
#ifdef JP
				color = "����";
#else
				color = "black";
#endif
			else if (r_ptr->level > p_ptr->lev + 15)
#ifdef JP
				color = "�翧��";
#else
				color = "purple";
#endif
			else if (r_ptr->level > p_ptr->lev + 5)
#ifdef JP
				color = "��ӡ�����";
#else
				color = "deep red";
#endif
			else if (r_ptr->level > p_ptr->lev - 5)
#ifdef JP
				color = "�֤�";
#else
				color = "red";
#endif
			else if (r_ptr->level > p_ptr->lev - 15)
#ifdef JP
				color = "�ԥ󥯿���";
#else
				color = "pink";
#endif
			else
#ifdef JP
				color = "��";
#else
				color = "white";
#endif

			o_ptr = choose_warning_item();
			object_desc(o_name, o_ptr, FALSE, 0);
#ifdef JP
			msg_format("%s��%s���ä���",o_name, color);
#else
			msg_format("%s glows %s.",o_name, color);
#endif
		}
	}

	if (is_explosive_rune_grid(c_ptr))
	{
		/* Break the ward */
		if (randint1(BREAK_MINOR_GLYPH) > r_ptr->level)
		{
			/* Describe observable breakage */
			if (c_ptr->info & CAVE_MARK)
			{
#ifdef JP
msg_print("�롼����ȯ������");
#else
				msg_print("The rune explodes!");
#endif

				project(0, 2, y, x, 2 * (p_ptr->lev + damroll(7, 7)), GF_MANA, (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
			}
		}
		else
		{
#ifdef JP
msg_print("��ȯ�Υ롼��ϲ�����줿��");
#else
			msg_print("An explosive rune was disarmed.");
#endif
		}

		/* Forget the rune */
		c_ptr->info &= ~(CAVE_MARK);

		/* Break the rune */
		c_ptr->info &= ~(CAVE_OBJECT);
		c_ptr->mimic = 0;

		note_spot(y, x);
		lite_spot(y, x);
	}

	/* Success */
	return (TRUE);
}


/*
 *  improved version of scatter() for place monster
 */

#define MON_SCAT_MAXD 10

static bool mon_scatter(int *yp, int *xp, int y, int x, int max_dist)
{
	int place_x[MON_SCAT_MAXD];
	int place_y[MON_SCAT_MAXD];
	int num[MON_SCAT_MAXD];
	int i;
	int nx, ny;

	if (max_dist >= MON_SCAT_MAXD)
		return FALSE;

	for (i = 0; i < MON_SCAT_MAXD; i++)
		num[i] = 0;

	for (nx = x - max_dist; nx <= x + max_dist; nx++)
		for (ny = y - max_dist; ny <= y + max_dist; ny++)
		{
			/* Ignore annoying locations */
			if (!in_bounds(ny, nx)) continue;
			
			/* Require "line of sight" */
			if (!los(y, x, ny, nx)) continue;
			
			/* Walls and Monsters block flow */
			if (!cave_empty_bold2(ny, nx)) continue;
			if (cave[ny][nx].m_idx) continue;
			if ((ny == py) && (nx == px)) continue;
						
			/* ... nor on the Pattern */
			if ((cave[ny][nx].feat >= FEAT_PATTERN_START) &&
			    (cave[ny][nx].feat <= FEAT_PATTERN_XTRA2))
				continue;
			
			i = distance(y, x, ny, nx);

			if (i > max_dist)
				continue;
			
			num[i]++;

			/* random swap */
			if(one_in_(num[i]))
			{
				place_x[i] = nx;
				place_y[i] = ny;
			}
			
		}

	i = 0;
	while (i < MON_SCAT_MAXD && 0 == num[i])
		i++;
	if (i >= MON_SCAT_MAXD)
		return FALSE;

	*xp = place_x[i];
	*yp = place_y[i];

	return TRUE;
}


/*
 * Maximum size of a group of monsters
 */
#define GROUP_MAX	32


/*
 * Attempt to place a "group" of monsters around the given location
 */
static bool place_monster_group(int who, int y, int x, int r_idx, u32b mode)
{
	monster_race *r_ptr = &r_info[r_idx];

	int old, n, i;
	int total = 0, extra = 0;

	int hack_n = 0;

	byte hack_y[GROUP_MAX];
	byte hack_x[GROUP_MAX];


	/* Pick a group size */
	total = randint1(10);

	/* Hard monsters, small groups */
	if (r_ptr->level > dun_level)
	{
		extra = r_ptr->level - dun_level;
		extra = 0 - randint1(extra);
	}

	/* Easy monsters, large groups */
	else if (r_ptr->level < dun_level)
	{
		extra = dun_level - r_ptr->level;
		extra = randint1(extra);
	}

	/* Hack -- limit group reduction */
	if (extra > 9) extra = 9;

	/* Modify the group size */
	total += extra;

	/* Minimum size */
	if (total < 1) total = 1;

	/* Maximum size */
	if (total > GROUP_MAX) total = GROUP_MAX;


	/* Save the rating */
	old = rating;

	/* Start on the monster */
	hack_n = 1;
	hack_x[0] = x;
	hack_y[0] = y;

	/* Puddle monsters, breadth first, up to total */
	for (n = 0; (n < hack_n) && (hack_n < total); n++)
	{
		/* Grab the location */
		int hx = hack_x[n];
		int hy = hack_y[n];

		/* Check each direction, up to total */
		for (i = 0; (i < 8) && (hack_n < total); i++)
		{
			int mx, my;

			scatter(&my, &mx, hy, hx, 4, 0);

			/* Walls and Monsters block flow */
			if (!cave_empty_bold2(my, mx)) continue;

			/* Attempt to place another monster */
			if (place_monster_one(who, my, mx, r_idx, mode))
			{
				/* Add it to the "hack" set */
				hack_y[hack_n] = my;
				hack_x[hack_n] = mx;
				hack_n++;
			}
		}
	}

	/* Hack -- restore the rating */
	rating = old;


	/* Success */
	return (TRUE);
}


/*
 * Hack -- help pick an escort type
 */
static int place_monster_idx = 0;
static int place_monster_m_idx = 0;

/*
 * Hack -- help pick an escort type
 */
static bool place_monster_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[place_monster_idx];
	monster_type *m_ptr = &m_list[place_monster_m_idx];

	monster_race *z_ptr = &r_info[r_idx];

	/* Hack - Escorts have to have the same dungeon flag */
	if (monster_dungeon(place_monster_idx) != monster_dungeon(r_idx)) return (FALSE);

	/* Require similar "race" */
	if (z_ptr->d_char != r_ptr->d_char) return (FALSE);

	/* Skip more advanced monsters */
	if (z_ptr->level > r_ptr->level) return (FALSE);

	/* Skip unique monsters */
	if (z_ptr->flags1 & RF1_UNIQUE) return (FALSE);

	/* Paranoia -- Skip identical monsters */
	if (place_monster_idx == r_idx) return (FALSE);

	/* Skip different alignment */
	if (((m_ptr->sub_align & SUB_ALIGN_EVIL) && (z_ptr->flags3 & RF3_GOOD)) ||
	    ((m_ptr->sub_align & SUB_ALIGN_GOOD) && (z_ptr->flags3 & RF3_EVIL)))
		return FALSE;

	if (r_ptr->flags7 & RF7_FRIENDLY)
	{
		if (((p_ptr->align < 0) && (z_ptr->flags3 & RF3_GOOD)) ||
		    ((p_ptr->align > 0) && (z_ptr->flags3 & RF3_EVIL)))
			return FALSE;
	}

	if ((r_ptr->flags7 & RF7_CHAMELEON) && !(z_ptr->flags7 & RF7_CHAMELEON))
		return FALSE;

	/* Okay */
	return (TRUE);
}


/*
 * Attempt to place a monster of the given race at the given location
 *
 * Note that certain monsters are now marked as requiring "friends".
 * These monsters, if successfully placed, and if the "grp" parameter
 * is TRUE, will be surrounded by a "group" of identical monsters.
 *
 * Note that certain monsters are now marked as requiring an "escort",
 * which is a collection of monsters with similar "race" but lower level.
 *
 * Some monsters induce a fake "group" flag on their escorts.
 *
 * Note the "bizarre" use of non-recursion to prevent annoying output
 * when running a code profiler.
 *
 * Note the use of the new "monster allocation table" code to restrict
 * the "get_mon_num()" function to "legal" escort types.
 */
bool place_monster_aux(int who, int y, int x, int r_idx, u32b mode)
{
	int             i;
	monster_race    *r_ptr = &r_info[r_idx];

	if (one_in_(333) && !(mode & PM_NO_KAGE) && !(mode & PM_FORCE_PET))
		is_kage = TRUE;
	else
		is_kage = FALSE;

	/* Place one monster, or fail */
	if (!place_monster_one(who, y, x, r_idx, mode)) return (FALSE);


	/* Require the "group" flag */
	if (!(mode & PM_ALLOW_GROUP)) return (TRUE);

	place_monster_m_idx = hack_m_idx_ii;

	/* Friends for certain monsters */
	if (r_ptr->flags1 & (RF1_FRIENDS))
	{
		/* Attempt to place a group */
		(void)place_monster_group(who, y, x, r_idx, mode);
	}


	/* Escorts for certain monsters */
	if (r_ptr->flags1 & (RF1_ESCORT))
	{
		/* Set the escort index */
		place_monster_idx = r_idx;

		/* Try to place several "escorts" */
		for (i = 0; i < 32; i++)
		{
			int nx, ny, z, d = 3;

			/* Pick a location */
			scatter(&ny, &nx, y, x, d, 0);

			/* Require empty grids */
			if (!cave_empty_bold2(ny, nx)) continue;

			/* Prepare allocation table */
			get_mon_num_prep(place_monster_okay, get_monster_hook2(ny, nx));

			/* Pick a random race */
			z = get_mon_num(r_ptr->level);

			/* Handle failure */
			if (!z) break;

			/* Place a single escort */
			(void)place_monster_one(place_monster_m_idx, ny, nx, z, mode);

			/* Place a "group" of escorts if needed */
			if ((r_info[z].flags1 & RF1_FRIENDS) ||
			    (r_ptr->flags1 & RF1_ESCORTS))
			{
				/* Place a group of monsters */
				(void)place_monster_group(place_monster_m_idx, ny, nx, z, mode);
			}
		}
	}

	/* Success */
	return (TRUE);
}


/*
 * Hack -- attempt to place a monster at the given location
 *
 * Attempt to find a monster appropriate to the "monster_level"
 */
bool place_monster(int y, int x, u32b mode)
{
	int r_idx;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), get_monster_hook2(y, x));

	/* Pick a monster */
	r_idx = get_mon_num(monster_level);

	/* Handle failure */
	if (!r_idx) return (FALSE);

	/* Attempt to place the monster */
	if (place_monster_aux(0, y, x, r_idx, mode)) return (TRUE);

	/* Oops */
	return (FALSE);
}


#ifdef MONSTER_HORDES

bool alloc_horde(int y, int x)
{
	monster_race *r_ptr = NULL;
	int r_idx = 0;
	int m_idx;
	int attempts = 1000;
	int cy = y;
	int cx = x;

	/* Prepare allocation table */
	get_mon_num_prep(get_monster_hook(), get_monster_hook2(y, x));

	while (--attempts)
	{
		/* Pick a monster */
		r_idx = get_mon_num(monster_level);

		/* Handle failure */
		if (!r_idx) return (FALSE);

		r_ptr = &r_info[r_idx];

		if (r_ptr->flags1 & RF1_UNIQUE) continue;

		if (r_idx == MON_HAGURE) continue;
		break;
	}
	if (attempts < 1) return FALSE;

	attempts = 1000;

	while (--attempts)
	{
		/* Attempt to place the monster */
		if (place_monster_aux(0, y, x, r_idx, 0L)) break;
	}

	if (attempts < 1) return FALSE;

	m_idx = cave[y][x].m_idx;

	if (m_list[m_idx].mflag2 & MFLAG_CHAMELEON) r_ptr = &r_info[m_list[m_idx].r_idx];
	summon_kin_type = r_ptr->d_char;

	for (attempts = randint1(10) + 5; attempts; attempts--)
	{
		scatter(&cy, &cx, y, x, 5, 0);

		(void)summon_specific(m_idx, cy, cx, dun_level + 5, SUMMON_KIN, PM_ALLOW_GROUP);

		y = cy;
		x = cx;
	}

	return TRUE;
}

#endif /* MONSTER_HORDES */



/*
 * Attempt to allocate a random monster in the dungeon.
 *
 * Place the monster at least "dis" distance from the player.
 *
 * Use "slp" to choose the initial "sleep" status
 *
 * Use "monster_level" for the monster level
 */
bool alloc_monster(int dis, u32b mode)
{
	int			y = 0, x = 0;
	int         attempts_left = 10000;
	int guardian = d_info[dungeon_type].final_guardian;

	/* Put an Guardian */
	if(guardian && d_info[dungeon_type].maxdepth == dun_level && r_info[guardian].cur_num < r_info[guardian].max_num )
	{
		int oy;
		int ox;
		int try = 4000;

		/* Find a good position */
		while(try)
		{
			/* Get a random spot */
			oy = randint1(cur_hgt - 4) + 2;
			ox = randint1(cur_wid - 4) + 2;

			/* Is it a good spot ? */
			if (cave_empty_bold2(oy, ox) && monster_can_cross_terrain(cave[oy][ox].feat, &r_info[guardian]))
			{
				/* Place the guardian */
				if (place_monster_aux(0, oy, ox, guardian, (PM_ALLOW_GROUP | PM_NO_KAGE | PM_NO_PET))) break;
			}
			/* One less try */
			try--;
		}
	}


	/* Find a legal, distant, unoccupied, space */
	while (attempts_left--)
	{
		/* Pick a location */
		y = randint0(cur_hgt);
		x = randint0(cur_wid);

		/* Require empty floor grid (was "naked") */
		if (dun_level || (wilderness[p_ptr->wilderness_y][p_ptr->wilderness_x].terrain != TERRAIN_MOUNTAIN))
		{
			if (!cave_empty_bold2(y, x)) continue;
		}
		else
		{
			if (!cave_empty_bold2(y, x) && (cave[y][x].feat != FEAT_MOUNTAIN)) continue;
		}

		/* Accept far away grids */
		if (distance(y, x, py, px) > dis) break;
	}

	if (!attempts_left)
	{
		if (cheat_xtra || cheat_hear)
		{
#ifdef JP
msg_print("�ٹ𡪿����ʥ�󥹥��������֤Ǥ��ޤ��󡣾��������Ǥ�����");
#else
			msg_print("Warning! Could not allocate a new monster. Small level?");
#endif

		}

		return (FALSE);
	}


#ifdef MONSTER_HORDES
	if (randint1(5000) <= dun_level)
	{
		if (alloc_horde(y, x))
		{
#ifdef JP
if (cheat_hear) msg_print("��󥹥������緲");
#else
			if (cheat_hear) msg_print("Monster horde.");
#endif

			return (TRUE);
		}
	}
	else
	{
#endif /* MONSTER_HORDES */

		/* Attempt to place the monster, allow groups */
		if (place_monster(y, x, (mode | PM_ALLOW_GROUP))) return (TRUE);

#ifdef MONSTER_HORDES
	}
#endif /* MONSTER_HORDES */

	/* Nope */
	return (FALSE);
}




/*
 * Hack -- help decide if a monster race is "okay" to summon
 */
static bool summon_specific_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Hack - Only summon dungeon monsters */
	if (!monster_dungeon(r_idx)) return (FALSE);

	/* Hack -- identify the summoning monster */
	if (summon_specific_who > 0)
	{
		monster_type *m_ptr = &m_list[summon_specific_who];

		/* Do not summon enemies */

		/* Friendly vs. opposite aligned normal or pet */
		if (((r_ptr->flags3 & RF3_EVIL) &&
			  (m_ptr->sub_align & SUB_ALIGN_GOOD)) ||
			 ((r_ptr->flags3 & RF3_GOOD) &&
			  (m_ptr->sub_align & SUB_ALIGN_EVIL)))
		{
			return FALSE;
		}

		/* Hostile vs. non-hostile */
		if (is_hostile(m_ptr) != summon_specific_hostile)
		{
			return FALSE;
		}
	}
	/* Use the player's alignment */
	else if (summon_specific_who < 0)
	{
		/* Do not summon enemies of the pets */
		if ((p_ptr->align < -9) && (r_ptr->flags3 & RF3_GOOD))
		{
			if (!one_in_((0-p_ptr->align)/2+1)) return FALSE;
		}
		else if ((p_ptr->align > 9) && (r_ptr->flags3 & RF3_EVIL))
		{
			if (!one_in_(p_ptr->align/2+1)) return FALSE;
		}
	}

	/* Hack -- no specific type specified */
	if (!summon_specific_type) return (TRUE);

	if (!summon_unique_okay && ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE_7))) return FALSE;

	if ((summon_specific_who < 0) &&
	    ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE_7)) &&
	    (((p_ptr->align > 9) && (r_ptr->flags3 & RF3_EVIL)) ||
	     ((p_ptr->align < -9) && (r_ptr->flags3 & RF3_GOOD))))
		return FALSE;

	if ((r_ptr->flags7 & RF7_CHAMELEON) && (d_info[dungeon_type].flags1 & DF1_CHAMELEON)) return TRUE;

	return (summon_specific_aux(r_idx));
}


/*
 * Place a monster (of the specified "type") near the given
 * location.  Return TRUE if a monster was actually summoned.
 *
 * We will attempt to place the monster up to 10 times before giving up.
 *
 * Note: SUMMON_UNIQUE and SUMMON_AMBERITES will summon Unique's
 * Note: SUMMON_HI_UNDEAD and SUMMON_HI_DRAGON may summon Unique's
 * Note: None of the other summon codes will ever summon Unique's.
 *
 * This function has been changed.  We now take the "monster level"
 * of the summoning monster as a parameter, and use that, along with
 * the current dungeon level, to help determine the level of the
 * desired monster.  Note that this is an upper bound, and also
 * tends to "prefer" monsters of that level.  Currently, we use
 * the average of the dungeon and monster levels, and then add
 * five to allow slight increases in monster power.
 *
 * Note that we use the new "monster allocation table" creation code
 * to restrict the "get_mon_num()" function to the set of "legal"
 * monsters, making this function much faster and more reliable.
 *
 * Note that this function may not succeed, though this is very rare.
 */
bool summon_specific(int who, int y1, int x1, int lev, int type, u32b mode)
{
	int x, y, r_idx;

	if (p_ptr->inside_arena) return (FALSE);

	if (!mon_scatter(&y, &x, y1, x1, 2)) return FALSE;

	/* Save the summoner */
	summon_specific_who = who;

	/* Save the "summon" type */
	summon_specific_type = type;

	summon_unique_okay = (mode & PM_ALLOW_UNIQUE) ? TRUE : FALSE;

	/* Save the hostility */
	summon_specific_hostile = (!(mode & PM_FORCE_FRIENDLY) && !(is_friendly_idx(who)) && !(mode & PM_FORCE_PET));

	/* Prepare allocation table */
	get_mon_num_prep(summon_specific_okay, get_monster_hook2(y, x));

	/* Pick a monster, using the level calculation */
	r_idx = get_mon_num((dun_level + lev) / 2 + 5);

	/* Handle failure */
	if (!r_idx)
	{
		summon_specific_type = 0;
		return (FALSE);
	}

	if ((type == SUMMON_BLUE_HORROR) || (type == SUMMON_DAWN)) mode |= PM_NO_KAGE;

	/* Attempt to place the monster (awake, allow groups) */
	if (!place_monster_aux(who, y, x, r_idx, mode))
	{
		summon_specific_type = 0;
		return (FALSE);
	}

	summon_specific_type = 0;
	/* Success */
	return (TRUE);
}

/* A "dangerous" function, creates a pet of the specified type */
bool summon_named_creature (int who, int oy, int ox, int r_idx, u32b mode)
{
	int x, y;

	/* Paranoia */
	/* if (!r_idx) return; */

	/* Prevent illegal monsters */
	if (r_idx >= max_r_idx) return FALSE;

	if (p_ptr->inside_arena) return FALSE;

	if (!mon_scatter(&y, &x, oy, ox, 2)) return FALSE;

	/* Place it (allow groups) */
	return place_monster_aux(who, y, x, r_idx, (mode | PM_NO_KAGE));
}


/*
 * Let the given monster attempt to reproduce.
 *
 * Note that "reproduction" REQUIRES empty space.
 */
bool multiply_monster(int m_idx, bool clone, u32b mode)
{
	monster_type	*m_ptr = &m_list[m_idx];

	int y, x;

	if (!mon_scatter(&y, &x, m_ptr->fy, m_ptr->fx, 1))
		return FALSE;

	if (m_ptr->mflag2 & MFLAG_NOPET) mode |= PM_NO_PET;

	/* Create a new monster (awake, no groups) */
	if (!place_monster_aux(m_idx, y, x, m_ptr->r_idx, (mode | PM_NO_KAGE)))
		return FALSE;

	if (clone)
	{
		m_list[hack_m_idx_ii].smart |= SM_CLONED;
		m_list[hack_m_idx_ii].mflag2 |= MFLAG_NOPET;
	}

	return TRUE;
}





/*
 * Dump a message describing a monster's reaction to damage
 *
 * Technically should attempt to treat "Beholder"'s as jelly's
 */
void message_pain(int m_idx, int dam)
{
	long oldhp, newhp, tmp;
	int percentage;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char m_name[80];


	/* Get the monster name */
	monster_desc(m_name, m_ptr, 0);

	/* Notice non-damage */
	if (dam == 0)
	{
#ifdef JP
		msg_format("%^s�ϥ��᡼��������Ƥ��ʤ���", m_name);
#else
		msg_format("%^s is unharmed.", m_name);
#endif

		return;
	}

	/* Note -- subtle fix -CFT */
	newhp = (long)(m_ptr->hp);
	oldhp = newhp + (long)(dam);
	tmp = (newhp * 100L) / oldhp;
	percentage = (int)(tmp);


	/* Mushrooms, Eyes, Jellies, Molds, Vortices, Worms, Quylthulgs */
	if (strchr(",ejmvwQ", r_ptr->d_char))
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^s�ϤۤȤ�ɵ��ˤȤ�Ƥ��ʤ���", m_name);
		else if (percentage > 75)
			msg_format("%^s�Ϥ�����ߤ�����", m_name);
		else if (percentage > 50)
			msg_format("%^s�Ͻ̤��ޤä���", m_name);
		else if (percentage > 35)
			msg_format("%^s���ˤߤ˿̤�����", m_name);
		else if (percentage > 20)
			msg_format("%^s�ϿȤ����������", m_name);
		else if (percentage > 10)
			msg_format("%^s�϶��ˤǿȤ����������", m_name);
		else
			msg_format("%^s�Ϥ��ˤ㤰�ˤ����ڻ������", m_name);
#else
		if (percentage > 95)
			msg_format("%^s barely notices.", m_name);
		else if (percentage > 75)
			msg_format("%^s flinches.", m_name);
		else if (percentage > 50)
			msg_format("%^s squelches.", m_name);
		else if (percentage > 35)
			msg_format("%^s quivers in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s writhes about.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s jerks limply.", m_name);
#endif

	}


	/* Fish */
	else if (strchr("l", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%^s�ϤۤȤ�ɵ��ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s barely notices.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^s�Ϥ�����ߤ�����", m_name);
#else
			msg_format("%^s flinches.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^s���������", m_name);
#else
			msg_format("%^s hesitates.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^s���ˤߤ˿̤�����", m_name);
#else
			msg_format("%^s quivers in pain.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^s�ϿȤ����������", m_name);
#else
			msg_format("%^s writhes about.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^s�϶��ˤǿȤ����������", m_name);
#else
			msg_format("%^s writhes in agony.", m_name);
#endif
		else
#ifdef JP
msg_format("%^s�Ϥ��ˤ㤰�ˤ����ڻ������", m_name);
#else
			msg_format("%^s jerks limply.", m_name);
#endif
	}


	/* Golems, Walls, Doors, Stairs */
	else if (strchr("g#+<>", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%s�Ϲ���˸��򤹤��᤿��", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^s�����ĤΤ褦���ʤ�����", m_name);
#else
			msg_format("%^s roars thunderously.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^s�϶줷�����ʤ�����", m_name);
#else
			msg_format("%^s rumbles.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^s�Ϥ��ᤤ����", m_name);
#else
			msg_format("%^s grunts.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^s���������", m_name);
#else
			msg_format("%^s hesitates.", m_name);
#endif
		else
#ifdef JP
msg_format("%^s�Ϥ����㤯����ˤʤä���", m_name);
#else
			msg_format("%^s crumples.", m_name);
#endif
	}


	/* Snakes, Hydrae, Reptiles, Mimics */
	else if (strchr("JMR", r_ptr->d_char) || !isalpha(r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%^s�ϤۤȤ�ɵ��ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s barely notices.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^s�ϥ����ä��Ĥ�����", m_name);
#else
			msg_format("%^s hisses.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^s���ܤä�Ƭ��夲����", m_name);
#else
			msg_format("%^s rears up in anger.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^s�������ȰҳŤ�����", m_name);
#else
			msg_format("%^s hisses furiously.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^s�ϿȤ����������", m_name);
#else
			msg_format("%^s writhes about.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^s�϶��ˤǿȤ����������", m_name);
#else
			msg_format("%^s writhes in agony.", m_name);
#endif
		else
#ifdef JP
msg_format("%^s�Ϥ��ˤ㤰�ˤ����ڻ������", m_name);
#else
			msg_format("%^s jerks limply.", m_name);
#endif
	}


	/* Felines */
	else if (strchr("f", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���˸��򤹤��᤿��", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^s���ʤ�����", m_name);
#else
			msg_format("%^s roars.", m_name);
#endif
		else if (percentage > 50)
#ifdef JP
msg_format("%^s���ܤä��ʤ�����", m_name);
#else
			msg_format("%^s growls angrily.", m_name);
#endif
		else if (percentage > 35)
#ifdef JP
msg_format("%^s���ˤߤǥ����ä��Ĥ�����", m_name);
#else
			msg_format("%^s hisses with pain.", m_name);
#endif
		else if (percentage > 20)
#ifdef JP
msg_format("%^s���ˤߤǼ塹�����Ĥ�����", m_name);
#else
			msg_format("%^s mewls in pain.", m_name);
#endif
		else if (percentage > 10)
#ifdef JP
msg_format("%^s�϶��ˤˤ��ᤤ����", m_name);
#else
			msg_format("%^s hisses in agony.", m_name);
#endif
		else
#ifdef JP
msg_format("%s�ϰ�����Ĥ�����Ф�����", m_name);
#else
			msg_format("%^s mewls pitifully.", m_name);
#endif
	}


	/* Ants, Centipedes, Flies, Insects, Beetles, Spiders */
	else if (strchr("acFIKS", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif
		else if (percentage > 75)
#ifdef JP
msg_format("%^s�ϥ��������Ĥ�����", m_name);
#else
			msg_format("%^s chitters.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^s�ϥ����ƨ����ä���", m_name);
#else
			msg_format("%^s scuttles about.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^s�Ϥ��뤵���Ĥ�����", m_name);
#else
			msg_format("%^s twitters.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^s���ˤߤ���ڻ������", m_name);
#else
			msg_format("%^s jerks in pain.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^s�϶��ˤ���ڻ������", m_name);
#else
			msg_format("%^s jerks in agony.", m_name);
#endif

		else
#ifdef JP
msg_format("%^s�ϥԥ��ԥ��Ҥ��Ĥä���", m_name);
#else
			msg_format("%^s twitches.", m_name);
#endif

	}


	/* Birds */
	else if (strchr("B", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%^s�Ϥ������ä���", m_name);
#else
			msg_format("%^s chirps.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%^s�ϥԡ��ԡ��Ĥ�����", m_name);
#else
			msg_format("%^s twitters.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^s�ϥ��㡼���㡼�Ĥ�����", m_name);
#else
			msg_format("%^s squawks.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^s�ϥ��㡼���㡼�Ĥ���ᤤ����", m_name);
#else
			msg_format("%^s chatters.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^s�϶줷�����", m_name);
#else
			msg_format("%^s jeers.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^s�ϤΤ�������ä���", m_name);
#else
			msg_format("%^s flutters about.", m_name);
#endif

		else
#ifdef JP
msg_format("%^s�ϥ����������Ĥ��������", m_name);
#else
			msg_format("%^s squeaks.", m_name);
#endif

	}


	/* Dragons, Demons, High Undead */
	else if (strchr("duDLUW", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%^s�Ϥ�����ߤ�����", m_name);
#else
			msg_format("%^s flinches.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^s���ˤߤǥ����ä��Ĥ�����", m_name);
#else
			msg_format("%^s hisses in pain.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^s���ˤߤǤ��ʤä���", m_name);
#else
			msg_format("%^s snarls with pain.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^s���ˤߤ��ʤ�����", m_name);
#else
			msg_format("%^s roars with pain.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^s�϶줷���˶������", m_name);
#else
			msg_format("%^s gasps.", m_name);
#endif

		else
#ifdef JP
msg_format("%^s�ϼ塹�������ʤä���", m_name);
#else
			msg_format("%^s snarls feebly.", m_name);
#endif

	}


	/* Skeletons */
	else if (strchr("s", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%s�Ϲ���˸��򤹤��᤿��", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^s�ϥ��������ȾФä���", m_name);
#else
			msg_format("%^s rattles.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^s�Ϥ��ᤤ����", m_name);
#else
			msg_format("%^s stumbles.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^s�ϥ����������ä���", m_name);
#else
			msg_format("%^s rattles.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^s�Ϥ��ᤤ����", m_name);
#else
			msg_format("%^s staggers.", m_name);
#endif

		else
#ifdef JP
msg_format("%^s�ϥ����������ä���", m_name);
#else
			msg_format("%^s clatters.", m_name);
#endif

	}


	/* Zombies */
	else if (strchr("z", r_ptr->d_char))
	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%s�Ϲ���˸��򤹤��᤿��", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%^s�Ϥ��ᤤ����", m_name);
#else
			msg_format("%^s groans.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%s�϶줷���ˤ��ᤤ����", m_name);
#else
			msg_format("%^s moans.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^s���������", m_name);
#else
			msg_format("%^s hesitates.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%^s�Ϥ��ʤä���", m_name);
#else
			msg_format("%^s grunts.", m_name);
#endif

		else
#ifdef JP
msg_format("%^s�Ϥ��ᤤ����", m_name);
#else
			msg_format("%^s staggers.", m_name);
#endif

	}


	/* Ghosts */
	else if (strchr("G", r_ptr->d_char))

	{
		if (percentage > 95)
#ifdef JP
msg_format("%s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
#else
			msg_format("%^s ignores the attack.", m_name);
#endif

		else if (percentage > 75)
#ifdef JP
msg_format("%s�Ϲ���˸��򤹤��᤿��", m_name);
#else
			msg_format("%^s shrugs off the attack.", m_name);
#endif

		else if (percentage > 50)
#ifdef JP
msg_format("%s�Ϥ��ᤤ����", m_name);
#else
			msg_format("%^s moans.", m_name);
#endif

		else if (percentage > 35)
#ifdef JP
msg_format("%^s�ϵ㤭��ᤤ����", m_name);
#else
			msg_format("%^s wails.", m_name);
#endif

		else if (percentage > 20)
#ifdef JP
msg_format("%^s���ʤ�����", m_name);
#else
			msg_format("%^s howls.", m_name);
#endif

		else if (percentage > 10)
#ifdef JP
msg_format("%s�ϼ塹�������ᤤ����", m_name);
#else
			msg_format("%^s moans softly.", m_name);
#endif

		else
#ifdef JP
msg_format("%^s�Ϥ������ˤ��ᤤ����", m_name);
#else
			msg_format("%^s sighs.", m_name);
#endif

	}


	/* Dogs and Hounds */
	else if (strchr("CZ", r_ptr->d_char))
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^s�Ϲ���˸��򤹤��᤿��", m_name);
		else if (percentage > 75)
			msg_format("%^s���ˤߤǤ��ʤä���", m_name);
		else if (percentage > 50)
			msg_format("%^s���ˤߤǥ���󥭥���ʤ�����", m_name);
		else if (percentage > 35)
			msg_format("%^s���ˤߤ��Ĥ���ᤤ����", m_name);
		else if (percentage > 20)
			msg_format("%^s�϶��ˤΤ��ޤ��Ĥ���ᤤ����", m_name);
		else if (percentage > 10)
			msg_format("%^s�϶��ˤǤ�����줷�����", m_name);
		else
			msg_format("%^s�ϼ塹�����ʤ�����", m_name);
#else
		if (percentage > 95)
			msg_format("%^s shrugs off the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s snarls with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s yelps in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s howls in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s howls in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s yelps feebly.", m_name);
#endif

	}

	/* One type of monsters (ignore,squeal,shriek) */
	else if (strchr("Xbilqrt", r_ptr->d_char))
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^s�Ϲ���򵤤ˤȤ�Ƥ��ʤ���", m_name);
		else if (percentage > 75)
			msg_format("%^s���ˤߤǤ��ʤä���", m_name);
		else if (percentage > 50)
			msg_format("%^s���ˤߤǶ������", m_name);
		else if (percentage > 35)
			msg_format("%^s���ˤߤ��䶫������", m_name);
		else if (percentage > 20)
			msg_format("%^s�϶��ˤΤ��ޤ��䶫������", m_name);
		else if (percentage > 10)
			msg_format("%^s�϶��ˤǤ�����줷�����", m_name);
		else
			msg_format("%^s�ϼ塹�����������", m_name);
#else
		if (percentage > 95)
			msg_format("%^s ignores the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s grunts with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s squeals in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s shrieks in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s shrieks in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s cries out feebly.", m_name);
#endif

	}

	/* Another type of monsters (shrug,cry,scream) */
	else
	{
#ifdef JP
		if (percentage > 95)
			msg_format("%^s�Ϲ���˸��򤹤��᤿��", m_name);
		else if (percentage > 75)
			msg_format("%^s���ˤߤǤ��ʤä���", m_name);
		else if (percentage > 50)
			msg_format("%^s���ˤߤǶ������", m_name);
		else if (percentage > 35)
			msg_format("%^s���ˤߤ��䶫������", m_name);
		else if (percentage > 20)
			msg_format("%^s�϶��ˤΤ��ޤ��䶫������", m_name);
		else if (percentage > 10)
			msg_format("%^s�϶��ˤǤ�����줷�����", m_name);
		else
			msg_format("%^s�ϼ塹�����������", m_name);
#else
		if (percentage > 95)
			msg_format("%^s shrugs off the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s grunts with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s cries out in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s screams in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s screams in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s cries out feebly.", m_name);
#endif

	}
}


/*
 * Learn about an "observed" resistance.
 */
void update_smart_learn(int m_idx, int what)
{
#ifdef DRS_SMART_OPTIONS

	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];


	/* Not allowed to learn */
	if (!smart_learn) return;

	/* Too stupid to learn anything */
	if (r_ptr->flags2 & (RF2_STUPID)) return;

	/* Not intelligent, only learn sometimes */
	if (!(r_ptr->flags2 & (RF2_SMART)) && (randint0(100) < 50)) return;


	/* XXX XXX XXX */

	/* Analyze the knowledge */
	switch (what)
	{
	case DRS_ACID:
		if (p_ptr->resist_acid) m_ptr->smart |= (SM_RES_ACID);
		if (p_ptr->oppose_acid) m_ptr->smart |= (SM_OPP_ACID);
		if (p_ptr->immune_acid) m_ptr->smart |= (SM_IMM_ACID);
		break;

	case DRS_ELEC:
		if (p_ptr->resist_elec) m_ptr->smart |= (SM_RES_ELEC);
		if (p_ptr->oppose_elec) m_ptr->smart |= (SM_OPP_ELEC);
		if (p_ptr->immune_elec) m_ptr->smart |= (SM_IMM_ELEC);
		break;

	case DRS_FIRE:
		if (p_ptr->resist_fire) m_ptr->smart |= (SM_RES_FIRE);
		if (p_ptr->oppose_fire) m_ptr->smart |= (SM_OPP_FIRE);
		if (p_ptr->immune_fire) m_ptr->smart |= (SM_IMM_FIRE);
		break;

	case DRS_COLD:
		if (p_ptr->resist_cold) m_ptr->smart |= (SM_RES_COLD);
		if (p_ptr->oppose_cold) m_ptr->smart |= (SM_OPP_COLD);
		if (p_ptr->immune_cold) m_ptr->smart |= (SM_IMM_COLD);
		break;

	case DRS_POIS:
		if (p_ptr->resist_pois) m_ptr->smart |= (SM_RES_POIS);
		if (p_ptr->oppose_pois) m_ptr->smart |= (SM_OPP_POIS);
		break;


	case DRS_NETH:
		if (p_ptr->resist_neth) m_ptr->smart |= (SM_RES_NETH);
		break;

	case DRS_LITE:
		if (p_ptr->resist_lite) m_ptr->smart |= (SM_RES_LITE);
		break;

	case DRS_DARK:
		if (p_ptr->resist_dark) m_ptr->smart |= (SM_RES_DARK);
		break;

	case DRS_FEAR:
		if (p_ptr->resist_fear) m_ptr->smart |= (SM_RES_FEAR);
		break;

	case DRS_CONF:
		if (p_ptr->resist_conf) m_ptr->smart |= (SM_RES_CONF);
		break;

	case DRS_CHAOS:
		if (p_ptr->resist_chaos) m_ptr->smart |= (SM_RES_CHAOS);
		break;

	case DRS_DISEN:
		if (p_ptr->resist_disen) m_ptr->smart |= (SM_RES_DISEN);
		break;

	case DRS_BLIND:
		if (p_ptr->resist_blind) m_ptr->smart |= (SM_RES_BLIND);
		break;

	case DRS_NEXUS:
		if (p_ptr->resist_nexus) m_ptr->smart |= (SM_RES_NEXUS);
		break;

	case DRS_SOUND:
		if (p_ptr->resist_sound) m_ptr->smart |= (SM_RES_SOUND);
		break;

	case DRS_SHARD:
		if (p_ptr->resist_shard) m_ptr->smart |= (SM_RES_SHARD);
		break;

	case DRS_FREE:
		if (p_ptr->free_act) m_ptr->smart |= (SM_IMM_FREE);
		break;

	case DRS_MANA:
		if (!p_ptr->msp) m_ptr->smart |= (SM_IMM_MANA);
		break;

	case DRS_REFLECT:
		if (p_ptr->reflect) m_ptr-> smart |= (SM_IMM_REFLECT);
		break;
	}

#endif /* DRS_SMART_OPTIONS */

}


/*
 * Place the player in the dungeon XXX XXX
 */
bool player_place(int y, int x)
{
	/* Paranoia XXX XXX */
	if (cave[y][x].m_idx != 0) return FALSE;

	/* Save player location */
	py = y;
	px = x;

	/* Success */
	return TRUE;
}


/*
 * Drop all items carried by a monster
 */
void monster_drop_carried_objects(monster_type *m_ptr)
{
	s16b this_o_idx, next_o_idx = 0;
	object_type forge;
	object_type *o_ptr;
	object_type *q_ptr;


	/* Drop objects being carried */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Paranoia */
		o_ptr->held_m_idx = 0;

		/* Get local object */
		q_ptr = &forge;

		/* Copy the object */
		object_copy(q_ptr, o_ptr);

		/* Delete the object */
		delete_object_idx(this_o_idx);

		/* Drop it */
		(void)drop_near(q_ptr, -1, m_ptr->fy, m_ptr->fx);
	}

	/* Forget objects */
	m_ptr->hold_o_idx = 0;
}
