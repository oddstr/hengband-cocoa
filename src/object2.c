/* File: object2.c */

/* Purpose: Object code, part 2 */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"

#include "kajitips.h"


/*
 * Excise a dungeon object from any stacks
 */
void excise_object_idx(int o_idx)
{
	object_type *j_ptr;

	s16b this_o_idx, next_o_idx = 0;

	s16b prev_o_idx = 0;


	/* Object */
	j_ptr = &o_list[o_idx];

	/* Monster */
	if (j_ptr->held_m_idx)
	{
		monster_type *m_ptr;

		/* Monster */
		m_ptr = &m_list[j_ptr->held_m_idx];

		/* Scan all objects in the grid */
		for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Done */
			if (this_o_idx == o_idx)
			{
				/* No previous */
				if (prev_o_idx == 0)
				{
					/* Remove from list */
					m_ptr->hold_o_idx = next_o_idx;
				}

				/* Real previous */
				else
				{
					object_type *k_ptr;

					/* Previous object */
					k_ptr = &o_list[prev_o_idx];

					/* Remove from list */
					k_ptr->next_o_idx = next_o_idx;
				}

				/* Forget next pointer */
				o_ptr->next_o_idx = 0;

				/* Done */
				break;
			}

			/* Save prev_o_idx */
			prev_o_idx = this_o_idx;
		}
	}

	/* Dungeon */
	else
	{
		cave_type *c_ptr;

		int y = j_ptr->iy;
		int x = j_ptr->ix;

		/* Grid */
		c_ptr = &cave[y][x];

		/* Scan all objects in the grid */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Done */
			if (this_o_idx == o_idx)
			{
				/* No previous */
				if (prev_o_idx == 0)
				{
					/* Remove from list */
					c_ptr->o_idx = next_o_idx;
				}

				/* Real previous */
				else
				{
					object_type *k_ptr;

					/* Previous object */
					k_ptr = &o_list[prev_o_idx];

					/* Remove from list */
					k_ptr->next_o_idx = next_o_idx;
				}

				/* Forget next pointer */
				o_ptr->next_o_idx = 0;

				/* Done */
				break;
			}

			/* Save prev_o_idx */
			prev_o_idx = this_o_idx;
		}
	}
}


/*
 * Delete a dungeon object
 *
 * Handle "stacks" of objects correctly.
 */
void delete_object_idx(int o_idx)
{
	object_type *j_ptr;

	/* Excise */
	excise_object_idx(o_idx);

	/* Object */
	j_ptr = &o_list[o_idx];

	/* Dungeon floor */
	if (!(j_ptr->held_m_idx))
	{
		int y, x;

		/* Location */
		y = j_ptr->iy;
		x = j_ptr->ix;

		/* Visual update */
		lite_spot(y, x);
	}

	/* Wipe the object */
	object_wipe(j_ptr);

	/* Count objects */
	o_cnt--;
}


/*
 * Deletes all objects at given location
 */
void delete_object(int y, int x)
{
	cave_type *c_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Refuse "illegal" locations */
	if (!in_bounds(y, x)) return;


	/* Grid */
	c_ptr = &cave[y][x];

	/* Scan all objects in the grid */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Wipe the object */
		object_wipe(o_ptr);

		/* Count objects */
		o_cnt--;
	}

	/* Objects are gone */
	c_ptr->o_idx = 0;

	/* Visual update */
	lite_spot(y, x);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
static void compact_objects_aux(int i1, int i2)
{
	int i;

	cave_type *c_ptr;

	object_type *o_ptr;


	/* Do nothing */
	if (i1 == i2) return;


	/* Repair objects */
	for (i = 1; i < o_max; i++)
	{
		/* Acquire object */
		o_ptr = &o_list[i];

		/* Skip "dead" objects */
		if (!o_ptr->k_idx) continue;

		/* Repair "next" pointers */
		if (o_ptr->next_o_idx == i1)
		{
			/* Repair */
			o_ptr->next_o_idx = i2;
		}
	}


	/* Acquire object */
	o_ptr = &o_list[i1];


	/* Monster */
	if (o_ptr->held_m_idx)
	{
		monster_type *m_ptr;

		/* Acquire monster */
		m_ptr = &m_list[o_ptr->held_m_idx];

		/* Repair monster */
		if (m_ptr->hold_o_idx == i1)
		{
			/* Repair */
			m_ptr->hold_o_idx = i2;
		}
	}

	/* Dungeon */
	else
	{
		int y, x;

		/* Acquire location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Acquire grid */
		c_ptr = &cave[y][x];

		/* Repair grid */
		if (c_ptr->o_idx == i1)
		{
			/* Repair */
			c_ptr->o_idx = i2;
		}
	}


	/* Structure copy */
	o_list[i2] = o_list[i1];

	/* Wipe the hole */
	object_wipe(o_ptr);
}


/*
 * Compact and Reorder the object list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" objects, we base the saving throw on a
 * combination of object level, distance from player, and current
 * "desperation".
 *
 * After "compacting" (if needed), we "reorder" the objects into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
void compact_objects(int size)
{
	int i, y, x, num, cnt;
	int cur_lev, cur_dis, chance;
	object_type *o_ptr;


	/* Compact */
	if (size)
	{
		/* Message */
#ifdef JP
		msg_print("�����ƥ����򰵽̤��Ƥ��ޤ�...");
#else
		msg_print("Compacting objects...");
#endif


		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
	}


	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Examine the objects */
		for (i = 1; i < o_max; i++)
		{
			o_ptr = &o_list[i];

			/* Skip dead objects */
			if (!o_ptr->k_idx) continue;

			/* Hack -- High level objects start out "immune" */
			if (get_object_level(o_ptr) > cur_lev) continue;

			/* Monster */
			if (o_ptr->held_m_idx)
			{
				monster_type *m_ptr;

				/* Acquire monster */
				m_ptr = &m_list[o_ptr->held_m_idx];

				/* Get the location */
				y = m_ptr->fy;
				x = m_ptr->fx;

				/* Monsters protect their objects */
				if (randint0(100) < 90) continue;
			}

			/* Dungeon */
			else
			{
				/* Get the location */
				y = o_ptr->iy;
				x = o_ptr->ix;
			}

			/* Nearby objects start out "immune" */
			if ((cur_dis > 0) && (distance(py, px, y, x) < cur_dis)) continue;

			/* Saving throw */
			chance = 90;

			/* Hack -- only compact artifacts in emergencies */
			if ((artifact_p(o_ptr) || o_ptr->art_name) &&
			    (cnt < 1000)) chance = 100;

			/* Apply the saving throw */
			if (randint0(100) < chance) continue;

			/* Delete the object */
			delete_object_idx(i);

			/* Count it */
			num++;
		}
	}


	/* Excise dead objects (backwards!) */
	for (i = o_max - 1; i >= 1; i--)
	{
		o_ptr = &o_list[i];

		/* Skip real objects */
		if (o_ptr->k_idx) continue;

		/* Move last object into open hole */
		compact_objects_aux(o_max - 1, i);

		/* Compress "o_max" */
		o_max--;
	}
}


/*
 * Delete all the items when player leaves the level
 *
 * Note -- we do NOT visually reflect these (irrelevant) changes
 *
 * Hack -- we clear the "c_ptr->o_idx" field for every grid,
 * and the "m_ptr->next_o_idx" field for every monster, since
 * we know we are clearing every object.  Technically, we only
 * clear those fields for grids/monsters containing objects,
 * and we clear it once for every such object.
 */
void wipe_o_list(void)
{
	int i;

	/* Delete the existing objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Mega-Hack -- preserve artifacts */
		if (!character_dungeon || preserve_mode)
		{
			/* Hack -- Preserve unknown artifacts */
			if (artifact_p(o_ptr) && !object_known_p(o_ptr))
			{
				/* Mega-Hack -- Preserve the artifact */
				a_info[o_ptr->name1].cur_num = 0;
			}
		}

		/* Monster */
		if (o_ptr->held_m_idx)
		{
			monster_type *m_ptr;

			/* Monster */
			m_ptr = &m_list[o_ptr->held_m_idx];

			/* Hack -- see above */
			m_ptr->hold_o_idx = 0;
		}

		/* Dungeon */
		else
		{
			cave_type *c_ptr;

			/* Access location */
			int y = o_ptr->iy;
			int x = o_ptr->ix;

			/* Access grid */
			c_ptr = &cave[y][x];

			/* Hack -- see above */
			c_ptr->o_idx = 0;
		}

		/* Wipe the object */
		object_wipe(o_ptr);
	}

	/* Reset "o_max" */
	o_max = 1;

	/* Reset "o_cnt" */
	o_cnt = 0;
}


/*
 * Acquires and returns the index of a "free" object.
 *
 * This routine should almost never fail, but in case it does,
 * we must be sure to handle "failure" of this routine.
 */
s16b o_pop(void)
{
	int i;


	/* Initial allocation */
	if (o_max < max_o_idx)
	{
		/* Get next space */
		i = o_max;

		/* Expand object array */
		o_max++;

		/* Count objects */
		o_cnt++;

		/* Use this object */
		return (i);
	}


	/* Recycle dead objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[i];

		/* Skip live objects */
		if (o_ptr->k_idx) continue;

		/* Count objects */
		o_cnt++;

		/* Use this object */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
#ifdef JP
	if (character_dungeon) msg_print("�����ƥब¿�����롪");
#else
	if (character_dungeon) msg_print("Too many objects!");
#endif


	/* Oops */
	return (0);
}


/*
 * Apply a "object restriction function" to the "object allocation table"
 */
errr get_obj_num_prep(void)
{
	int i;

	/* Get the entry */
	alloc_entry *table = alloc_kind_table;

	/* Scan the allocation table */
	for (i = 0; i < alloc_kind_size; i++)
	{
		/* Accept objects which pass the restriction, if any */
		if (!get_obj_num_hook || (*get_obj_num_hook)(table[i].index))
		{
			/* Accept this object */
			table[i].prob2 = table[i].prob1;
		}

		/* Do not use this object */
		else
		{
			/* Decline this object */
			table[i].prob2 = 0;
		}
	}

	/* Success */
	return (0);
}


/*
 * Choose an object kind that seems "appropriate" to the given level
 *
 * This function uses the "prob2" field of the "object allocation table",
 * and various local information, to calculate the "prob3" field of the
 * same table, which is then used to choose an "appropriate" object, in
 * a relatively efficient manner.
 *
 * It is (slightly) more likely to acquire an object of the given level
 * than one of a lower level.  This is done by choosing several objects
 * appropriate to the given level and keeping the "hardest" one.
 *
 * Note that if no objects are "appropriate", then this function will
 * fail, and return zero, but this should *almost* never happen.
 */
s16b get_obj_num(int level)
{
	int             i, j, p;
	int             k_idx;
	long            value, total;
	object_kind     *k_ptr;
	alloc_entry     *table = alloc_kind_table;

	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;

	/* Boost level */
	if ((level > 0) && !(d_info[dungeon_type].flags1 & DF1_BEGINNER))
	{
		/* Occasional "boost" */
		if (one_in_(GREAT_OBJ))
		{
			/* What a bizarre calculation */
			level = 1 + (level * MAX_DEPTH / randint1(MAX_DEPTH));
		}
	}

	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_kind_size; i++)
	{
		/* Objects are sorted by depth */
		if (table[i].level > level) break;

		/* Default */
		table[i].prob3 = 0;

		/* Access the index */
		k_idx = table[i].index;

		/* Access the actual kind */
		k_ptr = &k_info[k_idx];

		/* Hack -- prevent embedded chests */
		if (opening_chest && (k_ptr->tval == TV_CHEST)) continue;

		/* Accept */
		table[i].prob3 = table[i].prob2;

		/* Total */
		total += table[i].prob3;
	}

	/* No legal objects */
	if (total <= 0) return (0);


	/* Pick an object */
	value = randint0(total);

	/* Find the object */
	for (i = 0; i < alloc_kind_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}


	/* Power boost */
	p = randint0(100);

	/* Try for a "better" object once (50%) or twice (10%) */
	if (p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a object */
		value = randint0(total);

		/* Find the object */
		for (i = 0; i < alloc_kind_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Try for a "better" object twice (10%) */
	if (p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a object */
		value = randint0(total);

		/* Find the object */
		for (i = 0; i < alloc_kind_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}


	/* Result */
	return (table[i].index);
}


/*
 * Known is true when the "attributes" of an object are "known".
 * These include tohit, todam, toac, cost, and pval (charges).
 *
 * Note that "knowing" an object gives you everything that an "awareness"
 * gives you, and much more.  In fact, the player is always "aware" of any
 * item of which he has full "knowledge".
 *
 * But having full knowledge of, say, one "wand of wonder", does not, by
 * itself, give you knowledge, or even awareness, of other "wands of wonder".
 * It happens that most "identify" routines (including "buying from a shop")
 * will make the player "aware" of the object as well as fully "know" it.
 *
 * This routine also removes any inscriptions generated by "feelings".
 */
void object_known(object_type *o_ptr)
{
	/* Remove "default inscriptions" */
	o_ptr->feeling = FEEL_NONE;

	/* Clear the "Felt" info */
	o_ptr->ident &= ~(IDENT_SENSE);

	/* Clear the "Empty" info */
	o_ptr->ident &= ~(IDENT_EMPTY);

	/* Now we know about the item */
	o_ptr->ident |= (IDENT_KNOWN);
}


/*
 * The player is now aware of the effects of the given object.
 */
void object_aware(object_type *o_ptr)
{
	bool mihanmei = !object_aware_p(o_ptr);

#ifndef SCRIPT_OBJ_KIND
	/* Fully aware of the effects */
	k_info[o_ptr->k_idx].aware = TRUE;
#else /* SCRIPT_OBJ_KIND */
	/* Fully aware of the effects */
	o_ptr->aware = TRUE;
#endif /* SCRIPT_OBJ_KIND */

	if(mihanmei && !(k_info[o_ptr->k_idx].gen_flags & TRG_INSTA_ART) && record_ident &&
	   !p_ptr->is_dead && ((o_ptr->tval >= TV_AMULET && o_ptr->tval <= TV_POTION) || (o_ptr->tval == TV_FOOD)))
	{
		object_type forge;
		object_type *q_ptr;
		char o_name[MAX_NLEN];

		q_ptr = &forge;
		object_copy(q_ptr, o_ptr);

		q_ptr->number = 1;
		object_desc(o_name, q_ptr, TRUE, 0);
		
		do_cmd_write_nikki(NIKKI_HANMEI, 0, o_name);
	}
}


/*
 * Something has been "sampled"
 */
void object_tried(object_type *o_ptr)
{
#ifndef SCRIPT_OBJ_KIND
	/* Mark it as tried (even if "aware") */
	k_info[o_ptr->k_idx].tried = TRUE;
#else /* SCRIPT_OBJ_KIND */
	o_ptr->tried = TRUE;
#endif /* SCRIPT_OBJ_KIND */
}


/*
 * Return the "value" of an "unknown" item
 * Make a guess at the value of non-aware items
 */
static s32b object_value_base(object_type *o_ptr)
{
	/* Aware item -- use template cost */
	if (object_aware_p(o_ptr)) return (get_object_cost(o_ptr));

	/* Analyze the type */
	switch (o_ptr->tval)
	{

		/* Un-aware Food */
		case TV_FOOD: return (5L);

		/* Un-aware Potions */
		case TV_POTION: return (20L);

		/* Un-aware Scrolls */
		case TV_SCROLL: return (20L);

		/* Un-aware Staffs */
		case TV_STAFF: return (70L);

		/* Un-aware Wands */
		case TV_WAND: return (50L);

		/* Un-aware Rods */
		case TV_ROD: return (90L);

		/* Un-aware Rings */
		case TV_RING: return (45L);

		/* Un-aware Amulets */
		case TV_AMULET: return (45L);

		/* Figurines, relative to monster level */
		case TV_FIGURINE:
		{
			int level = r_info[o_ptr->pval].level;
			if (level < 20) return level*50L;
			else if (level < 30) return 1000+(level-20)*150L;
			else if (level < 40) return 2500+(level-30)*350L;
			else if (level < 50) return 6000+(level-40)*800L;
			else return 14000+(level-50)*2000L;
			break;
		}

		case TV_CAPTURE:
			if (!o_ptr->pval) return 1000L;
			else return ((r_info[o_ptr->pval].level) * 50L + 1000);
	}

	/* Paranoia -- Oops */
	return (0L);
}


/* Return the value of the flags the object has... */
s32b flag_cost(object_type * o_ptr, int plusses)
{
	s32b total = 0;
	u32b f1, f2, f3;
	s32b tmp_cost;
	int count;

	object_flags(o_ptr, &f1, &f2, &f3);

	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		f1 &= ~(a_ptr->flags1);
		f2 &= ~(a_ptr->flags2);
		f3 &= ~(a_ptr->flags3);
	}
	else
	{
		if (o_ptr->name2)
		{
			ego_item_type *e_ptr = &e_info[o_ptr->name2];

			f1 &= ~(e_ptr->flags1);
			f2 &= ~(e_ptr->flags2);
			f3 &= ~(e_ptr->flags3);
			if ((o_ptr->tval == TV_RING) || (o_ptr->tval == TV_AMULET))
			{
				object_kind *k_ptr = &k_info[o_ptr->k_idx];

				f1 &= ~(k_ptr->flags1);
				f2 &= ~(k_ptr->flags2);
				f3 &= ~(k_ptr->flags3);
			}
		}
		else if (o_ptr->art_name)
		{
			total = 5000;
		}
	}

	if (f1 & TR1_STR) total += (1500 * plusses);
	if (f1 & TR1_INT) total += (1500 * plusses);
	if (f1 & TR1_WIS) total += (1500 * plusses);
	if (f1 & TR1_DEX) total += (1500 * plusses);
	if (f1 & TR1_CON) total += (1500 * plusses);
	if (f1 & TR1_CHR) total += (750 * plusses);
	if (f1 & TR1_MAGIC_MASTERY) total += (600 * plusses);
	if (f1 & TR1_STEALTH) total += (250 * plusses);
	if (f1 & TR1_SEARCH) total += (100 * plusses);
	if (f1 & TR1_INFRA) total += (150 * plusses);
	if (f1 & TR1_TUNNEL) total += (175 * plusses);
	if ((f1 & TR1_SPEED) && (plusses > 0))
		total += (10000 + (2500 * plusses));
	if ((f1 & TR1_BLOWS) && (plusses > 0))
		total += (10000 + (2500 * plusses));
	if (f3 & TR3_DEC_MANA) total += 10000;

	tmp_cost = 0;
	count = 0;
	if (f1 & TR1_CHAOTIC) {total += 5000;count++;}
	if (f1 & TR1_VAMPIRIC) {total += 6500;count++;}
	if (f1 & TR1_FORCE_WEAPON) {tmp_cost += 2500;count++;}
	if (f1 & TR1_SLAY_ANIMAL) {tmp_cost += 1800;count++;}
	if (f1 & TR1_SLAY_EVIL) {tmp_cost += 2300;count++;}
	if (f3 & TR3_SLAY_HUMAN) {tmp_cost += 1800;count++;}
	if (f1 & TR1_SLAY_UNDEAD) {tmp_cost += 1800;count++;}
	if (f1 & TR1_SLAY_DEMON) {tmp_cost += 1800;count++;}
	if (f1 & TR1_SLAY_ORC) {tmp_cost += 1500;count++;}
	if (f1 & TR1_SLAY_TROLL) {tmp_cost += 1800;count++;}
	if (f1 & TR1_SLAY_GIANT) {tmp_cost += 1800;count++;}
	if (f1 & TR1_KILL_DRAGON) {tmp_cost += 2800;count++;}
	else if (f1 & TR1_SLAY_DRAGON) {tmp_cost += 1800;count++;}

	if (f1 & TR1_VORPAL) {tmp_cost += 2500;count++;}
	if (f1 & TR1_IMPACT) {tmp_cost += 2500;count++;}
	if (f1 & TR1_BRAND_POIS) {tmp_cost += 3800;count++;}
	if (f1 & TR1_BRAND_ACID) {tmp_cost += 3800;count++;}
	if (f1 & TR1_BRAND_ELEC) {tmp_cost += 3800;count++;}
	if (f1 & TR1_BRAND_FIRE) {tmp_cost += 2500;count++;}
	if (f1 & TR1_BRAND_COLD) {tmp_cost += 2500;count++;}
	total += (tmp_cost * count);

	if (f2 & TR2_SUST_STR) total += 850;
	if (f2 & TR2_SUST_INT) total += 850;
	if (f2 & TR2_SUST_WIS) total += 850;
	if (f2 & TR2_SUST_DEX) total += 850;
	if (f2 & TR2_SUST_CON) total += 850;
	if (f2 & TR2_SUST_CHR) total += 250;
	if (f2 & TR2_RIDING) total += 0;
	if (f2 & TR2_XXX2) total += 0;
	if (f2 & TR2_THROW) total += 5000;
	if (f2 & TR2_FREE_ACT) total += 4500;
	if (f2 & TR2_HOLD_LIFE) total += 8500;

	tmp_cost = 0;
	count = 0;
	if (f2 & TR2_IM_ACID) {tmp_cost += 15000;count += 2;}
	if (f2 & TR2_IM_ELEC) {tmp_cost += 15000;count += 2;}
	if (f2 & TR2_IM_FIRE) {tmp_cost += 15000;count += 2;}
	if (f2 & TR2_IM_COLD) {tmp_cost += 15000;count += 2;}
	if (f2 & TR2_REFLECT) {tmp_cost += 5000;count += 2;}
	if (f2 & TR2_RES_ACID) {tmp_cost += 500;count++;}
	if (f2 & TR2_RES_ELEC) {tmp_cost += 500;count++;}
	if (f2 & TR2_RES_FIRE) {tmp_cost += 500;count++;}
	if (f2 & TR2_RES_COLD) {tmp_cost += 500;count++;}
	if (f2 & TR2_RES_POIS) {tmp_cost += 1000;count += 2;}
	if (f2 & TR2_RES_FEAR) {tmp_cost += 1000;count += 2;}
	if (f2 & TR2_RES_LITE) {tmp_cost += 800;count += 2;}
	if (f2 & TR2_RES_DARK) {tmp_cost += 800;count += 2;}
	if (f2 & TR2_RES_BLIND) {tmp_cost += 900;count += 2;}
	if (f2 & TR2_RES_CONF) {tmp_cost += 900;count += 2;}
	if (f2 & TR2_RES_SOUND) {tmp_cost += 900;count += 2;}
	if (f2 & TR2_RES_SHARDS) {tmp_cost += 900;count += 2;}
	if (f2 & TR2_RES_NETHER) {tmp_cost += 900;count += 2;}
	if (f2 & TR2_RES_NEXUS) {tmp_cost += 900;count += 2;}
	if (f2 & TR2_RES_CHAOS) {tmp_cost += 1000;count += 2;}
	if (f2 & TR2_RES_DISEN) {tmp_cost += 2000;count += 2;}
	total += (tmp_cost * count);

	if (f3 & TR3_SH_FIRE) total += 5000;
	if (f3 & TR3_SH_ELEC) total += 5000;
	if (f3 & TR3_SH_COLD) total += 5000;
	if (f3 & TR3_NO_TELE) total -= 10000;
	if (f3 & TR3_NO_MAGIC) total += 2500;
	if (f3 & TR3_TY_CURSE) total -= 15000;
	if (f3 & TR3_HIDE_TYPE) total += 0;
	if (f3 & TR3_SHOW_MODS) total += 0;
	if (f3 & TR3_FEATHER) total += 1250;
	if (f3 & TR3_LITE) total += 1250;
	if (f3 & TR3_SEE_INVIS) total += 2000;
	if (f3 & TR3_TELEPATHY) total += 20000;
	if (f3 & TR3_SLOW_DIGEST) total += 750;
	if (f3 & TR3_REGEN) total += 2500;
	if (f3 & TR3_WARNING) total += 2000;
	if (f3 & TR3_XTRA_MIGHT) total += 2250;
	if (f3 & TR3_XTRA_SHOTS) total += 10000;
	if (f3 & TR3_IGNORE_ACID) total += 100;
	if (f3 & TR3_IGNORE_ELEC) total += 100;
	if (f3 & TR3_IGNORE_FIRE) total += 100;
	if (f3 & TR3_IGNORE_COLD) total += 100;
	if (f3 & TR3_ACTIVATE) total += 100;
	if (f3 & TR3_DRAIN_EXP) total -= 12500;
	if (f3 & TR3_TELEPORT)
	{
		if (cursed_p(o_ptr))
			total -= 7500;
		else
			total += 250;
	}
	if (f3 & TR3_AGGRAVATE) total -= 10000;
	if (f3 & TR3_BLESSED) total += 750;
	if (o_ptr->curse_flags & TRC_CURSED) total -= 5000;
	if (o_ptr->curse_flags & TRC_HEAVY_CURSE) total -= 12500;
	if (o_ptr->curse_flags & TRC_PERMA_CURSE) total -= 15000;

	/* Also, give some extra for activatable powers... */
	if (o_ptr->art_name && (o_ptr->art_flags3 & TR3_ACTIVATE))
	{
		int type = o_ptr->xtra2;

		if (type == ACT_SUNLIGHT) total += 250;
		else if (type == ACT_BO_MISS_1) total += 250;
		else if (type == ACT_BA_POIS_1) total += 300;
		else if (type == ACT_BO_ELEC_1) total += 250;
		else if (type == ACT_BO_ACID_1) total += 250;
		else if (type == ACT_BO_COLD_1) total += 250;
		else if (type == ACT_BO_FIRE_1) total += 250;
		else if (type == ACT_BA_COLD_1) total += 750;
		else if (type == ACT_BA_FIRE_1) total += 1000;
		else if (type == ACT_DRAIN_1) total += 500;
		else if (type == ACT_BA_COLD_2) total += 1250;
		else if (type == ACT_BA_ELEC_2) total += 1500;
		else if (type == ACT_DRAIN_2) total += 750;
		else if (type == ACT_VAMPIRE_1) total += 1000;
		else if (type == ACT_BO_MISS_2) total += 1000;
		else if (type == ACT_BA_FIRE_2) total += 1750;
		else if (type == ACT_BA_COLD_3) total += 2500;
		else if (type == ACT_BA_ELEC_3) total += 2500;
		else if (type == ACT_WHIRLWIND) total += 7500;
		else if (type == ACT_VAMPIRE_2) total += 2500;
		else if (type == ACT_CALL_CHAOS) total += 5000;
		else if (type == ACT_ROCKET) total += 5000;
		else if (type == ACT_DISP_EVIL) total += 4000;
		else if (type == ACT_DISP_GOOD) total += 3500;
		else if (type == ACT_BA_MISS_3) total += 5000;
		else if (type == ACT_CONFUSE) total += 500;
		else if (type == ACT_SLEEP) total += 750;
		else if (type == ACT_QUAKE) total += 600;
		else if (type == ACT_TERROR) total += 2500;
		else if (type == ACT_TELE_AWAY) total += 2000;
		else if (type == ACT_BANISH_EVIL) total += 2000;
		else if (type == ACT_GENOCIDE) total += 10000;
		else if (type == ACT_MASS_GENO) total += 10000;
		else if (type == ACT_CHARM_ANIMAL) total += 7500;
		else if (type == ACT_CHARM_UNDEAD) total += 10000;
		else if (type == ACT_CHARM_OTHER) total += 10000;
		else if (type == ACT_CHARM_ANIMALS) total += 12500;
		else if (type == ACT_CHARM_OTHERS) total += 17500;
		else if (type == ACT_SUMMON_ANIMAL) total += 10000;
		else if (type == ACT_SUMMON_PHANTOM) total += 12000;
		else if (type == ACT_SUMMON_ELEMENTAL) total += 15000;
		else if (type == ACT_SUMMON_DEMON) total += 20000;
		else if (type == ACT_SUMMON_UNDEAD) total += 20000;
		else if (type == ACT_CURE_LW) total += 500;
		else if (type == ACT_CURE_MW) total += 750;
		else if (type == ACT_CURE_POISON) total += 1000;
		else if (type == ACT_REST_LIFE) total += 7500;
		else if (type == ACT_REST_ALL) total += 15000;
		else if (type == ACT_CURE_700) total += 10000;
		else if (type == ACT_CURE_1000) total += 15000;
		else if (type == ACT_ESP) total += 1500;
		else if (type == ACT_BERSERK) total += 800;
		else if (type == ACT_PROT_EVIL) total += 5000;
		else if (type == ACT_RESIST_ALL) total += 5000;
		else if (type == ACT_SPEED) total += 15000;
		else if (type == ACT_XTRA_SPEED) total += 25000;
		else if (type == ACT_WRAITH) total += 25000;
		else if (type == ACT_INVULN) total += 25000;
		else if (type == ACT_LIGHT) total += 150;
		else if (type == ACT_MAP_LIGHT) total += 500;
		else if (type == ACT_DETECT_ALL) total += 1000;
		else if (type == ACT_DETECT_XTRA) total += 12500;
		else if (type == ACT_ID_FULL) total += 10000;
		else if (type == ACT_ID_PLAIN) total += 1250;
		else if (type == ACT_RUNE_EXPLO) total += 4000;
		else if (type == ACT_RUNE_PROT) total += 10000;
		else if (type == ACT_SATIATE) total += 2000;
		else if (type == ACT_DEST_DOOR) total += 100;
		else if (type == ACT_STONE_MUD) total += 1000;
		else if (type == ACT_RECHARGE) total += 1000;
		else if (type == ACT_ALCHEMY) total += 10000;
		else if (type == ACT_DIM_DOOR) total += 10000;
		else if (type == ACT_TELEPORT) total += 2000;
		else if (type == ACT_RECALL) total += 7500;
	}

	return total;
}


/*
 * Return the "real" price of a "known" item, not including discounts
 *
 * Wand and staffs get cost for each charge
 *
 * Armor is worth an extra 100 gold per bonus point to armor class.
 *
 * Weapons are worth an extra 100 gold per bonus point (AC,TH,TD).
 *
 * Missiles are only worth 5 gold per bonus point, since they
 * usually appear in groups of 20, and we want the player to get
 * the same amount of cash for any "equivalent" item.  Note that
 * missiles never have any of the "pval" flags, and in fact, they
 * only have a few of the available flags, primarily of the "slay"
 * and "brand" and "ignore" variety.
 *
 * Armor with a negative armor bonus is worthless.
 * Weapons with negative hit+damage bonuses are worthless.
 *
 * Every wearable item with a "pval" bonus is worth extra (see below).
 */
s32b object_value_real(object_type *o_ptr)
{
	s32b value;

	u32b f1, f2, f3;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];


	/* Hack -- "worthless" items */
	if (!get_object_cost(o_ptr)) return (0L);

	/* Base cost */
	value = get_object_cost(o_ptr);

	/* Extract some flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Artifact */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Hack -- "worthless" artifacts */
		if (!a_ptr->cost) return (0L);

		/* Hack -- Use the artifact cost instead */
		value = a_ptr->cost;
		value += flag_cost(o_ptr, o_ptr->pval);
		return (value);
	}

	/* Ego-Item */
	else if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Hack -- "worthless" ego-items */
		if (!e_ptr->cost) return (0L);

		/* Hack -- Reward the ego-item with a bonus */
		value += e_ptr->cost;
		value += flag_cost(o_ptr, o_ptr->pval);
	}

	else if (o_ptr->art_flags1 || o_ptr->art_flags2 || o_ptr->art_flags3)
	{
		value += flag_cost(o_ptr, o_ptr->pval);
	}


	/* Analyze pval bonus */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_BOW:
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_HELM:
		case TV_CROWN:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_LITE:
		case TV_AMULET:
		case TV_RING:
		{
			/* Hack -- Negative "pval" is always bad */
			if (o_ptr->pval < 0) return (0L);

			/* No pval */
			if (!o_ptr->pval) break;

			/* Give credit for stat bonuses */
			if (f1 & (TR1_STR)) value += (o_ptr->pval * 200L);
			if (f1 & (TR1_INT)) value += (o_ptr->pval * 200L);
			if (f1 & (TR1_WIS)) value += (o_ptr->pval * 200L);
			if (f1 & (TR1_DEX)) value += (o_ptr->pval * 200L);
			if (f1 & (TR1_CON)) value += (o_ptr->pval * 200L);
			if (f1 & (TR1_CHR)) value += (o_ptr->pval * 200L);

			/* Give credit for stealth and searching */
			if (f1 & (TR1_MAGIC_MASTERY)) value += (o_ptr->pval * 100L);
			if (f1 & (TR1_STEALTH)) value += (o_ptr->pval * 100L);
			if (f1 & (TR1_SEARCH)) value += (o_ptr->pval * 100L);

			/* Give credit for infra-vision and tunneling */
			if (f1 & (TR1_INFRA)) value += (o_ptr->pval * 50L);
			if (f1 & (TR1_TUNNEL)) value += (o_ptr->pval * 50L);

			/* Give credit for extra attacks */
			if (f1 & (TR1_BLOWS)) value += (o_ptr->pval * 5000L);

			/* Give credit for speed bonus */
			if (f1 & (TR1_SPEED)) value += (o_ptr->pval * 10000L);

			break;
		}
	}


	/* Analyze the item */
	switch (o_ptr->tval)
	{
		/* Wands/Staffs */
		case TV_WAND:
		{
			/* Pay extra for charges, depending on standard number of
			 * charges.  Handle new-style wands correctly. -LM-
			 */
			value += (value * o_ptr->pval / o_ptr->number / (k_ptr->pval * 2));

			/* Done */
			break;
		}
		case TV_STAFF:
		{
			/* Pay extra for charges, depending on standard number of
			 * charges.  -LM-
			 */
			value += (value * o_ptr->pval / (k_ptr->pval * 2));

			/* Done */
			break;
		}

		/* Rings/Amulets */
		case TV_RING:
		case TV_AMULET:
		{
			/* Hack -- negative bonuses are bad */
			if (o_ptr->to_a < 0) return (0L);
			if (o_ptr->to_h < 0) return (0L);
			if (o_ptr->to_d < 0) return (0L);

			/* Give credit for bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) * 200L);

			/* Done */
			break;
		}

		/* Armor */
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
			/* Hack -- negative armor bonus */
			if (o_ptr->to_a < 0) return (0L);

			/* Give credit for bonuses */
			value += (((o_ptr->to_h - k_ptr->to_h) + (o_ptr->to_d - k_ptr->to_d)) * 200L + (o_ptr->to_a) * 100L);

			/* Done */
			break;
		}

		/* Bows/Weapons */
		case TV_BOW:
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_SWORD:
		case TV_POLEARM:
		{
			/* Hack -- negative hit/damage bonuses */
			if (o_ptr->to_h + o_ptr->to_d < 0) return (0L);

			/* Factor in the bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) * 100L);

			/* Hack -- Factor in extra damage dice and sides */
			value += (o_ptr->dd - k_ptr->dd) * o_ptr->ds * 250L;
			value += (o_ptr->ds - k_ptr->ds) * o_ptr->dd * 250L;

			/* Done */
			break;
		}

		/* Ammo */
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			/* Hack -- negative hit/damage bonuses */
			if (o_ptr->to_h + o_ptr->to_d < 0) return (0L);

			/* Factor in the bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d) * 5L);

			/* Hack -- Factor in extra damage dice and sides */
			value += (o_ptr->dd - k_ptr->dd) * o_ptr->ds * 5L;
			value += (o_ptr->ds - k_ptr->ds) * o_ptr->dd * 5L;

			/* Done */
			break;
		}

		/* Figurines, relative to monster level */
		case TV_FIGURINE:
		{
			int level = r_info[o_ptr->pval].level;
			if (level < 20) value = level*50L;
			else if (level < 30) value = 1000+(level-20)*150L;
			else if (level < 40) value = 2500+(level-30)*350L;
			else if (level < 50) value = 6000+(level-40)*800L;
			else value = 14000+(level-50)*2000L;
			break;
		}

		case TV_CAPTURE:
		{
			if (!o_ptr->pval) value = 1000L;
			else value = ((r_info[o_ptr->pval].level) * 50L + 1000);
			break;
		}

		case TV_CHEST:
		{
			if (!o_ptr->pval) value = 0L;
			break;
		}
	}

	/* Return the value */
	return (value);
}


/*
 * Return the price of an item including plusses (and charges)
 *
 * This function returns the "value" of the given item (qty one)
 *
 * Never notice "unknown" bonuses or properties, including "curses",
 * since that would give the player information he did not have.
 *
 * Note that discounted items stay discounted forever, even if
 * the discount is "forgotten" by the player via memory loss.
 */
s32b object_value(object_type *o_ptr)
{
	s32b value;


	/* Unknown items -- acquire a base value */
	if (object_known_p(o_ptr))
	{
		/* Broken items -- worthless */
		if (broken_p(o_ptr)) return (0L);

		/* Cursed items -- worthless */
		if (cursed_p(o_ptr)) return (0L);

		/* Real value (see above) */
		value = object_value_real(o_ptr);
	}

	/* Known items -- acquire the actual value */
	else
	{
		/* Hack -- Felt broken items */
		if ((o_ptr->ident & (IDENT_SENSE)) && broken_p(o_ptr)) return (0L);

		/* Hack -- Felt cursed items */
		if ((o_ptr->ident & (IDENT_SENSE)) && cursed_p(o_ptr)) return (0L);

		/* Base value (see above) */
		value = object_value_base(o_ptr);
	}


	/* Apply discount (if any) */
	if (o_ptr->discount) value -= (value * o_ptr->discount / 100L);


	/* Return the final value */
	return (value);
}


/*
 * Determines whether an object can be destroyed, and makes fake inscription.
 */
bool can_player_destroy_object(object_type *o_ptr)
{
	/* Artifacts cannot be destroyed */
	if (artifact_p(o_ptr) || o_ptr->art_name)
	{
		byte feel = FEEL_SPECIAL;

		/* Hack -- Handle icky artifacts */
		if (cursed_p(o_ptr) || broken_p(o_ptr)) feel = FEEL_TERRIBLE;

		/* Hack -- inscribe the artifact */
		o_ptr->feeling = feel;

		/* We have "felt" it (again) */
		o_ptr->ident |= (IDENT_SENSE);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Done */
		return FALSE;
	}

	return TRUE;
}


/*
 * Distribute charges of rods or wands.
 *
 * o_ptr = source item
 * q_ptr = target item, must be of the same type as o_ptr
 * amt   = number of items that are transfered
 */
void distribute_charges(object_type *o_ptr, object_type *q_ptr, int amt)
{
	/*
	 * Hack -- If rods or wands are dropped, the total maximum timeout or
	 * charges need to be allocated between the two stacks.  If all the items
	 * are being dropped, it makes for a neater message to leave the original
	 * stack's pval alone. -LM-
	 */
	if ((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_ROD))
	{
		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
		if (amt < o_ptr->number) o_ptr->pval -= q_ptr->pval;

		/* Hack -- Rods also need to have their timeouts distributed.  The
		 * dropped stack will accept all time remaining to charge up to its
		 * maximum.
		 */
		if ((o_ptr->tval == TV_ROD) && (o_ptr->timeout))
		{
			if (q_ptr->pval > o_ptr->timeout)
				q_ptr->timeout = o_ptr->timeout;
			else
				q_ptr->timeout = q_ptr->pval;

			if (amt < o_ptr->number) o_ptr->timeout -= q_ptr->timeout;
		}
	}
}

void reduce_charges(object_type *o_ptr, int amt)
{
	/*
	 * Hack -- If rods or wand are destroyed, the total maximum timeout or
	 * charges of the stack needs to be reduced, unless all the items are
	 * being destroyed. -LM-
	 */
	if (((o_ptr->tval == TV_WAND) || (o_ptr->tval == TV_ROD)) &&
		(amt < o_ptr->number))
	{
		o_ptr->pval -= o_ptr->pval * amt / o_ptr->number;
	}
}


/*
 * Determine if an item can "absorb" a second item
 *
 * See "object_absorb()" for the actual "absorption" code.
 *
 * If permitted, we allow staffs (if they are known to have equal charges
 * and both are either known or confirmed empty) and wands (if both are
 * either known or confirmed empty) and rods (in all cases) to combine.
 * Staffs will unstack (if necessary) when they are used, but wands and
 * rods will only unstack if one is dropped. -LM-
 *
 * If permitted, we allow weapons/armor to stack, if fully "known".
 *
 * Missiles will combine if both stacks have the same "known" status.
 * This is done to make unidentified stacks of missiles useful.
 *
 * Food, potions, scrolls, and "easy know" items always stack.
 *
 * Chests, and activatable items, never stack (for various reasons).
 */

/*
 *  Determine if an item can partly absorb a second item.
 */
static bool object_similar_part(object_type *o_ptr, object_type *j_ptr)
{
	/* Require identical object types */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);


	/* Analyze the items */
	switch (o_ptr->tval)
	{
		/* Chests and Statues*/
		case TV_CHEST:
		case TV_CARD:
		case TV_CAPTURE:
		{
			/* Never okay */
			return (0);
		}

		case TV_STATUE:
		{
			if ((o_ptr->sval != SV_PHOTO) || (j_ptr->sval != SV_PHOTO)) return (0);
			if (o_ptr->pval != j_ptr->pval) return (0);
			break;
		}

		/* Figurines and Corpses*/
		case TV_FIGURINE:
		case TV_CORPSE:
		{
			/* Same monster */
			if (o_ptr->pval != j_ptr->pval) return (0);

			/* Assume okay */
			break;
		}

		/* Food and Potions and Scrolls */
		case TV_FOOD:
		case TV_POTION:
		case TV_SCROLL:
		{
			/* Assume okay */
			break;
		}

		/* Staffs */
		case TV_STAFF:
		{
			/* Require either knowledge or known empty for both staffs. */
			if ((!(o_ptr->ident & (IDENT_EMPTY)) &&
				!object_known_p(o_ptr)) ||
				(!(j_ptr->ident & (IDENT_EMPTY)) &&
				!object_known_p(j_ptr))) return(0);

			/* Require identical charges, since staffs are bulky. */
			if (o_ptr->pval != j_ptr->pval) return (0);

			/* Assume okay */
			break;
		}

		/* Wands */
		case TV_WAND:
		{
			/* Require either knowledge or known empty for both wands. */
			if ((!(o_ptr->ident & (IDENT_EMPTY)) &&
				!object_known_p(o_ptr)) ||
				(!(j_ptr->ident & (IDENT_EMPTY)) &&
				!object_known_p(j_ptr))) return(0);

			/* Wand charges combine in O&ZAngband.  */

			/* Assume okay */
			break;
		}

		/* Staffs and Wands and Rods */
		case TV_ROD:
		{
			/* Assume okay */
			break;
		}

		/* Weapons and Armor */
		case TV_BOW:
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_HELM:
		case TV_CROWN:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		{
			/* Require permission */
			if (!stack_allow_items) return (0);

			/* Fall through */
		}

		/* Rings, Amulets, Lites */
		case TV_RING:
		case TV_AMULET:
		case TV_LITE:
		{
			/* Require full knowledge of both items */
			if (!object_known_p(o_ptr) || !object_known_p(j_ptr)) return (0);

			/* Fall through */
		}

		/* Missiles */
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			/* Require identical knowledge of both items */
			if (object_known_p(o_ptr) != object_known_p(j_ptr)) return (0);

			/* Require identical "bonuses" */
			if (o_ptr->to_h != j_ptr->to_h) return (FALSE);
			if (o_ptr->to_d != j_ptr->to_d) return (FALSE);
			if (o_ptr->to_a != j_ptr->to_a) return (FALSE);

			/* Require identical "pval" code */
			if (o_ptr->pval != j_ptr->pval) return (FALSE);

			/* Require identical "artifact" names */
			if (o_ptr->name1 != j_ptr->name1) return (FALSE);

			/* Random artifacts never stack */
			if (o_ptr->art_name || j_ptr->art_name) return (FALSE);

			/* Require identical "ego-item" names */
			if (o_ptr->name2 != j_ptr->name2) return (FALSE);

			/* Require identical added essence  */
			if (o_ptr->xtra3 != j_ptr->xtra3) return (FALSE);
			if (o_ptr->xtra4 != j_ptr->xtra4) return (FALSE);

			/* Hack -- Never stack "powerful" items */
			if (o_ptr->xtra1 || j_ptr->xtra1) return (FALSE);

			/* Hack -- Never stack recharging items */
			if (o_ptr->timeout || j_ptr->timeout) return (FALSE);

			/* Require identical "values" */
			if (o_ptr->ac != j_ptr->ac) return (FALSE);
			if (o_ptr->dd != j_ptr->dd) return (FALSE);
			if (o_ptr->ds != j_ptr->ds) return (FALSE);

			/* Probably okay */
			break;
		}

		/* Various */
		default:
		{
			/* Require knowledge */
			if (!object_known_p(o_ptr) || !object_known_p(j_ptr)) return (0);

			/* Probably okay */
			break;
		}
	}


	/* Hack -- Identical art_flags! */
	if ((o_ptr->art_flags1 != j_ptr->art_flags1) ||
	    (o_ptr->art_flags2 != j_ptr->art_flags2) ||
	    (o_ptr->art_flags3 != j_ptr->art_flags3))
		return (0);

	/* Hack -- Require identical "cursed" status */
	if (o_ptr->curse_flags != j_ptr->curse_flags) return (0);

	/* Hack -- Require identical "broken" status */
	if ((o_ptr->ident & (IDENT_BROKEN)) != (j_ptr->ident & (IDENT_BROKEN))) return (0);


	/* Hack -- require semi-matching "inscriptions" */
	if (o_ptr->inscription && j_ptr->inscription &&
	    (o_ptr->inscription != j_ptr->inscription))
		return (0);

	/* Hack -- normally require matching "inscriptions" */
	if (!stack_force_notes && (o_ptr->inscription != j_ptr->inscription)) return (0);

	/* Hack -- normally require matching "discounts" */
	if (!stack_force_costs && (o_ptr->discount != j_ptr->discount)) return (0);


	/* They match, so they must be similar */
	return (TRUE);
}

/*
 *  Determine if an item can absorb a second item.
 */
bool object_similar(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	if (!object_similar_part(o_ptr, j_ptr))
		return FALSE;

	/* Maximal "stacking" limit */
	if (total >= MAX_STACK_SIZE) return (0);


	/* They match, so they must be similar */
	return (TRUE);
}



/*
 * Allow one item to "absorb" another, assuming they are similar
 */
void object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Add together the item counts */
	o_ptr->number = ((total < MAX_STACK_SIZE) ? total : (MAX_STACK_SIZE - 1));

	/* Hack -- blend "known" status */
	if (object_known_p(j_ptr)) object_known(o_ptr);

	/* Hack -- clear "storebought" if only one has it */
	if (((o_ptr->ident & IDENT_STOREB) || (j_ptr->ident & IDENT_STOREB)) &&
	    (!((o_ptr->ident & IDENT_STOREB) && (j_ptr->ident & IDENT_STOREB))))
	{
		if (j_ptr->ident & IDENT_STOREB) j_ptr->ident &= 0xEF;
		if (o_ptr->ident & IDENT_STOREB) o_ptr->ident &= 0xEF;
	}

	/* Hack -- blend "mental" status */
	if (j_ptr->ident & (IDENT_MENTAL)) o_ptr->ident |= (IDENT_MENTAL);

	/* Hack -- blend "inscriptions" */
	if (j_ptr->inscription) o_ptr->inscription = j_ptr->inscription;

	/* Hack -- blend "feelings" */
	if (j_ptr->feeling) o_ptr->feeling = j_ptr->feeling;

	/* Hack -- could average discounts XXX XXX XXX */
	/* Hack -- save largest discount XXX XXX XXX */
	if (o_ptr->discount < j_ptr->discount) o_ptr->discount = j_ptr->discount;

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval;
		o_ptr->timeout += j_ptr->timeout;
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval;
	}
}


/*
 * Find the index of the object_kind with the given tval and sval
 */
s16b lookup_kind(int tval, int sval)
{
	int k;
	int num = 0;
	int bk = 0;

	/* Look for it */
	for (k = 1; k < max_k_idx; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Require correct tval */
		if (k_ptr->tval != tval) continue;

		/* Found a match */
		if (k_ptr->sval == sval) return (k);

		/* Ignore illegal items */
		if (sval != SV_ANY) continue;

		/* Apply the randomizer */
		if (!one_in_(++num)) continue;

		/* Use this value */
		bk = k;
	}

	/* Return this choice */
	if (sval == SV_ANY)
	{
		return bk;
	}

#if 0
	/* Oops */
#ifdef JP
	msg_format("�����ƥब�ʤ� (%d,%d)", tval, sval);
#else
	msg_format("No object (%d,%d)", tval, sval);
#endif
#endif


	/* Oops */
	return (0);
}


/*
 * Wipe an object clean.
 */
void object_wipe(object_type *o_ptr)
{
	/* Wipe the structure */
	(void)WIPE(o_ptr, object_type);
}


/*
 * Prepare an object based on an existing object
 */
void object_copy(object_type *o_ptr, object_type *j_ptr)
{
	/* Copy the structure */
	COPY(o_ptr, j_ptr, object_type);
}


/*
 * Prepare an object based on an object kind.
 */
void object_prep(object_type *o_ptr, int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Clear the record */
	object_wipe(o_ptr);

	/* Save the kind index */
	o_ptr->k_idx = k_idx;

	/* Efficiency -- tval/sval */
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;

	/* Default "pval" */
	o_ptr->pval = k_ptr->pval;

	/* Default number */
	o_ptr->number = 1;

	/* Default weight */
	o_ptr->weight = k_ptr->weight;

	/* Default magic */
	o_ptr->to_h = k_ptr->to_h;
	o_ptr->to_d = k_ptr->to_d;
	o_ptr->to_a = k_ptr->to_a;

	/* Default power */
	o_ptr->ac = k_ptr->ac;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Hack -- worthless items are always "broken" */
	if (get_object_cost(o_ptr) <= 0) o_ptr->ident |= (IDENT_BROKEN);

	/* Hack -- cursed items are always "cursed" */
	if (k_ptr->gen_flags & (TRG_CURSED)) o_ptr->curse_flags |= (TRC_CURSED);
	if (k_ptr->gen_flags & (TRG_HEAVY_CURSE)) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
	if (k_ptr->gen_flags & (TRG_PERMA_CURSE)) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
	if (k_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
	if (k_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
	if (k_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);
}


/*
 * Help determine an "enchantment bonus" for an object.
 *
 * To avoid floating point but still provide a smooth distribution of bonuses,
 * we simply round the results of division in such a way as to "average" the
 * correct floating point value.
 *
 * This function has been changed.  It uses "randnor()" to choose values from
 * a normal distribution, whose mean moves from zero towards the max as the
 * level increases, and whose standard deviation is equal to 1/4 of the max,
 * and whose values are forced to lie between zero and the max, inclusive.
 *
 * Since the "level" rarely passes 100 before Morgoth is dead, it is very
 * rare to get the "full" enchantment on an object, even a deep levels.
 *
 * It is always possible (albeit unlikely) to get the "full" enchantment.
 *
 * A sample distribution of values from "m_bonus(10, N)" is shown below:
 *
 *   N       0     1     2     3     4     5     6     7     8     9    10
 * ---    ----  ----  ----  ----  ----  ----  ----  ----  ----  ----  ----
 *   0   66.37 13.01  9.73  5.47  2.89  1.31  0.72  0.26  0.12  0.09  0.03
 *   8   46.85 24.66 12.13  8.13  4.20  2.30  1.05  0.36  0.19  0.08  0.05
 *  16   30.12 27.62 18.52 10.52  6.34  3.52  1.95  0.90  0.31  0.15  0.05
 *  24   22.44 15.62 30.14 12.92  8.55  5.30  2.39  1.63  0.62  0.28  0.11
 *  32   16.23 11.43 23.01 22.31 11.19  7.18  4.46  2.13  1.20  0.45  0.41
 *  40   10.76  8.91 12.80 29.51 16.00  9.69  5.90  3.43  1.47  0.88  0.65
 *  48    7.28  6.81 10.51 18.27 27.57 11.76  7.85  4.99  2.80  1.22  0.94
 *  56    4.41  4.73  8.52 11.96 24.94 19.78 11.06  7.18  3.68  1.96  1.78
 *  64    2.81  3.07  5.65  9.17 13.01 31.57 13.70  9.30  6.04  3.04  2.64
 *  72    1.87  1.99  3.68  7.15 10.56 20.24 25.78 12.17  7.52  4.42  4.62
 *  80    1.02  1.23  2.78  4.75  8.37 12.04 27.61 18.07 10.28  6.52  7.33
 *  88    0.70  0.57  1.56  3.12  6.34 10.06 15.76 30.46 12.58  8.47 10.38
 *  96    0.27  0.60  1.25  2.28  4.30  7.60 10.77 22.52 22.51 11.37 16.53
 * 104    0.22  0.42  0.77  1.36  2.62  5.33  8.93 13.05 29.54 15.23 22.53
 * 112    0.15  0.20  0.56  0.87  2.00  3.83  6.86 10.06 17.89 27.31 30.27
 * 120    0.03  0.11  0.31  0.46  1.31  2.48  4.60  7.78 11.67 25.53 45.72
 * 128    0.02  0.01  0.13  0.33  0.83  1.41  3.24  6.17  9.57 14.22 64.07
 */
s16b m_bonus(int max, int level)
{
	int bonus, stand, extra, value;


	/* Paranoia -- enforce maximal "level" */
	if (level > MAX_DEPTH - 1) level = MAX_DEPTH - 1;


	/* The "bonus" moves towards the max */
	bonus = ((max * level) / MAX_DEPTH);

	/* Hack -- determine fraction of error */
	extra = ((max * level) % MAX_DEPTH);

	/* Hack -- simulate floating point computations */
	if (randint0(MAX_DEPTH) < extra) bonus++;


	/* The "stand" is equal to one quarter of the max */
	stand = (max / 4);

	/* Hack -- determine fraction of error */
	extra = (max % 4);

	/* Hack -- simulate floating point computations */
	if (randint0(4) < extra) stand++;


	/* Choose an "interesting" value */
	value = randnor(bonus, stand);

	/* Enforce the minimum value */
	if (value < 0) return (0);

	/* Enforce the maximum value */
	if (value > max) return (max);

	/* Result */
	return (value);
}


/*
 * Cheat -- describe a created object for the user
 */
static void object_mention(object_type *o_ptr)
{
	char o_name[MAX_NLEN];

	/* Describe */
	object_desc_store(o_name, o_ptr, FALSE, 0);

	/* Artifact */
	if (artifact_p(o_ptr))
	{
		/* Silly message */
#ifdef JP
		msg_format("����Υ����ƥ� (%s)", o_name);
#else
		msg_format("Artifact (%s)", o_name);
#endif

	}

	/* Random Artifact */
	else if (o_ptr->art_name)
	{
#ifdef JP
		msg_print("�����ࡦ�����ƥ��ե�����");
#else
		msg_print("Random artifact");
#endif

	}

	/* Ego-item */
	else if (ego_item_p(o_ptr))
	{
		/* Silly message */
#ifdef JP
		msg_format("̾�Τ��륢���ƥ� (%s)", o_name);
#else
		msg_format("Ego-item (%s)", o_name);
#endif

	}

	/* Normal item */
	else
	{
		/* Silly message */
#ifdef JP
		msg_format("�����ƥ� (%s)", o_name);
#else
		msg_format("Object (%s)", o_name);
#endif

	}
}


/*
 * Mega-Hack -- Attempt to create one of the "Special Objects"
 *
 * We are only called from "make_object()", and we assume that
 * "apply_magic()" is called immediately after we return.
 *
 * Note -- see "make_artifact()" and "apply_magic()"
 */
static bool make_artifact_special(object_type *o_ptr)
{
	int i;
	int k_idx = 0;


	/* No artifacts in the town */
	if (!dun_level) return (FALSE);

	/* Themed object */
	if (get_obj_num_hook) return (FALSE);

	/* Check the artifact list (just the "specials") */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Cannot make an artifact twice */
		if (a_ptr->cur_num) continue;

		if (a_ptr->gen_flags & TRG_QUESTITEM) continue;
		if (!(a_ptr->gen_flags & TRG_INSTA_ART)) continue;

		/* XXX XXX Enforce minimum "depth" (loosely) */
		if (a_ptr->level > dun_level)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (a_ptr->level - dun_level) * 2;

			/* Roll for out-of-depth creation */
			if (!one_in_(d)) continue;
		}

		/* Artifact "rarity roll" */
		if (!one_in_(a_ptr->rarity)) continue;

		/* Find the base object */
		k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* XXX XXX Enforce minimum "object" level (loosely) */
		if (k_info[k_idx].level > object_level)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (k_info[k_idx].level - object_level) * 5;

			/* Roll for out-of-depth creation */
			if (!one_in_(d)) continue;
		}

		/* Assign the template */
		object_prep(o_ptr, k_idx);

		/* Mega-Hack -- mark the item as an artifact */
		o_ptr->name1 = i;

		/* Hack: Some artifacts get random extra powers */
		random_artifact_resistance(o_ptr, a_ptr);

		/* Success */
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}


/*
 * Attempt to change an object into an artifact
 *
 * This routine should only be called by "apply_magic()"
 *
 * Note -- see "make_artifact_special()" and "apply_magic()"
 */
static bool make_artifact(object_type *o_ptr)
{
	int i;


	/* No artifacts in the town */
	if (!dun_level) return (FALSE);

	/* Paranoia -- no "plural" artifacts */
	if (o_ptr->number != 1) return (FALSE);

	/* Check the artifact list (skip the "specials") */
	for (i = 0; i < max_a_idx; i++)
	{
		artifact_type *a_ptr = &a_info[i];

		/* Skip "empty" items */
		if (!a_ptr->name) continue;

		/* Cannot make an artifact twice */
		if (a_ptr->cur_num) continue;

		if (a_ptr->gen_flags & TRG_QUESTITEM) continue;

		if (a_ptr->gen_flags & TRG_INSTA_ART) continue;

		/* Must have the correct fields */
		if (a_ptr->tval != o_ptr->tval) continue;
		if (a_ptr->sval != o_ptr->sval) continue;

		/* XXX XXX Enforce minimum "depth" (loosely) */
		if (a_ptr->level > dun_level)
		{
			/* Acquire the "out-of-depth factor" */
			int d = (a_ptr->level - dun_level) * 2;

			/* Roll for out-of-depth creation */
			if (!one_in_(d)) continue;
		}

		/* We must make the "rarity roll" */
		if (!one_in_(a_ptr->rarity)) continue;

		/* Hack -- mark the item as an artifact */
		o_ptr->name1 = i;

		/* Hack: Some artifacts get random extra powers */
		random_artifact_resistance(o_ptr, a_ptr);

		/* Success */
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}


/*
 *  Choose random ego type
 */
static byte get_random_ego(byte slot, bool good, int level)
{
	int i, value;
	ego_item_type *e_ptr;

	long total = 0L;
	
	for (i = 1; i < max_e_idx; i++)
	{
		e_ptr = &e_info[i];
		
		if (e_ptr->slot == slot
		    /* && level >= e_ptr->level */
		    && ((good && e_ptr->rating) || (!good && !e_ptr->rating)) )
		{
			if (e_ptr->rarity)
				total += (255 / e_ptr->rarity);
		}
	}

	value = randint1(total);

	for (i = 1; i < max_e_idx; i++)
	{
		e_ptr = &e_info[i];
		
		if (e_ptr->slot == slot
		    /* && level >= e_ptr->level */
		    && ((good && e_ptr->rating) || (!good && !e_ptr->rating)) )
		{
			if (e_ptr->rarity)
				value -= (255 / e_ptr->rarity);
			if (value <= 0L) break;
		}
	}
	return (byte)i;
}


/*
 * Apply magic to an item known to be a "weapon"
 *
 * Hack -- note special base damage dice boosting
 * Hack -- note special processing for weapon/digger
 * Hack -- note special rating boost for dragon scale mail
 */
static void a_m_aux_1(object_type *o_ptr, int level, int power)
{
	int tohit1 = randint1(5) + m_bonus(5, level);
	int todam1 = randint1(5) + m_bonus(5, level);

	int tohit2 = m_bonus(10, level);
	int todam2 = m_bonus(10, level);

	if ((o_ptr->tval == TV_BOLT) || (o_ptr->tval == TV_ARROW) || (o_ptr->tval == TV_SHOT))
	{
		tohit2 = (tohit2+1)/2;
		todam2 = (todam2+1)/2;
	}

	/* Good */
	if (power > 0)
	{
		/* Enchant */
		o_ptr->to_h += tohit1;
		o_ptr->to_d += todam1;

		/* Very good */
		if (power > 1)
		{
			/* Enchant again */
			o_ptr->to_h += tohit2;
			o_ptr->to_d += todam2;
		}
	}

	/* Cursed */
	else if (power < 0)
	{
		/* Penalize */
		o_ptr->to_h -= tohit1;
		o_ptr->to_d -= todam1;

		/* Very cursed */
		if (power < -1)
		{
			/* Penalize again */
			o_ptr->to_h -= tohit2;
			o_ptr->to_d -= todam2;
		}

		/* Cursed (if "bad") */
		if (o_ptr->to_h + o_ptr->to_d < 0) o_ptr->curse_flags |= TRC_CURSED;
	}

	if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DIAMOND_EDGE)) return;

	/* Analyze type */
	switch (o_ptr->tval)
	{
		case TV_DIGGING:
		{
			/* Very good */
			if (power > 1)
			{
				if (one_in_(30))
					create_artifact(o_ptr, FALSE);
				else
					/* Special Ego-item */
					o_ptr->name2 = EGO_DIGGING;
			}

			/* Very bad */
			else if (power < -1)
			{
				/* Hack -- Horrible digging bonus */
				o_ptr->pval = 0 - (5 + randint1(5));
			}

			/* Bad */
			else if (power < 0)
			{
				/* Hack -- Reverse digging bonus */
				o_ptr->pval = 0 - (o_ptr->pval);
			}

			break;
		}


		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		{
			/* Very Good */
			if (power > 1)
			{
				if (one_in_(40))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				while (1)
				{
					/* Roll for an ego-item */
					o_ptr->name2 = get_random_ego(INVEN_RARM, TRUE, level);
					if (o_ptr->name2 == EGO_SHARPNESS && o_ptr->tval != TV_SWORD)
						continue;
					if (o_ptr->name2 == EGO_EARTHQUAKES && o_ptr->tval != TV_HAFTED)
						continue;
					break;
				}

				switch (o_ptr->name2)
				{
				case EGO_HA:
					if (one_in_(4) && (level > 40))
						o_ptr->art_flags1 |= TR1_BLOWS;
					break;
				case EGO_DF:
					if (one_in_(3))
						o_ptr->art_flags2 |= TR2_RES_POIS;
					if (one_in_(3))
						o_ptr->art_flags3 |= TR3_WARNING;
					break;
				case EGO_KILL_DRAGON:
					if (one_in_(3))
						o_ptr->art_flags2 |= TR2_RES_POIS;
					break;
				case EGO_WEST:
					if (one_in_(3))
						o_ptr->art_flags2 |= TR2_RES_FEAR;
					break;
				case EGO_SLAYING_WEAPON:
					if (one_in_(3)) /* double damage */
						o_ptr->dd *= 2;
					else
					{
						do
						{
							o_ptr->dd++;
						}
						while (one_in_(o_ptr->dd));
						
						do
						{
							o_ptr->ds++;
						}
						while (one_in_(o_ptr->ds));
					}
					
					if (one_in_(5))
					{
						o_ptr->art_flags1 |= TR1_BRAND_POIS;
					}
					if (o_ptr->tval == TV_SWORD && one_in_(3))
					{
						o_ptr->art_flags1 |= TR1_VORPAL;
					}
					break;
				case EGO_TRUMP:
					if (one_in_(5))
						o_ptr->art_flags1 |= TR1_SLAY_DEMON;
					if (one_in_(7))
						one_ability(o_ptr);
					break;
				case EGO_PATTERN:
					if (one_in_(3))
						o_ptr->art_flags2 |= TR2_HOLD_LIFE;
					if (one_in_(3))
						o_ptr->art_flags1 |= TR1_DEX;
					if (one_in_(5))
						o_ptr->art_flags2 |= TR2_RES_FEAR;
					break;
				case EGO_SHARPNESS:
					o_ptr->pval = m_bonus(5, level) + 1;
					break;
				case EGO_EARTHQUAKES:
					if (one_in_(3) && (level > 60))
						o_ptr->art_flags1 |= TR1_BLOWS;
					else
						o_ptr->pval = m_bonus(3, level);
					break;
				case EGO_VAMPIRIC:
					if (one_in_(5))
						o_ptr->art_flags3 |= TR3_SLAY_HUMAN;
					break;
				}

				if (!o_ptr->art_name)
				{
					/* Hack -- Super-charge the damage dice */
					while (one_in_(10L * o_ptr->dd * o_ptr->ds)) o_ptr->dd++;

					/* Hack -- Lower the damage dice */
					if (o_ptr->dd > 9) o_ptr->dd = 9;
				}
			}

			/* Very cursed */
			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					o_ptr->name2 = get_random_ego(INVEN_RARM, FALSE, level);
					switch (o_ptr->name2)
					{
					case EGO_MORGUL:
						if (one_in_(6)) o_ptr->art_flags3 |= TR3_TY_CURSE;
					}
				}
			}

			break;
		}


		case TV_BOW:
		{
			/* Very good */
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_BOW, TRUE, level);
			}

			break;
		}


		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			/* Very good */
			if (power > 1)
			{
				o_ptr->name2 = get_random_ego(INVEN_AMMO, TRUE, level);

				switch (o_ptr->name2)
				{
				case EGO_SLAYING_BOLT:
					o_ptr->dd++;
					break;
				}

				/* Hack -- super-charge the damage dice */
				while (one_in_(10L * o_ptr->dd * o_ptr->ds)) o_ptr->dd++;

				/* Hack -- restrict the damage dice */
				if (o_ptr->dd > 9) o_ptr->dd = 9;
			}

			/* Very cursed */
			else if (power < -1)
			{
				/* Roll for ego-item */
				if (randint0(MAX_DEPTH) < level)
				{
					o_ptr->name2 = get_random_ego(INVEN_AMMO, FALSE, level);
				}
			}

			break;
		}
	}
}


static void dragon_resist(object_type * o_ptr)
{
	do
	{
		if (one_in_(4))
			one_dragon_ele_resistance(o_ptr);
		else
			one_high_resistance(o_ptr);
	}
	while (one_in_(2));
}


/*
 * Apply magic to an item known to be "armor"
 *
 * Hack -- note special processing for crown/helm
 * Hack -- note special processing for robe of permanence
 */
static void a_m_aux_2(object_type *o_ptr, int level, int power)
{
	int toac1 = randint1(5) + m_bonus(5, level);

	int toac2 = m_bonus(10, level);

	/* Good */
	if (power > 0)
	{
		/* Enchant */
		o_ptr->to_a += toac1;

		/* Very good */
		if (power > 1)
		{
			/* Enchant again */
			o_ptr->to_a += toac2;
		}
	}

	/* Cursed */
	else if (power < 0)
	{
		/* Penalize */
		o_ptr->to_a -= toac1;

		/* Very cursed */
		if (power < -1)
		{
			/* Penalize again */
			o_ptr->to_a -= toac2;
		}

		/* Cursed (if "bad") */
		if (o_ptr->to_a < 0) o_ptr->curse_flags |= TRC_CURSED;
	}


	/* Analyze type */
	switch (o_ptr->tval)
	{
		case TV_DRAG_ARMOR:
		{
			/* Rating boost */
			rating += 30;
			if(one_in_(50))
				create_artifact(o_ptr, FALSE);

			/* Mention the item */
			if (cheat_peek) object_mention(o_ptr);

			break;
		}

		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		{
			/* Very good */
			if (power > 1)
			{
				/* Hack -- Try for "Robes of the Magi" */
				if ((o_ptr->tval == TV_SOFT_ARMOR) &&
				    (o_ptr->sval == SV_ROBE) &&
				    (randint0(100) < 15))
				{
					if (one_in_(5))
					{
						o_ptr->name2 = EGO_YOIYAMI;
						o_ptr->k_idx = lookup_kind(TV_SOFT_ARMOR, SV_YOIYAMI_ROBE);
						o_ptr->sval = SV_YOIYAMI_ROBE;
						o_ptr->ac = 0;
						o_ptr->to_a = 0;
					}
					else
					{
						o_ptr->name2 = EGO_PERMANENCE;
					}
					break;
				}

				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}

				while (1)
				{
					bool okay_flag = TRUE;

					o_ptr->name2 = get_random_ego(INVEN_BODY, TRUE, level);

					switch (o_ptr->name2)
					{
					case EGO_RESISTANCE:
						if (one_in_(4))
							o_ptr->art_flags2 |= TR2_RES_POIS;
						break;
					case EGO_ELVENKIND:
						break;
					case EGO_DWARVEN:
						if (o_ptr->tval != TV_HARD_ARMOR)
						{
							okay_flag = FALSE;
							break;
						}
						else
						{
							o_ptr->weight = (2 * k_info[o_ptr->k_idx].weight / 3);
							o_ptr->ac = k_info[o_ptr->k_idx].ac + 5;
							if (one_in_(4))
								o_ptr->art_flags1 |= TR1_CON;
							break;
						}
					}

					if (okay_flag)
						break;
				}
			}

			break;
		}

		case TV_SHIELD:
		{

			if (o_ptr->sval == SV_DRAGON_SHIELD)
			{
				/* Rating boost */
				rating += 5;

				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}

			/* Very good */
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_LARM, TRUE, level);
				
				switch (o_ptr->name2)
				{
				case EGO_ENDURANCE:
					if (!one_in_(3)) one_high_resistance(o_ptr);
					if (one_in_(4)) o_ptr->art_flags2 |= TR2_RES_POIS;
					break;
				case EGO_REFLECTION:
					if (o_ptr->sval == SV_SHIELD_OF_DEFLECTION)
						o_ptr->name2 = 0;
					break;
				}
			}
			break;
		}

		case TV_GLOVES:
		{
			if (o_ptr->sval == SV_SET_OF_DRAGON_GLOVES)
			{
				/* Rating boost */
				rating += 5;

				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_HANDS, TRUE, level);
			}
			
			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_HANDS, FALSE, level);
			}

			break;
		}

		case TV_BOOTS:
		{
			if (o_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE)
			{
				/* Rating boost */
				rating += 5;

				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}
			/* Very good */
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_FEET, TRUE, level);

				switch (o_ptr->name2)
				{
				case EGO_SLOW_DESCENT:
					if (one_in_(2))
					{
						one_high_resistance(o_ptr);
					}
					break;
				}
			}
			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_FEET, FALSE, level);
			}

			break;
		}

		case TV_CROWN:
		{
			/* Very good */
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				while (1)
				{
					bool ok_flag = TRUE;
					o_ptr->name2 = get_random_ego(INVEN_HEAD, TRUE, level);

					switch (o_ptr->name2)
					{
					case EGO_MAGI:
					case EGO_MIGHT:
					case EGO_TELEPATHY:
					case EGO_REGENERATION:
					case EGO_LORDLINESS:
						break;
					case EGO_SEEING:
						if (one_in_(3)) o_ptr->art_flags3 |= TR3_TELEPATHY;
						break;
					default:/* not existing crown (wisdom,lite, etc...) */
						ok_flag = FALSE;
					}
					if (ok_flag)
						break; /* while (1) */
				}
				break;
			}

			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_HEAD, FALSE, level);
			}

			break;
		}

		case TV_HELM:
		{
			if (o_ptr->sval == SV_DRAGON_HELM)
			{
				/* Rating boost */
				rating += 5;

				/* Mention the item */
				if (cheat_peek) object_mention(o_ptr);
				dragon_resist(o_ptr);
				if (!one_in_(3)) break;
			}

			/* Very good */
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				while (1)
				{
					bool ok_flag = TRUE;
					o_ptr->name2 = get_random_ego(INVEN_HEAD, TRUE, level);

					switch (o_ptr->name2)
					{
					case EGO_INTELLIGENCE:
					case EGO_WISDOM:
					case EGO_BEAUTY:
					case EGO_LITE:
					case EGO_INFRAVISION:
						break;
					case EGO_SEEING:
						if (one_in_(7)) o_ptr->art_flags3 |= TR3_TELEPATHY;
						break;
					default:/* not existing helm (Magi, Might, etc...)*/
						ok_flag = FALSE;
					}
					if (ok_flag)
						break; /* while (1) */
				}
				break;
			}
			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_HEAD, FALSE, level);
			}
			break;
		}

		case TV_CLOAK:
		{
			/* Very good */
			if (power > 1)
			{
				if (one_in_(20))
				{
					create_artifact(o_ptr, FALSE);
					break;
				}
				o_ptr->name2 = get_random_ego(INVEN_OUTER, TRUE, level);

				switch (o_ptr->name2)
				{
				case EGO_BAT:
					o_ptr->to_d -= 6;
					o_ptr->to_h -= 6;
					break;
				}

			}

			/* Very cursed */
			else if (power < -1)
			{
				o_ptr->name2 = get_random_ego(INVEN_OUTER, FALSE, level);
			}

			break;
		}
	}
}


/*
 * Apply magic to an item known to be a "ring" or "amulet"
 *
 * Hack -- note special rating boost for ring of speed
 * Hack -- note special rating boost for amulet of the magi
 * Hack -- note special "pval boost" code for ring of speed
 * Hack -- note that some items must be cursed (or blessed)
 */
static void a_m_aux_3(object_type *o_ptr, int level, int power)
{
	/* Apply magic (good or bad) according to type */
	switch (o_ptr->tval)
	{
		case TV_RING:
		{
			/* Analyze */
			switch (o_ptr->sval)
			{
				case SV_RING_ATTACKS:
				{
					/* Stat bonus */
					o_ptr->pval = m_bonus(2, level);
					if (one_in_(15)) o_ptr->pval++;
					if (o_ptr->pval < 1) o_ptr->pval = 1;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				case SV_RING_SHOTS:
				{
					break;
				}

				/* Strength, Constitution, Dexterity, Intelligence */
				case SV_RING_STR:
				case SV_RING_CON:
				case SV_RING_DEX:
				{
					/* Stat bonus */
					o_ptr->pval = 1 + m_bonus(5, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Ring of Speed! */
				case SV_RING_SPEED:
				{
					/* Base speed (1 to 10) */
					o_ptr->pval = randint1(5) + m_bonus(5, level);

					/* Super-charge the ring */
					while (randint0(100) < 50) o_ptr->pval++;

					/* Cursed Ring */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);

						break;
					}

					/* Rating boost */
					rating += 25;

					/* Mention the item */
					if (cheat_peek) object_mention(o_ptr);

					break;
				}

				case SV_RING_LORDLY:
				{
					do
					{
						one_lordly_high_resistance(o_ptr);
					}
					while (one_in_(4));

					/* Bonus to armor class */
					o_ptr->to_a = 10 + randint1(5) + m_bonus(10, level);
					rating += 15;
				}
				break;

				/* Searching */
				case SV_RING_SEARCHING:
				{
					/* Bonus to searching */
					o_ptr->pval = 1 + m_bonus(5, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Flames, Acid, Ice */
				case SV_RING_FLAMES:
				case SV_RING_ACID:
				case SV_RING_ICE:
				case SV_RING_ELEC:
				{
					/* Bonus to armor class */
					o_ptr->to_a = 5 + randint1(5) + m_bonus(10, level);
					break;
				}

				/* Weakness, Stupidity */
				case SV_RING_WEAKNESS:
				case SV_RING_STUPIDITY:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= TRC_CURSED;

					/* Penalize */
					o_ptr->pval = 0 - (1 + m_bonus(5, level));
					if (power > 0) power = 0 - power;

					break;
				}

				/* WOE, Stupidity */
				case SV_RING_WOE:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= TRC_CURSED;

					/* Penalize */
					o_ptr->to_a = 0 - (5 + m_bonus(10, level));
					o_ptr->pval = 0 - (1 + m_bonus(5, level));
					if (power > 0) power = 0 - power;

					break;
				}

				/* Ring of damage */
				case SV_RING_DAMAGE:
				{
					/* Bonus to damage */
					o_ptr->to_d = 1 + randint1(5) + m_bonus(16, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse bonus */
						o_ptr->to_d = 0 - o_ptr->to_d;
					}

					break;
				}

				/* Ring of Accuracy */
				case SV_RING_ACCURACY:
				{
					/* Bonus to hit */
					o_ptr->to_h = 1 + randint1(5) + m_bonus(16, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse tohit */
						o_ptr->to_h = 0 - o_ptr->to_h;
					}

					break;
				}

				/* Ring of Protection */
				case SV_RING_PROTECTION:
				{
					/* Bonus to armor class */
					o_ptr->to_a = 5 + randint1(8) + m_bonus(10, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse toac */
						o_ptr->to_a = 0 - o_ptr->to_a;
					}

					break;
				}

				/* Ring of Slaying */
				case SV_RING_SLAYING:
				{
					/* Bonus to damage and to hit */
					o_ptr->to_d = randint1(5) + m_bonus(12, level);
					o_ptr->to_h = randint1(5) + m_bonus(12, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse bonuses */
						o_ptr->to_h = 0 - o_ptr->to_h;
						o_ptr->to_d = 0 - o_ptr->to_d;
					}

					break;
				}

				case SV_RING_MUSCLE:
				{
					o_ptr->pval = 1 + m_bonus(3, level);
					if (one_in_(4)) o_ptr->pval++;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= TRC_CURSED;

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}
				case SV_RING_AGGRAVATION:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= TRC_CURSED;

					if (power > 0) power = 0 - power;
					break;
				}
			}
			if (one_in_(400) && (power > 0) && !cursed_p(o_ptr) && (level > 79))
			{
				o_ptr->pval = MIN(o_ptr->pval,4);
				/* Randart amulet */
				create_artifact(o_ptr, FALSE);
			}
			else if ((power == 2) && one_in_(2))
			{
				while(!o_ptr->name2)
				{
					int tmp = m_bonus(10, level);
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(28))
					{
					case 1: case 2:
						o_ptr->name2 = EGO_RING_THROW;
						break;
					case 3: case 4:
						if (k_ptr->flags3 & TR3_REGEN) break;
						o_ptr->name2 = EGO_RING_REGEN;
						break;
					case 5: case 6:
						if (k_ptr->flags3 & TR3_LITE) break;
						o_ptr->name2 = EGO_RING_LITE;
						break;
					case 7: case 8:
						if (k_ptr->flags2 & TR3_TELEPORT) break;
						o_ptr->name2 = EGO_RING_TELEPORT;
						break;
					case 9: case 10:
						if (o_ptr->to_h) break;
						o_ptr->name2 = EGO_RING_TO_H;
						break;
					case 11: case 12:
						if (o_ptr->to_d) break;
						o_ptr->name2 = EGO_RING_TO_D;
						break;
					case 13:
						if ((o_ptr->to_h) || (o_ptr->to_d)) break;
						o_ptr->name2 = EGO_RING_SLAY;
						break;
					case 14:
						if ((k_ptr->flags1 & TR1_STR) || o_ptr->to_h || o_ptr->to_d) break;
						o_ptr->name2 = EGO_RING_WIZARD;
						break;
					case 15:
						if (k_ptr->flags3 & TR3_ACTIVATE) break;
						o_ptr->name2 = EGO_RING_HERO;
						break;
					case 16:
						if (k_ptr->flags3 & TR3_ACTIVATE) break;
						if (tmp > 8) o_ptr->name2 = EGO_RING_MANA_BALL;
						else if (tmp > 4) o_ptr->name2 = EGO_RING_MANA_BOLT;
						else o_ptr->name2 = EGO_RING_MAGIC_MIS;
						break;
					case 17:
						if (k_ptr->flags3 & TR3_ACTIVATE) break;
						if (!(k_ptr->flags2 & TR2_RES_FIRE) && (k_ptr->flags2 & (TR2_RES_COLD | TR2_RES_ELEC | TR2_RES_ACID))) break;
						if (tmp > 7) o_ptr->name2 = EGO_RING_DRAGON_F;
						else if (tmp > 3) o_ptr->name2 = EGO_RING_FIRE_BALL;
						else o_ptr->name2 = EGO_RING_FIRE_BOLT;
						break;
					case 18:
						if (k_ptr->flags3 & TR3_ACTIVATE) break;
						if (!(k_ptr->flags2 & TR2_RES_COLD) && (k_ptr->flags2 & (TR2_RES_FIRE | TR2_RES_ELEC | TR2_RES_ACID))) break;
						if (tmp > 7) o_ptr->name2 = EGO_RING_DRAGON_C;
						else if (tmp > 3) o_ptr->name2 = EGO_RING_COLD_BALL;
						else o_ptr->name2 = EGO_RING_COLD_BOLT;
						break;
					case 19:
						if (k_ptr->flags3 & TR3_ACTIVATE) break;
						if (!(k_ptr->flags2 & TR2_RES_ELEC) && (k_ptr->flags2 & (TR2_RES_COLD | TR2_RES_FIRE | TR2_RES_ACID))) break;
						if (tmp > 4) o_ptr->name2 = EGO_RING_ELEC_BALL;
						else o_ptr->name2 = EGO_RING_ELEC_BOLT;
						break;
					case 20:
						if (k_ptr->flags3 & TR3_ACTIVATE) break;
						if (!(k_ptr->flags2 & TR2_RES_ACID) && (k_ptr->flags2 & (TR2_RES_COLD | TR2_RES_ELEC | TR2_RES_FIRE))) break;
						if (tmp > 4) o_ptr->name2 = EGO_RING_ACID_BALL;
						else o_ptr->name2 = EGO_RING_ACID_BOLT;
						break;
					case 21: case 22: case 23: case 24: case 25: case 26:
						switch (o_ptr->sval)
						{
						case SV_RING_SPEED:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_RING_D_SPEED;
							break;
						case SV_RING_DAMAGE:
						case SV_RING_ACCURACY:
						case SV_RING_SLAYING:
							if (one_in_(2)) break;
							if (one_in_(2)) o_ptr->name2 = EGO_RING_HERO;
							else
							{
								o_ptr->name2 = EGO_RING_BERSERKER;
								o_ptr->to_h -= 2+randint1(4);
							}
							break;
						case SV_RING_PROTECTION:
							o_ptr->name2 = EGO_RING_SUPER_AC;
							o_ptr->to_a += 7 + m_bonus(5, level);
							break;
						case SV_RING_RES_FEAR:
							o_ptr->name2 = EGO_RING_HERO;
							break;
						case SV_RING_SHOTS:
							if (one_in_(2)) break;
							o_ptr->name2 = EGO_RING_HUNTER;
							break;
						case SV_RING_SEARCHING:
							o_ptr->name2 = EGO_RING_STEALTH;
							break;
						case SV_RING_TELEPORTATION:
							o_ptr->name2 = EGO_RING_TELE_AWAY;
							break;
						case SV_RING_RES_BLINDNESS:
							if (one_in_(2))
								o_ptr->name2 = EGO_RING_RES_LITE;
							else
								o_ptr->name2 = EGO_RING_RES_DARK;
							break;
						case SV_RING_LORDLY:
							if (!one_in_(20)) break;
							one_lordly_high_resistance(o_ptr);
							one_lordly_high_resistance(o_ptr);
							o_ptr->name2 = EGO_RING_TRUE;
							break;
						case SV_RING_SUSTAIN:
							if (!one_in_(4)) break;
							o_ptr->name2 = EGO_RING_RES_TIME;
							break;
						case SV_RING_FLAMES:
							if (!one_in_(2)) break;
							o_ptr->name2 = EGO_RING_DRAGON_F;
							break;
						case SV_RING_ICE:
							if (!one_in_(2)) break;
							o_ptr->name2 = EGO_RING_DRAGON_C;
							break;
						case SV_RING_WARNING:
							if (!one_in_(2)) break;
							o_ptr->name2 = EGO_RING_M_DETECT;
							break;
						default:
							break;
						}
						break;
					}
				}
				/* Uncurse it */
				o_ptr->curse_flags = 0L;
			}
			else if ((power == -2) && one_in_(2))
			{
				if (o_ptr->to_h > 0) o_ptr->to_h = 0-o_ptr->to_h;
				if (o_ptr->to_d > 0) o_ptr->to_d = 0-o_ptr->to_d;
				if (o_ptr->to_a > 0) o_ptr->to_a = 0-o_ptr->to_a;
				if (o_ptr->pval > 0) o_ptr->pval = 0-o_ptr->pval;
				o_ptr->art_flags1 = 0;
				o_ptr->art_flags2 = 0;
				while(!o_ptr->name2)
				{
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(5))
					{
					case 1:
						if (k_ptr->flags3 & TR3_DRAIN_EXP) break;
						o_ptr->name2 = EGO_RING_DRAIN_EXP;
						break;
					case 2:
						o_ptr->name2 = EGO_RING_NO_MELEE;
						break;
					case 3:
						if (k_ptr->flags3 & TR3_AGGRAVATE) break;
						o_ptr->name2 = EGO_RING_AGGRAVATE;
						break;
					case 4:
						if (k_ptr->flags3 & TR3_TY_CURSE) break;
						o_ptr->name2 = EGO_RING_TY_CURSE;
						break;
					case 5:
						o_ptr->name2 = EGO_RING_ALBINO;
						break;
					}
				}
				/* Broken */
				o_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				o_ptr->curse_flags |= (TRC_CURSED | TRC_HEAVY_CURSE);
			}
			break;
		}

		case TV_AMULET:
		{
			/* Analyze */
			switch (o_ptr->sval)
			{
				/* Amulet of wisdom/charisma */
				case SV_AMULET_INTELLIGENCE:
				case SV_AMULET_WISDOM:
				case SV_AMULET_CHARISMA:
				{
					o_ptr->pval = 1 + m_bonus(5, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}

				/* Amulet of brilliance */
				case SV_AMULET_BRILLIANCE:
				{
					o_ptr->pval = 1 + m_bonus(3, level);
					if (one_in_(4)) o_ptr->pval++;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}

				case SV_AMULET_NO_MAGIC: case SV_AMULET_NO_TELE:
				{
					if (power < 0)
					{
						o_ptr->curse_flags |= (TRC_CURSED);
					}
					break;
				}

				case SV_AMULET_RESISTANCE:
				{
					if (one_in_(5)) one_high_resistance(o_ptr);
					if (one_in_(5)) o_ptr->art_flags2 |= TR2_RES_POIS;
				}
				break;

				/* Amulet of searching */
				case SV_AMULET_SEARCHING:
				{
					o_ptr->pval = randint1(2) + m_bonus(4, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Amulet of the Magi -- never cursed */
				case SV_AMULET_THE_MAGI:
				{
					o_ptr->pval = randint1(5) + m_bonus(5, level);
					o_ptr->to_a = randint1(5) + m_bonus(5, level);

					/* Boost the rating */
					rating += 15;

					/* Mention the item */
					if (cheat_peek) object_mention(o_ptr);

					break;
				}

				/* Amulet of Doom -- always cursed */
				case SV_AMULET_DOOM:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->curse_flags |= (TRC_CURSED);

					/* Penalize */
					o_ptr->pval = 0 - (randint1(5) + m_bonus(5, level));
					o_ptr->to_a = 0 - (randint1(5) + m_bonus(5, level));
					if (power > 0) power = 0 - power;

					break;
				}

				case SV_AMULET_MAGIC_MASTERY:
				{
					o_ptr->pval = 1 + m_bonus(4, level);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->curse_flags |= (TRC_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - o_ptr->pval;
					}

					break;
				}
			}
			if (one_in_(150) && (power > 0) && !cursed_p(o_ptr) && (level > 79))
			{
				o_ptr->pval = MIN(o_ptr->pval,4);
				/* Randart amulet */
				create_artifact(o_ptr, FALSE);
			}
			else if ((power == 2) && one_in_(2))
			{
				while(!o_ptr->name2)
				{
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(21))
					{
					case 1: case 2:
						if (k_ptr->flags3 & TR3_SLOW_DIGEST) break;
						o_ptr->name2 = EGO_AMU_SLOW_D;
						break;
					case 3: case 4:
						if (o_ptr->pval) break;
						o_ptr->name2 = EGO_AMU_INFRA;
						break;
					case 5: case 6:
						if (k_ptr->flags3 & TR3_SEE_INVIS) break;
						o_ptr->name2 = EGO_AMU_SEE_INVIS;
						break;
					case 7: case 8:
						if (k_ptr->flags2 & TR2_HOLD_LIFE) break;
						o_ptr->name2 = EGO_AMU_HOLD_LIFE;
						break;
					case 9:
						if (k_ptr->flags3 & TR3_FEATHER) break;
						o_ptr->name2 = EGO_AMU_LEVITATION;
						break;
					case 10: case 11: case 21:
						o_ptr->name2 = EGO_AMU_AC;
						break;
					case 12:
						if (k_ptr->flags2 & TR2_RES_FIRE) break;
						if (m_bonus(10, level) > 8)
							o_ptr->name2 = EGO_AMU_RES_FIRE_;
						else
							o_ptr->name2 = EGO_AMU_RES_FIRE;
						break;
					case 13:
						if (k_ptr->flags2 & TR2_RES_COLD) break;
						if (m_bonus(10, level) > 8)
							o_ptr->name2 = EGO_AMU_RES_COLD_;
						else
							o_ptr->name2 = EGO_AMU_RES_COLD;
						break;
					case 14:
						if (k_ptr->flags2 & TR2_RES_ELEC) break;
						if (m_bonus(10, level) > 8)
							o_ptr->name2 = EGO_AMU_RES_ELEC_;
						else
							o_ptr->name2 = EGO_AMU_RES_ELEC;
						break;
					case 15:
						if (k_ptr->flags2 & TR2_RES_ACID) break;
						if (m_bonus(10, level) > 8)
							o_ptr->name2 = EGO_AMU_RES_ACID_;
						else
							o_ptr->name2 = EGO_AMU_RES_ACID;
						break;
					case 16: case 17: case 18: case 19: case 20:
						switch (o_ptr->sval)
						{
						case SV_AMULET_TELEPORT:
							if (m_bonus(10, level) > 9) o_ptr->name2 = EGO_AMU_D_DOOR;
							else if (one_in_(2)) o_ptr->name2 = EGO_AMU_JUMP;
							else o_ptr->name2 = EGO_AMU_TELEPORT;
							break;
						case SV_AMULET_RESIST_ACID:
							if ((m_bonus(10, level) > 6) && one_in_(2)) o_ptr->name2 = EGO_AMU_RES_ACID_;
							break;
						case SV_AMULET_SEARCHING:
							o_ptr->name2 = EGO_AMU_STEALTH;
							break;
						case SV_AMULET_BRILLIANCE:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_AMU_IDENT;
							break;
						case SV_AMULET_CHARISMA:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_AMU_CHARM;
							break;
						case SV_AMULET_THE_MAGI:
							if (one_in_(2)) break;
							o_ptr->name2 = EGO_AMU_GREAT;
							break;
						case SV_AMULET_RESISTANCE:
							if (!one_in_(5)) break;
							o_ptr->name2 = EGO_AMU_DEFENDER;
							break;
						case SV_AMULET_TELEPATHY:
							if (!one_in_(3)) break;
							o_ptr->name2 = EGO_AMU_DETECTION;
							break;
						}
					}
				}
				/* Uncurse it */
				o_ptr->curse_flags = 0L;
			}
			else if ((power == -2) && one_in_(2))
			{
				if (o_ptr->to_h > 0) o_ptr->to_h = 0-o_ptr->to_h;
				if (o_ptr->to_d > 0) o_ptr->to_d = 0-o_ptr->to_d;
				if (o_ptr->to_a > 0) o_ptr->to_a = 0-o_ptr->to_a;
				if (o_ptr->pval > 0) o_ptr->pval = 0-o_ptr->pval;
				o_ptr->art_flags1 = 0;
				o_ptr->art_flags2 = 0;
				while(!o_ptr->name2)
				{
					object_kind *k_ptr = &k_info[o_ptr->k_idx];
					switch(randint1(5))
					{
					case 1:
						if (k_ptr->flags3 & TR3_DRAIN_EXP) break;
						o_ptr->name2 = EGO_AMU_DRAIN_EXP;
						break;
					case 2:
						o_ptr->name2 = EGO_AMU_FOOL;
						break;
					case 3:
						if (k_ptr->flags3 & TR3_AGGRAVATE) break;
						o_ptr->name2 = EGO_AMU_AGGRAVATE;
						break;
					case 4:
						if (k_ptr->flags3 & TR3_TY_CURSE) break;
						o_ptr->name2 = EGO_AMU_TY_CURSE;
						break;
					case 5:
						o_ptr->name2 = EGO_AMU_NAIVETY;
						break;
					}
				}
				/* Broken */
				o_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				o_ptr->curse_flags |= (TRC_CURSED | TRC_HEAVY_CURSE);
			}
			break;
		}
	}
}


/*
 * Hack -- help pick an item type
 */
static bool item_monster_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	/* No uniques */
	if (r_ptr->flags1 & RF1_UNIQUE) return (FALSE);
	if (r_ptr->flags7 & RF7_KAGE) return (FALSE);
	if (r_ptr->flags3 & RF3_RES_ALL) return (FALSE);
	if (r_ptr->flags7 & RF7_UNIQUE_7) return (FALSE);
	if (r_ptr->flags1 & RF1_FORCE_DEPTH) return (FALSE);
	if (r_ptr->flags7 & RF7_UNIQUE2) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Apply magic to an item known to be "boring"
 *
 * Hack -- note the special code for various items
 */
static void a_m_aux_4(object_type *o_ptr, int level, int power)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Apply magic (good or bad) according to type */
	switch (o_ptr->tval)
	{
		case TV_WHISTLE:
		{
#if 0
			/* Cursed */
			if (power < 0)
			{
				/* Broken */
				o_ptr->ident |= (IDENT_BROKEN);

				/* Cursed */
				o_ptr->curse_flags |= (TRC_CURSED);
			}
#endif
			break;
		}
		case TV_FLASK:
		{
			o_ptr->xtra4 = o_ptr->pval;
			o_ptr->pval = 0;
			break;
		}
		case TV_LITE:
		{
			/* Hack -- Torches -- random fuel */
			if (o_ptr->sval == SV_LITE_TORCH)
			{
				if (o_ptr->pval > 0) o_ptr->xtra4 = randint1(o_ptr->pval);
				o_ptr->pval = 0;
			}

			/* Hack -- Lanterns -- random fuel */
			if (o_ptr->sval == SV_LITE_LANTERN)
			{
				if (o_ptr->pval > 0) o_ptr->xtra4 = randint1(o_ptr->pval);
				o_ptr->pval = 0;
			}

			if ((power == 2) || ((power == 1) && one_in_(3)))
			{
				while (!o_ptr->name2)
				{
					while (1)
					{
						bool okay_flag = TRUE;

						o_ptr->name2 = get_random_ego(INVEN_LITE, TRUE, level);

						switch (o_ptr->name2)
						{
						case EGO_LITE_LONG:
							if (o_ptr->sval == SV_LITE_FEANOR)
								okay_flag = FALSE;
						}
						if (okay_flag)
							break;
					}
				}
			}
			else if (power == -2)
			{
				o_ptr->name2 = get_random_ego(INVEN_LITE, FALSE, level);

				switch (o_ptr->name2)
				{
				case EGO_LITE_DARKNESS:
					o_ptr->xtra4 = 0;
					break;
				}
			}

			break;
		}

		case TV_WAND:
		case TV_STAFF:
		{
			/* The wand or staff gets a number of initial charges equal
			 * to between 1/2 (+1) and the full object kind's pval. -LM-
			 */
			o_ptr->pval = k_ptr->pval / 2 + randint1((k_ptr->pval + 1) / 2);
			break;
		}

		case TV_ROD:
		{
			/* Transfer the pval. -LM- */
			o_ptr->pval = k_ptr->pval;
			break;
		}

		case TV_CAPTURE:
		{
			o_ptr->pval = 0;
			object_aware(o_ptr);
			object_known(o_ptr);
			break;
		}

		case TV_FIGURINE:
		{
			int i = 1;
			int check;

			monster_race *r_ptr;

			/* Pick a random non-unique monster race */
			while (1)
			{
				i = randint1(max_r_idx - 1);

				if (!item_monster_okay(i)) continue;
				if (i == MON_TSUCHINOKO) continue;

				r_ptr = &r_info[i];

				check = (dun_level < r_ptr->level) ? (r_ptr->level - dun_level) : 0;

				/* Ignore dead monsters */
				if (!r_ptr->rarity) continue;

				/* Prefer less out-of-depth monsters */
				if (randint0(check)) continue;

				break;
			}

			o_ptr->pval = i;

			/* Some figurines are cursed */
			if (one_in_(6)) o_ptr->curse_flags |= TRC_CURSED;

			if (cheat_peek)
			{
#ifdef JP
				msg_format("%s�οͷ�, ���� +%d%s",
#else
				msg_format("Figurine of %s, depth +%d%s",
#endif

							  r_name + r_ptr->name, check - 1,
							  !cursed_p(o_ptr) ? "" : " {cursed}");
			}

			break;
		}

		case TV_CORPSE:
		{
			int i = 1;
			int check;

			u32b match = 0;

			monster_race *r_ptr;

			if (o_ptr->sval == SV_SKELETON)
			{
				match = RF9_DROP_SKELETON;
			}
			else if (o_ptr->sval == SV_CORPSE)
			{
				match = RF9_DROP_CORPSE;
			}

			/* Hack -- Remove the monster restriction */
			get_mon_num_prep(item_monster_okay, NULL);

			/* Pick a random non-unique monster race */
			while (1)
			{
				i = get_mon_num(dun_level);

				r_ptr = &r_info[i];

				check = (dun_level < r_ptr->level) ? (r_ptr->level - dun_level) : 0;

				/* Ignore dead monsters */
				if (!r_ptr->rarity) continue;

				/* Ignore corpseless monsters */
				if (!(r_ptr->flags9 & match)) continue;

				/* Prefer less out-of-depth monsters */
				if (randint0(check)) continue;

				break;
			}

			o_ptr->pval = i;

			if (cheat_peek)
			{
#ifdef JP
				msg_format("%s�λ���,���� +%d",
#else
				msg_format("Corpse of %s, depth +%d",
#endif

							  r_name + r_ptr->name, check - 1);
			}

			object_aware(o_ptr);
			object_known(o_ptr);
			break;
		}

		case TV_STATUE:
		{
			int i = 1;

			monster_race *r_ptr;

			/* Pick a random monster race */
			while (1)
			{
				i = randint1(max_r_idx - 1);

				r_ptr = &r_info[i];

				/* Ignore dead monsters */
				if (!r_ptr->rarity) continue;

				break;
			}

			o_ptr->pval = i;

			if (cheat_peek)
			{
#ifdef JP
				msg_format("%s����,", r_name + r_ptr->name);
#else
				msg_format("Statue of %s", r_name + r_ptr->name);
#endif

			}
			object_aware(o_ptr);
			object_known(o_ptr);

			break;
		}

		case TV_CHEST:
		{
			byte obj_level = get_object_level(o_ptr);

			/* Hack -- skip ruined chests */
			if (obj_level <= 0) break;

			/* Hack -- pick a "difficulty" */
			o_ptr->pval = randint1(obj_level);
			if (o_ptr->sval == SV_CHEST_KANDUME) o_ptr->pval = 6;

			o_ptr->xtra3 = dun_level + 5;

			/* Never exceed "difficulty" of 55 to 59 */
			if (o_ptr->pval > 55) o_ptr->pval = 55 + (byte)randint0(5);

			break;
		}
	}
}


/*
 * Complete the "creation" of an object by applying "magic" to the item
 *
 * This includes not only rolling for random bonuses, but also putting the
 * finishing touches on ego-items and artifacts, giving charges to wands and
 * staffs, giving fuel to lites, and placing traps on chests.
 *
 * In particular, note that "Instant Artifacts", if "created" by an external
 * routine, must pass through this function to complete the actual creation.
 *
 * The base "chance" of the item being "good" increases with the "level"
 * parameter, which is usually derived from the dungeon level, being equal
 * to the level plus 10, up to a maximum of 75.  If "good" is true, then
 * the object is guaranteed to be "good".  If an object is "good", then
 * the chance that the object will be "great" (ego-item or artifact), also
 * increases with the "level", being equal to half the level, plus 5, up to
 * a maximum of 20.  If "great" is true, then the object is guaranteed to be
 * "great".  At dungeon level 65 and below, 15/100 objects are "great".
 *
 * If the object is not "good", there is a chance it will be "cursed", and
 * if it is "cursed", there is a chance it will be "broken".  These chances
 * are related to the "good" / "great" chances above.
 *
 * Otherwise "normal" rings and amulets will be "good" half the time and
 * "cursed" half the time, unless the ring/amulet is always good or cursed.
 *
 * If "okay" is true, and the object is going to be "great", then there is
 * a chance that an artifact will be created.  This is true even if both the
 * "good" and "great" arguments are false.  As a total hack, if "great" is
 * true, then the item gets 3 extra "attempts" to become an artifact.
 */
void apply_magic(object_type *o_ptr, int lev, bool okay, bool good, bool great, bool curse)
{

	int i, rolls, f1, f2, power;

	if (p_ptr->pseikaku == SEIKAKU_MUNCHKIN) lev += randint0(p_ptr->lev/2+10);

	/* Maximum "level" for various things */
	if (lev > MAX_DEPTH - 1) lev = MAX_DEPTH - 1;

	/* Base chance of being "good" */
	f1 = lev + 10;

	/* Maximal chance of being "good" */
	if (f1 > d_info[dungeon_type].obj_good) f1 = d_info[dungeon_type].obj_good;

	/* Base chance of being "great" */
	f2 = f1 / 2;

	/* Maximal chance of being "great" */
	if ((p_ptr->pseikaku != SEIKAKU_MUNCHKIN) && (f2 > d_info[dungeon_type].obj_great))
		f2 = d_info[dungeon_type].obj_great;

	if (p_ptr->muta3 & MUT3_GOOD_LUCK)
	{
		f1 += 5;
		f2 += 2;
	}
	else if(p_ptr->muta3 & MUT3_BAD_LUCK)
	{
		f1 -= 5;
		f2 -= 2;
	}

	/* Assume normal */
	power = 0;

	/* Roll for "good" */
	if (good || magik(f1))
	{
		/* Assume "good" */
		power = 1;

		/* Roll for "great" */
		if (great || magik(f2)) power = 2;
	}

	/* Roll for "cursed" */
	else if (magik(f1))
	{
		/* Assume "cursed" */
		power = -1;

		/* Roll for "broken" */
		if (magik(f2)) power = -2;
	}

	/* Apply curse */
	if (curse)
	{
		/* Assume 'cursed' */
		if (power > 0)
		{
			power = 0 - power;
		}
		/* Everything else gets more badly cursed */
		else
		{
			power--;
		}
	}

	/* Assume no rolls */
	rolls = 0;

	/* Get one roll if excellent */
	if (power >= 2) rolls = 1;

	/* Hack -- Get four rolls if forced great */
	if (great) rolls = 4;

	/* Hack -- Get no rolls if not allowed */
	if (!okay || o_ptr->name1) rolls = 0;

	/* Roll for artifacts if allowed */
	for (i = 0; i < rolls; i++)
	{
		/* Roll for an artifact */
		if (make_artifact(o_ptr)) break;
		if ((p_ptr->muta3 & MUT3_GOOD_LUCK) && one_in_(77))
		{
			if (make_artifact(o_ptr)) break;
		}
	}


	/* Hack -- analyze artifacts */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Hack -- Mark the artifact as "created" */
		a_ptr->cur_num = 1;

		/* Extract the other fields */
		o_ptr->pval = a_ptr->pval;
		o_ptr->ac = a_ptr->ac;
		o_ptr->dd = a_ptr->dd;
		o_ptr->ds = a_ptr->ds;
		o_ptr->to_a = a_ptr->to_a;
		o_ptr->to_h = a_ptr->to_h;
		o_ptr->to_d = a_ptr->to_d;
		o_ptr->weight = a_ptr->weight;

		/* Hack -- extract the "broken" flag */
		if (!a_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- extract the "cursed" flag */
		if (a_ptr->gen_flags & TRG_CURSED) o_ptr->curse_flags |= (TRC_CURSED);
		if (a_ptr->gen_flags & TRG_HEAVY_CURSE) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		if (a_ptr->gen_flags & TRG_PERMA_CURSE) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
		if (a_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
		if (a_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
		if (a_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);

		/* Mega-Hack -- increase the rating */
		rating += 10;

		/* Mega-Hack -- increase the rating again */
		if (a_ptr->cost > 50000L) rating += 10;

		/* Mega-Hack -- increase the rating again */
		if (a_ptr->cost > 100000L) rating += 10;

		/* Set the good item flag */
		good_item_flag = TRUE;

		/* Cheat -- peek at the item */
		if (cheat_peek) object_mention(o_ptr);

		/* Done */
		return;
	}


	/* Apply magic */
	switch (o_ptr->tval)
	{
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_BOW:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (power) a_m_aux_1(o_ptr, lev, power);
			break;
		}

		case TV_POLEARM:
		{
			if (power && !(o_ptr->sval == SV_DEATH_SCYTHE)) a_m_aux_1(o_ptr, lev, power);
			break;
		}

		case TV_SWORD:
		{
			if (power && !(o_ptr->sval == SV_DOKUBARI)) a_m_aux_1(o_ptr, lev, power);
			break;
		}

		case TV_DRAG_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_SHIELD:
		case TV_HELM:
		case TV_CROWN:
		case TV_CLOAK:
		case TV_GLOVES:
		case TV_BOOTS:
		{
			/* Elven Cloak and Black Clothes ... */
			if (((o_ptr->tval == TV_CLOAK) && (o_ptr->sval == SV_ELVEN_CLOAK)) ||
			    ((o_ptr->tval == TV_SOFT_ARMOR) && (o_ptr->sval == SV_KUROSHOUZOKU)))
				o_ptr->pval = randint1(4);

#if 1
			if (power ||
			     ((o_ptr->tval == TV_HELM) && (o_ptr->sval == SV_DRAGON_HELM)) ||
			     ((o_ptr->tval == TV_SHIELD) && (o_ptr->sval == SV_DRAGON_SHIELD)) ||
			     ((o_ptr->tval == TV_GLOVES) && (o_ptr->sval == SV_SET_OF_DRAGON_GLOVES)) ||
			     ((o_ptr->tval == TV_BOOTS) && (o_ptr->sval == SV_PAIR_OF_DRAGON_GREAVE)))
				a_m_aux_2(o_ptr, lev, power);
#else
			if (power) a_m_aux_2(o_ptr, lev, power);
#endif
			break;
		}

		case TV_RING:
		case TV_AMULET:
		{
			if (!power && (randint0(100) < 50)) power = -1;
			a_m_aux_3(o_ptr, lev, power);
			break;
		}

		default:
		{
			a_m_aux_4(o_ptr, lev, power);
			break;
		}
	}

	if ((o_ptr->tval == TV_SOFT_ARMOR) &&
	    (o_ptr->sval == SV_ABUNAI_MIZUGI) &&
	    (p_ptr->pseikaku == SEIKAKU_SEXY))
	{
		o_ptr->pval = 3;
		o_ptr->art_flags1 |= (TR1_STR | TR1_INT | TR1_WIS | TR1_DEX | TR1_CON | TR1_CHR);
	}

	if (o_ptr->art_name) rating += 30;

	/* Hack -- analyze ego-items */
	else if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Hack -- acquire "broken" flag */
		if (!e_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- acquire "cursed" flag */
		if (e_ptr->gen_flags & TRG_CURSED) o_ptr->curse_flags |= (TRC_CURSED);
		if (e_ptr->gen_flags & TRG_HEAVY_CURSE) o_ptr->curse_flags |= (TRC_HEAVY_CURSE);
		if (e_ptr->gen_flags & TRG_PERMA_CURSE) o_ptr->curse_flags |= (TRC_PERMA_CURSE);
		if (e_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
		if (e_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
		if (e_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);

		if (e_ptr->gen_flags & (TRG_ONE_SUSTAIN)) one_sustain(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_POWER)) one_ability(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_H_RES)) one_high_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_E_RES)) one_ele_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_D_RES)) one_dragon_ele_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_L_RES)) one_lordly_high_resistance(o_ptr);
		if (e_ptr->gen_flags & (TRG_XTRA_RES)) one_resistance(o_ptr);

		/* Hack -- apply extra penalties if needed */
		if (cursed_p(o_ptr) || broken_p(o_ptr))
		{
			/* Hack -- obtain bonuses */
			if (e_ptr->max_to_h) o_ptr->to_h -= randint1(e_ptr->max_to_h);
			if (e_ptr->max_to_d) o_ptr->to_d -= randint1(e_ptr->max_to_d);
			if (e_ptr->max_to_a) o_ptr->to_a -= randint1(e_ptr->max_to_a);

			/* Hack -- obtain pval */
			if (e_ptr->max_pval) o_ptr->pval -= randint1(e_ptr->max_pval);
		}

		/* Hack -- apply extra bonuses if needed */
		else
		{
			/* Hack -- obtain bonuses */
			if (e_ptr->max_to_h)
			{
				if (e_ptr->max_to_h > 127)
					o_ptr->to_h -= randint1(256-e_ptr->max_to_h);
				else o_ptr->to_h += randint1(e_ptr->max_to_h);
			}
			if (e_ptr->max_to_d)
			{
				if (e_ptr->max_to_d > 127)
					o_ptr->to_d -= randint1(256-e_ptr->max_to_d);
				else o_ptr->to_d += randint1(e_ptr->max_to_d);
			}
			if (e_ptr->max_to_a)
			{
				if (e_ptr->max_to_a > 127)
					o_ptr->to_a -= randint1(256-e_ptr->max_to_a);
				else o_ptr->to_a += randint1(e_ptr->max_to_a);
			}

			/* Hack -- obtain pval */
			if (e_ptr->max_pval)
			{
				if ((o_ptr->name2 == EGO_HA) && (o_ptr->art_flags1 & TR1_BLOWS))
				{
					o_ptr->pval++;
					if ((lev > 60) && one_in_(3) && ((o_ptr->dd*(o_ptr->ds+1)) < 15)) o_ptr->pval++;
				}
				else if (o_ptr->name2 == EGO_ATTACKS)
				{
					o_ptr->pval = randint1(e_ptr->max_pval*lev/100+1);
					if (o_ptr->pval > 3) o_ptr->pval = 3;
					if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_HAYABUSA))
						o_ptr->pval += randint1(2);
				}
				else if (o_ptr->name2 == EGO_BAT)
				{
					o_ptr->pval = randint1(e_ptr->max_pval);
					if (o_ptr->sval == SV_ELVEN_CLOAK) o_ptr->pval += randint1(2);
				}
				else
				{
					o_ptr->pval += randint1(e_ptr->max_pval);
				}
			}
			if ((o_ptr->name2 == EGO_SPEED) && (lev < 50))
			{
				o_ptr->pval = randint1(o_ptr->pval);
			}
			if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_HAYABUSA) && (o_ptr->pval > 2) && (o_ptr->name2 != EGO_ATTACKS))
				o_ptr->pval = 2;
		}

		/* Hack -- apply rating bonus */
		rating += e_ptr->rating;

		/* Cheat -- describe the item */
		if (cheat_peek) object_mention(o_ptr);

		/* Done */
		return;
	}

	/* Examine real objects */
	if (o_ptr->k_idx)
	{
		object_kind *k_ptr = &k_info[o_ptr->k_idx];

		/* Hack -- acquire "broken" flag */
		if (!get_object_cost(o_ptr)) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- acquire "cursed" flag */
		if (k_ptr->gen_flags & (TRG_CURSED)) o_ptr->curse_flags |= (TRC_CURSED);
		if (k_ptr->gen_flags & (TRG_HEAVY_CURSE)) o_ptr->curse_flags |= TRC_HEAVY_CURSE;
		if (k_ptr->gen_flags & (TRG_PERMA_CURSE)) o_ptr->curse_flags |= TRC_PERMA_CURSE;
		if (k_ptr->gen_flags & (TRG_RANDOM_CURSE0)) o_ptr->curse_flags |= get_curse(0, o_ptr);
		if (k_ptr->gen_flags & (TRG_RANDOM_CURSE1)) o_ptr->curse_flags |= get_curse(1, o_ptr);
		if (k_ptr->gen_flags & (TRG_RANDOM_CURSE2)) o_ptr->curse_flags |= get_curse(2, o_ptr);
	}
}


/*
 * Hack -- determine if a template is "good"
 */
static bool kind_is_good(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	switch (k_ptr->tval)
	{
		/* Armor -- Good unless damaged */
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_HELM:
		case TV_CROWN:
		{
			if (k_ptr->to_a < 0) return (FALSE);
			return (TRUE);
		}

		/* Weapons -- Good unless damaged */
		case TV_BOW:
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		{
			if (k_ptr->to_h < 0) return (FALSE);
			if (k_ptr->to_d < 0) return (FALSE);
			return (TRUE);
		}

		/* Ammo -- Arrows/Bolts are good */
		case TV_BOLT:
		case TV_ARROW:
		{
			return (TRUE);
		}

		/* Books -- High level books are good (except Arcane books) */
		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ENCHANT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		{
			if (k_ptr->sval >= SV_BOOK_MIN_GOOD) return (TRUE);
			return (FALSE);
		}

		/* Rings -- Rings of Speed are good */
		case TV_RING:
		{
			if (k_ptr->sval == SV_RING_SPEED) return (TRUE);
			if (k_ptr->sval == SV_RING_LORDLY) return (TRUE);
			return (FALSE);
		}

		/* Amulets -- Amulets of the Magi and Resistance are good */
		case TV_AMULET:
		{
			if (k_ptr->sval == SV_AMULET_THE_MAGI) return (TRUE);
			if (k_ptr->sval == SV_AMULET_RESISTANCE) return (TRUE);
			return (FALSE);
		}
	}

	/* Assume not good */
	return (FALSE);
}


/*
 * Attempt to make an object (normal or good/great)
 *
 * This routine plays nasty games to generate the "special artifacts".
 *
 * This routine uses "object_level" for the "generation level".
 *
 * We assume that the given object has been "wiped".
 */
bool make_object(object_type *j_ptr, bool good, bool great)
{
	int prob, base;
	byte obj_level;


	/* Chance of "special object" */
	prob = (good ? 10 : 1000);

	/* Base level for the object */
	base = (good ? (object_level + 10) : object_level);


	/* Generate a special object, or a normal object */
	if (!one_in_(prob) || !make_artifact_special(j_ptr))
	{
		int k_idx;

		/* Good objects */
		if (good)
		{
			/* Activate restriction */
			get_obj_num_hook = kind_is_good;

			/* Prepare allocation table */
			get_obj_num_prep();
		}

		/* Pick a random object */
		k_idx = get_obj_num(base);

		/* Good objects */
		if (get_obj_num_hook)
		{
			/* Clear restriction */
			get_obj_num_hook = NULL;

			/* Prepare allocation table */
			get_obj_num_prep();
		}

		/* Handle failure */
		if (!k_idx) return (FALSE);

		/* Prepare the object */
		object_prep(j_ptr, k_idx);
	}

	/* Apply magic (allow artifacts) */
	apply_magic(j_ptr, object_level, TRUE, good, great, FALSE);

	/* Hack -- generate multiple spikes/missiles */
	switch (j_ptr->tval)
	{
		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (!j_ptr->name1)
				j_ptr->number = (byte)damroll(6, 7);
		}
	}

	obj_level = get_object_level(j_ptr);
	if (artifact_p(j_ptr)) obj_level = a_info[j_ptr->name1].level;

	/* Notice "okay" out-of-depth objects */
	if (!cursed_p(j_ptr) && !broken_p(j_ptr) &&
	    (obj_level > dun_level))
	{
		/* Rating increase */
		rating += (obj_level - dun_level);

		/* Cheat -- peek at items */
		if (cheat_peek) object_mention(j_ptr);
	}

	/* Success */
	return (TRUE);
}


/*
 * Attempt to place an object (normal or good/great) at the given location.
 *
 * This routine plays nasty games to generate the "special artifacts".
 *
 * This routine uses "object_level" for the "generation level".
 *
 * This routine requires a clean floor grid destination.
 */
void place_object(int y, int x, bool good, bool great)
{
	s16b o_idx;

	cave_type *c_ptr;

	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- check bounds */
	if (!in_bounds(y, x)) return;

	/* Require clean floor space */
	if (!cave_clean_bold(y, x)) return;


	/* Get local object */
	q_ptr = &forge;

	/* Wipe the object */
	object_wipe(q_ptr);

	/* Make an object (if possible) */
	if (!make_object(q_ptr, good, great)) return;


	/* Make an object */
	o_idx = o_pop();

	/* Success */
	if (o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[o_idx];

		/* Structure Copy */
		object_copy(o_ptr, q_ptr);

		/* Location */
		o_ptr->iy = y;
		o_ptr->ix = x;

		/* Acquire grid */
		c_ptr = &cave[y][x];

		/* Build a stack */
		o_ptr->next_o_idx = c_ptr->o_idx;

		/* Place the object */
		c_ptr->o_idx = o_idx;

		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
	else
	{
		/* Hack -- Preserve artifacts */
		if (q_ptr->name1)
		{
			a_info[q_ptr->name1].cur_num = 0;
		}
	}
}


/*
 * Make a treasure object
 *
 * The location must be a legal, clean, floor grid.
 */
bool make_gold(object_type *j_ptr)
{
	int i;

	s32b base;


	/* Hack -- Pick a Treasure variety */
	i = ((randint1(object_level + 2) + 2) / 2) - 1;

	/* Apply "extra" magic */
	if (one_in_(GREAT_OBJ))
	{
		i += randint1(object_level + 1);
	}

	/* Hack -- Creeping Coins only generate "themselves" */
	if (coin_type) i = coin_type;

	/* Do not create "illegal" Treasure Types */
	if (i >= MAX_GOLD) i = MAX_GOLD - 1;

	/* Prepare a gold object */
	object_prep(j_ptr, OBJ_GOLD_LIST + i);

	/* Hack -- Base coin cost */
	base = k_info[OBJ_GOLD_LIST+i].cost;

	/* Determine how much the treasure is "worth" */
	j_ptr->pval = (base + (8L * randint1(base)) + randint1(8));

	/* Success */
	return (TRUE);
}


/*
 * Places a treasure (Gold or Gems) at given location
 *
 * The location must be a legal, clean, floor grid.
 */
void place_gold(int y, int x)
{
	s16b o_idx;

	cave_type *c_ptr;

	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- check bounds */
	if (!in_bounds(y, x)) return;

	/* Require clean floor space */
	if (!cave_clean_bold(y, x)) return;


	/* Get local object */
	q_ptr = &forge;

	/* Wipe the object */
	object_wipe(q_ptr);

	/* Make some gold */
	if (!make_gold(q_ptr)) return;


	/* Make an object */
	o_idx = o_pop();

	/* Success */
	if (o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[o_idx];

		/* Copy the object */
		object_copy(o_ptr, q_ptr);

		/* Save location */
		o_ptr->iy = y;
		o_ptr->ix = x;

		/* Acquire grid */
		c_ptr = &cave[y][x];

		/* Build a stack */
		o_ptr->next_o_idx = c_ptr->o_idx;

		/* Place the object */
		c_ptr->o_idx = o_idx;

		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}
}


/*
 * Let an object fall to the ground at or near a location.
 *
 * The initial location is assumed to be "in_bounds()".
 *
 * This function takes a parameter "chance".  This is the percentage
 * chance that the item will "disappear" instead of drop.  If the object
 * has been thrown, then this is the chance of disappearance on contact.
 *
 * Hack -- this function uses "chance" to determine if it should produce
 * some form of "description" of the drop event (under the player).
 *
 * We check several locations to see if we can find a location at which
 * the object can combine, stack, or be placed.  Artifacts will try very
 * hard to be placed, including "teleporting" to a useful grid if needed.
 */
s16b drop_near(object_type *j_ptr, int chance, int y, int x)
{
	int i, k, d, s;

	int bs, bn;
	int by, bx;
	int dy, dx;
	int ty, tx;

	s16b o_idx = 0;

	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;

	char o_name[MAX_NLEN];

	bool flag = FALSE;
	bool done = FALSE;

	bool plural = FALSE;


	/* Extract plural */
	if (j_ptr->number != 1) plural = TRUE;

	/* Describe object */
	object_desc(o_name, j_ptr, FALSE, 0);


	/* Handle normal "breakage" */
	if (!(j_ptr->art_name || artifact_p(j_ptr)) && (randint0(100) < chance))
	{
		/* Message */
#ifdef JP
                msg_format("%s�Ͼä�����", o_name);
#else
		msg_format("The %s disappear%s.",
			   o_name, (plural ? "" : "s"));
#endif


		/* Debug */
#ifdef JP
		if (p_ptr->wizard) msg_print("(��»)");
#else
		if (p_ptr->wizard) msg_print("(breakage)");
#endif


		/* Failure */
		return (0);
	}


	/* Score */
	bs = -1;

	/* Picker */
	bn = 0;

	/* Default */
	by = y;
	bx = x;

	/* Scan local grids */
	for (dy = -3; dy <= 3; dy++)
	{
		/* Scan local grids */
		for (dx = -3; dx <= 3; dx++)
		{
			bool comb = FALSE;

			/* Calculate actual distance */
			d = (dy * dy) + (dx * dx);

			/* Ignore distant grids */
			if (d > 10) continue;

			/* Location */
			ty = y + dy;
			tx = x + dx;

			/* Skip illegal grids */
			if (!in_bounds(ty, tx)) continue;

			/* Require line of sight */
			if (!los(y, x, ty, tx)) continue;

			/* Obtain grid */
			c_ptr = &cave[ty][tx];

			/* Require floor space */
			if ((c_ptr->feat != FEAT_FLOOR) &&
			    (c_ptr->feat != FEAT_SHAL_WATER) &&
			    (c_ptr->feat != FEAT_GRASS) &&
			    (c_ptr->feat != FEAT_DIRT) &&
			    (c_ptr->feat != FEAT_FLOWER) &&
			    (c_ptr->feat != FEAT_DEEP_GRASS) &&
			    (c_ptr->feat != FEAT_SHAL_LAVA) &&
				(c_ptr->feat != FEAT_TREES)) continue;
			if (c_ptr->info & (CAVE_TRAP | CAVE_IN_MIRROR)) continue;

			/* No objects */
			k = 0;

			/* Scan objects in that grid */
			for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Acquire object */
				o_ptr = &o_list[this_o_idx];

				/* Acquire next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Check for possible combination */
				if (object_similar(o_ptr, j_ptr)) comb = TRUE;

				/* Count objects */
				k++;
			}

			/* Add new object */
			if (!comb) k++;

			/* Paranoia */
			if (k > 99) continue;

			/* Calculate score */
			s = 1000 - (d + k * 5);

			/* Skip bad values */
			if (s < bs) continue;

			/* New best value */
			if (s > bs) bn = 0;

			/* Apply the randomizer to equivalent values */
			if ((++bn >= 2) && !one_in_(bn)) continue;

			/* Keep score */
			bs = s;

			/* Track it */
			by = ty;
			bx = tx;

			/* Okay */
			flag = TRUE;
		}
	}


	/* Handle lack of space */
	if (!flag && !(artifact_p(j_ptr) || j_ptr->art_name))
	{
		/* Message */
#ifdef JP
                msg_format("%s�Ͼä�����", o_name);
#else
		msg_format("The %s disappear%s.",
			   o_name, (plural ? "" : "s"));
#endif


		/* Debug */
#ifdef JP
		if (p_ptr->wizard) msg_print("(�����ڡ������ʤ�)");
#else
		if (p_ptr->wizard) msg_print("(no floor space)");
#endif


		/* Failure */
		return (0);
	}


	/* Find a grid */
	for (i = 0; !flag; i++)
	{
		/* Bounce around */
		if (i < 1000)
		{
			ty = rand_spread(by, 1);
			tx = rand_spread(bx, 1);
		}

		/* Random locations */
		else
		{
			ty = randint0(cur_hgt);
			tx = randint0(cur_wid);
		}

		/* Grid */
		c_ptr = &cave[ty][tx];

		/* Require floor space (or shallow terrain) -KMW- */
		if ((c_ptr->feat != FEAT_FLOOR) &&
		    (c_ptr->feat != FEAT_SHAL_WATER) &&
		    (c_ptr->feat != FEAT_GRASS) &&
		    (c_ptr->feat != FEAT_DIRT) &&
		    (c_ptr->feat != FEAT_SHAL_LAVA)) continue;

		/* Bounce to that location */
		by = ty;
		bx = tx;

		/* Require floor space */
		if (!cave_clean_bold(by, bx)) continue;

		/* Okay */
		flag = TRUE;
	}


	/* Grid */
	c_ptr = &cave[by][bx];

	/* Scan objects in that grid for combination */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Check for combination */
		if (object_similar(o_ptr, j_ptr))
		{
			/* Combine the items */
			object_absorb(o_ptr, j_ptr);

			/* Success */
			done = TRUE;

			/* Done */
			break;
		}
	}

	/* Get new object */
	if (!done) o_idx = o_pop();

	/* Failure */
	if (!done && !o_idx)
	{
		/* Message */
#ifdef JP
                msg_format("%s�Ͼä�����", o_name);
#else
		msg_format("The %s disappear%s.",
			   o_name, (plural ? "" : "s"));
#endif


		/* Debug */
#ifdef JP
		if (p_ptr->wizard) msg_print("(�����ƥब¿�᤮��)");
#else
		if (p_ptr->wizard) msg_print("(too many objects)");
#endif


		/* Hack -- Preserve artifacts */
		if (j_ptr->name1)
		{
			a_info[j_ptr->name1].cur_num = 0;
		}

		/* Failure */
		return (0);
	}

	/* Stack */
	if (!done)
	{
		/* Structure copy */
		object_copy(&o_list[o_idx], j_ptr);

		/* Access new object */
		j_ptr = &o_list[o_idx];

		/* Locate */
		j_ptr->iy = by;
		j_ptr->ix = bx;

		/* No monster */
		j_ptr->held_m_idx = 0;

		/* Build a stack */
		j_ptr->next_o_idx = c_ptr->o_idx;

		/* Place the object */
		c_ptr->o_idx = o_idx;

		/* Success */
		done = TRUE;
	}

	/* Note the spot */
	note_spot(by, bx);

	/* Draw the spot */
	lite_spot(by, bx);

	/* Sound */
	sound(SOUND_DROP);

	/* Mega-Hack -- no message if "dropped" by player */
	/* Message when an object falls under the player */
	if (chance && (by == py) && (bx == px))
	{
#ifdef JP
		msg_print("������­����ž���äƤ�����");
#else
		msg_print("You feel something roll beneath your feet.");
#endif

	}

	/* XXX XXX XXX */

	/* Result */
	return (o_idx);
}


/*
 * Scatter some "great" objects near the player
 */
void acquirement(int y1, int x1, int num, bool great, bool known)
{
	object_type *i_ptr;
	object_type object_type_body;

	/* Acquirement */
	while (num--)
	{
		/* Get local object */
		i_ptr = &object_type_body;

		/* Wipe the object */
		object_wipe(i_ptr);

		/* Make a good (or great) object (if possible) */
		if (!make_object(i_ptr, TRUE, great)) continue;

		if (known)
		{
			object_aware(i_ptr);
			object_known(i_ptr);
		}

		/* Drop the object */
		(void)drop_near(i_ptr, -1, y1, x1);
	}
}


#define MAX_TRAPS		18

static int trap_num[MAX_TRAPS] =
{
	FEAT_TRAP_TRAPDOOR,
	FEAT_TRAP_PIT,
	FEAT_TRAP_SPIKED_PIT,
	FEAT_TRAP_POISON_PIT,
	FEAT_TRAP_TY_CURSE,
	FEAT_TRAP_TELEPORT,
	FEAT_TRAP_FIRE,
	FEAT_TRAP_ACID,
	FEAT_TRAP_SLOW,
	FEAT_TRAP_LOSE_STR,
	FEAT_TRAP_LOSE_DEX,
	FEAT_TRAP_LOSE_CON,
	FEAT_TRAP_BLIND,
	FEAT_TRAP_CONFUSE,
	FEAT_TRAP_POISON,
	FEAT_TRAP_SLEEP,
	FEAT_TRAP_TRAPS,
	FEAT_TRAP_ALARM,
};


/*
 * Hack -- instantiate a trap
 *
 * XXX XXX XXX This routine should be redone to reflect trap "level".
 * That is, it does not make sense to have spiked pits at 50 feet.
 * Actually, it is not this routine, but the "trap instantiation"
 * code, which should also check for "trap doors" on quest levels.
 */
void pick_trap(int y, int x)
{
	int feat;

	cave_type *c_ptr = &cave[y][x];

	/* Paranoia */
	if (!(c_ptr->info & CAVE_TRAP)) return;
	c_ptr->info &= ~(CAVE_TRAP);

	/* Pick a trap */
	while (1)
	{
		/* Hack -- pick a trap */
		feat = trap_num[randint0(MAX_TRAPS)];

		/* Accept non-trapdoors */
		if (feat != FEAT_TRAP_TRAPDOOR) break;

		/* Hack -- no trap doors on special levels */
		if (p_ptr->inside_arena || quest_number(dun_level)) continue;

		/* Hack -- no trap doors on the deepest level */
		if (dun_level >= d_info[dungeon_type].maxdepth) continue;

		break;
	}

	/* Activate the trap */
	cave_set_feat(y, x, feat);
}


/*
 * Places a random trap at the given location.
 *
 * The location must be a legal, naked, floor grid.
 *
 * Note that all traps start out as "invisible" and "untyped", and then
 * when they are "discovered" (by detecting them or setting them off),
 * the trap is "instantiated" as a visible, "typed", trap.
 */
void place_trap(int y, int x)
{
	/* Paranoia -- verify location */
	if (!in_bounds(y, x)) return;

	/* Require empty, clean, floor grid */
	if (!cave_naked_bold(y, x)) return;

	/* Place an invisible trap */
	cave[y][x].info |= CAVE_TRAP;
}


/*
 * Describe the charges on an item in the inventory.
 */
void inven_item_charges(int item)
{
	object_type *o_ptr = &inventory[item];

	/* Require staff/wand */
	if ((o_ptr->tval != TV_STAFF) && (o_ptr->tval != TV_WAND)) return;

	/* Require known item */
	if (!object_known_p(o_ptr)) return;

#ifdef JP
        if (o_ptr->pval <= 0)
        {
                msg_print("�⤦���Ϥ��ĤäƤ��ʤ���");
        }
        else
        {
                msg_format("���� %d ��ʬ�����Ϥ��ĤäƤ��롣", o_ptr->pval);
        }
#else
	/* Multiple charges */
	if (o_ptr->pval != 1)
	{
		/* Print a message */
		msg_format("You have %d charges remaining.", o_ptr->pval);
	}

	/* Single charge */
	else
	{
		/* Print a message */
		msg_format("You have %d charge remaining.", o_ptr->pval);
	}
#endif

}


/*
 * Describe an item in the inventory.
 */
void inven_item_describe(int item)
{
	object_type *o_ptr = &inventory[item];
	char        o_name[MAX_NLEN];

	/* Get a description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Print a message */
#ifdef JP
        /* "no more" �ξ��Ϥ������ɽ������ */
        if (o_ptr->number <= 0)
        {
                /*FIRST*//*�����Ϥ⤦�̤�ʤ����� */
                msg_format("�⤦%s����äƤ��ʤ���", o_name);
        }
        else
        {
                /* �����ƥ�̾������ڤ��ؤ���ǽ�б� */
                msg_format("�ޤ� %s����äƤ��롣", o_name);
        }
#else
	msg_format("You have %s.", o_name);
#endif

}


/*
 * Increase the "number" of an item in the inventory
 */
void inven_item_increase(int item, int num)
{
	object_type *o_ptr = &inventory[item];

	/* Apply */
	num += o_ptr->number;

	/* Bounds check */
	if (num > 255) num = 255;
	else if (num < 0) num = 0;

	/* Un-apply */
	num -= o_ptr->number;

	/* Change the number and weight */
	if (num)
	{
		/* Add the number */
		o_ptr->number += num;

		/* Add the weight */
		p_ptr->total_weight += (num * o_ptr->weight);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate mana XXX */
		p_ptr->update |= (PU_MANA);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}
}


/*
 * Erase an inventory slot if it has no more items
 */
void inven_item_optimize(int item)
{
	object_type *o_ptr = &inventory[item];

	/* Only optimize real items */
	if (!o_ptr->k_idx) return;

	/* Only optimize empty items */
	if (o_ptr->number) return;

	/* The item is in the pack */
	if (item < INVEN_RARM)
	{
		int i;

		/* One less item */
		inven_cnt--;

		/* Slide everything down */
		for (i = item; i < INVEN_PACK; i++)
		{
			/* Structure copy */
			inventory[i] = inventory[i+1];
		}

		/* Erase the "final" slot */
		object_wipe(&inventory[i]);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* The item is being wielded */
	else
	{
		/* One less item */
		equip_cnt--;

		/* Erase the empty slot */
		object_wipe(&inventory[item]);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate torch */
		p_ptr->update |= (PU_TORCH);

		/* Recalculate mana XXX */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);
	}

	/* Window stuff */
	p_ptr->window |= (PW_SPELL);
}


/*
 * Describe the charges on an item on the floor.
 */
void floor_item_charges(int item)
{
	object_type *o_ptr = &o_list[item];

	/* Require staff/wand */
	if ((o_ptr->tval != TV_STAFF) && (o_ptr->tval != TV_WAND)) return;

	/* Require known item */
	if (!object_known_p(o_ptr)) return;

#ifdef JP
        if (o_ptr->pval <= 0)
        {
                msg_print("���ξ���Υ����ƥ�ϡ��⤦���Ϥ��ĤäƤ��ʤ���");
        }
        else
        {
                msg_format("���ξ���Υ����ƥ�ϡ����� %d ��ʬ�����Ϥ��ĤäƤ��롣", o_ptr->pval);
        }
#else
	/* Multiple charges */
	if (o_ptr->pval != 1)
	{
		/* Print a message */
		msg_format("There are %d charges remaining.", o_ptr->pval);
	}

	/* Single charge */
	else
	{
		/* Print a message */
		msg_format("There is %d charge remaining.", o_ptr->pval);
	}
#endif

}


/*
 * Describe an item in the inventory.
 */
void floor_item_describe(int item)
{
	object_type *o_ptr = &o_list[item];
	char        o_name[MAX_NLEN];

	/* Get a description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Print a message */
#ifdef JP
        /* "no more" �ξ��Ϥ������ɽ����ʬ���� */
        if (o_ptr->number <= 0)
        {
                msg_format("����ˤϡ��⤦%s�Ϥʤ���", o_name);
        }
        else
        {
                msg_format("����ˤϡ��ޤ� %s�����롣", o_name);
        }
#else
	msg_format("You see %s.", o_name);
#endif

}


/*
 * Increase the "number" of an item on the floor
 */
void floor_item_increase(int item, int num)
{
	object_type *o_ptr = &o_list[item];

	/* Apply */
	num += o_ptr->number;

	/* Bounds check */
	if (num > 255) num = 255;
	else if (num < 0) num = 0;

	/* Un-apply */
	num -= o_ptr->number;

	/* Change the number */
	o_ptr->number += num;
}


/*
 * Optimize an item on the floor (destroy "empty" items)
 */
void floor_item_optimize(int item)
{
	object_type *o_ptr = &o_list[item];

	/* Paranoia -- be sure it exists */
	if (!o_ptr->k_idx) return;

	/* Only optimize empty items */
	if (o_ptr->number) return;

	/* Delete the object */
	delete_object_idx(item);
}


/*
 * Check if we have space for an item in the pack without overflow
 */
bool inven_carry_okay(object_type *o_ptr)
{
	int j;

	/* Empty slot? */
	if (inven_cnt < INVEN_PACK) return (TRUE);

	/* Similar slot? */
	for (j = 0; j < INVEN_PACK; j++)
	{
		object_type *j_ptr = &inventory[j];

		/* Skip non-objects */
		if (!j_ptr->k_idx) continue;

		/* Check if the two items can be combined */
		if (object_similar(j_ptr, o_ptr)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Add an item to the players inventory, and return the slot used.
 *
 * If the new item can combine with an existing item in the inventory,
 * it will do so, using "object_similar()" and "object_absorb()", else,
 * the item will be placed into the "proper" location in the inventory.
 *
 * This function can be used to "over-fill" the player's pack, but only
 * once, and such an action must trigger the "overflow" code immediately.
 * Note that when the pack is being "over-filled", the new item must be
 * placed into the "overflow" slot, and the "overflow" must take place
 * before the pack is reordered, but (optionally) after the pack is
 * combined.  This may be tricky.  See "dungeon.c" for info.
 *
 * Note that this code must remove any location/stack information
 * from the object once it is placed into the inventory.
 */
s16b inven_carry(object_type *o_ptr)
{
	int i, j, k;
	int n = -1;

	object_type *j_ptr;


	/* Check for combining */
	for (j = 0; j < INVEN_PACK; j++)
	{
		j_ptr = &inventory[j];

		/* Skip non-objects */
		if (!j_ptr->k_idx) continue;

		/* Hack -- track last item */
		n = j;

		/* Check if the two items can be combined */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Combine the items */
			object_absorb(j_ptr, o_ptr);

			/* Increase the weight */
			p_ptr->total_weight += (o_ptr->number * o_ptr->weight);

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN);

			/* Success */
			return (j);
		}
	}


	/* Paranoia */
	if (inven_cnt > INVEN_PACK) return (-1);

	/* Find an empty slot */
	for (j = 0; j <= INVEN_PACK; j++)
	{
		j_ptr = &inventory[j];

		/* Use it if found */
		if (!j_ptr->k_idx) break;
	}

	/* Use that slot */
	i = j;


	/* Reorder the pack */
	if (i < INVEN_PACK)
	{
		s32b o_value, j_value;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_PACK; j++)
		{
			j_ptr = &inventory[j];

			/* Use empty slots */
			if (!j_ptr->k_idx) break;

			/* Hack -- readable books always come first */
			if ((o_ptr->tval == REALM1_BOOK) &&
			    (j_ptr->tval != REALM1_BOOK)) break;
			if ((j_ptr->tval == REALM1_BOOK) &&
			    (o_ptr->tval != REALM1_BOOK)) continue;

			if ((o_ptr->tval == REALM2_BOOK) &&
			    (j_ptr->tval != REALM2_BOOK)) break;
			if ((j_ptr->tval == REALM2_BOOK) &&
			    (o_ptr->tval != REALM2_BOOK)) continue;

			/* Objects sort by decreasing type */
			if (o_ptr->tval > j_ptr->tval) break;
			if (o_ptr->tval < j_ptr->tval) continue;

			/* Non-aware (flavored) items always come last */
			if (!object_aware_p(o_ptr)) continue;
			if (!object_aware_p(j_ptr)) break;

			/* Objects sort by increasing sval */
			if (o_ptr->sval < j_ptr->sval) break;
			if (o_ptr->sval > j_ptr->sval) continue;

			/* Unidentified objects always come last */
			if (!object_known_p(o_ptr)) continue;
			if (!object_known_p(j_ptr)) break;

			/* Hack:  otherwise identical rods sort by
			increasing recharge time --dsb */
			if (o_ptr->tval == TV_ROD)
			{
				if (o_ptr->pval < j_ptr->pval) break;
				if (o_ptr->pval > j_ptr->pval) continue;
			}

			/* Determine the "value" of the pack item */
			j_value = object_value(j_ptr);

			/* Objects sort by decreasing value */
			if (o_value > j_value) break;
			if (o_value < j_value) continue;
		}

		/* Use that slot */
		i = j;

		/* Slide objects */
		for (k = n; k >= i; k--)
		{
			/* Hack -- Slide the item */
			object_copy(&inventory[k+1], &inventory[k]);
		}

		/* Wipe the empty slot */
		object_wipe(&inventory[i]);
	}


	/* Copy the item */
	object_copy(&inventory[i], o_ptr);

	/* Access new object */
	j_ptr = &inventory[i];

	/* Forget stack */
	j_ptr->next_o_idx = 0;

	/* Forget monster */
	j_ptr->held_m_idx = 0;

	/* Forget location */
	j_ptr->iy = j_ptr->ix = 0;

	/* No longer marked */
	j_ptr->marked = FALSE;

	/* Increase the weight */
	p_ptr->total_weight += (j_ptr->number * j_ptr->weight);

	/* Count the items */
	inven_cnt++;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine and Reorder pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Return the slot */
	return (i);
}


/*
 * Take off (some of) a non-cursed equipment item
 *
 * Note that only one item at a time can be wielded per slot.
 *
 * Note that taking off an item when "full" may cause that item
 * to fall to the ground.
 *
 * Return the inventory slot into which the item is placed.
 */
s16b inven_takeoff(int item, int amt)
{
	int slot;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr act;

	char o_name[MAX_NLEN];


	/* Get the item to take off */
	o_ptr = &inventory[item];

	/* Paranoia */
	if (amt <= 0) return (-1);

	/* Verify */
	if (amt > o_ptr->number) amt = o_ptr->number;

	/* Get local object */
	q_ptr = &forge;

	/* Obtain a local object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = amt;

	/* Describe the object */
	object_desc(o_name, q_ptr, TRUE, 3);

	/* Took off weapon */
	if (item == INVEN_RARM)
	{
#ifdef JP
		act = "����������Ϥ�����";
#else
		act = "You were wielding";
#endif

	}

	/* Took off bow */
	else if (item == INVEN_BOW)
	{
#ifdef JP
		act = "����������Ϥ�����";
#else
		act = "You were holding";
#endif

	}

	/* Took off light */
	else if (item == INVEN_LITE)
	{
#ifdef JP
		act = "���������Ϥ�����";
#else
		act = "You were holding";
#endif

	}

	/* Took off something */
	else
	{
#ifdef JP
		act = "����������Ϥ�����";
#else
		act = "You were wearing";
#endif

	}

	/* Modify, Optimize */
	inven_item_increase(item, -amt);
	inven_item_optimize(item);

	/* Carry the object */
	slot = inven_carry(q_ptr);

	/* Message */
#ifdef JP
        msg_format("%s(%c)%s��", o_name, index_to_label(slot), act);
#else
	msg_format("%s %s (%c).", act, o_name, index_to_label(slot));
#endif


	/* Return slot */
	return (slot);
}


/*
 * Drop (some of) a non-cursed inventory/equipment item
 *
 * The object will be dropped "near" the current location
 */
void inven_drop(int item, int amt)
{
	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	char o_name[MAX_NLEN];


	/* Access original object */
	o_ptr = &inventory[item];

	/* Error check */
	if (amt <= 0) return;

	/* Not too many */
	if (amt > o_ptr->number) amt = o_ptr->number;


	/* Take off equipment */
	if (item >= INVEN_RARM)
	{
		/* Take off first */
		item = inven_takeoff(item, amt);

		/* Access original object */
		o_ptr = &inventory[item];
	}


	/* Get local object */
	q_ptr = &forge;

	/* Obtain local object */
	object_copy(q_ptr, o_ptr);

	/* Distribute charges of wands or rods */
	distribute_charges(o_ptr, q_ptr, amt);

	/* Modify quantity */
	q_ptr->number = amt;

	/* Describe local object */
	object_desc(o_name, q_ptr, TRUE, 3);

	/* Message */
#ifdef JP
	msg_format("%s(%c)����Ȥ�����", o_name, index_to_label(item));
#else
	msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif


	/* Drop it near the player */
	(void)drop_near(q_ptr, 0, py, px);

	/* Modify, Describe, Optimize */
	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
}


/*
 * Combine items in the pack
 *
 * Note special handling of the "overflow" slot
 */
void combine_pack(void)
{
	int             i, j, k;
	object_type     *o_ptr;
	object_type     *j_ptr;
	bool            flag = FALSE;


	/* Combine the pack (backwards) */
	for (i = INVEN_PACK; i > 0; i--)
	{
		/* Get the item */
		o_ptr = &inventory[i];

		/* Skip empty items */
		if (!o_ptr->k_idx) continue;

		/* Scan the items above that item */
		for (j = 0; j < i; j++)
		{
			/* Get the item */
			j_ptr = &inventory[j];

			/* Skip empty items */
			if (!j_ptr->k_idx) continue;

			/* Can we (partialy) drop "o_ptr" onto "j_ptr"? */
			if (object_similar_part(j_ptr, o_ptr)
			    && j_ptr->number < MAX_STACK_SIZE-1)
			{
				if (o_ptr->number + j_ptr->number < MAX_STACK_SIZE)
				{
					/* Take note */
					flag = TRUE;

					/* Add together the item counts */
					object_absorb(j_ptr, o_ptr);

					/* One object is gone */
					inven_cnt--;

					/* Slide everything down */
					for (k = i; k < INVEN_PACK; k++)
					{
						/* Structure copy */
						inventory[k] = inventory[k+1];
					}
					
					/* Erase the "final" slot */
					object_wipe(&inventory[k]);
				}
				else
				{
					int remain = j_ptr->number + o_ptr->number - 99;
					
					o_ptr->number -= remain;
					
					/* Add together the item counts */
					object_absorb(j_ptr, o_ptr);

					o_ptr->number = remain;
					
				}
				/* Window stuff */
				p_ptr->window |= (PW_INVEN);
				
				/* Done */
				break;
			}
		}
	}

	/* Message */
#ifdef JP
	if (flag) msg_print("���å�����Υ����ƥ��ޤȤ�ľ������");
#else
	if (flag) msg_print("You combine some items in your pack.");
#endif

}


/*
 * Reorder items in the pack
 *
 * Note special handling of the "overflow" slot
 */
void reorder_pack(void)
{
	int             i, j, k;
	s32b            o_value;
	s32b            j_value;
	object_type     forge;
	object_type     *q_ptr;
	object_type     *j_ptr;
	object_type     *o_ptr;
	bool            flag = FALSE;


	/* Re-order the pack (forwards) */
	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Mega-Hack -- allow "proper" over-flow */
		if ((i == INVEN_PACK) && (inven_cnt == INVEN_PACK)) break;

		/* Get the item */
		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_PACK; j++)
		{
			/* Get the item already there */
			j_ptr = &inventory[j];

			/* Use empty slots */
			if (!j_ptr->k_idx) break;

			/* Hack -- readable books always come first */
			if ((o_ptr->tval == REALM1_BOOK) &&
			    (j_ptr->tval != REALM1_BOOK)) break;
			if ((j_ptr->tval == REALM1_BOOK) &&
			    (o_ptr->tval != REALM1_BOOK)) continue;

			if ((o_ptr->tval == REALM2_BOOK) &&
			    (j_ptr->tval != REALM2_BOOK)) break;
			if ((j_ptr->tval == REALM2_BOOK) &&
			    (o_ptr->tval != REALM2_BOOK)) continue;

			/* Objects sort by decreasing type */
			if (o_ptr->tval > j_ptr->tval) break;
			if (o_ptr->tval < j_ptr->tval) continue;

			/* Non-aware (flavored) items always come last */
			if (!object_aware_p(o_ptr)) continue;
			if (!object_aware_p(j_ptr)) break;

			/* Objects sort by increasing sval */
			if (o_ptr->sval < j_ptr->sval) break;
			if (o_ptr->sval > j_ptr->sval) continue;

			/* Unidentified objects always come last */
			if (!object_known_p(o_ptr)) continue;
			if (!object_known_p(j_ptr)) break;

			/* Hack:  otherwise identical rods sort by
			increasing recharge time --dsb */
			if (o_ptr->tval == TV_ROD)
			{
				if (o_ptr->pval < j_ptr->pval) break;
				if (o_ptr->pval > j_ptr->pval) continue;
			}

			/* Determine the "value" of the pack item */
			j_value = object_value(j_ptr);



			/* Objects sort by decreasing value */
			if (o_value > j_value) break;
			if (o_value < j_value) continue;
		}

		/* Never move down */
		if (j >= i) continue;

		/* Take note */
		flag = TRUE;

		/* Get local object */
		q_ptr = &forge;

		/* Save a copy of the moving item */
		object_copy(q_ptr, &inventory[i]);

		/* Slide the objects */
		for (k = i; k > j; k--)
		{
			/* Slide the item */
			object_copy(&inventory[k], &inventory[k-1]);
		}

		/* Insert the moving item */
		object_copy(&inventory[j], q_ptr);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* Message */
#ifdef JP
	if (flag) msg_print("���å�����Υ����ƥ���¤�ľ������");
#else
	if (flag) msg_print("You reorder some items in your pack.");
#endif

}


/*
 * Hack -- display an object kind in the current window
 *
 * Include list of usable spells for readible books
 */
void display_koff(int k_idx)
{
	int y;

	object_type forge;
	object_type *q_ptr;

	char o_name[MAX_NLEN];


	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* No info */
	if (!k_idx) return;

	/* Get local object */
	q_ptr = &forge;

	/* Prepare the object */
	object_prep(q_ptr, k_idx);

	/* Describe */
	object_desc_store(o_name, q_ptr, FALSE, 0);

	/* Mention the object name */
	Term_putstr(0, 0, -1, TERM_WHITE, o_name);

	/* Warriors are illiterate */
	if (!(p_ptr->realm1 || p_ptr->realm2)) return;

	/* Display spells in readible books */
	if ((q_ptr->tval == REALM1_BOOK) ||
	    (q_ptr->tval == REALM2_BOOK))
	{
		int     sval;
		int     spell = -1;
		int     num = 0;
		byte    spells[64];


		/* Access the item's sval */
		sval = q_ptr->sval;

		/* Extract spells */
		for (spell = 0; spell < 32; spell++)
		{
			/* Check for this spell */
			if (fake_spell_flags[sval] & (1L << spell))
			{
				/* Collect this spell */
				spells[num++] = spell;
			}
		}

		/* Print spells */
		print_spells(0, spells, num, 2, 0,
		    (q_ptr->tval == REALM1_BOOK ? p_ptr->realm1 : p_ptr->realm2));
	}
}

/* Choose one of items that have warning flag */
object_type *choose_warning_item(void)
{
	int i;
	int choices[INVEN_TOTAL-INVEN_RARM];
	int number = 0;

	/* Paranoia -- Player has no warning-item */
	if (!p_ptr->warning) return (NULL);

	/* Search Inventry */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		u32b f1, f2, f3;
		object_type *o_ptr = &inventory[i];

		object_flags(o_ptr, &f1, &f2, &f3);
		if (f3 & (TR3_WARNING))
		{
			choices[number] = i;
			number++;
		}
	}

	/* Choice one of them */
	return (&inventory[choices[randint0(number)]]);
}

/* Examine the grid (xx,yy) and warn the player if there are any danger */
bool process_frakir(int xx, int yy)
{
	int mx,my;
	cave_type *c_ptr;
	char o_name[MAX_NLEN];

#define FRAKIR_AWARE_RANGE 12
	int dam_max = 0;
	static int old_damage = 0;

	for (mx = xx-FRAKIR_AWARE_RANGE; mx < xx+FRAKIR_AWARE_RANGE+1; mx++)
	{
		for (my = yy-FRAKIR_AWARE_RANGE; my < yy+FRAKIR_AWARE_RANGE+1; my++)
		{
			int dam_max0=0;
			monster_type *m_ptr;
			monster_race *r_ptr;
			u32b f4, f5, f6;
			int rlev;

			if (!in_bounds(my,mx) || (distance(my,mx,yy,xx)>FRAKIR_AWARE_RANGE)) continue;

			c_ptr = &cave[my][mx];

			if (!c_ptr->m_idx) continue;

			m_ptr = &m_list[c_ptr->m_idx];
			r_ptr = &r_info[m_ptr->r_idx];

			f4 = r_ptr->flags4;
			f5 = r_ptr->flags5;
			f6 = r_ptr->flags6;

			rlev = ((r_ptr->level >= 1) ? r_ptr->level : 1);

			if (m_ptr->csleep) continue;
			if (is_pet(m_ptr)) continue;

			/* Monster spells (only powerful ones)*/
			if(projectable(my,mx,yy,xx))
			{

#define DAMCALC(f,val,max,im,vln,res,resx,resy,op,opx,opy,dmax) \
	   if (f){ int dam = (val)>(max)? (max):(val); \
           if (im) dam=0; \
           if (vln) dam *= 2; \
           if (res) {dam = (dam * resx) / resy;} \
           if (op) {dam = (dam * opx) / opy;} \
           if (dam>dmax) dmax = dam; \
           }

				DAMCALC(f4 & (RF4_BR_FIRE), m_ptr->hp / 3, 1600, 
					p_ptr->immune_fire, p_ptr->muta3 & MUT3_VULN_ELEM,
					p_ptr->resist_fire, 1, 3,
					p_ptr->oppose_fire, 1, 3, dam_max0);

				DAMCALC(f4 & (RF4_BR_COLD), m_ptr->hp / 3, 1600, 
					p_ptr->immune_cold, p_ptr->muta3 & MUT3_VULN_ELEM,
					p_ptr->resist_cold, 1, 3,
					p_ptr->oppose_cold, 1, 3, dam_max0);

				DAMCALC(f4 & (RF4_BR_ELEC), m_ptr->hp / 3, 1600, 
					p_ptr->immune_elec, p_ptr->muta3 & MUT3_VULN_ELEM,
					p_ptr->resist_elec, 1, 3,
					p_ptr->oppose_elec, 1, 3, dam_max0);

				DAMCALC(f4 & (RF4_BR_ACID), m_ptr->hp / 3, 1600, 
					p_ptr->immune_acid, p_ptr->muta3 & MUT3_VULN_ELEM,
					p_ptr->resist_acid, 1, 3,
					p_ptr->oppose_acid, 1, 3, dam_max0);

				DAMCALC(f4 & (RF4_BR_POIS), m_ptr->hp / 3, 800,
					FALSE , FALSE,
					p_ptr->resist_pois, 1, 3,
					p_ptr->oppose_pois, 1, 3, dam_max0);


				DAMCALC(f4 & (RF4_BR_NETH), m_ptr->hp / 6, 550, FALSE , FALSE,
					p_ptr->resist_neth, 6, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_LITE), m_ptr->hp / 6, 400, FALSE , FALSE,
					p_ptr->resist_lite, 4, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_DARK), m_ptr->hp / 6, 400, FALSE , FALSE,
					p_ptr->resist_dark, 4, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_CONF), m_ptr->hp / 6, 450, FALSE , FALSE,
					p_ptr->resist_conf, 5, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_SOUN), m_ptr->hp / 6, 450, FALSE , FALSE,
					p_ptr->resist_sound, 5, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_CHAO), m_ptr->hp / 6, 600, FALSE , FALSE,
					p_ptr->resist_chaos, 6, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_DISE), m_ptr->hp / 6, 500, FALSE , FALSE,
					p_ptr->resist_disen, 6, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_NEXU), m_ptr->hp / 3, 250, FALSE , FALSE,
					p_ptr->resist_nexus, 6, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_TIME), m_ptr->hp / 3, 150, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_INER), m_ptr->hp / 6, 200, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_GRAV), m_ptr->hp / 3, 200, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_SHAR), m_ptr->hp / 6, 500, FALSE , FALSE,
					p_ptr->resist_shard, 6, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_PLAS), m_ptr->hp / 6, 150, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_WALL), m_ptr->hp / 6, 200, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_MANA), m_ptr->hp / 3, 250, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f4 & (RF4_BR_NUKE), m_ptr->hp / 3, 800, FALSE , FALSE,
					p_ptr->resist_pois, 2, 5, 
					p_ptr->oppose_pois, 2, 5, dam_max0);

				DAMCALC(f4 & (RF4_BR_DISI), m_ptr->hp / 3, 300, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);


				DAMCALC(f4 & (RF4_ROCKET), m_ptr->hp / 4, 800, FALSE , FALSE,
					p_ptr->resist_shard, 1, 2, FALSE, 1, 1, dam_max0);

				DAMCALC(f5 & (RF5_BA_MANA), rlev*4 + 150, 9999, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

				DAMCALC(f5 & (RF5_BA_DARK), rlev*4 + 150, 9999, FALSE , FALSE,
					p_ptr->resist_dark, 4, 9, FALSE, 1, 1, dam_max0);

				DAMCALC(f5 & (RF5_BA_LITE), rlev*4 + 150, 9999, FALSE , FALSE,
					p_ptr->resist_lite, 4, 9, FALSE, 1, 1, dam_max0);


				DAMCALC(f6 & (RF6_HAND_DOOM), p_ptr->chp*6/10, 9999, FALSE , FALSE,
					FALSE, 1, 1, FALSE, 1, 1, dam_max0);

			}

			/* Monster melee attacks */
			if(mx <= xx+1 && mx >= xx-1 && my <=yy+1 && my >= yy-1)
			{
				int m;
				int dam_melee=0;
				for (m = 0; m < 4; m++)
				{
					int d1, d2;

					/* Skip non-attacks */
					if (!r_ptr->blow[m].method) continue;

					/* Extract the attack info */
					d1 = r_ptr->blow[m].d_dice;
					d2 = r_ptr->blow[m].d_side;

					dam_melee += d1*d2;
				}
				if(dam_melee>dam_max0)dam_max0=dam_melee;
			}

			/* Contribution from this monster */
			dam_max+=dam_max0;
		}
	}

	/* Prevent excessive warning */
	if(dam_max > old_damage)
	{
		old_damage=dam_max * 3 / 2;

		if (dam_max>(p_ptr->chp)/2)
		{
			object_type *o_ptr = choose_warning_item();

			object_desc(o_name, o_ptr, FALSE, 0);
#ifdef JP
			msg_format("%s���Ԥ��̤�����", o_name);
#else
			msg_format("Your %s pulsates sharply!", o_name);
#endif
			disturb(0,0);
#ifdef JP
			return (get_check("�����ˤ��Τޤ޿ʤफ��"));
#else
			return (get_check("Realy want to go ahead? "));
#endif
		}
	}
	else old_damage = old_damage/2;

	c_ptr = &cave[yy][xx];
	if (((is_trap(c_ptr->feat) && !easy_disarm) || (c_ptr->info & CAVE_TRAP)) && !one_in_(13))
	{
		object_type *o_ptr = choose_warning_item();

		object_desc(o_name, o_ptr, FALSE, 0);
#ifdef JP
		msg_format("%s���̤�����", o_name);
#else
		msg_format("Your %s pulsates!", o_name);
#endif
		disturb(0,0);
#ifdef JP
		return (get_check("�����ˤ��Τޤ޿ʤफ��"));
#else
		return (get_check("Realy want to go ahead? "));
#endif
	}
	return(TRUE);
}


typedef struct essence_type essence_type;
struct essence_type
{
	cptr drain_name;
	cptr add_name;
	int link;
	int type;
	int value;
};

#ifdef JP
static essence_type essence_info[MAX_ESSENCE] = {
{"����","����", 1, 4, 20},
{"��ǽ","��ǽ", 2, 4, 20},
{"����","����", 3, 4, 20},
{"���Ѥ�","���Ѥ�", 4, 4, 20},
{"�ѵ���","�ѵ���", 5, 4, 20},
{"̥��","̥��", 6, 4, 20},
{"���ϻ���","���ϻ���", 7, 4, 20},
{"","", 0, 0, 0},
{"��̩","��̩", 9, 4, 40},
{"õ��","õ��", 10, 4, 15},
{"�ֳ�������","�ֳ�������", 11, 4, 15},
{"�η�","�η�", 12, 4, 15},
{"���ԡ���","���ԡ���", 13, 4, 12},
{"�ɲù���","�ɲù���", 14, 1, 20},
{"����������","����������", 15, 1, 15},
{"�۷칶��","�۷칶��", 16, 1, 60},
{"ưʪ����","ưʪ����", 17, 1, 20},
{"�ٰ�����","�ٰ�����", 18, 1, 100},
{"�Ի�����","�Ի�����", 19, 1, 20},
{"��������","��������", 20, 1, 20},
{"����������","����������", 21, 1, 15},
{"�ȥ������","�ȥ������", 22, 1, 15},
{"�������","�������", 23, 1, 20},
{"ε����","ε����", 24, 1, 20},
{"","ε������", 24, 1, 60},
{"","", 0, 0, 0},
{"�Ͽ�","�Ͽ�ȯư", 27, 5, 15},
{"�ǻ�","�ǻ�", 28, 1, 20},
{"�ϲ�","�ϲ�", 29, 1, 20},
{"�ŷ�","�ŷ�", 30, 1, 20},
{"�ƴ�","�ƴ�", 31, 1, 20},
{"���","���", 32, 1, 20},
{"ǽ�ϰݻ�","���ϰݻ�", 33, 3, 15},
{"","��ǽ�ݻ�", 33, 3, 15},
{"","�����ݻ�", 33, 3, 15},
{"","���Ѥ��ݻ�", 33, 3, 15},
{"","�ѵ��ϰݻ�", 33, 3, 15},
{"","̥�ϰݻ�", 33, 3, 15},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"�ȱ�","���ȱ�", 41, 2, 20},
{"","�ŷ��ȱ�", 41, 2, 20},
{"","�б��ȱ�", 41, 2, 20},
{"","�䵤�ȱ�", 41, 2, 20},
{"","", 0, 0, 0},
{"ȿ��","ȿ��", 46, 2, 20},
{"�����Τ餺","�����Τ餺", 47, 3, 20},
{"��̿�ϰݻ�","��̿�ϰݻ�", 48, 3, 20},
{"�ѻ�","�ѻ�", 49, 2, 15},
{"���ŷ�","���ŷ�", 50, 2, 15},
{"�Ѳб�","�Ѳб�", 51, 2, 15},
{"���䵤","���䵤", 52, 2, 15},
{"����","����", 53, 2, 25},
{"�Ѷ���","�Ѷ���", 54, 2, 20},
{"������","������", 55, 2, 20},
{"�ѰŹ�","�ѰŹ�", 56, 2, 20},
{"������","������", 57, 2, 20},
{"�Ѻ���","�Ѻ���", 58, 2, 20},
{"�ѹ첻","�ѹ첻", 59, 2, 20},
{"������","������", 60, 2, 20},
{"���Ϲ�","���Ϲ�", 61, 2, 20},
{"�Ѱ��̺���","�Ѱ��̺���", 62, 2, 20},
{"�ѥ�����","�ѥ�����", 63, 2, 20},
{"������","������", 64, 2, 20},
{"","", -1, 0, 0},
{"","", -1, 0, 0},
{"�ʹ�����","�ʹ�����", 67, 1, 20},
{"","", -1, 0, 0},
{"","", 0, 0, 0},
{"ȿ��ˡ","ȿ��ˡ", 70, 3, 15},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"�ٹ�","�ٹ�", 73, 3, 20},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"��ͷ","��ͷ", 77, 3, 20},
{"�ʵ׸���","�ʵ׸���", 78, 3, 15},
{"�Ļ�Ʃ��","�Ļ�Ʃ��", 79, 3, 20},
{"�ƥ�ѥ���","�ƥ�ѥ���", 80, 3, 15},
{"�پò�","�پò�", 81, 3, 15},
{"��®����","��®����", 82, 3, 20},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"�ƥ�ݡ���","�ƥ�ݡ���", 91, 3, 25},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"����","����", 97, 6, 30},
{"�ɸ�","�ɸ�", 98, 6, 15},
{"","������ȯư", 49, 5, 50},
{"","�ŷ�����ȯư", 50, 5, 50},
{"","�б�����ȯư", 51, 5, 50},
{"","�䵤����ȯư", 52, 5, 50},
{"","�бꥪ����", 0, 5, 30},
{"","�ŷ⥪����", 0, 5, 30},
{"","�䵤������", 0, 5, 30},
{"","������", 0, 2, 150},
{"","�����ݻ�", 0, 6, 10},
{"","��٤�ξ���", 97, 1, 200},
};
#else
static essence_type essence_info[MAX_ESSENCE] = {
{"strength","strength", 1, 4, 20},
{"intelligen.","intelligence", 2, 4, 20},
{"wisdom","wisdom", 3, 4, 20},
{"dexterity","dexterity", 4, 4, 20},
{"constitut.","constitution", 5, 4, 20},
{"charisma","charisma", 6, 4, 20},
{"magic mast.","magic mastery", 7, 4, 20},
{"","", 0, 0, 0},
{"stealth","stealth", 9, 4, 40},
{"serching","serching", 10, 4, 15},
{"inflavision","inflavision", 11, 4, 15},
{"digging","digging", 12, 4, 15},
{"speed","speed", 13, 4, 12},
{"extra atk","extra attack", 14, 1, 20},
{"chaos brand","chaos brand", 15, 1, 15},
{"vampiric","vampiric brand", 16, 1, 60},
{"slay animal","slay animal", 17, 1, 20},
{"slay evil","slay evil", 18, 1, 100},
{"slay undead","slay undead", 19, 1, 20},
{"slay demon","slay demon", 20, 1, 20},
{"slay orc","slay orc", 21, 1, 15},
{"slay troll","slay troll", 22, 1, 15},
{"slay giant","slay giant", 23, 1, 20},
{"slay dragon","slay dragon", 24, 1, 20},
{"","kill dragon", 24, 1, 60},
{"","", 0, 0, 0},
{"quake","quake activation", 27, 5, 15},
{"pois. brand","poison brand", 28, 1, 20},
{"acid brand","acid brand", 29, 1, 20},
{"elec. brand","electric brand", 30, 1, 20},
{"fire brand","fire brand", 31, 1, 20},
{"cold brand","cold brand", 32, 1, 20},
{"sustain","sustain strength", 33, 3, 15},
{"","sustain intelligence", 33, 3, 15},
{"","sustain wisdom", 33, 3, 15},
{"","sustain dexterity", 33, 3, 15},
{"","sustain constitution", 33, 3, 15},
{"","sustain charisma", 33, 3, 15},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"immunity","acid immunity", 41, 2, 20},
{"","electric immunity", 41, 2, 20},
{"","fire immunity", 41, 2, 20},
{"","cold immunity", 41, 2, 20},
{"","", 0, 0, 0},
{"reflection","reflection", 46, 2, 20},
{"free action","free action", 47, 3, 20},
{"hold life","hold life", 48, 3, 20},
{"res. acid","resistance to acid", 49, 2, 15},
{"res. elec.","resistance to electric", 50, 2, 15},
{"res. fire","resistance to fire", 51, 2, 15},
{"res. cold","resistance to cold", 52, 2, 15},
{"res. poison","resistance to poison", 53, 2, 25},
{"res. fear","resistance to fear", 54, 2, 20},
{"res. light","resistance to light", 55, 2, 20},
{"res. dark","resistance to dark", 56, 2, 20},
{"res. blind","resistance to blind", 57, 2, 20},
{"res.confuse","resistance to confusion", 58, 2, 20},
{"res. sound","resistance to sound", 59, 2, 20},
{"res. shard","resistance to shard", 60, 2, 20},
{"res. nether","resistance to nether", 61, 2, 20},
{"res. nexus","resistance to nexus", 62, 2, 20},
{"res. chaos","resistance to chaos", 63, 2, 20},
{"res. disen.","resistance to disenchantment", 64, 2, 20},
{"","", -1, 0, 0},
{"","", -1, 0, 0},
{"slay human","slay human", 67, 1, 20},
{"","", -1, 0, 0},
{"","", 0, 0, 0},
{"anti magic","anti magic", 70, 3, 15},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"warning","warning", 73, 3, 20},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"levitation","levitation", 77, 3, 20},
{"perm. light","permanent light", 78, 3, 15},
{"see invis.","see invisible", 79, 3, 20},
{"telepathy","telepathy", 80, 3, 15},
{"slow dige.","slow digestion", 81, 3, 15},
{"regen.","regeneration", 82, 3, 20},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"teleport","teleport", 91, 3, 25},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"","", 0, 0, 0},
{"weapon enc.","weapon enchant", 97, 6, 30},
{"armor enc.","armor enchant", 98, 6, 15},
{"","resist acid activation", 49, 5, 50},
{"","resist electricity activation", 50, 5, 50},
{"","resist fire activation", 51, 5, 50},
{"","resist cold activation", 52, 5, 50},
{"","fiery sheath", 0, 5, 30},
{"","electric sheath", 0, 5, 30},
{"","sheath of coldness", 0, 5, 30},
{"","resistance", 0, 2, 150},
{"","elements proof", 0, 6, 10},
{"","gauntlets of slay", 97, 1, 200},
};
#endif

static bool item_tester_hook_melee_ammo(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
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


static void display_essence(void)
{
	int i, num = 0;

	screen_save();
	for (i = 1; i < 22; i++)
	{
		prt("",i,0);
	}
#ifdef JP
	prt("���å���   �Ŀ�     ���å���   �Ŀ�     ���å���   �Ŀ�", 1, 8);
#else
	prt("Essence      Num      Essence      Num      Essence      Num ", 1, 8);
#endif
	for (i = 0; i < MAX_ESSENCE; i++)
	{
		if (!essence_info[i].drain_name[0]) continue;
		prt(format("%-11s %5d", essence_info[i].drain_name, p_ptr->magic_num1[i]), 2+num%21, 8+num/21*22);
		num++;
	}
#ifdef JP
	prt("���߽�����Ƥ��륨�å���", 0, 0);
#else
	prt("List of all essences you have.", 0, 0);
#endif
	(void)inkey();
	screen_load();
	return;
}

static void drain_essence(void)
{
	int drain_value[MAX_ESSENCE];
	int i, item;
	int dec = 4;
	bool observe = FALSE;
	int old_ds, old_dd, old_to_h, old_to_d, old_ac, old_to_a, old_pval, old_name2;
	u32b old_f1, old_f2, old_f3, new_f1, new_f2, new_f3;
	object_type *o_ptr;
	cptr            q, s;
	byte iy, ix, marked, number;
	s16b next_o_idx, weight;

	for (i = 0; i < MAX_ESSENCE; i++)
		drain_value[i] = 0;

	item_tester_hook = item_tester_hook_weapon_armour;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥफ����Ф��ޤ�����";
	s = "��ФǤ��륢���ƥब����ޤ���";
#else
	q = "Extract from which item? ";
	s = "You have nothing you can extract from.";
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

	if (object_known_p(o_ptr) && (o_ptr->name1 || o_ptr->name2 || o_ptr->art_name || o_ptr->xtra3)) {
		char o_name[MAX_NLEN];
		object_desc(o_name, o_ptr, FALSE, 0);
#ifdef JP
		if (!get_check(format("������%s������Ф��Ƥ�����Ǥ�����", o_name))) return;
#else
		if (!get_check(format("Really extract from %s? ", o_name))) return;
#endif
	}

	energy_use = 100;

	object_flags(o_ptr, &old_f1, &old_f2, &old_f3);
	if (old_f1 & TR1_KILL_DRAGON) old_f1 |= TR1_SLAY_DRAGON;

	old_to_a = o_ptr->to_a;
	old_ac = o_ptr->ac;
	old_to_h = o_ptr->to_h;
	old_to_d = o_ptr->to_d;
	old_ds = o_ptr->ds;
	old_dd = o_ptr->dd;
	old_pval = o_ptr->pval;
	old_name2 = o_ptr->name2;
	if (o_ptr->curse_flags & (TRC_CURSED | TRC_HEAVY_CURSE | TRC_PERMA_CURSE)) dec--;
	if (old_f3 & (TR3_AGGRAVATE)) dec--;
	if (old_f3 & (TR3_NO_TELE)) dec--;
	if (old_f3 & (TR3_DRAIN_EXP)) dec--;
	if (old_f3 & (TR3_TY_CURSE)) dec--;

	iy = o_ptr->iy;
	ix = o_ptr->ix;
	next_o_idx = o_ptr->next_o_idx;
	marked = o_ptr->marked;
	weight = o_ptr->weight;
	number = o_ptr->number;

	object_prep(o_ptr, o_ptr->k_idx);

	o_ptr->iy=iy;
	o_ptr->ix=ix;
	o_ptr->next_o_idx=next_o_idx;
	o_ptr->marked=marked;
	o_ptr->number = number;
	if (item >= 0) p_ptr->total_weight += (o_ptr->weight*o_ptr->number - weight*number);
	o_ptr->ident |= (IDENT_MENTAL);
	object_aware(o_ptr);
	object_known(o_ptr);

	object_flags(o_ptr, &new_f1, &new_f2, &new_f3);

	for (i = 0; i < 96; i++)
	{
		if (i < 32)
		{
			int pval = 0;

			if (((1 << i) & TR1_PVAL_MASK) && old_pval) pval = ((new_f1 >> i) & 0x00000001) ? old_pval-o_ptr->pval : old_pval;
			if ((!((new_f1 >> i) & 0x00000001) || pval) && ((old_f1 >> i) & 0x00000001) && essence_info[i].link)
			{
				drain_value[essence_info[i].link-1] += (10 * (pval ? pval : 1));
			}
		}
		else if (i < 64)
		{
			if (!((new_f2 >> (i-32)) & 0x00000001) && ((old_f2 >> (i-32)) & 0x00000001) && essence_info[i].link)
			{
				drain_value[essence_info[i].link-1] += 10;
			}
		}
		else
		{
			if (!((new_f3 >> (i-64)) & 0x00000001) && ((old_f3 >> (i-64)) & 0x00000001) && essence_info[i].link)
			{
				if (essence_info[i].link == -1)
				{
					if (i == ESSENCE__SH__FIRE-1)
					{
						drain_value[ESSENCE_B_FIRE-1] += 10;
						drain_value[ESSENCE_RES_FIRE-1] += 10;
					}
					else if (i == ESSENCE__SH__ELEC-1)
					{
						drain_value[ESSENCE_B_ELEC-1] += 10;
						drain_value[ESSENCE_RES_ELEC-1] += 10;
					}
					else if (i == ESSENCE__SH__COLD-1)
					{
						drain_value[ESSENCE_B_COLD-1] += 10;
						drain_value[ESSENCE_RES_COLD-1] += 10;
					}
				}
				else drain_value[essence_info[i].link-1] += 10;
			}
		}
	}

	if ((old_f1 & TR1_FORCE_WEAPON) && !(new_f1 & TR1_FORCE_WEAPON))
	{
		drain_value[ESSENCE_INT-1] += 5;
		drain_value[ESSENCE_WIS-1] += 5;
	}
	if ((old_f1 & TR1_VORPAL) && !(new_f1 & TR1_VORPAL))
	{
		drain_value[ESSENCE_B_POIS-1] += 5;
		drain_value[ESSENCE_B_ACID-1] += 5;
		drain_value[ESSENCE_B_ELEC-1] += 5;
		drain_value[ESSENCE_B_FIRE-1] += 5;
		drain_value[ESSENCE_B_COLD-1] += 5;
	}
	if ((old_f3 & TR3_DEC_MANA) && !(new_f3 & TR3_DEC_MANA))
	{
		drain_value[ESSENCE_INT-1] += 10;
	}
	if ((old_f3 & TR3_XTRA_MIGHT) && !(new_f3 & TR3_XTRA_MIGHT))
	{
		drain_value[ESSENCE_STR-1] += 10;
	}
	if ((old_f3 & TR3_XTRA_SHOTS) && !(new_f3 & TR3_XTRA_SHOTS))
	{
		drain_value[ESSENCE_DEX-1] += 10;
	}
	if (old_name2 == EGO_2WEAPON)
	{
		drain_value[ESSENCE_DEX-1] += 20;
	}
	if ((o_ptr->tval >= TV_SHOT) && (o_ptr->tval <= TV_SWORD) && (o_ptr->tval != TV_BOW))
	{
		if (old_ds > o_ptr->ds) drain_value[ESSENCE_ATTACK-1] += (old_ds-o_ptr->ds)*10;

		if (old_dd > o_ptr->dd) drain_value[ESSENCE_ATTACK-1] += (old_dd-o_ptr->dd)*10;
	}
	if (old_to_h > o_ptr->to_h) drain_value[ESSENCE_ATTACK-1] += (old_to_h-o_ptr->to_h)*10;
	if (old_to_d > o_ptr->to_d) drain_value[ESSENCE_ATTACK-1] += (old_to_d-o_ptr->to_d)*10;
	if (old_ac > o_ptr->ac) drain_value[ESSENCE_AC-1] += (old_ac-o_ptr->ac)*10;
	if (old_to_a > o_ptr->to_a) drain_value[ESSENCE_AC-1] += (old_to_a-o_ptr->to_a)*10;

	for (i = 0; i < MAX_ESSENCE; i++)
	{
		drain_value[i] *= number;
		drain_value[i] = drain_value[i] * dec / 4;
		drain_value[i] = MAX(drain_value[i], 0);
		if ((o_ptr->tval >= TV_SHOT) && (o_ptr->tval <= TV_BOLT)) drain_value[i] /= 10;
		if (drain_value[i])
		{
			observe = TRUE;
		}
	}
	if (!observe)
	{
#ifdef JP
		msg_print("���å��󥹤���ФǤ��ޤ���Ǥ�����");
#else
		msg_print("You were not able to extract any essence.");
#endif
	}
	else
	{
#ifdef JP
		msg_print("��Ф������å���:");
#else
		msg_print("Extracted essences:");
#endif
		for (i = 0; i < MAX_ESSENCE; i++)
		{
			if (!drain_value[i]) continue;
			p_ptr->magic_num1[i] += drain_value[i];
			p_ptr->magic_num1[i] = MIN(20000, p_ptr->magic_num1[i]);
			msg_print(NULL);
			msg_format("%s...%d", essence_info[i].drain_name, drain_value[i]);
		}
	}

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}



static int choose_essence(void)
{
	int mode = 0;
	char choice;
	int menu_line = (use_menu ? 1 : 0);

#ifdef ALLOW_REPEAT
	if (repeat_pull(&mode) && 1 <= mode && mode <= 5)
		return mode;
	mode = 0;
#endif /* ALLOW_REPEAT */

	if (use_menu)
	{
		screen_save();

		while(!mode)
		{
#ifdef JP
			prt(format(" %s ���°��", (menu_line == 1) ? "��" : "  "), 2, 14);
			prt(format(" %s ����", (menu_line == 2) ? "��" : "  "), 3, 14);
			prt(format(" %s ǽ��", (menu_line == 3) ? "��" : "  "), 4, 14);
			prt(format(" %s ����", (menu_line == 4) ? "��" : "  "), 5, 14);
			prt(format(" %s ����¾", (menu_line == 5) ? "��" : "  "), 6, 14);
			prt("�ɤμ���Υ��å����ղä�Ԥ��ޤ�����", 0, 0);
#else
			prt(format(" %s Brand weapon", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s Resistance", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s Ability", (menu_line == 3) ? "> " : "  "), 4, 14);
			prt(format(" %s Magic number", (menu_line == 4) ? "> " : "  "), 5, 14);
			prt(format(" %s Others", (menu_line == 5) ? "> " : "  "), 6, 14);
			prt("Choose from menu.", 0, 0);
#endif
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return 0;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 4;
				break;
			case '\r':
			case '\n':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if (menu_line > 5) menu_line -= 5;
		}
		screen_load();
	}
	else
	{
		screen_save();
		while (!mode)
		{
#ifdef JP
			prt("  a) ����°��", 2, 14);
			prt("  b) ����", 3, 14);
			prt("  c) ǽ��", 4, 14);
			prt("  d) ����", 5, 14);
			prt("  e) ����¾", 6, 14);
			if (!get_com("�����ղä��ޤ���:", &choice, TRUE))
#else
			prt("  a) Brand weapon", 2, 14);
			prt("  b) Resistance", 3, 14);
			prt("  c) Ability", 4, 14);
			prt("  d) Magic number", 5, 14);
			prt("  e) Others", 6, 14);
			if (!get_com("Command :", &choice, TRUE))
#endif
			{
				screen_load();
				return 0;
			}

			switch (choice)
			{
			case 'A':
			case 'a':
				mode = 1;
				break;
			case 'B':
			case 'b':
				mode = 2;
				break;
			case 'C':
			case 'c':
				mode = 3;
				break;
			case 'D':
			case 'd':
				mode = 4;
				break;
			case 'E':
			case 'e':
				mode = 5;
				break;
			}
		}
		screen_load();
	}

#ifdef ALLOW_REPEAT
	repeat_push(mode);
#endif /* ALLOW_REPEAT */
	return mode;
}

static void add_essence(int mode)
{
	int item, max_num = 0;
	int i;
	bool flag,redraw;
	char choice;
	cptr            q, s;
	object_type *o_ptr;
	int ask = TRUE;
	char out_val[160];
	int num[22];
	char o_name[MAX_NLEN];
	int use_essence;

	int menu_line = (use_menu ? 1 : 0);

	for (i = 0; i < MAX_ESSENCE; i++)
	{
		if (essence_info[i].type != mode) continue;
		num[max_num++] = i;
	}

#ifdef ALLOW_REPEAT
	if (!repeat_pull(&i) || i<0 || i>=max_num)
	{
#endif /* ALLOW_REPEAT */


	/* Nothing chosen yet */
	flag = FALSE;

	/* No redraw yet */
	redraw = FALSE;

	/* Build a prompt */
#ifdef JP
	(void) strnfmt(out_val, 78, "('*'�ǰ���, ESC������) �ɤ�ǽ�Ϥ��ղä��ޤ�����");
#else
	(void)strnfmt(out_val, 78, "(*=List, ESC=exit) Add which ability? ");
#endif
	if (use_menu) screen_save();

	/* Get a spell from the user */

        choice = (always_show_list || use_menu) ? ESCAPE:1;
        while (!flag)
        {
		bool able[22];
		if( choice==ESCAPE ) choice = ' '; 
		else if( !get_com(out_val, &choice, FALSE) )break; 

		if (use_menu && choice != ' ')
		{
			switch(choice)
			{
				case '0':
				{
					screen_load();
					return;
					break;
				}

				case '8':
				case 'k':
				case 'K':
				{
					menu_line += (max_num-1);
					break;
				}

				case '2':
				case 'j':
				case 'J':
				{
					menu_line++;
					break;
				}

				case '4':
				case 'h':
				case 'H':
				{
					menu_line = 1;
					break;
				}
				case '6':
				case 'l':
				case 'L':
				{
					menu_line = max_num;
					break;
				}

				case 'x':
				case 'X':
				case '\r':
				case '\n':
				{
					i = menu_line - 1;
					ask = FALSE;
					break;
				}
			}
			if (menu_line > max_num) menu_line -= max_num;
		}
		/* Request redraw */
		if ((choice == ' ') || (choice == '*') || (choice == '?') || (use_menu && ask))
		{
			/* Show the list */
			if (!redraw || use_menu)
			{
				byte y, x = 10;
				int ctr;
				char dummy[80], dummy2[80];
				byte col;

				strcpy(dummy, "");

				/* Show list */
				redraw = TRUE;

				/* Save the screen */
				if (!use_menu) screen_save();

				for (y = 1; y < 24; y++)
					prt("", y, x);

				/* Print header(s) */
#ifdef JP
				prt("   ǽ��(ɬ�ץ��å���)          ɬ�׿�/�����", 1, x);

#else
				prt(" Ability(essence to need)        Needs/Possess", 1, x);
#endif
				/* Print list */
				for (ctr = 0; ctr < max_num; ctr++)
				{
					if (use_menu)
					{
						if (ctr == (menu_line-1))
#ifdef JP
							strcpy(dummy, "�� ");
#else
							strcpy(dummy, ">  ");
#endif
						else strcpy(dummy, "   ");
						
					}
					/* letter/number for power selection */
					else
					{
						sprintf(dummy, "%c) ",I2A(ctr));
					}

					strcat(dummy, essence_info[num[ctr]].add_name);

					col = TERM_WHITE;
					able[ctr] = TRUE;

					if (essence_info[num[ctr]].link)
					{
						strcat(dummy, format("(%s)", essence_info[essence_info[num[ctr]].link-1].drain_name));
						if (p_ptr->magic_num1[essence_info[num[ctr]].link-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
					}
					else
					{
						switch(num[ctr]+1)
						{
						case ESSENCE_SH_FIRE:
#ifdef JP
							strcat(dummy, "(�ƴ�+�Ѳб�)              ");
#else
							strcat(dummy, "(brand fire + res.fire)              ");
#endif
							if (p_ptr->magic_num1[ESSENCE_B_FIRE-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_FIRE-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							break;
						case ESSENCE_SH_ELEC:
#ifdef JP
							strcat(dummy, "(�ŷ�+���ŷ�)              ");
#else
							strcat(dummy, "(brand elec. + res. elec.)              ");
#endif
							if (p_ptr->magic_num1[ESSENCE_B_ELEC-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_ELEC-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							break;
						case ESSENCE_SH_COLD:
#ifdef JP
							strcat(dummy, "(���+���䵤)              ");
#else
							strcat(dummy, "(brand cold + res. cold)              ");
#endif
							if (p_ptr->magic_num1[ESSENCE_B_COLD-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_COLD-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							break;
						case ESSENCE_RESISTANCE:
#ifdef JP
							strcat(dummy, "(�Ѳб�+���䵤+���ŷ�+�ѻ�)");
#else
							strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
							if (p_ptr->magic_num1[ESSENCE_RES_FIRE-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_COLD-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_ELEC-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_ACID-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							break;
						case ESSENCE_SUSTAIN:
#ifdef JP
							strcat(dummy, "(�Ѳб�+���䵤+���ŷ�+�ѻ�)");
#else
							strcat(dummy, "(r.fire+r.cold+r.elec+r.acid)");
#endif
							if (p_ptr->magic_num1[ESSENCE_RES_FIRE-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_COLD-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_ELEC-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							if (p_ptr->magic_num1[ESSENCE_RES_ACID-1] < essence_info[num[ctr]].value) able[ctr] = FALSE;
							break;
						}
					}

					if (!able[ctr]) col = TERM_RED;

					strcpy(dummy2, format("%-50s",dummy));

					if (essence_info[num[ctr]].link)
					{
						strcat(dummy2, format(" %d/%d",essence_info[num[ctr]].value, p_ptr->magic_num1[essence_info[num[ctr]].link-1]));
					}
					else
					{
						strcat(dummy2, format(" %d/(\?\?)",essence_info[num[ctr]].value));
					}

					c_prt(col, dummy2, ctr+2, x);
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
			/* Note verify */
			ask = (isupper(choice));

			/* Lowercase */
			if (ask) choice = tolower(choice);

			/* Extract request */
			i = (islower(choice) ? A2I(choice) : -1);
		}

		/* Totally Illegal */
		if ((i < 0) || (i >= max_num) || !able[i])
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
			(void) strnfmt(tmp_val, 78, "%s���ղä��ޤ����� ", essence_info[num[i]].add_name);
#else
			(void) strnfmt(tmp_val, 78, "Add the abilitiy of %s? ", essence_info[num[i]].add_name);
#endif

			/* Belay that order */
			if (!get_check(tmp_val)) continue;
		}

		/* Stop the loop */
		flag = TRUE;
	}

	/* Restore the screen */
	if (redraw) screen_load();

	if (!flag) return;

#ifdef ALLOW_REPEAT
	repeat_push(i);
	}
#endif /* ALLOW_REPEAT */

	if (num[i] == ESSENCE_SLAY_GLOVE-1)
		item_tester_tval = TV_GLOVES;
	else if (mode == 1)
		item_tester_hook = item_tester_hook_melee_ammo;
	else if (num[i] == ESSENCE_ATTACK-1)
		item_tester_hook = item_tester_hook_weapon;
	else if (num[i] == ESSENCE_AC-1)
		item_tester_hook = item_tester_hook_armour;
	else
		item_tester_hook = item_tester_hook_weapon_armour;
	item_tester_no_ryoute = TRUE;

	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ����ɤ��ޤ�����";
	s = "���ɤǤ��륢���ƥब����ޤ���";
#else
	q = "Improve which item? ";
	s = "You have nothing to improve.";
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

	if ((mode != 6) && (o_ptr->name1 || o_ptr->art_name || o_ptr->xtra3))
	{
#ifdef JP
		msg_print("���Υ����ƥ�Ϥ���ʾ���ɤǤ��ʤ���");
#else
		msg_print("This item is no more able to be improved");
#endif
		return;
	}
	
	object_desc(o_name, o_ptr, FALSE, 0);

	use_essence = essence_info[num[i]].value;
	if ((o_ptr->tval >= TV_SHOT) && (o_ptr->tval <= TV_BOLT)) use_essence = (use_essence+9)/10;
	if (o_ptr->number > 1)
	{
		use_essence *= o_ptr->number;
#ifdef JP
		msg_format("%d�Ĥ���Τǥ��å��󥹤�%dɬ�פǤ���", o_ptr->number, use_essence);
#else
		msg_format("It will take %d essences.",use_essence);
#endif

	}

	if (essence_info[num[i]].link)
	{
		if (p_ptr->magic_num1[essence_info[num[i]].link-1] < use_essence)
		{
#ifdef JP
			msg_print("���å��󥹤�­��ʤ���");
#else
			msg_print("You don't have enough essences.");
#endif
			return;
		}
		if ((num[i] < 32) && (TR1_PVAL_MASK & (0x1L << num[i])))
		{
			if (num[i] == ESSENCE_BLOWS-1)
			{
				if (o_ptr->pval > 1)
				{
#ifdef JP
					if (!get_check("�����ͤ�1�ˤʤ�ޤ���������Ǥ�����")) return;
#else
					if (!get_check("The magic number of this weapon will become 1. Are you sure? ")) return;
#endif
				}
				o_ptr->pval = 1;
			}
			else if (o_ptr->pval)
			{
				use_essence *= o_ptr->pval;
#ifdef JP
				msg_format("���å��󥹤�%d�Ļ��Ѥ��ޤ���",use_essence);
#else
				msg_format("It will take %d essences.",use_essence);
#endif
			}
			else
			{
				char tmp[80];
				char tmp_val[160];
				int pval;
				int limit = MIN(5, p_ptr->magic_num1[essence_info[num[i]].link-1]/essence_info[num[i]].value);


#ifdef JP
				sprintf(tmp, "�������ղä��ޤ����� (1-%d): ", limit);
#else
				sprintf(tmp, "Enchant how many? (1-%d): ", limit);
#endif
				strcpy(tmp_val, "1");

				if (!get_string(tmp, tmp_val, 1)) return;
				pval = atoi(tmp_val);
				if (pval > limit) pval = limit;
				else if (pval < 1) pval = 1;
				o_ptr->pval = pval;
				use_essence *= pval;
#ifdef JP
				msg_format("���å��󥹤�%d�Ļ��Ѥ��ޤ���",use_essence);
#else
				msg_format("It will take %d essences.",use_essence);
#endif
			}
			if (p_ptr->magic_num1[essence_info[num[i]].link-1] < use_essence)
			{
#ifdef JP
				msg_print("���å��󥹤�­��ʤ���");
#else
				msg_print("You don't have enough essences.");
#endif
				return;
			}
		}
		else if (num[i] == ESSENCE_SLAY_GLOVE-1)
		{
			char tmp_val[160];
			int val;
			int get_to_h, get_to_d;

			strcpy(tmp_val, "1");
#ifdef JP
			if (!get_string(format("�������ղä��ޤ����� (1-%d):", p_ptr->lev/7+3), tmp_val, 2)) return;
#else
			if (!get_string(format("Enchant how many? (1-%d):", p_ptr->lev/7+3), tmp_val, 2)) return;
#endif
			val = atoi(tmp_val);
			if (val > p_ptr->lev/7+3) val = p_ptr->lev/7+3;
			else if (val < 1) val = 1;
			use_essence *= val;
#ifdef JP
			msg_format("���å��󥹤�%d�Ļ��Ѥ��ޤ���",use_essence);
#else
			msg_format("It will take %d essences.",use_essence);
#endif
			if (p_ptr->magic_num1[essence_info[num[i]].link-1] < use_essence)
			{
#ifdef JP
				msg_print("���å��󥹤�­��ʤ���");
#else
				msg_print("You don't have enough essences");
#endif
				return;
			}
			get_to_h = ((val+1)/2+randint0(val/2+1));
			get_to_d = ((val+1)/2+randint0(val/2+1));
			o_ptr->xtra4 = (get_to_h<<8)+get_to_d;
			o_ptr->to_h += get_to_h;
			o_ptr->to_d += get_to_d;
		}
		p_ptr->magic_num1[essence_info[num[i]].link-1] -= use_essence;
		if (num[i] == ESSENCE_ATTACK-1)
		{
			if ((o_ptr->to_h >= p_ptr->lev/5+5) && (o_ptr->to_d >= p_ptr->lev/5+5))
			{
#ifdef JP
				msg_print("���ɤ˼��Ԥ�����");
#else
				msg_print("You failed to enchant.");
#endif
				energy_use = 100;
				return;
			}
			else
			{
				if (o_ptr->to_h < p_ptr->lev/5+5) o_ptr->to_h++;
				if (o_ptr->to_d < p_ptr->lev/5+5) o_ptr->to_d++;
			}
		}
		else if (num[i] == ESSENCE_AC-1)
		{
			if (o_ptr->to_a >= p_ptr->lev/5+5)
			{
#ifdef JP
				msg_print("���ɤ˼��Ԥ�����");
#else
				msg_print("You failed to enchant.");
#endif
				energy_use = 100;
				return;
			}
			else
			{
				if (o_ptr->to_a < p_ptr->lev/5+5) o_ptr->to_a++;
			}
		}
		else
		{
			o_ptr->xtra3 = num[i]+1;
		}
	}
	else
	{
		bool success = TRUE;

		switch(num[i]+1)
		{
		case ESSENCE_SH_FIRE:
			if ((p_ptr->magic_num1[ESSENCE_B_FIRE-1] < use_essence) || (p_ptr->magic_num1[ESSENCE_RES_FIRE-1] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[ESSENCE_B_FIRE-1] -= use_essence;
			p_ptr->magic_num1[ESSENCE_RES_FIRE-1] -= use_essence;
			break;
		case ESSENCE_SH_ELEC:
			if ((p_ptr->magic_num1[ESSENCE_B_ELEC-1] < use_essence) || (p_ptr->magic_num1[ESSENCE_RES_ELEC-1] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[ESSENCE_B_ELEC-1] -= use_essence;
			p_ptr->magic_num1[ESSENCE_RES_ELEC-1] -= use_essence;
			break;
		case ESSENCE_SH_COLD:
			if ((p_ptr->magic_num1[ESSENCE_B_COLD-1] < use_essence) || (p_ptr->magic_num1[ESSENCE_RES_COLD-1] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[ESSENCE_B_COLD-1] -= use_essence;
			p_ptr->magic_num1[ESSENCE_RES_COLD-1] -= use_essence;
			break;
		case ESSENCE_RESISTANCE:
		case ESSENCE_SUSTAIN:
			if ((p_ptr->magic_num1[ESSENCE_RES_ACID-1] < use_essence) || (p_ptr->magic_num1[ESSENCE_RES_ELEC-1] < use_essence) || (p_ptr->magic_num1[ESSENCE_RES_FIRE-1] < use_essence) || (p_ptr->magic_num1[ESSENCE_RES_COLD-1] < use_essence))
			{
				success = FALSE;
				break;
			}
			p_ptr->magic_num1[ESSENCE_RES_ACID-1] -= use_essence;
			p_ptr->magic_num1[ESSENCE_RES_ELEC-1] -= use_essence;
			p_ptr->magic_num1[ESSENCE_RES_FIRE-1] -= use_essence;
			p_ptr->magic_num1[ESSENCE_RES_COLD-1] -= use_essence;
			break;
		}
		if (!success)
		{
#ifdef JP
			msg_print("���å��󥹤�­��ʤ���");
#else
			msg_print("You don't have enough essences");
#endif
			return;
		}
		if (num[i] == ESSENCE_SUSTAIN-1)
			o_ptr->art_flags3 |= (TR3_IGNORE_ACID | TR3_IGNORE_ELEC | TR3_IGNORE_FIRE | TR3_IGNORE_COLD);
		else o_ptr->xtra3 = num[i]+1;
	}

	energy_use = 100;

#ifdef JP
	msg_format("%s��%s��ǽ�Ϥ��ղä��ޤ�����", o_name, essence_info[num[i]].add_name);
#else
	msg_format("You have added ability of %s to %s.", essence_info[num[i]].add_name, o_name);
#endif

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}


static bool item_tester_hook_kaji(object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		case TV_BOW:
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		case TV_DRAG_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_BOOTS:
		case TV_GLOVES:
		{
			if (o_ptr->xtra3) return (TRUE);
		}
	}

	return (FALSE);
}


static void erase_essence(void)
{
	int item;
	cptr q, s;
	object_type *o_ptr;
	char o_name[MAX_NLEN];
	u32b f1, f2, f3;

	item_tester_hook = item_tester_hook_kaji;

	/* Get an item */
#ifdef JP
	q = "�ɤΥ����ƥ�Υ��å��󥹤�õ�ޤ�����";
	s = "���å��󥹤��ղä��������ƥब����ޤ���";
#else
	q = "Remove from which item? ";
	s = "You have nothing to remove essence.";
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

	object_desc(o_name, o_ptr, FALSE, 0);
#ifdef JP
	if (!get_check(format("������Ǥ�����[%s]", o_name))) return;
#else
	if (!get_check(format("Are you sure?[%s]", o_name))) return;
#endif

	energy_use = 100;

	if (o_ptr->xtra3 == ESSENCE_SLAY_GLOVE)
	{
		o_ptr->to_h -= (o_ptr->xtra4>>8);
		o_ptr->to_d -= (o_ptr->xtra4 & 0x000f);
		o_ptr->xtra4 = 0;
	}
	o_ptr->xtra3 = 0;
	object_flags(o_ptr, &f1, &f2, &f3);
	if (!(f1 & TR1_PVAL_MASK)) o_ptr->pval = 0;
#ifdef JP
	msg_print("���å��󥹤����ä���");
#else
	msg_print("You removed all essence you have added");
#endif

	/* Combine the pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);
}

void do_cmd_kaji(bool only_browse)
{
	int mode = 0;
	char choice;

	int menu_line = (use_menu ? 1 : 0);

	if (!only_browse)
	{
		if (p_ptr->confused)
		{
#ifdef JP
			msg_print("���𤷤Ƥ��ƺ�ȤǤ��ʤ���");
#else
			msg_print("You are too confused!");
#endif

			return;
		}
		if (p_ptr->blind)
		{
#ifdef JP
			msg_print("�ܤ������ʤ��ƺ�ȤǤ��ʤ���");
#else
			msg_print("You are blind!");
#endif

			return;
		}
		if (p_ptr->image)
		{
#ifdef JP
			msg_print("���ޤ������ʤ��ƺ�ȤǤ��ʤ���");
#else
			msg_print("You are hullcinating!");
#endif

			return;
		}
	}

#ifdef ALLOW_REPEAT
	if (!(repeat_pull(&mode) && 1 <= mode && mode <= 5))
	{
#endif /* ALLOW_REPEAT */

	if (only_browse) screen_save();
	do {
	if (!only_browse) screen_save();
	if (use_menu)
	{
		while(!mode)
		{
#ifdef JP
			prt(format(" %s ���å��󥹰���", (menu_line == 1) ? "��" : "  "), 2, 14);
			prt(format(" %s ���å������", (menu_line == 2) ? "��" : "  "), 3, 14);
			prt(format(" %s ���å��󥹾õ�", (menu_line == 3) ? "��" : "  "), 4, 14);
			prt(format(" %s ���å����ղ�", (menu_line == 4) ? "��" : "  "), 5, 14);
			prt(format(" %s ���/�ɶ񶯲�", (menu_line == 5) ? "��" : "  "), 6, 14);
			prt(format("�ɤμ���ε��Ѥ�%s�ޤ�����", only_browse ? "Ĵ��" : "�Ȥ�"), 0, 0);
#else
			prt(format(" %s List essences", (menu_line == 1) ? "> " : "  "), 2, 14);
			prt(format(" %s Extract essence", (menu_line == 2) ? "> " : "  "), 3, 14);
			prt(format(" %s Remove essence", (menu_line == 3) ? "> " : "  "), 4, 14);
			prt(format(" %s Add essence", (menu_line == 4) ? "> " : "  "), 5, 14);
			prt(format(" %s Enchant weapon/armor", (menu_line == 5) ? "> " : "  "), 6, 14);
			prt(format("Choose command from menu."), 0, 0);
#endif
			choice = inkey();
			switch(choice)
			{
			case ESCAPE:
			case 'z':
			case 'Z':
				screen_load();
				return;
			case '2':
			case 'j':
			case 'J':
				menu_line++;
				break;
			case '8':
			case 'k':
			case 'K':
				menu_line+= 4;
				break;
			case '\r':
			case '\n':
			case 'x':
			case 'X':
				mode = menu_line;
				break;
			}
			if (menu_line > 5) menu_line -= 5;
		}
	}

	else
	{
		while (!mode)
		{
#ifdef JP
			prt("  a) ���å��󥹰���", 2, 14);
			prt("  b) ���å������", 3, 14);
			prt("  c) ���å��󥹾õ�", 4, 14);
			prt("  d) ���å����ղ�", 5, 14);
			prt("  e) ���/�ɶ񶯲�", 6, 14);
			if (!get_com(format("�ɤ�ǽ�Ϥ�%s�ޤ���:", only_browse ? "Ĵ��" : "�Ȥ�"), &choice, TRUE))
#else
			prt("  a) List essences", 2, 14);
			prt("  b) Extract essence", 3, 14);
			prt("  c) Remove essence", 4, 14);
			prt("  d) Add essence", 5, 14);
			prt("  e) Enchant weapon/armor", 6, 14);
			if (!get_com("Command :", &choice, TRUE))
#endif
			{
				screen_load();
				return;
			}
			switch (choice)
			{
			case 'A':
			case 'a':
				mode = 1;
				break;
			case 'B':
			case 'b':
				mode = 2;
				break;
			case 'C':
			case 'c':
				mode = 3;
				break;
			case 'D':
			case 'd':
				mode = 4;
				break;
			case 'E':
			case 'e':
				mode = 5;
				break;
			}
		}
	}

	if (only_browse)
	{
		char temp[62*5];
		int line, j;

		/* Clear lines, position cursor  (really should use strlen here) */
		Term_erase(14, 21, 255);
		Term_erase(14, 20, 255);
		Term_erase(14, 19, 255);
		Term_erase(14, 18, 255);
		Term_erase(14, 17, 255);
		Term_erase(14, 16, 255);

		roff_to_buf( kaji_tips[mode-1],62,temp);
		for(j=0, line = 17;temp[j];j+=(1+strlen(&temp[j])))
		{
			prt(&temp[j], line, 15);
			line++;
		}
		mode = 0;
	}
	if (!only_browse) screen_load();
	} while (only_browse);
#ifdef ALLOW_REPEAT
	repeat_push(mode);
        }
#endif /* ALLOW_REPEAT */

	switch(mode)
	{
		case 1: display_essence();break;
		case 2: drain_essence();break;
		case 3: erase_essence();break;
		case 4:
			mode = choose_essence();
			if (mode == 0)
				break;
			add_essence(mode);
			break;
		case 5: add_essence(6);break;
	}
}
