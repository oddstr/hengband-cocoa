/* File: files.c */

/* Purpose: code dealing with files (and death) */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * You may or may not want to use the following "#undef".
 */
/* #undef _POSIX_SAVED_IDS */


/*
 * Hack -- drop permissions
 */
void safe_setuid_drop(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(getuid()) != 0)
	{
#ifdef JP
quit("setuid(): ���������Ĥ����ޤ���");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(getgid()) != 0)
	{
#ifdef JP
quit("setgid(): ���������Ĥ����ޤ���");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
quit("setreuid(): ���������Ĥ����ޤ���");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
quit("setregid(): ���������Ĥ����ޤ���");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif

# endif

#endif

}


/*
 * Hack -- grab permissions
 */
void safe_setuid_grab(void)
{

#ifdef SET_UID

# ifdef SAFE_SETUID

#  ifdef SAFE_SETUID_POSIX

	if (setuid(player_euid) != 0)
	{
#ifdef JP
quit("setuid(): ���������Ĥ����ޤ���");
#else
		quit("setuid(): cannot set permissions correctly!");
#endif

	}
	if (setgid(player_egid) != 0)
	{
#ifdef JP
quit("setgid(): ���������Ĥ����ޤ���");
#else
		quit("setgid(): cannot set permissions correctly!");
#endif

	}

#  else

	if (setreuid(geteuid(), getuid()) != 0)
	{
#ifdef JP
quit("setreuid(): ���������Ĥ����ޤ���");
#else
		quit("setreuid(): cannot set permissions correctly!");
#endif

	}
	if (setregid(getegid(), getgid()) != 0)
	{
#ifdef JP
quit("setregid(): ���������Ĥ����ޤ���");
#else
		quit("setregid(): cannot set permissions correctly!");
#endif

	}

#  endif /* SAFE_SETUID_POSIX */

# endif /* SAFE_SETUID */

#endif /* SET_UID */

}


/*
 * Extract the first few "tokens" from a buffer
 *
 * This function uses "colon" and "slash" as the delimeter characters.
 *
 * We never extract more than "num" tokens.  The "last" token may include
 * "delimeter" characters, allowing the buffer to include a "string" token.
 *
 * We save pointers to the tokens in "tokens", and return the number found.
 *
 * Hack -- Attempt to handle the 'c' character formalism
 *
 * Hack -- An empty buffer, or a final delimeter, yields an "empty" token.
 *
 * Hack -- We will always extract at least one token
 */
s16b tokenize(char *buf, s16b num, char **tokens, int mode)
{
	int i = 0;

	char *s = buf;


	/* Process */
	while (i < num - 1)
	{
		char *t;

		/* Scan the string */
		for (t = s; *t; t++)
		{
			/* Found a delimiter */
			if ((*t == ':') || (*t == '/')) break;

			/* Handle single quotes */
			if ((mode & TOKENIZE_CHECKQUOTE) && (*t == '\''))
			{
				/* Advance */
				t++;

				/* Handle backslash */
				if (*t == '\\') t++;

				/* Require a character */
				if (!*t) break;

				/* Advance */
				t++;

				/* Hack -- Require a close quote */
				if (*t != '\'') *t = '\'';
			}

			/* Handle back-slash */
			if (*t == '\\') t++;
		}

		/* Nothing left */
		if (!*t) break;

		/* Nuke and advance */
		*t++ = '\0';

		/* Save the token */
		tokens[i++] = s;

		/* Advance */
		s = t;
	}

	/* Save the token */
	tokens[i++] = s;

	/* Number found */
	return (i);
}


/* A number with a name */
typedef struct named_num named_num;

struct named_num
{
	cptr name;		/* The name of this thing */
	int num;			/* A number associated with it */
};


/* Index of spell type names */
static named_num gf_desc[] =
{
	{"GF_ELEC", 				GF_ELEC				},
	{"GF_POIS", 				GF_POIS				},
	{"GF_ACID", 				GF_ACID				},
	{"GF_COLD", 				GF_COLD				},
	{"GF_FIRE",		 			GF_FIRE				},
	{"GF_PSY_SPEAR",			GF_PSY_SPEAR			},
	{"GF_MISSILE",				GF_MISSILE			},
	{"GF_ARROW",				GF_ARROW				},
	{"GF_PLASMA",				GF_PLASMA			},
	{"GF_WATER",				GF_WATER				},
	{"GF_LITE",					GF_LITE				},
	{"GF_DARK",					GF_DARK				},
	{"GF_LITE_WEAK",			GF_LITE_WEAK		},
	{"GF_DARK_WEAK",			GF_DARK_WEAK		},
	{"GF_SHARDS",				GF_SHARDS			},
	{"GF_SOUND",				GF_SOUND				},
	{"GF_CONFUSION",			GF_CONFUSION		},
	{"GF_FORCE",				GF_FORCE				},
	{"GF_INERTIA",				GF_INERTIA			},
	{"GF_MANA",					GF_MANA				},
	{"GF_METEOR",				GF_METEOR			},
	{"GF_ICE",					GF_ICE				},
	{"GF_CHAOS",				GF_CHAOS				},
	{"GF_NETHER",				GF_NETHER			},
	{"GF_DISENCHANT",			GF_DISENCHANT		},
	{"GF_NEXUS",				GF_NEXUS				},
	{"GF_TIME",					GF_TIME				},
	{"GF_GRAVITY",				GF_GRAVITY			},
	{"GF_KILL_WALL",			GF_KILL_WALL		},
	{"GF_KILL_DOOR",			GF_KILL_DOOR		},
	{"GF_KILL_TRAP",			GF_KILL_TRAP		},
	{"GF_MAKE_WALL",			GF_MAKE_WALL		},
	{"GF_MAKE_DOOR",			GF_MAKE_DOOR		},
	{"GF_MAKE_TRAP",			GF_MAKE_TRAP		},
	{"GF_MAKE_TREE",			GF_MAKE_TREE		},
	{"GF_OLD_CLONE",			GF_OLD_CLONE		},
	{"GF_OLD_POLY",			GF_OLD_POLY			},
	{"GF_OLD_HEAL",			GF_OLD_HEAL			},
	{"GF_OLD_SPEED",			GF_OLD_SPEED		},
	{"GF_OLD_SLOW",			GF_OLD_SLOW			},
	{"GF_OLD_CONF",			GF_OLD_CONF			},
	{"GF_OLD_SLEEP",			GF_OLD_SLEEP		},
	{"GF_OLD_DRAIN",			GF_OLD_DRAIN		},
	{"GF_AWAY_UNDEAD",		GF_AWAY_UNDEAD		},
	{"GF_AWAY_EVIL",			GF_AWAY_EVIL		},
	{"GF_AWAY_ALL",			GF_AWAY_ALL			},
	{"GF_TURN_UNDEAD",		GF_TURN_UNDEAD		},
	{"GF_TURN_EVIL",			GF_TURN_EVIL		},
	{"GF_TURN_ALL",			GF_TURN_ALL			},
	{"GF_DISP_UNDEAD",		GF_DISP_UNDEAD		},
	{"GF_DISP_EVIL",			GF_DISP_EVIL		},
	{"GF_DISP_ALL",			GF_DISP_ALL			},
	{"GF_DISP_DEMON",			GF_DISP_DEMON		},
	{"GF_DISP_LIVING",		GF_DISP_LIVING		},
	{"GF_ROCKET",				GF_ROCKET			},
	{"GF_NUKE",					GF_NUKE				},
	{"GF_MAKE_GLYPH",			GF_MAKE_GLYPH		},
	{"GF_STASIS",				GF_STASIS			},
	{"GF_STONE_WALL",			GF_STONE_WALL		},
	{"GF_DEATH_RAY",			GF_DEATH_RAY		},
	{"GF_STUN",					GF_STUN				},
	{"GF_HOLY_FIRE",			GF_HOLY_FIRE		},
	{"GF_HELL_FIRE",			GF_HELL_FIRE		},
	{"GF_DISINTEGRATE",		GF_DISINTEGRATE	},
	{"GF_CHARM",				GF_CHARM				},
	{"GF_CONTROL_UNDEAD",	GF_CONTROL_UNDEAD	},
	{"GF_CONTROL_ANIMAL",	GF_CONTROL_ANIMAL	},
	{"GF_PSI",					GF_PSI				},
	{"GF_PSI_DRAIN",			GF_PSI_DRAIN		},
	{"GF_TELEKINESIS",		GF_TELEKINESIS		},
	{"GF_JAM_DOOR",			GF_JAM_DOOR			},
	{"GF_DOMINATION",			GF_DOMINATION		},
	{"GF_DISP_GOOD",			GF_DISP_GOOD		},
	{"GF_DRAIN_MANA",			GF_DRAIN_MANA		},
	{"GF_MIND_BLAST",			GF_MIND_BLAST		},
	{"GF_BRAIN_SMASH",			GF_BRAIN_SMASH		},
	{"GF_CAUSE_1",			GF_CAUSE_1		},
	{"GF_CAUSE_2",			GF_CAUSE_2		},
	{"GF_CAUSE_3",			GF_CAUSE_3		},
	{"GF_CAUSE_4",			GF_CAUSE_4		},
	{"GF_HAND_DOOM",			GF_HAND_DOOM		},
	{"GF_CAPTURE",			GF_CAPTURE		},
	{"GF_ANIM_DEAD",			GF_ANIM_DEAD		},
	{"GF_CONTROL_LIVING",		GF_CONTROL_LIVING	},
	{"GF_IDENTIFY",			GF_IDENTIFY	},
	{"GF_ATTACK",			GF_ATTACK	},
	{"GF_ENGETSU",			GF_ENGETSU	},
	{"GF_GENOCIDE",			GF_GENOCIDE	},
	{"GF_PHOTO",			GF_PHOTO	},
	{"GF_CONTROL_DEMON",	GF_CONTROL_DEMON	},
	{"GF_LAVA_FLOW",	GF_LAVA_FLOW	},
	{"GF_BLOOD_CURSE",	GF_BLOOD_CURSE	},
	{"GF_SEEKER",			GF_SEEKER			},
	{"GF_SUPER_RAY",		GF_SUPER_RAY			},
	{"GF_STAR_HEAL",		GF_STAR_HEAL			},
	{NULL, 						0						}
};


/*
 * Parse a sub-file of the "extra info" (format shown below)
 *
 * Each "action" line has an "action symbol" in the first column,
 * followed by a colon, followed by some command specific info,
 * usually in the form of "tokens" separated by colons or slashes.
 *
 * Blank lines, lines starting with white space, and lines starting
 * with pound signs ("#") are ignored (as comments).
 *
 * Note the use of "tokenize()" to allow the use of both colons and
 * slashes as delimeters, while still allowing final tokens which
 * may contain any characters including "delimiters".
 *
 * Note the use of "strtol()" to allow all "integers" to be encoded
 * in decimal, hexidecimal, or octal form.
 *
 * Note that "monster zero" is used for the "player" attr/char, "object
 * zero" will be used for the "stack" attr/char, and "feature zero" is
 * used for the "nothing" attr/char.
 *
 * Parse another file recursively, see below for details
 *   %:<filename>
 *
 * Specify the attr/char values for "monsters" by race index
 *   R:<num>:<a>:<c>
 *
 * Specify the attr/char values for "objects" by kind index
 *   K:<num>:<a>:<c>
 *
 * Specify the attr/char values for "features" by feature index
 *   F:<num>:<a>:<c>
 *
 * Specify the attr/char values for unaware "objects" by kind tval
 *   U:<tv>:<a>:<c>
 *
 * Specify the attr/char values for inventory "objects" by kind tval
 *   E:<tv>:<a>:<c>
 *
 * Define a macro action, given an encoded macro action
 *   A:<str>
 *
 * Create a normal macro, given an encoded macro trigger
 *   P:<str>
 *
 * Create a command macro, given an encoded macro trigger
 *   C:<str>
 *
 * Create a keyset mapping
 *   S:<key>:<key>:<dir>
 *
 * Turn an option off, given its name
 *   X:<str>
 *
 * Turn an option on, given its name
 *   Y:<str>
 *
 * Specify visual information, given an index, and some data
 *   V:<num>:<kv>:<rv>:<gv>:<bv>
 *
 * Specify the set of colors to use when drawing a zapped spell
 *   Z:<type>:<str>
 *
 * Specify a macro trigger template and macro trigger names.
 *   T:<template>:<modifier chr>:<modifier name1>:<modifier name2>:...
 *   T:<trigger>:<keycode>:<shift-keycode>
 *
 */

errr process_pref_file_command(char *buf)
{
	int i, j, n1, n2;

	char *zz[16];


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
		return (process_pref_file(buf + 2));
	}


	/* Process "R:<num>:<a>/<c>" -- attr/char for monster races */
	if (buf[0] == 'R')
	{
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			monster_race *r_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_r_idx) return (1);
			r_ptr = &r_info[i];
			if (n1) r_ptr->x_attr = n1;
			if (n2) r_ptr->x_char = n2;
			return (0);
		}
	}

	/* Process "K:<num>:<a>/<c>"  -- attr/char for object kinds */
	else if (buf[0] == 'K')
	{
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			object_kind *k_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_k_idx) return (1);
			k_ptr = &k_info[i];
			if (n1) k_ptr->x_attr = n1;
			if (n2) k_ptr->x_char = n2;
			return (0);
		}
	}

	/* Process "F:<num>:<a>/<c>" -- attr/char for terrain features */
	else if (buf[0] == 'F')
	{
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			feature_type *f_ptr;
			i = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			if (i >= max_f_idx) return (1);
			f_ptr = &f_info[i];
			if (n1) f_ptr->x_attr = n1;
			if (n2) f_ptr->x_char = n2;
			return (0);
		}
	}

	/* Process "S:<num>:<a>/<c>" -- attr/char for special things */
	else if (buf[0] == 'S')
	{
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (byte)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			misc_to_attr[j] = n1;
			misc_to_char[j] = n2;
			return (0);
		}
	}

	/* Process "U:<tv>:<a>/<c>" -- attr/char for unaware items */
	else if (buf[0] == 'U')
	{
		if (tokenize(buf+2, 3, zz, TOKENIZE_CHECKQUOTE) == 3)
		{
			j = (huge)strtol(zz[0], NULL, 0);
			n1 = strtol(zz[1], NULL, 0);
			n2 = strtol(zz[2], NULL, 0);
			for (i = 1; i < max_k_idx; i++)
			{
				object_kind *k_ptr = &k_info[i];
				if (k_ptr->tval == j)
				{
					if (n1) k_ptr->d_attr = n1;
					if (n2) k_ptr->d_char = n2;
				}
			}
			return (0);
		}
	}

	/* Process "E:<tv>:<a>" -- attribute for inventory objects */
	else if (buf[0] == 'E')
	{
		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) == 2)
		{
			j = (byte)strtol(zz[0], NULL, 0) % 128;
			n1 = strtol(zz[1], NULL, 0);
			if (n1) tval_to_attr[j] = n1;
			return (0);
		}
	}


	/* Process "A:<str>" -- save an "action" for later */
	else if (buf[0] == 'A')
	{
		text_to_ascii(macro__buf, buf+2);
		return (0);
	}

	/* Process "P:<str>" -- normal macro */
	else if (buf[0] == 'P')
	{
		char tmp[1024];
		text_to_ascii(tmp, buf+2);
		macro_add(tmp, macro__buf);
		return (0);
	}


	/* Process "C:<str>" -- create keymap */
	else if (buf[0] == 'C')
	{
		int mode;

		char tmp[1024];

		if (tokenize(buf+2, 2, zz, TOKENIZE_CHECKQUOTE) != 2) return (1);

		mode = strtol(zz[0], NULL, 0);
		if ((mode < 0) || (mode >= KEYMAP_MODES)) return (1);

		text_to_ascii(tmp, zz[1]);
		if (!tmp[0] || tmp[1]) return (1);
		i = (byte)(tmp[0]);

		string_free(keymap_act[mode][i]);

		keymap_act[mode][i] = string_make(macro__buf);

		return (0);
	}


	/* Process "V:<num>:<kv>:<rv>:<gv>:<bv>" -- visual info */
	else if (buf[0] == 'V')
	{
		if (tokenize(buf+2, 5, zz, TOKENIZE_CHECKQUOTE) == 5)
		{
			i = (byte)strtol(zz[0], NULL, 0);
			angband_color_table[i][0] = (byte)strtol(zz[1], NULL, 0);
			angband_color_table[i][1] = (byte)strtol(zz[2], NULL, 0);
			angband_color_table[i][2] = (byte)strtol(zz[3], NULL, 0);
			angband_color_table[i][3] = (byte)strtol(zz[4], NULL, 0);
			return (0);
		}
	}


	/* Process "X:<str>" -- turn option off */
	else if (buf[0] == 'X')
	{
		for (i = 0; option_info[i].o_desc; i++)
		{
			int os = option_info[i].o_set;
			int ob = option_info[i].o_bit;

			if (option_info[i].o_var &&
				 option_info[i].o_text &&
				 streq(option_info[i].o_text, buf + 2) &&
				(!alive || option_info[i].o_page !=6))
			{
				/* Clear */
				option_flag[os] &= ~(1L << ob);
				(*option_info[i].o_var) = FALSE;
				return (0);
			}
		}
	}

	/* Process "Y:<str>" -- turn option on */
	else if (buf[0] == 'Y')
	{
		for (i = 0; option_info[i].o_desc; i++)
		{
			int os = option_info[i].o_set;
			int ob = option_info[i].o_bit;

			if (option_info[i].o_var &&
				 option_info[i].o_text &&
				 streq(option_info[i].o_text, buf + 2) &&
				(!alive || option_info[i].o_page !=6))
			{
				/* Set */
				option_flag[os] |= (1L << ob);
				(*option_info[i].o_var) = TRUE;
				return (0);
			}
		}
	}

	/* Process "Z:<type>:<str>" -- set spell color */
	else if (buf[0] == 'Z')
	{
		/* Find the colon */
		char *t = strchr(buf + 2, ':');

		/* Oops */
		if (!t) return (1);

		/* Nuke the colon */
		*(t++) = '\0';

		for (i = 0; gf_desc[i].name; i++)
		{
			/* Match this type */
			if (streq(gf_desc[i].name, buf + 2))
			{
				/* Remember this color set */
				gf_color[gf_desc[i].num] = quark_add(t);

				/* Success */
				return (0);
			}
		}
	}
	/* set macro trigger names and a template */
	/* Process "T:<trigger>:<keycode>:<shift-keycode>" */
	/* Process "T:<template>:<modifier chr>:<modifier name>:..." */
	else if (buf[0] == 'T')
	{
		int len, tok;
		tok = tokenize(buf+2, 2+MAX_MACRO_MOD, zz, 0);
		if (tok >= 4)
		{
			int i;
			int num;

			if (macro_template != NULL)
			{
				free(macro_template);
				macro_template = NULL;
				for (i = 0; i < max_macrotrigger; i++)
					free(macro_trigger_name[i]);
				max_macrotrigger = 0;
			}
			
			if (*zz[0] == '\0') return 0; /* clear template */
			num = strlen(zz[1]);
			if (2 + num != tok) return 1; /* error */

			len = strlen(zz[0])+1+num+1;
			for (i = 0; i < num; i++)
				len += strlen(zz[2+i])+1;
			macro_template = malloc(len);

			strcpy(macro_template, zz[0]);
			macro_modifier_chr =
				macro_template + strlen(macro_template) + 1;
			strcpy(macro_modifier_chr, zz[1]);
			macro_modifier_name[0] =
				macro_modifier_chr + strlen(macro_modifier_chr) + 1;
			for (i = 0; i < num; i++)
			{
				strcpy(macro_modifier_name[i], zz[2+i]);
				macro_modifier_name[i+1] = macro_modifier_name[i] + 
					strlen(macro_modifier_name[i]) + 1;
			}
		}
		else if (tok >= 2)
		{
			int m;
			char *t, *s;
			if (max_macrotrigger >= MAX_MACRO_TRIG)
			{
#ifdef JP
				msg_print("�ޥ���ȥꥬ�������꤬¿�����ޤ�!");
#else
				msg_print("Too many macro triggers!");
#endif
				return 1;
			}
			m = max_macrotrigger;
			max_macrotrigger++;

			len = strlen(zz[0]) + 1 + strlen(zz[1]) + 1;
			if (tok == 3)
				len += strlen(zz[2]) + 1;
			macro_trigger_name[m] = malloc(len);

			t = macro_trigger_name[m];
			s = zz[0];
			while (*s)
			{
				if ('\\' == *s) s++;
				*t++ = *s++;
			}
			*t = '\0';

			macro_trigger_keycode[0][m] = macro_trigger_name[m] +
				strlen(macro_trigger_name[m]) + 1;
			strcpy(macro_trigger_keycode[0][m], zz[1]);
			if (tok == 3)
			{
				macro_trigger_keycode[1][m] = macro_trigger_keycode[0][m] +
					strlen(macro_trigger_keycode[0][m]) + 1;
				strcpy(macro_trigger_keycode[1][m], zz[2]);
			}
			else
			{
				macro_trigger_keycode[1][m] = macro_trigger_keycode[0][m];
			}
		}
		return 0;
	}

	/* Failure */
	return (1);
}


/*
 * Helper function for "process_pref_file()"
 *
 * Input:
 *   v: output buffer array
 *   f: final character
 *
 * Output:
 *   result
 */
static cptr process_pref_file_expr(char **sp, char *fp)
{
	cptr v;

	char *b;
	char *s;

	char b1 = '[';
	char b2 = ']';

	char f = ' ';
	static char tmp[8];

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
		t = process_pref_file_expr(&s, &f);

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
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(t, "0")) v = "1";
			}
		}

		/* Function: AND */
		else if (streq(t, "AND"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "0")) v = "0";
			}
		}

		/* Function: NOT */
		else if (streq(t, "NOT"))
		{
			v = "1";
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
				if (*t && streq(t, "1")) v = "0";
			}
		}

		/* Function: EQU */
		else if (streq(t, "EQU"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && !streq(p, t)) v = "0";
			}
		}

		/* Function: LEQ */
		else if (streq(t, "LEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && (strcmp(p, t) > 0)) v = "0";
			}
		}

		/* Function: GEQ */
		else if (streq(t, "GEQ"))
		{
			v = "1";
			if (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
			}
			while (*s && (f != b2))
			{
				p = t;
				t = process_pref_file_expr(&s, &f);
				if (*t && (strcmp(p, t) < 0)) v = "0";
			}
		}

		/* Oops */
		else
		{
			while (*s && (f != b2))
			{
				t = process_pref_file_expr(&s, &f);
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

                        else if (streq(b+1, "KEYBOARD"))
                        {
                                v = ANGBAND_KEYBOARD;
                        }

			/* Graphics */
			else if (streq(b+1, "GRAF"))
			{
				v = ANGBAND_GRAF;
			}

			/* Monochrome mode */
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

			/* Player */
			else if (streq(b+1, "PLAYER"))
			{
				v = player_base;
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

			/* Level */
			else if (streq(b+1, "LEVEL"))
			{
				sprintf(tmp, "%02d", p_ptr->lev);
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


/*
 * Open the "user pref file" and parse it.
 */
static errr process_pref_file_aux(cptr name)
{
	FILE *fp;

	char buf[1024];

	char old[1024];

	int line = -1;

	errr err = 0;

	bool bypass = FALSE;


	/* Open the file */
	fp = my_fopen(name, "r");

	/* No such file */
	if (!fp) return (-1);

	/* Process the file */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Count lines */
		line++;


		/* Skip "empty" lines */
		if (!buf[0]) continue;

		/* Skip "blank" lines */
		if (isspace(buf[0])) continue;

		/* Skip comments */
		if (buf[0] == '#') continue;


		/* Save a copy */
		strcpy(old, buf);


		/* Process "?:<expr>" */
		if ((buf[0] == '?') && (buf[1] == ':'))
		{
			char f;
			cptr v;
			char *s;

			/* Start */
			s = buf + 2;

			/* Parse the expr */
			v = process_pref_file_expr(&s, &f);

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
			(void)process_pref_file(buf + 2);

			/* Continue */
			continue;
		}


		/* Process the line */
		err = process_pref_file_command(buf);

		/* Oops */
		if (err) break;
	}


	/* Error */
	if (err)
	{
		/* Print error message */
		/* ToDo: Add better error messages */
#ifdef JP
              msg_format("�ե�����'%s'��%d�Ԥǥ��顼�ֹ�%d�Υ��顼��", name, line, err);
              msg_format("('%s'�������)", old);
#else
		msg_format("Error %d in line %d of file '%s'.", err, line, name);
		msg_format("Parsing '%s'", old);
#endif
		msg_print(NULL);
	}

	/* Close the file */
	my_fclose(fp);

	/* Result */
	return (err);
}



/*
 * Process the "user pref file" with the given name
 *
 * See the functions above for a list of legal "commands".
 *
 * We also accept the special "?" and "%" directives, which
 * allow conditional evaluation and filename inclusion.
 */
errr process_pref_file(cptr name)
{
	char buf[1024];

	errr err = 0;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_PREF, name);

	/* Process the pref file */
	err = process_pref_file_aux(buf);

	/* Stop at parser errors, but not at non-existing file */
	if (err < 1)
	{
		/* Build the filename */
		path_build(buf, 1024, ANGBAND_DIR_USER, name);

		/* Process the pref file */
		err = process_pref_file_aux(buf);
	}

	/* Result */
	return (err);
}



#ifdef CHECK_TIME

/*
 * Operating hours for ANGBAND (defaults to non-work hours)
 */
static char days[7][29] =
{
	"SUN:XXXXXXXXXXXXXXXXXXXXXXXX",
	"MON:XXXXXXXX.........XXXXXXX",
	"TUE:XXXXXXXX.........XXXXXXX",
	"WED:XXXXXXXX.........XXXXXXX",
	"THU:XXXXXXXX.........XXXXXXX",
	"FRI:XXXXXXXX.........XXXXXXX",
	"SAT:XXXXXXXXXXXXXXXXXXXXXXXX"
};

/*
 * Restict usage (defaults to no restrictions)
 */
static bool check_time_flag = FALSE;

#endif


/*
 * Handle CHECK_TIME
 */
errr check_time(void)
{

#ifdef CHECK_TIME

	time_t      c;
	struct tm   *tp;

	/* No restrictions */
	if (!check_time_flag) return (0);

	/* Check for time violation */
	c = time((time_t *)0);
	tp = localtime(&c);

	/* Violation */
	if (days[tp->tm_wday][tp->tm_hour + 4] != 'X') return (1);

#endif

	/* Success */
	return (0);
}



/*
 * Initialize CHECK_TIME
 */
errr check_time_init(void)
{

#ifdef CHECK_TIME

	FILE        *fp;

	char	buf[1024];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "time.txt");

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Assume restrictions */
	check_time_flag = TRUE;

	/* Parse the file */
	while (0 == my_fgets(fp, buf, 80))
	{
		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Chop the buffer */
		buf[29] = '\0';

		/* Extract the info */
		if (prefix(buf, "SUN:")) strcpy(days[0], buf);
		if (prefix(buf, "MON:")) strcpy(days[1], buf);
		if (prefix(buf, "TUE:")) strcpy(days[2], buf);
		if (prefix(buf, "WED:")) strcpy(days[3], buf);
		if (prefix(buf, "THU:")) strcpy(days[4], buf);
		if (prefix(buf, "FRI:")) strcpy(days[5], buf);
		if (prefix(buf, "SAT:")) strcpy(days[6], buf);
	}

	/* Close it */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}



#ifdef CHECK_LOAD

#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN  64
#endif

typedef struct statstime statstime;

struct statstime
{
	int                 cp_time[4];
	int                 dk_xfer[4];
	unsigned int        v_pgpgin;
	unsigned int        v_pgpgout;
	unsigned int        v_pswpin;
	unsigned int        v_pswpout;
	unsigned int        v_intr;
	int                 if_ipackets;
	int                 if_ierrors;
	int                 if_opackets;
	int                 if_oerrors;
	int                 if_collisions;
	unsigned int        v_swtch;
	long                avenrun[3];
	struct timeval      boottime;
	struct timeval      curtime;
};

/*
 * Maximal load (if any).
 */
static int check_load_value = 0;

#endif


/*
 * Handle CHECK_LOAD
 */
errr check_load(void)
{

#ifdef CHECK_LOAD

	struct statstime    st;

	/* Success if not checking */
	if (!check_load_value) return (0);

	/* Check the load */
	if (0 == rstat("localhost", &st))
	{
		long val1 = (long)(st.avenrun[2]);
		long val2 = (long)(check_load_value) * FSCALE;

		/* Check for violation */
		if (val1 >= val2) return (1);
	}

#endif

	/* Success */
	return (0);
}


/*
 * Initialize CHECK_LOAD
 */
errr check_load_init(void)
{

#ifdef CHECK_LOAD

	FILE        *fp;

	char	buf[1024];

	char	temphost[MAXHOSTNAMELEN+1];
	char	thishost[MAXHOSTNAMELEN+1];


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, "load.txt");

	/* Open the "load" file */
	fp = my_fopen(buf, "r");

	/* No file, no restrictions */
	if (!fp) return (0);

	/* Default load */
	check_load_value = 100;

	/* Get the host name */
	(void)gethostname(thishost, (sizeof thishost) - 1);

	/* Parse it */
	while (0 == my_fgets(fp, buf, 1024))
	{
		int value;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Parse, or ignore */
		if (sscanf(buf, "%s%d", temphost, &value) != 2) continue;

		/* Skip other hosts */
		if (!streq(temphost, thishost) &&
		    !streq(temphost, "localhost")) continue;

		/* Use that value */
		check_load_value = value;

		/* Done */
		break;
	}

	/* Close the file */
	my_fclose(fp);

#endif

	/* Success */
	return (0);
}


/*
 * Print long number with header at given row, column
 * Use the color for the number, not the header
 */
static void prt_lnum(cptr header, s32b num, int row, int col, byte color)
{
	int len = strlen(header);
	char out_val[32];
	put_str(header, row, col);
	(void)sprintf(out_val, "%9ld", (long)num);
	c_put_str(color, out_val, row, col + len);
}


/*
 * Print number with header at given row, column
 */
static void prt_num(cptr header, int num, int row, int col, byte color)
{
	int len = strlen(header);
	char out_val[32];
	put_str(header, row, col);
	put_str("   ", row, col + len);
	(void)sprintf(out_val, "%6ld", (long)num);
	c_put_str(color, out_val, row, col + len + 3);
}


#ifdef JP
/*    ���ܸ��ǳ�ĥ
 *         �ȣ� / ���� �Τ褦��ɽ��
 *        xxxxx / yyyyy
 */
static void prt_num_max( int num, int max , int row, int col, byte color1, byte color2 )
{
	char out_val[32];
	(void)sprintf(out_val, "%5ld", (long)num);
	c_put_str(color1, out_val, row, col );
	put_str("/",row, col+6);
	(void)sprintf(out_val, "%5ld", (long)max);
	c_put_str(color2, out_val, row, col+8 );
}

/*    ���ܸ��ǳ�ĥ
 *    xx ��  �Ȥ� xx kg �ʤɤ�ɽ���� cptr tailer ��ñ�̤����롣
 */
static void prt_num2(cptr header, cptr tailer, int num, int row, int col, byte color)
{
	int len = strlen(header);
	char out_val[32];
	put_str(header, row, col);
	put_str("   ", row, col + len);
	(void)sprintf(out_val, "%6ld", (long)num);
	c_put_str(color, out_val, row, col + len + 3);
	put_str(tailer, row, col + len + 3+6);
}
#endif
/*
 * Prints the following information on the screen.
 *
 * For this to look right, the following should be spaced the
 * same as in the prt_lnum code... -CFT
 */
static void display_player_middle(void)
{
        char buf[160];
#ifdef JP
        byte statcolor;
#endif
	int show_tohit, show_todam;
	object_type *o_ptr;
	int tmul = 0;

	if(p_ptr->migite)
	{
		show_tohit = p_ptr->dis_to_h[0];
		show_todam = p_ptr->dis_to_d[0];

		o_ptr = &inventory[INVEN_RARM];

		/* Hack -- add in weapon info if known */
		if (object_known_p(o_ptr)) show_tohit += o_ptr->to_h;
		if (object_known_p(o_ptr)) show_todam += o_ptr->to_d;

		/* Melee attacks */
		sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);

		/* Dump the bonuses to hit/dam */
#ifdef JP
		if(!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			Term_putstr(1, 9, -1, TERM_WHITE, "�Ƿ⽤��(��Ʈ)");
		else if(p_ptr->ryoute)
			Term_putstr(1, 9, -1, TERM_WHITE, "�Ƿ⽤��(ξ��)");
		else
			Term_putstr(1, 9, -1, TERM_WHITE, (left_hander ? "�Ƿ⽤��(����)" : "�Ƿ⽤��(����)"));
#else
		if(!buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
			Term_putstr(1, 9, -1, TERM_WHITE, "Melee(bare h.)");
		else if(p_ptr->ryoute)
			Term_putstr(1, 9, -1, TERM_WHITE, "Melee(2hands)");
		else
			Term_putstr(1, 9, -1, TERM_WHITE, (left_hander ? "Melee(Left)" : "Melee(Right)"));
#endif
		Term_putstr(15, 9, -1, TERM_L_BLUE, format("%11s", buf));
	}

	if(p_ptr->hidarite)
	{
		show_tohit = p_ptr->dis_to_h[1];
		show_todam = p_ptr->dis_to_d[1];

		o_ptr = &inventory[INVEN_LARM];

		/* Hack -- add in weapon info if known */
		if (object_known_p(o_ptr)) show_tohit += o_ptr->to_h;
		if (object_known_p(o_ptr)) show_todam += o_ptr->to_d;

		/* Melee attacks */
		sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);

		/* Dump the bonuses to hit/dam */
#ifdef JP
		Term_putstr(1, 10, -1, TERM_WHITE, (left_hander ? "�Ƿ⽤��(����)" : "�Ƿ⽤��(����)"));
#else
		Term_putstr(1, 10, -1, TERM_WHITE, "Melee(Left)");
#endif
		Term_putstr(15, 10, -1, TERM_L_BLUE, format("%11s", buf));
	}
	else if ((p_ptr->pclass == CLASS_MONK) && (empty_hands(TRUE) > 1))
	{
		int i;
		if (p_ptr->special_defense & KAMAE_MASK)
		{
			for (i = 0; i < MAX_KAMAE; i++)
			{
				if ((p_ptr->special_defense >> i) & KAMAE_GENBU) break;
			}
#ifdef JP
			if (i < MAX_KAMAE) Term_putstr(16, 10, -1, TERM_YELLOW, format("%s�ι���", kamae_shurui[i].desc));
#else
			if (i < MAX_KAMAE) Term_putstr(10, 10, -1, TERM_YELLOW, format("%11.11s form", kamae_shurui[i].desc));
#endif
		}
		else
#ifdef JP
			Term_putstr(18, 10, -1, TERM_YELLOW, "�����ʤ�");
#else
			Term_putstr(16, 10, -1, TERM_YELLOW, "no posture");
#endif
	}

	/* Range weapon */
	o_ptr = &inventory[INVEN_BOW];

	/* Base skill */
	show_tohit = p_ptr->dis_to_h_b;
	show_todam = 0;

	/* Apply weapon bonuses */
	if (object_known_p(o_ptr)) show_tohit += o_ptr->to_h;
	if (object_known_p(o_ptr)) show_todam += o_ptr->to_d;

	show_tohit += (weapon_exp[0][o_ptr->sval]-4000)/200;

	/* Range attacks */
	sprintf(buf, "(%+d,%+d)", show_tohit, show_todam);
#ifdef JP
	Term_putstr(1, 11, -1, TERM_WHITE, "�ͷ⹶�⽤��");
#else
	Term_putstr(1, 11, -1, TERM_WHITE, "Shoot");
#endif
	Term_putstr(15, 11, -1, TERM_L_BLUE, format("%11s", buf));

	
	if (inventory[INVEN_BOW].k_idx)
	{
		switch (inventory[INVEN_BOW].sval)
		{
			/* Sling and ammo */
			case SV_SLING:
			{
				tmul = 2;
				break;
			}

			/* Short Bow and Arrow */
			case SV_SHORT_BOW:
			{
				tmul = 2;
				break;
			}

			/* Long Bow and Arrow */
			case SV_LONG_BOW:
			case SV_NAMAKE_BOW:
			{
				tmul = 3;
				break;
			}

			/* Light Crossbow and Bolt */
			case SV_LIGHT_XBOW:
			{
				tmul = 3;
				break;
			}

			/* Heavy Crossbow and Bolt */
			case SV_HEAVY_XBOW:
			{
				tmul = 4;
				break;
			}

			/* Long Bow and Arrow */
			{
				tmul = 5;
				break;
			}
		}

		/* Get extra "power" from "extra might" */
		if (p_ptr->xtra_might) tmul++;

		tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	}
	sprintf(buf, "x%d.%02d", tmul/100, tmul%100);
#ifdef JP
	Term_putstr(1, 12, -1, TERM_WHITE, "�ͷ������Ψ");
#else
	Term_putstr(1, 12, -1, TERM_WHITE, "Shoot Power");
#endif
	Term_putstr(15, 12, -1, TERM_L_BLUE, format("%11s", buf));

	/* Dump the armor class bonus */
#ifdef JP
prt_num("AC ����         ", p_ptr->dis_to_a, 13, 1, TERM_L_BLUE);
#else
	prt_num("+ To AC         ", p_ptr->dis_to_a, 13, 1, TERM_L_BLUE);
#endif


	/* Dump the total armor class */
#ifdef JP
prt_num("���� AC         ", p_ptr->dis_ac, 14, 1, TERM_L_BLUE);
#else
	prt_num("Base AC         ", p_ptr->dis_ac, 14, 1, TERM_L_BLUE);
#endif

#ifdef JP
prt_num("��٥�     ", (int)p_ptr->lev, 9, 28, TERM_L_GREEN);
#else
	prt_num("Level      ", (int)p_ptr->lev, 9, 28, TERM_L_GREEN);
#endif

	if (p_ptr->prace == RACE_ANDROID)
	{
put_str("�и���   ", 10, 28);
c_put_str(TERM_L_GREEN, "    *****", 10, 28+11);
	}
	else if (p_ptr->exp >= p_ptr->max_exp)
	{
#ifdef JP
prt_lnum("�и���     ", p_ptr->exp, 10, 28, TERM_L_GREEN);
#else
		prt_lnum("Experience ", p_ptr->exp, 10, 28, TERM_L_GREEN);
#endif

	}
	else
	{
#ifdef JP
prt_lnum("�и���     ", p_ptr->exp, 10, 28, TERM_YELLOW);
#else
		prt_lnum("Experience ", p_ptr->exp, 10, 28, TERM_YELLOW);
#endif

	}

	if (p_ptr->prace == RACE_ANDROID)
	{
put_str("����и� ", 11, 28);
c_put_str(TERM_L_GREEN, "    *****", 11, 28+11);
	}
	else
#ifdef JP
prt_lnum("����и�   ", p_ptr->max_exp, 11, 28, TERM_L_GREEN);
#else
	prt_lnum("Max Exp    ", p_ptr->max_exp, 11, 28, TERM_L_GREEN);
#endif


	if ((p_ptr->lev >= PY_MAX_LEVEL) || (p_ptr->prace == RACE_ANDROID))
	{
#ifdef JP
put_str("����٥�   ", 12, 28);
c_put_str(TERM_L_GREEN, "    *****", 12, 28+11);
#else
		put_str("Exp to Adv.", 12, 28);
		c_put_str(TERM_L_GREEN, "    *****", 12, 28+11);
#endif

	}
	else
	{
#ifdef JP
prt_lnum("����٥�   ",
#else
		prt_lnum("Exp to Adv.",
#endif

		         (s32b)(player_exp[p_ptr->lev - 1] * p_ptr->expfact / 100L),
		         12, 28, TERM_L_GREEN);
	}

#ifdef JP
prt_lnum("�����     ", p_ptr->au, 13, 28, TERM_L_GREEN);
#else
	prt_lnum("Gold       ", p_ptr->au, 13, 28, TERM_L_GREEN);
#endif

#ifdef JP
prt_lnum("�������   ", MAX(turn_real(turn),0), 14, 28, TERM_L_GREEN  );
#else
prt_lnum("Total turn ", MAX(turn_real(turn),0), 14, 28, TERM_L_GREEN  );
#endif

#ifdef JP
/*           54321 / 54321   */
put_str("     �ȣ� /  ����    ", 9, 52);
	if (p_ptr->chp >= p_ptr->mhp) 
	  statcolor = TERM_L_GREEN;
	else
	if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10) 
	  statcolor = TERM_YELLOW;
	else
	  statcolor=TERM_RED;
prt_num_max( p_ptr->chp , p_ptr->mhp, 10, 56, statcolor , TERM_L_GREEN);

#else
	prt_num("Max Hit Points ", p_ptr->mhp, 9, 52, TERM_L_GREEN);

	if (p_ptr->chp >= p_ptr->mhp)
	{
		prt_num("Cur Hit Points ", p_ptr->chp, 10, 52, TERM_L_GREEN);
	}
	else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
	{
		prt_num("Cur Hit Points ", p_ptr->chp, 10, 52, TERM_YELLOW);
	}
	else
	{
		prt_num("Cur Hit Points ", p_ptr->chp, 10, 52, TERM_RED);
	}
#endif


#ifdef JP
/*           54321 / 54321   */
put_str("     �ͣ� /  ����    ", 11, 52);
	if (p_ptr->csp >= p_ptr->msp) 
	  statcolor = TERM_L_GREEN;
	else
	if (p_ptr->csp > (p_ptr->msp * hitpoint_warn) / 10) 
	  statcolor = TERM_YELLOW;
	else
	  statcolor=TERM_RED;
prt_num_max( p_ptr->csp , p_ptr->msp, 12, 56, statcolor , TERM_L_GREEN);
#else
	prt_num("Max SP (Mana)  ", p_ptr->msp, 11, 52, TERM_L_GREEN);

	if (p_ptr->csp >= p_ptr->msp)
	{
		prt_num("Cur SP (Mana)  ", p_ptr->csp, 12, 52, TERM_L_GREEN);
	}
	else if (p_ptr->csp > (p_ptr->msp * hitpoint_warn) / 10)
	{
		prt_num("Cur SP (Mana)  ", p_ptr->csp, 12, 52, TERM_YELLOW);
	}
	else
	{
		prt_num("Cur SP (Mana)  ", p_ptr->csp, 12, 52, TERM_RED);
	}
#endif

	sprintf(buf, "%.2lu:%.2lu:%.2lu", playtime/(60*60), (playtime/60)%60, playtime%60);
#ifdef JP
	Term_putstr(52, 14, -1, TERM_WHITE, "�ץ쥤����");
#else
	Term_putstr(52, 14, -1, TERM_WHITE, "Playtime");
#endif
	Term_putstr(63, 14, -1, TERM_L_GREEN, format("%11s", buf));

}


/*
 * Hack -- pass color info around this file
 */
static byte likert_color = TERM_WHITE;


/*
 * Returns a "rating" of x depending on y
 */
static cptr likert(int x, int y)
{
	static char dummy[20] = "";

	/* Paranoia */
	if (y <= 0) y = 1;

	/* Negative value */
	if (x < 0)
	{
		likert_color = TERM_L_DARK;
#ifdef JP
return ("����");
#else
		return ("Very Bad");
#endif

	}

	/* Analyze the value */
	switch ((x / y))
	{
		case 0:
		case 1:
		{
			likert_color = TERM_RED;
#ifdef JP
return ("����");
#else
			return ("Bad");
#endif

		}
		case 2:
		{
			likert_color = TERM_L_RED;
#ifdef JP
return ("����");
#else
			return ("Poor");
#endif

		}
		case 3:
		case 4:
		{
			likert_color = TERM_ORANGE;
#ifdef JP
return ("����");
#else
			return ("Fair");
#endif

		}
		case 5:
		{
			likert_color = TERM_YELLOW;
#ifdef JP
return ("�ɤ�");
#else
			return ("Good");
#endif

		}
		case 6:
		{
			likert_color = TERM_YELLOW;
#ifdef JP
return ("�����ɤ�");
#else
			return ("Very Good");
#endif

		}
		case 7:
		case 8:
		{
			likert_color = TERM_L_GREEN;
#ifdef JP
return ("���");
#else
			return ("Excellent");
#endif

		}
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		{
			likert_color = TERM_GREEN;
#ifdef JP
return ("Ķ��");
#else
			return ("Superb");
#endif

		}
		case 14:
		case 15:
		case 16:
		case 17:
		{
			likert_color = TERM_BLUE;
#ifdef JP
return ("���������");
#else
			return ("Chaos Rank");
#endif

		}
		default:
		{
			likert_color = TERM_VIOLET;
#ifdef JP
sprintf(dummy,"����С� [%d]", (int) ((((x/y)-17)*5)/2));
#else
			sprintf(dummy,"Amber [%d]", (int) ((((x/y)-17)*5)/2));
#endif

			return dummy;
		}
	}
}


/*
 * Prints ratings on certain abilities
 *
 * This code is "imitated" elsewhere to "dump" a character sheet.
 */
static void display_player_various(void)
{
	int         tmp, damage[2], blows1, blows2, i, basedam;
	int			xthn, xthb, xfos, xsrh;
	int			xdis, xdev, xsav, xstl;
	cptr		desc;
	int         muta_att = 0;
	u32b        f1, f2, f3;
	int		energy_fire = 100;
	int		shots, shot_frac;

	object_type		*o_ptr;

	if (p_ptr->muta2 & MUT2_HORNS)     muta_att++;
	if (p_ptr->muta2 & MUT2_SCOR_TAIL) muta_att++;
	if (p_ptr->muta2 & MUT2_BEAK)      muta_att++;
	if (p_ptr->muta2 & MUT2_TRUNK)     muta_att++;
	if (p_ptr->muta2 & MUT2_TENTACLES) muta_att++;

	xthn = p_ptr->skill_thn + (p_ptr->to_h_m * BTH_PLUS_ADJ);

	/* Shooting Skill (with current bow and normal missile) */
	o_ptr = &inventory[INVEN_BOW];
	tmp = p_ptr->to_h_b + o_ptr->to_h;
	xthb = p_ptr->skill_thb + (tmp * BTH_PLUS_ADJ);

	/* If the player is wielding one? */
	if (o_ptr->k_idx)
	{
		/* Analyze the launcher */
		switch (o_ptr->sval)
		{
			/* Sling and ammo */
			case SV_SLING:
			{
				energy_fire = 8000;
				break;
			}

			/* Short Bow and Arrow */
			case SV_SHORT_BOW:
			{
				energy_fire = 10000;
				break;
			}

			/* Long Bow and Arrow */
			case SV_LONG_BOW:
			{
				energy_fire = 10000;
				break;
			}
		
			case SV_NAMAKE_BOW:
			{
				energy_fire = 7777;
				break;
			}
		
			/* Light Crossbow and Bolt */
			case SV_LIGHT_XBOW:
			{
				energy_fire = 12000;
				break;
			}

			/* Heavy Crossbow and Bolt */
			case SV_HEAVY_XBOW:
			{		
				energy_fire = 13333;
				break;
			}
		}
		/* Calculate shots per round */
		shots = p_ptr->num_fire * 100;
		shot_frac = (shots * 100 / energy_fire) % 100;
		shots = shots / energy_fire;
		if (o_ptr->name1 == ART_CRIMSON)
		{
			shots = 1;
			shot_frac = 0;
			if (p_ptr->pclass == CLASS_ARCHER)
			{
				/* Extra shot at level 10 */
				if (p_ptr->lev >= 10) shots++;

				/* Extra shot at level 30 */
				if (p_ptr->lev >= 30) shots++;

				/* Extra shot at level 45 */
				if (p_ptr->lev >= 45) shots++;
			}
		}
	}
	else
	{
		shots = 0;
		shot_frac = 0;
	}

	for(i = 0; i< 2; i++)
	{
		damage[i] = p_ptr->dis_to_d[i]*100;
		if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER)) && (empty_hands(TRUE) > 1))
		{
			int level = p_ptr->lev;
			if (i)
			{
				damage[i] = 0;
				break;
			}
			if (p_ptr->pclass == CLASS_FORCETRAINER) level = MAX(1, level - 3);
			if (p_ptr->special_defense & KAMAE_BYAKKO)
				basedam = monk_ave_damage[level][1];
			else if (p_ptr->special_defense & (KAMAE_GENBU | KAMAE_SUZAKU))
				basedam = monk_ave_damage[level][2];
			else
				basedam = monk_ave_damage[level][0];
		}
		else
		{
			/* Average damage per round */
			o_ptr = &inventory[INVEN_RARM+i];
			if (object_known_p(o_ptr)) damage[i] += o_ptr->to_d*100;
			basedam = (o_ptr->dd * (o_ptr->ds + 1))*50;
			object_flags(o_ptr, &f1, &f2, &f3);
			if ((o_ptr->ident & IDENT_MENTAL) && (o_ptr->name1 == ART_VORPAL_BLADE))
			{
				/* vorpal blade */
				basedam *= 5;
				basedam /= 3;
			}
			else if (object_known_p(o_ptr) && (f1 & TR1_VORPAL))
			{
				/* vorpal flag only */
				basedam *= 11;
				basedam /= 9;
			}
			if (object_known_p(o_ptr) && (p_ptr->pclass != CLASS_SAMURAI) && (f1 & TR1_FORCE_WEPON) && (p_ptr->csp > (o_ptr->dd * o_ptr->ds / 5)))
				basedam = basedam * 7 / 2;
			if (p_ptr->riding && (o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
				basedam = basedam*(o_ptr->dd+2)/o_ptr->dd;
		}
		damage[i] += basedam;
		if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) damage[i] = 1;
		if (damage[i] < 0) damage[i] = 0;
	}
	blows1 = p_ptr->migite ? p_ptr->num_blow[0]: 0;
	blows2 = p_ptr->hidarite ? p_ptr->num_blow[1] : 0;

	/* Basic abilities */

	xdis = p_ptr->skill_dis;
	xdev = p_ptr->skill_dev;
	xsav = p_ptr->skill_sav;
	xstl = p_ptr->skill_stl;
	xsrh = p_ptr->skill_srh;
	xfos = p_ptr->skill_fos;


#ifdef JP
put_str("�Ƿ⹶��ǽ�� :", 17, 1);
#else
	put_str("Fighting    :", 17, 1);
#endif

	desc = likert(xthn, 12);
	c_put_str(likert_color, desc, 17, 15);

#ifdef JP
put_str("�ͷ⹶��ǽ�� :", 18, 1);
#else
	put_str("Bows/Throw  :", 18, 1);
#endif

	desc = likert(xthb, 12);
	c_put_str(likert_color, desc, 18, 15);

#ifdef JP
put_str("��ˡ�ɸ�ǽ�� :", 19, 1);
#else
	put_str("Saving Throw:", 19, 1);
#endif

	desc = likert(xsav, 7);
	c_put_str(likert_color, desc, 19, 15);

#ifdef JP
put_str("��̩��ưǽ�� :", 20, 1);
#else
	put_str("Stealth     :", 20, 1);
#endif

	desc = likert(xstl, 1);
	c_put_str(likert_color, desc, 20, 15);


#ifdef JP
put_str("�γ�ǽ��     :", 17, 28);
#else
	put_str("Perception  :", 17, 28);
#endif

	desc = likert(xfos, 6);
	c_put_str(likert_color, desc, 17, 42);

#ifdef JP
put_str("õ��ǽ��     :", 18, 28);
#else
	put_str("Searching   :", 18, 28);
#endif

	desc = likert(xsrh, 6);
	c_put_str(likert_color, desc, 18, 42);

#ifdef JP
put_str("���ǽ��     :", 19, 28);
#else
	put_str("Disarming   :", 19, 28);
#endif

	desc = likert(xdis, 8);
	c_put_str(likert_color, desc, 19, 42);

#ifdef JP
put_str("��ˡƻ����� :", 20, 28);
#else
	put_str("Magic Device:", 20, 28);
#endif

	desc = likert(xdev, 6);
	c_put_str(likert_color, desc, 20, 42);


#ifdef JP
put_str("�Ƿ���    :", 17, 55);
#else
	put_str("Blows/Round:", 17, 55);
#endif

	if (!muta_att)
		put_str(format("%d+%d", blows1, blows2), 17, 69);
	else
		put_str(format("%d+%d+%d", blows1, blows2, muta_att), 17, 69);

#ifdef JP
put_str("�ͷ���    :", 18, 55);
#else
	put_str("Shots/Round:", 18, 55);
#endif

	put_str(format("%d.%02d", shots, shot_frac), 18, 69);

#ifdef JP
put_str("ʿ�ѥ��᡼��:", 19, 55);	                     
#else
	put_str("Wpn.dmg/Rnd:", 19, 55);	/* From PsiAngband */
#endif


	if ((damage[0]+damage[1]) == 0)
		desc = "nil!";
	else
		desc = format("%d+%d", blows1 * damage[0] / 100, blows2 * damage[1] / 100);

	put_str(desc, 19, 69);

#ifdef JP
put_str("�ֳ�������  :", 20, 55);
#else
	put_str("Infra-Vision:", 20, 55);
#endif

#ifdef JP
put_str(format("%d feet", p_ptr->see_infra * 10), 20, 69);
#else
	put_str(format("%d feet", p_ptr->see_infra * 10), 20, 69);
#endif

}



/*
 * Obtain the "flags" for the player as if he was an item
 */
static void player_flags(u32b *f1, u32b *f2, u32b *f3)
{
	/* Clear */
	(*f1) = (*f2) = (*f3) = 0L;

	/* Classes */
	switch (p_ptr->pclass)
	{
	case CLASS_WARRIOR:
		if (p_ptr->lev > 44)
			(*f3) |= (TR3_REGEN);
	case CLASS_SAMURAI:
		if (p_ptr->lev > 29)
			(*f2) |= (TR2_RES_FEAR);
		break;
	case CLASS_PALADIN:
		if (p_ptr->lev > 39)
			(*f2) |= (TR2_RES_FEAR);
		break;
	case CLASS_CHAOS_WARRIOR:
		if (p_ptr->lev > 29)
			(*f2) |= (TR2_RES_CHAOS);
		if (p_ptr->lev > 39)
			(*f2) |= (TR2_RES_FEAR);
		break;
	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		if ((p_ptr->lev > 9) && !heavy_armor())
			(*f1) |= TR1_SPEED;
		if ((p_ptr->lev>24) && !heavy_armor())
			(*f2) |= (TR2_FREE_ACT);
		break;
	case CLASS_NINJA:
		if (heavy_armor())
			(*f1) |= TR1_SPEED;
		else
		{
			if (!inventory[INVEN_LARM].tval || p_ptr->hidarite)
				(*f1) |= TR1_SPEED;
			if (p_ptr->lev>24)
				(*f2) |= (TR2_FREE_ACT);
		}
		(*f3) |= TR3_SLOW_DIGEST;
		(*f2) |= TR2_RES_FEAR;
		if (p_ptr->lev > 19) (*f2) |= TR2_RES_POIS;
		if (p_ptr->lev > 24) (*f2) |= TR2_SUST_DEX;
		if (p_ptr->lev > 29) (*f3) |= TR3_SEE_INVIS;
		break;
	case CLASS_MINDCRAFTER:
		if (p_ptr->lev > 9)
			(*f2) |= (TR2_RES_FEAR);
		if (p_ptr->lev > 19)
			(*f2) |= (TR2_SUST_WIS);
		if (p_ptr->lev > 29)
			(*f2) |= (TR2_RES_CONF);
		if (p_ptr->lev > 39)
			(*f3) |= (TR3_TELEPATHY);
		break;
	case CLASS_BARD:
		(*f2) |= (TR2_RES_SOUND);
		break;
	case CLASS_BERSERKER:
		(*f2) |= (TR2_SUST_STR);
		(*f2) |= (TR2_SUST_DEX);
		(*f2) |= (TR2_SUST_CON);
		(*f3) |= (TR3_REGEN);
		(*f2) |= (TR2_FREE_ACT);
		(*f1) |= (TR1_SPEED);
		if (p_ptr->lev > 39) (*f2) |= (TR2_REFLECT);
		break;
	case CLASS_MIRROR_MASTER:
		if(p_ptr->lev > 39)(*f2) |= (TR2_REFLECT);
		break;
	default:
		break; /* Do nothing */
	}

	/* Races */
	if (p_ptr->mimic_form)
	{
		switch(p_ptr->mimic_form)
		{
		case MIMIC_DEMON:
			(*f2) |= (TR2_HOLD_LIFE);
			(*f2) |= (TR2_RES_CHAOS);
			(*f2) |= (TR2_RES_NETHER);
			(*f2) |= (TR2_RES_FIRE);
			(*f3) |= (TR3_SEE_INVIS);
			(*f1) |= (TR1_SPEED);
			break;
		case MIMIC_DEMON_LORD:
			(*f2) |= (TR2_HOLD_LIFE);
			(*f2) |= (TR2_RES_CHAOS);
			(*f2) |= (TR2_RES_NETHER);
			(*f2) |= (TR2_RES_FIRE);
			(*f2) |= (TR2_RES_COLD);
			(*f2) |= (TR2_RES_ELEC);
			(*f2) |= (TR2_RES_ACID);
			(*f2) |= (TR2_RES_POIS);
			(*f2) |= (TR2_RES_CONF);
			(*f2) |= (TR2_RES_DISEN);
			(*f2) |= (TR2_RES_NEXUS);
			(*f2) |= (TR2_RES_FEAR);
			(*f2) |= (TR2_IM_FIRE);
			(*f3) |= (TR3_SH_FIRE);
			(*f3) |= (TR3_SEE_INVIS);
			(*f3) |= (TR3_TELEPATHY);
			(*f3) |= (TR3_FEATHER);
			(*f1) |= (TR1_SPEED);
			break;
		case MIMIC_VAMPIRE:
			(*f2) |= (TR2_HOLD_LIFE);
			(*f2) |= (TR2_RES_DARK);
			(*f2) |= (TR2_RES_NETHER);
			if (p_ptr->pclass != CLASS_NINJA) (*f3) |= (TR3_LITE);
			(*f2) |= (TR2_RES_POIS);
			(*f2) |= (TR2_RES_COLD);
			(*f3) |= (TR3_SEE_INVIS);
			(*f1) |= (TR1_SPEED);
			break;
		}
	}
	else
	{
	switch (p_ptr->prace)
	{
	case RACE_ELF:
		(*f2) |= (TR2_RES_LITE);
		break;
	case RACE_HOBBIT:
		(*f2) |= (TR2_SUST_DEX);
		break;
	case RACE_GNOME:
		(*f2) |= (TR2_FREE_ACT);
		break;
	case RACE_DWARF:
		(*f2) |= (TR2_RES_BLIND);
		break;
	case RACE_HALF_ORC:
		(*f2) |= (TR2_RES_DARK);
		break;
	case RACE_HALF_TROLL:
		(*f2) |= (TR2_SUST_STR);
		if (p_ptr->lev > 14)
		{
			(*f3) |= (TR3_REGEN);
			if (p_ptr->pclass == CLASS_WARRIOR)
			{
				(*f3) |= (TR3_SLOW_DIGEST);
				/*
				 * Let's not make Regeneration a disadvantage
				 * for the poor warriors who can never learn
				 * a spell that satisfies hunger (actually
				 * neither can rogues, but half-trolls are not
				 * supposed to play rogues)
				 */
			}
		}
		break;
	case RACE_AMBERITE:
		(*f2) |= (TR2_SUST_CON);
		(*f3) |= (TR3_REGEN); /* Amberites heal fast */
		break;
	case RACE_HIGH_ELF:
		(*f2) |= (TR2_RES_LITE);
		(*f3) |= (TR3_SEE_INVIS);
		break;
	case RACE_BARBARIAN:
		(*f2) |= (TR2_RES_FEAR);
		break;
	case RACE_HALF_OGRE:
		(*f2) |= (TR2_SUST_STR);
		(*f2) |= (TR2_RES_DARK);
		break;
	case RACE_HALF_GIANT:
		(*f2) |= (TR2_RES_SHARDS);
		(*f2) |= (TR2_SUST_STR);
		break;
	case RACE_HALF_TITAN:
		(*f2) |= (TR2_RES_CHAOS);
		break;
	case RACE_CYCLOPS:
		(*f2) |= (TR2_RES_SOUND);
		break;
	case RACE_YEEK:
		(*f2) |= (TR2_RES_ACID);
		if (p_ptr->lev > 19)
			(*f2) |= (TR2_IM_ACID);
		break;
	case RACE_KLACKON:
		(*f2) |= (TR2_RES_CONF);
		(*f2) |= (TR2_RES_ACID);
		if (p_ptr->lev > 9)
			(*f1) |= TR1_SPEED;
		break;
	case RACE_KOBOLD:
		(*f2) |= (TR2_RES_POIS);
		break;
	case RACE_NIBELUNG:
		(*f2) |= (TR2_RES_DISEN);
		(*f2) |= (TR2_RES_DARK);
		break;
	case RACE_DARK_ELF:
		(*f2) |= (TR2_RES_DARK);
		if (p_ptr->lev > 19)
			(*f3) |= (TR3_SEE_INVIS);
		break;
	case RACE_DRACONIAN:
		(*f3) |= TR3_FEATHER;
		if (p_ptr->lev > 4)
			(*f2) |= (TR2_RES_FIRE);
		if (p_ptr->lev > 9)
			(*f2) |= (TR2_RES_COLD);
		if (p_ptr->lev > 14)
			(*f2) |= (TR2_RES_ACID);
		if (p_ptr->lev > 19)
			(*f2) |= (TR2_RES_ELEC);
		if (p_ptr->lev > 34)
			(*f2) |= (TR2_RES_POIS);
		break;
	case RACE_MIND_FLAYER:
		(*f2) |= (TR2_SUST_INT);
		(*f2) |= (TR2_SUST_WIS);
		if (p_ptr->lev > 14)
			(*f3) |= (TR3_SEE_INVIS);
		if (p_ptr->lev > 29)
			(*f3) |= (TR3_TELEPATHY);
		break;
	case RACE_IMP:
		(*f2) |= (TR2_RES_FIRE);
		if (p_ptr->lev > 9)
			(*f3) |= (TR3_SEE_INVIS);
		break;
	case RACE_GOLEM:
		(*f3) |= (TR3_SEE_INVIS);
		(*f2) |= (TR2_FREE_ACT);
		(*f2) |= (TR2_RES_POIS);
		(*f3) |= (TR3_SLOW_DIGEST);
		if (p_ptr->lev > 34)
			(*f2) |= (TR2_HOLD_LIFE);
		break;
	case RACE_SKELETON:
		(*f3) |= (TR3_SEE_INVIS);
		(*f2) |= (TR2_RES_SHARDS);
		(*f2) |= (TR2_HOLD_LIFE);
		(*f2) |= (TR2_RES_POIS);
		if (p_ptr->lev > 9)
			(*f2) |= (TR2_RES_COLD);
		break;
	case RACE_ZOMBIE:
		(*f3) |= (TR3_SEE_INVIS);
		(*f2) |= (TR2_HOLD_LIFE);
		(*f2) |= (TR2_RES_NETHER);
		(*f2) |= (TR2_RES_POIS);
		(*f3) |= (TR3_SLOW_DIGEST);
		if (p_ptr->lev > 4)
			(*f2) |= (TR2_RES_COLD);
		break;
	case RACE_VAMPIRE:
		(*f2) |= (TR2_HOLD_LIFE);
		(*f2) |= (TR2_RES_DARK);
		(*f2) |= (TR2_RES_NETHER);
		if (p_ptr->pclass != CLASS_NINJA) (*f3) |= (TR3_LITE);
		(*f2) |= (TR2_RES_POIS);
		(*f2) |= (TR2_RES_COLD);
		break;
	case RACE_SPECTRE:
		(*f3) |= (TR3_FEATHER);
		(*f2) |= (TR2_FREE_ACT);
		(*f2) |= (TR2_RES_COLD);
		(*f3) |= (TR3_SEE_INVIS);
		(*f2) |= (TR2_HOLD_LIFE);
		(*f2) |= (TR2_RES_NETHER);
		(*f2) |= (TR2_RES_POIS);
		(*f3) |= (TR3_SLOW_DIGEST);
		/* XXX pass_wall */
		if (p_ptr->lev > 34)
			(*f3) |= TR3_TELEPATHY;
		break;
	case RACE_SPRITE:
		(*f2) |= (TR2_RES_LITE);
		(*f3) |= (TR3_FEATHER);
		if (p_ptr->lev > 9)
			(*f1) |= (TR1_SPEED);
		break;
	case RACE_BEASTMAN:
		(*f2) |= (TR2_RES_SOUND);
		(*f2) |= (TR2_RES_CONF);
		break;
	case RACE_ANGEL:
		(*f3) |= (TR3_FEATHER);
		(*f3) |= (TR3_SEE_INVIS);
		break;
	case RACE_DEMON:
		(*f2) |= (TR2_RES_FIRE);
		(*f2) |= (TR2_RES_NETHER);
		(*f2) |= (TR2_HOLD_LIFE);
		if (p_ptr->lev > 9)
			(*f3) |= (TR3_SEE_INVIS);
		break;
	case RACE_DUNADAN:
		(*f2) |= (TR2_SUST_CON);
		break;
	case RACE_S_FAIRY:
		(*f3) |= (TR3_FEATHER);
		break;
	case RACE_KUTA:
		(*f2) |= (TR2_RES_CONF);
		break;
	case RACE_ANDROID:
		(*f2) |= (TR2_FREE_ACT);
		(*f2) |= (TR2_RES_POIS);
		(*f3) |= (TR3_SLOW_DIGEST);
		(*f2) |= (TR2_HOLD_LIFE);
		break;
	default:
		; /* Do nothing */
	}
	}

	/* Mutations */
	if (p_ptr->muta3)
	{
		if (p_ptr->muta3 & MUT3_FLESH_ROT)
		{
			(*f3) &= ~(TR3_REGEN);
		}

		if ((p_ptr->muta3 & MUT3_XTRA_FAT) ||
			(p_ptr->muta3 & MUT3_XTRA_LEGS) ||
			(p_ptr->muta3 & MUT3_SHORT_LEG))
		{
			(*f1) |= TR1_SPEED;
		}

		if (p_ptr->muta3  & MUT3_ELEC_TOUC)
		{
			(*f3) |= TR3_SH_ELEC;
		}

		if (p_ptr->muta3 & MUT3_FIRE_BODY)
		{
			(*f3) |= TR3_SH_FIRE;
			(*f3) |= TR3_LITE;
		}

		if (p_ptr->muta3 & MUT3_WINGS)
		{
			(*f3) |= TR3_FEATHER;
		}

		if (p_ptr->muta3 & MUT3_FEARLESS)
		{
			(*f2) |= (TR2_RES_FEAR);
		}

		if (p_ptr->muta3 & MUT3_REGEN)
		{
			(*f3) |= TR3_REGEN;
		}

		if (p_ptr->muta3 & MUT3_ESP)
		{
			(*f3) |= TR3_TELEPATHY;
		}

		if (p_ptr->muta3 & MUT3_MOTION)
		{
			(*f2) |= TR2_FREE_ACT;
		}
	}

	if (p_ptr->pseikaku == SEIKAKU_SEXY)
		(*f3) |= TR3_AGGRAVATE;
	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN)
	{
		(*f2) |= (TR2_RES_BLIND);
		(*f2) |= (TR2_RES_CONF);
		(*f2) |= (TR2_HOLD_LIFE);
		(*f3) |= (TR3_LITE);
		if (p_ptr->lev > 9)
			(*f1) |= (TR1_SPEED);
	}
	if (p_ptr->special_defense & KATA_FUUJIN)
		(*f2) |= TR2_REFLECT;
	if (p_ptr->special_defense & KAMAE_GENBU)
		(*f2) |= TR2_REFLECT;
	if (p_ptr->special_defense & KAMAE_SUZAKU)
		(*f3) |= TR3_FEATHER;
	if (p_ptr->special_defense & KAMAE_SEIRYU)
	{
		(*f2) |= (TR2_RES_FIRE);
		(*f2) |= (TR2_RES_COLD);
		(*f2) |= (TR2_RES_ACID);
		(*f2) |= (TR2_RES_ELEC);
		(*f2) |= (TR2_RES_POIS);
		(*f3) |= (TR3_FEATHER);
		(*f3) |= (TR3_SH_FIRE);
		(*f3) |= (TR3_SH_ELEC);
		(*f3) |= (TR3_SH_COLD);
	}
	if (p_ptr->special_defense & KATA_MUSOU)
	{
		(*f2) |= TR2_RES_FEAR;
		(*f2) |= TR2_RES_LITE;
		(*f2) |= TR2_RES_DARK;
		(*f2) |= TR2_RES_BLIND;
		(*f2) |= TR2_RES_CONF;
		(*f2) |= TR2_RES_SOUND;
		(*f2) |= TR2_RES_SHARDS;
		(*f2) |= TR2_RES_NETHER;
		(*f2) |= TR2_RES_NEXUS;
		(*f2) |= TR2_RES_CHAOS;
		(*f2) |= TR2_RES_DISEN;
		(*f2) |= TR2_REFLECT;
		(*f2) |= TR2_HOLD_LIFE;
		(*f2) |= TR2_FREE_ACT;
		(*f3) |= TR3_SH_FIRE;
		(*f3) |= TR3_SH_ELEC;
		(*f3) |= TR3_SH_COLD;
		(*f3) |= TR3_FEATHER;
		(*f3) |= TR3_LITE;
		(*f3) |= TR3_SEE_INVIS;
		(*f3) |= TR3_TELEPATHY;
		(*f3) |= TR3_SLOW_DIGEST;
		(*f3) |= TR3_REGEN;
		(*f2) |= (TR2_SUST_STR);
		(*f2) |= (TR2_SUST_INT);
		(*f2) |= (TR2_SUST_WIS);
		(*f2) |= (TR2_SUST_DEX);
		(*f2) |= (TR2_SUST_CON);
		(*f2) |= (TR2_SUST_CHR);
	}
}


static void tim_player_flags(u32b *f1, u32b *f2, u32b *f3, bool im_and_res)
{
	/* Clear */
	(*f1) = (*f2) = (*f3) = 0L;

	if (p_ptr->hero || p_ptr->shero || music_singing(MUSIC_HERO) || music_singing(MUSIC_SHERO))
		(*f2) |= TR2_RES_FEAR;
	if (p_ptr->tim_invis)
		(*f3) |= TR3_SEE_INVIS;
	if (p_ptr->tim_regen)
		(*f3) |= TR3_REGEN;
	if (p_ptr->tim_esp || music_singing(MUSIC_MIND))
		(*f3) |= TR3_TELEPATHY;
	if (p_ptr->fast || p_ptr->slow || music_singing(MUSIC_SPEED) || music_singing(MUSIC_SHERO))
		(*f1) |= TR1_SPEED;
	if  ((p_ptr->special_defense & KATA_MUSOU) || music_singing(MUSIC_RESIST))
	{
		(*f2) |= (TR2_RES_FIRE);
		(*f2) |= (TR2_RES_COLD);
		(*f2) |= (TR2_RES_ACID);
		(*f2) |= (TR2_RES_ELEC);
		(*f2) |= (TR2_RES_POIS);
	}
	if (im_and_res)
	{
		if (p_ptr->oppose_acid && !(p_ptr->special_defense & DEFENSE_ACID) && !((prace_is_(RACE_YEEK)) && (p_ptr->lev > 19)))
			(*f2) |= TR2_RES_ACID;
		if (p_ptr->oppose_elec && !(p_ptr->special_defense & DEFENSE_ELEC))
			(*f2) |= TR2_RES_ELEC;
		if (p_ptr->oppose_fire && !(p_ptr->special_defense & DEFENSE_FIRE))
			(*f2) |= TR2_RES_FIRE;
		if (p_ptr->oppose_cold && !(p_ptr->special_defense & DEFENSE_COLD))
			(*f2) |= TR2_RES_COLD;
	}
	else
	{
		if (p_ptr->oppose_acid)
			(*f2) |= TR2_RES_ACID;
		if (p_ptr->oppose_elec)
			(*f2) |= TR2_RES_ELEC;
		if (p_ptr->oppose_fire)
			(*f2) |= TR2_RES_FIRE;
		if (p_ptr->oppose_cold)
			(*f2) |= TR2_RES_COLD;
	}
	if (p_ptr->oppose_pois)
		(*f2) |= TR2_RES_POIS;
	if (p_ptr->special_attack & ATTACK_ACID)
		(*f1) |= TR1_BRAND_ACID;
	if (p_ptr->special_attack & ATTACK_ELEC)
		(*f1) |= TR1_BRAND_ELEC;
	if (p_ptr->special_attack & ATTACK_FIRE)
		(*f1) |= TR1_BRAND_FIRE;
	if (p_ptr->special_attack & ATTACK_COLD)
		(*f1) |= TR1_BRAND_COLD;
	if (p_ptr->special_attack & ATTACK_POIS)
		(*f1) |= TR1_BRAND_POIS;
	if (p_ptr->special_defense & DEFENSE_ACID)
		(*f2) |= TR2_IM_ACID;
	if (p_ptr->special_defense & DEFENSE_ELEC)
		(*f2) |= TR2_IM_ELEC;
	if (p_ptr->special_defense & DEFENSE_FIRE)
		(*f2) |= TR2_IM_FIRE;
	if (p_ptr->special_defense & DEFENSE_COLD)
		(*f2) |= TR2_IM_COLD;
	if (p_ptr->wraith_form)
		(*f2) |= TR2_REFLECT;
	/* by henkma */
	if (p_ptr->tim_reflect){
		(*f2) |= TR2_REFLECT;
	}

	if (p_ptr->magicdef)
	{
		(*f2) |= TR2_RES_BLIND;
		(*f2) |= TR2_RES_CONF;
		(*f2) |= TR2_REFLECT;
		(*f2) |= TR2_FREE_ACT;
		(*f3) |= TR3_FEATHER;
	}
	if (p_ptr->tim_res_nether)
	{
		(*f2) |= TR2_RES_NETHER;
	}
	if (p_ptr->tim_sh_fire)
	{
		(*f3) |= TR3_SH_FIRE;
	}
	if (p_ptr->ult_res)
	{
		(*f2) |= TR2_RES_FEAR;
		(*f2) |= TR2_RES_LITE;
		(*f2) |= TR2_RES_DARK;
		(*f2) |= TR2_RES_BLIND;
		(*f2) |= TR2_RES_CONF;
		(*f2) |= TR2_RES_SOUND;
		(*f2) |= TR2_RES_SHARDS;
		(*f2) |= TR2_RES_NETHER;
		(*f2) |= TR2_RES_NEXUS;
		(*f2) |= TR2_RES_CHAOS;
		(*f2) |= TR2_RES_DISEN;
		(*f2) |= TR2_REFLECT;
		(*f2) |= TR2_HOLD_LIFE;
		(*f2) |= TR2_FREE_ACT;
		(*f3) |= TR3_SH_FIRE;
		(*f3) |= TR3_SH_ELEC;
		(*f3) |= TR3_SH_COLD;
		(*f3) |= TR3_FEATHER;
		(*f3) |= TR3_LITE;
		(*f3) |= TR3_SEE_INVIS;
		(*f3) |= TR3_TELEPATHY;
		(*f3) |= TR3_SLOW_DIGEST;
		(*f3) |= TR3_REGEN;
		(*f2) |= (TR2_SUST_STR);
		(*f2) |= (TR2_SUST_INT);
		(*f2) |= (TR2_SUST_WIS);
		(*f2) |= (TR2_SUST_DEX);
		(*f2) |= (TR2_SUST_CON);
		(*f2) |= (TR2_SUST_CHR);
	}
}


/*
 * Equippy chars
 */
static void display_player_equippy(int y, int x)
{
	int i;

	byte a;
	char c;

	object_type *o_ptr;


	/* Dump equippy chars */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Object */
		o_ptr = &inventory[i];

		a = object_attr(o_ptr);
		c = object_char(o_ptr);

		/* Clear the part of the screen */
		if (!equippy_chars || !o_ptr->k_idx)
		{
			c = ' ';
			a = TERM_DARK;
		}

		/* Dump */
		Term_putch(x + i - INVEN_RARM, y, a, c);
	}
}


void print_equippy(void)
{
	display_player_equippy(ROW_EQUIPPY, COL_EQUIPPY);
}

/*
 *
 */

static void known_obj_immunity(u32b *f1, u32b *f2, u32b *f3)
{
	int i;

	/* Clear */
	(*f1) = (*f2) = (*f3) = 0L;

	/* Check equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b    o_f1, o_f2, o_f3;

		object_type *o_ptr;

		/* Object */
		o_ptr = &inventory[i];

		if (!o_ptr->k_idx) continue;

		/* Known flags */
		object_flags_known(o_ptr, &o_f1, &o_f2, &o_f3);

		if (o_f2 & TR2_IM_ACID) (*f2) |= TR2_RES_ACID;
		if (o_f2 & TR2_IM_ELEC) (*f2) |= TR2_RES_ELEC;
		if (o_f2 & TR2_IM_FIRE) (*f2) |= TR2_RES_FIRE;
		if (o_f2 & TR2_IM_COLD) (*f2) |= TR2_RES_COLD;
	}
}

static void player_immunity(u32b *f1, u32b *f2, u32b *f3)
{
	/* Clear */
	(*f1) = (*f2) = (*f3) = 0L;

	if (prace_is_(RACE_SPECTRE))
		(*f2) |= TR2_RES_NETHER;
	if (p_ptr->mimic_form == MIMIC_VAMPIRE || prace_is_(RACE_VAMPIRE))
		(*f2) |= TR2_RES_DARK;
	if (p_ptr->mimic_form == MIMIC_DEMON_LORD)
		(*f2) |= TR2_RES_FIRE;
	else if (prace_is_(RACE_YEEK) && p_ptr->lev > 19)
		(*f2) |= TR2_RES_ACID;
}

static void tim_player_immunity(u32b *f1, u32b *f2, u32b *f3)
{
	/* Clear */
	(*f1) = (*f2) = (*f3) = 0L;

	if (p_ptr->special_defense & DEFENSE_ACID)
		(*f2) |= TR2_RES_ACID;
	if (p_ptr->special_defense & DEFENSE_ELEC)
		(*f2) |= TR2_RES_ELEC;
	if (p_ptr->special_defense & DEFENSE_FIRE)
		(*f2) |= TR2_RES_FIRE;
	if (p_ptr->special_defense & DEFENSE_COLD)
		(*f2) |= TR2_RES_COLD;
	if (p_ptr->wraith_form)
		(*f2) |= TR2_RES_DARK;
}

static void player_vuln_flags(u32b *f1, u32b *f2, u32b *f3)
{
	/* Clear */
	(*f1) = (*f2) = (*f3) = 0L;

	if ((p_ptr->muta3 & MUT3_VULN_ELEM) || (p_ptr->special_defense & KATA_KOUKIJIN))
	{
		(*f2) |= TR2_RES_ACID;
		(*f2) |= TR2_RES_ELEC;
		(*f2) |= TR2_RES_FIRE;
		(*f2) |= TR2_RES_COLD;
	}
	if (prace_is_(RACE_ANDROID))
		(*f2) |= TR2_RES_ELEC;
	if (prace_is_(RACE_ENT))
		(*f2) |= TR2_RES_FIRE;
	if (prace_is_(RACE_VAMPIRE) || prace_is_(RACE_S_FAIRY) ||
	    (p_ptr->mimic_form == MIMIC_VAMPIRE))
		(*f2) |= TR2_RES_LITE;
}

/*
 * Helper function, see below
 */
static void display_player_flag_aux(int row, int col, char *header,
				    int n, u32b flag1, u32b flag2,
				    u32b im_f[], u32b vul_f)
{
	int     i;
	u32b    f[3];
	bool    vuln = FALSE;

	if ((vul_f & flag1) && !((im_f[0] | im_f[1] | im_f[2]) & flag1))
		vuln = TRUE;

	/* Header */
	c_put_str(TERM_WHITE, header, row, col);

	/* Advance */
	col += strlen(header) + 1;

	/* Check equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr;
		f[0] = f[1] = f[2] = 0L;

		/* Object */
		o_ptr = &inventory[i];

		/* Known flags */
		object_flags_known(o_ptr, &f[0], &f[1], &f[2]);

		/* Default */
		c_put_str(vuln ? TERM_RED : TERM_SLATE, ".", row, col);

		/* Check flags */
		if (f[n - 1] & flag1) c_put_str(vuln ? TERM_L_RED : TERM_WHITE, "+", row, col);
		if (f[n - 1] & flag2) c_put_str(TERM_WHITE, "*", row, col);

		/* Advance */
		col++;
	}

	/* Player flags */
	player_flags(&f[0], &f[1], &f[2]);

	/* Default */
	c_put_str(vuln ? TERM_RED : TERM_SLATE, ".", row, col);

	/* Check flags */
	if (f[n-1] & flag1) c_put_str(vuln ? TERM_L_RED : TERM_WHITE, "+", row, col);

	/* Timed player flags */
	tim_player_flags(&f[0], &f[1], &f[2], TRUE);

	/* Check flags */
	if (f[n-1] & flag1) c_put_str(vuln ? TERM_ORANGE : TERM_YELLOW, "#", row, col);

	/* Immunity */
	if (im_f[2] & flag1) c_put_str(TERM_YELLOW, "*", row, col);
	if (im_f[1] & flag1) c_put_str(TERM_WHITE, "*", row, col);

	/* Vulnerability */
	if (vuln) c_put_str(TERM_RED, "v", row, col + 1);
}


/*
 * Special display, part 1
 */
static void display_player_flag_info(void)
{
	int row;
	int col;

	u32b im_f[3][3], vul_f[3];

	known_obj_immunity(&im_f[0][0], &im_f[1][0], &im_f[2][0]);
	player_immunity(&im_f[0][1], &im_f[1][1], &im_f[2][1]);
	tim_player_immunity(&im_f[0][2], &im_f[1][2], &im_f[2][2]);

	player_vuln_flags(&vul_f[0], &vul_f[1], &vul_f[2]);

	/*** Set 1 ***/

	row = 12;
	col = 1;

	display_player_equippy(row-2, col+8);
	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
display_player_flag_aux(row+0, col, "�ѻ�  :", 2, TR2_RES_ACID, TR2_IM_ACID, im_f[1], vul_f[1]);
display_player_flag_aux(row+1, col, "���ŷ�:", 2, TR2_RES_ELEC, TR2_IM_ELEC, im_f[1], vul_f[1]);
display_player_flag_aux(row+2, col, "�Ѳб�:", 2, TR2_RES_FIRE, TR2_IM_FIRE, im_f[1], vul_f[1]);
display_player_flag_aux(row+3, col, "���䵤:", 2, TR2_RES_COLD, TR2_IM_COLD, im_f[1], vul_f[1]);
display_player_flag_aux(row+4, col, "����  :", 2, TR2_RES_POIS, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+5, col, "������:", 2, TR2_RES_LITE, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+6, col, "�ѰŹ�:", 2, TR2_RES_DARK, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+7, col, "������:", 2, TR2_RES_SHARDS, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+8, col, "������:", 2, TR2_RES_BLIND, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+9, col, "�Ѻ���:", 2, TR2_RES_CONF, 0, im_f[1], vul_f[1]);
#else
	display_player_flag_aux(row+0, col, "Acid  :", 2, TR2_RES_ACID, TR2_IM_ACID, im_f[1], vul_f[1]);
	display_player_flag_aux(row+1, col, "Elec  :", 2, TR2_RES_ELEC, TR2_IM_ELEC, im_f[1], vul_f[1]);
	display_player_flag_aux(row+2, col, "Fire  :", 2, TR2_RES_FIRE, TR2_IM_FIRE, im_f[1], vul_f[1]);
	display_player_flag_aux(row+3, col, "Cold  :", 2, TR2_RES_COLD, TR2_IM_COLD, im_f[1], vul_f[1]);
	display_player_flag_aux(row+4, col, "Poison:", 2, TR2_RES_POIS, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+5, col, "Light :", 2, TR2_RES_LITE, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+6, col, "Dark  :", 2, TR2_RES_DARK, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+7, col, "Shard :", 2, TR2_RES_SHARDS, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+8, col, "Blind :", 2, TR2_RES_BLIND, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+9, col, "Conf  :", 2, TR2_RES_CONF, 0, im_f[1], vul_f[1]);
#endif


	/*** Set 2 ***/

	row = 12;
	col = 26;

	display_player_equippy(row-2, col+8);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+8);

#ifdef JP
display_player_flag_aux(row+0, col, "�ѹ첻:", 2, TR2_RES_SOUND, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+1, col, "���Ϲ�:", 2, TR2_RES_NETHER, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+2, col, "�Ѱ���:", 2, TR2_RES_NEXUS, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+3, col, "�ѥ���:", 2, TR2_RES_CHAOS, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+4, col, "������:", 2, TR2_RES_DISEN, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+5, col, "�Ѷ���:", 2, TR2_RES_FEAR, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+6, col, "ȿ��  :", 2, TR2_REFLECT, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+7, col, "�бꥪ:", 3, TR3_SH_FIRE, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+8, col, "�ŵ���:", 3, TR3_SH_ELEC, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+9, col, "�䵤��:", 3, TR3_SH_COLD, 0, im_f[2], vul_f[2]);
#else
	display_player_flag_aux(row+0, col, "Sound :", 2, TR2_RES_SOUND, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+1, col, "Nether:", 2, TR2_RES_NETHER, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+2, col, "Nexus :", 2, TR2_RES_NEXUS, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+3, col, "Chaos :", 2, TR2_RES_CHAOS, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+4, col, "Disnch:", 2, TR2_RES_DISEN, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+5, col, "Fear  :", 2, TR2_RES_FEAR, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+6, col, "Reflct:", 2, TR2_REFLECT, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+7, col, "AuFire:", 3, TR3_SH_FIRE, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+8, col, "AuElec:", 3, TR3_SH_ELEC, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+9, col, "AuCold:", 3, TR3_SH_COLD, 0, im_f[2], vul_f[2]);
#endif


	/*** Set 3 ***/

	row = 12;
	col = 51;

	display_player_equippy(row-2, col+12);

	c_put_str(TERM_WHITE, "abcdefghijkl@", row-1, col+12);

#ifdef JP
display_player_flag_aux(row+0, col, "��®      :", 1, TR1_SPEED, 0, im_f[0], vul_f[0]);
display_player_flag_aux(row+1, col, "������    :", 2, TR2_FREE_ACT, 0, im_f[1], vul_f[1]);
display_player_flag_aux(row+2, col, "Ʃ���λ�ǧ:", 3, TR3_SEE_INVIS, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+3, col, "�и����ݻ�:", 2, TR2_HOLD_LIFE, 0, im_f[2], vul_f[1]);
display_player_flag_aux(row+4, col, "�ƥ�ѥ���:", 3, TR3_TELEPATHY, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+5, col, "�پò�    :", 3, TR3_SLOW_DIGEST, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+6, col, "�޲���    :", 3, TR3_REGEN, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+7, col, "��ͷ      :", 3, TR3_FEATHER, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+8, col, "�ʱ����  :", 3, TR3_LITE, 0, im_f[2], vul_f[2]);
display_player_flag_aux(row+9, col, "����      :", 3, (TR3_CURSED | TR3_HEAVY_CURSE), TR3_PERMA_CURSE, im_f[2], vul_f[2]);
#else
	display_player_flag_aux(row+0, col, "Speed     :", 1, TR1_SPEED, 0, im_f[0], vul_f[0]);
	display_player_flag_aux(row+1, col, "FreeAction:", 2, TR2_FREE_ACT, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+2, col, "SeeInvisi.:", 3, TR3_SEE_INVIS, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+3, col, "Hold Life :", 2, TR2_HOLD_LIFE, 0, im_f[1], vul_f[1]);
	display_player_flag_aux(row+4, col, "Telepathy :", 3, TR3_TELEPATHY, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+5, col, "SlowDigest:", 3, TR3_SLOW_DIGEST, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+6, col, "Regene.   :", 3, TR3_REGEN, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+7, col, "Levitation:", 3, TR3_FEATHER, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+8, col, "Perm Lite :", 3, TR3_LITE, 0, im_f[2], vul_f[2]);
	display_player_flag_aux(row+9, col, "Cursed    :", 3, (TR3_CURSED | TR3_HEAVY_CURSE), TR3_PERMA_CURSE, im_f[2], vul_f[2]);
#endif

}


/*
 * Special display, part 2a
 */
static void display_player_misc_info(void)
{
	char	buf[80];
	char	tmp[80];

	/* Display basics */
#ifdef JP
put_str("̾��  :", 1, 26);
put_str("����  :", 3, 1);
put_str("��²  :", 4, 1);
put_str("����  :", 5, 1);
#else
	put_str("Name  :", 1, 26);
	put_str("Sex   :", 3, 1);
	put_str("Race  :", 4, 1);
	put_str("Class :", 5, 1);
#endif

	strcpy(tmp,ap_ptr->title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"��");
#else
		strcat(tmp," ");
#endif
	strcat(tmp,player_name);

	c_put_str(TERM_L_BLUE, tmp, 1, 34);
	c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 9);
	c_put_str(TERM_L_BLUE, (p_ptr->mimic_form ? mimic_info[p_ptr->mimic_form].title : rp_ptr->title), 4, 9);
	c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 9);

	/* Display extras */
#ifdef JP
put_str("��٥�:", 6, 1);
put_str("�ȣ�  :", 7, 1);
put_str("�ͣ�  :", 8, 1);
#else
	put_str("Level :", 6, 1);
	put_str("Hits  :", 7, 1);
	put_str("Mana  :", 8, 1);
#endif


	(void)sprintf(buf, "%d", (int)p_ptr->lev);
	c_put_str(TERM_L_BLUE, buf, 6, 9);
	(void)sprintf(buf, "%d/%d", (int)p_ptr->chp, (int)p_ptr->mhp);
	c_put_str(TERM_L_BLUE, buf, 7, 9);
	(void)sprintf(buf, "%d/%d", (int)p_ptr->csp, (int)p_ptr->msp);
	c_put_str(TERM_L_BLUE, buf, 8, 9);
}


/*
 * Special display, part 2b
 *
 * How to print out the modifications and sustains.
 * Positive mods with no sustain will be light green.
 * Positive mods with a sustain will be dark green.
 * Sustains (with no modification) will be a dark green 's'.
 * Negative mods (from a curse) will be red.
 * Huge mods (>9), like from MICoMorgoth, will be a '*'
 * No mod, no sustain, will be a slate '.'
 */
static void display_player_stat_info(void)
{
	int i, e_adj;
	int stat_col, stat;
	int row, col;

	object_type *o_ptr;
	u32b f1, f2, f3;
	s16b k_idx;

	byte a;
	char c;

	char buf[80];


	/* Column */
	stat_col = 22;

	/* Row */
	row = 3;

	/* Print out the labels for the columns */
#ifdef JP
c_put_str(TERM_WHITE, "ǽ��", row, stat_col+1);
c_put_str(TERM_BLUE, "  ����", row, stat_col+7);
c_put_str(TERM_L_BLUE, " �� �� �� �� ", row, stat_col+13);
c_put_str(TERM_L_GREEN, "���", row, stat_col+28);
c_put_str(TERM_YELLOW, "����", row, stat_col+33);
#else
	c_put_str(TERM_WHITE, "Stat", row, stat_col+1);
	c_put_str(TERM_BLUE, "  Base", row, stat_col+7);
	c_put_str(TERM_L_BLUE, "RacClaPerMod", row, stat_col+13);
	c_put_str(TERM_L_GREEN, "Actual", row, stat_col+26);
	c_put_str(TERM_YELLOW, "Current", row, stat_col+32);
#endif


	/* Display the stats */
	for (i = 0; i < 6; i++)
	{
		int r_adj;

		if (p_ptr->mimic_form) r_adj = mimic_info[p_ptr->mimic_form].r_adj[i];
		else r_adj = rp_ptr->r_adj[i];

		/* Calculate equipment adjustment */
		e_adj = 0;

		/* Icky formula to deal with the 18 barrier */
		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - p_ptr->stat_max[i]) / 10;
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - p_ptr->stat_max[i];
		if ((p_ptr->stat_max[i] <= 18) && (p_ptr->stat_top[i] > 18))
			e_adj = (p_ptr->stat_top[i] - 18) / 10 - p_ptr->stat_max[i] + 18;

		if ((p_ptr->stat_max[i] > 18) && (p_ptr->stat_top[i] <= 18))
			e_adj = p_ptr->stat_top[i] - (p_ptr->stat_max[i] - 19) / 10 - 19;

		if (prace_is_(RACE_ENT))
		{
			switch (i)
			{
				case A_STR:
				case A_CON:
					if (p_ptr->lev > 25) r_adj++;
					if (p_ptr->lev > 40) r_adj++;
					if (p_ptr->lev > 45) r_adj++;
					break;
				case A_DEX:
					if (p_ptr->lev > 25) r_adj--;
					if (p_ptr->lev > 40) r_adj--;
					if (p_ptr->lev > 45) r_adj--;
					break;
			}
		}

		e_adj -= r_adj;
		e_adj -= cp_ptr->c_adj[i];
		e_adj -= ap_ptr->a_adj[i];

		/* Reduced name of stat */
#ifdef JP
		c_put_str(TERM_WHITE, stat_names[i], row + i+1, stat_col+1);
#else
		c_put_str(TERM_WHITE, stat_names_reduced[i], row + i+1, stat_col+1);
#endif


		/* Internal "natural" max value.  Maxes at 18/100 */
		/* This is useful to see if you are maxed out */
		cnv_stat(p_ptr->stat_max[i], buf);
		if (p_ptr->stat_max[i] == p_ptr->stat_max_max[i])
		{
			c_put_str(TERM_WHITE, "!", row + i+1, stat_col + 6);
		}
		c_put_str(TERM_BLUE, buf, row + i+1, stat_col + 13 - strlen(buf));

		/* Race, class, and equipment modifiers */
		(void)sprintf(buf, "%3d", r_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 13);
		(void)sprintf(buf, "%3d", (int)cp_ptr->c_adj[i]);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 16);
		(void)sprintf(buf, "%3d", (int)ap_ptr->a_adj[i]);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 19);
		(void)sprintf(buf, "%3d", (int)e_adj);
		c_put_str(TERM_L_BLUE, buf, row + i+1, stat_col + 22);

		/* Actual maximal modified value */
		cnv_stat(p_ptr->stat_top[i], buf);
		c_put_str(TERM_L_GREEN, buf, row + i+1, stat_col + 26);

		/* Only display stat_use if not maximal */
		if (p_ptr->stat_use[i] < p_ptr->stat_top[i])
		{
			cnv_stat(p_ptr->stat_use[i], buf);
			c_put_str(TERM_YELLOW, buf, row + i+1, stat_col + 33);
		}
	}

	/* Column */
	col = stat_col + 41;

	/* Header and Footer */
	c_put_str(TERM_WHITE, "abcdefghijkl@", row, col);
#ifdef JP
c_put_str(TERM_L_GREEN, "ǽ�Ͻ���", row - 1, col);
#else
	c_put_str(TERM_L_GREEN, "Modification", row - 1, col);
#endif


	/* Process equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Access object */
		o_ptr = &inventory[i];

		/* Object kind */
		k_idx = o_ptr->k_idx;

		/* Acquire "known" flags */
		object_flags_known(o_ptr, &f1, &f2, &f3);

		/* Initialize color based of sign of pval. */
		for (stat = 0; stat < 6; stat++)
		{
			/* Default */
			a = TERM_SLATE;
			c = '.';

			/* Boost */
			if (f1 & 1 << stat)
			{
				/* Default */
				c = '*';

				/* Good */
				if (o_ptr->pval > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (o_ptr->pval < 10) c = '0' + o_ptr->pval;
				}

				if (f2 & 1 << stat)
				{
					/* Dark green for sustained stats */
					a = TERM_GREEN;
				}

				/* Bad */
				if (o_ptr->pval < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (o_ptr->pval < 10) c = '0' - o_ptr->pval;
				}
			}

			/* Sustain */
			else if (f2 & 1 << stat)
			{
				/* Dark green "s" */
				a = TERM_GREEN;
				c = 's';
			}

			/* Dump proper character */
			Term_putch(col, row + stat+1, a, c);
		}

		/* Advance */
		col++;
	}

	/* Player flags */
	player_flags(&f1, &f2, &f3);

	/* Check stats */
	for (stat = 0; stat < 6; stat++)
	{
		/* Default */
		a = TERM_SLATE;
		c = '.';

		/* Mutations ... */
		if (p_ptr->muta3 || p_ptr->tsuyoshi)
		{
			int dummy = 0;

			if (stat == A_STR)
			{
				if (p_ptr->muta3 & MUT3_HYPER_STR) dummy += 4;
				if (p_ptr->muta3 & MUT3_PUNY) dummy -= 4;
				if (p_ptr->tsuyoshi) dummy += 4;
			}
			else if (stat == A_WIS || stat == A_INT)
			{
				if (p_ptr->muta3 & MUT3_HYPER_INT) dummy += 4;
				if (p_ptr->muta3 & MUT3_MORONIC) dummy -= 4;
			}
			else if (stat == A_DEX)
			{
				if (p_ptr->muta3 & MUT3_IRON_SKIN) dummy -= 1;
				if (p_ptr->muta3 & MUT3_LIMBER) dummy += 3;
				if (p_ptr->muta3 & MUT3_ARTHRITIS) dummy -= 3;
			}
			else if (stat == A_CON)
			{
				if (p_ptr->muta3 & MUT3_RESILIENT) dummy += 4;
				if (p_ptr->muta3 & MUT3_XTRA_FAT) dummy += 2;
				if (p_ptr->muta3 & MUT3_ALBINO) dummy -= 4;
				if (p_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 2;
				if (p_ptr->tsuyoshi) dummy += 4;
			}
			else if (stat == A_CHR)
			{
				if (p_ptr->muta3 & MUT3_SILLY_VOI) dummy -= 4;
				if (p_ptr->muta3 & MUT3_BLANK_FAC) dummy -= 1;
				if (p_ptr->muta3 & MUT3_FLESH_ROT) dummy -= 1;
				if (p_ptr->muta3 & MUT3_SCALES) dummy -= 1;
				if (p_ptr->muta3 & MUT3_WART_SKIN) dummy -= 2;
				if (p_ptr->muta3 & MUT3_ILL_NORM) dummy = 0;
			}

			/* Boost */
			if (dummy)
			{
				/* Default */
				c = '*';

				/* Good */
				if (dummy > 0)
				{
					/* Good */
					a = TERM_L_GREEN;

					/* Label boost */
					if (dummy < 10) c = '0' + dummy;
				}

				/* Bad */
				if (dummy < 0)
				{
					/* Bad */
					a = TERM_RED;

					/* Label boost */
					if (dummy < 10) c = '0' - dummy;
				}
			}
		}


		/* Sustain */
		if (f2 & 1<<stat)
		{
			/* Dark green "s" */
			a = TERM_GREEN;
			c = 's';
		}


		/* Dump */
		Term_putch(col, row + stat+1, a, c);
	}
}


/*
 * Object flag names
 */
static cptr object_flag_names[96] =
{
#ifdef JP
"+����",
"+��ǽ",
"+����",
"+����",
"+�ѵ�",
"+̥��",
#else
	"Add Str",
	"Add Int",
	"Add Wis",
	"Add Dex",
	"Add Con",
	"Add Chr",
#endif

#ifdef JP
	"��ƻ��",

	"����",
#else
	"M.Item-Mas",

	"Force wep.",
#endif

#ifdef JP
"+��̩��ư",
"+õ��",
"+�ֳ�����",
"+����",
"+���ԡ���",
"+�Ƿ���",
"����������",
"�۷�",
"ưʪ ����",
"�ٰ� ����",
"�Ի� ����",
"���� ����",
"����������",
"�ȥ������",
"��� ����",
"ζ ����",
"ζ ������",
"�Կ�",
"�Ͽ�ȯ��",
"��°������",
"��°������",
"��°������",
"��°������",
"��°������",
#else
	"Add Stea.",
	"Add Sear.",
	"Add Infra",
	"Add Tun..",
	"Add Speed",
	"Add Blows",
	"Chaotic",
	"Vampiric",
	"Slay Anim.",
	"Slay Evil",
	"Slay Und.",
	"Slay Demon",
	"Slay Orc",
	"Slay Troll",
	"Slay Giant",
	"Slay Drag.",
	"Kill Drag.",
	"Sharpness",
	"Impact",
	"Poison Brd",
	"Acid Brand",
	"Elec Brand",
	"Fire Brand",
	"Cold Brand",
#endif


#ifdef JP
"���� �ݻ�",
"��ǽ �ݻ�",
"���� �ݻ�",
"���� �ݻ�",
"�ѵ� �ݻ�",
"̥�� �ݻ�",
#else
	"Sust Str",
	"Sust Int",
	"Sust Wis",
	"Sust Dex",
	"Sust Con",
	"Sust Chr",
#endif

	NULL,
	NULL,
#ifdef JP
"Ķ�ѻ�  ",
"Ķ���ŷ�",
"Ķ�Ѳб�",
"Ķ���䵤",
#else
	"Imm Acid",
	"Imm Elec",
	"Imm Fire",
	"Imm Cold",
#endif

	NULL,
#ifdef JP
"ȿ��",
"������",
"�и����ݻ�",
#else
	"Reflect",
	"Free Act",
	"Hold Life",
#endif

#ifdef JP
"�ѻ�  ",
"���ŷ�",
"�Ѳб�",
"���䵤",
"����  ",
"�Ѷ���",
"������",
"�ѰŹ�",
"������",
"�Ѻ���",
"�ѹ첻",
"������",
"���Ϲ�",
"�Ѱ���",
"�ѥ���",
"������",
#else
	"Res Acid",
	"Res Elec",
	"Res Fire",
	"Res Cold",
	"Res Pois",
	"Res Fear",
	"Res Lite",
	"Res Dark",
	"Res Blind",
	"Res Conf",
	"Res Sound",
	"Res Shard",
	"Res Neth",
	"Res Nexus",
	"Res Chaos",
	"Res Disen",
#endif




#ifdef JP
	"�бꥪ����",

	"�ŵ�������",
#else
	"Aura Fire",

	"Aura Elec",
#endif

 	NULL,
#ifdef JP
	"�䵤������",
#else
	"Aura Cold",
#endif
#ifdef JP
"�ɥƥ��",
"ȿ��ˡ",
"����������",
"�ٰ��ʱ�ǰ",
NULL,
"Hide Type",
"Show Mods",
"�������ʪ",
"��ͷ",
"����",
"Ʃ����ǧ",
"�ƥ�ѥ���",
"�پò�",
"�޲���",
"���ϼͷ�",
"��®�ͷ�",
"̵�� ��",
"̵�� ��",
"̵�� ��",
"̵�� ��",
"��ư",
"�и��ۼ�",
"�ƥ�ݡ���",
"ȿ��",
"��ʡ",
"����",
"�Ť�����",
"�ʱ�μ���"
#else
	"NoTeleport",
	"AntiMagic",
	"DecMana",
	"EvilCurse",
 	NULL,
 	"Hide Type",
	"Show Mods",
	"Insta Art",
	"Levitate",
	"Lite",
	"See Invis",
	"Telepathy",
	"Digestion",
	"Regen",
	"Xtra Might",
	"Xtra Shots",
	"Ign Acid",
	"Ign Elec",
	"Ign Fire",
	"Ign Cold",
	"Activate",
	"Drain Exp",
	"Teleport",
	"Aggravate",
	"Blessed",
	"Cursed",
	"Hvy Curse",
	"Prm Curse"
#endif

};


/*
 * Summarize resistances
 */
static void display_player_ben(void)
{
	int i, x, y;

	object_type *o_ptr;

	u32b f1, f2, f3;

	u16b b[6];
	u16b color[6];


	/* Reset */
	for (i = 0; i < 6; i++) b[i] = 0;


	/* Scan equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Object */
		o_ptr = &inventory[i];

		/* Known object flags */
		object_flags_known(o_ptr, &f1, &f2, &f3);


		if ((prace_is_(RACE_S_FAIRY)) && (f3 & TR3_AGGRAVATE))
		{
			f3 &= ~(TR3_AGGRAVATE);
			f1 |= TR1_STEALTH;
		}

		/* Incorporate */
		b[0] |= (f1 & 0xFFFF);
		b[1] |= (f1 >> 16);
		b[2] |= (f2 & 0xFFFF);
		b[3] |= (f2 >> 16);
		b[4] |= (f3 & 0xFFFF);
		b[5] |= (f3 >> 16);
	}


	/* Player flags */
	player_flags(&f1, &f2, &f3);

	/* Incorporate */
	b[0] |= (f1 & 0xFFFF);
	b[1] |= (f1 >> 16);
	b[2] |= (f2 & 0xFFFF);
	b[3] |= (f2 >> 16);
	b[4] |= (f3 & 0xFFFF);
	b[5] |= (f3 >> 16);

	/* Player flags */
	tim_player_flags(&f1, &f2, &f3, FALSE);

	/* Incorporate */
	b[0] |= (f1 & 0xFFFF);
	b[1] |= (f1 >> 16);
	b[2] |= (f2 & 0xFFFF);
	b[3] |= (f2 >> 16);
	b[4] |= (f3 & 0xFFFF);
	b[5] |= (f3 >> 16);
	color[0] = (f1 & 0xFFFF);
	color[1] = (f1 >> 16);
	color[2] = (f2 & 0xFFFF);
	color[3] = (f2 >> 16);
	color[4] = (f3 & 0xFFFF);
	color[5] = (f3 >> 16);

	/* Scan cols */
	for (x = 0; x < 6; x++)
	{
		/* Scan rows */
		for (y = 0; y < 16; y++)
		{
			byte a = TERM_SLATE;
			char c = '.';

			cptr name = object_flag_names[16*x+y];

			/* No name */
			if (!name) continue;

			/* Dump name */
			Term_putstr(x * 13, y + 4, -1, TERM_WHITE, name);

			/* Dump colon */
			Term_putch(x * 13 + 10, y + 4, TERM_WHITE, ':');

			/* Check flag */
			if (b[x] & (1<<y))
			{
				if (color[x] & (1<<y))
				{
					a = TERM_YELLOW;
					c = '#';
				}
				else
				{
					a = TERM_WHITE;
					c = '+';
				}
			}

			/* Dump flag */
			Term_putch(x * 13 + 11, y + 4, a, c);
		}
	}
}


/*
 * Summarize resistances
 */
static void display_player_ben_one(int mode)
{
	int i, n, x, y;

	object_type *o_ptr;

	u32b f1, f2, f3;

	u16b b[13][6];
	u16b color[6];


	/* Scan equipment */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		/* Index */
		n = (i - INVEN_RARM);

		/* Object */
		o_ptr = &inventory[i];

		object_flags_known(o_ptr, &f1, &f2, &f3);

		if ((prace_is_(RACE_S_FAIRY)) && (f3 & TR3_AGGRAVATE))
		{
			f3 &= ~(TR3_AGGRAVATE);
			f1 |= TR1_STEALTH;
		}

		/* Incorporate */
		b[n][0] = (u16b)(f1 & 0xFFFF);
		b[n][1] = (u16b)(f1 >> 16);
		b[n][2] = (u16b)(f2 & 0xFFFF);
		b[n][3] = (u16b)(f2 >> 16);
		b[n][4] = (u16b)(f3 & 0xFFFF);
		b[n][5] = (u16b)(f3 >> 16);
	}


	/* Index */
	n = 12;

	/* Player flags */
	player_flags(&f1, &f2, &f3);

	/* Incorporate */
	b[n][0] = (u16b)(f1 & 0xFFFF);
	b[n][1] = (u16b)(f1 >> 16);
	b[n][2] = (u16b)(f2 & 0xFFFF);
	b[n][3] = (u16b)(f2 >> 16);
	b[n][4] = (u16b)(f3 & 0xFFFF);
	b[n][5] = (u16b)(f3 >> 16);

	/* Player flags */
	tim_player_flags(&f1, &f2, &f3, FALSE);

	/* Incorporate */
	b[n][0] |= (f1 & 0xFFFF);
	b[n][1] |= (f1 >> 16);
	b[n][2] |= (f2 & 0xFFFF);
	b[n][3] |= (f2 >> 16);
	b[n][4] |= (f3 & 0xFFFF);
	b[n][5] |= (f3 >> 16);
	color[0] = (f1 & 0xFFFF);
	color[1] = (f1 >> 16);
	color[2] = (f2 & 0xFFFF);
	color[3] = (f2 >> 16);
	color[4] = (f3 & 0xFFFF);
	color[5] = (f3 >> 16);


	/* Scan cols */
	for (x = 0; x < 3; x++)
	{
		/* Equippy */
		display_player_equippy(2, x * 26 + 11);

		/* Label */
		Term_putstr(x * 26 + 11, 3, -1, TERM_WHITE, "abcdefghijkl@");

		/* Scan rows */
		for (y = 0; y < 16; y++)
		{
			cptr name = object_flag_names[48*mode+16*x+y];

			/* No name */
			if (!name) continue;

			/* Dump name */
			Term_putstr(x * 26, y + 4, -1, TERM_WHITE, name);

			/* Dump colon */
			Term_putch(x * 26 + 10, y + 4, TERM_WHITE, ':');

			/* Check flags */
			for (n = 0; n < 13; n++)
			{
				byte a = TERM_SLATE;
				char c = '.';

				/* Check flag */
				if (b[n][3*mode+x] & (1<<y))
				{
					if ((n == 12) && (color[3*mode+x] & (1<<y)))
					{
						a = TERM_YELLOW;
						c = '#';
					}
					else
					{
						a = TERM_WHITE;
						c = '+';
					}
				}

				/* Dump flag */
				Term_putch(x * 26 + 11 + n, y + 4, a, c);
			}
		}
	}
}


/*
 * Display the character on the screen (various modes)
 *
 * The top two and bottom two lines are left blank.
 *
 * Mode 0 = standard display with skills
 * Mode 1 = standard display with history
 * Mode 2 = summary of various things
 * Mode 3 = current flags (combined)
 * Mode 4 = current flags (part 1)
 * Mode 5 = current flags (part 2)
 * Mode 6 = mutations
 */
void display_player(int mode)
{
	int i;

	char	buf[80];
	char	tmp[64];


	/* XXX XXX XXX */
	if ((p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3) && skip_mutations)
		mode = (mode % 7);
	else
		mode = (mode % 6);

	/* Erase screen */
	clear_from(0);

	/* Standard */
	if ((mode == 0) || (mode == 1))
	{
		/* Name, Sex, Race, Class */
#ifdef JP
put_str("̾��  :", 1,26);
put_str("����        :", 3, 1);
put_str("��²        :", 4, 1);
put_str("����        :", 5, 1);
#else
		put_str("Name  :", 1,26);
		put_str("Sex         :", 3, 1);
		put_str("Race        :", 4, 1);
		put_str("Class       :", 5, 1);
#endif

		if (p_ptr->realm1 || p_ptr->realm2)
#ifdef JP
put_str("��ˡ        :", 6, 1);
#else
			put_str("Magic       :", 6, 1);
#endif

		if (p_ptr->pclass == CLASS_CHAOS_WARRIOR)
#ifdef JP
put_str("������    :", 7, 1);
#else
			put_str("Patron      :", 7, 1);
#endif

		strcpy(tmp,ap_ptr->title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"��");
#else
		strcat(tmp," ");
#endif
		strcat(tmp,player_name);

		c_put_str(TERM_L_BLUE, tmp, 1, 34);
		c_put_str(TERM_L_BLUE, sp_ptr->title, 3, 15);
		c_put_str(TERM_L_BLUE, (p_ptr->mimic_form ? mimic_info[p_ptr->mimic_form].title : rp_ptr->title), 4, 15);
		c_put_str(TERM_L_BLUE, cp_ptr->title, 5, 15);
		if (p_ptr->realm1)
			c_put_str(TERM_L_BLUE, realm_names[p_ptr->realm1], 6, 15);
		if (p_ptr->pclass == CLASS_CHAOS_WARRIOR)
			c_put_str(TERM_L_BLUE, chaos_patrons[p_ptr->chaos_patron], 7, 15);

		else if (p_ptr->realm2)
			c_put_str(TERM_L_BLUE, realm_names[p_ptr->realm2], 7, 15);

		/* Age, Height, Weight, Social */
#ifdef JP
        /* ��Ĺ�ϥ�����᡼�ȥ�ˡ��νŤϥ��������ѹ����Ƥ���ޤ� */
        /* �������ܸ��Ǥ��ɲäȤ���ñ�̤�ɽ�����ޤ� */
        /* ����Υ��������ܸ��ɽ������ˤ϶����Τǡ�����4��ư�����ޤ��� */
	       prt_num2("ǯ��         ", "��",(int)p_ptr->age, 3, 32-2, TERM_L_BLUE);
	       prt_num2("��Ĺ         ","cm",(int)((p_ptr->ht*254)/100) , 4, 32-2, TERM_L_BLUE);
	       prt_num2("�ν�         ", "kg",(int)((p_ptr->wt*4536)/10000) , 5, 32-2, TERM_L_BLUE);
		prt_num("�Ҳ�Ū�ϰ�   ", (int)p_ptr->sc , 6, 32-2, TERM_L_BLUE);
#else
		prt_num("Age          ", (int)p_ptr->age, 3, 32, TERM_L_BLUE);
		prt_num("Height       ", (int)p_ptr->ht , 4, 32, TERM_L_BLUE);
		prt_num("Weight       ", (int)p_ptr->wt , 5, 32, TERM_L_BLUE);
		prt_num("Social Class ", (int)p_ptr->sc , 6, 32, TERM_L_BLUE);
#endif


		/* Display the stats */
		for (i = 0; i < 6; i++)
		{
			/* Special treatment of "injured" stats */
			if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
			{
				int value;

				/* Use lowercase stat name */
				put_str(stat_names_reduced[i], 2 + i, 59);

				/* Get the current stat */
				value = p_ptr->stat_use[i];

				/* Obtain the current stat (modified) */
				cnv_stat(value, buf);

				/* Display the current stat (modified) */
				c_put_str(TERM_YELLOW, buf, 2 + i, 66);

				/* Acquire the max stat */
				value = p_ptr->stat_top[i];

				/* Obtain the maximum stat (modified) */
				cnv_stat(value, buf);

				/* Display the maximum stat (modified) */
				c_put_str(TERM_L_GREEN, buf, 2 + i, 73);
			}

			/* Normal treatment of "normal" stats */
			else
			{
				/* Assume uppercase stat name */
				put_str(stat_names[i], 2 + i, 59);

				/* Obtain the current stat (modified) */
				cnv_stat(p_ptr->stat_use[i], buf);

				/* Display the current stat (modified) */
				c_put_str(TERM_L_GREEN, buf, 2 + i, 66);
			}

			if (p_ptr->stat_max[i] == p_ptr->stat_max_max[i])
			{
				c_put_str(TERM_WHITE, "!", 2+i, 64);
			}
		}

		/* Extra info */
		display_player_middle();

		/* Display "history" info */
		if (mode == 1)
		{
#ifdef JP
			put_str("(����饯����������Ω��)", 16, 25);
#else
			put_str("(Character Background)", 16, 25);
#endif


			for (i = 0; i < 4; i++)
			{
				put_str(history[i], i + 17, 10);
			}
		}

		/* Display "various" info */
		else
		{
#ifdef JP
			put_str("     (����¾��ǽ��)     ", 16, 25);
#else
			put_str("(Miscellaneous Abilities)", 16, 25);
#endif


			display_player_various();
		}
	}

	/* Special */
	else if (mode == 2)
	{
		/* See "http://www.cs.berkeley.edu/~davidb/angband.html" */

		/* Dump the info */
		display_player_misc_info();
		display_player_stat_info();
		display_player_flag_info();
	}

	/* Special */
	else if (mode == 3)
	{
		display_player_ben();
	}

	else if (mode == 6)
	{
		do_cmd_knowledge_mutations();
	}

	/* Special */
	else
	{
		display_player_ben_one(mode % 2);
	}
}

errr make_character_dump(FILE *fff)
{
	int		i, x, y;
	byte		a;
	char		c;
	cptr		paren = ")";
	store_type  *st_ptr;
	char		o_name[MAX_NLEN];
	char		buf[1024];
	cptr            disp_align;


#ifndef FAKE_VERSION
	/* Begin dump */
	fprintf(fff, "  [Angband %d.%d.%d Character Dump]\n\n",
	        VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
#else
#ifdef JP
	fprintf(fff, "  [�Ѷ����� %d.%d.%d ����饯������]\n\n",
	        FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#else
	fprintf(fff, "  [Hengband %d.%d.%d Character Dump]\n\n",
	        FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH);
#endif

#endif

	update_playtime();

	/* Display player */
	display_player(0);

	/* Dump part of the screen */
	for (y = 1; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
#ifdef JP
                        fprintf(fff, "%s\n", buf);
#else
		fprintf(fff, "%s\n", buf);
#endif

	}

	/* Display history */
	display_player(1);

	/* Dump part of the screen */
	for (y = 16; y < 21; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	fprintf(fff, "\n");
	/* Display history */
	display_player(2);

	/* Dump part of the screen */
	for (y = 2; y < 22; y++)
	{
		/* Dump each row */
		for (x = 0; x < 79; x++)
		{
			/* Get the attr/char */
			(void)(Term_what(x, y, &a, &c));

			/* Dump it */
			buf[x] = c;
		}

		/* End the string */
		buf[x] = '\0';

		/* Kill trailing spaces */
		while ((x > 0) && (buf[x-1] == ' ')) buf[--x] = '\0';

		/* End the row */
		fprintf(fff, "%s\n", buf);
	}

	for (i = 0; i < p_ptr->count / 80; i++)
		fprintf(fff, " ");
	fprintf(fff, "\n");
	for (i = 0; i < p_ptr->count % 80; i++)
		fprintf(fff, " ");
	{
		bool pet = FALSE;

		for (i = m_max - 1; i >= 1; i--)
		{
			monster_type *m_ptr = &m_list[i];
			char pet_name[80];

			if (!m_ptr->r_idx) continue;
			if (!is_pet(m_ptr)) continue;
			if (!m_ptr->nickname && (p_ptr->riding != i)) continue;
			if (!pet)
			{
#ifdef JP
				fprintf(fff, "\n  [��ʥڥå�]\n\n");
#else
				fprintf(fff, "\n  [leading pets]\n\n");
#endif
				pet = TRUE;
			}
			monster_desc(pet_name, m_ptr, 0x88);
			fprintf(fff, "%s", pet_name);
			if (p_ptr->riding == i)
#ifdef JP
				fprintf(fff, " ������");
#else
				fprintf(fff, " riding");
#endif
			fprintf(fff, "\n");
		}
		if (pet) fprintf(fff, "\n");
	}

	if (death && !total_winner)
	{
#ifdef JP
		fprintf(fff, "\n  [���ľ���Υ�å�����]\n\n");
#else
		fprintf(fff, "\n  [Last messages]\n\n");
#endif
		for (i = MIN(message_num(), 15); i >= 0; i--)
		{
			fprintf(fff,"> %s\n",message_str((s16b)i));
		}
		fprintf(fff, "\n");
	}

#ifdef JP
	fprintf(fff, "\n  [����¾�ξ���]        \n");
#else
	fprintf(fff, "\n  [Miscellaneous information]\n");
#endif

#ifdef JP
	fprintf(fff, "\n ���Ծ��:\n");
#else
        fprintf(fff, "\n Recall Depth:\n");
#endif
	for (y = 1; y < max_d_idx; y++)
	{
		bool seiha = FALSE;

		if (!d_info[y].maxdepth) continue;
		if (!max_dlv[y]) continue;
		if (d_info[y].final_guardian)
		{
			if (!r_info[d_info[y].final_guardian].max_num) seiha = TRUE;
		}
		else if (max_dlv[y] == d_info[y].maxdepth) seiha = TRUE;

#ifdef JP
		fprintf(fff, "   %c%-12s: %3d ��\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#else
		fprintf(fff, "   %c%-16s: level %3d\n", seiha ? '!' : ' ', d_name+d_info[y].name, max_dlv[y]);
#endif
	}

	if (preserve_mode)
#ifdef JP
		fprintf(fff, "\n ��¸�⡼��:         ON");
#else
		fprintf(fff, "\n Preserve Mode:      ON");
#endif

	else
#ifdef JP
		fprintf(fff, "\n ��¸�⡼��:         OFF");
#else
		fprintf(fff, "\n Preserve Mode:      OFF");
#endif


	if (ironman_autoscum)
#ifdef JP
	        fprintf(fff, "\n ��ư���깥��  :     ALWAYS");
#else
		fprintf(fff, "\n Autoscum:           ALWAYS");
#endif

	else if (auto_scum)
#ifdef JP
	        fprintf(fff, "\n ��ư���깥��  :     ON");
#else
		fprintf(fff, "\n Autoscum:           ON");
#endif

	else
#ifdef JP
	        fprintf(fff, "\n ��ư���깥��  :     OFF");
#else
		fprintf(fff, "\n Autoscum:           OFF");
#endif


	if (ironman_small_levels)
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   ALWAYS");
#else
		fprintf(fff, "\n Small Levels:       ALWAYS");
#endif

	else if (always_small_levels)
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   ON");
#else
		fprintf(fff, "\n Small Levels:       ON");
#endif

	else if (small_levels)
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   ENABLED");
#else
		fprintf(fff, "\n Small Levels:       ENABLED");
#endif

	else
#ifdef JP
		fprintf(fff, "\n ���������󥸥��:   OFF");
#else
		fprintf(fff, "\n Small Levels:       OFF");
#endif


	if (vanilla_town)
#ifdef JP
		fprintf(fff, "\n ���Ĥ�Į�Τ�: ON");
#else
		fprintf(fff, "\n Vanilla Town:       ON");
#endif

	else if (lite_town)
#ifdef JP
		fprintf(fff, "\n �����Ϥ�Į:         ON");
#else
		fprintf(fff, "\n Lite Town:          ON");
#endif


	if (ironman_shops)
#ifdef JP
		fprintf(fff, "\n Ź�ʤ�:             ON");
#else
		fprintf(fff, "\n No Shops:           ON");
#endif


	if (ironman_downward)
#ifdef JP
		fprintf(fff, "\n ���ʤ�夬��ʤ�:   ON");
#else
		fprintf(fff, "\n Diving only:        ON");
#endif


	if (ironman_rooms)
#ifdef JP
		fprintf(fff, "\n ���̤Ǥʤ�����������:         ON");
#else
		fprintf(fff, "\n Unusual rooms:      ON");
#endif


	if (ironman_nightmare)
#ifdef JP
		fprintf(fff, "\n ��̴�⡼��:         ON");
#else
		fprintf(fff, "\n Nightmare Mode:     ON");
#endif


	if (ironman_empty_levels)
#ifdef JP
		fprintf(fff, "\n ���꡼��:           ALWAYS");
#else
		fprintf(fff, "\n Arena Levels:       ALWAYS");
#endif

	else if (empty_levels)
#ifdef JP
		fprintf(fff, "\n ���꡼��:           ON");
#else
		fprintf(fff, "\n Arena Levels:       ENABLED");
#endif

	else
#ifdef JP
	        fprintf(fff, "\n ���꡼��:           OFF");
#else
		fprintf(fff, "\n Arena Levels:       OFF");
#endif


#ifdef JP
	fprintf(fff, "\n �����९�����ȿ�: %d", number_of_quests());
#else
	fprintf(fff, "\n Num. Random Quests: %d", number_of_quests());
#endif

	if (p_ptr->arena_number == 99)
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: ����\n");
#else
		fprintf(fff, "\n Arena: defeated\n");
#endif
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS+2)
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: ���Υ����ԥ���\n");
#else
		fprintf(fff, "\n Arena: True Champion\n");
#endif
	}
	else if (p_ptr->arena_number > MAX_ARENA_MONS-1)
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����: �����ԥ���\n");
#else
		fprintf(fff, "\n Arena: Champion\n");
#endif
	}
	else
	{
#ifdef JP
		fprintf(fff, "\n Ʈ����:   %2d��\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number));
#else
		fprintf(fff, "\n Arena:   %2d victor%s\n", (p_ptr->arena_number > MAX_ARENA_MONS ? MAX_ARENA_MONS : p_ptr->arena_number), (p_ptr->arena_number>1) ? "ies" : "y");
#endif
	}

	if (noscore)
#ifdef JP
fprintf(fff, "\n ���������ʤ��Ȥ򤷤Ƥ��ޤäƤޤ���");
#else
		fprintf(fff, "\n You have done something illegal.");
#endif


	if (stupid_monsters)
#ifdef JP
fprintf(fff, "\n Ũ�϶򤫤ʹ�ư����ޤ���");
#else
		fprintf(fff, "\n Your opponents are behaving stupidly.");
#endif


	if (munchkin_death)
#ifdef JP
fprintf(fff, "\n ���ʤ��ϻ����򤹤륤��������Ϥ���äƤ��ޤ���");
#else
		fprintf(fff, "\n You possess munchkinish power over death.");
#endif

	fprintf(fff,"\n");

	/* Monsters slain */
	{
		int k;
		s32b Total = 0;

		for (k = 1; k < max_r_idx; k++)
		{
			monster_race *r_ptr = &r_info[k];

			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				bool dead = (r_ptr->max_num == 0);
				if (dead)
				{
					Total++;
				}
			}
			else
			{
				s16b This = r_ptr->r_pkills;
				if (This > 0)
				{
					Total += This;
				}
			}
		}

		if (Total < 1)
#ifdef JP
fprintf(fff,"\n �ޤ�Ũ���ݤ��Ƥ��ޤ���\n");
#else
			fprintf(fff,"\n You have defeated no enemies yet.\n");
#endif

		else if (Total == 1)
#ifdef JP
fprintf(fff,"\n ���Τ�Ũ���ݤ��Ƥ��ޤ���\n");
#else
			fprintf(fff,"\n You have defeated one enemy.\n");
#endif

		else
#ifdef JP
fprintf(fff,"\n %lu �Τ�Ũ���ݤ��Ƥ��ޤ���\n", Total);
#else
			fprintf(fff,"\n You have defeated %lu enemies.\n", Total);
#endif

	}


	if (p_ptr->old_race1 || p_ptr->old_race2)
	{
#ifdef JP
		fprintf(fff, "\n\n ���ʤ���%s�Ȥ������ޤ줿��", race_info[p_ptr->start_race].title);
#else
		fprintf(fff, "\n\n You were born as %s.", race_info[p_ptr->start_race].title);
#endif
		for (i = 0; i < MAX_RACES; i++)
		{
			if (p_ptr->start_race == i) continue;
			if (i < 32)
			{
				if (!(p_ptr->old_race1 & 1L << i)) continue;
			}
			else
			{
				if (!(p_ptr->old_race2 & 1L << (i-32))) continue;
			}
#ifdef JP
			fprintf(fff, "\n ���ʤ��Ϥ��Ĥ�%s���ä���", race_info[i].title);
#else
			fprintf(fff, "\n You were a %s before.", race_info[i].title);
#endif
		}
	}

	if (p_ptr->old_realm)
	{
		for (i = 0; i < MAX_MAGIC; i++)
		{
			if (!(p_ptr->old_realm & 1L << i)) continue;
#ifdef JP
			fprintf(fff, "\n ���ʤ��Ϥ��Ĥ�%s��ˡ��Ȥ�����", realm_names[i+1]);
#else
			fprintf(fff, "\n You were able to use %s magic before.", realm_names[i+1]);
#endif
		}
	}

#ifdef JP
fprintf(fff, "\n\n  [�ץ쥤�䡼����]\n\n");
#else
	fprintf(fff, "\n\n  [Virtues]\n\n");
#endif

#ifdef JP
	if (p_ptr->align > 150) disp_align = "����";
	else if (p_ptr->align > 50) disp_align = "����";
	else if (p_ptr->align > 10) disp_align = "����";
	else if (p_ptr->align > -11) disp_align = "��Ω";
	else if (p_ptr->align > -51) disp_align = "����";
	else if (p_ptr->align > -151) disp_align = "�氭";
	else disp_align = "�簭";
	fprintf(fff, "°�� : %s\n", disp_align);
#else
	if (p_ptr->align > 150) disp_align = "lawful";
	else if (p_ptr->align > 50) disp_align = "good";
	else if (p_ptr->align > 10) disp_align = "neutral good";
	else if (p_ptr->align > -11) disp_align = "neutral";
	else if (p_ptr->align > -51) disp_align = "neutral evil";
	else if (p_ptr->align > -151) disp_align = "evil";
	else disp_align = "chaotic";
	fprintf(fff, "Your alighnment : %s\n", disp_align);
#endif
	fprintf(fff, "\n");
	dump_virtues(fff);

	if (p_ptr->muta1 || p_ptr->muta2 || p_ptr->muta3)
	{
#ifdef JP
fprintf(fff, "\n\n  [�����Ѱ�]\n\n");
#else
		fprintf(fff, "\n\n  [Mutations]\n\n");
#endif

		dump_mutations(fff);
	}


	/* Skip some lines */
	fprintf(fff, "\n\n");


	/* Dump the equipment */
	if (equip_cnt)
	{
#ifdef JP
fprintf(fff, "  [ ����饯�������� ]\n\n");
#else
		fprintf(fff, "  [Character Equipment]\n\n");
#endif

		for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
		{
			object_desc(o_name, &inventory[i], TRUE, 3);
			if ((i == INVEN_LARM) && p_ptr->ryoute)
#ifdef JP
				strcpy(o_name, "(����ξ�����)");
#else
				strcpy(o_name, "(wielding with two-hands)");
#endif
			fprintf(fff, "%c%s %s\n",
				index_to_label(i), paren, o_name);
		}
		fprintf(fff, "\n\n");
	}

	/* Dump the inventory */
#ifdef JP
fprintf(fff, "  [ ����饯���λ���ʪ ]\n\n");
#else
	fprintf(fff, "  [Character Inventory]\n\n");
#endif

	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Don't dump the empty slots */
		if (!inventory[i].k_idx) break;

		/* Dump the inventory slots */
		object_desc(o_name, &inventory[i], TRUE, 3);
		fprintf(fff, "%c%s %s\n", index_to_label(i), paren, o_name);
	}

	/* Add an empty line */
	fprintf(fff, "\n\n");

	process_dungeon_file("w_info_j.txt", 0, 0, max_wild_y, max_wild_x);

	/* Print all homes in the different towns */
	st_ptr = &town[1].store[STORE_HOME];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		/* Header with name of the town */
#ifdef JP
		fprintf(fff, "  [ �椬�ȤΥ����ƥ� ]\n");
#else
		fprintf(fff, "  [Home Inventory]\n");
#endif
		x=1;

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			if ((i % 12) == 0)
#ifdef JP
				fprintf(fff, "\n ( %d �ڡ��� )\n", x++);
#else
			        fprintf(fff, "\n ( page %d )\n", x++);
#endif
			object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
			fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}


	/* Print all homes in the different towns */
	st_ptr = &town[1].store[STORE_MUSEUM];

	/* Home -- if anything there */
	if (st_ptr->stock_num)
	{
		/* Header with name of the town */
#ifdef JP
		fprintf(fff, "  [ ��ʪ�ۤΥ����ƥ� ]\n");
#else
		fprintf(fff, "  [Museum]\n");
#endif
		x=1;

		/* Dump all available items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
#ifdef JP
                if ((i % 12) == 0) fprintf(fff, "\n ( %d �ڡ��� )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
			fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
#else
                if ((i % 12) == 0) fprintf(fff, "\n ( page %d )\n", x++);
			object_desc(o_name, &st_ptr->stock[i], TRUE, 3);
			fprintf(fff, "%c%s %s\n", I2A(i%12), paren, o_name);
#endif

		}

		/* Add an empty line */
		fprintf(fff, "\n\n");
	}

	return 0;
}

/*
 * Hack -- Dump a character description file
 *
 * XXX XXX XXX Allow the "full" flag to dump additional info,
 * and trigger its usage from various places in the code.
 */
errr file_character(cptr name, bool full)
{
	int		fd = -1;
	FILE		*fff = NULL;
	char		buf[1024];

	/* Drop priv's */
	safe_setuid_drop();

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, name);

	/* File type is "TEXT" */
	FILE_TYPE(FILE_TYPE_TEXT);

	/* Check for existing file */
	fd = fd_open(buf, O_RDONLY);

	/* Existing file */
	if (fd >= 0)
	{
		char out_val[160];

		/* Close the file */
		(void)fd_close(fd);

		/* Build query */
#ifdef JP
(void)sprintf(out_val, "��¸����ե����� %s �˾�񤭤��ޤ���? ", buf);
#else
		(void)sprintf(out_val, "Replace existing file %s? ", buf);
#endif


		/* Ask */
		if (get_check(out_val)) fd = -1;
	}

	/* Open the non-existing file */
	if (fd < 0) fff = my_fopen(buf, "w");

	/* Grab priv's */
	safe_setuid_grab();


	/* Invalid file */
	if (!fff)
	{
		/* Message */
#ifdef JP
msg_format("����饯������Υե�����ؤν񤭽Ф��˼��Ԥ��ޤ�����");
#else
		msg_format("Character dump failed!");
#endif

		msg_print(NULL);

		/* Error */
		return (-1);
	}

	(void)make_character_dump(fff);

	/* Close it */
	my_fclose(fff);


	/* Message */
#ifdef JP
msg_print("����饯������Υե�����ؤν񤭽Ф����������ޤ�����");
#else
	msg_print("Character dump successful.");
#endif

	msg_print(NULL);

	/* Success */
	return (0);
}


/*
 * Recursive file perusal.
 *
 * Return FALSE on "ESCAPE", otherwise TRUE.
 *
 * Process various special text in the input file, including
 * the "menu" structures used by the "help file" system.
 *
 * XXX XXX XXX Consider using a temporary file.
 *
 * XXX XXX XXX Allow the user to "save" the current file.
 */
bool show_file(bool show_version, cptr name, cptr what, int line, int mode)
{
	int i, n, k;

	/* Number of "real" lines passed by */
	int next = 0;

	/* Number of "real" lines in the file */
	int size = 0;

	/* Backup value for "line" */
	int back = 0;

	/* Loop counter */
	int cnt;

	/* This screen has sub-screens */
	bool menu = FALSE;

	/* Current help file */
	FILE *fff = NULL;

	/* Find this string (if any) */
	cptr find = NULL;

	/* Jump to this tag */
	cptr tag = NULL;

	/* Hold a string to find */
	char finder[81];

	/* Hold a string to show */
	char shower[81];

	/* Filename */
	char filename[1024];

	/* Describe this thing */
	char caption[128];

	/* Path buffer */
	char path[1024];

	/* General buffer */
	char buf[1024];

	/* Lower case version of the buffer, for searching */
	char lc_buf[1024];

	/* Aux pointer for making lc_buf (and find!) lowercase */
	cptr lc_buf_ptr;

	/* Sub-menu information */
	char hook[68][32];

	/* Tags for in-file references */
	int tags[68];

	bool reverse = (line < 0);

	/* Wipe finder */
	strcpy(finder, "");

	/* Wipe shower */
	strcpy(shower, "");

	/* Wipe caption */
	strcpy(caption, "");

	/* Wipe the hooks */
	for (i = 0; i < 68; i++)
	{
		hook[i][0] = '\0';
	}

	/* Copy the filename */
	strcpy(filename, name);

	n = strlen(filename);

	/* Extract the tag from the filename */
	for (i = 0; i < n; i++)
	{
		if (filename[i] == '#')
		{
			filename[i] = '\0';
			tag = filename + i + 1;
			break;
		}
	}

	/* Redirect the name */
	name = filename;

	/* Hack XXX XXX XXX */
	if (what)
	{
		/* Caption */
		strcpy(caption, what);

		/* Access the "file" */
		strcpy(path, name);

		/* Open */
		fff = my_fopen(path, "r");
	}

	/* Look in "help" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
sprintf(caption, "�إ�ס��ե�����'%s'", name);
#else
		sprintf(caption, "Help file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, 1024, ANGBAND_DIR_HELP, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Look in "info" */
	if (!fff)
	{
		/* Caption */
#ifdef JP
sprintf(caption, "���ݥ��顼���ե�����'%s'", name);
#else
		sprintf(caption, "Info file '%s'", name);
#endif


		/* Build the filename */
		path_build(path, 1024, ANGBAND_DIR_INFO, name);

		/* Open the file */
		fff = my_fopen(path, "r");
	}

	/* Oops */
	if (!fff)
	{
		/* Message */
#ifdef JP
msg_format("'%s'�򥪡��ץ�Ǥ��ޤ���", name);
#else
		msg_format("Cannot open '%s'.", name);
#endif

		msg_print(NULL);

		/* Oops */
		return (TRUE);
	}


	/* Pre-Parse the file */
	while (TRUE)
	{
		/* Read a line or stop */
		if (my_fgets(fff, buf, 1024)) break;

		/* XXX Parse "menu" items */
		if (prefix(buf, "***** "))
		{
			/* Notice "menu" requests */
			if ((buf[6] == '[') && (isdigit(buf[7]) || isalpha(buf[7])))
			{
				/* This is a menu file */
				menu = TRUE;

				/* Extract the menu item */
				k = isdigit(buf[7]) ? D2I(buf[7]) : buf[7] - 'A' + 10;

				if ((buf[8] == ']') && (buf[9] == ' '))
				{
					/* Extract the menu item */
					strcpy(hook[k], buf + 10);
				}
			}
			/* Notice "tag" requests */
			else if ((buf[6] == '<') && (isdigit(buf[7]) || isalpha(buf[7])) &&
			    (buf[8] == '>'))
			{
				/* Extract the menu item */
				k = isdigit(buf[7]) ? D2I(buf[7]) : buf[7] - 'A' + 10;

				/* Extract the menu item */
				tags[k] = next;
			}

			/* Skip this */
			continue;
		}

		/* Count the "real" lines */
		next++;
	}

	/* Save the number of "real" lines */
	size = next;

	if (line == -1) line = ((size-1)/20)*20;

	/* Go to the tagged line */
	if (tag)
		line = tags[isdigit(tag[0]) ? D2I(tag[0]) : tag[0] - 'A' + 10];

	/* Display the file */
	while (TRUE)
	{
		/* Clear screen */
		Term_clear();

		/* Restart when necessary */
		if (line >= size) line = 0;


		/* Re-open the file if needed */
		if (next > line)
		{
			/* Close it */
			my_fclose(fff);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			/* Oops */
			if (!fff) return (FALSE);

			/* File has been restarted */
			next = 0;
		}

		/* Goto the selected line */
		while (next < line)
		{
			/* Get a line */
			if (my_fgets(fff, buf, 1024)) break;

			/* Skip tags/links */
			if (prefix(buf, "***** ")) continue;

			/* Count the lines */
			next++;
		}

		/* Dump the next 20 lines of the file */
		for (i = 0; i < 20; )
		{
			/* Hack -- track the "first" line */
			if (!i) line = next;

			/* Get a line of the file or stop */
			if (my_fgets(fff, buf, 1024)) break;

			/* Hack -- skip "special" lines */
			if (prefix(buf, "***** ")) continue;

			/* Count the "real" lines */
			next++;

			/* Make a lower case version of buf for searching */
			strcpy(lc_buf, buf);

			for (lc_buf_ptr = lc_buf; *lc_buf_ptr != 0; lc_buf_ptr++)
			{
				lc_buf[lc_buf_ptr-lc_buf] = tolower(*lc_buf_ptr);
			}

			/* Hack -- keep searching */
			if (find && !i && !strstr(lc_buf, find)) continue;

			/* Hack -- stop searching */
			find = NULL;

			/* Dump the line */
			Term_putstr(0, i+2, -1, TERM_WHITE, buf);

			/* Hilite "shower" */
			if (shower[0])
			{
				cptr str = lc_buf;

				/* Display matches */
				while ((str = strstr(str, shower)) != NULL)
				{
					int len = strlen(shower);

					/* Display the match */
					Term_putstr(str-lc_buf, i+2, len, TERM_YELLOW, &buf[str-lc_buf]);

					/* Advance */
					str += len;
				}
			}

			/* Count the printed lines */
			i++;
		}

		/* Hack -- failed search */
		if (find)
		{
			bell();
			line = back;
			find = NULL;
			continue;
		}


		/* Show a general "title" */
		if (show_version)
		{
#ifdef JP
prt(format("[�Ѷ����� %d.%d.%d, %s, %d/%d]",
#else
		prt(format("[Hengband %d.%d.%d, %s, Line %d/%d]",
#endif

		           FAKE_VER_MAJOR-10, FAKE_VER_MINOR, FAKE_VER_PATCH,
		           caption, line, size), 0, 0);
		}
		else
		{
#ifdef JP
prt(format("[%s, %d/%d]",
#else
			prt(format("[%s, Line %d/%d]",
#endif
			           caption, line, size), 0, 0);
		}

		/* Prompt -- menu screen */
		if (menu)
		{
			/* Wait for it */
#ifdef JP
prt("[ �ֹ�����Ϥ��Ʋ�����( ESC�ǽ�λ ) ]", 23, 0);
#else
			prt("[Press a Number, or ESC to exit.]", 23, 0);
#endif

		}

		/* Prompt -- small files */
		else if (size <= 20)
		{
			/* Wait for it */
#ifdef JP
prt("[����:(?)�إ�� (ESC)��λ]", 23, 0);
#else
			prt("[Press ESC to exit.]", 23, 0);
#endif

		}

		/* Prompt -- large files */
		else
		{
#ifdef JP
			if(reverse)
				prt("[����:(RET/���ڡ���)�� (-)�� (?)�إ�� (ESC)��λ]", 23, 0);
			else
				prt("[����:(RET/���ڡ���)�� (-)�� (?)�إ�� (ESC)��λ]", 23, 0);
#else
			prt("[Press Return, Space, -, =, /, |, or ESC to exit.]", 23, 0);
#endif
		}

		/* Get a keypress */
		k = inkey();

		/* Hack -- return to last screen */
		if (k == '<') break;

		/* Show the help for the help */
		if (k == '?')
		{
			/* Hack - prevent silly recursion */
#ifdef JP
			if (strcmp(name, "jhelpinfo.txt") != 0)
				show_file(TRUE, "jhelpinfo.txt", NULL, 0, mode);
#else
			if (strcmp(name, "helpinfo.txt") != 0)
				show_file(TRUE, "helpinfo.txt", NULL, 0, mode);
#endif
		}

		/* Hack -- try showing */
		if (k == '=')
		{
			/* Get "shower" */
#ifdef JP
prt("��Ĵ: ", 23, 0);
#else
			prt("Show: ", 23, 0);
#endif

			(void)askfor_aux(shower, 80);
		}

		/* Hack -- try finding */
		if (k == '/')
		{
			/* Get "finder" */
#ifdef JP
prt("����: ", 23, 0);
#else
			prt("Find: ", 23, 0);
#endif


			if (askfor_aux(finder, 80))
			{
				/* Find it */
				find = finder;
				back = line;
				line = line + 1;

				/* Make finder lowercase */
				for (cnt = 0; finder[cnt] != 0; cnt++)
				{
					finder[cnt] = tolower(finder[cnt]);
				}

				/* Show it */
				strcpy(shower, finder);
			}
		}

		/* Hack -- go to a specific line */
		if (k == '#')
		{
			char tmp[81];
#ifdef JP
prt("��: ", 23, 0);
#else
			prt("Goto Line: ", 23, 0);
#endif

			strcpy(tmp, "0");

			if (askfor_aux(tmp, 80))
			{
				line = atoi(tmp);
			}
		}

		/* Hack -- go to a specific file */
		if (k == '%')
		{
			char tmp[81];
#ifdef JP
prt("�ե����롦�͡���: ", 23, 0);
strcpy(tmp, "jhelp.hlp");
#else
			prt("Goto File: ", 23, 0);
			strcpy(tmp, "help.hlp");
#endif


			if (askfor_aux(tmp, 80))
			{
				if (!show_file(TRUE, tmp, NULL, 0, mode)) k = ESCAPE;
			}
		}

		/* Hack -- Allow backing up */
		if (k == '-')
		{
			line = line + (reverse ? 20 : -20);
			if (line < 0) line = ((size-1)/20)*20;
		}

		/* Hack -- Advance a single line */
		if ((k == '\n') || (k == '\r'))
		{
			line = line + (reverse ? -1 : 1);
			if (line < 0) line = ((size-1)/20)*20;
		}

		/* Advance one page */
		if (k == ' ')
		{
			line = line + (reverse ? -20 : 20);
			if (line < 0) line = ((size-1)/20)*20;
		}

#ifdef JP_FALSE
		/* ���ܸ��Ǥ��ɲä��줿�إ�פ�ɽ�� */
		/* ���ޤ�褤�����λ����Ȥϻפ��ʤ����������ޤ� */

		/* �ʰץ��ޥ�ɰ��� */
		if (menu && (k == 'c' || k == 'C'))
		{
			char tmp[80];
			switch (rogue_like_commands)
			{
				case TRUE:
				{
					strcpy(tmp, "j_com_r.txt");
					if(!show_file(TRUE, tmp, NULL, 0, mode)) k = ESCAPE;
					break;
				}
				case FALSE:
				{
					strcpy(tmp, "j_com_o.txt");
					if(!show_file(TRUE, tmp, NULL, 0, mode)) k = ESCAPE;
					break;
				}
			}
		}

#endif
		/* Recurse on numbers */
		if (menu)
		{
			int key = -1;

			if (isdigit(k)) key = D2I(k);
			else if (isalpha(k)) key = k - 'A' + 10;

			if ((key > -1) && hook[key][0])
			{
				/* Recurse on that file */
				if (!show_file(TRUE, hook[key], NULL, 0, mode))
					k = ESCAPE;
			}
		}

		/* Hack, dump to file */
		if (k == '|')
		{
			FILE *ffp;
			char buff[1024];
			char xtmp[82];

			strcpy (xtmp, "");

#ifdef JP
if (get_string("�ե�����̾: ", xtmp, 80))
#else
			if (get_string("File name: ", xtmp, 80))
#endif

			{
				if (xtmp[0] && (xtmp[0] != ' '))
				{
				}
			}
			else
			{
				continue;
			}

			/* Build the filename */
			path_build(buff, 1024, ANGBAND_DIR_USER, xtmp);

			/* Close it */
			my_fclose(fff);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");

			ffp = my_fopen(buff, "w");

			/* Oops */
			if (!(fff && ffp))
			{
#ifdef JP
msg_print("�ե����뤬���Ĥ���ޤ���");
#else
				msg_print("Failed to open file.");
#endif

				k = ESCAPE;
				break;
			}

			sprintf(xtmp, "%s: %s", player_name, what);
			my_fputs(ffp, xtmp, 80);
			my_fputs(ffp, "\n", 80);

			while (!my_fgets(fff, buff, 80))
				my_fputs(ffp, buff, 80);

			/* Close it */
			my_fclose(fff);
			my_fclose(ffp);

			/* Hack -- Re-Open the file */
			fff = my_fopen(path, "r");
		}

		/* Exit on escape */
		if (k == ESCAPE) break;
	}

	/* Close the file */
	my_fclose(fff);

	/* Escape */
	if (k == ESCAPE) return (FALSE);

	/* Normal return */
	return (TRUE);
}


/*
 * Peruse the On-Line-Help
 */
void do_cmd_help(void)
{
	/* Save screen */
	screen_save();

	/* Peruse the main help file */
#ifdef JP
(void)show_file(TRUE, "jhelp.hlp", NULL, 0, 0);
#else
	(void)show_file(TRUE, "help.hlp", NULL, 0, 0);
#endif


	/* Load screen */
	screen_load();
}


/*
 * Process the player name.
 * Extract a clean "base name".
 * Build the savefile name if needed.
 */
void process_player_name(bool sf)
{
	int i, k = 0;


	/* Cannot be too long */
#if defined(MACINTOSH) || defined(MSDOS) || defined(USE_EMX) || defined(AMIGA) || defined(ACORN) || defined(VM)
#ifdef MSDOS
	if (strlen(player_name) > 8)
#else
	if (strlen(player_name) > 15)
#endif
	{
		/* Name too long */
#ifdef JP
quit_fmt("'%s'�Ȥ���̾����Ĺ�����ޤ���", player_name);
#else
		quit_fmt("The name '%s' is too long!", player_name);
#endif

	}
#endif

	/* Cannot contain "icky" characters */
	for (i = 0; player_name[i]; i++)
	{
		/* No control characters */
#ifdef JP
		if (iskanji(player_name[i])){i++;continue;}
		if (iscntrl( (unsigned char)player_name[i]))
#else
		if (iscntrl(player_name[i]))
#endif

		{
			/* Illegal characters */
#ifdef JP
quit_fmt("'%s' �Ȥ���̾���������ʥ���ȥ��륳���ɤ�ޤ�Ǥ��ޤ���", player_name);
#else
			quit_fmt("The name '%s' contains control chars!", player_name);
#endif

		}
	}


#ifdef MACINTOSH

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif


		/* Convert "dot" to "underscore" */
		if (c == '.') c = '_';

		/* Accept all the letters */
		player_base[k++] = c;
	}

#else

	/* Extract "useful" letters */
	for (i = 0; player_name[i]; i++)
	{
#ifdef JP
		unsigned char c = player_name[i];
#else
		char c = player_name[i];
#endif

		/* Accept some letters */
#ifdef JP
		if(iskanji(c)){
		  if(k + 2 >= sizeof(player_base) || !player_name[i+1]) break;
		  player_base[k++] = c;
		  i++;
		  player_base[k++] = player_name[i];
		}
#ifdef SJIS
		else if (iskana(c)) player_base[k++] = c;
#endif
		else
#endif
		/* Convert path separator to underscore */
		if (!strncmp(PATH_SEP, player_name+i, strlen(PATH_SEP))){
			player_base[k++] = '_';
			i += strlen(PATH_SEP);
		}
#ifdef MSDOS
		/* Convert space, dot, and underscore to underscore */
		else if (strchr(". _", c)) player_base[k++] = '_';
#endif
		else if (isprint(c)) player_base[k++] = c;
	}

#endif


#if defined(WINDOWS) || defined(MSDOS)

	/* Hack -- max length */
	if (k > 8) k = 8;

#endif

	/* Terminate */
	player_base[k] = '\0';

	/* Require a "base" name */
	if (!player_base[0]) strcpy(player_base, "PLAYER");


#ifdef SAVEFILE_MUTABLE

	/* Accept */
	sf = TRUE;

#endif
	if (!savefile_base[0] && savefile[0])
	{
		cptr s;
		s = savefile;
		while (1)
		{
			cptr t;
			t = strstr(s, PATH_SEP);
			if (!t)
				break;
			s = t+1;
		}
		strcpy(savefile_base, s);
	}

	if (!savefile_base[0] || !savefile[0])
		sf = TRUE;

	/* Change the savefile name */
	if (sf)
	{
		char temp[128];

		strcpy(savefile_base, player_base);

#ifdef SAVEFILE_USE_UID
		/* Rename the savefile, using the player_uid and player_base */
		(void)sprintf(temp, "%d.%s", player_uid, player_base);
#else
		/* Rename the savefile, using the player_base */
		(void)sprintf(temp, "%s", player_base);
#endif

#ifdef VM
		/* Hack -- support "flat directory" usage on VM/ESA */
		(void)sprintf(temp, "%s.sv", player_base);
#endif /* VM */

		/* Build the filename */
		path_build(savefile, 1024, ANGBAND_DIR_SAVE, temp);
	}
}


/*
 * Gets a name for the character, reacting to name changes.
 *
 * Assumes that "display_player(0)" has just been called
 *
 * Perhaps we should NOT ask for a name (at "birth()") on
 * Unix machines?  XXX XXX
 *
 * What a horrible name for a global function.  XXX XXX XXX
 */
void get_name(void)
{
	char tmp[64];

	/* Save the player name */
	strcpy(tmp, player_name);

	/* Prompt for a new name */
#ifdef JP
	if (get_string("����饯������̾�������Ϥ��Ʋ�����: ", tmp, 15))
#else
	if (get_string("Enter a name for your character: ", tmp, 15))
#endif
	{
		/* Use the name */
		strcpy(player_name, tmp);

		/* Process the player name */
		process_player_name(FALSE);
	}
	else if (strlen(player_name))
	{
		/* Process the player name */
		process_player_name(FALSE);
	}

	strcpy(tmp,ap_ptr->title);
#ifdef JP
	if(ap_ptr->no == 1)
		strcat(tmp,"��");
#else
	strcat(tmp, " ");
#endif
	strcat(tmp,player_name);

	/* Re-Draw the name (in light blue) */
	c_put_str(TERM_L_BLUE, tmp, 1, 34);

	/* Erase the prompt, etc */
	clear_from(22);
}



/*
 * Hack -- commit suicide
 */
void do_cmd_suicide(void)
{
	int i;

	/* Flush input */
	flush();

	/* Verify Retirement */
	if (total_winner)
	{
		/* Verify */
#ifdef JP
if (!get_check("���ष�ޤ���? ")) return;
#else
		if (!get_check("Do you want to retire? ")) return;
#endif

	}

	/* Verify Suicide */
	else
	{
		/* Verify */
#ifdef JP
if (!get_check("�����˼������ޤ�����")) return;
#else
		if (!get_check("Do you really want to commit suicide? ")) return;
#endif
	}


	if (!noscore)
	{
		/* Special Verification for suicide */
#ifdef JP
prt("��ǧ�Τ��� '@' �򲡤��Ʋ�������", 0, 0);
#else
		prt("Please verify SUICIDE by typing the '@' sign: ", 0, 0);
#endif

		flush();
		i = inkey();
		prt("", 0, 0);
		if (i != '@') return;
	}

	/* Stop playing */
	alive = FALSE;

	/* Kill the player */
	death = TRUE;

	/* Leaving */
	p_ptr->leaving = TRUE;

	if (!total_winner)
	{
#ifdef JP
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "���󥸥���õ������˾���Ƽ���������");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "-------- �����४���С� --------");
#else
		do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "give up all hope to commit suicide.");
		do_cmd_write_nikki(NIKKI_GAMESTART, 1, "--------   Game  Over   --------");
#endif
		do_cmd_write_nikki(NIKKI_BUNSHOU, 1, "\n\n\n\n");
	}

	/* Cause of death */
#ifdef JP
(void)strcpy(died_from, "���潪λ");
#else
	(void)strcpy(died_from, "Quitting");
#endif

}



/*
 * Save the game
 */
void do_cmd_save_game(int is_autosave)
{
	/* Autosaves do not disturb */
	if (is_autosave)
	{
#ifdef JP
msg_print("��ư��������");
#else
		msg_print("Autosaving the game...");
#endif

	}
	else
	{
		/* Disturb the player */
		disturb(1, 0);
	}

	/* Clear messages */
	msg_print(NULL);

	/* Handle stuff */
	handle_stuff();

	/* Message */
#ifdef JP
prt("������򥻡��֤��Ƥ��ޤ�...", 0, 0);
#else
	prt("Saving game...", 0, 0);
#endif


	/* Refresh */
	Term_fresh();

	/* The player is not dead */
#ifdef JP
(void)strcpy(died_from, "(������)");
#else
	(void)strcpy(died_from, "(saved)");
#endif


	/* Forbid suspend */
	signals_ignore_tstp();

	/* Save the player */
	if (save_player())
	{
#ifdef JP
prt("������򥻡��֤��Ƥ��ޤ�... ��λ", 0, 0);
#else
		prt("Saving game... done.", 0, 0);
#endif

	}

	/* Save failed (oops) */
	else
	{
#ifdef JP
prt("������򥻡��֤��Ƥ��ޤ�... ���ԡ�", 0, 0);
#else
		prt("Saving game... failed!", 0, 0);
#endif

	}

	/* Allow suspend again */
	signals_handle_tstp();

	/* Refresh */
	Term_fresh();

	/* Note that the player is not dead */
#ifdef JP
(void)strcpy(died_from, "(�����������Ƥ���)");
#else
	(void)strcpy(died_from, "(alive and well)");
#endif

}


/*
 * Save the game and exit
 */
void do_cmd_save_and_exit(void)
{
	alive = FALSE;

	/* Leaving */
	p_ptr->leaving = TRUE;
#ifdef JP
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "----����������----");
#else
	do_cmd_write_nikki(NIKKI_GAMESTART, 0, "---- Save and Exit Game ----");
#endif
}


/*
 * Hack -- Calculates the total number of points earned		-JWT-
 */
long total_points(void)
{
	int i, mult = 100;
	s16b max_dl = 0;
	u32b point, point_h, point_l;
	int arena_win = MIN(p_ptr->arena_number, MAX_ARENA_MONS);

	if (stupid_monsters) mult -= 70;
	if (!preserve_mode) mult += 10;
	if (!autoroller) mult += 10;
	if (!smart_learn) mult -= 20;
	if (!terrain_streams) mult -= 20;
	if (smart_cheat) mult += 30;
	if (ironman_shops) mult += 50;
	if (ironman_small_levels) mult += 10;
	if (ironman_empty_levels) mult += 20;
	if (!powerup_home) mult += 50;
	if (ironman_rooms) mult += 100;
	if (ironman_nightmare) mult += 100;

	if (mult < 5) mult = 5;

        for (i = 0; i < max_d_idx; i++)
                if(max_dlv[i] > max_dl)
                        max_dl = max_dlv[i];

	point_l = (p_ptr->max_exp + (100 * max_dl));
	point_h = point_l / 0x10000L;
	point_l = point_l % 0x10000L;
	point_h *= mult;
	point_l *= mult;
	point_h += point_l / 0x10000L;
	point_l %= 0x10000L;

	point_l += ((point_h % 100) << 16);
	point_h /= 100;
	point_l /= 100;

	point = (point_h << 16) + (point_l);
	if (p_ptr->arena_number < 99)
		point += (arena_win * arena_win * (arena_win > 29 ? 1000 : 100));

	if (ironman_downward) point *= 2;
	if (p_ptr->pclass == CLASS_BERSERKER)
	{
		if ((p_ptr->prace == RACE_SPECTRE) || (p_ptr->prace == RACE_AMBERITE))
			point = point / 5;
	}

	if ((p_ptr->pseikaku == SEIKAKU_MUNCHKIN) && point)
	{
		point = 1;
		if (total_winner) point = 2;
	}
	if (easy_band) point = (0 - point);

	return point;
}



/*
 * Centers a string within a 31 character string		-JWT-
 */
static void center_string(char *buf, cptr str)
{
	int i, j;

	/* Total length */
	i = strlen(str);

	/* Necessary border */
	j = 15 - i / 2;

	/* Mega-Hack */
	(void)sprintf(buf, "%*s%s%*s", j, "", str, 31 - i - j, "");
}


#if 0
/*
 * Save a "bones" file for a dead character
 *
 * Note that we will not use these files until Angband 2.8.0, and
 * then we will only use the name and level on which death occured.
 *
 * Should probably attempt some form of locking...
 */
static void make_bones(void)
{
	FILE                *fp;

	char                str[1024];


	/* Ignore wizards and borgs */
	if (!(noscore & 0x00FF))
	{
		/* Ignore people who die in town */
		if (dun_level)
		{
			char tmp[128];

			/* XXX XXX XXX "Bones" name */
			sprintf(tmp, "bone.%03d", dun_level);

			/* Build the filename */
			path_build(str, 1024, ANGBAND_DIR_BONE, tmp);

			/* Attempt to open the bones file */
			fp = my_fopen(str, "r");

			/* Close it right away */
			if (fp) my_fclose(fp);

			/* Do not over-write a previous ghost */
			if (fp) return;

			/* File type is "TEXT" */
			FILE_TYPE(FILE_TYPE_TEXT);

			/* Try to write a new "Bones File" */
			fp = my_fopen(str, "w");

			/* Not allowed to write it?  Weird. */
			if (!fp) return;

			/* Save the info */
			fprintf(fp, "%s\n", player_name);
			fprintf(fp, "%d\n", p_ptr->mhp);
			fprintf(fp, "%d\n", p_ptr->prace);
			fprintf(fp, "%d\n", p_ptr->pclass);

			/* Close and save the Bones file */
			my_fclose(fp);
		}
	}
}
#endif


/*
 * Redefinable "print_tombstone" action
 */
bool (*tombstone_aux)(void) = NULL;


/*
 * Display a "tomb-stone"
 */
static void print_tomb(void)
{
	bool done = FALSE;

	/* Do we use a special tombstone ? */
	if (tombstone_aux)
	{
		/* Use tombstone hook */
		done = (*tombstone_aux)();
	}

	/* Print the text-tombstone */
	if (!done)
	{
		cptr	p;

		char	tmp[160];

		char	buf[1024];
#ifndef JP
		char    dummy[80];
#endif

		FILE        *fp;

		time_t	ct = time((time_t)0);


		/* Clear screen */
		Term_clear();

		/* Build the filename */
#ifdef JP
		path_build(buf, 1024, ANGBAND_DIR_FILE, "dead_j.txt");
#else
		path_build(buf, 1024, ANGBAND_DIR_FILE, "dead.txt");
#endif


		/* Open the News file */
		fp = my_fopen(buf, "r");

		/* Dump */
		if (fp)
		{
			int i = 0;

			/* Dump the file to the screen */
			while (0 == my_fgets(fp, buf, 1024))
			{
				/* Display and advance */
				put_str(buf, i++, 0);
			}

			/* Close */
			my_fclose(fp);
		}


		/* King or Queen */
		if (total_winner || (p_ptr->lev > PY_MAX_LEVEL))
		{
#ifdef JP
                /* �����ڤ��ؤ� */
		  p= "����ʤ��";
#else
			p = "Magnificent";
#endif

		}

		/* Normal */
		else
		{
			p =  player_title[p_ptr->pclass][(p_ptr->lev - 1) / 5];
		}

		center_string(buf, player_name);
		put_str(buf, 6, 11);

#ifndef JP
		center_string(buf, "the");
		put_str(buf, 7, 11);
#endif

		center_string(buf, p);
		put_str(buf, 8, 11);


		center_string(buf, cp_ptr->title);

		put_str(buf, 10, 11);

#ifdef JP
(void)sprintf(tmp, "��٥�: %d", (int)p_ptr->lev);
#else
		(void)sprintf(tmp, "Level: %d", (int)p_ptr->lev);
#endif

		center_string(buf, tmp);
		put_str(buf, 11, 11);

#ifdef JP
(void)sprintf(tmp, "�и���: %ld", (long)p_ptr->exp);
#else
		(void)sprintf(tmp, "Exp: %ld", (long)p_ptr->exp);
#endif

		center_string(buf, tmp);
		put_str(buf, 12, 11);

#ifdef JP
(void)sprintf(tmp, "�����: %ld", (long)p_ptr->au);
#else
		(void)sprintf(tmp, "AU: %ld", (long)p_ptr->au);
#endif

		center_string(buf, tmp);
		put_str(buf, 13, 11);

#ifdef JP
        /* ��˹����դ򥪥ꥸ�ʥ���٤���ɽ�� */
        if (streq(died_from, "���潪λ"))
        {
                strcpy(tmp, "<����>");
        }
        else
        {
                if (streq(died_from, "ripe"))
                {
                        strcpy(tmp, "������ŷ��������");
                }
                else if (streq(died_from, "Seppuku"))
                {
                        strcpy(tmp, "�����θ塢��ʢ");
                }
                else
                {
                        strcpy(tmp, died_from);
                }
        }
        center_string(buf, tmp);
        put_str(buf, 14, 11);

        if(!streq(died_from, "ripe") && !streq(died_from, "Seppuku"))
        {
                if( dun_level == 0 )
                {
			cptr town = (p_ptr->town_num ? "��" : "����");
                        if(streq(died_from, "���潪λ"))
                        {
                                sprintf(tmp, "%s�ǻ���", town);
                        }
                        else
                        {
                                sprintf(tmp, "��%s�ǻ����줿", town);
                        }
                }
                else
                {
                        if(streq(died_from, "���潪λ"))
                        {
                                sprintf(tmp, "�ϲ� %d ���ǻ���", dun_level);
                        }
                        else
                        {
                                sprintf(tmp, "���ϲ� %d ���ǻ����줿", dun_level);
                        }
                }
                center_string(buf, tmp);
                put_str(buf, 15, 11);
        }
#else
		(void)sprintf(tmp, "Killed on Level %d", dun_level);
		center_string(buf, tmp);
		put_str(buf, 14, 11);


		if (strlen(died_from) > 24)
		{
			strncpy(dummy, died_from, 24);
			dummy[24] = '\0';
			(void)sprintf(tmp, "by %s.", dummy);
		}
		else
			(void)sprintf(tmp, "by %s.", died_from);

		center_string(buf, tmp);
		put_str(buf, 15, 11);
#endif



		(void)sprintf(tmp, "%-.24s", ctime(&ct));
		center_string(buf, tmp);
		put_str(buf, 17, 11);

#ifdef JP
msg_format("���褦�ʤ顢%s!", player_name);
#else
		msg_format("Goodbye, %s!", player_name);
#endif

	}
}


/*
 * Display some character info
 */
static void show_info(void)
{
	int             i, j, k, l;
	object_type		*o_ptr;
	store_type		*st_ptr;

	/* Hack -- Know everything in the inven/equip */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);
	}

	for (i = 1; i < max_towns; i++)
	{
		st_ptr = &town[i].store[STORE_HOME];

		/* Hack -- Know everything in the home */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			o_ptr = &st_ptr->stock[j];

			/* Skip non-objects */
			if (!o_ptr->k_idx) continue;

			/* Aware and Known */
			object_aware(o_ptr);
			object_known(o_ptr);
		}
	}

	/* Hack -- Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Flush all input keys */
	flush();

	/* Flush messages */
	msg_print(NULL);


	/* Describe options */
#ifdef JP
prt("����饯�����ε�Ͽ��ե�����˽񤭽Ф����Ȥ��Ǥ��ޤ���", 21, 0);
prt("�꥿���󥭡��ǥ���饯�����򸫤ޤ���ESC�����Ǥ��ޤ���", 22, 0);
#else
	prt("You may now dump a character record to one or more files.", 21, 0);
	prt("Then, hit RETURN to see the character, or ESC to abort.", 22, 0);
#endif


	/* Dump character records as requested */
	while (TRUE)
	{
		char out_val[160];

		/* Prompt */
#ifdef JP
put_str("�ե�����͡���: ", 23, 0);
#else
		put_str("Filename: ", 23, 0);
#endif


		/* Default */
		strcpy(out_val, "");

		/* Ask for filename (or abort) */
		if (!askfor_aux(out_val, 60)) return;

		/* Return means "show on screen" */
		if (!out_val[0]) break;

		/* Save screen */
		screen_save();

		/* Dump a character file */
		(void)file_character(out_val, TRUE);

		/* Load screen */
		screen_load();
	}

	update_playtime();

	/* Display player */
	display_player(0);

	/* Prompt for inventory */
#ifdef JP
prt("���������򲡤��Ȥ���˾���³���ޤ� (ESC������): ", 23, 0);
#else
	prt("Hit any key to see more information (ESC to abort): ", 23, 0);
#endif


	/* Allow abort at this point */
	if (inkey() == ESCAPE) return;


	/* Show equipment and inventory */

	/* Equipment -- if any */
	if (equip_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_equip(0);
#ifdef JP
prt("�������Ƥ��������ƥ�: -³��-", 0, 0);
#else
		prt("You are using: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Inventory -- if any */
	if (inven_cnt)
	{
		Term_clear();
		item_tester_full = TRUE;
		(void)show_inven(0);
#ifdef JP
prt("���äƤ��������ƥ�: -³��-", 0, 0);
#else
		prt("You are carrying: -more-", 0, 0);
#endif

		if (inkey() == ESCAPE) return;
	}

	/* Homes in the different towns */
	for (l = 1; l < max_towns; l++)
	{
		st_ptr = &town[l].store[STORE_HOME];

		/* Home -- if anything there */
		if (st_ptr->stock_num)
		{
			/* Display contents of the home */
			for (k = 0, i = 0; i < st_ptr->stock_num; k++)
			{
				/* Clear screen */
				Term_clear();

				/* Show 12 items */
				for (j = 0; (j < 12) && (i < st_ptr->stock_num); j++, i++)
				{
					char o_name[MAX_NLEN];
					char tmp_val[80];

					/* Acquire item */
					o_ptr = &st_ptr->stock[i];

					/* Print header, clear line */
					sprintf(tmp_val, "%c) ", I2A(j));
					prt(tmp_val, j+2, 4);

					/* Display object description */
					object_desc(o_name, o_ptr, TRUE, 3);
					c_put_str(tval_to_attr[o_ptr->tval], o_name, j+2, 7);
				}

				/* Caption */
#ifdef JP
prt(format("�椬�Ȥ��֤��Ƥ��ä������ƥ� ( %d �ڡ���): -³��-", k+1), 0, 0);
#else
				prt(format("Your home contains (page %d): -more-", k+1), 0, 0);
#endif


				/* Wait for it */
				if (inkey() == ESCAPE) return;
			}
		}
	}
}


static bool check_score(void)
{
	/* Clear screen */
	Term_clear();

	/* No score file */
	if (highscore_fd < 0)
	{
#ifdef JP
msg_print("���������ե����뤬���ѤǤ��ޤ���");
#else
		msg_print("Score file unavailable.");
#endif

		msg_print(NULL);
		return FALSE;
	}

#ifndef SCORE_WIZARDS
	/* Wizard-mode pre-empts scoring */
	if (noscore & 0x000F)
	{
#ifdef JP
msg_print("���������ɡ��⡼�ɤǤϥ���������Ͽ����ޤ���");
#else
		msg_print("Score not registered for wizards.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_BORGS
	/* Borg-mode pre-empts scoring */
	if (noscore & 0x00F0)
	{
#ifdef JP
msg_print("�ܡ������⡼�ɤǤϥ���������Ͽ����ޤ���");
#else
		msg_print("Score not registered for borgs.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

#ifndef SCORE_CHEATERS
	/* Cheaters are not scored */
	if (noscore & 0xFF00)
	{
#ifdef JP
msg_print("�������ä��ͤϥ���������Ͽ����ޤ���");
#else
		msg_print("Score not registered for cheaters.");
#endif

		msg_print(NULL);
		return FALSE;
	}
#endif

	/* Interupted */
#ifdef JP
if (!total_winner && streq(died_from, "������λ"))
#else
	if (!total_winner && streq(died_from, "Interrupting"))
#endif

	{
#ifdef JP
msg_print("������λ�Τ��᥹��������Ͽ����ޤ���");
#else
		msg_print("Score not registered due to interruption.");
#endif

		msg_print(NULL);
		return FALSE;
	}

	/* Quitter */
#ifdef JP
if (!total_winner && streq(died_from, "���潪λ"))
#else
	if (!total_winner && streq(died_from, "Quitting"))
#endif

	{
#ifdef JP
msg_print("���潪λ�Τ��᥹��������Ͽ����ޤ���");
#else
		msg_print("Score not registered due to quitting.");
#endif

		msg_print(NULL);
		return FALSE;
	}
	return TRUE;
}

/*
 * Close up the current game (player may or may not be dead)
 *
 * This function is called only from "main.c" and "signals.c".
 */
void close_game(void)
{
	char buf[1024];
	bool do_send = TRUE;

/*	cptr p = "[i:����饯���ξ���, f:�ե�����񤭽Ф�, t:������, x:*����*, ESC:�����ཪλ]"; */

	/* Handle stuff */
	handle_stuff();

	/* Flush the messages */
	msg_print(NULL);

	/* Flush the input */
	flush();


	/* No suspending now */
	signals_ignore_tstp();


	/* Hack -- Character is now "icky" */
	character_icky = TRUE;


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_APEX, "scores.raw");

	/* Open the high score file, for reading/writing */
	highscore_fd = fd_open(buf, O_RDWR);


	/* Handle death */
	if (death)
	{
		/* Handle retirement */
		if (total_winner) kingly();

		/* Save memories */
#ifdef JP
		if (!munchkin_death || get_check("�����ǡ����򥻡��֤��ޤ����� "))
#else
		if (!munchkin_death || get_check("Save death? "))
#endif
		{

#ifdef JP
if (!save_player()) msg_print("�����ּ��ԡ�");
#else
			if (!save_player()) msg_print("death save failed!");
#endif
		}
		else do_send = FALSE;

		/* You are dead */
		print_tomb();

		flush();

		/* Show more info */
		show_info();

		/* Clear screen */
		Term_clear();

		if (check_score())
		{
			if ((!send_world_score(do_send)))
			{
#ifdef JP
				if (get_check("��ǥ���������Ͽ���뤿����Ե����ޤ�����"))
#else
				if (get_check("Stand by for later score registration? "))
#endif
				{
					wait_report_score = TRUE;
					death = FALSE;
#ifdef JP
					if (!save_player()) msg_print("�����ּ��ԡ�");
#else
					if (!save_player()) msg_print("death save failed!");
#endif
				}
			}
			if (!wait_report_score)
				(void)top_twenty();
		}
		else if (highscore_fd >= 0)
		{
			display_scores_aux(0, 10, -1, NULL);
		}
#if 0
		/* Dump bones file */
		make_bones();
#endif
	}

	/* Still alive */
	else
	{
		/* Save the game */
		do_cmd_save_game(FALSE);

		/* Prompt for scores XXX XXX XXX */
#ifdef JP
prt("�꥿���󥭡��� ESC �����򲡤��Ʋ�������", 0, 40);
#else
		prt("Press Return (or Escape).", 0, 40);
#endif


		/* Predict score (or ESCAPE) */
		if (inkey() != ESCAPE) predict_score();
	}


	/* Shut the high score file */
	(void)fd_close(highscore_fd);

	/* Forget the high score fd */
	highscore_fd = -1;


	/* Allow suspending now */
	signals_handle_tstp();
}


/*
 * Handle abrupt death of the visual system
 *
 * This routine is called only in very rare situations, and only
 * by certain visual systems, when they experience fatal errors.
 *
 * XXX XXX Hack -- clear the death flag when creating a HANGUP
 * save file so that player can see tombstone when restart.
 */
void exit_game_panic(void)
{
	/* If nothing important has happened, just quit */
#ifdef JP
if (!character_generated || character_saved) quit("�۵޻���");
#else
	if (!character_generated || character_saved) quit("panic");
#endif


	/* Mega-Hack -- see "msg_print()" */
	msg_flag = FALSE;

	/* Clear the top line */
	prt("", 0, 0);

	/* Hack -- turn off some things */
	disturb(1, 0);

	/* Mega-Hack -- Delay death */
	if (p_ptr->chp < 0) death = FALSE;

	/* Hardcode panic save */
	panic_save = 1;

	/* Forbid suspend */
	signals_ignore_tstp();

	/* Indicate panic save */
#ifdef JP
(void)strcpy(died_from, "(�۵ޥ�����)");
#else
	(void)strcpy(died_from, "(panic save)");
#endif


	/* Panic save, or get worried */
#ifdef JP
if (!save_player()) quit("�۵ޥ����ּ��ԡ�");
#else
	if (!save_player()) quit("panic save failed!");
#endif


	/* Successful panic save */
#ifdef JP
quit("�۵ޥ�����������");
#else
	quit("panic save succeeded!");
#endif

}


/*
 * Get a random line from a file
 * Based on the monster speech patch by Matt Graham,
 */
errr get_rnd_line(cptr file_name, int entry, char *output)
{
	FILE    *fp;
	char    buf[1024];
	int     line, counter, test, numentries;
	int     line_num = 0;
	bool    found = FALSE;


	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_FILE, file_name);

	/* Open the file */
	fp = my_fopen(buf, "r");

	/* Failed */
	if (!fp) return (-1);

	/* Find the entry of the monster */
	while (TRUE)
	{
		/* Get a line from the file */
		if (my_fgets(fp, buf, 1024) == 0)
		{
			/* Count the lines */
			line_num++;

			/* Look for lines starting with 'N:' */
			if ((buf[0] == 'N') && (buf[1] == ':'))
			{
				/* Allow default lines */
				if (buf[2] == '*')
				{
					/* Default lines */
					found = TRUE;
					break;
				}
				else if (buf[2] == 'M')
				{
					if (r_info[entry].flags1 & RF1_MALE)
					{
						found = TRUE;
						break;
					}
				}
				else if (buf[2] == 'F')
				{
					if (r_info[entry].flags1 & RF1_FEMALE)
					{
						found = TRUE;
						break;
					}
				}
				/* Get the monster number */
				else if (sscanf(&(buf[2]), "%d", &test) != EOF)
				{
					/* Is it the right monster? */
					if (test == entry)
					{
						found = TRUE;
						break;
					}
				}
				else
				{
					/* Error while converting the monster number */
					msg_format("Error in line %d of %s!",
					          line_num, file_name);
					my_fclose(fp);
					return (-1);
				}
			}
		}
		else
		{
			/* Reached end of file */
			my_fclose(fp);
			return (-1);
		}

	}

	/* Get the number of entries */
	while (TRUE)
	{
		/* Get the line */
		if (my_fgets(fp, buf, 1024) == 0)
		{
			/* Count the lines */
			line_num++;

			/* Look for the number of entries */
			if (isdigit(buf[0]))
			{
				/* Get the number of entries */
				numentries = atoi(buf);
				break;
			}
		}
		else
		{
			/* Count the lines */
			line_num++;

			/* Reached end of file without finding the number */
			msg_format("Error in line %d of %s!",
			          line_num, file_name);

			my_fclose(fp);
			return (-1);
		}
	}

	if (numentries > 0)
	{
		/* Grab an appropriate line number */
		line = rand_int(numentries);

		/* Get the random line */
		for (counter = 0; counter <= line; counter++)
		{
			/* Count the lines */
			line_num++;

#ifdef JP
                      while(TRUE){
                      test=my_fgets(fp, buf, 1024);
                      if(test || buf[0]!='#')break;
                                 }
                        if (test==0){
#else
			/* Try to read the line */
			if (my_fgets(fp, buf, 1024) == 0)
			{
#endif

				/* Found the line */
				if (counter == line) break;
			}
			else
			{
				/* Error - End of file */
				msg_format("Error in line %d of %s!",
				          line_num, file_name);

				my_fclose(fp);
				return (-1);
			}
		}

		/* Copy the line */
		strcpy(output, buf);
	}
	else
	{
		return (-1);
	}

	/* Close the file */
	my_fclose(fp);

	/* Success */
	return (0);
}


#ifdef JP
errr get_rnd_line_jonly(cptr file_name, int entry, char *output, int count)
{
  int i,j,kanji;
  errr result=1;
  for (i=0;i<count;i++){
    result=get_rnd_line(file_name, entry, output);
    if(result)break;
    kanji=0;
    for(j=0 ; j<strlen(output) ; j++) kanji|=iskanji( output[j] );
    if(kanji)break;
  }
  return(result);
}
#endif


/*AUTOPICK*/
errr process_pickpref_file(cptr name)
{
	FILE *fp;

	char buf[1024] , *s, *s2, isnew;

	int i;

	int num = -1;

	errr err = 0;

	bool bypass = FALSE;

	/* Build the filename */
	path_build(buf, 1024, ANGBAND_DIR_USER, name);

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
		if (buf[0] == '\0') continue;

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
			v = process_pref_file_expr(&s, &f);

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
			(void)process_pickpref_file(buf + 2);

			/* Continue */
			continue;
		}

                /* Nuke illegal char */
                for(i=0 ; buf[i]; i++)
		{
#ifdef JP
			if (iskanji(buf[i]))
			{
				i++;
				continue;
			}
#endif
			if (isspace(buf[i]) && buf[i] != ' ')
				break;
		}
		buf[i]=0;

                s = buf;
                /* Skip '!','~' */
                if(buf[0] == '!' || buf[0] == '~') s++;

                /* Auto-inscription? */
                s2=strchr(s,'#');
                if (s2) {*s2=0; s2++;}

                /* Skip empty line */
                if (*s == 0) continue;

		/* don't mind upper or lower case */
		for (i = 0; s[i]; i++)
		{
#ifdef JP
			if (iskanji(s[i]))
			{
				i++;
				continue;
			}
#endif
			if ('#' == s[i])
				break;
			else if (isupper(s[i]))
				s[i] = tolower(s[i]);
		}
 
                /* Already has the same entry? */ 
		isnew=1;
                for(i=0;i<max_autopick;i++)
			if( !strcmp(s,autopick_name[i]) ){isnew=0;break;} 

		if(isnew==0) continue;
		autopick_name [max_autopick] = malloc(strlen(s) + 1);
		strcpy(autopick_name [max_autopick], s);
		switch(buf[0]){
		case '~':
			autopick_action[max_autopick] = DONT_AUTOPICK;
			break;
		case '!':
			autopick_action[max_autopick] = DO_AUTODESTROY;
			break;
		default:
			autopick_action[max_autopick] = DO_AUTOPICK;
			break;
		}
                if( s2 ) {
                    autopick_insc[max_autopick] = malloc(strlen(s2) + 1);
                    strcpy(autopick_insc[max_autopick], s2);
                } else {
                    autopick_insc[max_autopick]=NULL;
		}
		max_autopick++;
		if(max_autopick==MAX_AUTOPICK) break;
	}

	/* Close the file */
	my_fclose(fp);
	/* Result */
	return (err);
}

static errr counts_seek(int fd, s32b where, bool flag)
{
	huge seekpoint;
	char temp1[128], temp2[128];
	s32b zero_header[3] = {0L, 0L, 0L};
	int i;

#ifdef SAVEFILE_USE_UID
	(void)sprintf(temp1, "%d.%s.%d%d%d", player_uid, savefile_base, p_ptr->pclass, p_ptr->pseikaku, p_ptr->age);
#else
	(void)sprintf(temp1, "%s.%d%d%d", savefile_base, p_ptr->pclass, p_ptr->pseikaku, p_ptr->age);
#endif
	for (i = 0; temp1[i]; i++)
		temp1[i] ^= (i+1) * 63;

	seekpoint = 0;
	while (1)
	{
		if (fd_seek(fd, seekpoint + 3 * sizeof(s32b)))
			return 1;
		if (fd_read(fd, (char*)(temp2), sizeof(temp2)))
		{
			if (!flag)
				return 1;
			/* add new name */
			fd_seek(fd, seekpoint);
			fd_write(fd, (char*)zero_header, 3*sizeof(s32b));
			fd_write(fd, (char*)(temp1), sizeof(temp1));
			break;
		}

		if (strcmp(temp1, temp2) == 0)
			break;

		seekpoint += 128 + 3 * sizeof(s32b);
	}

	return fd_seek(fd, seekpoint + where * sizeof(s32b));
}

s32b counts_read(int where)
{
	int fd;
	s32b count = 0;
	char buf[1024];

#ifdef JP
	path_build(buf, 1024, ANGBAND_DIR_DATA, "z_info_j.raw");
#else
	path_build(buf, 1024, ANGBAND_DIR_DATA, "z_info.raw");
#endif
	fd = fd_open(buf, O_RDONLY);

	if (counts_seek(fd, where, FALSE) ||
	    fd_read(fd, (char*)(&count), sizeof(s32b)))
		count = 0;

	(void)fd_close(fd);

	return count;
}

errr counts_write(int where, s32b count)
{
	int fd;
	char buf[1024];

#ifdef JP
	path_build(buf, 1024, ANGBAND_DIR_DATA, "z_info_j.raw");
#else
	path_build(buf, 1024, ANGBAND_DIR_DATA, "z_info.raw");
#endif
	fd = fd_open(buf, O_RDWR);
	if (fd < 0)
	{
		/* File type is "DATA" */
		FILE_TYPE(FILE_TYPE_DATA);

		/* Create a new high score file */
		fd = fd_make(buf, 0644);
	}

	if (fd_lock(fd, F_WRLCK)) return 1;

	counts_seek(fd, where, TRUE);
	fd_write(fd, (char*)(&count), sizeof(s32b));

	if (fd_lock(fd, F_UNLCK)) return 1;

	(void)fd_close(fd);

	return 0;
}


#ifdef HANDLE_SIGNALS


#include <signal.h>


/*
 * Handle signals -- suspend
 *
 * Actually suspend the game, and then resume cleanly
 */
static void handle_signal_suspend(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);

#ifdef SIGSTOP

	/* Flush output */
	Term_fresh();

	/* Suspend the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 0);

	/* Suspend ourself */
	(void)kill(0, SIGSTOP);

	/* Resume the "Term" */
	Term_xtra(TERM_XTRA_ALIVE, 1);

	/* Redraw the term */
	Term_redraw();

	/* Flush the term */
	Term_fresh();

#endif

	/* Restore handler */
	(void)signal(sig, handle_signal_suspend);
}


/*
 * Handle signals -- simple (interrupt and quit)
 *
 * This function was causing a *huge* number of problems, so it has
 * been simplified greatly.  We keep a global variable which counts
 * the number of times the user attempts to kill the process, and
 * we commit suicide if the user does this a certain number of times.
 *
 * We attempt to give "feedback" to the user as he approaches the
 * suicide thresh-hold, but without penalizing accidental keypresses.
 *
 * To prevent messy accidents, we should reset this global variable
 * whenever the user enters a keypress, or something like that.
 */
static void handle_signal_simple(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	/* Count the signals */
	signal_count++;


	/* Terminate dead characters */
	if (death)
	{
		/* Mark the savefile */
#ifdef JP
(void)strcpy(died_from, "������λ");
#else
		(void)strcpy(died_from, "Abortion");
#endif

		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
quit("������λ");
#else
		quit("interrupt");
#endif

	}

	/* Allow suicide (after 5) */
	else if (signal_count >= 5)
	{
		/* Cause of "death" */
#ifdef JP
(void)strcpy(died_from, "������λ��");
#else
		(void)strcpy(died_from, "Interrupting");
#endif


		forget_lite();
		forget_view();
		clear_mon_lite();

		/* Stop playing */
		alive = FALSE;

		/* Suicide */
		death = TRUE;

		/* Leaving */
		p_ptr->leaving = TRUE;

		/* Close stuff */
		close_game();

		/* Quit */
#ifdef JP
quit("������λ");
#else
		quit("interrupt");
#endif

	}

	/* Give warning (after 4) */
	else if (signal_count >= 4)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);

		/* Clear the top line */
		Term_erase(0, 0, 255);

		/* Display the cause */
#ifdef JP
Term_putstr(0, 0, -1, TERM_WHITE, "��θ�ξ�μ�����");
#else
		Term_putstr(0, 0, -1, TERM_WHITE, "Contemplating suicide!");
#endif


		/* Flush */
		Term_fresh();
	}

	/* Give warning (after 2) */
	else if (signal_count >= 2)
	{
		/* Make a noise */
		Term_xtra(TERM_XTRA_NOISE, 0);
	}

	/* Restore handler */
	(void)signal(sig, handle_signal_simple);
}


/*
 * Handle signal -- abort, kill, etc
 */
static void handle_signal_abort(int sig)
{
	/* Disable handler */
	(void)signal(sig, SIG_IGN);


	/* Nothing to save, just quit */
	if (!character_generated || character_saved) quit(NULL);


	forget_lite();
	forget_view();
	clear_mon_lite();

	/* Clear the bottom line */
	Term_erase(0, 23, 255);

	/* Give a warning */
	Term_putstr(0, 23, -1, TERM_RED,
#ifdef JP
"���������եȤΥХ������Ӥ����äƤ�����");
#else
	            "A gruesome software bug LEAPS out at you!");
#endif


	/* Message */
#ifdef JP
Term_putstr(45, 23, -1, TERM_RED, "�۵ޥ�����...");
#else
	Term_putstr(45, 23, -1, TERM_RED, "Panic save...");
#endif


	/* Flush output */
	Term_fresh();

	/* Panic Save */
	panic_save = 1;

	/* Panic save */
#ifdef JP
(void)strcpy(died_from, "(�۵ޥ�����)");
#else
	(void)strcpy(died_from, "(panic save)");
#endif


	/* Forbid suspend */
	signals_ignore_tstp();

	/* Attempt to save */
	if (save_player())
	{
#ifdef JP
Term_putstr(45, 23, -1, TERM_RED, "�۵ޥ�����������");
#else
		Term_putstr(45, 23, -1, TERM_RED, "Panic save succeeded!");
#endif

	}

	/* Save failed */
	else
	{
#ifdef JP
Term_putstr(45, 23, -1, TERM_RED, "�۵ޥ����ּ��ԡ�");
#else
		Term_putstr(45, 23, -1, TERM_RED, "Panic save failed!");
#endif

	}

	/* Flush output */
	Term_fresh();

	/* Quit */
#ifdef JP
quit("���եȤΥХ�");
#else
	quit("software bug");
#endif

}




/*
 * Ignore SIGTSTP signals (keyboard suspend)
 */
void signals_ignore_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, SIG_IGN);
#endif

}

/*
 * Handle SIGTSTP signals (keyboard suspend)
 */
void signals_handle_tstp(void)
{

#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif

}


/*
 * Prepare to handle the relevant signals
 */
void signals_init(void)
{

#ifdef SIGHUP
	(void)signal(SIGHUP, SIG_IGN);
#endif


#ifdef SIGTSTP
	(void)signal(SIGTSTP, handle_signal_suspend);
#endif


#ifdef SIGINT
	(void)signal(SIGINT, handle_signal_simple);
#endif

#ifdef SIGQUIT
	(void)signal(SIGQUIT, handle_signal_simple);
#endif


#ifdef SIGFPE
	(void)signal(SIGFPE, handle_signal_abort);
#endif

#ifdef SIGILL
	(void)signal(SIGILL, handle_signal_abort);
#endif

#ifdef SIGTRAP
	(void)signal(SIGTRAP, handle_signal_abort);
#endif

#ifdef SIGIOT
	(void)signal(SIGIOT, handle_signal_abort);
#endif

#ifdef SIGKILL
	(void)signal(SIGKILL, handle_signal_abort);
#endif

#ifdef SIGBUS
	(void)signal(SIGBUS, handle_signal_abort);
#endif

#ifdef SIGSEGV
	(void)signal(SIGSEGV, handle_signal_abort);
#endif

#ifdef SIGTERM
	(void)signal(SIGTERM, handle_signal_abort);
#endif

#ifdef SIGPIPE
	(void)signal(SIGPIPE, handle_signal_abort);
#endif

#ifdef SIGEMT
	(void)signal(SIGEMT, handle_signal_abort);
#endif

#ifdef SIGDANGER
	(void)signal(SIGDANGER, handle_signal_abort);
#endif

#ifdef SIGSYS
	(void)signal(SIGSYS, handle_signal_abort);
#endif

#ifdef SIGXCPU
	(void)signal(SIGXCPU, handle_signal_abort);
#endif

#ifdef SIGPWR
	(void)signal(SIGPWR, handle_signal_abort);
#endif

}


#else	/* HANDLE_SIGNALS */


/*
 * Do nothing
 */
void signals_ignore_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_handle_tstp(void)
{
}

/*
 * Do nothing
 */
void signals_init(void)
{
}
#endif	/* HANDLE_SIGNALS */
