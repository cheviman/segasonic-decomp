/**
 *    Name:        player/states.c
 *    Description: the player's states in play: standing, walking, running, falling and jumping (player_move, player_animate), teetering on a ledge and going over it, dropping, falling into a pit, climbing a wall, skidding, hopping, dying, launched, hurt, bouncing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Common frame of states 0, 1, 2, 4, 0xC, 0x13, 0x14 and 0x17: counts down hit_stop, control_lock and
   free_turn_timer, checks contacts and walls, steers by the trackball and moves (in state 1 by the walk
   animation's steps), find_floor, then player_move_check_edge; a state set on the way, or by
   floor_type_player_states, goes to player_dispatch_state. */
void player_move(uint32_t depth, uint32_t cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, sink_offset = 0, state = 0, state_entry = 0;
	uint32_t flags = 0, flags_2 = 0;

	if (PLAYER(current_player)->hit_stop != 0u)
		PLAYER(current_player)->hit_stop--;
	if (PLAYER(current_player)->control_lock != 0u)
		PLAYER(current_player)->control_lock--;
	if (PLAYER(current_player)->free_turn_timer != 0u)
		PLAYER(current_player)->free_turn_timer--;
	check_zone_contact();
	if (PLAYER(current_player)->state == 0x22u)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	OBJECT(current_object)->flags &= ~0x1000u;
	check_player_contacts(&depth, &cursor);
	check_player_stalled();
	state = PLAYER(current_player)->state;
	if (player_state_stops_move[state] != 0u)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	push_player_out_of_wall(&state, &depth, &cursor);
	if (PLAYER(current_player)->state == 0xEu)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	if (per_player_view_mode == 0u && PLAYER(current_player)->state != 4u && PLAYER(current_player)->state != 0x17u && OBJECT(current_object)->floor_type != 0xAu && (OBJECT(current_object)->flags & 0x80000000u) == 0u)
	{
		check_player_step(&state, &cursor);
		if (PLAYER(current_player)->state == 0xEu)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
	}
	if (OBJECT(current_object)->floor_block < 0)
		player_steer_in_air();
	else
	{
		apply_floor_push();
		if ((uint8_t)PLAYER(current_player)->pushed_out == 0u && PLAYER(current_player)->control_lock == 0u)
		{
			if (PLAYER(current_player)->state == 2u)
				player_steer_in_air();
			else
				player_steer(state);
		}
	}
	sink_offset = set_speed_from_player_speed();
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~0x800000u;
	if ((flags & 0x800000u) == 0u)
	{
		if (PLAYER(current_player)->state != 1u || (int16_t)(OBJECT(current_object)->speed_int - PLAYER(current_player)->animation_speed_int) >= 0x10)
		{
			if (PLAYER(current_player)->hit_stop == 0u)
			{
				PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
				PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
				OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
				OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
				OBJECT(current_object)->flags &= ~0x8000u;
			}
		}
		else
		{
			PLAYER(current_player)->walk_pending_x += OBJECT(current_object)->speed_x;
			PLAYER(current_player)->walk_pending_y += OBJECT(current_object)->speed_y;
			flags_2 = OBJECT(current_object)->flags;
			OBJECT(current_object)->flags = flags_2 & ~0x8000u;
			if ((flags_2 & 0x8000u) != 0u)
			{
				PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
				PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
				OBJECT(current_object)->x += PLAYER(current_player)->walk_pending_x;
				OBJECT(current_object)->y += PLAYER(current_player)->walk_pending_y;
				PLAYER(current_player)->walk_pending_x = 0;
				PLAYER(current_player)->walk_pending_y = 0;
			}
		}
	}
	if (per_player_view_mode == 0u)
		find_floor(&ignored, &sink_offset, &ignored_2, &ignored_3, &cursor);
	sink_offset = update_quicksand_object(sink_offset);
	sink_offset = update_sinking_floor(sink_offset);
	if (OBJECT(current_object)->floor_type != 0u)
	{
		state_entry = floor_type_player_states_ADDRESS + OBJECT(current_object)->floor_type;
		if (FLOOR_TYPE_PLAYER_STATE(state_entry)->state == 0u)
		{
			player_move_check_edge(sink_offset, cursor);
			return;
		}
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = FLOOR_TYPE_PLAYER_STATE(state_entry)->state;
		PLAYER(current_player)->saved_substate = PLAYER(current_player)->substate;
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->sequence_step = 0u;
		player_dispatch_state(sink_offset, cursor);
		return;
	}
	player_move_check_edge(sink_offset, cursor);
}

/* The rest of player_move: set_floor_sprite_priority and check_player_floor_edge (states 5-9
   dispatched), apply_moving_floor, projection (sprite_offset_y 0x12 for it in a jump with attack_power
   above 1), keep_player_on_screen (state 0x16 dispatched, player_respawn stops), the jump check, blink,
   platform and lists, then player_animate. */
void player_move_check_edge(uint32_t depth, uint32_t cursor)
{
	uint32_t ignored = 0;

	set_floor_sprite_priority();
	check_player_floor_edge(&depth, &cursor);
	if (PLAYER(current_player)->state >= 5 && PLAYER(current_player)->state <= 9)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	apply_moving_floor();
	if (per_player_view_mode == 0u)
	{
		if (PLAYER(current_player)->state != 4u || PLAYER(current_player)->attack_power <= 1)
			project_object(&ignored, &depth);
		else
		{
			saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
			OBJECT(current_object)->sprite_offset_y = 0x12;
			project_object(&ignored, &depth);
			OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
		}
	}
	else
		depth = project_object_2();
	depth = keep_player_on_screen(depth);
	if (PLAYER(current_player)->state == 0x16u)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	if (OBJECT(current_object)->handler == player_respawn_ADDRESS)
		return;
	check_player_jump();
	update_player_blink();
	remember_platform();
	add_player_to_lists();
	player_animate(depth, cursor);
}

/* Second switch on the player's state, after player_move: the animation and drawing of states 0
   (standing), 1 (walking), 2 (falling), 4 (jumping), 0xC (skidding), 0x13 and 0x14 (running) and 0x17
   (hopping); state 3 does nothing and every other state goes back to player_dispatch_state. */
void player_animate(uint32_t depth, uint32_t cursor)
{
	switch (PLAYER(current_player)->state)
	{
	case 0:
	case 59:
		player_animate_standing();
		return;
	case 1:
		player_animate_walking();
		return;
	case 2:
		player_animate_falling();
		return;
	case 3:
		player_state_inactive();
		return;
	case 4:
		player_animate_jumping(cursor);
		return;
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 21:
	case 22:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
	case 36:
	case 37:
	case 38:
	case 39:
	case 40:
	case 41:
	case 42:
	case 43:
	case 44:
	case 45:
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
		player_dispatch_state(depth, cursor);
		return;
	case 12:
		player_animate_skidding(depth, cursor);
		return;
	case 19:
		player_animate_running();
		return;
	case 20:
		player_animate_running_fast();
		return;
	case 23:
		player_animate_hopping();
		return;
	default:
		invalid_jump(0x2FFCu + PLAYER(current_player)->state * 3u);
	}
}

/* State 0's animation: the character's standing table by facing, restarted when it was not standing
   (first step kind * 8 + 0x2C frames, standing_started), later steps 3 frames but 2 and 6. Drawn unless
   hidden (flags bit 19): at its base, or with player flags bit 0 clipped at the ground (clip entries at
   +0x102 and +0xF0) at draw_depth. */
void player_animate_standing(void)
{
	uint32_t animation_table = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if ((int32_t)OBJECT(current_object)->animation >= (int32_t)sonic_walking_animations_ADDRESS)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = ((uint32_t)OBJECT(current_object)->kind << 3) + 0x2Cu;
		OBJECT(current_object)->animation_frame = 0xFFu;
		PLAYER(current_player)->standing_started = 0xFFu;
	}
	animation_table = sonic_standing_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_table = ray_standing_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation_table = mighty_standing_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = animation_table + PLAYER_FACING_ANIMATION_OFFSETS(animation_table)[PLAYER(current_player)->facing];
	animate_with_flags(&ignored, &ignored_2);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed && PLAYER(current_player)->standing_started == 0u && OBJECT(current_object)->animation_frame != 2u && OBJECT(current_object)->animation_frame != 6u)
		OBJECT(current_object)->animation_timer = 3;
	PLAYER(current_player)->standing_started = 0u;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* State 1's animation: the character's walking table by facing, stepped the frame after walk_cycle (+
   animation_speed_int) reaches 0x200, which also sets flags bit 15 for player_move to apply
   walk_pending_x/_y; walk_pending_x/_y and walk_cycle cleared when it was not walking. Drawn like the
   standing player. */
void player_animate_walking(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, walking_animations = 0;
	uint32_t flags = 0;

	if ((int32_t)OBJECT(current_object)->animation < (int32_t)sonic_walking_animations_ADDRESS || (int32_t)OBJECT(current_object)->animation >= 0x11230C)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
	}
	if ((int32_t)OBJECT(current_object)->animation < (int32_t)sonic_walking_animations_ADDRESS || (int32_t)OBJECT(current_object)->animation >= (int32_t)sonic_running_animations_ADDRESS)
	{
		PLAYER(current_player)->walk_pending_x = 0;
		PLAYER(current_player)->walk_pending_y = 0;
		PLAYER(current_player)->walk_cycle = 0;
	}
	walking_animations = sonic_walking_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		walking_animations = ray_walking_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			walking_animations = mighty_walking_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = walking_animations + PLAYER_FACING_ANIMATION_OFFSETS(walking_animations)[PLAYER(current_player)->facing];
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~0x4000u;
	if ((flags & 0x4000u) != 0u)
		animate_step_with_flags(&ignored, &ignored_2);
	PLAYER(current_player)->walk_cycle += PLAYER(current_player)->animation_speed_int;
	if (PLAYER(current_player)->walk_cycle >= 0x200)
	{
		PLAYER(current_player)->walk_cycle_high = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
		OBJECT(current_object)->flags |= 0x8000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* State 0x14's animation: the character's running fast table (sonic/ray/mighty_running_fast_animations)
   through player_animate_run_cycle. */
void player_animate_running_fast(void)
{
	uint32_t animation_table = 0;

	animation_table = sonic_running_fast_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_table = ray_running_fast_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
		{
			animation_table = mighty_running_fast_animations_ADDRESS;
			player_animate_run_cycle(animation_table);
			return;
		}
	}
	player_animate_run_cycle(animation_table);
}

/* State 0x13's animation: the character's running table (sonic/ray/mighty_running_animations) through
   player_animate_run_cycle. */
void player_animate_running(void)
{
	uint32_t animation_table = 0;

	animation_table = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_table = ray_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
		{
			animation_table = mighty_running_animations_ADDRESS;
			player_animate_run_cycle(animation_table);
			return;
		}
	}
	player_animate_run_cycle(animation_table);
}

/* Picks the run animation: the alternate run (player_animate_run_cycle_alternate) with flags bit 31, or
   on floor type 1 with floor_attribute 1-4 while facing is 0-4 past
   floor_1_facing_bases[floor_attribute] (for 1 and 4 only while alternate_run_toggle, + 2 a frame, is
   0-0x7F); else player_animate_run_from_table. */
void player_animate_run_cycle(uint32_t animation_table)
{
	uint32_t floor_attribute = 0;
	uint8_t run_toggle = 0;

	if ((OBJECT(current_object)->flags & 0x80000000u) != 0u)
	{
		player_animate_run_cycle_alternate();
		return;
	}
	if (OBJECT(current_object)->floor_type != 1u || OBJECT(current_object)->floor_attribute < 1 || OBJECT(current_object)->floor_attribute > 4)
	{
		player_animate_run_from_table(animation_table);
		return;
	}
	floor_attribute = OBJECT(current_object)->floor_attribute;
	if ((uint8_t)(PLAYER(current_player)->facing - floor_1_facing_bases[floor_attribute]) >= 5u)
	{
		player_animate_run_from_table(animation_table);
		return;
	}
	if ((uint8_t)floor_attribute != 2u && (uint8_t)floor_attribute != 3u)
	{
		run_toggle = PLAYER(current_player)->alternate_run_toggle + 2u;
		PLAYER(current_player)->alternate_run_toggle = run_toggle;
		if ((int8_t)run_toggle < 0)
		{
			player_animate_run_from_table(animation_table);
			return;
		}
	}
	player_animate_run_cycle_alternate();
}

/* The alternate run animations of the player's character (sonic/ray/mighty_alternate_run_animations)
   through player_animate_run_from_table. */
void player_animate_run_cycle_alternate(void)
{
	uint32_t alternate_run_animations = 0;

	alternate_run_animations = sonic_alternate_run_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		alternate_run_animations = ray_alternate_run_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
		{
			alternate_run_animations = mighty_alternate_run_animations_ADDRESS;
			player_animate_run_from_table(alternate_run_animations);
			return;
		}
	}
	player_animate_run_from_table(alternate_run_animations);
}

/* The run cycles' common part: animation_table's animation for facing (restarted at step 2 after a
   non-running one), stepped the frame after walk_cycle (+ animation_speed_int) reaches 0x4F0 in state
   0x14, else 0x600 (0x1800 on floor type 4's floor_4_slow_run_steps), walk_cycle keeping its low byte;
   drawn like the standing player. */
void player_animate_run_from_table(uint32_t animation_table)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, walk_cycle_limit = 0;
	uint32_t flags = 0;

	if ((int32_t)OBJECT(current_object)->animation < (int32_t)sonic_running_animations_ADDRESS || (int32_t)OBJECT(current_object)->animation >= 0x11230C)
	{
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 2u;
	}
	OBJECT(current_object)->animation = animation_table + PLAYER_FACING_ANIMATION_OFFSETS(animation_table)[PLAYER(current_player)->facing];
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags & ~0x4000u;
	if ((flags & 0x4000u) != 0u)
		animate_step_with_flags(&ignored, &ignored_2);
	PLAYER(current_player)->walk_cycle += PLAYER(current_player)->animation_speed_int;
	walk_cycle_limit = 0x4F0u;
	if (PLAYER(current_player)->state != 0x14u)
	{
		walk_cycle_limit = 0x600u;
		if (OBJECT(current_object)->floor_type == 4u && floor_4_slow_run_steps[OBJECT(current_object)->animation_frame] != 0u)
			walk_cycle_limit = (walk_cycle_limit * 0x400u) >> 8;
	}
	if ((int16_t)walk_cycle_limit <= PLAYER(current_player)->walk_cycle)
	{
		PLAYER(current_player)->walk_cycle_high = 0u;
		OBJECT(current_object)->flags |= 0x4000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* State 2, falling to a lower floor: a hop on the first frame (speed_z -0x10000, +0x10000 if player
   flags bit 7 was set), gravity 0x6000, the running animation; on the floor state 1. No floor below
   (0x7FF) drops it into the pit: state 8, ledge_side, player_status, mishap_flag, the edge hang
   animation. Drawn at its floor or the previous one. */
void player_animate_falling(void)
{
	uint8_t new_status = 0;
	uint32_t far_hang_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, near_hang_animations = 0, running_animations = 0, side = 0;
	uint32_t flags = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x10000;
		flags = PLAYER(current_player)->flags;
		PLAYER(current_player)->flags = flags & ~0x80u;
		if ((flags & 0x80u) != 0u)
			OBJECT(current_object)->speed_z = 0x10000;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z_int < OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->state = 1u;
		PLAYER(current_player)->substate = 0u;
	}
	running_animations = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		running_animations = ray_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			running_animations = mighty_running_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = running_animations + PLAYER_FACING_ANIMATION_OFFSETS(running_animations)[PLAYER(current_player)->facing];
	animate_step_with_flags(&ignored, &ignored_2);
	if (OBJECT(current_object)->floor_z_int >= 0x7FF && OBJECT(current_object)->previous_floor_z_int <= OBJECT(current_object)->floor_z_int)
	{
		PLAYER(current_player)->state = 8u;
		PLAYER(current_player)->substate = 0u;
		get_ledge_side(&side, &ignored_3);
		PLAYER(current_player)->ledge_side = side;
		new_status = 0xFFu;
		if (OBJECT(current_object)->health <= 8)
		{
			new_status = 1u;
			OBJECT(current_object)->flags |= 0x400u;
		}
		player_status[OBJECT(current_object)->player] = new_status;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		far_hang_animations = sonic_far_edge_hang_animations_ADDRESS;
		near_hang_animations = sonic_near_edge_hang_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			far_hang_animations = ray_far_edge_hang_animations_ADDRESS;
			near_hang_animations = ray_near_edge_hang_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
			{
				far_hang_animations = mighty_far_edge_hang_animations_ADDRESS;
				near_hang_animations = mighty_near_edge_hang_animations_ADDRESS;
			}
		}
		switch ((uint16_t)PLAYER(current_player)->ledge_side)
		{
		case 0:
		case 4:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		case 1:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 2:
			OBJECT(current_object)->animation = near_hang_animations;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 3:
			OBJECT(current_object)->animation = near_hang_animations;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		default:
			invalid_jump(0x3540u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
		}
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if (OBJECT(current_object)->floor_z_int < 0x7FF)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

/* State 3: returns at once. */
void player_state_inactive(void)
{
}

/* State 4, the jump: speed_z -0x90000 on the first frame, then attack_power 4 (sound 0x1F as it is set,
   flags bit 6), the attack jump animation and gravity 0x9000 once past its first frame (both less on
   floor type 4); in stage 5 before cage_bar_hit a sign and "ATTACK!" above. Lands after 0x50 frames, or
   on the floor once state_timer is out. */
void player_animate_jumping(uint32_t sign)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, new_attack_power = 0, x_int = 0, y = 0, y_int = 0, z_int = 0;
	uint32_t animation_rate = 0, gravity = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, jump_animation = 0, palette = 0, plain_jump_animations = 0, scale = 0, sprite_flags = 0, text = 0, text_x = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->state_timer = 4;
		OBJECT(current_object)->speed_z = -0x90000;
		if (OBJECT(current_object)->floor_type == 4u)
			OBJECT(current_object)->speed_z = -0x48000;
		PLAYER(current_player)->unused_jump_byte = 0u;
		PLAYER(current_player)->attack_power = 0;
		PLAYER(current_player)->jump_frames = 0;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 4;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->flags |= 0x40u;
		if (stage == 5u && cage_bar_hit == 0u)
		{
			x_int = (uint16_t)OBJECT(current_object)->x_int;
			y_int = (uint16_t)OBJECT(current_object)->y_int;
			kind = 4u;
			z_int = (uint16_t)OBJECT(current_object)->z_int;
			spawn_sign_2(x_int, y_int, kind, z_int, sign);
		}
	}
	bounce_off_wall_in_air();
	PLAYER(current_player)->jump_delay = 1;
	new_attack_power = 4u;
	if (4u == 0u)
		new_attack_power = 1u;
	if (new_attack_power != 4u || (uint8_t)PLAYER(current_player)->attack_power == new_attack_power)
		PLAYER(current_player)->attack_power = (uint8_t)new_attack_power;
	else
	{
		PLAYER(current_player)->attack_power = (uint8_t)new_attack_power;
		sound_id = 0x1Fu;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	OBJECT(current_object)->flags &= ~0x40u;
	if ((uint8_t)PLAYER(current_player)->attack_power != 0u)
	{
		OBJECT(current_object)->flags |= 0x40u;
		if ((int8_t)OBJECT(current_object)->animation_frame < 0)
			OBJECT(current_object)->animation_frame = 0u;
	}
	jump_animation = sonic_attack_jump_animations_ADDRESS;
	plain_jump_animations = sonic_plain_jump_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		jump_animation = ray_attack_jump_animations_ADDRESS;
		plain_jump_animations = ray_plain_jump_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
		{
			jump_animation = mighty_attack_jump_animations_ADDRESS;
			plain_jump_animations = mighty_plain_jump_animations_ADDRESS;
		}
	}
	if (PLAYER(current_player)->attack_power < 1)
		jump_animation = plain_jump_animations;
	sprite_flags = PLAYER(current_player)->facing;
	jump_animation += PLAYER_FACING_ANIMATION_OFFSETS(jump_animation)[sprite_flags];
	if ((int8_t)PLAYER(current_player)->substate < 0)
		jump_animation += 4u;
	else
	{
		PLAYER(current_player)->substate = 0xFFu;
		sprite_flags = OBJECT(current_object)->sprite_flags;
		sprite_flags = sprite_flags & ~0xC0u;
		sprite_flags = sprite_flags | FLAGGED_STEP(jump_animation)->flags;
		OBJECT(current_object)->sprite_flags = sprite_flags;
		jump_animation += 2u;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(jump_animation - 0x2u)->frame;
		PLAYER(current_player)->jump_first_tile = OBJECT(current_object)->sprite_frame;
		jump_animation += 2u;
	}
	OBJECT(current_object)->animation = jump_animation;
	animation_rate = (sprite_flags & ~0xFFu) | 1u;
	if ((uint8_t)PLAYER(current_player)->attack_power != 4u)
		animation_rate = (animation_rate & ~0xFFu) | 2u;
	OBJECT(current_object)->animation_speed = animation_rate;
	animate_with_flags(&ignored, &ignored_3);
	if (PLAYER(current_player)->jump_first_tile != OBJECT(current_object)->sprite_frame)
	{
		gravity = 0x9000u;
		if (OBJECT(current_object)->floor_type == 4u)
			gravity = 0x2400u;
		OBJECT(current_object)->speed_z += gravity;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	if (stage == 5u && cage_bar_hit == 0u)
	{
		text_x = (uint16_t)OBJECT(current_object)->screen_x;
		text_x = (uint16_t)(text_x - 0x18u);
		y = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x30u);
		palette = 0x8440u;
		scale = 0x100u;
		text = attack_text_ADDRESS;
		queue_string_at(text_x, y, &palette, &scale, &text);
	}
	PLAYER(current_player)->jump_frames++;
	if (PLAYER(current_player)->jump_frames > 0x50)
	{
		player_animate_jumping_land();
		return;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->state_timer = 0;
		if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
		{
			player_animate_jumping_land();
			return;
		}
	}
	player_draw();
}

/* The jump's end: state 1, z on the floor, jump_delay 0, flags bits 6, 13, 20 and 21 cleared, the normal
   palette (palette_list + 2); then player_draw. */
void player_animate_jumping_land(void)
{
	PLAYER(current_player)->state = 1u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	PLAYER(current_player)->jump_delay = 0;
	OBJECT(current_object)->flags &= ~0x2000u;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x100000u;
	OBJECT(current_object)->flags &= ~0x200000u;
	player_draw();
}

/* Draws the player unless hidden (flags bit 19): at its floor, or the previous floor when it has no
   floor block; with player flags bit 0, clipped at the ground: the clip entries at +0x102 and +0xF0 and
   the player at draw_depth (its floor's screen y). */
void player_draw(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0;

	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		if (OBJECT(current_object)->floor_block >= 0)
		{
			ignored_2 = queue_sprite_at_floor();
			return;
		}
		ignored_2 = queue_sprite_at_previous_floor();
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored, &depth);
}

/* State 5, teetering at a pit's edge with the teeter animation for its ledge_side. Pulled back
   (ledge_pullback 0x10) it stands on the previous floor, headings from ledge_recover_headings; else it
   falls off (state 6, player_status, mishap_flag): at once on a kind 0x19 object, else at the
   animation's step 7 once state_timer (0x3C; 1 for ledge_side 4) is out. */
void player_state_teetering(uint32_t cursor)
{
	uint8_t new_status = 0, sound_id = 0;
	uint32_t block = 0, depth = 0, edge_direction = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ledge_side = 0, player = 0, side = 0, teeter_animation = 0, teeter_variant = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_frame = 0xFFu;
		get_ledge_side(&side, &block);
		PLAYER(current_player)->ledge_side = side;
		switch (side)
		{
		case 0:
		case 5:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = FLOOR_BLOCK(block)->y_min;
			break;
		case 1:
			teeter_variant = 0u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x = FLOOR_BLOCK(block)->x_max;
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 2:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
			OBJECT(current_object)->y = FLOOR_BLOCK(block)->y_max;
			break;
		case 3:
			teeter_variant = 1u;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			OBJECT(current_object)->x = FLOOR_BLOCK(block)->x_min;
			OBJECT(current_object)->y -= OBJECT(current_object)->speed_y;
			break;
		case 4:
			if (OBJECT(current_object)->floor_type == 0xCu || mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) == 0x11u || mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x1Eu)
			{
				edge_direction = (uint16_t)(((OBJECT(current_object)->direction + 0x200u) & 0x3FFu) + 0x80u) & 0x3FFu;
				edge_direction = edge_direction >> 8;
			}
			else
				edge_direction = mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x15u);
			switch (edge_direction)
			{
			case 0:
				teeter_variant = 1u;
				OBJECT(current_object)->sprite_flags |= 0x40u;
				PLAYER(current_player)->ledge_direction = 2;
				break;
			case 1:
				teeter_variant = 1u;
				OBJECT(current_object)->sprite_flags &= 0xFFBFu;
				PLAYER(current_player)->ledge_direction = 3;
				break;
			case 2:
				if (OBJECT(current_object)->floor_attribute == 4u)
				{
					teeter_variant = 1u;
					OBJECT(current_object)->sprite_flags |= 0x40u;
					PLAYER(current_player)->ledge_direction = 2;
				}
				else
				{
					teeter_variant = 0u;
					OBJECT(current_object)->sprite_flags &= 0xFFBFu;
					PLAYER(current_player)->ledge_direction = 0;
				}
				break;
			case 3:
			case 4:
				teeter_variant = 0u;
				OBJECT(current_object)->sprite_flags |= 0x40u;
				PLAYER(current_player)->ledge_direction = 1;
				break;
			default:
				invalid_jump(0x3903u + edge_direction * 2u);
			}
			break;
		default:
			invalid_jump(0x3837u + side * 2u);
		}
		cursor = teetering_animations_ADDRESS;
		teeter_animation = teetering_animations[OBJECT(current_object)->kind].offsets[teeter_variant] + teetering_animations_ADDRESS;
		OBJECT(current_object)->sprite_frame = TEETER_RECORD(teeter_animation)->first_frame;
		OBJECT(current_object)->animation = teeter_animation + 2u;
		PLAYER(current_player)->state_timer = 0x3C;
		if ((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x19u)
		{
			if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
				PLAYER(current_player)->state_timer = 1;
		}
		else
			PLAYER(current_player)->state_timer = 0;
		PLAYER(current_player)->ledge_pullback = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x2000u;
		if (((OBJECT(current_object)->flags & 4u) == 0u || OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x19u) && (uint16_t)PLAYER(current_player)->ledge_side != 4u)
		{
			sound_id = 0x38u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x3Cu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x44u;
			}
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
	apply_moving_floor();
	player = update_ledge_pullback();
	if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
		PLAYER(current_player)->ledge_pullback = 0;
	if (PLAYER(current_player)->ledge_pullback < 0x10)
	{
		if ((uint16_t)PLAYER(current_player)->state_timer == 0u)
		{
			PLAYER(current_player)->state = 6u;
			PLAYER(current_player)->substate = 0u;
			if (OBJECT(current_object)->floor_type != 0xDu)
			{
				new_status = 0xFFu;
				if (OBJECT(current_object)->health <= 8)
				{
					new_status = 1u;
					OBJECT(current_object)->flags |= 0x400u;
				}
				player = OBJECT(current_object)->player;
				player_status[player] = new_status;
			}
			PLAYER(current_player)->mishap_flag = 0xFFu;
		}
		else
		{
			state_timer = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer - 1u;
			if ((int16_t)state_timer <= 1)
			{
				PLAYER(current_player)->state_timer = -1;
				if (OBJECT(current_object)->animation_speed == (uint8_t)OBJECT(current_object)->animation_timer && OBJECT(current_object)->animation_frame == 7u)
				{
					PLAYER(current_player)->state = 6u;
					PLAYER(current_player)->substate = 0u;
					if (OBJECT(current_object)->floor_type != 0xDu)
					{
						new_status = 0xFFu;
						if (OBJECT(current_object)->health <= 8)
						{
							new_status = 1u;
							OBJECT(current_object)->flags |= 0x400u;
						}
						player = OBJECT(current_object)->player;
						player_status[player] = new_status;
					}
					PLAYER(current_player)->mishap_flag = 0xFFu;
				}
			}
		}
	}
	else
	{
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
		PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
		PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
		PLAYER(current_player)->facing_angle = ledge_recover_headings[ledge_side];
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	if (PLAYER(current_player)->state != 6u)
	{
		check_zone_contact();
		check_player_contacts(&player, &cursor);
	}
	update_player_blink();
	remember_platform();
	add_player_to_lists();
	animate(&ignored, &ignored_3, &ignored_4);
	project_object(&ignored_5, &depth);
	update_sinking_floor(depth);
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		depth = queue_sprite_at_base(depth);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_5, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_5, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_5, &depth);
}

/* State 6, falling off the edge into the pit: fall_draw_depth below the edge, the fall animation shifted
   up and left, the voice at health 8 or less, then gravity 0x3000 (sinking and floor type 0xD as in
   state 9). Below screen y 0x100: 8 health off and player_respawn (state 0x16 at 8 or less); no loss in
   stage 5 and stage 8 before scroll_position 0x4000. */
void player_state_falling_off_edge(void)
{
	uint8_t sound_id = 0;
	uint16_t sink_threshold = 0, x_shift = 0;
	uint32_t at_last_step = 0, depth = 0, floor_block_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index != 0x1Du)
		{
			if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x1Fu && mem_read8(floor_blocks + 2u + OBJECT(current_object)->previous_floor_block + 0x12u) != 0x11u)
			{
				if (OBJECT(current_object)->floor_type == 1u)
				{
					if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
					{
						if (PLAYER(current_player)->ledge_side < 2)
							PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
						PLAYER(current_player)->fall_draw_depth -= 0x10u;
					}
					else if (OBJECT(current_object)->floor_attribute != 5u)
					{
						if (PLAYER(current_player)->ledge_direction >= 2)
							PLAYER(current_player)->fall_draw_depth -= 0x10u;
						else
						{
							PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
							PLAYER(current_player)->fall_draw_depth -= 0x10u;
						}
					}
				}
				else if (PLAYER(current_player)->ledge_side < 2)
				{
					if (stage != 0xAu)
						PLAYER(current_player)->fall_draw_depth -= 0x10u;
					else
					{
						PLAYER(current_player)->fall_draw_depth -= 0x20u;
						PLAYER(current_player)->fall_draw_depth -= 0x10u;
					}
				}
			}
			else
			{
				if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
				{
					if (PLAYER(current_player)->ledge_side < 2)
						PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
				}
				else if (PLAYER(current_player)->ledge_direction < 2)
					PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
				PLAYER(current_player)->fall_draw_depth -= 4u;
			}
		}
		else if (OBJECT(current_object)->x < (int32_t)((((uint32_t)FLOOR_BLOCK(floor_block_record)->x_max - FLOOR_BLOCK(floor_block_record)->x_min) >> 1) + FLOOR_BLOCK(floor_block_record)->x_min))
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		else
		{
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		OBJECT(current_object)->animation += 0x1Cu;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 0xAu;
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		OBJECT(current_object)->projected_y -= 0x200000u;
		OBJECT(current_object)->sprite_layer = OBJECT(current_object)->projected_y_int;
		x_shift = 0x18u;
		if ((int32_t)OBJECT(current_object)->animation >= (int32_t)sonic_far_edge_teeter_animation_ADDRESS)
			x_shift = 0x14u;
		OBJECT(current_object)->projected_x -= x_shift;
		OBJECT(current_object)->speed_z = 0;
		if (OBJECT(current_object)->floor_type != 0xDu && OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &ignored, &ignored_2);
		}
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x3B56u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x3000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index != 0x22u)
		{
			if (FLOOR_BLOCK(floor_block_record)->type_index == 0x26u)
			{
				sink_threshold = 0u;
				if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
				{
					PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
					PLAYER(current_player)->state = 0x33u;
					PLAYER(current_player)->substate = 0u;
				}
			}
		}
		else
		{
			sink_threshold = 0u;
			if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x33u;
				PLAYER(current_player)->substate = 0u;
			}
		}
	}
	if (OBJECT(current_object)->floor_type == 0xDu && OBJECT(current_object)->z_int >= 0x530)
	{
		PLAYER(current_player)->state = 0x20u;
		PLAYER(current_player)->substate = 0u;
	}
	if (OBJECT(current_object)->screen_y >= 0x100)
	{
		if (stage == 5u || (stage == 8u && scroll_position < 0x4000u))
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
		else
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 8u;
			if ((int8_t)health > 8)
			{
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
				if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
				{
					at_last_step = animate_and_move(0u);
					if ((uint8_t)at_last_step != 0u)
						OBJECT(current_object)->animation_speed = 0xFFu;
				}
				update_screen_position(&ignored_3, &ignored);
				if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					depth = PLAYER(current_player)->fall_draw_depth;
					queue_sprite_at_depth(&ignored_3, &depth);
					return;
				}
				depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
				queue_sprite_at_depth(&ignored_3, &depth);
				return;
			}
			OBJECT(current_object)->health = 0;
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
		}
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		at_last_step = animate_and_move(0u);
		if ((uint8_t)at_last_step != 0u)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	update_screen_position(&ignored_3, &ignored);
	if (PLAYER(current_player)->ledge_side >= 2 || (OBJECT(current_object)->flags & 0x20000u) == 0u)
	{
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* Unused: an empty return left after player_state_falling_off_edge's code; it does nothing. */
void player_state_falling_off_edge_spare_return(void)
{
}

/* State 7, over an edge: slides on until slip_distance reaches 0x20000000 (/4 on floor type 9 or with
   flags bit 17), then hangs by its ledge_side; pulled back (ledge_pullback 0x14) it climbs back onto the
   floor, else it drops (state 8; on a moving edge once edge_object has passed it). Off the screen while
   climbing back: player_respawn, or state 0x16. */
void player_state_over_edge(uint32_t jump_offset, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t animation = 0, at_last_step = 0, depth = 0, far_hang_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ledge_side = 0, object = 0, off_screen = 0, side = 0, slip_limit = 0;

	switch ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate)
	{
	case 0:
	case 6:
		PLAYER(current_player)->substate++;
		get_ledge_side(&side, &cursor);
		PLAYER(current_player)->ledge_side = side;
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		if (OBJECT(current_object)->floor_type == 1u)
		{
			if (PLAYER(current_player)->ledge_side < 2)
				PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		else if (PLAYER(current_player)->ledge_side < 2)
		{
			if (stage != 0xAu)
				PLAYER(current_player)->fall_draw_depth -= 0x10u;
			else
			{
				PLAYER(current_player)->fall_draw_depth -= 0x20u;
				PLAYER(current_player)->fall_draw_depth -= 0x10u;
			}
		}
		set_speed_from_direction();
		OBJECT(current_object)->flags &= ~0x100u;
		if (OBJECT(current_object)->floor_type == 1u)
			OBJECT(current_object)->flags |= 0x100u;
		PLAYER(current_player)->heading = OBJECT(current_object)->direction;
		PLAYER(current_player)->ledge_pullback = 0;
		PLAYER(current_player)->slip_distance = 0;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->flags &= ~0x20000u;
		PLAYER(current_player)->flags &= ~0x80u;
		OBJECT(current_object)->flags &= ~0x2000u;
	case 1:
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored_2, &depth);
		if ((uint32_t)OBJECT(current_object)->speed == 0u)
			PLAYER(current_player)->substate++;
		else
		{
			PLAYER(current_player)->slip_distance += OBJECT(current_object)->speed;
			if ((PLAYER(current_player)->flags & 0x80u) != 0u)
				PLAYER(current_player)->substate++;
			else
			{
				slip_limit = 0x20000000u;
				if (OBJECT(current_object)->floor_type == 9u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
					slip_limit = (int32_t)slip_limit >> 2;
				if ((int32_t)slip_limit <= PLAYER(current_player)->slip_distance)
					PLAYER(current_player)->substate++;
			}
		}
		update_player_blink();
		if (PLAYER(current_player)->previous_state != 0xCu)
		{
			animation = OBJECT(current_object)->animation;
			if (((FLAGGED_ANIMATION_START(animation)->first_flags | FLAGGED_ANIMATION_START(animation)->second_flags) & ~0xC0u) == 0u)
				animate_with_flags(&depth, &ignored_3);
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			check_zone_contact();
			check_player_contacts(&depth, &cursor);
		}
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth + 1u);
			queue_sprite_at_depth(&ignored_2, &depth);
			return;
		}
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	case 2:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		far_hang_animations = sonic_far_edge_hang_animations_ADDRESS;
		cursor = sonic_near_edge_hang_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			far_hang_animations = ray_far_edge_hang_animations_ADDRESS;
			cursor = ray_near_edge_hang_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
			{
				far_hang_animations = mighty_far_edge_hang_animations_ADDRESS;
				cursor = mighty_near_edge_hang_animations_ADDRESS;
			}
		}
		switch ((uint16_t)PLAYER(current_player)->ledge_side)
		{
		case 0:
		case 4:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		case 1:
			OBJECT(current_object)->animation = far_hang_animations;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 2:
			OBJECT(current_object)->animation = cursor;
			OBJECT(current_object)->sprite_flags |= 0x40u;
			break;
		case 3:
			OBJECT(current_object)->animation = cursor;
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
			break;
		default:
			invalid_jump(0x3FE9u + (uint16_t)PLAYER(current_player)->ledge_side * 2u);
		}
		PLAYER(current_player)->state_timer = 0x3C;
		PLAYER(current_player)->ledge_pullback = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		update_ledge_pullback();
		OBJECT(current_object)->speed = 0;
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
			player_over_edge_check_pull_up(cursor);
			return;
		}
		object = LINKED(current_object, PLAYER(current_player)->edge_object);
		switch (OBJECT(object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		case 1:
			if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		case 3:
			if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		default:
			invalid_jump(0x405Bu + OBJECT(object)->direction * 2u);
		}
		break;
	case 3:
		update_ledge_pullback();
		OBJECT(current_object)->speed = 0;
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
			player_over_edge_check_pull_up(cursor);
			return;
		}
		object = LINKED(current_object, PLAYER(current_player)->edge_object);
		switch (OBJECT(object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		case 1:
			if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		case 3:
			if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
			{
				player_over_edge_drop(cursor);
				return;
			}
			player_over_edge_check_pull_up(cursor);
			return;
		default:
			invalid_jump(0x405Bu + OBJECT(object)->direction * 2u);
		}
		break;
	case 4:
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation += 0x16u;
		OBJECT(current_object)->speed = 0x4000000;
		OBJECT(current_object)->direction = (PLAYER(current_player)->heading + 0x200u) & 0x3FFu;
		set_speed_from_direction();
		sound_id = 0x47u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x4Fu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x67u;
		}
		play_sound(sound_id, &ignored, &ignored_4);
		PLAYER(current_player)->substate++;
	case 5:
		find_floor(&ignored_2, &ignored, &ignored_4, &ignored_5, &cursor);
		if ((OBJECT(current_object)->flags & 0x100u) == 0u)
		{
			if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
			{
				if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
			}
			else if ((OBJECT(current_object)->flags & 4u) == 0u)
			{
				PLAYER(current_player)->state = 0u;
				PLAYER(current_player)->substate = 0u;
				OBJECT(current_object)->flags &= ~0x20000u;
				ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
				PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
				PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
			}
		}
		else
		{
			object = LINKED(current_object, PLAYER(current_player)->edge_object);
			switch (OBJECT(object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= OBJECT(object)->y_int)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				else if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
					{
						PLAYER(current_player)->state = 0u;
						PLAYER(current_player)->substate = 0u;
						OBJECT(current_object)->flags &= ~0x20000u;
						ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
						PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
						PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
					}
				}
				else if ((OBJECT(current_object)->flags & 4u) == 0u)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				break;
			case 1:
				if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				else if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
					{
						PLAYER(current_player)->state = 0u;
						PLAYER(current_player)->substate = 0u;
						OBJECT(current_object)->flags &= ~0x20000u;
						ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
						PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
						PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
					}
				}
				else if ((OBJECT(current_object)->flags & 4u) == 0u)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				break;
			case 2:
				if (OBJECT(current_object)->x_int <= OBJECT(object)->x_int)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				else if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
					{
						PLAYER(current_player)->state = 0u;
						PLAYER(current_player)->substate = 0u;
						OBJECT(current_object)->flags &= ~0x20000u;
						ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
						PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
						PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
					}
				}
				else if ((OBJECT(current_object)->flags & 4u) == 0u)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				break;
			case 3:
				if (OBJECT(object)->x_int <= OBJECT(current_object)->x_int)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				else if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
				{
					if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_type != 2u && (uint32_t)OBJECT(current_object)->previous_floor_block == (uint32_t)OBJECT(current_object)->floor_block)
					{
						PLAYER(current_player)->state = 0u;
						PLAYER(current_player)->substate = 0u;
						OBJECT(current_object)->flags &= ~0x20000u;
						ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
						PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
						PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
					}
				}
				else if ((OBJECT(current_object)->flags & 4u) == 0u)
				{
					PLAYER(current_player)->state = 0u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->flags &= ~0x20000u;
					ledge_side = (uint16_t)PLAYER(current_player)->ledge_side;
					PLAYER(current_player)->heading = ledge_recover_headings[ledge_side];
					PLAYER(current_player)->target_heading = ledge_recover_headings[ledge_side];
				}
				break;
			default:
				invalid_jump(0x41EDu + OBJECT(object)->direction * 2u);
			}
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&ignored, &ignored_6, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored_2, &depth);
		check_zone_contact();
		check_player_contacts(&depth, &cursor);
		add_player_to_lists();
		margin_x = 0x50u;
		margin_y = 0x50u;
		off_screen = is_off_screen_margin(margin_x, margin_y);
		if (off_screen == 0u)
		{
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		if (OBJECT(current_object)->health > 0)
		{
			OBJECT(current_object)->handler = player_respawn_ADDRESS;
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			ignored = queue_sprite_at_previous_floor();
			return;
		}
		OBJECT(current_object)->health = 0;
		PLAYER(current_player)->state = 0x16u;
		PLAYER(current_player)->substate = 0u;
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored = queue_sprite_at_previous_floor();
		return;
	default:
		invalid_jump(0x3E05u + ((jump_offset & ~0xFFu) | PLAYER(current_player)->substate) * 3u);
	}
}

/* State 7's hang: ledge_pullback at 0x14 or more moves on to climbing back (substate + 1); state_timer
   counts down and when out the player drops (player_over_edge_drop), but with player flags bit 1 or
   flags bit 17 only once the touched object's sprite_scale is 0x100 or more; else
   player_over_edge_hang_finish. */
void player_over_edge_check_pull_up(uint32_t cursor)
{
	uint16_t state_timer = 0, state_timer_2 = 0;

	if (PLAYER(current_player)->ledge_pullback >= 0x14)
		PLAYER(current_player)->substate++;
	if ((PLAYER(current_player)->flags & 2u) == 0u && (OBJECT(current_object)->flags & 0x20000u) == 0u)
	{
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer > 1)
		{
			player_over_edge_hang_finish(cursor);
			return;
		}
		player_over_edge_drop(cursor);
		return;
	}
	state_timer_2 = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer_2 - 1u;
	if ((int16_t)state_timer_2 > 1)
	{
		player_over_edge_hang_finish(cursor);
		return;
	}
	PLAYER(current_player)->state_timer = 0;
	if ((int16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->sprite_scale >= 0x100)
	{
		player_over_edge_drop(cursor);
		return;
	}
	player_over_edge_hang_finish(cursor);
}

/* State 7 ends in a drop: state 8, player_status 0xFF (1 with flags bit 10 at health 8 or less),
   mishap_flag 0xFF; then player_over_edge_hang_finish. */
void player_over_edge_drop(uint32_t cursor)
{
	uint8_t new_status = 0;

	PLAYER(current_player)->state = 8u;
	PLAYER(current_player)->substate = 0u;
	new_status = 0xFFu;
	if (OBJECT(current_object)->health <= 8)
	{
		new_status = 1u;
		OBJECT(current_object)->flags |= 0x400u;
	}
	player_status[OBJECT(current_object)->player] = new_status;
	PLAYER(current_player)->mishap_flag = 0xFFu;
	player_over_edge_hang_finish(cursor);
}

/* State 7's frame end while hanging (and on the drop): animate, update_player_blink, project, contacts
   unless flags bit 17, the lists; drawn at the previous floor (ledge_side 2 or more), just in front of
   touched_by (its draw_depth + 1) with player flags bit 1 or flags bit 17, else at fall_draw_depth. */
void player_over_edge_hang_finish(uint32_t cursor)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, projected_depth = 0;

	animate(&ignored, &ignored_2, &ignored_3);
	update_player_blink();
	project_object(&ignored_4, &projected_depth);
	if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
	{
		check_zone_contact();
		check_player_contacts(&projected_depth, &cursor);
	}
	add_player_to_lists();
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if (PLAYER(current_player)->ledge_side >= 2)
	{
		ignored = queue_sprite_at_previous_floor();
		return;
	}
	if ((PLAYER(current_player)->flags & 2u) != 0u || (OBJECT(current_object)->flags & 0x20000u) != 0u)
	{
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth + 1u);
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	depth = PLAYER(current_player)->fall_draw_depth;
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* State 8, dropping into the pit: holds the hang's last frames 0x1E frames, lets go (a slip effect,
   characters 1 and 2's slip pieces, sound 0x16, the voice at health 8 or less) and falls down the
   screen. Below screen y 0x110: 8 health off and player_respawn (state 0x16 at 8 or less); no loss in
   stage 5 and stage 8 before scroll_position 0x4000. */
void player_state_dropping(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->sprite_palette |= 0x8000u;
		set_fall_sprite_priority();
		OBJECT(current_object)->sprite_frame = EDGE_HANG_ANIMATIONS(OBJECT(current_object)->animation)->hold_frame;
		PLAYER(current_player)->state_timer = 0x1E;
		PLAYER(current_player)->substate++;
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &ignored_2);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored_2 = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
			PLAYER(current_player)->substate++;
		update_screen_position(&ignored, &ignored_2);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored_2 = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		depth = OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		allocate_object(&free_count, &cursor);
		if (free_count == 0u)
			return;
		OBJECT(cursor)->sprite_flags = (OBJECT(current_object)->sprite_flags & 0xCFF0u) | 0x100Au;
		OBJECT(cursor)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(cursor)->animation_frame = 0u;
		OBJECT(cursor)->animation_speed = 3u;
		OBJECT(cursor)->animation_timer = 0;
		OBJECT(cursor)->animation = OBJECT(current_object)->animation;
		OBJECT(cursor)->animation += 0x22u;
		OBJECT(cursor)->direction = PLAYER(current_player)->ledge_side;
		OBJECT(cursor)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(cursor)->projected_x -= 0x10u;
		OBJECT(cursor)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(cursor)->projected_y -= 0x280000u;
		OBJECT(cursor)->z = OBJECT(current_object)->z;
		OBJECT(cursor)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
		OBJECT(cursor)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
		OBJECT(cursor)->handler = slip_effect_update_ADDRESS;
		OBJECT(current_object)->sprite_frame = EDGE_HANG_ANIMATIONS(OBJECT(current_object)->animation)->let_go_frame;
		OBJECT(current_object)->speed_y = 0;
		if (OBJECT(current_object)->kind == 2u)
			cursor = spawn_slip_piece(cursor);
		if (OBJECT(current_object)->kind == 1u)
			spawn_slip_pieces(cursor);
		sound_id = 0x16u;
		play_sound(sound_id, &ignored_2, &ignored_3);
		if (OBJECT(current_object)->health <= 8)
		{
			sound_id = 0x2Fu;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x37u;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x77u;
			}
			play_sound(sound_id, &ignored_2, &ignored_3);
		}
		PLAYER(current_player)->substate++;
	case 3:
		OBJECT(current_object)->speed_y += 0x6000u;
		OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->screen_y >= 0x110)
		{
			if (stage == 5u || (stage == 8u && scroll_position < 0x4000u))
				OBJECT(current_object)->handler = player_respawn_ADDRESS;
			else
			{
				health = OBJECT(current_object)->health;
				OBJECT(current_object)->health = health - 8u;
				if ((int8_t)health > 8)
				{
					OBJECT(current_object)->handler = player_respawn_ADDRESS;
					update_screen_position(&ignored, &ignored_2);
					if (PLAYER(current_player)->ledge_side >= 2)
					{
						ignored_2 = queue_sprite_at_previous_floor();
						return;
					}
					if ((PLAYER(current_player)->flags & 2u) != 0u)
					{
						depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
					if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
					{
						depth = PLAYER(current_player)->fall_draw_depth;
						queue_sprite_at_depth(&ignored, &depth);
						return;
					}
					object = LINKED(current_object, OBJECT(current_object)->touched_by);
					OBJECT(current_object)->sprite_palette &= 0x9FFFu;
					if ((int16_t)(OBJECT(object)->screen_y + 0x20u) <= OBJECT(current_object)->screen_y)
						OBJECT(current_object)->sprite_palette |= 0x2000u;
					depth = OBJECT(object)->draw_depth;
					queue_sprite_at_depth(&ignored, &depth);
					return;
				}
				OBJECT(current_object)->health = 0;
				PLAYER(current_player)->state = 0x16u;
				PLAYER(current_player)->substate = 0u;
			}
		}
		update_screen_position(&ignored, &ignored_2);
		if (PLAYER(current_player)->ledge_side >= 2)
		{
			ignored_2 = queue_sprite_at_previous_floor();
			return;
		}
		if ((PLAYER(current_player)->flags & 2u) != 0u)
		{
			depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->draw_depth - 1u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 0x20000u) == 0u)
		{
			depth = PLAYER(current_player)->fall_draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		if ((int16_t)(OBJECT(object)->screen_y + 0x20u) <= OBJECT(current_object)->screen_y)
			OBJECT(current_object)->sprite_palette |= 0x2000u;
		depth = OBJECT(object)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x4301u + PLAYER(current_player)->substate * 3u);
	}
}

/* Unused: an empty return left after player_state_dropping's code; it does nothing. */
void player_state_dropping_spare_return(void)
{
}

/* State 9, falling into a pit: fall_draw_depth below the edge, mishap_flag, then gravity 0x6000, drawn
   at that depth. Over floor block type 0x22 or 0x26 with y_int under group_leaders[0]'s it sinks (state
   0x33; the depth test reads 0 for both); floor type 0xD from z_int 0x530: state 0x20; below screen y
   0x130: 8 health off, player_respawn or state 0x16. */
void player_state_falling_into_pit(void)
{
	uint16_t sink_threshold = 0;
	uint32_t depth = 0, floor_block_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, side = 0;
	uint8_t health = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		get_ledge_side(&side, &ignored);
		PLAYER(current_player)->ledge_side = side;
		if ((uint16_t)PLAYER(current_player)->ledge_side == 4u)
			PLAYER(current_player)->ledge_direction = ((OBJECT(current_object)->direction + 0x80u) & 0x3FFu) >> 8;
		set_fall_sprite_priority();
		PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index != 0x1Du)
		{
			if ((uint16_t)PLAYER(current_player)->ledge_side != 4u)
			{
				if (PLAYER(current_player)->ledge_side < 2)
					PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
				PLAYER(current_player)->fall_draw_depth -= 0x10u;
			}
			else if (OBJECT(current_object)->floor_attribute != 5u)
			{
				if (PLAYER(current_player)->ledge_direction >= 2)
					PLAYER(current_player)->fall_draw_depth -= 0x10u;
				else
				{
					PLAYER(current_player)->fall_draw_depth = OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth - 1u;
					PLAYER(current_player)->fall_draw_depth -= 0x10u;
				}
			}
		}
		else if (OBJECT(current_object)->x < (int32_t)((((uint32_t)FLOOR_BLOCK(floor_block_record)->x_max - FLOOR_BLOCK(floor_block_record)->x_min) >> 1) + FLOOR_BLOCK(floor_block_record)->x_min))
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		else
		{
			PLAYER(current_player)->fall_draw_depth -= 0x70u;
			PLAYER(current_player)->fall_draw_depth -= 0x10u;
		}
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->substate++;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x4547u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->player_view.floor_block_high >= 0)
	{
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_BLOCK(floor_block_record)->type_index != 0x22u)
		{
			if (FLOOR_BLOCK(floor_block_record)->type_index == 0x26u)
			{
				sink_threshold = 0u;
				if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
				{
					PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
					PLAYER(current_player)->state = 0x33u;
					PLAYER(current_player)->substate = 0u;
				}
			}
		}
		else
		{
			sink_threshold = 0u;
			if ((int16_t)sink_threshold <= OBJECT(current_object)->z_int && OBJECT(current_object)->y_int < OBJECT(LINKED(current_object, group_leaders[0]))->y_int)
			{
				PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
				PLAYER(current_player)->state = 0x33u;
				PLAYER(current_player)->substate = 0u;
			}
		}
	}
	if (OBJECT(current_object)->floor_type == 0xDu && OBJECT(current_object)->z_int >= 0x530)
	{
		PLAYER(current_player)->state = 0x20u;
		PLAYER(current_player)->substate = 0u;
	}
	if (OBJECT(current_object)->screen_y < 0x130)
	{
		if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
			animate_with_flags(&ignored_2, &ignored_3);
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
		OBJECT(current_object)->sprite_offset_y = 0x12;
		project_object(&ignored_4, &ignored_2);
		OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	health = OBJECT(current_object)->health;
	OBJECT(current_object)->health = health - 8u;
	if ((int8_t)health > 8)
	{
		OBJECT(current_object)->handler = player_respawn_ADDRESS;
		if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
			animate_with_flags(&ignored_2, &ignored_3);
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
		OBJECT(current_object)->sprite_offset_y = 0x12;
		project_object(&ignored_4, &ignored_2);
		OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
		depth = PLAYER(current_player)->fall_draw_depth;
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	OBJECT(current_object)->health = 0;
	PLAYER(current_player)->state = 0x16u;
	PLAYER(current_player)->substate = 0u;
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		animate_with_flags(&ignored_2, &ignored_3);
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored_4, &ignored_2);
	OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
	depth = PLAYER(current_player)->fall_draw_depth;
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Unused: an empty return left after player_state_falling_into_pit's code; it does nothing. */
void player_state_falling_into_pit_spare_return(void)
{
}

/* State 0xA, climbing a wall (attribute_record, its floor_types record by floor_attribute): contacts
   first (states 0xD, 0x1F, 0x20, 0x36 dispatched); touched by a kind 5 object it only updates its
   position; else by substate: 0 start (from the top after a jump: state 9), 1 grab the wall, 2 over the
   top edge, 3 climb, 4 pull up. */
void player_state_climbing(uint32_t depth, uint32_t cursor)
{
	uint32_t attribute_record = 0;

	check_zone_contact();
	check_player_contacts(&depth, &cursor);
	check_player_stalled();
	if (PLAYER(current_player)->state == 0x1Fu || PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x36u || PLAYER(current_player)->state == 0x20u)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	PLAYER(current_player)->state = 0xAu;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	update_player_blink();
	if ((OBJECT(current_object)->flags & 4u) != 0u && OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind == 5u)
	{
		player_climbing_update_position(cursor, attribute_record);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 5:
		break;
	case 1:
		player_climbing_grab_wall(cursor, attribute_record);
		return;
	case 2:
		player_climbing_descend_over_edge(cursor, attribute_record);
		return;
	case 3:
		player_climbing_move(cursor, attribute_record);
		return;
	case 4:
		player_climbing_pull_up(attribute_record);
		return;
	default:
		invalid_jump(0x4755u + PLAYER(current_player)->substate * 3u);
	}
	PLAYER(current_player)->climb_from_top = 0u;
	if (OBJECT(current_object)->z_int <= FLOOR_ATTRIBUTE_RECORD(attribute_record)->top_z)
		PLAYER(current_player)->climb_from_top = 0xFFu;
	if (PLAYER(current_player)->climb_from_top == 0u || PLAYER(current_player)->previous_state != 4u)
	{
		player_climbing_start(cursor, attribute_record);
		return;
	}
	PLAYER(current_player)->state = 9u;
	PLAYER(current_player)->substate = 0u;
	player_dispatch_state(depth, cursor);
}

/* State 0xA substate 0: flags bits 6 and 13 cleared, the normal palette, speed 0, control_lock 0,
   substate 1; then player_climbing_grab_wall. */
void player_climbing_start(uint32_t cursor, uint32_t attribute_record)
{
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x2000u;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->substate++;
	player_climbing_grab_wall(cursor, attribute_record);
}

/* State 0xA substate 1: keeps y (x with the wall record's byte 0) within the record's span (+0xA, +0xC
   long). From below: climb_remaining = record +6, the climb animation, snapped to the block's edge, z
   and climb_base_z at the floor, player_climbing_begin_climb. From the top: at the block's x_max, the
   climb-over animation round an anchor, substate 2. */
void player_climbing_grab_wall(uint32_t cursor, uint32_t attribute_record)
{
	uint16_t wall_limit = 0;
	uint32_t along_wall = 0, climb_animation = 0, climb_over_animation = 0, floor_block_record = 0, ignored = 0, ignored_2 = 0;

	along_wall = current_object + 0x50u;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
		along_wall = current_object + 0x4Cu;
	wall_limit = FLOOR_ATTRIBUTE_RECORD(attribute_record)->along_min;
	if ((int16_t)mem_read16(along_wall + 2u) < (int16_t)wall_limit)
	{
		mem_write16(along_wall, 0u);
		mem_write16(along_wall + 2u, wall_limit);
	}
	else
	{
		wall_limit += FLOOR_ATTRIBUTE_RECORD(attribute_record)->along_length;
		if ((int16_t)wall_limit < (int16_t)mem_read16(along_wall + 2u))
		{
			mem_write16(along_wall, 0u);
			mem_write16(along_wall + 2u, wall_limit);
		}
	}
	if (PLAYER(current_player)->climb_from_top == 0u)
	{
		PLAYER(current_player)->climb_remaining = FLOOR_ATTRIBUTE_RECORD(attribute_record)->height;
		OBJECT(current_object)->flags &= ~0x80u;
		climb_animation = sonic_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			climb_animation = ray_climb_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				climb_animation = mighty_climb_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = climb_animation;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
			OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_frame = 0xFFu;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis == 0u)
			OBJECT(current_object)->x = FLOOR_BLOCK(floor_block_record)->x_min;
		else
			OBJECT(current_object)->y = FLOOR_BLOCK(floor_block_record)->y_max;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->climb_base_z = OBJECT(current_object)->floor_z;
		project_object(&ignored, &ignored_2);
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->projected_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
		player_climbing_begin_climb(cursor, attribute_record);
		return;
	}
	OBJECT(current_object)->x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 4u);
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->projected_x -= 0x18u;
	OBJECT(current_object)->projected_y_int -= 0x1Cu;
	OBJECT(current_object)->z_int += 0x14u;
	OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->anchor_x += 0xDu;
	OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
	OBJECT(current_object)->anchor_y += 0x1Eu;
	climb_over_animation = sonic_climb_over_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_over_animation = ray_climb_over_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_over_animation = mighty_climb_over_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_over_animation;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->climb_remaining = 0;
	OBJECT(current_object)->flags |= 0x80u;
	PLAYER(current_player)->substate++;
	player_climbing_descend_over_edge(cursor, attribute_record);
}

/* State 0xA substate 2, getting over the top edge to climb down: animate_around_anchor (drawn meanwhile)
   until the climb-over animation ends, then the climb animation from step 6 at the anchor's screen
   position and player_climbing_begin_climb. */
void player_climbing_descend_over_edge(uint32_t cursor, uint32_t attribute_record)
{
	uint32_t at_last_step = 0, climb_animation = 0, ignored = 0, ignored_2 = 0;

	at_last_step = animate_around_anchor();
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = climb_animation;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_frame = 6u;
	PLAYER(current_player)->screen_x_int = OBJECT(current_object)->anchor_x;
	PLAYER(current_player)->screen_x_fraction = 0u;
	OBJECT(current_object)->projected_y_int = OBJECT(current_object)->anchor_y;
	OBJECT(current_object)->projected_y_fraction = 0u;
	player_climbing_begin_climb(cursor, attribute_record);
}

/* State 0xA: step_accumulator 0, substate 3, then player_climbing_move. */
void player_climbing_begin_climb(uint32_t cursor, uint32_t attribute_record)
{
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->substate = 3u;
	player_climbing_move(cursor, attribute_record);
}

/* State 0xA substate 3: while a step's animation runs, player_climbing_check_vertical_step; else the
   trackball's speed (pushed_out pushes up) adds to step_accumulator when mostly vertical, to
   side_step_accumulator when sideways (each cleared when the input turns round), a side step at +-2 (+-8
   with fast_climb: block type 0x20 above z 0x4DE). */
void player_climbing_move(uint32_t cursor, uint32_t attribute_record)
{
	uint16_t side_threshold = 0;
	uint32_t cos_speed = 0;

	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		player_climbing_check_vertical_step(cursor, attribute_record);
		return;
	}
	cos_speed = set_climb_speed_from_trackball();
	if ((uint8_t)PLAYER(current_player)->pushed_out != 0u)
	{
		OBJECT(current_object)->direction = 0u;
		OBJECT(current_object)->speed = 0x3000000;
		cos_speed = set_speed_from_direction();
	}
	PLAYER(current_player)->fast_climb = 0u;
	if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) == 0x20u && OBJECT(current_object)->z_int > 0x4DE)
		PLAYER(current_player)->fast_climb = 0xFFu;
	PLAYER(current_player)->target_heading = OBJECT(current_object)->direction;
	if ((uint16_t)((OBJECT(current_object)->direction & 0x1FFu) - 0x80u) >= 0x100u)
	{
		if (PLAYER(current_player)->step_accumulator_int < 0)
		{
			if (OBJECT(current_object)->speed_y_int >= 0)
				PLAYER(current_player)->step_accumulator = 0;
		}
		else if (OBJECT(current_object)->speed_y_int < 0)
		{
			PLAYER(current_player)->step_accumulator = 0;
			PLAYER(current_player)->step_accumulator += OBJECT(current_object)->speed_y;
			player_climbing_check_vertical_step(cursor, attribute_record);
			return;
		}
		PLAYER(current_player)->step_accumulator += OBJECT(current_object)->speed_y;
		player_climbing_check_vertical_step(cursor, attribute_record);
		return;
	}
	if (PLAYER(current_player)->side_step_accumulator_int < 0)
	{
		if (OBJECT(current_object)->speed_x_int >= 0)
			PLAYER(current_player)->side_step_accumulator = 0;
	}
	else if (OBJECT(current_object)->speed_x_int < 0)
	{
		PLAYER(current_player)->side_step_accumulator = 0;
		PLAYER(current_player)->side_step_accumulator += OBJECT(current_object)->speed_x;
		side_threshold = 2u;
		if (PLAYER(current_player)->fast_climb != 0u)
			side_threshold = (uint16_t)((uint32_t)side_threshold << 2);
		if ((int16_t)side_threshold <= PLAYER(current_player)->side_step_accumulator_int)
		{
			OBJECT(current_object)->flags &= ~0x80u;
			player_climbing_side_step(cos_speed, cursor, attribute_record);
			return;
		}
		if ((int16_t)(0u - side_threshold) < PLAYER(current_player)->side_step_accumulator_int)
		{
			player_climbing_update_position(cursor, attribute_record);
			return;
		}
		OBJECT(current_object)->flags |= 0x80u;
		player_climbing_side_step(cos_speed, cursor, attribute_record);
		return;
	}
	PLAYER(current_player)->side_step_accumulator += OBJECT(current_object)->speed_x;
	side_threshold = 2u;
	if (PLAYER(current_player)->fast_climb != 0u)
		side_threshold = (uint16_t)((uint32_t)side_threshold << 2);
	if ((int16_t)side_threshold <= PLAYER(current_player)->side_step_accumulator_int)
	{
		OBJECT(current_object)->flags &= ~0x80u;
		player_climbing_side_step(cos_speed, cursor, attribute_record);
		return;
	}
	if ((int16_t)(0u - side_threshold) < PLAYER(current_player)->side_step_accumulator_int)
	{
		player_climbing_update_position(cursor, attribute_record);
		return;
	}
	OBJECT(current_object)->flags |= 0x80u;
	player_climbing_side_step(cos_speed, cursor, attribute_record);
}

/* State 0xA, a sideways step: both accumulators 0, the climb-sideways animation; screen_x moves 2 (back
   with flags bit 7) and projected_y 1, unless the new spot, unprojected, leaves the wall record's span
   or the screen rows 0x10-0xCF; the old position kept in saved_screen_x/_y. Then
   player_climbing_update_position. */
void player_climbing_side_step(uint32_t cos_speed, uint32_t cursor, uint32_t attribute_record)
{
	uint16_t wall_start = 0;
	uint32_t climb_sideways_animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, side_step = 0, step_x = 0;

	PLAYER(current_player)->side_step_accumulator = 0;
	PLAYER(current_player)->step_accumulator = 0;
	climb_sideways_animation = sonic_climb_sideways_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_sideways_animation = ray_climb_sideways_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_sideways_animation = mighty_climb_sideways_animation_ADDRESS;
	}
	if (OBJECT(current_object)->animation != climb_sideways_animation)
	{
		OBJECT(current_object)->animation = climb_sideways_animation;
		OBJECT(current_object)->animation_frame = 0u;
	}
	side_step = 0x20000u;
	if ((OBJECT(current_object)->flags & 0x80u) != 0u)
		side_step = 0u - side_step;
	step_x = PLAYER(current_player)->screen_x + side_step;
	side_step_screen_x = step_x;
	side_step = (int32_t)side_step >> 1;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
		side_step = 0u - side_step;
	side_step += OBJECT(current_object)->projected_y;
	step_projected_y = side_step;
	side_step = side_step * 2u;
	unproject_point(&step_x, &side_step);
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
		side_step = step_x;
	side_step = (int32_t)side_step >> 16;
	wall_start = FLOOR_ATTRIBUTE_RECORD(attribute_record)->along_min;
	if ((int16_t)side_step < (int16_t)wall_start || (int16_t)(wall_start + FLOOR_ATTRIBUTE_RECORD(attribute_record)->along_length) < (int16_t)side_step || (uint16_t)(((int32_t)((uint32_t)step_projected_y + OBJECT(current_object)->z + cos_speed) >> 16) - view_y - 0x10u) >= 0xC0u)
	{
		player_climbing_update_position(cursor, attribute_record);
		return;
	}
	PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
	PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
	PLAYER(current_player)->screen_x = side_step_screen_x;
	OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
	OBJECT(current_object)->projected_y = step_projected_y;
	animate_step(&ignored, &ignored_2, &ignored_3);
	player_climbing_update_position(cursor, attribute_record);
}

/* State 0xA: step_accumulator_int at 4 or more starts a step down (flags bit 7 set), at -4 or less a
   step up (bit cleared), through player_climbing_start_vertical_step; in between
   player_climbing_animate_vertical_step. */
void player_climbing_check_vertical_step(uint32_t cursor, uint32_t attribute_record)
{
	if (PLAYER(current_player)->step_accumulator_int >= 4)
	{
		OBJECT(current_object)->flags |= 0x80u;
		player_climbing_start_vertical_step(cursor, attribute_record);
		return;
	}
	if (PLAYER(current_player)->step_accumulator_int > -4)
	{
		player_climbing_animate_vertical_step(cursor, attribute_record);
		return;
	}
	OBJECT(current_object)->flags &= ~0x80u;
	player_climbing_start_vertical_step(cursor, attribute_record);
}

/* State 0xA, a new vertical step: both accumulators 0, the climb animation at speed 4. Refused
   (animation_speed 0xFF, step_accumulator 0) when it would go below climb_base_z or past the screen's
   edge (screen y 0xD0 going down, 0x10 going up). Then player_climbing_animate_vertical_step. */
void player_climbing_start_vertical_step(uint32_t cursor, uint32_t attribute_record)
{
	uint32_t climb_animation = 0, step_dz = 0, step_target = 0;

	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->side_step_accumulator = 0;
	climb_animation = sonic_climb_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		climb_animation = ray_climb_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			climb_animation = mighty_climb_animation_ADDRESS;
	}
	if (OBJECT(current_object)->animation != climb_animation)
	{
		OBJECT(current_object)->animation = climb_animation;
		OBJECT(current_object)->animation_frame = 0u;
	}
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	step_dz = 0x180000u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
		step_dz = 0u - step_dz;
	step_target = step_dz + OBJECT(current_object)->z;
	if (PLAYER(current_player)->climb_base_z < (int32_t)step_target)
	{
		OBJECT(current_object)->animation_speed = 0xFFu;
		PLAYER(current_player)->step_accumulator = 0;
		player_climbing_animate_vertical_step(cursor, attribute_record);
		return;
	}
	step_target = (uint16_t)(((int32_t)(step_target + OBJECT(current_object)->projected_y) >> 16) - view_y);
	if ((OBJECT(current_object)->flags & 0x80u) != 0u)
	{
		if ((int16_t)step_target < 0xD0)
		{
			player_climbing_animate_vertical_step(cursor, attribute_record);
			return;
		}
		OBJECT(current_object)->animation_speed = 0xFFu;
		PLAYER(current_player)->step_accumulator = 0;
		player_climbing_animate_vertical_step(cursor, attribute_record);
		return;
	}
	if ((int16_t)step_target <= 0x10)
	{
		OBJECT(current_object)->animation_speed = 0xFFu;
		PLAYER(current_player)->step_accumulator = 0;
		player_climbing_animate_vertical_step(cursor, attribute_record);
		return;
	}
	player_climbing_animate_vertical_step(cursor, attribute_record);
}

/* State 0xA, a vertical step (none while animation_speed is negative): speed 4 - min(|speed_y_int|, 3),
   * 4 with fast_climb; each new step moves z and climb_remaining 2 (down with flags bit 7) and the
   screen position by the wall record's +4 slope, keeping the old ones; it ends at frame 6 or 0. Then
   player_climbing_update_position. */
void player_climbing_animate_vertical_step(uint32_t cursor, uint32_t attribute_record)
{
	uint8_t animation_speed = 0;
	uint16_t climb_speed = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, screen_shift = 0, screen_x_shift = 0, z_step = 0;

	if ((int8_t)OBJECT(current_object)->animation_speed < 0)
	{
		player_climbing_update_position(cursor, attribute_record);
		return;
	}
	climb_speed = (uint16_t)OBJECT(current_object)->speed_y_int;
	if ((int16_t)climb_speed < 0)
		climb_speed = (uint16_t)(0u - climb_speed);
	if ((int16_t)climb_speed > 3)
		climb_speed = 3u;
	animation_speed = (uint8_t)(uint16_t)(4u - climb_speed);
	if (PLAYER(current_player)->fast_climb != 0u)
		animation_speed = (uint8_t)(uint16_t)((uint32_t)animation_speed << 2);
	OBJECT(current_object)->animation_speed = animation_speed;
	animate(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->animation_speed != (uint8_t)OBJECT(current_object)->animation_timer)
	{
		player_climbing_update_position(cursor, attribute_record);
		return;
	}
	if (OBJECT(current_object)->animation_frame != 6u)
	{
		if (OBJECT(current_object)->animation_frame == 0u)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	else
		OBJECT(current_object)->animation_speed = 0xFFu;
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	z_step = 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
		z_step = 0u - z_step;
	OBJECT(current_object)->z_int += z_step;
	PLAYER(current_player)->saved_climb_remaining = PLAYER(current_player)->climb_remaining;
	PLAYER(current_player)->climb_remaining += z_step;
	PLAYER(current_player)->saved_screen_x = PLAYER(current_player)->screen_x;
	screen_shift = FLOOR_ATTRIBUTE_RECORD(attribute_record)->x_per_rise * z_step;
	screen_x_shift = screen_shift;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis == 0u)
		screen_x_shift = 0u - screen_x_shift;
	PLAYER(current_player)->screen_x += screen_x_shift;
	OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
	PLAYER(current_player)->saved_screen_y = OBJECT(current_object)->projected_y;
	OBJECT(current_object)->projected_y += (int32_t)screen_shift >> 1;
	player_climbing_update_position(cursor, attribute_record);
}

/* State 0xA's frame end: climb_remaining at 0 starts the pull-up (substate 4); at the wall record's +6
   or more (the foot) the player stands (state 0, free_turn_timer 3, 2 off the wall). Then x/y
   unprojected from the screen, the lists, on floor block type 0x20 the priority and splash_zone (z
   0x4C7-0x4E9, spawn_climb_zone_effect on entering); drawn. */
void player_climbing_update_position(uint32_t cursor, uint32_t attribute_record)
{
	uint32_t across_wall = 0, climb_over_animation = 0, ignored = 0, ignored_2 = 0, world_x = 0, world_y = 0;

	if (PLAYER(current_player)->climb_remaining <= 0)
	{
		climb_over_animation = sonic_climb_over_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			climb_over_animation = ray_climb_over_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				climb_over_animation = mighty_climb_over_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = climb_over_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		PLAYER(current_player)->substate++;
		player_climbing_pull_up(attribute_record);
		return;
	}
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->height <= PLAYER(current_player)->climb_remaining)
	{
		across_wall = current_object + 0x4Eu;
		if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
			across_wall = current_object + 0x52u;
		mem_write16(across_wall, mem_read16(across_wall) - 2u);
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->free_turn_timer = 3u;
		OBJECT(current_object)->flags &= ~0x80u;
	}
	update_screen_position(&ignored, &ignored_2);
	world_x = PLAYER(current_player)->screen_x;
	world_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&world_x, &world_y);
	OBJECT(current_object)->x = world_x;
	OBJECT(current_object)->y = world_y;
	add_player_to_lists();
	if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) != 0x20u)
	{
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	OBJECT(current_object)->sprite_palette &= 0x1FFFu;
	OBJECT(current_object)->sprite_palette |= 0x8000u;
	if (OBJECT(current_object)->z_int >= 0x4B7)
	{
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
	}
	PLAYER(current_player)->splash_zone = 0u;
	if (OBJECT(current_object)->z_int >= 0x4C7 && OBJECT(current_object)->z_int <= 0x4E9)
	{
		PLAYER(current_player)->splash_zone = 0xFFu;
		if (PLAYER(current_player)->was_in_splash_zone == 0u)
			spawn_climb_zone_effect(cursor);
	}
	PLAYER(current_player)->was_in_splash_zone = PLAYER(current_player)->splash_zone;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	ignored_2 = queue_sprite(ignored_2);
}

/* State 0xA substate 4, the pull-up over the top: animate_around_anchor (drawn meanwhile) until it ends;
   then z and floor_z at the wall record's +8 (the top), state 0 with speeds 0, placed 0xA past the floor
   block's edge and turned toward heading 0x80 (0x380 when the record's byte 0 is set). */
void player_climbing_pull_up(uint32_t attribute_record)
{
	uint16_t new_heading = 0;
	uint32_t at_last_step = 0, floor_block_record = 0, ignored = 0, ignored_2 = 0;

	at_last_step = animate_around_anchor();
	pull_up_done = at_last_step;
	update_screen_position(&ignored, &ignored_2);
	if (pull_up_done == 0u)
	{
		add_player_to_lists();
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	OBJECT(current_object)->z_int = FLOOR_ATTRIBUTE_RECORD(attribute_record)->top_z;
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->flags &= ~0x80u;
	PLAYER(current_player)->state = 0u;
	PLAYER(current_player)->free_turn_timer = 3u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	PLAYER(current_player)->animation_speed = 0;
	floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis == 0u)
	{
		OBJECT(current_object)->x = FLOOR_BLOCK(floor_block_record)->x_max;
		OBJECT(current_object)->x_int += 0xAu;
	}
	else
	{
		OBJECT(current_object)->y = FLOOR_BLOCK(floor_block_record)->y_min;
		OBJECT(current_object)->y_int -= 0xAu;
	}
	new_heading = 0x80u;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
		new_heading = 0x380u;
	PLAYER(current_player)->target_heading = new_heading;
	turn_player();
	add_player_to_lists();
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	ignored_2 = queue_sprite(ignored_2);
}

/* States 0xB and 0x37, not playing: out of the game (flags bit 10, in_game and health 0, player_status
   0). Unless joining_closed, a start with credits (try_join) gives handler player_join, in_game 1, rings
   0, stages_reached 1 and is_joining, and erases the player's HUD message (hud_text_positions_3p, or _2p
   with cabinet_config bit 0 clear). */
void player_state_not_playing(void)
{
	uint32_t joined = 0, player = 0, text = 0, text_position = 0;

	OBJECT(current_object)->flags |= 0x400u;
	PLAYER(current_player)->in_game = 0;
	OBJECT(current_object)->health = 0;
	PLAYER(current_player)->enter_on_respawn = 0u;
	player_status[OBJECT(current_object)->player] = 0u;
	if (joining_closed == 0u)
	{
		joined = try_join();
		if (joined != 0u)
		{
			OBJECT(current_object)->handler = player_join_ADDRESS;
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->is_joining = 0xFFu;
			PLAYER(current_player)->in_game = 1;
			PLAYER(current_player)->rings = 0u;
			PLAYER(current_player)->stages_reached = 1u;
			text = hud_message_blank_text_ADDRESS;
			player = OBJECT(current_object)->player;
			text_position = 0x310000u | hud_text_positions_3p[player];
			if ((cabinet_config & 1u) == 0u)
				text_position = (text_position & ~0xFFFFu) | hud_text_positions_2p[player];
			erase_text_lines(&text, &text_position);
		}
	}
}

/* State 0xC, skidding, trackball locked; the jump button turns it into a jump at once. First frame: the
   skid animation for the opposite facing, skid_slowdown and skid_speed_slowdown (ground_speed and speed
   / 8, / 32 on floor type 7), sound 0x27; then player_animate_skidding_slow_down; stopped, the animation
   ends in walking (state 1). */
void player_animate_skidding(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t ground_slowdown = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, skid_animations = 0, skid_facing = 0, speed_slowdown = 0;

	PLAYER(current_player)->control_lock = 0xFFu;
	start_jump_if_pressed();
	if (PLAYER(current_player)->state == 4u)
	{
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->free_turn_timer = 3u;
		PLAYER(current_player)->animation_speed = 0;
		player_animate(depth, cursor);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 3:
		skid_animations = sonic_skid_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			skid_animations = ray_skid_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				skid_animations = mighty_skid_animations_ADDRESS;
		}
		skid_facing = PLAYER(current_player)->facing;
		skid_facing = (uint8_t)(skid_facing + 8u);
		OBJECT(current_object)->animation = skid_animations + PLAYER_FACING_ANIMATION_OFFSETS(skid_animations)[skid_facing & 0xFu];
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		PLAYER(current_player)->skid_dust_timer = 0;
		speed_slowdown = (int32_t)OBJECT(current_object)->speed >> 3;
		if (OBJECT(current_object)->floor_type == 7u)
			speed_slowdown = (int32_t)speed_slowdown >> 2;
		PLAYER(current_player)->skid_speed_slowdown = speed_slowdown;
		ground_slowdown = (int32_t)PLAYER(current_player)->ground_speed >> 3;
		if (OBJECT(current_object)->floor_type == 7u)
			ground_slowdown = (int32_t)ground_slowdown >> 2;
		PLAYER(current_player)->skid_slowdown = ground_slowdown;
		sound_id = 0x27u;
		play_sound(sound_id, &ignored, &ignored_2);
		PLAYER(current_player)->substate++;
		player_animate_skidding_slow_down(cursor);
		return;
	case 1:
		player_animate_skidding_slow_down(cursor);
		return;
	case 2:
		animate_with_flags(&ignored, &ignored_3);
		if (OBJECT(current_object)->animation_frame == 0u)
		{
			PLAYER(current_player)->control_lock = 0u;
			PLAYER(current_player)->free_turn_timer = 3u;
			PLAYER(current_player)->state = 1u;
		}
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		if ((PLAYER(current_player)->flags & 1u) == 0u)
		{
			ignored = queue_sprite_at_base(ignored);
			return;
		}
		PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		current_object = current_player + 0x102u;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored_4, &depth);
		current_object = object_cursor;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored_4, &depth);
		current_object = current_player + 0xF0u;
		depth = PLAYER(current_player)->draw_depth;
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	default:
		invalid_jump(0x4EF5u + PLAYER(current_player)->substate * 3u);
	}
}

/* The skid's slowing down (substate 1): a dust puff every 4 frames (spawn_skid_dust; not on floor type 7
   or while drawn clipped), the skid animation up to its step 2, ground_speed and speed less
   skid_slowdown and skid_speed_slowdown; when speed runs out both are 0 and substate 2. Then drawn. */
void player_animate_skidding_slow_down(uint32_t cursor)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;
	uint8_t skid_dust_timer = 0;
	uint32_t skid_speed_slowdown = 0, speed = 0;

	skid_dust_timer = PLAYER(current_player)->skid_dust_timer;
	PLAYER(current_player)->skid_dust_timer = skid_dust_timer - 1u;
	if ((int8_t)skid_dust_timer <= 1)
	{
		PLAYER(current_player)->skid_dust_timer = 4;
		if (OBJECT(current_object)->floor_type != 7u && (PLAYER(current_player)->flags & 1u) == 0u)
			spawn_skid_dust(cursor);
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
	{
		animate_with_flags(&ignored, &ignored_2);
		if ((int8_t)OBJECT(current_object)->animation_frame >= 2)
			OBJECT(current_object)->animation_speed = 0xFFu;
	}
	PLAYER(current_player)->ground_speed -= PLAYER(current_player)->skid_slowdown;
	skid_speed_slowdown = PLAYER(current_player)->skid_speed_slowdown;
	speed = OBJECT(current_object)->speed;
	OBJECT(current_object)->speed = speed - skid_speed_slowdown;
	if ((int32_t)speed <= (int32_t)skid_speed_slowdown)
	{
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->ground_speed = 0;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->substate++;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if ((PLAYER(current_player)->flags & 1u) == 0u)
	{
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
	current_object = current_player + 0x102u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = object_cursor;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = current_player + 0xF0u;
	depth = PLAYER(current_player)->draw_depth;
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* State 0x2E, game over: on its first frame start_ranking_entry and player_status 3;
   enter_ranking_initials every frame until game_over_done is set, then state 0x24 with
   continue_countdown and state_timer 0. */
void player_state_game_over(uint32_t jump_offset)
{
	if (PLAYER(current_player)->state_timer <= 0)
	{
		PLAYER(current_player)->state_timer++;
		start_ranking_entry();
		jump_offset = OBJECT(current_object)->player;
		player_status[jump_offset] = 3u;
	}
	enter_ranking_initials(jump_offset);
	if (PLAYER(current_player)->game_over_done != 0u)
	{
		PLAYER(current_player)->state = 0x24u;
		PLAYER(current_player)->continue_countdown = 0u;
		PLAYER(current_player)->state_timer = 0;
	}
}

/* State 0x1B: the character's one-off animation (speed 5) with the normal palette for 0x41 frames, then
   grey_screen_request cleared and standing (state 0). Projected and drawn at its base. */
void player_state_short_animation(void)
{
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, state_1b_animation = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		state_1b_animation = sonic_state_1b_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			state_1b_animation = ray_state_1b_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				state_1b_animation = mighty_state_1b_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = state_1b_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 5u;
		OBJECT(current_object)->animation_timer = 0;
		PLAYER(current_player)->state_timer = 0x41;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&ignored, &ignored_2, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
			OBJECT(current_object)->animation_frame = 0xFFu;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->grey_screen_request = 0u;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
	}
	project_object(&ignored_3, &ignored);
	ignored = queue_sprite_at_base(ignored);
}

/* State 0x1A: returns at once. */
void player_state_inactive_2(void)
{
}

/* State 0x17, a hop: on the first frame speed_z -0x80000 (by speed from hop_speeds_by_speed on floor
   type 6) and the normal palette; then bounce_off_wall_in_air, jump_delay 2, the running animation and
   gravity 0x6000. Landing: state 1, z on the floor, jump_delay 0, flags bits 13, 20 and 21 cleared. */
void player_animate_hopping(void)
{
	uint32_t ignored = 0, ignored_2 = 0, running_animations = 0, speed_int = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x80000;
		if (OBJECT(current_object)->floor_type == 6u)
		{
			speed_int = (uint16_t)OBJECT(current_object)->speed_int;
			OBJECT(current_object)->speed_z_int = hop_speeds_by_speed[speed_int >> 8];
		}
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->animation_speed = 1u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	bounce_off_wall_in_air();
	PLAYER(current_player)->jump_delay = 2;
	running_animations = sonic_running_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		running_animations = ray_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			running_animations = mighty_running_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = running_animations + PLAYER_FACING_ANIMATION_OFFSETS(running_animations)[PLAYER(current_player)->facing];
	animate_with_flags(&ignored, &ignored_2);
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
	{
		PLAYER(current_player)->state = 1u;
		PLAYER(current_player)->substate = 0u;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		PLAYER(current_player)->jump_delay = 0;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~0x100000u;
		OBJECT(current_object)->flags &= ~0x200000u;
	}
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	if (OBJECT(current_object)->floor_block >= 0)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

/* State 0x16: the character's dying frame flies up off the screen from where it was (screen x kept
   within 0x20-0x100), swaying (direction +-7 a frame, kept in the wall field) and speeding up after 0x40
   frames, blinking. Past screen y -0x28: player_status 0xFF, in_game 0 and the continue countdown (state
   0x23, continue_seconds 9). */
void player_state_dying(void)
{
	uint16_t character_frame = 0;
	uint32_t depth = 0, direction = 0, ignored = 0, speed_x_int = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 1u;
		PLAYER(current_player)->screen_x_int = OBJECT(current_object)->screen_x;
		PLAYER(current_player)->screen_x_fraction = 0u;
		if (PLAYER(current_player)->screen_x_int < 0x20)
			PLAYER(current_player)->screen_x_int = 0x20;
		if (PLAYER(current_player)->screen_x_int > 0x100)
			PLAYER(current_player)->screen_x_int = 0x100;
		OBJECT(current_object)->projected_y_int = OBJECT(current_object)->screen_y;
		if (OBJECT(current_object)->projected_y_int > 0xE0)
		{
			OBJECT(current_object)->projected_y_int = 0xE0;
			if (PLAYER(current_player)->screen_x_int < 0x78)
				PLAYER(current_player)->screen_x_int = 0x78;
			if (PLAYER(current_player)->screen_x_int > 0xC8)
				PLAYER(current_player)->screen_x_int = 0xC8;
		}
		OBJECT(current_object)->projected_y_fraction = 0u;
		OBJECT(current_object)->direction = 0u;
		OBJECT(current_object)->speed_y = -0x10000;
		PLAYER(current_player)->state_timer = 0x40;
		OBJECT(current_object)->speed_x = 0x60000;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->wall = 7u;
		if (PLAYER(current_player)->screen_x_int < 0xA0)
		{
			OBJECT(current_object)->sprite_flags |= 0x40u;
			OBJECT(current_object)->wall = 0u - OBJECT(current_object)->wall;
		}
		character_frame = 0x1B50u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_frame = 0x5270u;
			if (OBJECT(current_object)->kind != 1u)
				character_frame = 0x36B8u;
		}
		OBJECT(current_object)->sprite_frame = character_frame;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->flags |= 0x400u;
		player_status[OBJECT(current_object)->player] = 1u;
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->speed_x += 0x8000u;
	}
	speed_x_int = (uint16_t)OBJECT(current_object)->speed_x_int;
	OBJECT(current_object)->direction += OBJECT(current_object)->wall;
	direction = OBJECT(current_object)->direction;
	PLAYER(current_player)->screen_x += sine_table[direction & 0x3FFu] * speed_x_int;
	OBJECT(current_object)->screen_x = PLAYER(current_player)->screen_x_int;
	if (PLAYER(current_player)->state_timer <= 0)
		OBJECT(current_object)->speed_y -= 0x3000u;
	OBJECT(current_object)->projected_y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->screen_y = OBJECT(current_object)->projected_y_int;
	if (OBJECT(current_object)->screen_y <= -0x28)
	{
		player_status[OBJECT(current_object)->player] = 0xFFu;
		PLAYER(current_player)->previous_z_low = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
	}
	PLAYER(current_player)->invulnerable_timer = 0x3C;
	toggle_hidden();
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	depth = 0xE0u;
	queue_sprite_at_depth(&ignored, &depth);
}

/* States 0xF-0x12, launched: by substate 0 a launch toward direction 0x100, 1 toward 0x200
   (player_launch), 2 and 3 the flight (player_launched_fly), 4 the landing; then animated, projected
   with sprite_offset_y 0x12, kept on screen, on the lists and drawn at its floor (or the previous one). */
void player_state_launched(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_launch_angle_100();
		break;
	case 1:
		player_launch_angle_200();
		break;
	case 2:
		player_launched_fly();
		break;
	case 3:
		player_launched_fly();
		break;
	case 4:
		player_launched_land();
		break;
	case 5:
		player_launch_angle_200();
		break;
	default:
		invalid_jump(0x5437u + PLAYER(current_player)->substate * 3u);
	}
	animate_with_flags(&ignored, &ignored_2);
	saved_sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->sprite_offset_y = 0x12;
	project_object(&ignored_3, &ignored);
	OBJECT(current_object)->sprite_offset_y = saved_sprite_offset_y;
	keep_player_on_screen(ignored);
	add_player_to_lists();
	if (OBJECT(current_object)->floor_block >= 0)
	{
		ignored = queue_sprite_at_floor();
		return;
	}
	ignored = queue_sprite_at_previous_floor();
}

/* Unused: an empty return left after player_state_launched's code; it does nothing. */
void player_state_launched_spare_return(void)
{
}

/* Substate 1 of states 0xF-0x12: direction 0x200, then player_launch. */
void player_launch_angle_200(void)
{
	OBJECT(current_object)->direction = 0x200u;
	player_launch();
}

/* Substate 0 of states 0xF-0x12: direction 0x100, then player_launch (which goes on to substate 2). */
void player_launch_angle_100(void)
{
	OBJECT(current_object)->direction = 0x100u;
	PLAYER(current_player)->substate = 1u;
	player_launch();
}

/* Launches the player along direction: speed_z -0x100000, speed 0x5000000, facing it, the attack jump
   animation's first step, flags bits 6 and 13 cleared and 12 set, the normal palette, control_lock 0,
   the next substate; then player_launched_fly. */
void player_launch(void)
{
	uint32_t animation_record = 0;

	face_movement_direction(0u);
	OBJECT(current_object)->speed_z = -0x100000;
	OBJECT(current_object)->speed = 0x5000000;
	set_speed_from_direction();
	animation_record = sonic_attack_jump_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		animation_record = ray_attack_jump_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			animation_record = mighty_attack_jump_animations_ADDRESS;
	}
	animation_record += PLAYER_FACING_ANIMATION_OFFSETS(animation_record)[PLAYER(current_player)->facing];
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xC0u) | FLAGGED_STEP(animation_record)->flags;
	animation_record += 2u;
	OBJECT(current_object)->sprite_frame = FLAGGED_STEP(animation_record - 0x2u)->frame;
	OBJECT(current_object)->animation = animation_record + 2u;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x1000u;
	PLAYER(current_player)->substate++;
	player_launched_fly();
}

/* The launch's flight (substates 2 and 3): every speed turned round at screen_x 0x120 or more, gravity
   0xA000, moves (previous_x/_y kept), find_floor; landing on a floor block: the next substate, z on the
   floor, speed_x/_y halved and speed_z turned round and halved. */
void player_launched_fly(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	if (OBJECT(current_object)->screen_x >= 0x120)
	{
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		OBJECT(current_object)->speed_y = 0u - OBJECT(current_object)->speed_y;
		OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
	}
	OBJECT(current_object)->speed_z += 0xA000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	if (OBJECT(current_object)->player_view.floor_block_low >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
		OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
		OBJECT(current_object)->speed_z = 0u - ((int32_t)OBJECT(current_object)->speed_z >> 1);
	}
}

/* The launch's end (substate 4): state 1, every speed and animation_speed 0, flags bit 12 cleared. */
void player_launched_land(void)
{
	OBJECT(current_object)->speed_z = 0;
	PLAYER(current_player)->state = 1u;
	PLAYER(current_player)->substate = 0u;
	OBJECT(current_object)->flags &= ~0x1000u;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->animation_speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
}

/* State 0xD, hurt (contacts once hurt_contact_delay is out), by substate: 0 takes 4 health (not in stage
   5; state 0x16 at 0), 1 (flags bit 24) none, then player_hurt_knockback; 2 flies back; 3 lies
   state_timer frames, 4 hops, 5 gets up; 6 waits state_timer frames (the button or the trackball cut it
   short), then walks; 7 and 8 the shock, then flying back. */
void player_state_hurt(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, gravity = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, kind = 0, knock_speed = 0, object = 0, shocked_animations = 0;
	uint8_t hurt_contact_delay = 0, health = 0, health_2 = 0;
	uint16_t state_timer = 0, state_timer_2 = 0, state_timer_3 = 0;

	hurt_contact_delay = PLAYER(current_player)->hurt_contact_delay;
	PLAYER(current_player)->hurt_contact_delay = hurt_contact_delay - 1u;
	if ((int8_t)hurt_contact_delay <= 1)
	{
		PLAYER(current_player)->hurt_contact_delay = 0;
		check_zone_contact();
		check_player_contacts(&depth, &cursor);
		if (PLAYER(current_player)->state == 0x28u)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
	}
	push_player_out_of_wall(&ignored, &depth, &cursor);
	PLAYER(current_player)->state = 0xDu;
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 9:
		if ((uint8_t)OBJECT(current_object)->health == 0u)
		{
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			player_dispatch_state(depth, cursor);
			return;
		}
		if (stage != 5u)
		{
			health = OBJECT(current_object)->health;
			OBJECT(current_object)->health = health - 4u;
			if ((int8_t)health <= 4)
				OBJECT(current_object)->health = 0;
		}
		PLAYER(current_player)->invulnerable_timer = 0x258;
		OBJECT(current_object)->flags &= ~0x1000000u;
		player_hurt_knockback(cursor);
		return;
	case 1:
		OBJECT(current_object)->flags |= 0x1000000u;
		player_hurt_knockback(cursor);
		return;
	case 2:
		player_hurt_fly(depth, cursor);
		return;
	case 3:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer > 1)
		{
			if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
				flash_hurt_palette();
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
			project_object(&ignored, &ignored_2);
			keep_player_on_screen(ignored_2);
			add_player_to_lists();
			ignored_2 = queue_sprite_at_base(ignored_2);
			return;
		}
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z = -0x40000;
		if (OBJECT(current_object)->floor_type == 4u)
			OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		add_player_to_lists();
		ignored_2 = queue_sprite_at_base(ignored_2);
		return;
	case 4:
		gravity = 0x6000u;
		if (OBJECT(current_object)->floor_type == 4u)
			gravity = gravity >> 2;
		OBJECT(current_object)->speed_z += gravity;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			player_hurt_land_after_hop();
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate_with_flags(&depth, &ignored_6);
			if (OBJECT(current_object)->animation_frame == 5u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored, &depth, &ignored_3, &ignored_4, &cursor);
		if (current_game_mode != 0x1Au)
		{
			check_fall_into_pit();
			if (PLAYER(current_player)->state == 9u)
			{
				OBJECT(current_object)->animation_frame = 0xFFu;
				player_dispatch_state(depth, cursor);
				return;
			}
		}
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		add_player_to_lists();
		ignored_2 = queue_sprite_at_floor();
		return;
	case 5:
		player_hurt_get_up();
		return;
	case 6:
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
	case 7:
		if ((uint8_t)OBJECT(current_object)->health == 0u)
		{
			PLAYER(current_player)->state = 0x16u;
			PLAYER(current_player)->substate = 0u;
			player_dispatch_state(depth, cursor);
			return;
		}
		health_2 = OBJECT(current_object)->health;
		OBJECT(current_object)->health = health_2 - 4u;
		if ((int8_t)health_2 <= 4)
			OBJECT(current_object)->health = 0;
		PLAYER(current_player)->invulnerable_timer = 0x258;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~0x40u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->state_timer = 4;
		OBJECT(current_object)->speed_z = -0x80000;
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		if (OBJECT(object)->kind != 0x15u)
		{
			knock_speed = 0x40000u;
			switch (OBJECT(object)->spawn_arg_2_high)
			{
			case 0:
			case 4:
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = 0u - knock_speed;
				OBJECT(current_object)->direction = 0x200u;
				break;
			case 1:
				OBJECT(current_object)->speed_x = 0;
				OBJECT(current_object)->speed_y = knock_speed;
				OBJECT(current_object)->direction = 0u;
				break;
			case 2:
				OBJECT(current_object)->speed_x = knock_speed;
				OBJECT(current_object)->speed_y = 0;
				OBJECT(current_object)->direction = 0x300u;
				break;
			case 3:
				OBJECT(current_object)->speed_x = 0u - knock_speed;
				OBJECT(current_object)->speed_y = 0;
				OBJECT(current_object)->direction = 0x100u;
				break;
			default:
				invalid_jump(0x5AE7u + OBJECT(object)->spawn_arg_2_high * 2u);
			}
		}
		else
		{
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0;
			OBJECT(current_object)->direction = 0x300u;
		}
		face_movement_direction(ignored);
		shocked_animations = sonic_shocked_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			shocked_animations = ray_shocked_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				shocked_animations = mighty_shocked_animations_ADDRESS;
		}
		depth = PLAYER_FACING_ANIMATION_OFFSETS(shocked_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation = shocked_animations + depth;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x18u;
		play_sound(sound_id, &depth, &ignored_3);
		sound_id = 0x2Fu;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &depth, &ignored_3);
		animate_with_flags(&depth, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			state_timer_3 = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer_3 - 1u;
			if ((int16_t)state_timer_3 <= 1)
			{
				PLAYER(current_player)->substate = 2u;
				OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
				player_hurt_set_animation(cursor);
				return;
			}
		}
		kind = OBJECT(current_object)->kind;
		OBJECT(current_object)->sprite_palette = shocked_palettes[kind & 0xFFFu].palettes[OBJECT(current_object)->animation_frame];
		update_screen_position(&ignored, &depth);
		keep_player_on_screen(depth);
		add_player_to_lists();
		depth = queue_sprite_at_base(depth);
		return;
	case 8:
		animate_with_flags(&depth, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			state_timer_3 = PLAYER(current_player)->state_timer;
			PLAYER(current_player)->state_timer = state_timer_3 - 1u;
			if ((int16_t)state_timer_3 <= 1)
			{
				PLAYER(current_player)->substate = 2u;
				OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
				player_hurt_set_animation(cursor);
				return;
			}
		}
		kind = OBJECT(current_object)->kind;
		OBJECT(current_object)->sprite_palette = shocked_palettes[kind & 0xFFFu].palettes[OBJECT(current_object)->animation_frame];
		update_screen_position(&ignored, &depth);
		keep_player_on_screen(depth);
		add_player_to_lists();
		depth = queue_sprite_at_base(depth);
		return;
	default:
		invalid_jump(0x55C8u + PLAYER(current_player)->substate * 3u);
	}
}

/* State 0xD substate 0 or 1: flags bits 2, 6 and 13 cleared, mishap_flag, hurt_contact_delay 0xA,
   substate 2, speed_z -0x80000 and a push of 0x40000 along the toucher's direction (none from kind 0x24;
   both halved on floor type 4); the player faces it, sound 0x21 and the character's voice, then
   player_hurt_set_animation. */
void player_hurt_knockback(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, knock_direction = 0, knock_speed_x = 0, knock_speed_y = 0, object = 0;
	uint32_t push_speed = 0, push_speed_2 = 0;

	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->mishap_flag = 0xFFu;
	PLAYER(current_player)->hurt_contact_delay = 0xA;
	PLAYER(current_player)->substate = 2u;
	OBJECT(current_object)->speed_z = -0x80000;
	if (OBJECT(current_object)->floor_type == 4u)
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if (OBJECT(object)->kind != 0x24u)
	{
		knock_speed_x = 0x40000u;
		if (OBJECT(current_object)->floor_type == 4u)
			knock_speed_x = (int32_t)knock_speed_x >> 1;
		knock_speed_y = 0u;
		knock_direction = OBJECT(object)->direction;
		OBJECT(current_object)->direction = knock_direction;
		OBJECT(current_object)->speed = knock_speed_x << 8;
		if (knock_direction != 0u)
		{
			if (knock_direction != 0x200u)
			{
				if (knock_direction != 0x100u)
					knock_speed_x = 0u - knock_speed_x;
			}
			else
			{
				push_speed = knock_speed_x;
				knock_speed_x = knock_speed_y;
				knock_speed_y = push_speed;
			}
		}
		else
		{
			push_speed_2 = knock_speed_x;
			knock_speed_x = knock_speed_y;
			knock_speed_y = 0u - push_speed_2;
		}
	}
	else
	{
		knock_speed_x = 0u;
		knock_speed_y = 0u;
	}
	OBJECT(current_object)->speed_x = knock_speed_x;
	OBJECT(current_object)->speed_y = knock_speed_y;
	face_movement_direction(knock_direction);
	sound_id = 0x21u;
	play_sound(sound_id, &ignored, &ignored_2);
	sound_id = 0x55u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x4Au;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x5Bu;
	}
	play_sound(sound_id, &ignored, &ignored_2);
	player_hurt_set_animation(cursor);
}

/* State 0xD: the character's hurt animation by facing from its first step (flips and frame), the hurt
   flash every 3 frames (none, 0xFF, with flags bit 24), then player_hurt_fly. */
void player_hurt_set_animation(uint32_t cursor)
{
	uint32_t facing_offset = 0, hurt_animations = 0, step_record = 0;

	hurt_animations = sonic_hurt_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		hurt_animations = ray_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			hurt_animations = mighty_hurt_animations_ADDRESS;
	}
	facing_offset = PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
	OBJECT(current_object)->animation = hurt_animations + facing_offset;
	OBJECT(current_object)->animation_frame = 0u;
	step_record = OBJECT(current_object)->animation + 2u;
	OBJECT(current_object)->sprite_flags &= ~0xC0u;
	OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
	OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
	OBJECT(current_object)->player_view.hurt_flash_period = 3u;
	if ((OBJECT(current_object)->flags & 0x1000000u) != 0u)
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
	OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
	OBJECT(current_object)->player_view.hurt_flash_on = 0u;
	player_hurt_fly(facing_offset, cursor);
}

/* State 0xD substate 2, knocked back: gravity 0x6000 (/4 on floor type 4 unless the toucher is kind
   0x27), moves, find_floor, check_fall_into_pit (state 9 dispatched); a floor 8 or more above sends it
   back to its previous x, y and floor. Landing: substate 3, state_timer 2, sound 0x20, the animation's
   next step; drawn at its floor. */
void player_hurt_fly(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t gravity = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, step_record = 0;

	gravity = 0x6000u;
	if (OBJECT(current_object)->floor_type == 4u && OBJECT(LINKED(current_object, OBJECT(current_object)->touched_by))->kind != 0x27u)
		gravity = gravity >> 2;
	OBJECT(current_object)->speed_z += gravity;
	PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	find_floor(&ignored, &depth, &ignored_2, &ignored_3, &cursor);
	check_fall_into_pit();
	if (PLAYER(current_player)->state == 9u)
	{
		OBJECT(current_object)->animation_frame = 0xFFu;
		player_dispatch_state(depth, cursor);
		return;
	}
	if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 8)
	{
		OBJECT(current_object)->x = PLAYER(current_player)->previous_x;
		OBJECT(current_object)->y = PLAYER(current_player)->previous_y;
		OBJECT(current_object)->floor_block = OBJECT(current_object)->previous_floor_block;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->previous_floor_z;
	}
	if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->state_timer = 2;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		sound_id = 0x20u;
		play_sound(sound_id, &ignored_4, &ignored_2);
		OBJECT(current_object)->animation_frame++;
		step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
		add_player_to_lists();
		project_object(&ignored, &ignored_4);
		if (current_game_mode != 0xDu)
			keep_player_on_screen(ignored_4);
		ignored_4 = queue_sprite_at_floor();
		return;
	}
	if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
	{
		OBJECT(current_object)->animation_frame++;
		step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
	}
	add_player_to_lists();
	project_object(&ignored, &ignored_4);
	if (current_game_mode != 0xDu)
		keep_player_on_screen(ignored_4);
	ignored_4 = queue_sprite_at_floor();
}

/* State 0xD substate 4 lands: substate 5, z on the floor, animation_timer 0, then player_hurt_get_up. */
void player_hurt_land_after_hop(void)
{
	PLAYER(current_player)->substate++;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->animation_timer = 0;
	player_hurt_get_up();
}

/* State 0xD substate 5: the get-up animation (speed 3); at its end substate 6, mishap_flag 0, the normal
   palette, state_timer 0x1E and, unless flags bit 24, invulnerable_timer 0x78 with the blink (flags bit
   29); state 0x16 at health 0. Each frame the hurt flash, z on the floor, drawn at its base. */
void player_hurt_get_up(void)
{
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->animation_speed = 3u;
	animate_with_flags(&ignored, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored_2, &ignored, &ignored_3, &ignored_4, &ignored_5);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		project_object(&ignored_2, &ignored);
		keep_player_on_screen(ignored);
		add_player_to_lists();
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->substate++;
	PLAYER(current_player)->mishap_flag = 0u;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	set_floor_sprite_priority();
	OBJECT(current_object)->player_view.hurt_flash_timer = 0x7Fu;
	PLAYER(current_player)->state_timer = 0x1E;
	if ((OBJECT(current_object)->flags & 0x1000000u) == 0u)
	{
		PLAYER(current_player)->invulnerable_timer = 0x78;
		OBJECT(current_object)->flags |= 0x20000000u;
	}
	if (OBJECT(current_object)->health > 0)
	{
		if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
			flash_hurt_palette();
		find_floor(&ignored_2, &ignored, &ignored_3, &ignored_4, &ignored_5);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		project_object(&ignored_2, &ignored);
		keep_player_on_screen(ignored);
		add_player_to_lists();
		ignored = queue_sprite_at_base(ignored);
		return;
	}
	PLAYER(current_player)->state = 0x16u;
	PLAYER(current_player)->substate = 0u;
	if ((int8_t)OBJECT(current_object)->player_view.hurt_flash_period >= 0)
		flash_hurt_palette();
	find_floor(&ignored_2, &ignored, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored_2, &ignored);
	keep_player_on_screen(ignored);
	add_player_to_lists();
	ignored = queue_sprite_at_base(ignored);
}

/* Unused: an empty return left after player_state_hurt's code; it does nothing. */
void player_state_hurt_spare_return(void)
{
}

/* State 0xE, bouncing off a wall or an object: the bounce record for bounce_direction, speed 0x1000000,
   speed_z -0x68000, sound 0x25; a hop and a rebound at half speed_z (gravity 0x8000), the animation
   stepping as speed_z passes bounce_speed_z; then up to 0x14 frames on the floor (the button or
   trackball end them) and the headings turned round. */
void player_state_bouncing(void)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, bounce_record = 0, bounce_records = 0, gravity = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, new_heading = 0, rebound_speed = 0;
	uint16_t state_timer = 0;

	push_player_out_of_wall(&ignored, &ignored_2, &ignored_3);
	if ((OBJECT(current_object)->flags & 0x20u) != 0u)
		PLAYER(current_player)->state = 0xEu;
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 4:
		PLAYER(current_player)->substate++;
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
		face_movement_direction(ignored);
		OBJECT(current_object)->speed = 0x1000000;
		OBJECT(current_object)->speed_z = -0x68000;
		PLAYER(current_player)->bounce_speed_z = -0x68000;
		PLAYER(current_player)->bounce_step = 0x34000;
		PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
		set_speed_from_direction();
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[0];
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_timer = 3;
		OBJECT(current_object)->animation_speed = 3u;
		PLAYER(current_player)->state_timer = 0x14;
		sound_id = 0x25u;
		play_sound(sound_id, &ignored_2, &ignored_4);
		add_player_to_lists();
		ignored_2 = queue_sprite_at_floor();
		return;
	case 1:
	case 2:
		gravity = 0x8000u;
		if (OBJECT(current_object)->floor_type == 4u)
			gravity = gravity >> 2;
		OBJECT(current_object)->speed_z += gravity;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		find_floor(&ignored, &ignored_2, &ignored_4, &ignored_5, &ignored_3);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			rebound_speed = (int32_t)OBJECT(current_object)->speed_z >> 1;
			OBJECT(current_object)->speed_z = 0u - rebound_speed;
			PLAYER(current_player)->bounce_speed_z = 0u - rebound_speed;
			PLAYER(current_player)->bounce_step = (int32_t)(rebound_speed * 2u) / 3;
			PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
			if ((int8_t)OBJECT(current_object)->animation_frame < 6)
			{
				OBJECT(current_object)->animation_frame++;
				OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[OBJECT(current_object)->animation_frame];
			}
			update_player_blink();
			add_to_rider_list();
			project_object(&ignored, &ignored_2);
			keep_player_on_screen(ignored_2);
			if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
				return;
			ignored_2 = queue_sprite_at_floor();
			return;
		}
		if (PLAYER(current_player)->bounce_speed_z <= OBJECT(current_object)->speed_z)
		{
			PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
			if ((int8_t)OBJECT(current_object)->animation_frame < 6)
			{
				OBJECT(current_object)->animation_frame++;
				OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[OBJECT(current_object)->animation_frame];
			}
		}
		update_player_blink();
		add_to_rider_list();
		project_object(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored_2 = queue_sprite_at_floor();
		return;
	case 3:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer > 1)
		{
			if ((int8_t)OBJECT(current_object)->animation_speed < 0)
			{
				start_jump_if_pressed();
				if (PLAYER(current_player)->state == 4u)
				{
					OBJECT(current_object)->flags &= ~4u;
					PLAYER(current_player)->substate = 0u;
					OBJECT(current_object)->speed = 0;
					PLAYER(current_player)->animation_speed = 0;
					new_heading = (PLAYER(current_player)->target_heading + 0x200u) & 0x3FFu;
					PLAYER(current_player)->target_heading = new_heading;
					PLAYER(current_player)->heading = new_heading;
					PLAYER(current_player)->facing_angle = new_heading;
					set_facing(new_heading);
				}
				else
				{
					read_player_trackball();
					if ((PLAYER(current_player)->trackball_x | PLAYER(current_player)->trackball_y) != 0u)
					{
						PLAYER(current_player)->state = 1u;
						OBJECT(current_object)->flags &= ~4u;
						PLAYER(current_player)->substate = 0u;
						OBJECT(current_object)->speed = 0;
						PLAYER(current_player)->animation_speed = 0;
						new_heading = (PLAYER(current_player)->target_heading + 0x200u) & 0x3FFu;
						PLAYER(current_player)->target_heading = new_heading;
						PLAYER(current_player)->heading = new_heading;
						PLAYER(current_player)->facing_angle = new_heading;
						set_facing(new_heading);
					}
				}
			}
		}
		else
		{
			PLAYER(current_player)->state = 0u;
			OBJECT(current_object)->flags &= ~4u;
			PLAYER(current_player)->substate = 0u;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->animation_speed = 0;
			new_heading = (PLAYER(current_player)->target_heading + 0x200u) & 0x3FFu;
			PLAYER(current_player)->target_heading = new_heading;
			PLAYER(current_player)->heading = new_heading;
			PLAYER(current_player)->facing_angle = new_heading;
			set_facing(new_heading);
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate(&ignored_2, &ignored_6, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		update_player_blink();
		update_screen_position(&ignored, &ignored_2);
		keep_player_on_screen(ignored_2);
		if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
			return;
		ignored_2 = queue_sprite_at_base(ignored_2);
		return;
	default:
		invalid_jump(0x5C0Fu + PLAYER(current_player)->substate * 3u);
	}
}
