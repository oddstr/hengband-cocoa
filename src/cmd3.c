/* File: cmd3.c */

/* Purpose: Inventory commands */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"



/*
 * Display inventory
 */
void do_cmd_inven(void)
{
	char out_val[160];


	/* Note that we are in "inventory" mode */
	command_wrk = FALSE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "inventory" mode */
	if (easy_floor) command_wrk = (USE_INVEN);

#endif /* ALLOW_EASY_FLOOR */

	/* Save screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the inventory */
	(void)show_inven(0);

	/* Hack -- hide empty slots */
	item_tester_full = FALSE;

#ifdef JP
	sprintf(out_val, "����ʪ�� ��� %3d.%1d kg (�³���%ld%%) ���ޥ��: ",
	    lbtokg1(p_ptr->total_weight) , lbtokg2(p_ptr->total_weight) ,
	    (p_ptr->total_weight * 100) / ((adj_str_wgt[p_ptr->stat_ind[A_STR]] * (p_ptr->pclass == CLASS_BERSERKER ? 150 : 100)) 
/ 2));
#else
	sprintf(out_val, "Inventory: carrying %d.%d pounds (%ld%% of capacity). Command: ",
	    (int)(p_ptr->total_weight / 10), (int)(p_ptr->total_weight % 10),
	    (p_ptr->total_weight * 100) / ((adj_str_wgt[p_ptr->stat_ind[A_STR]] * (p_ptr->pclass == CLASS_BERSERKER ? 150 : 100)) / 2));
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Load screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Hack -- Use "display" mode */
		command_see = TRUE;
	}
}


/*
 * Display equipment
 */
void do_cmd_equip(void)
{
	char out_val[160];


	/* Note that we are in "equipment" mode */
	command_wrk = TRUE;

#ifdef ALLOW_EASY_FLOOR

	/* Note that we are in "equipment" mode */
	if (easy_floor) command_wrk = (USE_EQUIP);

#endif /* ALLOW_EASY_FLOOR  */

	/* Save the screen */
	screen_save();

	/* Hack -- show empty slots */
	item_tester_full = TRUE;

	/* Display the equipment */
	(void)show_equip(0);

	/* Hack -- undo the hack above */
	item_tester_full = FALSE;

	/* Build a prompt */
#ifdef JP
	sprintf(out_val, "������ ��� %3d.%1d kg (�³���%ld%%) ���ޥ��: ",
	    lbtokg1(p_ptr->total_weight) , lbtokg2(p_ptr->total_weight) ,
	    (p_ptr->total_weight * 100) / ((adj_str_wgt[p_ptr->stat_ind[A_STR]] * (p_ptr->pclass == CLASS_BERSERKER ? 150 : 100)) 
/ 2));
#else
	sprintf(out_val, "Equipment: carrying %d.%d pounds (%ld%% of capacity). Command: ",
	    (int)(p_ptr->total_weight / 10), (int)(p_ptr->total_weight % 10),
	    (p_ptr->total_weight * 100) / ((adj_str_wgt[p_ptr->stat_ind[A_STR]] * (p_ptr->pclass == CLASS_BERSERKER ? 150 : 100)) / 2));
#endif


	/* Get a command */
	prt(out_val, 0, 0);

	/* Get a new command */
	command_new = inkey();

	/* Restore the screen */
	screen_load();


	/* Process "Escape" */
	if (command_new == ESCAPE)
	{
		int wid, hgt;

		/* Get size */
		Term_get_size(&wid, &hgt);

		/* Reset stuff */
		command_new = 0;
		command_gap = wid - 30;
	}

	/* Process normal keys */
	else
	{
		/* Enter "display" mode */
		command_see = TRUE;
	}
}


/*
 * The "wearable" tester
 */
static bool item_tester_hook_wear(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_ABUNAI_MIZUGI))
		if (p_ptr->psex == SEX_MALE) return FALSE;

	/* Check for a usable slot */
	if (wield_slot(o_ptr) >= INVEN_RARM) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


static bool item_tester_hook_mochikae(object_type *o_ptr)
{
	/* Check for a usable slot */
	if (((o_ptr->tval >= TV_DIGGING) && (o_ptr->tval <= TV_SWORD)) ||
	    (o_ptr->tval == TV_SHIELD) || (o_ptr->tval == TV_CAPTURE) ||
	    (o_ptr->tval == TV_CARD)) return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


static bool item_tester_hook_melee_weapon(object_type *o_ptr)
{
	/* Check for a usable slot */
	if ((o_ptr->tval >= TV_DIGGING) && (o_ptr->tval <= TV_SWORD))return (TRUE);

	/* Assume not wearable */
	return (FALSE);
}


/*
 * Wield or wear a single item from the pack or floor
 */
void do_cmd_wield(void)
{
	int i, item, slot;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr act;

	char o_name[MAX_NLEN];

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Restrict the choices */
	item_tester_hook = item_tester_hook_wear;

	/* Get an item */
#ifdef JP
	q = "�ɤ���������ޤ���? ";
	s = "������ǽ�ʥ����ƥब�ʤ���";
#else
	q = "Wear/Wield which item? ";
	s = "You have nothing you can wear or wield.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* Check the slot */
	slot = wield_slot(o_ptr);
#if 1 /* EASY_RING -- TNB */

	if ((o_ptr->tval == TV_RING) && inventory[INVEN_LEFT].k_idx &&
		inventory[INVEN_RIGHT].k_idx)
	{
		/* Restrict the choices */
		item_tester_tval = TV_RING;
		item_tester_no_ryoute = TRUE;

		/* Choose a ring from the equipment only */
#ifdef JP
q = "�ɤ���λ��ؤȼ���ؤ��ޤ���?";
#else
		q = "Replace which ring? ";
#endif

#ifdef JP
s = "���äȡ�";
#else
		s = "Oops.";
#endif

		if (!get_item(&slot, q, s, (USE_EQUIP)))
			return;
	}

#endif /* EASY_RING -- TNB */

	if (((o_ptr->tval == TV_SHIELD) || (o_ptr->tval == TV_CARD) || (o_ptr->tval == TV_CAPTURE)) &&
		buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
	{
		/* Restrict the choices */
		item_tester_hook = item_tester_hook_melee_weapon;
		item_tester_no_ryoute = TRUE;

		/* Choose a weapon from the equipment only */
#ifdef JP
q = "�ɤ�������ȼ���ؤ��ޤ���?";
#else
		q = "Replace which weapon? ";
#endif

#ifdef JP
s = "���äȡ�";
#else
		s = "Oops.";
#endif

		if (!get_item(&slot, q, s, (USE_EQUIP)))
			return;
		if ((slot == INVEN_RARM) && !cursed_p(&inventory[INVEN_RARM]))
		{
			object_type *or_ptr = &inventory[INVEN_RARM];
			object_type *ol_ptr = &inventory[INVEN_LARM];
			object_type *otmp_ptr;
			object_type object_tmp;
			char ol_name[MAX_NLEN];

			otmp_ptr = &object_tmp;

			object_desc(ol_name, ol_ptr, FALSE, 0);

			object_copy(otmp_ptr, ol_ptr);
			object_copy(ol_ptr, or_ptr);
			object_copy(or_ptr, otmp_ptr);
#ifdef JP
			msg_format("%s��%s�˹����ʤ�������", ol_name, left_hander ? "����" : "����");
#else
			msg_format("You wield %s at %s hand.", ol_name, left_hander ? "left" : "right");
#endif

			slot = INVEN_LARM;
		}
	}

	/* ����ή�ˤ��뤫�ɤ��� */
	if ((o_ptr->tval >= TV_DIGGING) && (o_ptr->tval <= TV_SWORD) && (slot == INVEN_LARM))
	{
#ifdef JP
		if (!get_check("����ή���襤�ޤ�����"))
#else
		if (!get_check("Dual wielding? "))
#endif
		{
			slot = INVEN_RARM;
		}
	}

	if ((o_ptr->tval >= TV_DIGGING) && (o_ptr->tval <= TV_SWORD) &&
	    inventory[INVEN_LARM].k_idx &&
		inventory[INVEN_RARM].k_idx)
	{
		/* Restrict the choices */
		item_tester_hook = item_tester_hook_mochikae;

		/* Choose a ring from the equipment only */
#ifdef JP
q = "�ɤ���μ���������ޤ���?";
#else
		q = "Equip which hand? ";
#endif

#ifdef JP
s = "���äȡ�";
#else
		s = "Oops.";
#endif

		if (!get_item(&slot, q, s, (USE_EQUIP)))
			return;
	}

	/* Prevent wielding into a cursed slot */
	if (cursed_p(&inventory[slot]))
	{
		/* Describe it */
		object_desc(o_name, &inventory[slot], FALSE, 0);

		/* Message */
#ifdef JP
		msg_format("%s%s�ϼ����Ƥ���褦����",
			   describe_use(slot) , o_name );
#else
		msg_format("The %s you are %s appears to be cursed.",
			   o_name, describe_use(slot));
#endif


		/* Cancel the command */
		return;
	}

	if (cursed_p(o_ptr) && confirm_wear &&
	    (object_known_p(o_ptr) || (o_ptr->ident & IDENT_SENSE)))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, FALSE, 0);

#ifdef JP
sprintf(dummy, "������%s{�����Ƥ���}��Ȥ��ޤ�����", o_name);
#else
		sprintf(dummy, "Really use the %s {cursed}? ", o_name);
#endif


		if (!get_check(dummy))
			return;
	}

	if ((o_ptr->name1 == ART_STONEMASK) && object_known_p(o_ptr) && (p_ptr->prace != RACE_VAMPIRE) && (p_ptr->prace != RACE_ANDROID))
	{
		char dummy[MAX_NLEN+80];

		/* Describe it */
		object_desc(o_name, o_ptr, FALSE, 0);

#ifdef JP
sprintf(dummy, "%s����������ȵ۷쵴�ˤʤ�ޤ���������Ǥ�����", o_name);
#else
		msg_format("%s will transforms you into a vampire permanently when equiped.", o_name);
		sprintf(dummy, "Do you become a vampire?");
#endif


		if (!get_check(dummy))
			return;
	}

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
			msg_print("You completed the quest!");
#endif

			msg_print(NULL);
		}
	}

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) identify_item(o_ptr);

	/* Take a turn */
	energy_use = 100;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = 1;

	/* Decrease the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -1);
		inven_item_optimize(item);
	}

	/* Decrease the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -1);
		floor_item_optimize(0 - item);
	}

	/* Access the wield slot */
	o_ptr = &inventory[slot];

	/* Take off existing item */
	if (o_ptr->k_idx)
	{
		/* Take off existing item */
		(void)inven_takeoff(slot, 255);
	}

	/* Wear the new stuff */
	object_copy(o_ptr, q_ptr);

	/* Increase the weight */
	p_ptr->total_weight += q_ptr->weight;

	/* Increment the equip counter by hand */
	equip_cnt++;

	/* Where is the item now */
	if (slot == INVEN_RARM)
	{
		if((o_ptr->tval != TV_SHIELD) && (o_ptr->tval != TV_CAPTURE) && (o_ptr->tval != TV_CARD) && (empty_hands(FALSE) & 0x00000001) && ((o_ptr->weight > 99) || (o_ptr->tval == TV_POLEARM)) && (!p_ptr->riding || (p_ptr->pet_extra_flags & PF_RYOUTE)))
#ifdef JP
			act = "��ξ��ǹ�����";
#else
			act = "You are wielding";
#endif
		else
#ifdef JP
			act = (left_hander ? "�򺸼����������" : "�򱦼����������");
#else
			act = "You are wielding";
#endif

	}
	else if (slot == INVEN_LARM)
	{
#ifdef JP
		act = (left_hander ? "�򱦼����������" : "�򺸼����������");
#else
		act = "You are wielding";
#endif

	}
	else if (slot == INVEN_BOW)
	{
#ifdef JP
		act = "��ͷ��Ѥ���������";
#else
		act = "You are shooting with";
#endif

	}
	else if (slot == INVEN_LITE)
	{
#ifdef JP
		act = "������ˤ���";
#else
		act = "Your light source is";
#endif

	}
	else
	{
#ifdef JP
		act = "����������";
#else
		act = "You are wearing";
#endif

	}

	/* Describe the result */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Message */
#ifdef JP
	msg_format("%s(%c)%s��", o_name, index_to_label(slot), act );
#else
	msg_format("%s %s (%c).", act, o_name, index_to_label(slot));
#endif


	/* Cursed! */
	if (cursed_p(o_ptr))
	{
		/* Warn the player */
#ifdef JP
		msg_print("��� �����ޤ����䤿����");
#else
		msg_print("Oops! It feels deathly cold!");
#endif


		chg_virtue(V_HARMONY, -1);

		/* Note the curse */
		o_ptr->ident |= (IDENT_SENSE);
	}

	if ((o_ptr->name1 == ART_STONEMASK) && (p_ptr->prace != RACE_VAMPIRE) && (p_ptr->prace != RACE_ANDROID))
	{
		int h_percent;
		if (p_ptr->prace < 32)
		{
			p_ptr->old_race1 |= 1L << p_ptr->prace;
		}
		else
		{
			p_ptr->old_race2 |= 1L << (p_ptr->prace-32);
		}
		p_ptr->prace = RACE_VAMPIRE;
#ifdef JP
		msg_format("���ʤ��ϵ۷쵴���Ѳ�������");
#else
		msg_format("You polymorphed into a vampire!");
#endif

		rp_ptr = &race_info[p_ptr->prace];

		/* Experience factor */
		p_ptr->expfact = rp_ptr->r_exp + cp_ptr->c_exp;

		/* Calculate the height/weight for males */
		if (p_ptr->psex == SEX_MALE)
		{
			p_ptr->ht = randnor(rp_ptr->m_b_ht, rp_ptr->m_m_ht);
			h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->m_b_ht);
			p_ptr->wt = randnor((int)(rp_ptr->m_b_wt) * h_percent /100
					    , (int)(rp_ptr->m_m_wt) * h_percent / 300 );
		}

		/* Calculate the height/weight for females */
		else if (p_ptr->psex == SEX_FEMALE)
		{
			p_ptr->ht = randnor(rp_ptr->f_b_ht, rp_ptr->f_m_ht);
			h_percent = (int)(p_ptr->ht) * 100 / (int)(rp_ptr->f_b_ht);
			p_ptr->wt = randnor((int)(rp_ptr->f_b_wt) * h_percent /100
					    , (int)(rp_ptr->f_m_wt) * h_percent / 300 );
		}

		check_experience();

		/* Hitdice */
		if (p_ptr->pclass == CLASS_SORCERER)
			p_ptr->hitdie = rp_ptr->r_mhp/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
		else
			p_ptr->hitdie = rp_ptr->r_mhp + cp_ptr->c_mhp + ap_ptr->a_mhp;

		do_cmd_rerate(FALSE);

		p_ptr->redraw |= (PR_BASIC);

		p_ptr->update |= (PU_BONUS);

		handle_stuff();
		lite_spot(py, px);
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);

	/* Recalculate mana */
	p_ptr->update |= (PU_MANA);

	p_ptr->redraw |= (PR_EQUIPPY);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER);

	calc_android_exp();
}


void kamaenaoshi(int item)
{
	object_type *o_ptr, *o2_ptr;
	char o_name[MAX_NLEN];

	if ((item == INVEN_RARM) && buki_motteruka(INVEN_LARM))
	{
		o_ptr = &inventory[INVEN_RARM];
		o2_ptr = &inventory[INVEN_LARM];
		object_copy(o_ptr, o2_ptr);
		p_ptr->total_weight += o2_ptr->weight;
		inven_item_increase(INVEN_LARM,-1);
		inven_item_optimize(INVEN_LARM);
		object_desc(o_name, o_ptr, TRUE, 3);
		if (((o_ptr->weight > 99) || (o_ptr->tval == TV_POLEARM)) && (!p_ptr->riding || (p_ptr->pet_extra_flags & PF_RYOUTE)))
#ifdef JP
			msg_format("%s��ξ��ǹ�������", o_name );
#else
			msg_format("You are wielding %s with two-handed.", o_name );
#endif
		 else
#ifdef JP
			msg_format("%s��%s�ǹ�������", o_name, (left_hander ? "����" : "����"));
#else
			msg_format("You are wielding %s with %s hand.", o_name, (left_hander ? "left":"right") );
#endif
	}
	else if ((item == INVEN_LARM) && buki_motteruka(INVEN_RARM))
	{
		o_ptr = &inventory[INVEN_RARM];
		object_desc(o_name, o_ptr, TRUE, 3);
		if (((o_ptr->weight > 99) || (o_ptr->tval == TV_POLEARM)) && (!p_ptr->riding || (p_ptr->pet_extra_flags & PF_RYOUTE)))
#ifdef JP
			msg_format("%s��ξ��ǹ�������", o_name );
#else
			msg_format("You are wielding %s with two-handed.", o_name );
#endif
	}
	else if ((item == INVEN_LARM) && !(empty_hands(FALSE) & 0x0002))
	{
		o_ptr = &inventory[INVEN_LARM];
		o2_ptr = &inventory[INVEN_RARM];
		object_copy(o_ptr, o2_ptr);
		p_ptr->total_weight += o2_ptr->weight;
		inven_item_increase(INVEN_RARM,-1);
		inven_item_optimize(INVEN_RARM);
		object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
		msg_format("%s������ؤ�����", o_name );
#else
		msg_format("You switched hand of %s.", o_name );
#endif
	}
}


/*
 * Take off an item
 */
void do_cmd_takeoff(void)
{
	int item;

	object_type *o_ptr;

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ɤ����������Ϥ����ޤ���? ";
	s = "�Ϥ������������ʤ���";
#else
	q = "Take off which item? ";
	s = "You are not wearing anything to take off.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP))) return;

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


	/* Item is cursed */
	if (cursed_p(o_ptr))
	{
		if ((o_ptr->curse_flags & TRC_PERMA_CURSE) || (p_ptr->pclass != CLASS_BERSERKER))
		{
			/* Oops */
#ifdef JP
			msg_print("�ա��ࡢ�ɤ��������Ƥ���褦����");
#else
			msg_print("Hmmm, it seems to be cursed.");
#endif

			/* Nope */
			return;
		}

		if (((o_ptr->curse_flags & TRC_HEAVY_CURSE) && one_in_(7)) || one_in_(4))
		{
#ifdef JP
			msg_print("����줿�������ϤŤ�����������");
#else
			msg_print("You teared a cursed equipment off by sheer strength!");
#endif

			/* Hack -- Assume felt */
			o_ptr->ident |= (IDENT_SENSE);

			o_ptr->curse_flags = 0L;

			/* Take note */
			o_ptr->feeling = FEEL_NONE;

			/* Recalculate the bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_EQUIP);

#ifdef JP
			msg_print("�������Ǥ��ˤä���");
#else
			msg_print("You break the curse.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("�����򳰤��ʤ��ä���");
#else
			msg_print("You couldn't remove the equipment.");
#endif
			energy_use = 50;
			return;
		}
	}

	/* Take a partial turn */
	energy_use = 50;

	/* Take off the item */
	(void)inven_takeoff(item, 255);

	kamaenaoshi(item);

	calc_android_exp();

	p_ptr->redraw |= (PR_EQUIPPY);
}


/*
 * Drop an item
 */
void do_cmd_drop(void)
{
	int item, amt = 1;

	object_type *o_ptr;

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ����Ȥ��ޤ���? ";
	s = "��Ȥ��륢���ƥ����äƤ��ʤ���";
#else
	q = "Drop which item? ";
	s = "You have nothing to drop.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return;

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


	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_RARM) && cursed_p(o_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("�ա��ࡢ�ɤ��������Ƥ���褦����");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif


		/* Nope */
		return;
	}


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Take a partial turn */
	energy_use = 50;

	/* Drop (some of) the item */
	inven_drop(item, amt);

	if ((item == INVEN_RARM) || (item == INVEN_LARM)) kamaenaoshi(item);

	if (item >= INVEN_RARM) calc_android_exp();

	p_ptr->redraw |= (PR_EQUIPPY);
}


static bool high_level_book(object_type *o_ptr)
{
	if ((o_ptr->tval == TV_LIFE_BOOK) ||
	    (o_ptr->tval == TV_SORCERY_BOOK) ||
	    (o_ptr->tval == TV_NATURE_BOOK) ||
	    (o_ptr->tval == TV_CHAOS_BOOK) ||
	    (o_ptr->tval == TV_DEATH_BOOK) ||
	    (o_ptr->tval == TV_TRUMP_BOOK) ||
	    (o_ptr->tval == TV_ENCHANT_BOOK) ||
	    (o_ptr->tval == TV_DAEMON_BOOK) ||
	    (o_ptr->tval == TV_CRUSADE_BOOK) ||
	    (o_ptr->tval == TV_MUSIC_BOOK))
	{
		if (o_ptr->sval > 1)
			return TRUE;
		else
			return FALSE;
	}

	return FALSE;
}


/*
 * Destroy an item
 */
void do_cmd_destroy(void)
{
	int			item, amt = 1;
	int			old_number;

	bool		force = FALSE;

	object_type		*o_ptr;
	object_type             forge;
	object_type             *q_ptr = &forge;

	char		o_name[MAX_NLEN];

	char		out_val[MAX_NLEN+40];

	cptr q, s;

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* Hack -- force destruction */
	if (command_arg > 0) force = TRUE;


	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ������ޤ���? ";
	s = "�����륢���ƥ����äƤ��ʤ���";
#else
	q = "Destroy which item? ";
	s = "You have nothing to destroy.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* See how many items */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}


	/* Describe the object */
	old_number = o_ptr->number;
	o_ptr->number = amt;
	object_desc(o_name, o_ptr, TRUE, 3);
	o_ptr->number = old_number;

	/* Verify unless quantity given */
	if (!force)
	{
		if (confirm_destroy || (object_value(o_ptr) > 0))
		{
			/* Make a verification */
#ifdef JP
		sprintf(out_val, "������%s������ޤ���? ", o_name);
#else
			sprintf(out_val, "Really destroy %s? ", o_name);
#endif

			if (!get_check(out_val)) return;
		}
	}

	/* Take a turn */
	energy_use = 100;

	/* Artifacts cannot be destroyed */
	if (!can_player_destroy_object(o_ptr))
	{
		energy_use = 0;

		/* Message */
#ifdef JP
		msg_format("%s���˲��Բ�ǽ����", o_name);
#else
		msg_format("You cannot destroy %s.", o_name);
#endif

		/* Done */
		return;
	}

	object_copy(q_ptr, o_ptr);

	/* Message */
#ifdef JP
	msg_format("%s���������", o_name);
#else
	msg_format("You destroy %s.", o_name);
#endif

	sound(SOUND_DESTITEM);

	/* Reduce the charges of rods/wands */
	reduce_charges(o_ptr, amt);

	/* Eliminate the item (from the pack) */
	if (item >= 0)
	{
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);
	}

	/* Eliminate the item (from the floor) */
	else
	{
		floor_item_increase(0 - item, -amt);
		floor_item_describe(0 - item);
		floor_item_optimize(0 - item);
	}

	if (high_level_book(q_ptr))
	{
		bool gain_expr = FALSE;

		if (p_ptr->prace == RACE_ANDROID)
		{
		}
		else if ((p_ptr->pclass == CLASS_WARRIOR) || (p_ptr->pclass == CLASS_BERSERKER))
		{
			gain_expr = TRUE;
		}
		else if (p_ptr->pclass == CLASS_PALADIN)
		{
			if (is_good_realm(p_ptr->realm1))
			{
				if (!is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
			else
			{
				if (is_good_realm(tval2realm(q_ptr->tval))) gain_expr = TRUE;
			}
		}

		if (gain_expr && (p_ptr->exp < PY_MAX_EXP))
		{
			s32b tester_exp = p_ptr->max_exp / 20;
			if (tester_exp > 10000) tester_exp = 10000;
			if (q_ptr->sval < 3) tester_exp /= 4;
			if (tester_exp<1) tester_exp = 1;

#ifdef JP
msg_print("���˷и����Ѥ���褦�ʵ������롣");
#else
			msg_print("You feel more experienced.");
#endif

			gain_exp(tester_exp * amt);
		}
		if (high_level_book(q_ptr) && q_ptr->tval == TV_LIFE_BOOK)
		{
			chg_virtue(V_UNLIFE, 1);
			chg_virtue(V_VITALITY, -1);
		}
		else if (high_level_book(q_ptr) && q_ptr->tval == TV_DEATH_BOOK)
		{
			chg_virtue(V_UNLIFE, -1);
			chg_virtue(V_VITALITY, 1);
		}
	
		if (q_ptr->to_a || q_ptr->to_h || q_ptr->to_d)
			chg_virtue(V_ENCHANT, -1);
	
		if (object_value_real(q_ptr) > 30000)
			chg_virtue(V_SACRIFICE, 2);
	
		else if (object_value_real(q_ptr) > 10000)
			chg_virtue(V_SACRIFICE, 1);
	}

	if (q_ptr->to_a != 0 || q_ptr->to_d != 0 || q_ptr->to_h != 0)
		chg_virtue(V_HARMONY, 1);

	if (item >= INVEN_RARM) calc_android_exp();
}


/*
 * Observe an item which has been *identify*-ed
 */
void do_cmd_observe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ��Ĵ�٤ޤ���? ";
	s = "Ĵ�٤��륢���ƥब�ʤ���";
#else
	q = "Examine which item? ";
	s = "You have nothing to examine.";
#endif

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


	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
#ifdef JP
		msg_print("���Υ����ƥ�ˤĤ����ä��ΤäƤ��뤳�ȤϤʤ���");
#else
		msg_print("You have no special knowledge about that item.");
#endif

		return;
	}


	/* Description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Describe */
#ifdef JP
	msg_format("%s��Ĵ�٤Ƥ���...", o_name);
#else
	msg_format("Examining %s...", o_name);
#endif

	/* Describe it fully */
#ifdef JP
	if (!screen_object(o_ptr, TRUE)) msg_print("�ä��Ѥ�ä��Ȥ���Ϥʤ��褦����");
#else
	if (!screen_object(o_ptr, TRUE)) msg_print("You see nothing special.");
#endif

}



/*
 * Remove the inscription from an object
 * XXX Mention item (when done)?
 */
void do_cmd_uninscribe(void)
{
	int   item;

	object_type *o_ptr;

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ���ä�ä��ޤ���? ";
	s = "�ä�ä��륢���ƥब�ʤ���";
#else
	q = "Un-inscribe which item? ";
	s = "You have nothing to un-inscribe.";
#endif

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

	/* Nothing to remove */
	if (!o_ptr->inscription)
	{
#ifdef JP
		msg_print("���Υ����ƥ�ˤϾä��٤��ä��ʤ���");
#else
		msg_print("That item had no inscription to remove.");
#endif

		return;
	}

	/* Message */
#ifdef JP
	msg_print("�ä�ä�����");
#else
	msg_print("Inscription removed.");
#endif


	/* Remove the incription */
	o_ptr->inscription = 0;

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP);

	/* .��$�δط���, �Ʒ׻���ɬ�פʤϤ� -- henkma */
	p_ptr->update |= (PU_BONUS);

}


/*
 * Inscribe an object with a comment
 */
void do_cmd_inscribe(void)
{
	int			item;

	object_type		*o_ptr;

	char		o_name[MAX_NLEN];

	char		out_val[80];

	cptr q, s;

	item_tester_no_ryoute = TRUE;
	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ���ä��ߤޤ���? ";
	s = "�ä���륢���ƥब�ʤ���";
#else
	q = "Inscribe which item? ";
	s = "You have nothing to inscribe.";
#endif

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

	/* Describe the activity */
	object_desc(o_name, o_ptr, TRUE, 2);

	/* Message */
#ifdef JP
	msg_format("%s���ä��ࡣ", o_name);
#else
	msg_format("Inscribing %s.", o_name);
#endif

	msg_print(NULL);

	/* Start with nothing */
	strcpy(out_val, "");

	/* Use old inscription */
	if (o_ptr->inscription)
	{
		/* Start with the old inscription */
		strcpy(out_val, quark_str(o_ptr->inscription));
	}

	/* Get a new inscription (possibly empty) */
#ifdef JP
	if (get_string("��: ", out_val, 80))
#else
	if (get_string("Inscription: ", out_val, 80))
#endif
	{
		/* Save the inscription */
		o_ptr->inscription = quark_add(out_val);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* .��$�δط���, �Ʒ׻���ɬ�פʤϤ� -- henkma */
		p_ptr->update |= (PU_BONUS);
	}
}



/*
 * An "item_tester_hook" for refilling lanterns
 */
static bool item_tester_refill_lantern(object_type *o_ptr)
{
	/* Flasks of oil are okay */
	if (o_ptr->tval == TV_FLASK) return (TRUE);

	/* Laterns are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_LANTERN)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refill the players lamp (from the pack or floor)
 */
static void do_cmd_refill_lamp(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_lantern;

	/* Get an item */
#ifdef JP
	q = "�ɤ����Ĥܤ������ޤ���? ";
	s = "���Ĥܤ��ʤ���";
#else
	q = "Refill with which flask? ";
	s = "You have no flasks of oil.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the lantern */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4;

	/* Message */
#ifdef JP
	msg_print("���פ�����������");
#else
	msg_print("You fuel your lamp.");
#endif

	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("���פ��ä��Ƥ��ޤä���");
#else
		msg_print("Your lamp has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("���������פ���������ʤ���");
#else
		msg_print("Curiously, your lamp doesn't light.");
#endif
	}
	else if (j_ptr->xtra4 >= FUEL_LAMP)
	{
		j_ptr->xtra4 = FUEL_LAMP;
#ifdef JP
		msg_print("���פ����ϰ��դ���");
#else
		msg_print("Your lamp is full.");
#endif

	}

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

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}


/*
 * An "item_tester_hook" for refilling torches
 */
static bool item_tester_refill_torch(object_type *o_ptr)
{
	/* Torches are okay */
	if ((o_ptr->tval == TV_LITE) &&
	    (o_ptr->sval == SV_LITE_TORCH)) return (TRUE);

	/* Assume not okay */
	return (FALSE);
}


/*
 * Refuel the players torch (from the pack or floor)
 */
static void do_cmd_refill_torch(void)
{
	int item;

	object_type *o_ptr;
	object_type *j_ptr;

	cptr q, s;


	/* Restrict the choices */
	item_tester_hook = item_tester_refill_torch;

	/* Get an item */
#ifdef JP
	q = "�ɤξ�����������򶯤�ޤ���? ";
	s = "¾�˾������ʤ���";
#else
	q = "Refuel with which torch? ";
	s = "You have no extra torches.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

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


	/* Take a partial turn */
	energy_use = 50;

	/* Access the primary torch */
	j_ptr = &inventory[INVEN_LITE];

	/* Refuel */
	j_ptr->xtra4 += o_ptr->xtra4 + 5;

	/* Message */
#ifdef JP
	msg_print("�������礷����");
#else
	msg_print("You combine the torches.");
#endif


	/* Comment */
	if ((o_ptr->name2 == EGO_LITE_DARKNESS) && (j_ptr->xtra4 > 0))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("�������ä��Ƥ��ޤä���");
#else
		msg_print("Your torch has gone out!");
#endif
	}
	else if ((o_ptr->name2 == EGO_LITE_DARKNESS) || (j_ptr->name2 == EGO_LITE_DARKNESS))
	{
		j_ptr->xtra4 = 0;
#ifdef JP
		msg_print("��������������������ʤ���");
#else
		msg_print("Curiously, your torche don't light.");
#endif
	}
	/* Over-fuel message */
	else if (j_ptr->xtra4 >= FUEL_TORCH)
	{
		j_ptr->xtra4 = FUEL_TORCH;
#ifdef JP
		msg_print("�����μ�̿�Ͻ�ʬ����");
#else
		msg_print("Your torch is fully fueled.");
#endif

	}

	/* Refuel message */
	else
	{
#ifdef JP
		msg_print("�����Ϥ��ä������뤯��������");
#else
		msg_print("Your torch glows more brightly.");
#endif

	}

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

	/* Recalculate torch */
	p_ptr->update |= (PU_TORCH);
}


/*
 * Refill the players lamp, or restock his torches
 */
void do_cmd_refill(void)
{
	object_type *o_ptr;

	/* Get the light */
	o_ptr = &inventory[INVEN_LITE];

	if (p_ptr->special_defense & KATA_MUSOU)
	{
		set_action(ACTION_NONE);
	}

	/* It is nothing */
	if (o_ptr->tval != TV_LITE)
	{
#ifdef JP
		msg_print("�������������Ƥ��ʤ���");
#else
		msg_print("You are not wielding a light.");
#endif

	}

	/* It's a lamp */
	else if (o_ptr->sval == SV_LITE_LANTERN)
	{
		do_cmd_refill_lamp();
	}

	/* It's a torch */
	else if (o_ptr->sval == SV_LITE_TORCH)
	{
		do_cmd_refill_torch();
	}

	/* No torch to refill */
	else
	{
#ifdef JP
		msg_print("���θ����ϼ�̿���Ф��ʤ���");
#else
		msg_print("Your light cannot be refilled.");
#endif

	}
}


/*
 * Target command
 */
void do_cmd_target(void)
{
	/* Target set */
	if (target_set(TARGET_KILL))
	{
#ifdef JP
		msg_print("�������åȷ��ꡣ");
#else
		msg_print("Target Selected.");
#endif

	}

	/* Target aborted */
	else
	{
#ifdef JP
		msg_print("�������åȲ����");
#else
		msg_print("Target Aborted.");
#endif

	}
}



/*
 * Look command
 */
void do_cmd_look(void)
{
	/* Look around */
	if (target_set(TARGET_LOOK))
	{
#ifdef JP
		msg_print("�������åȷ��ꡣ");
#else
		msg_print("Target Selected.");
#endif

	}
}



/*
 * Allow the player to examine other sectors on the map
 */
void do_cmd_locate(void)
{
	int		dir, y1, x1, y2, x2;

	char	tmp_val[80];

	char	out_val[160];

	int wid, hgt;

	/* Get size */
	get_screen_size(&wid, &hgt);


	/* Start at current panel */
	y2 = y1 = panel_row_min;
	x2 = x1 = panel_col_min;

	/* Show panels until done */
	while (1)
	{
		/* Describe the location */
		if ((y2 == y1) && (x2 == x1))
		{
#ifdef JP
			strcpy(tmp_val, "����");
#else
			tmp_val[0] = '\0';
#endif

		}
		else
		{
#ifdef JP
			sprintf(tmp_val, "%s%s",
				((y2 < y1) ? "��" : (y2 > y1) ? "��" : ""),
				((x2 < x1) ? "��" : (x2 > x1) ? "��" : ""));
#else
			sprintf(tmp_val, "%s%s of",
				((y2 < y1) ? " North" : (y2 > y1) ? " South" : ""),
				((x2 < x1) ? " West" : (x2 > x1) ? " East" : ""));
#endif

		}

		/* Prepare to ask which way to look */
		sprintf(out_val,
#ifdef JP
			"�ޥåװ��� [%d(%02d),%d(%02d)] (�ץ쥤�䡼��%s)  ����?",
#else
			"Map sector [%d(%02d),%d(%02d)], which is%s your sector.  Direction?",
#endif

			y2 / (hgt / 2), y2 % (hgt / 2),
			x2 / (wid / 2), x2 % (wid / 2), tmp_val);

		/* Assume no direction */
		dir = 0;

		/* Get a direction */
		while (!dir)
		{
			char command;

			/* Get a command (or Cancel) */
			if (!get_com(out_val, &command, TRUE)) break;

			/* Extract the action (if any) */
			dir = get_keymap_dir(command);

			/* Error */
			if (!dir) bell();
		}

		/* No direction */
		if (!dir) break;

		/* Apply the motion */
		if (change_panel(ddy[dir], ddx[dir]))
		{
			y2 = panel_row_min;
			x2 = panel_col_min;
		}
	}


	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Handle stuff */
	handle_stuff();
}



/*
 * The table of "symbol info" -- each entry is a string of the form
 * "X:desc" where "X" is the trigger, and "desc" is the "info".
 */
static cptr ident_info[] =
{
#ifdef JP
	" :�Ű�",
	"!:��, ������",
	"\":���ߥ��å�, ������",
	"#:��(�����ɥ�)/��ʪ/����",
	"$:����(�⤫����)",
	"%:��̮(�ϴ䤫�б�)",
	"&:Ȣ",
	"':�������ɥ�",
	"(:��餫���ɶ�",
	"):��",
	"*:������ޤ����̮�ޤ��ϵ���β�ʪ",
	"+:�Ĥ����ɥ�",
	",:����ʪ, ���Ф����Υ�",
	"-:��ˡ��, ��å�",
	".:��",
	"/:�Ⱦ����(���å���/�ѥ���/��)",
	"0:��ʪ�ۤ�����",
	"1:���߲�������",
	"2:�ɶ񲰤�����",
	"3:�������Ź������",
	"4:����������",
	"5:ϣ��Ѥ�Ź������",
	"6:��ˡ��Ź������",
	"7:�֥�å��ޡ����åȤ�����",
	"8:�椬�Ȥ�����",
	"9:��Ź������",
	"::����",
	";:�����Ħ��/��ȯ�Υ롼��",
	"<:��곬��",
	"=:����",
	">:���곬��",
	"?:��ʪ",
	"@:�ץ쥤�䡼",
	"A:ŷ��",
	"B:Ļ",
	"C:��",
	"D:����ɥ饴��/�磻������",
	"E:�����󥿥�",
	"F:�ȥ��",
	"G:��������",
	"H:����",
	"I:����",
	"J:�إ�",
	"K:���顼���ӡ��ȥ�",
	"L:��å�",
	"M:¿��������",
	"N:�����ʪ",
	"O:������",
	"P:����ʹַ���ʪ",
	"Q:�����륹�륰(̮�Ǥ�����)",
	"R:�����/ξ����",
	"S:����/������/����",
	"T:�ȥ��",
	"U:���ǡ����",
	"V:�Х�ѥ���",
	"W:�磻��/�쥤��/��",
	"X:������/�����/��",
	"Y:�����ƥ�",
	"Z:�ϥ����",
	"[:���������ޡ�",
	"\\:�ߴ�(�ᥤ��/���/��)",
	"]:����ɶ�",
	"^:�ȥ�å�",
	"_:��",
	"`:�ͷ���Ħ��",
	"a:����",
	"b:�������",
	"c:�५��",
	"d:�ɥ饴��",
	"e:�ܶ�",
	"f:�ͥ�",
	"g:�������",
	"h:�ۥӥå�/�����/�ɥ��",
	"i:�٥ȥ٥�",
	"j:���꡼",
	"k:���ܥ��",
	"l:������ʪ",
	"m:����",
	"n:�ʡ���",
	"o:������",
	"p:�ʹ�",
	"q:��­��",
	"r:�ͥ���",
	"s:������ȥ�",
	"t:Į�ο�",
	"u:����ǡ����",
	"v:�ܥ�ƥå���",
	"w:����ॷ/�緲",
	/* "x:unused", */
	"y:������",
	"z:�����/�ߥ���",
	"{:����ƻ�����(��/��)",
	"|:�����(������/������/��)",
	"}:����ƻ��(��/�����ܥ�/�����)",
	"~:��/�ϴ�ή(��Υ����ƥ�)",
#else
	" :A dark grid",
	"!:A potion (or oil)",
	"\":An amulet (or necklace)",
	"#:A wall (or secret door) / a plant / a gas",
	"$:Treasure (gold or gems)",
	"%:A vein (magma or quartz)",
	"&:A chest",
	"':An open door",
	"(:Soft armor",
	"):A shield",
	"*:A vein with treasure or a ball monster",
	"+:A closed door",
	",:Food (or mushroom patch)",
	"-:A wand (or rod)",
	".:Floor",
	"/:A polearm (Axe/Pike/etc)",
	"0:Entrance to Museum",
	"1:Entrance to General Store",
	"2:Entrance to Armory",
	"3:Entrance to Weaponsmith",
	"4:Entrance to Temple",
	"5:Entrance to Alchemy shop",
	"6:Entrance to Magic store",
	"7:Entrance to Black Market",
	"8:Entrance to your home",
	"9:Entrance to the bookstore",
	"::Rubble",
	";:A glyph of warding / an explosive rune",
	"<:An up staircase",
	"=:A ring",
	">:A down staircase",
	"?:A scroll",
	"@:You",
	"A:Angel",
	"B:Bird",
	"C:Canine",
	"D:Ancient Dragon/Wyrm",
	"E:Elemental",
	"F:Dragon Fly",
	"G:Ghost",
	"H:Hybrid",
	"I:Insect",
	"J:Snake",
	"K:Killer Beetle",
	"L:Lich",
	"M:Multi-Headed Reptile",
	"N:Mystery Living",
	"O:Ogre",
	"P:Giant Humanoid",
	"Q:Quylthulg (Pulsing Flesh Mound)",
	"R:Reptile/Amphibian",
	"S:Spider/Scorpion/Tick",
	"T:Troll",
	"U:Major Demon",
	"V:Vampire",
	"W:Wight/Wraith/etc",
	"X:Xorn/Xaren/etc",
	"Y:Yeti",
	"Z:Zephyr Hound",
	"[:Hard armor",
	"\\:A hafted weapon (mace/whip/etc)",
	"]:Misc. armor",
	"^:A trap",
	"_:A staff",
	"`:A figurine or statue",
	"a:Ant",
	"b:Bat",
	"c:Centipede",
	"d:Dragon",
	"e:Floating Eye",
	"f:Feline",
	"g:Golem",
	"h:Hobbit/Elf/Dwarf",
	"i:Icky Thing",
	"j:Jelly",
	"k:Kobold",
	"l:Aquatic monster",
	"m:Mold",
	"n:Naga",
	"o:Orc",
	"p:Person/Human",
	"q:Quadruped",
	"r:Rodent",
	"s:Skeleton",
	"t:Townsperson",
	"u:Minor Demon",
	"v:Vortex",
	"w:Worm/Worm-Mass",
	/* "x:unused", */
	"y:Yeek",
	"z:Zombie/Mummy",
	"{:A missile (arrow/bolt/shot)",
	"|:An edged weapon (sword/dagger/etc)",
	"}:A launcher (bow/crossbow/sling)",
	"~:Fluid terrain (or miscellaneous item)",
#endif

	NULL
};


/*
 * Sorting hook -- Comp function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform on "u".
 */
bool ang_sort_comp_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b *why = (u16b*)(v);

	int w1 = who[a];
	int w2 = who[b];

	int z1, z2;


	/* Sort by player kills */
	if (*why >= 4)
	{
		/* Extract player kills */
		z1 = r_info[w1].r_pkills;
		z2 = r_info[w2].r_pkills;

		/* Compare player kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by total kills */
	if (*why >= 3)
	{
		/* Extract total kills */
		z1 = r_info[w1].r_tkills;
		z2 = r_info[w2].r_tkills;

		/* Compare total kills */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster level */
	if (*why >= 2)
	{
		/* Extract levels */
		z1 = r_info[w1].level;
		z2 = r_info[w2].level;

		/* Compare levels */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Sort by monster experience */
	if (*why >= 1)
	{
		/* Extract experience */
		z1 = r_info[w1].mexp;
		z2 = r_info[w2].mexp;

		/* Compare experience */
		if (z1 < z2) return (TRUE);
		if (z1 > z2) return (FALSE);
	}


	/* Compare indexes */
	return (w1 <= w2);
}


/*
 * Sorting hook -- Swap function -- see below
 *
 * We use "u" to point to array of monster indexes,
 * and "v" to select the type of sorting to perform.
 */
void ang_sort_swap_hook(vptr u, vptr v, int a, int b)
{
	u16b *who = (u16b*)(u);

	u16b holder;

	/* Swap */
	holder = who[a];
	who[a] = who[b];
	who[b] = holder;
}



/*
 * Identify a character, allow recall of monsters
 *
 * Several "special" responses recall "multiple" monsters:
 *   ^A (all monsters)
 *   ^U (all unique monsters)
 *   ^N (all non-unique monsters)
 *
 * The responses may be sorted in several ways, see below.
 *
 * Note that the player ghosts are ignored. XXX XXX XXX
 */
void do_cmd_query_symbol(void)
{
	int		i, n, r_idx;
	char	sym, query;
	char	buf[128];

	bool	all = FALSE;
	bool	uniq = FALSE;
	bool	norm = FALSE;
	char    temp[80] = "";

	bool	recall = FALSE;

	u16b	why = 0;
	u16b	*who;

	/* Get a character, or abort */
#ifdef JP
	if (!get_com("�Τꤿ��ʸ�������Ϥ��Ʋ�����(���� or ^A��,^U��,^N���,^M̾��): ", &sym, FALSE)) return;
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE)) return;
#endif


	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

	/* Describe */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "����󥹥����Υꥹ��");
#else
		strcpy(buf, "Full monster list.");
#endif

	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "��ˡ�������󥹥����Υꥹ��");
#else
		strcpy(buf, "Unique monster list.");
#endif

	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "��ˡ�������󥹥����Υꥹ��");
#else
		strcpy(buf, "Non-unique monster list.");
#endif

	}
	/* XTRA HACK WHATSEARCH */
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("̾��(�Ѹ�ξ�羮ʸ���ǲ�)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;
			return;
		}
#ifdef JP
		sprintf(buf, "̾��:%s�˥ޥå�",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
		sprintf(buf, "%c - %s", sym, "̵����ʸ��");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif

	}

	/* Display the result */
	prt(buf, 0, 0);

	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* Nothing to recall */
		if (!cheat_know && !r_ptr->r_sights) continue;

		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* XTRA HACK WHATSEARCH */
		if (temp[0]){
		  int xx;
		  char temp2[80];
  
		  for (xx=0; temp[xx] && xx<80; xx++){
#ifdef JP
		    if (iskanji( temp[xx])) { xx++; continue; }
#endif
		    if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
		  }
  
#ifdef JP
		  strcpy(temp2, r_name+r_ptr->E_name);
#else
		  strcpy(temp2, r_name+r_ptr->name);
#endif
		  for (xx=0; temp2[xx] && xx<80; xx++)
		    if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);
  
#ifdef JP
		  if (strstr(temp2, temp) || strstr_j(r_name + r_ptr->name, temp) )
#else
		  if (strstr(temp2, temp))
#endif
			  who[n++]=i;
		}else
		/* Collect "appropriate" monsters */
		if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		return;
	}


	/* Prompt XXX XXX XXX */
#ifdef JP
	put_str("�פ��Ф򸫤ޤ���? (k:������/y/n): ", 0, 36);
#else
	put_str("Recall details? (k/y/n): ", 0, 40);
#endif


	/* Query */
	query = inkey();

	/* Restore */
	prt(buf, 0, 0);

	why = 2;

	/* Select the sort method */
	ang_sort_comp = ang_sort_comp_hook;
	ang_sort_swap = ang_sort_swap_hook;

	/* Sort the array */
	ang_sort(who, &why, n);

	/* Sort by kills (and level) */
	if (query == 'k')
	{
		why = 4;
		query = 'y';
	}

	/* Catch "escape" */
	if (query != 'y')
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		return;
	}

	/* Sort if needed */
	if (why == 4)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	i = n - 1;

	/* Scan the monster memory */
	while (1)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Auto-recall */
		monster_race_track(r_idx);

		/* Hack -- Handle stuff */
		handle_stuff();

		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Save the screen */
				screen_save();

				/* Recall on screen */
				screen_roff(who[i], 0);
			}

			/* Hack -- Begin the prompt */
			roff_top(r_idx);

			/* Hack -- Complete the prompt */
#ifdef JP
			Term_addstr(-1, TERM_WHITE, " ['r'�פ���, ESC]");
#else
			Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC]");
#endif

			/* Command */
			query = inkey();

			/* Unrecall */
			if (recall)
			{
				/* Restore */
				screen_load();
			}

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Re-display the identity */
	prt(buf, 0, 0);
}


/*
 *  research_mon
 *  -KMW-
 */
bool research_mon(void)
{
	int i, n, r_idx;
	char sym, query;
	char buf[128];

	bool notpicked;

	bool recall = FALSE;

	u16b why = 0;

	u16b	*who;

	/* XTRA HACK WHATSEARCH */
	bool    all = FALSE;
	bool    uniq = FALSE;
	bool    norm = FALSE;
	char temp[80] = "";

	/* XTRA HACK REMEMBER_IDX */
	static int old_sym = '\0';
	static int old_i = 0;


	/* Save the screen */
	screen_save();

	/* Get a character, or abort */
#ifdef JP
if (!get_com("��󥹥�����ʸ�������Ϥ��Ʋ�����(���� or ^A��,^U��,^N���,^M̾��):", &sym, FALSE)) 
#else
	if (!get_com("Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): ", &sym, FALSE))
#endif

	{
		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Find that character info, and describe it */
	for (i = 0; ident_info[i]; ++i)
	{
		if (sym == ident_info[i][0]) break;
	}

		/* XTRA HACK WHATSEARCH */
	if (sym == KTRL('A'))
	{
		all = TRUE;
#ifdef JP
		strcpy(buf, "����󥹥����Υꥹ��");
#else
		strcpy(buf, "Full monster list.");
#endif
	}
	else if (sym == KTRL('U'))
	{
		all = uniq = TRUE;
#ifdef JP
		strcpy(buf, "��ˡ�������󥹥����Υꥹ��");
#else
		strcpy(buf, "Unique monster list.");
#endif
	}
	else if (sym == KTRL('N'))
	{
		all = norm = TRUE;
#ifdef JP
		strcpy(buf, "��ˡ�������󥹥����Υꥹ��");
#else
		strcpy(buf, "Non-unique monster list.");
#endif
	}
	else if (sym == KTRL('M'))
	{
		all = TRUE;
#ifdef JP
		if (!get_string("̾��(�Ѹ�ξ�羮ʸ���ǲ�)",temp, 70))
#else
		if (!get_string("Enter name:",temp, 70))
#endif
		{
			temp[0]=0;

			/* Restore */
			screen_load();

			return FALSE;
		}
#ifdef JP
		sprintf(buf, "̾��:%s�˥ޥå�",temp);
#else
		sprintf(buf, "Monsters with a name \"%s\"",temp);
#endif
	}
	else if (ident_info[i])
	{
		sprintf(buf, "%c - %s.", sym, ident_info[i] + 2);
	}
	else
	{
#ifdef JP
sprintf(buf, "%c - %s", sym, "̵����ʸ��");
#else
		sprintf(buf, "%c - %s.", sym, "Unknown Symbol");
#endif

	}

	/* Display the result */
	prt(buf, 16, 10);


	/* Allocate the "who" array */
	C_MAKE(who, max_r_idx, u16b);

	/* Collect matching monsters */
	for (n = 0, i = 1; i < max_r_idx; i++)
	{
		monster_race *r_ptr = &r_info[i];

		/* XTRA HACK WHATSEARCH */
		/* Require non-unique monsters if needed */
		if (norm && (r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* Require unique monsters if needed */
		if (uniq && !(r_ptr->flags1 & (RF1_UNIQUE))) continue;

		/* ̾������ */
		if (temp[0]){
		  int xx;
		  char temp2[80];
  
		  for (xx=0; temp[xx] && xx<80; xx++){
#ifdef JP
		    if (iskanji( temp[xx])) { xx++; continue; }
#endif
		    if (isupper(temp[xx])) temp[xx]=tolower(temp[xx]);
		  }
  
#ifdef JP
		  strcpy(temp2, r_name+r_ptr->E_name);
#else
		  strcpy(temp2, r_name+r_ptr->name);
#endif
		  for (xx=0; temp2[xx] && xx<80; xx++)
		    if (isupper(temp2[xx])) temp2[xx]=tolower(temp2[xx]);
  
#ifdef JP
		  if (strstr(temp2, temp) || strstr_j(r_name + r_ptr->name, temp) )
#else
		  if (strstr(temp2, temp))
#endif
			  who[n++]=i;
		}
		else if (all || (r_ptr->d_char == sym)) who[n++] = i;
	}

	/* Nothing to recall */
	if (!n)
	{
		/* Free the "who" array */
		C_KILL(who, max_r_idx, u16b);

		/* Restore */
		screen_load();

		return (FALSE);
	}

	/* Sort by level */
	why = 2;
	query = 'y';

	/* Sort if needed */
	if (why)
	{
		/* Select the sort method */
		ang_sort_comp = ang_sort_comp_hook;
		ang_sort_swap = ang_sort_swap_hook;

		/* Sort the array */
		ang_sort(who, &why, n);
	}


	/* Start at the end */
	/* XTRA HACK REMEMBER_IDX */
	if (old_sym == sym && old_i < n) i = old_i;
	else i = n - 1;

	notpicked = TRUE;

	/* Scan the monster memory */
	while (notpicked)
	{
		/* Extract a race */
		r_idx = who[i];

		/* Hack -- Begin the prompt */
		roff_top(r_idx);

		/* Hack -- Complete the prompt */
#ifdef JP
Term_addstr(-1, TERM_WHITE, " ['r'�פ���, ' '��³��, ESC]");
#else
		Term_addstr(-1, TERM_WHITE, " [(r)ecall, ESC, space to continue]");
#endif


		/* Interact */
		while (1)
		{
			/* Recall */
			if (recall)
			{
				/* Recall on screen */
				monster_race *r_ptr = &r_info[r_idx];
				int m;

				/* Hack -- Maximal info */
				r_ptr->r_wake = r_ptr->r_ignore = MAX_UCHAR;
				
				/* Observe "maximal" attacks */
				for (m = 0; m < 4; m++)
				{
					/* Examine "actual" blows */
					if (r_ptr->blow[m].effect || r_ptr->blow[m].method)
					{
						/* Hack -- maximal observations */
						r_ptr->r_blows[m] = MAX_UCHAR;
					}
				}
				
				/* Hack -- maximal drops */
				r_ptr->r_drop_gold = r_ptr->r_drop_item =
					(((r_ptr->flags1 & RF1_DROP_4D2) ? 8 : 0) +
					 ((r_ptr->flags1 & RF1_DROP_3D2) ? 6 : 0) +
					 ((r_ptr->flags1 & RF1_DROP_2D2) ? 4 : 0) +
					 ((r_ptr->flags1 & RF1_DROP_1D2) ? 2 : 0) +
					 ((r_ptr->flags1 & RF1_DROP_90)  ? 1 : 0) +
					 ((r_ptr->flags1 & RF1_DROP_60)  ? 1 : 0));
				
				/* Hack -- but only "valid" drops */
				if (r_ptr->flags1 & RF1_ONLY_GOLD) r_ptr->r_drop_item = 0;
				if (r_ptr->flags1 & RF1_ONLY_ITEM) r_ptr->r_drop_gold = 0;
				
				/* Hack -- observe many spells */
				r_ptr->r_cast_inate = MAX_UCHAR;
				r_ptr->r_cast_spell = MAX_UCHAR;
				
				/* Hack -- know all the flags */
				r_ptr->r_flags1 = r_ptr->flags1;
				r_ptr->r_flags2 = r_ptr->flags2;
				r_ptr->r_flags3 = r_ptr->flags3;
				r_ptr->r_flags4 = r_ptr->flags4;
				r_ptr->r_flags5 = r_ptr->flags5;
				r_ptr->r_flags6 = r_ptr->flags6;
				
				r_ptr->r_xtra1 |= MR1_SINKA;
			
				/* Save this monster ID */
				monster_race_track(r_idx);

				/* Hack -- Handle stuff */
				handle_stuff();

				/* know every thing mode */
				screen_roff(r_idx, 0x01);
				notpicked = FALSE;

				/* XTRA HACK REMEMBER_IDX */
				old_sym = sym;
				old_i = i;
			}

			/* Command */
			query = inkey();

			/* Normal commands */
			if (query != 'r') break;

			/* Toggle recall */
			recall = !recall;
		}

		/* Stop scanning */
		if (query == ESCAPE) break;

		/* Move to "prev" monster */
		if (query == '-')
		{
			if (++i == n)
			{
				i = 0;
				if (!expand_list) break;
			}
		}

		/* Move to "next" monster */
		else
		{
			if (i-- == 0)
			{
				i = n - 1;
				if (!expand_list) break;
			}
		}
	}


	/* Re-display the identity */
	/* prt(buf, 5, 5);*/

	/* Free the "who" array */
	C_KILL(who, max_r_idx, u16b);

	/* Restore */
	screen_load();

	return (!notpicked);
}

