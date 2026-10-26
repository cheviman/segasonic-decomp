/**
 *    Name:        objects/stage_03.c
 *    Description: stage 3's objects: the spiked panels and the electric poles, the crate pushed to a post, the breakable panel, the spiked roller that chases the players (also in stage E) and the floor that crumbles at the finale, the machine at 8300 with its fire and fireballs, the fire vents, the end pillar
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* The spiked panel and four spawned parts: 1-2 (kind 0xC, spike_panel_part_idle), 3 a hitbox 0x8D x 0xBC
   (kind 0x12, spike_panel_hitbox_update), 4 the plate 0x6B58 (spike_panel_base_draw), cut off the chain.
   The head: step = start_step, step_timer = spawn_arg_2, trigger_delay 0x1E, spike_panel_hold. */
void spike_panel_init(void)
{
	uint32_t block = 0, first = 0, object = 0, parts_left = 0, spawned = 0;

	spawn_count = 4u;
	spawn_objects(0x00F092u, &spawned, &first, &block);
	if ((uint8_t)spawned == 0u)
		return;
	OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
	OBJECT(current_object)->spike_panel.step_timer = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spike_panel.trigger_delay = 0x1Eu;
	OBJECT(current_object)->child = first;
	OBJECT(current_object)->sprite_palette = 0x9020u;
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_offset_y = -0x16;
	OBJECT(current_object)->hitbox_height = 0x3Cu;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->handler = spike_panel_hold_ADDRESS;
	parts_left = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->floor_z = OBJECT(object_cursor)->floor_z;
		OBJECT(current_object)->sprite_palette = 0x9020u;
		OBJECT(current_object)->flags |= 2u;
		OBJECT(current_object)->hitbox_offset_y = -0x1A;
		OBJECT(current_object)->hitbox_height = 0x3Cu;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spike_panel_part_idle_ADDRESS;
		parts_left--;
	} while (parts_left != 0u);
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
	OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
	OBJECT(current_object)->projected_x -= 0x7Bu;
	OBJECT(current_object)->projected_y_int -= 0x3Cu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x8Du;
	OBJECT(current_object)->hitbox_size_y = 0xBCu;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = 0;
	OBJECT(current_object)->hitbox_height = 0x3Cu;
	OBJECT(current_object)->kind = 0x12u;
	OBJECT(current_object)->handler = spike_panel_hitbox_update_ADDRESS;
	object = LINKED(first, OBJECT(current_object)->child);
	OBJECT(current_object)->child = 0xFFFFu;
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x7Cu;
	OBJECT(object)->projected_y_int += 0x38u;
	OBJECT(object)->sprite_frame = 0x6B58u;
	OBJECT(object)->sprite_palette = 0x11E0u;
	OBJECT(object)->handler = spike_panel_base_draw_ADDRESS;
}

/* Holds the panel at start_step while no player is within 0x40 in x (find_nearest_player_x); with one
   there, or from the start when start_step is 0, trigger_delay counts down, then start_step = 0 and
   spike_panel_update takes over. Runs spike_panel_update. */
void spike_panel_hold(void)
{
	uint16_t trigger_delay = 0;

	if (OBJECT(current_object)->spike_panel.start_step == 0u)
	{
		trigger_delay = OBJECT(current_object)->spike_panel.trigger_delay;
		OBJECT(current_object)->spike_panel.trigger_delay = trigger_delay - 1u;
		if (trigger_delay == 0u)
		{
			OBJECT(current_object)->spike_panel.start_step = 0u;
			OBJECT(current_object)->handler = spike_panel_update_ADDRESS;
			spike_panel_update();
			return;
		}
		OBJECT(current_object)->spike_panel.step_timer = 0;
		OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
		spike_panel_update();
		return;
	}
	find_nearest_player_x();
	if (nearest_player_distance > 0x40)
	{
		OBJECT(current_object)->spike_panel.step_timer = 0;
		OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
		spike_panel_update();
		return;
	}
	trigger_delay = OBJECT(current_object)->spike_panel.trigger_delay;
	OBJECT(current_object)->spike_panel.trigger_delay = trigger_delay - 1u;
	if (trigger_delay == 0u)
	{
		OBJECT(current_object)->spike_panel.start_step = 0u;
		OBJECT(current_object)->handler = spike_panel_update_ADDRESS;
		spike_panel_update();
		return;
	}
	OBJECT(current_object)->spike_panel.step_timer = 0;
	OBJECT(current_object)->spike_panel.step = OBJECT(current_object)->spike_panel.start_step;
	spike_panel_update();
}

/* With chaser_object at a direction between 0x100 and 0x378 the panel is removed in 0xF debris; else it
   steps spike_panel_frame_sequence (spike_panel_step_durations) and places, sizes and draws the head and
   parts 1-2 by the frame. Sound 0x25 sits where frame_index is under 3: it never plays. */
void spike_panel_update(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint16_t count = 0;
	uint32_t chaser_distance = 0, cursor = 0, deleted = 0, depth = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, offset_record = 0, part_2_frame = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &cursor);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	r14 = LINKED(WORK_RAM, chaser_object);
	chaser_distance = face_object();
	if ((int16_t)OBJECT(current_object)->direction <= 0x100 || (int16_t)OBJECT(current_object)->direction >= 0x378)
	{
		OBJECT(current_object)->spike_panel.step_timer--;
		if (OBJECT(current_object)->spike_panel.step_timer < 0 && OBJECT(current_object)->spike_panel.start_step == 0u)
		{
			OBJECT(current_object)->spike_panel.step++;
			OBJECT(current_object)->spike_panel.step_timer = spike_panel_step_durations[(uint8_t)OBJECT(current_object)->spike_panel.step];
			if (OBJECT(current_object)->spike_panel.step > 8)
			{
				OBJECT(current_object)->spike_panel.step = 0;
				OBJECT(current_object)->spike_panel.step_timer = OBJECT(current_object)->spawn_arg_2;
			}
		}
		frame_index = spike_panel_frame_sequence[(uint8_t)OBJECT(current_object)->spike_panel.step];
		OBJECT(current_object)->spike_panel.frame_index = frame_index;
		offset_record = spike_panel_head_offsets_ADDRESS + frame_index * 4u;
		OBJECT(current_object)->screen_x += SPIKE_PANEL_SCREEN_OFFSET(offset_record)->dx;
		OBJECT(current_object)->screen_y += SPIKE_PANEL_SCREEN_OFFSET(offset_record)->dy;
		OBJECT(current_object)->sprite_frame = spike_panel_head_sprites[frame_index];
		if ((int8_t)frame_index >= 5)
		{
			OBJECT(current_object)->hitbox_size_x = 0x65u;
			OBJECT(current_object)->hitbox_size_y = 0x30u;
			OBJECT(current_object)->hitbox_offset_x = -0x11;
		}
		else
		{
			OBJECT(current_object)->hitbox_size_x = 0x54u;
			OBJECT(current_object)->hitbox_size_y = 0x30u;
			OBJECT(current_object)->hitbox_offset_x = -1;
		}
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
		queue_sprite_at_depth(&ignored, &depth);
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(ignored);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction = 0x300u;
		OBJECT(current_object)->projected_x = OBJECT(object_cursor)->projected_x;
		OBJECT(current_object)->projected_y = OBJECT(object_cursor)->projected_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		frame_index = (uint8_t)OBJECT(object_cursor)->spike_panel.frame_index;
		offset_record = spike_panel_part_1_offsets_ADDRESS + (frame_index * 2u * 2u + frame_index + frame_index);
		OBJECT(current_object)->projected_x += SPIKE_PANEL_PART_OFFSET(offset_record)->dx;
		offset_record += 2u;
		OBJECT(current_object)->projected_y_int += SPIKE_PANEL_PART_OFFSET(offset_record - 0x2u)->dy;
		OBJECT(current_object)->z_int += SPIKE_PANEL_PART_OFFSET(offset_record - 0x2u)->dz;
		OBJECT(current_object)->sprite_flags = spike_panel_part_1_sprite_flags[frame_index];
		OBJECT(current_object)->sprite_frame = spike_panel_part_1_sprites[frame_index];
		unproject_object(&ignored, &ignored_2, &ignored_3, &cursor);
		if (OBJECT(object_cursor)->spike_panel.frame_index >= 3)
			queue_sprite(offset_record);
		else if ((uint8_t)OBJECT(object_cursor)->spike_panel.frame_index != 5u)
			queue_sprite_at_previous_floor_2();
		else
		{
			sound_id = 0x25u;
			play_sound(sound_id, &ignored_4, &ignored_2);
			bucket = 0u;
			queue_sprite_in_bucket(bucket);
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_5);
		part_2_frame = (uint8_t)OBJECT(object_cursor)->spike_panel.frame_index;
		offset_record = spike_panel_part_2_offsets_ADDRESS + part_2_frame * 4u;
		OBJECT(current_object)->sprite_frame = spike_panel_part_2_sprites[part_2_frame];
		if (part_2_frame != 3u)
		{
			OBJECT(current_object)->screen_x += SPIKE_PANEL_SCREEN_OFFSET(offset_record)->dx;
			OBJECT(current_object)->screen_y += SPIKE_PANEL_SCREEN_OFFSET(offset_record)->dy;
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xFu);
			queue_sprite_at_depth(&part_2_frame, &depth);
		}
		if ((int8_t)part_2_frame >= 5)
		{
			OBJECT(current_object)->hitbox_size_x = 0x59u;
			OBJECT(current_object)->hitbox_size_y = 0x2Au;
			OBJECT(current_object)->hitbox_offset_x = -0x14;
		}
		else
		{
			OBJECT(current_object)->hitbox_size_x = 0x4Bu;
			OBJECT(current_object)->hitbox_size_y = 0x2Au;
			OBJECT(current_object)->hitbox_offset_x = -5;
		}
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(part_2_frame);
		return;
	}
	count = 0xFu;
	OBJECT(current_object)->projected_x -= 0x90u;
	OBJECT(current_object)->projected_y_int -= 0x40u;
	spawn_debris(chaser_distance, count, cursor);
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* Part 4, the plate with the holes: drawn in bucket 0xFF, behind everything, until it is removed off
   screen. */
void spike_panel_base_draw(void)
{
	uint8_t bucket = 0;
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 1u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		return;
	}
}

/* Part 3, touchable only while the panel's frame_index is 4 or more: at part 1's place 0xD8 back in
   projected x, kind 0x12, or 0xC from the panel's step 5. */
void spike_panel_hitbox_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	if (OBJECT(current_object)->spike_panel.frame_index < 4)
		return;
	OBJECT(object_cursor)->kind = 0x12u;
	if (OBJECT(current_object)->spike_panel.step >= 5)
		OBJECT(object_cursor)->kind = 0xCu;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	OBJECT(object_cursor)->floor_z = OBJECT(current_object)->floor_z;
	OBJECT(object_cursor)->projected_x -= 0xD8u;
	current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
}

/* The handler of parts 1 and 2: does nothing, as spike_panel_update moves and draws them. */
void spike_panel_part_idle(void)
{
}

/* The electric pole, closed (sprite 0x67D8): hitbox_size_x 0x19, height 0x53, health 4 (against
   attacks), kind 5, electric_pole_wait; its three spark pieces get palette 0x83D0, the third a step
   ahead. */
void electric_pole_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x67D8u;
	OBJECT(current_object)->sprite_palette = 0x87F0u;
	OBJECT(current_object)->hitbox_size_x = 0x19u;
	OBJECT(current_object)->hitbox_height = 0x53u;
	OBJECT(current_object)->health = 4;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = electric_pole_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x83D0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6850u;
	OBJECT(current_object)->sprite_palette = 0x83D0u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction = 0x401u;
	OBJECT(current_object)->sprite_frame = 0x6850u;
	OBJECT(current_object)->sprite_palette = 0x83D0u;
}

/* Waits closed (electric_pole_update) until the nearest player (face_nearest_player) is within 0x70,
   then electric_pole_rise. */
void electric_pole_wait(void)
{
	face_nearest_player(0u, 0u);
	if (nearest_player_distance > 0x70)
	{
		electric_pole_update();
		return;
	}
	OBJECT(current_object)->handler = electric_pole_rise_ADDRESS;
	electric_pole_rise();
}

/* Every other frame the next of electric_pole_rise_frames (through electric_pole_sprites); at step 0xA
   it is up: projected_y_int - 0xB (draw_offset_y 0xB puts the sprite back) and electric_pole_update
   takes over. Runs electric_pole_update. */
void electric_pole_rise(void)
{
	uint16_t odd_frame = 0;

	OBJECT(current_object)->electric_pole.frame_toggle++;
	odd_frame = OBJECT(current_object)->electric_pole.frame_toggle & 1u;
	OBJECT(current_object)->electric_pole.frame_toggle = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->electric_pole.frame_index++;
		OBJECT(current_object)->sprite_frame = electric_pole_sprites[electric_pole_rise_frames[(uint8_t)OBJECT(current_object)->electric_pole.frame_index]];
		if (OBJECT(current_object)->electric_pole.frame_index >= 0xA)
		{
			OBJECT(current_object)->projected_y_int -= 0xBu;
			OBJECT(current_object)->electric_pole.draw_offset_y = 0xBu;
			OBJECT(current_object)->handler = electric_pole_update_ADDRESS;
			electric_pole_update();
			return;
		}
	}
	electric_pole_update();
}

/* Explodes (sprite_scale 0x200, explosion_animate, sound 7) when chaser_object comes within 0xF in x or
   it is touched with health under 4; touched with health 4 or more it becomes kind 0x15. Then
   electric_pole_draw. */
void electric_pole_update(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &ignored_2);
	if (distance_to_player_1 <= 0xF)
	{
		OBJECT(current_object)->sprite_scale = 0x200u;
		OBJECT(current_object)->projected_y_int += 0xBu;
		OBJECT(current_object)->electric_pole.frame_index = 0;
		OBJECT(current_object)->electric_pole.last_frame = 0xF;
		OBJECT(current_object)->sprite_palette = 0x9000u;
		OBJECT(current_object)->handler = explosion_animate_ADDRESS;
		OBJECT(current_object)->electric_pole.frame_toggle = 0u;
		sound_id = 7u;
		play_sound(sound_id, &ignored_2, &ignored_3);
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		electric_pole_draw();
		return;
	}
	if (OBJECT(current_object)->health >= 4)
	{
		OBJECT(current_object)->kind = 0x15u;
		electric_pole_draw();
		return;
	}
	OBJECT(current_object)->sprite_scale = 0x200u;
	OBJECT(current_object)->projected_y_int += 0xBu;
	OBJECT(current_object)->electric_pole.frame_index = 0;
	OBJECT(current_object)->electric_pole.last_frame = 0xF;
	OBJECT(current_object)->sprite_palette = 0x9000u;
	OBJECT(current_object)->handler = explosion_animate_ADDRESS;
	OBJECT(current_object)->electric_pole.frame_toggle = 0u;
	sound_id = 7u;
	play_sound(sound_id, &ignored_2, &ignored_3);
}

/* Drawn draw_offset_y lower unless removed off screen. Rising (frame_index under 0xA) it shows the
   charging spark and is touchable from frame 2 on; risen, it is touchable and the front spark and two
   side sparks step through their 7 frames every other frame. */
void electric_pole_draw(void)
{
	uint32_t charging_spark_y = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, spark_y = 0;
	uint8_t front_odd_frame = 0, side_odd_frame = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->electric_pole.draw_offset_y;
	queue_sprite(0u);
	if (OBJECT(object_cursor)->electric_pole.frame_index_word < 0xA)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if (OBJECT(object_cursor)->electric_pole.frame_index < 2)
			return;
		OBJECT(current_object)->sprite_frame = electric_pole_side_spark_sprites[(uint8_t)OBJECT(object_cursor)->electric_pole.frame_index - 2u];
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		charging_spark_y = (uint16_t)OBJECT(object_cursor)->screen_y;
		OBJECT(current_object)->screen_y = charging_spark_y;
		charging_spark_y++;
		queue_sprite_at_depth(&ignored, &charging_spark_y);
		current_object = object_cursor;
		add_to_touchable_list(ignored);
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->direction_high++;
	front_odd_frame = OBJECT(current_object)->direction_high & 1u;
	OBJECT(current_object)->direction_high = front_odd_frame;
	if (front_odd_frame == 0u)
	{
		OBJECT(current_object)->direction_low++;
		if ((int8_t)OBJECT(current_object)->direction_low > 6)
			OBJECT(current_object)->direction = 0u;
		OBJECT(current_object)->sprite_frame = electric_pole_front_spark_sprites[OBJECT(current_object)->direction_low];
	}
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	spark_y = (uint16_t)(OBJECT(object_cursor)->screen_y - 0xAu);
	OBJECT(current_object)->screen_y = spark_y;
	spark_y += 0xBu;
	queue_sprite_at_depth(&ignored, &spark_y);
	r14 = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->direction_high++;
		side_odd_frame = OBJECT(current_object)->direction_high & 1u;
		OBJECT(current_object)->direction_high = side_odd_frame;
		if (side_odd_frame == 0u)
		{
			OBJECT(current_object)->direction_low++;
			if ((int8_t)OBJECT(current_object)->direction_low > 6)
				OBJECT(current_object)->direction = 0u;
			OBJECT(current_object)->sprite_frame = electric_pole_side_spark_sprites[OBJECT(current_object)->direction_low];
		}
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x += electric_pole_side_spark_offsets_x[r14 - 1u];
		spark_y = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x10u);
		OBJECT(current_object)->screen_y = spark_y;
		spark_y += 0x11u;
		queue_sprite_at_depth(&ignored, &spark_y);
		r14 = r14 - 1u;
	} while (r14 != 0u);
	current_object = object_cursor;
	add_to_touchable_list(ignored);
}

/* The crate (0x6968, box hitbox, kind 0x20, push_crate_update) and its stop post with a top piece
   (spawn_object_with_pieces 2, push_crate_stop_post_update), set back post_distance * (0x22, 0x11) +
   (0x11, 0x1D). child, which held the crate's own piece, now points at the post. */
void push_crate_init(void)
{
	uint16_t post_distance = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->sprite_frame = 0x6968u;
	OBJECT(current_object)->sprite_palette = 0x9D0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x4Bu;
	OBJECT(current_object)->hitbox_offset_x = -9;
	OBJECT(current_object)->hitbox_offset_y = -0x42;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x20u;
	OBJECT(current_object)->push_crate.frame_count = 0x12Cu;
	OBJECT(current_object)->handler = push_crate_update_ADDRESS;
	spawn_count = 2u;
	spawn_object_with_pieces(0x011203u, &ignored, &ignored_2, &free_count, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	post_distance = OBJECT(object_cursor)->push_crate.post_distance;
	OBJECT(object)->projected_x -= post_distance * 0x22u + 0x11u;
	OBJECT(object)->projected_y_int -= post_distance * 0x11u + 0x1Du;
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
	OBJECT(object)->handler = push_crate_stop_post_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x73B0u;
	OBJECT(object)->sprite_palette = 0x89E0u;
}

/* chaser_object within 3 in y, or a touch by an attacking object (flag 0x40): push_crate_break. Touched
   by an object whose (direction + 0x40) / 0x40 - 1 equals push_quadrant, unless stopped: speed along
   push_crate_push_angles[push_quadrant] at the toucher's speed. Then push_crate_move. */
void push_crate_update(void)
{
	uint32_t direction = 0, distance_y = 0, ignored = 0, object = 0, speed = 0, speed_x = 0, speed_y = 0;

	r14 = LINKED(WORK_RAM, chaser_object);
	distance_xy_to_object(&ignored, &distance_y);
	if ((int16_t)distance_y <= 3)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if ((OBJECT(current_object)->flags & 4u) == 0u)
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->touched_by);
	if ((OBJECT(object)->flags & 0x40u) != 0u)
	{
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		OBJECT(current_object)->handler = push_crate_break_ADDRESS;
		push_crate_break();
		return;
	}
	if (OBJECT(current_object)->push_crate.stopped != 0u || OBJECT(current_object)->push_crate.push_quadrant != (uint16_t)((int16_t)(OBJECT(object)->direction + 0x40u) / 0x40 - 1u))
	{
		push_crate_move(0u, 0u, 0u);
		return;
	}
	direction = push_crate_push_angles[OBJECT(current_object)->push_crate.push_quadrant];
	speed = (uint32_t)OBJECT(object)->speed >> 16;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	push_crate_move(0u, 0u, 0u);
}

/* Every other frame the next of push_crate_break_sprites; at the last (2) push_crate_move takes over,
   after push_crate_release_all_rings when ring_count is above 0. Then push_crate_move. */
void push_crate_break(void)
{
	uint32_t allocated = 0, break_frame = 0, random_value = 0, ring = 0;
	uint16_t odd_frame = 0;

	OBJECT(current_object)->push_crate.frame_count++;
	odd_frame = OBJECT(current_object)->push_crate.frame_count & 1u;
	OBJECT(current_object)->push_crate.frame_count = odd_frame;
	if (odd_frame == 0u)
	{
		OBJECT(current_object)->push_crate.break_frame++;
		break_frame = OBJECT(current_object)->push_crate.break_frame;
		OBJECT(current_object)->sprite_frame = push_crate_break_sprites[break_frame];
		if (break_frame == 2u)
		{
			OBJECT(current_object)->handler = push_crate_move_ADDRESS;
			if (OBJECT(current_object)->push_crate.ring_count > 0)
			{
				push_crate_release_all_rings(&allocated, &random_value, &ring);
				push_crate_move(allocated, random_value, ring);
				return;
			}
		}
	}
	push_crate_move(0u, 0u, 0u);
}

/* Moves, removed off screen. Whole: touchable, drawn at screen_y - 0xD, and while speed_x is not 0 a
   ring (push_crate_release_rings) each time ring_timer runs out (reloaded with 0x10) while ring_count
   lasts. Broken: drawn in bucket 0xFF, under everything. */
void push_crate_move(uint32_t cursor, uint32_t random_value, uint32_t ring)
{
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, slope_height = 0;
	uint16_t ring_delay = 0;

	move_object_projected(&ignored, &ignored_2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &ignored_2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->push_crate.break_frame != 2u)
	{
		OBJECT(current_object)->health = 0x7F;
		add_to_touchable_list(deleted);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0xDu);
		queue_sprite_at_depth(&ignored, &depth);
		if ((uint32_t)OBJECT(current_object)->speed_x != 0u)
		{
			ring_delay = OBJECT(current_object)->push_crate.ring_timer - 1u;
			OBJECT(current_object)->push_crate.ring_timer = ring_delay;
			if ((int16_t)ring_delay < 0 && OBJECT(current_object)->push_crate.ring_count > 0)
			{
				OBJECT(current_object)->push_crate.ring_count--;
				OBJECT(current_object)->push_crate.ring_timer = 0x10;
				rings_to_release = 1u;
				push_crate_release_rings(slope_height, &cursor, &random_value, &ring);
			}
		}
	}
	else
	{
		deleted = (uint16_t)(deleted + 0xFFu);
		queue_sprite_in_bucket(deleted);
	}
}

/* The stop post: when its crate (parent) comes within 0x33 in y, the crate stops for good (stopped,
   speeds 0). Draws the post (queue_sprite) and its top piece 0xF higher, at screen_y + 0x11. */
void push_crate_stop_post_update(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	r14 = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	distance_xy_to_object(&ignored, &ignored_5);
	if (distance_to_player_2 <= 0x33)
	{
		OBJECT(r14)->push_crate.stopped = 1u;
		OBJECT(r14)->speed_x = 0;
		OBJECT(r14)->speed_y = 0;
	}
	queue_sprite(ignored_5);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0xFFF1u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x11u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* rings_to_release = ring_count, then push_crate_release_rings: every ring the crate still holds. */
void push_crate_release_all_rings(uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	rings_to_release = OBJECT(current_object)->push_crate.ring_count;
	push_crate_release_rings(0u, p_allocated, p_random_value, p_ring);
}

/* drop_rings rings_to_release rings behind the crate: released_ring_angle =
   push_crate_push_angles[(push_quadrant + 2) & 3], no flip, no random spread. */
void push_crate_release_rings(uint32_t seed, uint32_t *p_allocated, uint32_t *p_random_value, uint32_t *p_ring)
{
	uint32_t opposite_quadrant = 0;

	opposite_quadrant = OBJECT(current_object)->push_crate.push_quadrant;
	opposite_quadrant = (uint8_t)(opposite_quadrant + 2u);
	released_ring_angle = push_crate_push_angles[opposite_quadrant & 3u];
	released_ring_flip = 0u;
	released_rings_random = 0u;
	drop_rings(seed, p_allocated, p_random_value, p_ring);
}

/* The blue stone panel (sprite 0x9278, palette 0x91F0): box 7 x 0x40, height 0x64, kind 5,
   breakable_panel_wait. Its piece is the arch outline (0x9308) whose palette draw_breakable_panel
   cycles. */
void breakable_panel_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x9278u;
	OBJECT(current_object)->sprite_palette = 0x91F0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 7u;
	OBJECT(current_object)->hitbox_size_y = 0x40u;
	OBJECT(current_object)->hitbox_offset_x = -4;
	OBJECT(current_object)->hitbox_offset_y = -0x28;
	OBJECT(current_object)->hitbox_height = 0x64u;
	OBJECT(current_object)->kind = 5u;
	OBJECT(current_object)->handler = breakable_panel_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x9308u;
	OBJECT(current_object)->sprite_palette = 0x9200u;
}

/* Once touched by an attacking object (flag 0x40 of touched_by): drop_rings spawn_arg_2 rings at angle
   spawn_arg_1, spread at random, and breakable_panel_break. Until then draw_breakable_panel. */
void breakable_panel_wait(void)
{
	uint32_t allocated = 0, random_value = 0, ring = 0;

	if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
	{
		draw_breakable_panel();
		return;
	}
	released_ring_flip = 0u;
	released_rings_random = 1u;
	released_ring_angle = OBJECT(current_object)->spawn_arg_1;
	rings_to_release = OBJECT(current_object)->spawn_arg_2;
	drop_rings(0u, &allocated, &random_value, &ring);
	OBJECT(current_object)->handler = breakable_panel_break_ADDRESS;
	breakable_panel_break();
}

/* Plays breakable_panel_break_frames 0-0x10, one every 3 frames (frame_timer), as the panel cracks open
   to an arched doorway; then only draw_breakable_panel. Drawn every frame. */
void breakable_panel_break(void)
{
	uint16_t frame_timer = 0;

	frame_timer = OBJECT(current_object)->breakable_panel.frame_timer;
	OBJECT(current_object)->breakable_panel.frame_timer = frame_timer - 1u;
	if (frame_timer == 0u)
	{
		OBJECT(current_object)->sprite_frame = breakable_panel_break_frames[(uint8_t)OBJECT(current_object)->breakable_panel.frame_index];
		OBJECT(current_object)->breakable_panel.frame_timer = 2u;
		OBJECT(current_object)->breakable_panel.frame_index++;
		if (OBJECT(current_object)->breakable_panel.frame_index > 0x10)
		{
			OBJECT(current_object)->handler = draw_breakable_panel_ADDRESS;
			draw_breakable_panel();
			return;
		}
	}
	draw_breakable_panel();
}

/* Drawn at screen_y - 0x20 and touchable, even open; removed off screen. Unbroken, its outline piece is
   drawn at screen_y - 0x19 and its palette steps through breakable_panel_glow_steps every 3 frames,
   pausing midway and at the end. */
void draw_breakable_panel(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t direction = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	if ((uint8_t)OBJECT(current_object)->draw_breakable_panel.frame_index != 0u)
		return;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		offset_x = 0u;
		offset_y = 0u;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x19u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->sprite_palette = breakable_panel_glow_palettes[breakable_panel_glow_steps[(uint8_t)OBJECT(current_object)->piece_frame]];
	OBJECT(current_object)->direction = 2u;
	OBJECT(current_object)->piece_frame++;
	if ((uint8_t)OBJECT(current_object)->piece_frame == 0xFu)
		OBJECT(current_object)->direction = 0xFu;
	if (OBJECT(current_object)->piece_frame <= 0x1C)
	{
		offset_x = 0u;
		offset_y = 0u;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x19u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	OBJECT(current_object)->direction = 0x1Eu;
	OBJECT(current_object)->piece_frame = 0;
	offset_x = 0u;
	offset_y = 0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x19u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* A stone floor block of the stage 3 finale: its kind (sprite_frame, kept in speed_x_int) gives from
   stage_03_floor_block_kinds its sprite offsets, a depth offset (speed_y_fraction), the animation and
   which stage_03_floor_block_falls byte it waits for (speed_x_fraction). Clears both bytes; z 0. */
void stage_03_floor_block_start(void)
{
	uint32_t block_kind = 0, ignored = 0, ignored_2 = 0, kind_entry = 0, kind_record = 0;

	block_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->speed_x_int = block_kind;
	kind_entry = stage_03_floor_block_kinds_ADDRESS + block_kind * 4u;
	kind_record = STAGE_03_FLOOR_BLOCK_KIND(kind_entry)->shape + stage_03_floor_block_kinds_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = STAGE_03_FLOOR_BLOCK_SHAPE(kind_record)->sprite_offset_x;
	kind_record++;
	OBJECT(current_object)->sprite_offset_y = STAGE_03_FLOOR_BLOCK_SHAPE(kind_record - 0x1u)->sprite_offset_y;
	kind_record++;
	OBJECT(current_object)->speed_y_fraction = STAGE_03_FLOOR_BLOCK_SHAPE(kind_record - 0x2u)->depth_offset;
	OBJECT(current_object)->animation = kind_record + 2u;
	OBJECT(current_object)->speed_x_fraction = STAGE_03_FLOOR_BLOCK_KIND(kind_entry)->fall_flag;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9040u;
	OBJECT(current_object)->handler = stage_03_floor_block_update_ADDRESS;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->flags = 2u;
	stage_03_floor_block_falls[0] = 0u;
	stage_03_floor_block_falls_1 = 0u;
	OBJECT(current_object)->sprite_frame = STAGE_03_FLOOR_BLOCK_ANIMATION(OBJECT(current_object)->animation)->first_frame;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = 0;
	project_object(&ignored, &ignored_2);
}

/* Waits until its stage_03_floor_block_falls byte is set, then falls (speed_z + 0x1000 a frame), kinds
   0-3 dropping their sprite offsets, and plays its crumble animation once. Drawn at screen_y - the depth
   offset - sprite_offset_y; removed below screen y 0x158. */
void stage_03_floor_block_update(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if (stage_03_floor_block_falls[OBJECT(current_object)->speed_x_fraction] == 0u)
	{
		update_screen_position(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0x158)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
			queue_sprite_at_depth(&ignored_3, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z += 0x1000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->speed_x_int < 4)
	{
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed < 0)
	{
		update_screen_position(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0x158)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
			queue_sprite_at_depth(&ignored_3, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored_3, &ignored);
		if (OBJECT(current_object)->screen_y < 0x158)
		{
			depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
			queue_sprite_at_depth(&ignored_3, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->animation_speed = 0xFFu;
	update_screen_position(&ignored_3, &ignored);
	if (OBJECT(current_object)->screen_y < 0x158)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->speed_y_fraction - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* Builds a spiked roller of kind sprite_frame (spiked_roller_kinds): its parts side by side (spacing and
   part record from its layout) in state 2 behind the leader, which takes the first state (0 sets
   players_frozen, 0xE hides it at z 0x50), the script, the back zone and chaser_object if free. */
void spiked_roller_start(void)
{
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, layout_cursor = 0, object = 0, record_cursor = 0, roller_kind = 0, zone_index = 0;
	uint8_t parts_left = 0;

	roller_kind = OBJECT(current_object)->sprite_frame;
	OBJECT(current_object)->spiked_roller.roller_kind = roller_kind;
	OBJECT(current_object)->spiked_roller.event_offset = spiked_roller_events[roller_kind];
	record_cursor = spiked_roller_kinds_ADDRESS + spiked_roller_kinds[roller_kind];
	roller_parts_left = SPIKED_ROLLER_KIND(record_cursor)->part_count;
	record_cursor++;
	layout_cursor = SPIKED_ROLLER_KIND(record_cursor - 0x1u)->layout;
	record_cursor += 2u;
	roller_first_state = SPIKED_ROLLER_KIND(record_cursor - 0x3u)->first_state;
	record_cursor += 2u;
	roller_back_zone_index = SPIKED_ROLLER_KIND(record_cursor - 0x5u)->back_zone;
	OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
	layout_cursor += spiked_roller_layouts_ADDRESS;
	roller_step_x = (uint32_t)SPIKED_ROLLER_LAYOUT(layout_cursor)->step_x << 16;
	layout_cursor++;
	roller_step_y = (uint32_t)SPIKED_ROLLER_LAYOUT(layout_cursor - 0x1u)->step_y << 16;
	layout_cursor++;
	roller_part_record = layout_cursor;
	OBJECT(current_object)->flags = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &layout_cursor);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->spiked_roller.screen_threshold = 0;
	OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
	OBJECT(current_object)->spiked_roller.base_speed = 0;
	OBJECT(current_object)->spiked_roller.wait_timer = 0;
	OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
	OBJECT(current_object)->spiked_roller.substate = 0u;
	OBJECT(current_object)->player = 0u;
	OBJECT(current_object)->spiked_roller.holding_back = 0u;
	roller_ram_count = 0u;
	roller_ram_hit = 0u;
	machine_fire_done = 0u;
	OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
	if (chaser_object == 0u)
		chaser_object = current_object;
	roller_part_x = OBJECT(current_object)->x;
	roller_part_y = OBJECT(current_object)->y;
	roller_part_frame_cursor = spiked_roller_part_frames_ADDRESS;
	object = current_object;
	for (;;)
	{
		OBJECT(current_object)->sprite_palette = 0x82F0u;
		OBJECT(current_object)->hitbox_height = 0xF0u;
		OBJECT(current_object)->kind = 0xCu;
		OBJECT(current_object)->handler = spiked_roller_update_ADDRESS;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 5;
		OBJECT(current_object)->speed_z = 0;
		OBJECT(current_object)->spiked_roller.state = 2u;
		OBJECT(current_object)->player = 0u;
		OBJECT(current_object)->speed_x = 0;
		OBJECT(current_object)->speed_y = 0;
		record_cursor = roller_part_record;
		OBJECT(current_object)->sprite_flags = SPIKED_ROLLER_PART(record_cursor)->sprite_flags;
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_x = SPIKED_ROLLER_PART(record_cursor - 0x2u)->hitbox_size_x;
		record_cursor += 2u;
		OBJECT(current_object)->hitbox_size_y = SPIKED_ROLLER_PART(record_cursor - 0x4u)->hitbox_size_y;
		record_cursor += 2u;
		OBJECT(current_object)->spiked_roller.path_direction = SPIKED_ROLLER_PART(record_cursor - 0x6u)->path_direction;
		record_cursor += 2u;
		OBJECT(current_object)->flags = SPIKED_ROLLER_PART(record_cursor - 0x8u)->flags;
		record_cursor += 4u;
		OBJECT(current_object)->hitbox_offset_x = SPIKED_ROLLER_PART(record_cursor - 0xCu)->hitbox_offset_x;
		OBJECT(current_object)->hitbox_offset_y = SPIKED_ROLLER_PART(record_cursor - 0xCu)->hitbox_offset_y;
		OBJECT(current_object)->direction = (uint32_t)OBJECT(current_object)->spiked_roller.path_direction << 8;
		OBJECT(current_object)->spiked_roller.leader = object_cursor;
		OBJECT(current_object)->parent = object;
		OBJECT(object)->child = current_object;
		object = (object & ~0xFFFFu) | (uint16_t)current_object;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->animation = spiked_roller_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = SPIKED_ROLLER_START_FRAMES(roller_part_frame_cursor)[0];
		roller_part_frame_cursor++;
		OBJECT(current_object)->sprite_frame = spiked_roller_frames[OBJECT(current_object)->animation_frame];
		parts_left = roller_parts_left;
		roller_parts_left = parts_left - 1u;
		if ((int8_t)parts_left <= 1)
			break;
		allocate_object(&free_count, &layout_cursor);
		if (free_count == 0u)
			return;
		current_object = layout_cursor;
		roller_part_x += roller_step_x;
		OBJECT(current_object)->x = roller_part_x;
		roller_part_y += roller_step_y;
		OBJECT(current_object)->y = roller_part_y;
		OBJECT(current_object)->z = OBJECT(object_cursor)->z;
		OBJECT(current_object)->spiked_roller.path_direction = OBJECT(object_cursor)->spiked_roller.path_direction;
	}
	OBJECT(current_object)->child = 0xFFFFu;
	OBJECT(object_cursor)->spiked_roller.state_word = roller_first_state;
	zone_index = roller_back_zone_index;
	spawn_roller_back_zone(zone_index, layout_cursor);
	if (OBJECT(object_cursor)->spiked_roller.state == 0u)
		players_frozen = 0xFFu;
	if (OBJECT(object_cursor)->spiked_roller.state == 0xEu)
	{
		OBJECT(object_cursor)->speed_z = -0x100000;
		OBJECT(object_cursor)->z = 0x500000;
		OBJECT(object_cursor)->speed_x = 0x8000;
		OBJECT(object_cursor)->speed_y = 0;
		OBJECT(object_cursor)->flags |= 1u;
	}
}

/* Every roller part's handler: the state's routine (0-0x10); then a part marked in player is removed
   (clearing chaser_object if it is it), a hidden one (flags bit 0) is not drawn, and the rest join the
   zone list and are drawn (queue_sprite_at_base, at the previous floor in state 1). */
void spiked_roller_update(void)
{
	uint32_t ignored_2 = 0;

	switch (OBJECT(current_object)->spiked_roller.state)
	{
	case 0:
		spiked_roller_chase();
		break;
	case 1:
		spiked_roller_fall_off_ledge();
		break;
	case 2:
		spiked_roller_follow_leader();
		break;
	case 3:
		spiked_roller_drop_in();
		break;
	case 4:
		spiked_roller_ram();
		break;
	case 5:
		spiked_roller_remove();
		break;
	case 6:
		spiked_roller_crash_into_machine();
		break;
	case 7:
		spiked_roller_sink_and_jump();
		break;
	case 8:
		spiked_roller_fall();
		break;
	case 9:
		spiked_roller_burst_out();
		break;
	case 10:
		spiked_roller_smash_wall();
		break;
	case 11:
		spiked_roller_wait_for_turn();
		break;
	case 12:
		spiked_roller_wait_to_burst_out();
		break;
	case 13:
		spiked_roller_shift_box();
		break;
	case 14:
		spiked_roller_wait_hidden();
		break;
	case 15:
		spiked_roller_spawn_fixed_zone();
		break;
	case 16:
		spiked_roller_chase();
		break;
	default:
		invalid_jump(0x803BBu + OBJECT(current_object)->spiked_roller.state * 3u);
	}
	if (OBJECT(current_object)->player != 0u)
	{
		if (chaser_object == (uint16_t)current_object)
			chaser_object = 0u;
		delete_current_object();
		return;
	}
	if ((OBJECT(current_object)->flags & 1u) != 0u)
		return;
	add_to_zone_list(0u);
	if (OBJECT(current_object)->spiked_roller.state == 1u)
	{
		ignored_2 = queue_sprite_at_previous_floor();
		return;
	}
	ignored_2 = queue_sprite_at_base(0u);
}

/* State 0, the leader: the first frame and, unless holding back, every frame with no player mishap
   (no_player_in_play, player_mishap, player_dropping_in) read the script; a mishap starts holding back
   (base_speed 0, screen_threshold 0x160 or -0x20, hold_timer 0xFF). Then spiked_roller_chase_move_step. */
void spiked_roller_chase(void)
{
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		spiked_roller_chase_read_script();
		return;
	}
	if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
	{
		spiked_roller_chase_move_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		spiked_roller_chase_read_script();
		return;
	}
	OBJECT(current_object)->spiked_roller.holding_back = 0xFFu;
	OBJECT(current_object)->spiked_roller.saved_base_speed = OBJECT(current_object)->spiked_roller.base_speed;
	OBJECT(current_object)->spiked_roller.base_speed = 0;
	OBJECT(current_object)->spiked_roller.hold_start_position = scroll_position;
	OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
	OBJECT(current_object)->spiked_roller.saved_threshold = OBJECT(current_object)->spiked_roller.screen_threshold;
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 3:
	case 4:
		OBJECT(current_object)->spiked_roller.screen_threshold = 0x160;
		spiked_roller_chase_move_step();
		return;
	case 1:
	case 2:
		OBJECT(current_object)->spiked_roller.screen_threshold = -0x20;
		spiked_roller_chase_move_step();
		return;
	default:
		invalid_jump(0x80448u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
}

/* With wait_timer out and scroll_position at the next path record: loads screen_threshold,
   catch_up_factor, base_speed and wait_timer (no wait: kind 0x18 and players_frozen 0); a negative
   factor ends the path (speeds 0, state 5 once off screen). Then spiked_roller_chase_move_step. */
void spiked_roller_chase_read_script(void)
{
	uint32_t off_screen = 0, record_cursor = 0;
	uint16_t record_position = 0;

	if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer != 0u)
	{
		spiked_roller_chase_move_step();
		return;
	}
	record_cursor = OBJECT(current_object)->spiked_roller.path;
	record_position = SPIKED_ROLLER_CHASE_STEP(record_cursor)->position;
	record_cursor += 2u;
	if (scroll_position < record_position)
	{
		spiked_roller_chase_move_step();
		return;
	}
	if (SPIKED_ROLLER_CHASE_STEP(record_cursor - 0x2u)->catch_up_factor >= 0)
	{
		OBJECT(current_object)->spiked_roller.screen_threshold = SPIKED_ROLLER_CHASE_STEP(record_cursor - 0x2u)->screen_threshold;
		record_cursor += 2u;
		OBJECT(current_object)->spiked_roller.catch_up_factor = SPIKED_ROLLER_CHASE_STEP(record_cursor - 0x4u)->catch_up_factor;
		record_cursor += 2u;
		OBJECT(current_object)->spiked_roller.base_speed = SPIKED_ROLLER_CHASE_STEP(record_cursor - 0x6u)->base_speed;
		record_cursor += 2u;
		OBJECT(current_object)->spiked_roller.wait_timer = SPIKED_ROLLER_CHASE_STEP(record_cursor - 0x8u)->wait;
		OBJECT(current_object)->spiked_roller.path = record_cursor + 2u;
		if ((uint16_t)OBJECT(current_object)->spiked_roller.wait_timer != 0u)
		{
			spiked_roller_chase_move_step();
			return;
		}
		OBJECT(current_object)->kind = 0x18u;
		players_frozen = 0u;
		spiked_roller_chase_move_step();
		return;
	}
	OBJECT(current_object)->spiked_roller.catch_up_factor = 0u;
	OBJECT(current_object)->spiked_roller.base_speed = 0;
	off_screen = is_off_screen();
	if (off_screen == 0u)
	{
		spiked_roller_chase_move_step();
		return;
	}
	OBJECT(current_object)->spiked_roller.state = 5u;
}

/* Keeps to its floor (not kinds 2 and 5; a drop over 8: state 1, chaser_object 0); chaser_speed =
   base_speed past screen_threshold, else camera ground speed x catch_up_factor >> 8 (at least base), in
   the hold-back limits; after wait_timer it moves along path_direction unless grey_screen_active. */
void spiked_roller_chase_move_step(void)
{
	uint16_t chase_speed = 0, height_above_floor = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, move_step = 0, speed_level = 0;
	uint8_t hold_timer = 0;
	uint16_t z_int = 0, floor_z_int = 0;

	if (OBJECT(current_object)->spiked_roller.roller_kind != 5u && OBJECT(current_object)->spiked_roller.roller_kind != 2u)
	{
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		z_int = OBJECT(current_object)->z_int;
		floor_z_int = OBJECT(current_object)->floor_z_int;
		height_above_floor = (uint16_t)(floor_z_int - z_int);
		if (floor_z_int == z_int || (int16_t)height_above_floor <= 8)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
		{
			OBJECT(current_object)->spiked_roller.state = 1u;
			chaser_object = 0u;
		}
	}
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->spiked_roller.holding_back == 0u)
	{
		switch (OBJECT(current_object)->spiked_roller.path_direction)
		{
		case 0:
		case 3:
			if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->spiked_roller.screen_threshold)
				chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
			else
			{
				OBJECT(current_object)->spiked_roller.wait_timer = 0;
				if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
				{
					OBJECT(current_object)->spiked_roller.holding_back = 0u;
					OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
					OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
				}
				compute_camera_ground_speed(ignored_4);
				chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
				if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
					chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
			}
			break;
		case 1:
		case 2:
		case 4:
			if (OBJECT(current_object)->spiked_roller.screen_threshold <= OBJECT(current_object)->screen_x)
				chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
			else
			{
				OBJECT(current_object)->spiked_roller.wait_timer = 0;
				if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
				{
					OBJECT(current_object)->spiked_roller.holding_back = 0u;
					OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
					OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
				}
				compute_camera_ground_speed(ignored_4);
				chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
				if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
					chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
			}
			break;
		default:
			invalid_jump(0x80548u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->spiked_roller.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->spiked_roller.holding_back = 0u;
		OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
		OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
		compute_camera_ground_speed(ignored_4);
		chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
		if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
			chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->spiked_roller.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->spiked_roller.hold_timer;
		OBJECT(current_object)->spiked_roller.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u || (players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u)) == 0xFFFFFFu)
		{
			OBJECT(current_object)->spiked_roller.holding_back = 0u;
			OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
			OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
			compute_camera_ground_speed(ignored_4);
			chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
			if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
				chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
		}
		else
		{
			switch (OBJECT(current_object)->spiked_roller.path_direction)
			{
			case 0:
			case 3:
				if (OBJECT(current_object)->screen_x <= OBJECT(current_object)->spiked_roller.screen_threshold)
					chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
				else
				{
					OBJECT(current_object)->spiked_roller.wait_timer = 0;
					if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
					{
						OBJECT(current_object)->spiked_roller.holding_back = 0u;
						OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
						OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
					}
					compute_camera_ground_speed(ignored_4);
					chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
					if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
						chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
				}
				break;
			case 1:
			case 2:
			case 4:
				if (OBJECT(current_object)->spiked_roller.screen_threshold <= OBJECT(current_object)->screen_x)
					chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
				else
				{
					OBJECT(current_object)->spiked_roller.wait_timer = 0;
					if (OBJECT(current_object)->spiked_roller.holding_back != 0u)
					{
						OBJECT(current_object)->spiked_roller.holding_back = 0u;
						OBJECT(current_object)->spiked_roller.screen_threshold = OBJECT(current_object)->spiked_roller.saved_threshold;
						OBJECT(current_object)->spiked_roller.base_speed = OBJECT(current_object)->spiked_roller.saved_base_speed;
					}
					compute_camera_ground_speed(ignored_4);
					chase_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->spiked_roller.catch_up_factor) >> 8;
					if ((int16_t)chase_speed <= OBJECT(current_object)->spiked_roller.base_speed)
						chase_speed = (uint16_t)OBJECT(current_object)->spiked_roller.base_speed;
				}
				break;
			default:
				invalid_jump(0x80548u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
			}
		}
	}
	chaser_speed = chase_speed;
	if (current_game_mode != 0x11u)
		OBJECT(current_object)->spiked_roller.wait_timer--;
	if (OBJECT(current_object)->spiked_roller.wait_timer <= 0)
	{
		OBJECT(current_object)->spiked_roller.wait_timer = 0;
		if (grey_screen_active == 0u)
		{
			switch (OBJECT(current_object)->spiked_roller.path_direction)
			{
			case 0:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y -= move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 1:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x += move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			case 2:
			case 4:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->y += move_step;
				OBJECT(current_object)->speed_y = move_step;
				OBJECT(current_object)->speed_x = 0;
				break;
			case 3:
				move_step = (uint32_t)chaser_speed << 12;
				OBJECT(current_object)->x -= move_step;
				OBJECT(current_object)->speed_x = move_step;
				OBJECT(current_object)->speed_y = 0;
				break;
			default:
				invalid_jump(0x805E4u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
			}
		}
		else
			chaser_speed = 0;
	}
	if ((uint16_t)chaser_speed != 0u)
	{
		speed_level = (uint16_t)chaser_speed >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
		animate(&ignored_2, &ignored_6, &ignored_7);
	}
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 4:
		if (OBJECT(current_object)->screen_x > 0x140)
		{
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
			spiked_roller_chase_check_event();
			return;
		}
		spiked_roller_chase_rumble();
		return;
	case 1:
		if (OBJECT(current_object)->screen_x < -0x50)
		{
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
			spiked_roller_chase_check_event();
			return;
		}
		spiked_roller_chase_rumble();
		return;
	case 2:
		if (OBJECT(current_object)->screen_x < 0)
		{
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
			spiked_roller_chase_check_event();
			return;
		}
		spiked_roller_chase_rumble();
		return;
	case 3:
		if (OBJECT(current_object)->screen_x > 0x140)
		{
			OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
			spiked_roller_chase_check_event();
			return;
		}
		spiked_roller_chase_rumble();
		return;
	default:
		invalid_jump(0x80678u + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
}

/* The rumble on screen: back from off screen (rumble_off_screen) rumble_timer restarts; while it moves,
   sound 0xB every spiked_roller_rumble_delays[chaser_speed >> 4] frames. Then
   spiked_roller_chase_check_event. */
void spiked_roller_chase_rumble(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, speed_level = 0;
	uint8_t rumble_timer = 0;

	if (OBJECT(current_object)->spiked_roller.rumble_off_screen != 0u)
	{
		OBJECT(current_object)->spiked_roller.rumble_off_screen = 0u;
		OBJECT(current_object)->spiked_roller.rumble_timer = 0u;
	}
	if ((uint16_t)chaser_speed == 0u)
	{
		OBJECT(current_object)->spiked_roller.rumble_off_screen = 0xFFu;
		spiked_roller_chase_check_event();
		return;
	}
	rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
	OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
	if (rumble_timer != 0u)
	{
		spiked_roller_chase_check_event();
		return;
	}
	sound_id = 0xBu;
	play_sound(sound_id, &ignored, &ignored_2);
	speed_level = (uint16_t)chaser_speed;
	speed_level = speed_level >> 4;
	if ((int16_t)speed_level < 0)
		speed_level = 0u;
	if ((int16_t)speed_level > 4)
		speed_level = 4u;
	OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
	spiked_roller_chase_check_event();
}

/* Once scroll_position reaches the next spiked_roller_events record (event_offset) with substate 0, its
   test (y at most, x at least, y at least or x at most a coordinate) passing sets state from the record
   and moves to the next record. roller_ram_hit = 0. */
void spiked_roller_chase_check_event(void)
{
	uint32_t event_kind = 0, record_cursor = 0;
	uint16_t event_position = 0, max_y = 0, min_x = 0, min_y = 0, max_x = 0;

	record_cursor = OBJECT(current_object)->spiked_roller.event_offset + spiked_roller_events_ADDRESS;
	event_position = SPIKED_ROLLER_EVENT(record_cursor)->position;
	record_cursor += 2u;
	if (scroll_position < event_position || OBJECT(current_object)->spiked_roller.substate != 0u)
	{
		roller_ram_hit = 0u;
		return;
	}
	event_kind = SPIKED_ROLLER_EVENT(record_cursor - 0x2u)->test * 2u;
	record_cursor += 2u;
	switch (event_kind)
	{
	case 0x0u:
	case 0x8u:
		max_y = SPIKED_ROLLER_EVENT(record_cursor - 0x4u)->coordinate;
		record_cursor += 2u;
		if (OBJECT(current_object)->y_int <= (int16_t)max_y)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)SPIKED_ROLLER_EVENT(record_cursor - 0x6u)->state;
			OBJECT(current_object)->spiked_roller.event_offset_long += 8u;
		}
		break;
	case 0x2u:
		min_x = SPIKED_ROLLER_EVENT(record_cursor - 0x4u)->coordinate;
		record_cursor += 2u;
		if ((int16_t)min_x <= OBJECT(current_object)->x_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)SPIKED_ROLLER_EVENT(record_cursor - 0x6u)->state;
			OBJECT(current_object)->spiked_roller.event_offset_long += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x4u:
		min_y = SPIKED_ROLLER_EVENT(record_cursor - 0x4u)->coordinate;
		record_cursor += 2u;
		if ((int16_t)min_y <= OBJECT(current_object)->y_int)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)SPIKED_ROLLER_EVENT(record_cursor - 0x6u)->state;
			OBJECT(current_object)->spiked_roller.event_offset_long += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	case 0x6u:
		max_x = SPIKED_ROLLER_EVENT(record_cursor - 0x4u)->coordinate;
		record_cursor += 2u;
		if (OBJECT(current_object)->x_int <= (int16_t)max_x)
		{
			OBJECT(current_object)->spiked_roller.state = (uint8_t)SPIKED_ROLLER_EVENT(record_cursor - 0x6u)->state;
			OBJECT(current_object)->spiked_roller.event_offset_long += 8u;
			roller_ram_hit = 0u;
			return;
		}
		break;
	default:
		invalid_jump(0x8071Bu + event_kind);
	}
	roller_ram_hit = 0u;
}

/* State 1: its palette word gets 0x4000 (another priority group), then spiked_roller_fall. */
void spiked_roller_fall_off_ledge(void)
{
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	spiked_roller_fall();
}

/* States 1 and 8: falls (speed_z + 0x1000 a frame), turning fast (animation_speed 1); player = 0xFF
   (removal) once below screen y 0x130. */
void spiked_roller_fall(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	OBJECT(current_object)->speed_z += 0x1000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&ignored, &ignored_2, &ignored_3);
	if (OBJECT(current_object)->screen_y >= 0x130)
		OBJECT(current_object)->player = 0xFFu;
	project_object(&ignored_4, &ignored);
}

/* State 2, the four other parts: the leader's y and speed_y (x and speed_x for path_direction 1 and 3),
   kind, box, z and floors, its states 1 and 8, its hidden and backwards bits; animated at the chase
   speed. Removed (player = 0xFF) when the leader is in state 5. */
void spiked_roller_follow_leader(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, leader_hidden_bit = 0, leader_reverse_bit = 0, object = 0, speed_level = 0;

	if (OBJECT(LINKED(current_object, OBJECT(current_object)->spiked_roller.leader))->spiked_roller.state == 5u)
	{
		OBJECT(current_object)->player = 0xFFu;
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->spiked_roller.leader);
	switch (OBJECT(current_object)->spiked_roller.path_direction)
	{
	case 0:
	case 2:
	case 4:
		OBJECT(current_object)->y = OBJECT(object)->y;
		OBJECT(current_object)->speed_y = OBJECT(object)->speed_y;
		break;
	case 1:
	case 3:
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->speed_x = OBJECT(object)->speed_x;
		break;
	default:
		invalid_jump(0x807BBu + OBJECT(current_object)->spiked_roller.path_direction * 2u);
	}
	OBJECT(current_object)->kind = OBJECT(object)->kind;
	OBJECT(current_object)->hitbox_height = OBJECT(object)->hitbox_height;
	OBJECT(current_object)->hitbox_offset_x = OBJECT(object)->hitbox_offset_x;
	OBJECT(current_object)->hitbox_offset_y = OBJECT(object)->hitbox_offset_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	OBJECT(current_object)->floor_z = OBJECT(object)->floor_z;
	OBJECT(current_object)->previous_floor_z = OBJECT(object)->previous_floor_z;
	if (OBJECT(object)->spiked_roller.state == 1u)
		OBJECT(current_object)->spiked_roller.state = 1u;
	if (OBJECT(object)->spiked_roller.state == 8u)
		OBJECT(current_object)->spiked_roller.state = 8u;
	project_object(&ignored, &ignored_2);
	if ((uint16_t)chaser_speed == 0u)
	{
		leader_hidden_bit = OBJECT(LINKED(current_object, OBJECT(current_object)->spiked_roller.leader))->flags & 1u;
		OBJECT(current_object)->flags &= ~1u;
		OBJECT(current_object)->flags |= leader_hidden_bit;
		return;
	}
	speed_level = (uint16_t)chaser_speed >> 4;
	if ((int16_t)speed_level > 5)
		speed_level = 5u;
	OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
	leader_reverse_bit = OBJECT(LINKED(current_object, OBJECT(current_object)->spiked_roller.leader))->flags & 0x80u;
	OBJECT(current_object)->flags &= ~0x80u;
	OBJECT(current_object)->flags |= leader_reverse_bit;
	animate(&ignored_2, &ignored_3, &ignored_4);
	leader_hidden_bit = OBJECT(LINKED(current_object, OBJECT(current_object)->spiked_roller.leader))->flags & 1u;
	OBJECT(current_object)->flags &= ~1u;
	OBJECT(current_object)->flags |= leader_hidden_bit;
}

/* State 0xE: hidden (flags bit 0) until scroll_position reaches 0x9B00 or machine_fire_done is set; then
   shown, state 3 (spiked_roller_drop_in). */
void spiked_roller_wait_hidden(void)
{
	OBJECT(current_object)->flags |= 1u;
	if (scroll_position >= 0x9B00u || machine_fire_done != 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 3u;
		OBJECT(current_object)->flags &= ~1u;
	}
}

/* State 3: leaps and comes down (speed_z + 0x8000 a frame, moving by speed_x and speed_y) onto z 0:
   camera_shake_active, sound 0x20, state 0. */
void spiked_roller_drop_in(void)
{
	uint8_t sound_number = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->speed_z += 0x8000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&ignored, &ignored_2, &ignored_3);
	OBJECT(current_object)->floor_z = 0;
	if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
	{
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->spiked_roller.state = 0u;
		camera_shake_active = 1u;
		sound_number = 0x20u;
		play_sound(sound_number, &ignored, &ignored_4);
	}
	project_object(&ignored_5, &ignored);
}

/* State 4, the ram: spiked_roller_ram_charge, _pause, _back_off or _wait by substate, with its own
   rumble (sound 0xB). No script or event sets state 4, so it never runs. */
void spiked_roller_ram(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, speed_level = 0;
	uint8_t rumble_timer = 0;

	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
		spiked_roller_ram_charge();
		break;
	case 1:
		spiked_roller_ram_pause();
		break;
	case 2:
		spiked_roller_ram_back_off();
		break;
	case 3:
		spiked_roller_ram_wait();
		break;
	case 4:
		spiked_roller_ram_charge();
		break;
	default:
		invalid_jump(0x80944u + OBJECT(current_object)->spiked_roller.substate * 3u);
	}
	rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
	OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
	if ((int8_t)rumble_timer > 1)
	{
		project_object(&ignored_3, &ignored);
		return;
	}
	sound_id = 0xBu;
	play_sound(sound_id, &ignored, &ignored_2);
	speed_level = (uint16_t)chaser_speed;
	speed_level = speed_level >> 4;
	if ((int16_t)speed_level < 0)
		speed_level = 0u;
	if ((int16_t)speed_level > 4)
		speed_level = 4u;
	OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
	project_object(&ignored_3, &ignored);
}

/* Ram substates 0 and 4: rolls -y 4 a frame (chaser_speed 0x40) until y is at most the event's
   coordinate - 0x50: camera_shake_active, roller_ram_hit, sound 7, next substate. Off screen, or after 4
   charges (roller_ram_count), back to state 0. */
void spiked_roller_ram_charge(void)
{
	uint8_t sound_id = 0;
	uint16_t margin_x = 0, margin_y = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = -0x40000;
	chaser_speed = 0x40;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&ignored, &ignored_2, &ignored_3);
	if ((int16_t)(SPIKED_ROLLER_EVENT(spiked_roller_events_ADDRESS + OBJECT(current_object)->spiked_roller.event_offset)[-1].coordinate - 0x50u) < OBJECT(current_object)->y_int)
		return;
	OBJECT(current_object)->spiked_roller.substate++;
	camera_shake_active = 1u;
	roller_ram_hit = 0xFFu;
	OBJECT(current_object)->spiked_roller.pause_timer = 0u;
	sound_id = 7u;
	play_sound(sound_id, &ignored, &ignored_4);
	margin_x = 0x50u;
	margin_y = 0x50u;
	off_screen = is_off_screen_margin(margin_x, margin_y);
	if (off_screen != 0u)
	{
		OBJECT(current_object)->spiked_roller.substate = 0u;
		roller_ram_count = 0u;
		OBJECT(current_object)->spiked_roller.state = 0u;
		OBJECT(current_object)->flags &= ~0x800u;
		OBJECT(current_object)->flags |= 0x80u;
	}
	roller_ram_count++;
	if ((int8_t)roller_ram_count >= 4)
	{
		OBJECT(current_object)->spiked_roller.substate = 0u;
		roller_ram_count = 0u;
		OBJECT(current_object)->spiked_roller.state = 0u;
	}
}

/* Ram substate 1: stopped (chaser_speed 0, roller_ram_hit 0) for 0x3C frames (pause_timer); then
   substate 2 with the animation forwards. */
void spiked_roller_ram_pause(void)
{
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_speed = 0;
	roller_ram_hit = 0u;
	OBJECT(current_object)->spiked_roller.pause_timer++;
	if ((int8_t)OBJECT(current_object)->spiked_roller.pause_timer >= 0x3C)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->flags &= ~0x80u;
	}
}

/* Ram substate 2: rolls back +y one a frame (chaser_speed 0x10) until y reaches the coordinate of the
   event that fired; then substate 3 with the animation backwards (flags bit 7). */
void spiked_roller_ram_back_off(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0x10000;
	chaser_speed = 0x10;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->animation_speed = 4u;
	animate(&ignored, &ignored_2, &ignored_3);
	if (SPIKED_ROLLER_EVENT(spiked_roller_events_ADDRESS + OBJECT(current_object)->spiked_roller.event_offset)[-1].coordinate <= OBJECT(current_object)->y_int)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->spiked_roller.pause_timer = 0u;
		OBJECT(current_object)->flags |= 0x80u;
	}
}

/* Ram substate 3: turns in place (chaser_speed 0x40) for 0x3C frames, counted only while a player is in
   play, then charges again (substate 0). */
void spiked_roller_ram_wait(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	chaser_speed = 0x40;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&ignored, &ignored_2, &ignored_3);
	if (no_player_in_play == 0u)
	{
		OBJECT(current_object)->spiked_roller.pause_timer++;
		if ((int8_t)OBJECT(current_object)->spiked_roller.pause_timer >= 0x3C)
			OBJECT(current_object)->spiked_roller.substate = 0u;
	}
}

/* State 5: player = 0xFF, the mark spiked_roller_update removes the part by (clearing chaser_object if
   it is the leader). */
void spiked_roller_remove(void)
{
	OBJECT(current_object)->player = 0xFFu;
}

/* State 6: rolls -y at chaser_speed 0x10 (0x50 once scroll_position reaches 0x8600) with its rumble
   until y is 0xAE0 or less: stops with camera_shake_active, sound 6, machine_wrecked and
   scroll_resume_request; state 5 and chaser_object 0 once below screen y 0x158. */
void spiked_roller_crash_into_machine(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, jump_offset = 0, speed_level = 0;
	uint8_t rumble_timer = 0;

	jump_offset = OBJECT(current_object)->spiked_roller.substate * 3u;
	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
	case 4:
		OBJECT(current_object)->spiked_roller.substate++;
		chaser_speed = 0x10;
		OBJECT(current_object)->speed_y = -0x10000;
		speed_level = (uint16_t)chaser_speed;
		speed_level = speed_level >> 4;
		if ((int16_t)speed_level > 5)
			speed_level = 5u;
		OBJECT(current_object)->animation_speed = spiked_roller_animation_speeds[speed_level];
	case 1:
		if (scroll_position >= 0x8600u)
		{
			chaser_speed = 0x50;
			OBJECT(current_object)->speed_y = -0x50000;
		}
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		animate(&ignored, &ignored_2, &ignored_3);
		rumble_timer = OBJECT(current_object)->spiked_roller.rumble_timer;
		OBJECT(current_object)->spiked_roller.rumble_timer = rumble_timer - 1u;
		if ((int8_t)rumble_timer > 1)
		{
			if (OBJECT(current_object)->y_int <= 0xAE0)
				OBJECT(current_object)->spiked_roller.substate++;
			project_object(&ignored_5, &ignored);
			return;
		}
		sound_id = 0xBu;
		play_sound(sound_id, &ignored, &ignored_4);
		speed_level = (uint16_t)chaser_speed;
		speed_level = speed_level >> 4;
		if ((int16_t)speed_level < 0)
			speed_level = 0u;
		if ((int16_t)speed_level > 4)
			speed_level = 4u;
		OBJECT(current_object)->spiked_roller.rumble_timer = spiked_roller_rumble_delays[speed_level];
		if (OBJECT(current_object)->y_int <= 0xAE0)
			OBJECT(current_object)->spiked_roller.substate++;
		project_object(&ignored_5, &ignored);
		return;
	case 2:
		OBJECT(current_object)->spiked_roller.substate++;
		chaser_speed = 0;
		OBJECT(current_object)->speed_y = 0;
		camera_shake_active = 1u;
		sound_id = 6u;
		play_sound(sound_id, &ignored, &ignored_4);
		machine_wrecked = 0xFFu;
		scroll_resume_request = 0xFFu;
		update_screen_position(&ignored_5, &ignored);
		return;
	case 3:
		if (OBJECT(current_object)->screen_y >= 0x158)
		{
			OBJECT(current_object)->spiked_roller.state = 5u;
			chaser_object = 0u;
		}
		update_screen_position(&ignored_5, &ignored);
		return;
	default:
		invalid_jump(0x80ACBu + OBJECT(current_object)->spiked_roller.substate * 3u);
	}
}

/* State 7: sets stage_03_floor_block_falls[0] (sound 0xD the first time) and sinks 0x40 into its floor,
   then jumps (speed_z -6, speed_x 3, gravity 0x3000) and lands on it: state 8,
   stage_03_floor_block_falls_1, camera_shake_active, goal_unlocked, sound 0xD. */
void spiked_roller_sink_and_jump(void)
{
	uint8_t sound_number = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	switch (OBJECT(current_object)->spiked_roller.substate)
	{
	case 0:
	case 2:
		break;
	case 1:
		OBJECT(current_object)->speed_z += 0x3000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
		OBJECT(current_object)->animation_speed = 1u;
		animate(&ignored, &ignored_3, &ignored_4);
		if (OBJECT(current_object)->speed_z_int >= 0 && OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->spiked_roller.state = 8u;
			OBJECT(current_object)->speed_z = 0;
			stage_03_floor_block_falls_1 = 0xFFu;
			camera_shake_active = 1u;
			goal_unlocked = 0xFFu;
			sound_number = 0xDu;
			play_sound(sound_number, &ignored, &ignored_2);
		}
		project_object(&ignored_5, &ignored);
		return;
	default:
		invalid_jump(0x80BC9u + OBJECT(current_object)->spiked_roller.substate * 2u);
	}
	if (stage_03_floor_block_falls[0] == 0u)
	{
		sound_number = 0xDu;
		play_sound(sound_number, &ignored, &ignored_2);
	}
	stage_03_floor_block_falls[0] = 0xFFu;
	OBJECT(current_object)->speed_z += 0x1000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	chaser_speed = 0x40;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->animation_speed = 1u;
	animate(&ignored, &ignored_3, &ignored_4);
	if ((int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->floor_z_int) >= 0x40)
	{
		OBJECT(current_object)->spiked_roller.substate++;
		OBJECT(current_object)->speed_z = -0x60000;
		OBJECT(current_object)->speed_x = 0x30000;
	}
	project_object(&ignored_5, &ignored);
}

/* State 9: hidden for wait_timer frames, then the roller bursts out: state 0, it becomes chaser_object,
   scroll_resume_request, sound 7, and roller_wall_fragment_count pieces of wall (roller_wall_fragments
   records) fly off as rock_debris_fly objects. */
void spiked_roller_burst_out(void)
{
	uint8_t sound_id = 0;
	uint16_t screen_x = 0;
	uint32_t fragment_cursor = 0, fragments_left = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;
	uint16_t wait_timer = 0;

	OBJECT(current_object)->flags |= 1u;
	wait_timer = OBJECT(current_object)->spiked_roller.wait_timer;
	OBJECT(current_object)->spiked_roller.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer > 1)
		return;
	OBJECT(current_object)->spiked_roller.state = 0u;
	chaser_object = current_object;
	OBJECT(current_object)->flags &= ~1u;
	scroll_resume_request = 0xFFu;
	project_object(&ignored, &ignored_2);
	sound_id = 7u;
	play_sound(sound_id, &ignored_2, &ignored_3);
	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	fragments_left = roller_wall_fragment_count;
	fragment_cursor = roller_wall_fragments_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = ROCK_DEBRIS_PIECE(fragment_cursor)->sprite_flags;
		OBJECT(object)->screen_x = screen_x;
		fragment_cursor += 2u;
		OBJECT(object)->screen_x += ROCK_DEBRIS_PIECE(fragment_cursor - 0x2u)->offset_x;
		OBJECT(object)->screen_y = r14;
		fragment_cursor += 2u;
		OBJECT(object)->screen_y += ROCK_DEBRIS_PIECE(fragment_cursor - 0x4u)->offset_y;
		fragment_cursor += 2u;
		OBJECT(object)->speed_x_fraction = ROCK_DEBRIS_PIECE(fragment_cursor - 0x6u)->speed_x;
		fragment_cursor += 2u;
		OBJECT(object)->speed_y_fraction = ROCK_DEBRIS_PIECE(fragment_cursor - 0x8u)->speed_y;
		fragment_cursor += 2u;
		OBJECT(object)->animation = rock_debris_animations_ADDRESS + ROCK_DEBRIS_PIECE(fragment_cursor - 0xAu)->animation_offset;
		OBJECT(object)->sprite_palette = 0x8240u;
		OBJECT(object)->projected_x = (uint32_t)OBJECT(object)->screen_x << 4;
		OBJECT(object)->projected_y_fraction = (uint32_t)OBJECT(object)->screen_y << 4;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->handler = rock_debris_fly_ADDRESS;
		fragment_cursor += 2u;
		fragments_left--;
	} while (fragments_left != 0u);
}

/* State 0xA: stops as a solid kind 0xC (chaser_speed 0); in stage 0xE it sets chaser_stopped. State 5
   once off screen (margins 0x68, 0x70). */
void spiked_roller_smash_wall(void)
{
	uint16_t margin_y = 0, x_margin = 0;
	uint32_t ignored = 0, ignored_2 = 0, off_screen = 0;

	OBJECT(current_object)->kind = 0xCu;
	chaser_speed = 0;
	if (stage == 0xEu)
		chaser_stopped = 0xFFu;
	x_margin = 0x68u;
	margin_y = 0x70u;
	off_screen = is_off_screen_margin(x_margin, margin_y);
	if (off_screen != 0u)
		OBJECT(current_object)->spiked_roller.state = 5u;
	update_screen_position(&ignored, &ignored_2);
}

/* State 0xB: waits as a still kind 0xC until chaser_object is free, then takes it: state 0,
   hitbox_height 0x40. */
void spiked_roller_wait_for_turn(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->kind = 0xCu;
	chaser_speed = 0;
	if (chaser_object == 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 0u;
		chaser_object = current_object;
		OBJECT(current_object)->hitbox_height = 0x40u;
	}
	project_object(&ignored, &ignored_2);
}

/* State 0xC: hidden until chaser_object is free, then state 9 with wait_timer 0x3C
   (spiked_roller_burst_out). */
void spiked_roller_wait_to_burst_out(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->flags |= 1u;
	if (chaser_object == 0u)
	{
		OBJECT(current_object)->spiked_roller.state = 9u;
		OBJECT(current_object)->spiked_roller.wait_timer = 0x3C;
	}
	project_object(&ignored, &ignored_2);
}

/* State 0xD: hitbox_offset_y = -0x26, then back to the chase (state 0, spiked_roller_chase). */
void spiked_roller_shift_box(void)
{
	OBJECT(current_object)->spiked_roller.state = 0u;
	OBJECT(current_object)->hitbox_offset_y = -0x26;
	spiked_roller_chase();
}

/* State 0xF: leaves an invisible hazard box (0x100 x 0xF3, height 0xA0, kind 0xC,
   roller_fixed_zone_update) at x -0x8AD, y 0x114A, z 0x470, then back to the chase (state 0). */
void spiked_roller_spawn_fixed_zone(void)
{
	uint32_t free_count = 0, object = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->kind = 0xCu;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->hitbox_size_x = 0x100u;
		OBJECT(object)->hitbox_size_y = 0xF3u;
		OBJECT(object)->direction = 0x300u;
		OBJECT(object)->hitbox_height = 0xA0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->x = -0x8AD0000;
		OBJECT(object)->y = 0x114A0000;
		OBJECT(object)->z = 0x4700000;
		OBJECT(object)->handler = roller_fixed_zone_update_ADDRESS;
	}
	OBJECT(current_object)->spiked_roller.state = 0u;
	spiked_roller_chase();
}

/* The hazard box spiked_roller_spawn_fixed_zone leaves: touchable until scroll_position reaches 0x7600,
   then removed. */
void roller_fixed_zone_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	if (scroll_position < 0x7600u)
	{
		project_object(&ignored, &ignored_2);
		ignored = add_to_touchable_list(ignored);
		return;
	}
	delete_current_object();
}

/* The machinery (machine_animation, box 0xE8 x 0x20, height 0x50, kind 0xC, machine_run) on its floor;
   clears machine_wrecked. Spawns its fire (machine_fire_update, a small flame) and a GO! arrow sign
   (spawn_sign kind 5). */
void machine_start(void)
{
	uint16_t kind = 0, x = 0, y = 0, z = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1006u;
	OBJECT(current_object)->sprite_palette = 0x9050u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_offset_x = 0;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->hitbox_size_x = 0xE8u;
	OBJECT(current_object)->hitbox_size_y = 0x20u;
	OBJECT(current_object)->hitbox_height = 0x50u;
	OBJECT(current_object)->kind = 0xCu;
	machine_wrecked = 0u;
	OBJECT(current_object)->handler = machine_run_ADDRESS;
	OBJECT(current_object)->animation = machine_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 8u;
	OBJECT(current_object)->animation_timer = 0;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1004u;
		OBJECT(object)->sprite_palette = 0x9060u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = machine_fire_update_ADDRESS;
		OBJECT(object)->machine_fire.state = 0u;
		OBJECT(object)->parent = current_object;
		OBJECT(object)->animation = machine_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(object)->projected_x += 0x2Cu;
		OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(object)->projected_y_int -= 0x59u;
		OBJECT(object)->z = OBJECT(current_object)->z;
		x = 0xF74Du;
		y = 0xAEDu;
		kind = 5u;
		z = 0x43Eu;
		spawn_sign(x, y, kind, z, &ignored_2, &object);
	}
}

/* The machine: animated until machine_wrecked, then stopped on the smashed frame (0x68E8); touchable,
   drawn at screen_y - 0x68; removed once it has left the screen. */
void machine_run(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	if (machine_wrecked != 0u)
	{
		OBJECT(current_object)->animation_frame = 0xFFu;
		OBJECT(current_object)->sprite_frame = 0x68E8u;
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	update_screen_position(&ignored, &ignored_2);
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
		animate(&ignored_2, &ignored_3, &ignored_4);
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x68u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* The machine's fire: a small flame until the machine is wrecked; then an invisible box (kind 0x13,
   rising_fire_object): the spawns_with_next_handler fireballs, a climb (machine_fire_script), 0x80
   frames +x with fireballs in three lanes (lane 3's timer is sound_timer), machine_fire_done. */
void machine_fire_update(void)
{
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, ignored_8 = 0, object = 0, random_bits = 0, spawn_record = 0;
	uint8_t lane_timer_before = 0, lane_before = 0, sound_timer = 0, sound_timer_2 = 0;
	uint16_t spawns_left_before = 0;
	uint32_t flags = 0, lane_timer_address = 0;

	switch (OBJECT(current_object)->machine_fire.state)
	{
	case 0:
	case 8:
		if ((int8_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->animation_frame < 0)
			OBJECT(current_object)->machine_fire.state++;
		update_screen_position(&ignored, &ignored_2);
		animate(&ignored_2, &ignored_3, &ignored_4);
		ignored_2 = queue_sprite(ignored_2);
		return;
	case 1:
		OBJECT(current_object)->machine_fire.state++;
		OBJECT(current_object)->x = -0x8B40000;
		OBJECT(current_object)->y = 0xA900000;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->hitbox_size_x = 0x90u;
		OBJECT(current_object)->hitbox_size_y = 0xD0u;
		OBJECT(current_object)->hitbox_height = 0x70u;
		OBJECT(current_object)->hitbox_offset_x = -0x48;
		OBJECT(current_object)->hitbox_offset_y = 0;
		OBJECT(current_object)->kind = 0x13u;
		OBJECT(current_object)->machine_fire.off_screen = 0xFFu;
		find_floor(&ignored, &ignored_2, &ignored_5, &ignored_6, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->z += (uint32_t)OBJECT(current_object)->hitbox_height << 16;
		rising_fire_object = current_object;
		OBJECT(current_object)->machine_fire.path = machine_fire_script_ADDRESS;
		OBJECT(current_object)->machine_fire.holding_back = 0u;
		project_object(&ignored, &ignored_2);
		OBJECT(current_object)->machine_fire.projected_x_whole = OBJECT(current_object)->projected_x;
		OBJECT(current_object)->machine_fire.projected_x_fraction = 0u;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->handler = flame_start_ADDRESS;
			OBJECT(object)->x = -0x8B80000;
			OBJECT(object)->y = 0xA500000;
			spawn_record = spawns_with_next_handler_ADDRESS;
			if (free_object_stack < (int16_t)spawns_with_next_handler)
				return;
			temp_0 = MACHINE_FIREBALL_SPAWNS(spawn_record)->count;
			spawn_record += 2u;
			do
			{
				allocate_object(&ignored, &object);
				OBJECT(object)->handler = fireball_start_ADDRESS;
				OBJECT(object)->fireball_start.state = 0u;
				OBJECT(object)->x_int = MACHINE_FIREBALL_SPAWN(spawn_record)->x;
				OBJECT(object)->x_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->y_int = MACHINE_FIREBALL_SPAWN(spawn_record - 0x2u)->y;
				OBJECT(object)->y_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->fireball_start.target_x = MACHINE_FIREBALL_SPAWN(spawn_record - 0x4u)->target_x;
				spawn_record += 2u;
				OBJECT(object)->fireball_start.target_y = MACHINE_FIREBALL_SPAWN(spawn_record - 0x6u)->target_y;
				spawn_record += 2u;
				OBJECT(object)->z_int = MACHINE_FIREBALL_SPAWN(spawn_record - 0x8u)->z;
				OBJECT(object)->z_fraction = 0u;
				spawn_record += 2u;
				OBJECT(object)->next_handler = MACHINE_FIREBALL_SPAWN(spawn_record - 0xAu)->next_handler;
				spawns_left_before = temp_0;
				temp_0 = spawns_left_before - 1u;
				spawn_record += 4u;
			} while ((int16_t)spawns_left_before > 1);
		}
		return;
	case 2:
		flags = OBJECT(current_object)->flags;
		OBJECT(current_object)->flags = flags | 0x80000000u;
		if ((flags & 0x80000000u) == 0u)
		{
			machine_fire_read_script();
			return;
		}
		if (OBJECT(current_object)->machine_fire.holding_back != 0u)
		{
			machine_fire_climb_step();
			return;
		}
		if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
		{
			machine_fire_read_script();
			return;
		}
		OBJECT(current_object)->machine_fire.holding_back = 0xFFu;
		OBJECT(current_object)->machine_fire.saved_base_speed = OBJECT(current_object)->machine_fire.base_speed;
		OBJECT(current_object)->machine_fire.base_speed = 0;
		OBJECT(current_object)->machine_fire.hold_start_position = scroll_position;
		OBJECT(current_object)->machine_fire.hold_timer = 0xFFu;
		OBJECT(current_object)->machine_fire.saved_threshold = OBJECT(current_object)->machine_fire.screen_threshold;
		OBJECT(current_object)->machine_fire.screen_threshold = 0xE0;
		machine_fire_climb_step();
		return;
	case 3:
		OBJECT(current_object)->machine_fire.state++;
		OBJECT(current_object)->machine_fire.sound_timer = 0;
		OBJECT(current_object)->machine_fire.off_screen = 0u;
		OBJECT(current_object)->machine_fire.move_frames = 0;
		OBJECT(current_object)->z = 0x100000;
		OBJECT(current_object)->x = -0x8520000;
		OBJECT(current_object)->y = 0xA790000;
		OBJECT(current_object)->hitbox_size_x = 0x58u;
		OBJECT(current_object)->hitbox_size_y = 0x1A2u;
		OBJECT(current_object)->hitbox_offset_x = -0x68;
		OBJECT(current_object)->hitbox_offset_y = -0x60;
		OBJECT(current_object)->kind = 0x14u;
		OBJECT(current_object)->direction = 0x100u;
	case 4:
		OBJECT(current_object)->x += 0x10000u;
		OBJECT(current_object)->machine_fire.move_frames++;
		if (OBJECT(current_object)->machine_fire.move_frames >= 0x80)
			OBJECT(current_object)->machine_fire.state++;
		find_floor(&ignored, &ignored_2, &ignored_5, &random_bits, &object);
		fireball_lane_timer_cursor = current_object + 0x72u;
		fireball_lane = 3;
		do
		{
			lane_timer_address = fireball_lane_timer_cursor;
			lane_timer_before = mem_read8(lane_timer_address);
			mem_write8(lane_timer_address, lane_timer_before - 1u);
			if ((int8_t)lane_timer_before <= 1)
			{
				next_random(&random_bits, &ignored_7, &ignored_8);
				random_bits = (random_bits & ~0xFFu) | (random_bits & 0x1Fu);
				mem_write8(fireball_lane_timer_cursor, random_bits);
				allocate_object(&free_count, &object);
				if (free_count == 0u)
					break;
				OBJECT(object)->handler = fireball_start_ADDRESS;
				OBJECT(object)->fireball_start.state = 0u;
				OBJECT(object)->x = OBJECT(current_object)->x;
				OBJECT(object)->y = OBJECT(current_object)->y;
				OBJECT(object)->y_int += (uint8_t)fireball_lane * 0x38u;
				OBJECT(object)->fireball_start.target_x = OBJECT(object)->x_int;
				OBJECT(object)->fireball_start.target_x += mem_read8(fireball_lane_timer_cursor);
				OBJECT(object)->fireball_start.target_y = OBJECT(object)->y_int;
				OBJECT(object)->z = OBJECT(current_object)->z;
				OBJECT(object)->next_handler = flame_start_ADDRESS;
			}
			fireball_lane_timer_cursor++;
			lane_before = fireball_lane;
			fireball_lane = lane_before - 1u;
		} while ((int8_t)lane_before > 1);
		sound_timer = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer - 1u;
		if ((int8_t)sound_timer <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id = 9u;
			play_sound(sound_id, &ignored_2, &ignored_5);
		}
		project_object(&ignored, &ignored_2);
		ignored = add_to_touchable_list(ignored);
		return;
	case 5:
		machine_fire_done = 0xFFu;
		OBJECT(current_object)->machine_fire.state++;
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) > 0x110 || OBJECT(current_object)->screen_x <= -0xA0)
			OBJECT(current_object)->machine_fire.state++;
		sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
		if ((int8_t)sound_timer_2 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id = 9u;
			play_sound(sound_id, &ignored_2, &ignored_5);
		}
		update_screen_position(&ignored, &ignored_2);
		ignored = add_to_touchable_list(ignored);
		return;
	case 6:
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) > 0x110 || OBJECT(current_object)->screen_x <= -0xA0)
			OBJECT(current_object)->machine_fire.state++;
		sound_timer_2 = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer_2 - 1u;
		if ((int8_t)sound_timer_2 <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id = 9u;
			play_sound(sound_id, &ignored_2, &ignored_5);
		}
		update_screen_position(&ignored, &ignored_2);
		ignored = add_to_touchable_list(ignored);
		return;
	case 7:
		delete_current_object();
		return;
	default:
		invalid_jump(0x8358Cu + OBJECT(current_object)->machine_fire.state * 3u);
	}
}

/* Until the fire's top (z - hitbox_height) reaches the height of the next machine_fire_script record,
   machine_fire_climb_step; then it loads the record's screen_threshold, catch_up_factor, base_speed and
   wait_timer and climbs on, or at a negative factor the climb ends (next state). */
void machine_fire_read_script(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, path_cursor = 0;
	uint8_t sound_timer = 0;
	uint16_t record_height = 0;

	path_cursor = OBJECT(current_object)->machine_fire.path;
	record_height = MACHINE_FIRE_SCRIPT_STEP(path_cursor)->height;
	path_cursor += 2u;
	if ((int16_t)record_height < (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height))
	{
		machine_fire_climb_step();
		return;
	}
	if (MACHINE_FIRE_SCRIPT_STEP(path_cursor - 0x2u)->catch_up_factor < 0)
	{
		OBJECT(current_object)->machine_fire.state++;
		sound_timer = OBJECT(current_object)->machine_fire.sound_timer;
		OBJECT(current_object)->machine_fire.sound_timer = sound_timer - 1u;
		if ((int8_t)sound_timer <= 1)
		{
			OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
			sound_id = 9u;
			play_sound(sound_id, &ignored, &ignored_2);
		}
		update_screen_position(&ignored_3, &ignored);
		ignored_3 = add_to_touchable_list(ignored_3);
		return;
	}
	OBJECT(current_object)->machine_fire.screen_threshold = MACHINE_FIRE_SCRIPT_STEP(path_cursor - 0x2u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->machine_fire.catch_up_factor = MACHINE_FIRE_SCRIPT_STEP(path_cursor - 0x4u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->machine_fire.base_speed = MACHINE_FIRE_SCRIPT_STEP(path_cursor - 0x6u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->machine_fire.wait_timer = MACHINE_FIRE_SCRIPT_STEP(path_cursor - 0x8u)->wait;
	OBJECT(current_object)->machine_fire.path = path_cursor + 2u;
	machine_fire_climb_step();
}

/* The climb: base_speed while the top's screen y (screen_y - hitbox_height) is screen_threshold or less,
   else catching up with the camera, in the hold-back limits; after wait_timer z climbs by it and
   projected x, y follow the floor attribute's x_per_rise (x, y from it). Sound 9 every 0x3C frames on
   screen. */
void machine_fire_climb_step(void)
{
	uint16_t climb_speed = 0;
	uint32_t ignored_2 = 0, ignored_3 = 0, point_x = 0, point_y = 0, projected_dx = 0, sound_id = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t wait_timer = 0;

	if (OBJECT(current_object)->machine_fire.holding_back == 0u)
	{
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->machine_fire.screen_threshold)
			climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
		else
		{
			if (OBJECT(current_object)->machine_fire.holding_back != 0u)
			{
				OBJECT(current_object)->machine_fire.holding_back = 0u;
				OBJECT(current_object)->machine_fire.screen_threshold = OBJECT(current_object)->machine_fire.saved_threshold;
				OBJECT(current_object)->machine_fire.base_speed = OBJECT(current_object)->machine_fire.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->machine_fire.catch_up_factor) >> 8;
			if ((int16_t)climb_speed <= OBJECT(current_object)->machine_fire.base_speed)
				climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->machine_fire.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->machine_fire.holding_back = 0u;
		OBJECT(current_object)->machine_fire.screen_threshold = OBJECT(current_object)->machine_fire.saved_threshold;
		OBJECT(current_object)->machine_fire.base_speed = OBJECT(current_object)->machine_fire.saved_base_speed;
		compute_camera_ground_speed(0u);
		climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->machine_fire.catch_up_factor) >> 8;
		if ((int16_t)climb_speed <= OBJECT(current_object)->machine_fire.base_speed)
			climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->machine_fire.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->machine_fire.hold_timer;
		OBJECT(current_object)->machine_fire.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->machine_fire.holding_back = 0u;
			OBJECT(current_object)->machine_fire.screen_threshold = OBJECT(current_object)->machine_fire.saved_threshold;
			OBJECT(current_object)->machine_fire.base_speed = OBJECT(current_object)->machine_fire.saved_base_speed;
			compute_camera_ground_speed(0u);
			climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->machine_fire.catch_up_factor) >> 8;
			if ((int16_t)climb_speed <= OBJECT(current_object)->machine_fire.base_speed)
				climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
		}
		else if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->machine_fire.screen_threshold)
			climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
		else
		{
			if (OBJECT(current_object)->machine_fire.holding_back != 0u)
			{
				OBJECT(current_object)->machine_fire.holding_back = 0u;
				OBJECT(current_object)->machine_fire.screen_threshold = OBJECT(current_object)->machine_fire.saved_threshold;
				OBJECT(current_object)->machine_fire.base_speed = OBJECT(current_object)->machine_fire.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->machine_fire.catch_up_factor) >> 8;
			if ((int16_t)climb_speed <= OBJECT(current_object)->machine_fire.base_speed)
				climb_speed = (uint16_t)OBJECT(current_object)->machine_fire.base_speed;
		}
	}
	OBJECT(current_object)->speed_fraction = climb_speed;
	wait_timer = OBJECT(current_object)->machine_fire.wait_timer;
	OBJECT(current_object)->machine_fire.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->machine_fire.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	}
	projected_dx = (int32_t)((uint32_t)FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->x_per_rise * OBJECT(current_object)->speed_fraction) >> 4;
	OBJECT(current_object)->machine_fire.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->machine_fire.projected_x_whole;
	OBJECT(current_object)->projected_y -= (int32_t)projected_dx >> 1;
	update_screen_position(&sound_id, &ignored_2);
	if (OBJECT(current_object)->screen_y > 0x160)
	{
		OBJECT(current_object)->machine_fire.off_screen = 0xFFu;
		point_x = OBJECT(current_object)->machine_fire.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
		sound_id = add_to_touchable_list(sound_id);
		return;
	}
	if (OBJECT(current_object)->machine_fire.off_screen != 0u)
	{
		OBJECT(current_object)->machine_fire.off_screen = 0u;
		OBJECT(current_object)->machine_fire.sound_timer = 0;
	}
	if (current_game_mode == 7u)
	{
		point_x = OBJECT(current_object)->machine_fire.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
		sound_id = add_to_touchable_list(sound_id);
		return;
	}
	sound_timer = OBJECT(current_object)->machine_fire.sound_timer;
	OBJECT(current_object)->machine_fire.sound_timer = sound_timer - 1u;
	if ((int8_t)sound_timer > 1)
	{
		point_x = OBJECT(current_object)->machine_fire.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
		sound_id = add_to_touchable_list(sound_id);
		return;
	}
	OBJECT(current_object)->machine_fire.sound_timer = 0x3C;
	sound_id = 9u;
	play_sound((uint8_t)sound_id, &ignored_2, &ignored_3);
	point_x = OBJECT(current_object)->machine_fire.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	sound_id = add_to_touchable_list(sound_id);
}

/* A fireball flipped in x (sprite flags 0x1044, palette 0x10C0), then init_fireball_flight. */
void start_fireball_flipped(void)
{
	OBJECT(current_object)->sprite_flags = 0x1044u;
	OBJECT(current_object)->sprite_palette = 0x10C0u;
	init_fireball_flight();
}

/* The fireballs machine_fire_update throws: sprite flags 0x1004, palette 0x9010, then
   init_fireball_flight. */
void fireball_start(void)
{
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	init_fireball_flight();
}

/* fireball_animation; flight_timer 0x36 frames (0x24 in stage 0xC), speed_x and speed_y to reach
   target_x, target_y in that time, the arc's base floor_z = z; sound 0xF, fireball_fly. */
void init_fireball_flight(void)
{
	uint8_t sound_id = 0;
	uint32_t flight_frames = 0, ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = fireball_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->init_fireball_flight.state = 0u;
	OBJECT(current_object)->animation_speed = 3u;
	OBJECT(current_object)->init_fireball_flight.flight_timer = 0x36;
	if (stage == 0xCu)
	{
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->init_fireball_flight.flight_timer = 0x24;
	}
	flight_frames = (uint16_t)OBJECT(current_object)->init_fireball_flight.flight_timer;
	OBJECT(current_object)->speed_x = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->init_fireball_flight.target_x << 16) - OBJECT(current_object)->x, flight_frames);
	OBJECT(current_object)->speed_y = divide_s32(((uint32_t)(uint16_t)OBJECT(current_object)->init_fireball_flight.target_y << 16) - OBJECT(current_object)->y, flight_frames);
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->direction = 0u;
	sound_id = 0xFu;
	play_sound(sound_id, &ignored, &ignored_2);
	OBJECT(current_object)->handler = fireball_fly_ADDRESS;
}

/* While flight_timer runs: moves by speed_x, speed_y on an arc, z = floor_z - sine_table[arc_angle] *
   0x140 (arc_angle + 9 a frame, 0xE in stage 0xC), drawn at its floor. Landed, it plays the rest of its
   animation, then next_handler takes over 0x28 higher. */
void fireball_fly(void)
{
	uint32_t arc_step = 0, at_last_step = 0, ignored = 0, ignored_2 = 0;
	uint16_t flight_timer = 0;

	switch (OBJECT(current_object)->fireball_fly.state)
	{
	case 0:
	case 2:
		break;
	case 1:
		at_last_step = animate_and_move(0u);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->handler = OBJECT(current_object)->next_handler;
			OBJECT(current_object)->fireball_fly.state = 0u;
			OBJECT(current_object)->z -= 0x280000u;
		}
		update_screen_position(&ignored, &ignored_2);
		ignored_2 = queue_sprite(ignored_2);
		return;
	default:
		invalid_jump(0x83AA5u + OBJECT(current_object)->fireball_fly.state * 2u);
	}
	OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	arc_step = 9u;
	if (stage == 0xCu)
		arc_step = 0xEu;
	OBJECT(current_object)->fireball_fly.arc_angle += arc_step;
	OBJECT(current_object)->z = sine_table[(uint16_t)OBJECT(current_object)->fireball_fly.arc_angle & 0x3FFu] * 0xFFFFFEC0u + OBJECT(current_object)->floor_z;
	animate_and_move(0u);
	flight_timer = OBJECT(current_object)->fireball_fly.flight_timer;
	OBJECT(current_object)->fireball_fly.flight_timer = flight_timer - 1u;
	if ((int16_t)flight_timer <= 1)
	{
		OBJECT(current_object)->fireball_fly.state++;
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	project_object(&ignored, &ignored_2);
	ignored_2 = queue_sprite_at_floor();
}

/* A big flame (flame_animation, palette 0x9010) set on its floor; flame_burn. */
void flame_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->handler = flame_burn_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Animated and drawn at screen_y + 0x30 until it has left the screen (check_left_screen), then removed. */
void flame_burn(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, off_screen = 0;

	off_screen = check_left_screen();
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		animate(&ignored_2, &ignored_3, &ignored_4);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x30u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* The flames that rise along the machine's fire: rising_flame_init, rising_flame_rise,
   rising_flame_spawn_next or rising_flame_wait_offscreen by state; removed when rising_fire_object
   reaches state 7 or off_screen is set. Animated, drawn at screen_y + 0x20 up to screen y 0x108. */
void rising_flame_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	switch (OBJECT(current_object)->rising_flame.state)
	{
	case 0:
		rising_flame_init();
		break;
	case 1:
		rising_flame_rise();
		break;
	case 2:
		rising_flame_spawn_next();
		break;
	case 3:
		rising_flame_wait_offscreen();
		break;
	case 4:
		rising_flame_init();
		break;
	default:
		invalid_jump(0x83BF7u + OBJECT(current_object)->rising_flame.state * 3u);
	}
	if (OBJECT(LINKED(current_object, rising_fire_object))->machine_fire.state == 7u || OBJECT(current_object)->rising_flame.off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	animate(&ignored, &ignored_2, &ignored_3);
	update_screen_position(&ignored_4, &ignored);
	if (OBJECT(current_object)->screen_y > 0x108)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* A rising flame: flame_animation, palette 0x9010 (0x9260 in stage 8), its base floor_z = z; next state. */
void rising_flame_init(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->rising_flame.state++;
	OBJECT(current_object)->sprite_flags = 0x1000u;
	OBJECT(current_object)->sprite_palette = 0x9010u;
	if (stage == 8u)
		OBJECT(current_object)->sprite_palette = 0x9260u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->animation = flame_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->rising_flame.off_screen = 0u;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	project_object(&ignored, &ignored_2);
}

/* z - 1 a frame until 0x28 above its base (floor_z); then the next state. */
void rising_flame_rise(void)
{
	OBJECT(current_object)->z -= 0x10000u;
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) >= 0x28)
		OBJECT(current_object)->rising_flame.state++;
}

/* While rising_fire_object climbs (state 2): once the fire's top (z - hitbox_height) is level with this
   flame or above it, another rising flame at the fire's x and top and this flame's y, and the next
   state. Without the climb, the next state at once. */
void rising_flame_spawn_next(void)
{
	uint32_t free_count = 0, object = 0, object_2 = 0, object_3 = 0;

	object = LINKED(current_object, rising_fire_object);
	if (OBJECT(object)->machine_fire.state != 2u)
	{
		OBJECT(current_object)->rising_flame.state++;
		return;
	}
	if (OBJECT(current_object)->z_int < (int16_t)(OBJECT(object)->z_int - OBJECT(object)->hitbox_height))
		return;
	allocate_object(&free_count, &object_2);
	if (free_count == 0u)
		return;
	OBJECT(object_2)->handler = rising_flame_update_ADDRESS;
	OBJECT(object_2)->rising_flame.state = 0u;
	object_3 = LINKED(current_object, rising_fire_object);
	OBJECT(object_2)->x = OBJECT(object_3)->x;
	OBJECT(object_2)->y = OBJECT(current_object)->y;
	OBJECT(object_2)->z = OBJECT(object_3)->z;
	OBJECT(object_2)->z_int -= OBJECT(object_3)->hitbox_height;
	OBJECT(current_object)->rising_flame.state++;
}

/* off_screen = 0xFF once the flame is below screen y 0x110 or left of screen x -0x40. */
void rising_flame_wait_offscreen(void)
{
	if (OBJECT(current_object)->screen_y >= 0x110 || OBJECT(current_object)->screen_x <= -0x40)
		OBJECT(current_object)->rising_flame.off_screen = 0xFFu;
}

/* A brick pillar sprite (0x6600, palette 0x8740) fixed at screen 0x146, 0x75, shown by
   stage_03_end_pillar_show; hands back the object (allocate_object_outside_loop). */
uint32_t spawn_stage_03_end_pillar(uint32_t pillar)
{
	uint32_t free_count = 0;

	allocate_object_outside_loop(&free_count, &pillar);
	if (free_count != 0u)
	{
		OBJECT(pillar)->sprite_flags = 0x1004u;
		OBJECT(pillar)->sprite_frame = 0x6600u;
		OBJECT(pillar)->sprite_palette = 0x8740u;
		OBJECT(pillar)->sprite_offset_x = 0;
		OBJECT(pillar)->sprite_offset_y = 0;
		OBJECT(pillar)->flags = 0u;
		OBJECT(pillar)->handler = stage_03_end_pillar_show_ADDRESS;
		OBJECT(pillar)->screen_x = 0x146;
		OBJECT(pillar)->screen_y = 0x75;
	}
	return pillar;
}

/* The pillar is drawn at screen_y + 0x20 until map_screen_active is set, then removed. */
void stage_03_end_pillar_show(void)
{
	uint32_t depth = 0, ignored = 0;

	if (map_screen_active == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Unused: a gate (sprite 0x6520) hung 0xE0 above its floor, exit_gate_update; clears sign_stop_flag and
   puts up spawn_sign_2 kind 8 (an IN sign with an arrow) at 0x942, 0x10E0, 0x50 above the floor. */
void exit_gate_start(uint32_t sign)
{
	uint16_t kind = 0, x = 0, y = 0, z = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	OBJECT(current_object)->sprite_flags = 0x1005u;
	OBJECT(current_object)->sprite_frame = 0x6520u;
	OBJECT(current_object)->sprite_palette = 0x86A0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->z = 0;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->handler = exit_gate_update_ADDRESS;
	sign_stop_flag = 0u;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &sign);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0xE0u;
	project_object(&ignored, &ignored_2);
	x = 0x942u;
	y = 0x10E0u;
	kind = 8u;
	z = (uint16_t)(OBJECT(current_object)->floor_z_int - 0x50u);
	spawn_sign_2(x, y, kind, z, sign);
}

/* Unused: removed while map_screen_active. Once the three players_finished bytes are FF with
   goal_unlocked still 0 (one 32-bit test) it falls (speed_z + 0x6000) to its floor: sign_stop_flag =
   0xFF, sound 0x20. Drawn with queue_sprite. */
void exit_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if (map_screen_active != 0u)
	{
		delete_current_object();
		return;
	}
	if ((uint32_t)OBJECT(current_object)->z != (uint32_t)OBJECT(current_object)->floor_z && (players_finished[0] | ((uint32_t)players_finished[1] << 8u) | ((uint32_t)players_finished[2] << 16u) | ((uint32_t)goal_unlocked << 24u)) == 0xFFFFFFu)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->floor_z_int <= OBJECT(current_object)->z_int)
		{
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
			sign_stop_flag = 0xFFu;
			sound_id = 0x20u;
			play_sound(sound_id, &ignored, &ignored_2);
		}
	}
	update_screen_position(&ignored_3, &ignored);
	ignored = queue_sprite(ignored);
}

/* Unused: the roller's ram target (sprite 0x6A58, palette 0x91A0): box 0x30 x 0x10, height 0x28, kind
   0x21, direction 0x300, ram_target_animation, damage_script = ram_target_damage_script, on its floor;
   ram_target_update. */
void ram_target_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->sprite_flags = 0x1045u;
	OBJECT(current_object)->sprite_frame = 0x6A58u;
	OBJECT(current_object)->sprite_palette = 0x91A0u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->handler = ram_target_update_ADDRESS;
	OBJECT(current_object)->sprite_offset_x = 6;
	OBJECT(current_object)->sprite_offset_y = 0x16;
	OBJECT(current_object)->hitbox_offset_x = -0x30;
	OBJECT(current_object)->hitbox_offset_y = -0x20;
	OBJECT(current_object)->hitbox_size_x = 0x30u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->hitbox_height = 0x28u;
	OBJECT(current_object)->kind = 0x21u;
	OBJECT(current_object)->direction = 0x300u;
	OBJECT(current_object)->ram_target.appear_state = 0;
	OBJECT(current_object)->ram_target.hit_count = 0;
	OBJECT(current_object)->animation = ram_target_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->ram_target.damage_script = ram_target_damage_script_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Unused: health 0x7F; removed once it has left the screen (margin 0x40); appear_state 0xFF once
   screen_x reaches 0x50; then ram_target_draw. It never calls ram_target_check_hits. */
void ram_target_update(void)
{
	uint16_t margin = 0;
	uint32_t off_screen = 0;

	OBJECT(current_object)->health = 0x7F;
	margin = 0x40u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state == 0u && OBJECT(current_object)->screen_x >= 0x50)
		OBJECT(current_object)->ram_target.appear_state = -1;
	ram_target_draw();
}

/* Unused: ram_target_update never calls it. On a roller_ram_hit frame hit_count + 1 and the animation
   restarts; at each threshold of damage_script the next animation; at 4 hits ram_target_burst. Then
   ram_target_draw. */
void ram_target_check_hits(void)
{
	uint32_t animation = 0, script_record = 0;

	if (roller_ram_hit != 0u)
	{
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->ram_target.hit_count++;
		if (RAM_TARGET_DAMAGE_STEP(OBJECT(current_object)->ram_target.damage_script)->hits <= OBJECT(current_object)->ram_target.hit_count)
		{
			script_record = OBJECT(current_object)->ram_target.damage_script + 2u;
			animation = RAM_TARGET_DAMAGE_STEP(script_record - 0x2u)->animation + ram_target_damage_script_ADDRESS;
			OBJECT(current_object)->animation = animation;
			OBJECT(current_object)->ram_target.damage_script = script_record + 2u;
			if (OBJECT(current_object)->ram_target.hit_count >= 4)
			{
				ram_target_burst(animation);
				ram_target_draw();
				return;
			}
		}
	}
	ram_target_draw();
}

/* Unused: once appear_state is set the animation plays once (then appear_state 1, animation_frame 0xFF);
   with appear_state 1-0x7F it is solid (add_to_zone_list). Drawn at screen_y - sprite_offset_y + 0x10,
   0x30 less before it appears. */
void ram_target_draw(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state != 0u && (int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		animate(&ignored, &ignored_2, &at_last_step);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->ram_target.appear_state = 1;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
	}
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state != 0u && OBJECT(current_object)->ram_target.appear_state >= 0)
		add_to_zone_list(0u);
	update_screen_position(&ignored_3, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y + 0x10u);
	if ((uint8_t)OBJECT(current_object)->ram_target.appear_state == 0u)
		depth = (uint16_t)(depth - 0x30u);
	queue_sprite_at_depth(&ignored_3, &depth);
}

/* Unused: the target bursts into ram_target_debris_count rock_debris_fly pieces, each ram_target_debris
   record giving its sprite flags, offset from the target's screen position, speeds and animation (in
   ram_target_debris_animations); palette 0x8240. */
void ram_target_burst(uint32_t piece)
{
	uint16_t screen_x = 0;
	uint32_t free_count = 0, piece_record = 0, pieces_left = 0;

	screen_x = (uint16_t)OBJECT(current_object)->screen_x;
	r14 = (uint16_t)OBJECT(current_object)->screen_y;
	pieces_left = ram_target_debris_count;
	piece_record = ram_target_debris_ADDRESS;
	do
	{
		allocate_object_outside_loop(&free_count, &piece);
		if (free_count == 0u)
			break;
		OBJECT(piece)->sprite_flags = ROCK_DEBRIS_PIECE(piece_record)->sprite_flags;
		OBJECT(piece)->screen_x = screen_x;
		piece_record += 2u;
		OBJECT(piece)->screen_x += ROCK_DEBRIS_PIECE(piece_record - 0x2u)->offset_x;
		OBJECT(piece)->screen_y = r14;
		piece_record += 2u;
		OBJECT(piece)->screen_y += ROCK_DEBRIS_PIECE(piece_record - 0x4u)->offset_y;
		piece_record += 2u;
		OBJECT(piece)->speed_x_fraction = ROCK_DEBRIS_PIECE(piece_record - 0x6u)->speed_x;
		piece_record += 2u;
		OBJECT(piece)->speed_y_fraction = ROCK_DEBRIS_PIECE(piece_record - 0x8u)->speed_y;
		piece_record += 2u;
		OBJECT(piece)->animation = ram_target_debris_animations_ADDRESS + ROCK_DEBRIS_PIECE(piece_record - 0xAu)->animation_offset;
		OBJECT(piece)->sprite_palette = 0x8240u;
		OBJECT(piece)->projected_x = (uint32_t)OBJECT(piece)->screen_x << 4;
		OBJECT(piece)->projected_y_fraction = (uint32_t)OBJECT(piece)->screen_y << 4;
		OBJECT(piece)->animation_frame = 0u;
		OBJECT(piece)->animation_speed = 4u;
		OBJECT(piece)->animation_timer = 0;
		OBJECT(piece)->flags = 0u;
		OBJECT(piece)->sprite_scale = 0x100u;
		OBJECT(piece)->handler = rock_debris_fly_ADDRESS;
		piece_record += 2u;
		pieces_left--;
	} while (pieces_left != 0u);
}

/* Unused: a multi-part sprite (flags 0xD006) of ram_target_part_list_2, or ram_target_part_list_3 when
   variant (the spawn's sprite_frame) is not 0, on its floor; ram_target_part_update. */
void ram_target_part_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, part_list = 0;

	OBJECT(current_object)->ram_target.variant = OBJECT(current_object)->sprite_frame;
	part_list = ram_target_part_list_2_ADDRESS;
	if (OBJECT(current_object)->sprite_frame != 0u)
		part_list = ram_target_part_list_3_ADDRESS;
	OBJECT(current_object)->part_list = part_list;
	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 2;
	OBJECT(current_object)->sprite_offset_y = 1;
	OBJECT(current_object)->ram_target.hit_count = 0;
	OBJECT(current_object)->handler = ram_target_part_update_ADDRESS;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	project_object(&ignored, &ignored_2);
}

/* Unused: on roller_ram_hit frames with chaser_object no more than 0x60 past its y, hit_count + 1; at 4
   variant 0 becomes the sprite 0x6A88 and the others are removed. Drawn at screen_y - 0x28; removed once
   it has left the screen (margin 0x88). */
void ram_target_part_update(void)
{
	uint16_t margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	if ((int16_t)(OBJECT(current_object)->y_int + 0x60u) < OBJECT(LINKED(current_object, chaser_object))->y_int || roller_ram_hit == 0u)
	{
		margin = 0x88u;
		off_screen = check_left_screen_margin(margin);
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &ignored_2);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x28u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	OBJECT(current_object)->ram_target.hit_count++;
	if (OBJECT(current_object)->ram_target.hit_count < 4)
	{
		margin = 0x88u;
		off_screen = check_left_screen_margin(margin);
		if (off_screen == 0u)
		{
			update_screen_position(&ignored, &ignored_2);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x28u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->ram_target.variant != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->sprite_flags = 0x1049u;
	OBJECT(current_object)->sprite_frame = 0x6A88u;
	OBJECT(current_object)->sprite_palette = 0x11B0u;
	OBJECT(current_object)->sprite_offset_x = 2;
	OBJECT(current_object)->sprite_offset_y = -0x19;
	margin = 0x88u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* fire_vent_cursor = fire_vent_list, then fire_vent_spawner_update from now on. */
void fire_vent_spawner_start(void)
{
	fire_vent_cursor = fire_vent_list_ADDRESS;
	OBJECT(current_object)->handler = fire_vent_spawner_update_ADDRESS;
	fire_vent_spawner_update();
}

/* When scroll_position reaches the next fire_vent_list record, a vent at its x, y, z with its
   blast_delay (sprite 0x6A48, box 0x90 x 0x48, height 0x28, kind 0x22, glow palettes, fire_vent_start);
   a negative delay ends the list and removes the spawner. */
void fire_vent_spawner_update(void)
{
	uint32_t free_count = 0, object = 0, vent_record = 0;
	uint16_t spawn_position = 0;

	vent_record = fire_vent_cursor;
	spawn_position = FIRE_VENT_SPAWN(vent_record)->position;
	vent_record += 2u;
	if (scroll_position < spawn_position)
		return;
	if (FIRE_VENT_SPAWN(vent_record - 0x2u)->blast_delay < 0)
	{
		delete_current_object();
		return;
	}
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x_int = FIRE_VENT_SPAWN(vent_record - 0x2u)->x;
		OBJECT(object)->x_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->y_int = FIRE_VENT_SPAWN(vent_record - 0x4u)->y;
		OBJECT(object)->y_fraction = 0u;
		vent_record += 2u;
		OBJECT(object)->z_int = FIRE_VENT_SPAWN(vent_record - 0x6u)->z;
		OBJECT(object)->z_fraction = 0u;
		OBJECT(object)->fire_vent.blast_delay = FIRE_VENT_SPAWN(vent_record - 0x6u)->blast_delay;
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_frame = 0x6A48u;
		OBJECT(object)->sprite_palette = 0x9110u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->handler = fire_vent_start_ADDRESS;
		OBJECT(object)->fire_vent.burst_state = 0u;
		OBJECT(object)->fire_vent.palette_list = fire_vent_glow_palettes_ADDRESS;
		OBJECT(object)->fire_vent.palette_index = 0u;
		OBJECT(object)->fire_vent.palette_delay = 8u;
		OBJECT(object)->fire_vent.palette_timer = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0x10;
		OBJECT(object)->kind = 0x22u;
		OBJECT(object)->hitbox_size_x = 0x90u;
		OBJECT(object)->hitbox_size_y = 0x48u;
		OBJECT(object)->hitbox_height = 0x28u;
		OBJECT(object)->hitbox_offset_x = -0x48;
		OBJECT(object)->hitbox_offset_y = -8;
		fire_vent_cursor += 0xAu;
	}
}

/* project_object, then fire_vent_update from now on. */
void fire_vent_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = fire_vent_update_ADDRESS;
	fire_vent_update();
}

/* Removed once it has left the screen (margin 0x40). The glow cycles (animate_sign), faster once
   blast_delay is 0x1E or less; blast_delay counts down while screen_y is 0x18 or more and at 0
   fire_vent_blast (-1 means blasted). Else fire_vent_draw. */
void fire_vent_update(void)
{
	uint32_t zero = 0, negative = 0, overflow = 0;
	uint16_t margin = 0;
	uint32_t off_screen = 0;
	uint16_t blast_delay = 0, next_delay = 0;

	margin = 0x40u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.palette_index >= 0)
	{
		if (OBJECT(current_object)->fire_vent.blast_delay <= 0x1E)
			OBJECT(current_object)->fire_vent.palette_delay = 2u;
		animate_sign();
	}
	if ((uint16_t)OBJECT(current_object)->fire_vent.blast_delay == 0u)
	{
		fire_vent_blast();
		return;
	}
	if (OBJECT(current_object)->screen_y < 0x18)
	{
		fire_vent_draw();
		return;
	}
	blast_delay = OBJECT(current_object)->fire_vent.blast_delay;
	next_delay = blast_delay - 1u;
	overflow = blast_delay == 0x8000u;
	zero = next_delay == 0u;
	negative = next_delay >> 15;
	OBJECT(current_object)->fire_vent.blast_delay = next_delay;
	if (blast_delay == 1u)
	{
		fire_vent_blast();
		return;
	}
	if (((negative ^ overflow) | zero) == 0u)
	{
		fire_vent_draw();
		return;
	}
	OBJECT(current_object)->fire_vent.blast_delay = -1;
	fire_vent_draw();
}

/* The vent blasts: blast_delay -1, burst_state 1, the dark grille (0x6A50, palette 0x9120), the glow
   stops (palette_index 0xFF); a fire_vent_burst_animate object anchored 0x3A, -0x31 off in projected x,
   y, and sound 0xA. Then fire_vent_draw. */
void fire_vent_blast(void)
{
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->fire_vent.blast_delay = -1;
	OBJECT(current_object)->fire_vent.burst_state = 1u;
	OBJECT(current_object)->sprite_frame = 0x6A50u;
	OBJECT(current_object)->sprite_palette = 0x9120u;
	OBJECT(current_object)->fire_vent.palette_index = 0xFFu;
	allocate_object(&free_count, &object);
	if (free_count == 0u)
	{
		fire_vent_draw();
		return;
	}
	OBJECT(object)->sprite_flags = 0x1009u;
	OBJECT(object)->sprite_palette = 0x90B0u;
	OBJECT(object)->sprite_offset_x = 0;
	OBJECT(object)->sprite_offset_y = 0;
	OBJECT(object)->flags = 0u;
	OBJECT(object)->handler = fire_vent_burst_animate_ADDRESS;
	OBJECT(object)->parent = current_object;
	OBJECT(object)->animation = fire_vent_blast_animation_ADDRESS;
	OBJECT(object)->animation_frame = 0u;
	OBJECT(object)->animation_speed = 3u;
	OBJECT(object)->animation_timer = 0;
	OBJECT(object)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object)->projected_x += 0x3Au;
	OBJECT(object)->anchor_x = OBJECT(object)->projected_x;
	OBJECT(object)->projected_y = OBJECT(current_object)->projected_y;
	OBJECT(object)->projected_y_int -= 0x31u;
	OBJECT(object)->anchor_y = OBJECT(object)->projected_y_int;
	OBJECT(object)->z = OBJECT(current_object)->z;
	sound_id = 0xAu;
	play_sound(sound_id, &ignored, &ignored_2);
	fire_vent_draw();
}

/* Draws the vent at screen_y - 0x48; touchable once it has blasted (burst_state set), and when
   burst_state is 0xFF (the burst is over) it turns to 1 and the vent to kind 0xC. */
void fire_vent_draw(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0;

	update_screen_position(&ignored, &ignored_2);
	if (OBJECT(current_object)->fire_vent.burst_state == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	if ((int8_t)OBJECT(current_object)->fire_vent.burst_state < 0)
	{
		OBJECT(current_object)->fire_vent.burst_state = 1u;
		OBJECT(current_object)->kind = 0xCu;
	}
	add_to_touchable_list(ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x48u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* The burst of fire: fire_vent_blast_animation, with fire_vent_blast_palettes' palette for each new
   step, drawn at screen_y + 0x68. At its last step it sets its vent's burst_state to 0xFF and is
   removed. */
void fire_vent_burst_animate(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)OBJECT(current_object)->animation_timer == OBJECT(current_object)->animation_speed)
		OBJECT(current_object)->sprite_palette = fire_vent_blast_palettes[OBJECT(current_object)->animation_frame];
	if ((uint8_t)at_last_step == 0u)
	{
		update_screen_position(&ignored_3, &ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x68u);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->fire_vent.burst_state = 0xFFu;
	delete_current_object();
}
