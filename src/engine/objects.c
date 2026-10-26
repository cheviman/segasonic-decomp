/**
 *    Name:        engine/objects.c
 *    Description: the object system: the object list and its two pools, allocating, linking and deleting objects, spawning them from the stage lists (by type, by handler, in groups led by a leader), and what the stage objects share
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Unused: while next_stage_slot_request is set, moves the current object's x, y, z_int,
   previous_floor_z_int and previous_floor_block from this stage's link point to the next one's (-
   stage_link_from_*, + stage_link_to_*), the object side of continue_to_next_stage_slot. */
void move_object_to_next_stage_slot(void)
{
	if (next_stage_slot_request != 0u)
	{
		OBJECT(current_object)->x = (uint32_t)OBJECT(current_object)->x - stage_link_from_x + stage_link_to_x;
		OBJECT(current_object)->y = (uint32_t)OBJECT(current_object)->y - stage_link_from_y + stage_link_to_y;
		OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->previous_floor_z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->z_int = OBJECT(current_object)->z_int - stage_link_from_height + stage_link_to_height;
		OBJECT(current_object)->previous_floor_block = (uint16_t)(OBJECT(current_object)->previous_floor_block - stage_link_from_block + stage_link_to_block);
	}
}

/* Two steps through the random number area at 0xD80000: random_position moves on by the word it points
   at (within 0x80000) and the word at the new place is taken. random_value hands back the two words, the
   first in the low half. */
void next_random(uint32_t *p_random_value, uint32_t *p_words_left, uint32_t *p_random_offset)
{
	uint32_t random_value = *p_random_value;
	uint32_t random_offset = 0, words_left = 0;

	random_offset = random_position;
	words_left = 2u;
	do
	{
		random_offset = (random_offset + mem_read16(random_offset + 0xD80000u)) & 0x7FFFFu;
		random_value = (random_value & ~0xFFFFu) | mem_read16(random_offset + 0xD80000u);
		random_value = (random_value << 16) | (random_value >> 16);
		words_left--;
	} while (words_left != 0u);
	random_position = random_offset;
	*p_random_value = random_value;
	*p_words_left = words_left;
	*p_random_offset = random_offset;
}

/* Unused: create_child_shadow with sprite_flags 0x1000 and no sprite_scale, palette 0x9FFE, and without
   touching current_player. */
void create_child_shadow_unscaled(uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0, owner = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	owner = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0x1000u;
		OBJECT(shadow)->sprite_palette = 0x9FFEu;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->shadow_blink = 0u;
		OBJECT(owner)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

/* Unused: create_child_shadow with palette 0x9FFE instead of 0x87A0. */
void create_child_shadow_9ffe(uint16_t scale, uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	current_player = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0u;
		OBJECT(shadow)->sprite_palette = 0x9FFEu;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->sprite_scale = scale;
		OBJECT(shadow)->shadow_blink = 0u;
		OBJECT(current_player)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

/* Gives the current object a shadow: a pool 2 block (allocate_object_2) with sprite 0x5D90, palette
   0x87A0, sprite_flags 0, sprite_scale = scale and parent = the object, kept as its child (0xFFFF when
   pool 2 is empty). It leaves current_player = the object. */
void create_child_shadow(uint16_t scale, uint32_t shadow)
{
	uint32_t free_count = 0, ignored = 0;

	OBJECT(current_object)->child = 0xFFFFu;
	current_player = current_object;
	allocate_object_2(&free_count, &ignored, &shadow);
	if (free_count != 0u)
	{
		OBJECT(shadow)->sprite_flags = 0u;
		OBJECT(shadow)->sprite_palette = 0x87A0u;
		OBJECT(shadow)->sprite_frame = 0x5D90u;
		OBJECT(shadow)->sprite_scale = scale;
		OBJECT(shadow)->shadow_blink = 0u;
		OBJECT(current_player)->child = shadow;
		OBJECT(shadow)->parent = current_object;
	}
}

/* Places the current object's shadow (its pool 2 child) on its floor, at projected_x and projected_y +
   floor_z with the sprite offsets, palette 0x9FFE, queued at its screen_y - 0x10. With flag 0x10:
   palette 0x87A0, drawn every other frame (shadow_blink), and on the skipped frames current_object is
   left at the shadow. */
void draw_child_shadow(void)
{
	uint32_t depth = 0, ignored = 0;
	uint8_t shadow_blink = 0;

	if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + OBJECT(object_cursor)->sprite_offset_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + OBJECT(object_cursor)->sprite_offset_y;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	if ((OBJECT(object_cursor)->flags & 0x10u) != 0u)
	{
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		shadow_blink = ~OBJECT(current_object)->shadow_blink;
		OBJECT(current_object)->shadow_blink = shadow_blink;
		if (shadow_blink == 0u)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
		}
		return;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
}

/* Unused: 0 when the current object's floor_block (low half) is one of nearby_floor_blocks, the floor
   blocks near the screen, else 0xFFFFFFFF (also when there are none). */
uint32_t is_floor_block_off_screen(void)
{
	uint32_t zero = 0;
	uint16_t floor_block = 0;
	uint32_t blocks_left = 0, cursor = 0, off_screen = 0;
	uint16_t listed_block = 0;

	off_screen = 0u;
	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	if (blocks_left == 0u)
	{
		off_screen = 0xFFFFFFFFu;
		return off_screen;
	}
	floor_block = (uint16_t)OBJECT(current_object)->floor_block;
	do
	{
		listed_block = mem_read16(cursor);
		zero = (uint16_t)(floor_block - listed_block) == 0u;
		cursor += 2u;
		blocks_left--;
	} while ((blocks_left != 0u) & (floor_block != listed_block));
	if (zero)
		return off_screen;
	off_screen = 0xFFFFFFFFu;
	return off_screen;
}

/* 0xFFFFFFFF when the current object is more than 0x20 outside the 0x140 x 0xE0 screen (screen_x + 0x20
   >= 0x180 or screen_y + 0x20 >= 0x120, unsigned), else 0. */
uint32_t is_off_screen(void)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x20u) >= 0x180u || (uint16_t)(OBJECT(current_object)->screen_y + 0x20u) >= 0x120u)
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

/* is_off_screen with margin_x and margin_y in place of 0x20. */
uint32_t is_off_screen_margin(uint16_t margin_x, uint16_t margin_y)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((uint16_t)(0x140u + margin_x * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin_x) || (uint16_t)(0xE0u + margin_y * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin_y))
		off_screen = 0xFFFFFFFFu;
	return off_screen;
}

/* Hands back 0xFFFFFFFF once the current object, having been on screen (flag 0x800), is more than 0x20
   outside the 0x140 x 0xE0 screen, else 0; sets flag 0x800 while it is on the screen (no margin until
   then). */
uint32_t check_left_screen(void)
{
	uint16_t edge_margin = 0;
	uint32_t off_screen = 0;

	off_screen = 0u;
	edge_margin = 0x20u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		edge_margin = 0u;
	if (0x140u + edge_margin * 2u <= (uint16_t)(OBJECT(current_object)->screen_x + edge_margin) || 0xE0u + edge_margin * 2u <= (uint16_t)(OBJECT(current_object)->screen_y + edge_margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

/* check_left_screen with margin instead of 0x20 once the object has been on screen (0 before). */
uint32_t check_left_screen_margin(uint16_t margin)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		margin = 0u;
	if ((uint16_t)(0x140u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin) || (uint16_t)(0xE0u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

/* check_left_screen with margin once the object has been on screen and unseen_margin before. */
uint32_t check_left_screen_margins(uint16_t margin, uint16_t unseen_margin)
{
	uint32_t off_screen = 0;

	off_screen = 0u;
	if ((OBJECT(current_object)->flags & 0x800u) == 0u)
		margin = (uint16_t)unseen_margin;
	if ((uint16_t)(0x140u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_x + margin) || (uint16_t)(0xE0u + margin * 2u) <= (uint16_t)(OBJECT(current_object)->screen_y + margin))
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
			off_screen = 0xFFFFFFFFu;
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	return off_screen;
}

/* allocate_object for code outside run_objects (the spawners the game modes run): it first reloads
   free_objects, first_object and last_object from free_object_stack and first/last_object_address. */
void allocate_object_outside_loop(uint32_t *p_free_count, uint32_t *p_object)
{
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	allocate_object(p_free_count, p_object);
}

/* delete_object for code outside run_objects: reloads free_objects, first_object and last_object first,
   and stores first_object and last_object back into first/last_object_address after. */
void delete_object_outside_loop(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	delete_object(object, p_free_count, p_previous);
	first_object_address = first_object;
	last_object_address = last_object;
}

/* Spawns the stage's stage_spawns_by_handler records as the scroll reaches them, one a frame (the list
   starts on the stage's first call): an object at the record's x and y with its word in sprite_frame (a
   parameter its handler reads) and its handler. A record 0x100 or more behind the scroll is skipped; it
   waits while no object is free. */
void spawn_objects_by_handler(uint32_t *p_list_offset, uint32_t *p_record, uint32_t *p_object)
{
	uint32_t object = *p_object;
	uint32_t free_count = 0, record = 0;
	uint32_t old_init_flags = 0;

	old_init_flags = stage_init_flags;
	stage_init_flags = old_init_flags | 1u;
	if ((old_init_flags & 1u) == 0u)
	{
		*p_list_offset = stage_spawns_by_handler[stage];
		handler_spawn_cursor = stage_spawns_by_handler_ADDRESS + *p_list_offset;
	}
	record = handler_spawn_cursor;
	if (HANDLER_SPAWN_RECORD(record)->position <= scroll_position)
	{
		if ((uint16_t)(scroll_position - HANDLER_SPAWN_RECORD(record)->position) >= 0x100u)
			handler_spawn_cursor += 0xCu;
		else
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count != 0u)
			{
				record = handler_spawn_cursor + 2u;
				OBJECT(object)->x_int = HANDLER_SPAWN_RECORD(record - 0x2u)->x;
				OBJECT(object)->x_fraction = 0u;
				record += 2u;
				OBJECT(object)->y_int = HANDLER_SPAWN_RECORD(record - 0x4u)->y;
				OBJECT(object)->y_fraction = 0u;
				record += 2u;
				OBJECT(object)->sprite_frame = HANDLER_SPAWN_RECORD(record - 0x6u)->handler_arg;
				record += 2u;
				OBJECT(object)->handler = HANDLER_SPAWN_RECORD(record - 0x8u)->handler;
				record += 4u;
				handler_spawn_cursor += 0xCu;
				*p_record = record;
				*p_object = object;
				return;
			}
		}
	}
	*p_record = record;
	*p_object = object;
}

/* electric_fence_state = 0 when a game starts (fence_post_init sets 1, flame_machine_remove 0xFF;
   fence_post_update switches on it). */
void reset_linked_objects_flag(void)
{
	electric_fence_state = 0u;
}

/* Starts the stage's stage_spawns_by_type list: type_spawn_cursor at its start, type_spawn_stage =
   stage; also clears string_count, stage_trigger_bits and unused_type_spawn_byte. */
void start_spawns_by_type(void)
{
	uint32_t stage_number = 0;

	stage_number = stage;
	type_spawn_stage = stage_number;
	type_spawn_cursor[0] = stage_spawns_by_type[stage_number];
	unused_type_spawn_byte = 0u;
	string_count = 0;
	stage_trigger_bits = 0u;
}

/* Spawns the next stage_spawns_by_type record once scroll_position passes it: object_types[type] gives
   spawn_count and the handler (spawn_object_with_pieces); its words go to projected_x, projected_y, z,
   spawn_arg_1 and spawn_arg_2 (unproject_object works out x and y). A record the pools cannot take is
   dropped. Then count_active_players. */
void spawn_objects_by_type(uint32_t *p_player_object, uint32_t *p_player_index, uint32_t *p_first, uint32_t *p_projected_x, uint32_t *p_projected_y, uint32_t *p_object_type, uint32_t *p_object)
{
	uint16_t spawn_z = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, record = 0, scroll = 0, trigger_scroll = 0;

	if (type_spawn_stage != stage)
	{
		start_spawns_by_type();
		return;
	}
	record = type_spawn_cursor[0];
	scroll = scroll_position;
	trigger_scroll = TYPE_SPAWN_RECORD(record)->position;
	record += 2u;
	if (trigger_scroll == 0xFFFFu || scroll <= trigger_scroll)
	{
		count_active_players(p_player_object, p_player_index);
		return;
	}
	*p_projected_x = TYPE_SPAWN_RECORD(record - 0x2u)->projected_x;
	record += 2u;
	*p_projected_y = TYPE_SPAWN_RECORD(record - 0x4u)->projected_y;
	record += 2u;
	spawn_z = (uint16_t)TYPE_SPAWN_RECORD(record - 0x6u)->z;
	record += 2u;
	if ((int32_t)trigger_scroll <= (int32_t)scroll)
	{
		*p_object_type = 0x100000u + TYPE_SPAWN_RECORD(record - 0x8u)->object_type * 8u;
		spawn_count = OBJECT_TYPE_RECORD(*p_object_type)->part_count;
		record += 2u;
		*p_object_type += 4u;
		spawn_object_with_pieces(0x08DCB1u, &scroll, &ignored, p_player_index, p_first, p_object);
		if ((uint8_t)scroll == 0u)
		{
			type_spawn_cursor[0] += 0xEu;
			count_active_players(p_player_object, p_player_index);
			return;
		}
		current_object = *p_first;
		*p_projected_y = *p_projected_y << 16;
		OBJECT(current_object)->projected_x = *p_projected_x;
		OBJECT(current_object)->projected_y = *p_projected_y;
		OBJECT(current_object)->z = (uint32_t)spawn_z << 16;
		OBJECT(current_object)->flags = 0u;
		OBJECT(current_object)->handler = OBJECT_TYPE_RECORD(*p_object_type - 0x4u)->handler;
		OBJECT(current_object)->spawn_arg_1 = TYPE_SPAWN_RECORD(record - 0xAu)->spawn_arg_1;
		record += 2u;
		OBJECT(current_object)->spawn_arg_2 = TYPE_SPAWN_RECORD(record - 0xCu)->spawn_arg_2;
		type_spawn_cursor[0] = record + 2u;
		unproject_object(&ignored_2, &ignored_3, p_first, p_object);
		count_active_players(p_player_object, p_player_index);
		return;
	}
	type_spawn_cursor[0] += 0xEu;
	count_active_players(p_player_object, p_player_index);
}

/* active_player_count = how many of the three players' objects (player_block_offsets) lack flag 0x400
   (out of the game). */
void count_active_players(uint32_t *p_player_object, uint32_t *p_player_index)
{
	uint32_t player = 0, player_index = 0, players_left = 0;

	players_left = 3u;
	player_index = 2u;
	active_player_count = 0u;
	do
	{
		player = GAME_VARIABLES + player_block_offsets[player_index];
		*p_player_object = LINKED(player, PLAYER(player)->object);
		if ((OBJECT(*p_player_object)->flags & 0x400u) == 0u)
			active_player_count++;
		players_left--;
		player_index--;
	} while (players_left != 0u);
	*p_player_index = player_index;
}

/* Spawns a pool 1 object (init_object, parent = object_cursor, next_handler = return_address) and
   spawn_count - 1 pool 2 pieces chained from it (add_object_pieces): first, spawned 1. Nothing when
   spawn_count <= 0 or the pools are short, but its empty-stack test keeps the high half of the caller's
   free_count: a non-zero one passes. */
void spawn_object_with_pieces(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t piece_count = *p_piece_count;
	uint32_t ignored = 0, pool_2_free = 0;
	uint16_t free_blocks = 0, spawn_total = 0;

	if ((int16_t)spawn_count > 0)
	{
		free_objects = free_object_stack_ADDRESS;
		first_object = first_object_address;
		last_object = last_object_address;
		*p_spawned &= ~0xFFu;
		*p_free_count = LINKED(*p_free_count, free_object_stack);
		if (*p_free_count != 0u)
		{
			piece_count = (uint16_t)(spawn_count - 1u);
			if (piece_count != 0u)
			{
				pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
				free_blocks = pool_2_free;
				*p_free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
				if (piece_count <= free_blocks)
				{
					allocate_object(&ignored, p_block);
					init_object(*p_block);
					OBJECT(*p_block)->next_handler = return_address;
					OBJECT(*p_block)->parent = object_cursor;
					OBJECT(*p_block)->child = 0xFFFFu;
					*p_first = *p_block;
					*p_spawned = 1u;
					spawn_total = spawn_count;
					piece_count = (uint16_t)(spawn_total - 1u);
					if (spawn_total != 1u)
					{
						add_object_pieces(p_spawned, &piece_count, p_free_count, p_first, p_block);
						*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (piece_count & 0xFFFFu);
						return;
					}
				}
				current_object = object_cursor;
				*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (piece_count & 0xFFFFu);
				return;
			}
			allocate_object(&ignored, p_block);
			init_object(*p_block);
			OBJECT(*p_block)->next_handler = return_address;
			OBJECT(*p_block)->parent = object_cursor;
			OBJECT(*p_block)->child = 0xFFFFu;
			*p_first = *p_block;
			*p_spawned = 1u;
			spawn_total = spawn_count;
			piece_count = (uint16_t)(spawn_total - 1u);
			if (spawn_total != 1u)
			{
				add_object_pieces(p_spawned, &piece_count, p_free_count, p_first, p_block);
				*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (piece_count & 0xFFFFu);
				return;
			}
			current_object = object_cursor;
			*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (piece_count & 0xFFFFu);
			return;
		}
	}
	current_object = object_cursor;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | (piece_count & 0xFFFFu);
}

/* add_object_pieces with spawn_count pieces chained after object_cursor. */
void add_pieces_to_current_object(uint32_t *p_spawned, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t piece_count = 0;

	*p_block = object_cursor;
	piece_count = spawn_count;
	add_object_pieces(p_spawned, &piece_count, p_free_count, p_first, p_block);
}

/* Allocates piece_count pool 2 blocks (init_object_2) chained through child after block, each with
   parent = spawn_first (block) and no child of its own; first = spawn_first, spawned = 1. Nothing
   (spawned 0) when free_object_stack_2 holds fewer than piece_count. */
void add_object_pieces(uint32_t *p_spawned, uint32_t *p_piece_count, uint32_t *p_free_count, uint32_t *p_first, uint32_t *p_block)
{
	uint16_t piece_count = (uint16_t)*p_piece_count;
	uint32_t block = *p_block;
	uint32_t ignored = 0, object = 0, pool_2_free = 0;
	uint16_t free_blocks = 0;

	spawn_first = block;
	spawn_last = block;
	*p_spawned &= ~0xFFu;
	piece_count = (uint16_t)piece_count;
	pool_2_free = LINKED(WORK_RAM, free_object_stack_2[0]);
	free_blocks = pool_2_free;
	*p_free_count = (pool_2_free & ~0xFFFFu) | (uint16_t)(free_blocks - piece_count);
	if (piece_count <= free_blocks)
	{
		spawn_count = piece_count;
		do
		{
			allocate_object_2(&ignored, p_free_count, &block);
			init_object_2(block);
			object = spawn_last;
			spawn_last_link = block;
			OBJECT(object)->child = block;
			OBJECT(block)->parent = (uint16_t)spawn_first;
			OBJECT(block)->child = 0xFFFFu;
			spawn_count--;
		} while (spawn_count != 0u);
		*p_first = spawn_first;
		*p_spawned = 1u;
	}
	current_object = object_cursor;
	*p_piece_count = (*p_piece_count & 0xFFFF0000u) | piece_count;
	*p_block = block;
}

/* Spawns spawn_count pool 1 objects chained through child (each init_object, parent = object_cursor,
   next_handler = return_address); first = the first one, spawned 1. Nothing when spawn_count <= 0 or
   free_object_stack holds fewer than spawn_count (spawned 0). */
void spawn_objects(uint32_t return_address, uint32_t *p_spawned, uint32_t *p_first, uint32_t *p_block)
{
	uint32_t block = *p_block;
	uint32_t ignored = 0;

	if ((int16_t)spawn_count <= 0)
	{
		*p_first = spawn_first;
		current_object = object_cursor;
		return;
	}
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	*p_spawned = 0u;
	if ((uint16_t)free_object_stack < spawn_count)
	{
		*p_first = spawn_first;
		current_object = object_cursor;
		return;
	}
	allocate_object(&ignored, &block);
	spawn_first = block;
	spawn_last = block;
	OBJECT(block)->parent = object_cursor;
	OBJECT(block)->child = 0xFFFFu;
	init_object(block);
	OBJECT(block)->next_handler = return_address;
	for (;;)
	{
		spawn_last = block;
		spawn_count--;
		*p_spawned = 1u;
		if (spawn_count == 0u)
			break;
		allocate_object(&ignored, &block);
		init_object(block);
		OBJECT(block)->next_handler = return_address;
		OBJECT(spawn_last)->child = block;
		OBJECT(block)->parent = object_cursor;
		OBJECT(block)->child = 0xFFFFu;
	}
	*p_first = spawn_first;
	current_object = object_cursor;
	*p_block = block;
}

/* Unused: breaks up the child chain from object: each child link becomes 0xFFFF, until one already is. */
void unlink_child_chain(uint32_t object)
{
	uint16_t next_child = 0;

	for (;;)
	{
		next_child = OBJECT(object)->child;
		if (OBJECT(object)->child == 0xFFFFu)
			break;
		OBJECT(object)->child = 0xFFFFu;
		object = (object & ~0xFFFFu) | next_child;
	}
}

/* init_object_fields with mode 0: the whole object. */
void init_object(uint32_t object)
{
	uint8_t mode = 0;

	mode = 0u;
	init_object_fields(object, mode);
}

/* init_object_fields with mode 1: only the first 0x30 bytes, the size of a pool 2 block. */
void init_object_2(uint32_t object)
{
	uint8_t mode = 0;

	mode = 1u;
	init_object_fields(object, mode);
}

/* Clears an object for use: draw_next 0, sprite_flags 4, sprite_frame and sprite_palette 0, sprite_scale
   0x100, the layer, screen position, hitbox, anchor and size 0, handler = delete_object_and_children,
   direction and projected_x/projected_y 0. With mode 0 also flags and init_object_fields_tail; previous,
   next, parent and child are kept. */
void init_object_fields(uint32_t object, uint8_t mode)
{
	OBJECT(object)->draw_next = 0u;
	OBJECT(object)->sprite_flags = 4u;
	OBJECT(object)->sprite_frame = 0u;
	OBJECT(object)->sprite_palette = 0u;
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->sprite_layer = 0u;
	OBJECT(object)->screen_y = 0;
	OBJECT(object)->screen_x = 0;
	OBJECT(object)->hitbox_size_x = 0u;
	OBJECT(object)->hitbox_size_y = 0u;
	OBJECT(object)->anchor_x = 0;
	OBJECT(object)->anchor_y = 0;
	OBJECT(object)->sprite_height = 0;
	OBJECT(object)->sprite_width = 0;
	OBJECT(object)->handler = delete_object_and_children_ADDRESS;
	OBJECT(object)->direction = 0u;
	OBJECT(object)->projected_x = 0;
	OBJECT(object)->projected_y = 0;
	OBJECT(object)->projected_y_int = 0;
	if ((uint8_t)mode != 0u)
		return;
	OBJECT(object)->flags = 0u;
	init_object_fields_tail(object, mode);
}

/* Unused: init_object_fields_tail on the current object with mode 2: only speed_x, speed_y and speed_z
   are cleared. */
void clear_object_speeds(void)
{
	uint8_t mode = 0;
	uint32_t object = 0;

	mode = 2u;
	object = current_object;
	init_object_fields_tail(object, mode);
}

/* speed_x, speed_y and speed_z = 0; unless mode is 2 (clear_object_speeds), also z and every field after
   it up to next_handler. */
void init_object_fields_tail(uint32_t object, uint8_t mode)
{
	OBJECT(object)->speed_x = 0;
	OBJECT(object)->speed_y = 0;
	OBJECT(object)->speed_z = 0;
	if ((uint8_t)mode != 2u)
	{
		OBJECT(object)->z = 0;
		OBJECT(object)->animation_speed = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->kind = 0u;
		OBJECT(object)->animation = 0u;
		OBJECT(object)->x = 0;
		OBJECT(object)->y = 0;
		OBJECT(object)->floor_z = 0;
		OBJECT(object)->previous_floor_z = 0;
		OBJECT(object)->floor_block = 0;
		OBJECT(object)->previous_floor_block = 0;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_height = 0u;
		OBJECT(object)->hit_list_next = 0u;
		OBJECT(object)->touched_by = 0u;
		OBJECT(object)->touch_distance = 0;
		OBJECT(object)->speed = 0;
		OBJECT(object)->platform_list_next = 0u;
		OBJECT(object)->platform = 0u;
		OBJECT(object)->floor_slope = 0u;
		OBJECT(object)->floor_type = 0u;
		OBJECT(object)->wall = 0u;
		OBJECT(object)->floor_attribute = 0u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->health = 0;
		OBJECT(object)->var_7F = 0u;
		OBJECT(object)->var_80 = 0u;
		OBJECT(object)->var_81 = 0u;
		OBJECT(object)->var_82 = 0u;
		OBJECT(object)->var_84 = 0u;
		OBJECT(object)->var_86 = 0u;
		OBJECT(object)->draw_depth = 0u;
		OBJECT(object)->var_8A = 0u;
		OBJECT(object)->var_8C = 0u;
		OBJECT(object)->var_8E = 0u;
		OBJECT(object)->var_8F = 0u;
		OBJECT(object)->var_90 = 0u;
		OBJECT(object)->spawn_arg_1 = 0u;
		OBJECT(object)->spawn_arg_2 = 0u;
		OBJECT(object)->var_96 = 0u;
		OBJECT(object)->var_98 = 0u;
		OBJECT(object)->var_9A = 0u;
		OBJECT(object)->next_handler = 0u;
	}
}

/* Moves the current object in projected space: speed_x adds up in projected_x_carry, whose whole pixels
   go to projected_x; projected_y + speed_y, z + speed_z; then unproject_object (x and y, the floor, the
   screen position). */
void move_object_projected(uint32_t *p_screen_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	unproject_object(p_screen_y, p_block_offset, p_slope_height, p_cursor);
}

/* Works the current object's x and y out from projected_x and projected_y, the inverse of
   project_object: x = (projected_x * 0x100 - whole projected_y * 0x200) * 0xB5, y the same with +; then
   find_floor and update_screen_position_no_offset. */
void unproject_object(uint32_t *p_screen_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t ignored = 0, scaled_x = 0, scaled_y = 0;

	scaled_x = (uint32_t)OBJECT(current_object)->projected_x << 8;
	scaled_y = (uint32_t)((int32_t)OBJECT(current_object)->projected_y >> 16) << 9;
	*p_block_offset = scaled_x;
	OBJECT(current_object)->x = (scaled_x - scaled_y) * 0xB5u;
	OBJECT(current_object)->y = (scaled_y + *p_block_offset) * 0xB5u;
	find_floor(p_screen_y, &ignored, p_block_offset, p_slope_height, p_cursor);
	*p_screen_y = update_screen_position_no_offset();
}

/* screen_x = projected_x - view_x and screen_y = (projected_y + z) >> 16 - view_y, without the sprite
   offsets update_screen_position adds; hands back screen_y. */
uint32_t update_screen_position_no_offset(void)
{
	uint32_t unscrolled_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	unscrolled_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	unscrolled_y = (unscrolled_y & ~0xFFFFu) | (uint16_t)(unscrolled_y - view_y);
	OBJECT(current_object)->screen_y = unscrolled_y;
	return unscrolled_y;
}

/* move_object_projected without unproject_object: x, y and the floor stay as they were and only
   screen_x/screen_y follow (without the sprite offsets). */
void move_object_projected_no_floor(void)
{
	OBJECT(current_object)->move_object_projected.projected_x_carry += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x += OBJECT(current_object)->move_object_projected.projected_x_carry_int;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->move_object_projected.projected_x_carry_int = 0;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y;
}

/* 1 while screen_x is within -0xC0..0x200 and screen_y within -0xC0..0x1A0 (delete_if_off_screen's
   bounds), else 0. */
uint32_t is_on_screen(void)
{
	uint16_t screen_x = 0, screen_y = 0;
	uint32_t on_screen = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	on_screen = 0u;
	if ((int16_t)screen_x >= -0xC0 && (int16_t)screen_x <= 0x200 && (int16_t)screen_y >= -0xC0 && (int16_t)screen_y <= 0x1A0)
		on_screen = 1u;
	return on_screen;
}

/* Deletes the current object and its children (delete_object_and_children, deleted = 1) when screen_x
   leaves -0xC0..0x200 or screen_y leaves -0xC0..0x1A0; deleted = 0 otherwise. */
void delete_if_off_screen(uint32_t *p_deleted, uint32_t *p_block_offset, uint32_t *p_slope_height)
{
	uint16_t screen_x = 0, screen_y = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_y = (uint16_t)OBJECT(current_object)->screen_y;
	if ((int16_t)screen_x < -0xC0 || (int16_t)screen_x > 0x200 || (int16_t)screen_y < -0xC0 || (int16_t)screen_y > 0x1A0)
	{
		delete_object_and_children(p_deleted, p_block_offset, p_slope_height);
		return;
	}
	*p_deleted = 0u;
}

/* The default handler (init_object_fields): frees the current object's child chain (pool 1 children with
   delete_object, pool 2 ones with free_object_2), then delete_current_object; deleted = 1. Halts in
   int_default if free_object_stack_2's count goes past 0x100. */
void delete_object_and_children(uint32_t *p_deleted, uint32_t *p_block_offset, uint32_t *p_slope_height)
{
	uint32_t child = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(object_cursor)->child != 0xFFFFu)
	{
		child = LINKED(WORK_RAM, OBJECT(object_cursor)->child);
		deleted_child = child;
		for (;;)
		{
			if ((int16_t)child < 0x5000)
				delete_object(child, &ignored, p_block_offset);
			else
			{
				*p_block_offset = free_object_2((uint16_t)child);
				*p_deleted = LINKED(WORK_RAM, free_object_stack_2[0]);
				if ((int16_t)*p_deleted > 0x100)
				{
					int_default(p_deleted, p_block_offset, p_slope_height, &ignored_2);
					return;
				}
			}
			object = LINKED(WORK_RAM, deleted_child);
			child = LINKED(object, OBJECT(object)->child);
			if ((uint16_t)child == 0xFFFFu)
				break;
			deleted_child = child;
		}
	}
	delete_current_object();
	*p_deleted = 1u;
}

/* Copies object_cursor's x, y, projected_x, projected_y, z and floor_z into object. */
void copy_position_to(uint32_t object)
{
	OBJECT(object)->x = OBJECT(object_cursor)->x;
	OBJECT(object)->y = OBJECT(object_cursor)->y;
	OBJECT(object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(object)->z = OBJECT(object_cursor)->z;
	OBJECT(object)->floor_z = OBJECT(object_cursor)->floor_z;
}

/* The current object's screen_x and screen_y = object_cursor's + offset_x and offset_y. */
void place_sprite_near_cursor(uint16_t offset_x, uint16_t offset_y)
{
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += offset_x;
	OBJECT(current_object)->screen_y += offset_y;
}

/* Copies the current object's projected_x, projected_y, z and floor_z into object_cursor (x and y are
   left alone). */
void copy_position_to_cursor(void)
{
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
}

/* The current object takes object_cursor's x, y, projected_x, projected_y, z and floor_z. */
void copy_position_from_cursor(void)
{
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->floor_z = OBJECT(object_cursor)->floor_z;
}

/* The current object's screen_x and screen_y = object_cursor's. Its only caller, draw_eel_piece_parts,
   is unused. */
void copy_screen_position_from_cursor(void)
{
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
}

/* Distance and angle (square_root, angle_of_vector) to each player in play, players 3, 2, 1, into the
   per-player and nearest_player variables. Its direction then becomes the last angle worked out plus
   direction_to_player_offsets[nearest_player], an offset (0x1C-0x20) rather than that player's angle, so
   it does not face the nearest player. */
uint32_t face_nearest_player(uint32_t direction, uint32_t octant)
{
	uint16_t dy = 0, player_dx = 0, player_dy = 0;
	uint32_t abs_dy = 0, distance = 0, ignored = 0, player_index = 0, player_object = 0;
	uint16_t diff_x = 0, diff_y = 0;
	uint32_t players_left = 0;

	nearest_player_distance = 0x7FFF;
	player_index = 3u;
	for (;;)
	{
		players_left = player_index;
		player_index = players_left - 1u;
		if (players_left == 0u)
			break;
		player_object = GAME_VARIABLES + player_block_offsets[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
		if ((OBJECT(player_object)->flags & 0x400u) != 0u)
		{
			RAM16(GAME_VARIABLES + distance_to_player_offsets[player_index]) = 0x7FFFu;
			RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = 0x7FFFu;
			continue;
		}
		diff_x = OBJECT(player_object)->projected_x - OBJECT(current_object)->projected_x;
		distance = diff_x;
		player_dx = (uint16_t)distance;
		if ((int16_t)diff_x < 0)
			distance = (uint16_t)(0u - distance);
		diff_y = OBJECT(player_object)->projected_y_int - OBJECT(current_object)->projected_y_int;
		abs_dy = diff_y;
		player_dy = (uint16_t)abs_dy;
		if ((int16_t)diff_y < 0)
			abs_dy = (uint16_t)(0u - abs_dy);
		distance = (int16_t)distance;
		abs_dy = (int16_t)abs_dy;
		distance = distance * distance + abs_dy * abs_dy;
		square_root(&distance, &ignored);
		if ((int16_t)distance <= nearest_player_distance)
		{
			nearest_player = player_index;
			nearest_player_distance = distance;
			nearest_player_dx = player_dx;
			nearest_player_dy = player_dy;
		}
		RAM16(GAME_VARIABLES + distance_to_player_offsets[player_index]) = distance;
		direction = player_dx;
		dy = (uint16_t)player_dy;
		angle_of_vector(dy, &direction, &octant);
		RAM16(GAME_VARIABLES + direction_to_player_offsets[player_index]) = direction;
	}
	player_index = nearest_player;
	player_object = GAME_VARIABLES + player_block_offsets[player_index];
	player_object = LINKED(player_object, PLAYER(player_object)->object);
	nearest_player_z = OBJECT(player_object)->z_int;
	nearest_player_floor_z = OBJECT(player_object)->floor_z_int;
	direction = (uint16_t)(direction + direction_to_player_offsets[player_index]);
	OBJECT(current_object)->direction = direction;
	direction_to_nearest_player = direction;
	return octant;
}

/* nearest_player_distance = the smallest |player x - x| (whole x) over the three players, in the game or
   not. */
void find_nearest_player_x(void)
{
	uint16_t distance_x = 0;
	uint32_t player_index = 0, player_object = 0;
	uint16_t diff_x = 0;
	uint32_t checked_index = 0;

	nearest_player_distance = 0x7FFF;
	player_index = 2u;
	do
	{
		player_object = GAME_VARIABLES + player_block_offsets[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
		diff_x = OBJECT(player_object)->x_int - OBJECT(current_object)->x_int;
		distance_x = diff_x;
		if ((int16_t)diff_x < 0)
			distance_x = (uint16_t)(0u - distance_x);
		if ((int16_t)distance_x <= nearest_player_distance)
			nearest_player_distance = distance_x;
		checked_index = player_index;
		player_index = checked_index - 1u;
	} while (checked_index != 0u);
}

/* The current object's direction = the angle to the object at r14 (projected coordinates), then
   distance_to_object, whose distance it hands back. */
uint32_t face_object(void)
{
	uint16_t dy = 0;
	uint32_t dx = 0, ignored = 0;

	dx = (uint16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (uint16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	angle_of_vector(dy, &dx, &ignored);
	OBJECT(current_object)->direction = dx;
	return distance_to_object();
}

/* nearest_player_distance = the distance from the current object to the object at r14 (square_root of dx
   * dx + dy * dy, projected coordinates), also handed back. */
uint32_t distance_to_object(void)
{
	uint32_t distance = 0, dy = 0, ignored = 0;

	distance = (int16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (int16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	if ((int32_t)distance < 0)
		distance = 0u - distance;
	if ((int32_t)dy < 0)
		dy = 0u - dy;
	distance = distance * distance + dy * dy;
	square_root(&distance, &ignored);
	nearest_player_distance = distance;
	return distance;
}

/* The current object's direction = the angle (angle_of_vector) to the object at r14 in projected
   coordinates; hands back angle_of_vector's octant. */
uint32_t direction_to_object(void)
{
	uint16_t dy = 0;
	uint32_t dx = 0, octant = 0;

	dx = (uint16_t)(OBJECT(r14)->projected_x - OBJECT(current_object)->projected_x);
	dy = (uint16_t)(OBJECT(r14)->projected_y_int - OBJECT(current_object)->projected_y_int);
	angle_of_vector(dy, &dx, &octant);
	OBJECT(current_object)->direction = dx;
	return octant;
}

/* distance_x = |dx| and distance_y = |dy| (whole x and y) from the current object to the object at r14,
   also kept in distance_to_player_1 and distance_to_player_2. */
void distance_xy_to_object(uint32_t *p_distance_x, uint32_t *p_distance_y)
{
	uint32_t distance_x = 0, distance_y = 0, x_int = 0, y_int = 0;
	uint16_t dx = 0, dy = 0;

	x_int = (uint16_t)OBJECT(r14)->x_int;
	dx = x_int - OBJECT(current_object)->x_int;
	distance_x = dx;
	if ((int16_t)dx < 0)
		distance_x = (uint16_t)(0u - distance_x);
	distance_to_player_1 = distance_x;
	y_int = (uint16_t)OBJECT(r14)->y_int;
	dy = y_int - OBJECT(current_object)->y_int;
	distance_y = dy;
	if ((int16_t)dy < 0)
		distance_y = (uint16_t)(0u - distance_y);
	distance_to_player_2 = distance_y;
	*p_distance_x = distance_x;
	*p_distance_y = distance_y;
}

/* Gravity for the current object: speed_z + 0x6000 each frame z is not floor_z, and when z + speed_z
   would reach floor_z it lands (z = floor_z, speed_z 0); z itself is moved elsewhere. height = floor_z -
   z (0 on the floor); over_pit = 1 when floor_z_int is 0x800 or more. */
void fall_to_floor(uint32_t *p_height, uint32_t *p_over_pit, uint32_t *p_z)
{
	uint8_t over_pit = 0;
	uint32_t height = 0;

	over_pit = 1u;
	if (OBJECT(current_object)->floor_z_int < 0x800)
		over_pit = 0u;
	if ((uint32_t)OBJECT(current_object)->z != (uint32_t)OBJECT(current_object)->floor_z)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		if ((int32_t)((uint32_t)OBJECT(current_object)->z + OBJECT(current_object)->speed_z) < OBJECT(current_object)->floor_z)
		{
			*p_z = OBJECT(current_object)->z;
			height = OBJECT(current_object)->floor_z - *p_z;
		}
		else
		{
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->speed_z = 0;
			height = 0u;
		}
	}
	else
	{
		OBJECT(current_object)->speed_z = 0;
		height = 0u;
	}
	*p_height = height;
	*p_over_pit = (*p_over_pit & 0xFFFFFF00u) | over_pit;
}

/* The current object's screen position = object_cursor's, moved down to object_cursor's floor (floor_z -
   z): the place for its shadow. Hands back 0xFF, the bucket drawn first (under everything). */
uint32_t place_sprite_at_cursor_floor(void)
{
	uint32_t bucket = 0;

	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
	bucket = 0xFFu;
	return bucket;
}

/* speed_x = sine_table[direction] * speed >> 6 and speed_y = -(cosine_table[direction] * speed >> 6),
   with direction taken & 0x3FF. */
void direction_to_speed(uint32_t speed, uint32_t *p_direction, uint32_t *p_speed_x, uint32_t *p_speed_y)
{
	uint32_t direction = *p_direction;

	direction &= 0x3FFu;
	*p_speed_x = (int32_t)(sine_table[direction] * speed) >> 6;
	*p_speed_y = 0u - ((int32_t)(cosine_table[direction] * speed) >> 6);
	*p_direction = direction;
}

/* Hands back the current object's screen_y moved down by object_cursor's floor_z - z (whole pixels):
   where object_cursor's floor is on the screen. */
uint32_t cursor_floor_screen_y(uint32_t floor_screen_y)
{
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->floor_z_int;
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)(floor_screen_y - OBJECT(object_cursor)->z_int);
	floor_screen_y = (floor_screen_y & ~0xFFFFu) | (uint16_t)(floor_screen_y + OBJECT(current_object)->screen_y);
	return floor_screen_y;
}

/* Unused: sets explosion frames 0xF-0x1F, palette 0x82A0 and handler explosion_animate, then runs on
   into explode_object, which puts frames 0x1F-0x2B and palette 0x86D0 over them (and plays sound 7). */
void explode_object_medium(void)
{
	OBJECT(current_object)->explode_object.frame_index = 0xF;
	OBJECT(current_object)->explode_object.last_frame = 0x1F;
	OBJECT(current_object)->sprite_palette = 0x82A0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	explode_object();
}

/* Turns the current object into the big explosion: explosion_frames 0x1F-0x2B, palette 0x86D0, handler
   explosion_animate; sound 7. */
void explode_object(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->explode_object.frame_index = 0x1F;
	OBJECT(current_object)->explode_object.last_frame = 0x2B;
	OBJECT(current_object)->sprite_palette = 0x86D0u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	OBJECT(current_object)->explode_object.frame_toggle = 0u;
	sound_id = 7u;
	play_sound(sound_id, &ignored, &ignored_2);
}

/* An explosion: unproject_object, the next explosion_frames frame every other frame until last_frame,
   then removed (delete_object_and_children); drawn every frame (queue_sprite). */
void explosion_animate(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t toggle = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->explosion.frame_toggle++;
	toggle = OBJECT(current_object)->explosion.frame_toggle & 1u;
	OBJECT(current_object)->explosion.frame_toggle = toggle;
	if (toggle == 0u)
	{
		OBJECT(current_object)->explosion.frame_index++;
		if (OBJECT(current_object)->explosion.last_frame < OBJECT(current_object)->explosion.frame_index)
		{
			delete_object_and_children(&ignored, &ignored_2, &ignored_3);
			return;
		}
	}
	OBJECT(current_object)->sprite_frame = explosion_frames[(uint8_t)OBJECT(current_object)->explosion.frame_index];
	queue_sprite(0u);
}

/* Releases rings_to_release rings from object_cursor, one scatter_rings_random ring at a time: at its
   position, heading released_ring_angle at speed 0x60 (spread at random with released_rings_random),
   each 0x10 frames after the last (released_ring_wait); released_ring_flip marks their palettes. It
   never checks that a ring was made. */
void drop_rings(uint32_t shadow, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t ring = *p_ring;
	uint32_t count = 0, ignored = 0, ignored_2 = 0, new_ring = 0, random_bits = 0;

	if ((int16_t)rings_to_release <= 0)
		return;
	r14 = 0u;
	do
	{
		if (released_rings_random != 0u)
		{
			next_random(&shadow, &ignored, &ignored_2);
			*p_random_value = shadow;
		}
		count = 1u;
		scatter_rings_random(shadow, count, p_allocated, &ring);
		new_ring = ring;
		copy_position_to(new_ring);
		OBJECT(new_ring)->parent = object_cursor;
		OBJECT(new_ring)->animation_speed = 0u;
		OBJECT(new_ring)->animation_timer = 0;
		OBJECT(new_ring)->speed_z_int++;
		OBJECT(new_ring)->released_ring.bounce_speed = OBJECT(new_ring)->speed_z;
		OBJECT(new_ring)->direction = released_ring_angle;
		OBJECT(new_ring)->speed = 0x60;
		OBJECT(new_ring)->speed &= 0x3FFu;
		OBJECT(new_ring)->released_ring.release_delay = r14;
		OBJECT(new_ring)->handler = released_ring_wait_ADDRESS;
		r14 = (uint16_t)(r14 + 0x10u);
		if (released_rings_random != 0u)
		{
			random_bits = *p_random_value & 0x7Fu;
			OBJECT(new_ring)->speed += random_bits;
			OBJECT(new_ring)->direction += (random_bits & 0x3Fu) * 2u;
			OBJECT(new_ring)->direction -= 0x40u;
		}
		shadow = LINKED(new_ring, OBJECT(new_ring)->child);
		OBJECT(shadow)->child = 0xFFFFu;
		if (released_ring_flip == 1u)
		{
			OBJECT(ring)->sprite_palette |= 1u;
			OBJECT(shadow)->sprite_palette |= 0x2000u;
			OBJECT(ring)->sprite_palette ^= 0x8000u;
			OBJECT(shadow)->sprite_palette ^= 0x8000u;
		}
		if (released_ring_flip == 2u)
		{
			OBJECT(ring)->sprite_palette |= 0x4000u;
			OBJECT(shadow)->sprite_palette |= 0x4000u;
			OBJECT(ring)->sprite_palette ^= 0x8000u;
			OBJECT(shadow)->sprite_palette ^= 0x8000u;
		}
		rings_to_release--;
	} while ((int16_t)rings_to_release >= 1);
	*p_ring = ring;
}

/* A released ring waiting release_delay frames, undrawn; then sound 0x13 and it goes on as
   released_ring_start. */
void released_ring_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t release_delay = 0;

	release_delay = OBJECT(current_object)->released_ring.release_delay;
	OBJECT(current_object)->released_ring.release_delay = release_delay - 1u;
	if (release_delay != 0u)
		return;
	sound_id = 0x13u;
	play_sound(sound_id, &ignored, &ignored_2);
	OBJECT(current_object)->released_ring.release_delay = 0u;
	OBJECT(current_object)->handler = released_ring_start_ADDRESS;
	released_ring_start();
}

/* A released ring that cannot be collected yet: released_ring_move for pickup_delay frames, then it
   clears flag 0x200 and goes on as released_ring_bounce. */
void released_ring_start(void)
{
	uint16_t pickup_delay = 0;

	OBJECT(current_object)->released_ring.projected_x_carry_int = 0;
	pickup_delay = OBJECT(current_object)->released_ring.pickup_delay;
	OBJECT(current_object)->released_ring.pickup_delay = pickup_delay - 1u;
	if ((int16_t)pickup_delay > 1)
	{
		released_ring_move(0u);
		return;
	}
	OBJECT(current_object)->flags &= ~0x200u;
	OBJECT(current_object)->handler = released_ring_bounce_ADDRESS;
	released_ring_bounce();
}

/* A released ring that can be collected, for lifetime frames: with flag 0x20 its direction turns by
   0x300 (speeds cleared for the frame); touched (flag 4) it plays sound 0x13 and is removed, unless its
   toucher is a player in state 0xD or 0x20 with hurt_contact_delay. Else released_ring_move; at the end
   it goes on as scattered_ring_vanish. */
void released_ring_bounce(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, player = 0;
	uint16_t lifetime = 0;

	lifetime = OBJECT(current_object)->released_ring.lifetime;
	OBJECT(current_object)->released_ring.lifetime = lifetime - 1u;
	if ((int16_t)lifetime > 1)
	{
		if ((OBJECT(current_object)->flags & 0x20u) != 0u)
		{
			OBJECT(current_object)->direction += 0x300u;
			OBJECT(current_object)->direction &= 0x3FFu;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			released_ring_move(player);
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		if ((int8_t)OBJECT(object)->kind <= 2)
		{
			player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 9);
			if ((PLAYER(player)->state == 0xDu || PLAYER(player)->state == 0x20u) && (uint8_t)PLAYER(player)->hurt_contact_delay != 0u)
			{
				released_ring_move(player);
				return;
			}
		}
		sound_id = 0x13u;
		play_sound(sound_id, &ignored, &ignored_2);
		delete_object_and_children(&ignored_3, &ignored_2, &ignored_4);
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

/* Moves a released ring (move_object_projected, fall_to_floor): on landing it bounces up again at
   bounce_speed - bounce_speed / 4 + 0x10000, and in the air animation_frame gains 3 a frame. Speeds from
   direction and speed, animate (spawn_ring_glint at frame 4), drawn at its floor with its shadow, in the
   touchable and rider lists. */
void released_ring_move(uint32_t cursor)
{
	uint32_t bounce_speed = 0, direction = 0, height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &cursor);
	fall_to_floor(&height, &ignored_4, &ignored_2);
	OBJECT(current_object)->animation_frame += 3u;
	if (height == 0u)
	{
		OBJECT(current_object)->animation_frame -= 3u;
		bounce_speed = OBJECT(current_object)->released_ring.bounce_speed;
		bounce_speed = bounce_speed - ((int32_t)bounce_speed >> 2) + 0x10000u;
		OBJECT(current_object)->released_ring.bounce_speed = bounce_speed;
		OBJECT(current_object)->speed_z = bounce_speed;
	}
	direction = OBJECT(current_object)->direction;
	speed = OBJECT(current_object)->speed;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	animate(&ignored_4, &ignored_5, &ignored_6);
	if (OBJECT(current_object)->animation_frame == 4u)
		spawn_ring_glint(cursor);
	queue_sprite_at_floor();
	add_to_touchable_list(direction);
	add_to_rider_list();
	draw_child_shadow();
}

/* Throws count debris pieces (spawn_objects, handler debris_fly) from the current object: each at its
   position, palette 0x8300, thrown up at a random speed_z, with random extra zoom, direction and speed
   on top of the object's own, debris_frames 0-9 or (odd random) 0xA-0x13. */
void spawn_debris(uint32_t spawned, uint16_t count, uint32_t block)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0, random_byte = 0, random_value = 0;

	spawn_count = count;
	spawn_objects(0x08E73Au, &spawned, &random_value, &block);
	if (spawned == 0u)
		return;
	r14 = random_value;
	do
	{
		OBJECT(r14)->x = OBJECT(current_object)->x;
		OBJECT(r14)->y = OBJECT(current_object)->y;
		OBJECT(r14)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(r14)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(r14)->z = OBJECT(current_object)->z;
		OBJECT(r14)->z_int--;
		OBJECT(r14)->floor_z = OBJECT(current_object)->floor_z;
		OBJECT(r14)->sprite_palette = 0x8300u;
		OBJECT(r14)->debris.last_frame = 9;
		OBJECT(r14)->debris.frame_timer = 3u;
		OBJECT(r14)->debris.lifetime = 0xFu;
		next_random(&random_value, &ignored, &ignored_2);
		OBJECT(r14)->sprite_scale += random_value & 0xFFu;
		OBJECT(r14)->speed_z = -0x40000;
		random_byte = random_value & 0x3FFFFu;
		OBJECT(r14)->speed_z -= random_byte;
		OBJECT(r14)->direction = OBJECT(current_object)->direction;
		random_byte = random_byte & 0xFFu;
		OBJECT(r14)->direction += random_byte;
		OBJECT(r14)->speed = OBJECT(current_object)->speed;
		random_byte &= 0x3Fu;
		OBJECT(r14)->speed += random_byte;
		if ((random_byte & 1u) != 0u)
		{
			OBJECT(r14)->debris.frame_index = 0xA;
			OBJECT(r14)->debris.last_frame = 0x13;
		}
		OBJECT(r14)->handler = debris_fly_ADDRESS;
		object = r14;
		r14 = LINKED(r14, OBJECT(r14)->child);
		OBJECT(object)->child = 0xFFFFu;
	} while ((uint16_t)r14 != 0xFFFFu);
}

/* A debris piece: move_object_projected, removed off screen (delete_if_off_screen), fall_to_floor. In
   the air its direction and speed are added to speed_x/speed_y every frame and it steps through
   debris_frames every 4 frames (back 9 past last_frame); on the floor it stops and blinks for lifetime
   frames, then goes. */
void debris_fly(void)
{
	uint32_t deleted = 0, direction = 0, height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t phase = 0, lifetime = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	fall_to_floor(&height, &ignored_5, &ignored_2);
	if (height != 0u)
	{
		direction = OBJECT(current_object)->direction;
		speed = OBJECT(current_object)->speed;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		OBJECT(current_object)->debris.frame_timer++;
		phase = OBJECT(current_object)->debris.frame_timer & 3u;
		OBJECT(current_object)->debris.frame_timer = phase;
		if (phase != 0u)
		{
			queue_sprite_at_floor();
			return;
		}
		OBJECT(current_object)->debris.frame_index++;
		if (OBJECT(current_object)->debris.last_frame < OBJECT(current_object)->debris.frame_index)
			OBJECT(current_object)->debris.frame_index -= 9u;
		OBJECT(current_object)->sprite_frame = debris_frames[(uint8_t)OBJECT(current_object)->debris.frame_index];
		queue_sprite_at_floor();
		return;
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	lifetime = OBJECT(current_object)->debris.lifetime;
	OBJECT(current_object)->debris.lifetime = lifetime - 1u;
	if (lifetime == 0u)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->debris.lifetime & 1u) != 0u)
		return;
	queue_sprite_at_floor();
}

/* Spawns the stage's object_group_spawns records as the scroll reaches them. Types 0, 2 and 6 build a
   group (spawn_object_groups_build_group; 2 with group_depth_step -8, 6 silent), 1 and 8 spawn a group
   leader, 3 a group slot trigger, 4 an ice platform leader, 5 a sand block leader, 7 a waterfall. */
void spawn_object_groups(uint32_t *p_list_offset, uint32_t *p_free_stack, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t kind_index = 0, record = 0, spawned = 0;
	uint16_t spawn_position = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x200u;
	if ((init_flags & 0x200u) == 0u)
	{
		*p_list_offset = object_group_spawns[stage];
		group_spawn_cursor = object_group_spawns_ADDRESS + *p_list_offset;
	}
	for (;;)
	{
		record = group_spawn_cursor;
		spawn_position = GROUP_SPAWN_RECORD(record)->position;
		record += 2u;
		if (scroll_position < spawn_position)
		{
			return;
		}
		group_silent = 0u;
		kind_index = GROUP_SPAWN_RECORD(record - 0x2u)->kind * 2u;
		record++;
		switch (kind_index)
		{
		case 0x0u:
			spawn_object_groups_no_depth_step(record, p_list_offset, p_slope_z, p_count, p_object);
			return;
		case 0x2u:
		case 0x10u:
			spawn_group_leader(record, &spawned, p_object);
			if (spawned == 0u)
			{
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0x4u:
			group_depth_step = -8;
			spawn_object_groups_build_group(record, p_list_offset, p_slope_z, p_count, p_object);
			return;
		case 0x6u:
			spawn_group_slot_trigger(record, &spawned, p_count, p_object);
			if (spawned == 0u)
			{
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0x8u:
			spawn_ice_platform_leader(record, &spawned, p_list_offset, p_object);
			if (spawned == 0u)
			{
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xAu:
			spawn_sand_block_leader(record, &spawned, p_object);
			if (spawned == 0u)
			{
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		case 0xCu:
			spawn_object_groups_mark_silent(record, p_list_offset, p_slope_z, p_count, p_object);
			return;
		case 0xEu:
			spawn_waterfall(record, &spawned, p_free_stack, p_object);
			if (spawned == 0u)
			{
				return;
			}
			group_spawn_cursor += 0xEu;
			continue;
		default:
			invalid_jump(0x9125Eu + kind_index);
		}
	}
}

/* Type 6 records: group_silent = 0xFF, which the members keep (crumbling_floor_start_falling then plays
   no sound 0xD), then as type 0 (spawn_object_groups_no_depth_step). */
void spawn_object_groups_mark_silent(uint32_t record, uint32_t *p_list_offset, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t list_offset = *p_list_offset;

	group_silent = 0xFFu;
	spawn_object_groups_no_depth_step(record, &list_offset, p_slope_z, p_count, p_object);
	*p_list_offset = (*p_list_offset & 0xFFFF0000u) | (list_offset & 0xFFFFu);
}

/* Types 0 and 6: group_depth_step = 0 (type 2 sets -8), then spawn_object_groups_build_group. */
void spawn_object_groups_no_depth_step(uint32_t record, uint32_t *p_list_offset, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t list_offset = *p_list_offset;

	group_depth_step = 0;
	spawn_object_groups_build_group(record, &list_offset, p_slope_z, p_count, p_object);
	*p_list_offset = (*p_list_offset & 0xFFFF0000u) | (list_offset & 0xFFFFu);
}

/* Builds a group when the scroll is within 0x100 past the record (else skips it) and free_object_stack
   has room: a row of stage_object_records members sharing the leader, floor block and direction, each
   numbered and set on the floor (slope too), depth_offset stepped by group_depth_step; a group slot when
   the record asks. */
void spawn_object_groups_build_group(uint32_t record, uint32_t *p_list_offset, uint32_t *p_slope_z, uint32_t *p_count, uint32_t *p_object)
{
	uint16_t list_offset = (uint16_t)*p_list_offset;
	uint32_t count = *p_count;
	uint32_t object = *p_object;
	uint32_t floor_block_record = 0, free_count = 0, member_record = 0, slope_number = 0, slope_record = 0, slot_cursor = 0;
	uint16_t slot_taken = 0, members_left = 0;

	if ((uint16_t)(scroll_position - GROUP_SPAWN_RECORD(record - 0x3u)->position) >= 0x100u)
	{
		group_spawn_cursor += 0x18u;
		*p_list_offset = (*p_list_offset & 0xFFFF0000u) | list_offset;
		*p_count = count;
		*p_object = object;
		return;
	}
	group_leader = group_leaders[GROUP_SPAWN_RECORD(record - 0x3u)->leader];
	record++;
	group_takes_slot = GROUP_SPAWN_RECORD(record - 0x4u)->takes_slot;
	record += 2u;
	group_floor_block = GROUP_SPAWN_RECORD(record - 0x6u)->floor_block;
	record += 4u;
	group_member_count = GROUP_SPAWN_RECORD(record - 0xAu)->member_count;
	record += 2u;
	if (free_object_stack < group_member_count)
		return;
	group_step_x = GROUP_SPAWN_RECORD(record - 0xCu)->step_x;
	record += 2u;
	group_step_y = GROUP_SPAWN_RECORD(record - 0xEu)->step_y;
	record += 2u;
	group_direction = GROUP_SPAWN_RECORD(record - 0x10u)->direction;
	record += 2u;
	group_member_x = GROUP_SPAWN_RECORD(record - 0x12u)->member_x;
	record += 2u;
	group_member_y = GROUP_SPAWN_RECORD(record - 0x14u)->member_y;
	object = GROUP_SPAWN_RECORD(record - 0x14u)->member_template + stage_object_records_ADDRESS;
	group_record = object;
	group_member_number = 0u;
	if (group_takes_slot != 0u)
	{
		slot_cursor = group_slots_ADDRESS;
		count = 0x40u;
		do
		{
			slot_taken = mem_read16(slot_cursor);
			slot_cursor += 2u;
			count--;
		} while ((count != 0u) & (slot_taken != 0u));
		mem_write16(slot_cursor - 2u, 1u);
		count = (count & ~0xFFFFu) | (uint16_t)(count + 1u);
		list_offset = (uint16_t)(0x40u - count);
		group_slot_index = list_offset;
	}
	do
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
		{
			*p_list_offset = (*p_list_offset & 0xFFFF0000u) | list_offset;
			*p_count = count;
			*p_object = object;
			return;
		}
		OBJECT(object)->x_int = group_member_x;
		OBJECT(object)->x_fraction = 0u;
		group_member_x += group_step_x;
		OBJECT(object)->y_int = group_member_y;
		OBJECT(object)->y_fraction = 0u;
		group_member_y += group_step_y;
		member_record = group_record;
		OBJECT(object)->sprite_flags = STAGE_OBJECT_RECORD(member_record)->sprite_flags;
		member_record += 2u;
		OBJECT(object)->sprite_frame = STAGE_OBJECT_RECORD(member_record - 0x2u)->sprite_frame;
		member_record += 2u;
		OBJECT(object)->sprite_palette = STAGE_OBJECT_RECORD(member_record - 0x4u)->sprite_palette;
		member_record += 2u;
		OBJECT(object)->handler = STAGE_OBJECT_RECORD(member_record - 0x6u)->handler;
		member_record += 4u;
		OBJECT(object)->animation = STAGE_OBJECT_RECORD(member_record - 0xAu)->animation;
		member_record += 4u;
		OBJECT(object)->hitbox_size_x = STAGE_OBJECT_RECORD(member_record - 0xEu)->hitbox_size_x;
		member_record += 2u;
		OBJECT(object)->hitbox_size_y = STAGE_OBJECT_RECORD(member_record - 0x10u)->hitbox_size_y;
		member_record += 2u;
		OBJECT(object)->sprite_offset_x = STAGE_OBJECT_RECORD(member_record - 0x12u)->sprite_offset_x;
		member_record++;
		OBJECT(object)->sprite_offset_y = STAGE_OBJECT_RECORD(member_record - 0x13u)->sprite_offset_y;
		OBJECT(object)->group_member.depth_offset = STAGE_OBJECT_RECORD(member_record - 0x13u)->depth_offset;
		OBJECT(object)->floor_block = group_floor_block;
		OBJECT(object)->direction = group_direction;
		OBJECT(object)->parent = group_leader;
		OBJECT(object)->group_member.normal_palette = OBJECT(object)->sprite_palette;
		OBJECT(object)->group_member.silent_row = group_silent;
		OBJECT(object)->member_number = group_member_number;
		group_member_number++;
		OBJECT(object)->group_member.group_slot = group_slot_index;
		OBJECT(object)->spawn_arg_1_low = group_takes_slot;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 6u;
		OBJECT(object)->floor_slope = mem_read8(floor_blocks + 2u + OBJECT(object)->floor_block + 0x13u);
		OBJECT(object)->flags = 0u;
		OBJECT(object)->member_state = 0u;
		if ((uint8_t)group_depth_step != 0u)
		{
			list_offset = (uint16_t)group_depth_step;
			OBJECT(object)->group_member.depth_offset += (uint32_t)group_member_count * list_offset;
		}
		floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
		OBJECT(object)->floor_z_int = FLOOR_BLOCK(floor_block_record)->height;
		OBJECT(object)->floor_z_fraction = 0u;
		if (FLOOR_BLOCK(floor_block_record)->slope != 0u)
		{
			slope_number = FLOOR_BLOCK(floor_block_record)->slope;
			slope_record = floor_slopes_ADDRESS + (uint16_t)(slope_number * 2u) * 4u;
			if ((FLOOR_SLOPE(slope_record)->flags & 2u) == 0u)
				*p_slope_z = (uint32_t)OBJECT(object)->y - FLOOR_BLOCK(floor_block_record)->y_min;
			else
				*p_slope_z = (uint32_t)OBJECT(object)->x - FLOOR_BLOCK(floor_block_record)->x_min;
			*p_slope_z = (uint32_t)((int32_t)*p_slope_z >> 16) * FLOOR_SLOPE(slope_record)->factor;
			OBJECT(object)->floor_z += *p_slope_z;
		}
		OBJECT(object)->z = OBJECT(object)->floor_z;
		members_left = group_member_count;
		group_member_count = members_left - 1u;
	} while ((int16_t)members_left > 1);
	group_spawn_cursor += 0x18u;
	*p_list_offset = (*p_list_offset & 0xFFFF0000u) | list_offset;
	*p_count = count;
	*p_object = object;
}

/* A crumbling floor block of a group (stage 4 rock ledges, stage 9 ice blocks), a platform: within 0x60
   of its leader (parent) it cracks, once the leader passes it falls with gravity 0x2000 and is removed
   below the screen; a rider with flag 0x400000 makes it fall at once. While grey_screen_active it is
   only drawn, in palette 0x83A0. */
void crumbling_floor_update(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0;

	switch (OBJECT(current_object)->crumbling_floor.state)
	{
	case 0:
	case 3:
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->crumbling_floor.state++;
	case 1:
		update_screen_position(&ignored, &ignored_2);
		if (grey_screen_active != 0u)
		{
			OBJECT(current_object)->sprite_palette = 0x83A0u;
			crumbling_floor_draw();
			return;
		}
		if (grey_screen_ended != 0u)
			OBJECT(current_object)->sprite_palette = OBJECT(current_object)->crumbling_floor.normal_palette;
		if ((OBJECT(current_object)->flags & 8u) != 0u && (OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->flags & 0x400000u) != 0u)
			OBJECT(current_object)->flags |= 0x400000u;
		if ((OBJECT(current_object)->flags & 0x400000u) != 0u)
		{
			crumbling_floor_play_crack_steps();
			return;
		}
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
			{
				crumbling_floor_start_falling();
				return;
			}
			if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x60u))
			{
				crumbling_floor_crack();
				return;
			}
			crumbling_floor_draw();
			return;
		case 1:
			if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			{
				crumbling_floor_start_falling();
				return;
			}
			if ((int16_t)(OBJECT(current_player)->y_int - 0x60u) <= OBJECT(current_object)->y_int)
			{
				crumbling_floor_crack();
				return;
			}
			crumbling_floor_draw();
			return;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
			{
				crumbling_floor_start_falling();
				return;
			}
			if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x60u))
			{
				crumbling_floor_crack();
				return;
			}
			crumbling_floor_draw();
			return;
		case 3:
			if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
			{
				crumbling_floor_start_falling();
				return;
			}
			if ((int16_t)(OBJECT(current_player)->x_int - 0x60u) <= OBJECT(current_object)->x_int)
			{
				crumbling_floor_crack();
				return;
			}
			crumbling_floor_draw();
			return;
		default:
			invalid_jump(0x91555u + OBJECT(current_object)->direction * 2u);
		}
		break;
	case 2:
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->animation_timer < 0)
			apply_step_screen_offset();
		else
		{
			at_last_step = animate_with_screen_offset(0u);
			if ((int8_t)at_last_step < 0)
				OBJECT(current_object)->animation_timer = -1;
		}
		if (OBJECT(current_object)->screen_y > 0x1C0)
		{
			delete_current_object();
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
			add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->crumbling_floor.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x914DFu + OBJECT(current_object)->crumbling_floor.state * 3u);
	}
}

/* Within 0x60 of the leader: the cracking steps (crumbling_floor_play_crack_steps); once they have
   stopped (animation_timer negative) the block holds the last step's screen offset
   (apply_step_screen_offset) and is drawn. */
void crumbling_floor_crack(void)
{
	if (OBJECT(current_object)->animation_timer < 0)
	{
		apply_step_screen_offset();
		crumbling_floor_draw();
		return;
	}
	crumbling_floor_play_crack_steps();
}

/* Advances the cracking steps (animate_with_screen_offset); at step 4 they stop (animation_timer -1) and
   with flag 0x400000 the block falls at once (crumbling_floor_start_falling); else crumbling_floor_draw. */
void crumbling_floor_play_crack_steps(void)
{
	animate_with_screen_offset(0u);
	if ((int8_t)OBJECT(current_object)->animation_frame < 4)
	{
		crumbling_floor_draw();
		return;
	}
	OBJECT(current_object)->animation_timer = -1;
	if ((OBJECT(current_object)->flags & 0x400000u) != 0u)
	{
		crumbling_floor_start_falling();
		return;
	}
	crumbling_floor_draw();
}

/* The block starts to fall: state + 1 (falling), speed_z 0, the crumbling steps from step 4 at speed 6,
   and sound 0xD unless silent_row is set or screen_x is past an edge that depends on direction; then
   crumbling_floor_draw. */
void crumbling_floor_start_falling(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->crumbling_floor.state++;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->animation_speed = 6u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 4u;
	if (OBJECT(current_object)->crumbling_floor.silent_row != 0u)
	{
		crumbling_floor_draw();
		return;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->screen_x < 0)
		{
			crumbling_floor_draw();
			return;
		}
		sound_id = 0xDu;
		play_sound(sound_id, &ignored, &ignored_2);
		crumbling_floor_draw();
		return;
	case 1:
		if (OBJECT(current_object)->screen_x > 0x180)
		{
			crumbling_floor_draw();
			return;
		}
		sound_id = 0xDu;
		play_sound(sound_id, &ignored, &ignored_2);
		crumbling_floor_draw();
		return;
	case 2:
		if (OBJECT(current_object)->screen_x < -8)
		{
			crumbling_floor_draw();
			return;
		}
		sound_id = 0xDu;
		play_sound(sound_id, &ignored, &ignored_2);
		crumbling_floor_draw();
		return;
	case 3:
		if (OBJECT(current_object)->screen_x > 0x1A0)
		{
			crumbling_floor_draw();
			return;
		}
		sound_id = 0xDu;
		play_sound(sound_id, &ignored, &ignored_2);
		crumbling_floor_draw();
		return;
	default:
		invalid_jump(0x9161Cu + OBJECT(current_object)->direction * 2u);
	}
}

/* Puts the block in the platform list and, while screen_x is within -0xA0..0x1E0, queues it at screen_y
   + depth_offset - sprite_offset_y, kept in draw_depth (the players standing on it read it). */
void crumbling_floor_draw(void)
{
	uint32_t ignored = 0, sprite_depth = 0;

	add_to_platform_list();
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
		return;
	sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->crumbling_floor.depth_offset - OBJECT(current_object)->sprite_offset_y);
	OBJECT(current_object)->draw_depth = sprite_depth;
	queue_sprite_at_depth(&ignored, &sprite_depth);
}

/* Spawns group leader n (the record's first byte & 3), kept in group_leaders[n]: sprite_frame = n, the
   record's x, y, z (floor_z = z), direction and path in group_leader_data, handler group_leader_move
   (group_leader_wait while group_leader_0_held or group_leaders_1_3_held). spawned 0xFFFFFFFF, or 0 when
   no object is free. */
void spawn_group_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;
	uint32_t leader_index = 0;

	allocate_object_outside_loop(p_spawned, &leader);
	if (*p_spawned != 0u)
	{
		leader_index = LEADER_SPAWN(record)->leader & 3u;
		OBJECT(leader)->sprite_frame = leader_index;
		group_leaders[leader_index] = leader;
		record++;
		OBJECT(leader)->x_int = LEADER_SPAWN(record - 0x1u)->x;
		OBJECT(leader)->x_fraction = 0u;
		record += 2u;
		OBJECT(leader)->y_int = LEADER_SPAWN(record - 0x3u)->y;
		OBJECT(leader)->y_fraction = 0u;
		record += 2u;
		OBJECT(leader)->z_int = LEADER_SPAWN(record - 0x5u)->z;
		OBJECT(leader)->z_fraction = 0u;
		OBJECT(leader)->floor_z = OBJECT(leader)->z;
		record += 2u;
		OBJECT(leader)->direction = LEADER_SPAWN(record - 0x7u)->direction;
		OBJECT(leader)->group_leader_move.path = LEADER_SPAWN(record - 0x7u)->path + group_leader_data_ADDRESS;
		OBJECT(leader)->handler = group_leader_move_ADDRESS;
		OBJECT(leader)->flags = 0u;
		OBJECT(leader)->sprite_offset_x = 0;
		OBJECT(leader)->sprite_offset_y = 0;
		OBJECT(leader)->group_leader_move.holding_back = 0u;
		OBJECT(leader)->group_leader_move.wait_timer = 1;
		if (group_leaders_1_3_held != 0u || group_leader_0_held != 0u)
			OBJECT(leader)->handler = group_leader_wait_ADDRESS;
		*p_spawned = 0xFFFFFFFFu;
	}
	*p_leader = leader;
}

/* Holds a new group leader: leader 0 (sprite_frame 0) until group_leader_0_held is clear, the others
   until group_leaders_1_3_held is; then handler group_leader_move. */
void group_leader_wait(void)
{
	if (OBJECT(current_object)->sprite_frame == 0u)
	{
		if (group_leader_0_held == 0u)
			OBJECT(current_object)->handler = group_leader_move_ADDRESS;
	}
	else if (group_leaders_1_3_held == 0u)
	{
		OBJECT(current_object)->handler = group_leader_move_ADDRESS;
		return;
	}
}

/* An invisible marker leading a group along its row (direction 0/4 +y, 1 -y, 2 +x, 3 -x) by its
   group_leader_data entries (group_leader_read_script, group_leader_move_step); the members fall as it
   passes. It holds back (base_speed 0) in stage 8 in game modes 0x17 and 0x1D, or while
   no_player_in_play, player_mishap or player_dropping_in. */
void group_leader_move(void)
{
	uint16_t hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 1u;
	if ((flags & 1u) == 0u)
	{
		group_leader_read_script();
		return;
	}
	if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
	{
		group_leader_move_step();
		return;
	}
	if ((stage != 8u || (current_game_mode != 0x17u && current_game_mode != 0x1Du)) && no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		group_leader_read_script();
		return;
	}
	OBJECT(current_object)->group_leader_move.holding_back = 0xFFu;
	OBJECT(current_object)->group_leader_move.saved_base_speed = OBJECT(current_object)->group_leader_move.base_speed;
	OBJECT(current_object)->group_leader_move.base_speed = 0;
	OBJECT(current_object)->group_leader_move.hold_start_position = scroll_position;
	OBJECT(current_object)->group_leader_move.hold_timer = 0xFFu;
	OBJECT(current_object)->group_leader_move.saved_threshold = OBJECT(current_object)->group_leader_move.screen_threshold;
	hold_threshold = 0xFF90u;
	if ((OBJECT(current_object)->direction_low & 1u) != 0u)
		hold_threshold = 0x1B0u;
	OBJECT(current_object)->group_leader_move.screen_threshold = hold_threshold;
	group_leader_move_step();
}

/* The leader's wait_timer counts down; then, once scroll_position reaches its next group_leader_data
   entry, the entry gives screen_threshold, catch_up_factor, base_speed and wait_timer (a negative
   catch-up factor ends the path and removes the leader). Then group_leader_move_step. */
void group_leader_read_script(void)
{
	uint32_t cursor = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0;

	wait_timer = OBJECT(current_object)->group_leader_move.wait_timer;
	OBJECT(current_object)->group_leader_move.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		group_leader_move_step();
		return;
	}
	OBJECT(current_object)->group_leader_move.wait_timer = 0;
	cursor = OBJECT(current_object)->group_leader_move.path;
	trigger_scroll = CHASE_STEP_AT_SCROLL(cursor)->position;
	cursor += 2u;
	if (scroll_position < trigger_scroll)
	{
		group_leader_move_step();
		return;
	}
	if (CHASE_STEP_AT_SCROLL(cursor - 0x2u)->catch_up_factor < 0)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->group_leader_move.screen_threshold = CHASE_STEP_AT_SCROLL(cursor - 0x2u)->screen_threshold;
	cursor += 2u;
	OBJECT(current_object)->group_leader_move.catch_up_factor = CHASE_STEP_AT_SCROLL(cursor - 0x4u)->catch_up_factor;
	cursor += 2u;
	OBJECT(current_object)->group_leader_move.base_speed = CHASE_STEP_AT_SCROLL(cursor - 0x6u)->base_speed;
	cursor += 2u;
	OBJECT(current_object)->group_leader_move.wait_timer = CHASE_STEP_AT_SCROLL(cursor - 0x8u)->wait;
	OBJECT(current_object)->group_leader_move.path = cursor + 2u;
	group_leader_move_step();
}

/* project_object, then chaser_speed: base_speed while screen_x is past screen_threshold, else the
   camera's ground speed * catch_up_factor / 0x100, at least base_speed (which also ends a hold-back, as
   do 0x300 scroll units or hold_timer running out); the leader moves by it along its direction unless
   waiting (wait_timer) or grey_screen_active. */
void group_leader_move_step(void)
{
	uint16_t speed = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint8_t hold_timer = 0;

	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->group_leader_move.holding_back == 0u)
	{
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 2:
		case 4:
			if (OBJECT(current_object)->group_leader_move.screen_threshold <= OBJECT(current_object)->screen_x)
				speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
			else
			{
				OBJECT(current_object)->group_leader_move.wait_timer = 0;
				if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
				{
					OBJECT(current_object)->group_leader_move.holding_back = 0u;
					OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
					OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
				}
				compute_camera_ground_speed(0u);
				speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
				if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
				{
					speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
					chaser_speed = speed;
					if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
						return;
					switch (OBJECT(current_object)->direction)
					{
					case 0:
					case 4:
						OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
						break;
					case 1:
						OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
						break;
					case 2:
						OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
						break;
					case 3:
						OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
						break;
					default:
						invalid_jump(0x918FAu + OBJECT(current_object)->direction * 2u);
					}
					return;
				}
			}
			break;
		case 1:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->group_leader_move.screen_threshold)
			{
				speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
				chaser_speed = speed;
				if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
					return;
				switch (OBJECT(current_object)->direction)
				{
				case 0:
				case 4:
					OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
					break;
				case 1:
					OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
					break;
				case 2:
					OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
					break;
				case 3:
					OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
					break;
				default:
					invalid_jump(0x918FAu + OBJECT(current_object)->direction * 2u);
				}
				return;
			}
			OBJECT(current_object)->group_leader_move.wait_timer = 0;
			if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
			{
				OBJECT(current_object)->group_leader_move.holding_back = 0u;
				OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
				OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
				speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
			break;
		default:
			invalid_jump(0x91887u + OBJECT(current_object)->direction * 2u);
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->group_leader_move.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->group_leader_move.holding_back = 0u;
		OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
		OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
		compute_camera_ground_speed(0u);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
			speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->group_leader_move.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->group_leader_move.hold_timer;
		OBJECT(current_object)->group_leader_move.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->group_leader_move.holding_back = 0u;
			OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
			OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
				speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
		}
		else
		{
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 2:
			case 4:
				if (OBJECT(current_object)->group_leader_move.screen_threshold <= OBJECT(current_object)->screen_x)
					speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
				else
				{
					OBJECT(current_object)->group_leader_move.wait_timer = 0;
					if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
					{
						OBJECT(current_object)->group_leader_move.holding_back = 0u;
						OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
						OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
					}
					compute_camera_ground_speed(0u);
					speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
					if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
					{
						speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
						chaser_speed = speed;
						if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
							return;
						switch (OBJECT(current_object)->direction)
						{
						case 0:
						case 4:
							OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
							break;
						case 1:
							OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
							break;
						case 2:
							OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
							break;
						case 3:
							OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
							break;
						default:
							invalid_jump(0x918FAu + OBJECT(current_object)->direction * 2u);
						}
						return;
					}
				}
				break;
			case 1:
			case 3:
				if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->group_leader_move.screen_threshold)
				{
					speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
					chaser_speed = speed;
					if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
						return;
					switch (OBJECT(current_object)->direction)
					{
					case 0:
					case 4:
						OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
						break;
					case 1:
						OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
						break;
					case 2:
						OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
						break;
					case 3:
						OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
						break;
					default:
						invalid_jump(0x918FAu + OBJECT(current_object)->direction * 2u);
					}
					return;
				}
				OBJECT(current_object)->group_leader_move.wait_timer = 0;
				if (OBJECT(current_object)->group_leader_move.holding_back != 0u)
				{
					OBJECT(current_object)->group_leader_move.holding_back = 0u;
					OBJECT(current_object)->group_leader_move.screen_threshold = OBJECT(current_object)->group_leader_move.saved_threshold;
					OBJECT(current_object)->group_leader_move.base_speed = OBJECT(current_object)->group_leader_move.saved_base_speed;
				}
				compute_camera_ground_speed(0u);
				speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->group_leader_move.catch_up_factor) >> 8;
				if ((int16_t)speed <= OBJECT(current_object)->group_leader_move.base_speed)
					speed = (uint16_t)OBJECT(current_object)->group_leader_move.base_speed;
				break;
			default:
				invalid_jump(0x91887u + OBJECT(current_object)->direction * 2u);
			}
		}
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->group_leader_move.wait_timer > 0 || grey_screen_active != 0u)
		return;
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
		break;
	case 1:
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
		break;
	case 2:
		OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
		break;
	case 3:
		OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
		break;
	default:
		invalid_jump(0x918FAu + OBJECT(current_object)->direction * 2u);
	}
}

/* Spawns the stage's stage_record_spawns records as the scroll reaches them; the first call also clears
   the bridges' plank counts and the group slots. Type 0 builds a group on a free bridge
   (spawn_object_groups_2_pick_bridge), 2 and 3 on bridge 0 and bridge 1, and 1 and 4 spawn a group
   leader. */
void spawn_object_groups_2(uint32_t *p_list_offset, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t fill_word = 0, kind_index = 0, plank_count_var = 0, record = 0, slot_cursor = 0, spawned = 0;
	uint16_t spawn_position = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x10u;
	if ((init_flags & 0x10u) == 0u)
	{
		unused_group_spawn_word_2 = 0u;
		unused_group_spawn_word = 0u;
		unused_group_spawn_word_3 = 1u;
		*p_list_offset = stage_record_spawns[stage];
		group_spawn_cursor_2 = stage_record_spawns_ADDRESS + *p_list_offset;
		bridge_0_plank_count = 0u;
		bridge_1_plank_count = 0u;
		slot_cursor = group_slots_ADDRESS;
		*p_count = 0x10u;
		fill_word = 0u;
		do
		{
			mem_write32(slot_cursor, fill_word);
			slot_cursor += 4u;
			(*p_count)--;
		} while (*p_count != 0u);
	}
	for (;;)
	{
		record = group_spawn_cursor_2;
		spawn_position = GROUP_SPAWN_RECORD(record)->position;
		record += 2u;
		if (scroll_position < spawn_position)
		{
			return;
		}
		kind_index = GROUP_SPAWN_RECORD(record - 0x2u)->kind * 2u;
		record++;
		switch (kind_index)
		{
		case 0x0u:
			spawn_object_groups_2_pick_bridge(record, p_list_offset, p_count, p_object);
			return;
		case 0x2u:
		case 0x8u:
			break;
		case 0x4u:
			group_bridge_number = 0u;
			plank_count_var = bridge_0_plank_count_ADDRESS;
			spawn_object_groups_2_build_group(record, plank_count_var, p_list_offset, p_count, p_object);
			return;
		case 0x6u:
			group_bridge_number = 1u;
			plank_count_var = bridge_1_plank_count_ADDRESS;
			spawn_object_groups_2_build_group(record, plank_count_var, p_list_offset, p_count, p_object);
			return;
		default:
			invalid_jump(0x938A6u + kind_index);
		}
		spawn_group_leader(record, &spawned, p_object);
		if (spawned == 0u)
		{
			return;
		}
		group_spawn_cursor_2 += 0xEu;
	}
}

/* Type 0: the group goes to bridge 0 if bridge_0_plank_count is 0, else to bridge 1 if
   bridge_1_plank_count is 0 (else it waits); that count + 1, then spawn_object_groups_2_build_group. Its
   free-object wait reads the record's word at +5, the floor block's high half (0), not the member count,
   so it never waits. */
void spawn_object_groups_2_pick_bridge(uint32_t record, uint32_t *p_list_offset, uint32_t *p_count, uint32_t *p_object)
{
	uint8_t bridge_number = 0;
	uint32_t plank_count_var = 0;

	if (free_object_stack < (int16_t)(uint16_t)(GROUP_SPAWN_RECORD(record - 0x3u)->floor_block >> 16u))
		return;
	bridge_number = 0u;
	plank_count_var = bridge_0_plank_count_ADDRESS;
	if (bridge_0_plank_count == 0u)
	{
		group_bridge_number = bridge_number;
		mem_write16(plank_count_var, mem_read16(plank_count_var) + 1u);
		group_member_number = 0u;
		spawn_object_groups_2_build_group(record, plank_count_var, p_list_offset, p_count, p_object);
		return;
	}
	bridge_number = 1u;
	plank_count_var = bridge_1_plank_count_ADDRESS;
	if (bridge_1_plank_count != 0u)
		return;
	group_bridge_number = bridge_number;
	mem_write16(plank_count_var, mem_read16(plank_count_var) + 1u);
	group_member_number = 0u;
	spawn_object_groups_2_build_group(record, plank_count_var, p_list_offset, p_count, p_object);
}

/* Builds bridge group_bridge_number's planks when the scroll is within 0x100 past the record (else skips
   it): a row of stage_object_records_2 members on the floor block, each linked to the one before
   (var_9A); plank_count_var + the count, and the last plank (spawn_arg_2_low 0xFF) goes in
   bridge_0/1_last_plank. Free objects are never checked. */
void spawn_object_groups_2_build_group(uint32_t record, uint32_t plank_count_var, uint32_t *p_list_offset, uint32_t *p_count, uint32_t *p_object)
{
	uint32_t count = *p_count;
	uint32_t object = *p_object;
	uint32_t floor_block_record = 0, floor_type_record = 0, ignored = 0, last_plank_var = 0, member_record = 0, slot_cursor = 0;
	uint8_t slot_taken = 0;
	uint16_t members_left = 0;

	if ((uint16_t)(scroll_position - GROUP_SPAWN_RECORD(record - 0x3u)->position) >= 0x100u)
	{
		group_spawn_cursor_2 += 0x18u;
		return;
	}
	group_leader = group_leaders[GROUP_SPAWN_RECORD(record - 0x3u)->leader];
	record++;
	group_takes_slot = GROUP_SPAWN_RECORD(record - 0x4u)->takes_slot;
	record += 2u;
	group_floor_block = GROUP_SPAWN_RECORD(record - 0x6u)->floor_block;
	record += 4u;
	group_member_count = GROUP_SPAWN_RECORD(record - 0xAu)->member_count;
	group_previous_member = 0xFFFFu;
	mem_write16(plank_count_var, mem_read16(plank_count_var) + group_member_count);
	record += 2u;
	group_step_x = GROUP_SPAWN_RECORD(record - 0xCu)->step_x;
	record += 2u;
	group_step_y = GROUP_SPAWN_RECORD(record - 0xEu)->step_y;
	record += 2u;
	group_direction = GROUP_SPAWN_RECORD(record - 0x10u)->direction;
	record += 2u;
	group_member_x = GROUP_SPAWN_RECORD(record - 0x12u)->member_x;
	record += 2u;
	group_member_y = GROUP_SPAWN_RECORD(record - 0x14u)->member_y;
	group_record = GROUP_SPAWN_RECORD(record - 0x14u)->member_template;
	if (group_takes_slot != 0u)
	{
		slot_cursor = group_slots_ADDRESS;
		count = 0x40u;
		do
		{
			slot_taken = mem_read8(slot_cursor);
			slot_cursor++;
			count--;
		} while ((count != 0u) & (slot_taken != 0u));
		mem_write8(slot_cursor - 1u, 1u);
		count = (count & ~0xFFFFu) | (uint16_t)(count + 1u);
		group_slot_index = 0x40u - count;
	}
	do
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->x_int = group_member_x;
		OBJECT(object)->x_fraction = 0u;
		group_member_x += group_step_x;
		OBJECT(object)->y_int = group_member_y;
		OBJECT(object)->y_fraction = 0u;
		group_member_y += group_step_y;
		member_record = group_record;
		OBJECT(object)->kind = member_record;
		member_record += stage_object_records_2_ADDRESS;
		OBJECT(object)->sprite_flags = STAGE_OBJECT_RECORD_2(member_record)->sprite_flags;
		member_record += 2u;
		OBJECT(object)->sprite_frame = STAGE_OBJECT_RECORD_2(member_record - 0x2u)->sprite_frame;
		member_record += 2u;
		OBJECT(object)->sprite_palette = STAGE_OBJECT_RECORD_2(member_record - 0x4u)->sprite_palette;
		member_record += 2u;
		OBJECT(object)->handler = STAGE_OBJECT_RECORD_2(member_record - 0x6u)->handler;
		member_record += 4u;
		OBJECT(object)->animation = STAGE_OBJECT_RECORD_2(member_record - 0xAu)->animation;
		member_record += 4u;
		*p_list_offset = STAGE_OBJECT_RECORD_2(member_record - 0xEu)->odd_member_animation;
		member_record += 4u;
		if ((group_member_number & 1u) != 0u)
			OBJECT(object)->animation = *p_list_offset;
		OBJECT(object)->hitbox_size_x = STAGE_OBJECT_RECORD_2(member_record - 0x12u)->hitbox_size_x;
		member_record += 2u;
		OBJECT(object)->hitbox_size_y = STAGE_OBJECT_RECORD_2(member_record - 0x14u)->hitbox_size_y;
		OBJECT(object)->group_member.depth_offset = STAGE_OBJECT_RECORD_2(member_record - 0x14u)->depth_offset;
		OBJECT(object)->sprite_offset_x = 0x2E;
		OBJECT(object)->sprite_offset_y = 0x11;
		OBJECT(object)->member_state = 0u;
		group_member_number++;
		OBJECT(object)->member_number = group_member_number;
		OBJECT(object)->floor_block = group_floor_block;
		OBJECT(object)->spawn_arg_2_high = group_bridge_number;
		OBJECT(object)->direction = group_direction;
		OBJECT(object)->parent = group_leader;
		OBJECT(object)->group_member.previous_member = group_previous_member;
		group_previous_member = object;
		floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
		OBJECT(object)->z_int = FLOOR_BLOCK(floor_block_record)->height;
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->floor_z = OBJECT(object)->z;
		OBJECT(object)->speed_x = OBJECT(object)->z;
		OBJECT(object)->floor_slope = FLOOR_BLOCK(floor_block_record)->slope;
		floor_type_record = floor_types_ADDRESS + FLOOR_BLOCK(floor_block_record)->type_index * 4u;
		OBJECT(object)->floor_type = FLOOR_BLOCK_TYPE(floor_type_record)->floor_type;
		OBJECT(object)->floor_attribute = FLOOR_BLOCK_TYPE(floor_type_record)->floor_attribute;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->spawn_arg_2_low = 0u;
		OBJECT(object)->group_member.group_slot = group_slot_index;
		OBJECT(object)->spawn_arg_1_low = group_takes_slot;
		members_left = group_member_count;
		group_member_count = members_left - 1u;
	} while ((int16_t)members_left > 1);
	OBJECT(object)->spawn_arg_2_low = 0xFFu;
	last_plank_var = bridge_0_last_plank_ADDRESS;
	if (group_bridge_number != 0u)
		last_plank_var = bridge_1_last_plank_ADDRESS;
	mem_write16(last_plank_var, object);
	group_spawn_cursor_2 += 0x18u;
	*p_count = count;
	*p_object = object;
}

/* Empties the object list and both pools (reset_object_list, reset_object_pool_2); the run_objects after
   it finds no objects and only reloads free_objects. Called when the game mode changes. */
void reset_objects(uint32_t *p_block, uint32_t *p_stack_slot)
{
	reset_object_list();
	reset_object_pool_2(p_block, p_stack_slot);
	run_objects();
}

/* Unused: reset_objects with reset_object_pool_2_alt for the second pool. */
void reset_objects_alt(void)
{
	reset_object_list();
	reset_object_pool_2_alt();
	run_objects();
}

/* Runs every object once: from the one after last_object round to last_object, object_cursor and
   current_object = each and its handler is called (call_code); a handler that deletes its object moves
   object_cursor back. Then first/last_object_address take first_object and last_object. Nothing when
   object_count is 0. */
void run_objects(void)
{
	free_objects = free_object_stack_ADDRESS;
	if (object_count == 0u)
		return;
	first_object = first_object_address;
	last_object = last_object_address;
	object_cursor = last_object;
	do
	{
		object_cursor = LINKED(object_cursor, OBJECT(object_cursor)->next);
		current_object = object_cursor;
		call_code(OBJECT(current_object)->handler);
	} while ((uint16_t)object_cursor != (uint16_t)last_object);
	first_object_link = first_object;
	last_object_link = last_object;
}

/* Empties the object list: first/last_object_address = 0x200000, object_count 0, and free_object_stack
   holds all 0x80 blocks of 0xA0 bytes (offsets 0x0000-0x4F60 from 0x200000). */
void reset_object_list(void)
{
	uint16_t object_offset = 0;
	uint32_t objects_left = 0, stack_slot = 0;

	object_offset = 0u;
	first_object_address = 0x200000u;
	last_object_address = 0x200000u;
	objects_left = 0x80u;
	free_object_stack = 0x80;
	stack_slot = 0x20E902u;
	do
	{
		mem_write16(stack_slot, object_offset);
		objects_left--;
		stack_slot += 2u;
		object_offset += 0xA0u;
	} while (objects_left != 0u);
	object_count = objects_left;
}

/* Pops a block off free_object_stack (object = 0x200000 | its entry) without linking it into the list;
   free_count is how many were free (0: none, object left as it was). */
void take_free_object(uint32_t *p_free_count, uint32_t *p_object)
{
	*p_free_count = mem_read16(free_objects);
	if (*p_free_count == 0u)
	{
		return;
	}
	*p_object = 0x200000u | mem_read16(free_objects + *p_free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
}

/* allocate_object, or when object_count is 0 pops a block and starts the list with it: first_object and
   last_object (and their addresses) = it, linked to itself (link_object). Hands back the object. */
uint32_t allocate_object_or_start_list(uint32_t object)
{
	uint32_t ignored = 0, new_object = 0;

	if (object_count != 0u)
	{
		allocate_object(&ignored, &object);
		return object;
	}
	new_object = 0x200000u | mem_read16(free_objects + mem_read16(free_objects) * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	first_object_link = new_object;
	first_object = new_object;
	last_object = new_object;
	link_object(new_object);
	return new_object;
}

/* Pops a block off free_object_stack and links it at the end of the object list (link_object): object =
   0x200000 | its entry. free_count is how many were free; 0 means none, and object is left as it was. */
void allocate_object(uint32_t *p_free_count, uint32_t *p_object)
{
	*p_free_count = mem_read16(free_objects);
	if (*p_free_count == 0u)
	{
		return;
	}
	*p_object = 0x200000u | mem_read16(free_objects + *p_free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	link_object(*p_object);
}

/* Links object at the end of the circular list, between last_object and first_object (previous and
   next), makes it last_object (and last_object_address) and adds 1 to object_count. */
void link_object(uint32_t object)
{
	OBJECT(object)->previous = last_object;
	OBJECT(last_object)->next = object;
	OBJECT(first_object)->previous = object;
	OBJECT(object)->next = first_object;
	last_object_link = object;
	last_object = object;
	object_count++;
}

/* Unused: allocate_object, but the block is linked right after the current object instead of at the end
   (last_object moves to it when the current object was last); object_count + 1. */
void allocate_object_after_current(void)
{
	uint32_t free_count = 0, new_object = 0, object = 0;

	free_count = mem_read16(free_objects);
	if (free_count == 0u)
		return;
	new_object = 0x200000u | mem_read16(free_objects + free_count * 2u);
	mem_write16(free_objects, mem_read16(free_objects) - 1u);
	object = LINKED(current_object, OBJECT(current_object)->next);
	OBJECT(current_object)->next = new_object;
	OBJECT(object)->previous = new_object;
	OBJECT(new_object)->previous = current_object;
	OBJECT(new_object)->next = object;
	object_count++;
	if ((uint16_t)last_object == (uint16_t)current_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)new_object;
}

/* Deletes the object at object_cursor: unlinks it (fixing first_object and last_object), object_count -
   1, pushes its block on free_object_stack (current_object = it, push_free_object) and moves
   object_cursor back to the previous object, so run_objects goes on with the right next one. */
void delete_current_object(void)
{
	uint32_t object = 0, object_2 = 0;

	object_count--;
	object = LINKED(object_cursor, OBJECT(object_cursor)->previous);
	object_2 = LINKED(object_cursor, OBJECT(object_cursor)->next);
	OBJECT(object)->next = object_2;
	OBJECT(object_2)->previous = object;
	if ((uint16_t)object_cursor == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_2;
	if ((uint16_t)object_cursor == (uint16_t)last_object)
		last_object = (last_object & ~0xFFFFu) | (uint16_t)object;
	current_object = object_cursor;
	object_cursor = (object_cursor & ~0xFFFFu) | (uint16_t)object;
	push_free_object();
}

/* Pushes current_object's block on free_object_stack (count + 1, the entry on top). */
void push_free_object(void)
{
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	mem_write16(free_objects + mem_read16(free_objects) * 2u, current_object);
}

/* Unused: delete_object of the object after the current one (its next). */
void delete_next_object(void)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	object = LINKED(object_cursor, OBJECT(current_object)->next);
	delete_object(object, &ignored, &ignored_2);
}

/* Deletes object: unlinks it (fixing first_object and last_object), object_count - 1, and pushes its
   block on free_object_stack; free_count is the new count and previous the object before it.
   object_cursor is left alone. */
void delete_object(uint32_t object, uint32_t *p_free_count, uint32_t *p_previous)
{
	uint32_t object_3 = 0;

	object_count--;
	*p_previous = LINKED(object, OBJECT(object)->previous);
	object_3 = LINKED(object, OBJECT(object)->next);
	OBJECT(object_3)->previous = *p_previous;
	OBJECT(*p_previous)->next = object_3;
	if ((uint16_t)object == (uint16_t)first_object)
		first_object = (first_object & ~0xFFFFu) | (uint16_t)object_3;
	if ((uint16_t)object != (uint16_t)last_object)
	{
		mem_write16(free_objects, mem_read16(free_objects) + 1u);
		*p_free_count = mem_read16(free_objects);
		mem_write16(free_objects + *p_free_count * 2u, object);
		return;
	}
	last_object = (last_object & ~0xFFFFu) | (uint16_t)*p_previous;
	mem_write16(free_objects, mem_read16(free_objects) + 1u);
	*p_free_count = mem_read16(free_objects);
	mem_write16(free_objects + *p_free_count * 2u, object);
}

/* Fills free_object_stack_2 with all 0xFF pool 2 blocks of 0x30 bytes from 0x205000 (offsets 0x5000,
   0x5030 ...). */
void reset_object_pool_2(uint32_t *p_block, uint32_t *p_stack_slot)
{
	uint32_t block = 0, pool_blocks_left = 0, stack_slot = 0;

	block = 0x205000u;
	pool_blocks_left = 0xFFu;
	free_object_stack_2[0] = 0xFF;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x30u;
	} while (pool_blocks_left != 0u);
	*p_block = block;
	*p_stack_slot = stack_slot;
}

/* Unused: reset_object_pool_2 with 0x1D8 blocks of 0x34 bytes (0x205000-0x20AFE0). */
void reset_object_pool_2_alt(void)
{
	uint16_t block = 0;
	uint32_t pool_blocks_left = 0, stack_slot = 0;

	block = 0x5000u;
	pool_blocks_left = 0x1D8u;
	free_object_stack_2[0] = 0x1D8;
	stack_slot = 0x20EA12u;
	do
	{
		mem_write16(stack_slot, block);
		pool_blocks_left--;
		stack_slot += 2u;
		block += 0x34u;
	} while (pool_blocks_left != 0u);
}

/* Pops a block off free_object_stack_2, the second pool (object = 0x200000 | its entry), without linking
   it into the list; free_count is how many were free (0: none, object left as it was). */
void allocate_object_2(uint32_t *p_free_count, uint32_t *p_free_stack, uint32_t *p_object)
{
	*p_free_stack = free_object_stack_2_ADDRESS;
	*p_free_count = (uint16_t)free_object_stack_2[0];
	if (*p_free_count == 0u)
	{
		return;
	}
	*p_object = 0x200000u | (uint16_t)free_object_stack_2[*p_free_count];
	free_object_stack_2[0]--;
}

/* Pushes object back on free_object_stack_2 (pool 2 blocks are never linked into the list). */
uint32_t free_object_2(uint16_t object)
{
	uint32_t free_stack = 0;

	free_stack = free_object_stack_2_ADDRESS;
	free_object_stack_2[0]++;
	free_object_stack_2[(uint16_t)free_object_stack_2[0]] = object;
	return free_stack;
}
