/* File: init1.c */

/* Purpose: Initialization (part 1) -BEN- */

#include "angband.h"


#ifdef JP
#undef strchr
char* _strchr(char* ptr, char ch)
{
  int k_flag = 0;

  for ( ; *ptr != '\0'; ++ptr)
    if (k_flag == 0) {
      if (*ptr == ch)
	return ptr;
      if (iskanji(*ptr))
	k_flag = 1;
    } else
      k_flag = 0;

  return NULL;
}
#define strchr _strchr
#endif
/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * Note that if "ALLOW_TEMPLATES" is not defined, then a lot of the code
 * in this file is compiled out, and the game will not run unless valid
 * "binary template files" already exist in "lib/data".  Thus, one can
 * compile Angband with ALLOW_TEMPLATES defined, run once to create the
 * "*.raw" files in "lib/data", and then quit, and recompile without
 * defining ALLOW_TEMPLATES, which will both save 20K and prevent people
 * from changing the ascii template files in potentially dangerous ways.
 *
 * The code could actually be removed and placed into a "stand-alone"
 * program, but that feels a little silly, especially considering some
 * of the platforms that we currently support.
 */


#ifdef ALLOW_TEMPLATES


/*** Helper arrays for parsing ascii template files ***/

/*
 * Monster Blow Methods
 */
static cptr r_info_blow_method[] =
{
	"",
	"HIT",
	"TOUCH",
	"PUNCH",
	"KICK",
	"CLAW",
	"BITE",
	"STING",
	"SLASH",
	"BUTT",
	"CRUSH",
	"ENGULF",
	"CHARGE",
	"CRAWL",
	"DROOL",
	"SPIT",
	"EXPLODE",
	"GAZE",
	"WAIL",
	"SPORE",
	"XXX4",
	"BEG",
	"INSULT",
	"MOAN",
	"SHOW",
	NULL
};


/*
 * Monster Blow Effects
 */
static cptr r_info_blow_effect[] =
{
	"",
	"HURT",
	"POISON",
	"UN_BONUS",
	"UN_POWER",
	"EAT_GOLD",
	"EAT_ITEM",
	"EAT_FOOD",
	"EAT_LITE",
	"ACID",
	"ELEC",
	"FIRE",
	"COLD",
	"BLIND",
	"CONFUSE",
	"TERRIFY",
	"PARALYZE",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_ALL",
	"SHATTER",
	"EXP_10",
	"EXP_20",
	"EXP_40",
	"EXP_80",
	"DISEASE",
	"TIME",
	"EXP_VAMP",
	"DR_MANA",
	"SUPERHURT",
	NULL
};


/*
 * Monster race flags
 */
static cptr r_info_flags1[] =
{
	"UNIQUE",
	"QUESTOR",
	"MALE",
	"FEMALE",
	"CHAR_CLEAR",
	"CHAR_MULTI",
	"ATTR_CLEAR",
	"ATTR_MULTI",
	"FORCE_DEPTH",
	"FORCE_MAXHP",
	"FORCE_SLEEP",
	"FORCE_EXTRA",
	"FRIEND",
	"FRIENDS",
	"ESCORT",
	"ESCORTS",
	"NEVER_BLOW",
	"NEVER_MOVE",
	"RAND_25",
	"RAND_50",
	"ONLY_GOLD",
	"ONLY_ITEM",
	"DROP_60",
	"DROP_90",
	"DROP_1D2",
	"DROP_2D2",
	"DROP_3D2",
	"DROP_4D2",
	"DROP_GOOD",
	"DROP_GREAT",
	"DROP_USEFUL",
	"DROP_CHOSEN"
};

/*
 * Monster race flags
 */
static cptr r_info_flags2[] =
{
	"STUPID",
	"SMART",
	"CAN_SPEAK",
	"REFLECTING",
	"INVISIBLE",
	"COLD_BLOOD",
	"EMPTY_MIND",
	"WEIRD_MIND",
	"MULTIPLY",
	"REGENERATE",
	"SHAPECHANGER",
	"ATTR_ANY",
	"POWERFUL",
	"ELDRITCH_HORROR",
	"AURA_FIRE",
	"AURA_ELEC",
	"OPEN_DOOR",
	"BASH_DOOR",
	"PASS_WALL",
	"KILL_WALL",
	"MOVE_BODY",
	"KILL_BODY",
	"TAKE_ITEM",
	"KILL_ITEM",
	"BRAIN_1",
	"BRAIN_2",
	"BRAIN_3",
	"BRAIN_4",
	"BRAIN_5",
	"BRAIN_6",
	"BRAIN_7",
	"QUANTUM"
};

/*
 * Monster race flags
 */
static cptr r_info_flags3[] =
{
	"ORC",
	"TROLL",
	"GIANT",
	"DRAGON",
	"DEMON",
	"UNDEAD",
	"EVIL",
	"ANIMAL",
	"AMBERITE",
	"GOOD",
	"AURA_COLD",
	"NONLIVING",
	"HURT_LITE",
	"HURT_ROCK",
	"HURT_FIRE",
	"HURT_COLD",
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"IM_POIS",
	"RES_TELE",
	"RES_NETH",
	"RES_WATE",
	"RES_PLAS",
	"RES_NEXU",
	"RES_DISE",
	"RES_ALL",
	"NO_FEAR",
	"NO_STUN",
	"NO_CONF",
	"NO_SLEEP"
};

/*
 * Monster race flags
 */
static cptr r_info_flags4[] =
{
	"SHRIEK",
	"XXX1",
	"DISPEL",
	"ROCKET",
	"ARROW_1",
	"ARROW_2",
	"ARROW_3",
	"ARROW_4",
	"BR_ACID",
	"BR_ELEC",
	"BR_FIRE",
	"BR_COLD",
	"BR_POIS",
	"BR_NETH",
	"BR_LITE",
	"BR_DARK",
	"BR_CONF",
	"BR_SOUN",
	"BR_CHAO",
	"BR_DISE",
	"BR_NEXU",
	"BR_TIME",
	"BR_INER",
	"BR_GRAV",
	"BR_SHAR",
	"BR_PLAS",
	"BR_WALL",
	"BR_MANA",
	"BA_NUKE",
	"BR_NUKE",
	"BA_CHAO",
	"BR_DISI",
};

/*
 * Monster race flags
 */
static cptr r_info_flags5[] =
{
	"BA_ACID",
	"BA_ELEC",
	"BA_FIRE",
	"BA_COLD",
	"BA_POIS",
	"BA_NETH",
	"BA_WATE",
	"BA_MANA",
	"BA_DARK",
	"DRAIN_MANA",
	"MIND_BLAST",
	"BRAIN_SMASH",
	"CAUSE_1",
	"CAUSE_2",
	"CAUSE_3",
	"CAUSE_4",
	"BO_ACID",
	"BO_ELEC",
	"BO_FIRE",
	"BO_COLD",
	"BA_LITE",
	"BO_NETH",
	"BO_WATE",
	"BO_MANA",
	"BO_PLAS",
	"BO_ICEE",
	"MISSILE",
	"SCARE",
	"BLIND",
	"CONF",
	"SLOW",
	"HOLD"
};

/*
 * Monster race flags
 */
static cptr r_info_flags6[] =
{
	"HASTE",
	"HAND_DOOM",
	"HEAL",
	"INVULNER",
	"BLINK",
	"TPORT",
	"WORLD",
	"SPECIAL",
	"TELE_TO",
	"TELE_AWAY",
	"TELE_LEVEL",
	"PSY_SPEAR",
	"DARKNESS",
	"TRAPS",
	"FORGET",
	"ANIM_DEAD", /* ToDo: Implement ANIM_DEAD */
	"S_KIN",
	"S_CYBER",
	"S_MONSTER",
	"S_MONSTERS",
	"S_ANT",
	"S_SPIDER",
	"S_HOUND",
	"S_HYDRA",
	"S_ANGEL",
	"S_DEMON",
	"S_UNDEAD",
	"S_DRAGON",
	"S_HI_UNDEAD",
	"S_HI_DRAGON",
	"S_AMBERITES",
	"S_UNIQUE"
};


/*
 * Monster race flags
 */
static cptr r_info_flags7[] =
{
	"AQUATIC",
	"CAN_SWIM",
	"CAN_FLY",
	"FRIENDLY",
	"UNIQUE_7",
	"UNIQUE2",
	"RIDING",
	"KAGE",
	"HAS_LITE_1",
	"SELF_LITE_1",
	"HAS_LITE_2",
	"SELF_LITE_2",
	"GUARDIAN",
	"CHAMELEON",
	"KILL_EXP",
	"XXX7X15",
	"XXX7X16",
	"XXX7X17",
	"XXX7X18",
	"XXX7X19",
	"XXX7X20",
	"XXX7X21",
	"XXX7X22",
	"XXX7X23",
	"XXX7X24",
	"XXX7X25",
	"XXX7X26",
	"XXX7X27",
	"XXX7X28",
	"XXX7X29",
	"XXX7X30",
	"XXX7X31",
};

/*
 * Monster race flags
 */
static cptr r_info_flags8[] =
{
	"WILD_ONLY",
	"WILD_TOWN",
	"XXX8X02",
	"WILD_SHORE",
	"WILD_OCEAN",
	"WILD_WASTE",
	"WILD_WOOD",
	"WILD_VOLCANO",
	"XXX8X08",
	"WILD_MOUNTAIN",
	"WILD_GRASS",
	"XXX8X11",
	"XXX8X12",
	"XXX8X13",
	"XXX8X14",
	"XXX8X15",
	"XXX8X16",
	"XXX8X17",
	"XXX8X18",
	"XXX8X19",
	"XXX8X20",
	"XXX8X21",
	"XXX8X22",
	"XXX8X23",
	"XXX8X24",
	"XXX8X25",
	"XXX8X26",
	"XXX8X27",
	"XXX8X28",
	"XXX8X29",
	"WILD_SWAMP",	/* ToDo: Implement Swamp */
	"WILD_TOO",
};


/*
 * Monster race flags - Drops
 */
static cptr r_info_flags9[] =
{
	"DROP_CORPSE",
	"DROP_SKELETON",
	"EAT_BLIND",
	"EAT_CONF",
	"EAT_MANA",
	"EAT_NEXUS",
	"EAT_BLINK",
	"EAT_SLEEP",
	"EAT_BERSERKER",
	"EAT_ACIDIC",
	"EAT_SPEED",
	"EAT_CURE",
	"EAT_FIRE_RES",
	"EAT_COLD_RES",
	"EAT_ACID_RES",
	"EAT_ELEC_RES",
	"EAT_POIS_RES",
	"EAT_INSANITY",
	"EAT_DRAIN_EXP",
	"EAT_POISONOUS",
	"EAT_GIVE_STR",
	"EAT_GIVE_INT",
	"EAT_GIVE_WIS",
	"EAT_GIVE_DEX",
	"EAT_GIVE_CON",
	"EAT_GIVE_CHR",
	"EAT_LOSE_STR",
	"EAT_LOSE_INT",
	"EAT_LOSE_WIS",
	"EAT_LOSE_DEX",
	"EAT_LOSE_CON",
	"EAT_LOSE_CHR",
	"EAT_DRAIN_MANA",
};


/*
 * Object flags
 */
static cptr k_info_flags1[] =
{
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHR",
	"MAGIC_MASTERY",
	"FORCE_WEPON",
	"STEALTH",
	"SEARCH",
	"INFRA",
	"TUNNEL",
	"SPEED",
	"BLOWS",
	"CHAOTIC",
	"VAMPIRIC",
	"SLAY_ANIMAL",
	"SLAY_EVIL",
	"SLAY_UNDEAD",
	"SLAY_DEMON",
	"SLAY_ORC",
	"SLAY_TROLL",
	"SLAY_GIANT",
	"SLAY_DRAGON",
	"KILL_DRAGON",
	"VORPAL",
	"IMPACT",
	"BRAND_POIS",
	"BRAND_ACID",
	"BRAND_ELEC",
	"BRAND_FIRE",
	"BRAND_COLD"
};

/*
 * Object flags
 */
static cptr k_info_flags2[] =
{
	"SUST_STR",
	"SUST_INT",
	"SUST_WIS",
	"SUST_DEX",
	"SUST_CON",
	"SUST_CHR",
	"RIDING",
	"XXX2",
	"IM_ACID",
	"IM_ELEC",
	"IM_FIRE",
	"IM_COLD",
	"THROW",
	"REFLECT",
	"FREE_ACT",
	"HOLD_LIFE",
	"RES_ACID",
	"RES_ELEC",
	"RES_FIRE",
	"RES_COLD",
	"RES_POIS",
	"RES_FEAR",
	"RES_LITE",
	"RES_DARK",
	"RES_BLIND",
	"RES_CONF",
	"RES_SOUND",
	"RES_SHARDS",
	"RES_NETHER",
	"RES_NEXUS",
	"RES_CHAOS",
	"RES_DISEN"
};

/*
 * Object flags
 */
static cptr k_info_flags3[] =
{
	"SH_FIRE",
	"SH_ELEC",
	"QUESTITEM",
	"SH_COLD",
	"NO_TELE",
	"NO_MAGIC",
	"DEC_MANA",
	"TY_CURSE",
	"XXX1",
	"HIDE_TYPE",
	"SHOW_MODS",
	"INSTA_ART",
	"FEATHER",
	"LITE",
	"SEE_INVIS",
	"TELEPATHY",
	"SLOW_DIGEST",
	"REGEN",
	"XTRA_MIGHT",
	"XTRA_SHOTS",
	"IGNORE_ACID",
	"IGNORE_ELEC",
	"IGNORE_FIRE",
	"IGNORE_COLD",
	"ACTIVATE",
	"DRAIN_EXP",
	"TELEPORT",
	"AGGRAVATE",
	"BLESSED",
	"CURSED",
	"HEAVY_CURSE",
	"PERMA_CURSE"
};


/*
 * Dungeon flags
 */
static cptr d_info_flags1[] =
{
        "WINNER",
        "MAZE",
        "SMALLEST",
        "BEGINNER",
        "BIG",
        "NO_DOORS",
        "WATER_RIVER",
        "LAVA_RIVER",
        "WATER_RIVERS",
        "LAVA_RIVERS",
        "CAVE",
        "CAVERN",
        "NO_UP",
        "HOT",
        "COLD",
        "NO_DOWN",
        "FORGET",
        "LAKE_WATER",
        "LAKE_LAVA",
        "LAKE_RUBBLE",
        "LAKE_TREE",
        "NO_VAULT",
        "ARENA",
        "DESTROY",
        "XXX1",
        "NO_CAVE",
        "NO_MAGIC",
        "NO_MELEE",
        "CHAMELEON",
        "DARKNESS",
        "XXX1",
        "XXX1"
};


/*
 * Convert a "color letter" into an "actual" color
 * The colors are: dwsorgbuDWvyRGBU, as shown below
 */
static int color_char_to_attr(char c)
{
	switch (c)
	{
		case 'd': return (TERM_DARK);
		case 'w': return (TERM_WHITE);
		case 's': return (TERM_SLATE);
		case 'o': return (TERM_ORANGE);
		case 'r': return (TERM_RED);
		case 'g': return (TERM_GREEN);
		case 'b': return (TERM_BLUE);
		case 'u': return (TERM_UMBER);

		case 'D': return (TERM_L_DARK);
		case 'W': return (TERM_L_WHITE);
		case 'v': return (TERM_VIOLET);
		case 'y': return (TERM_YELLOW);
		case 'R': return (TERM_L_RED);
		case 'G': return (TERM_L_GREEN);
		case 'B': return (TERM_L_BLUE);
		case 'U': return (TERM_L_UMBER);
	}

	return (-1);
}



/*** Initialize from ascii template files ***/


/*
 * Initialize the "v_info" array, by parsing an ascii "template" file
 */
errr init_v_info_txt(FILE *fp, char *buf, bool start)
{
	int i;
	char *s;

	/* Current entry */
	vault_type *v_ptr = NULL;

	if (start)
	{
		/* Just before the first record */
		error_idx = -1;

		/* Just before the first line */
		error_line = -1;

		/* Prepare the "fake" stuff */
		v_head->name_size = 0;
		v_head->text_size = 0;
	}

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;
		if ((buf[0] == 'Q') || (buf[0] == 'T')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

			/* Paranoia -- require a name */
			if (!*s) return (1);

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i <= error_idx) return (4);

			/* Verify information */
			if (i >= v_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			v_ptr = &v_info[i];

			/* Hack -- Verify space */
			if (v_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!v_ptr->name) v_ptr->name = ++v_head->name_size;

			/* Append chars to the name */
			strcpy(v_name + v_head->name_size, s);

			/* Advance the index */
			v_head->name_size += strlen(s);

			/* Next... */
			continue;
		}

		/* There better be a current v_ptr */
		if (!v_ptr) return (3);

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Acquire the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (v_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
			if (!v_ptr->text) v_ptr->text = ++v_head->text_size;

			/* Append chars to the name */
			strcpy(v_text + v_head->text_size, s);

			/* Advance the index */
			v_head->text_size += strlen(s);

			/* Next... */
			continue;
		}


		/* Process 'X' for "Extra info" (one line only) */
		if (buf[0] == 'X')
		{
			int typ, rat, hgt, wid;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%d",
				&typ, &rat, &hgt, &wid)) return (1);

			/* Save the values */
			v_ptr->typ = typ;
			v_ptr->rat = rat;
			v_ptr->hgt = hgt;
			v_ptr->wid = wid;

			/* Next... */
			continue;
		}


		/* Oops */
		return (6);
	}


	/* Complete the "name" and "text" sizes */
	if (!start)
	{
		++v_head->name_size;
		++v_head->text_size;
	}


	/* Success */
	return (0);
}



/*
 * Initialize the "f_info" array, by parsing an ascii "template" file
 */
errr init_f_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s;

	/* Current entry */
	feature_type *f_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	f_head->name_size = 0;
#ifdef JP
	/* �Ѹ�̾�� */
	f_head->E_name_size = 0;
#endif
	f_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

#ifdef JP
			/* Paranoia -- require a name */
			if (!*s) return (1);
#endif

			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i <= error_idx) return (4);

			/* Verify information */
			if (i >= f_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			f_ptr = &f_info[i];

#ifdef JP
			/* Hack -- Verify space */
			if (f_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!f_ptr->name) f_ptr->name = ++f_head->name_size;

			/* Append chars to the name */
			strcpy(f_name + f_head->name_size, s);

			/* Advance the index */
			f_head->name_size += strlen(s);
#endif
			/* Default "mimic" */
			f_ptr->mimic = i;

			/* Next... */
			continue;
		}

		/* There better be a current f_ptr */
		if (!f_ptr) return (3);

#ifdef JP
		/* �Ѹ�̾���ɤ�롼������ɲ� */
		/* 'E' ����Ϥޤ�ԤϱѸ�̾�Ȥ��Ƥ��� */
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (f_head->E_name_size + strlen(s) + 8 > E_fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!f_ptr->E_name) f_ptr->E_name = ++f_head->E_name_size;

			/* Append chars to the name */
			strcpy(E_f_name+ f_head->E_name_size, s);

			/* Advance the index */
			f_head->E_name_size += strlen(s);

			/* Next... */
			continue;
		}
#else
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (f_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!f_ptr->name) f_ptr->name = ++f_head->name_size;

			/* Append chars to the name */
			strcpy(f_name+ f_head->name_size, s);

			/* Advance the index */
			f_head->name_size += strlen(s);

			/* Next... */
			continue;
		}
#endif

#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Acquire the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (f_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
			if (!f_ptr->text) f_ptr->text = ++f_head->text_size;

			/* Append chars to the name */
			strcpy(f_text + f_head->text_size, s);

			/* Advance the index */
			f_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif


		/* Process 'M' for "Mimic" (one line only) */
		if (buf[0] == 'M')
		{
			int mimic;

			/* Scan for the values */
			if (1 != sscanf(buf+2, "%d",
				&mimic)) return (1);

			/* Save the values */
			f_ptr->mimic = mimic;

			/* Next... */
			continue;
		}


		/* Process 'G' for "Graphics" (one line only) */
		if (buf[0] == 'G')
		{
			int tmp;

			/* Paranoia */
			if (!buf[2]) return (1);
			if (!buf[3]) return (1);
			if (!buf[4]) return (1);

			/* Extract the color */
			tmp = color_char_to_attr(buf[4]);

			/* Paranoia */
			if (tmp < 0) return (1);

			/* Save the values */
			f_ptr->d_attr = tmp;
			f_ptr->d_char = buf[2];

			/* Next... */
			continue;
		}


		/* Oops */
		return (6);
	}


	/* Complete the "name" and "text" sizes */
	++f_head->name_size;
#ifdef JP
	/* �Ѹ�̾�� */
	++f_head->E_name_size;
#endif
	++f_head->text_size;


	/* Success */
	return (0);
}


/*
 * Grab one flag in an object_kind from a textual string
 */
static errr grab_one_kind_flag(object_kind *k_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			k_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			k_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			k_ptr->flags3 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�ΤΥ����ƥࡦ�ե饰 '%s'��", what);
#else
	msg_format("Unknown object flag '%s'.", what);
#endif


	/* Error */
	return (1);
}


/*
 * Initialize the "k_info" array, by parsing an ascii "template" file
 */
errr init_k_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Current entry */
	object_kind *k_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Prepare the "fake" stuff */
	k_head->name_size = 0;
#ifdef JP
	/* �Ѹ�̾�� */
	k_head->E_name_size = 0;
#endif
	k_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';

#ifdef JP
			/* Paranoia -- require a name */
			if (!*s) return (1);
#endif
			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i <= error_idx) return (4);

			/* Verify information */
			if (i >= k_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			k_ptr = &k_info[i];

#ifdef JP
			/* Hack -- Verify space */
			if (k_head->name_size + strlen(s) + 8 > fake_name_size)
			{
				fake_name_size += 1000;

				/* Reallocate the extra memory */
				k_info = realloc(k_name, fake_name_size);
			}

			/* Advance and Save the name index */
			if (!k_ptr->name) k_ptr->name = ++k_head->name_size;

			/* Append chars to the name */
			strcpy(k_name + k_head->name_size, s);

			/* Advance the index */
			k_head->name_size += strlen(s);
#endif
			/* Next... */
			continue;
		}

		/* There better be a current k_ptr */
		if (!k_ptr) return (3);


#ifdef JP
		/* �Ѹ�̾���ɤ�롼������ɲ� */
		/* 'E' ����Ϥޤ�ԤϱѸ�̾�Ȥ��Ƥ��� */
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (k_head->E_name_size + strlen(s) + 8 > E_fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!k_ptr->E_name) k_ptr->E_name = ++k_head->E_name_size;

			/* Append chars to the name */
			strcpy(E_k_name+ k_head->E_name_size, s);

			/* Advance the index */
			k_head->E_name_size += strlen(s);

			/* Next... */
			continue;
		}
#else
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (k_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!k_ptr->name) k_ptr->name = ++k_head->name_size;

			/* Append chars to the name */
			strcpy(k_name+ k_head->name_size, s);

			/* Advance the index */
			k_head->name_size += strlen(s);

			/* Next... */
			continue;
		}
#endif
#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Acquire the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (k_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
			if (!k_ptr->text) k_ptr->text = ++k_head->text_size;

			/* Append chars to the name */
			strcpy(k_text + k_head->text_size, s);

			/* Advance the index */
			k_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif


		/* Process 'G' for "Graphics" (one line only) */
		if (buf[0] == 'G')
		{
			char sym;
			int tmp;

			/* Paranoia */
			if (!buf[2]) return (1);
			if (!buf[3]) return (1);
			if (!buf[4]) return (1);

			/* Extract the char */
			sym = buf[2];

			/* Extract the attr */
			tmp = color_char_to_attr(buf[4]);

			/* Paranoia */
			if (tmp < 0) return (1);

			/* Save the values */
			k_ptr->d_attr = tmp;
			k_ptr->d_char = sym;

			/* Next... */
			continue;
		}

		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int tval, sval, pval;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
				&tval, &sval, &pval)) return (1);

			/* Save the values */
			k_ptr->tval = tval;
			k_ptr->sval = sval;
			k_ptr->pval = pval;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int level, extra, wgt;
			long cost;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
				&level, &extra, &wgt, &cost)) return (1);

			/* Save the values */
			k_ptr->level = level;
			k_ptr->extra = extra;
			k_ptr->weight = wgt;
			k_ptr->cost = cost;

			/* Next... */
			continue;
		}

		/* Process 'A' for "Allocation" (one line only) */
		if (buf[0] == 'A')
		{
			int i;

			/* XXX XXX XXX Simply read each number following a colon */
			for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i)
			{
				/* Default chance */
				k_ptr->chance[i] = 1;

				/* Store the attack damage index */
				k_ptr->locale[i] = atoi(s+1);

				/* Find the slash */
				t = strchr(s+1, '/');

				/* Find the next colon */
				s = strchr(s+1, ':');

				/* If the slash is "nearby", use it */
				if (t && (!s || t < s))
				{
					int chance = atoi(t+1);
					if (chance > 0) k_ptr->chance[i] = chance;
				}
			}

			/* Next... */
			continue;
		}

		/* Hack -- Process 'P' for "power" and such */
		if (buf[0] == 'P')
		{
			int ac, hd1, hd2, th, td, ta;

			/* Scan for the values */
			if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
				&ac, &hd1, &hd2, &th, &td, &ta)) return (1);

			k_ptr->ac = ac;
			k_ptr->dd = hd1;
			k_ptr->ds = hd2;
			k_ptr->to_h = th;
			k_ptr->to_d = td;
			k_ptr->to_a =  ta;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while (*t == ' ' || *t == '|') t++;
				}

				/* Parse this entry */
				if (0 != grab_one_kind_flag(k_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}


		/* Oops */
		return (6);
	}


	/* Complete the "name" and "text" sizes */
	++k_head->name_size;
#ifdef JP
	/* �Ѹ�̾�� */
	++k_head->E_name_size;
#endif
	++k_head->text_size;


	/* Success */
	return (0);
}


/*
 * Grab one flag in an artifact_type from a textual string
 */
static errr grab_one_artifact_flag(artifact_type *a_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			a_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			a_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			a_ptr->flags3 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�Τ�����Υ����ƥࡦ�ե饰 '%s'��", what);
#else
	msg_format("Unknown artifact flag '%s'.", what);
#endif


	/* Error */
	return (1);
}




/*
 * Initialize the "a_info" array, by parsing an ascii "template" file
 */
errr init_a_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Current entry */
	artifact_type *a_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';
#ifdef JP
			/* Paranoia -- require a name */
			if (!*s) return (1);
#endif
			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (4);

			/* Verify information */
			if (i >= a_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			a_ptr = &a_info[i];

			/* Ignore everything */
			a_ptr->flags3 |= (TR3_IGNORE_ACID);
			a_ptr->flags3 |= (TR3_IGNORE_ELEC);
			a_ptr->flags3 |= (TR3_IGNORE_FIRE);
			a_ptr->flags3 |= (TR3_IGNORE_COLD);
#ifdef JP
			/* Hack -- Verify space */
			if (a_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!a_ptr->name) a_ptr->name = ++a_head->name_size;

			/* Append chars to the name */
			strcpy(a_name + a_head->name_size, s);

			/* Advance the index */
			a_head->name_size += strlen(s);
#endif
			/* Next... */
			continue;
		}

		/* There better be a current a_ptr */
		if (!a_ptr) return (3);


#ifdef JP
		/* �Ѹ�̾���ɤ�롼������ɲ� */
		/* 'E' ����Ϥޤ�ԤϱѸ�̾�Ȥ��Ƥ��� */
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (a_head->E_name_size + strlen(s) + 8 > E_fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!a_ptr->E_name) a_ptr->E_name = ++a_head->E_name_size;

			/* Append chars to the name */
			strcpy(E_a_name+ a_head->E_name_size, s);

			/* Advance the index */
			a_head->E_name_size += strlen(s);

			/* Next... */
			continue;
		}
#else
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (a_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!a_ptr->name) a_ptr->name = ++a_head->name_size;

			/* Append chars to the name */
			strcpy(a_name+ a_head->name_size, s);

			/* Advance the index */
			a_head->name_size += strlen(s);

			/* Next... */
			continue;
		}
#endif

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
#ifdef JP
			if (buf[2] == '$')
				continue;
			/* Acquire the text */
			s = buf+2;
#else
			if (buf[2] != '$')
				continue;
			/* Acquire the text */
			s = buf+3;
#endif

			/* Hack -- Verify space */
			if (a_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
			if (!a_ptr->text) a_ptr->text = ++a_head->text_size;

			/* Append chars to the name */
			strcpy(a_text + a_head->text_size, s);

			/* Advance the index */
			a_head->text_size += strlen(s);

			/* Next... */
			continue;
		}


		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int tval, sval, pval;

			/* Scan for the values */
			if (3 != sscanf(buf+2, "%d:%d:%d",
				&tval, &sval, &pval)) return (1);

			/* Save the values */
			a_ptr->tval = tval;
			a_ptr->sval = sval;
			a_ptr->pval = pval;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int level, rarity, wgt;
			long cost;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
				&level, &rarity, &wgt, &cost)) return (1);

			/* Save the values */
			a_ptr->level = level;
			a_ptr->rarity = rarity;
			a_ptr->weight = wgt;
			a_ptr->cost = cost;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'P' for "power" and such */
		if (buf[0] == 'P')
		{
			int ac, hd1, hd2, th, td, ta;

			/* Scan for the values */
			if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
				&ac, &hd1, &hd2, &th, &td, &ta)) return (1);

			a_ptr->ac = ac;
			a_ptr->dd = hd1;
			a_ptr->ds = hd2;
			a_ptr->to_h = th;
			a_ptr->to_d = td;
			a_ptr->to_a =  ta;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* Parse this entry */
				if (0 != grab_one_artifact_flag(a_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}


		/* Oops */
		return (6);
	}


	/* Complete the "name" and "text" sizes */
	++a_head->name_size;
#ifdef JP
	/* �Ѹ�̾�� */
	++a_head->E_name_size;
#endif
	++a_head->text_size;


	/* Success */
	return (0);
}


/*
 * Grab one flag in a ego-item_type from a textual string
 */
static bool grab_one_ego_item_flag(ego_item_type *e_ptr, cptr what)
{
	int i;

	/* Check flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags1[i]))
		{
			e_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Check flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags2[i]))
		{
			e_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Check flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, k_info_flags3[i]))
		{
			e_ptr->flags3 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�Τ�̾�Τ��륢���ƥࡦ�ե饰 '%s'��", what);
#else
	msg_format("Unknown ego-item flag '%s'.", what);
#endif


	/* Error */
	return (1);
}




/*
 * Initialize the "e_info" array, by parsing an ascii "template" file
 */
errr init_e_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Current entry */
	ego_item_type *e_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';
#ifdef JP
			/* Paranoia -- require a name */
			if (!*s) return (1);
#endif
			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (4);

			/* Verify information */
			if (i >= e_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			e_ptr = &e_info[i];
#ifdef JP
			/* Hack -- Verify space */
			if (e_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!e_ptr->name) e_ptr->name = ++e_head->name_size;

			/* Append chars to the name */
			strcpy(e_name + e_head->name_size, s);

			/* Advance the index */
			e_head->name_size += strlen(s);
#endif
			/* Next... */
			continue;
		}

		/* There better be a current e_ptr */
		if (!e_ptr) return (3);


#ifdef JP
		/* �Ѹ�̾���ɤ�롼������ɲ� */
		/* 'E' ����Ϥޤ�ԤϱѸ�̾ */
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (e_head->E_name_size + strlen(s) + 8 > E_fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!e_ptr->E_name) e_ptr->E_name = ++e_head->E_name_size;

			/* Append chars to the name */
			strcpy(E_e_name+ e_head->E_name_size, s);

			/* Advance the index */
			e_head->E_name_size += strlen(s);

			/* Next... */
			continue;
		}
#else
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (e_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!e_ptr->name) e_ptr->name = ++e_head->name_size;

			/* Append chars to the name */
			strcpy(e_name+ e_head->name_size, s);

			/* Advance the index */
			e_head->name_size += strlen(s);

			/* Next... */
			continue;
		}
#endif
#if 0

		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
			/* Acquire the text */
			s = buf+2;

			/* Hack -- Verify space */
			if (e_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
			if (!e_ptr->text) e_ptr->text = ++e_head->text_size;

			/* Append chars to the name */
			strcpy(e_text + e_head->text_size, s);

			/* Advance the index */
			e_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

#endif

		/* Process 'X' for "Xtra" (one line only) */
		if (buf[0] == 'X')
		{
			int slot, rating;

			/* Scan for the values */
			if (2 != sscanf(buf+2, "%d:%d",
				&slot, &rating)) return (1);

			/* Save the values */
			e_ptr->slot = slot;
			e_ptr->rating = rating;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int level, rarity, pad2;
			long cost;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
				&level, &rarity, &pad2, &cost)) return (1);

			/* Save the values */
			e_ptr->level = level;
			e_ptr->rarity = rarity;
			/* e_ptr->weight = wgt; */
			e_ptr->cost = cost;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'C' for "creation" */
		if (buf[0] == 'C')
		{
			int th, td, ta, pv;

			/* Scan for the values */
			if (4 != sscanf(buf+2, "%d:%d:%d:%d",
				&th, &td, &ta, &pv)) return (1);

			e_ptr->max_to_h = th;
			e_ptr->max_to_d = td;
			e_ptr->max_to_a = ta;
			e_ptr->max_pval = pv;

			/* Next... */
			continue;
		}

		/* Hack -- Process 'F' for flags */
		if (buf[0] == 'F')
		{
			/* Parse every entry textually */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* Parse this entry */
				if (0 != grab_one_ego_item_flag(e_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Oops */
		return (6);
	}


	/* Complete the "name" and "text" sizes */
	++e_head->name_size;
#ifdef JP
	/* �Ѹ�̾�� */
	++e_head->E_name_size;
#endif
	++e_head->text_size;


	/* Success */
	return (0);
}


/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
static errr grab_one_basic_flag(monster_race *r_ptr, cptr what)
{
	int i;

	/* Scan flags1 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags1[i]))
		{
			r_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags2[i]))
		{
			r_ptr->flags2 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags3[i]))
		{
			r_ptr->flags3 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags7 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags7[i]))
		{
			r_ptr->flags7 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags8 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags8[i]))
		{
			r_ptr->flags8 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags9 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags9[i]))
		{
			r_ptr->flags9 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�ΤΥ�󥹥������ե饰 '%s'��", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif


	/* Failure */
	return (1);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_spell_flag(monster_race *r_ptr, cptr what)
{
	int i;

	/* Scan flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags4[i]))
		{
			r_ptr->flags4 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags5 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags5[i]))
		{
			r_ptr->flags5 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags6 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags6[i]))
		{
			r_ptr->flags6 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�ΤΥ�󥹥������ե饰 '%s'��", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif


	/* Failure */
	return (1);
}




/*
 * Initialize the "r_info" array, by parsing an ascii "template" file
 */
errr init_r_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Current entry */
	monster_race *r_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Start the "fake" stuff */
	r_head->name_size = 0;
	r_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}


		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';
#ifdef JP
			/* Paranoia -- require a name */
			if (!*s) return (1);
#endif
			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (4);

			/* Verify information */
			if (i >= r_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			r_ptr = &r_info[i];
#ifdef JP
			/* Hack -- Verify space */
			if (r_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!r_ptr->name) r_ptr->name = ++r_head->name_size;

			/* Append chars to the name */
			strcpy(r_name + r_head->name_size, s);

			/* Advance the index */
			r_head->name_size += strlen(s);
#endif
			/* Next... */
			continue;
		}

		/* There better be a current r_ptr */
		if (!r_ptr) return (3);


#ifdef JP
		/* �Ѹ�̾���ɤ�롼������ɲ� */
		/* 'E' ����Ϥޤ�ԤϱѸ�̾ */
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (r_head->E_name_size + strlen(s) + 8 > E_fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!r_ptr->E_name) r_ptr->E_name = ++r_head->E_name_size;

			/* Append chars to the name */
			strcpy(E_r_name+ r_head->E_name_size, s);

			/* Advance the index */
			r_head->E_name_size += strlen(s);

			/* Next... */
			continue;
		}
#else
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
			if (r_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
			if (!r_ptr->name) r_ptr->name = ++r_head->name_size;

			/* Append chars to the name */
			strcpy(r_name+ r_head->name_size, s);

			/* Advance the index */
			r_head->name_size += strlen(s);

			/* Next... */
			continue;
		}
#endif
		/* Process 'D' for "Description" */
		if (buf[0] == 'D')
		{
#ifdef JP
			if (buf[2] == '$')
				continue;
			/* Acquire the text */
			s = buf+2;
#else
			if (buf[2] != '$')
				continue;
			/* Acquire the text */
			s = buf+3;
#endif

			/* Hack -- Verify space */
			if (r_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
			if (!r_ptr->text) r_ptr->text = ++r_head->text_size;

			/* Append chars to the name */
			strcpy(r_text + r_head->text_size, s);

			/* Advance the index */
			r_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

		/* Process 'G' for "Graphics" (one line only) */
		if (buf[0] == 'G')
		{
			char sym;
			int tmp;

			/* Paranoia */
			if (!buf[2]) return (1);
			if (!buf[3]) return (1);
			if (!buf[4]) return (1);

			/* Extract the char */
			sym = buf[2];

			/* Extract the attr */
			tmp = color_char_to_attr(buf[4]);

			/* Paranoia */
			if (tmp < 0) return (1);

			/* Save the values */
			r_ptr->d_char = sym;
			r_ptr->d_attr = tmp;

			/* Next... */
			continue;
		}

		/* Process 'I' for "Info" (one line only) */
		if (buf[0] == 'I')
		{
			int spd, hp1, hp2, aaf, ac, slp;

			/* Scan for the other values */
			if (6 != sscanf(buf+2, "%d:%dd%d:%d:%d:%d",
				&spd, &hp1, &hp2, &aaf, &ac, &slp)) return (1);

			/* Save the values */
			r_ptr->speed = spd;
			r_ptr->hdice = hp1;
			r_ptr->hside = hp2;
			r_ptr->aaf = aaf;
			r_ptr->ac = ac;
			r_ptr->sleep = slp;

			/* Next... */
			continue;
		}

		/* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
			int lev, rar, pad;
			long exp;
			long nextexp;
			int nextmon;

			/* Scan for the values */
			if (6 != sscanf(buf+2, "%d:%d:%d:%ld:%ld:%d",
				&lev, &rar, &pad, &exp, &nextexp, &nextmon)) return (1);

			/* Save the values */
			r_ptr->level = lev;
			r_ptr->rarity = rar;
			r_ptr->extra = pad;
			r_ptr->mexp = exp;
			r_ptr->next_exp = nextexp;
			r_ptr->next_r_idx = nextmon;

			/* Next... */
			continue;
		}

		/* Process 'B' for "Blows" (up to four lines) */
		if (buf[0] == 'B')
		{
			int n1, n2;

			/* Find the next empty blow slot (if any) */
			for (i = 0; i < 4; i++) if (!r_ptr->blow[i].method) break;

			/* Oops, no more slots */
			if (i == 4) return (1);

			/* Analyze the first field */
			for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

			/* Terminate the field (if necessary) */
			if (*t == ':') *t++ = '\0';

			/* Analyze the method */
			for (n1 = 0; r_info_blow_method[n1]; n1++)
			{
				if (streq(s, r_info_blow_method[n1])) break;
			}

			/* Invalid method */
			if (!r_info_blow_method[n1]) return (1);

			/* Analyze the second field */
			for (s = t; *t && (*t != ':'); t++) /* loop */;

			/* Terminate the field (if necessary) */
			if (*t == ':') *t++ = '\0';

			/* Analyze effect */
			for (n2 = 0; r_info_blow_effect[n2]; n2++)
			{
				if (streq(s, r_info_blow_effect[n2])) break;
			}

			/* Invalid effect */
			if (!r_info_blow_effect[n2]) return (1);

			/* Analyze the third field */
			for (s = t; *t && (*t != 'd'); t++) /* loop */;

			/* Terminate the field (if necessary) */
			if (*t == 'd') *t++ = '\0';

			/* Save the method */
			r_ptr->blow[i].method = n1;

			/* Save the effect */
			r_ptr->blow[i].effect = n2;

			/* Extract the damage dice and sides */
			r_ptr->blow[i].d_dice = atoi(s);
			r_ptr->blow[i].d_side = atoi(t);

			/* Next... */
			continue;
		}

		/* Process 'F' for "Basic Flags" (multiple lines) */
		if (buf[0] == 'F')
		{
			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while (*t == ' ' || *t == '|') t++;
				}

				/* Parse this entry */
				if (0 != grab_one_basic_flag(r_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Process 'S' for "Spell Flags" (multiple lines) */
		if (buf[0] == 'S')
		{
			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* XXX XXX XXX Hack -- Read spell frequency */
				if (1 == sscanf(s, "1_IN_%d", &i))
				{
					/* Extract a "frequency" */
					r_ptr->freq_spell = r_ptr->freq_inate = 100 / i;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

				/* Parse this entry */
				if (0 != grab_one_spell_flag(r_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Oops */
		return (6);
	}


	/* Complete the "name" and "text" sizes */
	++r_head->name_size;
#ifdef JP
	/* �Ѹ�̾�� */
	++r_head->E_name_size;
#endif
	++r_head->text_size;


	for (i = 1; i < max_r_idx; i++)
	{
		/* Invert flag WILD_ONLY <-> RF8_DUNGEON */
		r_info[i].flags8 ^= 1L;

		/* WILD_TOO without any other wilderness flags enables all flags */
		if ((r_info[i].flags8 & RF8_WILD_TOO) && !(r_info[i].flags8 & 0x7FFFFFFE))
			r_info[i].flags8 = 0x0463;
	}

	/* Success */
	return (0);
}


/*
 * Grab one flag for a dungeon type from a textual string
 */
static errr grab_one_dungeon_flag(dungeon_info_type *d_ptr, cptr what)
{
	int i;

	/* Scan flags1 */
	for (i = 0; i < 32; i++)
	{
                if (streq(what, d_info_flags1[i]))
		{
                        d_ptr->flags1 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
        msg_format("̤�ΤΥ��󥸥�󡦥ե饰 '%s'��", what);
#else
        msg_format("Unknown dungeon type flag '%s'.", what);
#endif

	/* Failure */
	return (1);
}

/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
static errr grab_one_basic_monster_flag(dungeon_info_type *d_ptr, cptr what)
{
	int i;

	/* Scan flags1 */
	for (i = 0; i < 32; i++)
	{
                if (streq(what, r_info_flags1[i]))
		{
                        d_ptr->mflags1 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags2 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags2[i]))
		{
                        d_ptr->mflags2 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags3 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags3[i]))
		{
                        d_ptr->mflags3 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags7 */
	for (i = 0; i < 32; i++)
	{
                if (streq(what, r_info_flags7[i]))
		{
                        d_ptr->mflags7 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags8 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags8[i]))
		{
                        d_ptr->mflags8 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags9 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags9[i]))
		{
                        d_ptr->mflags9 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�ΤΥ�󥹥������ե饰 '%s'��", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif
	/* Failure */
	return (1);
}


/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_spell_monster_flag(dungeon_info_type *d_ptr, cptr what)
{
	int i;

	/* Scan flags4 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags4[i]))
		{
                        d_ptr->mflags4 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags5 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags5[i]))
		{
                        d_ptr->mflags5 |= (1L << i);
			return (0);
		}
	}

	/* Scan flags6 */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, r_info_flags6[i]))
		{
                        d_ptr->mflags6 |= (1L << i);
			return (0);
		}
	}

	/* Oops */
#ifdef JP
	msg_format("̤�ΤΥ�󥹥������ե饰 '%s'��", what);
#else
	msg_format("Unknown monster flag '%s'.", what);
#endif

	/* Failure */
	return (1);
}

/*
 * Initialize the "d_info" array, by parsing an ascii "template" file
 */
errr init_d_info_txt(FILE *fp, char *buf)
{
	int i;

	char *s, *t;

	/* Current entry */
	dungeon_info_type *d_ptr = NULL;


	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = -1;


	/* Start the "fake" stuff */
	d_head->name_size = 0;
	d_head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (1);


		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			/* ignore */
			continue;
		}

		/* Process 'N' for "New/Number/Name" */
		if (buf[0] == 'N')
		{
			/* Find the colon before the name */
			s = strchr(buf+2, ':');

			/* Verify that colon */
			if (!s) return (1);

			/* Nuke the colon, advance to the name */
			*s++ = '\0';
#ifdef JP
			/* Paranoia -- require a name */
			if (!*s) return (1);
#endif
			/* Get the index */
			i = atoi(buf+2);

			/* Verify information */
			if (i < error_idx) return (4);

			/* Verify information */
                        if (i >= d_head->info_num) return (2);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
                        d_ptr = &d_info[i];
#ifdef JP
			/* Hack -- Verify space */
                        if (d_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
                        if (!d_ptr->name) d_ptr->name = ++d_head->name_size;

			/* Append chars to the name */
                        strcpy(d_name + d_head->name_size, s);

			/* Advance the index */
                        d_head->name_size += strlen(s);
#endif
			/* Next... */
			continue;
		}

#ifdef JP
		if (buf[0] == 'E')
			continue;
#else
		if (buf[0] == 'E')
		{
			/* Acquire the Text */
			s = buf+2;

			/* Hack -- Verify space */
                        if (d_head->name_size + strlen(s) + 8 > fake_name_size) return (7);

			/* Advance and Save the name index */
                        if (!d_ptr->name) d_ptr->name = ++d_head->name_size;

			/* Append chars to the name */
                        strcpy(d_name + d_head->name_size, s);

			/* Advance the index */
                        d_head->name_size += strlen(s);

			/* Next... */
			continue;
		}
#endif

                /* Process 'D' for "Description */
                if (buf[0] == 'D')
		{
#ifdef JP
			if (buf[2] == '$')
				continue;
			/* Acquire the text */
			s = buf+2;
#else
			if (buf[2] != '$')
				continue;
			/* Acquire the text */
			s = buf+3;
#endif

			/* Hack -- Verify space */
                        if (d_head->text_size + strlen(s) + 8 > fake_text_size) return (7);

			/* Advance and Save the text index */
                        if (!d_ptr->text) d_ptr->text = ++d_head->text_size;

			/* Append chars to the name */
                        strcpy(d_text + d_head->text_size, s);

			/* Advance the index */
                        d_head->text_size += strlen(s);

			/* Next... */
			continue;
		}

                /* Process 'W' for "More Info" (one line only) */
		if (buf[0] == 'W')
		{
                        int min_lev, max_lev;
                        int min_plev, mode;
                        int min_alloc, max_chance;
                        int obj_good, obj_great;
			int pit, nest;

			/* Scan for the values */
			if (10 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%x:%x",
                                &min_lev, &max_lev, &min_plev, &mode, &min_alloc, &max_chance, &obj_good, &obj_great, (unsigned int *)&pit, (unsigned int *)&nest)) return (1);

			/* Save the values */
                        d_ptr->mindepth = min_lev;
                        d_ptr->maxdepth = max_lev;
                        d_ptr->min_plev = min_plev;
                        d_ptr->mode = mode;
                        d_ptr->min_m_alloc_level = min_alloc;
                        d_ptr->max_m_alloc_chance = max_chance;
                        d_ptr->obj_good = obj_good;
                        d_ptr->obj_great = obj_great;
                        d_ptr->pit = pit;
                        d_ptr->nest = nest;

                        /* Next... */
			continue;
		}

                /* Process 'P' for "Place Info" */
		if (buf[0] == 'P')
		{
                        int dy, dx;

			/* Scan for the values */
			if (2 != sscanf(buf+2, "%d:%d", &dy, &dx)) return (1);

			/* Save the values */
                        d_ptr->dy = dy;
                        d_ptr->dx = dx;

                        /* Next... */
			continue;
		}

                /* Process 'L' for "fLoor type" (one line only) */
                if (buf[0] == 'L')
		{
                        int f1, f2, f3;
                        int p1, p2, p3;
			int tunnel;

			/* Scan for the values */
                        if (7 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d",
                                &f1, &p1, &f2, &p2, &f3, &p3, &tunnel)) return (1);

			/* Save the values */
                        d_ptr->floor1 = f1;
                        d_ptr->floor_percent1 = p1;
                        d_ptr->floor2 = f2;
                        d_ptr->floor_percent2 = p2;
                        d_ptr->floor3 = f3;
                        d_ptr->floor_percent3 = p3;
			d_ptr->tunnel_percent = tunnel;

			/* Next... */
			continue;
		}

                /* Process 'A' for "wAll type" (one line only) */
                if (buf[0] == 'A')
		{
                        int w1, w2, w3, outer, inner, stream1, stream2;
                        int p1, p2, p3;

			/* Scan for the values */
			if (10 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
                                &w1, &p1, &w2, &p2, &w3, &p3, &outer, &inner, &stream1, &stream2)) return (1);

			/* Save the values */
                        d_ptr->fill_type1 = w1;
                        d_ptr->fill_percent1 = p1;
                        d_ptr->fill_type2 = w2;
                        d_ptr->fill_percent2 = p2;
                        d_ptr->fill_type3 = w3;
                        d_ptr->fill_percent3 = p3;
                        d_ptr->outer_wall = outer;
                        d_ptr->inner_wall = inner;
			d_ptr->stream1 = stream1;
			d_ptr->stream2 = stream2;

			/* Next... */
			continue;
		}

                /* Process 'F' for "Dungeon Flags" (multiple lines) */
		if (buf[0] == 'F')
		{
                        int artif = 0, monst = 0;

			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while (*t == ' ' || *t == '|') t++;
				}

                                /* XXX XXX XXX Hack -- Read Final Artifact */
                                if (1 == sscanf(s, "FINAL_ARTIFACT_%d", &artif))
                                {
                                        /* Extract a "Final Artifact" */
                                        d_ptr->final_artifact = artif;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

                                /* XXX XXX XXX Hack -- Read Final Object */
				if (1 == sscanf(s, "FINAL_OBJECT_%d", &artif))
                                {
                                        /* Extract a "Final Artifact" */
                                        d_ptr->final_object = artif;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

                                /* XXX XXX XXX Hack -- Read Artifact Guardian */
                                if (1 == sscanf(s, "FINAL_GUARDIAN_%d", &monst))
                                {
                                        /* Extract a "Artifact Guardian" */
                                        d_ptr->final_guardian = monst;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

                                /* XXX XXX XXX Hack -- Read Special Percentage */
                                if (1 == sscanf(s, "MONSTER_DIV_%d", &monst))
                                {
                                        /* Extract a "Special %" */
                                        d_ptr->special_div = monst;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

				/* Parse this entry */
                                if (0 != grab_one_dungeon_flag(d_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

                /* Process 'M' for "Basic Flags" (multiple lines) */
                if (buf[0] == 'M')
		{
                        byte r_char_number = 0, r_char;

			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while (*t == ' ' || *t == '|') t++;
				}

                                /* XXX XXX XXX Hack -- Read monster symbols */
                                if (1 == sscanf(s, "R_CHAR_%c", &r_char))
                                {
                                        /* Limited to 5 races */
                                        if(r_char_number >= 5) continue;

					/* Extract a "frequency" */
                                        d_ptr->r_char[r_char_number++] = r_char;

					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

				/* Parse this entry */
                                if (0 != grab_one_basic_monster_flag(d_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Process 'S' for "Spell Flags" (multiple lines) */
		if (buf[0] == 'S')
		{
			/* Parse every entry */
			for (s = buf + 2; *s; )
			{
				/* Find the end of this entry */
				for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

				/* Nuke and skip any dividers */
				if (*t)
				{
					*t++ = '\0';
					while ((*t == ' ') || (*t == '|')) t++;
				}

				/* XXX XXX XXX Hack -- Read spell frequency */
				if (1 == sscanf(s, "1_IN_%d", &i))
				{
					/* Start at next entry */
					s = t;

					/* Continue */
					continue;
				}

				/* Parse this entry */
                                if (0 != grab_one_spell_monster_flag(d_ptr, s)) return (5);

				/* Start the next entry */
				s = t;
			}

			/* Next... */
			continue;
		}

		/* Oops */
		return (6);
	}

        ++d_head->text_size;

	/* Success */
	return (0);
}


#else	/* ALLOW_TEMPLATES */

#ifdef MACINTOSH
static int i = 0;
#endif

#endif	/* ALLOW_TEMPLATES */


/* Random dungeon grid effects */
#define RANDOM_NONE         0x00
#define RANDOM_FEATURE      0x01
#define RANDOM_MONSTER      0x02
#define RANDOM_OBJECT       0x04
#define RANDOM_EGO          0x08
#define RANDOM_ARTIFACT     0x10
#define RANDOM_TRAP         0x20


typedef struct dungeon_grid dungeon_grid;

struct dungeon_grid
{
	int		feature;		/* Terrain feature */
	int		monster;		/* Monster */
	int		object;			/* Object */
	int		ego;			/* Ego-Item */
	int		artifact;		/* Artifact */
	int		trap;			/* Trap */
	int		cave_info;		/* Flags for CAVE_MARK, CAVE_GLOW, CAVE_ICKY, CAVE_ROOM */
	int		special;		/* Reserved for special terrain info */
	int		random;			/* Number of the random effect */
};


static dungeon_grid letter[255];


/*
 * Process "F:<letter>:<terrain>:<cave_info>:<monster>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid
 */
static errr parse_line_feature(char *buf)
{
	int num;
	char *zz[9];


	if (init_flags & INIT_ONLY_BUILDINGS) return (0);

	/* Tokenize the line */
	if ((num = tokenize(buf+2, 9, zz, 0)) > 1)
	{
		/* Letter to assign */
		int index = zz[0][0];

		/* Reset the info for the letter */
		letter[index].feature = 0;
		letter[index].monster = 0;
		letter[index].object = 0;
		letter[index].ego = 0;
		letter[index].artifact = 0;
		letter[index].trap = 0;
		letter[index].cave_info = 0;
		letter[index].special = 0;
		letter[index].random = 0;

		switch (num)
		{
			/* Special */
			case 9:
				letter[index].special = atoi(zz[8]);
				/* Fall through */
			/* Trap */
			case 8:
				if (zz[7][0] == '*')
				{
					letter[index].random |= RANDOM_TRAP;

					if (zz[7][1])
					{
						zz[7]++;
						letter[index].trap = atoi(zz[7]);
					}
				}
				else
				{
					letter[index].trap = atoi(zz[7]);
				}
				/* Fall through */
			/* Artifact */
			case 7:
				if (zz[6][0] == '*')
				{
					letter[index].random |= RANDOM_ARTIFACT;

					if (zz[6][1])
					{
						zz[6]++;
						letter[index].artifact = atoi(zz[6]);
					}
				}
				else
				{
					letter[index].artifact = atoi(zz[6]);
				}
				/* Fall through */
			/* Ego-item */
			case 6:
				if (zz[5][0] == '*')
				{
					letter[index].random |= RANDOM_EGO;

					if (zz[5][1])
					{
						zz[5]++;
						letter[index].ego = atoi(zz[5]);
					}
				}
				else
				{
					letter[index].ego = atoi(zz[5]);
				}
				/* Fall through */
			/* Object */
			case 5:
				if (zz[4][0] == '*')
				{
					letter[index].random |= RANDOM_OBJECT;

					if (zz[4][1])
					{
						zz[4]++;
						letter[index].object = atoi(zz[4]);
					}
				}
				else
				{
					letter[index].object = atoi(zz[4]);
				}
				/* Fall through */
			/* Monster */
			case 4:
				if (zz[3][0] == '*')
				{
					letter[index].random |= RANDOM_MONSTER;
					if (zz[3][1])
					{
						zz[3]++;
						letter[index].monster = atoi(zz[3]);
					}
				}
				else
				{
					letter[index].monster = atoi(zz[3]);
				}
				/* Fall through */
			/* Cave info */
			case 3:
				letter[index].cave_info = atoi(zz[2]);
				/* Fall through */
			/* Feature */
			case 2:
				if (zz[1][0] == '*')
				{
					letter[index].random |= RANDOM_FEATURE;
					if (zz[1][1])
					{
						zz[1]++;
						letter[index].feature = atoi(zz[1]);
					}
				}
				else
				{
					letter[index].feature = atoi(zz[1]);
				}
				break;
		}

		return (0);
	}

	return (1);
}


/*
 * Process "F:<letter>:<terrain>:<cave_info>:<monster>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid
 */
static errr parse_line_building(char *buf)
{
	int i;
	char *zz[37];
	int index;
	char *s;

#ifdef JP
	if (buf[2] == '$')
		return 0;
	s = buf + 2;
#else
	if (buf[2] != '$')
		return 0;
	s = buf + 3;
#endif
	/* Get the building number */
	index = atoi(s);

	/* Find the colon after the building number */
	s = strchr(s, ':');

	/* Verify that colon */
	if (!s) return (1);

	/* Nuke the colon, advance to the sub-index */
	*s++ = '\0';

	/* Paranoia -- require a sub-index */
	if (!*s) return (1);

	/* Building definition sub-index */
	switch (s[0])
	{
		/* Building name, owner, race */
		case 'N':
		{
			if (tokenize(s + 2, 3, zz, 0) == 3)
			{
				/* Name of the building */
				strcpy(building[index].name, zz[0]);

				/* Name of the owner */
				strcpy(building[index].owner_name, zz[1]);

				/* Race of the owner */
				strcpy(building[index].owner_race, zz[2]);

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Action */
		case 'A':
		{
			if (tokenize(s + 2, 8, zz, 0) >= 7)
			{
				/* Index of the action */
				int action_index = atoi(zz[0]);

				/* Name of the action */
				strcpy(building[index].act_names[action_index], zz[1]);

				/* Cost of the action for members */
				building[index].member_costs[action_index] = atoi(zz[2]);

				/* Cost of the action for non-members */
				building[index].other_costs[action_index] = atoi(zz[3]);

				/* Letter assigned to the action */
				building[index].letters[action_index] = zz[4][0];

				/* Action code */
				building[index].actions[action_index] = atoi(zz[5]);

				/* Action restriction */
				building[index].action_restr[action_index] = atoi(zz[6]);

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Classes */
		case 'C':
		{
			if (tokenize(s + 2, MAX_CLASS, zz, 0) == MAX_CLASS)
			{
				for (i = 0; i < MAX_CLASS; i++)
				{
					building[index].member_class[i] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Races */
		case 'R':
		{
			if (tokenize(s+2, MAX_RACES, zz, 0) == MAX_RACES)
			{
				for (i = 0; i < MAX_RACES; i++)
				{
					building[index].member_race[i] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		/* Building Realms */
		case 'M':
		{
			if (tokenize(s+2, MAX_MAGIC, zz, 0) == MAX_MAGIC)
			{
				for (i = 0; i < MAX_MAGIC; i++)
				{
					building[index].member_realm[i+1] = atoi(zz[i]);
				}

				break;
			}

			return (PARSE_ERROR_TOO_FEW_ARGUMENTS);
		}

		case 'Z':
		{
#ifdef USE_SCRIPT
			if (tokenize(s+2, 2, zz, 0) == 2)
			{
				/* Index of the action */
				int action_index = atoi(zz[0]);

				/* Name of the action */
				strcpy(building[index].act_script[action_index], zz[1]);

				break;
			}
			return (1);
#else /* USE_SCRIPT */
			/* Ignore scripts */
			break;
#endif /* USE_SCRIPT */
		}

		default:
		{
			return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
		}
	}

	return (0);
}


/*
 * Parse a sub-file of the "extra info"
 */
static errr process_dungeon_file_aux(char *buf, int ymin, int xmin, int ymax, int xmax, int *y, int *x)
{
	int i;

	char *zz[33];


	/* Skip "empty" lines */
	if (!buf[0]) return (0);

	/* Skip "blank" lines */
	if (isspace(buf[0])) return (0);

	/* Skip comments */
	if (buf[0] == '#') return (0);

	/* Require "?:*" format */
	if (buf[1] != ':') return (1);


	/* Process "%:<fname>" */
	if (buf[0] == '%')
	{
		/* Attempt to Process the given file */
		return (process_dungeon_file(buf + 2, ymin, xmin, ymax, xmax));
	}

	/* Process "F:<letter>:<terrain>:<cave_info>:<monster>:<object>:<ego>:<artifact>:<trap>:<special>" -- info for dungeon grid */
	if (buf[0] == 'F')
	{
		return parse_line_feature(buf);
	}

	/* Process "D:<dungeon>" -- info for the cave grids */
	else if (buf[0] == 'D')
	{
		object_type object_type_body;

		/* Acquire the text */
		char *s = buf + 2;

		/* Length of the text */
		int len = strlen(s);

		if (init_flags & INIT_ONLY_BUILDINGS) return (0);

		for (*x = xmin, i = 0; ((*x < xmax) && (i < len)); (*x)++, s++, i++)
		{
			/* Access the grid */
			cave_type *c_ptr = &cave[*y][*x];

			int idx = s[0];

			int object_index = letter[idx].object;
			int monster_index = letter[idx].monster;
			int random = letter[idx].random;
			int artifact_index = letter[idx].artifact;

			/* Lay down a floor */
			c_ptr->feat = letter[idx].feature;

			/* Only the features */
			if (init_flags & INIT_ONLY_FEATURES) continue;

			/* Cave info */
			c_ptr->info = letter[idx].cave_info;

			/* Create a monster */
			if (random & RANDOM_MONSTER)
			{
				monster_level = base_level + monster_index;

				place_monster(*y, *x, TRUE, TRUE);

				monster_level = base_level;
			}
			else if (monster_index)
			{
				/* Make alive again */
				if (r_info[monster_index].flags1 & RF1_UNIQUE)
				{
					r_info[monster_index].cur_num = 0;
					r_info[monster_index].max_num = 1;
				}

				/* Make alive again */
				if (r_info[monster_index].flags7 & RF7_UNIQUE_7)
				{
					if (r_info[monster_index].cur_num == r_info[monster_index].max_num)
					{
						r_info[monster_index].max_num++;
					}
				}

				/* Place it */
				place_monster_aux(*y, *x, monster_index, TRUE, FALSE, FALSE, FALSE, TRUE, FALSE);
			}

			/* Object (and possible trap) */
			if ((random & RANDOM_OBJECT) && (random & RANDOM_TRAP))
			{
				object_level = base_level + object_index;

				/*
				 * Random trap and random treasure defined
				 * 25% chance for trap and 75% chance for object
				 */
				if (rand_int(100) < 75)
				{
					place_object(*y, *x, FALSE, FALSE);
				}
				else
				{
					place_trap(*y, *x);
				}

				object_level = base_level;
			}
			else if (random & RANDOM_OBJECT)
			{
				object_level = base_level + object_index;

				/* Create an out of deep object */
				if (rand_int(100) < 75)
					place_object(*y, *x, FALSE, FALSE);
				else if (rand_int(100) < 80)
					place_object(*y, *x, TRUE, FALSE);
				else
					place_object(*y, *x, TRUE, TRUE);

				object_level = base_level;
			}
			/* Random trap */
			else if (random & RANDOM_TRAP)
			{
				place_trap(*y, *x);
			}
			else if (object_index)
			{
				/* Get local object */
				object_type *o_ptr = &object_type_body;

				/* Create the item */
				object_prep(o_ptr, object_index);

				if (o_ptr->tval == TV_GOLD)
				{
					coin_type = object_index - OBJ_GOLD_LIST;
					make_gold(o_ptr);
					coin_type = 0;
				}

				/* Apply magic (no messages, no artifacts) */
				apply_magic(o_ptr, base_level, FALSE, TRUE, FALSE, FALSE);

#ifdef USE_SCRIPT
				o_ptr->python = object_create_callback(o_ptr);
#endif /* USE_SCRIPT */

				(void)drop_near(o_ptr, -1, *y, *x);
			}

			/* Artifact */
			if (artifact_index)
			{
				if (a_info[artifact_index].cur_num)
				{
					s16b k_idx = 198;
					object_type forge;
					object_type *q_ptr = &forge;

					object_prep(q_ptr, k_idx);
#ifdef USE_SCRIPT
					q_ptr->python = object_create_callback(q_ptr);
#endif /* USE_SCRIPT */

					/* Drop it in the dungeon */
					(void)drop_near(q_ptr, -1, *y, *x);
				}
				else
				{
					/* Create the artifact */
					create_named_art(artifact_index, *y, *x);

					a_info[artifact_index].cur_num = 1;
				}
			}

			/* Terrain special */
			c_ptr->special = letter[idx].special;
		}

		(*y)++;

		return (0);
	}

	/* Process "Q:<number>:<command>:... -- quest info */
	else if (buf[0] == 'Q')
	{
		int num;
		quest_type *q_ptr;
#ifdef JP
		if (buf[2] == '$')
			return 0;
		num = tokenize(buf + 2, 33, zz, 0);
#else
		if (buf[2] != '$')
			return 0;
		num = tokenize(buf + 3, 33, zz, 0);
#endif

		/* Have we enough parameters? */
		if (num < 3) return (PARSE_ERROR_TOO_FEW_ARGUMENTS);

		/* Get the quest */
		q_ptr = &(quest[atoi(zz[0])]);

		/* Process "Q:<q_index>:Q:<type>:<num_mon>:<cur_num>:<max_num>:<level>:<r_idx>:<k_idx>:<flags>" -- quest info */
		if (zz[1][0] == 'Q')
		{
			if (init_flags & INIT_ASSIGN)
			{
				monster_race *r_ptr;
				artifact_type *a_ptr;

				if (num < 9) return (PARSE_ERROR_TOO_FEW_ARGUMENTS);

				q_ptr->type    = atoi(zz[2]);
				q_ptr->num_mon = atoi(zz[3]);
				q_ptr->cur_num = atoi(zz[4]);
				q_ptr->max_num = atoi(zz[5]);
				q_ptr->level   = atoi(zz[6]);
				q_ptr->r_idx   = atoi(zz[7]);
				q_ptr->k_idx   = atoi(zz[8]);
				q_ptr->dungeon = atoi(zz[9]);

				if (num > 10)
					q_ptr->flags  = atoi(zz[10]);

				r_ptr = &r_info[q_ptr->r_idx];
				if (r_ptr->flags1 & RF1_UNIQUE)
					r_ptr->flags1 |= RF1_QUESTOR;

				a_ptr = &a_info[q_ptr->k_idx];
				a_ptr->flags3 |= TR3_QUESTITEM;
			}
			return (0);
		}

		/* Process "Q:<q_index>:N:<name>" -- quest name */
		else if (zz[1][0] == 'N')
		{
			if (init_flags & (INIT_ASSIGN | INIT_SHOW_TEXT))
			{
				strcpy(q_ptr->name, zz[2]);
			}

			return (0);
		}

		/* Process "Q:<q_index>:T:<text>" -- quest description line */
		else if (zz[1][0] == 'T')
		{
			if (init_flags & INIT_SHOW_TEXT)
			{
				strcpy(quest_text[quest_text_line], zz[2]);
				quest_text_line++;
			}

			return (0);
		}
	}

	/* Process "W:<command>: ..." -- info for the wilderness */
	else if (buf[0] == 'W')
	{
		return parse_line_wilderness(buf, ymin, xmin, ymax, xmax, y, x);
	}

	/* Process "P:<y>:<x>" -- player position */
	else if (buf[0] == 'P')
	{
		if (init_flags & INIT_CREATE_DUNGEON)
		{
			if (tokenize(buf + 2, 2, zz, 0) == 2)
			{
				int panels_x, panels_y;

				/* Hack - Set the dungeon size */
				panels_y = (*y / SCREEN_HGT);
				if (*y % SCREEN_HGT) panels_y++;
				cur_hgt = panels_y * SCREEN_HGT;

				panels_x = (*x / SCREEN_WID);
				if (*x % SCREEN_WID) panels_x++;
				cur_wid = panels_x * SCREEN_WID;

				/* Choose a panel row */
				max_panel_rows = (cur_hgt / SCREEN_HGT) * 2 - 2;
				if (max_panel_rows < 0) max_panel_rows = 0;

				/* Choose a panel col */
				max_panel_cols = (cur_wid / SCREEN_WID) * 2 - 2;
				if (max_panel_cols < 0) max_panel_cols = 0;

				/* Assume illegal panel */
				panel_row = max_panel_rows;
				panel_col = max_panel_cols;

				/* Place player in a quest level */
				if (p_ptr->inside_quest)
				{
					int y, x;

					/* Delete the monster (if any) */
					delete_monster(py, px);

					y = atoi(zz[0]);
					x = atoi(zz[1]);

					py = y;
					px = x;
				}
				/* Place player in the town */
				else if (!p_ptr->oldpx && !p_ptr->oldpy)
				{
					p_ptr->oldpy = atoi(zz[0]);
					p_ptr->oldpx = atoi(zz[1]);
				}
			}
		}

		return (0);
	}

	/* Process "B:<Index>:<Command>:..." -- Building definition */
	else if (buf[0] == 'B')
	{
		return parse_line_building(buf);
	}

	/* Process "M:<type>:<maximum>" -- set maximum values */
	else if (buf[0] == 'M')
	{
		if (tokenize(buf+2, 2, zz, 0) == 2)
		{
			/* Maximum towns */
			if (zz[0][0] == 'T')
			{
				max_towns = atoi(zz[1]);
			}

			/* Maximum quests */
			else if (zz[0][0] == 'Q')
			{
				max_quests = atoi(zz[1]);
			}

			/* Maximum r_idx */
			else if (zz[0][0] == 'R')
			{
				max_r_idx = atoi(zz[1]);
			}

			/* Maximum k_idx */
			else if (zz[0][0] == 'K')
			{
				max_k_idx = atoi(zz[1]);
			}

			/* Maximum v_idx */
			else if (zz[0][0] == 'V')
			{
				max_v_idx = atoi(zz[1]);
			}

			/* Maximum f_idx */
			else if (zz[0][0] == 'F')
			{
				max_f_idx = atoi(zz[1]);
			}

			/* Maximum a_idx */
			else if (zz[0][0] == 'A')
			{
				max_a_idx = atoi(zz[1]);
			}

			/* Maximum e_idx */
			else if (zz[0][0] == 'E')
			{
				max_e_idx = atoi(zz[1]);
			}

                        /* Maximum d_idx */
                        else if (zz[0][0] == 'D')
			{
                                max_d_idx = atoi(zz[1]); 
			}

			/* Maximum o_idx */
			else if (zz[0][0] == 'O')
			{
				max_o_idx = atoi(zz[1]);
			}

			/* Maximum m_idx */
			else if (zz[0][0] == 'M')
			{
				max_m_idx = atoi(zz[1]);
			}

			/* Wilderness size */
			else if (zz[0][0] == 'W')
			{
				/* Maximum wild_x_size */
				if (zz[0][1] == 'X')
					max_wild_x = atoi(zz[1]);
				/* Maximum wild_y_size */
				if (zz[0][1] == 'Y')
					max_wild_y = atoi(zz[1]);
			}

			return (0);
		}
	}


	/* Failure */
	return (1);
}


static char tmp[8];
static cptr variant = "ZANGBAND";


/*
 * Helper function for "process_dungeon_file()"
 */
static cptr process_dungeon_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';

	/* Initial */
	s = (*sp);

	/* Skip spaces */
	while (isspace(*s)) s++;

	/* Save start */
	b = s;

	/* Default */
	v = "?o?o?";

	/* Analyze */
	if (*s == b1)
	{
		const char *p;
		const char *t;

		/* Skip b1 */
		s++;

		/* First */
		t = process_dungeon_file_expr(&s, &f);

		/* Oops */
		if (!*t)
		{
			/* Nothing */
		}

		/* Function: IOR */
		else if (streq(t, "IOR"))
		{
			v = "0";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
				if (*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if (streq(t, "EQU"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(&s, &f);
				if (*t && !streq(p, t)) v = "0";
			}
		}

		/* Function: LEQ */
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(&s, &f);
				if (*t && (strcmp(p, t) > 0)) v = "0";
			}
		}

		/* Function: GEQ */
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_dungeon_file_expr(&s, &f);
				if (*t && (strcmp(p, t) < 0)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_dungeon_file_expr(&s, &f);
			}
		}

		/* Verify ending */
		if (f != b2) v = "?x?x?";

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';
	}

	/* Other */
	else
	{
		/* Accept all printables except spaces and brackets */
		while (isprint(*s) && !strchr(" []", *s)) ++s;

		/* Extract final and Terminate */
		if ((f = *s) != '\0') *s++ = '\0';

		/* Variable */
		if (*b == '$')
		{
			/* System */
			if (streq(b+1, "SYS"))
			{
				v = ANGBAND_SYS;
			}

			/* Graphics */
			else if (streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

			else if (streq(b+1, "MONOCHROME"))
			{
				if (arg_monochrome)
					v = "ON";
				else
					v = "OFF";
			}

			/* Race */
			else if (streq(b+1, "RACE"))
			{
#ifdef JP
                                v = rp_ptr->E_title;
#else
				v = rp_ptr->title;
#endif
			}

			/* Class */
			else if (streq(b+1, "CLASS"))
			{
#ifdef JP
                                v = cp_ptr->E_title;
#else
				v = cp_ptr->title;
#endif
			}

			/* First realm */
			else if (streq(b+1, "REALM1"))
			{
#ifdef JP
				v = E_realm_names[p_ptr->realm1];
#else
                                v = realm_names[p_ptr->realm1];
#endif
			}

			/* Second realm */
			else if (streq(b+1, "REALM2"))
			{
#ifdef JP
                                v = E_realm_names[p_ptr->realm2];
#else
				v = realm_names[p_ptr->realm2];
#endif
			}

			/* Player name */
			else if (streq(b+1, "PLAYER"))
			{
				v = player_base;
			}

			/* Town */
			else if (streq(b+1, "TOWN"))
			{
				sprintf(tmp, "%d", p_ptr->town_num);
				v = tmp;
			}

			/* Level */
			else if (streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%d", p_ptr->lev);
				v = tmp;
			}

			/* Current quest number */
			else if (streq(b+1, "QUEST_NUMBER"))
			{
				sprintf(tmp, "%d", p_ptr->inside_quest);
				v = tmp;
			}

			/* Number of last quest */
			else if (streq(b+1, "LEAVING_QUEST"))
			{
				sprintf(tmp, "%d", leaving_quest);
				v = tmp;
			}

			/* Quest status */
			else if (prefix(b+1, "QUEST"))
			{
				/* "QUEST" uses a special parameter to determine the number of the quest */
				sprintf(tmp, "%d", quest[atoi(b+6)].status);
				v = tmp;
			}

			/* Random */
			else if (prefix(b+1, "RANDOM"))
			{
				/* "RANDOM" uses a special parameter to determine the number of the quest */
				sprintf(tmp, "%d", (int)(seed_town%atoi(b+7)));
				v = tmp;
			}

			/* Variant name */
			else if (streq(b+1, "VARIANT"))
			{
				v = variant;
			}

			/* Wilderness */
			else if (streq(b+1, "WILDERNESS"))
			{
				if (vanilla_town)
					sprintf(tmp, "NONE");
				else if (lite_town)
					sprintf(tmp, "LITE");
				else
					sprintf(tmp, "NORMAL");
				v = tmp;
			}
		}

		/* Constant */
		else
		{
			v = b;
		}
	}

	/* Save */
	(*fp) = f;

	/* Save */
	(*sp) = s;

	/* Result */
	return (v);
}


errr process_dungeon_file(cptr name, int ymin, int xmin, int ymax, int xmax)
{
	FILE *fp;

	char buf[1024];

	int num = -1;

	errr err = 0;

	bool bypass = FALSE;

	int x = xmin, y = ymin;


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_EDIT, name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No such file */
	if (!fp) return (-1);


	/* Process the file */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Count lines */
		num++;


		/* Skip "empty" lines */
		if (!buf[0]) continue;

		/* Skip "blank" lines */
		if (isspace(buf[0])) continue;

		/* Skip comments */
		if (buf[0] == '#') continue;


		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_dungeon_file_expr(&s, &f);

			/* Set flag */
			bypass = (streq(v, "0") ? TRUE : FALSE);

			/* Continue */
			continue;
		}

		/* Apply conditionals */
		if (bypass) continue;


		/* Process "%:<file>" */
		if (buf[0] == '%')
		{
			/* Process that file if allowed */
			(void)process_dungeon_file(buf + 2, ymin, xmin, ymax, xmax);

			/* Continue */
			continue;
		}


		/* Process the line */
		err = process_dungeon_file_aux(buf, ymin, xmin, ymax, xmax, &y, &x);

		/* Oops */
		if (err) break;
	}

	/* Errors */
	if (err)
	{
		cptr oops;

		/* Error string */
		oops = (((err > 0) && (err < PARSE_ERROR_MAX)) ? err_str[err] : "unknown");

		/* Oops */
		msg_format("Error %d (%s) at line %d of '%s'.", err, oops, num, name);
#ifdef JP
msg_format("'%s'������档", buf);
#else
		msg_format("Parsing '%s'.", buf);
#endif

		msg_print(NULL);
	}


	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



void write_r_info_txt(void)
{
	int i, j, z, fc, bc;
	int dlen;

	cptr flags[288];

	u32b f_ptr[9];
	cptr *n_ptr[9];

	monster_race *r_ptr;

	monster_blow *b_ptr;

	FILE *fff = fopen("output.txt", "wt");

	cptr desc;

	int mode = -1;

	if (!fff) return;

	fprintf(fff, "# File: r_info.txt (autogenerated)\n\n");

	fprintf(fff, "# Version stamp (required)\n\n");

	/* Write Version */
	fprintf(fff, "V:%d.%d.%d\n\n\n", r_head->v_major, r_head->v_minor, r_head->v_patch);

	/* Write a note */
	fprintf(fff, "##### The Player #####\n\n");

	for (z = -1; z < alloc_race_size; z++)
	{
		/* Output the monsters in order */
		i = (z >= 0) ? alloc_race_table[z].index : 0;

		/* Acquire the monster */
		r_ptr = &r_info[i];

		/* Ignore empty monsters */
		if (!strlen(r_name + r_ptr->name)) continue;

		/* Ignore useless monsters */
		if (i && !r_ptr->speed) continue;

		/* Write a note if necessary */
		if (i && (!r_ptr->level != !mode))
		{
			/* Note the town */
			if (!r_ptr->level)
			{
				fprintf(fff, "\n##### Town monsters #####\n\n");
			}
			/* Note the dungeon */
			else
			{
				fprintf(fff, "\n##### Normal monsters #####\n\n");
			}

			/* Record the change */
			mode = r_ptr->level;
		}

		/* Acquire the flags */
		f_ptr[0] = r_ptr->flags1; n_ptr[0] = r_info_flags1;
		f_ptr[1] = r_ptr->flags2; n_ptr[1] = r_info_flags2;
		f_ptr[2] = r_ptr->flags3; n_ptr[2] = r_info_flags3;
		f_ptr[3] = r_ptr->flags4; n_ptr[3] = r_info_flags4;
		f_ptr[4] = r_ptr->flags5; n_ptr[4] = r_info_flags5;
		f_ptr[5] = r_ptr->flags6; n_ptr[5] = r_info_flags6;
		f_ptr[6] = r_ptr->flags7; n_ptr[6] = r_info_flags7;
		f_ptr[7] = r_ptr->flags8; n_ptr[7] = r_info_flags8;
		f_ptr[8] = r_ptr->flags9; n_ptr[8] = r_info_flags9;

		/* Write New/Number/Name */
		fprintf(fff, "N:%d:%s\n", z + 1, r_name + r_ptr->name);

		/* Write Graphic */
		fprintf(fff, "G:%c:%c\n", r_ptr->d_char, color_char[r_ptr->d_attr]);

		/* Write Information */
		fprintf(fff, "I:%d:%dd%d:%d:%d:%d\n", r_ptr->speed, r_ptr->hdice, r_ptr->hside,
														  r_ptr->aaf, r_ptr->ac, r_ptr->sleep);

		/* Write more information */
		fprintf(fff, "W:%d:%d:%d:%ld\n", r_ptr->level, r_ptr->rarity, r_ptr->extra, r_ptr->mexp);

		/* Write Blows */
		for(j = 0; j < 4; j++)
		{
			b_ptr = &(r_ptr->blow[j]);

			/* Stop when done */
			if (!b_ptr->method) break;

			/* Write the blows */
			fprintf(fff, "B:%s:%s:%dd%d\n", r_info_blow_method[b_ptr->method],
													  r_info_blow_effect[b_ptr->effect],
													  b_ptr->d_dice, b_ptr->d_side);
		}

		/* Extract the flags */
		for (fc = 0, j = 0; j < 96; j++)
		{
			/* Check this flag */
			if (f_ptr[j / 32] & (1L << (j % 32))) flags[fc++] = n_ptr[j / 32][j % 32];
		}

		/* Extract the extra flags */
		for (j = 192; j < 288; j++)
		{
			/* Check this flag */
			if (f_ptr[j / 32] & (1L << (j % 32))) flags[fc++] = n_ptr[j / 32][j % 32];
		}

		/* Write the flags */
		for (j = 0; j < fc;)
		{
			char buf[160];

			/* Start the line */
			sprintf(buf, "F:");

			for (bc = 0; (bc < 60) && (j < fc); j++)
			{
				char t[80];

				/* Format the flag */
				sprintf(t, "%s%s", flags[j], (j < fc - 1) ? " | " : "");

				/* Add it to the buffer */
				strcat(buf, t);

				/* Note the length */
				bc += strlen(t);
			}

			/* Done with this line; write it */
			fprintf(fff, "%s\n", buf);
		}

		/* Write Spells if applicable */
		if (r_ptr->freq_spell)
		{
			/* Write the frequency */
			fprintf(fff, "S:1_IN_%d | \n", 100 / r_ptr->freq_spell);

			/* Extract the spell flags */
			for (fc = 0, j = 96; j < 192; j++)
			{
				/* Check this flag */
				if (f_ptr[j / 32] & (1L << (j % 32))) flags[fc++] = n_ptr[j / 32][j % 32];
			}

			/* Write the flags */
			for (j = 0; j < fc;)
			{
				char buf[160], *t;

				/* Start the line */
				sprintf(buf, "S:");

				for (bc = 0, t = buf + 2; (bc < 60) && (j < fc); j++)
				{
					int tlen;

					/* Format the flag */
					sprintf(t, "%s%s", flags[j], (j < fc - 1) ? " | " : "");

					tlen = strlen(t);

					/* Note the length */
					bc += tlen;

					/* Advance */
					t += tlen;
				}

				/* Done with this line; write it */
				fprintf(fff, "%s\n", buf);
			}
		}

		/* Acquire the description */
		desc = r_text + r_ptr->text;
		dlen = strlen(desc);

		/* Write Description */
		for (j = 0; j < dlen;)
		{
			char buf[160], *t;

			/* Start the line */
			sprintf(buf, "D:");

			for (bc = 0, t = buf + 2; ((bc < 60) || !isspace(desc[j])) && (j < dlen); j++, bc++, t++)
			{
				*t = desc[j];
			}

			/* Terminate it */
			*t = '\0';

			/* Done with this line; write it */
			fprintf(fff, "%s\n", buf);
		}

		/* Space between entries */
		fprintf(fff, "\n");
	}

	/* Done */
	fclose(fff);
}

