/* Purpose: Object flavor code */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- note that "TERM_MULTI" is now just "TERM_VIOLET".
 * We will have to find a cleaner method for "MULTI_HUED" later.
 * There were only two multi-hued "flavors" (one potion, one food).
 * Plus five multi-hued "base-objects" (3 dragon scales, one blade
 * of chaos, and one something else).  See the SHIMMER_OBJECTS code
 * in "dungeon.c" and the object color extractor in "cave.c".
 */
#define TERM_MULTI      TERM_VIOLET


/*
 * Max sizes of the following arrays
 */
#define MAX_ROCKS      59       /* Used with rings (min 38) */
#define MAX_AMULETS    26       /* Used with amulets (min 14) */
#define MAX_WOODS      34       /* Used with staffs (min 30) */
#define MAX_METALS     40       /* Used with wands/rods (min 29/29) */
#define MAX_COLORS     70       /* Used with potions (min 60) */
#define MAX_SHROOM     20       /* Used with mushrooms (min 20) */
#define MAX_TITLES     55       /* Used with scrolls (min 48) */
#define MAX_SYLLABLES 164       /* Used with scrolls (see below) */


/*
 * Rings (adjectives and colors)
 */

static cptr ring_adj[MAX_ROCKS] =
{
	"Alexandrite", "Amethyst", "Aquamarine", "Azurite", "Beryl",
	"Bloodstone", "Calcite", "Carnelian", "Corundum", "Diamond",
	"Emerald", "Fluorite", "Garnet", "Granite", "Jade",
	"Jasper", "Lapis Lazuli", "Malachite", "Marble", "Moonstone",
	"Onyx", "Opal", "Pearl", "Quartz", "Quartzite",
	"Rhodonite", "Ruby", "Sapphire", "Tiger Eye", "Topaz",
	"Turquoise", "Zircon", "Platinum", "Bronze", "Gold",
	"Obsidian", "Silver", "Tortoise Shell", "Mithril", "Jet",
	"Engagement", "Adamantite",
	"Wire", "Dilithium", "Bone", "Wooden",
	"Spikard", "Serpent",   "Wedding", "Double",
	"Plain", "Brass",  "Scarab","Shining",
	"Rusty","Transparent", "Steel", "Tanzanite",
	"Nephrite",
};

#ifdef JP
static cptr J_ring_adj[MAX_ROCKS] =
{
    "�����Ф�","���᥸���Ȥ�","�������ޥ���","��Τ���","�����Ф�",
    "��̿��","�����Ф�","�֤�Τ���","�˶̤�","���������ɤ�",
    "������ɤ�","�ۥ����Ф�","�����ͥåȤ�","����Ф�","�Ҥ�����",
    "���㥹�ѡ���","��������","�����㥯�Ф�","�����Ф�","�ࡼ�󥹥ȡ����",
    "�ʤ�Τ���","���ѡ����","�����","�徽��","�бѴ��",
    "�������Ф�","��ӡ���","���ե�������","��������������","�ȥѡ�����",
    "�ȥ륳�Ф�","���륳���","�ץ���ʤ�","�֥�󥺤�","���",
    "�����Ф�","���","�٤ùä�","�ߥ�����","���̤�",
    "����","�����ޥ󥿥��Ȥ�",
    "�˶��","�ǥ��ꥷ�����","����","�ڤ�",
    "���ԥ���" /*nuke me*/ ,"�ؤ�","�뺧","��Ť�",
    "����Τʤ�","��Ƽ��","������٤�" ,"����",
    "���Ӥ�","Ʃ����","��Ŵ��","���󥶥ʥ��Ȥ�",
    "��̤�",
};
#endif
static byte ring_col[MAX_ROCKS] =
{
	TERM_GREEN, TERM_VIOLET, TERM_L_BLUE, TERM_L_BLUE, TERM_L_GREEN,
	TERM_RED, TERM_WHITE, TERM_RED, TERM_SLATE, TERM_WHITE,
	TERM_GREEN, TERM_L_GREEN, TERM_RED, TERM_L_DARK, TERM_L_GREEN,
	TERM_UMBER, TERM_BLUE, TERM_GREEN, TERM_WHITE, TERM_L_WHITE,
	TERM_L_RED, TERM_L_WHITE, TERM_WHITE, TERM_L_WHITE, TERM_L_WHITE,
	TERM_L_RED, TERM_RED, TERM_BLUE, TERM_YELLOW, TERM_YELLOW,
	TERM_L_BLUE, TERM_L_UMBER, TERM_WHITE, TERM_L_UMBER, TERM_YELLOW,
	TERM_L_DARK, TERM_L_WHITE, TERM_GREEN, TERM_L_BLUE, TERM_L_DARK,
	TERM_YELLOW, TERM_VIOLET,
	TERM_UMBER, TERM_L_WHITE, TERM_WHITE, TERM_UMBER,
	TERM_BLUE, TERM_GREEN, TERM_YELLOW, TERM_ORANGE,
	TERM_YELLOW, TERM_ORANGE, TERM_L_GREEN, TERM_YELLOW,
	TERM_RED, TERM_WHITE, TERM_WHITE, TERM_YELLOW,
	TERM_GREEN,
};


/*
 * Amulets (adjectives and colors)
 */
static cptr amulet_adj[MAX_AMULETS] =
{
	"Amber", "Driftwood", "Coral", "Agate", "Ivory",
	"Obsidian", "Bone", "Brass", "Bronze", "Pewter",
	"Tortoise Shell", "Golden", "Azure", "Crystal", "Silver",
	"Copper", "Swastika", "Platinum","Runed", "Rusty",
	"Curved", "Dragon's claw", "Rosary", "Jade", "Mithril",
	"Ruby"
};

#ifdef JP
static cptr J_amulet_adj[MAX_AMULETS] =
{
    "�����","ή�ڤ�","���󥴤�","��Τ���","�ݲ��",
    "�����Ф�","����","��﫤�","��Ƽ��","������",
    "�٤ùä�","���","������","�徽��","���",
    "Ƽ��","�Ĥ�", "�ץ���ʤ�", "�롼�󤬹�ޤ줿","���Ӥ�",
    "�ʤ��ä�", "�ɥ饴����ޤ�", "�����", "�Ҥ�����", "�ߥ�����",
    "��ӡ���"
};
#endif
static byte amulet_col[MAX_AMULETS] =
{
	TERM_YELLOW, TERM_L_UMBER, TERM_WHITE, TERM_L_WHITE, TERM_WHITE,
	TERM_L_DARK, TERM_WHITE, TERM_L_UMBER, TERM_L_UMBER, TERM_SLATE,
	TERM_GREEN, TERM_YELLOW, TERM_L_BLUE, TERM_L_BLUE, TERM_L_WHITE,
	TERM_L_UMBER, TERM_VIOLET, TERM_WHITE, TERM_UMBER, TERM_RED, 
	TERM_GREEN, TERM_L_GREEN, TERM_L_GREEN, TERM_GREEN, TERM_L_BLUE, 
	TERM_RED
};


/*
 * Staffs (adjectives and colors)
 */
static cptr staff_adj[MAX_WOODS] =
{
	"Aspen", "Balsa", "Banyan", "Birch", "Cedar",
	"Cottonwood", "Cypress", "Dogwood", "Elm", "Eucalyptus",
	"Hemlock", "Hickory", "Ironwood", "Locust", "Mahogany",
	"Maple", "Mulberry", "Oak", "Pine", "Redwood",
	"Rosewood", "Spruce", "Sycamore", "Teak", "Walnut",
	"Mistletoe", "Hawthorn", "Bamboo", "Silver", "Runed",
	"Golden", "Ashen", "Ivory","Gnarled"/*,"Willow"*/
};

#ifdef JP
static cptr J_staff_adj[MAX_WOODS] =
{
    "�ݥץ��","�Х륵��","�Х����","���Ф�","���Υ�����",
    "�ؤ�","���ȥ�����","�ߥ�����","�˥��","�桼�����",
    "�ĥ���","�֥ʤ�","���ɤ�","����������","�ޥۥ��ˡ���",
    "�����Ǥ�","�����","������","����","����",
    "���ɤ�","�����ޥĤ�","����������","��������","����ߤ�",
    "��ɥꥮ��","���󥶥���","�ݤ�","���","�롼���",
    "���","�ȥͥꥳ��", "�ݲ��", "��˼���"
};
#endif
static byte staff_col[MAX_WOODS] =
{
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_UMBER, TERM_L_UMBER, TERM_UMBER,
	TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_RED,
	TERM_RED, TERM_L_UMBER, TERM_L_UMBER, TERM_L_UMBER, TERM_UMBER,
	TERM_GREEN, TERM_L_UMBER, TERM_L_UMBER, TERM_L_WHITE, TERM_UMBER,
	TERM_YELLOW, TERM_SLATE, TERM_WHITE, TERM_SLATE
};


/*
 * Wands (adjectives and colors)
 */
static cptr wand_adj[MAX_METALS] =
{
	"Aluminum", "Cast Iron", "Chromium", "Copper", "Gold",
	"Iron", "Magnesium", "Molybdenum", "Nickel", "Rusty",
	"Silver", "Steel", "Tin", "Titanium", "Tungsten",
	"Zirconium", "Zinc", "Aluminum-Plated", "Copper-Plated", "Gold-Plated",
	"Nickel-Plated", "Silver-Plated", "Steel-Plated", "Tin-Plated", "Zinc-Plated",
	"Mithril-Plated", "Mithril", "Runed", "Bronze", "Brass",
	"Platinum", "Lead","Lead-Plated", "Ivory" , "Adamantite",
	"Uridium", "Long", "Short", "Hexagonal", "Carbonized"
};

#ifdef JP
static cptr J_wand_adj[MAX_METALS] =
{
    "����ߤ�","��Ŵ��","������","Ƽ��","���",
    "Ŵ��","�ޥ��ͥ������","���֥ǥ��","�˥å����","���Ӥ�",
    "���","��Ŵ��","�֥ꥭ��","�������","���󥰥��ƥ��",
    "���륳���","������","����ߥ�å���","Ƽ��å���","���å���",
    "��Ƽ��å���","���å���","Ŵ��å���","������å���","������å���",
    "�ߥ�����å���","�ߥ�����","�롼�󤬹�ޤ줿","��Ƽ��","��﫤�",
    "�ץ���ʤ�","����","����å���","�ݲ��","�����ޥ󥿥��Ȥ�",
    "����¥����","Ĺ��","û��","ϻ�ѷ���", "ú�Ǥ�"
};
#endif
static byte wand_col[MAX_METALS] =
{
	TERM_L_BLUE, TERM_L_DARK, TERM_WHITE, TERM_L_UMBER, TERM_YELLOW,
	TERM_SLATE, TERM_L_WHITE, TERM_L_WHITE, TERM_L_UMBER, TERM_RED,
	TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE, TERM_WHITE, TERM_WHITE,
	TERM_L_WHITE, TERM_L_WHITE, TERM_L_BLUE, TERM_L_UMBER, TERM_YELLOW,
	TERM_L_UMBER, TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE, TERM_L_WHITE,
	TERM_L_BLUE, TERM_L_BLUE, TERM_UMBER, TERM_L_UMBER, TERM_L_UMBER,
	TERM_WHITE, TERM_SLATE, TERM_SLATE, TERM_WHITE, TERM_VIOLET,
	TERM_L_RED, TERM_L_BLUE, TERM_BLUE, TERM_RED, TERM_L_DARK
};


/*
 * Rods (adjectives and colors).
 * Efficiency -- copied from wand arrays
 */

static cptr rod_adj[MAX_METALS];

#ifdef JP
static cptr J_rod_adj[MAX_METALS];
#endif
static byte rod_col[MAX_METALS];


/*
 * Mushrooms (adjectives and colors)
 */

static cptr food_adj[MAX_SHROOM] =
{
	"Blue", "Black", "Black Spotted", "Brown", "Dark Blue",
	"Dark Green", "Dark Red", "Yellow", "Furry", "Green",
	"Grey", "Light Blue", "Light Green", "Violet", "Red",
	"Slimy", "Tan", "White", "White Spotted", "Wrinkled",
};

#ifdef JP
static cptr J_food_adj[MAX_SHROOM] =
{
    "�Ĥ�","����","���ä�","�㿧��","���Ĥ�",
    "���Ф�","�ȿ���","������","�ݤष��","�Ф�",
    "���졼��","������","���Ф�","���ߥ쿧��","�֤�",
    "�ͤФͤФ���","���쿧��","��","���ä�","���路���",
};
#endif
static byte food_col[MAX_SHROOM] =
{
	TERM_BLUE, TERM_L_DARK, TERM_L_DARK, TERM_UMBER, TERM_BLUE,
	TERM_GREEN, TERM_RED, TERM_YELLOW, TERM_L_WHITE, TERM_GREEN,
	TERM_SLATE, TERM_L_BLUE, TERM_L_GREEN, TERM_VIOLET, TERM_RED,
	TERM_SLATE, TERM_L_UMBER, TERM_WHITE, TERM_WHITE, TERM_UMBER
};


/*
 * Color adjectives and colors, for potions.
 * Hack -- The first four entries are hard-coded.
 * (water, apple juice, slime mold juice, something)
 */

static cptr potion_adj[MAX_COLORS] =
{
	"Clear", "Light Brown", "Icky Green", "xxx",
	"Azure", "Blue", "Blue Speckled", "Black", "Brown", "Brown Speckled",
	"Bubbling", "Chartreuse", "Cloudy", "Copper Speckled", "Crimson", "Cyan",
	"Dark Blue", "Dark Green", "Dark Red", "Gold Speckled", "Green",
	"Green Speckled", "Grey", "Grey Speckled", "Hazy", "Indigo",
	"Light Blue", "Light Green", "Magenta", "Metallic Blue", "Metallic Red",
	"Metallic Green", "Metallic Purple", "Misty", "Orange", "Orange Speckled",
	"Pink", "Pink Speckled", "Puce", "Purple", "Purple Speckled",
	"Red", "Red Speckled", "Silver Speckled", "Smoky", "Tangerine",
	"Violet", "Vermilion", "White", "Yellow", "Violet Speckled",
	"Pungent", "Clotted Red", "Viscous Pink", "Oily Yellow", "Gloopy Green",
	"Shimmering", "Coagulated Crimson", "Yellow Speckled", "Gold",
	"Manly", "Stinking", "Oily Black", "Ichor", "Ivory White", "Sky Blue",
	"Gray", "Silver", "Bronze", "Flashing",
};

#ifdef JP
static cptr J_potion_adj[MAX_COLORS] =
{
    "Ʃ����","���㿧��","��ɤ���Ф�","������",
    "���ˤ�","�Ĥ�","���ä�","����","�㿧��","���ä�",
    "ˢ���ä�","�����п���","���ä�","Ƽ�ä�","�ȿ���","�����󿧤�",
    "���Ŀ���","�����Ф�","�����ֿ���","���ä�","�п���",
    "���ä�","������","���ä�","�ܤ䤱������","������",
    "�����Ŀ���","�����п���","�ޥ��󥿿���","�Ĥ�����","�֤�����",
    "�Ф˵���","��˵���","̸����","����󥸿���","������ä�",
    "�ԥ󥯿���","�ԥ��ä�","���쿧��","�翧��","���������ä�",
    "�֤�","���ä�","���ä�","��ä�","������",
    "���ߥ쿧��","�뿧��","��","������", "���ä�",
    "�ɷ㽭�Τ���","��ɤ���֤�","�ɥ�ɥ��","�ͤФä�������","���п���",
    "����","�ͤФä����Ȥ�","���ä�","�⿧��",
    "�˽���" /*nuke me*/,"�����Τ���","��������","Ǿ����",
    "�ݲ翧��","������", "�ͤ��߿���", "�俧��", "��Ƽ����",
    "���饭�����"
};
#endif
static byte potion_col[MAX_COLORS] =
{
	TERM_WHITE, TERM_L_UMBER, TERM_GREEN, 0,
	TERM_L_BLUE, TERM_BLUE, TERM_BLUE, TERM_L_DARK, TERM_UMBER, TERM_UMBER,
	TERM_L_WHITE, TERM_L_GREEN, TERM_WHITE, TERM_L_UMBER, TERM_RED, TERM_L_BLUE,
	TERM_BLUE, TERM_GREEN, TERM_RED, TERM_YELLOW, TERM_GREEN,
	TERM_GREEN, TERM_SLATE, TERM_SLATE, TERM_L_WHITE, TERM_VIOLET,
	TERM_L_BLUE, TERM_L_GREEN, TERM_RED, TERM_BLUE, TERM_RED,
	TERM_GREEN, TERM_VIOLET, TERM_L_WHITE, TERM_ORANGE, TERM_ORANGE,
	TERM_L_RED, TERM_L_RED, TERM_VIOLET, TERM_VIOLET, TERM_VIOLET,
	TERM_RED, TERM_RED, TERM_L_WHITE, TERM_L_DARK, TERM_ORANGE,
	TERM_VIOLET, TERM_RED, TERM_WHITE, TERM_YELLOW, TERM_VIOLET,
	TERM_L_RED, TERM_RED, TERM_L_RED, TERM_YELLOW, TERM_GREEN,
	TERM_MULTI, TERM_RED, TERM_YELLOW, TERM_YELLOW,
	TERM_L_UMBER, TERM_UMBER, TERM_L_DARK, TERM_RED,
	TERM_WHITE, TERM_L_BLUE, TERM_L_WHITE, TERM_WHITE, TERM_RED,
	TERM_YELLOW
};


/*
 * Syllables for scrolls (must be 1-4 letters each)
 */

#ifdef JP
static cptr J_syllables[MAX_SYLLABLES] =
{
	"ŷ", "��", "̵", "��", "��", "��", "ξ", "��",
	"��", "˥", "��", "��", "��", "��", "��", "��",
	"��", "��", "ž", "Ȭ", "��", "��", "��",
	"��", "��", "��", "��", "��", "��", "ʸ",
	"��", "ƻ", "ε", "Ƭ", "��", "��", "ñ", "��", "ľ", "��",
	"��", "��", "��", "¡", "ϻ", "�", "��", "��", "��",
	"ư", "��", "��", "��", "��", "��", "̿", "��",
	"��", "��", "��", "��", "��", "��", "ķ", "��",
	"��", "�", "�", "̥", "�", "�", "��",
	"��", "��", "��", "��", "��", "��", "��", "��", "��",
	"��", "��", "��", "��", "��", "��", "��", "��",
	"̣", "��", "��", "��", "��", "��", "��", "߸",
	"ǰ", "ȯ", "��", "��", "̤", "ʹ", "��", "��",
	"ڼ", "��", "��", "��", "��", "��", "��", "��",
	"��", "��", "Ȭ", "��", "��", "��", "��", "��",
	"��", "��", "��", "��", "��", "��", "��", "��",
	"ʻ", "��", "��", "��", "��", "��", "��", "��",
	"��", "��", "��", "��", "��", "��", "��", "��",
	"��", "��", "��", "��", "��", "Ǧ", "��", "ǽ",
	"��", "��", "��", "��", "��", "��", "��", "��",
	"��", "��", "��"
};
#else
static cptr syllables[MAX_SYLLABLES] =
{
	"a", "ab", "ag", "aks", "ala", "an", "ankh", "app",
	"arg", "arze", "ash", "aus", "ban", "bar", "bat", "bek",
	"bie", "bin", "bit", "bjor", "blu", "bot", "bu",
	"byt", "comp", "con", "cos", "cre", "dalf", "dan",
	"den", "der", "doe", "dok", "eep", "el", "eng", "er", "ere", "erk",
	"esh", "evs", "fa", "fid", "flit", "for", "fri", "fu", "gan",
	"gar", "glen", "gop", "gre", "ha", "he", "hyd", "i",
	"ing", "ion", "ip", "ish", "it", "ite", "iv", "jo",
	"kho", "kli", "klis", "la", "lech", "man", "mar",
	"me", "mi", "mic", "mik", "mon", "mung", "mur", "nag", "nej",
	"nelg", "nep", "ner", "nes", "nis", "nih", "nin", "o",
	"od", "ood", "org", "orn", "ox", "oxy", "pay", "pet",
	"ple", "plu", "po", "pot", "prok", "re", "rea", "rhov",
	"ri", "ro", "rog", "rok", "rol", "sa", "san", "sat",
	"see", "sef", "seh", "shu", "ski", "sna", "sne", "snik",
	"sno", "so", "sol", "sri", "sta", "sun", "ta", "tab",
	"tem", "ther", "ti", "tox", "trol", "tue", "turs", "u",
	"ulk", "um", "un", "uni", "ur", "val", "viv", "vly",
	"vom", "wah", "wed", "werg", "wex", "whon", "wun", "x",
	"yerg", "yp", "zun", "tri", "blaa", "jah", "bul", "on",
	"foo", "ju", "xuxu"
};
#endif


/*
 * Hold the titles of scrolls, 6 to 14 characters each
 * Also keep an array of scroll colors (always WHITE for now)
 */
static char scroll_adj[MAX_TITLES][16];

static byte scroll_col[MAX_TITLES];


/*
 * Certain items, if aware, are known instantly
 * This function is used only by "flavor_init()"
 */
static bool object_easy_know(int i)
{
	object_kind *k_ptr = &k_info[i];

	/* Analyze the "tval" */
	switch (k_ptr->tval)
	{
		/* Spellbooks */
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_ENCHANT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		{
			return (TRUE);
		}

		/* Simple items */
		case TV_FLASK:
		case TV_JUNK:
		case TV_BOTTLE:
		case TV_SKELETON:
		case TV_SPIKE:
		case TV_WHISTLE:
		{
			return (TRUE);
		}

		/* All Food, Potions, Scrolls, Rods */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		case TV_ROD:
		{
			return (TRUE);
		}
	}

	/* Nope */
	return (FALSE);
}


/*
 * Certain items have a flavor
 * This function is used only by "flavor_init()"
 */
static bool object_flavor(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item */
	switch (k_ptr->tval)
	{
		case TV_AMULET:
		{
			return (0x80 + amulet_col[k_ptr->sval]);
		}

		case TV_RING:
		{
			return (0x90 + ring_col[k_ptr->sval]);
		}

		case TV_STAFF:
		{
			return (0xA0 + staff_col[k_ptr->sval]);
		}

		case TV_WAND:
		{
			return (0xB0 + wand_col[k_ptr->sval]);
		}

		case TV_ROD:
		{
			return (0xC0 + rod_col[k_ptr->sval]);
		}

		case TV_SCROLL:
		{
			return (0xD0 + scroll_col[k_ptr->sval]);
		}

		case TV_POTION:
		{
			return (0xE0 + potion_col[k_ptr->sval]);
		}

		case TV_FOOD:
		{
			if (k_ptr->sval < SV_FOOD_MIN_FOOD)
			{
				return (0xF0 + food_col[k_ptr->sval]);
			}

			break;
		}
	}

	/* No flavor */
	return (0);
}


void get_table_name(char *out_string)
{
#ifdef JP
        char Syllable[80];
	strcpy(out_string, "��");
	get_rnd_line("aname_j.txt", 1, Syllable);
	strcat(out_string, Syllable);
	get_rnd_line("aname_j.txt", 2, Syllable);
	strcat(out_string, Syllable);
	strcat(out_string, "��");
#else
	int testcounter = randint(3) + 1;

	strcpy(out_string, "'");

	if (randint(3) == 2)
	{
		while (testcounter--)
			strcat(out_string, syllables[rand_int(MAX_SYLLABLES)]);
	}
	else
	{
		char Syllable[80];
		testcounter = randint(2) + 1;
		while (testcounter--)
		{
			(void)get_rnd_line("elvish.txt", 0, Syllable);
			strcat(out_string, Syllable);
		}
	}

	out_string[1] = toupper(out_string[1]);

	strcat(out_string, "'");
#endif


	out_string[18] = '\0';

	return;
}


/*
 * Prepare the "variable" part of the "k_info" array.
 *
 * The "color"/"metal"/"type" of an item is its "flavor".
 * For the most part, flavors are assigned randomly each game.
 *
 * Initialize descriptions for the "colored" objects, including:
 * Rings, Amulets, Staffs, Wands, Rods, Food, Potions, Scrolls.
 *
 * The first 4 entries for potions are fixed (Water, Apple Juice,
 * Slime Mold Juice, Unused Potion).
 *
 * Scroll titles are always between 6 and 14 letters long.  This is
 * ensured because every title is composed of whole words, where every
 * word is from 1 to 8 letters long (one or two syllables of 1 to 4
 * letters each), and that no scroll is finished until it attempts to
 * grow beyond 15 letters.  The first time this can happen is when the
 * current title has 6 letters and the new word has 8 letters, which
 * would result in a 6 letter scroll title.
 *
 * Duplicate titles are avoided by requiring that no two scrolls share
 * the same first four letters (not the most efficient method, and not
 * the least efficient method, but it will always work).
 *
 * Hack -- make sure everything stays the same for each saved game
 * This is accomplished by the use of a saved "random seed", as in
 * "town_gen()".  Since no other functions are called while the special
 * seed is in effect, so this function is pretty "safe".
 *
 * Note that the "hacked seed" may provide an RNG with alternating parity!
 */
void flavor_init(void)
{
	int     i, j;

	byte    temp_col;

	cptr    temp_adj;


	/* Hack -- Use the "simple" RNG */
	Rand_quick = TRUE;

	/* Hack -- Induce consistant flavors */
	Rand_value = seed_flavor;


	/* Efficiency -- Rods/Wands share initial array */
	for (i = 0; i < MAX_METALS; i++)
	{
		rod_adj[i] = wand_adj[i];
#ifdef JP
                J_rod_adj[i] = J_wand_adj[i];
#endif
		rod_col[i] = wand_col[i];
	}


	/* Rings have "ring colors" */
	for (i = 0; i < MAX_ROCKS; i++)
	{
		j = rand_int(MAX_ROCKS);
		temp_adj = ring_adj[i];
		ring_adj[i] = ring_adj[j];
		ring_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_ring_adj[i];
                J_ring_adj[i] = J_ring_adj[j];
                J_ring_adj[j] = temp_adj;
#endif
		temp_col = ring_col[i];
		ring_col[i] = ring_col[j];
		ring_col[j] = temp_col;
	}

	/* Amulets have "amulet colors" */
	for (i = 0; i < MAX_AMULETS; i++)
	{
		j = rand_int(MAX_AMULETS);
		temp_adj = amulet_adj[i];
		amulet_adj[i] = amulet_adj[j];
		amulet_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_amulet_adj[i];
                J_amulet_adj[i] = J_amulet_adj[j];
                J_amulet_adj[j] = temp_adj;
#endif
		temp_col = amulet_col[i];
		amulet_col[i] = amulet_col[j];
		amulet_col[j] = temp_col;
	}

	/* Staffs */
	for (i = 0; i < MAX_WOODS; i++)
	{
		j = rand_int(MAX_WOODS);
		temp_adj = staff_adj[i];
		staff_adj[i] = staff_adj[j];
		staff_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_staff_adj[i];
                J_staff_adj[i] = J_staff_adj[j];
                J_staff_adj[j] = temp_adj;
#endif
		temp_col = staff_col[i];
		staff_col[i] = staff_col[j];
		staff_col[j] = temp_col;
	}

	/* Wands */
	for (i = 0; i < MAX_METALS; i++)
	{
		j = rand_int(MAX_METALS);
		temp_adj = wand_adj[i];
		wand_adj[i] = wand_adj[j];
		wand_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_wand_adj[i];
                J_wand_adj[i] = J_wand_adj[j];
                J_wand_adj[j] = temp_adj;
#endif
		temp_col = wand_col[i];
		wand_col[i] = wand_col[j];
		wand_col[j] = temp_col;
	}

	/* Rods */
	for (i = 0; i < MAX_METALS; i++)
	{
		j = rand_int(MAX_METALS);
		temp_adj = rod_adj[i];
		rod_adj[i] = rod_adj[j];
		rod_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_rod_adj[i];
                J_rod_adj[i] = J_rod_adj[j];
                J_rod_adj[j] = temp_adj;
#endif
		temp_col = rod_col[i];
		rod_col[i] = rod_col[j];
		rod_col[j] = temp_col;
	}

	/* Foods (Mushrooms) */
	for (i = 0; i < MAX_SHROOM; i++)
	{
		j = rand_int(MAX_SHROOM);
		temp_adj = food_adj[i];
		food_adj[i] = food_adj[j];
		food_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_food_adj[i];
                J_food_adj[i] = J_food_adj[j];
                J_food_adj[j] = temp_adj;
#endif
		temp_col = food_col[i];
		food_col[i] = food_col[j];
		food_col[j] = temp_col;
	}

	/* Potions */
	for (i = 4; i < MAX_COLORS; i++)
	{
		j = rand_int(MAX_COLORS - 4) + 4;
		temp_adj = potion_adj[i];
		potion_adj[i] = potion_adj[j];
		potion_adj[j] = temp_adj;
#ifdef JP
                temp_adj = J_potion_adj[i];
                J_potion_adj[i] = J_potion_adj[j];
                J_potion_adj[j] = temp_adj;
#endif
		temp_col = potion_col[i];
		potion_col[i] = potion_col[j];
		potion_col[j] = temp_col;
	}

	/* Scrolls (random titles, always white) */
	for (i = 0; i < MAX_TITLES; i++)
	{
		/* Get a new title */
		while (TRUE)
		{
			char buf[80];

			bool okay;

			/* Start a new title */
			buf[0] = '\0';

			/* Collect words until done */
			while (1)
			{
				int q, s;

				char tmp[80];

				/* Start a new word */
				tmp[0] = '\0';

				/* Choose one or two syllables */
				s = ((rand_int(100) < 30) ? 1 : 2);

				/* Add a one or two syllable word */
				for (q = 0; q < s; q++)
				{
					/* Add the syllable */
#ifdef JP
					strcat(tmp, J_syllables[rand_int(MAX_SYLLABLES)]);
#else
					strcat(tmp, syllables[rand_int(MAX_SYLLABLES)]);
#endif
				}

				/* Stop before getting too long */
				if (strlen(buf) + 1 + strlen(tmp) > 15) break;

				/* Add a space */
				strcat(buf, " ");

				/* Add the word */
				strcat(buf, tmp);
			}

			/* Save the title */
			strcpy(scroll_adj[i], buf+1);

			/* Assume okay */
			okay = TRUE;

			/* Check for "duplicate" scroll titles */
			for (j = 0; j < i; j++)
			{
				cptr hack1 = scroll_adj[j];
				cptr hack2 = scroll_adj[i];

				/* Compare first four characters */
				if (*hack1++ != *hack2++) continue;
				if (*hack1++ != *hack2++) continue;
				if (*hack1++ != *hack2++) continue;
				if (*hack1++ != *hack2++) continue;

				/* Not okay */
				okay = FALSE;

				/* Stop looking */
				break;
			}

			/* Break when done */
			if (okay) break;
		}

		/* All scrolls are white */
		scroll_col[i] = TERM_WHITE;
	}


	/* Hack -- Use the "complex" RNG */
	Rand_quick = FALSE;

	/* Analyze every object */
	for (i = 1; i < max_k_idx; i++)
	{
		object_kind *k_ptr = &k_info[i];

		/* Skip "empty" objects */
		if (!k_ptr->name) continue;

		/* Extract "flavor" (if any) */
		k_ptr->flavor = object_flavor(i);

		/* No flavor yields aware */
		if (!k_ptr->flavor) k_ptr->aware = TRUE;

		/* Check for "easily known" */
		k_ptr->easy_know = object_easy_know(i);
	}
}


/*
 * Print a char "c" into a string "t", as if by sprintf(t, "%c", c),
 * and return a pointer to the terminator (t + 1).
 */
static char *object_desc_chr(char *t, char c)
{
	/* Copy the char */
	*t++ = c;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Print a string "s" into a string "t", as if by strcpy(t, s),
 * and return a pointer to the terminator.
 */
static char *object_desc_str(char *t, cptr s)
{
	/* Copy the string */
	while (*s) *t++ = *s++;

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}



/*
 * Print an unsigned number "n" into a string "t", as if by
 * sprintf(t, "%u", n), and return a pointer to the terminator.
 */
static char *object_desc_num(char *t, uint n)
{
	uint p;

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}




#ifdef JP
/*
 * ���ܸ�θĿ�ɽ���롼����
 *��cmd1.c ��ή�Ѥ��뤿��� object_desc_japanese �����ư��������
 */

extern char *object_desc_kosuu(char *t, object_type *o_ptr)
{
    t = object_desc_num(t, o_ptr->number);

    switch (o_ptr->tval)
    {
      case TV_BOLT:
      case TV_ARROW:
      case TV_POLEARM:
      case TV_STAFF:
      case TV_WAND:
      case TV_ROD:
      case TV_DIGGING:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_SCROLL:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_POTION:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case  TV_LIFE_BOOK:
      case  TV_SORCERY_BOOK:
      case  TV_NATURE_BOOK:
      case  TV_CHAOS_BOOK:
      case  TV_DEATH_BOOK:
      case  TV_TRUMP_BOOK:
      case  TV_ARCANE_BOOK:
      case  TV_ENCHANT_BOOK:
      case  TV_DAEMON_BOOK:
      case  TV_MUSIC_BOOK:
      case  TV_HISSATSU_BOOK:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_SOFT_ARMOR:
      case TV_HARD_ARMOR:
      case TV_DRAG_ARMOR:
      case TV_CLOAK:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_SWORD:
      case TV_HAFTED:
      case TV_BOW:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
      case TV_BOOTS:
      {
	  t = object_desc_str(t, "­");
	  break;
      }
      case TV_CARD:
      {
	  t = object_desc_str(t, "��");
	  break;
      }
	    /* ���٤�� by ita */
      case TV_FOOD:
      {
	  if(o_ptr->sval == SV_FOOD_JERKY)
          {
	      t = object_desc_str(t, "�ڤ�");
	      break;
	  }
      }
      default:
      {
	  if (o_ptr->number < 10)
	  {
	      t = object_desc_str(t, "��");
	  }
	  else
	  {
	      t = object_desc_str(t, "��");
	  }
	  break;
      }
  }
  return (t);		       
}
#endif

/*
 * Print an signed number "v" into a string "t", as if by
 * sprintf(t, "%+d", n), and return a pointer to the terminator.
 * Note that we always print a sign, either "+" or "-".
 */
static char *object_desc_int(char *t, sint v)
{
	uint p, n;

	/* Negative */
	if (v < 0)
	{
		/* Take the absolute value */
		n = 0 - v;

		/* Use a "minus" sign */
		*t++ = '-';
	}

	/* Positive (or zero) */
	else
	{
		/* Use the actual number */
		n = v;

		/* Use a "plus" sign */
		*t++ = '+';
	}

	/* Find "size" of "n" */
	for (p = 1; n >= p * 10; p = p * 10) /* loop */;

	/* Dump each digit */
	while (p >= 1)
	{
		/* Dump the digit */
		*t++ = '0' + n / p;

		/* Remove the digit */
		n = n % p;

		/* Process next digit */
		p = p / 10;
	}

	/* Terminate */
	*t = '\0';

	/* Result */
	return (t);
}


/*
 * Creates a description of the item "o_ptr", and stores it in "out_val".
 *
 * One can choose the "verbosity" of the description, including whether
 * or not the "number" of items should be described, and how much detail
 * should be used when describing the item.
 *
 * The given "buf" must be MAX_NLEN chars long to hold the longest possible
 * description, which can get pretty long, including incriptions, such as:
 * "no more Maces of Disruption (Defender) (+10,+10) [+5] (+3 to stealth)".
 * Note that the inscription will be clipped to keep the total description
 * under MAX_NLEN-1 chars (plus a terminator).
 *
 * Note the use of "object_desc_num()" and "object_desc_int()" as hyper-efficient,
 * portable, versions of some common "sprintf()" commands.
 *
 * Note that all ego-items (when known) append an "Ego-Item Name", unless
 * the item is also an artifact, which should NEVER happen.
 *
 * Note that all artifacts (when known) append an "Artifact Name", so we
 * have special processing for "Specials" (artifact Lites, Rings, Amulets).
 * The "Specials" never use "modifiers" if they are "known", since they
 * have special "descriptions", such as "The Necklace of the Dwarves".
 *
 * Special Lite's use the "k_info" base-name (Phial, Star, or Arkenstone),
 * plus the artifact name, just like any other artifact, if known.
 *
 * Special Ring's and Amulet's, if not "aware", use the same code as normal
 * rings and amulets, and if "aware", use the "k_info" base-name (Ring or
 * Amulet or Necklace).  They will NEVER "append" the "k_info" name.  But,
 * they will append the artifact name, just like any artifact, if known.
 *
 * Hack -- Display "The One Ring" as "a Plain Gold Ring" until aware.
 *
 * If "pref" then a "numeric" prefix will be pre-pended.
 *
 * Mode:
 *   0 -- The Cloak of Death
 *   1 -- The Cloak of Death [1,+3]
 *   2 -- The Cloak of Death [1,+3] (+2 to Stealth)
 *   3 -- The Cloak of Death [1,+3] (+2 to Stealth) {nifty}
 */
void object_desc(char *buf, object_type *o_ptr, int pref, int mode)
{
	cptr            basenm, modstr;
	int             power, indexx, i;

	bool            aware = FALSE;
	bool            known = FALSE;

	bool            append_name = FALSE;

	bool            show_weapon = FALSE;
	bool            show_armour = FALSE;

	cptr            s, u;
	char            *t;

	char            p1 = '(', p2 = ')';
	char            b1 = '[', b2 = ']';
	char            c1 = '{', c2 = '}';

	char            tmp_val[MAX_NLEN+160];
	char            tmp_val2[MAX_NLEN+10];

	u32b            f1, f2, f3;

	object_type	*bow_ptr;


	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	monster_race *r_ptr = &r_info[o_ptr->pval];

	/* Extract some flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* See if the object is "aware" */
	if (object_aware_p(o_ptr) || (o_ptr->ident & IDENT_MENTAL)) aware = TRUE;

	/* See if the object is "known" */
	if (object_known_p(o_ptr)) known = TRUE;

	/* Hack -- Extract the sub-type "indexx" */
	indexx = o_ptr->sval;

	/* Extract default "base" string */
	basenm = get_object_name(o_ptr);

	/* Assume no "modifier" string */
	modstr = "";


	/* Analyze the object */
	switch (o_ptr->tval)
	{
		/* Some objects are easy to describe */
		case TV_SKELETON:
		case TV_BOTTLE:
		case TV_JUNK:
		case TV_SPIKE:
		case TV_FLASK:
		case TV_CHEST:
		case TV_WHISTLE:
		{
			break;
		}

		case TV_CAPTURE:
		{
			if (known)
			{
				if (!o_ptr->pval)
				{
#ifdef JP
					modstr = "�ʶ���";
#else
					modstr = " (empty)";
#endif
				}
				else
				{
#ifdef JP
					sprintf(tmp_val2, "��%s��",r_name + r_ptr->name);
					modstr = tmp_val2;
#else
					cptr t = r_name + r_ptr->name;

					if (!(r_ptr->flags1 & RF1_UNIQUE))
					{
						sprintf(tmp_val2, " (%s%s)", (is_a_vowel(*t) ? "an " : "a "), t);

						modstr = tmp_val2;
					}
					else
					{
						sprintf(tmp_val2, "(%s)", t);

						modstr = t;
					}
#endif
				}
			}
			break;
		}

		/* Figurines/Statues */
		case TV_FIGURINE:
		case TV_STATUE:
		{
#ifdef JP
			modstr = r_name + r_ptr->name;
#else
			cptr t = r_name + r_ptr->name;

			if (!(r_ptr->flags1 & RF1_UNIQUE))
			{
				sprintf(tmp_val2, "%s%s", (is_a_vowel(*t) ? "an " : "a "), t);

				modstr = tmp_val2;
			}
			else
			{
				modstr = t;
			}
#endif


			break;
		}

		/* Corpses */
		case TV_CORPSE:
		{
			modstr = r_name + r_ptr->name;


#ifdef JP
			sprintf(tmp_val2, "& #%s", basenm + 2);
#else
			if (r_ptr->flags1 & RF1_UNIQUE)
			{
				sprintf(tmp_val2, "%s %s", basenm, "of #");
			}
			else
			{
				sprintf(tmp_val2, "& # %s", basenm + 2);
			}
#endif


			basenm = tmp_val2;
			break;
		}

		/* Missiles/ Bows/ Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_BOW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			show_weapon = TRUE;
			break;
		}


		/* Armour */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_SHIELD:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			show_armour = TRUE;
			break;
		}


		/* Lites (including a few "Specials") */
		case TV_LITE:
		{
			break;
		}

		/* Amulets (including a few "Specials") */
		case TV_AMULET:
		{
			/* Known artifacts */
			if (artifact_p(o_ptr) && aware) break;

			if ((k_ptr->flags3 & TR3_INSTA_ART) && aware) break;

			/* Color the object */
#ifdef JP
			modstr = J_amulet_adj[indexx];
#else
			modstr = amulet_adj[indexx];
#endif

			if (aware) append_name = TRUE;

			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware ? "& %�Υ��ߥ��å�" : "& ���ߥ��å�";
			else
				basenm = aware ? "& #%�Υ��ߥ��å�" : "& #���ߥ��å�";
#else
				basenm = "& Amulet~";
			else
				basenm = aware ? "& # Amulet~" : "& # Amulet~";
#endif

			break;
		}

		/* Rings (including a few "Specials") */
		case TV_RING:
		{
			/* Known artifacts */
			if (artifact_p(o_ptr) && aware) break;

			if ((k_ptr->flags3 & TR3_INSTA_ART) && aware) break;

			/* Color the object */
#ifdef JP
			modstr = J_ring_adj[indexx];
#else
			modstr = ring_adj[indexx];
#endif

			if (aware) append_name = TRUE;

			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware ? "& %�λ���" : "& ����";
			else
				basenm = aware ? "& #%�λ���" : "& #����";
#else
				basenm = "& Ring~";
			else
				basenm = aware ? "& # Ring~" : "& # Ring~";
#endif


			/* Hack -- The One Ring */
#ifdef JP
			if (!aware && (o_ptr->sval == SV_RING_POWER)) modstr = "��̵����";
			if (!aware && (o_ptr->sval == SV_RING_AHO)) modstr = "��ͭ����";
#else
			if (!aware && (o_ptr->sval == SV_RING_POWER)) modstr = "Plain Gold";
			if (!aware && (o_ptr->sval == SV_RING_AHO)) modstr = "Plain Goldarn";
#endif
			if (!k_ptr->to_h && !k_ptr->to_d && (o_ptr->to_h || o_ptr->to_d)) show_weapon = TRUE;

			break;
		}

		case TV_CARD:
		{
			break;
		}

		case TV_STAFF:
		{
			/* Color the object */
#ifdef JP
			modstr = J_staff_adj[indexx];
#else
			modstr = staff_adj[indexx];
#endif

			if (aware) append_name = TRUE;
			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware ? "& %�ξ�" : "& ��";
			else
				basenm = aware ? "& #%�ξ�" : "& #��";
#else
				basenm = "& Staff~";
			else
				basenm = aware ? "& # Staff~" : "& # Staff~";
#endif

			break;
		}

		case TV_WAND:
		{
			/* Color the object */
#ifdef JP
			modstr = J_wand_adj[indexx];
#else
			modstr = wand_adj[indexx];
#endif

			if (aware) append_name = TRUE;
			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware? "& %����ˡ��":"& ��ˡ��";
			else
				basenm = aware ? "& #%����ˡ��" : "& #��ˡ��";
#else
				basenm = "& Wand~";
			else
				basenm = aware ? "& # Wand~" : "& # Wand~";
#endif

			break;
		}

		case TV_ROD:
		{
			/* Color the object */
#ifdef JP
			modstr = J_rod_adj[indexx];
#else
			modstr = rod_adj[indexx];
#endif

			if (aware) append_name = TRUE;
			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware? "& %�Υ�å�":"& ��å�";
			else
				basenm = aware ? "& #%�Υ�å�" : "& #��å�";
#else
				basenm = "& Rod~";
			else
				basenm = aware ? "& # Rod~" : "& # Rod~";
#endif

			break;
		}

		case TV_SCROLL:
		{
			/* Color the object */
			modstr = scroll_adj[indexx];
			if (aware) append_name = TRUE;
			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware ? "& %�δ�ʪ" : "& ��ʪ";
			else
				basenm = aware ? "& ��#�פȽ񤫤줿%�δ�ʪ" : "& ��#�פȽ񤫤줿��ʪ";
#else
				basenm = "& Scroll~";
			else
				basenm = aware ? "& Scroll~ titled \"#\"" : "& Scroll~ titled \"#\"";
#endif

			break;
		}

		case TV_POTION:
		{
			/* Color the object */
#ifdef JP
			modstr = J_potion_adj[indexx];
#else
			modstr = potion_adj[indexx];
#endif

			if (aware) append_name = TRUE;
			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware ? "& %����" : "& ��";
			else
				basenm = aware ? "& #%����" : "& #��";
#else
				basenm = "& Potion~";
			else
				basenm = aware ? "& # Potion~" : "& # Potion~";
#endif

			break;
		}

		case TV_FOOD:
		{
			/* Ordinary food is "boring" */
			if (o_ptr->sval >= SV_FOOD_MIN_FOOD) break;

			/* Color the object */
#ifdef JP
			modstr = J_food_adj[indexx];
#else
			modstr = food_adj[indexx];
#endif

			if (aware) append_name = TRUE;
			if (((plain_descriptions) && (aware)) || o_ptr->ident & IDENT_STOREB)
#ifdef JP
				basenm = aware ? "& %�Υ��Υ�" : "& ���Υ�";
			else
				basenm = aware ? "& #%�Υ��Υ�" : "& #���Υ�";
#else
				basenm = "& Mushroom~";
			else
				basenm = aware ? "& # Mushroom~" : "& # Mushroom~";
#endif

			break;
		}

		case TV_PARCHEMENT:
		{
			modstr = basenm;
#ifdef JP
                        basenm = "& ����� - #";
#else
                        basenm = "& Parchement~ - #";
#endif
			break;
		}

		/* Magic Books */
		case TV_LIFE_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ��̿����ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Life Magic #";
			else
				basenm = "& Life Spellbook~ #";
#endif

			break;
		}

		case TV_SORCERY_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ��Ѥ���ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Sorcery #";
			else
				basenm = "& Sorcery Spellbook~ #";
#endif

			break;
		}

		case TV_NATURE_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ��������ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Nature Magic #";
			else
				basenm = "& Nature Spellbook~ #";
#endif

			break;
		}

		case TV_CHAOS_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ����������ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Chaos Magic #";
			else
				basenm = "& Chaos Spellbook~ #";
#endif

			break;
		}

		case TV_DEATH_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& �Ź�����ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Death Magic #";
			else
				basenm = "& Death Spellbook~ #";
#endif

			break;
		}

		case TV_TRUMP_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& �ȥ��פ���ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Trump Magic #";
			else
				basenm = "& Trump Spellbook~ #";
#endif

			break;
		}

		case TV_ARCANE_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ��Ѥ���ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Arcane Magic #";
			else
				basenm = "& Arcane Spellbook~ #";
#endif

			break;
		}
		case TV_ENCHANT_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ������ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Craft Magic #";
			else
				basenm = "& Craft Spellbook~ #";
#endif

			break;
		}
		case TV_DAEMON_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& �������ˡ��#";
#else
			if (mp_ptr->spell_book == TV_LIFE_BOOK)
				basenm = "& Book~ of Daemon Magic #";
			else
				basenm = "& Daemon Spellbook~ #";
#endif

			break;
		}
		case TV_MUSIC_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& �ν�#";
#else
				basenm = "& Song Book~ #";
#endif

			break;
		}
		case TV_HISSATSU_BOOK:
		{
			modstr = basenm;
#ifdef JP
				basenm = "& ��ݤν�#";
#else
				basenm = "& Book~ of Kendo #";
#endif

			break;
		}



		/* Hack -- Gold/Gems */
		case TV_GOLD:
		{
			strcpy(buf, basenm);
			return;
		}

		/* Used in the "inventory" routine */
		default:
		{
#ifdef JP
                        strcpy(buf, "(�ʤ�)");
#else
			strcpy(buf, "(nothing)");
#endif

			return;
		}
	}


	/* Start dumping the result */
	t = tmp_val;

#ifdef JP
	if (basenm[0] == '&')
		s = basenm + 2;
	else
		s = basenm;

	/* No prefix */
	if (!pref)
	{
		/* Nothing */
	}
	else if (o_ptr->number > 1)
	{
		if ( old_way_of_kaz == FALSE ){
			t = object_desc_num(t, o_ptr->number);
			if (o_ptr->number > 9)
				t = object_desc_str(t, "�Ĥ� ");
			else
				t = object_desc_str(t, "�Ĥ� ");
		}
		else
		{
			t = object_desc_kosuu(t,o_ptr);
			t = object_desc_str(t, "�� ");
		}
	}

	/* �Ѹ�ξ�祢���ƥ��ե����Ȥ� The ���դ��Τ�ʬ���뤬
	 * ���ܸ�Ǥ�ʬ����ʤ��Τǥޡ�����Ĥ��� 
	 */
	if (known && artifact_p(o_ptr)) t = object_desc_str(t, "��");
	else if (known && o_ptr->art_name) t = object_desc_str(t, "��");

#else

	/* The object "expects" a "number" */
	if (basenm[0] == '&')
	{
		/* Skip the ampersand (and space) */
		s = basenm + 2;

		/* No prefix */
		if (!pref)
		{
			/* Nothing */
		}

		/* Hack -- None left */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Extract the number */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && (artifact_p(o_ptr) || o_ptr->art_name))
		{
			t = object_desc_str(t, "The ");
		}

		/* Unique corpses are unique */
		else if ((o_ptr->tval == TV_CORPSE) && (r_ptr->flags1 & RF1_UNIQUE))
		{
			t = object_desc_str(t, "The ");
		}

		/* A single one, with a vowel in the modifier */
		else if ((*s == '#') && (is_a_vowel(modstr[0])))
		{
			t = object_desc_str(t, "an ");
		}

		/* A single one, with a vowel */
		else if (is_a_vowel(*s))
		{
			t = object_desc_str(t, "an ");
		}

		/* A single one, without a vowel */
		else
		{
			t = object_desc_str(t, "a ");
		}
	}

	/* Hack -- objects that "never" take an article */
	else
	{
		/* No ampersand */
		s = basenm;

		/* No pref */
		if (!pref)
		{
			/* Nothing */
		}

		/* Hack -- all gone */
		else if (o_ptr->number <= 0)
		{
			t = object_desc_str(t, "no more ");
		}

		/* Prefix a number if required */
		else if (o_ptr->number > 1)
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_chr(t, ' ');
		}

		/* Hack -- The only one of its kind */
		else if (known && (artifact_p(o_ptr) || o_ptr->art_name))
		{
			t = object_desc_str(t, "The ");
		}

		/* Hack -- single items get no prefix */
		else
		{
			/* Nothing */
		}
	}
#endif

	/* Paranoia -- skip illegal tildes */
	/* while (*s == '~') s++; */

#ifdef JP
	if ((o_ptr->tval > TV_CAPTURE) && o_ptr->xtra3)
	{
		t = object_desc_str(t,format("�����%s��",player_name));
	}

        /* ����Υ����ƥࡢ̾�Τ��륢���ƥ��̾�����ղä��� */
        if (known) {
                /* �����ࡦ�����ƥ��ե����� */
		if (o_ptr->art_name)
		{       char temp[256];
		        strcpy(temp, quark_str(o_ptr->art_name));
                        /* '��' ����Ϥޤ�ʤ�����Υ����ƥ��̾���Ϻǽ���ղä��� */
                        /* �Ѹ��ǤΥ����֥ե����뤫���褿 'of XXX' ��,��XXX�Ρפ�ɽ������ */
			if ( strncmp( temp , "of ",3)==0 ) {t=object_desc_str(t,&temp[3]);t=object_desc_str(t,"��");}
			else 
                        if ( strncmp( temp , "��" , 2 ) != 0 && temp[0]!='\'')
                                t=object_desc_str(t,  temp);
		}
                /* ����Υ����ƥ� */
                else if (o_ptr->name1) {
                        artifact_type *a_ptr = &a_info[o_ptr->name1];
                        /* '��' ����Ϥޤ�ʤ�����Υ����ƥ��̾���Ϻǽ���ղä��� */
                        if ( strncmp( (a_name + a_ptr->name), "��" , 2) != 0){
                                t=object_desc_str(t, (a_name + a_ptr->name));
                        }
                }
                /* ̾�Τ��륢���ƥ� */
                else if (o_ptr->name2) {
                        ego_item_type *e_ptr = &e_info[o_ptr->name2];
                        t=object_desc_str(t, (e_name + e_ptr->name));
                }
        }
#endif
	/* Copy the string */
	for (; *s; s++)
	{
		/* Pluralizer */
#ifdef JP
                if (*s == '#')
#else
		if (*s == '~')
		{
			/* Add a plural if needed */
			if (pref && o_ptr->number != 1)
			{
				char k = t[-1];

				/* XXX XXX XXX Mega-Hack */

				/* Hack -- "Cutlass-es" and "Torch-es" */
				if ((k == 's') || (k == 'h')) *t++ = 'e';

				/* Add an 's' */
				*t++ = 's';
			}
		}

		/* Modifier */
		else if (*s == '#')
#endif

		{
			/* Insert the modifier */
			for (u = modstr; *u; u++) *t++ = *u;
		}

#ifdef JP
		else if (*s == '%')
		  /* saigo �����ꡣ��ǽ���ղ����� by ita*/
		{
		  modstr=get_object_name(o_ptr);
		  for (u = modstr; *u; u++) *t++ = *u;
		}
#endif
		/* Normal */
		else
		{
			/* Copy */
			*t++ = *s;
		}
	}

	/* Terminate */
	*t = '\0';


	/* Append the "kind name" to the "base name" */
#ifndef JP
	if (append_name)
	{
		t = object_desc_str(t, " of ");
		t = object_desc_str(t, get_object_name(o_ptr));
	}
#endif


#ifdef JP
        /* '��'����Ϥޤ�����Υ����ƥ��̾���ϺǸ���ղä��� */
        if (known) {
		/* �����ॢ���ƥ��ե����Ȥ�̾���ϥ����֥ե�����˵�Ͽ
                   �����Τǡ��Ѹ��Ǥ�̾���⤽��餷���Ѵ����� */
		if (o_ptr->art_name)
		{       char temp[256];int itemp;
		        strcpy(temp, quark_str(o_ptr->art_name));
			/* MEGA HACK by ita*/
                                if ( strncmp( temp , "��" , 2 ) == 0 ) t=object_desc_str(t,  temp);else
			 if( temp[0]=='\'' ) { itemp=strlen(temp);temp[itemp-1]=0; 
			 t=object_desc_str(t,"��");
			 t=object_desc_str(t,&temp[1]);
			 t=object_desc_str(t,"��");}
                                
		}
                else if (o_ptr->name1) {
                                artifact_type *a_ptr = &a_info[o_ptr->name1];
                                if ( strncmp( (a_name + a_ptr->name) , "��" , 2 ) == 0 ){
                                        t=object_desc_str(t, (a_name + a_ptr->name));
                                }
                }
		else if (o_ptr->inscription)
		{
			cptr str = quark_str(o_ptr->inscription);

			while(*str)
			{
				if (iskanji(*str))
				{
					str += 2;
					continue;
				}
				if (*str == '#') break;
				str++;
			}
			if (*str)
			{
				/* Find the '#' */
				cptr str = strchr(quark_str(o_ptr->inscription), '#');

				/* Add the false name */
				t=object_desc_str(t,"��");
				t = object_desc_str(t, &str[1]);
				t=object_desc_str(t,"��");
			}
		}
        }
#else
	if ((o_ptr->tval > TV_CAPTURE) && o_ptr->xtra3)
	{
		t = object_desc_str(t,format(" of %s the smith",player_name));
	}

	/* Hack -- Append "Artifact" or "Special" names */
	if (known)
	{
		if (o_ptr->inscription && strchr(quark_str(o_ptr->inscription), '#'))
		{
			/* Find the '#' */
			cptr str = strchr(quark_str(o_ptr->inscription), '#');

			/* Add the false name */
			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, &str[1]);
		}

		/* Is it a new random artifact ? */
		else if (o_ptr->art_name)
		{
			t = object_desc_chr(t, ' ');

			t = object_desc_str(t, quark_str(o_ptr->art_name));
		}

		/* Grab any artifact name */
		else if (o_ptr->name1)
		{
			artifact_type *a_ptr = &a_info[o_ptr->name1];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, (a_name + a_ptr->name));
		}

		/* Grab any ego-item name */
		else if (o_ptr->name2)
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			t = object_desc_chr(t, ' ');
			t = object_desc_str(t, (e_name + e_ptr->name));
		}
	}
#endif



	/* No more details wanted */
	if (mode < 1) goto copyback;

	/* Hack -- Chests must be described in detail */
	if (o_ptr->tval == TV_CHEST)
	{
		/* Not searched yet */
		if (!known)
		{
			/* Nothing */
		}

		/* May be "empty" */
		else if (!o_ptr->pval)
		{
#ifdef JP
t = object_desc_str(t, "(��)");
#else
			t = object_desc_str(t, " (empty)");
#endif

		}

		/* May be "disarmed" */
		else if (o_ptr->pval < 0)
		{
			if (chest_traps[0 - o_ptr->pval])
			{
#ifdef JP
t = object_desc_str(t, "(�����)");
#else
				t = object_desc_str(t, " (disarmed)");
#endif

			}
			else
			{
#ifdef JP
t = object_desc_str(t, "(��ܾ�)");
#else
				t = object_desc_str(t, " (unlocked)");
#endif

			}
		}

		/* Describe the traps, if any */
		else
		{
			/* Describe the traps */
			switch (chest_traps[o_ptr->pval])
			{
				case 0:
				{
#ifdef JP
t = object_desc_str(t, "(�ܾ�)");
#else
					t = object_desc_str(t, " (Locked)");
#endif

					break;
				}
				case CHEST_LOSE_STR:
				{
#ifdef JP
t = object_desc_str(t, "(�ǿ�)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif

					break;
				}
				case CHEST_LOSE_CON:
				{
#ifdef JP
t = object_desc_str(t, "(�ǿ�)");
#else
					t = object_desc_str(t, " (Poison Needle)");
#endif

					break;
				}
				case CHEST_POISON:
				{
#ifdef JP
t = object_desc_str(t, "(�������ȥ�å�)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif

					break;
				}
				case CHEST_PARALYZE:
				{
#ifdef JP
t = object_desc_str(t, "(�������ȥ�å�)");
#else
					t = object_desc_str(t, " (Gas Trap)");
#endif

					break;
				}
				case CHEST_EXPLODE:
				{
#ifdef JP
t = object_desc_str(t, "(��ȯ����)");
#else
					t = object_desc_str(t, " (Explosion Device)");
#endif

					break;
				}
				case CHEST_SUMMON:
				case CHEST_BIRD_STORM:
				case CHEST_E_SUMMON:
				case CHEST_H_SUMMON:
				{
#ifdef JP
t = object_desc_str(t, "(�����Υ롼��)");
#else
					t = object_desc_str(t, " (Summoning Runes)");
#endif

					break;
				}
				case CHEST_RUNES_OF_EVIL:
				{
#ifdef JP
t = object_desc_str(t, "(�ٰ��ʥ롼��)");
#else
					t = object_desc_str(t, " (Gleaming Black Runes)");
#endif

					break;
				}
				default:
				{
#ifdef JP
t = object_desc_str(t, "(�ޥ�����ȥ�å�)");
#else
					t = object_desc_str(t, " (Multiple Traps)");
#endif

					break;
				}
			}
		}
	}


	/* Display the item like a weapon */
	if (f3 & (TR3_SHOW_MODS)) show_weapon = TRUE;

	/* Display the item like a weapon */
	if (o_ptr->to_h && o_ptr->to_d) show_weapon = TRUE;

	/* Display the item like armour */
	if (o_ptr->ac) show_armour = TRUE;


	/* Dump base weapon info */
	switch (o_ptr->tval)
	{
		/* Missiles and Weapons */
		case TV_SHOT:
		case TV_BOLT:
		case TV_ARROW:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:

		/* Append a "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, o_ptr->dd);
		t = object_desc_chr(t, 'd');
		t = object_desc_num(t, o_ptr->ds);
		t = object_desc_chr(t, p2);

		/* All done */
		break;


		/* Bows get a special "damage string" */
		case TV_BOW:

		/* Mega-Hack -- Extract the "base power" */
		power = (o_ptr->sval % 10);

		/* Apply the "Extra Might" flag */
		if (f3 & (TR3_XTRA_MIGHT)) power++;

		/* Append a special "damage" string */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_chr(t, 'x');
		t = object_desc_num(t, power);
		t = object_desc_chr(t, p2);

		/* All done */
		break;
	}


	/* Add the weapon bonuses */
	if (known)
	{
		/* Show the tohit/todam on request */
		if (show_weapon)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}

		/* Show the tohit if needed */
		else if (o_ptr->to_h)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_h);
			t = object_desc_chr(t, p2);
		}

		/* Show the todam if needed */
		else if (o_ptr->to_d)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, p1);
			t = object_desc_int(t, o_ptr->to_d);
			t = object_desc_chr(t, p2);
		}
	}

	bow_ptr = &inventory[INVEN_BOW];

	/* if have a firing weapon + ammo matches bow*/
	if (bow_ptr->k_idx &&
	    (((bow_ptr->sval == SV_SLING) && (o_ptr->tval == TV_SHOT)) ||
	     (((bow_ptr->sval == SV_SHORT_BOW) ||
	       (bow_ptr->sval == SV_LONG_BOW) ||
	       (bow_ptr->sval == SV_NAMAKE_BOW)) && (o_ptr->tval == TV_ARROW)) ||
	     (((bow_ptr->sval == SV_LIGHT_XBOW) ||
	       (bow_ptr->sval == SV_HEAVY_XBOW)) && (o_ptr->tval == TV_BOLT))))
	{
		int avgdam = 10;
		int tmul,energy_fire;

		avgdam = o_ptr->dd * (o_ptr->ds + 1) * 10 / 2;

		/* See if the bow is "known" - then set damage bonus*/
		if (object_known_p(bow_ptr))
		{
			avgdam += (bow_ptr->to_d * 10);
		}

		/* effect of ammo */
		if (known) avgdam += (o_ptr->to_d * 10);

		/* Stop compiler warnings */
		energy_fire = 100;
		tmul = 1;

		/* Analyze the launcher */
		switch (bow_ptr->sval)
		{
			/* Sling and ammo */
			case SV_SLING:
			{
				tmul = 2;
				energy_fire = 8000;
				break;
			}

			/* Short Bow and Arrow */
			case SV_SHORT_BOW:
			{
				tmul = 2;
				energy_fire = 10000;
				break;
			}

			/* Long Bow and Arrow */
			case SV_LONG_BOW:
			{
				tmul = 3;
				energy_fire = 10000;
				break;
			}

			case SV_NAMAKE_BOW:
			{
				tmul = 3;
				energy_fire = 7777;
				break;
			}

			/* Light Crossbow and Bolt */
			case SV_LIGHT_XBOW:
			{
				tmul = 3;
				energy_fire = 12000;
				break;
			}

			/* Heavy Crossbow and Bolt */
			case SV_HEAVY_XBOW:
			{
				tmul = 4;
				energy_fire = 13333;
				break;
			}
		}

		/* Get extra "power" from "extra might" */
		if (p_ptr->xtra_might) tmul++;

		tmul = tmul * (100 + (int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);

		/* launcher multiplier */
		avgdam *= tmul;
		avgdam /= (100 * 10);
		if (avgdam < 0) avgdam = 0;

		/* display (shot damage/ avg damage) */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);
		t = object_desc_num(t, avgdam);
		t = object_desc_chr(t, '/');

		tmul = p_ptr->num_fire;
		if (tmul == 0)
		{
			t = object_desc_chr(t, '0');
		}
		else
		{
			/* calc effects of energy */
			avgdam *= (p_ptr->num_fire * 100);

			avgdam /= energy_fire;

			t = object_desc_num(t, avgdam);
		}

		t = object_desc_chr(t, p2);
	}


	/* Add the armor bonuses */
	if (known)
	{
		/* Show the armor class info */
		if (show_armour)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_num(t, o_ptr->ac);
			t = object_desc_chr(t, ',');
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}

		/* No base armor, but does increase armor */
		else if (o_ptr->to_a)
		{
			t = object_desc_chr(t, ' ');
			t = object_desc_chr(t, b1);
			t = object_desc_int(t, o_ptr->to_a);
			t = object_desc_chr(t, b2);
		}
	}

	/* Hack -- always show base armor */
	else if (show_armour)
	{
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, b1);
		t = object_desc_num(t, o_ptr->ac);
		t = object_desc_chr(t, b2);
	}


	/* No more details wanted */
	if (mode < 2) goto copyback;


	/*
	 * Hack -- Wands and Staffs have charges.  Make certain how many charges
	 * a stack of staffs really has is clear. -LM-
	 */
	if (known &&
	    ((o_ptr->tval == TV_STAFF) ||
	     (o_ptr->tval == TV_WAND)))
	{
		/* Dump " (N charges)" */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);

		/* Clear explaination for staffs. */
		if ((o_ptr->tval == TV_STAFF) && (o_ptr->number > 1))
		{
			t = object_desc_num(t, o_ptr->number);
			t = object_desc_str(t, "x ");
		}
		t = object_desc_num(t, o_ptr->pval);
#ifdef JP
                t = object_desc_str(t, "��ʬ");
#else
		t = object_desc_str(t, " charge");

		if (o_ptr->pval != 1)
		{
			t = object_desc_chr(t, 's');
		}
#endif


		t = object_desc_chr(t, p2);
	}
	/* Hack -- Rods have a "charging" indicator.  Now that stacks of rods may
	 * be in any state of charge or discharge, this now includes a number. -LM-
	 */
	else if (known && (o_ptr->tval == TV_ROD))
	{
		/* Hack -- Dump " (# charging)" if relevant */
		if (o_ptr->timeout)
		{
			/* Stacks of rods display an exact count of charging rods. */
			if (o_ptr->number > 1)
			{
				/* Paranoia. */
				if (k_ptr->pval == 0) k_ptr->pval = 1;

				/* Find out how many rods are charging, by dividing
				 * current timeout by each rod's maximum timeout.
				 * Ensure that any remainder is rounded up.  Display
			 	 * very discharged stacks as merely fully discharged.
			 	 */
				power = (o_ptr->timeout + (k_ptr->pval - 1)) / k_ptr->pval;
				if (power > o_ptr->number) power = o_ptr->number;

				/* Display prettily. */
				t = object_desc_str(t, " (");
				t = object_desc_num(t, power);
#ifdef JP
				t = object_desc_str(t, "�� ��Ŷ��)");
#else
				t = object_desc_str(t, " charging)");
#endif

			}

			/* "one Rod of Perception (1 charging)" would look tacky. */
			else
			{
#ifdef JP
t = object_desc_str(t, "(��Ŷ��)");
#else
				t = object_desc_str(t, " (charging)");
#endif

			}
		}
	}

	/* Dump "pval" flags for wearable items */
	if (known && (f1 & (TR1_PVAL_MASK)))
	{
		/* Start the display */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, p1);

		/* Dump the "pval" itself */
		t = object_desc_int(t, o_ptr->pval);

		/* Do not display the "pval" flags */
		if (f3 & (TR3_HIDE_TYPE))
		{
			/* Nothing */
		}

		/* Speed */
		else if (f1 & (TR1_SPEED))
		{
			/* Dump " to speed" */
#ifdef JP
t = object_desc_str(t, "��®");
#else
			t = object_desc_str(t, " to speed");
#endif

		}

		/* Attack speed */
		else if (f1 & (TR1_BLOWS))
		{
			/* Add " attack" */
#ifdef JP
t = object_desc_str(t, "����");
#else
			t = object_desc_str(t, " attack");

			/* Add "attacks" */
			if (ABS(o_ptr->pval) != 1) t = object_desc_chr(t, 's');
#endif

		}

		/* Stealth */
		else if (f1 & (TR1_STEALTH))
		{
			/* Dump " to stealth" */
#ifdef JP
t = object_desc_str(t, "��̩");
#else
			t = object_desc_str(t, " to stealth");
#endif

		}

		/* Search */
		else if (f1 & (TR1_SEARCH))
		{
			/* Dump " to searching" */
#ifdef JP
t = object_desc_str(t, "õ��");
#else
			t = object_desc_str(t, " to searching");
#endif

		}

		/* Infravision */
		else if (f1 & (TR1_INFRA))
		{
			/* Dump " to infravision" */
#ifdef JP
t = object_desc_str(t, "�ֳ�������");
#else
			t = object_desc_str(t, " to infravision");
#endif

		}

		/* Tunneling */
		else if (f1 & (TR1_TUNNEL))
		{
			/* Nothing */
		}

		/* Finish the display */
		t = object_desc_chr(t, p2);
	}

	/* Hack -- Process Lanterns/Torches */
	if (known && (o_ptr->tval == TV_LITE) && (!(artifact_p(o_ptr) || (o_ptr->sval == SV_LITE_FEANOR))))
	{
		/* Hack -- Turns of light for normal lites */
#ifdef JP
t = object_desc_str(t, "(");
#else
		t = object_desc_str(t, " (with ");
#endif

		if (o_ptr->name2 == EGO_LITE_LONG) t = object_desc_num(t, o_ptr->xtra4*2);
		else t = object_desc_num(t, o_ptr->xtra4);
#ifdef JP
t = object_desc_str(t, "������μ�̿)");
#else
		t = object_desc_str(t, " turns of light)");
#endif

	}


	/* Indicate charging objects, but not rods. */
	if (known && o_ptr->timeout && o_ptr->tval != TV_ROD)
	{
		/* Hack -- Dump " (charging)" if relevant */
#ifdef JP
t = object_desc_str(t, "(��Ŷ��)");
#else
		t = object_desc_str(t, " (charging)");
#endif

	}


	/* No more details wanted */
	if (mode < 3) goto copyback;


	/* No inscription yet */
	tmp_val2[0] = '\0';

	/* Use the standard inscription if available */
	if (o_ptr->inscription)
	{
		char *u = tmp_val2;

		strcpy(tmp_val2, quark_str(o_ptr->inscription));

		for (; *u && (*u != '#'); u++);

		*u = '\0';
	}

	/* Use the game-generated "feeling" otherwise, if available */
	else if (o_ptr->feeling)
	{
		strcpy(tmp_val2, game_inscriptions[o_ptr->feeling]);
	}

	/* Note "cursed" if the item is known to be cursed */
	else if (cursed_p(o_ptr) && (known || (o_ptr->ident & (IDENT_SENSE))))
	{
#ifdef JP
strcpy(tmp_val2, "�����Ƥ���");
#else
		strcpy(tmp_val2, "cursed");
#endif

	}
 
	/* Note "unidentified" if the item is unidentified */
	else if ((o_ptr->tval == TV_RING || o_ptr->tval == TV_AMULET
		  || o_ptr->tval == TV_LITE || o_ptr->tval == TV_FIGURINE)
		 && object_aware_p(o_ptr) && !object_known_p(o_ptr)
		 && !((o_ptr->ident)&IDENT_SENSE))
	{
#ifdef JP
strcpy(tmp_val2, "̤����");
#else
		strcpy(tmp_val2, "unidentified");
#endif
	}

	/* Mega-Hack -- note empty wands/staffs */
	else if (!known && (o_ptr->ident & (IDENT_EMPTY)))
	{
#ifdef JP
strcpy(tmp_val2, "��");
#else
		strcpy(tmp_val2, "empty");
#endif

	}

	/* Note "tried" if the object has been tested unsuccessfully */
	else if (!aware && object_tried_p(o_ptr))
	{
#ifdef JP
strcpy(tmp_val2, "̤Ƚ��");
#else
		strcpy(tmp_val2, "tried");
#endif

	}

	/* Note the discount, if any */
	else if (o_ptr->discount)
	{
		(void)object_desc_num(tmp_val2, o_ptr->discount);
#ifdef JP
strcat(tmp_val2, "%����");
#else
		strcat(tmp_val2, "% off");
#endif

	}

	/* Append the inscription, if any */
	if (tmp_val2[0])
	{
		/* Append the inscription */
		t = object_desc_chr(t, ' ');
		t = object_desc_chr(t, c1);
		t = object_desc_str(t, tmp_val2);
		t = object_desc_chr(t, c2);
	}

copyback:
	t = tmp_val;
	for (i = 0; i < MAX_NLEN-2; i++)
	{
#ifdef JP
		if (iskanji(*(t+i)))
		{
			*(buf+i) = *(t+i);
			i++;
		}
#endif
		*(buf+i) = *(t+i);
	}
	if (i == MAX_NLEN-2)
	{
#ifdef JP
		if (iskanji(*(t+i)))
			*(buf+i) = '\0';
		else
#endif
			*(buf+i) = *(t+i);
	}
	*(buf+MAX_NLEN-1) = '\0';
}

