/**
 *    Name:        stage/camera.c
 *    Description: the stage view, the scroll and the camera: the view set-up and the tile sets around it, the scroll modes and the screen edges the players push, the camera's speeds, shake and path along the sections, the fades between stages, the start section of the demos and the unused scroll skip panel
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* When camera_velocity_x/_y changed (camera_ground_speed_vx/_vy): camera_ground_speed = sqrt(vx * vx +
   vy * vy) << 13 with vx, vy the velocities >> 5 and >> 4, camera_ground_direction the angle of (>> 9,
   >> 8) + 0x80, camera_map_direction the angle of (>> 8, >> 8); hands back that angle's octant (else
   octant as it came). */
uint32_t compute_camera_ground_speed(uint32_t octant)
{
	uint16_t dy = 0;
	uint32_t ground_angle = 0, ignored = 0, ignored_2 = 0, map_angle = 0, vx = 0, vy = 0;

	if (camera_ground_speed_vx != (uint32_t)camera_velocity_x || camera_ground_speed_vy != (uint32_t)camera_velocity_y)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_ground_speed = (vx & 0xFFFFu) << 13;
		ground_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &ground_angle, &ignored_2);
		camera_ground_direction = (ground_angle + 0x80u) & 0x3FFu;
		map_angle = (int32_t)camera_velocity_x >> 8;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &map_angle, &octant);
		camera_map_direction = map_angle;
		camera_ground_speed_vx = camera_velocity_x;
		camera_ground_speed_vy = camera_velocity_y;
	}
	return octant;
}

/* The same on a floor slope (slope_factor, 0 flat), recomputed only when the velocity or slope_factor
   changed: camera_slope_speed, camera_slope_direction (+ 0x80), camera_scroll_direction (angle of the
   velocities >> 5). On a slope camera_velocity_y is taken >> 5 and divided by 1 + |slope_factor|
   (16.16), not >> 4 as when flat. */
uint32_t compute_camera_slope_speed(uint32_t octant, uint32_t slope_factor)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, ignored_2 = 0, scroll_angle = 0, slope_angle = 0, vx = 0, vy = 0;

	if (((uint32_t)camera_velocity_x != 0u || (uint32_t)camera_velocity_y != 0u || camera_slope_speed == 0u) && camera_slope_factor == (uint16_t)slope_factor && camera_slope_speed_vx == (uint32_t)camera_velocity_x && camera_slope_speed_vy == (uint32_t)camera_velocity_y)
		return octant;
	camera_slope_factor = slope_factor;
	if (slope_factor == 0u)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		camera_slope_speed = (vx & 0xFFFFu) << 13;
		slope_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &slope_angle, &ignored_2);
		camera_slope_direction = (slope_angle + 0x80u) & 0x3FFu;
		scroll_angle = (int32_t)camera_velocity_x >> 5;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
		angle_of_vector(dy, &scroll_angle, &octant);
		camera_scroll_direction = scroll_angle;
		camera_slope_speed_vx = camera_velocity_x;
		camera_slope_speed_vy = camera_velocity_y;
		return octant;
	}
	if ((int32_t)slope_factor < 0)
		slope_factor = 0u - slope_factor;
	vy = divide_s32((uint32_t)camera_velocity_y << 11, slope_factor + 0x10000u);
	slope_velocity_y = vy;
	vx = (int32_t)camera_velocity_x >> 5;
	vx = vx * vx + vy * vy;
	square_root(&vx, &ignored);
	camera_slope_speed = (vx & 0xFFFFu) << 13;
	slope_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)slope_velocity_y;
	angle_of_vector(dy, &slope_angle, &ignored_2);
	camera_slope_direction = (slope_angle + 0x80u) & 0x3FFu;
	scroll_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
	angle_of_vector(dy, &scroll_angle, &octant);
	camera_scroll_direction = scroll_angle;
	camera_slope_speed_vx = camera_velocity_x;
	camera_slope_speed_vy = camera_velocity_y;
	return octant;
}

/* Sets up a stage's view: display off 8 frames, default palettes, the tile sets of
   stage_initial_tile_sets, zoom, blending, the NBG1-3 layers; the camera at the stage's origin, view
   offsets, scroll_end and the first section's speeds, limits, scroll mode and direction from the
   per-stage tables; scene steps, shake and scroll skip panel reset. */
void init_stage_view(uint32_t *p_direction_y, uint32_t *p_direction_x, uint32_t *p_count, uint32_t *p_stage_offset)
{
	uint16_t camera_origin = 0;
	uint32_t dest = 0, direction_y_list = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, list = 0, sets_left = 0, stage_number = 0, table_offset = 0, tile_set = 0, tile_set_cursor = 0;
	uint8_t io_cnt_bits = 0;
	uint32_t unrotated_sets = 0;

	io_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, io_cnt_bits & ~2u);
	display_off_frames = 8u;
	dest = load_default_palettes();
	debug_sprite_x = 0x30u;
	debug_sprite_y = 0x30u;
	debug_sprite_spawned = 0u;
	circle_wipe_left = 0x82u;
	circle_wipe_right = 0xBEu;
	circle_wipe_line = 0x32u;
	unused_circle_wipe_byte = 0u;
	circle_wipe_radius_low = 0xAu;
	loaded_tile_sets = 0u;
	nbg0_pages = 0x3020100u;
	nbg2_pages = 0xD0C0B0Au;
	mem_write32(nbg0_page_select, 0x3020100u);
	tile_set_cursor = stage_initial_tile_sets[stage];
	sets_left = 4u;
	mem_write8(video_layer_disable, mem_read8(video_layer_disable) | 0xFu);
	fade_speed = 3u;
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
	tile_set_cursor += stage_initial_tile_sets_ADDRESS;
	do
	{
		tile_set = STAGE_START_TILE_SETS(tile_set_cursor)[0];
		loaded_tile_sets_low += tile_set;
		unrotated_sets = loaded_tile_sets;
		loaded_tile_sets = (unrotated_sets << 8) | (unrotated_sets >> 24);
		tile_set_cursor += 4u;
		load_tile_set(tile_set, &ignored, &ignored_2, &ignored_3, &dest);
		sets_left--;
	} while (sets_left != 0u);
	wanted_tile_sets = loaded_tile_sets;
	mixer_blend &= 0x1F00u;
	switch (stage)
	{
	case 0:
	case 9:
	case 16:
		break;
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
	case 19:
		mixer_blend = 0u;
		break;
	default:
		invalid_jump(0x15A33u + stage * 3u);
	}
	stage_06_background_step = 0u;
	scroll_ended = 0;
	fade_step = 0u;
	stage_03_end_started = 0u;
	stage_03_end_step = 0u;
	stage_03_end_frames = 0u;
	unused_view_word = 0u;
	stage_00_exit_step = 0u;
	stage_00_exit_delay = 0u;
	stage_00_exit_timer = 0;
	stage_00_exit_zoom = 0u;
	color_offset_red = 0u;
	color_offset_blue = 0u;
	color_offset_green = 0u;
	stage_05_switch_step_word = 0u;
	map_screen_step = 0u;
	dest = init_nbg2_layer(dest);
	dest = init_nbg1_layer(dest);
	init_nbg3_layer(dest);
	*p_count = set_stage_layer_mixing();
	load_stage_palette_set(&ignored_4, &ignored_5);
	view_y = 0;
	view_x = 0;
	camera_x_pixels = 0u;
	camera_y_pixels = 0u;
	tile_set_slot = 0u;
	camera_x = 0;
	camera_y = 0;
	view_x = 0;
	view_y = 0;
	scroll_position = 0u;
	table_offset = stage;
	stage_block_map_width = stage_block_map_widths[table_offset];
	table_offset = (uint16_t)(table_offset * 2u);
	camera_origin = (uint16_t)stage_camera_origins_x[table_offset / 2u];
	camera_origin_x = camera_origin;
	camera_x += (uint32_t)camera_origin << 16;
	camera_origin = (uint16_t)stage_camera_origins_y[table_offset / 2u];
	camera_origin_y = camera_origin;
	camera_y += (uint32_t)camera_origin << 16;
	view_offset_y = stage_view_offsets_y[table_offset / 2u];
	view_offset_x = stage_view_offsets_x[table_offset / 2u];
	scroll_end = stage_scroll_ends[table_offset / 2u];
	stage_number = stage;
	list = stage_section_speeds[stage_number] + stage_section_speeds_ADDRESS;
	scroll_speed = SECTION_SCROLL_SPEEDS(list)[0];
	section_speed_list = list + 1u;
	list = stage_section_follow_speeds[stage_number] + stage_section_follow_speeds_ADDRESS;
	follow_base_speed = SECTION_FOLLOW_SPEEDS(list)[0];
	follow_base_speed_list = list + 1u;
	section_end_list = stage_section_ends[stage_number] + stage_section_ends_ADDRESS;
	section_end_list += 2u;
	section_unused_cursor = stage_section_unused[stage_number];
	list = stage_section_limits_x[stage_number] + stage_section_limits_x_ADDRESS;
	list = (list & ~0xFFFFu) | (uint16_t)(list + 2u);
	section_limit_x = SECTION_CAMERA_LIMITS(list)[0];
	section_limit_x_list = list + 2u;
	list = stage_section_limits_y[stage_number] + stage_section_limits_y_ADDRESS;
	list = (list & ~0xFFFFu) | (uint16_t)(list + 2u);
	section_limit_y = SECTION_CAMERA_LIMITS(list)[0];
	section_limit_y_list = list + 2u;
	list = stage_scroll_modes[stage_number] + stage_scroll_modes_ADDRESS;
	scroll_mode = SCROLL_MODE_STEP(list)->mode;
	list += 2u;
	scroll_mode_timer = SCROLL_MODE_STEP(list - 0x2u)->duration;
	scroll_mode_duration = scroll_mode_timer;
	scroll_ramp_speed = 1u;
	scroll_mode_script = list + 2u;
	direction_y_list = stage_section_directions_y[stage_number] + stage_section_directions_y_ADDRESS;
	list = stage_section_directions_x[stage_number] + stage_section_directions_x_ADDRESS;
	*p_direction_x = SECTION_DIRECTIONS(list)[0];
	*p_direction_y = SECTION_DIRECTIONS(direction_y_list)[0];
	section_direction_x = *p_direction_x;
	section_direction_y = *p_direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	unused_scroll_flag = 0u;
	unused_scroll_offset = -0xA;
	nbg0_scroll_x_int = (int32_t)camera_x >> 16;
	nbg0_scroll_y_int = (int32_t)camera_y >> 16;
	nbg2_scroll_x = nbg0_scroll_x_int;
	nbg1_scroll_x_int = nbg0_scroll_x_int;
	nbg2_scroll_y = nbg0_scroll_y_int;
	nbg1_scroll_y_int = nbg0_scroll_y_int;
	camera_shake_phase = 0u;
	camera_shake_step = 0u;
	camera_shake_length = 0u;
	camera_shake_active = 0u;
	camera_shake_frame = 0u;
	camera_shake_frame_pad = 0u;
	back_edge_frames = 0u;
	nearby_floor_blocks_locked = 0u;
	scroll_skip_open = 0u;
	*p_stage_offset = table_offset;
}

/* A stage frame's tile layers: animate_stage_palette, then (until scroll_ended) clamp_camera_pixels,
   NBG0's scroll and wanted_tile_sets from the 2 x 2 blocks of stage_block_maps around the camera, a set
   a byte; when they differ from loaded_tile_sets reload_changed_tile_set (a reload past its second slot
   keeps its sets), else tile_set_slot 0. */
void update_stage_tile_layers(uint32_t *p_shifted_tile_set, uint32_t *p_copy_entry, uint32_t *p_columns_left, uint32_t *p_palette_count, uint32_t *p_stage_number, uint32_t *p_row_skip, uint32_t *p_cycle, uint32_t *p_block_cursor, uint32_t *p_dest)
{
	animate_stage_palette(p_shifted_tile_set, p_copy_entry, p_palette_count, p_cycle, p_block_cursor, p_dest);
	if (scroll_ended < 0)
	{
		return;
	}
	clamp_camera_pixels();
	set_nbg0_scroll_from_camera(*p_copy_entry);
	*p_stage_number = stage;
	*p_block_cursor = stage_block_maps[*p_stage_number] + stage_block_maps_ADDRESS + tile_block_row + tile_block_column;
	*p_columns_left = 2u;
	*p_copy_entry = 2u;
	*p_palette_count = 0u;
	wanted_tile_sets = 0u;
	for (;;)
	{
		*p_shifted_tile_set = rotate_by(BLOCK_TILE_SETS(*p_block_cursor)[0], (uint8_t)*p_palette_count, 32);
		wanted_tile_sets += *p_shifted_tile_set;
		*p_palette_count = (uint16_t)(*p_palette_count + 8u);
		(*p_columns_left)--;
		(*p_block_cursor)++;
		if (*p_columns_left == 0u)
		{
			*p_row_skip = stage_block_map_width;
			*p_row_skip = (uint16_t)(*p_row_skip - 2u);
			*p_block_cursor += *p_row_skip;
			*p_columns_left = 2u;
			(*p_copy_entry)--;
			if (*p_copy_entry != 0u)
				continue;
			break;
		}
	}
	if (wanted_tile_sets != loaded_tile_sets)
	{
		if (tile_sets_in_progress != wanted_tile_sets && (int8_t)(uint8_t)tile_set_slot > 1)
			wanted_tile_sets = tile_sets_in_progress;
		reload_changed_tile_set(p_shifted_tile_set, p_copy_entry, p_palette_count, p_stage_number, p_row_skip, p_block_cursor, p_dest);
		clamp_camera_pixels();
		return;
	}
	tile_set_slot = 0u;
	clamp_camera_pixels();
}

/* Checks one slot of wanted_tile_sets a frame (tile_set_slot, from 0 again when the wanted sets
   changed): a set not among the four of loaded_tile_sets is loaded (load_tile_set). Meanwhile
   tile_sets_in_progress keeps the wanted sets; after the fourth slot loaded_tile_sets takes them and
   tile_set_slot is 0 again. */
void reload_changed_tile_set(uint32_t *p_shifted_tile_set, uint32_t *p_slots_left, uint32_t *p_set_shift, uint32_t *p_slot_tile_set, uint32_t *p_loaded_shift, uint32_t *p_script, uint32_t *p_dest)
{
	uint8_t shift_per_slot = 0;
	uint32_t wanted_tile_set = 0;

	*p_slots_left = 4u;
	*p_set_shift = 0u;
	shift_per_slot = 0xF8u;
	*p_loaded_shift = 0u;
	if (tile_sets_in_progress != wanted_tile_sets)
		tile_set_slot = 0u;
	wanted_tile_set = shift_by(wanted_tile_sets, (uint8_t)((uint32_t)shift_per_slot * tile_set_slot), 32) & 0xFFu;
	tile_set_slot++;
	for (;;)
	{
		*p_slot_tile_set = shift_by(loaded_tile_sets, (uint8_t)*p_loaded_shift, 32) & 0xFFu;
		if (wanted_tile_set == *p_slot_tile_set)
			break;
		*p_loaded_shift = (uint16_t)(*p_loaded_shift - 8u);
		(*p_slots_left)--;
		if (*p_slots_left == 0u)
		{
			load_tile_set(wanted_tile_set, p_shifted_tile_set, p_slot_tile_set, p_script, p_dest);
			return;
		}
	}
	if ((int8_t)(uint8_t)tile_set_slot < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
}

/* Loads tile set tile_set of the stage: every vram script of its list in stage_tile_sets (up to a
   negative entry) through run_vram_script, list and scripts moved for the stage (get_tile_set_list_bank,
   _script_bank); then tile_sets_in_progress = wanted_tile_sets, which loaded_tile_sets takes after the
   fourth slot (tile_set_slot 0 again). */
void load_tile_set(uint32_t tile_set, uint32_t *p_count, uint32_t *p_stage_offset, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script_list = 0;

	script_list = stage_tile_sets[stage];
	get_tile_set_list(tile_set, p_stage_offset, &script_list);
	script_list = get_tile_set_list_bank(script_list);
	for (;;)
	{
		*p_script = TILE_SET_VRAM_SCRIPTS(script_list)[0];
		script_list += 4u;
		if ((int32_t)*p_script < 0)
			break;
		*p_script = get_tile_set_script_bank(*p_script);
		run_vram_script(p_count, p_script, p_dest);
	}
	if ((int8_t)(uint8_t)tile_set_slot < 4)
	{
		tile_sets_in_progress = wanted_tile_sets;
		return;
	}
	loaded_tile_sets = wanted_tile_sets;
	tile_set_slot = 0u;
	tile_sets_in_progress = wanted_tile_sets;
}

/* Hands back script + 0x100000 for stages 3-0x10 and 0x13, unchanged for stages 0-2, 0x11 and 0x12. */
uint32_t get_tile_set_script_bank(uint32_t script)
{
	uint32_t stage_number = 0;

	stage_number = (uint8_t)stage;
	switch ((uint16_t)(stage_number << 1))
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
		return script;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
	case 0x26u:
		script += 0x100000u;
		return script;
	default:
		invalid_jump(0x15E85u + (uint16_t)(stage_number << 1));
	}
}

/* Hands back script_list + 0x100000 for stages 3-0x10 and 0x13, unchanged for stages 0-2, 0x11 and 0x12. */
uint32_t get_tile_set_list_bank(uint32_t script_list)
{
	uint32_t stage_number = 0;

	stage_number = (uint8_t)stage;
	switch ((uint16_t)(stage_number << 1))
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
		return script_list;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
	case 0x26u:
		script_list += 0x100000u;
		return script_list;
	default:
		invalid_jump(0x15EC0u + (uint16_t)(stage_number << 1));
	}
}

/* Hands back in script_list the vram script list of tile set tile_set from the stage's table at
   script_list: 16-bit offsets from blank_tile_set_lists for stages 0-2 and 0x11-0x13, 32-bit pointers
   for the others; stage_offset gets stage * 2. */
void get_tile_set_list(uint32_t tile_set, uint32_t *p_stage_offset, uint32_t *p_script_list)
{
	uint32_t script_list = *p_script_list;
	uint32_t stage_offset = 0;

	stage_offset = (uint8_t)stage;
	stage_offset = (uint16_t)(stage_offset << 1);
	switch (stage_offset)
	{
	case 0x0u:
	case 0x2u:
	case 0x4u:
	case 0x22u:
	case 0x24u:
	case 0x26u:
		script_list = mem_read16((script_list & ~0xFFFFu) | (uint16_t)(script_list + (tile_set << 1))) + blank_tile_set_lists_ADDRESS;
		break;
	case 0x6u:
	case 0x8u:
	case 0xAu:
	case 0xCu:
	case 0xEu:
	case 0x10u:
	case 0x12u:
	case 0x14u:
	case 0x16u:
	case 0x18u:
	case 0x1Au:
	case 0x1Cu:
	case 0x1Eu:
	case 0x20u:
		script_list = mem_read32(script_list + (tile_set << 2));
		break;
	default:
		invalid_jump(0x15EFAu + stage_offset);
	}
	*p_stage_offset = stage_offset;
	*p_script_list = script_list;
}

/* Keeps camera_x_pixels within 0-0x1FFF and camera_y_pixels within 0-0xFFF. */
void clamp_camera_pixels(void)
{
	if ((int16_t)camera_x_pixels <= 0)
		camera_x_pixels = 0u;
	if ((int16_t)camera_x_pixels >= 0x2000)
		camera_x_pixels = 0x1FFFu;
	if ((int16_t)camera_y_pixels <= 0)
		camera_y_pixels = 0u;
	if ((int16_t)camera_y_pixels >= 0x1000)
		camera_y_pixels = 0xFFFu;
}

/* The scroll's frame by scroll_mode (update_scroll_at_end from scroll_end on): 0 at scroll_speed, 1
   stopped, 2 paused, then the next mode of scroll_mode_script, 3 slowing, 4 speeding up, 5/6
   nearby_floor_blocks_locked set/cleared; 7 and 8 follow_base_speed plus the speed of the fastest player
   pushing a leading screen edge facing the scroll. */
void update_scroll(uint32_t *p_position, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_second_player_object, uint32_t *p_third_player_object, uint32_t *p_end_script, uint32_t *p_dest)
{
	uint16_t target_heading = 0;
	uint32_t check_result = 0, count = 0, mode_script_cursor = 0, object = 0, object_2 = 0, object_3 = 0, player_2 = 0, player_speed = 0;

	if (scroll_ended < 0)
	{
		update_scroll_at_end(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	}
	*p_position = scroll_position;
	if ((int32_t)scroll_end <= (int32_t)*p_position)
	{
		update_scroll_at_end(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	}
	switch (scroll_mode)
	{
	case 0:
		scroll_at_scroll_speed(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 1:
		scroll_speed = 0u;
		*p_speed &= ~0xFFFFu;
		set_camera_velocity(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 2:
		scroll_mode_timer--;
		if (scroll_mode_timer != 0u)
		{
			update_camera_view();
			*p_speed = 0u;
			update_camera_view();
			set_camera_velocity(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			return;
		}
		if (scroll_mode != 2u)
		{
			scroll_mode = 0u;
			scroll_ramp_speed = 1u;
			update_camera_view();
			set_camera_velocity(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			return;
		}
		mode_script_cursor = scroll_mode_script;
		scroll_mode = SCROLL_MODE_STEP(mode_script_cursor)->mode;
		mode_script_cursor += 2u;
		scroll_mode_timer = SCROLL_MODE_STEP(mode_script_cursor - 0x2u)->duration;
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 3:
		scroll_slowing_down(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 4:
		scroll_speeding_up(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 5:
		nearby_floor_blocks_locked = 1u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = SCROLL_MODE_STEP(mode_script_cursor)->mode;
		mode_script_cursor += 2u;
		scroll_mode_timer = SCROLL_MODE_STEP(mode_script_cursor - 0x2u)->duration;
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 6:
		nearby_floor_blocks_locked = 0u;
		mode_script_cursor = scroll_mode_script;
		scroll_mode = SCROLL_MODE_STEP(mode_script_cursor)->mode;
		mode_script_cursor += 2u;
		scroll_mode_timer = SCROLL_MODE_STEP(mode_script_cursor - 0x2u)->duration;
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode_script = mode_script_cursor + 2u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		return;
	case 7:
	case 8:
		*p_end_script = PLAYER_BLOCK(1);
		player_2 = PLAYER_BLOCK(2);
		r14 = PLAYER_BLOCK(3);
		object = LINKED(WORK_RAM, player_1_object);
		object_2 = LINKED(WORK_RAM, player_2_object);
		object_3 = LINKED(WORK_RAM, player_3_object);
		players_pushing_edge = 0u;
		players_facing_scroll = 0u;
		players_in_play_bits = 0u;
		player_past_leading_edge = 0u;
		if ((OBJECT(object)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 1u;
		if ((OBJECT(object_2)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 2u;
		if ((OBJECT(object_3)->flags & 0x10000400u) == 0u)
			players_in_play_bits |= 4u;
		check_result = 0u;
		set_screen_push_edges(check_result, &count, p_dest);
		is_direction_in_quadrant(count, &check_result, p_dest);
		if ((uint8_t)check_result == 0u)
		{
			if (player_at_trailing_edge == 0u || (int16_t)((uint32_t)SECTION_END_POSITIONS(section_end_list)[0] - scroll_position) > 0x180)
			{
				*p_speed = 0u;
				*p_speed &= 0xFFu;
				*p_speed = (uint16_t)(*p_speed + follow_base_speed);
				advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				return;
			}
			if ((int8_t)back_edge_frames >= 0x40)
			{
				follow_base_speed = 6u;
				*p_speed = 0u;
				*p_speed &= 0xFFu;
				*p_speed = (uint16_t)(*p_speed + follow_base_speed);
				advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				return;
			}
			back_edge_frames++;
			*p_speed = 0u;
			*p_speed &= 0xFFu;
			*p_speed = (uint16_t)(*p_speed + follow_base_speed);
			advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			return;
		}
		if ((players_in_play_bits & 1u) != 0u)
		{
			target_heading = PLAYER(*p_end_script)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)*p_dest);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 1u;
		}
		if ((players_in_play_bits & 2u) != 0u)
		{
			target_heading = PLAYER(player_2)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)*p_dest);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 2u;
		}
		if ((players_in_play_bits & 4u) != 0u)
		{
			target_heading = PLAYER(r14)->target_heading;
			check_result = is_player_facing_scroll(check_result, target_heading, (uint16_t)*p_dest);
			if ((uint8_t)check_result != 0u)
				players_facing_scroll |= 4u;
		}
		player_at_trailing_edge = 0u;
		if ((OBJECT(object)->flags & 0x400u) == 0u)
		{
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 1u;
		}
		if ((OBJECT(object_2)->flags & 0x400u) == 0u)
		{
			object = object_2;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 2u;
		}
		if ((OBJECT(object_3)->flags & 0x400u) == 0u)
		{
			object = object_3;
			check_result = is_player_pushing_edge(check_result, object);
			if ((uint8_t)check_result != 0u)
				players_pushing_edge |= 4u;
		}
		if (players_pushing_edge == 0u || (int8_t)players_pushing_edge > 7 || player_at_trailing_edge != 0u)
		{
			if (player_at_trailing_edge == 0u || (int16_t)((uint32_t)SECTION_END_POSITIONS(section_end_list)[0] - scroll_position) > 0x180)
			{
				*p_speed = 0u;
				*p_speed &= 0xFFu;
				*p_speed = (uint16_t)(*p_speed + follow_base_speed);
				advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				return;
			}
			if ((int8_t)back_edge_frames >= 0x40)
			{
				follow_base_speed = 6u;
				*p_speed = 0u;
				*p_speed &= 0xFFu;
				*p_speed = (uint16_t)(*p_speed + follow_base_speed);
				advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				return;
			}
			back_edge_frames++;
			*p_speed = 0u;
			*p_speed &= 0xFFu;
			*p_speed = (uint16_t)(*p_speed + follow_base_speed);
			advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			return;
		}
		players_pushing_edge &= players_facing_scroll;
		players_pushing_edge &= players_in_play_bits;
		object = LINKED(object, PLAYER(*p_end_script)->object);
		switch (players_pushing_edge)
		{
		case 0:
		case 8:
			if (players_facing_scroll != 0u || player_past_leading_edge == 0u)
			{
				*p_speed = 5u;
				*p_speed = (uint16_t)(*p_speed + follow_base_speed);
				advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
				*p_second_player_object = object_2;
				*p_third_player_object = object_3;
				return;
			}
			*p_speed = 5u;
			*p_speed = (uint16_t)(*p_speed + follow_base_speed);
			advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			return;
		case 1:
		case 9:
			break;
		case 2:
		case 10:
			object = LINKED(object, PLAYER(player_2)->object);
			break;
		case 3:
			if (OBJECT(object)->speed <= OBJECT(object_2)->speed)
				object = object_2;
			break;
		case 4:
			object = LINKED(object, PLAYER(r14)->object);
			break;
		case 5:
			if (OBJECT(object)->speed <= OBJECT(object_3)->speed)
				object = object_3;
			break;
		case 6:
			object = object_3;
			if (OBJECT(object)->speed <= OBJECT(object_2)->speed)
				object = object_2;
			break;
		case 7:
			if (OBJECT(object)->speed <= OBJECT(object_2)->speed)
				object = object_2;
			if (OBJECT(object)->speed <= OBJECT(object_3)->speed)
				object = object_3;
			break;
		default:
			invalid_jump(0x16151u + players_pushing_edge * 3u);
		}
		player_speed = (int32_t)OBJECT(object)->speed >> 16;
		if ((int32_t)player_speed > 0x500 || (int32_t)player_speed < 0)
		{
			*p_speed = 5u;
			*p_speed = (uint16_t)(*p_speed + follow_base_speed);
			advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			return;
		}
		*p_speed = ((uint16_t)player_speed * 0x400u) >> 16;
		if (player_past_leading_edge == 0u)
		{
			*p_speed &= 0xFFu;
			*p_speed = (uint16_t)(*p_speed + follow_base_speed);
			advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
			*p_second_player_object = object_2;
			*p_third_player_object = object_3;
			return;
		}
		*p_speed = (*p_speed & 0xFFu) + 5u;
		*p_speed &= 0xFFu;
		*p_speed = (uint16_t)(*p_speed + follow_base_speed);
		advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		*p_second_player_object = object_2;
		*p_third_player_object = object_3;
		return;
		*p_speed = (uint16_t)(*p_speed + follow_base_speed);
		advance_scroll(p_position, p_speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		*p_second_player_object = object_2;
		*p_third_player_object = object_3;
		return;
	default:
		invalid_jump(0x15FA9u + scroll_mode * 3u);
	}
}

/* Hands back 1 (pushing's low byte) when the player object object is at a leading screen edge for
   push_edge_case (the push_edge_* lines and, in cases 2, 4, 6 and 8, a diagonal); at a trailing edge it
   sets player_at_trailing_edge and hands back 0. In stage 3 between scroll_position 0x87A9 and 0x96F1
   case 8 counts only the top edge. */
uint32_t is_player_pushing_edge(uint32_t pushing, uint32_t object)
{
	uint16_t edge_offset = 0;
	uint32_t current_position = 0;

	switch (push_edge_case)
	{
	case 0:
	case 9:
		pushing &= ~0xFFu;
		return pushing;
	case 1:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 2:
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xF0u;
			pushing = is_player_at_diagonal_edge_f0(pushing, object, edge_offset);
			if ((uint8_t)pushing == 0u)
			{
				pushing = is_player_at_right_edge(pushing, object);
				if ((uint8_t)pushing == 0u)
				{
					pushing = is_player_at_bottom_edge(pushing, object);
					if ((uint8_t)pushing == 0u)
					{
						pushing &= ~0xFFu;
						return pushing;
					}
				}
			}
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 3:
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 4:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0x60u;
			pushing = is_player_at_diagonal_edge_60(pushing, object, edge_offset);
			if ((uint8_t)pushing == 0u)
			{
				pushing = is_player_at_left_edge(pushing, object);
				if ((uint8_t)pushing == 0u)
				{
					pushing = is_player_at_bottom_edge(pushing, object);
					if ((uint8_t)pushing == 0u)
					{
						pushing &= ~0xFFu;
						return pushing;
					}
				}
			}
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 5:
		pushing = is_player_at_right_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_left_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 6:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			edge_offset = 0xC0u;
			pushing = is_player_at_diagonal_edge_c0(pushing, object, edge_offset);
			if ((uint8_t)pushing == 0u)
			{
				pushing = is_player_at_left_edge(pushing, object);
				if ((uint8_t)pushing == 0u)
				{
					pushing = is_player_at_top_edge(pushing, object);
					if ((uint8_t)pushing == 0u)
					{
						pushing &= ~0xFFu;
						return pushing;
					}
				}
			}
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	case 7:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			player_at_trailing_edge = 1u;
			pushing &= ~0xFFu;
			return pushing;
		}
		pushing = is_player_at_top_edge(pushing, object);
		if ((uint8_t)pushing != 0u)
		{
			pushing = (pushing & ~0xFFu) | 1u;
			return pushing;
		}
		pushing &= ~0xFFu;
		return pushing;
	case 8:
		pushing = is_player_at_bottom_edge(pushing, object);
		if ((uint8_t)pushing == 0u)
		{
			pushing = is_player_at_left_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				player_at_trailing_edge = 1u;
				pushing &= ~0xFFu;
				return pushing;
			}
			if (stage == 3u)
			{
				current_position = scroll_position;
				if ((int32_t)current_position >= 0x87A9 && (int32_t)current_position <= 0x96F1)
				{
					pushing = is_player_at_top_edge(pushing, object);
					if ((uint8_t)pushing != 0u)
					{
						pushing = (pushing & ~0xFFu) | 1u;
						return pushing;
					}
					pushing &= ~0xFFu;
					return pushing;
				}
			}
			edge_offset = 0x30u;
			pushing = is_player_at_diagonal_edge_30(pushing, object, edge_offset);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_right_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing = is_player_at_top_edge(pushing, object);
			if ((uint8_t)pushing != 0u)
			{
				pushing = (pushing & ~0xFFu) | 1u;
				return pushing;
			}
			pushing &= ~0xFFu;
			return pushing;
		}
		player_at_trailing_edge = 1u;
		pushing &= ~0xFFu;
		return pushing;
	default:
		invalid_jump(0x16274u + push_edge_case * 3u);
	}
}

/* Hands back 1 (at_edge's low byte) when object is on or above the line screen_y = screen_x / 2 +
   edge_offset; a second pass at edge_offset - 0xA sets player_past_leading_edge, which it clears first.
   Its inner test, screen_x against 2 * (screen_y - edge_offset), is the same line and never passes. */
uint32_t is_player_at_diagonal_edge_30(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if ((int16_t)((uint32_t)(screen_y - edge_offset) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object is on or below the line screen_y = edge_offset -
   screen_x / 2. It clears player_past_leading_edge, and its second pass, at edge_offset - 0xA, takes the
   line 0xA back, so both count from 0xA before the line; its inner test is the same line and never
   passes. */
uint32_t is_player_at_diagonal_edge_f0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if ((int16_t)((0u - (screen_y - edge_offset)) << 1) <= OBJECT(object)->screen_x)
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object is on or above the line screen_y = edge_offset -
   screen_x / 2; a second pass at edge_offset - 0xA sets player_past_leading_edge, which it clears first.
   Its inner test is the same line and never passes. */
uint32_t is_player_at_diagonal_edge_c0(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)(0u - ((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset) < (int16_t)screen_y)
		{
			if (OBJECT(object)->screen_x <= (int16_t)((0u - (screen_y - edge_offset)) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object is on or below the line screen_y = screen_x / 2 +
   edge_offset. It clears player_past_leading_edge, and its second pass, at edge_offset - 0xA, takes the
   line 0xA back, so both count from 0xA before the line; its inner test is the same line and never
   passes. */
uint32_t is_player_at_diagonal_edge_60(uint32_t at_edge, uint32_t object, uint16_t edge_offset)
{
	uint16_t screen_y = 0;
	uint32_t passes_left = 0;

	passes_left = 2u;
	at_edge &= ~0xFFFFu;
	player_past_leading_edge = 0u;
	do
	{
		screen_y = (uint16_t)OBJECT(object)->screen_y;
		if ((int16_t)screen_y < (int16_t)(((int16_t)OBJECT(object)->screen_x >> 1) + edge_offset))
		{
			if (OBJECT(object)->screen_x <= (int16_t)((uint32_t)(screen_y - edge_offset) << 1))
			{
				at_edge = (at_edge & ~0xFFu) | 1u;
				if ((uint16_t)passes_left == 1u)
					player_past_leading_edge = 1u;
			}
		}
		else
		{
			at_edge = (at_edge & ~0xFFu) | 1u;
			if ((uint16_t)passes_left == 1u)
				player_past_leading_edge = 1u;
		}
		edge_offset -= 0xAu;
		passes_left--;
	} while (passes_left != 0u);
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object's screen_x is at or right of push_edge_right, else 0;
   0xA past it also sets player_past_leading_edge. */
uint32_t is_player_at_right_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_x < (int16_t)push_edge_right)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_right;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_x)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object's screen_x is at or left of push_edge_left, else 0; 0xA
   past it also sets player_past_leading_edge. */
uint32_t is_player_at_left_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_left < OBJECT(object)->screen_x)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_left;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_x <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object's screen_y is at or above push_edge_top, else 0; 0xA
   past it also sets player_past_leading_edge. */
uint32_t is_player_at_top_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if ((int16_t)push_edge_top < OBJECT(object)->screen_y)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_top;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit - 0xAu);
	if (OBJECT(object)->screen_y <= (int16_t)leading_limit)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

/* Hands back 1 (at_edge's low byte) when object's screen_y is at or below push_edge_bottom, else 0; 0xA
   past it also sets player_past_leading_edge. */
uint32_t is_player_at_bottom_edge(uint32_t at_edge, uint32_t object)
{
	uint32_t leading_limit = 0;

	if (OBJECT(object)->screen_y < (int16_t)push_edge_bottom)
	{
		at_edge &= ~0xFFu;
		return at_edge;
	}
	leading_limit = (at_edge & ~0xFFFFu) | push_edge_bottom;
	leading_limit = (leading_limit & ~0xFFFFu) | (uint16_t)(leading_limit + 0xAu);
	if ((int16_t)leading_limit <= OBJECT(object)->screen_y)
		player_past_leading_edge = 1u;
	at_edge = (leading_limit & ~0xFFu) | 1u;
	return at_edge;
}

/* From section_direction_x/_y's signs: the quadrant (sine_entry at its quarter of sine_table, count
   0x100 for is_direction_in_quadrant) and the screen edges players push in it (push_edge_left, _right,
   _top, _bottom; 0xFFFF none), with push_edge_case 1, 2, 4, 6, 7 or 8 (1 and 7 for straight +x and -y). */
void set_screen_push_edges(uint32_t quadrant_bits, uint32_t *p_count, uint32_t *p_sine_entry)
{
	*p_sine_entry = sine_table_ADDRESS;
	if ((int16_t)(uint16_t)section_direction_y >= 0)
	{
		if ((int16_t)(uint16_t)section_direction_x < 0)
		{
			*p_sine_entry += 0x200u;
			quadrant_bits |= 2u;
		}
	}
	else
	{
		if ((int16_t)(uint16_t)section_direction_x >= 0)
		{
			*p_sine_entry += 0x200u;
			quadrant_bits |= 8u;
		}
		*p_sine_entry += 0x200u;
		quadrant_bits |= 4u;
		*p_sine_entry += 0x200u;
		quadrant_bits |= 2u;
	}
	*p_count = 0x100u;
	push_edge_top = 0xFFFFu;
	push_edge_bottom = 0xFFFFu;
	push_edge_right = 0xFFFFu;
	push_edge_left = 0xFFFFu;
	switch (quadrant_bits | 1u)
	{
	case 0:
	case 2:
	case 4:
	case 5:
	case 6:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 16:
		return;
	case 1:
		push_edge_case = 0u;
		if ((uint16_t)section_direction_y != 0u)
		{
			push_edge_bottom = 0x88u;
			push_edge_top = 0x20u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_right = 0xB8u;
		push_edge_left = 0x40u;
		return;
	case 3:
		push_edge_case = 2u;
		if ((uint16_t)section_direction_x != 0u)
		{
			push_edge_left = 0x88u;
			push_edge_right = 0x100u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_bottom = 0x88u;
		push_edge_top = 0x20u;
		return;
	case 7:
		push_edge_case = 4u;
		if ((uint16_t)section_direction_y != 0u)
		{
			push_edge_top = 0x58u;
			push_edge_bottom = 0xC0u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_left = 0x88u;
		push_edge_right = 0x100u;
		return;
	case 15:
		push_edge_case = 6u;
		if ((uint16_t)section_direction_x != 0u)
		{
			push_edge_right = 0xB8u;
			push_edge_left = 0x40u;
			push_edge_case++;
		}
		push_edge_case++;
		push_edge_top = 0x58u;
		push_edge_bottom = 0xC0u;
		return;
	default:
		invalid_jump(0x16726u + (quadrant_bits | 1u) * 3u);
	}
}

/* Hands back 1 (facing's low byte) when heading, a target_heading, is within a quarter turn either side
   of the middle of the scroll's quadrant (sine_entry's quarter of sine_table: 0x180, 0x280, 0x380 or
   0x80), else 0. */
uint32_t is_player_facing_scroll(uint32_t facing, uint16_t heading, uint16_t sine_entry)
{
	uint16_t sine_offset = 0;

	heading = (uint16_t)((uint32_t)heading << 1);
	sine_offset = (uint16_t)(sine_entry - sine_table_ADDRESS);
	if ((int16_t)sine_offset < 0x200)
	{
		if ((int16_t)heading < 0x100 || (int16_t)heading > 0x500)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x400)
	{
		if ((int16_t)heading < 0x300 || (int16_t)heading > 0x700)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)sine_offset < 0x600)
	{
		if ((int16_t)heading < 0x500)
			heading += 0x800u;
		if ((int16_t)heading > 0x900)
		{
			facing &= ~0xFFu;
			return facing;
		}
		facing = (facing & ~0xFFu) | 1u;
		return facing;
	}
	if ((int16_t)heading < 0x700)
		heading += 0x800u;
	if ((int16_t)heading > 0xB00)
	{
		facing &= ~0xFFu;
		return facing;
	}
	facing = (facing & ~0xFFu) | 1u;
	return facing;
}

/* Looks through count entries of sine_table from sine_entry for the pair section_direction_y lies
   between: found (1 in its low byte) with sine_entry left there, or 0 after count entries; a direction
   straight along an axis (0x4000 or 0xC000, the other 0) is found at once. */
void is_direction_in_quadrant(uint32_t count, uint32_t *p_found, uint32_t *p_sine_entry)
{
	uint8_t entry_at_least = 0, found = 0, next_at_least = 0;
	uint16_t direction_y = 0, next_entry = 0;

	for (;;)
	{
		if ((uint16_t)section_direction_y != 0u)
		{
			if ((uint16_t)section_direction_x == 0u && ((uint16_t)section_direction_y == 0x4000u || (uint16_t)section_direction_y == 0xC000u))
			{
				found = 1u;
				*p_found = (*p_found & 0xFFFFFF00u) | found;
				return;
			}
		}
		else if ((uint16_t)section_direction_x == 0x4000u || (uint16_t)section_direction_x == 0xC000u)
			break;
		direction_y = (uint16_t)section_direction_y;
		next_entry = (uint16_t)SCROLL_QUADRANT_SINES(*p_sine_entry)[1];
		if (SCROLL_QUADRANT_SINES(*p_sine_entry)[0] < (int16_t)direction_y)
			entry_at_least = 0u;
		else
			entry_at_least = 1u;
		if ((int16_t)next_entry < (int16_t)direction_y)
			next_at_least = 0u;
		else
			next_at_least = 1u;
		if (next_at_least != entry_at_least)
		{
			found = 1u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			return;
		}
		*p_sine_entry += 2u;
		count--;
		if (count == 0u)
		{
			found = 0u;
			*p_found = (*p_found & 0xFFFFFF00u) | found;
			return;
		}
	}
	found = 1u;
	*p_found = (*p_found & 0xFFFFFF00u) | found;
}

/* Scroll mode 3: scroll_speed - 1 each time scroll_mode_timer runs out (reloaded from
   scroll_mode_duration), down to 1; advances at scroll_speed (scroll_at_scroll_speed). */
void scroll_slowing_down(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	scroll_mode_timer--;
	if (scroll_mode_timer != 0u || (int16_t)scroll_speed < 2)
	{
		scroll_at_scroll_speed(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
		return;
	}
	scroll_speed--;
	scroll_mode_timer = scroll_mode_duration;
	scroll_at_scroll_speed(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
}

/* Scroll mode 4: advances at scroll_ramp_speed, + 1 each time scroll_mode_timer runs out (reloaded from
   scroll_mode_duration); once it has reached scroll_speed, scroll mode 0 and scroll_ramp_speed 1, and
   that frame only set_camera_velocity with the speed its caller passed. */
void scroll_speeding_up(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	scroll_mode_timer--;
	if (scroll_mode_timer != 0u)
	{
		*p_speed = scroll_ramp_speed;
		advance_scroll_unless_ended(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
		return;
	}
	if (scroll_speed_low <= (int8_t)scroll_ramp_speed)
	{
		scroll_mode = 0u;
		scroll_ramp_speed = 1u;
		set_camera_velocity(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
		return;
	}
	scroll_ramp_speed++;
	scroll_mode_timer = scroll_mode_duration;
	*p_speed = scroll_ramp_speed;
	advance_scroll_unless_ended(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
}

/* Scroll mode 0: advance_scroll_unless_ended at scroll_speed. */
void scroll_at_scroll_speed(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	*p_speed = scroll_speed;
	advance_scroll_unless_ended(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
}

/* advance_scroll, unless scroll_ended is set. */
void advance_scroll_unless_ended(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	if (scroll_ended < 0)
		return;
	advance_scroll(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
}

/* move_camera_along_path by speed and update_camera_view; once scroll_position reaches the section's end
   the next section starts: scroll_speed, follow_base_speed, the x and y limits, the next (mode,
   duration) of scroll_mode_script, the direction and the end, back_edge_frames 0. Then
   set_camera_velocity. */
void advance_scroll(uint32_t *p_other_list, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t direction_y_list = 0, limit_x_cursor = 0, limit_y_cursor = 0, list = 0, mode_script_cursor = 0;
	uint16_t direction_y = 0;

	*p_next_scroll_position = move_camera_along_path(*p_speed);
	update_camera_view();
	if ((int32_t)scroll_position < (int32_t)SECTION_END_POSITIONS(section_end_list)[0])
	{
		set_camera_velocity(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
		return;
	}
	back_edge_frames = 0u;
	list = section_speed_list;
	scroll_speed = SECTION_SCROLL_SPEEDS(list)[0];
	section_speed_list = list + 1u;
	list = follow_base_speed_list;
	follow_base_speed = SECTION_FOLLOW_SPEEDS(list)[0];
	follow_base_speed_list = list + 1u;
	limit_y_cursor = section_limit_y_list;
	section_limit_y = SECTION_CAMERA_LIMITS(limit_y_cursor)[0];
	section_limit_y_list = limit_y_cursor + 2u;
	limit_x_cursor = section_limit_x_list;
	section_limit_x = SECTION_CAMERA_LIMITS(limit_x_cursor)[0];
	section_limit_x_list = limit_x_cursor + 2u;
	mode_script_cursor = scroll_mode_script;
	scroll_mode = SCROLL_MODE_STEP(mode_script_cursor)->mode;
	mode_script_cursor += 2u;
	scroll_mode_timer = SCROLL_MODE_STEP(mode_script_cursor - 0x2u)->duration;
	scroll_mode_duration = scroll_mode_timer;
	scroll_mode_script = mode_script_cursor + 2u;
	list = section_direction_x_list;
	direction_y_list = section_direction_y_list;
	direction_y = SECTION_DIRECTIONS(direction_y_list)[0];
	section_direction_x = SECTION_DIRECTIONS(list)[0];
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_list + 2u;
	section_end_list += 2u;
	section_unused_cursor += 2u;
	set_camera_velocity(p_other_list, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
}

/* camera_velocity_x/_y = section_direction_x/_y * speed's low half; camera_shake_length and
   camera_shake_wave 0x200, update_camera_with_shake, then (unless scroll_ended)
   update_background_scroll. */
void set_camera_velocity(uint32_t *p_scroll_y, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t ignored = 0, velocity_y = 0;

	*p_speed_value = (uint16_t)*p_speed;
	velocity_y = section_direction_y * *p_speed_value;
	camera_velocity_x = section_direction_x * *p_speed_value;
	camera_velocity_y = velocity_y;
	camera_shake_length = 0x200u;
	camera_shake_wave = 0x200u;
	*p_scroll_y = update_camera_with_shake();
	if (scroll_ended < 0)
	{
		return;
	}
	update_background_scroll(&ignored, p_scroll_y, p_speed, p_speed_value, p_next_scroll_position, p_range, p_quadrant_cursor);
}

/* update_nbg1_scroll, update_nbg3_scroll and update_nbg2_scroll; then follow_base_speed is forced to
   0x10 once scroll_position reaches the first position of the stage's stage_fast_scroll_ranges list past
   the current section's start. */
void update_background_scroll(uint32_t *p_fast_scroll_start, uint32_t *p_scroll_y, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_range, uint32_t *p_quadrant_cursor)
{
	uint32_t section_start = 0;

	update_nbg1_scroll();
	update_nbg3_scroll(p_scroll_y, p_speed, p_speed_value, p_next_scroll_position, p_quadrant_cursor);
	*p_speed = update_nbg2_scroll(*p_speed);
	*p_fast_scroll_start = stage;
	*p_range = stage_fast_scroll_ranges[*p_fast_scroll_start] + stage_fast_scroll_ranges_ADDRESS;
	section_start = SECTION_END_POSITIONS(section_end_list)[-1];
	do
	{
		if (FAST_SCROLL_STARTS(*p_range)[0] == 0xFFFFu)
		{
			return;
		}
		*p_fast_scroll_start = FAST_SCROLL_STARTS(*p_range)[0];
		*p_range += 2u;
	} while ((int32_t)*p_fast_scroll_start <= (int32_t)section_start);
	*p_scroll_y = scroll_position;
	if ((int32_t)*p_scroll_y < (int32_t)*p_fast_scroll_start)
	{
		return;
	}
	follow_base_speed = 0x10u;
}

/* scroll_ended = -1; only stage 3 goes on, its end by stage_03_end_step: two end scripts and NBG0's
   pages, the camera climbing straight up, sped up to run at 0x25 for stage_03_end_timer (0x78) frames;
   then camera_y 1, a third script, a slowdown to a stop, all_players_path_request 0x23 and
   spawn_stage_03_end_pillar; the camera then stays still. */
void update_scroll_at_end(uint32_t *p_position, uint32_t *p_speed, uint32_t *p_speed_value, uint32_t *p_next_scroll_position, uint32_t *p_end_script, uint32_t *p_dest)
{
	uint32_t speed = *p_speed;
	uint16_t layer_scroll_x = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, script = 0;

	scroll_ended = -1;
	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 4:
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
	case 16:
	case 17:
	case 18:
		return;
	case 3:
		break;
	case 19:
		return;
	default:
		invalid_jump(0x16BE2u + stage * 3u);
	}
	if ((int8_t)stage_03_end_started <= 0)
	{
		stage_03_end_started++;
		script = stage_03_end_script_1_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		*p_end_script = stage_03_end_script_2_ADDRESS;
		run_vram_script(&ignored, p_end_script, p_dest);
		nbg0_pages = 0x2020202u;
		scroll_mode_timer = 0x3Cu;
		scroll_mode_duration = scroll_mode_timer;
		scroll_mode = 4u;
		scroll_speed = 0x19u;
		stage_03_end_step = 0u;
		stage_03_end_timer = 0x78;
		stage_03_end_frames = 0u;
	}
	switch (stage_03_end_step)
	{
	case 0:
	case 4:
		stage_03_end_frames++;
		scroll_speeding_up(&ignored_2, &speed, &ignored_3, p_next_scroll_position, p_end_script, p_dest);
		if (scroll_mode_low == 0u)
		{
			speed = (speed & ~0xFFFFu) | 0x25u;
			scroll_speed = 0x25u;
			stage_03_end_step++;
		}
		break;
	case 1:
		stage_03_end_frames++;
		speed = (speed & ~0xFFFFu) | scroll_speed;
		stage_03_end_timer--;
		if (stage_03_end_timer <= 0)
		{
			camera_y = 0x10000;
			*p_end_script = stage_03_end_script_3_ADDRESS;
			run_vram_script(&ignored, p_end_script, p_dest);
			nbg0_pages = 0x2020200u;
			stage_03_end_step++;
			scroll_mode_timer = 5u;
			scroll_mode_duration = scroll_mode_timer;
			scroll_mode = 4u;
			scroll_speed = 0x19u;
		}
		break;
	case 2:
		stage_03_end_frames++;
		scroll_slowing_down(p_position, &speed, p_speed_value, p_next_scroll_position, p_end_script, p_dest);
		if ((int16_t)scroll_speed <= 1)
		{
			scroll_speed = 0u;
			stage_03_end_step++;
			all_players_path_request = 0x23u;
			*p_dest = spawn_stage_03_end_pillar(*p_dest);
			*p_speed = speed;
			return;
		}
		break;
	case 3:
		camera_velocity_x = 0;
		camera_velocity_y = 0;
		return;
	default:
		invalid_jump(0x174D2u + stage_03_end_step * 3u);
	}
	section_limit_y = -1;
	section_direction_y = -0x4000;
	section_direction_x = 0;
	advance_camera_y(speed);
	update_camera_view();
	layer_scroll_x = camera_x_pixels;
	nbg0_scroll_y_int = camera_y_pixels;
	nbg0_scroll_x_int = layer_scroll_x;
	nbg0_scroll_x_int &= 0x3FFu;
	nbg0_scroll_y_int &= 0x1FFu;
	*p_speed_value = (uint16_t)speed;
	*p_position = section_direction_y * *p_speed_value;
	camera_velocity_x = section_direction_x * *p_speed_value;
	camera_velocity_y = *p_position;
	*p_speed = speed;
}

/* Unused: once per stage view (debug_sprite_spawned), an object with handler debug_sprite_mover_update
   (allocate_object_outside_loop). */
void spawn_debug_sprite_mover(uint32_t object)
{
	uint32_t free_count = 0;

	if (debug_sprite_spawned == 0u)
	{
		allocate_object_outside_loop(&free_count, &object);
		if ((uint8_t)free_count != 0u)
		{
			OBJECT(object)->handler = debug_sprite_mover_update_ADDRESS;
			debug_sprite_spawned++;
		}
	}
}

/* Unused: the debug sprite's handler: sprite 0x8DB8 (palette 0x130, layer 0x32) at debug_sprite_x,
   debug_sprite_y; held 1P jump and 1P start move it right and left, 2P jump and input bit 25 down and
   up, a pixel a frame; the two are shown in hex at text cells (3,22) and (8,22). */
void debug_sprite_mover_update(void)
{
	uint32_t attribute = 0, text_position = 0, value = 0;

	OBJECT(current_object)->screen_x = debug_sprite_x;
	OBJECT(current_object)->screen_y = debug_sprite_y;
	OBJECT(current_object)->sprite_layer = 0x32u;
	OBJECT(current_object)->sprite_frame = 0x8DB8u;
	OBJECT(current_object)->sprite_palette = 0x130u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	queue_sprite_in_layer();
	if ((input_ports_active & 0x10000u) != 0u)
		debug_sprite_x++;
	if ((input_ports_active & 0x10u) != 0u)
		debug_sprite_x--;
	if ((input_ports_active & 0x1000000u) != 0u)
		debug_sprite_y++;
	if ((input_ports_active & 0x2000000u) != 0u)
		debug_sprite_y--;
	value = debug_sprite_x;
	text_position = TEXT_CELL(3, 22);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
	value = debug_sprite_y;
	text_position = TEXT_CELL(8, 22);
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
}

/* init_stage_view for every stage but 0x11 (unused_scroll_flag set first, which init_stage_view clears);
   stage 0x11 only clears ending_scene_step. */
void start_stage_view(uint32_t *p_direction_y, uint32_t *p_direction_x, uint32_t *p_count, uint32_t *p_stage_offset)
{
	if (stage != 0x11u)
	{
		unused_scroll_flag = 1u;
		init_stage_view(p_direction_y, p_direction_x, p_count, p_stage_offset);
		return;
	}
	ending_scene_step = 0u;
}

/* Unused: calls run_map_screen with 0 and object. */
void call_run_map_screen(uint32_t object)
{
	run_map_screen(0u, object);
}

/* Stage 5's switch to stage 0's view by stage_05_switch_step: every layer blended, a fade to black,
   stage 0's tile layers (coast, horizon, sky, city, tower top) with the camera at 0xF80,0x670 and NBG1-3
   scrolled as stage 0's, a fade in from black; then scroll_position 0x5E5A, stage 0, blending off and
   stage_05_switch_step -1. */
void run_stage_05_switch(uint32_t speed, uint32_t *p_scroll_y, uint32_t *p_speed_value, uint32_t *p_dest)
{
	uint32_t camera_view_y = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, jump_offset = 0, script = 0;

	jump_offset = (uint8_t)stage_05_switch_step * 3u;
	switch ((uint8_t)stage_05_switch_step)
	{
	case 0:
	case 4:
		mixer_text_blend |= 0x4000u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		stage_05_switch_step++;
		return;
	case 1:
		fade_kind = 0;
		update_fade(jump_offset);
		if (fade_kind >= 0)
			return;
		stage_05_switch_step++;
		fade_step = 0u;
		return;
	case 2:
		mem_write8(video_layer_disable, 0x3Fu);
		script = stage_00_coast_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = stage_00_horizon_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = blue_sky_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = aerial_city_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = stage_00_tower_top_page_1_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = stage_00_tower_top_page_3_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = stage_00_tower_top_page_0_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		script = stage_00_tower_top_page_2_script_ADDRESS;
		run_vram_script(&ignored, &script, p_dest);
		nbg3_pages = 0x7070707u;
		camera_x = 0xF800000;
		camera_y = 0x6700000;
		camera_view_y = update_camera_view();
		*p_scroll_y = (camera_view_y & ~0xFFFFu) | camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = *p_scroll_y;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		stage = 0u;
		update_background_scroll(&ignored, p_scroll_y, &ignored_2, p_speed_value, &ignored_3, &ignored_4, p_dest);
		stage = 5u;
		mem_write8(video_layer_disable, mem_read8(video_layer_disable) & 0xE6u);
		stage_05_switch_step++;
		return;
	case 3:
		fade_kind = 2;
		update_fade(jump_offset);
		if (fade_kind >= 0)
			return;
		stage_05_switch_step = -1;
		scroll_position = 0x5E5Au;
		stage = 0u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		set_nbg3_page_5_flags(&ignored, p_dest);
		loaded_tile_sets = STAGE_05_SWITCHED_TILE_SETS;
		return;
	default:
		invalid_jump(0x16D16u + (uint8_t)stage_05_switch_step * 3u);
	}
}

/* One step of a fade of the mixer's colour offsets (color_offset_red, _blue, _green) by fade_kind: 0 to
   black (0x3F down to 0x20), 1 from white (0x1F down to 0), 2 from black (0x20 up to 0x3F), 3 to white
   (0 up to 0x1F), fade_speed a frame; fade_step 0 sets the start, and fade_kind is -1 the frame after
   the end. */
uint32_t update_fade(uint32_t delta)
{
	uint32_t jump_offset = 0;

	jump_offset = fade_step * 3u;
	switch (fade_step)
	{
	case 0:
	case 3:
		fade_kind &= 3u;
		switch ((jump_offset & ~0xFFu) | (uint8_t)fade_kind)
		{
		case 0:
		case 4:
			color_offset_red = 0x3Fu;
			color_offset_blue = 0x3Fu;
			color_offset_green = 0x3Fu;
			delta = (delta & ~0xFFFFu) | fade_speed;
			delta = (delta & ~0xFFFFu) | (uint16_t)(0u - delta);
			fade_delta = delta;
			break;
		case 1:
			color_offset_red = 0x1Fu;
			color_offset_blue = 0x1Fu;
			color_offset_green = 0x1Fu;
			delta = (delta & ~0xFFFFu) | fade_speed;
			delta = (delta & ~0xFFFFu) | (uint16_t)(0u - delta);
			fade_delta = delta;
			break;
		case 2:
			color_offset_red = 0x20u;
			color_offset_blue = 0x20u;
			color_offset_green = 0x20u;
			delta = (delta & ~0xFFFFu) | fade_speed;
			fade_delta = delta;
			break;
		case 3:
			color_offset_red = 0u;
			color_offset_blue = 0u;
			color_offset_green = 0u;
			delta = (delta & ~0xFFFFu) | fade_speed;
			fade_delta = delta;
			break;
		default:
			invalid_jump(0x16EF2u + ((jump_offset & ~0xFFu) | (uint8_t)fade_kind) * 3u);
		}
		fade_delay = 1u;
		fade_step++;
		return delta;
	case 1:
		fade_delay--;
		if (fade_delay != 0u)
			return delta;
		fade_delay = 1u;
		color_offset_red += fade_delta;
		color_offset_blue += fade_delta;
		color_offset_green += fade_delta;
		switch ((jump_offset & ~0xFFu) | (uint8_t)fade_kind)
		{
		case 0:
		case 4:
			if ((int16_t)color_offset_red <= 0x20)
			{
				fade_step++;
				color_offset_red = 0x20u;
				color_offset_blue = 0x20u;
				color_offset_green = 0x20u;
			}
			break;
		case 1:
			if ((int16_t)color_offset_red <= 0)
			{
				fade_step++;
				color_offset_red = 0u;
				color_offset_blue = 0u;
				color_offset_green = 0u;
			}
			break;
		case 2:
			if ((int16_t)color_offset_red >= 0x3F)
			{
				fade_step++;
				color_offset_red = 0x3Fu;
				color_offset_blue = 0x3Fu;
				color_offset_green = 0x3Fu;
			}
			break;
		case 3:
			if ((int16_t)color_offset_red >= 0x1F)
			{
				fade_step++;
				color_offset_red = 0x1Fu;
				color_offset_blue = 0x1Fu;
				color_offset_green = 0x1Fu;
			}
			break;
		default:
			invalid_jump(0x16FD3u + ((jump_offset & ~0xFFu) | (uint8_t)fade_kind) * 3u);
		}
		return delta;
	case 2:
		fade_kind = -1;
		return delta;
	default:
		invalid_jump(0x16ED6u + fade_step * 3u);
	}
}

/* Stage 0's exit by stage_00_exit_step: the camera climbs straight up at scroll_speed (0x35, later down
   to 2), the tower wall and beam scripts run as NBG0's y scroll passes 0x20 and 0xF6, pages changing
   with stage_00_exit_timer; a fade to white; then NBG0 and NBG1 zoom in on new pages (0x2000000 /
   stage_00_exit_zoom >> 8) as colours come back. */
void update_stage_00_exit_scene(uint32_t dest, uint32_t *p_speed_value, uint32_t *p_script)
{
	uint16_t layer_scroll_x = 0, layer_zoom = 0, scroll_y_past_trigger = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, speed = 0;
	uint16_t scroll_y_offset = 0;

	if ((int8_t)stage_00_exit_step < 2)
	{
		if (stage_00_exit_step == 0u)
		{
			scroll_speed = 0x35u;
			scroll_y_past_trigger = (uint16_t)(nbg0_scroll_y_int - 0x20u);
			if ((int16_t)scroll_y_past_trigger < 8 && (int16_t)scroll_y_past_trigger >= 0)
			{
				*p_script = stage_00_exit_tower_wall_script_ADDRESS;
				run_vram_script(&ignored, p_script, &dest);
				*p_script = clear_tile_page_03_script_ADDRESS;
				run_vram_script(&ignored, p_script, &dest);
				stage_00_exit_step++;
			}
		}
		if ((int8_t)stage_00_exit_step < 2)
		{
			scroll_y_offset = nbg0_scroll_y_int - 0xF6u;
			scroll_y_past_trigger = scroll_y_offset;
			if ((int16_t)scroll_y_offset >= 0 && (int16_t)scroll_y_past_trigger < 8)
			{
				nbg0_pages = 0x2020202u;
				*p_script = stage_00_exit_beam_script_ADDRESS;
				run_vram_script(&ignored, p_script, &dest);
				*p_script = stage_00_exit_beam_end_script_ADDRESS;
				run_vram_script(&ignored, p_script, &dest);
				mixer_text_blend &= 0xBFFFu;
				mixer_nbg0_blend |= 0x4000u;
				mixer_nbg1_blend |= 0x4000u;
				mixer_nbg2_blend |= 0x4000u;
				mixer_nbg3_blend |= 0x4000u;
				mixer_bitmap_blend &= 0xBFFFu;
				mixer_sprite_control |= 0x8000u;
				color_offset_red = 0u;
				color_offset_blue = 0u;
				color_offset_green = 0u;
				stage_00_exit_step++;
			}
		}
	}
	switch (stage_00_exit_step)
	{
	case 0:
	case 1:
	case 2:
	case 11:
		if (stage_00_exit_timer >= 0x8C)
		{
			nbg0_pages = 0x3020100u;
			stage_00_exit_step++;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
		return;
	case 3:
		if (stage_00_exit_timer >= 0xA0)
		{
			nbg0_pages = 0x3030100u;
			stage_00_exit_step++;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
		return;
	case 4:
		if (stage_00_exit_timer >= 0xAF)
		{
			nbg0_pages = 0x3030303u;
			stage_00_exit_step++;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
		return;
	case 5:
		scroll_speed--;
		if ((int16_t)scroll_speed <= 2)
			scroll_speed = 2u;
		if (stage_00_exit_timer >= 0xD2)
		{
			stage_00_exit_step++;
			return;
		}
		stage_00_exit_timer++;
		speed = scroll_speed;
		section_limit_y = -1;
		section_direction_y = -0x4000;
		section_direction_x = 0;
		advance_camera_y(speed);
		update_camera_view();
		layer_scroll_x = camera_x_pixels;
		nbg0_scroll_y_int = camera_y_pixels;
		nbg0_scroll_x_int = layer_scroll_x;
		nbg0_scroll_x_int &= 0x3FFu;
		nbg0_scroll_y_int &= 0x1FFu;
		update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
		return;
	case 6:
		clamp_colour_offsets();
		stage_00_exit_delay++;
		if (stage_00_exit_delay != 5u)
		{
			stage_00_exit_timer++;
			speed = scroll_speed;
			section_limit_y = -1;
			section_direction_y = -0x4000;
			section_direction_x = 0;
			advance_camera_y(speed);
			update_camera_view();
			layer_scroll_x = camera_x_pixels;
			nbg0_scroll_y_int = camera_y_pixels;
			nbg0_scroll_x_int = layer_scroll_x;
			nbg0_scroll_x_int &= 0x3FFu;
			nbg0_scroll_y_int &= 0x1FFu;
			update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red++;
		color_offset_blue++;
		color_offset_green++;
		scroll_speed--;
		if ((int16_t)scroll_speed <= 2)
			scroll_speed = 2u;
		if ((int16_t)color_offset_red < 0x1F)
		{
			stage_00_exit_timer++;
			speed = scroll_speed;
			section_limit_y = -1;
			section_direction_y = -0x4000;
			section_direction_x = 0;
			advance_camera_y(speed);
			update_camera_view();
			layer_scroll_x = camera_x_pixels;
			nbg0_scroll_y_int = camera_y_pixels;
			nbg0_scroll_x_int = layer_scroll_x;
			nbg0_scroll_x_int &= 0x3FFu;
			nbg0_scroll_y_int &= 0x1FFu;
			update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
			return;
		}
		nbg0_scroll_y_int = 0x58u;
		nbg0_scroll_x_int = 0xB8u;
		nbg1_scroll_y_int = 0x58u;
		nbg1_scroll_x_int = 0xB8u;
		nbg0_center_x = 0xA0u;
		nbg0_center_y = 0x90u;
		nbg1_center_x = 0xA0u;
		nbg1_center_y = 0x90u;
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 1u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 2u);
		stage_00_exit_step++;
		return;
	case 7:
		nbg0_pages = 0x6060505u;
		video_layer_opaque_shadow = 0xBu;
		stage_00_exit_step++;
		return;
	case 8:
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~1u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~2u);
		mem_write32(video_layer_disable, mem_read32(video_layer_disable) | 8u);
		mem_write32(video_control, mem_read32(video_control) | 0x1000u);
		mem_write32(video_control, mem_read32(video_control) | 0x2000u);
		mem_write32(video_control, mem_read32(video_control) | 0x4000u);
		mem_write16(nbg0_zoom_x, 0x100u);
		mem_write16(nbg0_zoom_y, 0x100u);
		mem_write16(nbg1_zoom_x, 0x100u);
		mem_write16(nbg1_zoom_y, 0x100u);
		*p_script = stage_00_exit_zoom_page_0_script_ADDRESS;
		run_vram_script(&ignored, p_script, &dest);
		*p_script = stage_00_exit_zoom_page_1_script_ADDRESS;
		run_vram_script(&ignored, p_script, &dest);
		*p_script = stage_00_exit_zoom_page_2_script_ADDRESS;
		run_vram_script(&ignored, p_script, &dest);
		mem_write16(backdrop_color, 0x1356u);
		nbg0_pages = 0x2020200u;
		nbg1_pages = 0x2020201u;
		stage_00_exit_step++;
		stage_00_exit_zoom = 0x200u;
		return;
	case 9:
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		clamp_colour_offsets();
		stage_00_exit_delay++;
		if (stage_00_exit_delay != 3u)
		{
			update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
			return;
		}
		stage_00_exit_delay = 0u;
		color_offset_red--;
		color_offset_blue--;
		color_offset_green--;
		if (color_offset_red != 0u)
		{
			update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
			return;
		}
		stage_00_exit_step++;
		return;
	case 10:
		update_background_scroll(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_script, &ignored_5);
		if ((int16_t)stage_00_exit_zoom < 0x42)
			stage_00_exit_zoom = 0x42u;
		stage_00_exit_zoom--;
		layer_zoom = (uint16_t)(divide_s32(0x2000000u, stage_00_exit_zoom) >> 8);
		mem_write16(nbg0_zoom_x, layer_zoom);
		mem_write16(nbg0_zoom_y, layer_zoom);
		mem_write16(nbg1_zoom_x, layer_zoom);
		mem_write16(nbg1_zoom_y, layer_zoom);
		return;
	default:
		invalid_jump(0x17160u + stage_00_exit_step * 3u);
	}
}

/* Masks color_offset_red, _blue and _green to 0-0x1F. */
void clamp_colour_offsets(void)
{
	color_offset_red &= 0x1Fu;
	color_offset_blue &= 0x1Fu;
	color_offset_green &= 0x1Fu;
}

/* update_camera_view, then while camera_shake_active every other frame camera y moves by sine_table at
   camera_shake_wave + camera_shake_step (* 0x20: up to 8 pixels), the step going up 0xA a frame until
   camera_shake_length ends the shake; NBG0's scroll follows. Hands back view_y. */
uint32_t update_camera_with_shake(void)
{
	uint32_t shaken_y = 0, wave_table = 0, y = 0;

	y = update_camera_view();
	if (camera_shake_active == 0u)
		return y;
	switch (camera_shake_phase)
	{
	case 0:
	case 2:
		camera_shake_phase++;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x17610u + camera_shake_phase * 3u);
	}
	camera_shake_base_y = (uint32_t)(int16_t)camera_y_pixels << 16;
	camera_shake_frame++;
	wave_table = 0x100000u | (uint16_t)(0x1632u + camera_shake_wave);
	shaken_y = (int16_t)mem_read16((wave_table & ~0xFFFFu) | (uint16_t)(wave_table + camera_shake_step));
	camera_shake_step += 0xAu;
	shaken_y = shaken_y * 0x20u + camera_shake_base_y;
	if ((camera_shake_frame & 1u) != 0u)
		shaken_y = camera_shake_base_y;
	y = shaken_y;
	y = update_camera_view_y(y);
	y = set_nbg0_scroll_from_camera(y);
	if ((int16_t)camera_shake_length <= (int16_t)camera_shake_step)
	{
		camera_shake_active = 0u;
		camera_shake_frame = 0u;
		camera_shake_frame_pad = 0u;
		camera_shake_step = 0u;
		camera_shake_phase = 0u;
	}
	return y;
}

/* NBG0's scroll from the camera's pixels (x & 0x3FF, y & 0x1FF) and the camera's place in the stage's
   block map: tile_block_column = camera_x_pixels / 0x200, tile_block_row = camera_y_pixels / 0x100 *
   stage_block_map_width. Hands back scroll_x with camera_x_pixels in its low half. */
uint32_t set_nbg0_scroll_from_camera(uint32_t scroll_x)
{
	uint16_t layer_scroll_y = 0;
	uint16_t block_column = 0, block_row = 0;

	layer_scroll_y = camera_y_pixels;
	scroll_x = (scroll_x & ~0xFFFFu) | camera_x_pixels;
	nbg0_scroll_y_int = layer_scroll_y;
	nbg0_scroll_x_int = scroll_x;
	tile_block_row = layer_scroll_y;
	tile_block_column = scroll_x;
	tile_block_column &= ~0x1FFu;
	tile_block_row &= ~0xFFu;
	nbg0_scroll_x_int &= 0x3FFu;
	nbg0_scroll_y_int &= 0x1FFu;
	block_column = tile_block_column;
	tile_block_column = ((uint32_t)block_column << 7) | (block_column >> 9);
	block_row = tile_block_row;
	tile_block_row = ((uint32_t)block_row << 8) | (block_row >> 8);
	tile_block_row = (uint32_t)tile_block_row * stage_block_map_width;
	return scroll_x;
}

/* camera_x_pixels and camera_y_pixels from camera_x/_y's whole parts, view_x/_y from them minus
   view_offset_x/_y; hands back view_y. */
uint32_t update_camera_view(void)
{
	uint32_t y = 0;

	camera_x_pixels = (uint32_t)camera_x >> 16;
	view_x = camera_x_pixels - view_offset_x;
	y = camera_y;
	return update_camera_view_y(y);
}

/* camera_y_pixels from y's whole part (16.16) and view_y = it minus view_offset_y, which it hands back. */
uint32_t update_camera_view_y(uint32_t y)
{
	y = y >> 16;
	camera_y_pixels = y;
	y = camera_y_pixels;
	y = (uint16_t)(y - view_offset_y);
	view_y = y;
	return y;
}

/* Moves the camera along the section by speed: camera_x/_y by section_direction_x/_y * speed unless that
   passes section_limit_x/_y (from camera_origin, the way the limits run), scroll_position unless it
   passes the section end; camera_limits_reached keeps which stopped (all three:
   snap_camera_to_section_end). Hands back scroll_position + speed. */
uint32_t move_camera_along_path(uint32_t speed)
{
	uint16_t next_offset = 0;
	uint32_t current_position = 0, limit_list = 0;

	camera_limits_reached = 0u;
	if ((uint32_t)section_direction_y == 0u)
		camera_limits_reached |= 4u;
	else
	{
		next_offset = (uint16_t)(((int32_t)(section_direction_y * speed + camera_y) >> 16) - camera_origin_y);
		limit_list = section_limit_y_list;
		if (SECTION_CAMERA_LIMITS(limit_list)[-2] < SECTION_CAMERA_LIMITS(limit_list)[-1])
		{
			if (section_limit_y <= (int16_t)next_offset)
			{
				camera_limits_reached |= 4u;
				if ((uint32_t)section_direction_x == 0u)
				{
					camera_limits_reached |= 2u;
					current_position = scroll_position;
					current_position = (uint16_t)(current_position + speed);
					if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
						camera_limits_reached |= 1u;
					switch (camera_limits_reached)
					{
					case 0:
						advance_camera_scroll_x_y(speed);
						break;
					case 1:
						advance_camera_x_y(speed);
						break;
					case 2:
						advance_camera_y_scroll(speed);
						break;
					case 3:
						advance_camera_y(speed);
						break;
					case 4:
						advance_camera_x_scroll(speed);
						break;
					case 5:
						advance_camera_x(speed);
						break;
					case 6:
						advance_scroll_position((uint16_t)speed);
						break;
					case 7:
						snap_camera_to_section_end();
						break;
					case 8:
						advance_camera_scroll_x_y(speed);
						break;
					default:
						invalid_jump(0x177FDu + camera_limits_reached * 3u);
					}
					return current_position;
				}
				next_offset = (uint16_t)(((int32_t)(section_direction_x * speed + camera_x) >> 16) - camera_origin_x);
				limit_list = section_limit_x_list;
				if (SECTION_CAMERA_LIMITS(limit_list)[-2] < SECTION_CAMERA_LIMITS(limit_list)[-1])
				{
					if ((int16_t)next_offset < section_limit_x)
					{
						current_position = scroll_position;
						current_position = (uint16_t)(current_position + speed);
						if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
							camera_limits_reached |= 1u;
						switch (camera_limits_reached)
						{
						case 0:
							advance_camera_scroll_x_y(speed);
							break;
						case 1:
							advance_camera_x_y(speed);
							break;
						case 2:
							advance_camera_y_scroll(speed);
							break;
						case 3:
							advance_camera_y(speed);
							break;
						case 4:
							advance_camera_x_scroll(speed);
							break;
						case 5:
							advance_camera_x(speed);
							break;
						case 6:
							advance_scroll_position((uint16_t)speed);
							break;
						case 7:
							snap_camera_to_section_end();
							break;
						case 8:
							advance_camera_scroll_x_y(speed);
							break;
						default:
							invalid_jump(0x177FDu + camera_limits_reached * 3u);
						}
						return current_position;
					}
					camera_limits_reached |= 2u;
					current_position = scroll_position;
					current_position = (uint16_t)(current_position + speed);
					if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
						camera_limits_reached |= 1u;
					switch (camera_limits_reached)
					{
					case 0:
						advance_camera_scroll_x_y(speed);
						break;
					case 1:
						advance_camera_x_y(speed);
						break;
					case 2:
						advance_camera_y_scroll(speed);
						break;
					case 3:
						advance_camera_y(speed);
						break;
					case 4:
						advance_camera_x_scroll(speed);
						break;
					case 5:
						advance_camera_x(speed);
						break;
					case 6:
						advance_scroll_position((uint16_t)speed);
						break;
					case 7:
						snap_camera_to_section_end();
						break;
					case 8:
						advance_camera_scroll_x_y(speed);
						break;
					default:
						invalid_jump(0x177FDu + camera_limits_reached * 3u);
					}
					return current_position;
				}
				if (section_limit_x < (int16_t)next_offset)
				{
					current_position = scroll_position;
					current_position = (uint16_t)(current_position + speed);
					if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
						camera_limits_reached |= 1u;
					switch (camera_limits_reached)
					{
					case 0:
						advance_camera_scroll_x_y(speed);
						break;
					case 1:
						advance_camera_x_y(speed);
						break;
					case 2:
						advance_camera_y_scroll(speed);
						break;
					case 3:
						advance_camera_y(speed);
						break;
					case 4:
						advance_camera_x_scroll(speed);
						break;
					case 5:
						advance_camera_x(speed);
						break;
					case 6:
						advance_scroll_position((uint16_t)speed);
						break;
					case 7:
						snap_camera_to_section_end();
						break;
					case 8:
						advance_camera_scroll_x_y(speed);
						break;
					default:
						invalid_jump(0x177FDu + camera_limits_reached * 3u);
					}
					return current_position;
				}
				camera_limits_reached |= 2u;
				current_position = scroll_position;
				current_position = (uint16_t)(current_position + speed);
				if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
					camera_limits_reached |= 1u;
				switch (camera_limits_reached)
				{
				case 0:
					advance_camera_scroll_x_y(speed);
					break;
				case 1:
					advance_camera_x_y(speed);
					break;
				case 2:
					advance_camera_y_scroll(speed);
					break;
				case 3:
					advance_camera_y(speed);
					break;
				case 4:
					advance_camera_x_scroll(speed);
					break;
				case 5:
					advance_camera_x(speed);
					break;
				case 6:
					advance_scroll_position((uint16_t)speed);
					break;
				case 7:
					snap_camera_to_section_end();
					break;
				case 8:
					advance_camera_scroll_x_y(speed);
					break;
				default:
					invalid_jump(0x177FDu + camera_limits_reached * 3u);
				}
				return current_position;
			}
		}
		else if ((int16_t)next_offset <= section_limit_y)
			camera_limits_reached |= 4u;
	}
	if ((uint32_t)section_direction_x == 0u)
	{
		camera_limits_reached |= 2u;
		current_position = scroll_position;
		current_position = (uint16_t)(current_position + speed);
		if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
			camera_limits_reached |= 1u;
		switch (camera_limits_reached)
		{
		case 0:
			advance_camera_scroll_x_y(speed);
			break;
		case 1:
			advance_camera_x_y(speed);
			break;
		case 2:
			advance_camera_y_scroll(speed);
			break;
		case 3:
			advance_camera_y(speed);
			break;
		case 4:
			advance_camera_x_scroll(speed);
			break;
		case 5:
			advance_camera_x(speed);
			break;
		case 6:
			advance_scroll_position((uint16_t)speed);
			break;
		case 7:
			snap_camera_to_section_end();
			break;
		case 8:
			advance_camera_scroll_x_y(speed);
			break;
		default:
			invalid_jump(0x177FDu + camera_limits_reached * 3u);
		}
		return current_position;
	}
	next_offset = (uint16_t)(((int32_t)(section_direction_x * speed + camera_x) >> 16) - camera_origin_x);
	limit_list = section_limit_x_list;
	if (SECTION_CAMERA_LIMITS(limit_list)[-2] < SECTION_CAMERA_LIMITS(limit_list)[-1])
	{
		if ((int16_t)next_offset < section_limit_x)
		{
			current_position = scroll_position;
			current_position = (uint16_t)(current_position + speed);
			if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
				camera_limits_reached |= 1u;
			switch (camera_limits_reached)
			{
			case 0:
				advance_camera_scroll_x_y(speed);
				break;
			case 1:
				advance_camera_x_y(speed);
				break;
			case 2:
				advance_camera_y_scroll(speed);
				break;
			case 3:
				advance_camera_y(speed);
				break;
			case 4:
				advance_camera_x_scroll(speed);
				break;
			case 5:
				advance_camera_x(speed);
				break;
			case 6:
				advance_scroll_position((uint16_t)speed);
				break;
			case 7:
				snap_camera_to_section_end();
				break;
			case 8:
				advance_camera_scroll_x_y(speed);
				break;
			default:
				invalid_jump(0x177FDu + camera_limits_reached * 3u);
			}
			return current_position;
		}
		camera_limits_reached |= 2u;
		current_position = scroll_position;
		current_position = (uint16_t)(current_position + speed);
		if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
			camera_limits_reached |= 1u;
		switch (camera_limits_reached)
		{
		case 0:
			advance_camera_scroll_x_y(speed);
			break;
		case 1:
			advance_camera_x_y(speed);
			break;
		case 2:
			advance_camera_y_scroll(speed);
			break;
		case 3:
			advance_camera_y(speed);
			break;
		case 4:
			advance_camera_x_scroll(speed);
			break;
		case 5:
			advance_camera_x(speed);
			break;
		case 6:
			advance_scroll_position((uint16_t)speed);
			break;
		case 7:
			snap_camera_to_section_end();
			break;
		case 8:
			advance_camera_scroll_x_y(speed);
			break;
		default:
			invalid_jump(0x177FDu + camera_limits_reached * 3u);
		}
		return current_position;
	}
	if (section_limit_x < (int16_t)next_offset)
	{
		current_position = scroll_position;
		current_position = (uint16_t)(current_position + speed);
		if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
			camera_limits_reached |= 1u;
		switch (camera_limits_reached)
		{
		case 0:
			advance_camera_scroll_x_y(speed);
			break;
		case 1:
			advance_camera_x_y(speed);
			break;
		case 2:
			advance_camera_y_scroll(speed);
			break;
		case 3:
			advance_camera_y(speed);
			break;
		case 4:
			advance_camera_x_scroll(speed);
			break;
		case 5:
			advance_camera_x(speed);
			break;
		case 6:
			advance_scroll_position((uint16_t)speed);
			break;
		case 7:
			snap_camera_to_section_end();
			break;
		case 8:
			advance_camera_scroll_x_y(speed);
			break;
		default:
			invalid_jump(0x177FDu + camera_limits_reached * 3u);
		}
		return current_position;
	}
	camera_limits_reached |= 2u;
	current_position = scroll_position;
	current_position = (uint16_t)(current_position + speed);
	if ((int32_t)SECTION_END_POSITIONS(section_end_list)[0] < (int32_t)current_position)
		camera_limits_reached |= 1u;
	switch (camera_limits_reached)
	{
	case 0:
		advance_camera_scroll_x_y(speed);
		break;
	case 1:
		advance_camera_x_y(speed);
		break;
	case 2:
		advance_camera_y_scroll(speed);
		break;
	case 3:
		advance_camera_y(speed);
		break;
	case 4:
		advance_camera_x_scroll(speed);
		break;
	case 5:
		advance_camera_x(speed);
		break;
	case 6:
		advance_scroll_position((uint16_t)speed);
		break;
	case 7:
		snap_camera_to_section_end();
		break;
	case 8:
		advance_camera_scroll_x_y(speed);
		break;
	default:
		invalid_jump(0x177FDu + camera_limits_reached * 3u);
	}
	return current_position;
}

/* advance_scroll_position, advance_camera_x and advance_camera_y by speed: the scroll and the camera
   both go on. */
void advance_camera_scroll_x_y(uint32_t speed)
{
	advance_scroll_position((uint16_t)speed);
	advance_camera_x(speed);
	advance_camera_y(speed);
}

/* advance_camera_x and advance_camera_y by speed (scroll_position at the section's end). */
void advance_camera_x_y(uint32_t speed)
{
	advance_camera_x(speed);
	advance_camera_y(speed);
}

/* advance_camera_y and advance_scroll_position by speed (the camera's x at its limit). */
void advance_camera_y_scroll(uint32_t speed)
{
	advance_camera_y(speed);
	advance_scroll_position((uint16_t)speed);
}

/* camera_y += section_direction_y * speed. */
void advance_camera_y(uint32_t speed)
{
	camera_y += section_direction_y * speed;
}

/* advance_camera_x and advance_scroll_position by speed (the camera's y at its limit). */
void advance_camera_x_scroll(uint32_t speed)
{
	advance_camera_x(speed);
	advance_scroll_position((uint16_t)speed);
}

/* camera_x += section_direction_x * speed. */
void advance_camera_x(uint32_t speed)
{
	camera_x += section_direction_x * speed;
}

/* scroll_position += speed. */
void advance_scroll_position(uint16_t speed)
{
	scroll_position += speed;
}

/* Puts the camera at the section's x and y limits (+ camera_origin_x/_y) and scroll_position at the
   section's end. */
void snap_camera_to_section_end(void)
{
	uint32_t section_end_y = 0;

	section_end_y = (uint32_t)(uint16_t)(SECTION_CAMERA_LIMITS(section_limit_y_list)[-1] + camera_origin_y) << 16;
	camera_x = (uint32_t)(uint16_t)(SECTION_CAMERA_LIMITS(section_limit_x_list)[-1] + camera_origin_x) << 16;
	camera_y = section_end_y;
	scroll_position = SECTION_END_POSITIONS(section_end_list)[0];
}

/* Unused: the scroll skip panel's frame. Closed (scroll_skip_open 0), it opens while service and 2P
   start are held (input_ports_active bits 0 and 5): scroll_skip_last_point for the stage,
   stage_start_section 0xD, armed flags cleared, the panel printed. Open: scroll_skip_draw_values,
   _next_point, _previous_point, _check_call, _check_exit. */
void run_scroll_skip_mode(void)
{
	if (scroll_skip_open == 0u)
	{
		if ((input_ports_active & 0x21u) != 0x21u)
		{
			scroll_skip_open = 0u;
			return;
		}
		scroll_skip_last_point = scroll_skip_last_points[stage & 0x7FFFu];
		stage_start_section = 0xDu;
		scroll_skip_value_index = 0u;
		scroll_skip_next_armed = 0u;
		scroll_skip_back_armed = 0u;
		scroll_skip_exit_armed = 0u;
		scroll_skip_call_armed = 0u;
		scroll_skip_open = 1u;
		scroll_skip_print_panel();
	}
	scroll_skip_draw_values();
	scroll_skip_next_point();
	scroll_skip_previous_point();
	scroll_skip_check_call();
	scroll_skip_check_exit();
}

/* Unused: "NEXT:1P JUMP": 1P jump pressed after a release (scroll_skip_next_armed) adds 1 to
   stage_start_section, which is kept at most scroll_skip_last_point; nothing while both jumps are held. */
void scroll_skip_next_point(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x1010000u;
	if (held_buttons == 0x1010000u)
	{
		if (scroll_skip_last_point < (int16_t)stage_start_section)
			stage_start_section = scroll_skip_last_point;
		return;
	}
	if ((held_buttons & 0x10000u) == 0u)
	{
		scroll_skip_next_armed = 1u;
		if (scroll_skip_last_point < (int16_t)stage_start_section)
			stage_start_section = scroll_skip_last_point;
		return;
	}
	if (scroll_skip_next_armed == 0u)
	{
		if (scroll_skip_last_point < (int16_t)stage_start_section)
			stage_start_section = scroll_skip_last_point;
		return;
	}
	stage_start_section++;
	scroll_skip_next_armed = 0u;
	if (scroll_skip_last_point < (int16_t)stage_start_section)
		stage_start_section = scroll_skip_last_point;
}

/* Unused: "BACK:2P JUMP": 2P jump pressed after a release (scroll_skip_back_armed) takes 1 off
   stage_start_section, not below 0; nothing while both jumps are held. */
void scroll_skip_previous_point(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x1010000u;
	if (held_buttons == 0x1010000u)
	{
		if ((int16_t)stage_start_section < 0)
			stage_start_section = 0u;
		return;
	}
	if ((held_buttons & 0x1000000u) == 0u)
	{
		scroll_skip_back_armed = 1u;
		if ((int16_t)stage_start_section < 0)
			stage_start_section = 0u;
		return;
	}
	if (scroll_skip_back_armed == 0u)
	{
		if ((int16_t)stage_start_section < 0)
			stage_start_section = 0u;
		return;
	}
	stage_start_section--;
	scroll_skip_back_armed = 0u;
	if ((int16_t)stage_start_section < 0)
		stage_start_section = 0u;
}

/* Unused: "EXIT:2P START": 2P start pressed after a release (scroll_skip_exit_armed) closes the panel
   (scroll_skip_open 0, scroll_skip_erase_panel); nothing while both starts are held. */
void scroll_skip_check_exit(void)
{
	uint32_t held_buttons = 0;

	held_buttons = input_ports_active & 0x30u;
	if (held_buttons == 0x30u)
		return;
	if ((held_buttons & 0x20u) != 0u)
	{
		if (scroll_skip_exit_armed != 0u)
		{
			scroll_skip_open = 0u;
			scroll_skip_erase_panel();
			scroll_skip_exit_armed = 0u;
		}
	}
	else
		scroll_skip_exit_armed = 1u;
}

/* Unused: prints scroll_skip_blank_text over the panel (print_text_list). */
void scroll_skip_erase_panel(void)
{
	uint32_t ignored = 0, text = 0;

	text = scroll_skip_blank_text_ADDRESS;
	print_text_list(&text, &ignored);
}

/* Unused: "CALL:1P START": 1P start pressed after a release (scroll_skip_call_armed) moves the camera to
   stage_start_section (move_camera_to_start_section), every frame while it stays held, as the flag is
   not cleared; nothing while both starts are held. */
void scroll_skip_check_call(void)
{
	uint32_t held_buttons = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	held_buttons = input_ports_active & 0x30u;
	if (held_buttons == 0x30u)
		return;
	if ((held_buttons & 0x10u) != 0u)
	{
		if (scroll_skip_call_armed == 0u)
			return;
		move_camera_to_start_section(&ignored, &ignored_2, &ignored_3);
		return;
	}
	scroll_skip_call_armed = 1u;
}

/* load_camera_start_section, then update_camera_view; y gets view_y. */
void move_camera_to_start_section(uint32_t *p_y, uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	load_camera_start_section(p_section_offset, p_stage_index);
	*p_y = update_camera_view();
}

/* Puts the camera and the scroll at the start of section stage_start_section of the stage's tables, as
   init_stage_view does for the first: camera_x/_y at the section's start (+ camera_origin_x/_y), its x
   and y limits and direction, scroll_position at its start and its end next, scroll_speed,
   follow_base_speed and its scroll mode. */
void load_camera_start_section(uint32_t *p_section_offset, uint32_t *p_stage_index)
{
	uint32_t direction_y_cursor = 0, limit_y_cursor = 0, list = 0, section_index = 0, section_offset = 0, stage_index = 0;
	uint16_t start_y = 0, limit_y = 0, direction_y = 0;

	stage_index = stage;
	section_offset = stage_start_section;
	section_offset = (uint16_t)(section_offset << 1);
	list = stage_section_limits_x[stage_index] + stage_section_limits_x_ADDRESS + section_offset;
	limit_y_cursor = stage_section_limits_y[stage_index] + stage_section_limits_y_ADDRESS + section_offset;
	start_y = SECTION_CAMERA_LIMITS(limit_y_cursor)[0];
	limit_y_cursor += 2u;
	camera_x = (uint32_t)(uint16_t)SECTION_CAMERA_LIMITS(list)[0] << 16;
	camera_y = (uint32_t)start_y << 16;
	camera_x += (uint32_t)camera_origin_x << 16;
	camera_y += (uint32_t)camera_origin_y << 16;
	list += 2u;
	section_limit_x = SECTION_CAMERA_LIMITS(list)[0];
	limit_y = SECTION_CAMERA_LIMITS(limit_y_cursor)[0];
	section_limit_y = limit_y;
	section_limit_x_list = list + 2u;
	section_limit_y_list = limit_y_cursor + 2u;
	list = stage_section_directions_x[stage_index] + stage_section_directions_x_ADDRESS + section_offset;
	direction_y_cursor = stage_section_directions_y[stage_index] + stage_section_directions_y_ADDRESS + section_offset;
	section_direction_x = SECTION_DIRECTIONS(list)[0];
	direction_y = SECTION_DIRECTIONS(direction_y_cursor)[0];
	section_direction_y = (int16_t)direction_y;
	section_direction_x_list = list + 2u;
	section_direction_y_list = direction_y_cursor + 2u;
	list = stage_section_ends[stage_index] + stage_section_ends_ADDRESS + section_offset;
	scroll_position = SECTION_END_POSITIONS(list)[0];
	section_end_list = list + 2u;
	section_index = section_offset >> 1;
	list = stage_section_speeds[stage_index] + stage_section_speeds_ADDRESS + section_index;
	scroll_speed = SECTION_SCROLL_SPEEDS(list)[0];
	section_speed_list = list + 1u;
	list = stage_section_follow_speeds[stage_index] + stage_section_follow_speeds_ADDRESS + section_index;
	follow_base_speed = SECTION_FOLLOW_SPEEDS(list)[0];
	follow_base_speed_list = list + 1u;
	list = stage_scroll_modes[stage_index] + stage_scroll_modes_ADDRESS + section_offset + section_offset;
	scroll_mode = SCROLL_MODE_STEP(list)->mode;
	list += 2u;
	scroll_mode_timer = SCROLL_MODE_STEP(list - 0x2u)->duration;
	scroll_mode_duration = scroll_mode_timer;
	scroll_ramp_speed = 1u;
	scroll_ramp_speed_pad = 0u;
	scroll_mode_script = list + 2u;
	*p_section_offset = section_offset;
	*p_stage_index = stage_index;
}

/* Unused: reads two per-stage tables at once: first_entry and second_entry each become the long at their
   address + stage_offset, plus section_offset. */
void index_stage_list_pair(uint32_t section_offset, uint32_t stage_offset, uint32_t *p_first_entry, uint32_t *p_second_entry)
{
	uint32_t second_entry = *p_second_entry;

	second_entry = mem_read32((second_entry & ~0xFFFFu) | (uint16_t)(second_entry + stage_offset)) + section_offset;
	*p_first_entry = mem_read32(*p_first_entry + stage_offset) + section_offset;
	*p_second_entry = second_entry;
}

/* Unused: hands back text cell (34,18), where the X POSITION value goes. */
uint32_t scroll_skip_x_position_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 18);
	return text_position;
}

/* Unused: hands back text cell (34,19), where the Y POSITION value goes. */
uint32_t scroll_skip_y_position_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 19);
	return text_position;
}

/* Unused: hands back text cell (34,17), where the SCROLL TOTAL value goes. */
uint32_t scroll_skip_total_cell(void)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(34, 17);
	return text_position;
}

/* Unused: one value a frame (scroll_skip_value_index 0-2): the stage's entry for stage_start_section in
   scroll_skip_value_lists[index] (x position, y position, scroll total) in hex at its cell (attribute
   0x200); after the third, stage_start_section in hex at (24,15) (attribute 0x600). */
void scroll_skip_draw_values(void)
{
	uint16_t attribute_2 = 0;
	uint32_t attribute = 0, point = 0, text_position = 0, value = 0, value_list = 0;

	point = stage_start_section;
	value_list = scroll_skip_value_lists[scroll_skip_value_index];
	value = SECTION_VALUES(value_list + STAGE_SECTION_LIST_OFFSETS((value_list & ~0xFFFFu) | (uint16_t)(value_list + stage * 2u))[0])[point & 0x7FFFu];
	switch (scroll_skip_value_index)
	{
	case 0:
		text_position = scroll_skip_x_position_cell();
		break;
	case 1:
		text_position = scroll_skip_y_position_cell();
		break;
	case 2:
		text_position = scroll_skip_total_cell();
		break;
	case 3:
		text_position = scroll_skip_x_position_cell();
		break;
	default:
		invalid_jump(0x1AAC1u + scroll_skip_value_index * 3u);
	}
	attribute = 0x200u;
	draw_hex_4(&value, &attribute, &text_position);
	scroll_skip_value_index++;
	if (scroll_skip_value_index != 3u)
		return;
	scroll_skip_value_index = 0u;
	value = stage_start_section;
	text_position = TEXT_CELL(24, 15);
	attribute_2 = 0x600u;
	draw_hex_2(value, attribute_2, text_position);
}

/* Unused: prints the scroll skip panel (scroll_skip_panel_text) with print_text_list. */
void scroll_skip_print_panel(void)
{
	uint32_t ignored = 0, text = 0;

	text = scroll_skip_panel_text_ADDRESS;
	print_text_list(&text, &ignored);
}

/* Unused: clears the lower half of the 8 bpp bitmap layer (rows 0x70-0xDF: 0x7000 halfwords from video
   RAM 0xE000, also bitmap_address) and turns the bitmap layer on with bitmap_8bpp 1 and bitmap_window
   0,0-0x13F,0xDF. */
void clear_bitmap_lower_half(void)
{
	bitmap_address = VIDEO_RAM + 0xE000u;
	fill_value = fill_value & ~0xFFFFu;
	set_halfwords(VIDEO_RAM + 0xE000u, 0x7000u, fill_value);
	mem_write32(video_layer_disable, mem_read32(video_layer_disable) & ~0x20u);
	bitmap_8bpp = 1u;
	bitmap_8bpp_pad = 0u;
	bitmap_window_left = 0;
	bitmap_window_top = 0;
	bitmap_window_right = 0x13F;
	bitmap_window_bottom = 0xDF;
}
