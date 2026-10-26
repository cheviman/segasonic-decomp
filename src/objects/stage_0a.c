/**
 *    Name:        objects/stage_0a.c
 *    Description: stage A's objects, the sand: the cactus, the centipede, the wall pieces, the shaking blocks, the sand rock, the sand arch, the stepped wall, the tornado and the whirlwinds it throws, quicksand, sinkholes, the collapsing sand blocks
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Object type 19: a cactus (sprite 0x77D8, palette 0x9070), flipped (sprite_flags 0x40) when spawn_arg_1
   is set, with a small shadow part (sprite 0x5D80, scale 0x60); then cactus_update. */
void cactus_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x77D8u;
	OBJECT(current_object)->sprite_palette = 0x9070u;
	OBJECT(current_object)->handler = cactus_update_ADDRESS;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	OBJECT(current_object)->sprite_scale = 0x60u;
}

/* Rests on its floor until the tornado (tornado_object) comes within 0x60; then, as sprite 0x77E0, it
   spirals up (angle - 0x10, spiral_speed + 0x18 and speed_z 0xC00 more upward each frame) until removed
   off screen. Drawn with its shadow. The z_int - 3 after fall_to_floor is never reached: its distance
   test has already failed. */
void cactus_update(void)
{
	uint32_t bucket = 0, deleted = 0, direction = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, speed_x = 0, speed_y = 0, spin_speed = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->cactus.lifted != 0u)
	{
		OBJECT(current_object)->cactus.angle -= 0x10u;
		OBJECT(current_object)->cactus.angle &= 0x3FFu;
		OBJECT(current_object)->cactus.spiral_speed += 0x18u;
		direction = OBJECT(current_object)->cactus.angle;
		spin_speed = OBJECT(current_object)->cactus.spiral_speed;
		direction_to_speed(spin_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		OBJECT(current_object)->speed_z -= 0xC00u;
		queue_sprite_at_floor();
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	r14 = tornado_object;
	distance_to_object();
	if (nearest_player_distance <= 0x60)
	{
		OBJECT(current_object)->cactus.lifted = 1u;
		OBJECT(current_object)->sprite_frame = 0x77E0u;
		OBJECT(current_object)->speed_z = -0xC001;
		OBJECT(current_object)->cactus.angle -= 0x10u;
		OBJECT(current_object)->cactus.angle &= 0x3FFu;
		OBJECT(current_object)->cactus.spiral_speed += 0x18u;
		direction = OBJECT(current_object)->cactus.angle;
		spin_speed = OBJECT(current_object)->cactus.spiral_speed;
		direction_to_speed(spin_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		OBJECT(current_object)->speed_z -= 0xC00u;
		queue_sprite_at_floor();
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	fall_to_floor(&ignored, &ignored_5, &ignored_2);
	if (nearest_player_distance > 0x60 || (uint16_t)OBJECT(current_object)->floor_z_int != (uint16_t)OBJECT(current_object)->z_int)
	{
		queue_sprite_at_floor();
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		return;
	}
	OBJECT(current_object)->z_int -= 3u;
	queue_sprite_at_floor();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = place_sprite_at_cursor_floor();
	queue_sprite_in_bucket(bucket);
}

/* Called by tornado_throw_whirlwinds: a 1-block whirlwind at the tornado's position (palette 0x90F0, box
   8, height 0x28, kind 0x1F), its spin_step the shared whirlwind_spin_step + 0xB (mod 0x40); handler
   whirlwind_aim. The self_copied_byte line copies a byte onto itself. */
uint32_t spawn_whirlwind(uint32_t spawned, uint32_t free_count, uint32_t whirlwind, uint32_t object)
{
	uint32_t ignored = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x01217Cu, &spawned, &ignored, &free_count, &whirlwind, &object);
	if ((uint8_t)spawned != 0u)
	{
		current_object = whirlwind;
		copy_position_to(whirlwind);
		whirlwind_spin_step += 0xBu;
		whirlwind_spin_step &= 0x3Fu;
		OBJECT(current_object)->sprite_palette = 0x90F0u;
		OBJECT(current_object)->whirlwind.spin_step = whirlwind_spin_step;
		OBJECT(current_object)->whirlwind.self_copied_byte = OBJECT(current_object)->whirlwind.self_copied_byte;
		OBJECT(current_object)->hitbox_size_x = 8u;
		OBJECT(current_object)->hitbox_height = 0x28u;
		OBJECT(current_object)->kind = 0x1Fu;
		OBJECT(current_object)->handler = whirlwind_aim_ADDRESS;
	}
	return object;
}

/* face_nearest_player, then heads for the farthest player (each distance_to_player raised by 1 first) at
   speed 3 x nearest_player_distance, for 0xF0 frames of whirlwind_move, starting now. When player 3 is
   the farthest it takes direction_to_player_2. */
void whirlwind_aim(void)
{
	uint16_t farthest_distance = 0;

	face_nearest_player(0u, 0u);
	distance_to_player_1++;
	distance_to_player_2++;
	distance_to_player_3++;
	farthest_distance = (uint16_t)distance_to_player_1;
	OBJECT(current_object)->direction = direction_to_player_1;
	if ((int16_t)farthest_distance < distance_to_player_2)
	{
		farthest_distance = (uint16_t)(distance_to_player_2 + 1u);
		OBJECT(current_object)->direction = direction_to_player_2;
	}
	if ((int16_t)farthest_distance < distance_to_player_3)
		OBJECT(current_object)->direction = direction_to_player_2;
	OBJECT(current_object)->speed = (uint16_t)nearest_player_distance * 3u;
	OBJECT(current_object)->whirlwind.timer = 0xF0;
	OBJECT(current_object)->handler = whirlwind_move_ADDRESS;
	whirlwind_move();
}

/* For whirlwind.timer frames, or until touched: moves along its direction at its speed plus a circle of
   speed 0x100 (spin_angle + spin_step a frame), cycling the four whirlwind_sprites, touchable; removed
   off screen. Then whirlwind_vanish. */
void whirlwind_move(void)
{
	uint32_t aim_speed = 0, deleted = 0, direction = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, speed = 0, speed_x = 0, speed_y = 0, spin_angle = 0;

	if (OBJECT(current_object)->whirlwind.timer <= 0)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->whirlwind.frame_index = 0;
		OBJECT(current_object)->handler = whirlwind_vanish_ADDRESS;
		whirlwind_vanish();
		return;
	}
	OBJECT(current_object)->whirlwind.timer--;
	if ((OBJECT(current_object)->flags & 4u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->whirlwind.frame_index = 0;
		OBJECT(current_object)->handler = whirlwind_vanish_ADDRESS;
		whirlwind_vanish();
		return;
	}
	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 1u)
	{
		spin_angle = (OBJECT(current_object)->whirlwind.spin_angle + OBJECT(current_object)->whirlwind.spin_step) & 0x3FFu;
		OBJECT(current_object)->whirlwind.spin_angle = spin_angle;
		speed = 0x100u;
		direction_to_speed(speed, &spin_angle, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		direction = OBJECT(current_object)->direction;
		aim_speed = OBJECT(current_object)->speed;
		direction_to_speed(aim_speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		frame_index = (uint8_t)OBJECT(current_object)->whirlwind.frame_index;
		frame_index = (uint8_t)(frame_index + 1u);
		frame_index = frame_index & 3u;
		OBJECT(current_object)->whirlwind.frame_index = frame_index;
		OBJECT(current_object)->sprite_frame = whirlwind_sprites[frame_index];
		queue_sprite(aim_speed);
		add_to_touchable_list(frame_index);
	}
}

/* Every other frame shows the next of the eight whirlwind_vanish_sprites, then deletes itself; still
   drawn and touchable meanwhile. */
void whirlwind_vanish(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t odd_frame = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->whirlwind.timer++;
	odd_frame = OBJECT(current_object)->whirlwind.timer & 1u;
	OBJECT(current_object)->whirlwind.timer = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->sprite_frame = whirlwind_vanish_sprites[(uint8_t)OBJECT(current_object)->whirlwind.frame_index];
		OBJECT(current_object)->whirlwind.frame_index++;
		if (OBJECT(current_object)->whirlwind.frame_index > 7)
		{
			delete_object_and_children(&ignored, &ignored_2, &ignored_3);
			return;
		}
	}
	queue_sprite(0u);
	add_to_touchable_list(ignored);
}

/* Object type 20: the centipede's head (sprite 0x7940, kind 0x24, height 0x32, centipede_chase) and ten
   2-part pieces at its position that wait 5, 0xA ... 0x32 frames (path_index) before they follow: nine
   segments (0x7980, centipede_segment_wait) and the tail (0x7A08, centipede_tail_wait, segment_ahead the
   last segment). All have shadows. */
void centipede_init(void)
{
	uint16_t follow_delay = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, previous_segment = 0;

	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_height = 0x32u;
	OBJECT(current_object)->kind = 0x24u;
	OBJECT(current_object)->sprite_frame = 0x7940u;
	OBJECT(current_object)->sprite_palette = 0x9270u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x5D80u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	r14 = 0xAu;
	follow_delay = 0u;
	do
	{
		previous_segment = current_object;
		spawn_count = 2u;
		spawn_object_with_pieces(0x012351u, &ignored, &ignored_2, &free_count, &object, &block);
		copy_position_to(object);
		OBJECT(object)->sprite_frame = 0x7980u;
		OBJECT(object)->sprite_palette = 0x9270u;
		OBJECT(object)->handler = centipede_segment_wait_ADDRESS;
		follow_delay += 5u;
		OBJECT(object)->centipede_segment.path_index = follow_delay;
		current_object = object;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x87A0u;
		OBJECT(object)->sprite_scale = 0xA0u;
		r14 = r14 - 1u;
	} while (r14 != 0u);
	OBJECT(current_object)->centipede_tail.segment_ahead = previous_segment;
	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x7A08u;
	OBJECT(current_object)->sprite_palette = 0x9270u;
	OBJECT(current_object)->handler = centipede_tail_wait_ADDRESS;
	current_object = object_cursor;
	OBJECT(current_object)->handler = centipede_chase_ADDRESS;
	centipede_command = 0u;
	add_to_rider_list();
}

/* The centipede's head: until scroll_position passes spawn_arg_1, centipede_command 0 and centipede_move
   (touchable and aiming with face_nearest_player only while not falling). With no active players it
   holds its last path point and centipede_command 1 holds the body. Then centipede_leave. Its anchor_y
   stores are never read. */
void centipede_chase(void)
{
	uint32_t block_offset = 0, cursor = 0, path_cursor = 0, screen_y = 0, slope_height = 0;
	uint32_t path_write_index = 0;

	move_object_projected(&screen_y, &block_offset, &slope_height, &cursor);
	if (scroll_position <= OBJECT(current_object)->spawn_arg_1)
	{
		if ((int8_t)active_player_count > 0)
		{
			nearest_player_distance = 0;
			centipede_command = 0u;
			if (OBJECT(current_object)->speed_z <= 0)
			{
				screen_y = add_to_touchable_list(screen_y);
				face_nearest_player(screen_y, slope_height);
				if (nearest_player_floor_z >= 0x800)
				{
					OBJECT(current_object)->anchor_y = 0;
					centipede_move(cursor);
					return;
				}
			}
			centipede_move(cursor);
			return;
		}
		path_write_index = (uint8_t)OBJECT(current_object)->centipede.path_write_index;
		path_cursor = path_write_index - 1u;
		if (path_write_index == 0u)
			path_cursor = 0x32u;
		path_cursor = path_cursor * 4u + centipede_path_ADDRESS;
		OBJECT(current_object)->projected_x = CENTIPEDE_PATH_POINT(path_cursor)->projected_x;
		OBJECT(current_object)->projected_y_int = CENTIPEDE_PATH_POINT(path_cursor)->projected_y;
		path_cursor = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		CENTIPEDE_PATH_POINT(path_cursor)->projected_x = OBJECT(current_object)->projected_x;
		path_cursor += 2u;
		CENTIPEDE_PATH_POINT(path_cursor - 0x2u)->projected_y = OBJECT(current_object)->projected_y_int;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->anchor_y = 0x280;
		centipede_command = 1u;
		path_cursor += 2u;
		centipede_hop(&path_cursor, &block_offset);
		if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
			spawn_centipede_sand_puff(path_cursor, block_offset, slope_height, cursor);
		draw_centipede_piece();
		return;
	}
	OBJECT(current_object)->handler = centipede_leave_ADDRESS;
	nearest_player_distance = 0;
	centipede_command = 0u;
	if (OBJECT(current_object)->speed_z <= 0)
	{
		screen_y = add_to_touchable_list(screen_y);
		face_nearest_player(screen_y, slope_height);
		if (nearest_player_floor_z >= 0x800)
		{
			OBJECT(current_object)->anchor_y = 0;
			centipede_move(cursor);
			return;
		}
	}
	centipede_move(cursor);
}

/* The head once scroll_position has passed spawn_arg_1: heads along spawn_arg_2 with centipede_move at
   the speed of a player 0x1F away (nearest_player_distance forced); centipede_command 0, and 9 once it
   is removed off screen, which removes the body. */
void centipede_leave(void)
{
	uint32_t cursor = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &cursor);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
	{
		centipede_command = 9u;
		return;
	}
	centipede_command = 0u;
	OBJECT(current_object)->direction = OBJECT(current_object)->spawn_arg_2;
	nearest_player_distance = 0x1F;
	nearest_player_dx = 0;
	centipede_move(cursor);
}

/* The head's step: sprite centipede_head_frames[(direction / 0x92 + 1) & 7]; speed
   ((nearest_player_distance x 2) & 0x3FF) + 0x100, twice that along direction plus once along
   wobble_angle (- 0x10 a frame), so it weaves. Unless centipede_command is 1 it writes its projected x/y
   to centipede_path. Then hop, sand puff, draw. */
void centipede_move(uint32_t block)
{
	uint16_t saved_direction = 0;
	uint32_t angle_or_path_cursor = 0, chase_speed = 0, direction = 0, head_frame = 0, speed = 0, speed_x = 0, speed_y = 0, wobble_speed_x = 0, wobble_speed_y = 0;

	head_frame = OBJECT(current_object)->direction;
	saved_direction = (uint16_t)head_frame;
	head_frame = (uint16_t)((int16_t)head_frame / 0x92);
	head_frame = (uint16_t)(head_frame + 1u);
	OBJECT(current_object)->sprite_frame = centipede_head_frames[head_frame & 7u];
	direction = saved_direction;
	chase_speed = (uint16_t)nearest_player_distance * 2u;
	chase_speed = (chase_speed & ~0xFFFFu) | (chase_speed & 0x3FFu);
	chase_speed = (chase_speed & ~0xFFFFu) | (uint16_t)(chase_speed + 0x100u);
	r14 = chase_speed;
	chase_speed = chase_speed * 2u;
	direction_to_speed(chase_speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	OBJECT(current_object)->centipede.wobble_angle -= 0x10u;
	OBJECT(current_object)->centipede.wobble_angle &= 0x3FFu;
	angle_or_path_cursor = OBJECT(current_object)->centipede.wobble_angle;
	speed = r14;
	direction_to_speed(speed, &angle_or_path_cursor, &wobble_speed_x, &wobble_speed_y);
	OBJECT(current_object)->speed_x += wobble_speed_x;
	OBJECT(current_object)->speed_y += wobble_speed_y;
	if (centipede_command != 1u)
	{
		angle_or_path_cursor = (uint8_t)OBJECT(current_object)->centipede.path_write_index * 4u + centipede_path_ADDRESS;
		CENTIPEDE_PATH_POINT(angle_or_path_cursor)->projected_x = OBJECT(current_object)->projected_x;
		angle_or_path_cursor += 2u;
		CENTIPEDE_PATH_POINT(angle_or_path_cursor - 0x2u)->projected_y = OBJECT(current_object)->projected_y_int;
		OBJECT(current_object)->centipede.path_write_index++;
		angle_or_path_cursor += 2u;
		if (OBJECT(current_object)->centipede.path_write_index > 0x32)
			OBJECT(current_object)->centipede.path_write_index = 0;
	}
	centipede_hop(&angle_or_path_cursor, &wobble_speed_x);
	if (OBJECT(current_object)->centipede.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(angle_or_path_cursor, wobble_speed_x, wobble_speed_y, block);
	draw_centipede_piece();
}

/* A body segment's start: counts its delay in path_index down while hopping in place, then
   centipede_segment_follow. */
void centipede_segment_wait(void)
{
	uint32_t height = 0, z = 0;

	OBJECT(current_object)->centipede_segment.path_index--;
	if (OBJECT(current_object)->centipede_segment.path_index <= 0)
	{
		OBJECT(current_object)->handler = centipede_segment_follow_ADDRESS;
		centipede_segment_follow();
		return;
	}
	centipede_hop(&height, &z);
	if (OBJECT(current_object)->centipede_segment.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(height, z, 0u, 0u);
	draw_centipede_piece();
}

/* A body segment: removed with its shadow when centipede_command is 9; otherwise takes the next
   centipede_path point (centipede_follow_path) unless centipede_command is 1, then hops (a sand puff on
   landing) and is drawn like the head. */
void centipede_segment_follow(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, path_cursor = 0, slope_height = 0;

	move_object_projected(&path_cursor, &block_offset, &slope_height, &cursor);
	if (centipede_command == 9u)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	if (centipede_command != 1u)
		path_cursor = centipede_follow_path();
	centipede_hop(&path_cursor, &block_offset);
	if (OBJECT(current_object)->centipede_segment.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(path_cursor, block_offset, slope_height, cursor);
	draw_centipede_piece();
}

/* The tail's start: counts its delay in path_index (0x32 frames) down while hopping in place, then
   centipede_tail_follow. */
void centipede_tail_wait(void)
{
	uint32_t height = 0, z = 0;

	OBJECT(current_object)->centipede_tail.path_index--;
	if (OBJECT(current_object)->centipede_tail.path_index <= 0)
	{
		OBJECT(current_object)->handler = centipede_tail_follow_ADDRESS;
		centipede_tail_follow();
		return;
	}
	centipede_hop(&height, &z);
	if (OBJECT(current_object)->centipede_tail.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(height, z, 0u, 0u);
	draw_centipede_piece();
}

/* The tail: centipede_segment_follow, and while following it turns toward segment_ahead
   (direction_to_object) for its sprite, centipede_tail_frames[(direction / 0x92 + 5) & 7]. */
void centipede_tail_follow(void)
{
	uint32_t block_offset = 0, cursor = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, octant = 0, path_cursor = 0, tail_frame = 0;

	move_object_projected(&path_cursor, &block_offset, &octant, &cursor);
	if (centipede_command == 9u)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	if (centipede_command != 1u)
	{
		r14 = OBJECT(current_object)->centipede_tail.segment_ahead;
		octant = direction_to_object();
		tail_frame = OBJECT(current_object)->direction;
		tail_frame = (uint16_t)((int16_t)tail_frame / 0x92);
		tail_frame = (uint16_t)(tail_frame + 1u);
		tail_frame = (uint16_t)(tail_frame + 4u);
		OBJECT(current_object)->sprite_frame = centipede_tail_frames[tail_frame & 7u];
		path_cursor = centipede_follow_path();
	}
	centipede_hop(&path_cursor, &block_offset);
	if (OBJECT(current_object)->centipede_tail.hop_timer == 0x5Au)
		spawn_centipede_sand_puff(path_cursor, block_offset, octant, cursor);
	draw_centipede_piece();
}

/* Puts the piece at centipede_path[path_index] (projected_x and the whole of projected_y), then
   path_index steps on, back to 0 after 0x32. Hands back the address after that entry. */
uint32_t centipede_follow_path(void)
{
	uint32_t path_cursor = 0;

	path_cursor = (uint16_t)OBJECT(current_object)->centipede_follow_path.path_index * 4u + centipede_path_ADDRESS;
	OBJECT(current_object)->projected_x = CENTIPEDE_PATH_POINT(path_cursor)->projected_x;
	path_cursor += 2u;
	OBJECT(current_object)->projected_y_int = CENTIPEDE_PATH_POINT(path_cursor - 0x2u)->projected_y;
	OBJECT(current_object)->centipede_follow_path.path_index++;
	path_cursor += 2u;
	if (OBJECT(current_object)->centipede_follow_path.path_index > 0x32)
		OBJECT(current_object)->centipede_follow_path.path_index = 0;
	return path_cursor;
}

/* The hop of a centipede piece: while hop_timer runs it counts down with speed_z 0x600 more upward each
   frame; at 0 the piece falls (fall_to_floor with 0x4000 of its gravity taken back) and, once z (all 32
   bits) is 8 or more, hop_timer restarts at 0x5A with speed_z 0. Hands back fall_to_floor's height and
   z. */
void centipede_hop(uint32_t *p_height, uint32_t *p_z)
{
	uint32_t ignored = 0;

	if (OBJECT(current_object)->centipede.hop_timer == 0u)
	{
		fall_to_floor(p_height, &ignored, p_z);
		OBJECT(current_object)->speed_z -= 0x4000u;
		if (OBJECT(current_object)->z < 8)
		{
			return;
		}
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->centipede.hop_timer = 0x5Au;
		return;
	}
	OBJECT(current_object)->centipede.hop_timer--;
	OBJECT(current_object)->speed_z -= 0x600u;
}

/* Draws a centipede piece sorted by the floor point under it (queue_sprite_at_floor), puts it in the
   rider list, and draws its shadow part at its floor at depth screen_y - 0xA. */
void draw_centipede_piece(void)
{
	uint32_t depth = 0, ignored_2 = 0;

	queue_sprite_at_floor();
	add_to_rider_list();
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	place_sprite_at_cursor_floor();
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xAu);
	queue_sprite_at_depth(&ignored_2, &depth);
}

/* On the frame a centipede piece lands: a 1-block sand puff at its position, 6 lower (projected_y + 6):
   sprite 0x75A8, palette 0x1020, scale 0x180, lifetime 0x2D frames, handler sand_puff_start. */
void spawn_centipede_sand_puff(uint32_t spawned, uint32_t free_count, uint32_t puff, uint32_t block)
{
	uint32_t ignored = 0;

	spawn_count = 1u;
	spawn_object_with_pieces(0x012698u, &spawned, &ignored, &free_count, &puff, &block);
	if ((uint8_t)spawned == 0u)
		return;
	copy_position_to(puff);
	OBJECT(puff)->projected_y_int += 6u;
	OBJECT(puff)->sand_puff.lifetime = 0x2Du;
	OBJECT(puff)->sprite_frame = 0x75A8u;
	OBJECT(puff)->sprite_palette = 0x1020u;
	OBJECT(puff)->sprite_scale = 0x180u;
	OBJECT(puff)->handler = sand_puff_start_ADDRESS;
}

/* unproject_object and the rider list, then sand_puff_animate from the next frame. */
void sand_puff_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	add_to_rider_list();
	OBJECT(current_object)->handler = sand_puff_animate_ADDRESS;
}

/* Removed over a pit (floor_z_int 0x800 or more) or after its lifetime frames; meanwhile cycles the
   three sand_puff_frames every frame, drawn and in the rider list. */
void sand_puff_animate(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, next_frame = 0;
	uint16_t lifetime = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	if (OBJECT(current_object)->floor_z_int >= 0x800)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	lifetime = OBJECT(current_object)->sand_puff.lifetime;
	OBJECT(current_object)->sand_puff.lifetime = lifetime - 1u;
	if (lifetime == 0u)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	next_frame = (uint16_t)OBJECT(current_object)->sand_puff.frame_index;
	next_frame = (uint16_t)(next_frame + 1u);
	if ((int16_t)next_frame > 2)
		next_frame = 0u;
	OBJECT(current_object)->sand_puff.frame_index = next_frame;
	OBJECT(current_object)->sprite_frame = sand_puff_frames[next_frame];
	queue_sprite(0u);
	add_to_rider_list();
}

/* Object type 22: a piece of green stone wall, sprite wall_piece_frames[spawn_arg_1], palette 0x9080; a
   box from wall_piece_boxes[spawn_arg_1] and height 0x28 it never uses (wall_piece_draw joins no list);
   then wall_piece_draw. */
void wall_piece_init(void)
{
	uint32_t box_record = 0, spawn_arg_1 = 0;

	spawn_arg_1 = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->sprite_frame = wall_piece_frames[spawn_arg_1];
	OBJECT(current_object)->sprite_palette = 0x9080u;
	OBJECT(current_object)->handler = wall_piece_draw_ADDRESS;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	box_record = wall_piece_boxes_ADDRESS + spawn_arg_1 * 6u;
	OBJECT(current_object)->hitbox_size_x = HITBOX_SIZE_AND_OFFSET(box_record)->size_x;
	box_record += 2u;
	OBJECT(current_object)->hitbox_size_y = HITBOX_SIZE_AND_OFFSET(box_record - 0x2u)->size_y;
	box_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = HITBOX_SIZE_AND_OFFSET(box_record - 0x4u)->offset_x;
	OBJECT(current_object)->hitbox_offset_y = HITBOX_SIZE_AND_OFFSET(box_record - 0x4u)->offset_y;
}

/* Unprojects and is removed off screen; drawn at depth screen_y + 0x14 (at most 0xEE) while its screen_y
   is 0xE0 or less, lower down at its screen_y. */
void wall_piece_draw(void)
{
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	if (OBJECT(current_object)->screen_y <= 0xE0)
	{
		depth = (uint16_t)(depth + 0x14u);
		if ((int16_t)depth > 0xEE)
			depth = 0xEEu;
	}
	queue_sprite_at_depth(&ignored, &depth);
}

/* Object type 23: palette 0x9050, a box from shaking_block_boxes[spawn_arg_1] it never uses, and one
   more part: sprite 0x7AA0 with spawn_arg_1 set, else its shadow strip 0x7570 (palette 0x1060); then
   shaking_block_animate (tried again next frame without a free part). */
void shaking_block_init(void)
{
	uint32_t box_record = 0, ignored = 0, ignored_2 = 0, object = 0, spawned = 0;

	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	box_record = shaking_block_boxes_ADDRESS + OBJECT(current_object)->spawn_arg_1 * 6u;
	OBJECT(current_object)->hitbox_size_x = HITBOX_SIZE_AND_OFFSET(box_record)->size_x;
	box_record += 2u;
	OBJECT(current_object)->hitbox_size_y = HITBOX_SIZE_AND_OFFSET(box_record - 0x2u)->size_y;
	box_record += 2u;
	OBJECT(current_object)->hitbox_offset_x = HITBOX_SIZE_AND_OFFSET(box_record - 0x4u)->offset_x;
	OBJECT(current_object)->hitbox_offset_y = HITBOX_SIZE_AND_OFFSET(box_record - 0x4u)->offset_y;
	spawn_count = 1u;
	add_pieces_to_current_object(&spawned, &ignored, &ignored_2, &object);
	if ((uint8_t)spawned == 0u)
		return;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->sprite_frame = 0x7AA0u;
		OBJECT(object)->sprite_palette = 0x9050u;
	}
	else
	{
		OBJECT(object)->sprite_frame = 0x7570u;
		OBJECT(object)->sprite_palette = 0x1060u;
	}
	OBJECT(current_object)->handler = shaking_block_animate_ADDRESS;
}

/* Every 3 frames the next of its three shaking_block_frames (by spawn_arg_1); removed off screen. With
   spawn_arg_1 0 it is drawn under everything (bucket 0xFF) with its shadow part 0x10 left and 7 down
   from its floor point; with 1 on top of everything (bucket 0), its part at -0x28, +0x20. */
void shaking_block_animate(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t bucket_2 = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t frame_timer = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	frame_timer = OBJECT(current_object)->shaking_block.frame_timer;
	OBJECT(current_object)->shaking_block.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->shaking_block.frame_timer = 2u;
		OBJECT(current_object)->sprite_frame = shaking_block_frames[(uint8_t)OBJECT(current_object)->shaking_block.frame_index + OBJECT(current_object)->spawn_arg_1 * 3u];
		OBJECT(current_object)->shaking_block.frame_index++;
		if (OBJECT(current_object)->shaking_block.frame_index > 2)
			OBJECT(current_object)->shaking_block.frame_index = 0;
	}
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket_2 = place_sprite_at_cursor_floor();
		OBJECT(current_object)->screen_x -= 0x10u;
		OBJECT(current_object)->screen_y += 7u;
		queue_sprite_in_bucket(bucket_2);
	}
	else
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0xFFD8u;
		offset_y = 0x20u;
		place_sprite_near_cursor(offset_x, offset_y);
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
}

/* Object type 24: a sandstone boulder (sprite 0x7BA8, palette 0x9010), box 0x22 x 0x21, height 0x20,
   kind 5, its part in the same palette; then sand_rock_update. */
void sand_rock_init(void)
{
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7BA8u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = sand_rock_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
}

/* Falls to its floor; removed off screen. The first touch (flag 4) starts it crumbling
   (sand_rock_crumble), dropping its ring_count rings at angle spawn_arg_2 first when it has any; until
   then sand_rock_draw. */
void sand_rock_update(void)
{
	uint32_t cursor = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, random_value = 0, ring = 0, slope_height = 0;

	unproject_object(&ignored, &ignored_2, &slope_height, &cursor);
	fall_to_floor(&ignored, &ignored_3, &ignored_2);
	delete_if_off_screen(&deleted, &ignored_2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->sand_rock.crumbling != 0u)
	{
		sand_rock_crumble();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		sand_rock_draw();
		return;
	}
	OBJECT(current_object)->sand_rock.crumbling = 1u;
	if (OBJECT(current_object)->sand_rock.ring_count == 0u)
	{
		sand_rock_crumble();
		return;
	}
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->sand_rock.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	drop_rings(slope_height, &cursor, &random_value, &ring);
	sand_rock_draw();
}

/* Every second frame shows the next of the 0xE sand_rock_crumble_frames on the rock and the one after it
   on its part, still drawn; past the last the rock and its part are deleted. */
void sand_rock_crumble(void)
{
	uint32_t crumble_frame = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	OBJECT(current_object)->sand_rock.frame_count++;
	if (OBJECT(current_object)->sand_rock.frame_count <= 1)
	{
		sand_rock_draw();
		return;
	}
	if (OBJECT(current_object)->sand_rock.crumble_frame <= 0xD)
	{
		crumble_frame = (uint8_t)OBJECT(current_object)->sand_rock.crumble_frame;
		OBJECT(current_object)->sprite_frame = sand_rock_crumble_frames[crumble_frame];
		OBJECT(current_object)->sand_rock.frame_count = 0;
		OBJECT(current_object)->sand_rock.crumble_frame++;
		crumble_frame++;
		if ((int8_t)crumble_frame > 0xD)
		{
			sand_rock_draw();
			return;
		}
		OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->child))->sprite_frame = sand_rock_crumble_frames[crumble_frame];
		sand_rock_draw();
		return;
	}
	OBJECT(current_object)->sand_rock.frame_count = 0;
	OBJECT(current_object)->sand_rock.crumble_frame = 0;
	OBJECT(current_object)->sand_rock.crumbling = 0u;
	OBJECT(current_object)->sprite_frame = sand_rock_crumble_frames[(uint8_t)OBJECT(current_object)->sand_rock.crumble_frame];
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* Drawn at depth screen_y + 0x14; touchable while crumble_frame is 0. */
void sand_rock_draw(void)
{
	uint32_t depth = 0, ignored = 0;

	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x14u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((uint8_t)OBJECT(current_object)->sand_rock.crumble_frame == 0u)
		add_to_touchable_list(ignored);
}

/* Unused: puts the rock's part at the rock's screen position and draws it at depth screen_y + 0x1F. */
void sand_rock_draw_piece(void)
{
	uint32_t depth = 0, ignored = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x1Fu);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Object type 41: a row of sand balls, direction row_direction x 0x40; ball_count 2-part balls at its
   position (sprite 0xA1C0, palette 0x9200, box 0x10, height 0x10, kind 0x24, with shadows), their
   row_position 6, 0xC, 0x12 ... and bob_phase spread by spawn_arg_2 / (ball_count + 1); then
   sand_arch_update. */
void sand_arch_init(void)
{
	uint16_t ball_phase = 0, phase_spacing = 0;
	uint32_t balls_left = 0, block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->direction = OBJECT(current_object)->sand_arch.row_direction * 0x40u;
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
	OBJECT(current_object)->handler = sand_arch_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	object = current_object;
	balls_left = (uint8_t)OBJECT(object_cursor)->sand_arch.ball_count;
	current_player = (current_player & ~0xFFFFu) | 6u;
	ball_phase = 0u;
	phase_spacing = divide_s16(OBJECT(object_cursor)->spawn_arg_2, (uint16_t)(balls_left + 1u));
	do
	{
		spawn_count = 2u;
		spawn_object_with_pieces(0x01369Cu, &ignored, &ignored_2, &free_count, &object_2, &block);
		OBJECT(object)->child = object_2;
		copy_position_to(object_2);
		OBJECT(object_2)->sprite_flags = 0u;
		OBJECT(object_2)->sprite_frame = 0xA1C0u;
		OBJECT(object_2)->sprite_palette = 0x9200u;
		OBJECT(object_2)->direction = OBJECT(object_cursor)->direction;
		OBJECT(object_2)->hitbox_size_x = 0x10u;
		OBJECT(object_2)->hitbox_height = 0x10u;
		OBJECT(object_2)->kind = 0x24u;
		OBJECT(object_2)->handler = sand_arch_ball_init_ADDRESS;
		ball_phase = (ball_phase - phase_spacing) & 0x3FFu;
		OBJECT(object_2)->sand_arch.bob_phase = ball_phase;
		OBJECT(object_2)->sand_arch.row_position = current_player;
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player + 6u);
		object_2 = LINKED(object_2, OBJECT(object_2)->child);
		OBJECT(object_2)->sprite_flags = 0u;
		OBJECT(object_2)->sprite_frame = 0xA1C0u;
		OBJECT(object_2)->sprite_palette = 0x87A0u;
		object = object_2;
		balls_left--;
	} while (balls_left != 0u);
}

/* The row's leader: its own sprite is never drawn, only its shadow part at its floor under everything;
   removed with its balls once balls_gone reaches ball_count. The is_on_screen toggle it keeps in
   row_position is never read. */
void sand_arch_update(void)
{
	uint32_t bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, on_screen = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	on_screen = is_on_screen();
	OBJECT(current_object)->sand_arch.row_position += on_screen;
	OBJECT(current_object)->sand_arch.row_position &= 1u;
	if (OBJECT(current_object)->sand_arch.balls_gone < OBJECT(current_object)->sand_arch.ball_count)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		bucket = place_sprite_at_cursor_floor();
		queue_sprite_in_bucket(bucket);
		current_object = object_cursor;
		return;
	}
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* Moves the ball once by 0x200 x row_position along its direction (each ball 0xC pixels further than the
   one before), then sand_arch_ball_delay. */
void sand_arch_ball_init(void)
{
	uint32_t ball_direction = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, speed = 0, speed_x = 0, speed_y = 0;

	ball_direction = OBJECT(current_object)->direction;
	speed = 0x200u * OBJECT(current_object)->sand_arch.row_position;
	direction_to_speed(speed, &ball_direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->handler = sand_arch_ball_delay_ADDRESS;
	sand_arch_ball_delay();
}

/* In the rider list, not drawn: adds 2 to bob_phase until it passes 0x3FF (so the balls start one after
   another), then sand_arch_ball_move with the phase wrapped. */
void sand_arch_ball_delay(void)
{
	add_to_rider_list();
	OBJECT(current_object)->sand_arch.bob_phase += 2u;
	if (OBJECT(current_object)->sand_arch.bob_phase <= 0x3FF)
	{
		sand_arch_ball_gone();
		return;
	}
	OBJECT(current_object)->sand_arch.bob_phase &= 0x3FFu;
	OBJECT(current_object)->handler = sand_arch_ball_move_ADDRESS;
}

/* Moves, touchable, a rider. Over a floor it bobs (sand_arch_ball_bob): hidden from 0x10 below the
   surface, its sprite drawn only more than 8 above it, its part showing sand spray at the surface while
   within 0x10 of it, else its shadow. Over a pit it falls, and at z_int 0x800 counts in the leader's
   balls_gone and stops. */
void sand_arch_ball_move(void)
{
	uint16_t below_floor = 0, offset_x = 0, offset_y = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	add_to_touchable_list(ignored);
	add_to_rider_list();
	if (OBJECT(current_object)->floor_z_int < 0x800)
	{
		sand_arch_ball_bob();
		below_floor = (uint16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int);
		if ((int16_t)below_floor >= 0x10)
		{
			sand_arch_ball_gone();
			return;
		}
		if ((int16_t)below_floor < -8)
			ignored_5 = queue_sprite_at_floor();
	}
	else
	{
		if (OBJECT(current_object)->z_int >= 0x800)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->parent);
			OBJECT(current_object)->sand_arch.balls_gone++;
			current_object = object_cursor;
			OBJECT(current_object)->handler = sand_arch_ball_gone_ADDRESS;
		}
		fall_to_floor(&ignored, &ignored_5, &ignored_2);
		ignored_5 = queue_sprite_at_floor();
	}
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) <= 0x10)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = sand_arch_spray_frames[(uint8_t)OBJECT(current_object)->piece_frame];
			OBJECT(current_object)->sprite_palette = 0x9020u;
			OBJECT(current_object)->piece_frame++;
			OBJECT(current_object)->piece_frame = (uint8_t)OBJECT(current_object)->piece_frame % 4u;
		}
		offset_x = 0u;
		offset_y = 8u;
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->z_int - OBJECT(object_cursor)->floor_z_int;
		queue_sprite(ignored_5);
		sand_arch_ball_gone();
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xA1C0u;
	OBJECT(current_object)->sprite_palette = 0x87A0u;
	offset_x = 0u;
	offset_y = (uint16_t)(OBJECT(object_cursor)->floor_z_int - OBJECT(object_cursor)->z_int);
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
	queue_sprite_at_depth(&ignored, &depth);
	OBJECT(current_object)->piece_frame = 0;
	sand_arch_ball_gone();
}

/* Does nothing: the handler of a ball that has fallen out of the stage, until its leader removes the
   row; the other ball code also ends in it. */
void sand_arch_ball_gone(void)
{
}

/* bob_phase + 0xC (& 0x3FF) and speed_z = -sine_table[bob_phase] x 0xC: the ball bobs up and down. */
void sand_arch_ball_bob(void)
{
	OBJECT(current_object)->sand_arch.bob_phase += 0xCu;
	OBJECT(current_object)->sand_arch.bob_phase &= 0x3FFu;
	OBJECT(current_object)->speed_z = 0u - sine_table[(uint16_t)OBJECT(current_object)->sand_arch.bob_phase] * 0xCu;
}

/* Object type 44: a long green wall of five stepped panels (sprite 0xB670, palette 0x8D60 | 0x2000,
   sprite_flags 6), box 0x1D x 0xDC, height 0x80, kind 0xC; then stepped_wall_draw. */
void stepped_wall_init(void)
{
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0xB670u;
	OBJECT(current_object)->sprite_palette = 0x8D60u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x1Du;
	OBJECT(current_object)->hitbox_size_y = 0xDCu;
	OBJECT(current_object)->hitbox_offset_x = 0x73;
	OBJECT(current_object)->hitbox_offset_y = -0x7E;
	OBJECT(current_object)->hitbox_height = 0x80u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = stepped_wall_draw_ADDRESS;
}

/* Unprojects; unless removed off screen, drawn under everything (bucket 0xFF) and touchable. */
void stepped_wall_draw(void)
{
	uint32_t bucket = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		add_to_touchable_list(bucket);
		return;
	}
}

/* Stage list object: waits while sand_block_leader_active is set (kind 1 only before scroll 0x9C00),
   then takes x, y, hazard and whirlwind lists and script from tornado_records[sprite_frame], becomes
   chaser_object and tornado_object, holds group leader 0, adds its back zone (kind 0, also freezing the
   players) or rear zone and five columns. */
void tornado_start(void)
{
	uint16_t angle_offset = 0, radius = 0;
	uint32_t column_slot = 0, columns_left = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0, tornado_record = 0, zone_index = 0;

	if (sand_block_leader_active != 0u && (OBJECT(current_object)->sprite_frame != 1u || scroll_position < 0x9C00u))
		return;
	tornado_record = tornado_records_ADDRESS + tornado_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = TORNADO_START_RECORD(tornado_record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->y_int = TORNADO_START_RECORD(tornado_record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	tornado_record += 2u;
	OBJECT(current_object)->animation = TORNADO_START_RECORD(tornado_record - 0x4u)->hazard_list_offset + tornado_hazard_lists_ADDRESS;
	tornado_kind = OBJECT(current_object)->sprite_frame;
	tornado_record += 2u;
	object = TORNADO_START_RECORD(tornado_record - 0x6u)->whirlwind_list_offset + tornado_whirlwind_lists_ADDRESS;
	OBJECT(current_object)->part_list = object;
	OBJECT(current_object)->tornado.path = tornado_record + 2u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->tornado.holding_back = 0u;
	OBJECT(current_object)->kind = 0x1Eu;
	OBJECT(current_object)->tornado.wait_timer = 0;
	OBJECT(current_object)->tornado.rotation_angle = 0u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->tornado.off_screen = 0xFFu;
	OBJECT(current_object)->handler = tornado_move_ADDRESS;
	chaser_object = current_object;
	tornado_object = current_object;
	group_leader_0_held = 0xFFu;
	if (tornado_kind == 0u)
		players_frozen = 0xFFu;
	if (tornado_kind == 0u)
	{
		zone_index = 1u;
		object = spawn_chaser_back_zone(zone_index, object);
	}
	else
		object = spawn_tornado_rear_zone(object);
	if (free_object_stack < 5)
		return;
	columns_left = 5u;
	column_slot = current_object + 0x96u;
	angle_offset = 0u;
	radius = 0x4000u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x1100u;
		OBJECT(object)->handler = tornado_column_circle_ADDRESS;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_size_y = 0x200u;
		OBJECT(object)->speed_z_fraction = radius;
		radius += 0x2000u;
		OBJECT(object)->speed_z_int = angle_offset;
		angle_offset += 0xCCu;
		OBJECT(object)->animation = tornado_column_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 1u;
		OBJECT(object)->animation_timer = 0;
		mem_write16(column_slot, object);
		OBJECT(object)->parent = current_object;
		object_2 = object;
		OBJECT(object_2)->child = 0xFFFFu;
		column_slot += 2u;
		if ((uint8_t)tornado_kind != 0u)
		{
			allocate_object_2(&free_count, &ignored_2, &object);
			if (free_count != 0u)
			{
				OBJECT(object)->sprite_flags = 0x1004u;
				OBJECT(object)->sprite_palette = 0x1100u;
				OBJECT(object_2)->child = object;
			}
		}
		columns_left--;
	} while (columns_left != 0u);
}

/* The tornado, a chaser: from its second frame it holds back whenever no player is in play, one has a
   mishap or drops in. After its wait (not counted in game mode 0x11) it loads the next script record
   once scroll_position or its x/y along its direction reaches the record's; then tornado_move_step.
   Removed with its columns on map_screen_active. */
void tornado_move(void)
{
	uint16_t threshold = 0;
	uint32_t direction = 0, path_cursor = 0;
	uint16_t record_scroll = 0;
	uint32_t flags = 0;

	if (map_screen_active != 0u)
	{
		tornado_remove_with_columns();
		return;
	}
	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->tornado.holding_back != 0u)
		{
			tornado_move_step();
			return;
		}
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->tornado.holding_back = 0xFFu;
			OBJECT(current_object)->tornado.saved_base_speed = OBJECT(current_object)->tornado.base_speed;
			OBJECT(current_object)->tornado.base_speed = 0;
			OBJECT(current_object)->tornado.hold_start_position = scroll_position;
			OBJECT(current_object)->tornado.hold_timer = 0xFFu;
			OBJECT(current_object)->tornado.saved_threshold = OBJECT(current_object)->tornado.screen_threshold;
			threshold = 0xFFB0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				threshold = 0x190u;
			OBJECT(current_object)->tornado.screen_threshold = threshold;
			tornado_move_step();
			return;
		}
	}
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->tornado.wait_timer--;
	if (OBJECT(current_object)->tornado.wait_timer > 0)
	{
		tornado_move_step();
		return;
	}
	OBJECT(current_object)->tornado.wait_timer = 0;
	path_cursor = OBJECT(current_object)->tornado.path;
	record_scroll = TORNADO_PATH_STEP(path_cursor)->position;
	path_cursor += 2u;
	if (record_scroll <= scroll_position)
	{
		path_cursor += 2u;
		tornado_load_record(path_cursor);
		return;
	}
	threshold = (uint16_t)TORNADO_PATH_STEP(path_cursor - 0x2u)->coordinate;
	direction = OBJECT(current_object)->direction;
	path_cursor += 2u;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		if ((int16_t)threshold < OBJECT(current_object)->y_int)
		{
			tornado_move_step();
			return;
		}
		tornado_load_record(path_cursor);
		return;
	case 1:
		if (OBJECT(current_object)->x_int < (int16_t)threshold)
		{
			tornado_move_step();
			return;
		}
		tornado_load_record(path_cursor);
		return;
	case 2:
		if (OBJECT(current_object)->y_int < (int16_t)threshold)
		{
			tornado_move_step();
			return;
		}
		tornado_load_record(path_cursor);
		return;
	case 3:
		if ((int16_t)threshold < OBJECT(current_object)->x_int)
		{
			tornado_move_step();
			return;
		}
		tornado_load_record(path_cursor);
		return;
	default:
		invalid_jump(0x89306u + (direction >> 8) * 2u);
	}
}

/* Loads the script record at path_cursor: screen_threshold, catch_up_factor, base_speed, wait_timer,
   direction (box 0xF0 across, 0x80 along); direction 3 with no wait unfreezes the players, the record
   ending at tornado_kind_0_end_record frees group leader 0. A negative second word ends it: removed with
   its columns, goal_unlocked if the third is too. */
void tornado_load_record(uint32_t path_cursor)
{
	uint16_t box_across = 0, box_along = 0;
	uint32_t direction = 0;

	if (TORNADO_PATH_STEP(path_cursor - 0x4u)->catch_up_factor < 0)
	{
		if (TORNADO_PATH_STEP(path_cursor - 0x4u)->base_speed >= 0)
		{
			tornado_remove_with_columns();
			return;
		}
		goal_unlocked = 0xFFu;
		tornado_remove_with_columns();
		return;
	}
	OBJECT(current_object)->tornado.screen_threshold = TORNADO_PATH_STEP(path_cursor - 0x4u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->tornado.catch_up_factor = TORNADO_PATH_STEP(path_cursor - 0x6u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->tornado.base_speed = TORNADO_PATH_STEP(path_cursor - 0x8u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->tornado.wait_timer = TORNADO_PATH_STEP(path_cursor - 0xAu)->wait;
	path_cursor += 2u;
	OBJECT(current_object)->direction = TORNADO_PATH_STEP(path_cursor - 0xCu)->direction;
	OBJECT(current_object)->tornado.path = path_cursor + 2u;
	box_across = 0xF0u;
	box_along = 0x80u;
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->hitbox_size_x = box_across;
		OBJECT(current_object)->hitbox_size_y = box_along;
		OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
		OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
		if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
		{
			tornado_move_step();
			return;
		}
		group_leader_0_held = 0u;
		tornado_move_step();
		return;
	case 1:
		OBJECT(current_object)->hitbox_size_x = box_along;
		OBJECT(current_object)->hitbox_size_y = box_across;
		OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
		OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
		if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
		{
			tornado_move_step();
			return;
		}
		group_leader_0_held = 0u;
		tornado_move_step();
		return;
	case 2:
		OBJECT(current_object)->hitbox_size_x = box_across;
		OBJECT(current_object)->hitbox_size_y = box_along;
		OBJECT(current_object)->hitbox_offset_x = 0u - (box_across >> 1);
		OBJECT(current_object)->hitbox_offset_y = 0u - (box_along >> 1);
		if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
		{
			tornado_move_step();
			return;
		}
		group_leader_0_held = 0u;
		tornado_move_step();
		return;
	case 3:
		break;
	default:
		invalid_jump(0x8939Du + (direction >> 8) * 2u);
	}
	OBJECT(current_object)->hitbox_size_x = box_along;
	OBJECT(current_object)->hitbox_size_y = box_across;
	OBJECT(current_object)->hitbox_offset_x = 0u - (box_along >> 1);
	OBJECT(current_object)->hitbox_offset_y = 0u - (box_across >> 1);
	if ((uint16_t)OBJECT(current_object)->tornado.wait_timer != 0u)
	{
		if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
		{
			tornado_move_step();
			return;
		}
		group_leader_0_held = 0u;
		tornado_move_step();
		return;
	}
	players_frozen = 0u;
	if (OBJECT(current_object)->tornado.path != tornado_kind_0_end_record_ADDRESS)
	{
		tornado_move_step();
		return;
	}
	group_leader_0_held = 0u;
	tornado_move_step();
}

/* Sets flag 0x400 on its five columns (the slots from 0x96, which then delete themselves) and removes
   the tornado. */
void tornado_remove_with_columns(void)
{
	uint32_t columns_left = 0, object = 0, slot_cursor = 0;

	object = current_object;
	slot_cursor = current_object + 0x96u;
	columns_left = 5u;
	do
	{
		object = (object & ~0xFFFFu) | mem_read16(slot_cursor);
		OBJECT(object)->flags |= 0x400u;
		columns_left--;
		slot_cursor += 2u;
	} while (columns_left != 0u);
	delete_current_object();
}

/* The tornado's step: 8 below its floor, rotation_angle - 4, zone list, tornado_throw_whirlwinds, a
   hazard from the edge of its floor block at each point of its hazard list it passes. A hold ends after
   0x300 of scroll, or 0x100 frames past the last mishap, drop-in or empty play; then
   tornado_choose_speed or tornado_catch_up_speed. */
void tornado_move_step(void)
{
	uint16_t hazard_coordinate = 0;
	uint32_t block_offset = 0, direction = 0, hazard_record = 0, ignored = 0, ignored_2 = 0, object = 0, old_head = 0, slope_height = 0, x = 0, y = 0, z = 0;
	uint8_t hold_timer = 0;
	uint16_t hazard_scroll = 0;

	find_floor(&ignored, &ignored_2, &block_offset, &slope_height, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int += 8u;
	OBJECT(current_object)->tornado.rotation_angle -= 4u;
	OBJECT(current_object)->tornado.rotation_angle &= 0x3FFu;
	project_object(&old_head, &ignored_2);
	old_head = add_to_zone_list(old_head);
	object = tornado_throw_whirlwinds((uint8_t)old_head, block_offset, slope_height, object);
	hazard_record = OBJECT(current_object)->animation;
	hazard_scroll = TORNADO_HAZARD(hazard_record)->position;
	hazard_record += 2u;
	if (hazard_scroll <= scroll_position)
	{
		hazard_coordinate = (uint16_t)TORNADO_HAZARD(hazard_record - 0x2u)->coordinate;
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 4:
			if (OBJECT(current_object)->y_int <= (int16_t)hazard_coordinate)
			{
				y = OBJECT(current_object)->y;
				x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block) - 0x80000u;
				z = OBJECT(current_object)->z;
				spawn_tornado_hazard(x, y, z, object);
				OBJECT(current_object)->animation += 4u;
			}
			break;
		case 1:
			if ((int16_t)hazard_coordinate <= OBJECT(current_object)->x_int)
			{
				x = OBJECT(current_object)->x - 0x800000u;
				y = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 8u) - 0x80000u;
				z = OBJECT(current_object)->z;
				spawn_tornado_hazard(x, y, z, object);
				OBJECT(current_object)->animation += 4u;
				if (OBJECT(current_object)->tornado.holding_back == 0u)
				{
					tornado_choose_speed();
					return;
				}
				if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
				{
					OBJECT(current_object)->tornado.holding_back = 0u;
					OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
					OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
					tornado_catch_up_speed();
					return;
				}
				if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
					OBJECT(current_object)->tornado.hold_timer = 0xFFu;
				hold_timer = OBJECT(current_object)->tornado.hold_timer;
				OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
				if (hold_timer == 0u)
				{
					OBJECT(current_object)->tornado.holding_back = 0u;
					OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
					OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
					tornado_catch_up_speed();
					return;
				}
				tornado_choose_speed();
				return;
			}
			break;
		case 2:
			if ((int16_t)hazard_coordinate <= OBJECT(current_object)->y_int)
			{
				y = OBJECT(current_object)->y - 0x800000u;
				x = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block) - 0x80000u;
				z = OBJECT(current_object)->z;
				spawn_tornado_hazard(x, y, z, object);
				OBJECT(current_object)->animation += 4u;
				if (OBJECT(current_object)->tornado.holding_back == 0u)
				{
					tornado_choose_speed();
					return;
				}
				if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
				{
					OBJECT(current_object)->tornado.holding_back = 0u;
					OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
					OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
					tornado_catch_up_speed();
					return;
				}
				if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
					OBJECT(current_object)->tornado.hold_timer = 0xFFu;
				hold_timer = OBJECT(current_object)->tornado.hold_timer;
				OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
				if (hold_timer == 0u)
				{
					OBJECT(current_object)->tornado.holding_back = 0u;
					OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
					OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
					tornado_catch_up_speed();
					return;
				}
				tornado_choose_speed();
				return;
			}
			break;
		case 3:
			if (OBJECT(current_object)->x_int <= (int16_t)hazard_coordinate)
			{
				x = OBJECT(current_object)->x;
				y = mem_read32(floor_blocks + 2u + OBJECT(current_object)->floor_block + 8u) - 0x80000u;
				z = OBJECT(current_object)->z;
				spawn_tornado_hazard(x, y, z, object);
				OBJECT(current_object)->animation += 4u;
				if (OBJECT(current_object)->tornado.holding_back == 0u)
				{
					tornado_choose_speed();
					return;
				}
				if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
				{
					OBJECT(current_object)->tornado.holding_back = 0u;
					OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
					OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
					tornado_catch_up_speed();
					return;
				}
				if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
					OBJECT(current_object)->tornado.hold_timer = 0xFFu;
				hold_timer = OBJECT(current_object)->tornado.hold_timer;
				OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
				if (hold_timer == 0u)
				{
					OBJECT(current_object)->tornado.holding_back = 0u;
					OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
					OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
					tornado_catch_up_speed();
					return;
				}
				tornado_choose_speed();
				return;
			}
			break;
		default:
			invalid_jump(0x89477u + (direction >> 8) * 2u);
		}
	}
	if (OBJECT(current_object)->tornado.holding_back == 0u)
	{
		tornado_choose_speed();
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->tornado.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->tornado.holding_back = 0u;
		OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
		OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
		tornado_catch_up_speed();
		return;
	}
	if (player_mishap != 0u || player_dropping_in != 0u || no_player_in_play != 0u)
		OBJECT(current_object)->tornado.hold_timer = 0xFFu;
	hold_timer = OBJECT(current_object)->tornado.hold_timer;
	OBJECT(current_object)->tornado.hold_timer = hold_timer - 1u;
	if (hold_timer == 0u)
	{
		OBJECT(current_object)->tornado.holding_back = 0u;
		OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
		OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
		tornado_catch_up_speed();
		return;
	}
	tornado_choose_speed();
}

/* base_speed while screen_x is at or below screen_threshold (directions 0 and 3) or at or above it (1, 2
   and 4); otherwise, or while screen_y is negative, it ends any hold (threshold and base speed back) and
   catches up with tornado_catch_up_speed. */
void tornado_choose_speed(void)
{
	uint16_t move_speed = 0;
	uint32_t direction = 0;

	if (OBJECT(current_object)->screen_y < 0)
	{
		if (OBJECT(current_object)->tornado.holding_back == 0u)
		{
			tornado_catch_up_speed();
			return;
		}
		OBJECT(current_object)->tornado.holding_back = 0u;
		OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
		OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
		tornado_catch_up_speed();
		return;
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->tornado.screen_threshold)
		{
			move_speed = (uint16_t)OBJECT(current_object)->tornado.base_speed;
			tornado_move_at_speed(move_speed);
			return;
		}
		if (OBJECT(current_object)->tornado.holding_back == 0u)
		{
			tornado_catch_up_speed();
			return;
		}
		OBJECT(current_object)->tornado.holding_back = 0u;
		OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
		OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
		tornado_catch_up_speed();
		return;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->tornado.screen_threshold <= OBJECT(current_object)->screen_x)
		{
			move_speed = (uint16_t)OBJECT(current_object)->tornado.base_speed;
			tornado_move_at_speed(move_speed);
			return;
		}
		if (OBJECT(current_object)->tornado.holding_back == 0u)
		{
			tornado_catch_up_speed();
			return;
		}
		OBJECT(current_object)->tornado.holding_back = 0u;
		OBJECT(current_object)->tornado.screen_threshold = OBJECT(current_object)->tornado.saved_threshold;
		OBJECT(current_object)->tornado.base_speed = OBJECT(current_object)->tornado.saved_base_speed;
		tornado_catch_up_speed();
		return;
	default:
		invalid_jump(0x89545u + (direction >> 8) * 2u);
	}
}

/* The tornado's catching-up speed: camera_slope_speed (with floor_slope_factors' factor where the floor
   slopes or has a type) x catch_up_factor >> 8, at least base_speed; then tornado_move_at_speed. */
void tornado_catch_up_speed(void)
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
	move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->tornado.catch_up_factor) >> 8;
	if (OBJECT(current_object)->tornado.base_speed < (int16_t)move_speed)
	{
		tornado_move_at_speed(move_speed);
		return;
	}
	move_speed = (uint16_t)OBJECT(current_object)->tornado.base_speed;
	tornado_move_at_speed(move_speed);
}

/* chaser_speed = move_speed (x4 once every player has finished); after the wait it moves chaser_speed <<
   12 along its direction (0 y-, 1 x+, 2 and 4 y+, 3 x-). Past screen x 0x180 or -0x40 (by direction) it
   is off screen; on screen, outside game mode 7, sound 0x12 every 0x29 frames, at once when it comes
   back. */
void tornado_move_at_speed(uint16_t move_speed)
{
	uint8_t sound_id = 0;
	uint32_t direction = 0, ignored = 0, ignored_2 = 0;
	uint8_t sound_timer = 0;

	if ((players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u)) == 0xFFFFFFu)
		move_speed = (uint16_t)((uint32_t)move_speed << 2);
	chaser_speed = move_speed;
	if (OBJECT(current_object)->tornado.wait_timer <= 0)
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
			invalid_jump(0x895E6u + (direction >> 8) * 2u);
		}
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x > 0x180)
		{
			OBJECT(current_object)->tornado.off_screen = 0xFFu;
			return;
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->screen_x < -0x40)
		{
			OBJECT(current_object)->tornado.off_screen = 0xFFu;
			return;
		}
		break;
	default:
		invalid_jump(0x89634u + (direction >> 8) * 2u);
	}
	if (OBJECT(current_object)->tornado.off_screen != 0u)
	{
		OBJECT(current_object)->tornado.off_screen = 0u;
		OBJECT(current_object)->tornado.sound_timer = 0u;
	}
	if (current_game_mode == 7u)
		return;
	sound_timer = OBJECT(current_object)->tornado.sound_timer;
	OBJECT(current_object)->tornado.sound_timer = sound_timer - 1u;
	if (sound_timer != 0u)
		return;
	sound_id = 0x12u;
	play_sound(sound_id, &ignored, &ignored_2);
	OBJECT(current_object)->tornado.sound_timer = 0x28u;
}

/* One of the tornado's five twisters: removed once it has flag 0x400; circles the tornado at its
   rotation angle (the tornado's spawn_arg_2) plus its own offset (speed_z_int), the radius
   (speed_z_fraction) swinging between 0x1000 and 0x6800 at a random rate (hitbox_size_y); animated on
   the floor, its top part 0xE0 higher when it has one. */
void tornado_column_circle(void)
{
	uint16_t radius_limit = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, ignored_8 = 0, ignored_9 = 0, object = 0, object_2 = 0, orbit_dx = 0, orbit_dy = 0, radius_rate = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z_fraction += OBJECT(current_object)->hitbox_size_y;
	if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x1000)
	{
		next_random(&radius_rate, &ignored, &ignored_2);
		radius_rate = (radius_rate + 0x100u) & 0x3FFu;
		radius_limit = 0x1000u;
		OBJECT(current_object)->hitbox_size_y = radius_rate;
		OBJECT(current_object)->speed_z_fraction = radius_limit;
	}
	else if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x6800)
	{
		next_random(&radius_rate, &ignored, &ignored_2);
		radius_rate = (uint16_t)(0u - ((radius_rate + 0x100u) & 0x3FFu));
		radius_limit = 0x6800u;
		OBJECT(current_object)->hitbox_size_y = radius_rate;
		OBJECT(current_object)->speed_z_fraction = radius_limit;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object_2)->x;
	OBJECT(current_object)->y = OBJECT(object_2)->y;
	orbit_dx = (OBJECT(object_2)->spawn_arg_2 + OBJECT(current_object)->speed_z_int) & 0x3FFu;
	orbit_dy = OBJECT(current_object)->speed_z_fraction;
	polar_to_xy(&ignored_3, &orbit_dx, &orbit_dy);
	OBJECT(current_object)->x += orbit_dx;
	OBJECT(current_object)->y += orbit_dy;
	animate(&ignored_3, &ignored_4, &ignored_5);
	find_floor(&ignored_6, &ignored_3, &ignored_7, &ignored_8, &ignored_9);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored_6, &ignored_3);
	queue_sprite(ignored_3);
	if ((int16_t)OBJECT(current_object)->child < 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object)->screen_x = OBJECT(current_object)->screen_x;
	OBJECT(object)->screen_y = OBJECT(current_object)->screen_y;
	OBJECT(object)->screen_y -= 0xE0u;
	OBJECT(object)->sprite_frame = tornado_column_top_frames[OBJECT(current_object)->animation_frame];
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	current_object = object;
	queue_sprite_at_depth(&ignored_6, &depth);
}

/* An invisible box the tornado leaves at x, y, z: kind 0xC, height 0x50, 0xF0 by 0x80 turned with the
   tornado's direction, handler tornado_hazard_update; nothing when no object is free. */
void spawn_tornado_hazard(uint32_t x, uint32_t y, uint32_t z, uint32_t hazard)
{
	uint32_t direction = 0, free_count = 0, ignored = 0, ignored_2 = 0;
	uint32_t saved_current_object = 0;

	allocate_object(&free_count, &hazard);
	if (free_count == 0u)
		return;
	OBJECT(hazard)->x = x;
	OBJECT(hazard)->y = y;
	OBJECT(hazard)->z = z;
	OBJECT(hazard)->sprite_flags = 0x1004u;
	OBJECT(hazard)->handler = tornado_hazard_update_ADDRESS;
	OBJECT(hazard)->flags = 2u;
	OBJECT(hazard)->hitbox_offset_x = 0;
	OBJECT(hazard)->hitbox_offset_y = 0;
	OBJECT(hazard)->direction = OBJECT(current_object)->direction;
	OBJECT(hazard)->kind = 0xCu;
	OBJECT(hazard)->hitbox_height = 0x50u;
	direction = OBJECT(hazard)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(hazard)->hitbox_size_x = 0xF0u;
		OBJECT(hazard)->hitbox_size_y = 0x80u;
		break;
	case 1:
	case 3:
		OBJECT(hazard)->hitbox_size_x = 0x80u;
		OBJECT(hazard)->hitbox_size_y = 0xF0u;
		break;
	default:
		invalid_jump(0x897BBu + (direction >> 8) * 2u);
	}
	saved_current_object = current_object;
	current_object = hazard;
	project_object(&ignored, &ignored_2);
	current_object = saved_current_object;
}

/* Touchable (and never drawn) while within 0xF0 of the screen, else removed. */
void tornado_hazard_update(void)
{
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t ignored = 0, ignored_2 = 0, off_screen = 0;

	margin_x = 0xF0u;
	margin_y = 0xF0u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored, &ignored_2);
		return;
	}
	delete_current_object();
}

/* Once scroll_position reaches the next position in its whirlwind list (part_list), spawn_whirlwind, and
   the list steps on. */
uint32_t tornado_throw_whirlwinds(uint8_t spawned, uint32_t free_count, uint32_t whirlwind, uint32_t object)
{
	uint32_t saved_current_object = 0;

	if (mem_read16(OBJECT(current_object)->part_list) <= scroll_position)
	{
		saved_current_object = current_object;
		object = spawn_whirlwind(spawned, free_count, whirlwind, object);
		current_object = saved_current_object;
		OBJECT(current_object)->part_list += 2u;
	}
	return object;
}

/* The rear zone of tornado kind 1: a new object of kind 9 (flags 0x40000002, height 0x160) with handler
   tornado_rear_zone_follow; hands back zone (unchanged when none is free). */
uint32_t spawn_tornado_rear_zone(uint32_t zone)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &zone);
	if (free_count != 0u)
	{
		OBJECT(zone)->sprite_flags = 0x1004u;
		OBJECT(zone)->kind = 9u;
		OBJECT(zone)->flags = 0x40000002u;
		OBJECT(zone)->hitbox_height = 0x160u;
		OBJECT(zone)->hitbox_offset_x = 0;
		OBJECT(zone)->hitbox_offset_y = 0;
		OBJECT(zone)->handler = tornado_rear_zone_follow_ADDRESS;
	}
	return zone;
}

/* Removed once scroll_position reaches 0xE500; until then takes chaser_object's box, position and
   direction, 0x60 lower (z_int + 0x60), makes the box 0x200 long starting 0x40 behind the tornado's
   position (against its direction) and joins the zone list. */
void tornado_rear_zone_follow(void)
{
	uint32_t direction = 0, object = 0;

	if (scroll_position >= 0xE500u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, chaser_object);
	OBJECT(current_object)->hitbox_size_x = OBJECT(object)->hitbox_size_x;
	OBJECT(current_object)->hitbox_size_y = OBJECT(object)->hitbox_size_y;
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->z_int += 0x60u;
	OBJECT(current_object)->direction = OBJECT(object)->direction;
	direction = OBJECT(object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 4:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	case 2:
		OBJECT(current_object)->hitbox_size_y = 0x200u;
		OBJECT(current_object)->y_int -= 0x240u;
		OBJECT(current_object)->hitbox_offset_y = 0;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x200u;
		OBJECT(current_object)->x_int += 0x40u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		break;
	default:
		invalid_jump(0x898C7u + (direction >> 8) * 2u);
	}
	direction = add_to_zone_list(direction);
}

/* Stage list object of stage 0xA: quicksand of kind 0x1C (it sinks the player), touch size 0x60, height
   0x50, 0x30 below its floor, sprite 0x74E0 cycling quicksand_palettes, quicksand_animation; with two
   pool 2 blocks free, a multi-part copy of quicksand_parts over it. A non-zero sprite_frame sets flag 1:
   never drawn. */
void quicksand_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, object_2 = 0;

	OBJECT(current_object)->flags = 0u;
	if (OBJECT(current_object)->sprite_frame != 0u)
		OBJECT(current_object)->flags |= 1u;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_frame = 0x74E0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x34;
	OBJECT(current_object)->hitbox_size_x = 0x60u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = quicksand_swirl_ADDRESS;
	OBJECT(current_object)->kind = 0x1Cu;
	OBJECT(current_object)->quicksand.palette_index = 0u;
	OBJECT(current_object)->quicksand.palette_delay = 4u;
	OBJECT(current_object)->quicksand.palette_timer = 0u;
	OBJECT(current_object)->quicksand.palette_list = quicksand_palettes_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = quicksand_animation_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int += 0x30u;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->child = 0xFFFFu;
	if (free_object_stack_2[0] >= 2)
	{
		allocate_object_2(&ignored, &ignored_3, &object);
		OBJECT(object)->sprite_flags = 0xD000u;
		OBJECT(current_object)->child = object;
		object_2 = object;
		allocate_object_2(&ignored, &ignored_3, &object);
		move_halfwords(object, quicksand_parts_ADDRESS, 0x11u);
		OBJECT(object_2)->part_list = object;
	}
}

/* Touchable while within 0x70 of the screen (then removed with its two part blocks); unless flag 1 is
   set it cycles its palette (animate_sign) and every animation_speed frames puts the next
   quicksand_animation frame in every part of its multi-part sprite, both drawn at depth screen_y - 0x58. */
void quicksand_swirl(void)
{
	uint16_t margin = 0, object = 0, sprite_frame = 0, step_frame = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, next_step = 0, object_2 = 0, off_screen = 0, part_cursor = 0, parts_left = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen == 0u)
	{
		add_to_touchable_list(0u);
		update_screen_position(&ignored_2, &ignored_3);
		if ((OBJECT(current_object)->flags & 1u) != 0u)
			return;
		animate_sign();
		if ((int16_t)OBJECT(current_object)->child < 0)
		{
			current_object = object_cursor;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored_2, &depth);
			return;
		}
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer > 1)
		{
			object_2 = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(object_2)->screen_x = OBJECT(current_object)->screen_x;
			OBJECT(object_2)->screen_y = OBJECT(current_object)->screen_y;
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored_2, &depth);
			current_object = object_cursor;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
			queue_sprite_at_depth(&ignored_2, &depth);
			return;
		}
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		step_list = OBJECT(current_object)->animation;
		next_step = OBJECT(current_object)->animation_frame;
		next_step = (uint8_t)(next_step + 1u);
		last_step = FRAME_ANIMATION(step_list)->last_frame;
		step_list += 2u;
		if ((int16_t)last_step < (int16_t)next_step)
			next_step = 0u;
		OBJECT(current_object)->animation_frame = next_step;
		step_frame = SPRITE_FRAME_NUMBERS(step_list)[next_step];
		part_cursor = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->part_list;
		parts_left = SPRITE_PART_LIST(part_cursor)->count;
		part_cursor += 2u;
		do
		{
			SPRITE_PART(part_cursor)->frame = step_frame;
			part_cursor += 8u;
			parts_left--;
		} while (parts_left != 0u);
		object_2 = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object_2)->screen_x = OBJECT(current_object)->screen_x;
		OBJECT(object_2)->screen_y = OBJECT(current_object)->screen_y;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored_2, &depth);
		current_object = object_cursor;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x58u);
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	}
	object_2 = LINKED(current_object, OBJECT(current_object)->child);
	sprite_frame = OBJECT(object_2)->sprite_frame;
	free_object_2(sprite_frame);
	object = (uint16_t)object_2;
	free_object_2(object);
	delete_current_object();
}

/* Stage list object of stage 0xA: a sinkhole that opens once scroll_position reaches open_position, its
   delay past now; zoom_step and delay from sinkhole_growth_records[sprite_frame & 0x7FFF], bit 15 flips
   it; scale 1, kind 0x1B, sinkhole_animation, on its floor; then sinkhole_open. */
void sinkhole_start(void)
{
	uint32_t growth_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 8u;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->sinkhole.zoom_step = SINKHOLE_GROWTH(growth_record)->zoom_step;
	OBJECT(current_object)->sinkhole.open_position = SINKHOLE_GROWTH(growth_record)->open_delay;
	OBJECT(current_object)->sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_palette = 0x1020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sinkhole.unscaled_offset_x = -8;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->sinkhole.unscaled_offset_x = 0u - OBJECT(current_object)->sinkhole.unscaled_offset_x;
	OBJECT(current_object)->sinkhole.unscaled_offset_y = -0x30;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->handler = sinkhole_open_ADDRESS;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0x200u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation = sinkhole_animation_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Removed off screen (margin 0x68). From open_position on: sound 0xF on its first step, then
   sprite_scale + zoom_step a frame up to 0x180 with its sprite offsets and hitbox_size_x scaled to
   match, animated; touchable and drawn (draw_depth = screen_y) from then on. */
void sinkhole_open(void)
{
	uint16_t scale = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, margin_or_scale = 0, off_screen = 0, sound_id = 0;

	margin_or_scale = 0x68u;
	off_screen = check_left_screen_margin((uint16_t)margin_or_scale);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x180)
	{
		add_to_touchable_list(sound_id);
		update_screen_position(&ignored_5, &margin_or_scale);
		OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y;
		margin_or_scale = queue_sprite(margin_or_scale);
		return;
	}
	if (OBJECT(current_object)->sprite_scale == 1u)
	{
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &ignored, &ignored_2);
	}
	OBJECT(current_object)->sprite_scale += OBJECT(current_object)->sinkhole.zoom_step;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->sprite_offset_x = (int32_t)((uint32_t)OBJECT(current_object)->sinkhole.unscaled_offset_x * scale) >> 8;
	OBJECT(current_object)->sprite_offset_y = (int32_t)((uint32_t)OBJECT(current_object)->sinkhole.unscaled_offset_y * scale) >> 8;
	animate(&ignored, &ignored_3, &ignored_4);
	margin_or_scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->hitbox_size_x = (int32_t)(OBJECT(current_object)->sinkhole.unscaled_hitbox_size_x * margin_or_scale) >> 8;
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored_5, &margin_or_scale);
	OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y;
	margin_or_scale = queue_sprite(margin_or_scale);
}

/* Stage list object of stage 0xA: a triangle of sand (sprite 0x7530, palette 0x9020), flipped when bit
   15 of sprite_frame is set, on its floor; then sand_patch_draw. */
void sand_patch_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	if ((OBJECT(current_object)->sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_frame = 0x7530u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->handler = sand_patch_draw_ADDRESS;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Drawn at depth screen_y - 0x38 until it leaves the screen (check_left_screen_margins: 0x38 once seen,
   0x10 before), then removed. */
void sand_patch_draw(void)
{
	uint16_t margin = 0, unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	margin = 0x38u;
	unseen_margin = 0x10u;
	off_screen = check_left_screen_margins(margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x38u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Stage list object of stage 0xA: stays as the hole's edge zone (kind 0x1D, size 0x70, height 0x40,
   rimmed_sinkhole_edge_zone_update) and spawns the hole: like sinkhole_start (sinkhole_growth_records,
   kind 0x1B, sprite 0x7518) with a pool 2 rim part (sprite 0x7538), handler rimmed_sinkhole_open. */
void rimmed_sinkhole_start(void)
{
	uint32_t free_count = 0, growth_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;
	uint32_t sprite_frame = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->hitbox_size_x = 0x70u;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->flags_low = 0u;
	OBJECT(current_object)->kind = 0x1Du;
	allocate_object(&free_count, &object);
	if (free_count == 0u)
		return;
	OBJECT(object_cursor)->handler = rimmed_sinkhole_edge_zone_update_ADDRESS;
	current_object = object;
	OBJECT(object_cursor)->parent = current_object;
	OBJECT(current_object)->rimmed_sinkhole.edge_zone = object_cursor;
	OBJECT(current_object)->x = OBJECT(object_cursor)->x;
	OBJECT(current_object)->y = OBJECT(object_cursor)->y;
	OBJECT(current_object)->sprite_flags = 0u;
	sprite_frame = OBJECT(object_cursor)->sprite_frame;
	growth_record = sprite_frame & ~0x8000u;
	if ((sprite_frame & 0x8000u) != 0u)
		OBJECT(current_object)->sprite_flags |= 0x40u;
	growth_record = sinkhole_growth_records_ADDRESS + growth_record * 4u;
	OBJECT(current_object)->rimmed_sinkhole.zoom_step = SINKHOLE_GROWTH(growth_record)->zoom_step;
	OBJECT(current_object)->rimmed_sinkhole.open_position = SINKHOLE_GROWTH(growth_record)->open_delay;
	OBJECT(current_object)->rimmed_sinkhole.open_position += scroll_position;
	OBJECT(current_object)->sprite_frame = 0x7518u;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_scale = 1u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->handler = rimmed_sinkhole_open_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 1u;
	OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_height = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0u;
	OBJECT(current_object)->kind = 0x1Bu;
	OBJECT(current_object)->direction = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->animation = rimmed_sinkhole_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
	allocate_object_2(&free_count, &ignored_3, &object);
	if (free_count == 0u)
		return;
	OBJECT(object)->sprite_flags = 0u;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		OBJECT(object)->sprite_flags |= 0x40u;
	OBJECT(object)->sprite_frame = 0x7538u;
	OBJECT(object)->sprite_palette = 0xF020u;
	OBJECT(object)->sprite_scale = 1u;
	OBJECT(current_object)->child = object;
}

/* Like sinkhole_open, growing to scale 0x100 with its rim; touchable, drawn at draw_depth = screen_y -
   0x30 (no less than -0xF) with the rim 8 behind it. Off screen (margins 0x58, 0x20) it frees the rim,
   sets flag 0x400 on its edge zone (which then goes) and is removed. */
void rimmed_sinkhole_open(void)
{
	uint16_t child = 0, margin = 0, unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, off_screen = 0, sound_id = 0;

	margin = 0x58u;
	unseen_margin = 0x20u;
	off_screen = check_left_screen_margins(margin, unseen_margin);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->rimmed_sinkhole.edge_zone);
		OBJECT(current_object)->flags |= 0x400u;
		delete_current_object();
		return;
	}
	if (scroll_position < OBJECT(current_object)->rimmed_sinkhole.open_position)
		return;
	if ((int16_t)OBJECT(current_object)->sprite_scale >= 0x100)
	{
		add_to_touchable_list(sound_id);
		update_screen_position(&ignored_5, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x30u);
		if ((int16_t)depth <= -0xF)
			depth = 0xFFF1u;
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored_5, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		depth = (uint16_t)(OBJECT(object_cursor)->draw_depth - 8u);
		queue_sprite_at_depth(&ignored_5, &depth);
		return;
	}
	if (OBJECT(current_object)->sprite_scale == 1u)
	{
		sound_id = 0xFu;
		play_sound((uint8_t)sound_id, &ignored_2, &ignored);
	}
	OBJECT(current_object)->sprite_scale += OBJECT(current_object)->rimmed_sinkhole.zoom_step;
	OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = OBJECT(current_object)->sprite_scale;
	animate(&ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->hitbox_size_x = (int32_t)((uint32_t)OBJECT(current_object)->rimmed_sinkhole.unscaled_hitbox_size_x * OBJECT(current_object)->sprite_scale) >> 8;
	add_to_touchable_list(sound_id);
	update_screen_position(&ignored_5, &ignored_2);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x30u);
	if ((int16_t)depth <= -0xF)
		depth = 0xFFF1u;
	OBJECT(current_object)->draw_depth = depth;
	queue_sprite_at_depth(&ignored_5, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(object_cursor)->draw_depth - 8u);
	queue_sprite_at_depth(&ignored_5, &depth);
}

/* The hole's edge zone: copies the hole's sprite_scale and draw_depth and stays touchable (kind 0x1D)
   until flag 0x400 is set, then removed. */
void rimmed_sinkhole_edge_zone_update(void)
{
	uint32_t ignored = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
		OBJECT(current_object)->draw_depth = OBJECT(object)->draw_depth;
		ignored = add_to_touchable_list(0u);
		return;
	}
	delete_current_object();
}

/* spawn_object_groups' leader of the stage 0xA sand blocks: a new group_leaders entry from the
   leader_spawn record at record (x, y, z, direction, group_leader_data script), handler
   sand_block_leader_move (sand_block_leader_wait while group_leader_0_held is set); sets
   sand_block_leader_active. spawned is 0xFFFFFFFF, or 0 with no object free. */
void spawn_sand_block_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_leader)
{
	uint32_t leader = *p_leader;

	allocate_object_outside_loop(p_spawned, &leader);
	if (*p_spawned != 0u)
	{
		group_leaders[LEADER_SPAWN(record)->leader & 3u] = leader;
		record++;
		OBJECT(leader)->x_int = LEADER_SPAWN(record - 0x1u)->x;
		OBJECT(leader)->x_fraction = 0u;
		record += 2u;
		OBJECT(leader)->y_int = LEADER_SPAWN(record - 0x3u)->y;
		OBJECT(leader)->y_fraction = 0u;
		record += 2u;
		OBJECT(leader)->z_int = LEADER_SPAWN(record - 0x5u)->z;
		OBJECT(leader)->z_fraction = 0u;
		OBJECT(leader)->floor_z = OBJECT(leader)->z;
		record += 2u;
		OBJECT(leader)->direction = LEADER_SPAWN(record - 0x7u)->direction;
		OBJECT(leader)->sand_block_leader.path = LEADER_SPAWN(record - 0x7u)->path + group_leader_data_ADDRESS;
		OBJECT(leader)->handler = sand_block_leader_move_ADDRESS;
		OBJECT(leader)->flags = 0u;
		OBJECT(leader)->sprite_offset_x = 0;
		OBJECT(leader)->sprite_offset_y = 0;
		OBJECT(leader)->sand_block_leader.holding_back = 0u;
		OBJECT(leader)->sand_block_leader.wait_timer = 1;
		sand_block_leader_active = 0xFFu;
		if (group_leader_0_held != 0u)
			OBJECT(leader)->handler = sand_block_leader_wait_ADDRESS;
		*p_spawned = 0xFFFFFFFFu;
	}
	*p_leader = leader;
}

/* Waits until scroll_position reaches 0x5100 or group_leader_0_held is cleared, then
   sand_block_leader_move from the next frame. */
void sand_block_leader_wait(void)
{
	if (scroll_position >= 0x5100u || group_leader_0_held == 0u)
		OBJECT(current_object)->handler = sand_block_leader_move_ADDRESS;
}

/* The sand blocks' leader, an invisible chaser on its group_leader_data script: on its first frame and
   while all is well sand_block_leader_read_script; when no player is in play, one has a mishap or drops
   in, it starts holding back (base speed 0, screen_threshold -0x20, or 0x160 in the odd directions) with
   sand_block_leader_move_step. */
void sand_block_leader_move(void)
{
	uint16_t hold_threshold = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		sand_block_leader_read_script();
		return;
	}
	if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
	{
		sand_block_leader_move_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		sand_block_leader_read_script();
		return;
	}
	OBJECT(current_object)->sand_block_leader.holding_back = 0xFFu;
	OBJECT(current_object)->sand_block_leader.saved_base_speed = OBJECT(current_object)->sand_block_leader.base_speed;
	OBJECT(current_object)->sand_block_leader.base_speed = 0;
	OBJECT(current_object)->sand_block_leader.hold_start_position = scroll_position;
	OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
	OBJECT(current_object)->sand_block_leader.saved_threshold = OBJECT(current_object)->sand_block_leader.screen_threshold;
	hold_threshold = 0xFFE0u;
	if ((OBJECT(current_object)->direction_low & 1u) != 0u)
		hold_threshold = 0x160u;
	OBJECT(current_object)->sand_block_leader.screen_threshold = hold_threshold;
	sand_block_leader_move_step();
}

/* While wait_timer runs it only moves; then, once scroll_position reaches the next record's position, it
   loads screen_threshold, catch_up_factor, base_speed and wait_timer from it (a negative factor word
   ends the script: removed). Then sand_block_leader_move_step. */
void sand_block_leader_read_script(void)
{
	uint32_t path_cursor = 0;
	uint16_t wait_timer = 0, record_scroll = 0;

	wait_timer = OBJECT(current_object)->sand_block_leader.wait_timer;
	OBJECT(current_object)->sand_block_leader.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		sand_block_leader_move_step();
		return;
	}
	OBJECT(current_object)->sand_block_leader.wait_timer = 0;
	path_cursor = OBJECT(current_object)->sand_block_leader.path;
	record_scroll = CHASE_STEP_AT_SCROLL(path_cursor)->position;
	path_cursor += 2u;
	if (scroll_position < record_scroll)
	{
		sand_block_leader_move_step();
		return;
	}
	if (CHASE_STEP_AT_SCROLL(path_cursor - 0x2u)->catch_up_factor < 0)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sand_block_leader.screen_threshold = CHASE_STEP_AT_SCROLL(path_cursor - 0x2u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->sand_block_leader.catch_up_factor = CHASE_STEP_AT_SCROLL(path_cursor - 0x4u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->sand_block_leader.base_speed = CHASE_STEP_AT_SCROLL(path_cursor - 0x6u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->sand_block_leader.wait_timer = CHASE_STEP_AT_SCROLL(path_cursor - 0x8u)->wait;
	OBJECT(current_object)->sand_block_leader.path = path_cursor + 2u;
	sand_block_leader_move_step();
}

/* The leader's step: chaser_speed is base_speed on its side of screen_threshold, else the camera's
   ground speed x catch_up_factor >> 8 (at least base_speed); a hold ends after 0x300 of scroll or 0x100
   frames with players in play. After the wait it moves along its direction, centred on its floor block;
   y_int 0xA2B or less clears sand_block_leader_active. */
void sand_block_leader_move_step(void)
{
	uint16_t move_speed = 0;
	uint32_t block_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint8_t hold_timer = 0;

	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	project_object(&ignored, &ignored_2);
	add_to_rider_list();
	if (OBJECT(current_object)->sand_block_leader.holding_back == 0u)
	{
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 2:
		case 4:
			if (OBJECT(current_object)->sand_block_leader.screen_threshold <= OBJECT(current_object)->screen_x)
				move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
			else
			{
				OBJECT(current_object)->sand_block_leader.wait_timer = 0;
				if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
				{
					OBJECT(current_object)->sand_block_leader.holding_back = 0u;
					OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
					OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
				}
				compute_camera_ground_speed(ignored_4);
				move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
				if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
					move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
			}
			break;
		case 1:
		case 3:
			if (OBJECT(current_object)->sand_block_leader.screen_threshold < OBJECT(current_object)->screen_x || OBJECT(current_object)->screen_y > 0xE0)
			{
				OBJECT(current_object)->sand_block_leader.wait_timer = 0;
				if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
				{
					OBJECT(current_object)->sand_block_leader.holding_back = 0u;
					OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
					OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
				}
				compute_camera_ground_speed(ignored_4);
				move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
				if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
					move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
			}
			else
				move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
			break;
		default:
			invalid_jump(0x8A046u + OBJECT(current_object)->direction * 2u);
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->sand_block_leader.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->sand_block_leader.holding_back = 0u;
		OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
		OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
		compute_camera_ground_speed(ignored_4);
		move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
		if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
			move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->sand_block_leader.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->sand_block_leader.hold_timer;
		OBJECT(current_object)->sand_block_leader.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->sand_block_leader.holding_back = 0u;
			OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
			OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
			compute_camera_ground_speed(ignored_4);
			move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
			if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
				move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
		}
		else
		{
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 2:
			case 4:
				if (OBJECT(current_object)->sand_block_leader.screen_threshold <= OBJECT(current_object)->screen_x)
					move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
				else
				{
					OBJECT(current_object)->sand_block_leader.wait_timer = 0;
					if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
					{
						OBJECT(current_object)->sand_block_leader.holding_back = 0u;
						OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
						OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
					}
					compute_camera_ground_speed(ignored_4);
					move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
					if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
						move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
				}
				break;
			case 1:
			case 3:
				if (OBJECT(current_object)->sand_block_leader.screen_threshold < OBJECT(current_object)->screen_x || OBJECT(current_object)->screen_y > 0xE0)
				{
					OBJECT(current_object)->sand_block_leader.wait_timer = 0;
					if (OBJECT(current_object)->sand_block_leader.holding_back != 0u)
					{
						OBJECT(current_object)->sand_block_leader.holding_back = 0u;
						OBJECT(current_object)->sand_block_leader.screen_threshold = OBJECT(current_object)->sand_block_leader.saved_threshold;
						OBJECT(current_object)->sand_block_leader.base_speed = OBJECT(current_object)->sand_block_leader.saved_base_speed;
					}
					compute_camera_ground_speed(ignored_4);
					move_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->sand_block_leader.catch_up_factor) >> 8;
					if ((int16_t)move_speed <= OBJECT(current_object)->sand_block_leader.base_speed)
						move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
				}
				else
					move_speed = (uint16_t)OBJECT(current_object)->sand_block_leader.base_speed;
				break;
			default:
				invalid_jump(0x8A046u + OBJECT(current_object)->direction * 2u);
			}
		}
	}
	chaser_speed = move_speed;
	if (OBJECT(current_object)->sand_block_leader.wait_timer > 0)
	{
		if (OBJECT(current_object)->y_int <= 0xA2B)
			sand_block_leader_active = 0u;
		return;
	}
	block_record = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	switch (OBJECT(current_object)->direction)
	{
	case 0:
	case 4:
		OBJECT(current_object)->y += (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->x_max - FLOOR_BLOCK(block_record)->x_min) >> 1) + FLOOR_BLOCK(block_record)->x_min;
		if (OBJECT(current_object)->y_int <= 0xA2B)
			sand_block_leader_active = 0u;
		return;
	case 1:
		OBJECT(current_object)->y -= (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->x_max - FLOOR_BLOCK(block_record)->x_min) >> 1) + FLOOR_BLOCK(block_record)->x_min;
		if (OBJECT(current_object)->y_int <= 0xA2B)
			sand_block_leader_active = 0u;
		return;
	case 2:
		OBJECT(current_object)->x += (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->y = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->y_max - FLOOR_BLOCK(block_record)->y_min) >> 1) + FLOOR_BLOCK(block_record)->y_min;
		if (OBJECT(current_object)->y_int <= 0xA2B)
			sand_block_leader_active = 0u;
		return;
	case 3:
		OBJECT(current_object)->x -= (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->y = (uint32_t)((int32_t)((uint32_t)FLOOR_BLOCK(block_record)->y_max - FLOOR_BLOCK(block_record)->y_min) >> 1) + FLOOR_BLOCK(block_record)->y_min;
		if (OBJECT(current_object)->y_int <= 0xA2B)
			sand_block_leader_active = 0u;
		return;
	default:
		invalid_jump(0x8A0D4u + OBJECT(current_object)->direction * 2u);
	}
}

/* A sand block, a platform eaten by its leader: once the leader's y is inside it (y + hitbox_size_y -
   0x10), objects allowing, out come its cut face (sand_face_update) with sand streams, two clip entries
   and sound 0x11; then hitbox_size_y shrinks to the leader's y - y, the clip line at the leader, and at
   0 the face falls (flag 0x400) and the block goes. */
void sand_block_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, part_scale = 0, span_y = 0;
	uint32_t blocks_left = 0, depth = 0, floor_z = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, object_2 = 0, object_3 = 0, part_cursor = 0, part_list = 0, point_x = 0, point_y = 0, stream_parts_left = 0;

	switch (OBJECT(current_object)->sand_block.state)
	{
	case 0:
	case 4:
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->sand_block.state++;
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			invalid_jump(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &ignored_2);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		switch (OBJECT(current_object)->direction)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			break;
		default:
			invalid_jump(0x8A1A5u + OBJECT(current_object)->direction * 2u);
		}
		if (OBJECT(current_player)->y_int <= (int16_t)(OBJECT(current_object)->y_int + OBJECT(current_object)->hitbox_size_y - 0x10u))
			OBJECT(current_object)->sand_block.state++;
		add_to_platform_list();
		update_screen_position(&ignored, &ignored_2);
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
			return;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (free_object_stack < 2 || free_object_stack_2[0] < 3)
		{
			add_to_platform_list();
			update_screen_position(&ignored, &ignored_2);
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) >= 0x281u)
				return;
			depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y);
			OBJECT(current_object)->draw_depth = depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->sand_block.state++;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_face_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->speed_z = 0;
		OBJECT(object_2)->sand_face.depth_offset = 0x70;
		OBJECT(object_2)->animation_frame = 0u;
		OBJECT(object_2)->animation_speed = 2u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(object_2)->animation = sand_face_animation_ADDRESS;
		OBJECT(object_2)->x = OBJECT(current_object)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->sprite_offset_x = -6;
		OBJECT(object_2)->sprite_offset_y = 0x10;
		OBJECT(object_2)->z = OBJECT(current_object)->z;
		OBJECT(object_2)->parent = current_object;
		OBJECT(current_object)->sand_block.face_object = object_2;
		object_3 = object_2;
		allocate_object_2(&ignored, &ignored_3, &object_2);
		move_halfwords_backward(object_2, sand_face_parts_ADDRESS, 9u);
		part_list = OBJECT(current_object)->part_list;
		OBJECT(object_2)->sprite_scale = SAND_BLOCK_PART_LIST(part_list)->first_palette;
		OBJECT(object_2)->hitbox_size_x = SAND_BLOCK_PART_LIST(part_list)->first_palette;
		OBJECT(object_3)->part_list = object_2;
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0xD002u;
		OBJECT(object_2)->handler = sand_streams_update_ADDRESS;
		OBJECT(object_2)->flags = 0u;
		move_halfwords_backward(object_2 + 0x40u, sand_stream_parts_ADDRESS, 0x19u);
		OBJECT(object_2)->part_list = object_2 + 0x40u;
		part_scale = (uint16_t)mem_read32(OBJECT(object_3)->part_list + 8u);
		part_cursor = OBJECT(object_2)->part_list;
		stream_parts_left = SPRITE_PART_LIST(part_cursor)->count;
		part_cursor += 2u;
		do
		{
			SPRITE_PART(part_cursor)->palette = part_scale;
			part_cursor += 8u;
			stream_parts_left--;
		} while (stream_parts_left != 0u);
		OBJECT(object_2)->parent = object_3;
		OBJECT(object_3)->child = object_2;
		blocks_left = 2u;
		object_2 = current_object;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored_3, &object_2);
			object = object_2 + 2u;
			OBJECT(object - 0x2u)->sprite_flags = 0x5000u;
			object += 2u;
			OBJECT(object - 0x4u)->sprite_frame = 0xE0u;
			object += 2u;
			OBJECT(object - 0x6u)->sprite_palette = 0u;
			object += 2u;
			OBJECT(object - 0x8u)->sprite_scale = 0x140u;
			object += 2u;
			OBJECT(object - 0xAu)->sprite_layer = 0u;
			object += 2u;
			OBJECT(object - 0xCu)->screen_y = 0;
			object += 2u;
			OBJECT(object - 0xEu)->screen_x = 0;
			OBJECT(object - 0xEu)->hitbox_size_x = 0u;
			OBJECT(object_2)->parent = object_3;
			OBJECT(object_3)->child = object_2;
			blocks_left--;
		} while (blocks_left != 0u);
		if (OBJECT(current_object)->screen_x <= 0x148 && OBJECT(current_object)->screen_y <= 0x130)
		{
			sound_id = 0x11u;
			play_sound(sound_id, &ignored_2, &ignored_3);
		}
	case 3:
		update_screen_position(&ignored, &ignored_2);
		object = LINKED(current_object, OBJECT(current_object)->parent);
		span_y = (uint16_t)(OBJECT(object)->y_int - OBJECT(current_object)->y_int);
		OBJECT(current_object)->hitbox_size_y = span_y;
		if ((int16_t)span_y > 0)
		{
			point_x = OBJECT(current_object)->x + ((uint32_t)OBJECT(current_object)->hitbox_size_x << 16);
			point_y = OBJECT(object)->y;
			floor_z = OBJECT(current_object)->floor_z;
			project_point(floor_z, &ignored_2, &point_x, &point_y);
			OBJECT(LINKED(current_object, OBJECT(current_object)->child))->sprite_scale = point_x;
			add_to_platform_list();
			if ((uint16_t)(OBJECT(current_object)->screen_x + 0x140u) < 0x281u)
			{
				OBJECT(current_object)->draw_depth = OBJECT(current_object)->screen_y + OBJECT(current_object)->sand_block.depth_offset - OBJECT(current_object)->sprite_offset_y;
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = object_cursor;
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
				current_object = LINKED(current_object, OBJECT(current_object)->child);
				depth = OBJECT(object_cursor)->draw_depth;
				queue_sprite_at_depth(&ignored, &depth);
			}
			return;
		}
		object = LINKED(current_object, OBJECT(current_object)->sand_block.face_object);
		OBJECT(object)->flags |= 0x400u;
		child = OBJECT(current_object)->child;
		free_object_2(child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		child = OBJECT(current_object)->child;
		free_object_2(child);
		delete_current_object();
		return;
	default:
		invalid_jump(0x8A17Cu + OBJECT(current_object)->sand_block.state * 3u);
	}
}

/* The moving cut face of a sand block (a 2-part sprite): follows the leader's y and z, animated, at
   depth screen_y - 0x70. With flag 0x400 it falls (speed_z + 0x6000 a frame, sand_face_fall_animation)
   until screen y 0x150, then frees its part list, sets flag 0x400 on its streams and is removed. */
void sand_face_update(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0;

	if ((OBJECT(current_object)->flags & 0x400u) == 0u)
	{
		object = LINKED(current_object, OBJECT(current_object)->parent);
		object = LINKED(object, OBJECT(object)->parent);
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->z = OBJECT(object)->z;
		animate_all_parts(&ignored_2, &ignored_3);
		project_object(&ignored_4, &ignored_5);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sand_face.depth_offset);
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	if (OBJECT(current_object)->screen_y >= 0x150)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(object)->flags |= 0x400u;
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sand_face.depth_offset = 0x38;
	OBJECT(current_object)->animation = sand_face_fall_animation_ADDRESS;
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	animate_all_parts(&ignored_2, &ignored_3);
	project_object(&ignored_4, &ignored_5);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sand_face.depth_offset);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Falling sand under a sand block's face: removed with flag 0x400; else at the face's screen position
   (less its sprite offsets), every part of its multi-part sprite showing sand_stream_frames[the face's
   animation_frame], at the face's depth. */
void sand_streams_update(void)
{
	uint16_t stream_frame = 0;
	uint32_t depth = 0, ignored = 0, object = 0, part_cursor = 0, parts_left = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_x -= OBJECT(object)->sprite_offset_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_y -= OBJECT(object)->sprite_offset_y;
	stream_frame = sand_stream_frames[OBJECT(object)->animation_frame];
	part_cursor = OBJECT(current_object)->part_list;
	parts_left = SPRITE_PART_LIST(part_cursor)->count;
	part_cursor += 2u;
	do
	{
		SPRITE_PART(part_cursor)->frame = stream_frame;
		part_cursor += 8u;
		parts_left--;
	} while (parts_left != 0u);
	depth = (uint16_t)(OBJECT(object)->screen_y - OBJECT(object)->sand_face.depth_offset);
	queue_sprite_at_depth(&ignored, &depth);
}

/* When a player starts sinking in quicksand, within 8 of its centre (check_player_contacts), with 4
   objects free: two swirls at the player, 8 lower, from sink_swirl_effects: a streak (palette 0x1210,
   its depth offset, handler sink_swirl_start) with a sand puff part (palette 0x1230) each. */
uint32_t spawn_sink_swirl(uint32_t allocated)
{
	uint32_t effect_record = 0, ignored = 0, object = 0, swirls_left = 0;

	if (free_object_stack < 4)
		return allocated;
	effect_record = sink_swirl_effects_ADDRESS;
	swirls_left = 2u;
	do
	{
		allocate_object(&ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_palette = 0x1210u;
		OBJECT(allocated)->flags_low = 0u;
		OBJECT(allocated)->sprite_offset_x = 0;
		OBJECT(allocated)->sprite_offset_y = 0;
		OBJECT(allocated)->sink_swirl.depth_offset = SINK_SWIRL_EFFECT(effect_record)->depth_offset;
		OBJECT(allocated)->handler = sink_swirl_start_ADDRESS;
		effect_record += 2u;
		OBJECT(allocated)->animation = SINK_SWIRL_EFFECT(effect_record - 0x2u)->streak_animation;
		OBJECT(allocated)->animation_frame = 0xFFu;
		OBJECT(allocated)->animation_speed = 0xAu;
		OBJECT(allocated)->animation_timer = 0;
		OBJECT(allocated)->x = OBJECT(current_object)->x;
		OBJECT(allocated)->y = OBJECT(current_object)->y;
		OBJECT(allocated)->z = OBJECT(current_object)->z;
		OBJECT(allocated)->z_int += 8u;
		object = allocated;
		effect_record += 4u;
		take_free_object(&ignored, &allocated);
		OBJECT(allocated)->sprite_flags = 0x1004u;
		OBJECT(allocated)->sprite_palette = 0x1230u;
		OBJECT(allocated)->flags_low = 0u;
		OBJECT(allocated)->animation = SINK_SWIRL_EFFECT(effect_record - 0x6u)->puff_animation;
		OBJECT(allocated)->animation_frame = 0xFFu;
		OBJECT(allocated)->animation_speed = 4u;
		OBJECT(allocated)->animation_timer = 0;
		OBJECT(object)->child = allocated;
		swirls_left--;
		effect_record += 4u;
	} while (swirls_left != 0u);
	return allocated;
}

/* project_object, then sink_swirl_animate from this frame on. */
void sink_swirl_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = sink_swirl_animate_ADDRESS;
	sink_swirl_animate();
}

/* Animates the swirl's streak and, at the same screen position, its sand puff part, both at depth
   screen_y + depth_offset; once the streak's animation reaches its last step it frees the puff and is
   removed. */
void sink_swirl_animate(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		push_free_object();
		delete_current_object();
		return;
	}
	update_screen_position(&ignored_3, &ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	animate(&ignored, &ignored_2, &ignored_4);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(object_cursor)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored_3, &depth);
	current_object = object_cursor;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->sink_swirl.depth_offset);
	queue_sprite_at_depth(&ignored_3, &depth);
}
