/**
 *    Name:        objects/stage_05.c
 *    Description: stage 5's objects: the exit door, the floor patch, the bubbles, the spiked press, the cage
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Stage list object of stage 5: sprite 0x7348, palette 0x91A0, sprite_flags 0x1004, z 0; projected, then
   exit_door_wait_for_switch. */
void exit_door_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_frame = 0x7348u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = exit_door_wait_for_switch_ADDRESS;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored_2);
}

/* exit_door_show until stage_05_switch_step is 2; then it becomes sprite 0x71A0, palette 0x9140, moved
   to x 0x1E2, y 0x154C and projected, with exit_door_show from then on. */
void exit_door_wait_for_switch(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	if ((uint8_t)stage_05_switch_step != 2u)
	{
		exit_door_show();
		return;
	}
	OBJECT(current_object)->sprite_frame = 0x71A0u;
	OBJECT(current_object)->sprite_palette = 0x9140u;
	OBJECT(current_object)->x = 0x1E20000;
	OBJECT(current_object)->y = 0x154C0000;
	OBJECT(current_object)->handler = exit_door_show_ADDRESS;
	project_object(&ignored, &ignored_2);
}

/* Drawn (queue_sprite) until its screen_y reaches 0x130, then removed. */
void exit_door_show(void)
{
	uint32_t depth = 0, ignored = 0;

	update_screen_position(&ignored, &depth);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		depth = queue_sprite(depth);
		return;
	}
	delete_current_object();
}

/* Stage list object of stage 5: a patch of stone floor tiles (sprite 0x7050, palette 0xB0E0,
   sprite_flags 0x1008), z 0; projected, then stage_05_floor_patch_show. */
void stage_05_floor_patch_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_frame = 0x7050u;
	OBJECT(current_object)->sprite_palette = 0xB0E0u;
	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = stage_05_floor_patch_show_ADDRESS;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored_2);
}

/* Drawn (queue_sprite) while stage is 5, else removed. */
void stage_05_floor_patch_show(void)
{
	uint32_t depth = 0, ignored = 0;

	update_screen_position(&ignored, &depth);
	if (stage == 5u)
	{
		depth = queue_sprite(depth);
		return;
	}
	delete_current_object();
}

/* Stage list object (stages 5, 8, 0xB, 0xC): a bubble that swells and pops (bubble_animation, speed 4),
   z from bubble_heights[sprite_frame] (its variant), base_x/base_y its position, palette 0x9000 (0xD000
   in stage 8, 0x10B0 outside stages 5 and 8); projected, then bubble_update. */
void bubble_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->z_int = bubble_heights[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->bubble_start.variant = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->bubble_start.base_x = OBJECT(current_object)->x_int;
	OBJECT(current_object)->bubble_start.base_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9000u;
	if (stage != 5u && stage != 8u)
		OBJECT(current_object)->sprite_palette = 0x10B0u;
	if (stage == 8u)
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = bubble_update_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = bubble_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	project_object(&ignored, &ignored_2);
}

/* In stages 5, 8, 0xB and 0xC outside game mode 0x20 (YOU FAILED): draw_depth = screen_y, then
   bubble_pop; anywhere else removed. */
void bubble_update(void)
{
	if ((stage == 0xCu || stage == 8u || stage == 0xBu || stage == 5u) && current_game_mode != 0x20u)
	{
		OBJECT(current_object)->bubble_update.draw_depth = OBJECT(current_object)->screen_y;
		bubble_pop();
		return;
	}
	delete_current_object();
}

/* A lava column turned bubble (lava_column_become_bubble): bubble_pop at draw_depth 0xFFF0 (under
   everything) while its y_int is greater than its parent's y_int + 0xB0; removed once it is not. */
void bubble_near_parent_update(void)
{
	if ((int16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->y_int + 0xB0u) < OBJECT(current_object)->y_int)
	{
		OBJECT(current_object)->bubble_near_parent_update.draw_depth = 0xFFF0u;
		bubble_pop();
		return;
	}
	delete_current_object();
}

/* Removed when map_screen_active is set or it has left the screen; otherwise animated and drawn at its
   draw_depth. Each time its animation wraps (a pop) it takes a new animation speed 1-8: variant 6 at
   random; the others first move to base_x and base_y plus one random offset (-0xF to 0xF) on both, the
   speed from the new x_int's low bits. */
void bubble_pop(void)
{
	uint16_t jitter_y = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, jitter = 0, off_screen = 0, wrapped = 0;

	if (map_screen_active != 0u)
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
	update_screen_position(&ignored, &ignored_2);
	animate(&ignored_2, &wrapped, &ignored_3);
	if ((uint8_t)wrapped == 0u)
	{
		depth = OBJECT(current_object)->bubble_pop.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	next_random(&jitter, &ignored_4, &ignored_5);
	if (OBJECT(current_object)->bubble_pop.variant == 6u)
	{
		OBJECT(current_object)->animation_speed = (jitter & 7u) + 1u;
		depth = OBJECT(current_object)->bubble_pop.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	jitter = (jitter & ~0xFFFFu) | (jitter & 0xFu);
	if ((int32_t)jitter < 0)
		jitter = (uint16_t)(0u - jitter);
	jitter_y = (uint16_t)jitter;
	jitter = (uint16_t)(jitter + OBJECT(current_object)->bubble_pop.base_x);
	OBJECT(current_object)->x_int = jitter;
	OBJECT(current_object)->y_int = jitter_y + OBJECT(current_object)->bubble_pop.base_y;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->animation_speed = (jitter & 7u) + 1u;
	depth = OBJECT(current_object)->bubble_pop.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

/* Stage list object of stage 5: the spiked press (sprite 0x7070), kind 9, box 0xE0 x 0x20, height 0xE0,
   speed_y 0x2000, on its floor; it becomes spiked_press_object, stops the scroll (scroll_mode 1),
   freezes the players and spawns Eggman's booth (spawn_eggman_booth); then spiked_press_update. */
void spiked_press_start(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, slope_height = 0, x = 0;

	OBJECT(current_object)->sprite_flags = 0x1104u;
	OBJECT(current_object)->sprite_frame = 0x7070u;
	OBJECT(current_object)->sprite_palette = 0x9100u;
	OBJECT(current_object)->sprite_offset_x = 0x38;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0xE0u;
	OBJECT(current_object)->hitbox_size_y = 0x20u;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->hitbox_height = 0xE0u;
	OBJECT(current_object)->spiked_press.sound_timer = 0;
	OBJECT(current_object)->spiked_press.frame_count = 0;
	OBJECT(current_object)->spiked_press.stopped_frames = 0;
	OBJECT(current_object)->spiked_press.holding_back = 0u;
	OBJECT(current_object)->handler = spiked_press_update_ADDRESS;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0x2000;
	OBJECT(current_object)->direction = 0x200u;
	find_floor(&x, &ignored, &block_offset, &slope_height, &cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	spiked_press_object = current_object;
	scroll_mode = 1u;
	players_frozen = 0xFFu;
	spawn_eggman_booth(x, ignored, slope_height, &block_offset, &cursor);
}

/* Counts frames in game modes 3 and 0xA: at 0x14 a message box and at 0x50 a voice by caged_character,
   at 0xC8 the "BREAK!!" sign and arrow, players unfrozen, at 0x168 sign 3. From 0xC8 it creeps along y
   (sound 0xD every 0x3C frames), pausing 0x100 frames when no one is in play or one drops in, to y
   0x3D0: scroll speed 5. */
void spiked_press_update(void)
{
	uint8_t sound = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0, x = 0, y = 0, z = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, player = 0, player_index = 0, players_left = 0, sign = 0, status_cursor = 0;
	uint8_t hold_timer = 0, status = 0;
	uint16_t sound_timer = 0;

	if (current_game_mode == 3u || current_game_mode == 0xAu)
		OBJECT(current_object)->spiked_press.frame_count++;
	if (OBJECT(current_object)->spiked_press.frame_count >= 0xC8 && OBJECT(current_object)->spiked_press.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->spiked_press.holding_back = 0xFFu;
			OBJECT(current_object)->spiked_press.hold_timer = 0xFFu;
		}
		else
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	}
	if (OBJECT(current_object)->spiked_press.holding_back != 0u)
	{
		if (current_game_mode != 3u && current_game_mode != 0xAu)
			OBJECT(current_object)->spiked_press.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_press.hold_timer;
		OBJECT(current_object)->spiked_press.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
			OBJECT(current_object)->spiked_press.holding_back = 0u;
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0xC8u)
	{
		x = 0xFE0Du;
		y = 0x3E0u;
		kind = 1u;
		z = 0xFFB0u;
		sign = spawn_sign_2(x, y, kind, z, 0u);
		x = 0xFE0Du;
		y = 0x3E0u;
		kind = 2u;
		z = 0xFFC2u;
		sign = spawn_sign_2(x, y, kind, z, sign);
		players_frozen = 0u;
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x168u)
	{
		x = 0xFE70u;
		y = 0x480u;
		kind = 3u;
		z = 0xFFB0u;
		sign = spawn_sign_2(x, y, kind, z, sign);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x14u)
	{
		players_left = 3u;
		player_index = 0u;
		status_cursor = player_status_ADDRESS;
		do
		{
			status = mem_read8(status_cursor);
			status_cursor++;
			if (status == 0xFFu)
				break;
			player_index = (uint16_t)(player_index + 1u);
			players_left--;
		} while (players_left != 0u);
		player = PLAYER_BLOCK(1) + (player_index << 9);
		caged_character = OBJECT(LINKED(player, PLAYER(player)->object))->kind;
		kind = 0xFu;
		if (caged_character != 0u)
		{
			kind = 0x1Au;
			if (caged_character != 1u)
				kind = 0x1Bu;
		}
		frames = 0x78u;
		screen_x = 0xA0u;
		screen_y = 0x40u;
		spawn_message_box(screen_x, screen_y, kind, frames, sign);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x14u)
	{
		sound = 0x34u;
		play_sound(sound, &ignored, &ignored_2);
	}
	if ((uint16_t)OBJECT(current_object)->spiked_press.frame_count == 0x50u)
	{
		sound = 0x3Fu;
		if (caged_character != 0u)
		{
			sound = 0x5Cu;
			if (caged_character != 1u)
				sound = 0x40u;
		}
		play_sound(sound, &ignored, &ignored_2);
	}
	if (OBJECT(current_object)->y_int >= 0x3D0)
	{
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->spiked_press.stopped_frames++;
		scroll_mode = 0u;
		scroll_speed = 5u;
	}
	if (stage != 5u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->speed_y_fraction != 0u && OBJECT(current_object)->spiked_press.frame_count >= 0xC8 && OBJECT(current_object)->spiked_press.holding_back == 0u)
	{
		sound_timer = OBJECT(current_object)->spiked_press.sound_timer;
		OBJECT(current_object)->spiked_press.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->spiked_press.sound_timer = 0x3C;
			sound = 0xDu;
			play_sound(sound, &ignored, &ignored_2);
		}
	}
	OBJECT(current_object)->health = 0x7F;
	project_object(&ignored_3, &ignored);
	add_to_touchable_list(ignored_3);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* Stage list object of stage 5: once ten objects are free, ten bars 0x18 apart in x from its position
   (sprite 0x7058, kind 0xC, box 0x20 x 0x10, height 0xE0, cage_bar_animation, handler cage_bar_start),
   cage_bar_hit cleared; then removes itself. */
void cage_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t bars_left_before = 0;

	cage_bars_left = 0xA;
	if (free_object_stack < cage_bars_left)
		return;
	cage_bar_x = OBJECT(current_object)->x_int;
	cage_bar_y = OBJECT(current_object)->y_int;
	cage_bar_hit = 0u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x7058u;
		OBJECT(object)->sprite_palette = 0x90F0u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 8;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x10u;
		OBJECT(object)->hitbox_offset_x = -0x10;
		OBJECT(object)->hitbox_offset_y = -0xC;
		OBJECT(object)->hitbox_height = 0xE0u;
		OBJECT(object)->animation = cage_bar_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = -1;
		OBJECT(object)->health = 5;
		OBJECT(object)->handler = cage_bar_start_ADDRESS;
		OBJECT(object)->x_int = cage_bar_x;
		OBJECT(object)->x_fraction = 0u;
		OBJECT(object)->y_int = cage_bar_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0;
		cage_bar_x += 0x18u;
		bars_left_before = cage_bars_left;
		cage_bars_left = bars_left_before - 1u;
	} while ((int16_t)bars_left_before > 1);
	delete_current_object();
}

/* Projects the bar, then cage_bar_update from this frame on. */
void cage_bar_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = cage_bar_update_ADDRESS;
	cage_bar_update();
}

/* A bar of the stage 5 cage, kept at health 5. Once the spiked press has counted 0xC8 frames, a hit by
   an attacking player bends it (the next frame of cage_bar_animation, cage_bar_hit set, sound 0x21) and
   the next breaks it: 8 frames later the broken frame 1 pixel right, no longer touchable. Removed
   outside stage 5 or off screen. */
void cage_bar_update(void)
{
	uint8_t sound_id = 0;
	uint32_t animation_frame = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, off_screen = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	OBJECT(current_object)->health = 5;
	if (OBJECT(LINKED(current_object, spiked_press_object))->spiked_press.frame_count >= 0xC8 && (int8_t)OBJECT(current_object)->animation_frame >= 0 && (OBJECT(current_object)->flags & 4u) != 0u && (OBJECT(current_object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->animation_frame++;
		animation_frame = OBJECT(current_object)->animation_frame;
		step_list = OBJECT(current_object)->animation;
		last_step = CAGE_BAR_ANIMATION(step_list)->last_frame;
		step_list += 2u;
		if ((int16_t)animation_frame < (int16_t)last_step)
		{
			OBJECT(current_object)->sprite_frame = SPRITE_FRAME_NUMBERS(step_list)[animation_frame];
			OBJECT(current_object)->health = 1;
			cage_bar_hit = 0xFFu;
		}
		else
		{
			OBJECT(current_object)->animation_timer = 8;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
		sound_id = 0x21u;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	if ((int8_t)OBJECT(current_object)->animation_frame < 0 && OBJECT(current_object)->animation_timer >= 0)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer < 1)
		{
			OBJECT(current_object)->sprite_frame = CAGE_BAR_ANIMATION(OBJECT(current_object)->animation)->broken_frame;
			OBJECT(current_object)->projected_x++;
		}
	}
	update_screen_position(&ignored_3, &ignored);
	if (stage != 5u)
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
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		add_to_touchable_list(ignored_3);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
	queue_sprite_at_depth(&ignored_3, &depth);
}
