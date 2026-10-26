/**
 *    Name:        objects/rings.c
 *    Description: rings: the stage rows, spinning, carried and scattered rings, sparkles; the unused gauge ring and template spawner
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Spawns the stage's ring rows (stage_template_spawns, cursor set on the stage's first call): once
   scroll_position reaches a record, count objects of its stage_object_templates record from x, y, dx, dy
   apart, each var_82 above its floor with a pool 2 shadow. A record passed by 0x100 is skipped; waits
   while a pool has fewer than count blocks. */
void spawn_ring_rows(uint32_t *p_stage_list_offset, uint32_t *p_free_stack, uint32_t *p_slope_height, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint16_t scroll_past_row = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, row_cursor = 0, template_cursor = 0;
	uint16_t rings_left = 0;
	uint32_t old_init_flags = 0;

	old_init_flags = stage_init_flags;
	stage_init_flags = old_init_flags | 4u;
	if ((old_init_flags & 4u) == 0u)
	{
		*p_stage_list_offset = stage_template_spawns[stage];
		ring_spawn_cursor = stage_template_spawns_ADDRESS + *p_stage_list_offset;
	}
	row_cursor = ring_spawn_cursor;
	if (scroll_position < RING_ROW(row_cursor)->position)
	{
		return;
	}
	scroll_past_row = (uint16_t)(scroll_position - RING_ROW(row_cursor)->position);
	row_cursor += 2u;
	if ((int16_t)scroll_past_row >= 0x100)
	{
		ring_spawn_cursor += 0xCu;
		*p_object = object;
		return;
	}
	if (free_object_stack < RING_ROW(row_cursor - 0x2u)->ring_count || free_object_stack_2[0] < RING_ROW(row_cursor - 0x2u)->ring_count)
	{
		return;
	}
	ring_row_template = RING_ROW(row_cursor - 0x2u)->template_offset;
	row_cursor += 2u;
	ring_row_rings_left = RING_ROW(row_cursor - 0x4u)->ring_count;
	row_cursor += 2u;
	ring_row_x = RING_ROW(row_cursor - 0x6u)->x;
	row_cursor += 2u;
	ring_row_y = RING_ROW(row_cursor - 0x8u)->y;
	row_cursor += 2u;
	ring_row_dx = RING_ROW(row_cursor - 0xAu)->dx;
	ring_row_dy = RING_ROW(row_cursor - 0xAu)->dy;
	ring_row_next_80 = 0u;
	do
	{
		allocate_object_outside_loop(&ignored, &object);
		template_cursor = stage_object_templates_ADDRESS + ring_row_template;
		OBJECT(object)->sprite_flags = STAGE_OBJECT_TEMPLATE(template_cursor)->sprite_flags;
		template_cursor += 2u;
		OBJECT(object)->sprite_frame = STAGE_OBJECT_TEMPLATE(template_cursor - 0x2u)->sprite_frame;
		template_cursor += 2u;
		OBJECT(object)->sprite_palette = STAGE_OBJECT_TEMPLATE(template_cursor - 0x4u)->sprite_palette;
		template_cursor += 2u;
		OBJECT(object)->hitbox_size_x = STAGE_OBJECT_TEMPLATE(template_cursor - 0x6u)->hitbox_size_x;
		OBJECT(object)->hitbox_size_y = 0u;
		template_cursor += 2u;
		OBJECT(object)->row_ring.hover_height = STAGE_OBJECT_TEMPLATE(template_cursor - 0x8u)->hover_height;
		template_cursor += 2u;
		OBJECT(object)->animation_speed = STAGE_OBJECT_TEMPLATE(template_cursor - 0xAu)->animation_speed;
		template_cursor++;
		OBJECT(object)->animation_timer = STAGE_OBJECT_TEMPLATE(template_cursor - 0xBu)->animation_timer;
		template_cursor++;
		OBJECT(object)->animation_frame = STAGE_OBJECT_TEMPLATE(template_cursor - 0xCu)->animation_frame;
		template_cursor++;
		OBJECT(object)->kind = STAGE_OBJECT_TEMPLATE(template_cursor - 0xDu)->kind;
		template_cursor++;
		OBJECT(object)->animation = STAGE_OBJECT_TEMPLATE(template_cursor - 0xEu)->animation;
		template_cursor += 4u;
		OBJECT(object)->handler = STAGE_OBJECT_TEMPLATE(template_cursor - 0x12u)->handler;
		template_cursor += 4u;
		OBJECT(object)->sprite_offset_x = STAGE_OBJECT_TEMPLATE(template_cursor - 0x16u)->sprite_offset_x;
		template_cursor++;
		OBJECT(object)->sprite_offset_y = STAGE_OBJECT_TEMPLATE(template_cursor - 0x17u)->sprite_offset_y;
		OBJECT(object)->hitbox_height = STAGE_OBJECT_TEMPLATE(template_cursor - 0x17u)->hitbox_height;
		OBJECT(object)->row_ring.spawn_scroll_position = scroll_position;
		OBJECT(object)->row_ring.glint_timer = ring_row_next_80;
		ring_row_next_80 += 0x20u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->x_int = ring_row_x;
		OBJECT(object)->x_fraction = 0u;
		OBJECT(object)->y_int = ring_row_y;
		OBJECT(object)->y_fraction = 0u;
		ring_row_x += ring_row_dx;
		ring_row_y += ring_row_dy;
		current_object = object;
		find_floor(&ignored, &ignored_2, &ignored_3, p_slope_height, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z_int -= OBJECT(current_object)->row_ring.hover_height;
		project_object(&ignored, p_stage_list_offset);
		object_cursor = current_object;
		allocate_object_2(&ignored, p_free_stack, &object);
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_frame = 0x5D90u;
		OBJECT(object)->shadow_blink = 0u;
		OBJECT(object_cursor)->child = object;
		OBJECT(object)->parent = current_object;
		rings_left = ring_row_rings_left;
		ring_row_rings_left = rings_left - 1u;
	} while ((int16_t)rings_left > 1);
	ring_spawn_cursor += 0xCu;
	*p_object = object;
}

/* Handler of the spinning ring: removed with its shadow while map_screen_active is set or in game mode
   0x20; touched (flags bit 2), it is taken (sound 0x13, spawn_ring_sparkle, removed with its shadow)
   unless the toucher is a player in state 0xD or 0x20 whose hurt_contact_delay is running. Otherwise
   ring_spin_keep_spinning. */
void ring_spin(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0, player = 0;

	if (map_screen_active != 0u || current_game_mode == 0x20u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		ring_spin_keep_spinning(player);
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((int8_t)OBJECT(object)->kind <= 2)
	{
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			ring_spin_keep_spinning(player);
			return;
		}
	}
	sound_id = 0x13u;
	play_sound(sound_id, &ignored, &ignored_2);
	spawn_ring_sparkle(player);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

/* The spinning ring's frame while not taken: animates (a glint at step 0x15, unused_glint_timer 0x78),
   removed with its shadow once off the screen it was on, else touchable, drawn with its shadow. On floor
   type 7 the shadow block shows the ring again as far below the floor as it is above, unless bit 4 is
   set in step_accumulator of whatever block current_player holds. */
void ring_spin_keep_spinning(uint32_t sparkle)
{
	uint16_t child = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	animate(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0x15u)
	{
		OBJECT(current_object)->ring_spin.unused_glint_timer = 0x78u;
		spawn_ring_glint(sparkle);
	}
	update_screen_position(&ignored_4, &ignored);
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	add_to_touchable_list(ignored_4);
	queue_sprite_at_floor();
	if (OBJECT(current_object)->floor_type != 7u || (PLAYER(current_player)->step_accumulator & 0x10u) != 0u)
	{
		draw_child_shadow();
		return;
	}
	if (OBJECT(current_object)->floor_block >= 0 && (int16_t)OBJECT(current_object)->child >= 0)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x2000u;
		OBJECT(current_object)->sprite_flags = 0x1004u;
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + OBJECT(object_cursor)->sprite_offset_x;
		OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(object_cursor)->projected_y + ((uint32_t)OBJECT(object_cursor)->floor_z - OBJECT(object_cursor)->z + OBJECT(object_cursor)->floor_z + 0x180000u)) >> 16) - view_y - OBJECT(object_cursor)->sprite_offset_y;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
		queue_sprite_at_depth(&ignored_4, &depth);
	}
}

/* Handler of a ring that does not spin (stage_object_templates 2): touched, sound 0x13 and removed with
   its shadow; removed too once off the screen; else on the touchable list and drawn with its shadow. */
void still_ring_wait(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, off_screen = 0;

	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		sound_id = 0x13u;
		play_sound(sound_id, &ignored, &ignored_2);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	update_screen_position(&ignored_3, &ignored);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		add_to_touchable_list(ignored_3);
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

/* Handler of the carried rings (stage_object_templates 1 and 3) until they stand on a platform (flags
   bit 3), then carried_ring_spin; meanwhile on the rider list and not drawn, its shadow's palette set to
   0x87A0. */
void carried_ring_wait(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = carried_ring_spin_ADDRESS;
	add_to_rider_list();
	OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_palette = 0x87A0u;
}

/* Handler of a ring that rides a platform: removed with its shadow while map_screen_active is set;
   touched (flags bit 2), it is taken (sound 0x13, spawn_ring_sparkle, removed with its shadow) unless
   the toucher is a player in state 0xD or 0x20 whose hurt_contact_delay is running. Otherwise
   carried_ring_keep_spinning. */
void carried_ring_spin(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0, player = 0;

	if (map_screen_active != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		carried_ring_keep_spinning(player);
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((int8_t)OBJECT(object)->kind <= 2)
	{
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			carried_ring_keep_spinning(player);
			return;
		}
	}
	sound_id = 0x13u;
	play_sound(sound_id, &ignored, &ignored_2);
	spawn_ring_sparkle(player);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

/* The frame of a ring riding a platform while it is not taken: flags bit 4 copies bit 3; the ring floats
   hover_height above its platform's z on the platform's floor_block, animates (a glint at step 0x15,
   glint_timer 0x78), is touchable and drawn with its shadow; once off the screen it was on, removed with
   its shadow. */
void carried_ring_keep_spinning(uint32_t sparkle)
{
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, off_screen = 0, platform_z = 0;

	OBJECT(current_object)->flags &= ~0x10u;
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->flags |= 0x10u;
	object = LINKED(current_object, OBJECT(current_object)->platform);
	platform_z = OBJECT(object)->z;
	OBJECT(current_object)->floor_z = platform_z;
	OBJECT(current_object)->z = platform_z;
	OBJECT(current_object)->z_int -= OBJECT(current_object)->carried_ring.hover_height;
	OBJECT(current_object)->floor_block = OBJECT(object)->floor_block;
	animate(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0x15u)
	{
		OBJECT(current_object)->carried_ring.glint_timer = 0x78u;
		OBJECT(current_object)->carried_ring.glint_timer_high = 0u;
		spawn_ring_glint(sparkle);
	}
	update_screen_position(&ignored_4, &ignored);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		add_to_touchable_list(ignored_4);
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

/* Unused: an empty return left after carried_ring_keep_spinning's code; it does nothing. */
void carried_ring_spin_spare_return(void)
{
}

/* scatter_rings with scatter_fixed 0xFFFF: the rings get no speed and no jump and fall from the current
   object where they are (an odd count still draws a direction). */
void scatter_rings_fixed(uint32_t count, uint32_t *p_random_value, uint32_t *p_allocated)
{
	uint32_t ignored = 0;

	scatter_fixed = 0xFFFFu;
	scatter_rings(count, p_random_value, p_allocated, &ignored);
}

/* scatter_rings with scatter_fixed cleared first, so the rings get a random speed and jump (until
   scatter_rings' kept direction turns them off for the rings after the first odd count). */
void scatter_rings_random(uint32_t random_value, uint32_t count, uint32_t *p_allocated, uint32_t *p_ring)
{
	scatter_fixed = 0u;
	scatter_rings(count, &random_value, p_allocated, p_ring);
}

/* Throws count rings (at most 10) from the current object, kind 3 with a pool 2 shadow, handler
   scattered_ring_wait, until a pool runs out. A ring at an odd count takes a random direction, kept in
   scatter_fixed, the next its mirror; speed and jump are random only while scatter_fixed's bytes are 0,
   so the kept direction leaves later rings without them. */
void scatter_rings(uint32_t count, uint32_t *p_random_value, uint32_t *p_allocated, uint32_t *p_ring)
{
	uint32_t random_value = *p_random_value;
	uint32_t allocated = *p_allocated;
	uint32_t ring = *p_ring;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_4 = 0, ignored_5 = 0;

	if ((int16_t)count > 0xA)
		count = (count & ~0xFFFFu) | 0xAu;
	do
	{
		if (free_object_stack_2[0] <= 0)
			break;
		allocate_object(&free_count, &allocated);
		if (free_count == 0u)
			break;
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_frame = 0x5E70u;
		OBJECT(allocated)->sprite_palette = 0x8430u;
		OBJECT(allocated)->hitbox_size_x = 0x10u;
		OBJECT(allocated)->hitbox_size_y = 0u;
		OBJECT(allocated)->hitbox_height = 0x20u;
		OBJECT(allocated)->kind = 3u;
		OBJECT(allocated)->x = OBJECT(current_object)->x;
		OBJECT(allocated)->y = OBJECT(current_object)->y;
		OBJECT(allocated)->z = OBJECT(current_object)->z;
		OBJECT(allocated)->animation_speed = 2u;
		OBJECT(allocated)->animation_timer = 2;
		OBJECT(allocated)->animation_frame = 0u;
		OBJECT(allocated)->animation = ring_animation_ADDRESS;
		OBJECT(allocated)->flags = 0x204u;
		OBJECT(allocated)->scattered_ring.lifetime = 0x12C;
		OBJECT(allocated)->scattered_ring.pickup_delay = 0x1E;
		OBJECT(allocated)->sprite_offset_x = 0;
		OBJECT(allocated)->sprite_offset_y = 0;
		OBJECT(allocated)->handler = scattered_ring_wait_ADDRESS;
		OBJECT(allocated)->speed_z = 0;
		if (scatter_fixed_jump == 0u)
		{
			next_random(&random_value, &ignored, &ignored_2);
			random_value = (random_value & 0x1FFFFu) + 0x80000u;
			OBJECT(allocated)->speed_z = 0u - random_value;
		}
		OBJECT(allocated)->speed = 0;
		if ((uint8_t)scatter_fixed == 0u)
		{
			next_random(&random_value, &ignored, &ignored_2);
			random_value = (random_value & 0x7FFFFFu) + 0x800000u;
			OBJECT(allocated)->speed = random_value;
		}
		if ((count & 1u) == 0u)
			OBJECT(allocated)->direction = 0u - (scatter_fixed - 0x200u) + 0x200u;
		else
		{
			next_random(&random_value, &ignored, &ignored_2);
			random_value = (random_value & ~0xFFFFu) | (random_value & 0xFFu);
			random_value = (random_value & ~0xFFFFu) | (uint16_t)(random_value + 0x180u);
			OBJECT(allocated)->direction = random_value;
			scatter_fixed = random_value;
		}
		current_object = allocated;
		set_speed_from_direction();
		if ((OBJECT(object_cursor)->flags & 0x10u) != 0u)
		{
			OBJECT(current_object)->flags |= 8u;
			OBJECT(current_object)->platform = OBJECT(object_cursor)->platform;
		}
		ring = allocated;
		allocate_object_2(&ignored_4, &ignored_5, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1000u;
		OBJECT(allocated)->sprite_palette = 0x9FFEu;
		OBJECT(allocated)->sprite_frame = 0x5D90u;
		OBJECT(ring)->child = allocated;
		OBJECT(allocated)->parent = ring;
		count--;
	} while (count != 0u);
	current_object = object_cursor;
	*p_random_value = random_value;
	*p_allocated = allocated;
	*p_ring = ring;
}

/* Handler of a ring just thrown: moves it (move_scattered_ring) while pickup_delay counts down with
   flags bit 9 set (players ignore its touches); then clears the bit and goes on to scattered_ring_live. */
void scattered_ring_wait(void)
{
	uint16_t pickup_delay = 0;

	pickup_delay = OBJECT(current_object)->scattered_ring.pickup_delay;
	OBJECT(current_object)->scattered_ring.pickup_delay = pickup_delay - 1u;
	if ((int16_t)pickup_delay > 1)
	{
		move_scattered_ring(0u);
		return;
	}
	OBJECT(current_object)->flags &= ~0x200u;
	OBJECT(current_object)->handler = scattered_ring_live_ADDRESS;
	scattered_ring_live();
}

/* Handler of a scattered ring that can be taken: after lifetime frames it shrinks away
   (scattered_ring_vanish); touched (flags bit 2), it is taken (sound 0x13, spawn_ring_sparkle, removed
   with its shadow) unless the toucher is a player in state 0xD or 0x20 whose hurt_contact_delay is
   running; otherwise move_scattered_ring. */
void scattered_ring_live(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0, player = 0;
	uint16_t lifetime = 0;

	lifetime = OBJECT(current_object)->scattered_ring.lifetime;
	OBJECT(current_object)->scattered_ring.lifetime = lifetime - 1u;
	if ((int16_t)lifetime <= 1)
	{
		OBJECT(current_object)->sprite_flags = 4u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(current_object)->handler = scattered_ring_vanish_ADDRESS;
		scattered_ring_vanish();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		move_scattered_ring(player);
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((int8_t)OBJECT(object)->kind <= 2)
	{
		player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
		if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
		{
			move_scattered_ring(player);
			return;
		}
	}
	sound_id = 0x13u;
	play_sound(sound_id, &ignored, &ignored_2);
	spawn_ring_sparkle(player);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

/* Moves a scattered ring by speed_x/speed_y, stepping back and turning round when the floor ahead is
   more than 8 above it; gravity 0x6000, bouncing with 3/4 of speed_z; animates with a glint. On screen
   it joins the touchable and rider lists and is drawn with its shadow; off it (margins 0x20, 0x58)
   scattered_ring_vanish. */
void move_scattered_ring(uint32_t cursor)
{
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, object = 0, off_screen = 0, rebound_speed = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &cursor);
	if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) > 8)
	{
		OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		OBJECT(current_object)->speed_y = 0u - OBJECT(current_object)->speed_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		rebound_speed = 0u - OBJECT(current_object)->speed_z;
		OBJECT(current_object)->speed_z = rebound_speed - ((int32_t)rebound_speed >> 2);
	}
	project_object(&ignored, &ignored_2);
	animate(&ignored_2, &ignored_5, &ignored_6);
	if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0x15u)
		spawn_ring_glint(cursor);
	margin_x = 0x20u;
	margin_y = 0x58u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen == 0u)
	{
		OBJECT(current_object)->flags &= ~0x10u;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->flags |= 0x10u;
		add_to_touchable_list(ignored);
		add_to_rider_list();
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(current_object)->handler = scattered_ring_vanish_ADDRESS;
	scattered_ring_vanish();
}

/* Handler of a scattered ring going away: drifts on by speed_x/speed_y while it and its shadow shrink by
   0x10 of sprite_scale a frame; removed with its shadow once it has shrunk away. */
void scattered_ring_vanish(void)
{
	uint16_t child = 0;
	uint32_t ignored_2 = 0, ignored_3 = 0, object = 0;
	uint16_t sprite_scale = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->sprite_scale -= 0x10u;
	sprite_scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_scale = sprite_scale - 0x10u;
	if ((int16_t)sprite_scale > 0x10)
	{
		project_object(&ignored_2, &ignored_3);
		queue_sprite_at_floor();
		draw_child_shadow();
		return;
	}
	child = OBJECT(current_object)->child;
	free_object_2(child);
	delete_current_object();
}

/* Spawns the sparkle of a taken ring at the current object: ring_sparkle_animation, drawn 0x10 up
   (init_sparkle_object); nothing when no block is free. */
void spawn_ring_sparkle(uint32_t sparkle)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &sparkle);
	if (free_count == 0u)
		return;
	OBJECT(sparkle)->animation = ring_sparkle_animation_ADDRESS;
	OBJECT(sparkle)->sprite_offset_x = 0;
	OBJECT(sparkle)->sprite_offset_y = -0x10;
	init_sparkle_object(sparkle);
}

/* Spawns a glint on the current ring: ring_glint_animation, drawn 0x10 right of it and 8 up
   (init_sparkle_object); nothing when no block is free. */
void spawn_ring_glint(uint32_t sparkle)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &sparkle);
	if (free_count == 0u)
		return;
	OBJECT(sparkle)->animation = ring_glint_animation_ADDRESS;
	OBJECT(sparkle)->sprite_offset_x = 0x10;
	OBJECT(sparkle)->sprite_offset_y = -8;
	init_sparkle_object(sparkle);
}

/* Sets up the new sparkle or glint sparkle at the current object's x, y, z and floor_z: sprite_flags
   0x1000, palette 0x1B0, flags 0, handler sparkle_start, its animation to start from the first step at 2
   frames a step. */
void init_sparkle_object(uint32_t sparkle)
{
	OBJECT(sparkle)->sprite_flags = 0x1000u;
	OBJECT(sparkle)->sprite_palette = 0x1B0u;
	OBJECT(sparkle)->flags = 0u;
	OBJECT(sparkle)->handler = sparkle_start_ADDRESS;
	OBJECT(sparkle)->x = OBJECT(current_object)->x;
	OBJECT(sparkle)->y = OBJECT(current_object)->y;
	OBJECT(sparkle)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(sparkle)->z = OBJECT(current_object)->z;
	OBJECT(sparkle)->animation_frame = 0xFFu;
	OBJECT(sparkle)->animation_timer = 0;
	OBJECT(sparkle)->animation_speed = 2u;
}

/* First frame of a sparkle or glint: projects it to the screen, then becomes sparkle_animate. */
void sparkle_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = sparkle_animate_ADDRESS;
	sparkle_animate();
}

/* Handler of a sparkle or glint: animates it and draws it in front of its floor point (depth: the
   floor's screen y + 0x10); removed at the end of its animation. */
void sparkle_animate(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((int8_t)at_last_step >= 0)
	{
		update_screen_position(&ignored_3, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + 0x10u);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* Unused: spawns a ring that flies from the current object (its parent, 0x20 above it on the screen,
   kind copied) to its player's health gauge: ring_animation, sound 0x7F, speed_x to reach the gauge's x
   + 0x10 (hud_gauge_positions_3p) in 0x3C frames, handler gauge_ring_update. */
void spawn_gauge_ring(uint32_t ring)
{
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0;

	allocate_object(&free_count, &ring);
	if (free_count != 0u)
	{
		OBJECT(ring)->sprite_flags = 0u;
		OBJECT(ring)->sprite_palette = 0x8430u;
		OBJECT(ring)->sprite_frame = 0x5E70u;
		OBJECT(ring)->sprite_offset_x = 0;
		OBJECT(ring)->sprite_offset_y = 0;
		OBJECT(ring)->handler = gauge_ring_update_ADDRESS;
		OBJECT(ring)->flags = 0u;
		OBJECT(ring)->sprite_scale = 0x100u;
		OBJECT(ring)->animation = ring_animation_ADDRESS;
		OBJECT(ring)->animation_speed = 4u;
		OBJECT(ring)->animation_timer = 0;
		OBJECT(ring)->animation_frame = 0u;
		OBJECT(ring)->parent = current_object;
		OBJECT(ring)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(ring)->screen_y = OBJECT(current_object)->screen_y - 0x20u;
		OBJECT(ring)->x_int = OBJECT(current_object)->screen_x;
		OBJECT(ring)->x_fraction = 0u;
		OBJECT(ring)->y_int = OBJECT(ring)->screen_y;
		OBJECT(ring)->y_fraction = 0u;
		OBJECT(ring)->gauge_ring.base_y = OBJECT(ring)->screen_y;
		OBJECT(ring)->direction = 0u;
		OBJECT(ring)->gauge_ring.step = 0u;
		OBJECT(ring)->kind = OBJECT(current_object)->kind;
		OBJECT(ring)->speed_x = (int32_t)((uint32_t)(uint16_t)(hud_gauge_positions_3p[OBJECT(current_object)->player].x + 0x10u - OBJECT(ring)->screen_x) << 16) / 0x3C;
		OBJECT(ring)->gauge_ring.turn_speed = 0x88u;
		OBJECT(ring)->gauge_ring.hop_timer = 0x1E;
		sound_id = 0x7Fu;
		play_sound(sound_id, &ignored, &ignored_2);
	}
}

/* Unused: the gauge ring's handler: step 0 gauge_ring_rise, step 1 gauge_ring_fall_into_gauge (nothing
   sets step 2), then animates it and queues its sprite. */
void gauge_ring_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	switch (OBJECT(current_object)->gauge_ring.step)
	{
	case 0:
		gauge_ring_rise();
		break;
	case 1:
		gauge_ring_fall_into_gauge();
		break;
	case 2:
		gauge_ring_rise();
		break;
	default:
		invalid_jump(0x1EC2Du + OBJECT(current_object)->gauge_ring.step * 3u);
	}
	animate(&ignored, &ignored_2, &ignored_3);
	ignored = queue_sprite(ignored);
}

/* Unused: the gauge ring's step 0: moves on by speed_x while it rises along a sine arc up to 0x40 above
   base_y (direction + turn_speed a frame); after hop_timer frames step 1, with base_y at its gauge's y
   (hud_gauge_positions_3p[kind].y) and drop_height from the distance down to it. */
void gauge_ring_rise(void)
{
	uint16_t gauge_y = 0, y = 0;
	uint32_t direction = 0;
	uint8_t hop_timer = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->gauge_ring.turn_speed;
	OBJECT(current_object)->direction &= 0x3FFFu;
	direction = OBJECT(current_object)->direction;
	y = (uint16_t)(((int16_t)(0u - sine_table[direction >> 4]) >> 8) + OBJECT(current_object)->gauge_ring.base_y);
	OBJECT(current_object)->y_int = y;
	OBJECT(current_object)->screen_y = y;
	hop_timer = OBJECT(current_object)->gauge_ring.hop_timer;
	OBJECT(current_object)->gauge_ring.hop_timer = hop_timer - 1u;
	if ((int8_t)hop_timer <= 1)
	{
		OBJECT(current_object)->gauge_ring.step++;
		gauge_y = (uint16_t)hud_gauge_positions_3p[OBJECT(current_object)->kind].y;
		OBJECT(current_object)->gauge_ring.base_y = gauge_y;
		OBJECT(current_object)->gauge_ring.drop_height = 0u - (uint16_t)((uint32_t)(gauge_y - OBJECT(current_object)->screen_y) << 2);
		OBJECT(current_object)->gauge_ring.turn_speed = 0x88u;
	}
}

/* Unused: the gauge ring's step 1: moves on by speed_x and drops along a sine arc from base_y by
   drop_height onto its gauge while it shrinks (sprite_scale - 8 a frame); when it has shrunk away the
   parent's health goes up by 1 and the ring is removed. */
void gauge_ring_fall_into_gauge(void)
{
	uint32_t direction = 0, object = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->gauge_ring.turn_speed;
	OBJECT(current_object)->direction &= 0x3FFFu;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->y = (uint32_t)sine_table[direction >> 4] * OBJECT(current_object)->gauge_ring.drop_height + ((uint32_t)OBJECT(current_object)->gauge_ring.base_y << 16);
	OBJECT(current_object)->screen_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->sprite_scale -= 8u;
	if ((int16_t)OBJECT(current_object)->sprite_scale > 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(object)->health++;
	delete_current_object();
}

/* Unused: when pool 2 has a block, spawns stage_object_templates' first record (the spinning ring) at x,
   y, z (kept in template_spawn_x/_y/_z) with scroll_position in its health halfword, projected,
   find_floor, and a pool 2 shadow child; current_object is kept, current_player is left on the new
   object. */
void spawn_stage_template_0(uint16_t x, uint16_t y, uint16_t z, uint32_t object)
{
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint32_t saved_current_object = 0;

	saved_current_object = current_object;
	template_spawn_x = x;
	template_spawn_y = y;
	template_spawn_z = z;
	if (free_object_stack_2[0] > 0)
	{
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = stage_object_templates;
			OBJECT(object)->sprite_frame = stage_template_0_sprite_frame;
			OBJECT(object)->sprite_palette = stage_template_0_sprite_palette;
			OBJECT(object)->hitbox_size_x = stage_template_0_hitbox_size;
			OBJECT(object)->hitbox_size_y = 0u;
			OBJECT(object)->animation_speed = stage_template_0_animation_speed;
			OBJECT(object)->animation_timer = stage_template_0_animation_timer;
			OBJECT(object)->animation_frame = stage_template_0_animation_frame;
			OBJECT(object)->kind = stage_template_0_kind;
			OBJECT(object)->animation = stage_template_0_animation;
			OBJECT(object)->handler = stage_template_0_handler;
			OBJECT(object)->sprite_offset_x = stage_template_0_offset_x;
			OBJECT(object)->sprite_offset_y = stage_template_0_offset_y;
			OBJECT(object)->hitbox_height = stage_template_0_hitbox_height;
			OBJECT(object)->ring_spin.spawn_scroll_position = scroll_position;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->x_int = template_spawn_x;
			OBJECT(object)->x_fraction = 0u;
			OBJECT(object)->y_int = template_spawn_y;
			OBJECT(object)->y_fraction = 0u;
			OBJECT(object)->z_int = template_spawn_z;
			OBJECT(object)->z_fraction = 0u;
			current_object = object;
			project_object(&ignored, &ignored_2);
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
			current_player = current_object;
			allocate_object_2(&ignored, &ignored_3, &object);
			OBJECT(object)->sprite_flags = 0x1000u;
			OBJECT(object)->sprite_palette = 0x9FFEu;
			OBJECT(object)->sprite_frame = 0x5D90u;
			OBJECT(current_player)->child = object;
			OBJECT(object)->parent = current_player;
		}
	}
	current_object = saved_current_object;
}

/* Unused: spawn_stage_template_0 with record template_index of stage_object_templates (kept in
   template_spawn_index) in place of the first. */
void spawn_stage_template(uint16_t template_index, uint16_t x, uint16_t y, uint16_t z, uint32_t object)
{
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, template_cursor = 0;
	uint32_t saved_current_object = 0;

	saved_current_object = current_object;
	template_spawn_x = x;
	template_spawn_y = y;
	template_spawn_z = z;
	template_spawn_index = template_index;
	if (free_object_stack_2[0] > 0)
	{
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			template_cursor = stage_object_templates_ADDRESS + template_spawn_index * 0x1Au;
			OBJECT(object)->sprite_flags = STAGE_OBJECT_TEMPLATE(template_cursor)->sprite_flags;
			template_cursor += 2u;
			OBJECT(object)->sprite_frame = STAGE_OBJECT_TEMPLATE(template_cursor - 0x2u)->sprite_frame;
			template_cursor += 2u;
			OBJECT(object)->sprite_palette = STAGE_OBJECT_TEMPLATE(template_cursor - 0x4u)->sprite_palette;
			template_cursor += 2u;
			OBJECT(object)->hitbox_size_x = STAGE_OBJECT_TEMPLATE(template_cursor - 0x6u)->hitbox_size_x;
			OBJECT(object)->hitbox_size_y = 0u;
			template_cursor += 4u;
			OBJECT(object)->animation_speed = STAGE_OBJECT_TEMPLATE(template_cursor - 0xAu)->animation_speed;
			template_cursor++;
			OBJECT(object)->animation_timer = STAGE_OBJECT_TEMPLATE(template_cursor - 0xBu)->animation_timer;
			template_cursor++;
			OBJECT(object)->animation_frame = STAGE_OBJECT_TEMPLATE(template_cursor - 0xCu)->animation_frame;
			template_cursor++;
			OBJECT(object)->kind = STAGE_OBJECT_TEMPLATE(template_cursor - 0xDu)->kind;
			template_cursor++;
			OBJECT(object)->animation = STAGE_OBJECT_TEMPLATE(template_cursor - 0xEu)->animation;
			template_cursor += 4u;
			OBJECT(object)->handler = STAGE_OBJECT_TEMPLATE(template_cursor - 0x12u)->handler;
			template_cursor += 4u;
			OBJECT(object)->sprite_offset_x = STAGE_OBJECT_TEMPLATE(template_cursor - 0x16u)->sprite_offset_x;
			template_cursor++;
			OBJECT(object)->sprite_offset_y = STAGE_OBJECT_TEMPLATE(template_cursor - 0x17u)->sprite_offset_y;
			OBJECT(object)->hitbox_height = STAGE_OBJECT_TEMPLATE(template_cursor - 0x17u)->hitbox_height;
			OBJECT(object)->ring_spin.spawn_scroll_position = scroll_position;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->x_int = template_spawn_x;
			OBJECT(object)->x_fraction = 0u;
			OBJECT(object)->y_int = template_spawn_y;
			OBJECT(object)->y_fraction = 0u;
			OBJECT(object)->z_int = template_spawn_z;
			OBJECT(object)->z_fraction = 0u;
			current_object = object;
			project_object(&ignored, &ignored_2);
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
			current_player = current_object;
			allocate_object_2(&ignored, &ignored_3, &object);
			OBJECT(object)->sprite_flags = 0x1000u;
			OBJECT(object)->sprite_palette = 0x9FFEu;
			OBJECT(object)->sprite_frame = 0x5D90u;
			OBJECT(current_player)->child = object;
			OBJECT(object)->parent = current_player;
		}
	}
	current_object = saved_current_object;
}
