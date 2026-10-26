/**
 *    Name:        stage/game_modes.c
 *    Description: the game modes (run_game_mode) and what a stage frame runs: starting a stage, layer blending, the stage clear check, the player summary, stage slots and events, restart points, the player objects, the GAME OVER banner and the demo prompt
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    26-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* One frame of the game: runs the mode game_mode names (0x21, past the table, runs game_mode_start_game
   too). A mode that changes game_mode runs the new one in the same frame (run_game_mode again). */
void run_game_mode(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	switch (game_mode)
	{
	case 0:
	case 33:
		game_mode_start_game(p_count, p_list_cursor);
		return;
	case 1:
		game_mode_intro(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 2:
		start_stage(next_scroll_position, &position, &speed, p_speed_value, p_count, p_cycle, p_list_cursor, &object);
		return;
	case 3:
	case 5:
	case 6:
		game_mode_play_stage(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 4:
		*p_speed_value = game_mode_stage_end_fade(*p_speed_value, object);
		return;
	case 7:
		game_mode_game_over(pressed, position, (uint16_t)speed, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 8:
		game_mode_stage_select(p_count, p_list_cursor);
		return;
	case 9:
		game_mode_event_09(speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 10:
	case 11:
		game_mode_event_0A(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 12:
		game_mode_event_0C(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 13:
		game_mode_event_0D(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 14:
		game_mode_continue(position, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 15:
		game_mode_event_0F(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 16:
		game_mode_event_10(speed, object, p_speed_value, p_cycle, p_list_cursor);
		return;
	case 17:
		game_mode_screen_wipe(position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 18:
		game_mode_event_12(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 19:
		game_mode_event_13(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 20:
		game_mode_event_14(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 21:
		game_mode_event_15(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 22:
		game_mode_event_16(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 23:
		game_mode_stage_clear(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 24:
		game_mode_event_18(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 25:
		game_mode_event_19(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 26:
		game_mode_event_1A(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 27:
		game_mode_character_select(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 28:
		game_mode_event_1C(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 29:
		game_mode_stage_results(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 30:
		game_mode_stage_change_fade(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 31:
		game_mode_event_1F(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	case 32:
		game_mode_you_failed(pressed, position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	default:
		invalid_jump(0x65Fu + game_mode * 3u);
	}
}

/* Game modes 0 and 0x21, starting a game or a demo: the screen and the player blocks cleared,
   player_status 0 (the spare byte 0xFF), the player objects started. A game (game_in_progress) clears
   cleared_stages and game_frames, current_screen 6, and goes to character select (game_mode 0x1B); a
   demo takes stage from demo_input and starts it (game_mode 2, stage_start_kind 0xFF). */
void game_mode_start_game(uint32_t *p_count, uint32_t *p_list_cursor)
{
	uint8_t mode = 0, next_game_mode = 0;
	uint32_t control_register = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;
	uint8_t new_game_cnt_bits = 0;

	display_off_frames = 2u;
	new_game_cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, new_game_cnt_bits & ~2u);
	clear_text_layer(&ignored, &ignored_2, &ignored_3);
	clear_page_0_and_scroll();
	if ((int8_t)game_in_progress < 0)
	{
		mem_write16(cleared_stages_ADDRESS, 0u);
		current_screen = 6u;
		game_frames = 0u;
	}
	else
		stage = DEMO_HEADER(demo_input)->stage_number;
	stage_start_kind = 0u;
	player_status[0] = 0u;
	player_2_status = 0u;
	player_3_status = 0u;
	player_status_spare = 0xFFu;
	stage_init_flags = 0u;
	intro_character = -1;
	players_in_play = 0;
	mode = 0u;
	control_register = set_screen_mode(mode);
	*p_count = clear_player_blocks();
	*p_list_cursor = start_player_objects(control_register);
	next_game_mode = 0x1Bu;
	if ((int8_t)game_in_progress < 0)
	{
		game_mode = next_game_mode;
		current_game_mode = 0u;
		reset_linked_objects_flag();
		return;
	}
	next_game_mode = 2u;
	stage_start_kind = 0xFFu;
	game_mode = next_game_mode;
	current_game_mode = 0u;
	reset_linked_objects_flag();
}

/* Game mode 2, a stage starts: cleared_stages + 1 (at most 0xE) if stage_start_kind is 0, player objects
   restarted unless it is negative, contact lists and stage state cleared; blending, view (a demo's start
   section from demo_input), map, goal, floors, spawns, stage_start_sounds[stage], palette; game_mode 3.
   In a game, cabinet_events 0x15 (0x14 when cleared_stages - 2 is odd). */
void start_stage(uint32_t stage_offset, uint32_t *p_write_index_address, uint32_t *p_floor_block, uint32_t *p_bounds_offset, uint32_t *p_corners_left, uint32_t *p_palette_record, uint32_t *p_source, uint32_t *p_object)
{
	uint8_t cabinet_event = 0, sound_id = 0, stages_past_2 = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t cleared_count = 0;

	if (stage_start_kind == 0u)
	{
		mem_write16(cleared_stages_ADDRESS, cleared_stages + 1u);
		if ((int8_t)(uint8_t)cleared_stages > 0xE)
			mem_write8(cleared_stages_ADDRESS, 0xEu);
	}
	if ((int8_t)stage_start_kind >= 0)
		*p_floor_block = start_player_objects_alt(*p_object);
	hitter_list = 0xFFFFu;
	touchable_list = 0xFFFFu;
	platform_list = 0xFFFFu;
	rider_list = 0xFFFFu;
	wall_list = 0xFFFFu;
	zone_list = 0xFFFFu;
	skip_player_summary = 0xFFu;
	unused_game_byte = 0u;
	camera_ground_speed_vx = 0u;
	camera_ground_speed_vy = 0u;
	camera_slope_speed_vx = 0u;
	camera_slope_speed_vy = 0u;
	scroll_drift_vx = 0u;
	scroll_drift_vy = 0u;
	camera_slope_speed = 0u;
	camera_slope_direction = 0u;
	scroll_drift_speed = 0u;
	scroll_drift_direction = 0u;
	camera_scroll_direction = 0u;
	unused_camera_speed_word = 0u;
	player_dropping_in = 0u;
	player_dropping_in_before = 0u;
	grey_screen_active = 0u;
	grey_screen_ended = 0u;
	player_mishap = 0u;
	all_players_path_request = 0u;
	players_finished[0] = 0u;
	players_finished[1] = 0u;
	players_finished[2] = 0u;
	goal_unlocked = 0u;
	unused_player_flags[0] = 0u;
	unused_player_flags[1] = 0u;
	unused_player_flags[2] = 0u;
	unused_player_flags[3] = 0u;
	unused_stage_byte = 0u;
	map_screen_active = 0u;
	per_player_view_mode = 0u;
	players_pulled = 0u;
	handle_rod_drop_players = 0u;
	stage_exit_started = 0u;
	chaser_object = 0u;
	scroll_resume_request = 0u;
	demo_prompt_shown = 0u;
	event_0d_active = 0u;
	players_frozen = 0u;
	rope_bridge_collapsing = 0u;
	barred_gate_closing = 0u;
	event_1c_riding = 0u;
	event_1c_ride_phase = 0u;
	ending_finished = 0u;
	event_12_fast_scroll = 0u;
	group_leader_0_held = 0u;
	group_leaders_1_3_held = 0u;
	sand_block_leader_active = 0u;
	chaser_stopped = 0u;
	sliding_door_opened = 0u;
	stage_b_sound_done = 0u;
	spiked_press_object = 0u;
	icicle_dropper_ready = 0xFFu;
	apply_stage_layer_blending(p_write_index_address, p_corners_left);
	start_stage_view(p_write_index_address, p_floor_block, p_bounds_offset, &ignored);
	if ((int8_t)game_in_progress >= 0)
	{
		stage_start_section = DEMO_HEADER(demo_input)->start_section;
		move_camera_to_start_section(p_write_index_address, p_floor_block, &ignored);
		demo_input += 0x10u;
	}
	load_stage_map_pointers(&ignored_2, &ignored_3);
	load_stage_goal();
	find_nearby_floor_blocks(&ignored_2, p_floor_block, p_bounds_offset, &ignored, p_corners_left, &ignored_3, &ignored_4);
	start_spawns_by_type();
	sound_id = stage_start_sounds[stage];
	play_sound(sound_id, &ignored_2, p_write_index_address);
	game_mode = 3u;
	current_game_mode = 2u;
	stage_start_kind = 0u;
	if (game_in_progress == 0u)
	{
		load_stage_palette(p_palette_record, p_source, p_object);
		return;
	}
	cleared_count = cleared_stages;
	stages_past_2 = (uint8_t)(uint16_t)(cleared_count - 2u);
	if ((int16_t)cleared_count < 2)
		stages_past_2 = 0u;
	cabinet_event = 0x14u;
	if ((stages_past_2 & 1u) == 0u)
		cabinet_event = 0x15u;
	mem_write8(cabinet_events, cabinet_event);
	load_stage_palette(p_palette_record, p_source, p_object);
}

/* Game modes 3, 5 and 6, a frame of play: update_player_summary (on stage 0xB in a game it also closes
   joining, once); with no player left game_mode 7; then check_stage_clear, the grey screen, the scroll,
   tiles, floors, restart point, spawners and contacts, and run_stage_events. When game_mode changes it
   runs the new mode at once. */
void game_mode_play_stage(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint32_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t box_edge = 0, ignored = 0, speed = 0;

	current_game_mode = 3u;
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, p_count, p_list_cursor, &object);
	if (game_in_progress != 0u && joining_closed == 0u && stage == 0xBu)
	{
		joining_closed = 0xFFu;
		player_1_hud_message_shown = 0u;
		player_2_hud_message_shown = 0u;
		player_3_hud_message_shown = 0u;
	}
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, in_play_count, (uint16_t)next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	check_stage_clear(p_count, p_list_cursor, &object);
	if (game_mode != 3u)
	{
		run_game_mode(pressed, position, in_play_count, (uint16_t)next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	speed = update_grey_screen();
	update_scroll(&position, &speed, p_speed_value, &next_scroll_position, p_count, p_cycle, &ignored, &object);
	update_stage_tile_layers(&box_edge, &position, &speed, p_speed_value, &next_scroll_position, p_count, p_cycle, &ignored, &object);
	find_nearby_floor_blocks(&box_edge, &speed, p_speed_value, &next_scroll_position, p_count, &ignored, &object);
	continue_to_next_stage_slot(&box_edge, &ignored, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&box_edge, &position, &speed, p_speed_value, &next_scroll_position, p_count, &object);
	spawn_ring_rows(&box_edge, &position, &speed, &object);
	spawn_walls(&box_edge, &position, &next_scroll_position, p_count, p_cycle, &object);
	spawn_object_groups_2(&box_edge, p_count, &object);
	spawn_object_groups(&box_edge, &position, &speed, p_count, &object);
	spawn_objects_by_handler(&box_edge, &ignored, &object);
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &speed, p_speed_value, &next_scroll_position, p_count, &object);
	box_edge = check_walls(box_edge);
	check_platforms(&box_edge, &position, &speed, p_speed_value, &next_scroll_position, p_count, p_cycle, &ignored, &object);
	run_stage_events(&box_edge, p_list_cursor);
	if (game_mode != 3u)
	{
		run_game_mode(box_edge, position, speed, (uint16_t)next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
}

/* Game mode 1, the intro at a game's start (stage 7, how to play): set up once (stage_init_flags bit 13:
   contact lists and camera cleared, view at start section 6, map, goal, floors, spawns, palette,
   stage_start_sounds[stage]); then the stage frame without zones, until a player in play presses the
   button or game_mode_timer reaches 0x21C: game_mode 2. */
void game_mode_intro(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint8_t p2_status = 0, p3_status = 0;
	uint32_t intro_init_flags = 0;

	intro_init_flags = stage_init_flags;
	stage_init_flags = intro_init_flags | 0x2000u;
	if ((intro_init_flags & 0x2000u) == 0u)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		unused_game_byte = 0u;
		camera_ground_speed_vx = 0u;
		camera_ground_speed_vy = 0u;
		camera_slope_speed_vx = 0u;
		camera_slope_speed_vy = 0u;
		camera_slope_speed = 0u;
		camera_slope_direction = 0u;
		camera_scroll_direction = 0u;
		unused_camera_speed_word = 0u;
		player_dropping_in = 0u;
		player_dropping_in_before = 0u;
		grey_screen_active = 0u;
		grey_screen_ended = 0u;
		player_mishap = 0u;
		all_players_path_request = 0u;
		game_mode_timer = 0;
		players_pulled_word = 0u;
		stage_exit_started = 0u;
		stage_06_shake = 0u;
		start_stage_view(&ignored, &ignored_2, p_speed_value, &ignored_3);
		stage_start_section = 6u;
		move_camera_to_start_section(&position, &speed, &ignored_3);
		load_stage_map_pointers(&ignored_4, &ignored_5);
		load_stage_goal();
		find_nearby_floor_blocks(&ignored_4, &speed, p_speed_value, &ignored_3, p_count, &ignored_5, &ignored_6);
		start_spawns_by_type();
		load_stage_palette(p_cycle, &ignored_5, &object);
		current_game_mode = 1u;
		sound_id = stage_start_sounds[stage];
		play_sound(sound_id, &ignored_4, &position);
	}
	update_scroll(&position, &speed, p_speed_value, &ignored_3, p_count, p_cycle, &ignored_5, &object);
	update_stage_tile_layers(&ignored_4, &position, &speed, p_speed_value, &ignored_3, p_count, p_cycle, &ignored_5, &object);
	find_nearby_floor_blocks(&ignored_4, &speed, p_speed_value, &ignored_3, p_count, &ignored_5, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_4, &position, &speed, p_speed_value, &ignored_3, p_count, &object);
	spawn_objects_by_handler(&ignored_4, &ignored_5, &object);
	check_hits(&ignored_4, &position, &ignored_2, p_speed_value, &ignored_3, p_count, &ignored_6);
	check_walls(ignored_4);
	check_platforms(&ignored_4, &ignored, &ignored_2, p_speed_value, &ignored_3, p_count, p_cycle, &ignored_5, &ignored_6);
	*p_list_cursor = player_2_status_ADDRESS;
	if (player_status[0] != 0xFFu || (inputs_pressed & 0x10000u) == 0u)
	{
		p2_status = player_2_status;
		(*p_list_cursor)++;
		if (p2_status != 0xFFu || (inputs_pressed & 0x1000000u) == 0u)
		{
			p3_status = player_3_status;
			(*p_list_cursor)++;
			if (p3_status != 0xFFu || (inputs_pressed & 0x40000u) == 0u)
			{
				game_mode_timer++;
				if (game_mode_timer < 0x21C)
				{
					return;
				}
			}
		}
	}
	game_mode = 2u;
	system_mode_step = 0u;
}

/* Game mode 0xE, which nothing sets: CONTINUE ? (printed while continue_count is 9) and continue_count
   at text cell (24, 14), down 1 a second (a button press at once); after 1 the texts go and game_mode 7.
   A start pressed with credits (by cabinet_config) clears player_status and the contact lists, erases
   the texts and returns to saved_game_mode. */
void game_mode_continue(uint32_t position, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t speed_value = *p_speed_value;
	uint16_t attribute = 0, cell = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, text = 0, text_position = 0, value = 0;
	uint8_t old_continue_count = 0;

	if (continue_count == 9u)
	{
		text = continue_text_ADDRESS;
		print_text(&text, &ignored);
	}
	current_game_mode = 0xEu;
	ignored_2 = check_zones(0u);
	check_hits(&ignored_2, &position, &ignored_3, &speed_value, &ignored_4, p_count, &ignored);
	check_walls(ignored_2);
	check_platforms(&ignored_2, &ignored_5, &ignored_3, &speed_value, &ignored_4, p_count, p_cycle, p_list_cursor, &ignored);
	if ((inputs_pressed & 0x10000u) != 0u || (inputs_pressed & 0x1000000u) != 0u || (inputs_pressed & 0x40000u) != 0u)
		continue_second_frames = 0x3C;
	continue_second_frames++;
	if (continue_second_frames >= 0x3C)
	{
		continue_second_frames = 0;
		old_continue_count = continue_count;
		continue_count = old_continue_count - 1u;
		if ((int8_t)old_continue_count <= 1)
		{
			game_mode = 7u;
			game_mode_timer = 0;
			text = continue_text_ADDRESS;
			erase_text(text);
			cell = 0u;
			*p_list_cursor = continue_text_ADDRESS + 0x2u;
			text_position = TEXT_CELL(24, 14);
			erase_text_here(cell, p_list_cursor, &text_position);
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			return;
		}
	}
	value = continue_count;
	attribute = 0u;
	text_position = TEXT_CELL(24, 14);
	draw_hex_2(value, attribute, text_position);
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits == 0u || ((inputs_pressed & 0x10u) == 0u && (inputs_pressed & 0x20u) == 0u && (inputs_pressed & 0x80u) == 0u))
		{
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			return;
		}
		game_mode = saved_game_mode;
		player_status[0] = 0u;
		player_2_status = 0u;
		player_3_status = 0u;
		player_status_spare = 0xFFu;
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		text = continue_text_ADDRESS;
		erase_text(text);
		cell = 0u;
		*p_list_cursor = continue_text_ADDRESS + 0x2u;
		text_position = TEXT_CELL(24, 14);
		erase_text_here(cell, p_list_cursor, &text_position);
		*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
		return;
	case 2:
		if ((p2_credits == 0u || (inputs_pressed & 0x20u) == 0u) && (p1_credits == 0u || (inputs_pressed & 0x10u) == 0u))
		{
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			return;
		}
		game_mode = saved_game_mode;
		player_status[0] = 0u;
		player_2_status = 0u;
		player_3_status = 0u;
		player_status_spare = 0xFFu;
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		text = continue_text_ADDRESS;
		erase_text(text);
		cell = 0u;
		*p_list_cursor = continue_text_ADDRESS + 0x2u;
		text_position = TEXT_CELL(24, 14);
		erase_text_here(cell, p_list_cursor, &text_position);
		*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
		return;
	case 3:
		if (p3_credits == 0u || (inputs_pressed & 0x80u) == 0u)
		{
			if ((p2_credits != 0u && (inputs_pressed & 0x20u) != 0u) || (p1_credits != 0u && (inputs_pressed & 0x10u) != 0u))
			{
				game_mode = saved_game_mode;
				player_status[0] = 0u;
				player_2_status = 0u;
				player_3_status = 0u;
				player_status_spare = 0xFFu;
				hitter_list = 0xFFFFu;
				touchable_list = 0xFFFFu;
				wall_list = 0xFFFFu;
				rider_list = 0xFFFFu;
				platform_list = 0xFFFFu;
				zone_list = 0xFFFFu;
				text = continue_text_ADDRESS;
				erase_text(text);
				cell = 0u;
				*p_list_cursor = continue_text_ADDRESS + 0x2u;
				text_position = TEXT_CELL(24, 14);
				erase_text_here(cell, p_list_cursor, &text_position);
			}
			*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
			return;
		}
		game_mode = saved_game_mode;
		player_status[0] = 0u;
		player_2_status = 0u;
		player_3_status = 0u;
		player_status_spare = 0xFFu;
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		text = continue_text_ADDRESS;
		erase_text(text);
		cell = 0u;
		*p_list_cursor = continue_text_ADDRESS + 0x2u;
		text_position = TEXT_CELL(24, 14);
		erase_text_here(cell, p_list_cursor, &text_position);
		*p_speed_value = (*p_speed_value & 0xFFFF0000u) | (speed_value & 0xFFFFu);
		return;
	default:
		invalid_jump(0xBDCu + cabinet_config * 2u);
	}
}

/* Game mode 4, between stages: on its first frame sounds 0x95 and 3; clears mixer_blend bit 11 (the
   blending), runs the island map screen (run_map_screen, map_screen_active set every frame) and starts
   the next stage (game_mode 2) once map_screen_step reaches 5. Hands back run_map_screen's bottom_speed. */
uint32_t game_mode_stage_end_fade(uint32_t bottom_speed, uint32_t object)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if (current_game_mode != 4u)
	{
		sound_id = 0x95u;
		play_sound(sound_id, &ignored, &ignored_2);
		sound_id = 3u;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	current_game_mode = 4u;
	mixer_blend &= 0xF7FFu;
	bottom_speed = run_map_screen(bottom_speed, object);
	map_screen_active = 0xFFu;
	if (map_screen_step == 5u)
		game_mode = 2u;
	return bottom_speed;
}

/* Game mode 0x11, the first event of stages 3, 4, 0xA, 0xC and 0x10: the stage frame (the scroll held on
   stage 4) while update_screen_wipe opens the screen's windows, then game_mode 3 once edges_done shows
   all four edges in place (0xF). */
void game_mode_screen_wipe(uint32_t position, uint32_t speed, uint32_t edges_done, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	current_game_mode = 0x11u;
	if (stage == 4u)
		scroll_mode = 1u;
	update_scroll(&position, &speed, p_speed_value, &edges_done, p_count, p_cycle, &ignored, &object);
	update_stage_tile_layers(&ignored_2, &position, &speed, p_speed_value, &edges_done, p_count, p_cycle, &ignored, &object);
	find_nearby_floor_blocks(&ignored_2, &speed, p_speed_value, &edges_done, p_count, &ignored, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_2, &position, &speed, p_speed_value, &edges_done, p_count, &object);
	spawn_ring_rows(&ignored_2, &position, &ignored_3, &object);
	spawn_walls(&ignored_2, &position, &edges_done, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored_2, p_count, &object);
	spawn_object_groups(&ignored_2, &position, &ignored_3, p_count, &object);
	spawn_objects_by_handler(&ignored_2, &ignored, &object);
	ignored_2 = check_zones(ignored_2);
	check_hits(&ignored_2, &position, &ignored_3, p_speed_value, &edges_done, p_count, &ignored_4);
	check_walls(ignored_2);
	check_platforms(&ignored_2, &ignored_5, &ignored_3, p_speed_value, &edges_done, p_count, p_cycle, p_list_cursor, &ignored_4);
	update_screen_wipe(&ignored_2, &ignored_5, &ignored_3, &edges_done);
	if ((edges_done & 0xFu) != 0xFu)
	{
		return;
	}
	game_mode = 3u;
}

/* Game mode 0x17, a stage cleared: once, stage_clear_timer 0x14A and two banners, the stage's name
   (stage_name_banners[stage], stage_clear_banner_reveal) and CLEARED (cleared_banner_wait), with sounds
   0x94 and 0x7B; banner_break from timer 0x78. The stage frame goes on; when the timer runs out, the
   results (game_mode 0x1D). */
void game_mode_stage_clear(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t sound_id = 0;
	uint32_t banner_record = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint16_t old_clear_timer = 0;
	uint32_t stage_clear_init_flags = 0;

	stage_clear_init_flags = stage_init_flags;
	stage_init_flags = stage_clear_init_flags | 0x4000u;
	if ((stage_clear_init_flags & 0x4000u) == 0u)
	{
		stage_name_shown = 0u;
		cleared_shown = 0u;
		banner_break = 0u;
		stage_clear_timer = 0x14Au;
		stage_exit_started = 0u;
		allocate_object_outside_loop(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->sprite_flags = 0x1002u;
			OBJECT(object)->sprite_palette = 0x89A0u;
			OBJECT(object)->handler = stage_clear_banner_reveal_ADDRESS;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->stage_clear_banner.sparkle_offset = 0x10u;
			banner_record = stage_name_banners[(uint8_t)stage] + stage_name_banners_ADDRESS;
			OBJECT(object)->screen_x = STAGE_NAME_BANNER(banner_record)->screen_x;
			banner_record += 2u;
			OBJECT(object)->screen_y = STAGE_NAME_BANNER(banner_record - 0x2u)->screen_y;
			OBJECT(object)->animation = banner_record + 2u;
			OBJECT(object)->animation_frame = 0xFFu;
			OBJECT(object)->animation_speed = 4u;
			OBJECT(object)->animation_timer = 4;
			allocate_object_outside_loop(&free_count, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1002u;
				OBJECT(object)->sprite_palette = 0x89A0u;
				OBJECT(object)->handler = cleared_banner_wait_ADDRESS;
				OBJECT(object)->flags = 1u;
				OBJECT(object)->stage_clear_banner.sparkle_offset = 0x10u;
				OBJECT(object)->screen_x = cleared_banner_x;
				OBJECT(object)->screen_y = cleared_banner_y;
				OBJECT(object)->animation = cleared_banner_animation_ADDRESS;
				OBJECT(object)->animation_frame = 0xFFu;
				OBJECT(object)->animation_speed = 4u;
				OBJECT(object)->animation_timer = 4;
				sound_id = 0x94u;
				play_sound(sound_id, &ignored, &position);
				sound_id = 0x7Bu;
				play_sound(sound_id, &ignored, &position);
			}
		}
	}
	if ((int16_t)stage_clear_timer <= 0x78)
		banner_break = 0xFFu;
	current_game_mode = 0x17u;
	next_stage_slot_request = 0u;
	update_scroll(&position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
	update_stage_tile_layers(&ignored, &position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
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
	check_platforms(&ignored, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
	old_clear_timer = stage_clear_timer;
	stage_clear_timer = old_clear_timer - 1u;
	if ((int16_t)old_clear_timer > 1)
	{
		return;
	}
	game_mode = 0x1Du;
}

/* Game mode 7, game over: on its first frame sounds 0x94 and 0x73 (0x76 on stage 0x11) and
   cabinet_events 0x1F; then the GAME OVER banner (its text after the ending) with the contacts running;
   from 0x78 frames back to the attract screens (game_in_progress 0, blending and colour offsets off). A
   start with enough credits begins a new game. */
void game_mode_game_over(uint32_t box_edge, uint32_t position, uint16_t scale, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t credits_needed = 0, sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, record = 0;
	uint8_t game_over_cnt_bits = 0;

	if ((uint16_t)game_mode_timer == 0u)
	{
		sound_id = 0x94u;
		queue_sound(sound_id, &ignored, &ignored_2);
		sound_id = 0x73u;
		if (stage == 0x11u)
			sound_id = 0x76u;
		queue_sound(sound_id, &box_edge, &position);
		joining_closed = 0u;
		character_select_timer = 0;
		mem_write8(cabinet_events, 0x1Fu);
	}
	current_game_mode = 7u;
	if (ending_finished == 0u)
		draw_game_over_banner();
	else
	{
		record = game_over_string_ADDRESS;
		queue_string_record(record, (uint16_t)box_edge, scale, &position, p_speed_value);
	}
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &ignored_3, p_speed_value, &ignored_4, p_count, &ignored_5);
	check_walls(box_edge);
	check_platforms(&ignored, &ignored_2, &ignored_3, p_speed_value, &ignored_4, p_count, p_cycle, p_list_cursor, &ignored_5);
	game_mode_timer++;
	if (game_mode_timer >= 0x78)
	{
		game_in_progress = 0u;
		attract_step = 0u;
		demo_prompt_shown = 0u;
		stage_init_flags &= ~0x8000u;
		advance_attract_screen();
		display_off_frames = 2u;
		game_over_cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, game_over_cnt_bits & ~2u);
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		color_offset_red = 0u;
		color_offset_green = 0u;
		color_offset_blue = 0u;
	}
	credits_needed = (uint8_t)credits_per_start;
	if (free_play != 0u)
		credits_needed = 0u;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)credits_needed || ((inputs_pressed & 0x10u) == 0u && (inputs_pressed & 0x20u) == 0u && (inputs_pressed & 0x80u) == 0u))
		{
			return;
		}
		system_mode = 6u;
		game_mode = 0u;
		system_mode_step = 0u;
		game_mode_start_game(p_count, p_list_cursor);
		return;
	case 2:
		if ((p2_credits < (uint8_t)credits_needed || (inputs_pressed & 0x20u) == 0u) && (p1_credits < (uint8_t)credits_needed || (inputs_pressed & 0x10u) == 0u))
		{
			return;
		}
		system_mode = 6u;
		game_mode = 0u;
		system_mode_step = 0u;
		game_mode_start_game(p_count, p_list_cursor);
		return;
	case 3:
		if ((uint8_t)credits_needed <= p3_credits && (inputs_pressed & 0x80u) != 0u)
		{
			system_mode = 6u;
			game_mode = 0u;
			system_mode_step = 0u;
			game_mode_start_game(p_count, p_list_cursor);
			return;
		}
		if ((p2_credits < (uint8_t)credits_needed || (inputs_pressed & 0x20u) == 0u) && (p1_credits < (uint8_t)credits_needed || (inputs_pressed & 0x10u) == 0u))
		{
			return;
		}
		system_mode = 6u;
		game_mode = 0u;
		system_mode_step = 0u;
		game_mode_start_game(p_count, p_list_cursor);
		return;
	default:
		invalid_jump(0xF8Fu + cabinet_config * 2u);
	}
}

/* Game mode 0x1B, choosing characters at a game's start: set up once (stage_init_flags bit 16: contact
   lists, camera and characters_taken cleared; the stage's view, map pointers, goal, spawns and palette;
   cabinet_events 0x15), then the spawners and contacts each frame; the intro (game_mode 1) after at
   least 3 frames, once character_select_timer is 0. */
void game_mode_character_select(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint32_t character_select_init_flags = 0;

	character_select_init_flags = stage_init_flags;
	stage_init_flags = character_select_init_flags | 0x10000u;
	if ((character_select_init_flags & 0x10000u) == 0u)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		platform_list = 0xFFFFu;
		rider_list = 0xFFFFu;
		wall_list = 0xFFFFu;
		zone_list = 0xFFFFu;
		unused_game_byte = 0u;
		camera_ground_speed_vx = 0u;
		camera_ground_speed_vy = 0u;
		camera_slope_speed_vx = 0u;
		camera_slope_speed_vy = 0u;
		camera_slope_speed = 0u;
		camera_slope_direction = 0u;
		camera_scroll_direction = 0u;
		unused_camera_speed_word = 0u;
		player_dropping_in = 0u;
		player_dropping_in_before = 0u;
		grey_screen_active = 0u;
		grey_screen_ended = 0u;
		player_mishap = 0u;
		all_players_path_request = 0u;
		game_mode_timer = 0;
		players_pulled_word = 0u;
		stage_exit_started = 0u;
		stage_06_shake = 0u;
		nearby_floor_block_count = 0u;
		characters_taken = 0u;
		start_stage_view(&position, &speed, p_speed_value, &ignored);
		load_stage_map_pointers(&ignored_2, &ignored_3);
		load_stage_goal();
		start_spawns_by_type();
		load_stage_palette(p_cycle, &ignored_3, &object);
		current_game_mode = 0x1Bu;
		mem_write8(cabinet_events, 0x15u);
	}
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_2, &position, &speed, p_speed_value, &ignored, p_count, &object);
	spawn_objects_by_handler(&ignored_2, &ignored_3, &object);
	check_hits(&ignored_2, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_2);
	check_platforms(&ignored_2, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
	game_mode_timer++;
	if ((uint16_t)game_mode_timer < 3u || (uint16_t)character_select_timer != 0u)
	{
		return;
	}
	game_mode = 1u;
	system_mode_step = 0u;
}

/* Game mode 0x1D, the stage's results: once, game_mode_timer 0x1E0 and the ring tally
   (start_ring_tally); the stage frame goes on until stage_results_done is set, then game_mode 4 (the map
   screen). */
void game_mode_stage_results(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint32_t stage_results_init_flags = 0;

	stage_results_init_flags = stage_init_flags;
	stage_init_flags = stage_results_init_flags | 0x20000u;
	if ((stage_results_init_flags & 0x20000u) == 0u)
	{
		game_mode_timer = 0x1E0;
		start_ring_tally(&position, &speed, p_speed_value, p_count, p_cycle, &object);
	}
	current_game_mode = 0x1Du;
	next_stage_slot_request = 0u;
	update_scroll(&position, &speed, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	update_stage_tile_layers(&ignored_3, &position, &speed, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	find_nearby_floor_blocks(&ignored_3, &speed, p_speed_value, &ignored, p_count, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_3, &position, &speed, p_speed_value, &ignored, p_count, &object);
	spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
	spawn_walls(&ignored_3, &position, &ignored, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored_3, p_count, &object);
	spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
	spawn_objects_by_handler(&ignored_3, &ignored_2, &object);
	ignored_3 = check_zones(ignored_3);
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
	if (stage_results_done == 0u)
	{
		return;
	}
	game_mode = 4u;
	game_mode_timer = 0;
}

/* Game mode 0x1E (check_stage_clear on stages 0xB-0xF): fades to black (update_fade) with the stage
   frame going on, then starts the next stage (start_stage) and fades it in from black (game_mode_timer
   -1) before game_mode 3; on stage 0x11 it goes to game_mode 3 straight after start_stage. The layers'
   colour offsets are on during the fades. */
void game_mode_stage_change_fade(uint32_t delta, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	if ((uint16_t)game_mode_timer == 0u)
	{
		game_mode_timer = 1;
		fade_step = 0u;
		mixer_text_blend |= 0x4000u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_bitmap_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
	}
	if (game_mode_timer < 0)
	{
		fade_kind = 2;
		delta = update_fade(delta);
		if (fade_kind >= 0)
		{
			current_game_mode = 0x1Eu;
			next_stage_slot_request = 0u;
			update_player_summary(&in_play_count, p_count, &ignored, &object);
			update_scroll(&position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
			update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
			find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored_2, p_count, &ignored, &object);
			continue_to_next_stage_slot(&ignored_3, &ignored, &object);
			*p_speed_value = update_restart_point(*p_speed_value);
			spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored_2, p_count, &object);
			spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
			spawn_walls(&ignored_3, &position, &ignored_2, p_count, p_cycle, &object);
			spawn_object_groups_2(&ignored_3, p_count, &object);
			spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
			spawn_objects_by_handler(&ignored_3, &ignored, &object);
			ignored_3 = check_zones(ignored_3);
			check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
			check_walls(ignored_3);
			check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
			return;
		}
		game_mode = 3u;
		mixer_text_blend &= 0xBFFFu;
		mixer_nbg0_blend &= 0xBFFFu;
		mixer_nbg1_blend &= 0xBFFFu;
		mixer_nbg2_blend &= 0xBFFFu;
		mixer_nbg3_blend &= 0xBFFFu;
		mixer_bitmap_blend &= 0xBFFFu;
		mixer_sprite_control &= ~0x8000u;
		run_game_mode(delta, position, in_play_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	fade_kind = 0;
	update_fade(delta);
	if (fade_kind >= 0)
	{
		current_game_mode = 0x1Eu;
		next_stage_slot_request = 0u;
		update_player_summary(&in_play_count, p_count, &ignored, &object);
		update_scroll(&position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored_2, p_count, &ignored, &object);
		continue_to_next_stage_slot(&ignored_3, &ignored, &object);
		*p_speed_value = update_restart_point(*p_speed_value);
		spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored_2, p_count, &object);
		spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
		spawn_walls(&ignored_3, &position, &ignored_2, p_count, p_cycle, &object);
		spawn_object_groups_2(&ignored_3, p_count, &object);
		spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
		spawn_objects_by_handler(&ignored_3, &ignored, &object);
		ignored_3 = check_zones(ignored_3);
		check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
		check_walls(ignored_3);
		check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
		return;
	}
	start_stage(next_scroll_position, &position, &in_play_count, p_speed_value, p_count, p_cycle, p_list_cursor, &object);
	game_mode = 0x1Eu;
	game_mode_timer = -1;
	fade_step = 0u;
	mixer_text_blend |= 0x4000u;
	mixer_nbg0_blend |= 0x4000u;
	mixer_nbg1_blend |= 0x4000u;
	mixer_nbg2_blend |= 0x4000u;
	mixer_nbg3_blend |= 0x4000u;
	mixer_bitmap_blend |= 0x4000u;
	mixer_sprite_control |= 0x8000u;
	if (stage != 0x11u)
	{
		current_game_mode = 0x1Eu;
		next_stage_slot_request = 0u;
		update_player_summary(&in_play_count, p_count, &ignored, &object);
		update_scroll(&position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored, &object);
		find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored_2, p_count, &ignored, &object);
		continue_to_next_stage_slot(&ignored_3, &ignored, &object);
		*p_speed_value = update_restart_point(*p_speed_value);
		spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored_2, p_count, &object);
		spawn_ring_rows(&ignored_3, &position, &ignored_4, &object);
		spawn_walls(&ignored_3, &position, &ignored_2, p_count, p_cycle, &object);
		spawn_object_groups_2(&ignored_3, p_count, &object);
		spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &object);
		spawn_objects_by_handler(&ignored_3, &ignored, &object);
		ignored_3 = check_zones(ignored_3);
		check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
		check_walls(ignored_3);
		check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
		return;
	}
	game_mode = 3u;
}

/* Game mode 0x20, YOU FAILED (from the stage 0xB escape): once, sound 0x94 and the colour offsets on;
   they rise to 0x1F (white) a step every 4 frames while the stage frame runs without the scroll; then
   game_mode_timer counts: sound 0x85 at 2, the YOU FAILED text from 0x78, you_failed_done at 0x12C;
   game_mode 7 once no player is left. */
void game_mode_you_failed(uint32_t box_edge, uint32_t position, uint32_t in_play_count, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, record = 0;

	if ((uint16_t)game_mode_timer == 0u)
	{
		game_mode_timer++;
		current_game_mode = 0x20u;
		you_failed_done = 0u;
		you_failed_fade_red = 0u;
		you_failed_fade_green = 0u;
		you_failed_fade_blue = 0u;
		color_offset_red = 0u;
		color_offset_green = 0u;
		color_offset_blue = 0u;
		mixer_nbg0_blend |= 0x4000u;
		mixer_nbg1_blend |= 0x4000u;
		mixer_nbg2_blend |= 0x4000u;
		mixer_nbg3_blend |= 0x4000u;
		mixer_sprite_control |= 0x8000u;
		sound_id = 0x94u;
		queue_sound(sound_id, &box_edge, &position);
	}
	next_stage_slot_request = 0u;
	update_player_summary(&in_play_count, p_count, &ignored, &object);
	find_nearby_floor_blocks(&box_edge, &in_play_count, p_speed_value, &ignored_2, p_count, &ignored, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&box_edge, &position, &in_play_count, p_speed_value, &ignored_2, p_count, &object);
	spawn_ring_rows(&box_edge, &position, &in_play_count, &object);
	spawn_walls(&box_edge, &position, &ignored_2, p_count, p_cycle, &object);
	spawn_object_groups_2(&box_edge, p_count, &object);
	spawn_object_groups(&box_edge, &position, &in_play_count, p_count, &object);
	spawn_objects_by_handler(&box_edge, &ignored, &object);
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &in_play_count, p_speed_value, &ignored_2, p_count, &ignored_3);
	box_edge = check_walls(box_edge);
	check_platforms(&box_edge, &position, &in_play_count, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_3);
	if ((int16_t)color_offset_red < 0x1F)
	{
		you_failed_fade_red++;
		color_offset_red = you_failed_fade_red >> 2;
	}
	if ((int16_t)color_offset_green < 0x1F)
	{
		you_failed_fade_green++;
		color_offset_green = you_failed_fade_green >> 2;
	}
	if ((int16_t)color_offset_blue < 0x1F)
	{
		you_failed_fade_blue++;
		color_offset_blue = you_failed_fade_blue >> 2;
	}
	if (you_failed_done == 0u && (int16_t)color_offset_red >= 0x1F && (int16_t)color_offset_green >= 0x1F && (int16_t)color_offset_blue >= 0x1F)
		game_mode_timer++;
	if ((uint16_t)game_mode_timer == 2u)
	{
		sound_id = 0x85u;
		play_sound(sound_id, &box_edge, &position);
	}
	if (game_mode_timer >= 0x78)
	{
		record = you_failed_string_ADDRESS;
		queue_string_record(record, (uint16_t)box_edge, in_play_count, &position, p_speed_value);
		mixer_sprite_control &= ~0x8000u;
	}
	if (game_mode_timer >= 0x12C)
		you_failed_done = 0xFFu;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
}

/* Unused: while the service button is held (input_ports_active bit 0), player 1's button pressed sets
   game_mode 8, the debug stage select (written as the halfword game_mode_word). */
void check_stage_select_buttons(void)
{
	if ((input_ports_active & 1u) != 0u && (inputs_pressed & 0x10000u) != 0u)
		game_mode_word = 8u;
}

/* Game mode 8, the debug stage select (only the unused check_stage_select_buttons sets it): once, stage
   3, its texts, and video RAM from 0xE000 cleared; player 1's trackball steps stage through 0-0x11,
   drawn at text cell (24, 21); start 1 takes cleared_stages from stage_select_cleared_values[stage] and
   starts it (game_mode 2, stage_start_kind 1). */
void game_mode_stage_select(uint32_t *p_count, uint32_t *p_list_cursor)
{
	uint16_t cell = 0;
	uint32_t attribute = 0, clear_dest = 0, fill_word = 0, ignored = 0, new_stage = 0, player_number = 0, text_position = 0, value = 0;
	uint32_t stage_select_init_flags = 0;

	stage_select_init_flags = stage_init_flags;
	stage_init_flags = stage_select_init_flags | 0x40u;
	if ((stage_select_init_flags & 0x40u) == 0u)
	{
		stage = 3u;
		*p_list_cursor = stage_select_mode_text_ADDRESS;
		text_position = TEXT_CELL(14, 20);
		print_text_here(&ignored, p_list_cursor, &text_position);
		*p_list_cursor = stage_number_text_ADDRESS;
		text_position = TEXT_CELL(18, 21);
		print_text_here(&ignored, p_list_cursor, &text_position);
		player_number = 0u;
		reset_trackball_step(player_number);
		clear_dest = VIDEO_RAM + 0xE000u;
		*p_count = 0xE00u;
		fill_word = 0u;
		do
		{
			mem_write32(clear_dest, fill_word);
			clear_dest += 4u;
			mem_write32(clear_dest, fill_word);
			clear_dest += 4u;
			mem_write32(clear_dest, fill_word);
			clear_dest += 4u;
			mem_write32(clear_dest, fill_word);
			(*p_count)--;
			clear_dest += 4u;
		} while (*p_count != 0u);
	}
	new_stage = 0u;
	read_trackball_step(&new_stage, p_list_cursor);
	if ((uint16_t)new_stage == 0u)
	{
		value = stage;
		attribute = 0u;
		text_position = TEXT_CELL(24, 21);
		draw_hex_4(&value, &attribute, &text_position);
		if ((inputs_pressed & 0x10u) == 0u)
		{
			return;
		}
		mem_write16(cleared_stages_ADDRESS, stage_select_cleared_values[stage]);
		game_mode = 2u;
		stage_init_flags &= ~0x40u;
		stage_start_kind = 1u;
		*p_list_cursor = stage_select_mode_text_ADDRESS;
		text_position = TEXT_CELL(14, 20);
		cell = 0u;
		erase_text_here(cell, p_list_cursor, &text_position);
		*p_list_cursor = stage_number_text_ADDRESS;
		text_position = TEXT_CELL(18, 21);
		erase_text_here(cell, p_list_cursor, &text_position);
		*p_list_cursor = stage_number_text_ADDRESS;
		text_position = TEXT_CELL(24, 21);
		erase_text_here(cell, p_list_cursor, &text_position);
		return;
	}
	new_stage = (uint16_t)(new_stage + stage);
	if ((int16_t)new_stage < 0)
		new_stage = 0x11u;
	if ((int16_t)new_stage > 0x11)
		new_stage = 0u;
	stage = new_stage;
	value = stage;
	attribute = 0u;
	text_position = TEXT_CELL(24, 21);
	draw_hex_4(&value, &attribute, &text_position);
	if ((inputs_pressed & 0x10u) == 0u)
	{
		return;
	}
	mem_write16(cleared_stages_ADDRESS, stage_select_cleared_values[stage]);
	game_mode = 2u;
	stage_init_flags &= ~0x40u;
	stage_start_kind = 1u;
	*p_list_cursor = stage_select_mode_text_ADDRESS;
	text_position = TEXT_CELL(14, 20);
	cell = 0u;
	erase_text_here(cell, p_list_cursor, &text_position);
	*p_list_cursor = stage_number_text_ADDRESS;
	text_position = TEXT_CELL(18, 21);
	erase_text_here(cell, p_list_cursor, &text_position);
	*p_list_cursor = stage_number_text_ADDRESS;
	text_position = TEXT_CELL(24, 21);
	erase_text_here(cell, p_list_cursor, &text_position);
}

/* Game mode 9, which no code or stage event sets: only the scroll, the tile layers,
   continue_to_next_stage_slot and the nearby floor blocks; nothing changes the mode. */
void game_mode_event_09(uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	update_scroll(&ignored, &speed, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &object);
	update_stage_tile_layers(&ignored_3, &ignored, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &object);
	continue_to_next_stage_slot(&ignored_3, p_list_cursor, &ignored_5);
	find_nearby_floor_blocks(&ignored_3, &ignored_4, p_speed_value, &ignored_2, p_count, p_list_cursor, &ignored_5);
}

/* Game modes 0xA and 0xB (only 0xA is ever set: stage 5's event at scroll 0x100): with a player left,
   stage 5's switch event (update_stage_05_event); with none, game_mode 7 at once, game_mode saved. */
void game_mode_event_0A(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	current_game_mode = 0xAu;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) != 0u)
	{
		update_stage_05_event(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	saved_game_mode = game_mode;
	game_mode = 7u;
	game_mode_timer = 0;
	run_game_mode(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
}

/* Game mode 0xC, stage 4's event at scroll 0: by scene_step event_0c_start (also for step 4),
   event_0c_first_quake, event_0c_second_quake, event_0c_finish; then the scroll, tiles, restart point
   and the hit, wall and platform checks (no spawners). */
void game_mode_event_0C(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	current_game_mode = 0xCu;
	switch (scene_step)
	{
	case 0:
		event_0c_start(&position, &speed, &object);
		break;
	case 1:
		position = event_0c_first_quake(position);
		break;
	case 2:
		event_0c_second_quake(&position, p_count, p_cycle, &object);
		break;
	case 3:
		event_0c_finish();
		break;
	case 4:
		event_0c_start(&position, &speed, &object);
		break;
	default:
		invalid_jump(0x9487Cu + scene_step * 3u);
	}
	update_scroll(&position, &speed, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	update_stage_tile_layers(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
}

/* Game mode 0xD, stage 8's event at scroll 0: the players frozen (players_frozen, event_0d_active) for
   0xB4 frames (scene_timer), then the scroll runs at speed 0x1C; with the stage frame going on (no
   zones), at scroll_position 0xA00 game_mode 3, the players free and scroll mode 7. */
void game_mode_event_0D(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint16_t event_0d_timer = 0;

	current_game_mode = 0xDu;
	switch (scene_step)
	{
	case 0:
	case 3:
		scene_timer = 0xB4;
		event_0d_active = 0xFFu;
		players_frozen = 0xFFu;
		scene_step++;
		break;
	case 1:
	case 2:
		break;
	default:
		invalid_jump(0x946A8u + scene_step * 2u);
	}
	if (scene_step != 2u)
	{
		event_0d_timer = scene_timer;
		scene_timer = event_0d_timer - 1u;
		if ((int16_t)event_0d_timer > 1)
		{
			find_nearby_floor_blocks(&ignored, &speed, p_speed_value, &ignored_2, p_count, &ignored_3, &object);
			update_scroll(&position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
			update_stage_tile_layers(&ignored, &position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
			spawn_objects_by_type(&ignored, &position, &speed, p_speed_value, &ignored_2, p_count, &object);
			spawn_ring_rows(&ignored, &position, &ignored_4, &object);
			spawn_walls(&ignored, &position, &ignored_2, p_count, p_cycle, &object);
			spawn_object_groups_2(&ignored, p_count, &object);
			spawn_object_groups(&ignored, &position, &ignored_4, p_count, &object);
			spawn_objects_by_handler(&ignored, &ignored_3, &object);
			check_hits(&ignored, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
			check_walls(ignored);
			check_platforms(&ignored, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
			if (scroll_position < 0xA00u)
			{
				return;
			}
			game_mode = 3u;
			event_0d_active = 0u;
			players_frozen = 0u;
			scroll_mode = 7u;
			return;
		}
		scene_step++;
	}
	scroll_mode = 0u;
	scroll_speed = 0x1Cu;
	find_nearby_floor_blocks(&ignored, &speed, p_speed_value, &ignored_2, p_count, &ignored_3, &object);
	update_scroll(&position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
	update_stage_tile_layers(&ignored, &position, &speed, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
	spawn_objects_by_type(&ignored, &position, &speed, p_speed_value, &ignored_2, p_count, &object);
	spawn_ring_rows(&ignored, &position, &ignored_4, &object);
	spawn_walls(&ignored, &position, &ignored_2, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored, p_count, &object);
	spawn_object_groups(&ignored, &position, &ignored_4, p_count, &object);
	spawn_objects_by_handler(&ignored, &ignored_3, &object);
	check_hits(&ignored, &position, &ignored_4, p_speed_value, &ignored_2, p_count, &ignored_5);
	check_walls(ignored);
	check_platforms(&ignored, &ignored_6, &ignored_4, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_5);
	if (scroll_position < 0xA00u)
	{
		return;
	}
	game_mode = 3u;
	event_0d_active = 0u;
	players_frozen = 0u;
	scroll_mode = 7u;
}

/* Game mode 0x1A, stage 8's event at scroll 0x3100: set up on scene_step 0, then each
   event_1a_scroll_script entry gives the scroll its mode and speed once scroll_position reaches its
   position (an 0xFFFF one: once seesaw_launched is set and scene_timer runs out). The stage frame; game
   over with no player left; at scroll 0x6080 game_mode 3, scroll mode 7, speed 5. */
void game_mode_event_1A(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, scroll_script = 0;
	uint16_t event_1a_timer = 0;

	current_game_mode = 0x1Au;
	if (scene_step == 0u)
	{
		scene_step++;
		player_1_sequence_step = 0u;
		player_2_sequence_step = 0u;
		player_3_sequence_step = 0u;
		event_1a_scroll_mode = event_1a_first_scroll_mode;
		event_1a_scroll_speed = event_1a_first_scroll_speed;
		seesaw_launched = 0u;
		scene_timer = 0x1E;
		scene_object = 0xFFFFu;
		object = event_1a_scroll_script_step_1_ADDRESS;
	}
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
	scroll_script = event_1a_scroll_script_ADDRESS + scene_step * 3u * 2u;
	if (scroll_position < EVENT_1A_SCROLL_STEP(scroll_script)->position)
	{
		if ((int16_t)EVENT_1A_SCROLL_STEP(scroll_script)->position < 0 && seesaw_launched != 0u)
		{
			event_1a_timer = scene_timer;
			scene_timer = event_1a_timer - 1u;
			if ((int16_t)event_1a_timer <= 1)
			{
				scene_timer = 0;
				scroll_script += 2u;
				event_1a_scroll_mode = EVENT_1A_SCROLL_STEP(scroll_script - 0x2u)->mode;
				scroll_script += 2u;
				event_1a_scroll_speed = EVENT_1A_SCROLL_STEP(scroll_script - 0x4u)->speed;
				scene_step++;
				scroll_script += 2u;
			}
		}
	}
	else
	{
		scroll_script += 2u;
		event_1a_scroll_mode = EVENT_1A_SCROLL_STEP(scroll_script - 0x2u)->mode;
		scroll_script += 2u;
		event_1a_scroll_speed = EVENT_1A_SCROLL_STEP(scroll_script - 0x4u)->speed;
		scene_step++;
		scroll_script += 2u;
	}
	scroll_mode = event_1a_scroll_mode;
	scroll_speed = event_1a_scroll_speed;
	update_scroll(&position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &scroll_script);
	update_stage_tile_layers(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &scroll_script);
	find_nearby_floor_blocks(&ignored_3, &in_play_count, p_speed_value, &ignored, p_count, &ignored_2, &scroll_script);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_3, &position, &in_play_count, p_speed_value, &ignored, p_count, &scroll_script);
	spawn_ring_rows(&ignored_3, &position, &ignored_4, &scroll_script);
	spawn_walls(&ignored_3, &position, &ignored, p_count, p_cycle, &scroll_script);
	spawn_object_groups_2(&ignored_3, p_count, &scroll_script);
	spawn_object_groups(&ignored_3, &position, &ignored_4, p_count, &scroll_script);
	spawn_objects_by_handler(&ignored_3, &ignored_2, &scroll_script);
	ignored_3 = check_zones(ignored_3);
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
	if (scroll_position < 0x6080u)
	{
		return;
	}
	game_mode = 3u;
	scroll_speed_low = 5;
	scroll_mode_low = 7u;
}

/* Game mode 0xF, which no code or stage event sets: by scene_step event_0f_start (also for step 3),
   event_0f_stop_scroll, event_0f_show_messages; then the scroll, tiles, restart point, spawn_walls and
   the hit, wall and platform checks. */
void game_mode_event_0F(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	current_game_mode = 0xFu;
	switch (scene_step)
	{
	case 0:
		event_0f_start();
		break;
	case 1:
		event_0f_stop_scroll();
		break;
	case 2:
		event_0f_show_messages(p_count, &object);
		break;
	case 3:
		event_0f_start();
		break;
	default:
		invalid_jump(0x94606u + scene_step * 3u);
	}
	update_scroll(&position, &speed, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	update_stage_tile_layers(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_walls(&ignored_3, &position, &ignored, p_count, p_cycle, &object);
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &ignored_5);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_6, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_5);
}

/* Game mode 0x10, stage 6's opening scene (the first of a game): by scene_step, the opening objects and
   update_stage_06_background for 0x1A4 frames (background step 3 at 0x168; the colour offsets down 1
   every 8 frames from 0xF0 to 0x12B), then background step 5, the offsets reset and 0x12C frames of
   lightning (spawn_stage_06_lightning); then game_mode 2. */
void game_mode_event_10(uint32_t random_value, uint32_t object, uint32_t *p_rolls_left, uint32_t *p_words_left, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0;

	current_game_mode = 0x10u;
	switch (scene_step)
	{
	case 0:
	case 4:
		scene_step++;
		scene_timer = 0;
		scene_timer++;
		spawn_objects_by_handler(&ignored, p_list_cursor, &object);
		if ((uint16_t)scene_timer == 0x168u)
			stage_06_background_step = 3u;
		update_stage_06_background(random_value, object, p_rolls_left, p_words_left, p_list_cursor);
		if (scene_timer >= 0xF0 && scene_timer <= 0x12B && (scene_timer & 7u) == 0u)
		{
			mixer_sprite_control |= 0x8000u;
			color_offset_red--;
			color_offset_green--;
			color_offset_blue--;
		}
		if (scene_timer < 0x1A4)
		{
			return;
		}
		scene_step++;
		stage_06_background_step = 5u;
		return;
	case 1:
		scene_timer++;
		spawn_objects_by_handler(&ignored, p_list_cursor, &object);
		if ((uint16_t)scene_timer == 0x168u)
			stage_06_background_step = 3u;
		update_stage_06_background(random_value, object, p_rolls_left, p_words_left, p_list_cursor);
		if (scene_timer >= 0xF0 && scene_timer <= 0x12B && (scene_timer & 7u) == 0u)
		{
			mixer_sprite_control |= 0x8000u;
			color_offset_red--;
			color_offset_green--;
			color_offset_blue--;
		}
		if (scene_timer < 0x1A4)
		{
			return;
		}
		scene_step++;
		stage_06_background_step = 5u;
		return;
	case 2:
		scene_step++;
		scene_timer = 0;
		mixer_sprite_control &= ~0x8000u;
		color_offset_red = 0u;
		color_offset_green = 0u;
		color_offset_blue = 0u;
		scene_timer++;
		spawn_objects_by_handler(&ignored, &ignored_2, &object);
		spawn_stage_06_lightning(p_list_cursor, &object);
		update_stage_06_background(random_value, object, p_rolls_left, p_words_left, p_list_cursor);
		if (scene_timer < 0x12C)
		{
			return;
		}
		game_mode = 2u;
		return;
	case 3:
		scene_timer++;
		spawn_objects_by_handler(&ignored, &ignored_2, &object);
		spawn_stage_06_lightning(p_list_cursor, &object);
		update_stage_06_background(random_value, object, p_rolls_left, p_words_left, p_list_cursor);
		if (scene_timer < 0x12C)
		{
			return;
		}
		game_mode = 2u;
		return;
	default:
		invalid_jump(0x94510u + scene_step * 3u);
	}
}

/* Game mode 0x12, stage 9's event at scroll 0x4C00: event_12_stage_frame until scroll_position 0x80B1,
   where the scroll stops (scroll_mode 1) until every player is out of play or done drifting
   (drift_done); then a sign (kind 0xF at 0x644, 0x149E), event_12_fast_scroll cleared, scroll mode 7 and
   game_mode 3, run at once. */
void game_mode_event_12(uint32_t pressed, uint32_t position, uint32_t done_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint16_t kind = 0, x = 0, y = 0;
	uint8_t event_12_status = 0;

	current_game_mode = 0x12u;
	if (scroll_position < 0x80B1u)
	{
		if (scroll_position < 0x80B1u)
		{
			event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
			return;
		}
		scroll_mode = 1u;
		event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	done_count &= ~0xFFu;
	object = PLAYER_BLOCK(1);
	*p_list_cursor = player_status_ADDRESS;
	*p_count = 3u;
	do
	{
		event_12_status = mem_read8(*p_list_cursor);
		(*p_list_cursor)++;
		if (event_12_status != 0xFFu || PLAYER(object)->drift_done != 0u)
		{
			done_count = (done_count & ~0xFFu) | (uint8_t)(done_count + 1u);
		}
		object += 0x200u;
		(*p_count)--;
	} while (*p_count != 0u);
	if ((uint8_t)done_count != 3u)
	{
		if (scroll_position < 0x80B1u)
		{
			event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
			return;
		}
		scroll_mode = 1u;
		event_12_stage_frame(pressed, position, done_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	x = 0x644u;
	y = 0x149Eu;
	kind = 0xFu;
	*p_count = (*p_count & ~0xFFFFu) | 0x168u;
	spawn_sign(x, y, kind, (uint16_t)*p_count, &pressed, &object);
	event_12_fast_scroll = 0u;
	scroll_mode = 7u;
	game_mode = 3u;
	run_game_mode(pressed, position, done_count, x, object, p_speed_value, p_count, p_cycle, p_list_cursor);
}

/* Game mode 0x13, stage 9's event at scroll 0: the scroll held (scroll_mode 1) for 0x78 frames
   (scene_timer) while update_screen_wipe opens the screen, then scroll mode 0; event_12_stage_frame
   every frame, and from scroll_position 0x16AC game_mode 3, run at once. */
void game_mode_event_13(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint32_t edges_done, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t clip_bottom = 0, clip_right = 0, clip_top = 0;
	uint16_t event_13_timer = 0;

	current_game_mode = 0x13u;
	switch (scene_step)
	{
	case 0:
	case 3:
		scene_step++;
		scene_timer = 0x78;
		break;
	case 1:
		break;
	case 2:
		if (scroll_position < 0x16ACu)
		{
			event_12_stage_frame(pressed, position, in_play_count, (uint16_t)edges_done, object, p_speed_value, p_count, p_cycle, p_list_cursor);
			return;
		}
		game_mode = 3u;
		run_game_mode(pressed, position, in_play_count, (uint16_t)edges_done, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	default:
		invalid_jump(0x943A5u + scene_step * 2u);
	}
	scroll_mode = 1u;
	event_13_timer = scene_timer;
	scene_timer = event_13_timer - 1u;
	if ((int16_t)event_13_timer > 1)
	{
		update_screen_wipe(&clip_right, &clip_top, &clip_bottom, &edges_done);
		event_12_stage_frame(clip_right, clip_top, clip_bottom, (uint16_t)edges_done, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	scene_step++;
	scroll_mode = 0u;
	update_screen_wipe(&clip_right, &clip_top, &clip_bottom, &edges_done);
	event_12_stage_frame(clip_right, clip_top, clip_bottom, (uint16_t)edges_done, object, p_speed_value, p_count, p_cycle, p_list_cursor);
}

/* Game mode 0x14, stage 9's event at scroll 0x1800, the scroll stopped: once a player is in play and
   none in state 0x25 has yet to bounce (sequence_bounced), it spawns an icicle dropper and Eggman's
   speech, and 0x12C frames later scroll mode 7 and game_mode 3; a sign (kind 0xE) at scene_timer 0x3C.
   The stage frame; game over with no player left. */
void game_mode_event_14(uint32_t pressed, uint32_t position, uint32_t unbounced_count, uint16_t x, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t playing_count = 0;
	uint16_t kind = 0, y = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0;
	uint8_t event_14_status = 0;
	uint16_t event_14_timer = 0;

	current_game_mode = 0x14u;
	switch (scene_step)
	{
	case 0:
	case 3:
		scene_step++;
		break;
	case 1:
		unbounced_count &= ~0xFFu;
		playing_count = 3u;
		*p_list_cursor = PLAYER_BLOCK(1);
		object = player_status_ADDRESS;
		*p_count = 3u;
		do
		{
			event_14_status = mem_read8(object);
			object++;
			if (event_14_status == 0xFFu)
			{
				if (PLAYER(*p_list_cursor)->state == 0x25u && PLAYER(*p_list_cursor)->sequence_bounced == 0u)
					unbounced_count = (unbounced_count & ~0xFFu) | (uint8_t)(unbounced_count + 1u);
			}
			else
				playing_count--;
			*p_list_cursor += 0x200u;
			(*p_count)--;
		} while (*p_count != 0u);
		if (playing_count != 0u && (uint8_t)unbounced_count == 0u && free_object_stack >= 2)
		{
			scene_step++;
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->sprite_frame = 0u;
			OBJECT(object)->handler = icicle_dropper_start_ADDRESS;
			allocate_object_outside_loop(&ignored, &object);
			OBJECT(object)->sprite_frame = 2u;
			OBJECT(object)->handler = eggman_speech_start_ADDRESS;
			scene_timer = 0x12C;
			event_14_timer = scene_timer;
			scene_timer = event_14_timer - 1u;
			if ((int16_t)event_14_timer <= 1)
			{
				scroll_mode = 7u;
				game_mode = 3u;
				run_game_mode(pressed, position, unbounced_count, x, object, p_speed_value, p_count, p_cycle, p_list_cursor);
				return;
			}
		}
		break;
	case 2:
		event_14_timer = scene_timer;
		scene_timer = event_14_timer - 1u;
		if ((int16_t)event_14_timer <= 1)
		{
			scroll_mode = 7u;
			game_mode = 3u;
			run_game_mode(pressed, position, unbounced_count, x, object, p_speed_value, p_count, p_cycle, p_list_cursor);
			return;
		}
		break;
	default:
		invalid_jump(0x94253u + scene_step * 3u);
	}
	if ((uint16_t)scene_timer == 0x3Cu)
	{
		x = 0xFDEFu;
		y = 0xAF0u;
		kind = 0xEu;
		*p_count = (*p_count & ~0xFFFFu) | 0x28u;
		spawn_sign(x, y, kind, (uint16_t)*p_count, &pressed, &object);
	}
	update_player_summary(&unbounced_count, p_count, p_list_cursor, &object);
	scroll_mode = 1u;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, unbounced_count, x, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	update_scroll(&position, &unbounced_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
	update_stage_tile_layers(&ignored_4, &position, &unbounced_count, p_speed_value, &ignored_2, p_count, p_cycle, &ignored_3, &object);
	find_nearby_floor_blocks(&ignored_4, &unbounced_count, p_speed_value, &ignored_2, p_count, &ignored_3, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_4, &position, &unbounced_count, p_speed_value, &ignored_2, p_count, &object);
	spawn_ring_rows(&ignored_4, &position, &ignored_5, &object);
	spawn_walls(&ignored_4, &position, &ignored_2, p_count, p_cycle, &object);
	spawn_object_groups_2(&ignored_4, p_count, &object);
	spawn_object_groups(&ignored_4, &position, &ignored_5, p_count, &object);
	spawn_objects_by_handler(&ignored_4, &ignored_3, &object);
	ignored_4 = check_zones(ignored_4);
	check_hits(&ignored_4, &position, &ignored_5, p_speed_value, &ignored_2, p_count, &ignored_6);
	check_walls(ignored_4);
	check_platforms(&ignored_4, &ignored_7, &ignored_5, p_speed_value, &ignored_2, p_count, p_cycle, p_list_cursor, &ignored_6);
}

/* Game mode 0x15, stage 3's event at scroll 0xA360: the stage frame with the scroll stopped (scroll_mode
   1) until check_stage_clear changes game_mode (run at once); game over with no player left. */
void game_mode_event_15(uint32_t pressed, uint32_t position, uint32_t speed, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	current_game_mode = 0x15u;
	next_stage_slot_request = 0u;
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	check_stage_clear(p_count, p_list_cursor, &object);
	if (game_mode != 0x15u)
	{
		run_game_mode(pressed, position, speed, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	scroll_mode = 1u;
	update_scroll(&position, &speed, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	update_stage_tile_layers(&ignored_3, &position, &speed, p_speed_value, &ignored, p_count, p_cycle, &ignored_2, &object);
	find_nearby_floor_blocks(&ignored_3, &speed, p_speed_value, &ignored, p_count, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&ignored_3, &position, &speed, p_speed_value, &ignored, p_count, &object);
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

/* Game mode 0x16, stage 3's event at scroll 0x1B80 and 0x8610: the stage frame without the scroll or
   tile updates until scroll_resume_request is set, then scroll mode 7 and game_mode 3; game over with no
   player left. */
void game_mode_event_16(uint32_t box_edge, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	current_game_mode = 0x16u;
	update_player_summary(&in_play_count, p_count, p_list_cursor, &object);
	if ((player_status[0] | player_2_status | player_3_status | player_status_spare) == 0u)
	{
		saved_game_mode = game_mode;
		game_mode = 7u;
		game_mode_timer = 0;
		run_game_mode(box_edge, position, in_play_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
	}
	find_nearby_floor_blocks(&box_edge, &in_play_count, p_speed_value, &ignored, p_count, &ignored_2, &object);
	*p_speed_value = update_restart_point(*p_speed_value);
	spawn_objects_by_type(&box_edge, &position, &in_play_count, p_speed_value, &ignored, p_count, &object);
	spawn_ring_rows(&box_edge, &position, &ignored_3, &object);
	spawn_walls(&box_edge, &position, &ignored, p_count, p_cycle, &object);
	spawn_object_groups_2(&box_edge, p_count, &object);
	spawn_object_groups(&box_edge, &position, &ignored_3, p_count, &object);
	spawn_objects_by_handler(&box_edge, &ignored_2, &object);
	box_edge = check_zones(box_edge);
	check_hits(&box_edge, &position, &ignored_3, p_speed_value, &ignored, p_count, &ignored_4);
	check_walls(box_edge);
	check_platforms(&ignored_5, &ignored_6, &ignored_3, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &ignored_4);
	if (scroll_resume_request == 0u)
	{
		return;
	}
	scroll_resume_request = 0u;
	scroll_mode = 7u;
	game_mode = 3u;
}

/* Game mode 0x18, the last stage's (0x11) event at scroll 0: the ending (update_ending). */
void game_mode_event_18(uint32_t position, uint32_t speed, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	current_game_mode = 0x18u;
	update_ending(position, speed, object, p_speed_value, p_count, p_cycle, p_list_cursor);
}

/* Game mode 0x19, stage 0x10's event at scroll 0xC80: update_player_summary (game over with no player
   left); a player in play whose x passes 0x17C sets barred_gate_closing; then
   event_19_check_rope_bridge. */
void game_mode_event_19(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t player_block = 0, player_object = 0, status_cursor = 0;
	uint8_t event_19_status = 0;

	current_game_mode = 0x19u;
	current_game_mode = 0x19u;
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
	status_cursor = player_status_ADDRESS;
	player_object = player_status_ADDRESS;
	player_block = PLAYER_BLOCK(1);
	*p_count = 3u;
	for (;;)
	{
		event_19_status = mem_read8(status_cursor);
		status_cursor++;
		if (event_19_status == 0xFFu)
		{
			player_object = LINKED(player_object, PLAYER(player_block)->object);
			if (OBJECT(player_object)->x_int > 0x17C)
				break;
		}
		(*p_count)--;
		player_block += 0x200u;
		if (*p_count == 0u)
		{
			event_19_check_rope_bridge(position, in_play_count, player_block, p_speed_value, p_count, p_cycle, p_list_cursor);
			return;
		}
	}
	barred_gate_closing = 0xFFu;
	event_19_check_rope_bridge(position, in_play_count, player_block, p_speed_value, p_count, p_cycle, p_list_cursor);
}

/* Game mode 0x1C, stage 8's riding event at scroll 0x7A8D: by scene_step event_1c_follow (steps 0, 1 and
   4), event_1c_stop, event_1c_scroll; the stage frame; game over with no player left; at scroll_position
   0x84A0 game_mode 3 at scroll speed 5, and bridge_0_plank_count 0. */
void game_mode_event_1C(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	current_game_mode = 0x1Cu;
	if (scene_step == 0u)
	{
		scene_step = 1u;
		event_1c_riding = 0u;
	}
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
	switch (scene_step)
	{
	case 0:
		event_1c_follow();
		break;
	case 1:
		event_1c_follow();
		break;
	case 2:
		event_1c_stop();
		break;
	case 3:
		event_1c_scroll();
		break;
	case 4:
		event_1c_follow();
		break;
	default:
		invalid_jump(0x9510Eu + scene_step * 2u);
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
	if (scroll_position < 0x84A0u)
	{
		return;
	}
	game_mode = 3u;
	scroll_speed_low = 5;
	bridge_0_plank_count = 0u;
}

/* Game mode 0x1F, stage 0xB's escape at scroll 0xF00: start_escape_countdown once; the stage frame after
   check_stage_clear (game over with no player left); with escape_started the alarm (flash, shake, sounds
   0xF and 0x7C, 0x7C again at 0x30C); the countdown runs until all have finished (the swap loop marks
   player 1's byte for player 3); at escape_seconds 0 YOU FAILED (0x20). */
void game_mode_event_1F(uint32_t pressed, uint32_t position, uint32_t in_play_count, uint16_t next_scroll_position, uint32_t object, uint32_t *p_speed_value, uint32_t *p_count, uint32_t *p_cycle, uint32_t *p_list_cursor)
{
	uint8_t sound_id = 0;
	uint32_t exit_status_cursor = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	current_game_mode = 0x1Fu;
	if (scene_step == 0u)
	{
		scene_step = 0xFFu;
		unused_escape_byte = 0u;
		escape_alarm_timer = 0u;
		start_escape_countdown(&position, p_count, &object);
	}
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
	check_stage_clear(p_count, p_list_cursor, &object);
	if (game_mode != 0x1Fu)
	{
		run_game_mode(pressed, position, in_play_count, next_scroll_position, object, p_speed_value, p_count, p_cycle, p_list_cursor);
		return;
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
	check_hits(&ignored_3, &position, &ignored_4, p_speed_value, &ignored, p_count, &object);
	check_walls(ignored_3);
	check_platforms(&ignored_3, &ignored_5, &ignored_4, p_speed_value, &ignored, p_count, p_cycle, p_list_cursor, &object);
	if (escape_started != 0u)
	{
		escape_alarm_timer++;
		if (escape_alarm_timer == 0x30Cu)
		{
			sound_id = 0x7Cu;
			play_sound(sound_id, &ignored_3, &ignored_5);
		}
		else if (escape_alarm_timer == 1u)
		{
			start_escape_alarm_flash(object);
			camera_shake_active = 1u;
			sound_id = 0xFu;
			play_sound(sound_id, &ignored_3, &ignored_5);
			sound_id = 0x7Cu;
			play_sound(sound_id, &ignored_3, &ignored_5);
		}
	}
	if (stage_exit_started == 0u)
	{
		if (no_player_in_play == 0u)
			update_escape_countdown();
	}
	else
	{
		exit_status_cursor = player_status_ADDRESS;
		*p_list_cursor = (players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u));
		*p_count = (*p_count & ~0xFFu) | 3u;
		do
		{
			if ((int8_t)mem_read8(exit_status_cursor) >= 0)
				*p_list_cursor = (*p_list_cursor & ~0xFFu) | 0xFFu;
			*p_list_cursor = (*p_list_cursor & ~0xFFFFu) | (uint16_t)((*p_list_cursor << 8) | ((uint16_t)*p_list_cursor >> 8));
			exit_status_cursor++;
			(*p_count)--;
		} while (*p_count != 0u);
		if (*p_list_cursor != 0xFFFFFFFFu)
		{
			if (no_player_in_play == 0u)
				update_escape_countdown();
			if (escape_seconds != 0u)
			{
				return;
			}
			game_mode = 0x20u;
			game_mode_timer = 0;
			return;
		}
	}
	if (escape_seconds != 0u)
	{
		return;
	}
	game_mode = 0x20u;
	game_mode_timer = 0;
}

/* The CLEARED banner, not drawn yet: once stage_name_shown is set it is revealed
   (stage_clear_banner_reveal). */
void cleared_banner_wait(void)
{
	if (stage_name_shown != 0u)
		OBJECT(current_object)->handler = stage_clear_banner_reveal_ADDRESS;
}

/* Reveals a stage clear banner a step at a time (its animation), a sparkle 0x10 further along at each
   step (spawn_banner_sparkle, sparkle_offset); at the last step it holds (stage_clear_banner_hold) and
   sets stage_name_shown, or cleared_shown for the CLEARED banner (flags bit 0). */
void stage_clear_banner_reveal(void)
{
	uint16_t sparkle_offset = 0;
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0;

	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
	{
		sparkle_offset = OBJECT(current_object)->stage_clear_banner.sparkle_offset;
		spawn_banner_sparkle(sparkle_offset);
		OBJECT(current_object)->stage_clear_banner.sparkle_offset += 0x10u;
	}
	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		stage_clear_banner_hold();
		return;
	}
	OBJECT(current_object)->handler = stage_clear_banner_hold_ADDRESS;
	if ((OBJECT(current_object)->flags & 1u) != 0u)
	{
		cleared_shown = 0xFFu;
		stage_clear_banner_hold();
		return;
	}
	stage_name_shown = 0xFFu;
	stage_clear_banner_hold();
}

/* A shown stage clear banner: queued at depth 0xFF until banner_break; then sound 0x1B and it splits
   into letters (banner_letter_spin): itself and new objects, as many as its animation's first word
   (banner_letters_left), 0x10 apart from 8 past its x, each with its number in kind, odd ones y-flipped
   (temp_1 is banner_letters_left). */
void stage_clear_banner_hold(void)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;
	uint16_t letters_left = 0;

	if (banner_break == 0u)
	{
		if ((int8_t)OBJECT(current_object)->animation_frame < 0)
			return;
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	sound_id = 0x1Bu;
	play_sound(sound_id, &ignored, &ignored_2);
	banner_letters_left = FRAME_ANIMATION(OBJECT(current_object)->animation)->last_frame;
	banner_letter_offset_x = 8u;
	object = current_object;
	do
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		if ((temp_1 & 1u) != 0u)
			OBJECT(object)->sprite_flags |= 0x80u;
		OBJECT(object)->sprite_palette = 0x8980u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = banner_letter_spin_ADDRESS;
		OBJECT(object)->kind = (uint8_t)banner_letters_left;
		OBJECT(object)->animation = banner_letter_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x += banner_letter_offset_x;
		banner_letter_offset_x += 0x10u;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		letters_left = banner_letters_left;
		banner_letters_left = letters_left - 1u;
		if ((int16_t)letters_left <= 1)
			break;
		allocate_object(&free_count, &object);
	} while (free_count != 0u);
	if ((int8_t)OBJECT(current_object)->animation_frame < 0)
		return;
	depth = 0xFFu;
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* Spawns a ring sparkle (ring_sparkle_animation, palette 0x81B0, handler banner_sparkle_animate)
   offset_x along the banner from its screen position. */
void spawn_banner_sparkle(uint16_t offset_x)
{
	uint32_t free_count = 0, object = 0;

	sparkle_offset_x = offset_x;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x81B0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = banner_sparkle_animate_ADDRESS;
		OBJECT(object)->animation = ring_sparkle_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_x += sparkle_offset_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	}
}

/* A banner sparkle: animates, queued at depth 0xFF, and is removed when its animation wraps. */
void banner_sparkle_animate(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, wrapped = 0;

	animate(&ignored, &wrapped, &ignored_2);
	if ((uint8_t)wrapped == 0u)
	{
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* A letter of a broken stage clear banner: spins through banner_letter_animation, queued at depth 0xFF,
   and is removed at its end; at step 0xC an odd letter (kind holds its number) spawns a sparkle
   (banner_sparkle_animation, y-flipped when kind bit 1 is set). */
void banner_letter_spin(void)
{
	uint32_t depth = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, wrapped = 0;

	if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 0xCu && (OBJECT(current_object)->kind & 1u) != 0u)
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			animate(&ignored, &wrapped, &ignored_2);
			if ((uint8_t)wrapped == 0u)
			{
				depth = 0xFFu;
				queue_sprite_at_depth(&ignored_3, &depth);
				return;
			}
			delete_current_object();
			return;
		}
		OBJECT(object)->sprite_flags = 0x1000u;
		if ((OBJECT(current_object)->kind & 2u) != 0u)
			OBJECT(object)->sprite_flags |= 0x80u;
		OBJECT(object)->sprite_palette = 0x8990u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->handler = banner_sparkle_animate_ADDRESS;
		OBJECT(object)->animation = banner_sparkle_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	}
	animate(&ignored, &wrapped, &ignored_2);
	if ((uint8_t)wrapped == 0u)
	{
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* Sets the stage's layer blending: clears bit 12 (blending) in the seven layer shadows from
   mixer_text_blend, then for each entry of stage_layer_blending[stage] (a count, then the entries) puts
   the entry's mask in the low 6 bits of the layer shadow it names and sets the entry's bit there, and
   its blend factor in mixer_blend with the enable bit 11. */
void apply_stage_layer_blending(uint32_t *p_layer_bit, uint32_t *p_count)
{
	uint16_t blend_mask = 0;
	uint32_t count = 0, cursor = 0, layer_shadow = 0;

	cursor = mixer_text_blend_ADDRESS;
	count = 7u;
	do
	{
		mem_write32(cursor, mem_read32(cursor) & ~0x1000u);
		cursor += 2u;
		count--;
	} while (count != 0u);
	cursor = stage_layer_blending_ADDRESS + stage_layer_blending[stage];
	if (LAYER_BLEND_LIST(cursor)->count != 0u)
	{
		count = LAYER_BLEND_LIST(cursor)->count;
		cursor++;
		do
		{
			blend_mask = LAYER_BLEND_RECORD(cursor)->blend_mask;
			cursor++;
			*p_layer_bit = LAYER_BLEND_RECORD(cursor - 0x1u)->layer_bit;
			cursor++;
			layer_shadow = GAME_VARIABLES + LAYER_BLEND_RECORD(cursor - 0x2u)->shadow_offset;
			mem_write16(layer_shadow, mem_read16(layer_shadow) & ~0x3Fu);
			mem_write16(layer_shadow, mem_read16(layer_shadow) | blend_mask);
			mem_write32(layer_shadow, mem_read32(layer_shadow) | shl32(1u, (int)(*p_layer_bit)));
			mixer_blend &= ~0xF00u;
			cursor += 2u;
			mixer_blend |= LAYER_BLEND_RECORD(cursor - 0x4u)->blend_factor;
			mixer_blend |= 0x800u;
			count--;
			cursor += 2u;
		} while (count != 0u);
	}
	*p_count = count;
}

/* Once scroll_position reaches goal_scroll_position with stage_exit_started set (and player_status_spare
   0): players_finished is set for each player not in play (player_status 0, 2, 3, 4 or 0x82), and when
   players_finished and goal_unlocked, read as one 32-bit word, are all 0xFF the stage is cleared:
   game_mode 0x17, or 0x1E on stages 0xB-0xF. */
void check_stage_clear(uint32_t *p_count, uint32_t *p_finished_flag, uint32_t *p_status)
{
	uint32_t count = *p_count;
	uint32_t finished_flag = *p_finished_flag;
	uint32_t status = *p_status;
	uint8_t next_game_mode = 0;

	if (goal_scroll_position <= scroll_position && player_status_spare == 0u && stage_exit_started != 0u)
	{
		finished_flag = players_finished_ADDRESS;
		status = player_status_ADDRESS;
		count = (count & ~0xFFu) | 3u;
		do
		{
			if (mem_read8(status) == 0u || mem_read8(status) == 0x82u || mem_read8(status) == 3u || mem_read8(status) == 4u || mem_read8(status) == 2u)
				mem_write8(finished_flag, 0xFFu);
			finished_flag++;
			status++;
			count--;
		} while (count != 0u);
		if ((players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u)) == 0xFFFFFFFFu)
		{
			game_mode_timer = 0;
			next_game_mode = 0x17u;
			if (stage == 0xBu || stage == 0xCu || stage == 0xDu || stage == 0xEu || stage == 0xFu)
				next_game_mode = 0x1Eu;
			game_mode = next_game_mode;
		}
	}
	*p_count = count;
	*p_finished_flag = finished_flag;
	*p_status = status;
}

/* Zeroes the three player blocks, 0x600 bytes from PLAYER_BLOCK(1); hands back 0. */
uint32_t clear_player_blocks(void)
{
	uint32_t count = 0, dest = 0, fill_word = 0;

	dest = PLAYER_BLOCK(1);
	count = 0x30u;
	fill_word = 0u;
	do
	{
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		count--;
		dest += 4u;
	} while (count != 0u);
	return count;
}

/* The grey screen while a player asks for it: with grey_screen_request set in any player block,
   grey_screen_active and mixer_offset_select bits 1-4 are set; when none asks any more they are cleared
   and grey_screen_ended is set for that one frame. */
uint32_t update_grey_screen(void)
{
	uint32_t count = 0, player_block = 0;

	grey_screen_ended = 0u;
	player_block = 0x208200u;
	count = 3u;
	do
	{
		player_block += 0x200u;
		count--;
	} while ((count != 0u) & (PLAYER(player_block)->grey_screen_request == 0u));
	if (PLAYER(player_block)->grey_screen_request != 0u)
	{
		grey_screen_active = 0xFFu;
		mixer_offset_select |= 0x1Eu;
		return count;
	}
	if (grey_screen_active != 0u)
	{
		grey_screen_ended = 0xFFu;
		grey_screen_active = 0u;
		mixer_offset_select &= ~0x1Eu;
	}
	return count;
}

/* Sums up the players: player_dropping_in while a player_status is 0x81 (scroll_mode 0 meanwhile, then
   saved_scroll_mode back), no_player_in_play when no status has bit 7, player_mishap when a player still
   in has its mishap_flag (all three cleared), players_in_play (also in in_play_count). With
   skip_player_summary set it only clears the three flags. */
void update_player_summary(uint32_t *p_in_play_count, uint32_t *p_count, uint32_t *p_cursor, uint32_t *p_player_object)
{
	uint8_t in_play_count = (uint8_t)*p_in_play_count;
	uint32_t count = *p_count;
	uint32_t cursor = *p_cursor;
	uint32_t player_object = *p_player_object;
	uint32_t zero = 0, negative = 0;
	uint32_t object = 0, player_block = 0, players_left = 0, status_cursor = 0;
	uint8_t drop_in_status = 0, in_play_status = 0, status = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x800u;
	if ((init_flags & 0x800u) == 0u)
		unused_summary_byte = 0u;
	player_dropping_in = 0u;
	no_player_in_play = 0u;
	player_mishap = 0u;
	if (skip_player_summary != 0u)
		return;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		drop_in_status = mem_read8(status_cursor);
		zero = (uint8_t)(drop_in_status + 0x7Fu) == 0u;
		status_cursor++;
		players_left--;
	} while ((players_left != 0u) & (drop_in_status != 0x81u));
	if (zero == 0u)
	{
		if (player_dropping_in_before != 0u)
			scroll_mode = saved_scroll_mode;
	}
	else
	{
		player_dropping_in = 0xFFu;
		if (player_dropping_in_before == 0u || scroll_mode != 0u)
			saved_scroll_mode = scroll_mode;
		scroll_mode = 0u;
		if (SCROLL_MODE_STEP(scroll_mode_script)[-1].mode != 0u)
			scroll_speed_low = 5;
	}
	player_dropping_in_before = player_dropping_in;
	status_cursor = player_status_ADDRESS;
	players_left = 3u;
	do
	{
		in_play_status = mem_read8(status_cursor);
		negative = in_play_status >> 7;
		status_cursor++;
		players_left--;
	} while ((players_left != 0u) & ((int8_t)in_play_status >= 0));
	if (negative == 0u)
		no_player_in_play = 0xFFu;
	player_block = PLAYER_BLOCK(1);
	object = PLAYER_BLOCK(1);
	players_left = 3u;
	for (;;)
	{
		object = LINKED(object, PLAYER(player_block)->object);
		if ((OBJECT(object)->flags & 0x400u) != 0u || PLAYER(player_block)->mishap_flag == 0u)
		{
			players_left--;
			player_block += 0x200u;
			if (players_left == 0u)
				break;
			continue;
		}
		player_mishap = 0xFFu;
		player_1_mishap_flag = 0u;
		player_2_mishap_flag = 0u;
		player_3_mishap_flag = 0u;
		break;
	}
	count = 3u;
	in_play_count = 0u;
	cursor = player_status_ADDRESS;
	do
	{
		status = mem_read8(cursor);
		cursor++;
		if ((int8_t)status < 0)
			in_play_count++;
		count--;
	} while (count != 0u);
	players_in_play = in_play_count;
	*p_in_play_count = (*p_in_play_count & 0xFFFFFF00u) | in_play_count;
	*p_count = count;
	*p_cursor = cursor;
	*p_player_object = object;
}

/* When next_stage_slot_request is set, which nothing does (it is only ever cleared): keeps the floor
   block where this stage links to the next (stage_link_points) and the one it reaches there, their x, y
   and height (stage_link_from_*, stage_link_to_*); then stage + 1, its map pointers, and
   stage_init_flags bits 0-3 cleared. */
void continue_to_next_stage_slot(uint32_t *p_table_offset, uint32_t *p_link_record, uint32_t *p_block)
{
	uint32_t block = *p_block;
	uint32_t current_stage = 0, from_block = 0, from_record = 0, link_point = 0, to_block = 0;

	if (next_stage_slot_request == 0u)
		return;
	link_point = stage_link_points_ADDRESS + stage_link_points[stage];
	from_block = STAGE_LINK_POINT(link_point)->from_block;
	from_record = floor_blocks + 2u + from_block;
	stage_link_from_block = from_block;
	stage_link_from_x = FLOOR_BLOCK(from_record)->x_min;
	stage_link_from_y = FLOOR_BLOCK(from_record)->y_min;
	stage_link_from_height = FLOOR_BLOCK(from_record)->height;
	current_stage = stage;
	block = stage_floor_blocks_ADDRESS + stage_floor_blocks[(uint16_t)(current_stage + 1u)];
	to_block = STAGE_LINK_POINT(link_point)->to_block;
	block = block + 2u + to_block;
	stage_link_to_block = to_block;
	stage_link_to_x = FLOOR_BLOCK(block)->x_min;
	stage_link_to_y = FLOOR_BLOCK(block)->y_min;
	stage_link_to_height = FLOOR_BLOCK(block)->height;
	stage++;
	load_stage_map_pointers(p_table_offset, p_link_record);
	stage_init_flags &= 0xFFFFFFF0u;
	*p_block = block;
}

/* Points current_restart_point, floor_blocks and floor_block_bounds at stage's lists
   (stage_restart_points, stage_floor_blocks, stage_floor_block_bounds); hands back the bounds'
   table_offset and bounds_list. */
void load_stage_map_pointers(uint32_t *p_table_offset, uint32_t *p_bounds_list)
{
	uint32_t stage_index = 0;

	current_restart_point = stage_restart_points_ADDRESS + stage_restart_points[stage];
	stage_index = stage;
	floor_blocks = stage_floor_blocks_ADDRESS + stage_floor_blocks[stage_index];
	*p_table_offset = stage_floor_block_bounds[stage_index];
	*p_bounds_list = stage_floor_block_bounds_ADDRESS + *p_table_offset;
	floor_block_bounds = *p_bounds_list;
}

/* The stage's events: once scroll_position reaches the next (scroll position, game mode) record of
   stage_events[stage], it switches game_mode to it (scene_step and scene_timer 0), unless the scroll is
   already 0x100 or more past it (stage 0x11 takes it anyway); the cursor moves on either way. */
void run_stage_events(uint32_t *p_table_offset, uint32_t *p_event)
{
	uint16_t distance_past = 0;
	uint32_t event = 0;
	uint32_t init_flags = 0;

	init_flags = stage_init_flags;
	stage_init_flags = init_flags | 0x100u;
	if ((init_flags & 0x100u) == 0u)
	{
		*p_table_offset = stage_events[stage];
		stage_event_cursor = stage_events_ADDRESS + *p_table_offset;
	}
	event = stage_event_cursor;
	if (STAGE_EVENT(event)->position <= scroll_position)
	{
		distance_past = (uint16_t)(scroll_position - STAGE_EVENT(event)->position);
		event += 2u;
		if (stage == 0x11u || distance_past < 0x100u)
		{
			game_mode = STAGE_EVENT(event - 0x2u)->next_game_mode;
			scene_step = 0u;
			scene_timer = 0;
		}
		stage_event_cursor += 4u;
	}
	*p_event = event;
}

/* Clears trackball_step_sums[player_number] and reads the trackball once (read_trackball_step): the
   start of the debug stage select. */
void reset_trackball_step(uint32_t player_number)
{
	uint32_t ignored = 0;

	trackball_step_sums[player_number] = 0u;
	read_trackball_step(&player_number, &ignored);
}

/* Adds player_number's trackball x motion to trackball_step_sums (restarting the sum when the direction
   changes) and hands back in player_number a step: 1 each 0x40 counts one way, -1 the other, else 0;
   trackball comes back as the address of the trackball's motion. */
void read_trackball_step(uint32_t *p_player_number, uint32_t *p_trackball)
{
	uint32_t player_number = *p_player_number;
	uint16_t step = 0;
	uint32_t step_sum = 0;

	*p_trackball = trackball_1_x_ADDRESS + player_number * 4u;
	step_sum = trackball_step_sums_ADDRESS + player_number * 2u;
	if ((int16_t)(mem_read16(step_sum) ^ TRACKBALL_MOTION(*p_trackball)->x) < 0)
		mem_write16(step_sum, 0u);
	mem_write16(step_sum, mem_read16(step_sum) + TRACKBALL_MOTION(*p_trackball)->x);
	step = 0u;
	if ((int16_t)mem_read16(step_sum) >= 0x40)
	{
		step = 1u;
		mem_write16(step_sum, 0u);
		*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
		return;
	}
	if ((int16_t)mem_read16(step_sum) <= -0x40)
	{
		step = 0xFFFFu;
		mem_write16(step_sum, 0u);
	}
	*p_player_number = (*p_player_number & 0xFFFF0000u) | step;
}

/* Moves current_restart_point on through the stage's 8-byte restart records to the first whose scroll
   position is past scroll_position; hands back position, its low half now scroll_position. */
uint32_t update_restart_point(uint32_t position)
{
	uint32_t restart_point = 0;

	restart_point = current_restart_point - 8u;
	position = (position & ~0xFFFFu) | scroll_position;
	do
	{
		restart_point += 8u;
	} while (RESTART_POINT_RECORD(restart_point)->position <= (uint16_t)position);
	current_restart_point = restart_point;
	return position;
}

/* Restarts the player objects for a stage: reset_objects_from_list with player_objects_alt
   (player_start_stage for each player, which it turns into player_resume or player_join as their blocks
   say, and the three shadows). Hands back player_number. */
uint32_t start_player_objects_alt(uint32_t object)
{
	uint32_t ignored = 0, player_number = 0;

	player_handler_list = player_objects_alt_ADDRESS;
	reset_objects_from_list(object, &player_number, &ignored);
	return player_number;
}

/* Starts the player objects afresh: reset_objects_from_list with player_objects (player_1_start to
   player_3_start and the three shadows). Hands back handler_cursor. */
uint32_t start_player_objects(uint32_t object)
{
	uint32_t handler_cursor = 0, ignored = 0;

	player_handler_list = player_objects_ADDRESS;
	reset_objects_from_list(object, &ignored, &handler_cursor);
	return handler_cursor;
}

/* Empties the object list and pools (reset_objects), then starts an object per handler of the list in
   object_handler_list (a count, then handlers), walking the player blocks (player_block_cursor):
   player_start_stage becomes player_join for a player who is_joining, or player_resume when its
   player_status is 1, 2 or 3. Stops when no object is free. */
void reset_objects_from_list(uint32_t object, uint32_t *p_player_number, uint32_t *p_handler_cursor)
{
	uint8_t status = 0;
	uint32_t free_count = 0, handler_cursor = 0, handlers_left = 0, ignored = 0, player_number = 0;
	uint32_t handler_count = 0;

	player_block_cursor = PLAYER_BLOCK(1);
	reset_objects(&ignored, &player_number);
	handler_cursor = object_handler_list;
	handlers_left = OBJECT_START_LIST(handler_cursor)->count;
	handler_cursor += 2u;
	object = allocate_object_or_start_list(object);
	OBJECT(object)->handler = OBJECT_START_HANDLERS(handler_cursor)[0];
	handler_cursor += 4u;
	if (OBJECT(object)->handler == player_start_stage_ADDRESS)
	{
		if (mem_read8(player_block_cursor + 9u) == 0u)
		{
			status = player_status[0];
			if (status == 2u || status == 1u || status == 3u)
				OBJECT(object)->handler = player_resume_ADDRESS;
		}
		else
			OBJECT(object)->handler = player_join_ADDRESS;
	}
	player_block_cursor += 0x200u;
	handler_count = handlers_left;
	handlers_left = handler_count - 1u;
	if ((int32_t)handler_count > 1)
	{
		do
		{
			allocate_object(&free_count, &object);
			if (free_count == 0u)
				break;
			OBJECT(object)->handler = OBJECT_START_HANDLERS(handler_cursor)[0];
			handler_cursor += 4u;
			if (OBJECT(object)->handler == player_start_stage_ADDRESS)
			{
				if (mem_read8(player_block_cursor + 9u) == 0u)
				{
					player_number = 1u;
					if (player_block_cursor != PLAYER_BLOCK(2))
						player_number = 2u;
					status = player_status[player_number];
					if (status == 2u || status == 1u || status == 3u)
						OBJECT(object)->handler = player_resume_ADDRESS;
				}
				else
					OBJECT(object)->handler = player_join_ADDRESS;
			}
			player_block_cursor += 0x200u;
			handlers_left--;
		} while (handlers_left != 0u);
	}
	*p_player_number = player_number;
	*p_handler_cursor = handler_cursor;
}

/* Draws the GAME OVER banner (sprite 0x89A0, palette 0x8970: the words with Eggman behind them) with the
   static sprite block at 0x209FC0, at screen 0xA0, 0x6C, queued on top of everything. */
void draw_game_over_banner(void)
{
	current_object = 0x209FC0u;
	banner_sprite_flags = 0x1000u;
	banner_sprite_x = 0xA0u;
	banner_sprite_y = 0x6Cu;
	banner_sprite_frame = 0x89A0u;
	banner_sprite_palette = 0x8970u;
	queue_sprite_on_top();
}

/* The demos' prompt: every 0x20 frames demo_prompt_shown flips, and on showing, the static sprite block
   at 0x209FC0 is set to PUSH START BUTTON (0x89B8), or INSERT COIN(S) (0x89A8) short of credits, at
   screen 0xA0, 0x64. It is queued on top while shown in a demo (current_screen 4 or 5); otherwise bits
   2-3 of port D are cleared. */
uint32_t blink_demo_prompt(uint32_t prompt_frame, uint32_t credit_block)
{
	uint8_t port_d = 0, shown = 0;

	if ((frame_counter & 0x1Fu) != 0u)
	{
		if ((current_screen == 4u || current_screen == 5u) && demo_prompt_shown != 0u)
		{
			current_object = 0x209FC0u;
			queue_sprite_on_top();
			return prompt_frame;
		}
		port_d = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d & ~0xCu);
		return prompt_frame;
	}
	shown = ~demo_prompt_shown;
	demo_prompt_shown = shown;
	if (shown == 0u)
	{
		port_d = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d & ~0xCu);
		return prompt_frame;
	}
	current_object = 0x209FC0u;
	banner_sprite_flags = 0x1000u;
	banner_sprite_x = 0xA0u;
	banner_sprite_y = 0x64u;
	banner_sprite_palette = 0x8960u;
	prompt_frame = (prompt_frame & ~0xFFFFu) | 0x89B8u;
	if (free_play == 0u && p1_credits < credits_per_start)
		prompt_frame = (prompt_frame & ~0xFFFFu) | 0x89A8u;
	OBJECT(current_object)->sprite_frame = prompt_frame;
	if ((current_screen == 4u || current_screen == 5u) && demo_prompt_shown != 0u)
	{
		current_object = 0x209FC0u;
		queue_sprite_on_top();
		return prompt_frame;
	}
	port_d = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d & ~0xCu);
	return prompt_frame;
}
