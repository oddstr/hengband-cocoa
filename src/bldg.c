/* File: bldg.c */

/*
 * Purpose: Building commands
 * Created by Ken Wigle for Kangband - a variant of Angband 2.8.3
 * -KMW-
 *
 * Rewritten for Kangband 2.8.3i using Kamband's version of
 * bldg.c as written by Ivan Tkatchev
 *
 * Changed for ZAngband by Robert Ruehlmann
 */

#include "angband.h"

/* hack as in leave_store in store.c */
static bool leave_bldg = FALSE;

/* remember building location */
static int building_loc = 0;

static bool is_owner(building_type *bldg)
{
	if (bldg->member_class[p_ptr->pclass] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if (bldg->member_race[p_ptr->prace] == BUILDING_OWNER)
	{
		return (TRUE);
	}

	if ((is_magic(p_ptr->realm1) && (bldg->member_realm[p_ptr->realm1] == BUILDING_OWNER)) ||
		(is_magic(p_ptr->realm2) && (bldg->member_realm[p_ptr->realm2] == BUILDING_OWNER)))
	{
		return (TRUE);
	}

	return (FALSE);
}


static bool is_member(building_type *bldg)
{
	if (bldg->member_class[p_ptr->pclass])
	{
		return (TRUE);
	}

	if (bldg->member_race[p_ptr->prace])
	{
		return (TRUE);
	}

	if ((is_magic(p_ptr->realm1) && bldg->member_realm[p_ptr->realm1]) ||
	    (is_magic(p_ptr->realm2) && bldg->member_realm[p_ptr->realm2]))
	{
		return (TRUE);
	}


	if (p_ptr->pclass == CLASS_SORCERER)
	{
		int i;
		bool OK = FALSE;
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (bldg->member_realm[i+1]) OK = TRUE;
		}
		return OK;
	}
	return (FALSE);
}


/*
 * Clear the building information
 */
static void clear_bldg(int min_row, int max_row)
{
	int   i;

	for (i = min_row; i <= max_row; i++)
		prt("", i, 0);
}

static void building_prt_gold(void)
{
	char tmp_str[80];

#ifdef JP
prt("������Τ���: ", 23,53);
#else
	prt("Gold Remaining: ", 23, 53);
#endif


	sprintf(tmp_str, "%9ld", (long)p_ptr->au);
	prt(tmp_str, 23, 68);
}


/*
 * Display a building.
 */
static void show_building(building_type* bldg)
{
	char buff[20];
	int i;
	byte action_color;
	char tmp_str[80];

	Term_clear();
	sprintf(tmp_str, "%s (%s) %35s", bldg->owner_name, bldg->owner_race, bldg->name);
	prt(tmp_str, 2, 1);


	for (i = 0; i < 8; i++)
	{
		if (bldg->letters[i])
		{
			if (bldg->action_restr[i] == 0)
			{
				if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(!is_owner(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->other_costs[i]);
#endif

				}
			}
			else if (bldg->action_restr[i] == 1)
			{
				if (!is_member(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(��Ź)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if ((is_owner(bldg) && (bldg->member_costs[i] == 0)) ||
					(is_member(bldg) && (bldg->other_costs[i] == 0)))
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
				else if (is_owner(bldg))
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->other_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->other_costs[i]);
#endif

				}
			}
			else
			{
				if (!is_owner(bldg))
				{
					action_color = TERM_L_DARK;
#ifdef JP
strcpy(buff, "(��Ź)");
#else
					strcpy(buff, "(closed)");
#endif

				}
				else if (bldg->member_costs[i] != 0)
				{
					action_color = TERM_YELLOW;
#ifdef JP
sprintf(buff, "($%ld)", bldg->member_costs[i]);
#else
					sprintf(buff, "(%ldgp)", bldg->member_costs[i]);
#endif

				}
				else
				{
					action_color = TERM_WHITE;
					buff[0] = '\0';
				}
			}

			sprintf(tmp_str," %c) %s %s", bldg->letters[i], bldg->act_names[i], buff);
			c_put_str(action_color, tmp_str, 19+(i/2), 35*(i%2));
		}
	}

#ifdef JP
prt(" ESC) ��ʪ��Ф�", 23, 0);
#else
	prt(" ESC) Exit building", 23, 0);
#endif

}


/*
 * arena commands
 */
static void arena_comm(int cmd)
{
	monster_race    *r_ptr;
	cptr            name;


	switch (cmd)
	{
		case BACT_ARENA:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
			{
				clear_bldg(5, 19);
#ifdef JP
prt("���꡼�ʤ�ͥ���ԡ�", 5, 0);
prt("����ǤȤ������ʤ������Ƥ�Ũ���ݤ��ޤ�����", 7, 0); 
prt("�޶�Ȥ��� $1,000,000 ��Ϳ�����ޤ���", 8, 0);
#else
				prt("               Arena Victor!", 5, 0);
				prt("Congratulations!  You have defeated all before you.", 7, 0);
				prt("For that, receive the prize: 1,000,000 gold pieces", 8, 0);
#endif

				prt("", 10, 0);
				prt("", 11, 0);
				p_ptr->au += 1000000L;
#ifdef JP
msg_print("���ڡ���������³��");
#else
				msg_print("Press the space bar to continue");
#endif

				msg_print(NULL);
				p_ptr->arena_number++;
			}
			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
				if (p_ptr->arena_number < MAX_ARENA_MONS+2)
				{
#ifdef JP
msg_print("���Τ���˺Ƕ���ĩ��Ԥ��Ѱդ��Ƥ�������");
#else
					msg_print("The strongest challenger is waiting for you.");
#endif

					msg_print(NULL);
#ifdef JP
					if (get_check("ĩ�魯�뤫�͡�"))
#else
					if (get_check("Do you fight? "))
#endif
					{
						p_ptr->leftbldg = TRUE;
						p_ptr->inside_arena = TRUE;
						p_ptr->exit_bldg = FALSE;
						reset_tim_flags();
						p_ptr->leaving = TRUE;
						leave_bldg = TRUE;
					}
					else
					{
#ifdef JP
msg_print("��ǰ����");
#else
						msg_print("We are disappointed.");
#endif
					}
				}
				else
				{
#ifdef JP
msg_print("���ʤ��ϥ��꡼�ʤ����ꡢ���Ф餯�δֱɸ��ˤҤ��ä���");
#else
					msg_print("You enter the arena briefly and bask in your glory.");
#endif

					msg_print(NULL);
				}
			}
			else if (p_ptr->riding && (p_ptr->pclass != CLASS_BEASTMASTER) && (p_ptr->pclass != CLASS_CAVALRY))
			{
#ifdef JP
msg_print("�ڥåȤ˾�ä��ޤޤǤϥ��꡼�ʤ����줵���Ƥ�館�ʤ��ä���");
#else
				msg_print("You don't have permission to enter with pet.");
#endif

				msg_print(NULL);
			}
			else
			{
				p_ptr->leftbldg = TRUE;
				p_ptr->inside_arena = TRUE;
				p_ptr->exit_bldg = FALSE;
				reset_tim_flags();
				p_ptr->leaving = TRUE;
				leave_bldg = TRUE;
			}
			break;
		case BACT_POSTER:
			if (p_ptr->arena_number == MAX_ARENA_MONS)
#ifdef JP
msg_print("���ʤ��Ͼ����Ԥ��� ���꡼�ʤǤΥ����ˡ��˻��ä��ʤ�����");
#else
				msg_print("You are victorious. Enter the arena for the ceremony.");
#endif

			else if (p_ptr->arena_number > MAX_ARENA_MONS)
			{
#ifdef JP
msg_print("���ʤ��Ϥ��٤Ƥ�Ũ�˾���������");
#else
				msg_print("You have won against all foes.");
#endif
			}
			else
			{
				r_ptr = &r_info[arena_monsters[p_ptr->arena_number]];
				name = (r_name + r_ptr->name);
#ifdef JP
msg_format("%s ��ĩ�魯���ΤϤ��ʤ�����", name);
#else
				msg_format("Do I hear any challenges against: %s", name);
#endif
			}
			break;
		case BACT_ARENA_RULES:

			/* Save screen */
			screen_save();

			/* Peruse the arena help file */
#ifdef JP
(void)show_file(TRUE, "arena_j.txt", NULL, 0, 0);
#else
			(void)show_file(TRUE, "arena.txt", NULL, 0, 0);
#endif


			/* Load screen */
			screen_load();

			break;
	}
}


/*
 * display fruit for dice slots
 */
static void display_fruit(int row, int col, int fruit)
{
	switch (fruit)
	{
		case 0: /* lemon */
#ifdef JP
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " ���� ", row + 8, col);
#else
			c_put_str(TERM_YELLOW, "   ####.", row, col);
			c_put_str(TERM_YELLOW, "  #    #", row + 1, col);
			c_put_str(TERM_YELLOW, " #     #", row + 2, col);
			c_put_str(TERM_YELLOW, "#      #", row + 3, col);
			c_put_str(TERM_YELLOW, "#      #", row + 4, col);
			c_put_str(TERM_YELLOW, "#     # ", row + 5, col);
			c_put_str(TERM_YELLOW, "#    #  ", row + 6, col);
			c_put_str(TERM_YELLOW, ".####   ", row + 7, col);
			prt(                   " Lemon  ", row + 8, col);
#endif

			break;
		case 1: /* orange */
#ifdef JP
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   "�����", row + 8, col);
#else
			c_put_str(TERM_ORANGE, "   ##   ", row, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 1, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 2, col);
			c_put_str(TERM_ORANGE, "#......#", row + 3, col);
			c_put_str(TERM_ORANGE, "#......#", row + 4, col);
			c_put_str(TERM_ORANGE, " #....# ", row + 5, col);
			c_put_str(TERM_ORANGE, "  #..#  ", row + 6, col);
			c_put_str(TERM_ORANGE, "   ##   ", row + 7, col);
			prt(                   " Orange ", row + 8, col);
#endif

			break;
		case 2: /* sword */
#ifdef JP
			c_put_str(TERM_SLATE, "   ��   " , row, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ||   " , row + 5, col);
			c_put_str(TERM_UMBER, " |=��=| " , row + 6, col);
			c_put_str(TERM_UMBER, "   ��   " , row + 7, col);
			prt(                  "   ��   " , row + 8, col);
#else
			c_put_str(TERM_SLATE, "   /\\   " , row, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 1, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 2, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 3, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 4, col);
			c_put_str(TERM_SLATE, "   ##   " , row + 5, col);
			c_put_str(TERM_UMBER, " ###### " , row + 6, col);
			c_put_str(TERM_UMBER, "   ##   " , row + 7, col);
			prt(                  " Sword  " , row + 8, col);
#endif

			break;
		case 3: /* shield */
#ifdef JP
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  "   ��   ", row + 8, col);
#else
			c_put_str(TERM_SLATE, " ###### ", row, col);
			c_put_str(TERM_SLATE, "#      #", row + 1, col);
			c_put_str(TERM_SLATE, "# ++++ #", row + 2, col);
			c_put_str(TERM_SLATE, "# +==+ #", row + 3, col);
			c_put_str(TERM_SLATE, "#  ++  #", row + 4, col);
			c_put_str(TERM_SLATE, " #    # ", row + 5, col);
			c_put_str(TERM_SLATE, "  #  #  ", row + 6, col);
			c_put_str(TERM_SLATE, "   ##   ", row + 7, col);
			prt(                  " Shield ", row + 8, col);
#endif

			break;
		case 4: /* plum */
#ifdef JP
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   " �ץ�� ", row + 8, col);
#else
			c_put_str(TERM_VIOLET, "   ##   ", row, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 1, col);
			c_put_str(TERM_VIOLET, "########", row + 2, col);
			c_put_str(TERM_VIOLET, "########", row + 3, col);
			c_put_str(TERM_VIOLET, "########", row + 4, col);
			c_put_str(TERM_VIOLET, " ###### ", row + 5, col);
			c_put_str(TERM_VIOLET, "  ####  ", row + 6, col);
			c_put_str(TERM_VIOLET, "   ##   ", row + 7, col);
			prt(                   "  Plum  ", row + 8, col);
#endif

			break;
		case 5: /* cherry */
#ifdef JP
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                "�����꡼", row + 8, col);
#else
			c_put_str(TERM_RED, "      ##", row, col);
			c_put_str(TERM_RED, "   ###  ", row + 1, col);
			c_put_str(TERM_RED, "  #..#  ", row + 2, col);
			c_put_str(TERM_RED, "  #..#  ", row + 3, col);
			c_put_str(TERM_RED, " ###### ", row + 4, col);
			c_put_str(TERM_RED, "#..##..#", row + 5, col);
			c_put_str(TERM_RED, "#..##..#", row + 6, col);
			c_put_str(TERM_RED, " ##  ## ", row + 7, col);
			prt(                " Cherry ", row + 8, col);
#endif

			break;
	}
}

/*
 * kpoker no (tyuto-hannpa na)pakuri desu...
 * joker ha shineru node haitte masen.
 *
 * TODO: donataka! tsukutte!
 *  - agatta yaku no kiroku (like DQ).
 *  - kakkoii card no e.
 *  - sousa-sei no koujyo.
 *  - code wo wakariyasuku.
 *  - double up.
 *  - Joker... -- done.
 *
 * 9/13/2000 --Koka
 * 9/15/2000 joker wo jissou. soreto, code wo sukosi kakikae. --Habu
 */
#define SUIT_OF(card)  ((card) / 13)
#define NUM_OF(card)   ((card) % 13)
#define IS_JOKER(card) ((card) == 52)

static int cards[5]; /* tefuda no card */

static void reset_deck(int deck[])
{
	int i;
	for (i = 0; i < 53; i++) deck[i] = i;

	/* shuffle cards */
	for (i = 0; i < 53; i++){
		int tmp1 = randint0(53 - i) + i;
		int tmp2 = deck[i];
		deck[i] = deck[tmp1];
		deck[tmp1] = tmp2;
	}
}

static bool have_joker(void)
{
	int i;

	for (i = 0; i < 5; i++){
	  if(IS_JOKER(cards[i])) return TRUE;
	}
	return FALSE;
}

static bool find_card_num(int num)
{
	int i;
	for (i = 0; i < 5; i++)
		if (NUM_OF(cards[i]) == num && !IS_JOKER(cards[i])) return TRUE;
	return FALSE;
}

static bool yaku_check_flush(void)
{
	int i, suit;
	bool joker_is_used = FALSE;

	suit = IS_JOKER(cards[0]) ? SUIT_OF(cards[1]) : SUIT_OF(cards[0]);
	for (i = 0; i < 5; i++){
		if (SUIT_OF(cards[i]) != suit){
		  if(have_joker() && !joker_is_used)
		    joker_is_used = TRUE;
		  else
		    return FALSE;
		}
	}

	return TRUE;
}

static int yaku_check_straight(void)
{
	int i, lowest = 99;
	bool joker_is_used = FALSE;

	/* get lowest */
	for (i = 0; i < 5; i++)
	{
		if (NUM_OF(cards[i]) < lowest && !IS_JOKER(cards[i]))
			lowest = NUM_OF(cards[i]);
	}
	
	if (yaku_check_flush())
	{
	  if( lowest == 0 ){
		for (i = 0; i < 4; i++)
		{
			if (!find_card_num(9 + i)){
				if( have_joker() && !joker_is_used )
				  joker_is_used = TRUE;
				else
				  break;
			}
		}
		if (i == 4) return 3; /* Wow! Royal Flush!!! */
	  }
	  if( lowest == 9 ){
		for (i = 0; i < 3; i++)
		{
			if (!find_card_num(10 + i))
				break;
		}
		if (i == 3 && have_joker()) return 3; /* Wow! Royal Flush!!! */
	  }
	}

	joker_is_used = FALSE;
	for (i = 0; i < 5; i++)
	{
		if (!find_card_num(lowest + i)){
		  if( have_joker() && !joker_is_used )
		    joker_is_used = TRUE;
		  else
		    return 0;
		}
	}
	
	if (yaku_check_flush())
		return 2; /* Straight Flush */

	return 1;
}

/*
 * 0:nopair 1:1 pair 2:2 pair 3:3 cards 4:full house 6:4cards
 */
static int yaku_check_pair(void)
{
	int i, i2, matching = 0;

	for (i = 0; i < 5; i++)
	{
		for (i2 = i+1; i2 < 5; i2++)
		{
			if (IS_JOKER(cards[i]) || IS_JOKER(cards[i2])) continue;
			if (NUM_OF(cards[i]) == NUM_OF(cards[i2]))
				matching++;
		}
	}

	if(have_joker()){
	  switch(matching){
	  case 0:
	    matching = 1;
	    break;
	  case 1:
	    matching = 3;
	    break;
	  case 2:
	    matching = 4;
	    break;
	  case 3:
	    matching = 6;
	    break;
	  case 6:
	    matching = 7;
	    break;
	  default:
	    /* don't reach */
	    break;
	  }
	}

	return matching;
}

#define ODDS_5A 3000
#define ODDS_5C 400
#define ODDS_RF 200
#define ODDS_SF 80
#define ODDS_4C 16
#define ODDS_FH 12
#define ODDS_FL 8
#define ODDS_ST 4
#define ODDS_3C 1
#define ODDS_2P 1

static int yaku_check(void)
{
	prt("                            ", 4, 3);

	switch(yaku_check_straight()){
	case 3: /* RF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "����륹�ȥ졼�ȥե�å���",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Royal Flush",  4,  3);
#endif
		return ODDS_RF;
	case 2: /* SF! */
#ifdef JP
		c_put_str(TERM_YELLOW, "���ȥ졼�ȥե�å���",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight Flush",  4,  3);
#endif
		return ODDS_SF;
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "���ȥ졼��",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Straight",  4,  3);
#endif
		return ODDS_ST;
	default:
		/* Not straight -- fall through */
		break;
	}

	if (yaku_check_flush())
	{

#ifdef JP
	c_put_str(TERM_YELLOW, "�ե�å���",  4,  3);
#else
	c_put_str(TERM_YELLOW, "Flush",  4,  3);
#endif
		return ODDS_FL;
	}

	switch (yaku_check_pair())
	{
	case 1:
#ifdef JP
		c_put_str(TERM_YELLOW, "���ڥ�",  4,  3);
#else
		c_put_str(TERM_YELLOW, "One pair",  4,  3);
#endif
		return 0;
	case 2:
#ifdef JP
		c_put_str(TERM_YELLOW, "�ġ��ڥ�",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Two pair",  4,  3);
#endif
		return ODDS_2P;
	case 3:
#ifdef JP
		c_put_str(TERM_YELLOW, "���꡼������",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Three of a kind",  4,  3);
#endif
		return ODDS_3C;
	case 4:
#ifdef JP
		c_put_str(TERM_YELLOW, "�ե�ϥ���",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Full house",  4,  3);
#endif
		return ODDS_FH;
	case 6:
#ifdef JP
		c_put_str(TERM_YELLOW, "�ե���������",  4,  3);
#else
		c_put_str(TERM_YELLOW, "Four of a kind",  4,  3);
#endif
		return ODDS_4C;
	case 7:
		if (!NUM_OF(cards[0]) || !NUM_OF(cards[1]))
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�ե����֥�����",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five ace",  4,  3);
#endif
			return ODDS_5A;
		}
		else
		{
#ifdef JP
			c_put_str(TERM_YELLOW, "�ե����֥�����",  4,  3);
#else
			c_put_str(TERM_YELLOW, "Five of a kind",  4,  3);
#endif
			return ODDS_5C;
		}
	default:
		break;
	}
	return 0;
}

static void display_kaeruka(int hoge, int kaeruka[])
{
	int i;
	char col = TERM_WHITE;
	for (i = 0; i < 5; i++)
	{
		if (i == hoge) col = TERM_YELLOW;
		else if(kaeruka[i]) col = TERM_WHITE;
		else col = TERM_L_BLUE;
#ifdef JP
		if(kaeruka[i])
			c_put_str(col, "������", 14,  5+i*16);
		else
			c_put_str(col, "�Τ���", 14,  5+i*16);
#else
		if(kaeruka[i])
			c_put_str(col, "Change", 14,  5+i*16);
		else
			c_put_str(col, " Stay ", 14,  5+i*16);
#endif
	}
	if (hoge > 4) col = TERM_YELLOW;
	else col = TERM_WHITE;
#ifdef JP
	c_put_str(col, "����", 16,  38);
#else
	c_put_str(col, "Sure", 16,  38);
#endif

	/* Hilite current option */
	if (hoge < 5) move_cursor(14, 5+hoge*16);
	else move_cursor(16, 38);
}


static void display_cards(void)
{
	int i, j;
	char suitcolor[4] = {TERM_YELLOW, TERM_L_RED, TERM_L_BLUE, TERM_L_GREEN};
#ifdef JP
	cptr suit[4] = {"��", "��", "��", "��"};
	cptr card_grph[13][7] = {{"��   %s     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     %s     ",
				  "          ��"},
				 {"��          ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "          ��"},
				 {"��          ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "          ��"},
				 {"��          ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "          ��"},
				 {"��          ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "          ��"},
				 {"��          ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ��"},
				 {"��          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "          ��"},
				 {"��          ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "          ��"},
				 {"�� %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s ��"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"��   ��     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=��=| %s",
				  "     ��   ��"},
				 {"�� ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##   ��"},
				 {"��          ",
				  "%s ���ޡ�   ",
				  "  �æææ�  ",
				  "  �� �� ��  ",
				  "   ��    �� ",
				  "    �� �� %s",
				  "          ��"}};
	cptr joker_grph[7] = {    "            ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "     ��     ",
				  "            "};

#else

	cptr suit[4] = {"[]", "qp", "<>", "db"};
	cptr card_grph[13][7] = {{"A    %s     ",
				  "     He     ",
				  "     ng     ",
				  "     ba     ",
				  "     nd     ",
				  "     %s     ",
				  "           A"},
				 {"2           ",
				  "     %s     ",
				  "            ",
				  "            ",
				  "            ",
				  "     %s     ",
				  "           2"},
				 {"3           ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "     %s     ",
				  "           3"},
				 {"4           ",
				  "   %s  %s   ",
				  "            ",
				  "            ",
				  "            ",
				  "   %s  %s   ",
				  "           4"},
				 {"5           ",
				  "   %s  %s   ",
				  "            ",
				  "     %s     ",
				  "            ",
				  "   %s  %s   ",
				  "           5"},
				 {"6           ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           6"},
				 {"7           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "           7"},
				 {"8           ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "           8"},
				 {"9  %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s  9"},
				 {"10 %s  %s   ",
				  "     %s     ",
				  "   %s  %s   ",
				  "            ",
				  "   %s  %s   ",
				  "     %s     ",
				  "   %s  %s 10"},
				 {"J    /\\     ",
				  "%s   ||     ",
				  "     ||     ",
				  "     ||     ",
				  "     ||     ",
				  "   |=HH=| %s",
				  "     ][    J"},
				 {"Q  ######   ",
				  "%s#      #  ",
				  "  # ++++ #  ",
				  "  # +==+ #  ",
				  "   # ++ #   ",
				  "    #  #  %s",
				  "     ##    Q"},
				 {"K           ",
				  "%s _'~~`_   ",
				  "   jjjjj$&  ",
				  "   q q uu   ",
				  "   c    &   ",
				  "    v__/  %s",
				  "           K"}};
	cptr joker_grph[7] = {    "            ",
				  "     J      ",
				  "     O      ",
				  "     K      ",
				  "     E      ",
				  "     R      ",
				  "            "};
#endif

	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������",  5,  i*16);
#else
		prt(" +------------+ ",  5,  i*16);
#endif
	}

	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 7; j++)
		{
#ifdef JP
			prt("��",  j+6,  i*16);
#else
			prt(" |",  j+6,  i*16);
#endif
			if(IS_JOKER(cards[i]))
				c_put_str(TERM_VIOLET, joker_grph[j],  j+6,  2+i*16);
			else
				c_put_str(suitcolor[SUIT_OF(cards[i])], format(card_grph[NUM_OF(cards[i])][j], suit[SUIT_OF(cards[i])], suit[SUIT_OF(cards[i])]),  j+6,  2+i*16);
#ifdef JP
			prt("��",  j+6,  i*16+14);
#else
			prt("| ",  j+6,  i*16+14);
#endif
		}
	}
	for (i = 0; i < 5; i++)
	{
#ifdef JP
		prt("����������������", 13,  i*16);
#else
		prt(" +------------+ ", 13,  i*16);
#endif
	}
}

static int do_poker(void)
{
	int i, k = 2;
	char cmd;
	int deck[53]; /* yamafuda : 0...52 */
	int deck_ptr = 0;
	int kaeruka[5]; /* 0:kaenai 1:kaeru */

	bool done = FALSE;
	bool kettei = TRUE;
	bool kakikae = TRUE;

	reset_deck(deck);

	for (i = 0; i < 5; i++)
	{
		cards[i] = deck[deck_ptr++];
		kaeruka[i] = 0; /* default:nokosu */
	}
	
#if 0
	/* debug:RF */
	cards[0] = 12;
	cards[1] = 0;
	cards[2] = 9;
	cards[3] = 11;
	cards[4] = 10;
#endif
#if 0
	/* debug:SF */
	cards[0] = 3;
	cards[1] = 2;
	cards[2] = 4;
	cards[3] = 6;
	cards[4] = 5;
#endif
#if 0
	/* debug:Four Cards */
	cards[0] = 0;
	cards[1] = 0 + 13 * 1;
	cards[2] = 0 + 13 * 2;
	cards[3] = 0 + 13 * 3;
	cards[4] = 51;
#endif
#if 0
	/* debug:Straight */
	cards[0] = 1;
	cards[1] = 0 + 13;
	cards[2] = 3;
	cards[3] = 2 + 26;
	cards[4] = 4;
#endif
#if 0
	/* debug */
	cards[0] = 52;
	cards[1] = 0;
	cards[2] = 1;
	cards[3] = 2;
	cards[4] = 3;
#endif

	/* suteruno wo kimeru */
#ifdef JP
	prt("�Ĥ������ɤ���Ʋ�����(�����ǰ�ư, ���ڡ���������)��", 0, 0);
#else
	prt("Stay witch? ", 0, 0);
#endif

	display_cards();
	yaku_check();

	while (!done)
	{
		if (kakikae) display_kaeruka(k+kettei*5, kaeruka);
		kakikae = FALSE;
		cmd = inkey();
		switch (cmd)
		{
		case '6': case 'l': case 'L': case KTRL('F'):
			if (!kettei) k = (k+1)%5;
			else {k = 0;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '4': case 'h': case 'H': case KTRL('B'):
			if (!kettei) k = (k+4)%5;
			else {k = 4;kettei = FALSE;}
			kakikae = TRUE;
			break;
		case '2': case 'j': case 'J': case KTRL('N'):
			if (!kettei) {kettei = TRUE;kakikae = TRUE;}
			break;
		case '8': case 'k': case 'K': case KTRL('P'):
			if (kettei) {kettei = FALSE;kakikae = TRUE;}
			break;
		case ' ': case '\r':
			if (kettei) done = TRUE;
			else {kaeruka[k] = !kaeruka[k];kakikae = TRUE;}
			break;
		default:
			break;
		}
	}
	
	prt("",0,0);

	for (i = 0; i < 5; i++)
		if (kaeruka[i] == 1) cards[i] = deck[deck_ptr++]; /* soshite toru */

	display_cards();
	
	return yaku_check();
}
#undef SUIT_OF
#undef NUM_OF
#undef IS_JOKER
/* end of poker codes --Koka */

/*
 * gamble_comm
 */
static bool gamble_comm(int cmd)
{
	int i;
	int roll1, roll2, roll3, choice, odds, win;
	s32b wager;
	s32b maxbet;
	s32b oldgold;

	char out_val[160], tmp_str[80], again;
	cptr p;

	screen_save();

	if (cmd == BACT_GAMBLE_RULES)
	{
		/* Peruse the gambling help file */
#ifdef JP
(void)show_file(TRUE, "jgambling.txt", NULL, 0, 0);
#else
		(void)show_file(TRUE, "gambling.txt", NULL, 0, 0);
#endif

	}
	else
	{
		/* No money */
		if (p_ptr->au < 1)
		{
#ifdef JP
			msg_print("���������ޤ���ʸ�ʤ�����ʤ��������ä���ФƤ�����");
#else
			msg_print("Hey! You don't have gold - get out of here!");
#endif

			msg_print(NULL);
			screen_load();
			return FALSE;
		}

		clear_bldg(5, 23);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"�Ҥ��� (1-%ld)��", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au)
			{
#ifdef JP
msg_print("�������⤬­��ʤ�����ʤ������ФƤ�����");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ld������ɤ��������褦���Ĥ�ϼ�äȤ��ʡ�", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�ϣˡ���������ɤ���Ϥ���褦��");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			win = FALSE;
			odds = 0;
			oldgold = p_ptr->au;

#ifdef JP
sprintf(tmp_str, "���������ν����: %9ld", oldgold);
#else
			sprintf(tmp_str, "Gold before game: %9ld", oldgold);
#endif

			prt(tmp_str, 20, 2);

#ifdef JP
sprintf(tmp_str, "���ߤγݤ���:     %9ld", wager);
#else
			sprintf(tmp_str, "Current Wager:    %9ld", wager);
#endif

			prt(tmp_str, 21, 2);

			/* Prevent savefile-scumming of the casino */
/*			Rand_quick = TRUE; */
			Rand_value = time(NULL);

			do
			{
#ifdef JP /* Prevent random seed cracking of the casino */
				clock_t clk;
				clk = clock();
				Rand_value *= clk;
#endif
				p_ptr->au -= wager;
				switch (cmd)
				{
				 case BACT_IN_BETWEEN: /* Game of In-Between */
#ifdef JP
c_put_str(TERM_GREEN, "���󡦥ӥȥ�����",5,2);
#else
					c_put_str(TERM_GREEN, "In Between", 5, 2);
#endif

					odds = 4;
					win = FALSE;
					roll1 = randint1(10);
					roll2 = randint1(10);
					choice = randint1(10);
#ifdef JP
sprintf(tmp_str, "��������: %d        ��������: %d", roll1, roll2);
#else
					sprintf(tmp_str, "Black die: %d       Black Die: %d", roll1, roll2);
#endif

					prt(tmp_str, 8, 3);
#ifdef JP
sprintf(tmp_str, "�֥�����: %d", choice);
#else
					sprintf(tmp_str, "Red die: %d", choice);
#endif

					prt(tmp_str, 11, 14);
					if (((choice > roll1) && (choice < roll2)) ||
						((choice < roll1) && (choice > roll2)))
						win = TRUE;
					break;
				case BACT_CRAPS:  /* Game of Craps */
#ifdef JP
c_put_str(TERM_GREEN, "����åץ�", 5, 2);
#else
					c_put_str(TERM_GREEN, "Craps", 5, 2);
#endif

					win = 3;
					odds = 2;
					roll1 = randint1(6);
					roll2 = randint1(6);
					roll3 = roll1 +  roll2;
					choice = roll3;
#ifdef JP
sprintf(tmp_str, "�������: %d %d      Total: %d", roll1, 
#else
					sprintf(tmp_str, "First roll: %d %d    Total: %d", roll1,
#endif

						 roll2, roll3);
					prt(tmp_str, 7, 5);
					if ((roll3 == 7) || (roll3 == 11))
						win = TRUE;
					else if ((roll3 == 2) || (roll3 == 3) || (roll3 == 12))
						win = FALSE;
					else
						do
						{
#ifdef JP
msg_print("�ʤˤ������򲡤��Ȥ⤦��󿶤�ޤ���");
#else
							msg_print("Hit any key to roll again");
#endif

							msg_print(NULL);
							roll1 = randint1(6);
							roll2 = randint1(6);
							roll3 = roll1 +  roll2;

#ifdef JP
sprintf(tmp_str, "����: %d %d          ���:      %d",
#else
							sprintf(tmp_str, "Roll result: %d %d   Total:     %d",
#endif

								 roll1, roll2, roll3);
							prt(tmp_str, 8, 5);
							if (roll3 == choice)
								win = TRUE;
							else if (roll3 == 7)
								win = FALSE;
						} while ((win != TRUE) && (win != FALSE));
					break;

				case BACT_SPIN_WHEEL:  /* Spin the Wheel Game */
					win = FALSE;
					odds = 9;
#ifdef JP
c_put_str(TERM_GREEN, "�롼��å�", 5, 2);
#else
					c_put_str(TERM_GREEN, "Wheel", 5, 2);
#endif

					prt("0  1  2  3  4  5  6  7  8  9", 7, 5);
					prt("--------------------------------", 8, 3);
					strcpy(out_val, "");
#ifdef JP
get_string("���֡� (0-9): ", out_val, 32);
#else
					get_string("Pick a number (0-9): ", out_val, 32);
#endif

					for (p = out_val; isspace(*p); p++);
					choice = atol(p);
					if (choice < 0)
					{
#ifdef JP
msg_print("0�֤ˤ��Ȥ�����");
#else
						msg_print("I'll put you down for 0.");
#endif

						choice = 0;
					}
					else if (choice > 9)
					{
#ifdef JP
msg_print("�ϣˡ�9�֤ˤ��Ȥ�����");
#else
						msg_print("Ok, I'll put you down for 9.");
#endif

						choice = 9;
					}
					msg_print(NULL);
					roll1 = randint0(10);
#ifdef JP
sprintf(tmp_str, "�롼��åȤϲ�ꡢ�ߤޤä������Ԥ� %d�֤���",
#else
					sprintf(tmp_str, "The wheel spins to a stop and the winner is %d",
#endif

						roll1);
					prt(tmp_str, 13, 3);
					prt("", 9, 0);
					prt("*", 9, (3 * roll1 + 5));
					if (roll1 == choice)
						win = TRUE;
					break;

				case BACT_DICE_SLOTS: /* The Dice Slots */
#ifdef JP
c_put_str(TERM_GREEN, "������������å�", 5, 2);
					c_put_str(TERM_YELLOW, "����   ����            2", 6, 37);
					c_put_str(TERM_YELLOW, "����   ����   ����   5", 7, 37);
					c_put_str(TERM_ORANGE, "����� ����� ����� 10", 8, 37);
					c_put_str(TERM_UMBER, "��       ��       ��       20", 9, 37);
					c_put_str(TERM_SLATE, "��       ��       ��       50", 10, 37);
					c_put_str(TERM_VIOLET, "�ץ��   �ץ��   �ץ��   200", 11, 37);
					c_put_str(TERM_RED, "�����꡼ �����꡼ �����꡼ 1000", 12, 37);
#else
					c_put_str(TERM_GREEN, "Dice Slots", 5, 2);
#endif

					win = FALSE;
					roll1 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll1-i) < 1)
						{
							roll1 = 7-i;
							break;
						}
						roll1 -= i;
					}
					roll2 = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((roll2-i) < 1)
						{
							roll2 = 7-i;
							break;
						}
						roll2 -= i;
					}
					choice = randint1(21);
					for (i=6;i>0;i--)
					{
						if ((choice-i) < 1)
						{
							choice = 7-i;
							break;
						}
						choice -= i;
					}
					put_str("/--------------------------\\", 7, 2);
					prt("\\--------------------------/", 17, 2);
					display_fruit(8,  3, roll1 - 1);
					display_fruit(8, 12, roll2 - 1);
					display_fruit(8, 21, choice - 1);
					if ((roll1 == roll2) && (roll2 == choice))
					{
						win = TRUE;
						switch(roll1)
						{
						case 1:
							odds = 5;break;
						case 2:
							odds = 10;break;
						case 3:
							odds = 20;break;
						case 4:
							odds = 50;break;
						case 5:
							odds = 200;break;
						case 6:
							odds = 1000;break;
						}
					}
					else if ((roll1 == 1) && (roll2 == 1))
					{
						win = TRUE;
						odds = 2;
					}
					break;
				case BACT_POKER:
					win = FALSE;
					odds = do_poker();
					if (odds) win = TRUE;
					break;
				}

				if (win)
				{
#ifdef JP
prt("���ʤ��ξ���", 16, 37);
#else
					prt("YOU WON", 16, 37);
#endif

					p_ptr->au += odds * wager;
#ifdef JP
sprintf(tmp_str, "��Ψ: %d", odds);
#else
					sprintf(tmp_str, "Payoff: %d", odds);
#endif

					prt(tmp_str, 17, 37);
				}
				else
				{
#ifdef JP
prt("���ʤ����餱", 16, 37);
#else
					prt("You Lost", 16, 37);
#endif

					prt("", 17, 37);
				}
#ifdef JP
sprintf(tmp_str, "���ߤν����:     %9ld", p_ptr->au);
#else
				sprintf(tmp_str, "Current Gold:     %9ld", p_ptr->au);
#endif

				prt(tmp_str, 22, 2);
#ifdef JP
prt("�⤦����(Y/N)��", 18, 37);
#else
				prt("Again(Y/N)?", 18, 37);
#endif

				move_cursor(18, 52);
				again = inkey();
				prt("", 16, 37);
				prt("", 17, 37);
				prt("", 18, 37);
				if (wager > p_ptr->au)
				{
#ifdef JP
msg_print("�������⤬­��ʤ�����ʤ�������������ФƹԤ���");
#else
					msg_print("Hey! You don't have the gold - get out of here!");
#endif

					msg_print(NULL);

					/* Get out here */
					break;
				}
			} while ((again == 'y') || (again == 'Y'));

			/* Switch back to complex RNG */
			Rand_quick = FALSE;

			prt("", 18, 37);
			if (p_ptr->au >= oldgold)
			{
#ifdef JP
msg_print("�ֺ�����٤����ʡ��Ǥ⼡�Ϥ��ä������äƤ�뤫��ʡ����Фˡ���");
#else
				msg_print("You came out a winner! We'll win next time, I'm sure.");
#endif
				chg_virtue(V_CHANCE, 3);
			}
			else
			{
#ifdef JP
msg_print("�ֶ�򥹥äƤ��ޤä��ʡ���Ϥϡ������˵��ä���������������");
#else
				msg_print("You lost gold! Haha, better head home.");
#endif
				chg_virtue(V_CHANCE, -3);
			}
		}
		msg_print(NULL);
	}
	screen_load();
	return (TRUE);
}

static bool vault_aux_battle(int r_idx)
{
	int i;
	int dam = 0;

	monster_race *r_ptr = &r_info[r_idx];

	/* Decline town monsters */
/*	if (!monster_dungeon(r_idx)) return FALSE; */

	/* Decline unique monsters */
/*	if (r_ptr->flags1 & (RF1_UNIQUE)) return (FALSE); */
/*	if (r_ptr->flags7 & (RF7_UNIQUE_7)) return (FALSE); */

	if (r_ptr->flags1 & (RF1_NEVER_MOVE)) return (FALSE);
	if (r_ptr->flags2 & (RF2_MULTIPLY)) return (FALSE);
	if (r_ptr->flags2 & (RF2_QUANTUM)) return (FALSE);
	if (r_ptr->flags7 & (RF7_AQUATIC)) return (FALSE);
	if (r_ptr->flags7 & (RF7_CHAMELEON)) return (FALSE);

	for (i = 0; i < 4; i++)
	{
		if (r_ptr->blow[i].method == RBM_EXPLODE) return (FALSE);
		if (r_ptr->blow[i].effect != RBE_DR_MANA) dam += r_ptr->blow[i].d_dice;
	}
	if (!dam && !(r_ptr->flags4 & (RF4_BOLT_MASK | RF4_BEAM_MASK | RF4_BALL_MASK | RF4_BREATH_MASK)) && !(r_ptr->flags5 & (RF5_BOLT_MASK | RF5_BEAM_MASK | RF5_BALL_MASK | RF5_BREATH_MASK)) && !(r_ptr->flags6 & (RF6_BOLT_MASK | RF6_BEAM_MASK | RF6_BALL_MASK | RF6_BREATH_MASK))) return (FALSE);

	/* Okay */
	return (TRUE);
}

void battle_monsters(void)
{
	int total, i;
	int max_dl = 0;
	int mon_level;
	int power[4];
	bool tekitou;
	bool old_inside_battle = p_ptr->inside_battle;

	for (i = 0; i < max_d_idx; i++)
		if (max_dl < max_dlv[i]) max_dl = max_dlv[i];

	mon_level = randint1(MIN(max_dl, 122))+5;
	if (randint0(100) < 60)
	{
		i = randint1(MIN(max_dl, 122))+5;
		mon_level = MAX(i, mon_level);
	}
	if (randint0(100) < 30)
	{
		i = randint1(MIN(max_dl, 122))+5;
		mon_level = MAX(i, mon_level);
	}

	while (1)
	{
		total = 0;
		tekitou = FALSE;
		for(i=0;i<4;i++)
		{
			int r_idx, j;
			while (1)
			{
				get_mon_num_prep(vault_aux_battle, NULL);
				p_ptr->inside_battle = TRUE;
				r_idx = get_mon_num(mon_level);
				p_ptr->inside_battle = old_inside_battle;
				if (!r_idx) continue;

				if ((r_info[r_idx].flags1 & RF1_UNIQUE) || (r_info[r_idx].flags7 & RF7_UNIQUE2))
				{
					if ((r_info[r_idx].level + 10) > mon_level) continue;
				}

				for (j = 0; j < i; j++)
					if(r_idx == battle_mon[j]) break;
				if (j<i) continue;

				break;
			}
			battle_mon[i] = r_idx;
			if (r_info[r_idx].level < 45) tekitou = TRUE;
		}

		for (i=0;i<4;i++)
		{
			monster_race *r_ptr = &r_info[battle_mon[i]];
			int num_taisei = count_bits(r_ptr->flags3 & (RF3_IM_ACID | RF3_IM_ELEC | RF3_IM_FIRE | RF3_IM_COLD | RF3_IM_POIS));

			if (r_ptr->flags1 & RF1_FORCE_MAXHP)
				power[i] = r_ptr->hdice * r_ptr->hside * 2;
			else
				power[i] = r_ptr->hdice * (r_ptr->hside + 1);
			power[i] = power[i] * (100 + r_ptr->level) / 100;
			if (r_ptr->speed > 110)
				power[i] = power[i] * (r_ptr->speed * 2 - 110) / 100;
			if (r_ptr->speed < 110)
				power[i] = power[i] * (r_ptr->speed - 20) / 100;
			if (num_taisei > 2)
				power[i] = power[i] * (num_taisei*2+5) / 10;
			else if (r_ptr->flags6 & RF6_INVULNER)
				power[i] = power[i] * 4 / 3;
			else if (r_ptr->flags6 & RF6_HEAL)
				power[i] = power[i] * 4 / 3;
			else if (r_ptr->flags5 & RF5_DRAIN_MANA)
				power[i] = power[i] * 11 / 10;
			if (r_ptr->flags1 & RF1_RAND_25)
				power[i] = power[i] * 9 / 10;
			if (r_ptr->flags1 & RF1_RAND_50)
				power[i] = power[i] * 9 / 10;

			switch (battle_mon[i])
			{
				case MON_GREEN_G:
				case MON_THAT_BAT:
				case MON_GHOST_Q:
					power[i] /= 4;
					break;
				case MON_LOST_SOUL:
				case MON_GHOST:
					power[i] /= 2;
					break;
				case MON_UND_BEHOLDER:
				case MON_SANTACLAUS:
				case MON_ULT_BEHOLDER:
				case MON_UNGOLIANT:
				case MON_ATLACH_NACHA:
				case MON_Y_GOLONAC:
					power[i] = power[i] * 3 / 5;
					break;
				case MON_ROBIN_HOOD:
				case MON_RICH:
				case MON_LICH:
				case MON_COLOSSUS:
				case MON_CRYPT_THING:
				case MON_MASTER_LICH:
				case MON_DREADMASTER:
				case MON_DEMILICH:
				case MON_SHADOWLORD:
				case MON_ARCHLICH:
				case MON_BLEYS:
				case MON_CAIN:
				case MON_JULIAN:
				case MON_VENOM_WYRM:
				case MON_MASTER_MYS:
				case MON_G_MASTER_MYS:
					power[i] = power[i] * 3 / 4;
					break;
				case MON_VORPAL_BUNNY:
				case MON_SHAGRAT:
				case MON_GORBAG:
				case MON_LOG_MASTER:
				case MON_JURT:
				case MON_GRAV_HOUND:
				case MON_SHIM_VOR:
				case MON_JUBJUB:
				case MON_CLUB_DEMON:
				case MON_LLOIGOR:
				case MON_NIGHTCRAWLER:
				case MON_NIGHTWALKER:
				case MON_RAPHAEL:
				case MON_SHAMBLER:
				case MON_SKY_DRAKE:
				case MON_GERARD:
				case MON_G_CTHULHU:
				case MON_SPECT_WYRM:
				case MON_BAZOOKER:
				case MON_GCWADL:
				case MON_KIRIN:
				case MON_HOUOU:
					power[i] = power[i] * 4 / 3;
					break;
				case MON_UMBER_HULK:
				case MON_FIRE_VOR:
				case MON_WATER_VOR:
				case MON_COLD_VOR:
				case MON_ENERGY_VOR:
				case MON_BARNEY:
				case MON_REVENANT:
				case MON_NEXUS_VOR:
				case MON_PLASMA_VOR:
				case MON_TIME_VOR:
				case MON_MANDOR:
				case MON_KAVLAX:
				case MON_RINALDO:
				case MON_STORMBRINGER:
				case MON_TIME_HOUND:
				case MON_PLASMA_HOUND:
				case MON_TINDALOS:
				case MON_CHAOS_VOR:
				case MON_AETHER_VOR:
				case MON_AETHER_HOUND:
				case MON_CANTORAS:
				case MON_GODZILLA:
				case MON_TARRASQUE:
				case MON_DESTROYER:
				case MON_MORGOTH:
				case MON_SERPENT:
				case MON_OROCHI:
				case MON_D_ELF_SHADE:
				case MON_MANA_HOUND:
				case MON_SHARD_VOR:
				case MON_BANORLUPART:
				case MON_BOTEI:
				case MON_JAIAN:
				case MON_BAHAMUT:
				case MON_WAHHA:
					power[i] = power[i] * 3 / 2;
					break;
				case MON_ROLENTO:
				case MON_CYBER:
				case MON_CYBER_KING:
				case MON_UNICORN_ORD:
					power[i] = power[i] * 5 / 3;
					break;
				case MON_ARCH_VILE:
				case MON_PHANTOM_B:
				case MON_WYRM_POWER:
					power[i] *= 2;
					break;
				case MON_NODENS:
				case MON_CULVERIN:
					power[i] *= 3;
					break;
				case MON_ECHIZEN:
					power[i] *= 9;
					break;
				case MON_HAGURE:
					power[i] *= 100000;
					break;
				default:
					break;
			}
			total += power[i];
		}
		for (i=0;i<4;i++)
		{
			power[i] = total*60/power[i];
			if (tekitou && ((power[i] < 160) || power[i] > 1500)) break;
			if ((power[i] < 160) && randint0(20)) break;
			if (power[i] < 101) power[i] = 100 + randint1(5);
			mon_odds[i] = power[i];
		}
		if (i == 4) break;
	}
}

static bool kakutoujou(void)
{
	s32b maxbet;
	s32b wager;
	char out_val[160], tmp_str[80];
	cptr p;

	if ((turn - old_battle) > TURNS_PER_TICK*250)
	{
		battle_monsters();
		old_battle = turn;
	}

	screen_save();

	/* No money */
	if (p_ptr->au < 1)
	{
#ifdef JP
		msg_print("���������ޤ���ʸ�ʤ�����ʤ��������ä���ФƤ�����");
#else
		msg_print("Hey! You don't have gold - get out of here!");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}
	else
	{
		int i;

		clear_bldg(4, 10);

#ifdef JP
		prt("��󥹥���                                                     ��Ψ", 4, 4);
#else
		prt("Monsters                                                       Odds", 4, 4);
#endif
		for (i=0;i<4;i++)
		{
			char buf[80];
			monster_race *r_ptr = &r_info[battle_mon[i]];

#ifdef JP
			sprintf(buf,"%d) %-58s  %4ld.%02ld��", i+1, format("%s%s",r_name + r_ptr->name, (r_ptr->flags1 & RF1_UNIQUE) ? "��ɤ�" : "      "), mon_odds[i]/100, mon_odds[i]%100);
#else
			sprintf(buf,"%d) %-58s  %4ld.%02ld", i+1, format("%s%s", (r_ptr->flags1 & RF1_UNIQUE) ? "Fake " : "", r_name + r_ptr->name), mon_odds[i]/100, mon_odds[i]%100);
#endif
			prt(buf, 5+i, 1);
		}

#ifdef JP
		prt("�ɤ���Ҥ��ޤ���:", 0, 0);
#else
		prt("Which monster: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if (i == ESCAPE)
			{
				screen_load();
				return FALSE;
			}
			if (i >= '1' && i <= '4')
			{
				sel_monster = i-'1';
				battle_odds = mon_odds[sel_monster];
				break;
			}
			else bell();
		}

		clear_bldg(4,4);
		for (i=0;i<4;i++)
			if (i !=sel_monster) clear_bldg(i+5,i+5);

		maxbet = p_ptr->lev * 200;

		/* We can't bet more than we have */
		maxbet = MIN(maxbet, p_ptr->au);

		/* Get the wager */
		strcpy(out_val, "");
#ifdef JP
sprintf(tmp_str,"�Ҥ��� (1-%ld)��", maxbet);
#else
		sprintf(tmp_str,"Your wager (1-%ld) ? ", maxbet);
#endif


		/*
		 * Use get_string() because we may need more than
		 * the s16b value returned by get_quantity().
		 */
		if (get_string(tmp_str, out_val, 32))
		{
			/* Strip spaces */
			for (p = out_val; *p == ' '; p++);

			/* Get the wager */
			wager = atol(p);

			if (wager > p_ptr->au)
			{
#ifdef JP
msg_print("�������⤬­��ʤ�����ʤ������ФƤ�����");
#else
				msg_print("Hey! You don't have the gold - get out of here!");
#endif

				msg_print(NULL);
				screen_load();
				return (FALSE);
			}
			else if (wager > maxbet)
			{
#ifdef JP
msg_format("%ld������ɤ��������褦���Ĥ�ϼ�äȤ��ʡ�", maxbet);
#else
				msg_format("I'll take %ld gold of that. Keep the rest.", maxbet);
#endif

				wager = maxbet;
			}
			else if (wager < 1)
			{
#ifdef JP
msg_print("�ϣˡ���������ɤǤ�������");
#else
				msg_print("Ok, we'll start with 1 gold.");
#endif


				wager = 1;
			}
			msg_print(NULL);
			battle_odds = MAX(wager+1, wager * battle_odds / 100);
			kakekin = wager;
			p_ptr->au -= wager;
			p_ptr->leftbldg = TRUE;
			p_ptr->inside_battle = TRUE;
			reset_tim_flags();
			p_ptr->leaving = TRUE;
			leave_bldg = TRUE;
			screen_load();

			return (TRUE);
		}
	}
	screen_load();

	return (FALSE);
}

static void today_target(void)
{
	char buf[160];
	monster_race *r_ptr = &r_info[today_mon];

	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "�����ξ޶��", 5, 10);
#else
	prt("Wanted monster that changes from day to day", 5, 10);
#endif
#ifdef JP
	sprintf(buf,"�������åȡ� %s",r_name + r_ptr->name);
#else
	sprintf(buf,"target: %s",r_name + r_ptr->name);
#endif
	c_put_str(TERM_YELLOW, buf, 6, 10);
#ifdef JP
	sprintf(buf,"���� ---- $%d",r_ptr->level * 50 + 100);
#else
	sprintf(buf,"corpse   ---- $%d",r_ptr->level * 50 + 100);
#endif
	prt(buf, 8, 10);
#ifdef JP
	sprintf(buf,"��   ---- $%d",r_ptr->level * 30 + 60);
#else
	sprintf(buf,"skeleton ---- $%d",r_ptr->level * 30 + 60);
#endif
	prt(buf, 9, 10);
	p_ptr->today_mon = today_mon;
}

static void tsuchinoko(void)
{
	clear_bldg(4,18);
#ifdef JP
c_put_str(TERM_YELLOW, "������������󥹡�����", 5, 10);
c_put_str(TERM_YELLOW, "�������åȡ��������á֥ĥ��Υ���", 6, 10);
c_put_str(TERM_WHITE, "������� ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "����     ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "��       ----   $100,000", 10, 10);
#else
c_put_str(TERM_YELLOW, "Big chance to quick money!!!", 5, 10);
c_put_str(TERM_YELLOW, "target: the rarest animal 'Tsuchinoko'", 6, 10);
c_put_str(TERM_WHITE, "catch alive ---- $1,000,000", 8, 10);
c_put_str(TERM_WHITE, "corpse      ----   $200,000", 9, 10);
c_put_str(TERM_WHITE, "bones       ----   $100,000", 10, 10);
#endif
}

static void shoukinkubi(void)
{
	int i;
	int y = 0;

	clear_bldg(4,18);

#ifdef JP
	prt("���Τ��������Ⱦ޶�򺹤��夲�ޤ���",4 ,10);
c_put_str(TERM_YELLOW, "���ߤξ޶��", 6, 10);
#else
	prt("Offer a prize when you bring a wanted monster's corpse",4 ,10);
c_put_str(TERM_YELLOW, "Wanted monsters", 6, 10);
#endif

	for (i = 0; i < MAX_KUBI; i++)
	{
		char buf[160];
		monster_race *r_ptr = &r_info[(kubi_r_idx[i] > 10000 ? kubi_r_idx[i] - 10000 : kubi_r_idx[i])];

		sprintf(buf,"%-40s ---- ",r_name + r_ptr->name);
		prt(buf, y+7, 10);
		if (kubi_r_idx[i] > 10000)
#ifdef JP
			c_put_str(TERM_RED,"��", y+7, 56);
#else
			c_put_str(TERM_RED,"done", y+7, 56);
#endif
		else
		{
			sprintf(buf,"$%d", 300 * (r_ptr->level + 1));
			prt(buf, y+7, 56);
		}
		y = (y+1) % 10;
		if (!y && (i < MAX_KUBI -1))
		{
#ifdef JP
			prt("���������򲡤��Ƥ�������", 0, 0);
#else
			prt("Hit any key.", 0, 0);
#endif
			(void)inkey();
			prt("", 0, 0);
			clear_bldg(7,18);
		}
	}
}

static bool kankin(void)
{
	int i, j;
	bool change = FALSE;
	char o_name[MAX_NLEN];
	object_type *o_ptr;

	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CAPTURE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
			sprintf(buf, "%s �򴹶⤷�ޤ�����",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�޶� %ld��������줿��", 1000000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 1000000L * o_ptr->number);
#endif
				p_ptr->au += 1000000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}
	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
			sprintf(buf, "%s �򴹶⤷�ޤ�����",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�޶� %ld��������줿��", 200000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 200000L * o_ptr->number);
#endif
				p_ptr->au += 200000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}
	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == MON_TSUCHINOKO))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
			sprintf(buf, "%s �򴹶⤷�ޤ�����",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�޶� %ld��������줿��", 100000L * o_ptr->number);
#else
				msg_format("You get %ldgp.", 100000L * o_ptr->number);
#endif
				p_ptr->au += 100000L * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == today_mon))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
			sprintf(buf, "%s �򴹶⤷�ޤ�����",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�޶� %ld��������줿��", (r_info[today_mon].level * 50 + 100) * o_ptr->number);
#else
				msg_format("You get %ldgp.", (r_info[today_mon].level * 50 + 100) * o_ptr->number);
#endif
				p_ptr->au += (r_info[today_mon].level * 50 + 100) * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}
	for (i = 0;i <= INVEN_LARM; i++)
	{
		o_ptr = &inventory[i];
		if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_SKELETON) && (o_ptr->pval == today_mon))
		{
			char buf[MAX_NLEN+20];
			object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
			sprintf(buf, "%s �򴹶⤷�ޤ�����",o_name);
#else
			sprintf(buf, "Convert %s into money? ",o_name);
#endif
			if (get_check(buf))
			{
#ifdef JP
				msg_format("�޶� %ld��������줿��", (r_info[today_mon].level * 30 + 60) * o_ptr->number);
#else
				msg_format("You get %ldgp.", (r_info[today_mon].level * 30 + 60) * o_ptr->number);
#endif
				p_ptr->au += (r_info[today_mon].level * 30 + 60) * o_ptr->number;
				p_ptr->redraw |= (PR_GOLD);
				inven_item_increase(i, -o_ptr->number);
				inven_item_describe(i);
				inven_item_optimize(i);
			}
			change = TRUE;
		}
	}

	for (j = 0; j < MAX_KUBI; j++)
	{
		for (i = 0;i <= INVEN_LARM; i++)
		{
			o_ptr = &inventory[i];
			if ((o_ptr->tval == TV_CORPSE) && (o_ptr->sval == SV_CORPSE) && (o_ptr->pval == kubi_r_idx[j]))
			{
				char buf[MAX_NLEN+20];
				object_desc(o_name, o_ptr, TRUE, 3);
#ifdef JP
				sprintf(buf, "%s �򴹶⤷�ޤ�����",o_name);
#else
				sprintf(buf, "Convert %s into money? ",o_name);
#endif
				if (get_check(buf))
				{
#ifdef JP
					msg_format("�޶� %ld��������줿��", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#else
					msg_format("You get %ldgp.", (r_info[kubi_r_idx[j]].level + 1) * 300 * o_ptr->number);
#endif
					p_ptr->au += (r_info[kubi_r_idx[j]].level+1) * 300 * o_ptr->number;
					p_ptr->redraw |= (PR_GOLD);
					inven_item_increase(i, -o_ptr->number);
					inven_item_describe(i);
					inven_item_optimize(i);
					chg_virtue(V_JUSTICE, 5);
					kubi_r_idx[j] += 10000;
				}
				change = TRUE;
			}
		}
	}

	if (!change)
	{
#ifdef JP
		msg_print("�޶������줽���ʤ�Τϻ��äƤ��ʤ��ä���");
#else
		msg_print("You have nothing.");
#endif
		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

bool get_nightmare(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* Require eldritch horrors */
	if (!(r_ptr->flags2 & (RF2_ELDRITCH_HORROR))) return (FALSE);

	/* Require high level */
	if (r_ptr->level <= p_ptr->lev) return (FALSE);

	/* Accept this monster */
	return (TRUE);
}


void have_nightmare(int r_idx)
{
	bool happened = FALSE;

	int power = 100;

	monster_race *r_ptr = &r_info[r_idx];

	char m_name[80];
	cptr desc = r_name + r_ptr->name;


	power = r_ptr->level + 10;

#ifdef JP
	if (0)
#else
	if (!(r_ptr->flags1 & RF1_UNIQUE))
#endif

	{
		/* Describe it */
		sprintf(m_name, "%s %s", (is_a_vowel(desc[0]) ? "an" : "a"), desc);

		if (r_ptr->flags1 & RF1_FRIENDS)
		{
			power /= 2;
		}
	}
	else
	{
		/* Describe it */
		sprintf(m_name, "%s", desc);

		power *= 2;
	}

	if (saving_throw(p_ptr->skill_sav * 100 / power))
	{
#ifdef JP
	msg_format("̴�����%s���ɤ�������줿��", m_name);
#else
	msg_format("%^s chases you through your dreams.", m_name);
#endif


		/* Safe */
		return;
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

		/* Never mind; we can't see it clearly enough */
		return;
	}

	/* Something frightening happens... */
#ifdef JP
msg_format("%s%s�δ�򸫤Ƥ��ޤä���",
#else
	msg_format("You behold the %s visage of %s!",
#endif

				  horror_desc[randint0(MAX_SAN_HORROR)], desc);

	r_ptr->r_flags2 |= RF2_ELDRITCH_HORROR;

	switch(p_ptr->prace)
	{
		/* Imps may make a saving throw */
		case RACE_IMP:
		case RACE_DEMON:
		{
			if (saving_throw(20 + p_ptr->lev)) return;
		}
		/* Undead may make a saving throw */
		case RACE_SKELETON:
		case RACE_ZOMBIE:
		case RACE_SPECTRE:
		case RACE_VAMPIRE:
		{
			if (saving_throw(10 + p_ptr->lev)) return;
		}
	}

	/* Mind blast */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
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

	/* Lose int & wis */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		do_dec_stat(A_INT);
		do_dec_stat(A_WIS);
		return;
	}

	/* Brain smash */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (!p_ptr->resist_conf)
		{
			(void)set_confused(p_ptr->confused + randint0(4) + 4);
		}
		if (!p_ptr->free_act)
		{
			(void)set_paralyzed(p_ptr->paralyzed + randint0(4) + 4);
		}
		while (!saving_throw(p_ptr->skill_sav))
		{
			(void)do_dec_stat(A_INT);
		}
		while (!saving_throw(p_ptr->skill_sav))
		{
			(void)do_dec_stat(A_WIS);
		}
		if (!p_ptr->resist_chaos)
		{
			(void)set_image(p_ptr->image + randint0(250) + 150);
		}
		return;
	}


	/* Amnesia */
	if (!saving_throw(p_ptr->skill_sav * 100 / power))
	{
		if (lose_all_info())
		{
#ifdef JP
msg_print("���ޤ�ζ��ݤ����ƤΤ��Ȥ�˺��Ƥ��ޤä���");
#else
			msg_print("You forget everything in your utmost terror!");
#endif

		}
		return;
	}

	/* Else gain permanent insanity */
	if ((p_ptr->muta3 & MUT3_MORONIC) && (p_ptr->muta2 & MUT2_BERS_RAGE) &&
		((p_ptr->muta2 & MUT2_COWARDICE) || (p_ptr->resist_fear)) &&
		((p_ptr->muta2 & MUT2_HALLU) || (p_ptr->resist_chaos)))
	{
		/* The poor bastard already has all possible insanities! */
		return;
	}

	while (!happened)
	{
		switch (randint1(4))
		{
			case 1:
			{
				if (!(p_ptr->muta3 & MUT3_MORONIC))
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
			}
			case 2:
			{
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
			}
			case 3:
			{
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
			}
			default:
			{
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
	}

	p_ptr->update |= PU_BONUS;
	handle_stuff();
}


/*
 * inn commands
 * Note that resting for the night was a perfect way to avoid player
 * ghosts in the town *if* you could only make it to the inn in time (-:
 * Now that the ghosts are temporarily disabled in 2.8.X, this function
 * will not be that useful.  I will keep it in the hopes the player
 * ghost code does become a reality again. Does help to avoid filthy urchins.
 * Resting at night is also a quick way to restock stores -KMW-
 */
static bool inn_comm(int cmd)
{
	int dawnval;

	switch (cmd)
	{
		case BACT_FOOD: /* Buy food & drink */
#ifdef JP
msg_print("�С��ƥ�Ϥ����餫�ο���ʪ�ȥӡ���򤯤줿��");
#else
			msg_print("The barkeep gives you some gruel and a beer.");
#endif

			(void)set_food(PY_FOOD_MAX - 1);
			break;

		case BACT_REST: /* Rest for the night */
			dawnval = ((turn % (TURNS_PER_TICK * TOWN_DAWN)));
			if (dawnval > (TURNS_PER_TICK * TOWN_DAWN)/4)
			{  /* nighttime */
				if ((p_ptr->poisoned) || (p_ptr->cut))
				{
#ifdef JP
msg_print("���ʤ���ɬ�פʤΤ������ǤϤʤ������żԤǤ���");
#else
					msg_print("You need a healer, not a room.");
#endif

					msg_print(NULL);
#ifdef JP
msg_print("���ߤޤ��󡢤Ǥ⤦����ï���˻�ʤ���㺤��ޤ���ǡ�");
#else
					msg_print("Sorry, but don't want anyone dying in here.");
#endif

				}
				else
				{
					int oldturn = turn;
#ifdef JP
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "�ɲ�����ޤä���");
#else
					do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "stay over night at the inn");
#endif
					turn = (turn / (TURNS_PER_TICK*TOWN_DAWN/2) + 1) * (TURNS_PER_TICK*TOWN_DAWN/2);
					if (((oldturn + TURNS_PER_TICK * TOWN_DAWN / 4) % (TURNS_PER_TICK * TOWN_DAWN)) > TURNS_PER_TICK * TOWN_DAWN/4) do_cmd_write_nikki(NIKKI_HIGAWARI, 0, NULL);
					p_ptr->chp = p_ptr->mhp;

					dungeon_turn += MIN(turn - oldturn, TURNS_PER_TICK*250);

					if (ironman_nightmare)
					{
#ifdef JP
msg_print("̲��˽����ȶ��������ʤ�����褮�ä���");
#else
						msg_print("Horrible visions flit through your mind as you sleep.");
#endif


						/* Pick a nightmare */
						get_mon_num_prep(get_nightmare, NULL);

						/* Have some nightmares */
						while(1)
						{
							have_nightmare(get_mon_num(MAX_DEPTH));

							if (!one_in_(3)) break;
						}

						/* Remove the monster restriction */
						get_mon_num_prep(NULL, NULL);

#ifdef JP
msg_print("���ʤ����䶫�����ܤ�Фޤ�����");
#else
						msg_print("You awake screaming.");
#endif

#ifdef JP
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "��̴�ˤ��ʤ���Ƥ褯̲��ʤ��ä���");
#else
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "be troubled by a nightmare.");
#endif
					}
					else
					{
						set_blind(0);
						set_confused(0);
						p_ptr->stun = 0;
						p_ptr->chp = p_ptr->mhp;
						p_ptr->csp = p_ptr->msp;
						if (p_ptr->pclass == CLASS_MAGIC_EATER)
						{
							int i;
							for (i = 0; i < 72; i++)
							{
								p_ptr->magic_num1[i] = p_ptr->magic_num2[i]*EATER_CHARGE;
							}
							for (; i < 108; i++)
							{
								p_ptr->magic_num1[i] = 0;
							}
						}

#ifdef JP
msg_print("���ʤ��ϥ�ե�å��夷���ܳФᡢ����������ޤ�����");
#else
						msg_print("You awake refreshed for the new day.");
#endif

#ifdef JP
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "������������ī��फ������");
#else
						do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "awake refreshed.");
#endif
					}

					p_ptr->leftbldg = TRUE;
				}
			}
			else
			{
#ifdef JP
msg_print("��������������Ѳ�ǽ�Ǥ���");
#else
				msg_print("The rooms are available only at night.");
#endif

				return (FALSE);
			}
			break;
		case BACT_RUMORS: /* Listen for rumors */
			{
				char Rumor[1024];

#ifdef JP
				if (!get_rnd_line_jonly("rumors_j.txt", 0, Rumor, 10))
#else
				if (!get_rnd_line("rumors.txt", 0, Rumor))
#endif

					msg_format("%s", Rumor);
				break;
			}
	}

	return (TRUE);
}


/*
 * Share gold for thieves
 */
static void share_gold(void)
{
	int i = (p_ptr->lev * 2) * 10;
#ifdef JP
msg_format("��%d �������줿��", i);
#else
	msg_format("You collect %d gold pieces", i);
#endif

	p_ptr->au += i;
}


/*
 * Display quest information
 */
static void get_questinfo(int questnum)
{
	int     i;
	int     old_quest;
	char    tmp_str[80];


	/* Clear the text */
	for (i = 0; i < 10; i++)
	{
		quest_text[i][0] = '\0';
	}

	quest_text_line = 0;

	/* Set the quest number temporary */
	old_quest = p_ptr->inside_quest;
	p_ptr->inside_quest = questnum;

	/* Get the quest text */
	init_flags = INIT_SHOW_TEXT | INIT_ASSIGN;

	process_dungeon_file("q_info.txt", 0, 0, 0, 0);

	/* Reset the old quest number */
	p_ptr->inside_quest = old_quest;

	/* Print the quest info */
#ifdef JP
sprintf(tmp_str, "�������Ⱦ��� (����: %d ������)", quest[questnum].level);
#else
	sprintf(tmp_str, "Quest Information (Danger level: %d)", quest[questnum].level);
#endif

	prt(tmp_str, 5, 0);

	prt(quest[questnum].name, 7, 0);

	for (i = 0; i < 10; i++)
	{
		c_put_str(TERM_YELLOW, quest_text[i], i + 8, 0);
	}
}


/*
 * Request a quest from the Lord.
 */
static void castle_quest(void)
{
	int             q_index = 0;
	monster_race    *r_ptr;
	quest_type      *q_ptr;
	cptr            name;


	clear_bldg(4, 18);

	/* Current quest of the building */
	q_index = cave[py][px].special;

	/* Is there a quest available at the building? */
	if (!q_index)
	{
#ifdef JP
put_str("���ΤȤ��������ȤϤ���ޤ���", 8, 0);
#else
		put_str("I don't have a quest for you at the moment.", 8, 0);
#endif

		return;
	}

	q_ptr = &quest[q_index];

	/* Quest is completed */
	if (q_ptr->status == QUEST_STATUS_COMPLETED)
	{
		/* Rewarded quest */
		q_ptr->status = QUEST_STATUS_REWARDED;

		get_questinfo(q_index);

		reinit_wilderness = TRUE;
	}
	/* Failed quest */
	else if (q_ptr->status == QUEST_STATUS_FAILED)
	{
		get_questinfo(q_index);

		/* Mark quest as done (but failed) */
		q_ptr->status = QUEST_STATUS_FAILED_DONE;

		reinit_wilderness = TRUE;
	}
	/* Quest is still unfinished */
	else if (q_ptr->status == QUEST_STATUS_TAKEN)
	{
#ifdef JP
put_str("���ʤ��ϸ��ߤΥ������Ȥ�λ�����Ƥ��ޤ���", 8, 0);
#else
		put_str("You have not completed your current quest yet!", 8, 0);
#endif

#ifdef JP
put_str("CTRL-Q��Ȥ��Х������Ȥξ��֤������å��Ǥ��ޤ���", 9, 0);
#else
		put_str("Use CTRL-Q to check the status of your quest.", 9, 0);
#endif

#ifdef JP
put_str("�������Ȥ򽪤�餻������ä���Ʋ�������", 12, 0);
#else
		put_str("Return when you have completed your quest.", 12, 0);
#endif

	}
	/* No quest yet */
	else if (q_ptr->status == QUEST_STATUS_UNTAKEN)
	{
		q_ptr->status = QUEST_STATUS_TAKEN;

		reinit_wilderness = TRUE;

		/* Assign a new quest */
		if (q_ptr->type == QUEST_TYPE_KILL_ANY_LEVEL)
		{
			if (q_ptr->r_idx == 0)
			{
				/* Random monster at least 5 - 10 levels out of deep */
				q_ptr->r_idx = get_mon_num(q_ptr->level + 4 + randint1(6));
			}

			r_ptr = &r_info[q_ptr->r_idx];

			while ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->rarity != 1))
			{
				q_ptr->r_idx = get_mon_num(q_ptr->level) + 4 + randint1(6);
				r_ptr = &r_info[q_ptr->r_idx];
			}

			if (q_ptr->max_num == 0)
			{
				/* Random monster number */
				if (randint1(10) > 7)
					q_ptr->max_num = 1;
				else
					q_ptr->max_num = randint1(3) + 1;
			}

			q_ptr->cur_num = 0;
			name = (r_name + r_ptr->name);
#ifdef JP
msg_format("��������: %s�� %d���ݤ�", name,q_ptr->max_num);
#else
			msg_format("Your quest: kill %d %s", q_ptr->max_num, name);
#endif

		}
		else
		{
			get_questinfo(q_index);
		}
	}
}


/*
 * Display town history
 */
static void town_history(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the building help file */
#ifdef JP
(void)show_file(TRUE, "jbldg.txt", NULL, 0, 0);
#else
	(void)show_file(TRUE, "bldg.txt", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Display the damage figure of an object
 * (used by compare_weapon_aux1)
 *
 * Only accurate for the current weapon, because it includes
 * the current +dam of the player.
 */
static void compare_weapon_aux2(object_type *o_ptr, int numblows,
				int r, int c, int mult, cptr attr,
				byte color)
{
	char tmp_str[80];

	/* Print the intro text */
	c_put_str(color, attr, r, c);

	/* Calculate the min and max damage figures */
#ifdef JP
sprintf(tmp_str, "��������: %d-%d ���᡼��",
#else
	sprintf(tmp_str, "Attack: %d-%d damage",
#endif

	    (numblows * (mult * o_ptr->dd /60 + o_ptr->to_d + p_ptr->to_d[0])),
	    (numblows * (mult * o_ptr->ds * o_ptr->dd /60 + o_ptr->to_d + p_ptr->to_d[0])));

	/* Print the damage */
	put_str(tmp_str, r, c + 8);
}


/*
 * Show the damage figures for the various monster types
 *
 * Only accurate for the current weapon, because it includes
 * the current number of blows for the player.
 */
static void compare_weapon_aux1(object_type *o_ptr, int col, int r)
{
	int mult = 60;
	u32b flgs[TR_FLAG_SIZE];
	int blow = p_ptr->num_blow[0];

	/* Get the flags of the weapon */
	object_flags(o_ptr, flgs);

	if (p_ptr->riding)
	{
		if ((o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
			mult = mult * (o_ptr->dd + 2) / o_ptr->dd;
	}
	if ((p_ptr->pclass != CLASS_SAMURAI) && (have_flag(flgs, TR_FORCE_WEAPON)) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5))) mult = mult * 7 / 2;

	/* Print the relevant lines */
#ifdef JP
if (have_flag(flgs, TR_FORCE_WEAPON))     compare_weapon_aux2(o_ptr, blow, r++, col, 1*mult, "����:", TERM_L_BLUE);
if (have_flag(flgs, TR_KILL_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "ưʪ:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "ưʪ:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 7*mult/2, "�ٰ�:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 2*mult, "�ٰ�:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_HUMAN))   compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "�ʹ�:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_HUMAN))   compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "�ʹ�:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "�Ի�:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "�Ի�:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_DEMON))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "����:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_DEMON))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "����:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "������:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "������:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "�ȥ��:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "�ȥ��:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "���:", TERM_YELLOW);
 else if (have_flag(flgs, TR_SLAY_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "���:", TERM_YELLOW);
if (have_flag(flgs, TR_KILL_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "ε:", TERM_YELLOW);
else if (have_flag(flgs, TR_SLAY_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "ε:", TERM_YELLOW);
if (have_flag(flgs, TR_BRAND_ACID))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "��°��:", TERM_RED);
if (have_flag(flgs, TR_BRAND_ELEC))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "��°��:", TERM_RED);
if (have_flag(flgs, TR_BRAND_FIRE))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "��°��:", TERM_RED);
if (have_flag(flgs, TR_BRAND_COLD))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "��°��:", TERM_RED);
if (have_flag(flgs, TR_BRAND_POIS))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "��°��:", TERM_RED);
#else
	if (have_flag(flgs, TR_FORCE_WEAPON))     compare_weapon_aux2(o_ptr, blow, r++, col, 1*mult, "Force  :", TERM_L_BLUE);
	if (have_flag(flgs, TR_KILL_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "Animals:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_ANIMAL)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Animals:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 7*mult/2, "Evil:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_EVIL))   compare_weapon_aux2(o_ptr, blow, r++, col, 2*mult, "Evil:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_HUMAN))   compare_weapon_aux2(o_ptr, blow, r++, col, 4*mult, "Human:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_HUMAN))   compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Human:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Undead:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_UNDEAD)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Undead:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DEMON))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Demons:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DEMON))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Demons:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Orcs:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_ORC))    compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Orcs:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Trolls:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_TROLL))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Trolls:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Giants:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_GIANT))  compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Giants:", TERM_YELLOW);
	if (have_flag(flgs, TR_KILL_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult, "Dragons:", TERM_YELLOW);
	else if (have_flag(flgs, TR_SLAY_DRAGON)) compare_weapon_aux2(o_ptr, blow, r++, col, 3*mult, "Dragons:", TERM_YELLOW);
	if (have_flag(flgs, TR_BRAND_ACID))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Acid:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_ELEC))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Elec:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_FIRE))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Fire:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_COLD))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Cold:", TERM_RED);
	if (have_flag(flgs, TR_BRAND_POIS))  compare_weapon_aux2(o_ptr, blow, r++, col, 5*mult/2, "Poison:", TERM_RED);
#endif

}

static int hit_chance(int to_h, int ac)
{
	int chance = 0;
	int meichuu = p_ptr->skill_thn + (p_ptr->to_h[0] + to_h) * BTH_PLUS_ADJ;

	if (meichuu <= 0) return 5;

	chance = 100 - ((ac * 75) / meichuu);

	if (chance > 95) chance = 95;
	if (chance < 5) chance = 5;
	if (p_ptr->pseikaku == SEIKAKU_NAMAKE)
		chance = (chance*19+9)/20;
	return chance;
}

/*
 * Displays all info about a weapon
 *
 * Only accurate for the current weapon, because it includes
 * various info about the player's +to_dam and number of blows.
 */
static void list_weapon(object_type *o_ptr, int row, int col)
{
	char o_name[MAX_NLEN];
	char tmp_str[80];

	/* Print the weapon name */
	object_desc(o_name, o_ptr, TRUE, 0);
	c_put_str(TERM_YELLOW, o_name, row, col);

	/* Print the player's number of blows */
#ifdef JP
sprintf(tmp_str, "������: %d", p_ptr->num_blow[0]);
#else
	sprintf(tmp_str, "Number of Blows: %d", p_ptr->num_blow[0]);
#endif

	put_str(tmp_str, row+1, col);

	/* Print to_hit and to_dam of the weapon */
#ifdef JP
sprintf(tmp_str, "̿��Ψ:  0  50 100 150 200 (Ũ��AC)");
#else
sprintf(tmp_str, "To Hit:  0  50 100 150 200 (AC)");
#endif

	put_str(tmp_str, row+2, col);

	/* Print the weapons base damage dice */
#ifdef JP
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#else
sprintf(tmp_str, "        %2d  %2d  %2d  %2d  %2d (%%)", hit_chance(o_ptr->to_h, 0), hit_chance(o_ptr->to_h, 50), hit_chance(o_ptr->to_h, 100), hit_chance(o_ptr->to_h, 150), hit_chance(o_ptr->to_h, 200));
#endif

	put_str(tmp_str, row+3, col);

#ifdef JP
c_put_str(TERM_YELLOW, "��ǽ�ʥ��᡼��:", row+5, col);
#else
	c_put_str(TERM_YELLOW, "Possible Damage:", row+5, col);
#endif


	/* Damage for one blow (if it hits) */
#ifdef JP
sprintf(tmp_str, "������ˤĤ� %d-%d",
#else
	sprintf(tmp_str, "One Strike: %d-%d damage",
#endif

	    o_ptr->dd + o_ptr->to_d + p_ptr->to_d[0],
	    o_ptr->ds * o_ptr->dd + o_ptr->to_d + p_ptr->to_d[0]);
	put_str(tmp_str, row+6, col+1);

	/* Damage for the complete attack (if all blows hit) */
#ifdef JP
sprintf(tmp_str, "��������ˤĤ� %d-%d",
#else
	sprintf(tmp_str, "One Attack: %d-%d damage",
#endif

	    p_ptr->num_blow[0] * (o_ptr->dd + o_ptr->to_d + p_ptr->to_d[0]),
	    p_ptr->num_blow[0] * (o_ptr->ds * o_ptr->dd + o_ptr->to_d + p_ptr->to_d[0]));
	put_str(tmp_str, row+7, col+1);
}


/*
 * Hook to specify "weapon"
 */
static bool item_tester_hook_melee_weapon(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		{
			return (TRUE);
		}
		case TV_SWORD:
		{
			if (o_ptr->sval != SV_DOKUBARI) return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Hook to specify "ammo"
 */
static bool item_tester_hook_ammo(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Compare weapons
 *
 * Copies the weapons to compare into the weapon-slot and
 * compares the values for both weapons.
 */
static bool compare_weapons(void)
{
	int item, item2;
	object_type *o1_ptr, *o2_ptr;
	object_type orig_weapon;
	object_type *i_ptr;
	cptr q, s;
	int row = 2;
	bool old_character_xtra = character_xtra;

	screen_save();
	/* Clear the screen */
	clear_bldg(0, 22);

	/* Store copy of original wielded weapon */
	i_ptr = &inventory[INVEN_RARM];
	object_copy(&orig_weapon, i_ptr);

	item_tester_no_ryoute = TRUE;
	/* Only compare melee weapons */
	item_tester_hook = item_tester_hook_melee_weapon;

	/* Get the first weapon */
#ifdef JP
q = "�������ϡ�";
s = "��٤��Τ�����ޤ���";
#else
	q = "What is your first weapon? ";
	s = "You have nothing to compare.";
#endif

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN)))
	{
		screen_load();
		return (FALSE);
	}

	/* Get the item (in the pack) */
	o1_ptr = &inventory[item];

	/* Clear the screen */
	clear_bldg(0, 22);

	item_tester_no_ryoute = TRUE;
	/* Only compare melee weapons */
	item_tester_hook = item_tester_hook_melee_weapon;

	/* Get the second weapon */
#ifdef JP
q = "��������ϡ�";
s = "��٤��Τ�����ޤ���";
#else
	q = "What is your second weapon? ";
	s = "You have nothing to compare.";
#endif

	if (!get_item(&item2, q, s, (USE_EQUIP | USE_INVEN)))
	{
		screen_load();
		return (FALSE);
	}

	/* Get the item (in the pack) */
	o2_ptr = &inventory[item2];

	/* Clear the screen */
	clear_bldg(0, 22);

	/* Copy first weapon into the weapon slot (if it's not already there) */
	if (o1_ptr != i_ptr)
		object_copy(i_ptr, o1_ptr);

	/* Hack -- prevent "icky" message */
	character_xtra = TRUE;

	/* Get the new values */
	calc_bonuses();

	character_xtra = old_character_xtra;

	/* List the new values */
	list_weapon(o1_ptr, row, 2);
	compare_weapon_aux1(o1_ptr, 2, row + 8);

	/* Copy second weapon into the weapon slot (if it's not already there) */
	if (o2_ptr != i_ptr)
		object_copy(i_ptr, o2_ptr);
	else
		object_copy(i_ptr, &orig_weapon);

	/* Hack -- prevent "icky" message */
	character_xtra = TRUE;

	/* Get the new values */
	calc_bonuses();

	character_xtra = old_character_xtra;

	/* List the new values */
	list_weapon(o2_ptr, row, 40);
	compare_weapon_aux1(o2_ptr, 40, row + 8);

	/* Copy back the original weapon into the weapon slot */
	object_copy(i_ptr, &orig_weapon);

	/* Reset the values for the old weapon */
	calc_bonuses();

#ifdef JP
put_str("(���ֹ⤤���᡼����Ŭ�Ѥ���ޤ���ʣ�������Ǹ��̤�­��������ޤ���)", row + 4, 0);
#else
	put_str("(Only highest damage applies per monster. Special damage not cumulative.)", row + 4, 0);
#endif

#ifdef JP
msg_print("���ߤε��̤���Ƚ�Ǥ���ȡ����ʤ������ϰʲ��Τ褦�ʰ��Ϥ�ȯ�����ޤ�:");
#else
	msg_print("Based on your current abilities, here is what your weapons will do");
#endif


	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Evaluate AC
 *
 * AC�������Ψ�����᡼������Ψ��׻���ɽ������
 * Calculate and display the dodge-rate and the protection-rate
 * based on AC
 */
static bool eval_ac(int iAC)
{
#ifdef JP
	const char memo[] =
		"���᡼���ڸ�Ψ�Ȥϡ�Ũ�ι��⤬�����ä������Υ��᡼����\n"
		"���ѡ�����ȷڸ����뤫�򼨤��ޤ���\n"
		"���᡼���ڸ����̾��ľ�ܹ���(���ब�ֹ��⤹��פȡ�ʴ�դ���פ�ʪ)\n"
		"���Ф��ƤΤ߸��̤�����ޤ���\n \n"
		"Ũ�Υ�٥�Ȥϡ�����Ũ���̾ﲿ���˸���뤫�򼨤��ޤ���\n \n"
		"����Ψ��Ũ��ľ�ܹ���򲿥ѡ�����Ȥγ�Ψ���򤱤뤫�򼨤���\n"
		"Ũ�Υ�٥�Ȥ��ʤ���AC�ˤ�äƷ��ꤵ��ޤ���\n \n"
		"���᡼�������ͤȤϡ�Ũ�Σ������ݥ���Ȥ��̾ﹶ����Ф���\n"
		"����Ψ�ȥ��᡼���ڸ�Ψ���θ�������᡼���δ����ͤ򼨤��ޤ���\n";
#else
	const char memo[] =
		"'Protection Rate' means how much damage is reduced by your armor.\n"
		"Note that the Protection rate is effective only against normal "
		"'attack' and 'shatter' type melee attacks, "
		"and has no effect against any other types such as 'poison'.\n \n"
		"'Dodge Rate' indicates the success rate on dodging the "
		"monster's melee attacks.  "
		"It is depend on the level of the monster and your AC.\n \n"
		"'Average Damage' indicates the expected amount of damage "
		"when you are attacked by normal melee attacks with power=100.";
#endif

	int protection;
	int col, row = 2;
	int lvl, i;
	char buf[80*20], *t;

	/* AC lower than zero has no effect */
	if (iAC < 0) iAC = 0;

	/* ���᡼���ڸ�Ψ��׻� */
	protection = 100 * MIN(iAC, 150) / 250;

	screen_save();
	clear_bldg(0, 22);

#ifdef JP
	put_str(format("���ʤ��θ��ߤ�AC: %3d", iAC), row++, 0);
	put_str(format("���᡼���ڸ�Ψ  : %3d%%", protection), row++, 0);
	row++;

	put_str("Ũ�Υ�٥�      :", row + 0, 0);
	put_str("����Ψ          :", row + 1, 0);
	put_str("���᡼��������  :", row + 2, 0);
#else
	put_str(format("Your current AC : %3d", iAC), row++, 0);
	put_str(format("Protection rate : %3d%%", protection), row++, 0);
	row++;

	put_str("Level of Monster:", row + 0, 0);
	put_str("Dodge Rate      :", row + 1, 0);
	put_str("Average Damage  :", row + 2, 0);
#endif
    
	for (col = 17 + 1, lvl = 0; lvl <= 100; lvl += 10, col += 5)
	{
		int quality = 60 + lvl * 3; /* attack quality with power 60 */
		int dodge;   /* ����Ψ(%) */
		int average; /* ���᡼�������� */

		put_str(format("%3d", lvl), row + 0, col);

		/* ����Ψ��׻� */
		dodge = 5 + (MIN(100, 100 * (iAC * 3 / 4) / quality) * 9 + 5) / 10;
		put_str(format("%3d%%", dodge), row + 1, col);

		/* 100���ι�����Ф��ƤΥ��᡼�������ͤ�׻� */
		average = (100 - dodge) * (100 - protection) / 100;
		put_str(format("%3d", average), row + 2, col);
	}

	/* Display note */
	roff_to_buf(memo, 70, buf, sizeof(buf));
	for (t = buf; t[0]; t += strlen(t) + 1)
		put_str(t, (row++) + 4, 4);

#ifdef JP
	prt("���ߤΤ��ʤ����������餹��ȡ����ʤ����ɸ��Ϥ�"
		   "���줯�餤�Ǥ�:", 0, 0);
#else
	prt("Defense abilities from your current Armor Class are evaluated below.", 0, 0);
#endif
  
	flush();
	(void)inkey();
	screen_load();

	/* Done */
	return (TRUE);
}


/*
 * Enchant item
 */
static bool enchant_item(int cost, int to_hit, int to_dam, int to_ac)
{
	int         i, item;
	bool        okay = FALSE;
	object_type *o_ptr;
	cptr        q, s;
	int         maxenchant = (p_ptr->lev / 5);
	char        tmp_str[MAX_NLEN];


	clear_bldg(4, 18);
#ifdef JP
prt(format("���ߤΤ��ʤ��ε��̤��ȡ�+%d �ޤǲ��ɤǤ��ޤ���", maxenchant), 5, 0);
prt(format(" ���ɤ�����ϰ�ĤˤĤ���%d �Ǥ���", cost), 7, 0);
#else
	prt(format("  Based on your skill, we can improve up to +%d.", maxenchant), 5, 0);
	prt(format("  The price for the service is %d gold per item.", cost), 7, 0);
#endif

	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
q = "�ɤΥ����ƥ����ɤ��ޤ�����";
s = "���ɤǤ����Τ�����ޤ���";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
#endif

	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP))) return (FALSE);

	/* Get the item (in the pack) */
	o_ptr = &inventory[item];

	/* Check if the player has enough money */
	if (p_ptr->au < (cost * o_ptr->number))
	{
		object_desc(tmp_str, o_ptr, TRUE, 0);
#ifdef JP
msg_format("%s����ɤ�������Υ�����ɤ�����ޤ���", tmp_str);
#else
		msg_format("You do not have the gold to improve %s!", tmp_str);
#endif

		return (FALSE);
	}

	/* Enchant to hit */
	for (i = 0; i < to_hit; i++)
	{
		if (o_ptr->to_h < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOHIT | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to damage */
	for (i = 0; i < to_dam; i++)
	{
		if (o_ptr->to_d < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TODAM | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Enchant to AC */
	for (i = 0; i < to_ac; i++)
	{
		if (o_ptr->to_a < maxenchant)
		{
			if (enchant(o_ptr, 1, (ENCH_TOAC | ENCH_FORCE)))
			{
				okay = TRUE;
				break;
			}
		}
	}

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
#ifdef JP
msg_print("���ɤ˼��Ԥ�����");
#else
		msg_print("The improvement failed.");
#endif


		return (FALSE);
	}
	else
	{
		object_desc(tmp_str, o_ptr, TRUE, 1);
#ifdef JP
msg_format("��%d ��%s����ɤ��ޤ�����", cost * o_ptr->number, tmp_str );
#else
		msg_format("Improved %s for %d gold.", tmp_str, cost * o_ptr->number);
#endif

		/* Charge the money */
		p_ptr->au -= (cost * o_ptr->number);

		if (item >= INVEN_RARM) calc_android_exp();

		/* Something happened */
		return (TRUE);
	}
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
static void building_recharge(void)
{
	int         item, lev;
	object_type *o_ptr;
	object_kind *k_ptr;
	cptr        q, s;
	int         price;
	int         charges;
	int         max_charges;
	char        tmp_str[MAX_NLEN];

	msg_flag = FALSE;

	/* Display some info */
	clear_bldg(4, 18);
#ifdef JP
prt("  �ƽ�Ŷ�����Ѥϥ����ƥ�μ���ˤ��ޤ���", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif


	/* Only accept legal items */
	item_tester_hook = item_tester_hook_recharge;

	/* Get an item */
#ifdef JP
q = "�ɤΥ����ƥ�����Ϥ�Ŷ���ޤ���? ";
s = "���Ϥ�Ŷ���٤������ƥब�ʤ���";
#else
	q = "Recharge which item? ";
	s = "You have nothing to recharge.";
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

	k_ptr = &k_info[o_ptr->k_idx];

	/*
	 * We don't want to give the player free info about
	 * the level of the item or the number of charges.
	 */
	/* The item must be "known" */
	if (!object_known_p(o_ptr))
	{
#ifdef JP
msg_format("��Ŷ�������˴��ꤵ��Ƥ���ɬ�פ�����ޤ���");
#else
		msg_format("The item must be identified first!");
#endif

		msg_print(NULL);

		if ((p_ptr->au >= 50) &&
#ifdef JP
get_check("��50�Ǵ��ꤷ�ޤ����� "))
#else
			get_check("Identify for 50 gold? "))
#endif

		{
			/* Pay the price */
			p_ptr->au -= 50;

			/* Identify it */
			identify_item(o_ptr);

			/* Description */
			object_desc(tmp_str, o_ptr, TRUE, 3);

#ifdef JP
msg_format("%s �Ǥ���", tmp_str);
#else
			msg_format("You have: %s.", tmp_str);
#endif


			/* Update the gold display */
			building_prt_gold();
		}
		else
		{
			return;
		}
	}

	/* Extract the object "level" */
	lev = get_object_level(o_ptr);

	/* Price for a rod */
	if (o_ptr->tval == TV_ROD)
	{
		if (o_ptr->timeout > 0)
		{
			/* Fully recharge */
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
		}
		else
		{
			/* No recharge necessary */
			price = 0;
#ifdef JP
msg_format("����Ϻƽ�Ŷ����ɬ�פϤ���ޤ���");
#else
			msg_format("That doesn't need to be recharged.");
#endif

			return;
		}
	}
	else if (o_ptr->tval == TV_STAFF)
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (get_object_cost(o_ptr) / 10) * o_ptr->number;

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}
	else
	{
		/* Price per charge ( = double the price paid by shopkeepers for the charge) */
		price = (get_object_cost(o_ptr) / 10);

		/* Pay at least 10 gold per charge */
		price = MAX(10, price);
	}

	/* Limit the number of charges for wands and staffs */
	if (o_ptr->tval == TV_WAND
		&& (o_ptr->pval / o_ptr->number >= k_ptr->pval))
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("������ˡ���Ϥ⤦��ʬ�˽�Ŷ����Ƥ��ޤ���");
#else
			msg_print("These wands are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("������ˡ���Ϥ⤦��ʬ�˽�Ŷ����Ƥ��ޤ���");
#else
			msg_print("This wand is already fully charged.");
#endif
		}
		return;
	}
	else if (o_ptr->tval == TV_STAFF && o_ptr->pval >= k_ptr->pval)
	{
		if (o_ptr->number > 1)
		{
#ifdef JP
msg_print("���ξ�Ϥ⤦��ʬ�˽�Ŷ����Ƥ��ޤ���");
#else
			msg_print("These staffs are already fully charged.");
#endif
		}
		else
		{
#ifdef JP
msg_print("���ξ�Ϥ⤦��ʬ�˽�Ŷ����Ƥ��ޤ���");
#else
			msg_print("This staff is already fully charged.");
#endif
		}
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au < price)
	{
		object_desc(tmp_str, o_ptr, TRUE, 0);
#ifdef JP
msg_format("%s��ƽ�Ŷ����ˤϡ�%d ɬ�פǤ���", tmp_str,price );
#else
		msg_format("You need %d gold to recharge %s!", price, tmp_str);
#endif

		return;
	}

	if (o_ptr->tval == TV_ROD)
	{
#ifdef JP
if (get_check(format("���Υ�åɤ��%d �Ǻƽ�Ŷ���ޤ�����",
 price)))
#else
		if (get_check(format("Recharge the %s for %d gold? ",
			((o_ptr->number > 1) ? "rods" : "rod"), price)))
#endif

		{
			/* Recharge fully */
			o_ptr->timeout = 0;
		}
		else
		{
			return;
		}
	}
	else
	{
		if (o_ptr->tval == TV_STAFF)
			max_charges = k_ptr->pval - o_ptr->pval;
		else
			max_charges = o_ptr->number * k_ptr->pval - o_ptr->pval;

		/* Get the quantity for staves and wands */
#ifdef JP
charges = get_quantity(format("���ʬ��%d �ǲ���ʬ��Ŷ���ޤ�����",
#else
		charges = get_quantity(format("Add how many charges for %d gold? ",
#endif

			      price), MIN(p_ptr->au / price, max_charges));

		/* Do nothing */
		if (charges < 1) return;

		/* Get the new price */
		price *= charges;

		/* Recharge */
		o_ptr->pval += charges;

		/* We no longer think the item is empty */
		o_ptr->ident &= ~(IDENT_EMPTY);
	}

	/* Give feedback */
	object_desc(tmp_str, o_ptr, TRUE, 3);
#ifdef JP
msg_format("%s���%d �Ǻƽ�Ŷ���ޤ�����", tmp_str, price);
#else
	msg_format("%^s %s recharged for %d gold.", tmp_str, ((o_ptr->number > 1) ? "were" : "was"), price);
#endif

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au -= price;

	/* Finished */
	return;
}


/*
 * Recharge rods, wands and staves
 *
 * The player can select the number of charges to add
 * (up to a limit), and the recharge never fails.
 *
 * The cost for rods depends on the level of the rod. The prices
 * for recharging wands and staves are dependent on the cost of
 * the base-item.
 */
static void building_recharge_all(void)
{
	int         i;
	int         lev;
	object_type *o_ptr;
	object_kind *k_ptr;
	int         price = 0;
	int         total_cost = 0;


	/* Display some info */
	msg_flag = FALSE;
	clear_bldg(4, 18);
#ifdef JP
	prt("  �ƽ�Ŷ�����Ѥϥ����ƥ�μ���ˤ��ޤ���", 6, 0);
#else
	prt("  The prices of recharge depend on the type.", 6, 0);
#endif

	/* Calculate cost */
	for ( i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
				
		/* skip non magic device */
		if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		/* need identified */
		if (!object_known_p(o_ptr)) total_cost += 50;

		/* Extract the object "level" */
		lev = get_object_level(o_ptr);

		k_ptr = &k_info[o_ptr->k_idx];

		switch (o_ptr->tval)
		{
		case TV_ROD:
			price = (lev * 50 * o_ptr->timeout) / k_ptr->pval;
			break;

		case TV_STAFF:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (get_object_cost(o_ptr) / 10) * o_ptr->number;

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (k_ptr->pval - o_ptr->pval) * price;
			break;

		case TV_WAND:
			/* Price per charge ( = double the price paid by shopkeepers for the charge) */
			price = (get_object_cost(o_ptr) / 10);

			/* Pay at least 10 gold per charge */
			price = MAX(10, price);

			/* Fully charge */
			price = (o_ptr->number * k_ptr->pval - o_ptr->pval) * price;
			break;
		}

		/* if price <= 0 then item have enough charge */
		if (price > 0) total_cost += price;
	}

	if (!total_cost)
	{
#ifdef JP
		msg_print("��Ŷ����ɬ�פϤ���ޤ���");
#else
		msg_print("No need to recharge.");
#endif

		msg_print(NULL);
		return;
	}

	/* Check if the player has enough money */
	if (p_ptr->au < total_cost)
	{
#ifdef JP
		msg_format("���٤ƤΥ����ƥ��ƽ�Ŷ����ˤϡ�%d ɬ�פǤ���", total_cost );
#else
		msg_format("You need %d gold to recharge all items!",total_cost);
#endif

		msg_print(NULL);
		return;
	}

#ifdef JP
	if (!get_check(format("���٤ƤΥ����ƥ�� ��%d �Ǻƽ�Ŷ���ޤ�����",  total_cost))) return;
#else
	if (!get_check(format("Recharge all items for %d gold? ", total_cost))) return;
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		o_ptr = &inventory[i];
		k_ptr = &k_info[o_ptr->k_idx];
				
		/* skip non magic device */
		if (o_ptr->tval < TV_STAFF || o_ptr->tval > TV_ROD) continue;

		/* Identify it */
		if (!object_known_p(o_ptr)) identify_item(o_ptr);

		/* Recharge */
		switch (o_ptr->tval)
		{
		case TV_ROD:
			o_ptr->timeout = 0;
			break;
		case TV_STAFF:
			if (o_ptr->pval < k_ptr->pval) o_ptr->pval = k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		case TV_WAND:
			if (o_ptr->pval < o_ptr->number * k_ptr->pval)
				o_ptr->pval = o_ptr->number * k_ptr->pval;
			/* We no longer think the item is empty */
			o_ptr->ident &= ~(IDENT_EMPTY);
			break;
		}
	}

	/* Give feedback */
#ifdef JP
	msg_format("��%d �Ǻƽ�Ŷ���ޤ�����", total_cost);
#else
	msg_format("You pay %d gold.", total_cost);
#endif

	msg_print(NULL);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Pay the price */
	p_ptr->au -= total_cost;

	/* Finished */
	return;
}


bool tele_town(void)
{
	int i, x, y;
	int num = 0;

	if (dun_level)
	{
#ifdef JP
		msg_print("������ˡ���Ͼ�Ǥ����Ȥ��ʤ���");
#else
		msg_print("This spell can only be used on the surface!");
#endif
		return FALSE;
	}

	if (p_ptr->inside_arena || p_ptr->inside_battle)
	{
#ifdef JP
		msg_print("������ˡ�ϳ��Ǥ����Ȥ��ʤ���");
#else
		msg_print("This spell can only be used outside!");
#endif
		return FALSE;
	}

	screen_save();
	clear_bldg(4, 10);

	for (i=1;i<max_towns;i++)
	{
		char buf[80];

		if ((i == NO_TOWN) || (i == SECRET_TOWN) || (i == p_ptr->town_num) || !(p_ptr->visit & (1L << (i-1)))) continue;

		sprintf(buf,"%c) %-20s", I2A(i-1), town[i].name);
		prt(buf, 5+i, 5);
		num++;
	}

	if (!num)
	{
#ifdef JP
		msg_print("�ޤ��Ԥ���Ȥ����ʤ���");
#else
		msg_print("You have not yet visited any town.");
#endif

		msg_print(NULL);
		screen_load();
		return FALSE;
	}

#ifdef JP
	prt("�ɤ��˹Ԥ��ޤ���:", 0, 0);
#else
	prt("Which town you go: ", 0, 0);
#endif
	while(1)
	{
		i = inkey();

		if (i == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((i < 'a') || (i > ('a'+max_towns-2))) continue;
		else if (((i-'a'+1) == p_ptr->town_num) || ((i-'a'+1) == NO_TOWN) || ((i-'a'+1) == SECRET_TOWN) || !(p_ptr->visit & (1L << (i-'a')))) continue;
		break;
	}

	for (y = 0; y < max_wild_y; y++)
	{
		for (x = 0; x < max_wild_x; x++)
		{
			if(wilderness[y][x].town == (i-'a'+1))
			{
				p_ptr->wilderness_y = y;
				p_ptr->wilderness_x = x;
			}
		}
	}
	p_ptr->leftbldg = TRUE;
	p_ptr->leaving = TRUE;
	leave_bldg = TRUE;
	p_ptr->teleport_town = TRUE;
	screen_load();
	return TRUE;
}


/*
 * Execute a building command
 */
static void bldg_process_command(building_type *bldg, int i)
{
	int bact = bldg->actions[i];
	int bcost;
	bool paid = FALSE;
	bool set_reward = FALSE;
	int amt;

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	if (is_owner(bldg))
		bcost = bldg->member_costs[i];
	else
		bcost = bldg->other_costs[i];

	/* action restrictions */
	if (((bldg->action_restr[i] == 1) && !is_member(bldg)) ||
	    ((bldg->action_restr[i] == 2) && !is_owner(bldg)))
	{
#ifdef JP
msg_print("��������򤹤븢���Ϥ���ޤ���");
#else
		msg_print("You have no right to choose that!");
#endif
		return;
	}

	/* check gold (HACK - Recharge uses variable costs) */
	if ((bact != BACT_RECHARGE) &&
	    (((bldg->member_costs[i] > p_ptr->au) && is_owner(bldg)) ||
	     ((bldg->other_costs[i] > p_ptr->au) && !is_owner(bldg))))
	{
#ifdef JP
msg_print("���⤬­��ޤ���");
#else
		msg_print("You do not have the gold!");
#endif
		return;
	}

	if (!bcost) set_reward = TRUE;

	switch (bact)
	{
	case BACT_NOTHING:
				/* Do nothing */
		break;
	case BACT_RESEARCH_ITEM:
		paid = identify_fully(FALSE);
		break;
	case BACT_TOWN_HISTORY:
		town_history();
		break;
	case BACT_RACE_LEGENDS:
		race_legends();
		break;
	case BACT_QUEST:
		castle_quest();
		break;
	case BACT_KING_LEGENDS:
	case BACT_ARENA_LEGENDS:
	case BACT_LEGENDS:
		show_highclass(building_loc);
		break;
	case BACT_POSTER:
	case BACT_ARENA_RULES:
	case BACT_ARENA:
		arena_comm(bact);
		break;
	case BACT_IN_BETWEEN:
	case BACT_CRAPS:
	case BACT_SPIN_WHEEL:
	case BACT_DICE_SLOTS:
	case BACT_GAMBLE_RULES:
	case BACT_POKER:
		gamble_comm(bact);
		break;
	case BACT_REST:
	case BACT_RUMORS:
	case BACT_FOOD:
		paid = inn_comm(bact);
		break;
	case BACT_RESEARCH_MONSTER:
		paid = research_mon();
		break;
	case BACT_COMPARE_WEAPONS:
		paid = compare_weapons();
		break;
	case BACT_ENCHANT_WEAPON:
		item_tester_hook = item_tester_hook_melee_weapon;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_ARMOR:
		item_tester_hook = item_tester_hook_armour;
		enchant_item(bcost, 0, 0, 1);
		break;
	case BACT_RECHARGE:
		building_recharge();
		break;
	case BACT_RECHARGE_ALL:
		building_recharge_all();
		break;
	case BACT_IDENTS: /* needs work */
#ifdef JP
		if (!get_check("����ʪ�����ƴ��ꤷ�Ƥ�����Ǥ�����")) break;
		identify_pack();
		msg_print(" ����ʪ���Ƥ����ꤵ��ޤ�����");
#else
		if (!get_check("Do you pay for identify all your possession? ")) break;
		identify_pack();
		msg_print("Your possessions have been identified.");
#endif

		paid = TRUE;
		break;
	case BACT_IDENT_ONE: /* needs work */
		paid = ident_spell(FALSE);
		break;
	case BACT_LEARN:
		do_cmd_study();
		break;
	case BACT_HEALING: /* needs work */
		hp_player(200);
		set_poisoned(0);
		set_blind(0);
		set_confused(0);
		set_cut(0);
		set_stun(0);
		paid = TRUE;
		break;
	case BACT_RESTORE: /* needs work */
		if (do_res_stat(A_STR)) paid = TRUE;
		if (do_res_stat(A_INT)) paid = TRUE;
		if (do_res_stat(A_WIS)) paid = TRUE;
		if (do_res_stat(A_DEX)) paid = TRUE;
		if (do_res_stat(A_CON)) paid = TRUE;
		if (do_res_stat(A_CHR)) paid = TRUE;
		break;
	case BACT_GOLD: /* set timed reward flag */
		if (!p_ptr->rewards[BACT_GOLD])
		{
			share_gold();
			p_ptr->rewards[BACT_GOLD] = TRUE;
		}
		else
		{
#ifdef JP
			msg_print("������ʬ�����Ϥ��Ǥ˻�ʧ�ä�����");
#else
			msg_print("You just had your daily allowance!");
#endif
		}
		break;
	case BACT_ENCHANT_ARROWS:
		item_tester_hook = item_tester_hook_ammo;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_ENCHANT_BOW:
		item_tester_tval = TV_BOW;
		enchant_item(bcost, 1, 1, 0);
		break;
	case BACT_RECALL:
		if (recall_player(1)) paid = TRUE;
		break;
	case BACT_TELEPORT_LEVEL:
	{
		int select_dungeon;
		int i, num = 0;
		s16b *dun;
		int max_depth;

		/* Allocate the "dun" array */
		C_MAKE(dun, max_d_idx, s16b);

		screen_save();
		clear_bldg(4, 20);

		for(i = 1; i < max_d_idx; i++)
		{
			char buf[80];
			bool seiha = FALSE;

			if (!d_info[i].maxdepth) continue;
			if (!max_dlv[i]) continue;
			if (d_info[i].final_guardian)
			{
				if (!r_info[d_info[i].final_guardian].max_num) seiha = TRUE;
			}
			else if (max_dlv[i] == d_info[i].maxdepth) seiha = TRUE;

#ifdef JP
			sprintf(buf,"%c) %c%-12s : ���� %d ��", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#else
			sprintf(buf,"%c) %c%-12s : Max level %d", 'a'+num, seiha ? '!' : ' ', d_name + d_info[i].name, max_dlv[i]);
#endif
			put_str(buf, 4+num, 5);
			dun[num] = i;
			num++;
		}
#ifdef JP
		prt("�ɤΥ��󥸥��˥ƥ�ݡ��Ȥ��ޤ���:", 0, 0);
#else
		prt("Which dungeon do you teleport?: ", 0, 0);
#endif
		while(1)
		{
			i = inkey();

			if (i == ESCAPE)
			{
				/* Free the "dun" array */
				C_KILL(dun, max_d_idx, s16b);

				screen_load();
				return;
			}
			if (i >= 'a' && i <('a'+num))
			{
				select_dungeon = dun[i-'a'];
				break;
			}
			else bell();
		}
		screen_load();

		/* Free the "dun" array */
		C_KILL(dun, max_d_idx, s16b);

		max_depth = d_info[select_dungeon].maxdepth;

		/* Limit depth in Angband */
		if (select_dungeon == DUNGEON_ANGBAND)
		{
			if (quest[QUEST_OBERON].status != QUEST_STATUS_FINISHED) max_depth = 98;
			else if(quest[QUEST_SERPENT].status != QUEST_STATUS_FINISHED) max_depth = 99;
		}

#ifdef JP
		amt = get_quantity(format("%s�β����˥ƥ�ݡ��Ȥ��ޤ�����", d_name + d_info[select_dungeon].name), max_depth);
#else
		amt = get_quantity(format("Teleport to which level of %s? ", d_name + d_info[select_dungeon].name), max_depth);
#endif

		if (amt > 0)
		{
			p_ptr->word_recall = 1;
			p_ptr->recall_dungeon = select_dungeon;
			max_dlv[p_ptr->recall_dungeon] = ((amt > d_info[select_dungeon].maxdepth) ? d_info[select_dungeon].maxdepth : ((amt < d_info[select_dungeon].mindepth) ? d_info[select_dungeon].mindepth : amt));
			if (record_maxdeapth)
#ifdef JP
				do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "�ȥ��ץ����");
#else
			do_cmd_write_nikki(NIKKI_TRUMP, select_dungeon, "at Trump Tower");
#endif
#ifdef JP
			msg_print("�����絤��ĥ��Ĥ�Ƥ���...");
#else
			msg_print("The air about you becomes charged...");
#endif

			paid = TRUE;
			p_ptr->redraw |= (PR_STATUS);
		}
		break;
	}
	case BACT_LOSE_MUTATION:
		paid = lose_mutation(0);
		/* ToDo: Better message text. */
		if (!paid)
#ifdef JP
			msg_print("��̯�ʤ��餤���̤ˤʤä��������롣");
#else
		msg_print("You feel oddly normal.");
#endif


		break;
	case BACT_BATTLE:
		kakutoujou();
		break;
	case BACT_TSUCHINOKO:
		tsuchinoko();
		break;
	case BACT_KUBI:
		shoukinkubi();
		break;
	case BACT_TARGET:
		today_target();
		break;
	case BACT_KANKIN:
		kankin();
		break;
	case BACT_HEIKOUKA:
#ifdef JP
		msg_print("ʿ�ղ��ε�����Ԥʤä���");
#else
		msg_print("You received an equalization ritual.");
#endif
		set_virtue(V_COMPASSION, 0);
		set_virtue(V_HONOUR, 0);
		set_virtue(V_JUSTICE, 0);
		set_virtue(V_SACRIFICE, 0);
		set_virtue(V_KNOWLEDGE, 0);
		set_virtue(V_FAITH, 0);
		set_virtue(V_ENLIGHTEN, 0);
		set_virtue(V_ENCHANT, 0);
		set_virtue(V_CHANCE, 0);
		set_virtue(V_NATURE, 0);
		set_virtue(V_HARMONY, 0);
		set_virtue(V_VITALITY, 0);
		set_virtue(V_UNLIFE, 0);
		set_virtue(V_PATIENCE, 0);
		set_virtue(V_TEMPERANCE, 0);
		set_virtue(V_DILIGENCE, 0);
		set_virtue(V_VALOUR, 0);
		set_virtue(V_INDIVIDUALISM, 0);
		get_virtues();
		paid = TRUE;
		break;
	case BACT_TELE_TOWN:
		paid = tele_town();
		break;
	case BACT_EVAL_AC:
		paid = eval_ac(p_ptr->dis_ac + p_ptr->dis_to_a);
		break;
	}

	if (paid)
	{
		p_ptr->au -= bcost;
	}
}


/*
 * Enter quest level
 */
void do_cmd_quest(void)
{
	energy_use = 100;

	if (cave[py][px].feat != FEAT_QUEST_ENTER)
	{
#ifdef JP
msg_print("�����ˤϥ������Ȥ������Ϥʤ���");
#else
		msg_print("You see no quest level here.");
#endif

		return;
	}
	else
	{
#ifdef JP
		msg_print("�����ˤϥ������Ȥؤ�����������ޤ���");
		if (!get_check("�������Ȥ�����ޤ�����")) return;
		if ((p_ptr->pseikaku == SEIKAKU_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
			msg_print("�ؤȤˤ������äƤߤ褦��������");
#else
		msg_print("There is an entry of a quest.");
		if (!get_check("Do you enter? ")) return;
#endif

		/* Player enters a new quest */
		p_ptr->oldpy = 0;
		p_ptr->oldpx = 0;

		leave_quest_check();

		p_ptr->inside_quest = cave[py][px].special;
		if(quest[leaving_quest].type != QUEST_TYPE_RANDOM) dun_level = 1;
		p_ptr->leftbldg = TRUE;
		p_ptr->leaving = TRUE;
	}
}


/*
 * Do building commands
 */
void do_cmd_bldg(void)
{
	int             i, which;
	char            command;
	bool            validcmd;
	building_type   *bldg;


	energy_use = 100;

	if (!((cave[py][px].feat >= FEAT_BLDG_HEAD) &&
		  (cave[py][px].feat <= FEAT_BLDG_TAIL)))
	{
#ifdef JP
msg_print("�����ˤϷ�ʪ�Ϥʤ���");
#else
		msg_print("You see no building here.");
#endif

		return;
	}

	which = (cave[py][px].feat - FEAT_BLDG_HEAD);
	building_loc = which;

	bldg = &building[which];

	/* Don't re-init the wilderness */
	reinit_wilderness = FALSE;

	if ((which == 2) && (p_ptr->arena_number == 99))
	{
#ifdef JP
msg_print("���ԼԤ��ѤϤʤ�����");
#else
		msg_print("'There's no place here for a LOSER like you!'");
#endif
		return;
	}
	else if ((which == 2) && p_ptr->inside_arena && !p_ptr->exit_bldg)
	{
#ifdef JP
prt("�����Ȥ��ĤޤäƤ��롣��󥹥��������ʤ����ԤäƤ��롪",0,0);
#else
		prt("The gates are closed.  The monster awaits!", 0, 0);
#endif

		return;
	}
	else if ((which == 2) && p_ptr->inside_arena)
	{
		p_ptr->leaving = TRUE;
		p_ptr->inside_arena = FALSE;
	}
	else if (p_ptr->inside_battle)
	{
		p_ptr->leaving = TRUE;
		p_ptr->inside_battle = FALSE;
	}
	else
	{
		p_ptr->oldpy = py;
		p_ptr->oldpx = px;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();

	/* Hack -- Increase "icky" depth */
	character_icky++;

	command_arg = 0;
	command_rep = 0;
	command_new = 0;

	show_building(bldg);
	leave_bldg = FALSE;

	while (!leave_bldg)
	{
		validcmd = FALSE;
		prt("", 1, 0);

		building_prt_gold();

		command = inkey();

		if (command == ESCAPE)
		{
			leave_bldg = TRUE;
			p_ptr->inside_arena = FALSE;
			p_ptr->inside_battle = FALSE;
			break;
		}

		for (i = 0; i < 8; i++)
		{
			if (bldg->letters[i])
			{
				if (bldg->letters[i] == command)
				{
					validcmd = TRUE;
					break;
				}
			}
		}

		if (validcmd)
			bldg_process_command(bldg, i);

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();
	}

	/* Flush messages XXX XXX XXX */
	msg_flag = FALSE;
	msg_print(NULL);

	/* Reinit wilderness to activate quests ... */
	if (reinit_wilderness)
		p_ptr->leaving = TRUE;

	/* Hack -- Decrease "icky" depth */
	character_icky--;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	p_ptr->update |= (PU_VIEW | PU_MONSTERS | PU_BONUS | PU_LITE | PU_MON_LITE);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY | PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}


/* Array of places to find an inscription */
static cptr find_quest[] =
{
#ifdef JP
"���˥�å���������ޤ�Ƥ���:",
#else
	"You find the following inscription in the floor",
#endif

#ifdef JP
"�ɤ˥�å���������ޤ�Ƥ���:",
#else
	"You see a message inscribed in the wall",
#endif

#ifdef JP
"��å������򸫤Ĥ���:",
#else
	"There is a sign saying",
#endif

#ifdef JP
"���������ʤξ�˽񤤤Ƥ���:",
#else
	"Something is written on the staircase",
#endif

#ifdef JP
"��ʪ�򸫤Ĥ�������å��������񤤤Ƥ���:",
#else
	"You find a scroll with the following message",
#endif

};


/*
 * Discover quest
 */
void quest_discovery(int q_idx)
{
	quest_type      *q_ptr = &quest[q_idx];
	monster_race    *r_ptr = &r_info[q_ptr->r_idx];
	int             q_num = q_ptr->max_num;
	char            name[80];

	/* No quest index */
	if (!q_idx) return;

	strcpy(name, (r_name + r_ptr->name));

	msg_print(find_quest[rand_range(0, 4)]);
	msg_print(NULL);

	if (q_num == 1)
	{
		/* Unique */

		/* Hack -- "unique" monsters must be "unique" */
		if ((r_ptr->flags1 & RF1_UNIQUE) &&
		    (0 == r_ptr->max_num))
		{
#ifdef JP
			msg_print("���γ��ϰ�����ï���ˤ�äƼ���Ƥ����褦���ġ�");
#else
			msg_print("It seems that this level was protected by someone before...");
#endif
			/* The unique is already dead */
			quest[q_idx].status = QUEST_STATUS_FINISHED;
		}
		else
		{
#ifdef JP
			msg_format("��դ��衪���γ���%s�ˤ�äƼ���Ƥ��롪", name);
#else
			msg_format("Beware, this level is protected by %s!", name);
#endif
		}
	}
	else
	{
		/* Normal monsters */
#ifdef JP
msg_format("��դ������γ���%d�Τ�%s�ˤ�äƼ���Ƥ��롪", q_num, name);
#else
		plural_aux(name);
		msg_format("Be warned, this level is guarded by %d %s!", q_num, name);
#endif

	}
}


/*
 * Hack -- Check if a level is a "quest" level
 */
int quest_number(int level)
{
	int i;

	/* Check quests */
	if (p_ptr->inside_quest)
		return (p_ptr->inside_quest);

	for (i = 0; i < max_quests; i++)
	{
		if (quest[i].status != QUEST_STATUS_TAKEN) continue;

		if ((quest[i].type == QUEST_TYPE_KILL_LEVEL) &&
			!(quest[i].flags & QUEST_FLAG_PRESET) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == dungeon_type))
			return (i);
	}

	/* Check for random quest */
	return (random_quest_number(level));
}


/*
 * Return the index of the random quest on this level
 * (or zero)
 */
int random_quest_number(int level)
{
	int i;

	if (dungeon_type != DUNGEON_ANGBAND) return 0;

	for (i = MIN_RANDOM_QUEST; i < MAX_RANDOM_QUEST + 1; i++)
	{
		if ((quest[i].type == QUEST_TYPE_RANDOM) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
		    (quest[i].level == level) &&
		    (quest[i].dungeon == DUNGEON_ANGBAND))
		{
			return i;
		}
	}

	/* Nope */
	return 0;
}
