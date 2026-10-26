/**
 *    Name:        objects/shared.c
 *    Description: objects several stages use: the signs, the scenery sprites, the walls players and objects bounce off, the zones behind the chasers, the pacer that keeps two players together, the hold on a group leader; object types 4 and 13; the crawler, which nothing spawns
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Cycles the current object's palette: every palette_delay frames palette_index steps on through the
   list at animate_sign.palette_list (a word with the last index, then the palettes), back to 0 after the
   last, and sprite_palette takes that entry. */
void animate_sign(void)
{
	uint8_t last_index = 0;
	uint32_t next_index = 0, palettes = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->animate_sign.palette_timer;
	OBJECT(current_object)->animate_sign.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->animate_sign.palette_timer = OBJECT(current_object)->animate_sign.palette_delay;
	palettes = OBJECT(current_object)->animate_sign.palette_list;
	last_index = (uint8_t)SPRITE_PALETTE_CYCLE(palettes)->last_index;
	next_index = OBJECT(current_object)->animate_sign.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->animate_sign.palette_index = next_index;
	OBJECT(current_object)->sprite_palette = SPRITE_CYCLE_PALETTES(palettes)[next_index];
}

/* Object type 13: unprojects itself, then place_sign makes it sign kind spawn_arg_1 at its own x, y, z,
   shown for spawn_arg_2 frames (stage 4's "BREAK!!", stage 9's "CATCH!!"). */
void sign_start_from_type(void)
{
	uint16_t spawn_arg_1 = 0, x_int = 0, y_int = 0, z_int = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	z_int = (uint16_t)OBJECT(current_object)->z_int;
	object = current_object;
	place_sign(x_int, y_int, spawn_arg_1, z_int, &ignored_5, &object);
	OBJECT(object)->speed_x_fraction = OBJECT(current_object)->spawn_arg_2;
}

/* Object type 21, which no stage spawns: eight 2-part pieces stepped from the object by
   crawler_segment_offsets (kind 4, box 0x29 x 0x2F, crawler_segment_animate, staggered step timers and
   walk steps); the last becomes the tail (kind 0xD, crawler_segment_update), the object the head (kind
   0xD, crawler_head_update). All have shadows. */
void crawler_init(void)
{
	uint8_t segment_walk_step = 0;
	uint16_t segment_x = 0, step_stagger = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, offset_record = 0, rom_record = 0, variant = 0;

	variant = OBJECT(current_object)->crawler.variant;
	r14 = 8u;
	step_stagger = 0u;
	segment_walk_step = 0u;
	segment_x = 0u;
	current_player = 0u;
	do
	{
		spawn_count = 2u;
		spawn_object_with_pieces(0x01276Bu, &ignored, &ignored_2, &free_count, &object, &block);
		copy_position_to(object);
		offset_record = crawler_segment_offsets_ADDRESS + variant * 4u;
		segment_x += CRAWLER_PIECE_OFFSET(offset_record)->dx;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player + CRAWLER_PIECE_OFFSET(offset_record)->dy);
		OBJECT(object)->projected_x += segment_x;
		OBJECT(object)->projected_y_int += current_player;
		segment_walk_step++;
		if ((int8_t)segment_walk_step > 5)
			segment_walk_step = 0u;
		OBJECT(object)->direction = 0x280u;
		step_stagger += 4u;
		OBJECT(object)->crawler.step_timer = step_stagger;
		OBJECT(object)->crawler.walk_step = segment_walk_step;
		OBJECT(object)->sprite_palette = 0x90A0u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x29u;
		OBJECT(object)->hitbox_size_y = 0x2Fu;
		OBJECT(object)->hitbox_offset_x = 0xA;
		OBJECT(object)->hitbox_offset_y = -0x3D;
		OBJECT(object)->hitbox_height = 0x20u;
		OBJECT(object)->kind = 4u;
		OBJECT(object)->handler = crawler_segment_animate_ADDRESS;
		OBJECT(object)->crawler.variant = OBJECT(object_cursor)->crawler.variant;
		OBJECT(object)->crawler.on_platform = OBJECT(object_cursor)->crawler.on_platform;
		OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
		current_object = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_scale = 0x100u;
		r14 = r14 - 1u;
	} while (r14 != 0u);
	OBJECT(current_object)->direction = 0x280u;
	OBJECT(current_object)->crawler.walk_step = -1;
	rom_record = crawler_tail_offsets_ADDRESS + variant * 4u;
	OBJECT(current_object)->projected_x += CRAWLER_PIECE_OFFSET(rom_record)->dx;
	OBJECT(current_object)->projected_y_int += CRAWLER_PIECE_OFFSET(rom_record)->dy;
	OBJECT(current_object)->sprite_frame = crawler_tail_frames[variant];
	OBJECT(current_object)->sprite_palette = 0x90C0u;
	rom_record = crawler_collision_boxes_ADDRESS + variant * 6u;
	OBJECT(current_object)->hitbox_size_x = HITBOX_SIZE_AND_OFFSET(rom_record)->size_x;
	rom_record += 2u;
	OBJECT(current_object)->hitbox_size_y = HITBOX_SIZE_AND_OFFSET(rom_record - 0x2u)->size_y;
	rom_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = HITBOX_SIZE_AND_OFFSET(rom_record - 0x4u)->offset_x;
	OBJECT(current_object)->hitbox_offset_y = HITBOX_SIZE_AND_OFFSET(rom_record - 0x4u)->offset_y;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0xDu;
	OBJECT(current_object)->handler = crawler_segment_update_ADDRESS;
	OBJECT(object_cursor)->parent = object_cursor;
	rom_record = crawler_head_offsets_ADDRESS + variant * 4u;
	OBJECT(object_cursor)->projected_x += CRAWLER_PIECE_OFFSET(rom_record)->dx;
	OBJECT(object_cursor)->projected_y_int += CRAWLER_PIECE_OFFSET(rom_record)->dy;
	OBJECT(object_cursor)->sprite_palette = 0x90B0u;
	variant = OBJECT(object_cursor)->crawler.variant;
	OBJECT(object_cursor)->sprite_frame = crawler_head_frames[variant];
	rom_record = crawler_collision_boxes_ADDRESS + variant * 6u;
	OBJECT(object_cursor)->hitbox_size_x = HITBOX_SIZE_AND_OFFSET(rom_record)->size_x;
	rom_record += 2u;
	OBJECT(object_cursor)->hitbox_size_y = HITBOX_SIZE_AND_OFFSET(rom_record - 0x2u)->size_y;
	rom_record += 2u;
	OBJECT(object_cursor)->hitbox_offset_x = HITBOX_SIZE_AND_OFFSET(rom_record - 0x4u)->offset_x;
	OBJECT(object_cursor)->hitbox_offset_y = HITBOX_SIZE_AND_OFFSET(rom_record - 0x4u)->offset_y;
	OBJECT(object_cursor)->hitbox_height = 0x20u;
	OBJECT(object_cursor)->kind = 0xDu;
	OBJECT(object_cursor)->flags |= 2u;
	OBJECT(object_cursor)->handler = crawler_head_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	OBJECT(current_object)->sprite_scale = 0x100u;
}

/* The crawler's head: moves, falls to its floor and is removed off screen. Once a player's x comes
   within 0x28 of it (0x35 for variant 1) it sets crawler_head.triggered, which starts the crawl.
   Touchable, a rider, drawn with its shadow (over a pit with on_platform: at its platform's draw_depth -
   0xF, no shadow); then crawler_step. */
void crawler_head_update(void)
{
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, over_pit = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	fall_to_floor(&ignored, &over_pit, &ignored_2);
	OBJECT(current_object)->crawler_head.over_pit = over_pit;
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	find_nearest_player_x();
	if (OBJECT(current_object)->crawler_head.variant == 0u)
	{
		if (nearest_player_distance > 0x28)
		{
			OBJECT(current_object)->direction = 0x280u;
			add_to_touchable_list(deleted);
			add_to_rider_list();
			if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
			{
				depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->draw_depth - 0xFu);
				queue_sprite_at_depth(&ignored, &depth);
				crawler_step();
				return;
			}
			queue_sprite(over_pit);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			place_sprite_at_cursor_floor();
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = object_cursor;
			crawler_step();
			return;
		}
		OBJECT(current_object)->crawler_head.triggered = 1u;
	}
	if (nearest_player_distance > 0x35)
	{
		OBJECT(current_object)->direction = 0x280u;
		add_to_touchable_list(deleted);
		add_to_rider_list();
		if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->draw_depth - 0xFu);
			queue_sprite_at_depth(&ignored, &depth);
			crawler_step();
			return;
		}
		queue_sprite(over_pit);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		crawler_step();
		return;
	}
	OBJECT(current_object)->crawler_head.triggered = 1u;
	OBJECT(current_object)->direction = 0x280u;
	add_to_touchable_list(deleted);
	add_to_rider_list();
	if (OBJECT(current_object)->crawler_head.over_pit != 0u && OBJECT(current_object)->crawler_head.on_platform != 0u)
	{
		depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->draw_depth - 0xFu);
		queue_sprite_at_depth(&ignored, &depth);
		crawler_step();
		return;
	}
	queue_sprite(over_pit);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	crawler_step();
}

/* Every 4 frames the next of the variant's six walking frames (crawler_walk_steps into
   crawler_segment_frames, walk_step back to 0 after 5), then crawler_segment_update. */
void crawler_segment_animate(void)
{
	OBJECT(current_object)->crawler.frame_timer++;
	if (OBJECT(current_object)->crawler.frame_timer > 3)
	{
		OBJECT(current_object)->crawler.frame_timer = 0;
		OBJECT(current_object)->sprite_frame = crawler_segment_frames[crawler_walk_steps[OBJECT(current_object)->crawler.variant].steps[OBJECT(current_object)->crawler.walk_step]];
		OBJECT(current_object)->crawler.walk_step++;
		if (OBJECT(current_object)->crawler.walk_step > 5)
		{
			OBJECT(current_object)->crawler.walk_step = 0;
			crawler_segment_update();
			return;
		}
	}
	crawler_segment_update();
}

/* A segment or the tail: moves, touchable, a rider, falls to its floor (removed off screen while the
   head is over a pit) and is drawn with its shadow, variant 1's tail sorted 8 lower. Over a pit, while
   the head is not, it stops falling and is not drawn; with on_platform it sorts at its platform's
   draw_depth - 0xF. Then crawler_step. */
void crawler_segment_update(void)
{
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, is_over_pit = 0;

	move_object_projected(&deleted, &ignored, &ignored_2, &ignored_3);
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->crawler_head.over_pit != 0u)
	{
		delete_if_off_screen(&deleted, &ignored, &ignored_2);
		if ((uint8_t)deleted == 1u)
			return;
	}
	add_to_touchable_list(deleted);
	add_to_rider_list();
	fall_to_floor(&ignored_4, &is_over_pit, &ignored);
	if ((uint8_t)is_over_pit == 0u)
	{
		if (OBJECT(current_object)->crawler.variant == 0u || (uint8_t)OBJECT(current_object)->crawler.walk_step != 0xFFu)
			queue_sprite(is_over_pit);
		else
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
			queue_sprite_at_depth(&ignored_4, &depth);
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
		queue_sprite_at_depth(&ignored_4, &depth);
		current_object = object_cursor;
		crawler_step();
		return;
	}
	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->crawler_head.over_pit == 0u)
	{
		OBJECT(current_object)->speed_z = 0;
		crawler_step();
		return;
	}
	if (OBJECT(current_object)->crawler.on_platform != 0u)
	{
		depth = (uint16_t)(OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->platform))->draw_depth - 0xFu);
		queue_sprite_at_depth(&ignored_4, &depth);
		crawler_step();
		return;
	}
	if (OBJECT(current_object)->crawler.variant == 0u || (uint8_t)OBJECT(current_object)->crawler.walk_step != 0xFFu)
		queue_sprite(is_over_pit);
	else
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
		queue_sprite_at_depth(&ignored_4, &depth);
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored_4, &depth);
	current_object = object_cursor;
	crawler_step();
}

/* Once the head has crawler_head.triggered set: every 5 frames (step_timer) flips moving; while moving
   the piece goes at speed 0x200 along the direction in spawn_arg_2, otherwise it stands still. */
void crawler_step(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t step_timer = 0;

	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->crawler_head.triggered == 0u)
		return;
	step_timer = OBJECT(current_object)->crawler.step_timer;
	OBJECT(current_object)->crawler.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		if (OBJECT(current_object)->crawler.moving != 0u)
		{
			direction = OBJECT(current_object)->spawn_arg_2;
			speed = 0x200u;
			direction_to_speed(speed, &direction, &speed_x, &speed_y);
			OBJECT(current_object)->speed_x = speed_x;
			OBJECT(current_object)->speed_y = speed_y;
		}
		return;
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	if (OBJECT(current_object)->crawler.moving == 0u)
	{
		OBJECT(current_object)->crawler.moving = 1u;
		OBJECT(current_object)->crawler.step_timer = 4u;
		if (OBJECT(current_object)->crawler.moving != 0u)
		{
			direction = OBJECT(current_object)->spawn_arg_2;
			speed = 0x200u;
			direction_to_speed(speed, &direction, &speed_x, &speed_y);
			OBJECT(current_object)->speed_x = speed_x;
			OBJECT(current_object)->speed_y = speed_y;
		}
		return;
	}
	OBJECT(current_object)->crawler.moving = 0u;
	OBJECT(current_object)->crawler.step_timer = 4u;
	if (OBJECT(current_object)->crawler.moving != 0u)
	{
		direction = OBJECT(current_object)->spawn_arg_2;
		speed = 0x200u;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
	}
}

/* Object type 4's handler (5 blocks): does nothing. No stage list uses type 4. */
void object_type_4_idle(void)
{
}

/* Gives the current chaser its back zone: a new object of kind 9 (flags 0x40000002) whose end_position
   and handler come from object_steps record zone_index, parent the chaser. Hands back the zone (zone
   unchanged when no object is free). */
uint32_t spawn_chaser_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0, step_record = 0;

	chaser_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		step_record = object_steps_ADDRESS + chaser_zone_index * 3u * 2u;
		OBJECT(zone)->chaser_back_zone.end_position = BACK_ZONE_RECORD(step_record)->limit;
		OBJECT(zone)->handler = BACK_ZONE_RECORD(step_record)->handler;
		OBJECT(zone)->parent = current_object;
	}
	return zone;
}

/* The back zone of a stage 8 boulder: kind 0x14; removed once its parent's burning_boulder.state is 3
   (the boulder falling away), else chaser_back_zone_update. */
void chaser_back_zone_check_parent(void)
{
	OBJECT(current_object)->kind = 0x14u;
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->burning_boulder.state == 3u)
	{
		delete_current_object();
		return;
	}
	chaser_back_zone_update();
}

/* Removed once scroll_position reaches end_position; until then it takes its parent's box, position,
   height and direction, turns the box into one 0x200 long right behind the parent (against its
   direction, direction >> 8) and joins the zone list. */
void chaser_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	if (OBJECT(current_object)->chaser_back_zone.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		invalid_jump(0x82F30u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

/* spiked_roller_start's back zone: a new object of kind 0x18 (flags 0x40000002, handler
   roller_back_zone_update) whose limit is the word of object_steps record zone_index, parent the leader
   (object_cursor). */
void spawn_roller_back_zone(uint32_t zone_index, uint32_t zone)
{
	uint32_t free_count = 0;

	roller_zone_index = zone_index;
	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->kind = 0x18u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->roller_back_zone.limit = object_steps[roller_zone_index * 3u];
		OBJECT(zone)->handler = roller_back_zone_update_ADDRESS;
		OBJECT(zone)->parent = object_cursor;
	}
}

/* Takes the leader's box, position, height and direction and turns the box into one 0x200 deep behind it
   and 0x1A2 across, in the zone list; removed once the leader reaches roller_back_zone.limit along its
   direction (direction >> 8). */
void roller_back_zone_update(void)
{
	uint32_t direction = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->y_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		direction = add_to_zone_list(direction);
		return;
	case 1:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->x_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->y_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		direction = add_to_zone_list(direction);
		return;
	case 2:
		if (OBJECT(current_object)->roller_back_zone.limit <= OBJECT(current_object)->y_int)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_offset_y;
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= OBJECT(current_object)->hitbox_size_y;
		OBJECT(current_object)->x_int -= 0xB0u;
		OBJECT(current_object)->hitbox_size_x = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		direction = add_to_zone_list(direction);
		return;
	case 3:
		if (OBJECT(current_object)->x_int <= OBJECT(current_object)->roller_back_zone.limit)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x;
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		direction = add_to_zone_list(direction);
		return;
	default:
		invalid_jump(0x8303Bu + (direction >> 8) * 3u);
	}
}

/* Stage list object (stages 3, 4 and 8): keeps pacer_end_positions[sprite_frame] in speed_x_fraction as
   its end, then player_pacer_update now and every frame after. */
void player_pacer_start(void)
{
	OBJECT(current_object)->speed_x_fraction = pacer_end_positions[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->handler = player_pacer_update_ADDRESS;
	player_pacer_update();
}

/* Every frame clears the three players' speed_scale; until scroll_position reaches its end
   (speed_x_fraction, then removed) it ranks the players in play along get_pacing_axis: with two, the one
   ahead gets speed_scale 0xD; with three, the first 0xD and the second 0xE. */
void player_pacer_update(void)
{
	uint32_t axis_field = 0, decreasing = 0, last_player = 0, leader_object = 0, middle_player = 0, other_object = 0, player = 0, player_1 = 0, player_2 = 0, player_3 = 0, players_left = 0, playing_count = 0, status_cursor = 0;
	uint8_t status = 0;
	uint32_t swap = 0, swap_2 = 0, swap_3 = 0, swap_4 = 0;

	if (OBJECT(current_object)->speed_x_fraction <= scroll_position)
	{
		player_1_speed_scale = 0u;
		player_2_speed_scale = 0u;
		player_3_speed_scale = 0u;
		delete_current_object();
		return;
	}
	player_1_speed_scale = 0u;
	player_2_speed_scale = 0u;
	player_3_speed_scale = 0u;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	playing_count = 0u;
	do
	{
		status = mem_read8(status_cursor);
		status_cursor++;
		if (status == 0xFFu)
			playing_count = (uint8_t)(playing_count + 1u);
		players_left--;
	} while (players_left != 0u);
	switch (playing_count)
	{
	case 0:
	case 1:
		return;
	case 2:
	case 4:
		player = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		player_3 = PLAYER_BLOCK(3);
		if (player_status[0] == 0xFFu)
		{
			if (player_2_status != 0xFFu)
				player_2 = player_3;
		}
		else
		{
			player = player_2;
			player_2 = player_3;
		}
		get_pacing_axis(player, &axis_field, &decreasing);
		if ((int16_t)mem_read16(LINKED(current_object, PLAYER(player)->object) + axis_field * 2u) < (int16_t)mem_read16(LINKED(current_object, PLAYER(player_2)->object) + axis_field * 2u))
		{
			swap = player;
			player = player_2;
			player_2 = swap;
		}
		if ((uint16_t)decreasing != 0u)
		{
			PLAYER(player_2)->speed_scale = 0xDu;
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		return;
	case 3:
		player_1 = PLAYER_BLOCK(1);
		get_pacing_axis(player_1, &axis_field, &decreasing);
		player = PLAYER_BLOCK(1);
		middle_player = PLAYER_BLOCK(2);
		last_player = PLAYER_BLOCK(3);
		leader_object = LINKED(current_object, player_1_object);
		other_object = LINKED(current_object, player_2_object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_2 = player;
			player = middle_player;
			middle_player = swap_2;
			leader_object = LINKED(leader_object, PLAYER(player)->object);
		}
		other_object = LINKED(other_object, PLAYER(last_player)->object);
		if ((int16_t)mem_read16(leader_object + axis_field * 2u) < (int16_t)mem_read16(other_object + axis_field * 2u))
		{
			swap_3 = player;
			player = last_player;
			last_player = swap_3;
		}
		if ((int16_t)mem_read16(LINKED(leader_object, PLAYER(middle_player)->object) + axis_field * 2u) < (int16_t)mem_read16(LINKED(other_object, PLAYER(last_player)->object) + axis_field * 2u))
		{
			swap_4 = middle_player;
			middle_player = last_player;
			last_player = swap_4;
		}
		if ((uint16_t)decreasing != 0u)
		{
			PLAYER(last_player)->speed_scale = 0xDu;
			PLAYER(middle_player)->speed_scale = 0xEu;
			return;
		}
		PLAYER(player)->speed_scale = 0xDu;
		PLAYER(middle_player)->speed_scale = 0xEu;
		return;
	default:
		invalid_jump(0x84428u + playing_count * 2u);
	}
}

/* From the travel direction of the floor block under the object of player (a player block): hands back
   in axis_field the word index of the object coordinate it runs along (0x29 y_int, 0x27 x_int) and in
   decreasing 0xFFFF when it runs toward smaller values (0 and 4 along y, 3 along x), else 0. */
void get_pacing_axis(uint32_t player, uint32_t *p_axis_field, uint32_t *p_decreasing)
{
	uint16_t decreasing = 0;
	uint32_t axis_field = 0;

	switch (mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u))
	{
	case 0:
	case 4:
		axis_field = 0x29u;
		decreasing = 0xFFFFu;
		break;
	case 1:
		axis_field = 0x27u;
		decreasing = 0u;
		break;
	case 2:
		axis_field = 0x29u;
		decreasing = 0u;
		break;
	case 3:
		axis_field = 0x27u;
		decreasing = 0xFFFFu;
		break;
	default:
		invalid_jump(0x8451Du + mem_read8(floor_blocks + 2u + OBJECT(LINKED(current_object, PLAYER(player)->object))->floor_block + 0x15u) * 2u);
	}
	*p_axis_field = axis_field;
	*p_decreasing = (*p_decreasing & 0xFFFF0000u) | decreasing;
}

/* Stage list object made a sign: its z from record sprite_frame of object_kinds, the rest of the record
   set up on itself by init_sign_from_kind. */
void sign_start(void)
{
	uint32_t ignored = 0, kind_record = 0, sign = 0;

	kind_record = OBJECT(current_object)->sprite_frame * 0xCu + object_kinds_ADDRESS;
	OBJECT(current_object)->z_int = SIGN_KIND(kind_record)->z;
	OBJECT(current_object)->z_fraction = 0u;
	sign = current_object;
	kind_record += 2u;
	init_sign_from_kind(kind_record, &ignored, &sign);
}

/* Takes a free object (allocate_object_outside_loop) and makes it sign kind of object_kinds at x, y, z
   (place_sign); hands back the sign and its depth. Nothing when no object is free. */
void spawn_sign(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, p_sign);
	if (free_count == 0u)
	{
		return;
	}
	place_sign(x, y, kind, z, p_depth, p_sign);
}

/* spawn_sign with allocate_object; hands back the sign. */
uint32_t spawn_sign_2(uint16_t x, uint16_t y, uint16_t kind, uint16_t z, uint32_t sign)
{
	uint32_t free_count = 0, ignored = 0;

	allocate_object(&free_count, &sign);
	if (free_count == 0u)
		return sign;
	place_sign(x, y, kind, z, &ignored, &sign);
	return sign;
}

/* Puts the object sign at x, y, z and sets it up from record kind of object_kinds, without the record's
   own z (init_sign_from_kind); hands back sign and its depth. */
void place_sign(uint16_t x, uint16_t y, uint32_t kind, uint16_t z, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;

	OBJECT(sign)->x_int = x;
	OBJECT(sign)->x_fraction = 0u;
	OBJECT(sign)->y_int = y;
	OBJECT(sign)->y_fraction = 0u;
	OBJECT(sign)->z_int = z;
	OBJECT(sign)->z_fraction = 0u;
	kind = (uint16_t)kind * 0xCu + 0x84FB7u;
	init_sign_from_kind(kind, p_depth, &sign);
	*p_sign = sign;
}

/* Sets up the object sign from the object_kinds record at record (just past its z): sprite_frame, how
   long it shows (speed_x_fraction), its palette cycle (an offset into sign_palette_lists, a step every 4
   frames) and handler; sprite_flags 0x1000, parent the current object. Then projects it and hands back
   its depth. */
void init_sign_from_kind(uint32_t record, uint32_t *p_depth, uint32_t *p_sign)
{
	uint32_t sign = *p_sign;
	uint32_t ignored = 0;
	uint32_t saved_current_object = 0;

	OBJECT(sign)->sprite_frame = SIGN_KIND(record - 0x2u)->sprite_frame;
	record += 2u;
	OBJECT(sign)->speed_x_fraction = SIGN_KIND(record - 0x4u)->duration;
	record += 2u;
	OBJECT(sign)->animate_sign.palette_list = SIGN_KIND(record - 0x6u)->palette_list_offset;
	OBJECT(sign)->animate_sign.palette_list += sign_palette_lists_ADDRESS;
	OBJECT(sign)->handler = SIGN_KIND(record - 0x6u)->handler;
	OBJECT(sign)->sprite_flags = 0x1000u;
	OBJECT(sign)->flags = 0u;
	OBJECT(sign)->sprite_offset_x = 0;
	OBJECT(sign)->sprite_offset_y = 0;
	OBJECT(sign)->animate_sign.palette_index = 0u;
	OBJECT(sign)->animate_sign.palette_delay = 4u;
	OBJECT(sign)->animate_sign.palette_timer = 0u;
	OBJECT(sign)->parent = current_object;
	saved_current_object = current_object;
	current_object = sign;
	project_object(&ignored, p_depth);
	sign = current_object;
	current_object = saved_current_object;
	*p_sign = sign;
}

/* sign_show, except in game mode 0x20 (YOU FAILED), where the sign is removed. */
void sign_show_unless_mode_32(void)
{
	if (current_game_mode != 0x20u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

/* sign_show while the parent's player is stuck or trapped (state 0x1D, 0x2C or 0x32); removed once it is
   not. */
void sign_show_while_player_busy(void)
{
	uint32_t player = 0, player_2 = 0;

	player = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player;
	player_2 = PLAYER_BLOCK(1) + (uint16_t)(player << 9);
	if (PLAYER(player_2)->state == 0x1Du || PLAYER(player_2)->state == 0x2Cu || PLAYER(player_2)->state == 0x32u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

/* The "BREAK!!" sign: sign_show until goal_unlocked is set, then removed. */
void sign_show_until_goal_unlocked(void)
{
	if (goal_unlocked == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

/* sign_show while the parent's speed_z_int is 0; removed once it is not. */
void sign_show_while_parent_still(void)
{
	if ((uint16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->speed_z_int == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

/* sign_show until sign_stop_flag is set, then removed (only the unused exit gate sets the flag). */
void sign_show_until_gate_closed(void)
{
	if (sign_stop_flag == 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

/* The arrow over "BREAK!!" in stage 5: sign_show_unless_stage_0 until cage_bar_hit is set, then removed. */
void sign_show_until_cage_bar_hit(void)
{
	if (cage_bar_hit == 0u)
	{
		sign_show_unless_stage_0();
		return;
	}
	delete_current_object();
}

/* sign_show, except in stage 0, where the sign is removed. */
void sign_show_unless_stage_0(void)
{
	if (stage != 0u)
	{
		sign_show();
		return;
	}
	delete_current_object();
}

/* Each frame: update_screen_position, animate_sign, counts speed_x_fraction down and is drawn on top
   (bucket 0); removed when it runs out. */
void sign_show(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t speed_x_fraction = 0;

	update_screen_position(&ignored, &ignored_2);
	animate_sign();
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

/* animate_sign, then drawn on top 0x38 above the floor point under the parent, at the parent's screen x,
   while the parent's flags have bit 0x40; removed once it is clear. */
void sign_follow_parent(void)
{
	uint8_t bucket = 0;
	uint32_t object = 0;

	animate_sign();
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y - 0x38u;
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

/* animate_sign, then drawn on top 0x28 above the floor point under the parent, at the parent's screen x,
   while the parent's player is in state 0x22 (in a zone); removed once it is not. */
void sign_follow_player(void)
{
	uint8_t bucket = 0;
	uint32_t object = 0, player = 0;

	animate_sign();
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y - 0x28u;
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state == 0x22u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

/* Unused: a stage list object made a screen sign: its x_int/y_int become the screen position and
   sprite_frame the kind, then place_screen_sign on itself. */
void screen_sign_start(void)
{
	uint16_t sprite_frame = 0, x_int = 0, y_int = 0;
	uint32_t sign = 0;

	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	sign = current_object;
	place_screen_sign(x_int, y_int, sprite_frame, sign);
}

/* Unused: takes a free object (allocate_object_outside_loop) and makes it a screen sign of kind at
   screen_x, screen_y (place_screen_sign); nothing when none is free. */
void spawn_screen_sign(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint32_t sign)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &sign);
	if (free_count == 0u)
		return;
	place_screen_sign(screen_x, screen_y, kind, sign);
}

/* Unused: spawn_screen_sign with allocate_object. */
void spawn_screen_sign_2(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint32_t sign)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &sign);
	if (free_count == 0u)
		return;
	place_screen_sign(screen_x, screen_y, kind, sign);
}

/* Unused: sets up sign at screen_x, screen_y from screen_sign_kinds[kind]: sprite, how long it shows
   (speed_x_fraction) and palette cycle from sign_palette_lists (a step every 4 frames); sprite_flags
   0x1000, handler screen_sign_show. It is never projected: the position stays a screen position. */
void place_screen_sign(uint16_t screen_x, uint16_t screen_y, uint32_t kind, uint32_t sign)
{
	OBJECT(sign)->screen_x = screen_x;
	OBJECT(sign)->screen_y = screen_y;
	kind = (uint16_t)kind * 6u + screen_sign_kinds_ADDRESS;
	OBJECT(sign)->sprite_frame = SCREEN_SIGN_KIND(kind)->sprite_frame;
	kind += 2u;
	OBJECT(sign)->speed_x_fraction = SCREEN_SIGN_KIND(kind - 0x2u)->duration;
	OBJECT(sign)->screen_sign.palette_list = SCREEN_SIGN_KIND(kind - 0x2u)->palette_list_offset;
	OBJECT(sign)->screen_sign.palette_list += sign_palette_lists_ADDRESS;
	OBJECT(sign)->sprite_flags = 0x1000u;
	OBJECT(sign)->flags = 0u;
	OBJECT(sign)->handler = screen_sign_show_ADDRESS;
	OBJECT(sign)->screen_sign.palette_index = 0u;
	OBJECT(sign)->screen_sign.palette_delay = 4u;
	OBJECT(sign)->screen_sign.palette_timer = 0u;
}

/* Unused: sign_show without update_screen_position: animate_sign, counts speed_x_fraction down and is
   drawn on top (bucket 0); removed when it runs out. */
void screen_sign_show(void)
{
	uint8_t bucket = 0;
	uint16_t speed_x_fraction = 0;

	animate_sign();
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

/* Unused: a stage list object made a sign on the floor: sprite floor_sign_sprites[sprite_frame], palette
   0x8450, sprite_flags 0x1004, z at its floor (find_floor); projected, then floor_sign_update. */
void floor_sign_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x8450u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = floor_sign_update_ADDRESS;
	OBJECT(current_object)->sprite_frame = floor_sign_sprites[OBJECT(current_object)->sprite_frame];
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Unused: drawn (update_screen_position, queue_sprite) until check_left_screen says it has left the
   screen, then removed. */
void floor_sign_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	delete_current_object();
}

/* Stage list object, one of the stages' static scenery sprites: loads scenery_sprite_kinds[sprite_frame]
   (sprite flags, sprite, palette, depth_offset, handler) and stands on its floor (kind 0xD at z_int 0x8D
   instead); projected. */
void scenery_sprite_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, kind_record = 0, sprite_kind = 0;

	sprite_kind = OBJECT(current_object)->sprite_frame;
	scenery_sprite_kind = sprite_kind;
	kind_record = scenery_sprite_kinds[sprite_kind] + scenery_sprite_kinds_ADDRESS;
	OBJECT(current_object)->sprite_flags = SCENERY_KIND_RECORD(kind_record)->sprite_flags;
	kind_record += 2u;
	OBJECT(current_object)->sprite_frame = SCENERY_KIND_RECORD(kind_record - 0x2u)->sprite_frame;
	kind_record += 2u;
	OBJECT(current_object)->sprite_palette = SCENERY_KIND_RECORD(kind_record - 0x4u)->sprite_palette;
	kind_record += 2u;
	OBJECT(current_object)->scenery_sprite.depth_offset = SCENERY_KIND_RECORD(kind_record - 0x6u)->depth_offset;
	OBJECT(current_object)->handler = SCENERY_KIND_RECORD(kind_record - 0x6u)->handler;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->scenery_sprite.unused_stage = stage;
	if (scenery_sprite_kind != 0xDu)
	{
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	else
		OBJECT(current_object)->z = 0x8D0000;
	project_object(&ignored, &ignored_2);
}

/* Removed once game_mode_timer reaches 0x3C in game mode 0x20 (YOU FAILED); until then
   scenery_sprite_draw. */
void scenery_sprite_draw_until_you_failed(void)
{
	if (current_game_mode != 0x20u || game_mode_timer < 0x3C)
	{
		scenery_sprite_draw();
		return;
	}
	delete_current_object();
}

/* Drawn at depth screen_y + depth_offset until map_screen_active is set (the map screen), then removed. */
void scenery_sprite_draw(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0;

	if (map_screen_active == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Drawn at depth screen_y + depth_offset until check_left_screen says it has left the screen, then
   removed. */
void scenery_sprite_draw_until_offscreen(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* scenery_sprite_draw_until_offscreen with a margin of 0x50 (check_left_screen_margin). */
void scenery_sprite_draw_wide_margin(void)
{
	uint16_t margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	margin = 0x50u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* The handler of scenery kind 0xA (stage 0xF): its palette cycles through scenery_sprite_palettes every
   6 frames (animate_sign), the sprite 0x10 higher; then scenery_sprite_cycle_palette from the next
   frame. */
void scenery_sprite_cycle_palette_start(void)
{
	OBJECT(current_object)->scenery_sprite.palette_index = 0u;
	OBJECT(current_object)->scenery_sprite.palette_delay = 6u;
	OBJECT(current_object)->scenery_sprite.palette_timer = 0u;
	OBJECT(current_object)->scenery_sprite.palette_list = scenery_sprite_palettes_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x10;
	OBJECT(current_object)->handler = scenery_sprite_cycle_palette_ADDRESS;
}

/* Until map_screen_active is set (then removed): animate_sign, then drawn at depth screen_y +
   depth_offset like scenery_sprite_draw. */
void scenery_sprite_cycle_palette(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0;

	if (map_screen_active == 0u)
	{
		animate_sign();
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->scenery_sprite.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Stage list object: loads group_leader_hold_records[sprite_frame] (leader number, direction,
   coordinate, end scroll position, which_flag) and sets group_leader_0_held, or group_leaders_1_3_held
   when which_flag is set, so the leaders spawned meanwhile wait; then group_leader_hold_wait. */
void group_leader_hold_start(void)
{
	uint32_t held_flag = 0, hold_record = 0;

	hold_record = group_leader_hold_records[OBJECT(current_object)->sprite_frame] + group_leader_hold_records_ADDRESS;
	OBJECT(current_object)->group_leader_hold.leader = GROUP_LEADER_HOLD_RECORD(hold_record)->leader;
	hold_record++;
	OBJECT(current_object)->direction = GROUP_LEADER_HOLD_RECORD(hold_record - 0x1u)->direction;
	hold_record++;
	OBJECT(current_object)->group_leader_hold.coordinate = GROUP_LEADER_HOLD_RECORD(hold_record - 0x2u)->coordinate;
	hold_record += 2u;
	OBJECT(current_object)->group_leader_hold.end_position = GROUP_LEADER_HOLD_RECORD(hold_record - 0x4u)->end_position;
	OBJECT(current_object)->group_leader_hold.which_flag = GROUP_LEADER_HOLD_RECORD(hold_record - 0x4u)->which_flag;
	held_flag = group_leader_0_held_ADDRESS;
	if ((uint8_t)OBJECT(current_object)->group_leader_hold.which_flag != 0u)
		held_flag = group_leaders_1_3_held_ADDRESS;
	mem_write8(held_flag, 0xFFu);
	OBJECT(current_object)->handler = group_leader_hold_wait_ADDRESS;
}

/* Clears its held flag and is removed once scroll_position reaches end_position, or once
   group_leaders[leader] reaches coordinate along direction (0 and 4: y down to it, 1: x up, 2: y up, 3:
   x down). */
void group_leader_hold_wait(void)
{
	uint32_t held_flag = 0, object = 0;

	if (OBJECT(current_object)->group_leader_hold.end_position <= scroll_position)
	{
		held_flag = group_leader_0_held_ADDRESS;
		if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
			held_flag = group_leaders_1_3_held_ADDRESS;
		mem_write8(held_flag, 0u);
		delete_current_object();
		return;
	}
	object = (current_object & ~0xFFFFu) | group_leaders[OBJECT(current_object)->group_leader_hold.leader];
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->group_leader_hold.coordinate < OBJECT(object)->y_int)
			return;
		held_flag = group_leader_0_held_ADDRESS;
		if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
			held_flag = group_leaders_1_3_held_ADDRESS;
		mem_write8(held_flag, 0u);
		delete_current_object();
		return;
	case 1:
		if (OBJECT(object)->x_int < OBJECT(current_object)->group_leader_hold.coordinate)
			return;
		held_flag = group_leader_0_held_ADDRESS;
		if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
			held_flag = group_leaders_1_3_held_ADDRESS;
		mem_write8(held_flag, 0u);
		delete_current_object();
		return;
	case 2:
		if (OBJECT(object)->y_int < OBJECT(current_object)->group_leader_hold.coordinate)
			return;
		held_flag = group_leader_0_held_ADDRESS;
		if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
			held_flag = group_leaders_1_3_held_ADDRESS;
		mem_write8(held_flag, 0u);
		delete_current_object();
		return;
	case 3:
		if (OBJECT(current_object)->group_leader_hold.coordinate < OBJECT(object)->x_int)
			return;
		held_flag = group_leader_0_held_ADDRESS;
		if (OBJECT(current_object)->group_leader_hold.which_flag != 0u)
			held_flag = group_leaders_1_3_held_ADDRESS;
		mem_write8(held_flag, 0u);
		delete_current_object();
		return;
	default:
		invalid_jump(0x85BE6u + OBJECT(current_object)->direction * 2u);
	}
}

/* Once scroll_position reaches the stage's next wall_spawns record (wall_spawn_cursor; records 0x100 or
   more behind are skipped) spawns that wall: position, end_position, kind, part count, handler
   wall_update, parts from build_wall_parts and its far corner's screen offset in speed_z. Short of pool
   2 blocks it tries again next call. */
void spawn_walls(uint32_t *p_spawns_offset, uint32_t *p_part_size, uint32_t *p_second_sprite, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_wall)
{
	uint32_t second_sprite = *p_second_sprite;
	uint32_t point_y = *p_point_y;
	uint32_t wall = *p_wall;
	uint16_t blocks_needed = 0, past_spawn = 0;
	uint32_t corner_x = 0, free_count = 0, ignored = 0, ignored_2 = 0, kind_record = 0, object = 0, record = 0;
	uint16_t spawn_position = 0, current_scroll = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x20u;
	if ((init_flags & 0x20u) == 0u)
	{
		*p_spawns_offset = wall_spawns[stage];
		wall_spawn_cursor = wall_spawns_ADDRESS + *p_spawns_offset;
	}
	spawn_position = WALL_SPAWN(wall_spawn_cursor)->position;
	current_scroll = scroll_position;
	past_spawn = (uint16_t)(current_scroll - spawn_position);
	if (current_scroll < spawn_position)
	{
		return;
	}
	if ((int16_t)past_spawn >= 0x100)
	{
		wall_spawn_cursor += 0xEu;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | (second_sprite & 0xFFFFu);
		*p_point_y = point_y;
		*p_wall = wall;
		return;
	}
	allocate_object_outside_loop(&free_count, &wall);
	if (free_count == 0u)
	{
		*p_wall = wall;
		return;
	}
	record = wall_spawn_cursor + 2u;
	OBJECT(wall)->wall_update.end_position = WALL_SPAWN(record - 0x2u)->end_position;
	record += 2u;
	OBJECT(wall)->x_int = WALL_SPAWN(record - 0x4u)->x;
	OBJECT(wall)->x_fraction = 0u;
	record += 2u;
	OBJECT(wall)->y_int = WALL_SPAWN(record - 0x6u)->y;
	OBJECT(wall)->y_fraction = 0u;
	record += 2u;
	OBJECT(wall)->z_int = WALL_SPAWN(record - 0x8u)->z;
	OBJECT(wall)->z_fraction = 0u;
	OBJECT(wall)->floor_z = OBJECT(wall)->z;
	record += 2u;
	OBJECT(wall)->sprite_frame = WALL_SPAWN(record - 0xAu)->kind;
	record += 2u;
	OBJECT(wall)->speed_x_fraction = WALL_SPAWN(record - 0xCu)->part_count;
	OBJECT(wall)->wall_update.depth_from_next = WALL_SPAWN(record - 0xCu)->depth_from_next;
	OBJECT(wall)->handler = wall_update_ADDRESS;
	OBJECT(wall)->sprite_flags = 0x1004u;
	OBJECT(wall)->hitbox_height = 0x100u;
	OBJECT(wall)->flags = 0u;
	OBJECT(wall)->wall_update.edge_flags = 0u;
	current_object = wall;
	blocks_needed = OBJECT(current_object)->speed_x_fraction;
	if (WALL_KIND_PARTS(wall_kinds_ADDRESS + 6u + OBJECT(current_object)->sprite_frame)->second_sprite >= 0)
		blocks_needed = (uint16_t)(blocks_needed * 2u);
	if (free_object_stack_2[0] <= (int16_t)blocks_needed)
	{
		object = current_object;
		delete_object_outside_loop(object, p_spawns_offset, p_part_size);
		*p_wall = wall;
		return;
	}
	build_wall_parts(p_part_size, &second_sprite, &wall);
	kind_record = OBJECT(current_object)->sprite_frame + wall_kinds_ADDRESS;
	OBJECT(current_object)->hitbox_offset_x = WALL_KIND_OFFSETS(kind_record)->hitbox_offset_x;
	kind_record++;
	OBJECT(current_object)->hitbox_offset_y = WALL_KIND_OFFSETS(kind_record - 0x1u)->hitbox_offset_y;
	kind_record++;
	OBJECT(current_object)->sprite_offset_x = WALL_KIND_OFFSETS(kind_record - 0x2u)->sprite_offset_x;
	kind_record++;
	OBJECT(current_object)->sprite_offset_y = WALL_KIND_OFFSETS(kind_record - 0x3u)->sprite_offset_y;
	OBJECT(current_object)->direction = WALL_KIND_OFFSETS(kind_record - 0x3u)->direction;
	project_object(&ignored, &ignored_2);
	wall_corner_x = OBJECT(current_object)->x + ((uint32_t)OBJECT(current_object)->hitbox_size_x << 16);
	wall_corner_y = OBJECT(current_object)->y + ((uint32_t)OBJECT(current_object)->hitbox_size_y << 16);
	*p_z = OBJECT(current_object)->z;
	corner_x = wall_corner_x;
	point_y = wall_corner_y;
	project_point(*p_z, p_spawns_offset, &corner_x, &point_y);
	OBJECT(current_object)->speed_z_fraction = corner_x - OBJECT(current_object)->screen_x;
	point_y = (point_y & ~0xFFFFu) | (uint16_t)(point_y - OBJECT(current_object)->screen_y);
	OBJECT(current_object)->speed_z_int = point_y;
	wall_spawn_cursor += 0xEu;
	*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | (second_sprite & 0xFFFFu);
	*p_point_y = point_y;
	*p_wall = wall;
}

/* spawn_walls' handler: until scroll_position reaches end_position (or, with end_position 0, until
   check_wall_left_screen says both ends have gone) wall_update_list_and_draw; then, or as soon as
   map_screen_active is set, frees its pool 2 parts and is removed. */
void wall_update(void)
{
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, off_screen = 0;

	update_screen_position(&ignored, &ignored_2);
	if (map_screen_active == 0u)
	{
		if (OBJECT(current_object)->wall_update.end_position == 0u)
		{
			off_screen = check_wall_left_screen();
			if (off_screen == 0u)
			{
				wall_update_list_and_draw();
				return;
			}
		}
		else if (scroll_position < OBJECT(current_object)->wall_update.end_position)
		{
			wall_update_list_and_draw();
			return;
		}
	}
	while ((int16_t)OBJECT(current_object)->child >= 0)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
	}
	delete_current_object();
}

/* Joins the wall list and, unless invisible, draws its parts as a strip from its screen position, each a
   step on from the last (those off screen skipped): each sorted at its own screen y + 0x10, or at the
   next part's - 0x10 with depth_from_next, and with a second sprite its second part under everything
   (bucket 0xFF). */
void wall_update_list_and_draw(void)
{
	uint8_t bucket = 0;
	uint16_t depth_bias = 0, part_y = 0;
	uint32_t depth_y = 0, ignored = 0;

	add_to_wall_list();
	if (OBJECT(current_object)->wall_update.invisible != 0u)
		return;
	r14 = (uint16_t)OBJECT(current_object)->screen_x;
	part_y = (uint16_t)OBJECT(current_object)->screen_y;
	wall_sprite_offset_x = (uint8_t)OBJECT(current_object)->sprite_offset_x;
	depth_bias = 0x10u;
	if (OBJECT(current_object)->wall_update.depth_from_next == 0u)
		depth_bias = (uint16_t)(0u - depth_bias);
	current_player = current_object;
	for (;;)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((int16_t)current_object < 0)
			break;
		OBJECT(current_object)->screen_x = r14;
		r14 = (uint16_t)(r14 + OBJECT(current_object)->hitbox_centre_x);
		OBJECT(current_object)->screen_y = part_y;
		depth_y = part_y;
		part_y += OBJECT(current_object)->hitbox_centre_y;
		if ((int16_t)(r14 - wall_sprite_offset_x) <= -0x20 || (int16_t)(OBJECT(current_object)->screen_x - wall_sprite_offset_x) > 0x160)
		{
			if (OBJECT(object_cursor)->wall_update.no_second_sprite != 0u)
				continue;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			continue;
		}
		if ((int16_t)depth_bias >= 0)
			depth_y = part_y;
		depth_y = (uint16_t)(depth_y - depth_bias);
		queue_sprite_at_depth(&ignored, &depth_y);
		if (OBJECT(object_cursor)->wall_update.no_second_sprite != 0u)
			continue;
		current_player = LINKED(current_player, OBJECT(current_object)->child);
		OBJECT(current_player)->screen_x = OBJECT(current_player)->hitbox_centre_x + OBJECT(current_object)->screen_x;
		OBJECT(current_player)->screen_y = OBJECT(current_player)->hitbox_centre_y + OBJECT(current_object)->screen_y;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)current_player;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
	}
}

/* Unused: does nothing; a second return right after wall_update_list_and_draw's own. */
void wall_update_spare_return(void)
{
}

/* Follows the wall's two ends in speed_y: bit 0 once its start (screen position less the sprite offsets)
   is on screen, bit 1 once it has left again, bits 2 and 3 the same for its far corner (at the screen
   offset spawn_walls keeps in speed_z). Hands back 0xFFFFFFFF once both ends have come and gone, else 0. */
uint32_t check_wall_left_screen(void)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(OBJECT(current_object)->screen_x - OBJECT(current_object)->sprite_offset_x + 0x10u) >= 0x160u || (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y + 0x20u) >= 0x120u)
	{
		if ((OBJECT(current_object)->speed_y & 1u) != 0u)
			OBJECT(current_object)->speed_y |= 2u;
	}
	else
		OBJECT(current_object)->speed_y |= 1u;
	if ((uint16_t)(OBJECT(current_object)->screen_x + OBJECT(current_object)->speed_z_fraction + 0x10u) < 0x160u && (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->speed_z_int + 0x20u) < 0x120u)
	{
		OBJECT(current_object)->speed_y |= 4u;
		if ((OBJECT(current_object)->speed_y & 2u) != 0u && (OBJECT(current_object)->speed_y & 8u) != 0u)
			off_screen = 0xFFFFFFFFu;
		return off_screen;
	}
	if ((OBJECT(current_object)->speed_y & 4u) == 0u)
	{
		if ((OBJECT(current_object)->speed_y & 2u) != 0u && (OBJECT(current_object)->speed_y & 8u) != 0u)
			off_screen = 0xFFFFFFFFu;
		return off_screen;
	}
	OBJECT(current_object)->speed_y |= 8u;
	if ((OBJECT(current_object)->speed_y & 2u) != 0u && (OBJECT(current_object)->speed_y & 8u) != 0u)
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

/* Unused: copies 12 characters into wall_editor_side_text: wall_editor_far_label ("Oku", far) when the
   wall's depth_from_next is set, else wall_editor_near_label ("Temae", near). */
void wall_editor_set_side_label(void)
{
	uint32_t label_text = 0;

	label_text = wall_editor_far_label_ADDRESS;
	if (OBJECT(current_object)->wall_editor.depth_from_next == 0u)
		label_text = wall_editor_near_label_ADDRESS;
	move_bytes(wall_editor_side_text_ADDRESS, label_text, 0xCu);
}

/* Unused: draws one more arrow (sprite 0x6388, palette 0x8230, sprite_flags 0x1004) in the next
   0x80-byte slot after wall_editor_arrow_count, up to 7 (nothing resets the count): at marker_x,
   marker_y on the floor there, on object_cursor's platform when it rides one, projected and drawn on
   top. */
void wall_editor_draw_arrow(uint32_t marker_x, uint32_t marker_y)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, marker = 0, marker_number = 0;
	uint32_t saved_current_object = 0;

	marker = wall_editor_arrow_count_ADDRESS;
	marker_number = (uint16_t)wall_editor_arrow_count;
	marker_number = (uint16_t)(marker_number + 1u);
	if ((int16_t)marker_number >= 8)
		return;
	wall_editor_arrow_count = marker_number;
	marker_number = (uint16_t)(marker_number - 1u);
	marker = marker + 2u + (uint16_t)(marker_number * 0x80u);
	OBJECT(marker)->sprite_flags = 0x1004u;
	OBJECT(marker)->sprite_frame = 0x6388u;
	OBJECT(marker)->sprite_palette = 0x8230u;
	OBJECT(marker)->sprite_offset_x = 0;
	OBJECT(marker)->sprite_offset_y = 0;
	OBJECT(marker)->flags = 0u;
	if ((OBJECT(object_cursor)->flags & 0x10u) != 0u || (OBJECT(object_cursor)->flags & 8u) != 0u)
	{
		OBJECT(marker)->flags |= 8u;
		OBJECT(marker)->platform = OBJECT(object_cursor)->platform;
	}
	OBJECT(marker)->x = marker_x;
	OBJECT(marker)->y = marker_y;
	saved_current_object = current_object;
	current_object = marker;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = saved_current_object;
}

/* Unused: wall_editor_draw_arrow with z = marker_z instead of the floor's. */
void wall_editor_draw_arrow_at_z(uint32_t marker_x, uint32_t marker_y, uint32_t marker_z)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, marker = 0, marker_number = 0;
	uint32_t saved_current_object = 0;

	marker = wall_editor_arrow_count_ADDRESS;
	marker_number = (uint16_t)wall_editor_arrow_count;
	marker_number = (uint16_t)(marker_number + 1u);
	if ((int16_t)marker_number >= 8)
		return;
	wall_editor_arrow_count = marker_number;
	marker_number = (uint16_t)(marker_number - 1u);
	marker = marker + 2u + (uint16_t)(marker_number * 0x80u);
	OBJECT(marker)->sprite_flags = 0x1004u;
	OBJECT(marker)->sprite_frame = 0x6388u;
	OBJECT(marker)->sprite_palette = 0x8230u;
	OBJECT(marker)->sprite_offset_x = 0;
	OBJECT(marker)->sprite_offset_y = 0;
	OBJECT(marker)->flags = 0u;
	if ((OBJECT(object_cursor)->flags & 0x10u) != 0u || (OBJECT(object_cursor)->flags & 8u) != 0u)
	{
		OBJECT(marker)->flags |= 8u;
		OBJECT(marker)->platform = OBJECT(object_cursor)->platform;
	}
	OBJECT(marker)->x = marker_x;
	OBJECT(marker)->y = marker_y;
	OBJECT(marker)->z = marker_z;
	saved_current_object = current_object;
	current_object = marker;
	project_object(&ignored, &ignored_2);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = saved_current_object;
}

/* Unused: prints the wall's kind (its wall_kinds offset in sprite_frame / 0x14) with draw_hex_2 at text
   column 12, row 18, after the wall editor menu's "Pattern No.". */
void wall_editor_draw_pattern_number(void)
{
	uint16_t attribute = 0;
	uint32_t text_position = 0, value = 0;

	value = (uint16_t)((int16_t)OBJECT(current_object)->sprite_frame / 0x14);
	attribute = 0u;
	text_position = TEXT_CELL(12, 18);
	value = draw_hex_2(value, attribute, text_position);
}

/* Unused: frees the wall's chain of pool 2 parts and builds it again with build_wall_parts. */
void wall_editor_rebuild_parts(uint32_t part)
{
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	current_player = current_object;
	while ((int16_t)OBJECT(current_player)->child >= 0)
	{
		child = OBJECT(current_player)->child;
		free_object_2(child);
		current_player = LINKED(current_player, OBJECT(current_player)->child);
	}
	build_wall_parts(&ignored, &ignored_2, &part);
}

/* Sets the wall's box and palette from its wall_kinds record (a negative size counts per part: times the
   part count in speed_x_fraction) and chains its parts from pool 2: per step one with the part sprite
   and x/y step, and a second (palette 0x9FFE) when the kind has a second sprite. A negative part sprite
   makes the wall invisible. */
void build_wall_parts(uint32_t *p_part_size, uint32_t *p_second_sprite, uint32_t *p_part)
{
	uint32_t part_size = *p_part_size;
	uint16_t second_sprite = (uint16_t)*p_second_sprite;
	uint32_t part = *p_part;
	uint8_t part_step_x = 0, part_step_y = 0, second_offset_x = 0;
	uint16_t part_sprite = 0;
	uint32_t free_count = 0, object = 0, parts_left = 0, record = 0;

	record = OBJECT(current_object)->sprite_frame + 0x90A78u;
	OBJECT(current_object)->hitbox_size_x = WALL_KIND_PARTS(record)->size_x;
	record += 2u;
	OBJECT(current_object)->hitbox_size_y = WALL_KIND_PARTS(record - 0x2u)->size_y;
	record += 2u;
	if ((int16_t)OBJECT(current_object)->hitbox_size_x >= 0)
	{
		if ((int16_t)OBJECT(current_object)->hitbox_size_y < 0)
		{
			part_size = (part_size & ~0xFFFFu) | OBJECT(current_object)->hitbox_size_y;
			part_size = (part_size & ~0xFFFFu) | (part_size & 0x7FFFu);
			OBJECT(current_object)->hitbox_size_y = OBJECT(current_object)->speed_x_fraction * part_size;
		}
	}
	else
	{
		part_size = (part_size & ~0xFFFFu) | OBJECT(current_object)->hitbox_size_x;
		part_size = (part_size & ~0xFFFFu) | (part_size & 0x7FFFu);
		OBJECT(current_object)->hitbox_size_x = OBJECT(current_object)->speed_x_fraction * part_size;
	}
	parts_left = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->sprite_palette = WALL_KIND_PARTS(record - 0x4u)->palette;
	record += 2u;
	part_step_x = WALL_KIND_PARTS(record - 0x6u)->part_step_x;
	record++;
	part_step_y = WALL_KIND_PARTS(record - 0x7u)->part_step_y;
	record++;
	part_sprite = (uint16_t)WALL_KIND_PARTS(record - 0x8u)->part_sprite;
	OBJECT(current_object)->build_wall_parts.invisible = 0u;
	record += 2u;
	if ((int16_t)part_sprite < 0)
	{
		OBJECT(current_object)->build_wall_parts.invisible = 0xFFu;
		part = current_object;
		OBJECT(part)->child = 0xFFFFu;
		OBJECT(current_object)->speed_y_int = part;
		*p_part_size = part_size;
		*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | second_sprite;
		*p_part = part;
		return;
	}
	second_offset_x = WALL_KIND_PARTS(record - 0xAu)->second_offset_x;
	record++;
	current_player = (current_player & ~0xFFu) | WALL_KIND_PARTS(record - 0xBu)->second_offset_y;
	second_sprite = (uint16_t)WALL_KIND_PARTS(record - 0xBu)->second_sprite;
	OBJECT(current_object)->build_wall_parts.no_second_sprite = 0xFFu;
	if ((int16_t)second_sprite >= 0)
		OBJECT(current_object)->build_wall_parts.no_second_sprite = 0u;
	object = current_object;
	do
	{
		allocate_object_2(&free_count, &part_size, &part);
		if (free_count == 0u)
			break;
		OBJECT(part)->sprite_frame = part_sprite;
		OBJECT(part)->hitbox_centre_x = part_step_x;
		OBJECT(part)->hitbox_centre_y = part_step_y;
		OBJECT(part)->sprite_flags = OBJECT(object)->sprite_flags;
		OBJECT(part)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(part)->parent = object;
		OBJECT(object)->child = part;
		object = part;
		if ((int16_t)second_sprite >= 0)
		{
			allocate_object_2(&free_count, &part_size, &part);
			if (free_count == 0u)
				break;
			OBJECT(part)->sprite_frame = second_sprite;
			OBJECT(part)->hitbox_centre_x = second_offset_x;
			OBJECT(part)->hitbox_centre_y = current_player;
			OBJECT(part)->sprite_flags = OBJECT(object)->sprite_flags;
			OBJECT(part)->sprite_palette = 0x9FFEu;
			OBJECT(part)->parent = object;
			OBJECT(object)->child = part;
			object = part;
		}
		parts_left--;
	} while (parts_left != 0u);
	OBJECT(part)->child = 0xFFFFu;
	OBJECT(current_object)->speed_y_int = part;
	*p_part_size = part_size;
	*p_second_sprite = (*p_second_sprite & 0xFFFF0000u) | second_sprite;
	*p_part = part;
}
