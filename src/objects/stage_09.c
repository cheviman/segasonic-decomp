/**
 *    Name:        objects/stage_09.c
 *    Description: stage 9's objects, the ice: the handle rod between two posts, the spear traps, the ice cube launcher, the padded wall, the snow mound, the ice blocks and the moving ice platform, the crumbling ice floor, the icicles
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* The rod's pivot (0x73B8) raised 0x12 per handle and handle_count handles (handle_rod_handle_init) 0x12
   apart. The first becomes the near post (handle_rod_post_init) a rod's length away along angle 0x300,
   the second the grip (kind 0x1A, spawn_arg_2 = the pivot's x); one more object is the far post. */
void handle_rod_init(void)
{
	uint16_t angle_offset = 0, angle_step = 0;
	uint32_t block = 0, count_or_spawned = 0, grip_handle = 0, handle_index = 0, ignored = 0, object = 0, object_2 = 0, rod_dx = 0, rod_dz = 0, rod_length = 0;

	OBJECT(current_object)->sprite_scale = 0x100u;
	OBJECT(current_object)->sprite_frame = 0x73B8u;
	OBJECT(current_object)->sprite_palette = 0x9170u;
	OBJECT(current_object)->handle_rod.angle = 0x322u;
	OBJECT(current_object)->handle_rod.swing_accel = 0x60;
	count_or_spawned = OBJECT(current_object)->handle_rod.handle_count;
	spawn_count = count_or_spawned;
	count_or_spawned = (uint16_t)(count_or_spawned * 0x12u);
	OBJECT(current_object)->z_int -= count_or_spawned;
	spawn_objects(0x011474u, &count_or_spawned, &object_2, &block);
	if ((uint8_t)count_or_spawned == 0u)
		return;
	OBJECT(current_object)->child = object_2;
	OBJECT(current_object)->parent = object_2;
	grip_handle = LINKED(WORK_RAM, OBJECT(object_2)->child);
	OBJECT(current_object)->handler = handle_rod_wait_ADDRESS;
	handle_index = (count_or_spawned & ~0xFFFFu) | OBJECT(current_object)->handle_rod.handle_count;
	angle_step = divide_s16(0x400u, (uint16_t)handle_index);
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r14 = (uint16_t)current_object;
		copy_position_from_cursor();
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
		OBJECT(current_object)->handler = handle_rod_handle_init_ADDRESS;
		OBJECT(current_object)->handle_rod_handle.start_angle = OBJECT(object_cursor)->handle_rod.angle;
		OBJECT(current_object)->handle_rod_handle.rod_angle_speed = OBJECT(object_cursor)->handle_rod.angle_speed;
		OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
		OBJECT(current_object)->handle_rod_handle.index_word = handle_index;
		angle_offset = (uint16_t)(angle_step * handle_index);
		OBJECT(current_object)->spawn_arg_1 = 0x400u;
		OBJECT(current_object)->spawn_arg_1 -= angle_offset;
		OBJECT(current_object)->spawn_arg_2 += angle_offset;
		OBJECT(current_object)->z_int += handle_index * 0x12u;
		handle_index--;
	} while (handle_index != 0u);
	OBJECT(grip_handle)->handle_rod_handle.grip = 1u;
	OBJECT(grip_handle)->handle_rod_handle.rod = object_cursor;
	OBJECT(grip_handle)->spawn_arg_2 = OBJECT(object_cursor)->x_int;
	OBJECT(grip_handle)->hitbox_size_x = 0x10u;
	OBJECT(grip_handle)->kind = 0x1Au;
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(object_cursor)->child = 0xFFFFu;
	OBJECT(object_cursor)->handle_rod.angle = 0x300u;
	object = LINKED(WORK_RAM, OBJECT(current_object)->child);
	OBJECT(current_object)->child = 0xFFFFu;
	OBJECT(current_object)->sprite_frame = 0x74C8u;
	OBJECT(current_object)->sprite_palette = 0x1060u;
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->projected_x -= 0x11u;
	OBJECT(current_object)->projected_y_int += 3u;
	OBJECT(current_object)->z = OBJECT(object_cursor)->z;
	OBJECT(current_object)->handler = handle_rod_post_init_ADDRESS;
	OBJECT(current_object)->sprite_scale = 0x100u;
	rod_length = OBJECT(current_object)->handle_rod_post.handle_index * 0x12u;
	rod_dx = sine_of_300 * 2u * 2u * rod_length;
	rod_dz = cosine_of_300 * 2u * 2u * rod_length;
	OBJECT(current_object)->projected_x += (int32_t)rod_dx >> 16;
	OBJECT(current_object)->projected_y += 0u - ((int32_t)rod_dx >> 1);
	OBJECT(current_object)->z += rod_dz;
	spawn_count = 1u;
	spawn_objects(0x0115D3u, &ignored, &object_2, &block);
	copy_position_to(object_2);
	OBJECT(object_2)->projected_x -= 0x12u;
	OBJECT(object_2)->projected_y_int -= 6u;
	OBJECT(object_2)->handle_rod_post.fallen = 1u;
	OBJECT(object_2)->handler = handle_rod_post_init_ADDRESS;
	OBJECT(object_cursor)->z_int -= 0x18u;
}

/* Gives the handle a shadow piece (sprite 0x5D80, sprite_scale 0x80), then handle_rod_handle_update. */
void handle_rod_handle_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0, spawned = 0;

	spawn_count = 1u;
	add_pieces_to_current_object(&spawned, &ignored, &ignored_2, &object);
	if ((uint8_t)spawned != 0u)
	{
		OBJECT(current_object)->handler = handle_rod_handle_update_ADDRESS;
		OBJECT(object)->sprite_frame = 0x5D80u;
		OBJECT(object)->sprite_palette = 0x87A0u;
		OBJECT(object)->sprite_scale = 0x80u;
	}
}

/* Drawn and rideable until the near post (its parent) falls; then with drops_players
   handle_rod_drop_players = 1, else players_pulled = 1 and event_12_fast_scroll = 1, and
   handle_rod_start_swing. hitbox_size_x keeps the pivot's screen_y, its handles' depth. */
void handle_rod_wait(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	if (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->handle_rod_post.fallen == 0u)
	{
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->hitbox_size_x = depth;
		queue_sprite_at_depth(&ignored, &depth);
		add_to_rider_list();
		return;
	}
	OBJECT(current_object)->handler = handle_rod_start_swing_ADDRESS;
	if (OBJECT(current_object)->handle_rod.drops_players == 0u)
	{
		players_pulled = 1u;
		event_12_fast_scroll = 1u;
	}
	else
		handle_rod_drop_players = 1u;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

/* Clears handle_rod_drop_players and players_pulled (they were set for one frame), then
   handle_rod_swing. */
void handle_rod_start_swing(void)
{
	handle_rod_drop_players = 0u;
	players_pulled = 0u;
	OBJECT(current_object)->handler = handle_rod_swing_ADDRESS;
	handle_rod_swing();
}

/* The pendulum: angle_speed + swing_accel and angle + angle_speed_int a frame; at the bottom swing_accel
   flips and angle_speed loses 0x100 (with drops_players the rod bounces back from angle 0 at half
   speed). Hangs at angle 0 once slow (handle_rod_hang); removed past end_position. */
void handle_rod_swing(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	if (OBJECT(current_object)->handle_rod.end_position < scroll_position)
	{
		OBJECT(current_object)->handle_rod.end_position = 0xFFFFu;
		OBJECT(current_object)->handler = handle_rod_remove_ADDRESS;
		return;
	}
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	if (OBJECT(current_object)->handle_rod.swing_side != 0u)
	{
		if (OBJECT(current_object)->handle_rod.drops_players != 0u)
		{
			OBJECT(current_object)->handle_rod.angle = 0u;
			OBJECT(current_object)->handle_rod.angle_speed = (int16_t)OBJECT(current_object)->handle_rod.angle_speed >> 1;
			OBJECT(current_object)->handle_rod.angle_speed = 0u - OBJECT(current_object)->handle_rod.angle_speed;
			OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.swing_side = 0u;
			OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
			OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			OBJECT(current_object)->hitbox_size_x = depth;
			queue_sprite_at_depth(&ignored, &depth);
			add_to_rider_list();
			return;
		}
		if ((int16_t)OBJECT(current_object)->handle_rod.angle < 0x200)
		{
			OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
			OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			OBJECT(current_object)->hitbox_size_x = depth;
			queue_sprite_at_depth(&ignored, &depth);
			add_to_rider_list();
			return;
		}
		if (OBJECT(current_object)->handle_rod.angle_speed < -0x100)
			OBJECT(current_object)->handle_rod.angle_speed += 0x100u;
		OBJECT(current_object)->handle_rod.angle_speed++;
		OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
		OBJECT(current_object)->handle_rod.swing_side = 0u;
	}
	else
	{
		if (OBJECT(current_object)->handle_rod.angle == 0u && OBJECT(current_object)->handle_rod.angle_speed <= 0x100)
		{
			OBJECT(current_object)->handle_rod.angle = 0u;
			OBJECT(current_object)->handler = handle_rod_hang_ADDRESS;
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			OBJECT(current_object)->hitbox_size_x = depth;
			queue_sprite_at_depth(&ignored, &depth);
			add_to_rider_list();
			return;
		}
		if ((int16_t)OBJECT(current_object)->handle_rod.angle > 0x200)
		{
			OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
			OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
			OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
			depth = (uint16_t)OBJECT(current_object)->screen_y;
			OBJECT(current_object)->hitbox_size_x = depth;
			queue_sprite_at_depth(&ignored, &depth);
			add_to_rider_list();
			return;
		}
		if (OBJECT(current_object)->handle_rod.angle_speed > 0x100)
			OBJECT(current_object)->handle_rod.angle_speed -= 0x100u;
		OBJECT(current_object)->handle_rod.angle_speed--;
		OBJECT(current_object)->handle_rod.swing_accel = 0u - OBJECT(current_object)->handle_rod.swing_accel;
		OBJECT(current_object)->handle_rod.swing_side = 1u;
	}
	OBJECT(current_object)->handle_rod.angle_speed += OBJECT(current_object)->handle_rod.swing_accel;
	OBJECT(current_object)->handle_rod.angle += OBJECT(current_object)->handle_rod.angle_speed_int;
	OBJECT(current_object)->handle_rod.angle &= 0x3FFu;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

/* The rod hangs straight down, falling when there is no floor under it; moved, drawn, rideable; past
   end_position, handle_rod_remove. */
void handle_rod_hang(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	if ((OBJECT(current_object)->flags & 8u) == 0u && OBJECT(current_object)->floor_z_int >= 0x800)
		fall_to_floor(&ignored, &ignored_2, &ignored_3);
	move_object_projected(&ignored, &ignored_3, &ignored_4, &ignored_5);
	if (OBJECT(current_object)->handle_rod.end_position < scroll_position)
	{
		OBJECT(current_object)->handle_rod.end_position = 0xFFFFu;
		OBJECT(current_object)->handler = handle_rod_remove_ADDRESS;
		return;
	}
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->hitbox_size_x = depth;
	queue_sprite_at_depth(&ignored, &depth);
	add_to_rider_list();
}

/* Removes the rod's pivot and its children (delete_object_and_children). */
void handle_rod_remove(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* The handle sits index * 0x12 from the pivot (its parent) along the rod's angle, through the speed
   fields; removed with the rod (end_position 0xFFFF). The grip is touchable and rideable. Drawn by the
   pivot's depth, its shadow on the floor unless the rod drops_players. */
void handle_rod_handle_update(void)
{
	uint32_t depth = 0, handle_distance = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, rod_angle = 0, rod_dx = 0, rod_dz = 0;

	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	if (OBJECT(r14)->handle_rod.end_position == 0xFFFFu)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	if (OBJECT(current_object)->handle_rod_handle.grip == 0u)
		move_object_projected_no_floor();
	else
	{
		move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
		add_to_touchable_list(ignored);
		add_to_rider_list();
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->speed_z = 0;
	if (OBJECT(current_object)->handle_rod_handle.start_angle == 0x300u || OBJECT(r14)->handle_rod.angle != 0u)
	{
		rod_angle = OBJECT(r14)->handle_rod.angle;
		handle_distance = OBJECT(current_object)->handle_rod_handle.index * 0x12u;
		rod_dx = sine_table[rod_angle] * 2u * 2u * handle_distance;
		rod_dz = cosine_table[rod_angle] * 2u * 2u * handle_distance;
		OBJECT(current_object)->speed_x = rod_dx;
		OBJECT(current_object)->speed_y = 0u - ((int32_t)rod_dx >> 1);
		OBJECT(current_object)->speed_z = rod_dz;
	}
	else
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->speed_z = 0x10000u * (OBJECT(current_object)->handle_rod_handle.index * 0x12u);
	}
	OBJECT(current_object)->projected_x = OBJECT(r14)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(r14)->projected_y;
	OBJECT(current_object)->z = OBJECT(r14)->z;
	OBJECT(current_object)->floor_z = OBJECT(r14)->floor_z;
	if (OBJECT(current_object)->screen_x <= 0x145 && OBJECT(current_object)->screen_y >= 0)
	{
		depth = OBJECT(current_object)->handle_rod_handle.index;
		depth = (uint16_t)(depth * 2u);
		depth = (uint16_t)(depth * 2u);
		depth = (uint16_t)(depth + OBJECT(r14)->hitbox_size_x);
		queue_sprite_at_depth(&ignored, &depth);
		if (OBJECT(r14)->handle_rod.drops_players == 0u)
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			place_sprite_at_cursor_floor();
			queue_sprite(depth);
		}
	}
}

/* The near post (0x74C8) with its arm piece (0x74D0), or the far post (0x74D8) when fallen is already
   set; rideable; handle_rod_post_update. */
void handle_rod_post_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	add_to_rider_list();
	OBJECT(current_object)->sprite_frame = 0x74C8u;
	if (OBJECT(current_object)->handle_rod_post.fallen != 0u)
		OBJECT(current_object)->sprite_frame = 0x74D8u;
	OBJECT(current_object)->sprite_palette = 0x9060u;
	OBJECT(current_object)->handler = handle_rod_post_update_ADDRESS;
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
	{
		spawn_count = 1u;
		add_pieces_to_current_object(&ignored, &ignored_2, &ignored_3, &object);
		OBJECT(object)->sprite_frame = 0x74D0u;
		OBJECT(object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
	}
}

/* With no floor under it (floor_z 0x800 or more, not on a platform) the post sets fallen and
   handle_rod_post_fall takes over. Rideable; drawn, a fallen one at the pivot's depth; the near post's
   arm piece is drawn on top. */
void handle_rod_post_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	if ((OBJECT(current_object)->flags & 8u) == 0u && OBJECT(current_object)->floor_z_int >= 0x800)
	{
		OBJECT(current_object)->handle_rod_post.fallen = 1u;
		OBJECT(current_object)->handler = handle_rod_post_fall_ADDRESS;
	}
	add_to_rider_list();
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
		queue_sprite(0u);
	else
	{
		depth = OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->hitbox_size_x;
		queue_sprite_at_depth(&ignored, &depth);
	}
	if (OBJECT(current_object)->handle_rod_post.fallen == 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		offset_x = 0x13u;
		offset_y = 0xFFD6u;
		place_sprite_near_cursor(offset_x, offset_y);
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
}

/* The post falls to its floor and is drawn at it until removed off screen. */
void handle_rod_post_fall(void)
{
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 1u)
	{
		fall_to_floor(&ignored, &ignored_5, &ignored_2);
		queue_sprite_at_floor();
	}
}

/* Object type 15: direction 0x100, then spear_trap_init. */
void spear_trap_init_100(void)
{
	OBJECT(current_object)->direction = 0x100u;
	spear_trap_init();
}

/* Object type 28: direction 0x300, then spear_trap_init. */
void spear_trap_init_300(void)
{
	OBJECT(current_object)->direction = 0x300u;
	spear_trap_init();
}

/* The spear trap of ten objects: spear sprites (0x73A0, 0x7390, 0x7388, 0x7398, 0x73A8) and clip entries
   (sprite_flags 0x5000, their words a clip rectangle) spear_trap_draw sets at the floor line; box 0x18 x
   0x18, height 0x100; first wait spawn_arg_2; spear_trap_wait. */
void spear_trap_init(void)
{
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x18u;
	OBJECT(current_object)->hitbox_size_y = 0x18u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x13;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->spear_trap.phase_timer = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spear_trap.spear_z = OBJECT(current_object)->z;
	OBJECT(current_object)->handler = spear_trap_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A0u;
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->sprite_flags = 0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A0u;
	OBJECT(current_object)->sprite_palette = 0x1050u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	OBJECT(current_object)->sprite_flags = 0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7390u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7388u;
	OBJECT(current_object)->sprite_palette = 0x1030u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x7398u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x73A8u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
}

/* The spear rests (drawn, not touchable) for phase_timer frames, then kind 4, phase_timer 0xC and
   spear_trap_rise; removed off screen. */
void spear_trap_wait(void)
{
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0;
	uint16_t phase_timer = 0;

	OBJECT(current_object)->health = 0x7F;
	delete_if_off_screen(&deleted, &ignored, &ignored_2);
	if ((uint8_t)deleted == 1u)
		return;
	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw();
		return;
	}
	OBJECT(current_object)->spear_trap.phase_timer = 0xCu;
	OBJECT(current_object)->kind = 4u;
	OBJECT(current_object)->handler = spear_trap_rise_ADDRESS;
	spear_trap_rise();
}

/* The spear shoots up (speed_z - 0x10F00 a frame) for 0xC frames, touchable as kind 4; then sound 0x1B,
   kind 0xC, and 0x3C frames of spear_trap_stay_up. */
void spear_trap_rise(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint16_t phase_timer = 0;

	OBJECT(current_object)->speed_z -= 0x10F00u;
	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	sound_id = 0x1Bu;
	play_sound(sound_id, &ignored, &ignored_2);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->spear_trap.phase_timer = 0x3Cu;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spear_trap_stay_up_ADDRESS;
	spear_trap_stay_up();
}

/* The spear stays up and touchable for phase_timer (0x3C) frames, then spear_trap_retract. */
void spear_trap_stay_up(void)
{
	uint16_t phase_timer = 0;

	phase_timer = OBJECT(current_object)->spear_trap.phase_timer;
	OBJECT(current_object)->spear_trap.phase_timer = phase_timer - 1u;
	if (phase_timer != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	OBJECT(current_object)->handler = spear_trap_retract_ADDRESS;
	spear_trap_retract();
}

/* The spear falls back to its floor (fall_to_floor from spear_z), touchable; landed, phase_timer =
   spawn_arg_1 (the period) and spear_trap_wait. */
void spear_trap_retract(void)
{
	uint32_t height = 0, ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->z = OBJECT(current_object)->spear_trap.spear_z;
	fall_to_floor(&height, &ignored, &ignored_2);
	if (height != 0u)
	{
		spear_trap_draw_touchable();
		return;
	}
	OBJECT(current_object)->spear_trap.phase_timer = OBJECT(current_object)->spawn_arg_1;
	OBJECT(current_object)->handler = spear_trap_wait_ADDRESS;
	spear_trap_draw_touchable();
}

/* add_to_touchable_list, then spear_trap_draw. */
void spear_trap_draw_touchable(void)
{
	add_to_touchable_list(0u);
	spear_trap_draw();
}

/* Moves the spear with z from spear_z, then draws its pieces by the floor's screen y, the clip entries
   (bottoms floor - 5 and floor + 0x20) cutting the spears at the floor line. Keeps z in spear_z and
   leaves z = floor_z. */
void spear_trap_draw(void)
{
	uint8_t bucket = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	OBJECT(current_object)->z = OBJECT(current_object)->spear_trap.spear_z;
	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	r14 = (uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int + OBJECT(current_object)->screen_y);
	depth = r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 0x39u;
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xFFFBu + r14;
	depth = (uint16_t)r14;
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y += 0x39u;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x20u + r14;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	OBJECT(current_object)->screen_y += 0x20u;
	bucket = 0xFEu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	OBJECT(current_object)->screen_y += 0x20u;
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = r14;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	OBJECT(current_object)->spear_trap.spear_z = OBJECT(current_object)->z;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
}

/* The ice launcher (sprite 0x7420): box hitbox, height 0x28, kind 5, last_step 0xF, ice_launcher_update. */
void ice_launcher_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7420u;
	OBJECT(current_object)->sprite_palette = 0x9110u;
	OBJECT(current_object)->ice_launcher.last_step = 0xF;
	OBJECT(current_object)->sprite_flags = 9u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = ice_launcher_update_ADDRESS;
}

/* Removed once scroll_position reaches spawn_arg_2. Steps through ice_launcher_frames (each step lasts
   ice_launcher_frame_durations[step_duration_index]) with each frame's hitbox; at step 0xC it launches
   an ice cube (ice_cube_slide, kind 0x19) at ice_cube_speeds[cube_speed_index]. */
void ice_launcher_update(void)
{
	uint32_t cursor = 0, depth = 0, duration_index_or_spawned = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, offset_record = 0, size_record = 0;
	uint16_t step_timer = 0;

	unproject_object(&duration_index_or_spawned, &ignored, &object, &cursor);
	if (OBJECT(current_object)->spawn_arg_2 <= scroll_position)
	{
		delete_object_and_children(&ignored_3, &ignored, &ignored_4);
		return;
	}
	step_timer = OBJECT(current_object)->ice_launcher.step_timer;
	OBJECT(current_object)->ice_launcher.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		add_to_touchable_list(duration_index_or_spawned);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	OBJECT(current_object)->ice_launcher.step++;
	if (OBJECT(current_object)->ice_launcher.last_step < OBJECT(current_object)->ice_launcher.step)
	{
		OBJECT(current_object)->ice_launcher.step = 0;
		OBJECT(current_object)->sprite_frame = 0x7420u;
		OBJECT(current_object)->ice_launcher.step_timer = 0u;
		add_to_touchable_list(duration_index_or_spawned);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	frame_index = ice_launcher_frames[(uint8_t)OBJECT(current_object)->ice_launcher.step];
	OBJECT(current_object)->sprite_frame = ice_launcher_sprites[frame_index];
	size_record = ice_launcher_hitbox_sizes_ADDRESS + frame_index * 4u;
	OBJECT(current_object)->hitbox_size_x = HITBOX_SIZE_PAIR(size_record)->size_x;
	OBJECT(current_object)->hitbox_size_y = HITBOX_SIZE_PAIR(size_record)->size_y;
	offset_record = ice_launcher_hitbox_offsets_ADDRESS + frame_index * 2u;
	OBJECT(current_object)->hitbox_offset_x = HITBOX_OFFSET_PAIR(offset_record)->x;
	offset_record++;
	OBJECT(current_object)->hitbox_offset_y = HITBOX_OFFSET_PAIR(offset_record - 0x1u)->y;
	duration_index_or_spawned = OBJECT(current_object)->ice_launcher.step_duration_index;
	OBJECT(current_object)->ice_launcher.step_timer = ice_launcher_frame_durations[duration_index_or_spawned];
	if ((uint8_t)OBJECT(current_object)->ice_launcher.step != 0xCu)
	{
		add_to_touchable_list(duration_index_or_spawned);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	spawn_count = 2u;
	spawn_object_with_pieces(0x011D7Du, &duration_index_or_spawned, &ignored_2, &offset_record, &object, &cursor);
	if ((uint8_t)duration_index_or_spawned == 0u)
	{
		add_to_touchable_list(duration_index_or_spawned);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	OBJECT(object_cursor)->ice_launcher.step_timer += 3u;
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x2Bu;
	OBJECT(object)->projected_y_int += 0x4Fu;
	OBJECT(object)->sprite_frame = 0x7418u;
	OBJECT(object)->sprite_palette = OBJECT(object_cursor)->sprite_palette;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x22u;
	OBJECT(object)->hitbox_size_y = 0x42u;
	OBJECT(object)->hitbox_offset_x = -0xB;
	OBJECT(object)->hitbox_offset_y = -0x3C;
	OBJECT(object)->hitbox_height = 0x28u;
	OBJECT(object)->kind = 0x19u;
	OBJECT(object)->handler = ice_cube_slide_ADDRESS;
	OBJECT(object)->speed = ice_cube_speeds[OBJECT(current_object)->ice_launcher.cube_speed_index];
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x7418u;
	OBJECT(object)->sprite_palette = 0x11D0u;
	OBJECT(object)->sprite_palette |= 0x2000u;
	add_to_touchable_list(duration_index_or_spawned);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* The launched ice cube slides at direction 0x2B6 with its speed, falling to its floor; drawn at
   screen_y - 0x10 with its shadow 0x1E below in bucket 0xFF; on the hitter list until removed off
   screen. */
void ice_cube_slide(void)
{
	uint8_t bucket = 0;
	uint32_t deleted = 0, depth = 0, direction = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, speed = 0, speed_x = 0, speed_y = 0;

	OBJECT(current_object)->health = 0x7F;
	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	fall_to_floor(&ignored, &ignored_5, &ignored_2);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 1u)
	{
		direction = 0x2B6u;
		speed = OBJECT(current_object)->speed;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x = speed_x;
		OBJECT(current_object)->speed_y = speed_y;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x10u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y += 0x1Eu;
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		current_object = object_cursor;
		add_to_hitter_list();
	}
}

/* The handler object_types gives type 17 (five objects): does nothing. */
void object_type_17_idle(void)
{
}

/* The padded wall (0x77D0, padded_wall_update) and four pads (spawn_objects 4: sprite 0x77A8, box 0x17 x
   0x35, height 0x20, kind 0xB, wall_pad_update) along it, 0x26, 0x13 apart in projected x, y, each
   unlinked from the next; all go at spawn_arg_1. */
void padded_wall_init(void)
{
	uint16_t pad_offset_x = 0, pad_offset_y = 0;
	uint32_t block = 0, object = 0, pads_left = 0, spawned = 0;

	spawn_count = 4u;
	spawn_objects(0x011F2Bu, &spawned, &object, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(current_object)->sprite_frame = 0x77D0u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->handler = padded_wall_update_ADDRESS;
	OBJECT(current_object)->sprite_flags = 0xAu;
	pads_left = 4u;
	pad_offset_x = 0u;
	pad_offset_y = 0u;
	do
	{
		copy_position_to(object);
		OBJECT(object)->direction = 0x300u;
		OBJECT(object)->projected_x += 7u;
		OBJECT(object)->projected_y_int -= 0xAu;
		pad_offset_x += 0x26u;
		pad_offset_y += 0x13u;
		OBJECT(object)->projected_x += pad_offset_x;
		OBJECT(object)->projected_y_int += pad_offset_y;
		OBJECT(object)->sprite_frame = 0x77A8u;
		OBJECT(object)->sprite_palette = 0x9180u;
		OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
		OBJECT(object)->sprite_flags = 9u;
		OBJECT(object)->flags |= 2u;
		OBJECT(object)->hitbox_size_x = 0x17u;
		OBJECT(object)->hitbox_size_y = 0x35u;
		OBJECT(object)->hitbox_offset_x = -0x6E;
		OBJECT(object)->hitbox_offset_y = 0x15;
		OBJECT(object)->hitbox_height = 0x20u;
		OBJECT(object)->kind = 0xBu;
		OBJECT(object)->handler = wall_pad_update_ADDRESS;
		current_object = LINKED(current_object, OBJECT(object)->child);
		OBJECT(object)->child = 0xFFFFu;
		object = current_object;
		pads_left--;
	} while (pads_left != 0u);
}

/* The wall is drawn until scroll_position passes spawn_arg_1, then removed with its children. */
void padded_wall_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	if (scroll_position <= OBJECT(current_object)->spawn_arg_1)
	{
		queue_sprite(0u);
		return;
	}
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* A pad of the padded wall: once touched it plays one round of wall_pad_frames (wall_pad_sprites,
   wall_pad_frame_durations); drawn and touchable; removed past spawn_arg_1. */
void wall_pad_update(void)
{
	uint32_t frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, pad_step = 0;
	uint16_t frame_timer = 0;

	unproject_object(&pad_step, &ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->spawn_arg_1 < scroll_position)
	{
		delete_object_and_children(&ignored_4, &ignored, &ignored_2);
		return;
	}
	if (OBJECT(current_object)->wall_pad.frame_step != 0u || (OBJECT(current_object)->flags & 4u) != 0u)
	{
		frame_timer = OBJECT(current_object)->wall_pad.frame_timer;
		OBJECT(current_object)->wall_pad.frame_timer = frame_timer - 1u;
		if (frame_timer == 0u)
		{
			OBJECT(current_object)->wall_pad.frame_step++;
			OBJECT(current_object)->wall_pad.frame_step = OBJECT(current_object)->wall_pad.frame_step % 0xBu;
			pad_step = OBJECT(current_object)->wall_pad.frame_step;
			frame_index = wall_pad_frames[pad_step];
			OBJECT(current_object)->sprite_frame = wall_pad_sprites[frame_index];
			OBJECT(current_object)->wall_pad.frame_timer = wall_pad_frame_durations[pad_step];
		}
	}
	queue_sprite(frame_index);
	add_to_touchable_list(pad_step);
}

/* The snow mound (0x7B30, palette 0x91B0) 0x20 lower in projected y: box 0x22 x 0x21, height 0x20, kind
   5, rideable; part 3 flipped in x, part 4 flipped in y (palette 0x31C0, its reflection on the ice);
   snow_mound_update. */
void snow_mound_init(void)
{
	OBJECT(current_object)->projected_y_int += 0x20u;
	add_to_rider_list();
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_frame = 0x7B30u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x22u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = -0x21;
	OBJECT(current_object)->hitbox_offset_y = 3;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->snow_mound.part_3_frame = 4u;
	OBJECT(current_object)->handler = snow_mound_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x40u;
	OBJECT(current_object)->sprite_palette = 0x91B0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0u;
	OBJECT(current_object)->sprite_flags |= 0x80u;
	OBJECT(current_object)->sprite_palette = 0x11C0u;
	OBJECT(current_object)->sprite_palette |= 0x2000u;
}

/* The mound moves and falls to its floor; removed off screen. At the first touch it bursts
   (snow_mound_burst), dropping its ring_count rings first (drop_rings at angle spawn_arg_2,
   random_scatter for the spread); untouched, snow_mound_draw. */
void snow_mound_update(void)
{
	uint32_t cursor = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, random_value = 0, ring = 0, slope_height = 0;

	move_object_projected(&ignored, &ignored_2, &slope_height, &cursor);
	fall_to_floor(&ignored, &ignored_3, &ignored_2);
	delete_if_off_screen(&deleted, &ignored_2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->snow_mound.bursting != 0u)
	{
		snow_mound_burst();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		snow_mound_draw();
		return;
	}
	OBJECT(current_object)->snow_mound.bursting = 1u;
	if (OBJECT(current_object)->snow_mound.ring_count == 0u)
	{
		snow_mound_burst();
		return;
	}
	released_ring_flip = 0u;
	rings_to_release = OBJECT(current_object)->snow_mound.ring_count;
	released_ring_angle = OBJECT(current_object)->spawn_arg_2;
	released_rings_random = OBJECT(current_object)->snow_mound.random_scatter;
	drop_rings(slope_height, &cursor, &random_value, &ring);
	snow_mound_burst();
}

/* The burst into snow: every second frame the mound takes the next of snow_mound_burst_frames and part 2
   the frame two ahead; every other step, part 3 its own next frame (from part_3_frame 4). Past frame 0xE
   the mound and its parts are removed. Drawn by snow_mound_draw. */
void snow_mound_burst(void)
{
	uint32_t burst_frame = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;

	OBJECT(current_object)->snow_mound.frame_count++;
	OBJECT(current_object)->snow_mound.part_3_count++;
	if (OBJECT(current_object)->snow_mound.frame_count <= 1)
	{
		snow_mound_draw();
		return;
	}
	if (OBJECT(current_object)->snow_mound.burst_frame <= 0xE)
	{
		burst_frame = (uint8_t)OBJECT(current_object)->snow_mound.burst_frame;
		OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[burst_frame];
		OBJECT(current_object)->snow_mound.frame_count = 0;
		OBJECT(current_object)->snow_mound.burst_frame++;
		burst_frame += 2u;
		if ((int8_t)burst_frame <= 0xE)
		{
			object = LINKED(WORK_RAM, OBJECT(current_object)->child);
			OBJECT(object)->sprite_frame = snow_mound_burst_frames[burst_frame];
			if (OBJECT(current_object)->snow_mound.part_3_count <= 2)
			{
				snow_mound_draw();
				return;
			}
			object = LINKED(object, OBJECT(current_object)->child);
			object = LINKED(object, OBJECT(object)->child);
			OBJECT(current_object)->snow_mound.part_3_count = 0;
			OBJECT(current_object)->snow_mound.part_3_frame++;
			OBJECT(object)->sprite_frame = snow_mound_burst_frames[OBJECT(current_object)->snow_mound.part_3_frame];
			snow_mound_draw();
			return;
		}
		snow_mound_draw();
		return;
	}
	OBJECT(current_object)->snow_mound.frame_count = 0;
	OBJECT(current_object)->snow_mound.part_3_count = 0;
	OBJECT(current_object)->snow_mound.burst_frame = 0;
	OBJECT(current_object)->snow_mound.bursting = 0u;
	OBJECT(current_object)->snow_mound.part_3_frame = 4u;
	OBJECT(current_object)->sprite_frame = snow_mound_burst_frames[(uint8_t)OBJECT(current_object)->snow_mound.burst_frame];
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* Draws the mound at screen_y + 0x12. Whole, it is touchable and rideable and part 4, its reflection,
   copies its frame 0x15 lower in bucket 0xFF; bursting, part 2 at its place, part 3 0x16 higher and part
   4 with part 2's frame. */
void snow_mound_draw(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t depth = 0, ignored = 0, object = 0;

	depth = (uint16_t)OBJECT(current_object)->screen_y;
	depth = (uint16_t)(depth + 0x12u);
	queue_sprite_at_depth(&ignored, &depth);
	if ((uint8_t)OBJECT(current_object)->snow_mound.burst_frame == 0u)
	{
		add_to_touchable_list(ignored);
		add_to_rider_list();
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		object = current_object;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		if ((uint8_t)OBJECT(object_cursor)->snow_mound.burst_frame != 0u)
			OBJECT(current_object)->sprite_frame = OBJECT(object)->sprite_frame;
		offset_x = 0u;
		offset_y = 0x15u;
		place_sprite_near_cursor(offset_x, offset_y);
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		return;
	}
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x11u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y -= 0x16u;
	queue_sprite(depth);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	object = current_object;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
	if ((uint8_t)OBJECT(object_cursor)->snow_mound.burst_frame != 0u)
		OBJECT(current_object)->sprite_frame = OBJECT(object)->sprite_frame;
	offset_x = 0u;
	offset_y = 0x15u;
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* Once 0xE objects are free: 0xE ice blocks (sprite 0x7498, box 0x20 x 0x5C, height 0x50, kind 0xC,
   ice_block_update) at x -0x2B2, z 0x78, 0x5C apart along y from its y; then removed. */
void ice_block_row_start(void)
{
	uint32_t ignored = 0, object = 0;
	uint16_t blocks_left = 0;

	OBJECT(current_object)->ice_block_row.blocks_left = 0xE;
	OBJECT(current_object)->handler = ice_block_row_start_ADDRESS;
	if (free_object_stack < OBJECT(current_object)->ice_block_row.blocks_left)
		return;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1002u;
		OBJECT(object)->sprite_frame = 0x7498u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->handler = ice_block_update_ADDRESS;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x20u;
		OBJECT(object)->hitbox_size_y = 0x5Cu;
		OBJECT(object)->hitbox_height = 0x50u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->speed_z = 0;
		OBJECT(object)->ice_block.state = 0u;
		OBJECT(object)->x = -0x2B20000;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = 0x780000;
		OBJECT(current_object)->y_int -= 0x5Cu;
		blocks_left = OBJECT(current_object)->ice_block_row.blocks_left;
		OBJECT(current_object)->ice_block_row.blocks_left = blocks_left - 1u;
	} while ((int16_t)blocks_left > 1);
	delete_current_object();
}

/* By state: projected; touchable until touched, then it slides towards -x at 1.0 a frame (pushed by an
   object that is not a player, at its speed_fraction << 8); it falls past x -0x2BA and is removed below
   screen y 0x120. */
void ice_block_update(void)
{
	uint16_t margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, object = 0, off_screen = 0, push_speed = 0;

	switch (OBJECT(current_object)->ice_block.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_block.state++;
		project_object(&ignored, &ignored_2);
		return;
	case 1:
		margin = 0x50u;
		off_screen = check_left_screen_margin(margin);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 4u) == 0u)
		{
			add_to_touchable_list(push_speed);
			update_screen_position(&ignored, &ignored_2);
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->ice_block.state++;
		object = LINKED(current_object, OBJECT(current_object)->touched_by);
		push_speed = 0x10000u;
		if ((int8_t)OBJECT(object)->kind > 2)
			push_speed = (uint32_t)OBJECT(object)->speed_fraction << 8;
		OBJECT(current_object)->speed_x = push_speed;
		add_to_touchable_list(push_speed);
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		OBJECT(current_object)->x -= OBJECT(current_object)->speed_x;
		if (OBJECT(current_object)->x_int <= -0x2BA)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		}
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->screen_y < 0x120)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		invalid_jump(0x87E2Fu + OBJECT(current_object)->ice_block.state * 2u);
	}
}

/* The ice platform's invisible leader from the spawn record: group_leaders[its leader & 3],
   end_position, travel = x - reference_x, x, y, z, speed_x 1.0, ice_platform_leader_move; it becomes
   moving_floor_object. spawned = -1, or 0 with no free object. */
void spawn_ice_platform_leader(uint32_t record, uint32_t *p_spawned, uint32_t *p_travel, uint32_t *p_leader)
{
	uint16_t travel = (uint16_t)*p_travel;
	uint32_t leader = *p_leader;
	uint16_t reference_x = 0;

	allocate_object_outside_loop(p_spawned, &leader);
	if (*p_spawned != 0u)
	{
		group_leaders[ICE_LEADER_SPAWN(record)->leader & 3u] = leader;
		record++;
		OBJECT(leader)->ice_platform_leader.end_position = ICE_LEADER_SPAWN(record - 0x1u)->end_position;
		record += 2u;
		reference_x = (uint16_t)ICE_LEADER_SPAWN(record - 0x3u)->reference_x;
		record += 2u;
		travel = (uint16_t)(ICE_LEADER_SPAWN(record - 0x5u)->x - reference_x);
		OBJECT(leader)->ice_platform_leader.travel = travel;
		OBJECT(leader)->x_int = ICE_LEADER_SPAWN(record - 0x5u)->x;
		OBJECT(leader)->x_fraction = 0u;
		record += 2u;
		OBJECT(leader)->y_int = ICE_LEADER_SPAWN(record - 0x7u)->y;
		OBJECT(leader)->y_fraction = 0u;
		OBJECT(leader)->z_int = ICE_LEADER_SPAWN(record - 0x7u)->z;
		OBJECT(leader)->z_fraction = 0u;
		OBJECT(leader)->floor_z = OBJECT(leader)->z;
		OBJECT(leader)->handler = ice_platform_leader_move_ADDRESS;
		OBJECT(leader)->speed_x = 0x10000;
		OBJECT(leader)->speed_y = 0;
		OBJECT(leader)->flags = 0u;
		OBJECT(leader)->sprite_offset_x = 0;
		OBJECT(leader)->sprite_offset_y = 0;
		moving_floor_object = leader;
		*p_spawned = 0xFFFFFFFFu;
	}
	*p_travel = (*p_travel & 0xFFFF0000u) | travel;
	*p_leader = leader;
}

/* The invisible leader of the moving ice platform (moving_floor_object): x + speed_x a frame, turning
   back (sound 0xF) when travel leaves -0x87..0x87; removed once scroll_position reaches end_position. */
void ice_platform_leader_move(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->ice_platform_leader.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->ice_platform_leader.travel += OBJECT(current_object)->speed_x_int;
	if ((uint16_t)(OBJECT(current_object)->ice_platform_leader.travel + 0x87u) >= 0x10Fu)
	{
		OBJECT(current_object)->ice_platform_leader.travel = 0;
		OBJECT(current_object)->speed_x = 0u - OBJECT(current_object)->speed_x;
		sound_id = 0xFu;
		play_sound(sound_id, &ignored_2, &ignored_3);
	}
}

/* A piece of the ice platform, by state: at its leader's (parent's) x - member * 0x2D and y, on the
   platform list, until an icicle lands on it (icicle_landed): ice_platform_piece_parts_2 and it falls
   until off screen. The last piece (member 3) starts with those parts. */
void ice_platform_piece_update(void)
{
	uint16_t margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, object = 0, off_screen = 0;

	switch (OBJECT(current_object)->ice_platform_piece.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->ice_platform_piece.state++;
		OBJECT(current_object)->speed_z = 0;
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->ice_platform_piece.member == 3u)
			OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
		return;
	case 1:
		break;
	case 2:
		off_screen = is_off_screen();
		if (off_screen == 0u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			update_screen_position(&ignored, &ignored_2);
			depth = OBJECT(current_object)->draw_depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	default:
		invalid_jump(0x87FB3u + OBJECT(current_object)->ice_platform_piece.state * 3u);
	}
	margin = 0x60u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->ice_platform_piece.icicle_landed != 0u)
	{
		OBJECT(current_object)->ice_platform_piece.state++;
		OBJECT(current_object)->part_list = ice_platform_piece_parts_2_ADDRESS;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->x_int -= OBJECT(current_object)->ice_platform_piece.member * 0x2Du;
	add_to_platform_list();
	project_object(&ignored, &ignored_2);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->ice_platform_piece.depth_offset - OBJECT(current_object)->sprite_offset_y);
	OBJECT(current_object)->draw_depth = depth;
	queue_sprite_at_depth(&ignored, &depth);
}

/* With 9 objects free (it takes 35): 5 rows of 7 ice cubes (ice_floor_block_update, sprite 0x7490, the
   first of each row palette | 0x2000) 0x2E apart from its x, y, on the floor block
   ice_floor_floors[sprite_frame] at its height; then removed. */
void ice_floor_start(void)
{
	uint32_t cubes_left = 0, ignored = 0, object = 0, rows_left = 0;

	if (free_object_stack < 9)
		return;
	ice_row_start_x = OBJECT(current_object)->x;
	ice_cube_floor_block = ice_floor_floors[OBJECT(current_object)->sprite_frame];
	rows_left = 5u;
	do
	{
		cubes_left = 7u;
		do
		{
			allocate_object(&ignored, &object);
			OBJECT(object)->sprite_flags = 0x100Au;
			OBJECT(object)->sprite_frame = 0x7490u;
			OBJECT(object)->sprite_palette = 0x9120u;
			OBJECT(object)->handler = ice_floor_block_update_ADDRESS;
			OBJECT(object)->flags = 0u;
			OBJECT(object)->hitbox_size_x = 0x30u;
			OBJECT(object)->hitbox_size_y = 0x30u;
			OBJECT(object)->hitbox_height = 0x30u;
			OBJECT(object)->sprite_offset_x = 2;
			OBJECT(object)->sprite_offset_y = -0x10;
			OBJECT(object)->hitbox_offset_x = 0;
			OBJECT(object)->hitbox_offset_y = 0;
			OBJECT(object)->speed_z = 0;
			OBJECT(object)->ice_floor_block.state = 0u;
			OBJECT(object)->floor_block = ice_cube_floor_block;
			if ((uint16_t)cubes_left == 8u || (uint16_t)cubes_left == 7u)
				OBJECT(object)->sprite_palette |= 0x2000u;
			OBJECT(object)->x = OBJECT(current_object)->x;
			OBJECT(object)->y = OBJECT(current_object)->y;
			OBJECT(current_object)->x_int += 0x2Eu;
			OBJECT(object)->floor_z_int = mem_read16(floor_blocks + 2u + OBJECT(object)->floor_block + 0x10u);
			OBJECT(object)->floor_z_fraction = 0u;
			OBJECT(object)->z = OBJECT(object)->floor_z;
			cubes_left--;
		} while (cubes_left != 0u);
		OBJECT(current_object)->y_int += 0x2Eu;
		OBJECT(current_object)->x = ice_row_start_x;
		rows_left--;
	} while (rows_left != 0u);
	delete_current_object();
}

/* An ice cube of the floor, by state: a platform until a player stands on it (sound 0x14); it shakes (z
   +/- 1) for 0x1E frames, 0x12C while that player's invulnerable_timer runs; then palette | 0x2000,
   sound 0x2A, and it falls until off screen. */
void ice_floor_block_update(void)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, off_screen = 0;
	uint8_t shake_timer = 0;
	uint16_t shake_duration = 0;

	switch (OBJECT(current_object)->ice_floor_block.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->ice_floor_block.state++;
		project_object(&ignored_3, &ignored);
		return;
	case 1:
		off_screen = check_left_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 8u) != 0u)
		{
			object = LINKED(current_object, OBJECT(current_object)->platform);
			if ((int8_t)OBJECT(object)->kind <= 2)
			{
				OBJECT(current_object)->ice_floor_block.state++;
				OBJECT(current_object)->ice_floor_block.shake_offset = 1;
				OBJECT(current_object)->ice_floor_block.shake_timer = 0u;
				OBJECT(current_object)->ice_floor_block.shake_duration = 0x1E;
				sound_id = 0x14u;
				play_sound(sound_id, &ignored, &ignored_2);
				if (PLAYER(PLAYER_BLOCK(1) + ((uint32_t)OBJECT(object)->player << 8) * 2u)->invulnerable_timer > 0)
					OBJECT(current_object)->ice_floor_block.shake_duration = 0x12C;
			}
		}
		add_to_platform_list();
		update_screen_position(&ignored_3, &ignored);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	case 2:
		shake_duration = OBJECT(current_object)->ice_floor_block.shake_duration;
		OBJECT(current_object)->ice_floor_block.shake_duration = shake_duration - 1u;
		if ((int16_t)shake_duration <= 1)
		{
			OBJECT(current_object)->ice_floor_block.state++;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
			sound_id = 0x2Au;
			play_sound(sound_id, &ignored, &ignored_2);
		}
		shake_timer = OBJECT(current_object)->ice_floor_block.shake_timer;
		OBJECT(current_object)->ice_floor_block.shake_timer = shake_timer - 1u;
		if ((int8_t)shake_timer <= 1)
		{
			OBJECT(current_object)->ice_floor_block.shake_timer = 2u;
			OBJECT(current_object)->z_int += OBJECT(current_object)->ice_floor_block.shake_offset;
			OBJECT(current_object)->ice_floor_block.shake_offset = 0u - OBJECT(current_object)->ice_floor_block.shake_offset;
		}
		add_to_platform_list();
		update_screen_position(&ignored_3, &ignored);
		depth = (uint16_t)OBJECT(current_object)->screen_y;
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	case 3:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored_3, &ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	default:
		invalid_jump(0x8816Cu + OBJECT(current_object)->ice_floor_block.state * 3u);
	}
}

/* Waits for icicle_dropper_ready (and clears it); then from icicle_dropper_records[sprite_frame]: x, y,
   z, direction, rides_platform, last_dropper and the path (icicle_dropper_scripts); travel 0x180,
   wait_timer 1; icicle_dropper_move. current_icicle_dropper = it. */
void icicle_dropper_start(void)
{
	uint32_t dropper_record = 0;

	if (icicle_dropper_ready == 0u)
		return;
	icicle_dropper_ready = 0u;
	dropper_record = icicle_dropper_records_ADDRESS + icicle_dropper_records[OBJECT(current_object)->sprite_frame];
	OBJECT(current_object)->x_int = ICICLE_DROPPER_RECORD(dropper_record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	dropper_record += 2u;
	OBJECT(current_object)->y_int = ICICLE_DROPPER_RECORD(dropper_record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	dropper_record += 2u;
	OBJECT(current_object)->z_int = ICICLE_DROPPER_RECORD(dropper_record - 0x4u)->z;
	OBJECT(current_object)->z_fraction = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	dropper_record += 2u;
	OBJECT(current_object)->direction = ICICLE_DROPPER_RECORD(dropper_record - 0x6u)->direction;
	dropper_record += 2u;
	OBJECT(current_object)->icicle_dropper.record_flags = ICICLE_DROPPER_RECORD(dropper_record - 0x8u)->flags;
	OBJECT(current_object)->icicle_dropper.path = icicle_dropper_scripts_ADDRESS + ICICLE_DROPPER_RECORD(dropper_record - 0x8u)->script;
	OBJECT(current_object)->handler = icicle_dropper_move_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->icicle_dropper.holding_back = 0u;
	OBJECT(current_object)->icicle_dropper.platform_offset = 0;
	OBJECT(current_object)->icicle_dropper.travel = 0x180;
	OBJECT(current_object)->icicle_dropper.wait_timer = 1;
	OBJECT(current_object)->flags = 0u;
	current_icicle_dropper = current_object;
}

/* An invisible chaser that drops rows of icicles: holds back on a player mishap; after wait_timer the
   pending path record loads once its coordinate or scroll position is reached (riding the ice platform,
   platform_offset reaching the coordinate ends the path). Then icicle_dropper_move_step. */
void icicle_dropper_move(void)
{
	uint16_t hold_threshold = 0;
	uint32_t direction = 0, path = 0;
	uint16_t wait_timer = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) != 0u)
	{
		if (OBJECT(current_object)->icicle_dropper.holding_back != 0u)
		{
			icicle_dropper_move_step();
			return;
		}
		if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
		{
			OBJECT(current_object)->icicle_dropper.holding_back = 0xFFu;
			OBJECT(current_object)->icicle_dropper.saved_base_speed = OBJECT(current_object)->icicle_dropper.base_speed;
			OBJECT(current_object)->icicle_dropper.base_speed = 0;
			OBJECT(current_object)->icicle_dropper.hold_start_position = scroll_position;
			OBJECT(current_object)->icicle_dropper.hold_timer = 0xFFu;
			OBJECT(current_object)->icicle_dropper.saved_threshold = OBJECT(current_object)->icicle_dropper.screen_threshold;
			hold_threshold = 0xFFE0u;
			if (OBJECT(current_object)->direction < 0x100 || OBJECT(current_object)->direction > 0x200)
				hold_threshold = 0x160u;
			OBJECT(current_object)->icicle_dropper.screen_threshold = hold_threshold;
			icicle_dropper_move_step();
			return;
		}
	}
	wait_timer = OBJECT(current_object)->icicle_dropper.wait_timer;
	OBJECT(current_object)->icicle_dropper.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
	{
		icicle_dropper_move_step();
		return;
	}
	OBJECT(current_object)->icicle_dropper.wait_timer = 0;
	path = OBJECT(current_object)->icicle_dropper.path;
	if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
	{
		if (OBJECT(current_object)->icicle_dropper.platform_offset_int < ICICLE_DROPPER_STEP(path)[-1].end_coordinate)
		{
			icicle_dropper_check_scroll_position(path);
			return;
		}
		icicle_dropper_ready = 0xFFu;
		if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u)
		{
			OBJECT(current_object)->icicle_dropper.finish_timer = 3u;
			OBJECT(current_object)->handler = icicle_dropper_finish_ADDRESS;
			icicle_dropper_finish();
			return;
		}
		delete_current_object();
		return;
	}
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
		if (OBJECT(current_object)->y_int <= ICICLE_DROPPER_STEP(path)[-1].end_coordinate)
		{
			icicle_dropper_coordinate_reached(path);
			return;
		}
		icicle_dropper_check_scroll_position(path);
		return;
	case 1:
		if (ICICLE_DROPPER_STEP(path)[-1].end_coordinate <= OBJECT(current_object)->x_int)
		{
			icicle_dropper_coordinate_reached(path);
			return;
		}
		icicle_dropper_check_scroll_position(path);
		return;
	case 2:
	case 4:
		if (ICICLE_DROPPER_STEP(path)[-1].end_coordinate <= OBJECT(current_object)->y_int)
		{
			icicle_dropper_coordinate_reached(path);
			return;
		}
		icicle_dropper_check_scroll_position(path);
		return;
	case 3:
		if (ICICLE_DROPPER_STEP(path)[-1].end_coordinate < OBJECT(current_object)->x_int)
		{
			icicle_dropper_check_scroll_position(path);
			return;
		}
		icicle_dropper_coordinate_reached(path);
		return;
	default:
		invalid_jump(0x883C6u + (direction >> 8) * 2u);
	}
}

/* The dropper reached the pending record's coordinate: icicle_dropper_load_record, skipping the record's
   scroll position. */
void icicle_dropper_coordinate_reached(uint32_t path_cursor)
{
	path_cursor += 2u;
	icicle_dropper_load_record(path_cursor);
}

/* The pending path record loads (icicle_dropper_load_record) once scroll_position reaches its scroll
   position; until then icicle_dropper_move_step. */
void icicle_dropper_check_scroll_position(uint32_t path_cursor)
{
	uint16_t record_scroll = 0;

	record_scroll = ICICLE_DROPPER_STEP(path_cursor)->position;
	path_cursor += 2u;
	if (scroll_position < record_scroll)
	{
		icicle_dropper_move_step();
		return;
	}
	icicle_dropper_load_record(path_cursor);
}

/* Loads the pending path record's screen_threshold, catch_up_factor, base_speed and wait_timer and moves
   path to the next; a negative factor ends the path: icicle_dropper_ready = 0xFF and removed (the last
   dropper: icicle_dropper_finish). */
void icicle_dropper_load_record(uint32_t path_cursor)
{
	if (ICICLE_DROPPER_STEP(path_cursor - 0x2u)->catch_up_factor < 0)
	{
		icicle_dropper_ready = 0xFFu;
		if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u)
		{
			OBJECT(current_object)->icicle_dropper.finish_timer = 3u;
			OBJECT(current_object)->handler = icicle_dropper_finish_ADDRESS;
			icicle_dropper_finish();
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->icicle_dropper.screen_threshold = ICICLE_DROPPER_STEP(path_cursor - 0x2u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->icicle_dropper.catch_up_factor = ICICLE_DROPPER_STEP(path_cursor - 0x4u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->icicle_dropper.base_speed = ICICLE_DROPPER_STEP(path_cursor - 0x6u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->icicle_dropper.wait_timer = ICICLE_DROPPER_STEP(path_cursor - 0x8u)->wait;
	OBJECT(current_object)->icicle_dropper.path = path_cursor + 4u;
	icicle_dropper_move_step();
}

/* On its floor, or riding at moving_floor_object's x - platform_offset + 0x2D; projected. A hold-back
   ends after 0x300 of scroll, when hold_timer runs out, or for the last dropper once every player
   finished (catching up); else icicle_dropper_choose_speed. */
void icicle_dropper_move_step(void)
{
	uint32_t cursor = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, slope_height = 0;
	uint8_t hold_timer = 0;

	if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
	{
		find_floor(&ignored, &ignored_2, &ignored_3, &slope_height, &cursor);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
	{
		OBJECT(current_object)->x = OBJECT(LINKED(current_object, moving_floor_object))->x;
		OBJECT(current_object)->x -= OBJECT(current_object)->icicle_dropper.platform_offset;
		OBJECT(current_object)->x_int += 0x2Du;
	}
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->icicle_dropper.holding_back == 0u)
	{
		icicle_dropper_choose_speed(slope_height, cursor);
		return;
	}
	if ((int16_t)(scroll_position - OBJECT(current_object)->icicle_dropper.hold_start_position) >= 0x300 || (OBJECT(current_object)->icicle_dropper.last_dropper != 0u && (players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u)) == 0xFFFFFFu))
	{
		OBJECT(current_object)->icicle_dropper.holding_back = 0u;
		OBJECT(current_object)->icicle_dropper.screen_threshold = OBJECT(current_object)->icicle_dropper.saved_threshold;
		OBJECT(current_object)->icicle_dropper.base_speed = OBJECT(current_object)->icicle_dropper.saved_base_speed;
		icicle_dropper_catch_up_speed(slope_height, cursor);
		return;
	}
	if (no_player_in_play != 0u)
		OBJECT(current_object)->icicle_dropper.hold_timer = 0xFFu;
	hold_timer = OBJECT(current_object)->icicle_dropper.hold_timer;
	OBJECT(current_object)->icicle_dropper.hold_timer = hold_timer - 1u;
	if (hold_timer == 0u)
	{
		OBJECT(current_object)->icicle_dropper.holding_back = 0u;
		OBJECT(current_object)->icicle_dropper.screen_threshold = OBJECT(current_object)->icicle_dropper.saved_threshold;
		OBJECT(current_object)->icicle_dropper.base_speed = OBJECT(current_object)->icicle_dropper.saved_base_speed;
		icicle_dropper_catch_up_speed(slope_height, cursor);
		return;
	}
	icicle_dropper_choose_speed(slope_height, cursor);
}

/* base_speed once the dropper is past screen_threshold in screen_x (the way it moves); else wait_timer
   is dropped, a hold-back ends and it catches up (icicle_dropper_catch_up_speed). Then
   icicle_dropper_move_at_speed. */
void icicle_dropper_choose_speed(uint32_t slope_height, uint32_t cursor)
{
	uint16_t move_speed = 0;
	uint32_t direction = 0;

	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->icicle_dropper.screen_threshold)
		{
			move_speed = (uint16_t)OBJECT(current_object)->icicle_dropper.base_speed;
			icicle_dropper_move_at_speed(move_speed, slope_height, cursor);
			return;
		}
		OBJECT(current_object)->icicle_dropper.wait_timer = 0;
		if (OBJECT(current_object)->icicle_dropper.holding_back == 0u)
		{
			icicle_dropper_catch_up_speed(slope_height, cursor);
			return;
		}
		OBJECT(current_object)->icicle_dropper.holding_back = 0u;
		OBJECT(current_object)->icicle_dropper.screen_threshold = OBJECT(current_object)->icicle_dropper.saved_threshold;
		OBJECT(current_object)->icicle_dropper.base_speed = OBJECT(current_object)->icicle_dropper.saved_base_speed;
		icicle_dropper_catch_up_speed(slope_height, cursor);
		return;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->icicle_dropper.screen_threshold <= OBJECT(current_object)->screen_x)
		{
			move_speed = (uint16_t)OBJECT(current_object)->icicle_dropper.base_speed;
			icicle_dropper_move_at_speed(move_speed, slope_height, cursor);
			return;
		}
		OBJECT(current_object)->icicle_dropper.wait_timer = 0;
		if (OBJECT(current_object)->icicle_dropper.holding_back == 0u)
		{
			icicle_dropper_catch_up_speed(slope_height, cursor);
			return;
		}
		OBJECT(current_object)->icicle_dropper.holding_back = 0u;
		OBJECT(current_object)->icicle_dropper.screen_threshold = OBJECT(current_object)->icicle_dropper.saved_threshold;
		OBJECT(current_object)->icicle_dropper.base_speed = OBJECT(current_object)->icicle_dropper.saved_base_speed;
		icicle_dropper_catch_up_speed(slope_height, cursor);
		return;
	default:
		invalid_jump(0x884C0u + (direction >> 8) * 2u);
	}
}

/* The catching-up speed: camera_slope_speed_int (compute_camera_slope_speed with the floor slope's
   floor_slope_factors entry) x catch_up_factor >> 8, at least base_speed; then
   icicle_dropper_move_at_speed. */
void icicle_dropper_catch_up_speed(uint32_t slope_height, uint32_t cursor)
{
	uint16_t move_speed = 0;
	uint32_t floor_slope = 0, slope_factor = 0;

	slope_factor = 0u;
	if ((OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
	{
		floor_slope = OBJECT(current_object)->floor_slope;
		slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
	}
	slope_height = compute_camera_slope_speed(slope_height, slope_factor);
	move_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->icicle_dropper.catch_up_factor) >> 8;
	if (OBJECT(current_object)->icicle_dropper.base_speed < (int16_t)move_speed)
	{
		icicle_dropper_move_at_speed(move_speed, slope_height, cursor);
		return;
	}
	move_speed = (uint16_t)OBJECT(current_object)->icicle_dropper.base_speed;
	icicle_dropper_move_at_speed(move_speed, slope_height, cursor);
}

/* chaser_speed = move_speed (0x400 for the last dropper once every player finished); after the wait it
   moves that far along its direction (only platform_offset while riding) and counts travel; unless the
   row lined up (flags bit 0), drop_icicle_row every 0x400 of travel (0x300 riding). */
void icicle_dropper_move_at_speed(uint16_t move_speed, uint32_t slope_height, uint32_t cursor)
{
	uint16_t row_spacing = 0;
	uint32_t direction = 0, step = 0;

	chaser_speed = move_speed;
	if (OBJECT(current_object)->icicle_dropper.wait_timer <= 0)
	{
		if (OBJECT(current_object)->icicle_dropper.last_dropper != 0u && (players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u)) == 0xFFFFFFu)
			chaser_speed = 0x400;
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
			step = chaser_speed;
			OBJECT(current_object)->icicle_dropper.travel += step;
			step = step << 12;
			OBJECT(current_object)->icicle_dropper.platform_offset += step;
			if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
				OBJECT(current_object)->y -= step;
			break;
		case 1:
			step = chaser_speed;
			OBJECT(current_object)->icicle_dropper.travel += step;
			step = step << 12;
			OBJECT(current_object)->icicle_dropper.platform_offset += step;
			if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
				OBJECT(current_object)->x += step;
			break;
		case 2:
		case 4:
			step = chaser_speed;
			OBJECT(current_object)->icicle_dropper.travel += step;
			step = step << 12;
			OBJECT(current_object)->icicle_dropper.platform_offset += step;
			if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
				OBJECT(current_object)->y += step;
			break;
		case 3:
			step = chaser_speed;
			OBJECT(current_object)->icicle_dropper.travel += step;
			step = step << 12;
			OBJECT(current_object)->icicle_dropper.platform_offset += step;
			if (OBJECT(current_object)->icicle_dropper.rides_platform == 0u)
				OBJECT(current_object)->x -= step;
			break;
		default:
			invalid_jump(0x88572u + (direction >> 8) * 2u);
		}
	}
	if ((OBJECT(current_object)->flags & 1u) != 0u)
		return;
	row_spacing = 0x400u;
	if (OBJECT(current_object)->icicle_dropper.rides_platform != 0u)
		row_spacing = 0x300u;
	if ((int16_t)row_spacing <= OBJECT(current_object)->icicle_dropper.travel)
	{
		OBJECT(current_object)->icicle_dropper.travel -= row_spacing;
		drop_icicle_row(slope_height, cursor);
	}
}

/* The last dropper's end: once its icicles have counted goal_unlocked up to 3, after finish_timer (3
   frames) goal_unlocked = 0xFF and it is removed. */
void icicle_dropper_finish(void)
{
	uint16_t finish_timer = 0;

	if ((int8_t)goal_unlocked >= 3)
	{
		finish_timer = OBJECT(current_object)->icicle_dropper.finish_timer;
		OBJECT(current_object)->icicle_dropper.finish_timer = finish_timer - 1u;
		if ((int16_t)finish_timer <= 1)
		{
			goal_unlocked = 0xFFu;
			delete_current_object();
			return;
		}
	}
}

/* Three icicles (icicle_update, kind 0x28) 0xE0 above the dropper, 0x48 apart across its direction,
   falling after 0x3C-0x5B frames, each with an icicle_shadow_follow shadow. Within 0x40 of the pending
   path record's coordinate they line up 0x20 short of it (flags bit 0 on them and the dropper). */
void drop_icicle_row(uint32_t random_value, uint32_t block)
{
	uint16_t end_coordinate = 0, line_coordinate = 0;
	uint32_t direction = 0, free_count = 0, icicles_left = 0, ignored = 0, ignored_2 = 0, object = 0;

	icicles_left = 3u;
	icicle_row_offset = 0x20u;
	do
	{
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			break;
		OBJECT(block)->sprite_flags = 0x1004u;
		OBJECT(block)->sprite_frame = 0x74A0u;
		OBJECT(block)->sprite_palette = 0x9140u;
		OBJECT(block)->handler = icicle_update_ADDRESS;
		OBJECT(block)->hitbox_size_x = 0x28u;
		OBJECT(block)->hitbox_centre_x = 0;
		OBJECT(block)->hitbox_centre_y = -0x10;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->hitbox_height = 0xA0u;
		OBJECT(block)->flags = 0x40000000u;
		OBJECT(block)->kind = 0x28u;
		OBJECT(block)->speed_z = 1;
		OBJECT(block)->direction = OBJECT(current_object)->direction;
		OBJECT(block)->speed = 0;
		OBJECT(block)->icicle_update.dropper_flags = OBJECT(current_object)->drop_icicle_row.dropper_flags;
		OBJECT(block)->icicle_update.state = 0u;
		if (OBJECT(block)->icicle_update.rides_platform != 0u)
			OBJECT(block)->icicle_update.state = 4u;
		OBJECT(block)->x = OBJECT(current_object)->x;
		OBJECT(block)->y = OBJECT(current_object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->z_int -= 0xE0u;
		OBJECT(block)->floor_z = OBJECT(current_object)->z;
		direction = OBJECT(current_object)->direction;
		switch (direction >> 8)
		{
		case 0:
		case 2:
		case 4:
		case 5:
			OBJECT(block)->x_int += icicle_row_offset;
			break;
		case 1:
		case 3:
			OBJECT(block)->y_int += icicle_row_offset;
			break;
		default:
			invalid_jump(0x886F7u + (direction >> 8) * 2u);
		}
		icicle_row_offset += 0x48u;
		if (OBJECT(block)->icicle_update.rides_platform == 0u)
		{
			end_coordinate = (uint16_t)ICICLE_DROPPER_STEP(OBJECT(current_object)->drop_icicle_row.path)[-1].end_coordinate;
			line_coordinate = (uint16_t)end_coordinate;
			direction = OBJECT(current_object)->direction;
			switch (direction >> 8)
			{
			case 0:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->y_int) >= -0x40)
				{
					OBJECT(block)->y_int = line_coordinate + 0x20u;
					OBJECT(block)->flags |= 1u;
					OBJECT(current_object)->flags |= 1u;
					OBJECT(block)->icicle_update.line_coordinate = ICICLE_DROPPER_STEP(OBJECT(current_object)->drop_icicle_row.path)[-1].end_coordinate;
				}
				break;
			case 1:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->x_int) <= 0x40)
				{
					OBJECT(block)->x_int = line_coordinate - 0x20u;
					OBJECT(block)->flags |= 1u;
					OBJECT(current_object)->flags |= 1u;
					OBJECT(block)->icicle_update.line_coordinate = ICICLE_DROPPER_STEP(OBJECT(current_object)->drop_icicle_row.path)[-1].end_coordinate;
				}
				break;
			case 2:
			case 4:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->y_int) <= 0x40)
				{
					OBJECT(block)->y_int = line_coordinate - 0x20u;
					OBJECT(block)->flags |= 1u;
					OBJECT(current_object)->flags |= 1u;
					OBJECT(block)->icicle_update.line_coordinate = ICICLE_DROPPER_STEP(OBJECT(current_object)->drop_icicle_row.path)[-1].end_coordinate;
				}
				break;
			case 3:
				if ((int16_t)(end_coordinate - OBJECT(current_object)->x_int) >= -0x40)
				{
					OBJECT(block)->x_int = line_coordinate + 0x20u;
					OBJECT(block)->flags |= 1u;
					OBJECT(current_object)->flags |= 1u;
					OBJECT(block)->icicle_update.line_coordinate = ICICLE_DROPPER_STEP(OBJECT(current_object)->drop_icicle_row.path)[-1].end_coordinate;
				}
				break;
			default:
				invalid_jump(0x8873Cu + (direction >> 8) * 2u);
			}
		}
		next_random(&random_value, &ignored, &ignored_2);
		random_value = (random_value & ~0xFFFFu) | (random_value & 0x1Fu);
		random_value = (random_value & ~0xFFFFu) | (uint16_t)(random_value + 0x3Cu);
		OBJECT(block)->icicle_update.timer = random_value;
		object = block;
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			break;
		OBJECT(block)->sprite_flags = 0u;
		OBJECT(block)->sprite_palette = 0x1FFEu;
		OBJECT(block)->sprite_frame = 0x5D90u;
		OBJECT(block)->sprite_scale = 1u;
		OBJECT(block)->handler = icicle_shadow_follow_ADDRESS;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->speed_z = 0;
		OBJECT(block)->icicle_shadow.blink_timer_word = 0u;
		OBJECT(block)->parent = object;
		OBJECT(block)->x = OBJECT(object)->x;
		OBJECT(block)->y = OBJECT(object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->floor_z = OBJECT(current_object)->z;
		if (OBJECT(current_object)->drop_icicle_row.rides_platform != 0u)
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		icicles_left--;
	} while (icicles_left != 0u);
}

/* The icicle by state: waits its timer and falls; stabs in (sound 0x29, icicle_buried_part_draw, clips,
   icicle_cracks_spread), sinks 0x70 and set_icicle_wall_box. On the ice platform it aims where the
   platform will be and brings its piece down. Lined-up ones with counts_goal count goal_unlocked up. */
void icicle_update(void)
{
	uint8_t sound_id = 0;
	uint16_t child = 0, margin_x_2 = 0, margin_y = 0, projected_x = 0, swing_limit = 0;
	uint32_t depth = 0, direction = 0, floor_z = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, landing_offset = 0, margin_x = 0, object = 0, object_2 = 0, object_3 = 0, off_screen = 0, off_screen_or_clip_y = 0, parts_left = 0, platform_shift = 0, projected_y = 0, screen_y = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->icicle_update.state)
	{
	case 0:
	case 8:
		OBJECT(current_object)->icicle_update.state++;
		project_object(&ignored, &ignored_3);
		return;
	case 1:
		margin_x = 0x50u;
		margin_y = 0x140u;
		off_screen = is_off_screen_margin((uint16_t)margin_x, margin_y);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		timer = OBJECT(current_object)->icicle_update.timer;
		OBJECT(current_object)->icicle_update.timer = timer - 1u;
		if ((int16_t)timer <= 1)
		{
			OBJECT(current_object)->icicle_update.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
				OBJECT(current_object)->icicle_update.state++;
		}
		update_screen_position(&ignored, &margin_x);
		margin_x = queue_sprite(margin_x);
		return;
	case 2:
		if (free_object_stack < 2 || free_object_stack_2[0] < 2)
		{
			if ((uint16_t)(OBJECT(current_object)->screen_x + 8u) < 0x150u)
			{
				sound_id = 0x29u;
				play_sound(sound_id, &ignored_3, &ignored_2);
			}
			update_screen_position(&ignored, &ignored_3);
			add_to_touchable_list(ignored);
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
			OBJECT(current_object)->draw_depth = depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			allocate_object(&ignored, &object_2);
			OBJECT(object_2)->sprite_flags = OBJECT(current_object)->sprite_flags;
			OBJECT(object_2)->sprite_frame = OBJECT(current_object)->sprite_frame;
			OBJECT(object_2)->sprite_palette = OBJECT(current_object)->sprite_palette;
			OBJECT(object_2)->sprite_palette &= 0x1FFFu;
			OBJECT(object_2)->sprite_palette |= 0x2000u;
			OBJECT(object_2)->handler = icicle_buried_part_draw_ADDRESS;
			OBJECT(object_2)->x = OBJECT(current_object)->x;
			OBJECT(object_2)->y = OBJECT(current_object)->y;
			OBJECT(object_2)->floor_z = OBJECT(current_object)->floor_z;
			OBJECT(object_2)->parent = current_object;
			OBJECT(current_object)->child = object_2;
		}
		else
			object_2 = current_object;
		parts_left = 2u;
		do
		{
			object_3 = object_2;
			allocate_object_2(&ignored, &ignored_2, &object_2);
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
			parts_left--;
		} while (parts_left != 0u);
		allocate_object(&ignored, &object_2);
		OBJECT(object_2)->sprite_flags = 0x1004u;
		OBJECT(object_2)->sprite_palette = 0x9150u;
		OBJECT(object_2)->handler = icicle_cracks_spread_ADDRESS;
		OBJECT(object_2)->x = OBJECT(current_object)->x;
		OBJECT(object_2)->y = OBJECT(current_object)->y;
		OBJECT(object_2)->floor_z = OBJECT(current_object)->floor_z;
		OBJECT(object_2)->z = OBJECT(object_2)->floor_z;
		OBJECT(object_2)->flags = 0u;
		OBJECT(object_2)->sprite_offset_x = 0;
		OBJECT(object_2)->sprite_offset_y = 0;
		OBJECT(object_2)->parent = current_object;
		OBJECT(object_2)->animation = icicle_crack_animation_ADDRESS;
		OBJECT(object_2)->animation_frame = 0xFFu;
		OBJECT(object_2)->animation_speed = 0u;
		OBJECT(object_2)->animation_timer = 0;
		OBJECT(current_object)->icicle_update.state++;
		if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			if ((uint16_t)(OBJECT(current_object)->screen_x + 8u) < 0x150u)
			{
				sound_id = 0x29u;
				play_sound(sound_id, &ignored_3, &ignored_2);
			}
			update_screen_position(&ignored, &ignored_3);
			add_to_touchable_list(ignored);
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
			OBJECT(current_object)->draw_depth = depth;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->icicle_update.state = 7u;
		if ((uint16_t)(OBJECT(current_object)->screen_x + 8u) < 0x150u)
		{
			sound_id = 0x29u;
			play_sound(sound_id, &ignored_3, &ignored_2);
		}
		update_screen_position(&ignored, &ignored_3);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u)
		{
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x70)
			{
				OBJECT(current_object)->speed_z = 0;
				set_icicle_wall_box();
			}
		}
		if (map_screen_active != 0u)
		{
			delete_current_object();
			return;
		}
		margin_x_2 = 0x30u;
		margin_y = 0xA0u;
		off_screen = is_off_screen_margin(margin_x_2, margin_y);
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		update_screen_position(&ignored, &ignored_3);
		add_to_touchable_list(ignored);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x10u);
		if ((int16_t)depth < -0x10)
			depth = 0xFFF0u;
		if ((int16_t)depth > 0xEE)
			depth = 0xEEu;
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 4:
		OBJECT(current_object)->icicle_update.state++;
		object = LINKED(current_object, moving_floor_object);
		landing_offset = OBJECT(object)->speed_x * ((uint16_t)OBJECT(current_object)->icicle_update.timer + 0x19u);
		platform_shift = landing_offset;
		landing_offset = (uint16_t)(((int32_t)landing_offset >> 16) + OBJECT(object)->ice_platform_leader.travel);
		swing_limit = 0x87u;
		if ((int16_t)landing_offset <= 0x87)
		{
			swing_limit = (uint16_t)(0u - swing_limit);
			if ((int16_t)landing_offset < (int16_t)swing_limit)
				OBJECT(current_object)->x += platform_shift - ((uint32_t)(uint16_t)((landing_offset - swing_limit) * 2u) << 16);
			else
				OBJECT(current_object)->x += platform_shift;
			project_object(&ignored, &ignored_3);
			return;
		}
		OBJECT(current_object)->x += platform_shift - ((uint32_t)(uint16_t)((landing_offset - swing_limit) * 2u) << 16);
		project_object(&ignored, &ignored_3);
		return;
	case 5:
		if (OBJECT(current_object)->z_int >= 0x320)
		{
			delete_current_object();
			return;
		}
		timer_2 = OBJECT(current_object)->icicle_update.timer;
		OBJECT(current_object)->icicle_update.timer = timer_2 - 1u;
		if ((int16_t)timer_2 <= 1)
		{
			OBJECT(current_object)->icicle_update.timer = 0;
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((OBJECT(current_object)->flags & 8u) != 0u)
			{
				object = LINKED(current_object, OBJECT(current_object)->platform);
				if (OBJECT(object)->z_int <= OBJECT(current_object)->z_int)
				{
					OBJECT(current_object)->icicle_update.state++;
					OBJECT(current_object)->speed_z = 0;
					OBJECT(current_object)->sprite_palette |= 0x2000u;
					OBJECT(object)->spawn_arg_1_low = 0xFFu;
				}
			}
		}
		update_screen_position(&ignored, &ignored_3);
		add_to_rider_list();
		ignored_3 = queue_sprite_at_floor();
		return;
	case 6:
		off_screen = is_off_screen();
		if (off_screen != 0u)
		{
			delete_current_object();
			return;
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored, &ignored_3);
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->platform))->draw_depth + 1u);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 7:
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u)
		{
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x70)
			{
				OBJECT(current_object)->speed_z = 0;
				if (OBJECT(current_object)->icicle_update.counts_goal != 0u)
					goal_unlocked++;
				set_icicle_wall_box();
				direction = OBJECT(current_object)->direction;
				switch (direction >> 8)
				{
				case 0:
				case 4:
					OBJECT(current_object)->y_int = OBJECT(current_object)->icicle_update.line_coordinate;
					OBJECT(current_object)->hitbox_offset_y = 0;
					break;
				case 1:
					OBJECT(current_object)->x_int = OBJECT(current_object)->icicle_update.line_coordinate;
					break;
				case 2:
					OBJECT(current_object)->y_int = OBJECT(current_object)->icicle_update.line_coordinate;
					break;
				case 3:
					OBJECT(current_object)->x_int = OBJECT(current_object)->icicle_update.line_coordinate;
					OBJECT(current_object)->hitbox_offset_x = 0;
					break;
				default:
					invalid_jump(0x88BCBu + (direction >> 8) * 2u);
				}
			}
		}
		if (map_screen_active != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		margin_x_2 = 0x28u;
		margin_y = 0xA0u;
		off_screen_or_clip_y = is_off_screen_margin(margin_x_2, margin_y);
		if (off_screen_or_clip_y != 0u)
		{
			child = OBJECT(current_object)->child;
			free_object_2(child);
			child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
			free_object_2(child);
			delete_current_object();
			return;
		}
		update_screen_position(&ignored, &ignored_3);
		add_to_touchable_list(ignored);
		OBJECT(current_object)->draw_depth = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y;
		depth = OBJECT(current_object)->draw_depth;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
		depth = OBJECT(current_object)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		projected_x = (uint16_t)OBJECT(current_object)->projected_x;
		projected_y = OBJECT(current_object)->projected_y;
		floor_z = OBJECT(current_object)->floor_z;
		point_to_screen(projected_x, floor_z, &projected_y, &ignored_4, &screen_y);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->sprite_frame = screen_y;
		depth = OBJECT(object_cursor)->draw_depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x88840u + OBJECT(current_object)->icicle_update.state * 3u);
	}
}

/* The part of a stabbed icicle below the floor: a copy of it (palette | 0x2000) drawn over it after a
   clip entry whose top is the floor's screen y (project_point) and before one that resets the clip.
   Removed with them (free_object_2) off screen or once map_screen_active. */
void icicle_buried_part_draw(void)
{
	uint16_t child = 0, margin_x = 0, margin_y = 0;
	uint32_t depth = 0, floor_z = 0, ignored_2 = 0, ignored_3 = 0, object = 0, off_screen = 0, part_depth = 0, point_x = 0, point_y = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	if (map_screen_active != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	margin_x = 0x28u;
	margin_y = 0xA0u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		child = OBJECT(current_object)->child;
		free_object_2(child);
		child = OBJECT(LINKED(current_object, OBJECT(current_object)->child))->child;
		free_object_2(child);
		delete_current_object();
		return;
	}
	part_depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->draw_depth + 1u);
	buried_part_depth = part_depth;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite_at_depth(&ignored_2, &part_depth);
	current_object = object_cursor;
	part_depth = buried_part_depth;
	queue_sprite_at_depth(&ignored_2, &part_depth);
	point_x = OBJECT(current_object)->x;
	point_y = OBJECT(current_object)->y;
	floor_z = OBJECT(current_object)->floor_z;
	project_point(floor_z, &ignored_3, &point_x, &point_y);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xFFFu) | (point_y & 0xFFFu);
	depth = buried_part_depth;
	queue_sprite_at_depth(&ignored_2, &depth);
}

/* The cracks round a stabbed icicle (icicle_crack_animation around its anchor): one step each time the
   icicle sinks another 0x1C into the floor; drawn over it; removed off screen or once map_screen_active. */
void icicle_cracks_spread(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, object = 0, off_screen = 0;
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 1u;
	if ((flags & 1u) == 0u)
	{
		OBJECT(current_object)->icicle_cracks.next_depth = 0;
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->anchor_x = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->anchor_y = OBJECT(current_object)->projected_y_int;
		return;
	}
	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	if (OBJECT(current_object)->icicle_cracks.next_depth <= (int16_t)(OBJECT(object)->z_int - OBJECT(object)->floor_z_int))
	{
		animate_around_anchor();
		OBJECT(current_object)->icicle_cracks.next_depth += 0x1Cu;
	}
	update_screen_position(&ignored, &ignored_2);
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->draw_depth + 2u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* An icicle's shadow on the floor (sprite 0x5D90): grows (sprite_scale + 9 up to 0x21C) once the
   icicle's timer is 0x32 or less; removed off screen or when the icicle has sunk. On the ice platform it
   moves with it, blinks, and falls with a falling piece. */
void icicle_shadow_follow(void)
{
	uint32_t depth = 0, icicle = 0, ignored = 0, ignored_2 = 0, off_screen = 0;
	uint8_t blink_timer = 0;

	project_object(&ignored, &ignored_2);
	off_screen = is_off_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	icicle = LINKED(current_object, OBJECT(current_object)->parent);
	if ((OBJECT(icicle)->icicle_update.state == 3u || OBJECT(icicle)->icicle_update.state == 7u) && (uint32_t)OBJECT(icicle)->speed_z == 0u)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(icicle)->icicle_update.rides_platform != 0u)
	{
		OBJECT(current_object)->x += OBJECT(LINKED(current_object, moving_floor_object))->speed_x;
		if (OBJECT(icicle)->icicle_update.state == 6u)
		{
			OBJECT(current_object)->speed_z += 0x6000u;
			OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
			OBJECT(current_object)->sprite_palette |= 0x2000u;
		}
		OBJECT(current_object)->flags &= ~0x80000u;
		blink_timer = OBJECT(current_object)->icicle_shadow.blink_timer;
		OBJECT(current_object)->icicle_shadow.blink_timer = blink_timer - 1u;
		if ((int8_t)blink_timer <= 1)
		{
			OBJECT(current_object)->icicle_shadow.blink_timer_word = 2u;
			OBJECT(current_object)->flags |= 0x80000u;
		}
	}
	if (OBJECT(icicle)->icicle_update.timer <= 0x32 && (int16_t)OBJECT(current_object)->sprite_scale < 0x21C)
		OBJECT(current_object)->sprite_scale += 9u;
	if ((OBJECT(current_object)->flags & 0x80000u) != 0u)
		return;
	icicle = LINKED(current_object, OBJECT(current_object)->parent);
	if (OBJECT(icicle)->icicle_update.state == 6u)
		depth = OBJECT(icicle)->draw_depth;
	else
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* The sunk icicle becomes a wall: a box hitbox (flags bit 1) 0x70 x 0x80 or 0x80 x 0x70 across its
   direction, with the offsets for that direction. */
void set_icicle_wall_box(void)
{
	uint32_t direction = 0;

	OBJECT(current_object)->flags |= 2u;
	direction = OBJECT(current_object)->direction;
	switch (direction >> 8)
	{
	case 0:
		OBJECT(current_object)->hitbox_size_x = 0x70u;
		OBJECT(current_object)->hitbox_size_y = 0x80u;
		OBJECT(current_object)->hitbox_offset_x = -0x20;
		OBJECT(current_object)->hitbox_offset_y = -0x40;
		break;
	case 1:
		OBJECT(current_object)->hitbox_size_x = 0x80u;
		OBJECT(current_object)->hitbox_size_y = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = -0x40;
		OBJECT(current_object)->hitbox_offset_y = -0x40;
		break;
	case 2:
	case 4:
		OBJECT(current_object)->hitbox_size_x = 0x70u;
		OBJECT(current_object)->hitbox_size_y = 0x80u;
		OBJECT(current_object)->hitbox_offset_x = -0x20;
		OBJECT(current_object)->hitbox_offset_y = -0x7F;
		break;
	case 3:
		OBJECT(current_object)->hitbox_size_x = 0x80u;
		OBJECT(current_object)->hitbox_size_y = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = 0;
		OBJECT(current_object)->hitbox_offset_y = -0x20;
		break;
	default:
		invalid_jump(0x88EBFu + (direction >> 8) * 2u);
	}
}
