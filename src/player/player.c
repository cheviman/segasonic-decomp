/**
 *    Name:        player/player.c
 *    Description: the player: starting, joining, resetting and placing a player, player_update, the movement (screen edges, trackball, turning, jumps, walls), the shadow, the floor checks, the restart point, the checks of every frame, and check_player_contacts (what touching each kind of object does)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Player 1's object starts: player = 0, then player_start. */
void player_1_start(void)
{
	OBJECT(current_object)->player = 0u;
	player_start();
}

/* Player 2's object starts: player = 1, then player_start. */
void player_2_start(void)
{
	OBJECT(current_object)->player = 1u;
	player_start();
}

/* Player 3's object starts: player = 2, then player_start. */
void player_3_start(void)
{
	OBJECT(current_object)->player = 2u;
	player_start();
}

/* A new game for the player object: links it with its player block (current_player), health =
   starting_health, rings 0, kind = player (its character), state 0xB (0x37 for the third player on a
   2-player cabinet), two full-screen clip entries; in_game 1 when try_join succeeds. Next comes
   player_start_stage. */
void player_start(void)
{
	uint32_t clip_cursor = 0, joined = 0, records_left = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	unused_player_start_byte = 3u;
	PLAYER(current_player)->joined_health_setting = health_setting;
	PLAYER(current_player)->object = current_object;
	OBJECT(current_object)->player_view.block = current_player;
	OBJECT(current_object)->health = starting_health;
	OBJECT(current_object)->handler = player_start_stage_ADDRESS;
	PLAYER(current_player)->state = 0xBu;
	PLAYER(current_player)->stages_reached = 1u;
	PLAYER(current_player)->path_request = 0u;
	PLAYER(current_player)->unused_start_byte = 0u;
	PLAYER(current_player)->rings = 0u;
	PLAYER(current_player)->is_joining = 0u;
	PLAYER(current_player)->ending_done = 0u;
	PLAYER(current_player)->ending_step = 0u;
	PLAYER(current_player)->rings_to_health = 0;
	OBJECT(current_object)->kind = OBJECT(current_object)->player;
	records_left = 2u;
	clip_cursor = current_player + 0xF0u;
	do
	{
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x2u)->command = 0x5000u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x4u)->bottom = 0xE0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x6u)->left = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x8u)->right = 0x140u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0xAu)->unk_0A = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0xCu)->unk_0C = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0xEu)->unk_0E = 0u;
		clip_cursor += 2u;
		CLIP_ENTRY(clip_cursor - 0x10u)->unk_10 = 0u;
		clip_cursor += 2u;
		records_left--;
	} while (records_left != 0u);
	if ((cabinet_config & 1u) == 0u && OBJECT(current_object)->player == 2u)
		PLAYER(current_player)->state = 0x37u;
	PLAYER(current_player)->in_game = 0;
	joined = try_join();
	if (joined != 0u)
		PLAYER(current_player)->in_game = 1;
}

/* Makes player_update the handler, for a player already in the stage (reset_objects_from_list sets it). */
void player_resume(void)
{
	OBJECT(current_object)->handler = player_update_ADDRESS;
}

/* The handler of a player who paid to join: player_status 4 and the character select
   (player_select_character) until it is done, except when current_game_mode and game_mode are both 0x1B;
   then player_reset_joined. */
void player_join(void)
{
	uint32_t player = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	if (current_game_mode != 0x1Bu || game_mode != 0x1Bu)
	{
		player = OBJECT(current_object)->player;
		player_status[player] = 4u;
		player_select_character(player);
		if (PLAYER(current_player)->substate != 0u)
			return;
	}
	OBJECT(current_object)->handler = player_reset_joined_ADDRESS;
	player_reset_joined();
}

/* The player's handler at a stage start: flags bit 25 (a stage start), block flags bit 4 off,
   stage_rings 0, the HUD records (player_hud_init), then player_restart. */
void player_start_stage(void)
{
	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	OBJECT(current_object)->flags |= 0x2000000u;
	PLAYER(current_player)->flags &= ~0x10u;
	PLAYER(current_player)->stage_rings = 0u;
	player_hud_init();
	player_restart();
}

/* The handler set for a player that comes back after losing health (off screen, sunk, fallen):
   player_restart without the stage-start flag (flags bit 25). */
void player_respawn(void)
{
	OBJECT(current_object)->flags &= ~0x2000000u;
	player_restart();
}

/* Restarts the player: in a game (in_game above 0) player_reset; otherwise it does not play: in_game 0,
   state 0xB (0x37 for the third player on a 2-player cabinet), player_update as its handler. */
void player_restart(void)
{
	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	PLAYER(current_player)->flags &= ~0x10u;
	if (PLAYER(current_player)->in_game > 0)
	{
		player_reset();
		return;
	}
	PLAYER(current_player)->previous_z_low = 0u;
	PLAYER(current_player)->in_game = 0;
	PLAYER(current_player)->state = 0xBu;
	if ((cabinet_config & 1u) == 0u && OBJECT(current_object)->player == 2u)
		PLAYER(current_player)->state = 0x37u;
	OBJECT(current_object)->handler = player_update_ADDRESS;
}

/* After joining: block flags bit 4, health = starting_health, rings_to_health 0 and, unless a stage exit
   is under way, unused_player_flags and players_finished cleared for it; then player_reset. */
void player_reset_joined(void)
{
	uint32_t player = 0;

	OBJECT(current_object)->flags &= ~0x2000000u;
	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	PLAYER(current_player)->flags |= 0x10u;
	OBJECT(current_object)->health = starting_health;
	PLAYER(current_player)->rings_to_health = 0;
	if (stage_exit_started != 0u)
	{
		player_reset();
		return;
	}
	player = OBJECT(current_object)->player;
	unused_player_flags[player] = 0u;
	players_finished[player] = 0u;
	player_reset();
}

/* Resets the player for a start or a respawn: speeds, movement, path and timers cleared, state 0, 0x78
   frames invulnerable and blinking unless a stage starts (flags bit 25), the stage's palette for its
   character; plays its character's join or respawn voice unless a stage starts or current_game_mode is
   0x1B; then player_place. */
void player_reset(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, stage_index = 0;

	OBJECT(current_object)->sprite_palette &= 0x5FFFu;
	PLAYER(current_player)->invulnerable_timer = 0;
	OBJECT(current_object)->flags &= 0xDFFFFFFFu;
	if ((OBJECT(current_object)->flags & 0x2000000u) == 0u)
	{
		PLAYER(current_player)->invulnerable_timer = 0x78;
		OBJECT(current_object)->flags |= 0x20000000u;
	}
	OBJECT(current_object)->sprite_flags = 0x104u;
	OBJECT(current_object)->sprite_frame = 0xCF0u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->flags &= 0x22000000u;
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->z = 0;
	PLAYER(current_player)->previous_z = 0x8000000;
	PLAYER(current_player)->trackball_x = 0;
	PLAYER(current_player)->trackball_y = 0;
	PLAYER(current_player)->substate = 0u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->hit_stop = 0u;
	PLAYER(current_player)->ignore_screen_edges = 0u;
	PLAYER(current_player)->walk_cycle = 0;
	PLAYER(current_player)->walk_pending_x = 0;
	PLAYER(current_player)->walk_pending_y = 0;
	OBJECT(current_object)->previous_floor_block = 0;
	OBJECT(current_object)->screen_x = 0;
	OBJECT(current_object)->screen_y = 0;
	PLAYER(current_player)->path = 0u;
	PLAYER(current_player)->mishap_flag = 0u;
	PLAYER(current_player)->scene_step_seen = 0u;
	PLAYER(current_player)->speed_scale = 0u;
	PLAYER(current_player)->goal_step = 0u;
	PLAYER(current_player)->entry_step = 0u;
	PLAYER(current_player)->effect_spawned = 0u;
	PLAYER(current_player)->flags &= 0x10u;
	PLAYER(current_player)->unused_reset_word = 0u;
	PLAYER(current_player)->ridden_object = 0u;
	PLAYER(current_player)->splash_zone = 0u;
	PLAYER(current_player)->was_in_splash_zone = 0u;
	PLAYER(current_player)->stage_b_path_done = 0u;
	PLAYER(current_player)->hurt_contact_delay = 0;
	PLAYER(current_player)->stall_frames = 0;
	PLAYER(current_player)->state = 0u;
	PLAYER(current_player)->enter_on_respawn = 0u;
	if (game_in_progress != 0u && (OBJECT(current_object)->flags & 0x2000000u) != 0u)
		PLAYER(current_player)->enter_on_respawn = 0xFFu;
	PLAYER(current_player)->facing = 8u;
	PLAYER(current_player)->target_heading = 0x200u;
	PLAYER(current_player)->heading = 0x200u;
	PLAYER(current_player)->facing_angle = 0x200u;
	OBJECT(current_object)->hitbox_size_x = 8u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->hitbox_height = 0x18u;
	PLAYER(current_player)->grey_screen_request = 0u;
	PLAYER(current_player)->unused_reset_byte = 0x78u;
	PLAYER(current_player)->sink_depth = 0x100;
	PLAYER(current_player)->free_turn_timer = 2u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0xA;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sonic_walking_animations_ADDRESS;
	stage_index = stage;
	OBJECT(current_object)->player_view.palette_list = player_palette_lists[(uint32_t)((uint16_t)(stage_index * 0xCu) / 4u) + OBJECT(current_object)->kind];
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	if ((OBJECT(current_object)->flags & 0x2000000u) != 0u || current_game_mode == 0x1Bu)
	{
		player_place();
		return;
	}
	if ((PLAYER(current_player)->flags & 0x10u) != 0u)
	{
		sound_id = 0x50u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x6Fu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x62u;
		}
	}
	else
	{
		sound_id = 0x5Du;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x68u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x52u;
		}
	}
	play_sound(sound_id, &ignored, &ignored_2);
	player_place();
}

/* Places the player at its restart point on its floor (start_climb_at_spawn on a wall) and sets
   entry_status and player_status: 0x81 over a pit or while touched or in a zone, 0x82 waiting for the
   scene (set_player_entry_by_scene), else 0xFF, staying the handler while it waits; then
   add_player_to_lists and player_hud_start. */
void player_place(void)
{
	uint8_t entry_status = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, player_offset = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	player_offset = place_player_at_restart_point();
	find_floor(&player_offset, &ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	PLAYER(current_player)->substate = 0u;
	PLAYER(current_player)->state = 0u;
	start_climb_at_spawn(player_offset);
	if (OBJECT(current_object)->floor_z_int >= 0x7FF)
	{
		entry_status = 0x81u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	else if ((uint16_t)PLAYER(current_player)->previous_z_int != 0x800u)
	{
		if ((OBJECT(current_object)->flags & 4u) != 0u || (OBJECT(current_object)->flags & 0x8000000u) != 0u)
		{
			entry_status = 0x81u;
			OBJECT(current_object)->handler = player_place_ADDRESS;
		}
		else
		{
			OBJECT(current_object)->handler = player_update_ADDRESS;
			entry_status = 0xFFu;
		}
	}
	else
	{
		entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	PLAYER(current_player)->entry_status = entry_status;
	set_player_entry_by_scene();
	player_status[OBJECT(current_object)->player] = PLAYER(current_player)->entry_status;
	player_status_spare = 0u;
	OBJECT(current_object)->flags &= 0xEFFFFFFFu;
	if (PLAYER(current_player)->entry_status != 0x82u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		project_object(&ignored_5, &ignored);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
		remember_platform();
		add_player_to_lists();
		player_hud_start();
		if (stage_exit_started == 0u && event_0d_active == 0u && (PLAYER(current_player)->flags & 0x10u) != 0u)
			PLAYER(current_player)->mishap_flag = 0xFFu;
	}
	else
	{
		OBJECT(current_object)->flags |= 0x10000000u;
		check_player_stalled();
		if (PLAYER(current_player)->state != 0xDu)
		{
			PLAYER(current_player)->previous_z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
			project_object(&ignored_5, &ignored);
			PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
			PLAYER(current_player)->screen_x_fraction = 0u;
			remember_platform();
			add_player_to_lists();
			player_hud_start();
			if (stage_exit_started == 0u && event_0d_active == 0u && (PLAYER(current_player)->flags & 0x10u) != 0u)
				PLAYER(current_player)->mishap_flag = 0xFFu;
			return;
		}
		OBJECT(current_object)->handler = player_update_ADDRESS;
		PLAYER(current_player)->entry_status = 0xFFu;
		player_status[OBJECT(current_object)->player] = PLAYER(current_player)->entry_status;
	}
}

/* The player object's handler: the seat output and the HUD, invulnerable_timer counting down to -1, the
   entry path, goal and scene checks that force states, then player_dispatch_state runs the code of its
   state. */
void player_update(void)
{
	uint32_t digit_attribute = 0, text_position = 0;
	uint16_t invulnerable_left = 0;

	current_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	OBJECT(current_object)->flags &= ~0x2000000u;
	skip_player_summary = 0u;
	update_hurt_output();
	player_hud_update(&digit_attribute, &text_position);
	invulnerable_left = PLAYER(current_player)->invulnerable_timer - 1u;
	PLAYER(current_player)->invulnerable_timer = invulnerable_left;
	if ((int16_t)invulnerable_left < 0)
		PLAYER(current_player)->invulnerable_timer = -1;
	check_stage_entry_path();
	check_player_reached_goal();
	text_position = check_scene_states(text_position);
	player_dispatch_state(digit_attribute, text_position);
}

/* Runs the code of the player's state: player_move for the moving states (0, 1, 2, 4, 12, 19, 20, 23 and
   58), one player_state_ function for each other state. State code that changes the state calls it
   again, so the new state runs in the same frame. */
void player_dispatch_state(uint32_t depth, uint32_t cursor)
{
	uint32_t jump_offset = 0;

	jump_offset = PLAYER(current_player)->state * 3u;
	switch (PLAYER(current_player)->state)
	{
	case 0:
	case 1:
	case 2:
	case 4:
	case 12:
	case 19:
	case 20:
	case 23:
	case 58:
		player_move(depth, cursor);
		return;
	case 3:
		player_state_inactive();
		return;
	case 5:
		player_state_teetering(cursor);
		return;
	case 6:
		player_state_falling_off_edge();
		return;
	case 7:
		player_state_over_edge(jump_offset, cursor);
		return;
	case 8:
		player_state_dropping(cursor);
		return;
	case 9:
		player_state_falling_into_pit();
		return;
	case 10:
		player_state_climbing(depth, cursor);
		return;
	case 11:
	case 55:
		player_state_not_playing();
		return;
	case 13:
		player_state_hurt(depth, cursor);
		return;
	case 14:
		player_state_bouncing();
		return;
	case 15:
	case 16:
	case 17:
	case 18:
		player_state_launched();
		return;
	case 21:
		player_state_held(depth, cursor);
		return;
	case 22:
		player_state_dying();
		return;
	case 24:
		player_state_stage_scene(depth, cursor);
		return;
	case 25:
		player_state_scene_thrown(depth, cursor);
		return;
	case 26:
		player_state_inactive_2();
		return;
	case 27:
		player_state_short_animation();
		return;
	case 28:
		player_state_opening_scene();
		return;
	case 29:
		player_state_stuck(cursor);
		return;
	case 30:
		player_state_intro_turn(depth, cursor);
		return;
	case 31:
		player_state_blown_away(cursor);
		return;
	case 32:
		player_state_tumbling(depth, cursor);
		return;
	case 33:
		player_state_stage_exit(depth, cursor);
		return;
	case 34:
		player_state_in_zone(depth, cursor);
		return;
	case 35:
		player_state_continue();
		return;
	case 36:
		player_state_out_of_game();
		return;
	case 37:
		player_state_floor_8_sequence(cursor);
		return;
	case 38:
		player_state_pulled(cursor);
		return;
	case 39:
		player_state_climbing_object(cursor);
		return;
	case 40:
		player_state_swept_away();
		return;
	case 41:
		player_state_tripped(cursor);
		return;
	case 42:
		player_state_entering_stage(depth, cursor);
		return;
	case 43:
		player_state_final_stage();
		return;
	case 44:
		player_state_stuck_on_object(depth, cursor);
		return;
	case 45:
		player_state_held_still();
		return;
	case 46:
		player_state_game_over(jump_offset);
		return;
	case 47:
		player_state_stunned(depth, cursor);
		return;
	case 48:
		player_state_caught(cursor);
		return;
	case 49:
		player_state_carried_off(depth, cursor);
		return;
	case 50:
		player_state_trapped(depth, cursor);
		return;
	case 51:
		player_state_sinking(cursor);
		return;
	case 52:
		player_state_choosing_character(cursor);
		return;
	case 53:
		player_state_riding();
		return;
	case 54:
		player_state_knocked_off(depth, cursor);
		return;
	case 56:
		player_state_stage_b_sequence(depth, cursor);
		return;
	case 57:
		player_state_you_failed();
		return;
	default:
		invalid_jump(0x2D3Eu + PLAYER(current_player)->state * 3u);
	}
}

/* Off screen (but in stage 8 before scroll 0x4000) the player loses 8 health and respawns, or dies
   (state 0x16) when that empties it; stage 5 only respawns it. Near an edge it sets screen_edge, says
   its edge voice once with 2 or more players in the stage's range, is pushed back and, on edges the
   scroll pushes, moves with the camera. */
uint32_t keep_player_on_screen(uint32_t depth)
{
	uint8_t sound_id = 0;
	uint16_t base_x = 0, base_y = 0, edge = 0, scroll_direction = 0;
	uint32_t direction_row = 0, floor_slope = 0, ignored = 0, ignored_2 = 0, offset_x = 0, slope_direction = 0, slope_factor = 0, slope_speed = 0, voice_range = 0;
	uint8_t health = 0;

	if (nearby_floor_blocks_locked != 0u || (OBJECT(current_object)->flags & 0x4000000u) != 0u)
		return depth;
	offset_x = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	base_x = (uint16_t)(OBJECT(current_object)->screen_x - offset_x);
	depth = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	base_y = (uint16_t)(OBJECT(current_object)->screen_y - depth);
	if ((stage != 8u || scroll_position >= 0x4000u) && ((uint16_t)(base_x + 0x10u) >= 0x160u || (uint16_t)(base_y + 0xE0u) >= 0x1E0u))
	{
		if (stage != 5u)
		{
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
			return depth;
		}
		OBJECT(current_object)->handler = player_respawn_ADDRESS;
		return depth;
	}
	if ((OBJECT(current_object)->flags & 0x1000u) != 0u || PLAYER(current_player)->ignore_screen_edges != 0u || OBJECT(current_object)->floor_z_int >= 0x7FF)
		return depth;
	if ((int16_t)base_x > 0x10)
	{
		if ((int16_t)base_x < 0x128)
		{
			if (PLAYER(current_player)->state == 4u)
				base_y -= 9u;
			base_y += OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int;
			if ((int16_t)base_y > 0x20)
			{
				if ((int16_t)base_y < 0xD0)
					return depth;
				edge = 2u;
			}
			else
				edge = 0u;
		}
		else
			edge = 1u;
	}
	else
		edge = 3u;
	PLAYER(current_player)->screen_edge = edge;
	if ((PLAYER(current_player)->flags & 0x20u) == 0u)
	{
		voice_range = edge_voice_scroll_ranges_ADDRESS + stage * 4u;
		if ((uint16_t)(scroll_position - EDGE_VOICE_SCROLL_RANGE(voice_range)->start) < EDGE_VOICE_SCROLL_RANGE(voice_range)->length && players_in_play >= 2)
		{
			sound_id = 0x39u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x43u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x4Cu;
			}
			play_sound(sound_id, &ignored, &ignored_2);
			PLAYER(current_player)->flags |= 0x20u;
		}
	}
	slope_factor = 0u;
	if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
	{
		floor_slope = OBJECT(current_object)->floor_slope;
		slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
	}
	compute_camera_slope_speed(0u, slope_factor);
	if (camera_slope_speed == 0u)
		return push_player_from_screen_edge();
	scroll_direction = camera_scroll_direction;
	direction_row = 0u;
	if ((uint8_t)scroll_direction != 0u)
		direction_row = (uint16_t)(direction_row + 4u);
	direction_row = (uint16_t)(direction_row + (scroll_direction >> 8));
	if (screen_edge_scroll_carry_flags[(uint32_t)(uint16_t)(direction_row << 2) + PLAYER(current_player)->screen_edge] >= 0)
		return push_player_from_screen_edge();
	push_player_from_screen_edge();
	slope_direction = (int16_t)camera_slope_direction;
	slope_speed = camera_slope_speed >> 14;
	OBJECT(current_object)->x += (int32_t)(sine_table[slope_direction] * slope_speed) >> 8;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	depth = (int32_t)(cosine_table[slope_direction] * slope_speed) >> 8;
	OBJECT(current_object)->y -= depth;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	return depth;
}

/* Pushes the player 0x200 along the angle screen_edge_push_directions gives for screen_edge; when its
   direction is not within a quarter turn after that entry's direction, it first goes back to its
   previous position, floor block and floor_z. */
uint32_t push_player_from_screen_edge(void)
{
	uint16_t relative_direction = 0;
	uint32_t push_cos = 0, push_record = 0, push_x = 0, push_y = 0;

	push_record = screen_edge_push_directions_ADDRESS + PLAYER(current_player)->screen_edge * 4u;
	relative_direction = (OBJECT(current_object)->direction - SCREEN_EDGE_PUSH(push_record)->limit_direction) & 0x3FFu;
	push_record += 2u;
	if (relative_direction >= 0x101u)
	{
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	push_x = SCREEN_EDGE_PUSH(push_record - 0x2u)->push_direction;
	push_y = 0x200u;
	polar_to_xy(&push_cos, &push_x, &push_y);
	OBJECT(current_object)->x += push_x;
	OBJECT(current_object)->y += push_y;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	return push_cos;
}

/* Turns the camera's movement (camera_velocity_x/y) into scroll_drift_speed and scroll_drift_direction
   for floor block block: on a flat block its length and angle (y counting double), on a slope y slowed
   by the slope's factor and the block's own direction; also camera_scroll_direction. Skipped when block
   and velocity are unchanged. */
void compute_scroll_drift(uint32_t block, uint8_t drift_byte)
{
	uint16_t dy = 0;
	uint32_t divisor = 0, drift_angle = 0, ignored = 0, ignored_2 = 0, scroll_angle = 0, slope = 0, vx = 0, vy = 0;

	if (scroll_drift_block == block && scroll_drift_vx == (uint32_t)camera_velocity_x && scroll_drift_vy == (uint32_t)camera_velocity_y)
		return;
	unused_drift_byte = drift_byte;
	scroll_drift_block = block;
	if (FLOOR_BLOCK(block)->slope == 0u)
	{
		vx = (int32_t)camera_velocity_x >> 5;
		vy = (int32_t)camera_velocity_y >> 4;
		vx = vx * vx + vy * vy;
		square_root(&vx, &ignored);
		scroll_drift_speed = (vx & 0xFFFFu) << 13;
		drift_angle = (int32_t)camera_velocity_x >> 9;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 8);
		angle_of_vector(dy, &drift_angle, &ignored_2);
		scroll_drift_direction = (drift_angle + 0x80u) & 0x3FFu;
		scroll_angle = (int32_t)camera_velocity_x >> 5;
		dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
		angle_of_vector(dy, &scroll_angle, &ignored_2);
		camera_scroll_direction = scroll_angle;
		scroll_drift_vx = camera_velocity_x;
		scroll_drift_vy = camera_velocity_y;
		return;
	}
	slope = FLOOR_BLOCK(block)->slope;
	divisor = floor_slope_factors[(uint16_t)(slope * 2u)];
	if ((int32_t)divisor < 0)
		divisor = 0u - divisor;
	vy = camera_velocity_y;
	divisor += 0x10000u;
	unused_drift_divisor = divisor;
	vy = divide_s32(vy << 11, divisor);
	vx = (int32_t)camera_velocity_x >> 5;
	vx = vx * vx + vy * vy;
	square_root(&vx, &ignored);
	scroll_drift_speed = (vx & 0xFFFFu) << 13;
	scroll_drift_direction = (uint32_t)FLOOR_BLOCK(scroll_drift_block)->travel_direction << 8;
	scroll_angle = (int32_t)camera_velocity_x >> 5;
	dy = (uint16_t)((int32_t)camera_velocity_y >> 5);
	angle_of_vector(dy, &scroll_angle, &ignored_2);
	camera_scroll_direction = scroll_angle;
	scroll_drift_vx = camera_velocity_x;
	scroll_drift_vy = camera_velocity_y;
}

/* The slip effect player_state_dropping spawns: animates and is deleted when its animation is back at
   frame 0; drawn on top (bucket 0) when its direction (the ledge_side) is 2 or more, else in bucket
   0xFF. */
void slip_effect_update(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	animate(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->animation_frame == 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored_4, &ignored);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* Sets the player's speed from its trackball: the length of (trackball_x, trackball_y) squared, at most
   0x5000000, along its angle (direction = angle + 0x80), then speed_x and speed_y
   (set_speed_from_direction). */
uint32_t set_speed_from_trackball(void)
{
	uint16_t trackball_y_2 = 0;
	uint32_t ignored = 0, ignored_2 = 0, speed = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed = (uint16_t)speed;
	speed = speed * speed;
	if ((int32_t)speed > 0x5000000)
		speed = 0x5000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored_2);
	OBJECT(current_object)->direction = (trackball_angle + 0x80u) & 0x3FFu;
	return set_speed_from_direction();
}

/* As set_speed_from_trackball for climbing: speed at most 0x3000000 and direction the trackball's angle
   itself. */
uint32_t set_climb_speed_from_trackball(void)
{
	uint16_t trackball_y_2 = 0;
	uint32_t ignored = 0, ignored_2 = 0, speed = 0, trackball_angle = 0, trackball_y = 0;

	read_player_trackball();
	speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	speed = speed * speed + trackball_y * trackball_y;
	square_root(&speed, &ignored);
	speed = (uint16_t)speed;
	speed = speed * speed;
	if ((int32_t)speed > 0x3000000)
		speed = 0x3000000u;
	OBJECT(current_object)->speed = speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	trackball_y_2 = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(trackball_y_2, &trackball_angle, &ignored_2);
	OBJECT(current_object)->direction = trackball_angle & 0x3FFu;
	return set_speed_from_direction();
}

/* Sets the speed from the trackball (set_speed_from_trackball) and adds to ledge_pullback the part of it
   that points back from the ledge on ledge_side (side 4: none). */
uint32_t update_ledge_pullback(void)
{
	uint32_t cos_speed = 0;

	cos_speed = set_speed_from_trackball();
	switch ((uint16_t)PLAYER(current_player)->ledge_side)
	{
	case 0:
	case 5:
		if (OBJECT(current_object)->speed_y_int >= 0)
			PLAYER(current_player)->ledge_pullback += OBJECT(current_object)->speed_y_int;
		break;
	case 1:
		if (OBJECT(current_object)->speed_x_int < 0)
			PLAYER(current_player)->ledge_pullback -= OBJECT(current_object)->speed_x_int;
		break;
	case 2:
		if (OBJECT(current_object)->speed_y_int < 0)
			PLAYER(current_player)->ledge_pullback -= OBJECT(current_object)->speed_y_int;
		break;
	case 3:
		if (OBJECT(current_object)->speed_x_int >= 0)
			PLAYER(current_player)->ledge_pullback += OBJECT(current_object)->speed_x_int;
		break;
	case 4:
		break;
	default:
		invalid_jump(0xB36Fu + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
	}
	return cos_speed;
}

/* Hands back in side which side of its previous floor block the object has gone past (3 before x_min, 1
   past x_max, 0 before y_min, 2 past y_max, 4 still inside) and in block that block's record. */
void get_ledge_side(uint32_t *p_side, uint32_t *p_block)
{
	uint32_t bound_cursor = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	bound_cursor = floor_blocks + OBJECT(current_object)->previous_floor_block + 2u;
	*p_block = bound_cursor;
	*p_side = 3u;
	x_min = FLOOR_BLOCK(bound_cursor)->x_min;
	bound_cursor += 4u;
	if (OBJECT(current_object)->x < (int32_t)x_min)
	{
		return;
	}
	*p_side = 1u;
	x_max = FLOOR_BLOCK(bound_cursor - 0x4u)->x_max;
	bound_cursor += 4u;
	if ((int32_t)x_max < OBJECT(current_object)->x)
	{
		return;
	}
	*p_side = 0u;
	y_min = FLOOR_BLOCK(bound_cursor - 0x8u)->y_min;
	bound_cursor += 4u;
	if (OBJECT(current_object)->y < (int32_t)y_min)
	{
		return;
	}
	*p_side = 2u;
	if (FLOOR_BLOCK(bound_cursor - 0xCu)->y_max < OBJECT(current_object)->y)
	{
		return;
	}
	*p_side = 4u;
}

/* Jumping with no floor block under it, the player goes over the edge (state 7, substate 1, ledge_side
   and fall_draw_depth set); on the ground over a pit (floor_z past 0x7FF) it teeters (5), or goes over
   (7) faster than 0x200 off most block types or with block flags bit 1. Hopping or jumping over a floor:
   check_fall_into_pit. */
void check_player_floor_edge(uint32_t *p_depth, uint32_t *p_block)
{
	uint8_t next_state = 0;
	uint32_t block_2 = 0, edge_speed_x = 0, edge_speed_y = 0, side = 0;

	if (PLAYER(current_player)->state == 0x17u)
	{
		check_fall_into_pit();
		return;
	}
	if (PLAYER(current_player)->state == 4u)
	{
		if (OBJECT(current_object)->floor_block >= 0)
		{
			check_fall_into_pit();
			return;
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 1u;
		get_ledge_side(&side, p_block);
		if ((uint16_t)side == 4u)
			side = (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x15u) + 2u) & 3u;
		PLAYER(current_player)->ledge_side = side;
		*p_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
		*p_depth = (*p_depth & ~0xFFFFu) | (uint16_t)(*p_depth - view_y);
		PLAYER(current_player)->fall_draw_depth = *p_depth;
		if (OBJECT(current_object)->floor_type == 1u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
			{
				*p_depth = (*p_depth & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth;
				*p_depth = (*p_depth & ~0xFFFFu) | (uint16_t)(*p_depth - 1u);
				PLAYER(current_player)->fall_draw_depth = *p_depth;
			}
		}
		else
		{
			if (PLAYER(current_player)->ledge_side >= 2)
			{
				OBJECT(current_object)->flags &= ~0x100u;
				if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
				{
					OBJECT(current_object)->flags |= 0x100u;
					PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
				}
				PLAYER(current_player)->heading = OBJECT(current_object)->direction;
				PLAYER(current_player)->ledge_pullback = 0;
				PLAYER(current_player)->slip_distance = 0;
				PLAYER(current_player)->control_lock = 0u;
				OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->check_player.palette_list)->normal_palette;
				OBJECT(current_object)->flags &= ~0x40u;
				OBJECT(current_object)->flags &= ~0x20000u;
				PLAYER(current_player)->flags |= 0x80u;
				OBJECT(current_object)->flags &= ~0x2000u;
				return;
			}
			if (stage == 0xAu)
				PLAYER(current_player)->fall_draw_depth -= 0x20u;
		}
		PLAYER(current_player)->fall_draw_depth -= 0x10u;
		OBJECT(current_object)->flags &= ~0x100u;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x10u || OBJECT(current_object)->floor_type == 1u)
		{
			OBJECT(current_object)->flags |= 0x100u;
			PLAYER(current_player)->edge_object = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->parent;
		}
		PLAYER(current_player)->heading = OBJECT(current_object)->direction;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->check_player.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x20000u;
		PLAYER(current_player)->flags |= 0x80u;
		OBJECT(current_object)->flags &= ~0x2000u;
		return;
	}
	if (PLAYER(current_player)->state != 2u && OBJECT(current_object)->floor_z_int > 0x7FF)
	{
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 7u;
			PLAYER(current_player)->substate = 0u;
			if (OBJECT(current_object)->speed_int >= 0x200)
			{
				return;
			}
			edge_speed_x = OBJECT(current_object)->direction;
			edge_speed_y = 0x100u;
			polar_to_xy(p_depth, &edge_speed_x, &edge_speed_y);
			OBJECT(current_object)->speed_x = edge_speed_x;
			OBJECT(current_object)->speed_y = edge_speed_y;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		next_state = 5u;
		if (OBJECT(current_object)->floor_type != 1u && (OBJECT(current_object)->flags & 0x10u) == 0u)
		{
			block_2 = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
			if (FLOOR_BLOCK(block_2)->type_index != 0x12u && FLOOR_BLOCK(block_2)->type_index != 0x23u && FLOOR_BLOCK(block_2)->type_index != 0x18u && FLOOR_BLOCK(block_2)->type_index != 0x19u && OBJECT(current_object)->speed_int > 0x200)
				next_state = 7u;
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = next_state;
	}
}

/* Over a pit (floor_z 0x7FF or more, not floor type 6), a player falling or still (speed_z not negative)
   at or below its previous floor's height goes to state 9, falling into the pit. */
void check_fall_into_pit(void)
{
	if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int >= 0x7FF && OBJECT(current_object)->floor_type != 6u && OBJECT(current_object)->previous_floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->state = 9u;
	}
}

/* Puts the player object in hitter_list and rider_list for this frame's contact checks. */
void add_player_to_lists(void)
{
	add_to_hitter_list();
	add_to_rider_list();
}

/* Copies flags bit 3 (on a platform this frame) into bit 4, so the next frame knows the player was on
   one. */
void remember_platform(void)
{
	OBJECT(current_object)->flags_low &= ~0x10u;
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->flags_low |= 0x10u;
}

/* While invulnerable_timer runs and flags bit 29 is set, toggles hidden each frame (the blink); once it
   is out, clears both, so the player shows. */
void update_player_blink(void)
{
	if (PLAYER(current_player)->invulnerable_timer < 0)
	{
		OBJECT(current_object)->flags &= ~0x80000u;
		OBJECT(current_object)->flags &= 0xDFFFFFFFu;
		return;
	}
	if ((OBJECT(current_object)->flags & 0x20000000u) == 0u)
		return;
	toggle_hidden();
}

/* Flips flags bit 19, which keeps the player from being drawn: the blink. */
void toggle_hidden(void)
{
	OBJECT(current_object)->flags ^= 0x80000u;
}

/* Unless players_frozen: on floor type 6 the player hops (state 0x17) unless jump_delay is 2; elsewhere,
   once jump_delay has run out, not falling and with no control_lock, start_jump_if_pressed. */
void check_player_jump(void)
{
	uint8_t jump_delay = 0;

	if (players_frozen != 0u)
		return;
	if (OBJECT(current_object)->floor_type == 6u)
	{
		if ((uint8_t)PLAYER(current_player)->jump_delay != 2u)
		{
			PLAYER(current_player)->state = 0x17u;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->ledge_side = -1;
		}
		return;
	}
	jump_delay = PLAYER(current_player)->jump_delay;
	PLAYER(current_player)->jump_delay = jump_delay - 1u;
	if ((int8_t)jump_delay < 1)
	{
		PLAYER(current_player)->jump_delay = 0;
		if (PLAYER(current_player)->state != 2u && PLAYER(current_player)->control_lock == 0u)
		{
			start_jump_if_pressed();
			return;
		}
	}
}

/* Starts a jump (state 4) when the player's button is newly pressed (inputs_pressed bit 0x10, 0x18 or
   0x12 by player in a game, 0x19 to 0x1B outside one), unless pushed_out or on a path. */
void start_jump_if_pressed(void)
{
	uint32_t button_bit = 0;

	if ((uint8_t)PLAYER(current_player)->pushed_out != 0u || PLAYER(current_player)->path != 0u)
		return;
	if (game_in_progress != 0u)
	{
		button_bit = 0x10u;
		if (OBJECT(current_object)->player != 0u)
		{
			button_bit = 0x18u;
			if (OBJECT(current_object)->player != 1u)
				button_bit = 0x12u;
		}
	}
	else
	{
		button_bit = 0x19u;
		if (OBJECT(current_object)->player != 0u)
		{
			button_bit = 0x1Au;
			if (OBJECT(current_object)->player != 1u)
				button_bit = 0x1Bu;
		}
	}
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) != 0u)
	{
		PLAYER(current_player)->state = 4u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->ledge_side = -1;
	}
}

/* In a wall's box (flags bit 5, wall) the player goes back to its previous place and floor, for 0x1E
   frames per wall. Jumping or hopping it bounces off (speed 0x4000000, a hop, control_lock 0xA); at
   speed 0x200 or more or on floor type 4 it turns along the wall (push_player_out_of_wall_deflect); else
   it stops. Not on a path. */
void push_player_out_of_wall(uint32_t *p_angle, uint32_t *p_push_direction, uint32_t *p_record_index)
{
	uint32_t angle = *p_angle;
	uint16_t push_direction = (uint16_t)*p_push_direction;
	uint16_t drift_bounce_direction = 0;
	uint32_t drift_angle = 0, rebound_direction = 0, wall_push_direction = 0;
	uint8_t wall_timer = 0;

	if (PLAYER(current_player)->path_request != 0u || PLAYER(current_player)->path != 0u || all_players_path_request != 0u)
		return;
	if ((OBJECT(current_object)->flags & 0x20u) == 0u)
	{
		PLAYER(current_player)->last_wall = 0xFFFFu;
		return;
	}
	if (OBJECT(current_object)->wall != PLAYER(current_player)->last_wall)
	{
		PLAYER(current_player)->wall_timer = 0x1E;
		PLAYER(current_player)->last_wall = OBJECT(current_object)->wall;
		if ((stage != 8u || scroll_position >= 0x4000u) && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
			OBJECT(current_object)->flags |= 0x1000u;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		PLAYER(current_player)->control_lock = 0u;
	}
	else
	{
		wall_timer = PLAYER(current_player)->wall_timer;
		PLAYER(current_player)->wall_timer = wall_timer - 1u;
		if ((int8_t)wall_timer > 1)
		{
			if ((stage != 8u || scroll_position >= 0x4000u) && PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
				OBJECT(current_object)->flags |= 0x1000u;
			OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
			OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
			OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
			PLAYER(current_player)->control_lock = 0u;
		}
		else
			PLAYER(current_player)->wall_timer = 0;
	}
	if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u)
	{
		rebound_direction = (angle & ~0xFFFFu) | OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction;
		rebound_direction = (rebound_direction & ~0xFFFFu) | (uint16_t)(rebound_direction << 8);
		rebound_direction = (rebound_direction & ~0xFFFFu) | (uint16_t)(rebound_direction + 0x200u);
		rebound_direction = (rebound_direction & ~0xFFFFu) | (rebound_direction & 0x3FFu);
		OBJECT(current_object)->direction = rebound_direction;
		rebound_direction = face_movement_direction(rebound_direction);
		OBJECT(current_object)->speed = 0x4000000;
		OBJECT(current_object)->speed_z = -0x18000;
		PLAYER(current_player)->state_timer = 6;
		OBJECT(current_object)->flags |= 0x2000u;
		PLAYER(current_player)->control_lock = 0xAu;
		*p_angle = rebound_direction;
		return;
	}
	if (current_game_mode == 0xAu || PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x20u)
	{
		angle = 0u;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->free_turn_timer = 3u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0u;
		*p_angle = angle;
		return;
	}
	if (PLAYER(current_player)->state != 0x25u && PLAYER(current_player)->state != 0x26u)
	{
		if (OBJECT(current_object)->floor_type == 4u || OBJECT(current_object)->speed_int >= 0x200)
		{
			push_player_out_of_wall_deflect(&angle, &wall_push_direction, p_record_index);
			*p_angle = angle;
			*p_push_direction = (*p_push_direction & 0xFFFF0000u) | (wall_push_direction & 0xFFFFu);
			return;
		}
		angle = 0u;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->free_turn_timer = 3u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0u;
		*p_angle = angle;
		return;
	}
	drift_angle = (angle & ~0xFFFFu) | scroll_drift_direction;
	drift_angle = (drift_angle & ~0xFFFFu) | (uint16_t)(drift_angle + 0x200u);
	drift_angle = (drift_angle & ~0xFFFFu) | (drift_angle & 0x3FFu);
	drift_bounce_direction = sonic_bounce_records[OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction_low * 3u];
	drift_angle = (drift_angle & ~0xFFFFu) | (uint16_t)(drift_angle - drift_bounce_direction);
	drift_angle = (drift_angle & ~0xFFFFu) | (drift_angle & 0x3FFu);
	if ((int16_t)drift_angle < 0x200)
	{
		drift_bounce_direction -= 0x80u;
		scroll_drift_direction = drift_bounce_direction & 0x3FFu;
		push_player_out_of_wall_deflect(&drift_angle, &wall_push_direction, p_record_index);
		*p_angle = drift_angle;
		*p_push_direction = (*p_push_direction & 0xFFFF0000u) | (wall_push_direction & 0xFFFFu);
		return;
	}
	drift_bounce_direction += 0x80u;
	scroll_drift_direction = drift_bounce_direction & 0x3FFu;
	push_player_out_of_wall_deflect(&drift_angle, &wall_push_direction, p_record_index);
	*p_angle = drift_angle;
	*p_push_direction = (*p_push_direction & 0xFFFF0000u) | (wall_push_direction & 0xFFFFu);
}

/* Turns the player along the wall: direction = the wall's sonic_bounce_records angle - or + 0x80, by the
   side the player came from, with control_lock 4; in stage 8 before scroll 0x4000 it is sent straight
   back instead (the wall's direction + 0x200). */
void push_player_out_of_wall_deflect(uint32_t *p_angle, uint32_t *p_push_direction, uint32_t *p_record_index)
{
	uint16_t angle = 0, new_direction = 0, push_direction = 0;

	*p_record_index = OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction_low * 3u;
	push_direction = sonic_bounce_records[*p_record_index];
	new_direction = (uint16_t)push_direction;
	angle = (((OBJECT(current_object)->direction + 0x200u) & 0x3FFu) - push_direction) & 0x3FFu;
	if ((int16_t)angle < 0x200)
		new_direction -= 0x80u;
	else
		new_direction += 0x80u;
	OBJECT(current_object)->direction = new_direction & 0x3FFu;
	PLAYER(current_player)->control_lock = 4u;
	if (stage == 8u && scroll_position < 0x4000u)
	{
		angle = (((uint32_t)OBJECT(LINKED(current_object, OBJECT(current_object)->wall))->direction << 8) + 0x200u) & 0x3FFu;
		OBJECT(current_object)->direction = angle;
	}
	*p_angle = (*p_angle & 0xFFFF0000u) | angle;
	*p_push_direction = (*p_push_direction & 0xFFFF0000u) | push_direction;
}

/* Steers the player in the air: speed goes to the trackball's (its length squared, at most 0x5000000) at
   once when higher, by 1/32 of the gap when lower; the trackball turns target_heading unless flags bit
   13 is set; then turn_player. */
void player_steer_in_air(void)
{
	uint32_t ignored = 0, input_speed = 0, speed_diff = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0;

	read_player_trackball();
	input_speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	input_speed = input_speed * input_speed + trackball_y * trackball_y;
	square_root(&input_speed, &ignored);
	input_speed &= 0xFFFFu;
	input_speed = input_speed * input_speed;
	if ((int32_t)input_speed > 0x5000000)
		input_speed = 0x5000000u;
	speed = OBJECT(current_object)->speed;
	target_speed = input_speed;
	speed_diff = target_speed - speed;
	if ((int32_t)target_speed <= (int32_t)speed)
		OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
	else
		OBJECT(current_object)->speed += speed_diff;
	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u && (OBJECT(current_object)->flags & 0x2000u) == 0u)
		set_target_heading_from_trackball();
	turn_player();
}

/* Steers the player on the ground: a path gives the speed (player_steer_approach_speed); else the
   trackball (zeroed while players_frozen) or a push turns target_heading (not with flags bit 13) and
   sets the speed (player_steer_speed_from_trackball); with no input it slows down and, under speed 0x20,
   stops and stands (state 0). */
void player_steer(uint32_t wanted_speed)
{
	wanted_speed = follow_player_path(wanted_speed);
	if (PLAYER(current_player)->path != 0u)
	{
		player_steer_approach_speed(wanted_speed);
		return;
	}
	read_player_trackball();
	if (players_frozen != 0u)
	{
		PLAYER(current_player)->trackball_x = 0;
		PLAYER(current_player)->trackball_y = 0;
	}
	if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || PLAYER(current_player)->state == 0x2Du || (PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || PLAYER(current_player)->push_xy != 0u)
	{
		if ((OBJECT(current_object)->flags & 0x2000u) != 0u)
		{
			player_steer_speed_from_trackball();
			return;
		}
		set_target_heading_from_trackball();
		player_steer_speed_from_trackball();
		return;
	}
	if (OBJECT(current_object)->speed_int >= 0x20)
	{
		player_steer_speed_from_trackball();
		return;
	}
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
	PLAYER(current_player)->state = 0u;
	player_steer_turn_and_ground_speed();
}

/* The wanted speed is 1.5 x the length of trackball + push squared, at most 0x5000000; a quarter of it
   with flags bit 31 (0 with block flags bit 2), slowed by sink_depth on floor type 0xA (setting
   struggle_speed), 0x60/0x100 of it on floor type 4; then player_steer_approach_speed. */
void player_steer_speed_from_trackball(void)
{
	uint32_t ignored = 0, input_y = 0, wanted_speed = 0;

	wanted_speed = (uint32_t)PLAYER(current_player)->trackball_x + PLAYER(current_player)->push_x;
	input_y = (uint32_t)PLAYER(current_player)->trackball_y + PLAYER(current_player)->push_y;
	wanted_speed = wanted_speed * wanted_speed + input_y * input_y;
	square_root(&wanted_speed, &ignored);
	wanted_speed &= 0xFFFFu;
	wanted_speed = wanted_speed * wanted_speed;
	wanted_speed += wanted_speed >> 1;
	if ((int32_t)wanted_speed > 0x5000000)
		wanted_speed = 0x5000000u;
	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		wanted_speed = wanted_speed >> 2;
		if ((PLAYER(current_player)->flags & 4u) != 0u)
		{
			wanted_speed = 0u;
			OBJECT(current_object)->speed = 0;
		}
	}
	if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type == 0xAu)
	{
		PLAYER(current_player)->struggle_speed = wanted_speed >> 16;
		wanted_speed = (wanted_speed >> 10) * (uint16_t)(0x100u - ((uint16_t)PLAYER(current_player)->sink_depth >> 8) * 6u);
	}
	if (OBJECT(current_object)->floor_type != 4u)
	{
		player_steer_approach_speed(wanted_speed);
		return;
	}
	wanted_speed = (wanted_speed >> 8) * 0x60u;
	player_steer_approach_speed(wanted_speed);
}

/* Brings speed toward wanted_speed (apply_speed_scale): up at once and down 1/32 of the gap (up 1/32,
   down 1/64 on floor type 7), sets animation_speed, and picks standing (0), walking (1), running (0x13)
   or fast running (0x14) by it, except in states 4, 0x17, 0x25, 0x26 and 0x2D; then
   player_steer_turn_and_ground_speed. */
void player_steer_approach_speed(uint32_t wanted_speed)
{
	uint8_t next_state = 0;
	uint16_t animation_speed_int = 0;
	uint32_t animation_speed_diff = 0, ignored = 0, input_animation_speed = 0, run_threshold = 0, speed_diff = 0, trackball_y = 0;
	uint32_t speed = 0, target_speed = 0, speed_2 = 0, target_speed_2 = 0, animation_speed = 0, target_animation_speed = 0;

	wanted_speed = apply_speed_scale(wanted_speed);
	if (OBJECT(current_object)->floor_type == 7u && PLAYER(current_player)->path == 0u)
	{
		speed = OBJECT(current_object)->speed;
		target_speed = wanted_speed;
		speed_diff = target_speed - speed;
		if ((int32_t)target_speed <= (int32_t)speed)
			OBJECT(current_object)->speed += (int32_t)speed_diff >> 6;
		else
			OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
	}
	else
	{
		speed_2 = OBJECT(current_object)->speed;
		target_speed_2 = wanted_speed;
		speed_diff = target_speed_2 - speed_2;
		if ((int32_t)target_speed_2 <= (int32_t)speed_2)
			OBJECT(current_object)->speed += (int32_t)speed_diff >> 5;
		else
			OBJECT(current_object)->speed += speed_diff;
	}
	if (OBJECT(current_object)->floor_type == 7u || PLAYER(current_player)->push_xy != 0u || PLAYER(current_player)->speed_scale != 0u)
	{
		if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u || (OBJECT(current_object)->floor_type != 7u && PLAYER(current_player)->animation_speed_int >= 0x20))
		{
			input_animation_speed = PLAYER(current_player)->trackball_x;
			trackball_y = PLAYER(current_player)->trackball_y;
			input_animation_speed = input_animation_speed * input_animation_speed + trackball_y * trackball_y;
			square_root(&input_animation_speed, &ignored);
			input_animation_speed &= 0xFFFFu;
			input_animation_speed = input_animation_speed * input_animation_speed;
			input_animation_speed += input_animation_speed >> 1;
			if ((int32_t)input_animation_speed > 0x5000000)
				input_animation_speed = 0x5000000u;
			animation_speed = PLAYER(current_player)->animation_speed;
			target_animation_speed = input_animation_speed;
			animation_speed_diff = target_animation_speed - animation_speed;
			if ((int32_t)target_animation_speed <= (int32_t)animation_speed)
				PLAYER(current_player)->animation_speed += (int32_t)animation_speed_diff >> 5;
			else
				PLAYER(current_player)->animation_speed += animation_speed_diff;
		}
		else
			PLAYER(current_player)->animation_speed = 0;
	}
	else
		PLAYER(current_player)->animation_speed = OBJECT(current_object)->speed;
	if (PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || PLAYER(current_player)->state == 0x2Du)
	{
		player_steer_turn_and_ground_speed();
		return;
	}
	next_state = 0u;
	animation_speed_int = (uint16_t)PLAYER(current_player)->animation_speed_int;
	if (animation_speed_int == 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = next_state;
		player_steer_turn_and_ground_speed();
		return;
	}
	next_state = 1u;
	run_threshold = 0x200u;
	if (OBJECT(current_object)->floor_type == 4u)
		run_threshold = (run_threshold * 0x60u) >> 8;
	if ((int16_t)run_threshold < (int16_t)animation_speed_int)
	{
		next_state = 0x13u;
		if (OBJECT(current_object)->floor_type != 4u && (int16_t)animation_speed_int > 0x4F0)
			next_state = 0x14u;
	}
	PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
	PLAYER(current_player)->state = next_state;
	player_steer_turn_and_ground_speed();
}

/* Runs turn_player, then sets ground_speed: speed scaled by heading_speed_factors for the heading (speed
   itself with flags bit 31), and on a slope by the slope_speed_direction_pairs entry, which also sets
   direction (as compute_ground_speed). */
void player_steer_turn_and_ground_speed(void)
{
	uint32_t carry = 0;
	uint32_t direction = 0, floor_slope = 0, rounded_heading = 0, rounded_speed = 0, slope_pair = 0, speed_factor = 0;

	turn_player();
	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		rounded_heading = (uint16_t)(PLAYER(current_player)->heading + 0x20u) & 0x3FFu;
		PLAYER(current_player)->ground_speed = ((uint32_t)OBJECT(current_object)->speed >> 8) * heading_speed_factors[rounded_heading >> 6];
	}
	else
		PLAYER(current_player)->ground_speed = OBJECT(current_object)->speed;
	if (OBJECT(current_object)->floor_slope == 0u)
		return;
	floor_slope = OBJECT(current_object)->floor_slope;
	direction = OBJECT(current_object)->direction;
	slope_pair = slope_speed_direction_pairs_ADDRESS + floor_slope_direction_tables[(uint16_t)(floor_slope << 2)] + (direction >> 6) * 4u;
	speed_factor = SLOPE_SPEED_PAIR(slope_pair)->speed_factor;
	rounded_speed = PLAYER(current_player)->ground_speed;
	carry = (rounded_speed >> 11) & 1u;
	rounded_speed = rounded_speed >> 12;
	slope_pair += 2u;
	if (carry)
		rounded_speed++;
	PLAYER(current_player)->ground_speed = rounded_speed * speed_factor;
	OBJECT(current_object)->direction = SLOPE_SPEED_PAIR(slope_pair - 0x2u)->direction;
}

/* Sets facing_angle to the angle of the trackball (x, 1.5 y) when it moves; target_heading =
   facing_angle, or with a push the angle of trackball plus push_x/push_y (y 1.5 times unless flags bit
   31). */
void set_target_heading_from_trackball(void)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, push_angle = 0, trackball_angle = 0, trackball_y = 0;

	if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
	{
		trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
		dy = (uint16_t)((int32_t)(PLAYER(current_player)->trackball_y * 0x180u) >> 8);
		angle_of_vector(dy, &trackball_angle, &ignored);
		PLAYER(current_player)->facing_angle = trackball_angle;
	}
	if (PLAYER(current_player)->push_xy == 0u)
	{
		PLAYER(current_player)->target_heading = PLAYER(current_player)->facing_angle;
		return;
	}
	push_angle = (uint16_t)(PLAYER(current_player)->trackball_x + PLAYER(current_player)->push_x);
	trackball_y = PLAYER(current_player)->trackball_y;
	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
		trackball_y = (int32_t)(trackball_y * 0x180u) >> 8;
	trackball_y = (uint16_t)(trackball_y + PLAYER(current_player)->push_y);
	angle_of_vector(trackball_y, &push_angle, &ignored);
	PLAYER(current_player)->target_heading = push_angle;
}

/* Turns heading toward target_heading: at once at speed 0x200 or less, while free_turn_timer runs, with
   flags bit 31, or in states 2, 4, 0x17, 0x25 and 0x26; else by at most 0x6000 / speed_int a frame, a
   quarter turn or more making it skid (state 0xC). Then set_heading_direction. */
void turn_player(void)
{
	uint16_t max_turn = 0, turn = 0;
	uint32_t speed_int = 0;
	uint16_t heading_delta = 0;

	if (PLAYER(current_player)->free_turn_timer != 0u || PLAYER(current_player)->state == 2u || PLAYER(current_player)->state == 4u || PLAYER(current_player)->state == 0x17u || PLAYER(current_player)->state == 0x25u || PLAYER(current_player)->state == 0x26u || (OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
		PLAYER(current_player)->heading &= 0x3FFu;
		set_heading_direction();
		return;
	}
	speed_int = (uint16_t)OBJECT(current_object)->speed_int;
	if ((int16_t)speed_int <= 0x200)
	{
		PLAYER(current_player)->heading = PLAYER(current_player)->target_heading;
		PLAYER(current_player)->heading &= 0x3FFu;
		set_heading_direction();
		return;
	}
	max_turn = (uint16_t)divide_s32(0x180000u, speed_int);
	heading_delta = ((uint32_t)PLAYER(current_player)->target_heading << 6) - ((uint32_t)PLAYER(current_player)->heading << 6);
	turn = heading_delta;
	if ((int16_t)heading_delta < 0)
	{
		turn = (uint16_t)(0u - turn);
		if (turn >= 0x4000u)
		{
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0xCu;
			set_heading_direction();
			return;
		}
		if ((int16_t)max_turn < (int16_t)turn)
			turn = (uint16_t)max_turn;
		PLAYER(current_player)->heading -= turn >> 6;
		PLAYER(current_player)->heading &= 0x3FFu;
		set_heading_direction();
		return;
	}
	if (turn >= 0x4000u)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0xCu;
		set_heading_direction();
		return;
	}
	if ((int16_t)max_turn < (int16_t)turn)
		turn = (uint16_t)max_turn;
	PLAYER(current_player)->heading += turn >> 6;
	PLAYER(current_player)->heading &= 0x3FFu;
	set_heading_direction();
}

/* Sets heading_snapped to heading rounded to one of 16 directions and direction to that + 0x80, then
   set_facing. */
void set_heading_direction(void)
{
	uint32_t snapped_heading = 0;

	snapped_heading = (PLAYER(current_player)->heading + 0x20u) & 0x3C0u;
	PLAYER(current_player)->heading_snapped = snapped_heading;
	OBJECT(current_object)->direction = (snapped_heading + 0x80u) & 0x3C0u;
	snapped_heading = set_facing(snapped_heading);
}

/* Sets facing to facing_angle rounded to one of 16 directions (((facing_angle + 0x20) >> 6) & 0xF) and
   hands it back. */
uint32_t set_facing(uint32_t facing)
{
	uint32_t new_facing = 0;

	new_facing = (facing & ~0xFFFFu) | PLAYER(current_player)->facing_angle;
	new_facing = (new_facing & ~0xFFFFu) | (uint16_t)(new_facing + 0x20u);
	new_facing = (new_facing & ~0xFFFFu) | ((uint16_t)new_facing >> 6);
	new_facing = (new_facing & ~0xFFFFu) | (new_facing & 0xFu);
	PLAYER(current_player)->facing = new_facing;
	return new_facing;
}

/* Turns the player to its movement: heading, target_heading and facing_angle = direction - 0xA0, then
   set_facing. */
uint32_t face_movement_direction(uint32_t facing)
{
	uint32_t new_heading = 0;

	new_heading = (facing & ~0xFFFFu) | OBJECT(current_object)->direction;
	new_heading = (new_heading & ~0xFFFFu) | (uint16_t)(new_heading - 0xA0u);
	new_heading = (new_heading & ~0xFFFFu) | (new_heading & 0x3FFu);
	PLAYER(current_player)->target_heading = new_heading;
	PLAYER(current_player)->heading = new_heading;
	PLAYER(current_player)->facing_angle = new_heading;
	return set_facing(new_heading);
}

/* Sets ground_speed = (speed >> 8) x heading_speed_factors[(heading + 0x20) >> 6]; on a slope it is
   scaled by the slope_speed_direction_pairs entry for floor_slope and direction, whose second word
   becomes direction. */
uint32_t compute_ground_speed(void)
{
	uint32_t carry = 0;
	uint32_t direction = 0, floor_slope = 0, rounded_heading = 0, rounded_speed = 0, slope_pair = 0, speed_factor = 0, speed_table_entry = 0;

	rounded_heading = (uint16_t)(PLAYER(current_player)->heading + 0x20u) & 0x3FFu;
	speed_table_entry = heading_speed_factors[rounded_heading >> 6];
	PLAYER(current_player)->ground_speed = ((uint32_t)OBJECT(current_object)->speed >> 8) * speed_table_entry;
	if (OBJECT(current_object)->floor_slope == 0u)
		return speed_table_entry;
	floor_slope = OBJECT(current_object)->floor_slope;
	speed_table_entry = floor_slope_direction_tables[(uint16_t)(floor_slope << 2)];
	direction = OBJECT(current_object)->direction;
	slope_pair = slope_speed_direction_pairs_ADDRESS + speed_table_entry + (direction >> 6) * 4u;
	speed_factor = SLOPE_SPEED_PAIR(slope_pair)->speed_factor;
	rounded_speed = PLAYER(current_player)->ground_speed;
	carry = (rounded_speed >> 11) & 1u;
	rounded_speed = rounded_speed >> 12;
	slope_pair += 2u;
	if (carry)
		rounded_speed++;
	PLAYER(current_player)->ground_speed = rounded_speed * speed_factor;
	OBJECT(current_object)->direction = SLOPE_SPEED_PAIR(slope_pair - 0x2u)->direction;
	return speed_table_entry;
}

/* Reads the player's trackball motion (trackball_1_x on, 4 bytes per player) into trackball_x and
   trackball_y: each axis with its lowest bit dropped and scaled by 0x100, sign kept. */
void read_player_trackball(void)
{
	uint16_t axis_value = 0;
	uint32_t trackball = 0;

	trackball = trackball_1_x_ADDRESS + OBJECT(current_object)->player * 4u;
	axis_value = (uint16_t)TRACKBALL_MOTION(trackball)->x;
	trackball += 2u;
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_x = axis_value;
	axis_value = (uint16_t)TRACKBALL_MOTION(trackball - 0x2u)->y;
	if ((int16_t)axis_value >= 0)
		axis_value = (uint16_t)((axis_value & ~1u) << 8);
	else
		axis_value = (uint16_t)(0u - (((0u - axis_value) & 0xFFFEu) << 8));
	PLAYER(current_player)->trackball_y = axis_value;
}

/* Player 1's shadow object starts: player = 0, then shadow_start. */
void shadow_1_start(void)
{
	OBJECT(current_object)->player = 0u;
	shadow_start();
}

/* Player 2's shadow object starts: player = 1, then shadow_start. */
void shadow_2_start(void)
{
	OBJECT(current_object)->player = 1u;
	shadow_start();
}

/* Player 3's shadow object starts: player = 2, then shadow_start. */
void shadow_3_start(void)
{
	OBJECT(current_object)->player = 2u;
	shadow_start();
}

/* Sets up the player's shadow object (frame 0x5D90, shadow_update) and a child block (allocate_object_2)
   for the attack effect; in stage 9 a second child (sprite_flags 0x1088) for a copy of the effect on
   floor type 7. */
void shadow_start(void)
{
	uint32_t free_count = 0, ignored = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1008u;
	OBJECT(current_object)->sprite_palette = 0x1FFEu;
	OBJECT(current_object)->sprite_frame = 0x5D90u;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_layer = 0u;
	OBJECT(current_object)->z = 0;
	OBJECT(current_object)->shadow.flicker_timer = 0;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -2;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = shadow_update_ADDRESS;
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &ignored, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->child = object;
		OBJECT(object)->child = 0xFFFFu;
		if (stage == 9u)
		{
			object_2 = object;
			allocate_object_2(&free_count, &ignored, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1088u;
				OBJECT(object)->parent = object_2;
				OBJECT(object_2)->child = object;
			}
		}
	}
}

/* Draws the player's shadow (current_player is borrowed for the player's object): on its floor,
   flickering on some floors, sinking on floor types 0xA and 0xB (setting ground_clip_bottom), the
   player's reflection in stage 9 on floor types 7 and 8, or game mode 0x10's by scene_step; none over a
   pit. Then draw_attack_effect. */
void shadow_update(void)
{
	uint16_t priority_bits = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, offset_y = 0, player = 0, point_x = 0, point_y = 0, scaled_offset_y = 0, scaled_player = 0, scene_player = 0, sprite_scale = 0, z = 0;
	uint8_t flicker_timer = 0;

	player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
	current_player = LINKED(current_player, PLAYER(player)->object);
	PLAYER(player)->flags &= ~1u;
	if ((PLAYER(player)->flags & 8u) != 0u || PLAYER(player)->state == 0xBu || PLAYER(player)->state == 0x23u || PLAYER(player)->state == 0x24u || PLAYER(player)->state == 0x16u || PLAYER(player)->state == 0x2Eu || PLAYER(player)->state == 0x37u)
		return;
	if (PLAYER(player)->state == 0x10u)
	{
		draw_attack_effect(player);
		return;
	}
	if (PLAYER(player)->state == 0x11u || PLAYER(player)->state == 0x12u || PLAYER(player)->state == 0x15u || PLAYER(player)->state == 0x1Du || PLAYER(player)->state == 0x27u || PLAYER(player)->state == 0x2Du || PLAYER(player)->state == 0x34u || PLAYER(player)->state == 0x35u || PLAYER(player)->state == 0x39u || (PLAYER(player)->state == 0x1Eu && (uint8_t)intro_character != OBJECT(current_player)->kind) || (PLAYER(player)->state == 0x32u && (int8_t)PLAYER(player)->substate < 3) || (PLAYER(player)->state == 0x25u && ((PLAYER(player)->sequence_step >= 8 && PLAYER(player)->sequence_step <= 0xC) || PLAYER(player)->sequence_step == 0xEu)) || (OBJECT(current_player)->flags & 0x10000000u) != 0u || (current_game_mode == 0xAu && (int8_t)scene_step >= 6))
		return;
	if (current_game_mode != 0x10u)
	{
		if (stage == 9u && (OBJECT(current_player)->floor_type == 8u || (OBJECT(current_player)->floor_type == 7u && (OBJECT(current_player)->flags & 0x10u) == 0u)))
		{
			if (OBJECT(current_player)->floor_block >= 0)
			{
				OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
				OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
				OBJECT(current_object)->z = (uint32_t)OBJECT(current_player)->floor_z - OBJECT(current_player)->z + OBJECT(current_player)->floor_z;
				OBJECT(current_object)->sprite_frame = OBJECT(current_player)->sprite_frame;
				OBJECT(current_object)->sprite_palette = OBJECT(current_player)->sprite_palette;
				OBJECT(current_object)->sprite_palette &= 0x1FFFu;
				OBJECT(current_object)->sprite_palette |= 0x2000u;
				OBJECT(current_object)->sprite_flags = 0x1188u;
				OBJECT(current_object)->sprite_flags |= OBJECT(current_player)->sprite_flags & 0x40u;
				update_screen_position(&ignored, &ignored_2);
				depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x18u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			draw_attack_effect(player);
			return;
		}
		if (PLAYER(player)->state == 0x28u || (OBJECT(current_player)->floor_type != 0xAu && (OBJECT(current_player)->flags & 0x80000000u) == 0u && OBJECT(current_player)->floor_type != 0xBu))
		{
			if (PLAYER(player)->state == 0x33u)
			{
				OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
				OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
				OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
				if (OBJECT(current_player)->floor_block < 0)
					OBJECT(current_object)->z = OBJECT(current_player)->previous_floor_z;
				OBJECT(current_object)->z_int += 8u;
				OBJECT(current_object)->sprite_flags = 0x1004u;
				OBJECT(current_object)->flags &= ~0x80000u;
				OBJECT(current_object)->sprite_palette = 0x2D50u;
				priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
				OBJECT(current_object)->sprite_palette &= 0x9FFFu;
				OBJECT(current_object)->sprite_palette |= priority_bits;
				OBJECT(current_object)->animation = pit_sinking_shadow_animation_ADDRESS;
				animate(&ignored_2, &ignored_3, &ignored_4);
				update_screen_position(&ignored, &ignored_2);
				depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
				queue_sprite_at_depth(&ignored, &depth);
				draw_attack_effect(player);
				return;
			}
			if (player_status[OBJECT(current_object)->player] == 0x82u)
				return;
			if (OBJECT(current_player)->floor_block < 0)
			{
				draw_attack_effect(player);
				return;
			}
			OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
			OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
			OBJECT(current_object)->projected_y_int -= 3u;
			OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
			OBJECT(current_object)->sprite_flags = 0x1008u;
			OBJECT(current_object)->sprite_frame = 0x5D90u;
			OBJECT(current_object)->flags &= ~0x80000u;
			if (OBJECT(current_player)->floor_type == 9u)
			{
				OBJECT(current_object)->sprite_palette = 0x87A0u;
				flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
				OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
				if ((int8_t)flicker_timer <= 1)
				{
					OBJECT(current_object)->shadow.flicker_timer = 2;
					OBJECT(current_object)->flags |= 0x80000u;
				}
			}
			else
			{
				if (OBJECT(current_player)->floor_type == 4u || (stage == 0x10u && mem_read8(floor_blocks + 2u + OBJECT(current_player)->floor_block + 0x12u) == 0x20u))
				{
					OBJECT(current_object)->sprite_palette = 0x7A0u;
					flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
					OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
					if ((int8_t)flicker_timer <= 1)
					{
						OBJECT(current_object)->shadow.flicker_timer = 2;
						OBJECT(current_object)->flags |= 0x80000u;
					}
					priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
					OBJECT(current_object)->sprite_palette &= 0x9FFFu;
					OBJECT(current_object)->sprite_palette |= priority_bits;
					update_screen_position(&ignored, &offset_y);
					if (per_player_view_mode != 0u && scroll_position >= 0xA5F0u)
					{
						offset_y = (offset_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
						OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(player)->own_view_y + offset_y;
					}
					if ((OBJECT(current_object)->flags & 0x80000u) == 0u)
						queue_sprite(offset_y);
					draw_attack_effect(player);
					return;
				}
				if ((current_game_mode_word != 0x1Au || (int8_t)PLAYER(player)->sequence_step < 2) && (OBJECT(current_player)->flags & 0x10u) == 0u)
					OBJECT(current_object)->sprite_palette = 0x1FFEu;
				else
				{
					OBJECT(current_object)->sprite_palette = 0x87A0u;
					flicker_timer = OBJECT(current_object)->shadow.flicker_timer;
					OBJECT(current_object)->shadow.flicker_timer = flicker_timer - 1u;
					if ((int8_t)flicker_timer <= 1)
					{
						OBJECT(current_object)->shadow.flicker_timer = 2;
						OBJECT(current_object)->flags |= 0x80000u;
					}
				}
			}
			priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
			OBJECT(current_object)->sprite_palette &= 0x9FFFu;
			OBJECT(current_object)->sprite_palette |= priority_bits;
			update_screen_position(&ignored, &offset_y);
			if (per_player_view_mode != 0u && scroll_position >= 0xA5F0u)
			{
				offset_y = (offset_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
				OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(player)->own_view_y + offset_y;
			}
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			{
				draw_attack_effect(player);
				return;
			}
			queue_sprite(offset_y);
			draw_attack_effect(player);
			return;
		}
		if (PLAYER(player)->state == 6u || PLAYER(player)->state == 8u || PLAYER(player)->state == 7u || PLAYER(player)->state == 9u)
		{
			draw_attack_effect(player);
			return;
		}
		PLAYER(player)->flags |= 1u;
		OBJECT(current_object)->projected_x = OBJECT(current_player)->projected_x;
		OBJECT(current_object)->projected_y = OBJECT(current_player)->projected_y;
		OBJECT(current_object)->z = OBJECT(current_player)->floor_z;
		if (OBJECT(current_player)->floor_block < 0)
			OBJECT(current_object)->z = OBJECT(current_player)->previous_floor_z;
		if (OBJECT(current_player)->floor_block < 0)
			z = OBJECT(current_player)->previous_floor_z;
		else
			z = OBJECT(current_player)->floor_z;
		point_x = OBJECT(current_player)->x;
		point_y = OBJECT(current_player)->y;
		project_point(z, &ignored_2, &point_x, &point_y);
		PLAYER(player)->ground_clip_bottom = point_y;
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->flags &= ~0x80000u;
		OBJECT(current_object)->sprite_palette = 0x1020u;
		OBJECT(current_object)->sprite_scale = 0x120u;
		priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= priority_bits;
		OBJECT(current_object)->animation = sinking_shadow_animation_ADDRESS;
		animate(&ignored_2, &ignored_3, &ignored_4);
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		draw_attack_effect(player);
		return;
	}
	switch (scene_step)
	{
	case 0:
	case 1:
	case 4:
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->sprite_palette = 0x160u;
		scene_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
		scene_player = LINKED(scene_player, PLAYER(scene_player)->object);
		OBJECT(current_object)->screen_x = OBJECT(scene_player)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(scene_player)->screen_y;
		OBJECT(current_object)->screen_y -= 8u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->sprite_flags = 0u;
		OBJECT(current_object)->sprite_frame = 0x1B60u;
		OBJECT(current_object)->sprite_palette = 0x8130u;
		OBJECT(current_object)->sprite_width = 0;
		OBJECT(current_object)->sprite_height = 8;
		return;
	case 3:
		scaled_player = PLAYER_BLOCK(1) + ((uint32_t)OBJECT(current_object)->player << 7) * 4u;
		scaled_player = LINKED(scaled_player, PLAYER(scaled_player)->object);
		OBJECT(current_object)->sprite_scale = OBJECT(scaled_player)->sprite_scale;
		OBJECT(current_object)->screen_x = OBJECT(scaled_player)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(scaled_player)->screen_y;
		sprite_scale = OBJECT(current_object)->sprite_scale;
		OBJECT(current_object)->screen_x += (int32_t)(OBJECT(current_object)->sprite_width * sprite_scale) >> 8;
		scaled_offset_y = (int32_t)(OBJECT(current_object)->sprite_height * sprite_scale) >> 8;
		OBJECT(current_object)->screen_y += scaled_offset_y;
		scaled_offset_y = queue_sprite(scaled_offset_y);
		return;
	default:
		invalid_jump(0x6E3Eu + scene_step * 3u);
	}
}

/* Draws the attack effect (the shadow's child) over the player while it attacks (flags bit 6) with
   attack_power 3 or more: the frame for its attack_power, facing and animation_frame, its character's
   palette, just in front of it; on floor type 7 the second child (stage 9) copies it. current_player is
   the player object here. */
void draw_attack_effect(uint32_t player)
{
	uint16_t palette = 0, priority_bits = 0;
	uint32_t depth = 0, effect_record = 0, facing_offset = 0, ignored = 0, object = 0, object_2 = 0;

	if ((OBJECT(current_player)->flags & 0x40u) == 0u || PLAYER(player)->attack_power <= 2 || PLAYER(player)->state == 9u || PLAYER(player)->state == 0xAu || (OBJECT(current_player)->flags & 0x80000000u) != 0u || (OBJECT(current_player)->flags & 0x20000u) != 0u || (int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	effect_record = attack_power_3_effect_animations_ADDRESS;
	if (PLAYER(player)->attack_power > 3)
		effect_record = attack_power_4_effect_animations_ADDRESS;
	facing_offset = PLAYER_FACING_ANIMATION_OFFSETS(effect_record)[PLAYER(player)->facing];
	effect_record = effect_record + facing_offset + 2u + OBJECT(current_player)->animation_frame * 2u * 2u;
	OBJECT(object)->sprite_flags = (OBJECT(object)->sprite_flags & ~0xC0u) | FLAGGED_STEP(effect_record)->flags;
	OBJECT(object)->sprite_frame = FLAGGED_STEP(effect_record)->frame;
	palette = 0x8320u;
	if (OBJECT(current_player)->kind != 0u)
	{
		palette = 0x8700u;
		if (OBJECT(current_player)->kind != 1u)
			palette = 0x8710u;
	}
	OBJECT(object)->sprite_palette = palette;
	priority_bits = OBJECT(current_player)->sprite_palette & 0x6000u;
	OBJECT(object)->sprite_palette &= 0x9FFFu;
	OBJECT(object)->sprite_palette |= priority_bits;
	OBJECT(object)->screen_x = OBJECT(current_player)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_player)->screen_y;
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	depth = (uint16_t)(depth + 8u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	if (OBJECT(current_player)->floor_block >= 0 && (int16_t)OBJECT(object)->child >= 0 && OBJECT(current_player)->floor_type == 7u && (OBJECT(current_player)->flags & 0x10u) == 0u)
	{
		object_2 = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
		OBJECT(object)->sprite_frame = OBJECT(object_2)->sprite_frame;
		OBJECT(object)->sprite_flags &= ~0x40u;
		OBJECT(object)->sprite_flags |= OBJECT(object_2)->sprite_flags & 0x40u;
		OBJECT(object)->sprite_palette = OBJECT(object_2)->sprite_palette;
		priority_bits = OBJECT(object_cursor)->sprite_palette & 0xE000u;
		OBJECT(object)->sprite_palette &= 0x1FFFu;
		OBJECT(object)->sprite_palette |= priority_bits;
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object;
		queue_sprite(depth);
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	}
}

/* Sets step_floor_z, the height the player stands on: its floor_z, or a flat platform's z (snapped to at
   once over a block of type 0x24); then check_player_step_against_floor. In stage 8 before scroll 0x4000
   the player just snaps to its floor. */
void check_player_step(uint32_t *p_height_diff, uint32_t *p_cursor)
{
	uint32_t object = 0;

	step_floor_z = OBJECT(current_object)->floor_z;
	if (stage == 8u && scroll_position < 0x4000u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		return;
	}
	if ((OBJECT(current_object)->flags & 8u) == 0u)
	{
		check_player_step_against_floor(p_height_diff, p_cursor);
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->platform);
	if (OBJECT(object)->floor_slope != 0u)
	{
		check_player_step_against_floor(p_height_diff, p_cursor);
		return;
	}
	step_floor_z = OBJECT(object)->z;
	if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x24u)
	{
		check_player_step_against_floor(p_height_diff, p_cursor);
		return;
	}
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = step_floor_z;
}

/* Puts the player on step_floor_z: on a path or within 8, z snaps to it (previous_z keeps the old z); a
   lower floor starts a fall (state 2); a higher one blocks it unless sloped: back to its previous place
   and floor, bouncing (state 0xE, away from the block's side) at speed 0x500 or more, else stopping
   (state 0). */
void check_player_step_against_floor(uint32_t *p_height_diff, uint32_t *p_cursor)
{
	uint32_t floor_block = 0, object_z = 0, opposite_side = 0, y = 0;
	uint16_t floor_z_int = 0, z_int = 0, height_diff_2 = 0;
	uint32_t bounce_direction_halfword = 0;

	if (PLAYER(current_player)->path != 0u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		return;
	}
	object_z = (*p_height_diff & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->z_int;
	floor_z_int = step_floor_z_int;
	z_int = object_z;
	*p_height_diff = (object_z & ~0xFFFFu) | (uint16_t)(z_int - floor_z_int);
	if (z_int == floor_z_int)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		return;
	}
	*p_height_diff = (*p_height_diff & ~0xFFFFu) | (uint16_t)(*p_height_diff + 8u);
	if ((uint16_t)*p_height_diff < 0x11u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		return;
	}
	height_diff_2 = *p_height_diff - 8u;
	*p_height_diff = (*p_height_diff & ~0xFFFFu) | height_diff_2;
	if ((int16_t)height_diff_2 < 0)
	{
		if (PLAYER(current_player)->state == 2u)
		{
			return;
		}
		PLAYER(current_player)->state = 2u;
		PLAYER(current_player)->substate = 0u;
		return;
	}
	if (OBJECT(current_object)->floor_slope != 0u)
	{
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z = step_floor_z;
		return;
	}
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
	step_blocking_floor_block = OBJECT(current_object)->floor_block;
	OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	PLAYER(current_player)->control_lock = 0u;
	if (OBJECT(current_object)->speed_int >= 0x500)
	{
		floor_block = step_blocking_floor_block;
		*p_cursor = OBJECT(current_object)->x;
		y = OBJECT(current_object)->y;
		opposite_side = get_floor_block_side(floor_block, *p_cursor, y);
		opposite_side = (opposite_side & ~0xFFu) | (uint8_t)(opposite_side + 2u);
		bounce_direction_halfword = (opposite_side & ~0xFFu) | (opposite_side & 3u);
		PLAYER(current_player)->bounce_direction = (uint8_t)bounce_direction_halfword;
		PLAYER(current_player)->bounce_speed_z = (PLAYER(current_player)->bounce_speed_z & 0xFFFFFF00u) | ((bounce_direction_halfword >> 8u) & 0xFFu);
		PLAYER(current_player)->state = 0xEu;
		PLAYER(current_player)->substate = 0u;
		return;
	}
	*p_height_diff = 0u;
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->free_turn_timer = 3u;
	OBJECT(current_object)->flags |= 0x800000u;
	PLAYER(current_player)->state = 0u;
	if (step_floor_z <= OBJECT(current_object)->z)
	{
		return;
	}
	if (PLAYER(current_player)->state == 2u)
	{
		return;
	}
	PLAYER(current_player)->state = 2u;
	PLAYER(current_player)->substate = 0u;
}

/* When the player has moved over a floor block at least 8 higher than its last and is below its top,
   puts it back at its previous position and floor and turns it 0x80 to one side of straight away from
   the side it hit (get_floor_block_side), facing that way, with control_lock 0xA and flags bit 13. */
void bounce_off_wall_in_air(void)
{
	uint32_t floor_block = 0, new_direction = 0, x = 0, y = 0;

	if (OBJECT(current_object)->z_int <= OBJECT(current_object)->floor_z_int || (int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->floor_z_int) < 8)
		return;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	floor_block = OBJECT(current_object)->floor_block;
	OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
	x = OBJECT(current_object)->x;
	y = OBJECT(current_object)->y;
	new_direction = get_floor_block_side(floor_block, x, y);
	new_direction = ((new_direction << 8) + 0x200u) & 0x3FFu;
	if ((int16_t)(((uint32_t)OBJECT(current_object)->direction << 6) - (new_direction << 6)) >= 0)
		new_direction = (uint16_t)(new_direction + 0x180u);
	else
		new_direction = (uint16_t)(new_direction - 0x180u);
	OBJECT(current_object)->direction = new_direction & 0x3FFu;
	face_movement_direction(0u);
	PLAYER(current_player)->control_lock = 0xAu;
	OBJECT(current_object)->flags |= 0x2000u;
}

/* Hands back which side of floor block floor_block the point x, y is on: 3 before x_min, 1 past x_max,
   else 0 before y_min and 2 otherwise (y_max is not checked: the point is taken to be outside the
   block). */
uint32_t get_floor_block_side(uint32_t floor_block, uint32_t x, uint32_t y)
{
	uint32_t side = 0;
	uint32_t x_min = 0, x_max = 0;

	floor_block = floor_block + floor_blocks + 2u;
	x_min = FLOOR_BLOCK(floor_block)->x_min;
	floor_block += 4u;
	if ((int32_t)x < (int32_t)x_min)
	{
		side = 3u;
		return side;
	}
	x_max = FLOOR_BLOCK(floor_block - 0x4u)->x_max;
	floor_block += 4u;
	if ((int32_t)x <= (int32_t)x_max)
	{
		if (FLOOR_BLOCK(floor_block - 0x8u)->y_min <= (int32_t)y)
		{
			side = 2u;
			return side;
		}
		side = 0u;
		return side;
	}
	side = 1u;
	return side;
}

/* Unused: as get_floor_block_side with the corners: hands back the outcode of x, y against block (1 past
   x_max, 2 before x_min, 4 before y_min, 8 past y_max) and side_code = floor_block_side_codes[outcode]. */
void get_floor_block_side_or_corner(uint32_t block, uint32_t x, uint32_t y, uint32_t *p_outcode, uint32_t *p_side_code)
{
	uint32_t outcode = 0;

	outcode = 0u;
	block = block + floor_blocks + 2u;
	if (FLOOR_BLOCK(block)->x_min <= (int32_t)x)
	{
		if (FLOOR_BLOCK(block)->x_max < (int32_t)x)
			outcode = outcode | 1u;
	}
	else
		outcode = outcode | 2u;
	if (FLOOR_BLOCK(block)->y_min <= (int32_t)y)
	{
		if (FLOOR_BLOCK(block)->y_max < (int32_t)y)
			outcode = outcode | 8u;
	}
	else
		outcode = outcode | 4u;
	*p_side_code = floor_block_side_codes[outcode];
	*p_outcode = outcode;
}

/* Puts the player at its restart point: the record at current_restart_point as map x, y (kinds 0, 2) or
   a screen point on the ground (kind 1), players 2 apart in x (the third on the other side); at a stage
   start in a game (flags bit 25, not game mode 1) the 6 bytes before the stage's list, x + player
   shifted by their first word. */
uint32_t place_player_at_restart_point(void)
{
	uint8_t offset_shift = 0;
	uint32_t kind_offset = 0, player_offset = 0, record = 0, restart_x = 0, restart_y = 0, spacing_step = 0, stage_index = 0, z = 0;

	if ((OBJECT(current_object)->flags & 0x2000000u) != 0u && game_in_progress != 0u && current_game_mode != 1u)
	{
		stage_index = stage;
		record = stage_restart_points_ADDRESS + stage_restart_points[stage_index] - 6u;
		offset_shift = (uint8_t)STAGE_START_POINT(record)->spacing_shift;
		record += 2u;
		OBJECT(current_object)->x_int = STAGE_START_POINT(record - 0x2u)->x;
		player_offset = OBJECT(current_object)->player;
		player_offset = shift_signed_by(player_offset, (uint8_t)offset_shift, 16);
		OBJECT(current_object)->x_int += player_offset;
		OBJECT(current_object)->x_fraction = 0u;
		OBJECT(current_object)->y_int = STAGE_START_POINT(record - 0x2u)->y;
		OBJECT(current_object)->y_fraction = 0u;
		return player_offset;
	}
	record = current_restart_point + 2u;
	kind_offset = RESTART_POINT(record - 0x2u)->kind * 3u;
	record++;
	switch (kind_offset)
	{
	case 0x0u:
	case 0x6u:
		record++;
		OBJECT(current_object)->x_int = RESTART_POINT(record - 0x4u)->x;
		player_offset = OBJECT(current_object)->player;
		record += 2u;
		if (player_offset == 2u)
			player_offset = 0xFFFFu;
		player_offset = (uint16_t)(player_offset << 1);
		OBJECT(current_object)->x_int += player_offset;
		OBJECT(current_object)->x_fraction = 0u;
		OBJECT(current_object)->y_int = RESTART_POINT(record - 0x6u)->y_or_height;
		OBJECT(current_object)->y_fraction = 0u;
		return player_offset;
	case 0x3u:
		restart_y = RESTART_POINT(record - 0x3u)->screen_y;
		record++;
		restart_x = RESTART_POINT(record - 0x4u)->x;
		z = (uint32_t)RESTART_POINT(record - 0x4u)->y_or_height << 16;
		screen_point_to_ground(z, &restart_x, &restart_y);
		spacing_step = OBJECT(current_object)->player;
		if (spacing_step == 2u)
			spacing_step = 0xFFFFu;
		spacing_step = spacing_step << 17;
		OBJECT(current_object)->x = restart_x + spacing_step;
		OBJECT(current_object)->y = restart_y;
		return spacing_step;
	default:
		invalid_jump(0xCC13u + kind_offset);
	}
}

/* Hands back in dx and dy a step of 0x200 from the player toward the ground point under the screen
   centre (0xA0, 0x70) at its z; check_player_contacts_solid pushes the player out with it. */
void aim_at_screen_centre(uint32_t *p_cos_part, uint32_t *p_dx, uint32_t *p_dy)
{
	uint16_t centre_dy = 0;
	uint32_t centre_angle = 0, centre_x = 0, centre_y = 0, ignored = 0, z = 0;

	centre_x = 0xA0u;
	centre_y = 0x70u;
	z = OBJECT(current_object)->z;
	screen_point_to_ground(z, &centre_x, &centre_y);
	centre_angle = (int32_t)(centre_x - OBJECT(current_object)->x) >> 12;
	centre_dy = (uint16_t)((int32_t)(centre_y - OBJECT(current_object)->y) >> 12);
	angle_of_vector(centre_dy, &centre_angle, &ignored);
	*p_dx = (uint16_t)centre_angle;
	*p_dy = 0x200u;
	polar_to_xy(p_cos_part, p_dx, p_dy);
}

/* Every flash_period frames swaps the player's sprite_palette between its own (from palette_list) and
   0x83B0. */
void flash_hurt_palette(void)
{
	uint16_t palette = 0;
	uint8_t flash_timer = 0, flashing = 0;

	flash_timer = OBJECT(current_object)->flash_hurt_palette.flash_timer;
	OBJECT(current_object)->flash_hurt_palette.flash_timer = flash_timer - 1u;
	if ((int8_t)flash_timer > 1)
		return;
	OBJECT(current_object)->flash_hurt_palette.flash_timer = OBJECT(current_object)->flash_hurt_palette.flash_period;
	palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->flash_hurt_palette.palette_list)->normal_palette;
	flashing = ~OBJECT(current_object)->flash_hurt_palette.flash_on;
	OBJECT(current_object)->flash_hurt_palette.flash_on = flashing;
	if (flashing != 0u)
		palette = 0x83B0u;
	OBJECT(current_object)->sprite_palette = palette;
}

/* Sets the player's sprite priority bits (the top three of sprite_palette) for its floor: 0x4000 on
   floor type 4, 0xA000 on floor type 0xC when the block it came from is of type 0xC too. */
void set_floor_sprite_priority(void)
{
	if (OBJECT(current_object)->floor_type == 4u)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	if (floor_types[mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u)].floor_type == 0xCu && OBJECT(current_object)->floor_type == 0xCu)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x2000u;
		OBJECT(current_object)->sprite_palette |= 0x8000u;
	}
}

/* While the quicksand holds the player (flags bit 31) its floor_z and z go 0x4EC4 x (0x68 -
   touch_distance) lower, and with block flags bit 2 it sinks: sink_depth + 0x33 a frame added to z; at
   0x2000 deep it loses 8 health and respawns, or dies (state 0x16). */
uint32_t update_quicksand_object(uint32_t sink_offset)
{
	uint8_t health = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		sink_offset = 0x4EC4u * (uint16_t)(0x68u - OBJECT(current_object)->touch_distance);
		OBJECT(current_object)->floor_z += sink_offset;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if ((PLAYER(current_player)->flags & 4u) != 0u)
		{
			PLAYER(current_player)->sink_depth += 0x33u;
			OBJECT(current_object)->z += (uint32_t)(uint16_t)PLAYER(current_player)->sink_depth << 8;
			if (PLAYER(current_player)->sink_depth >= 0x2000)
			{
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
			}
		}
	}
	return sink_offset;
}

/* On floor type 0xA the player sinks: sink_depth grows 0x33 a frame less struggle_speed / 16 (at least
   0x100, and 0xA0 back when past the floor's sink_depth_limits entry), z = floor_z + sink_depth << 8; at
   0x2000 deep it loses 8 health and respawns, or dies (state 0x16). */
uint32_t update_sinking_floor(uint32_t sink_offset)
{
	uint8_t health = 0;

	if (OBJECT(current_object)->floor_type != 0xAu)
		return sink_offset;
	if (OBJECT(current_object)->floor_block >= 0)
	{
		PLAYER(current_player)->sink_depth += 0x33u;
		PLAYER(current_player)->sink_depth -= PLAYER(current_player)->struggle_speed >> 4;
		if (PLAYER(current_player)->sink_depth < 0x100)
			PLAYER(current_player)->sink_depth = 0x100;
		if (sink_depth_limits[OBJECT(current_object)->floor_attribute] < PLAYER(current_player)->sink_depth)
			PLAYER(current_player)->sink_depth -= 0xA0u;
		sink_offset = (uint32_t)(uint16_t)PLAYER(current_player)->sink_depth << 8;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z + sink_offset;
	}
	if (PLAYER(current_player)->sink_depth < 0x2000)
		return sink_offset;
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
	return sink_offset;
}

/* Carries the player with a moving floor: on floor type 7 whose floor attribute's ride is 1, x +=
   moving_floor_object's speed_x; on floor type 0xC, y += its platform's speed_y (not while jumping, nor
   while teetering off a block not of type 0xC). */
void apply_moving_floor(void)
{
	if (OBJECT(current_object)->floor_type == 7u && FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->ride == 1u)
		OBJECT(current_object)->x += OBJECT(LINKED(current_object, moving_floor_object))->speed_x;
	if (PLAYER(current_player)->state != 4u && (PLAYER(current_player)->state != 5u || floor_types[mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u)].floor_type == 0xCu) && OBJECT(current_object)->floor_type == 0xCu)
		OBJECT(current_object)->y += OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->speed_y;
}

/* When check_zones put the player in a zone (flags bit 27, the zone in anchor_x): kind 0x16 sends it to
   state 0x22; kinds 9, 0xC, 0x14, 0x18, 0x1E and 0x21 count as a touch (flags bit 2, touched_by) for
   check_player_contacts. */
void check_zone_contact(void)
{
	uint32_t object = 0;

	if ((OBJECT(current_object)->flags & 0x8000000u) == 0u)
		return;
	object = (current_object & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->anchor_x;
	if (OBJECT(object)->kind != 0x16u)
	{
		if (OBJECT(object)->kind == 0x18u || OBJECT(object)->kind == 0xCu || OBJECT(object)->kind == 0x1Eu || OBJECT(object)->kind == 9u || OBJECT(object)->kind == 0x21u || OBJECT(object)->kind == 0x14u)
		{
			OBJECT(current_object)->flags |= 4u;
			OBJECT(current_object)->touched_by = OBJECT(current_object)->anchor_x;
		}
	}
	else
	{
		PLAYER(current_player)->state = 0x22u;
		PLAYER(current_player)->substate = 0u;
	}
}

/* With enter_on_respawn set, a stage that has an entry path (stage_player_entries) sends the player to
   state 0x2A (entering the stage); otherwise enter_on_respawn is cleared. */
void check_stage_entry_path(void)
{
	if (PLAYER(current_player)->enter_on_respawn == 0u)
		return;
	if (stage_player_entries[stage] != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x2Au;
		return;
	}
	PLAYER(current_player)->enter_on_respawn = 0u;
}

/* Loads the stage's goal from its stage_goals record: goal_scroll_position, goal_direction, goal_line,
   then, past a word it skips, goal_line_min and goal_line_max. */
void load_stage_goal(void)
{
	uint32_t goal_record = 0;

	goal_record = stage_goals[stage] + stage_goals_ADDRESS;
	goal_scroll_position = STAGE_GOAL(goal_record)->position;
	goal_record += 2u;
	goal_direction = STAGE_GOAL(goal_record - 0x2u)->direction;
	goal_record += 2u;
	goal_line = STAGE_GOAL(goal_record - 0x4u)->line;
	goal_record = ((goal_record + 2u) & ~0xFFFFu) | (uint16_t)(goal_record + 2u + 2u);
	goal_line_min = STAGE_GOAL_LINE_RANGE(goal_record)->line_min;
	goal_line_max = STAGE_GOAL_LINE_RANGE(goal_record)->line_max;
}

/* Ends the stage for the player (state 0x21) once scroll_position reaches goal_scroll_position and it
   crosses goal_line by goal_direction (between goal_line_min and goal_line_max unless 0). Stage 0: 0x18
   at once; 4: when goal_unlocked; 5: 0x18 with scene_path 0xB, or 0x11 once the spiked press has stood
   0x258 frames. */
void check_player_reached_goal(void)
{
	uint32_t object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u || (uint8_t)OBJECT(current_object)->health == 0u || PLAYER(current_player)->state == 9u || PLAYER(current_player)->state == 0x24u || (uint16_t)OBJECT(current_object)->floor_z_int == 0x800u || current_game_mode == 0x20u || PLAYER(current_player)->state == 0x28u)
		return;
	switch ((uint8_t)stage)
	{
	case 0:
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x18u;
		return;
	case 1:
	case 2:
	case 3:
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
		if (scroll_position < goal_scroll_position)
			return;
		switch (goal_direction)
		{
		case 0:
		case 4:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->x_int && OBJECT(current_object)->x_int <= goal_line_max)) && OBJECT(current_object)->y_int <= goal_line)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
			}
			break;
		case 1:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->x_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		case 2:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->x_int && OBJECT(current_object)->x_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		case 3:
			if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && OBJECT(current_object)->x_int <= goal_line)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x21u;
				return;
			}
			break;
		default:
			invalid_jump(0xD0BEu + goal_direction * 2u);
		}
		return;
	case 4:
		if (goal_scroll_position <= scroll_position && goal_unlocked != 0u)
			PLAYER(current_player)->state = 0x21u;
		return;
	case 5:
		if (scroll_position < goal_scroll_position)
			return;
		if (((uint16_t)goal_line_min == 0u || (goal_line_min <= OBJECT(current_object)->y_int && OBJECT(current_object)->y_int <= goal_line_max)) && goal_line <= OBJECT(current_object)->x_int)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x18u;
			PLAYER(current_player)->scene_path = 0xBu;
			return;
		}
		object = LINKED(current_object, spiked_press_object);
		if ((uint16_t)object != 0u && OBJECT(object)->spiked_press.stopped_frames >= 0x258)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x18u;
			PLAYER(current_player)->scene_path = 0x11u;
		}
		return;
	default:
		invalid_jump(0xD06Du + (uint8_t)stage * 3u);
	}
}

/* Spawns the tumble effect over the player (tumble_effect_animation, tumble_effect_update), flipped when
   the player's facing is 1 to 7. */
void spawn_tumble_effect(uint32_t effect)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &effect);
	if (free_count != 0u)
	{
		OBJECT(effect)->sprite_flags = 0x1004u;
		OBJECT(effect)->sprite_palette = 0x8360u;
		OBJECT(effect)->sprite_offset_x = 0;
		OBJECT(effect)->sprite_offset_y = 0;
		OBJECT(effect)->flags = 0u;
		OBJECT(effect)->handler = tumble_effect_update_ADDRESS;
		OBJECT(effect)->animation = tumble_effect_animation_ADDRESS;
		OBJECT(effect)->parent = current_object;
		if (PLAYER(current_player)->facing >= 1 && PLAYER(current_player)->facing <= 7)
			OBJECT(effect)->sprite_flags |= 0x40u;
	}
}

/* The tumble effect: at its player's screen position, deleted once the player leaves state 0x20 or its
   animation_frame reaches 0xB; its frame follows the player's animation_frame (moved by
   tumble_effect_y_offsets), drawn by the player's draw_depth and facing. */
void tumble_effect_update(void)
{
	uint32_t animation_frame = 0, depth = 0, ignored = 0, object = 0, player = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state != 0x20u || (int8_t)OBJECT(object)->animation_frame >= 0xB)
	{
		delete_current_object();
		return;
	}
	animation_frame = OBJECT(object)->animation_frame;
	OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[animation_frame];
	OBJECT(current_object)->screen_y += tumble_effect_y_offsets[animation_frame];
	depth = (uint16_t)(PLAYER(player)->draw_depth - 0x10u + tumble_effect_depth_offsets[PLAYER(player)->facing]);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Hands back target_speed x speed_scale / 16 when speed_scale is set, else target_speed. */
uint32_t apply_speed_scale(uint32_t target_speed)
{
	if (PLAYER(current_player)->speed_scale != 0u)
		target_speed = (target_speed * PLAYER(current_player)->speed_scale) >> 4;
	return target_speed;
}

/* Lets the scene decide how the player enters: it waits (entry_status 0x82, player_place staying its
   handler) in several game modes and events (a stage exit, modes 0xA, 0x1A, 0x1D, the collapsing rope
   bridge, the ride...), or starts in a scene state: 0x1C in game mode 0x10, 0x1E in 1, 0x2B in 0x18,
   0x34 in 0x1B. */
void set_player_entry_by_scene(void)
{
	if (current_game_mode == 0xAu && (int8_t)scene_step > 1)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 2u && player_status[OBJECT(current_object)->player] == 0x82u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x10u)
	{
		if ((OBJECT(current_object)->flags & 0x2000000u) != 0u)
		{
			OBJECT(current_object)->flags &= ~0x2000000u;
			PLAYER(current_player)->state = 0x1Cu;
			PLAYER(current_player)->entry_status = 0xFFu;
			OBJECT(current_object)->handler = player_update_ADDRESS;
		}
		else
		{
			PLAYER(current_player)->entry_status = 0x82u;
			OBJECT(current_object)->handler = player_place_ADDRESS;
		}
	}
	if (current_game_mode == 1u && OBJECT(current_object)->set_player_entry_by_scene.floor_block_high != 0xFFFFu)
	{
		PLAYER(current_player)->state = 0x1Eu;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (current_game_mode == 0x18u)
	{
		PLAYER(current_player)->state = 0x2Bu;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (current_game_mode == 0x1Bu && game_mode == 0x1Bu)
	{
		PLAYER(current_player)->state = 0x34u;
		PLAYER(current_player)->entry_status = 0xFFu;
		OBJECT(current_object)->handler = player_update_ADDRESS;
	}
	if (stage_exit_started != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x1Du)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x15u && stage_exit_started != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if ((OBJECT(current_object)->flags & 0x2000000u) == 0u && (current_game_mode == 0x13u || (current_game_mode == 0x12u && event_12_fast_scroll != 0u)))
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x19u && rope_bridge_collapsing != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (event_1c_ride_phase != 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0xDu && (OBJECT(current_object)->flags & 0x2000000u) == 0u)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
	if (current_game_mode == 0x1Au)
	{
		PLAYER(current_player)->entry_status = 0x82u;
		OBJECT(current_object)->handler = player_place_ADDRESS;
	}
}

/* Unless the player is in state 0x37: in a game, pay_and_join with cost (the continue); with no game in
   progress it counts as joined at once. Hands back -1 when joined, else 0. */
uint32_t try_continue(uint8_t cost)
{
	uint32_t joined = 0;

	joined = 0u;
	if (PLAYER(current_player)->state == 0x37u)
		return joined;
	if (game_in_progress != 0u)
		return pay_and_join(cost, joined);
	joined = 0xFFFFFFFFu;
	return joined;
}

/* As try_continue with the cost of a start (credits_per_start). */
uint32_t try_join(void)
{
	uint8_t cost = 0;
	uint32_t joined = 0;

	joined = 0u;
	if (PLAYER(current_player)->state == 0x37u)
		return joined;
	if (game_in_progress != 0u)
	{
		cost = (uint8_t)credits_per_start;
		return pay_and_join(cost, joined);
	}
	joined = 0xFFFFFFFFu;
	return joined;
}

/* Joins the player when it has cost credits (none on free play) and its start button (trackball button
   with setting_no_start_buttons) is newly pressed, from the shared or its own credit block by
   cabinet_config: credits taken, cabinet event written, io_port_d bit cleared on 2-player setups,
   credits saved, -1 handed back; else joined. */
uint32_t pay_and_join(uint8_t cost, uint32_t joined)
{
	uint8_t cabinet_event = 0;
	uint32_t bit = 0, credit_block = 0, start_bit = 0;
	uint32_t port_d = 0;

	if (free_play != 0u)
		cost = 0u;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)cost)
			return joined;
		credit_block = p1_coins_ADDRESS;
		switch (OBJECT(current_object)->player)
		{
		case 0:
		case 3:
			start_bit = 4u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x10u;
			break;
		case 1:
			start_bit = 5u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x18u;
			break;
		case 2:
			start_bit = 7u;
			if (setting_no_start_buttons != 0u)
				start_bit = 0x12u;
			break;
		default:
			invalid_jump(0xD50Cu + OBJECT(current_object)->player * 2u);
		}
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
			return joined;
		break;
	case 2:
		if (OBJECT(current_object)->player != 1u)
		{
			if (OBJECT(current_object)->player != 0u || p1_credits < (uint8_t)cost)
				return joined;
			credit_block = p1_coins_ADDRESS;
			bit = 0x10u;
			if (setting_no_start_buttons == 0u)
				bit = 4u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		else
		{
			if (p2_credits < (uint8_t)cost)
				return joined;
			credit_block = p2_coins_ADDRESS;
			bit = 0x18u;
			if (setting_no_start_buttons == 0u)
				bit = 5u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		break;
	case 3:
		if (OBJECT(current_object)->player != 2u)
		{
			if (OBJECT(current_object)->player != 1u)
			{
				if (OBJECT(current_object)->player != 0u || p1_credits < (uint8_t)cost)
					return joined;
				credit_block = p1_coins_ADDRESS;
				bit = 0x10u;
				if (setting_no_start_buttons == 0u)
					bit = 4u;
				if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
					return joined;
			}
			else
			{
				if (p2_credits < (uint8_t)cost)
					return joined;
				credit_block = p2_coins_ADDRESS;
				bit = 0x18u;
				if (setting_no_start_buttons == 0u)
					bit = 5u;
				if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
					return joined;
			}
		}
		else
		{
			if (p3_credits < (uint8_t)cost)
				return joined;
			credit_block = p3_coins_ADDRESS;
			bit = 0x12u;
			if (setting_no_start_buttons == 0u)
				bit = 7u;
			if ((inputs_pressed & shl32(1u, (int)(bit))) == 0u)
				return joined;
		}
		break;
	default:
		invalid_jump(0xD4E6u + cabinet_config * 3u);
	}
	CREDIT_BLOCK(credit_block)->credits -= cost;
	CREDIT_BLOCK(credit_block)->bonus_coins = 0u;
	cabinet_event = 1u;
	if (OBJECT(current_object)->player != 0u)
	{
		cabinet_event = 3u;
		if (OBJECT(current_object)->player != 1u)
			cabinet_event = 5u;
	}
	mem_write8(cabinet_events, cabinet_event);
	if (PLAYER(current_player)->state == 0x23u)
		mem_write8(cabinet_events, cabinet_event);
	if ((cabinet_config & 1u) != 0u)
	{
		save_credits();
		joined = 0xFFFFFFFFu;
		return joined;
	}
	bit = 2u;
	if (OBJECT(current_object)->player != 0u)
		bit = 3u;
	port_d = mem_read32(io_port_d);
	mem_write32(io_port_d, port_d & ~shl32(1u, (int)(bit)));
	save_credits();
	joined = 0xFFFFFFFFu;
	return joined;
}

/* Forces the states of the current game mode and events on a playing player: 0x19 (mode 0xD), 0x1E (1),
   0x2B (0x18), 0x30 (0x19, bridge collapsing), 0x31 (0x1A), the ride of mode 0x1C (spawn_player_ride,
   then 0x35), 0x21 (0x17, 0x1D, 4), 0x26 (players_pulled), 0x38 (mode 3 in stage 0xB, y up to 0x500),
   0x39 (0x20). */
uint32_t check_scene_states(uint32_t object)
{
	if (PLAYER(current_player)->state == 0xBu || PLAYER(current_player)->state == 0x23u || PLAYER(current_player)->state == 0x24u || PLAYER(current_player)->state == 0x16u || PLAYER(current_player)->state == 0x2Eu || PLAYER(current_player)->state == 0x37u)
		return object;
	if (stage == 5u && OBJECT(current_object)->y_int >= 0x400)
		scroll_mode = 7u;
	if (current_game_mode == 0xDu)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x19u;
	}
	if (current_game_mode == 1u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x1Eu;
	}
	if (current_game_mode == 0x18u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x2Bu;
	}
	if (current_game_mode == 0x19u && PLAYER(current_player)->state != 0x30u && rope_bridge_collapsing != 0u)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x30u;
	}
	if (current_game_mode == 0x1Au && PLAYER(current_player)->state != 0x32u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x31u;
	}
	if (current_game_mode == 0x1Cu && event_1c_ride_phase != 0u)
	{
		if (event_1c_riding == 0u)
			object = spawn_player_ride(object);
		else if (PLAYER(current_player)->state != 6u && PLAYER(current_player)->state != 9u && PLAYER(current_player)->state != 0xAu)
		{
			PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
			PLAYER(current_player)->state = 0x35u;
			if (PLAYER(current_player)->previous_state != 0x35u)
				PLAYER(current_player)->substate = 0u;
		}
	}
	if (current_game_mode == 0x17u || current_game_mode == 0x1Du || current_game_mode == 4u)
	{
		PLAYER(current_player)->state = 0x21u;
		PLAYER(current_player)->goal_step = 2u;
		if (stage == 4u)
			PLAYER(current_player)->goal_step = 4u;
	}
	if (PLAYER(current_player)->state != 0x26u && PLAYER(current_player)->state != 6u && PLAYER(current_player)->state != 8u && PLAYER(current_player)->state != 9u && players_pulled != 0u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x26u;
		PLAYER(current_player)->substate = 0u;
	}
	if (current_game_mode == 3u && stage == 0xBu && (uint8_t)OBJECT(current_object)->health != 0u && PLAYER(current_player)->state != 9u && (uint16_t)OBJECT(current_object)->floor_z_int != 0x800u && PLAYER(current_player)->ending_done == 0u && OBJECT(current_object)->y_int <= 0x500)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x38u;
	}
	if (current_game_mode == 0x20u)
	{
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0x39u;
	}
	return object;
}

/* Sets the player's sprite priority bits for its fall into a pit from fall_sprite_priorities: the entry
   of the block it fell from, by ledge_side (ledge_direction when 4), or by the half of the block it is
   over when the block it left is of type 0x11 or 0x1F, or its current one 0x1D or 0x1E; a negative entry
   leaves them. */
void set_fall_sprite_priority(void)
{
	uint8_t priority = 0;
	uint32_t record = 0, side = 0;

	record = floor_blocks + 2u + OBJECT(current_object)->previous_floor_block;
	if (FLOOR_BLOCK(record)->type_index == 0x1Fu || FLOOR_BLOCK(record)->type_index == 0x11u)
	{
		side = 3u;
		if ((int32_t)((((uint32_t)FLOOR_BLOCK(record)->x_max - FLOOR_BLOCK(record)->x_min) >> 1) + FLOOR_BLOCK(record)->x_min) <= OBJECT(current_object)->x)
			side = 1u;
		record = fall_sprite_priorities_ADDRESS + FLOOR_BLOCK(record)->fall_priorities * 4u;
	}
	else
	{
		record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(record)->type_index == 0x1Eu || FLOOR_BLOCK(record)->type_index == 0x1Du)
		{
			side = 3u;
			if ((int32_t)((((uint32_t)FLOOR_BLOCK(record)->x_max - FLOOR_BLOCK(record)->x_min) >> 1) + FLOOR_BLOCK(record)->x_min) <= OBJECT(current_object)->x)
				side = 1u;
			record = fall_sprite_priorities_ADDRESS + FLOOR_BLOCK(record)->fall_priorities * 4u;
			priority = (uint8_t)FALL_SPRITE_PRIORITY_SIDES(record)[side];
			if ((int8_t)priority >= 0)
			{
				OBJECT(current_object)->sprite_palette &= 0x9FFFu;
				OBJECT(current_object)->sprite_palette |= (uint32_t)priority << 13;
			}
			return;
		}
		record = fall_sprite_priorities_ADDRESS + mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x14u) * 4u;
		side = (uint8_t)PLAYER(current_player)->ledge_side;
		if (side == 4u)
			side = (uint8_t)PLAYER(current_player)->ledge_direction;
	}
	priority = (uint8_t)FALL_SPRITE_PRIORITY_SIDES(record)[side];
	if ((int8_t)priority >= 0)
	{
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= (uint32_t)priority << 13;
	}
}

/* On floor type 2 (a wall to climb) a player placed there starts climbing: its character's climb
   animation, z raised by the wall's rise (its floor_types record) times how far it is into the block,
   climb_remaining and climb_base_z set; state 0xA, substate 3. */
void start_climb_at_spawn(uint32_t climbed_height)
{
	uint32_t attribute_record = 0, block = 0, climb_animation = 0, distance = 0;

	if (OBJECT(current_object)->floor_type != 2u)
		return;
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	if (FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->axis != 0u)
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	PLAYER(current_player)->climb_remaining = FLOOR_ATTRIBUTE_RECORD(attribute_record)->height;
	climbed_height = (climbed_height & ~0xFFFFu) | FLOOR_ATTRIBUTE_RECORD(attribute_record)->rise;
	block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis == 0u)
		distance = (uint32_t)OBJECT(current_object)->x - FLOOR_BLOCK(block)->x_min;
	else
		distance = (uint32_t)FLOOR_BLOCK(block)->y_max - OBJECT(current_object)->y;
	climbed_height = climbed_height * ((int32_t)distance >> 8);
	OBJECT(current_object)->z -= climbed_height;
	PLAYER(current_player)->climb_remaining -= climbed_height >> 16;
	PLAYER(current_player)->climb_base_z_int = FLOOR_BLOCK(block)->height;
	PLAYER(current_player)->climb_base_z_fraction = 0u;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	PLAYER(current_player)->state = 0xAu;
}

/* On floor type 2 (a wall to climb) starts the riding player climbing: its character's climb animation,
   x + 0x41000, climb_base_z from the floor block, climb_remaining from its floor attribute's height less
   its height above climb_base_z; state 0xA, substate 3. */
void start_climb(void)
{
	uint32_t animation = 0, ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->floor_type != 2u)
		return;
	animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	if (FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->axis != 0u)
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->x += 0x41000u;
	PLAYER(current_player)->climb_base_z_int = mem_read16(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x10u);
	PLAYER(current_player)->climb_base_z_fraction = 0u;
	PLAYER(current_player)->climb_remaining = FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->height;
	PLAYER(current_player)->climb_remaining -= PLAYER(current_player)->climb_base_z_int - OBJECT(current_object)->z_int;
	project_object(&ignored, &ignored_2);
	PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
	PLAYER(current_player)->screen_x_fraction = 0u;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	PLAYER(current_player)->state = 0xAu;
}

/* Unused: polar_to_xy(heading_snapped, speed_int) plus the push, each >> 4, gives speed (length << 12, 0
   when negative, at most 0x5000000) and heading; the push is then cleared. It reads push_x and push_y as
   32-bit words, each taking the halfword after it, and the clear also zeroes animation_speed's low half. */
void add_push_to_player_motion(void)
{
	uint16_t dy = 0;
	uint32_t angle = 0, ignored = 0, ignored_2 = 0, length = 0, sum_dx = 0, sum_dy = 0;

	if (PLAYER(current_player)->push_xy == 0u)
	{
		PLAYER(current_player)->push_xy = 0u;
		PLAYER(current_player)->push_y = 0;
		PLAYER(current_player)->animation_speed &= 0xFFFF0000u;
		return;
	}
	length = (uint16_t)OBJECT(current_object)->speed_int;
	angle = PLAYER(current_player)->heading_snapped;
	polar_to_xy(&ignored, &angle, &length);
	push_motion_x = angle;
	push_motion_y = length;
	push_motion_x += PLAYER(current_player)->push_xy;
	push_motion_y += ((uint16_t)PLAYER(current_player)->push_y | ((uint32_t)((uint32_t)PLAYER(current_player)->animation_speed & 0xFFFFu) << 16u));
	sum_dx = (int32_t)push_motion_x >> 4;
	push_motion_x = sum_dx;
	sum_dy = (int32_t)push_motion_y >> 4;
	push_motion_y = sum_dy;
	sum_dx = sum_dx * sum_dx + sum_dy * sum_dy;
	square_root(&sum_dx, &ignored);
	OBJECT(current_object)->speed = (uint32_t)(uint16_t)sum_dx << 12;
	if (OBJECT(current_object)->speed_int < 0)
		OBJECT(current_object)->speed = 0;
	if (OBJECT(current_object)->speed > 0x5000000)
		OBJECT(current_object)->speed = 0x5000000;
	sum_dx = push_motion_x;
	dy = (uint16_t)push_motion_y;
	angle_of_vector(dy, &sum_dx, &ignored_2);
	PLAYER(current_player)->heading = sum_dx;
	PLAYER(current_player)->push_xy = 0u;
	PLAYER(current_player)->push_y = 0;
	PLAYER(current_player)->animation_speed &= 0xFFFF0000u;
}

/* Sets push_x and push_y from the floor: cleared, then the floor_types record's first two words added on
   floor types 4, 0xB, 5 (not while hopping) and 7 (with an attribute), not while invulnerable. With
   flags bit 31 set it leaves the push alone. */
void apply_floor_push(void)
{
	uint32_t attribute_record = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		PLAYER(current_player)->push_xy = 0u;
		if (PLAYER(current_player)->invulnerable_timer < 0 && ((OBJECT(current_object)->floor_type == 7u && OBJECT(current_object)->floor_attribute != 0u) || OBJECT(current_object)->floor_type == 0xBu || OBJECT(current_object)->floor_type == 4u || (OBJECT(current_object)->floor_type == 5u && PLAYER(current_player)->state != 0x17u)))
		{
			attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
			PLAYER(current_player)->push_x += FLOOR_ATTRIBUTE_RECORD(attribute_record)->push_x;
			PLAYER(current_player)->push_y += FLOOR_ATTRIBUTE_RECORD(attribute_record)->push_y;
		}
	}
}

/* For Mighty (kind 2) slipping off a ledge (player_state_dropping): one piece with the frame at its
   animation + 0x3A, at the player's position, that waits 0x10 frames before it falls
   (slip_piece_update). */
uint32_t spawn_slip_piece(uint32_t piece)
{
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0;

	allocate_object(&free_count, &piece);
	if (free_count == 0u)
		return piece;
	OBJECT(piece)->sprite_flags = 0x1004u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(piece)->sprite_flags |= 0x40u;
	OBJECT(piece)->sprite_frame = EDGE_HANG_ANIMATIONS(OBJECT(current_object)->animation)->slip_piece_frame;
	OBJECT(piece)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(piece)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
	OBJECT(piece)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(piece)->flags = 0u;
	OBJECT(piece)->handler = slip_piece_update_ADDRESS;
	OBJECT(piece)->speed_x_fraction = 0x10u;
	OBJECT(piece)->speed_z = 0;
	OBJECT(piece)->direction = PLAYER(current_player)->ledge_side;
	OBJECT(piece)->x = OBJECT(current_object)->x;
	OBJECT(piece)->y = OBJECT(current_object)->y;
	OBJECT(piece)->z = OBJECT(current_object)->z;
	current_object = piece;
	project_object(&ignored, &ignored_2);
	current_object = object_cursor;
	return piece;
}

/* Mighty's slip piece: deleted off screen; waits while speed_x_fraction (a timer here) counts down from
   0x10, then falls (gravity 0x6000); drawn at its base when its direction (the ledge_side) is 2 or more,
   else in bucket 0xFF. */
void slip_piece_update(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, off_screen = 0;
	uint16_t speed_x_fraction = 0;

	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	speed_x_fraction = OBJECT(current_object)->speed_x_fraction;
	OBJECT(current_object)->speed_x_fraction = speed_x_fraction - 1u;
	if ((int16_t)speed_x_fraction <= 1)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored, &ignored_2);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		ignored_2 = queue_sprite_at_base(ignored_2);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* For Ray (kind 1) slipping off a ledge: two pieces (the player's animation + 0x3A) thrown up (speed_z
   -0x80000) and apart at 0x8000 in x (ledge sides 0, 2 and 4) or y (1 and 3), run by slip_pieces_update;
   needs 2 free objects. */
void spawn_slip_pieces(uint32_t piece)
{
	uint32_t ignored = 0;
	uint16_t pieces_left = 0;

	if (free_object_stack < 2)
		return;
	slip_pieces_left = 2;
	slip_piece_speed = 0x8000u;
	do
	{
		allocate_object(&ignored, &piece);
		OBJECT(piece)->sprite_flags = 0x1000u;
		OBJECT(piece)->animation = OBJECT(current_object)->animation + 0x3Au;
		OBJECT(piece)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(piece)->sprite_offset_x = 0;
		OBJECT(piece)->sprite_offset_y = 0;
		OBJECT(piece)->flags = 0u;
		OBJECT(piece)->handler = slip_pieces_update_ADDRESS;
		OBJECT(piece)->speed_z = -0x80000;
		OBJECT(piece)->direction = PLAYER(current_player)->ledge_side;
		OBJECT(piece)->animation_frame = 0u;
		OBJECT(piece)->animation_speed = 4u;
		OBJECT(piece)->animation_timer = 0;
		OBJECT(piece)->x = OBJECT(current_object)->x;
		OBJECT(piece)->y = OBJECT(current_object)->y;
		OBJECT(piece)->z = OBJECT(current_object)->z;
		switch (OBJECT(piece)->direction)
		{
		case 0:
		case 2:
		case 4:
			OBJECT(piece)->speed_x = slip_piece_speed;
			OBJECT(piece)->speed_y = 0;
			break;
		case 1:
		case 3:
			OBJECT(piece)->speed_y = slip_piece_speed;
			OBJECT(piece)->speed_x = 0;
			break;
		default:
			invalid_jump(0xDC37u + OBJECT(piece)->direction * 2u);
		}
		slip_piece_speed = 0u - slip_piece_speed;
		pieces_left = slip_pieces_left;
		slip_pieces_left = pieces_left - 1u;
	} while ((int16_t)pieces_left > 1);
}

/* Ray's two slip pieces: each moves by speed_x and speed_y, falls (gravity 0x6000), animates and is
   deleted off screen (margins 0x20 and 0x50); drawn at its base when its direction (the ledge_side) is 2
   or more, else in bucket 0xFF. */
void slip_pieces_update(void)
{
	uint8_t bucket = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	project_object(&ignored, &ignored_2);
	margin_x = 0x20u;
	margin_y = 0x50u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&ignored_2, &ignored_3, &ignored_4);
	if ((int16_t)OBJECT(current_object)->direction >= 2)
	{
		ignored_2 = queue_sprite_at_base(ignored_2);
		return;
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* The player's autopilot: takes a new path from path_request or all_players_path_request
   (follow_player_path_start) or runs the current one (follow_player_path_run), and hands back the speed
   to aim for: the path's, or target_speed with no path. */
uint32_t follow_player_path(uint32_t target_speed)
{
	if (PLAYER(current_player)->path_request != 0u)
	{
		PLAYER(current_player)->path = PLAYER(current_player)->path_request;
		PLAYER(current_player)->path_request = 0u;
		return follow_player_path_start();
	}
	if (all_players_path_request == 0u)
	{
		if (PLAYER(current_player)->path != 0u)
			return follow_player_path_run();
		return target_speed;
	}
	if (PLAYER(current_player)->path == all_players_path_request)
		return follow_player_path_run();
	PLAYER(current_player)->path = all_players_path_request;
	return follow_player_path_start();
}

/* Starts path on the player: path_pointer from player_path_offsets[path + player], speed 0, z on its
   floor, state 1, flags bits 6 and 13 off and bit 0 on (so the first aim sets heading too); then its
   first record. */
uint32_t follow_player_path_start(void)
{
	uint32_t path = 0;

	path = PLAYER(current_player)->path;
	PLAYER(current_player)->path_pointer = player_path_offsets_ADDRESS + player_path_offsets[(uint8_t)(path + OBJECT(current_object)->player)];
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->speed_z = 0;
	PLAYER(current_player)->state_timer = 0;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->follow_player_path.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->state = 1u;
	OBJECT(current_object)->flags |= 1u;
	return follow_player_path_load_record();
}

/* Runs the current path: during a wait the speed to aim for is 0 (speed stopped below 0x20) until
   path_wait runs out; else the player heads for the record's point until within 8 in x and y (at once
   with the record's flag bit 1), then the next record. */
uint32_t follow_player_path_run(void)
{
	uint16_t x_offset = 0;
	uint32_t point = 0, target_speed = 0;
	uint16_t path_wait = 0;

	all_players_path_request = 0u;
	if ((uint16_t)PLAYER(current_player)->path_wait == 0u)
	{
		point = PLAYER(current_player)->path_pointer;
		if ((PLAYER_PATH_STEP(point)->flags & 2u) == 0u)
		{
			point += 2u;
			x_offset = (uint16_t)(OBJECT(current_object)->x_int - PLAYER_PATH_STEP(point - 0x2u)->x_or_wait + 8u);
			point += 2u;
			if (x_offset >= 0x11u || (uint16_t)(OBJECT(current_object)->y_int - PLAYER_PATH_STEP(point - 0x4u)->y + 8u) >= 0x11u)
				return follow_player_path_aim_at_point();
		}
		return follow_player_path_next_record();
	}
	path_wait = PLAYER(current_player)->path_wait;
	PLAYER(current_player)->path_wait = path_wait - 1u;
	if ((int16_t)path_wait <= 1)
		return follow_player_path_next_record();
	target_speed = 0u;
	if (OBJECT(current_object)->speed_int < 0x20)
		OBJECT(current_object)->speed = 0;
	return target_speed;
}

/* Moves path_pointer to the next 8-byte record unless its speed word is negative, which ends the path
   (path = 0). */
uint32_t follow_player_path_next_record(void)
{
	if (PLAYER_PATH_STEP(PLAYER(current_player)->path_pointer)[1].speed >= 0)
	{
		PLAYER(current_player)->path_pointer += 8u;
		return follow_player_path_load_record();
	}
	PLAYER(current_player)->path = 0u;
	return follow_player_path_target_speed();
}

/* Starts the path record at path_pointer: path_speed from it; its flag bit 0 makes the player jump
   (state 4); a speed of 0 makes it wait path_wait (the record's second word) frames, else it heads for
   the record's point (follow_player_path_aim_at_point). */
uint32_t follow_player_path_load_record(void)
{
	PLAYER(current_player)->path_speed = PLAYER_PATH_STEP(PLAYER(current_player)->path_pointer)->speed;
	if ((PLAYER_PATH_STEP(PLAYER(current_player)->path_pointer)->flags & 1u) != 0u)
	{
		PLAYER(current_player)->state = 4u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->ledge_side = -1;
	}
	if (PLAYER(current_player)->path_speed != 0u)
	{
		PLAYER(current_player)->path_wait = 0;
		return follow_player_path_aim_at_point();
	}
	PLAYER(current_player)->path_wait = PLAYER_PATH_STEP(PLAYER(current_player)->path_pointer)->x_or_wait;
	return follow_player_path_target_speed();
}

/* Turns target_heading and facing_angle toward the point of the current path record (its x and y words)
   and, on a path's first record (flags bit 0, cleared here), heading too; then
   follow_player_path_target_speed. */
uint32_t follow_player_path_aim_at_point(void)
{
	uint16_t dy = 0;
	uint32_t ignored = 0, path_heading = 0, point = 0;
	uint32_t flags = 0;

	point = PLAYER(current_player)->path_pointer + 2u;
	path_heading = (uint16_t)(PLAYER_PATH_STEP(point - 0x2u)->x_or_wait - OBJECT(current_object)->x_int);
	dy = (uint16_t)(PLAYER_PATH_STEP(point - 0x2u)->y - OBJECT(current_object)->y_int);
	angle_of_vector(dy, &path_heading, &ignored);
	path_heading = (path_heading - 0x80u) & 0x3FFu;
	PLAYER(current_player)->target_heading = path_heading;
	PLAYER(current_player)->facing_angle = path_heading;
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~1u;
	if ((flags & 1u) == 0u)
		return follow_player_path_target_speed();
	PLAYER(current_player)->heading = path_heading;
	return follow_player_path_target_speed();
}

/* Hands back path_speed << 16 as the speed to aim for; a slow path (0x20 or less) gives 0, and stops the
   player, once its speed is under 0x20. */
uint32_t follow_player_path_target_speed(void)
{
	uint32_t target_speed = 0;

	target_speed = (uint32_t)PLAYER(current_player)->path_speed << 16;
	if ((int32_t)target_speed <= 0x200000 && OBJECT(current_object)->speed_int < 0x20)
	{
		target_speed = 0u;
		OBJECT(current_object)->speed = 0;
	}
	return target_speed;
}

/* What touching an object does to the player, by the kind of the object in touched_by: rings (3 and 42)
   give rings and health, hazards hurt (state 0xD) or tumble it, solid kinds stop or bounce it, others
   launch, hold, catch or sink it. Nothing on a path (but 0x14-0x16), untouched, or when the object has
   flags bit 9. */
void check_player_contacts(uint32_t *p_health_limit, uint32_t *p_cursor)
{
	uint8_t new_substate = 0, sound_id = 0;
	uint16_t dy = 0, held_frame = 0, saved_sides = 0;
	uint32_t carried_x = 0, carried_y = 0, distance = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, joined_health_setting = 0, move_angle = 0, object = 0, sides = 0;
	uint32_t bounce_direction_halfword = 0, flags = 0, flags_2 = 0;

	if (PLAYER(current_player)->path_request != 0u || all_players_path_request != 0u || (PLAYER(current_player)->path != 0u && (PLAYER(current_player)->path < 0x14 || PLAYER(current_player)->path > 0x16)) || (OBJECT(current_object)->flags & 4u) == 0u)
	{
		check_player_contacts_clear_state();
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x200u) != 0u)
	{
		check_player_contacts_clear_state();
		return;
	}
	if (OBJECT(object)->kind != 8u)
		OBJECT(current_object)->flags &= ~0x40000u;
	switch (OBJECT(object)->kind)
	{
	case 0:
	case 1:
	case 2:
		return;
	case 3:
	case 42:
		PLAYER(current_player)->stage_rings++;
		PLAYER(current_player)->rings++;
		PLAYER(current_player)->flags |= 0x40u;
		switch (health_setting)
		{
		case 0:
		case 4:
			OBJECT(current_object)->health++;
			joined_health_setting = PLAYER(current_player)->joined_health_setting;
			*p_health_limit = (*p_health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
			if (OBJECT(current_object)->health <= (int8_t)*p_health_limit)
			{
				check_player_contacts_clear_state();
				return;
			}
			OBJECT(current_object)->health = *p_health_limit;
			check_player_contacts_clear_state();
			return;
		case 1:
		case 2:
			*p_health_limit = (*p_health_limit & ~0xFFu) | 4u;
			break;
		case 3:
			*p_health_limit = (*p_health_limit & ~0xFFu) | 8u;
			break;
		default:
			invalid_jump(0xE578u + health_setting * 2u);
		}
		if (players_in_play >= 2)
			*p_health_limit = (*p_health_limit & ~0xFFFFu) | ((uint16_t)*p_health_limit >> 1);
		PLAYER(current_player)->rings_to_health++;
		if (PLAYER(current_player)->rings_to_health < (int8_t)*p_health_limit)
		{
			joined_health_setting = PLAYER(current_player)->joined_health_setting;
			*p_health_limit = (*p_health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
			if (OBJECT(current_object)->health <= (int8_t)*p_health_limit)
			{
				check_player_contacts_clear_state();
				return;
			}
			OBJECT(current_object)->health = *p_health_limit;
			check_player_contacts_clear_state();
			return;
		}
		OBJECT(current_object)->health++;
		PLAYER(current_player)->rings_to_health = 0;
		joined_health_setting = PLAYER(current_player)->joined_health_setting;
		*p_health_limit = (*p_health_limit & ~0xFFu) | (uint8_t)max_health_by_setting[(uint16_t)(joined_health_setting * 2u)];
		if (OBJECT(current_object)->health <= (int8_t)*p_health_limit)
		{
			check_player_contacts_clear_state();
			return;
		}
		OBJECT(current_object)->health = *p_health_limit;
		check_player_contacts_clear_state();
		return;
	case 4:
	case 15:
	case 16:
	case 22:
		check_player_contacts_hurt_unless_invulnerable(object);
		return;
	case 5:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		*p_health_limit = check_player_contacts_solid(*p_health_limit, object);
		return;
	case 6:
		new_substate = 0u;
		*p_cursor = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (OBJECT(*p_cursor)->catapult.state != 1u)
		{
			return;
		}
		PLAYER(current_player)->state = 0xFu;
		PLAYER(current_player)->substate = new_substate;
		return;
	case 7:
		new_substate = 1u;
		*p_cursor = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (OBJECT(*p_cursor)->catapult.state != 1u)
		{
			return;
		}
		PLAYER(current_player)->state = 0xFu;
		PLAYER(current_player)->substate = new_substate;
		return;
	case 8:
	case 10:
		if ((OBJECT(current_object)->flags & 0x200000u) != 0u || PLAYER(current_player)->state != 4u)
			return;
		PLAYER(current_player)->holder = OBJECT(current_object)->touched_by;
		PLAYER(current_player)->state = 0x15u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		held_frame = 0x1B10u;
		if (OBJECT(current_object)->kind != 0u)
		{
			held_frame = 0x5230u;
			if (OBJECT(current_object)->kind != 1u)
				held_frame = 0x3678u;
		}
		OBJECT(current_object)->sprite_frame = held_frame;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->check_player.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		return;
	case 9:
	case 13:
	case 24:
	case 40:
		check_player_contacts_hurt(object);
		return;
	case 11:
		new_substate = 1u;
		check_player_contacts_attack_or_hurt(new_substate, object);
		return;
	case 12:
	case 32:
	case 33:
		*p_health_limit = check_player_contacts_solid(*p_health_limit, object);
		return;
	case 14:
		new_substate = 7u;
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 17:
		if (PLAYER(current_player)->state == 4u)
		{
			check_player_contacts_attack(object);
			return;
		}
		if (OBJECT(current_object)->speed_int < 0x200)
		{
			*p_health_limit = check_player_contacts_solid(*p_health_limit, object);
			return;
		}
		PLAYER(current_player)->state = 0x1Du;
		PLAYER(current_player)->substate = 0u;
		return;
	case 18:
		if (PLAYER(current_player)->state == 0x2Fu)
			return;
		PLAYER(current_player)->state = 0x2Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 19:
		PLAYER(current_player)->state = 0x1Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 20:
		if ((uint16_t)object == PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->state = 0x20u;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->last_hazard = object;
		PLAYER(current_player)->hurt_contact_delay = 2;
		PLAYER(current_player)->state = 0x20u;
		return;
	case 21:
		new_substate = 7u;
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 23:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		if ((uint16_t)object == PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->state = 0x20u;
			return;
		}
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->last_hazard = object;
		PLAYER(current_player)->hurt_contact_delay = 2;
		PLAYER(current_player)->state = 0x20u;
		return;
	case 25:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		contact_saved_x = OBJECT(current_object)->x;
		contact_saved_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		side_of_box(object, p_health_limit, &sides);
		contact_box_side = sides;
		if ((uint16_t)sides != 8u && (uint16_t)sides != 0u && PLAYER(current_player)->state == 4u)
		{
			check_player_contacts_attack(object);
			return;
		}
		OBJECT(current_object)->flags |= 0x1000u;
		saved_sides = contact_box_side;
		if (saved_sides != 0u)
		{
			if (saved_sides != 8u)
			{
				if (OBJECT(current_object)->speed_int >= 0x500)
				{
					bounce_direction_halfword = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
					PLAYER(current_player)->bounce_direction = (uint8_t)bounce_direction_halfword;
					PLAYER(current_player)->bounce_speed_z = (PLAYER(current_player)->bounce_speed_z & 0xFFFFFF00u) | ((bounce_direction_halfword >> 8u) & 0xFFu);
					PLAYER(current_player)->state = 0xEu;
					PLAYER(current_player)->substate = 0u;
					return;
				}
				OBJECT(current_object)->speed = 0;
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0;
				PLAYER(current_player)->control_lock = 0u;
				OBJECT(current_object)->flags |= 0x800000u;
				PLAYER(current_player)->free_turn_timer = 3u;
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->state_timer = -1;
				OBJECT(current_object)->flags &= ~0x2000u;
				OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->check_player.palette_list)->normal_palette;
				OBJECT(current_object)->flags &= ~0x40u;
				OBJECT(current_object)->flags &= ~0x100000u;
				OBJECT(current_object)->flags &= ~0x200000u;
				return;
			}
			OBJECT(current_object)->x = contact_saved_x;
			OBJECT(current_object)->y = contact_saved_y;
		}
		carried_x = OBJECT(object)->speed_x;
		carried_y = OBJECT(object)->speed_y * 2u;
		unproject_point(&carried_x, &carried_y);
		OBJECT(current_object)->x += carried_x;
		OBJECT(current_object)->x += carried_x;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		OBJECT(current_object)->y += carried_y;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->state_timer = -1;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->check_player.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x100000u;
		OBJECT(current_object)->flags &= ~0x200000u;
		return;
	case 26:
		*p_cursor = LINKED(current_object, OBJECT(object)->parent);
		if (OBJECT(*p_cursor)->handle_rod.angle != 0x300u)
		{
			return;
		}
		PLAYER(current_player)->state = 0x27u;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->climb_remaining = OBJECT(object)->spawn_arg_2;
		return;
	case 27:
		OBJECT(current_object)->flags |= 0x20000u;
		PLAYER(current_player)->flags &= ~2u;
		PLAYER(current_player)->flags &= ~0x80u;
		if (PLAYER(current_player)->state == 7u || PLAYER(current_player)->state == 8u)
			return;
		if ((uint32_t)OBJECT(current_object)->speed_x == 0u && (uint32_t)OBJECT(current_object)->speed_y == 0u)
		{
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->direction = OBJECT(object)->direction;
		}
		if (PLAYER(current_player)->state == 0xCu && (uint16_t)OBJECT(current_object)->speed_int == 0u)
		{
			OBJECT(current_object)->speed = 0x1000000;
			set_speed_from_direction();
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 7u;
		PLAYER(current_player)->substate = 1u;
		OBJECT(current_object)->floor_block = -1;
		OBJECT(current_object)->floor_z = 0x8000000;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x100u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->check_player.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->heading = OBJECT(object)->direction;
		OBJECT(current_object)->speed_x = OBJECT(current_object)->speed_x * 2u;
		OBJECT(current_object)->speed_y = OBJECT(current_object)->speed_y * 2u;
		PLAYER(current_player)->ledge_side = OBJECT(object)->direction >> 8;
		*p_health_limit = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
		*p_health_limit = (*p_health_limit & ~0xFFFFu) | (uint16_t)(*p_health_limit - view_y);
		PLAYER(current_player)->fall_draw_depth = *p_health_limit;
		return;
	case 28:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 0x80000000u;
		if ((flags & 0x80000000u) == 0u)
		{
			sound_id = 0x36u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x46u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x56u;
			}
			play_sound(sound_id, &ignored, &ignored_2);
		}
		distance = (uint16_t)OBJECT(current_object)->touch_distance;
		square_root(&distance, &ignored);
		OBJECT(current_object)->touch_distance = distance;
		if (OBJECT(current_object)->touch_distance <= 8)
		{
			flags_2 = PLAYER(current_player)->flags;
			PLAYER(current_player)->flags = flags_2 | 4u;
			if ((flags_2 & 4u) == 0u)
			{
				PLAYER(current_player)->sink_depth = 0;
				PLAYER(current_player)->mishap_flag = 0xFFu;
				sound_id = 0x35u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_id = 0x31u;
					if (OBJECT(current_object)->kind != 1u)
						sound_id = 0x3Du;
				}
				play_sound(sound_id, &ignored, &ignored_2);
				*p_cursor = spawn_sink_swirl(*p_cursor);
			}
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		move_angle = (uint16_t)(OBJECT(object)->x_int - OBJECT(current_object)->x_int);
		dy = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		angle_of_vector(dy, &move_angle, &ignored_3);
		move_angle = (uint16_t)(move_angle - 0x80u) & 0x3FFu;
		PLAYER(current_player)->push_x = (int32_t)(sine_table[move_angle] * 0x6000u) >> 16;
		*p_health_limit = (int32_t)(cosine_table[move_angle] * 0x6000u) >> 16;
		PLAYER(current_player)->push_y = 0u - *p_health_limit;
		return;
	case 29:
		PLAYER(current_player)->flags |= 2u;
		OBJECT(current_object)->flags &= ~0x20000u;
		return;
	case 30:
		if ((PLAYER(current_player)->flags & 4u) != 0u)
			return;
		PLAYER(current_player)->state = 0x28u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 31:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		if (PLAYER(current_player)->state == 0x29u || PLAYER(current_player)->state == 5u || PLAYER(current_player)->state == 7u || PLAYER(current_player)->state == 0xDu || OBJECT(current_object)->floor_type == 0xAu || OBJECT(current_object)->floor_type == 0xBu)
			return;
		PLAYER(current_player)->state = 0x29u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 34:
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		PLAYER(current_player)->state = 0x1Fu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 35:
		PLAYER(current_player)->state = 0x2Cu;
		PLAYER(current_player)->substate = 0u;
		return;
	case 36:
		new_substate = 0u;
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return;
		}
		if ((uint16_t)object != PLAYER(current_player)->last_hazard)
		{
			PLAYER(current_player)->substate = new_substate;
			PLAYER(current_player)->last_hazard = object;
			PLAYER(current_player)->hurt_contact_delay = 2;
		}
		PLAYER(current_player)->state = 0xDu;
		return;
	case 37:
		PLAYER(current_player)->state = 0x2Du;
		PLAYER(current_player)->substate = 0u;
		return;
	case 38:
		PLAYER(current_player)->state = 0x36u;
		PLAYER(current_player)->substate = 0u;
		return;
	case 39:
		OBJECT(current_object)->flags |= 0x1000u;
		check_player_contacts_hurt_unless_invulnerable(object);
		return;
	case 41:
		OBJECT(current_object)->flags |= 0x1000u;
		check_player_contacts_hurt(object);
		return;
	default:
		invalid_jump(0xE4DFu + OBJECT(object)->kind * 3u);
	}
}

/* Nothing holds the player: pushed_out 0, last_hazard and last_damaged 0xFFFF (the next touch counts as
   new), flags bits 18, 23 and 31 off and block flags bit 1 off. */
void check_player_contacts_clear_state(void)
{
	PLAYER(current_player)->pushed_out = 0;
	PLAYER(current_player)->last_hazard = 0xFFFFu;
	PLAYER(current_player)->last_damaged = 0xFFFFu;
	OBJECT(current_object)->flags &= ~0x40000u;
	OBJECT(current_object)->flags &= ~0x800000u;
	OBJECT(current_object)->flags &= 0x7FFFFFFFu;
	PLAYER(current_player)->flags &= ~2u;
}

/* Ignores toucher while invulnerable_timer runs (check_player_contacts_clear_state); else
   check_player_contacts_hurt. */
void check_player_contacts_hurt_unless_invulnerable(uint32_t toucher)
{
	if (PLAYER(current_player)->invulnerable_timer >= 0)
	{
		check_player_contacts_clear_state();
		return;
	}
	check_player_contacts_hurt(toucher);
}

/* Hurts the player with substate 0 (check_player_contacts_attack_or_hurt), so a jumping player attacks
   instead. */
void check_player_contacts_hurt(uint32_t toucher)
{
	uint8_t new_substate = 0;

	new_substate = 0u;
	check_player_contacts_attack_or_hurt(new_substate, toucher);
}

/* A jumping player attacks toucher (check_player_contacts_attack); otherwise it is hurt (state 0xD),
   taking new_substate and hurt_contact_delay 2 only for a new toucher (last_hazard). */
void check_player_contacts_attack_or_hurt(uint8_t new_substate, uint32_t toucher)
{
	if (PLAYER(current_player)->state == 4u)
	{
		check_player_contacts_attack(toucher);
		return;
	}
	if ((uint16_t)toucher != PLAYER(current_player)->last_hazard)
	{
		PLAYER(current_player)->substate = new_substate;
		PLAYER(current_player)->last_hazard = toucher;
		PLAYER(current_player)->hurt_contact_delay = 2;
	}
	PLAYER(current_player)->state = 0xDu;
}

/* The player against a solid object: jumping, it attacks it; otherwise it goes back to its previous
   place and floor and bounces (state 0xE) at speed 0x500 or more (not off kind 0x20) or stops. After
   0x1E frames on the same object it is pushed out toward the screen centre (pushed_out); climbing, it
   undoes the climb step. */
uint32_t check_player_contacts_solid(uint32_t health_limit, uint32_t toucher)
{
	uint32_t dx = 0, dy = 0;
	uint8_t push_out_delay = 0, push_out_delay_2 = 0;
	uint32_t bounce_direction_halfword = 0, bounce_direction_halfword_2 = 0;

	if (PLAYER(current_player)->state == 4u)
	{
		check_player_contacts_attack(toucher);
		return health_limit;
	}
	if (PLAYER(current_player)->state == 0xAu)
	{
		if (PLAYER(current_player)->invulnerable_timer >= 0)
		{
			check_player_contacts_clear_state();
			return health_limit;
		}
		if ((uint8_t)PLAYER(current_player)->pushed_out != 0u)
			return health_limit;
		if (PLAYER(current_player)->last_hazard != (uint16_t)toucher)
		{
			PLAYER(current_player)->push_out_delay = 0x1E;
			PLAYER(current_player)->last_hazard = toucher;
		}
		push_out_delay = PLAYER(current_player)->push_out_delay;
		PLAYER(current_player)->push_out_delay = push_out_delay - 1u;
		if ((int8_t)push_out_delay <= 1)
			PLAYER(current_player)->pushed_out = -1;
		PLAYER(current_player)->screen_x = PLAYER(current_player)->saved_screen_x;
		OBJECT(current_object)->projected_y = PLAYER(current_player)->saved_screen_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		PLAYER(current_player)->climb_remaining = PLAYER(current_player)->saved_climb_remaining;
		OBJECT(current_object)->animation_speed = 0xFFu;
		OBJECT(current_object)->animation_frame = 0u;
		PLAYER(current_player)->step_accumulator = 0;
		PLAYER(current_player)->side_step_accumulator = 0;
		return health_limit;
	}
	if (PLAYER(current_player)->state == 0x2Fu)
		return health_limit;
	if (PLAYER(current_player)->pushed_out < 0)
	{
		if ((uint8_t)PLAYER(current_player)->pushed_out == 0u)
		{
			aim_at_screen_centre(&health_limit, &dx, &dy);
			PLAYER(current_player)->push_out_x = dx;
			PLAYER(current_player)->push_out_y = dy;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
		}
		PLAYER(current_player)->pushed_out = -1;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += PLAYER(current_player)->push_out_x;
		OBJECT(current_object)->y += PLAYER(current_player)->push_out_y;
		PLAYER(current_player)->jump_delay = 2;
		PLAYER(current_player)->state = 0u;
		return health_limit;
	}
	if (PLAYER(current_player)->last_hazard != (uint16_t)toucher)
	{
		PLAYER(current_player)->push_out_delay = 0x1E;
		PLAYER(current_player)->last_hazard = toucher;
		OBJECT(current_object)->flags |= 0x1000u;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
		if (OBJECT(toucher)->kind != 0x20u && OBJECT(current_object)->speed_int >= 0x500)
		{
			bounce_direction_halfword = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
			PLAYER(current_player)->bounce_direction = (uint8_t)bounce_direction_halfword;
			PLAYER(current_player)->bounce_speed_z = (PLAYER(current_player)->bounce_speed_z & 0xFFFFFF00u) | ((bounce_direction_halfword >> 8u) & 0xFFu);
			PLAYER(current_player)->state = 0xEu;
			PLAYER(current_player)->substate = 0u;
			return health_limit;
		}
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags |= 0x800000u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->state = 0u;
		return health_limit;
	}
	push_out_delay_2 = PLAYER(current_player)->push_out_delay;
	PLAYER(current_player)->push_out_delay = push_out_delay_2 - 1u;
	if ((int8_t)push_out_delay_2 <= 1)
	{
		if ((uint8_t)PLAYER(current_player)->pushed_out == 0u)
		{
			aim_at_screen_centre(&health_limit, &dx, &dy);
			PLAYER(current_player)->push_out_x = dx;
			PLAYER(current_player)->push_out_y = dy;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
		}
		PLAYER(current_player)->pushed_out = -1;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += PLAYER(current_player)->push_out_x;
		OBJECT(current_object)->y += PLAYER(current_player)->push_out_y;
		PLAYER(current_player)->jump_delay = 2;
		PLAYER(current_player)->state = 0u;
		return health_limit;
	}
	OBJECT(current_object)->flags |= 0x1000u;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
	OBJECT(current_object)->z = PLAYER(current_player)->previous_z;
	OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	if (OBJECT(toucher)->kind != 0x20u && OBJECT(current_object)->speed_int >= 0x500)
	{
		bounce_direction_halfword_2 = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
		PLAYER(current_player)->bounce_direction = (uint8_t)bounce_direction_halfword_2;
		PLAYER(current_player)->bounce_speed_z = (PLAYER(current_player)->bounce_speed_z & 0xFFFFFF00u) | ((bounce_direction_halfword_2 >> 8u) & 0xFFu);
		PLAYER(current_player)->state = 0xEu;
		PLAYER(current_player)->substate = 0u;
		return health_limit;
	}
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x800000u;
	PLAYER(current_player)->free_turn_timer = 3u;
	PLAYER(current_player)->state = 0u;
	return health_limit;
}

/* A jumping player hits toucher once (last_damaged): toucher gets flags bit 6 and loses attack_power
   health. If that finishes it and it is not a sturdy kind (9, 0xB-0xE, 0x11, 0x18, 0x19), the player
   gets hit_stop 3; else it is thrown back with a hop, along toucher's direction (flags bit 30) or back
   the way it came. */
void check_player_contacts_attack(uint32_t toucher)
{
	uint32_t move_angle = 0;
	uint8_t attack_power = 0, health = 0;

	if (PLAYER(current_player)->last_damaged == (uint16_t)toucher)
		return;
	PLAYER(current_player)->last_damaged = toucher;
	OBJECT(toucher)->flags |= 0x40u;
	attack_power = PLAYER(current_player)->attack_power;
	health = OBJECT(toucher)->health;
	OBJECT(toucher)->health = health - attack_power;
	if ((int8_t)health <= (int8_t)attack_power && OBJECT(toucher)->kind != 0xCu && OBJECT(toucher)->kind != 9u && OBJECT(toucher)->kind != 0xBu && OBJECT(toucher)->kind != 0xDu && OBJECT(toucher)->kind != 0xEu && OBJECT(toucher)->kind != 0x11u && OBJECT(toucher)->kind != 0x18u && OBJECT(toucher)->kind != 0x19u)
	{
		PLAYER(current_player)->hit_stop = 3u;
		return;
	}
	OBJECT(current_object)->flags |= 0x2000u;
	if ((OBJECT(toucher)->flags & 0x40000000u) != 0u)
	{
		OBJECT(current_object)->direction = OBJECT(toucher)->direction;
		face_movement_direction(move_angle);
		OBJECT(current_object)->speed = 0x4000000;
		OBJECT(current_object)->speed_z = -0x18000;
		PLAYER(current_player)->state_timer = 6;
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		return;
	}
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u)
		move_angle = (uint16_t)(OBJECT(current_object)->direction + 0x200u);
	else
		move_angle = OBJECT(toucher)->direction;
	OBJECT(current_object)->direction = move_angle & 0x3FFu;
	face_movement_direction(move_angle);
	OBJECT(current_object)->speed = 0x4000000;
	OBJECT(current_object)->speed_z = -0x18000;
	PLAYER(current_player)->state_timer = 6;
	OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
	OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
}
