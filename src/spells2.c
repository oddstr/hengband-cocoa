/* File: spells2.c */

/* Purpose: Spell code (part 2) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * self-knowledge... idea from nethack.  Useful for determining powers and
 * resistences of items.  It saves the screen, clears it, then starts listing
 * attributes, a screenful at a time.  (There are a LOT of attributes to
 * list.  It will probably take 2 or 3 screens for a powerful character whose
 * using several artifacts...) -CFT
 *
 * It is now a lot more efficient. -BEN-
 *
 * See also "identify_fully()".
 *
 * XXX XXX XXX Use the "show_file()" method, perhaps.
 */
void self_knowledge(void)
{
	int i = 0, j, k;

	int v_nr = 0;
	char v_string [8] [128];
	char s_string [6] [128];

	u32b flgs[TR_FLAG_SIZE];

	object_type *o_ptr;

	char Dummy[80];
	char buf[2][80];

	cptr info[220];

	int plev = p_ptr->lev;

	int percent;

	for (j = 0; j < TR_FLAG_SIZE; j++)
		flgs[j] = 0L;

	p_ptr->knowledge |= (KNOW_STAT | KNOW_HPRATE);

	strcpy(Dummy, "");

	percent = (int)(((long)p_ptr->player_hp[PY_MAX_LEVEL - 1] * 200L) /
		(2 * p_ptr->hitdie +
		((PY_MAX_LEVEL - 1+3) * (p_ptr->hitdie + 1))));

#ifdef JP
sprintf(Dummy, "���ߤ����ϥ�� : %d/100", percent);
#else
	sprintf(Dummy, "Your current Life Rating is %d/100.", percent);
#endif

	strcpy(buf[0], Dummy);
	info[i++] = buf[0];
	info[i++] = "";

	chg_virtue(V_KNOWLEDGE, 1);
	chg_virtue(V_ENLIGHTEN, 1);

	/* Acquire item flags from equipment */
	for (k = INVEN_RARM; k < INVEN_TOTAL; k++)
	{
		u32b tflgs[TR_FLAG_SIZE];

		o_ptr = &inventory[k];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Extract the flags */
		object_flags(o_ptr, tflgs);

		/* Extract flags */
		for (j = 0; j < TR_FLAG_SIZE; j++)
			flgs[j] |= tflgs[j];
	}

#ifdef JP
	info[i++] = "ǽ�Ϥκ�����";
#else
	info[i++] = "Limits of maximum stats";
#endif

	for (v_nr = 0; v_nr < 6; v_nr++)
	{
		char stat_desc[80];

		sprintf(stat_desc, "%s 18/%d", stat_names[v_nr], p_ptr->stat_max_max[v_nr]-18);

		strcpy(s_string[v_nr], stat_desc);

		info[i++] = s_string[v_nr];
	}
	info[i++] = "";

#ifdef JP
	sprintf(Dummy, "���ߤ�°�� : %s(%ld)", your_alignment(), p_ptr->align);
#else
	sprintf(Dummy, "Your alighnment : %s(%ld)", your_alignment(), p_ptr->align);
#endif
	strcpy(buf[1], Dummy);
	info[i++] = buf[1];
	for (v_nr = 0; v_nr < 8; v_nr++)
	{
		char v_name [20];
		char vir_desc[80];
		int tester = p_ptr->virtues[v_nr];
	
		strcpy(v_name, virtue[(p_ptr->vir_types[v_nr])-1]);
 
#ifdef JP
		sprintf(vir_desc, "���äȡ�%s�ξ���ʤ���", v_name);
#else
		sprintf(vir_desc, "Oops. No info about %s.", v_name);
#endif
		if (tester < -100)
#ifdef JP
			sprintf(vir_desc, "[%s]���ж� (%d)",
#else
			sprintf(vir_desc, "You are the polar opposite of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -80)
#ifdef JP
			sprintf(vir_desc, "[%s]����Ũ (%d)",
#else
			sprintf(vir_desc, "You are an arch-enemy of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -60)
#ifdef JP
			sprintf(vir_desc, "[%s]�ζ�Ũ (%d)",
#else
			sprintf(vir_desc, "You are a bitter enemy of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -40)
#ifdef JP
			sprintf(vir_desc, "[%s]��Ũ (%d)",
#else
			sprintf(vir_desc, "You are an enemy of %s (%d).",
#endif
				v_name, tester);
		else if (tester < -20)
#ifdef JP
			sprintf(vir_desc, "[%s]�κ�� (%d)",
#else
			sprintf(vir_desc, "You have sinned against %s (%d).",
#endif
				v_name, tester);
		else if (tester < 0)
#ifdef JP
			sprintf(vir_desc, "[%s]����ƻ�� (%d)",
#else
			sprintf(vir_desc, "You have strayed from the path of %s (%d).",
#endif
				v_name, tester);
		else if (tester == 0)                   
#ifdef JP
			sprintf(vir_desc, "[%s]����Ω�� (%d)",
#else
			sprintf(vir_desc,"You are neutral to %s (%d).",
#endif
				v_name, tester);
		else if (tester < 20)
#ifdef JP
			sprintf(vir_desc, "[%s]�ξ����� (%d)",
#else
			sprintf(vir_desc,"You are somewhat virtuous in %s (%d).",
#endif
				v_name, tester);
		else if (tester < 40)
#ifdef JP
			sprintf(vir_desc, "[%s]�������� (%d)",
#else
			sprintf(vir_desc,"You are virtuous in %s (%d).",
#endif
				v_name, tester);
		else if (tester < 60)
#ifdef JP
			sprintf(vir_desc, "[%s]�ι����� (%d)",
#else
			sprintf(vir_desc,"You are very virtuous in %s (%d).",
#endif
				v_name, tester);
		else if (tester < 80)
#ifdef JP
			sprintf(vir_desc, "[%s]���Ƽ� (%d)",
#else
			sprintf(vir_desc,"You are a champion of %s (%d).",
#endif
				v_name, tester);
		else if (tester < 100)
#ifdef JP
			sprintf(vir_desc, "[%s]�ΰ�����Ƽ� (%d)",
#else
			sprintf(vir_desc,"You are a great champion of %s (%d).",
#endif
				v_name, tester);
		else
#ifdef JP
			sprintf(vir_desc, "[%s]�ζ񸽼� (%d)",
#else
			sprintf(vir_desc,"You are the living embodiment of %s (%d).",
#endif
		v_name, tester);
	
		strcpy(v_string[v_nr], vir_desc);
	
		info[i++] = v_string[v_nr];
	}
	info[i++] = "";
	
	/* Racial powers... */
	if (p_ptr->mimic_form)
	{
		switch (p_ptr->mimic_form)
		{
			case MIMIC_DEMON:
			case MIMIC_DEMON_LORD:
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼�����Ϲ����б�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(%d MP)", 3 * plev, 10+plev/3);
#else
				sprintf(Dummy, "You can nether breathe, dam. %d (cost %d).", 3 * plev, 10+plev/3);
#endif

				info[i++] = Dummy;
			break;
		case MIMIC_VAMPIRE:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "���ʤ���Ũ���� %d-%d HP ����̿�Ϥ�ۼ��Ǥ��롣(%d MP)",
#else
				sprintf(Dummy, "You can steal life from a foe, dam. %d-%d (cost %d).",
#endif

				    plev + MAX(1, plev / 10), plev + plev * MAX(1, plev / 10), 1 + (plev / 3));
				info[i++] = Dummy;
			}
			break;
		}
	}
	else
	{
	switch (p_ptr->prace)
	{
		case RACE_NIBELUNG:
		case RACE_DWARF:
			if (plev > 4)
#ifdef JP
info[i++] = "���ʤ���櫤ȥɥ��ȳ��ʤ��ΤǤ��롣(5 MP)";
#else
				info[i++] = "You can find traps, doors and stairs (cost 5).";
#endif

			break;
		case RACE_HOBBIT:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "���ʤ��Ͽ����������Ǥ��롣(10 MP)";
#else
				info[i++] = "You can produce food (cost 10).";
#endif

			}
			break;
		case RACE_GNOME:
			if (plev > 4)
			{
#ifdef JP
sprintf(Dummy, "���ʤ����ϰ� %d ����˥ƥ�ݡ��ȤǤ��롣(%d MP)",
#else
				sprintf(Dummy, "You can teleport, range %d (cost %d).",
#endif

				    (1 + plev), (5 + (plev / 5)));
				info[i++] = Dummy;
			}
			break;
		case RACE_HALF_ORC:
			if (plev > 2)
#ifdef JP
info[i++] = "���ʤ��϶��ݤ����Ǥ��롣(5 MP)";
#else
				info[i++] = "You can remove fear (cost 5).";
#endif

			break;
		case RACE_HALF_TROLL:
			if (plev > 9)
#ifdef JP
info[i++] = "���ʤ��϶�˽�����뤳�Ȥ��Ǥ��롣(12 MP) ";
#else
				info[i++] = "You enter berserk fury (cost 12).";
#endif

			break;
		case RACE_AMBERITE:
			if (plev > 29)
#ifdef JP
info[i++] = "���ʤ��ϥ���ɥ����եȤ��뤳�Ȥ��Ǥ��롣(50 MP)";
#else
				info[i++] = "You can Shift Shadows (cost 50).";
#endif

			if (plev > 39)
#ifdef JP
info[i++] = "���ʤ��ϡ֥ѥ�����פ򿴤��������⤯���Ȥ��Ǥ��롣(75 MP)";
#else
				info[i++] = "You can mentally Walk the Pattern (cost 75).";
#endif

			break;
		case RACE_BARBARIAN:
			if (plev > 7)
#ifdef JP
info[i++] = "���ʤ��϶�˽�����뤳�Ȥ��Ǥ��롣(10 MP) ";
#else
				info[i++] = "You can enter berserk fury (cost 10).";
#endif

			break;
		case RACE_HALF_OGRE:
			if (plev > 24)
#ifdef JP
info[i++] = "���ʤ�����ȯ�Υ롼���ųݤ��뤳�Ȥ��Ǥ��롣(35 MP)";
#else
				info[i++] = "You can set an Explosive Rune (cost 35).";
#endif

			break;
		case RACE_HALF_GIANT:
			if (plev > 19)
#ifdef JP
info[i++] = "���ʤ����Ф��ɤ�������Ȥ��Ǥ��롣(10 MP)";
#else
				info[i++] = "You can break stone walls (cost 10).";
#endif

			break;
		case RACE_HALF_TITAN:
			if (plev > 34)
#ifdef JP
info[i++] = "���ʤ��ϥ�󥹥����򥹥���󤹤뤳�Ȥ��Ǥ��롣(20 MP)";
#else
				info[i++] = "You can probe monsters (cost 20).";
#endif

			break;
		case RACE_CYCLOPS:
			if (plev > 19)
			{
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���δ��Ф��ꤲ�뤳�Ȥ��Ǥ��롣(15 MP)",
#else
				sprintf(Dummy, "You can throw a boulder, dam. %d (cost 15).",
#endif

				    3 * plev);
				info[i++] = Dummy;
			}
			break;
		case RACE_YEEK:
			if (plev > 14)
#ifdef JP
info[i++] = "���ʤ��϶��ݤ�Ƥӵ�������������ȯ���뤳�Ȥ��Ǥ��롣(15 MP)";
#else
				info[i++] = "You can make a terrifying scream (cost 15).";
#endif

			break;
		case RACE_KLACKON:
			if (plev > 8)
			{
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���λ���᤭�����뤳�Ȥ��Ǥ��롣(9 MP)", plev);
#else
				sprintf(Dummy, "You can spit acid, dam. %d (cost 9).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_KOBOLD:
			if (plev > 11)
			{
				sprintf(Dummy,
#ifdef JP
    "���ʤ��� %d ���᡼����������ꤲ�뤳�Ȥ��Ǥ��롣(8 MP)", plev);
#else
				    "You can throw a dart of poison, dam. %d (cost 8).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_DARK_ELF:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υޥ��å����ߥ�����μ�ʸ��Ȥ��롣(2 MP)",
#else
				sprintf(Dummy, "You can cast a Magic Missile, dam %d (cost 2).",
#endif

				    (3 + ((plev-1) / 5)));
				info[i++] = Dummy;
			}
			break;
		case RACE_DRACONIAN:
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(%d MP)", 2 * plev, plev);
#else
			sprintf(Dummy, "You can breathe, dam. %d (cost %d).", 2 * plev, plev);
#endif

			info[i++] = Dummy;
			break;
		case RACE_MIND_FLAYER:
			if (plev > 14)
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼������������򤹤뤳�Ȥ��Ǥ��롣(12 MP)", plev);
#else
				sprintf(Dummy, "You can mind blast your enemies, dam %d (cost 12).", plev);
#endif

			info[i++] = Dummy;
			break;
		case RACE_IMP:
			if (plev > 29)
			{
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υե��������ܡ���μ�ʸ��Ȥ��롣(15 MP)", plev);
#else
				sprintf(Dummy, "You can cast a Fire Ball, dam. %d (cost 15).", plev);
#endif

				info[i++] = Dummy;
			}
			else if (plev > 8)
			{
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υե��������ܥ�Ȥμ�ʸ��Ȥ��롣(15 MP)", plev);
#else
				sprintf(Dummy, "You can cast a Fire Bolt, dam. %d (cost 15).", plev);
#endif

				info[i++] = Dummy;
			}
			break;
		case RACE_GOLEM:
			if (plev > 19)
#ifdef JP
info[i++] = "���ʤ��� d20+30 ������δ�ȩ���Ф��Ѳ��������롣(15 MP)";
#else
				info[i++] = "You can turn your skin to stone, dur d20+30 (cost 15).";
#endif

			break;
		case RACE_ZOMBIE:
		case RACE_SKELETON:
			if (plev > 29)
#ifdef JP
info[i++] = "���ʤ��ϼ��ä���̿�Ϥ�������뤳�Ȥ��Ǥ��롣(30 MP)";
#else
				info[i++] = "You can restore lost life forces (cost 30).";
#endif

			break;
		case RACE_VAMPIRE:
			if (plev > 1)
			{
#ifdef JP
sprintf(Dummy, "���ʤ���Ũ���� %d-%d HP ����̿�Ϥ�ۼ��Ǥ��롣(%d MP)",
#else
				sprintf(Dummy, "You can steal life from a foe, dam. %d-%d (cost %d).",
#endif

				    plev + MAX(1, plev / 10), plev + plev * MAX(1, plev / 10), 1 + (plev / 3));
				info[i++] = Dummy;
			}
			break;
		case RACE_SPECTRE:
			if (plev > 3)
			{
#ifdef JP
info[i++] = "���ʤ��ϵ㤭�����Ũ���ݤ����뤳�Ȥ��Ǥ��롣(3 MP)";
#else
				info[i++] = "You can wail to terrify your enemies (cost 3).";
#endif

			}
			break;
		case RACE_SPRITE:
			if (plev > 11)
			{
#ifdef JP
info[i++] = "���ʤ���Ũ��̲�餻����ˡ��ʴ���ꤲ�뤳�Ȥ��Ǥ��롣(12 MP)";
#else
				info[i++] = "You can throw magical dust which induces sleep (cost 12).";
#endif

			}
			break;
		case RACE_DEMON:
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼�����Ϲ����б�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(%d MP)", 3 * plev, 10+plev/3);
#else
			sprintf(Dummy, "You can breathe nether, dam. %d (cost %d).", 3 * plev, 10+plev/3);
#endif

			info[i++] = Dummy;
			break;
		case RACE_KUTA:
			if (plev > 19)
#ifdef JP
info[i++] = "���ʤ��� d20+30 ������δֲ��˿��Ӥ뤳�Ȥ��Ǥ��롣(15 MP)";
#else
				info[i++] = "You can expand horizontally, dur d20+30 (cost 15).";
#endif

			break;
		case RACE_ANDROID:
			if (plev < 10)
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υ쥤������Ĥ��Ȥ��Ǥ��롣(7 MP)", (plev + 1) / 2);
#else
				sprintf(Dummy, "You can fire a ray gun with damage %d (cost 7).", (plev+1)/2);
#endif
			else if (plev < 25)
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υ֥饹�������Ĥ��Ȥ��Ǥ��롣(13 MP)", plev);
#else
				sprintf(Dummy, "You can fire a blaster with damage %d (cost 13).", plev);
#endif
			else if (plev < 35)
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���ΥХ��������Ĥ��Ȥ��Ǥ��롣(26 MP)", plev * 2);
#else
				sprintf(Dummy, "You can fire a bazooka with damage %d (cost 26).", plev * 2);
#endif
			else if (plev < 45)
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υӡ��७��Υ���Ĥ��Ȥ��Ǥ��롣(40 MP)", plev * 2);
#else
				sprintf(Dummy, "You can fire a beam cannon with damage %d (cost 40).", plev * 2);
#endif
			else
#ifdef JP
sprintf(Dummy, "���ʤ��� %d ���᡼���Υ��åȤ��Ĥ��Ȥ��Ǥ��롣(60 MP)", plev * 5);
#else
				sprintf(Dummy, "You can fire a rocket with damage %d (cost 60).", plev * 5);
#endif

			info[i++] = Dummy;
			break;
		default:
			break;
	}
	}

	switch(p_ptr->pclass)
	{
		case CLASS_WARRIOR:
			if (plev > 39)
			{
#ifdef JP
info[i++] = "���ʤ��ϥ�������������Ф��ƿ��󹶷⤹�뤳�Ȥ��Ǥ��롣(75 MP)";
#else
				info[i++] = "You can attack some random directions at a time (cost 75).";
#endif
			}
			break;
		case CLASS_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_SORCERER:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "���ʤ��ϥ����ƥ�����Ϥ�ۼ����뤳�Ȥ��Ǥ��롣(1 MP)";
#else
				info[i++] = "You can absorb charges from an item (cost 1).";
#endif
			}
			break;
		case CLASS_PRIEST:
			if (is_good_realm(p_ptr->realm1))
			{
				if (plev > 34)
				{
#ifdef JP
info[i++] = "���ʤ��������ʡ���뤳�Ȥ��Ǥ��롣(70 MP)";
#else
					info[i++] = "You can bless a weapon (cost 70).";
#endif
				}
			}
			else
			{
				if (plev > 41)
				{
#ifdef JP
info[i++] = "���ʤ��ϼ���Τ��٤ƤΥ�󥹥����򹶷⤹�뤳�Ȥ��Ǥ��롣(40 MP)";
#else
					info[i++] = "You can damages all monsters in sight (cost 40).";
#endif
				}
			}
			break;
		case CLASS_ROGUE:
			if (plev > 7)
			{
#ifdef JP
info[i++] = "���ʤ��Ϲ��⤷��¨�¤�ƨ���뤳�Ȥ��Ǥ��롣(12 MP)";
#else
				info[i++] = "You can hit a monster and teleport at a time (cost 12).";
#endif
			}
			break;
		case CLASS_RANGER:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "���ʤ��ϲ�ʪ��Ĵ�����뤳�Ȥ��Ǥ��롣(20 MP)";
#else
				info[i++] = "You can prove monsters (cost 20).";
#endif
			}
			break;
		case CLASS_PALADIN:
			if (is_good_realm(p_ptr->realm1))
			{
				if (plev > 29)
				{
#ifdef JP
info[i++] = "���ʤ������ʤ�������Ĥ��Ȥ��Ǥ��롣(30 MP)";
#else
					info[i++] = "You can fires a holy spear (cost 30).";
#endif
				}
			}
			else
			{
				if (plev > 29)
				{
#ifdef JP
info[i++] = "���ʤ�����̿�Ϥ򸺾�������������Ĥ��Ȥ��Ǥ��롣(30 MP)";
#else
					info[i++] = "You can fires a spear which drains vitality (cost 30).";
#endif
				}
			}
			break;
		case CLASS_WARRIOR_MAGE:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "���ʤ��ϣȣФ�ͣФ��Ѵ����뤳�Ȥ��Ǥ��롣(0 MP)";
#else
				info[i++] = "You can convert HP to SP (cost 0).";
#endif
#ifdef JP
info[i++] = "���ʤ��ϣͣФ�ȣФ��Ѵ����뤳�Ȥ��Ǥ��롣(0 MP)";
#else
				info[i++] = "You can convert SP to HP (cost 0).";
#endif
			}
			break;
		case CLASS_CHAOS_WARRIOR:
			if (plev > 39)
			{
#ifdef JP
info[i++] = "���ʤ��ϼ��Ϥ˲�ʪ���Ǥ魯����ȯ�������뤳�Ȥ��Ǥ��롣(50 MP)";
#else
				info[i++] = "You can radiate light which confuses nearby monsters (cost 50).";
#endif
			}
			break;
		case CLASS_MONK:
			if (plev > 24)
			{
#ifdef JP
info[i++] = "���ʤ��Ϲ����뤳�Ȥ��Ǥ��롣(0 MP)";
#else
				info[i++] = "You can assume a posture of special form (cost 0).";
#endif
			}
			if (plev > 29)
			{
#ifdef JP
info[i++] = "���ʤ����̾��2�ܤι����Ԥ����Ȥ��Ǥ��롣(30 MP)";
#else
				info[i++] = "You can perform double attacks in a time (cost 30).";
#endif
			}
			break;
		case CLASS_MINDCRAFTER:
		case CLASS_FORCETRAINER:
			if (plev > 14)
			{
#ifdef JP
info[i++] = "���ʤ����������椷�ƣͣФ���������뤳�Ȥ��Ǥ��롣(0 MP)";
#else
				info[i++] = "You can concentrate to regenerate your mana (cost 0).";
#endif
			}
			break;
		case CLASS_TOURIST:
#ifdef JP
info[i++] = "���ʤ��ϼ̿��򻣱Ƥ��뤳�Ȥ��Ǥ��롣(0 MP)";
#else
				info[i++] = "You can take a photograph (cost 0).";
#endif
			if (plev > 24)
			{
#ifdef JP
info[i++] = "���ʤ��ϥ����ƥ�����˴��ꤹ�뤳�Ȥ��Ǥ��롣(20 MP)";
#else
				info[i++] = "You can *identify* items (cost 20).";
#endif
			}
			break;
		case CLASS_IMITATOR:
			if (plev > 29)
			{
#ifdef JP
info[i++] = "���ʤ��ϲ�ʪ���ü칶�����᡼��2�ܤǤޤͤ뤳�Ȥ��Ǥ��롣(100 MP)";
#else
				info[i++] = "You can imitate monster's special attacks with double damage (cost 100).";
#endif
			}
			break;
		case CLASS_BEASTMASTER:
#ifdef JP
info[i++] = "���ʤ���1ɤ����̿�Τ����󥹥�������ۤ��뤳�Ȥ��Ǥ��롣(��٥�/4 MP)";
#else
			info[i++] = "You can dominate a monster (cost level/4).";
#endif
			if (plev > 29)
			{
#ifdef JP
info[i++] = "���ʤ��ϻ볦�����̿�Τ����󥹥�������ۤ��뤳�Ȥ��Ǥ��롣((��٥�+20)/2 MP)";
#else
				info[i++] = "You can dominate living monsters in sight (cost (level+20)/4).";
#endif
			}
			break;
		case CLASS_MAGIC_EATER:
#ifdef JP
info[i++] = "���ʤ��Ͼ�/��ˡ��/��åɤ����Ϥ�ʬ�Τ�Τˤ��뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can absorb a staff, wand or rod itself.";
#endif
			break;
		case CLASS_RED_MAGE:
			if (plev > 47)
			{
#ifdef JP
info[i++] = "���ʤ���1�������2����ˡ�򾧤��뤳�Ȥ��Ǥ��롣(77 MP)";
#else
				info[i++] = "You can cast two spells in one time (cost 77).";
#endif
			}
			break;
		case CLASS_SAMURAI:
			{
#ifdef JP
info[i++] = "���ʤ����������椷�Ƶ��礤��ί��뤳�Ȥ��Ǥ��롣";
#else
				info[i++] = "You can concentrate to regenerate your mana.";
#endif
			}
			if (plev > 24)
			{
#ifdef JP
info[i++] = "���ʤ����ü�ʷ��ǹ����뤳�Ȥ��Ǥ��롣";
#else
				info[i++] = "You can assume a posture of special form.";
#endif
			}
			break;
		case CLASS_BLUE_MAGE:
#ifdef JP
info[i++] = "���ʤ������˻Ȥ�줿��ˡ��ؤ֤��Ȥ��Ǥ��롣";
#else
			info[i++] = "You can study spells which your enemy casts on you.";
#endif
			break;
		case CLASS_CAVALRY:
			if (plev > 9)
			{
#ifdef JP
info[i++] = "���ʤ��ϥ�󥹥����˾�ä�̵�������ڥåȤˤ��뤳�Ȥ��Ǥ��롣";
#else
				info[i++] = "You can ride on a hostile monster forcibly to turn it into pet.";
#endif
			}
			break;
		case CLASS_BERSERKER:
			if (plev > 9)
			{
#ifdef JP
info[i++] = "���ʤ��ϳ��ȥ��󥸥��δ֤�Ԥ��褹�뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can travel between town and the depths.";
#endif
			}
			break;
		case CLASS_MIRROR_MASTER:
#ifdef JP
info[i++] = "���ʤ��϶�����Ф����Ȥ��Ǥ��롣(2 MP)";
#else
				info[i++] = "You can create a Mirror (cost 2).";
#endif
#ifdef JP
info[i++] = "���ʤ��϶����뤳�Ȥ��Ǥ��롣(0 MP)";
#else
				info[i++] = "You can break distant Mirrors (cost 0).";
#endif
			break;
		case CLASS_NINJA:
			if (plev > 19)
			{
#ifdef JP
info[i++] = "���ʤ������᤯��ư���뤳�Ȥ��Ǥ��롣";
#else
				info[i++] = "You can walk extremery fast.";
#endif
			}
			break;
	}

	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_SPIT_ACID)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ���᤭�����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X1)";
#else
			info[i++] = "You can spit acid (dam lvl).";
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
info[i++] = "���ʤ��ϱ�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(���᡼�� ��٥�X2)";
#else
			info[i++] = "You can breathe fire (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
info[i++] = "���ʤ����ˤߤϺ�̲���̤��ġ�";
#else
			info[i++] = "Your gaze is hypnotic.";
#endif

		}
		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
info[i++] = "���ʤ���ǰư�Ϥ��äƤ��롣";
#else
			info[i++] = "You are telekinetic.";
#endif

		}
		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ�ʬ�ΰջפǥƥ�ݡ��ȤǤ��롣";
#else
			info[i++] = "You can teleport at will.";
#endif

		}
		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
info[i++] = "���ʤ������������Ԥ��롣(���᡼�� 3��12d3)";
#else
			info[i++] = "You can Mind Blast your enemies (3 to 12d3 dam).";
#endif

		}
		if (p_ptr->muta1 & MUT1_RADIATION)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ�ʬ�ΰջפǶ�����������ȯ�����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X2)";
#else
			info[i++] = "You can emit hard radiation at will (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
info[i++] = "���ʤ��ϵ۷쵴�Τ褦��Ũ������̿�Ϥ�ۼ����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X2)";
#else
			info[i++] = "You can drain life from a foe like a vampire (dam lvl * 2).";
#endif

		}
		if (p_ptr->muta1 & MUT1_SMELL_MET)
		{
#ifdef JP
info[i++] = "���ʤ��϶᤯�ˤ��뵮��°�򤫤�ʬ���뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can smell nearby precious metal.";
#endif

		}
		if (p_ptr->muta1 & MUT1_SMELL_MON)
		{
#ifdef JP
info[i++] = "���ʤ��϶᤯�Υ�󥹥�����¸�ߤ򤫤�ʬ���뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can smell nearby monsters.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
info[i++] = "���ʤ���û����Υ��ƥ�ݡ��ȤǤ��롣";
#else
			info[i++] = "You can teleport yourself short distances.";
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
info[i++] = "���ʤ��ϹŤ���򿩤٤뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can consume solid rock.";
#endif

		}
		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
info[i++] = "���ʤ���¾�μԤȾ��������ؤ�뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can switch locations with another being.";
#endif

		}
		if (p_ptr->muta1 & MUT1_SHRIEK)
		{
#ifdef JP
info[i++] = "���ʤ��ϿȤ��Ӥ����Ķ�������ȯ���뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X2)";
#else
			info[i++] = "You can emit a horrible shriek (dam 2 * lvl).";
#endif

		}
		if (p_ptr->muta1 & MUT1_ILLUMINE)
		{
#ifdef JP
info[i++] = "���ʤ������뤤�������Ĥ��Ȥ��Ǥ��롣";
#else
			info[i++] = "You can emit bright light.";
#endif

		}
		if (p_ptr->muta1 & MUT1_DET_CURSE)
		{
#ifdef JP
info[i++] = "���ʤ��ϼٰ�����ˡ�δ��򴶤��Ȥ뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can feel the danger of evil magic.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ�ʬ�ΰջפǶ�����Ʈ���֤ˤʤ뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can drive yourself into a berserk frenzy.";
#endif

		}
		if (p_ptr->muta1 & MUT1_POLYMORPH)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ�ʬ�ΰջ֤��Ѳ��Ǥ��롣";
#else
			info[i++] = "You can polymorph yourself at will.";
#endif

		}
		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
info[i++] = "���ʤ����̾異���ƥ�����Ѥ��뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can turn ordinary items to gold.";
#endif

		}
		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ��Ϥ˥��Υ������䤹���Ȥ��Ǥ��롣";
#else
			info[i++] = "You can cause mold to grow near you.";
#endif

		}
		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
info[i++] = "���ʤ��ϸ��Ǥι�����Ф��ƿȤ�Ť����뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can harden yourself to the ravages of the elements.";
#endif

		}
		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ��ϤΥ��󥸥������������뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can bring down the dungeon around your ears.";
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
info[i++] = "���ʤ�����ˡ�Υ��ͥ륮����ʬ��ʪ�Ȥ��ƻ��ѤǤ��롣";
#else
			info[i++] = "You can consume magic energy for your own use.";
#endif

		}
		if (p_ptr->muta1 & MUT1_WEIGH_MAG)
		{
#ifdef JP
info[i++] = "���ʤ��ϼ�ʬ�˱ƶ���Ϳ������ˡ���Ϥ򴶤��뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can feel the strength of the magics affecting you.";
#endif

		}
		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
info[i++] = "���ʤ��Ͻ���Ū������ǽ�򵯤������Ȥ��Ǥ��롣";
#else
			info[i++] = "You can cause mass impotence.";
#endif

		}
		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
info[i++] = "���ʤ��Ϲ��⤷����Ȥ��뤿��ƨ���뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can run for your life after hitting something.";
#endif

		}
		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
info[i++] = "���ʤ��Ϻ�������ܤ��������������ǽ��ȯ�����뤳�Ȥ��Ǥ��롣 ";
#else
			info[i++] = "You can emit confusing, blinding radiation.";
#endif

		}
		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
info[i++] = "���ʤ����ܤ���졼����������ȯ���뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X2)";
#else
			info[i++] = "Your eyes can fire laser beams (dam 2 * lvl).";
#endif

		}
		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
info[i++] = "���ʤ��ϳ��ȥ��󥸥��δ֤�Ԥ��褹�뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can travel between town and the depths.";
#endif

		}
		if (p_ptr->muta1 & MUT1_BANISH)
		{
#ifdef JP
info[i++] = "���ʤ��ϼٰ��ʥ�󥹥������Ϲ�����Ȥ����Ȥ��Ǥ��롣";
#else
			info[i++] = "You can send evil creatures directly to Hell.";
#endif

		}
		if (p_ptr->muta1 & MUT1_COLD_TOUCH)
		{
#ifdef JP
info[i++] = "���ʤ���Ũ�򿨤ä���餻�뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X3)";
#else
			info[i++] = "You can freeze things with a touch (dam 3 * lvl).";
#endif

		}
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ����ƥ���϶����ꤲ�뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "You can hurl objects with great force.";
#endif

		}
	}

	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
info[i++] = "���ʤ��϶���β���ȯ��򵯤�����";
#else
			info[i++] = "You are subject to berserker fits.";
#endif

		}
		if (p_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ������¤ˤʤ롣";
#else
			info[i++] = "You are subject to cowardice.";
#endif

		}
		if (p_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ�����˥ƥ�ݡ��Ȥ��롣";
#else
			info[i++] = "You are teleporting randomly.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
info[i++] = "���ʤ����Τϥ��륳�����ʬ�礹�롣";
#else
			info[i++] = "Your body produces alcohol.";
#endif

		}
		if (p_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
info[i++] = "���ʤ��ϸ��Ф������������������˿�����Ƥ��롣";
#else
			info[i++] = "You have a hallucinatory insanity.";
#endif

		}
		if (p_ptr->muta2 & MUT2_FLATULENT)
		{
#ifdef JP
info[i++] = "���ʤ�������Ǥ��ʤ����������򤳤���";
#else
			info[i++] = "You are subject to uncontrollable flatulence.";
#endif

		}
		if (p_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
info[i++] = "���ʤ���������ǽ����ˡ�Υ��ͥ륮����ȯ���Ƥ��롣";
#else
			info[i++] = "You are producing magical energy uncontrollably.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
info[i++] = "���ʤ��ϥǡ���������Ĥ��롣";
#else
			info[i++] = "You attract demons.";
#endif

		}
		if (p_ptr->muta2 & MUT2_SCOR_TAIL)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ�����ο����������Ƥ��롣(�ǡ����᡼�� 3d7)";
#else
			info[i++] = "You have a scorpion tail (poison, 3d7).";
#endif

		}
		if (p_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
info[i++] = "���ʤ��ϳѤ������Ƥ��롣(���᡼�� 2d6)";
#else
			info[i++] = "You have horns (dam. 2d6).";
#endif

		}
		if (p_ptr->muta2 & MUT2_BEAK)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ����Х��������Ƥ��롣(���᡼�� 2d4)";
#else
			info[i++] = "You have a beak (dam. 2d4).";
#endif

		}
		if (p_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ�������᤯ư�������٤�ư�����ꤹ�롣";
#else
			info[i++] = "You move faster or slower randomly.";
#endif

		}
		if (p_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ����᤯�Υ�󥹥�������Ǥ����롣";
#else
			info[i++] = "You sometimes cause nearby creatures to vanish.";
#endif

		}
		if (p_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ������Ϥθ���ۼ����Ʊ��ܤˤ��롣";
#else
			info[i++] = "You sometimes feed off of the light around you.";
#endif

		}
		if (p_ptr->muta2 & MUT2_TRUNK)
		{
#ifdef JP
info[i++] = "���ʤ��ϾݤΤ褦��ɡ����äƤ��롣(���᡼�� 1d4)";
#else
			info[i++] = "You have an elephantine trunk (dam 1d4).";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
info[i++] = "���ʤ���ưʪ������Ĥ��롣";
#else
			info[i++] = "You attract animals.";
#endif

		}
		if (p_ptr->muta2 & MUT2_TENTACLES)
		{
#ifdef JP
info[i++] = "���ʤ��ϼٰ��ʿ������äƤ��롣(���᡼�� 2d5)";
#else
			info[i++] = "You have evil looking tentacles (dam 2d5).";
#endif

		}
		if (p_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
info[i++] = "���ʤ��Ϥ��Ф��н㥫��������ޤ�롣";
#else
			info[i++] = "You occasionally are surrounded with raw chaos.";
#endif

		}
		if (p_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
info[i++] = "���ʤ����Ѱۤ��Ƥ��������������Ƥ��Ƥ��롣";
#else
			info[i++] = "You may be mutated, but you're recovering.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
info[i++] = "���ʤ������Τ�ͩ�β���������β������ꤹ�롣";
#else
			info[i++] = "You fade in and out of physical reality.";
#endif

		}
		if (p_ptr->muta2 & MUT2_POLY_WOUND)
		{
#ifdef JP
info[i++] = "���ʤ��η򹯤ϥ��������Ϥ˱ƶ�������롣";
#else
			info[i++] = "Your health is subject to chaotic forces.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WASTING)
		{
#ifdef JP
info[i++] = "���ʤ��Ͽ�夹�붲�����µ��ˤ����äƤ��롣";
#else
			info[i++] = "You have a horrible wasting disease.";
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
info[i++] = "���ʤ��ϥɥ饴�������Ĥ��롣";
#else
			info[i++] = "You attract dragons.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WEIRD_MIND)
		{
#ifdef JP
info[i++] = "���ʤ��������ϥ�����˳��礷����̾������ꤷ�Ƥ��롣";
#else
			info[i++] = "Your mind randomly expands and contracts.";
#endif

		}
		if (p_ptr->muta2 & MUT2_NAUSEA)
		{
#ifdef JP
info[i++] = "���ʤ��ΰߤ���������夭���ʤ���";
#else
			info[i++] = "You have a seriously upset stomach.";
#endif

		}
		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ������μ��⤫��˫���򤦤��Ȥ롣";
#else
			info[i++] = "Chaos deities give you gifts.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
info[i++] = "���ʤ��Ϥ��Ф���¾�Ρֱơפ��¤����ࡣ";
#else
			info[i++] = "You occasionally stumble into other shadows.";
#endif

		}
		if (p_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
info[i++] = "���ʤ���Ũ�˴ؤ���ٹ�򴶤��롣";
#else
			info[i++] = "You receive warnings about your foes.";
#endif

		}
		if (p_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ����餱�Τ餺�ʵ�ʬ�ˤʤ롣";
#else
			info[i++] = "You occasionally feel invincible.";
#endif

		}
		if (p_ptr->muta2 & MUT2_SP_TO_HP)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ����줬�����ˤɤä�ή��롣";
#else
			info[i++] = "Your blood sometimes rushes to your muscles.";
#endif

		}
		if (p_ptr->muta2 & MUT2_HP_TO_SP)
		{
#ifdef JP
info[i++] = "���ʤ��ϻ���Ƭ�˷줬�ɤä�ή��롣";
#else
			info[i++] = "Your blood sometimes rushes to your head.";
#endif

		}
		if (p_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
info[i++] = "���ʤ��Ϥ褯�ĤޤŤ���ʪ����Ȥ���";
#else
			info[i++] = "You occasionally stumble and drop things.";
#endif

		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
info[i++] = "���ʤ���Ķ��Ū�˶�����(����+4)";
#else
			info[i++] = "You are superhumanly strong (+4 STR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
info[i++] = "���ʤ��ϵ������(����-4)";
#else
			info[i++] = "You are puny (-4 STR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
info[i++] = "���ʤ���Ǿ�����Υ���ԥ塼������(��ǽ������+4)";
#else
			info[i++] = "Your brain is a living computer (+4 INT/WIS).";
#endif

		}
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
info[i++] = "���ʤ��������������(��ǽ������-4)";
#else
			info[i++] = "You are moronic (-4 INT/WIS).";
#endif

		}
		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
info[i++] = "���ʤ������˥��դ���(�ѵ�+4)";
#else
			info[i++] = "You are very resilient (+4 CON).";
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_FAT)
		{
#ifdef JP
info[i++] = "���ʤ��϶�ü�����äƤ��롣(�ѵ�+2,���ԡ���-2)";
#else
			info[i++] = "You are extremely fat (+2 CON, -2 speed).";
#endif

		}
		if (p_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
info[i++] = "���ʤ��ϥ���ӥΤ���(�ѵ�-4)";
#else
			info[i++] = "You are albino (-4 CON).";
#endif

		}
		if (p_ptr->muta3 & MUT3_FLESH_ROT)
		{
#ifdef JP
info[i++] = "���ʤ������Τ����Ԥ��Ƥ��롣(�ѵ�-2,̥��-1)";
#else
			info[i++] = "Your flesh is rotting (-2 CON, -1 CHR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_SILLY_VOI)
		{
#ifdef JP
info[i++] = "���ʤ������ϴ�ȴ���ʥ�������������(̥��-4)";
#else
			info[i++] = "Your voice is a silly squeak (-4 CHR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
info[i++] = "���ʤ��ϤΤäڤ�ܤ�����(̥��-1)";
#else
			info[i++] = "Your face is featureless (-1 CHR).";
#endif

		}
		if (p_ptr->muta3 & MUT3_ILL_NORM)
		{
#ifdef JP
info[i++] = "���ʤ��ϸ��Ƥ�ʤ���Ƥ��롣";
#else
			info[i++] = "Your appearance is masked with illusion.";
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
info[i++] = "���ʤ���;ʬ����Ĥ��ܤ���äƤ��롣(õ��+15)";
#else
			info[i++] = "You have an extra pair of eyes (+15 search).";
#endif

		}
		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
info[i++] = "���ʤ�����ˡ�ؤ��������äƤ��롣";
#else
			info[i++] = "You are resistant to magic.";
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
info[i++] = "���ʤ����Ѥʲ���ȯ���Ƥ��롣(��̩-3)";
#else
			info[i++] = "You make a lot of strange noise (-3 stealth).";
#endif

		}
		if (p_ptr->muta3 & MUT3_INFRAVIS)
		{
#ifdef JP
info[i++] = "���ʤ��������餷���ֳ������Ϥ���äƤ��롣(+3)";
#else
			info[i++] = "You have remarkable infravision (+3).";
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_LEGS)
		{
#ifdef JP
info[i++] = "���ʤ���;ʬ�����ܤ�­�������Ƥ��롣(��®+3)";
#else
			info[i++] = "You have an extra pair of legs (+3 speed).";
#endif

		}
		if (p_ptr->muta3 & MUT3_SHORT_LEG)
		{
#ifdef JP
info[i++] = "���ʤ���­��û���͵�����(��®-3)";
#else
			info[i++] = "Your legs are short stubs (-3 speed).";
#endif

		}
		if (p_ptr->muta3 & MUT3_ELEC_TOUC)
		{
#ifdef JP
info[i++] = "���ʤ��η�ɤˤ���ή��ή��Ƥ��롣";
#else
			info[i++] = "Electricity is running through your veins.";
#endif

		}
		if (p_ptr->muta3 & MUT3_FIRE_BODY)
		{
#ifdef JP
info[i++] = "���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣";
#else
			info[i++] = "Your body is enveloped in flames.";
#endif
		}
		if (p_ptr->muta3 & MUT3_WART_SKIN)
		{
#ifdef JP
info[i++] = "���ʤ���ȩ�ϥ��ܤ�����Ƥ��롣(̥��-2, AC+5)";
#else
			info[i++] = "Your skin is covered with warts (-2 CHR, +5 AC).";
#endif

		}
		if (p_ptr->muta3 & MUT3_SCALES)
		{
#ifdef JP
info[i++] = "���ʤ���ȩ���ڤˤʤäƤ��롣(̥��-1, AC+10)";
#else
			info[i++] = "Your skin has turned into scales (-1 CHR, +10 AC).";
#endif

		}
		if (p_ptr->muta3 & MUT3_IRON_SKIN)
		{
#ifdef JP
info[i++] = "���ʤ���ȩ��Ŵ�ǤǤ��Ƥ��롣(����-1, AC+25)";
#else
			info[i++] = "Your skin is made of steel (-1 DEX, +25 AC).";
#endif

		}
		if (p_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
info[i++] = "���ʤ��ϱ�����äƤ��롣";
#else
			info[i++] = "You have wings.";
#endif

		}
		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			/* Unnecessary */
		}
		if (p_ptr->muta3 & MUT3_REGEN)
		{
			/* Unnecessary */
		}
		if (p_ptr->muta3 & MUT3_ESP)
		{
			/* Unnecessary */
		}
		if (p_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
info[i++] = "���ʤ����Τ����ˤ��ʤ䤫����(����+3)";
#else
			info[i++] = "Your body is very limber (+3 DEX).";
#endif

		}
		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
info[i++] = "���ʤ��Ϥ��Ĥ������ˤߤ򴶤��Ƥ��롣(����-3)";
#else
			info[i++] = "Your joints ache constantly (-3 DEX).";
#endif

		}
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
info[i++] = "���ʤ��ϸ��Ǥι���˼夤��";
#else
			info[i++] = "You are susceptible to damage from the elements.";
#endif

		}
		if (p_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
info[i++] = "���ʤ���ư������Τ��϶�����(��̩+1)";
#else
			info[i++] = "Your movements are precise and forceful (+1 STL).";
#endif

		}
		if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
info[i++] = "���ʤ����򤤥�����ˤĤĤޤ�Ƥ��롣";
#else
			info[i++] = "There is a white aura surrounding you.";
#endif
		}
		if (p_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
info[i++] = "���ʤ��Ϲ���������ˤĤĤޤ�Ƥ��롣";
#else
			info[i++] = "There is a black aura surrounding you.";
#endif
		}
	}

	if (p_ptr->blind)
	{
#ifdef JP
info[i++] = "���ʤ����ܤ������ʤ���";
#else
		info[i++] = "You cannot see.";
#endif

	}
	if (p_ptr->confused)
	{
#ifdef JP
info[i++] = "���ʤ��Ϻ��𤷤Ƥ��롣";
#else
		info[i++] = "You are confused.";
#endif

	}
	if (p_ptr->afraid)
	{
#ifdef JP
info[i++] = "���ʤ��϶��ݤ˿�����Ƥ��롣";
#else
		info[i++] = "You are terrified.";
#endif

	}
	if (p_ptr->cut)
	{
#ifdef JP
info[i++] = "���ʤ��Ͻз줷�Ƥ��롣";
#else
		info[i++] = "You are bleeding.";
#endif

	}
	if (p_ptr->stun)
	{
#ifdef JP
info[i++] = "���ʤ��Ϥ⤦���Ȥ��Ƥ��롣";
#else
		info[i++] = "You are stunned.";
#endif

	}
	if (p_ptr->poisoned)
	{
#ifdef JP
info[i++] = "���ʤ����Ǥ˿�����Ƥ��롣";
#else
		info[i++] = "You are poisoned.";
#endif

	}
	if (p_ptr->image)
	{
#ifdef JP
info[i++] = "���ʤ��ϸ��Ф򸫤Ƥ��롣";
#else
		info[i++] = "You are hallucinating.";
#endif

	}
	if (p_ptr->cursed & TRC_TY_CURSE)
	{
#ifdef JP
info[i++] = "���ʤ��ϼٰ��ʱ�ǰ����ޤ�Ƥ��롣";
#else
		info[i++] = "You carry an ancient foul curse.";
#endif

	}
	if (p_ptr->cursed & TRC_AGGRAVATE)
	{
#ifdef JP
info[i++] = "���ʤ��ϥ�󥹥������ܤ餻�Ƥ��롣";
#else
		info[i++] = "You aggravate monsters.";
#endif

	}
	if (p_ptr->cursed & TRC_DRAIN_EXP)
	{
#ifdef JP
info[i++] = "���ʤ��Ϸи��ͤ�ۤ��Ƥ��롣";
#else
		info[i++] = "You are drained.";
#endif

	}
	if (p_ptr->cursed & TRC_SLOW_REGEN)
	{
#ifdef JP
info[i++] = "���ʤ��β����Ϥ������٤���";
#else
		info[i++] = "You regenerate slowly.";
#endif

	}
	if (p_ptr->cursed & TRC_ADD_L_CURSE)
	{
#ifdef JP
info[i++] = "���ʤ��μ夤�����������롣"; /* ����Ū -- henkma */
#else
		info[i++] = "Your weak curses multiply.";
#endif

	}
	if (p_ptr->cursed & TRC_ADD_H_CURSE)
	{
#ifdef JP
info[i++] = "���ʤ��ζ��������������롣"; /* ����Ū -- henkma */
#else
		info[i++] = "Your heavy curses multiply.";
#endif

	}
	if (p_ptr->cursed & TRC_CALL_ANIMAL)
	{
#ifdef JP
info[i++] = "���ʤ���ưʪ�������Ƥ��롣";
#else
		info[i++] = "You attract animals.";
#endif

	}
	if (p_ptr->cursed & TRC_CALL_DEMON)
	{
#ifdef JP
info[i++] = "���ʤ��ϰ���������Ƥ��롣";
#else
		info[i++] = "You attract demons.";
#endif

	}
	if (p_ptr->cursed & TRC_CALL_DRAGON)
	{
#ifdef JP
info[i++] = "���ʤ��ϥɥ饴��������Ƥ��롣";
#else
		info[i++] = "You attract dragons.";
#endif

	}
	if (p_ptr->cursed & TRC_COWARDICE)
	{
#ifdef JP
info[i++] = "���ʤ��ϻ������¤ˤʤ롣";
#else
		info[i++] = "You are subject to cowardice.";
#endif

	}
	if (p_ptr->cursed & TRC_TELEPORT)
	{
#ifdef JP
info[i++] = "���ʤ��ΰ��֤ϤҤ��礦���԰������";
#else
		info[i++] = "Your position is very uncertain.";
#endif

	}
	if (p_ptr->cursed & TRC_LOW_MELEE)
	{
#ifdef JP
info[i++] = "���ʤ������Ϲ���򳰤��䤹����";
#else
		info[i++] = "Your weapon causes you to miss blows.";
#endif

	}
	if (p_ptr->cursed & TRC_LOW_AC)
	{
#ifdef JP
info[i++] = "���ʤ��Ϲ��������䤹����";
#else
		info[i++] = "You are subject to be hit.";
#endif

	}
	if (p_ptr->cursed & TRC_LOW_MAGIC)
	{
#ifdef JP
info[i++] = "���ʤ�����ˡ���Ԥ��䤹����";
#else
		info[i++] = "You are subject to fail spellcasting.";
#endif

	}
	if (p_ptr->cursed & TRC_FAST_DIGEST)
	{
#ifdef JP
info[i++] = "���ʤ��Ϥ�����ʢ���ؤ롣";
#else
		info[i++] = "You have a good appetite.";
#endif

	}
	if (p_ptr->cursed & TRC_DRAIN_HP)
	{
#ifdef JP
info[i++] = "���ʤ������Ϥ�ۤ��Ƥ��롣";
#else
		info[i++] = "You are drained.";
#endif

	}
	if (p_ptr->cursed & TRC_DRAIN_MANA)
	{
#ifdef JP
info[i++] = "���ʤ������Ϥ�ۤ��Ƥ��롣";
#else
		info[i++] = "You brain is drained.";
#endif

	}
	if (p_ptr->blessed)
	{
#ifdef JP
info[i++] = "���ʤ��ϸ������򴶤��Ƥ��롣";
#else
		info[i++] = "You feel rightous.";
#endif

	}
	if (p_ptr->hero)
	{
#ifdef JP
info[i++] = "���ʤ��ϥҡ�����ʬ����";
#else
		info[i++] = "You feel heroic.";
#endif

	}
	if (p_ptr->shero)
	{
#ifdef JP
info[i++] = "���ʤ�����Ʈ������";
#else
		info[i++] = "You are in a battle rage.";
#endif

	}
	if (p_ptr->protevil)
	{
#ifdef JP
info[i++] = "���ʤ��ϼٰ��ʤ�¸�ߤ������Ƥ��롣";
#else
		info[i++] = "You are protected from evil.";
#endif

	}
	if (p_ptr->shield)
	{
#ifdef JP
info[i++] = "���ʤ��Ͽ���Υ�����ɤǼ���Ƥ��롣";
#else
		info[i++] = "You are protected by a mystic shield.";
#endif

	}
	if (p_ptr->invuln)
	{
#ifdef JP
info[i++] = "���ʤ��ϸ��߽��Ĥ��ʤ���";
#else
		info[i++] = "You are temporarily invulnerable.";
#endif

	}
	if (p_ptr->wraith_form)
	{
#ifdef JP
info[i++] = "���ʤ��ϰ��Ū��ͩ�β����Ƥ��롣";
#else
		info[i++] = "You are temporarily incorporeal.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
#ifdef JP
info[i++] = "���ʤ��μ���֤������Ƥ��롣";
#else
		info[i++] = "Your hands are glowing dull red.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_FIRE)
	{
#ifdef JP
info[i++] = "���ʤ��μ�ϲб��ʤ���Ƥ��롣";
#else
		info[i++] = "You can strike the enemy with flame.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_COLD)
	{
#ifdef JP
info[i++] = "���ʤ��μ���䵤��ʤ���Ƥ��롣";
#else
		info[i++] = "You can strike the enemy with cold.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_ACID)
	{
#ifdef JP
info[i++] = "���ʤ��μ�ϻ���ʤ���Ƥ��롣";
#else
		info[i++] = "You can strike the enemy with acid.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_ELEC)
	{
#ifdef JP
info[i++] = "���ʤ��μ���ŷ��ʤ���Ƥ��롣";
#else
		info[i++] = "You can strike the enemy with electoric shock.";
#endif

	}
	if (p_ptr->special_attack & ATTACK_POIS)
	{
#ifdef JP
info[i++] = "���ʤ��μ���Ǥ�ʤ���Ƥ��롣";
#else
		info[i++] = "You can strike the enemy with poison.";
#endif

	}
	switch (p_ptr->action)
	{
		case ACTION_SEARCH:
#ifdef JP
info[i++] = "���ʤ��ϤҤ��礦����տ������Ϥ��Ϥ��Ƥ��롣";
#else
			info[i++] = "You are looking around very carefully.";
#endif
			break;
	}
	if (p_ptr->new_spells)
	{
#ifdef JP
info[i++] = "���ʤ��ϼ�ʸ�䵧���ؤ֤��Ȥ��Ǥ��롣";
#else
		info[i++] = "You can learn some spells/prayers.";
#endif

	}
	if (p_ptr->word_recall)
	{
#ifdef JP
info[i++] = "���ʤ��Ϥ����˵��Ԥ��������";
#else
		info[i++] = "You will soon be recalled.";
#endif

	}
	if (p_ptr->see_infra)
	{
#ifdef JP
info[i++] = "���ʤ���Ʒ���ֳ������Ҵ��Ǥ��롣";
#else
		info[i++] = "Your eyes are sensitive to infrared light.";
#endif

	}
	if (p_ptr->see_inv)
	{
#ifdef JP
info[i++] = "���ʤ���Ʃ���ʥ�󥹥����򸫤뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "You can see invisible creatures.";
#endif

	}
	if (p_ptr->ffall)
	{
#ifdef JP
info[i++] = "���ʤ������֤��Ȥ��Ǥ��롣";
#else
		info[i++] = "You can fly.";
#endif

	}
	if (p_ptr->free_act)
	{
#ifdef JP
info[i++] = "���ʤ��������Τ餺�θ��̤���äƤ��롣";
#else
		info[i++] = "You have free action.";
#endif

	}
	if (p_ptr->regenerate)
	{
#ifdef JP
info[i++] = "���ʤ������᤯���Ϥ�������롣";
#else
		info[i++] = "You regenerate quickly.";
#endif

	}
	if (p_ptr->slow_digest)
	{
#ifdef JP
info[i++] = "���ʤ��Ͽ��ߤ����ʤ���";
#else
		info[i++] = "Your appetite is small.";
#endif

	}
	if (p_ptr->telepathy)
	{
#ifdef JP
info[i++] = "���ʤ��ϥƥ�ѥ���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You have ESP.";
#endif

	}
	if (p_ptr->esp_animal)
	{
#ifdef JP
info[i++] = "���ʤ��ϼ���������ʪ��¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense natural creatures.";
#endif

	}
	if (p_ptr->esp_undead)
	{
#ifdef JP
info[i++] = "���ʤ��ϥ���ǥåɤ�¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense undead.";
#endif

	}
	if (p_ptr->esp_demon)
	{
#ifdef JP
info[i++] = "���ʤ��ϰ����¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense demons.";
#endif

	}
	if (p_ptr->esp_orc)
	{
#ifdef JP
info[i++] = "���ʤ��ϥ�������¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense orcs.";
#endif

	}
	if (p_ptr->esp_troll)
	{
#ifdef JP
info[i++] = "���ʤ��ϥȥ���¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense trolls.";
#endif

	}
	if (p_ptr->esp_giant)
	{
#ifdef JP
info[i++] = "���ʤ��ϵ�ͤ�¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense giants.";
#endif

	}
	if (p_ptr->esp_dragon)
	{
#ifdef JP
info[i++] = "���ʤ��ϥɥ饴���¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense dragons.";
#endif

	}
	if (p_ptr->esp_human)
	{
#ifdef JP
info[i++] = "���ʤ��Ͽʹ֤�¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense humans.";
#endif

	}
	if (p_ptr->esp_evil)
	{
#ifdef JP
info[i++] = "���ʤ��ϼٰ�������ʪ��¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense evil creatures.";
#endif

	}
	if (p_ptr->esp_good)
	{
#ifdef JP
info[i++] = "���ʤ������ɤ�����ʪ��¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense good creatures.";
#endif

	}
	if (p_ptr->esp_nonliving)
	{
#ifdef JP
info[i++] = "���ʤ��ϳ�ư����̵��ʪ�Τ�¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense non-living creatures.";
#endif

	}
	if (p_ptr->esp_unique)
	{
#ifdef JP
info[i++] = "���ʤ������̤ʶ�Ũ��¸�ߤ򴶤���ǽ�Ϥ���äƤ��롣";
#else
		info[i++] = "You sense unique monsters.";
#endif

	}
	if (p_ptr->hold_life)
	{
#ifdef JP
info[i++] = "���ʤ��ϼ��ʤ���̿�Ϥ򤷤ä���Ȱݻ����롣";
#else
		info[i++] = "You have a firm hold on your life force.";
#endif

	}
	if (p_ptr->reflect)
	{
#ifdef JP
info[i++] = "���ʤ������ܥ�Ȥ�ȿ�ͤ��롣";
#else
		info[i++] = "You reflect arrows and bolts.";
#endif

	}
	if (p_ptr->sh_fire)
	{
#ifdef JP
info[i++] = "���ʤ��ϱ�Υ��������ޤ�Ƥ��롣";
#else
		info[i++] = "You are surrounded with a fiery aura.";
#endif

	}
	if (p_ptr->sh_elec)
	{
#ifdef JP
info[i++] = "���ʤ����ŵ�����ޤ�Ƥ��롣";
#else
		info[i++] = "You are surrounded with electricity.";
#endif

	}
	if (p_ptr->sh_cold)
	{
#ifdef JP
info[i++] = "���ʤ����䵤�Υ��������ޤ�Ƥ��롣";
#else
		info[i++] = "You are surrounded with an aura of coldness.";
#endif

	}
	if (p_ptr->tim_sh_holy)
	{
#ifdef JP
info[i++] = "���ʤ������ʤ륪�������ޤ�Ƥ��롣";
#else
		info[i++] = "You are surrounded with a holy aura.";
#endif

	}
	if (p_ptr->tim_sh_touki)
	{
#ifdef JP
info[i++] = "���ʤ���Ʈ���Υ��������ޤ�Ƥ��롣";
#else
		info[i++] = "You are surrounded with a energy aura.";
#endif

	}
	if (p_ptr->anti_magic)
	{
#ifdef JP
info[i++] = "���ʤ���ȿ��ˡ������ɤ���ޤ�Ƥ��롣";
#else
		info[i++] = "You are surrounded by an anti-magic shell.";
#endif

	}
	if (p_ptr->anti_tele)
	{
#ifdef JP
info[i++] = "���ʤ��ϥƥ�ݡ��ȤǤ��ʤ���";
#else
		info[i++] = "You cannot teleport.";
#endif

	}
	if (p_ptr->lite)
	{
#ifdef JP
info[i++] = "���ʤ��ο��Τϸ��äƤ��롣";
#else
		info[i++] = "You are carrying a permanent light.";
#endif

	}
	if (p_ptr->warning)
	{
#ifdef JP
info[i++] = "���ʤ��Ϲ�ư�����˴����Τ��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "You will be warned before dangerous actions.";
#endif

	}
	if (p_ptr->dec_mana)
	{
#ifdef JP
info[i++] = "���ʤ��Ͼ��ʤ��������Ϥ���ˡ�򾧤��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "You can cast spells with fewer mana points.";
#endif

	}
	if (p_ptr->easy_spell)
	{
#ifdef JP
info[i++] = "���ʤ����㤤����Ψ����ˡ�򾧤��뤳�Ȥ��Ǥ��롣";
#else
		info[i++] = "Fail rate of your magic is decreased.";
#endif

	}
	if (p_ptr->heavy_spell)
	{
#ifdef JP
info[i++] = "���ʤ��Ϲ⤤����Ψ����ˡ�򾧤��ʤ���Ф����ʤ���";
#else
		info[i++] = "Fail rate of your magic is increased.";
#endif

	}
	if (p_ptr->mighty_throw)
	{
#ifdef JP
info[i++] = "���ʤ��϶���ʪ���ꤲ�롣";
#else
		info[i++] = "You can throw objects powerfully.";
#endif

	}

	if (p_ptr->immune_acid)
	{
#ifdef JP
info[i++] = "���ʤ��ϻ����Ф��봰���ʤ��ȱ֤���äƤ��롣";
#else
		info[i++] = "You are completely immune to acid.";
#endif

	}
	else if ((p_ptr->resist_acid) && (p_ptr->oppose_acid))
	{
#ifdef JP
info[i++] = "���ʤ��ϻ��ؤζ��Ϥ���������äƤ��롣";
#else
		info[i++] = "You resist acid exceptionally well.";
#endif

	}
	else if ((p_ptr->resist_acid) || (p_ptr->oppose_acid))
	{
#ifdef JP
info[i++] = "���ʤ��ϻ��ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to acid.";
#endif

	}

	if (p_ptr->immune_elec)
	{
#ifdef JP
info[i++] = "���ʤ����ŷ���Ф��봰���ʤ��ȱ֤���äƤ��롣";
#else
		info[i++] = "You are completely immune to lightning.";
#endif

	}
	else if ((p_ptr->resist_elec) && (p_ptr->oppose_elec))
	{
#ifdef JP
info[i++] = "���ʤ����ŷ�ؤζ��Ϥ���������äƤ��롣";
#else
		info[i++] = "You resist lightning exceptionally well.";
#endif

	}
	else if ((p_ptr->resist_elec) || (p_ptr->oppose_elec))
	{
#ifdef JP
info[i++] = "���ʤ����ŷ�ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to lightning.";
#endif

	}

	if (prace_is_(RACE_ANDROID) && !p_ptr->immune_elec)
	{
#ifdef JP
info[i++] = "���ʤ����ŷ�˼夤��";
#else
		info[i++] = "You are susceptible to damage from lightning.";
#endif

	}

	if (p_ptr->immune_fire)
	{
#ifdef JP
info[i++] = "���ʤ��ϲФ��Ф��봰���ʤ��ȱ֤���äƤ��롣";
#else
		info[i++] = "You are completely immune to fire.";
#endif

	}
	else if ((p_ptr->resist_fire) && (p_ptr->oppose_fire))
	{
#ifdef JP
info[i++] = "���ʤ��ϲФؤζ��Ϥ���������äƤ��롣";
#else
		info[i++] = "You resist fire exceptionally well.";
#endif

	}
	else if ((p_ptr->resist_fire) || (p_ptr->oppose_fire))
	{
#ifdef JP
info[i++] = "���ʤ��ϲФؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to fire.";
#endif

	}

	if (prace_is_(RACE_ENT) && !p_ptr->immune_fire)
	{
#ifdef JP
info[i++] = "���ʤ��ϲФ˼夤��";
#else
		info[i++] = "You are susceptible to damage from fire.";
#endif

	}

	if (p_ptr->immune_cold)
	{
#ifdef JP
info[i++] = "���ʤ����䵤���Ф��봰���ʤ��ȱ֤���äƤ��롣";
#else
		info[i++] = "You are completely immune to cold.";
#endif

	}
	else if ((p_ptr->resist_cold) && (p_ptr->oppose_cold))
	{
#ifdef JP
info[i++] = "���ʤ����䵤�ؤζ��Ϥ���������äƤ��롣";
#else
		info[i++] = "You resist cold exceptionally well.";
#endif

	}
	else if ((p_ptr->resist_cold) || (p_ptr->oppose_cold))
	{
#ifdef JP
info[i++] = "���ʤ����䵤�ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to cold.";
#endif

	}

	if ((p_ptr->resist_pois) && (p_ptr->oppose_pois))
	{
#ifdef JP
info[i++] = "���ʤ����Ǥؤζ��Ϥ���������äƤ��롣";
#else
		info[i++] = "You resist poison exceptionally well.";
#endif

	}
	else if ((p_ptr->resist_pois) || (p_ptr->oppose_pois))
	{
#ifdef JP
info[i++] = "���ʤ����Ǥؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to poison.";
#endif

	}

	if (p_ptr->resist_lite)
	{
#ifdef JP
info[i++] = "���ʤ��������ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to bright light.";
#endif

	}

	if (prace_is_(RACE_VAMPIRE) || prace_is_(RACE_S_FAIRY) || (p_ptr->mimic_form == MIMIC_VAMPIRE))
	{
#ifdef JP
info[i++] = "���ʤ��������˼夤��";
#else
		info[i++] = "You are susceptible to damage from bright light.";
#endif

	}

	if (prace_is_(RACE_VAMPIRE) || (p_ptr->mimic_form == MIMIC_VAMPIRE) || p_ptr->wraith_form)
	{
#ifdef JP
info[i++] = "���ʤ��ϰŹ����Ф��봰���ʤ��ȱ֤���äƤ��롣";
#else
		info[i++] = "You are completely immune to darkness.";
#endif
	}

	else if (p_ptr->resist_dark)
	{
#ifdef JP
info[i++] = "���ʤ��ϰŹ��ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to darkness.";
#endif

	}
	if (p_ptr->resist_conf)
	{
#ifdef JP
info[i++] = "���ʤ��Ϻ���ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to confusion.";
#endif

	}
	if (p_ptr->resist_sound)
	{
#ifdef JP
info[i++] = "���ʤ��ϲ��Ȥξ׷�ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to sonic attacks.";
#endif

	}
	if (p_ptr->resist_disen)
	{
#ifdef JP
info[i++] = "���ʤ��������ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to disenchantment.";
#endif

	}
	if (p_ptr->resist_chaos)
	{
#ifdef JP
info[i++] = "���ʤ��ϥ��������Ϥؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to chaos.";
#endif

	}
	if (p_ptr->resist_shard)
	{
#ifdef JP
info[i++] = "���ʤ������Ҥι���ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to blasts of shards.";
#endif

	}
	if (p_ptr->resist_nexus)
	{
#ifdef JP
info[i++] = "���ʤ��ϰ��̺���ι���ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to nexus attacks.";
#endif

	}

	if (prace_is_(RACE_SPECTRE))
	{
#ifdef JP
info[i++] = "���ʤ����Ϲ����Ϥ�ۼ��Ǥ��롣";
#else
		info[i++] = "You can drain nether forces.";
#endif

	}
	else if (p_ptr->resist_neth)
	{
#ifdef JP
info[i++] = "���ʤ����Ϲ����Ϥؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to nether forces.";
#endif

	}
	if (p_ptr->resist_fear)
	{
#ifdef JP
info[i++] = "���ʤ����������ݤ򴶤��ʤ���";
#else
		info[i++] = "You are completely fearless.";
#endif

	}
	if (p_ptr->resist_blind)
	{
#ifdef JP
info[i++] = "���ʤ����ܤ����ܤؤ���������äƤ��롣";
#else
		info[i++] = "Your eyes are resistant to blindness.";
#endif

	}
	if (p_ptr->resist_time)
	{
#ifdef JP
info[i++] = "���ʤ��ϻ��ֵ�ž�ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to time.";
#endif

	}

	if (p_ptr->sustain_str)
	{
#ifdef JP
info[i++] = "���ʤ������Ϥϰݻ�����Ƥ��롣";
#else
		info[i++] = "Your strength is sustained.";
#endif

	}
	if (p_ptr->sustain_int)
	{
#ifdef JP
info[i++] = "���ʤ�����ǽ�ϰݻ�����Ƥ��롣";
#else
		info[i++] = "Your intelligence is sustained.";
#endif

	}
	if (p_ptr->sustain_wis)
	{
#ifdef JP
info[i++] = "���ʤ��θ����ϰݻ�����Ƥ��롣";
#else
		info[i++] = "Your wisdom is sustained.";
#endif

	}
	if (p_ptr->sustain_con)
	{
#ifdef JP
info[i++] = "���ʤ����ѵ��Ϥϰݻ�����Ƥ��롣";
#else
		info[i++] = "Your constitution is sustained.";
#endif

	}
	if (p_ptr->sustain_dex)
	{
#ifdef JP
info[i++] = "���ʤ��δ��Ѥ��ϰݻ�����Ƥ��롣";
#else
		info[i++] = "Your dexterity is sustained.";
#endif

	}
	if (p_ptr->sustain_chr)
	{
#ifdef JP
info[i++] = "���ʤ���̥�Ϥϰݻ�����Ƥ��롣";
#else
		info[i++] = "Your charisma is sustained.";
#endif

	}

	if (have_flag(flgs, TR_STR))
	{
#ifdef JP
info[i++] = "���ʤ������Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your strength is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_INT))
	{
#ifdef JP
info[i++] = "���ʤ�����ǽ�������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your intelligence is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_WIS))
	{
#ifdef JP
info[i++] = "���ʤ��θ����������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your wisdom is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_DEX))
	{
#ifdef JP
info[i++] = "���ʤ��δ��Ѥ��������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your dexterity is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_CON))
	{
#ifdef JP
info[i++] = "���ʤ����ѵ��Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your constitution is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_CHR))
	{
#ifdef JP
info[i++] = "���ʤ���̥�Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your charisma is affected by your equipment.";
#endif

	}

	if (have_flag(flgs, TR_STEALTH))
	{
#ifdef JP
info[i++] = "���ʤ��α�̩��ưǽ�Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your stealth is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_SEARCH))
	{
#ifdef JP
info[i++] = "���ʤ���õ��ǽ�Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your searching ability is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_INFRA))
	{
#ifdef JP
info[i++] = "���ʤ����ֳ������Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your infravision is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_TUNNEL))
	{
#ifdef JP
info[i++] = "���ʤ��κη�ǽ�Ϥ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your digging ability is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_SPEED))
	{
#ifdef JP
info[i++] = "���ʤ��Υ��ԡ��ɤ������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your speed is affected by your equipment.";
#endif

	}
	if (have_flag(flgs, TR_BLOWS))
	{
#ifdef JP
info[i++] = "���ʤ��ι���®�٤������ˤ�äƱƶ�������Ƥ��롣";
#else
		info[i++] = "Your attack speed is affected by your equipment.";
#endif

	}


	/* Access the current weapon */
	o_ptr = &inventory[INVEN_RARM];

	/* Analyze the weapon */
	if (o_ptr->k_idx)
	{
		/* Indicate Blessing */
		if (have_flag(flgs, TR_BLESSED))
		{
#ifdef JP
info[i++] = "���ʤ������Ͽ��ν�ʡ������Ƥ��롣";
#else
			info[i++] = "Your weapon has been blessed by the gods.";
#endif

		}

		if (have_flag(flgs, TR_CHAOTIC))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ��륹��ħ��°�����ġ�";
#else
			info[i++] = "Your weapon is branded with the Sign of Logrus.";
#endif

		}

		/* Hack */
		if (have_flag(flgs, TR_IMPACT))
		{
#ifdef JP
info[i++] = "���ʤ��������Ƿ���Ͽ̤�ȯ�����뤳�Ȥ��Ǥ��롣";
#else
			info[i++] = "The impact of your weapon can cause earthquakes.";
#endif

		}

		if (have_flag(flgs, TR_VORPAL))
		{
#ifdef JP
info[i++] = "���ʤ����������˱Ԥ���";
#else
			info[i++] = "Your weapon is very sharp.";
#endif

		}

		if (have_flag(flgs, TR_VAMPIRIC))
		{
#ifdef JP
info[i++] = "���ʤ�������Ũ������̿�Ϥ�ۼ����롣";
#else
			info[i++] = "Your weapon drains life from your foes.";
#endif

		}

		/* Special "Attack Bonuses" */
		if (have_flag(flgs, TR_BRAND_ACID))
		{
#ifdef JP
info[i++] = "���ʤ�������Ũ���Ϥ�����";
#else
			info[i++] = "Your weapon melts your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_ELEC))
		{
#ifdef JP
info[i++] = "���ʤ�������Ũ���Ť����롣";
#else
			info[i++] = "Your weapon shocks your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_FIRE))
		{
#ifdef JP
info[i++] = "���ʤ�������Ũ��ǳ�䤹��";
#else
			info[i++] = "Your weapon burns your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_COLD))
		{
#ifdef JP
info[i++] = "���ʤ�������Ũ����餻�롣";
#else
			info[i++] = "Your weapon freezes your foes.";
#endif

		}
		if (have_flag(flgs, TR_BRAND_POIS))
		{
#ifdef JP
info[i++] = "���ʤ�������Ũ���Ǥǿ�����";
#else
			info[i++] = "Your weapon poisons your foes.";
#endif

		}

		/* Special "slay" flags */
		if (have_flag(flgs, TR_KILL_ANIMAL))
		{
#ifdef JP
info[i++] = "���ʤ�������ưʪ��ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of animals.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_ANIMAL))
		{
#ifdef JP
info[i++] = "���ʤ�������ưʪ���Ф��ƶ����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon strikes at animals with extra force.";
#endif

		}
		if (have_flag(flgs, TR_KILL_EVIL))
		{
#ifdef JP
info[i++] = "���ʤ������ϼٰ��ʤ�¸�ߤ�ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of evil.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_EVIL))
		{
#ifdef JP
info[i++] = "���ʤ������ϼٰ��ʤ�¸�ߤ��Ф��ƶ����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon strikes at evil with extra force.";
#endif

		}
		if (have_flag(flgs, TR_KILL_HUMAN))
		{
#ifdef JP
info[i++] = "���ʤ������Ͽʹ֤�ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of humans.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_HUMAN))
		{
#ifdef JP
info[i++] = "���ʤ������Ͽʹ֤��Ф����ä˶����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon is especially deadly against humans.";
#endif

		}
		if (have_flag(flgs, TR_KILL_UNDEAD))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ���ǥåɤ�ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of undead.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_UNDEAD))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ���ǥåɤ��Ф��ƿ����ʤ��Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon strikes at undead with holy wrath.";
#endif

		}
		if (have_flag(flgs, TR_KILL_DEMON))
		{
#ifdef JP
info[i++] = "���ʤ������ϥǡ�����ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of demons.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_DEMON))
		{
#ifdef JP
info[i++] = "���ʤ������ϥǡ������Ф��ƿ����ʤ��Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon strikes at demons with holy wrath.";
#endif

		}
		if (have_flag(flgs, TR_KILL_ORC))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ�������ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of orcs.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_ORC))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ��������Ф����ä˶����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon is especially deadly against orcs.";
#endif

		}
		if (have_flag(flgs, TR_KILL_TROLL))
		{
#ifdef JP
info[i++] = "���ʤ������ϥȥ���ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of trolls.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_TROLL))
		{
#ifdef JP
info[i++] = "���ʤ������ϥȥ����Ф����ä˶����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon is especially deadly against trolls.";
#endif

		}
		if (have_flag(flgs, TR_KILL_GIANT))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ��㥤����Ȥ�ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of giants.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_GIANT))
		{
#ifdef JP
info[i++] = "���ʤ������ϥ��㥤����Ȥ��Ф����ä˶����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon is especially deadly against giants.";
#endif

		}
		/* Special "kill" flags */
		if (have_flag(flgs, TR_KILL_DRAGON))
		{
#ifdef JP
info[i++] = "���ʤ������ϥɥ饴���ŷŨ�Ǥ��롣";
#else
			info[i++] = "Your weapon is a great bane of dragons.";
#endif

		}
		else if (have_flag(flgs, TR_SLAY_DRAGON))
		{
#ifdef JP
info[i++] = "���ʤ������ϥɥ饴����Ф����ä˶����Ϥ�ȯ�����롣";
#else
			info[i++] = "Your weapon is especially deadly against dragons.";
#endif

		}

		if (have_flag(flgs, TR_FORCE_WEAPON))
		{
#ifdef JP
info[i++] = "���ʤ�������MP��Ȥäƹ��⤹�롣";
#else
			info[i++] = "Your weapon causes greate damages using your MP.";
#endif

		}
		if (have_flag(flgs, TR_THROW))
		{
#ifdef JP
info[i++] = "���ʤ��������ꤲ�䤹����";
#else
			info[i++] = "Your weapon can be thrown well.";
#endif
		}
	}


	/* Save the screen */
	screen_save();

	/* Erase the screen */
	for (k = 1; k < 24; k++) prt("", k, 13);

	/* Label the information */
#ifdef JP
prt("        ���ʤ��ξ���:", 1, 15);
#else
	prt("     Your Attributes:", 1, 15);
#endif


	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if ((k == 22) && (j+1 < i))
		{
#ifdef JP
prt("-- ³�� --", k, 15);
#else
			prt("-- more --", k, 15);
#endif

			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Pause */
#ifdef JP
prt("[���������򲡤��ȥ���������ޤ�]", k, 13);
#else
	prt("[Press any key to continue]", k, 13);
#endif

	inkey();

	/* Restore the screen */
	screen_load();
}


static int report_magics_aux(int dur)
{
	if (dur <= 5)
	{
		return 0;
	}
	else if (dur <= 10)
	{
		return 1;
	}
	else if (dur <= 20)
	{
		return 2;
	}
	else if (dur <= 50)
	{
		return 3;
	}
	else if (dur <= 100)
	{
		return 4;
	}
	else if (dur <= 200)
	{
		return 5;
	}
	else
	{
		return 6;
	}
}

static cptr report_magic_durations[] =
{
#ifdef JP
"����û����",
"�����δ�",
"���Ф餯�δ�",
"¿��Ĺ����",
"Ĺ����",
"����Ĺ����",
"�����񤤤ۤ�Ĺ����",
"��󥹥����򹶷⤹��ޤ�"
#else
	"for a short time",
	"for a little while",
	"for a while",
	"for a long while",
	"for a long time",
	"for a very long time",
	"for an incredibly long time",
	"until you hit a monster"
#endif

};


/*
 * Report all currently active magical effects.
 */
void report_magics(void)
{
	int     i = 0, j, k;
	char    Dummy[80];
	cptr    info[128];
	int     info2[128];


	if (p_ptr->blind)
	{
		info2[i]  = report_magics_aux(p_ptr->blind);
#ifdef JP
info[i++] = "���ʤ����ܤ������ʤ���";
#else
		info[i++] = "You cannot see";
#endif

	}
	if (p_ptr->confused)
	{
		info2[i]  = report_magics_aux(p_ptr->confused);
#ifdef JP
info[i++] = "���ʤ��Ϻ��𤷤Ƥ��롣";
#else
		info[i++] = "You are confused";
#endif

	}
	if (p_ptr->afraid)
	{
		info2[i]  = report_magics_aux(p_ptr->afraid);
#ifdef JP
info[i++] = "���ʤ��϶��ݤ˿�����Ƥ��롣";
#else
		info[i++] = "You are terrified";
#endif

	}
	if (p_ptr->poisoned)
	{
		info2[i]  = report_magics_aux(p_ptr->poisoned);
#ifdef JP
info[i++] = "���ʤ����Ǥ˿�����Ƥ��롣";
#else
		info[i++] = "You are poisoned";
#endif

	}
	if (p_ptr->image)
	{
		info2[i]  = report_magics_aux(p_ptr->image);
#ifdef JP
info[i++] = "���ʤ��ϸ��Ф򸫤Ƥ��롣";
#else
		info[i++] = "You are hallucinating";
#endif

	}
	if (p_ptr->blessed)
	{
		info2[i]  = report_magics_aux(p_ptr->blessed);
#ifdef JP
info[i++] = "���ʤ��ϸ������򴶤��Ƥ��롣";
#else
		info[i++] = "You feel rightous";
#endif

	}
	if (p_ptr->hero)
	{
		info2[i]  = report_magics_aux(p_ptr->hero);
#ifdef JP
info[i++] = "���ʤ��ϥҡ�����ʬ����";
#else
		info[i++] = "You feel heroic";
#endif

	}
	if (p_ptr->shero)
	{
		info2[i]  = report_magics_aux(p_ptr->shero);
#ifdef JP
info[i++] = "���ʤ�����Ʈ������";
#else
		info[i++] = "You are in a battle rage";
#endif

	}
	if (p_ptr->protevil)
	{
		info2[i]  = report_magics_aux(p_ptr->protevil);
#ifdef JP
info[i++] = "���ʤ��ϼٰ��ʤ�¸�ߤ������Ƥ��롣";
#else
		info[i++] = "You are protected from evil";
#endif

	}
	if (p_ptr->shield)
	{
		info2[i]  = report_magics_aux(p_ptr->shield);
#ifdef JP
info[i++] = "���ʤ��Ͽ���Υ�����ɤǼ���Ƥ��롣";
#else
		info[i++] = "You are protected by a mystic shield";
#endif

	}
	if (p_ptr->invuln)
	{
		info2[i]  = report_magics_aux(p_ptr->invuln);
#ifdef JP
info[i++] = "̵Ũ�Ǥ����롣";
#else
		info[i++] = "You are invulnerable";
#endif

	}
	if (p_ptr->wraith_form)
	{
		info2[i]  = report_magics_aux(p_ptr->wraith_form);
#ifdef JP
info[i++] = "ͩ�β��Ǥ��롣";
#else
		info[i++] = "You are incorporeal";
#endif

	}
	if (p_ptr->special_attack & ATTACK_CONFUSE)
	{
		info2[i]  = 7;
#ifdef JP
info[i++] = "���ʤ��μ���֤������Ƥ��롣";
#else
		info[i++] = "Your hands are glowing dull red.";
#endif

	}
	if (p_ptr->word_recall)
	{
		info2[i]  = report_magics_aux(p_ptr->word_recall);
#ifdef JP
info[i++] = "���θ嵢�Ԥξۤ�ȯư���롣";
#else
		info[i++] = "You are waiting to be recalled";
#endif

	}
	if (p_ptr->oppose_acid)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_acid);
#ifdef JP
info[i++] = "���ʤ��ϻ��ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to acid";
#endif

	}
	if (p_ptr->oppose_elec)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_elec);
#ifdef JP
info[i++] = "���ʤ����ŷ�ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to lightning";
#endif

	}
	if (p_ptr->oppose_fire)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_fire);
#ifdef JP
info[i++] = "���ʤ��ϲФؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to fire";
#endif

	}
	if (p_ptr->oppose_cold)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_cold);
#ifdef JP
info[i++] = "���ʤ����䵤�ؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to cold";
#endif

	}
	if (p_ptr->oppose_pois)
	{
		info2[i]  = report_magics_aux(p_ptr->oppose_pois);
#ifdef JP
info[i++] = "���ʤ����Ǥؤ���������äƤ��롣";
#else
		info[i++] = "You are resistant to poison";
#endif

	}

	/* Save the screen */
	screen_save();

	/* Erase the screen */
	for (k = 1; k < 24; k++) prt("", k, 13);

	/* Label the information */
#ifdef JP
prt("           ��ˡ        :", 1, 15);
#else
	prt("     Your Current Magic:", 1, 15);
#endif


	/* We will print on top of the map (column 13) */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
#ifdef JP
sprintf(Dummy, "���ʤ���%s%s", info[j],
#else
		sprintf(Dummy, "%s %s.", info[j],
#endif

			report_magic_durations[info2[j]]);
		prt(Dummy, k++, 15);

		/* Every 20 entries (lines 2 to 21), start over */
		if ((k == 22) && (j + 1 < i))
		{
#ifdef JP
prt("-- ³�� --", k, 15);
#else
			prt("-- more --", k, 15);
#endif

			inkey();
			for (; k > 2; k--) prt("", k, 15);
		}
	}

	/* Pause */
#ifdef JP
prt("[���������򲡤��ȥ���������ޤ�]", k, 13);
#else
	prt("[Press any key to continue]", k, 13);
#endif

	inkey();

	/* Restore the screen */
	screen_load();
}


/*
 * Detect all traps on current panel
 */
bool detect_traps(int range, bool known)
{
	int             x, y;
	bool            detect = FALSE;
	cave_type       *c_ptr;


	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x <= cur_wid - 1; x++)
		{
			int dist = distance(py, px, y, x);
			if (dist > range) continue;

			/* Access the grid */
			c_ptr = &cave[y][x];

			/* Mark as detected */
			if (dist <= range && known)
			{
				if (dist <= range - 1)
					c_ptr->info |= (CAVE_IN_DETECT);

				c_ptr->info &= ~(CAVE_UNSAFE);

				/* Redraw */
				lite_spot(y, x);
			}

			/* Detect traps */
			if (is_trap(c_ptr->feat))
			{
				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				if (c_ptr->mimic)
				{
					/* Disclose a hidden trap */
					disclose_grid(y, x);
				}
				else
				{
					/* Redraw */
					lite_spot(y, x);
				}

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	if (known) p_ptr->dtrap = TRUE;

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT)) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("�ȥ�åפ�¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of traps!");
#endif

	}

	/* Result */
	return (detect);
}



/*
 * Detect all doors on current panel
 */
bool detect_doors(int range)
{
	int y, x;

	bool detect = FALSE;

	cave_type *c_ptr;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the panel */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if (distance(py, px, y, x) > range) continue;

			c_ptr = &cave[y][x];

			/* Detect secret doors */
			if (is_hidden_door(c_ptr))
			{
				/* Pick a door */
				disclose_grid(y, x);
			}

			/* Detect doors */
			if (((c_ptr->feat >= FEAT_DOOR_HEAD) &&
			     (c_ptr->feat <= FEAT_DOOR_TAIL)) ||
			    ((c_ptr->feat == FEAT_OPEN) ||
			     (c_ptr->feat == FEAT_BROKEN)))
			{
				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT)) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("�ɥ���¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of doors!");
#endif

	}

	/* Result */
	return (detect);
}


/*
 * Detect all stairs on current panel
 */
bool detect_stairs(int range)
{
	int y, x;

	bool detect = FALSE;

	cave_type *c_ptr;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the panel */
	for (y = 1; y < cur_hgt - 1; y++)
	{
		for (x = 1; x < cur_wid - 1; x++)
		{
			if (distance(py, px, y, x) > range) continue;

			c_ptr = &cave[y][x];

			/* Detect stairs */
			if ((c_ptr->feat == FEAT_LESS) ||
			    (c_ptr->feat == FEAT_LESS_LESS) ||
			    (c_ptr->feat == FEAT_MORE) ||
			    (c_ptr->feat == FEAT_MORE_MORE) ||
			    (c_ptr->feat == FEAT_ENTRANCE))
			{
				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT)) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("���ʤ�¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of stairs!");
#endif

	}

	/* Result */
	return (detect);
}


/*
 * Detect any treasure on the current panel
 */
bool detect_treasure(int range)
{
	int y, x;

	bool detect = FALSE;

	cave_type *c_ptr;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan the current panel */
	for (y = 1; y < cur_hgt; y++)
	{
		for (x = 1; x < cur_wid; x++)
		{
			if (distance(py, px, y, x) > range) continue;

			c_ptr = &cave[y][x];

			/* Notice embedded gold */
			if ((c_ptr->feat == FEAT_MAGMA_H) ||
			    (c_ptr->feat == FEAT_QUARTZ_H))
			{
				/* Expose the gold */
				c_ptr->feat += 0x02;
			}

			/* Magma/Quartz + Known Gold */
			if ((c_ptr->feat == FEAT_MAGMA_K) ||
			    (c_ptr->feat == FEAT_QUARTZ_K))
			{
				/* Hack -- Memorize */
				c_ptr->info |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Detect */
				detect = TRUE;
			}
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT+6)) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("��¢���줿������¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of buried treasure!");
#endif

	}


	/* Result */
	return (detect);
}



/*
 * Detect all "gold" objects on the current panel
 */
bool detect_objects_gold(int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range2) continue;

		/* Detect "gold" objects */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT+6)) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("������¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of treasure!");
#endif

	}

	if (detect_monsters_string(range, "$"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "normal" objects on the current panel
 */
bool detect_objects_normal(int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range2) continue;

		/* Detect "real" objects */
		if (o_ptr->tval != TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT+6)) detect = FALSE;

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("�����ƥ��¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of objects!");
#endif

	}

	if (detect_monsters_string(range, "!=?|/`"))
	{
		detect = TRUE;
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "magic" objects on the current panel.
 *
 * This will light up all spaces with "magic" items, including artifacts,
 * ego-items, potions, scrolls, books, rods, wands, staves, amulets, rings,
 * and "enchanted" items of the "good" variety.
 *
 * It can probably be argued that this function is now too powerful.
 */
bool detect_objects_magic(int range)
{
	int i, y, x, tv;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range) continue;

		/* Examine the tval */
		tv = o_ptr->tval;

		/* Artifacts, misc magic items, or enchanted wearables */
		if (artifact_p(o_ptr) ||
			ego_item_p(o_ptr) ||
			o_ptr->art_name ||
		    (tv == TV_WHISTLE) ||
		    (tv == TV_AMULET) ||
			(tv == TV_RING) ||
		    (tv == TV_STAFF) ||
			(tv == TV_WAND) ||
			(tv == TV_ROD) ||
		    (tv == TV_SCROLL) ||
			(tv == TV_POTION) ||
		    (tv == TV_LIFE_BOOK) ||
			(tv == TV_SORCERY_BOOK) ||
		    (tv == TV_NATURE_BOOK) ||
			(tv == TV_CHAOS_BOOK) ||
		    (tv == TV_DEATH_BOOK) ||
		    (tv == TV_TRUMP_BOOK) ||
			(tv == TV_ARCANE_BOOK) ||
			(tv == TV_ENCHANT_BOOK) ||
			(tv == TV_DAEMON_BOOK) ||
			(tv == TV_CRUSADE_BOOK) ||
			(tv == TV_MUSIC_BOOK) ||
			(tv == TV_HISSATSU_BOOK) ||
		    ((o_ptr->to_a > 0) || (o_ptr->to_h + o_ptr->to_d > 0)))
		{
			/* Memorize the item */
			o_ptr->marked |= OM_FOUND;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	/* Describe */
	if (detect)
	{
#ifdef JP
msg_print("��ˡ�Υ����ƥ��¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of magic objects!");
#endif

	}

	/* Return result */
	return (detect);
}


/*
 * Detect all "normal" monsters on the current panel
 */
bool detect_monsters_normal(int range)
{
	int i, y, x;

	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect all non-invisible monsters */
		if ((!(r_ptr->flags2 & RF2_INVISIBLE)) ||
		    p_ptr->see_inv || p_ptr->tim_invis)
		{
			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT+3)) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("��󥹥�����¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of monsters!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all "invisible" monsters around the player
 */
bool detect_monsters_invis(int range)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect invisible monsters */
		if (r_ptr->flags2 & RF2_INVISIBLE)
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT+3)) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("Ʃ������ʪ��¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of invisible creatures!");
#endif

	}

	/* Result */
	return (flag);
}



/*
 * Detect all "evil" monsters on current panel
 */
bool detect_monsters_evil(int range)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect evil monsters */
		if (r_ptr->flags3 & RF3_EVIL)
		{
			/* Take note that they are evil */
			r_ptr->r_flags3 |= (RF3_EVIL);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("�ٰ��ʤ���ʪ��¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of evil creatures!");
#endif

	}

	/* Result */
	return (flag);
}




/*
 * Detect all "nonliving", "undead" or "demonic" monsters on current panel
 */
bool detect_monsters_nonliving(int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect non-living monsters */
		if (!monster_living(r_ptr))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("�����Ǥʤ���󥹥�����¸�ߤ򴶤�����");
#else
		msg_print("You sense the presence of unnatural beings!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all monsters it has mind on current panel
 */
bool detect_monsters_mind(int range)
{
	int     i, y, x;
	bool    flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect non-living monsters */
		if (!(r_ptr->flags2 & RF2_EMPTY_MIND))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("�����򴶤��Ȥä���");
#else
		msg_print("You sense the presence of someone's mind!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * Detect all (string) monsters on current panel
 */
bool detect_monsters_string(int range, cptr Match)
{
	int i, y, x;
	bool flag = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect monsters with the same symbol */
		if (strchr(Match, r_ptr->d_char))
		{
			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	if ((p_ptr->pclass == CLASS_BARD) && (p_ptr->magic_num1[0] > MUSIC_DETECT+3)) flag = FALSE;

	/* Describe */
	if (flag)
	{
		/* Describe result */
#ifdef JP
msg_print("��󥹥�����¸�ߤ򴶤��Ȥä���");
#else
		msg_print("You sense the presence of monsters!");
#endif

	}

	/* Result */
	return (flag);
}


/*
 * A "generic" detect monsters routine, tagged to flags3
 */
bool detect_monsters_xxx(int range, u32b match_flag)
{
	int  i, y, x;
	bool flag = FALSE;
#ifdef JP
cptr desc_monsters = "�Ѥʥ�󥹥���";
#else
	cptr desc_monsters = "weird monsters";
#endif

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range /= 3;

	/* Scan monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (distance(py, px, y, x) > range) continue;

		/* Detect evil monsters */
		if (r_ptr->flags3 & (match_flag))
		{
			/* Take note that they are something */
			r_ptr->r_flags3 |= (match_flag);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Repair visibility later */
			repair_monsters = TRUE;

			/* Hack -- Detect monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		switch (match_flag)
		{
			case RF3_DEMON:
#ifdef JP
desc_monsters = "�ǡ����";
#else
				desc_monsters = "demons";
#endif

				break;
			case RF3_UNDEAD:
#ifdef JP
desc_monsters = "����ǥå�";
#else
				desc_monsters = "the undead";
#endif

				break;
		}

		/* Describe result */
#ifdef JP
msg_format("%s��¸�ߤ򴶤��Ȥä���", desc_monsters);
#else
		msg_format("You sense the presence of %s!", desc_monsters);
#endif

		msg_print(NULL);
	}

	/* Result */
	return (flag);
}


/*
 * Detect everything
 */
bool detect_all(int range)
{
	bool detect = FALSE;

	/* Detect everything */
	if (detect_traps(range, TRUE)) detect = TRUE;
	if (detect_doors(range)) detect = TRUE;
	if (detect_stairs(range)) detect = TRUE;
	if (detect_treasure(range)) detect = TRUE;
	if (detect_objects_gold(range)) detect = TRUE;
	if (detect_objects_normal(range)) detect = TRUE;
	if (detect_monsters_invis(range)) detect = TRUE;
	if (detect_monsters_normal(range)) detect = TRUE;

	/* Result */
	return (detect);
}


/*
 * Apply a "project()" directly to all viewable monsters
 *
 * Note that affected monsters are NOT auto-tracked by this usage.
 *
 * To avoid misbehavior when monster deaths have side-effects,
 * this is done in two passes. -- JDL
 */
bool project_hack(int typ, int dam)
{
	int     i, x, y;
	int     flg = PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE;
	bool    obvious = FALSE;


	/* Mark all (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Require line of sight */
		if (!player_has_los_bold(y, x)) continue;

		/* Mark the monster */
		m_ptr->mflag |= (MFLAG_TEMP);
	}

	/* Affect all marked monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];

		/* Skip unmarked monsters */
		if (!(m_ptr->mflag & (MFLAG_TEMP))) continue;

		/* Remove mark */
		m_ptr->mflag &= ~(MFLAG_TEMP);

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Jump directly to the target monster */
		if (project(0, 0, y, x, dam, typ, flg, -1)) obvious = TRUE;
	}

	/* Result */
	return (obvious);
}


/*
 * Speed monsters
 */
bool speed_monsters(void)
{
	return (project_hack(GF_OLD_SPEED, p_ptr->lev));
}

/*
 * Slow monsters
 */
bool slow_monsters(void)
{
	return (project_hack(GF_OLD_SLOW, p_ptr->lev));
}

/*
 * Sleep monsters
 */
bool sleep_monsters(void)
{
	return (project_hack(GF_OLD_SLEEP, p_ptr->lev));
}


/*
 * Banish evil monsters
 */
bool banish_evil(int dist)
{
	return (project_hack(GF_AWAY_EVIL, dist));
}


/*
 * Turn undead
 */
bool turn_undead(void)
{
	bool tester = (project_hack(GF_TURN_UNDEAD, p_ptr->lev));
	if (tester)
		chg_virtue(V_UNLIFE, -1);
	return tester;
}


/*
 * Dispel undead monsters
 */
bool dispel_undead(int dam)
{
	bool tester = (project_hack(GF_DISP_UNDEAD, dam));
	if (tester)
		chg_virtue(V_UNLIFE, -2);
	return tester;
}

/*
 * Dispel evil monsters
 */
bool dispel_evil(int dam)
{
	return (project_hack(GF_DISP_EVIL, dam));
}

/*
 * Dispel good monsters
 */
bool dispel_good(int dam)
{
	return (project_hack(GF_DISP_GOOD, dam));
}

/*
 * Dispel all monsters
 */
bool dispel_monsters(int dam)
{
	return (project_hack(GF_DISP_ALL, dam));
}

/*
 * Dispel 'living' monsters
 */
bool dispel_living(int dam)
{
	return (project_hack(GF_DISP_LIVING, dam));
}

/*
 * Dispel demons
 */
bool dispel_demons(int dam)
{
	return (project_hack(GF_DISP_DEMON, dam));
}


/*
 * Crusade
 */
bool crusade(void)
{
	return (project_hack(GF_CRUSADE, p_ptr->lev*4));
}


/*
 * Wake up all monsters, and speed up "los" monsters.
 */
void aggravate_monsters(int who)
{
	int     i;
	bool    sleep = FALSE;
	bool    speed = FALSE;


	/* Aggravate everyone nearby */
	for (i = 1; i < m_max; i++)
	{
		monster_type    *m_ptr = &m_list[i];
/*		monster_race    *r_ptr = &r_info[m_ptr->r_idx]; */

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip aggravating monster (or player) */
		if (i == who) continue;

		/* Wake up nearby sleeping monsters */
		if (m_ptr->cdis < MAX_SIGHT * 2)
		{
			/* Wake up */
			if (m_ptr->csleep)
			{
				/* Wake up */
				m_ptr->csleep = 0;
				if (r_info[m_ptr->r_idx].flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);
				sleep = TRUE;
			}
			if (!is_pet(m_ptr)) m_ptr->mflag2 |= MFLAG_NOPET;
		}

		/* Speed up monsters in line of sight */
		if (player_has_los_bold(m_ptr->fy, m_ptr->fx))
		{
			if (!is_pet(m_ptr))
			{
				m_ptr->fast = MIN(200, m_ptr->fast + 100);
				speed = TRUE;
			}
		}
	}

	/* Messages */
#ifdef JP
if (speed) msg_print("�ն�ǲ�������ǡ��ʳ�����褦�ʴ������������");
else if (sleep) msg_print("��������ǡ��ʳ�����褦���������������󤯤�ʹ��������");
#else
	if (speed) msg_print("You feel a sudden stirring nearby!");
	else if (sleep) msg_print("You hear a sudden stirring in the distance!");
#endif
	if (p_ptr->riding) p_ptr->update |= PU_BONUS;
}



/*
 * Delete all non-unique/non-quest monsters of a given "type" from the level
 */
bool symbol_genocide(int power, int player_cast)
{
	int     i;
	char    typ;
	bool    result = FALSE;
	int     msec = delay_factor * delay_factor * delay_factor;

	/* Prevent genocide in quest levels */
	if (p_ptr->inside_quest && !random_quest_number(dun_level))
	{
		return (FALSE);
	}

	/* Mega-Hack -- Get a monster symbol */
#ifdef JP
while(!get_com("�ɤμ���(ʸ��)�Υ�󥹥������������ޤ���: ", &typ, FALSE));
#else
	while(!get_com("Choose a monster race (by symbol) to genocide: ", &typ, FALSE));
#endif


	/* Delete the monsters of that "type" */
	for (i = 1; i < m_max; i++)
	{
		monster_type    *m_ptr = &m_list[i];
		monster_race    *r_ptr = &r_info[m_ptr->r_idx];
		bool angry = FALSE;
		char m_name[80];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip "wrong" monsters */
		if (r_ptr->d_char != typ) continue;

		if (is_pet(m_ptr) && !player_cast) continue;

		/* Hack -- Skip Unique Monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE)) angry = TRUE;

		/* Hack -- Skip Quest Monsters */
		else if (r_ptr->flags1 & RF1_QUESTOR) angry = TRUE;

		else if (r_ptr->flags7 & RF7_UNIQUE2) angry = TRUE;

		else if (i == p_ptr->riding) angry = TRUE;

		else if (player_cast && (r_ptr->level > randint0(power))) angry = TRUE;

		else if (player_cast && (m_ptr->mflag2 & MFLAG_NOGENO)) angry = TRUE;

		/* Delete the monster */
		else delete_monster_idx(i);

		if (angry && player_cast)
		{
			monster_desc(m_name, m_ptr, 0);
			if (m_ptr->ml && !p_ptr->blind)
			{
#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
				msg_format("%^s is unaffected.", m_name);
#endif
			}
			if (m_ptr->csleep)
			{
				m_ptr->csleep = 0;
				if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);
				if (m_ptr->ml && !p_ptr->blind)
				{
#ifdef JP
msg_format("%^s���ܤ�Фޤ�����", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}
			if (is_friendly(m_ptr) && !is_pet(m_ptr))
			{
				if (m_ptr->ml && !p_ptr->blind)
				{
#ifdef JP
					msg_format("%s���ܤä���", m_name);
#else
					msg_format("%^s gets angry!", m_name);
#endif
				}
				set_hostile(m_ptr);
			}
			if (one_in_(13)) m_ptr->mflag2 |= MFLAG_NOGENO;
		}

		if (player_cast)
		{
			/* Take damage */
#ifdef JP
take_hit(DAMAGE_GENO, randint1(4), "�����μ�ʸ�򾧤�����ϫ", -1);
#else
			take_hit(DAMAGE_GENO, randint1(4), "the strain of casting Genocide", -1);
#endif

		}

		/* Visual feedback */
		move_cursor_relative(py, px);

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Handle */
		handle_stuff();

		/* Fresh */
		Term_fresh();

		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, msec);

		/* Take note */
		result = TRUE;
	}
	if (result)
	{
		chg_virtue(V_VITALITY, -2);
		chg_virtue(V_CHANCE, -1);
	}

	return (result);
}


/*
 * Delete all nearby (non-unique) monsters
 */
bool mass_genocide(int power, int player_cast)
{
	int     i;
	bool    result = FALSE;
	int     msec = delay_factor * delay_factor * delay_factor;


	/* Prevent mass genocide in quest levels */
	if (p_ptr->inside_quest && !random_quest_number(dun_level))
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type    *m_ptr = &m_list[i];
		monster_race    *r_ptr = &r_info[m_ptr->r_idx];
		bool angry = FALSE;
		char m_name[80];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

		if (is_pet(m_ptr) && !player_cast) continue;

		/* Hack -- Skip unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE)) angry = TRUE;

		/* Hack -- Skip Quest Monsters */
		else if (r_ptr->flags1 & RF1_QUESTOR) angry = TRUE;

		else if (r_ptr->flags7 & RF7_UNIQUE2) angry = TRUE;

		else if (i == p_ptr->riding) angry = TRUE;

		else if (player_cast && (r_ptr->level > randint0(power))) angry = TRUE;

		else if (player_cast && (m_ptr->mflag2 & MFLAG_NOGENO)) angry = TRUE;

		/* Delete the monster */
		else delete_monster_idx(i);

		if (angry && player_cast)
		{
			monster_desc(m_name, m_ptr, 0);
			if (m_ptr->ml && !p_ptr->blind)
			{
#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
				msg_format("%^s is unaffected.", m_name);
#endif
			}
			if (m_ptr->csleep)
			{
				m_ptr->csleep = 0;
				if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);
				if (m_ptr->ml && !p_ptr->blind)
				{
#ifdef JP
msg_format("%^s���ܤ�Фޤ�����", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}
			if (is_friendly(m_ptr) && !is_pet(m_ptr))
			{
				if (m_ptr->ml && !p_ptr->blind)
				{
#ifdef JP
					msg_format("%s���ܤä���", m_name);
#else
					msg_format("%^s gets angry!", m_name);
#endif
				}
				set_hostile(m_ptr);
			}
			if (one_in_(13)) m_ptr->mflag2 |= MFLAG_NOGENO;
		}

		if (player_cast)
		{
			/* Hack -- visual feedback */
#ifdef JP
take_hit(DAMAGE_GENO, randint1(3), "���������μ�ʸ�򾧤�����ϫ", -1);
#else
			take_hit(DAMAGE_GENO, randint1(3), "the strain of casting Mass Genocide", -1);
#endif

		}

		move_cursor_relative(py, px);

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Handle */
		handle_stuff();

		/* Fresh */
		Term_fresh();

		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, msec);

		/* Note effect */
		result = TRUE;
	}

	if (result)
	{
		chg_virtue(V_VITALITY, -2);
		chg_virtue(V_CHANCE, -1);
	}

	return (result);
}



/*
 * Delete all nearby (non-unique) undead
 */
bool mass_genocide_undead(int power, int player_cast)
{
	int     i;
	bool    result = FALSE;
	int     msec = delay_factor * delay_factor * delay_factor;


	/* Prevent mass genocide in quest levels */
	if (p_ptr->inside_quest && !random_quest_number(dun_level))
	{
		return (FALSE);
	}

	/* Delete the (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type    *m_ptr = &m_list[i];
		monster_race    *r_ptr = &r_info[m_ptr->r_idx];
		bool angry = FALSE;
		char m_name[80];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		if (!(r_ptr->flags3 & RF3_UNDEAD)) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

		if (is_pet(m_ptr) && !player_cast) continue;

		/* Hack -- Skip unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE)) angry = TRUE;

		/* Hack -- Skip Quest Monsters */
		else if (r_ptr->flags1 & RF1_QUESTOR) angry = TRUE;

		else if (r_ptr->flags7 & RF7_UNIQUE2) angry = TRUE;

		else if (i == p_ptr->riding) angry = TRUE;

		else if (player_cast && (r_ptr->level > randint0(power))) angry = TRUE;

		else if (player_cast && (m_ptr->mflag2 & MFLAG_NOGENO)) angry = TRUE;

		/* Delete the monster */
		else delete_monster_idx(i);

		if (angry && player_cast)
		{
			monster_desc(m_name, m_ptr, 0);
			if (m_ptr->ml && !p_ptr->blind)
			{
#ifdef JP
msg_format("%^s�ˤϸ��̤��ʤ��ä���", m_name);
#else
				msg_format("%^s is unaffected.", m_name);
#endif
			}
			if (m_ptr->csleep)
			{
				m_ptr->csleep = 0;
				if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);
				if (m_ptr->ml && !p_ptr->blind)
				{
#ifdef JP
msg_format("%^s���ܤ�Фޤ�����", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
				}
			}
			if (is_friendly(m_ptr) && !is_pet(m_ptr))
			{
				if (m_ptr->ml && !p_ptr->blind)
				{
#ifdef JP
					msg_format("%s���ܤä���", m_name);
#else
					msg_format("%^s gets angry!", m_name);
#endif
				}
				set_hostile(m_ptr);
			}
			if (one_in_(13)) m_ptr->mflag2 |= MFLAG_NOGENO;
		}

		if (player_cast)
		{
			/* Hack -- visual feedback */
#ifdef JP
take_hit(DAMAGE_GENO, randint1(3), "����ǥåɾ��Ǥμ�ʸ�򾧤�����ϫ", -1);
#else
			take_hit(DAMAGE_GENO, randint1(3), "the strain of casting Mass Genocide", -1);
#endif

		}

		move_cursor_relative(py, px);

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);

		/* Handle */
		handle_stuff();

		/* Fresh */
		Term_fresh();

		/* Delay */
		Term_xtra(TERM_XTRA_DELAY, msec);

		/* Note effect */
		result = TRUE;
	}

	if (result)
	{
		chg_virtue(V_UNLIFE, -2);
		chg_virtue(V_CHANCE, -1);
	}

	return (result);
}



/*
 * Probe nearby monsters
 */
bool probing(void)
{
	int     i, speed;
	int cu, cv;
	bool    probe = FALSE;
	char buf[256];
	cptr align;

	cu = Term->scr->cu;
	cv = Term->scr->cv;
	Term->scr->cu = 0;
	Term->scr->cv = 1;

	/* Probe all (nearby) monsters */
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Require line of sight */
		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) continue;

		/* Probe visible monsters */
		if (m_ptr->ml)
		{
			char m_name[80];

			/* Start the message */
#ifdef JP
			if (!probe) {msg_print("Ĵ����...");msg_print(NULL);}
#else
			if (!probe) {msg_print("Probing...");msg_print(NULL);}
#endif

			if (m_ptr->ap_r_idx != m_ptr->r_idx)
			{
				if (m_ptr->mflag2 & MFLAG_KAGE)
					m_ptr->mflag2 &= ~(MFLAG_KAGE);

				m_ptr->ap_r_idx = m_ptr->r_idx;
				lite_spot(m_ptr->fy, m_ptr->fx);
			}
			/* Get "the monster" or "something" */
			monster_desc(m_name, m_ptr, 0x204);

			speed = m_ptr->mspeed - 110;
			if(m_ptr->fast) speed += 10;
			if(m_ptr->slow) speed -= 10;

			/* Get the monster's alignment */
#ifdef JP
			if ((r_ptr->flags3 & RF3_EVIL) && (r_ptr->flags3 & RF3_GOOD)) align = "����";
			else if (r_ptr->flags3 & RF3_EVIL) align = "�ٰ�";
			else if (r_ptr->flags3 & RF3_GOOD) align = "����";
			else if ((m_ptr->sub_align & SUB_ALIGN_EVIL) && (m_ptr->sub_align & SUB_ALIGN_GOOD)) align = "��Ω(����)";
			else if (m_ptr->sub_align & SUB_ALIGN_EVIL) align = "��Ω(�ٰ�)";
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD) align = "��Ω(����)";
			else align = "��Ω";
#else
			if ((r_ptr->flags3 & RF3_EVIL) && (r_ptr->flags3 & RF3_GOOD)) align = "good&evil";
			else if (r_ptr->flags3 & RF3_EVIL) align = "evil";
			else if (r_ptr->flags3 & RF3_GOOD) align = "good";
			else if ((m_ptr->sub_align & SUB_ALIGN_EVIL) && (m_ptr->sub_align & SUB_ALIGN_GOOD)) align = "neutral(good&evil)";
			else if (m_ptr->sub_align & SUB_ALIGN_EVIL) align = "neutral(evil)";
			else if (m_ptr->sub_align & SUB_ALIGN_GOOD) align = "neutral(good)";
			else align = "neutral";
#endif

			/* Describe the monster */
#ifdef JP
sprintf(buf,"%s ... °��:%s HP:%d/%d AC:%d ®��:%s%d �и�:", m_name, align, m_ptr->hp, m_ptr->maxhp, r_ptr->ac, (speed > 0) ? "+" : "", speed);
#else
sprintf(buf, "%s ... align:%s HP:%d/%d AC:%d speed:%s%d exp:", m_name, align, m_ptr->hp, m_ptr->maxhp, r_ptr->ac, (speed > 0) ? "+" : "", speed);
#endif
			if (r_ptr->next_r_idx)
			{
				strcat(buf, format("%d/%d ", m_ptr->exp, r_ptr->next_exp));
			}
			else
			{
				strcat(buf, "xxx ");
			}

#ifdef JP
			if (m_ptr->csleep) strcat(buf,"��̲ ");
			if (m_ptr->stunned) strcat(buf,"ۯ۰ ");
			if (m_ptr->monfear) strcat(buf,"���� ");
			if (m_ptr->confused) strcat(buf,"���� ");
			if (m_ptr->invulner) strcat(buf,"̵Ũ ");
#else
			if (m_ptr->csleep) strcat(buf,"sleeping ");
			if (m_ptr->stunned) strcat(buf,"stunned ");
			if (m_ptr->monfear) strcat(buf,"scared ");
			if (m_ptr->confused) strcat(buf,"confused ");
			if (m_ptr->invulner) strcat(buf,"invulnerable ");
#endif
			buf[strlen(buf)-1] = '\0';
			prt(buf,0,0);

			/* HACK : Add the line to message buffer */
			message_add(buf);
			p_ptr->window |= (PW_MESSAGE);
			window_stuff();

			/* Learn all of the non-spell, non-treasure flags */
			lore_do_probe(i);

			if (m_ptr->ml) move_cursor_relative(m_ptr->fy, m_ptr->fx);
			inkey();

			Term_erase(0, 0, 255);

			/* Probe worked */
			probe = TRUE;
		}
	}

	Term->scr->cu = cu;
	Term->scr->cv = cv;
	Term_fresh();

	/* Done */
	if (probe)
	{
		chg_virtue(V_KNOWLEDGE, 1);

#ifdef JP
msg_print("����������Ǥ���");
#else
		msg_print("That's all.");
#endif

	}

	/* Result */
	return (probe);
}



/*
 * The spell of destruction
 *
 * This spell "deletes" monsters (instead of "killing" them).
 *
 * Later we may use one function for both "destruction" and
 * "earthquake" by using the "full" to select "destruction".
 */
bool destroy_area(int y1, int x1, int r, int full)
{
	int       y, x, k, t;
	cave_type *c_ptr;
	bool      flag = FALSE;


	/* Prevent destruction of quest levels and town */
	if ((p_ptr->inside_quest && (p_ptr->inside_quest < MIN_RANDOM_QUEST)) || !dun_level)
	{
		return (FALSE);
	}

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			monster_type *m_ptr;
			monster_race *r_ptr;

			/* Skip illegal grids */
			if (!in_bounds(y, x)) continue;

			/* Extract the distance */
			k = distance(y1, x1, y, x);

			/* Stay in the circle of death */
			if (k > r) continue;

			/* Access the grid */
			c_ptr = &cave[y][x];
			m_ptr = &m_list[c_ptr->m_idx];
			r_ptr = &r_info[m_ptr->r_idx];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY | CAVE_UNSAFE | CAVE_OBJECT);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_MARK | CAVE_GLOW);

			/* Hack -- Notice player affect */
			if ((x == px) && (y == py))
			{
				/* Hurt the player later */
				flag = TRUE;

				/* Do not hurt this grid */
				continue;
			}

			/* Hack -- Skip the epicenter */
			if ((y == y1) && (x == x1)) continue;

			if ((r_ptr->flags1 & RF1_QUESTOR))
			{
				/* Heal the monster */
				m_list[c_ptr->m_idx].hp = m_list[c_ptr->m_idx].maxhp;

				/* Try to teleport away quest monsters */
				if (!teleport_away(c_ptr->m_idx, (r * 2) + 1, TRUE)) continue;
			}
			else
			{
				if (c_ptr->m_idx)
				{
					if (record_named_pet && is_pet(&m_list[c_ptr->m_idx]) && m_list[c_ptr->m_idx].nickname)
					{
						char m_name[80];

						monster_desc(m_name, &m_list[c_ptr->m_idx], 0x08);
						do_cmd_write_nikki(NIKKI_NAMED_PET, 6, m_name);
					}
				}
				/* Delete the monster (if any) */
				delete_monster(y, x);
			}

			if (preserve_mode)
			{
				s16b this_o_idx, next_o_idx = 0;

				/* Scan all objects in the grid */
				for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Hack -- Preserve unknown artifacts */
					if (artifact_p(o_ptr) && !object_known_p(o_ptr))
					{
						/* Mega-Hack -- Preserve the artifact */
						a_info[o_ptr->name1].cur_num = 0;
					}
				}
			}
			delete_object(y, x);

			/* Destroy "valid" grids */
			if (!cave_perma_bold(y, x))
			{
				/* Wall (or floor) type */
				t = randint0(200);

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					cave_set_feat(y, x, FEAT_WALL_EXTRA);
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(y, x, FEAT_QUARTZ);
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					cave_set_feat(y, x, FEAT_MAGMA);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(y, x, floor_type[randint0(100)]);
				}
			}
		}
	}


	/* Hack -- Affect player */
	if (flag)
	{
		/* Message */
#ifdef JP
msg_print("ǳ����褦��������ȯ��������");
#else
		msg_print("There is a searing blast of light!");
#endif


		/* Blind the player */
		if (!p_ptr->resist_blind && !p_ptr->resist_lite)
		{
			/* Become blind */
			(void)set_blind(p_ptr->blind + 10 + randint1(10));
		}
	}

	forget_flow();

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Success */
	return (TRUE);
}


/*
 * Induce an "earthquake" of the given radius at the given location.
 *
 * This will turn some walls into floors and some floors into walls.
 *
 * The player will take damage and "jump" into a safe grid if possible,
 * otherwise, he will "tunnel" through the rubble instantaneously.
 *
 * Monsters will take damage, and "jump" into a safe grid if possible,
 * otherwise they will be "buried" in the rubble, disappearing from
 * the level in the same way that they do when genocided.
 *
 * Note that thus the player and monsters (except eaters of walls and
 * passers through walls) will never occupy the same grid as a wall.
 * Note that as of now (2.7.8) no monster may occupy a "wall" grid, even
 * for a single turn, unless that monster can pass_walls or kill_walls.
 * This has allowed massive simplification of the "monster" code.
 */
bool earthquake(int cy, int cx, int r)
{
	int             i, t, y, x, yy, xx, dy, dx, oy, ox;
	int             damage = 0;
	int             sn = 0, sy = 0, sx = 0;
	bool            hurt = FALSE;
	cave_type       *c_ptr;
	bool            map[32][32];


	/* Prevent destruction of quest levels and town */
	if ((p_ptr->inside_quest && (p_ptr->inside_quest < MIN_RANDOM_QUEST)) || !dun_level)
	{
		return (FALSE);
	}

	/* Paranoia -- Enforce maximum range */
	if (r > 12) r = 12;

	/* Clear the "maximal blast" area */
	for (y = 0; y < 32; y++)
	{
		for (x = 0; x < 32; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			/* Lose room and vault */
			c_ptr->info &= ~(CAVE_ROOM | CAVE_ICKY | CAVE_UNSAFE);

			/* Lose light and knowledge */
			c_ptr->info &= ~(CAVE_GLOW | CAVE_MARK);

			/* Skip the epicenter */
			if (!dx && !dy) continue;

			/* Skip most grids */
			if (randint0(100) < 85) continue;

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

			/* Hack -- Take note of player damage */
			if ((yy == py) && (xx == px)) hurt = TRUE;
		}
	}

	/* First, affect the player (if necessary) */
	if (hurt && !prace_is_(RACE_SPECTRE) && !p_ptr->wraith_form && !p_ptr->kabenuke)
	{
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			/* Access the location */
			y = py + ddy_ddd[i];
			x = px + ddx_ddd[i];

			/* Skip non-empty grids */
			if (!cave_empty_bold(y, x)) continue;

			/* Important -- Skip "quake" grids */
			if (map[16+y-cy][16+x-cx]) continue;

			if (cave[y][x].m_idx) continue;

			/* Count "safe" grids */
			sn++;

			/* Randomize choice */
			if (randint0(sn) > 0) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}

		/* Random message */
		switch (randint1(3))
		{
			case 1:
			{
#ifdef JP
msg_print("���󥸥����ɤ����줿��");
#else
				msg_print("The cave ceiling collapses!");
#endif

				break;
			}
			case 2:
			{
#ifdef JP
msg_print("���󥸥��ξ����Լ����ˤͤ��ʤ��ä���");
#else
				msg_print("The cave floor twists in an unnatural way!");
#endif

				break;
			}
			default:
			{
#ifdef JP
msg_print("���󥸥���ɤ줿�����줿�䤬Ƭ�˹ߤäƤ�����");
#else
				msg_print("The cave quakes!  You are pummeled with debris!");
#endif

				break;
			}
		}

		/* Hurt the player a lot */
		if (!sn)
		{
			/* Message and damage */
#ifdef JP
msg_print("���ʤ��ϤҤɤ��������ä���");
#else
			msg_print("You are severely crushed!");
#endif

			damage = 200;
		}

		/* Destroy the grid, and push the player to safety */
		else
		{
			/* Calculate results */
			switch (randint1(3))
			{
				case 1:
				{
#ifdef JP
msg_print("�ߤ�����򤦤ޤ��򤱤���");
#else
					msg_print("You nimbly dodge the blast!");
#endif

					damage = 0;
					break;
				}
				case 2:
				{
#ifdef JP
msg_print("���Ф����ʤ���ľ�⤷��!");
#else
					msg_print("You are bashed by rubble!");
#endif

					damage = damroll(10, 4);
					(void)set_stun(p_ptr->stun + randint1(50));
					break;
				}
				case 3:
				{
#ifdef JP
msg_print("���ʤ��Ͼ����ɤȤδ֤˶��ޤ�Ƥ��ޤä���");
#else
					msg_print("You are crushed between the floor and ceiling!");
#endif

					damage = damroll(10, 4);
					(void)set_stun(p_ptr->stun + randint1(50));
					break;
				}
			}

			/* Save the old location */
			oy = py;
			ox = px;

			/* Move the player to the safe location */
			py = sy;
			px = sx;

			if (p_ptr->riding)
			{
				int tmp;
				tmp = cave[py][px].m_idx;
				cave[py][px].m_idx = cave[oy][ox].m_idx;
				cave[oy][ox].m_idx = tmp;
				m_list[p_ptr->riding].fy = py;
				m_list[p_ptr->riding].fx = px;
				update_mon(cave[py][px].m_idx, TRUE);
			}

			/* Redraw the old spot */
			lite_spot(oy, ox);

			/* Redraw the new spot */
			lite_spot(py, px);

			/* Check for new panel */
			verify_panel();
		}

		/* Important -- no wall on player */
		map[16+py-cy][16+px-cx] = FALSE;

		/* Take some damage */
#ifdef JP
if (damage) take_hit(DAMAGE_ATTACK, damage, "�Ͽ�", -1);
#else
		if (damage) take_hit(DAMAGE_ATTACK, damage, "an earthquake", -1);
#endif

	}


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Access the grid */
			c_ptr = &cave[yy][xx];

			if (c_ptr->m_idx == p_ptr->riding) continue;

			/* Process monsters */
			if (c_ptr->m_idx)
			{
				monster_type *m_ptr = &m_list[c_ptr->m_idx];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/* Quest monsters */
				if (r_ptr->flags1 & RF1_QUESTOR)
				{
					/* No wall on quest monsters */
					map[16+yy-cy][16+xx-cx] = FALSE;

					continue;
				}

				/* Most monsters cannot co-exist with rock */
				if (!(r_ptr->flags2 & (RF2_KILL_WALL)) &&
				    !(r_ptr->flags2 & (RF2_PASS_WALL)))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & (RF1_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Access the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (is_glyph_grid(&cave[y][x])) continue;
							if (is_explosive_rune_grid(&cave[y][x])) continue;

							/* ... nor on the Pattern */
							if ((cave[y][x].feat <= FEAT_PATTERN_XTRA2) &&
							    (cave[y][x].feat >= FEAT_PATTERN_START))
								continue;

							/* Important -- Skip "quake" grids */
							if (map[16+y-cy][16+x-cx]) continue;

							if (cave[y][x].m_idx) continue;
							if ((y == py) && (x == px)) continue;

							/* Count "safe" grids */
							sn++;

							/* Randomize choice */
							if (randint0(sn) > 0) continue;

							/* Save the safe grid */
							sy = y; sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, m_ptr, 0);

					/* Scream in pain */
#ifdef JP
msg_format("%^s�϶��ˤǵ㤭��ᤤ����", m_name);
#else
					msg_format("%^s wails out in pain!", m_name);
#endif


					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->hp + 1));

					/* Monster is certainly awake */
					m_ptr->csleep = 0;

					/* Apply damage directly */
					m_ptr->hp -= damage;

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
#ifdef JP
msg_format("%^s�ϴ��Ф�����Ƥ��ޤä���", m_name);
#else
						msg_format("%^s is embedded in the rock!", m_name);
#endif

						if (c_ptr->m_idx)
						{
							if (record_named_pet && is_pet(&m_list[c_ptr->m_idx]) && m_list[c_ptr->m_idx].nickname)
							{
								char m2_name[80];

								monster_desc(m2_name, m_ptr, 0x08);
								do_cmd_write_nikki(NIKKI_NAMED_PET, 7, m2_name);
							}
						}

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						int m_idx = cave[yy][xx].m_idx;

						/* Update the new location */
						cave[sy][sx].m_idx = m_idx;

						/* Update the old location */
						cave[yy][xx].m_idx = 0;

						/* Move the monster */
						m_ptr->fy = sy;
						m_ptr->fx = sx;

						/* Update the monster (new location) */
						update_mon(m_idx, TRUE);

						/* Redraw the old grid */
						lite_spot(yy, xx);

						/* Redraw the new grid */
						lite_spot(sy, sx);
					}
				}
			}
		}
	}


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Access the cave grid */
			c_ptr = &cave[yy][xx];

			/* Paranoia -- never affect player */
/*			if ((yy == py) && (xx == px)) continue; */

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				bool floor = cave_floor_bold(yy, xx);

				/* Delete objects */
				delete_object(yy, xx);

				/* Clear mirror, runes flag */
				c_ptr->info &= ~CAVE_OBJECT;

				/* Wall (or floor) type */
				t = (floor ? randint0(100) : 200);

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					cave_set_feat(yy, xx, FEAT_WALL_EXTRA);
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					cave_set_feat(yy, xx, FEAT_QUARTZ);
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					cave_set_feat(yy, xx, FEAT_MAGMA);
				}

				/* Floor */
				else
				{
					/* Create floor */
					cave_set_feat(yy, xx, floor_type[randint0(100)]);
				}
			}
		}
	}


	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_DISTANCE);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_UHEALTH);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Success */
	return (TRUE);
}


void discharge_minion(void)
{
	int i;
	bool okay = TRUE;

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if (!m_ptr->r_idx || !is_pet(m_ptr)) continue;
		if (m_ptr->nickname) okay = FALSE;
	}
	if (!okay || p_ptr->riding)
	{
#ifdef JP
		if (!get_check("���������ڥåȤ����ˤ��ޤ�����"))
#else
		if (!get_check("You will blast all pets. Are you sure? "))
#endif
			return;
	}
	for (i = 1; i < m_max; i++)
	{
		int dam;
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr;

		if (!m_ptr->r_idx || !is_pet(m_ptr)) continue;
		r_ptr = &r_info[m_ptr->r_idx];

		/* Uniques resist discharging */
		if (r_ptr->flags1 & RF1_UNIQUE)
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0x00);
#ifdef JP
			msg_format("%s�����ˤ����Τ�����ꡢ����˼�ʬ�������ؤȵ��ä���", m_name);
#else
			msg_format("%^s resists to be blasted, and run away.", m_name);
#endif
			delete_monster_idx(i);
			continue;
		}
		dam = m_ptr->hp / 2;
		if (dam > 100) dam = (dam-100)/2 + 100;
		if (dam > 400) dam = (dam-400)/2 + 400;
		if (dam > 800) dam = 800;
		project(i, 2+(r_ptr->level/20), m_ptr->fy,
			m_ptr->fx, dam, GF_PLASMA, 
			PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL, -1);
		delete_monster_idx(i);
	}
}


/*
 * This routine clears the entire "temp" set.
 *
 * This routine will Perma-Lite all "temp" grids.
 *
 * This routine is used (only) by "lite_room()"
 *
 * Dark grids are illuminated.
 *
 * Also, process all affected monsters.
 *
 * SMART monsters always wake up when illuminated
 * NORMAL monsters wake up 1/4 the time when illuminated
 * STUPID monsters wake up 1/10 the time when illuminated
 */
static void cave_temp_room_lite(void)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		cave_type *c_ptr = &cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Update only non-CAVE_GLOW grids */
		/* if (c_ptr->info & (CAVE_GLOW)) continue; */

		/* Perma-Lite */
		c_ptr->info |= (CAVE_GLOW);

		/* Process affected monsters */
		if (c_ptr->m_idx)
		{
			int chance = 25;

			monster_type    *m_ptr = &m_list[c_ptr->m_idx];

			monster_race    *r_ptr = &r_info[m_ptr->r_idx];

			/* Update the monster */
			update_mon(c_ptr->m_idx, FALSE);

			/* Stupid monsters rarely wake up */
			if (r_ptr->flags2 & (RF2_STUPID)) chance = 10;

			/* Smart monsters always wake up */
			if (r_ptr->flags2 & (RF2_SMART)) chance = 100;

			/* Sometimes monsters wake up */
			if (m_ptr->csleep && (randint0(100) < chance))
			{
				/* Wake up! */
				m_ptr->csleep = 0;

				if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);

				/* Notice the "waking up" */
				if (m_ptr->ml)
				{
					char m_name[80];

					/* Acquire the monster name */
					monster_desc(m_name, m_ptr, 0);

					/* Dump a message */
#ifdef JP
msg_format("%^s���ܤ�Фޤ�����", m_name);
#else
					msg_format("%^s wakes up.", m_name);
#endif
					/* Redraw the health bar */
					if (p_ptr->health_who == c_ptr->m_idx)
						p_ptr->redraw |= (PR_HEALTH);

				}
			}
		}

		/* Note */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}

	/* None left */
	temp_n = 0;
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will "darken" all "temp" grids.
 *
 * In addition, some of these grids will be "unmarked".
 *
 * This routine is used (only) by "unlite_room()"
 *
 * Also, process all affected monsters
 */
static void cave_temp_room_unlite(void)
{
	int i;

	/* Clear them all */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		cave_type *c_ptr = &cave[y][x];

		/* No longer in the array */
		c_ptr->info &= ~(CAVE_TEMP);

		/* Darken the grid */
		if (!is_mirror_grid(c_ptr)) c_ptr->info &= ~(CAVE_GLOW);

		/* Hack -- Forget "boring" grids */
		if ((c_ptr->feat <= FEAT_INVIS) || (c_ptr->feat == FEAT_DIRT) || (c_ptr->feat == FEAT_GRASS))
		{
			/* Forget the grid */
			if (!view_torch_grids) c_ptr->info &= ~(CAVE_MARK);

			/* Notice */
			note_spot(y, x);
		}

		/* Process affected monsters */
		if (c_ptr->m_idx)
		{
			/* Update the monster */
			update_mon(c_ptr->m_idx, FALSE);
		}

		/* Redraw */
		lite_spot(y, x);
	}

	/* None left */
	temp_n = 0;
}



/*
 * Determine how much contiguous open space this grid is next to
 */
static int next_to_open(int cy, int cx)
{
	int i;

	int y, x;

	int len = 0;
	int blen = 0;

	for (i = 0; i < 16; i++)
	{
		y = cy + ddy_cdd[i % 8];
		x = cx + ddx_cdd[i % 8];

		/* Found a wall, break the length */
		if (!cave_floor_bold(y, x))
		{
			/* Track best length */
			if (len > blen)
			{
				blen = len;
			}

			len = 0;
		}
		else
		{
			len++;
		}
	}

	return (MAX(len, blen));
}


static int next_to_walls_adj(int cy, int cx)
{
	int i;

	int y, x;

	int c = 0;

	for (i = 0; i < 8; i++)
	{
		y = cy + ddy_ddd[i];
		x = cx + ddx_ddd[i];

		if (!cave_floor_bold(y, x)) c++;
	}

	return c;
}


/*
 * Aux function -- see below
 */
static void cave_temp_room_aux(int y, int x, bool only_room)
{
	cave_type *c_ptr;

	/* Get the grid */
	c_ptr = &cave[y][x];

	/* Avoid infinite recursion */
	if (c_ptr->info & (CAVE_TEMP)) return;

	/* Do not "leave" the current room */
	if (!(c_ptr->info & (CAVE_ROOM)))
	{
		if (only_room) return;

		/* Verify */
		if (!in_bounds2(y, x)) return;

		/* Do not exceed the maximum spell range */
		if (distance(py, px, y, x) > MAX_RANGE) return;

		/* Verify this grid */
		/*
		 * The reason why it is ==6 instead of >5 is that 8 is impossible
		 * due to the check for cave_bold above.
		 * 7 lights dead-end corridors (you need to do this for the
		 * checkboard interesting rooms, so that the boundary is lit
		 * properly.
		 * This leaves only a check for 6 bounding walls!
		 */
		if (in_bounds(y, x) && cave_floor_bold(y, x) &&
		    (next_to_walls_adj(y, x) == 6) && (next_to_open(y, x) <= 1)) return;
	}

	/* Paranoia -- verify space */
	if (temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	c_ptr->info |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}

/*
 * Aux function -- see below
 */
static void cave_temp_lite_room_aux(int y, int x)
{
	cave_temp_room_aux(y, x, FALSE);
}

/*
 * Aux function -- see below
 */
static void cave_temp_unlite_room_aux(int y, int x)
{
	cave_temp_room_aux(y, x, TRUE);
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(int y1, int x1)
{
	int i, x, y;

	/* Add the initial grid */
	cave_temp_lite_room_aux(y1, x1);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get lit, but stop light */
		if (!cave_floor_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_lite_room_aux(y + 1, x);
		cave_temp_lite_room_aux(y - 1, x);
		cave_temp_lite_room_aux(y, x + 1);
		cave_temp_lite_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_lite_room_aux(y + 1, x + 1);
		cave_temp_lite_room_aux(y - 1, x - 1);
		cave_temp_lite_room_aux(y - 1, x + 1);
		cave_temp_lite_room_aux(y + 1, x - 1);
	}

	/* Now, lite them all up at once */
	cave_temp_room_lite();
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(int y1, int x1)
{
	int i, x, y;

	/* Add the initial grid */
	cave_temp_unlite_room_aux(y1, x1);

	/* Spread, breadth first */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get dark, but stop darkness */
		if (!cave_floor_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_unlite_room_aux(y + 1, x);
		cave_temp_unlite_room_aux(y - 1, x);
		cave_temp_unlite_room_aux(y, x + 1);
		cave_temp_unlite_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_unlite_room_aux(y + 1, x + 1);
		cave_temp_unlite_room_aux(y - 1, x - 1);
		cave_temp_unlite_room_aux(y - 1, x + 1);
		cave_temp_unlite_room_aux(y + 1, x - 1);
	}

	/* Now, darken them all at once */
	cave_temp_room_unlite();
}



/*
 * Hack -- call light around the player
 * Affect all monsters in the projection radius
 */
bool lite_area(int dam, int rad)
{
	int flg = PROJECT_GRID | PROJECT_KILL;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS)
	{
#ifdef JP
		msg_print("���󥸥�󤬸���ۼ�������");
#else
		msg_print("The darkness of this dungeon absorb your light.");
#endif
		return FALSE;
	}

	/* Hack -- Message */
	if (!p_ptr->blind)
	{
#ifdef JP
msg_print("�򤤸����դ��ʤ�ä���");
#else
		msg_print("You are surrounded by a white light.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(0, rad, py, px, dam, GF_LITE_WEAK, flg, -1);

	/* Lite up the room */
	lite_room(py, px);

	if (p_ptr->special_defense & NINJA_S_STEALTH)
	{
		set_superstealth(FALSE);
	}

	/* Assume seen */
	return (TRUE);
}


/*
 * Hack -- call darkness around the player
 * Affect all monsters in the projection radius
 */
bool unlite_area(int dam, int rad)
{
	int flg = PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!p_ptr->blind)
	{
#ifdef JP
msg_print("�ŰǤ��դ��ʤ�ä���");
#else
		msg_print("Darkness surrounds you.");
#endif

	}

	/* Hook into the "project()" function */
	(void)project(0, rad, py, px, dam, GF_DARK_WEAK, flg, -1);

	/* Lite up the room */
	unlite_room(py, px);

	/* Assume seen */
	return (TRUE);
}



/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_ball(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	if (typ == GF_CONTROL_LIVING) flg|= PROJECT_HIDE;
	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_rocket(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a ball spell
 * Stop if we hit a monster, act as a "ball"
 * Allow "target" mode to pass over monsters
 * Affect grids, objects, and monsters
 */
bool fire_ball_hide(int typ, int dir, int dam, int rad)
{
	int tx, ty;

	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_HIDE;

	/* Use the given direction */
	tx = px + 99 * ddx[dir];
	ty = py + 99 * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		flg &= ~(PROJECT_STOP);
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target".  Hurt items on floor. */
	return (project(0, rad, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a meteor spell, defined as a ball spell cast by an arbitary monster, 
 * player, or outside source, that starts out at an arbitrary location, and 
 * leaving no trail from the "caster" to the target.  This function is 
 * especially useful for bombardments and similar. -LM-
 *
 * Option to hurt the player.
 */
bool fire_meteor(int who, int typ, int y, int x, int dam, int rad)
{
	int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;

	/* Analyze the "target" and the caster. */
	return (project(who, rad, y, x, dam, typ, flg, -1));
}


bool fire_blast(int typ, int dir, int dd, int ds, int num, int dev)
{
	int ly, lx, ld;
	int ty, tx, y, x;
	int i;

	int flg = PROJECT_FAST | PROJECT_THRU | PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID;

	/* Assume okay */
	bool result = TRUE;

	/* Use the given direction */
	if (dir != 5)
	{
		ly = ty = py + 20 * ddy[dir];
		lx = tx = px + 20 * ddx[dir];
	}

	/* Use an actual "target" */
	else if (dir == 5)
	{
		tx = target_col;
		ty = target_row;

		lx = 20 * (tx - px) + px;
		ly = 20 * (ty - py) + py;
	}

	ld = distance(py, px, ly, lx);

	/* Blast */
	for (i = 0; i < num; i++)
	{
		while (1)
		{
			/* Get targets for some bolts */
			y = rand_spread(ly, ld * dev / 20);
			x = rand_spread(lx, ld * dev / 20);

			if (distance(ly, lx, y, x) <= ld * dev / 20) break;
		}

		/* Analyze the "dir" and the "target". */
		if (!project(0, 0, y, x, damroll(dd, ds), typ, flg, -1))
		{
			result = FALSE;
		}
	}

	return (result);
}


/*
 * Switch position with a monster.
 */
bool teleport_swap(int dir)
{
	int tx, ty;
	cave_type * c_ptr;
	monster_type * m_ptr;
	monster_race * r_ptr;

	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}
	else
	{
		tx = px + ddx[dir];
		ty = py + ddy[dir];
	}
	c_ptr = &cave[ty][tx];

	if (p_ptr->anti_tele)
	{
#ifdef JP
msg_print("�Ի׵Ĥ��Ϥ��ƥ�ݡ��Ȥ��ɤ�����");
#else
		msg_print("A mysterious force prevents you from teleporting!");
#endif

		return FALSE;
	}

	if (!c_ptr->m_idx || (c_ptr->m_idx == p_ptr->riding))
	{
#ifdef JP
msg_print("����ȤϾ���򴹤Ǥ��ޤ���");
#else
		msg_print("You can't trade places with that!");
#endif


		/* Failure */
		return FALSE;
	}

	if ((c_ptr->info & CAVE_ICKY) || (distance(ty, tx, py, px) > p_ptr->lev * 3 / 2 + 10))
	{
#ifdef JP
msg_print("���Ԥ�����");
#else
		msg_print("Failed to swap.");
#endif


		/* Failure */
		return FALSE;
	}

	m_ptr = &m_list[c_ptr->m_idx];
	r_ptr = &r_info[m_ptr->r_idx];

	if (r_ptr->flags3 & RF3_RES_TELE)
	{
#ifdef JP
msg_print("�ƥ�ݡ��Ȥ���⤵�줿��");
#else
		msg_print("Your teleportation is blocked!");
#endif

		m_ptr->csleep = 0;
		if (r_ptr->flags7 & (RF7_HAS_LITE_1 | RF7_HAS_LITE_2)) p_ptr->update |= (PU_MON_LITE);

		/* Failure */
		return FALSE;
	}

	sound(SOUND_TELEPORT);

	cave[py][px].m_idx = c_ptr->m_idx;

	/* Update the old location */
	c_ptr->m_idx = p_ptr->riding;

	/* Move the monster */
	m_ptr->fy = py;
	m_ptr->fx = px;

	/* Move the player */
	px = tx;
	py = ty;

	if (p_ptr->riding)
	{
		m_list[p_ptr->riding].fy = ty;
		m_list[p_ptr->riding].fx = tx;

		/* Update the monster (new location) */
		update_mon(cave[ty][tx].m_idx, TRUE);
	}

	tx = m_ptr->fx;
	ty = m_ptr->fy;

	m_ptr->csleep = 0;

	/* Update the monster (new location) */
	update_mon(cave[ty][tx].m_idx, TRUE);

	/* Redraw the old grid */
	lite_spot(ty, tx);

	/* Redraw the new grid */
	lite_spot(py, px);

	/* Check for new panel (redraw map) */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_DISTANCE);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	/* Redraw the health bar */
	if (p_ptr->health_who == cave[ty][tx].m_idx)
		p_ptr->redraw |= (PR_HEALTH);

	/* Handle stuff XXX XXX XXX */
	handle_stuff();

	/* Success */
	return TRUE;
}


/*
 * Hack -- apply a "projection()" in a direction (or at the target)
 */
bool project_hook(int typ, int dir, int dam, int flg)
{
	int tx, ty;

	/* Pass through the target if needed */
	flg |= (PROJECT_THRU);

	/* Use the given direction */
	tx = px + ddx[dir];
	ty = py + ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	/* Analyze the "dir" and the "target", do NOT explode */
	return (project(0, 0, ty, tx, dam, typ, flg, -1));
}


/*
 * Cast a bolt spell
 * Stop if we hit a monster, as a "bolt"
 * Affect monsters (not grids or objects)
 */
bool fire_bolt(int typ, int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE | PROJECT_GRID;
	return (project_hook(typ, dir, dam, flg));
}


/*
 * Cast a beam spell
 * Pass through monsters, as a "beam"
 * Affect monsters (not grids or objects)
 */
bool fire_beam(int typ, int dir, int dam)
{
	int flg = PROJECT_BEAM | PROJECT_KILL | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(typ, dir, dam, flg));
}


/*
 * Cast a bolt spell, or rarely, a beam spell
 */
bool fire_bolt_or_beam(int prob, int typ, int dir, int dam)
{
	if (randint0(100) < prob)
	{
		return (fire_beam(typ, dir, dam));
	}
	else
	{
		return (fire_bolt(typ, dir, dam));
	}
}


/*
 * Some of the old functions
 */
bool lite_line(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL;
	return (project_hook(GF_LITE_WEAK, dir, damroll(6, 8), flg));
}


bool drain_life(int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_DRAIN, dir, dam, flg));
}


bool wall_to_mud(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(GF_KILL_WALL, dir, 20 + randint1(30), flg));
}


bool wizard_lock(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
	return (project_hook(GF_JAM_DOOR, dir, 20 + randint1(30), flg));
}


bool destroy_door(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(GF_KILL_DOOR, dir, 0, flg));
}


bool disarm_trap(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (project_hook(GF_KILL_TRAP, dir, 0, flg));
}


bool heal_monster(int dir, int dam)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_HEAL, dir, dam, flg));
}


bool speed_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SPEED, dir, p_ptr->lev, flg));
}


bool slow_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SLOW, dir, p_ptr->lev, flg));
}


bool sleep_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_SLEEP, dir, p_ptr->lev, flg));
}


bool stasis_monster(int dir)
{
	return (fire_ball_hide(GF_STASIS, dir, p_ptr->lev*2, 0));
}


bool stasis_evil(int dir)
{
	return (fire_ball_hide(GF_STASIS_EVIL, dir, p_ptr->lev*2, 0));
}


bool confuse_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_CONF, dir, plev, flg));
}


bool stun_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_STUN, dir, plev, flg));
}


bool poly_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	bool tester = (project_hook(GF_OLD_POLY, dir, p_ptr->lev, flg));
	if (tester)
		chg_virtue(V_CHANCE, 1);
	return(tester);
}


bool clone_monster(int dir)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_OLD_CLONE, dir, 0, flg));
}


bool fear_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_TURN_ALL, dir, plev, flg));
}


bool death_ray(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL | PROJECT_REFLECTABLE;
	return (project_hook(GF_DEATH_RAY, dir, plev * 200, flg));
}


bool teleport_monster(int dir)
{
	int flg = PROJECT_BEAM | PROJECT_KILL;
	return (project_hook(GF_AWAY_ALL, dir, MAX_SIGHT * 5, flg));
}

/*
 * Hooks -- affect adjacent grids (radius 1 ball attack)
 */
bool door_creation(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, py, px, 0, GF_MAKE_DOOR, flg, -1));
}


bool trap_creation(int y, int x)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, y, x, 0, GF_MAKE_TRAP, flg, -1));
}


bool tree_creation(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, py, px, 0, GF_MAKE_TREE, flg, -1));
}


bool glyph_creation(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM;
	return (project(0, 1, py, px, 0, GF_MAKE_GLYPH, flg, -1));
}


bool wall_stone(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;

	bool dummy = (project(0, 1, py, px, 0, GF_STONE_WALL, flg, -1));

	/* Update stuff */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

	return dummy;
}


bool destroy_doors_touch(void)
{
	int flg = PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(0, 1, py, px, 0, GF_KILL_DOOR, flg, -1));
}


bool sleep_monsters_touch(void)
{
	int flg = PROJECT_KILL | PROJECT_HIDE;
	return (project(0, 1, py, px, p_ptr->lev, GF_OLD_SLEEP, flg, -1));
}


bool animate_dead(int who, int y, int x)
{
	int flg = PROJECT_ITEM | PROJECT_HIDE;
	return (project(who, 5, y, x, 0, GF_ANIM_DEAD, flg, -1));
}


void call_chaos(void)
{
	int Chaos_type, dummy, dir;
	int plev = p_ptr->lev;
	bool line_chaos = FALSE;

	int hurt_types[31] =
	{
		GF_ELEC,      GF_POIS,    GF_ACID,    GF_COLD,
		GF_FIRE,      GF_MISSILE, GF_ARROW,   GF_PLASMA,
		GF_HOLY_FIRE, GF_WATER,   GF_LITE,    GF_DARK,
		GF_FORCE,     GF_INERTIA, GF_MANA,    GF_METEOR,
		GF_ICE,       GF_CHAOS,   GF_NETHER,  GF_DISENCHANT,
		GF_SHARDS,    GF_SOUND,   GF_NEXUS,   GF_CONFUSION,
		GF_TIME,      GF_GRAVITY, GF_ROCKET,  GF_NUKE,
		GF_HELL_FIRE, GF_DISINTEGRATE, GF_PSY_SPEAR
	};

	Chaos_type = hurt_types[randint0(31)];
	if (one_in_(4)) line_chaos = TRUE;

	if (one_in_(6))
	{
		for (dummy = 1; dummy < 10; dummy++)
		{
			if (dummy - 5)
			{
				if (line_chaos)
					fire_beam(Chaos_type, dummy, 150);
				else
					fire_ball(Chaos_type, dummy, 150, 2);
			}
		}
	}
	else if (one_in_(3))
	{
		fire_ball(Chaos_type, 0, 500, 8);
	}
	else
	{
		if (!get_aim_dir(&dir)) return;
		if (line_chaos)
			fire_beam(Chaos_type, dir, 250);
		else
			fire_ball(Chaos_type, dir, 250, 3 + (plev / 35));
	}
}


/*
 * Activate the evil Topi Ylinen curse
 * rr9: Stop the nasty things when a Cyberdemon is summoned
 * or the player gets paralyzed.
 */
bool activate_ty_curse(bool stop_ty, int *count)
{
	int     i = 0;

	int flg = (PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_JUMP);

	do
	{
		switch (randint1(34))
		{
		case 28: case 29:
			if (!(*count))
			{
#ifdef JP
msg_print("���̤��ɤ줿...");
#else
				msg_print("The ground trembles...");
#endif

				earthquake(py, px, 5 + randint0(10));
				if (!one_in_(6)) break;
			}
		case 30: case 31:
			if (!(*count))
			{
				int dam = damroll(10, 10);
#ifdef JP
msg_print("�������Ϥμ����ؤ��⤬��������");
#else
				msg_print("A portal opens to a plane of raw mana!");
#endif

				project(0, 8, py, px, dam, GF_MANA, flg, -1);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, dam, "�������Ϥβ���", -1);
#else
				take_hit(DAMAGE_NOESCAPE, dam, "released pure mana", -1);
#endif
				if (!one_in_(6)) break;
			}
		case 32: case 33:
			if (!(*count))
		{
#ifdef JP
msg_print("���Ϥζ��֤��Ĥ����");
#else
				msg_print("Space warps about you!");
#endif

				teleport_player(damroll(10, 10));
				if (randint0(13)) (*count) += activate_hi_summon(py, px, FALSE);
				if (!one_in_(6)) break;
			}
		case 34:
#ifdef JP
msg_print("���ͥ륮���Τ��ͤ�򴶤�����");
#else
			msg_print("You feel a surge of energy!");
#endif

			wall_breaker();
			if (!randint0(7))
			{
				project(0, 7, py, px, 50, GF_KILL_WALL, flg, -1);
#ifdef JP
				take_hit(DAMAGE_NOESCAPE, 50, "���ͥ륮���Τ��ͤ�", -1);
#else
				take_hit(DAMAGE_NOESCAPE, 50, "surge of energy", -1);
#endif
			}
			if (!one_in_(6)) break;
		case 1: case 2: case 3: case 16: case 17:
			aggravate_monsters(0);
			if (!one_in_(6)) break;
		case 4: case 5: case 6:
			(*count) += activate_hi_summon(py, px, FALSE);
			if (!one_in_(6)) break;
		case 7: case 8: case 9: case 18:
			(*count) += summon_specific(0, py, px, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			if (!one_in_(6)) break;
		case 10: case 11: case 12:
#ifdef JP
msg_print("��̿�Ϥ��Τ���ۤ����줿�������롪");
#else
			msg_print("You feel your life draining away...");
#endif

			lose_exp(p_ptr->exp / 16);
			if (!one_in_(6)) break;
		case 13: case 14: case 15: case 19: case 20:
			if (stop_ty || (p_ptr->free_act && (randint1(125) < p_ptr->skill_sav)) || (p_ptr->pclass == CLASS_BERSERKER))
			{
				/* Do nothing */ ;
			}
			else
			{
#ifdef JP
msg_print("Ħ���ˤʤä���ʬ����");
#else
				msg_print("You feel like a statue!");
#endif

				if (p_ptr->free_act)
					set_paralyzed(p_ptr->paralyzed + randint1(3));
				else
					set_paralyzed(p_ptr->paralyzed + randint1(13));
				stop_ty = TRUE;
			}
			if (!one_in_(6)) break;
		case 21: case 22: case 23:
			(void)do_dec_stat(randint0(6));
			if (!one_in_(6)) break;
		case 24:
#ifdef JP
msg_print("�ۤ������ï�������ǲ����Ƥ롩");
#else
			msg_print("Huh? Who am I? What am I doing here?");
#endif

			lose_all_info();
			if (!one_in_(6)) break;
		case 25:
			/*
			 * Only summon Cyberdemons deep in the dungeon.
			 */
			if ((dun_level > 65) && !stop_ty)
			{
				(*count) += summon_cyber(-1, py, px);
				stop_ty = TRUE;
				break;
			}
			if (!one_in_(6)) break;
		default:
			while (i < 6)
			{
				do
				{
					(void)do_dec_stat(i);
				}
				while (one_in_(2));

				i++;
			}
		}
	}
	while (one_in_(3) && !stop_ty);

	return stop_ty;
}


int activate_hi_summon(int y, int x, bool can_pet)
{
	int i;
	int count = 0;
	int summon_lev;
	u32b mode = PM_ALLOW_GROUP;
	bool pet = FALSE;

	if (can_pet)
	{
		if (one_in_(4))
		{
			mode |= PM_FORCE_FRIENDLY;
		}
		else
		{
			mode |= PM_FORCE_PET;
			pet = TRUE;
		}
	}

	if (!pet) mode |= PM_NO_PET;

	summon_lev = (pet ? p_ptr->lev * 2 / 3 + randint1(p_ptr->lev / 2) : dun_level);

	for (i = 0; i < (randint1(7) + (dun_level / 40)); i++)
	{
		switch (randint1(25) + (dun_level / 20))
		{
			case 1: case 2:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_ANT, mode);
				break;
			case 3: case 4:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_SPIDER, mode);
				break;
			case 5: case 6:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HOUND, mode);
				break;
			case 7: case 8:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HYDRA, mode);
				break;
			case 9: case 10:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_ANGEL, mode);
				break;
			case 11: case 12:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_UNDEAD, mode);
				break;
			case 13: case 14:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_DRAGON, mode);
				break;
			case 15: case 16:
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_DEMON, mode);
				break;
			case 17:
				if (can_pet) break;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_AMBERITES, (mode | PM_ALLOW_UNIQUE));
				break;
			case 18: case 19:
				if (can_pet) break;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_UNIQUE, (mode | PM_ALLOW_UNIQUE));
				break;
			case 20: case 21:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HI_UNDEAD, mode);
				break;
			case 22: case 23:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? -1 : 0), y, x, summon_lev, SUMMON_HI_DRAGON, mode);
				break;
			case 24:
				count += summon_specific((pet ? -1 : 0), y, x, 100, SUMMON_CYBER, mode);
				break;
			default:
				if (!can_pet) mode |= PM_ALLOW_UNIQUE;
				count += summon_specific((pet ? -1 : 0), y, x,pet ? summon_lev : (((summon_lev * 3) / 2) + 5), 0, mode);
		}
	}

	return count;
}


/* ToDo: check */
int summon_cyber(int who, int y, int x)
{
	int i;
	int max_cyber = (easy_band ? 1 : (dun_level / 50) + randint1(2));
	int count = 0;
	u32b mode = PM_ALLOW_GROUP;

	/* Summoned by a monster */
	if (who > 0)
	{
		monster_type *m_ptr = &m_list[who];
		if (is_pet(m_ptr)) mode |= PM_FORCE_PET;
	}

	if (max_cyber > 4) max_cyber = 4;

	for (i = 0; i < max_cyber; i++)
	{
		count += summon_specific(who, y, x, 100, SUMMON_CYBER, mode);
	}

	return count;
}


void wall_breaker(void)
{
	int i;
	int y, x;
	int attempts = 1000;

	if (randint1(80 + p_ptr->lev) < 70)
	{
		while(attempts--)
		{
			scatter(&y, &x, py, px, 4, 0);

			if (!cave_floor_bold(y, x)) continue;

			if ((y != py) || (x != px)) break;
		}

		project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL,
				  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
	}
	else if (randint1(100) > 30)
	{
		earthquake(py, px, 1);
	}
	else
	{
		int num = damroll(5, 3);

		for (i = 0; i < num; i++)
		{
			while(1)
			{
				scatter(&y, &x, py, px, 10, 0);

				if ((y != py) && (x != px)) break;
			}

			project(0, 0, y, x, 20 + randint1(30), GF_KILL_WALL,
					  (PROJECT_BEAM | PROJECT_THRU | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL), -1);
		}
	}
}


/*
 * Confuse monsters
 */
bool confuse_monsters(int dam)
{
	return (project_hack(GF_OLD_CONF, dam));
}


/*
 * Charm monsters
 */
bool charm_monsters(int dam)
{
	return (project_hack(GF_CHARM, dam));
}


/*
 * Charm animals
 */
bool charm_animals(int dam)
{
	return (project_hack(GF_CONTROL_ANIMAL, dam));
}


/*
 * Stun monsters
 */
bool stun_monsters(int dam)
{
	return (project_hack(GF_STUN, dam));
}


/*
 * Stasis monsters
 */
bool stasis_monsters(int dam)
{
	return (project_hack(GF_STASIS, dam));
}


/*
 * Mindblast monsters
 */
bool mindblast_monsters(int dam)
{
	return (project_hack(GF_PSI, dam));
}


/*
 * Banish all monsters
 */
bool banish_monsters(int dist)
{
	return (project_hack(GF_AWAY_ALL, dist));
}


/*
 * Turn evil
 */
bool turn_evil(int dam)
{
	return (project_hack(GF_TURN_EVIL, dam));
}


/*
 * Turn everyone
 */
bool turn_monsters(int dam)
{
	return (project_hack(GF_TURN_ALL, dam));
}


/*
 * Death-ray all monsters (note: OBSCENELY powerful)
 */
bool deathray_monsters(void)
{
	return (project_hack(GF_DEATH_RAY, p_ptr->lev * 200));
}


bool charm_monster(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CHARM, dir, plev, flg));
}


bool control_one_undead(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_UNDEAD, dir, plev, flg));
}


bool control_one_demon(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_DEMON, dir, plev, flg));
}


bool charm_animal(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_ANIMAL, dir, plev, flg));
}


bool charm_living(int dir, int plev)
{
	int flg = PROJECT_STOP | PROJECT_KILL;
	return (project_hook(GF_CONTROL_LIVING, dir, plev, flg));
}


void kawarimi(bool success)
{
	object_type forge;
	object_type *q_ptr = &forge;
	int y, x;

	if (p_ptr->confused || p_ptr->blind || p_ptr->paralyzed || p_ptr->image) return;
	if (randint0(200) < p_ptr->stun) return;

	if (!success && one_in_(3))
	{
#ifdef JP
		msg_print("���ԡ�ƨ�����ʤ��ä���");
#else
		msg_print("Failed! You couldn't run away.");
#endif
		p_ptr->special_defense &= ~(NINJA_KAWARIMI);
		p_ptr->redraw |= (PR_STATUS);
		return;
	}

	y = py;
	x = px;

	teleport_player(10+randint1(90));

	object_wipe(q_ptr);

	object_prep(q_ptr, lookup_kind(TV_STATUE, SV_WOODEN_STATUE));

	q_ptr->pval = MON_NINJA;

	/* Drop it in the dungeon */
	(void)drop_near(q_ptr, -1, y, x);

#ifdef JP
	if (success) msg_print("�����������������᤯�Ȥ�Ҥ뤬��������");
	else msg_print("���ԡ����������Ƥ��ޤä���");
#else
	if (success) msg_print("You have turned around just before the attack hit you.");
	else msg_print("Failed! You are hit by the attack.");
#endif

	p_ptr->special_defense &= ~(NINJA_KAWARIMI);
	p_ptr->redraw |= (PR_STATUS);
}


/*
 * "Rush Attack" routine for Samurai or Ninja
 * Return value is for checking "done"
 */
bool rush_attack(bool *mdeath)
{
	int dir;
	int tx, ty, nx, ny;
	int tm_idx = 0;
	u16b path_g[32];
	int path_n, i;
	bool tmp_mdeath = FALSE;

	if (mdeath) *mdeath = FALSE;

	project_length = 5;
	if (!get_aim_dir(&dir)) return FALSE;

	/* Use the given direction */
	tx = px + project_length * ddx[dir];
	ty = py + project_length * ddy[dir];

	/* Hack -- Use an actual "target" */
	if ((dir == 5) && target_okay())
	{
		tx = target_col;
		ty = target_row;
	}

	if (in_bounds(ty, tx)) tm_idx = cave[ty][tx].m_idx;

	path_n = project_path(path_g, project_length, py, px, ty, tx, PROJECT_STOP | PROJECT_KILL);
	project_length = 0;

	/* No need to move */
	if (!path_n) return TRUE;

	/* Use ty and tx as to-move point */
	ty = py;
	tx = px;

	/* Project along the path */
	for (i = 0; i < path_n; i++)
	{
		ny = GRID_Y(path_g[i]);
		nx = GRID_X(path_g[i]);

		if (!cave_empty_bold(ny, nx) || !player_can_enter(cave[ny][nx].feat))
		{
			if (cave[ny][nx].m_idx)
			{
				monster_type *m_ptr = &m_list[cave[ny][nx].m_idx];

				if (tm_idx != cave[ny][nx].m_idx)
				{
#ifdef JP
					msg_format("%s%s��Ω���դ����äƤ��롪", tm_idx ? "�̤�" : "",
						m_ptr->ml ? "��󥹥���" : "����");
#else
					msg_format("There is %s in the way!", m_ptr->ml ? (tm_idx ? "another monster" : "a monster") :
						"someone");
#endif
				}
				else
				{
					if ((py != ty) || (px != tx))
					{
						/* Hold the monster name */
						char m_name[80];

						/* Get the monster name (BEFORE polymorphing) */
						monster_desc(m_name, m_ptr, 0);
#ifdef JP
						msg_format("���᤯%s�β�������������", m_name);
#else
						msg_format("You quickly jump in and attack %s!", m_name);
#endif
					}
				}

				tmp_mdeath = py_attack(ny, nx, HISSATSU_NYUSIN);
			}
			else
			{
				if (tm_idx)
				{
#ifdef JP
					msg_print("���ԡ�");
#else
					msg_print("Failed!");
#endif
				}
				else
				{
#ifdef JP
					msg_print("�����ˤ����ȤǤ�����ʤ���");
#else
					msg_print("You can't move to that place.");
#endif
				}
			}
			break;
		}
		else
		{
			ty = ny;
			tx = nx;
		}
	}

	if ((py != ty) || (px != tx)) teleport_player_to(ty, tx, FALSE);

	if (mdeath) *mdeath = tmp_mdeath;
	return TRUE;
}
