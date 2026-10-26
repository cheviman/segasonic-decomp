/**
 *    Name:        stage/layers.c
 *    Description: the stage's background: the palettes animated per stage, the palette sets and the backdrop, and the tilemap layers NBG0-NBG3 (pictures, mixing, per-stage scroll)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Game mode 0x10's stage 6 background, by stage_06_background_step: 0-2 the cloudy horizon and the
   forest (palette set 1), drifting sideways; 3-4 the upper and lower jaws drawn and closed over the
   screen, sound 0x1E, then 7 a shake dying away (stage_06_shake); 5-6 the horizon and the island tower,
   two palette cycles started at random. */
void update_stage_06_background(uint32_t random_value, uint32_t dest, uint32_t *p_rolls_left, uint32_t *p_words_left, uint32_t *p_source)
{
	uint32_t source = *p_source;
	uint8_t sound_id = 0;
	uint16_t shake_y = 0;
	uint32_t cycle_set = 0, ignored = 0, ignored_3 = 0, ignored_4 = 0, palette_stage = 0, script = 0;
	uint8_t flipped_sign = 0;
	uint16_t previous_shake = 0;

	switch (stage_06_background_step)
	{
	case 0:
	case 8:
		mixer_blend &= 0x1F00u;
		mixer_blend |= 0xA4u;
		camera_x = 0;
		camera_y = 0;
		update_camera_view();
		stage_06_background_step++;
		break;
	case 1:
		if (mixer_blend != 0xA4u)
		{
			mixer_blend = 0xA4u;
			*p_source = source;
			return;
		}
		load_default_palettes();
		palette_stage = 1u;
		load_palette_set_and_backdrop(palette_stage, &ignored_3, &source, &dest);
		if (mem_read16(palette_color_61D0) == 0x7B0u)
		{
			camera_x = 0;
			camera_y = -0x600000;
			update_camera_view();
			nbg3_scroll_x = 0u;
			nbg3_scroll_y = 0u;
			nbg1_scroll_x = 0u;
			nbg1_scroll_y = 0u;
			nbg0_scroll_x = 0u;
			nbg0_scroll_y = 0xFFA00000u;
			mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
			script = cloudy_horizon_script_ADDRESS;
			run_vram_script(&ignored_4, &script, &dest);
			source = stage_06_forest_script_ADDRESS;
			run_vram_script(&ignored_4, &source, &dest);
			nbg3_pages = 0u;
			nbg0_pages = 0x5050505u;
			mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
			stage_06_background_step++;
		}
		else
			mixer_blend = 0xA4u;
		break;
	case 2:
		nbg0_scroll_x -= 0x2000u;
		nbg1_scroll_x -= 0x1000u;
		nbg3_scroll_x_long = (int32_t)nbg1_scroll_x >> 16;
		break;
	case 3:
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		mixer_blend &= 0x1F00u;
		script = clear_tile_page_00_script_ADDRESS;
		run_vram_script(&ignored_4, &script, &dest);
		script = stage_06_upper_jaw_script_ADDRESS;
		run_vram_script(&ignored_4, &script, &dest);
		script = clear_tile_page_0a_script_ADDRESS;
		run_vram_script(&ignored_4, &script, &dest);
		source = stage_06_lower_jaw_script_ADDRESS;
		run_vram_script(&ignored_4, &source, &dest);
		nbg0_pages = 1u;
		nbg3_pages = 6u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0x90u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0xFF70u;
		mem_write16(mixer_nbg0, mixer_default_nbg0 | 0xFu);
		mem_write16(mixer_nbg3, mixer_default_nbg3 | 0xFu);
		stage_06_background_step++;
		break;
	case 4:
		nbg0_scroll_y_int -= 0x10u;
		if ((int16_t)nbg0_scroll_y_int <= 0)
			nbg0_scroll_y_int = 0u;
		nbg3_scroll_y += 0x10u;
		if ((int16_t)nbg3_scroll_y >= 0)
			nbg3_scroll_y = 0u;
		if ((nbg0_scroll_y_int | nbg3_scroll_y) == 0u)
		{
			stage_06_background_step = 7u;
			stage_06_shake = 0x800u;
			stage_06_shake_sign = 0;
			sound_id = 0x1Eu;
			play_sound(sound_id, &ignored_4, &ignored);
		}
		break;
	case 5:
		mixer_blend &= 0x1F00u;
		mixer_blend |= 0xA4u;
		palette_cycle_set = 0x209090u;
		cycle_set = palette_cycle_set;
		PALETTE_SET_CYCLE(cycle_set)->dest = PALETTE_RAM + 0x83A0u;
		PALETTE_SET_CYCLE(cycle_set)->frames = stage_06_cycle_palettes_0_ADDRESS;
		PALETTE_SET_CYCLE(cycle_set)->timer = 2u;
		PALETTE_SET_CYCLE(cycle_set)->delay = 2u;
		PALETTE_SET_CYCLE(cycle_set)->rows_left = 3u;
		PALETTE_SET_CYCLE(cycle_set)->rows = 3u;
		PALETTE_SET_CYCLE(cycle_set)->frame = 0;
		PALETTE_SET_CYCLE(cycle_set)->frame_count = 6;
		cycle_set += 0x10u;
		PALETTE_SET_CYCLE(cycle_set)->dest = PALETTE_RAM + 0x8400u;
		PALETTE_SET_CYCLE(cycle_set)->frames = stage_06_cycle_palettes_1_ADDRESS;
		PALETTE_SET_CYCLE(cycle_set)->timer = 7u;
		PALETTE_SET_CYCLE(cycle_set)->delay = 7u;
		PALETTE_SET_CYCLE(cycle_set)->rows_left = 2u;
		PALETTE_SET_CYCLE(cycle_set)->rows = 2u;
		PALETTE_SET_CYCLE(cycle_set)->frame = 0;
		PALETTE_SET_CYCLE(cycle_set)->frame_count = 6;
		camera_x = 0;
		camera_y = 0;
		update_camera_view();
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		nbg0_scroll_x_int = 0u;
		nbg0_scroll_y_int = 0u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0x3Fu);
		script = cloudy_horizon_script_ADDRESS;
		run_vram_script(&ignored_4, &script, &dest);
		source = stage_06_island_tower_script_ADDRESS;
		run_vram_script(&ignored_4, &source, &dest);
		nbg3_pages = 0x3020100u;
		nbg0_pages = 0x8070605u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0x26u);
		mem_write16(mixer_nbg0, mixer_default_nbg0);
		mem_write16(mixer_nbg3, mixer_default_nbg3);
		stage_06_background_step++;
		break;
	case 6:
		if (palette_cycle_set_0_frame != 0u)
		{
			source = cycle_palette_set_0(source);
			source = cycle_palette_set_1(source);
			*p_source = source;
			return;
		}
		if (palette_cycle_set_1_frame != 0u)
		{
			source = cycle_palette_set_1(source);
			*p_source = source;
			return;
		}
		*p_rolls_left = 2u;
		for (;;)
		{
			next_random(&random_value, p_words_left, &source);
			random_value &= 0xFu;
			if ((int8_t)random_value > 2)
				break;
			(*p_rolls_left)--;
			if (*p_rolls_left == 0u)
			{
				source = cycle_palette_set_0(source);
				source = cycle_palette_set_1(source);
				break;
			}
		}
		break;
	case 7:
		if (stage_06_shake != 0u)
		{
			previous_shake = stage_06_shake;
			stage_06_shake = previous_shake - 0x80u;
			if ((int16_t)previous_shake < 0x80)
				stage_06_shake = 0u;
			shake_y = stage_06_shake_pixels;
			flipped_sign = ~stage_06_shake_sign;
			stage_06_shake_sign = flipped_sign;
			if (flipped_sign != 0u)
				shake_y = (uint16_t)(0u - shake_y);
			nbg0_scroll_y_int = shake_y;
			nbg3_scroll_y = shake_y;
		}
		break;
	default:
		invalid_jump(0x178B8u + stage_06_background_step * 3u);
	}
	*p_source = source;
}

/* Runs the stage's palette animation: animate_palette_stage_04, _09, _0a, _0c, _0d_0e (stages 0xD and
   0xE), _0f or _10; the other stages have none (skip_stage_palette_animation). */
void animate_stage_palette(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	switch (stage)
	{
	case 0:
		skip_stage_palette_animation();
		break;
	case 1:
		skip_stage_palette_animation();
		break;
	case 2:
		skip_stage_palette_animation();
		break;
	case 3:
		skip_stage_palette_animation();
		break;
	case 4:
		animate_palette_stage_04(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 5:
		skip_stage_palette_animation();
		break;
	case 6:
		skip_stage_palette_animation();
		break;
	case 7:
		skip_stage_palette_animation();
		break;
	case 8:
		skip_stage_palette_animation();
		break;
	case 9:
		animate_palette_stage_09(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 10:
		animate_palette_stage_0a(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 11:
		skip_stage_palette_animation();
		break;
	case 12:
		animate_palette_stage_0c(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 13:
		animate_palette_stage_0d_0e(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 14:
		animate_palette_stage_0d_0e(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 15:
		animate_palette_stage_0f(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 16:
		animate_palette_stage_10(p_copy_offset, p_copy_entry, p_palette_count, p_cycle, p_source, p_dest);
		break;
	case 17:
		skip_stage_palette_animation();
		break;
	case 18:
		skip_stage_palette_animation();
		break;
	case 19:
		skip_stage_palette_animation();
		break;
	default:
		invalid_jump(0x1AD41u + stage * 3u);
	}
}

/* Does nothing: animate_stage_palette's case for the stages without a palette animation. */
void skip_stage_palette_animation(void)
{
}

/* Stage 0x10's palette animation: palette_cycle_0 every 6 frames through the 8 frames of
   stage_10_cycle_palettes_0 (6 rows at palette RAM + 0x8B40), palette_cycle_1 every 4 frames through the
   8 of stage_10_cycle_palettes_1 (1 row at + 0x8940). */
void animate_palette_stage_10(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;
	uint32_t cycle = 0, dest = 0, ignored = 0, palette_count = 0, source = 0;

	dest = PALETTE_RAM + 0x8B40u;
	palette_count = 6u;
	source = stage_10_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(cycle, halfwords, &ignored, p_copy_entry, &palette_count, &source, &dest);
	*p_cycle = palette_cycle_1_ADDRESS;
	*p_dest = PALETTE_RAM + 0x8940u;
	*p_palette_count = 1u;
	*p_source = stage_10_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 4u;
	palette_cycle_1_frames = 8u;
	cycle_palette(*p_cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
}

/* Stage 0xC's palette animation: palette_cycle_0 every 6 frames through the 8 frames of
   stage_0c_cycle_palettes, 1 row at palette RAM + 0x8800. */
void animate_palette_stage_0c(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;

	*p_dest = PALETTE_RAM + 0x8800u;
	*p_palette_count = 1u;
	*p_source = stage_0c_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	*p_cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(*p_cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
}

/* The palette animation of stages 0xD and 0xE: palette_cycle_0 every 6 frames through the 8 frames of
   stage_0d_0e_cycle_palettes, 1 row at palette RAM + 0x88C0. */
void animate_palette_stage_0d_0e(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;

	*p_dest = PALETTE_RAM + 0x88C0u;
	*p_palette_count = 1u;
	*p_source = stage_0d_0e_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	*p_cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 6u;
	palette_cycle_0_frames = 8u;
	cycle_palette(*p_cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
}

/* Stage 0xF's palette animation: palette_cycle_0 every 0xC frames through the 6 frames of
   stage_0f_cycle_palettes_0 (3 rows at palette RAM + 0x8920), palette_cycle_1 every 6 frames through the
   6 of stage_0f_cycle_palettes_1 (1 row at + 0x8900). */
void animate_palette_stage_0f(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;
	uint32_t cycle = 0, ignored = 0;

	*p_dest = PALETTE_RAM + 0x8920u;
	*p_palette_count = 3u;
	*p_source = stage_0f_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 0xCu;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, p_copy_entry, p_palette_count, p_source, p_dest);
	cycle = palette_cycle_1_ADDRESS;
	*p_dest = PALETTE_RAM + 0x8900u;
	*p_palette_count = 1u;
	*p_source = stage_0f_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 6u;
	palette_cycle_1_frames = 6u;
	cycle_palette(cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
	*p_cycle = cycle;
}

/* Stage 0xA's palette animation: palette_cycle_0 every 5 frames through the 6 frames of
   stage_0a_cycle_palettes_0 (1 row at palette RAM + 0x8A40), palette_cycle_1 every 5 frames through the
   0xA of stage_0a_cycle_palettes_1 (9 rows at + 0x8A60). */
void animate_palette_stage_0a(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;
	uint32_t cycle = 0, ignored = 0;

	*p_dest = PALETTE_RAM + 0x8A40u;
	*p_palette_count = 1u;
	*p_source = stage_0a_cycle_palettes_0_ADDRESS;
	halfwords = 0x10u;
	cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 5u;
	palette_cycle_0_frames = 6u;
	cycle_palette(cycle, halfwords, &ignored, p_copy_entry, p_palette_count, p_source, p_dest);
	cycle = palette_cycle_1_ADDRESS;
	*p_dest = PALETTE_RAM + 0x8A60u;
	*p_palette_count = 9u;
	*p_source = stage_0a_cycle_palettes_1_ADDRESS;
	halfwords = 0x10u;
	palette_cycle_1 = 5u;
	palette_cycle_1_frames = 0xAu;
	cycle_palette(cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
	*p_cycle = cycle;
}

/* Stage 9's palette animation: palette_cycle_0 every 3 frames through the 7 frames of
   stage_09_cycle_palettes, 2 rows at palette RAM + 0x88A0. */
void animate_palette_stage_09(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;

	*p_dest = PALETTE_RAM + 0x88A0u;
	*p_palette_count = 2u;
	*p_source = stage_09_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	*p_cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 3u;
	palette_cycle_0_frames = 7u;
	cycle_palette(*p_cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
}

/* Stage 4's palette animation: palette_cycle_0 every 4 frames through the 8 frames of
   stage_04_cycle_palettes, each copied to 2 rows at palette RAM + 0x8500. */
void animate_palette_stage_04(uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_cycle, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;

	*p_dest = PALETTE_RAM + 0x8500u;
	*p_palette_count = 2u;
	*p_source = stage_04_cycle_palettes_ADDRESS;
	halfwords = 0x10u;
	*p_cycle = palette_cycle_0_ADDRESS;
	palette_cycle_0 = 4u;
	palette_cycle_0_frames = 8u;
	cycle_palette(*p_cycle, halfwords, p_copy_offset, p_copy_entry, p_palette_count, p_source, p_dest);
}

/* Steps the palette cycle whose state (delay, frame_count, frame, timer) is at cycle: every delay frames
   it queues the next 16-colour frame from source (halfwords long, the frames 0x20 bytes apart) to the
   palette_count rows at dest, 0x20 apart, back to the first after frame_count. */
void cycle_palette(uint32_t cycle, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry, uint32_t *p_palette_count, uint32_t *p_source, uint32_t *p_dest)
{
	*p_copy_offset = (uint32_t)(uint8_t)PALETTE_CYCLE(cycle)->frame << 5;
	*p_source += *p_copy_offset;
	if (PALETTE_CYCLE(cycle)->timer > 0)
	{
		PALETTE_CYCLE(cycle)->timer--;
		return;
	}
	do
	{
		PALETTE_CYCLE(cycle)->timer = PALETTE_CYCLE(cycle)->delay;
		queue_vblank_copy(*p_source, *p_dest, halfwords, p_copy_offset, p_copy_entry);
		*p_dest += 0x20u;
		(*p_palette_count)--;
	} while (*p_palette_count != 0u);
	PALETTE_CYCLE(cycle)->frame++;
	if (PALETTE_CYCLE(cycle)->frame < PALETTE_CYCLE(cycle)->frame_count)
	{
		PALETTE_CYCLE(cycle)->timer--;
		return;
	}
	PALETTE_CYCLE(cycle)->frame = 0;
	PALETTE_CYCLE(cycle)->timer--;
}

/* cycle_palette_set on the first record (palette_cycle_set = 0x209090), for update_stage_06_background. */
uint32_t cycle_palette_set_0(uint32_t source)
{
	palette_cycle_set = 0x209090u;
	source = cycle_palette_set(source);
	return source;
}

/* cycle_palette_set on the second record (palette_cycle_set = 0x2090A0), for update_stage_06_background. */
uint32_t cycle_palette_set_1(uint32_t source)
{
	palette_cycle_set = 0x2090A0u;
	source = cycle_palette_set(source);
	return source;
}

/* Steps the palette cycle record at palette_cycle_set (frames, dest, timer, delay, rows, frame,
   frame_count): every delay frames it queues the next 16-colour frame of frames to each of its rows at
   dest, 0x20 apart, back to the first after frame_count; hands back the last source. */
uint32_t cycle_palette_set(uint32_t source)
{
	uint16_t halfwords = 0;
	uint32_t cycle_set = 0, dest = 0, ignored = 0, ignored_2 = 0;

	cycle_set = palette_cycle_set;
	if (PALETTE_SET_CYCLE(cycle_set)->timer != 0u)
	{
		PALETTE_SET_CYCLE(cycle_set)->timer--;
		if (PALETTE_SET_CYCLE(cycle_set)->frame_count <= PALETTE_SET_CYCLE(cycle_set)->frame)
			PALETTE_SET_CYCLE(cycle_set)->frame = 0;
		return source;
	}
	PALETTE_SET_CYCLE(cycle_set)->timer = PALETTE_SET_CYCLE(cycle_set)->delay;
	PALETTE_SET_CYCLE(cycle_set)->rows_left = PALETTE_SET_CYCLE(cycle_set)->rows;
	do
	{
		halfwords = 0x10u;
		dest = PALETTE_SET_CYCLE(cycle_set)->dest + ((uint32_t)(uint8_t)(PALETTE_SET_CYCLE(cycle_set)->rows - PALETTE_SET_CYCLE(cycle_set)->rows_left) << 5);
		source = PALETTE_SET_CYCLE(cycle_set)->frames + ((uint32_t)(uint8_t)PALETTE_SET_CYCLE(cycle_set)->frame << 5);
		queue_vblank_copy(source, dest, halfwords, &ignored, &ignored_2);
		if (PALETTE_SET_CYCLE(cycle_set)->rows_left == 0u)
			break;
		PALETTE_SET_CYCLE(cycle_set)->rows_left--;
	} while (PALETTE_SET_CYCLE(cycle_set)->rows_left != 0u);
	PALETTE_SET_CYCLE(cycle_set)->frame++;
	PALETTE_SET_CYCLE(cycle_set)->timer--;
	if (PALETTE_SET_CYCLE(cycle_set)->frame_count <= PALETTE_SET_CYCLE(cycle_set)->frame)
		PALETTE_SET_CYCLE(cycle_set)->frame = 0;
	return source;
}

/* load_palette_set_and_backdrop for the current stage; hands back its tile bank bits and dest. */
void load_stage_palette_set(uint32_t *p_tile_bank_bits, uint32_t *p_dest)
{
	uint32_t ignored = 0, stage_index = 0;

	stage_index = stage;
	load_palette_set_and_backdrop(stage_index, p_tile_bank_bits, &ignored, p_dest);
}

/* Loads stage_index's palettes: video_control's tile bank bit 0x400 from stage_tile_bank_bits, the
   palette list stage_palette_sets[stage_index] (load_palette_list), backdrop control bit 0x200, and
   backdrop_color by the current stage, not stage_index: 0x200 in stages 3 and 7, 0x6212 in 9 and 0x13,
   else 0. Hands back the tile bank bits. */
void load_palette_set_and_backdrop(uint32_t stage_index, uint32_t *p_tile_bank_bits, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t palette_list = 0, palette_set = 0, tile_bank_entry = 0;

	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	tile_bank_entry = stage_tile_bank_bits_ADDRESS + stage_index;
	*p_tile_bank_bits = (tile_bank_entry & ~0xFFFFu) | TILE_BANK_SELECTS(tile_bank_entry)[0];
	*p_tile_bank_bits = (*p_tile_bank_bits & ~0xFFFFu) | (uint16_t)(*p_tile_bank_bits << 10);
	mem_write16(video_control, mem_read16(video_control) | *p_tile_bank_bits);
	palette_set = stage_palette_sets[stage_index];
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = palette_set;
	load_palette_list(palette_list, p_source, p_dest);
	mem_write16(video_backdrop_control, mem_read16(video_backdrop_control) | 0x200u);
	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
		mem_write16(backdrop_color, 0u);
		return;
	case 3:
	case 7:
		mem_write16(backdrop_color, 0x200u);
		return;
	case 9:
	case 19:
		mem_write16(backdrop_color, 0x6212u);
		return;
	default:
		invalid_jump(0x1B033u + stage * 3u);
	}
}

/* Unused: backdrop_color = 0x19AA, a case of load_palette_set_and_backdrop's stage switch that no stage
   uses. */
void set_backdrop_color_19aa(void)
{
	mem_write16(backdrop_color, 0x19AAu);
}

/* Prepares a full-screen picture on NBG0: display off for 3 frames, the tile bank bit 0x400 set, the
   tile_bank_1_palette_list palettes, only NBG0 and the text layer on (video_layer_disable 0x2E), NBG0
   scroll 0 and pages 0. Hands back load_palette_list's dest. */
uint32_t init_nbg0_picture_screen(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;
	uint8_t io_cnt_bits = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 3u;
	mem_write32(video_control, mem_read32(video_control) & ~0x400u);
	mem_write16(video_control, mem_read16(video_control) | 0x400u);
	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = tile_bank_1_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	mem_write16(video_layer_disable, 0x2Eu);
	nbg0_scroll_x_int = 0u;
	nbg0_scroll_y_int = 0u;
	nbg0_pages = 0u;
	return dest;
}

/* Unused: init_nbg0_picture_screen, then winners_dont_use_drugs_script on page 0: the "WINNERS DON'T USE
   DRUGS" screen with the FBI seal. */
void show_winners_dont_use_drugs_screen(void)
{
	uint32_t dest = 0, ignored = 0, script = 0;

	dest = init_nbg0_picture_screen();
	script = winners_dont_use_drugs_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
}

/* Unused: init_nbg0_picture_screen, then recycle_it_script on page 0: the "RECYCLE IT, DON'T TRASH IT!"
   screen with the EPA seal. */
void show_recycle_it_screen(void)
{
	uint32_t dest = 0, ignored = 0, script = 0;

	dest = init_nbg0_picture_screen();
	script = recycle_it_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
}

/* init_nbg0_picture_screen, then NBG0 on page 1 with a repeating pattern of Sonic heads
   (sonic_head_pattern_script) and its 16 colours (nbg0_picture_palette) queued to palette RAM + 0x8BE0:
   the background of the ranking and PLAYER SELECT screens. */
void show_nbg0_picture(uint32_t *p_copy_entry, uint32_t *p_rows, uint32_t *p_source, uint32_t *p_dest)
{
	uint16_t halfwords = 0;
	uint32_t dest = 0, ignored = 0, rows = 0, script = 0;

	dest = init_nbg0_picture_screen();
	nbg0_pages = 0x1010101u;
	script = sonic_head_pattern_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	dest = PALETTE_RAM + 0x8BE0u;
	rows = 1u;
	*p_source = nbg0_picture_palette_ADDRESS;
	halfwords = 0x10u;
	do
	{
		queue_vblank_copy(*p_source, dest, halfwords, &ignored, p_copy_entry);
		dest += 0x20u;
		rows--;
	} while (rows != 0u);
	*p_rows = rows;
	*p_dest = dest;
}

/* The stage's layer mixing: the four NBG mixer registers from mixer_nbg0 on =
   stage_layer_priorities[stage] bytes + 0x200, the low four bits of video_layer_disable from
   stage_layer_disables, video_rowscroll_control from stage_rowscroll_settings. */
uint32_t set_stage_layer_mixing(void)
{
	uint32_t count = 0, mixer_register = 0, priority_cursor = 0, stage_index = 0;

	priority_cursor = stage_layer_priorities_ADDRESS + ((uint32_t)stage << 2);
	mixer_register = mixer_nbg0;
	count = 4u;
	do
	{
		mem_write16(mixer_register, NBG_LAYER_PRIORITIES(priority_cursor)[0] + 0x200u);
		count--;
		priority_cursor++;
		mixer_register += 2u;
	} while (count != 0u);
	stage_index = stage;
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) & ~0xFu);
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | stage_layer_disables[stage_index]);
	mem_write16(video_rowscroll_control, stage_rowscroll_settings[stage_index]);
	return count;
}

/* Puts NBG1 on pages 0xF-0x12 and clears them (four vram scripts), then sets the stage's mixer blend
   bits: one setting for stages 9 and 0x13, one for stage 0x10, one for the rest. Hands back dest. */
uint32_t init_nbg1_layer(uint32_t dest)
{
	uint32_t ignored = 0, script = 0;

	nbg1_pages = 0x1211100Fu;
	script = clear_tile_page_0f_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_10_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_11_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_12_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 17:
	case 18:
		mixer_nbg1_blend &= 0xFF7Fu;
		return dest;
	case 9:
	case 19:
		mixer_nbg0_blend &= ~0x780u;
		mixer_nbg1_blend &= ~0x780u;
		mixer_nbg2_blend &= ~0x780u;
		mixer_nbg3_blend &= ~0x780u;
		mixer_nbg0_blend |= 0x100u;
		return dest;
	case 16:
		mixer_nbg0_blend |= 0x100u;
		mixer_blend |= 0xB00u;
		mixer_nbg1_blend &= ~0xF00u;
		mixer_nbg1_blend &= 0xFF7Fu;
		return dest;
	default:
		invalid_jump(0x1B30Bu + stage * 3u);
	}
}

/* NBG1 follows the camera (set_nbg1_scroll_to_camera) in stages 3, 7, 8, 9, 0xA, 0xB and 0x10; in the
   other stages it stays put. */
void update_nbg1_scroll(void)
{
	switch (stage)
	{
	case 0:
		skip_nbg1_scroll();
		break;
	case 1:
		skip_nbg1_scroll();
		break;
	case 2:
		skip_nbg1_scroll();
		break;
	case 3:
		set_nbg1_scroll_to_camera();
		break;
	case 4:
		skip_nbg1_scroll();
		break;
	case 5:
		skip_nbg1_scroll();
		break;
	case 6:
		skip_nbg1_scroll();
		break;
	case 7:
		set_nbg1_scroll_to_camera();
		break;
	case 8:
		set_nbg1_scroll_to_camera();
		break;
	case 9:
		set_nbg1_scroll_to_camera();
		break;
	case 10:
		set_nbg1_scroll_to_camera();
		break;
	case 11:
		set_nbg1_scroll_to_camera();
		break;
	case 12:
		skip_nbg1_scroll();
		break;
	case 13:
		skip_nbg1_scroll();
		break;
	case 14:
		skip_nbg1_scroll();
		break;
	case 15:
		skip_nbg1_scroll();
		break;
	case 16:
		set_nbg1_scroll_to_camera();
		break;
	case 17:
		skip_nbg1_scroll();
		break;
	case 18:
		skip_nbg1_scroll();
		break;
	case 19:
		skip_nbg1_scroll();
		break;
	default:
		invalid_jump(0x1B398u + stage * 3u);
	}
	skip_nbg1_scroll();
}

/* Does nothing: update_nbg1_scroll's case for the stages whose NBG1 stays put, and its common end. */
void skip_nbg1_scroll(void)
{
}

/* NBG1's scroll = the camera's position in whole pixels (camera_x_pixels, camera_y_pixels). */
void set_nbg1_scroll_to_camera(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg1_scroll_x_int = camera_x_pixels;
	nbg1_scroll_y_int = layer_scroll_y;
}

/* Puts NBG2 on pages 0xA-0xD and clears them, then the stage's mixer blend bits; in stages 1 and 2 it
   also fills the two row scroll tables (video RAM + 0xA800 and + 0xA000, 0x100 entries each) with a sine
   wave and blends NBG2 (mixer_blend 0xB00), as stage 4 does without the wave. */
uint32_t init_nbg2_layer(uint32_t dest)
{
	uint32_t entries_left = 0, ignored = 0, script = 0, table_entry = 0, tables_left = 0, wave_index = 0;

	nbg2_pages = 0xD0C0B0Au;
	script = clear_tile_page_0a_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_0b_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_0c_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_0d_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	switch (stage)
	{
	case 0:
	case 17:
	case 18:
		mixer_nbg2_blend &= 0xFF7Fu;
		return dest;
	case 1:
	case 2:
		row_scroll_wave_step = 0u;
		entries_left = 0x100u;
		table_entry = VIDEO_RAM + 0xA800u;
		tables_left = 2u;
		for (;;)
		{
			row_scroll_wave_step &= 0x7Fu;
			wave_index = row_scroll_wave_step;
			mem_write16(table_entry, (int32_t)(((uint32_t)sine_table[(wave_index & 0xFFFu) * 8u] << 3) & ~0xFFFFu) >> 16);
			row_scroll_wave_step++;
			table_entry += 2u;
			entries_left--;
			if (entries_left != 0u)
				continue;
			entries_left = 0x100u;
			table_entry = VIDEO_RAM + 0xA000u;
			tables_left--;
			if (tables_left == 0u)
				break;
		}
		mixer_nbg2_blend |= 0x80u;
		mixer_blend &= 0xFFu;
		mixer_blend |= 0xB00u;
		return dest;
	case 3:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		mixer_nbg2_blend &= 0xFF7Fu;
		return dest;
	case 4:
		mixer_nbg2_blend |= 0x80u;
		mixer_blend &= 0xFFu;
		mixer_blend |= 0xB00u;
		return dest;
	case 16:
	case 19:
		mixer_nbg2_blend &= ~0xF00u;
		return dest;
	default:
		invalid_jump(0x1B44Fu + stage * 3u);
	}
}

/* Scrolls NBG2 by stage: with the camera in stages 1, 2, 9, 0xA, 0x10 and 0x13,
   set_nbg2_scroll_stages_03_07 in stages 3 and 7, set_nbg2_scroll_stage_04 in stage 4; elsewhere it
   stays put. Hands back position, scroll_position after the stage 3 and 7 case. */
uint32_t update_nbg2_scroll(uint32_t position)
{
	switch (stage)
	{
	case 0:
		skip_nbg2_scroll();
		break;
	case 1:
		set_nbg2_scroll_to_camera_2();
		break;
	case 2:
		set_nbg2_scroll_to_camera_2();
		break;
	case 3:
		position = set_nbg2_scroll_stages_03_07();
		break;
	case 4:
		set_nbg2_scroll_stage_04();
		break;
	case 5:
		skip_nbg2_scroll();
		break;
	case 6:
		skip_nbg2_scroll();
		break;
	case 7:
		position = set_nbg2_scroll_stages_03_07();
		break;
	case 8:
		skip_nbg2_scroll();
		break;
	case 9:
		set_nbg2_scroll_to_camera();
		break;
	case 10:
		set_nbg2_scroll_to_camera();
		break;
	case 11:
		skip_nbg2_scroll();
		break;
	case 12:
		skip_nbg2_scroll();
		break;
	case 13:
		skip_nbg2_scroll();
		break;
	case 14:
		skip_nbg2_scroll();
		break;
	case 15:
		skip_nbg2_scroll();
		break;
	case 16:
		set_nbg2_scroll_to_camera();
		break;
	case 17:
		skip_nbg2_scroll();
		break;
	case 18:
		skip_nbg2_scroll();
		break;
	case 19:
		set_nbg2_scroll_to_camera();
		break;
	default:
		invalid_jump(0x1B525u + stage * 3u);
	}
	skip_nbg2_scroll();
	return position;
}

/* NBG2's scroll = the camera's position in whole pixels (camera_x_pixels, camera_y_pixels). */
void set_nbg2_scroll_to_camera(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg2_scroll_x = camera_x_pixels;
	nbg2_scroll_y = layer_scroll_y;
}

/* Does nothing: update_nbg2_scroll's case for the stages whose NBG2 stays put, and its common end. */
void skip_nbg2_scroll(void)
{
}

/* set_nbg2_scroll_to_camera again: a second copy, for stages 1 and 2. */
void set_nbg2_scroll_to_camera_2(void)
{
	uint16_t layer_scroll_y = 0;

	layer_scroll_y = camera_y_pixels;
	nbg2_scroll_x = camera_x_pixels;
	nbg2_scroll_y = layer_scroll_y;
}

/* Stages 3 and 7: NBG2 on pages 0xA-0xD, scrolled with the camera, 0x10 further in x and y once
   scroll_position reaches 0x9159; hands back scroll_position. */
uint32_t set_nbg2_scroll_stages_03_07(void)
{
	uint16_t layer_scroll_y = 0;
	uint32_t current_position = 0;

	nbg2_pages = 0xD0C0B0Au;
	layer_scroll_y = (uint16_t)((uint32_t)camera_y >> 16);
	nbg2_scroll_x = (uint32_t)camera_x >> 16;
	nbg2_scroll_y = layer_scroll_y;
	current_position = scroll_position;
	if ((int32_t)current_position < 0x9159)
	{
		skip_nbg2_scroll();
		return current_position;
	}
	layer_scroll_y = (uint16_t)(camera_y_pixels + 0x10u);
	nbg2_scroll_x = camera_x_pixels + 0x10u;
	nbg2_scroll_y = layer_scroll_y;
	return current_position;
}

/* Stage 4: NBG2 on pages 0xA-0xD, scrolled with the camera (the original adds 0x20 to x and takes it off
   again). */
void set_nbg2_scroll_stage_04(void)
{
	uint16_t layer_scroll_y = 0;

	nbg2_pages = 0xD0C0B0Au;
	layer_scroll_y = camera_y_pixels;
	nbg2_scroll_x = camera_x_pixels + 0x20u - 0x20u;
	nbg2_scroll_y = layer_scroll_y;
}

/* Draws NBG3 for the stage and sets nbg3_block_map_width (stage_nbg3_widths): the default script with
   set_nbg3_page_5_flags in stages 0-3, 5-7, 0x11 and 0x12, the stages' own scripts in 4, 8 (also 0x13),
   0xA and 0xC-0x10, cleared pages 5-8 in 9 and 0xB; then its nbg3_pages, mostly with the scroll at 0. */
void init_nbg3_layer(uint32_t dest)
{
	uint32_t ignored = 0, ignored_2 = 0, script = 0, stage_index = 0;

	stage_index = stage;
	nbg3_block_map_width = stage_nbg3_widths[stage_index];
	switch (stage_index)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 5:
	case 6:
	case 7:
	case 17:
	case 18:
		script = default_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x6060505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		set_nbg3_page_5_flags(&ignored, &ignored_2);
		return;
	case 4:
		script = stage_04_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = stage_04_nbg3_fill_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x6060505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 8:
	case 19:
		script = stage_08_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 9:
	case 11:
		script = clear_tile_page_05_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = clear_tile_page_06_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = clear_tile_page_07_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		script = clear_tile_page_08_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x8070605u;
		video_layer_opaque_shadow = 0u;
		return;
	case 10:
		script = stage_0a_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x7070707u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 12:
		script = stage_0c_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 13:
		script = stage_0d_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 14:
		script = stage_0e_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 15:
		script = stage_0f_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	case 16:
		script = stage_10_nbg3_script_ADDRESS;
		run_vram_script(&ignored, &script, &dest);
		nbg3_pages = 0x5050505u;
		nbg3_scroll_x = 0u;
		nbg3_scroll_y = 0u;
		return;
	default:
		invalid_jump(0x1B63Fu + stage_index * 3u);
	}
}

/* Unused: a case of init_nbg3_layer no stage uses: clears tile pages 0xA-0xD (as init_nbg2_layer does)
   and puts NBG3 on them, scroll 0, then set_nbg3_page_5_flags. */
void init_nbg3_layer_cleared_pages(uint32_t dest)
{
	uint32_t ignored = 0, ignored_2 = 0, script = 0;

	script = clear_tile_page_0a_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_0b_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_0c_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	script = clear_tile_page_0d_script_ADDRESS;
	run_vram_script(&ignored, &script, &dest);
	nbg3_pages = 0xA0B0C0Du;
	nbg3_scroll_x = 0u;
	nbg3_scroll_y = 0u;
	set_nbg3_page_5_flags(&ignored, &ignored_2);
}

/* Unused: NBG3 on pages 5, 5, 6, 6 (0x06060505) with scroll 0: the end of init_nbg3_layer's stage 4 case
   again, without its scripts. */
void set_nbg3_pages_5_6(void)
{
	nbg3_pages = 0x6060505u;
	nbg3_scroll_x = 0u;
	nbg3_scroll_y = 0u;
}

/* video_layer_opaque_shadow = 0xA, palette_color_4000 = 0xF70, and bit 13 (0x2000) set in the 0x400
   cells from video RAM + 0x1400, tile pages 5 and 6. */
void set_nbg3_page_5_flags(uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t count = 0, dest = 0;

	video_layer_opaque_shadow = 0xAu;
	mem_write16(palette_color_4000, 0xF70u);
	count = 0x400u;
	dest = VIDEO_RAM + 0x1400u;
	do
	{
		mem_write16(dest, mem_read16(dest) | 0x2000u);
		count--;
		dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_dest = dest;
}

/* Per-stage NBG3 parallax: half the view (y + 0x80) with select_nbg3_pages in stages 3, 5, 6, 7 and 0xA,
   and in stage 0 until scroll_ended (then a quarter); three quarters in stages 0xC and 0xE, a quarter in
   0x10, the camera itself in 9; the other stages leave it alone. */
void update_nbg3_scroll(uint32_t *p_scroll_y, uint32_t *p_quadrants, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_quadrant_cursor)
{
	uint32_t scroll_y = *p_scroll_y;
	uint16_t half_view_y = 0, layer_scroll_y = 0;

	switch (stage)
	{
	case 0:
		if ((uint8_t)scroll_ended == 0u)
		{
			half_view_y = (uint16_t)((int16_t)view_y >> 1);
			nbg3_scroll_x = (int16_t)view_x >> 1;
			nbg3_scroll_y = half_view_y;
			nbg3_scroll_y += 0x80u;
			select_nbg3_pages(&scroll_y, p_quadrants, p_speed_value, p_next_scroll_position, p_quadrant_cursor);
		}
		else
		{
			scroll_y = (scroll_y & ~0xFFFFu) | (uint16_t)view_y;
			scroll_y = (scroll_y & ~0xFFFFu) | (uint16_t)((int16_t)scroll_y >> 2);
			nbg3_scroll_x = (int16_t)view_x >> 2;
			nbg3_scroll_y = scroll_y;
			nbg3_scroll_y += 0x80u;
		}
		break;
	case 1:
	case 2:
	case 4:
	case 8:
	case 11:
	case 13:
	case 15:
	case 17:
	case 18:
	case 19:
		break;
	case 3:
	case 5:
	case 6:
	case 7:
	case 10:
		half_view_y = (uint16_t)((int16_t)view_y >> 1);
		nbg3_scroll_x = (int16_t)view_x >> 1;
		nbg3_scroll_y = half_view_y;
		nbg3_scroll_y += 0x80u;
		select_nbg3_pages(&scroll_y, p_quadrants, p_speed_value, p_next_scroll_position, p_quadrant_cursor);
		*p_scroll_y = scroll_y;
		return;
	case 9:
	case 20:
		layer_scroll_y = camera_y_pixels;
		nbg3_scroll_x = camera_x_pixels;
		nbg3_scroll_y = layer_scroll_y;
		break;
	case 12:
	case 14:
		scroll_y = (uint16_t)view_y * 3u;
		scroll_y = (scroll_y & ~0xFFFFu) | (uint16_t)((int16_t)scroll_y >> 2);
		nbg3_scroll_x = (int16_t)(view_x * 3u) >> 2;
		nbg3_scroll_y = scroll_y;
		break;
	case 16:
		scroll_y = (uint16_t)view_y;
		scroll_y = (uint16_t)((int16_t)scroll_y >> 2);
		nbg3_scroll_x = (int16_t)view_x >> 2;
		nbg3_scroll_y = scroll_y;
		break;
	default:
		invalid_jump(0x1B89Eu + stage * 3u);
	}
	*p_scroll_y = scroll_y;
}

/* Sets nbg3_pages from the stage's stage_nbg3_block_maps: the pages of the 2 x 2 blocks at NBG3's scroll
   (get_nbg3_block), each placed by nbg3_quadrant_grid at the parity of the block's column and of its row
   x (nbg3_block_map_width - 1); the scroll is wrapped to 0x3FF, 0x1FF. */
void select_nbg3_pages(uint32_t *p_block_pages, uint32_t *p_quadrants, uint32_t *p_byte_mask, uint32_t *p_shift, uint32_t *p_quadrant_cursor)
{
	uint8_t byte_shift = 0;
	uint32_t block_column = 0, block_row = 0, byte_mask = 0, bytes_left = 0, gathered_pages = 0, gathered_quadrants = 0, map_cursor = 0, pages = 0, quadrant_cursor = 0, shift = 0, stage_index = 0;
	uint32_t next_mask = 0;

	get_nbg3_block();
	stage_index = stage;
	nbg3_block_row = (uint32_t)nbg3_block_row * nbg3_block_map_width;
	map_cursor = stage_nbg3_block_maps[stage_index];
	map_cursor = (map_cursor & ~0xFFFFu) | (uint16_t)(map_cursor + nbg3_block_row);
	map_cursor = (map_cursor & ~0xFFFFu) | (uint16_t)(map_cursor + nbg3_block_column);
	get_nbg3_block();
	nbg3_block_row = nbg3_block_row * (nbg3_block_map_width - 1u);
	block_row = nbg3_block_row;
	block_column = nbg3_block_column;
	quadrant_cursor = nbg3_quadrant_grid_ADDRESS + (block_row & 1u) * 3u + (block_column & 1u);
	nbg3_scroll_x &= 0x3FFu;
	nbg3_scroll_y &= 0x1FFu;
	bytes_left = 4u;
	byte_shift = 0u;
	gathered_pages = 0u;
	do
	{
		gathered_pages |= rotate_by(NBG3_BLOCK_PAGES(map_cursor)[0], (uint8_t)byte_shift, 32);
		map_cursor++;
		byte_shift += 8u;
		if (bytes_left == 3u)
			map_cursor = (map_cursor & ~0xFFFFu) | (uint16_t)(map_cursor + (nbg3_block_map_width - 2u));
		bytes_left--;
	} while (bytes_left != 0u);
	*p_block_pages = gathered_pages;
	bytes_left = 4u;
	byte_shift = 0u;
	gathered_quadrants = 0u;
	do
	{
		gathered_quadrants |= rotate_by(NBG3_QUADRANTS(quadrant_cursor)[0], (uint8_t)byte_shift, 32);
		quadrant_cursor++;
		byte_shift += 8u;
		if (bytes_left == 3u)
			quadrant_cursor = (quadrant_cursor & ~0xFFFFu) | (uint16_t)(quadrant_cursor + 1u);
		bytes_left--;
	} while (bytes_left != 0u);
	*p_quadrants = gathered_quadrants;
	byte_mask = 0xFFu;
	shift = 0u;
	pages = 0u;
	do
	{
		pages |= shift_by(shift_by(*p_block_pages & byte_mask, (uint8_t)shift, 32), (uint8_t)(shift_by(*p_quadrants & byte_mask, (uint8_t)shift, 32) << 3), 32);
		next_mask = byte_mask << 8;
		byte_mask = next_mask;
		shift -= 8u;
	} while (next_mask != 0u);
	nbg3_pages = pages;
	*p_byte_mask = byte_mask;
	*p_shift = shift;
	*p_quadrant_cursor = quadrant_cursor;
}

/* The NBG3 block under its scroll: nbg3_block_column = nbg3_scroll_x >> 9 (blocks of 0x200 pixels),
   nbg3_block_row = (nbg3_scroll_y + 0x400) >> 8 (blocks of 0x100). */
void get_nbg3_block(void)
{
	nbg3_block_column = nbg3_scroll_x;
	nbg3_block_row = nbg3_scroll_y;
	nbg3_block_row += 0x400u;
	nbg3_block_column &= ~0x1FFu;
	nbg3_block_row &= ~0xFFu;
	nbg3_block_column = (int16_t)nbg3_block_column >> 9;
	nbg3_block_row = (int16_t)nbg3_block_row >> 8;
}

/* Copies the stage's own palette: stage_palettes[stage] gives a source and a halfword count, copied to
   palette RAM + 0xB800. */
void load_stage_palette(uint32_t *p_palette_record, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t halfword_count = 0, palette_record = 0;

	palette_record = stage_palettes_ADDRESS + ((uint32_t)stage << 3);
	*p_source = STAGE_PALETTE(palette_record)->source;
	palette_record += 4u;
	halfword_count = STAGE_PALETTE(palette_record - 0x4u)->halfword_count;
	*p_dest = PALETTE_RAM + 0xB800u;
	move_halfwords(PALETTE_RAM + 0xB800u, *p_source, halfword_count);
	*p_palette_record = palette_record;
}
