/* File: xtra2.c */

/* Purpose: effects of various "objects" */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#define REWARD_CHANCE 10


/*
 * Advance experience levels and print experience
 */
void check_experience(void)
{
	int		i;
	bool level_reward = FALSE;
	bool level_mutation = FALSE;
	bool level_inc_stat = FALSE;
	bool android = (p_ptr->prace == RACE_ANDROID ? TRUE : FALSE);


	/* Note current level */
	i = p_ptr->lev;

	/* Hack -- lower limit */
	if (p_ptr->exp < 0) p_ptr->exp = 0;

	/* Hack -- lower limit */
	if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;

	/* Hack -- upper limit */
	if (p_ptr->exp > PY_MAX_EXP) p_ptr->exp = PY_MAX_EXP;

	/* Hack -- upper limit */
	if (p_ptr->max_exp > PY_MAX_EXP) p_ptr->max_exp = PY_MAX_EXP;

	/* Hack -- maintain "max" experience */
	if (p_ptr->exp > p_ptr->max_exp) p_ptr->max_exp = p_ptr->exp;

	/* Redraw experience */
	p_ptr->redraw |= (PR_EXP);

	/* Handle stuff */
	handle_stuff();


	/* Lose levels while possible */
	while ((p_ptr->lev > 1) &&
	       (p_ptr->exp < ((android ? player_exp_a : player_exp)[p_ptr->lev - 2] * p_ptr->expfact / 100L)))
	{
		/* Lose a level */
		p_ptr->lev--;
		lite_spot(py, px);

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Handle stuff */
		handle_stuff();
	}


	/* Gain levels while possible */
	while ((p_ptr->lev < PY_MAX_LEVEL) &&
	       (p_ptr->exp >= ((android ? player_exp_a : player_exp)[p_ptr->lev-1] * p_ptr->expfact / 100L)))
	{
		/* Gain a level */
		p_ptr->lev++;
		lite_spot(py, px);

		/* Save the highest level */
		if (p_ptr->lev > p_ptr->max_plv)
		{
			p_ptr->max_plv = p_ptr->lev;

			if ((p_ptr->pclass == CLASS_CHAOS_WARRIOR) ||
			    (p_ptr->muta2 & MUT2_CHAOS_GIFT))
			{
				level_reward = TRUE;
			}
			if (p_ptr->prace == RACE_BEASTMAN)
			{
				if (randint(5) == 1) level_mutation = TRUE;
			}
			level_inc_stat = TRUE;

			do_cmd_write_nikki(NIKKI_LEVELUP, p_ptr->lev, NULL);
		}

		/* Sound */
		sound(SOUND_LEVEL);

		/* Message */
#ifdef JP
msg_format("��٥� %d �ˤ褦������", p_ptr->lev);
#else
		msg_format("Welcome to level %d.", p_ptr->lev);
#endif

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER | PW_SPELL);

#ifdef JP
                level_up = 1;
#endif
		/* Handle stuff */
		handle_stuff();

#ifdef JP
                level_up = 0;
#endif
		if (level_reward)
		{
			gain_level_reward(0);
			level_reward = FALSE;
		}

		if (level_inc_stat)
		{
			if(!(p_ptr->max_plv % 10))
			{
				int choice;
				screen_save();
				while(1)
				{
					int n;
					char tmp[32];

#ifdef JP
					cnv_stat(p_ptr->stat_max[0], tmp);
					prt(format("        a) ���� (������ %s)", tmp), 2, 14);
					cnv_stat(p_ptr->stat_max[1], tmp);
					prt(format("        b) ��ǽ (������ %s)", tmp), 3, 14);
					cnv_stat(p_ptr->stat_max[2], tmp);
					prt(format("        c) ���� (������ %s)", tmp), 4, 14);
					cnv_stat(p_ptr->stat_max[3], tmp);
					prt(format("        d) ���� (������ %s)", tmp), 5, 14);
					cnv_stat(p_ptr->stat_max[4], tmp);
					prt(format("        e) �ѵ� (������ %s)", tmp), 6, 14);
					cnv_stat(p_ptr->stat_max[5], tmp);
					prt(format("        f) ̥�� (������ %s)", tmp), 7, 14);
					prt("", 8, 14);
					prt("        �ɤ�ǽ���ͤ�夲�ޤ�����", 1, 14);
#else
					cnv_stat(p_ptr->stat_max[0], tmp);
					prt(format("        a) Str (cur %s)", tmp), 2, 14);
					cnv_stat(p_ptr->stat_max[1], tmp);
					prt(format("        b) Int (cur %s)", tmp), 3, 14);
					cnv_stat(p_ptr->stat_max[2], tmp);
					prt(format("        c) Wis (cur %s)", tmp), 4, 14);
					cnv_stat(p_ptr->stat_max[3], tmp);
					prt(format("        d) Dex (cur %s)", tmp), 5, 14);
					cnv_stat(p_ptr->stat_max[4], tmp);
					prt(format("        e) Con (cur %s)", tmp), 6, 14);
					cnv_stat(p_ptr->stat_max[5], tmp);
					prt(format("        f) Chr (cur %s)", tmp), 7, 14);
					prt("", 8, 14);
					prt("        Which stat do you want to raise?", 1, 14);
#endif
					while(1)
					{
						choice = inkey();
						if ((choice >= 'a') && (choice <= 'f')) break;
					}
					for(n = 0; n < 6; n++)
						if (n != choice - 'a')
							prt("",n+2,14);
#ifdef JP
					if (get_check("������Ǥ�����")) break;
#else
					if (get_check("Are you sure? ")) break;
#endif
				}
				do_inc_stat(choice - 'a');
				screen_load();
			}
			else if(!(p_ptr->max_plv % 2))
				do_inc_stat(rand_int(6));
		}

		if (level_mutation)
		{
#ifdef JP
msg_print("���ʤ����Ѥ�ä���������...");
#else
			msg_print("You feel different...");
#endif

			(void)gain_random_mutation(0);
			level_mutation = FALSE;
		}

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER | PW_SPELL);

		/* Handle stuff */
		handle_stuff();
	}
}


/*
 * Hack -- Return the "automatic coin type" of a monster race
 * Used to allocate proper treasure when "Creeping coins" die
 *
 * XXX XXX XXX Note the use of actual "monster names"
 */
static int get_coin_type(int r_idx)
{
	monster_race    *r_ptr = &r_info[r_idx];

	/* Analyze "coin" monsters */
	if (r_ptr->d_char == '$')
	{
		/* Look for textual clues */
		switch (r_idx)
		{
		case MON_COPPER_COINS: return (2);
		case MON_SILVER_COINS: return (5);
		case MON_GOLD_COINS: return (10);
		case MON_MITHRIL_COINS: return (16);
		case MON_ADAMANT_COINS: return (17);
		}
	}

	/* Assume nothing */
	return (0);
}


/*
 * Hack -- determine if a template is Cloak
 */
static bool kind_is_cloak(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_CLOAK)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Polearm
 */
static bool kind_is_polearm(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_POLEARM)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Sword
 */
static bool kind_is_sword(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval == TV_SWORD) && (k_ptr->sval > 2))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Book
 */
static bool kind_is_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_LIFE_BOOK) && (k_ptr->tval <= TV_DAEMON_BOOK))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Good book
 */
static bool kind_is_good_book(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if ((k_ptr->tval >= TV_LIFE_BOOK) && (k_ptr->tval <= TV_DAEMON_BOOK) && (k_ptr->tval != TV_ARCANE_BOOK) && (k_ptr->sval > 1))
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Hack -- determine if a template is Armor
 */
static bool kind_is_armor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	if (k_ptr->tval == TV_HARD_ARMOR)
	{
		return (TRUE);
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Check for "Quest" completion when a quest monster is killed or charmed.
 */
void check_quest_completion(monster_type *m_ptr)
{
	int i, j, y, x, ny, nx, i2, j2;

	int quest_num;

	bool create_stairs = FALSE;
	bool reward = FALSE;

	object_type forge;
	object_type *q_ptr;

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Inside a quest */
	quest_num = p_ptr->inside_quest;

	/* Search for an active quest on this dungeon level */
	if (!quest_num)
	{
		for (i = max_quests - 1; i > 0; i--)
		{
			/* Quest is not active */
			if (quest[i].status != QUEST_STATUS_TAKEN)
				continue;

			/* Quest is not a dungeon quest */
			if (quest[i].flags & QUEST_FLAG_PRESET)
				continue;

			/* Quest is not on this level */
			if ((quest[i].level != dun_level) &&
			    (quest[i].type != QUEST_TYPE_KILL_ANY_LEVEL))
				continue;

			/* Not a "kill monster" quest */
			if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) ||
			    (quest[i].type == QUEST_TYPE_FIND_EXIT))
				continue;

			/* Interesting quest */
			if ((quest[i].type == QUEST_TYPE_KILL_NUMBER) ||
			    (quest[i].type == QUEST_TYPE_KILL_ALL))
				break;

			/* Interesting quest */
			if (((quest[i].type == QUEST_TYPE_KILL_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_KILL_ANY_LEVEL) ||
			     (quest[i].type == QUEST_TYPE_RANDOM)) &&
			     (quest[i].r_idx == m_ptr->r_idx))
				break;
		}

		quest_num = i;
	}

	/* Handle the current quest */
	if (quest_num && (quest[quest_num].status == QUEST_STATUS_TAKEN))
	{
		/* Current quest */
		i = quest_num;

		switch (quest[i].type)
		{
			case QUEST_TYPE_KILL_NUMBER:
			{
				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].num_mon)
				{
					if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = p_ptr->lev;

					if (!(quest[i].flags & QUEST_FLAG_SILENT))
					{
#ifdef JP
msg_print("�������Ȥ�ã��������");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}

					quest[i].cur_num = 0;
				}
				break;
			}
			case QUEST_TYPE_KILL_ALL:
			{
				int number_mon = 0;

				if (!is_hostile(m_ptr)) break;

				/* Count all hostile monsters */
				for (i2 = 0; i2 < cur_wid; ++i2)
					for (j2 = 0; j2 < cur_hgt; j2++)
						if (cave[j2][i2].m_idx > 0)
							if (is_hostile(&m_list[cave[j2][i2].m_idx])) 
								number_mon++;

				if ((number_mon - 1) == 0)
				{
					if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					/* completed */
					if (quest[i].flags & QUEST_FLAG_SILENT)
					{
						quest[i].status = QUEST_STATUS_FINISHED;
					}
					else
					{
						quest[i].status = QUEST_STATUS_COMPLETED;
						quest[i].complev = p_ptr->lev;
#ifdef JP
msg_print("�������Ȥ�ã��������");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_LEVEL:
			case QUEST_TYPE_RANDOM:
			{
				/* Only count valid monsters */
				if (quest[i].r_idx != m_ptr->r_idx)
					break;

				quest[i].cur_num++;

				if (quest[i].cur_num >= quest[i].max_num)
				{
					if (record_fix_quest && (quest[i].type == QUEST_TYPE_KILL_LEVEL)) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					if (record_rand_quest && (quest[i].type == QUEST_TYPE_RANDOM)) do_cmd_write_nikki(NIKKI_RAND_QUEST_C, i, NULL);
					/* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = p_ptr->lev;
					if (!(quest[i].flags & QUEST_FLAG_PRESET))
					{
						create_stairs = TRUE;
						p_ptr->inside_quest = 0;
					}

					if (!quest[i].flags & QUEST_FLAG_SILENT)
					{
#ifdef JP
msg_print("�������Ȥ�ã��������");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}

					/* Finish the two main quests without rewarding */
					if ((i == QUEST_OBERON) || (i == QUEST_SERPENT))
					{
						quest[i].status = QUEST_STATUS_FINISHED;
					}

					if (quest[i].type == QUEST_TYPE_RANDOM)
					{
						reward = TRUE;
						quest[i].status = QUEST_STATUS_FINISHED;
					}
				}
				break;
			}
			case QUEST_TYPE_KILL_ANY_LEVEL:
			{
				quest[i].cur_num++;
				if (quest[i].cur_num >= quest[i].max_num)
				{
					if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
					 /* completed quest */
					quest[i].status = QUEST_STATUS_COMPLETED;
					quest[i].complev = p_ptr->lev;

					if (!quest[i].flags & QUEST_FLAG_SILENT)
					{
#ifdef JP
msg_print("�������Ȥ�ã��������");
#else
						msg_print("You just completed your quest!");
#endif

						msg_print(NULL);
					}
					quest[i].cur_num = 0;
				}
				break;
			}
		}
	}

	/* Create a magical staircase */
	if (create_stairs)
	{
		/* Stagger around */
		while (cave_perma_bold(y, x) || cave[y][x].o_idx)
		{
			/* Pick a location */
			scatter(&ny, &nx, y, x, 1, 0);

			/* Stagger */
			y = ny; x = nx;
		}

		/* Explain the staircase */
#ifdef JP
msg_print("��ˡ�γ��ʤ����줿...");
#else
		msg_print("A magical staircase appears...");
#endif


		/* Create stairs down */
		cave_set_feat(y, x, FEAT_MORE);

		/* Delete invisible trap */
		cave[y][x].info &= ~(CAVE_TRAP);

		/* Remember to update everything */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MONSTERS);
	}

	/*
	 * Drop quest reward
	 */
	if (reward)
	{
		for (j = 0; j < (dun_level / 15)+1; j++)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Wipe the object */
			object_wipe(q_ptr);

			/* Make a great object */
			make_object(q_ptr, TRUE, TRUE);

#ifdef USE_SCRIPT
			q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
	}
}

/*
 * Handle the "death" of a monster.
 *
 * Disperse treasures centered at the monster location based on the
 * various flags contained in the monster flags fields.
 *
 * Check for "Quest" completion when a quest monster is killed.
 *
 * Note that only the player can induce "monster_death()" on Uniques.
 * Thus (for now) all Quest monsters should be Uniques.
 *
 * Note that monsters can now carry objects, and when a monster dies,
 * it drops all of its objects, which may disappear in crowded rooms.
 */
void monster_death(int m_idx, bool drop_item)
{
	int i, j, y, x;

	int dump_item = 0;
	int dump_gold = 0;

	int number = 0;

	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool visible = (m_ptr->ml || (r_ptr->flags1 & RF1_UNIQUE));

	bool good = (r_ptr->flags1 & RF1_DROP_GOOD) ? TRUE : FALSE;
	bool great = (r_ptr->flags1 & RF1_DROP_GREAT) ? TRUE : FALSE;

	bool do_gold = (!(r_ptr->flags1 & RF1_ONLY_ITEM));
	bool do_item = (!(r_ptr->flags1 & RF1_ONLY_GOLD));
	bool cloned = FALSE;
	int force_coin = get_coin_type(m_ptr->r_idx);

	object_type forge;
	object_type *q_ptr;


	if (world_monster) world_monster = FALSE;

	/* Notice changes in view */
	if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2 | RF7_SELF_LITE_1 | RF7_SELF_LITE_2))
	{
		/* Update some things */
		p_ptr->update |= (PU_MON_LITE);
	}

	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	if (m_ptr->smart & SM_CLONED)
		cloned = TRUE;

	if (record_named_pet && is_pet(m_ptr) && m_ptr->nickname)
	{
		char m_name[80];

		monster_desc(m_name, m_ptr, 0x08);
		do_cmd_write_nikki(NIKKI_NAMED_PET, 3, m_name);
	}

	/* Let monsters explode! */
	for (i = 0; i < 4; i++)
	{
		if ((r_ptr->blow[i].method == RBM_EXPLODE) || (m_ptr->r_idx == MON_ROLENTO))
		{
			int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
			int typ = GF_MISSILE;
			int d_dice = r_ptr->blow[i].d_dice;
			int d_side = r_ptr->blow[i].d_side;
			int damage = damroll(d_dice, d_side);

			switch (r_ptr->blow[i].effect)
			{
				case RBE_HURT:      typ = GF_MISSILE; break;
				case RBE_POISON:    typ = GF_POIS; break;
				case RBE_UN_BONUS:  typ = GF_DISENCHANT; break;
				case RBE_UN_POWER:  typ = GF_MISSILE; break; /* ToDo: Apply the correct effects */
				case RBE_EAT_GOLD:  typ = GF_MISSILE; break;
				case RBE_EAT_ITEM:  typ = GF_MISSILE; break;
				case RBE_EAT_FOOD:  typ = GF_MISSILE; break;
				case RBE_EAT_LITE:  typ = GF_MISSILE; break;
				case RBE_ACID:      typ = GF_ACID; break;
				case RBE_ELEC:      typ = GF_ELEC; break;
				case RBE_FIRE:      typ = GF_FIRE; break;
				case RBE_COLD:      typ = GF_COLD; break;
				case RBE_BLIND:     typ = GF_MISSILE; break;
				case RBE_CONFUSE:   typ = GF_CONFUSION; break;
				case RBE_TERRIFY:   typ = GF_MISSILE; break;
				case RBE_PARALYZE:  typ = GF_MISSILE; break;
				case RBE_LOSE_STR:  typ = GF_MISSILE; break;
				case RBE_LOSE_DEX:  typ = GF_MISSILE; break;
				case RBE_LOSE_CON:  typ = GF_MISSILE; break;
				case RBE_LOSE_INT:  typ = GF_MISSILE; break;
				case RBE_LOSE_WIS:  typ = GF_MISSILE; break;
				case RBE_LOSE_CHR:  typ = GF_MISSILE; break;
				case RBE_LOSE_ALL:  typ = GF_MISSILE; break;
				case RBE_SHATTER:   typ = GF_ROCKET; break;
				case RBE_EXP_10:    typ = GF_MISSILE; break;
				case RBE_EXP_20:    typ = GF_MISSILE; break;
				case RBE_EXP_40:    typ = GF_MISSILE; break;
				case RBE_EXP_80:    typ = GF_MISSILE; break;
				case RBE_DISEASE:   typ = GF_POIS; break;
				case RBE_TIME:      typ = GF_TIME; break;
				case RBE_EXP_VAMP:  typ = GF_MISSILE; break;
				case RBE_DR_MANA:   typ = GF_MANA; break;
				case RBE_SUPERHURT: typ = GF_MISSILE; break;
			}
			if (m_ptr->r_idx == MON_ROLENTO)
			{
				typ = GF_FIRE;
				damage = damroll(20,10);
			}

			project(m_idx, 3, y, x, damage, typ, flg, -1);
			break;
		}
	}

	if (m_ptr->mflag2 & MFLAG_CHAMELEON)
	{
		choose_new_monster(m_idx, TRUE, MON_CHAMELEON);
		r_ptr = &r_info[m_ptr->r_idx];
	}

	/* Check for quest completion */
	check_quest_completion(m_ptr);

	/* Handle the possibility of player vanquishing arena combatant -KMW- */
	if (p_ptr->inside_arena && !is_pet(m_ptr))
	{
		char m_name[80];

		/* Extract monster name */
		monster_desc(m_name, m_ptr, 0);

		p_ptr->exit_bldg = TRUE;

		if (p_ptr->arena_number > MAX_ARENA_MONS)
		{
#ifdef JP
msg_print("�����餷�������������ξ����Ԥ���");
#else
			msg_print("You are a Genuine Champion!");
#endif
		}
		else
		{
#ifdef JP
msg_print("�����������ԥ���ؤ�ƻ��ʤ�Ǥ��롣");
#else
			msg_print("Victorious! You're on your way to becoming Champion.");
#endif
		}

		if (arena_shouhin[p_ptr->arena_number])
		{
			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a Blade of Chaos */
			object_prep(q_ptr, lookup_kind(k_info[arena_shouhin[p_ptr->arena_number]].tval, k_info[arena_shouhin[p_ptr->arena_number]].sval));

			apply_magic(q_ptr, object_level, FALSE, FALSE, FALSE, FALSE);

#ifdef USE_SCRIPT
			q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}

		if (p_ptr->arena_number > MAX_ARENA_MONS) p_ptr->arena_number++;
		p_ptr->arena_number++;
		if (record_arena) do_cmd_write_nikki(NIKKI_ARENA, p_ptr->arena_number, m_name);
	}

	if (m_idx == p_ptr->riding)
	{
		if (rakuba(-1, FALSE))
		{
#ifdef JP
msg_print("���̤���Ȥ��줿��");
#else
			msg_print("You have fallen from your riding pet.");
#endif
		}
	}

	/* Drop a dead corpse? */
	if ((randint(r_ptr->flags1 & RF1_UNIQUE ? 1 : 4) == 1) &&
	    ((r_ptr->flags9 & RF9_DROP_CORPSE) ||
        (r_ptr->flags9 & RF9_DROP_SKELETON)) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle || ((m_ptr->r_idx == today_mon) && is_pet(m_ptr))))
	{
		/* Assume skeleton */
		bool corpse = FALSE;

		/*
		 * We cannot drop a skeleton? Note, if we are in this check,
		 * we *know* we can drop at least a corpse or a skeleton
		 */
		if (!(r_ptr->flags9 & RF9_DROP_SKELETON))
			corpse = TRUE;
		else if ((r_ptr->flags9 & RF9_DROP_CORPSE) && (r_ptr->flags1 && RF1_UNIQUE))
			corpse = TRUE;

		/* Else, a corpse is more likely unless we did a "lot" of damage */
		else if (r_ptr->flags9 & RF9_DROP_CORPSE)
		{
			/* Lots of damage in one blow */
			if ((0 - ((m_ptr->maxhp) / 4)) > m_ptr->hp)
			{
				if (randint(5) == 1) corpse = TRUE;
			}
			else
			{
				if (randint(5) != 1) corpse = TRUE;
			}
		}

		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make a Blade of Chaos */
		object_prep(q_ptr, lookup_kind(TV_CORPSE, (corpse ? SV_CORPSE : SV_SKELETON)));

		apply_magic(q_ptr, object_level, FALSE, FALSE, FALSE, FALSE);

		q_ptr->pval = m_ptr->r_idx;

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	/* Drop objects being carried */
	monster_drop_carried_objects(m_ptr);

	/*
	 * Mega^3-hack: killing a 'Warrior of the Dawn' is likely to
	 * spawn another in the fallen one's place!
	 */
	if (m_ptr->r_idx == MON_DAWN &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		if (randint(7) != 1)
		{
			int wy = py, wx = px;
			int attempts = 100;
			bool pet = is_pet(m_ptr);

			do
			{
				scatter(&wy, &wx, py, px, 20, 0);
			}
			while (!(in_bounds(wy, wx) && cave_floor_bold(wy, wx)) && --attempts);

			if (attempts > 0)
			{
				if (summon_specific((pet ? -1 : 0), wy, wx, 100, SUMMON_DAWN, FALSE, is_friendly(m_ptr), pet, FALSE, FALSE))
				{
					if (player_can_see_bold(wy, wx))
#ifdef JP
						msg_print("��������Τ����줿��");
#else
						msg_print("A new warrior steps forth!");
#endif

				}
			}
		}
	}

	/* Pink horrors are replaced with 2 Blue horrors */
	else if (m_ptr->r_idx == MON_PINK_HORROR &&
		 !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		bool notice = FALSE;

		for (i = 0; i < 2; i++)
		{
			int wy = y, wx = x;
			bool pet = is_pet(m_ptr);

			if (summon_specific((pet ? -1 : 0), wy, wx, 100, SUMMON_BLUE_HORROR, FALSE, is_friendly(m_ptr), pet, FALSE, FALSE))
			{
				if (player_can_see_bold(wy, wx))
					notice = TRUE;
			}
		}

		if (notice)
#ifdef JP
			msg_print("�ԥ󥯡��ۥ顼��ʬ��������");
#else
			msg_print("The Pink horror divides!");
#endif

	}
	/* One more ultra-hack: An Unmaker goes out with a big bang! */
	else if (m_ptr->r_idx == MON_UNMAKER)

	{
		int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
		(void)project(m_idx, 6, y, x, 100, GF_CHAOS, flg, -1);
	}

	/* Bloodletters of Khorne may drop a blade of chaos */
	else if (m_ptr->r_idx == MON_BLOODLETTER &&

	         (randint(100) < 15) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make a Blade of Chaos */
		object_prep(q_ptr, lookup_kind(TV_SWORD, SV_BLADE_OF_CHAOS));

		apply_magic(q_ptr, object_level, FALSE, FALSE, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if (m_ptr->r_idx == MON_RAAL &&
	    (dun_level > 9) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Activate restriction */
		if ((dun_level > 49) && one_in_(5))
			get_obj_num_hook = kind_is_good_book;
		else
			get_obj_num_hook = kind_is_book;

		/* Prepare allocation table */
		get_obj_num_prep();

		/* Make a great object */
		make_object(q_ptr, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if (m_ptr->r_idx == MON_B_DEATH_SWORD &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make a Blade of Chaos */
		object_prep(q_ptr, lookup_kind(TV_SWORD, randint(2)));

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if ((r_ptr->d_char == '|') && !(r_ptr->flags1 & RF1_DROP_CHOSEN) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Activate restriction */
		get_obj_num_hook = kind_is_sword;

		/* Prepare allocation table */
		get_obj_num_prep();

		/* Make a great object */
		make_object(q_ptr, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if ((r_ptr->d_char == '(') && (dun_level > 0) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Activate restriction */
		get_obj_num_hook = kind_is_cloak;

		/* Prepare allocation table */
		get_obj_num_prep();

		/* Make a great object */
		make_object(q_ptr, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if ((r_ptr->d_char == '/') && (dun_level > 4) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Activate restriction */
		get_obj_num_hook = kind_is_polearm;

		/* Prepare allocation table */
		get_obj_num_prep();

		/* Make a great object */
		make_object(q_ptr, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if ((r_ptr->d_char == '[') && (dun_level > 19) &&
	    !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Activate restriction */
		get_obj_num_hook = kind_is_armor;

		/* Prepare allocation table */
		get_obj_num_prep();

		/* Make a great object */
		make_object(q_ptr, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	else if ((m_ptr->r_idx == MON_A_GOLD || (m_ptr->r_idx == MON_A_SILVER && !((r_ptr->r_pkills+1)%5))) && !(p_ptr->inside_arena || p_ptr->inside_battle))
	{
		/* Get local object */
		q_ptr = &forge;

		/* Prepare to make a Blade of Chaos */
		object_prep(q_ptr, lookup_kind(TV_CHEST, 50));

		apply_magic(q_ptr, object_level, FALSE, FALSE, FALSE, FALSE);

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}
	/* Mega-Hack -- drop "winner" treasures */
	else if (r_ptr->flags1 & RF1_DROP_CHOSEN)
	{
		if (m_ptr->r_idx == MON_SERPENT)

		{
			/* Get local object */
			q_ptr = &forge;

			/* Mega-Hack -- Prepare to make "Grond" */
			object_prep(q_ptr, lookup_kind(TV_HAFTED, SV_GROND));

			/* Mega-Hack -- Mark this item as "Grond" */
			q_ptr->name1 = ART_GROND;

			/* Mega-Hack -- Actually create "Grond" */
			apply_magic(q_ptr, -1, TRUE, TRUE, TRUE, FALSE);

#ifdef USE_SCRIPT
			q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);

			/* Get local object */
			q_ptr = &forge;

			/* Mega-Hack -- Prepare to make "Morgoth" */
			object_prep(q_ptr, lookup_kind(TV_CROWN, SV_MORGOTH));

			/* Mega-Hack -- Mark this item as "Morgoth" */
			q_ptr->name1 = ART_MORGOTH;

			/* Mega-Hack -- Actually create "Morgoth" */
			apply_magic(q_ptr, -1, TRUE, TRUE, TRUE, FALSE);

#ifdef USE_SCRIPT
			q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
		else
		{
			byte a_idx = 0;
			int chance = 0;

			if (m_ptr->r_idx == MON_OBERON)
			{
				if (randint(3) == 1)
				{
					a_idx = ART_THRAIN;
					chance = 33;
				}
				else
				{
					a_idx = ART_GONDOR;
					chance = 50;
				}
			}
			else if ((m_ptr->r_idx == MON_UNICORN_ORD ||
				 m_ptr->r_idx == MON_MORGOTH ||
				 m_ptr->r_idx == MON_ONE_RING)
				&& (p_ptr->pseikaku == SEIKAKU_NAMAKE))
			{
				do
				{
					if (one_in_(3))
					{
						a_idx = ART_NAMAKE_HAMMER;
						chance = 100;
					}
					else if (one_in_(2))
					{
						a_idx = ART_NAMAKE_BOW;
						chance = 100;
					}
					else
					{
						a_idx = ART_NAMAKE_ARMOR;
						chance = 100;
					}
				}
				while (a_info[a_idx].cur_num);
			}
			else if (m_ptr->r_idx == MON_GHB)
			{
				a_idx = ART_GHB;
				chance = 100;
			}
			else if (m_ptr->r_idx == MON_STORMBRINGER)
			{
				a_idx = ART_STORMBRINGER;
				chance = 100;
			}
			else if (m_ptr->r_idx == MON_ECHIZEN)
			{
				a_idx = ART_CRIMSON;
				chance = 50;
			}
			else if (m_ptr->r_idx == MON_GANDALF)
			{
				a_idx = ART_ICANUS;
				chance = 20;
			}
			else if (m_ptr->r_idx == MON_OROCHI)
			{
				a_idx = ART_KUSANAGI;
				chance = 25;
			}
			else if (m_ptr->r_idx == MON_DWORKIN)
			{
				a_idx = ART_THRAIN;
				chance = 20;
			}
			else if (m_ptr->r_idx == MON_SAURON)
			{
				if (randint(10) == 1)
				{
					a_idx = ART_POWER;
					chance = 100;
				}
				else
				{
					a_idx = ART_AHO;
					chance = 100;
				}
			}
			else if (m_ptr->r_idx == MON_BRAND)
			{
				if (randint(3) != 1)
				{
					a_idx = ART_BRAND;
					chance = 25;
				}
				else
				{
					a_idx = ART_ANGUIREL;
					chance = 33;
				}
			}
			else if (m_ptr->r_idx == MON_CORWIN)
			{
				if (randint(3) != 1)
				{
					a_idx = ART_GRAYSWANDIR;
					chance = 33;
				}
				else
				{
					a_idx = ART_CORWIN;
					chance = 33;
				}
			}
			else if (m_ptr->r_idx == MON_SURTUR)
			{
				a_idx = ART_MORMEGIL;
				chance = 66;
			}
			else if (m_ptr->r_idx == MON_SARUMAN)
			{
				a_idx = ART_ELENDIL;
				chance = 33;
			}
			else if (m_ptr->r_idx == MON_FIONA)
			{
				a_idx = ART_BELANGIL;
				chance = 50;
			}
			else if (m_ptr->r_idx == MON_JULIAN)
			{
				a_idx = ART_CELEBORN;
				chance = 45;
			}
			else if (m_ptr->r_idx == MON_KLING)
			{
				a_idx = ART_OROME;
				chance = 40;
			}
			else if (m_ptr->r_idx == MON_GOEMON)
			{
				a_idx = ART_ZANTETSU;
				chance = 75;
			}
			else if (m_ptr->r_idx == MON_HAGEN)
			{
				a_idx = ART_NIMLOTH;
				chance = 66;
			}
			else if (m_ptr->r_idx == MON_CAIN)
			{
				a_idx = ART_ANGRIST;
				chance = 50;
			}
			else if (m_ptr->r_idx == MON_BULLGATES)
			{
				a_idx = ART_WINBLOWS;
				chance = 66;
			}
			else if (m_ptr->r_idx == MON_LUNGORTHIN)
			{
				a_idx = ART_CALRIS;
				chance = 50;
			}
			else if (m_ptr->r_idx == MON_JACK_SHADOWS)
			{
				a_idx = ART_JACK;
				chance = 15;
			}
			else if (m_ptr->r_idx == MON_DIO)
			{
				a_idx = ART_STONEMASK;
				chance = 20;
			}
			else if (m_ptr->r_idx == MON_BELD)
			{
				a_idx = ART_SOULCRUSH;
				chance = 10;
			}
			else if (m_ptr->r_idx == MON_PIP)
			{
				a_idx = ART_EXCALIBUR_J;
				chance = 50;
			}

			else if (m_ptr->r_idx == MON_SHUTEN)
			{
				a_idx = ART_SHUTEN_DOJI;
				chance = 33;
			}

			else if (m_ptr->r_idx == MON_GOTHMOG)
			{
				a_idx = ART_GOTHMOG;
				chance = 33;
			}

			else if (m_ptr->r_idx == MON_FUNDIN)
			{
				a_idx = ART_FUNDIN;
				chance = 5;
			}

			if ((a_idx > 0) && ((randint(99) < chance) || (wizard)))
			{
				if (a_info[a_idx].cur_num == 0)
				{
					/* Create the artifact */
					create_named_art(a_idx, y, x);

					a_info[a_idx].cur_num = 1;
				}
			}
		}
	}
	if ((r_ptr->flags7 & RF7_GUARDIAN) && !p_ptr->inside_battle && (d_info[dungeon_type].final_guardian == m_ptr->r_idx))
	{
		if (d_info[dungeon_type].final_artifact)
		{
			byte a_idx = d_info[dungeon_type].final_artifact;
			if (a_info[a_idx].cur_num == 0)
			{
				/* Create the artifact */
				create_named_art(a_idx, y, x);

				a_info[a_idx].cur_num = 1;
			}
		}
		else
		{
			s16b k_idx;

			if (d_info[dungeon_type].final_object)
				k_idx = d_info[dungeon_type].final_object;
			else k_idx = 198;/* Acquirement */

			/* Get local object */
			q_ptr = &forge;

			/* Prepare to make a reward */
			object_prep(q_ptr, k_idx);

			apply_magic(q_ptr, object_level, FALSE, TRUE, FALSE, FALSE);

#ifdef USE_SCRIPT
			q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, y, x);
		}
#ifdef JP
		msg_format("���ʤ���%s�����Ƥ�����",d_name+d_info[dungeon_type].name);
#else
		msg_format("You have conquered %s!",d_name+d_info[dungeon_type].name);
#endif
	}

	/* Determine how much we can drop */
	if ((r_ptr->flags1 & RF1_DROP_60) && (rand_int(100) < 60)) number++;
	if ((r_ptr->flags1 & RF1_DROP_90) && (rand_int(100) < 90)) number++;
	if  (r_ptr->flags1 & RF1_DROP_1D2) number += damroll(1, 2);
	if  (r_ptr->flags1 & RF1_DROP_2D2) number += damroll(2, 2);
	if  (r_ptr->flags1 & RF1_DROP_3D2) number += damroll(3, 2);
	if  (r_ptr->flags1 & RF1_DROP_4D2) number += damroll(4, 2);

	if (cloned) number = 0; /* Clones drop no stuff */
	if (is_pet(m_ptr) || p_ptr->inside_battle || p_ptr->inside_arena) number = 0; /* Pets drop no stuff */
	if (!drop_item && (r_ptr->d_char != '$')) number = 0;

	/* Hack -- handle creeping coins */
	coin_type = force_coin;

	/* Average dungeon and monster levels */
	object_level = (dun_level + r_ptr->level) / 2;

	/* Drop some objects */
	for (j = 0; j < number; j++)
	{
		/* Get local object */
		q_ptr = &forge;

		/* Wipe the object */
		object_wipe(q_ptr);

		/* Make Gold */
		if (do_gold && (!do_item || (rand_int(100) < 50)))
		{
			/* Make some gold */
			if (!make_gold(q_ptr)) continue;

			/* XXX XXX XXX */
			dump_gold++;
		}

		/* Make Object */
		else
		{
			/* Make an object */
			if (!make_object(q_ptr, good, great)) continue;

			/* XXX XXX XXX */
			dump_item++;
		}

#ifdef USE_SCRIPT
		q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

		/* Drop it in the dungeon */
		(void)drop_near(q_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = base_level;

	/* Reset "coin" type */
	coin_type = 0;


	/* Take note of any dropped treasure */
	if (visible && (dump_item || dump_gold))
	{
		/* Take notes on treasure */
		lore_treasure(m_idx, dump_item, dump_gold);
	}

#ifdef USE_SCRIPT
	kill_monster_callback(m_idx);
#endif /* USE_SCRIPT */

	/* Only process "Quest Monsters" */
	if (!(r_ptr->flags1 & RF1_QUESTOR)) return;
	if (p_ptr->inside_battle) return;

	/* Winner? */
	if (m_ptr->r_idx == MON_SERPENT)
	{
		/* Total winner */
		total_winner = TRUE;

		/* Redraw the "title" */
		p_ptr->redraw |= (PR_TITLE);

#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�������Ѷ����ܤξ����ԤȤʤä���");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "become *WINNER* of Hengband finly!");
#endif

		if (p_ptr->pclass == CLASS_CHAOS_WARRIOR)
		{
#ifdef JP
			msg_format("%s�����������������", chaos_patrons[p_ptr->chaos_patron]);
			msg_print("�ؤ褯��ä�����̿�μԤ衪��");
#else
			msg_format("The voice of %s booms out:", chaos_patrons[p_ptr->chaos_patron]);
			msg_print("'Thou art donst well, mortal!'");
#endif
		}

		/* Congratulations */
#ifdef JP
msg_print("*** ����ǤȤ� ***");
#else
		msg_print("*** CONGRATULATIONS ***");
#endif

#ifdef JP
msg_print("���ʤ��ϥ�����򥳥�ץ꡼�Ȥ��ޤ�����");
#else
		msg_print("You have won the game!");
#endif

#ifdef JP
msg_print("���������ä������(�������ޥ��)���Ƥ�빽�Ǥ���");
#else
		msg_print("You may retire (commit suicide) when you are ready.");
#endif

	}
}

/*
 * Modify the physical damage done to the monster.
 * (for example when it's invulnerable or shielded)
 *
 * ToDo: Accept a damage-type to calculate the modified damage from
 * things like fire, frost, lightning, poison, ... attacks.
 *
 * "type" is not yet used and should be 0.
 */
int mon_damage_mod(monster_type *m_ptr, int dam, bool is_psy_spear)
{
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	if ((r_ptr->flags3 & RF3_RES_ALL) && dam > 0)
	{
		dam /= 100;
		if((dam == 0) && (randint(3) == 1)) dam = 1;
	}

	if (m_ptr->invulner)
	{
		if (is_psy_spear)
		{
			if(!p_ptr->blind && m_ptr->ml)
			{
#ifdef JP
msg_print("�Хꥢ���ڤ���������");
#else
				msg_print("The barrier is penetrated!");
#endif
			}
		}
		else if (!(randint(PENETRATE_INVULNERABILITY) == 1))
		{
			return (0);
		}
	}
	return (dam);
}

static void get_exp_from_mon(int dam, monster_type *m_ptr)
{
	s32b         div, new_exp, new_exp_frac;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int          monnum_penarty = 0;

	if (!m_ptr->r_idx) return;
	if (is_pet(m_ptr) || p_ptr->inside_battle) return;
	else
	{
		u32b m_exp;
		u32b m_exp_h, m_exp_l;
		u32b div_h, div_l;
		if ((r_ptr->flags2 & RF2_MULTIPLY) || (m_ptr->r_idx == MON_DAWN))
		{
			monnum_penarty = r_ptr->r_pkills / 400;
			if (monnum_penarty > 8) monnum_penarty = 8;
		}
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			m_exp = (long)r_ptr->mexp * r_ptr->level;
			div = (p_ptr->max_plv+2);
		}
		else
		{
			m_exp = (long)r_ptr->mexp * r_ptr->level * extract_energy[m_ptr->mspeed];
			div = (p_ptr->max_plv+2) * extract_energy[r_ptr->speed];
		}
		m_exp_h = m_exp/0x10000L;
		m_exp_l = m_exp%0x10000L;
		m_exp_h *= dam;
		m_exp_l *= dam;
		m_exp_h += m_exp_l / 0x10000L;
		m_exp_l %= 0x10000L;

		/* real monster maxhp have effect on EXP */
		if(!(r_ptr->flags1 & RF1_FORCE_MAXHP))
		{
		  u32b maxhp = m_ptr->max_maxhp*2;
		  m_exp_h *= maxhp;
		  m_exp_l *= maxhp;
		  m_exp_h += m_exp_l / 0x10000L;
		  m_exp_l %= 0x10000L;

		  div *= r_ptr->hdice * (r_ptr->hside + 1);
		}
		if (!dun_level && ((r_ptr->flags8 & RF8_DUNGEON) || !(r_ptr->flags1 & RF1_UNIQUE))) div *= 5;
		div_h = div/0x10000L;
		div_l = div%0x10000L;
		div_h *= (m_ptr->max_maxhp*2);
		div_l *= (m_ptr->max_maxhp*2);
		div_h += div_l / 0x10000L;
		div_l %= 0x10000L;

		while (monnum_penarty)
		{
			div_h *= 4;
			div_l *= 4;
			div_h += div_l / 0x10000L;
			div_l %= 0x10000L;
			monnum_penarty--;
		}

		m_exp_l = (0x7fffffff & (m_exp_h << 16)) | m_exp_l;
		m_exp_h = m_exp_h >> 15;
		div_l = (0x7fffffff & (div_h << 16)) | div_l;
		div_h = div_h >> 15;

#define M_INT_GREATER63(h1,l1,h2,l2)  ( (h1>h2)||( (h1==h2)&&(l1>=l2)))
#define M_INT_SUB63(h1,l1, h2,l2) {h1-=h2;if(l1<l2){l1+=0x80000000;h1--;}l1-=l2;}
#define M_INT_LSHIFT63(h1,l1) {h1=(h1<<1)|(l1>>30);l1=(l1<<1)&0x7fffffff;}
#define M_INT_RSHIFT63(h1,l1) {l1=(l1>>1)|(h1<<30);h1>>=1;}
#define M_INT_DIV63(h1,l1,h2,l2,result) \
		do{ \
	          int bit=1; \
                  result = 0; \
		  while( M_INT_GREATER63(h1,l1, h2, l2) ){M_INT_LSHIFT63(h2, l2); bit<<=1;} \
		  for(bit>>=1; bit>=1; bit>>=1){ \
		    M_INT_RSHIFT63(h2, l2); \
		    if(M_INT_GREATER63(h1, l1, h2, l2)) \
		      {result|=bit;M_INT_SUB63(h1, l1, h2, l2);} \
		  } \
                } while(0)

		/* Give some experience for the kill */
		M_INT_DIV63(m_exp_h, m_exp_l, div_h, div_l, new_exp);

		/* Handle fractional experience */
		/* multiply 0x10000L to remainder */
		m_exp_h = (m_exp_h<<16) | (m_exp_l>>15);
		m_exp_l <<= 16;
		M_INT_DIV63(m_exp_h, m_exp_l, div_h, div_l, new_exp_frac);
		new_exp_frac += p_ptr->exp_frac;
		/* Keep track of experience */
		if (new_exp_frac >= 0x10000L)
		{
			new_exp++;
			p_ptr->exp_frac = (u16b)(new_exp_frac - 0x10000L);
		}
		else
		{
			p_ptr->exp_frac = (u16b)new_exp_frac;
		}

		/* Gain experience */
		gain_exp(new_exp);
	}
}



/*
 * Decreases monsters hit points, handling monster death.
 *
 * We return TRUE if the monster has been killed (and deleted).
 *
 * We announce monster death (using an optional "death message"
 * if given, and a otherwise a generic killed/destroyed message).
 *
 * Only "physical attacks" can induce the "You have slain" message.
 * Missile and Spell attacks will induce the "dies" message, or
 * various "specialized" messages.  Note that "You have destroyed"
 * and "is destroyed" are synonyms for "You have slain" and "dies".
 *
 * Hack -- unseen monsters yield "You have killed it." message.
 *
 * Added fear (DGK) and check whether to print fear messages -CWS
 *
 * Made name, sex, and capitalization generic -BEN-
 *
 * As always, the "ghost" processing is a total hack.
 *
 * Hack -- we "delay" fear messages by passing around a "fear" flag.
 *
 * XXX XXX XXX Consider decreasing monster experience over time, say,
 * by using "(m_exp * m_lev * (m_lev)) / (p_lev * (m_lev + n_killed))"
 * instead of simply "(m_exp * m_lev) / (p_lev)", to make the first
 * monster worth more than subsequent monsters.  This would also need
 * to induce changes in the monster recall code.
 */
bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note)
{
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	monster_type    exp_mon;

	/* Innocent until proven otherwise */
	bool        innocent = TRUE, thief = FALSE;
	int         i;
	int         expdam;

	COPY(&exp_mon, m_ptr, monster_type);
	if (!(r_ptr->flags7 & RF7_KILL_EXP))
	{
		expdam = (m_ptr->hp > dam) ? dam : m_ptr->hp;
		if (r_ptr->flags6 & RF6_HEAL) expdam = (expdam+1) * 2 / 3;

		get_exp_from_mon(expdam, &exp_mon);
	}

	/* Redraw (later) if needed */
	if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
	if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

	/* Wake it up */
	m_ptr->csleep = 0;

	/* Hack - Cancel any special player stealth magics. -LM- */
	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		set_superstealth(FALSE);
	}

	/* Hurt it */
	m_ptr->hp -= dam;

	/* It is dead now */
	if (m_ptr->hp < 0)
	{
		char m_name[80];

		/* Extract monster name */
		monster_desc(m_name, m_ptr, 0x100);

		/* Don't kill Amberites */
		if ((r_ptr->flags3 & RF3_AMBERITE) && (randint(2) == 1))
		{
			int curses = 1 + randint(3);
			bool stop_ty = FALSE;
			int count = 0;

#ifdef JP
msg_format("%^s�϶�������μ����򤢤ʤ��ˤ�������", m_name);
#else
			msg_format("%^s puts a terrible blood curse on you!", m_name);
#endif

			curse_equipment(100, 50);

			do
			{
				stop_ty = activate_ty_curse(stop_ty, &count);
			}
			while (--curses);
		}

		if (r_ptr->flags2 & RF2_CAN_SPEAK)
		{
			char line_got[1024];

			/* Dump a message */
#ifdef JP
			if (!get_rnd_line("mondeath_j.txt", m_ptr->r_idx, line_got))
#else
			if (!get_rnd_line("mondeath.txt", m_ptr->r_idx, line_got))
#endif

				msg_format("%^s %s", m_name, line_got);
		}

		if (!(d_info[dungeon_type].flags1 & DF1_BEGINNER))
		{
			if (!dun_level && !ambush_flag && !p_ptr->inside_arena)
			{
				chg_virtue(V_VALOUR, -1);
			}
			else if (r_ptr->level > dun_level)
			{
				if (randint(10) <= (r_ptr->level - dun_level))
					chg_virtue(V_VALOUR, 1);
			}
			if (r_ptr->level > 60)
			{
				chg_virtue(V_VALOUR, 1);
			}
			if (r_ptr->level >= 2 * (p_ptr->lev+1))
				chg_virtue(V_VALOUR, 2);
		}

		if ((r_ptr->flags1 & RF1_UNIQUE) && ((r_ptr->flags3 & RF3_EVIL) ||
			(r_ptr->flags3 & RF3_GOOD)))
			
			chg_virtue(V_HARMONY, 2);

		if ((r_ptr->flags1 & RF1_UNIQUE) && (r_ptr->flags3 & RF3_GOOD))
		{
			chg_virtue(V_UNLIFE, 2);
			chg_virtue(V_VITALITY, -2);
		}

		if ((r_ptr->flags1 & RF1_UNIQUE) & (randint(3)==1))
			chg_virtue(V_INDIVIDUALISM, -1);

		if (m_ptr->r_idx == MON_BEGGAR || m_ptr->r_idx == MON_LEPER)
		{
			chg_virtue(V_COMPASSION, -1);
		}

		if ((r_ptr->flags3 & RF3_GOOD) &&
			((r_ptr->level) / 10 + (3 * dun_level) >= randint(100)))
			
			chg_virtue(V_UNLIFE, 1);

		if (r_ptr->d_char == 'A')
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_FAITH, -2);
			else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint(100))
			{
				if (r_ptr->flags3 & RF3_GOOD) chg_virtue(V_FAITH, -1);
				else chg_virtue(V_FAITH, 1);
			}
		}
		else if (r_ptr->flags3 & RF3_DEMON)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_FAITH, 2);
			else if ((r_ptr->level) / 10 + (3 * dun_level) >= randint(100))
				chg_virtue(V_FAITH, 1);
		}

		if ((r_ptr->flags3 & RF3_UNDEAD) && (r_ptr->flags1 & RF1_UNIQUE))
			chg_virtue(V_VITALITY, 2);

		if (r_ptr->r_deaths)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				chg_virtue(V_HONOUR, 10);
			}
			else if ((r_ptr->level) / 10 + (2 * dun_level) >= randint(100))
			{
				chg_virtue(V_HONOUR, 1);
			}
		}
		if ((r_ptr->flags2 & RF2_MULTIPLY) && (r_ptr->r_pkills > 1000) && one_in_(10))
		{
			chg_virtue(V_VALOUR, -1);
		}
		
		for (i = 0; i < 4; i++)
		{
			if(r_ptr->blow[i].d_dice != 0) innocent = FALSE; /* Murderer! */
		
			if ((r_ptr->blow[i].effect == RBE_EAT_ITEM)
				|| (r_ptr->blow[i].effect == RBE_EAT_GOLD))
			
				thief = TRUE; /* Thief! */
		}

		/* The new law says it is illegal to live in the dungeon */
		if (r_ptr->level != 0) innocent = FALSE;

		if (thief)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				chg_virtue(V_JUSTICE, 3);
			else if (1+((r_ptr->level) / 10 + (2 * dun_level))
				>= randint(100))
				
				chg_virtue(V_JUSTICE, 1);
		}
		else if (innocent)
		{
			chg_virtue (V_JUSTICE, -1);
		}

		if ((r_ptr->flags3 & RF3_ANIMAL) && !(r_ptr->flags3 & RF3_EVIL) && !(r_ptr->flags4 & ~(RF4_NOMAGIC_MASK))  && !(r_ptr->flags5 & ~(RF5_NOMAGIC_MASK)) && !(r_ptr->flags6 & ~(RF6_NOMAGIC_MASK)))
		{
			if (randint(4)==1) chg_virtue(V_NATURE, -1);
		}

		if((r_ptr->flags1 & RF1_UNIQUE) && record_destroy_uniq)
			do_cmd_write_nikki(NIKKI_UNIQUE, 0, r_name + r_ptr->name);

		/* Make a sound */
		sound(SOUND_KILL);

		/* Death by Missile/Spell attack */
		if (note)
		{
			msg_format("%^s%s", m_name, note);
		}

		/* Death by physical attack -- invisible monster */
		else if (!m_ptr->ml)
		{
#ifdef JP
			if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				msg_format("���ä���������%s�򻦤�����", m_name);
			else
msg_format("%s�򻦤�����", m_name);
#else
				msg_format("You have killed %s.", m_name);
#endif

		}

		/* Death by Physical attack -- non-living monster */
		else if (!monster_living(r_ptr))
		{
			int i;
			bool explode = FALSE;

			for (i = 0; i < 4; i++)
			{
				if (r_ptr->blow[i].method == RBM_EXPLODE) explode = TRUE;
			}

			/* Special note at death */
			if (explode)
#ifdef JP
msg_format("%s����ȯ����ʴ���ˤʤä���", m_name);
#else
				msg_format("%s explodes into tiny shreds.", m_name);
#endif
			else
			{
#ifdef JP
				if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_format("���ä���������%s���ݤ�����", m_name);
				else
msg_format("%s���ݤ�����", m_name);
#else
				msg_format("You have destroyed %s.", m_name);
#endif
			}
		}

		/* Death by Physical attack -- living monster */
		else
		{
#ifdef JP
			if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
				msg_format("���ä���������%s������ä���", m_name);
			else
msg_format("%s������ä���", m_name);
#else
				msg_format("You have slain %s.", m_name);
#endif

		}
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			for (i = 0; i < MAX_KUBI; i++)
			{
				if ((kubi_r_idx[i] == m_ptr->r_idx) && !(m_ptr->mflag2 & MFLAG_CHAMELEON))
				{
#ifdef JP
msg_format("%s�μ�ˤϾ޶⤬�����äƤ��롣", m_name);
#else
					msg_format("There is a price on %s's head.", m_name);
#endif
					break;
				}
			}
		}

		if (r_ptr->flags7 & RF7_KILL_EXP)
			get_exp_from_mon((long)m_ptr->max_maxhp*2, &exp_mon);
		else
			get_exp_from_mon(((long)m_ptr->max_maxhp+1L) * 9L / 10L, &exp_mon);

		/* Generate treasure */
		monster_death(m_idx, TRUE);
		if (m_ptr->mflag2 & MFLAG_CHAMELEON)
		{
			if (r_ptr->flags1 & RF1_UNIQUE)
				r_ptr = &r_info[MON_CHAMELEON_K];
			else
				r_ptr = &r_info[MON_CHAMELEON];
			if (r_ptr->r_sights < MAX_SHORT) r_ptr->r_sights++;
		}

		/* When the player kills a Unique, it stays dead */
		if (r_ptr->flags1 & RF1_UNIQUE) r_ptr->max_num = 0;

		/* When the player kills a Nazgul, it stays dead */
		if (r_ptr->flags7 & RF7_UNIQUE_7) r_ptr->max_num--;

		/* Recall even invisible uniques or winners */
		if (m_ptr->ml || (r_ptr->flags1 & RF1_UNIQUE))
		{
			/* Count kills this life */
			if ((m_ptr->mflag2 & MFLAG_KAGE) && (r_info[MON_KAGE].r_pkills < MAX_SHORT)) r_info[MON_KAGE].r_pkills++;
			else if (r_ptr->r_pkills < MAX_SHORT) r_ptr->r_pkills++;

			/* Count kills in all lives */
			if ((m_ptr->mflag2 & MFLAG_KAGE) && (r_info[MON_KAGE].r_tkills < MAX_SHORT)) r_info[MON_KAGE].r_tkills++;
			else if (r_ptr->r_tkills < MAX_SHORT) r_ptr->r_tkills++;

			/* Hack -- Auto-recall */
			monster_race_track((m_ptr->mflag2 & MFLAG_KAGE), m_ptr->r_idx);
		}

		if ((m_ptr->r_idx == MON_BANOR) || (m_ptr->r_idx == MON_LUPART))
		{
			r_info[MON_BANORLUPART].max_num = 0;
			r_info[MON_BANORLUPART].r_pkills++;
			if (r_info[MON_BANORLUPART].r_tkills < MAX_SHORT) r_info[MON_BANORLUPART].r_tkills++;
		}

		if (m_ptr->r_idx == MON_BANORLUPART)
		{
			r_info[MON_BANOR].max_num = 0;
			r_info[MON_BANOR].r_pkills++;
			if (r_info[MON_BANOR].r_tkills < MAX_SHORT) r_info[MON_BANOR].r_tkills++;
			r_info[MON_LUPART].max_num = 0;
			r_info[MON_LUPART].r_pkills++;
			if (r_info[MON_LUPART].r_tkills < MAX_SHORT) r_info[MON_LUPART].r_tkills++;
		}

		/* Mega hack : replace IKETA to BIKETAL */
		if ((m_ptr->r_idx == MON_IKETA) &&
		    !(p_ptr->inside_arena || p_ptr->inside_battle))
		{
			int dummy_y = m_ptr->fy;
			int dummy_x = m_ptr->fx;
			bool friend = is_friendly(m_ptr);
			bool pet = is_pet(m_ptr);

			/* Delete the monster */
			delete_monster_idx(m_idx);

			if (summon_named_creature(dummy_y, dummy_x, MON_BIKETAL, FALSE, FALSE, friend, pet))
			{
#ifdef JP
				msg_print("�֥ϥ��åϥåϥåϡ����䤬�Х��������������");
#else
				msg_print("Uwa-hahaha!  *I* am Biketal!");
#endif
			}
		}
		else
		{
			/* Delete the monster */
			delete_monster_idx(m_idx);
		}

		/* Not afraid */
		(*fear) = FALSE;

		/* Monster is dead */
		return (TRUE);
	}


#ifdef ALLOW_FEAR

	/* Mega-Hack -- Pain cancels fear */
	if (m_ptr->monfear && (dam > 0))
	{
		int tmp = randint(dam);

		/* Cure a little fear */
		if (tmp < m_ptr->monfear)
		{
			/* Reduce fear */
			m_ptr->monfear -= tmp;
		}

		/* Cure all the fear */
		else
		{
			/* Cure fear */
			m_ptr->monfear = 0;

			/* No more fear */
			(*fear) = FALSE;
		}
	}

	/* Sometimes a monster gets scared by damage */
	if (!m_ptr->monfear && !(r_ptr->flags3 & (RF3_NO_FEAR)))
	{
		int percentage;

		/* Percentage of fully healthy */
		percentage = (100L * m_ptr->hp) / m_ptr->maxhp;

		/*
		 * Run (sometimes) if at 10% or less of max hit points,
		 * or (usually) when hit for half its current hit points
		 */
		if (((percentage <= 10) && (rand_int(10) < percentage)) ||
		    ((dam >= m_ptr->hp) && (rand_int(100) < 80)))
		{
			/* Hack -- note fear */
			(*fear) = TRUE;

			/* XXX XXX XXX Hack -- Add some timed fear */
			m_ptr->monfear = (randint(10) +
			                  (((dam >= m_ptr->hp) && (percentage > 7)) ?
			                   20 : ((11 - percentage) * 5)));
		}
	}

#endif

#if 0
	if (p_ptr->riding && (p_ptr->riding == m_idx) && (dam > 0))
	{
		char m_name[80];

		/* Extract monster name */
		monster_desc(m_name, m_ptr, 0);

		if (m_ptr->hp > m_ptr->maxhp/3) dam = (dam + 1) / 2;
		if (rakuba((dam > 200) ? 200 : dam, FALSE))
		{
#ifdef JP
msg_format("%^s�˿�����Ȥ��줿��", m_name);
#else
				msg_format("%^s has thrown you off!", m_name);
#endif
		}
	}
#endif

	/* Not dead yet */
	return (FALSE);
}



/*
 * Calculates current boundaries
 * Called below and from "do_cmd_locate()".
 */
void panel_bounds(void)
{
	panel_row_min = panel_row * (SCREEN_HGT / 2);
	panel_row_max = panel_row_min + SCREEN_HGT - 1;
	panel_row_prt = panel_row_min - 1;
	panel_col_min = panel_col * (SCREEN_WID / 2);
	panel_col_max = panel_col_min + SCREEN_WID - 1;
	panel_col_prt = panel_col_min - 13;
}


/*
 * Calculates current boundaries
 * Called below and from "do_cmd_locate()".
 * Modified for "centering player on screen"
 */
void panel_bounds_center(void)
{
	panel_row = panel_row_min / (SCREEN_HGT / 2);
	panel_row_max = panel_row_min + SCREEN_HGT - 1;
	panel_row_prt = panel_row_min - 1;
	panel_col = panel_col_min / (SCREEN_WID / 2);
	panel_col_max = panel_col_min + SCREEN_WID - 1;
	panel_col_prt = panel_col_min - 13;
}


/*
 * Handle a request to change the current panel
 *
 * Return TRUE if the panel was changed.
 *
 * Also used in do_cmd_locate
 */
bool change_panel(int dy, int dx)
{
	/* Apply the motion */
	int y = panel_row_min + dy * (SCREEN_HGT / 2);
	int x = panel_col_min + dx * (SCREEN_WID / 2);

	/* Verify the row */
	if (y > max_panel_rows * (SCREEN_HGT / 2)) y = max_panel_rows * (SCREEN_HGT / 2);
	else if (y < 0) y = 0;

	/* Verify the col */
	if (x > max_panel_cols * (SCREEN_WID / 2)) x = max_panel_cols * (SCREEN_WID / 2);
	else if (x < 0) x = 0;

	/* Handle "changes" */
	if ((y != panel_row_min) || (x != panel_col_min))
	{
		/* Save the new panel info */
		panel_row_min = y;
		panel_col_min = x;

		/* Recalculate the boundaries */
		panel_bounds_center();

		/* Update stuff */
		p_ptr->update |= (PU_MONSTERS);

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Handle stuff */
		handle_stuff();

		/* Success */
		return (TRUE);
	}

	/* No change */
	return (FALSE);
}


/*
 * Given an row (y) and col (x), this routine detects when a move
 * off the screen has occurred and figures new borders. -RAK-
 *
 * "Update" forces a "full update" to take place.
 *
 * The map is reprinted if necessary, and "TRUE" is returned.
 */
void verify_panel(void)
{
	int y = py;
	int x = px;

	/* Center on player */
	if (center_player && (center_running || !running))
	{
		int prow_min;
		int pcol_min;

		int max_prow_min = max_panel_rows * (SCREEN_HGT / 2);
		int max_pcol_min = max_panel_cols * (SCREEN_WID / 2);

		/* Center vertically */
		prow_min = y - SCREEN_HGT / 2;
		if (prow_min > max_prow_min) prow_min = max_prow_min;
		else if (prow_min < 0) prow_min = 0;

		/* Center horizontally */
		pcol_min = x - SCREEN_WID / 2;
		if (pcol_min > max_pcol_min) pcol_min = max_pcol_min;
		else if (pcol_min < 0) pcol_min = 0;

		/* Check for "no change" */
		if ((prow_min == panel_row_min) && (pcol_min == panel_col_min)) return;

		/* Save the new panel info */
		panel_row_min = prow_min;
		panel_col_min = pcol_min;

		/* Recalculate the boundaries */
		panel_bounds_center();
	}

	else
  	{
		int prow = panel_row;
		int pcol = panel_col;

		/* Scroll screen when 2 grids from top/bottom edge */
		if ((y < panel_row_min + 2) || (y > panel_row_max - 2))
		{
			prow = ((y - SCREEN_HGT / 4) / (SCREEN_HGT / 2));
			if (prow > max_panel_rows) prow = max_panel_rows;
			else if (prow < 0) prow = 0;
		}

		/* Scroll screen when 4 grids from left/right edge */
		if ((x < panel_col_min + 4) || (x > panel_col_max - 4))
		{
			pcol = ((x - SCREEN_WID / 4) / (SCREEN_WID / 2));
			if (pcol > max_panel_cols) pcol = max_panel_cols;
			else if (pcol < 0) pcol = 0;
		}

		/* Check for "no change" */
		if ((prow == panel_row) && (pcol == panel_col)) return;

		/* Hack -- optional disturb on "panel change" */
		if (disturb_panel && !center_player) disturb(0, 0);

		/* Save the new panel info */
		panel_row = prow;
		panel_col = pcol;

		/* Recalculate the boundaries */
		panel_bounds();
	}

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/*
 * Monster health description
 */
cptr look_mon_desc(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	bool         living;
	int          perc;


	/* Determine if the monster is "living" */
	living = monster_living(r_ptr);


	/* Healthy monsters */
	if (m_ptr->hp >= m_ptr->maxhp)
	{
		/* No damage */
#ifdef JP
return (living ? "̵��" : "̵���᡼��");
#else
		return (living ? "unhurt" : "undamaged");
#endif

	}


	/* Calculate a health "percentage" */
	perc = 100L * m_ptr->hp / m_ptr->maxhp;

	if (perc >= 60)
	{
#ifdef JP
return (living ? "�ڽ�" : "�����᡼��");
#else
		return (living ? "somewhat wounded" : "somewhat damaged");
#endif

	}

	if (perc >= 25)
	{
#ifdef JP
return (living ? "���" : "����᡼��");
#else
		return (living ? "wounded" : "damaged");
#endif

	}

	if (perc >= 10)
	{
#ifdef JP
return (living ? "�Ž�" : "����᡼��");
#else
		return (living ? "badly wounded" : "badly damaged");
#endif

	}

#ifdef JP
return (living ? "Ⱦ��Ⱦ��" : "�ݤ줫��");
#else
	return (living ? "almost dead" : "almost destroyed");
#endif

}



/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort_aux(vptr u, vptr v, int p, int q)
{
	int z, a, b;

	/* Done sort */
	if (p >= q) return;

	/* Pivot */
	z = p;

	/* Begin */
	a = p;
	b = q;

	/* Partition */
	while (TRUE)
	{
		/* Slide i2 */
		while (!(*ang_sort_comp)(u, v, b, z)) b--;

		/* Slide i1 */
		while (!(*ang_sort_comp)(u, v, z, a)) a++;

		/* Done partition */
		if (a >= b) break;

		/* Swap */
		(*ang_sort_swap)(u, v, a, b);

		/* Advance */
		a++, b--;
	}

	/* Recurse left side */
	ang_sort_aux(u, v, p, b);

	/* Recurse right side */
	ang_sort_aux(u, v, b+1, q);
}


/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort(vptr u, vptr v, int n)
{
	/* Sort the array */
	ang_sort_aux(u, v, 0, n-1);
}



/*** Targeting Code ***/


/*
 * Determine is a monster makes a reasonable target
 *
 * The concept of "targeting" was stolen from "Morgul" (?)
 *
 * The player can target any location, or any "target-able" monster.
 *
 * Currently, a monster is "target_able" if it is visible, and if
 * the player can hit it with a projection, and the player is not
 * hallucinating.  This allows use of "use closest target" macros.
 *
 * Future versions may restrict the ability to target "trappers"
 * and "mimics", but the semantics is a little bit weird.
 */
bool target_able(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	/* Monster must be alive */
	if (!m_ptr->r_idx) return (FALSE);

	/* Hack -- no targeting hallucinations */
	if (p_ptr->image) return (FALSE);

	/* Monster must be visible */
	if (!m_ptr->ml) return (FALSE);

	if (p_ptr->riding && (p_ptr->riding == m_idx)) return (TRUE);

	/* Monster must be projectable */
	if (!projectable(py, px, m_ptr->fy, m_ptr->fx)) return (FALSE);

	/* XXX XXX XXX Hack -- Never target trappers */
	/* if (CLEAR_ATTR && (CLEAR_CHAR)) return (FALSE); */

	/* Assume okay */
	return (TRUE);
}




/*
 * Update (if necessary) and verify (if possible) the target.
 *
 * We return TRUE if the target is "okay" and FALSE otherwise.
 */
bool target_okay(void)
{
	/* Accept stationary targets */
	if (target_who < 0) return (TRUE);

	/* Check moving targets */
	if (target_who > 0)
	{
		/* Accept reasonable targets */
		if (target_able(target_who))
		{
			monster_type *m_ptr = &m_list[target_who];

			/* Acquire monster location */
			target_row = m_ptr->fy;
			target_col = m_ptr->fx;

			/* Good target */
			return (TRUE);
		}
	}

	/* Assume no target */
	return (FALSE);
}



/*
 * Sorting hook -- comp function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by double-distance to the player.
 */
static bool ang_sort_comp_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	int da, db, kx, ky;

	/* Absolute distance components */
	kx = x[a]; kx -= px; kx = ABS(kx);
	ky = y[a]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	da = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Absolute distance components */
	kx = x[b]; kx -= px; kx = ABS(kx);
	ky = y[b]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	db = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Compare the distances */
	return (da <= db);
}


/*
 * Sorting hook -- swap function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by distance to the player.
 */
static void ang_sort_swap_distance(vptr u, vptr v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	byte temp;

	/* Swap "x" */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;

	/* Swap "y" */
	temp = y[a];
	y[a] = y[b];
	y[b] = temp;
}



/*
 * Hack -- help "select" a location (see below)
 */
static s16b target_pick(int y1, int x1, int dy, int dx)
{
	int i, v;

	int x2, y2, x3, y3, x4, y4;

	int b_i = -1, b_v = 9999;


	/* Scan the locations */
	for (i = 0; i < temp_n; i++)
	{
		/* Point 2 */
		x2 = temp_x[i];
		y2 = temp_y[i];

		/* Directed distance */
		x3 = (x2 - x1);
		y3 = (y2 - y1);

		/* Verify quadrant */
		if (dx && (x3 * dx <= 0)) continue;
		if (dy && (y3 * dy <= 0)) continue;

		/* Absolute distance */
		x4 = ABS(x3);
		y4 = ABS(y3);

		/* Verify quadrant */
		if (dy && !dx && (x4 > y4)) continue;
		if (dx && !dy && (y4 > x4)) continue;

		/* Approximate Double Distance */
		v = ((x4 > y4) ? (x4 + x4 + y4) : (y4 + y4 + x4));

		/* XXX XXX XXX Penalize location */

		/* Track best */
		if ((b_i >= 0) && (v >= b_v)) continue;

		/* Track best */
		b_i = i; b_v = v;
	}

	/* Result */
	return (b_i);
}


/*
 * Hack -- determine if a given location is "interesting"
 */
static bool target_set_accept(int y, int x)
{
	cave_type *c_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Player grid is always interesting */
	if ((y == py) && (x == px)) return (TRUE);


	/* Handle hallucination */
	if (p_ptr->image) return (FALSE);


	/* Examine the grid */
	c_ptr = &cave[y][x];

	/* Visible monsters */
	if (c_ptr->m_idx)
	{
		monster_type *m_ptr = &m_list[c_ptr->m_idx];

		/* Visible monsters */
		if (m_ptr->ml) return (TRUE);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized object */
		if (o_ptr->marked) return (TRUE);
	}

	/* Interesting memorized features */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Notice glyphs */
		if (c_ptr->feat == FEAT_GLYPH) return (TRUE);
		if (c_ptr->feat == FEAT_MINOR_GLYPH) return (TRUE);
		if (c_ptr->feat == FEAT_MIRROR) return (TRUE);

		/* Notice the Pattern */
		if ((c_ptr->feat <= FEAT_PATTERN_XTRA2) &&
		    (c_ptr->feat >= FEAT_PATTERN_START))
			return (TRUE);

		/* Notice doors */
		if (c_ptr->feat == FEAT_OPEN) return (TRUE);
		if (c_ptr->feat == FEAT_BROKEN) return (TRUE);

		/* Notice stairs */
		if (c_ptr->feat == FEAT_LESS) return (TRUE);
		if (c_ptr->feat == FEAT_MORE) return (TRUE);
		if (c_ptr->feat == FEAT_LESS_LESS) return (TRUE);
		if (c_ptr->feat == FEAT_MORE_MORE) return (TRUE);

		/* Notice shops */
		if ((c_ptr->feat >= FEAT_SHOP_HEAD) &&
		    (c_ptr->feat <= FEAT_SHOP_TAIL)) return (TRUE);

		if (c_ptr->feat == FEAT_MUSEUM) return (TRUE);

		/* Notice buildings -KMW- */
		if ((c_ptr->feat >= FEAT_BLDG_HEAD) &&
		    (c_ptr->feat <= FEAT_BLDG_TAIL)) return (TRUE);

		/* Notice traps */
		if (is_trap(c_ptr->feat)) return (TRUE);

		/* Notice doors */
		if ((c_ptr->feat >= FEAT_DOOR_HEAD) &&
		    (c_ptr->feat <= FEAT_DOOR_TAIL)) return (TRUE);

		/* Notice rubble */
		if (c_ptr->feat == FEAT_RUBBLE) return (TRUE);

		/* Notice veins with treasure */
		if (c_ptr->feat == FEAT_MAGMA_K) return (TRUE);
		if (c_ptr->feat == FEAT_QUARTZ_K) return (TRUE);

		/* Notice quest features */
		if (c_ptr->feat == FEAT_QUEST_ENTER) return (TRUE);
		if (c_ptr->feat == FEAT_QUEST_EXIT) return (TRUE);
		if (c_ptr->feat == FEAT_QUEST_DOWN) return (TRUE);
		if (c_ptr->feat == FEAT_QUEST_UP) return (TRUE);
		if (c_ptr->feat == FEAT_TOWN) return (TRUE);
		if (c_ptr->feat == FEAT_ENTRANCE) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Prepare the "temp" array for "target_set"
 *
 * Return the number of target_able monsters in the set.
 */
static void target_set_prepare(int mode)
{
	int y, x;

	/* Reset "temp" array */
	temp_n = 0;

	/* Scan the current panel */
	for (y = panel_row_min; y <= panel_row_max; y++)
	{
		for (x = panel_col_min; x <= panel_col_max; x++)
		{
			cave_type *c_ptr = &cave[y][x];

			/* Require line of sight, unless "look" is "expanded" */
			if (!expand_look && !player_has_los_bold(y, x)) continue;

			/* Require "interesting" contents */
			if (!target_set_accept(y, x)) continue;

			/* Require target_able monsters for "TARGET_KILL" */
			if ((mode & (TARGET_KILL)) && !target_able(c_ptr->m_idx)) continue;

			if ((mode & (TARGET_KILL)) && !target_pet && is_pet(&m_list[c_ptr->m_idx])) continue;

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Set the sort hooks */
	ang_sort_comp = ang_sort_comp_distance;
	ang_sort_swap = ang_sort_swap_distance;

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);

	if (p_ptr->riding && target_pet && (temp_n > 1) && (mode & (TARGET_KILL)))
	{
		byte tmp;

		tmp = temp_y[0];
		temp_y[0] = temp_y[1];
		temp_y[1] = tmp;
		tmp = temp_x[0];
		temp_x[0] = temp_x[1];
		temp_x[1] = tmp;
	}
}


/*
 * Examine a grid, return a keypress.
 *
 * The "mode" argument contains the "TARGET_LOOK" bit flag, which
 * indicates that the "space" key should scan through the contents
 * of the grid, instead of simply returning immediately.  This lets
 * the "look" command get complete information, without making the
 * "target" command annoying.
 *
 * The "info" argument contains the "commands" which should be shown
 * inside the "[xxx]" text.  This string must never be empty, or grids
 * containing monsters will be displayed with an extra comma.
 *
 * Note that if a monster is in the grid, we update both the monster
 * recall info and the health bar info to track that monster.
 *
 * Eventually, we may allow multiple objects per grid, or objects
 * and terrain features in the same grid. XXX XXX XXX
 *
 * This function must handle blindness/hallucination.
 */
static int target_set_aux(int y, int x, int mode, cptr info)
{
	cave_type *c_ptr = &cave[y][x];

	s16b this_o_idx, next_o_idx = 0;

	cptr s1, s2, s3;

	bool boring;

	int feat;

	int query;

	char out_val[MAX_NLEN+80];


	/* Repeat forever */
	while (1)
	{
		/* Paranoia */
		query = ' ';

		/* Assume boring */
		boring = TRUE;

		/* Default */
#ifdef JP
s1 = "";
#else
		s1 = "You see ";
#endif

		s2 = "";
		s3 = "";

		/* Hack -- under the player */
		if ((y == py) && (x == px))
		{
			/* Description */
#ifdef JP
			s1 = "���ʤ���";
			s2 = "�ξ�";
			s3 = "�ˤ���";
#else
			s1 = "You are ";

			/* Preposition */
			s2 = "on ";
#endif

		}
		else
		{
#ifdef JP
			s1 = "�������å�:";
#else
			s1 = "Target:";
#endif
		}

		/* Hack -- hallucination */
		if (p_ptr->image)
		{
#ifdef JP
cptr name = "������̯��ʪ";
#else
			cptr name = "something strange";
#endif


			/* Display a message */
#ifdef JP
			sprintf(out_val, "%s%s%s%s [%s]", s1, name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Stop on everything but "return" */
			if ((query != '\r') && (query != '\n')) break;

			/* Repeat forever */
			continue;
		}


		/* Actual monsters */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Visible */
			if (m_ptr->ml)
			{
				bool recall = FALSE;

				char m_name[80];

				/* Not boring */
				boring = FALSE;

				/* Get the monster name ("a kobold") */
				monster_desc(m_name, m_ptr, 0x08);

				/* Hack -- track this monster race */
				monster_race_track((m_ptr->mflag2 & MFLAG_KAGE), m_ptr->r_idx);

				/* Hack -- health bar for this monster */
				health_track(c_ptr->m_idx);

				/* Hack -- handle stuff */
				handle_stuff();

				/* Interact */
				while (1)
				{
					/* Recall */
					if (recall)
					{
						/* Save */
						screen_save();

						/* Recall on screen */
						if (m_ptr->mflag2 & MFLAG_KAGE) screen_roff(MON_KAGE,0);
						else screen_roff(m_ptr->r_idx, 0);

						/* Hack -- Complete the prompt (again) */
#ifdef JP
Term_addstr(-1, TERM_WHITE, format("  [r�� %s]", info));
#else
						Term_addstr(-1, TERM_WHITE, format("  [r,%s]", info));
#endif


						/* Command */
						query = inkey();

						/* Restore */
						screen_load();
					}

					/* Normal */
					else
					{
						cptr attitude;
						cptr tekitou;

						if (p_ptr->riding && (p_ptr->riding == c_ptr->m_idx))
						{
#ifdef JP
							 tekitou = "(�ڥå�)";
#else
							 tekitou = "(pet)";
#endif
						}
						else
						{
							 tekitou = "";
						}

						if (is_pet(m_ptr) && (p_ptr->riding != c_ptr->m_idx))
#ifdef JP
attitude = " (�ڥå�) ";
#else
							attitude = " (pet) ";
#endif

						else if (is_friendly(m_ptr))
#ifdef JP
attitude = " (ͧ��Ū) ";
#else
							attitude = " (friendly) ";
#endif

						else
#ifdef JP
attitude = " ";
#else
							attitude = " ";
#endif


						/* Describe, and prompt for recall */
{
#define M_INT_GREATER(h1,l1,h2,l2)  ( (h1>h2)||( (h1==h2)&&(l1>=l2)))
#define M_INT_SUB(h1,l1, h2,l2) {h1-=h2;if(l1<l2){l1+=0x10000;h1--;}l1-=l2;}
#define M_INT_ADD(h1,l1, h2,l2) {h1+=h2;l1+=l2;if(l1>=0x10000L){l1&=0xFFFF;h1++;}}
#define M_INT_LSHIFT(h1,l1) {h1=(h1<<1)|(l1>>15);l1=(l1<<1)&0xffff;}
#define M_INT_RSHIFT(h1,l1) {l1=(l1>>1)|((h1&1)<<15);h1>>=1;}
#define M_INT_MULT(h1,l1,mul,h2,l2) {l2=(l1*mul)&0xffff;h2=((l1*mul)>>16)+h1*mul;}
	char acount[10];
	u32b tmp_h,tmp_l;
	int bit,result;
	u32b exp_mon= (r_ptr->mexp)*(r_ptr->level);
	u32b exp_mon_h= exp_mon / (p_ptr->max_plv+2);
	u32b exp_mon_l= ((exp_mon % (p_ptr->max_plv+2))*0x10000/(p_ptr->max_plv+2))&0xFFFF;
	
	u32b exp_adv_h = player_exp[p_ptr->lev -1] * p_ptr->expfact /100;
	u32b exp_adv_l = ((player_exp[p_ptr->lev -1]%100) * p_ptr->expfact *0x10000/100)&0xFFFF;
	
	M_INT_SUB(exp_adv_h, exp_adv_l, p_ptr->exp, p_ptr->exp_frac);
	if ((p_ptr->lev>=PY_MAX_LEVEL) || (p_ptr->prace == RACE_ANDROID))
		sprintf(acount,"[**]");
	else if (!r_ptr->r_tkills || (m_ptr->mflag2 & MFLAG_KAGE))
		sprintf(acount,"[??]");
	else if (M_INT_GREATER(exp_mon_h, exp_mon_l, exp_adv_h, exp_adv_l))
		sprintf(acount,"[001]");
	else 
	{
		M_INT_MULT(exp_mon_h, exp_mon_l, 1000,tmp_h, tmp_l);
		if( M_INT_GREATER(exp_adv_h, exp_adv_l, tmp_h, tmp_l) )
			sprintf(acount,"[999]");
		else
		{
			bit=1; result=0;
			M_INT_ADD(exp_adv_h, exp_adv_l, exp_mon_h, exp_mon_l);
			M_INT_SUB(exp_adv_h, exp_adv_l, 0, 1);
			while(M_INT_GREATER(exp_adv_h, exp_adv_l, exp_mon_h,exp_mon_l))
			{
				M_INT_LSHIFT(exp_mon_h,exp_mon_l);
				bit <<= 1;
			}
			M_INT_RSHIFT(exp_mon_h,exp_mon_l);bit>>=1;
			for(;bit>=1;bit>>=1)
			{
				if(M_INT_GREATER(exp_adv_h,exp_adv_l,exp_mon_h,exp_mon_l))
				{
					result |= bit;
					M_INT_SUB(exp_adv_h,exp_adv_l,exp_mon_h,exp_mon_l);
				}
				M_INT_RSHIFT(exp_mon_h,exp_mon_l); 
			}
			sprintf(acount,"[%03d]",result);
		}
	}
	sprintf(out_val, 
#ifdef JP
		"%s%s%s(%s)%s%s%s%s%s[r�� %s]",
		acount, s1, m_name, look_mon_desc(c_ptr->m_idx), tekitou, s2, s3, 
#else
		"%s%s%s%s%s(%s)%s%s%s[r, %s]",
		acount, s1, s2, s3, m_name, look_mon_desc(c_ptr->m_idx), tekitou, 
#endif
		(m_ptr->smart & SM_CLONED ? " (clone)": ""),
		attitude,info);
}

						prt(out_val, 0, 0);

						/* Place cursor */
						move_cursor_relative(y, x);

						/* Command */
						query = inkey();
					}

					/* Normal commands */
					if (query != 'r') break;

					/* Toggle recall */
					recall = !recall;
				}

				/* Always stop at "normal" keys */
				if ((query != '\r') && (query != '\n') && (query != ' ')) break;

				/* Sometimes stop at "space" key */
				if ((query == ' ') && !(mode & (TARGET_LOOK))) break;

				/* Change the intro */
#ifdef JP
s1 = "�����";
#else
				s1 = "It is ";
#endif


				/* Hack -- take account of gender */
#ifdef JP
if (r_ptr->flags1 & (RF1_FEMALE)) s1 = "�����";
#else
				if (r_ptr->flags1 & (RF1_FEMALE)) s1 = "She is ";
#endif

#ifdef JP
else if (r_ptr->flags1 & (RF1_MALE)) s1 = "���";
#else
				else if (r_ptr->flags1 & (RF1_MALE)) s1 = "He is ";
#endif


				/* Use a preposition */
#ifdef JP
				s2 = "��";
				s3 = "���äƤ���";
#else
				s2 = "carrying ";
#endif


				/* Scan all objects being carried */
				for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					char o_name[MAX_NLEN];

					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Obtain an object description */
					object_desc(o_name, o_ptr, TRUE, 3);

					/* Describe the object */
#ifdef JP
					sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
					sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

					prt(out_val, 0, 0);
					move_cursor_relative(y, x);
					query = inkey();

					/* Always stop at "normal" keys */
					if ((query != '\r') && (query != '\n') && (query != ' ')) break;

					/* Sometimes stop at "space" key */
					if ((query == ' ') && !(mode & (TARGET_LOOK))) break;

					/* Change the intro */
#ifdef JP
s2 = "��ޤ�";
#else
					s2 = "also carrying ";
#endif

				}

				/* Double break */
				if (this_o_idx) break;

				/* Use a preposition */
#ifdef JP
                              s2 = "�ξ�";
                              s3 = "�ˤ���";
#else
				s2 = "on ";
#endif

			}
		}


#ifdef ALLOW_EASY_FLOOR

		/* Scan all objects in the grid */
		if (easy_floor)
		{
			int floor_list[23], floor_num;

			if (scan_floor(floor_list, &floor_num, y, x, 0x02))
			{
				/* Not boring */
				boring = FALSE;

				while (1)
				{
					if (floor_num == 1)
					{
						char o_name[MAX_NLEN];

						object_type *o_ptr;

						/* Acquire object */
						o_ptr = &o_list[floor_list[0]];

						/* Describe the object */
						object_desc(o_name, o_ptr, TRUE, 3);

						/* Message */
#ifdef JP
sprintf(out_val, "%s%s%s%s[%s]",
s1, o_name, s2, s3, info);
#else
						sprintf(out_val, "%s%s%s%s [%s]",
							s1, s2, s3, o_name, info);
#endif

					}
					else
					{
						/* Message */
#ifdef JP
						sprintf(out_val, "%s %d�ĤΥ����ƥ�%s%s ['x'�ǰ���, %s]",
							s1, floor_num, s2, s3, info);
#else
						sprintf(out_val, "%s%s%sa pile of %d items [x,%s]",
							s1, s2, s3, floor_num, info);
#endif

					}

					prt(out_val, 0, 0);
					move_cursor_relative(y, x);

					/* Command */
					query = inkey();

					/* Display list of items (query == "el", not "won") */
					if ((floor_num > 1) && (query == 'x'))
					{
						/* Save screen */
						screen_save();

						/* Display */
						(void)show_floor(0, y, x);

						/* Prompt */
#ifdef JP
prt("���������򲡤��ȥ���������ޤ�", 0, 0);
#else
						prt("Hit any key to continue", 0, 0);
#endif


						/* Wait */
						(void) inkey();

						/* Load screen */
						screen_load();
					}
					else
					{
						/* Stop */
						break;
					}
				}

				/* Stop */
				break;
			}
		}

#endif /* ALLOW_EASY_FLOOR */


		/* Scan all objects in the grid */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Describe it */
			if (o_ptr->marked)
			{
				char o_name[MAX_NLEN];

				/* Not boring */
				boring = FALSE;

				/* Obtain an object description */
				object_desc(o_name, o_ptr, TRUE, 3);

				/* Describe the object */
#ifdef JP
sprintf(out_val, "%s%s%s%s[%s]", s1, o_name, s2, s3, info);
#else
				sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
#endif

				prt(out_val, 0, 0);
				move_cursor_relative(y, x);
				query = inkey();

				/* Always stop at "normal" keys */
				if ((query != '\r') && (query != '\n') && (query != ' ')) break;

				/* Sometimes stop at "space" key */
				if ((query == ' ') && !(mode & TARGET_LOOK)) break;

				/* Change the intro */
#ifdef JP
s1 = "�����";
#else
				s1 = "It is ";
#endif


				/* Plurals */
#ifdef JP
if (o_ptr->number != 1) s1 = "������";
#else
				if (o_ptr->number != 1) s1 = "They are ";
#endif


				/* Preposition */
#ifdef JP
				s2 = "�ξ�";
				s3 = "�˸�����";
#else
				s2 = "on ";
#endif

			}
		}

		/* Double break */
		if (this_o_idx) break;

		if (c_ptr->mimic)
		{
			feat = c_ptr->mimic;
		}
		else
		{
			feat = f_info[c_ptr->feat].mimic;
		}

		/* Require knowledge about grid, or ability to see grid */
		if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(y, x))
		{
			/* Forget feature */
			feat = FEAT_NONE;
		}

		/* Terrain feature if needed */
		if (boring || (feat > FEAT_INVIS))
		{
			cptr name;

			/* Hack -- special handling for building doors */
			if ((feat >= FEAT_BLDG_HEAD) && (feat <= FEAT_BLDG_TAIL))
			{
				name = building[feat - FEAT_BLDG_HEAD].name;
			}
			else if (feat == FEAT_ENTRANCE)
			{
#ifdef JP
				name = format("%s(%d������)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#else
				name = format("%s(level %d)", d_text + d_info[c_ptr->special].text, d_info[c_ptr->special].mindepth);
#endif
                        }
			else if (feat == FEAT_TOWN)
			{
				name = town[c_ptr->special].name;
                        }
			else if (p_ptr->wild_mode && (feat == FEAT_FLOOR))
			{
#ifdef JP
				name = "ƻ";
#else
				name = "road";
#endif
			}
			else
			{
				name = f_name + f_info[feat].name;
			}

			/* Hack -- handle unknown grids */
#ifdef JP
if (feat == FEAT_NONE) name = "̤�Τ��Ϸ�";
#else
			if (feat == FEAT_NONE) name = "unknown grid";
#endif


			/* Pick a prefix */
			if (*s2 && ((feat >= FEAT_MINOR_GLYPH) &&
			   (feat <= FEAT_PATTERN_XTRA2)))
			{
#ifdef JP
s2 = "�ξ�";
#else
				s2 = "on ";
#endif

			}
			else if (*s2 && ((feat >= FEAT_DOOR_HEAD) &&
				(feat <= FEAT_PERM_SOLID)))
			{
#ifdef JP
s2 = "����";
#else
				s2 = "in ";
#endif

			}
			else if (*s2 && (feat == FEAT_TOWN))
			{
#ifdef JP
s2 = "����";
#else
				s2 = "in ";
#endif

			}

			/* Hack -- special introduction for store & building doors -KMW- */
			if (((feat >= FEAT_SHOP_HEAD) && (feat <= FEAT_SHOP_TAIL)) ||
			    ((feat >= FEAT_BLDG_HEAD) && (feat <= FEAT_BLDG_TAIL)) ||
			    (feat == FEAT_MUSEUM) ||
			    (feat == FEAT_ENTRANCE))
			{
#ifdef JP
s2 = "������";
#else
				s3 = "";
#endif

			}
			else if ((feat == FEAT_TOWN) || (feat == FEAT_FLOOR) || (feat == FEAT_DIRT) || (feat == FEAT_FLOWER))
			{
				s3 ="";
			}
			else
			{
				/* Pick proper indefinite article */
#ifndef JP
				s3 = (is_a_vowel(name[0])) ? "an " : "a ";
#endif
			}

			/* Display a message */
			if (wizard)
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s] %x %d %d %d %d", s1, name, s2, s3, info, c_ptr->info, c_ptr->feat, c_ptr->dist, c_ptr->cost, c_ptr->when);
#else
			sprintf(out_val, "%s%s%s%s [%s] %x %d %d %d %d", s1, s2, s3, name, info, c_ptr->info, c_ptr->feat, c_ptr->dist, c_ptr->cost, c_ptr->when);
#endif
			else
#ifdef JP
			sprintf(out_val, "%s%s%s%s[%s]", s1, name, s2, s3, info);
#else
			sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
#endif

			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Always stop at "normal" keys */
			if ((query != '\r') && (query != '\n') && (query != ' ')) break;
		}

		/* Stop on everything but "return" */
		if ((query != '\r') && (query != '\n')) break;
	}

	/* Keep going */
	return (query);
}


/*
 * Handle "target" and "look".
 *
 * Note that this code can be called from "get_aim_dir()".
 *
 * All locations must be on the current panel.  Consider the use of
 * "panel_bounds()" to allow "off-panel" targets, perhaps by using
 * some form of "scrolling" the map around the cursor.  XXX XXX XXX
 * That is, consider the possibility of "auto-scrolling" the screen
 * while the cursor moves around.  This may require changes in the
 * "update_mon()" code to allow "visibility" even if off panel, and
 * may require dynamic recalculation of the "temp" grid set.
 *
 * Hack -- targeting/observing an "outer border grid" may induce
 * problems, so this is not currently allowed.
 *
 * The player can use the direction keys to move among "interesting"
 * grids in a heuristic manner, or the "space", "+", and "-" keys to
 * move through the "interesting" grids in a sequential manner, or
 * can enter "location" mode, and use the direction keys to move one
 * grid at a time in any direction.  The "t" (set target) command will
 * only target a monster (as opposed to a location) if the monster is
 * target_able and the "interesting" mode is being used.
 *
 * The current grid is described using the "look" method above, and
 * a new command may be entered at any time, but note that if the
 * "TARGET_LOOK" bit flag is set (or if we are in "location" mode,
 * where "space" has no obvious meaning) then "space" will scan
 * through the description of the current grid until done, instead
 * of immediately jumping to the next "interesting" grid.  This
 * allows the "target" command to retain its old semantics.
 *
 * The "*", "+", and "-" keys may always be used to jump immediately
 * to the next (or previous) interesting grid, in the proper mode.
 *
 * The "return" key may always be used to scan through a complete
 * grid description (forever).
 *
 * This command will cancel any old target, even if used from
 * inside the "look" command.
 */
bool target_set(int mode)
{
	int		i, d, m, t, bd;
	int		y = py;
	int		x = px;

	bool	done = FALSE;

	bool	flag = TRUE;

	char	query;

	char	info[80];

	cave_type		*c_ptr;


	/* Cancel target */
	target_who = 0;


	/* Cancel tracking */
	/* health_track(0); */


	/* Prepare the "temp" array */
	target_set_prepare(mode);

	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{
		/* Interesting grids */
		if (flag && temp_n)
		{
			y = temp_y[m];
			x = temp_x[m];

			if (!(mode & TARGET_LOOK)) prt_path(y, x);

			/* Access */
			c_ptr = &cave[y][x];

			/* Allow target */
			if (target_able(c_ptr->m_idx))
			{
#ifdef JP
strcpy(info, "q�� t�� p�� o�� +�� -��");
#else
				strcpy(info, "q,t,p,o,+,-,<dir>");
#endif

			}

			/* Dis-allow target */
			else
			{
#ifdef JP
strcpy(info, "q�� p�� o�� +�� -��");
#else
				strcpy(info, "q,p,o,+,-,<dir>");
#endif

			}

			/* Describe and Prompt */
			query = target_set_aux(y, x, mode, info);

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no "direction" */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}  

			/* Analyze */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case 't':
				case '.':
				case '5':
				case '0':
				{
					if (target_able(c_ptr->m_idx))
					{
						health_track(c_ptr->m_idx);
						target_who = c_ptr->m_idx;
						target_row = y;
						target_col = x;
						done = TRUE;
					}
					else
					{
						bell();
					}
					break;
				}

				case ' ':
				case '*':
				case '+':
				{
					if (++m == temp_n)
					{
						m = 0;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case '-':
				{
					if (m-- == 0)
					{
						m = temp_n - 1;
						if (!expand_list) done = TRUE;
					}
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel();

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(mode);

					y = py;
					x = px;
				}

				case 'o':
				{
					flag = FALSE;
					break;
				}

				case 'm':
				{
					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					if (!d) bell();
					break;
				}
			}

			/* Hack -- move around */
			if (d)
			{
				/* Modified to scroll to monster */
				int y2 = panel_row_min;
				int x2 = panel_col_min;

				/* Find a new monster */
				i = target_pick(temp_y[m], temp_x[m], ddy[d], ddx[d]);

				/* Request to target past last interesting grid */
				while (flag && (i < 0))
				{
					/* Note the change */
					if (change_panel(ddy[d], ddx[d]))
					{
						int v = temp_y[m];
						int u = temp_x[m];

						/* Recalculate interesting grids */
						target_set_prepare(mode);

						/* Look at interesting grids */
						flag = TRUE;

						/* Find a new monster */
						i = target_pick(v, u, ddy[d], ddx[d]);

						/* Use that grid */
						if (i >= 0) m = i;
					}

					/* Nothing interesting */
					else
					{
						int dx = ddx[d];
						int dy = ddy[d];

						/* Restore previous position */
						panel_row_min = y2;
						panel_col_min = x2;
						panel_bounds_center();

						/* Update stuff */
						p_ptr->update |= (PU_MONSTERS);

						/* Redraw map */
						p_ptr->redraw |= (PR_MAP);

						/* Window stuff */
						p_ptr->window |= (PW_OVERHEAD);

						/* Handle stuff */
						handle_stuff();

						/* Recalculate interesting grids */
						target_set_prepare(mode);

						/* Look at boring grids */
						flag = FALSE;

						/* Move */
						x += dx;
						y += dy;

						/* Do not move horizontally if unnecessary */
						if (((x < panel_col_min + SCREEN_WID / 2) && (dx > 0)) ||
							 ((x > panel_col_min + SCREEN_WID / 2) && (dx < 0)))
						{
							dx = 0;
						}

						/* Do not move vertically if unnecessary */
						if (((y < panel_row_min + SCREEN_HGT / 2) && (dy > 0)) ||
							 ((y > panel_row_min + SCREEN_HGT / 2) && (dy < 0)))
						{
							dy = 0;
						}

						/* Apply the motion */
						if ((y >= panel_row_min+SCREEN_HGT) || (y < panel_row_min) ||
						    (x >= panel_col_min+SCREEN_WID) || (x < panel_col_min))
						{
							if (change_panel(dy, dx)) target_set_prepare(mode);
						}

						/* Slide into legality */
						if (x >= cur_wid-1) x = cur_wid - 2;
						else if (x <= 0) x = 1;

						/* Slide into legality */
						if (y >= cur_hgt-1) y = cur_hgt- 2;
						else if (y <= 0) y = 1;
					}
				}

				/* Use that grid */
				m = i;
			}
		}

		/* Arbitrary grids */
		else
		{
			if (!(mode & TARGET_LOOK)) prt_path(y, x);

			/* Access */
			c_ptr = &cave[y][x];

			/* Default prompt */
#ifdef JP
strcpy(info, "q�� t�� p�� m�� +�� -��");
#else
			strcpy(info, "q,t,p,m,+,-,<dir>");
#endif


			/* Describe and Prompt (enable "TARGET_LOOK") */
			query = target_set_aux(y, x, mode | TARGET_LOOK, info);

			/* Cancel tracking */
			/* health_track(0); */

			/* Assume no direction */
			d = 0;

			if (use_menu)
			{
				if (query == '\r') query = 't';
			}  

			/* Analyze the keypress */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case 't':
				case '.':
				case '5':
				case '0':
				{
					target_who = -1;
					target_row = y;
					target_col = x;
					done = TRUE;
					break;
				}

				case 'p':
				{
					/* Recenter the map around the player */
					verify_panel();

					/* Update stuff */
					p_ptr->update |= (PU_MONSTERS);

					/* Redraw map */
					p_ptr->redraw |= (PR_MAP);

					/* Window stuff */
					p_ptr->window |= (PW_OVERHEAD);

					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_prepare(mode);

					y = py;
					x = px;
				}

				case 'o':
				{
					break;
				}

				case ' ':
				case '*':
				case '+':
				case '-':
				case 'm':
				{
					flag = TRUE;

					m = 0;
					bd = 999;

					/* Pick a nearby monster */
					for (i = 0; i < temp_n; i++)
					{
						t = distance(y, x, temp_y[i], temp_x[i]);

						/* Pick closest */
						if (t < bd)
						{
							m = i;
							bd = t;
						}
					}

					/* Nothing interesting */
					if (bd == 999) flag = FALSE;

					break;
				}

				default:
				{
					/* Extract the action (if any) */
					d = get_keymap_dir(query);

					if (!d) bell();
					break;
				}
			}

			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

				/* Move */
				x += dx;
				y += dy;

				/* Do not move horizontally if unnecessary */
				if (((x < panel_col_min + SCREEN_WID / 2) && (dx > 0)) ||
					 ((x > panel_col_min + SCREEN_WID / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + SCREEN_HGT / 2) && (dy > 0)) ||
					 ((y > panel_row_min + SCREEN_HGT / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + SCREEN_HGT) || (y < panel_row_min) ||
					 (x >= panel_col_min + SCREEN_WID) || (x < panel_col_min))
				{
					if (change_panel(dy, dx)) target_set_prepare(mode);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;
			}
		}
	}

	/* Forget */
	temp_n = 0;

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Failure to set target */
	if (!target_who) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Get an "aiming direction" from the user.
 *
 * The "dir" is loaded with 1,2,3,4,6,7,8,9 for "actual direction", and
 * "0" for "current target", and "-1" for "entry aborted".
 *
 * Note that "Force Target", if set, will pre-empt user interaction,
 * if there is a usable target already set.
 *
 * Note that confusion over-rides any (explicit?) user choice.
 */
bool get_aim_dir(int *dp)
{
	int		dir;

	char	command;

	cptr	p;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

	/* Hack -- auto-target if requested */
	if (use_old_target && target_okay()) dir = 5;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		/* Confusion? */

		/* Verify */
		if (!(*dp == 5 && !target_okay()))
		{
/*			return (TRUE); */
			dir = *dp;
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "���� ('*'�ǥ������å�����, ESC������)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "���� ('5'�ǥ������åȤ�, '*'�ǥ������åȺ�����, ESC������)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
		}  

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			case ' ':
			case '\r':
			{
				if (target_set(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Extract the action (if any) */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir)
	{
		project_length = 0; /* reset to default */
		return (FALSE);
	}

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[rand_int(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���ʤ��Ϻ��𤷤Ƥ��롣");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* A "valid" direction was entered */
	return (TRUE);
}



/*
 * Request a "movement" direction (1,2,3,4,6,7,8,9) from the user,
 * and place it into "command_dir", unless we already have one.
 *
 * This function should be used for all "repeatable" commands, such as
 * run, walk, open, close, bash, disarm, spike, tunnel, etc, as well
 * as all commands which must reference a grid adjacent to the player,
 * and which may not reference the grid under the player.  Note that,
 * for example, it is no longer possible to "disarm" or "open" chests
 * in the same grid as the player.
 *
 * Direction "5" is illegal and will (cleanly) abort the command.
 *
 * This function tracks and uses the "global direction", and uses
 * that as the "desired direction", to which "confusion" is applied.
 */
bool get_rep_dir(int *dp, bool under)
{
	int dir;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
if (!get_com("���� (ESC������)? ", &ch, TRUE)) break;
#else
		if (!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if (!dir) bell();
	}

	/* Prevent weirdness */
	if ((dir == 5) && (!under)) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if (p_ptr->confused)
	{
		/* Standard confusion */
		if (rand_int(100) < 75)
		{
			/* Random direction */
			dir = ddd[rand_int(8)];
		}
	}
	else if (p_ptr->riding)
	{
		monster_type *m_ptr = &m_list[p_ptr->riding];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (m_ptr->confused)
		{
			/* Standard confusion */
			if (rand_int(100) < 75)
			{
				/* Random direction */
				dir = ddd[rand_int(8)];
			}
		}
		else if ((r_ptr->flags1 & RF1_RAND_50) && (r_ptr->flags1 & RF1_RAND_25) && (rand_int(100) < 50))
		{
			/* Random direction */
			dir = ddd[rand_int(8)];
		}
		else if ((r_ptr->flags1 & RF1_RAND_50) && (rand_int(100) < 25))
		{
			/* Random direction */
			dir = ddd[rand_int(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		if (p_ptr->confused)
		{
			/* Warn the user */
#ifdef JP
msg_print("���ʤ��Ϻ��𤷤Ƥ��롣");
#else
			msg_print("You are confused.");
#endif
		}
		else
		{
			char m_name[80];
			monster_type *m_ptr = &m_list[p_ptr->riding];

			monster_desc(m_name, m_ptr, 0);
			if (m_ptr->confused)
			{
#ifdef JP
msg_format("%s�Ϻ��𤷤Ƥ��롣", m_name);
#else
 msg_format("%^s is confusing.", m_name);

#endif
			}
			else
			{
#ifdef JP
msg_format("%s�ϻפ��̤��ư���Ƥ���ʤ���", m_name);
#else
msg_format("You cannot control %s.", m_name);
#endif
			}
		}
	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


bool get_rep_dir2(int *dp)
{
	int dir;

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = command_dir;

#ifdef ALLOW_REPEAT /* TNB */

	if (repeat_pull(dp))
	{
		dir = *dp;
/*		return (TRUE); */
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Get a direction */
	while (!dir)
	{
		char ch;

		/* Get a command (or Cancel) */
#ifdef JP
if (!get_com("���� (ESC������)? ", &ch, TRUE)) break;
#else
		if (!get_com("Direction (Escape to cancel)? ", &ch, TRUE)) break;
#endif


		/* Look up the direction */
		dir = get_keymap_dir(ch);

		/* Oops */
		if (!dir) bell();
	}

	/* Prevent weirdness */
	if (dir == 5) dir = 0;

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	command_dir = dir;

	/* Apply "confusion" */
	if (p_ptr->confused)
	{
		/* Standard confusion */
		if (rand_int(100) < 75)
		{
			/* Random direction */
			dir = ddd[rand_int(8)];
		}
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���ʤ��Ϻ��𤷤Ƥ��롣");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT /* TNB */

/*	repeat_push(dir); */
	repeat_push(command_dir);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


int get_chaos_patron(void)
{
	return ((p_ptr->age + p_ptr->sc) % MAX_PATRON);
}


void gain_level_reward(int chosen_reward)
{
	object_type *q_ptr;
	object_type forge;
	char        wrath_reason[32] = "";
	int         nasty_chance = 6;
	int         dummy = 0, dummy2 = 0;
	int         type, effect;
	cptr        reward = NULL;
	char o_name[MAX_NLEN];

	int count = 0;

	if (!chosen_reward)
	{
		if (multi_rew) return;
		else multi_rew = TRUE;
	}


	if (p_ptr->lev == 13) nasty_chance = 2;
	else if (!(p_ptr->lev % 13)) nasty_chance = 3;
	else if (!(p_ptr->lev % 14)) nasty_chance = 12;

	if (randint(nasty_chance) == 1)
		type = randint(20); /* Allow the 'nasty' effects */
	else
		type = randint(15) + 5; /* Or disallow them */

	if (type < 1) type = 1;
	if (type > 20) type = 20;
	type--;


#ifdef JP
sprintf(wrath_reason, "%s���ܤ�",
		chaos_patrons[p_ptr->chaos_patron]);
#else
	sprintf(wrath_reason, "the Wrath of %s",
		chaos_patrons[p_ptr->chaos_patron]);
#endif


	effect = chaos_rewards[p_ptr->chaos_patron][type];

	if ((randint(6) == 1) && !chosen_reward)
	{
#ifdef JP
msg_format("%^s��˫���Ȥ��Ƥ��ʤ��������Ѱۤ�������",
			chaos_patrons[p_ptr->chaos_patron]);
#else
		msg_format("%^s rewards you with a mutation!",
			chaos_patrons[p_ptr->chaos_patron]);
#endif

		(void)gain_random_mutation(0);
#ifdef JP
		reward = "�Ѱۤ�����";
#else
		reward = "mutation";
#endif
	}
	else
	{
	switch (chosen_reward ? chosen_reward : effect)
	{
		case REW_POLY_SLF:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("���򡢿����ʤ�Ѥ�ɬ�פȤ��ꡪ��");
#else
			msg_print("'Thou needst a new form, mortal!'");
#endif

			do_poly_self();
#ifdef JP
			reward = "�Ѱۤ�����";
#else
			reward = "polymorphing";
#endif
			break;
		case REW_GAIN_EXP:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("������ɤ��Ԥ����ꡪ³���衪��");
#else
			msg_print("'Well done, mortal! Lead on!'");
#endif

			if (p_ptr->prace == RACE_ANDROID)
			{
				msg_print("���������ⵯ����ʤ��ä���");
			}
			else if (p_ptr->exp < PY_MAX_EXP)
			{
				s32b ee = (p_ptr->exp / 2) + 10;
				if (ee > 100000L) ee = 100000L;
#ifdef JP
msg_print("���˷и����Ѥ���褦�ʵ������롣");
#else
				msg_print("You feel more experienced.");
#endif

				gain_exp(ee);
#ifdef JP
				reward = "�и��ͤ�����";
#else
				reward = "experience";
#endif
			}
			break;
		case REW_LOSE_EXP:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ��ͤ衢�򤽤���ͤ�������");
#else
			msg_print("'Thou didst not deserve that, slave.'");
#endif

			if (p_ptr->prace == RACE_ANDROID)
			{
				msg_print("���������ⵯ����ʤ��ä���");
			}
			else
			{
				lose_exp(p_ptr->exp / 6);
#ifdef JP
				reward = "�и��ͤ򼺤ä���";
#else
				reward = "losing experience";
#endif
			}
			break;
		case REW_GOOD_OBJ:
#ifdef JP
msg_format("%s�����������䤤��:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ椬Ϳ����ʪ�����˻Ȥ��٤�����");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(py, px, 1, FALSE, FALSE);
#ifdef JP
			reward = "����ʥ����ƥ�������줿��";
#else
			reward = "a good item";
#endif
			break;
		case REW_GREA_OBJ:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ椬Ϳ����ʪ�����˻Ȥ��٤�����");
#else
			msg_print("'Use my gift wisely.'");
#endif

			acquirement(py, px, 1, TRUE, FALSE);
#ifdef JP
			reward = "����ʤΥ����ƥ�������줿��";
#else
			reward = "an excellent item";
#endif
			break;
		case REW_CHAOS_WP:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("����ιԤ��ϵ��������ͤ��ꡣ��");
#else
			msg_print("'Thy deed hath earned thee a worthy blade.'");
#endif

			/* Get local object */
			q_ptr = &forge;
			dummy = TV_SWORD;
			switch (randint(p_ptr->lev))
			{
				case 0: case 1:
					dummy2 = SV_DAGGER;
					break;
				case 2: case 3:
					dummy2 = SV_MAIN_GAUCHE;
					break;
				case 4:
					dummy2 = SV_TANTO;
					break;
				case 5: case 6:
					dummy2 = SV_RAPIER;
					break;
				case 7: case 8:
					dummy2 = SV_SMALL_SWORD;
					break;
				case 9: case 10:
					dummy2 = SV_BASILLARD;
					break;
				case 11: case 12: case 13:
					dummy2 = SV_SHORT_SWORD;
					break;
				case 14: case 15:
					dummy2 = SV_SABRE;
					break;
				case 16: case 17:
					dummy2 = SV_CUTLASS;
					break;
				case 18:
					dummy2 = SV_WAKIZASHI;
					break;
				case 19:
					dummy2 = SV_KHOPESH;
					break;
				case 20:
					dummy2 = SV_TULWAR;
					break;
				case 21:
					dummy2 = SV_BROAD_SWORD;
					break;
				case 22: case 23:
					dummy2 = SV_LONG_SWORD;
					break;
				case 24: case 25:
					dummy2 = SV_SCIMITAR;
					break;
				case 26:
					dummy2 = SV_NINJATO;
					break;
				case 27:
					dummy2 = SV_KATANA;
					break;
				case 28: case 29:
					dummy2 = SV_BASTARD_SWORD;
					break;
				case 30:
					dummy2 = SV_GREAT_SCIMITAR;
					break;
				case 31:
					dummy2 = SV_CLAYMORE;
					break;
				case 32:
					dummy2 = SV_ESPADON;
					break;
				case 33:
					dummy2 = SV_TWO_HANDED_SWORD;
					break;
				case 34:
					dummy2 = SV_FLAMBERGE;
					break;
				case 35:
					dummy2 = SV_NO_DACHI;
					break;
				case 36:
					dummy2 = SV_EXECUTIONERS_SWORD;
					break;
				case 37:
					dummy2 = SV_ZWEIHANDER;
					break;
				case 38:
					dummy2 = SV_HAYABUSA;
					break;
				default:
					dummy2 = SV_BLADE_OF_CHAOS;
			}

			object_prep(q_ptr, lookup_kind(dummy, dummy2));
			q_ptr->to_h = 3 + randint(dun_level) % 10;
			q_ptr->to_d = 3 + randint(dun_level) % 10;
			random_resistance(q_ptr, FALSE, randint(34) + 4);
			q_ptr->name2 = EGO_CHAOTIC;

#ifdef USE_SCRIPT
			q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

			/* Drop it in the dungeon */
			(void)drop_near(q_ptr, -1, py, px);
#ifdef JP
			reward = "(����)�����������줿��";
#else
			reward = "chaos weapon";
#endif
			break;
		case REW_GOOD_OBS:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("����ιԤ��ϵ����󤤤��ͤ��ꡣ��");
#else
			msg_print("'Thy deed hath earned thee a worthy reward.'");
#endif

			acquirement(py, px, randint(2) + 1, FALSE, FALSE);
#ifdef JP
			reward = "����ʥ����ƥ�������줿��";
#else
			reward = "good items";
#endif
			break;
		case REW_GREA_OBS:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ��ͤ衢��θ��Ȥؤβ椬�ˤ���̵���󤤤򸫤뤬�褤����");
#else
			msg_print("'Behold, mortal, how generously I reward thy loyalty.'");
#endif

			acquirement(py, px, randint(2) + 1, TRUE, FALSE);
#ifdef JP
			reward = "����ʤΥ����ƥ�������줿��";
#else
			reward = "excellent items";
#endif
			break;
		case REW_TY_CURSE:
#ifdef JP
msg_format("%s�������줭�Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ��ͤ衢�������ʤꡣ��");
#else
			msg_print("'Thou art growing arrogant, mortal.'");
#endif

			(void)activate_ty_curse(FALSE, &count);
#ifdef JP
			reward = "�ҡ����������򤫤���줿��";
#else
			reward = "cursing";
#endif
			break;
		case REW_SUMMON_M:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ椬���ͤ����衢���������ʤ�Ԥ��ݤ��٤�����");
#else
			msg_print("'My pets, destroy the arrogant mortal!'");
#endif

			for (dummy = 0; dummy < randint(5) + 1; dummy++)
			{
				(void)summon_specific(0, py, px, dun_level, 0, TRUE, FALSE, FALSE, TRUE, TRUE);
			}
#ifdef JP
			reward = "��󥹥����򾤴����줿��";
#else
			reward = "summoning hostile monsters";
#endif
			break;
		case REW_H_SUMMON:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("���򡢤�궯��Ũ��ɬ�פȤ��ꡪ��");
#else
			msg_print("'Thou needst worthier opponents!'");
#endif

			activate_hi_summon(py, px, FALSE);
#ifdef JP
			reward = "��󥹥����򾤴����줿��";
#else
			reward = "summoning many hostile monsters";
#endif
			break;
		case REW_DO_HAVOC:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֻ���˲������椬��Ӥʤꡪ��");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			call_chaos();
#ifdef JP
			reward = "���������Ϥ�����������";
#else
			reward = "calling chaos";
#endif
			break;
		case REW_GAIN_ABL:
#ifdef JP
msg_format("%s�������Ĥ������:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s rings out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("��α�ޤ�Τ������ͤ衣;��������Τ��ä��󡣡�");
#else
			msg_print("'Stay, mortal, and let me mold thee.'");
#endif

			if ((randint(3) == 1) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_inc_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				do_inc_stat(rand_int(6));
#ifdef JP
			reward = "ǽ���ͤ��夬�ä���";
#else
			reward = "increasing a stat";
#endif
			break;
		case REW_LOSE_ABL:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ��ͤ衢;�����˰�ߤ��ꡣ��");
#else
			msg_print("'I grow tired of thee, mortal.'");
#endif

			if ((randint(3) == 1) && !(chaos_stats[p_ptr->chaos_patron] < 0))
				do_dec_stat(chaos_stats[p_ptr->chaos_patron]);
			else
				(void)do_dec_stat(rand_int(6));
#ifdef JP
			reward = "ǽ���ͤ������ä���";
#else
			reward = "decreasing a stat";
#endif
			break;
		case REW_RUIN_ABL:
#ifdef JP
msg_format("%s�������줭�Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s thunders:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("���򡢸������뤳�Ȥ�ؤ֤٤�����");
msg_print("���ʤ��ϰ������夯�ʤä���");
#else
			msg_print("'Thou needst a lesson in humility, mortal!'");
			msg_print("You feel less powerful!");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint(15), TRUE);
			}
#ifdef JP
			reward = "��ǽ���ͤ������ä���";
#else
			reward = "decreasing all stats";
#endif
			break;
		case REW_POLY_WND:
#ifdef JP
msg_format("%s���Ϥ������Τ򴶤�����",
#else
			msg_format("You feel the power of %s touch you.",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
			do_poly_wounds();
#ifdef JP
			reward = "�����Ѳ�������";
#else
			reward = "polymorphing wounds";
#endif
			break;
		case REW_AUGM_ABL:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ椬�����䤫�ʤ��ʪ������Ȥ뤬�褤����");
#else
			msg_print("'Receive this modest gift from me!'");
#endif

			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_inc_stat(dummy);
			}
#ifdef JP
			reward = "��ǽ���ͤ��夬�ä���";
#else
			reward = "increasing all stats";
#endif
			break;
		case REW_HURT_LOT:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֶ줷�ब�褤��̵ǽ�ʶ򤫼Ԥ衪��");
#else
			msg_print("'Suffer, pathetic fool!'");
#endif

			fire_ball(GF_DISINTEGRATE, 0, p_ptr->lev * 4, 4);
			take_hit(DAMAGE_NOESCAPE, p_ptr->lev * 4, wrath_reason, -1);
#ifdef JP
			reward = "ʬ��ε夬ȯ��������";
#else
			reward = "generating disintegration ball";
#endif
			break;
	   case REW_HEAL_FUL:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("��ᴤ뤬�褤���椬���ͤ衪��");
#else
			msg_print("'Rise, my servant!'");
#endif

			restore_level();
			(void)set_poisoned(0);
			(void)set_blind(0);
			(void)set_confused(0);
			(void)set_image(0);
			(void)set_stun(0);
			(void)set_cut(0);
			hp_player(5000);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)do_res_stat(dummy);
			}
#ifdef JP
			reward = "���Ϥ�����������";
#else
			reward = "healing";
#endif
			break;
		case REW_CURSE_WP:
			if (!buki_motteruka(INVEN_RARM)) break;
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("����������뤳�Ȥʤ��졣��");
#else
			msg_print("'Thou reliest too much on thy weapon.'");
#endif

			object_desc(o_name, &inventory[INVEN_RARM], TRUE, 0);
			(void)curse_weapon(FALSE, INVEN_RARM);
#ifdef JP
			reward = format("%s���˲����줿��", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_CURSE_AR:
			if (!inventory[INVEN_BODY].k_idx) break;
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�����ɶ����뤳�Ȥʤ��졣��");
#else
			msg_print("'Thou reliest too much on thine equipment.'");
#endif

			object_desc(o_name, &inventory[INVEN_BODY], TRUE, 0);
			(void)curse_armor();
#ifdef JP
			reward = format("%s���˲����줿��", o_name);
#else
			reward = format("destroying %s", o_name);
#endif
			break;
		case REW_PISS_OFF:
#ifdef JP
msg_format("%s�����������䤤��:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s whispers:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ���ܤꤷ�᤿�������٤�����");
#else
			msg_print("'Now thou shalt pay for annoying me.'");
#endif

			switch (randint(4))
			{
				case 1:
					(void)activate_ty_curse(FALSE, &count);
#ifdef JP
					reward = "�ҡ����������򤫤���줿��";
#else
					reward = "cursing";
#endif
					break;
				case 2:
					activate_hi_summon(py, px, FALSE);
#ifdef JP
					reward = "��󥹥����򾤴����줿��";
#else
					reward = "summoning hostile monsters";
#endif
					break;
				case 3:
					if (randint(2) == 1)
					{
						if (!buki_motteruka(INVEN_RARM)) break;
						object_desc(o_name, &inventory[INVEN_RARM], TRUE, 0);
						(void)curse_weapon(FALSE, INVEN_RARM);
						reward = format("%s���˲����줿��", o_name);
					}
					else
					{
						if (!inventory[INVEN_BODY].k_idx) break;
						object_desc(o_name, &inventory[INVEN_BODY], TRUE, 0);
						(void)curse_armor();
						reward = format("%s���˲����줿��", o_name);
					}
					break;
				default:
					for (dummy = 0; dummy < 6; dummy++)
					{
						(void)dec_stat(dummy, 10 + randint(15), TRUE);
					}
#ifdef JP
					reward = "��ǽ���ͤ������ä���";
#else
					reward = "decreasing all stats";
#endif
					break;
			}
			break;
		case REW_WRATH:
#ifdef JP
msg_format("%s�������줭�Ϥä�:",
#else
			msg_format("The voice of %s thunders:",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
#ifdef JP
msg_print("�ֻ�̤��褤�����ͤ衪��");
#else
			msg_print("'Die, mortal!'");
#endif

			take_hit(DAMAGE_LOSELIFE, p_ptr->lev * 4, wrath_reason, -1);
			for (dummy = 0; dummy < 6; dummy++)
			{
				(void)dec_stat(dummy, 10 + randint(15), FALSE);
			}
			activate_hi_summon(py, px, FALSE);
			(void)activate_ty_curse(FALSE, &count);
			if (randint(2) == 1) (void)curse_weapon(FALSE, INVEN_RARM);
			if (randint(2) == 1) (void)curse_armor();
			break;
		case REW_DESTRUCT:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֻ���˲������椬��Ӥʤꡪ��");
#else
			msg_print("'Death and destruction! This pleaseth me!'");
#endif

			destroy_area(py, px, 25, TRUE);
#ifdef JP
			reward = "���󥸥��*�˲�*���줿��";
#else
			reward = "*destruct*ing dungeon";
#endif
			break;
		case REW_GENOCIDE:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ桢���Ũ���������󡪡�");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)symbol_genocide(0, FALSE);
#ifdef JP
			reward = "��󥹥������������줿��";
#else
			reward = "genociding monsters";
#endif
			break;
		case REW_MASS_GEN:
#ifdef JP
msg_format("%s�����������Ϥä�:",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("The voice of %s booms out:",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

#ifdef JP
msg_print("�ֲ桢���Ũ���������󡪡�");
#else
			msg_print("'Let me relieve thee of thine oppressors!'");
#endif

			(void)mass_genocide(0, FALSE);
#ifdef JP
			reward = "��󥹥������������줿��";
#else
			reward = "genociding nearby monsters";
#endif
			break;
		case REW_DISPEL_C:
#ifdef JP
msg_format("%s���Ϥ�Ũ�򹶷⤹��Τ򴶤�����",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("You can feel the power of %s assault your enemies!",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			(void)dispel_monsters(p_ptr->lev * 4);
			break;
		case REW_IGNORE:
#ifdef JP
msg_format("%s�Ϥ��ʤ���̵�뤷����",
				chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s ignores you.",
				chaos_patrons[p_ptr->chaos_patron]);
#endif

			break;
		case REW_SER_DEMO:
#ifdef JP
msg_format("%s��˫���Ȥ��ư���λȤ���褳������",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with a demonic servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, SUMMON_DEMON, FALSE, TRUE, TRUE, FALSE, FALSE))
#ifdef JP
msg_print("���⸽��ʤ��ä�...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "���⤬�ڥåȤˤʤä���";
#else
				reward = "a demonic servant";
#endif

			break;
		case REW_SER_MONS:
#ifdef JP
msg_format("%s��˫���Ȥ��ƻȤ���褳������",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with a servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, 0, FALSE, TRUE, TRUE, FALSE, FALSE))
#ifdef JP
msg_print("���⸽��ʤ��ä�...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "��󥹥������ڥåȤˤʤä���";
#else
				reward = "a servant";
#endif

			break;
		case REW_SER_UNDE:
#ifdef JP
msg_format("%s��˫���Ȥ��ƥ���ǥåɤλȤ���褳������",chaos_patrons[p_ptr->chaos_patron]);
#else
			msg_format("%s rewards you with an undead servant!",chaos_patrons[p_ptr->chaos_patron]);
#endif

			if (!summon_specific(-1, py, px, dun_level, SUMMON_UNDEAD, FALSE, TRUE, TRUE, FALSE, FALSE))
#ifdef JP
msg_print("���⸽��ʤ��ä�...");
#else
				msg_print("Nobody ever turns up...");
#endif
			else
#ifdef JP
				reward = "����ǥåɤ��ڥåȤˤʤä���";
#else
				reward = "an undead servant";
#endif

			break;
		default:
#ifdef JP
msg_format("%s�������ɤ�ä�:",
#else
			msg_format("The voice of %s stammers:",
#endif

				chaos_patrons[p_ptr->chaos_patron]);
#ifdef JP
msg_format("�֤����������������� %d/%d������ϲ�����", type, effect);
#else
			msg_format("'Uh... uh... the answer's %d/%d, what's the question?'", type, effect);
#endif

	}
	}
	if (reward)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("�ѥȥ����󽷤�%s", reward));
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, format("The patron rewards you with %s.", reward));
#endif
	}
}


/*
 * old -- from PsiAngband.
 */
bool tgt_pt(int *x_ptr, int *y_ptr)
{
	char ch = 0;
	int d, x, y;
	bool success = FALSE;

	x = px;
	y = py;

#ifdef JP
msg_print("��������ǥ��ڡ��������򲡤��Ʋ�������");
#else
	msg_print("Select a point and press space.");
#endif
	msg_flag = FALSE; /* prevents "-more-" message. */

	while ((ch != ESCAPE) && !success)
	{
		bool move_fast = FALSE;

		move_cursor_relative(y, x);
		ch = inkey();
		switch (ch)
		{
		case ESCAPE:
			break;
		case ' ':
		case 't':
		case '.':
		case '5':
		case '0':
			/* illegal place */
			if (x == px && y == py) ch = 0;
			
			/* okay place */
			else success = TRUE;

			break;
		default:
			/* Look up the direction */
			d = get_keymap_dir(ch);

			/* XTRA HACK MOVEFAST */
			if (isupper(ch)) move_fast = TRUE;

			/* Handle "direction" */
			if (d)
			{
				int dx = ddx[d];
				int dy = ddy[d];

                                /* XTRA HACK MOVEFAST */
                                if (move_fast)
                                {
                                     x += dx * SCREEN_WID / 2;
                                     y += dy * SCREEN_HGT / 2;
                                } else {
                                x += dx;
                                y += dy;
                                }

				/* Do not move horizontally if unnecessary */
				if (((x < panel_col_min + SCREEN_WID / 2) && (dx > 0)) ||
					 ((x > panel_col_min + SCREEN_WID / 2) && (dx < 0)))
				{
					dx = 0;
				}

				/* Do not move vertically if unnecessary */
				if (((y < panel_row_min + SCREEN_HGT / 2) && (dy > 0)) ||
					 ((y > panel_row_min + SCREEN_HGT / 2) && (dy < 0)))
				{
					dy = 0;
				}

				/* Apply the motion */
				if ((y >= panel_row_min + SCREEN_HGT) || (y < panel_row_min) ||
					 (x >= panel_col_min + SCREEN_WID) || (x < panel_col_min))
				{
					/* if (change_panel(dy, dx)) target_set_prepare(mode); */
					change_panel(dy, dx);
				}

				/* Slide into legality */
				if (x >= cur_wid-1) x = cur_wid - 2;
				else if (x <= 0) x = 1;

				/* Slide into legality */
				if (y >= cur_hgt-1) y = cur_hgt- 2;
				else if (y <= 0) y = 1;

			}
			break;
		}
	}

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	*x_ptr = x;
	*y_ptr = y;
	return success;
}


bool get_hack_dir(int *dp)
{
	int		dir;
	cptr    p;
	char    command;


	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = 0;

	/* (No auto-targeting) */

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
#ifdef JP
p = "���� ('*'�ǥ������å�����, ESC������)? ";
#else
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
#endif

		}
		else
		{
#ifdef JP
p = "���� ('5'�ǥ������åȤ�, '*'�ǥ������åȺ�����, ESC������)? ";
#else
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
#endif

		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command, TRUE)) break;

		if (use_menu)
		{
			if (command == '\r') command = 't';
		}  

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			case ' ':
			case '\r':
			{
				if (target_set(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Look up the direction */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell();
	}

	/* No direction */
	if (!dir) return (FALSE);

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[rand_int(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
#ifdef JP
msg_print("���ʤ��Ϻ��𤷤Ƥ��롣");
#else
		msg_print("You are confused.");
#endif

	}

	/* Save direction */
	(*dp) = dir;

	/* A "valid" direction was entered */
	return (TRUE);
}
