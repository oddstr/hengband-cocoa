/* File: mspells2.c */

/* Purpose: Monster spells (attack monster) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Monster casts a breath (or ball) attack at another monster.
 * Pass over any monsters that may be in the way
 * Affect grids, objects, monsters, and the player
 */
static void monst_breath_monst(int m_idx, int y, int x, int typ, int dam_hp, int rad, bool breath, int monspell, bool learnable)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	/* Determine the radius of the blast */
	if (rad < 1 && breath) rad = (r_ptr->flags2 & RF2_POWERFUL) ? 3 : 2;

	/* Handle breath attacks */
	if (breath) rad = 0 - rad;

	if (typ == GF_ROCKET) flg |= PROJECT_STOP;

	(void)project(m_idx, rad, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}


/*
 * Monster casts a bolt at another monster
 * Stop if we hit a monster
 * Affect monsters and the player
 */
static void monst_bolt_monst(int m_idx, int y, int x, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;

	(void)project(m_idx, 0, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

static void monst_beam_monst(int m_idx, int y, int x, int typ, int dam_hp, int monspell, bool learnable)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU;

	(void)project(m_idx, 0, y, x, dam_hp, typ, flg, (learnable ? monspell : -1));
}

/*
 * Determine if a beam spell will hit the target.
 */
static bool direct_beam(int y1, int x1, int y2, int x2, monster_type *m_ptr)
{
	bool hit2 = FALSE;
	int i, y, x;

	int grid_n = 0;
	u16b grid_g[512];

	bool friend = is_pet(m_ptr);

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, PROJECT_THRU);

	/* No grid is ever projectable from itself */
	if (!grid_n) return (FALSE);

	for (i = 0; i < grid_n; i++)
	{
		y = GRID_Y(grid_g[i]);
		x = GRID_X(grid_g[i]);

		if (y == y2 && x == x2)
			hit2 = TRUE;
		else if (friend && cave[y][x].m_idx > 0 &&
			 !are_enemies(m_ptr, &m_list[cave[y][x].m_idx]))
		{
			/* Friends don't shoot friends */
			return FALSE;
		}

		if (friend && y == py && x == px)
			return FALSE;
	}
	if (!hit2)
		return FALSE;
	return TRUE;
}

static bool breath_direct(int y1, int x1, int y2, int x2, int rad, bool disint_ball, bool friend)
{
	/* Must be the same as projectable() */

	int i;

	/* Initial grid */
	int y = y1;
	int x = x1;

	int grid_n = 0;
	u16b grid_g[512];

	int grids = 0;
	byte gx[1024], gy[1024];
	byte gm[32];
	int gm_rad = rad;

	bool hit2 = FALSE;
	bool hityou = FALSE;

	/* Check the projection path */
	grid_n = project_path(grid_g, MAX_RANGE, y1, x1, y2, x2, disint_ball ? PROJECT_DISI : 0);

	/* Project along the path */
	for (i = 0; i < grid_n; ++i)
	{
		int ny = GRID_Y(grid_g[i]);
		int nx = GRID_X(grid_g[i]);

		if (disint_ball)
		{
			/* Hack -- Balls explode before reaching walls */
			if (cave_stop_disintegration(ny, nx)) break;
		}
		else
		{
			/* Hack -- Balls explode before reaching walls */
			if (!cave_floor_bold(ny, nx)) break;
		}

		/* Save the "blast epicenter" */
		y = ny;
		x = nx;
	}

	grid_n = i;

	if (!grid_n)
	{
		if (disint_ball)
		{
			if (in_disintegration_range(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (in_disintegration_range(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
		else
		{
			if (los(y1, x1, y2, x2) && (distance(y1, x1, y2, x2) <= rad)) hit2 = TRUE;
			if (los(y1, x1, py, px) && (distance(y1, x1, py, px) <= rad)) hityou = TRUE;
		}
	}
	else
	{
		breath_shape(grid_g, grid_n, &grids, gx, gy, gm, &gm_rad, rad, y1, x1, y, x, disint_ball, FALSE);

		for (i = 0; i < grids; i++)
		{
			/* Extract the location */
			y = gy[i];
			x = gx[i];

			if ((y == y2) && (x == x2)) hit2 = TRUE;
			if ((y == py) && (x == px)) hityou = TRUE;
		}
	}

	if (!hit2) return FALSE;
	if (friend && hityou) return FALSE;

	return TRUE;
}

/*
 * Get the actual center point of ball spells (rad > 1) (originally from TOband)
 */
static void get_project_point(int sy, int sx, int *ty, int *tx, int flg)
{
	u16b path_g[128];
	int  path_n, i;

	path_n = project_path(path_g, MAX_RANGE, sy, sx, *ty, *tx, flg);

	*ty = sy;
	*tx = sx;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		sy = GRID_Y(path_g[i]);
		sx = GRID_X(path_g[i]);

		/* Hack -- Balls explode before reaching walls */
		if (!cave_floor_bold(sy, sx)) break;

		*ty = sy;
		*tx = sx;
	}
}

/*
 * Monster tries to 'cast a spell' (or breath, etc)
 * at another monster.
 *
 * The player is only disturbed if able to be affected by the spell.
 */
bool monst_spell_monst(int m_idx)
{
	int y = 0, x = 0;
	int i, k, t_idx;
	int chance, thrown_spell, count = 0;
	int rlev;
	int dam = 0;
	int start;
	int plus = 1;
	u32b p_mode = 0L, u_mode = 0L;
	int s_num_6 = (easy_band ? 2 : 6);
	int s_num_4 = (easy_band ? 1 : 4);

	byte spell[96], num = 0;

	char m_name[160];
	char t_name[160];
	char m_poss[160];
	char ddesc[160];

	monster_type *m_ptr = &m_list[m_idx];
	monster_type *t_ptr;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_race *tr_ptr;

	u32b f4, f5, f6;

	bool wake_up = FALSE;
	bool fear = FALSE;

	bool blind = (p_ptr->blind ? TRUE : FALSE);

	bool see_m = m_ptr->ml;
	bool maneable = player_has_los_bold(m_ptr->fy, m_ptr->fx);
	bool learnable = (see_m && maneable && !world_monster);
	bool see_t;
	bool see_either;
	bool see_both;
	bool known;

	bool pet = is_pet(m_ptr);

	/* Prepare flags for summoning */
	if (pet) p_mode |= PM_FORCE_PET;
	if (!pet) u_mode |= PM_ALLOW_UNIQUE;

	/* Cannot cast spells when confused */
	if (m_ptr->confused) return (FALSE);

	/* Hack -- Extract the spell probability */
	chance = (r_ptr->freq_inate + r_ptr->freq_spell) / 2;

	/* Not allowed to cast spells */
	if (!chance) return (FALSE);

	if (randint0(100) >= chance) return (FALSE);

	if (p_ptr->inside_battle)
	{
		start = randint1(m_max-1)+m_max;
		if(randint0(2)) plus = -1;
	}
	else start = m_max + 1;

	/* Scan thru all monsters */
	for (i = start; ((i < start + m_max) && (i > start - m_max)); i+=plus)
	{
		/* The monster itself isn't a target */
		int dummy = (i % m_max);
		if (!dummy) continue;

		t_idx = dummy;
		t_ptr = &m_list[t_idx];
		tr_ptr = &r_info[t_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!t_ptr->r_idx) continue;

		if (pet)
		{
			if (pet_t_m_idx && (dummy != pet_t_m_idx) && !los(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx)) continue;
		}

		/* Monster must be 'an enemy' */
		if (!are_enemies(m_ptr, t_ptr)) continue;

		/* Extract the racial spell flags */
		f4 = r_ptr->flags4;
		f5 = r_ptr->flags5;
		f6 = r_ptr->flags6;

		/* Monster must be projectable */
		if (!projectable(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx))
		{
			bool success = FALSE;
			if (m_ptr->target_y)
			{
				if ((m_ptr->target_y == t_ptr->fy) && (m_ptr->target_x == t_ptr->fx))
				{
					y = m_ptr->target_y;
					x = m_ptr->target_x;
					f4 &= (RF4_INDIRECT_MASK);
					f5 &= (RF5_INDIRECT_MASK);
					f6 &= (RF6_INDIRECT_MASK);
					success = TRUE;
				}
			}
			if (!success) continue;
		}

		reset_target(m_ptr);

		/* OK -- we've got a target */
		y = t_ptr->fy;
		x = t_ptr->fx;

		/* Extract the monster level */
		rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

		/* Remove unimplemented spells */
		f4 &= ~(RF4_DISPEL);
		f6 &= ~(RF6_WORLD | RF6_FORGET);

		/* Remove unimplemented special moves */
		if (f6 & RF6_SPECIAL)
		{
			if (r_ptr->d_char != 'B') f6 &= ~(RF6_SPECIAL);
		}

		if (pet)
		{
			f4 &= ~(RF4_SHRIEK);
			f6 &= ~(RF6_DARKNESS | RF6_TRAPS);
		}

		if (dun_level && (!p_ptr->inside_quest || (p_ptr->inside_quest < MIN_RANDOM_QUEST)) && (d_info[dungeon_type].flags1 & DF1_NO_MAGIC))
		{
			f4 &= (RF4_NOMAGIC_MASK);
			f5 &= (RF5_NOMAGIC_MASK);
			f6 &= (RF6_NOMAGIC_MASK);
		}

		if (p_ptr->inside_arena || p_ptr->inside_battle)
		{
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
		}
		if (p_ptr->inside_battle && !one_in_(3))
		{
			f6 &= ~(RF6_HEAL);
		}

		if (!(p_ptr->pet_extra_flags & PF_TELEPORT) && pet)
		{
			f6 &= ~((RF6_BLINK | RF6_TPORT | RF6_TELE_AWAY));
		}

		if (m_idx == p_ptr->riding)
		{
			f4 &= ~(RF4_RIDING_MASK);
			f5 &= ~(RF5_RIDING_MASK);
			f6 &= ~(RF6_RIDING_MASK);
		}

		if (!(p_ptr->pet_extra_flags & PF_ATTACK_SPELL) && pet)
		{
			f4 &= ~(RF4_ATTACK_MASK);
			f5 &= ~(RF5_ATTACK_MASK);
			f6 &= ~(RF6_ATTACK_MASK);
		}

		if (!(p_ptr->pet_extra_flags & PF_SUMMON_SPELL) && pet)
		{
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
		}

		/* Prevent collateral damage */
		if (!(p_ptr->pet_extra_flags & PF_BALL_SPELL) && pet && (m_idx != p_ptr->riding))
		{
			if ((f4 & (RF4_BALL_MASK & ~(RF4_ROCKET))) ||
			    (f5 & RF5_BALL_MASK) ||
			    (f6 & RF6_BALL_MASK))
			{
				int real_y = y;
				int real_x = x;

				get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, 0L);

				if (los(real_y, real_x, py, px))
				{
					int dist = distance(real_y, real_x, py, px);

					if (dist <= 2)
					{
						f4 &= ~(RF4_BALL_MASK & ~(RF4_ROCKET));
						f5 &= ~(RF5_BALL_MASK);
						f6 &= ~(RF6_BALL_MASK);
					}
					else if (dist <= 4)
					{
						f4 &= ~(RF4_BIG_BALL_MASK);
						f5 &= ~(RF5_BIG_BALL_MASK);
						f6 &= ~(RF6_BIG_BALL_MASK);
					}
				}
			}

			if (f4 & RF4_ROCKET)
			{
				int real_y = y;
				int real_x = x;

				get_project_point(m_ptr->fy, m_ptr->fx, &real_y, &real_x, PROJECT_STOP);
				if (los(real_y, real_x, py, px) && (distance(real_y, real_x, py, px) <= 2))
					f4 &= ~(RF4_ROCKET);
			}

			if (((f4 & RF4_BEAM_MASK) ||
			     (f5 & RF5_BEAM_MASK) ||
			     (f6 & RF6_BEAM_MASK)) &&
			    !direct_beam(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, m_ptr))
			{
				f4 &= ~(RF4_BEAM_MASK);
				f5 &= ~(RF5_BEAM_MASK);
				f6 &= ~(RF6_BEAM_MASK);
			}

			if ((f4 & RF4_BREATH_MASK) ||
			    (f5 & RF5_BREATH_MASK) ||
			    (f6 & RF6_BREATH_MASK))
			{
				/* Expected breath radius */
				int rad = (r_ptr->flags2 & RF2_POWERFUL) ? 3 : 2;

				if (!breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, FALSE, TRUE))
				{
					f4 &= ~(RF4_BREATH_MASK);
					f5 &= ~(RF5_BREATH_MASK);
					f6 &= ~(RF6_BREATH_MASK);
				}
				else if ((f4 & RF4_BR_DISI) &&
					 !breath_direct(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, rad, TRUE, TRUE))
				{
					f4 &= ~(RF4_BR_DISI);
				}
			}
		}

		/* Remove some spells if necessary */

		/* Check for a clean bolt shot */
		if (((f4 & RF4_BOLT_MASK) ||
		     (f5 & RF5_BOLT_MASK) ||
		     (f6 & RF6_BOLT_MASK)) &&
		    !(r_ptr->flags2 & RF2_STUPID) &&
		    !clean_shot(m_ptr->fy, m_ptr->fx, t_ptr->fy, t_ptr->fx, pet))
		{
			f4 &= ~(RF4_BOLT_MASK);
			f5 &= ~(RF5_BOLT_MASK);
			f6 &= ~(RF6_BOLT_MASK);
		}

		/* Check for a possible summon */
		if (((f4 & RF4_SUMMON_MASK) ||
		     (f5 & RF5_SUMMON_MASK) ||
		     (f6 & RF6_SUMMON_MASK)) &&
		    !(r_ptr->flags2 & RF2_STUPID) &&
		    !(summon_possible(t_ptr->fy, t_ptr->fx)))
		{
			/* Remove summoning spells */
			f4 &= ~(RF4_SUMMON_MASK);
			f5 &= ~(RF5_SUMMON_MASK);
			f6 &= ~(RF6_SUMMON_MASK);
		}

		/* Hack -- allow "desperate" spells */
		if ((r_ptr->flags2 & RF2_SMART) &&
			(m_ptr->hp < m_ptr->maxhp / 10) &&
			(randint0(100) < 50))
		{
			/* Require intelligent spells */
			f4 &= (RF4_INT_MASK);
			f5 &= (RF5_INT_MASK);
			f6 &= (RF6_INT_MASK);

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

		/* Handle "leaving" */
		if (p_ptr->leaving) return (FALSE);

		/* Get the monster name (or "it") */
		monster_desc(m_name, m_ptr, 0x00);

		/* Get the monster possessive ("his"/"her"/"its") */
		monster_desc(m_poss, m_ptr, 0x22);

		/* Get the target's name (or "it") */
		monster_desc(t_name, t_ptr, 0x00);

		/* Hack -- Get the "died from" name */
		monster_desc(ddesc, m_ptr, 0x88);

		/* Choose a spell to cast */
		thrown_spell = spell[randint0(num)];

		see_t = t_ptr->ml;
		see_either = (see_m || see_t);
		see_both = (see_m && see_t);

		/* Can the player be aware of this attack? */
		known = (m_ptr->cdis <= MAX_SIGHT) || (t_ptr->cdis <= MAX_SIGHT);

		if (p_ptr->riding && (m_idx == p_ptr->riding)) disturb(1, 0);

		/* Check for spell failure (inate attacks never fail) */
		if ((thrown_spell >= 128) && m_ptr->stunned && one_in_(2))
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

		switch (thrown_spell)
		{
			/* RF4_SHRIEK */
			case 96+0:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƶ������", m_name, t_name);
#else
						msg_format("%^s shrieks at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				wake_up = TRUE;

				break;
			}

			/* RF4_XXX1 */
			case 96+1:
			{
				/* XXX XXX XXX */
				return FALSE;
			}

			/* RF4_DISPEL */
			case 96+2:
			{
				return FALSE;
			}

			/* RF4_ROCKET */
			case 96+3:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s��������ͤä���", m_name);
#else
							msg_format("%^s shoots something.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˥��åȤ�ȯ�ͤ�����", m_name, t_name);
#else
							msg_format("%^s fires a rocket at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = ((m_ptr->hp / 4) > 800 ? 800 : (m_ptr->hp / 4));
				monst_breath_monst(m_idx, y, x, GF_ROCKET,
					dam, 2, FALSE, MS_ROCKET, learnable);

				break;
			}

			/* RF4_SHOOT */
			case 96+4:
			{
				if (known)
				{
					if (see_either)
					{
						if (blind)
						{
#ifdef JP
msg_format("%^s����̯�ʲ���ȯ������", m_name);
#else
							msg_format("%^s makes a strange noise.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s��������ä���", m_name, t_name);
#else
							msg_format("%^s fires an arrow at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_SHOOT);
				}

				dam = damroll(r_ptr->blow[0].d_dice, r_ptr->blow[0].d_side);
				monst_bolt_monst(m_idx, y, x, GF_ARROW, dam, MS_SHOOT, learnable);

				break;
			}

			/* RF4_XXX2 */
			case 96+5:
			{
				/* XXX XXX XXX */
				return FALSE;
			}

			/* RF4_XXX3 */
			case 96+6:
			{
				/* XXX XXX XXX */
				return FALSE;
			}

			/* RF4_XXX4 */
			case 96+7:
			{
				/* XXX XXX XXX */
				return FALSE;
			}

			/* RF4_BR_ACID */
			case 96+8:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˻��Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes acid at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_ACID,
					dam,0, TRUE, MS_BR_ACID, learnable);

				break;
			}

			/* RF4_BR_ELEC */
			case 96+9:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˰�ʤΥ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes lightning at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_ELEC,
					dam,0, TRUE, MS_BR_ELEC, learnable);

				break;
			}

			/* RF4_BR_FIRE */
			case 96+10:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˲б�Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes fire at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_FIRE,
					dam,0, TRUE, MS_BR_FIRE, learnable);

				break;
			}

			/* RF4_BR_COLD */
			case 96+11:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s���䵤�Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes frost at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 1600 ? 1600 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_COLD,
					dam,0, TRUE, MS_BR_COLD, learnable);
				break;
			}

			/* RF4_BR_POIS */
			case 96+12:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˥����Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes gas at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_POIS,
					dam,0, TRUE, MS_BR_POIS, learnable);

				break;
			}

			/* RF4_BR_NETH */
			case 96+13:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s���Ϲ��Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes nether at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 550 ? 550 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_NETHER,
					dam,0, TRUE, MS_BR_NETHER, learnable);

				break;
			}

			/* RF4_BR_LITE */
			case 96+14:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�������Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes light at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_LITE,
					dam,0, TRUE, MS_BR_LITE, learnable);

				break;
			}

			/* RF4_BR_DARK */
			case 96+15:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˰Ź��Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes darkness at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 400 ? 400 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_DARK,
					dam,0, TRUE, MS_BR_DARK, learnable);

				break;
			}

			/* RF4_BR_CONF */
			case 96+16:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˺���Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes confusion at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_CONFUSION,
					dam,0, TRUE, MS_BR_CONF, learnable);

				break;
			}

			/* RF4_BR_SOUN */
			case 96+17:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (m_ptr->r_idx == MON_JAIAN)
#ifdef JP
							msg_format("�֥ܥ�����������������");
#else
							msg_format("'Booooeeeeee'");
#endif
						else if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˹첻�Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes sound at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 450 ? 450 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_SOUND,
					dam,0, TRUE, MS_BR_SOUND, learnable);

				break;
			}

			/* RF4_BR_CHAO */
			case 96+18:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˥������Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes chaos at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 600 ? 600 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_CHAOS,
					dam,0, TRUE, MS_BR_CHAOS, learnable);

				break;
			}

			/* RF4_BR_DISE */
			case 96+19:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�������Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes disenchantment at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_DISENCHANT,
					dam,0, TRUE, MS_BR_DISEN, learnable);

				break;
			}

			/* RF4_BR_NEXU */
			case 96+20:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˰��̺���Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes nexus at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_NEXUS,
					dam,0, TRUE, MS_BR_NEXUS, learnable);

				break;
			}

			/* RF4_BR_TIME */
			case 96+21:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˻��ֵ�ž�Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes time at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 150 ? 150 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_TIME,
					dam,0, TRUE, MS_BR_TIME, learnable);

				break;
			}

			/* RF4_BR_INER */
			case 96+22:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�����ߤΥ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes inertia at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_INERTIA,
					dam,0, TRUE, MS_BR_INERTIA, learnable);

				break;
			}

			/* RF4_BR_GRAV */
			case 96+23:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˽��ϤΥ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes gravity at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 200 ? 200 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_GRAVITY,
					dam,0, TRUE, MS_BR_GRAVITY, learnable);

				break;
			}

			/* RF4_BR_SHAR */
			case 96+24:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (m_ptr->r_idx == MON_BOTEI)
#ifdef JP
							msg_format("�֥���ӥ륫�å�����������");
#else
							msg_format("'Boty-Build cutter!!!'");
#endif
						else if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�����ҤΥ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes shards at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 500 ? 500 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_SHARDS,
					dam,0, TRUE, MS_BR_SHARDS, learnable);

				break;
			}

			/* RF4_BR_PLAS */
			case 96+25:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˥ץ饺�ޤΥ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes plasma at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_PLASMA,
					dam,0, TRUE, MS_BR_PLASMA, learnable);

				break;
			}

			/* RF4_BR_WALL */
			case 96+26:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˥ե������Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes force at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 200 ? 200 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_FORCE,
					dam,0, TRUE, MS_BR_FORCE, learnable);
				break;
			}

			/* RF4_BR_MANA */
			case 96+27:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�����ϤΥ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes mana at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 250 ? 250 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_MANA,
					dam,0, TRUE, MS_BR_MANA, learnable);

				break;
			}

			/* RF4_BA_NUKE */
			case 96+28:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s������ǽ������ä���", m_name, t_name);
#else
							msg_format("%^s casts a ball of radiation at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (rlev + damroll(10, 6));
				monst_breath_monst(m_idx, y, x, GF_NUKE,
					dam, 2, FALSE, MS_BALL_NUKE, learnable);

				break;
			}

			/* RF4_BR_NUKE */
			case 96+29:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s���������Ѵ�ʪ�Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes toxic waste at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 3) > 800 ? 800 : (m_ptr->hp / 3));
				monst_breath_monst(m_idx, y, x, GF_NUKE,
					dam,0, TRUE, MS_BR_NUKE, learnable);
				break;
			}

			/* RF4_BA_CHAO */
			case 96+30:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s���������ˤĤ֤䤤����", m_name);
#else
							msg_format("%^s mumbles frighteningly.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s�˽���륹�����ä���", m_name, t_name);
#else
							msg_format("%^s invokes raw Logrus upon %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (rlev * 2) + damroll(10, 10);
				monst_breath_monst(m_idx, y, x, GF_CHAOS,
					dam, 4, FALSE, MS_BALL_CHAOS, learnable);

				break;
			}

			/* RF4_BR_DISI */
			case 96+31:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
						{
#ifdef JP
msg_format("%^s�������Υ֥쥹���Ǥ�����", m_name);
#else
							msg_format("%^s breathes.", m_name);
#endif

						}
						else
						{
#ifdef JP
msg_format("%^s��%s��ʬ��Υ֥쥹���Ǥ�����", m_name, t_name);
#else
							msg_format("%^s breathes disintegration at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}

					sound(SOUND_BREATH);
				}

				dam = ((m_ptr->hp / 6) > 150 ? 150 : (m_ptr->hp / 6));
				monst_breath_monst(m_idx, y, x, GF_DISINTEGRATE,
					dam,0, TRUE, MS_BR_DISI, learnable);
				break;
			}

			/* RF5_BA_ACID */
			case 128+0:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s�˸����äƥ����åɡ��ܡ���μ�ʸ�򾧤�����", m_name, t_name);
#else
							msg_format("%^s casts an acid ball at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (randint1(rlev * 3) + 15) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_breath_monst(m_idx, y, x, GF_ACID, dam, 2, FALSE, MS_BALL_ACID, learnable);

				break;
			}

			/* RF5_BA_ELEC */
			case 128+1:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s�˸����äƥ���������ܡ���μ�ʸ�򾧤�����", m_name, t_name);
#else
							msg_format("%^s casts a lightning ball at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (randint1(rlev * 3 / 2) + 8) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_breath_monst(m_idx, y, x, GF_ELEC, dam, 2, FALSE, MS_BALL_ELEC, learnable);

				break;
			}

			/* RF5_BA_FIRE */
			case 128+2:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (m_ptr->r_idx == MON_ROLENTO)
						{
#ifdef JP
if (blind)
	msg_format("%^s���������ꤲ����", m_name);
else
	msg_format("%^s��%^s�˸����äƼ����Ƥ��ꤲ����", m_name, t_name);
#else
if (blind)
	msg_format("%^s throws something.", m_name);
else
	msg_format("%^s throws a hand grenade.", m_name);
#endif
						}
						else
						{
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
msg_format("%^s��%s�˸����äƥե��������ܡ���μ�ʸ�򾧤�����", m_name, t_name);
#else
								msg_format("%^s casts a fire ball at %s.", m_name, t_name);
#endif

							}
						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (randint1(rlev * 7 / 2) + 10) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_breath_monst(m_idx, y, x, GF_FIRE, dam, 2, FALSE, MS_BALL_FIRE, learnable);

				break;
			}

			/* RF5_BA_COLD */
			case 128+3:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s�˸����äƥ��������ܡ���μ�ʸ�򾧤�����", m_name, t_name);
#else
							msg_format("%^s casts a frost ball at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (randint1(rlev * 3 / 2) + 10) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_breath_monst(m_idx, y, x, GF_COLD, dam, 2, FALSE, MS_BALL_COLD, learnable);

				break;
			}

			/* RF5_BA_POIS */
			case 128+4:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s�˸����äư������μ�ʸ�򾧤�����", m_name, t_name);
#else
							msg_format("%^s casts a stinking cloud at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(12, 2) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_breath_monst(m_idx, y, x, GF_POIS, dam, 2, FALSE, MS_BALL_POIS, learnable);

				break;
			}

			/* RF5_BA_NETH */
			case 128+5:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s�˸����ä��Ϲ���μ�ʸ�򾧤�����", m_name, t_name);
#else
							msg_format("%^s casts a nether ball at %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = 50 + damroll(10, 10) + (rlev * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1));
				monst_breath_monst(m_idx, y, x, GF_NETHER, dam, 2, FALSE, MS_BALL_NETHER, learnable);

				break;
			}

			/* RF5_BA_WATE */
			case 128+6:
			{
				if (known)
				{
					if (see_either)
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
msg_format("%^s��%s���Ф���ή���褦�ʿȿ���򤷤���", m_name, t_name);
#else
							msg_format("%^s gestures fluidly at %s.", m_name, t_name);
#endif

#ifdef JP
msg_format("%^s�ϱ����˰��߹��ޤ줿��", t_name);
#else
							msg_format("%^s is engulfed in a whirlpool.", t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = ((r_ptr->flags2 & RF2_POWERFUL) ? randint1(rlev * 3) : randint1(rlev * 2)) + 50;
				monst_breath_monst(m_idx, y, x, GF_WATER, dam, 4, FALSE, MS_BALL_WATER, learnable);

				break;
			}

			/* RF5_BA_MANA */
			case 128+7:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
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
msg_format("%^s��%s���Ф������Ϥ���μ�ʸ��ǰ������", m_name, t_name);
#else
							msg_format("%^s invokes a mana storm upon %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (rlev * 4) + 50 + damroll(10, 10);
				monst_breath_monst(m_idx, y, x, GF_MANA, dam, 4, FALSE, MS_BALL_MANA, learnable);

				break;
			}

			/* RF5_BA_DARK */
			case 128+8:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
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
msg_format("%^s��%s���Ф��ưŹ�����μ�ʸ��ǰ������", m_name, t_name);
#else
							msg_format("%^s invokes a darkness storm upon %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (rlev * 4) + 50 + damroll(10, 10);
				monst_breath_monst(m_idx, y, x, GF_DARK, dam, 4, FALSE, MS_BALL_DARK, learnable);

				break;
			}

			/* RF5_DRAIN_MANA */
			case 128+9:
			{
				/* Attack power */
				int power = (randint1(rlev) / 2) + 1;

				if (see_m)
				{
					/* Basic message */
#ifdef JP
msg_format("%^s���������ͥ륮����%s����ۤ��Ȥä���", m_name, t_name);
#else
					msg_format("%^s draws psychic energy from %s.", m_name, t_name);
#endif

				}

				/* Heal the monster */
				if (m_ptr->hp < m_ptr->maxhp)
				{
					if (!tr_ptr->flags4 && !tr_ptr->flags5 && !tr_ptr->flags6)
					{
						if (see_both)
						{
#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
							msg_format("%^s is unaffected!", t_name);
#endif

						}
					}
					else
					{
						/* Heal */
						m_ptr->hp += 6 * power;
						if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (see_m)
						{
#ifdef JP
msg_format("%^s�ϵ�ʬ���ɤ���������", m_name);
#else
							msg_format("%^s appears healthier.", m_name);
#endif

						}
					}
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_MIND_BLAST */
			case 128+10:
			{
				if (see_m)
				{
#ifdef JP
					msg_format("%^s��%s�򤸤ä��ˤ����", m_name, t_name);
#else
					msg_format("%^s gazes intently at %s.", m_name, t_name);
#endif

				}

				dam = damroll(7, 7);
				/* Attempt a saving throw */
				if ((tr_ptr->flags1 & RF1_UNIQUE) ||
					 (tr_ptr->flags3 & RF3_NO_CONF) ||
					 (tr_ptr->flags3 & RF3_RES_ALL) ||
					 (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10))
				{
					/* No obvious effect */
					if (see_both)
					{
						/* Memorize a flag */
						if (tr_ptr->flags3 & (RF3_RES_ALL))
						{
							tr_ptr->r_flags3 |= (RF3_RES_ALL);
						}
						else if (tr_ptr->flags3 & (RF3_NO_CONF))
						{
							tr_ptr->r_flags3 |= (RF3_NO_CONF);
						}

#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
						msg_format("%^s is unaffected!", t_name);
#endif

					}
				}
				else
				{
					if (see_t)
					{
#ifdef JP
msg_format("%^s����������򿩤�ä���", t_name);
#else
						msg_format("%^s is blasted by psionic energy.", t_name);
#endif

					}

					t_ptr->confused += randint0(4) + 4;

#ifdef JP
mon_take_hit_mon(FALSE, t_idx, dam, &fear, "�������������������Τ�ȴ���̤Ȥʤä���", m_idx);
#else
					mon_take_hit_mon(FALSE, t_idx, dam, &fear, " collapses, a mindless husk.", m_idx);
#endif

				}

				wake_up = TRUE;

				break;
			}

			/* RF5_BRAIN_SMASH */
			case 128+11:
			{
				if (see_m)
				{
#ifdef JP
					msg_format("%^s��%s�򤸤ä��ˤ����", m_name, t_name);
#else
					msg_format("%^s gazes intently at %s.", m_name, t_name);
#endif

				}

				dam = damroll(12, 12);
				/* Attempt a saving throw */
				if ((tr_ptr->flags1 & RF1_UNIQUE) ||
					 (tr_ptr->flags3 & RF3_NO_CONF) ||
					 (tr_ptr->flags3 & RF3_RES_ALL) ||
					 (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10))
				{
					/* No obvious effect */
					if (see_both)
					{
						/* Memorize a flag */
						if (tr_ptr->flags3 & (RF3_RES_ALL))
						{
							tr_ptr->r_flags3 |= (RF3_RES_ALL);
						}
						else if (tr_ptr->flags3 & (RF3_NO_CONF))
						{
							tr_ptr->r_flags3 |= (RF3_NO_CONF);
						}

#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
						msg_format("%^s is unaffected!", t_name);
#endif

					}
				}
				else
				{
					if (see_t)
					{
#ifdef JP
msg_format("%^s����������򿩤�ä���", t_name);
#else
						msg_format("%^s is blasted by psionic energy.", t_name);
#endif

					}

					t_ptr->confused += randint0(4) + 4;
					t_ptr->slow = MIN(200, t_ptr->slow + 10);
					t_ptr->stunned += randint0(4) + 4;

#ifdef JP
mon_take_hit_mon(FALSE, t_idx, dam, &fear, "�������������������Τ�ȴ���̤Ȥʤä���", m_idx);
#else
					mon_take_hit_mon(FALSE, t_idx, dam, &fear, " collapses, a mindless husk.", m_idx);
#endif

				}

				wake_up = TRUE;

				break;
			}

			/* RF5_CAUSE_1 */
			case 128+12:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s��%s��ؤ����Ƽ����򤫤�����", m_name, t_name);
#else
						msg_format("%^s points at %s and curses.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(3, 8);
				if ((randint0(100 + rlev/2) < (tr_ptr->level + 35)) ||
					 (tr_ptr->flags3 & RF3_RES_ALL))
				{
					/* Memorize a flag */
					if (tr_ptr->flags3 & (RF3_RES_ALL))
					{
						tr_ptr->r_flags3 |= (RF3_RES_ALL);
					}
#ifdef JP
if (see_both) msg_format("%^s����������äƤ��롪", t_name);
#else
					if (see_both) msg_format("%^s resists!", t_name);
#endif

				}
				else
				{
					mon_take_hit_mon(FALSE, t_idx, dam, &fear, NULL, m_idx);
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_CAUSE_2 */
			case 128+13:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s��%s��ؤ����ƶ������˼����򤫤�����", m_name, t_name);
#else
						msg_format("%^s points at %s and curses horribly.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(8, 8);
				if ((randint0(100 + rlev/2) < (tr_ptr->level + 35)) ||
					 (tr_ptr->flags3 & RF3_RES_ALL))
				{
					/* Memorize a flag */
					if (tr_ptr->flags3 & (RF3_RES_ALL))
					{
						tr_ptr->r_flags3 |= (RF3_RES_ALL);
					}
#ifdef JP
if (see_both) msg_format("%^s����������äƤ��롪", t_name);
#else
					if (see_both) msg_format("%^s resists!", t_name);
#endif

				}
				else
				{
					mon_take_hit_mon(FALSE, t_idx, dam, &fear, NULL, m_idx);

				}

				wake_up = TRUE;

				break;
			}

			/* RF5_CAUSE_3 */
			case 128+14:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s��%s��ؤ������������˼�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s points at %s, incanting terribly!", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(10, 15);
				if ((randint0(100 + rlev/2) < (tr_ptr->level + 35)) ||
					 (tr_ptr->flags3 & RF3_RES_ALL))
				{
					/* Memorize a flag */
					if (tr_ptr->flags3 & (RF3_RES_ALL))
					{
						tr_ptr->r_flags3 |= (RF3_RES_ALL);
					}
#ifdef JP
if (see_both) msg_format("%^s����������äƤ��롪", t_name);
#else
					if (see_both) msg_format("%^s resists!", t_name);
#endif

				}
				else
				{
					mon_take_hit_mon(FALSE, t_idx, dam, &fear, NULL, m_idx);
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_CAUSE_4 */
			case 128+15:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s��%s���빦���ͤ��ơ��֤����ϴ��˻��Ǥ���פȶ������", m_name, t_name);
#else
						msg_format("%^s points at %s, screaming the word, 'DIE!'", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(15, 15);
				if (((randint0(100 + rlev/2) < (tr_ptr->level + 35)) && (m_ptr->r_idx != MON_KENSHIROU)) ||
					 (tr_ptr->flags3 & RF3_RES_ALL))
				{
					/* Memorize a flag */
					if (tr_ptr->flags3 & (RF3_RES_ALL))
					{
						tr_ptr->r_flags3 |= (RF3_RES_ALL);
					}
#ifdef JP
if (see_both) msg_format("%^s����������äƤ��롪", t_name);
#else
					if (see_both) msg_format("%^s resists!", t_name);
#endif

				}
				else
				{
					mon_take_hit_mon(FALSE, t_idx, dam, &fear, NULL, m_idx);
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_BO_ACID */
			case 128+16:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%s��%s�˸����äƥ����åɡ��ܥ�Ȥμ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts an acid bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (damroll(7, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_bolt_monst(m_idx, y, x, GF_ACID,
					dam, MS_BOLT_ACID, learnable);

				break;
			}

			/* RF5_BO_ELEC */
			case 128+17:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƥ���������ܥ�Ȥμ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a lightning bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (damroll(4, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_bolt_monst(m_idx, y, x, GF_ELEC,
					dam, MS_BOLT_ELEC, learnable);

				break;
			}

			/* RF5_BO_FIRE */
			case 128+18:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƥե��������ܥ�Ȥμ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a fire bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (damroll(9, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_bolt_monst(m_idx, y, x, GF_FIRE,
					dam, MS_BOLT_FIRE, learnable);

				break;
			}

			/* RF5_BO_COLD */
			case 128+19:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƥ��������ܥ�Ȥμ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a frost bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (damroll(6, 8) + (rlev / 3)) * ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 1);
				monst_bolt_monst(m_idx, y, x, GF_COLD,
					dam, MS_BOLT_COLD, learnable);

				break;
			}

			/* RF5_BA_LITE */
			case 128+20:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (blind)
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
msg_format("%^s��%s���Ф��ƥ������С����Ȥμ�ʸ��ǰ������", m_name, t_name);
#else
							msg_format("%^s invokes a starburst upon %s.", m_name, t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (rlev * 4) + 50 + damroll(10, 10);
				monst_breath_monst(m_idx, y, x, GF_LITE, dam, 4, FALSE, MS_STARBURST, learnable);

				break;
			}

			/* RF5_BO_NETH */
			case 128+21:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����ä��Ϲ�����μ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a nether bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = 30 + damroll(5, 5) + (rlev * 4) / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3);
				monst_bolt_monst(m_idx, y, x, GF_NETHER,
					dam, MS_BOLT_NETHER, learnable);

				break;
			}

			/* RF5_BO_WATE */
			case 128+22:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƥ������������ܥ�Ȥμ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a water bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(10, 10) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
				monst_bolt_monst(m_idx, y, x, GF_WATER,
					dam, MS_BOLT_WATER, learnable);

				break;
			}

			/* RF5_BO_MANA */
			case 128+23:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����ä����Ϥ���μ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a mana bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = randint1(rlev * 7 / 2) + 50;
				monst_bolt_monst(m_idx, y, x, GF_MANA,
					dam, MS_BOLT_MANA, learnable);

				break;
			}

			/* RF5_BO_PLAS */
			case 128+24:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƥץ饺�ޡ��ܥ�Ȥμ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a plasma bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = 10 + damroll(8, 7) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
				monst_bolt_monst(m_idx, y, x, GF_PLASMA,
					dam, MS_BOLT_PLASMA, learnable);

				break;
			}

			/* RF5_BO_ICEE */
			case 128+25:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƶ˴�����μ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts an ice bolt at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(6, 6) + (rlev * 3 / ((r_ptr->flags2 & RF2_POWERFUL) ? 2 : 3));
				monst_bolt_monst(m_idx, y, x, GF_ICE,
					dam, MS_BOLT_ICE, learnable);

				break;
			}

			/* RF5_MISSILE */
			case 128+26:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƥޥ��å����ߥ�����μ�ʸ�򾧤�����", m_name, t_name);
#else
						msg_format("%^s casts a magic missile at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = damroll(2, 6) + (rlev / 3);
				monst_bolt_monst(m_idx, y, x, GF_MISSILE,
					dam, MS_MAGIC_MISSILE, learnable);

				break;
			}

			/* RF5_SCARE */
			case 128+27:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s���������ʸ��Ф���Ф�����", m_name, t_name);
#else
						msg_format("%^s casts a fearful illusion in front of %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if (tr_ptr->flags3 & RF3_NO_FEAR)
				{
#ifdef JP
if (see_t) msg_format("%^s�϶��ݤ򴶤��ʤ���", t_name);
#else
					if (see_t) msg_format("%^s refuses to be frightened.", t_name);
#endif

				}
				else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
				{
#ifdef JP
if (see_t) msg_format("%^s�϶��ݤ򴶤��ʤ���", t_name);
#else
					if (see_t) msg_format("%^s refuses to be frightened.", t_name);
#endif

				}
				else
				{
					if (!t_ptr->monfear) fear = TRUE;

					t_ptr->monfear += randint0(4) + 4;
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_BLIND */
			case 128+28:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%s�ϼ�ʸ�򾧤���%s���ܤ�Ƥ��դ�������", m_name, t_name);
#else
						msg_format("%^s casts a spell, burning %s%s eyes.", m_name, t_name,
									  (streq(t_name, "it") ? "s" : "'s"));
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				/* Simulate blindness with confusion */
				if (tr_ptr->flags3 & RF3_NO_CONF)
				{
#ifdef JP
if (see_t) msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

				}
				else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
				{
#ifdef JP
if (see_t) msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

				}
				else
				{
#ifdef JP
if (see_t)   msg_format("%^s���ܤ������ʤ��ʤä��� ", t_name);
#else
					if (see_t) msg_format("%^s is blinded!", t_name);
#endif


					t_ptr->confused += 12 + (byte)randint0(4);
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_CONF */
			case 128+29:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�����˸���Ū�ʸ���Ĥ���Ф�����", m_name, t_name);
#else
						msg_format("%^s casts a mesmerizing illusion in front of %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if (tr_ptr->flags3 & RF3_NO_CONF)
				{
#ifdef JP
if (see_t) msg_format("%^s���Ǥ蘆��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s disbelieves the feeble spell.", t_name);
#endif

				}
				else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
				{
#ifdef JP
if (see_t) msg_format("%^s���Ǥ蘆��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s disbelieves the feeble spell.", t_name);
#endif

				}
				else
				{
#ifdef JP
if (see_t) msg_format("%^s�Ϻ��𤷤��褦����", t_name);
#else
					if (see_t) msg_format("%^s seems confused.", t_name);
#endif


					t_ptr->confused += 12 + (byte)randint0(4);
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_SLOW */
			case 128+30:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%s��%s�ζ��������Ϥ�ۤ��Ȥä���", m_name, t_name);
#else
						msg_format("%^s drains power from %s%s muscles.", m_name, t_name,
									  (streq(t_name, "it") ? "s" : "'s"));
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if (tr_ptr->flags1 & RF1_UNIQUE)
				{
#ifdef JP
if (see_t) msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

				}
				else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
				{
#ifdef JP
if (see_t) msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

				}
				else
				{
					if (!t_ptr->slow)
					{
#ifdef JP
if (see_t) msg_format("%s��ư�����٤��ʤä���", t_name);
#else
					if (see_t) msg_format("%^s starts moving slower.", t_name);
#endif
					}

					t_ptr->slow = MIN(200, t_ptr->slow + 50);
				}

				wake_up = TRUE;

				break;
			}

			/* RF5_HOLD */
			case 128+31:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�򤸤äȸ��Ĥ᤿��", m_name, t_name);
#else
						msg_format("%^s stares intently at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if ((tr_ptr->flags1 & RF1_UNIQUE) ||
					 (tr_ptr->flags3 & RF3_NO_STUN))
				{
#ifdef JP
if (see_t) msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

				}
				else if (tr_ptr->level > randint1((rlev - 10) < 1 ? 1 : (rlev - 10)) + 10)
				{
#ifdef JP
if (see_t) msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_t) msg_format("%^s is unaffected.", t_name);
#endif

				}
				else
				{
#ifdef JP
if (see_t) msg_format("%^s�����㤷����", t_name);
#else
					if (see_t) msg_format("%^s is paralyzed!", t_name);
#endif


					t_ptr->stunned += randint1(4) + 4;
				}

				wake_up = TRUE;

				break;
			}


			/* RF6_HASTE */
			case 160+0:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s����ʬ���Τ�ǰ�����ä���", m_name, m_poss);
#else
						msg_format("%^s concentrates on %s body.", m_name, m_poss);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				/* Allow quick speed increases to base+10 */
				if (!m_ptr->fast)
				{
#ifdef JP
if (see_m) msg_format("%^s��ư����®���ʤä���", m_name);
#else
					if (see_m) msg_format("%^s starts moving faster.", m_name);
#endif

				}
				m_ptr->fast = MIN(200, m_ptr->fast + 100);
				if (p_ptr->riding == m_idx) p_ptr->update |= PU_BONUS;
				break;
			}

			/* RF6_HAND_DOOM */
			case 160+1:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s��%s��<���Ǥμ�>�����ä���", m_name, t_name);
#else
						msg_format("%^s invokes the Hand of Doom upon %s!", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if ((tr_ptr->flags1 & RF1_UNIQUE) || (tr_ptr->flags3 & RF3_RES_ALL))
				{
					/* Memorize a flag */
					if (tr_ptr->flags3 & (RF3_RES_ALL))
					{
						tr_ptr->r_flags3 |= (RF3_RES_ALL);
					}
#ifdef JP
if (see_both) msg_format("�ˤϸ��̤��ʤ��ä���", t_name);
#else
					if (see_both) msg_format("^%s is unaffected!", t_name);
#endif

				}
				else
				{
					if ((r_ptr->level + randint1(20)) >
						(tr_ptr->level + 10 + randint1(20)))
					{
						t_ptr->hp = t_ptr->hp -
						  (((s32b)((40 + randint1(20)) * t_ptr->hp)) / 100);

						if (t_ptr->hp < 1) t_ptr->hp = 1;
					}
					else
					{
#ifdef JP
if (see_both) msg_format("%^s����������äƤ��롪", t_name);
#else
						if (see_both) msg_format("%^s resists!", t_name);
#endif

					}
				}

				wake_up = TRUE;

				break;
			}

			/* RF6_HEAL */
			case 160+2:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s�ϼ�ʬ�ν���ǰ���椷����", m_name);
#else
						msg_format("%^s concentrates on %s wounds.", m_name, m_poss);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				/* Heal some */
				m_ptr->hp += (rlev * 6);

				/* Fully healed */
				if (m_ptr->hp >= m_ptr->maxhp)
				{
					/* Fully healed */
					m_ptr->hp = m_ptr->maxhp;

					if (known)
					{
						if (see_m)
						{
#ifdef JP
msg_format("%^s�ϴ����˼��ä���", m_name);
#else
							msg_format("%^s looks completely healed!", m_name);
#endif

						}
						else
						{
							mon_fight = TRUE;
						}
					}
				}

				/* Partially healed */
				else if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s�����Ϥ���������褦����", m_name);
#else
						msg_format("%^s looks healthier.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
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
if (see_m) msg_format("%^s��ͦ�������ᤷ����", m_name);
#else
					if (see_m) msg_format("%^s recovers %s courage.", m_name, m_poss);
#endif

				}

				break;
			}

			/* RF6_INVULNER */
			case 160+3:
			{
				if (known)
				{
					if (see_m)
					{
						disturb(1, 0);
#ifdef JP
msg_format("%s��̵���ε�μ�ʸ�򾧤�����", m_name);
#else
						msg_format("%^s casts a Globe of Invulnerability.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if (!m_ptr->invulner) m_ptr->invulner = randint1(4) + 4;

				if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
				if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);
				break;
			}

			/* RF6_BLINK */
			case 160+4:
			{
				if (see_m)
				{
#ifdef JP
msg_format("%^s���ֻ��˾ä�����", m_name);
#else
					msg_format("%^s blinks away.", m_name);
#endif

				}

				teleport_away(m_idx, 10, FALSE);

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

				if (see_m)
				{
#ifdef JP
msg_format("%^s���ƥ�ݡ��Ȥ�����", m_name);
#else
					msg_format("%^s teleports away.", m_name);
#endif

				}

				teleport_away(m_idx, MAX_SIGHT * 2 + 5, FALSE);

				if (los(py, px, oldfy, oldfx) && !world_monster && see_m)
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
									p_ptr->energy_need = ENERGY_NEED();
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
#if 0
				int who = 0;
				if(m_ptr->r_idx = MON_DIO) who == 1;
				else if(m_ptr->r_idx = MON_WONG) who == 3;
				dam = who;
				if(!process_the_world(randint1(2)+2, who, los(py, px, m_ptr->fy, m_ptr->fx))) return (FALSE);
#endif
				return FALSE;
			}

			/* RF6_SPECIAL */
			case 160+7:
			{
				int k;
				if (p_ptr->inside_arena || p_ptr->inside_battle) return FALSE;
				switch(m_ptr->r_idx)
				{
					case MON_OHMU:
					{
						for (k = 0; k < 6; k++)
						{
							summon_specific(m_idx, m_ptr->fy, m_ptr->fx, rlev, SUMMON_BIZARRE1, PM_ALLOW_GROUP);
						}
						return FALSE;
					}
				default:
					if (r_ptr->d_char == 'B')
					{
						if (one_in_(3))
						{
							if (see_m)
							{
#ifdef JP
								msg_format("%^s�������޾徺���ƻ볦����ä���!", m_name);
#else
								msg_format("%^s suddenly go out of your sight!", m_name);
#endif
							}
							teleport_away(m_idx, 10, FALSE);
							p_ptr->update |= (PU_MONSTERS);
							break;
						}
						else
						{
							/* Not implemented */
							return FALSE;
						}
						break;
					}
					
					/* Something is wrong */
					else return FALSE;
				}

				/* done */
				break;
			}
			/* RF6_TELE_TO */
			case 160+8:
			{
				/* Not implemented */
				return FALSE;
			}

			/* RF6_TELE_AWAY */
			case 160+9:
			{
				bool resists_tele = FALSE;

				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s��ƥ�ݡ��Ȥ�������", m_name, t_name);
#else
						msg_format("%^s teleports %s away.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if (tr_ptr->flags3 & RF3_RES_TELE)
				{
					if ((tr_ptr->flags1 & RF1_UNIQUE) || (tr_ptr->flags3 & (RF3_RES_ALL)))
					{
						if (see_t)
						{
							tr_ptr->r_flags3 |= RF3_RES_TELE;
#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", t_name);
#else
							msg_format("%^s is unaffected!", t_name);
#endif

						}

						resists_tele = TRUE;
					}
					else if (tr_ptr->level > randint1(100))
					{
						if (see_t)
						{
							tr_ptr->r_flags3 |= RF3_RES_TELE;
#ifdef JP
msg_format("%^s����������äƤ��롪", t_name);
#else
							msg_format("%^s resists!", t_name);
#endif

						}

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
					if (t_idx == p_ptr->riding) teleport_player(MAX_SIGHT * 2 + 5);
					else teleport_away(t_idx, MAX_SIGHT * 2 + 5, FALSE);
				}

				break;
			}

			/* RF6_TELE_LEVEL */
			case 160+10:
			{
				/* Not implemented */
				return FALSE;
			}

			/* RF6_PSY_SPEAR */
			case 160+11:
			{
				if (known)
				{
					if (see_either)
					{
#ifdef JP
msg_format("%^s��%s�˸����äƸ��η������ä���", m_name, t_name);
#else
						msg_format("%^s throw a Psycho-spear at %s.", m_name, t_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				dam = (r_ptr->flags2 & RF2_POWERFUL) ? (randint1(rlev * 2) + 180) : (randint1(rlev * 3 / 2) + 120);
				monst_beam_monst(m_idx, y, x, GF_PSY_SPEAR,
					dam, MS_PSY_SPEAR, learnable);
				break;
			}

			/* RF6_DARKNESS */
			case 160+12:
			{
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s���ŰǤ���Ǽ�򿶤ä���", m_name);
#else
						msg_format("%^s gestures in shadow.", m_name);
#endif


						if (see_t)
						{
#ifdef JP
msg_format("%^s�ϰŰǤ���ޤ줿��", t_name);
#else
							msg_format("%^s is surrounded by darkness.", t_name);
#endif

						}
					}
					else
					{
						mon_fight = TRUE;
					}
				}

				(void)project(m_idx, 3, y, x, 0, GF_DARK_WEAK, PROJECT_GRID | PROJECT_KILL, MS_DARKNESS);

				unlite_room(y, x);

				break;
			}

			/* RF6_TRAPS */
			case 160+13:
			{
#if 0
				if (known)
				{
					if (see_m)
					{
#ifdef JP
msg_format("%^s����ʸ�򾧤��Ƽٰ������Ф����", m_name);
#else
						msg_format("%^s casts a spell and cackles evilly.", m_name);
#endif
					}
					else
					{
#ifdef JP
msg_format("%^s��������Ĥ֤䤤����", m_name);
#else
						msg_format("%^s mumbles.", m_name);
#endif
					}
				}

				trap_creation(y, x);
#endif
				break;
			}

			/* RF6_FORGET */
			case 160+14:
			{
				/* Not implemented */
				return FALSE;
			}

			/* RF6_RAISE_DEAD */
			case 160+15:
			{
				if (known)
				{
					if (see_either)
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
					}
					else
					{
						mon_fight = TRUE;
					}
				}
				animate_dead(m_idx, m_ptr->fy, m_ptr->fx);
				break;
			}

			/* RF6_SUMMON_KIN */
			case 160+16:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

						if (m_ptr->r_idx == MON_ROLENTO)
						{
#ifdef JP
							msg_format("%^s�ϼ����Ƥ�Ф�ޤ�����",
								m_name);
#else
							msg_format("%^s throws some hand grenades.",
								m_name);
#endif
						}
						else if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
						{
#ifdef JP
							msg_format("%^s�����󥸥��μ�򾤴�������", m_name);
#else
							msg_format("%^s magically summons guardians of dungeons.", m_name);
#endif
						}
						else
						{
#ifdef JP
msg_format("%s����ˡ��%s�򾤴�������", m_name,
  ((r_ptr->flags1 & RF1_UNIQUE) ? "�겼" : "���"));
#else
							msg_format("%^s magically summons %s %s.", m_name, m_poss,
									  ((r_ptr->flags1 & RF1_UNIQUE) ? "minions" : "kin"));
#endif
						}

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if(m_ptr->r_idx == MON_ROLENTO)
				{
					int num = 1 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, MON_SHURYUUDAN, p_mode);
					}
				}
				else if(m_ptr->r_idx == MON_THORONDOR ||
					m_ptr->r_idx == MON_GWAIHIR ||
					m_ptr->r_idx == MON_MENELDOR)
				{
					int num = 4 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_EAGLES, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | p_mode));
					}
				}
				else if(m_ptr->r_idx == MON_LOUSY)
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_LOUSE, (PM_ALLOW_GROUP | p_mode));
					}
				}
				else if(m_ptr->r_idx == MON_BULLGATES)
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, 921, p_mode);
					}
				}
				else if (m_ptr->r_idx == MON_CALDARM)
				{
					int num = randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_named_creature(m_idx, y, x, 930, p_mode);
					}
				}
				else if (m_ptr->r_idx == MON_SERPENT || m_ptr->r_idx == MON_ZOMBI_SERPENT)
				{
					int num = 2 + randint1(3);
					for (k = 0; k < num; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_GUARDIANS, (PM_ALLOW_GROUP | p_mode | PM_ALLOW_UNIQUE));
					}
				}
				else
				{
					summon_kin_type = r_ptr->d_char;

					for (k = 0; k < 4; k++)
					{
						count += summon_specific(m_idx, y, x, rlev, SUMMON_KIN, (PM_ALLOW_GROUP | p_mode));
					}
				}

				if (known && !see_t && count)
				{
				     mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_CYBER */
			case 160+17:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s�������С��ǡ����򾤴�������", m_name);
#else
						msg_format("%^s magically summons Cyberdemons!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if (is_friendly(m_ptr))
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_CYBER, (PM_ALLOW_GROUP | p_mode));
				}
				else
				{
					count += summon_cyber(m_idx, y, x);
				}

				if (known && !see_t && count)
				{
				     mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_MONSTER */
			case 160+18:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ����֤򾤴�������", m_name);
#else
						msg_format("%^s magically summons help!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				count += summon_specific(m_idx, y, x, rlev, 0, (p_mode | u_mode));

				if (known && !see_t && count)
				{
				      mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_MONSTERS */
			case 160+19:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�ǥ�󥹥����򾤴�������", m_name);
#else
						msg_format("%^s magically summons monsters!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, 0, (PM_ALLOW_GROUP | p_mode | u_mode));
				}

				if (known && !see_t && count)
				{
				  mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_ANT */
			case 160+20:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�ǥ���򾤴�������", m_name);
#else
						msg_format("%^s magically summons ants.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_ANT, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				     mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_SPIDER */
			case 160+21:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�ǥ���򾤴�������", m_name);
#else
						msg_format("%^s magically summons spiders.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_SPIDER, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				    mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_HOUND */
			case 160+22:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�ǥϥ���ɤ򾤴�������", m_name);
#else
						msg_format("%^s magically summons hounds.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HOUND, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				     mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_HYDRA */
			case 160+23:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�ǥҥɥ�򾤴�������", m_name);
#else
						msg_format("%^s magically summons hydras.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HYDRA, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				    mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_ANGEL */
			case 160+24:
			{
				int num = 1;
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ��ŷ�Ȥ򾤴�������", m_name);
#else
						msg_format("%^s magically summons an angel!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				if ((r_ptr->flags1 & RF1_UNIQUE) && !easy_band)
				{
					num += r_ptr->level/40;
				}

				for (k = 0; k < num; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_ANGEL, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				   mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_DEMON */
			case 160+25:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�Ǻ��٤ε����ǡ����򾤴�������", m_name);
#else
						msg_format("%^s magically summons a demon from the Courts of Chaos!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < 1; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_DEMON, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				    mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_UNDEAD */
			case 160+26:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%s����ˡ�ǥ���ǥåɤ򾤴�������", m_name);
#else
						msg_format("%^s magically summons undead.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < 1; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_UNDEAD, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				    mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_DRAGON */
			case 160+27:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�ǥɥ饴��򾤴�������", m_name);
#else
						msg_format("%^s magically summons a dragon!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < 1; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_DRAGON, (PM_ALLOW_GROUP | p_mode));
				}

				if (known && !see_t && count)
				{
				    mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_HI_UNDEAD */
			case 160+28:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%s����ˡ�ǥ���ǥåɤ򾤴�������", m_name);
#else
						msg_format("%^s magically summons undead.", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_6; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_UNDEAD, (PM_ALLOW_GROUP | p_mode | u_mode));
				}

				if (known && !see_t && count)
				{
				   mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_HI_DRAGON */
			case 160+29:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�Ǹ���ɥ饴��򾤴�������", m_name);
#else
						msg_format("%^s magically summons ancient dragons!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_HI_DRAGON, (PM_ALLOW_GROUP | p_mode | u_mode));
				}

				if (known && !see_t && count)
				{
				   mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_AMBERITES */
			case 160+30:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s������С��β�²�򾤴�������", m_name);
#else
						msg_format("%^s magically summons Lords of Amber!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_AMBERITES, (PM_ALLOW_GROUP | p_mode | PM_ALLOW_UNIQUE));
				}

				if (known && !see_t && count)
				{
				   mon_fight = TRUE;
				}

				break;
			}

			/* RF6_S_UNIQUE */
			case 160+31:
			{
				if (known)
				{
					if (see_either)
					{
						disturb(1, 0);

#ifdef JP
msg_format("%^s����ˡ�����̤ʶ�Ũ�򾤴�������", m_name);
#else
						msg_format("%^s magically summons special opponents!", m_name);
#endif

					}
					else
					{
						mon_fight = TRUE;
					}
				}

				for (k = 0; k < s_num_4; k++)
				{
					count += summon_specific(m_idx, y, x, rlev, SUMMON_UNIQUE, (PM_ALLOW_GROUP | p_mode | PM_ALLOW_UNIQUE));
				}

				if (known && !see_t && count)
				{
				   mon_fight = TRUE;
				}

				break;
			}
		}

		if (wake_up)
		{
			t_ptr->csleep = 0;
			if (tr_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);
		}

		if (fear && see_t)
		{
#ifdef JP
msg_format("%^s�϶��ݤ���ƨ���Ф�����", t_name);
#else
			msg_format("%^s flees in terror!", t_name);
#endif

		}

		if (see_m && maneable && !world_monster && !p_ptr->blind && (p_ptr->pclass == CLASS_IMITATOR))
		{
			if (thrown_spell != 167)
			{
				if (p_ptr->mane_num == MAX_MANE)
				{
					int i;
					p_ptr->mane_num--;
					for (i = 0;i < p_ptr->mane_num-1;i++)
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

		/* Remember what the monster did, if we saw it */
		if (see_m)
		{
			/* Inate spell */
			if (thrown_spell < 32*4)
			{
				r_ptr->r_flags4 |= (1L << (thrown_spell - 32*3));
				if (r_ptr->r_cast_inate < MAX_UCHAR) r_ptr->r_cast_inate++;
			}

			/* Bolt or Ball */
			else if (thrown_spell < 32*5)
			{
				r_ptr->r_flags5 |= (1L << (thrown_spell - 32*4));
				if (r_ptr->r_cast_spell < MAX_UCHAR) r_ptr->r_cast_spell++;
			}

			/* Special spell */
			else if (thrown_spell < 32*6)
			{
				r_ptr->r_flags6 |= (1L << (thrown_spell - 32*5));
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

	/* No enemy found */
	return (FALSE);
}
