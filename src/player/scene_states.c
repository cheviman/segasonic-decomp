/**
 *    Name:        player/scene_states.c
 *    Description: the player's states in scenes and stage events: out of the game, continue, entering and leaving a stage, tumbling, blown away, the intro and opening scenes, stunned, choosing a character, YOU FAILED, held, stuck
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* State 0x24, out of the game: on its first frame clears its player_status and stage_rings, erases its
   name and prints GAME OVER in its HUD. A start with credits (try_join, unless joining_closed) joins
   again (player_join, rings 0, stages_reached 1); else after 0x78 frames state 0xB. GAME OVER is erased
   either way. */
void player_state_out_of_game(void)
{
	uint16_t cell = 0;
	uint32_t ignored = 0, joined = 0, player = 0, player_number = 0, text = 0, text_position = 0;

	if ((uint16_t)PLAYER(current_player)->state_timer == 0u)
	{
		player_status[OBJECT(current_object)->player] = 0u;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_name_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_name_positions_2p[player];
		text = hud_mighty_name_text_ADDRESS;
		cell = 0u;
		erase_text_here(cell, &text, &text_position);
		text = hud_game_over_text_ADDRESS;
		player_number = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_game_over_positions_3p[player_number];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_game_over_positions_2p[player_number];
		print_text_here(&ignored, &text, &text_position);
		PLAYER(current_player)->stage_rings = 0u;
	}
	if (joining_closed != 0u)
	{
		PLAYER(current_player)->state_timer++;
		if (PLAYER(current_player)->state_timer < 0x78)
			return;
		PLAYER(current_player)->state = 0xBu;
	}
	else
	{
		joined = try_join();
		if (joined == 0u)
		{
			PLAYER(current_player)->state_timer++;
			if (PLAYER(current_player)->state_timer < 0x78)
				return;
			PLAYER(current_player)->state = 0xBu;
			text = hud_game_over_text_ADDRESS;
			player = OBJECT(current_object)->player;
			text_position = 0x310000u | hud_game_over_positions_3p[player];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_game_over_positions_2p[player];
			cell = 0u;
			erase_text_here(cell, &text, &text_position);
			return;
		}
		OBJECT(current_object)->handler = player_join_ADDRESS;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->is_joining = 0xFFu;
		PLAYER(current_player)->in_game = 1;
		PLAYER(current_player)->rings = 0u;
		PLAYER(current_player)->stages_reached = 1u;
	}
	text = hud_game_over_text_ADDRESS;
	player = OBJECT(current_object)->player;
	text_position = 0x310000u | hud_game_over_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		text_position = (text_position & ~0xFFFFu) | hud_game_over_positions_2p[player];
	cell = 0u;
	erase_text_here(cell, &text, &text_position);
}

/* State 0x23, the continue countdown: on entry (continue_countdown 0x900) out of play (player_status 2,
   flag 0x400), CONTINUE in its HUD, record_player_game, its character freed. Each frame it shows the
   credits and continue_seconds; a credit (try_continue) joins again, start below 7 seconds skips one,
   and past 0 or with joining_closed state 0x2E. */
void player_state_continue(void)
{
	uint8_t cabinet_event = 0, cost = 0;
	uint16_t attribute = 0, cell = 0;
	uint32_t character_bit = 0, credit_block = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, joined = 0, player = 0, player_2 = 0, player_number = 0, start_bit = 0, text = 0, text_position = 0, value = 0;
	uint16_t saved_credits_per_start = 0;
	uint8_t continue_seconds = 0;

	if (PLAYER(current_player)->continue_countdown == 0x900u)
	{
		player_status[OBJECT(current_object)->player] = 2u;
		OBJECT(current_object)->flags |= 0x400u;
		text = hud_ring_count_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_ring_count_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_ring_count_positions_2p[player];
		cell = 0u;
		erase_text_here(cell, &text, &text_position);
		player_number = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_continue_positions_3p[player_number];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_continue_positions_2p[player_number];
		text = hud_continue_text_ADDRESS;
		print_text_here(&ignored, &text, &text_position);
		record_player_game();
		cabinet_event = 0u;
		if (OBJECT(current_object)->player != 0u)
		{
			cabinet_event = 2u;
			if (OBJECT(current_object)->player != 1u)
				cabinet_event = 4u;
		}
		mem_write8(cabinet_events, cabinet_event);
		character_bit = 0u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_bit = 1u;
			if (OBJECT(current_object)->kind != 1u)
				character_bit = 2u;
		}
		characters_taken &= ~shl32(1u, (int)(character_bit));
	}
	if (joining_closed != 0u)
	{
		PLAYER(current_player)->state = 0x2Eu;
		PLAYER(current_player)->continue_countdown = 0u;
		PLAYER(current_player)->state_timer = 0;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, &text_position);
	}
	else
	{
		switch (cabinet_config)
		{
		case 0:
		case 1:
		case 4:
			credit_block = p1_coins_ADDRESS;
			break;
		case 2:
		case 3:
			credit_block = p1_coins_ADDRESS + OBJECT(current_object)->player * 4u;
			break;
		default:
			invalid_jump(0x5FF0u + cabinet_config * 3u);
		}
		player = OBJECT(current_object)->player;
		text_position = 0x310000u | credit_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | credit_positions_2p[player];
		draw_credit_count_now(credit_block, &ignored_2, &ignored_3, &ignored_4, &text_position);
		value = (uint8_t)PLAYER(current_player)->continue_seconds;
		attribute = 0u;
		player_2 = OBJECT(current_object)->player;
		text_position = 0x310000u | hud_continue_positions_3p[player_2];
		if ((cabinet_config & 1u) == 0u)
			text_position = (text_position & ~0xFFFFu) | hud_continue_positions_2p[player_2];
		text_position += 0x14u;
		draw_hex_2(value, attribute, text_position);
		saved_credits_per_start = credits_per_start;
		credits_per_start = 1u;
		draw_player_start_prompt();
		credits_per_start = saved_credits_per_start;
		if (PLAYER(current_player)->continue_seconds < 7)
		{
			switch (OBJECT(current_object)->player)
			{
			case 0:
			case 3:
				start_bit = 0x10u;
				break;
			case 1:
				start_bit = 0x18u;
				break;
			case 2:
				start_bit = 0x12u;
				break;
			default:
				invalid_jump(0x609Eu + OBJECT(current_object)->player * 2u);
			}
			if ((inputs_pressed & shl32(1u, (int)(start_bit))) != 0u)
				PLAYER(current_player)->continue_frames = 0x3C;
		}
		cost = 1u;
		joined = try_continue(cost);
		if (joined != 0u)
		{
			OBJECT(current_object)->handler = player_join_ADDRESS;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->is_joining = 0xFFu;
			PLAYER(current_player)->in_game = 1;
			text = hud_message_blank_text_ADDRESS;
			player = OBJECT(current_object)->player;
			text_position = 0x310000u | hud_text_positions_3p[player];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
			erase_text_lines(&text, &text_position);
			return;
		}
		PLAYER(current_player)->continue_frames++;
		if (PLAYER(current_player)->continue_frames >= 0x3C)
		{
			PLAYER(current_player)->continue_frames = 0;
			continue_seconds = PLAYER(current_player)->continue_seconds;
			PLAYER(current_player)->continue_seconds = continue_seconds - 1u;
			if ((int8_t)continue_seconds <= 1)
			{
				PLAYER(current_player)->state = 0x2Eu;
				PLAYER(current_player)->continue_countdown = 0u;
				PLAYER(current_player)->state_timer = 0;
				text = hud_message_blank_text_ADDRESS;
				player = OBJECT(current_object)->player;
				text_position = 0x310000u | hud_text_positions_3p[player];
				if ((cabinet_config & 1u) == 0u)
					text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
				erase_text_lines(&text, &text_position);
				return;
			}
		}
	}
}

/* State 0x22, in a kind 0x16 zone: on entry it stands on its floor facing direction 0x100 with its
   normal palette and controls, the first time with sign kind 0xB over it (sign_follow_player); then each
   frame check_player_contacts and, unless that hurt it (state 0xD or 0x20, run at once),
   player_in_zone_steer. */
void player_state_in_zone(uint32_t depth, uint32_t cursor)
{
	uint16_t kind = 0, x_int = 0, y_int = 0, z_int = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->direction = 0x100u;
		face_movement_direction(0u);
		add_player_to_lists();
		if (PLAYER(current_player)->effect_spawned == 0u)
		{
			x_int = (uint16_t)OBJECT(current_object)->x_int;
			y_int = (uint16_t)OBJECT(current_object)->y_int;
			kind = 0xBu;
			z_int = (uint16_t)OBJECT(current_object)->z_int;
			spawn_sign_2(x_int, y_int, kind, z_int, cursor);
			PLAYER(current_player)->effect_spawned = 0xFFu;
		}
		depth = queue_sprite_at_base(depth);
		return;
	case 1:
		check_player_contacts(&depth, &cursor);
		if (PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x20u)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
		player_in_zone_steer(depth, cursor);
		return;
	default:
		invalid_jump(0x615Bu + PLAYER(current_player)->substate * 3u);
	}
}

/* Steering in a zone: speed follows the square of the trackball's speed (at most 0x5000000; at once up,
   1/32 of the gap a frame down) and stops with no input below speed_int 0x20; state 0, 1, 0x13 or 0x14
   by it for player_animate, then state 0x22 again while flags bit 0x8000000 (in a zone) is set. */
void player_in_zone_steer(uint32_t depth, uint32_t cursor)
{
	uint8_t next_state = 0;
	uint32_t ignored = 0, speed_diff = 0, steps_left = 0, trackball_speed = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0;

	read_player_trackball();
	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) == 0u && OBJECT(current_object)->speed_int < 0x20)
	{
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->state = 0u;
		update_screen_position(&ignored, &depth);
		update_player_blink();
		player_animate(depth, cursor);
		PLAYER(current_player)->free_turn_timer = 0x10u;
		if ((OBJECT(current_object)->flags & 0x8000000u) != 0u)
			PLAYER(current_player)->state = 0x22u;
		add_player_to_lists();
		return;
	}
	trackball_speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	trackball_speed = trackball_speed * trackball_speed + trackball_y * trackball_y;
	square_root(&trackball_speed, &steps_left);
	trackball_speed &= 0xFFFFu;
	trackball_speed = trackball_speed * trackball_speed;
	if ((int32_t)trackball_speed > 0x5000000)
		trackball_speed = 0x5000000u;
	speed = OBJECT(current_object)->speed;
	target_speed = trackball_speed;
	speed_diff = target_speed - speed;
	if ((int32_t)target_speed <= (int32_t)speed)
		OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
	else
		OBJECT(current_object)->speed += speed_diff;
	PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
	next_state = 0u;
	depth = (steps_left & ~0xFFFFu) | (uint16_t)PLAYER(current_player)->animation_speed_int;
	if ((uint16_t)depth != 0u)
	{
		next_state = 1u;
		if ((int16_t)depth > 0x200)
		{
			next_state = 0x13u;
			if ((int16_t)depth > 0x4F0)
				next_state = 0x14u;
		}
	}
	PLAYER(current_player)->state = next_state;
	update_screen_position(&ignored, &depth);
	update_player_blink();
	player_animate(depth, cursor);
	PLAYER(current_player)->free_turn_timer = 0x10u;
	if ((OBJECT(current_object)->flags & 0x8000000u) != 0u)
		PLAYER(current_player)->state = 0x22u;
	add_player_to_lists();
}

/* State 0x2A, walking in on the stage's entry path (stages 3, 4, 0xA-0x10; others return at once):
   requests stage_player_entries[stage] with flag 0x4000000 (stage 4 adds sound 0x64, 0x6B or 0x51 by
   character), clears it and enter_on_respawn once the path ends, moving as previous_state (player_move).
   Its state 0 store is overwritten at once. */
void player_state_entering_stage(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0;

	switch (stage)
	{
	case 0:
	case 1:
	case 2:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 17:
	case 18:
	case 19:
		return;
	case 3:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		switch (PLAYER(current_player)->entry_step)
		{
		case 0:
		case 2:
			PLAYER(current_player)->path_request = (uint8_t)stage_player_entries[stage];
			OBJECT(current_object)->flags |= 0x4000000u;
			PLAYER(current_player)->entry_step++;
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		case 1:
			if (PLAYER(current_player)->path == 0u)
			{
				OBJECT(current_object)->flags &= ~0x4000000u;
				PLAYER(current_player)->enter_on_respawn = 0u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->substate = 0u;
			}
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		default:
			invalid_jump(0x6345u + PLAYER(current_player)->entry_step * 3u);
		}
		break;
	case 4:
		switch (PLAYER(current_player)->entry_step)
		{
		case 0:
		case 2:
			PLAYER(current_player)->path_request = (uint8_t)stage_player_entries[stage];
			OBJECT(current_object)->flags |= 0x4000000u;
			PLAYER(current_player)->entry_step++;
			sound_id = 0x64u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x6Bu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x51u;
			}
			play_sound(sound_id, &depth, &ignored);
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		case 1:
			if (PLAYER(current_player)->path == 0u)
			{
				OBJECT(current_object)->flags &= ~0x4000000u;
				PLAYER(current_player)->enter_on_respawn = 0u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->substate = 0u;
			}
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		default:
			invalid_jump(0x62EBu + PLAYER(current_player)->entry_step * 3u);
		}
		break;
	default:
		invalid_jump(0x62A3u + stage * 3u);
	}
}

/* State 0x21, the stage exit: walks its goal's exit path (stage_goal_exit_paths, stage_exit_started;
   stages_reached + 1 except after stages 5 and 0xB-0xF), then sets its players_finished entry, no shadow
   (player flags 8). In stage 4: stands 0x168 frames, runs to (0x1225, 0x1439), jumps within 0xA0 of it
   (sound 0x16), falls to screen y 0xB8. */
void player_state_stage_exit(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t dy = 0;
	uint32_t goal_angle = 0, goal_dx = 0, goal_dy = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t state_timer = 0;

	switch (stage)
	{
	case 0:
	case 1:
	case 2:
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
	case 16:
	case 17:
	case 18:
	case 19:
		switch (PLAYER(current_player)->goal_step)
		{
		case 0:
		case 3:
			PLAYER(current_player)->goal_step++;
			OBJECT(current_object)->flags |= 0x4000000u;
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->path_request = stage_goal_exit_paths[stage_goals[(uint8_t)stage]];
			stage_exit_started = 0xFFu;
			if (stage != 5u && stage != 0xCu && stage != 0xDu && stage != 0xEu && stage != 0xFu && stage != 0xBu)
				PLAYER(current_player)->stages_reached++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			player_move(depth, cursor);
			return;
		case 1:
			if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
				player_move(depth, cursor);
				return;
			}
			PLAYER(current_player)->goal_step++;
			players_finished[OBJECT(current_object)->player] = 0xFFu;
			return;
		case 2:
			PLAYER(current_player)->flags |= 8u;
			return;
		default:
			invalid_jump(0x63F1u + PLAYER(current_player)->goal_step * 3u);
		}
		break;
	case 4:
		switch (PLAYER(current_player)->goal_step)
		{
		case 0:
		case 5:
			PLAYER(current_player)->goal_step++;
			PLAYER(current_player)->state_timer = 0x168;
			stage_exit_started = 0xFFu;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
			OBJECT(current_object)->flags &= ~0x40u;
			OBJECT(current_object)->flags &= ~0x2000u;
			OBJECT(current_object)->flags &= ~0x80000u;
			OBJECT(current_object)->flags &= 0xDFFFFFFFu;
			PLAYER(current_player)->stages_reached++;
			goal_angle = (uint16_t)(0x1225u - OBJECT(current_object)->x_int);
			dy = (uint16_t)(0x1439u - OBJECT(current_object)->y_int);
			angle_of_vector(dy, &goal_angle, &ignored);
			OBJECT(current_object)->direction = goal_angle;
			face_movement_direction(goal_angle);
			OBJECT(current_object)->speed = 0x4000000;
			PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
			set_speed_from_direction();
			OBJECT(current_object)->animation_timer = 0;
			player_animate_standing();
			return;
		case 1:
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
				PLAYER(current_player)->goal_step++;
			update_screen_position(&ignored_3, &ignored_2);
			OBJECT(current_object)->animation_timer = 2;
			player_animate_standing();
			return;
		case 2:
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			project_object(&ignored_3, &ignored_2);
			goal_dx = (int16_t)(0x1225u - OBJECT(current_object)->x_int);
			goal_dy = (int16_t)(0x1439u - OBJECT(current_object)->y_int);
			if ((int32_t)(goal_dy * goal_dy + goal_dx * goal_dx) <= 0x6400)
			{
				PLAYER(current_player)->goal_step++;
				OBJECT(current_object)->speed_z = -0x40000;
				PLAYER(current_player)->flags |= 8u;
				sound_id = 0x16u;
				play_sound(sound_id, &ignored_2, &ignored_4);
			}
			player_animate_running();
			return;
		case 3:
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
			project_object(&ignored_3, &ignored_2);
			if (OBJECT(current_object)->screen_y >= 0xB8)
			{
				PLAYER(current_player)->goal_step++;
				players_finished[OBJECT(current_object)->player] = 0xFFu;
			}
			if (OBJECT(current_object)->speed_z_int >= 0)
			{
				depth = 0xFFF0u;
				queue_sprite_at_depth(&ignored_3, &depth);
				return;
			}
			ignored_2 = queue_sprite_at_base(ignored_2);
			return;
		case 4:
			PLAYER(current_player)->flags |= 8u;
			return;
		default:
			invalid_jump(0x74A8u + PLAYER(current_player)->goal_step * 3u);
		}
		break;
	default:
		invalid_jump(0x63AAu + stage * 3u);
	}
}

/* State 0x20, hurt and tumbling: substate 0 costs 4 health (not in stage 8 before scroll 0x4000) with
   invulnerable_timer 0x258, substate 1 nothing; knocked flying (player_tumbling_start), it lands,
   bounces twice and lies until it walks off after 0x1E frames, or at once on the trackball or a jump.
   Out of health: knocked-out animation, then state 0x16. */
void player_state_tumbling(uint32_t depth, uint32_t cursor)
{
	uint16_t knocked_out_palette = 0;
	uint32_t at_last_step = 0, facing = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, knocked_out_animations = 0, knocked_out_entry = 0, sprite_depth = 0;
	uint8_t hurt_contact_delay = 0, health = 0;
	uint16_t state_timer = 0, state_timer_2 = 0;

	hurt_contact_delay = PLAYER(current_player)->hurt_contact_delay;
	PLAYER(current_player)->hurt_contact_delay = hurt_contact_delay - 1u;
	if ((int8_t)hurt_contact_delay <= 1)
	{
		PLAYER(current_player)->hurt_contact_delay = 0;
		check_player_contacts(&depth, &cursor);
	}
	if (stage != 8u || scroll_position >= 0x3800u)
		push_player_out_of_wall(&ignored, &ignored_2, &cursor);
	PLAYER(current_player)->state = 0x20u;
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 9:
		if (stage != 8u || scroll_position >= 0x4000u)
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 4u;
			if ((int8_t)health <= 4)
				OBJECT(current_object)->health = 0;
		}
		PLAYER(current_player)->invulnerable_timer = 0x258;
		OBJECT(current_object)->flags &= ~0x1000000u;
		player_tumbling_start(cursor);
		return;
	case 1:
		OBJECT(current_object)->flags |= 0x1000000u;
		player_tumbling_start(cursor);
		return;
	case 2:
		player_tumbling_fly();
		return;
	case 3:
	case 5:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->speed_z = -0x40000;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 6u;
			OBJECT(current_object)->animation_frame = 8u;
			OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
			OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
		}
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		project_object(&ignored, &sprite_depth);
		keep_player_on_screen(sprite_depth);
		add_player_to_lists();
		sprite_depth = queue_sprite_at_base(sprite_depth);
		PLAYER(current_player)->draw_depth = sprite_depth;
		return;
	case 4:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->speed_z = -0x40000;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_frame = 7u;
			animate_step_with_flags(&ignored_2, &ignored_6);
			if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
				flash_hurt_palette();
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
			project_object(&ignored, &ignored_2);
			keep_player_on_screen(ignored_2);
			add_player_to_lists();
			depth = queue_sprite_at_floor();
			PLAYER(current_player)->draw_depth = depth;
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed < 0)
		{
			if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
				flash_hurt_palette();
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
			project_object(&ignored, &ignored_2);
			keep_player_on_screen(ignored_2);
			add_player_to_lists();
			depth = queue_sprite_at_floor();
			PLAYER(current_player)->draw_depth = depth;
			return;
		}
		animate_with_flags(&ignored_2, &ignored_6);
		if (OBJECT(current_object)->animation_frame == 9u)
			OBJECT(current_object)->animation_frame = 5u;
		if (OBJECT(current_object)->animation_frame != 7u)
		{
			if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
				flash_hurt_palette();
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
			project_object(&ignored, &ignored_2);
			keep_player_on_screen(ignored_2);
			add_player_to_lists();
			depth = queue_sprite_at_floor();
			PLAYER(current_player)->draw_depth = depth;
			return;
		}
		OBJECT(current_object)->animation_speed = 0xFFu;
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		add_player_to_lists();
		depth = queue_sprite_at_floor();
		PLAYER(current_player)->draw_depth = depth;
		return;
	case 6:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
		{
			if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
			{
				animate_with_flags(&ignored_2, &at_last_step);
				if ((uint8_t)at_last_step != 0u)
					OBJECT(current_object)->animation_frame = 0xFFu;
			}
		}
		else
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			PLAYER(current_player)->mishap_flag = 0u;
			OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
			OBJECT(current_object)->player_view.hurt_flash_timer = 0x7Fu;
			PLAYER(current_player)->state_timer = 0x1E;
			if ((OBJECT(current_object)->flags & 0x1000000u) == 0u)
			{
				OBJECT(current_object)->flags |= 0x20000000u;
				PLAYER(current_player)->invulnerable_timer = 0x78;
			}
			if (OBJECT(current_object)->health > 0)
			{
				if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
					flash_hurt_palette();
				find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
				project_object(&ignored, &ignored_2);
				keep_player_on_screen(ignored_2);
				add_player_to_lists();
				depth = queue_sprite_at_floor();
				PLAYER(current_player)->draw_depth = depth;
				return;
			}
			PLAYER(current_player)->substate = 8u;
			knocked_out_animations = sonic_knocked_out_animations_ADDRESS;
			knocked_out_palette = 0xA90u;
			if (OBJECT(current_object)->kind != 0u)
			{
				knocked_out_animations = ray_knocked_out_animations_ADDRESS;
				knocked_out_palette = 0xAA0u;
				if (OBJECT(current_object)->kind != 1u)
				{
					knocked_out_animations = mighty_knocked_out_animations_ADDRESS;
					knocked_out_palette = 0xA80u;
				}
			}
			OBJECT(current_object)->sprite_palette &= ~0xFFFu;
			OBJECT(current_object)->sprite_palette |= knocked_out_palette;
			facing = PLAYER(current_player)->facing;
			knocked_out_entry = knocked_out_animations + (facing >> 2) * 4u;
			OBJECT(current_object)->sprite_flags &= ~0x40u;
			OBJECT(current_object)->sprite_flags |= KNOCKED_OUT_ENTRY(knocked_out_entry)->sprite_flags;
			OBJECT(current_object)->animation = knocked_out_animations + KNOCKED_OUT_ENTRY(knocked_out_entry)->animation_offset;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 3u;
		}
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		add_player_to_lists();
		depth = queue_sprite_at_floor();
		PLAYER(current_player)->draw_depth = depth;
		return;
	case 7:
		state_timer_2 = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer_2 - 1u;
		if ((int16_t)state_timer_2 > 1)
		{
			start_jump_if_pressed();
			if (PLAYER(current_player)->state == 4u)
			{
				OBJECT(current_object)->flags &= ~4u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->speed = 0;
				PLAYER(current_player)->animation_speed = 0;
				update_player_blink();
				find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				project_object(&ignored, &ignored_2);
				keep_player_on_screen(ignored_2);
				add_player_to_lists();
				if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
					return;
				ignored_2 = queue_sprite_at_base(ignored_2);
				return;
			}
			read_player_trackball();
			if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) == 0u)
			{
				update_player_blink();
				find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
				OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
				project_object(&ignored, &ignored_2);
				keep_player_on_screen(ignored_2);
				add_player_to_lists();
				if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
					return;
				ignored_2 = queue_sprite_at_base(ignored_2);
				return;
			}
		}
		PLAYER(current_player)->state = 1u;
		OBJECT(current_object)->flags &= ~4u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		update_player_blink();
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored_2 = queue_sprite_at_base(ignored_2);
		return;
	case 8:
		animate(&ignored_2, &ignored_7, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		}
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		ignored_2 = queue_sprite_at_floor();
		return;
	default:
		invalid_jump(0x64D9u + PLAYER(current_player)->substate * 3u);
	}
}

/* Starts the tumble (substate 2): mishap_flag, hurt_contact_delay 0x78, speed_z -0x80000, player_status
   1 when out of health. A toucher of kind 0x17 gives no push (player_tumbling_launch); others knock it
   0x40000 along their direction (0x100 on floor type 0xD): 0 y-, 0x100 x+, 0x200 y+, else x-; then
   player_tumbling_set_launch_speed. */
void player_tumbling_start(uint32_t cursor)
{
	uint16_t knock_direction = 0;
	uint32_t knock_speed_x = 0, knock_speed_y = 0, object = 0;
	uint32_t push_speed_4 = 0, push_speed_3 = 0;

	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->mishap_flag = 0xFFu;
	PLAYER(current_player)->hurt_contact_delay = 0x78;
	PLAYER(current_player)->substate = 2u;
	OBJECT(current_object)->sprite_flags &= ~0xFu;
	OBJECT(current_object)->sprite_flags |= 4u;
	if ((uint8_t)OBJECT(current_object)->health == 0u)
		player_status[OBJECT(current_object)->player] = 1u;
	OBJECT(current_object)->speed_z = -0x80000;
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if (OBJECT(object)->kind == 0x17u)
	{
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		player_tumbling_launch(object, cursor);
		return;
	}
	knock_speed_x = 0x40000u;
	knock_speed_y = 0u;
	if (OBJECT(current_object)->floor_type != 0xDu)
		knock_direction = OBJECT(object)->direction;
	else
		knock_direction = 0x100u;
	if (knock_direction == 0u)
	{
		push_speed_4 = knock_speed_x;
		knock_speed_x = knock_speed_y;
		knock_speed_y = 0u - push_speed_4;
		OBJECT(current_object)->speed_x = knock_speed_x;
		OBJECT(current_object)->speed_y = knock_speed_y;
		player_tumbling_set_launch_speed(object, cursor);
		return;
	}
	if (knock_direction != 0x200u)
	{
		if (knock_direction == 0x100u)
		{
			OBJECT(current_object)->speed_x = knock_speed_x;
			OBJECT(current_object)->speed_y = knock_speed_y;
			player_tumbling_set_launch_speed(object, cursor);
			return;
		}
		knock_speed_x = 0u - knock_speed_x;
		OBJECT(current_object)->speed_x = knock_speed_x;
		OBJECT(current_object)->speed_y = knock_speed_y;
		player_tumbling_set_launch_speed(object, cursor);
		return;
	}
	push_speed_3 = knock_speed_x;
	knock_speed_x = knock_speed_y;
	knock_speed_y = push_speed_3;
	OBJECT(current_object)->speed_x = knock_speed_x;
	OBJECT(current_object)->speed_y = knock_speed_y;
	player_tumbling_set_launch_speed(object, cursor);
}

/* Unused: takes twice the toucher's speed_x and speed_y in place of player_tumbling_start's fixed push,
   then player_tumbling_set_launch_speed. */
void player_tumbling_knock_with_toucher_speed(uint32_t toucher, uint32_t effect)
{
	OBJECT(current_object)->speed_x = OBJECT(toucher)->speed_x;
	OBJECT(current_object)->speed_x = (uint32_t)OBJECT(current_object)->speed_x << 1;
	OBJECT(current_object)->speed_y = OBJECT(toucher)->speed_y;
	OBJECT(current_object)->speed_y = (uint32_t)OBJECT(current_object)->speed_y << 1;
	player_tumbling_set_launch_speed(toucher, effect);
}

/* speed = 0x4000000 for a knocked-back tumble, then player_tumbling_launch. */
void player_tumbling_set_launch_speed(uint32_t toucher, uint32_t effect)
{
	OBJECT(current_object)->speed = 0x4000000;
	player_tumbling_launch(toucher, effect);
}

/* Turns the player to the toucher's direction (0x100 on floor type 0xD) and starts the character's
   tumble animation for its facing, its normal palette with a hurt flash every 3 frames, its voice (sound
   0x3A, 0x61 or 0x6E) and spawn_tumble_effect(effect); then the first frame of player_tumbling_fly. */
void player_tumbling_launch(uint32_t toucher, uint32_t effect)
{
	uint8_t sound_id = 0;
	uint32_t ignored_2 = 0, ignored_3 = 0, tumble_animations = 0;

	if (OBJECT(current_object)->floor_type != 0xDu)
		OBJECT(current_object)->direction = OBJECT(toucher)->direction;
	else
		OBJECT(current_object)->direction = 0x100u;
	face_movement_direction(0u);
	tumble_animations = sonic_tumble_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		tumble_animations = ray_tumble_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			tumble_animations = mighty_tumble_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = tumble_animations + PLAYER_FACING_ANIMATION_OFFSETS(tumble_animations)[PLAYER(current_player)->facing];
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	OBJECT(current_object)->player_view.hurt_flash_period = 3u;
	OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
	OBJECT(current_object)->player_view.hurt_flash_on = 0u;
	sound_id = 0x3Au;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x61u;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x6Eu;
	}
	play_sound(sound_id, &ignored_2, &ignored_3);
	spawn_tumble_effect(effect);
	player_tumbling_fly();
}

/* The tumble's flight: gravity 0x6000 a frame, moves, and steps back where the floor is 8 or more above
   it; tumble frames while hurt_contact_delay is above 0x50 or it is in the air (frames 5 and 6 when
   falling fast). On landing substate 3, frame 7, sound 0x20. Sorted at its floor (the previous one where
   there is none). */
void player_tumbling_fly(void)
{
	uint8_t pose_frame = 0, sound_id = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 8)
	{
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	if (PLAYER(current_player)->hurt_contact_delay > 0x50 || OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		if (OBJECT(current_object)->speed_z_int < 6)
		{
			animate_with_flags(&ignored_2, &ignored_6);
			if (OBJECT(current_object)->animation_frame == 5u)
				OBJECT(current_object)->animation_frame = 0xFFu;
		}
		else
		{
			pose_frame = 5u;
			if (OBJECT(current_object)->speed_z_int >= 7)
				pose_frame = 6u;
			OBJECT(current_object)->animation_frame = pose_frame;
			animate_step_with_flags(&ignored_2, &ignored_6);
		}
	}
	else
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->state_timer = 2;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->animation_frame = 7u;
		animate_step_with_flags(&ignored_2, &ignored_6);
		sound_id = 0x20u;
		play_sound(sound_id, &ignored_2, &ignored_3);
	}
	add_player_to_lists();
	project_object(&ignored, &ignored_2);
	keep_player_on_screen(ignored_2);
	if (OBJECT(current_object)->floor_block >= 0)
		depth = queue_sprite_at_floor();
	else
		depth = queue_sprite_at_previous_floor();
	PLAYER(current_player)->draw_depth = depth;
}

/* State 0x1F, blown away: on its first frame the character's tumble animation, speed_z -0x80000 and
   speed_x -0x10000, mishap_flag, its voice (sound 0x3A, 0x61 or 0x6E) and a blown_away_effect_update
   object over it. It flies under gravity to screen y 0x108, then loses 8 health: player_respawn with
   health left, else state 0x16. */
void player_state_blown_away(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, tumble_animations = 0;
	uint8_t health = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		tumble_animations = sonic_tumble_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			tumble_animations = ray_tumble_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				tumble_animations = mighty_tumble_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = tumble_animations + PLAYER_FACING_ANIMATION_OFFSETS(tumble_animations)[2];
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->speed_x = -0x10000;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		sound_id = 0x3Au;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x61u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x6Eu;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		allocate_object(&free_count, &cursor);
		if (free_count != 0u)
		{
			OBJECT(cursor)->sprite_flags = 0x1004u;
			OBJECT(cursor)->sprite_palette = 0x8360u;
			OBJECT(cursor)->sprite_offset_x = 0;
			OBJECT(cursor)->sprite_offset_y = 0;
			OBJECT(cursor)->flags = 0u;
			OBJECT(cursor)->handler = blown_away_effect_update_ADDRESS;
			OBJECT(cursor)->animation = blown_away_effect_animation_ADDRESS;
			OBJECT(cursor)->animation_frame = 0u;
			OBJECT(cursor)->animation_speed = 4u;
			OBJECT(cursor)->animation_timer = 0;
			OBJECT(cursor)->parent = current_object;
		}
	}
	animate_with_flags(&ignored, &ignored_3);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && OBJECT(current_object)->animation_frame == 5u)
		OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	project_object(&ignored_4, &ignored);
	if (OBJECT(current_object)->screen_y < 0x108)
	{
		ignored = queue_sprite(ignored);
		return;
	}
	health = OBJECT(current_object)->health;
	OBJECT(current_object)->health = health - 8u;
	if ((int8_t)health > 8)
		OBJECT(current_object)->handler = player_respawn_ADDRESS;
	else
	{
		OBJECT(current_object)->health = 0;
		PLAYER(current_player)->state = 0x16u;
		PLAYER(current_player)->substate = 0u;
	}
	ignored = queue_sprite(ignored);
}

/* The effect over a blown-away player (player_state_blown_away): at its parent's screen position,
   removed once that reaches screen y 0x100; cycles steps 0-2 of its animation at depth screen_y + 0x10. */
void blown_away_effect_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	if (OBJECT(current_object)->screen_y >= 0x100)
	{
		delete_current_object();
		return;
	}
	animate(&ignored, &ignored_2, &ignored_3);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && OBJECT(current_object)->animation_frame == 2u)
		OBJECT(current_object)->animation_frame = 0xFFu;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* State 0x1E, the game intro: waits until intro_character is its character, then requests path 0x14 once
   and moves as previous_state (player_move). */
void player_state_intro_turn(uint32_t depth, uint32_t cursor)
{
	if ((uint8_t)intro_character != OBJECT(current_object)->kind)
		return;
	if (PLAYER(current_player)->scene_step_seen == 0u)
	{
		PLAYER(current_player)->scene_step_seen = 0xFFu;
		PLAYER(current_player)->path_request = 0x14u;
	}
	PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
	player_move(depth, cursor);
}

/* State 0x1C, the opening scene (game mode 0x10), by scene_step: in steps 1 and 4 the player runs toward
   the screen from its place in opening_scene_player_positions, growing until scene_timer 0x12C, shaken
   with the stage 6 quake, its voice at 0x140; in step 3 a still frame on opening_machine_object, scaled
   with it; hidden in steps 0 and 2. */
void player_state_opening_scene(void)
{
	uint8_t sound_id = 0;
	uint16_t character_frame = 0, shake = 0, sprite_height = 0, width = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, layout_row = 0, object = 0, opening_run_animation = 0, opening_voice_animation = 0, players_left = 0, position_record = 0, sprite_scale = 0, status_cursor = 0;
	uint8_t status = 0;

	switch (scene_step)
	{
	case 0:
	case 2:
		return;
	case 1:
	case 4:
		if (PLAYER(current_player)->scene_step_seen != scene_step)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			opening_run_animation = sonic_opening_run_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				opening_run_animation = ray_opening_run_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					opening_run_animation = mighty_opening_run_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = opening_run_animation;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 2u;
			OBJECT(current_object)->animation_timer = 0;
			status_cursor = player_status_ADDRESS;
			players_left = 3u;
			layout_row = 0xFFFFFFFFu;
			do
			{
				status = mem_read8(status_cursor);
				status_cursor++;
				if (status == 0xFFu)
					layout_row++;
				players_left--;
			} while (players_left != 0u);
			position_record = opening_scene_player_positions_ADDRESS + layout_row * 0x12u + OBJECT(current_object)->player * 3u * 2u;
			OBJECT(current_object)->screen_x = OPENING_SCENE_POSITION(position_record)->screen_x;
			position_record += 2u;
			OBJECT(current_object)->projected_y_int = OPENING_SCENE_POSITION(position_record - 0x2u)->projected_y;
			OBJECT(current_object)->projected_y_fraction = 0u;
			position_record += 2u;
			PLAYER(current_player)->scene_zoom_int = OPENING_SCENE_POSITION(position_record - 0x4u)->zoom;
			PLAYER(current_player)->scene_zoom_fraction = 0u;
			position_record += 2u;
			spawn_intro_marker(position_record);
		}
		if ((uint16_t)scene_timer < 0x12Cu)
		{
			PLAYER(current_player)->scene_zoom += 0x6000u;
			OBJECT(current_object)->projected_y += 0x3000u;
		}
		OBJECT(current_object)->sprite_scale = PLAYER(current_player)->scene_zoom_int;
		OBJECT(current_object)->screen_y = OBJECT(current_object)->projected_y_int;
		shake = stage_06_shake_pixels;
		if (stage_06_shake_sign < 0)
			shake = (uint16_t)(0u - shake);
		OBJECT(current_object)->screen_y += shake;
		if ((uint16_t)scene_timer != 0x140u)
		{
			animate(&ignored, &ignored_3, &ignored_4);
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			if (OBJECT(current_object)->player != 1u && (int16_t)depth >= 0xED)
				depth = 0xEDu;
			queue_sprite_at_depth(&ignored_5, &depth);
			return;
		}
		opening_voice_animation = sonic_opening_voice_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			opening_voice_animation = ray_opening_voice_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				opening_voice_animation = mighty_opening_voice_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = opening_voice_animation;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x46u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		animate(&ignored, &ignored_3, &ignored_4);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		if (OBJECT(current_object)->player != 1u && (int16_t)depth >= 0xED)
			depth = 0xEDu;
		queue_sprite_at_depth(&ignored_5, &depth);
		return;
	case 3:
		if (PLAYER(current_player)->scene_step_seen != scene_step)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			OBJECT(current_object)->sprite_flags = 0u;
			OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
			character_frame = 0x1B58u;
			if (OBJECT(current_object)->kind != 0u)
			{
				character_frame = 0x5278u;
				if (OBJECT(current_object)->kind != 1u)
					character_frame = 0x36C0u;
			}
			OBJECT(current_object)->sprite_frame = character_frame;
			width = 0u;
			sprite_height = 0x30u;
			if (OBJECT(current_object)->player != 0u)
			{
				width = 0xFFE0u;
				sprite_height = 0x2Eu;
				if (OBJECT(current_object)->player != 1u)
				{
					width = 0x20u;
					sprite_height = 0x2Eu;
				}
			}
			OBJECT(current_object)->sprite_width = width;
			OBJECT(current_object)->sprite_height = sprite_height;
		}
		object = (current_object & ~0xFFFFu) | opening_machine_object;
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x += (int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8;
		OBJECT(current_object)->screen_y += (int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8;
		depth = (uint16_t)(OBJECT(object)->screen_y - 8u);
		queue_sprite_at_depth(&ignored_5, &depth);
		return;
	default:
		invalid_jump(0x6C19u + scene_step * 3u);
	}
}

/* Unused: does nothing; a lone return after player_state_opening_scene's last jump. */
void player_state_opening_scene_spare_return(void)
{
}

/* Unused: does nothing; a lone return after the opening scene's shadow code, where shadow_update jumps
   in game mode 0x10. */
void shadow_update_spare_return(void)
{
}

/* State 0x18, the stage 0 and 5 scenes by scene_step: 0 moves as usual; 1 and 8 walk scene_path, then
   its players_finished entry; 2 waits at (0x1C0, 0x154D) for stage_05_switch_step to finish, then path
   0xE; 3 stands; 4 path 5; 5-6 hop on stage_00_geyser and rise; 7 shrinks away to the screen centre
   after a delay by character, sound 0x16, voice. */
void player_state_stage_scene(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t y_shift = 0;
	uint32_t geyser_hop_animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, kind = 0, position_record = 0, shrink_away_animation = 0, sprite_zoom = 0, standing_animations = 0, x_shift = 0;
	uint16_t state_timer = 0, sprite_scale = 0;

	switch (scene_step)
	{
	case 0:
		player_move(depth, cursor);
		return;
	case 1:
	case 8:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u)
			{
				PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
				player_move(depth, cursor);
				return;
			}
			players_finished[OBJECT(current_object)->player] = 0xFFu;
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		OBJECT(current_object)->flags |= 0x4000000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->path_request = PLAYER(current_player)->scene_path;
		stage_exit_started = 0xFFu;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
		player_move(depth, cursor);
		return;
	case 2:
		if (scene_step != PLAYER(current_player)->scene_step_seen)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			PLAYER(current_player)->heading = 0x280u;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->path_request = 0xEu;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->x = 0x1C00000;
			OBJECT(current_object)->y = 0x154D0000;
		}
		if (stage_05_switch_step < 0)
		{
			player_move(depth, cursor);
			return;
		}
		return;
	case 3:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite_at_base(ignored);
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		standing_animations = sonic_standing_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			standing_animations = ray_standing_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				standing_animations = mighty_standing_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = standing_animations + PLAYER_FACING_ANIMATION_OFFSETS(standing_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation_frame = 0xFFu;
		animate_step_with_flags(&ignored, &ignored_2);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_base(ignored);
		return;
	case 4:
		if (scene_step != PLAYER(current_player)->scene_step_seen)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			PLAYER(current_player)->path_request = 5u;
		}
		player_move(depth, cursor);
		return;
	case 5:
		geyser_hop_animation = sonic_geyser_hop_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			geyser_hop_animation = ray_geyser_hop_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				geyser_hop_animation = mighty_geyser_hop_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = geyser_hop_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		PLAYER(current_player)->scene_hop_z = 0;
		OBJECT(current_object)->speed_z = -0x30000;
		OBJECT(current_object)->x = 0x17D0000;
		OBJECT(current_object)->y = 0x154D0000;
		y_shift = 0u;
		if (OBJECT(current_object)->player != 0u)
		{
			y_shift = 0x20u;
			if (OBJECT(current_object)->player != 1u)
				y_shift = (uint16_t)(0u - y_shift);
		}
		OBJECT(current_object)->y_int += y_shift;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x46u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &ignored, &ignored_4);
		project_object(&ignored_3, &ignored);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
		ignored = queue_sprite(ignored);
		return;
	case 6:
		if (scene_timer <= 0x90)
		{
			OBJECT(current_object)->floor_z = OBJECT((current_object & ~0xFFFFu) | stage_00_geyser)->z;
			OBJECT(current_object)->floor_z_int -= 0x20u;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			kind = OBJECT(current_object)->kind;
			OBJECT(current_object)->speed_z += (uint16_t)(kind << 12) + 0x6000u;
			PLAYER(current_player)->scene_hop_z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->z += PLAYER(current_player)->scene_hop_z;
			if (PLAYER(current_player)->scene_hop_z_int >= 0)
			{
				OBJECT(current_object)->speed_z = -0x30000;
				PLAYER(current_player)->scene_hop_z = 0;
			}
			animate(&depth, &ignored_5, &ignored_2);
			update_screen_position(&ignored_3, &depth);
			depth = queue_sprite(depth);
			return;
		}
		x_shift = 0u;
		if (OBJECT(current_object)->kind != 0u)
		{
			x_shift = 0xFFFF0000u;
			if (OBJECT(current_object)->kind != 1u)
				x_shift = 0x10000u;
		}
		PLAYER(current_player)->screen_x -= x_shift;
		OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
		OBJECT(current_object)->z_int -= 0xDu;
		animate(&depth, &ignored_5, &ignored_2);
		update_screen_position(&ignored_3, &depth);
		depth = queue_sprite(depth);
		return;
	case 7:
		if (scene_step != PLAYER(current_player)->scene_step_seen)
		{
			PLAYER(current_player)->scene_step_seen = scene_step;
			shrink_away_animation = sonic_shrink_away_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 0u)
			{
				shrink_away_animation = ray_shrink_away_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 1u)
					shrink_away_animation = mighty_shrink_away_animation_ADDRESS;
			}
			OBJECT(current_object)->animation = shrink_away_animation;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->sprite_flags &= ~0xCu;
			position_record = shrink_away_positions_ADDRESS + OBJECT(current_object)->kind * 4u;
			PLAYER(current_player)->screen_x = (int16_t)(SCREEN_POINT(position_record)->x - 0xA0u);
			OBJECT(current_object)->projected_y = (int16_t)(SCREEN_POINT(position_record)->y - 0x70u);
			PLAYER(current_player)->scene_zoom = 0;
			PLAYER(current_player)->state_timer = OBJECT(current_object)->kind * 0x1Eu + 0x3Cu;
		}
		if ((uint16_t)PLAYER(current_player)->state_timer == 1u)
		{
			sound_id = 0x16u;
			play_sound(sound_id, &ignored, &ignored_4);
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &ignored, &ignored_4);
		}
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer > 1)
			return;
		PLAYER(current_player)->state_timer = 0;
		if ((int16_t)OBJECT(current_object)->sprite_scale > 0x20)
		{
			PLAYER(current_player)->scene_zoom += 8u;
			OBJECT(current_object)->sprite_scale = divide_s32(0x8000u, PLAYER(current_player)->scene_zoom + 0x10u);
			sprite_zoom = OBJECT(current_object)->sprite_scale;
			OBJECT(current_object)->screen_x = ((int32_t)(PLAYER(current_player)->screen_x * sprite_zoom) >> 11) + 0xA0u;
			OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->projected_y * sprite_zoom) >> 11) + 0x70u;
			animate(&sprite_zoom, &ignored_5, &ignored_2);
			sprite_zoom = queue_sprite(sprite_zoom);
			return;
		}
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->sprite_scale = sprite_scale - 1u;
		if ((int16_t)sprite_scale <= 1)
			return;
		sprite_zoom = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x = ((int32_t)(PLAYER(current_player)->screen_x * sprite_zoom) >> 11) + 0xA0u;
		OBJECT(current_object)->screen_y = ((int32_t)(OBJECT(current_object)->projected_y * sprite_zoom) >> 11) + 0x70u;
		animate(&sprite_zoom, &ignored_5, &ignored_2);
		sprite_zoom = queue_sprite(sprite_zoom);
		return;
	default:
		invalid_jump(0x6F06u + scene_step * 3u);
	}
}

/* State 0x2F, stunned: unless contacts hurt it (state 0xD), it loses 4 health and shows its stunned
   frame (Ray animated) for 0x3C frames on its floor, then dies (state 0x16) with no health left, or
   plays its recovery animation and stands (state 0) facing direction 0x300 with control_lock 4. */
void player_state_stunned(uint32_t depth, uint32_t cursor)
{
	uint16_t stunned_frame = 0;
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, stun_recover_animation = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	check_zone_contact();
	check_player_contacts(&depth, &cursor);
	if (PLAYER(current_player)->state == 0xDu)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 3:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->state_timer = 0x3C;
		stunned_frame = 0x8410u;
		if (OBJECT(current_object)->kind != 0u)
		{
			stunned_frame = 0x8450u;
			if (OBJECT(current_object)->kind != 1u)
				stunned_frame = 0x83D0u;
		}
		OBJECT(current_object)->sprite_frame = stunned_frame;
		if (OBJECT(current_object)->kind == 1u)
		{
			OBJECT(current_object)->animation = ray_stunned_animation_ADDRESS;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_timer = 0;
		}
		health = OBJECT(current_object)->health;
		OBJECT(current_object)->health = health - 4u;
		if ((int8_t)health <= 4)
			OBJECT(current_object)->health = 0;
		break;
	case 1:
		break;
	case 2:
		animate(&ignored, &ignored_2, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			PLAYER(current_player)->state = 0u;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->control_lock = 4u;
			OBJECT(current_object)->direction = 0x300u;
			face_movement_direction(0u);
		}
		find_floor(&ignored_4, &ignored, &ignored_5, &ignored_6, &ignored_7);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		update_screen_position(&ignored_4, &ignored);
		add_player_to_lists();
		ignored = queue_sprite_at_base(ignored);
		return;
	default:
		invalid_jump(0x72D0u + PLAYER(current_player)->substate * 3u);
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer > 1)
	{
		if (OBJECT(current_object)->kind == 1u)
			animate(&ignored, &ignored_2, &ignored_3);
		find_floor(&ignored_4, &ignored, &ignored_5, &ignored_6, &ignored_7);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		update_screen_position(&ignored_4, &ignored);
		add_player_to_lists();
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->substate++;
	if (OBJECT(current_object)->health <= 0)
	{
		PLAYER(current_player)->state = 0x16u;
		PLAYER(current_player)->substate = 0u;
		if (OBJECT(current_object)->kind == 1u)
			animate(&ignored, &ignored_2, &ignored_3);
		find_floor(&ignored_4, &ignored, &ignored_5, &ignored_6, &ignored_7);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		update_screen_position(&ignored_4, &ignored);
		add_player_to_lists();
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	stun_recover_animation = sonic_stun_recover_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		stun_recover_animation = ray_stun_recover_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			stun_recover_animation = mighty_stun_recover_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = stun_recover_animation;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->animation_timer = 0;
	if (OBJECT(current_object)->kind == 1u)
		animate(&ignored, &ignored_2, &ignored_3);
	find_floor(&ignored_4, &ignored, &ignored_5, &ignored_6, &ignored_7);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	update_screen_position(&ignored_4, &ignored);
	add_player_to_lists();
	ignored = queue_sprite_at_base(ignored);
}

/* State 0x34, choosing a character: on its first frame keeps game_frames in join_time and, in game mode
   0x1B, opens its chooser (start_character_chooser). Once the game mode is not 0x1B it starts the stage
   (handler player_start_stage, is_joining 0), its character becoming intro_character if none is set yet. */
void player_state_choosing_character(uint32_t cursor)
{
	if (PLAYER(current_player)->substate != 0u)
	{
		if (game_mode != 0x1Bu)
		{
			OBJECT(current_object)->handler = player_start_stage_ADDRESS;
			PLAYER(current_player)->is_joining = 0u;
			if (intro_character < 0)
				intro_character = OBJECT(current_object)->kind;
		}
		return;
	}
	PLAYER(current_player)->substate = 0xFFu;
	PLAYER(current_player)->join_time = game_frames;
	if (game_mode != 0x1Bu)
	{
		OBJECT(current_object)->handler = player_start_stage_ADDRESS;
		PLAYER(current_player)->is_joining = 0u;
		if (intro_character < 0)
			intro_character = OBJECT(current_object)->kind;
		return;
	}
	start_character_chooser(cursor);
	if (game_mode != 0x1Bu)
	{
		OBJECT(current_object)->handler = player_start_stage_ADDRESS;
		PLAYER(current_player)->is_joining = 0u;
		if (intro_character < 0)
			intro_character = OBJECT(current_object)->kind;
	}
}

/* State 0x39, game mode 0x20 (YOU FAILED): once you_failed_done is set it starts the continue countdown
   (state 0x23 with 9 seconds, player_status 0xFF, in_game 0); drawn until game_mode_timer reaches 0x3C. */
void player_state_you_failed(void)
{
	uint32_t ignored = 0;

	if (you_failed_done != 0u)
	{
		player_status[OBJECT(current_object)->player] = 0xFFu;
		PLAYER(current_player)->previous_z_low = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
	}
	if (game_mode_timer >= 0x3C)
		return;
	ignored = queue_sprite(0u);
}

/* State 0x15, held by its holder (an object of kind 8 or 0xA): while substate is 0 or the holder's
   vine_bar.holding is set it hangs at the holder's x, y - 8 and z + 0x20, a rider, drawn at depth 0xFF;
   when it lets go, state 0x17 (hopping) at speed 0, run at once. */
void player_state_held(uint32_t depth, uint32_t cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->flags |= 0x10000u;
	if (PLAYER(current_player)->substate == 0u || OBJECT(LINKED(current_object, PLAYER(current_player)->holder))->vine_bar.holding != 0u)
	{
		PLAYER(current_player)->substate = 1u;
		object = LINKED(current_object, PLAYER(current_player)->holder);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->y_int -= 8u;
		OBJECT(current_object)->z = OBJECT(object)->z;
		OBJECT(current_object)->z_int += 0x20u;
		update_player_blink();
		remember_platform();
		add_to_rider_list();
		project_object(&ignored, &ignored_2);
		depth = 0xFFu;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	PLAYER(current_player)->state = 0x17u;
	OBJECT(current_object)->flags |= 0x200000u;
	OBJECT(current_object)->flags &= ~0x10000u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->animation_speed = 0;
	player_dispatch_state(depth, cursor);
}

/* State 0x1D, stuck in an object of kind 0x11: runs its substate (player_stuck_begin,
   player_stuck_wait_release, player_stuck_shake, player_stuck_jump_out), then moves with that object
   (touched_by): its speed_z, and the word at its var_80 added to screen_x; drawn at draw_depth, which
   follows only while the object is still. */
void player_state_stuck(uint32_t cursor)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_stuck_begin(cursor);
		break;
	case 1:
		player_stuck_wait_release();
		break;
	case 2:
		player_stuck_shake();
		break;
	case 3:
		player_stuck_jump_out();
		break;
	case 4:
		player_stuck_begin(cursor);
		break;
	default:
		invalid_jump(0x7709u + PLAYER(current_player)->substate * 3u);
	}
	remember_platform();
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	if ((uint32_t)OBJECT(object)->speed_z == 0u)
	{
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		depth = (uint16_t)(depth - OBJECT(current_object)->sprite_offset_y);
		PLAYER(current_player)->draw_depth = depth;
	}
	else
		OBJECT(current_object)->z += OBJECT(object)->speed_z;
	update_screen_position(&ignored, &depth);
	OBJECT(current_object)->screen_x += OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->propped_board.shake_dx;
	find_floor(&ignored, &depth, &ignored_2, &ignored_3, &ignored_4);
	keep_player_on_screen(depth);
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

/* Substate 0 of state 0x1D: the character's stuck frame with its normal palette, control_lock and
   escape_progress 0, a stuck_effect_update object over the player, sign kind 0xD 0x28 above it
   (sign_show_while_player_busy) and sound 0x23. */
void player_stuck_begin(uint32_t object)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, stuck_frame = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	PLAYER(current_player)->substate++;
	stuck_frame = 0x1A40u;
	if (OBJECT(current_object)->kind != 0u)
	{
		stuck_frame = 0x5180u;
		if (OBJECT(current_object)->kind != 1u)
			stuck_frame = 0x35C8u;
	}
	OBJECT(current_object)->sprite_frame = stuck_frame;
	OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->escape_progress = 0;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x8380u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->projected_y_int += 0xAu;
		OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(object)->projected_x += 5u;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(object)->handler = stuck_effect_update_ADDRESS;
		OBJECT(object)->animation = stuck_effect_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->kind = OBJECT(current_object)->kind;
		OBJECT(object)->player = OBJECT(current_object)->player;
		OBJECT(object)->parent = current_object;
	}
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	kind = 0xDu;
	z = (uint16_t)(OBJECT(current_object)->z_int - 0x28u);
	spawn_sign_2(x_int, y_int, kind, z, object);
	sound_id = 0x23u;
	play_sound(sound_id, &ignored, &ignored_2);
	project_object(&ignored_3, &ignored);
}

/* Waits until the object it is stuck in (touched_by) has its word at var_80 back to 0, then substate 2:
   its animation restarted at speed 4 and the character's voice (sound 0x35, 0x31 or 0x3D). */
void player_stuck_wait_release(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	if ((uint16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->propped_board.shake_dx != 0u)
		return;
	PLAYER(current_player)->substate++;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	sound_id = 0x35u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x31u;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x3Du;
	}
	play_sound(sound_id, &ignored, &ignored_2);
}

/* Shaking free: escape_progress grows by the trackball's speed_int while it pushes at speed 0x100 or
   more toward headings 0x180-0x380, back to 0 otherwise; the stuck animation at 0, else one of three
   struggle animations by progress. At 0x10000 the escape animation, sound 0x2B and substate 3. */
void player_stuck_shake(void)
{
	uint8_t sound_id = 0;
	uint16_t trackball_y_2 = 0;
	uint32_t animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, speed = 0, struggle_index = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed &= 0xFFFFu;
	speed = speed * speed;
	if ((int32_t)speed > 0x5000000)
		speed = 0x5000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored_2);
	PLAYER(current_player)->target_heading = trackball_angle;
	if (OBJECT(current_object)->speed_int < 0x100 || PLAYER(current_player)->target_heading < 0x180 || PLAYER(current_player)->target_heading > 0x380)
		PLAYER(current_player)->escape_progress = 0;
	else
		PLAYER(current_player)->escape_progress += (uint16_t)OBJECT(current_object)->speed_int;
	if ((uint32_t)PLAYER(current_player)->escape_progress == 0u)
	{
		animation = sonic_stuck_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			animation = ray_stuck_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				animation = mighty_stuck_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = animation;
		animate(&ignored, &ignored_3, &ignored_4);
		return;
	}
	animation = sonic_stuck_struggle_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation = ray_stuck_struggle_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation = mighty_stuck_struggle_animations_ADDRESS;
	}
	struggle_index = (int32_t)PLAYER(current_player)->escape_progress / 0x5555;
	if ((int16_t)struggle_index > 2)
		struggle_index = 2u;
	OBJECT(current_object)->animation = mem_read32(animation + struggle_index * 4u);
	animate(&ignored, &ignored_3, &ignored_4);
	if (PLAYER(current_player)->escape_progress < 0x10000)
		return;
	PLAYER(current_player)->substate++;
	animation = sonic_stuck_escape_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation = ray_stuck_escape_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation = mighty_stuck_escape_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = animation;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	sound_id = 0x2Bu;
	play_sound(sound_id, &ignored, &ignored_5);
}

/* Once the escape animation ends, bounces out: the character's bounce record 1 (direction, flip,
   animation), speed 0x4000000, speed_z -0x34000, state 0xE (bouncing) substate 1 for 0x14 frames. */
void player_stuck_jump_out(void)
{
	uint32_t at_last_step = 0, bounce_record = 0, bounce_records = 0, ignored = 0, ignored_2 = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
		return;
	PLAYER(current_player)->bounce_direction = 1u;
	bounce_records = sonic_bounce_records_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		bounce_records = ray_bounce_records_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			bounce_records = mighty_bounce_records_ADDRESS;
	}
	bounce_record = bounce_records + PLAYER(current_player)->bounce_direction * 3u * 2u;
	OBJECT(current_object)->direction = PLAYER_BOUNCE_RECORD(bounce_record)->direction;
	OBJECT(current_object)->sprite_flags &= ~0x40u;
	bounce_record += 2u;
	OBJECT(current_object)->sprite_flags |= PLAYER_BOUNCE_RECORD(bounce_record - 0x2u)->sprite_flags;
	OBJECT(current_object)->animation = PLAYER_BOUNCE_RECORD(bounce_record - 0x2u)->animation_offset + bounce_records;
	face_movement_direction(0u);
	OBJECT(current_object)->speed = 0x4000000;
	OBJECT(current_object)->speed_z = -0x34000;
	PLAYER(current_player)->bounce_speed_z = -0x34000;
	PLAYER(current_player)->bounce_step = 0x1A000;
	PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
	set_speed_from_direction();
	OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[1];
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 3;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->state_timer = 0x14;
	PLAYER(current_player)->state = 0xEu;
	PLAYER(current_player)->substate = 1u;
}

/* The object over a stuck player: follows propped_board_object down and its shake, drawn at its
   draw_depth (screen_y - 0x20 while the board is still); removed off screen. Animated while its player
   struggles (substates 0, 1, 4), sprite 0x1AA8 in substate 2, sprite 0x1AB0 with the animation over once
   the player bounces out (state 0xE). */
void stuck_effect_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	OBJECT(current_object)->z += OBJECT(LINKED(current_object, propped_board_object))->speed_z;
	update_screen_position(&ignored, &ignored_2);
	if ((uint32_t)OBJECT(LINKED(current_object, propped_board_object))->speed_z == 0u)
		OBJECT(current_object)->stuck_effect.draw_depth = OBJECT(current_object)->screen_y - 0x20u;
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame < 0)
	{
		OBJECT(current_object)->screen_x += OBJECT(LINKED(current_object, propped_board_object))->propped_board.shake_dx;
		depth = OBJECT(current_object)->stuck_effect.draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	r14 = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 9);
	switch (PLAYER(r14)->substate)
	{
	case 0:
	case 1:
	case 4:
		animate(&ignored_2, &ignored_3, &ignored_4);
		break;
	case 2:
		OBJECT(current_object)->sprite_frame = 0x1AA8u;
		OBJECT(current_object)->sprite_palette = 0x8390u;
		break;
	case 3:
		break;
	default:
		invalid_jump(0x7A7Cu + PLAYER(r14)->substate * 2u);
	}
	if (PLAYER(r14)->state == 0xEu)
	{
		OBJECT(current_object)->sprite_frame = 0x1AB0u;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	OBJECT(current_object)->screen_x += OBJECT(LINKED(current_object, propped_board_object))->propped_board.shake_dx;
	depth = OBJECT(current_object)->stuck_effect.draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

/* State 0x38, the stage 0xB sequence by ending_step: walks path 0x4D, then stands facing direction 0
   (stage_b_path_done); when sliding_door_opened, restarts the scroll and walks path 0x50 (the first one
   plays sound 0x94 and its voice); once stage_trigger_bits is set and cleared it runs off on path 0x53
   (state 0x13) with escape_countdown_running. */
void player_state_stage_b_sequence(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, standing_animations = 0;

	for (;;)
	{
		switch (PLAYER(current_player)->ending_step)
		{
		case 0:
		case 6:
			PLAYER(current_player)->ending_step++;
			PLAYER(current_player)->path_request = 0x4Du;
			PLAYER(current_player)->control_lock = 0u;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			player_move(depth, cursor);
			return;
		case 1:
		case 3:
			break;
		case 2:
			if (sliding_door_opened != 0u)
			{
				PLAYER(current_player)->ending_step++;
				PLAYER(current_player)->path_request = 0x50u;
				scroll_mode = 0u;
				scroll_speed = 5u;
				if (stage_b_sound_done != 0u)
				{
					update_screen_position(&ignored, &depth);
					depth = queue_sprite(depth);
					return;
				}
				stage_b_sound_done = 0xFFu;
				sound_id = 0x94u;
				play_sound(sound_id, &depth, &ignored_3);
				sound_id = 0x58u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x4Eu;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x60u;
				}
				play_sound(sound_id, &depth, &ignored_3);
			}
			update_screen_position(&ignored, &depth);
			depth = queue_sprite(depth);
			return;
		case 4:
			if (stage_trigger_bits != 0u)
				PLAYER(current_player)->ending_step++;
			update_screen_position(&ignored, &depth);
			depth = queue_sprite(depth);
			return;
		case 5:
			if (stage_trigger_bits != 0u)
			{
				update_screen_position(&ignored, &depth);
				depth = queue_sprite(depth);
				return;
			}
			PLAYER(current_player)->ending_done = 0xFFu;
			escape_countdown_running = 0xFFu;
			PLAYER(current_player)->state = 0x13u;
			PLAYER(current_player)->path_request = 0x53u;
			update_screen_position(&ignored, &depth);
			depth = queue_sprite(depth);
			return;
		default:
			invalid_jump(0x7AD5u + PLAYER(current_player)->ending_step * 3u);
		}
		if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u)
		{
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		}
		PLAYER(current_player)->ending_step++;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->direction = 0u;
		face_movement_direction(ignored);
		standing_animations = sonic_standing_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			standing_animations = ray_standing_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				standing_animations = mighty_standing_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = standing_animations + PLAYER_FACING_ANIMATION_OFFSETS(standing_animations)[PLAYER(current_player)->facing];
		PLAYER(current_player)->stage_b_path_done = 0xFFu;
		OBJECT(current_object)->animation_frame = 0u;
		animate_step_with_flags(&depth, &ignored_2);
	}
}
