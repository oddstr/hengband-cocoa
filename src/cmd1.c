/* File: cmd1.c */

/* Purpose: Movement commands (part 1) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"
#define MAX_VAMPIRIC_DRAIN 50


/*
 * Determine if the player "hits" a monster (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_fire(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		if (one_in_(20)) return (FALSE);

	/* Never hit */
	if (chance <= 0) return (FALSE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = (chance + 1) / 2;

	/* Power competes against armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Determine if the player "hits" a monster (normal combat).
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_norm(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		if (one_in_(20)) return (FALSE);

	/* Wimpy attack never hits */
	if (chance <= 0) return (FALSE);

	/* Penalize invisible targets */
	if (!vis) chance = (chance + 1) / 2;

	/* Power must defeat armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
s16b critical_shot(int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = (weight + ((p_ptr->to_h_b + plus) * 4) + (p_ptr->lev * 2));

	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight + randint1(500);

		if (k < 500)
		{
#ifdef JP
			msg_print("�ꤴ���������ä���");
#else
			msg_print("It was a good hit!");
#endif

			dam = 2 * dam + 5;
		}
		else if (k < 1000)
		{
#ifdef JP
			msg_print("���ʤ�μꤴ���������ä���");
#else
			msg_print("It was a great hit!");
#endif

			dam = 2 * dam + 10;
		}
		else
		{
#ifdef JP
			msg_print("�񿴤ΰ�����");
#else
			msg_print("It was a superb hit!");
#endif

			dam = 3 * dam + 15;
		}
	}

	return (dam);
}



/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
s16b critical_norm(int weight, int plus, int dam, s16b meichuu, int mode)
{
	int i, k;

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3));

	/* Chance */
	if ((randint1((p_ptr->pclass == CLASS_NINJA) ? 4444 : 5000) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		k = weight + randint1(650);
		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if (k < 400)
		{
#ifdef JP
			msg_print("�ꤴ���������ä���");
#else
			msg_print("It was a good hit!");
#endif

			dam = 2 * dam + 5;
		}
		else if (k < 700)
		{
#ifdef JP
			msg_print("���ʤ�μꤴ���������ä���");
#else
			msg_print("It was a great hit!");
#endif

			dam = 2 * dam + 10;
		}
		else if (k < 900)
		{
#ifdef JP
			msg_print("�񿴤ΰ�����");
#else
			msg_print("It was a superb hit!");
#endif

			dam = 3 * dam + 15;
		}
		else if (k < 1300)
		{
#ifdef JP
			msg_print("�ǹ�β񿴤ΰ�����");
#else
			msg_print("It was a *GREAT* hit!");
#endif

			dam = 3 * dam + 20;
		}
		else
		{
#ifdef JP
			msg_print("����ʤ��ǹ�β񿴤ΰ�����");
#else
			msg_print("It was a *SUPERB* hit!");
#endif

			dam = ((7 * dam) / 2) + 25;
		}
	}

	return (dam);
}



/*
 * Extract the "total damage" from a given object hitting a given monster.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  XXX XXX
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */
s16b tot_dam_aux(object_type *o_ptr, int tdam, monster_type *m_ptr, int mode)
{
	int mult = 10;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	u32b flgs[TR_FLAG_SIZE];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			/* Slay Animal */
			if ((have_flag(flgs, TR_SLAY_ANIMAL)) &&
			    (r_ptr->flags3 & RF3_ANIMAL))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_ANIMAL;
				}

				if (mult < 25) mult = 25;
			}

			/* Execute Animal */
			if ((have_flag(flgs, TR_KILL_ANIMAL)) &&
			    (r_ptr->flags3 & RF3_ANIMAL))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_ANIMAL;
				}

				if (mult < 40) mult = 40;
			}

			/* Slay Evil */
			if ((have_flag(flgs, TR_SLAY_EVIL)) &&
			    (r_ptr->flags3 & RF3_EVIL))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_EVIL;
				}

				if (mult < 20) mult = 20;
			}

			/* Execute Evil */
			if ((have_flag(flgs, TR_KILL_EVIL)) &&
			    (r_ptr->flags3 & RF3_EVIL))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_EVIL;
				}

				if (mult < 35) mult = 35;
			}

			/* Slay Human */
			if ((have_flag(flgs, TR_SLAY_HUMAN)) &&
			    (r_ptr->flags2 & RF2_HUMAN))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags2 |= RF2_HUMAN;
				}

				if (mult < 25) mult = 25;
			}

			/* Execute Human */
			if ((have_flag(flgs, TR_KILL_HUMAN)) &&
			    (r_ptr->flags2 & RF2_HUMAN))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags2 |= RF2_HUMAN;
				}

				if (mult < 40) mult = 40;
			}

			/* Slay Undead */
			if ((have_flag(flgs, TR_SLAY_UNDEAD)) &&
			    (r_ptr->flags3 & RF3_UNDEAD))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_UNDEAD;
				}

				if (mult < 30) mult = 30;
			}

			/* Execute Undead */
			if ((have_flag(flgs, TR_KILL_UNDEAD)) &&
			    (r_ptr->flags3 & RF3_UNDEAD))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_UNDEAD;
				}

				if (mult < 50) mult = 50;
			}

			/* Slay Demon */
			if ((have_flag(flgs, TR_SLAY_DEMON)) &&
			    (r_ptr->flags3 & RF3_DEMON))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_DEMON;
				}

				if (mult < 30) mult = 30;
			}

			/* Execute Demon */
			if ((have_flag(flgs, TR_KILL_DEMON)) &&
			    (r_ptr->flags3 & RF3_DEMON))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_DEMON;
				}

				if (mult < 50) mult = 50;
			}

			/* Slay Orc */
			if ((have_flag(flgs, TR_SLAY_ORC)) &&
			    (r_ptr->flags3 & RF3_ORC))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_ORC;
				}

				if (mult < 30) mult = 30;
			}

			/* Execute Orc */
			if ((have_flag(flgs, TR_KILL_ORC)) &&
			    (r_ptr->flags3 & RF3_ORC))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_ORC;
				}

				if (mult < 50) mult = 50;
			}

			/* Slay Troll */
			if ((have_flag(flgs, TR_SLAY_TROLL)) &&
			    (r_ptr->flags3 & RF3_TROLL))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_TROLL;
				}

				if (mult < 30) mult = 30;
			}

			/* Execute Troll */
			if ((have_flag(flgs, TR_KILL_TROLL)) &&
			    (r_ptr->flags3 & RF3_TROLL))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_TROLL;
				}

				if (mult < 50) mult = 50;
			}

			/* Slay Giant */
			if ((have_flag(flgs, TR_SLAY_GIANT)) &&
			    (r_ptr->flags3 & RF3_GIANT))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_GIANT;
				}

				if (mult < 30) mult = 30;
				if (o_ptr->name1 == ART_HRUNTING)
					mult *= 3;
			}

			/* Execute Giant */
			if ((have_flag(flgs, TR_KILL_GIANT)) &&
			    (r_ptr->flags3 & RF3_GIANT))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_GIANT;
				}

				if (mult < 50) mult = 50;
			}

			/* Slay Dragon  */
			if ((have_flag(flgs, TR_SLAY_DRAGON)) &&
			    (r_ptr->flags3 & RF3_DRAGON))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_DRAGON;
				}

				if (mult < 30) mult = 30;
			}

			/* Execute Dragon */
			if ((have_flag(flgs, TR_KILL_DRAGON)) &&
			    (r_ptr->flags3 & RF3_DRAGON))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_DRAGON;
				}

				if (mult < 50) mult = 50;

				if ((o_ptr->name1 == ART_NOTHUNG) && (m_ptr->r_idx == MON_FAFNER))
					mult *= 3;
			}

			/* Brand (Acid) */
			if ((have_flag(flgs, TR_BRAND_ACID)) || (p_ptr->special_attack & (ATTACK_ACID)))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & RF3_IM_ACID)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_IM_ACID;
					}
				}

				/* Otherwise, take the damage */
				else
				{
					if (mult < 25) mult = 25;
				}
			}

			/* Brand (Elec) */
			if ((have_flag(flgs, TR_BRAND_ELEC)) || (p_ptr->special_attack & (ATTACK_ELEC)) || (mode == HISSATSU_ELEC))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & RF3_IM_ELEC)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_IM_ELEC;
					}
				}

				/* Otherwise, take the damage */
				else if (((have_flag(flgs, TR_BRAND_ELEC)) || (p_ptr->special_attack & (ATTACK_ELEC))) && (mode == HISSATSU_ELEC))
				{
					if (mult < 70) mult = 70;
				}
				else if (mode == HISSATSU_ELEC)
				{
					if (mult < 50) mult = 50;
				}

				else
				{
					if (mult < 25) mult = 25;
				}
			}

			/* Brand (Fire) */
			if ((have_flag(flgs, TR_BRAND_FIRE)) || (p_ptr->special_attack & (ATTACK_FIRE)) || (mode == HISSATSU_FIRE))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & RF3_IM_FIRE)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_IM_FIRE;
					}
				}

				/* Otherwise, take the damage */
				else if (((have_flag(flgs, TR_BRAND_FIRE)) || (p_ptr->special_attack & (ATTACK_FIRE))) && (mode == HISSATSU_FIRE))
				{
					if (r_ptr->flags3 & RF3_HURT_FIRE)
					{
						if (mult < 70) mult = 70;
						if (m_ptr->ml)
						{
							r_ptr->r_flags3 |= RF3_HURT_FIRE;
						}
					}
					else if (mult < 35) mult = 35;
				}
				else
				{
					if (r_ptr->flags3 & RF3_HURT_FIRE)
					{
						if (mult < 50) mult = 50;
						if (m_ptr->ml)
						{
							r_ptr->r_flags3 |= RF3_HURT_FIRE;
						}
					}
					else if (mult < 25) mult = 25;
				}
			}

			/* Brand (Cold) */
			if ((have_flag(flgs, TR_BRAND_COLD)) || (p_ptr->special_attack & (ATTACK_COLD)) || (mode == HISSATSU_COLD))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & RF3_IM_COLD)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_IM_COLD;
					}
				}
				/* Otherwise, take the damage */
				else if (((have_flag(flgs, TR_BRAND_COLD)) || (p_ptr->special_attack & (ATTACK_COLD))) && (mode == HISSATSU_COLD))
				{
					if (r_ptr->flags3 & RF3_HURT_COLD)
					{
						if (mult < 70) mult = 70;
						if (m_ptr->ml)
						{
							r_ptr->r_flags3 |= RF3_HURT_COLD;
						}
					}
					else if (mult < 35) mult = 35;
				}
				else
				{
					if (r_ptr->flags3 & RF3_HURT_COLD)
					{
						if (mult < 50) mult = 50;
						if (m_ptr->ml)
						{
							r_ptr->r_flags3 |= RF3_HURT_COLD;
						}
					}
					else if (mult < 25) mult = 25;
				}
			}

			/* Brand (Poison) */
			if ((have_flag(flgs, TR_BRAND_POIS)) || (p_ptr->special_attack & (ATTACK_POIS)) || (mode == HISSATSU_POISON))
			{
				/* Notice immunity */
				if (r_ptr->flags3 & RF3_IM_POIS)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_IM_POIS;
					}
				}

				/* Otherwise, take the damage */
				else if (((have_flag(flgs, TR_BRAND_POIS)) || (p_ptr->special_attack & (ATTACK_POIS))) && (mode == HISSATSU_POISON))
				{
					if (mult < 35) mult = 35;
				}
				else
				{
					if (mult < 25) mult = 25;
				}
			}
			if ((mode == HISSATSU_ZANMA) && (r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING)) && (r_ptr->flags3 & RF3_EVIL))
			{
				if (mult < 15) mult = 25;
				else if (mult < 50) mult = MIN(50, mult+20);
			}
			if (mode == HISSATSU_UNDEAD)
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_UNDEAD;
					}
					if (mult == 10) mult = 70;
					else if (mult < 140) mult = MIN(140, mult+60);
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = MIN(60, mult+30);
			}
			if ((mode == HISSATSU_SEKIRYUKA) && p_ptr->cut && !(r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING)))
			{
				int tmp = MIN(100, MAX(10, p_ptr->cut / 10));
				if (mult < tmp) mult = tmp;
			}
			if ((mode == HISSATSU_HAGAN) && (r_ptr->flags3 & RF3_HURT_ROCK))
			{
				if (m_ptr->ml)
				{
					r_ptr->r_flags3 |= RF3_HURT_ROCK;
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = 60;
			}
			if ((p_ptr->pclass != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
			{
				p_ptr->csp -= (1+(o_ptr->dd * o_ptr->ds / 5));
				p_ptr->redraw |= (PR_MANA);
				mult = mult * 3 / 2 + 20;
			}
			break;
		}
	}
	if (mult > 150) mult = 150;

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Search for hidden things
 */
void search(void)
{
	int y, x, chance;

	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;


	/* Start with base search ability */
	chance = p_ptr->skill_srh;

	/* Penalize various conditions */
	if (p_ptr->blind || no_lite()) chance = chance / 10;
	if (p_ptr->confused || p_ptr->image) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (py - 1); y <= (py + 1); y++)
	{
		for (x = (px - 1); x <= (px + 1); x++)
		{
			/* Sometimes, notice things */
			if (randint0(100) < chance)
			{
				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Invisible trap */
				if (c_ptr->mimic && is_trap(c_ptr->feat))
				{
					/* Pick a trap */
					disclose_grid(y, x);

					/* Message */
#ifdef JP
					msg_print("�ȥ�åפ�ȯ��������");
#else
					msg_print("You have found a trap.");
#endif


					/* Disturb */
					disturb(0, 0);
				}

				/* Secret door */
				if (is_hidden_door(c_ptr))
				{
					/* Message */
#ifdef JP
					msg_print("�����ɥ���ȯ��������");
#else
					msg_print("You have found a secret door.");
#endif

					/* Disclose */
					disclose_grid(y, x);

					/* Disturb */
					disturb(0, 0);
				}

				/* Scan all objects in the grid */
				for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Skip non-chests */
					if (o_ptr->tval != TV_CHEST) continue;

					/* Skip non-trapped chests */
					if (!chest_traps[o_ptr->pval]) continue;

					/* Identify once */
					if (!object_known_p(o_ptr))
					{
						/* Message */
#ifdef JP
						msg_print("Ȣ�˻ųݤ���줿�ȥ�åפ�ȯ��������");
#else
						msg_print("You have discovered a trap on the chest!");
#endif


						/* Know the trap */
						object_known(o_ptr);

						/* Notice it */
						disturb(0, 0);
					}
				}
			}
		}
	}
}


/*
 * Helper routine for py_pickup() and py_pickup_floor().
 *
 * Add the given dungeon object to the character's inventory.
 *
 * Delete the object afterwards.
 */
void py_pickup_aux(int o_idx)
{
	int slot, i;

#ifdef JP
/*
 * �����ƥ�򽦤ä��ݤˡ֣��ĤΥ���������äƤ����
 * "You have two cakes." �ȥ����ƥ�򽦤ä���ι�פΤߤ�ɽ�������ꥸ�ʥ�
 * ���������´���
 * ����Ȥ�����Ŧ�򤦤����Τǡ��֡��򽦤ä���������äƤ���פȤ���ɽ��
 * �ˤ����Ƥ��롣���Τ��������
 */
	char o_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;
#else
	char o_name[MAX_NLEN];
#endif

	object_type *o_ptr;

	o_ptr = &o_list[o_idx];

#ifdef JP
	/* Describe the object */
	object_desc(old_name, o_ptr, TRUE, 0);
	object_desc_kosuu(kazu_str, o_ptr);
	hirottakazu = o_ptr->number;
#endif
	/* Carry the object */
	slot = inven_carry(o_ptr);

	/* Get the object again */
	o_ptr = &inventory[slot];

	/* Delete the object */
	delete_object_idx(o_idx);

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) identify_item(o_ptr);

	/* Describe the object */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
#ifdef JP
	if ((o_ptr->name1 == ART_CRIMSON) && (p_ptr->pseikaku == SEIKAKU_COMBAT))
	{
		msg_format("�������ơ�%s�ϡإ���ॾ��٤������줿��", player_name);
		msg_print("�����������غ��٤Υ����ڥ�ȡ٤����ä���󥹥�������");
		msg_format("%s�˽��������롥����", player_name);
	}
	else
	{
		if (plain_pickup)
		{
			msg_format("%s(%c)����äƤ��롣",o_name, index_to_label(slot));
		}
		else
		{
			if (o_ptr->number > hirottakazu) {
			    msg_format("%s���äơ�%s(%c)����äƤ��롣",
			       kazu_str, o_name, index_to_label(slot));
			} else {
				msg_format("%s(%c)�򽦤ä���", o_name, index_to_label(slot));
			}
		}
	}
	strcpy(record_o_name, old_name);
#else
	msg_format("You have %s (%c).", o_name, index_to_label(slot));
	strcpy(record_o_name, o_name);
#endif
	record_turn = turn;


	/* Check if completed a quest */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->name1))
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)p_ptr->lev;
#ifdef JP
			msg_print("�������Ȥ�ã��������");
#else
			msg_print("You completed your quest!");
#endif

			msg_print(NULL);
		}
	}
}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "move_player()" for handling of other things.
 */
void carry(int pickup)
{
	cave_type *c_ptr = &cave[py][px];

	s16b this_o_idx, next_o_idx = 0;

	char	o_name[MAX_NLEN];

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

	/* Automatically pickup/destroy/inscribe items */
	auto_pickup_items(c_ptr);


#ifdef ALLOW_EASY_FLOOR

	if (easy_floor)
	{
		py_pickup_floor(pickup);
		return;
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Scan the pile of objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

#ifdef ALLOW_EASY_SENSE /* TNB */

		/* Option: Make item sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void)sense_object(o_ptr);
		}

#endif /* ALLOW_EASY_SENSE -- TNB */

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			int value = (long)o_ptr->pval;

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Message */
#ifdef JP
		msg_format(" $%ld �β��ͤ�����%s�򸫤Ĥ�����",
			   (long)value, o_name);
#else
			msg_format("You collect %ld gold pieces worth of %s.",
				   (long)value, o_name);
#endif


			sound(SOUND_SELL);

			/* Collect the gold */
			p_ptr->au += value;

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}

		/* Pick up objects */
		else
		{
			/* Hack - some objects were handled in auto_pickup_items(). */
			if (o_ptr->marked & OM_NOMSG)
			{
				/* Clear the flag. */
				o_ptr->marked &= ~OM_NOMSG;
			}
			/* Describe the object */
			else if (!pickup)

			{
#ifdef JP
				msg_format("%s�����롣", o_name);
#else
				msg_format("You see %s.", o_name);
#endif

			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(o_ptr))
			{
#ifdef JP
				msg_format("���å��ˤ�%s��������֤��ʤ���", o_name);
#else
				msg_format("You have no room for %s.", o_name);
#endif

			}

			/* Pick up the item (if requested and allowed) */
			else
			{
				int okay = TRUE;

				/* Hack -- query every item */
				if (carry_query_flag)
				{
					char out_val[MAX_NLEN+20];
#ifdef JP
					sprintf(out_val, "%s�򽦤��ޤ���? ", o_name);
#else
					sprintf(out_val, "Pick up %s? ", o_name);
#endif

					okay = get_check(out_val);
				}

				/* Attempt to pick up an object. */
				if (okay)
				{
					/* Pick up the object */
					py_pickup_aux(this_o_idx);
				}
			}
		}
	}
}


/*
 * Determine if a trap affects the player.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match trap power against player armor.
 */
static int check_hit(int power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if (k < 10) return (k < 5);

	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		if (one_in_(20)) return (TRUE);

	/* Paranoia -- No power */
	if (power <= 0) return (FALSE);

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Power competes against Armor */
	if (randint1(power) > ((ac * 3) / 4)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Handle player hitting a real trap
 */
static void hit_trap(bool break_trap)
{
	int i, num, dam;
	int x = px, y = py;

	cave_type *c_ptr;

#ifdef JP
	cptr		name = "�ȥ�å�";
#else
	cptr name = "a trap";
#endif



	/* Disturb the player */
	disturb(0, 0);

	/* Get the cave grid */
	c_ptr = &cave[y][x];

	/* Analyze XXX XXX XXX */
	switch (c_ptr->feat)
	{
		case FEAT_TRAP_TRAPDOOR:
		{
			if (p_ptr->ffall)
			{
#ifdef JP
				msg_print("��ͤ����ӱۤ�����");
#else
				msg_print("You fly over a trap door.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("��ͤ��������");
				if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_print("���ä�����");
#else
				msg_print("You have fallen through a trap door!");
#endif

				sound(SOUND_FALL);
				dam = damroll(2, 8);
#ifdef JP
				name = "���";
#else
				name = "a trap door";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (p_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_STAIR, 1, "��ͤ������");
#else
				do_cmd_write_nikki(NIKKI_STAIR, 1, "You have fallen through a trap door!");
#endif
				dun_level++;

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
			break;
		}

		case FEAT_TRAP_PIT:
		{
			if (p_ptr->ffall)
			{
#ifdef JP
				msg_print("�������ӱۤ�����");
#else
				msg_print("You fly over a pit trap.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("��������Ƥ��ޤä���");
#else
				msg_print("You have fallen into a pit!");
#endif

				dam = damroll(2, 6);
#ifdef JP
				name = "���";
#else
				name = "a pit trap";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case FEAT_TRAP_SPIKED_PIT:
		{
			if (p_ptr->ffall)
			{
#ifdef JP
				msg_print("�ȥ��Τ����������ӱۤ�����");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("���ѥ������ߤ��줿��������Ƥ��ޤä���");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
#ifdef JP
				name = "���";
#else
				name = "a pit trap";
#endif

				dam = damroll(2, 6);

				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("���ѥ������ɤ��ä���");
#else
					msg_print("You are impaled!");
#endif


#ifdef JP
					name = "�ȥ��Τ������";
#else
					name = "a spiked pit";
#endif

					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam));
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case FEAT_TRAP_POISON_PIT:
		{
			if (p_ptr->ffall)
			{
#ifdef JP
				msg_print("�ȥ��Τ����������ӱۤ�����");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("���ѥ������ߤ��줿��������Ƥ��ޤä���");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
				dam = damroll(2, 6);

#ifdef JP
				name = "���";
#else
				name = "a pit trap";
#endif


				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("�Ǥ��ɤ�줿���ѥ������ɤ��ä���");
#else
					msg_print("You are impaled on poisonous spikes!");
#endif


#ifdef JP
					name = "�ȥ��Τ������";
#else
					name = "a spiked pit";
#endif


					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam));

					if (p_ptr->resist_pois || p_ptr->oppose_pois)
					{
#ifdef JP
						msg_print("�������Ǥαƶ��Ϥʤ��ä���");
#else
						msg_print("The poison does not affect you!");
#endif

					}

					else
					{
						dam = dam * 2;
						(void)set_poisoned(p_ptr->poisoned + randint1(dam));
					}
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}

			break;
		}

		case FEAT_TRAP_TY_CURSE:
		{
#ifdef JP
			msg_print("�������ԥ��äȸ��ä���");
#else
			msg_print("There is a flash of shimmering light!");
#endif

			c_ptr->info &= ~(CAVE_MARK);
			cave_set_feat(y, x, floor_type[randint0(100)]);
			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (one_in_(6));
			}
			break;
		}

		case FEAT_TRAP_TELEPORT:
		{
#ifdef JP
			msg_print("�ƥ�ݡ��ȡ��ȥ�åפˤҤä����ä���");
#else
			msg_print("You hit a teleport trap!");
#endif

			teleport_player(100);
			break;
		}

		case FEAT_TRAP_FIRE:
		{
#ifdef JP
			msg_print("�����ޤ줿��");
#else
			msg_print("You are enveloped in flames!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)fire_dam(dam, "��Υȥ�å�", -1);
#else
			(void)fire_dam(dam, "a fire trap", -1);
#endif

			break;
		}

		case FEAT_TRAP_ACID:
		{
#ifdef JP
			msg_print("�����᤭������줿��");
#else
			msg_print("You are splashed with acid!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)acid_dam(dam, "���Υȥ�å�", -1);
#else
			(void)acid_dam(dam, "an acid trap", -1);
#endif

			break;
		}

		case FEAT_TRAP_SLOW:
		{
			if (check_hit(125))
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ��ƻɤ��ä���");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
				take_hit(DAMAGE_ATTACK, dam, name, -1);
				(void)set_slow(p_ptr->slow + randint0(20) + 20, FALSE);
			}
			else
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ������������ɤ�������ʤ��ä���");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case FEAT_TRAP_LOSE_STR:
		{
			if (check_hit(125))
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ��ƻɤ��ä���");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�����Ĥ��", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				(void)do_dec_stat(A_STR);
			}
			else
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ������������ɤ�������ʤ��ä���");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case FEAT_TRAP_LOSE_DEX:
		{
			if (check_hit(125))
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ��ƻɤ��ä���");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�����Ĥ��", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				(void)do_dec_stat(A_DEX);
			}
			else
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ������������ɤ�������ʤ��ä���");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case FEAT_TRAP_LOSE_CON:
		{
			if (check_hit(125))
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ��ƻɤ��ä���");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "�����Ĥ��", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				(void)do_dec_stat(A_CON);
			}
			else
			{
#ifdef JP
				msg_print("�����ʥ����Ĥ�����Ǥ������������ɤ�������ʤ��ä���");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case FEAT_TRAP_BLIND:
		{
#ifdef JP
			msg_print("������������߹��ޤ줿��");
#else
			msg_print("A black gas surrounds you!");
#endif

			if (!p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + randint0(50) + 25);
			}
			break;
		}

		case FEAT_TRAP_CONFUSE:
		{
#ifdef JP
			msg_print("����᤯��������߹��ޤ줿��");
#else
			msg_print("A gas of scintillating colors surrounds you!");
#endif

			if (!p_ptr->resist_conf)
			{
				(void)set_confused(p_ptr->confused + randint0(20) + 10);
			}
			break;
		}

		case FEAT_TRAP_POISON:
		{
#ifdef JP
			msg_print("�ɷ�Ū���п��Υ�������߹��ޤ줿��");
#else
			msg_print("A pungent green gas surrounds you!");
#endif

			if (!p_ptr->resist_pois && !p_ptr->oppose_pois)
			{
				(void)set_poisoned(p_ptr->poisoned + randint0(20) + 10);
			}
			break;
		}

		case FEAT_TRAP_SLEEP:
		{
#ifdef JP
			msg_print("��̯����̸����ޤ줿��");
#else
			msg_print("A strange white mist surrounds you!");
#endif

			if (!p_ptr->free_act)
			{
#ifdef JP
msg_print("���ʤ���̲��˽�������");
#else
				msg_print("You fall asleep.");
#endif


				if (ironman_nightmare)
				{
#ifdef JP
msg_print("�Ȥ��Ӥ����ĸ��ʤ�Ƭ���⤫�����");
#else
					msg_print("A horrible vision enters your mind.");
#endif


					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					have_nightmare(get_mon_num(MAX_DEPTH));

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);
				}
				(void)set_paralyzed(p_ptr->paralyzed + randint0(10) + 5);
			}
			break;
		}

		case FEAT_TRAP_TRAPS:
		{
#ifdef JP
msg_print("�ޤФ椤���������ä���");
#else
			msg_print("There is a bright flash of light!");
#endif


			/* Destroy this trap */
			cave_set_feat(y, x, floor_type[randint0(100)]);

			/* Make some new traps */
			project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

			break;
		}

		case FEAT_TRAP_ALARM:
		{
#ifdef JP
			msg_print("�������ޤ��������Ĥ��������");
#else
			msg_print("An alarm sounds!");
#endif

			aggravate_monsters(0);

			break;
		}

		case FEAT_TRAP_OPEN:
		{
#ifdef JP
			msg_print("�粻���ȶ��ˤޤ����ɤ����줿��");
#else
			msg_print("Suddenly, surrounding walls are opened!");
#endif
			(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_monsters(0);

			break;
		}

		case FEAT_TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};

			int lev;
#ifdef JP
			msg_print("����ŷ��������˴������ޤ줿��");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif

			/* Destroy this trap */
			cave_set_feat(y, x, floor_type[randint0(100)]);

			/* Summon Demons and Angels */
			for (lev = dun_level; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(x, 7);
					int y1 = rand_spread(y, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of sight */
					if (!player_has_los_bold(y1, x1)) continue;

					(void)summon_specific(0, y1, x1, lev, SUMMON_DEMON, (PM_NO_PET));
					(void)summon_specific(0, y1, x1, lev, SUMMON_ANGEL, (PM_NO_PET));
				}
			}
			break;
		}

		case FEAT_TRAP_PIRANHA:
		{
#ifdef JP
			msg_print("�����ɤ���夬���Ф������ԥ�˥������롪");
#else
			msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

			/* Destroy this trap */
			cave_set_feat(y, x, floor_type[randint0(100)]);

			/* Water fills room */
			fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

			/* Summon Piranhas */
			num = 1 + dun_level/20;
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
			}
			break;
		}
	}
	if (break_trap && is_trap(c_ptr->feat))
	{
		cave_set_feat(y, x, floor_type[randint0(100)]);
#ifdef JP
		msg_print("�ȥ�åפ�ʴ�դ�����");
#else
		msg_print("You destroyed the trap.");
#endif
	}
}


static void touch_zap_player(monster_type *m_ptr)
{
	int aura_damage = 0;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (r_ptr->flags2 & RF2_AURA_FIRE)
	{
		if (!p_ptr->immune_fire)
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, 0x288);

#ifdef JP
			msg_print("�����ȤƤ�Ǯ���ʤä���");
#else
			msg_print("You are suddenly very hot!");
#endif


			if (prace_is_(RACE_ENT)) aura_damage += aura_damage / 3;
			if (p_ptr->oppose_fire) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_fire) aura_damage = (aura_damage + 2) / 3;

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			r_ptr->r_flags2 |= RF2_AURA_FIRE;
			handle_stuff();
		}
	}

	if (r_ptr->flags3 & RF3_AURA_COLD)
	{
		if (!p_ptr->immune_cold)
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, 0x288);

#ifdef JP
			msg_print("�����ȤƤ⴨���ʤä���");
#else
			msg_print("You are suddenly very cold!");
#endif


			if (p_ptr->oppose_cold) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_cold) aura_damage = (aura_damage + 2) / 3;

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			r_ptr->r_flags3 |= RF3_AURA_COLD;
			handle_stuff();
		}
	}

	if (r_ptr->flags2 & RF2_AURA_ELEC)
	{
		if (!p_ptr->immune_elec)
		{
			char aura_dam[80];

			aura_damage = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));

			/* Hack -- Get the "died from" name */
			monster_desc(aura_dam, m_ptr, 0x288);

			if (prace_is_(RACE_ANDROID)) aura_damage += aura_damage / 3;
			if (p_ptr->oppose_elec) aura_damage = (aura_damage + 2) / 3;
			if (p_ptr->resist_elec) aura_damage = (aura_damage + 2) / 3;

#ifdef JP
			msg_print("�ŷ�򤯤�ä���");
#else
			msg_print("You get zapped!");
#endif

			take_hit(DAMAGE_NOESCAPE, aura_damage, aura_dam, -1);
			r_ptr->r_flags2 |= RF2_AURA_ELEC;
			handle_stuff();
		}
	}
}


static void natural_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	int             k, bonus, chance;
	int             n_weight = 0;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	int             dss, ddd;

	cptr            atk_desc;

	switch (attack)
	{
		case MUT2_SCOR_TAIL:
			dss = 3;
			ddd = 7;
			n_weight = 5;
#ifdef JP
			atk_desc = "����";
#else
			atk_desc = "tail";
#endif

			break;
		case MUT2_HORNS:
			dss = 2;
			ddd = 6;
			n_weight = 15;
#ifdef JP
			atk_desc = "��";
#else
			atk_desc = "horns";
#endif

			break;
		case MUT2_BEAK:
			dss = 2;
			ddd = 4;
			n_weight = 5;
#ifdef JP
			atk_desc = "�����Х�";
#else
			atk_desc = "beak";
#endif

			break;
		case MUT2_TRUNK:
			dss = 1;
			ddd = 4;
			n_weight = 35;
#ifdef JP
			atk_desc = "�ݤ�ɡ";
#else
			atk_desc = "trunk";
#endif

			break;
		case MUT2_TENTACLES:
			dss = 2;
			ddd = 5;
			n_weight = 5;
#ifdef JP
			atk_desc = "����";
#else
			atk_desc = "tentacles";
#endif

			break;
		default:
			dss = ddd = n_weight = 1;
#ifdef JP
			atk_desc = "̤���������";
#else
			atk_desc = "undefined body part";
#endif

	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);


	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h_m;
	bonus += (p_ptr->lev * 6 / 5);
	chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));

	/* Test for hit */
	if ((!(r_ptr->flags2 & RF2_QUANTUM) || !randint0(2)) && test_hit_norm(chance, r_ptr->ac, m_ptr->ml))
	{
		/* Sound */
		sound(SOUND_HIT);

#ifdef JP
		msg_format("%s��%s�ǹ��⤷����", m_name, atk_desc);
#else
		msg_format("You hit %s with your %s.", m_name, atk_desc);
#endif


		k = damroll(ddd, dss);
		k = critical_norm(n_weight, bonus, k, (s16b)bonus, 0);

		/* Apply the player damage bonuses */
		k += p_ptr->to_d_m;

		/* No negative damage */
		if (k < 0) k = 0;

		/* Modify the damage */
		k = mon_damage_mod(m_ptr, k, FALSE);

		/* Complex message */
		if (p_ptr->wizard)
		{
#ifdef JP
				msg_format("%d/%d �Υ��᡼����Ϳ������", k, m_ptr->hp);
#else
			msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif

		}

		/* Anger the monster */
		if (k > 0) anger_monster(m_ptr);

		/* Damage, check for fear and mdeath */
		switch (attack)
		{
			case MUT2_SCOR_TAIL:
				project(0, 0, m_ptr->fy, m_ptr->fx, k, GF_POIS, PROJECT_KILL, -1);
				*mdeath = (m_ptr->r_idx == 0);
				break;
			case MUT2_HORNS:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_BEAK:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_TRUNK:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT2_TENTACLES:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			default:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
		}

		touch_zap_player(m_ptr);
	}
	/* Player misses */
	else
	{
		/* Sound */
		sound(SOUND_MISS);

		/* Message */
#ifdef JP
			msg_format("�ߥ��� %s�ˤ��蘆�줿��", m_name);
#else
		msg_format("You miss %s.", m_name);
#endif

	}
}



/*
 * Player attacks a (poor, defenseless) creature        -RAK-
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */
static void py_attack_aux(int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	int		num = 0, k, bonus, chance, vir;

	cave_type       *c_ptr = &cave[y][x];

	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];

	object_type     *o_ptr;

	char            m_name[80];

	bool            success_hit = FALSE;
	bool            old_success_hit = FALSE;
	bool            backstab = FALSE;
	bool            vorpal_cut = FALSE;
	int             chaos_effect = 0;
	bool            stab_fleeing = FALSE;
	bool            fuiuchi = FALSE;
	bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	int             num_blow;
	int             drain_left = MAX_VAMPIRIC_DRAIN;
	u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */
	bool            is_human = (r_ptr->d_char == 'p');
	bool            is_lowlevel = (r_ptr->level < (p_ptr->lev - 15));
	bool            zantetsu_mukou, e_j_mukou;



	if (((p_ptr->pclass == CLASS_ROGUE) || (p_ptr->pclass == CLASS_NINJA)) && inventory[INVEN_RARM+hand].tval)
	{
		int tmp = p_ptr->lev*6+(p_ptr->skill_stl+10)*4;
		if (p_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
		if (p_ptr->cursed & TRC_AGGRAVATE) tmp /= 2;
		if (r_ptr->level > (p_ptr->lev*p_ptr->lev/20+10)) tmp /= 3;
		if (m_ptr->csleep && m_ptr->ml)
		{
			/* Can't backstab creatures that we can't see, right? */
			backstab = TRUE;
		}
		else if ((p_ptr->special_defense & NINJA_S_STEALTH) && (randint0(tmp) > (r_ptr->level+20)) && m_ptr->ml && !(r_ptr->flags3 & RF3_RES_ALL))
		{
			fuiuchi = TRUE;
		}
		else if (m_ptr->monfear && m_ptr->ml)
		{
			stab_fleeing = TRUE;
		}
	}

	if(!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
	{
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			if (p_ptr->skill_exp[GINOU_SUDE] < s_info[p_ptr->pclass].s_max[GINOU_SUDE])
			{
				if (p_ptr->skill_exp[GINOU_SUDE] < 4000)
					p_ptr->skill_exp[GINOU_SUDE]+=40;
				else if((p_ptr->skill_exp[GINOU_SUDE] < 6000))
					p_ptr->skill_exp[GINOU_SUDE]+=5;
				else if((p_ptr->skill_exp[GINOU_SUDE] < 7000) && (p_ptr->lev > 19))
					p_ptr->skill_exp[GINOU_SUDE]+=1;
				else if((p_ptr->skill_exp[GINOU_SUDE] < 8000) && (p_ptr->lev > 34))
					if (one_in_(3)) p_ptr->skill_exp[GINOU_SUDE]+=1;
				p_ptr->update |= (PU_BONUS);
			}
		}
	}
	else
	{
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			int tval = inventory[INVEN_RARM+hand].tval - TV_BOW;
			int sval = inventory[INVEN_RARM+hand].sval;
			int now_exp = p_ptr->weapon_exp[tval][sval];
			if (now_exp < s_info[p_ptr->pclass].w_max[tval][sval])
			{
				int amount = 0;
				if (now_exp < 4000) amount = 80;
				else if(now_exp < 6000) amount = 10;
				else if((now_exp < 7000) && (p_ptr->lev > 19)) amount = 1;
				else if((p_ptr->lev > 34) && one_in_(2)) amount = 1;
				p_ptr->weapon_exp[tval][sval] += amount;
				p_ptr->update |= (PU_BONUS);
			}
		}
	}

	/* Disturb the monster */
	m_ptr->csleep = 0;
	if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2))
		p_ptr->update |= (PU_MON_LITE);

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Access the weapon */
	o_ptr = &inventory[INVEN_RARM+hand];

	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h[hand] + o_ptr->to_h;
	chance = (p_ptr->skill_thn + (bonus * BTH_PLUS_ADJ));
	if (mode == HISSATSU_IAI) chance += 60;
	if (p_ptr->special_defense & KATA_KOUKIJIN) chance += 150;

	if (p_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);

	vir = virtue_number(V_VALOUR);
	if (vir)
	{
		chance += (p_ptr->virtues[vir - 1]/10);
	}

	zantetsu_mukou = ((o_ptr->name1 == ART_ZANTETSU) && (r_ptr->d_char == 'j'));
	e_j_mukou = ((o_ptr->name1 == ART_EXCALIBUR_J) && (r_ptr->d_char == 'S'));

	if ((mode == HISSATSU_KYUSHO) || (mode == HISSATSU_MINEUCHI) || (mode == HISSATSU_3DAN) || (mode == HISSATSU_IAI)) num_blow = 1;
	else if (mode == HISSATSU_COLD) num_blow = p_ptr->num_blow[hand]+2;
	else num_blow = p_ptr->num_blow[hand];

	/* Attack once for each legal blow */
	while ((num++ < num_blow) && !p_ptr->is_dead)
	{
		if (((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
		{
			if (p_ptr->migite && p_ptr->hidarite)
			{
				success_hit = one_in_(2);
			}
			else success_hit = TRUE;
		}
		else if (mode == HISSATSU_MAJIN)
		{
			if (num == 1)
			{
				if (one_in_(2))
					success_hit = FALSE;
				old_success_hit = success_hit;
			}
			else success_hit = old_success_hit;
		}
		else if ((p_ptr->pclass == CLASS_NINJA) && ((backstab || fuiuchi) && !(r_ptr->flags3 & RF3_RES_ALL))) success_hit = TRUE;
		else success_hit = test_hit_norm(chance, r_ptr->ac, m_ptr->ml);

		/* Test for hit */
		if (success_hit)
		{
			int vorpal_chance = ((o_ptr->name1 == ART_VORPAL_BLADE) || (o_ptr->name1 == ART_CHAINSWORD)) ? 2 : 4;

			/* Sound */
			sound(SOUND_HIT);

			/* Message */
			if (backstab)
#ifdef JP
				msg_format("���ʤ������ˤ�̲�äƤ���̵�Ϥ�%s���ͤ��ɤ�����",
#else
				msg_format("You cruelly stab the helpless, sleeping %s!",
#endif

				    m_name);
			else if (fuiuchi)
#ifdef JP
				msg_format("�԰դ��ͤ���%s�˶����ʰ������碌����",
#else
				msg_format("You make surprise attack, and hit %s with a powerful blow!",
#endif

				    m_name);
			else if (stab_fleeing)
#ifdef JP
				msg_format("ƨ����%s�����椫���ͤ��ɤ�����",
#else
				msg_format("You backstab the fleeing %s!",
#endif

				    m_name);
			else
			{
				if (!(((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(TRUE) > 1)))
#ifdef JP
			msg_format("%s�򹶷⤷����", m_name);
#else
					msg_format("You hit %s.", m_name);
#endif

			}

			/* Hack -- bare hands do one damage */
			k = 1;

			object_flags(o_ptr, flgs);

			/* Select a chaotic effect (50% chance) */
			if ((have_flag(flgs, TR_CHAOTIC)) && one_in_(2))
			{
				if (one_in_(10))
				chg_virtue(V_CHANCE, 1);

				if (randint1(5) < 3)
				{
					/* Vampiric (20%) */
					chaos_effect = 1;
				}
				else if (one_in_(250))
				{
					/* Quake (0.12%) */
					chaos_effect = 2;
				}
				else if (!one_in_(10))
				{
					/* Confusion (26.892%) */
					chaos_effect = 3;
				}
				else if (one_in_(2))
				{
					/* Teleport away (1.494%) */
					chaos_effect = 4;
				}
				else
				{
					/* Polymorph (1.494%) */
					chaos_effect = 5;
				}
			}

			/* Vampiric drain */
			if ((have_flag(flgs, TR_VAMPIRIC)) || (chaos_effect == 1) || (mode == HISSATSU_DRAIN))
			{
				/* Only drain "living" monsters */
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;
			}

			if ((have_flag(flgs, TR_VORPAL)) && (randint1(vorpal_chance*3/2) == 1) && !zantetsu_mukou)
				vorpal_cut = TRUE;
			else vorpal_cut = FALSE;

			if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(TRUE) > 1))
			{
				int special_effect = 0, stun_effect = 0, times = 0, max_times;
				int min_level = 1;
				martial_arts *ma_ptr = &ma_blows[0], *old_ptr = &ma_blows[0];
				int resist_stun = 0;
				int weight = 8;

				if (r_ptr->flags1 & RF1_UNIQUE) resist_stun += 88;
				if (r_ptr->flags3 & RF3_NO_STUN) resist_stun += 66;
				if (r_ptr->flags3 & RF3_NO_CONF) resist_stun += 33;
				if (r_ptr->flags3 & RF3_NO_SLEEP) resist_stun += 33;
				if ((r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags3 & RF3_NONLIVING))
					resist_stun += 66;

				if (p_ptr->special_defense & KAMAE_BYAKKO)
					max_times = (p_ptr->lev < 3 ? 1 : p_ptr->lev / 3);
				else if (p_ptr->special_defense & KAMAE_SUZAKU)
					max_times = 1;
				else if (p_ptr->special_defense & KAMAE_GENBU)
					max_times = 1;
				else
					max_times = (p_ptr->lev < 7 ? 1 : p_ptr->lev / 7);
				/* Attempt 'times' */
				for (times = 0; times < max_times; times++)
				{
					do
					{
						ma_ptr = &ma_blows[randint0(MAX_MA)];
						if ((p_ptr->pclass == CLASS_FORCETRAINER) && (ma_ptr->min_level > 1)) min_level = ma_ptr->min_level + 3;
						else min_level = ma_ptr->min_level;
					}
					while ((min_level > p_ptr->lev) ||
					       (randint1(p_ptr->lev) < ma_ptr->chance));

					/* keep the highest level attack available we found */
					if ((ma_ptr->min_level > old_ptr->min_level) &&
					    !p_ptr->stun && !p_ptr->confused)
					{
						old_ptr = ma_ptr;

						if (p_ptr->wizard && cheat_xtra)
						{
#ifdef JP
							msg_print("���������򤷤ޤ�����");
#else
							msg_print("Attack re-selected.");
#endif

						}
					}
					else
					{
						ma_ptr = old_ptr;
					}
				}

				if (p_ptr->pclass == CLASS_FORCETRAINER) min_level = MAX(1, ma_ptr->min_level - 3);
				else min_level = ma_ptr->min_level;
				k = damroll(ma_ptr->dd, ma_ptr->ds);
				if (p_ptr->special_attack & ATTACK_SUIKEN) k *= 2;

				if (ma_ptr->effect == MA_KNEE)
				{
					if (r_ptr->flags1 & RF1_MALE)
					{
#ifdef JP
						msg_format("%s�˶�Ūɨ����򤯤�路����", m_name);
#else
						msg_format("You hit %s in the groin with your knee!", m_name);
#endif

						sound(SOUND_PAIN);
						special_effect = MA_KNEE;
					}
					else
						msg_format(ma_ptr->desc, m_name);
				}

				else if (ma_ptr->effect == MA_SLOW)
				{
					if (!((r_ptr->flags1 & RF1_NEVER_MOVE) ||
					    strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", r_ptr->d_char)))
					{
#ifdef JP
						msg_format("%s��­��˴��έ��򤯤�路����", m_name);
#else
						msg_format("You kick %s in the ankle.", m_name);
#endif

						special_effect = MA_SLOW;
					}
					else msg_format(ma_ptr->desc, m_name);
				}
				else
				{
					if (ma_ptr->effect)
					{
						stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
					}

					msg_format(ma_ptr->desc, m_name);
				}

				if (p_ptr->special_defense & KAMAE_SUZAKU) weight = 4;
				if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
				{
					weight += (p_ptr->magic_num1[0]/30);
					if (weight > 20) weight = 20;
				}

				k = critical_norm(p_ptr->lev * weight, min_level, k, p_ptr->to_h[0], 0);

				if ((special_effect == MA_KNEE) && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
#ifdef JP
					msg_format("%^s�϶��ˤˤ��ᤤ�Ƥ��롪", m_name);
#else
					msg_format("%^s moans in agony!", m_name);
#endif

					stun_effect = 7 + randint1(13);
					resist_stun /= 3;
				}

				else if ((special_effect == MA_SLOW) && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
					if (!(r_ptr->flags1 & RF1_UNIQUE) &&
					    (randint1(p_ptr->lev) > r_ptr->level) &&
					    m_ptr->mspeed > 60)
					{
#ifdef JP
						msg_format("%^s��­��Ҥ�����Ϥ᤿��", m_name);
#else
						msg_format("%^s starts limping slower.", m_name);
#endif

						m_ptr->mspeed -= 10;
					}
				}

				if (stun_effect && ((k + p_ptr->to_d[hand]) < m_ptr->hp))
				{
					if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
					{
						if (m_ptr->stunned)
#ifdef JP
							msg_format("%^s�Ϥ���˥ե�ե�ˤʤä���", m_name);
#else
							msg_format("%^s is more stunned.", m_name);
#endif

						else
#ifdef JP
							msg_format("%^s�ϥե�ե�ˤʤä���", m_name);
#else
							msg_format("%^s is stunned.", m_name);
#endif


						m_ptr->stunned += stun_effect;
					}
				}
			}

			/* Handle normal weapon */
			else if (o_ptr->k_idx)
			{
				k = damroll(o_ptr->dd, o_ptr->ds);
				if (p_ptr->riding)
				{
					if((o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
					{
						k += damroll(2, o_ptr->ds);
					}
				}

				k = tot_dam_aux(o_ptr, k, m_ptr, mode);

				if (backstab)
				{
					k *= (3 + (p_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
					k = k*(5+(p_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}

				if ((p_ptr->impact[hand] && ((k > 50) || one_in_(7))) ||
					 (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				{
					do_quake = TRUE;
				}

				if ((!(o_ptr->tval == TV_SWORD) || !(o_ptr->sval == SV_DOKUBARI)) && !(mode == HISSATSU_KYUSHO))
					k = critical_norm(o_ptr->weight, o_ptr->to_h, k, p_ptr->to_h[hand], mode);

				drain_result = k;

				if (vorpal_cut)
				{
					int mult = 2;

					if ((o_ptr->name1 == ART_CHAINSWORD) && !one_in_(2))
					{
						char chainsword_noise[1024];
#ifdef JP
       if (!get_rnd_line("chainswd_j.txt", 0, chainsword_noise))
#else
						if (!get_rnd_line("chainswd.txt", 0, chainsword_noise))
#endif

						{
							msg_print(chainsword_noise);
						}
					}

					if (o_ptr->name1 == ART_VORPAL_BLADE)
					{
#ifdef JP
						msg_print("�ܤˤ�ߤޤ�̥������ѥ�֥졼�ɡ���ϣ����ȡ�");
#else
						msg_print("Your Vorpal Blade goes snicker-snack!");
#endif

					}
					else
					{
#ifdef JP
						msg_format("%s�򥰥å����ڤ���������", m_name);
#else
						msg_format("Your weapon cuts deep into %s!", m_name);
#endif

					}

					/* Try to increase the damage */
					while (one_in_(vorpal_chance))
					{
						mult++;
					}

					k *= mult;

					/* Ouch! */
					if (((r_ptr->flags3 & RF3_RES_ALL) ? k/100 : k) > m_ptr->hp)
					{
#ifdef JP
						msg_format("%s�򿿤���Ĥˤ�����", m_name);
#else
						msg_format("You cut %s in half!", m_name);
#endif

					}
					else
					{
						switch(mult)
						{
#ifdef JP
case 2:	msg_format("%s��¤ä���", m_name);		break;
#else
							case 2:	msg_format("You gouge %s!", m_name);		break;
#endif

#ifdef JP
case 3:	msg_format("%s��֤ä��¤ä���", m_name);			break;
#else
							case 3:	msg_format("You maim %s!", m_name);			break;
#endif

#ifdef JP
case 4:	msg_format("%s���å��¤�ˤ�����", m_name);		break;
#else
							case 4:	msg_format("You carve %s!", m_name);		break;
#endif

#ifdef JP
case 5:	msg_format("%s���å��᥿�˻¤ä���", m_name);		break;
#else
							case 5:	msg_format("You cleave %s!", m_name);		break;
#endif

#ifdef JP
case 6:	msg_format("%s��ɿȤˤ�����", m_name);		break;
#else
							case 6:	msg_format("You smite %s!", m_name);		break;
#endif

#ifdef JP
case 7:	msg_format("%s��¤äƻ¤äƻ¤�ޤ��ä���", m_name);	break;
#else
							case 7:	msg_format("You eviscerate %s!", m_name);	break;
#endif

#ifdef JP
default:	msg_format("%s����ڤ�ˤ�����", m_name);		break;
#else
							default:	msg_format("You shred %s!", m_name);		break;
#endif

						}
					}
					drain_result = drain_result * 3 / 2;
				}

				k += o_ptr->to_d;
				drain_result += o_ptr->to_d;
			}

			/* Apply the player damage bonuses */
			k += p_ptr->to_d[hand];
			drain_result += p_ptr->to_d[hand];

			if ((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 2;
			if ((mode == HISSATSU_SEKIRYUKA) && (r_ptr->flags3 & (RF3_UNDEAD | RF3_DEMON | RF3_NONLIVING))) k = 0;
			if ((mode == HISSATSU_SEKIRYUKA) && !p_ptr->cut) k /= 2;

			/* No negative damage */
			if (k < 0) k = 0;

			if ((mode == HISSATSU_ZANMA) && !((r_ptr->flags3 & (RF3_DEMON | RF3_UNDEAD | RF3_NONLIVING)) && (r_ptr->flags3 & RF3_EVIL)))
			{
				k = 0;
			}

			if (zantetsu_mukou)
			{
#ifdef JP
				msg_print("�������餫����Τ��ڤ��");
#else
				msg_print("You cannot cut such a elastic thing!");
#endif
				k = 0;
			}

			if (e_j_mukou)
			{
#ifdef JP
				msg_print("����϶�����");
#else
				msg_print("Spiders are difficult for you to deal with!");
#endif
				k /= 2;
			}

			if (mode == HISSATSU_MINEUCHI)
			{
				int tmp = (10 + randint1(15) + p_ptr->lev / 5);

				k = 0;
				anger_monster(m_ptr);

				if (!(r_ptr->flags3 & (RF3_NO_STUN)))
				{
					/* Get stunned */
					if (m_ptr->stunned)
					{
#ifdef JP
msg_format("%s�ϤҤɤ��⤦���Ȥ�����", m_name);
#else
						msg_format("%s is more dazed.", m_name);
#endif

						tmp /= 2;
					}
					else
					{
#ifdef JP
msg_format("%s �Ϥ⤦���Ȥ�����", m_name);
#else
						msg_format("%s is dazed.", m_name);
#endif
					}

					/* Apply stun */
					m_ptr->stunned = (tmp < 200) ? tmp : 200;
				}
				else
				{
#ifdef JP
msg_format("%s �ˤϸ��̤��ʤ��ä���", m_name);
#else
						msg_format("%s is not effected.", m_name);
#endif
				}
			}

			/* Modify the damage */
			k = mon_damage_mod(m_ptr, k, (bool)(((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE)) || ((p_ptr->pclass == CLASS_BERSERKER) && one_in_(2))));
			if (((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
			{
				if ((randint1(randint1(r_ptr->level/7)+5) == 1) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2))
				{
					k = m_ptr->hp + 1;
#ifdef JP
msg_format("%s�ε޽���ͤ��ɤ�����", m_name);
#else
					msg_format("You hit %s on a fatal spot!", m_name);
#endif
				}
				else k = 1;
			}
			else if ((p_ptr->pclass == CLASS_NINJA) && (!p_ptr->icky_wield[hand]) && ((p_ptr->cur_lite <= 0) || one_in_(7)))
			{
				int maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
				if (one_in_(backstab ? 13 : (stab_fleeing || fuiuchi) ? 15 : 27))
				{
					k *= 5;
					drain_result *= 2;
#ifdef JP
msg_format("�Ϥ�%s�˿������ͤ��ɤ��ä���", m_name);
#else
					msg_format("You critically injured %s!", m_name);
#endif
				}
				else if (((m_ptr->hp < maxhp/2) && one_in_((p_ptr->num_blow[0]+p_ptr->num_blow[1]+1)*10)) || ((one_in_(666) || ((backstab || fuiuchi) && one_in_(11))) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2)))
				{
					if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (m_ptr->hp >= maxhp/2))
					{
						k = MAX(k*5, m_ptr->hp/2);
						drain_result *= 2;
#ifdef JP
msg_format("%s����̿������碌����", m_name);
#else
					msg_format("You fatally injured %s!", m_name);
#endif
					}
					else
					{
						k = m_ptr->hp + 1;
#ifdef JP
msg_format("�Ϥ�%s�ε޽��Ӥ�����", m_name);
#else
					msg_format("You hit %s on a fatal spot!", m_name);
#endif
					}
				}
			}

			/* Complex message */
			if (p_ptr->wizard || cheat_xtra)
			{
#ifdef JP
				msg_format("%d/%d �Υ��᡼����Ϳ������", k, m_ptr->hp);
#else
				msg_format("You do %d (out of %d) damage.", k, m_ptr->hp);
#endif

			}

			if (k <= 0) can_drain = FALSE;

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;

			/* Damage, check for fear and death */
			if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;
				if ((p_ptr->pclass == CLASS_BERSERKER) && energy_use)
				{
					if (p_ptr->migite && p_ptr->hidarite)
					{
						if (hand) energy_use = energy_use*3/5+energy_use*num*2/(p_ptr->num_blow[hand]*5);
						else energy_use = energy_use*num*3/(p_ptr->num_blow[hand]*5);
					}
					else
					{
						energy_use = energy_use*num/p_ptr->num_blow[hand];
					}
				}
				if ((o_ptr->name1 == ART_ZANTETSU) && is_lowlevel)
#ifdef JP
					msg_print("�ޤ��Ĥޤ�̤�Τ�¤äƤ��ޤä�������");
#else
					msg_print("Sign..Another trifling thing I've cut....");
#endif
				break;
			}

			/* Anger the monster */
			if (k > 0) anger_monster(m_ptr);

			touch_zap_player(m_ptr);

			/* Are we draining it?  A little note: If the monster is
			dead, the drain does not work... */

			if (can_drain && (drain_result > 0))
			{
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int i, flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
#ifdef JP
							msg_print("����Ϸ��ۤäƶ����ʤä���");
#else
							msg_print("Muramasa sucked blood, and became more powerful!");
#endif
							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						drain_heal = damroll(2, drain_result / 6);

						if (cheat_xtra)
						{
#ifdef JP
							msg_format("Draining left: %d", drain_left);
#else
							msg_format("Draining left: %d", drain_left);
#endif

						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
#ifdef JP
								msg_format("�Ϥ�%s������̿�Ϥ�ۤ���ä���", m_name);
#else
								msg_format("Your weapon drains life from %s!", m_name);
#endif

								drain_msg = FALSE;
							}

							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player(drain_heal);
							/* We get to keep some of it! */
						}
					}
				}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				weak = TRUE;
			}
			can_drain = FALSE;
			drain_result = 0;

			/* Confusion attack */
			if ((p_ptr->special_attack & ATTACK_CONFUSE) || (chaos_effect == 3) || (mode == HISSATSU_CONF))
			{
				/* Cancel glowing hands */
				if (p_ptr->special_attack & ATTACK_CONFUSE)
				{
					p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("��ε������ʤ��ʤä���");
#else
					msg_print("Your hands stop glowing.");
#endif
					p_ptr->redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (m_ptr->ml)
					{
						r_ptr->r_flags3 |= RF3_NO_CONF;
					}

#ifdef JP
					msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^s�Ϻ��𤷤��褦����", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					m_ptr->confused += 10 + randint0(p_ptr->lev) / 5;
				}
			}

			else if (chaos_effect == 4)
			{
				bool resists_tele = FALSE;

				if (r_ptr->flags3 & RF3_RES_TELE)
				{
					if (r_ptr->flags1 & RF1_UNIQUE)
					{
						if (m_ptr->ml) r_ptr->r_flags3 |= RF3_RES_TELE;
#ifdef JP
						msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
						msg_format("%^s is unaffected!", m_name);
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (m_ptr->ml) r_ptr->r_flags3 |= RF3_RES_TELE;
#ifdef JP
						msg_format("%^s�����Ϥ���äƤ��롪", m_name);
#else
						msg_format("%^s resists!", m_name);
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
#ifdef JP
					msg_format("%^s�Ͼä�����", m_name);
#else
					msg_format("%^s disappears!", m_name);
#endif

					teleport_away(c_ptr->m_idx, 50, FALSE);
					num = num_blow + 1; /* Can't hit it anymore! */
					*mdeath = TRUE;
				}
			}

			else if ((chaos_effect == 5) && cave_floor_bold(y, x) &&
				 (randint1(90) > r_ptr->level))
			{
				if (!(r_ptr->flags1 & RF1_UNIQUE) &&
				    !(r_ptr->flags4 & RF4_BR_CHAO) &&
				    !(r_ptr->flags1 & RF1_QUESTOR))
				{
					if (polymorph_monster(y, x))
					{
#ifdef JP
						msg_format("%^s���Ѳ�������", m_name);
#else
						msg_format("%^s changes!", m_name);
#endif


						/* Hack -- Get new monster */
						m_ptr = &m_list[c_ptr->m_idx];

						/* Oops, we need a different name... */
						monster_desc(m_name, m_ptr, 0);

						/* Hack -- Get new race */
						r_ptr = &r_info[m_ptr->r_idx];

						*fear = FALSE;
						weak = FALSE;
					}
					else
					{
#ifdef JP
					msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
						msg_format("%^s is unaffected.", m_name);
#endif

					}
				}
			}
			else if (o_ptr->name1 == ART_G_HAMMER)
			{
				monster_type *m_ptr = &m_list[c_ptr->m_idx];

				if (m_ptr->hold_o_idx)
				{
					object_type *q_ptr = &o_list[m_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, TRUE, 0);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = 0;
					m_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
#ifdef JP
					msg_format("%s��å�ä���", o_name);
#else
					msg_format("You snatched %s.", o_name);
#endif
					inven_carry(q_ptr);
				}
			}
		}

		/* Player misses */
		else
		{
			backstab = FALSE; /* Clumsy! */
			fuiuchi = FALSE; /* Clumsy! */

			if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
			{
				u32b flgs[TR_FLAG_SIZE];

				/* Sound */
				sound(SOUND_HIT);

				/* Message */
#ifdef JP
				msg_format("�ߥ��� %s�ˤ��蘆�줿��", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
				/* Message */
#ifdef JP
				msg_print("����󤷤��������ʬ���Ȥ��֤äƤ�����");
#else
				msg_print("Your scythe returns to you!");
#endif

				/* Extract the flags */
				object_flags(o_ptr, flgs);

				k = damroll(o_ptr->dd, o_ptr->ds);
				{
					int mult;
					switch (p_ptr->mimic_form)
					{
					case MIMIC_NONE:
						switch (p_ptr->prace)
						{
							case RACE_YEEK:
							case RACE_KLACKON:
							case RACE_HUMAN:
							case RACE_AMBERITE:
							case RACE_DUNADAN:
								mult = 25;break;
							case RACE_HALF_ORC:
							case RACE_HALF_TROLL:
							case RACE_HALF_OGRE:
							case RACE_HALF_GIANT:
							case RACE_HALF_TITAN:
							case RACE_CYCLOPS:
							case RACE_IMP:
							case RACE_SKELETON:
							case RACE_ZOMBIE:
							case RACE_VAMPIRE:
							case RACE_SPECTRE:
							case RACE_DEMON:
							case RACE_DRACONIAN:
								mult = 30;break;
							default:
								mult = 10;break;
						}
						break;
					case MIMIC_DEMON:
					case MIMIC_DEMON_LORD:
					case MIMIC_VAMPIRE:
						mult = 30;break;
					default:
						mult = 10;break;
					}

					if (p_ptr->align < 0 && mult < 20)
						mult = 20;
					if (!(p_ptr->resist_acid || p_ptr->oppose_acid || p_ptr->immune_acid) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_elec || p_ptr->oppose_elec || p_ptr->immune_elec) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_fire || p_ptr->oppose_fire || p_ptr->immune_fire) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_cold || p_ptr->oppose_cold || p_ptr->immune_cold) && (mult < 25))
						mult = 25;
					if (!(p_ptr->resist_pois || p_ptr->oppose_pois) && (mult < 25))
						mult = 25;

					if ((p_ptr->pclass != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (p_ptr->msp / 30)))
					{
						p_ptr->csp -= (1+(p_ptr->msp / 30));
						p_ptr->redraw |= (PR_MANA);
						mult = mult * 3 / 2 + 20;
					}
					k *= mult;
					k /= 10;
				}

				k = critical_norm(o_ptr->weight, o_ptr->to_h, k, p_ptr->to_h[hand], mode);
				if (one_in_(6))
				{
					int mult = 2;
#ifdef JP
					msg_format("���å����ڤ������줿��");
#else
					msg_format("Your weapon cuts deep into yourself!");
#endif
					/* Try to increase the damage */
					while (one_in_(4))
					{
						mult++;
					}

					k *= mult;
				}
				k += (p_ptr->to_d[hand] + o_ptr->to_d);

				if (k < 0) k = 0;

#ifdef JP
				take_hit(DAMAGE_FORCE, k, "������", -1);
#else
				take_hit(DAMAGE_FORCE, k, "Death scythe", -1);
#endif

				redraw_stuff();
			}
			else
			{
				/* Sound */
				sound(SOUND_MISS);

				/* Message */
#ifdef JP
				msg_format("�ߥ��� %s�ˤ��蘆�줿��", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
			}
		}
		backstab = FALSE;
		fuiuchi = FALSE;
	}


	if (weak && !(*mdeath))
	{
#ifdef JP
		msg_format("%s�ϼ夯�ʤä��褦����", m_name);
#else
		msg_format("%^s seems weakened.", m_name);
#endif
	}
	if (drain_left != MAX_VAMPIRIC_DRAIN)
	{
		if (one_in_(4))
		{
			chg_virtue(V_UNLIFE, 1);
		}
	}
	/* Mega-Hack -- apply earthquake brand */
	if (do_quake)
	{
		earthquake(py, px, 10);
		if (!cave[y][x].m_idx) *mdeath = TRUE;
	}
}

bool py_attack(int y, int x, int mode)
{
	bool            fear = FALSE;
	bool            mdeath = FALSE;
	bool            stormbringer = FALSE;

	cave_type       *c_ptr = &cave[y][x];
	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	char            m_name[80];

	/* Disturb the player */
	disturb(0, 0);

	energy_use = 100;

	if (m_ptr->csleep) /* It is not honorable etc to attack helpless victims */
	{
		if (!(r_info[m_ptr->r_idx].flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
		if (!(r_info[m_ptr->r_idx].flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Auto-Recall if possible and visible */
	if (m_ptr->ml) monster_race_track(m_ptr->ap_r_idx);

	/* Track a new monster */
	if (m_ptr->ml) health_track(c_ptr->m_idx);

	if ((r_info[m_ptr->r_idx].flags1 & RF1_FEMALE) &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image || !m_ptr->ml))
	{
		if ((inventory[INVEN_RARM].name1 == ART_ZANTETSU) || (inventory[INVEN_LARM].name1 == ART_ZANTETSU))
		{
#ifdef JP
			msg_print("�ۼԡ����ʤ��ϻ¤�̡�");
#else
			msg_print("I can not attack women!");
#endif
			return FALSE;
		}
	}

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
	{
#ifdef JP
		msg_print("�ʤ������⤹�뤳�Ȥ��Ǥ��ʤ���");
#else
		msg_print("Something prevent you from attacking.");
#endif
		return FALSE;
	}

	/* Stop if friendly */
	if (!is_hostile(m_ptr) &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image ||
	    p_ptr->shero || !m_ptr->ml))
	{
		if (inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (stormbringer)
		{
#ifdef JP
			msg_format("�����Ϥ϶��ߤ�%s�򹶷⤷����", m_name);
#else
			msg_format("Your black blade greedily attacks %s!", m_name);
#endif
			chg_virtue(V_INDIVIDUALISM, 1);
			chg_virtue(V_HONOUR, -1);
			chg_virtue(V_JUSTICE, -1);
			chg_virtue(V_COMPASSION, -1);
		}
		else if (p_ptr->pclass != CLASS_BERSERKER)
		{
#ifdef JP
			if (get_check("�����˹��⤷�ޤ�����"))
#else
			if (get_check("Really hit it? "))
#endif
			{
				chg_virtue(V_INDIVIDUALISM, 1);
				chg_virtue(V_HONOUR, -1);
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_COMPASSION, -1);
			}
			else
			{
#ifdef JP
				msg_format("%s�򹶷⤹��Τ�ߤ᤿��", m_name);
#else
				msg_format("You stop to avoid hitting %s.", m_name);
#endif
			return FALSE;
			}
		}
	}


	/* Handle player fear */
	if (p_ptr->afraid)
	{
		/* Message */
		if (m_ptr->ml)
#ifdef JP
		msg_format("������%s�򹶷�Ǥ��ʤ���", m_name);
#else
			msg_format("You are too afraid to attack %s!", m_name);
#endif

		else
#ifdef JP
			msg_format ("���ä��ˤϲ���������Τ����롪");
#else
			msg_format ("There is something scary in your way!");
#endif

		/* Disturb the monster */
		m_ptr->csleep = 0;
		p_ptr->update |= (PU_MON_LITE);

		/* Done */
		return FALSE;
	}

	if (p_ptr->migite && p_ptr->hidarite)
	{
		if ((p_ptr->skill_exp[GINOU_NITOURYU] < s_info[p_ptr->pclass].s_max[GINOU_NITOURYU]) && ((p_ptr->skill_exp[GINOU_NITOURYU] - 1000) / 200 < r_info[m_ptr->r_idx].level))
		{
			if (p_ptr->skill_exp[GINOU_NITOURYU] < 4000)
				p_ptr->skill_exp[GINOU_NITOURYU]+=80;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < 6000)
				p_ptr->skill_exp[GINOU_NITOURYU]+=4;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < 7000)
				p_ptr->skill_exp[GINOU_NITOURYU]+=1;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < 8000)
				if (one_in_(3)) p_ptr->skill_exp[GINOU_NITOURYU]+=1;
			p_ptr->update |= (PU_BONUS);
		}
	}

	/* Gain riding experience */
	if (p_ptr->riding)
	{
		int cur = p_ptr->skill_exp[GINOU_RIDING];
		int max = s_info[p_ptr->pclass].s_max[GINOU_RIDING];

		if (cur < max)
		{
			int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
			int targetlevel = r_info[m_ptr->r_idx].level;
			int inc = 0;

			if ((cur / 200 - 5) < targetlevel)
				inc += 1;

			/* Extra experience */
			if ((cur / 100) < ridinglevel)
			{
				if ((cur / 100 + 15) < ridinglevel)
					inc += 1 + (ridinglevel - (cur / 100 + 15));
				else
					inc += 1;
			}

			p_ptr->skill_exp[GINOU_RIDING] = MIN(max, cur + inc);

			p_ptr->update |= (PU_BONUS);
		}
	}

	riding_t_m_idx = c_ptr->m_idx;
	if (p_ptr->migite) py_attack_aux(y, x, &fear, &mdeath, 0, mode);
	if (p_ptr->hidarite && !mdeath) py_attack_aux(y, x, &fear, &mdeath, 1, mode);

	/* Mutations which yield extra 'natural' attacks */
	if (!mdeath)
	{
		if ((p_ptr->muta2 & MUT2_HORNS) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_HORNS, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_BEAK) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_BEAK, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_SCOR_TAIL) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_SCOR_TAIL, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_TRUNK) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_TRUNK, &fear, &mdeath);
		if ((p_ptr->muta2 & MUT2_TENTACLES) && !mdeath)
			natural_attack(c_ptr->m_idx, MUT2_TENTACLES, &fear, &mdeath);
	}

	/* Hack -- delay fear messages */
	if (fear && m_ptr->ml && !mdeath)
	{
		/* Sound */
		sound(SOUND_FLEE);

		/* Message */
#ifdef JP
		msg_format("%^s�϶��ݤ���ƨ���Ф�����", m_name);
#else
		msg_format("%^s flees in terror!", m_name);
#endif

	}

	if ((p_ptr->special_defense & KATA_IAI) && ((mode != HISSATSU_IAI) || mdeath))
	{
		set_action(ACTION_NONE);
	}

	return mdeath;
}


static bool pattern_seq(int c_y, int c_x, int n_y, int n_x)
{
	if (!pattern_tile(c_y, c_x) && !pattern_tile(n_y, n_x))
		return TRUE;

	if (cave[n_y][n_x].feat == FEAT_PATTERN_START)
	{
		if (!pattern_tile(c_y, c_x) &&
		    !p_ptr->confused && !p_ptr->stun && !p_ptr->image)
		{
#ifdef JP
			if (get_check("�ѥ�����ξ���⤭�Ϥ��ȡ����Ƥ��⤫�ʤ���Фʤ�ޤ��󡣤����Ǥ�����"))
#else
			if (get_check("If you start walking the Pattern, you must walk the whole way. Ok? "))
#endif

				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
	else if ((cave[n_y][n_x].feat == FEAT_PATTERN_OLD) ||
		 (cave[n_y][n_x].feat == FEAT_PATTERN_END) ||
		 (cave[n_y][n_x].feat == FEAT_PATTERN_XTRA2))
	{
		if (pattern_tile(c_y, c_x))
		{
			return TRUE;
		}
		else
		{
#ifdef JP
			msg_print("�ѥ�����ξ���⤯�ˤϥ����������������⤭�Ϥ�ʤ��ƤϤʤ�ޤ���");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
	}
	else if ((cave[n_y][n_x].feat == FEAT_PATTERN_XTRA1) ||
		 (cave[c_y][c_x].feat == FEAT_PATTERN_XTRA1))
	{
		return TRUE;
	}
	else if (cave[c_y][c_x].feat == FEAT_PATTERN_START)
	{
		if (pattern_tile(n_y, n_x))
			return TRUE;
		else
		{
#ifdef JP
			msg_print("�ѥ�����ξ��������������⤫�ͤФʤ�ޤ���");
#else
			msg_print("You must walk the Pattern in correct order.");
#endif

			return FALSE;
		}
	}
	else if ((cave[c_y][c_x].feat == FEAT_PATTERN_OLD) ||
		 (cave[c_y][c_x].feat == FEAT_PATTERN_END) ||
		 (cave[c_y][c_x].feat == FEAT_PATTERN_XTRA2))
	{
		if (!pattern_tile(n_y, n_x))
		{
#ifdef JP
			msg_print("�ѥ������Ƨ�߳����ƤϤ����ޤ���");
#else
			msg_print("You may not step off from the Pattern.");
#endif

			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (!pattern_tile(c_y, c_x))
		{
#ifdef JP
			msg_print("�ѥ�����ξ���⤯�ˤϥ����������������⤭�Ϥ�ʤ��ƤϤʤ�ޤ���");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
		else
		{
			byte ok_move = FEAT_PATTERN_START;
			switch (cave[c_y][c_x].feat)
			{
				case FEAT_PATTERN_1:
					ok_move = FEAT_PATTERN_2;
					break;
				case FEAT_PATTERN_2:
					ok_move = FEAT_PATTERN_3;
					break;
				case FEAT_PATTERN_3:
					ok_move = FEAT_PATTERN_4;
					break;
				case FEAT_PATTERN_4:
					ok_move = FEAT_PATTERN_1;
					break;
				default:
					if (p_ptr->wizard)
#ifdef JP
						msg_format("�������ʥѥ�������ԡ�%d��", cave[c_y][c_x]);
#else
						msg_format("Funny Pattern walking, %d.", cave[c_y][c_x]);
#endif

					return TRUE; /* Goof-up */
			}

			if ((cave[n_y][n_x].feat == ok_move) ||
			    (cave[n_y][n_x].feat == cave[c_y][c_x].feat))
				return TRUE;
			else
			{
				if (!pattern_tile(n_y, n_x))
#ifdef JP
					msg_print("�ѥ������Ƨ�߳����ƤϤ����ޤ���");
#else
					msg_print("You may not step off from the Pattern.");
#endif

				else
#ifdef JP
					msg_print("�ѥ�����ξ��������������⤫�ͤФʤ�ޤ���");
#else
					msg_print("You must walk the Pattern in correct order.");
#endif


				return FALSE;
			}
		}
	}
}



bool player_can_enter(byte feature)
{
	bool pass_wall;

	/* Player can not walk through "walls" unless in Shadow Form */
	if (p_ptr->wraith_form || p_ptr->pass_wall || p_ptr->kabenuke)
		pass_wall = TRUE;
	else
		pass_wall = FALSE;

	switch (feature)
	{
		case FEAT_DEEP_WATER:
		case FEAT_SHAL_LAVA:
		case FEAT_DEEP_LAVA:
			return (TRUE);

		case FEAT_DARK_PIT:
		{
			if (p_ptr->ffall)
				return (TRUE);
			else
				return (FALSE);
		}

		case FEAT_TREES:
		{
			return (TRUE);
		}

		case FEAT_RUBBLE:
		case FEAT_MAGMA:
		case FEAT_QUARTZ:
		case FEAT_MAGMA_H:
		case FEAT_QUARTZ_H:
		case FEAT_MAGMA_K:
		case FEAT_QUARTZ_K:
		case FEAT_WALL_EXTRA:
		case FEAT_WALL_INNER:
		case FEAT_WALL_OUTER:
		case FEAT_WALL_SOLID:
		{
			return (pass_wall);
		}

		case FEAT_MOUNTAIN:
		{
			return (!dun_level && p_ptr->ffall);
		}
		case FEAT_PERM_EXTRA:
		case FEAT_PERM_INNER:
		case FEAT_PERM_OUTER:
		case FEAT_PERM_SOLID:
		case FEAT_PATTERN_START:
		case FEAT_PATTERN_1:
		case FEAT_PATTERN_2:
		case FEAT_PATTERN_3:
		case FEAT_PATTERN_4:
		case FEAT_PATTERN_END:
		case FEAT_PATTERN_OLD:
		case FEAT_PATTERN_XTRA1:
		case FEAT_PATTERN_XTRA2:
		{
			return (FALSE);
		}
	}

	return (TRUE);
}


/*
 * Move player in the given direction, with the given "pickup" flag.
 *
 * This routine should (probably) always induce energy expenditure.
 *
 * Note that moving will *always* take a turn, and will *always* hit
 * any monster which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible monsters.
 */
void move_player(int dir, int do_pickup, bool break_trap)
{
	int y, x;

	cave_type *c_ptr;
	monster_type *m_ptr;

	char m_name[80];

	bool p_can_pass_walls = FALSE;
	bool stormbringer = FALSE;

	bool oktomove = TRUE;
	bool do_past = FALSE;

	/* Find the result of moving */
	y = py + ddy[dir];
	x = px + ddx[dir];

	/* Examine the destination */
	c_ptr = &cave[y][x];

	/* Exit the area */
	if (!dun_level && !p_ptr->wild_mode &&
		((x == 0) || (x == MAX_WID - 1) ||
		 (y == 0) || (y == MAX_HGT - 1)))
	{
		/* Can the player enter the grid? */
		if (c_ptr->mimic && player_can_enter(c_ptr->mimic))
		{
			/* Hack: move to new area */
			if ((y == 0) && (x == 0))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == 0) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = 1;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == 0))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = cur_wid - 2;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = 1;
				ambush_flag = FALSE;
			}

			else if (y == 0)
			{
				p_ptr->wilderness_y--;
				p_ptr->oldpy = cur_hgt - 2;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (y == MAX_HGT - 1)
			{
				p_ptr->wilderness_y++;
				p_ptr->oldpy = 1;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (x == 0)
			{
				p_ptr->wilderness_x--;
				p_ptr->oldpx = cur_wid - 2;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}

			else if (x == MAX_WID - 1)
			{
				p_ptr->wilderness_x++;
				p_ptr->oldpx = 1;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}

			p_ptr->leftbldg = TRUE;
			p_ptr->leaving = TRUE;
			energy_use = 100;

			return;
		}

		oktomove = FALSE;
	}

	/* Get the monster */
	m_ptr = &m_list[c_ptr->m_idx];


	if (inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	if (inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	if (p_ptr->wraith_form || p_ptr->pass_wall || p_ptr->kabenuke)
		p_can_pass_walls = TRUE;
	if ((cave[y][x].feat >= FEAT_PERM_EXTRA) &&
	    (cave[y][x].feat <= FEAT_PERM_SOLID))
	{
		p_can_pass_walls = FALSE;
	}

	if (p_ptr->riding)
	{
		cave[py][px].m_idx = 0;
	}

	/* Hack -- attack monsters */
	if (c_ptr->m_idx && (m_ptr->ml || cave_floor_bold(y, x) || p_can_pass_walls))
	{

		/* Attack -- only if we can see it OR it is not in a wall */
		if (!is_hostile(m_ptr) &&
		    !(p_ptr->confused || p_ptr->image || !m_ptr->ml || p_ptr->stun ||
		    ((p_ptr->muta2 & MUT2_BERS_RAGE) && p_ptr->shero)) &&
		    (pattern_seq(py, px, y, x)) &&
		    ((cave_floor_bold(y, x)) || (c_ptr->feat == FEAT_TREES) || (p_can_pass_walls)))
		{
			m_ptr->csleep = 0;
			p_ptr->update |= (PU_MON_LITE);

			/* Extract monster name (or "it") */
			monster_desc(m_name, m_ptr, 0);

			/* Auto-Recall if possible and visible */
			if (m_ptr->ml) monster_race_track(m_ptr->ap_r_idx);

			/* Track a new monster */
			if (m_ptr->ml) health_track(c_ptr->m_idx);

			/* displace? */
			if ((stormbringer && (randint1(1000) > 666)) || (p_ptr->pclass == CLASS_BERSERKER))
			{
				py_attack(y, x, 0);
				oktomove = FALSE;
			}
			else if (monster_can_cross_terrain(cave[py][px].feat, &r_info[m_ptr->r_idx]) &&
				 (cave_floor_bold(py, px) || cave[py][px].feat == FEAT_TREES ||
				  (r_info[m_ptr->r_idx].flags2 & RF2_PASS_WALL)))
			{
				do_past = TRUE;
			}
			else
			{
#ifdef JP
				msg_format("%^s���������", m_name);
#else
				msg_format("%^s is in your way!", m_name);
#endif

				energy_use = 0;
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			py_attack(y, x, 0);
			oktomove = FALSE;
		}
	}

	if (!oktomove)
	{
	}

	else if ((c_ptr->feat == FEAT_DARK_PIT) && !p_ptr->ffall)
	{
#ifdef JP
		msg_print("�����ܤ��ڤ뤳�ȤϤǤ��ޤ���");
#else
		msg_print("You can't cross the chasm.");
#endif

		energy_use = 0;
		running = 0;
		oktomove = FALSE;
	}

	else if (c_ptr->feat == FEAT_MOUNTAIN)
	{
		if (dun_level || !p_ptr->ffall)
		{
#ifdef JP
			msg_print("���ˤ��Ф�ޤ���");
#else
			msg_print("You can't climb the mountains!");
#endif

			running = 0;
			energy_use = 0;
			oktomove = FALSE;
		}
	}
	/*
	 * Player can move through trees and
	 * has effective -10 speed
	 * Rangers can move without penality
	 */
	else if (c_ptr->feat == FEAT_TREES)
	{
		oktomove = TRUE;
		if ((p_ptr->pclass != CLASS_RANGER) && !p_ptr->ffall) energy_use *= 2;
	}

	else if ((c_ptr->feat >= FEAT_QUEST_ENTER) &&
		(c_ptr->feat <= FEAT_QUEST_EXIT))
	{
		oktomove = TRUE;
	}

#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && is_known_trap(c_ptr))
	{
		bool ignore = FALSE;
		switch (c_ptr->feat)
		{
			case FEAT_TRAP_TRAPDOOR:
			case FEAT_TRAP_PIT:
			case FEAT_TRAP_SPIKED_PIT:
			case FEAT_TRAP_POISON_PIT:
				if (p_ptr->ffall) ignore = TRUE;
				break;
			case FEAT_TRAP_TELEPORT:
				if (p_ptr->anti_tele) ignore = TRUE;
				break;
			case FEAT_TRAP_FIRE:
				if (p_ptr->immune_fire) ignore = TRUE;
				break;
			case FEAT_TRAP_ACID:
				if (p_ptr->immune_acid) ignore = TRUE;
				break;
			case FEAT_TRAP_BLIND:
				if (p_ptr->resist_blind) ignore = TRUE;
				break;
			case FEAT_TRAP_CONFUSE:
				if (p_ptr->resist_conf) ignore = TRUE;
				break;
			case FEAT_TRAP_POISON:
				if (p_ptr->resist_pois) ignore = TRUE;
				break;
			case FEAT_TRAP_SLEEP:
				if (p_ptr->free_act) ignore = TRUE;
				break;
		}

		if (!ignore)
		{
			(void)do_cmd_disarm_aux(y, x, dir);
			return;
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */
	else if (p_ptr->riding && (r_info[m_list[p_ptr->riding].r_idx].flags1 & RF1_NEVER_MOVE))
	{
#ifdef JP
		msg_print("ư���ʤ���");
#else
		msg_print("Can't move!");
#endif
		energy_use = 0;
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if (p_ptr->riding && m_list[p_ptr->riding].monfear)
	{
		char m_name[80];

		/* Acquire the monster name */
		monster_desc(m_name, &m_list[p_ptr->riding], 0);

		/* Dump a message */
#ifdef JP
msg_format("%s�����ݤ��Ƥ�������Ǥ��ʤ���", m_name);
#else
		msg_format("%^s is too scared to control.", m_name);
#endif
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if (p_ptr->riding && p_ptr->riding_ryoute)
	{
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if ((p_ptr->riding && (r_info[m_list[p_ptr->riding].r_idx].flags7 & RF7_AQUATIC)) && (c_ptr->feat != FEAT_SHAL_WATER) && (c_ptr->feat != FEAT_DEEP_WATER))
	{
#ifdef JP
		msg_print("Φ��˾夬��ʤ���");
#else
		msg_print("Can't land.");
#endif
		energy_use = 0;
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if ((p_ptr->riding && !(r_info[m_list[p_ptr->riding].r_idx].flags7 & (RF7_AQUATIC | RF7_CAN_SWIM | RF7_CAN_FLY))) && (c_ptr->feat == FEAT_DEEP_WATER))
	{
#ifdef JP
		msg_print("���˹Ԥ��ʤ���");
#else
		msg_print("Can't swim.");
#endif
		energy_use = 0;
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if ((p_ptr->riding && (r_info[m_list[p_ptr->riding].r_idx].flags2 & (RF2_AURA_FIRE)) && !(r_info[m_list[p_ptr->riding].r_idx].flags7 & (RF7_CAN_FLY))) && (c_ptr->feat == FEAT_SHAL_WATER))
	{
#ifdef JP
		msg_print("���˹Ԥ��ʤ���");
#else
		msg_print("Can't swim.");
#endif
		energy_use = 0;
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if ((p_ptr->riding && !(r_info[m_list[p_ptr->riding].r_idx].flags7 & (RF7_CAN_FLY)) && !(r_info[m_list[p_ptr->riding].r_idx].flags3 & (RF3_IM_FIRE))) && ((c_ptr->feat == FEAT_SHAL_LAVA) || (c_ptr->feat == FEAT_DEEP_LAVA)))
	{
#ifdef JP
		msg_print("�ϴ�ξ�˹Ԥ��ʤ���");
#else
		msg_print("Too hot to go through.");
#endif
		energy_use = 0;
		oktomove = FALSE;
		disturb(0, 0);
	}

	else if (p_ptr->riding && m_list[p_ptr->riding].stunned && one_in_(2))
	{
		char m_name[80];
		monster_desc(m_name, &m_list[p_ptr->riding], 0);
#ifdef JP
		msg_format("%s��ۯ۰�Ȥ��Ƥ��Ƥ��ޤ�ư���ʤ���",m_name);
#else
		msg_format("You cannot control stunned %s!",m_name);
#endif
		oktomove = FALSE;
		disturb(0, 0);
	}

	/* Player can not walk through "walls" unless in wraith form...*/
	else if ((!cave_floor_bold(y, x)) &&
		(!p_can_pass_walls))
	{
		byte feat;

		oktomove = FALSE;

		/* Feature code (applying "mimic" field) */
		feat = c_ptr->mimic ? c_ptr->mimic : f_info[c_ptr->feat].mimic;

		/* Disturb the player */
		disturb(0, 0);

		/* Notice things in the dark */
		if ((!(c_ptr->info & (CAVE_MARK))) &&
		    (p_ptr->blind || !(c_ptr->info & (CAVE_LITE))))
		{
			/* Rubble */
			if (feat == FEAT_RUBBLE)
			{
#ifdef JP
				msg_print("���Ф��Ԥ����ϤФ�Ǥ���褦����");
#else
				msg_print("You feel some rubble blocking your way.");
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(y, x);
			}

			/* Closed door */
			else if (is_closed_door(feat))
			{
#ifdef JP
				msg_print("�ɥ����Ԥ����ϤФ�Ǥ���褦����");
#else
				msg_print("You feel a closed door blocking your way.");
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(y, x);
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_print("�ɤ��Ԥ����ϤФ�Ǥ���褦����");
#else
				msg_print("You feel a wall blocking your way.");
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(y, x);
			}
		}

		/* Notice things */
		else
		{
			/* Rubble */
			if (feat == FEAT_RUBBLE)
			{
#ifdef JP
				msg_print("���Ф��Ԥ����ϤФ�Ǥ��롣");
#else
				msg_print("There is rubble blocking your way.");
#endif


				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
			}
			/* Closed doors */
			else if (is_closed_door(feat))
			{
#ifdef ALLOW_EASY_OPEN

				if (easy_open && easy_open_door(y, x)) return;

#endif /* ALLOW_EASY_OPEN */

#ifdef JP
				msg_print("�ɥ����Ԥ����ϤФ�Ǥ��롣");
#else
				msg_print("There is a closed door blocking your way.");
#endif


				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_print("�ɤ��Ԥ����ϤФ�Ǥ��롣");
#else
				msg_print("There is a wall blocking your way.");
#endif


				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
					energy_use = 0;
			}
		}

		/* Sound */
		sound(SOUND_HITWALL);
	}

	/* Normal movement */
	if (!pattern_seq(py, px, y, x))
	{
		if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
		{
			energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(0, 0);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		int oy, ox;

		if (p_ptr->warning)
		{
			if(!process_frakir(x,y))
			{
				energy_use = 25;
				return;
			}
		}

		if (do_past)
		{
#ifdef JP
			msg_format("%s�򲡤��ऱ����", m_name);
#else
			msg_format("You push past %s.", m_name);
#endif

			m_ptr->fy = py;
			m_ptr->fx = px;
			cave[py][px].m_idx = c_ptr->m_idx;
			c_ptr->m_idx = 0;
			update_mon(cave[py][px].m_idx, TRUE);
		}

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if (p_ptr->wild_mode)
		{
			if(ddy[dir] > 0)  p_ptr->oldpy = 1;
			if(ddy[dir] < 0)  p_ptr->oldpy = MAX_HGT - 2;
			if(ddy[dir] == 0) p_ptr->oldpy = MAX_HGT / 2;
			if(ddx[dir] > 0)  p_ptr->oldpx = 1;
			if(ddx[dir] < 0)  p_ptr->oldpx = MAX_WID - 2;
			if(ddx[dir] == 0) p_ptr->oldpx = MAX_WID / 2;
		}

		/* Save old location */
		oy = py;
		ox = px;

		/* Move the player */
		py = y;
		px = x;

		if (p_ptr->riding && (r_info[m_list[p_ptr->riding].r_idx].flags2 & RF2_KILL_WALL))
		{
			if (cave[py][px].feat >= FEAT_RUBBLE && cave[py][px].feat < FEAT_PERM_SOLID)
			{
				/* Forget the wall */
				cave[py][px].info &= ~(CAVE_MARK);

				/* Notice */
				cave_set_feat(py, px, floor_type[randint0(100)]);
			}
		}
		if (music_singing(MUSIC_WALL))
		{
			project(0, 0, py, px,
				(60 + p_ptr->lev), GF_DISINTEGRATE, PROJECT_KILL | PROJECT_ITEM, -1);
		}
		else if (p_ptr->kill_wall)
		{
			if (cave_valid_bold(py, px) &&
				(cave[py][px].feat < FEAT_PATTERN_START ||
				 cave[py][px].feat > FEAT_PATTERN_XTRA2) &&
				(cave[py][px].feat < FEAT_DEEP_WATER ||
				 cave[py][px].feat > FEAT_GRASS))
			{
				if (cave[py][px].feat == FEAT_TREES)
					cave_set_feat(py, px, FEAT_GRASS);
				else
					cave_set_feat(py, px, floor_type[randint0(100)]);
			}
				/* Update some things -- similar to GF_KILL_WALL */
			p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MONSTERS | PU_MON_LITE);
		}

		/* Redraw new spot */
		lite_spot(py, px);

		/* Redraw old spot */
		lite_spot(oy, ox);

		/* Sound */
		/* sound(SOUND_WALK); */

		/* Check for new panel (redraw map) */
		verify_panel();

		/* For get everything when requested hehe I'm *NASTY* */
		if (dun_level && (d_info[dungeon_type].flags1 & DF1_FORGET))
		{
			wiz_dark();
		}

		if ((p_ptr->pclass == CLASS_NINJA))
		{
			if (c_ptr->info & (CAVE_GLOW)) set_superstealth(FALSE);
			else if (p_ptr->cur_lite <= 0) set_superstealth(TRUE);
		}
		if ((p_ptr->action == ACTION_HAYAGAKE) && !cave_floor_bold(py, px))
		{
#ifdef JP
			msg_print("�����Ǥ����᤯ư���ʤ���");
#else
			msg_print("You cannot run in wall.");
#endif
			set_action(ACTION_NONE);
		}

		/* Update stuff */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

		/* Update the monsters */
		p_ptr->update |= (PU_DISTANCE);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Spontaneous Searching */
		if ((p_ptr->skill_fos >= 50) ||
		    (0 == randint0(50 - p_ptr->skill_fos)))
		{
			search();
		}

		/* Continuous Searching */
		if (p_ptr->action == ACTION_SEARCH)
		{
			search();
		}

		/* Handle "objects" */

#ifdef ALLOW_EASY_DISARM /* TNB */

		carry(do_pickup != always_pickup);

#else /* ALLOW_EASY_DISARM -- TNB */

		carry(do_pickup);

#endif /* ALLOW_EASY_DISARM -- TNB */

		/* Handle "store doors" */
		if (((c_ptr->feat >= FEAT_SHOP_HEAD) &&
		    (c_ptr->feat <= FEAT_SHOP_TAIL)) ||
		    (c_ptr->feat == FEAT_MUSEUM))
		{
			/* Disturb */
			disturb(0, 0);

			energy_use = 0;
			/* Hack -- Enter store */
			command_new = SPECIAL_KEY_STORE;
		}

		/* Handle "building doors" -KMW- */
		else if ((c_ptr->feat >= FEAT_BLDG_HEAD) &&
		    (c_ptr->feat <= FEAT_BLDG_TAIL))
		{
			/* Disturb */
			disturb(0, 0);

			energy_use = 0;
			/* Hack -- Enter building */
			command_new = SPECIAL_KEY_BUILDING;
		}

		/* Handle quest areas -KMW- */
		else if (cave[y][x].feat == FEAT_QUEST_ENTER)
		{
			/* Disturb */
			disturb(0, 0);

			energy_use = 0;
			/* Hack -- Enter quest level */
			command_new = SPECIAL_KEY_QUEST;
		}

		else if (cave[y][x].feat == FEAT_QUEST_EXIT)
		{
			if (quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_EXIT)
			{
				if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, p_ptr->inside_quest, NULL);
				quest[p_ptr->inside_quest].status = QUEST_STATUS_COMPLETED;
				quest[p_ptr->inside_quest].complev = (byte)p_ptr->lev;
#ifdef JP
				msg_print("�������Ȥ�ã��������");
#else
				msg_print("You accomplished your quest!");
#endif

				msg_print(NULL);
			}

			leave_quest_check();

			p_ptr->inside_quest = cave[y][x].special;
			dun_level = 0;
			p_ptr->oldpx = 0;
			p_ptr->oldpy = 0;
			p_ptr->leaving = TRUE;
		}

		/* Set off a trap */
		else if (is_trap(c_ptr->feat))
		{
			/* Disturb */
			disturb(0, 0);

			/* Hidden trap */
			if (c_ptr->mimic)
			{
				/* Message */
#ifdef JP
				msg_print("�ȥ�åפ���");
#else
				msg_print("You found a trap!");
#endif

				/* Pick a trap */
				disclose_grid(py, px);
			}

			/* Hit the trap */
			hit_trap(break_trap);
		}

		/* Warn when leaving trap detected region */
		if ((disturb_trap_detect || alert_trap_detect)
		    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
		{
			/* No duplicate warning */
			p_ptr->dtrap = FALSE;

			/* You are just on the edge */
			if (!(cave[py][px].info & CAVE_UNSAFE))
			{
				if (alert_trap_detect)
				{
#ifdef JP
					msg_print("* ���:������ϥȥ�åפδ����ϰϳ��Ǥ��� *");
#else
					msg_print("*Leaving trap detect region!*");
#endif
				}
				
				if (disturb_trap_detect)
				{
					disturb(0, 0);
				}
			}
		}
	}

	if (p_ptr->riding)
	{
		m_list[p_ptr->riding].fy = py;
		m_list[p_ptr->riding].fx = px;
		cave[py][px].m_idx = p_ptr->riding;
		update_mon(cave[py][px].m_idx, TRUE);
		p_ptr->update |= (PU_MON_LITE);
	}
}


/*
 * Hack -- Check for a "known wall" (see below)
 */
static int see_wall(int dir, int y, int x)
{
	cave_type   *c_ptr;
	byte feat;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are not known walls */
	if (!in_bounds2(y, x)) return (FALSE);

	/* Access grid */
	c_ptr = &cave[y][x];

	/* Feature code (applying "mimic" field) */
	feat = c_ptr->mimic ? c_ptr->mimic : f_info[c_ptr->feat].mimic;

	/* Must be known to the player */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Rubble, Magma, Quartz, Wall, Perm wall */
		if (feat >= FEAT_RUBBLE && feat <= FEAT_PERM_SOLID) return TRUE;

		/* Tree */
		if (feat == FEAT_TREES) return TRUE;

		/* Mountain */
		if (feat == FEAT_MOUNTAIN) return TRUE;
	}

	return FALSE;
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int y, int x)
{
	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_bounds2(y, x)) return (TRUE);

	/* Memorized grids are always known */
	if (cave[y][x].info & (CAVE_MARK)) return (FALSE);

	/* Non-floor grids are unknown */
	if (!cave_floor_bold(y, x)) return (TRUE);

	/* Viewable door/wall grids are known */
	if (player_can_see_bold(y, x)) return (FALSE);

	/* Default */
	return (TRUE);
}





/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * ##################
 * o@x       1
 * ########### ######
 * #2          #
 * #############
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 *   ...!              ...
 *   .o@!  (normal)    .o.!  (diagonal)
 *   ...!  (east)      ..@!  (south east)
 *                      !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible monsters or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * ##s
 * @x?
 * #.?
 *
 * If they are both seen to be closed, then it is seen that no benefit
 * is gained from moving straight. It is a known corner.  To cut the
 * corner, go diagonally, otherwise go straight, but pretend you
 * stepped diagonally into that next location for a full view next
 * time. Conversely, if one of the ? squares is not seen to be closed,
 * then there is a potential choice. We check to see whether it is a
 * potential corner or an intersection/room entrance.  If the square
 * two spaces straight ahead, and the space marked with 's' are both
 * unknown space, then it is a potential corner and enter if
 * find_examine is set, otherwise must stop because it is not a
 * corner.
 */




/*
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

/*
 * The direction we are running
 */
static byte find_current;

/*
 * The direction we came from
 */
static byte find_prevdir;

/*
 * We are looking for open area
 */
static bool find_openarea;

/*
 * We are looking for a break
 */
static bool find_breakright;
static bool find_breakleft;



/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
static void run_init(int dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;


	/* Save the direction */
	find_current = dir;

	/* Assume running straight */
	find_prevdir = dir;

	/* Assume looking for open area */
	find_openarea = TRUE;

	/* Assume not looking for breaks */
	find_breakright = find_breakleft = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	p_ptr->run_py = py;
	p_ptr->run_px = px;

	/* Find the destination grid */
	row = py + ddy[dir];
	col = px + ddx[dir];

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if (see_wall(cycle[i+1], py, px))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if (see_wall(cycle[i-1], py, px))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cycle[i], row, col))
		{
			if (shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
			{
				find_prevdir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
static bool run_test(void)
{
	int         prev_dir, new_dir, check_dir = 0;
	int         row, col;
	int         i, max, inv;
	int         option = 0, option2 = 0;
	cave_type   *c_ptr;
	byte feat;

	/* Where we came from */
	prev_dir = find_prevdir;


	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* ���:������ϥȥ�åפδ����ϰϳ��Ǥ��� *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		s16b this_o_idx, next_o_idx = 0;


		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = c_ptr->mimic ? c_ptr->mimic : f_info[c_ptr->feat].mimic;

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Visible object */
			if (o_ptr->marked) return (TRUE);
		}


		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (c_ptr->info & (CAVE_MARK))
		{
			bool notice = TRUE;

			/* Examine the terrain */
			switch (feat)
			{
				/* Floors */
				case FEAT_FLOOR:

				/* Invis traps */
				case FEAT_INVIS:

				/* Normal veins */
				case FEAT_MAGMA:
				case FEAT_QUARTZ:

				/* Hidden treasure */
				case FEAT_MAGMA_H:
				case FEAT_QUARTZ_H:

				/* Known treasure (almost uninteresting) */
				case FEAT_MAGMA_K:
				case FEAT_QUARTZ_K:

				/* Walls */
				case FEAT_RUBBLE:
				case FEAT_WALL_EXTRA:
				case FEAT_WALL_INNER:
				case FEAT_WALL_OUTER:
				case FEAT_WALL_SOLID:
				case FEAT_PERM_EXTRA:
				case FEAT_PERM_INNER:
				case FEAT_PERM_OUTER:
				case FEAT_PERM_SOLID:
				/* dirt, grass, trees, ... */
				case FEAT_SHAL_WATER:
				case FEAT_DIRT:
				case FEAT_GRASS:
				case FEAT_DEEP_GRASS:
				case FEAT_FLOWER:
				case FEAT_DARK_PIT:
				case FEAT_TREES:
				case FEAT_MOUNTAIN:
				{
					/* Ignore */
					notice = FALSE;

					/* Done */
					break;
				}

				/* quest features */
				case FEAT_QUEST_ENTER:
				case FEAT_QUEST_EXIT:
				{
					/* Notice */
					notice = TRUE;

					/* Done */
					break;
				}

				case FEAT_DEEP_LAVA:
				case FEAT_SHAL_LAVA:
				{
					/* Ignore */
					if (p_ptr->invuln || p_ptr->immune_fire) notice = FALSE;

					/* Done */
					break;
				}

				case FEAT_DEEP_WATER:
				{
					/* Ignore */
					if (p_ptr->ffall || p_ptr->total_weight<= (((u32b)adj_str_wgt[p_ptr->stat_ind[A_STR]]*(p_ptr->pclass == CLASS_BERSERKER ? 150 : 100))/2)) notice = FALSE;

					/* Done */
					break;
				}

				/* Open doors */
				case FEAT_OPEN:
				case FEAT_BROKEN:
				{
					/* Option -- ignore */
					if (find_ignore_doors) notice = FALSE;

					/* Done */
					break;
				}

				/* Stairs */
				case FEAT_LESS:
				case FEAT_MORE:
				case FEAT_LESS_LESS:
				case FEAT_MORE_MORE:
				case FEAT_ENTRANCE:
				{
					/* Option -- ignore */
					if (find_ignore_stairs) notice = FALSE;

					/* Done */
					break;
				}
			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if (inv || (!(feat & 0x20)))
		{
			/* Looking for open area */
			if (find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if (find_openarea)
			{
				if (i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}


	/* Looking for open area */
	if (find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			new_dir = cycle[chome[prev_dir] + i];

			row = py + ddy[new_dir];
			col = px + ddx[new_dir];

			/* Access grid */
			c_ptr = &cave[row][col];

			/* Feature code (applying "mimic" field) */
			feat = c_ptr->mimic ? c_ptr->mimic : f_info[c_ptr->feat].mimic;

			/* Unknown grid or non-wall XXX XXX XXX cave_floor_grid(c_ptr)) */
			if (!(c_ptr->info & (CAVE_MARK)) ||
			    ((feat <= FEAT_DOOR_TAIL) ||
			     (feat == FEAT_FLOWER) ||
			     (feat == FEAT_DEEP_GRASS) ||
			     ((feat >= FEAT_DEEP_WATER) &&
			      (feat <= FEAT_GRASS))))

			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			new_dir = cycle[chome[prev_dir] + i];

			row = py + ddy[new_dir];
			col = px + ddx[new_dir];

			/* Access grid */
			c_ptr = &cave[row][col];

			/* Feature code (applying "mimic" field) */
			feat = c_ptr->mimic ? c_ptr->mimic : f_info[c_ptr->feat].mimic;

			/* Unknown grid or non-wall XXX XXX XXX cave_floor_grid(c_ptr)) */
			if (!(c_ptr->info & (CAVE_MARK)) ||
			    ((feat <= FEAT_DOOR_TAIL) ||
			     (feat == FEAT_FLOWER) ||
			     (feat == FEAT_DEEP_GRASS) ||
			     ((feat >= FEAT_DEEP_WATER) &&
			      (feat <= FEAT_GRASS))))

			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}
		}
	}


	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if (find_examine && !find_cut)
		{
			/* Primary option */
			find_current = option;

			/* Hack -- allow curving */
			find_prevdir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = py + ddy[option];
			col = px + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(option, row, col) ||
			    !see_wall(check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (find_examine &&
				    see_nothing(option, row, col) &&
				    see_nothing(option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
			{
				find_current = option2;
				find_prevdir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				find_current = option;
				find_prevdir = option2;
			}
		}
	}


	/* About to hit a known wall, stop */
	if (see_wall(find_current, py, px))
	{
		return (TRUE);
	}


	/* Failure */
	return (FALSE);
}



/*
 * Take one step along the current "run" path
 */
void run_step(int dir)
{
	/* Start running */
	if (dir)
	{
		cave_type   *c_ptr;
		byte feat;
	
		/* Access grid */
		c_ptr = &cave[py+ddy[dir]][px+ddx[dir]];

		/* Feature code (applying "mimic" field) */
		feat = c_ptr->mimic ? c_ptr->mimic : f_info[c_ptr->feat].mimic;

		/* Hack -- do not start silly run */
		if (see_wall(dir, py, px) &&
		   (feat != FEAT_TREES))
		{
			/* Message */
#ifdef JP
			msg_print("���������ˤϹԤ��ޤ���");
#else
			msg_print("You cannot run in that direction.");
#endif


			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);

		/* Initialize */
		run_init(dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test())
		{
			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	if (--running <= 0) return;

	/* Take time */
	energy_use = 100;

	/* Move the player, using the "pickup" flag */
#ifdef ALLOW_EASY_DISARM /* TNB */

	move_player(find_current, FALSE, FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

	move_player(find_current, always_pickup, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */

	if ((py == p_ptr->run_py) && (px == p_ptr->run_px))
	{
		p_ptr->run_py = 0;
		p_ptr->run_px = 0;
		disturb(0, 0);
	}
}
