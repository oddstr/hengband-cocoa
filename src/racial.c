/* File: racial.c */

/* Purpose: Racial powers (and mutations) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

/*
 * Hook to determine if an object is contertible in an arrow/bolt
 */
static bool item_tester_hook_convertible(object_type *o_ptr)
{
	if((o_ptr->tval==TV_JUNK) || (o_ptr->tval==TV_SKELETON)) return TRUE;

	if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON)) return TRUE;
	/* Assume not */
	return (FALSE);
}


/*
 * do_cmd_cast calls this function if the player's class
 * is 'archer'.
 */
static bool do_cmd_archer(void)
{
	int ext=0;
	char ch;

	object_type	forge;
	object_type     *q_ptr;

	char com[80];
	char o_name[MAX_NLEN];

	q_ptr = &forge;

	if(p_ptr->lev >= 20)
#ifdef JP
		sprintf(com, "[S]��, [A]��, [B]�����ܥ����� :");
#else
		sprintf(com, "Create [S]hots, Create [A]rrow or Create [B]olt ?");
#endif
	else if(p_ptr->lev >= 10)
#ifdef JP
		sprintf(com, "[S]��, [A]��:");
#else
		sprintf(com, "Create [S]hots or Create [A]rrow ?");
#endif
	else
#ifdef JP
		sprintf(com, "[S]��:");
#else
		sprintf(com, "Create [S]hots ?");
#endif

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("���𤷤Ƥ롪");
#else
		msg_print("You are too confused!");
#endif
		return FALSE;
	}

	if (p_ptr->blind)
	{
#ifdef JP
		msg_print("�ܤ������ʤ���");
#else
		msg_print("You are blind!");
#endif
		return FALSE;
	}

	while (TRUE)
	{
		if (!get_com(com, &ch, TRUE))
		{
			return FALSE;
		}
		if (ch == 'S' || ch == 's')
		{
			ext = 1;
			break;
		}
		if ((ch == 'A' || ch == 'a')&&(p_ptr->lev >= 10))
		{
			ext = 2;
			break;
		}
		if ((ch == 'B' || ch == 'b')&&(p_ptr->lev >= 20))
		{
			ext = 3;
			break;
		}
	}

	/**********Create shots*********/
	if (ext == 1)
	{
		int x,y, dir;
		cave_type *c_ptr;

		if (!get_rep_dir(&dir, FALSE)) return FALSE;
		y = py + ddy[dir];
		x = px + ddx[dir];
		c_ptr = &cave[y][x];
		if (c_ptr->feat == FEAT_RUBBLE)
		{
			/* Get local object */
			q_ptr = &forge;

			/* Hack -- Give the player some small firestones */
			object_prep(q_ptr, lookup_kind(TV_SHOT, m_bonus(1, p_ptr->lev) + 1));
			q_ptr->number = (byte)rand_range(15,30);
			object_aware(q_ptr);
			object_known(q_ptr);
			apply_magic(q_ptr, p_ptr->lev, FALSE, FALSE, FALSE, FALSE);
			q_ptr->discount = 99;

			(void)inven_carry(q_ptr);

			object_desc(o_name, q_ptr, TRUE, 2);
#ifdef JP
			msg_format("���Ф��ä�%s���ä���",o_name);
#else
			msg_print("You make some ammo.");
#endif

			(void)wall_to_mud(dir);
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW);
			p_ptr->window |= (PW_OVERHEAD);
		}
		else
		{
#ifdef JP
			msg_print("�����ˤϴ��Ф��ʤ���");
#else
			msg_print("You need pile of rubble.");
#endif
		}
	}
	/**********Create arrows*********/
	else if (ext == 2)
	{
		int item;

		cptr q, s;

		item_tester_hook = item_tester_hook_convertible;

		/* Get an item */
#ifdef JP
		q = "�ɤΥ����ƥफ����ޤ����� ";
		s = "��������äƤ��ʤ���";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			q_ptr = &inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			q_ptr = &o_list[0 - item];
		}       

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(q_ptr, lookup_kind(TV_ARROW, m_bonus(1, p_ptr->lev)+ 1));
		q_ptr->number = (byte)rand_range(5,10);
		object_aware(q_ptr);
		object_known(q_ptr);
		apply_magic(q_ptr, p_ptr->lev, FALSE, FALSE, FALSE, FALSE);

		q_ptr->discount = 99;

		object_desc(o_name, q_ptr, TRUE, 2);
#ifdef JP
		msg_format("%s���ä���", o_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}
		(void)inven_carry(q_ptr);
	}
	/**********Create bolts*********/
	else if (ext == 3)
	{
		int item;

		cptr q, s;

		item_tester_hook = item_tester_hook_convertible;

		/* Get an item */
#ifdef JP
		q = "�ɤΥ����ƥफ����ޤ����� ";
		s = "��������äƤ��ʤ���";
#else
		q = "Convert which item? ";
		s = "You have no item to convert.";
#endif
		if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return FALSE;

		/* Get the item (in the pack) */
		if (item >= 0)
		{
			q_ptr = &inventory[item];
		}

		/* Get the item (on the floor) */
		else
		{
			q_ptr = &o_list[0 - item];
		}       

		/* Get local object */
		q_ptr = &forge;

		/* Hack -- Give the player some small firestones */
		object_prep(q_ptr, lookup_kind(TV_BOLT, m_bonus(1, p_ptr->lev)+1));
		q_ptr->number = (byte)rand_range(4,8);
		object_aware(q_ptr);
		object_known(q_ptr);
		apply_magic(q_ptr, p_ptr->lev, FALSE, FALSE, FALSE, FALSE);

		q_ptr->discount = 99;

		object_desc(o_name, q_ptr, TRUE, 2);
#ifdef JP
		msg_format("%s���ä���", o_name);
#else
		msg_print("You make some ammo.");
#endif

		if (item >= 0)
		{
			inven_item_increase(item, -1);
			inven_item_describe(item);
			inven_item_optimize(item);
		}
		else
		{
			floor_item_increase(0 - item, -1);
			floor_item_describe(0 - item);
			floor_item_optimize(0 - item);
		}

		(void)inven_carry(q_ptr);
	}
	return TRUE;
}

bool gain_magic(void)
{
	int item;
	int pval;
	int ext = 0;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];

	/* Only accept legal items */
	item_tester_hook = item_tester_hook_recharge;

	/* Get an item */
#ifdef JP
q = "�ɤΥ����ƥ�����Ϥ�����ߤޤ���? ";
s = "���Ϥ������륢���ƥब�ʤ���";
#else
	q = "Gain power of which item? ";
	s = "You have nothing to gain power.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return (FALSE);

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

	if (o_ptr->tval == TV_STAFF && o_ptr->sval == SV_STAFF_NOTHING)
	{
#ifdef JP
		msg_print("���ξ�ˤ�ȯư�ΰ٤�ǽ�Ϥϲ�������äƤ��ʤ��褦����");
#else
		msg_print("This staff doesn't have any magical ability.");
#endif
		return FALSE;
	}


	if (!object_known_p(o_ptr))
	{
#ifdef JP
		msg_print("���ꤵ��Ƥ��ʤ��ȼ�����ʤ���");
#else
		msg_print("You need to identify before absorbing.");
#endif
		return FALSE;
	}

	if (o_ptr->timeout)
	{
#ifdef JP
		msg_print("��Ŷ��Υ����ƥ�ϼ�����ʤ���");
#else
		msg_print("This item is still charging.");
#endif
		return FALSE;
	}

	pval = o_ptr->pval;
	if (o_ptr->tval == TV_ROD)
		ext = 72;
	else if (o_ptr->tval == TV_WAND)
		ext = 36;

	if (o_ptr->tval == TV_ROD)
	{
		p_ptr->magic_num2[o_ptr->sval + ext] += o_ptr->number;
		if (p_ptr->magic_num2[o_ptr->sval + ext] > 99) p_ptr->magic_num2[o_ptr->sval + ext] = 99;
	}
	else
	{
		int num;
		for (num = o_ptr->number; num; num--)
		{
			int gain_num = pval;
			if (o_ptr->tval == TV_WAND) gain_num = (pval + num - 1) / num;
			if (p_ptr->magic_num2[o_ptr->sval + ext])
			{
				gain_num *= 256;
				gain_num = (gain_num/3 + randint0(gain_num/3)) / 256;
				if (gain_num < 1) gain_num = 1;
			}
			p_ptr->magic_num2[o_ptr->sval + ext] += gain_num;
			if (p_ptr->magic_num2[o_ptr->sval + ext] > 99) p_ptr->magic_num2[o_ptr->sval + ext] = 99;
			p_ptr->magic_num1[o_ptr->sval + ext] += pval * 0x10000;
			if (p_ptr->magic_num1[o_ptr->sval + ext] > 99 * 0x10000) p_ptr->magic_num1[o_ptr->sval + ext] = 99 * 0x10000;
			if (p_ptr->magic_num1[o_ptr->sval + ext] > p_ptr->magic_num2[o_ptr->sval + ext] * 0x10000) p_ptr->magic_num1[o_ptr->sval + ext] = p_ptr->magic_num2[o_ptr->sval + ext] * 0x10000;
			if (o_ptr->tval == TV_WAND) pval -= (pval + num - 1) / num;
		}
	}

	object_desc(o_name, o_ptr, TRUE, 3);
	/* Message */
#ifdef JP
	msg_format("%s�����Ϥ���������", o_name);
#else
	msg_format("You absorb magic of %s.", o_name);
#endif

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -999);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -999);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}
	energy_use = 100;
	return TRUE;
}


static bool choose_kamae(void)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("���𤷤Ƥ��ƹ������ʤ���");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) ������Ȥ�", 2, 20);
#else
	prt(" a) No form", 2, 20);
#endif

	for (i = 0; i < MAX_KAMAE; i++)
	{
		if (p_ptr->lev >= kamae_shurui[i].min_level)
		{
			sprintf(buf," %c) %-12s  %s",I2A(i+1), kamae_shurui[i].desc, kamae_shurui[i].info);
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        �ɤι�����Ȥ�ޤ�����", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A') || (choice == ESCAPE))
		{
			if (p_ptr->action == ACTION_KAMAE)
			{
				set_action(ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("��Ȥ�ȹ����Ƥ��ʤ���");
#else
				msg_print("You are not assuming a posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kamae = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
		{
			new_kamae = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
		{
			new_kamae = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
		{
			new_kamae = 3;
			break;
		}
	}
	set_action(ACTION_KAMAE);

	if (p_ptr->special_defense & (KAMAE_GENBU << new_kamae))
	{
#ifdef JP
		msg_print("����ľ������");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		p_ptr->special_defense &= ~(KAMAE_MASK);
		p_ptr->update |= (PU_BONUS);
		p_ptr->redraw |= (PR_STATE);
#ifdef JP
		msg_format("%s�ι�����Ȥä���",kamae_shurui[new_kamae].desc);
#else
		msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
#endif
		p_ptr->special_defense |= (KAMAE_GENBU << new_kamae);
	}
	p_ptr->redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

static bool choose_kata(void)
{
	char choice;
	int new_kata = 0;
	int i;
	char buf[80];

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("���𤷤Ƥ��ƹ������ʤ���");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	if (p_ptr->stun)
	{
#ifdef JP
		msg_print("�ռ����Ϥä���Ȥ��ʤ���");
#else
		msg_print("You are not clear headed");
#endif
		return FALSE;
	}

	if (p_ptr->afraid)
	{
#ifdef JP
		msg_print("�Τ��̤��ƹ������ʤ���");
#else
		msg_print("You are trembling with fear!");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) ��������", 2, 20);
#else
	prt(" a) No Form", 2, 20);
#endif

	for (i = 0; i < MAX_KATA; i++)
	{
		if (p_ptr->lev >= kata_shurui[i].min_level)
		{
#ifdef JP
			sprintf(buf," %c) %s�η�    %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#else
			sprintf(buf," %c) Form of %-12s  %s",I2A(i+1), kata_shurui[i].desc, kata_shurui[i].info);
#endif
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        �ɤη��ǹ����ޤ�����", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A') || (choice == ESCAPE))
		{
			if (p_ptr->action == ACTION_KATA)
			{
				set_action(ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("��Ȥ�ȹ����Ƥ��ʤ���");
#else
				msg_print("You are not assuming posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kata = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
		{
			new_kata = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
		{
			new_kata = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
		{
			new_kata = 3;
			break;
		}
	}
	set_action(ACTION_KATA);

	if (p_ptr->special_defense & (KATA_IAI << new_kata))
	{
#ifdef JP
		msg_print("����ľ������");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		p_ptr->special_defense &= ~(KATA_MASK);
		p_ptr->update |= (PU_BONUS);
		p_ptr->update |= (PU_MONSTERS);
#ifdef JP
		msg_format("%s�η��ǹ�������",kata_shurui[new_kata].desc);
#else
		msg_format("You assume a posture of %s form.",kata_shurui[new_kata].desc);
#endif
		p_ptr->special_defense |= (KATA_IAI << new_kata);
	}
	p_ptr->redraw |= (PR_STATE);
	p_ptr->redraw |= (PR_STATUS);
	screen_load();
	return TRUE;
}


typedef struct power_desc_type power_desc_type;

struct power_desc_type
{
	char name[40];
	int  level;
	int  cost;
	int  stat;
	int  fail;
	int  number;
};


/*
 * Returns the chance to activate a racial power/mutation
 */
static int racial_chance(power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  difficulty = pd_ptr->fail;

	int i;
	int val;
	int sum = 0;
	int stat = p_ptr->stat_cur[pd_ptr->stat];

	/* No chance for success */
	if ((p_ptr->lev < min_level) || p_ptr->confused)
	{
		return (0);
	}

	if (difficulty == 0) return 100;

	/* Calculate difficulty */
	if (p_ptr->stun)
	{
		difficulty += p_ptr->stun;
	}
	else if (p_ptr->lev > min_level)
	{
		int lev_adj = ((p_ptr->lev - min_level) / 3);
		if (lev_adj > 10) lev_adj = 10;
		difficulty -= lev_adj;
	}

	if (difficulty < 5) difficulty = 5;

	/* We only need halfs of the difficulty */
	difficulty = difficulty / 2;

	for (i = 1; i <= stat; i++)
	{
		val = i - difficulty;
		if (val > 0)
			sum += (val <= difficulty) ? val : difficulty;
	}

	if (difficulty == 0)
		return (100);
	else
		return (((sum * 100) / difficulty) / stat);
}


static int  racial_cost;
static bool racial_use_hp;

/*
 * Note: return value indicates that we have succesfully used the power
 * 1: Succeeded, 0: Cancelled, -1: Failed
 */
static int racial_aux(power_desc_type *pd_ptr)
{
	s16b min_level  = pd_ptr->level;
	int  use_stat   = pd_ptr->stat;
	int  difficulty = pd_ptr->fail;

	racial_cost   = pd_ptr->cost;
	racial_use_hp = FALSE;

	/* Not enough mana - use hp */
	if (p_ptr->csp < racial_cost) racial_use_hp = TRUE;

	/* Power is not available yet */
	if (p_ptr->lev < min_level)
	{
#ifdef JP
		msg_format("����ǽ�Ϥ���Ѥ���ˤϥ�٥� %d ��ã���Ƥ��ʤ���Фʤ�ޤ���", min_level);
#else
		msg_format("You need to attain level %d to use this power.", min_level);
#endif

		energy_use = 0;
		return 0;
	}

	/* Too confused */
	else if (p_ptr->confused)
	{
#ifdef JP
		msg_print("���𤷤Ƥ��Ƥ���ǽ�ϤϻȤ��ʤ���");
#else
		msg_print("You are too confused to use this power.");
#endif

		energy_use = 0;
		return 0;
	}

	/* Risk death? */
	else if (racial_use_hp && (p_ptr->chp < racial_cost))
	{
#ifdef JP
		if (!get_check("�����˺��ο�夷�����֤Ǥ���ǽ�Ϥ�Ȥ��ޤ�����"))
#else
		if (!get_check("Really use the power in your weakened state? "))
#endif
		{
			energy_use = 0;
			return 0;
		}
	}

	/* Else attempt to do it! */

	if (difficulty)
	{
		if (p_ptr->stun)
		{
			difficulty += p_ptr->stun;
		}
		else if (p_ptr->lev > min_level)
		{
			int lev_adj = ((p_ptr->lev - min_level) / 3);
			if (lev_adj > 10) lev_adj = 10;
			difficulty -= lev_adj;
		}

		if (difficulty < 5) difficulty = 5;
	}

	/* take time and pay the price */
	energy_use = 100;

	/* Success? */
	if (randint1(p_ptr->stat_cur[use_stat]) >=
	    ((difficulty / 2) + randint1(difficulty / 2)))
	{
		return 1;
	}

#ifdef JP
	msg_print("��ʬ�˽���Ǥ��ʤ��ä���");
#else
	msg_print("You've failed to concentrate hard enough.");
#endif
	if (flush_failure) flush();

	return -1;
}


static bool cmd_racial_power_aux(s32b command)
{
	s16b        plev = p_ptr->lev;
	int         dir = 0;

	if (command <= -3)
	{
		switch (p_ptr->pclass)
		{
		case CLASS_WARRIOR:
		{
			int y = 0, x = 0, i;
			cave_type       *c_ptr;
			monster_type    *m_ptr;

			for (i = 0; i < 6; i++)
			{
				dir = randint0(8);
				y = py + ddy_ddd[dir];
				x = px + ddx_ddd[dir];
				c_ptr = &cave[y][x];

				/* Get the monster */
				m_ptr = &m_list[c_ptr->m_idx];

				/* Hack -- attack monsters */
				if (c_ptr->m_idx)
					py_attack(y, x, 0);
				else
				{
#ifdef JP
					msg_print("���⤬���򤭤ä���");
#else
					msg_print("You attack the empty air.");
#endif
				}
			}
			break;
		}
		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
		{
			if (!eat_magic(p_ptr->lev * 2)) return FALSE;
			break;
		}
		case CLASS_PRIEST:
		{
			if (is_good_realm(p_ptr->realm1))
			{
				if (!bless_weapon()) return FALSE;
			}
			else
			{
				(void)dispel_monsters(plev * 4);
				turn_monsters(plev * 4);
				banish_monsters(plev * 4);
			}
			break;
		}
		case CLASS_ROGUE:
		{
			int x, y;

			if (!get_rep_dir(&dir, FALSE)) return FALSE;
			y = py + ddy[dir];
			x = px + ddx[dir];
			if (cave[y][x].m_idx)
			{
				py_attack(y, x, 0);
				if (randint0(p_ptr->skill_dis) < 7)
#ifdef JP
					msg_print("���ޤ�ƨ�����ʤ��ä���");
#else
					msg_print("You are failed to run away.");
#endif
				else teleport_player(30);
			}
			else
			{
#ifdef JP
				msg_print("���������ˤϥ�󥹥����Ϥ��ޤ���");
#else
				msg_print("You don't see any monster in this direction");
#endif

				msg_print(NULL);
			}
			break;
		}
		case CLASS_RANGER:
		{
#ifdef JP
			msg_print("Ũ��Ĵ������...");
#else
			msg_print("You examine your foes...");
#endif

			probing();
			break;
		}
		case CLASS_PALADIN:
		{
			if (is_good_realm(p_ptr->realm1))
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_beam(GF_HOLY_FIRE, dir, plev * 3);
			}
			else
			{
				if (!get_aim_dir(&dir)) return FALSE;
				fire_beam(GF_HELL_FIRE, dir, plev * 3);
			}
			break;
		}
		case CLASS_WARRIOR_MAGE:
		{
			if (command == -3)
			{
				int gain_sp;
#ifdef JP
				if ((gain_sp = take_hit(DAMAGE_USELIFE, p_ptr->lev, "�ȣФ���ͣФؤ�̵�Ť��Ѵ�", -1)))
#else
				if ((gain_sp = take_hit(DAMAGE_USELIFE, p_ptr->lev, "thoughtless convertion from HP to SP", -1)))
#endif
				{
					p_ptr->csp += gain_sp / 5;
					if (p_ptr->csp > p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
				}
				else
#ifdef JP
					msg_print("�Ѵ��˼��Ԥ�����");
#else
					msg_print("You failed to convert.");
#endif
			}
			else if (command == -4)
			{
				if (p_ptr->csp >= p_ptr->lev/5)
				{
					p_ptr->csp -= p_ptr->lev / 5;
					hp_player(p_ptr->lev);
				}
				else
#ifdef JP
					msg_print("�Ѵ��˼��Ԥ�����");
#else
					msg_print("You failed to convert.");
#endif
			}
			break;
		}
		case CLASS_CHAOS_WARRIOR:
		{
#ifdef JP
			msg_print("�դ���ˤ��...");
#else
			msg_print("You glare nearby monsters...");
#endif
			slow_monsters();
			stun_monsters(p_ptr->lev * 4);
			confuse_monsters(p_ptr->lev * 4);
			turn_monsters(p_ptr->lev * 4);
			stasis_monsters(p_ptr->lev * 4);
			break;
		}
		case CLASS_MONK:
		{
			if (empty_hands(TRUE) < 2)
			{
#ifdef JP
				msg_print("�Ǽꤸ��ʤ��ȤǤ��ޤ���");
#else
				msg_print("You need to be bare hand.");
#endif
				return FALSE;
			}

			if (command == -3)
			{
				if (choose_kamae()) energy_use = 100;
				else energy_use = 0;
				p_ptr->update |= (PU_BONUS);
				p_ptr->redraw |= (PR_ARMOR);
			}
			else if (command == -4)
			{
				int x, y;

				if (!get_rep_dir(&dir, FALSE)) return FALSE;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if (cave[y][x].m_idx)
				{
#ifdef JP
					if (one_in_(2)) msg_print("������������������������������������������������������");
					else msg_print("���饪�饪�饪�饪�饪�饪�饪�饪�饪�饪�饪�顪����");
#else
					if (one_in_(2)) msg_print("Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!");
					else msg_print("Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!");
#endif

					py_attack(y, x, 0);
					if (cave[y][x].m_idx)
					{
						handle_stuff();
						py_attack(y, x, 0);
					}
					p_ptr->energy_need += ENERGY_NEED();
				}
				else
				{
#ifdef JP
					msg_print("���������ˤϥ�󥹥����Ϥ��ޤ���");
#else
					msg_print("You don't see any monster in this direction");
#endif

					msg_print(NULL);
				}
			}
			break;
		}
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
		{
			if (total_friends)
			{
#ifdef JP
				msg_print("���ϥڥåȤ���뤳�Ȥ˽��椷�Ƥ��ʤ��ȡ�");
#else
				msg_print("You need concentration on the pets now.");
#endif
				return FALSE;
			}
#ifdef JP
			msg_print("����Ƭ���ϥå��ꤷ����");
#else
			msg_print("You feel your head clear a little.");
#endif

			p_ptr->csp += (3 + p_ptr->lev/20);
			if (p_ptr->csp >= p_ptr->msp)
			{
				p_ptr->csp = p_ptr->msp;
				p_ptr->csp_frac = 0;
			}
			break;
		}
		case CLASS_TOURIST:
		{
			if (command == -3)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				project_length = 1;
				fire_beam(GF_PHOTO, dir, 1);
			}
			else if (command == -4)
			{
				if (!identify_fully(FALSE)) return FALSE;
			}
			break;
		}
		case CLASS_IMITATOR:
		{
			handle_stuff();
			if (!do_cmd_mane(TRUE)) return FALSE;
			break;
		}
		case CLASS_BEASTMASTER:
		{
			if (command == -3)
			{
				if (!get_aim_dir(&dir)) return FALSE;
				(void)fire_ball_hide(GF_CONTROL_LIVING, dir, p_ptr->lev, 0);
				break;
			}
			else if (command == -4)
			{
				project_hack(GF_CONTROL_LIVING, p_ptr->lev);
				break;
			}
		}
		case CLASS_ARCHER:
		{
			if (!do_cmd_archer()) return FALSE;
			break;
		}
		case CLASS_MAGIC_EATER:
		{
			if (!gain_magic()) return FALSE;
			break;
		}
		case CLASS_BARD:
		{
			stop_singing();
			energy_use = 10;
			return FALSE;
		}
		case CLASS_RED_MAGE:
		{
			handle_stuff();
			do_cmd_cast();
			handle_stuff();
			if (!p_ptr->paralyzed)
				do_cmd_cast();
			break;
		}
		case CLASS_SAMURAI:
		{
			if (command == -3)
			{
				int max_csp = MAX(p_ptr->msp*4, p_ptr->lev*5+5);

				if (total_friends)
				{
#ifdef JP
					msg_print("���ϥڥåȤ���뤳�Ȥ˽��椷�Ƥ��ʤ��ȡ�");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (p_ptr->special_defense & KATA_MASK)
				{
#ifdef JP
					msg_print("���Ϲ����˽��椷�Ƥ��롣");
#else
					msg_print("You need concentration on your form.");
#endif
					return FALSE;
				}
#ifdef JP
				msg_print("�������椷�Ƶ��礤��ί�᤿��");
#else
				msg_print("You concentrate to charge your power.");
#endif

				p_ptr->csp += p_ptr->msp / 2;
				if (p_ptr->csp >= max_csp)
				{
					p_ptr->csp = max_csp;
					p_ptr->csp_frac = 0;
				}
			}
			else if (command == -4)
			{
				if (!buki_motteruka(INVEN_RARM))
				{
#ifdef JP
					msg_print("��������ʤ��Ȥ����ޤ���");
#else
					msg_print("You need to wield a weapon.");
#endif
					return FALSE;
				}
				if (choose_kata()) energy_use = 100;
				else energy_use = 0;
				p_ptr->update |= (PU_BONUS);
				p_ptr->redraw |= (PR_ARMOR);
			}
			break;
		}
		case CLASS_BLUE_MAGE:
		{
			if (p_ptr->action == ACTION_LEARN)
			{
				set_action(ACTION_NONE);
			}
			else
			{
				set_action(ACTION_LEARN);
			}
			energy_use = 0;
			break;
		}
		case CLASS_CAVALRY:
		{
			char m_name[80];
			monster_type *m_ptr;
			monster_race *r_ptr;
			int rlev;

			if (p_ptr->riding)
			{
#ifdef JP
				msg_print("���Ͼ��������");
#else
				msg_print("You ARE riding.");
#endif
				return FALSE;
			}
			if (!do_riding(TRUE)) return TRUE;
			m_ptr = &m_list[p_ptr->riding];
			r_ptr = &r_info[m_ptr->r_idx];
			monster_desc(m_name, m_ptr, 0);
#ifdef JP
			msg_format("%s�˾�ä���",m_name);
#else
			msg_format("You ride on %s.",m_name);
#endif
			if (is_pet(m_ptr)) break;
			rlev = r_ptr->level;
			if (r_ptr->flags1 & RF1_UNIQUE) rlev = rlev * 3 / 2;
			if (rlev > 60) rlev = 60+(rlev-60)/2;
			if ((randint1(p_ptr->skill_exp[GINOU_RIDING]/120+p_ptr->lev*2/3) > rlev) && one_in_(2) && !p_ptr->inside_arena && !p_ptr->inside_battle && !(r_ptr->flags7 & (RF7_GUARDIAN)) && !(r_ptr->flags1 & (RF1_QUESTOR)) && (rlev < p_ptr->lev*3/2+randint0(p_ptr->lev/5)))
			{
#ifdef JP
				msg_format("%s���ʤ�������",m_name);
#else
				msg_format("You tame %s.",m_name);
#endif
				set_pet(m_ptr);
			}
			else
			{
#ifdef JP
				msg_format("%s�˿�����Ȥ��줿��",m_name);
#else
				msg_format("You have thrown off by %s.",m_name);
#endif
				rakuba(1,TRUE);

				/* Paranoia */
				/* ���Ͻ����˼��Ԥ��Ƥ�Ȥˤ������ϲ�� */
				p_ptr->riding = 0;
			}
			break;
		}
		case CLASS_BERSERKER:
		{
			if (!word_of_recall()) return FALSE;
			break;
		}
		case CLASS_SMITH:
		{
			if (p_ptr->lev > 29)
			{
				if (!identify_fully(TRUE)) return FALSE;
			}
			else
			{
				if (!ident_spell(TRUE)) return FALSE;
			}
			break;
		}
		case CLASS_MIRROR_MASTER:
		{
			if (command == -3)
			{
				int x, y;
				for (x = 0; x < cur_wid; x++)
				{
					for (y = 0; y < cur_hgt; y++)
					{
						if (is_mirror_grid(&cave[y][x]))
						{
							remove_mirror(y, x);
							project(0, 2, y, x, p_ptr->lev / 2 + 5, GF_SHARDS,
								(PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP | PROJECT_NO_HANGEKI), -1);
						}
					}
				}
			}
			else if (command == -4)
			{
				if (total_friends)
				{
#ifdef JP
					msg_print("���ϥڥåȤ���뤳�Ȥ˽��椷�Ƥ��ʤ��ȡ�");
#else
					msg_print("You need concentration on the pets now.");
#endif
					return FALSE;
				}
				if (is_mirror_grid(&cave[py][px]))
				{
#ifdef JP
					msg_print("����Ƭ���ϥå��ꤷ����");
#else
					msg_print("You feel your head clear a little.");
#endif

					p_ptr->csp += (5 + p_ptr->lev * p_ptr->lev / 100);
					if (p_ptr->csp >= p_ptr->msp)
					{
						p_ptr->csp = p_ptr->msp;
						p_ptr->csp_frac = 0;
					}
				}
				else
				{
#ifdef JP
					msg_print("���ξ�Ǥʤ��Ƚ���Ǥ��ʤ���");
#else
					msg_print("Here are not any mirrors!");
#endif
				}
			}
			break;
		}
		case CLASS_NINJA:
		{
			if (p_ptr->action == ACTION_HAYAGAKE) set_action(ACTION_NONE);
			else set_action(ACTION_HAYAGAKE);
			energy_use = 0;
			break;
		}
		}
	}
	else if (p_ptr->mimic_form)
	{
		switch (p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		{
			int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_format("���ʤ���%s�Υ֥쥹���Ǥ�����",((type == GF_NETHER) ? "�Ϲ�" : "�б�"));
#else
			msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

			fire_ball(type, dir, plev * 3, -(plev / 15) - 1);
			break;
		}
		case MIMIC_VAMPIRE:
			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�ʤ������⤹�뤳�Ȥ��Ǥ��ʤ���");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir(&dir, FALSE)) return FALSE;   /* was get_aim_dir */
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("����ʤ����˳��ߤĤ�����");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���ʤ��ϥ˥��Ȥ��Ʋ��त��...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   /* Dmg */
				if (drain_life(dir, dummy))
				{
					if (p_ptr->food < PY_FOOD_FULL)
						/* No heal if we are "full" */
						(void)hp_player(dummy);
					else
#ifdef JP
						msg_print("���ʤ��϶�ʢ�ǤϤ���ޤ���");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = p_ptr->food + MIN(5000, 100 * dummy);
					if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("�������Ҥɤ�̣����");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;
		}
	}

	else 
	{

	switch (p_ptr->prace)
	{
		case RACE_DWARF:
#ifdef JP
			msg_print("���Ϥ�Ĵ�٤���");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			break;

		case RACE_HOBBIT:
			{
				object_type *q_ptr;
				object_type forge;

				/* Get local object */
				q_ptr = &forge;

				/* Create the food ration */
				object_prep(q_ptr, 21);

				/* Drop the object from heaven */
				(void)drop_near(q_ptr, -1, py, px);
#ifdef JP
				msg_print("�������������ƺ�ä���");
#else
				msg_print("You cook some food.");
#endif

			}
			break;

		case RACE_GNOME:
#ifdef JP
			msg_print("�ѥá�");
#else
			msg_print("Blink!");
#endif

			teleport_player(10);
			break;

		case RACE_HALF_ORC:
#ifdef JP
			msg_print("ͦ����Ф�����");
#else
			msg_print("You play tough.");
#endif

			(void)set_afraid(0);
			break;

		case RACE_HALF_TROLL:
#ifdef JP
			msg_print("����������");
#else
			msg_print("RAAAGH!");
#endif

			(void)set_afraid(0);
			(void)set_shero(10 + randint1(plev), FALSE);
			(void)hp_player(30);
			break;

		case RACE_AMBERITE:
			if (command == -1)
			{
				/* No effect in arena or quest */
				if (p_ptr->inside_arena || p_ptr->inside_quest)
				{
#ifdef JP
					msg_print("���̤��ʤ��ä���");
#else
					msg_print("There is no effect.");
#endif

				}
				else
				{
#ifdef JP
					msg_print("���ʤ����⤭����Ϥ᤿�����Ϥ��Ѳ����Ƥ��롣");
#else
					msg_print("You start walking around. Your surroundings change.");
#endif


					if (autosave_l) do_cmd_save_game(TRUE);

					/* Leaving */
					p_ptr->leaving = TRUE;
				}
			}
			else if (command == -2)
			{
#ifdef JP
				msg_print("���ʤ��ϡ֥ѥ�����פ򿴤������Ƥ��ξ���⤤��...");
#else
				msg_print("You picture the Pattern in your mind and walk it...");
#endif

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
			}
			break;

		case RACE_BARBARIAN:
#ifdef JP
			msg_print("������������");
#else
			msg_print("Raaagh!");
#endif

			(void)set_afraid(0);
			(void)set_shero(10 + randint1(plev), FALSE);
			(void)hp_player(30);
			break;

		case RACE_HALF_OGRE:
#ifdef JP
			msg_print("��ȯ�Υ롼��򿵽Ť˻ųݤ���...");
#else
			msg_print("You carefully set an explosive rune...");
#endif

			explosive_rune();
			break;

		case RACE_HALF_GIANT:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�Ф��ɤ�á���Ĥ�����");
#else
			msg_print("You bash at a stone wall.");
#endif

			(void)wall_to_mud(dir);
			break;

		case RACE_HALF_TITAN:
#ifdef JP
			msg_print("Ũ��Ĵ������...");
#else
			msg_print("You examine your foes...");
#endif

			probing();
			break;

		case RACE_CYCLOPS:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("����ʴ���ꤲ����");
#else
			msg_print("You throw a huge boulder.");
#endif

			fire_bolt(GF_MISSILE, dir, (3 * plev) / 2);
			break;

		case RACE_YEEK:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�Ȥ��Ӥ����Ķ�������夲����");
#else
			msg_print("You make a horrible scream!");
#endif

			(void)fear_monster(dir, plev);
			break;

		case RACE_KLACKON:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�����Ǥ�����");
#else
			msg_print("You spit acid.");
#endif

			if (plev < 25) fire_bolt(GF_ACID, dir, plev);
			else fire_ball(GF_ACID, dir, plev, 2);
			break;

		case RACE_KOBOLD:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�ǤΥ����Ĥ��ꤲ����");
#else
			msg_print("You throw a dart of poison.");
#endif

			fire_bolt(GF_POIS, dir, plev);
			break;

		case RACE_NIBELUNG:
#ifdef JP
			msg_print("���Ϥ�Ĵ��������");
#else
			msg_print("You examine your surroundings.");
#endif

			(void)detect_traps(DETECT_RAD_DEFAULT, TRUE);
			(void)detect_doors(DETECT_RAD_DEFAULT);
			(void)detect_stairs(DETECT_RAD_DEFAULT);
			break;

		case RACE_DARK_ELF:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("�ޥ��å����ߥ���������ä���");
#else
			msg_print("You cast a magic missile.");
#endif

			fire_bolt_or_beam(10, GF_MISSILE, dir,
			    damroll(3 + ((plev - 1) / 5), 4));
			break;

		case RACE_DRACONIAN:
			{
				int  Type = (one_in_(3) ? GF_COLD : GF_FIRE);
#ifdef JP
				cptr Type_desc = ((Type == GF_COLD) ? "�䵤" : "��");
#else
				cptr Type_desc = ((Type == GF_COLD) ? "cold" : "fire");
#endif

				if (!get_aim_dir(&dir)) return FALSE;

				if (randint1(100) < plev)
				{
					switch (p_ptr->pclass)
					{
						case CLASS_WARRIOR:
						case CLASS_BERSERKER:
						case CLASS_RANGER:
						case CLASS_TOURIST:
						case CLASS_IMITATOR:
						case CLASS_ARCHER:
						case CLASS_SMITH:
							if (one_in_(3))
							{
								Type = GF_MISSILE;
#ifdef JP
								Type_desc = "�������";
#else
								Type_desc = "the elements";
#endif
							}
							else
							{
								Type = GF_SHARDS;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "shards";
#endif
							}
							break;
						case CLASS_MAGE:
						case CLASS_WARRIOR_MAGE:
						case CLASS_HIGH_MAGE:
						case CLASS_SORCERER:
						case CLASS_MAGIC_EATER:
						case CLASS_RED_MAGE:
						case CLASS_BLUE_MAGE:
						case CLASS_MIRROR_MASTER:
							if (one_in_(3))
							{
								Type = GF_MANA;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "mana";
#endif
							}
							else
							{
								Type = GF_DISENCHANT;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "disenchantment";
#endif
							}
							break;
						case CLASS_CHAOS_WARRIOR:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_CHAOS;
#ifdef JP
								Type_desc = "������";
#else
								Type_desc = "chaos";
#endif
							}
							break;
						case CLASS_MONK:
						case CLASS_SAMURAI:
						case CLASS_FORCETRAINER:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "�첻";
#else
								Type_desc = "sound";
#endif
							}
							break;
						case CLASS_MINDCRAFTER:
							if (!one_in_(3))
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							else
							{
								Type = GF_PSI;
#ifdef JP
								Type_desc = "�������ͥ륮��";
#else
								Type_desc = "mental energy";
#endif
							}
							break;
						case CLASS_PRIEST:
						case CLASS_PALADIN:
							if (one_in_(3))
							{
								Type = GF_HELL_FIRE;
#ifdef JP
								Type_desc = "�Ϲ��ι��";
#else
								Type_desc = "hellfire";
#endif
							}
							else
							{
								Type = GF_HOLY_FIRE;
#ifdef JP
								Type_desc = "���ʤ��";
#else
								Type_desc = "holy fire";
#endif
							}
							break;
						case CLASS_ROGUE:
						case CLASS_NINJA:
							if (one_in_(3))
							{
								Type = GF_DARK;
#ifdef JP
								Type_desc = "�Ź�";
#else
								Type_desc = "darkness";
#endif
							}
							else
							{
								Type = GF_POIS;
#ifdef JP
								Type_desc = "��";
#else
								Type_desc = "poison";
#endif
							}
							break;
						case CLASS_BARD:
							if (!one_in_(3))
							{
								Type = GF_SOUND;
#ifdef JP
								Type_desc = "�첻";
#else
								Type_desc = "sound";
#endif
							}
							else
							{
								Type = GF_CONFUSION;
#ifdef JP
								Type_desc = "����";
#else
								Type_desc = "confusion";
#endif
							}
							break;
					}
				}

#ifdef JP
				msg_format("���ʤ���%s�Υ֥쥹���Ǥ�����", Type_desc);
#else
				msg_format("You breathe %s.", Type_desc);
#endif

				fire_ball(Type, dir, plev * 2,
				    -(plev / 15) - 1);
			}
			break;

		case RACE_MIND_FLAYER:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���ʤ��Ͻ��椷���ܤ��֤�������...");
#else
			msg_print("You concentrate and your eyes glow red...");
#endif

			fire_bolt(GF_PSI, dir, plev);
			break;

		case RACE_IMP:
			if (!get_aim_dir(&dir)) return FALSE;
			if (plev >= 30)
			{
#ifdef JP
				msg_print("�ե��������ܡ�������ä���");
#else
				msg_print("You cast a ball of fire.");
#endif

				fire_ball(GF_FIRE, dir, plev, 2);
			}
			else
			{
#ifdef JP
				msg_print("�ե��������ܥ�Ȥ����ä���");
#else
				msg_print("You cast a bolt of fire.");
#endif

				fire_bolt(GF_FIRE, dir, plev);
			}
			break;

		case RACE_GOLEM:
			(void)set_shield(randint1(20) + 30, FALSE);
			break;

		case RACE_SKELETON:
		case RACE_ZOMBIE:
#ifdef JP
			msg_print("���ʤ��ϼ��ä����ͥ륮�������᤽���Ȼ�ߤ���");
#else
			msg_print("You attempt to restore your lost energies.");
#endif

			(void)restore_level();
			break;

		case RACE_VAMPIRE:
			if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
			{
#ifdef JP
				msg_print("�ʤ������⤹�뤳�Ȥ��Ǥ��ʤ���");
#else
				msg_print("Something prevent you from attacking.");
#endif
				return FALSE;
			}
			else
			{
				int y, x, dummy = 0;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir(&dir,FALSE)) return FALSE;   /* was get_aim_dir */
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
					msg_print("����ʤ����˳��ߤĤ�����");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}

#ifdef JP
				msg_print("���ʤ��ϥ˥��Ȥ��Ʋ��त��...");
#else
				msg_print("You grin and bare your fangs...");
#endif

				dummy = plev + randint1(plev) * MAX(1, plev / 10);   /* Dmg */
				if (drain_life(dir, dummy))
				{
					if (p_ptr->food < PY_FOOD_FULL)
						/* No heal if we are "full" */
						(void)hp_player(dummy);
					else
#ifdef JP
						msg_print("���ʤ��϶�ʢ�ǤϤ���ޤ���");
#else
						msg_print("You were not hungry.");
#endif

					/* Gain nutritional sustenance: 150/hp drained */
					/* A Food ration gives 5000 food points (by contrast) */
					/* Don't ever get more than "Full" this way */
					/* But if we ARE Gorged,  it won't cure us */
					dummy = p_ptr->food + MIN(5000, 100 * dummy);
					if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX - 1 : dummy);
				}
				else
#ifdef JP
					msg_print("�������Ҥɤ�̣����");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;

		case RACE_SPECTRE:
			if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
			msg_print("���ʤ��Ϥ��ɤ��ɤ����������򤢤�����");
#else
			msg_print("You emit an eldritch howl!");
#endif

			(void)fear_monster(dir, plev);
			break;

		case RACE_SPRITE:
#ifdef JP
			msg_print("���ʤ�����ˡ��ʴ���ꤲ�Ĥ���...");
#else
			msg_print("You throw some magic dust...");
#endif

			if (plev < 25) sleep_monsters_touch();
			else (void)sleep_monsters();
			break;

		case RACE_DEMON:
			{
				int type = (one_in_(2) ? GF_NETHER : GF_FIRE);
				if (!get_aim_dir(&dir)) return FALSE;
#ifdef JP
				msg_format("���ʤ���%s�Υ֥쥹���Ǥ�����",((type == GF_NETHER) ? "�Ϲ�" : "�б�"));
#else
				msg_format("You breathe %s.",((type == GF_NETHER) ? "nether" : "fire"));
#endif

				fire_ball(type, dir, plev * 3, -(plev / 15) - 1);
			}
			break;

		case RACE_KUTA:
			(void)set_tsubureru(randint1(20) + 30, FALSE);
			break;

		case RACE_ANDROID:
			if (!get_aim_dir(&dir)) return FALSE;
			if (plev < 10)
			{
#ifdef JP
				msg_print("�쥤�����ȯ�ͤ�����");
#else
				msg_print("You fire your ray gun.");
#endif
				fire_bolt(GF_MISSILE, dir, (plev+1) / 2);
			}
			else if (plev < 25)
			{
#ifdef JP
				msg_print("�֥饹������ȯ�ͤ�����");
#else
				msg_print("You fire your blaster.");
#endif
				fire_bolt(GF_MISSILE, dir, plev);
			}
			else if (plev < 35)
			{
#ifdef JP
				msg_print("�Х�������ȯ�ͤ�����");
#else
				msg_print("You fire your bazooka.");
#endif
				fire_ball(GF_MISSILE, dir, plev * 2, 2);
			}
			else if (plev < 45)
			{
#ifdef JP
				msg_print("�ӡ��७��Υ��ȯ�ͤ�����");
#else
				msg_print("You fire a beam cannon.");
#endif
				fire_beam(GF_MISSILE, dir, plev * 2);
			}
			else
			{
#ifdef JP
				msg_print("���åȤ�ȯ�ͤ�����");
#else
				msg_print("You fire a rocket.");
#endif
				fire_ball(GF_ROCKET, dir, plev * 5, 2);
			}
			break;

		default:
#ifdef JP
			msg_print("���μ�²���ü��ǽ�Ϥ���äƤ��ޤ���");
#else
			msg_print("This race has no bonus power.");
#endif

			energy_use = 0;
	}
	}
	return TRUE;
}


/*
 * Allow user to choose a power (racial / mutation) to activate
 */
void do_cmd_racial_power(void)
{
	power_desc_type power_desc[36];
	int             num, i = 0;
	int             ask = TRUE;
	int             lvl = p_ptr->lev;
	bool            flag, redraw, cast = FALSE;
	bool            warrior = ((p_ptr->pclass == CLASS_WARRIOR || p_ptr->pclass == CLASS_BERSERKER) ? TRUE : FALSE);
	char            choice;
	char            out_val[160];
	int menu_line = (use_menu ? 1 : 0);


	for (num = 0; num < 36; num++)
	{
		strcpy(power_desc[num].name, "");
		power_desc[num].number = 0;
	}

	num = 0;

	if (p_ptr->confused)
	{
#ifdef JP
msg_print("���𤷤Ƥ����ü�ǽ�Ϥ�Ȥ��ޤ���");
#else
		msg_print("You are too confused to use any powers!");
#endif

		energy_use = 0;
		return;
	}

	if (p_ptr->special_defense & (KATA_MUSOU | KATA_KOUKIJIN))
	{
		set_action(ACTION_NONE);
	}

	switch (p_ptr->pclass)
	{
	case CLASS_WARRIOR:
	{
#ifdef JP
strcpy(power_desc[num].name, "������");
#else
		strcpy(power_desc[num].name, "Sword Dancing");
#endif

		power_desc[num].level = 40;
		power_desc[num].cost = 75;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 35;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MAGE:
	case CLASS_HIGH_MAGE:
	case CLASS_SORCERER:
	{
#ifdef JP
strcpy(power_desc[num].name, "���Ͽ���");
#else
		strcpy(power_desc[num].name, "Eat Magic");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 1;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 25;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_PRIEST:
	{
		if (is_good_realm(p_ptr->realm1))
		{
#ifdef JP
strcpy(power_desc[num].name, "����ʡ");
#else
			strcpy(power_desc[num].name, "Bless Weapon");
#endif

			power_desc[num].level = 35;
			power_desc[num].cost = 70;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 50;
			power_desc[num++].number = -3;
		}
		else
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Evocation");
#endif

			power_desc[num].level = 42;
			power_desc[num].cost = 40;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 35;
			power_desc[num++].number = -3;
		}
		break;
	}
	case CLASS_ROGUE:
	{
#ifdef JP
strcpy(power_desc[num].name, "�ҥåȡ���������");
#else
		strcpy(power_desc[num].name, "Hit and Away");
#endif

		power_desc[num].level = 8;
		power_desc[num].cost = 12;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 14;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_RANGER:
	{
#ifdef JP
strcpy(power_desc[num].name, "��󥹥���Ĵ��");
#else
		strcpy(power_desc[num].name, "Probe Monster");
#endif

		power_desc[num].level = 15;
		power_desc[num].cost = 20;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 12;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_PALADIN:
	{
		if (is_good_realm(p_ptr->realm1))
		{
#ifdef JP
strcpy(power_desc[num].name, "�ۡ��꡼�����");
#else
			strcpy(power_desc[num].name, "Holy Lance");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 30;
			power_desc[num++].number = -3;
		}
		else
		{
#ifdef JP
strcpy(power_desc[num].name, "�إ롦���");
#else
			strcpy(power_desc[num].name, "Hell Lance");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 30;
			power_desc[num++].number = -3;
		}
		break;
	}
	case CLASS_WARRIOR_MAGE:
	{
#ifdef JP
strcpy(power_desc[num].name, "�Ѵ�: �ȣТ��ͣ�");
#else
		strcpy(power_desc[num].name, "Convert HP to SP");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�Ѵ�: �ͣТ��ȣ�");
#else
		strcpy(power_desc[num].name, "Convert SP to HP");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 10;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_CHAOS_WARRIOR:
	{
#ifdef JP
strcpy(power_desc[num].name, "���Ǥθ�");
#else
		strcpy(power_desc[num].name, "Confusing Light");
#endif

		power_desc[num].level = 40;
		power_desc[num].cost = 50;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 25;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MONK:
	{
#ifdef JP
strcpy(power_desc[num].name, "������");
#else
		strcpy(power_desc[num].name, "Assume a Posture");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "ɴ����");
#else
		strcpy(power_desc[num].name, "Double Attack");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 30;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_MINDCRAFTER:
	case CLASS_FORCETRAINER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�����߿�");
#else
		strcpy(power_desc[num].name, "Clear Mind");
#endif

		power_desc[num].level = 15;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_TOURIST:
	{
#ifdef JP
strcpy(power_desc[num].name, "�̿�����");
#else
		strcpy(power_desc[num].name, "Take a Photograph");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "��������");
#else
		strcpy(power_desc[num].name, "Identify True");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 20;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_IMITATOR:
	{
#ifdef JP
strcpy(power_desc[num].name, "���֤�");
#else
		strcpy(power_desc[num].name, "Double Revenge");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 100;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 30;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_BEASTMASTER:
	{
#ifdef JP
strcpy(power_desc[num].name, "��ʪ����");
#else
		strcpy(power_desc[num].name, "Dominate a Living Thing");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = (p_ptr->lev+3)/4;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "������ʪ����");
#else
		strcpy(power_desc[num].name, "Dominate Living Things");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = (p_ptr->lev+20)/2;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_ARCHER:
	{
#ifdef JP
strcpy(power_desc[num].name, "��/�����¤");
#else
		strcpy(power_desc[num].name, "Create Ammo");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MAGIC_EATER:
	{
#ifdef JP
strcpy(power_desc[num].name, "���Ϥμ�����");
#else
		strcpy(power_desc[num].name, "Absorb Magic");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_BARD:
	{
#ifdef JP
strcpy(power_desc[num].name, "�Τ�ߤ��");
#else
		strcpy(power_desc[num].name, "Stop Singing");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_CHR;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_RED_MAGE:
	{
#ifdef JP
strcpy(power_desc[num].name, "Ϣ³��");
#else
		strcpy(power_desc[num].name, "Double Magic");
#endif

		power_desc[num].level = 48;
		power_desc[num].cost = 20;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_SAMURAI:
	{
#ifdef JP
strcpy(power_desc[num].name, "���礤����");
#else
		strcpy(power_desc[num].name, "Concentration");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_WIS;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "��");
#else
		strcpy(power_desc[num].name, "Assume a Posture");
#endif

		power_desc[num].level = 25;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_BLUE_MAGE:
	{
#ifdef JP
strcpy(power_desc[num].name, "�顼�˥�");
#else
		strcpy(power_desc[num].name, "Learning");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_CAVALRY:
	{
#ifdef JP
strcpy(power_desc[num].name, "���Ϥʤ餷");
#else
		strcpy(power_desc[num].name, "Rodeo");
#endif

		power_desc[num].level = 10;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_STR;
		power_desc[num].fail = 10;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_BERSERKER:
	{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
		strcpy(power_desc[num].name, "Recall");
#endif

		power_desc[num].level = 10;
		power_desc[num].cost = 10;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 20;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_MIRROR_MASTER:
	{
#ifdef JP
strcpy(power_desc[num].name, "�����");
#else
		strcpy(power_desc[num].name, "Break Mirrors");
#endif

		power_desc[num].level = 1;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
#ifdef JP
strcpy(power_desc[num].name, "�ſ�");
#else
		strcpy(power_desc[num].name, "Mirror Concentration");
#endif

		power_desc[num].level = 30;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -4;
		break;
	}
	case CLASS_SMITH:
	{
#ifdef JP
strcpy(power_desc[num].name, "������");
#else
		strcpy(power_desc[num].name, "Judgment");
#endif

		power_desc[num].level = 5;
		power_desc[num].cost = 15;
		power_desc[num].stat = A_INT;
		power_desc[num].fail = 20;
		power_desc[num++].number = -3;
		break;
	}
	case CLASS_NINJA:
	{
#ifdef JP
strcpy(power_desc[num].name, "®�");
#else
		strcpy(power_desc[num].name, "Quick Walk");
#endif

		power_desc[num].level = 20;
		power_desc[num].cost = 0;
		power_desc[num].stat = A_DEX;
		power_desc[num].fail = 0;
		power_desc[num++].number = -3;
		break;
	}
	default:
#ifdef JP
strcpy(power_desc[0].name, "(�ʤ�)");
#else
		strcpy(power_desc[0].name, "(none)");
#endif

	}

	if (p_ptr->mimic_form)
	{
		switch (p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
#ifdef JP
sprintf(power_desc[num].name, "�Ϲ�/�б�Υ֥쥹 (���᡼�� %d)", lvl * 3);
#else
			sprintf(power_desc[num].name, "Nether or Fire Breath (dam %d)", lvl * 3);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10+lvl/3;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 20;
			power_desc[num++].number = -1;
			break;
		case MIMIC_VAMPIRE:
#ifdef JP
strcpy(power_desc[num].name, "��̿�ϵۼ�");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 1 + (lvl / 3);
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 9;
			power_desc[num++].number = -1;
			break;
		}
	}
	else
	{
	switch (p_ptr->prace)
	{
		case RACE_DWARF:
#ifdef JP
strcpy(power_desc[num].name, "�ɥ���� ����");
#else
			strcpy(power_desc[num].name, "Detect Doors+Traps");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_NIBELUNG:
#ifdef JP
strcpy(power_desc[num].name, "�ɥ���� ����");
#else
			strcpy(power_desc[num].name, "Detect Doors+Traps");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
			break;
		case RACE_HOBBIT:
#ifdef JP
strcpy(power_desc[num].name, "��������");
#else
			strcpy(power_desc[num].name, "Create Food");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
			break;
		case RACE_GNOME:
#ifdef JP
sprintf(power_desc[num].name, "���硼�ȡ��ƥ�ݡ���");
#else
			sprintf(power_desc[num].name, "Blink");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_HALF_ORC:
#ifdef JP
strcpy(power_desc[num].name, "���ݽ���");
#else
			strcpy(power_desc[num].name, "Remove Fear");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = warrior ? 5 : 10;
			power_desc[num++].number = -1;
			break;
		case RACE_HALF_TROLL:
#ifdef JP
strcpy(power_desc[num].name, "����β�");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = warrior ? 6 : 12;
			power_desc[num++].number = -1;
			break;
		case RACE_BARBARIAN:
#ifdef JP
strcpy(power_desc[num].name, "����β�");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = warrior ? 6 : 12;
			power_desc[num++].number = -1;
			break;
		case RACE_AMBERITE:
#ifdef JP
strcpy(power_desc[num].name, "����ɥ������ե�");
#else
			strcpy(power_desc[num].name, "Shadow Shifting");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 50;
			power_desc[num++].number = -1;
#ifdef JP
strcpy(power_desc[num].name, "�ѥ����󡦥�������");
#else
			strcpy(power_desc[num].name, "Pattern Mindwalking");
#endif

			power_desc[num].level = 40;
			power_desc[num].cost = 75;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 50;
			power_desc[num++].number = -2;
			break;
		case RACE_HALF_OGRE:
#ifdef JP
strcpy(power_desc[num].name, "��ȯ�Υ롼��");
#else
			strcpy(power_desc[num].name, "Explosive Rune");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 35;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
			break;
		case RACE_HALF_GIANT:
#ifdef JP
strcpy(power_desc[num].name, "�����ϲ�");
#else
			strcpy(power_desc[num].name, "Stone to Mud");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_HALF_TITAN:
#ifdef JP
strcpy(power_desc[num].name, "������󡦥�󥹥���");
#else
			strcpy(power_desc[num].name, "Probing");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_CYCLOPS:
#ifdef JP
sprintf(power_desc[num].name, "�����ꤲ�ʥ��᡼�� %d��", (3 * lvl) / 2);
#else
			sprintf(power_desc[num].name, "Throw Boulder (dam %d)", (3 * lvl) / 2);
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_YEEK:
#ifdef JP
strcpy(power_desc[num].name, "��󥹥�������");
#else
			strcpy(power_desc[num].name, "Scare Monster");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 10;
			power_desc[num++].number = -1;
			break;
		case RACE_SPECTRE:
#ifdef JP
strcpy(power_desc[num].name, "��󥹥�������");
#else
			strcpy(power_desc[num].name, "Scare Monster");
#endif

			power_desc[num].level = 4;
			power_desc[num].cost = 6;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 3;
			power_desc[num++].number = -1;
			break;
		case RACE_KLACKON:
#ifdef JP
sprintf(power_desc[num].name, "������ (���᡼�� %d)", lvl);
#else
			sprintf(power_desc[num].name, "Spit Acid (dam %d)", lvl);
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
			break;
		case RACE_KOBOLD:
#ifdef JP
sprintf(power_desc[num].name, "�ǤΥ����� (���᡼�� %d)", lvl);
#else
			sprintf(power_desc[num].name, "Poison Dart (dam %d)", lvl);
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 8;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
			break;
		case RACE_DARK_ELF:
#ifdef JP
sprintf(power_desc[num].name, "�ޥ��å����ߥ����� (���᡼�� %dd%d)", 3 + ((lvl - 1) / 5), 4);
#else
			sprintf(power_desc[num].name, "Magic Missile (dm %dd%d)", 3 + ((lvl - 1) / 5), 4);
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 2;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 9;
			power_desc[num++].number = -1;
			break;
		case RACE_DRACONIAN:
#ifdef JP
sprintf(power_desc[num].name, "�֥쥹 (���᡼�� %d)", lvl * 2);
#else
			sprintf(power_desc[num].name, "Breath Weapon (dam %d)", lvl * 2);
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = lvl;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 12;
			power_desc[num++].number = -1;
			break;
		case RACE_MIND_FLAYER:
#ifdef JP
sprintf(power_desc[num].name, "�������� (���᡼�� %d)", lvl);
#else
			sprintf(power_desc[num].name, "Mind Blast (dam %d)", lvl);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 14;
			power_desc[num++].number = -1;
			break;
		case RACE_IMP:
#ifdef JP
sprintf(power_desc[num].name, "�ե��������ܥ��/�ܡ��� (���᡼�� %d)", lvl);
#else
			sprintf(power_desc[num].name, "Fire Bolt/Ball (dam %d)", lvl);
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
			break;
		case RACE_GOLEM:
#ifdef JP
strcpy(power_desc[num].name, "ȩ�в� (���� 1d20+30)");
#else
			strcpy(power_desc[num].name, "Stone Skin (dur 1d20+30)");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 8;
			power_desc[num++].number = -1;
			break;
		case RACE_SKELETON:
		case RACE_ZOMBIE:
#ifdef JP
strcpy(power_desc[num].name, "�и�������");
#else
			strcpy(power_desc[num].name, "Restore Life");
#endif

			power_desc[num].level = 30;
			power_desc[num].cost = 30;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 18;
			power_desc[num++].number = -1;
			break;
		case RACE_VAMPIRE:
#ifdef JP
strcpy(power_desc[num].name, "��̿�ϵۼ�");
#else
			strcpy(power_desc[num].name, "Drain Life");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 1 + (lvl / 3);
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 9;
			power_desc[num++].number = -1;
			break;
		case RACE_SPRITE:
#ifdef JP
strcpy(power_desc[num].name, "̲��ʴ");
#else
			strcpy(power_desc[num].name, "Sleeping Dust");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = -1;
			break;
		case RACE_DEMON:
#ifdef JP
sprintf(power_desc[num].name, "�Ϲ�/�б�Υ֥쥹 (���᡼�� %d)", lvl * 3);
#else
			sprintf(power_desc[num].name, "Nether or Fire Breath (dam %d)", lvl * 3);
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 10+lvl/3;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 20;
			power_desc[num++].number = -1;
			break;
		case RACE_KUTA:
#ifdef JP
strcpy(power_desc[num].name, "���˿��Ӥ�");
#else
			strcpy(power_desc[num].name, "Expand Horizontally (dur 30+1d20)");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 8;
			power_desc[num++].number = -1;
			break;
		case RACE_ANDROID:
			if (p_ptr->lev < 10)
			{
#ifdef JP
strcpy(power_desc[num].name, "�쥤����");
#else
				strcpy(power_desc[num].name, "Ray Gun");
#endif

				power_desc[num].level = 1;
				power_desc[num].cost = 7;
				power_desc[num].fail = 8;
			}
			else if (p_ptr->lev < 25)
			{
#ifdef JP
strcpy(power_desc[num].name, "�֥饹����");
#else
				strcpy(power_desc[num].name, "Blaster");
#endif

				power_desc[num].level = 10;
				power_desc[num].cost = 13;
				power_desc[num].fail = 10;
			}
			else if (p_ptr->lev < 35)
			{
#ifdef JP
strcpy(power_desc[num].name, "�Х�����");
#else
				strcpy(power_desc[num].name, "Bazooka");
#endif

				power_desc[num].level = 25;
				power_desc[num].cost = 26;
				power_desc[num].fail = 12;
			}
			else if (p_ptr->lev < 45)
			{
#ifdef JP
strcpy(power_desc[num].name, "�ӡ��७��Υ�");
#else
				strcpy(power_desc[num].name, "Beam Cannon");
#endif

				power_desc[num].level = 35;
				power_desc[num].cost = 40;
				power_desc[num].fail = 15;
			}
			else
			{
#ifdef JP
strcpy(power_desc[num].name, "���å�");
#else
				strcpy(power_desc[num].name, "Rocket");
#endif

				power_desc[num].level = 45;
				power_desc[num].cost = 60;
				power_desc[num].fail = 18;
			}
			power_desc[num].stat = A_STR;
			power_desc[num++].number = -1;
			break;
		default:
		{
			break;
		}
	}
	}

	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_SPIT_ACID)
		{
#ifdef JP
strcpy(power_desc[num].name, "������");
#else
			strcpy(power_desc[num].name, "Spit Acid");
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 15;
			power_desc[num++].number = MUT1_SPIT_ACID;
		}

		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
strcpy(power_desc[num].name, "��Υ֥쥹");
#else
			strcpy(power_desc[num].name, "Fire Breath");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = lvl;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num++].number = MUT1_BR_FIRE;
		}

		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
strcpy(power_desc[num].name, "��̲�ˤ�");
#else
			strcpy(power_desc[num].name, "Hypnotic Gaze");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 18;
			power_desc[num++].number = MUT1_HYPN_GAZE;
		}

		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
strcpy(power_desc[num].name, "ǰư��");
#else
			strcpy(power_desc[num].name, "Telekinesis");
#endif

			power_desc[num].level = 9;
			power_desc[num].cost = 9;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 14;
			power_desc[num++].number = MUT1_TELEKINES;
		}

		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ƥ�ݡ���");
#else
			strcpy(power_desc[num].name, "Teleport");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 7;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = MUT1_VTELEPORT;
		}

		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
strcpy(power_desc[num].name, "��������");
#else
			strcpy(power_desc[num].name, "Mind Blast");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = MUT1_MIND_BLST;
		}

		if (p_ptr->muta1 & MUT1_RADIATION)
		{
#ifdef JP
strcpy(power_desc[num].name, "����ǽ");
#else
			strcpy(power_desc[num].name, "Emit Radiation");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 14;
			power_desc[num++].number = MUT1_RADIATION;
		}

		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
strcpy(power_desc[num].name, "�۷�ɥ쥤��");
#else
			strcpy(power_desc[num].name, "Vampiric Drain");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = (1 + (lvl / 3));
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 9;
			power_desc[num++].number = MUT1_VAMPIRISM;
		}

		if (p_ptr->muta1 & MUT1_SMELL_MET)
		{
#ifdef JP
strcpy(power_desc[num].name, "��°�̳�");
#else
			strcpy(power_desc[num].name, "Smell Metal");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 2;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = MUT1_SMELL_MET;
		}

		if (p_ptr->muta1 & MUT1_SMELL_MON)
		{
#ifdef JP
strcpy(power_desc[num].name, "Ũ���̳�");
#else
			strcpy(power_desc[num].name, "Smell Monsters");
#endif

			power_desc[num].level = 5;
			power_desc[num].cost = 4;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 15;
			power_desc[num++].number = MUT1_SMELL_MON;
		}

		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
strcpy(power_desc[num].name, "���硼�ȡ��ƥ�ݡ���");
#else
			strcpy(power_desc[num].name, "Blink");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 3;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 12;
			power_desc[num++].number = MUT1_BLINK;
		}

		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
strcpy(power_desc[num].name, "�俩��");
#else
			strcpy(power_desc[num].name, "Eat Rock");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num++].number = MUT1_EAT_ROCK;
		}

		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
strcpy(power_desc[num].name, "���ָ�");
#else
			strcpy(power_desc[num].name, "Swap Position");
#endif

			power_desc[num].level = 15;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 16;
			power_desc[num++].number = MUT1_SWAP_POS;
		}

		if (p_ptr->muta1 & MUT1_SHRIEK)
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Shriek");
#endif

			power_desc[num].level = 20;
			power_desc[num].cost = 14;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 16;
			power_desc[num++].number = MUT1_SHRIEK;
		}

		if (p_ptr->muta1 & MUT1_ILLUMINE)
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Illuminate");
#endif

			power_desc[num].level = 3;
			power_desc[num].cost = 2;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = MUT1_ILLUMINE;
		}

		if (p_ptr->muta1 & MUT1_DET_CURSE)
		{
#ifdef JP
strcpy(power_desc[num].name, "��������");
#else
			strcpy(power_desc[num].name, "Detect Curses");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 14;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 14;
			power_desc[num++].number = MUT1_DET_CURSE;
		}

		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
strcpy(power_desc[num].name, "����β�");
#else
			strcpy(power_desc[num].name, "Berserk");
#endif

			power_desc[num].level = 8;
			power_desc[num].cost = 8;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 14;
			power_desc[num++].number = MUT1_BERSERK;
		}

		if (p_ptr->muta1 & MUT1_POLYMORPH)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ѿ�");
#else
			strcpy(power_desc[num].name, "Polymorph");
#endif

			power_desc[num].level = 18;
			power_desc[num].cost = 20;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 18;
			power_desc[num++].number = MUT1_POLYMORPH;
		}

		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ߥ����μ�");
#else
			strcpy(power_desc[num].name, "Midas Touch");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 5;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 12;
			power_desc[num++].number = MUT1_MIDAS_TCH;
		}

		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
strcpy(power_desc[num].name, "����ȯ��");
#else
			strcpy(power_desc[num].name, "Grow Mold");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = 6;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 14;
			power_desc[num++].number = MUT1_GROW_MOLD;
		}

		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
strcpy(power_desc[num].name, "�����������");
#else
			strcpy(power_desc[num].name, "Resist Elements");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 12;
			power_desc[num++].number = MUT1_RESIST;
		}

		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
strcpy(power_desc[num].name, "�Ͽ�");
#else
			strcpy(power_desc[num].name, "Earthquake");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 16;
			power_desc[num++].number = MUT1_EARTHQUAKE;
		}

		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
strcpy(power_desc[num].name, "���Ͽ���");
#else
			strcpy(power_desc[num].name, "Eat Magic");
#endif

			power_desc[num].level = 17;
			power_desc[num].cost = 1;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 15;
			power_desc[num++].number = MUT1_EAT_MAGIC;
		}

		if (p_ptr->muta1 & MUT1_WEIGH_MAG)
		{
#ifdef JP
strcpy(power_desc[num].name, "���ϴ���");
#else
			strcpy(power_desc[num].name, "Weigh Magic");
#endif

			power_desc[num].level = 6;
			power_desc[num].cost = 6;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 10;
			power_desc[num++].number = MUT1_WEIGH_MAG;
		}

		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
strcpy(power_desc[num].name, "�����˻�");
#else
			strcpy(power_desc[num].name, "Sterilize");
#endif

			power_desc[num].level = 12;
			power_desc[num].cost = 23;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 15;
			power_desc[num++].number = MUT1_STERILITY;
		}

		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ҥåȡ���������");
#else
			strcpy(power_desc[num].name, "Panic Hit");
#endif

			power_desc[num].level = 10;
			power_desc[num].cost = 12;
			power_desc[num].stat = A_DEX;
			power_desc[num].fail = 14;
			power_desc[num++].number = MUT1_PANIC_HIT;
		}

		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Dazzle");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 15;
			power_desc[num].stat = A_CHR;
			power_desc[num].fail = 8;
			power_desc[num++].number = MUT1_DAZZLE;
		}

		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
strcpy(power_desc[num].name, "�졼����������");
#else
			strcpy(power_desc[num].name, "Laser Eye");
#endif

			power_desc[num].level = 7;
			power_desc[num].cost = 10;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 9;
			power_desc[num++].number = MUT1_LASER_EYE;
		}

		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
strcpy(power_desc[num].name, "����");
#else
			strcpy(power_desc[num].name, "Recall");
#endif

			power_desc[num].level = 17;
			power_desc[num].cost = 50;
			power_desc[num].stat = A_INT;
			power_desc[num].fail = 16;
			power_desc[num++].number = MUT1_RECALL;
		}

		if (p_ptr->muta1 & MUT1_BANISH)
		{
#ifdef JP
strcpy(power_desc[num].name, "�ٰ�����");
#else
			strcpy(power_desc[num].name, "Banish Evil");
#endif

			power_desc[num].level = 25;
			power_desc[num].cost = 25;
			power_desc[num].stat = A_WIS;
			power_desc[num].fail = 18;
			power_desc[num++].number = MUT1_BANISH;
		}

		if (p_ptr->muta1 & MUT1_COLD_TOUCH)
		{
#ifdef JP
strcpy(power_desc[num].name, "���μ�");
#else
			strcpy(power_desc[num].name, "Cold Touch");
#endif

			power_desc[num].level = 2;
			power_desc[num].cost = 2;
			power_desc[num].stat = A_CON;
			power_desc[num].fail = 11;
			power_desc[num++].number = MUT1_COLD_TOUCH;
		}

		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
strcpy(power_desc[num].name, "�����ƥ��ꤲ");
#else
			strcpy(power_desc[num].name, "Throw Object");
#endif

			power_desc[num].level = 1;
			power_desc[num].cost = lvl;
			power_desc[num].stat = A_STR;
			power_desc[num].fail = 6;
			/* XXX_XXX_XXX Hack! MUT1_LAUNCHER counts as negative... */
			power_desc[num++].number = 3;
		}
	}

	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
#ifdef JP
(void) strnfmt(out_val, 78, "(�ü�ǽ�� %c-%c, *'�ǰ���, ESC������) �ɤ��ü�ǽ�Ϥ�Ȥ��ޤ�����",
#else
	(void)strnfmt(out_val, 78, "(Powers %c-%c, *=List, ESC=exit) Use which power? ",
#endif

		I2A(0), (num <= 26) ? I2A(num - 1) : '0' + num - 27);

#ifdef ALLOW_REPEAT
if (!repeat_pull(&i) || i<0 || i>=num) {
#endif /* ALLOW_REPEAT */
	if (use_menu) screen_save();
	 /* Get a spell from the user */

	choice = (always_show_list || use_menu) ? ESCAPE:1;
	while (!flag)
	{
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					energy_use = 0;
					return;
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

				case '6':
				case 'l':
				case 'L':
				case '4':
				case 'h':
				case 'H':
				{
					if (menu_line > 18)
						menu_line -= 18;
					else if (menu_line+18 <= num)
						menu_line += 18;
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
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y = 1, x = 0;
				int ctr = 0;
				char dummy[80];
				char letter;
				int x1, y1;

				strcpy(dummy, "");

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				/* Print header(s) */
				if (num < 18)
#ifdef JP
prt("                            Lv   MP ��Ψ", y++, x);
#else
					prt("                            Lv Cost Fail", y++, x);
#endif

				else
#ifdef JP
prt("                            Lv   MP ��Ψ                            Lv   MP ��Ψ", y++, x);
#else
					prt("                            Lv Cost Fail                            Lv Cost Fail", y++, x);
#endif


				/* Print list */
				while (ctr < num)
				{
					x1 = ((ctr < 18) ? x : x + 40);
					y1 = ((ctr < 18) ? y + ctr : y + ctr - 18);

					if (use_menu)
					{
#ifdef JP
						if (ctr == (menu_line-1)) strcpy(dummy, " �� ");
#else
						if (ctr == (menu_line-1)) strcpy(dummy, " >  ");
#endif
						else strcpy(dummy, "    ");
					}
					else
					{
						/* letter/number for power selection */
						if (ctr < 26)
							letter = I2A(ctr);
						else
							letter = '0' + ctr - 26;
						sprintf(dummy, " %c) ",letter);
					}
					strcat(dummy, format("%-23.23s %2d %4d %3d%%",
						power_desc[ctr].name, power_desc[ctr].level, power_desc[ctr].cost,
						100 - racial_chance(&power_desc[ctr])));
					prt(dummy, y1, x1);
					ctr++;
				}
			}

			/* Hide the list */
			else
			{
				/* Hide list */
				redraw = FALSE;

				/* Restore the screen */
				screen_load();
			}

			/* Redo asking */
			continue;
		}

		if (!use_menu)
		{
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
			char tmp_val[160];

			/* Prompt */
#ifdef JP
(void) strnfmt(tmp_val, 78, "%s��Ȥ��ޤ����� ", power_desc[i].name);
#else
			(void)strnfmt(tmp_val, 78, "Use %s? ", power_desc[i].name);
#endif


			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	/* Abort if needed */
	if (!flag)
	{
		energy_use = 0;
		return;
	}
#ifdef ALLOW_REPEAT
	repeat_push(i);
	} /*if (!repeat_pull(&i) || ...)*/
#endif /* ALLOW_REPEAT */
	switch (racial_aux(&power_desc[i]))
	{
	case 1:
		if (power_desc[i].number < 0)
			cast = cmd_racial_power_aux(power_desc[i].number);
		else
			cast = mutation_power_aux(power_desc[i].number);
		break;
	case 0:
		cast = FALSE;
		break;
	case -1:
		cast = TRUE;
		break;
	}

	if (cast)
	{
		if (racial_cost)
		{
			if (racial_use_hp)
			{
#ifdef JP
				take_hit(DAMAGE_USELIFE, (racial_cost / 2) + randint1(racial_cost / 2),
					 "���٤ν���", -1);
#else
				take_hit(DAMAGE_USELIFE, (racial_cost / 2) + randint1(racial_cost / 2),
					 "concentrating too hard", -1);
#endif
			}
			else
			{
				p_ptr->csp -= (racial_cost / 2) + randint1(racial_cost / 2);
			}

			/* Redraw mana and hp */
			p_ptr->redraw |= (PR_HP | PR_MANA);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER | PW_SPELL);
		}
	}
	else energy_use = 0;

	/* Success */
	return;
}
