/* File: mutation.c */

/* Purpose: Mutation effects (and racial powers) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


bool gain_random_mutation(int choose_mut)
{
	int     attempts_left = 20;
	cptr    muta_desc = "";
	bool    muta_chosen = FALSE;
	u32b    muta_which = 0;
	u32b    *muta_class = NULL;

	if (choose_mut) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : (p_ptr->pclass == CLASS_BERSERKER ? 74+randint1(119) : randint1(193)))
		{
		case 1: case 2: case 3: case 4:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SPIT_ACID;
#ifdef JP
muta_desc = "�����Ǥ�ǽ�Ϥ�������";
#else
			muta_desc = "You gain the ability to spit acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "�Ф��Ǥ�ǽ�Ϥ�������";
#else
			muta_desc = "You gain the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "��̲���ǽ�Ϥ�������";
#else
			muta_desc = "Your eyes look mesmerizing...";
#endif

			break;
		case 10: case 11:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "ʪ�Τ�ǰư�Ϥ�ư����ǽ�Ϥ�������";
#else
			muta_desc = "You gain the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "��ʬ�ΰջפǥƥ�ݡ��Ȥ���ǽ�Ϥ�������";
#else
			muta_desc = "You gain the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "���������ǽ�Ϥ�������";
#else
			muta_desc = "You gain the power of Mind Blast.";
#endif

			break;
		case 17: case 18:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RADIATION;
#ifdef JP
muta_desc = "���ʤ��϶�����������ȯ�����Ϥ᤿��";
#else
			muta_desc = "You start emitting hard radiation.";
#endif

			break;
		case 19: case 20:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "��̿�Ϥ�ۼ��Ǥ���褦�ˤʤä���";
#else
			muta_desc = "You become vampiric.";
#endif

			break;
		case 21: case 22: case 23:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SMELL_MET;
#ifdef JP
muta_desc = "��°���������̤�ʬ������褦�ˤʤä���";
#else
			muta_desc = "You smell a metallic odor.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SMELL_MON;
#ifdef JP
muta_desc = "��󥹥����ν������̤�ʬ������褦�ˤʤä���";
#else
			muta_desc = "You smell filthy monsters.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "���Υ�ƥ�ݡ��Ȥ�ǽ�Ϥ�������";
#else
			muta_desc = "You gain the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "�ɤ���̣�������˸����롣";
#else
			muta_desc = "The walls look delicious.";
#endif

			break;
		case 33: case 34:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "¾�ͤη��ǰ�ޥ����⤯�褦�ʵ�ʬ�����롣";
#else
			muta_desc = "You feel like walking a mile in someone else's shoes.";
#endif

			break;
		case 35: case 36: case 37:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SHRIEK;
#ifdef JP
muta_desc = "���ʤ����������������ʤä���";
#else
			muta_desc = "Your vocal cords get much tougher.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ILLUMINE;
#ifdef JP
muta_desc = "���ʤ��ϸ��굱�������������뤯����褦�ˤʤä���";
#else
			muta_desc = "You can light up rooms with your presence.";
#endif

			break;
		case 41: case 42:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DET_CURSE;
#ifdef JP
muta_desc = "�ٰ�����ˡ���ΤǤ���褦�ˤʤä���";
#else
			muta_desc = "You can feel evil magics.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "����Ǥ�����򴶤��롣";
#else
			muta_desc = "You feel a controlled rage.";
#endif

			break;
		case 46:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_POLYMORPH;
#ifdef JP
muta_desc = "�Τ��Ѱۤ��䤹���ʤä���";
#else
			muta_desc = "Your body seems mutable.";
#endif

			break;
		case 47: case 48:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "�֥ߥ������μ�פ�ǽ�Ϥ�������";/*�ȥ�����쥤�������ˤ���ޤ����͡� */
#else
			muta_desc = "You gain the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "�������Ӥ˿Ƥ��ߤ�Ф�����";
#else
			muta_desc = "You feel a sudden affinity for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "���ʤ��ϼ�ʬ���Ȥ���뵤�����롣";
#else
			muta_desc = "You feel like you can protect yourself.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "���󥸥����˲�����ǽ�Ϥ�������";
#else
			muta_desc = "You gain the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "��ˡ�Υ����ƥब��̣�����˸����롣";
#else
			muta_desc = "Your magic items look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_WEIGH_MAG;
#ifdef JP
muta_desc = "���ʤ��ϼ��Ϥˤ�����ˡ�����ɤ�����Ǥ��뵤�����롣";
#else
			muta_desc = "You feel you can better understand the magic around you.";
#endif

			break;
		case 59:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "��������ƤμԤ�Ƭ�ˤ򵯤������Ȥ��Ǥ��롣";
#else
			muta_desc = "You can give everything around you a headache.";
#endif

			break;
		case 60: case 61:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "������ť���ε�ʬ��ʬ����褦�ˤʤä���";
#else
			muta_desc = "You suddenly understand how thieves feel.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "����������ȯ����ǽ�Ϥ�������";
#else
			muta_desc = "You gain the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "���ʤ����ܤϰ�־Ƥ��դ�����";
#else
			muta_desc = "Your eyes burn for a moment.";
#endif

			break;
		case 68: case 69:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "���������ۡ��ॷ�å��ˤʤä���������ľ�ä���";
#else
			muta_desc = "You feel briefly homesick, but it passes.";
#endif

			break;
		case 70:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BANISH;
#ifdef JP
muta_desc = "�������ܤ���Ϥ��������줿��";
#else
			muta_desc = "You feel a holy wrath fill you.";
#endif

			break;
		case 71: case 72:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_COLD_TOUCH;
#ifdef JP
muta_desc = "���ʤ���ξ��ϤȤƤ��䤿���ʤä���";
#else
			muta_desc = "Your hands get very cold.";
#endif

			break;
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "���ʤ���ʪ���ꤲ���Ϥ��ʤ궯���ʤä��������롣";
#else
			muta_desc = "Your throwing arm feels much stronger.";
#endif

			break;
		case 75:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "���ʤ��϶�˽����ȯ��򵯤����褦�ˤʤä���";
#else
			muta_desc = "You become subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "�������ʤ����餤���¤ˤʤä���";
#else
			muta_desc = "You become an incredible coward!";
#endif

			break;
		case 77:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "���ʤ��ΰ��֤������Գ���ˤʤä���";
#else
			muta_desc = "Your position seems very uncertain...";
#endif

			break;
		case 78:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "���ʤ��ϥ��륳�����ʬ�礹��褦�ˤʤä���";
#else
			muta_desc = "Your body starts producing alcohol!";
#endif

			break;
		case 79:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "���ʤ��ϸ��Ф������������������˿����줿��";
#else
			muta_desc = "You are afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_FLATULENT;
#ifdef JP
muta_desc = "���ʤ���������ǽ�ʶ��������򤳤��褦�ˤʤä���";
#else
			muta_desc = "You become subject to uncontrollable flatulence.";
#endif

			break;
		case 81: case 82:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SCOR_TAIL;
#ifdef JP
muta_desc = "������ο����������Ƥ�����";
#else
			muta_desc = "You grow a scorpion tail!";
#endif

			break;
		case 83: case 84:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�ۤ˳Ѥ���������";
#else
			muta_desc = "Horns pop forth into your forehead!";
#endif

			break;
		case 85: case 86:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BEAK;
#ifdef JP
muta_desc = "�����Ԥ����������Х����Ѳ�������";
#else
			muta_desc = "Your mouth turns into a sharp, powerful beak!";
#endif

			break;
		case 87: case 88:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "���������դ���褦�ˤʤä���";
#else
			muta_desc = "You start attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "���ʤ���������ǽ����ˡ�Υ��ͥ륮����ȯ������褦�ˤʤä���";
#else
			muta_desc = "You start producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "���ʤ����ݵ���ˤʤä���";
#else
			muta_desc = "You become manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "�������Ϥ����ʤ����ظ������Ǥ��뵤�����롣";
#else
			muta_desc = "You feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "���ʤ��ϥ��󥴥ꥢ��Ȥ˴�̯�ʿƤ��ߤ�Ф���褦�ˤʤä���";
#else
			muta_desc = "You feel a strange kinship with Ungoliant.";
#endif

			break;
		case 95: case 96:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TRUNK;
#ifdef JP
muta_desc = "���ʤ���ɡ�Ͽ��Ӥƾݤ�ɡ�Τ褦�ˤʤä���";
#else
			muta_desc = "Your nose grows into an elephant-like trunk.";
#endif

			break;
		case 97:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "ưʪ������դ���褦�ˤʤä���";
#else
			muta_desc = "You start attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TENTACLES;
#ifdef JP
muta_desc = "�ٰ��ʿ��꤬�Τ�ξ¦�������Ƥ�����";
#else
			muta_desc = "Evil-looking tentacles sprout from your sides.";
#endif

			break;
		case 99:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "���Ϥζ��֤��԰���ˤʤä��������롣";
#else
			muta_desc = "You feel the universe is less stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "���ʤ��ϴ�̯�ʤۤ����̤ˤʤä��������롣";
#else
			muta_desc = "You feel strangely normal.";
#endif

			break;
		case 103:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "���ʤ���ͩ�β���������β������ꤹ��褦�ˤʤä���";
#else
			muta_desc = "You start to fade in and out of the physical world.";
#endif

			break;
		case 104:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_POLY_WOUND;
#ifdef JP
muta_desc = "���ʤ��ϥ��������Ϥ��Ť������������Ǥ���Τ򴶤�����";
#else
			muta_desc = "You feel forces of chaos entering your old scars.";
#endif

			break;
		case 105:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WASTING;
#ifdef JP
muta_desc = "���ʤ������������ޤ�������¤ˤ����ä���";
#else
			muta_desc = "You suddenly contract a horrible wasting disease.";
#endif

			break;
		case 106:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "���ʤ��ϥɥ饴�������Ĥ���褦�ˤʤä���";
#else
			muta_desc = "You start attracting dragons.";
#endif

			break;
		case 107: case 108:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WEIRD_MIND;
#ifdef JP
muta_desc = "���ʤ��λ׹ͤ������������������˸����Ϥ᤿��";
#else
			muta_desc = "Your thoughts suddenly take off in strange directions.";
#endif

			break;
		case 109:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NAUSEA;
#ifdef JP
muta_desc = "���ޤ��ԥ��ԥ����Ϥ��᤿��";
#else
			muta_desc = "Your stomach starts to roil nauseously.";
#endif

			break;
		case 110: case 111:
			/* Chaos warriors already have a chaos deity */
			if (p_ptr->pclass != CLASS_CHAOS_WARRIOR)
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "���ʤ��ϥ������μ������դ�椯�褦�ˤʤä���";
#else
				muta_desc = "You attract the notice of a chaos deity!";
#endif

			}
			break;
		case 112:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "���ʤ��ϸ��¤���Τ褦�������ȴ�����褦�ˤʤä���";
#else
			muta_desc = "You feel like reality is as thin as paper.";
#endif

			break;
		case 113: case 114:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "���ʤ��������ѥ�Υ����ˤʤä��������롣";
#else
			muta_desc = "You suddenly feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "���ʤ��Ͻ�ʡ���졢̵Ũ���֤ˤʤ�ȯ��򵯤����褦�ˤʤä���";
#else
			muta_desc = "You are blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SP_TO_HP;
#ifdef JP
muta_desc = "��ˡ�μ�����ȯ��򵯤����褦�ˤʤä���";
#else
			muta_desc = "You are subject to fits of magical healing.";
#endif

			break;
		case 118:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HP_TO_SP;
#ifdef JP
muta_desc = "�ˤߤ�ȼ���������Ʋ���ȯ��򵯤����褦�ˤʤä���";
#else
			muta_desc = "You are subject to fits of painful clarity.";
#endif

			break;
		case 119:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "���ʤ��εӤ�Ĺ�������ܤˤʤä���";
#else
			muta_desc = "Your feet grow to four times their former size.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "Ķ��Ū�˶����ʤä���";
#else
			muta_desc = "You turn into a superhuman he-man!";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "��������äƤ��ޤä�...";
#else
			muta_desc = "Your muscles wither away...";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "���ʤ���Ǿ�����Υ���ԥ塼���˿ʲ�������";
#else
			muta_desc = "Your brain evolves into a living computer!";
#endif

			break;
		case 129: case 130: case 131:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "Ǿ����̤��Ƥ��ޤä�...";
#else
			muta_desc = "Your brain withers away...";
#endif

			break;
		case 132: case 133:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "�³���ƥ��դˤʤä���";
#else
			muta_desc = "You become extraordinarily resilient.";
#endif

			break;
		case 134: case 135:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_FAT;
#ifdef JP
muta_desc = "���ʤ��ϵ������������餤���ä���";
#else
			muta_desc = "You become sickeningly fat!";
#endif

			break;
		case 136: case 137:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "����ӥΤˤʤä����夯�ʤä���������...";
#else
			muta_desc = "You turn into an albino! You feel frail...";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FLESH_ROT;
#ifdef JP
muta_desc = "���ʤ������Τ����Ԥ����µ��˿����줿��";
#else
			muta_desc = "Your flesh is afflicted by a rotting disease!";
#endif

			break;
		case 141: case 142:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SILLY_VOI;
#ifdef JP
muta_desc = "������ȴ���ʥ����������ˤʤä���";
#else
			muta_desc = "Your voice turns into a ridiculous squeak!";
#endif

			break;
		case 143: case 144:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "�Τäڤ�ܤ��ˤʤä���";
#else
			muta_desc = "Your face becomes completely featureless!";
#endif

			break;
		case 145:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ILL_NORM;
#ifdef JP
muta_desc = "���ΰ¤餰���Ƥ�Ǥ��Ф��褦�ˤʤä���";
#else
			muta_desc = "You start projecting a reassuring image.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
muta_desc = "��������Ĥ��ܤ����褿��";
#else
			muta_desc = "You grow an extra pair of eyes!";
#endif

			break;
		case 149: case 150:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "��ˡ�ؤ��������Ĥ�����";
#else
			muta_desc = "You become resistant to magic.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "���ʤ��ϴ�̯�ʲ���Ω�ƻϤ᤿��";
#else
			muta_desc = "You start making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_INFRAVIS;
#ifdef JP
muta_desc = "�ֳ������Ϥ���������";
#else
			muta_desc = "Your infravision is improved.";
#endif

			break;
		case 157: case 158:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_LEGS;
#ifdef JP
muta_desc = "���������ܤ�­�������Ƥ�����";
#else
			muta_desc = "You grow an extra pair of legs!";
#endif

			break;
		case 159: case 160:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SHORT_LEG;
#ifdef JP
muta_desc = "­��û���͵��ˤʤäƤ��ޤä���";
#else
			muta_desc = "Your legs turn into short stubs!";
#endif

			break;
		case 161: case 162:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ELEC_TOUC;
#ifdef JP
muta_desc = "��ɤ���ή��ή��Ϥ᤿��";
#else
			muta_desc = "Electricity starts running through you!";
#endif

			break;
		case 163: case 164:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FIRE_BODY;
#ifdef JP
muta_desc = "���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣";
#else
			muta_desc = "Your body is enveloped in flames!";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WART_SKIN;
#ifdef JP
muta_desc = "�������������ܥ��ܤ�����ˤǤ�����";
#else
			muta_desc = "Disgusting warts appear everywhere on you!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SCALES;
#ifdef JP
muta_desc = "ȩ�������ڤ��Ѥ�ä���";
#else
			muta_desc = "Your skin turns into black scales!";
#endif

			break;
		case 171: case 172:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_IRON_SKIN;
#ifdef JP
muta_desc = "���ʤ���ȩ��Ŵ�ˤʤä���";
#else
			muta_desc = "Your skin turns to steel!";
#endif

			break;
		case 173: case 174:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "����˱�����������";
#else
			muta_desc = "You grow a pair of wings.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "�������ݤ��Τ餺�ˤʤä���";
#else
			muta_desc = "You become completely fearless.";
#endif

			break;
		case 178: case 179:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_REGEN;
#ifdef JP
muta_desc = "��®�˲������Ϥ᤿��";
#else
			muta_desc = "You start regenerating.";
#endif

			break;
		case 180: case 181:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "�ƥ�ѥ�����ǽ�Ϥ�������";
#else
			muta_desc = "You develop a telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "���������ʤ䤫�ˤʤä���";
#else
			muta_desc = "Your muscles become limber.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "���᤬�����ˤ߽Ф�����";
#else
			muta_desc = "Your joints suddenly hurt.";
#endif

			break;
		case 188:
			if (p_ptr->pseikaku == SEIKAKU_LUCKY) break;
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "���դ����������������餬���ʤ���Ȥ�ޤ���...";
#else
			muta_desc = "There is a malignant black aura surrounding you...";
#endif

			break;
		case 189:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "̯��̵�����ˤʤä��������롣";
#else
			muta_desc = "You feel strangely exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "�Τ�ư�������Τˤʤä���";
#else
			muta_desc = "You move with new assurance.";
#endif

			break;
		case 193:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "���ῼ���򤤥����餬���ʤ���Ȥ�ޤ���...";
#else
			muta_desc = "There is a benevolent white aura surrounding you...";
#endif

			break;
		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (!(*muta_class & muta_which))
			{
				muta_chosen = TRUE;
			}
		}
		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
#ifdef JP
msg_print("���̤ˤʤä��������롣");
#else
		msg_print("You feel normal.");
#endif

		return FALSE;
	}
	else
	{
		chg_virtue(V_CHANCE, 1);

		/*
		  some races are apt to gain specified mutations
		  This should be allowed only if "choose_mut" is 0.
		                                        --- henkma
		*/
		if(!choose_mut){
			if (p_ptr->prace == RACE_VAMPIRE &&
			  !(p_ptr->muta1 & MUT1_HYPN_GAZE) &&
			   (randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "�㤬����Ū�ˤʤä�...";
#else
				muta_desc = "Your eyes look mesmerizing...";
#endif

			}

			else if (p_ptr->prace == RACE_IMP &&
				 !(p_ptr->muta2 & MUT2_HORNS) &&
				 (randint1(10) < 7))
			  {
			  	muta_class = &(p_ptr->muta2);
			  	muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�Ѥ��ۤ��������Ƥ�����";
#else
			   	muta_desc = "Horns pop forth into your forehead!";
#endif

			}

			else if (p_ptr->prace == RACE_YEEK &&
				!(p_ptr->muta1 & MUT1_SHRIEK) &&
				(randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_SHRIEK;
#ifdef JP
muta_desc = "���������ʤ궯���ʤä���";
#else
				muta_desc = "Your vocal cords get much tougher.";
#endif

			}

			else if (p_ptr->prace == RACE_BEASTMAN &&
				!(p_ptr->muta1 & MUT1_POLYMORPH) &&
				(randint1(10) < 2))
			{
				muta_class = &(p_ptr->muta1);
				muta_which = MUT1_POLYMORPH;
#ifdef JP
muta_desc = "���ʤ������Τ��Ѳ��Ǥ���褦�ˤʤä���";
#else
				muta_desc = "Your body seems mutable.";
#endif

			}

			else if (p_ptr->prace == RACE_MIND_FLAYER &&
				!(p_ptr->muta2 & MUT2_TENTACLES) &&
				(randint1(10) < 7))
			{
				muta_class = &(p_ptr->muta2);
				muta_which = MUT2_TENTACLES;
#ifdef JP
muta_desc = "�ٰ��ʿ��꤬���μ������������";
#else
				muta_desc = "Evil-looking tentacles sprout from your mouth.";
#endif

			}
		}
#ifdef JP
msg_print("�����Ѱۤ�����");
#else
		msg_print("You mutate!");
#endif

		msg_print(muta_desc);
		*muta_class |= muta_which;

		if (muta_class == &(p_ptr->muta3))
		{
			if (muta_which == MUT3_PUNY)
			{
				if (p_ptr->muta3 & MUT3_HYPER_STR)
				{
#ifdef JP
msg_print("���ʤ��Ϥ⤦Ķ��Ū�˶����Ϥʤ���");
#else
					msg_print("You no longer feel super-strong!");
#endif

					p_ptr->muta3 &= ~(MUT3_HYPER_STR);
				}
			}
			else if (muta_which == MUT3_HYPER_STR)
			{
				if (p_ptr->muta3 & MUT3_PUNY)
				{
#ifdef JP
msg_print("���ʤ��Ϥ⤦����ǤϤʤ���");
#else
					msg_print("You no longer feel puny!");
#endif

					p_ptr->muta3 &= ~(MUT3_PUNY);
				}
			}
			else if (muta_which == MUT3_MORONIC)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT)
				{
#ifdef JP
msg_print("���ʤ���Ǿ�Ϥ⤦���Υ���ԥ塼���ǤϤʤ���");
#else
					msg_print("Your brain is no longer a living computer.");
#endif

					p_ptr->muta3 &= ~(MUT3_HYPER_INT);
				}
			}
			else if (muta_which == MUT3_HYPER_INT)
			{
				if (p_ptr->muta3 & MUT3_MORONIC)
				{
#ifdef JP
msg_print("���ʤ��Ϥ⤦��������ǤϤʤ���");
#else
					msg_print("You are no longer moronic.");
#endif

					p_ptr->muta3 &= ~(MUT3_MORONIC);
				}
			}
			else if (muta_which == MUT3_IRON_SKIN)
			{
				if (p_ptr->muta3 & MUT3_SCALES)
				{
#ifdef JP
msg_print("�ڤ��ʤ��ʤä���");
#else
					msg_print("You lose your scales.");
#endif

					p_ptr->muta3 &= ~(MUT3_SCALES);
				}
				if (p_ptr->muta3 & MUT3_FLESH_ROT)
				{
#ifdef JP
msg_print("���Τ����𤷤ʤ��ʤä���");
#else
					msg_print("Your flesh rots no longer.");
#endif

					p_ptr->muta3 &= ~(MUT3_FLESH_ROT);
				}
				if (p_ptr->muta3 & MUT3_WART_SKIN)
				{
#ifdef JP
msg_print("ȩ�Υ��ܥ��ܤ��ʤ��ʤä���");
#else
					msg_print("You lose your warts.");
#endif

					p_ptr->muta3 &= ~(MUT3_WART_SKIN);
				}
			}
			else if (muta_which == MUT3_WART_SKIN || muta_which == MUT3_SCALES
				|| muta_which == MUT3_FLESH_ROT)
			{
				if (p_ptr->muta3 & MUT3_IRON_SKIN)
				{
#ifdef JP
msg_print("���ʤ���ȩ�Ϥ⤦Ŵ�ǤϤʤ���");
#else
					msg_print("Your skin is no longer made of steel.");
#endif

					p_ptr->muta3 &= ~(MUT3_IRON_SKIN);
				}
			}
			else if (muta_which == MUT3_FEARLESS)
			{
				if (p_ptr->muta2 & MUT2_COWARDICE)
				{
#ifdef JP
msg_print("���¤Ǥʤ��ʤä���");
#else
					msg_print("You are no longer cowardly.");
#endif

					p_ptr->muta2 &= ~(MUT2_COWARDICE);
				}
			}
			else if (muta_which == MUT3_FLESH_ROT)
			{
				if (p_ptr->muta3 & MUT3_REGEN)
				{
#ifdef JP
msg_print("��®�˲������ʤ��ʤä���");
#else
					msg_print("You stop regenerating.");
#endif

					p_ptr->muta3 &= ~(MUT3_REGEN);
				}
			}
			else if (muta_which == MUT3_REGEN)
			{
				if (p_ptr->muta3 & MUT3_FLESH_ROT)
				{
#ifdef JP
msg_print("���Τ����𤷤ʤ��ʤä���");
#else
					msg_print("Your flesh stops rotting.");
#endif

					p_ptr->muta3 &= ~(MUT3_FLESH_ROT);
				}
			}
			else if (muta_which == MUT3_LIMBER)
			{
				if (p_ptr->muta3 & MUT3_ARTHRITIS)
				{
#ifdef JP
msg_print("���᤬�ˤ��ʤ��ʤä���");
#else
					msg_print("Your joints stop hurting.");
#endif

					p_ptr->muta3 &= ~(MUT3_ARTHRITIS);
				}
			}
			else if (muta_which == MUT3_ARTHRITIS)
			{
				if (p_ptr->muta3 & MUT3_LIMBER)
				{
#ifdef JP
msg_print("���ʤ��Ϥ��ʤ䤫�Ǥʤ��ʤä���");
#else
					msg_print("You no longer feel limber.");
#endif

					p_ptr->muta3 &= ~(MUT3_LIMBER);
				}
			}
		}
		else if (muta_class == &(p_ptr->muta2))
		{
			if (muta_which == MUT2_COWARDICE)
			{
				if (p_ptr->muta3 & MUT3_FEARLESS)
				{
#ifdef JP
msg_print("�����Τ餺�Ǥʤ��ʤä���");
#else
					msg_print("You no longer feel fearless.");
#endif

					p_ptr->muta3 &= ~(MUT3_FEARLESS);
				}
			}
			if (muta_which == MUT2_BEAK)
			{
				if (p_ptr->muta2 & MUT2_TRUNK)
				{
#ifdef JP
msg_print("���ʤ���ɡ�Ϥ⤦�ݤ�ɡ�Τ褦�ǤϤʤ��ʤä���");
#else
					msg_print("Your nose is no longer elephantine.");
#endif

					p_ptr->muta2 &= ~(MUT2_TRUNK);
				}
			}
			if (muta_which == MUT2_TRUNK)
			{
				if (p_ptr->muta2 & MUT2_BEAK)
				{
#ifdef JP
msg_print("�Ť������Х����ʤ��ʤä���");
#else
					msg_print("You no longer have a hard beak.");
#endif

					p_ptr->muta2 &= ~(MUT2_BEAK);
				}
			}
		}

		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		p_ptr->update |= PU_BONUS;
		handle_stuff();
		return TRUE;
	}
}


bool lose_mutation(int choose_mut)
{
	int attempts_left = 20;
	cptr muta_desc = "";
	bool muta_chosen = FALSE;
	u32b muta_which = 0;
	u32b *muta_class = NULL;

	if (choose_mut) attempts_left = 1;

	while (attempts_left--)
	{
		switch (choose_mut ? choose_mut : randint1(193))
		{
		case 1: case 2: case 3: case 4:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SPIT_ACID;
#ifdef JP
muta_desc = "����᤭������ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the ability to spit acid.";
#endif

			break;
		case 5: case 6: case 7:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BR_FIRE;
#ifdef JP
muta_desc = "��Υ֥쥹���Ǥ�ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the ability to breathe fire.";
#endif

			break;
		case 8: case 9:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_HYPN_GAZE;
#ifdef JP
muta_desc = "���ʤ����ܤϤĤޤ�ʤ��ܤˤʤä���";
#else
			muta_desc = "Your eyes look uninteresting.";
#endif

			break;
		case 10: case 11:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_TELEKINES;
#ifdef JP
muta_desc = "ǰư�Ϥ�ʪ��ư����ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the ability to move objects telekinetically.";
#endif

			break;
		case 12: case 13: case 14:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VTELEPORT;
#ifdef JP
muta_desc = "��ʬ�ΰջפǥƥ�ݡ��Ȥ���ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the power of teleportation at will.";
#endif

			break;
		case 15: case 16:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIND_BLST;
#ifdef JP
muta_desc = "���������ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the power of Mind Blast.";
#endif

			break;
		case 17: case 18:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RADIATION;
#ifdef JP
muta_desc = "���ʤ�������ǽ��ȯ�����ʤ��ʤä���";
#else
			muta_desc = "You stop emitting hard radiation.";
#endif

			break;
		case 19: case 20:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_VAMPIRISM;
#ifdef JP
muta_desc = "�۷��ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You are no longer vampiric.";
#endif

			break;
		case 21: case 22: case 23:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SMELL_MET;
#ifdef JP
muta_desc = "��°�ν������̤��ʤ��ʤä���";
#else
			muta_desc = "You no longer smell a metallic odor.";
#endif

			break;
		case 24: case 25: case 26: case 27:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SMELL_MON;
#ifdef JP
muta_desc = "�Է�ʥ�󥹥����ν������̤��ʤ��ʤä���";
#else
			muta_desc = "You no longer smell filthy monsters.";
#endif

			break;
		case 28: case 29: case 30:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BLINK;
#ifdef JP
muta_desc = "���Υ�ƥ�ݡ��Ȥ�ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the power of minor teleportation.";
#endif

			break;
		case 31: case 32:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_ROCK;
#ifdef JP
muta_desc = "�ɤ���̣�������˸����ʤ��ʤä���";
#else
			muta_desc = "The walls look unappetizing.";
#endif

			break;
		case 33: case 34:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SWAP_POS;
#ifdef JP
muta_desc = "���ʤ��ϼ�ʬ�η���α�ޤ봶�������롣";
#else
			muta_desc = "You feel like staying in your own shoes.";
#endif

			break;
		case 35: case 36: case 37:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_SHRIEK;
#ifdef JP
muta_desc = "���ʤ��������ϼ夯�ʤä���";
#else
			muta_desc = "Your vocal cords get much weaker.";
#endif

			break;
		case 38: case 39: case 40:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_ILLUMINE;
#ifdef JP
muta_desc = "���������뤯�Ȥ餹���Ȥ�����ʤ��ʤä���";
#else
			muta_desc = "You can no longer light up rooms with your presence.";
#endif

			break;
		case 41: case 42:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DET_CURSE;
#ifdef JP
muta_desc = "�ٰ�����ˡ�򴶤����ʤ��ʤä���";
#else
			muta_desc = "You can no longer feel evil magics.";
#endif

			break;
		case 43: case 44: case 45:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BERSERK;
#ifdef JP
muta_desc = "����Ǥ�����򴶤��ʤ��ʤä���";
#else
			muta_desc = "You no longer feel a controlled rage.";
#endif

			break;
		case 46:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_POLYMORPH;
#ifdef JP
muta_desc = "���ʤ����Τϰ��ꤷ���褦�˸����롣";
#else
			muta_desc = "Your body seems stable.";
#endif

			break;
		case 47: case 48:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_MIDAS_TCH;
#ifdef JP
muta_desc = "�ߥ����μ��ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the Midas touch.";
#endif

			break;
		case 49:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_GROW_MOLD;
#ifdef JP
muta_desc = "�������Ӥ������ˤʤä���";
#else
			muta_desc = "You feel a sudden dislike for mold.";
#endif

			break;
		case 50: case 51: case 52:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RESIST;
#ifdef JP
muta_desc = "���Ĥ��פ��ʤä��������롣";
#else
			muta_desc = "You feel like you might be vulnerable.";
#endif

			break;
		case 53: case 54: case 55:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EARTHQUAKE;
#ifdef JP
muta_desc = "���󥸥������ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the ability to wreck the dungeon.";
#endif

			break;
		case 56:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_EAT_MAGIC;
#ifdef JP
muta_desc = "��ˡ�Υ����ƥ�Ϥ⤦��̣�������˸����ʤ��ʤä���";
#else
			muta_desc = "Your magic items no longer look delicious.";
#endif

			break;
		case 57: case 58:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_WEIGH_MAG;
#ifdef JP
muta_desc = "���Ϥ򴶤����ʤ��ʤä���";
#else
			muta_desc = "You no longer sense magic.";
#endif

			break;
		case 59:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_STERILITY;
#ifdef JP
muta_desc = "��������ΰ��Ȥ���©��ʹ��������";
#else
			muta_desc = "You hear a massed sigh of relief.";
#endif

			break;
		case 60: case 61:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_PANIC_HIT;
#ifdef JP
muta_desc = "����������ķ�٤뵤ʬ���ʤ��ʤä���";
#else
			muta_desc = "You no longer feel jumpy.";
#endif

			break;
		case 62: case 63: case 64:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_DAZZLE;
#ifdef JP
muta_desc = "�ޤФ椤������ȯ����ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose the ability to emit dazzling lights.";
#endif

			break;
		case 65: case 66: case 67:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LASER_EYE;
#ifdef JP
muta_desc = "�㤬�����δ־Ƥ��դ��ơ��ˤߤ��¤餤����";
#else
			muta_desc = "Your eyes burn for a moment, then feel soothed.";
#endif

			break;
		case 68: case 69:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_RECALL;
#ifdef JP
muta_desc = "�����δ֥ۡ��ॷ�å��ˤʤä���";
#else
			muta_desc = "You feel briefly homesick.";
#endif

			break;
		case 70:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_BANISH;
#ifdef JP
muta_desc = "�������ܤ���Ϥ򴶤��ʤ��ʤä���";
#else
			muta_desc = "You no longer feel a holy wrath.";
#endif

			break;
		case 71: case 72:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_COLD_TOUCH;
#ifdef JP
muta_desc = "�꤬�Ȥ����ʤä���";
#else
			muta_desc = "Your hands warm up.";
#endif

			break;
		case 73: case 74:
			muta_class = &(p_ptr->muta1);
			muta_which = MUT1_LAUNCHER;
#ifdef JP
muta_desc = "ʪ���ꤲ��꤬�夯�ʤä��������롣";
#else
			muta_desc = "Your throwing arm feels much weaker.";
#endif

			break;
		case 75:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BERS_RAGE;
#ifdef JP
muta_desc = "��˽����ȯ��ˤ��餵��ʤ��ʤä���";
#else
			muta_desc = "You are no longer subject to fits of berserk rage!";
#endif

			break;
		case 76:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_COWARDICE;
#ifdef JP
muta_desc = "�⤦�����������ۤɲ��¤ǤϤʤ��ʤä���";
#else
			muta_desc = "You are no longer an incredible coward!";
#endif

			break;
		case 77:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RTELEPORT;
#ifdef JP
muta_desc = "���ʤ��ΰ��֤Ϥ�����Ū�ˤʤä���";
#else
			muta_desc = "Your position seems more certain.";
#endif

			break;
		case 78:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ALCOHOL;
#ifdef JP
muta_desc = "���ʤ��ϥ��륳�����ʬ�礷�ʤ��ʤä���";
#else
			muta_desc = "Your body stops producing alcohol!";
#endif

			break;
		case 79:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HALLU;
#ifdef JP
muta_desc = "���Ф�Ҥ������������㳲�򵯤����ʤ��ʤä���";
#else
			muta_desc = "You are no longer afflicted by a hallucinatory insanity!";
#endif

			break;
		case 80:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_FLATULENT;
#ifdef JP
muta_desc = "�⤦���������Ϥ����ʤ��ʤä���";
#else
			muta_desc = "You are no longer subject to uncontrollable flatulence.";
#endif

			break;
		case 81: case 82:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SCOR_TAIL;
#ifdef JP
muta_desc = "������ο������ʤ��ʤä���";
#else
			muta_desc = "You lose your scorpion tail!";
#endif

			break;
		case 83: case 84:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HORNS;
#ifdef JP
muta_desc = "�ۤ���Ѥ��ä�����";
#else
			muta_desc = "Your horns vanish from your forehead!";
#endif

			break;
		case 85: case 86:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BEAK;
#ifdef JP
muta_desc = "�������̤���ä���";
#else
			muta_desc = "Your mouth reverts to normal!";
#endif

			break;
		case 87: case 88:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DEMON;
#ifdef JP
muta_desc = "�ǡ���������󤻤ʤ��ʤä���";
#else
			muta_desc = "You stop attracting demons.";
#endif

			break;
		case 89:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_PROD_MANA;
#ifdef JP
muta_desc = "������ǽ����ˡ�Υ��ͥ륮����ȯ�����ʤ��ʤä���";
#else
			muta_desc = "You stop producing magical energy uncontrollably.";
#endif

			break;
		case 90: case 91:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SPEED_FLUX;
#ifdef JP
muta_desc = "�ݵ���Ǥʤ��ʤä���";
#else
			muta_desc = "You are no longer manic-depressive.";
#endif

			break;
		case 92: case 93:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_BANISH_ALL;
#ifdef JP
muta_desc = "�ظ�˶������Ϥ򴶤��ʤ��ʤä���";
#else
			muta_desc = "You no longer feel a terrifying power lurking behind you.";
#endif

			break;
		case 94:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_EAT_LIGHT;
#ifdef JP
muta_desc = "���������뤤�ȴ����롣";
#else
			muta_desc = "You feel the world's a brighter place.";
#endif

			break;
		case 95: case 96:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TRUNK;
#ifdef JP
muta_desc = "ɡ�����̤�Ĺ������ä���";
#else
			muta_desc = "Your nose returns to a normal length.";
#endif

			break;
		case 97:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_ANIMAL;
#ifdef JP
muta_desc = "ưʪ������󤻤ʤ��ʤä���";
#else
			muta_desc = "You stop attracting animals.";
#endif

			break;
		case 98:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_TENTACLES;
#ifdef JP
muta_desc = "���꤬�ä�����";
#else
			muta_desc = "Your tentacles vanish from your sides.";
#endif

			break;
		case 99:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_RAW_CHAOS;
#ifdef JP
muta_desc = "���Ϥζ��֤����ꤷ���������롣";
#else
			muta_desc = "You feel the universe is more stable around you.";
#endif

			break;
		case 100: case 101: case 102:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NORMALITY;
#ifdef JP
muta_desc = "���̤˴�̯�ʴ��������롣";
#else
			muta_desc = "You feel normally strange.";
#endif

			break;
		case 103:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WRAITH;
#ifdef JP
muta_desc = "���ʤ���ʪ�������ˤ��ä���¸�ߤ��Ƥ��롣";
#else
			muta_desc = "You are firmly in the physical world.";
#endif

			break;
		case 104:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_POLY_WOUND;
#ifdef JP
muta_desc = "�Ť������饫�������Ϥ���äƤ��ä���";
#else
			muta_desc = "You feel forces of chaos departing your old scars.";
#endif

			break;
		case 105:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WASTING;
#ifdef JP
muta_desc = "�����ޤ�������¤����ä���";
#else
			muta_desc = "You are cured of the horrible wasting disease!";
#endif

			break;
		case 106:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_ATT_DRAGON;
#ifdef JP
muta_desc = "�ɥ饴�������󤻤ʤ��ʤä���";
#else
			muta_desc = "You stop attracting dragons.";
#endif

			break;
		case 107: case 108:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WEIRD_MIND;
#ifdef JP
muta_desc = "�׹ͤ��������������ä���";
#else
			muta_desc = "Your thoughts return to boring paths.";
#endif

			break;
		case 109:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_NAUSEA;
#ifdef JP
muta_desc = "�ߤ���ڻ���ʤ��ʤä���";
#else
			muta_desc = "Your stomach stops roiling.";
#endif

			break;
		case 110: case 111:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_CHAOS_GIFT;
#ifdef JP
muta_desc = "���٤ο����ζ�̣��椫�ʤ��ʤä���";
#else
			muta_desc = "You lose the attention of the chaos deities.";
#endif

			break;
		case 112:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WALK_SHAD;
#ifdef JP
muta_desc = "ʪ�������������Ƥ��뵤�����롣";
#else
			muta_desc = "You feel like you're trapped in reality.";
#endif

			break;
		case 113: case 114:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_WARNING;
#ifdef JP
muta_desc = "�ѥ�Υ����Ǥʤ��ʤä���";
#else
			muta_desc = "You no longer feel paranoid.";
#endif

			break;
		case 115:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_INVULN;
#ifdef JP
muta_desc = "̵Ũ���֤�ȯ��򵯤����ʤ��ʤä���";
#else
			muta_desc = "You are no longer blessed with fits of invulnerability.";
#endif

			break;
		case 116: case 117:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_SP_TO_HP;
#ifdef JP
muta_desc = "��ˡ�μ�����ȯ��˽����ʤ��ʤä���";
#else
			muta_desc = "You are no longer subject to fits of magical healing.";
#endif

			break;
		case 118:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_HP_TO_SP;
#ifdef JP
muta_desc = "�ˤߤ�ȼ���������Ʋ���ȯ��˽����ʤ��ʤä���";
#else
			muta_desc = "You are no longer subject to fits of painful clarity.";
#endif

			break;
		case 119:
			muta_class = &(p_ptr->muta2);
			muta_which = MUT2_DISARM;
#ifdef JP
muta_desc = "�Ӥ������礭������ä���";
#else
			muta_desc = "Your feet shrink to their former size.";
#endif

			break;
		case 120: case 121: case 122:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_STR;
#ifdef JP
muta_desc = "���������̤���ä���";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 123: case 124: case 125:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_PUNY;
#ifdef JP
muta_desc = "���������̤���ä���";
#else
			muta_desc = "Your muscles revert to normal.";
#endif

			break;
		case 126: case 127: case 128:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_HYPER_INT;
#ifdef JP
muta_desc = "Ǿ�����̤���ä���";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 129: case 130: case 131:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MORONIC;
#ifdef JP
muta_desc = "Ǿ�����̤���ä���";
#else
			muta_desc = "Your brain reverts to normal.";
#endif

			break;
		case 132: case 133:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_RESILIENT;
#ifdef JP
muta_desc = "���̤ξ��פ�����ä���";
#else
			muta_desc = "You become ordinarily resilient again.";
#endif

			break;
		case 134: case 135:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_FAT;
#ifdef JP
muta_desc = "����Ū�ʥ������åȤ�����������";
#else
			muta_desc = "You benefit from a miracle diet!";
#endif

			break;
		case 136: case 137:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ALBINO;
#ifdef JP
muta_desc = "����ӥΤǤʤ��ʤä���";
#else
			muta_desc = "You are no longer an albino!";
#endif

			break;
		case 138: case 139: case 140:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FLESH_ROT;
#ifdef JP
muta_desc = "���Τ����Ԥ������µ������ä���";
#else
			muta_desc = "Your flesh is no longer afflicted by a rotting disease!";
#endif

			break;
		case 141: case 142:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SILLY_VOI;
#ifdef JP
muta_desc = "���������̤���ä���";
#else
			muta_desc = "Your voice returns to normal.";
#endif

			break;
		case 143: case 144:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BLANK_FAC;
#ifdef JP
muta_desc = "�����ɡ����ä���";
#else
			muta_desc = "Your facial features return.";
#endif

			break;
		case 145:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ILL_NORM;
#ifdef JP
muta_desc = "�����¤餰���Ƥ�Ǥ��Ф��ʤ��ʤä���";
#else
			muta_desc = "You stop projecting a reassuring image.";
#endif

			break;
		case 146: case 147: case 148:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_EYES;
#ifdef JP
muta_desc = ";ʬ���ܤ��ä��Ƥ��ޤä���";
#else
			muta_desc = "Your extra eyes vanish!";
#endif

			break;
		case 149: case 150:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MAGIC_RES;
#ifdef JP
muta_desc = "��ˡ�˼夯�ʤä���";
#else
			muta_desc = "You become susceptible to magic again.";
#endif

			break;
		case 151: case 152: case 153:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_NOIS;
#ifdef JP
muta_desc = "��̯�ʲ���Ω�Ƥʤ��ʤä���";
#else
			muta_desc = "You stop making strange noise!";
#endif

			break;
		case 154: case 155: case 156:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_INFRAVIS;
#ifdef JP
muta_desc = "�ֳ������Ϥ��������";
#else
			muta_desc = "Your infravision is degraded.";
#endif

			break;
		case 157: case 158:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_XTRA_LEGS;
#ifdef JP
muta_desc = ";ʬ�ʵӤ��ä��Ƥ��ޤä���";
#else
			muta_desc = "Your extra legs disappear!";
#endif

			break;
		case 159: case 160:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SHORT_LEG;
#ifdef JP
muta_desc = "�Ӥ�Ĺ�������̤���ä���";
#else
			muta_desc = "Your legs lengthen to normal.";
#endif

			break;
		case 161: case 162:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ELEC_TOUC;
#ifdef JP
muta_desc = "�Τ���ή��ή��ʤ��ʤä���";
#else
			muta_desc = "Electricity stops running through you.";
#endif

			break;
		case 163: case 164:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FIRE_BODY;
#ifdef JP
muta_desc = "�Τ������ޤ�ʤ��ʤä���";
#else
			muta_desc = "Your body is no longer enveloped in flames.";
#endif

			break;
		case 165: case 166: case 167:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WART_SKIN;
#ifdef JP
muta_desc = "���ܥ��ܤ��ä�����";
#else
			muta_desc = "Your warts disappear!";
#endif

			break;
		case 168: case 169: case 170:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_SCALES;
#ifdef JP
muta_desc = "�ڤ��ä�����";
#else
			muta_desc = "Your scales vanish!";
#endif

			break;
		case 171: case 172:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_IRON_SKIN;
#ifdef JP
muta_desc = "ȩ�����ˤ�ɤä���";
#else
			muta_desc = "Your skin reverts to flesh!";
#endif

			break;
		case 173: case 174:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_WINGS;
#ifdef JP
muta_desc = "����α���������������";
#else
			muta_desc = "Your wings fall off.";
#endif

			break;
		case 175: case 176: case 177:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_FEARLESS;
#ifdef JP
muta_desc = "�ƤӶ��ݤ򴶤���褦�ˤʤä���";
#else
			muta_desc = "You begin to feel fear again.";
#endif

			break;
		case 178: case 179:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_REGEN;
#ifdef JP
muta_desc = "��®�������ʤ��ʤä���";
#else
			muta_desc = "You stop regenerating.";
#endif

			break;
		case 180: case 181:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ESP;
#ifdef JP
muta_desc = "�ƥ�ѥ�����ǽ�Ϥ򼺤ä���";
#else
			muta_desc = "You lose your telepathic ability!";
#endif

			break;
		case 182: case 183: case 184:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_LIMBER;
#ifdef JP
muta_desc = "�������Ť��ʤä���";
#else
			muta_desc = "Your muscles stiffen.";
#endif

			break;
		case 185: case 186: case 187:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_ARTHRITIS;
#ifdef JP
muta_desc = "���᤬�ˤ��ʤ��ʤä���";
#else
			muta_desc = "Your joints stop hurting.";
#endif

			break;
		case 188:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_BAD_LUCK;
#ifdef JP
muta_desc = "����������ϱ������ƾä�����";
#else
			muta_desc = "Your black aura swirls and fades.";
#endif

			break;
		case 189:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_VULN_ELEM;
#ifdef JP
muta_desc = "̵�����ʴ����Ϥʤ��ʤä���";
#else
			muta_desc = "You feel less exposed.";
#endif

			break;
		case 190: case 191: case 192:
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_MOTION;
#ifdef JP
muta_desc = "ư������Τ����ʤ��ʤä���";
#else
			muta_desc = "You move with less assurance.";
#endif

			break;
		case 193:
			if (p_ptr->pseikaku == SEIKAKU_LUCKY) break;
			muta_class = &(p_ptr->muta3);
			muta_which = MUT3_GOOD_LUCK;
#ifdef JP
muta_desc = "�򤤥�����ϵ����ƾä�����";
#else
			muta_desc = "Your white aura shimmers and fades.";
#endif

			break;
		default:
			muta_class = NULL;
			muta_which = 0;
		}

		if (muta_class && muta_which)
		{
			if (*(muta_class) & muta_which)
			{
				muta_chosen = TRUE;
			}
		}
		if (muta_chosen == TRUE) break;
	}

	if (!muta_chosen)
	{
		return FALSE;
	}
	else
	{
		msg_print(muta_desc);
		*(muta_class) &= ~(muta_which);

		p_ptr->update |= PU_BONUS;
		handle_stuff();
		mutant_regenerate_mod = calc_mutant_regenerate_mod();
		return TRUE;
	}
}


void dump_mutations(FILE *OutFile)
{
	if (!OutFile) return;

	if (p_ptr->muta1)
	{
		if (p_ptr->muta1 & MUT1_SPIT_ACID)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ���᤭�����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X1)\n");
#else
			fprintf(OutFile, " You can spit acid (dam lvl).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BR_FIRE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϱ�Υ֥쥹���Ǥ����Ȥ��Ǥ��롣(���᡼�� ��٥�X2)\n");
#else
			fprintf(OutFile, " You can breathe fire (dam lvl * 2).\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_HYPN_GAZE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����ˤߤϺ�̲���̤��ġ�\n");
#else
			fprintf(OutFile, " Your gaze is hypnotic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_TELEKINES)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ǰư�Ϥ��äƤ��롣\n");
#else
			fprintf(OutFile, " You are telekinetic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ�ʬ�ΰջפǥƥ�ݡ��ȤǤ��롣\n");
#else
			fprintf(OutFile, " You can teleport at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIND_BLST)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���Ũ����������Ǥ��롣\n");
#else
			fprintf(OutFile, " You can Mind Blast your enemies.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RADIATION)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ�ʬ�ΰջפ�����ǽ��ȯ�����뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can emit hard radiation at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_VAMPIRISM)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϵ۷쵴�Τ褦��Ũ������̿�Ϥ�ۼ����뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can drain life from a foe like a vampire.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SMELL_MET)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��϶᤯�ˤ��뵮��°�򤫤�ʬ���뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can smell nearby precious metal.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SMELL_MON)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��϶᤯�Υ�󥹥�����¸�ߤ򤫤�ʬ���뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can smell nearby monsters.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BLINK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���û����Υ��ƥ�ݡ��ȤǤ��롣\n");
#else
			fprintf(OutFile, " You can teleport yourself short distances.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_ROCK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϹŤ���򿩤٤뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can consume solid rock.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SWAP_POS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���¾�μԤȾ��������ؤ�뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can switch locations with another being.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_SHRIEK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϿȤ��Ӥ����Ķ�������ȯ���뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can emit a horrible shriek.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_ILLUMINE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ������뤤�������Ĥ��Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can emit bright light.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DET_CURSE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼٰ�����ˡ�δ��򴶤��Ȥ뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can feel the danger of evil magic.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BERSERK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ�ʬ�ΰջפǶ�����Ʈ���֤ˤʤ뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can drive yourself into a berserk frenzy.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_POLYMORPH)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ�ʬ�ΰջ֤��Ѳ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can polymorph yourself at will.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_MIDAS_TCH)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����̾異���ƥ�����Ѥ��뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can turn ordinary items to gold.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_GROW_MOLD)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ��Ϥ˥��Υ������䤹���Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can cause mold to grow near you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RESIST)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϸ��Ǥι�����Ф��ƿȤ�Ť����뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can harden yourself to the ravages of the elements.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EARTHQUAKE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ��ϤΥ��󥸥������������뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can bring down the dungeon around your ears.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_EAT_MAGIC)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ�����ˡ�Υ��ͥ륮����ʬ��ʪ�Ȥ��ƻ��ѤǤ��롣\n");
#else
			fprintf(OutFile, " You can consume magic energy for your own use.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_WEIGH_MAG)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼ�ʬ�˱ƶ���Ϳ������ˡ���Ϥ򴶤��뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can feel the strength of the magics affecting you.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_STERILITY)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ͻ���Ū������ǽ�򵯤������Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can cause mass impotence.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_PANIC_HIT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϲ��⤷����Ȥ��뤿��ƨ���뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can run for your life after hitting something.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_DAZZLE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϻ�������ܤ��������������ǽ��ȯ�����뤳�Ȥ��Ǥ��롣 \n");
#else
			fprintf(OutFile, " You can emit confusing, blinding radiation.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_LASER_EYE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����ܤ���졼����������ȯ�ͤ��뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " Your eyes can fire laser beams.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_RECALL)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϳ��ȥ��󥸥��δ֤�Ԥ��褹�뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can travel between town and the depths.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_BANISH)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼٰ��ʥ�󥹥������Ϲ�����Ȥ����Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can send evil creatures directly to Hell.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_COLD_TOUCH)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ʪ�򿨤ä���餻�뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can freeze things with a touch.\n");
#endif

		}
		if (p_ptr->muta1 & MUT1_LAUNCHER)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ����ƥ���϶����ꤲ�뤳�Ȥ��Ǥ��롣\n");
#else
			fprintf(OutFile, " You can hurl objects with great force.\n");
#endif

		}
	}

	if (p_ptr->muta2)
	{
		if (p_ptr->muta2 & MUT2_BERS_RAGE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��϶���β���ȯ��򵯤�����\n");
#else
			fprintf(OutFile, " You are subject to berserker fits.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_COWARDICE)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ������¤ˤʤ롣\n");
#else
			fprintf(OutFile, " You are subject to cowardice.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RTELEPORT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ�����˥ƥ�ݡ��Ȥ��롣\n");
#else
			fprintf(OutFile, " You are teleporting randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ALCOHOL)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����Τϥ��륳�����ʬ�礹�롣\n");
#else
			fprintf(OutFile, " Your body produces alcohol.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HALLU)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϸ��Ф������������������˿�����Ƥ��롣\n");
#else
			fprintf(OutFile, " You have a hallucinatory insanity.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_FLATULENT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ�������Ǥ��ʤ����������򤳤���\n");
#else
			fprintf(OutFile, " You are subject to uncontrollable flatulence.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_PROD_MANA)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���������ǽ����ˡ�Υ��ͥ륮����ȯ���Ƥ��롣\n");
#else
			fprintf(OutFile, " You are producing magical energy uncontrollably.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DEMON)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥǡ���������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract demons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SCOR_TAIL)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ�����ο����������Ƥ��롣(�ǡ����᡼�� 3d7)\n");
#else
			fprintf(OutFile, " You have a scorpion tail (poison, 3d7).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HORNS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϳѤ������Ƥ��롣(���᡼�� 2d6)\n");
#else
			fprintf(OutFile, " You have horns (dam. 2d6).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BEAK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ����Х��������Ƥ��롣(���᡼�� 2d4)\n");
#else
			fprintf(OutFile, " You have a beak (dam. 2d4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SPEED_FLUX)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ�������᤯ư�������٤�ư�����ꤹ�롣\n");
#else
			fprintf(OutFile, " You move faster or slower randomly.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_BANISH_ALL)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ����᤯�Υ�󥹥�������Ǥ����롣\n");
#else
			fprintf(OutFile, " You sometimes cause nearby creatures to vanish.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_EAT_LIGHT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ������Ϥθ���ۼ����Ʊ��ܤˤ��롣\n");
#else
			fprintf(OutFile, " You sometimes feed off of the light around you.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_TRUNK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϾݤΤ褦��ɡ����äƤ��롣(���᡼�� 1d4)\n");
#else
			fprintf(OutFile, " You have an elephantine trunk (dam 1d4).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_ANIMAL)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ưʪ������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract animals.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_TENTACLES)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϼٰ��ʿ������äƤ��롣(���᡼�� 2d5)\n");
#else
			fprintf(OutFile, " You have evil looking tentacles (dam 2d5).\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_RAW_CHAOS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϥ��Ф��н㥫��������ޤ�롣\n");
#else
			fprintf(OutFile, " You occasionally are surrounded with raw chaos.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_NORMALITY)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����Ѱۤ��Ƥ��������������Ƥ��Ƥ��롣\n");
#else
			fprintf(OutFile, " You may be mutated, but you're recovering.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WRAITH)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ������Τ�ͩ�β���������β������ꤹ�롣\n");
#else
			fprintf(OutFile, " You fade in and out of physical reality.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_POLY_WOUND)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��η򹯤ϥ��������Ϥ˱ƶ�������롣\n");
#else
			fprintf(OutFile, " Your health is subject to chaotic forces.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WASTING)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ͽ�夹�붲�����µ��ˤ����äƤ��롣\n");
#else
			fprintf(OutFile, " You have a horrible wasting disease.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_ATT_DRAGON)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥɥ饴�������Ĥ��롣\n");
#else
			fprintf(OutFile, " You attract dragons.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WEIRD_MIND)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��������ϥ�����˳��礷����̾������ꤷ�Ƥ��롣\n");
#else
			fprintf(OutFile, " Your mind randomly expands and contracts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_NAUSEA)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ΰߤ���������夭���ʤ���\n");
#else
			fprintf(OutFile, " You have a seriously upset stomach.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_CHAOS_GIFT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ������μ��⤫��˫���򤦤��Ȥ롣\n");
#else
			fprintf(OutFile, " Chaos deities give you gifts.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WALK_SHAD)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϥ��Ф���¾�Ρֱơפ��¤����ࡣ\n");
#else
			fprintf(OutFile, " You occasionally stumble into other shadows.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_WARNING)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���Ũ�˴ؤ���ٹ�򴶤��롣\n");
#else
			fprintf(OutFile, " You receive warnings about your foes.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_INVULN)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ����餱�Τ餺�ʵ�ʬ�ˤʤ롣\n");
#else
			fprintf(OutFile, " You occasionally feel invincible.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_SP_TO_HP)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ����줬�����ˤɤä�ή��롣\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your muscles.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_HP_TO_SP)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϻ���Ƭ�˷줬�ɤä�ή��롣\n");
#else
			fprintf(OutFile, " Your blood sometimes rushes to your head.\n");
#endif

		}
		if (p_ptr->muta2 & MUT2_DISARM)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϥ褯�ĤޤŤ���ʪ����Ȥ���\n");
#else
			fprintf(OutFile, " You occasionally stumble and drop things.\n");
#endif

		}
	}

	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_HYPER_STR)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���Ķ��Ū�˶�����(����+4)\n");
#else
			fprintf(OutFile, " You are superhumanly strong (+4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_PUNY)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϵ������(����-4)\n");
#else
			fprintf(OutFile, " You are puny (-4 STR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_HYPER_INT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���Ǿ�����Υ���ԥ塼������(��ǽ������+4)\n");
#else
			fprintf(OutFile, " Your brain is a living computer (+4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MORONIC)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��������������(��ǽ������-4)\n");
#else
			fprintf(OutFile, " You are moronic (-4 INT/WIS).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_RESILIENT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����Τ����������٤�Ǥ��롣(�ѵ�+4)\n");
#else
			fprintf(OutFile, " You are very resilient (+4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_FAT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��϶�ü�����äƤ��롣(�ѵ�+2,���ԡ���-2)\n");
#else
			fprintf(OutFile, " You are extremely fat (+2 CON, -2 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ALBINO)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥ���ӥΤ���(�ѵ�-4)\n");
#else
			fprintf(OutFile, " You are albino (-4 CON).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FLESH_ROT)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ������Τ����Ԥ��Ƥ��롣(�ѵ�-2,̥��-1)\n");
#else
			fprintf(OutFile, " Your flesh is rotting (-2 CON, -1 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SILLY_VOI)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ������ϴ�ȴ���ʥ�������������(̥��-4)\n");
#else
			fprintf(OutFile, " Your voice is a silly squeak (-4 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BLANK_FAC)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϤΤäڤ�ܤ�����(̥��-1)\n");
#else
			fprintf(OutFile, " Your face is featureless (-1 CHR).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ILL_NORM)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϸ��Ƥ�ʤ���Ƥ��롣\n");
#else
			fprintf(OutFile, " Your appearance is masked with illusion.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_EYES)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���;ʬ����Ĥ��ܤ���äƤ��롣(õ��+15)\n");
#else
			fprintf(OutFile, " You have an extra pair of eyes (+15 search).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MAGIC_RES)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ�����ˡ�ؤ��������äƤ��롣\n");
#else
			fprintf(OutFile, " You are resistant to magic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_NOIS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����Ѥʲ���ȯ���Ƥ��롣(��̩-3)\n");
#else
			fprintf(OutFile, " You make a lot of strange noise (-3 stealth).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_INFRAVIS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��������餷���ֳ������Ϥ���äƤ��롣(+3)\n");
#else
			fprintf(OutFile, " You have remarkable infravision (+3).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_XTRA_LEGS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���;ʬ�����ܤ�­�������Ƥ��롣(��®+3)\n");
#else
			fprintf(OutFile, " You have an extra pair of legs (+3 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SHORT_LEG)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���­��û���͵�����(��®-3)\n");
#else
			fprintf(OutFile, " Your legs are short stubs (-3 speed).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ELEC_TOUC)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��η�ɤˤ���ή��ή��Ƥ��롣\n");
#else
			fprintf(OutFile, " Electricity is running through your veins.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FIRE_BODY)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����Τϱ�ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " Your body is enveloped in flames.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WART_SKIN)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ȩ�ϥ��ܤ�����Ƥ��롣(̥��-2, AC+5)\n");
#else
			fprintf(OutFile, " Your skin is covered with warts (-2 CHR, +5 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_SCALES)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ȩ���ڤˤʤäƤ��롣(̥��-1, AC+10)\n");
#else
			fprintf(OutFile, " Your skin has turned into scales (-1 CHR, +10 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_IRON_SKIN)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ȩ��Ŵ�ǤǤ��Ƥ��롣(����-1, AC+25)\n");
#else
			fprintf(OutFile, " Your skin is made of steel (-1 DEX, +25 AC).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_WINGS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϱ�����äƤ��롣\n");
#else
			fprintf(OutFile, " You have wings.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����������ݤ򴶤��ʤ���\n");
#else
			fprintf(OutFile, " You are completely fearless.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_REGEN)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϵ�®�˲������롣\n");
#else
			fprintf(OutFile, " You are regenerating.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ESP)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϥƥ�ѥ�������äƤ��롣\n");
#else
			fprintf(OutFile, " You are telepathic.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_LIMBER)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����Τ����ˤ��ʤ䤫����(����+3)\n");
#else
			fprintf(OutFile, " Your body is very limber (+3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_ARTHRITIS)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϥ��Ĥ������ˤߤ򴶤��Ƥ��롣(����-3)\n");
#else
			fprintf(OutFile, " Your joints ache constantly (-3 DEX).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_VULN_ELEM)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��ϸ��Ǥι���˼夤��\n");
#else
			fprintf(OutFile, " You are susceptible to damage from the elements.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_MOTION)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ���ư������Τ��϶�����(��̩+1)\n");
#else
			fprintf(OutFile, " Your movements are precise and forceful (+1 STL).\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_GOOD_LUCK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ����򤤥�����ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " There is a white aura surrounding you.\n");
#endif

		}
		if (p_ptr->muta3 & MUT3_BAD_LUCK)
		{
#ifdef JP
fprintf(OutFile, " ���ʤ��Ϲ���������ˤĤĤޤ�Ƥ��롣\n");
#else
			fprintf(OutFile, " There is a black aura surrounding you.\n");
#endif

		}
	}
}


/*
 * List mutations we have...
 */
void do_cmd_knowledge_mutations(void)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if (fff) dump_mutations(fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
show_file(TRUE, file_name, "�����Ѱ�", 0, 0);
#else
	show_file(TRUE, file_name, "Mutations", 0, 0);
#endif


	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if (x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


static int count_mutations(void)
{
	return (count_bits(p_ptr->muta1) +
	        count_bits(p_ptr->muta2) +
	        count_bits(p_ptr->muta3));
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
int calc_mutant_regenerate_mod(void)
{
	int regen;
	int mod = 10;
	int count = count_mutations();

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	if (p_ptr->pseikaku == SEIKAKU_LUCKY) count--;
	if (p_ptr->prace == RACE_BEASTMAN)
	{
		count -= 10;
		mod = 5;
	}

	/* No negative modifier */
	if (count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if (regen < 10) regen = 10;

	return (regen);
}


void mutation_power_aux(u32b power)
{
	int     dir = 0;
	int     lvl = p_ptr->lev;


	switch (power)
	{
		case MUT1_SPIT_ACID:
			if (racial_aux(9, 9, A_DEX, 15))
			{
#ifdef JP
msg_print("�����Ǥ�������...");
#else
				msg_print("You spit acid...");
#endif

				if (get_aim_dir(&dir))
					fire_ball(GF_ACID, dir, lvl, 1 + (lvl / 30));
			}
			break;

		case MUT1_BR_FIRE:
			if (racial_aux(20, lvl, A_CON, 18))
			{
#ifdef JP
msg_print("���ʤ��ϲб�Υ֥쥹���Ǥ���...");
#else
				msg_print("You breathe fire...");
#endif

				if (get_aim_dir(&dir))
					fire_ball(GF_FIRE, dir, lvl * 2, 1 + (lvl / 20));
			}
			break;

		case MUT1_HYPN_GAZE:
			if (racial_aux(12, 12, A_CHR, 18))
			{
#ifdef JP
msg_print("���ʤ����ܤϸ���Ū�ˤʤä�...");
#else
				msg_print("Your eyes look mesmerizing...");
#endif

				if (get_aim_dir(&dir))
					(void)charm_monster(dir, lvl);
			}
			break;

		case MUT1_TELEKINES:
			if (racial_aux(9, 9, A_WIS, 14))
			{
#ifdef JP
msg_print("���椷�Ƥ���...");
#else
				msg_print("You concentrate...");
#endif

				if (get_aim_dir(&dir))
					fetch(dir, lvl * 10, TRUE);
			}
			break;

		case MUT1_VTELEPORT:
			if (racial_aux(7, 7, A_WIS, 15))
			{
#ifdef JP
msg_print("���椷�Ƥ���...");
#else
				msg_print("You concentrate...");
#endif

				teleport_player(10 + 4 * lvl);
			}
			break;

		case MUT1_MIND_BLST:
			if (racial_aux(5, 3, A_WIS, 15))
			{
#ifdef JP
msg_print("���椷�Ƥ���...");
#else
				msg_print("You concentrate...");
#endif

				if (!get_aim_dir(&dir)) return;
					fire_bolt(GF_PSI, dir, damroll(3 + ((lvl - 1) / 5), 3));
			}
			break;

		case MUT1_RADIATION:
			if (racial_aux(15, 15, A_CON, 14))
			{
#ifdef JP
msg_print("�Τ�������ǽ��ȯ��������");
#else
				msg_print("Radiation flows from your body!");
#endif

				fire_ball(GF_NUKE, 0, (lvl * 2), 3 + (lvl / 20));
			}
			break;

		case MUT1_VAMPIRISM:
			if (racial_aux(2, (1 + (lvl / 3)), A_CON, 9))
			{
				int x, y, dummy;
				cave_type *c_ptr;

				/* Only works on adjacent monsters */
				if (!get_rep_dir2(&dir)) break;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!(c_ptr->m_idx))
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


				dummy = lvl * 2;

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
						(void)set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
				}
				else
#ifdef JP
msg_print("�������Ҥɤ�̣����");
#else
					msg_print("Yechh. That tastes foul.");
#endif

			}
			break;

		case MUT1_SMELL_MET:
			if (racial_aux(3, 2, A_INT, 12))
			{
				(void)detect_treasure(DETECT_RAD_DEFAULT);
			}
			break;

		case MUT1_SMELL_MON:
			if (racial_aux(5, 4, A_INT, 15))
			{
				(void)detect_monsters_normal(DETECT_RAD_DEFAULT);
			}
			break;

		case MUT1_BLINK:
			if (racial_aux(3, 3, A_WIS, 12))
			{
				teleport_player(10);
			}
			break;

		case MUT1_EAT_ROCK:
			if (racial_aux(8, 12, A_CON, 18))
			{
				int x, y, ox, oy;
				cave_type *c_ptr;

				if (!get_rep_dir2(&dir)) break;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];
				if (cave_floor_bold(y, x))
				{
#ifdef JP
msg_print("����ʤ����˳��ߤĤ�����");
#else
					msg_print("You bite into thin air!");
#endif

					break;
				}
				else if (((c_ptr->feat >= FEAT_PERM_EXTRA) &&
					(c_ptr->feat <= FEAT_PERM_SOLID)) ||
					(c_ptr->feat == FEAT_MOUNTAIN))
				{
#ifdef JP
msg_print("���Ƥá������ɤϤ��ʤ��λ����Ť���");
#else
					msg_print("Ouch!  This wall is harder than your teeth!");
#endif

					break;
				}
				else if (c_ptr->m_idx)
				{
#ifdef JP
msg_print("���������⤷�Ƥ��ޤ���");
#else
					msg_print("There's something in the way!");
#endif

					break;
				}
				else if (c_ptr->feat == FEAT_TREES)
				{
#ifdef JP
msg_print("�ڤϤ��ޤ���̣�����ʤ���");
#else
					msg_print("You don't like the woody taste!");
#endif

					break;
				}
				else
				{
					if ((c_ptr->feat >= FEAT_DOOR_HEAD) &&
						(c_ptr->feat <= FEAT_RUBBLE))
					{
						(void)set_food(p_ptr->food + 3000);
					}
					else if ((c_ptr->feat >= FEAT_MAGMA) &&
						(c_ptr->feat <= FEAT_QUARTZ_K))
					{
						(void)set_food(p_ptr->food + 5000);
					}
					else
					{
#ifdef JP
msg_print("���β�־��ϤȤƤ⤪��������");
#else
						msg_print("This granite is very filling!");
#endif

						(void)set_food(p_ptr->food + 10000);
					}
				}
				(void)wall_to_mud(dir);

				oy = py;
				ox = px;

				py = y;
				px = x;

				if (p_ptr->riding)
				{
					m_list[p_ptr->riding].fy = py;
					m_list[p_ptr->riding].fx = px;
					cave[py][px].m_idx = p_ptr->riding;
					cave[oy][ox].m_idx = 0;
					update_mon(cave[py][px].m_idx, TRUE);
				}

				lite_spot(py, px);
				lite_spot(oy, ox);

				verify_panel();

				p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE);
				p_ptr->update |= (PU_DISTANCE);
				p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
			}
			break;

		case MUT1_SWAP_POS:
			if (racial_aux(15, 12, A_DEX, 16))
			{
				project_length = -1;
				if (get_aim_dir(&dir))
					(void)teleport_swap(dir);
				project_length = 0;
			}
			break;

		case MUT1_SHRIEK:
			if (racial_aux(20, 14, A_CON, 16))
			{
				(void)fire_ball(GF_SOUND, 0, 2 * lvl, 8);
				(void)aggravate_monsters(0);
			}
			break;

		case MUT1_ILLUMINE:
			if (racial_aux(3, 2, A_INT, 10))
			{
				(void)lite_area(damroll(2, (lvl / 2)), (lvl / 10) + 1);
			}
			break;

		case MUT1_DET_CURSE:
			if (racial_aux(7, 14, A_WIS, 14))
			{
				int i;

				for (i = 0; i < INVEN_TOTAL; i++)
				{
					object_type *o_ptr = &inventory[i];

					if (!o_ptr->k_idx) continue;
					if (!cursed_p(o_ptr)) continue;

					o_ptr->feeling = FEEL_CURSED;
				}
			}
			break;

		case MUT1_BERSERK:
			if (racial_aux(8, 8, A_STR, 14))
			{
				(void)set_shero(randint1(25) + 25, FALSE);
				(void)hp_player(30);
				(void)set_afraid(0);
			}
			break;

		case MUT1_POLYMORPH:
			if (racial_aux(18, 20, A_CON, 18))
			{
#ifdef JP
				if (!get_check("�ѿȤ��ޤ���������Ǥ�����")) return;
#else
				if (!get_check("You will polymorph your self. Are you sure? ")) return;
#endif
				do_poly_self();
			}
			break;

		case MUT1_MIDAS_TCH:
			if (racial_aux(10, 5, A_INT, 12))
			{
				(void)alchemy();
			}
			break;

		/* Summon pet molds around the player */
		case MUT1_GROW_MOLD:
			if (racial_aux(1, 6, A_CON, 14))
			{
				int i;
				for (i = 0; i < 8; i++)
				{
					summon_specific(-1, py, px, lvl, SUMMON_BIZARRE1, PM_FORCE_PET);
				}
			}
			break;

		case MUT1_RESIST:
			if (racial_aux(10, 12, A_CON, 12))
			{
				int num = lvl / 10;
				int dur = randint1(20) + 20;

				if (randint0(5) < num)
				{
					(void)set_oppose_acid(dur, FALSE);
					num--;
				}
				if (randint0(4) < num)
				{
					(void)set_oppose_elec(dur, FALSE);
					num--;
				}
				if (randint0(3) < num)
				{
					(void)set_oppose_fire(dur, FALSE);
					num--;
				}
				if (randint0(2) < num)
				{
					(void)set_oppose_cold(dur, FALSE);
					num--;
				}
				if (num)
				{
					(void)set_oppose_pois(dur, FALSE);
					num--;
				}
			}
			break;

		case MUT1_EARTHQUAKE:
			if (racial_aux(12, 12, A_STR, 16))
			{
				earthquake(py, px, 10);
			}
			break;

		case MUT1_EAT_MAGIC:
			if (racial_aux(17, 1, A_WIS, 15))
			{
				eat_magic(p_ptr->lev * 2);
			}
			break;

		case MUT1_WEIGH_MAG:
			if (racial_aux(6, 6, A_INT, 10))
			{
				report_magics();
			}
			break;

		case MUT1_STERILITY:
			if (racial_aux(12, 23, A_CHR, 15))
			{
				/* Fake a population explosion. */
#ifdef JP
msg_print("����Ƭ���ˤ��ʤä���");
take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, "���ߤ򶯤�����ϫ", -1);
#else
				msg_print("You suddenly have a headache!");
				take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, "the strain of forcing abstinence", -1);
#endif

				num_repro += MAX_REPRO;
			}
			break;

		case MUT1_PANIC_HIT:
			if (racial_aux(10, 12, A_DEX, 14))
			{
				int x, y;

				if (!get_rep_dir2(&dir)) return;
				y = py + ddy[dir];
				x = px + ddx[dir];
				if (cave[y][x].m_idx)
				{
					py_attack(y, x, 0);
					if (randint0(p_ptr->skill_dis) < 7)
#ifdef JP
msg_print("���ޤ�ƨ�����ʤ��ä���");
#else
						msg_print("You failed to teleport.");
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
			}
			break;

		case MUT1_DAZZLE:
			if (racial_aux(7, 15, A_CHR, 8))
			{
				stun_monsters(lvl * 4);
				confuse_monsters(lvl * 4);
				turn_monsters(lvl * 4);
			}
			break;

		case MUT1_LASER_EYE:
			if (racial_aux(7, 10, A_WIS, 9))
			{
				if (get_aim_dir(&dir))
					fire_beam(GF_LITE, dir, 2 * lvl);
			}
			break;

		case MUT1_RECALL:
			if (racial_aux(17, 50, A_INT, 16))
			{
				(void)word_of_recall();
			}
			break;

		case MUT1_BANISH:
			if (racial_aux(25, 25, A_WIS, 18))
			{
				int x, y;
				cave_type *c_ptr;
				monster_type *m_ptr;
				monster_race *r_ptr;

				if (!get_rep_dir2(&dir)) return;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
msg_print("�ٰ���¸�ߤ򴶤��Ȥ�ޤ���");
#else
					msg_print("You sense no evil there!");
#endif

					break;
				}

				m_ptr = &m_list[c_ptr->m_idx];
				r_ptr = &r_info[m_ptr->r_idx];

				if ((r_ptr->flags3 & RF3_EVIL) &&
				    !(r_ptr->flags1 & RF1_QUESTOR) &&
				    !(r_ptr->flags1 & RF1_UNIQUE) &&
				    !p_ptr->inside_arena && !p_ptr->inside_quest &&
					(r_ptr->level < randint1(p_ptr->lev+50)) &&
					!(m_ptr->mflag2 & MFLAG_NOGENO))
				{
					/* Delete the monster, rather than killing it. */
					delete_monster_idx(c_ptr->m_idx);
#ifdef JP
msg_print("���μٰ��ʥ�󥹥�����β��������ȤȤ�˾ä���ä���");
#else
					msg_print("The evil creature vanishes in a puff of sulfurous smoke!");
#endif

				}
				else
				{
#ifdef JP
msg_print("����ϸ��̤��ʤ��ä���");
#else
					msg_print("Your invocation is ineffectual!");
#endif

					if (one_in_(13)) m_ptr->mflag2 |= MFLAG_NOGENO;
				}
			}
			break;

		case MUT1_COLD_TOUCH:
			if (racial_aux(2, 2, A_CON, 11))
			{
				int x, y;
				cave_type *c_ptr;

				if (!get_rep_dir2(&dir)) return;
				y = py + ddy[dir];
				x = px + ddx[dir];
				c_ptr = &cave[y][x];

				if (!c_ptr->m_idx)
				{
#ifdef JP
msg_print("���ʤ��ϲ���ʤ����Ǽ�򿶤ä���");
#else
					msg_print("You wave your hands in the air.");
#endif

					break;
				}
				fire_bolt(GF_COLD, dir, 2 * lvl);
			}
			break;

		/* XXX_XXX_XXX Hack!  MUT1_LAUNCHER is negative, see above */
		case 3: /* MUT1_LAUNCHER */
			if (racial_aux(1, lvl, A_STR, 6))
			{
				/* Gives a multiplier of 2 at first, up to 3 at 40th */
				do_cmd_throw_aux(2 + lvl / 40, FALSE, 0);
			}
			break;

		default:
			energy_use = 0;
#ifdef JP
msg_format("ǽ�� %s �ϼ�������Ƥ��ޤ���", power);
#else
			msg_format("Power %s not implemented. Oops.", power);
#endif

	}
}
