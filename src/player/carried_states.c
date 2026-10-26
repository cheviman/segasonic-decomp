/**
 *    Name:        player/carried_states.c
 *    Description: the player's states while the stage carries or holds them: thrown, carried off, trapped, riding, the final stage, tripped, swept away, climbing an object, pulled, held still, stuck on an object, caught, sinking, knocked off
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* State 0x19 (game mode 0x0D), by scene_step: at each new step 0, 1 or 3 the player is lifted 0xA0 +
   0x20 * kind and falls hurt (state 0xD, substate 2); at step 2 it is thrown (speed_z -0xE0000, speed_y
   0x80000) with a cry. Between steps it stands on its floor, or stays hurt. */
void player_state_scene_thrown(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t hurt_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, step_record = 0;

	switch (scene_step)
	{
	case 0:
	case 1:
	case 3:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			if (PLAYER(current_player)->previous_state == 0xDu)
			{
				player_state_hurt(depth, cursor);
				return;
			}
			find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			project_object(&ignored, &ignored_2);
			add_to_rider_list();
			player_animate_standing();
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		OBJECT(current_object)->z_int -= 0xA0u;
		OBJECT(current_object)->z_int -= (uint32_t)OBJECT(current_object)->kind << 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		depth = PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation = hurt_animations + depth;
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		step_record += 2u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
		player_state_hurt(depth, cursor);
		return;
	case 2:
		if (scene_step == PLAYER(current_player)->scene_step_seen)
		{
			player_state_hurt(depth, cursor);
			return;
		}
		PLAYER(current_player)->scene_step_seen = scene_step;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		OBJECT(current_object)->speed_z = -0xE0000;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0x80000;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(0u);
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		depth = PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation = hurt_animations + depth;
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		step_record += 2u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &depth, &ignored_3);
		player_state_hurt(depth, cursor);
		return;
	default:
		invalid_jump(0x7C18u + scene_step * 3u);
	}
}

/* State 0x31 (stage 8): runs as previous_state while it has a floor; off the edge it falls, pulled
   toward scene_object, to z 0x9D0 and lands hurt. Step 4 sets burning_boulder_released and stands until
   seesaw_launched throws it up off the screen; it flies to its player_carriers object and at z 0x530 is
   trapped (state 0x32). */
void player_state_carried_off(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0, sound_number = 0;
	uint32_t at_last_step = 0, hurt_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, near_edge_fall_animation = 0, object = 0, seesaw_launch_animation = 0;

	switch (PLAYER(current_player)->sequence_step)
	{
	case 0:
	case 7:
		PLAYER(current_player)->sequence_step++;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed = 0x5000000;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->path_request = 0x2Fu;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~4u;
		OBJECT(current_object)->flags &= ~0x8000000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->invulnerable_timer = 0;
		PLAYER(current_player)->previous_state = 0x13u;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	case 1:
		if (OBJECT(current_object)->player_view.floor_block_high >= 0)
		{
			PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
			player_move(depth, cursor);
			return;
		}
		PLAYER(current_player)->sequence_step++;
		near_edge_fall_animation = sonic_near_edge_fall_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			near_edge_fall_animation = ray_near_edge_fall_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				near_edge_fall_animation = mighty_near_edge_fall_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = near_edge_fall_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		sound_number = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_number = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_number = 0x77u;
		}
		play_sound(sound_number, &ignored, &ignored_2);
	case 2:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			at_last_step = animate_and_move(0u);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		if (OBJECT(current_object)->z_int >= 0x9C0)
		{
			OBJECT(current_object)->z += 0x40000u;
			if (OBJECT(current_object)->z_int >= 0x9D0)
				PLAYER(current_player)->sequence_step++;
			find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite(ignored);
			return;
		}
		object = LINKED(current_object, scene_object);
		if ((uint16_t)object != 0xFFFFu)
		{
			OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 4;
			OBJECT(current_object)->y += (int32_t)(OBJECT(object)->y + ((uint32_t)OBJECT(current_object)->player << 21) - OBJECT(current_object)->y) >> 4;
			OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
			if (OBJECT(current_object)->z_int >= 0x9D0)
				PLAYER(current_player)->sequence_step++;
			find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite(ignored);
			return;
		}
		OBJECT(current_object)->z += 0x40000u;
		if (OBJECT(current_object)->z_int >= 0x9D0)
			PLAYER(current_player)->sequence_step++;
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite(ignored);
		return;
	case 3:
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->substate = 3u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		OBJECT(current_object)->flags &= ~4u;
		OBJECT(current_object)->flags &= ~0x8000000u;
		PLAYER(current_player)->state_timer = 2;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		PLAYER(current_player)->path_request = 0u;
		PLAYER(current_player)->path = 0u;
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = hurt_animations + PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation_frame = 2u;
		depth = OBJECT(current_object)->animation_frame;
		cursor = OBJECT(current_object)->animation + 2u + depth * 4u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(cursor)->flags;
		cursor += 2u;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(cursor - 0x2u)->frame;
	case 4:
		if (PLAYER(current_player)->previous_state == 0xDu)
		{
			PLAYER(current_player)->state = 0xDu;
			player_state_hurt(depth, cursor);
			return;
		}
		burning_boulder_released = 0xFFu;
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z_fraction -= 0x10u;
		project_object(&ignored_3, &depth);
		check_player_stalled();
		if (PLAYER(current_player)->state == 0xDu)
		{
			player_state_hurt(depth, cursor);
			return;
		}
		if (seesaw_launched == 0u)
		{
			player_animate_standing();
			return;
		}
		PLAYER(current_player)->sequence_step++;
		seesaw_launch_animation = sonic_seesaw_launch_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			seesaw_launch_animation = ray_seesaw_launch_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				seesaw_launch_animation = mighty_seesaw_launch_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = seesaw_launch_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->z -= 0xC0000u;
		animate(&ignored, &ignored_6, &ignored_7);
		project_object(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0)
			PLAYER(current_player)->sequence_step++;
		ignored = queue_sprite(ignored);
		return;
	case 5:
		OBJECT(current_object)->z -= 0xC0000u;
		animate(&ignored, &ignored_6, &ignored_7);
		project_object(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0)
			PLAYER(current_player)->sequence_step++;
		ignored = queue_sprite(ignored);
		return;
	case 6:
		object = (current_object & ~0xFFFFu) | player_carriers[OBJECT(current_object)->player];
		OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 4;
		OBJECT(current_object)->y += (int32_t)((uint32_t)OBJECT(object)->y - OBJECT(current_object)->y) >> 4;
		OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
		animate(&ignored, &ignored_6, &ignored_7);
		project_object(&ignored_3, &ignored);
		if (OBJECT(current_object)->z_int <= 0x530)
		{
			PLAYER(current_player)->state = 0x32u;
			PLAYER(current_player)->substate = 0u;
		}
		ignored = queue_sprite(ignored);
		return;
	default:
		invalid_jump(0x7DB5u + PLAYER(current_player)->sequence_step * 3u);
	}
}

/* State 0x32: two trap objects (trap_flash_start, trap_start) and sound 0x21, the trapped animation,
   then a sign (kind 0x14) and the struggle: trackball speed above 0x100 fills escape_progress (else it
   drains), struggle frames by it; at 4 the player jumps out, bounces once and stands (state 0). Tumbling
   (state 0x20) leaves at once. */
void player_state_trapped(uint32_t depth, uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t at_last_step = 0, character_frame = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, running_animations = 0, speed_int = 0, struggle_index = 0, trackball_speed = 0, trackball_y = 0, trap_struggle_animations = 0, trapped_animation = 0;
	uint32_t escape_progress = 0;

	check_player_contacts(&depth, &cursor);
	if (PLAYER(current_player)->state == 0x20u)
	{
		player_dispatch_state(depth, cursor);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 6:
		PLAYER(current_player)->substate++;
		trapped_animation = sonic_trapped_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			trapped_animation = ray_trapped_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				trapped_animation = mighty_trapped_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = trapped_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		allocate_object(&free_count, &cursor);
		if (free_count != 0u)
		{
			OBJECT(cursor)->sprite_flags = 0x1000u;
			OBJECT(cursor)->sprite_frame = 0xB4F0u;
			OBJECT(cursor)->sprite_palette = 0xD10u;
			OBJECT(cursor)->flags = 0u;
			OBJECT(cursor)->sprite_offset_x = 0;
			OBJECT(cursor)->sprite_offset_y = 0;
			OBJECT(cursor)->handler = trap_flash_start_ADDRESS;
			OBJECT(cursor)->x = OBJECT(current_object)->x;
			OBJECT(cursor)->y = OBJECT(current_object)->y;
			OBJECT(cursor)->z = OBJECT(current_object)->z;
			OBJECT(cursor)->animation_speed = 8u;
			allocate_object(&free_count, &cursor);
			if (free_count != 0u)
			{
				OBJECT(cursor)->sprite_flags = 0x1000u;
				OBJECT(cursor)->sprite_frame = 0xB4F8u;
				OBJECT(cursor)->sprite_palette = 0xD20u;
				OBJECT(cursor)->flags = 0u;
				OBJECT(cursor)->sprite_offset_x = 0;
				OBJECT(cursor)->sprite_offset_y = 2;
				OBJECT(cursor)->trap.player_block = OBJECT(current_object)->player_view.block;
				OBJECT(cursor)->x = OBJECT(current_object)->x;
				OBJECT(cursor)->y = OBJECT(current_object)->y;
				OBJECT(cursor)->z = OBJECT(current_object)->z;
				OBJECT(cursor)->handler = trap_start_ADDRESS;
				sound_id = 0x21u;
				play_sound(sound_id, &ignored, &ignored_2);
			}
		}
		return;
	case 1:
		animate(&ignored, &ignored_3, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 4u;
			PLAYER(current_player)->escape_progress = 0;
			x_int = (uint16_t)OBJECT(current_object)->x_int;
			y_int = (uint16_t)OBJECT(current_object)->y_int;
			kind = 0x14u;
			z = (uint16_t)(OBJECT(current_object)->z_int - 0x20u);
			spawn_sign_2(x_int, y_int, kind, z, cursor);
		}
		project_object(&ignored_4, &ignored);
		ignored = queue_sprite(ignored);
		return;
	case 2:
		read_player_trackball();
		trackball_speed = PLAYER(current_player)->trackball_x;
		trackball_y = PLAYER(current_player)->trackball_y;
		trackball_speed = trackball_speed * trackball_speed + trackball_y * trackball_y;
		square_root(&trackball_speed, &ignored);
		trackball_speed &= 0xFFFFu;
		trackball_speed = trackball_speed * trackball_speed;
		if ((int32_t)trackball_speed > 0x5000000)
			trackball_speed = 0x5000000u;
		OBJECT(current_object)->speed = trackball_speed;
		speed_int = (uint16_t)OBJECT(current_object)->speed_int;
		if ((int16_t)speed_int > 0x100)
			PLAYER(current_player)->escape_progress += speed_int * 4u;
		else
		{
			escape_progress = PLAYER(current_player)->escape_progress;
			PLAYER(current_player)->escape_progress = escape_progress - 0x2000u;
			if ((int32_t)escape_progress <= 0x2000)
				PLAYER(current_player)->escape_progress = 0;
		}
		trap_struggle_animations = sonic_trap_struggle_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			trap_struggle_animations = ray_trap_struggle_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				trap_struggle_animations = mighty_trap_struggle_animations_ADDRESS;
		}
		struggle_index = (uint16_t)PLAYER(current_player)->escape_progress_int;
		if ((int16_t)struggle_index >= 4)
			struggle_index = 4u;
		OBJECT(current_object)->animation = STRUGGLE_ANIMATION_POINTERS(trap_struggle_animations)[struggle_index];
		animate(&ignored, &ignored_3, &ignored_5);
		project_object(&ignored_4, &ignored);
		if (PLAYER(current_player)->escape_progress_int < 4)
		{
			add_player_to_lists();
			ignored = queue_sprite(ignored);
			return;
		}
		PLAYER(current_player)->substate++;
		character_frame = 0xB4E8u;
		if (OBJECT(current_object)->kind != 0u)
		{
			character_frame = 0xB580u;
			if (OBJECT(current_object)->kind != 1u)
				character_frame = 0xB468u;
		}
		OBJECT(current_object)->sprite_frame = character_frame;
		OBJECT(current_object)->animation_timer = 0x10;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->speed_x = 0x20000;
		OBJECT(current_object)->direction = 0x300u;
		face_movement_direction(character_frame);
		add_player_to_lists();
		ignored = queue_sprite(ignored);
		return;
	case 3:
	case 4:
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		running_animations = sonic_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			running_animations = ray_running_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				running_animations = mighty_running_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = running_animations + PLAYER_FACING_ANIMATION_OFFSETS(running_animations)[PLAYER(current_player)->facing];
		animate_with_flags(&ignored, &ignored_5);
		find_floor(&ignored_4, &ignored, &ignored_2, &ignored_6, &ignored_7);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->speed_z = -0x40000;
		}
		project_object(&ignored_4, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 5:
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->state = 0u;
		PLAYER(current_player)->substate = 0u;
		project_object(&ignored_4, &ignored);
		ignored = queue_sprite_at_base(ignored);
		return;
	default:
		invalid_jump(0x811Fu + PLAYER(current_player)->substate * 3u);
	}
}

/* Projects the trap's flash, then trap_flash_update. */
void trap_flash_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = trap_flash_update_ADDRESS;
}

/* The trap's flash: counts animation_speed down from 8 as a timer, drawn two frames out of four, then
   removed. */
void trap_flash_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0;
	uint8_t animation_speed = 0;

	update_screen_position(&ignored, &ignored_2);
	animation_speed = OBJECT(current_object)->animation_speed;
	OBJECT(current_object)->animation_speed = animation_speed - 1u;
	if ((int8_t)animation_speed <= 1)
	{
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->animation_speed & 2u) == 0u)
		return;
	ignored_2 = queue_sprite(ignored_2);
}

/* Projects the trap, then trap_update. */
void trap_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = trap_update_ADDRESS;
}

/* The trap: sprite 0xB500 once its player's substate is 3 or more (broken free); removed off screen. */
void trap_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, off_screen = 0;

	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)PLAYER(OBJECT(current_object)->trap.player_block)->substate >= 3)
		OBJECT(current_object)->sprite_frame = 0xB500u;
	update_screen_position(&ignored, &ignored_2);
	ignored_2 = queue_sprite(ignored_2);
}

/* State 0x35 (game mode 0x1C): the player hangs from its ride object (ridden_object; its platform plank
   instead when it started at x 0xA60-0xAE0), 0x20 lower and 0x10 along x, in the character's hanging
   frame; once that plank's flags bit 0 is set (the bridge has stopped) it starts climbing (start_climb). */
void player_state_riding(void)
{
	uint16_t character_frame = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0, object_2 = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		if (OBJECT(current_object)->x_int <= 0xAE0 && OBJECT(current_object)->x_int >= 0xA60)
			PLAYER(current_player)->ridden_object = 0u;
		if (OBJECT(current_object)->y_int > 0xE74)
		{
			if (OBJECT(current_object)->y_int >= 0xEF2)
				OBJECT(current_object)->y = 0xEF20000;
		}
		else
			OBJECT(current_object)->y = 0xE740000;
		if (OBJECT(current_object)->z_int < 0x530 || OBJECT(current_object)->z_int > 0x564)
			OBJECT(current_object)->z = 0x5300000;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
	}
	OBJECT(current_object)->flags &= ~8u;
	OBJECT(current_object)->flags &= ~0x10u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	if (PLAYER(current_player)->ridden_object != 0u)
	{
		object = (current_object & ~0xFFFFu) | PLAYER(current_player)->ridden_object;
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->z = OBJECT(object)->z;
	}
	else
	{
		object_2 = LINKED(current_object, OBJECT(current_object)->platform);
		OBJECT(current_object)->x = OBJECT(object_2)->x;
		OBJECT(current_object)->z = OBJECT(object_2)->z;
	}
	OBJECT(current_object)->z_int += 0x20u;
	OBJECT(current_object)->x_int += 0x10u;
	project_object(&ignored, &ignored_2);
	character_frame = 0xB2B8u;
	if (OBJECT(current_object)->kind != 0u)
	{
		character_frame = 0xB288u;
		if (OBJECT(current_object)->kind != 1u)
			character_frame = 0xB2E8u;
	}
	OBJECT(current_object)->sprite_frame = character_frame;
	OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	if ((OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->flags & 1u) == 0u)
	{
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	if (OBJECT(current_object)->y_int < 0xE77)
		OBJECT(current_object)->y = 0xE770000;
	if (OBJECT(current_object)->y_int > 0xED5)
		OBJECT(current_object)->y = 0xED50000;
	OBJECT(current_object)->x_int -= 0x10u;
	if (OBJECT(current_object)->x_int <= 0xC11)
		OBJECT(current_object)->x_int = 0xC19;
	start_climb();
	ignored_2 = queue_sprite(ignored_2);
}

/* State 0x2B: once ending_finished is set the player gets 0xC8 rings and one more stages_reached, leaves
   the game (in_game 0) and goes to the continue countdown (state 0x23, 9 seconds). */
void player_state_final_stage(void)
{
	if (ending_finished != 0u)
	{
		PLAYER(current_player)->previous_z_low = 0u;
		PLAYER(current_player)->in_game = 0;
		PLAYER(current_player)->continue_seconds = 9;
		PLAYER(current_player)->continue_frames = 0;
		PLAYER(current_player)->state = 0x23u;
		PLAYER(current_player)->rings += 0xC8u;
		PLAYER(current_player)->stages_reached++;
	}
}

/* State 0x29: tripped, the player hops (speed_z -0x80000) flailing with a cry, still touching zones and
   players; when the hop starts falling it goes hurt (state 0xD, substate 2) with the hurt animation. */
void player_state_tripped(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, flail_animation = 0, hurt_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, step_record = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed_z = -0x80000;
		PLAYER(current_player)->ground_speed = 0;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		flail_animation = flail_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			flail_animation = flail_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				flail_animation = flail_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = flail_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 0u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x47u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x4Fu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x67u;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		break;
	case 1:
		break;
	default:
		invalid_jump(0x85F7u + PLAYER(current_player)->substate * 3u);
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	animate_with_flags(&ignored, &ignored_3);
	if (OBJECT(current_object)->speed_z_int < 0)
	{
		project_object(&ignored_4, &depth);
		check_zone_contact();
		check_player_contacts(&depth, &cursor);
		add_player_to_lists();
		ignored = queue_sprite_at_floor();
		return;
	}
	PLAYER(current_player)->state = 0xDu;
	PLAYER(current_player)->substate = 2u;
	OBJECT(current_object)->flags |= 0x1000000u;
	hurt_animations = sonic_hurt_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		hurt_animations = ray_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			hurt_animations = mighty_hurt_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = hurt_animations + PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
	OBJECT(current_object)->animation_frame = 0u;
	step_record = OBJECT(current_object)->animation;
	OBJECT(current_object)->sprite_flags &= ~0xC0u;
	step_record += 2u;
	OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
	OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
	project_object(&ignored_4, &depth);
	check_zone_contact();
	check_player_contacts(&depth, &cursor);
	add_player_to_lists();
	ignored = queue_sprite_at_floor();
}

/* State 0x28: caught in a swirl, the player circles chaser_object (turning toward it by swirl_turn_rate)
   while rising ever faster (speed up to 0xA00, speed_z + 0x280 a frame), flailing, with a cry; above the
   screen (-0x40) it loses 8 health: player_respawn, or dying (state 0x16). */
void player_state_swept_away(void)
{
	uint8_t sound_id = 0;
	uint16_t chaser_dy = 0, dy = 0;
	uint32_t away_angle = 0, chaser_angle = 0, flail_animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0;
	uint8_t health = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		PLAYER(current_player)->mishap_flag = 0xFFu;
		PLAYER(current_player)->swirl_turn_rate = 0x220u;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->speed = 0x2000000;
		OBJECT(current_object)->speed_z = 0;
		object = LINKED(current_object, chaser_object);
		away_angle = (uint16_t)(OBJECT(current_object)->x_int - OBJECT(object)->x_int);
		chaser_dy = (uint16_t)(OBJECT(current_object)->y_int - OBJECT(object)->y_int);
		angle_of_vector(chaser_dy, &away_angle, &ignored);
		OBJECT(current_object)->direction = (away_angle - 0x100u) & 0x3FFu;
		flail_animation = flail_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			flail_animation = flail_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				flail_animation = flail_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = flail_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		sound_id = 0x2Fu;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x37u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x77u;
		}
		play_sound(sound_id, &ignored_2, &ignored_3);
	}
	object = LINKED(current_object, chaser_object);
	chaser_angle = (uint16_t)(OBJECT(object)->x_int - OBJECT(current_object)->x_int);
	dy = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
	angle_of_vector(dy, &chaser_angle, &ignored);
	OBJECT(current_object)->direction -= divide_s16((uint16_t)(((OBJECT(current_object)->direction - chaser_angle) & 0x3FFu) << 4), PLAYER(current_player)->swirl_turn_rate);
	OBJECT(current_object)->direction &= 0x3FFu;
	OBJECT(current_object)->speed += 0x100000u;
	if (OBJECT(current_object)->speed_int >= 0xA00)
		OBJECT(current_object)->speed = 0xA000000;
	OBJECT(current_object)->speed_z += 0x280u;
	OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
	set_speed_from_direction();
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	project_object(&ignored_4, &ignored_2);
	animate_with_flags(&ignored_2, &ignored_5);
	if (OBJECT(current_object)->screen_y > -0x40)
	{
		ignored_2 = queue_sprite(ignored_2);
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
		OBJECT(current_object)->screen_x = 0xA0;
	}
	ignored_2 = queue_sprite(ignored_2);
}

/* State 0x27: climbs hand over hand along the touched object (begin, push, move by substate) until x_int
   reaches climb_remaining, then stands. With handle_rod_drop_players set and x_int 0x450 or less it lets
   go and falls (state 6; a cry at 8 health or less). Sorted by the object's parent's spawn_arg_1_low and
   object_climb_steps. */
void player_state_climbing_object(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t x_shift = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, near_edge_fall_animation = 0, object = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_object_climb_begin();
		break;
	case 1:
		player_object_climb_push(cursor);
		break;
	case 2:
		player_object_climb_move();
		break;
	case 3:
		player_object_climb_begin();
		break;
	default:
		invalid_jump(0x8A0Au + PLAYER(current_player)->substate * 3u);
	}
	update_player_blink();
	OBJECT(current_object)->projected_x = PLAYER(current_player)->screen_x_int;
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->x_int > 0x450 || handle_rod_drop_players == 0u)
	{
		if (PLAYER(current_player)->climb_remaining <= OBJECT(current_object)->x_int)
		{
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->state = 0u;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->animation_speed = 0;
			OBJECT(current_object)->sprite_flags &= ~0xFu;
			OBJECT(current_object)->sprite_flags |= 4u;
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0xA;
			OBJECT(current_object)->flags &= ~4u;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		object = LINKED(object, OBJECT(object)->parent);
		PLAYER(current_player)->draw_depth = (OBJECT(object)->spawn_arg_1_low - PLAYER(current_player)->object_climb_steps) * 4u + OBJECT(object)->hitbox_size_x;
		depth = (uint16_t)(PLAYER(current_player)->draw_depth + 2u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	PLAYER(current_player)->state = 6u;
	PLAYER(current_player)->substate = 1u;
	PLAYER(current_player)->ledge_side = 4;
	OBJECT(current_object)->player_view.floor_block_low = -1;
	OBJECT(current_object)->player_view.floor_block_high = -1;
	OBJECT(current_object)->floor_z = 0x8000000;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0xA;
	PLAYER(current_player)->fall_draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y;
	near_edge_fall_animation = sonic_near_edge_fall_animation_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		near_edge_fall_animation = ray_near_edge_fall_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			near_edge_fall_animation = mighty_near_edge_fall_animation_ADDRESS;
	}
	OBJECT(current_object)->animation = near_edge_fall_animation;
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
	if (OBJECT(current_object)->health > 8)
	{
		if (PLAYER(current_player)->climb_remaining <= OBJECT(current_object)->x_int)
		{
			PLAYER(current_player)->substate = 0u;
			PLAYER(current_player)->state = 0u;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->animation_speed = 0;
			OBJECT(current_object)->sprite_flags &= ~0xFu;
			OBJECT(current_object)->sprite_flags |= 4u;
			OBJECT(current_object)->sprite_offset_x = 0;
			OBJECT(current_object)->sprite_offset_y = 0xA;
			OBJECT(current_object)->flags &= ~4u;
		}
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		object = LINKED(object, OBJECT(object)->parent);
		PLAYER(current_player)->draw_depth = (OBJECT(object)->spawn_arg_1_low - PLAYER(current_player)->object_climb_steps) * 4u + OBJECT(object)->hitbox_size_x;
		depth = (uint16_t)(PLAYER(current_player)->draw_depth + 2u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	sound_id = 0x2Fu;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x37u;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x77u;
	}
	play_sound(sound_id, &ignored_2, &ignored_3);
	if (PLAYER(current_player)->climb_remaining <= OBJECT(current_object)->x_int)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->state = 0u;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->animation_speed = 0;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0xA;
		OBJECT(current_object)->flags &= ~4u;
	}
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	object = LINKED(object, OBJECT(object)->parent);
	PLAYER(current_player)->draw_depth = (OBJECT(object)->spawn_arg_1_low - PLAYER(current_player)->object_climb_steps) * 4u + OBJECT(object)->hitbox_size_x;
	depth = (uint16_t)(PLAYER(current_player)->draw_depth + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Starts a climb along the touched object: at its x and y, z 0x78, no speed, object_climb_steps 1,
   screen_x its projected x, and the character's climb animation (Ray: object_climb_animation_ray with
   climb_hand -1, the others climb_hand 0). */
void player_object_climb_begin(void)
{
	uint32_t animation = 0, object = 0;

	PLAYER(current_player)->substate++;
	OBJECT(current_object)->sprite_flags &= ~0x4Fu;
	OBJECT(current_object)->sprite_flags |= 0xAu;
	OBJECT(current_object)->speed = 0;
	PLAYER(current_player)->animation_speed = 0;
	PLAYER(current_player)->step_accumulator = 0;
	PLAYER(current_player)->object_climb_steps = 1u;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_object_climb.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->z = 0x780000;
	OBJECT(current_object)->sprite_offset_x = -0x10;
	OBJECT(current_object)->sprite_offset_y = -0x26;
	PLAYER(current_player)->screen_x = ((int32_t)((uint32_t)OBJECT(current_object)->x + OBJECT(current_object)->y) >> 8) * 0xB5u;
	PLAYER(current_player)->step_accumulator_low = OBJECT(current_object)->x_int;
	animation = object_climb_animation_sonic_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
		animation = object_climb_animation_mighty_ADDRESS;
	OBJECT(current_object)->animation = animation;
	PLAYER(current_player)->climb_hand = 0;
	if (OBJECT(current_object)->kind == 1u)
	{
		OBJECT(current_object)->sprite_offset_x = -0x16;
		OBJECT(current_object)->sprite_offset_y = -0x33;
		OBJECT(current_object)->animation = object_climb_animation_ray_ADDRESS;
		PLAYER(current_player)->climb_hand = -1;
	}
	OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[0];
}

/* Climbing: the trackball's speed (set_climb_speed_from_trackball) aimed between 0x380 and 0x17F adds to
   step_accumulator; at 0x400 a step (substate 2, object_climb_steps + 1, spawn_climb_hand, sound 0x64,
   0x6B or 0x51 by character) if the next hold is on screen, else the push is lost. */
void player_object_climb_push(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	set_climb_speed_from_trackball();
	PLAYER(current_player)->target_heading = OBJECT(current_object)->direction;
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u && ((OBJECT(current_object)->direction - 0x380u) & 0x3FFu) < 0x200u)
	{
		PLAYER(current_player)->step_accumulator_int += OBJECT(current_object)->speed_int;
		if (PLAYER(current_player)->step_accumulator_int >= 0x400)
		{
			PLAYER(current_player)->step_accumulator_int = 0;
			if ((uint16_t)(PLAYER(current_player)->screen_x_int + 0x12u - view_x - 0x10u) >= 0x120u || (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z - 0x90000u) >> 16) - view_y - 0x10u) >= 0xC0u)
				return;
			PLAYER(current_player)->substate++;
			PLAYER(current_player)->object_climb_steps++;
			OBJECT(current_object)->animation_frame = 0u;
			OBJECT(current_object)->animation_speed = 4u;
			OBJECT(current_object)->animation_timer = 0;
			spawn_climb_hand(cursor);
			sound_id = 0x64u;
			if (OBJECT(current_object)->kind != 0u)
			{
				sound_id = 0x6Bu;
				if (OBJECT(current_object)->kind != 1u)
					sound_id = 0x51u;
			}
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
}

/* When a climb step's animation ends: back to substate 1, climb_hand flips (Ray alternates
   object_climb_animation_ray and object_climb_animation_ray_2) and the player moves one hold on:
   screen_x + 0x12, projected_y - 9, x and y from unproject_point. */
void player_object_climb_move(void)
{
	uint32_t negative = 0;
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0, world_x = 0, world_y = 0;
	uint8_t next_hand = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
		return;
	PLAYER(current_player)->substate = 1u;
	next_hand = 0u - PLAYER(current_player)->climb_hand;
	negative = next_hand >> 7;
	PLAYER(current_player)->climb_hand = next_hand;
	if (next_hand == 0u)
	{
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[0];
		PLAYER(current_player)->screen_x += 0x120000u;
		OBJECT(current_object)->projected_y -= 0x90000u;
		world_x = PLAYER(current_player)->screen_x;
		world_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&world_x, &world_y);
		OBJECT(current_object)->x = world_x;
		OBJECT(current_object)->y = world_y;
		return;
	}
	if (negative)
	{
		OBJECT(current_object)->animation = object_climb_animation_ray_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = -0x16;
		OBJECT(current_object)->sprite_offset_y = -0x33;
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[0];
		PLAYER(current_player)->screen_x += 0x120000u;
		OBJECT(current_object)->projected_y -= 0x90000u;
		world_x = PLAYER(current_player)->screen_x;
		world_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&world_x, &world_y);
		OBJECT(current_object)->x = world_x;
		OBJECT(current_object)->y = world_y;
		return;
	}
	OBJECT(current_object)->animation = object_climb_animation_ray_2_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = -0x10;
	OBJECT(current_object)->sprite_offset_y = -0x2A;
	OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[0];
	PLAYER(current_player)->screen_x += 0x120000u;
	OBJECT(current_object)->projected_y -= 0x90000u;
	world_x = PLAYER(current_player)->screen_x;
	world_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&world_x, &world_y);
	OBJECT(current_object)->x = world_x;
	OBJECT(current_object)->y = world_y;
}

/* Leaves a hand on the climbed object at the player's projected position and z (sprite 0x8300 for Sonic
   or 0x8258 with climb_hand 0, 0x82C8 below 0, 0x82C0 above), in the player's palette and offsets;
   handler climb_hand_update. hand is only scratch. */
void spawn_climb_hand(uint32_t hand)
{
	uint16_t sprite_frame = 0;
	uint32_t free_count = 0;

	allocate_object(&free_count, &hand);
	if (free_count == 0u)
		return;
	OBJECT(hand)->sprite_flags = 0x110Au;
	OBJECT(hand)->sprite_palette = OBJECT(current_object)->sprite_palette;
	OBJECT(hand)->sprite_offset_x = OBJECT(current_object)->sprite_offset_x;
	OBJECT(hand)->sprite_offset_y = OBJECT(current_object)->sprite_offset_y;
	OBJECT(hand)->flags = 0u;
	OBJECT(hand)->parent = current_object;
	if ((uint8_t)PLAYER(current_player)->climb_hand == 0u)
	{
		sprite_frame = 0x8300u;
		if (OBJECT(current_object)->kind != 0u)
			sprite_frame = 0x8258u;
		OBJECT(hand)->sprite_frame = sprite_frame;
		OBJECT(hand)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(hand)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(hand)->z = OBJECT(current_object)->z;
		OBJECT(hand)->handler = climb_hand_update_ADDRESS;
		return;
	}
	if (PLAYER(current_player)->climb_hand < 0)
	{
		sprite_frame = 0x82C8u;
		OBJECT(hand)->sprite_frame = sprite_frame;
		OBJECT(hand)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(hand)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(hand)->z = OBJECT(current_object)->z;
		OBJECT(hand)->handler = climb_hand_update_ADDRESS;
		return;
	}
	sprite_frame = 0x82C0u;
	OBJECT(hand)->sprite_frame = sprite_frame;
	OBJECT(hand)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(hand)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(hand)->z = OBJECT(current_object)->z;
	OBJECT(hand)->handler = climb_hand_update_ADDRESS;
}

/* A hand left on the climbed object: drawn just over its player (the player's draw_depth + 5) until the
   player leaves state 0x27 or its step animation starts step 4; then removed. */
void climb_hand_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, object = 0;

	if (PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player << 9))->state == 0x27u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((uint8_t)OBJECT(object)->animation_timer != OBJECT(object)->animation_speed || OBJECT(object)->animation_frame != 4u)
		{
			update_screen_position(&ignored, &ignored_2);
			depth = (uint16_t)(PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->player << 9))->draw_depth + 5u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
	}
	delete_current_object();
}

/* State 0x26: pulled after scene_object (an eighth of the way a frame in x and z) until scene_object's
   direction is 0x1D0 or less, thrown along +x (speed_x 0xA0000) to the floor, carried with the scroll
   (player_move_with_scroll, flailing) past x 0xCD0 and back to x 0x630, then it skids (state 0xC) with a
   cry. */
void player_state_pulled(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint16_t new_heading = 0, pull_distance = 0;
	uint32_t depth = 0, flail_animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, object = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 6:
		PLAYER(current_player)->substate++;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0xA;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->control_lock = 0u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~4u;
		OBJECT(current_object)->flags &= ~8u;
		OBJECT(current_object)->flags &= ~0x10u;
		PLAYER(current_player)->pull_angle = 0x300u;
		pull_distance = (uint16_t)((int32_t)(0x5D00000u - OBJECT(current_object)->x) >> 12);
		PLAYER(current_player)->pull_start_distance = pull_distance;
		PLAYER(current_player)->pull_distance_factor = (int16_t)(0x1200u - pull_distance) >> 5;
		sound_id = 0x36u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x46u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x56u;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		object = LINKED(current_object, scene_object);
		OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 3;
		OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
		PLAYER(current_player)->pull_angle = OBJECT(object)->direction;
		project_object(&ignored_3, &ignored);
		if ((int16_t)PLAYER(current_player)->pull_angle <= 0x1D0)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0xA0000;
			OBJECT(current_object)->speed_y = 0;
			OBJECT(current_object)->speed_z = 0;
		}
		update_player_blink();
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		ignored = queue_sprite_at_floor();
		return;
	case 1:
		object = LINKED(current_object, scene_object);
		OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 3;
		OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
		PLAYER(current_player)->pull_angle = OBJECT(object)->direction;
		project_object(&ignored_3, &ignored);
		if ((int16_t)PLAYER(current_player)->pull_angle <= 0x1D0)
		{
			PLAYER(current_player)->substate++;
			OBJECT(current_object)->speed = 0;
			OBJECT(current_object)->speed_x = 0xA0000;
			OBJECT(current_object)->speed_y = 0;
			OBJECT(current_object)->speed_z = 0;
		}
		update_player_blink();
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		ignored = queue_sprite_at_floor();
		return;
	case 2:
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
			PLAYER(current_player)->substate++;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 3:
		PLAYER(current_player)->substate++;
		PLAYER(current_player)->drift_anim_step = 1u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->drift_turned_back = 0u;
		PLAYER(current_player)->drift_done = 0u;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->ground_speed = 0;
		OBJECT(current_object)->sprite_flags &= 0xFF7Fu;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
	case 4:
		PLAYER(current_player)->control_lock = 0x7Fu;
		player_move_with_scroll(&depth, &cursor);
		if (PLAYER(current_player)->state == 0x16u)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
		if (OBJECT(current_object)->handler == player_respawn_ADDRESS)
		{
			if (PLAYER(current_player)->drift_turned_back != 0u && OBJECT(current_object)->x_int <= 0x630)
			{
				PLAYER(current_player)->substate++;
				PLAYER(current_player)->drift_done = 0xFFu;
				PLAYER(current_player)->control_lock = 0u;
			}
			return;
		}
		flail_animation = flail_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			flail_animation = flail_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				flail_animation = flail_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = flail_animation;
		animate_with_flags(&ignored, &ignored_6);
		if (OBJECT(current_object)->x_int < 0xCD0)
		{
			if (PLAYER(current_player)->drift_turned_back != 0u && OBJECT(current_object)->x_int <= 0x630)
			{
				PLAYER(current_player)->substate++;
				PLAYER(current_player)->drift_done = 0xFFu;
				PLAYER(current_player)->control_lock = 0u;
			}
			return;
		}
		PLAYER(current_player)->drift_turned_back = 0xFFu;
		if (PLAYER(current_player)->drift_turned_back != 0u && OBJECT(current_object)->x_int <= 0x630)
		{
			PLAYER(current_player)->substate++;
			PLAYER(current_player)->drift_done = 0xFFu;
			PLAYER(current_player)->control_lock = 0u;
		}
		return;
	case 5:
		PLAYER(current_player)->heading = 0x2A0u;
		new_heading = (PLAYER(current_player)->heading + 0x200u) & 0x3FFu;
		PLAYER(current_player)->target_heading = new_heading;
		PLAYER(current_player)->facing_angle = new_heading;
		set_heading_direction();
		OBJECT(current_object)->speed = 0x5000000;
		depth = compute_ground_speed();
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		PLAYER(current_player)->state = 0xCu;
		sound_id = 0x48u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x6Du;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x49u;
		}
		play_sound(sound_id, &depth, &ignored_2);
		player_animate_skidding(depth, cursor);
		return;
	default:
		invalid_jump(0x8CD7u + PLAYER(current_player)->substate * 3u);
	}
}

/* State 0x25, by sequence_step: from scene_step 2 the player drops hurt from above with a cry, hops,
   rides the scroll to y 0x4F2, sinks 0x20 into the floor and drifts on to y 0x9B0, floats to y 0xA1C and
   is caught: shaking free (step 12) within state_timer (0x258) jumps it out to land walking, else it
   falls (step 14). */
void player_state_floor_8_sequence(uint32_t cursor)
{
	uint8_t drift_byte = 0, sound_id = 0;
	uint16_t kind = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t depth = 0, drift_animation = 0, drift_direction = 0, drift_speed = 0, drift_x = 0, drift_y = 0, floor_8_landing_animation = 0, floor_8_ride_animation = 0, floor_block_record = 0, hurt_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, running_animations = 0, step_record = 0;
	uint16_t state_timer = 0, state_timer_2 = 0;

	switch (PLAYER(current_player)->sequence_step)
	{
	case 0:
	case 17:
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->flags |= 8u;
		PLAYER(current_player)->sequence_bounced = 0u;
		if (scene_step == 2u)
			PLAYER(current_player)->sequence_step++;
		return;
	case 1:
		if (scene_step == 2u)
			PLAYER(current_player)->sequence_step++;
		return;
	case 2:
		PLAYER(current_player)->sequence_step++;
		scroll_mode = 1u;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->flags |= 0x1000000u;
		PLAYER(current_player)->flags &= ~8u;
		OBJECT(current_object)->z_int -= 0xA0u;
		OBJECT(current_object)->z_int -= (uint32_t)OBJECT(current_object)->kind << 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed = 0;
		PLAYER(current_player)->ground_speed = 0;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(0u);
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = hurt_animations + PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		step_record += 2u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
		sound_id = 0x57u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x65u;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x67u;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->sequence_step++;
			PLAYER(current_player)->state_timer = 2;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->animation_frame++;
			step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
			OBJECT(current_object)->sprite_flags &= ~0xC0u;
			OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
			OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite_at_floor();
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
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 3:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->sequence_step++;
			PLAYER(current_player)->state_timer = 2;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			OBJECT(current_object)->animation_frame++;
			step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
			OBJECT(current_object)->sprite_flags &= ~0xC0u;
			OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
			OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite_at_floor();
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
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 4:
		state_timer = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer - 1u;
		if ((int16_t)state_timer <= 1)
		{
			PLAYER(current_player)->sequence_step++;
			OBJECT(current_object)->speed_z = -0x40000;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->animation_speed = 3u;
			OBJECT(current_object)->speed_x = 0;
			OBJECT(current_object)->speed_y = 0x40000;
			scroll_mode = 0u;
			scroll_speed_low = 0x10;
		}
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_base(ignored);
		return;
	case 5:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			PLAYER(current_player)->sequence_step++;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		}
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			animate_with_flags(&ignored, &ignored_6);
			if (OBJECT(current_object)->animation_frame == 3u)
				OBJECT(current_object)->animation_speed = 0xFFu;
		}
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 6:
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->drift_anim_step = 1u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		OBJECT(current_object)->sprite_flags &= 0xFF7Fu;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		PLAYER(current_player)->control_lock = 0x10u;
		player_move_with_scroll(&depth, &cursor);
		if (PLAYER(current_player)->state == 0x16u)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
		if (OBJECT(current_object)->handler != player_respawn_ADDRESS && OBJECT(current_object)->y_int >= 0x4F2)
		{
			PLAYER(current_player)->sequence_step++;
			OBJECT(current_object)->speed_z = 0;
		}
		return;
	case 7:
		PLAYER(current_player)->control_lock = 0x10u;
		player_move_with_scroll(&depth, &cursor);
		if (PLAYER(current_player)->state == 0x16u)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
		if (OBJECT(current_object)->handler != player_respawn_ADDRESS && OBJECT(current_object)->y_int >= 0x4F2)
		{
			PLAYER(current_player)->sequence_step++;
			OBJECT(current_object)->speed_z = 0;
		}
		return;
	case 8:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		floor_block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		drift_byte = 0u;
		compute_scroll_drift(floor_block_record, drift_byte);
		drift_direction = scroll_drift_direction;
		drift_speed = scroll_drift_speed >> 14;
		drift_x = (int32_t)(sine_table[drift_direction] * drift_speed) >> 8;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		OBJECT(current_object)->x += drift_x;
		drift_y = (int32_t)(cosine_table[drift_direction] * drift_speed) >> 8;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->y -= drift_y;
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		OBJECT(current_object)->floor_z_int += 0x20u;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
			PLAYER(current_player)->sequence_step++;
		animate(&ignored, &ignored_7, &ignored_6);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 9:
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->previous_state = PLAYER(current_player)->state;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		PLAYER(current_player)->drift_anim_step = 1u;
		OBJECT(current_object)->sprite_palette = (OBJECT(current_object)->sprite_palette & 0x1FFFu) | 0x2000u;
		floor_8_ride_animation = sonic_floor_8_ride_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			floor_8_ride_animation = ray_floor_8_ride_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				floor_8_ride_animation = mighty_floor_8_ride_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = floor_8_ride_animation;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 4u;
		OBJECT(current_object)->animation_timer = 0;
		cursor = spawn_player_effect(cursor);
	case 10:
		player_move_with_scroll(&depth, &cursor);
		animate(&depth, &ignored_7, &ignored_6);
		if (PLAYER(current_player)->state == 0x16u)
		{
			player_dispatch_state(depth, cursor);
			return;
		}
		if (OBJECT(current_object)->handler == player_respawn_ADDRESS)
			return;
		if (scroll_drift_speed == 0u)
		{
			PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
			OBJECT(current_object)->y += 0x40000u;
		}
		if (OBJECT(current_object)->y_int >= 0x800)
		{
			PLAYER(current_player)->control_lock = 0x7Fu;
			OBJECT(current_object)->speed = 0;
			PLAYER(current_player)->ground_speed = 0;
		}
		if (OBJECT(current_object)->y_int < 0x9B0)
			return;
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->control_lock = 0u;
		PLAYER(current_player)->state_timer = 0x258;
		OBJECT(current_object)->speed_z = -0x20000;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0x40000;
		drift_animation = sonic_drift_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			drift_animation = ray_drift_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				drift_animation = mighty_drift_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = drift_animation;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		spawn_player_effect(cursor);
		return;
	case 11:
		OBJECT(current_object)->speed_z += 0x2000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		if (OBJECT(current_object)->y_int < 0xA1C)
		{
			animate(&ignored, &ignored_7, &ignored_6);
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		}
		PLAYER(current_player)->sequence_step++;
		PLAYER(current_player)->escape_progress_low = 0;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->flags &= ~0x80u;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		OBJECT(current_object)->speed_z = 0;
		cursor = spawn_struggle_marker(cursor);
		x_int = (uint16_t)OBJECT(current_object)->x_int;
		y_int = (uint16_t)OBJECT(current_object)->y_int;
		kind = 0xCu;
		z = (uint16_t)(OBJECT(current_object)->z_int - 0x28u);
		spawn_sign_2(x_int, y_int, kind, z, cursor);
		sound_id = 0x38u;
		if (OBJECT(current_object)->kind != 0u)
		{
			sound_id = 0x3Cu;
			if (OBJECT(current_object)->kind != 1u)
				sound_id = 0x44u;
		}
		play_sound(sound_id, &ignored, &ignored_2);
		animate(&ignored, &ignored_7, &ignored_6);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 12:
		player_floor_8_sequence_struggle();
		return;
	case 13:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		running_animations = sonic_running_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			running_animations = ray_running_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				running_animations = mighty_running_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = running_animations + PLAYER_FACING_ANIMATION_OFFSETS(running_animations)[PLAYER(current_player)->facing];
		animate_with_flags(&ignored, &ignored_6);
		if ((OBJECT(current_object)->sprite_palette & 0x8000u) == 0u && OBJECT(current_object)->speed_z_int >= 0)
			OBJECT(current_object)->sprite_palette = (OBJECT(current_object)->sprite_palette & 0x1FFFu) | 0x8000u;
		if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z <= OBJECT(current_object)->z)
		{
			PLAYER(current_player)->sequence_step = 0xFu;
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			PLAYER(current_player)->sequence_bounced = 0xFFu;
			OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
			OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
			OBJECT(current_object)->speed_x = (int32_t)OBJECT(current_object)->speed_x >> 1;
			OBJECT(current_object)->speed_y = (int32_t)OBJECT(current_object)->speed_y >> 1;
			PLAYER(current_player)->sequence_step = 0x10u;
		}
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 14:
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored_3, &ignored);
		keep_player_on_screen(ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 15:
		animate(&ignored, &ignored_7, &ignored_6);
		state_timer_2 = PLAYER(current_player)->state_timer;
		PLAYER(current_player)->state_timer = state_timer_2 - 1u;
		if ((int16_t)state_timer_2 <= 1)
		{
			PLAYER(current_player)->state = 1u;
			PLAYER(current_player)->substate = 0u;
			OBJECT(current_object)->speed = 0;
		}
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	case 16:
		OBJECT(current_object)->speed_z += 0x6000u;
		PLAYER(current_player)->previous_z = OBJECT(current_object)->z;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
		PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		animate_with_flags(&ignored, &ignored_6);
		if (OBJECT(current_object)->speed_z_int < 0 || OBJECT(current_object)->z < OBJECT(current_object)->floor_z)
		{
			project_object(&ignored_3, &ignored);
			ignored = queue_sprite_at_floor();
			return;
		}
		PLAYER(current_player)->sequence_step = 0xFu;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed_z = 0;
		floor_8_landing_animation = sonic_floor_8_landing_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			floor_8_landing_animation = ray_floor_8_landing_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				floor_8_landing_animation = mighty_floor_8_landing_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = floor_8_landing_animation;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		PLAYER(current_player)->state_timer = 0x3C;
		OBJECT(current_object)->sprite_flags &= 0xFFBFu;
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite_at_floor();
		return;
	default:
		invalid_jump(0x8F3Fu + PLAYER(current_player)->sequence_step * 3u);
	}
}

/* State 0x25 step 12, caught: the trackball's speed >> 4 adds to escape_progress_low, which drops to 0
   when the ball stops or rolls between 0x280 and 0x380; escape frames by it (idle animation at 0). At
   0x400 the player jumps out (step 13, speed_z -0x80000, speed_y 0x20000); when state_timer runs out,
   step 14. */
void player_floor_8_sequence_struggle(void)
{
	uint16_t input_dy = 0;
	uint32_t floor_8_escape_animation = 0, floor_8_idle_animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, trackball_angle = 0, trackball_speed = 0, trackball_y = 0;
	uint16_t state_timer = 0;

	read_player_trackball();
	trackball_speed = PLAYER(current_player)->trackball_x;
	trackball_y = PLAYER(current_player)->trackball_y;
	trackball_speed = trackball_speed * trackball_speed + trackball_y * trackball_y;
	square_root(&trackball_speed, &ignored);
	trackball_speed &= 0xFFFFu;
	trackball_speed = trackball_speed * trackball_speed;
	if ((int32_t)trackball_speed > 0x5000000)
		trackball_speed = 0x5000000u;
	OBJECT(current_object)->speed = trackball_speed;
	trackball_angle = (uint16_t)PLAYER(current_player)->trackball_x;
	input_dy = (uint16_t)PLAYER(current_player)->trackball_y;
	angle_of_vector(input_dy, &trackball_angle, &ignored_2);
	PLAYER(current_player)->target_heading = trackball_angle;
	if ((uint16_t)OBJECT(current_object)->speed_int == 0u)
		PLAYER(current_player)->escape_progress_low = 0;
	else
	{
		trackball_angle = (trackball_angle & ~0xFFFFu) | PLAYER(current_player)->target_heading;
		trackball_angle = (trackball_angle & ~0xFFFFu) | (uint16_t)(trackball_angle - 0x280u);
		trackball_angle = (trackball_angle & ~0xFFFFu) | (trackball_angle & 0x3FFu);
		if ((uint16_t)trackball_angle < 0x101u)
			PLAYER(current_player)->escape_progress_low = 0;
		else
		{
			trackball_angle = (trackball_angle & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->speed_int;
			trackball_angle = (trackball_angle & ~0xFFFFu) | ((uint16_t)trackball_angle >> 4);
			PLAYER(current_player)->escape_progress_low += trackball_angle;
		}
	}
	if ((uint16_t)PLAYER(current_player)->escape_progress_low != 0u)
	{
		floor_8_escape_animation = floor_8_escape_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			floor_8_escape_animation = floor_8_escape_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				floor_8_escape_animation = floor_8_escape_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = floor_8_escape_animation;
		trackball_angle = (trackball_angle & ~0xFFFFu) | (uint16_t)PLAYER(current_player)->escape_progress_low;
		trackball_angle = (trackball_angle & ~0xFFFFu) | ((uint16_t)trackball_angle >> 8);
		if ((int16_t)FRAME_ANIMATION(floor_8_escape_animation)->last_frame < (int16_t)trackball_angle)
			trackball_angle = (trackball_angle & ~0xFFFFu) | FRAME_ANIMATION(floor_8_escape_animation)->last_frame;
		OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(floor_8_escape_animation)->frames[trackball_angle];
	}
	else
	{
		floor_8_idle_animation = floor_8_idle_animation_sonic_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			floor_8_idle_animation = floor_8_idle_animation_ray_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				floor_8_idle_animation = floor_8_idle_animation_mighty_ADDRESS;
		}
		OBJECT(current_object)->animation = floor_8_idle_animation;
		animate(&ignored, &ignored_3, &ignored_4);
	}
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
		PLAYER(current_player)->sequence_step = 0xEu;
	if (PLAYER(current_player)->escape_progress_low >= 0x400)
	{
		PLAYER(current_player)->sequence_step++;
		OBJECT(current_object)->speed_z = -0x80000;
		OBJECT(current_object)->speed_y = 0x20000;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(trackball_angle);
	}
	update_screen_position(&ignored_5, &ignored);
	ignored = queue_sprite_at_floor();
}

/* States 0x25 and 0x26: the player steers as usual (player_steer) on its floor and is carried by the
   scroll (compute_scroll_drift of its floor block; in 0x26 towards 0x300 once drift_turned_back, nudged
   0xA along x when the drift stops); drawn with the drift animations (drift_anim_step: turning to +x or
   -x and back). */
void player_move_with_scroll(uint32_t *p_depth, uint32_t *p_cursor)
{
	uint8_t drift_byte = 0;
	uint16_t heading_16 = 0, nudge_x = 0;
	uint32_t animation = 0, at_last_step = 0, block = 0, depth = 0, drift_angle = 0, drift_dx = 0, drift_speed = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, screen_y = 0;

	if (PLAYER(current_player)->hit_stop != 0u)
		PLAYER(current_player)->hit_stop--;
	if (PLAYER(current_player)->control_lock != 0u)
		PLAYER(current_player)->control_lock--;
	push_player_out_of_wall(&ignored, &ignored_2, p_cursor);
	PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
	PLAYER(current_player)->push_xy = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, p_cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	if (PLAYER(current_player)->state == 0x25u && PLAYER(current_player)->sequence_step == 0xAu)
		OBJECT(current_object)->z_int += 0x20u;
	project_object(&screen_y, &ignored_2);
	if ((uint8_t)PLAYER(current_player)->pushed_out == 0u && PLAYER(current_player)->control_lock == 0u)
		player_steer(screen_y);
	set_speed_from_player_speed();
	PLAYER(current_player)->state = PLAYER(current_player)->previous_state;
	if ((OBJECT(current_object)->flags & 0x20u) == 0u)
	{
		block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
		drift_byte = 0u;
		compute_scroll_drift(block, drift_byte);
	}
	if (PLAYER(current_player)->state == 0x26u && PLAYER(current_player)->drift_turned_back != 0u)
		scroll_drift_direction = 0x300u;
	drift_angle = scroll_drift_direction;
	drift_speed = scroll_drift_speed >> 14;
	drift_dx = (int32_t)(sine_table[drift_angle] * drift_speed) >> 8;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	OBJECT(current_object)->x += drift_dx;
	depth = (int32_t)(cosine_table[drift_angle] * drift_speed) >> 8;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	OBJECT(current_object)->y -= depth;
	if (PLAYER(current_player)->state == 0x26u && scroll_drift_speed == 0u)
	{
		PLAYER(current_player)->control_lock = 0x3Cu;
		nudge_x = 0xAu;
		depth = (depth & ~0xFFFFu) | 0x100u;
		if (PLAYER(current_player)->drift_turned_back != 0u)
		{
			nudge_x = (uint16_t)(0u - nudge_x);
			depth = (depth & ~0xFFFFu) | 0x300u;
		}
		OBJECT(current_object)->x_int += nudge_x;
		OBJECT(current_object)->direction = depth;
	}
	if (PLAYER(current_player)->hit_stop == 0u)
	{
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	}
	depth = keep_player_on_screen(depth);
	if (PLAYER(current_player)->state != 0x16u && OBJECT(current_object)->handler != player_respawn_ADDRESS)
	{
		add_player_to_lists();
		if ((PLAYER(current_player)->state == 0x25u && PLAYER(current_player)->sequence_step == 0xAu) || PLAYER(current_player)->state == 0x26u)
		{
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				depth = queue_sprite(depth);
			else
			{
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			*p_depth = depth;
			return;
		}
		switch (PLAYER(current_player)->drift_anim_step)
		{
		case 0:
		case 1:
		case 7:
			if (OBJECT(current_object)->speed_int <= 0x80)
			{
				OBJECT(current_object)->flags &= ~0x80u;
				animation = sonic_drift_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					animation = ray_drift_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						animation = mighty_drift_animation_ADDRESS;
				}
				OBJECT(current_object)->animation = animation;
				animate(&depth, &ignored_5, &ignored_6);
				if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
					depth = queue_sprite(depth);
				else
				{
					depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
					queue_sprite_at_depth(&ignored, &depth);
				}
				*p_depth = depth;
				return;
			}
			heading_16 = (uint16_t)((uint32_t)OBJECT(current_object)->direction << 6);
			depth = (depth & ~0xFFFFu) | heading_16;
			if ((uint16_t)(heading_16 + 0x7000u) < 0x6040u)
			{
				PLAYER(current_player)->drift_anim_step = 4u;
				OBJECT(current_object)->animation = drift_minus_x_start_animation_ADDRESS;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->flags &= ~0x80u;
				animate(&depth, &ignored_5, &at_last_step);
				if ((uint8_t)at_last_step == 0u)
				{
					if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
						depth = queue_sprite(depth);
					else
					{
						depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
						depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
						queue_sprite_at_depth(&ignored, &depth);
					}
					*p_depth = depth;
					return;
				}
				PLAYER(current_player)->drift_anim_step++;
				if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
					depth = queue_sprite(depth);
				else
				{
					depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
					queue_sprite_at_depth(&ignored, &depth);
				}
				*p_depth = depth;
				return;
			}
			depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x1000u);
			if ((uint16_t)depth >= 0x6040u)
			{
				OBJECT(current_object)->flags &= ~0x80u;
				animation = sonic_drift_animation_ADDRESS;
				if (OBJECT(current_object)->kind != 0u)
				{
					animation = ray_drift_animation_ADDRESS;
					if (OBJECT(current_object)->kind != 1u)
						animation = mighty_drift_animation_ADDRESS;
				}
				OBJECT(current_object)->animation = animation;
				animate(&depth, &ignored_5, &ignored_6);
				if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
					depth = queue_sprite(depth);
				else
				{
					depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
					queue_sprite_at_depth(&ignored, &depth);
				}
				*p_depth = depth;
				return;
			}
			PLAYER(current_player)->drift_anim_step = 2u;
			OBJECT(current_object)->animation = drift_plus_x_start_animation_ADDRESS;
			OBJECT(current_object)->animation_frame = 0xFFu;
			OBJECT(current_object)->animation_timer = 0;
			OBJECT(current_object)->flags &= ~0x80u;
			animate(&depth, &ignored_5, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				PLAYER(current_player)->drift_anim_step++;
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				depth = queue_sprite(depth);
			else
			{
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			*p_depth = depth;
			return;
		case 2:
		case 4:
			animate(&depth, &ignored_5, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				PLAYER(current_player)->drift_anim_step++;
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				depth = queue_sprite(depth);
			else
			{
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			*p_depth = depth;
			return;
		case 3:
			if (OBJECT(current_object)->speed_int <= 0x80 || (uint16_t)(((uint32_t)OBJECT(current_object)->direction << 6) - 0x1000u) >= 0x6040u)
			{
				PLAYER(current_player)->drift_anim_step = 6u;
				OBJECT(current_object)->animation = drift_plus_x_start_animation_ADDRESS;
				OBJECT(current_object)->animation_frame = 3u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->flags |= 0x80u;
				animate(&depth, &ignored_5, &at_last_step);
				if ((uint8_t)at_last_step == 0u)
				{
					if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
						depth = queue_sprite(depth);
					else
					{
						depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
						depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
						queue_sprite_at_depth(&ignored, &depth);
					}
					*p_depth = depth;
					return;
				}
				OBJECT(current_object)->animation_timer = 0;
				PLAYER(current_player)->drift_anim_step = 1u;
				if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
					depth = queue_sprite(depth);
				else
				{
					depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
					queue_sprite_at_depth(&ignored, &depth);
				}
				*p_depth = depth;
				return;
			}
			OBJECT(current_object)->flags &= ~0x80u;
			OBJECT(current_object)->animation = drift_plus_x_animation_ADDRESS;
			animate(&depth, &ignored_5, &ignored_6);
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				depth = queue_sprite(depth);
			else
			{
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			*p_depth = depth;
			return;
		case 5:
			if (OBJECT(current_object)->speed_int <= 0x80 || (uint16_t)(((uint32_t)OBJECT(current_object)->direction << 6) + 0x7000u) >= 0x6040u)
			{
				PLAYER(current_player)->drift_anim_step = 6u;
				OBJECT(current_object)->animation = drift_minus_x_start_animation_ADDRESS;
				OBJECT(current_object)->animation_frame = 3u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->flags |= 0x80u;
				animate(&depth, &ignored_5, &at_last_step);
				if ((uint8_t)at_last_step != 0u)
				{
					OBJECT(current_object)->animation_timer = 0;
					PLAYER(current_player)->drift_anim_step = 1u;
				}
				if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
					depth = queue_sprite(depth);
				else
				{
					depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
					depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
					queue_sprite_at_depth(&ignored, &depth);
				}
				*p_depth = depth;
				return;
			}
			OBJECT(current_object)->flags &= ~0x80u;
			OBJECT(current_object)->animation = drift_minus_x_animation_ADDRESS;
			animate(&depth, &ignored_5, &ignored_6);
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				depth = queue_sprite(depth);
			else
			{
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			*p_depth = depth;
			return;
		case 6:
			animate(&depth, &ignored_5, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
			{
				OBJECT(current_object)->animation_timer = 0;
				PLAYER(current_player)->drift_anim_step = 1u;
			}
			if (PLAYER(current_player)->state != 0x25u || PLAYER(current_player)->sequence_step != 0xAu)
				depth = queue_sprite(depth);
			else
			{
				depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
				depth = (depth & ~0xFFFFu) | (uint16_t)(depth - 0x20u);
				queue_sprite_at_depth(&ignored, &depth);
			}
			*p_depth = depth;
			return;
		default:
			invalid_jump(0x984Du + PLAYER(current_player)->drift_anim_step * 3u);
		}
	}
	*p_depth = depth;
}

/* Spawns an effect object 0x20 along x, 0x10 back along y and 0x10 lower than the player (palette
   0xD190, player_effect_animation, depth_offset 0x28, player_effect_start) and hands it back in effect. */
uint32_t spawn_player_effect(uint32_t effect)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &effect);
	if (free_count != 0u)
	{
		OBJECT(effect)->sprite_flags = 0x1004u;
		OBJECT(effect)->sprite_palette = 0xD190u;
		OBJECT(effect)->handler = player_effect_start_ADDRESS;
		OBJECT(effect)->flags = 0u;
		OBJECT(effect)->sprite_offset_x = 0;
		OBJECT(effect)->sprite_offset_y = 0;
		OBJECT(effect)->player_effect.depth_offset = 0x28;
		OBJECT(effect)->x = OBJECT(current_object)->x;
		OBJECT(effect)->x_int += 0x20u;
		OBJECT(effect)->y = OBJECT(current_object)->y;
		OBJECT(effect)->y_int -= 0x10u;
		OBJECT(effect)->z = OBJECT(current_object)->z;
		OBJECT(effect)->z_int += 0x10u;
		OBJECT(effect)->animation = player_effect_animation_ADDRESS;
		OBJECT(effect)->animation_frame = 0xFFu;
		OBJECT(effect)->animation_speed = 4u;
		OBJECT(effect)->animation_timer = 0;
	}
	return effect;
}

/* Projects the effect object, then player_effect_update. */
void player_effect_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = player_effect_update_ADDRESS;
	player_effect_update();
}

/* Plays the effect's animation once, sorted at screen_y + depth_offset, then removes it. */
void player_effect_update(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored_3, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->player_effect.depth_offset);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* Spawns the marker that follows a caught player (sprite 0x77A0 in the player's palette with priority
   bits 0x8000, offsets 2,-0xF, struggle_marker_update) and hands it back in marker. */
uint32_t spawn_struggle_marker(uint32_t marker)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &marker);
	if (free_count != 0u)
	{
		OBJECT(marker)->sprite_flags = 0x1004u;
		OBJECT(marker)->sprite_frame = 0x77A0u;
		OBJECT(marker)->sprite_palette = OBJECT(current_object)->sprite_palette;
		OBJECT(marker)->sprite_palette &= 0x1FFFu;
		OBJECT(marker)->sprite_palette |= 0x8000u;
		OBJECT(marker)->handler = struggle_marker_update_ADDRESS;
		OBJECT(marker)->flags = 0u;
		OBJECT(marker)->sprite_offset_x = 2;
		OBJECT(marker)->sprite_offset_y = -0xF;
		OBJECT(marker)->parent = current_object;
	}
	return marker;
}

/* Follows its player (its parent) while that player is in state 0x25 step 12, sorted just above the
   player's floor; otherwise removed. */
void struggle_marker_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, object = 0, player = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	player = PLAYER_BLOCK(1);
	if (OBJECT(object)->player != 0u)
	{
		player = PLAYER_BLOCK(2);
		if (OBJECT(object)->player != 1u)
			player = PLAYER_BLOCK(3);
	}
	if (PLAYER(player)->state != 0x25u || PLAYER(player)->sequence_step != 0xCu)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	update_screen_position(&ignored, &ignored_2);
	object = LINKED(current_object, OBJECT(current_object)->parent);
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(object)->projected_y + OBJECT(object)->floor_z) >> 16) - view_y + 1u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Sets up scene_object at (0x4D7, 0x145B), z 0x78, end_position 0x5C00, direction 0x300; then
   scene_object_update. */
void scene_object_start(void)
{
	OBJECT(current_object)->x = 0x4D70000;
	OBJECT(current_object)->y = 0x145B0000;
	OBJECT(current_object)->z = 0x780000;
	OBJECT(current_object)->scene_object_view.end_position = 0x5C00u;
	OBJECT(current_object)->scene_object_view.radius_x4 = 0xF90u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->handler = scene_object_update_ADDRESS;
	scene_object = current_object;
}

/* Between scroll 0x4FF0 and end_position the object circles in x and z around (0x5D0, z 0x78): direction
   - 0xF a frame, radius scene_object_view.radius_x4 >> 2, which grows by 1 a frame. Removed at
   end_position. */
void scene_object_update(void)
{
	uint32_t direction = 0, radius = 0;

	if (scroll_position < 0x4FF0u)
		return;
	if (OBJECT(current_object)->scene_object_view.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->scene_object_view.radius_x4 += 4u;
	OBJECT(current_object)->direction -= 0xFu;
	OBJECT(current_object)->direction &= 0x3FFu;
	direction = OBJECT(current_object)->direction;
	radius = (int16_t)OBJECT(current_object)->scene_object_view.radius_x4;
	radius = (radius & ~0xFFFFu) | ((uint16_t)radius >> 2);
	OBJECT(current_object)->x = sine_table[direction] * radius + 0x5D00000u;
	OBJECT(current_object)->z = 0u - cosine_table[direction] * radius + 0x780000u;
}

/* State 0x2D: does nothing. */
void player_state_held_still(void)
{
}

/* State 0x2C, stuck on an object, by substate: begin, wait, shake, jump out. Touched in a zone (flags
   0x8000000) it goes back through check_player_contacts and player_dispatch_state. */
void player_state_stuck_on_object(uint32_t depth, uint32_t cursor)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	check_zone_contact();
	if ((OBJECT(current_object)->flags & 0x8000000u) != 0u)
	{
		check_player_contacts(&depth, &cursor);
		player_dispatch_state(depth, cursor);
		return;
	}
	switch (PLAYER(current_player)->substate)
	{
	case 0:
		player_stuck_on_object_begin(cursor);
		break;
	case 1:
		player_stuck_on_object_wait();
		break;
	case 2:
		player_stuck_on_object_shake();
		break;
	case 3:
		player_stuck_on_object_jump_out();
		break;
	case 4:
		player_stuck_on_object_begin(cursor);
		break;
	default:
		invalid_jump(0x9D34u + PLAYER(current_player)->substate * 3u);
	}
	update_screen_position(&ignored, &depth);
	find_floor(&ignored, &depth, &ignored_2, &ignored_3, &ignored_4);
	depth = keep_player_on_screen(depth);
	add_player_to_lists();
	depth = queue_sprite(depth);
}

/* Puts the player on the touched object, 0x20 back along y, in its stuck frame (0x1A40, 0x5180 or 0x35C8
   by character), escape_progress 0, state_timer 0x1E, a sign (kind 0xD) above it and sound 0x23. sign is
   only scratch. */
void player_stuck_on_object_begin(uint32_t sign)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, sprite_frame = 0, x_int = 0, y_int = 0, z = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;

	PLAYER(current_player)->substate++;
	object = LINKED(current_object, OBJECT(current_object)->touched_by);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int -= 0x20u;
	PLAYER(current_player)->previous_x = OBJECT(current_object)->x;
	PLAYER(current_player)->previous_y = OBJECT(current_object)->y;
	sprite_frame = 0x1A40u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sprite_frame = 0x5180u;
		if (OBJECT(current_object)->kind != 1u)
			sprite_frame = 0x35C8u;
	}
	OBJECT(current_object)->sprite_frame = sprite_frame;
	OBJECT(current_object)->sprite_flags &= 0xFFBFu;
	OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
	OBJECT(current_object)->flags &= ~0x40u;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	OBJECT(current_object)->flags |= 0x1000u;
	set_floor_sprite_priority();
	PLAYER(current_player)->escape_progress = 0;
	PLAYER(current_player)->state_timer = 0x1E;
	x_int = (uint16_t)OBJECT(current_object)->x_int;
	y_int = (uint16_t)OBJECT(current_object)->y_int;
	kind = 0xDu;
	z = (uint16_t)(OBJECT(current_object)->z_int - 0x28u);
	spawn_sign_2(x_int, y_int, kind, z, sign);
	sound_id = 0x23u;
	play_sound(sound_id, &ignored, &ignored_2);
	project_object(&ignored_3, &ignored);
}

/* Waits state_timer frames, then substate 2 with the animation restarted at speed 4 and the character's
   voice (0x35, 0x31 or 0x3D). */
void player_stuck_on_object_wait(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t state_timer = 0;

	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer > 1)
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

/* Shaking free: the trackball's speed, aimed between 0x180 and 0x380, adds to escape_progress (a speed
   below 0x100 clears it); struggle frames by escape_progress / 0x4000, the stuck animation at 0. At
   0xC000 the escape animation and sound 0x2B (substate 3). */
void player_stuck_on_object_shake(void)
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
	if (OBJECT(current_object)->speed_int < 0x100)
		PLAYER(current_player)->escape_progress = 0;
	else if (PLAYER(current_player)->target_heading >= 0x180 && PLAYER(current_player)->target_heading <= 0x380)
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
	struggle_index = (int32_t)PLAYER(current_player)->escape_progress / 0x4000;
	if ((int16_t)struggle_index > 2)
		struggle_index = 2u;
	OBJECT(current_object)->animation = mem_read32(animation + struggle_index * 4u);
	animate(&ignored, &ignored_3, &ignored_4);
	if (PLAYER(current_player)->escape_progress < 0xC000)
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

/* When the escape animation ends the player bounces off (state 0xE, substate 1) by its character's
   bounce record 1 (direction, flip, animation): speed 0x2000000, speed_z -0x1A000. */
void player_stuck_on_object_jump_out(void)
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
	OBJECT(current_object)->speed = 0x2000000;
	OBJECT(current_object)->speed_z = -0x1A000;
	PLAYER(current_player)->bounce_speed_z = -0x1A000;
	PLAYER(current_player)->bounce_step = 0xD000;
	PLAYER(current_player)->bounce_speed_z += PLAYER(current_player)->bounce_step;
	set_speed_from_direction();
	OBJECT(current_object)->sprite_frame = FRAME_ANIMATION(OBJECT(current_object)->animation)->frames[1];
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 3;
	OBJECT(current_object)->animation_speed = 3u;
	PLAYER(current_player)->state_timer = 0x14;
	OBJECT(current_object)->flags &= ~0x1000u;
	PLAYER(current_player)->state = 0xEu;
	PLAYER(current_player)->substate = 1u;
}

/* State 0x30: the caught player plays the near-edge fall animation, then cries, pulled toward
   scene_object in game mode 0x19 (else sinking 0x84000 a frame); at z 0x4C8 it leaves a player effect
   (depth_offset 0x70) and goes hurt (state 0xD, substate 2) with sound 0x87. */
void player_state_caught(uint32_t cursor)
{
	uint8_t sound_number = 0;
	uint32_t at_last_step = 0, hurt_animations = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, near_edge_fall_animation = 0, object = 0, step_record = 0;

	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 3:
		PLAYER(current_player)->substate++;
		near_edge_fall_animation = sonic_near_edge_fall_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			near_edge_fall_animation = ray_near_edge_fall_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				near_edge_fall_animation = mighty_near_edge_fall_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = near_edge_fall_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->sprite_flags &= ~0x4Fu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->speed = 0;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~0x2000u;
		OBJECT(current_object)->flags &= ~8u;
		OBJECT(current_object)->flags &= ~0x10u;
		PLAYER(current_player)->control_lock = 0u;
	case 1:
		if ((int8_t)OBJECT(current_object)->animation_speed >= 0)
		{
			at_last_step = animate_and_move(0u);
			if ((uint8_t)at_last_step != 0u)
			{
				OBJECT(current_object)->animation_speed = 0xFFu;
				sound_number = 0x36u;
				if (OBJECT(current_object)->kind != 0u)
				{
					sound_number = 0x37u;
					if (OBJECT(current_object)->kind != 1u)
						sound_number = 0x77u;
				}
				play_sound(sound_number, &ignored, &ignored_2);
			}
		}
		if (current_game_mode == 0x19u)
		{
			object = LINKED(current_object, scene_object);
			OBJECT(current_object)->x += (int32_t)((uint32_t)OBJECT(object)->x - OBJECT(current_object)->x) >> 6;
			OBJECT(current_object)->y += (int32_t)(OBJECT(object)->y + ((uint32_t)OBJECT(current_object)->player << 21) - OBJECT(current_object)->y) >> 6;
			OBJECT(current_object)->z += (int32_t)((uint32_t)OBJECT(object)->z - OBJECT(current_object)->z) >> 3;
		}
		else
			OBJECT(current_object)->z += 0x84000u;
		if (OBJECT(current_object)->z_int >= 0x4C8)
			PLAYER(current_player)->substate++;
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite(ignored);
		return;
	case 2:
		cursor = spawn_player_effect(cursor);
		OBJECT(cursor)->sprite_palette &= 0xBFFFu;
		OBJECT(cursor)->player_effect.depth_offset = 0x70;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 2u;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->direction = 0x200u;
		face_movement_direction(0u);
		hurt_animations = sonic_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			hurt_animations = ray_hurt_animations_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				hurt_animations = mighty_hurt_animations_ADDRESS;
		}
		OBJECT(current_object)->animation = hurt_animations + PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
		OBJECT(current_object)->animation_frame = 0u;
		step_record = OBJECT(current_object)->animation + 2u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
		OBJECT(current_object)->player_view.hurt_flash_period = 0xFFu;
		OBJECT(current_object)->sprite_palette &= 0x1FFFu;
		OBJECT(current_object)->sprite_palette |= 0x4000u;
		sound_number = 0x87u;
		play_sound(sound_number, &ignored, &ignored_2);
		find_floor(&ignored_3, &ignored, &ignored_2, &ignored_4, &ignored_5);
		project_object(&ignored_3, &ignored);
		ignored = queue_sprite(ignored);
		return;
	default:
		invalid_jump(0xA01Au + PLAYER(current_player)->substate * 3u);
	}
}

/* State 0x33: sinks into the floor (floor_z and z 0x1D8 on block type 0x22, else 0x20; first moved 0xC
   toward its edge unless previous_state is 9) with its animation, an effect and sound 0x87; y + 1.5 a
   frame and after 0x1E frames z + 0xA000, cut at ground_clip_bottom. 0x20 under its floor: -8 health,
   respawn or dying. */
void player_state_sinking(uint32_t cursor)
{
	uint8_t sound_id = 0;
	uint32_t clip_cursor = 0, depth = 0, edge_direction = 0, floor_z = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, pit_sinking_animation = 0, point_x = 0, point_y = 0, records_left = 0, sink_z = 0;
	uint8_t health = 0;
	uint16_t state_timer = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->substate = 0xFFu;
		OBJECT(current_object)->flags &= ~0x40u;
		OBJECT(current_object)->sprite_palette = PLAYER_PALETTE_LIST(OBJECT(current_object)->player_view.palette_list)->normal_palette;
		OBJECT(current_object)->flags &= ~0x2000u;
		PLAYER(current_player)->state_timer = 0x1E;
		OBJECT(current_object)->sprite_flags &= ~0xFu;
		OBJECT(current_object)->sprite_flags |= 4u;
		OBJECT(current_object)->sprite_palette &= 0x9FFFu;
		OBJECT(current_object)->sprite_palette |= 0x2000u;
		if (PLAYER(current_player)->previous_state != 9u)
		{
			edge_direction = (uint16_t)PLAYER(current_player)->ledge_side;
			if (edge_direction == 4u)
				edge_direction = (uint8_t)PLAYER(current_player)->ledge_direction;
			switch (edge_direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y_int -= 0xCu;
				break;
			case 1:
				OBJECT(current_object)->x_int += 0xCu;
				break;
			case 2:
				OBJECT(current_object)->y_int += 0xCu;
				break;
			case 3:
				OBJECT(current_object)->x_int -= 0xCu;
				break;
			default:
				invalid_jump(0xA258u + edge_direction * 2u);
			}
		}
		sink_z = 0x1D80000u;
		if (mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x12u) != 0x22u)
			sink_z = 0x200000u;
		OBJECT(current_object)->floor_z = sink_z;
		OBJECT(current_object)->z = sink_z;
		pit_sinking_animation = sonic_pit_sinking_animation_ADDRESS;
		if (OBJECT(current_object)->kind != 0u)
		{
			pit_sinking_animation = ray_pit_sinking_animation_ADDRESS;
			if (OBJECT(current_object)->kind != 1u)
				pit_sinking_animation = mighty_pit_sinking_animation_ADDRESS;
		}
		OBJECT(current_object)->animation = pit_sinking_animation;
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation_speed = 4u;
		cursor = spawn_player_effect(cursor);
		OBJECT(cursor)->sprite_palette &= 0x9FFFu;
		OBJECT(cursor)->sprite_palette |= 0x2000u;
		OBJECT(cursor)->sinking_effect.depth_offset = 0x70;
		OBJECT(cursor)->x_int -= 0x10u;
		OBJECT(cursor)->handler = sinking_effect_update_ADDRESS;
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
			records_left--;
			clip_cursor += 2u;
		} while (records_left != 0u);
		sound_id = 0x87u;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	animate(&ignored, &ignored_3, &ignored_4);
	state_timer = PLAYER(current_player)->state_timer;
	PLAYER(current_player)->state_timer = state_timer - 1u;
	if ((int16_t)state_timer <= 1)
	{
		PLAYER(current_player)->state_timer = 0;
		OBJECT(current_object)->z += 0xA000u;
	}
	OBJECT(current_object)->y += 0x18000u;
	project_object(&ignored_5, &ignored);
	if ((int16_t)(OBJECT(current_object)->floor_z_int + 0x20u) <= OBJECT(current_object)->z_int)
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
		return;
	}
	floor_z = OBJECT(current_object)->floor_z;
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	project_point(floor_z, &ignored, &point_x, &point_y);
	PLAYER(current_player)->ground_clip_bottom = point_y;
	PLAYER(current_player)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
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

/* The effect of a sinking player: moves 1.5 along y a frame with the player, plays its animation once
   (sorted at screen_y + depth_offset), then is removed. */
void sinking_effect_update(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	OBJECT(current_object)->y += 0x18000u;
	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		project_object(&ignored_3, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sinking_effect.depth_offset);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* State 0x36: with health left, player_knocked_off_start and then player_knocked_off_fly; with none,
   dying (state 0x16). */
void player_state_knocked_off(uint32_t depth, uint32_t cursor)
{
	switch (PLAYER(current_player)->substate)
	{
	case 0:
	case 2:
		PLAYER(current_player)->substate++;
		if ((uint8_t)OBJECT(current_object)->health != 0u)
		{
			player_knocked_off_start();
			return;
		}
		PLAYER(current_player)->state = 0x16u;
		PLAYER(current_player)->substate = 0u;
		player_dispatch_state(depth, cursor);
		return;
	case 1:
		player_knocked_off_fly();
		return;
	default:
		invalid_jump(0xA433u + PLAYER(current_player)->substate * 3u);
	}
}

/* Knocked off (state 0x36): 0x258 frames invulnerable, mishap_flag, hurt_contact_delay 0xA, a hop
   (speed_z -0x80000) and a push of 4 a frame along the toucher's direction (both halved on floor type
   4), sound 0x1D and the character's cry, the hurt animation and flash; then player_knocked_off_fly. */
void player_knocked_off_start(void)
{
	uint8_t sound_id = 0;
	uint32_t hit_direction = 0, hurt_animations = 0, ignored = 0, ignored_2 = 0, knock_speed_x = 0, knock_speed_y = 0, object = 0, step_record = 0;
	uint32_t push_speed_5 = 0, push_speed_6 = 0;

	PLAYER(current_player)->invulnerable_timer = 0x258;
	OBJECT(current_object)->flags &= ~0x2000u;
	PLAYER(current_player)->control_lock = 0u;
	PLAYER(current_player)->mishap_flag = 0xFFu;
	PLAYER(current_player)->hurt_contact_delay = 0xA;
	OBJECT(current_object)->speed_z = -0x80000;
	if (OBJECT(current_object)->floor_type == 4u)
		OBJECT(current_object)->speed_z = (int32_t)OBJECT(current_object)->speed_z >> 1;
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	knock_speed_x = 0x40000u;
	if (OBJECT(current_object)->floor_type == 4u)
		knock_speed_x = (int32_t)knock_speed_x >> 1;
	knock_speed_y = 0u;
	hit_direction = OBJECT(object)->direction;
	OBJECT(current_object)->direction = hit_direction;
	OBJECT(current_object)->speed = knock_speed_x << 8;
	if (hit_direction != 0u)
	{
		if (hit_direction != 0x200u)
		{
			if (hit_direction != 0x100u)
				knock_speed_x = 0u - knock_speed_x;
		}
		else
		{
			push_speed_5 = knock_speed_x;
			knock_speed_x = knock_speed_y;
			knock_speed_y = push_speed_5;
		}
	}
	else
	{
		push_speed_6 = knock_speed_x;
		knock_speed_x = knock_speed_y;
		knock_speed_y = 0u - push_speed_6;
	}
	OBJECT(current_object)->speed_x = knock_speed_x;
	OBJECT(current_object)->speed_y = knock_speed_y;
	face_movement_direction(hit_direction);
	sound_id = 0x1Du;
	play_sound(sound_id, &ignored, &ignored_2);
	sound_id = 0x55u;
	if (OBJECT(current_object)->kind != 0u)
	{
		sound_id = 0x4Au;
		if (OBJECT(current_object)->kind != 1u)
			sound_id = 0x5Bu;
	}
	play_sound(sound_id, &ignored, &ignored_2);
	hurt_animations = sonic_hurt_animations_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		hurt_animations = ray_hurt_animations_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			hurt_animations = mighty_hurt_animations_ADDRESS;
	}
	OBJECT(current_object)->animation = hurt_animations + PLAYER_FACING_ANIMATION_OFFSETS(hurt_animations)[PLAYER(current_player)->facing];
	OBJECT(current_object)->animation_frame = 0u;
	step_record = OBJECT(current_object)->animation;
	OBJECT(current_object)->sprite_flags &= ~0xC0u;
	step_record += 2u;
	OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
	OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
	OBJECT(current_object)->player_view.hurt_flash_period = 3u;
	OBJECT(current_object)->player_view.hurt_flash_timer = 3u;
	OBJECT(current_object)->player_view.hurt_flash_on = 0u;
	player_knocked_off_fly();
}

/* The knocked-off player flies (gravity 0x6000, a quarter on floor type 4), the second hurt frame once
   falling; below screen y 0x120 it loses 8 health: player_respawn, or at 8 or less dying (state 0x16). */
void player_knocked_off_fly(void)
{
	uint32_t gravity = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, step_record = 0;
	uint8_t health = 0;

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
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	if (OBJECT(current_object)->animation_frame == 0u && OBJECT(current_object)->speed_z_int >= 0)
	{
		OBJECT(current_object)->animation_frame++;
		step_record = OBJECT(current_object)->animation + 2u + OBJECT(current_object)->animation_frame * 4u;
		OBJECT(current_object)->sprite_flags &= ~0xC0u;
		OBJECT(current_object)->sprite_flags |= FLAGGED_STEP(step_record)->flags;
		OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
	}
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->screen_y < 0x120)
	{
		ignored_2 = queue_sprite_at_floor();
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
	ignored_2 = queue_sprite_at_floor();
}
