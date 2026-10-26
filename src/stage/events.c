/**
 *    Name:        stage/events.c
 *    Description: the scripted game modes run_game_mode jumps into and what they spawn: platforms, rocks, lightning, message boxes, the escape alarm and countdown
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Stage 0xB's escape alarm: on its first frame sets bit 0x4000 in the NBG0-3 and bitmap blend words (and
   in escape_alarm_stray_flags, which nothing reads), colour offsets red 0, blue and green 0x40; then
   every 2 frames blue and green swing between 0x28 and 0x3F, sound 0x19 at each end. Removed when
   escape_seconds is 0. */
void escape_alarm_flash(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (escape_seconds == 0u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->escape_alarm_flash.started == 0u)
	{
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		escape_alarm_stray_flags |= 0x4000u;
		color_offset_red = 0u;
		color_offset_blue = 0x40u;
		color_offset_green = 0x40u;
		OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
		OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
		OBJECT(current_object)->escape_alarm_flash.started++;
	}
	OBJECT(current_object)->escape_alarm_flash.step_timer++;
	if (OBJECT(current_object)->escape_alarm_flash.step_timer < 2)
		return;
	OBJECT(current_object)->escape_alarm_flash.step_timer = 0;
	if (OBJECT(current_object)->escape_alarm_flash.counting_up != 0u)
	{
		color_offset_blue++;
		color_offset_green++;
		if (color_offset_blue == 0x3Fu)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 0u;
			sound_id = 0x19u;
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
	else
	{
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_blue == 0x28u)
		{
			OBJECT(current_object)->escape_alarm_flash.counting_up = 1u;
			sound_id = 0x19u;
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
}

/* Game mode 0x0F step 0 (nothing sets that mode): scene_timer 0, scroll_mode 0 at speed 3
   (scroll_speed_low), scene_step + 1. */
void event_0f_start(void)
{
	scene_timer = 0;
	scroll_mode = 0u;
	scroll_speed_low = 3;
	scene_step++;
}

/* Game mode 0x0F step 1: scene_timer + 1; from 0x3C the scroll stops (scroll_mode 1); at 0x78
   event_0f_message_cursor = event_0f_messages, step 2 and scene_timer 0. */
void event_0f_stop_scroll(void)
{
	scene_timer++;
	if (scene_timer >= 0x3C)
		scroll_mode = 1u;
	if (scene_timer >= 0x78)
	{
		scene_timer = 0;
		event_0f_message_cursor = event_0f_messages_ADDRESS;
		scene_step++;
	}
}

/* Game mode 0x0F step 2: scene_timer + 1, and the next event_0f_messages record whose time has come
   opens a message box (spawn_message_box with its x, y, kind and frames); at 0x258 scroll_mode 0 at
   speed 5 and game mode 3. Nothing sets game mode 0x0F. */
void event_0f_show_messages(uint32_t *p_frames, uint32_t *p_box)
{
	uint16_t frames = (uint16_t)*p_frames;
	uint16_t kind = 0, screen_x = 0, screen_y = 0;
	uint32_t cursor = 0;
	uint16_t show_time = 0;

	scene_timer++;
	cursor = event_0f_message_cursor;
	show_time = EVENT_0F_MESSAGE(cursor)->show_time;
	cursor += 2u;
	if ((int16_t)show_time <= scene_timer)
	{
		screen_x = EVENT_0F_MESSAGE(cursor - 0x2u)->screen_x;
		cursor += 2u;
		screen_y = EVENT_0F_MESSAGE(cursor - 0x4u)->screen_y;
		cursor += 2u;
		kind = EVENT_0F_MESSAGE(cursor - 0x6u)->kind;
		cursor += 2u;
		frames = EVENT_0F_MESSAGE(cursor - 0x8u)->frames;
		event_0f_message_cursor = cursor + 2u;
		*p_box = spawn_message_box(screen_x, screen_y, kind, frames, *p_box);
	}
	if (scene_timer >= 0x258)
	{
		scroll_mode = 0u;
		scroll_speed_low = 5;
		game_mode = 3u;
	}
	*p_frames = (*p_frames & 0xFFFF0000u) | frames;
}

/* Unused: falling_rock_count rocks from falling_rocks: x, y, z, speed x and y (<< 12) and an animation
   each, palette 0x8240, sprite_flags 0x1000, animation speed 4, handler falling_rock_bounce; stops when
   no object is free. */
void spawn_falling_rocks(uint32_t object)
{
	uint32_t free_count = 0, rock_record = 0, rocks_left = 0;

	rocks_left = falling_rock_count;
	rock_record = falling_rocks_ADDRESS;
	do
	{
		allocate_object_outside_loop(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->x_int = FALLING_ROCK(rock_record)->x;
		OBJECT(object)->x_fraction = 0u;
		rock_record += 2u;
		OBJECT(object)->y_int = FALLING_ROCK(rock_record - 0x2u)->y;
		OBJECT(object)->y_fraction = 0u;
		rock_record += 2u;
		OBJECT(object)->z_int = FALLING_ROCK(rock_record - 0x4u)->z;
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = falling_rock_bounce_ADDRESS;
		rock_record += 2u;
		OBJECT(object)->speed_x = (uint32_t)FALLING_ROCK(rock_record - 0x6u)->speed_x << 12;
		rock_record += 2u;
		OBJECT(object)->speed_y = (uint32_t)FALLING_ROCK(rock_record - 0x8u)->speed_y << 12;
		OBJECT(object)->speed_z = 0;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		rock_record += 2u;
		OBJECT(object)->animation = FALLING_ROCK(rock_record - 0xAu)->animation;
		rock_record += 4u;
		rocks_left--;
	} while (rocks_left != 0u);
}

/* Unused: a rock from spawn_falling_rocks: moves by speed_x/speed_y and falls (gravity 0x6000,
   find_floor), bouncing off its floor at half the speed while that is over 0x20000, then lies still
   (flag 0x400); animated while it moves and drawn (queue_sprite) until check_left_screen sees it leave. */
void falling_rock_bounce(void)
{
	uint32_t bounce_speed = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, off_screen = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		project_object(&ignored, &ignored_2);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			ignored_2 = queue_sprite(ignored_2);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		animate(&ignored_2, &ignored_6, &ignored_7);
		project_object(&ignored, &ignored_2);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			ignored_2 = queue_sprite(ignored_2);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	bounce_speed = (int32_t)OBJECT(current_object)->speed_z >> 1;
	if ((int32_t)bounce_speed > 0x20000)
	{
		OBJECT(current_object)->speed_z = 0u - bounce_speed;
		animate(&ignored_2, &ignored_6, &ignored_7);
		project_object(&ignored, &ignored_2);
		off_screen = check_left_screen();
		if (off_screen == 0u)
		{
			ignored_2 = queue_sprite(ignored_2);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->flags |= 0x400u;
	animate(&ignored_2, &ignored_6, &ignored_7);
	project_object(&ignored, &ignored_2);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	delete_current_object();
}

/* Game mode 0x0C step 0: scene_timer 0x3C, the rollers behind the door (spawn_door_rollers), scene_step
   + 1, scroll_mode 0 at scroll_speed 2. */
void event_0c_start(uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_object)
{
	scene_timer = 0x3C;
	spawn_door_rollers(p_block_offset, p_slope_height, p_object);
	scene_step++;
	scroll_mode = 0u;
	scroll_speed = 2u;
}

/* Game mode 0x0C step 1: the scroll stops (scroll_mode 1) once scene_timer is down to 0x1E; at 0x3C, its
   first frame, camera_shake_active and sound 0xF; when scene_timer runs out, step 2 with scene_timer
   0x3C. */
uint32_t event_0c_first_quake(uint32_t write_index_address)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;
	uint16_t time_left = 0;

	if (scene_timer <= 0x1E)
		scroll_mode = 1u;
	if ((uint16_t)scene_timer == 0x3Cu)
	{
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &ignored, &write_index_address);
	}
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		scene_step++;
		scene_timer = 0x3C;
	}
	return write_index_address;
}

/* Game mode 0x0C step 2: camera_shake_active and sound 0xF at scene_timer 0x3C (its first frame),
   spawn_walls every frame; when scene_timer runs out, step 3 with scene_timer 0xA and sound 0xA. */
void event_0c_second_quake(uint32_t *p_part_size, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_wall)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t time_left = 0;

	if ((uint16_t)scene_timer == 0x3Cu)
	{
		camera_shake_active = 1u;
		sound_id = 0xFu;
		play_sound(sound_id, &ignored, p_part_size);
	}
	spawn_walls(&ignored, p_part_size, &ignored_2, p_point_y, p_z, p_wall);
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left > 1)
	{
		return;
	}
	scene_step++;
	scene_timer = 0xA;
	sound_id = 0xAu;
	play_sound(sound_id, &ignored, p_part_size);
}

/* Game mode 0x0C step 3: when scene_timer runs out (0xA frames), game mode 3 and scroll_mode 7
   (following the players). */
void event_0c_finish(void)
{
	uint16_t time_left = 0;

	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		game_mode = 3u;
		scroll_mode = 7u;
	}
}

/* Stage 5's end, by scene_step: 0-1 the stage frame with goal_unlocked set until check_stage_clear moves
   to game mode 0x17 (kept at 0x0A); 2 run_stage_05_switch to stage 0 and its stone platform, 0xB4
   frames; 3 Eggman's booth and a message box; 4 a 0x3C-frame quake; 5 the geyser and the rocks; 6-7
   update_stage_00_exit_scene, then game mode 2. */
void update_stage_05_event(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t sound_id = 0, spawned = 0;
	uint16_t kind = 0, screen_x = 0, screen_y = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, piece_record = 0, scroll_y = 0;

	spawned = (uint8_t)(scene_step * 3u);
	switch (scene_step)
	{
	case 0:
	case 8:
		scene_timer = 0;
		unused_stage_05_event_long = 0u;
		scene_step++;
		update_player_summary(&speed, p_count, &ignored, &object);
		update_scroll(&position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		update_stage_tile_layers(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
		check_walls(ignored_3);
		check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
		goal_unlocked = 0xFFu;
		check_stage_clear(p_count, p_list_cursor, &ignored_5);
		if (game_mode != 0x17u)
		{
			return;
		}
		game_mode = 0xAu;
		scene_step++;
		scene_timer = 0;
		goal_scroll_position = 0xFFFFu;
		return;
	case 1:
		update_player_summary(&speed, p_count, &ignored, &object);
		update_scroll(&position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		update_stage_tile_layers(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
		check_walls(ignored_3);
		check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
		goal_unlocked = 0xFFu;
		check_stage_clear(p_count, p_list_cursor, &ignored_5);
		if (game_mode != 0x17u)
		{
			return;
		}
		game_mode = 0xAu;
		scene_step++;
		scene_timer = 0;
		goal_scroll_position = 0xFFFFu;
		return;
	case 2:
		if (stage_05_switch_step < 0)
		{
			find_nearby_floor_blocks(&ignored_3, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored, &ignored_5);
			check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
			check_walls(ignored_3);
			check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
			scene_timer++;
			if (scene_timer < 0xB4)
			{
				return;
			}
			scene_step++;
			scene_timer = 0;
			stage_init_flags &= ~0x20u;
			return;
		}
		run_stage_05_switch(speed, &position, p_speed_value, &object);
		if ((uint8_t)stage_05_switch_step == 2u)
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count != 0u)
				OBJECT(object)->handler = stage_00_platform_start_ADDRESS;
		}
		if (stage_05_switch_step >= 0)
		{
			find_nearby_floor_blocks(&ignored_3, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored, &ignored_5);
			check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
			check_walls(ignored_3);
			check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
			scene_timer++;
			if (scene_timer < 0xB4)
			{
				return;
			}
			scene_step++;
			scene_timer = 0;
			stage_init_flags &= ~0x20u;
			return;
		}
		load_stage_map_pointers(&ignored_3, &ignored);
		find_nearby_floor_blocks(&ignored_3, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored, &ignored_5);
		check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
		check_walls(ignored_3);
		check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
		scene_timer++;
		if (scene_timer < 0xB4)
		{
			return;
		}
		scene_step++;
		scene_timer = 0;
		stage_init_flags &= ~0x20u;
		return;
	case 3:
		if ((uint16_t)scene_timer == 0u)
			spawn_eggman_booth(spawned, 0u, speed, &position, &object);
		if ((uint16_t)scene_timer == 0xAu)
		{
			kind = 0x10u;
			*p_count = (*p_count & ~0xFFFFu) | 0xB4u;
			screen_y = 0x50u;
			stage_05_event_show_message(position, screen_y, kind, object, p_speed_value, p_count, p_cycle, p_list_cursor);
			return;
		}
		stage_05_event_step_3_frame(position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 4:
		camera_shake_active = 1u;
		camera_shake_length = 0x200u;
		camera_shake_wave = 0x200u;
		scroll_y = update_camera_with_shake();
		update_scroll(&scroll_y, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		update_stage_tile_layers(&ignored_3, &scroll_y, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		check_hits(&ignored_3, &scroll_y, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
		check_walls(ignored_3);
		check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
		if ((uint16_t)scene_timer == 1u || (uint16_t)scene_timer == 0x3Bu)
		{
			sound_id = 0xFu;
			play_sound(sound_id, &ignored_3, &ignored_6);
		}
		scene_timer++;
		if (scene_timer < 0x3C)
		{
			return;
		}
		scene_step++;
		return;
	case 5:
		allocate_object_outside_loop(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->flags = 0u;
			OBJECT(object)->handler = stage_00_geyser_rise_ADDRESS;
			stage_00_geyser = object;
		}
		*p_list_cursor = LINKED(WORK_RAM, stage_00_platform);
		screen_x = (uint16_t)OBJECT(*p_list_cursor)->screen_x;
		r14 = (r14 & ~0xFFFFu) | (uint16_t)OBJECT(*p_list_cursor)->screen_y;
		*p_count = stage_00_rock_count;
		piece_record = stage_00_rock_pieces_ADDRESS;
		do
		{
			allocate_object_outside_loop(&free_count, &object);
			if (free_count == 0u)
			{
				scene_timer = 0;
				sound_id = 6u;
				play_sound(sound_id, &ignored_3, &ignored_6);
				scene_step++;
				return;
			}
			OBJECT(object)->sprite_flags = ROCK_DEBRIS_PIECE(piece_record)->sprite_flags;
			OBJECT(object)->screen_x = screen_x;
			piece_record += 2u;
			OBJECT(object)->screen_x += ROCK_DEBRIS_PIECE(piece_record - 0x2u)->offset_x;
			OBJECT(object)->screen_y = r14;
			piece_record += 2u;
			OBJECT(object)->screen_y += ROCK_DEBRIS_PIECE(piece_record - 0x4u)->offset_y;
			piece_record += 2u;
			OBJECT(object)->speed_x_fraction = ROCK_DEBRIS_PIECE(piece_record - 0x6u)->speed_x;
			piece_record += 2u;
			OBJECT(object)->speed_y_fraction = ROCK_DEBRIS_PIECE(piece_record - 0x8u)->speed_y;
			piece_record += 2u;
			OBJECT(object)->animation = rock_debris_animations_ADDRESS + ROCK_DEBRIS_PIECE(piece_record - 0xAu)->animation_offset;
			OBJECT(object)->sprite_palette = 0x8240u;
			OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
			OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->animation_speed = 4u;
			OBJECT(object)->animation_timer = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->sprite_scale = 0x100u;
			OBJECT(object)->handler = rock_debris_fly_ADDRESS;
			(*p_count)--;
			piece_record += 2u;
		} while (*p_count != 0u);
		scene_timer = 0;
		sound_id = 6u;
		play_sound(sound_id, &ignored_3, &ignored_6);
		scene_step++;
		return;
	case 6:
		scene_timer++;
		if (scene_timer >= 0x1E)
			update_stage_00_exit_scene(object, p_speed_value, p_list_cursor);
		if ((uint16_t)scene_timer == 5u)
			sound_id = 6u;
		else
		{
			if ((uint16_t)scene_timer != 0x30u)
			{
				if (scene_timer < 0x186)
				{
					return;
				}
				scene_step++;
				scene_timer = 0;
				return;
			}
			sound_id = 6u;
		}
		play_sound(sound_id, &ignored_3, &ignored_6);
		if (scene_timer < 0x186)
		{
			return;
		}
		scene_step++;
		scene_timer = 0;
		return;
	case 7:
		update_stage_00_exit_scene(object, p_speed_value, p_list_cursor);
		scene_timer++;
		if (scene_timer < 0x12C)
		{
			return;
		}
		game_mode = 2u;
		return;
	default:
		invalid_jump(0x9493Cu + scene_step * 3u);
	}
}

/* Unused: like step 3's message, with message box kind 0x11 (sprite 0x65E8) at y 0x40 for 0x78 frames. */
void stage_05_event_show_message_11(uint32_t part_size, uint32_t box)
{
	uint16_t kind = 0, screen_y = 0;
	uint32_t frames = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	kind = 0x11u;
	frames = 0x78u;
	screen_y = 0x40u;
	stage_05_event_show_message(part_size, screen_y, kind, box, &ignored, &frames, &ignored_2, &ignored_3);
}

/* Unused: the same with message box kind 0x12 (sprite 0x65F0) at y 0x40 for 0x1E frames. */
void stage_05_event_show_message_12(uint32_t part_size, uint32_t box)
{
	uint16_t kind = 0, screen_y = 0;
	uint32_t frames = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	kind = 0x12u;
	frames = 0x1Eu;
	screen_y = 0x40u;
	stage_05_event_show_message(part_size, screen_y, kind, box, &ignored, &frames, &ignored_2, &ignored_3);
}

/* update_stage_05_event's step 3 at scene_timer 0xA: spawn_message_box(0xA0, screen_y, kind, frames) and
   sound 0x70, then stage_05_event_step_3_frame. */
void stage_05_event_show_message(uint32_t part_size, uint16_t screen_y, uint16_t kind, uint32_t box, uint32_t *p_sag, uint32_t *p_frames, uint32_t *p_z, uint32_t *p_platform)
{
	uint32_t sag = *p_sag;
	uint8_t sound_id = 0;
	uint16_t screen_x = 0;
	uint32_t ignored = 0;

	screen_x = 0xA0u;
	box = spawn_message_box(screen_x, screen_y, kind, (uint16_t)*p_frames, box);
	sound_id = 0x70u;
	play_sound(sound_id, &ignored, &part_size);
	stage_05_event_step_3_frame(part_size, box, &sag, p_frames, p_z, p_platform);
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
}

/* A frame of update_stage_05_event's step 3: spawn_walls and the contact checks (check_hits,
   check_walls, check_platforms); scene_timer + 1, and at 0xA0 the next step with scene_timer 0. */
void stage_05_event_step_3_frame(uint32_t part_size, uint32_t wall, uint32_t *p_sag, uint32_t *p_point_y, uint32_t *p_z, uint32_t *p_platform)
{
	uint32_t sag = *p_sag;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	spawn_walls(&ignored, &part_size, &ignored_2, p_point_y, p_z, &wall);
	check_hits(&ignored, &part_size, &ignored_3, &sag, &ignored_2, p_point_y, &ignored_4);
	check_walls(ignored);
	check_platforms(&ignored, &ignored_5, &ignored_3, &sag, &ignored_2, p_point_y, p_z, p_platform, &ignored_4);
	scene_timer++;
	if (scene_timer >= 0xA0)
	{
		scene_step++;
		scene_timer = 0;
	}
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
}

/* Waits while stage_05_switch_step is 2, then sets up the stone platform: sprite 0x6FF8, palette 0x9040,
   sprite_flags 0x1000, at x 0x185, y 0x1545, z 0 (project_object); stage_00_platform = it, handler
   stage_00_platform_stand. */
void stage_00_platform_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	if ((uint8_t)stage_05_switch_step == 2u)
		return;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x6FF8u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->x_int = 0x185;
	OBJECT(current_object)->x_fraction = 0u;
	OBJECT(current_object)->y_int = 0x1545;
	OBJECT(current_object)->y_fraction = 0u;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = stage_00_platform_stand_ADDRESS;
	stage_00_platform = current_object;
	stage_00_platform_stand();
}

/* The stone platform under the players: while game_mode is below 0x0A or scene_step below 5 it is queued
   0x3C buckets behind its depth (at most bucket 0xFF); at step 5 it takes palette 0x9030 and
   stage_00_platform_break_animation, moves (projected_x + 4, projected_y - 0x13) and goes on as
   stage_00_platform_break. */
void stage_00_platform_stand(void)
{
	uint16_t screen_y = 0;
	uint32_t bucket = 0, ignored = 0;

	if ((int8_t)(game_mode - 0xAu) < 0 || (int8_t)scene_step < 5)
	{
		update_screen_position(&bucket, &ignored);
		screen_y = (uint16_t)OBJECT(current_object)->screen_y;
		bucket = depth_to_bucket(bucket, screen_y);
		bucket = (uint16_t)(bucket + 0x3Cu);
		if ((int16_t)bucket > 0xFF)
			bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		return;
	}
	if (scene_step != 5u)
	{
		stage_00_platform_break();
		return;
	}
	OBJECT(current_object)->sprite_palette = 0x9030u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = stage_00_platform_break_animation_ADDRESS;
	OBJECT(current_object)->projected_x += 4u;
	OBJECT(current_object)->projected_y_int -= 0x13u;
	OBJECT(current_object)->handler = stage_00_platform_break_ADDRESS;
	stage_00_platform_break();
}

/* The platform breaking up: animates and is queued 0x3C buckets in front of its depth (depth_to_bucket -
   0x3C); removed once screen_y reaches 0x130. */
void stage_00_platform_break(void)
{
	uint16_t screen_y = 0;
	uint32_t bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	animate(&ignored, &ignored_2, &ignored_3);
	update_screen_position(&bucket, &ignored);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		screen_y = (uint16_t)OBJECT(current_object)->screen_y;
		bucket = depth_to_bucket(bucket, screen_y);
		bucket = (uint16_t)(bucket - 0x3Cu);
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

/* The top of the water spout bursting from stage_00_platform: starts at its position, z - 8, with
   stage_00_geyser_animation, and rises by speed_z from the stage_00_geyser_speeds steps (cursor kept in
   floor_z); every 0x1C of rise, 8 times, it adds a column segment (stage_00_geyser_column_start).
   Removed below screen y 0x130. */
void stage_00_geyser_rise(void)
{
	uint32_t cursor = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, object_2 = 0;
	uint16_t speed_y_fraction = 0, speed_z_int = 0, speed_x_int = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 4u;
	if ((flags & 4u) == 0u)
	{
		OBJECT(current_object)->sprite_flags = 0x1000u;
		OBJECT(current_object)->sprite_frame = 0x6F38u;
		OBJECT(current_object)->sprite_palette = 0x1010u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		object = LINKED(current_object, stage_00_platform);
		OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
		OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
		OBJECT(current_object)->z = OBJECT(object)->z;
		OBJECT(current_object)->z_int -= 8u;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 2;
		OBJECT(current_object)->animation = stage_00_geyser_animation_ADDRESS;
		OBJECT(current_object)->speed_x_int = 0x2B;
		OBJECT(current_object)->speed_x_fraction = 8u;
		OBJECT(current_object)->floor_z = stage_00_geyser_speeds_ADDRESS;
		OBJECT(current_object)->speed_y_fraction = 0u;
		OBJECT(current_object)->speed_z = 0;
	}
	speed_y_fraction = OBJECT(current_object)->speed_y_fraction;
	OBJECT(current_object)->speed_y_fraction = speed_y_fraction - 1u;
	if ((int16_t)speed_y_fraction <= 1)
	{
		cursor = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_y_fraction = GEYSER_RISE_STEP(cursor)->frames;
		cursor += 2u;
		OBJECT(current_object)->speed_z = (uint32_t)GEYSER_RISE_STEP(cursor - 0x2u)->rise_speed << 12;
		OBJECT(current_object)->floor_z = cursor + 2u;
	}
	OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->speed_x_fraction != 0u)
	{
		speed_z_int = OBJECT(current_object)->speed_z_int;
		speed_x_int = OBJECT(current_object)->speed_x_int;
		OBJECT(current_object)->speed_x_int = speed_x_int - speed_z_int;
		if ((int16_t)speed_x_int <= (int16_t)speed_z_int)
		{
			OBJECT(current_object)->speed_x_fraction--;
			OBJECT(current_object)->speed_x_int = 0x1C;
			allocate_object(&free_count, &object_2);
			if (free_count != 0u)
			{
				OBJECT(object_2)->handler = stage_00_geyser_column_start_ADDRESS;
				OBJECT(object_2)->parent = current_object;
			}
		}
	}
	update_screen_position(&ignored, &ignored_2);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		animate(&ignored_2, &ignored_3, &ignored_4);
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	delete_current_object();
}

/* A new column segment: sprite_flags 0x1000, palette 0x1010, at stage_00_platform's projected position
   with z + 8, then stage_00_geyser_column_rise. */
void stage_00_geyser_column_start(void)
{
	uint32_t object = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	object = LINKED(WORK_RAM, stage_00_platform);
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 8u;
	OBJECT(current_object)->handler = stage_00_geyser_column_rise_ADDRESS;
	stage_00_geyser_column_rise();
}

/* A segment of the water column under the geyser's top (parent): rises with it (z - its speed_z), takes
   stage_00_geyser_column_sprites[its animation_frame] and is drawn, until it is below screen y 0x130,
   when it is removed. */
void stage_00_geyser_column_rise(void)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->z -= OBJECT(object)->speed_z;
	OBJECT(current_object)->sprite_frame = stage_00_geyser_column_sprites[OBJECT(object)->animation_frame];
	update_screen_position(&ignored, &ignored_2);
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	delete_current_object();
}

/* A stone fragment in screen space: its 1/16-pixel position (projected_x, projected_y_fraction) moves by
   speed_x_fraction/speed_y_fraction, screen = position / 16, removed 0x20 off the screen. Pieces whose
   sprite_flags have no bits above 0xFFF grow (zoom + 0x30 a frame, removed at 0x600). Animated, bucket
   1. */
void rock_debris_fly(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	OBJECT(current_object)->projected_x += OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->screen_x = (int16_t)OBJECT(current_object)->projected_x >> 4;
	OBJECT(current_object)->projected_y_fraction += OBJECT(current_object)->speed_y_fraction;
	OBJECT(current_object)->screen_y = (int16_t)OBJECT(current_object)->projected_y_fraction >> 4;
	if ((uint16_t)(OBJECT(current_object)->screen_x + 0x20u) >= 0x180u || (uint16_t)(OBJECT(current_object)->screen_y + 0x20u) >= 0x120u)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->sprite_flags & ~0xFFFu) == 0u)
	{
		OBJECT(current_object)->sprite_scale += 0x30u;
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x600)
		{
			delete_current_object();
			return;
		}
	}
	animate(&ignored, &ignored_2, &ignored_3);
	bucket = 1u;
	queue_sprite_in_bucket(bucket);
}

/* Game mode 0x1C step 1: follow_base_speed 5, scroll_mode_low 7 (1 while no_player_in_play); at
   scroll_position 0x7E9F step 2, scene_timer 0x1E and event_1c_ride_phase 0xFF. */
void event_1c_follow(void)
{
	follow_base_speed = 5u;
	scroll_mode_low = 7u;
	if (no_player_in_play != 0u)
		scroll_mode_low = 1u;
	if (scroll_position >= 0x7E9Fu)
	{
		scene_step++;
		scene_timer = 0x1E;
		event_1c_ride_phase = 0xFFu;
	}
}

/* Game mode 0x1C step 2: follow_base_speed 0, scroll_mode_low 1 (stopped); when scene_timer runs out,
   step 3 and event_1c_riding 0xFF. */
void event_1c_stop(void)
{
	uint16_t time_left = 0;

	follow_base_speed = 0u;
	scroll_mode_low = 1u;
	time_left = scene_timer;
	scene_timer = time_left - 1u;
	if ((int16_t)time_left <= 1)
	{
		scene_step++;
		event_1c_riding = 0xFFu;
	}
}

/* Game mode 0x1C step 3: scroll_mode_low 0 at scroll_speed_low 0xF. */
void event_1c_scroll(void)
{
	scroll_mode_low = 0u;
	scroll_speed_low = 0xF;
}

/* Game mode 0x18 (stage 0x11, the ending): scene_step 0xFF on its first frame, next_stage_slot_request
   0, joining_closed 0xFF, build_ending_scene, the floor blocks, the restart point, every spawner and the
   contact checks; when every player_status is 0, game mode 7 (saved_game_mode keeps the mode). */
void update_ending(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_platform)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	if (scene_step == 0u)
		scene_step = 0xFFu;
	next_stage_slot_request = 0u;
	joining_closed = 0xFFu;
	build_ending_scene(&position, &speed, p_speed_value, p_count, p_cycle, &object);
	find_nearby_floor_blocks(&ignored, &speed, p_speed_value, &ignored_2, p_count, &ignored_3, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored, &position, &speed, p_speed_value, &ignored_2, p_count, &object);
	spawn_ring_rows(&ignored, &position, &ignored_4, &object);
	spawn_walls(&ignored, &position, &ignored_2, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored, p_count, &object);
	spawn_object_groups(&ignored, &position, &ignored_4, p_count, &object);
	spawn_objects_by_handler(&ignored, &ignored_3, &object);
	ignored = check_zones(ignored);
	check_hits(&ignored, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
	check_walls(ignored);
	check_platforms(&ignored, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_platform, &ignored_5);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
}

/* Stage 6's lightning: the first call (stage_init_flags 0x1000) starts stage_06_lightning_cursor at
   stage_06_lightning_strikes; when scene_timer reaches the next record's time, a bolt at its screen x/y
   with its animation, palette 0x9110, sprite_flags 0x1008, handler stage_06_lightning_animate, and sound
   0xB. Waits while no object is free. */
void spawn_stage_06_lightning(uint32_t *p_record, uint32_t *p_bolt)
{
	uint32_t bolt = *p_bolt;
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, record = 0;
	uint16_t strike_time = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x1000u;
	if ((init_flags & 0x1000u) == 0u)
		stage_06_lightning_cursor = stage_06_lightning_strikes_ADDRESS;
	record = stage_06_lightning_cursor;
	strike_time = STAGE_06_LIGHTNING_STRIKE(record)->strike_time;
	record += 2u;
	if ((int16_t)strike_time <= scene_timer)
	{
		allocate_object_outside_loop(&free_count, &bolt);
		if (free_count != 0u)
		{
			OBJECT(bolt)->sprite_flags = 0x1008u;
			OBJECT(bolt)->sprite_palette = 0x9110u;
			OBJECT(bolt)->flags = 0u;
			OBJECT(bolt)->handler = stage_06_lightning_animate_ADDRESS;
			OBJECT(bolt)->screen_x = STAGE_06_LIGHTNING_STRIKE(record - 0x2u)->screen_x;
			record += 2u;
			OBJECT(bolt)->screen_y = STAGE_06_LIGHTNING_STRIKE(record - 0x4u)->screen_y;
			record += 2u;
			OBJECT(bolt)->animation = STAGE_06_LIGHTNING_STRIKE(record - 0x6u)->animation;
			OBJECT(bolt)->animation_frame = 0u;
			OBJECT(bolt)->animation_timer = 0;
			OBJECT(bolt)->animation_speed = 4u;
			record += 4u;
			stage_06_lightning_cursor = record;
			sound_id = 0xBu;
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
	*p_record = record;
	*p_bolt = bolt;
}

/* A lightning bolt: animates and is drawn while current_game_mode is 0x10; removed at the end of its
   animation or once the game mode changes. */
void stage_06_lightning_animate(void)
{
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0;

	if (current_game_mode == 0x10u)
	{
		animate(&ignored, &ignored_2, &at_last_step);
		if ((uint8_t)at_last_step == 0u)
		{
			ignored = queue_sprite(ignored);
			return;
		}
	}
	delete_current_object();
}

/* Opens a message box: allocate_object_outside_loop, then place_message_box with screen_x, screen_y,
   kind and frames; hands back the box (left as it was when no object is free). */
uint32_t spawn_message_box(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint16_t frames, uint32_t box)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &box);
	if (free_count == 0u)
		return box;
	place_message_box(screen_x, screen_y, kind, frames, box);
	return box;
}

/* Unused: spawn_message_box with allocate_object instead of allocate_object_outside_loop. */
void spawn_message_box_2(uint16_t screen_x, uint16_t screen_y, uint16_t kind, uint16_t frames, uint32_t box)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &box);
	if (free_count == 0u)
		return;
	place_message_box(screen_x, screen_y, kind, frames, box);
}

/* Sets up box as a message box: at screen_x, screen_y, frames kept in speed_x_fraction, and from
   message_box_kinds[kind] its sprite, palette, full width (speed_x_int) and height (sprite_height);
   sprite_flags 0x9000, width 0, handler message_box_show, and sprite_scale = the word at +6 of its
   frame's record (sprite_frames). */
void place_message_box(uint16_t screen_x, uint16_t screen_y, uint32_t kind, uint16_t frames, uint32_t box)
{
	OBJECT(box)->screen_x = screen_x;
	OBJECT(box)->screen_y = screen_y;
	OBJECT(box)->speed_x_fraction = frames;
	kind = (uint16_t)kind * 8u + message_box_kinds_ADDRESS;
	OBJECT(box)->sprite_frame = MESSAGE_BOX_KIND(kind)->sprite_frame;
	kind += 2u;
	OBJECT(box)->sprite_palette = MESSAGE_BOX_KIND(kind - 0x2u)->palette;
	kind += 2u;
	OBJECT(box)->speed_x_int = MESSAGE_BOX_KIND(kind - 0x4u)->width;
	OBJECT(box)->sprite_height = MESSAGE_BOX_KIND(kind - 0x4u)->height;
	OBJECT(box)->sprite_flags = 0x9000u;
	OBJECT(box)->flags = 0u;
	OBJECT(box)->sprite_width = 0;
	OBJECT(box)->handler = message_box_show_ADDRESS;
	OBJECT(box)->sprite_scale = mem_read16(LINKED(sprite_frames, OBJECT(box)->sprite_frame) + 6u);
}

/* A message box: while its frames (speed_x_fraction) last, sprite_width grows 0x18 a frame up to the
   full width (speed_x_int); then it shrinks 0x18 a frame and is removed. Drawn in bucket 0, on top. */
void message_box_show(void)
{
	uint8_t bucket = 0;
	uint16_t speed_x_fraction = 0, sprite_width = 0;

	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction > 1)
	{
		OBJECT(current_object)->sprite_width += 0x18u;
		if (OBJECT(current_object)->speed_x_int < OBJECT(current_object)->sprite_width)
			OBJECT(current_object)->sprite_width = OBJECT(current_object)->speed_x_int;
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	OBJECT(current_object)->speed_x_fraction = 0u;
	sprite_width = OBJECT(current_object)->sprite_width;
	OBJECT(current_object)->sprite_width = sprite_width - 0x18u;
	if ((int16_t)sprite_width > 0x18)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	delete_current_object();
}

/* Sets up the escape's countdown: two pool 2 digit sprites (palette 0xF60, centre x 0xA0 in projected_x,
   y 0x80) into escape_tens_digit and escape_units_digit; escape_countdown_frames 0, escape_seconds 0x14
   (20), escape_started and escape_countdown_running 0. */
void start_escape_countdown(uint32_t *p_free_stack, uint32_t *p_count, uint32_t *p_digit)
{
	uint32_t digit = *p_digit;
	uint32_t count = 0, digit_slot = 0, ignored = 0;

	digit_slot = escape_tens_digit_ADDRESS;
	count = 2u;
	do
	{
		allocate_object_2(&ignored, p_free_stack, &digit);
		OBJECT(digit)->sprite_flags = 0u;
		OBJECT(digit)->sprite_palette = 0xF60u;
		OBJECT(digit)->projected_x = 0xA0;
		OBJECT(digit)->screen_y = 0x80;
		OBJECT(digit)->sprite_scale = 1u;
		mem_write16(digit_slot, digit);
		count--;
		digit_slot += 2u;
	} while (count != 0u);
	escape_countdown_frames = 0;
	escape_seconds = 0x14u;
	escape_started = 0u;
	escape_countdown_running = 0u;
	*p_count = count;
	*p_digit = digit;
}

/* While escape_started: escape_countdown_frames + 1 (held at 0x3B until escape_countdown_running is set)
   and at 0x3C escape_seconds - 1. The seconds are drawn on top as two digits (escape_countdown_digits)
   whose zoom grows with the frames (frames * 9); the tens only when not 0, the units centred then. */
void update_escape_countdown(void)
{
	uint32_t ignored = 0, seconds = 0, seconds_bcd = 0, tens_digit = 0;
	uint16_t tens = 0;

	if (escape_started == 0u)
		return;
	if (escape_countdown_running != 0u || escape_countdown_frames < 0x3B)
	{
		escape_countdown_frames++;
		if (escape_countdown_frames >= 0x3C)
		{
			escape_countdown_frames = 0;
			escape_seconds--;
		}
	}
	seconds = escape_seconds;
	convert_to_bcd(&seconds, &ignored);
	escape_seconds_bcd = seconds;
	escape_digit_scale = (uint8_t)escape_countdown_frames * 9u;
	current_object = LINKED(WORK_RAM, escape_tens_digit);
	OBJECT(current_object)->sprite_scale = escape_digit_scale;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - ((uint16_t)(OBJECT(current_object)->sprite_scale * 0xCu) >> 8);
	seconds_bcd = escape_seconds_bcd;
	tens = seconds_bcd >> 4;
	tens_digit = tens;
	if (tens != 0u)
	{
		OBJECT(current_object)->sprite_frame = escape_countdown_digits[tens_digit];
		queue_sprite_on_top();
	}
	current_object = LINKED(WORK_RAM, escape_units_digit);
	OBJECT(current_object)->sprite_scale = escape_digit_scale;
	if ((escape_seconds_bcd & 0xF0u) == 0u)
		OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x;
	else
		OBJECT(current_object)->screen_x = ((uint16_t)(OBJECT(current_object)->sprite_scale * 0xCu) >> 8) + OBJECT(current_object)->projected_x;
	seconds_bcd = escape_seconds_bcd;
	OBJECT(current_object)->sprite_frame = escape_countdown_digits[seconds_bcd & 0xFu];
	queue_sprite_on_top();
}

/* Spawns the escape alarm (allocate_object_outside_loop): an object with handler escape_alarm_flash,
   started 0. */
void start_escape_alarm_flash(uint32_t flash)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &flash);
	if (free_count != 0u)
	{
		OBJECT(flash)->escape_alarm_flash.started = 0u;
		OBJECT(flash)->handler = escape_alarm_flash_ADDRESS;
	}
}
