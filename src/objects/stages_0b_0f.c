/**
 *    Name:        objects/stages_0b_0f.c
 *    Description: the objects of stages B to F: the chasers (the brick wall that bursts, the biting head, the fire front) and the flames that spread ahead of it, the spike gate, the falling grates, the rising saws and the steps they cut, the puffs, the checkered pad, the static triangle, the railing
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Sets the white puff's palette (0x90A0, overwritten at once by 0x2000) and white_puff_animate as its
   handler. */
void white_puff_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x90A0u;
	OBJECT(current_object)->sprite_palette = 0x2000u;
	OBJECT(current_object)->handler = white_puff_animate_ADDRESS;
}

/* Animates the white puff: the ten white_puff_frames, four frames each, holding the last one 0x5A frames
   before starting over; placed from its projected position, deleted far off screen. */
void white_puff_animate(void)
{
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint8_t wrapped_frame_index = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint16_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->white_puff.frame_timer;
	OBJECT(current_object)->white_puff.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = white_puff_frames[OBJECT(current_object)->white_puff.frame_index];
		OBJECT(current_object)->white_puff.frame_timer = 3u;
		OBJECT(current_object)->white_puff.frame_index++;
		wrapped_frame_index = OBJECT(current_object)->white_puff.frame_index % 0xAu;
		OBJECT(current_object)->white_puff.frame_index = wrapped_frame_index;
		if (wrapped_frame_index == 0u)
			OBJECT(current_object)->white_puff.frame_timer = 0x5Au;
	}
	queue_sprite(0u);
}

/* Sets the checkered pad's palette (0x9040) and checker_pad_animate as its handler. */
void checker_pad_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = checker_pad_animate_ADDRESS;
}

/* Animates the checkered pad: the five checker_pad_frames in turn, nine frames each, at its projected
   position; deleted far off screen. */
void checker_pad_animate(void)
{
	uint32_t deleted = 0, ignored_2 = 0, ignored_3 = 0;
	uint16_t frame_timer = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->checker_pad.frame_timer;
	OBJECT(current_object)->checker_pad.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = checker_pad_frames[OBJECT(current_object)->checker_pad.frame_index_word];
		OBJECT(current_object)->checker_pad.frame_timer = 8u;
		OBJECT(current_object)->checker_pad.frame_index++;
		if (OBJECT(current_object)->checker_pad.frame_index > 4)
			OBJECT(current_object)->checker_pad.frame_index = 0;
	}
	queue_sprite(0u);
}

/* Sets the static triangle's palette (0x9050) and handler (static_triangle_animate), and its child's
   frame 0x7828 and palette 0x9060. */
void static_triangle_init(void)
{
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->handler = static_triangle_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7828u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
}

/* Animates the static triangle: static_triangle_frames in turn, three frames each; its child is drawn
   every other frame (its direction field counts them) 0x68 to the right. Deleted far off screen. */
void static_triangle_animate(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t deleted = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t frame_timer = 0, direction = 0;

	update_screen_position_no_offset();
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	frame_timer = OBJECT(current_object)->static_triangle.frame_timer;
	OBJECT(current_object)->static_triangle.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = static_triangle_frames[(uint8_t)OBJECT(current_object)->static_triangle.frame_index];
		OBJECT(current_object)->static_triangle.frame_timer = 2u;
		OBJECT(current_object)->static_triangle.frame_index++;
		if (OBJECT(current_object)->static_triangle.frame_index > 2)
		{
			OBJECT(current_object)->static_triangle.frame_index = 0;
			OBJECT(current_object)->static_triangle.frame_index_high = 0u;
		}
	}
	ignored_4 = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		offset_x = 0x68u;
		offset_y = 0u;
		place_sprite_near_cursor(offset_x, offset_y);
		queue_sprite(ignored_4);
	}
}

/* Sets up the railing (frame 0x7838): a solid box (kind 0xC) 0xF x 0x57, 0x100 high, and a second
   invisible box 0x56 x 0x11 at the same place, linked as its child (railing_side_update). */
void railing_init(void)
{
	uint32_t block = 0, ignored = 0, object = 0;

	OBJECT(current_object)->sprite_frame = 0x7838u;
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0xFu;
	OBJECT(current_object)->hitbox_size_y = 0x57u;
	OBJECT(current_object)->hitbox_offset_x = 3;
	OBJECT(current_object)->hitbox_offset_y = -0x67;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = railing_draw_ADDRESS;
	spawn_count = 1u;
	spawn_objects(0x015389u, &ignored, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x56u;
	OBJECT(object)->hitbox_size_y = 0x11u;
	OBJECT(object)->hitbox_offset_x = 0x12;
	OBJECT(object)->hitbox_offset_y = -0x11;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = railing_side_update_ADDRESS;
}

/* The railing's handler: placed from its projected position (unproject_object) and, unless deleted far
   off screen with its second box, drawn and put in the touchable list. */
void railing_draw(void)
{
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 0u)
	{
		queue_sprite(0u);
		add_to_touchable_list(deleted);
		return;
	}
}

/* The railing's second box: placed from its projected position and put in the touchable list each frame;
   it is deleted with the railing, its parent. */
void railing_side_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	add_to_touchable_list(ignored);
}

/* Sets up the spike gate: this object is the left pillar (frame 0xA200, a solid box 0x2D x 0x21, kind
   0xC), plus a right pillar at projected +0x94, +0x4A (0xA208) and the beam at +0x1A, +0x14: three
   sections and a shadow (0xA788 at scale 0x180), a 0x2D x 0xDA box 0x28 high, kind 0xC, starting in
   spike_gate_beam_wait. */
void spike_gate_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->sprite_frame = 0xA200u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x2Du;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0xE;
	OBJECT(current_object)->hitbox_offset_y = -0x1A;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spike_gate_pillar_update_ADDRESS;
	spawn_count = 1u;
	spawn_object_with_pieces(0x01543Du, &ignored, &ignored_2, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x94u;
	OBJECT(object)->projected_y_int += 0x4Au;
	OBJECT(object)->sprite_frame = 0xA208u;
	OBJECT(object)->sprite_palette = 0x91F0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x2Cu;
	OBJECT(object)->hitbox_size_y = 0x29u;
	OBJECT(object)->hitbox_offset_x = -9;
	OBJECT(object)->hitbox_offset_y = -0x20;
	OBJECT(object)->hitbox_height = 0x100u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->handler = spike_gate_pillar_update_ADDRESS;
	spawn_count = 4u;
	spawn_object_with_pieces(0x0154A4u, &ignored, &ignored_2, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->direction = 0x300u;
	OBJECT(object)->projected_x += 0x1Au;
	OBJECT(object)->projected_y_int += 0x14u;
	OBJECT(object)->sprite_frame = 0xA1E8u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x2Du;
	OBJECT(object)->hitbox_size_y = 0xDAu;
	OBJECT(object)->hitbox_offset_x = 3;
	OBJECT(object)->hitbox_offset_y = -0x38;
	OBJECT(object)->hitbox_height = 0x28u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->spawn_arg_2 = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->handler = spike_gate_beam_wait_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA1F0u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA1F8u;
	OBJECT(object)->sprite_palette = 0x91E0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_scale = 0x180u;
	OBJECT(object)->sprite_frame = 0xA788u;
	OBJECT(object)->sprite_palette = 0x87A0u;
}

/* A spike gate pillar: when the biting head comes within 0x20 in x or scroll_position passes
   spawn_arg_2, it breaks into five pieces of debris (spawn_debris, from 0x90, 0x40 back in projected x
   and y) and is deleted with its children; else drawn (depth screen_y - 8) and touchable. */
void spike_gate_pillar_update(void)
{
	uint16_t count = 0;
	uint32_t cursor = 0, depth = 0, head_distance_x = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &cursor);
	r14 = LINKED(WORK_RAM, biting_head);
	distance_xy_to_object(&head_distance_x, &ignored_4);
	if ((int32_t)head_distance_x <= 0x20 || (int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		count = 5u;
		OBJECT(current_object)->projected_x -= 0x90u;
		OBJECT(current_object)->projected_y_int -= 0x40u;
		spawn_debris(head_distance_x, count, cursor);
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 8u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
}

/* The beam rises 3 a frame; more than 0x20 above its floor it becomes kind 0x24 (hurts), and past 0x50
   it holds there (spike_gate_beam_hold). */
void spike_gate_beam_rise(void)
{
	uint16_t height_above_floor = 0;

	OBJECT(current_object)->z_int -= 3u;
	height_above_floor = (uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int);
	if ((int16_t)height_above_floor > 0x20)
		OBJECT(current_object)->kind = 0x24u;
	if ((int16_t)height_above_floor <= 0x50)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->spike_gate_beam.pause_timer = 8u;
	OBJECT(current_object)->handler = spike_gate_beam_hold_ADDRESS;
	spike_gate_beam_hold();
}

/* Keeps the beam at the top until pause_timer runs out (8 frames from spike_gate_beam_rise), then
   spike_gate_beam_drop. */
void spike_gate_beam_hold(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->spike_gate_beam.pause_timer;
	OBJECT(current_object)->spike_gate_beam.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->handler = spike_gate_beam_drop_ADDRESS;
	spike_gate_beam_drop();
}

/* The spike gate's beam drops: gravity plus 0x2000 a frame (fall_to_floor). Each landing makes it kind
   0xC (solid) and lifts it 6 (a bounce); the third leaves it 3 above its floor, plays sound 7 and waits
   0xA frames there (spike_gate_beam_wait). */
void spike_gate_beam_drop(void)
{
	uint8_t sound_id = 0;
	uint32_t height = 0, ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->speed_z += 0x2000u;
	fall_to_floor(&height, &ignored, &ignored_2);
	if ((int32_t)height < 1)
	{
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->z_int -= 6u;
		OBJECT(current_object)->spike_gate_beam.bounce_count++;
		if (OBJECT(current_object)->spike_gate_beam.bounce_count <= 2)
		{
			spike_gate_beam_update();
			return;
		}
		OBJECT(current_object)->spike_gate_beam.bounce_count = 0;
		OBJECT(current_object)->z_int += 3u;
		OBJECT(current_object)->spike_gate_beam.pause_timer = 0xAu;
		sound_id = 7u;
		play_sound(sound_id, &ignored, &ignored_2);
		OBJECT(current_object)->handler = spike_gate_beam_wait_ADDRESS;
		spike_gate_beam_wait();
		return;
	}
	spike_gate_beam_update();
}

/* Keeps the beam on the ground until pause_timer runs out (0xA frames after a drop), then
   spike_gate_beam_rise; spike_gate_init starts it here. */
void spike_gate_beam_wait(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->spike_gate_beam.pause_timer;
	OBJECT(current_object)->spike_gate_beam.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		spike_gate_beam_update();
		return;
	}
	OBJECT(current_object)->handler = spike_gate_beam_rise_ADDRESS;
	spike_gate_beam_update();
}

/* Moves the beam (move_object_projected) and breaks it up as the pillars break; else draws its three
   sections at screen offsets (0, 0), (0x30, 0x19) and (0x60, 0x2D), at depths from its height above the
   floor (cursor_floor_screen_y), and its shadow on the floor, and puts it in the touchable list. */
void spike_gate_beam_update(void)
{
	uint16_t count = 0, offset_x = 0, offset_y = 0;
	uint32_t bucket = 0, cursor = 0, depth = 0, head_distance_x = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &cursor);
	r14 = LINKED(WORK_RAM, biting_head);
	distance_xy_to_object(&head_distance_x, &depth);
	if ((int32_t)head_distance_x <= 0x20 || (int16_t)OBJECT(current_object)->spawn_arg_2 < (int16_t)scroll_position)
	{
		count = 5u;
		OBJECT(current_object)->projected_x -= 0x90u;
		OBJECT(current_object)->projected_y_int -= 0x40u;
		spawn_debris(head_distance_x, count, cursor);
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x30u;
	offset_y = 0x19u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x14u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0x60u;
	offset_y = 0x2Du;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = cursor_floor_screen_y(depth);
	depth = (uint16_t)(depth - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	OBJECT(current_object)->screen_x += 0x30u;
	OBJECT(current_object)->screen_y += 0x48u;
	queue_sprite_in_bucket(bucket);
}

/* Starts a flame of the spreading fire on its floor: flame_animation, palette 0x10C0, projected; then
   spreading_flame_burn. fire_front_init starts four, and each fireball a flame throws becomes one. */
void spreading_flame_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1044u;
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = spreading_flame_burn_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* A flame of the spreading fire. Once chaser_object is level with it in y, it throws a fireball
   (start_fireball_flipped) max(random 0-0x3F, its gap to the chaser) on in -y, 0x40 more in stage 0xC at
   chaser_speed over 8, landing as a new flame (spreading_flame_start); then, or with no chaser, it burns
   until off screen. */
void spreading_flame_burn(void)
{
	uint16_t chaser_gap = 0;
	uint32_t depth = 0, fireball = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, object = 0, off_screen = 0, target_dy = 0;

	if (chaser_object == 0u)
		OBJECT(current_object)->flags |= 0x400u;
	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &ignored_2);
			animate(&ignored_2, &ignored_3, &ignored_4);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, chaser_object);
		if (OBJECT(current_object)->y_int < OBJECT(object)->y_int)
		{
			update_screen_position(&ignored, &ignored_2);
			animate(&ignored_2, &ignored_3, &ignored_4);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		chaser_gap = (uint16_t)(OBJECT(current_object)->y_int - OBJECT(object)->y_int);
		if (stage == 0xCu && chaser_speed > 8)
			chaser_gap += 0x40u;
		flame_chaser_gap = chaser_gap;
		allocate_object(&free_count, &fireball);
		if (free_count == 0u)
		{
			update_screen_position(&ignored, &ignored_2);
			animate(&ignored_2, &ignored_3, &ignored_4);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		current_object = fireball;
		OBJECT(current_object)->x = OBJECT(object_cursor)->x;
		OBJECT(current_object)->y = OBJECT(object_cursor)->y;
		OBJECT(current_object)->init_fireball_flight.target_x = OBJECT(current_object)->x_int;
		OBJECT(current_object)->init_fireball_flight.target_y = OBJECT(current_object)->y_int;
		next_random(&target_dy, &ignored_5, &ignored_6);
		target_dy = target_dy & 0x3Fu;
		if ((int16_t)target_dy < flame_chaser_gap)
			target_dy = (uint16_t)flame_chaser_gap;
		OBJECT(current_object)->init_fireball_flight.target_y -= target_dy;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->init_fireball_flight.state = 0u;
		OBJECT(current_object)->next_handler = spreading_flame_start_ADDRESS;
		start_fireball_flipped();
		current_object = object_cursor;
		OBJECT(current_object)->flags |= 0x400u;
	}
	update_screen_position(&ignored, &ignored_2);
	animate(&ignored_2, &ignored_3, &ignored_4);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Sets up the brick wall: frame 0x7D90, palette 0x10D0, standing at z 0x1D8 (its floor_z too),
   projected; brick_wall_wait runs it. */
void brick_wall_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0x7D90u;
	OBJECT(current_object)->sprite_palette = 0x10D0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 8;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = brick_wall_wait_ADDRESS;
	project_object(&ignored, &ignored_2);
}

/* Draws the brick wall until chaser_stopped is set, then bursts it: brick_wall_bricks bricks from
   brick_wall_brick_records (screen offsets, speeds, animations) fly off as rock_debris_fly,
   brick_wall_dust dust puffs start from brick_wall_dust_records (brick_dust_animate), and the wall is
   deleted. */
void brick_wall_wait(void)
{
	uint16_t screen_x = 0;
	uint32_t brick_animation = 0, depth = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, piece_record = 0, pieces_left = 0;

	if (chaser_stopped == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_bricks;
	piece_record = brick_wall_brick_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(object)->sprite_flags = BRICK_WALL_BRICK(piece_record)->sprite_flags;
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += BRICK_WALL_BRICK(piece_record - 0x2u)->offset_x;
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += BRICK_WALL_BRICK(piece_record - 0x4u)->offset_y;
		piece_record += 2u;
		OBJECT(object)->speed_x_fraction = BRICK_WALL_BRICK(piece_record - 0x6u)->speed_x;
		piece_record += 2u;
		OBJECT(object)->speed_y_fraction = BRICK_WALL_BRICK(piece_record - 0x8u)->speed_y;
		piece_record += 2u;
		brick_animation = brick_animations_ADDRESS + BRICK_WALL_BRICK(piece_record - 0xAu)->look;
		OBJECT(object)->sprite_palette = BRICK_LOOK(brick_animation)->palette;
		OBJECT(object)->animation = brick_animation + 2u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = brick_wall_dust;
	piece_record = brick_wall_dust_records_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = BRICK_DUST_PUFF(piece_record)->sprite_flags;
		OBJECT(object)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(object)->screen_x += BRICK_DUST_PUFF(piece_record - 0x2u)->offset_x;
		OBJECT(object)->screen_y = r14;
		piece_record += 2u;
		OBJECT(object)->screen_y += BRICK_DUST_PUFF(piece_record - 0x4u)->offset_y;
		piece_record += 2u;
		OBJECT(object)->brick_dust.wait_timer = BRICK_DUST_PUFF(piece_record - 0x6u)->delay;
		OBJECT(object)->animation = brick_dust_animation_ADDRESS;
		OBJECT(object)->sprite_palette = 0x10E0u;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = brick_dust_animate_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
	delete_current_object();
}

/* A dust puff of the bursting brick wall: hidden while its wait_timer runs, then plays
   brick_dust_animation once and is deleted. */
void brick_dust_animate(void)
{
	uint32_t at_last_step = 0, ignored = 0, ignored_2 = 0;
	uint16_t wait_timer = 0;

	wait_timer = OBJECT(current_object)->brick_dust.wait_timer;
	OBJECT(current_object)->brick_dust.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->brick_dust.wait_timer = 0;
	animate(&ignored, &ignored_2, &at_last_step);
	if ((int8_t)at_last_step >= 0)
	{
		ignored = queue_sprite(ignored);
		return;
	}
	delete_current_object();
}

/* Sets up the biting head: x, y and path from biting_head_paths (by its sprite_frame), kind 9 (hurts)
   with a 0x80 x 0x40 box 0xF0 high, a part block from take_free_object (biting_head_parts and two clip
   entries) and a second 0x200 x 0x130 box as its child; biting_head = it, biting_head_done = 0; then
   biting_head_wait. */
void biting_head_init(void)
{
	uint32_t free_count = 0, head_record = 0, taken_object = 0;

	head_record = biting_head_paths_ADDRESS + biting_head_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = BITING_HEAD_PATH_START(head_record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	head_record += 2u;
	OBJECT(current_object)->y_int = BITING_HEAD_PATH_START(head_record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	OBJECT(current_object)->biting_head_view.path = head_record + 2u;
	OBJECT(current_object)->sprite_flags = 0xD004u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->biting_head_view.holding_back = 0u;
	OBJECT(current_object)->kind = 9u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->biting_head_view.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->hitbox_size_x = 0x80u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = 0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->biting_head_view.bounce_index = -1;
	OBJECT(current_object)->floor_z = 0x1D80000;
	OBJECT(current_object)->z = 0x1D80000;
	OBJECT(current_object)->spawn_arg_2_low = 0u;
	biting_head = current_object;
	biting_head_done = 0u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->biting_head_view.bite_step = 0u;
	OBJECT(current_object)->biting_head_view.bite_timer = 0;
	take_free_object(&free_count, &taken_object);
	if (free_count != 0u)
	{
		move_halfwords_backward(taken_object, biting_head_parts_ADDRESS, 0x19u);
		OBJECT(current_object)->part_list = taken_object;
		move_halfwords_backward(taken_object + 0x40u, biting_head_clip_entry_ADDRESS, 0xAu);
		move_halfwords_backward(taken_object + 0x54u, biting_head_clip_entry_ADDRESS, 0xAu);
		take_free_object(&free_count, &taken_object);
		if (free_count != 0u)
		{
			OBJECT(taken_object)->hitbox_size_x = 0x200u;
			OBJECT(taken_object)->hitbox_size_y = 0x130u;
			OBJECT(taken_object)->direction = 0x300u;
			OBJECT(taken_object)->hitbox_height = 0xF0u;
			OBJECT(taken_object)->hitbox_offset_x = 0;
			OBJECT(taken_object)->hitbox_offset_y = -0x80;
			OBJECT(taken_object)->flags = 0x40000000u;
			OBJECT(taken_object)->kind = 9u;
			OBJECT(current_object)->child = taken_object;
			OBJECT(current_object)->handler = biting_head_wait_ADDRESS;
		}
	}
}

/* Keeps the biting head waiting until chaser_stopped is set, or sets it itself once scroll_position
   reaches 0x1200; then biting_head_chase. */
void biting_head_wait(void)
{
	if (chaser_stopped != 0u)
		OBJECT(current_object)->handler = biting_head_chase_ADDRESS;
	else if (scroll_position >= 0x1200u)
	{
		chaser_stopped = 0xFFu;
		OBJECT(current_object)->handler = biting_head_chase_ADDRESS;
		return;
	}
}

/* Each frame of the biting head's chase: biting_head_read_script, or biting_head_chase_step while it
   holds back. From its second frame, when no_player_in_play, player_mishap or player_dropping_in is set,
   it starts holding back: base_speed 0 and screen_threshold 0x160 (-0x20 in directions 1 and 2), the old
   ones saved. */
void biting_head_chase(void)
{
	uint16_t hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		biting_head_read_script();
		return;
	}
	if (OBJECT(current_object)->biting_head_view.holding_back != 0u)
	{
		biting_head_chase_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		biting_head_read_script();
		return;
	}
	OBJECT(current_object)->biting_head_view.holding_back = 0xFFu;
	OBJECT(current_object)->biting_head_view.saved_base_speed = OBJECT(current_object)->biting_head_view.base_speed;
	OBJECT(current_object)->biting_head_view.base_speed = 0;
	OBJECT(current_object)->biting_head_view.hold_start_position = scroll_position;
	OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
	OBJECT(current_object)->biting_head_view.saved_threshold = OBJECT(current_object)->biting_head_view.screen_threshold;
	hold_threshold = 0xFFE0u;
	if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
		hold_threshold = 0x160u;
	OBJECT(current_object)->biting_head_view.screen_threshold = hold_threshold;
	biting_head_chase_step();
}

/* Counts the biting head's wait_timer down; once it is out, loads the next path record
   (biting_head_load_record) when scroll_position reaches its scroll word or the head reaches its
   coordinate along its direction (y in 0, 2 and 4, x in 1 and 3); then biting_head_chase_step. */
void biting_head_read_script(void)
{
	uint16_t record_coordinate = 0;
	uint32_t direction = 0, path_cursor = 0;
	uint16_t wait_timer = 0, record_scroll = 0;

	wait_timer = OBJECT(current_object)->biting_head_view.wait_timer;
	OBJECT(current_object)->biting_head_view.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		biting_head_chase_step();
		return;
	}
	OBJECT(current_object)->biting_head_view.wait_timer = 0;
	path_cursor = OBJECT(current_object)->biting_head_view.path;
	record_scroll = BITING_HEAD_PATH_ENTRY(path_cursor)->position;
	path_cursor += 2u;
	if (record_scroll <= scroll_position)
	{
		path_cursor += 2u;
		biting_head_load_record(path_cursor);
		return;
	}
	record_coordinate = (uint16_t)BITING_HEAD_PATH_ENTRY(path_cursor - 0x2u)->limit;
	direction = OBJECT(current_object)->direction;
	path_cursor += 2u;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if ((int16_t)record_coordinate < OBJECT(current_object)->y_int)
		{
			biting_head_chase_step();
			return;
		}
		biting_head_load_record(path_cursor);
		return;
	case 1:
		if (OBJECT(current_object)->x_int < (int16_t)record_coordinate)
		{
			biting_head_chase_step();
			return;
		}
		biting_head_load_record(path_cursor);
		return;
	case 2:
		if (OBJECT(current_object)->y_int < (int16_t)record_coordinate)
		{
			biting_head_chase_step();
			return;
		}
		biting_head_load_record(path_cursor);
		return;
	case 3:
		if ((int16_t)record_coordinate < OBJECT(current_object)->x_int)
		{
			biting_head_chase_step();
			return;
		}
		biting_head_load_record(path_cursor);
		return;
	default:
		invalid_jump(0x8ABC3u + (direction >> 8) * 2u);
	}
}

/* Loads the biting head's next path record from path_cursor: screen_threshold, catch_up_factor,
   base_speed, wait_timer and direction, path moving past it. A negative factor ends the chase: the part
   block (sprite_frame here is the low half of part_list) and the second box are freed and the head
   deleted. */
void biting_head_load_record(uint32_t path_cursor)
{
	if (BITING_HEAD_PATH_ENTRY(path_cursor - 0x4u)->catch_up_factor < 0)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->sprite_frame);
		push_free_object();
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		push_free_object();
		delete_current_object();
		return;
	}
	OBJECT(current_object)->biting_head_view.screen_threshold = BITING_HEAD_PATH_ENTRY(path_cursor - 0x4u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->biting_head_view.catch_up_factor = BITING_HEAD_PATH_ENTRY(path_cursor - 0x6u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->biting_head_view.base_speed = BITING_HEAD_PATH_ENTRY(path_cursor - 0x8u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->biting_head_view.wait_timer = BITING_HEAD_PATH_ENTRY(path_cursor - 0xAu)->wait;
	path_cursor += 2u;
	OBJECT(current_object)->direction = BITING_HEAD_PATH_ENTRY(path_cursor - 0xCu)->direction;
	OBJECT(current_object)->biting_head_view.path = path_cursor + 2u;
	biting_head_chase_step();
}

/* Projects the biting head, puts its second box 0x40 further in x (same y and z) and both boxes in the
   zone list. A hold-back ends, restoring screen_threshold and base_speed, after 0x300 of scroll or when
   hold_timer runs out (it only counts in game_mode 3), going to biting_head_catch_up_speed; otherwise
   biting_head_choose_speed. */
void biting_head_chase_step(void)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0;
	uint8_t hold_timer = 0;

	project_object(&ignored, &ignored_2);
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->x = OBJECT(current_object)->x;
	OBJECT(object)->x_int += 0x40u;
	OBJECT(object)->y = OBJECT(current_object)->y;
	OBJECT(object)->z = OBJECT(current_object)->z;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	add_to_zone_list(ignored);
	current_object = object_cursor;
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
	{
		biting_head_choose_speed();
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->biting_head_view.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->biting_head_view.holding_back = 0u;
		OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
		OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		biting_head_catch_up_speed();
		return;
	}
	if (game_mode != 3u)
		OBJECT(current_object)->biting_head_view.hold_timer = 0xFFu;
	hold_timer = OBJECT(current_object)->biting_head_view.hold_timer;
	OBJECT(current_object)->biting_head_view.hold_timer = hold_timer - 1u;
	if (hold_timer == 0u)
	{
		OBJECT(current_object)->biting_head_view.holding_back = 0u;
		OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
		OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		biting_head_catch_up_speed();
		return;
	}
	biting_head_choose_speed();
}

/* Moves the biting head at base_speed while it is within screen_threshold (screen_x at most it in
   directions 0 and 3, at least it in 1, 2 and 4); beyond it, or above the screen (screen_y below 0), it
   drops its wait and any hold-back and catches up (biting_head_catch_up_speed). */
void biting_head_choose_speed(void)
{
	uint16_t move_speed = 0;
	uint32_t direction = 0;

	if (OBJECT(current_object)->screen_y < 0)
	{
		OBJECT(current_object)->biting_head_view.wait_timer = 0;
		if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
		{
			biting_head_catch_up_speed();
			return;
		}
		OBJECT(current_object)->biting_head_view.holding_back = 0u;
		OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
		OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		biting_head_catch_up_speed();
		return;
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->biting_head_view.screen_threshold)
		{
			move_speed = (uint16_t)OBJECT(current_object)->biting_head_view.base_speed;
			biting_head_move_at_speed(move_speed);
			return;
		}
		OBJECT(current_object)->biting_head_view.wait_timer = 0;
		if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
		{
			biting_head_catch_up_speed();
			return;
		}
		OBJECT(current_object)->biting_head_view.holding_back = 0u;
		OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
		OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		biting_head_catch_up_speed();
		return;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->biting_head_view.screen_threshold <= OBJECT(current_object)->screen_x)
		{
			move_speed = (uint16_t)OBJECT(current_object)->biting_head_view.base_speed;
			biting_head_move_at_speed(move_speed);
			return;
		}
		OBJECT(current_object)->biting_head_view.wait_timer = 0;
		if (OBJECT(current_object)->biting_head_view.holding_back == 0u)
		{
			biting_head_catch_up_speed();
			return;
		}
		OBJECT(current_object)->biting_head_view.holding_back = 0u;
		OBJECT(current_object)->biting_head_view.screen_threshold = OBJECT(current_object)->biting_head_view.saved_threshold;
		OBJECT(current_object)->biting_head_view.base_speed = OBJECT(current_object)->biting_head_view.saved_base_speed;
		biting_head_catch_up_speed();
		return;
	default:
		invalid_jump(0x8ACAAu + (direction >> 8) * 2u);
	}
}

/* Moves the biting head (biting_head_move_at_speed) at the camera's speed on its slope
   (compute_camera_slope_speed with its floor_slope_factors entry) times catch_up_factor / 0x100, or at
   base_speed when that is higher. */
void biting_head_catch_up_speed(void)
{
	uint16_t move_speed = 0;
	uint32_t floor_slope = 0, slope_factor = 0;

	slope_factor = 0u;
	if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
	{
		floor_slope = OBJECT(current_object)->floor_slope;
		slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
	}
	compute_camera_slope_speed(0u, slope_factor);
	move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->biting_head_view.catch_up_factor) >> 8;
	if (OBJECT(current_object)->biting_head_view.base_speed < (int16_t)move_speed)
	{
		biting_head_move_at_speed(move_speed);
		return;
	}
	move_speed = (uint16_t)OBJECT(current_object)->biting_head_view.base_speed;
	biting_head_move_at_speed(move_speed);
}

/* Sets chaser_speed = move_speed; after the wait the head moves that much along its direction. Moving,
   parts 0 and 4 cycle biting_head_move_frames and it bobs; the jaw (part 3) bites, step 5 shaking it
   (biting_head_bounce). Boxes 0x50 high from x -0x355, biting_head_done from -0x431; drawn (with its
   clip entries before scroll 0x1500). */
void biting_head_move_at_speed(uint16_t move_speed)
{
	uint16_t bounce_dy = 0, move_frame = 0;
	uint32_t bite_record = 0, depth = 0, direction = 0, ignored = 0, ignored_2 = 0, move_frames = 0, next_bite_step = 0, next_frame = 0, point_x = 0, point_y = 0, z = 0;
	uint8_t animation_timer = 0, bob_phase = 0, bite_timer = 0;

	chaser_speed = move_speed;
	if (OBJECT(current_object)->biting_head_view.wait_timer <= 0)
	{
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
			OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
			break;
		case 1:
			OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
			break;
		case 2:
		case 4:
			OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
			break;
		case 3:
			OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
			break;
		default:
			invalid_jump(0x8AD42u + (direction >> 8) * 2u);
		}
	}
	if (OBJECT(current_object)->biting_head_view.holding_back == 0u && (uint16_t)chaser_speed != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = 2;
			next_frame = OBJECT(current_object)->animation_frame;
			next_frame = (uint8_t)(next_frame + 1u);
			move_frames = biting_head_move_frames_ADDRESS;
			if ((int8_t)biting_head_frames < (int8_t)next_frame)
				next_frame = 0u;
			OBJECT(current_object)->animation_frame = next_frame;
			move_frame = SPRITE_FRAME_NUMBERS(move_frames)[next_frame];
			mem_write16(OBJECT(current_object)->part_list + 2u, move_frame);
			mem_write16(OBJECT(current_object)->part_list + 0x22u, move_frame);
		}
		OBJECT(current_object)->biting_head_view.bob_counter++;
		bob_phase = OBJECT(current_object)->biting_head_view.bob_counter & 3u;
		OBJECT(current_object)->biting_head_view.bob_counter = bob_phase;
		if (bob_phase == 0u)
			OBJECT(current_object)->screen_y--;
	}
	bite_timer = OBJECT(current_object)->biting_head_view.bite_timer;
	OBJECT(current_object)->biting_head_view.bite_timer = bite_timer - 1u;
	if ((int8_t)bite_timer <= 1)
	{
		next_bite_step = OBJECT(current_object)->biting_head_view.bite_step;
		next_bite_step = (uint8_t)(next_bite_step + 1u);
		bite_record = biting_head_bite_step_records_ADDRESS;
		if ((int8_t)biting_head_bite_steps < (int8_t)next_bite_step)
			next_bite_step = 0u;
		OBJECT(current_object)->biting_head_view.bite_step = next_bite_step;
		bite_record += next_bite_step * 4u;
		mem_write16(OBJECT(current_object)->part_list + 0x1Au, BITING_HEAD_BITE_STEP(bite_record)->jaw_frame);
		OBJECT(current_object)->biting_head_view.bite_timer = BITING_HEAD_BITE_STEP(bite_record)->hold;
		if (OBJECT(current_object)->biting_head_view.bite_step == 5u)
			OBJECT(current_object)->biting_head_view.bounce_index = 0;
	}
	if (OBJECT(current_object)->biting_head_view.bounce_index >= 0)
	{
		OBJECT(current_object)->biting_head_view.bounce_index++;
		bounce_dy = (uint16_t)biting_head_bounce[(uint8_t)OBJECT(current_object)->biting_head_view.bounce_index];
		if ((uint8_t)bounce_dy != 0x7Fu)
			OBJECT(current_object)->screen_y += bounce_dy;
		else
		{
			OBJECT(current_object)->biting_head_view.bounce_index = -1;
			if ((uint8_t)bounce_dy != 0x7Fu)
				OBJECT(current_object)->screen_y += bounce_dy;
		}
	}
	if (OBJECT(current_object)->x_int <= -0x355)
	{
		OBJECT(LINKED(current_object, OBJECT(current_object)->child))->hitbox_height = 0x50u;
		OBJECT(current_object)->hitbox_height = 0x50u;
	}
	if (OBJECT(current_object)->x_int <= -0x431)
		biting_head_done = 0xFFu;
	if (scroll_position >= 0x1500u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	}
	point_x = 0x8210000u;
	point_y = 0x5EC0000u;
	z = 0x1D80000u;
	project_point(z, &ignored, &point_x, &point_y);
	mem_write16(OBJECT(current_object)->part_list + 0x48u, point_x);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	biting_head_depth = depth;
	current_object = OBJECT(current_object)->part_list;
	current_object = current_object + 0x54u;
	queue_sprite_at_depth(&ignored_2, &depth);
	current_object = object_cursor;
	depth = biting_head_depth;
	queue_sprite_at_depth(&ignored_2, &depth);
	current_object = OBJECT(current_object)->part_list;
	depth = biting_head_depth;
	current_object = current_object + 0x40u;
	queue_sprite_at_depth(&ignored_2, &depth);
}

/* Sets up the fire front, an invisible chaser: x, y, floor_z, direction and path from fire_front_paths
   (by its sprite_frame), a 0x132 x 0x200 box of kind 0x14 (tumbles the players), chaser_object = it;
   with 3 or more objects free, four flames (spreading_flame_start) 0x38 apart in x from 0x28; then
   fire_front_chase. */
void fire_front_init(void)
{
	uint16_t flame_offset_x = 0;
	uint32_t flames_left = 0, ignored = 0, object = 0, path = 0;

	path = fire_front_paths_ADDRESS + fire_front_paths[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = FIRE_FRONT_PATH_START(path)->x;
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = FIRE_FRONT_PATH_START(path - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->floor_z_int = FIRE_FRONT_PATH_START(path - 0x4u)->floor_z;
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	path += 2u;
	OBJECT(current_object)->direction = FIRE_FRONT_PATH_START(path - 0x6u)->direction;
	OBJECT(current_object)->fire_front.path = path + 2u;
	OBJECT(current_object)->flags = 0x40000000u;
	OBJECT(current_object)->fire_front.holding_back = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->fire_front.wait_timer = 1;
	OBJECT(current_object)->hitbox_height = 0xF0u;
	OBJECT(current_object)->handler = fire_front_chase_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x132u;
	OBJECT(current_object)->hitbox_size_y = 0x200u;
	OBJECT(current_object)->hitbox_offset_x = -0x40;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_object = current_object;
	if (free_object_stack < 3)
		return;
	flames_left = 4u;
	flame_offset_x = 0x28u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->handler = spreading_flame_start_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->x_int += flame_offset_x;
		flame_offset_x += 0x38u;
		flames_left--;
	} while (flames_left != 0u);
}

/* Each frame of the fire front's chase, as biting_head_chase: fire_front_read_script, or
   fire_front_chase_step while it holds back (base_speed and speeds 0). Its hold screen_threshold is
   direction ^ (direction >> 1), 0x160 only in direction 0, not the biting head's -0x20 or 0x160. */
void fire_front_chase(void)
{
	uint16_t direction = 0, threshold = 0;
	uint16_t hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		fire_front_read_script();
		return;
	}
	if (OBJECT(current_object)->fire_front.holding_back != 0u)
	{
		fire_front_chase_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		fire_front_read_script();
		return;
	}
	OBJECT(current_object)->fire_front.holding_back = 0xFFu;
	OBJECT(current_object)->fire_front.saved_base_speed = OBJECT(current_object)->fire_front.base_speed;
	OBJECT(current_object)->fire_front.base_speed = 0;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->fire_front.hold_start_position = scroll_position;
	OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
	OBJECT(current_object)->fire_front.saved_threshold = OBJECT(current_object)->fire_front.screen_threshold;
	direction = OBJECT(current_object)->direction;
	hold_threshold = direction ^ (direction >> 1);
	threshold = hold_threshold;
	if (hold_threshold == 0u)
		threshold = 0x160u;
	OBJECT(current_object)->fire_front.screen_threshold = threshold;
	fire_front_chase_step();
}

/* As biting_head_read_script for the fire front: once wait_timer is out, the next record loads when
   scroll_position reaches its scroll word or the front reaches its coordinate (y + hitbox_offset_y in
   directions 0 and 4); a coordinate of 0 waits for the scroll only. */
void fire_front_read_script(void)
{
	uint16_t trigger_coordinate = 0;
	uint32_t cursor = 0, direction = 0;
	uint16_t wait_timer = 0, trigger_scroll = 0;

	wait_timer = OBJECT(current_object)->fire_front.wait_timer;
	OBJECT(current_object)->fire_front.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		fire_front_chase_step();
		return;
	}
	OBJECT(current_object)->fire_front.wait_timer = 0;
	cursor = OBJECT(current_object)->fire_front.path;
	trigger_scroll = CHASE_STEP_WITH_LIMIT(cursor)->position;
	cursor += 2u;
	if (trigger_scroll <= scroll_position)
	{
		cursor += 2u;
		fire_front_load_record(cursor);
		return;
	}
	trigger_coordinate = (uint16_t)CHASE_STEP_WITH_LIMIT(cursor - 0x2u)->limit;
	cursor += 2u;
	if (trigger_coordinate == 0u)
	{
		fire_front_chase_step();
		return;
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if ((int16_t)trigger_coordinate < (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_offset_y))
		{
			fire_front_chase_step();
			return;
		}
		fire_front_load_record(cursor);
		return;
	case 1:
		if (OBJECT(current_object)->x_int < (int16_t)trigger_coordinate)
		{
			fire_front_chase_step();
			return;
		}
		fire_front_load_record(cursor);
		return;
	case 2:
		if (OBJECT(current_object)->y_int < (int16_t)trigger_coordinate)
		{
			fire_front_chase_step();
			return;
		}
		fire_front_load_record(cursor);
		return;
	case 3:
		if ((int16_t)trigger_coordinate < OBJECT(current_object)->x_int)
		{
			fire_front_chase_step();
			return;
		}
		fire_front_load_record(cursor);
		return;
	default:
		invalid_jump(0x8B05Du + (direction >> 8) * 2u);
	}
}

/* Loads the fire front's next path record from path_cursor: screen_threshold, catch_up_factor,
   base_speed and wait_timer (a wait stops its speeds), path moving past it. A negative factor ends the
   chase: chaser_object = 0 and the front is deleted. */
void fire_front_load_record(uint32_t path_cursor)
{
	if (CHASE_STEP_WITH_LIMIT(path_cursor - 0x4u)->catch_up_factor < 0)
	{
		chaser_object = 0u;
		delete_current_object();
		return;
	}
	OBJECT(current_object)->fire_front.screen_threshold = CHASE_STEP_WITH_LIMIT(path_cursor - 0x4u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->fire_front.catch_up_factor = CHASE_STEP_WITH_LIMIT(path_cursor - 0x6u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->fire_front.base_speed = CHASE_STEP_WITH_LIMIT(path_cursor - 0x8u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->fire_front.wait_timer = CHASE_STEP_WITH_LIMIT(path_cursor - 0xAu)->wait;
	path_cursor += 2u;
	if ((uint16_t)OBJECT(current_object)->fire_front.wait_timer != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
	}
	OBJECT(current_object)->fire_front.path = path_cursor;
	fire_front_chase_step();
}

/* Projects the fire front into the zone list and picks its speed as the biting head does (base_speed
   within screen_threshold, else the camera's ground speed x catch_up_factor / 0x100), ending hold-backs
   alike. After the wait it moves along its direction (speed_x/y set); a record with factor 0 narrows its
   box to 0xF2 and sets chaser_stopped. */
void fire_front_chase_step(void)
{
	uint16_t speed = 0;
	uint32_t ignored = 0, ignored_2 = 0, step = 0;
	uint8_t hold_timer = 0;

	project_object(&ignored, &ignored_2);
	add_to_zone_list(ignored);
	if (OBJECT(current_object)->fire_front.holding_back == 0u)
	{
		switch (OBJECT(current_object)->direction >> 8)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->fire_front.screen_threshold)
				speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
			else
			{
				OBJECT(current_object)->fire_front.wait_timer = 0;
				if (OBJECT(current_object)->fire_front.holding_back != 0u)
				{
					OBJECT(current_object)->fire_front.holding_back = 0u;
					OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
					OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
				}
				compute_camera_ground_speed(0u);
				speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
				if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
					speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->fire_front.screen_threshold <= OBJECT(current_object)->screen_x)
				speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
			else
			{
				OBJECT(current_object)->fire_front.wait_timer = 0;
				if (OBJECT(current_object)->fire_front.holding_back != 0u)
				{
					OBJECT(current_object)->fire_front.holding_back = 0u;
					OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
					OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
				}
				compute_camera_ground_speed(0u);
				speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
				if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
					speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
			}
			break;
		default:
			invalid_jump(0x8B117u + (OBJECT(current_object)->direction >> 8) * 2u);
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->fire_front.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->fire_front.holding_back = 0u;
		OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
		OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
		compute_camera_ground_speed(0u);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
			speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
	}
	else
	{
		if (game_mode != 3u)
			OBJECT(current_object)->fire_front.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->fire_front.hold_timer;
		OBJECT(current_object)->fire_front.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->fire_front.holding_back = 0u;
			OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
			OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
			compute_camera_ground_speed(0u);
			speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
			if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
				speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
		}
		else
		{
			switch (OBJECT(current_object)->direction >> 8)
			{
			case 0:
			case 3:
				if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->fire_front.screen_threshold)
					speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
				else
				{
					OBJECT(current_object)->fire_front.wait_timer = 0;
					if (OBJECT(current_object)->fire_front.holding_back != 0u)
					{
						OBJECT(current_object)->fire_front.holding_back = 0u;
						OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
						OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
					}
					compute_camera_ground_speed(0u);
					speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
					if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
						speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
				}
				break;
			case 1:
			case 2:
			case 4:
				if (OBJECT(current_object)->fire_front.screen_threshold <= OBJECT(current_object)->screen_x)
					speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
				else
				{
					OBJECT(current_object)->fire_front.wait_timer = 0;
					if (OBJECT(current_object)->fire_front.holding_back != 0u)
					{
						OBJECT(current_object)->fire_front.holding_back = 0u;
						OBJECT(current_object)->fire_front.screen_threshold = OBJECT(current_object)->fire_front.saved_threshold;
						OBJECT(current_object)->fire_front.base_speed = OBJECT(current_object)->fire_front.saved_base_speed;
					}
					compute_camera_ground_speed(0u);
					speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->fire_front.catch_up_factor) >> 8;
					if ((int16_t)speed <= OBJECT(current_object)->fire_front.base_speed)
						speed = (uint16_t)OBJECT(current_object)->fire_front.base_speed;
				}
				break;
			default:
				invalid_jump(0x8B117u + (OBJECT(current_object)->direction >> 8) * 2u);
			}
		}
	}
	chaser_speed = speed;
	if (OBJECT(current_object)->fire_front.wait_timer > 0)
	{
		if ((uint16_t)CHASE_STEP_WITH_LIMIT(OBJECT(current_object)->fire_front.path)[-1].catch_up_factor == 0u)
		{
			OBJECT(current_object)->hitbox_size_x = 0xF2u;
			OBJECT(current_object)->hitbox_offset_x = 0;
			chaser_stopped = 0xFFu;
		}
		return;
	}
	switch (OBJECT(current_object)->direction >> 8)
	{
	case 0:
		step = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->y -= step;
		OBJECT(current_object)->speed_y = 0u - step;
		OBJECT(current_object)->speed_x = 0;
		if ((uint16_t)CHASE_STEP_WITH_LIMIT(OBJECT(current_object)->fire_front.path)[-1].catch_up_factor == 0u)
		{
			OBJECT(current_object)->hitbox_size_x = 0xF2u;
			OBJECT(current_object)->hitbox_offset_x = 0;
			chaser_stopped = 0xFFu;
		}
		return;
	case 1:
		step = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x += step;
		OBJECT(current_object)->speed_x = step;
		OBJECT(current_object)->speed_y = 0;
		if ((uint16_t)CHASE_STEP_WITH_LIMIT(OBJECT(current_object)->fire_front.path)[-1].catch_up_factor == 0u)
		{
			OBJECT(current_object)->hitbox_size_x = 0xF2u;
			OBJECT(current_object)->hitbox_offset_x = 0;
			chaser_stopped = 0xFFu;
		}
		return;
	case 2:
	case 4:
		step = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->y += step;
		OBJECT(current_object)->speed_y = step;
		OBJECT(current_object)->speed_x = 0;
		if ((uint16_t)CHASE_STEP_WITH_LIMIT(OBJECT(current_object)->fire_front.path)[-1].catch_up_factor == 0u)
		{
			OBJECT(current_object)->hitbox_size_x = 0xF2u;
			OBJECT(current_object)->hitbox_offset_x = 0;
			chaser_stopped = 0xFFu;
		}
		return;
	case 3:
		step = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x -= step;
		OBJECT(current_object)->speed_x = 0u - step;
		OBJECT(current_object)->speed_y = 0;
		if ((uint16_t)CHASE_STEP_WITH_LIMIT(OBJECT(current_object)->fire_front.path)[-1].catch_up_factor == 0u)
		{
			OBJECT(current_object)->hitbox_size_x = 0xF2u;
			OBJECT(current_object)->hitbox_offset_x = 0;
			chaser_stopped = 0xFFu;
		}
		return;
	default:
		invalid_jump(0x8B193u + (OBJECT(current_object)->direction >> 8) * 2u);
	}
}

/* A grate in a row led by its parent (current_player is borrowed for the leader): a platform, drawn at
   screen_y + depth_offset; within 0x60 of the leader it rattles (z up and down 1), and when the leader
   reaches it in y or x by its direction it plays sound 0xD and falls (falling_grate_animation), a
   platform at first, until off the bottom. */
void falling_grate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint8_t animation_timer = 0;

	switch (OBJECT(current_object)->falling_grate.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->falling_grate.state++;
		OBJECT(current_object)->falling_grate.rattle_step = 1;
		project_object(&ignored, &ignored_2);
	case 1:
		update_screen_position(&ignored, &ignored_2);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &ignored_2, &ignored_3);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x60u))
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
			}
			break;
		case 1:
			if (OBJECT(current_player)->y_int <= OBJECT(current_object)->y_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &ignored_2, &ignored_3);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if ((int16_t)(OBJECT(current_player)->y_int - 0x60u) <= OBJECT(current_object)->y_int)
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		case 2:
			if (OBJECT(current_object)->x_int <= OBJECT(current_player)->x_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &ignored_2, &ignored_3);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x60u))
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		case 3:
			if (OBJECT(current_player)->x_int <= OBJECT(current_object)->x_int)
			{
				OBJECT(current_object)->falling_grate.state++;
				OBJECT(current_object)->speed_z = 0;
				OBJECT(current_object)->animation_speed = 6u;
				OBJECT(current_object)->animation_timer = 0;
				OBJECT(current_object)->animation_frame = 0xFFu;
				OBJECT(current_object)->animation = falling_grate_animation_ADDRESS;
				sound_id = 0xDu;
				play_sound(sound_id, &ignored_2, &ignored_3);
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			if ((int16_t)(OBJECT(current_player)->x_int - 0x60u) <= OBJECT(current_object)->x_int)
			{
				animation_timer = OBJECT(current_object)->animation_timer;
				OBJECT(current_object)->animation_timer = animation_timer - 1u;
				if ((int8_t)animation_timer <= 1)
				{
					OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
					OBJECT(current_object)->floor_z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->z_int += OBJECT(current_object)->falling_grate.rattle_step;
					OBJECT(current_object)->falling_grate.rattle_step = 0u - OBJECT(current_object)->falling_grate.rattle_step;
				}
				add_to_platform_list();
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
					return;
				depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = depth;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			break;
		default:
			invalid_jump(0x8B256u + OBJECT(current_object)->direction * 2u);
		}
		add_to_platform_list();
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->falling_grate.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->speed_z += 0x2000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->animation_timer >= 0)
		{
			animate(&ignored_2, &ignored_4, &at_last_step);
			if ((int8_t)at_last_step < 0)
				OBJECT(current_object)->animation_timer = -1;
		}
		if (OBJECT(current_object)->screen_y > 0x1C0)
		{
			delete_current_object();
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
			add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->falling_grate.depth_offset);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x8B224u + OBJECT(current_object)->falling_grate.state * 3u);
	}
}

/* Waits (as its own handler) for scroll_position 0x3780 or biting_head_done; then the rising saw: x, y,
   z and path from rising_saw_paths (by its sprite_frame), rising_saws[sprite_frame] = it, a 0x50 x 0x40
   box 0x20 high of kind 0x26 (knocks players off); with 4 or more objects free its blade and sparks;
   then rising_saw_rise. */
void rising_saw_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, object_2 = 0, path = 0;
	uint8_t blades_left = 0;

	OBJECT(current_object)->handler = rising_saw_init_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	path = rising_saw_paths[OBJECT(current_object)->sprite_frame] + rising_saw_paths_ADDRESS;
	OBJECT(current_object)->x_int = RISING_SAW_PATH_START(path)->x;
	OBJECT(current_object)->x_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->y_int = RISING_SAW_PATH_START(path - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	path += 2u;
	OBJECT(current_object)->z_int = RISING_SAW_PATH_START(path - 0x4u)->z;
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->rising_saw.path = path + 2u;
	rising_saws[OBJECT(current_object)->sprite_frame] = current_object;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object_2);
	OBJECT(current_object)->previous_floor_z_int = OBJECT(current_object)->z_int;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->rising_saw.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->rising_saw.projected_x_fraction = 0u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_offset_x = -0x10;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->hitbox_size_x = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x26u;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->rising_saw.holding_back = 0u;
	OBJECT(current_object)->rising_saw.wait_timer = 0;
	if (free_object_stack < 4)
		return;
	saw_blade_offset_x = 8u;
	saw_blades_left = 1;
	do
	{
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x1002u;
		OBJECT(object_2)->sprite_frame = 0x7F98u;
		OBJECT(object_2)->sprite_palette = 0x1190u;
		OBJECT(object_2)->sprite_offset_x = saw_blade_offset_x;
		saw_blade_offset_x += 0x10u;
		OBJECT(object_2)->sprite_offset_y = 0;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->saw_blade.next_shaft_z = OBJECT(current_object)->z_int;
		OBJECT(object_2)->saw_blade.next_shaft_z += 0x1Du;
		OBJECT(object_2)->saw_blade.state = 0u;
		OBJECT(object_2)->saw_blade.sound_timer = 0;
		OBJECT(object_2)->handler = saw_blade_update_ADDRESS;
		OBJECT(object_2)->animation = saw_blade_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 1u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->handler = saw_blade_update_ADDRESS;
		OBJECT(object_2)->parent = current_object;
		object = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x100Au;
		OBJECT(object_2)->sprite_frame = 0x8378u;
		OBJECT(object_2)->sprite_palette = 0x1200u;
		OBJECT(object_2)->sprite_offset_x = -4;
		OBJECT(object_2)->sprite_offset_y = -0x30;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->handler = saw_sparks_update_ADDRESS;
		OBJECT(object_2)->animation = saw_sparks_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->handler = saw_sparks_update_ADDRESS;
		OBJECT(object_2)->parent = object;
		OBJECT(object)->child = object_2;
		blades_left = saw_blades_left;
		saw_blades_left = blades_left - 1u;
	} while ((int8_t)blades_left > 1);
	OBJECT(current_object)->handler = rising_saw_rise_ADDRESS;
}

/* Each frame of the rising saw: reads its path records (on scroll_position or its z reaching the
   record's height; a negative factor deletes it) and holds back like the chasers (screen_threshold
   0x130). After the wait it rises at base_speed, or with the camera below screen_threshold, along its
   floor's slope; touchable. */
void rising_saw_rise(void)
{
	uint16_t speed = 0;
	uint32_t attribute_record = 0, cursor = 0, ignored = 0, point_x = 0, point_y = 0, projected_dx = 0, slope_step = 0;
	uint8_t hold_timer = 0;
	uint16_t wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		if ((uint16_t)OBJECT(current_object)->rising_saw.wait_timer == 0u)
		{
			cursor = OBJECT(current_object)->rising_saw.path;
			if (CHASE_STEP_WITH_LIMIT(cursor)->position <= scroll_position || OBJECT(current_object)->z_int <= CHASE_STEP_WITH_LIMIT(cursor)->limit)
			{
				cursor += 4u;
				if (CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->catch_up_factor < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->rising_saw.screen_threshold = CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->screen_threshold;
				cursor += 2u;
				OBJECT(current_object)->rising_saw.catch_up_factor = CHASE_STEP_WITH_LIMIT(cursor - 0x6u)->catch_up_factor;
				cursor += 2u;
				OBJECT(current_object)->rising_saw.base_speed = CHASE_STEP_WITH_LIMIT(cursor - 0x8u)->base_speed;
				cursor += 2u;
				OBJECT(current_object)->rising_saw.wait_timer = CHASE_STEP_WITH_LIMIT(cursor - 0xAu)->wait;
				OBJECT(current_object)->rising_saw.path = cursor + 2u;
			}
		}
	}
	else if (OBJECT(current_object)->rising_saw.holding_back == 0u)
	{
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->rising_saw.holding_back = 0xFFu;
			OBJECT(current_object)->rising_saw.saved_base_speed = OBJECT(current_object)->rising_saw.base_speed;
			OBJECT(current_object)->rising_saw.base_speed = 0;
			OBJECT(current_object)->rising_saw.hold_start_position = scroll_position;
			OBJECT(current_object)->rising_saw.hold_timer = 0xFFu;
			OBJECT(current_object)->rising_saw.saved_threshold = OBJECT(current_object)->rising_saw.screen_threshold;
			OBJECT(current_object)->rising_saw.screen_threshold = 0x130;
		}
		else if ((uint16_t)OBJECT(current_object)->rising_saw.wait_timer == 0u)
		{
			cursor = OBJECT(current_object)->rising_saw.path;
			if (CHASE_STEP_WITH_LIMIT(cursor)->position <= scroll_position || OBJECT(current_object)->z_int <= CHASE_STEP_WITH_LIMIT(cursor)->limit)
			{
				cursor += 4u;
				if (CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->catch_up_factor < 0)
				{
					delete_current_object();
					return;
				}
				OBJECT(current_object)->rising_saw.screen_threshold = CHASE_STEP_WITH_LIMIT(cursor - 0x4u)->screen_threshold;
				cursor += 2u;
				OBJECT(current_object)->rising_saw.catch_up_factor = CHASE_STEP_WITH_LIMIT(cursor - 0x6u)->catch_up_factor;
				cursor += 2u;
				OBJECT(current_object)->rising_saw.base_speed = CHASE_STEP_WITH_LIMIT(cursor - 0x8u)->base_speed;
				cursor += 2u;
				OBJECT(current_object)->rising_saw.wait_timer = CHASE_STEP_WITH_LIMIT(cursor - 0xAu)->wait;
				OBJECT(current_object)->rising_saw.path = cursor + 2u;
			}
		}
	}
	if (OBJECT(current_object)->rising_saw.holding_back != 0u)
	{
		if ((int16_t)(scroll_position - OBJECT(current_object)->rising_saw.hold_start_position) >= 0x300)
		{
			OBJECT(current_object)->rising_saw.holding_back = 0u;
			OBJECT(current_object)->rising_saw.screen_threshold = OBJECT(current_object)->rising_saw.saved_threshold;
			OBJECT(current_object)->rising_saw.base_speed = OBJECT(current_object)->rising_saw.saved_base_speed;
		}
		else
		{
			if (no_player_in_play != 0u)
				OBJECT(current_object)->rising_saw.hold_timer = 0xFFu;
			hold_timer = OBJECT(current_object)->rising_saw.hold_timer;
			OBJECT(current_object)->rising_saw.hold_timer = hold_timer - 1u;
			if (hold_timer == 0u)
			{
				OBJECT(current_object)->rising_saw.holding_back = 0u;
				OBJECT(current_object)->rising_saw.screen_threshold = OBJECT(current_object)->rising_saw.saved_threshold;
				OBJECT(current_object)->rising_saw.base_speed = OBJECT(current_object)->rising_saw.saved_base_speed;
			}
		}
	}
	update_screen_position(&slope_step, &ignored);
	if (OBJECT(current_object)->screen_y <= OBJECT(current_object)->rising_saw.screen_threshold)
		speed = (uint16_t)OBJECT(current_object)->rising_saw.base_speed;
	else
	{
		OBJECT(current_object)->rising_saw.wait_timer = 0;
		compute_camera_ground_speed(0u);
		speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->rising_saw.catch_up_factor) >> 8;
		if ((int16_t)speed <= OBJECT(current_object)->rising_saw.base_speed)
			speed = (uint16_t)OBJECT(current_object)->rising_saw.base_speed;
	}
	OBJECT(current_object)->speed_fraction = speed;
	wait_timer = OBJECT(current_object)->rising_saw.wait_timer;
	OBJECT(current_object)->rising_saw.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		add_to_touchable_list(slope_step);
		return;
	}
	OBJECT(current_object)->rising_saw.wait_timer = 0;
	OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	attribute_record = OBJECT(current_object)->floor_attribute + floor_types_ADDRESS;
	slope_step = ((uint32_t)FLOOR_ATTRIBUTE_RECORD(attribute_record)->x_per_rise * (int16_t)OBJECT(current_object)->speed_fraction) >> 4;
	projected_dx = slope_step;
	if (FLOOR_ATTRIBUTE_RECORD(attribute_record)->axis != 0u)
		projected_dx = 0u - projected_dx;
	OBJECT(current_object)->rising_saw.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->rising_saw.projected_x_whole;
	OBJECT(current_object)->projected_y -= slope_step >> 1;
	point_x = OBJECT(current_object)->rising_saw.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	add_to_touchable_list(slope_step);
}

/* The saw's blade by saw_blade.state: 0 starts 0x30 back in y with two clip entries and sound 0xA; 1
   slides 2 a frame to the saw, clipped at the saw's line; 2 frees the clips; 3 follows the saw, leaving
   a shaft piece (saw_shaft_update) every 0x1D it rises, sound 8 every 0x3C frames, drawn while screen_y
   is under 0x140. */
void saw_blade_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, object_4 = 0;
	uint32_t clip_x = 0, clips_left = 0, depth = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0, object_2 = 0, object_3 = 0, object_5 = 0, point_y = 0, z = 0;
	uint16_t sound_timer = 0;

	switch (OBJECT(current_object)->saw_blade.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->saw_blade.state++;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->y_int -= 0x30u;
		OBJECT(current_object)->z = OBJECT(object)->z;
		clips_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored_2, &object_2);
			object_5 = object_2 + 2u;
			OBJECT(object_5 - 0x2u)->sprite_flags = 0x5000u;
			object_5 += 2u;
			OBJECT(object_5 - 0x4u)->sprite_frame = 0xE0u;
			object_5 += 2u;
			OBJECT(object_5 - 0x6u)->sprite_palette = 0u;
			object_5 += 2u;
			OBJECT(object_5 - 0x8u)->sprite_scale = 0x140u;
			object_5 += 2u;
			OBJECT(object_5 - 0xAu)->sprite_layer = 0u;
			object_5 += 2u;
			OBJECT(object_5 - 0xCu)->screen_y = 0;
			object_5 += 2u;
			OBJECT(object_5 - 0xEu)->screen_x = 0;
			OBJECT(object_5 - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			clips_left--;
		} while (clips_left != 0u);
		sound_id = 0xAu;
		play_sound(sound_id, &ignored_3, &ignored_2);
		return;
	case 1:
		OBJECT(current_object)->y += 0x20000u;
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if (OBJECT(object)->y_int <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->saw_blade.state++;
			OBJECT(current_object)->y = OBJECT(object)->y;
		}
		animate(&ignored_3, &ignored_4, &ignored_5);
		project_object(&ignored, &ignored_3);
		OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y;
		depth = OBJECT(current_object)->draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		clip_x = OBJECT(current_object)->x;
		point_y = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->y;
		z = OBJECT(current_object)->z;
		project_point(z, &ignored_3, &clip_x, &point_y);
		clip_x = (uint16_t)(clip_x + OBJECT(current_object)->sprite_offset_x);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_palette = clip_x;
		depth = OBJECT(object_cursor)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->saw_blade.state++;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		object_4 = (uint16_t)current_object;
		free_object_2(object_4);
		current_object = object_cursor;
	case 3:
		object_3 = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->x = OBJECT(object_3)->x;
		OBJECT(current_object)->y = OBJECT(object_3)->y;
		OBJECT(current_object)->z = OBJECT(object_3)->z;
		project_object(&ignored, &ignored_3);
		if ((int16_t)(OBJECT(current_object)->saw_blade.next_shaft_z - OBJECT(current_object)->z_int) >= 0x1D)
		{
			OBJECT(current_object)->saw_blade.next_shaft_z -= 0x1Du;
			allocate_object(&free_count, &object_2);
			if (free_count != 0u)
			{
				OBJECT(object_2)->sprite_flags = 0x1004u;
				OBJECT(object_2)->sprite_frame = 0x7FC0u;
				OBJECT(object_2)->sprite_palette = 0x11A0u;
				OBJECT(object_2)->sprite_offset_x = 8;
				OBJECT(object_2)->sprite_offset_y = -0x1C;
				OBJECT(object_2)->flags = 0u;
				OBJECT(object_2)->saw_shaft_update.state = 0u;
				OBJECT(object_2)->animation = saw_shaft_animation_ADDRESS;
				OBJECT(object_2)->animation_frame = 1u;
				OBJECT(object_2)->projected_x = OBJECT(current_object)->projected_x;
				OBJECT(object_2)->projected_y = OBJECT(current_object)->projected_y;
				OBJECT(object_2)->z = OBJECT(current_object)->z;
				OBJECT(object_2)->saw_shaft_update.next_step_z = OBJECT(object_2)->z_int;
				OBJECT(object_2)->handler = saw_shaft_update_ADDRESS;
				OBJECT(object_2)->parent = current_object;
			}
		}
		animate(&ignored_3, &ignored_4, &ignored_5);
		sound_timer = OBJECT(current_object)->saw_blade.sound_timer;
		OBJECT(current_object)->saw_blade.sound_timer = sound_timer - 1u;
		if ((int16_t)sound_timer <= 1)
		{
			OBJECT(current_object)->saw_blade.sound_timer = 0x3C;
			sound_id = 8u;
			play_sound(sound_id, &ignored_3, &ignored_2);
		}
		if (OBJECT(current_object)->screen_y >= 0x140)
			return;
		OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y;
		ignored_3 = queue_sprite(ignored_3);
		return;
	default:
		invalid_jump(0x8B6FBu + OBJECT(current_object)->saw_blade.state * 3u);
	}
}

/* The blade's sparks: at the blade's screen position plus their sprite offsets, animated, drawn at the
   blade's draw_depth + 1 while screen_y is under 0x168 and the saw's base_speed is not 0. */
void saw_sparks_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(current_object)->sprite_offset_x;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->sprite_offset_y;
	animate(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->screen_y < 0x168)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((uint16_t)OBJECT(LINKED(object, OBJECT(object)->parent))->rising_saw.base_speed != 0u)
		{
			depth = (uint16_t)(OBJECT(object)->draw_depth + 1u);
			queue_sprite_at_depth(&ignored_4, &depth);
			return;
		}
	}
}

/* A piece of shaft left under the blade: steps saw_shaft_animation each time its parent (the blade)
   rises 5 more, until the animation wraps (drawn at depth 0xFFE9), then stays, drawn at depth 0xFFF7,
   until off screen. */
void saw_shaft_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, off_screen = 0, wrapped = 0;

	switch (OBJECT(current_object)->saw_shaft_update.state)
	{
	case 0:
	case 2:
		if ((int16_t)(OBJECT(current_object)->saw_shaft_update.next_step_z - OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->z_int) >= 5)
		{
			OBJECT(current_object)->saw_shaft_update.next_step_z -= 5u;
			wrapped = 0u;
			animate_step(&ignored, &wrapped, &ignored_2);
			if ((uint8_t)wrapped != 0u)
				OBJECT(current_object)->saw_shaft_update.state++;
		}
		update_screen_position(&ignored_3, &ignored);
		depth = 0xFFE9u;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	case 1:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			update_screen_position(&ignored_3, &ignored);
			depth = 0xFFF7u;
			queue_sprite_at_depth(&ignored_3, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		invalid_jump(0x8B957u + OBJECT(current_object)->saw_shaft_update.state * 2u);
	}
}

/* Places the grate steps' builder at x -0x3BD, y 0x4F4, z 8 (row_projected_x/y from there), finds its
   floor, sets rows_left 0xF and clears rising_saws (rising_saws[0], rising_saw_1 and rising_saw_2);
   build_grate_steps runs it. */
void grate_steps_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->x = -0x3BD0000;
	OBJECT(current_object)->y = 0x4F40000;
	OBJECT(current_object)->z = 0x80000;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->grate_steps.row_projected_x = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->spawn_arg_2 = 0u;
	OBJECT(current_object)->grate_steps.row_projected_y = OBJECT(current_object)->projected_y;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->grate_steps.rows_left = 0xF;
	rising_saws[0] = 0xFFFFu;
	rising_saw_1 = 0xFFFFu;
	rising_saw_2 = 0xFFFFu;
	OBJECT(current_object)->handler = build_grate_steps_ADDRESS;
}

/* Builds the grating steps, one row a frame once 6 objects are free: three steps (frame 0x7FF0,
   grate_step_update) 0x35 apart in projected x, each with its column 0-2 in kind and cut_z 0x1E below
   it; each row 0x1E lower down the floor's slope. Deletes itself after 15 rows. */
void build_grate_steps(void)
{
	uint32_t ignored = 0, object = 0, slope_shift = 0;
	uint8_t rows_to_build = 0;
	uint16_t rows_left = 0;

	if (free_object_stack < 6)
		return;
	grate_rows_to_build = 1;
	do
	{
		OBJECT(current_object)->projected_y = OBJECT(current_object)->build_grate_steps.row_projected_y;
		OBJECT(current_object)->projected_x = OBJECT(current_object)->build_grate_steps.row_projected_x_whole;
		grate_step_number = 0;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x1009u;
			OBJECT(object)->sprite_frame = 0x7FF0u;
			OBJECT(object)->sprite_palette = 0x11B0u;
			OBJECT(object)->sprite_offset_x = 0;
			OBJECT(object)->sprite_offset_y = 0;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->screen_y = 0;
			OBJECT(object)->kind = grate_step_number;
			OBJECT(object)->handler = grate_step_update_ADDRESS;
			OBJECT(object)->animation = grate_step_cut_animation_ADDRESS;
			OBJECT(object)->animation_frame = 0u;
			OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
			OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
			OBJECT(object)->z = OBJECT(current_object)->z;
			OBJECT(object)->grate_step_update.cut_z = OBJECT(object)->z_int;
			OBJECT(object)->grate_step_update.copied_animation_speed = OBJECT(object)->animation_speed;
			OBJECT(object)->grate_step_update.copied_animation_timer = OBJECT(object)->animation_timer;
			OBJECT(object)->grate_step_update.cut_z += 0x1Eu;
			OBJECT(current_object)->projected_x += 0x35u;
			OBJECT(current_object)->projected_y -= 0x1A8000u;
			grate_step_number++;
		} while (grate_step_number < 3);
		slope_shift = FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->x_per_rise * 0x1Eu;
		OBJECT(current_object)->build_grate_steps.row_projected_x += slope_shift;
		OBJECT(current_object)->build_grate_steps.row_projected_y += slope_shift >> 1;
		OBJECT(current_object)->z_int += 0x1Eu;
		rows_to_build = grate_rows_to_build;
		grate_rows_to_build = rows_to_build - 1u;
	} while ((int8_t)rows_to_build > 1);
	rows_left = OBJECT(current_object)->build_grate_steps.rows_left;
	OBJECT(current_object)->build_grate_steps.rows_left = rows_left - 1u;
	if ((int16_t)rows_left > 1)
		return;
	delete_current_object();
}

/* A grating step: once the top of the saw of its column (rising_saws[kind], 0x20 above its z) reaches
   cut_z it is cut: cut_z climbs 7 a frame with grate_step_cut_animation, debris and sound 0x28 while
   within 0x20 of the saw's top. Drawn at depth 0xFFF8 unless above the screen; deleted below it. */
void grate_step_update(void)
{
	uint8_t sound_id = 0;
	uint16_t cut_gap = 0;
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;
	uint16_t cut_z = 0, saw_top_z = 0;

	if (OBJECT(current_object)->screen_y >= 0x118)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		object = (current_object & ~0xFFFFu) | rising_saws[OBJECT(current_object)->kind];
		if ((int16_t)object >= 0)
		{
			cut_z = OBJECT(current_object)->grate_step_update.cut_z;
			saw_top_z = OBJECT(object)->z_int - 0x20u;
			cut_gap = (uint16_t)(saw_top_z - cut_z);
			if ((int16_t)cut_z < (int16_t)saw_top_z)
			{
				update_screen_position(&ignored_4, &ignored);
				if (OBJECT(current_object)->screen_y < -0x38)
					return;
				depth = 0xFFF8u;
				queue_sprite_at_depth(&ignored_4, &depth);
				return;
			}
			if ((int16_t)cut_gap >= -0x20)
			{
				spawn_grate_step_debris();
				sound_id = 0x28u;
				play_sound(sound_id, &ignored, &ignored_2);
			}
			OBJECT(current_object)->grate_step_update.cut_z -= 7u;
			at_last_step = 0u;
			animate_step(&ignored, &ignored_3, &at_last_step);
			if ((uint8_t)at_last_step != 0u)
				OBJECT(current_object)->animation_frame = 0xFFu;
		}
	}
	update_screen_position(&ignored_4, &ignored);
	if (OBJECT(current_object)->screen_y < -0x38)
		return;
	depth = 0xFFF8u;
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Spawns a grate step's debris when enough objects are free: the pieces of grate_step_debris (its first
   word the count), each at the step's projected position plus the record's offsets and at cut_z, with
   the record's speeds plus a random part and its animation (grate_step_debris_fly). */
void spawn_grate_step_debris(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, random_value = 0, record = 0;
	uint32_t stack_4 = 0;
	uint16_t debris_left = 0;

	record = grate_step_debris_ADDRESS;
	if (free_object_stack < (int16_t)grate_step_debris)
		return;
	grate_debris_left = GRATE_STEP_DEBRIS_LIST(record)->count;
	grate_debris_record = record + 2u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x11C0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = grate_step_debris_fly_ADDRESS;
		OBJECT(object)->grate_step_debris_view.projected_x_whole = OBJECT(current_object)->projected_x;
		OBJECT(object)->grate_step_debris_view.projected_x_fraction = 0u;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->z_int = OBJECT(current_object)->spawn_grate_step_debris.cut_z;
		OBJECT(object)->z_fraction = 0u;
		record = grate_debris_record;
		OBJECT(object)->grate_step_debris_view.projected_x_whole += GRATE_DEBRIS_PIECE(record)->dx;
		record += 2u;
		OBJECT(object)->projected_y_int += GRATE_DEBRIS_PIECE(record - 0x2u)->dy;
		record += 2u;
		OBJECT(object)->speed_x = (uint32_t)GRATE_DEBRIS_PIECE(record - 0x4u)->speed_x << 8;
		record += 2u;
		OBJECT(object)->speed_z = (uint32_t)GRATE_DEBRIS_PIECE(record - 0x6u)->speed_z << 8;
		stack_4 = record + 2u;
		next_random(&random_value, &ignored_2, &ignored_3);
		record = stack_4;
		OBJECT(object)->speed_x += (int16_t)random_value;
		random_value = (int32_t)random_value >> 16;
		OBJECT(object)->speed_z += random_value;
		OBJECT(object)->animation = GRATE_DEBRIS_PIECE(record - 0x8u)->animation + grate_step_debris_animations_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		grate_debris_record = record + 2u;
		debris_left = grate_debris_left;
		grate_debris_left = debris_left - 1u;
	} while ((int16_t)debris_left > 1);
}

/* A piece of grate step debris: falls (gravity 0x6000), moves speed_x in projected x, animates at depth
   0xE0 and is deleted off screen. */
void grate_step_debris_fly(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->grate_step_debris_view.fixed_projected_x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->grate_step_debris_view.projected_x_whole;
	update_screen_position(&ignored, &ignored_2);
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		animate(&ignored_2, &ignored_3, &ignored_4);
		depth = 0xE0u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Waits like rising_saw_init (scroll_position 0x3780 or biting_head_done), then builds the base of the
   saw's shaft: three pieces (0x7FE8, 0x7FB8, then 0x7FE8 with sprite_flags 0x80) from 0x3A above its
   floor, each 0x1D lower down the floor's slope (saw_shaft_base_draw); then deletes itself. */
void build_saw_shaft_base(void)
{
	uint16_t piece_frame = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, slope_shift = 0;

	OBJECT(current_object)->handler = build_saw_shaft_base_ADDRESS;
	if (scroll_position < 0x3780u && biting_head_done == 0u)
		return;
	OBJECT(current_object)->flags = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0x3Au;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->build_saw_shaft_base.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->build_saw_shaft_base.projected_x_fraction = 0u;
	saw_shaft_part = 0;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x11A0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->saw_shaft_base_draw.state = 1u;
		OBJECT(object)->handler = saw_shaft_base_draw_ADDRESS;
		piece_frame = 0x7FB8u;
		if ((uint16_t)saw_shaft_part == 0u)
			piece_frame = 0x7FE8u;
		else if ((uint16_t)saw_shaft_part == 2u)
		{
			OBJECT(object)->sprite_flags |= 0x80u;
			piece_frame = 0x7FE8u;
		}
		OBJECT(object)->sprite_frame = piece_frame;
		OBJECT(object)->projected_x = OBJECT(current_object)->build_saw_shaft_base.projected_x_whole;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(current_object)->z_int += 0x1Du;
		slope_shift = FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->x_per_rise * 0x1Du;
		OBJECT(current_object)->build_saw_shaft_base.fixed_projected_x += slope_shift;
		OBJECT(current_object)->projected_y += slope_shift >> 1;
		saw_shaft_part++;
	} while (saw_shaft_part < 3);
	delete_current_object();
}

/* Draws a piece of the shaft base at depth 0xFFF7 until it has left the screen (check_left_screen), then
   deletes it. */
void saw_shaft_base_draw(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	update_screen_position(&ignored, &ignored_2);
	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		depth = 0xFFF7u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}
