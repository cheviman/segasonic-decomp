/**
 *    Name:        scenes/title.c
 *    Description: the title screen: the SEGA logo, the emblem, the land, the craft, Sonic's loop, the clouds and the sky
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Every frame of the title: run_sega_logo; once sega_logo_step is 7 and title_scene_built is 0, builds
   the title: its variables, tile_bank_1_palette_list, title_sky_script and
   title_waving_background_script, NBG2/NBG3 scroll and pages, and the objects title_land_slide,
   title_emblem_animate, timed_spawner_run and title_sky_wave. */
void run_title_screen(uint32_t text_position, uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_text)
{
	uint32_t ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, palette_list = 0, script = 0;
	uint8_t cnt_bits = 0;

	run_sega_logo(text_position, p_color, p_row_pairs_left, p_blocks_per_row, p_text);
	if (sega_logo_step != 7u)
	{
		return;
	}
	if (title_scene_built != 0u)
	{
		return;
	}
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	display_off_frames = 3u;
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	title_sonic_step = 0;
	title_step_timer = 0;
	title_timer = 0;
	title_done = 0u;
	title_ray_mighty_started = 0u;
	title_ray_mighty_frame = 0u;
	title_ray_mighty_delay = 3u;
	title_ray_mighty_wait = 0x28u;
	unused_title_word = 0x58u;
	clear_sprite_buckets();
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x2Fu);
	mixer_blend = 0u;
	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored_2, &object);
	script = title_sky_script_ADDRESS;
	run_vram_script(&ignored_3, &script, &object);
	*p_text = title_waving_background_script_ADDRESS;
	run_vram_script(&ignored_3, p_text, &object);
	nbg2_scroll_x = 0u;
	nbg2_scroll_y = 0xFFE8u;
	nbg3_scroll_x = 0u;
	nbg3_scroll_y = 0xFFE0u;
	nbg2_pages = 0u;
	nbg3_pages = 0x5050505u;
	free_objects = free_object_stack_ADDRESS;
	first_object = first_object_address;
	last_object = last_object_address;
	object = allocate_object_or_start_list(object);
	OBJECT(object)->handler = title_land_slide_ADDRESS;
	OBJECT(object)->title.started = 0u;
	allocate_object(&ignored_4, &object);
	OBJECT(object)->handler = title_emblem_animate_ADDRESS;
	OBJECT(object)->title.started = 0u;
	allocate_object(&ignored_4, &object);
	OBJECT(object)->handler = timed_spawner_run_ADDRESS;
	OBJECT(object)->timed_spawner.started = 0u;
	allocate_object(&ignored_4, &object);
	OBJECT(object)->handler = title_sky_wave_ADDRESS;
	OBJECT(object)->title.started = 0u;
	title_scene_built = 1u;
	mem_write16(video_backdrop_control, 0u);
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) & 0xD0u);
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0x23u);
}

/* The title's emblem in four 8 bpp quarters (0x52E0-0x52F8), Sonic rising (0x5300 + 8 * n,
   title_sonic_rise_heights) then looping (title_sonic_loop_frames), and from step 0x13 Ray and Mighty
   popping up (title_ray_frames, title_mighty_frames). At title_timer 0x258 it sets title_done, frees its
   7 pool 2 blocks and goes. */
void title_emblem_animate(void)
{
	uint16_t animation = 0, emblem_bottom_left = 0, floor_z = 0, sonic_frame = 0, sonic_y = 0, speed_y = 0, x = 0, y = 0, z = 0;
	uint32_t emblem_part = 0, ignored = 0, ignored_2 = 0, loop_entry = 0, sonic_step = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->speed_y = emblem_part;
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->z = emblem_part;
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->title.emblem_bottom_left = emblem_part;
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->animation = emblem_part;
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->x = emblem_part;
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->y = emblem_part;
		allocate_object_2(&ignored, &ignored_2, &emblem_part);
		OBJECT(current_object)->floor_z = emblem_part;
		OBJECT(current_object)->title.started = 1u;
	}
	current_object = OBJECT(object_cursor)->speed_y;
	OBJECT(current_object)->sprite_layer = 5u;
	OBJECT(current_object)->screen_x = 0x5A;
	OBJECT(current_object)->sprite_frame = 0x52E0u;
	OBJECT(current_object)->screen_y = 0x72;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->z;
	OBJECT(current_object)->sprite_layer = 5u;
	OBJECT(current_object)->screen_x = 0xE6;
	OBJECT(current_object)->sprite_frame = 0x52E8u;
	OBJECT(current_object)->screen_y = 0x72;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->title.emblem_bottom_left;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0x63;
	OBJECT(current_object)->screen_y = 0x98;
	OBJECT(current_object)->sprite_frame = 0x52F0u;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->animation;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->screen_x = 0xDB;
	OBJECT(current_object)->screen_y = 0x98;
	OBJECT(current_object)->sprite_frame = 0x52F8u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	draw_title_emblem_part();
	current_object = OBJECT(object_cursor)->x;
	if (title_sonic_step < 0x14)
	{
		sonic_y = (uint16_t)(0x70u - title_sonic_rise_heights[(uint8_t)title_sonic_step]);
		sonic_frame = (uint8_t)title_sonic_step;
	}
	else
	{
		for (;;)
		{
			sonic_step = (uint8_t)title_sonic_step;
			loop_entry = title_sonic_loop_frames_ADDRESS + (uint8_t)(sonic_step - 0x14u);
			if (TITLE_SONIC_LOOP_CODES(loop_entry)[0] != 0xFFu)
				break;
			title_sonic_step = 0x14;
		}
		sonic_y = 0x46u;
		sonic_frame = (uint8_t)(TITLE_SONIC_LOOP_CODES(loop_entry)[0] - 1u);
	}
	title_timer++;
	if (title_timer < 0x258)
	{
		OBJECT(current_object)->screen_y = sonic_y;
		OBJECT(current_object)->sprite_palette = 0x100u;
		OBJECT(current_object)->sprite_frame = sonic_frame * 8u + 0x5300u;
		OBJECT(current_object)->sprite_layer = 0u;
		OBJECT(current_object)->screen_x = 0xA1;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags |= 0x100u;
		queue_sprite_in_layer();
		title_step_timer++;
		if (title_ray_mighty_started != 0u)
		{
			if (title_ray_mighty_wait == 0u)
			{
				title_ray_mighty_delay--;
				if (title_ray_mighty_delay == 0u)
				{
					title_ray_mighty_delay = 2u;
					title_ray_mighty_frame++;
					if (title_mighty_frames[title_ray_mighty_frame] == 0xFFFFu)
						title_ray_mighty_frame--;
				}
				current_object = OBJECT(object_cursor)->y;
				OBJECT(current_object)->sprite_frame = title_mighty_frames[title_ray_mighty_frame];
				OBJECT(current_object)->sprite_palette = 0x840u;
				OBJECT(current_object)->sprite_layer = 2u;
				OBJECT(current_object)->screen_y = 0x81;
				OBJECT(current_object)->screen_x = 0xDA;
				OBJECT(current_object)->sprite_flags = 0u;
				OBJECT(current_object)->sprite_flags &= ~0xFu;
				OBJECT(current_object)->sprite_flags |= 4u;
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_flags |= 0x100u;
				queue_sprite_in_layer();
				current_object = OBJECT(object_cursor)->floor_z;
				OBJECT(current_object)->sprite_frame = title_ray_frames[title_ray_mighty_frame];
				OBJECT(current_object)->sprite_palette = 0x830u;
				OBJECT(current_object)->sprite_layer = 2u;
				OBJECT(current_object)->screen_y = 0x81;
				OBJECT(current_object)->screen_x = 0x66;
				OBJECT(current_object)->sprite_flags = 0u;
				OBJECT(current_object)->sprite_flags &= ~0xFu;
				OBJECT(current_object)->sprite_flags |= 4u;
				OBJECT(current_object)->sprite_scale = 0x100u;
				OBJECT(current_object)->sprite_flags |= 0x100u;
				queue_sprite_in_layer();
			}
			else
				title_ray_mighty_wait--;
		}
		if (title_step_timer < 0)
			return;
		title_sonic_step++;
		if (title_sonic_step <= 4)
		{
			title_step_timer = -2;
			return;
		}
		if (title_sonic_step >= 0x13)
		{
			if (title_sonic_step >= 0x14)
			{
				title_step_timer = -3;
				return;
			}
			title_step_timer = -0x3C;
			title_ray_mighty_started = 1u;
			return;
		}
		title_step_timer = -4;
		return;
	}
	title_done = 1u;
	speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
	free_object_2(speed_y);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	emblem_bottom_left = (uint16_t)OBJECT(object_cursor)->title.emblem_bottom_left;
	free_object_2(emblem_bottom_left);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	floor_z = (uint16_t)OBJECT(object_cursor)->floor_z;
	free_object_2(floor_z);
	delete_current_object();
}

/* Unused: a lone return inside title_emblem_animate that nothing reaches: the code before it always
   branches past it. */
void title_emblem_animate_spare_return(void)
{
}

/* Queues the current object as a part of the title's emblem: palette 0, sprite_flags 0x200 (8 bits per
   pixel), zoom 0x100 (queue_sprite_in_layer). */
void draw_title_emblem_part(void)
{
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x200u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
}

/* The strip of land with green hills (sprite 0xB6D0, palette 0xD70, layer 7) at y 0x7A, sliding left
   from x 0x190 half a pixel a frame (speed_y holds x in 16.16) until title_done, then removed. */
void title_land_slide(void)
{
	if (OBJECT(current_object)->title.started == 0u)
	{
		OBJECT(current_object)->sprite_frame = 0xB6D0u;
		OBJECT(current_object)->sprite_palette = 0xD70u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 7u;
		OBJECT(current_object)->screen_y = 0x7A;
		OBJECT(current_object)->screen_x = 0xA0;
		OBJECT(current_object)->speed_y = 0x1900000;
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->step = 0u;
	}
	if (title_done == 0u)
	{
		OBJECT(current_object)->speed_y -= 0x8000u;
		OBJECT(current_object)->screen_x = (uint32_t)OBJECT(current_object)->speed_y >> 16;
		queue_sprite_in_layer();
		return;
	}
	delete_current_object();
}

/* Spawns the title's timed objects: counts frames in timer and, for each timed_spawns entry whose frame
   has passed, allocates an object whose handler is the entry's offset from timed_spawner_run
   (flags_low_byte 0). Removed at the list's 0xFFFF end or once title_done. */
void timed_spawner_run(void)
{
	uint32_t ignored = 0, object = 0, spawn_record = 0, spawned_handler = 0;
	uint16_t spawn_frame = 0;

	if (OBJECT(current_object)->timed_spawner.started == 0u)
	{
		OBJECT(current_object)->timed_spawner.started = 1u;
		OBJECT(current_object)->timed_spawner.list_index = 0u;
		OBJECT(current_object)->timer = 0;
	}
	for (;;)
	{
		spawn_record = timed_spawns_ADDRESS + OBJECT(current_object)->timed_spawner.list_index * 4u;
		if ((uint16_t)TIMED_SPAWN(spawn_record)->time == 0xFFFFu || title_done != 0u)
		{
			delete_current_object();
			return;
		}
		spawn_frame = TIMED_SPAWN(spawn_record)->time;
		spawn_record += 2u;
		if (OBJECT(current_object)->timer <= (int16_t)spawn_frame)
			break;
		OBJECT(current_object)->timed_spawner.list_index++;
		spawned_handler = TIMED_SPAWN(spawn_record - 0x2u)->handler_offset + 0x973B4u;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = spawned_handler;
		OBJECT(object)->flags_low_byte = 0u;
	}
	OBJECT(current_object)->timer++;
}

/* A small craft (sprite 0xBB58, palette 0xEB0, layer 7) flying left from x 0x154 at y 0x70, one pixel a
   frame; every 0xB frames it leaves a title_smoke_puff_animate puff at its x + 4. Removed once
   title_done. */
void title_flying_craft_move(void)
{
	uint32_t ignored = 0, object = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->sprite_frame = 0xBB58u;
		OBJECT(current_object)->screen_x = 0x154;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 7u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_palette = 0xEB0u;
		OBJECT(current_object)->timer = 0;
	}
	if (title_done != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->screen_x--;
	queue_sprite_in_layer();
	OBJECT(current_object)->timer++;
	if (OBJECT(current_object)->timer > 0xA)
	{
		OBJECT(current_object)->timer = 0;
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = title_smoke_puff_animate_ADDRESS;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x += 4u;
		OBJECT(object)->title.started = 0u;
	}
}

/* A puff of the craft's smoke (palette 0xEB0, layer 7, y 0x70): steps through title_smoke_puff_frames
   every 3 frames and is removed at its 0xFFFF end or once title_done. */
void title_smoke_puff_animate(void)
{
	uint32_t puff_frame = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->sprite_frame = 0xBB60u;
		OBJECT(current_object)->screen_y = 0x70;
		OBJECT(current_object)->sprite_layer = 7u;
		OBJECT(current_object)->sprite_scale = 0x100u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_palette = 0xEB0u;
		OBJECT(current_object)->timer = 0;
		OBJECT(current_object)->title.frame_index = 0u;
	}
	if (title_done != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->title.timer_low++;
	if (OBJECT(current_object)->timer >= 3)
	{
		OBJECT(current_object)->timer = 0;
		puff_frame = OBJECT(current_object)->title.frame_index;
		OBJECT(current_object)->title.frame_index++;
		puff_frame = title_smoke_puff_frames_ADDRESS + puff_frame * 2u;
		OBJECT(current_object)->sprite_frame = TITLE_SMOKE_PUFF_SPRITES(puff_frame)[0];
		if (TITLE_SMOKE_PUFF_SPRITES(puff_frame)[0] == 0xFFFFu)
		{
			delete_current_object();
			return;
		}
	}
	queue_sprite_in_layer();
}

/* Unused: sets up puff as a smoke puff like title_smoke_puff_animate's, but at y 0x7A: layer 7, zoom
   0x100, sprite_flags 0, palette 0xEB0. */
void init_title_puff_sprite(uint32_t puff)
{
	OBJECT(puff)->screen_y = 0x7A;
	OBJECT(puff)->sprite_layer = 7u;
	OBJECT(puff)->sprite_scale = 0x100u;
	OBJECT(puff)->sprite_flags = 0u;
	OBJECT(puff)->sprite_palette = 0xEB0u;
}

/* Twelve white clouds (sprites 0xB6D8, 0xB6E0, 0xB6F0 at several zooms, palette 0xD80, layer 8): itself
   and 11 pool 2 blocks kept in its fields, moved by drift_title_cloud every 4 frames and drawn every
   frame. Once title_done it frees the blocks and goes. */
void title_clouds_1_drift(void)
{
	uint16_t animation = 0, cloud_4 = 0, floor_block = 0, floor_z = 0, previous_floor_z = 0, speed_x = 0, speed_y = 0, speed_z = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->title_clouds.started == 0u)
	{
		OBJECT(current_object)->title_clouds.started = 1u;
		OBJECT(current_object)->screen_x = 0x31;
		OBJECT(current_object)->screen_y = 0x25;
		OBJECT(current_object)->sprite_scale = 0xE0u;
		OBJECT(current_object)->sprite_frame = 0xB6D8u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 8u;
		OBJECT(current_object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->speed_z = object;
		OBJECT(object)->screen_x = 0xB3;
		OBJECT(object)->screen_y = 0x2A;
		OBJECT(object)->sprite_scale = 0xE0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xB3;
		OBJECT(object)->screen_y = 0x2A;
		OBJECT(object)->sprite_scale = 0xE0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->title_clouds.cloud_4 = object;
		OBJECT(object)->screen_x = 0x113;
		OBJECT(object)->screen_y = 0x20;
		OBJECT(object)->sprite_scale = 0xE0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->animation = object;
		OBJECT(object)->screen_x = 0x3A;
		OBJECT(object)->screen_y = 0x5A;
		OBJECT(object)->sprite_scale = 0xB0u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->x = object;
		OBJECT(object)->screen_x = 0x61;
		OBJECT(object)->screen_y = 0x46;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->y = object;
		OBJECT(object)->screen_x = 0xD0;
		OBJECT(object)->screen_y = 0x4D;
		OBJECT(object)->sprite_scale = 0x80u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->floor_z = object;
		OBJECT(object)->screen_x = 0x8E;
		OBJECT(object)->screen_y = 0x63;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->previous_floor_z = object;
		OBJECT(object)->screen_x = 0x40;
		OBJECT(object)->screen_y = 0x7F;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->floor_block = object;
		OBJECT(object)->screen_x = 0x7D;
		OBJECT(object)->screen_y = 0x7A;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->speed_x = object;
		OBJECT(object)->screen_x = 0x115;
		OBJECT(object)->screen_y = 0x74;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->speed_y = object;
		OBJECT(object)->screen_x = 0x10D;
		OBJECT(object)->screen_y = 0x3E;
		OBJECT(object)->sprite_scale = 0x90u;
		OBJECT(object)->sprite_frame = 0xB6F0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		OBJECT(current_object)->timer = 0;
	}
	if (title_done == 0u)
	{
		OBJECT(object_cursor)->timer++;
		if (OBJECT(object_cursor)->timer > 3)
		{
			OBJECT(object_cursor)->timer = 0;
			current_object = object_cursor;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_y;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_x;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->animation;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->x;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->y;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->floor_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->previous_floor_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->floor_block;
			drift_title_cloud();
		}
		current_object = object_cursor;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->floor_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->previous_floor_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->floor_block;
		queue_sprite_in_layer();
		return;
	}
	speed_y = (uint16_t)OBJECT(object_cursor)->speed_y;
	free_object_2(speed_y);
	speed_x = (uint16_t)OBJECT(object_cursor)->speed_x;
	free_object_2(speed_x);
	speed_z = (uint16_t)OBJECT(object_cursor)->speed_z;
	free_object_2(speed_z);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	cloud_4 = (uint16_t)OBJECT(object_cursor)->title_clouds.cloud_4;
	free_object_2(cloud_4);
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
	floor_block = (uint16_t)OBJECT(object_cursor)->floor_block;
	free_object_2(floor_block);
	delete_current_object();
}

/* Seven more clouds (sprites 0xB6D8-0xB700, palette 0xD80, layer 8): itself and 6 pool 2 blocks kept in
   its fields, moved by drift_title_cloud_2 every 4 frames and drawn every frame. Once title_done it
   frees the blocks and goes. */
void title_clouds_2_drift(void)
{
	uint16_t animation = 0, cloud_4 = 0, speed_z = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->title_clouds.started == 0u)
	{
		OBJECT(current_object)->title_clouds.started = 1u;
		OBJECT(current_object)->screen_x = 0x6B;
		OBJECT(current_object)->screen_y = 0x41;
		OBJECT(current_object)->sprite_scale = 0x60u;
		OBJECT(current_object)->sprite_frame = 0xB6E0u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 8u;
		OBJECT(current_object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->speed_z = object;
		OBJECT(object)->screen_x = 0xB6;
		OBJECT(object)->screen_y = 0x40;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0xFC;
		OBJECT(object)->screen_y = 0x36;
		OBJECT(object)->sprite_scale = 0xD0u;
		OBJECT(object)->sprite_frame = 0xB700u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->title_clouds.cloud_4 = object;
		OBJECT(object)->screen_x = 0x5B;
		OBJECT(object)->screen_y = 0x56;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6F8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->animation = object;
		OBJECT(object)->screen_x = 0xE6;
		OBJECT(object)->screen_y = 0x51;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->x = object;
		OBJECT(object)->screen_x = 0x69;
		OBJECT(object)->screen_y = 0x71;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->y = object;
		OBJECT(object)->screen_x = 0xC1;
		OBJECT(object)->screen_y = 0x71;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		OBJECT(current_object)->timer = 0;
	}
	if (title_done == 0u)
	{
		OBJECT(object_cursor)->timer++;
		if (OBJECT(object_cursor)->timer > 3)
		{
			OBJECT(object_cursor)->timer = 0;
			current_object = object_cursor;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->speed_z;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->z;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->animation;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->x;
			drift_title_cloud_2();
			current_object = OBJECT(object_cursor)->y;
			drift_title_cloud_2();
		}
		current_object = object_cursor;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		queue_sprite_in_layer();
		return;
	}
	speed_z = (uint16_t)OBJECT(object_cursor)->speed_z;
	free_object_2(speed_z);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	cloud_4 = (uint16_t)OBJECT(object_cursor)->title_clouds.cloud_4;
	free_object_2(cloud_4);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	delete_current_object();
}

/* Seven clouds coming in from the right (x 0x146-0x1E7, palette 0xD80, layer 8): itself and 6 pool 2
   blocks, moved by drift_title_cloud every 4 frames and drawn every frame. Once title_done it frees the
   blocks and goes. */
void title_clouds_3_drift(void)
{
	uint16_t animation = 0, cloud_4 = 0, speed_z = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	if (OBJECT(current_object)->title_clouds.started == 0u)
	{
		OBJECT(current_object)->title_clouds.started = 1u;
		OBJECT(current_object)->screen_x = 0x156;
		OBJECT(current_object)->screen_y = 0x5F;
		OBJECT(current_object)->sprite_scale = 0x40u;
		OBJECT(current_object)->sprite_frame = 0xB6E0u;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_layer = 8u;
		OBJECT(current_object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->speed_z = object;
		OBJECT(object)->screen_x = 0x1A1;
		OBJECT(object)->screen_y = 0x5E;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->z = object;
		OBJECT(object)->screen_x = 0x1E7;
		OBJECT(object)->screen_y = 0x54;
		OBJECT(object)->sprite_scale = 0xA0u;
		OBJECT(object)->sprite_frame = 0xB6F8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->title_clouds.cloud_4 = object;
		OBJECT(object)->screen_x = 0x146;
		OBJECT(object)->screen_y = 0x74;
		OBJECT(object)->sprite_scale = 0x60u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->animation = object;
		OBJECT(object)->screen_x = 0x1D1;
		OBJECT(object)->screen_y = 0x6F;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->x = object;
		OBJECT(object)->screen_x = 0x154;
		OBJECT(object)->screen_y = 0x8F;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6E0u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		allocate_object_2(&ignored, &ignored_2, &object);
		OBJECT(current_object)->y = object;
		OBJECT(object)->screen_x = 0x1AC;
		OBJECT(object)->screen_y = 0x8F;
		OBJECT(object)->sprite_scale = 0x40u;
		OBJECT(object)->sprite_frame = 0xB6D8u;
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_layer = 8u;
		OBJECT(object)->sprite_palette = 0xD80u;
		OBJECT(current_object)->timer = 0;
	}
	if (title_done == 0u)
	{
		OBJECT(object_cursor)->timer++;
		if (OBJECT(object_cursor)->timer > 3)
		{
			OBJECT(object_cursor)->timer = 0;
			current_object = object_cursor;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->speed_z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->z;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->animation;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->x;
			drift_title_cloud();
			current_object = OBJECT(object_cursor)->y;
			drift_title_cloud();
		}
		current_object = object_cursor;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->speed_z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->z;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->title_clouds.cloud_4;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->animation;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->x;
		queue_sprite_in_layer();
		current_object = OBJECT(object_cursor)->y;
		queue_sprite_in_layer();
		return;
	}
	speed_z = (uint16_t)OBJECT(object_cursor)->speed_z;
	free_object_2(speed_z);
	z = (uint16_t)OBJECT(object_cursor)->z;
	free_object_2(z);
	cloud_4 = (uint16_t)OBJECT(object_cursor)->title_clouds.cloud_4;
	free_object_2(cloud_4);
	animation = (uint16_t)OBJECT(object_cursor)->animation;
	free_object_2(animation);
	x = (uint16_t)OBJECT(object_cursor)->x;
	free_object_2(x);
	y = (uint16_t)OBJECT(object_cursor)->y;
	free_object_2(y);
	delete_current_object();
}

/* Unused: sets up cloud as a title cloud: zoom 0x80, layer 8, sprite_flags 0, palette 0xD80 (the clouds'
   own set-up is the same with other zooms). */
void init_title_cloud_block(uint32_t cloud)
{
	OBJECT(cloud)->sprite_scale = 0x80u;
	OBJECT(cloud)->sprite_layer = 8u;
	OBJECT(cloud)->sprite_flags = 0u;
	OBJECT(cloud)->sprite_palette = 0xD80u;
}

/* Moves the current cloud up 1 and left 3 and grows it by 2 (sprite_scale), up to 0x140. */
void drift_title_cloud(void)
{
	OBJECT(current_object)->screen_y--;
	OBJECT(current_object)->screen_x -= 3u;
	OBJECT(current_object)->sprite_scale += 2u;
	if ((int16_t)OBJECT(current_object)->sprite_scale > 0x140)
		OBJECT(current_object)->sprite_scale = 0x140u;
}

/* drift_title_cloud, but a sprite_scale past 0x120 jumps to 0x140. */
void drift_title_cloud_2(void)
{
	OBJECT(current_object)->screen_y--;
	OBJECT(current_object)->screen_x -= 3u;
	OBJECT(current_object)->sprite_scale += 2u;
	if ((int16_t)OBJECT(current_object)->sprite_scale > 0x120)
		OBJECT(current_object)->sprite_scale = 0x140u;
}

/* The waving sky: first clears the row scroll table and its sums in video RAM and turns NBG2's row
   scroll on (video_rowscroll_control 0x401); then each frame its 0x80 rows add a sine_table value to
   their offsets and scroll by them, NBG3 moves 1/4 pixel and the colours cycle
   (title_sky_cycle_palettes). Removed once title_done. */
void title_sky_wave(void)
{
	uint16_t halfwords = 0;
	uint32_t cycle = 0, dest = 0, ignored_3 = 0, ignored_4 = 0, palette_count = 0, row_offset = 0, row_scroll = 0, sine_cursor = 0, source = 0, wave_rows_left = 0;

	if (OBJECT(current_object)->title.started == 0u)
	{
		fill_value = 0u;
		set_halfwords(VIDEO_RAM + 0x800u, 0x400u, fill_value);
		set_halfwords(VIDEO_RAM + 0x2000u, 0x400u, fill_value);
		mem_write16(video_rowscroll_control, 0x401u);
		OBJECT(current_object)->title.started = 1u;
		OBJECT(current_object)->speed_y = 0;
	}
	if (title_done != 0u)
	{
		delete_current_object();
		return;
	}
	sine_cursor = sine_table_ADDRESS;
	row_scroll = VIDEO_RAM + 0x900u;
	row_offset = VIDEO_RAM + 0x2100u;
	wave_rows_left = 0x80u;
	do
	{
		mem_write32(row_offset, mem_read32(row_offset) + ((uint32_t)TITLE_WAVE_SINES(sine_cursor)[0] << 3));
		mem_write32(row_offset, mem_read32(row_offset) & 0x1FFFFFFu);
		mem_write16(row_scroll, mem_read32(row_offset) >> 16);
		row_scroll += 2u;
		sine_cursor = ((sine_cursor + 2u) & ~0xFFFFu) | (uint16_t)(sine_cursor + 2u + 2u);
		wave_rows_left--;
		row_offset += 4u;
	} while (wave_rows_left != 0u);
	OBJECT(current_object)->speed_y += 0x4000u;
	OBJECT(current_object)->speed_y &= 0x3FFFFFFu;
	nbg3_scroll_x = (uint32_t)OBJECT(current_object)->speed_y >> 16;
	dest = PALETTE_RAM + 0x8F60u;
	palette_count = 5u;
	source = title_sky_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 3u;
	palette_cycle_0_frames = 0x10u;
	cycle_palette(cycle, halfwords, &ignored_3, &ignored_4, &palette_count, &source, &dest);
}

/* The SEGA logo, by sega_logo_step: 0 set-up (8 bpp bitmap filled with 0x8E, colour offsets 0x20), 1
   fade in (+ 1 every 2 frames to 0x3F), 2 zoom in (sega_logo_scale + 0x18 a frame), 3 animation
   (sega_logo_frame to 0x1E), 4 0x69 frames, 5 fade out, 6 0x3C frames, 7 done. The sprite is a pool 2
   block taken and freed every frame. */
void run_sega_logo(uint32_t logo_block, uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_lower_row_cursor)
{
	uint8_t is_8bpp = 0;
	uint16_t object = 0, scroll_x = 0, scroll_y = 0;
	uint32_t end_mark = 0, ignored = 0, ignored_2 = 0, ignored_5 = 0, ignored_6 = 0;
	uint8_t cnt_bits = 0;

	if (sega_logo_step == 7u)
		return;
	if (sega_logo_step == 0u)
	{
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits & ~2u);
		display_off_frames = 5u;
		init_sprites(&end_mark, p_lower_row_cursor);
		logo_block = clear_sprite_buckets();
		clear_bitmap_layer((uint8_t)end_mark, &ignored, p_color, &ignored_2, p_row_pairs_left, p_blocks_per_row, p_lower_row_cursor);
		title_scene_built = 1u;
		title_done = 0u;
		scroll_x = 0u;
		scroll_y = 0u;
		is_8bpp = 1u;
		set_bitmap_scroll(scroll_x, scroll_y, is_8bpp);
		mixer_text_blend &= 0xBFFFu;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		color_offset_red = 0x20u;
		color_offset_blue = 0x20u;
		color_offset_green = 0x20u;
		mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xFu);
		mem_write32(video_backdrop_control, mem_read32(video_backdrop_control) & ~0x200u);
		sega_logo_frame = 0;
		sega_logo_timer = 0;
		sega_logo_scale = 0;
		mem_write16(mixer_bitmap, 0x21u);
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = (fill_value & ~0xFFFFu) | 0x8E8Eu;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		sega_logo_step++;
	}
	allocate_object_2(&ignored_5, &ignored_6, &logo_block);
	current_object = logo_block;
	screen_sprite_block = current_object;
	switch (sega_logo_step)
	{
	case 0:
		sega_logo_timer = 0;
		sega_logo_step++;
		set_sega_logo_sprite();
		OBJECT(current_object)->sprite_scale = sega_logo_scale;
		OBJECT(current_object)->sprite_frame = 0x53D0u;
		queue_sprite_in_layer();
		sega_logo_scale += 0x18u;
		if (sega_logo_scale < 0xFF)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		sega_logo_frame++;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	case 1:
	case 7:
		sega_logo_timer++;
		if (sega_logo_timer < 2)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_timer_word = 0u;
		color_offset_red++;
		color_offset_blue++;
		color_offset_green++;
		if ((int16_t)color_offset_red < 0x3F)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_timer = 0;
		sega_logo_step++;
		set_sega_logo_sprite();
		OBJECT(current_object)->sprite_scale = sega_logo_scale;
		OBJECT(current_object)->sprite_frame = 0x53D0u;
		queue_sprite_in_layer();
		sega_logo_scale += 0x18u;
		if (sega_logo_scale < 0xFF)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		sega_logo_frame++;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	case 2:
		set_sega_logo_sprite();
		OBJECT(current_object)->sprite_scale = sega_logo_scale;
		OBJECT(current_object)->sprite_frame = 0x53D0u;
		queue_sprite_in_layer();
		sega_logo_scale += 0x18u;
		if (sega_logo_scale < 0xFF)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		sega_logo_frame++;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	case 3:
		set_sega_logo_sprite();
		queue_sprite_in_layer();
		sega_logo_timer++;
		if ((sega_logo_timer & 2u) == 0u)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_timer = 0;
		sega_logo_frame++;
		if (sega_logo_frame <= 0x1E)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		sega_logo_frame--;
		sega_logo_timer = 0;
		sega_logo_scale = 0;
		color_offset_red = 0x3Fu;
		color_offset_blue = 0x3Fu;
		color_offset_green = 0x3Fu;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	case 4:
		set_sega_logo_sprite();
		queue_sprite_in_layer();
		sega_logo_timer++;
		if (sega_logo_timer < 0x69)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	case 5:
		set_sega_logo_sprite();
		queue_sprite_in_layer();
		color_offset_red--;
		color_offset_blue--;
		color_offset_green--;
		if ((int16_t)color_offset_red >= 0x21)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		sega_logo_timer = 0;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	case 6:
		sega_logo_timer++;
		if (sega_logo_timer < 0x3C)
		{
			object = screen_sprite_block;
			free_object_2(object);
			return;
		}
		sega_logo_step++;
		mem_write32(video_control, mem_read32(video_control) | 0x800u);
		fill_value = fill_value & ~0xFFFFu;
		set_halfwords(VIDEO_RAM, 0xE000u, fill_value);
		mixer_sprite_control &= ~0x8000u;
		mixer_bitmap_blend &= 0xBFFFu;
		title_scene_built = 0u;
		object = screen_sprite_block;
		free_object_2(object);
		return;
	default:
		invalid_jump(0x9824Eu + sega_logo_step * 3u);
	}
}

/* The SEGA logo's sprite on the current object: at 0xA0,0x70, palette 0x420, layer 0, zoom 0x100,
   sprite_flags 0, frame 0x53D0 + 8 * sega_logo_frame. */
void set_sega_logo_sprite(void)
{
	OBJECT(current_object)->screen_x = 0xA0;
	OBJECT(current_object)->screen_y = 0x70;
	OBJECT(current_object)->sprite_palette = 0x420u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = ((uint32_t)(uint8_t)sega_logo_frame << 3) + 0x53D0u;
}
