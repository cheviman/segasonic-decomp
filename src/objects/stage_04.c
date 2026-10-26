/**
 *    Name:        objects/stage_04.c
 *    Description: stage 4's objects: the vines, the catapults, arrow boards, pebbles, the propped board, boulders, the door wall at the start and the rollers behind it, the fireball emitter and the bouncing fireballs, the electric fence and the flame machine at the end
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Sets up a vine: segment_count - 1 pieces (sprite 0x6E68), then the bar, a two-part object (sprite
   0x6E70, kind 0xA, vine_bar_update, its part the shadow 0x5D90) chained after them, current_player
   serving as the chain's last link; speed = phase_step << 16, base_x = x, depth_offset = segment_count *
   8, floor_z = z; then vine_swing. */
void vine_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0, object_3 = 0, spawned = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->speed_z = 0x3FF;
	OBJECT(current_object)->vine_init.base_x = OBJECT(current_object)->x;
	OBJECT(current_object)->speed = (uint32_t)OBJECT(current_object)->vine_init.phase_step << 16;
	OBJECT(current_object)->speed_x_int = OBJECT(current_object)->vine_init.segment_count;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->vine_init.depth_offset = OBJECT(current_object)->speed_x_int * 8u;
	spawn_count = OBJECT(current_object)->speed_x_int;
	spawn_count--;
	add_pieces_to_current_object(&spawned, &free_count, &object_2, &block);
	if ((uint8_t)spawned == 0u)
		return;
	object_3 = block;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00ECFFu, &spawned, &ignored, &free_count, &object_2, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(object_3)->child = object_2;
	OBJECT(object_2)->parent = object_3;
	OBJECT(object_2)->vine_bar.vine = object_cursor;
	OBJECT(object_2)->hitbox_size_x = 0x14u;
	OBJECT(object_2)->hitbox_height = 8u;
	OBJECT(object_2)->kind = 0xAu;
	OBJECT(object_2)->vine_bar.anchor_z = OBJECT(object_cursor)->z_int;
	OBJECT(object_2)->handler = vine_bar_update_ADDRESS;
	object_2 = LINKED(object_2, OBJECT(object_2)->child);
	OBJECT(object_2)->sprite_scale = 0x100u;
	OBJECT(object_2)->sprite_frame = 0x5D90u;
	OBJECT(object_2)->sprite_palette = 0x7A0u;
	OBJECT(object_2)->sprite_flags = 8u;
	OBJECT(current_object)->handler = vine_swing_ADDRESS;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	current_player = current_object;
	object = LINKED(block, OBJECT(current_object)->child);
	do
	{
		OBJECT(object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8350u;
		OBJECT(object)->sprite_frame = 0x6E68u;
		OBJECT(current_player)->child = object;
		OBJECT(object)->parent = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)object;
		object = LINKED(object, OBJECT(object)->child);
		speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
		OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
	OBJECT(current_player)->sprite_frame = 0x6E70u;
	OBJECT(current_player)->sprite_palette = 0x10E0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored_2, &ignored);
}

/* Swings the vine: its phase (speed_z) grows by speed, the curl speed_y = sine_table[phase] *
   curl_scale; each segment steps 0x18 * the sine and cosine of an angle growing by the curl in x and z,
   leaving the vine's x and z at the tip. holding is 0 only at phase 0x9F-0x110 without
   vines_hold_players. Deleted at screen_x +-0x200 (vines_hold_players = 0xFF). */
void vine_swing(void)
{
	uint8_t bucket = 0;
	uint16_t screen_x = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, in_front = 0, point_x = 0, point_y = 0, segment_angle_int = 0, segment_depth = 0, z = 0;
	uint16_t speed_x_fraction = 0;

	OBJECT(current_object)->vine_swing.holding = 1u;
	if (vines_hold_players == 0u && OBJECT(current_object)->speed_z_int >= 0x9F && OBJECT(current_object)->speed_z_int <= 0x110)
		OBJECT(current_object)->vine_swing.holding = 0u;
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	if ((int16_t)screen_x >= 0x200 || (int16_t)screen_x <= -0x200)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		vines_hold_players = 0xFFu;
		return;
	}
	OBJECT(current_object)->speed_z += OBJECT(current_object)->speed;
	OBJECT(current_object)->speed_z_int &= 0x3FFu;
	OBJECT(current_object)->speed_y = (uint32_t)sine_table[(uint16_t)OBJECT(current_object)->speed_z_int] * OBJECT(current_object)->vine_swing.curl_scale;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	update_screen_position(&ignored, &ignored_4);
	in_front = 0u;
	if (OBJECT(object_cursor)->vine_swing.segment_angle_int >= 0x100 && OBJECT(object_cursor)->vine_swing.segment_angle_int < 0x300)
		in_front = 1u;
	OBJECT(current_object)->vine_swing.segment_angle = 0u;
	OBJECT(current_object)->x = OBJECT(current_object)->vine_swing.base_x;
	OBJECT(current_object)->speed_x_fraction = OBJECT(current_object)->speed_x_int;
	OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_cursor)->vine_swing.segment_angle += OBJECT(object_cursor)->speed_y;
		OBJECT(object_cursor)->vine_swing.segment_angle_int &= 0x3FFu;
		segment_angle_int = (uint16_t)OBJECT(object_cursor)->vine_swing.segment_angle_int;
		OBJECT(object_cursor)->x += sine_table[segment_angle_int] * 0x18u;
		point_x = OBJECT(object_cursor)->x;
		OBJECT(object_cursor)->z += cosine_table[segment_angle_int] * 0x18u;
		z = OBJECT(object_cursor)->z;
		point_y = OBJECT(object_cursor)->y;
		project_point(z, &ignored_4, &point_x, &point_y);
		OBJECT(current_object)->screen_x = point_x;
		OBJECT(current_object)->screen_y = point_y;
		if (in_front == 0u)
		{
			segment_depth = (uint16_t)(OBJECT(object_cursor)->screen_y - (OBJECT(object_cursor)->x_int - OBJECT(object_cursor)->vine_swing.base_x_int) + OBJECT(object_cursor)->vine_swing.depth_offset);
			queue_sprite_at_depth(&ignored, &segment_depth);
		}
		else
		{
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		speed_x_fraction = OBJECT(object_cursor)->speed_x_fraction;
		OBJECT(object_cursor)->speed_x_fraction = speed_x_fraction - 1u;
	} while ((int16_t)speed_x_fraction > 1);
}

/* The bar at the vine's tip: the vine's x, y, z and holding, moved to match where vine_swing drew it;
   touchable and in the rider list (players jumping into it are held until holding is 0), its height
   reaching its floor (up to 0x50). Its shadow while on a platform; "CATCH!!" (spawn_sign kind 7) while
   holding is 1 over a floor. */
void vine_bar_update(void)
{
	uint16_t kind = 0, screen_dy = 0, screen_x = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t cursor = 0, height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, vine = 0;

	OBJECT(current_object)->vine_bar.holding = OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->vine_bar.vine))->vine_swing.holding;
	if ((OBJECT(current_object)->flags & 8u) != 0u && OBJECT(current_object)->floor_z_int < 0x800)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int;
		queue_sprite(0u);
		current_object = object_cursor;
	}
	vine = LINKED(WORK_RAM, OBJECT(current_object)->vine_bar.vine);
	OBJECT(current_object)->x = OBJECT(vine)->x;
	OBJECT(current_object)->y = OBJECT(vine)->y;
	OBJECT(current_object)->z = OBJECT(vine)->z;
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	screen_dy = (uint16_t)OBJECT(current_object)->screen_y;
	project_object(&ignored_2, &ignored);
	screen_dy -= OBJECT(current_object)->screen_y;
	OBJECT(current_object)->projected_x += screen_x - OBJECT(current_object)->screen_x;
	OBJECT(current_object)->projected_y_int += screen_dy;
	unproject_object(&ignored_2, &ignored_3, &ignored_4, &cursor);
	height = (OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int) & 0x7Fu;
	if ((int16_t)height > 0x50)
	{
		OBJECT(object_cursor)->hitbox_centre_y = 0;
		OBJECT(object_cursor)->hitbox_height = 8u;
	}
	else
	{
		OBJECT(object_cursor)->hitbox_centre_y = height;
		OBJECT(object_cursor)->hitbox_height = height;
	}
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(height);
	add_to_rider_list();
	current_object = object_cursor;
	if (OBJECT(current_object)->floor_z_int < 0x800 && OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->vine_bar.vine))->vine_swing.holding == 1u)
	{
		x_int = (uint16_t)OBJECT(current_object)->x_int;
		y_int = (uint16_t)OBJECT(current_object)->y_int;
		kind = 7u;
		z = (uint16_t)(OBJECT(current_object)->z_int + 0x14u);
		spawn_sign(x_int, y_int, kind, z, &ignored, &cursor);
	}
}

/* The invisible scroll stop at the vines: sets vines_hold_players, so held players cannot let go of the
   bars, and starts vine_scroll_stop_wait. */
void vine_scroll_stop_init(void)
{
	vines_hold_players = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_wait_ADDRESS;
	vine_scroll_stop_wait();
}

/* Stops the scroll (scroll_mode and saved_scroll_mode 1) once scroll_position reaches spawn_arg_1 and
   goes on to vine_scroll_stop_hold. Before that position it already runs vine_scroll_stop_hold every
   frame, which can restart the scroll and skip the stop. */
void vine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
	{
		vine_scroll_stop_hold();
		return;
	}
	scroll_mode = 1u;
	saved_scroll_mode = 1u;
	OBJECT(current_object)->handler = vine_scroll_stop_hold_ADDRESS;
	vine_scroll_stop_hold();
}

/* Restarts the scroll (scroll_mode and saved_scroll_mode 0, scroll_speed 8) and goes on to
   vine_scroll_stop_end once every player's object (vine_scroll_stop_player_blocks) is out (flags 0x400),
   held on a bar (flags 0x10000) or at x_int -0x120 or more; until then it runs vine_scroll_stop_end's
   check. */
void vine_scroll_stop_hold(void)
{
	uint32_t all_clear = 0, player_index = 0, player_object = 0;

	player_index = 3u;
	all_clear = 1u;
	do
	{
		player_index--;
		if (player_index == 0xFFFFFFFFu)
		{
			if (all_clear == 0u)
			{
				vine_scroll_stop_end();
				return;
			}
			scroll_mode = 0u;
			saved_scroll_mode = 0u;
			scroll_speed = 8u;
			OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
			vine_scroll_stop_end();
			return;
		}
		player_object = GAME_VARIABLES + vine_scroll_stop_player_blocks[player_index];
		player_object = LINKED(player_object, PLAYER(player_object)->object);
	} while ((OBJECT(player_object)->flags & 0x400u) != 0u || (OBJECT(player_object)->flags & 0x10000u) != 0u || OBJECT(player_object)->x_int >= -0x120);
	all_clear = 0u;
	if (all_clear == 0u)
	{
		vine_scroll_stop_end();
		return;
	}
	scroll_mode = 0u;
	saved_scroll_mode = 0u;
	scroll_speed = 8u;
	OBJECT(current_object)->handler = vine_scroll_stop_end_ADDRESS;
	vine_scroll_stop_end();
}

/* Once scroll_position passes spawn_arg_2: scroll_mode and saved_scroll_mode 7 (follow the players),
   vines_hold_players = 0, and the stop object deletes itself. */
void vine_scroll_stop_end(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if ((int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		scroll_mode = 7u;
		saved_scroll_mode = 7u;
		vines_hold_players = 0u;
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
	}
}

/* Builds the machine at the end of stage 4 as one chain from the box (sprite 0x6258, kind 0xC, health
   0x28, time_left 0xE10, flame_machine_update): its floor piece, the capsule's two pieces and shadow,
   the back wall with two rods, two arms (arm_index 0 and 1, side 1 and -1) and two cannons (spawn_arg_1
   0 and 1). flame_machine_flags = 2. */
void flame_machine_init(void)
{
	uint16_t back_wall = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	flame_machine_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x6258u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x116u;
	OBJECT(current_object)->hitbox_size_y = 0x70u;
	OBJECT(current_object)->hitbox_offset_x = -0x7F;
	OBJECT(current_object)->hitbox_offset_y = 4;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->health = 0x28;
	OBJECT(current_object)->flame_machine_init.time_left = 0xE10u;
	OBJECT(current_object)->handler = flame_machine_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6260u;
	OBJECT(current_object)->sprite_flags = 8u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6B20u;
	OBJECT(current_object)->sprite_palette = 0x9240u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6B18u;
	OBJECT(current_object)->sprite_palette = 0x9230u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6970u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	r14 = current_object;
	spawn_count = 3u;
	spawn_object_with_pieces(0x00FC8Fu, &ignored, &ignored_2, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->sprite_frame = 0x6268u;
	OBJECT(object)->sprite_palette = 0x9200u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->handler = flame_machine_back_wall_update_ADDRESS;
	back_wall = (uint16_t)object;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->sprite_flags = 5u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->sprite_flags = 5u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00FCE4u, &ignored, &ignored_2, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = back_wall;
	OBJECT(object)->spawn_arg_1 = 0xFFF5u;
	OBJECT(object)->spawn_arg_2 = 0x36u;
	OBJECT(object)->sprite_palette = 0x9220u;
	OBJECT(object)->flame_machine_arm.side = 1;
	OBJECT(object)->flame_machine_arm.arm_index = 0u;
	OBJECT(object)->handler = flame_machine_arm_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x9220u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x00FD2Eu, &ignored, &ignored_2, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = back_wall;
	OBJECT(object)->spawn_arg_1 = 0x40u;
	OBJECT(object)->spawn_arg_2 = 0x11u;
	OBJECT(object)->sprite_palette = 0x9220u;
	OBJECT(object)->flame_machine_arm.side = -1;
	OBJECT(object)->flame_machine_arm.arm_index = 1u;
	OBJECT(object)->handler = flame_machine_arm_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x9220u;
	r14 = object;
	spawn_count = 2u;
	spawn_objects(0x00FD7Au, &ignored, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x29u;
	OBJECT(object)->projected_y_int -= 0x7Eu;
	OBJECT(object)->sprite_frame = 0x61C8u;
	OBJECT(object)->sprite_palette = 0x91D0u;
	OBJECT(object)->sprite_flags = 8u;
	OBJECT(object)->spawn_arg_1 = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x11Eu;
	OBJECT(object)->hitbox_size_y = 0x30u;
	OBJECT(object)->hitbox_offset_x = -0x33;
	OBJECT(object)->hitbox_offset_y = 0x68;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = flame_machine_cannon_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x63u;
	OBJECT(object)->projected_y_int += 0x15u;
	OBJECT(object)->sprite_frame = 0x61D0u;
	OBJECT(object)->sprite_palette = 0x91D0u;
	OBJECT(object)->spawn_arg_1 = 1u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x11Eu;
	OBJECT(object)->hitbox_size_y = 0x30u;
	OBJECT(object)->hitbox_offset_x = -0x33;
	OBJECT(object)->hitbox_offset_y = 0x68;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = flame_machine_cannon_update_ADDRESS;
	flame_machine_flags |= 2u;
}

/* The box: time_left counts down while active_player_count is non-zero, and when it runs out the machine
   breaks (flame_machine_destroyed); otherwise flame_machine_draw. */
void flame_machine_update(void)
{
	uint16_t time_left = 0;

	if (active_player_count != 0u)
	{
		time_left = OBJECT(object_cursor)->flame_machine_update.time_left;
		OBJECT(object_cursor)->flame_machine_update.time_left = time_left - 1u;
		if (time_left == 0u)
		{
			flame_machine_destroyed(0u, 0u, 0u);
			return;
		}
	}
	flame_machine_draw();
}

/* Draws the box, touchable. A hit (health no longer previous_health) starts the flash:
   flame_machine_flags 0x1000, flame_machine_flash_step 4, sound 0x21, then per step a
   flame_machine_hit_palettes palette and sound 0x1E. Then its floor piece, the capsule (animated every
   other frame), its second piece and the shadow; at health 0, flame_machine_destroyed. */
void flame_machine_draw(void)
{
	uint8_t sound_id = 0;
	uint32_t block_offset = 0, bucket = 0, cursor = 0, depth = 0, ignored = 0, ignored_2 = 0, slope_height = 0;
	uint8_t flash_steps_left = 0, odd_frame = 0;
	uint16_t flash_count = 0;

	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	ignored_2 = queue_sprite(0u);
	add_to_touchable_list(ignored);
	if ((uint8_t)OBJECT(current_object)->flame_machine_update.previous_health != (uint8_t)OBJECT(current_object)->health)
	{
		OBJECT(current_object)->flame_machine_update.previous_health = OBJECT(current_object)->health;
		flame_machine_flags |= 0x1000u;
		flame_machine_flash_step = 4;
		sound_id = 0x21u;
		play_sound(sound_id, &ignored_2, &block_offset);
	}
	if ((flame_machine_flags & 0x1000u) != 0u)
	{
		flash_count = OBJECT(current_object)->flame_machine_update.flash_frame_count + 1u;
		OBJECT(current_object)->flame_machine_update.flash_frame_count = flash_count;
		if (flash_count != 0u)
		{
			flash_steps_left = flame_machine_flash_step;
			flame_machine_flash_step = flash_steps_left - 1u;
			if (flash_steps_left == 0u)
				flame_machine_flags &= ~0x1000u;
			else
			{
				OBJECT(current_object)->sprite_palette = flame_machine_hit_palettes[(uint8_t)flame_machine_flash_step];
				sound_id = 0x1Eu;
				play_sound(sound_id, &ignored_2, &block_offset);
			}
		}
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= 2u;
	OBJECT(current_object)->screen_y -= 0x40u;
	OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
	queue_sprite(ignored_2);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction_high++;
	odd_frame = OBJECT(current_object)->direction_high & 1u;
	OBJECT(current_object)->direction_high = odd_frame;
	if (odd_frame != 0u)
	{
		OBJECT(current_object)->direction_low++;
		if ((int8_t)OBJECT(current_object)->direction_low > 6)
			OBJECT(current_object)->direction_low = 0u;
		OBJECT(current_object)->sprite_frame = flame_machine_capsule_sprites[OBJECT(current_object)->direction_low];
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	depth = (uint16_t)OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y = depth;
	OBJECT(current_object)->screen_x += 0x2Du;
	OBJECT(current_object)->screen_y += 0x1Fu;
	depth += 2u;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	depth = (uint16_t)OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y = depth;
	OBJECT(current_object)->screen_x += 0x2Du;
	OBJECT(current_object)->screen_y += 0x2Bu;
	depth++;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
	if (OBJECT(object_cursor)->health <= 0)
	{
		flame_machine_destroyed(block_offset, slope_height, cursor);
		return;
	}
}

/* The machine breaks: goal_unlocked = 0xFF, flame_machine_flags 0x100000 (destroyed), health 0. Every
   8th of its destroyed_frames an explosion (flame_machine_explosion_start) at a random spot over the
   box; at 0x130, flame_machine_remove and the wreck (flame_machine_wreck_draw) at projected 0x1B2F,
   0x14F. */
void flame_machine_destroyed(uint32_t free_count, uint32_t first, uint32_t block)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, object_2 = 0, spawned = 0;
	uint16_t explosion_phase = 0;

	goal_unlocked = 0xFFu;
	flame_machine_flags |= 0x100000u;
	current_object = object_cursor;
	OBJECT(current_object)->health = 0;
	OBJECT(current_object)->flame_machine_update.destroyed_frames++;
	if (OBJECT(current_object)->flame_machine_update.destroyed_frames != 0x130u)
	{
		explosion_phase = OBJECT(current_object)->flame_machine_update.destroyed_frames & 7u;
		spawned = explosion_phase;
		if (explosion_phase == 0u)
		{
			spawn_count = 1u;
			spawn_object_with_pieces(0x00FFC1u, &spawned, &ignored, &free_count, &first, &block);
			if ((uint8_t)spawned == 0u)
				return;
			object_2 = first;
			copy_position_to(first);
			next_random(&first, &ignored_2, &ignored_3);
			first = (first & ~0xFFFFu) | (first & 0x3FFu);
			OBJECT(object_2)->sprite_scale += first;
			first = (first & ~0xFFFFu) | (first & 0x7Fu);
			OBJECT(object_2)->projected_y_int += 0x13u;
			OBJECT(object_2)->projected_y_int += first;
			next_random(&first, &ignored_2, &ignored_3);
			OBJECT(object_2)->projected_x += first & 0x7Fu;
			OBJECT(object_2)->projected_x -= 0x53u;
			OBJECT(object_2)->handler = flame_machine_explosion_start_ADDRESS;
			return;
		}
		return;
	}
	OBJECT(current_object)->handler = flame_machine_remove_ADDRESS;
	spawn_count = 3u;
	spawn_object_with_pieces(0x010820u, &ignored_4, &ignored, &free_count, &first, &block);
	OBJECT(first)->projected_x = 0x1B2F;
	OBJECT(first)->projected_y_int = 0x14F;
	OBJECT(first)->z = 0;
	OBJECT(first)->sprite_frame = 0x8360u;
	OBJECT(first)->sprite_palette = 0x87D0u;
	OBJECT(first)->handler = flame_machine_wreck_draw_ADDRESS;
	object = LINKED(first, OBJECT(first)->child);
	OBJECT(object)->sprite_frame = 0x8370u;
	OBJECT(object)->sprite_palette = 0x87E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x8368u;
	OBJECT(object)->sprite_palette = 0x87E0u;
	current_object = object_cursor;
}

/* Unused: an empty function, a lone return the original left between flame_machine_destroyed and
   flame_machine_remove. */
void flame_machine_update_spare_return(void)
{
}

/* The broken machine's last frame: electric_fence_state = 0xFF, so the electric fence's posts delete
   themselves, and the machine is deleted with all its parts. */
void flame_machine_remove(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	electric_fence_state = 0xFFu;
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* One of the breaking machine's explosions: speed 0x20, direction 0x80, 8 debris chunks (spawn_debris),
   then explode_object. */
void flame_machine_explosion_start(void)
{
	uint16_t count = 0;

	OBJECT(current_object)->speed = 0x20;
	OBJECT(current_object)->direction = 0x80u;
	count = 8u;
	spawn_debris(0u, count, 0u);
	explode_object();
}

/* The back wall: unless the machine is destroyed or an arm is active (0x10, 0x20), it steps
   flame_machine_back_wall_frames every 3 frames, placed by flame_machine_back_wall_offsets from the box,
   rods by flame_machine_rod_sprites. Frame 5 starts both arms; the cycle's end sets 4, 8 (fire), 0x400,
   0x800 (take discs), clears 0x200, pauses 0x1E frames. */
void flame_machine_back_wall_update(void)
{
	uint16_t rod_sprite = 0;
	uint32_t depth = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, offset_record = 0, wall_frame = 0;
	uint16_t next_phase = 0;

	if ((flame_machine_flags & 0x100000u) == 0u)
	{
		move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
		if ((flame_machine_flags & 0x10u) == 0u && (flame_machine_flags & 0x20u) == 0u)
		{
			if (OBJECT(current_object)->flame_machine_back_wall.pause_timer > 0)
				OBJECT(current_object)->flame_machine_back_wall.pause_timer--;
			else
			{
				OBJECT(current_object)->flame_machine_back_wall.frame_phase++;
				next_phase = OBJECT(current_object)->flame_machine_back_wall.frame_phase & 3u;
				OBJECT(current_object)->flame_machine_back_wall.frame_phase = next_phase;
				if (next_phase == 0u)
				{
					OBJECT(current_object)->flame_machine_back_wall.frame_phase = 1u;
					OBJECT(current_object)->flame_machine_back_wall.frame_index++;
					frame_index = (uint8_t)OBJECT(current_object)->flame_machine_back_wall.frame_index;
					if ((int8_t)frame_index > 9)
					{
						frame_index = 0u;
						OBJECT(current_object)->flame_machine_back_wall.frame_index = 0;
						OBJECT(current_object)->flame_machine_back_wall.pause_timer = 0x1E;
						flame_machine_flags |= 4u;
						flame_machine_flags |= 8u;
						flame_machine_flags |= 0x400u;
						flame_machine_flags |= 0x800u;
						flame_machine_flags &= ~0x200u;
					}
					wall_frame = flame_machine_back_wall_frames[frame_index];
					OBJECT(current_object)->flame_machine_back_wall.frame = wall_frame;
					rod_sprite = flame_machine_rod_sprites[wall_frame];
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(current_object)->sprite_frame = rod_sprite;
					current_object = LINKED(current_object, OBJECT(current_object)->child);
					OBJECT(current_object)->sprite_frame = rod_sprite;
					offset_record = flame_machine_back_wall_offsets_ADDRESS + wall_frame * 4u;
					current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
					OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
					OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
					OBJECT(object_cursor)->projected_x += FLAME_MACHINE_OFFSET(offset_record)->dx;
					OBJECT(object_cursor)->projected_y_int += FLAME_MACHINE_OFFSET(offset_record)->dy;
					if (wall_frame == 5u)
					{
						flame_machine_flags |= 0x10u;
						flame_machine_flags |= 0x20u;
					}
				}
			}
		}
	}
	current_object = object_cursor;
	queue_sprite(wall_frame);
	if ((flame_machine_flags & 0x100000u) != 0u)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if (OBJECT(current_object)->flame_machine_back_wall.frame == 5u)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	object = LINKED(WORK_RAM, OBJECT(object_cursor)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += 0xEu;
	OBJECT(current_object)->screen_y += 0x17u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += 0x36u;
	OBJECT(current_object)->screen_y += 3u;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Arm arm_index, not run once the machine is destroyed: bit 0x400 << arm_index gives it a disc; with bit
   0x10 << arm_index (active) it steps flame_machine_arm_frames every other frame; frame 0xC sets bit
   0x40 << arm_index and it sways (sway_step) until sway_count passes 0x26, then 0x200 (discs dropped)
   and flame_machine_arm_rest; frame 0xD ends. */
void flame_machine_arm_update(void)
{
	uint32_t block_offset = 0, cursor = 0, frame_number = 0, ignored = 0, slope_height = 0;
	uint8_t odd_sway_frame = 0;
	uint16_t odd_frame = 0;

	if ((flame_machine_flags & 0x100000u) != 0u)
		return;
	unproject_object(&ignored, &block_offset, &slope_height, &cursor);
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 0xAu))) != 0u)
		flame_machine_arm_take_disc(block_offset, slope_height, cursor);
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 4u))) == 0u)
	{
		flame_machine_arm_draw();
		return;
	}
	if ((flame_machine_flags & shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u))) == 0u)
	{
		OBJECT(current_object)->flame_machine_arm.frame_toggle++;
		odd_frame = OBJECT(current_object)->flame_machine_arm.frame_toggle & 1u;
		OBJECT(current_object)->flame_machine_arm.frame_toggle = odd_frame;
		if (odd_frame != 0u)
		{
			flame_machine_arm_draw();
			return;
		}
		OBJECT(current_object)->flame_machine_arm.frame_index++;
		frame_number = flame_machine_arm_frames[OBJECT(current_object)->flame_machine_arm.frame_index];
		if (frame_number != 0xCu)
		{
			if (frame_number != 0xDu)
			{
				OBJECT(current_object)->sprite_frame = flame_machine_arm_sprites[frame_number];
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				OBJECT(current_object)->sprite_frame = flame_machine_arm_part_sprites[frame_number];
				current_object = object_cursor;
				flame_machine_arm_draw();
				return;
			}
			OBJECT(current_object)->flame_machine_arm.frame_index = 0u;
			flame_machine_flags &= ~shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 4u));
			flame_machine_arm_draw();
			return;
		}
		flame_machine_flags |= shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u));
		OBJECT(current_object)->sprite_frame = flame_machine_arm_sprites[frame_number];
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = flame_machine_arm_part_sprites[frame_number];
		current_object = object_cursor;
		flame_machine_arm_draw();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_toggle++;
	odd_sway_frame = OBJECT(current_object)->flame_machine_arm.sway_toggle & 1u;
	OBJECT(current_object)->flame_machine_arm.sway_toggle = odd_sway_frame;
	if (odd_sway_frame != 0u)
	{
		flame_machine_arm_draw();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_count++;
	if (OBJECT(current_object)->flame_machine_arm.sway_count > 0x26)
	{
		flame_machine_flags |= 0x200u;
		OBJECT(current_object)->flame_machine_arm.rest_timer = 0x1E;
		OBJECT(current_object)->handler = flame_machine_arm_rest_ADDRESS;
		flame_machine_arm_rest();
		return;
	}
	OBJECT(current_object)->flame_machine_arm.sway_step++;
	OBJECT(current_object)->flame_machine_arm.sway_step = OBJECT(current_object)->flame_machine_arm.sway_step % 0x29u;
	OBJECT(current_object)->flame_machine_arm.sway_count++;
	flame_machine_arm_draw();
}

/* Holds the arm for rest_timer (0x1E) frames, then clears its sway bit in flame_machine_flags (0x40 <<
   arm_index) and goes back to flame_machine_arm_update. */
void flame_machine_arm_rest(void)
{
	OBJECT(current_object)->flame_machine_arm.rest_timer--;
	if (OBJECT(current_object)->flame_machine_arm.rest_timer > 0)
	{
		flame_machine_arm_draw();
		return;
	}
	flame_machine_flags &= ~shl32(1u, (int)(OBJECT(current_object)->flame_machine_arm.arm_index + 6u));
	OBJECT(current_object)->flame_machine_arm.frame_toggle = 1u;
	OBJECT(current_object)->handler = flame_machine_arm_update_ADDRESS;
	flame_machine_arm_draw();
}

/* Places the arm at the back wall's projected position plus (spawn_arg_1, spawn_arg_2) and its sway
   offset (flame_machine_arm_sway_offsets[sway_step]) times side; queues it at the depth of the second
   arm's screen_y - 1, and its part at its screen position in bucket arm_index * 4. */
void flame_machine_arm_draw(void)
{
	uint8_t bucket = 0;
	uint16_t side = 0, sway_dy = 0;
	uint32_t depth = 0, ignored = 0, object = 0, sway_offset = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->projected_x += OBJECT(object_cursor)->spawn_arg_1;
	OBJECT(object_cursor)->projected_y_int += OBJECT(object_cursor)->spawn_arg_2;
	current_object = object_cursor;
	sway_offset = flame_machine_arm_sway_offsets_ADDRESS + OBJECT(current_object)->flame_machine_arm.sway_step * 2u;
	side = (uint16_t)OBJECT(current_object)->flame_machine_arm.side;
	sway_dy = (uint16_t)((uint32_t)FLAME_MACHINE_ARM_SWAY(sway_offset)->dy * side);
	OBJECT(current_object)->projected_x += (uint32_t)FLAME_MACHINE_ARM_SWAY(sway_offset)->dx * side;
	OBJECT(current_object)->projected_y_int += sway_dy;
	object = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	object = LINKED(object, OBJECT(object)->child);
	depth = (uint16_t)(OBJECT(LINKED(object, OBJECT(object)->child))->screen_y - 1u);
	r14 = depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = (uint8_t)((uint32_t)OBJECT(object_cursor)->flame_machine_arm.arm_index << 2);
	queue_sprite_in_bucket(bucket);
}

/* A cannon (spawn_arg_1 0 or 1): drawn, touchable (health 0x7F), flashing with the box
   (flame_machine_cannon_hit_palettes[flame_machine_flash_step]). When its fire bit (4 << spawn_arg_1) is
   set it spawns a flame (sprite 0x6910, kind 0x17) at flame_machine_flame_offsets[spawn_arg_1] with
   flame_machine_flame_burn and clears the bit. */
void flame_machine_cannon_update(void)
{
	uint32_t block_offset = 0, cursor = 0, fire_bit_or_spawned = 0, ignored = 0, ignored_2 = 0, object = 0, offset_record = 0, spawn_arg_1 = 0;

	unproject_object(&ignored, &block_offset, &object, &cursor);
	queue_sprite(0u);
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
	if (flame_machine_flash_step >= 0)
		OBJECT(current_object)->sprite_palette = flame_machine_cannon_hit_palettes[(uint8_t)flame_machine_flash_step];
	fire_bit_or_spawned = OBJECT(current_object)->spawn_arg_1 + 2u;
	if ((flame_machine_flags & shl32(1u, (int)(fire_bit_or_spawned))) == 0u)
		return;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010440u, &fire_bit_or_spawned, &ignored_2, &block_offset, &object, &cursor);
	if ((uint8_t)fire_bit_or_spawned != 0u)
	{
		copy_position_to(object);
		spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
		offset_record = flame_machine_flame_offsets_ADDRESS + spawn_arg_1 * 4u;
		OBJECT(object)->direction = flame_machine_flame_directions[spawn_arg_1];
		OBJECT(object)->projected_x += FLAME_MACHINE_OFFSET(offset_record)->dx;
		OBJECT(object)->projected_y_int += FLAME_MACHINE_OFFSET(offset_record)->dy;
		OBJECT(object)->sprite_frame = 0x6910u;
		OBJECT(object)->sprite_palette = 0x7B0u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0xF8u;
		OBJECT(object)->hitbox_offset_x = -8;
		OBJECT(object)->hitbox_offset_y = -5;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->kind = 0x17u;
		OBJECT(object)->sprite_flags = 0xAu;
		OBJECT(object)->handler = flame_machine_flame_burn_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_palette = 0x9FFEu;
		OBJECT(object)->sprite_flags = 0xAu;
		flame_machine_flags &= ~shl32(1u, (int)(spawn_arg_1 + 2u));
	}
}

/* A cannon's flame: every other frame the next of flame_machine_flame_frames (kept in spawn_arg_2;
   sprites from flame_machine_flame_sprites), touchable on those frames from frame 2, deleted at frame
   0xB. Placed 0x20 above its z, its shadow on the floor at frames 2-4; explodes when the machine is
   destroyed. */
void flame_machine_flame_burn(void)
{
	uint32_t bucket = 0, depth = 0, flame_frame = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t odd_frame = 0;

	OBJECT(object_cursor)->z_int -= 0x20u;
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->flame_machine_flame.frame_toggle++;
	odd_frame = OBJECT(current_object)->flame_machine_flame.frame_toggle & 1u;
	OBJECT(current_object)->flame_machine_flame.frame_toggle = odd_frame;
	if (odd_frame != 0u)
	{
		OBJECT(current_object)->flame_machine_flame.frame_index++;
		flame_frame = flame_machine_flame_frames[OBJECT(current_object)->flame_machine_flame.frame_index];
		OBJECT(current_object)->spawn_arg_2 = flame_frame;
		OBJECT(current_object)->sprite_frame = flame_machine_flame_sprites[flame_frame];
		if (flame_frame == 0xBu)
		{
			delete_object_and_children(&ignored, &ignored_2, &ignored_3);
			return;
		}
		if ((int16_t)OBJECT(current_object)->spawn_arg_2 >= 2)
		{
			OBJECT(current_object)->health = 0x7F;
			add_to_touchable_list(ignored);
		}
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x60u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((int16_t)OBJECT(current_object)->spawn_arg_2 >= 2 && (int16_t)OBJECT(current_object)->spawn_arg_2 <= 4)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = flame_machine_flame_shadow_sprites[OBJECT(object_cursor)->spawn_arg_2 - 2u];
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
	}
	OBJECT(object_cursor)->z_int += 0x20u;
	if ((flame_machine_flags & 0x100000u) == 0u)
		return;
	current_object = object_cursor;
	explode_object();
}

/* Hands the arm a spiked disc: a two-part object at the arm (sprite 0x6278, sprite_scale 0xC0, kind
   0x24, which hurts players; spawn_arg_1 = arm_index) with spiked_disc_hold, its part the shadow
   (0x6988); clears the arm's take-disc bit (0x400 << arm_index). */
void flame_machine_arm_take_disc(uint32_t free_count, uint32_t disc, uint32_t block)
{
	uint32_t disc_flag_bit = 0, ignored = 0, ignored_2 = 0, object = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x01061Fu, &ignored, &ignored_2, &free_count, &disc, &block);
	copy_position_to(disc);
	OBJECT(disc)->direction = 0x200u;
	OBJECT(disc)->sprite_scale = 0xC0u;
	OBJECT(disc)->sprite_frame = 0x6278u;
	OBJECT(disc)->sprite_palette = 0x1210u;
	OBJECT(disc)->sprite_flags = 0u;
	OBJECT(disc)->flags |= 2u;
	OBJECT(disc)->hitbox_size_x = 0x18u;
	OBJECT(disc)->hitbox_size_y = 0x3Cu;
	OBJECT(disc)->hitbox_offset_x = -0x31;
	OBJECT(disc)->hitbox_offset_y = 3;
	OBJECT(disc)->hitbox_height = 0x28u;
	OBJECT(disc)->kind = 0x24u;
	OBJECT(disc)->handler = spiked_disc_hold_ADDRESS;
	disc_flag_bit = OBJECT(current_object)->flame_machine_arm.arm_index + 0xAu;
	OBJECT(disc)->spawn_arg_1 = OBJECT(current_object)->flame_machine_arm.arm_index;
	flame_machine_flags &= ~shl32(1u, (int)(disc_flag_bit));
	object = LINKED(disc, OBJECT(disc)->child);
	OBJECT(object)->sprite_frame = 0x6988u;
	OBJECT(object)->sprite_palette = 0x9FFEu;
}

/* Holds the disc on its arm (its parent): the arm's position (copy_position_to_cursor) plus
   spiked_disc_hold_offsets[the arm's frame_index] in projected x and z, arm_frame keeping that frame.
   Once flame_machine_flags has 0x200 (discs dropped) it goes on to spiked_disc_drop. */
void spiked_disc_hold(void)
{
	uint32_t arm_frame_index = 0, offset_record = 0;

	if ((flame_machine_flags & 0x200u) != 0u)
		OBJECT(current_object)->handler = spiked_disc_drop_ADDRESS;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	copy_position_to_cursor();
	arm_frame_index = OBJECT(current_object)->flame_machine_arm.frame_index;
	OBJECT(object_cursor)->spiked_disc.arm_frame = arm_frame_index;
	offset_record = spiked_disc_hold_offsets_ADDRESS + arm_frame_index * 4u;
	OBJECT(object_cursor)->projected_x += SPIKED_DISC_HOLD_OFFSET(offset_record)->dx;
	OBJECT(object_cursor)->z_int += SPIKED_DISC_HOLD_OFFSET(offset_record)->dz;
	current_object = object_cursor;
	spiked_disc_draw_held();
}

/* Lets the dropped disc fall to its floor (fall_to_floor), then goes on to spiked_disc_roll; drawn by
   spiked_disc_draw_held. */
void spiked_disc_drop(void)
{
	uint32_t height = 0, ignored = 0, ignored_2 = 0;

	fall_to_floor(&height, &ignored, &ignored_2);
	if (height != 0u)
	{
		spiked_disc_draw_held();
		return;
	}
	OBJECT(current_object)->handler = spiked_disc_roll_ADDRESS;
	spiked_disc_draw_held();
}

/* Moves the disc (move_object_projected) and queues it in bucket (spawn_arg_1 + 1) * 2, then its shadow
   and spiked_disc_spin. The test for arm frame 8 reads arm_frame as a halfword with spin_toggle's low
   byte, 1 after the disc's first frame, so from then on the shadow is drawn whatever the frame. */
void spiked_disc_draw_held(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	bucket = (uint8_t)((OBJECT(current_object)->spawn_arg_1 + 1u) * 2u);
	queue_sprite_in_bucket(bucket);
	if ((int16_t)(OBJECT(current_object)->spiked_disc.arm_frame | ((uint32_t)(OBJECT(current_object)->spiked_disc.spin_toggle & 0xFFu) << 8u)) >= 8)
	{
		spiked_disc_draw_shadow();
		return;
	}
	spiked_disc_spin();
}

/* Rolls toward the players: moves, falls to its floor and is deleted once off screen; else its speed is
   set along angle 0x154 at 0x300 (direction_to_speed) and it is queued at screen_y + 0x18, with its
   shadow, then spiked_disc_spin. */
void spiked_disc_roll(void)
{
	uint32_t deleted = 0, depth = 0, direction = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, speed = 0, speed_x = 0, speed_y = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	fall_to_floor(&ignored, &ignored_5, &ignored_2);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	direction = 0x154u;
	speed = 0x300u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x18u);
	queue_sprite_at_depth(&ignored, &depth);
	spiked_disc_draw_shadow();
}

/* Draws the disc's shadow part at its floor (place_sprite_at_cursor_floor), 0x28 lower, then
   spiked_disc_spin. */
void spiked_disc_draw_shadow(void)
{
	uint32_t bucket = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	OBJECT(current_object)->screen_y += 0x28u;
	queue_sprite_in_bucket(bucket);
	current_object = object_cursor;
	spiked_disc_spin();
}

/* Explodes the disc once the machine is destroyed. Else steps its spin through the four
   spiked_disc_sprites and makes it touchable (health 0x7F) and a rider; spin_toggle, set back to 1 each
   time, lets that happen every frame after the first instead of every other. */
void spiked_disc_spin(void)
{
	uint16_t odd_frame = 0;

	if ((flame_machine_flags & 0x100000u) != 0u)
	{
		explode_object();
		return;
	}
	OBJECT(current_object)->spiked_disc.spin_toggle++;
	odd_frame = OBJECT(current_object)->spiked_disc.spin_toggle & 1u;
	OBJECT(current_object)->spiked_disc.spin_toggle = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->spiked_disc.spin_toggle = 1u;
		OBJECT(current_object)->spiked_disc.spin_frame++;
		OBJECT(current_object)->spiked_disc.spin_frame &= 3u;
		OBJECT(current_object)->sprite_frame = spiked_disc_sprites[OBJECT(current_object)->spiked_disc.spin_frame];
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(0u);
		add_to_rider_list();
	}
}

/* Stops the scroll (scroll_mode 1) once scroll_position reaches spawn_arg_1, then
   flame_machine_scroll_stop_hold. Object type 8, which no stage list spawns, so it never runs. */
void flame_machine_scroll_stop_wait(void)
{
	if ((int16_t)scroll_position < (int16_t)OBJECT(current_object)->spawn_arg_1)
		return;
	scroll_mode = 1u;
	OBJECT(current_object)->handler = flame_machine_scroll_stop_hold_ADDRESS;
	flame_machine_scroll_stop_hold();
}

/* Sets scroll_mode 7 (follow the players) every frame once flame_machine_flags has 0x100000 (the machine
   destroyed). Object type 8, which no stage list spawns. */
void flame_machine_scroll_stop_hold(void)
{
	if ((flame_machine_flags & 0x100000u) != 0u)
		scroll_mode = 7u;
}

/* The handler object_types gives type 9: an empty function. No stage list spawns the type. */
void object_type_9_idle(void)
{
}

/* The machine's wreck: the scorched ground (sprite 0x8360) in bucket 0xFE and two wrecked pieces at
   (0x6C, -0x66) and (-0x21, -0x27) from it in buckets 0xFD and 0xFC; deleted once map_screen_active is
   set. */
void flame_machine_wreck_draw(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	if (map_screen_active != 0u)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	bucket = 0xFEu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x6Cu;
	offset_y = 0xFF9Au;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFDu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0xFFDFu;
	offset_y = 0xFFD9u;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFCu;
	queue_sprite_in_bucket(bucket);
}

/* Builds the electric fence (electric_fence_state = 1): this post and extra_posts more, each a new
   two-part object fence_post_offsets[row_direction] from the one before (kept in current_player). A
   post: sprite 0x6CA8, kind 0xE, height 0x64, box by row_direction, fence_post_start; its part the arc
   (sprite 0x6C88, fence_arc_flags, arc_phase). */
void fence_post_init(void)
{
	uint16_t arc_phase = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, object = 0, object_2 = 0, rom_record = 0, row_direction = 0;
	uint32_t posts_left = 0;

	electric_fence_state = 1u;
	r14 = OBJECT(current_object)->fence_post.extra_posts;
	row_direction = OBJECT(current_object)->fence_post.row_direction;
	arc_phase = OBJECT(current_object)->fence_post.arc_phase;
	for (;;)
	{
		OBJECT(current_object)->sprite_frame = 0x6CA8u;
		OBJECT(current_object)->sprite_palette = 0x9060u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_height = 0x64u;
		OBJECT(current_object)->kind = 0xEu;
		rom_record = fence_post_boxes_ADDRESS + row_direction * 4u;
		OBJECT(current_object)->hitbox_size_x = FENCE_POST_BOX(rom_record)->size_x;
		OBJECT(current_object)->hitbox_size_y = FENCE_POST_BOX(rom_record)->size_y;
		rom_record = fence_post_box_offsets_ADDRESS + row_direction * 2u;
		OBJECT(current_object)->hitbox_offset_x = FENCE_POST_BOX_OFFSET(rom_record)->x;
		OBJECT(current_object)->hitbox_offset_y = FENCE_POST_BOX_OFFSET(rom_record)->y;
		OBJECT(current_object)->handler = fence_post_start_ADDRESS;
		object = LINKED(WORK_RAM, OBJECT(current_object)->child);
		OBJECT(object)->direction = arc_phase;
		OBJECT(object)->sprite_frame = 0x6C88u;
		OBJECT(object)->sprite_palette = 0x9030u;
		OBJECT(object)->sprite_flags = fence_arc_flags[row_direction];
		posts_left = r14;
		r14 = posts_left - 1u;
		if (posts_left == 0u)
			break;
		current_player = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x013956u, &object, &ignored, &free_count, &object_2, &block);
		if ((uint8_t)object == 0u)
			break;
		OBJECT(object_2)->screen_x = OBJECT(current_player)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_player)->screen_y;
		OBJECT(object_2)->projected_x = OBJECT(current_player)->projected_x;
		OBJECT(object_2)->projected_y = OBJECT(current_player)->projected_y;
		OBJECT(object_2)->z = OBJECT(current_player)->z;
		OBJECT(object_2)->floor_z = OBJECT(current_player)->floor_z;
		OBJECT(object_2)->spawn_arg_2 = OBJECT(current_player)->spawn_arg_2;
		rom_record = fence_post_offsets_ADDRESS + row_direction * 4u;
		OBJECT(object_2)->projected_x += FENCE_OFFSET(rom_record)->dx;
		OBJECT(object_2)->projected_y_int += FENCE_OFFSET(rom_record)->dy;
		current_object = object_2;
	}
}

/* Joins the rider list, places the post from its projected position (unproject_object) and goes on to
   fence_post_update. */
void fence_post_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	add_to_rider_list();
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->handler = fence_post_update_ADDRESS;
}

/* While electric_fence_state is 1 it draws the post's arc toward the next post (fence_arc_offsets, the
   next of fence_arc_frames every other frame, the arc's handler_low the toggle) and the post; at 0xFF it
   deletes the post; any other value makes it a plain post (kind 5, box 0x1B x 0x19) left to
   fence_post_draw. */
void fence_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t arc_offset_record = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;
	uint16_t odd_frame = 0;

	if (electric_fence_state == 1u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		arc_offset_record = fence_arc_offsets_ADDRESS + OBJECT(object_cursor)->fence_post.row_direction * 4u;
		offset_x = FENCE_OFFSET(arc_offset_record)->dx;
		offset_y = FENCE_OFFSET(arc_offset_record)->dy;
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->handler_low++;
		odd_frame = OBJECT(current_object)->handler_low & 1u;
		OBJECT(current_object)->handler_low = odd_frame;
		if (odd_frame == 0u)
		{
			OBJECT(current_object)->direction++;
			OBJECT(current_object)->direction &= 3u;
			OBJECT(current_object)->sprite_frame = fence_arc_frames[OBJECT(current_object)->direction];
		}
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		fence_post_draw();
		return;
	}
	if (electric_fence_state == 0xFFu)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->hitbox_size_x = 0x1Bu;
	OBJECT(current_object)->hitbox_size_y = 0x19u;
	OBJECT(current_object)->hitbox_offset_x = -9;
	OBJECT(current_object)->hitbox_offset_y = -0x13;
	OBJECT(current_object)->handler = fence_post_draw_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	arc_offset_record = fence_arc_offsets_ADDRESS + OBJECT(object_cursor)->fence_post.row_direction * 4u;
	offset_x = FENCE_OFFSET(arc_offset_record)->dx;
	offset_y = FENCE_OFFSET(arc_offset_record)->dy;
	place_sprite_near_cursor(offset_x, offset_y);
	OBJECT(current_object)->handler_low++;
	odd_frame = OBJECT(current_object)->handler_low & 1u;
	OBJECT(current_object)->handler_low = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->direction++;
		OBJECT(current_object)->direction &= 3u;
		OBJECT(current_object)->sprite_frame = fence_arc_frames[OBJECT(current_object)->direction];
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	fence_post_draw();
}

/* Places the post from its projected position, lets it fall to its floor and deletes it once off screen;
   else queues it, touchable (health 0x7F) and in the rider list. */
void fence_post_draw(void)
{
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	fall_to_floor(&ignored, &ignored_5, &ignored_2);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 1u)
	{
		queue_sprite(ignored_5);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		add_to_rider_list();
	}
}

/* A catapult of stage 4 that throws players along +x: keeps template offset 0 (spoon sprites
   0x6BE0-0x6C08, launch kind 6) in catapult.state and waits in the rider list for a platform
   (catapult_wait_for_platform). */
void catapult_x_start(void)
{
	OBJECT(current_object)->catapult.state = 0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = catapult_wait_for_platform_ADDRESS;
	add_to_rider_list();
}

/* A catapult of stage 4 that throws players along +y: keeps template offset 0x1A (sprites 0x6C10-0x6C58
   with a separate base, launch kind 7) in catapult.state and waits in the rider list for a platform
   (catapult_wait_for_platform). */
void catapult_y_start(void)
{
	OBJECT(current_object)->catapult.state = 0x1Au;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = catapult_wait_for_platform_ADDRESS;
	add_to_rider_list();
}

/* Stays in the rider list until it stands on a platform (flags 8), then sets rides_platform to 0xFF and
   sets itself up from the template offset kept in catapult.state (catapult_wait_for_platform_setup). */
void catapult_wait_for_platform(void)
{
	uint32_t state = 0;

	if ((OBJECT(current_object)->flags & 8u) != 0u)
	{
		state = OBJECT(current_object)->catapult.state;
		OBJECT(current_object)->catapult.rides_platform = 0xFFu;
		catapult_wait_for_platform_setup(state, 0u);
		return;
	}
	add_to_rider_list();
}

/* Unused: sets up catapult_y_start's catapult at once, on the floor: rides_platform 0, flags 0,
   catapult_wait_for_platform_setup with template offset 0x1A and object. */
void catapult_y_start_on_floor(uint32_t object)
{
	uint32_t template_offset = 0;

	template_offset = 0x1Au;
	OBJECT(current_object)->catapult.rides_platform = 0u;
	OBJECT(current_object)->flags = 0u;
	catapult_wait_for_platform_setup(template_offset, object);
}

/* Unused: sets up catapult_x_start's catapult at once, on the floor: rides_platform 0, flags 0,
   catapult_wait_for_platform_setup with template offset 0 and object. */
void catapult_x_start_on_floor(uint32_t object)
{
	uint32_t template_offset = 0;

	OBJECT(current_object)->catapult.rides_platform = 0u;
	OBJECT(current_object)->flags = 0u;
	template_offset = 0u;
	catapult_wait_for_platform_setup(template_offset, object);
}

/* Sets the catapult up from object_pair_templates + template_offset (catapult_pair_template): handler,
   animation, launch_kind, anchored on its floor; with a catapult_launch_delays[sprite_frame] delay,
   state 4 as kind 5 until launch_position, else state 0 as launch_kind. Adds its base part (deleting
   itself when none is free) and, with a delay, a dummy part. */
void catapult_wait_for_platform_setup(uint32_t template_offset, uint32_t object)
{
	uint16_t launch_delay = 0;
	uint32_t frame = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, steps = 0, template_cursor = 0;

	template_cursor = object_pair_templates_ADDRESS + template_offset;
	catapult_pair_template = template_cursor;
	OBJECT(current_object)->handler = OBJECT_PAIR_TEMPLATE(template_cursor)->handler;
	template_cursor += 4u;
	OBJECT(current_object)->animation = OBJECT_PAIR_TEMPLATE(template_cursor - 0x4u)->animation;
	template_cursor += 4u;
	OBJECT(current_object)->sprite_offset_x = OBJECT_PAIR_TEMPLATE(template_cursor - 0x8u)->sprite_offset_x;
	template_cursor++;
	OBJECT(current_object)->sprite_offset_y = OBJECT_PAIR_TEMPLATE(template_cursor - 0x9u)->sprite_offset_y;
	OBJECT(current_object)->catapult.launch_kind = OBJECT_PAIR_TEMPLATE(template_cursor - 0x9u)->launch_kind;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->sprite_palette = 0x9000u;
	OBJECT(current_object)->animation_frame = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->catapult.launch_position = scroll_position;
	launch_delay = catapult_launch_delays[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->catapult.launch_position += launch_delay;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->catapult.state = 4u;
	if (launch_delay == 0u)
	{
		OBJECT(current_object)->kind = OBJECT(current_object)->catapult.launch_kind;
		OBJECT(current_object)->catapult.state = 0u;
	}
	frame = 0u;
	steps = OBJECT(current_object)->animation + 2u;
	set_step_around_anchor(frame, steps);
	if (free_object_stack < 1)
	{
		delete_current_object();
		return;
	}
	allocate_object(&ignored, &object);
	OBJECT(object)->sprite_flags = 0x1000u;
	OBJECT(object)->sprite_frame = OBJECT(current_object)->sprite_frame;
	OBJECT(object)->sprite_palette = 0x9000u;
	OBJECT(object)->anchor_x = OBJECT(current_object)->anchor_x;
	OBJECT(object)->anchor_y = OBJECT(current_object)->anchor_y;
	OBJECT(object)->x = OBJECT(current_object)->x;
	OBJECT(object)->y = OBJECT(current_object)->y;
	OBJECT(object)->z = OBJECT(current_object)->z;
	OBJECT(object)->hitbox_height = OBJECT(current_object)->hitbox_height;
	template_cursor = catapult_pair_template + 0xCu;
	OBJECT(object)->hitbox_size_x = OBJECT_PAIR_TEMPLATE(template_cursor - 0xCu)->base_hitbox_size_x;
	template_cursor += 2u;
	OBJECT(object)->hitbox_size_y = OBJECT_PAIR_TEMPLATE(template_cursor - 0xEu)->base_hitbox_size_y;
	template_cursor += 2u;
	OBJECT(object)->hitbox_offset_x = OBJECT_PAIR_TEMPLATE(template_cursor - 0x10u)->base_hitbox_offset_x;
	template_cursor++;
	OBJECT(object)->hitbox_offset_y = OBJECT_PAIR_TEMPLATE(template_cursor - 0x11u)->base_hitbox_offset_y;
	template_cursor++;
	OBJECT(object)->handler = OBJECT_PAIR_TEMPLATE(template_cursor - 0x12u)->base_handler;
	OBJECT(object)->animation = OBJECT_PAIR_TEMPLATE(template_cursor - 0x12u)->base_animation;
	OBJECT(object)->flags = 2u;
	OBJECT(object)->kind = 5u;
	OBJECT(object)->sprite_offset_x = 0;
	OBJECT(object)->sprite_offset_y = 0;
	OBJECT(object)->parent = current_object;
	OBJECT(current_object)->child = object;
	if (OBJECT(current_object)->catapult.state != 0u)
	{
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->handler = catapult_dummy_update_ADDRESS;
			OBJECT(object)->parent = current_object;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(object)->flags = 0u;
			if (OBJECT(current_object)->catapult.rides_platform != 0u)
			{
				OBJECT(object)->flags |= 8u;
				OBJECT(object)->platform = OBJECT(current_object)->platform;
			}
		}
	}
}

/* Deletes the catapult and its base part once it has left the screen. Else runs catapult.state (0 and 5
   wait for a rider, 1 swing up, 2 hold, 3 swing down, 4 wait for the scroll) with current_player on the
   base part, takes its platform's z when rides_platform, is touchable and queued at screen_y - 0x14 (+
   0x14 in states 1-3). */
void catapult_update(void)
{
	uint16_t depth_offset = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, off_screen = 0, platform_z = 0;

	off_screen = check_left_screen();
	if ((uint8_t)off_screen != 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->child);
		delete_object(object, &ignored, &ignored_2);
		delete_current_object();
		return;
	}
	current_player = current_object;
	current_player = LINKED(current_player, OBJECT(current_object)->child);
	switch (OBJECT(current_object)->catapult.state)
	{
	case 0:
		catapult_wait_for_rider();
		break;
	case 1:
		catapult_swing_up();
		break;
	case 2:
		catapult_hold_up();
		break;
	case 3:
		catapult_swing_down();
		break;
	case 4:
		catapult_wait_for_scroll();
		break;
	case 5:
		catapult_wait_for_rider();
		break;
	default:
		invalid_jump(0x810C5u + OBJECT(current_object)->catapult.state * 3u);
	}
	if (OBJECT(current_object)->catapult.rides_platform != 0u)
	{
		platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = platform_z;
		OBJECT(current_object)->z = platform_z;
	}
	update_screen_position(&ignored_3, &ignored);
	add_to_touchable_list(ignored_3);
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	depth_offset = 0xFFECu;
	if (OBJECT(current_object)->catapult.state >= 1 && OBJECT(current_object)->catapult.state <= 3)
		depth_offset = 0x14u;
	depth = (uint16_t)(depth + depth_offset);
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* Shows frame 1 (pressed) while touched (flags 4), else frame 0; touched with touch_distance at most
   0x211, it throws (catapult_launch). */
void catapult_wait_for_rider(void)
{
	uint32_t frame = 0, steps = 0;

	frame = 0u;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
		frame = 1u;
	OBJECT(current_object)->animation_frame = frame;
	steps = OBJECT(current_object)->animation + 2u;
	set_step_around_anchor(frame, steps);
	if ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(current_object)->touch_distance > 0x211)
		return;
	catapult_launch();
}

/* Throws: state 1 from frame 1 at animation speed 3, forwards (flags 0x80 cleared), sound 0x1D. A player
   touching it in state 1 is launched (check_player_contacts). */
void catapult_launch(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->catapult.state = 1u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 1u;
	OBJECT(current_object)->flags &= ~0x80u;
	sound_id = 0x1Du;
	play_sound(sound_id, &ignored, &ignored_2);
}

/* Swings the spoon up around its anchor (animate_around_anchor); after the last step, state 2 with
   animation_speed 0x32 and animation_timer 3 for catapult_hold_up. */
void catapult_swing_up(void)
{
	uint32_t at_last_step = 0;

	at_last_step = animate_around_anchor();
	if ((int8_t)at_last_step < 0)
	{
		OBJECT(current_object)->catapult.state++;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_speed = 0x32u;
	}
}

/* Holds the spoon up for animation_speed (0x32) frames, wobbling between frames 4 and 5 every 3 frames,
   the last 0x1E frames on frame 4; then state 3 from frame 4 at speed 4, animating backwards (flags
   0x80). The 4 it stores in those last frames goes to current_player, the base part (set by
   catapult_update), and catapult_base_update overwrites it. */
void catapult_hold_up(void)
{
	uint32_t frame = 0, steps = 0;
	uint8_t animation_speed = 0, animation_timer = 0;

	animation_speed = OBJECT(current_object)->animation_speed;
	OBJECT(current_object)->animation_speed = animation_speed - 1u;
	if ((int8_t)animation_speed <= 1)
	{
		OBJECT(current_object)->catapult.state++;
		OBJECT(current_object)->flags |= 0x80u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_frame = 4u;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed <= 0x1E)
	{
		frame = 4u;
		OBJECT(current_player)->animation_frame = 4u;
		steps = OBJECT(current_object)->animation + 2u;
		set_step_around_anchor(frame, steps);
		return;
	}
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return;
	OBJECT(current_object)->animation_timer = 3;
	frame = 5u;
	if (OBJECT(current_object)->animation_frame != 4u)
		frame = 4u;
	OBJECT(current_object)->animation_frame = frame;
	steps = OBJECT(current_object)->animation + 2u;
	set_step_around_anchor(frame, steps);
}

/* Swings the spoon back down around its anchor; at frame 1 or below it shows frame 0 and goes back to
   state 0, its kind set back to launch_kind. */
void catapult_swing_down(void)
{
	uint32_t frame = 0, steps = 0;

	animate_around_anchor();
	if ((int8_t)OBJECT(current_object)->animation_frame <= 1)
	{
		frame = 0u;
		steps = OBJECT(current_object)->animation + 2u;
		set_step_around_anchor(frame, steps);
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->catapult.state = 0u;
		OBJECT(current_object)->kind = OBJECT(current_object)->catapult.launch_kind;
	}
}

/* State 4 of a catapult with a launch delay: throws by itself (catapult_launch) once scroll_position
   reaches launch_position. No stage list uses a kind with a delay. */
void catapult_wait_for_scroll(void)
{
	if (scroll_position < OBJECT(current_object)->catapult.launch_position)
		return;
	catapult_launch();
}

/* The catapult's base part: takes its parent's z (and floor_z) and follows its frame (the parent's
   animation_frame - 1, stepped once by animate_around_anchor); queued when it has a sprite_frame. */
void catapult_base_update(void)
{
	uint32_t ignored_2 = 0, ignored_3 = 0, object = 0, parent_z = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	parent_z = OBJECT(object)->z;
	OBJECT(current_object)->floor_z = parent_z;
	OBJECT(current_object)->z = parent_z;
	OBJECT(current_object)->animation_frame = OBJECT(object)->animation_frame;
	OBJECT(current_object)->animation_frame--;
	OBJECT(current_object)->animation_timer = 0;
	animate_around_anchor();
	update_screen_position(&ignored_2, &ignored_3);
	if (OBJECT(current_object)->sprite_frame == 0u)
		return;
	ignored_3 = queue_sprite(ignored_3);
}

/* The handler of the dummy part catapult_wait_for_platform_setup adds to a catapult with a launch delay:
   an empty function. */
void catapult_dummy_update(void)
{
}

/* Sets goal_unlocked to 0xFF and deletes itself (near the ends of stages B to F). */
void goal_unlocker_start(void)
{
	goal_unlocked = 0xFFu;
	delete_current_object();
}

/* An arrow board of stage 4 (sprite 0x6EB8, a wooden board with a red arrow, palette 0x9190), projected
   from its x and y; then arrow_board_wait_for_platform, this frame too. */
void arrow_board_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x6EB8u;
	OBJECT(current_object)->sprite_palette = 0x9190u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = arrow_board_wait_for_platform_ADDRESS;
	project_object(&ignored, &ignored_2);
	arrow_board_wait_for_platform();
}

/* Stays in the rider list until it stands on a platform (flags 8), then arrow_board_ride. */
void arrow_board_wait_for_platform(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = arrow_board_ride_ADDRESS;
	add_to_rider_list();
}

/* Rides its platform (z and floor_z = the platform's z), queued at screen_y - 0x20 until
   check_left_screen finds it has left the screen, then deleted. Grey (palette 0x8400) while
   grey_screen_active is set, palette 0x9190 again once grey_screen_ended is set. */
void arrow_board_ride(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0, platform_z = 0;

	if (grey_screen_active != 0u)
	{
		OBJECT(current_object)->sprite_palette = 0x8400u;
		platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = platform_z;
		OBJECT(current_object)->z = platform_z;
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &ignored_2);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	if (grey_screen_ended == 0u)
	{
		platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = platform_z;
		OBJECT(current_object)->z = platform_z;
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &ignored_2);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_palette = 0x9190u;
	platform_z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
	OBJECT(current_object)->floor_z = platform_z;
	OBJECT(current_object)->z = platform_z;
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* A pebble of stage 4 (sprite 0x6C60, a small grey rock, palette 0x9020) set on its floor and projected;
   then pebble_update. */
void pebble_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_frame = 0x6C60u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = pebble_update_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Queued at screen_y - 0x10 until check_left_screen finds it has left the screen, then deleted. */
void pebble_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* The big board propped up in stage 4 (sprite 0x6EA8, kind 0x11, box 0x10 x 0x12B, height 0xE0, z
   0x230), propped_board_object = it; with two objects free it adds the two props (sprite 0x6EC0, kind
   0xC, board_prop_start) 0x20 lower in x and 0x70 either way in y. */
void propped_board_start(void)
{
	uint16_t prop_dy = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0;
	uint16_t props_left_before = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x6EA8u;
	OBJECT(current_object)->sprite_palette = 0x9100u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x12Bu;
	OBJECT(current_object)->hitbox_height = 0xE0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x2C;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->propped_board.shake_strength = 0;
	OBJECT(current_object)->propped_board.shake_dx = 0;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x80;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->z = 0x2300000;
	OBJECT(current_object)->kind = 0x11u;
	OBJECT(current_object)->handler = propped_board_update_ADDRESS;
	propped_board_object = current_object;
	project_object(&ignored, &ignored_2);
	if (free_object_stack < 2)
		return;
	board_props_left = 2;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6EC0u;
		OBJECT(object)->sprite_palette = 0x91B0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 6;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x30u;
		OBJECT(object)->hitbox_size_y = 0x20u;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->hitbox_offset_x = -2;
		OBJECT(object)->hitbox_offset_y = -0xC;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->x_int -= 0x20u;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		prop_dy = 0xFF90u;
		if ((uint16_t)board_props_left != 2u)
			prop_dy = (uint16_t)(0u - prop_dy);
		OBJECT(object)->y_int += prop_dy;
		OBJECT(object)->handler = board_prop_start_ADDRESS;
		props_left_before = board_props_left;
		board_props_left = props_left_before - 1u;
	} while ((int16_t)props_left_before > 1);
}

/* Health 0xA; deleted once it has been on screen and left it, or below screen y 0x180. When
   group_leader_1 (the bridge collapse) reaches its x it falls (speed_z + 0x2000 a frame). Touched, it
   shakes (shake_strength 0x3C, screen_x moved +-shake_strength >> 4) and the first time releases its
   rings; touchable, queued at draw_depth. */
void propped_board_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, shake_dx = 0;
	uint16_t shake_strength = 0;
	uint32_t flags = 0;

	OBJECT(current_object)->health = 0xA;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x88u) >= 0x250u)
	{
		if ((OBJECT(current_object)->flags & 0x800u) != 0u)
		{
			delete_current_object();
			return;
		}
	}
	else
		OBJECT(current_object)->flags |= 0x800u;
	if (OBJECT(current_object)->x_int <= OBJECT(LINKED(current_object, group_leader_1))->x_int)
		OBJECT(current_object)->speed_z += 0x2000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	update_screen_position(&shake_dx, &ignored);
	if ((uint32_t)OBJECT(current_object)->speed_z == 0u)
		OBJECT(current_object)->propped_board.draw_depth = OBJECT(current_object)->screen_y - 0x10u;
	if (OBJECT(current_object)->screen_y >= 0x180)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->propped_board.shake_strength_low = 0x3Cu;
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 1u;
		if ((flags & 1u) == 0u)
			release_board_rings();
	}
	if ((uint16_t)OBJECT(current_object)->propped_board.shake_strength == 0u)
	{
		add_to_touchable_list(shake_dx);
		depth = OBJECT(current_object)->propped_board.draw_depth;
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	}
	shake_dx = (uint16_t)OBJECT(current_object)->propped_board.shake_strength >> 4;
	OBJECT(current_object)->propped_board.shake_step++;
	if ((OBJECT(current_object)->propped_board.shake_step & 1u) == 0u)
		shake_dx = (uint16_t)(0u - shake_dx);
	OBJECT(current_object)->screen_x += shake_dx;
	OBJECT(current_object)->propped_board.shake_dx = shake_dx;
	shake_strength = OBJECT(current_object)->propped_board.shake_strength;
	OBJECT(current_object)->propped_board.shake_strength = shake_strength - 2u;
	if ((int16_t)shake_strength <= 2)
		OBJECT(current_object)->propped_board.shake_strength = 0;
	add_to_touchable_list(shake_dx);
	depth = OBJECT(current_object)->propped_board.draw_depth;
	queue_sprite_at_depth(&ignored_2, &depth);
}

/* Projects the prop from its x and y and goes on to board_prop_update. */
void board_prop_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = board_prop_update_ADDRESS;
}

/* A prop under the propped board: deleted once it has left the screen; falls with the board (its z moves
   by propped_board_object's speed_z), keeps draw_depth = screen_y - 0xB while the board is still, shakes
   with it (shake_dx), is touchable and queued at draw_depth. */
void board_prop_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, object = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z += OBJECT(LINKED(current_object, propped_board_object))->speed_z;
	update_screen_position(&ignored, &ignored_2);
	object = LINKED(current_object, propped_board_object);
	if ((uint32_t)OBJECT(object)->speed_z == 0u)
		OBJECT(current_object)->board_prop.draw_depth = OBJECT(current_object)->screen_y - 0xBu;
	OBJECT(current_object)->screen_x += OBJECT(object)->propped_board.shake_dx;
	add_to_touchable_list(ignored);
	depth = OBJECT(current_object)->board_prop.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

/* Scatters the board's rings: scatter_rings_fixed 8 times, from x - 0x40, y - 0x40, z - 0x90 and 0x20
   further in y each time; the board's x, y and z are put back after. */
void release_board_rings(void)
{
	uint32_t allocated = 0, count = 0, random_value = 0;
	uint32_t saved_x = 0, saved_y = 0, saved_z = 0;
	uint16_t rings_left_before = 0;

	saved_x = OBJECT(current_object)->x;
	saved_y = OBJECT(current_object)->y;
	saved_z = OBJECT(current_object)->z;
	OBJECT(current_object)->y_int -= 0x40u;
	OBJECT(current_object)->x_int -= 0x40u;
	OBJECT(current_object)->z_int -= 0x90u;
	board_rings_left = 8;
	do
	{
		count = 1u;
		scatter_rings_fixed(count, &random_value, &allocated);
		OBJECT(current_object)->y_int += 0x20u;
		rings_left_before = board_rings_left;
		board_rings_left = rings_left_before - 1u;
	} while ((int16_t)rings_left_before > 1);
	OBJECT(current_object)->z = saved_z;
	OBJECT(current_object)->y = saved_y;
	OBJECT(current_object)->x = saved_x;
}

/* A boulder of stage 4 (boulder_animation, sprite_scale 0x200, kind 9, box 0x80 x 0xC0, height 0x60),
   direction 0x300, speed 0x30000, wait_timer 0x78; sets group_leader_0_held, so new group leaders wait,
   joins the rider list and goes on to boulder_wait. */
void boulder_start(void)
{
	OBJECT(current_object)->sprite_flags = 4u;
	OBJECT(current_object)->sprite_palette = 0x90F0u;
	OBJECT(current_object)->sprite_scale = 0x200u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0xC0u;
	OBJECT(current_object)->hitbox_height = 0x60u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->sprite_offset_x = 0x20;
	OBJECT(current_object)->sprite_offset_y = 0x10;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->screen_y = 0;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x60;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation = boulder_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->handler = boulder_wait_ADDRESS;
	OBJECT(current_object)->boulder.wait_timer = 0x78;
	OBJECT(current_object)->speed = 0x30000;
	group_leader_0_held = 0xFFu;
	add_to_rider_list();
}

/* Waits in the rider list until scroll_position reaches 0x3B00, or for wait_timer (0x78) frames once it
   is past 0x3800, then boulder_roll. */
void boulder_wait(void)
{
	uint16_t wait_timer = 0;

	if (scroll_position >= 0x3B00u)
	{
		OBJECT(current_object)->handler = boulder_roll_ADDRESS;
		boulder_roll();
		return;
	}
	if (scroll_position >= 0x3800u)
	{
		wait_timer = OBJECT(current_object)->boulder.wait_timer;
		OBJECT(current_object)->boulder.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->handler = boulder_roll_ADDRESS;
			boulder_roll();
			return;
		}
	}
	add_to_rider_list();
}

/* Rolls just ahead of the bridge collapse: x = group_leaders[0]'s x - 0x30, speed_x = -chaser_speed <<
   12, health 0x7F, group_leader_0_held cleared; the first time x_int is 0x180 or less, camera shake and
   sound 0xF. Falls where there is no floor block (floor height 0x7FF becomes 0x230); touchable, a rider,
   deleted below screen y 0x180. */
void boulder_roll(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0;

	OBJECT(current_object)->health = 0x7F;
	if ((OBJECT(current_object)->flags & 1u) == 0u && OBJECT(current_object)->x_int <= 0x180)
	{
		OBJECT(current_object)->flags |= 1u;
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	group_leader_0_held = 0u;
	OBJECT(current_object)->x = OBJECT(LINKED(current_object, group_leaders[0]))->x;
	OBJECT(current_object)->x_int -= 0x30u;
	OBJECT(current_object)->speed_x = 0u - ((uint32_t)(uint16_t)chaser_speed << 12);
	OBJECT(current_object)->speed_y = 0;
	find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
	if ((int16_t)(uint16_t)OBJECT(current_object)->floor_block < 0)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		animate(&ignored, &ignored_6, &ignored_7);
		project_object(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0x180)
		{
			add_to_touchable_list(ignored_3);
			add_to_rider_list();
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if ((uint16_t)OBJECT(current_object)->z_int != 0x7FFu)
	{
		animate(&ignored, &ignored_6, &ignored_7);
		project_object(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0x180)
		{
			add_to_touchable_list(ignored_3);
			add_to_rider_list();
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z_int = 0x230;
	animate(&ignored, &ignored_6, &ignored_7);
	project_object(&ignored_3, &ignored);
	if (OBJECT(current_object)->screen_y < 0x180)
	{
		add_to_touchable_list(ignored_3);
		add_to_rider_list();
		ignored = queue_sprite_at_previous_floor();
		return;
	}
	delete_current_object();
}

/* The brick wall with a stone doorway at the start of stage 4 (sprite 0x6EB0, palette 0x9110) at x
   -0xE2E, y 0xF31 on its floor, with 0x10 pieces to burst into (door_fragments); then
   door_wall_wait_to_burst. */
void door_wall_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9110u;
	OBJECT(current_object)->sprite_frame = 0x6EB0u;
	OBJECT(current_object)->door_wall.fragment = door_fragments_ADDRESS;
	OBJECT(current_object)->door_wall.pieces_left = 0x10;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->x = -0xE2E0000;
	OBJECT(current_object)->y = 0xF310000;
	OBJECT(current_object)->handler = door_wall_wait_to_burst_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Drawn until scene_step reaches 3; then it bursts into pieces_left (0x10) fragments (rock_debris_fly,
   palette 0x8240) at random spots up to 0x3F pixels from its screen position, with random speeds and the
   animation of each door_fragments record, and deletes itself. */
void door_wall_wait_to_burst(void)
{
	uint32_t carry = 0;
	uint32_t fragment_animation = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, random_bits = 0, random_offsets = 0;
	uint8_t pieces_left = 0;

	if ((int8_t)scene_step < 3)
	{
		update_screen_position(&ignored_3, &ignored_4);
		ignored_4 = queue_sprite(ignored_4);
		return;
	}
	for (;;)
	{
		pieces_left = OBJECT(current_object)->door_wall.pieces_left;
		OBJECT(current_object)->door_wall.pieces_left = pieces_left - 1u;
		if ((int8_t)pieces_left < 1)
			break;
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		next_random(&random_bits, &ignored, &ignored_2);
		random_offsets = (random_bits & ~0xFFFFu) | (random_bits & 0x3Fu);
		OBJECT(object)->screen_x += random_offsets;
		OBJECT(object)->screen_y += (random_offsets >> 16) & 0x3Fu;
		random_bits = (random_bits & ~0xFFFFu) | (random_bits & 0xFFu);
		random_bits = (random_bits & ~0xFFFFu) | (uint16_t)(random_bits + 4u);
		OBJECT(object)->speed_x_fraction = random_bits;
		carry = (random_bits >> 16) & 1u;
		random_bits = random_bits >> 17;
		if (carry)
		{
			random_bits = random_bits & 0xFFu;
			random_bits = (uint16_t)(0u - random_bits);
			random_bits = (uint16_t)(random_bits - 0x10u);
		}
		else
		{
			random_bits = random_bits & 0xFFu;
			random_bits = (uint16_t)(random_bits + 4u);
		}
		OBJECT(object)->speed_y_fraction = random_bits;
		fragment_animation = OBJECT(current_object)->door_wall.fragment + 8u;
		OBJECT(object)->animation = rock_debris_animations_ADDRESS + DOOR_FRAGMENT(fragment_animation - 0x8u)->animation;
		OBJECT(current_object)->door_wall.fragment = fragment_animation + 2u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
	}
	delete_current_object();
}

/* Called by event_0c_start: places four spiked rollers behind the door from door_roller_layout (x, y,
   first frame; spiked_roller_animation, palette 0x82F0, kind 4, speed_z -0x80000, roll_out_timer 8) on
   their floors, with door_roller_update; current_object is left on the last one. */
void spawn_door_rollers(uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_object)
{
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, layout_record = 0;
	uint16_t rollers_left_before = 0;

	door_rollers_left = 4;
	do
	{
		allocate_object_outside_loop(&free_count, p_object);
		if (free_count == 0u)
		{
			return;
		}
		current_object = *p_object;
		layout_record = door_roller_layout_ADDRESS + (uint16_t)door_rollers_left * 3u * 2u;
		OBJECT(current_object)->x_int = DOOR_ROLLER_SPAWN(layout_record)->x;
		OBJECT(current_object)->x_fraction = 0u;
		layout_record += 2u;
		OBJECT(current_object)->y_int = DOOR_ROLLER_SPAWN(layout_record - 0x2u)->y;
		OBJECT(current_object)->y_fraction = 0u;
		OBJECT(current_object)->animation_frame = DOOR_ROLLER_SPAWN(layout_record - 0x2u)->first_frame;
		OBJECT(current_object)->sprite_flags = 0x1004u;
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_size_x = 0x10u;
		OBJECT(current_object)->hitbox_size_y = 0x10u;
		OBJECT(current_object)->hitbox_height = 0x28u;
		OBJECT(current_object)->kind = 4u;
		OBJECT(current_object)->handler = door_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->door_roller.state = 0u;
		OBJECT(current_object)->door_roller.roll_out_timer = 8;
		OBJECT(current_object)->door_roller.off_screen = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		find_floor(&ignored, &ignored_2, p_block_offset, p_slope_height, p_object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		rollers_left_before = door_rollers_left;
		door_rollers_left = rollers_left_before - 1u;
	} while ((int16_t)rollers_left_before > 1);
}

/* A roller behind the door: runs its state (door_roller.state, var_80: 0 and 4 door_roller_wait, 1
   door_roller_drop, 2 door_roller_roll_out, 3 door_roller_ride); deleted once off_screen (var_83) is
   set; animated, projected and, from state 1 on, queued (queue_sprite_at_base). */
void door_roller_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	switch (OBJECT(current_object)->door_roller.state)
	{
	case 0:
		door_roller_wait();
		break;
	case 1:
		door_roller_drop();
		break;
	case 2:
		door_roller_roll_out();
		break;
	case 3:
		door_roller_ride();
		break;
	case 4:
		door_roller_wait();
		break;
	default:
		invalid_jump(0x82AC6u + OBJECT(current_object)->door_roller.state * 3u);
	}
	if (OBJECT(current_object)->door_roller.off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&ignored, &ignored_2, &ignored_3);
	project_object(&ignored_4, &ignored);
	if (OBJECT(current_object)->door_roller.state == 0u)
		return;
	ignored = queue_sprite_at_base(ignored);
}

/* Health 0x7F; waits for scene_step 3 (the door bursts), then state 1. */
void door_roller_wait(void)
{
	OBJECT(current_object)->health = 0x7F;
	if ((int8_t)scene_step >= 3)
		OBJECT(current_object)->door_roller.state++;
}

/* Health 0x7F; flies out of the doorway (speed_z starts at -0x80000) and falls 0x6000 a frame, moving 5
   a frame along y, animated, in the rider list. On its floor: state 2 and a camera shake, or deleted
   when there is no floor there (z_int 0x800 or more). */
void door_roller_drop(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0;

	OBJECT(current_object)->health = 0x7F;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->y_int += 5u;
	animate(&ignored, &ignored_2, &ignored_3);
	find_floor(&ignored_4, &ignored, &ignored_5, &ignored_6, &ignored_7);
	if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->door_roller.state++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (OBJECT(current_object)->z_int >= 0x800)
	{
		delete_current_object();
		return;
	}
	camera_shake_active = 1u;
	add_to_rider_list();
}

/* Rolls 1 along y a frame for roll_out_timer (8) frames, then sets flags 0x400000, which drops the
   crumbling floor under it at once, and goes to state 3. */
void door_roller_roll_out(void)
{
	uint16_t roll_out_timer = 0;

	OBJECT(current_object)->y_int++;
	roll_out_timer = OBJECT(current_object)->door_roller_roll_out.roll_out_timer;
	OBJECT(current_object)->door_roller_roll_out.roll_out_timer = roll_out_timer - 1u;
	if ((int16_t)roll_out_timer <= 1)
	{
		OBJECT(current_object)->flags |= 0x400000u;
		OBJECT(current_object)->door_roller_roll_out.state++;
	}
}

/* Rides its platform, the bridge planks (z and floor_z = the platform's z), in the rider list until it
   is at screen y 0x130 or below; then sets off_screen, so door_roller_update deletes it. */
void door_roller_ride(void)
{
	uint32_t z = 0;

	if (OBJECT(current_object)->screen_y < 0x130)
	{
		z = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->z;
		OBJECT(current_object)->floor_z = z;
		OBJECT(current_object)->z = z;
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->door_roller.off_screen = 0xFFu;
}

/* The fireball emitter near the end of stage 4: from the list fireball_emitter_lists gives for its
   sprite_frame it takes its x, y and floor height and keeps the rest, its shots, in shot; then
   fireball_emitter_update. It has no sprite. */
void fireball_emitter_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, list_cursor = 0;

	list_cursor = fireball_emitter_lists[OBJECT(current_object)->sprite_frame] + fireball_emitter_lists_ADDRESS;
	OBJECT(current_object)->x_int = FIREBALL_EMITTER_LIST(list_cursor)->x;
	OBJECT(current_object)->x_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->y_int = FIREBALL_EMITTER_LIST(list_cursor - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	list_cursor += 2u;
	OBJECT(current_object)->floor_z_int = FIREBALL_EMITTER_LIST(list_cursor - 0x4u)->floor_z;
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->fireball_emitter.shot = list_cursor + 2u;
	OBJECT(current_object)->flags = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->handler = fireball_emitter_update_ADDRESS;
}

/* Moves with the camera's ground speed for its floor's slope (compute_camera_slope_speed) while screen_y
   is -0x40 or more, on its floor. When scroll_position reaches the next shot it spawns a fireball
   (bouncing_fireball_update, kind 0x17) at the shot's x and dy with its speeds, fireball_types looks and
   a shadow; deleted after the last shot. */
void fireball_emitter_update(void)
{
	uint16_t scale = 0;
	uint32_t floor_slope = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, list_record = 0, object = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, type_record = 0;

	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->screen_y >= -0x40)
	{
		slope_factor = 0u;
		if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		{
			floor_slope = OBJECT(current_object)->floor_slope;
			slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
		}
		compute_camera_slope_speed(0u, slope_factor);
		slope_direction = camera_slope_direction;
		slope_speed = camera_slope_speed >> 14;
		OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 7;
		OBJECT(current_object)->y -= (int32_t)(cosine_table[slope_direction] * slope_speed) >> 7;
	}
	find_floor(&ignored, &ignored_2, &ignored_4, &ignored_3, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (scroll_position < FIREBALL_SHOT(OBJECT(current_object)->fireball_emitter.shot)->position)
	{
		add_to_rider_list();
		return;
	}
	if (free_object_stack <= 0 || free_object_stack_2[0] <= 0)
		return;
	allocate_object(&ignored, &object);
	OBJECT(object)->handler = bouncing_fireball_update_ADDRESS;
	OBJECT(object)->flags = 0u;
	OBJECT(object)->hitbox_size_y = 0u;
	OBJECT(object)->kind = 0x17u;
	OBJECT(object)->z = OBJECT(current_object)->floor_z;
	OBJECT(object)->y = OBJECT(current_object)->y;
	list_record = OBJECT(current_object)->fireball_emitter.shot + 2u;
	OBJECT(object)->x_int = FIREBALL_SHOT(list_record - 0x2u)->x;
	OBJECT(object)->x_fraction = 0u;
	list_record += 2u;
	OBJECT(object)->y_int += FIREBALL_SHOT(list_record - 0x4u)->dy;
	list_record += 2u;
	OBJECT(object)->speed_y = (uint32_t)FIREBALL_SHOT(list_record - 0x6u)->speed_y << 8;
	OBJECT(object)->speed_x = 0;
	list_record += 2u;
	OBJECT(object)->speed_z = (uint32_t)FIREBALL_SHOT(list_record - 0x8u)->speed_z << 8;
	type_record = FIREBALL_SHOT(list_record - 0x8u)->type + fireball_types_ADDRESS;
	OBJECT(object)->sprite_flags = FIREBALL_TYPE(type_record)->sprite_flags;
	type_record += 2u;
	OBJECT(object)->sprite_palette = FIREBALL_TYPE(type_record - 0x2u)->sprite_palette;
	type_record += 2u;
	OBJECT(object)->sprite_scale = FIREBALL_TYPE(type_record - 0x4u)->sprite_scale;
	type_record += 2u;
	OBJECT(object)->hitbox_height = FIREBALL_TYPE(type_record - 0x6u)->hitbox_height;
	type_record += 2u;
	OBJECT(object)->hitbox_size_x = FIREBALL_TYPE(type_record - 0x8u)->hitbox_size_x;
	type_record += 2u;
	OBJECT(object)->sprite_offset_x = FIREBALL_TYPE(type_record - 0xAu)->sprite_offset_x;
	type_record++;
	OBJECT(object)->sprite_offset_y = FIREBALL_TYPE(type_record - 0xBu)->sprite_offset_y;
	type_record++;
	OBJECT(object)->hitbox_centre_x = FIREBALL_TYPE(type_record - 0xCu)->hitbox_centre_x;
	type_record++;
	OBJECT(object)->hitbox_centre_y = FIREBALL_TYPE(type_record - 0xDu)->hitbox_centre_y;
	OBJECT(object)->animation = FIREBALL_TYPE(type_record - 0xDu)->animation + blank_fireball_animation_ADDRESS;
	OBJECT(object)->animation_frame = 0u;
	OBJECT(object)->animation_timer = 0;
	OBJECT(object)->animation_speed = 4u;
	current_object = object;
	scale = 0x180u;
	if (OBJECT(current_object)->hitbox_size_x == 0u)
		scale = 0x30u;
	create_child_shadow(scale, object);
	current_object = object_cursor;
	OBJECT(current_object)->fireball_emitter.shot += 0xCu;
	if ((int16_t)FIREBALL_SHOT(OBJECT(current_object)->fireball_emitter.shot)->type < 0)
	{
		delete_current_object();
		return;
	}
	add_to_rider_list();
}

/* A fireball: moves by speed_x and speed_y, falls 0x6000 a frame and bounces off its floor at half speed
   (sound 0x21 when it goes back up at 2 or more). Deleted with its shadow once it has left the screen;
   else animated, touchable if it has a hitbox_size_x, a rider, queued, its shadow at its floor, blinking
   while on a platform. */
void bouncing_fireball_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, shadow_dx = 0, shadow_dy = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, off_screen = 0;
	uint8_t blink_phase = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (OBJECT(current_object)->speed_z_int <= -2)
		{
			sound_id = 0x21u;
			play_sound(sound_id, &ignored_2, &ignored_3);
		}
	}
	project_object(&ignored, &ignored_2);
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		if ((int16_t)child >= 0)
			free_object_2(child);
		delete_current_object();
		return;
	}
	animate(&ignored_2, &ignored_6, &ignored_7);
	if (OBJECT(current_object)->hitbox_size_x != 0u)
		add_to_touchable_list(ignored);
	OBJECT(current_object)->flags &= ~0x10u;
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->flags |= 0x10u;
	add_to_rider_list();
	queue_sprite(ignored_2);
	shadow_dx = 8u;
	shadow_dy = 0xFFFDu;
	if (OBJECT(current_object)->hitbox_size_x == 0u)
	{
		shadow_dx = 0u;
		shadow_dy = 0xFFFDu;
	}
	if (OBJECT(current_object)->floor_block < 0 || (int16_t)OBJECT(current_object)->child < 0)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->projected_x - view_x + shadow_dx;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(object_cursor)->projected_y + OBJECT(object_cursor)->floor_z) >> 16) - view_y + shadow_dy;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	if ((OBJECT(object_cursor)->flags & 0x10u) != 0u)
	{
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		blink_phase = ~OBJECT(current_object)->shadow_blink;
		OBJECT(current_object)->shadow_blink = blink_phase;
		if (blink_phase == 0u)
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

/* Unused: an empty function, a lone return the original left after bouncing_fireball_update. */
void bouncing_fireball_update_spare_return(void)
{
}

/* Unused: knocks the object flying: speed_z -0x80000, the direction and speed of the object that touched
   it (touched_by), the direction turned 0x80 one way or the other by bit 0 of speed_int,
   set_speed_from_direction; then knocked_object_fly. */
void bouncing_fireball_knocked_away(void)
{
	uint16_t turn = 0;
	uint32_t toucher = 0;

	OBJECT(current_object)->speed_z = -0x80000;
	toucher = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->direction = OBJECT(toucher)->direction;
	OBJECT(current_object)->speed = OBJECT(toucher)->speed;
	turn = 0x80u;
	if ((OBJECT(current_object)->speed_int & 1u) != 0u)
		turn = (uint16_t)(0u - turn);
	OBJECT(current_object)->direction += turn;
	OBJECT(current_object)->direction &= 0x3FFu;
	set_speed_from_direction();
	OBJECT(current_object)->handler = knocked_object_fly_ADDRESS;
	knocked_object_fly();
}

/* Unused: flies with gravity (speed_z + 0x6000 a frame) by speed_x, speed_y and speed_z with no floor
   test, animated and queued until is_off_screen; then frees its pool 2 child and deletes itself. Only
   bouncing_fireball_knocked_away sets it. */
void knocked_object_fly(void)
{
	uint16_t child = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_4 = 0, ignored_5 = 0, off_screen = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	project_object(&ignored, &ignored_2);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		animate(&ignored_2, &ignored_4, &ignored_5);
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	child = OBJECT(current_object)->child;
	if ((int16_t)child >= 0)
		free_object_2(child);
	delete_current_object();
}
