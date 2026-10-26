/**
 *    Name:        objects/stage_10.c
 *    Description: stage 10's objects, the river: the mines and the bubble column, the lid trap, the turtle, the eel and its den, the rafts, the geysers, the waterfall and the river water, the propeller machine that chases the players, the barred gate, the rope bridge
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Unused: spawns one bubble column (spawn_object_with_pieces) with palette 0x1060, spawn_arg_1 = r14
   (its run of mine_bubble_frames) and handler bubble_column_animate, and plays sound 0x24, as
   bubble_column_init sets one up. */
void spawn_bubble_column(uint32_t spawned, uint32_t free_count, uint32_t column, uint32_t block)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x013AB8u, &spawned, &ignored, &free_count, &column, &block);
	if ((uint8_t)spawned != 0u)
	{
		OBJECT(column)->sprite_palette = 0x1060u;
		OBJECT(column)->spawn_arg_1 = r14;
		OBJECT(column)->handler = bubble_column_animate_ADDRESS;
		sound_id = 0x24u;
		play_sound(sound_id, &ignored, &ignored_2);
	}
}

/* Sets up a bubble column, an object type no stage list spawns: palette 0x1060, sound 0x24; handler
   bubble_column_animate. */
void bubble_column_init(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->handler = bubble_column_animate_ADDRESS;
	sound_id = 0x24u;
	play_sound(sound_id, &ignored, &ignored_2);
}

/* A bubble column: every 3 frames the next of 0x14 frames of mine_bubble_frames from entry spawn_arg_1 *
   0x13 on, looping; queued at its screen_y and never removed. */
void bubble_column_animate(void)
{
	uint32_t frame_base = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	frame_timer = OBJECT(current_object)->bubble_column.frame_timer;
	OBJECT(current_object)->bubble_column.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->bubble_column.frame_timer = 2u;
		frame_index = (uint8_t)OBJECT(current_object)->bubble_column.frame_index;
		frame_base = OBJECT(current_object)->spawn_arg_1;
		frame_base = (uint16_t)(frame_base * 0x13u);
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[(uint16_t)(frame_index + frame_base)];
		OBJECT(current_object)->bubble_column.frame_index++;
		if (OBJECT(current_object)->bubble_column.frame_index > 0x13)
			OBJECT(current_object)->bubble_column.frame_index = 0;
	}
	queue_sprite(frame_base);
}

/* Sets up a mine: sprite 0x8610, half size when spawn_arg_2 is set, a round hitbox of radius 8 and
   height 0x1E, kind 0x24; its pieces: a shadow at the mine's sprite_scale (the + 0x24 just before is
   overwritten) and three bubble parts from frames 0, 1 and 2, the last x-flipped; handler mine_float. */
void mine_init(void)
{
	if (OBJECT(current_object)->spawn_arg_2 != 0u)
		OBJECT(current_object)->sprite_scale = 0x80u;
	OBJECT(current_object)->sprite_frame = 0x8610u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_height = 0x1Eu;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->handler = mine_float_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale += 0x24u;
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->sprite_frame = 0x5D90u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->piece_frame = 1;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->piece_frame = 2;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
}

/* A mine bobbing above the water: speed_z = sine_table[timer], timer + 0x10 a frame; once the nearest
   player is within 0x80 its handler becomes mine_fall. Runs mine_fall every frame. */
void mine_float(void)
{
	face_nearest_player(0u, 0u);
	if (nearest_player_distance < 0x80)
		OBJECT(current_object)->handler = mine_fall_ADDRESS;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->mine.timer += 0x10u;
	OBJECT(current_object)->mine.timer &= 0x3FFu;
	OBJECT(current_object)->speed_z += sine_table[OBJECT(current_object)->mine.timer];
	mine_fall();
}

/* A mine falling (and, run by mine_float, bobbing): moves, removed off screen, falls toward its floor.
   Above the water height spawn_arg_1 it flashes, is drawn at its floor depth and touchable, its shadow
   every other frame; at that height it splashes (spawn_mine_splash), plays sound 0x16, drops 4 and sinks
   (mine_sink). */
void mine_fall(void)
{
	uint8_t sinking = 0, sound_id = 0;
	uint32_t block_offset = 0, bucket = 0, cursor = 0, deleted = 0, height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, slope_height = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &block_offset, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &block_offset, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	fall_to_floor(&height, &ignored_2, &block_offset);
	if (OBJECT(current_object)->z_int < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		sinking = 0u;
		flash_mine_palette(sinking);
		queue_sprite_at_floor();
		add_to_touchable_list(height);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction++;
		odd_frame = OBJECT(current_object)->direction & 1u;
		OBJECT(current_object)->direction = odd_frame;
		if (odd_frame == 0u)
		{
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
		}
		return;
	}
	spawn_mine_splash(height, block_offset, slope_height, cursor);
	OBJECT(current_object)->mine.timer = 0u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->z_int += 4u;
	OBJECT(current_object)->handler = mine_sink_ADDRESS;
	sound_id = 0x16u;
	play_sound(sound_id, &ignored_2, &ignored_3);
	animate_sinking_mine();
}

/* A mine sinking through the water: speed_z - 0x5800 a frame against fall_to_floor's 0x6000, drawn by
   animate_sinking_mine; on the bottom it plays sound 0x20, takes palette 0x1070, switches its shadow off
   (piece_value 1) and explodes (mine_explode). */
void mine_sink(void)
{
	uint8_t sound_id = 0;
	uint32_t height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->speed_z -= 0x5800u;
	fall_to_floor(&height, &ignored_5, &ignored_2);
	if (height != 0u)
	{
		animate_sinking_mine();
		return;
	}
	sound_id = 0x20u;
	play_sound(sound_id, &ignored_5, &ignored_2);
	OBJECT(current_object)->mine.timer = 0u;
	OBJECT(current_object)->mine.frame_index = 0;
	OBJECT(current_object)->sprite_palette = 0x1070u;
	OBJECT(current_object)->handler = mine_explode_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->piece_value = 1u;
	current_object = object_cursor;
	mine_explode();
}

/* A mine exploding on the bottom: steps through underwater_explosion_frames, each held by its
   underwater_explosion_durations count, drawn with draw_mine_with_bubbles; after the table's 0x16 frames
   it plays sound 0x24 and is removed with its pieces (that last step reads one past both tables, never
   shown). */
void mine_explode(void)
{
	uint8_t sound = 0;
	uint32_t frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint8_t next_frame = 0;
	uint16_t timer = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	timer = OBJECT(current_object)->mine.timer;
	OBJECT(current_object)->mine.timer = timer - 1u;
	if (timer != 0u)
	{
		draw_mine_with_bubbles();
		return;
	}
	frame_index = (uint8_t)OBJECT(current_object)->mine.frame_index;
	OBJECT(current_object)->sprite_frame = underwater_explosion_frames[frame_index];
	OBJECT(current_object)->mine.timer = underwater_explosion_durations[frame_index];
	OBJECT(current_object)->mine.frame_index++;
	next_frame = (uint8_t)OBJECT(current_object)->mine.frame_index % 0x17u;
	OBJECT(current_object)->mine.frame_index = next_frame;
	if (next_frame == 0u)
	{
		sound = 0x24u;
		play_sound(sound, &ignored_5, &ignored_2);
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	if (OBJECT(current_object)->mine.frame_index <= 0xF)
	{
		draw_mine_with_bubbles();
		return;
	}
	draw_mine_with_bubbles();
}

/* Draws a sinking mine: flash_mine_palette for a sinking mine, a new frame every frame (mine_frames by
   mine_sink_steps[sink_step], sink_step looping over 8), then draw_mine_with_bubbles. */
void animate_sinking_mine(void)
{
	uint8_t sinking = 0;

	sinking = 1u;
	flash_mine_palette(sinking);
	OBJECT(current_object)->sprite_frame = mine_frames[mine_sink_steps[OBJECT(current_object)->animate_sinking_mine.sink_step]];
	OBJECT(current_object)->animate_sinking_mine.sink_step++;
	OBJECT(current_object)->animate_sinking_mine.sink_step = OBJECT(current_object)->animate_sinking_mine.sink_step % 8u;
	draw_mine_with_bubbles();
}

/* Draws a mine under water: at its floor depth and touchable; its shadow, unless switched off
   (piece_value), every other frame at its floor; the three bubble parts at (0, -0xA), (5, -0x14) and
   (0xA, -0x14) from it, a new frame every other frame, queued at r14 + 1 and r14 - 1: the shadow's
   screen_y on its frames, else whatever earlier code left in r14. */
void draw_mine_with_bubbles(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t bucket = 0, depth = 0, ignored = 0;
	uint16_t shadow_odd_frame = 0, bubble_1_odd_frame = 0, bubble_2_odd_frame = 0, bubble_3_odd_frame = 0;

	add_to_touchable_list(0u);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(current_object)->piece_value == 0u)
	{
		OBJECT(current_object)->direction++;
		shadow_odd_frame = OBJECT(current_object)->direction & 1u;
		OBJECT(current_object)->direction = shadow_odd_frame;
		if (shadow_odd_frame == 0u)
		{
			bucket = place_sprite_at_cursor_floor();
			r14 = (uint16_t)OBJECT(current_object)->screen_y;
			queue_sprite_in_bucket(bucket);
		}
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF6u;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_1_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_1_odd_frame;
	if (bubble_1_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[(uint8_t)OBJECT(current_object)->piece_frame];
		OBJECT(current_object)->piece_frame++;
		if (OBJECT(current_object)->piece_frame > 0x13)
			OBJECT(current_object)->piece_frame = 0;
	}
	r14 = (uint16_t)(r14 + 1u);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 5u;
	offset_y = 0xFFECu;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_2_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_2_odd_frame;
	if (bubble_2_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames_2[(uint8_t)OBJECT(current_object)->piece_frame];
		OBJECT(current_object)->piece_frame++;
		if (OBJECT(current_object)->piece_frame > 0x13)
			OBJECT(current_object)->piece_frame = 0;
	}
	r14 = (uint16_t)(r14 - 2u);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0xAu;
	offset_y = 0xFFECu;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->direction++;
	bubble_3_odd_frame = OBJECT(current_object)->direction & 1u;
	OBJECT(current_object)->direction = bubble_3_odd_frame;
	if (bubble_3_odd_frame != 0u)
	{
		OBJECT(current_object)->sprite_frame = mine_bubble_frames[(uint8_t)OBJECT(current_object)->piece_frame];
		OBJECT(current_object)->piece_frame++;
		if (OBJECT(current_object)->piece_frame > 0x13)
			OBJECT(current_object)->piece_frame = 0;
	}
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
}

/* Flashes a mine: every 5 frames sprite_palette = mine_flash_palettes[mine_flash_steps[flash_step]],
   flash_step looping over 0xC; with sinking set, bit 15 of the palette is flipped and bit 14 set. */
void flash_mine_palette(uint8_t sinking)
{
	uint16_t flash_timer = 0;

	flash_timer = OBJECT(current_object)->flash_mine_palette.flash_timer;
	OBJECT(current_object)->flash_mine_palette.flash_timer = flash_timer - 1u;
	if (flash_timer != 0u)
		return;
	OBJECT(current_object)->flash_mine_palette.flash_timer = 4u;
	OBJECT(current_object)->sprite_palette = mine_flash_palettes[mine_flash_steps[OBJECT(current_object)->flash_mine_palette.flash_step]];
	if ((uint8_t)sinking != 0u)
	{
		OBJECT(current_object)->sprite_palette ^= 0x8000u;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	OBJECT(current_object)->flash_mine_palette.flash_step++;
	OBJECT(current_object)->flash_mine_palette.flash_step = OBJECT(current_object)->flash_mine_palette.flash_step % 0xCu;
}

/* As a mine hits the water: its palette becomes 0x1070, and a two-part splash is spawned at its position
   (projected_y + 1): the water column (palette 0x90A0, handler mine_splash_animate) and the spray,
   sprite 0x8570, palette 0x1080 | 0x4000. */
void spawn_mine_splash(uint32_t spawned, uint32_t free_count, uint32_t splash, uint32_t block)
{
	uint32_t ignored = 0, object = 0;

	OBJECT(current_object)->sprite_palette = 0x1070u;
	spawn_count = 2u;
	spawn_object_with_pieces(0x013F45u, &spawned, &ignored, &free_count, &splash, &block);
	if ((uint8_t)spawned != 0u)
	{
		copy_position_to(splash);
		OBJECT(splash)->projected_y_int++;
		OBJECT(splash)->sprite_palette = 0x90A0u;
		OBJECT(splash)->handler = mine_splash_animate_ADDRESS;
		object = LINKED(splash, OBJECT(splash)->child);
		OBJECT(object)->sprite_frame = 0x8570u;
		OBJECT(object)->sprite_palette = 0x1080u;
		OBJECT(object)->sprite_palette |= 0x4000u;
	}
}

/* A mine's splash: the water column takes the next of mine_splash_frames every 3 frames, at its floor
   depth, and is removed with the spray after 0x1A; the spray, 7 below it, steps through
   mine_splash_spray_frames every 3 frames until 0x14, queued at r14 + 2, r14 being whatever earlier code
   left in it. */
void mine_splash_animate(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t frame_timer = 0, direction = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	if (OBJECT(current_object)->mine_splash.frame_index > 0x19)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	frame_timer = OBJECT(current_object)->mine_splash.frame_timer;
	OBJECT(current_object)->mine_splash.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->mine_splash.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = mine_splash_frames[(uint8_t)OBJECT(current_object)->mine_splash.frame_index];
		OBJECT(current_object)->mine_splash.frame_index++;
	}
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	if (OBJECT(current_object)->piece_frame >= 0x14)
		return;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = mine_splash_spray_frames[(uint8_t)OBJECT(current_object)->piece_frame];
		OBJECT(current_object)->piece_frame++;
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 7u;
	depth = (uint16_t)(r14 + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Sets up a lid trap: a pad (sprite 0x8840) with a 0x26 x 0x26 box hitbox, kind 0x23 (a player touching
   it gets stuck: state 0x2C), and its open lid as its child (sprite 0x8850); handler lid_trap_update. */
void lid_trap_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x8840u;
	OBJECT(current_object)->sprite_palette = 0x10F0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x26u;
	OBJECT(current_object)->hitbox_size_y = 0x26u;
	OBJECT(current_object)->hitbox_offset_x = 0x1A;
	OBJECT(current_object)->hitbox_offset_y = -0x44;
	OBJECT(current_object)->hitbox_height = 0u;
	OBJECT(current_object)->kind = 0x23u;
	OBJECT(current_object)->handler = lid_trap_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x8850u;
	OBJECT(current_object)->sprite_palette = 0x1100u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
}

/* A lid trap, removed off screen. Once touched (flags bit 2) the pad turns kind 5 with sprite 0x8848,
   the lid takes palette 0x1110, sound 0x11 plays and sprung is set; then the lid snaps shut, a frame of
   lid_trap_close_steps every 4 frames. The pad is touchable and queued at screen_y - 0x58, the lid 0x10
   above it. */
void lid_trap_update(void)
{
	uint8_t sound_id = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->kind = 5u;
		OBJECT(current_object)->sprite_frame = 0x8848u;
		OBJECT(current_object)->lid_trap.sprung = 1u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_palette = 0x1110u;
		sound_id = 0x11u;
		play_sound(sound_id, &ignored_5, &ignored_2);
		current_object = object_cursor;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->lid_trap.sprung != 0u && (int16_t)((uint8_t)OBJECT(current_object)->piece_frame | ((uint32_t)OBJECT(current_object)->piece_value << 8u)) < 4)
	{
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 3u;
			OBJECT(current_object)->sprite_frame = lid_trap_lid_frames[lid_trap_close_steps[(uint8_t)OBJECT(current_object)->piece_frame]];
			OBJECT(current_object)->piece_frame++;
		}
	}
	offset_x = 0u;
	offset_y = 0xFFF0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Sets up a sea turtle: its shell (sprite 0x7F28) with a 0x26 x 0x26 box hitbox and kind 0xC, and as its
   pieces its head (sprite 0x7F70) and its shadow (sprite 0x5D88); handler turtle_wait. */
void turtle_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7F28u;
	OBJECT(current_object)->sprite_palette = 0x1040u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x26u;
	OBJECT(current_object)->hitbox_size_y = 0x26u;
	OBJECT(current_object)->hitbox_offset_x = 0x1A;
	OBJECT(current_object)->hitbox_offset_y = -0x44;
	OBJECT(current_object)->hitbox_height = 0u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = turtle_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7F70u;
	OBJECT(current_object)->sprite_palette = 0x1050u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D88u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
}

/* A turtle waiting under the water: drawn at screen_y - 0x20 and touchable until touched (flags bit 2),
   then stroke_step 1 and turtle_surface. The head and shadow code after the draw runs only while
   stroke_step is 4, which it never is here. */
void turtle_wait(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint8_t head_step = 0;
	uint16_t direction = 0;

	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->turtle.stroke_timer = 0u;
		OBJECT(current_object)->turtle.stroke_step = 1u;
		OBJECT(current_object)->handler = turtle_surface_ADDRESS;
		turtle_surface();
		return;
	}
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->turtle.stroke_step != 4u)
		return;
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		offset_x = 0xBu;
		offset_y = 0xFFEDu;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x1Fu);
		queue_sprite_at_depth(&ignored, &depth);
	}
	else
	{
		OBJECT(current_object)->direction = 4u;
		OBJECT(current_object)->sprite_frame = turtle_head_frames[turtle_head_steps[((uint8_t)OBJECT(current_object)->piece_frame | ((uint32_t)OBJECT(current_object)->piece_value << 8u))]];
		OBJECT(current_object)->piece_frame++;
		head_step = OBJECT(current_object)->piece_frame & 0xFu;
		OBJECT(current_object)->piece_frame = head_step;
		if (head_step != 0u)
		{
			offset_x = 0xBu;
			offset_y = 0xFFEDu;
			place_sprite_near_cursor(offset_x, offset_y);
			depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x1Fu);
			queue_sprite_at_depth(&ignored, &depth);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			offset_x = 0u;
			offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
			place_sprite_near_cursor(offset_x, offset_y);
			bucket = 0xFFu;
			queue_sprite_in_bucket(bucket);
			return;
		}
		OBJECT(current_object)->direction = 0x1E0u;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* A touched turtle rising: speed_z alternates between -0x8000 and 0 while it swims (turtle_swim); once
   0x20 above its floor it stops rising and drops rings (turtle_drop_rings). */
void turtle_surface(void)
{
	OBJECT(current_object)->speed_z = 0u - ((OBJECT(current_object)->speed_z + 0x8000u) & 0x3FFFFu);
	if ((int16_t)(OBJECT(current_object)->floor_z_int - 0x20u) < OBJECT(current_object)->z_int)
	{
		turtle_swim();
		return;
	}
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->handler = turtle_drop_rings_ADDRESS;
	turtle_drop_rings();
}

/* The surfaced turtle drops rings while it swims: one (drop_rings, angle 0x154, flip 2) whenever its
   countdown spawn_arg_1 runs out, then 0x30 again; spawn_arg_2 counts them, and past 4 it only swims
   (turtle_swim). */
void turtle_drop_rings(void)
{
	uint32_t allocated = 0, random_value = 0, ring = 0;
	uint16_t spawn_arg_1 = 0;

	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->spawn_arg_1 = spawn_arg_1 - 1u;
	if (spawn_arg_1 != 0u)
	{
		turtle_swim();
		return;
	}
	OBJECT(current_object)->spawn_arg_1 = 0x30u;
	OBJECT(current_object)->spawn_arg_2++;
	if ((int16_t)OBJECT(current_object)->spawn_arg_2 > 4)
		OBJECT(current_object)->handler = turtle_swim_ADDRESS;
	released_ring_flip = 2u;
	released_rings_random = 0u;
	released_ring_angle = 0x154u;
	rings_to_release = 1u;
	drop_rings(0u, &allocated, &random_value, &ring);
	turtle_swim();
}

/* A swimming turtle: every 3 frames a stroke step (turtle_swim_frames, turtle_swim_speeds), with a
   0x3C-frame glide after the last; it moves along direction 0x14C at speed + 0xE0, speed losing 3 a
   frame, and is removed off screen. Its head animates beside it; over a floor its shadow is its own
   frame, shrunk by its height. Drawn at its floor, touchable. */
void turtle_swim(void)
{
	uint16_t offset_x = 0, offset_y = 0, shadow_offset_y = 0;
	uint32_t bucket = 0, deleted = 0, direction = 0, height_above_floor = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, speed = 0, speed_x = 0, speed_y = 0, stroke_step = 0;
	uint8_t wrapped_stroke_step = 0, wrapped_head_step = 0;
	uint16_t stroke_timer = 0, direction_2 = 0;

	stroke_timer = OBJECT(current_object)->turtle.stroke_timer;
	OBJECT(current_object)->turtle.stroke_timer = stroke_timer - 1u;
	if (stroke_timer == 0u)
	{
		OBJECT(current_object)->turtle.stroke_timer = 2u;
		stroke_step = OBJECT(current_object)->turtle.stroke_step;
		OBJECT(current_object)->sprite_frame = turtle_swim_frames[stroke_step];
		OBJECT(current_object)->speed = turtle_swim_speeds[stroke_step];
		OBJECT(current_object)->turtle.stroke_step++;
		wrapped_stroke_step = OBJECT(current_object)->turtle.stroke_step & 0xFu;
		OBJECT(current_object)->turtle.stroke_step = wrapped_stroke_step;
		if (wrapped_stroke_step == 0u)
			OBJECT(current_object)->turtle.stroke_timer = 0x3Cu;
	}
	OBJECT(current_object)->speed -= 3u;
	direction = 0x14Cu;
	speed = OBJECT(current_object)->speed + 0xE0u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction_2 = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction_2 - 1u;
	if (direction_2 == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		OBJECT(current_object)->sprite_frame = turtle_head_frames[turtle_head_steps[((uint8_t)OBJECT(current_object)->piece_frame | ((uint32_t)OBJECT(current_object)->piece_value << 8u))]];
		OBJECT(current_object)->piece_frame++;
		wrapped_head_step = OBJECT(current_object)->piece_frame & 0xFu;
		OBJECT(current_object)->piece_frame = wrapped_head_step;
		if (wrapped_head_step == 0u)
			OBJECT(current_object)->direction = 0x78u;
	}
	offset_x = 0xBu;
	offset_y = 0xFFEDu;
	place_sprite_near_cursor(offset_x, offset_y);
	height_above_floor = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	r14 = height_above_floor;
	height_above_floor = (uint16_t)(height_above_floor + OBJECT(current_object)->screen_y + 0x13u + 1u);
	queue_sprite_at_depth(&bucket, &height_above_floor);
	if (OBJECT(object_cursor)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0u;
		shadow_offset_y = (uint16_t)r14;
		place_sprite_near_cursor(offset_x, shadow_offset_y);
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_scale -= shadow_offset_y;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
	}
	current_object = object_cursor;
	queue_sprite_at_floor();
	add_to_touchable_list(bucket);
}

/* Sets up an eel at its den: the anchor ball (kind 0x24, speed 0x480 its stretch, stretch_step 0x10,
   direction 0x34A, 0x14A with facing) with two parts and a shadow, then piece_count four-part pieces
   after it, wave amplitudes growing by wave_step, wave angles 0x30 apart (kept in explode_delay): the
   head last (eel_head_move), the rest eel_segment_move. */
void eel_init(void)
{
	uint16_t explode_delay_step = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0, segment_amplitude = 0, segments_left = 0;

	OBJECT(current_object)->handler = eel_anchor_update_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA898u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_height = 0xAu;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->eel_init.stretch_step = 0x10;
	OBJECT(current_object)->speed = 0x480;
	OBJECT(current_object)->direction = 0x34Au;
	if (OBJECT(current_object)->eel_init.facing != 0u)
		OBJECT(current_object)->direction = 0x14Au;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x12E0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA898u;
	OBJECT(current_object)->sprite_palette = 0x7A0u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	r14 = 0x30u;
	explode_delay_step = 0x30u;
	segments_left = OBJECT(object_cursor)->eel_init.piece_count;
	segment_amplitude = OBJECT(object_cursor)->eel_init.wave_step;
	current_player = current_object;
	spawn_count = 4u;
	spawn_object_with_pieces(0x014A32u, &ignored, &ignored_2, &free_count, &object, &block);
	for (;;)
	{
		OBJECT(current_player)->child = object;
		copy_position_to(object);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA898u;
		OBJECT(object)->sprite_palette = 0x1210u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		OBJECT(object)->eel_segment.explode_delay = r14;
		OBJECT(object)->hitbox_size_x = 0x10u;
		OBJECT(object)->hitbox_height = 0x10u;
		OBJECT(object)->kind = 0x24u;
		OBJECT(object)->spawn_arg_1 = segment_amplitude;
		OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->eel_segment.height_offset = 0x4000u * segment_amplitude;
		OBJECT(object)->handler = eel_segment_move_ADDRESS;
		object_2 = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x1200u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x12E0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		OBJECT(object)->piece_frame = (uint8_t)segment_amplitude % 0x1Fu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA898u;
		OBJECT(object)->sprite_palette = 0x7A0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		current_player = object;
		r14 = (uint16_t)(r14 + explode_delay_step);
		segment_amplitude = (uint16_t)(segment_amplitude + OBJECT(current_object)->eel_init.wave_step);
		segments_left--;
		if (segments_left == 0u)
			break;
		spawn_count = 4u;
		spawn_object_with_pieces(0x014A3Fu, &ignored, &ignored_2, &free_count, &object, &block);
	}
	OBJECT(object_2)->sprite_frame = 0xAA38u;
	OBJECT(object_2)->sprite_palette = 0x1210u;
	OBJECT(object_2)->sprite_palette |= 0x4000u;
	OBJECT(object_2)->handler = eel_head_move_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_flags = 0u;
	OBJECT(object_2)->sprite_frame = 0xAA38u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_palette |= 0x4000u;
}

/* An eel's anchor at its den: once the den has set its bit (den_number) of stage_trigger_bits it waits
   for the pieces to explode (eel_anchor_wait_explode). Otherwise removed off screen, drawn at its floor
   and touchable, and the eel stretches out and back (eel_anchor_stretch), resting pause_timer frames at
   each end. */
void eel_anchor_update(void)
{
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t pause_timer = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_anchor.den_number))) != 0u)
	{
		OBJECT(current_object)->handler = eel_anchor_wait_explode_ADDRESS;
		eel_anchor_wait_explode();
		return;
	}
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	queue_sprite_at_floor();
	add_to_touchable_list(deleted);
	if (OBJECT(current_object)->eel_anchor.pausing == 0u)
	{
		eel_anchor_stretch();
		return;
	}
	pause_timer = OBJECT(current_object)->eel_anchor.pause_timer;
	OBJECT(current_object)->eel_anchor.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		eel_anchor_draw_shadow();
		return;
	}
	OBJECT(current_object)->eel_anchor.pausing = 0u;
	eel_anchor_stretch();
}

/* An eel whose den has collapsed: removed with all its pieces once piece_count, the pieces still to
   finish exploding, reaches 0. Its chaser test (facing 0 only: chaser_object within 0x10 in y) sets the
   den's bit of stage_trigger_bits, which is already set when this runs. */
void eel_anchor_wait_explode(void)
{
	uint32_t distance_y = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if (OBJECT(current_object)->eel_anchor.piece_count <= 0)
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->eel_anchor.facing == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int32_t)distance_y <= 0x10)
			stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->eel_anchor.den_number));
	}
}

/* Stretches the eel: the anchor's speed changes by stretch_step every frame, turning back in (-0x18)
   past 0x1111 and out again (+0x28) under 0x480, with a 0x3C-frame pause at each end. Piece n down the
   chain gets the anchor's direction, n times its speed and stretch_step (as var_81); then the anchor's
   shadow is drawn. */
void eel_anchor_stretch(void)
{
	uint32_t segment_speed = 0, speed_step = 0, stretch_step = 0;

	stretch_step = OBJECT(current_object)->eel_anchor.stretch_step;
	OBJECT(current_object)->speed += stretch_step;
	if ((int32_t)stretch_step >= 0)
	{
		if (OBJECT(current_object)->speed > 0x1111)
		{
			OBJECT(current_object)->eel_anchor.pause_timer = 0x3Cu;
			OBJECT(current_object)->eel_anchor.pausing = 1u;
			OBJECT(current_object)->eel_anchor.stretch_step = -0x18;
		}
	}
	else if (OBJECT(current_object)->speed < 0x480)
	{
		OBJECT(current_object)->eel_anchor.pause_timer = 0x3Cu;
		OBJECT(current_object)->eel_anchor.pausing = 1u;
		OBJECT(current_object)->eel_anchor.stretch_step = 0x28;
	}
	segment_speed = OBJECT(current_object)->speed;
	speed_step = segment_speed;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	for (;;)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction = OBJECT(object_cursor)->direction;
		OBJECT(current_object)->speed = segment_speed;
		OBJECT(current_object)->eel_piece.stretch_step = OBJECT(object_cursor)->eel_anchor.stretch_step;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if (OBJECT(current_object)->child == 0xFFFFu)
			break;
		segment_speed += speed_step;
	}
	eel_anchor_draw_shadow();
}

/* Draws the eel anchor's shadow, its third part, at the anchor's floor in bucket 0xFF. */
void eel_anchor_draw_shadow(void)
{
	uint32_t bucket = 0;

	current_object = object_cursor;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

/* Unused: an empty function, a lone return the original leaves after eel_anchor_wait_explode. */
void eel_anchor_wait_explode_spare_return(void)
{
}

/* An eel's head, placed along the eel (place_eel_piece) and drawn at its floor with its shadow, but not
   touchable: every 4 frames its next frame (eel_head_frames, or with facing set the extend or retract
   frames as stretch_step goes out or in), resting 0x3C frames after each run. Once its den's bit is set
   it explodes after (speed / the anchor's speed) * 0xB frames. */
void eel_head_move(void)
{
	uint16_t explosion_delay = 0, head_offset_x = 0, head_offset_y = 0;
	uint32_t bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t frame_timer = 0, frame_timer_2 = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_head.den_number))) == 0u)
	{
		place_eel_piece();
		if (OBJECT(current_object)->eel_head.facing == 0u)
		{
			head_offset_x = 0xFFF4u;
			head_offset_y = 3u;
			frame_timer = OBJECT(current_object)->eel_head.frame_timer;
			OBJECT(current_object)->eel_head.frame_timer = frame_timer - 1u;
			if (frame_timer == 0u)
			{
				OBJECT(current_object)->eel_head.frame_timer = 3u;
				OBJECT(current_object)->sprite_frame = eel_head_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
				OBJECT(current_object)->eel_head.frame_index++;
				if (OBJECT(current_object)->eel_head.frame_index > 2)
				{
					OBJECT(current_object)->eel_head.frame_index = 0;
					OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
				}
			}
			OBJECT(current_object)->projected_x += head_offset_x;
			OBJECT(current_object)->projected_y_int += head_offset_y;
			move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
			queue_sprite_at_floor();
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
			return;
		}
		head_offset_x = 0xBu;
		head_offset_y = 0u;
		frame_timer_2 = OBJECT(current_object)->eel_head.frame_timer;
		OBJECT(current_object)->eel_head.frame_timer = frame_timer_2 - 1u;
		if (frame_timer_2 != 0u)
		{
			OBJECT(current_object)->projected_x += head_offset_x;
			OBJECT(current_object)->projected_y_int += head_offset_y;
			move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
			queue_sprite_at_floor();
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
			return;
		}
		OBJECT(current_object)->eel_head.frame_timer = 3u;
		if (OBJECT(current_object)->eel_head.stretch_step < 0)
		{
			OBJECT(current_object)->sprite_frame = eel_head_retract_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
			OBJECT(current_object)->eel_head.frame_index++;
			if (OBJECT(current_object)->eel_head.frame_index <= 3)
			{
				OBJECT(current_object)->projected_x += head_offset_x;
				OBJECT(current_object)->projected_y_int += head_offset_y;
				move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
				queue_sprite_at_floor();
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				bucket = place_sprite_at_cursor_floor();
				queue_sprite_in_bucket(bucket);
				return;
			}
			OBJECT(current_object)->eel_head.frame_index = 0;
			OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
			OBJECT(current_object)->projected_x += head_offset_x;
			OBJECT(current_object)->projected_y_int += head_offset_y;
			move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
			queue_sprite_at_floor();
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
			return;
		}
		OBJECT(current_object)->eel_head.frame_index = (uint8_t)OBJECT(current_object)->eel_head.frame_index % 3u;
		OBJECT(current_object)->sprite_frame = eel_head_extend_frames[(uint8_t)OBJECT(current_object)->eel_head.frame_index];
		OBJECT(current_object)->eel_head.frame_index++;
		if (OBJECT(current_object)->eel_head.frame_index <= 2)
		{
			OBJECT(current_object)->projected_x += head_offset_x;
			OBJECT(current_object)->projected_y_int += head_offset_y;
			move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
			queue_sprite_at_floor();
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			bucket = place_sprite_at_cursor_floor();
			queue_sprite_in_bucket(bucket);
			return;
		}
		OBJECT(current_object)->eel_head.frame_index = 0;
		OBJECT(current_object)->eel_head.frame_timer = 0x3Cu;
		OBJECT(current_object)->projected_x += head_offset_x;
		OBJECT(current_object)->projected_y_int += head_offset_y;
		move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
		queue_sprite_at_floor();
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	explosion_delay = (uint16_t)(divide_s32((uint32_t)OBJECT(object_cursor)->speed, (uint32_t)OBJECT(current_object)->speed) * 0xBu);
	current_object = object_cursor;
	OBJECT(current_object)->eel_head.explode_delay = explosion_delay;
	OBJECT(current_object)->handler = eel_head_explode_delay_ADDRESS;
	eel_head_explode_delay();
}

/* A body piece of an eel: placed along the eel (place_eel_piece), drawn at its floor and touchable, its
   shadow (its fourth part) at its floor; parts 2 and 3 are not drawn. Once its den's bit is set it
   explodes after (speed / the anchor's speed) * 0xB frames (eel_segment_explode_delay). */
void eel_segment_move(void)
{
	uint16_t explosion_delay = 0;
	uint32_t bucket = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->eel_segment.den_number))) == 0u)
	{
		place_eel_piece();
		move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
		depth = queue_sprite_at_floor();
		r14 = depth;
		add_to_touchable_list(ignored);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	explosion_delay = (uint16_t)(divide_s32((uint32_t)OBJECT(object_cursor)->speed, (uint32_t)OBJECT(current_object)->speed) * 0xBu);
	current_object = object_cursor;
	OBJECT(current_object)->eel_segment.explode_delay = explosion_delay;
	OBJECT(current_object)->handler = eel_segment_explode_delay_ADDRESS;
	eel_segment_explode_delay();
}

/* Unused: an ending for eel_segment_move that also draws a piece's parts 2 and 3: every 3 frames each
   takes its next frame (eel_part_frames_2 by eel_part_steps, eel_part_frames), both at the piece's
   screen position and floor depth + 1; then the shadow at its floor. The live code skips parts 2 and 3,
   so they are never drawn. */
void draw_eel_piece_parts(void)
{
	uint32_t bucket = 0, depth = 0, floor_depth = 0, ignored = 0;
	uint16_t part_2_timer = 0, part_3_timer = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	part_2_timer = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = part_2_timer - 1u;
	if (part_2_timer == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = eel_part_frames_2[eel_part_steps[(uint8_t)OBJECT(current_object)->piece_frame]];
		OBJECT(current_object)->piece_frame++;
		OBJECT(current_object)->piece_frame = (uint8_t)OBJECT(current_object)->piece_frame % 0x18u;
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	floor_depth = (uint16_t)(((uint16_t)OBJECT(object_cursor)->projected_y_int | ((uint32_t)OBJECT(object_cursor)->flags_low << 16u)) + ((uint16_t)OBJECT(object_cursor)->floor_z_int | ((uint32_t)OBJECT(object_cursor)->previous_floor_z_fraction << 16u)) - view_y + 1u);
	r14 = floor_depth;
	queue_sprite_at_depth(&ignored, &floor_depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	part_3_timer = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = part_3_timer - 1u;
	if (part_3_timer == 0u)
	{
		OBJECT(current_object)->direction = 2u;
		OBJECT(current_object)->sprite_frame = eel_part_frames[(uint8_t)OBJECT(current_object)->piece_frame];
		OBJECT(current_object)->piece_frame++;
		OBJECT(current_object)->piece_frame = (uint8_t)OBJECT(current_object)->piece_frame % 0x1Fu;
	}
	copy_screen_position_from_cursor();
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

/* Puts an eel piece on its anchor (projected position, z and floor_z, z raised by height_offset) with
   speed_x/speed_y from direction and speed, so the next move carries it out along the eel; wave_angle
   steps by 8 a frame and sine_table[wave_angle] x wave_amplitude is added to z and, twice, to speed_y. */
void place_eel_piece(void)
{
	uint32_t direction = 0, speed = 0, wave_offset = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z -= OBJECT(object_cursor)->place_eel_piece.height_offset;
	current_object = object_cursor;
	direction = OBJECT(current_object)->direction;
	speed = OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * speed) >> 6;
	OBJECT(current_object)->speed_y = 0u - ((int32_t)(cosine_table[direction] * speed) >> 6);
	OBJECT(current_object)->place_eel_piece.wave_angle += 8u;
	OBJECT(current_object)->place_eel_piece.wave_angle &= 0x3FFu;
	wave_offset = (uint32_t)sine_table[OBJECT(current_object)->place_eel_piece.wave_angle] * OBJECT(current_object)->place_eel_piece.wave_amplitude;
	OBJECT(current_object)->z += wave_offset;
	OBJECT(current_object)->speed_y += wave_offset * 2u;
}

/* An eel's head waiting its turn to explode: counts explode_delay down, still drawn at its floor with
   its shadow; then eel_piece_start_explosion. */
void eel_head_explode_delay(void)
{
	uint32_t bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t explode_delay = 0;

	explode_delay = OBJECT(current_object)->eel_head.explode_delay;
	OBJECT(current_object)->eel_head.explode_delay = explode_delay - 1u;
	if (explode_delay == 0u)
	{
		eel_piece_start_explosion();
		return;
	}
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

/* A body piece of an eel waiting its turn to explode: counts explode_delay down, still drawn at its
   floor with its shadow; then eel_piece_start_explosion. */
void eel_segment_explode_delay(void)
{
	uint32_t bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t explode_delay = 0;

	explode_delay = OBJECT(current_object)->eel_segment.explode_delay;
	OBJECT(current_object)->eel_segment.explode_delay = explode_delay - 1u;
	if (explode_delay == 0u)
	{
		eel_piece_start_explosion();
		return;
	}
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

/* Starts an eel piece's explosion: 0x18 lower (projected_y), palette 0x1070, sprite_flags 4, from frame
   2 of underwater_explosion_frames; sound 0x24, handler eel_piece_explode. */
void eel_piece_start_explosion(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->projected_y_int += 0x18u;
	OBJECT(current_object)->eel_piece.frame_timer = 0u;
	OBJECT(current_object)->eel_piece.frame_index = 2;
	OBJECT(current_object)->sprite_palette = 0x1070u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->handler = eel_piece_explode_ADDRESS;
	sound_id = 0x24u;
	play_sound(sound_id, &ignored, &ignored_2);
	eel_piece_explode();
}

/* An exploding eel piece: the next frame of underwater_explosion_frames every 3 frames, from frame 2;
   after frame 0x15 it takes one off its anchor's piece count (spawn_arg_2_low), plays sound 0x24 and
   stays as eel_piece_gone. Queued at screen_y - 0x10. */
void eel_piece_explode(void)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_piece.frame_timer;
	OBJECT(current_object)->eel_piece.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->eel_piece.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = underwater_explosion_frames[(uint8_t)OBJECT(current_object)->eel_piece.frame_index];
		OBJECT(current_object)->eel_piece.frame_index++;
		if (OBJECT(current_object)->eel_piece.frame_index > 0x15)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			OBJECT(current_object)->spawn_arg_2_low--;
			current_object = object_cursor;
			OBJECT(current_object)->handler = eel_piece_gone_ADDRESS;
			sound_id = 0x24u;
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
	unproject_object(&ignored_3, &ignored_2, &ignored_4, &ignored_5);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored_3, &depth);
	eel_piece_gone();
}

/* An exploded eel piece: an empty function, so it is no longer drawn; its anchor removes it with the
   rest of the eel (eel_anchor_wait_explode). */
void eel_piece_gone(void)
{
}

/* Unused: spawns one object (spawn_object_with_pieces) and sets it up as an eel den (eel_den_init, which
   puts current_object back on object_cursor). */
void spawn_eel_den(uint32_t spawned, uint32_t free_count, uint32_t den, uint32_t block)
{
	uint32_t ignored = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x0156FFu, &spawned, &ignored, &free_count, &den, &block);
	if ((uint8_t)spawned == 0u)
	{
		current_object = object_cursor;
		return;
	}
	current_object = den;
	eel_den_init();
}

/* Sets up an eel den: a stone arch (sprite 0xAC70) with a 0x47 x 0x42 box hitbox 0x28 high, kind 0xC and
   health 1; handler eel_den_wait. It ends with current_object back on object_cursor, which matters only
   when spawn_eel_den runs it. */
void eel_den_init(void)
{
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x47u;
	OBJECT(current_object)->hitbox_size_y = 0x42u;
	OBJECT(current_object)->hitbox_offset_x = -3;
	OBJECT(current_object)->hitbox_offset_y = -0x43;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->sprite_frame = 0xAC70u;
	OBJECT(current_object)->sprite_palette = 0x1300u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->health = 1;
	OBJECT(current_object)->handler = eel_den_wait_ADDRESS;
	current_object = object_cursor;
}

/* An eel den waiting: it collapses when chaser_object comes within 0x35 in y, when it is hit with no
   health left, or when its bit (spawn_arg_1) of stage_trigger_bits is already set: it sets that bit,
   which makes its eel explode, drops 3 rings from 0x18 up and crumbles (eel_den_collapse). Else drawn
   (eel_den_draw). */
void eel_den_wait(void)
{
	uint32_t allocated = 0, distance_y = 0, ignored = 0, random_value = 0, ring = 0;

	if ((stage_trigger_bits & shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1))) == 0u)
	{
		r14 = LINKED(WORK_RAM, chaser_object);
		distance_xy_to_object(&ignored, &distance_y);
		if ((int16_t)distance_y > 0x35 && ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(current_object)->health > 0))
		{
			eel_den_draw();
			return;
		}
	}
	stage_trigger_bits |= shl32(1u, (int)(OBJECT(current_object)->spawn_arg_1));
	OBJECT(current_object)->handler = eel_den_collapse_ADDRESS;
	OBJECT(current_object)->projected_y_int -= 0x18u;
	released_ring_angle = 0x144u;
	rings_to_release = 3u;
	released_ring_flip = 2u;
	released_rings_random = 1u;
	drop_rings(0u, &allocated, &random_value, &ring);
	OBJECT(current_object)->projected_y_int += 0x18u;
	eel_den_collapse();
}

/* A collapsing eel den: the next frame of eel_den_collapse_frames every 3 frames, then (after frame
   0x2C) only eel_den_draw; drawn every frame. */
void eel_den_collapse(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->eel_den.frame_timer;
	OBJECT(current_object)->eel_den.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->eel_den.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = eel_den_collapse_frames[(uint8_t)OBJECT(current_object)->eel_den.frame_index];
		OBJECT(current_object)->eel_den.frame_index++;
		if (OBJECT(current_object)->eel_den.frame_index > 0x2C)
		{
			OBJECT(current_object)->handler = eel_den_draw_ADDRESS;
			eel_den_draw();
			return;
		}
	}
	eel_den_draw();
}

/* Draws an eel den: placed from its map position (unproject_object), removed off screen, else queued at
   screen_y - 0x18 and touchable. */
void eel_den_draw(void)
{
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 1u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
		queue_sprite_at_depth(&ignored, &depth);
		add_to_touchable_list(ignored);
	}
}

/* Sets up a raft spawner from river_raft_spawners[sprite_frame] (the spawn record's index): x, y, floor
   height, end_position, then its raft list (entry, list_start); last_raft_y = its y, last_raft_speed 0;
   handler river_raft_spawner_update. */
void river_raft_spawner_init(void)
{
	uint32_t record = 0;

	record = river_raft_spawners[OBJECT(current_object)->sprite_frame] + river_raft_spawners_ADDRESS;
	OBJECT(current_object)->x_int = OBJECT_START_PLACE(record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = OBJECT_START_PLACE(record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = OBJECT_START_PLACE(record - 0x4u)->floor_z;
	OBJECT(current_object)->floor_z_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->river_raft_spawner.end_position = OBJECT_START_PLACE(record - 0x6u)->end_position;
	record += 2u;
	OBJECT(current_object)->river_raft_spawner.entry = record;
	OBJECT(current_object)->river_raft_spawner.list_start = record;
	OBJECT(current_object)->river_raft_spawner.last_raft_y = OBJECT(current_object)->y;
	OBJECT(current_object)->river_raft_spawner.last_raft_speed = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = river_raft_spawner_update_ADDRESS;
}

/* A raft spawner, removed at end_position (from scroll_position 0x9C00 it sets goal_unlocked). When the
   last raft it launched (last_raft_y) has drifted down to it and the waterfall (group_leaders[0]) is
   downstream it launches its list (per raft: distance behind, speed, floor block; with foam) up to
   0x8000, which loops, or 0xFFFF. It moves upstream with the camera. */
void river_raft_spawner_update(void)
{
	uint32_t floor_block_record = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0, raft_entry = 0;

	if (OBJECT(current_object)->river_raft_spawner.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (scroll_position >= 0x9C00u)
		goal_unlocked = 0xFFu;
	OBJECT(current_object)->river_raft_spawner.last_raft_y += OBJECT(current_object)->river_raft_spawner.last_raft_speed >> 8;
	if (OBJECT(current_object)->river_raft_spawner.last_raft_y_int < OBJECT(current_object)->y_int || OBJECT(LINKED(current_object, group_leaders[0]))->y_int < OBJECT(current_object)->y_int)
	{
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->screen_x >= -0x10)
		{
			compute_camera_ground_speed(0u);
			OBJECT(current_object)->y -= camera_ground_speed >> 7;
		}
		return;
	}
	while (RIVER_RAFT_ENTRY(OBJECT(current_object)->river_raft_spawner.entry)->distance >= 0)
	{
		if (free_object_stack < 2)
		{
			project_object(&ignored, &ignored_2);
			if (OBJECT(current_object)->screen_x >= -0x10)
			{
				compute_camera_ground_speed(0u);
				OBJECT(current_object)->y -= camera_ground_speed >> 7;
			}
			return;
		}
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x100Au;
		OBJECT(object)->sprite_frame = 0x8DF0u;
		OBJECT(object)->sprite_palette = 0xB1A0u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x88u;
		OBJECT(object)->direction = 0u;
		OBJECT(object)->river_raft_update.state = 0u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = -0x10;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = river_raft_update_ADDRESS;
		raft_entry = OBJECT(current_object)->river_raft_spawner.entry;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->y -= (uint32_t)(uint16_t)RIVER_RAFT_ENTRY(raft_entry)->distance << 16;
		raft_entry += 2u;
		OBJECT(object)->speed_int = RIVER_RAFT_ENTRY(raft_entry - 0x2u)->speed;
		OBJECT(object)->speed_fraction = 0u;
		raft_entry += 2u;
		OBJECT(object)->floor_block = RIVER_RAFT_ENTRY(raft_entry - 0x4u)->floor_block;
		OBJECT(current_object)->river_raft_spawner.entry = raft_entry + 4u;
		floor_block_record = floor_blocks + 2u + OBJECT(object)->floor_block;
		OBJECT(object)->floor_z_int = FLOOR_BLOCK(floor_block_record)->height;
		OBJECT(object)->floor_z_fraction = 0u;
		OBJECT(object)->z = OBJECT(object)->floor_z;
		OBJECT(object)->x = FLOOR_BLOCK(floor_block_record)->x_min;
		OBJECT(object)->x -= 0x10000u;
		OBJECT(current_object)->river_raft_spawner.last_raft_y = OBJECT(object)->y;
		OBJECT(current_object)->river_raft_spawner.last_raft_speed = OBJECT(object)->speed;
		object_2 = object;
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = OBJECT(object_2)->sprite_flags;
		OBJECT(object)->sprite_offset_x = OBJECT(object_2)->sprite_offset_x;
		OBJECT(object)->sprite_offset_y = OBJECT(object_2)->sprite_offset_y;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->river_raft_foam.unused_state = 0u;
		OBJECT(object)->sprite_palette = 0xB190u;
		OBJECT(object)->handler = river_raft_foam_update_ADDRESS;
		OBJECT(object)->animation = river_raft_foam_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->parent = object_2;
		OBJECT(object_2)->child = object;
	}
	if ((uint16_t)RIVER_RAFT_ENTRY(OBJECT(current_object)->river_raft_spawner.entry)->distance != 0x8000u)
	{
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->screen_x >= -0x10)
		{
			compute_camera_ground_speed(0u);
			OBJECT(current_object)->y -= camera_ground_speed >> 7;
		}
		return;
	}
	OBJECT(current_object)->river_raft_spawner.entry = OBJECT(current_object)->river_raft_spawner.list_start;
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->screen_x >= -0x10)
	{
		compute_camera_ground_speed(0u);
		OBJECT(current_object)->y -= camera_ground_speed >> 7;
	}
}

/* A raft drifting down the river (+y by speed_y) on its floor, 2 lower while carrying something (flags
   bit 3), a platform with its foam; removed off screen. When the waterfall (group_leaders[0]) reaches
   its far end, then its middle, it drops the foam and tips over the edge (river_raft_fall_animation,
   river_raft_fall_palettes), falling till screen y 0xE8. */
void river_raft_update(void)
{
	uint16_t draw_depth = 0;
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->river_raft_update.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->river_raft_update.state++;
		OBJECT(current_object)->speed_y = (uint32_t)OBJECT(current_object)->speed >> 8;
		OBJECT(current_object)->speed_x = 0;
		project_object(&ignored, &ignored_2);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z_int = OBJECT(current_object)->floor_z_int;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->z_int += 2u;
		project_object(&ignored, &ignored_2);
		add_to_platform_list();
		if (OBJECT(LINKED(current_object, group_leaders[0]))->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y))
			OBJECT(current_object)->river_raft_update.state++;
		draw_depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u);
		if ((int16_t)draw_depth <= -0x10)
			draw_depth = 0xFFF1u;
		OBJECT(current_object)->draw_depth = draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = OBJECT(object_cursor)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->z_int = OBJECT(current_object)->floor_z_int;
		if ((OBJECT(current_object)->flags & 8u) != 0u)
			OBJECT(current_object)->z_int += 2u;
		add_to_platform_list();
		if (OBJECT(LINKED(current_object, group_leaders[0]))->y_int <= (int16_t)(OBJECT(current_object)->y_int + (OBJECT(current_object)->hitbox_size_y >> 1)))
		{
			OBJECT(current_object)->river_raft_update.state++;
			OBJECT(current_object)->animation = river_raft_fall_animation_ADDRESS;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[0];
			OBJECT(current_object)->sprite_flags = 0x1000u;
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0;
			OBJECT(current_object)->x_int += 0x16u;
			OBJECT(current_object)->y_int += 0x44u;
			OBJECT(current_object)->speed_z = 0;
			object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object)->flags |= 0x400u;
		}
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((OBJECT(current_object)->flags & 0x400u) == 0u)
		{
			depth = OBJECT(object_cursor)->draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
		}
		current_object = object_cursor;
		depth = OBJECT(current_object)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if (OBJECT(current_object)->screen_y >= 0xE8)
		{
			delete_current_object();
			return;
		}
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&ignored_2, &ignored_3, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		OBJECT(current_object)->sprite_palette = river_raft_fall_palettes[OBJECT(current_object)->animation_frame];
		project_object(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x8C340u + OBJECT(current_object)->river_raft_update.state * 3u);
	}
}

/* A raft's foam: sits at its raft's (parent's) screen position and animates (river_raft_foam_animation);
   the raft queues it. Removed once flags bit 10 is set, which the raft does when it leaves the screen or
   tips over. */
void river_raft_foam_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		animate(&ignored, &ignored_2, &ignored_3);
		return;
	}
	delete_current_object();
}

/* Unused: sets up sprite 0x7058 (palette 0x5050) standing on its floor; given a free object and two
   second-pool blocks it keeps a copy of its whole block in palette bank 0x8000 (copy_object) and chains
   two clip entries (sprite_flags 0x5000) as its child and grandchild, as icicle_buried_part_draw does;
   handler clipped_copy_sprite_update. */
void clipped_copy_sprite_start(uint32_t copy)
{
	uint32_t clips_left = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, object_2 = 0, object_3 = 0;

	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x7058u;
	OBJECT(current_object)->sprite_palette = 0x5050u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &copy);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
	if (free_object_stack <= 0 || free_object_stack_2[0] < 2)
		return;
	take_free_object(&ignored, &copy);
	move_halfwords_backward(copy, current_object, 0x50u);
	OBJECT(copy)->sprite_palette |= 0x8000u;
	OBJECT(copy)->sprite_palette &= 0x9FFFu;
	OBJECT(current_object)->clipped_copy_sprite.copy_object = copy;
	clips_left = 2u;
	object = current_object;
	do
	{
		object_2 = object;
		allocate_object_2(&ignored, &ignored_3, &object);
		object_3 = object + 2u;
		OBJECT(object_3 - 0x2u)->sprite_flags = 0x5000u;
		object_3 += 2u;
		OBJECT(object_3 - 0x4u)->sprite_frame = 0xE0u;
		object_3 += 2u;
		OBJECT(object_3 - 0x6u)->sprite_palette = 0u;
		object_3 += 2u;
		OBJECT(object_3 - 0x8u)->sprite_scale = 0x140u;
		object_3 += 2u;
		OBJECT(object_3 - 0xAu)->sprite_layer = 0u;
		object_3 += 2u;
		OBJECT(object_3 - 0xCu)->screen_y = 0;
		object_3 += 2u;
		OBJECT(object_3 - 0xEu)->screen_x = 0;
		OBJECT(object_3 - 0xEu)->hitbox_size_x = 0u;
		OBJECT(object)->parent = object_2;
		OBJECT(object_2)->child = object;
		clips_left--;
	} while (clips_left != 0u);
	OBJECT(current_object)->handler = clipped_copy_sprite_update_ADDRESS;
}

/* Unused: until it leaves the screen (then the copy and the two clip blocks are freed and it goes):
   draw_depth = its floor's screen y; it queues the second clip entry, itself, the first clip entry with
   the screen y of the point 0x60 above its floor (point_to_screen) in its low 12 sprite_flags bits, and
   the copy at its position, all at draw_depth. */
void clipped_copy_sprite_update(void)
{
	uint16_t child = 0, object = 0, projected_x = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0, projected_y = 0, screen_y = 0, z = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored_2, &ignored_3);
		OBJECT(current_object)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		depth = OBJECT(current_object)->draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored_2, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->draw_depth;
		queue_sprite_at_depth(&ignored_2, &depth);
		projected_x = (uint16_t)OBJECT(current_object)->projected_x;
		projected_y = OBJECT(current_object)->projected_y;
		z = OBJECT(current_object)->floor_z - 0x600000u;
		point_to_screen(projected_x, z, &projected_y, &ignored_4, &screen_y);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_flags &= ~0xFFFu;
		OBJECT(current_object)->sprite_flags |= screen_y & 0xFFFu;
		depth = OBJECT(object_cursor)->draw_depth;
		queue_sprite_at_depth(&ignored_2, &depth);
		current_object = LINKED(current_object, OBJECT(object_cursor)->clipped_copy_sprite.copy_object);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		depth = OBJECT(object_cursor)->draw_depth;
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	}
	current_object = LINKED(current_object, OBJECT(object_cursor)->clipped_copy_sprite.copy_object);
	push_free_object();
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	child = OBJECT(current_object)->child;
	free_object_2(child);
	object = (uint16_t)current_object;
	free_object_2(object);
	delete_current_object();
}

/* Sets up stage 0x10's invisible geyser front from geyser_fronts[sprite_frame] (the spawn's index): z,
   top_height, its run of geyser_lists, path and row_pattern, previous_floor_z_int 0x30 below (a row on
   the first step); clears geyser_front_start. Once an object is free it takes it as its hazard box (0x30
   x 0xC0, kind 0x29) and waits (geyser_front_wait). */
void geyser_front_init(void)
{
	uint32_t free_count = 0, hitbox_object = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, path = 0;

	path = geyser_fronts[OBJECT(current_object)->sprite_frame] + geyser_fronts_ADDRESS;
	OBJECT(current_object)->z_int = GEYSER_FRONT_RECORD(path)->start_z;
	OBJECT(current_object)->z_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->geyser_front.top_height = GEYSER_FRONT_RECORD(path - 0x2u)->top_height;
	path += 2u;
	OBJECT(current_object)->geyser_front.next_listed_geyser = GEYSER_FRONT_RECORD(path - 0x4u)->geyser_list + geyser_lists_ADDRESS;
	OBJECT(current_object)->geyser_front.path = path + 2u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &hitbox_object);
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	OBJECT(current_object)->previous_floor_z_int += 0x30u;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->geyser_front.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->geyser_front.projected_x_fraction = 0u;
	OBJECT(current_object)->geyser_front.row_pattern = geyser_row_patterns[OBJECT(current_object)->sprite_frame] + geyser_row_patterns_ADDRESS;
	OBJECT(current_object)->geyser_front.holding_back = 0u;
	OBJECT(current_object)->geyser_front.wait_timer = 0;
	geyser_front_start = 0u;
	OBJECT(current_object)->geyser_front.sound_timer = 0;
	take_free_object(&free_count, &hitbox_object);
	if (free_count != 0u)
	{
		OBJECT(hitbox_object)->hitbox_height = 0x40u;
		OBJECT(hitbox_object)->hitbox_size_x = 0x30u;
		OBJECT(hitbox_object)->hitbox_size_y = 0xC0u;
		OBJECT(hitbox_object)->flags = 2u;
		OBJECT(hitbox_object)->hitbox_offset_x = 0;
		OBJECT(hitbox_object)->hitbox_offset_y = 0;
		OBJECT(hitbox_object)->direction = 0x300u;
		OBJECT(hitbox_object)->kind = 0x29u;
		OBJECT(current_object)->child = hitbox_object;
		OBJECT(current_object)->handler = geyser_front_wait_ADDRESS;
	}
}

/* The geyser front before it rises: spawns its listed geysers as scroll_position reaches them
   (spawn_listed_geyser) and starts rising (geyser_front_rise) once geyser_front_start is set or
   scroll_position reaches 0x6500 (0x8400 for the second front). */
void geyser_front_wait(void)
{
	uint16_t start_position = 0;

	spawn_listed_geyser(0u);
	if (geyser_front_start != 0u)
	{
		OBJECT(current_object)->handler = geyser_front_rise_ADDRESS;
		return;
	}
	start_position = 0x6500u;
	if (OBJECT(current_object)->sprite_frame != 0u)
		start_position = 0x8400u;
	if (scroll_position < start_position)
		return;
	OBJECT(current_object)->handler = geyser_front_rise_ADDRESS;
}

/* The rising geyser front, the chasers' routine: its path (geyser_front_read_script) on the first frame
   and while play goes on; when a player mishap, a drop-in or no player in play begins it holds back
   (base_speed 0, screen_threshold 0x130, both saved, hold_timer 0xFF), then only steps
   (geyser_front_rise_step). */
void geyser_front_rise(void)
{
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x200000u;
	if ((flags & 0x200000u) == 0u)
	{
		geyser_front_read_script();
		return;
	}
	if (OBJECT(current_object)->geyser_front.holding_back != 0u)
	{
		geyser_front_rise_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		geyser_front_read_script();
		return;
	}
	OBJECT(current_object)->geyser_front.holding_back = 0xFFu;
	OBJECT(current_object)->geyser_front.saved_base_speed = OBJECT(current_object)->geyser_front.base_speed;
	OBJECT(current_object)->geyser_front.base_speed = 0;
	OBJECT(current_object)->geyser_front.hold_start_position = scroll_position;
	OBJECT(current_object)->geyser_front.hold_timer = 0xFFu;
	OBJECT(current_object)->geyser_front.saved_threshold = OBJECT(current_object)->geyser_front.screen_threshold;
	OBJECT(current_object)->geyser_front.screen_threshold = 0x130;
	geyser_front_rise_step();
}

/* The geyser front's path: with no wait left, once scroll_position reaches the entry's position or the
   front has risen to its z limit, it loads the entry's screen_threshold, catch_up_factor, base_speed and
   wait_timer, or at the path's end frees its hazard box and is removed; then geyser_front_rise_step. */
void geyser_front_read_script(void)
{
	uint32_t cursor = 0;

	if ((uint16_t)OBJECT(current_object)->geyser_front.wait_timer != 0u)
	{
		geyser_front_rise_step();
		return;
	}
	cursor = OBJECT(current_object)->geyser_front.path;
	if (scroll_position < CHASE_STEP_WITH_LIMIT(cursor)->position && CHASE_STEP_WITH_LIMIT(cursor)->limit < OBJECT(current_object)->z_int)
	{
		geyser_front_rise_step();
		return;
	}
	cursor += 4u;
	if (CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->catch_up_factor < 0)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		push_free_object();
		delete_current_object();
		return;
	}
	OBJECT(current_object)->geyser_front.screen_threshold = CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->screen_threshold;
	cursor += 2u;
	OBJECT(current_object)->geyser_front.catch_up_factor = CHASE_STEP_WITH_LIMIT(cursor - 0x6u)->catch_up_factor;
	cursor += 2u;
	OBJECT(current_object)->geyser_front.base_speed = CHASE_STEP_WITH_LIMIT(cursor - 0x8u)->base_speed;
	cursor += 2u;
	OBJECT(current_object)->geyser_front.wait_timer = CHASE_STEP_WITH_LIMIT(cursor - 0xAu)->wait;
	OBJECT(current_object)->geyser_front.path = cursor + 2u;
	geyser_front_rise_step();
}

/* A step of the geyser front: chaser_speed by the chasers' rule (base_speed within screen_threshold,
   else camera speed x catch_up_factor, at least base_speed; holding back ends after 0x300 scroll units
   or hold_timer); past the wait it climbs that far up the floor's slope, its hazard box 0xA0 below;
   listed geysers, a row every 0x30 of rise, the line at top_height. */
void geyser_front_rise_step(void)
{
	uint16_t step = 0;
	uint32_t attribute_record = 0, geyser = 0, ignored = 0, point_x = 0, point_y = 0, slope_step = 0, step_2 = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0;

	if (OBJECT(current_object)->geyser_front.holding_back == 0u)
	{
		update_screen_position(&slope_step, &ignored);
		if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->geyser_front.screen_threshold)
			step = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
		else
		{
			OBJECT(current_object)->geyser_front.wait_timer = 0;
			if (OBJECT(current_object)->geyser_front.holding_back != 0u)
			{
				OBJECT(current_object)->geyser_front.holding_back = 0u;
				OBJECT(current_object)->geyser_front.screen_threshold = OBJECT(current_object)->geyser_front.saved_threshold;
				OBJECT(current_object)->geyser_front.base_speed = OBJECT(current_object)->geyser_front.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			step = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->geyser_front.catch_up_factor) >> 8;
			if ((int16_t)step <= OBJECT(current_object)->geyser_front.base_speed)
				step = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->geyser_front.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->geyser_front.holding_back = 0u;
		OBJECT(current_object)->geyser_front.screen_threshold = OBJECT(current_object)->geyser_front.saved_threshold;
		OBJECT(current_object)->geyser_front.base_speed = OBJECT(current_object)->geyser_front.saved_base_speed;
		compute_camera_ground_speed(0u);
		step = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->geyser_front.catch_up_factor) >> 8;
		if ((int16_t)step <= OBJECT(current_object)->geyser_front.base_speed)
			step = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->geyser_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->geyser_front.hold_timer;
		OBJECT(current_object)->geyser_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->geyser_front.holding_back = 0u;
			OBJECT(current_object)->geyser_front.screen_threshold = OBJECT(current_object)->geyser_front.saved_threshold;
			OBJECT(current_object)->geyser_front.base_speed = OBJECT(current_object)->geyser_front.saved_base_speed;
			compute_camera_ground_speed(0u);
			step = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->geyser_front.catch_up_factor) >> 8;
			if ((int16_t)step <= OBJECT(current_object)->geyser_front.base_speed)
				step = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
		}
		else
		{
			update_screen_position(&slope_step, &ignored);
			if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->geyser_front.screen_threshold)
				step = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
			else
			{
				OBJECT(current_object)->geyser_front.wait_timer = 0;
				if (OBJECT(current_object)->geyser_front.holding_back != 0u)
				{
					OBJECT(current_object)->geyser_front.holding_back = 0u;
					OBJECT(current_object)->geyser_front.screen_threshold = OBJECT(current_object)->geyser_front.saved_threshold;
					OBJECT(current_object)->geyser_front.base_speed = OBJECT(current_object)->geyser_front.saved_base_speed;
				}
				compute_camera_ground_speed(0u);
				step = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->geyser_front.catch_up_factor) >> 8;
				if ((int16_t)step <= OBJECT(current_object)->geyser_front.base_speed)
					step = (uint16_t)OBJECT(current_object)->geyser_front.base_speed;
			}
		}
	}
	chaser_speed = step;
	wait_timer = OBJECT(current_object)->geyser_front.wait_timer;
	OBJECT(current_object)->geyser_front.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->geyser_front.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)chaser_speed << 12;
		attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
		slope_step = ((uint32_t)FLOOR_ATTRIBUTE_RECORD(attribute_record)->x_per_rise * chaser_speed) >> 4;
		step_2 = slope_step;
		if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
			step_2 = 0u - step_2;
		OBJECT(current_object)->geyser_front.fixed_projected_x += step_2;
		OBJECT(current_object)->projected_x = OBJECT(current_object)->geyser_front.projected_x_whole;
		OBJECT(current_object)->projected_y -= slope_step >> 1;
		point_x = OBJECT(current_object)->geyser_front.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->x_int -= 0x10u;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->z_int += OBJECT(current_object)->hitbox_height;
	OBJECT(current_object)->z_int += 0x60u;
	add_to_touchable_list(slope_step);
	current_object = object_cursor;
	geyser = spawn_listed_geyser(0u);
	OBJECT(current_object)->flags &= ~1u;
	if (OBJECT(current_object)->z_int <= OBJECT(current_object)->geyser_front.top_height)
	{
		OBJECT(current_object)->geyser_front.top_height = 0;
		spawn_geyser_line(geyser);
		geyser_front_spawn_geyser_row();
		return;
	}
	if ((int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->z_int) < 0x30)
	{
		geyser_front_rise_sound();
		return;
	}
	geyser_front_spawn_geyser_row();
}

/* A row of geysers from the front: sets flags bit 0 (the geysers of earlier rows then go one depth back)
   and previous_floor_z_int = z_int; with 5 free objects, five geysers at its x and z, 0x2E apart in y
   from 0x10 past it, each waiting its delay from the geyser_row_delays set its next row_pattern byte
   picks. Then geyser_front_rise_sound. */
void geyser_front_spawn_geyser_row(void)
{
	uint16_t wait = 0;
	uint32_t ignored = 0, row_delays = 0, row_pattern = 0, x = 0, y = 0;
	uint16_t geysers_remaining = 0;

	OBJECT(current_object)->flags |= 1u;
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	geysers_left = 5;
	if (free_object_stack < geysers_left)
		return;
	geyser_y = OBJECT(current_object)->y;
	geyser_y_int += 0x10u;
	row_pattern = OBJECT(current_object)->geyser_front.row_pattern;
	row_delays = GEYSER_ROW_DELAY_CHOICE(row_pattern)[0];
	OBJECT(current_object)->geyser_front.row_pattern = row_pattern + 1u;
	row_delays = row_delays * 0xAu + geyser_row_delays_ADDRESS;
	geyser_delay_cursor = row_delays;
	do
	{
		x = OBJECT(current_object)->x;
		y = geyser_y;
		geyser_y_int += 0x2Eu;
		r14 = OBJECT(current_object)->z;
		wait = mem_read16(geyser_delay_cursor);
		geyser_delay_cursor += 2u;
		spawn_geyser(x, y, wait, &ignored, &row_delays);
		geysers_remaining = geysers_left;
		geysers_left = geysers_remaining - 1u;
	} while ((int16_t)geysers_remaining > 1);
	geyser_front_rise_sound();
}

/* The geyser front's rumble: while its screen_y is at most 0x130, sound 0x12 every 0x32 frames
   (sound_timer). */
void geyser_front_rise_sound(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t sound_timer = 0;

	if (OBJECT(current_object)->screen_y <= 0x130)
	{
		sound_timer = OBJECT(current_object)->geyser_front.sound_timer;
		OBJECT(current_object)->geyser_front.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->geyser_front.sound_timer = 0x32;
			sound_id = 0x12u;
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
}

/* Spawns the front's next geyser of geyser_lists once scroll_position reaches its position: at its x, y
   and z, waiting for scroll_position to reach its start position (flags bit 31, animation_speed 0x20).
   Hands back geyser, the new object (unchanged when none was made). */
uint32_t spawn_listed_geyser(uint32_t geyser)
{
	uint16_t erupt_position = 0;
	uint32_t record = 0, spawned = 0, x = 0, y = 0;

	if (LISTED_GEYSER(OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser)->position <= scroll_position)
	{
		record = OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser + 2u;
		x = (uint32_t)LISTED_GEYSER(record - 0x2u)->x << 16;
		record += 2u;
		y = (uint32_t)LISTED_GEYSER(record - 0x4u)->y << 16;
		record += 2u;
		r14 = (uint32_t)LISTED_GEYSER(record - 0x6u)->z << 16;
		record += 2u;
		erupt_position = LISTED_GEYSER(record - 0x8u)->erupt_position;
		OBJECT(current_object)->spawn_listed_geyser.next_listed_geyser = record + 2u;
		spawn_geyser(x, y, erupt_position, &spawned, &geyser);
		if (spawned != 0u)
		{
			OBJECT(geyser)->flags |= 0x80000000u;
			OBJECT(geyser)->animation_speed = 0x20u;
		}
	}
	return geyser;
}

/* Makes a geyser at x, y and height r14: sprite 0x5DA0, a 0x20 x 0x30 box hitbox of kind 0x27 (it
   hurts), geyser_animation, draw_depth 0xEF, the front as its parent, erupt_delay = wait; handler
   geyser_wait. Hands back spawned (0xFFFFFFFF when made, 0 with no free object) and geyser. */
void spawn_geyser(uint32_t x, uint32_t y, uint16_t wait, uint32_t *p_spawned, uint32_t *p_geyser)
{
	uint32_t geyser = *p_geyser;

	allocate_object(p_spawned, &geyser);
	if (*p_spawned != 0u)
	{
		OBJECT(geyser)->x = x;
		OBJECT(geyser)->y = y;
		OBJECT(geyser)->z = r14;
		OBJECT(geyser)->geyser_wait.erupt_delay = wait;
		OBJECT(geyser)->sprite_flags = 0x1000u;
		OBJECT(geyser)->sprite_frame = 0x5DA0u;
		OBJECT(geyser)->sprite_palette = 0x8280u;
		OBJECT(geyser)->hitbox_size_x = 0x20u;
		OBJECT(geyser)->hitbox_size_y = 0x30u;
		OBJECT(geyser)->hitbox_height = 0x20u;
		OBJECT(geyser)->kind = 0x27u;
		OBJECT(geyser)->geyser_wait.draw_depth = 0xEFu;
		OBJECT(geyser)->handler = geyser_wait_ADDRESS;
		OBJECT(geyser)->direction = 0x300u;
		OBJECT(geyser)->sprite_offset_x = 0;
		OBJECT(geyser)->sprite_offset_y = -0x10;
		OBJECT(geyser)->flags = 2u;
		OBJECT(geyser)->hitbox_offset_x = -5;
		OBJECT(geyser)->hitbox_offset_y = -0x18;
		OBJECT(geyser)->parent = current_object;
		OBJECT(geyser)->animation_speed = 8u;
		OBJECT(geyser)->animation_timer = 0;
		OBJECT(geyser)->animation_frame = 0xFFu;
		OBJECT(geyser)->animation = geyser_animation_ADDRESS;
		*p_spawned = 0xFFFFFFFFu;
	}
	*p_geyser = geyser;
}

/* A geyser waiting to erupt (geyser_erupt): a listed one (flags bit 31) until scroll_position reaches
   erupt_delay, drawn meanwhile; a row one, not drawn, counts erupt_delay down, going a depth back
   whenever its front spawns a row. Neither counts while the front holds back; both keep
   anchor_x/anchor_y on their projected position. */
void geyser_wait(void)
{
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t erupt_delay = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->geyser_front.holding_back == 0u && OBJECT(current_object)->geyser_wait.erupt_delay <= scroll_position)
			OBJECT(current_object)->handler = geyser_erupt_ADDRESS;
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->flags & 1u) != 0u)
		OBJECT(current_object)->geyser_wait.draw_depth--;
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->geyser_front.holding_back == 0u)
	{
		erupt_delay = OBJECT(current_object)->geyser_wait.erupt_delay;
		OBJECT(current_object)->geyser_wait.erupt_delay = erupt_delay - 1u;
		if ((int16_t)erupt_delay <= 1)
			OBJECT(current_object)->handler = geyser_erupt_ADDRESS;
	}
}

/* An erupting geyser: the ground cracks (geyser_animation, faster from frame 3, sound 0xA at frame 3),
   then the water jet (geyser_jet_animation, looping); touchable from frame 4, removed below screen y
   0x180. Row geysers draw their jet at draw_depth (one back for each new row of their front), the others
   at screen_y. */
void geyser_erupt(void)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if ((int8_t)OBJECT(current_object)->animation_frame >= 3)
		OBJECT(current_object)->animation_speed = 2u;
	at_last_step = animate_around_anchor();
	if (at_last_step != 0u)
		OBJECT(current_object)->animation = geyser_jet_animation_ADDRESS;
	if (OBJECT(current_object)->animation_frame == 3u && (uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
	{
		sound_id = 0xAu;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	update_screen_position(&ignored_3, &ignored);
	if (OBJECT(current_object)->screen_y > 0x180)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->animation == geyser_jet_animation_ADDRESS || (int8_t)OBJECT(current_object)->animation_frame > 3)
		add_to_touchable_list(ignored_3);
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->flags & 1u) != 0u)
		OBJECT(current_object)->geyser_erupt.draw_depth--;
	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u || (OBJECT(current_object)->animation != geyser_jet_animation_ADDRESS && (int8_t)OBJECT(current_object)->animation_frame < 4))
	{
		ignored = queue_sprite(ignored);
		return;
	}
	depth = OBJECT(current_object)->geyser_erupt.draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* Spawns the waterfall as a group leader (group_leaders[the record's leader & 3]) from the record: x, y,
   z, direction and its group_leader_data path; a multi-part sprite whose part list is a second-pool copy
   of waterfall_parts, waterfall_animation at 3 frames a step; handler waterfall_move. With no
   second-pool block free the object it allocated is left unset. */
void spawn_waterfall(uint32_t record, uint32_t *p_spawned, uint32_t *p_free_stack, uint32_t *p_block)
{
	uint32_t block = *p_block;
	uint32_t ignored = 0, object = 0;

	allocate_object_outside_loop(p_spawned, &block);
	if (*p_spawned != 0u && (uint16_t)free_object_stack_2[0] != 0u)
	{
		group_leaders[LEADER_SPAWN(record)->leader & 3u] = block;
		record++;
		OBJECT(block)->x_int = LEADER_SPAWN(record - 0x1u)->x;
		OBJECT(block)->x_fraction = 0u;
		record += 2u;
		OBJECT(block)->y_int = LEADER_SPAWN(record - 0x3u)->y;
		OBJECT(block)->y_fraction = 0u;
		record += 2u;
		OBJECT(block)->z_int = LEADER_SPAWN(record - 0x5u)->z;
		OBJECT(block)->z_fraction = 0u;
		OBJECT(block)->floor_z = OBJECT(block)->z;
		record += 2u;
		OBJECT(block)->direction = LEADER_SPAWN(record - 0x7u)->direction;
		OBJECT(block)->waterfall.path = LEADER_SPAWN(record - 0x7u)->path + group_leader_data_ADDRESS;
		OBJECT(block)->handler = waterfall_move_ADDRESS;
		OBJECT(block)->sprite_flags = 0xD00Au;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->waterfall.holding_back = 0u;
		OBJECT(block)->waterfall.sound_timer = 0u;
		OBJECT(block)->waterfall.wait_timer = 1;
		OBJECT(block)->animation = waterfall_animation_ADDRESS;
		OBJECT(block)->animation_frame = 0u;
		OBJECT(block)->animation_speed = 3u;
		OBJECT(block)->animation_timer = 0;
		object = block;
		allocate_object_2(&ignored, p_free_stack, &block);
		move_halfwords_backward(block, waterfall_parts_ADDRESS, 0xDu);
		OBJECT(object)->part_list = block;
		*p_spawned = 0xFFFFFFFFu;
	}
	*p_block = block;
}

/* The waterfall, a group leader moving up the river (-y), the chasers' routine: its path
   (waterfall_read_script) on the first frame and while play goes on; when a player mishap, a drop-in or
   no player in play begins it holds back (base_speed 0, screen_threshold 0x160, both saved, hold_timer
   0xFF), then only steps (waterfall_move_step). */
void waterfall_move(void)
{
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		waterfall_read_script();
		return;
	}
	if (OBJECT(current_object)->waterfall.holding_back != 0u)
	{
		waterfall_move_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		waterfall_read_script();
		return;
	}
	OBJECT(current_object)->waterfall.holding_back = 0xFFu;
	OBJECT(current_object)->waterfall.saved_base_speed = OBJECT(current_object)->waterfall.base_speed;
	OBJECT(current_object)->waterfall.base_speed = 0;
	OBJECT(current_object)->waterfall.hold_start_position = scroll_position;
	OBJECT(current_object)->waterfall.hold_timer = 0xFFu;
	OBJECT(current_object)->waterfall.saved_threshold = OBJECT(current_object)->waterfall.screen_threshold;
	OBJECT(current_object)->waterfall.screen_threshold = 0x160;
	waterfall_move_step();
}

/* The waterfall's path: once wait_timer has run out and scroll_position reaches the entry's position it
   loads the entry's screen_threshold, catch_up_factor, base_speed and wait_timer, or at the path's end
   frees its part list and is removed; then waterfall_move_step. */
void waterfall_read_script(void)
{
	uint16_t part_list = 0;
	uint32_t cursor = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0;

	wait_timer = OBJECT(current_object)->waterfall.wait_timer;
	OBJECT(current_object)->waterfall.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		waterfall_move_step();
		return;
	}
	OBJECT(current_object)->waterfall.wait_timer = 0;
	cursor = OBJECT(current_object)->waterfall.path;
	trigger_scroll = CHASE_STEP_AT_SCROLL(cursor)->position;
	cursor += 2u;
	if (scroll_position < trigger_scroll)
	{
		waterfall_move_step();
		return;
	}
	if (CHASE_STEP_AT_SCROLL(cursor - 0x2u)->catch_up_factor < 0)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->waterfall.screen_threshold = CHASE_STEP_AT_SCROLL(cursor - 0x2u)->screen_threshold;
	cursor += 2u;
	OBJECT(current_object)->waterfall.catch_up_factor = CHASE_STEP_AT_SCROLL(cursor - 0x4u)->catch_up_factor;
	cursor += 2u;
	OBJECT(current_object)->waterfall.base_speed = CHASE_STEP_AT_SCROLL(cursor - 0x6u)->base_speed;
	cursor += 2u;
	OBJECT(current_object)->waterfall.wait_timer = CHASE_STEP_AT_SCROLL(cursor - 0x8u)->wait;
	OBJECT(current_object)->waterfall.path = cursor + 2u;
	waterfall_move_step();
}

/* A step of the waterfall: chaser_speed by the chasers' rule (base_speed while screen_x is within
   screen_threshold and screen_y within 0xE0, else camera speed x catch_up_factor, at least base_speed),
   and past the wait it moves -y by it; from y 0x1480 geyser_front_start. Sound 0x10 every 0x3C frames
   unless map_screen_active; animated, drawn. */
void waterfall_move_step(void)
{
	uint8_t sound_id = 0;
	uint16_t speed = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint8_t hold_timer = 0;
	uint16_t sound_timer = 0;

	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->waterfall.holding_back == 0u)
	{
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->waterfall.screen_threshold && OBJECT(current_object)->screen_y <= 0xE0)
		{
			speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
			chaser_speed = speed;
			if (OBJECT(current_object)->waterfall.wait_timer <= 0)
				OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			if (OBJECT(current_object)->y_int <= 0x1480)
				geyser_front_start = 0xFFu;
			if (map_screen_active == 0u)
			{
				sound_timer = OBJECT(current_object)->waterfall.sound_timer;
				OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
				if ((int16_t)sound_timer <= 1)
				{
					OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
					sound_id = 0x10u;
					play_sound(sound_id, &ignored_2, &ignored_3);
				}
			}
			animate_all_parts(&ignored_6, &ignored_5);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->waterfall.wait_timer = 0;
		if (OBJECT(current_object)->waterfall.holding_back != 0u)
		{
			OBJECT(current_object)->waterfall.holding_back = 0u;
			OBJECT(current_object)->waterfall.screen_threshold = OBJECT(current_object)->waterfall.saved_threshold;
			OBJECT(current_object)->waterfall.base_speed = OBJECT(current_object)->waterfall.saved_base_speed;
		}
		compute_camera_ground_speed(ignored_4);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->waterfall.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->waterfall.base_speed)
			speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->waterfall.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->waterfall.holding_back = 0u;
		OBJECT(current_object)->waterfall.screen_threshold = OBJECT(current_object)->waterfall.saved_threshold;
		OBJECT(current_object)->waterfall.base_speed = OBJECT(current_object)->waterfall.saved_base_speed;
		compute_camera_ground_speed(ignored_4);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->waterfall.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->waterfall.base_speed)
			speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->waterfall.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->waterfall.hold_timer;
		OBJECT(current_object)->waterfall.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->waterfall.holding_back = 0u;
			OBJECT(current_object)->waterfall.screen_threshold = OBJECT(current_object)->waterfall.saved_threshold;
			OBJECT(current_object)->waterfall.base_speed = OBJECT(current_object)->waterfall.saved_base_speed;
			compute_camera_ground_speed(ignored_4);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->waterfall.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->waterfall.base_speed)
				speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
		}
		else
		{
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->waterfall.screen_threshold && OBJECT(current_object)->screen_y <= 0xE0)
			{
				speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
				chaser_speed = speed;
				if (OBJECT(current_object)->waterfall.wait_timer <= 0)
					OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
				if (OBJECT(current_object)->y_int <= 0x1480)
					geyser_front_start = 0xFFu;
				if (map_screen_active == 0u)
				{
					sound_timer = OBJECT(current_object)->waterfall.sound_timer;
					OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
					if ((int16_t)sound_timer <= 1)
					{
						OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
						sound_id = 0x10u;
						play_sound(sound_id, &ignored_2, &ignored_3);
					}
				}
				animate_all_parts(&ignored_6, &ignored_5);
				depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->waterfall.wait_timer = 0;
			if (OBJECT(current_object)->waterfall.holding_back != 0u)
			{
				OBJECT(current_object)->waterfall.holding_back = 0u;
				OBJECT(current_object)->waterfall.screen_threshold = OBJECT(current_object)->waterfall.saved_threshold;
				OBJECT(current_object)->waterfall.base_speed = OBJECT(current_object)->waterfall.saved_base_speed;
			}
			compute_camera_ground_speed(ignored_4);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->waterfall.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->waterfall.base_speed)
				speed = (uint16_t)OBJECT(current_object)->waterfall.base_speed;
		}
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->waterfall.wait_timer <= 0)
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
	if (OBJECT(current_object)->y_int <= 0x1480)
		geyser_front_start = 0xFFu;
	if (map_screen_active == 0u)
	{
		sound_timer = OBJECT(current_object)->waterfall.sound_timer;
		OBJECT(current_object)->waterfall.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->waterfall.sound_timer = 0x3Cu;
			sound_id = 0x10u;
			play_sound(sound_id, &ignored_2, &ignored_3);
		}
	}
	animate_all_parts(&ignored_6, &ignored_5);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x60u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* A row of the river's water, its parent the waterfall: its frame follows the waterfall's
   animation_frame, drawn at screen_y + depth_offset, removed below screen y 0xE8. Once the waterfall
   reaches its y it takes two clip entries (given two second-pool blocks) cutting it at the waterfall's
   line; with the waterfall 0x5C past, it frees them and goes. */
void river_water_update(void)
{
	uint16_t child = 0;
	uint32_t clip_x = 0, clips_left = 0, depth = 0, floor_z = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, object_2 = 0, object_3 = 0, point_y = 0;

	switch (OBJECT(current_object)->river_water.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->river_water.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			OBJECT(current_object)->river_water.state++;
		if (OBJECT(current_object)->screen_y > 0xE8)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame];
		update_screen_position(&ignored, &ignored_2);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			OBJECT(current_object)->river_water.state++;
		if (OBJECT(current_object)->screen_y > 0xE8)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame];
		update_screen_position(&ignored, &ignored_2);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack_2[0] < 2)
		{
			if (OBJECT(current_object)->screen_y > 0xE8)
			{
				delete_current_object();
				return;
			}
			OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame];
			update_screen_position(&ignored, &ignored_2);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) >= 0x1A1u || OBJECT(current_object)->screen_y <= -0xB0)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->river_water.state++;
		clips_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored_3, &object_2);
			object = object_2 + 2u;
			OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
			object += 2u;
			OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
			object += 2u;
			OBJECT(object - 0x6u)->sprite_palette = 0u;
			object += 2u;
			OBJECT(object - 0x8u)->sprite_scale = 0x140u;
			object += 2u;
			OBJECT(object - 0xAu)->sprite_layer = 0u;
			object += 2u;
			OBJECT(object - 0xCu)->screen_y = 0;
			object += 2u;
			OBJECT(object - 0xEu)->screen_x = 0;
			OBJECT(object - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			clips_left--;
		} while (clips_left != 0u);
	case 3:
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame];
		update_screen_position(&ignored, &ignored_2);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int16_t)(OBJECT(current_object)->y_int - 0x5Cu) < OBJECT(object)->y_int)
		{
			clip_x = OBJECT(current_object)->x;
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored_2, &clip_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = clip_x;
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x10u) < 0x1A1u)
			{
				OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->river_water.depth_offset;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		invalid_jump(0x8CEC1u + OBJECT(current_object)->river_water.state * 3u);
	}
}

/* Sets up a stone block from stone_block_positions[sprite_frame] (the spawn record's index): x, y, z and
   depth_offset; sprite 0x8A00, palette 0x9120; handler stone_block_draw. */
void stone_block_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0, record = 0;

	record = stone_block_positions_ADDRESS + OBJECT(current_object)->sprite_frame * 2u * 4u;
	OBJECT(current_object)->x_int = STONE_BLOCK_POSITION(record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = STONE_BLOCK_POSITION(record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->z_int = STONE_BLOCK_POSITION(record - 0x4u)->z;
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->stone_block.depth_offset = STONE_BLOCK_POSITION(record - 0x4u)->depth_offset;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x8A00u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = stone_block_draw_ADDRESS;
}

/* A stone block: queued at screen_y + depth_offset until it has left the screen (check_left_screen),
   then removed. */
void stone_block_draw(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->stone_block.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* When the geyser front reaches its top: makes the invisible geyser line at
   geyser_line_starts[sprite_frame] (x, y, z, and its path in geyser_line_scripts); handler
   geyser_line_move. */
void spawn_geyser_line(uint32_t line)
{
	uint32_t free_count = 0, record = 0;

	allocate_object(&free_count, &line);
	if (free_count != 0u)
	{
		record = ((uint32_t)OBJECT(current_object)->sprite_frame << 3) + geyser_line_starts_ADDRESS;
		OBJECT(line)->x_int = GEYSER_LINE_START(record)->x;
		OBJECT(line)->x_fraction = 0u;
		record += 2u;
		OBJECT(line)->y_int = GEYSER_LINE_START(record - 0x2u)->y;
		OBJECT(line)->y_fraction = 0u;
		record += 2u;
		OBJECT(line)->z_int = GEYSER_LINE_START(record - 0x4u)->z;
		OBJECT(line)->z_fraction = 0u;
		OBJECT(line)->floor_z = OBJECT(line)->z;
		OBJECT(line)->geyser_line.path = GEYSER_LINE_START(record - 0x4u)->script + geyser_line_scripts_ADDRESS;
		OBJECT(line)->geyser_line.holding_back = 0u;
		OBJECT(line)->geyser_line.wait_timer = 0;
		OBJECT(line)->flags = 0u;
		OBJECT(line)->handler = geyser_line_move_ADDRESS;
		OBJECT(line)->geyser_line.travel = 0;
	}
}

/* The invisible geyser line: once its wait is over it takes the next geyser_line_scripts entry (speed,
   wait_timer) when scroll_position reaches its position or y its y limit; it moves -y by speed, a geyser
   burst every 0x300 of travel, one more at the script's end, then goes. After a player mishap it stands
   still for up to 0xFF frames or 0x300 scroll units. */
void geyser_line_move(void)
{
	uint32_t cursor = 0, hold_distance = 0, ignored = 0, speed = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0;

	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->geyser_line.holding_back = 0xFFu;
			OBJECT(current_object)->geyser_line.hold_start_position = scroll_position;
			OBJECT(current_object)->geyser_line.hold_timer = 0xFFu;
		}
		else
		{
			wait_timer = OBJECT(current_object)->geyser_line.wait_timer;
			OBJECT(current_object)->geyser_line.wait_timer = wait_timer - 1u;
			if ((int16_t)wait_timer <= 1)
			{
				OBJECT(current_object)->geyser_line.wait_timer = 0;
				cursor = OBJECT(current_object)->geyser_line.path;
				trigger_scroll = GEYSER_LINE_STEP(cursor)->position;
				cursor += 2u;
				if (scroll_position < trigger_scroll)
				{
					trigger_y = GEYSER_LINE_STEP(cursor - 0x2u)->y_limit;
					cursor += 2u;
					if (OBJECT(current_object)->y_int <= (int16_t)trigger_y)
					{
						if (GEYSER_LINE_STEP(cursor - 0x4u)->wait < 0)
						{
							spawn_geyser_burst();
							delete_current_object();
							return;
						}
						OBJECT(current_object)->geyser_line.speed = GEYSER_LINE_STEP(cursor - 0x4u)->speed;
						cursor += 2u;
						OBJECT(current_object)->geyser_line.wait_timer = GEYSER_LINE_STEP(cursor - 0x6u)->wait;
						OBJECT(current_object)->geyser_line.path = cursor + 2u;
					}
				}
				else
				{
					cursor += 2u;
					if (GEYSER_LINE_STEP(cursor - 0x4u)->wait < 0)
					{
						spawn_geyser_burst();
						delete_current_object();
						return;
					}
					OBJECT(current_object)->geyser_line.speed = GEYSER_LINE_STEP(cursor - 0x4u)->speed;
					cursor += 2u;
					OBJECT(current_object)->geyser_line.wait_timer = GEYSER_LINE_STEP(cursor - 0x6u)->wait;
					OBJECT(current_object)->geyser_line.path = cursor + 2u;
				}
			}
		}
	}
	project_object(&hold_distance, &ignored);
	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		if (OBJECT(current_object)->geyser_line.holding_back == 0u)
		{
			speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
			OBJECT(current_object)->geyser_line.travel += speed;
			OBJECT(current_object)->y -= speed << 12;
			if (OBJECT(current_object)->geyser_line.travel >= 0x300)
			{
				spawn_geyser_burst();
				OBJECT(current_object)->geyser_line.travel = 0;
			}
		}
		return;
	}
	hold_distance = (hold_distance & ~0xFFFFu) | scroll_position;
	hold_distance = (hold_distance & ~0xFFFFu) | (uint16_t)(hold_distance - OBJECT(current_object)->geyser_line.hold_start_position);
	if ((int16_t)hold_distance >= 0x300)
	{
		OBJECT(current_object)->geyser_line.holding_back = 0u;
		if (OBJECT(current_object)->geyser_line.holding_back == 0u)
		{
			speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
			OBJECT(current_object)->geyser_line.travel += speed;
			OBJECT(current_object)->y -= speed << 12;
			if (OBJECT(current_object)->geyser_line.travel >= 0x300)
			{
				spawn_geyser_burst();
				OBJECT(current_object)->geyser_line.travel = 0;
			}
		}
		return;
	}
	if (no_player_in_play != 0u)
		OBJECT(current_object)->geyser_line.hold_timer = 0xFFu;
	hold_timer = OBJECT(current_object)->geyser_line.hold_timer;
	OBJECT(current_object)->geyser_line.hold_timer = hold_timer - 1u;
	if (hold_timer != 0u)
	{
		if (OBJECT(current_object)->geyser_line.holding_back == 0u)
		{
			speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
			OBJECT(current_object)->geyser_line.travel += speed;
			OBJECT(current_object)->y -= speed << 12;
			if (OBJECT(current_object)->geyser_line.travel >= 0x300)
			{
				spawn_geyser_burst();
				OBJECT(current_object)->geyser_line.travel = 0;
			}
		}
		return;
	}
	OBJECT(current_object)->geyser_line.holding_back = 0u;
	if (OBJECT(current_object)->geyser_line.holding_back == 0u)
	{
		speed = (hold_distance & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->geyser_line.speed;
		OBJECT(current_object)->geyser_line.travel += speed;
		OBJECT(current_object)->y -= speed << 12;
		if (OBJECT(current_object)->geyser_line.travel >= 0x300)
		{
			spawn_geyser_burst();
			OBJECT(current_object)->geyser_line.travel = 0;
		}
	}
}

/* Makes a geyser burst at the geyser line's position: palette 0x9130, a 0x30 x 0x30 box hitbox 0xA0 high
   of kind 0x29 (it hurts), geyser_burst_animation at 6 frames a step; handler geyser_burst_start. */
void spawn_geyser_burst(void)
{
	uint32_t free_count = 0, object = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = geyser_burst_start_ADDRESS;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x30u;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->sprite_offset_x = 0x14;
		OBJECT(object)->sprite_offset_y = 8;
		OBJECT(object)->hitbox_height = 0xA0u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->kind = 0x29u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->animation = geyser_burst_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 6u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
	}
}

/* Places a geyser burst on screen (project_object) and hands it to geyser_burst_erupt from the next
   frame. */
void geyser_burst_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = geyser_burst_erupt_ADDRESS;
}

/* A geyser burst: the ground cracks and bursts (geyser_burst_animation, faster from frame 4, sound 0xA
   as frame 4 starts), then it sprays (geyser_burst_spray_animation, looping), touchable only while
   spraying; queued at screen_y - 0x10, removed once off screen. */
void geyser_burst_erupt(void)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&ignored, &ignored_2, &at_last_step);
	if (OBJECT(current_object)->animation != geyser_burst_spray_animation_ADDRESS)
	{
		if (OBJECT(current_object)->animation_frame != 4u)
		{
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation = geyser_burst_spray_animation_ADDRESS;
		}
		else
			OBJECT(current_object)->animation_speed = 2u;
	}
	if (OBJECT(current_object)->animation != geyser_burst_spray_animation_ADDRESS && OBJECT(current_object)->animation_frame == 4u && OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer)
	{
		sound_id = 0xAu;
		play_sound(sound_id, &ignored, &ignored_3);
	}
	update_screen_position(&ignored_4, &ignored);
	if (OBJECT(current_object)->animation == geyser_burst_spray_animation_ADDRESS)
		add_to_touchable_list(ignored_4);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Sets up stage 0x10's propeller machine as chaser_object: multi-part, a 0xF2 x 0x80 box 0xF0 high of
   kind 0x18, propeller_machine_animation and propeller_machine_path. With a take_free_object block it
   copies propeller_machine_parts there as its part list, spawns its back zone (spawn_chaser_back_zone 0)
   and chases (propeller_machine_chase); else it retries next frame. */
void propeller_machine_init(void)
{
	uint32_t free_count = 0, parts = 0, zone_index = 0;

	OBJECT(current_object)->propeller_machine.path = propeller_machine_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->kind = 0x18u;
	OBJECT(current_object)->flags = 0x40000002u;
	OBJECT(current_object)->hitbox_size_x = 0xF2u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -8;
	OBJECT(current_object)->hitbox_offset_x = -8;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	chaser_object = current_object;
	OBJECT(current_object)->propeller_machine.sound_timer = 0u;
	OBJECT(current_object)->animation = propeller_machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 5u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->propeller_machine.holding_back = 0u;
	OBJECT(current_object)->propeller_machine.wait_timer = 0;
	OBJECT(current_object)->handler = propeller_machine_init_ADDRESS;
	take_free_object(&free_count, &parts);
	if (free_count != 0u)
	{
		move_halfwords(parts, propeller_machine_parts_ADDRESS, 0x2Du);
		OBJECT(current_object)->part_list = parts;
		zone_index = 0u;
		spawn_chaser_back_zone(zone_index, parts);
		OBJECT(current_object)->handler = propeller_machine_chase_ADDRESS;
	}
}

/* The propeller machine chasing the players (+y), the chasers' routine: its path
   (propeller_machine_read_script) on the first frame and while play goes on; when a player mishap, a
   drop-in or no player in play begins it holds back (base_speed 0, screen_threshold -0x50, both saved,
   hold_timer 0xFF), then only steps (propeller_machine_chase_step). */
void propeller_machine_chase(void)
{
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		propeller_machine_read_script();
		return;
	}
	if (OBJECT(current_object)->propeller_machine.holding_back != 0u)
	{
		propeller_machine_chase_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		propeller_machine_read_script();
		return;
	}
	OBJECT(current_object)->propeller_machine.holding_back = 0xFFu;
	OBJECT(current_object)->propeller_machine.saved_base_speed = OBJECT(current_object)->propeller_machine.base_speed;
	OBJECT(current_object)->propeller_machine.base_speed = 0;
	OBJECT(current_object)->propeller_machine.hold_start_position = scroll_position;
	OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
	OBJECT(current_object)->propeller_machine.saved_threshold = OBJECT(current_object)->propeller_machine.screen_threshold;
	OBJECT(current_object)->propeller_machine.screen_threshold = -0x50;
	propeller_machine_chase_step();
}

/* The propeller machine's path: once wait_timer has run out, when scroll_position reaches the entry's
   position or y its y limit it loads the entry's screen_threshold, catch_up_factor, base_speed and
   wait_timer, or at the path's end frees its part list block (sprite_frame is part_list's low half) and
   is removed; then propeller_machine_chase_step. */
void propeller_machine_read_script(void)
{
	uint32_t cursor = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0, trigger_y = 0;

	wait_timer = OBJECT(current_object)->propeller_machine.wait_timer;
	OBJECT(current_object)->propeller_machine.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		propeller_machine_chase_step();
		return;
	}
	OBJECT(current_object)->propeller_machine.wait_timer = 0;
	cursor = OBJECT(current_object)->propeller_machine.path;
	trigger_scroll = CHASE_STEP_WITH_LIMIT(cursor)->position;
	cursor += 2u;
	if (trigger_scroll <= scroll_position)
	{
		cursor += 2u;
		if (CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->catch_up_factor < 0)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->sprite_frame);
			push_free_object();
			delete_current_object();
			return;
		}
		OBJECT(current_object)->propeller_machine.screen_threshold = CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->screen_threshold;
		cursor += 2u;
		OBJECT(current_object)->propeller_machine.catch_up_factor = CHASE_STEP_WITH_LIMIT(cursor - 0x6u)->catch_up_factor;
		cursor += 2u;
		OBJECT(current_object)->propeller_machine.base_speed = CHASE_STEP_WITH_LIMIT(cursor - 0x8u)->base_speed;
		cursor += 2u;
		OBJECT(current_object)->propeller_machine.wait_timer = CHASE_STEP_WITH_LIMIT(cursor - 0xAu)->wait;
		OBJECT(current_object)->propeller_machine.path = cursor + 2u;
		propeller_machine_chase_step();
		return;
	}
	trigger_y = CHASE_STEP_WITH_LIMIT(cursor - 0x2u)->limit;
	cursor += 2u;
	if (OBJECT(current_object)->y_int < (int16_t)trigger_y)
	{
		propeller_machine_chase_step();
		return;
	}
	if (CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->catch_up_factor < 0)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->sprite_frame);
		push_free_object();
		delete_current_object();
		return;
	}
	OBJECT(current_object)->propeller_machine.screen_threshold = CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->screen_threshold;
	cursor += 2u;
	OBJECT(current_object)->propeller_machine.catch_up_factor = CHASE_STEP_WITH_LIMIT(cursor - 0x6u)->catch_up_factor;
	cursor += 2u;
	OBJECT(current_object)->propeller_machine.base_speed = CHASE_STEP_WITH_LIMIT(cursor - 0x8u)->base_speed;
	cursor += 2u;
	OBJECT(current_object)->propeller_machine.wait_timer = CHASE_STEP_WITH_LIMIT(cursor - 0xAu)->wait;
	OBJECT(current_object)->propeller_machine.path = cursor + 2u;
	propeller_machine_chase_step();
}

/* A step of the propeller machine: past the wait it moves +y by chaser_speed (base_speed from
   screen_threshold on, else camera_slope_speed x catch_up_factor, at least base_speed); the propellers
   turn (propeller_machine_animation in three parts). From y 0x1B00 its box is 0x68 high, from 0x1BB0
   geyser_front_start; sound 0xB every 0x3C frames on screen; zone list. */
void propeller_machine_chase_step(void)
{
	uint8_t sound_id = 0;
	uint16_t move_speed = 0, part_frame = 0;
	uint32_t animation = 0, depth = 0, frame_entry = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, next_frame = 0, slope_factor = 0;
	uint8_t hold_timer = 0, animation_timer = 0;
	uint16_t sound_timer = 0;

	if (OBJECT(current_object)->propeller_machine.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->propeller_machine.hold_start_position) >= 0x300)
		{
			OBJECT(current_object)->propeller_machine.holding_back = 0u;
			OBJECT(current_object)->propeller_machine.screen_threshold = OBJECT(current_object)->propeller_machine.saved_threshold;
			OBJECT(current_object)->propeller_machine.base_speed = OBJECT(current_object)->propeller_machine.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->propeller_machine.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->propeller_machine.hold_timer;
			OBJECT(current_object)->propeller_machine.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
			{
				OBJECT(current_object)->propeller_machine.holding_back = 0u;
				OBJECT(current_object)->propeller_machine.screen_threshold = OBJECT(current_object)->propeller_machine.saved_threshold;
				OBJECT(current_object)->propeller_machine.base_speed = OBJECT(current_object)->propeller_machine.saved_base_speed;
			}
		}
	}
	if (OBJECT(current_object)->propeller_machine.wait_timer <= 0)
	{
		if (OBJECT(current_object)->propeller_machine.screen_threshold <= OBJECT(current_object)->screen_x)
			move_speed = (uint16_t)OBJECT(current_object)->propeller_machine.base_speed;
		else
		{
			slope_factor = 0u;
			compute_camera_slope_speed(0u, slope_factor);
			move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->propeller_machine.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->propeller_machine.base_speed)
				move_speed = (uint16_t)OBJECT(current_object)->propeller_machine.base_speed;
		}
		chaser_speed = move_speed;
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
	}
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer <= 1)
	{
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		animation = OBJECT(current_object)->animation;
		next_frame = OBJECT(current_object)->animation_frame;
		next_frame = (uint16_t)(next_frame + 1u);
		if ((int16_t)FRAME_ANIMATION(animation)->last_frame < (int16_t)next_frame)
			next_frame = 0u;
		OBJECT(current_object)->animation_frame = next_frame;
		frame_entry = animation + 2u + next_frame * 4u;
		part_frame = PROPELLER_MACHINE_ANIMATION_STEP(frame_entry)->parts_6_9_frame;
		mem_write16(OBJECT(current_object)->part_list + 0x22u, PROPELLER_MACHINE_ANIMATION_STEP(frame_entry)->part_4_frame);
		mem_write16(OBJECT(current_object)->part_list + 0x32u, part_frame);
		mem_write16(OBJECT(current_object)->part_list + 0x4Au, part_frame);
	}
	if (OBJECT(current_object)->y_int >= 0x1BB0)
		geyser_front_start = 0xFFu;
	if (OBJECT(current_object)->y_int >= 0x1B00)
		OBJECT(current_object)->hitbox_height = 0x68u;
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		if (OBJECT(current_object)->screen_x < -0x80)
			OBJECT(current_object)->propeller_machine.sound_timer = 0u;
		else
		{
			sound_timer = OBJECT(current_object)->propeller_machine.sound_timer;
			OBJECT(current_object)->propeller_machine.sound_timer = sound_timer - 1u;
			if ((int16_t)sound_timer <= 1)
			{
				OBJECT(current_object)->propeller_machine.sound_timer = 0x3Cu;
				sound_id = 0xBu;
				play_sound(sound_id, &ignored_2, &ignored_3);
			}
		}
	}
	find_floor(&ignored_4, &ignored_2, &ignored_3, &ignored, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored_4, &ignored_2);
	add_to_zone_list(ignored_4);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Sets up stage 0x10's barred gate: sprite 0x9F00 (palette 0x92F0) at height 0x158, with
   barred_gate_animation ready at 2 frames a step; handler barred_gate_update. */
void barred_gate_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_frame = 0x9F00u;
	OBJECT(current_object)->sprite_palette = 0x92F0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x49;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = barred_gate_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0x1580000;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->handler = barred_gate_update_ADDRESS;
}

/* The barred gate: removed once off screen (margin 0x50); once barred_gate_closing is set it plays
   barred_gate_animation once, then sound 0x29 (animation_frame 0xFF stops it). Queued at screen_y -
   0x40. */
void barred_gate_update(void)
{
	uint8_t sound_id = 0;
	uint16_t margin = 0;
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	margin = 0x50u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0 && barred_gate_closing != 0u)
	{
		animate(&ignored, &ignored_2, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->animation_frame = 0xFFu;
			sound_id = 0x29u;
			play_sound(sound_id, &ignored, &ignored_3);
		}
	}
	update_screen_position(&ignored_4, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x40u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Sets up stage 0x10's scene anchor from scene_anchor_positions[sprite_frame]: x, y, floor height and
   end_position; scene_object = it; handler scene_anchor_update. */
void scene_anchor_init(void)
{
	uint32_t record = 0;

	record = OBJECT(current_object)->sprite_frame * 8u + scene_anchor_positions_ADDRESS;
	OBJECT(current_object)->x_int = OBJECT_START_PLACE(record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->y_int = OBJECT_START_PLACE(record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	record += 2u;
	OBJECT(current_object)->floor_z_int = OBJECT_START_PLACE(record - 0x4u)->floor_z;
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->scene_anchor.end_position = OBJECT_START_PLACE(record - 0x4u)->end_position;
	OBJECT(current_object)->handler = scene_anchor_update_ADDRESS;
	scene_object = current_object;
}

/* Stage 0x10's scene anchor (scene_object), removed at end_position: every frame it sets its z so that
   at its x/y it sits at screen y 0xA0; the players caught in game mode 0x19 (state 0x30) ease their x
   and z toward it. */
void scene_anchor_update(void)
{
	uint32_t floor_z = 0, ignored = 0, point_x = 0, point_y = 0;

	if (OBJECT(current_object)->scene_anchor.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored, &point_x, &point_y);
	OBJECT(current_object)->z_int = 0x70u + view_y - (point_y + view_y - OBJECT(current_object)->floor_z_int) + 0x30u;
	OBJECT(current_object)->z_fraction = 0u;
}

/* Sets up stage 0x10's railing spawner at height 0x520, last_spawn_position 0; handler
   railing_spawner_update. */
void railing_spawner_init(void)
{
	OBJECT(current_object)->railing_spawner.last_spawn_position = 0u;
	OBJECT(current_object)->z = 0x5200000;
	OBJECT(current_object)->handler = railing_spawner_update_ADDRESS;
}

/* Stage 0x10's railing spawner, removed at scroll_position 0x5000: every 0x800 scroll units, with 0xA
   free objects, it lays 10 railing segments (multi-part railing_parts, handler railing_segment_start) at
   its x and z, 0xB8 apart in y, carrying on from where the last ten ended. The z of 0x528 each first
   gets is overwritten with the spawner's. */
void railing_spawner_update(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t segments_left = 0;

	if (scroll_position >= 0x5000u)
	{
		delete_current_object();
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->railing_spawner.last_spawn_position) < 0x800 || free_object_stack < 0xA)
		return;
	OBJECT(current_object)->railing_spawner.last_spawn_position = scroll_position;
	railing_segments_left = 0xA;
	OBJECT(current_object)->y_int += railing_segments_left * 0xB8u;
	railing_segment_y = OBJECT(current_object)->y;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0xD002u;
		OBJECT(object)->part_list = railing_parts_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 4;
		OBJECT(object)->handler = railing_segment_start_ADDRESS;
		OBJECT(object)->z = 0x5280000;
		railing_segment_y_int -= 0xB8u;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = railing_segment_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		segments_left = railing_segments_left;
		railing_segments_left = segments_left - 1u;
	} while ((int16_t)segments_left > 1);
}

/* Places a railing segment on screen (project_object) and hands it to railing_segment_draw from the next
   frame. */
void railing_segment_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = railing_segment_draw_ADDRESS;
}

/* A railing segment: removed at scroll_position 0x6700 or once it has left the screen (margin 0x88);
   queued at screen_y + 0x40 while on screen. */
void railing_segment_draw(void)
{
	uint16_t margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	if (scroll_position >= 0x6700u)
	{
		delete_current_object();
		return;
	}
	margin = 0x88u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &ignored_2);
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x1C8u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x40u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* A rope bridge plank (its parent the group leader), every 4th with two rope parts. It sags: z eases a
   quarter of the way a frame (in speed_x) to its floor plus bridge_0_sag/bridge_1_sag[plank_index] (in
   previous_floor_z). Once the leader passes it along direction, or rope_bridge_collapsing, it falls
   tumbling; past screen y 0x1C0 it frees its ropes and goes. */
void rope_bridge_plank_update(void)
{
	uint16_t child = 0;
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0, object_2 = 0, plank_index = 0, rope_offsets = 0, ropes_left = 0, sag = 0, sag_table = 0;

	switch (OBJECT(current_object)->rope_bridge_plank.state)
	{
	case 0:
	case 3:
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->child = 0xFFFFu;
		if ((OBJECT(current_object)->rope_bridge_plank.plank_index & 3u) == 0u && free_object_stack_2[0] >= 2)
		{
			ropes_left = 2u;
			object = current_object;
			rope_offsets = rope_bridge_rope_offsets_ADDRESS;
			if (OBJECT(current_object)->kind == 0x30u)
				rope_offsets = wide_rope_bridge_rope_offsets_ADDRESS;
			do
			{
				allocate_object_2(&ignored, &ignored_3, &object_2);
				OBJECT(object_2)->sprite_flags = 0x1004u;
				OBJECT(object_2)->sprite_palette = 0x8290u;
				OBJECT(object_2)->sprite_frame = 0x9D18u;
				OBJECT(object_2)->projected_x = ROPE_BRIDGE_ROPE_OFFSET(rope_offsets)->dx;
				rope_offsets += 2u;
				OBJECT(object_2)->projected_y_fraction = ROPE_BRIDGE_ROPE_OFFSET(rope_offsets - 0x2u)->dy;
				OBJECT(object)->child = object_2;
				OBJECT(object_2)->parent = object;
				object = (object & ~0xFFFFu) | (uint16_t)object_2;
				rope_offsets += 2u;
				ropes_left--;
			} while (ropes_left != 0u);
		}
		OBJECT(current_object)->rope_bridge_plank.state++;
	case 1:
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		plank_index = OBJECT(current_object)->rope_bridge_plank.plank_index;
		sag_table = bridge_0_sag_ADDRESS;
		if (OBJECT(current_object)->rope_bridge_plank.bridge != 0u)
			sag_table = bridge_1_sag_ADDRESS;
		sag = mem_read16(sag_table + plank_index * 2u);
		if ((sag & ~0xFFu) == 0u)
			sag = 0u;
		OBJECT(current_object)->previous_floor_z += sag << 8;
		OBJECT(current_object)->speed_x += (int32_t)((uint32_t)OBJECT(current_object)->previous_floor_z - OBJECT(current_object)->speed_x) >> 2;
		OBJECT(current_object)->z_int = OBJECT(current_object)->speed_x_int;
		update_screen_position(&ignored, &ignored_2);
		if (rope_bridge_collapsing != 0u)
		{
			rope_bridge_plank_start_falling();
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
				rope_bridge_plank_start_falling();
				return;
			}
			rope_bridge_plank_check_rider_and_draw();
			return;
		case 1:
			if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			{
				rope_bridge_plank_start_falling();
				return;
			}
			rope_bridge_plank_check_rider_and_draw();
			return;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
			{
				rope_bridge_plank_start_falling();
				return;
			}
			rope_bridge_plank_check_rider_and_draw();
			return;
		case 3:
			if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
			{
				rope_bridge_plank_start_falling();
				return;
			}
			rope_bridge_plank_check_rider_and_draw();
			return;
		default:
			invalid_jump(0x93BC7u + OBJECT(current_object)->direction * 2u);
		}
		break;
	case 2:
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->animation_timer >= 0)
		{
			if ((OBJECT(current_object)->sprite_flags & ~0xFFFu) != 0xD000u)
			{
				animate(&ignored_2, &ignored_5, &at_last_step);
				if ((int8_t)at_last_step < 0)
					OBJECT(current_object)->animation_timer = -1;
			}
			else if ((int32_t)OBJECT(current_object)->part_list >= 0x205000)
			{
				animate_all_parts(&at_last_step, &ignored_4);
				if ((int8_t)at_last_step < 0)
					OBJECT(current_object)->animation_timer = -1;
			}
		}
		update_screen_position(&ignored, &ignored_2);
		if (OBJECT(current_object)->screen_y <= 0x1C0)
		{
			draw_rope_bridge_ropes();
			ignored_2 = queue_sprite_at_floor();
			return;
		}
		if ((int16_t)OBJECT(current_object)->child >= 0)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			child = OBJECT(current_object)->child;
			free_object_2(child);
		}
		delete_current_object();
		return;
	default:
		invalid_jump(0x93AEAu + OBJECT(current_object)->rope_bridge_plank.state * 3u);
	}
}

/* Starts a plank's fall: the next state, the tumbling animation from its start (2 frames a step),
   speed_z 0; the bridge's last plank clears its plank count (bridge_0_plank_count or
   bridge_1_plank_count: no more sag); sound 0x28 when on screen; then
   rope_bridge_plank_check_rider_and_draw. */
void rope_bridge_plank_start_falling(void)
{
	uint8_t sound_id = 0;
	uint16_t last_plank = 0;
	uint32_t ignored = 0, ignored_2 = 0, plank_count_address = 0;

	OBJECT(current_object)->rope_bridge_plank.state++;
	last_plank = bridge_0_last_plank;
	plank_count_address = bridge_0_plank_count_ADDRESS;
	if (OBJECT(current_object)->rope_bridge_plank.bridge != 0u)
	{
		last_plank = bridge_1_last_plank;
		plank_count_address = bridge_1_plank_count_ADDRESS;
	}
	if (last_plank == (uint16_t)current_object)
		mem_write16(plank_count_address, 0u);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	if ((uint16_t)OBJECT(current_object)->screen_x >= 0x200u)
	{
		rope_bridge_plank_check_rider_and_draw();
		return;
	}
	sound_id = 0x28u;
	play_sound(sound_id, &ignored, &ignored_2);
	rope_bridge_plank_check_rider_and_draw();
}

/* Draws a plank on screen: platform list, its ropes, and queued at its floor's screen y + depth_offset
   (kept in draw_depth). A plank whose takes_slot is negative (none of stage 0x10's) first watches for a
   kind 0xF rider: the half it stands on marks the plank's group_slots byte, and a marked plank becomes
   record 0's normal plank on that half. */
void rope_bridge_plank_check_rider_and_draw(void)
{
	uint8_t slot_mark = 0;
	uint32_t hitbox_record = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0, odd_plank_animation = 0, slot_address = 0, sprite_depth = 0;

	if (OBJECT(current_object)->rope_bridge_plank.takes_slot >= 0)
	{
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		draw_rope_bridge_ropes();
		sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->rope_bridge_plank.depth_offset);
		OBJECT(current_object)->draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	}
	if ((OBJECT(current_object)->flags & 8u) != 0u)
	{
		object_2 = LINKED(current_object, OBJECT(current_object)->platform);
		if (OBJECT(object_2)->kind == 0xFu)
		{
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 1:
			case 4:
				slot_mark = 0xFFu;
				if ((int16_t)((OBJECT(current_object)->hitbox_size_x >> 1) + OBJECT(current_object)->x_int) <= OBJECT(object_2)->x_int)
					slot_mark = 0x80u;
				break;
			case 2:
			case 3:
				slot_mark = 0xFFu;
				if ((int16_t)((OBJECT(current_object)->hitbox_size_y >> 1) + OBJECT(current_object)->y_int) <= OBJECT(object_2)->y_int)
					slot_mark = 0x80u;
				break;
			default:
				invalid_jump(0x93C6Cu + OBJECT(current_object)->direction * 2u);
			}
			RAM8(group_slots_ADDRESS + OBJECT(current_object)->rope_bridge_plank.group_slot) = slot_mark;
		}
	}
	slot_address = group_slots_ADDRESS + OBJECT(current_object)->rope_bridge_plank.group_slot;
	if ((int8_t)mem_read8(slot_address) >= 0)
	{
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		draw_rope_bridge_ropes();
		sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->rope_bridge_plank.depth_offset);
		OBJECT(current_object)->draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored, &sprite_depth);
		return;
	}
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 1:
	case 4:
		OBJECT(current_object)->x_int += OBJECT(current_object)->hitbox_size_x >> 1;
		OBJECT(current_object)->floor_block += 0x16u;
		if (mem_read8(slot_address) != 0xFFu)
		{
			OBJECT(current_object)->rope_bridge_plank.depth_offset = (uint32_t)OBJECT(current_object)->rope_bridge_plank.depth_offset << 1;
			OBJECT(current_object)->floor_block += 0x16u;
		}
		break;
	case 2:
	case 3:
		OBJECT(current_object)->floor_block += 0x16u;
		if (mem_read8(slot_address) == 0xFFu)
		{
			OBJECT(current_object)->y_int += OBJECT(current_object)->hitbox_size_y >> 1;
			OBJECT(current_object)->floor_block += 0x16u;
			OBJECT(current_object)->rope_bridge_plank.depth_offset -= 0x28u;
		}
		break;
	default:
		invalid_jump(0x93CD1u + OBJECT(current_object)->direction * 2u);
	}
	OBJECT(current_object)->sprite_flags = stage_object_records_2;
	OBJECT(current_object)->sprite_frame = rope_plank_frame;
	OBJECT(current_object)->sprite_palette = rope_plank_palette;
	OBJECT(current_object)->animation = rope_plank_animation;
	odd_plank_animation = rope_plank_animation_2;
	hitbox_record = rope_plank_hitbox_size_ADDRESS;
	if ((OBJECT(current_object)->rope_bridge_plank.plank_index & 1u) != 0u)
		OBJECT(current_object)->animation = odd_plank_animation;
	OBJECT(current_object)->hitbox_size_x = HITBOX_SIZE_PAIR(hitbox_record)->size_x;
	OBJECT(current_object)->hitbox_size_y = HITBOX_SIZE_PAIR(hitbox_record)->size_y;
	OBJECT(current_object)->rope_bridge_plank.takes_slot = 1;
	if ((int16_t)OBJECT(current_object)->child >= 0)
	{
		object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object)->projected_x = rope_bridge_rope_offsets;
		OBJECT(object)->projected_y_fraction = rope_bridge_rope_1_dy;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->projected_x = rope_bridge_rope_2_dx;
		OBJECT(object)->projected_y_fraction = rope_bridge_rope_2_dy;
	}
	project_object(&ignored, &ignored_2);
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
		return;
	add_to_platform_list();
	draw_rope_bridge_ropes();
	sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->rope_bridge_plank.depth_offset);
	OBJECT(current_object)->draw_depth = sprite_depth;
	queue_sprite_at_depth(&ignored, &sprite_depth);
}

/* Draws a plank's two rope parts, if it has them, at its screen position plus their offsets (kept in
   projected_x and projected_y_fraction), slack (sprite 0x9D20) while it falls; on a plank that takes a
   group slot, the slot's mark hides one: 0xFF the first rope, 0x80 the second. */
void draw_rope_bridge_ropes(void)
{
	uint32_t ignored = 0, ropes_left = 0, slot_address = 0;

	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	ropes_left = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if ((uint8_t)OBJECT(object_cursor)->draw_rope_bridge_ropes.takes_slot == 0u)
		{
			if (OBJECT(object_cursor)->draw_rope_bridge_ropes.state == 2u)
				OBJECT(current_object)->sprite_frame = 0x9D20u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x + OBJECT(current_object)->projected_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y + OBJECT(current_object)->projected_y_fraction;
			ignored = queue_sprite(ignored);
		}
		else
		{
			slot_address = group_slots_ADDRESS + OBJECT(object_cursor)->draw_rope_bridge_ropes.group_slot;
			if ((uint16_t)ropes_left == 1u)
			{
				if (mem_read8(slot_address) != 0x80u)
				{
					if (OBJECT(object_cursor)->draw_rope_bridge_ropes.state == 2u)
						OBJECT(current_object)->sprite_frame = 0x9D20u;
					OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x + OBJECT(current_object)->projected_x;
					OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y + OBJECT(current_object)->projected_y_fraction;
					ignored = queue_sprite(ignored);
				}
			}
			else if (mem_read8(slot_address) != 0xFFu)
			{
				if (OBJECT(object_cursor)->draw_rope_bridge_ropes.state == 2u)
					OBJECT(current_object)->sprite_frame = 0x9D20u;
				OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x + OBJECT(current_object)->projected_x;
				OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y + OBJECT(current_object)->projected_y_fraction;
				ignored = queue_sprite(ignored);
			}
		}
		ropes_left--;
	} while (ropes_left != 0u);
	current_object = object_cursor;
}

/* Game mode 0x19's stage frame: while rope_bridge_collapsing is set the scroll runs at speed 0x1E
   (scroll_mode_low 0); before, from scroll_position 0xDA0 the scroll stops and, with a player in play,
   scene_timer counts to 0x78 and sets rope_bridge_collapsing. Then the stage frame; from scroll_position
   0x1A00 game_mode 3 at scroll speed 5. */
void event_19_check_rope_bridge(uint32_t position, uint32_t in_play_count, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	if (rope_bridge_collapsing != 0u)
	{
		scroll_mode_low = 0u;
		scroll_speed_low = 0x1E;
	}
	else
	{
		if (scroll_position < 0xDA0u)
		{
			update_scroll(&position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
			update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
			find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored, p_count, &ignored_2, &object);
			*p_speed_value = update_restart_point(*p_speed_value);
			spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, &object);
			spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
			spawn_walls(&ignored_3, &position, &ignored, p_count, p_cycle, &object);
			spawn_object_groups_2(&ignored_3, p_count, &object);
			spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
			spawn_objects_by_handler(&ignored_3, &ignored_2, &object);
			ignored_3 = check_zones(ignored_3);
			check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
			check_walls(ignored_3);
			check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
			if (scroll_position < 0x1A00u)
			{
				return;
			}
			game_mode = 3u;
			scroll_speed_low = 5;
			return;
		}
		if (no_player_in_play == 0u)
		{
			scene_timer++;
			if (scene_timer >= 0x78)
				rope_bridge_collapsing = 0xFFu;
		}
		scroll_mode_low = 1u;
	}
	update_scroll(&position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored, p_count, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, &object);
	spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
	spawn_walls(&ignored_3, &position, &ignored, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored_3, p_count, &object);
	spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
	spawn_objects_by_handler(&ignored_3, &ignored_2, &object);
	ignored_3 = check_zones(ignored_3);
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
	if (scroll_position < 0x1A00u)
	{
		return;
	}
	game_mode = 3u;
	scroll_speed_low = 5;
}

/* The stage frame of game modes 0x12 and 0x13: next_stage_slot_request 0 and update_player_summary; with
   no player left (player_status all 0) game_mode 7 runs at once, game_mode saved; with
   event_12_fast_scroll set scroll mode 7 becomes mode 0 at speed 0x20; then the scroll, tiles, floors,
   spawners and contacts. */
void event_12_stage_frame(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, p_count, p_list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	if (scroll_mode == 7u && event_12_fast_scroll != 0u)
	{
		scroll_mode = 0u;
		scroll_speed = 0x20u;
	}
	update_scroll(&position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored, p_count, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, &object);
	spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
	spawn_walls(&ignored_3, &position, &ignored, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored_3, p_count, &object);
	spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
	spawn_objects_by_handler(&ignored_3, &ignored_2, &object);
	ignored_3 = check_zones(ignored_3);
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
}
