/**
 *    Name:        scenes/ending.c
 *    Description: stage 0x11, the ending: the runners, the tower and the explosions, the flash to white, the credits (portraits, name plates, scrolling blocks), Eggman flying away and the three running across
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Builds the ending by ending_scene_step: 0 sets up the screen (display off for 0x10 frames, layers and
   palettes, sound 0x94, colour offsets 0x20, pages and scrolls, clip rectangle 1 cleared, six vram
   scripts) and goes to 1; 1 spawns the tower, the horizon, the three runners and the road (then 2); 2
   and 3 do nothing. */
void build_ending_scene(uint32_t *p_write_index_address, uint32_t *p_tile_bank_bits, uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_scene_actor)
{
	uint32_t scene_actor = *p_scene_actor;
	uint8_t is_8bpp = 0, sound_id = 0;
	uint32_t ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, script = 0;
	uint8_t io_cnt_bits = 0;

	is_8bpp = (uint8_t)(ending_scene_step * 3u);
	switch (ending_scene_step)
	{
	case 0:
		io_cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, io_cnt_bits & ~2u);
		display_off_frames = 0x10u;
		*p_color = set_stage_layer_mixing();
		clear_sprite_buckets();
		clear_bitmap_layer(is_8bpp, &ignored_2, p_color, &ignored_3, p_row_pairs_left, p_blocks_per_row, &ignored_4);
		init_video_registers();
		load_stage_palette_set(p_tile_bank_bits, &scene_actor);
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored_2, p_write_index_address);
		color_offset_red = 0x20u;
		color_offset_blue = 0x20u;
		color_offset_green = 0x20u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x2Fu);
		nbg0_pages = 0x3020100u;
		nbg1_pages = 0x1211100Fu;
		nbg2_pages = 0xD0C0B0Au;
		nbg3_pages = 0x8070605u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		nbg1_scroll_x_int = 0u;
		nbg1_scroll_y_int = 0u;
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		ending_scene_step = 1u;
		ending_first_scene_done = 0u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		mem_write32(video_control, mem_read32(video_control) & ~0x1000u);
		mem_write32(video_control, mem_read32(video_control) & ~0x2000u);
		mem_write32(video_control, mem_read32(video_control) & ~0x4000u);
		mem_write16(nbg0_zoom_x, 0x200u);
		mem_write16(nbg0_zoom_y, 0x200u);
		mem_write16(nbg1_zoom_x, 0x200u);
		mem_write16(nbg1_zoom_y, 0x200u);
		nbg0_center_x = 0u;
		nbg0_center_y = 0u;
		nbg1_center_x = 0u;
		nbg1_center_y = 0u;
		mem_write16(video_clip_1_left, 0u);
		mem_write16(video_clip_1_right, 0u);
		mem_write16(video_clip_1_top, 0u);
		mem_write16(video_clip_1_bottom, 0u);
		script = blue_sky_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &scene_actor);
		script = aerial_city_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &scene_actor);
		script = sonic_head_pattern_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &scene_actor);
		script = ending_eggman_machine_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &scene_actor);
		script = ending_logo_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &scene_actor);
		script = ending_bars_script_ADDRESS;
		run_vram_script(&ignored_2, &script, &scene_actor);
		nbg0_pages = 0x8080707u;
		nbg1_pages = 0x1010101u;
		nbg2_pages = 0x5050505u;
		break;
	case 1:
		allocate_object_outside_loop(&ignored_5, &scene_actor);
		OBJECT(scene_actor)->handler = ending_tower_animate_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored_5, &scene_actor);
		OBJECT(scene_actor)->handler = ending_horizon_show_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored_5, &scene_actor);
		OBJECT(scene_actor)->handler = ending_sonic_run_forward_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored_5, &scene_actor);
		OBJECT(scene_actor)->handler = ending_ray_run_forward_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored_5, &scene_actor);
		OBJECT(scene_actor)->handler = ending_mighty_run_forward_ADDRESS;
		init_ending_scene_object(scene_actor);
		allocate_object_outside_loop(&ignored_5, &scene_actor);
		OBJECT(scene_actor)->handler = ending_road_scroll_ADDRESS;
		init_ending_scene_object(scene_actor);
		break;
	case 2:
	case 3:
		break;
	default:
		invalid_jump(0x1BCD3u + ending_scene_step * 3u);
	}
	*p_scene_actor = scene_actor;
}

/* Clears the flags_low_byte (started), step and timer of object, a new scene object, and sets
   ending_scene_step to 2. */
void init_ending_scene_object(uint32_t object)
{
	OBJECT(object)->flags_low_byte = 0u;
	OBJECT(object)->step = 0u;
	OBJECT(object)->timer = 0;
	ending_scene_step = 2u;
}

/* The tower behind the runners: the first frame runs cloudy_horizon_script and turns NBG3 on; sprite and
   palette from ending_tower_frames by its timer (standing, then collapsing), layer 7, and every frame
   run_timed_spawns_2 starts the debris, blasts, explosion and flash on time. Removed at the end of the
   list. */
void ending_tower_animate(void)
{
	uint32_t frame_record = 0, ignored = 0, script = 0, spawned_object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&ignored, &script, &spawned_object);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~8u);
		nbg3_pages = 0u;
		OBJECT(current_object)->ending.list_index = 0u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.started++;
	}
	OBJECT(current_object)->sprite_layer = 7u;
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->screen_y = 0x6B;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	frame_record = ending_tower_frames_ADDRESS;
	for (;;)
	{
		if ((uint16_t)ENDING_TOWER_FRAME(frame_record)->end_time == 0xFFFFu)
		{
			if (ENDING_TOWER_FRAME(frame_record)->sprite_palette != 0xFFFFu)
			{
				OBJECT(current_object)->timer++;
				run_timed_spawns_2(spawned_object);
				return;
			}
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->timer < ENDING_TOWER_FRAME(frame_record)->end_time)
			break;
		frame_record += 6u;
	}
	OBJECT(current_object)->sprite_frame = ENDING_TOWER_FRAME(frame_record)->sprite_frame;
	OBJECT(current_object)->sprite_palette = ENDING_TOWER_FRAME(frame_record)->sprite_palette;
	queue_sprite_in_layer();
	if (ENDING_TOWER_FRAME(frame_record)->sprite_palette != 0xFFFFu)
	{
		OBJECT(current_object)->timer++;
		run_timed_spawns_2(spawned_object);
		return;
	}
	delete_current_object();
}

/* A burning chunk flying off the tower top (from the tower's timed list): sound 0xB on its first frame,
   then animate_ending_debris along ending_debris_1_path with sprite flags 9. */
void ending_debris_1_fly(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, path = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xBu;
		queue_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->timer = 0;
	}
	path = ending_debris_1_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 9u;
	animate_ending_debris(path);
}

/* The second chunk: sound 0xB, then animate_ending_debris along ending_debris_2_path with sprite flags
   0xA. */
void ending_debris_2_fly(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, path = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xBu;
		queue_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->timer = 0;
	}
	path = ending_debris_2_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0xAu;
	animate_ending_debris(path);
}

/* The third chunk: sound 0xB, then animate_ending_debris along ending_debris_3_path with sprite flags 9. */
void ending_debris_3_fly(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, path = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xBu;
		queue_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->timer = 0;
	}
	path = ending_debris_3_path_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 9u;
	animate_ending_debris(path);
}

/* A blast at the tower's base (from the tower's timed list): sound 0xA and start_ending_blast on its
   first frame, at (0x90, 0xA0), animate_tower_blast. */
void ending_tower_blast_1(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xAu;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x90;
	OBJECT(current_object)->screen_y = 0xA0;
	animate_tower_blast();
}

/* The second tower blast, at (0xA1, 0x98): as ending_tower_blast_1. */
void ending_tower_blast_2(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xAu;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xA1;
	OBJECT(current_object)->screen_y = 0x98;
	animate_tower_blast();
}

/* The third tower blast, at (0xAE, 0xA0): as ending_tower_blast_1. */
void ending_tower_blast_3(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 0xAu;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xAE;
	OBJECT(current_object)->screen_y = 0xA0;
	animate_tower_blast();
}

/* A blast along the horizon (from the tower's timed list): sound 9 and start_ending_blast on its first
   frame, at (0x35, 0xAA), animate_horizon_blast. */
void ending_horizon_blast_1(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x35;
	OBJECT(current_object)->screen_y = 0xAA;
	animate_horizon_blast();
}

/* The second horizon blast, at (0x52, 0xA8): as ending_horizon_blast_1. */
void ending_horizon_blast_2(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x52;
	OBJECT(current_object)->screen_y = 0xA8;
	animate_horizon_blast();
}

/* The third horizon blast, at (0x73, 0xA5): as ending_horizon_blast_1. */
void ending_horizon_blast_3(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x73;
	OBJECT(current_object)->screen_y = 0xA5;
	animate_horizon_blast();
}

/* The fourth horizon blast, at (0x93, 0xA0): as ending_horizon_blast_1. */
void ending_horizon_blast_4(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0x93;
	OBJECT(current_object)->screen_y = 0xA0;
	animate_horizon_blast();
}

/* The fifth horizon blast, at (0xAD, 0x9E): as ending_horizon_blast_1. */
void ending_horizon_blast_5(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xAD;
	OBJECT(current_object)->screen_y = 0x9E;
	animate_horizon_blast();
}

/* The sixth horizon blast, at (0xC6, 0xA2): as ending_horizon_blast_1. */
void ending_horizon_blast_6(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xC6;
	OBJECT(current_object)->screen_y = 0xA2;
	animate_horizon_blast();
}

/* The seventh horizon blast, at (0xCF, 0xAF): as ending_horizon_blast_1. */
void ending_horizon_blast_7(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xCF;
	OBJECT(current_object)->screen_y = 0xAF;
	animate_horizon_blast();
}

/* The eighth horizon blast, at (0xE4, 0xAC): as ending_horizon_blast_1. */
void ending_horizon_blast_8(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		sound_id = 9u;
		queue_sound(sound_id, &ignored, &ignored_2);
		start_ending_blast();
	}
	OBJECT(current_object)->screen_x = 0xE4;
	OBJECT(current_object)->screen_y = 0xAC;
	animate_horizon_blast();
}

/* The ninth horizon blast: as ending_horizon_blast_1, but only its first frame is at (0xF8, 0xB0); later
   frames are at (0xE4, 0xAC), the eighth blast's spot. */
void ending_horizon_blast_9(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		OBJECT(current_object)->screen_x = 0xE4;
		OBJECT(current_object)->screen_y = 0xAC;
		animate_horizon_blast();
		return;
	}
	sound_id = 9u;
	queue_sound(sound_id, &ignored, &ignored_2);
	start_ending_blast();
	OBJECT(current_object)->screen_x = 0xF8;
	OBJECT(current_object)->screen_y = 0xB0;
	animate_horizon_blast();
}

/* Starts a blast: started 1, frame_index 0, timer 0. */
void start_ending_blast(void)
{
	OBJECT(current_object)->start_ending_blast.started = 1u;
	OBJECT(current_object)->start_ending_blast.frame_index = 0u;
	OBJECT(current_object)->timer = 0;
}

/* The big explosion at the tower (from its timed list): at (0xA1, 0x62) in layer 9 with a second-pool
   part kept in speed_x, sound 7; frames from ending_big_explosion_frames and the part's from
   ending_big_explosion_frames_2, the next every 6 frames; at the 0xFFFF end the part is freed and it is
   removed. */
void ending_big_explosion(void)
{
	uint8_t sound_id = 0;
	uint16_t speed_x = 0;
	uint32_t frame_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, second_frame_record = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
		OBJECT(current_object)->screen_x = 0xA1;
		OBJECT(current_object)->screen_y = 0x62;
		OBJECT(current_object)->sprite_layer = 9u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_palette = 0x1100u;
		allocate_object_2(&ignored, &ignored_2, &second_object);
		OBJECT(current_object)->speed_x = second_object;
		current_object = second_object;
		OBJECT(current_object)->screen_x = 0xA1;
		OBJECT(current_object)->screen_y = 0x62;
		OBJECT(current_object)->sprite_layer = 9u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_palette = 0x1100u;
		sound_id = 7u;
		queue_sound(sound_id, &ignored_3, &ignored_2);
	}
	current_object = object_cursor;
	frame_record = ending_big_explosion_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 4u;
	second_frame_record = ending_big_explosion_frames_2_ADDRESS + OBJECT(current_object)->ending.frame_index * 4u;
	if (ENDING_EXPLOSION_FRAME(frame_record)->sprite_frame == 0xFFFFu)
	{
		speed_x = (uint16_t)OBJECT(object_cursor)->speed_x;
		free_object_2(speed_x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_frame = ENDING_EXPLOSION_FRAME(frame_record)->sprite_frame;
	OBJECT(current_object)->sprite_flags = ENDING_EXPLOSION_FRAME(frame_record)->sprite_flags;
	queue_sprite_in_layer();
	if (ENDING_EXPLOSION_FRAME(second_frame_record)->sprite_frame != 0xFFFFu)
	{
		current_object = OBJECT(current_object)->speed_x;
		OBJECT(current_object)->sprite_frame = ENDING_EXPLOSION_FRAME(second_frame_record)->sprite_frame;
		OBJECT(current_object)->sprite_flags = ENDING_EXPLOSION_FRAME(second_frame_record)->sprite_flags;
		queue_sprite_in_layer();
		current_object = object_cursor;
	}
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.frame_index++;
	}
}

/* The flash to white (from the tower's timed list): mixer_offset_select 0x80FF and the second colour
   offsets up from 0 by 1 every 3 frames; at 0x1F it sets ending_first_scene_done, starts
   ending_credits_fade_in and is removed. */
void ending_flash_to_white(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		mixer_offset_select = 0x80FFu;
		color_offset_2_red = 0u;
		color_offset_2_blue = 0u;
		color_offset_2_green = 0u;
		OBJECT(current_object)->timer = 0;
	}
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 3)
	{
		color_offset_2_red++;
		color_offset_2_blue++;
		color_offset_2_green++;
		OBJECT(current_object)->timer = 0;
	}
	if (color_offset_2_red != 0x1Fu)
		return;
	ending_first_scene_done = 1u;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->handler = ending_credits_fade_in_ADDRESS;
	OBJECT(object)->ending.started = 0u;
	delete_current_object();
}

/* Draws the next ending_horizon_blast_frames frame (draw_ending_blast_frame); removed at the 0xFFFF end. */
void animate_horizon_blast(void)
{
	uint32_t frame_entry = 0;

	frame_entry = ending_horizon_blast_frames_ADDRESS + OBJECT(current_object)->animate_horizon_blast.frame_index * 2u;
	if (ENDING_SPRITE_FRAMES(frame_entry)[0] != 0xFFFFu)
	{
		draw_ending_blast_frame(frame_entry);
		return;
	}
	delete_current_object();
}

/* Unused: the same code as draw_ending_blast_frame. */
void draw_ending_blast_frame_copy(uint32_t frame_entry)
{
	OBJECT(current_object)->sprite_frame = ENDING_SPRITE_FRAMES(frame_entry)[0];
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x10B0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->draw_ending_blast_frame.frame_index++;
		OBJECT(current_object)->timer = 0;
	}
}

/* Draws the next ending_tower_blast_frames frame (draw_ending_blast_frame); removed at the 0xFFFF end. */
void animate_tower_blast(void)
{
	uint32_t frame_entry = 0;

	frame_entry = ending_tower_blast_frames_ADDRESS + OBJECT(current_object)->animate_tower_blast.frame_index * 2u;
	if (ENDING_SPRITE_FRAMES(frame_entry)[0] != 0xFFFFu)
	{
		draw_ending_blast_frame(frame_entry);
		return;
	}
	delete_current_object();
}

/* Draws the blast frame at frame_entry (layer 6, palette 0x10B0, scale 0x100) and steps frame_index
   every 6 frames. */
void draw_ending_blast_frame(uint32_t frame_entry)
{
	OBJECT(current_object)->sprite_frame = ENDING_SPRITE_FRAMES(frame_entry)[0];
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x10B0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer >= 6)
	{
		OBJECT(current_object)->draw_ending_blast_frame.frame_index++;
		OBJECT(current_object)->timer = 0;
	}
}

/* Moves a piece of debris along path: the first record whose end_time is above the object's timer gives
   its screen x, y and sprite (palette 0x10C0, layer 6); removed at the 0xFFFF record. Only timer_low
   (timer's low byte) counts up, so timer wraps at 0x100. */
void animate_ending_debris(uint32_t path)
{
	OBJECT(current_object)->animate_ending_debris.timer_low++;
	for (;;)
	{
		if ((uint16_t)DEBRIS_PATH_STEP(path)->end_time == 0xFFFFu)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->timer < DEBRIS_PATH_STEP(path)->end_time)
			break;
		path += 8u;
	}
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->screen_x = DEBRIS_PATH_STEP(path)->screen_x;
	OBJECT(current_object)->screen_y = DEBRIS_PATH_STEP(path)->screen_y;
	OBJECT(current_object)->sprite_frame = DEBRIS_PATH_STEP(path)->sprite_frame;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
}

/* One step of timed_spawns_2 for the tower: once the next record's time is reached (the object's timer)
   it starts an object whose handler is the record's offset from ending_debris_1_fly, and moves on
   (list_index); the 0xFFFF end does nothing. object is only scratch. */
void run_timed_spawns_2(uint32_t object)
{
	uint32_t ignored = 0, spawn_record = 0;

	spawn_record = timed_spawns_2_ADDRESS + OBJECT(current_object)->run_timed_spawns.list_index * 4u;
	OBJECT(current_object)->z = TIMED_SPAWN(spawn_record)->handler_offset;
	if (OBJECT(current_object)->z_fraction != 0xFFFFu && TIMED_SPAWN(spawn_record)->time <= OBJECT(current_object)->timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += ending_debris_1_fly_ADDRESS;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		OBJECT(object)->flags_low_byte = 0u;
		OBJECT(current_object)->run_timed_spawns.list_index++;
	}
}

/* The dark hills under the tower (sprite 0x9AC0, palette 0x10F0, layer 5) at (0x9E, 0xBE); after 0x258
   frames it starts ending_dust_cloud_rise once; removed once ending_first_scene_done is 1. */
void ending_horizon_show(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->timer = 0;
	}
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_layer = 5u;
	OBJECT(current_object)->screen_x = 0x9E;
	OBJECT(current_object)->screen_y = 0xBE;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x9AC0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x10F0u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if ((int16_t)OBJECT(current_object)->flags_low <= 1 && OBJECT(current_object)->timer >= 0x258)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_dust_cloud_rise_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		OBJECT(current_object)->ending.started = 2u;
	}
}

/* The dust cloud along the horizon after the collapse: at x 0x9E (palette 0x10D0, layer 6), every 7
   frames the next (screen y, sprite) of ending_dust_cloud_frames, whose place it keeps in z; removed
   once ending_first_scene_done is 1. */
void ending_dust_cloud_rise(void)
{
	uint32_t frame_pointer = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->z = ending_dust_cloud_frames_ADDRESS;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
	}
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->timer++;
	OBJECT(current_object)->sprite_layer = 6u;
	OBJECT(current_object)->screen_x = 0x9E;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	frame_pointer = OBJECT(current_object)->z;
	if ((uint16_t)DUST_CLOUD_FRAME(frame_pointer)->screen_y != 0xFFFFu && OBJECT(current_object)->timer >= 7)
	{
		OBJECT(current_object)->screen_y = DUST_CLOUD_FRAME(frame_pointer)->screen_y;
		frame_pointer += 2u;
		OBJECT(current_object)->sprite_frame = DUST_CLOUD_FRAME(frame_pointer - 0x2u)->sprite_frame;
		OBJECT(current_object)->z = frame_pointer + 2u;
		OBJECT(current_object)->timer = 0;
	}
	queue_sprite_in_layer();
}

/* Sonic, the centre runner of the first scene: at x 0xA0 from y 0xC0, scale 0x40, palette 0x130, 1 pixel
   up every 0x20 frames; animate_ending_runner with ending_sonic_run_frames. */
void ending_sonic_run_forward(void)
{
	uint32_t frame_entry = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->screen_y = 0xC0;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	frame_entry = ending_sonic_run_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 2u;
	OBJECT(current_object)->sprite_palette = 0x130u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->ending.cycle_count++;
	if ((uint16_t)OBJECT(current_object)->ending.cycle_count == 0x20u)
	{
		OBJECT(current_object)->screen_y--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	animate_ending_runner(frame_entry);
}

/* Ray, the left runner of the first scene: from (0x90, 0xC0) at scale 0x40, palette 0x140, 2 pixels left
   and 1 up every 0x20 frames; animate_ending_runner with ending_ray_run_frames. */
void ending_ray_run_forward(void)
{
	uint32_t frame_entry = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->screen_y = 0xC0;
		OBJECT(current_object)->screen_x = 0x90;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	frame_entry = ending_ray_run_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 2u;
	OBJECT(current_object)->sprite_palette = 0x140u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->ending.cycle_count++;
	if ((uint16_t)OBJECT(current_object)->ending.cycle_count == 0x20u)
	{
		OBJECT(current_object)->screen_x--;
		OBJECT(current_object)->screen_x--;
		OBJECT(current_object)->screen_y--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	animate_ending_runner(frame_entry);
}

/* Mighty, the right runner of the first scene: from (0xB0, 0xC0) at scale 0x40, palette 0x150, 2 pixels
   right and 1 up every 0x20 frames; animate_ending_runner with ending_mighty_run_frames. */
void ending_mighty_run_forward(void)
{
	uint32_t frame_entry = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->screen_y = 0xC0;
		OBJECT(current_object)->screen_x = 0xB0;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	frame_entry = ending_mighty_run_frames_ADDRESS + OBJECT(current_object)->ending.frame_index * 2u;
	OBJECT(current_object)->sprite_palette = 0x150u;
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->ending.cycle_count++;
	if ((uint16_t)OBJECT(current_object)->ending.cycle_count == 0x20u)
	{
		OBJECT(current_object)->screen_x++;
		OBJECT(current_object)->screen_x++;
		OBJECT(current_object)->screen_y--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	animate_ending_runner(frame_entry);
}

/* The road under the runners (palette 0x12B0, layer 2) at (0x9F, 0xD4): the next of the 5
   ending_road_frames every frame; removed once ending_first_scene_done is 1. */
void ending_road_scroll(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started++;
		OBJECT(current_object)->ending.frame_index = 0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->screen_y = 0xD4;
		OBJECT(current_object)->screen_x = 0x9F;
		OBJECT(current_object)->sprite_layer = 2u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_palette = 0x12B0u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
	OBJECT(current_object)->ending.cycle_count++;
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->ending.cycle_count > 4)
		OBJECT(current_object)->ending.cycle_count = 0;
	OBJECT(current_object)->sprite_frame = ending_road_frames[(uint16_t)OBJECT(current_object)->ending.cycle_count];
	queue_sprite_in_layer();
}

/* Back from white for the credits: the first frame loads the credits palettes, sets the second colour
   offsets to 0x1F, starts ending_credits_spawner_run and sets up the layers, the backdrop colour
   (0x5FD2) and the scrolls; after 0x78 frames it starts ending_music_play once and lowers the offsets by
   1 every 3 frames, removed at 0. */
void ending_credits_fade_in(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		object = load_credits_palettes();
		color_offset_2_red = 0x1Fu;
		color_offset_2_blue = 0x1Fu;
		color_offset_2_green = 0x1Fu;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_credits_spawner_run_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0xFF0u);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x10A8u);
		mem_write16(video_backdrop_control, mem_read16(video_backdrop_control) | 0x200u);
		mem_write16(backdrop_color, 0x5FD2u);
		nbg2_scroll_x = 0u;
		nbg2_scroll_y = 0u;
		nbg1_scroll_x_int = 0u;
		nbg1_scroll_y_int = 0u;
		mixer_blend &= 0x1F00u;
		OBJECT(current_object)->ending.cycle_count = 0;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
	}
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer < 0x78)
		return;
	OBJECT(current_object)->timer = 0x79;
	if (OBJECT(current_object)->step == 0u)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_music_play_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		OBJECT(current_object)->step = 1u;
	}
	if (color_offset_2_red == 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.cycle_count++;
	if (OBJECT(current_object)->ending.cycle_count >= 3)
	{
		color_offset_2_red--;
		color_offset_2_blue--;
		color_offset_2_green--;
		OBJECT(current_object)->ending.cycle_count = 0;
	}
}

/* The credits' timer object: counts its timer and runs run_timed_spawns_3, which starts the portrait,
   the name plate and the seven credit blocks. */
void ending_credits_spawner_run(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	OBJECT(current_object)->timer++;
	run_timed_spawns_3();
}

/* Sets bit 0x400 of video_control (the tile bank bit), drops the pending vblank copies and loads
   tile_bank_1_palette_list. */
uint32_t load_credits_palettes(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;

	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	return dest;
}

/* Queues sounds 0x94 and 0x82 (the ending music) on its first frame, then the ending_music_cues sounds
   at their times; removed at the 0xFFFF end. */
void ending_music_play(void)
{
	uint8_t sound_id = 0;
	uint32_t cue_record = 0, ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored_2);
		sound_id = 0x82u;
		queue_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		OBJECT(current_object)->ending.list_index = 0u;
	}
	cue_record = ending_music_cues_ADDRESS + OBJECT(current_object)->ending.list_index * 4u;
	if ((uint16_t)ENDING_MUSIC_CUE(cue_record)->time == 0xFFFFu)
	{
		delete_current_object();
		return;
	}
	if (ENDING_MUSIC_CUE(cue_record)->time <= OBJECT(current_object)->timer)
	{
		sound_id = (uint8_t)ENDING_MUSIC_CUE(cue_record)->sound_id;
		queue_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->ending.list_index++;
	}
	OBJECT(current_object)->timer++;
}

/* The credits' character portrait at (0xE8, 0x4C): sprite and palette from ending_portrait_frames and
   ending_portrait_palettes by its timer, over a backdrop part (sprite 0x9880, kept in speed_y, palettes
   from ending_portrait_backdrop_palettes); at the end the part is freed and it is removed. */
void ending_portrait_show(void)
{
	uint16_t speed_y = 0;
	uint32_t backdrop_object = 0, ignored = 0, ignored_2 = 0, palette_record = 0, timed_record = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		allocate_object_2(&ignored, &ignored_2, &backdrop_object);
		OBJECT(current_object)->speed_y = backdrop_object;
	}
	OBJECT(current_object)->timer++;
	timed_record = ending_portrait_frames_ADDRESS;
	palette_record = ending_portrait_palettes_ADDRESS;
	while (ENDING_TIMED_FRAME(timed_record)->end_time < OBJECT(current_object)->timer)
	{
		timed_record += 4u;
		palette_record += 4u;
		if ((uint16_t)ENDING_TIMED_FRAME(timed_record)->end_time == 0xFFFFu)
		{
			speed_y = (uint16_t)OBJECT(current_object)->speed_y;
			free_object_2(speed_y);
			delete_current_object();
			return;
		}
	}
	OBJECT(current_object)->sprite_frame = ENDING_TIMED_FRAME(timed_record)->sprite_frame;
	OBJECT(current_object)->sprite_palette = ENDING_TIMED_PALETTE(palette_record)->sprite_palette;
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0x4C;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
	timed_record = ending_portrait_backdrop_palettes_ADDRESS;
	while (ENDING_TIMED_PALETTE(timed_record)->end_time < OBJECT(current_object)->timer)
	{
		timed_record += 4u;
		if ((uint16_t)ENDING_TIMED_PALETTE(timed_record)->end_time == 0xFFFFu)
			return;
	}
	current_object = OBJECT(current_object)->speed_y;
	OBJECT(current_object)->sprite_frame = 0x9880u;
	OBJECT(current_object)->sprite_palette = ENDING_TIMED_PALETTE(timed_record)->sprite_palette;
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0x4C;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
}

/* The name plate under the portrait: the first frame sets clip rectangle 1 to 0xB1,0x21-0x120,0x78 and
   takes a second-pool part (kept in z: sprite 0x9800 above, at 0xE9,0x18); the plate by its timer from
   ending_name_plate_frames at (0xE8, 0xA3), NBG0 scrolling 1 a frame; at the end ending_window_open. */
void ending_name_plate_show(void)
{
	uint32_t frame_record = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		mem_write16(video_clip_1_left, 0xB1u);
		mem_write16(video_clip_1_right, 0x120u);
		mem_write16(video_clip_1_top, 0x21u);
		mem_write16(video_clip_1_bottom, 0x78u);
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->z = object;
	}
	OBJECT(current_object)->timer++;
	frame_record = ending_name_plate_frames_ADDRESS;
	while (ENDING_TIMED_FRAME(frame_record)->end_time < OBJECT(current_object)->timer)
	{
		frame_record += 4u;
		if ((uint16_t)ENDING_TIMED_FRAME(frame_record)->end_time == 0xFFFFu)
		{
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->handler = ending_window_open_ADDRESS;
			OBJECT(object)->ending.started = 0u;
			delete_current_object();
			return;
		}
	}
	nbg0_scroll_x_int++;
	OBJECT(current_object)->sprite_frame = ENDING_TIMED_FRAME(frame_record)->sprite_frame;
	OBJECT(current_object)->screen_x = 0xE8;
	OBJECT(current_object)->screen_y = 0xA3;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	queue_sprite_in_layer();
	current_object = OBJECT(current_object)->z;
	OBJECT(current_object)->sprite_frame = 0x9800u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->screen_x = 0xE9;
	OBJECT(current_object)->screen_y = 0x18;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	queue_sprite_in_layer();
}

/* The first credit block: the team logo (header 0x9850 at screen y 0xF4) and its credits_team_logo_parts
   part, kept in speed_y, scroll up 1 pixel every 2 frames until the part is at screen y -0x14; then the
   part is freed and it is removed. */
void ending_credits_team_logo_scroll(void)
{
	uint16_t speed_y = 0;
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->step = 0u;
		OBJECT(current_object)->screen_y = 0xF4;
		OBJECT(current_object)->sprite_frame = 0x9850u;
		OBJECT(current_object)->sprite_palette = 0x1010u;
		init_credits_sprite();
		OBJECT(current_object)->speed_x = credits_team_logo_parts_ADDRESS;
		for (;;)
		{
			field_cursor = OBJECT(object_cursor)->speed_x;
			if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
				break;
			allocate_object_2(&ignored, &ignored_2, &object);
			OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
			field_cursor += 2u;
			OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
			field_cursor += 2u;
			OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
			field_cursor += 2u;
			current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
			OBJECT(object_cursor)->speed_x = field_cursor + 2u;
			OBJECT(current_object)->credit_part_slot = object;
			current_object = object;
			init_credits_sprite();
		}
	}
	if (OBJECT(current_object)->step != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->speed_y;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->step = 1u;
		return;
	}
}

/* The DIRECTOR. credit block: header 0x97C8 and its credits_director_parts (kept in speed_y and z);
   scroll_credits_block does the rest. */
void ending_credits_director_scroll(void)
{
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		scroll_credits_block();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = credits_director_parts_ADDRESS;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

/* Scrolls a credit block: the header and its parts in speed_y and z move up 1 pixel every 2 frames until
   the part in z is at screen y -0x14 (scrolled_out); then the parts are freed and the block removed. The
   longer blocks use it only on their first frame. */
void scroll_credits_block(void)
{
	uint16_t speed_y = 0, z = 0;

	if (OBJECT(current_object)->scroll_credits.scrolled_out != 0u)
	{
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		delete_current_object();
		return;
	}
	OBJECT(object_cursor)->timer++;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->speed_y;
	scroll_credits_sprite();
	current_object = OBJECT(current_object)->z;
	scroll_credits_sprite();
	wrap_credits_scroll_timer();
	current_object = OBJECT(current_object)->z;
	if (OBJECT(current_object)->screen_y <= -0x14)
	{
		OBJECT(object_cursor)->scroll_credits.scrolled_out = 1u;
		return;
	}
}

/* The PROGRAM. credit block: header 0x97C0 and four credits_program_parts kept in speed_y, z,
   ending.part_3 and animation, scrolling up 1 pixel every 2 frames until the last is at screen y -0x14;
   then the parts are freed and it is removed. */
void ending_credits_program_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->animation;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97C0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = credits_program_parts_ADDRESS;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

/* The DESIGN. credit block: header 0x97E8 and five credits_design_parts kept in speed_y, z,
   ending.part_3, animation and x, scrolling up 1 pixel every 2 frames until the last is at screen y
   -0x14; then the parts are freed and it is removed. */
void ending_credits_design_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, x = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->x;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E8u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = credits_design_parts_ADDRESS;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

/* The SOUND. credit block: header 0x97E0 and four credits_sound_parts kept in speed_y, z, ending.part_3
   and animation, scrolled like ending_credits_program_scroll. */
void ending_credits_sound_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->animation;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97E0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = credits_sound_parts_ADDRESS;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

/* The VOICE ACTOR. credit block: header 0x97F0 and five credits_voice_actor_parts kept in speed_y, z,
   ending.part_3, animation and x, scrolled like the other blocks. */
void ending_credits_voice_actor_scroll(void)
{
	uint16_t animation = 0, part_3 = 0, speed_y = 0, x = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->x;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97F0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = credits_voice_actor_parts_ADDRESS;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

/* The SPECIAL THANKS. credit block, the last: header 0x97D0 and eight credits_special_thanks_parts kept
   in speed_y, z, ending.part_3, animation, x, y, floor_z and previous_floor_z, scrolled like the other
   blocks. */
void ending_credits_special_thanks_scroll(void)
{
	uint16_t animation = 0, floor_z = 0, part_3 = 0, previous_floor_z = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t field_cursor = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		if (OBJECT(current_object)->step == 0u)
		{
			OBJECT(object_cursor)->timer++;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->speed_y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->ending.part_3;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->animation;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->x;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->y;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->floor_z;
			scroll_credits_sprite();
			current_object = OBJECT(current_object)->previous_floor_z;
			scroll_credits_sprite();
			wrap_credits_scroll_timer();
			current_object = OBJECT(current_object)->previous_floor_z;
			if (OBJECT(current_object)->screen_y <= -0x14)
			{
				OBJECT(object_cursor)->step = 1u;
				return;
			}
			return;
		}
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		part_3 = (uint16_t)OBJECT(object_cursor)->ending.part_3;
		free_object_2(part_3);
		animation = (uint16_t)OBJECT(object_cursor)->animation;
		free_object_2(animation);
		x = (uint16_t)OBJECT(object_cursor)->x;
		free_object_2(x);
		y = (uint16_t)OBJECT(object_cursor)->y;
		free_object_2(y);
		floor_z = (uint16_t)OBJECT(object_cursor)->floor_z;
		free_object_2(floor_z);
		previous_floor_z = (uint16_t)OBJECT(object_cursor)->previous_floor_z;
		free_object_2(previous_floor_z);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ending.started = 1u;
	OBJECT(current_object)->step = 0u;
	OBJECT(current_object)->timer = 0;
	OBJECT(current_object)->screen_y = 0xF4;
	OBJECT(current_object)->sprite_frame = 0x97D0u;
	OBJECT(current_object)->sprite_palette = 0x1010u;
	init_credits_sprite();
	OBJECT(current_object)->speed_x = credits_special_thanks_parts_ADDRESS;
	for (;;)
	{
		field_cursor = OBJECT(object_cursor)->speed_x;
		if (CREDIT_PART(field_cursor)->sprite_frame == 0xFFFFu)
			break;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(object)->sprite_frame = CREDIT_PART(field_cursor)->sprite_frame;
		field_cursor += 2u;
		OBJECT(object)->screen_y = CREDIT_PART(field_cursor - 0x2u)->screen_y;
		field_cursor += 2u;
		OBJECT(object)->sprite_palette = CREDIT_PART(field_cursor - 0x4u)->sprite_palette;
		field_cursor += 2u;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)(current_object + CREDIT_PART(field_cursor - 0x6u)->slot);
		OBJECT(object_cursor)->speed_x = field_cursor + 2u;
		OBJECT(current_object)->credit_part_slot = object;
		current_object = object;
		init_credits_sprite();
	}
	scroll_credits_block();
}

/* Draws a credit sprite in its layer, 1 pixel higher when the block's (object_cursor's) timer has
   reached 2; current_object goes back to object_cursor. */
void scroll_credits_sprite(void)
{
	if (OBJECT(object_cursor)->timer >= 2)
		OBJECT(current_object)->screen_y--;
	queue_sprite_in_layer();
	current_object = object_cursor;
}

/* Puts the block's (object_cursor's) timer back to 0 once it reaches 2: the credits move 1 pixel every 2
   frames. */
void wrap_credits_scroll_timer(void)
{
	if (OBJECT(object_cursor)->timer >= 2)
		OBJECT(object_cursor)->timer = 0;
}

/* Puts the current credit sprite at screen x 0x58, layer 0, no flags, scale 0x100; current_object goes
   back to object_cursor. */
void init_credits_sprite(void)
{
	OBJECT(current_object)->screen_x = 0x58;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	current_object = object_cursor;
}

/* One step of timed_spawns_3 for the credits: once the next record's time is reached it starts an object
   whose handler is the record's offset from ending_credits_fade_in, and moves on; at the 0xFFFF end the
   timer object is removed. */
void run_timed_spawns_3(void)
{
	uint32_t ignored = 0, object = 0, spawn_record = 0;

	spawn_record = timed_spawns_3_ADDRESS + OBJECT(current_object)->run_timed_spawns.list_index * 4u;
	OBJECT(current_object)->z = TIMED_SPAWN(spawn_record)->handler_offset;
	if (OBJECT(current_object)->z_fraction == 0xFFFFu)
	{
		delete_current_object();
		return;
	}
	if (TIMED_SPAWN(spawn_record)->time <= OBJECT(current_object)->timer)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(current_object)->z += ending_credits_fade_in_ADDRESS;
		OBJECT(object)->handler = OBJECT(current_object)->z;
		OBJECT(object)->flags_low_byte = 0u;
		OBJECT(current_object)->run_timed_spawns.list_index++;
	}
}

/* Draws a runner of the first scene with the frame at frame_entry: it grows by 1 every 4 frames and
   takes the next of its 12 frames every 2; removed once ending_first_scene_done is 1. */
void animate_ending_runner(uint32_t frame_entry)
{
	if (ending_first_scene_done == 1u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_frame = ENDING_SPRITE_FRAMES(frame_entry)[0];
	OBJECT(current_object)->sprite_flags = 0x100u;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if ((OBJECT(current_object)->timer & 3u) == 2u)
		OBJECT(current_object)->sprite_scale++;
	if ((OBJECT(current_object)->timer & 1u) != 0u)
	{
		OBJECT(current_object)->animate_ending.frame_index++;
		if (OBJECT(current_object)->animate_ending.frame_index > 0xB)
			OBJECT(current_object)->animate_ending.frame_index = 0;
	}
}

/* Opens the portrait window: clip rectangle 1 grows by 1 pixel a frame on each side up to 0,0-0x140,0xE0
   (step collects a bit per finished edge) while NBG0 scrolls 1 a frame; then it starts
   ending_credits_fade_out and is removed. */
void ending_window_open(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
	}
	nbg0_scroll_x_int++;
	mem_write16(video_clip_1_left, mem_read16(video_clip_1_left) - 1u);
	mem_write16(video_clip_1_right, mem_read16(video_clip_1_right) + 1u);
	mem_write16(video_clip_1_top, mem_read16(video_clip_1_top) - 1u);
	mem_write16(video_clip_1_bottom, mem_read16(video_clip_1_bottom) + 1u);
	if ((int16_t)mem_read16(video_clip_1_left) <= 0)
	{
		mem_write16(video_clip_1_left, 0u);
		OBJECT(current_object)->step |= 1u;
	}
	if ((int16_t)mem_read16(video_clip_1_top) <= 0)
	{
		mem_write16(video_clip_1_top, 0u);
		OBJECT(current_object)->step |= 2u;
	}
	if ((int16_t)mem_read16(video_clip_1_right) >= 0x140)
	{
		mem_write16(video_clip_1_right, 0x140u);
		OBJECT(current_object)->step |= 4u;
	}
	if ((int16_t)mem_read16(video_clip_1_bottom) >= 0xE0)
	{
		mem_write16(video_clip_1_bottom, 0xE0u);
		OBJECT(current_object)->step |= 8u;
	}
	if (OBJECT(current_object)->step != 0xFu)
		return;
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_credits_fade_out_ADDRESS;
	delete_current_object();
}

/* Fades the credits to black (update_fade, fade_kind 0), the second colour offsets following
   color_offset_red/green/blue; when done it starts ending_eggman_scene_fade_in and is removed. */
void ending_credits_fade_out(void)
{
	uint32_t ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		fade_step = 0u;
		fade_kind = 0;
		OBJECT(current_object)->ending.started = 1u;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	allocate_object_outside_loop(&ignored_2, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_scene_fade_in_ADDRESS;
	delete_current_object();
}

/* Sets up Eggman's scene on its first frame (ending_stripes_script, the page shadows, all layers on) and
   fades it in from black (fade_kind 2), the second colour offsets following; when done it starts
   ending_eggman_fly_in and is removed. */
void ending_eggman_scene_fade_in(void)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0, script = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		update_fade(0u);
		color_offset_2_red = color_offset_red;
		color_offset_2_blue = color_offset_blue;
		color_offset_2_green = color_offset_green;
		if (fade_kind >= 0)
			return;
		allocate_object_outside_loop(&ignored_2, &object);
		OBJECT(object)->ending_eggman_fly_in.started = 0u;
		OBJECT(object)->handler = ending_eggman_fly_in_ADDRESS;
		delete_current_object();
		return;
	}
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xBu);
	script = ending_stripes_script_ADDRESS;
	run_vram_script(&ignored, &script, &object);
	nbg0_pages = 0x3030303u;
	nbg1_pages = 0x2020202u;
	nbg2_pages = 0x8080707u;
	nbg3_pages = 0xF0F0F0Fu;
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & ~0xFu);
	OBJECT(current_object)->ending.started = 1u;
	fade_step = 0u;
	fade_kind = 2;
	nbg0_scroll_x_int = 0u;
}

/* Eggman's machine flies in along ending_eggman_flight_path for 0x1E0 frames, a jet flame 0x20 behind it
   until x 0xA6 and eight smoke puffs handed along every 0xA frames (drawn while timer is above 0x32);
   sound 0x75 at 0x104. Then ending_eggman_turn with its sprite; it frees its parts, and its path pointer
   too as if a part. */
void ending_eggman_fly_in(void)
{
	uint8_t sound_id = 0;
	uint16_t exhaust_flame = 0, path = 0, puff_1 = 0, puff_2 = 0, puff_3 = 0, puff_4 = 0, puff_5 = 0, puff_6 = 0, puff_7 = 0, puff_8 = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, object_2 = 0, y_entry = 0;

	if (OBJECT(current_object)->ending_eggman_fly_in.started == 0u)
	{
		OBJECT(current_object)->ending_eggman_fly_in.started = 1u;
		OBJECT(current_object)->step = 0u;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->screen_x = -0x20;
		OBJECT(current_object)->ending_eggman_fly_in.path = ending_eggman_flight_path_ADDRESS;
		OBJECT(current_object)->timer = 0x1E0;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.exhaust_flame = object;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_1 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_2 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_3 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_4 = object;
		init_eggman_smoke_puff(object);
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_5 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_6 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B78u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_7 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B70u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->ending_eggman_fly_in.puff_8 = object;
		init_eggman_smoke_puff(object);
		OBJECT(object)->sprite_frame = 0x9B68u;
	}
	OBJECT(current_object)->timer--;
	if ((uint16_t)OBJECT(current_object)->timer == 0x104u)
	{
		sound_id = 0x75u;
		queue_sound(sound_id, &ignored_3, &ignored_2);
	}
	if ((uint16_t)OBJECT(current_object)->timer != 0u)
	{
		y_entry = OBJECT(current_object)->ending_eggman_fly_in.path;
		OBJECT(current_object)->screen_x = SCREEN_POINT(y_entry)->x;
		y_entry += 2u;
		OBJECT(current_object)->screen_y = SCREEN_POINT(y_entry - 0x2u)->y;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AE0u;
		OBJECT(current_object)->sprite_palette = 0x1250u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->ending_eggman_fly_in.path = y_entry + 2u;
		queue_sprite_in_layer();
		if ((uint16_t)OBJECT(current_object)->screen_x != 0xA6u)
		{
			current_object = OBJECT(current_object)->ending_eggman_fly_in.exhaust_flame;
			OBJECT(current_object)->sprite_frame = 0x9AB0u;
			if ((OBJECT(object_cursor)->timer & 2u) != 0u)
				OBJECT(current_object)->sprite_frame = 0x9AB8u;
			OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(current_object)->sprite_layer = 0u;
			OBJECT(current_object)->sprite_scale = 0x100u;
			OBJECT(current_object)->screen_x -= 0x20u;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->sprite_palette = 0x10E0u;
			queue_sprite_in_layer();
		}
		else
			OBJECT(current_object)->ending_eggman_fly_in.path += 4u;
		current_object = object_cursor;
		if ((uint16_t)((int16_t)OBJECT(current_object)->timer % 0xA) == 0u)
		{
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_7;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_8;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_6;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_7;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_5;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_6;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_4;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_5;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_3;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_4;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_2;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_3;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object_2 = OBJECT(object_cursor)->ending_eggman_fly_in.puff_1;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_2;
			OBJECT(object)->screen_x = OBJECT(object_2)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_2)->screen_y;
			object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_1;
			OBJECT(object)->screen_x = OBJECT(object_cursor)->screen_x;
			OBJECT(object)->screen_y = OBJECT(object_cursor)->screen_y;
			OBJECT(object)->screen_x -= 0x34u;
		}
		if (OBJECT(object_cursor)->timer > 0x32)
		{
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_1;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_2;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_3;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_4;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_5;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_6;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_7;
			queue_sprite_in_layer();
			current_object = OBJECT(object_cursor)->ending_eggman_fly_in.puff_8;
			queue_sprite_in_layer();
		}
		return;
	}
	allocate_object_outside_loop(&ignored, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_turn_ADDRESS;
	OBJECT(object)->sprite_frame = OBJECT(current_object)->sprite_frame;
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->sprite_layer = OBJECT(current_object)->sprite_layer;
	OBJECT(object)->sprite_flags = OBJECT(current_object)->sprite_flags;
	OBJECT(object)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(object)->sprite_scale = OBJECT(current_object)->sprite_scale;
	exhaust_flame = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.exhaust_flame;
	free_object_2(exhaust_flame);
	path = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.path;
	free_object_2(path);
	puff_1 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_1;
	free_object_2(puff_1);
	puff_2 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_2;
	free_object_2(puff_2);
	puff_3 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_3;
	free_object_2(puff_3);
	puff_4 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_4;
	free_object_2(puff_4);
	puff_5 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_5;
	free_object_2(puff_5);
	puff_6 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_6;
	free_object_2(puff_6);
	puff_7 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_7;
	free_object_2(puff_7);
	puff_8 = (uint16_t)OBJECT(current_object)->ending_eggman_fly_in.puff_8;
	free_object_2(puff_8);
	delete_current_object();
}

/* Sets up one of Eggman's smoke puffs, piece: sprite 0x9B88, palette 0x1280, layer 0, at screen (-0x10,
   0), scale 0x100. */
void init_eggman_smoke_puff(uint32_t piece)
{
	OBJECT(piece)->sprite_frame = 0x9B88u;
	OBJECT(piece)->sprite_layer = 0u;
	OBJECT(piece)->screen_x = -0x10;
	OBJECT(piece)->screen_y = 0;
	OBJECT(piece)->sprite_scale = 0x100u;
	OBJECT(piece)->sprite_flags = 0u;
	OBJECT(piece)->sprite_palette = 0x1280u;
}

/* Eggman turns to the viewer: the sprite from ending_eggman_turn_frames by its timer (the other sprite
   fields come from the fly-in); at the 0xFFFF end it starts ending_eggman_scene_fade_out and is removed. */
void ending_eggman_turn(void)
{
	uint32_t frame_record = 0, ignored = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->step = 0u;
	}
	OBJECT(current_object)->timer++;
	frame_record = ending_eggman_turn_frames_ADDRESS + OBJECT(current_object)->step * 4u;
	if ((uint16_t)ENDING_TIMED_FRAME(frame_record)->end_time == 0xFFFFu)
	{
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->ending.started = 0u;
		OBJECT(object)->handler = ending_eggman_scene_fade_out_ADDRESS;
		delete_current_object();
		return;
	}
	if (ENDING_TIMED_FRAME(frame_record)->end_time < OBJECT(current_object)->timer)
		OBJECT(current_object)->step++;
	OBJECT(current_object)->sprite_frame = ENDING_TIMED_FRAME(frame_record)->sprite_frame;
	queue_sprite_in_layer();
}

/* Fades Eggman's scene to black (fade_kind 0), the second colour offsets following; when done it starts
   ending_close_up_fade_in and ending_eggman_fly_away and is removed. */
void ending_eggman_scene_fade_out(void)
{
	uint32_t ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		fade_step = 0u;
		fade_kind = 0;
		OBJECT(current_object)->ending.started = 1u;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	allocate_object_outside_loop(&ignored_2, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_close_up_fade_in_ADDRESS;
	allocate_object_outside_loop(&ignored_2, &object);
	OBJECT(object)->ending.started = 0u;
	OBJECT(object)->handler = ending_eggman_fly_away_ADDRESS;
	delete_current_object();
}

/* Fades the close-up in from black (update_fade, fade_kind 2), the second colour offsets following;
   removed when the fade is done. */
void ending_close_up_fade_in(void)
{
	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		fade_step = 0u;
		fade_kind = 2;
	}
	update_fade(0u);
	color_offset_2_red = color_offset_red;
	color_offset_2_blue = color_offset_blue;
	color_offset_2_green = color_offset_green;
	if (fade_kind >= 0)
		return;
	delete_current_object();
}

/* Eggman's machine in close-up flying off (scale 0x200 down by 3, by 2 below 0x100), palette cycling;
   below scale 0x20: sound 9, a twinkle growing to 0x110 and shrinking, the parts freed, then a circle
   opening around (0xA0, 0x70) in the per-line window tables (radius + 3 every other frame) past 0xE0;
   then the three runners across. */
void ending_eggman_fly_away(void)
{
	uint8_t sound_id = 0;
	uint16_t halfwords = 0, line_dy = 0, scale_step = 0, speed_y = 0, z = 0;
	uint32_t cycle = 0, half_width_squared = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, left_entry = 0, lines_left = 0, object = 0, palette_count = 0, right_entry = 0, source = 0;

	if (OBJECT(current_object)->ending.started == 0u)
	{
		OBJECT(current_object)->ending.started = 1u;
		OBJECT(current_object)->step = 0u;
		mem_write16(video_layer_disable, 0x20u);
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 4u);
		mixer_nbg1_blend = 0x580u;
		mixer_blend = 0u;
		OBJECT(current_object)->screen_x = 0xA0;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 1u;
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_frame = 0x9AD8u;
		OBJECT(current_object)->sprite_palette = 0x1230u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->speed_y = object;
		OBJECT(object)->screen_x = 0xA0;
		OBJECT(object)->screen_y = 0x70;
		OBJECT(object)->sprite_layer = 0u;
		OBJECT(object)->sprite_scale = 0x200u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_flags += 8u;
		OBJECT(object)->sprite_frame = 0x9AD0u;
		OBJECT(object)->sprite_palette = 0x1240u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xA0;
		OBJECT(object)->screen_y = 0x70;
		OBJECT(object)->sprite_layer = 0u;
		OBJECT(object)->sprite_scale = 0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0x9B50u;
		OBJECT(object)->sprite_palette = 0x1270u;
		sound_id = 0x72u;
		queue_sound(sound_id, &ignored_3, &ignored_2);
	}
	object = PALETTE_RAM + 0x8BC0u;
	palette_count = 1u;
	source = ending_eggman_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 9u;
	cycle_palette(cycle, halfwords, &ignored_3, &ignored_2, &palette_count, &source, &object);
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x20)
	{
		current_object = object_cursor;
		scale_step = 3u;
		if ((int16_t)OBJECT(current_object)->sprite_scale < 0x100)
			scale_step = 2u;
		OBJECT(current_object)->sprite_scale -= scale_step;
		queue_sprite_in_layer();
		current_object = OBJECT(current_object)->speed_y;
		OBJECT(current_object)->sprite_scale -= scale_step;
		queue_sprite_in_layer();
		return;
	}
	current_object = OBJECT(current_object)->z;
	switch (OBJECT(object_cursor)->step)
	{
	case 0:
	case 7:
		sound_id = 9u;
		queue_sound(sound_id, &ignored_3, &ignored_2);
		nbg2_pages = 0x2020202u;
		OBJECT(object_cursor)->step = 1u;
		return;
	case 1:
		OBJECT(current_object)->sprite_scale += 5u;
		queue_sprite_in_layer();
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x110)
			OBJECT(object_cursor)->step = 2u;
		return;
	case 2:
		OBJECT(current_object)->sprite_scale -= 5u;
		if ((int16_t)OBJECT(current_object)->sprite_scale >= 0)
		{
			queue_sprite_in_layer();
			return;
		}
		OBJECT(object_cursor)->step = 3u;
		return;
	case 3:
		speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
		free_object_2(speed_y);
		z = (uint16_t)OBJECT(object_cursor)->z;
		free_object_2(z);
		OBJECT(object_cursor)->step = 4u;
		return;
	case 4:
		mem_write16(video_rowscroll_control, 0x5015u);
		mem_write16(video_clip_select, 0x400u);
		mem_write32(nbg2_page_select, 0x2020202u);
		mixer_nbg2_blend = 0x580u;
		mixer_blend = 0u;
		mem_write16(video_layer_disable, 0x2122u);
		nbg2_scroll_x_fraction = 0u;
		nbg2_scroll_y_fraction = 0u;
		OBJECT(object_cursor)->step = 5u;
		circle_wipe_radius_low = 0u;
		OBJECT(current_object)->cycle_count = 0u;
		fill_value = (fill_value & ~0xFFFFu) | 1u;
		set_halfwords(VIDEO_RAM + 0xA000u, 0x200u, fill_value);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM + 0xA400u, 0x200u, fill_value);
		return;
	case 5:
		OBJECT(current_object)->cycle_count++;
		if ((OBJECT(current_object)->cycle_count & 1u) != 0u)
		{
			left_entry = VIDEO_RAM + 0xA0E0u;
			right_entry = VIDEO_RAM + 0xA4E0u;
			circle_wipe_line = 0x70u;
		}
		else
		{
			circle_wipe_radius += 3u;
			left_entry = VIDEO_RAM + 0xA000u;
			right_entry = VIDEO_RAM + 0xA400u;
			circle_wipe_line = 0u;
		}
		lines_left = 0x70u;
		do
		{
			half_width_squared = (uint16_t)circle_wipe_radius;
			line_dy = (uint16_t)(circle_wipe_line - 0x70u);
			half_width_squared = (uint16_t)(half_width_squared * half_width_squared - (uint32_t)line_dy * line_dy);
			if ((int16_t)half_width_squared >= 0)
			{
				square_root(&half_width_squared, &ignored_3);
				circle_wipe_left = 0xA0u;
				circle_wipe_right = 0xA0u;
				circle_wipe_right += half_width_squared;
				circle_wipe_left -= half_width_squared;
			}
			else if ((OBJECT(current_object)->cycle_count & 1u) != 0u)
			{
				if ((int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1) <= (int16_t)(circle_wipe_radius + 0x70u))
				{
					circle_wipe_left = 0u;
					circle_wipe_right = 0x13Fu;
				}
				else
				{
					circle_wipe_left = 1u;
					circle_wipe_right = 0u;
				}
			}
			else if ((int16_t)(0x70u - circle_wipe_radius) <= (int16_t)((int32_t)(left_entry - (VIDEO_RAM + 0xA000u)) >> 1))
			{
				circle_wipe_left = 0u;
				circle_wipe_right = 0x13Fu;
			}
			else
			{
				circle_wipe_left = 1u;
				circle_wipe_right = 0u;
			}
			if ((int16_t)circle_wipe_left <= 0)
				circle_wipe_left = 0u;
			if ((int16_t)circle_wipe_right >= 0x13F)
				circle_wipe_right = 0x13Fu;
			mem_write16(left_entry, circle_wipe_left);
			mem_write16(right_entry, circle_wipe_right);
			circle_wipe_line++;
			lines_left--;
			left_entry += 2u;
			right_entry += 2u;
		} while (lines_left != 0u);
		if (circle_wipe_radius > 0xE0)
			OBJECT(object_cursor)->step = 6u;
		return;
	case 6:
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_sonic_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_ray_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		allocate_object_outside_loop(&ignored, &object);
		OBJECT(object)->handler = ending_mighty_run_across_ADDRESS;
		OBJECT(object)->ending.started = 0u;
		delete_current_object();
		return;
	default:
		invalid_jump(0x1DB45u + OBJECT(object_cursor)->step * 3u);
	}
}

/* Unused: does nothing (a stray return the original has after ending_eggman_fly_away). */
void ending_eggman_fly_away_spare_return(void)
{
}

/* Sonic crossing the screen first: body 0xA810 and legs 0xA820 (layers 1 and 0, ending_sonic_body_frames
   and ending_sonic_leg_frames kept in x and y), 5 pixels a frame; after 0x78 frames, counted in
   hit_list_next, it sets ending_finished. */
void ending_sonic_run_across(void)
{
	uint32_t ignored = 0, ignored_2 = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		OBJECT(object_cursor)->hit_list_next++;
		if ((int16_t)OBJECT(object_cursor)->hit_list_next >= 0x78)
			ending_finished = 1u;
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 5u;
		OBJECT(current_object)->screen_x += 5u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 1u;
	OBJECT(current_object)->sprite_palette = 0x1220u;
	OBJECT(current_object)->sprite_frame = 0xA810u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored_2, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA820u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_palette = 0x1220u;
	OBJECT(current_object)->sprite_layer = 0u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = ending_sonic_body_frames_ADDRESS;
	OBJECT(object_cursor)->y = ending_sonic_leg_frames_ADDRESS;
	OBJECT(object_cursor)->hit_list_next = 0u;
}

/* Ray crossing the screen behind Sonic: body 0xA7E0 and legs 0xA7F0 (scale 0xF0, layers 3 and 2,
   ending_ray_body_frames and ending_ray_leg_frames kept in x and y), 4 pixels a frame. */
void ending_ray_run_across(void)
{
	uint32_t ignored = 0, ignored_2 = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 4u;
		OBJECT(current_object)->screen_x += 4u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 3u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_frame = 0xA7E0u;
	OBJECT(current_object)->sprite_scale = 0xF0u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored_2, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA7F0u;
	OBJECT(current_object)->sprite_scale = 0xF0u;
	OBJECT(current_object)->sprite_palette = 0x1210u;
	OBJECT(current_object)->sprite_layer = 2u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = ending_ray_body_frames_ADDRESS;
	OBJECT(object_cursor)->y = ending_ray_leg_frames_ADDRESS;
}

/* Mighty crossing the screen last: body 0xA7B0 and legs 0xA7C0 (scale 0xE0, layers 4 and 3,
   ending_mighty_body_frames and ending_mighty_leg_frames kept in x and y), 3 pixels a frame. */
void ending_mighty_run_across(void)
{
	uint32_t ignored = 0, ignored_2 = 0, second_object = 0;

	if (OBJECT(current_object)->ending.started != 0u)
	{
		animate_ending_runner_across();
		OBJECT(object_cursor)->screen_x += 3u;
		OBJECT(current_object)->screen_x += 3u;
		return;
	}
	OBJECT(current_object)->sprite_layer = 4u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_frame = 0xA7B0u;
	OBJECT(current_object)->sprite_scale = 0xE0u;
	init_ending_runner_across();
	allocate_object_2(&ignored, &ignored_2, &second_object);
	OBJECT(current_object)->speed_y = second_object;
	current_object = second_object;
	OBJECT(current_object)->sprite_frame = 0xA7C0u;
	OBJECT(current_object)->sprite_scale = 0xE0u;
	OBJECT(current_object)->sprite_palette = 0x1200u;
	OBJECT(current_object)->sprite_layer = 3u;
	init_ending_runner_across();
	OBJECT(current_object)->screen_y += 0x1Au;
	OBJECT(current_object)->screen_x -= 4u;
	OBJECT(object_cursor)->x = ending_mighty_body_frames_ADDRESS;
	OBJECT(object_cursor)->y = ending_mighty_leg_frames_ADDRESS;
}

/* Starts a runner across: the object's started set, timer and cycle_count 0; the current sprite (body or
   legs) at (0, 0x68), palette | 0x2000, sprite flags 0x140. */
void init_ending_runner_across(void)
{
	OBJECT(object_cursor)->ending.started = 1u;
	OBJECT(object_cursor)->timer = 0;
	OBJECT(object_cursor)->ending.cycle_count = 0;
	OBJECT(current_object)->screen_y = 0x68;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x140u;
}

/* Draws a runner crossing the screen: the body frame from body_frames by timer >> 2 (timer 0-7) and the
   legs part from leg_frames by cycle_count (0-3); at screen x 0x300 both go back to 0. */
void animate_ending_runner_across(void)
{
	uint32_t second_frame_record = 0;

	OBJECT(object_cursor)->timer++;
	OBJECT(object_cursor)->cycle_count++;
	OBJECT(object_cursor)->timer &= 7u;
	OBJECT(object_cursor)->cycle_count &= 3u;
	second_frame_record = OBJECT(object_cursor)->animate_ending.leg_frames + OBJECT(object_cursor)->cycle_count * 2u;
	OBJECT(current_object)->sprite_frame = ENDING_SPRITE_FRAMES(OBJECT(object_cursor)->animate_ending.body_frames)[(uint16_t)OBJECT(object_cursor)->timer >> 2];
	queue_sprite_in_layer();
	current_object = OBJECT(object_cursor)->animate_ending.legs;
	OBJECT(current_object)->sprite_frame = ENDING_SPRITE_FRAMES(second_frame_record)[0];
	queue_sprite_in_layer();
	if (OBJECT(object_cursor)->screen_x >= 0x300)
	{
		OBJECT(current_object)->screen_x = 0;
		OBJECT(object_cursor)->screen_x = 0;
	}
}
