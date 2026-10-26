/**
 *    Name:        scenes/map.c
 *    Description: the map screen between stages (game mode 4): the wipe, the island map, the trail, the markers, Eggman
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Game mode 4, the map screen, by map_screen_step: 0 the window wipe in around the stage
   (map_wipe_in_windows, start_window_wipe), bit 0x4000 set in the mixer blend words, 1 the fade
   (update_fade), 2 spawns map_intro_update, 3-5 waits while the map's objects run. Its caller
   (game_mode_stage_end_fade) goes to game mode 2 at step 5. */
uint32_t run_map_screen(uint32_t bottom_speed, uint32_t object)
{
	uint32_t free_count = 0, record = 0;

	switch (map_screen_step)
	{
	case 0:
	case 6:
		break;
	case 1:
		update_fade(0u);
		if (fade_kind < 0)
			map_screen_step++;
		return bottom_speed;
	case 2:
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			OBJECT(object)->handler = map_intro_update_ADDRESS;
			OBJECT(object)->map_intro.timer = 0;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->map_intro.base_x = window_wipe_left;
			OBJECT(object)->map_intro.base_y = window_wipe_top;
			OBJECT(object)->map_intro.step_x = 0;
			OBJECT(object)->map_intro.step_y = 0;
			OBJECT(object)->map_intro.offset_x = 0;
			OBJECT(object)->map_intro.offset_y = 0;
			map_screen_step++;
		}
		return bottom_speed;
	case 3:
	case 4:
	case 5:
		return bottom_speed;
	default:
		invalid_jump(0x95983u + map_screen_step * 3u);
	}
	mixer_text_blend |= 0x4000u;
	mixer_nbg0_blend |= 0x4000u;
	mixer_nbg1_blend |= 0x4000u;
	mixer_nbg2_blend |= 0x4000u;
	mixer_nbg3_blend |= 0x4000u;
	mixer_bitmap_blend |= 0x4000u;
	mixer_sprite_control |= 0x8000u;
	fade_kind = 0;
	fade_speed = 1u;
	window_wipe_closing = 0u;
	record = map_wipe_in_windows_ADDRESS + stage * 6u;
	bottom_speed = start_window_wipe(record);
	map_screen_step++;
	return bottom_speed;
}

/* Unused: a step of run_map_screen that its jump table does not list: update_screen_wipe until the four
   edges are done (edges_done & 0xF = 0xF), then map_screen_step + 1. */
void run_map_screen_wait_for_wipe(uint32_t edges_done)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	update_screen_wipe(&ignored, &ignored_2, &ignored_3, &edges_done);
	if ((edges_done & 0xFu) == 0xFu)
		map_screen_step++;
}

/* The map screen's first object, by animation_frame. Step 0 adds 4, so steps 1-3 (the icon from
   stage_map_icons flying to the corner) never run; 4 loads the island map (map_monitor_script) and
   Eggman's head (map_eggman_head_script) and starts the fade; 6 fades in over the markers, then spawns
   map_screen_update; 7 removes it. */
void map_intro_update(void)
{
	uint16_t icon_y = 0;
	uint32_t dest = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, palette_list = 0, script = 0, stage_number = 0, step_y = 0;
	uint8_t io_cnt_value = 0;

	switch (OBJECT(current_object)->animation_frame)
	{
	case 0:
	case 8:
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->map_intro.timer++;
		if (OBJECT(current_object)->map_intro.timer >= 0x1E)
			map_screen_active = 0xFFu;
		return;
	case 1:
		OBJECT(current_object)->animation_frame++;
		return;
	case 2:
		step_y = (int32_t)((0x10u - (uint16_t)OBJECT(current_object)->map_intro.base_y) << 16) / 0x3C;
		OBJECT(current_object)->map_intro.step_x = (int32_t)((0x20u - (uint16_t)OBJECT(current_object)->map_intro.base_x) << 16) / 0x3C;
		OBJECT(current_object)->map_intro.step_y = step_y;
		OBJECT(current_object)->map_intro.timer = 0x3C;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		mem_write16(backdrop_color, 0u);
		OBJECT(current_object)->animation_frame++;
		stage_number = stage;
		map_stage_icon = stage_map_icons[stage_number & 0x7FFFu];
		return;
	case 3:
		move_map_stage_icon();
		return;
	case 4:
		io_cnt_value = mem_read8(io_cnt);
		mem_write8(io_cnt, io_cnt_value & ~2u);
		display_off_frames = 2u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		mem_write16(backdrop_color, 0u);
		mem_write32(video_control, mem_read32(video_control) & ~0x400u);
		vblank_copies_pending = 0u;
		vblank_copy_count = 0u;
		palette_list = tile_bank_0_palette_list_ADDRESS;
		load_palette_list(palette_list, &ignored, &dest);
		script = map_monitor_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &dest);
		script = map_eggman_head_script_ADDRESS;
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		run_vram_script(&ignored_2, &script, &dest);
		nbg0_pages = 0x1010101u;
		nbg2_pages = 0u;
		fade_kind = 2;
		fade_speed = 1u;
		fade_step = 0u;
		OBJECT(current_object)->animation_frame++;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		update_fade(ignored_2);
		return;
	case 5:
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xEu);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~5u);
		OBJECT(current_object)->animation_frame++;
		return;
	case 6:
		update_fade(0u);
		if (color_offset_red != 0x3Fu)
		{
			draw_map_markers_and_trail();
			return;
		}
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			mixer_nbg0_blend &= 0xBFFFu;
			mixer_nbg1_blend &= 0xBFFFu;
			OBJECT(object)->handler = map_screen_update_ADDRESS;
			icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->map_intro.offset_y >> 16) + OBJECT(current_object)->map_intro.base_y);
			OBJECT(object)->map_screen.base_x = ((int32_t)OBJECT(current_object)->map_intro.offset_x >> 16) + OBJECT(current_object)->map_intro.base_x;
			OBJECT(object)->map_screen.base_y = icon_y;
			OBJECT(object)->sprite_frame = map_stage_icon;
			OBJECT(object)->sprite_palette = 0x6B0u;
			OBJECT(object)->sprite_scale = 0x100u;
			OBJECT(object)->sprite_layer = 1u;
			OBJECT(object)->screen_x = 0x20;
			OBJECT(object)->screen_y = 0x10;
			OBJECT(object)->sprite_flags = 0x10Au;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->map_screen.timer = 0;
			OBJECT(current_object)->animation_frame++;
			object = draw_map_stage_markers(object);
			draw_map_trail_dot(object);
		}
		return;
	case 7:
		delete_current_object();
		return;
	default:
		invalid_jump(0x95A5Fu + OBJECT(current_object)->animation_frame * 3u);
	}
}

/* The stage icon's flight: offset + step each frame, timer - 1 (the caller's animation_frame + 1 at 0),
   drawn at base + offset blinking with sprite 0x6538 every other frame (palette 0x6B0, layer 1). At
   0x30,0x10 it stays put and only blinks. */
void move_map_stage_icon(void)
{
	uint16_t icon_x = 0, icon_y = 0;

	icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_y >> 16) + OBJECT(current_object)->move_map_stage_icon.base_y);
	if ((uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_x >> 16) + OBJECT(current_object)->move_map_stage_icon.base_x) == 0x30u && icon_y == 0x10u)
	{
		icon_x = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_x >> 16) + OBJECT(current_object)->move_map_stage_icon.base_x);
		icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_y >> 16) + OBJECT(current_object)->move_map_stage_icon.base_y);
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->move_map_stage_icon.blink_phase++;
		OBJECT(current_object)->move_map_stage_icon.blink_phase &= 1u;
		if (OBJECT(current_object)->move_map_stage_icon.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = icon_x;
		OBJECT(current_object)->screen_y = icon_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		return;
	}
	OBJECT(current_object)->move_map_stage_icon.timer--;
	if ((uint16_t)OBJECT(current_object)->move_map_stage_icon.timer == 0u)
		OBJECT(current_object)->animation_frame++;
	OBJECT(current_object)->move_map_stage_icon.offset_x += OBJECT(current_object)->move_map_stage_icon.step_x;
	OBJECT(current_object)->move_map_stage_icon.offset_y += OBJECT(current_object)->move_map_stage_icon.step_y;
	icon_x = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_x >> 16) + OBJECT(current_object)->move_map_stage_icon.base_x);
	icon_y = (uint16_t)(((int32_t)OBJECT(current_object)->move_map_stage_icon.offset_y >> 16) + OBJECT(current_object)->move_map_stage_icon.base_y);
	OBJECT(current_object)->sprite_frame = map_stage_icon;
	OBJECT(current_object)->move_map_stage_icon.blink_phase++;
	OBJECT(current_object)->move_map_stage_icon.blink_phase &= 1u;
	if (OBJECT(current_object)->move_map_stage_icon.blink_phase == 0u)
		OBJECT(current_object)->sprite_frame = 0x6538u;
	OBJECT(current_object)->sprite_palette = 0x6B0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->screen_x = icon_x;
	OBJECT(current_object)->screen_y = icon_y;
	OBJECT(current_object)->sprite_flags = 0x10Au;
	queue_sprite_in_layer();
}

/* draw_map_stage_markers, then start_map_trail and draw_map_trail_dot: the markers with the trail's
   first dot, the trail starting over at every call (map_intro_update's fade-in). */
void draw_map_markers_and_trail(void)
{
	uint32_t badge_offset = 0;

	badge_offset = draw_map_stage_markers(0u);
	start_map_trail();
	draw_map_trail_dot(badge_offset);
}

/* Unused: map_screen_update's step 4 drawing alone: sprite 0x65C8, palette 0x6C0 at 0x20,0x10, zoom
   0x100, layer 0, sprite_flags 0x10A; blink_phase cycles 0-2 and timer counts down; sprite_flags gets
   0x40 while flags bit 1 (in timer's low byte) is set, else 0x80 while blink_phase bit 1 is. */
void map_screen_draw_flipping_icon(void)
{
	OBJECT(current_object)->sprite_frame = 0x65C8u;
	OBJECT(current_object)->sprite_palette = 0x6C0u;
	OBJECT(current_object)->screen_x = 0x20;
	OBJECT(current_object)->screen_y = 0x10;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags &= ~0xFFu;
	OBJECT(current_object)->sprite_flags |= 0x10Au;
	OBJECT(current_object)->map_screen.blink_phase++;
	if (OBJECT(current_object)->map_screen.blink_phase >= 3)
		OBJECT(current_object)->map_screen.blink_phase = 0;
	OBJECT(current_object)->map_screen.timer--;
	if ((OBJECT(current_object)->flags & 2u) == 0u)
	{
		if ((OBJECT(current_object)->map_screen.blink_phase & 2u) != 0u)
			OBJECT(current_object)->sprite_flags |= 0x80u;
	}
	else
		OBJECT(current_object)->sprite_flags |= 0x40u;
	queue_sprite_in_layer();
}

/* The map's icon, by animation_frame: spawns map_eggman_update; the markers, the trail to the next
   stage, the stage name blinking; sprite 0x65C8 flipping 0x28 frames; a wipe (map_screen_step + 1); the
   next stage's icon flying to its marker (map_stage_positions); the last wipe, map_screen_step = 5, then
   step 10 for good (11 is never reached). */
void map_screen_update(void)
{
	uint16_t next_x = 0;
	uint32_t edges_done = 0, free_count = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, marker_position = 0, next_y = 0, object = 0, record = 0, stage_number = 0;

	switch (OBJECT(current_object)->animation_frame)
	{
	case 0:
	case 12:
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count == 0u)
			return;
		OBJECT(object)->handler = map_eggman_update_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->map_eggman.sound_step = 0u;
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->map_screen.timer = 1;
		return;
	case 1:
		OBJECT(current_object)->map_screen.timer--;
		if ((uint8_t)OBJECT(current_object)->map_screen.timer == 0u)
		{
			OBJECT(current_object)->map_screen.timer = 0x64;
			OBJECT(current_object)->animation_frame++;
		}
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		object = draw_map_stage_markers(0u);
		start_map_trail();
		draw_map_trail_dot(object);
		return;
	case 2:
		object = draw_map_stage_markers(0u);
		start_map_trail();
		draw_map_trail_dot(object);
		OBJECT(object_cursor)->map_screen.timer--;
		if (OBJECT(object_cursor)->map_screen.timer <= 0)
		{
			OBJECT(object_cursor)->map_screen.timer = 0x12C;
			OBJECT(object_cursor)->map_screen.blink_frames = 0x154;
			OBJECT(object_cursor)->animation_frame++;
			start_map_trail();
		}
		return;
	case 3:
		object = draw_map_stage_markers(0u);
		draw_map_trail_dot(object);
		if ((uint16_t)OBJECT(current_object)->map_screen.blink_frames != 0u && OBJECT(current_object)->map_screen.trail_done < 0)
		{
			blink_current_map_marker();
			OBJECT(current_object)->map_screen.blink_frames--;
			if (OBJECT(current_object)->map_screen.blink_frames <= 0)
			{
				OBJECT(current_object)->map_screen.timer = 0x28;
				OBJECT(current_object)->animation_frame++;
			}
		}
		return;
	case 4:
		OBJECT(current_object)->map_screen.timer--;
		if ((uint16_t)OBJECT(current_object)->map_screen.timer == 0u)
		{
			OBJECT(current_object)->map_screen.timer = 0x12C;
			OBJECT(current_object)->map_screen.blink_frames = 0x154;
			OBJECT(current_object)->animation_frame++;
			start_map_trail();
		}
		OBJECT(current_object)->sprite_frame = 0x65C8u;
		OBJECT(current_object)->sprite_palette = 0x6C0u;
		OBJECT(current_object)->screen_x = 0x20;
		OBJECT(current_object)->screen_y = 0x10;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0xFFu;
		OBJECT(current_object)->sprite_flags |= 0x10Au;
		OBJECT(current_object)->map_screen.blink_phase++;
		if (OBJECT(current_object)->map_screen.blink_phase >= 3)
			OBJECT(current_object)->map_screen.blink_phase = 0;
		if ((OBJECT(current_object)->flags & 2u) == 0u)
		{
			if ((OBJECT(current_object)->map_screen.blink_phase & 2u) != 0u)
				OBJECT(current_object)->sprite_flags |= 0x80u;
		}
		else
			OBJECT(current_object)->sprite_flags |= 0x40u;
		queue_sprite_in_layer();
		return;
	case 5:
		stage_number = stage;
		map_stage_icon = next_stage_map_icons[stage_number & 0x7FFFu];
		window_wipe_closing = 0u;
		record = map_wipe_out_windows_ADDRESS + stage * 6u;
		start_window_wipe(record);
		OBJECT(current_object)->map_screen.base_x = 0x20;
		OBJECT(current_object)->map_screen.base_y = 0x10;
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x1130u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		OBJECT(current_object)->animation_frame++;
		OBJECT(current_object)->map_screen.timer = 0x3C;
		return;
	case 6:
		if ((uint16_t)OBJECT(current_object)->map_screen.timer == 0u)
		{
			update_screen_wipe(&ignored_2, &ignored_3, &ignored_4, &edges_done);
			if ((edges_done & 0xFu) != 0xFu)
			{
				OBJECT(current_object)->sprite_frame = map_stage_icon;
				OBJECT(current_object)->map_screen.blink_phase++;
				OBJECT(current_object)->map_screen.blink_phase &= 1u;
				if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
					OBJECT(current_object)->sprite_frame = 0x6538u;
				OBJECT(current_object)->sprite_palette = 0x6B0u;
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_layer = 1u;
				OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
				OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
				OBJECT(current_object)->sprite_flags = 0x10Au;
				queue_sprite_in_layer();
				return;
			}
			OBJECT(current_object)->animation_frame++;
			map_screen_step++;
		}
		OBJECT(current_object)->map_screen.timer--;
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		return;
	case 7:
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.blink_phase == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.base_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.base_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		marker_position = ((uint32_t)stage_map_markers[stage] << 2) + map_stage_positions_ADDRESS;
		next_x = MAP_ICON_POSITION(marker_position)->x;
		next_y = MAP_ICON_POSITION(marker_position)->y;
		OBJECT(current_object)->map_screen.target_x = next_x;
		OBJECT(current_object)->map_screen.target_y = next_y;
		next_y = (int32_t)((next_y - (uint16_t)OBJECT(current_object)->map_screen.base_y) << 16) / 0x3C;
		OBJECT(current_object)->map_screen.step_x = (int32_t)(((uint32_t)next_x - (uint16_t)OBJECT(current_object)->map_screen.base_x) << 16) / 0x3C;
		OBJECT(current_object)->map_screen.step_y = next_y;
		OBJECT(current_object)->map_screen.timer = 0x3C;
		OBJECT(current_object)->animation_frame++;
		return;
	case 8:
		move_map_stage_icon();
		return;
	case 9:
		window_wipe_closing = 1u;
		record = map_wipe_next_windows_ADDRESS + stage * 6u;
		start_window_wipe(record);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0x21u);
		OBJECT(current_object)->sprite_frame = map_stage_icon;
		OBJECT(current_object)->map_screen.blink_phase++;
		OBJECT(current_object)->map_screen.blink_phase &= 1u;
		if ((uint8_t)OBJECT(current_object)->map_screen.marker_blink_count == 0u)
			OBJECT(current_object)->sprite_frame = 0x6538u;
		OBJECT(current_object)->sprite_palette = 0x6B0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = OBJECT(current_object)->map_screen.target_x;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->map_screen.target_y;
		OBJECT(current_object)->sprite_flags = 0x10Au;
		queue_sprite_in_layer();
		OBJECT(current_object)->animation_frame++;
		map_screen_step = 5u;
		return;
	case 10:
		return;
	case 11:
		update_screen_wipe(&ignored_2, &ignored_3, &ignored_4, &edges_done);
		if ((edges_done & 0xFu) == 0xFu)
			delete_current_object();
		return;
	default:
		invalid_jump(0x95DBEu + OBJECT(current_object)->animation_frame * 3u);
	}
}

/* Eggman on the map (from behind): stage_map_sounds_1; after 0x3C frames his hands
   (map_eggman_hands_update) and 0x186 frames of map_eggman_gesture_frames (sound 0x21 for gesture 7); a
   pose held 0x50 frames (stage_map_sounds_2), then a loop (stage_map_sounds_3); gone at map_screen_step
   4. Its stage 9 count for sound 0x95 only reaches 1. */
void map_eggman_update(void)
{
	uint8_t sound_id = 0;
	uint16_t gesture = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, object = 0;
	uint16_t timer = 0;

	switch (OBJECT(current_object)->animation_frame)
	{
	case 0:
	case 4:
		OBJECT(current_object)->sprite_frame = 0x5760u;
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		OBJECT(current_object)->map_eggman.timer = 0x3Cu;
		OBJECT(current_object)->animation_frame++;
		map_stage_09_sound_count = 0u;
		sound_id = stage_map_sounds_1[stage];
		queue_sound(sound_id, &ignored, &ignored_2);
		return;
	case 1:
		timer = OBJECT(current_object)->map_eggman.timer;
		OBJECT(current_object)->map_eggman.timer = timer - 1u;
		if (timer == 1u)
		{
			allocate_object_outside_loop(&free_count, &object);
			if ((uint8_t)free_count != 0u)
			{
				OBJECT(object)->handler = map_eggman_hands_update_ADDRESS;
				OBJECT(object)->map_eggman_hands.timer = 0xF0u;
				OBJECT(object)->map_eggman_hands.frame_step = 0u;
				OBJECT(object)->map_eggman_hands.frame_step_pad = 0u;
				OBJECT(current_object)->animation_frame++;
				mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xEu);
				OBJECT(current_object)->map_eggman.timer = 0x186u;
				OBJECT(current_object)->kind = 0u;
				OBJECT(current_object)->map_eggman.gesture_list = map_eggman_gesture_frames_ADDRESS;
				OBJECT(current_object)->map_eggman.unused_step = 0u;
			}
		}
		OBJECT(current_object)->sprite_frame = 0x5760u;
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		return;
	case 2:
		OBJECT(current_object)->map_eggman.timer--;
		if (OBJECT(current_object)->map_eggman.timer == 0u)
		{
			OBJECT(current_object)->animation_frame++;
			OBJECT(current_object)->kind = 0u;
			OBJECT(current_object)->map_eggman.timer = 3u;
			OBJECT(current_object)->map_eggman.hold_frames = 0x50u;
		}
		for (;;)
		{
			if ((uint8_t)MAP_EGGMAN_GESTURES(OBJECT(current_object)->map_eggman.gesture_list)[OBJECT(current_object)->kind] == 7u)
			{
				sound_id = 0x21u;
				queue_sound(sound_id, &ignored, &ignored_2);
			}
			gesture = (uint16_t)MAP_EGGMAN_GESTURES(OBJECT(current_object)->map_eggman.gesture_list)[OBJECT(current_object)->kind];
			if ((int16_t)gesture >= 0)
				break;
			OBJECT(current_object)->kind--;
		}
		OBJECT(current_object)->sprite_frame = ((gesture - 1u) << 3) + 0x5760u;
		OBJECT(current_object)->sprite_palette = 0x680u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0xDF;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0x10Fu;
		OBJECT(current_object)->sprite_flags |= 0x107u;
		queue_sprite_in_layer();
		if ((OBJECT(current_object)->map_eggman.timer & 3u) == 3u)
		{
			OBJECT(current_object)->kind++;
			OBJECT(current_object)->map_eggman.unused_step++;
			if (OBJECT(current_object)->map_eggman.unused_step == 7u)
				OBJECT(current_object)->map_eggman.unused_step = 0u;
		}
		return;
	case 3:
		if (map_screen_step == 4u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->map_eggman.timer--;
		OBJECT(current_object)->sprite_frame = 0x57C8u;
		OBJECT(current_object)->sprite_frame += (uint32_t)OBJECT(current_object)->kind << 3;
		if (OBJECT(current_object)->map_eggman.timer == 0u)
		{
			OBJECT(current_object)->kind++;
			OBJECT(current_object)->map_eggman.timer = 3u;
			if ((int8_t)OBJECT(current_object)->kind < 4)
			{
				draw_map_eggman();
				return;
			}
		}
		if (OBJECT(current_object)->map_eggman.hold_frames != 0u)
		{
			if (OBJECT(current_object)->map_eggman.sound_step == 0u)
			{
				OBJECT(current_object)->map_eggman.sound_step = 1u;
				sound_id = stage_map_sounds_2[stage];
				queue_sound(sound_id, &ignored, &ignored_2);
			}
			OBJECT(current_object)->kind = 3u;
			OBJECT(current_object)->map_eggman.hold_frames--;
			draw_map_eggman();
			return;
		}
		if (OBJECT(current_object)->map_eggman.sound_step == 1u)
		{
			OBJECT(current_object)->map_eggman.sound_step = 2u;
			sound_id = stage_map_sounds_3[stage];
			queue_sound(sound_id, &ignored, &ignored_2);
			if (stage == 9u)
			{
				map_stage_09_sound_count++;
				if ((int8_t)map_stage_09_sound_count >= 0x5A)
				{
					map_stage_09_sound_count = 0x5Au;
					sound_id = 0x95u;
					queue_sound(sound_id, &ignored, &ignored_2);
				}
			}
		}
		if ((int16_t)(OBJECT(current_object)->kind | ((uint32_t)(OBJECT(current_object)->map_eggman.hold_frames & 0xFFu) << 8u)) <= 5)
		{
			draw_map_eggman();
			return;
		}
		OBJECT(current_object)->kind = 4u;
		OBJECT(current_object)->map_eggman.timer = 5u;
		draw_map_eggman();
		return;
	default:
		invalid_jump(0x96188u + OBJECT(current_object)->animation_frame * 3u);
	}
}

/* Eggman's hands on the map (map_eggman_update spawns them): for timer (0xF0) frames, sprite 0x5798 + 8
   * frame_step (0-5, a step every 4 frames), palette 0x690, layer 0, at 0xB7,0x9B; then the object stays
   without drawing. */
void map_eggman_hands_update(void)
{
	if (OBJECT(current_object)->map_eggman_hands.timer != 0u)
	{
		OBJECT(current_object)->map_eggman_hands.timer--;
		OBJECT(current_object)->sprite_frame = ((uint32_t)OBJECT(current_object)->map_eggman_hands.frame_step << 3) + 0x5798u;
		OBJECT(current_object)->sprite_palette = 0x690u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->screen_x = 0xB7;
		OBJECT(current_object)->screen_y = 0x9B;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 7u;
		queue_sprite_in_layer();
		if ((OBJECT(current_object)->map_eggman_hands.timer & 3u) == 3u)
		{
			OBJECT(current_object)->kind++;
			OBJECT(current_object)->map_eggman_hands.frame_step++;
			if (OBJECT(current_object)->map_eggman_hands.frame_step == 6u)
			{
				OBJECT(current_object)->map_eggman_hands.frame_step = 0u;
				return;
			}
		}
	}
}

/* Unused: sprite 0x5760 (the pose before the gestures), then draw_map_eggman. */
void map_eggman_draw_plain(void)
{
	OBJECT(current_object)->sprite_frame = 0x5760u;
	draw_map_eggman();
}

/* Unused: the same code again: sprite 0x5760, then draw_map_eggman. */
void map_eggman_draw_plain_2(void)
{
	OBJECT(current_object)->sprite_frame = 0x5760u;
	draw_map_eggman();
}

/* Queues the map's Eggman with the sprite_frame it was given: palette 0x680, zoom 0x100, layer 1, at
   0xB7,0xDF, sprite_flags 0x107 (queue_sprite_in_layer). */
void draw_map_eggman(void)
{
	OBJECT(current_object)->sprite_palette = 0x680u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->screen_x = 0xB7;
	OBJECT(current_object)->screen_y = 0xDF;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags &= ~0x10Fu;
	OBJECT(current_object)->sprite_flags |= 0x107u;
	queue_sprite_in_layer();
}

/* Starts the trail of stage_map_trails[stage] on the current object: its first word is the frames per
   segment (segment_frames, segment_timer), then next_map_trail_segment from its first point. A stage
   without a trail (0xFFFFFFFF) sets trail_done, but that address is read all the same. */
void start_map_trail(void)
{
	uint32_t trail = 0;

	OBJECT(current_object)->start_map_trail.trail_done = 0;
	trail = stage_map_trails[stage];
	if ((int32_t)trail <= 0)
		OBJECT(current_object)->start_map_trail.trail_done = -1;
	OBJECT(current_object)->start_map_trail.segment_frames = mem_read16(trail);
	OBJECT(current_object)->start_map_trail.segment_timer = mem_read16(trail);
	trail += 2u;
	next_map_trail_segment(trail);
}

/* Starts the trail's next segment, from the point at trail_point to the one after it (next_trail_point):
   base = start, target = end, steps = (base - target) / segment_frames in 16.16, offsets 0 (offset_y
   through z_int and the animation bytes that share it). A negative end x is the trail's end: trail_done
   = -1. */
void next_map_trail_segment(uint32_t trail_point)
{
	uint16_t end_x = 0, end_y = 0, start_x = 0, start_y = 0;
	uint32_t segment_frames = 0, step_y = 0;

	start_x = (uint16_t)SCREEN_POINT(trail_point)->x;
	trail_point += 2u;
	start_y = (uint16_t)SCREEN_POINT(trail_point - 0x2u)->y;
	trail_point += 2u;
	end_x = (uint16_t)SCREEN_POINT(trail_point)->x;
	end_y = (uint16_t)SCREEN_POINT(trail_point)->y;
	OBJECT(current_object)->next_map_trail_segment.next_trail_point = trail_point;
	if ((int16_t)end_x >= 0)
	{
		OBJECT(current_object)->next_map_trail_segment.base_x = start_x;
		OBJECT(current_object)->next_map_trail_segment.base_y = start_y;
		OBJECT(current_object)->next_map_trail_segment.target_x = end_x;
		OBJECT(current_object)->next_map_trail_segment.target_y = end_y;
		segment_frames = OBJECT(current_object)->next_map_trail_segment.segment_frames;
		step_y = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->next_map_trail_segment.base_y - (uint16_t)OBJECT(current_object)->next_map_trail_segment.target_y) << 16, segment_frames);
		OBJECT(current_object)->next_map_trail_segment.step_x = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->next_map_trail_segment.base_x - (uint16_t)OBJECT(current_object)->next_map_trail_segment.target_x) << 16, segment_frames);
		OBJECT(current_object)->next_map_trail_segment.step_y = step_y;
		OBJECT(current_object)->next_map_trail_segment.offset_x = 0;
		OBJECT(current_object)->z_int = 0;
		OBJECT(current_object)->animation_speed = 0u;
		OBJECT(current_object)->animation_timer = 0;
	}
	else
		OBJECT(current_object)->next_map_trail_segment.trail_done = -1;
}

/* Draws the trail's dot for object_cursor at base + offset, moving it one step along the segment while
   trail_done is clear (sprite 0x64E0, palette 0x660, layer 1, through a pool 2 block taken and freed
   again); when segment_timer runs out, the next segment (next_map_trail_segment). */
void draw_map_trail_dot(uint32_t object)
{
	uint16_t dot_y = 0, object_2 = 0;
	uint32_t ignored = 0, ignored_2 = 0, next_trail_point = 0;

	allocate_object_2(&ignored, &ignored_2, &object);
	current_object = object;
	screen_sprite_block_address = object;
	if (OBJECT(object_cursor)->draw_map_trail_dot.trail_done >= 0)
	{
		OBJECT(object_cursor)->draw_map_trail_dot.offset_x -= OBJECT(object_cursor)->draw_map_trail_dot.step_x;
		OBJECT(object_cursor)->draw_map_trail_dot.offset_y -= OBJECT(object_cursor)->draw_map_trail_dot.step_y;
	}
	dot_y = (uint16_t)(((int32_t)OBJECT(object_cursor)->draw_map_trail_dot.offset_y >> 16) + OBJECT(object_cursor)->draw_map_trail_dot.base_y);
	OBJECT(current_object)->screen_x = ((int32_t)OBJECT(object_cursor)->draw_map_trail_dot.offset_x >> 16) + OBJECT(object_cursor)->draw_map_trail_dot.base_x;
	OBJECT(current_object)->screen_y = dot_y;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x64E0u;
	OBJECT(current_object)->sprite_palette = 0x660u;
	queue_sprite_in_layer();
	current_object = object_cursor;
	object_2 = (uint16_t)screen_sprite_block_address;
	free_object_2(object_2);
	if (OBJECT(object_cursor)->draw_map_trail_dot.trail_done >= 0)
	{
		OBJECT(object_cursor)->draw_map_trail_dot.segment_timer--;
		if ((uint8_t)OBJECT(object_cursor)->draw_map_trail_dot.segment_timer == 0u)
		{
			OBJECT(object_cursor)->draw_map_trail_dot.segment_timer = OBJECT(object_cursor)->draw_map_trail_dot.segment_frames;
			next_trail_point = OBJECT(object_cursor)->draw_map_trail_dot.next_trail_point;
			next_map_trail_segment(next_trail_point);
		}
	}
}

/* Draws the map's 7 markers and their number badges (map_marker_layouts) from the sprite blocks at
   0x209102: those before stage_map_markers[stage] - 1 in cleared_palette, that one too once
   object_cursor's trail is done, when the stage's badge (map_number_badge_sprites) blinks every 8
   frames. Nothing for a stage with no marker. */
uint32_t draw_map_stage_markers(uint32_t badge_offset)
{
	uint8_t current_from_end = 0, current_marker = 0;
	uint32_t groups_left = 0, layout = 0, markers_left = 0;

	current_object = 0x209102u;
	current_marker = stage_map_markers[stage];
	if ((int8_t)current_marker < 0)
		return badge_offset;
	layout = map_marker_layouts_ADDRESS;
	groups_left = 2u;
	do
	{
		markers_left = 7u;
		current_from_end = (uint8_t)(uint16_t)(7u - current_marker);
		do
		{
			OBJECT(current_object)->sprite_flags = 0xAu;
			OBJECT(current_object)->sprite_scale = 0x100u;
			OBJECT(current_object)->sprite_layer = MAP_MARKER_LAYOUT(layout)->sprite_layer;
			layout += 2u;
			OBJECT(current_object)->screen_x = MAP_MARKER_LAYOUT(layout - 0x2u)->screen_x;
			layout += 2u;
			OBJECT(current_object)->screen_y = MAP_MARKER_LAYOUT(layout - 0x4u)->screen_y;
			layout += 2u;
			OBJECT(current_object)->sprite_frame = MAP_MARKER_LAYOUT(layout - 0x6u)->sprite_frame;
			layout += 2u;
			OBJECT(current_object)->sprite_palette = MAP_MARKER_LAYOUT(layout - 0x8u)->sprite_palette;
			layout += 2u;
			if ((int8_t)current_from_end < (int8_t)markers_left && ((uint8_t)(current_from_end + 1u) != (uint8_t)markers_left || OBJECT(object_cursor)->draw_map_stage_markers.trail_done < 0))
				OBJECT(current_object)->sprite_palette = MAP_MARKER_LAYOUT(layout - 0xAu)->cleared_palette;
			layout = (layout & ~0xFFFFu) | (uint16_t)(layout + 2u);
			if (OBJECT(object_cursor)->draw_map_stage_markers.trail_done >= 0)
			{
				queue_sprite_in_layer();
				current_object = current_object + 0x30u;
			}
			else
			{
				badge_offset = stage_map_markers[stage];
				if ((int8_t)badge_offset < 0)
				{
					queue_sprite_in_layer();
					current_object = current_object + 0x30u;
				}
				else
				{
					badge_offset = badge_offset * 0xCu;
					if (OBJECT(current_object)->sprite_frame != map_number_badge_sprites[badge_offset / 2u])
					{
						queue_sprite_in_layer();
						current_object = current_object + 0x30u;
					}
					else
					{
						OBJECT(object_cursor)->draw_map_stage_markers.marker_blink_count++;
						if ((OBJECT(object_cursor)->draw_map_stage_markers.marker_blink_count & 8u) != 0u)
						{
							queue_sprite_in_layer();
							current_object = current_object + 0x30u;
						}
					}
				}
			}
			markers_left--;
		} while (markers_left != 0u);
		groups_left--;
	} while (groups_left != 0u);
	current_object = object_cursor;
	return badge_offset;
}

/* While step_timer (the map screen's timer) counts down, draws the stage's name over its marker
   (map_marker_blink_sprites for stage_map_markers[stage]: layer, x, y, sprite, palette) on the frames
   its bit 3 is set, so it blinks every 8 frames. */
void blink_current_map_marker(void)
{
	uint16_t current_marker = 0;
	uint32_t record = 0;

	if (OBJECT(current_object)->blink_current_map_marker.step_timer != 0u)
	{
		OBJECT(current_object)->blink_current_map_marker.step_timer--;
		if ((OBJECT(current_object)->blink_current_map_marker.step_timer & 8u) != 0u)
		{
			current_marker = stage_map_markers[stage];
			if ((int8_t)current_marker >= 0)
			{
				record = 0x90000u | (uint16_t)(0x6C6Fu + ((uint32_t)current_marker << 3));
				record = (record & ~0xFFFFu) | (uint16_t)(record + current_marker * 2u);
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_flags = 0xAu;
				OBJECT(current_object)->sprite_layer = MAP_MARKER_BLINK(record)->sprite_layer;
				record += 2u;
				OBJECT(current_object)->screen_x = MAP_MARKER_BLINK(record - 0x2u)->screen_x;
				record += 2u;
				OBJECT(current_object)->screen_y = MAP_MARKER_BLINK(record - 0x4u)->screen_y;
				record += 2u;
				OBJECT(current_object)->sprite_frame = MAP_MARKER_BLINK(record - 0x6u)->sprite_frame;
				OBJECT(current_object)->sprite_palette = MAP_MARKER_BLINK(record - 0x6u)->sprite_palette;
				queue_sprite_in_layer();
			}
		}
	}
}

/* Starts a window wipe: the bitmap layer as a black frame (video RAM from 0xE000 filled with 0x1111)
   whose window moves from the screen's edges to the record's (left, top, right = left + 0x100, bottom =
   top + 0x80, kept on the screen) over its frames, or out from it with window_wipe_closing
   (update_screen_wipe). */
uint32_t start_window_wipe(uint32_t record)
{
	uint16_t left = 0, right = 0, top = 0;
	uint32_t bottom = 0, frames = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	fill_value = (fill_value & ~0xFFFFu) | 0x1111u;
	set_halfwords(VIDEO_RAM + 0xE000u, 0x7000u, fill_value);
	mem_write32(video_control, mem_read32(video_control) & ~0x800u);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 0x20u);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 0x8000u);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 0x400u);
	mem_write16(video_bitmap_scroll_x, 0u);
	mem_write16(video_bitmap_scroll_y, 0xE0u);
	mem_write16(mixer_bitmap, mem_read16(mixer_bitmap) & ~0xFu);
	mem_write16(mixer_bitmap, 0x2Fu);
	left = (uint16_t)WIPE_WINDOW(record)->left;
	record += 2u;
	top = (uint16_t)WIPE_WINDOW(record - 0x2u)->top;
	frames = WIPE_WINDOW(record - 0x2u)->frames;
	bottom = top + 0x80u;
	right = (uint16_t)(left + 0x100u);
	if ((int16_t)right >= 0x140)
		right = 0x140u;
	if ((int16_t)bottom >= 0xE0)
		bottom = 0xE0u;
	if ((int16_t)left <= 0)
		left = window_wipe_default_left;
	if ((int16_t)top <= 0)
		top = window_wipe_default_top;
	window_wipe_left = left;
	window_wipe_top = top;
	window_wipe_right = right;
	window_wipe_bottom = bottom;
	window_wipe_frames = frames;
	bottom = divide_s32((0u - bottom + 0xE0u) << 16, frames);
	window_wipe_left_speed = divide_s32((uint32_t)left << 16, frames);
	window_wipe_right_speed = divide_s32((0u - right + 0x140u) << 16, frames);
	window_wipe_top_speed = divide_s32((uint32_t)top << 16, frames);
	window_wipe_bottom_speed = bottom;
	window_wipe_left_edge = 0;
	window_wipe_right_edge = 0x1400000;
	window_wipe_top_edge = 0;
	window_wipe_bottom_edge = 0xE00000;
	if (window_wipe_closing != 0u)
	{
		window_wipe_left_edge_fraction = window_wipe_left;
		window_wipe_right_edge_fraction = window_wipe_right;
		window_wipe_top_edge_fraction = window_wipe_top;
		window_wipe_bottom_edge_fraction = window_wipe_bottom;
		window_wipe_left_edge = (uint32_t)window_wipe_left_edge << 16;
		window_wipe_right_edge = (uint32_t)window_wipe_right_edge << 16;
		window_wipe_top_edge = (uint32_t)window_wipe_top_edge << 16;
		window_wipe_bottom_edge = (uint32_t)window_wipe_bottom_edge << 16;
		window_wipe_left_speed = 0u - window_wipe_left_speed;
		window_wipe_right_speed = 0u - window_wipe_right_speed;
		window_wipe_top_speed = 0u - window_wipe_top_speed;
		window_wipe_bottom_speed = 0u - window_wipe_bottom_speed;
	}
	update_screen_wipe(&ignored_3, &ignored_4, &ignored_5, &frames);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~0x20u);
	return bottom;
}

/* Moves the wipe's four window edges by their speeds and stops each at its end (the record's window, or
   the screen's edges with window_wipe_closing), writing them to the bitmap layer's clip window;
   edges_done gets a bit per edge that has arrived (0xF for all four). Its test for 0xFFFF, which would
   turn the layer off, never passes. */
void update_screen_wipe(uint32_t *p_clip_right, uint32_t *p_clip_top, uint32_t *p_clip_bottom, uint32_t *p_edges_done)
{
	uint32_t edges_done = *p_edges_done;
	uint16_t clip_left = 0;
	uint32_t clip_bottom = 0, clip_right = 0, clip_top = 0;

	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0xF0u);
	window_wipe_left_edge += window_wipe_left_speed;
	window_wipe_right_edge -= window_wipe_right_speed;
	window_wipe_top_edge += window_wipe_top_speed;
	window_wipe_bottom_edge -= window_wipe_bottom_speed;
	clip_left = (uint16_t)((int32_t)window_wipe_left_edge >> 16);
	clip_right = (int32_t)window_wipe_right_edge >> 16;
	clip_top = (int32_t)window_wipe_top_edge >> 16;
	clip_bottom = (int32_t)window_wipe_bottom_edge >> 16;
	edges_done &= ~0xFFu;
	if ((window_wipe_closing & 1u) != 0u)
	{
		if ((int16_t)clip_left <= 0)
		{
			edges_done |= 1u;
			clip_left = 0u;
			window_wipe_left_edge = 0;
		}
		if ((int16_t)clip_right >= 0x13F)
		{
			edges_done |= 2u;
			clip_right = (clip_right & ~0xFFFFu) | 0x13Fu;
			window_wipe_right_edge = 0x13F0000;
		}
		if ((int16_t)clip_top <= 0)
		{
			edges_done |= 8u;
			clip_top = 0u;
			window_wipe_top_edge = 0;
		}
		if ((int16_t)clip_bottom >= 0xE0)
		{
			edges_done |= 4u;
			clip_bottom = (clip_bottom & ~0xFFFFu) | 0xDFu;
			window_wipe_bottom_edge = 0xDF0000;
		}
	}
	else
	{
		if ((int16_t)window_wipe_left <= (int16_t)clip_left)
		{
			edges_done |= 1u;
			window_wipe_left_edge = (uint32_t)window_wipe_left << 16;
			clip_left = window_wipe_left;
		}
		if ((int16_t)clip_right <= (int16_t)window_wipe_right)
		{
			edges_done |= 2u;
			clip_right = (uint32_t)window_wipe_right << 16;
			window_wipe_right_edge = clip_right;
			clip_right |= window_wipe_right;
		}
		if ((int16_t)window_wipe_top <= (int16_t)clip_top)
		{
			edges_done |= 4u;
			clip_top = (uint32_t)window_wipe_top << 16;
			window_wipe_top_edge = clip_top;
			clip_top |= window_wipe_top;
		}
		if ((int16_t)clip_bottom <= (int16_t)window_wipe_bottom)
		{
			edges_done |= 8u;
			clip_bottom = (uint32_t)window_wipe_bottom << 16;
			window_wipe_bottom_edge = clip_bottom;
			clip_bottom |= window_wipe_bottom;
		}
	}
	mem_write16(video_bitmap_clip_left, clip_left);
	mem_write16(video_bitmap_clip_top, clip_top);
	mem_write16(video_bitmap_clip_right, clip_right);
	mem_write16(video_bitmap_clip_bottom, clip_bottom);
	if ((uint16_t)edges_done == 0xFFFFu)
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x20u);
	*p_clip_right = clip_right;
	*p_clip_top = clip_top;
	*p_clip_bottom = clip_bottom;
	*p_edges_done = edges_done;
}
