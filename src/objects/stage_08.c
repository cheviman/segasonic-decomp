/**
 *    Name:        objects/stage_08.c
 *    Description: stage 8's objects, the lava: the treadmill that raises a gate, the flame jets, the rock wall, the collapsing log bridge, the burning boulders and embers, the carriers and the seesaw, the metal platform and the fire wall, the fire trenches, the lava flow under the hanging bridge, the rock slabs and the lava columns
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Sets up the treadmill: the belt (sprite 0x72B0, kind 0x16, box 0x10 x 0xE9; a first 0x61 x 0xE9 box is
   overwritten at once), the roller (+0x27,+0x18, treadmill_roller_update), the gate's first half
   (+0x73,+0x31, treadmill_gate_update) and its other half (+0xC3,+0x59, treadmill_gate_half_update),
   each with a piece. */
void treadmill_init(void)
{
	uint16_t gate = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->sprite_frame = 0x72B0u;
	OBJECT(current_object)->sprite_palette = 0x1150u;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0xAu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x61u;
	OBJECT(current_object)->hitbox_size_y = 0xE9u;
	OBJECT(current_object)->hitbox_offset_x = -0x3D;
	OBJECT(current_object)->hitbox_offset_y = 0x4C;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0xE9u;
	OBJECT(current_object)->hitbox_offset_x = -0x15;
	OBJECT(current_object)->hitbox_offset_y = 0x4C;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x16u;
	OBJECT(current_object)->handler = treadmill_update_ADDRESS;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010946u, &ignored, &ignored_2, &free_count, &object, &block);
	OBJECT(object_cursor)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x27u;
	OBJECT(object)->projected_y_int += 0x18u;
	OBJECT(object)->sprite_frame = 0x72F0u;
	OBJECT(object)->sprite_palette = 0x1160u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x62u;
	OBJECT(object)->hitbox_size_y = 0x28u;
	OBJECT(object)->hitbox_offset_x = -0x3A;
	OBJECT(object)->hitbox_offset_y = -0xE;
	OBJECT(object)->hitbox_height = 0x14u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = treadmill_roller_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x7310u;
	OBJECT(object)->sprite_palette = 0x1170u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x0109D1u, &ignored, &ignored_2, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0x73u;
	OBJECT(object)->projected_y_int += 0x31u;
	OBJECT(object)->sprite_frame = 0x7340u;
	OBJECT(object)->sprite_palette = 0x1190u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x1Eu;
	OBJECT(object)->hitbox_size_y = 0x71u;
	OBJECT(object)->hitbox_offset_x = 0x11;
	OBJECT(object)->hitbox_offset_y = -0x5C;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = treadmill_gate_update_ADDRESS;
	gate = (uint16_t)object;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x6460u;
	OBJECT(object)->sprite_palette = 0x1170u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	r14 = object;
	spawn_count = 2u;
	spawn_object_with_pieces(0x010A5Au, &ignored, &ignored_2, &free_count, &object, &block);
	OBJECT(r14)->child = object;
	OBJECT(object)->parent = gate;
	copy_position_to(object);
	OBJECT(object)->projected_x += 0xC3u;
	OBJECT(object)->projected_y_int += 0x59u;
	OBJECT(object)->sprite_frame = 0x7340u;
	OBJECT(object)->sprite_palette = 0x1190u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x1Eu;
	OBJECT(object)->hitbox_size_y = 0x71u;
	OBJECT(object)->hitbox_offset_x = 0x11;
	OBJECT(object)->hitbox_offset_y = -0x5C;
	OBJECT(object)->hitbox_height = 0x50u;
	OBJECT(object)->kind = 0xCu;
	OBJECT(object)->handler = treadmill_gate_half_update_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0x6460u;
	OBJECT(object)->sprite_palette = 0x1170u;
	OBJECT(object)->sprite_palette |= 0x4000u;
}

/* The treadmill: the summed speed of the players in state 0x22 (treadmill_player_blocks), in whole
   pixels less spawn_arg_1, picks speed_z from treadmill_lift_forces (the gate's lift) and turns the belt
   (belt_phase, treadmill_belt_sprites). Drawn under everything (bucket 0xFF); in the zone list until the
   gate is up (flags bit 0). */
void treadmill_update(void)
{
	uint32_t belt_phase = 0, bucket = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, player_block = 0, player_index = 0, speed_level = 0, total_speed = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	OBJECT(current_object)->speed_z = 0;
	player_index = 2u;
	total_speed = 0u;
	do
	{
		player_block = GAME_VARIABLES + treadmill_player_blocks[player_index];
		if (PLAYER(player_block)->state == 0x22u)
			total_speed += OBJECT(LINKED(player_block, PLAYER(player_block)->object))->speed;
		player_index--;
	} while ((int32_t)player_index >= 0);
	speed_level = total_speed;
	if (speed_level == 0u)
	{
		bucket = 0xFFu;
		queue_sprite_in_bucket(bucket);
		if ((OBJECT(current_object)->flags & 1u) == 0u)
			add_to_zone_list(bucket);
		OBJECT(current_object)->flags &= ~0x8000000u;
		return;
	}
	speed_level = ((speed_level >> 24) & 0xFu) - OBJECT(current_object)->spawn_arg_1;
	if ((int32_t)speed_level < 0)
		speed_level = 0u;
	OBJECT(current_object)->speed_z = treadmill_lift_forces[speed_level];
	OBJECT(current_object)->treadmill_update.belt_phase += speed_level * 9u;
	OBJECT(current_object)->treadmill_update.belt_phase &= 0x7Fu;
	belt_phase = OBJECT(current_object)->treadmill_update.belt_phase;
	OBJECT(current_object)->sprite_frame = treadmill_belt_sprites[belt_phase >> 4];
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	if ((OBJECT(current_object)->flags & 1u) == 0u)
		add_to_zone_list(bucket);
	OBJECT(current_object)->flags &= ~0x8000000u;
}

/* The treadmill's roller: its frame and its piece's (0x10 right, 8 up) from treadmill_roller_sprites and
   treadmill_roller_part_sprites by the treadmill's belt_phase >> 5; touchable, kept at full health. */
void treadmill_roller_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	r14 = OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->treadmill_update.belt_phase >> 5;
	OBJECT(current_object)->sprite_frame = treadmill_roller_sprites[r14];
	queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = treadmill_roller_part_sprites[r14];
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += 0x10u;
	OBJECT(current_object)->screen_y -= 8u;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y + 2u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = object_cursor;
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(ignored);
}

/* The gate's first half: falls (fall_to_floor) with the treadmill's speed_z taken off (its lift) and
   stops 0x28 above its floor, where it sets the treadmill's flags bit 0 (sound 0x1E once); while moving,
   sounds 0x1E and 0xC. Its chain-top piece 0x63 above is drawn on top; touchable, kept at full health. */
void treadmill_gate_update(void)
{
	uint8_t sound_id = 0;
	uint32_t depth = 0, draw_bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0;
	uint8_t sound_1e_timer = 0, sound_c_timer = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	fall_to_floor(&ignored, &ignored_5, &ignored_2);
	OBJECT(current_object)->speed_z += 0u - OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->parent))->speed_z;
	if ((uint16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) != 0x28u)
	{
		if (OBJECT(current_object)->speed < OBJECT(current_object)->speed_z || (uint32_t)OBJECT(current_object)->speed_z == 0u)
		{
			if (OBJECT(current_object)->speed < OBJECT(current_object)->speed_z)
			{
				OBJECT(current_object)->speed = OBJECT(current_object)->speed_z;
				OBJECT(current_object)->speed += 0x3C000u;
				sound_1e_timer = OBJECT(current_object)->treadmill_gate_update.sound_1e_timer;
				OBJECT(current_object)->treadmill_gate_update.sound_1e_timer = sound_1e_timer - 1u;
				if (sound_1e_timer == 0u)
				{
					sound_id = 0x1Eu;
					play_sound(sound_id, &ignored_5, &ignored_2);
					OBJECT(current_object)->treadmill_gate_update.sound_1e_timer = 0x14u;
				}
			}
		}
		else
		{
			OBJECT(current_object)->speed = OBJECT(current_object)->speed_z;
			OBJECT(current_object)->speed -= 0x3C000u;
			sound_c_timer = OBJECT(current_object)->treadmill_gate_update.sound_c_timer;
			OBJECT(current_object)->treadmill_gate_update.sound_c_timer = sound_c_timer - 1u;
			if (sound_c_timer == 0u)
			{
				sound_id = 0xCu;
				play_sound(sound_id, &ignored_5, &ignored_2);
				OBJECT(current_object)->treadmill_gate_update.sound_c_timer = 0x3Cu;
			}
		}
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->parent);
	OBJECT(object)->flags &= ~1u;
	if ((int16_t)(OBJECT(current_object)->floor_z_int - OBJECT(current_object)->z_int) >= 0x28)
	{
		OBJECT(object)->flags |= 1u;
		if ((uint32_t)OBJECT(current_object)->speed_z != 0u && OBJECT(current_object)->treadmill_gate_update.reached_top == 0u)
		{
			sound_id = 0x1Eu;
			play_sound(sound_id, &ignored_5, &ignored_2);
			OBJECT(current_object)->treadmill_gate_update.reached_top = 1u;
		}
		OBJECT(current_object)->speed_z = 0;
	}
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y - 0x25u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y -= 0x63u;
	draw_bucket = 0u;
	queue_sprite_in_bucket(draw_bucket);
	current_object = object_cursor;
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(draw_bucket);
}

/* The gate's other half: takes the first half's z, with its chain-top piece 0x63 above drawn on top
   (bucket 0); touchable, kept at full health. */
void treadmill_gate_half_update(void)
{
	uint32_t depth = 0, draw_bucket = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->z = OBJECT(current_object)->z;
	current_object = object_cursor;
	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y - 0x25u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_y -= 0x63u;
	draw_bucket = 0u;
	queue_sprite_in_bucket(draw_bucket);
	current_object = object_cursor;
	OBJECT(current_object)->health = 0x7F;
	add_to_touchable_list(draw_bucket);
}

/* Sets up a flame jet: the nozzle (flame_jet_nozzle_sprites[spawn_arg_1], box 0x4C x 6, kind 0x14,
   direction spawn_arg_2) with a shadow piece, and a flame (0x6910, kind 0x14, angle 0x158, 0x350 with
   spawn_arg_1, flame_jet_pilot) with 9 pieces scaled 0x168 down by 0x18. The flame's start offset tests
   the shadow's spawn_arg_1. */
void flame_jet_init(void)
{
	uint8_t piece_frame = 0;
	uint16_t piece_scale = 0;
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->sprite_frame = flame_jet_nozzle_sprites[OBJECT(current_object)->spawn_arg_1];
	OBJECT(current_object)->sprite_palette = 0x9180u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x4Cu;
	OBJECT(current_object)->hitbox_size_y = 6u;
	OBJECT(current_object)->hitbox_offset_x = -0x1A;
	OBJECT(current_object)->hitbox_offset_y = -2;
	OBJECT(current_object)->hitbox_height = 0x20u;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->direction = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->handler = flame_jet_nozzle_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x6990u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	OBJECT(current_object)->sprite_palette |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0xAu;
	if (OBJECT(object_cursor)->spawn_arg_1 != 0u)
	{
		OBJECT(current_object)->sprite_flags = 5u;
		OBJECT(current_object)->sprite_flags |= 0x40u;
		OBJECT(current_object)->sprite_flags |= 0x80u;
	}
	spawn_count = 0xAu;
	spawn_object_with_pieces(0x010E60u, &ignored, &ignored_2, &free_count, &object, &block);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	OBJECT(current_object)->child = object;
	copy_position_to(object);
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->projected_x -= 0x26u;
		OBJECT(object)->projected_y_int -= 0x37u;
	}
	OBJECT(object)->sprite_scale = 0x100u;
	OBJECT(object)->spawn_arg_2 = 0x180u;
	OBJECT(object)->sprite_flags = 0u;
	OBJECT(object)->sprite_frame = 0x6910u;
	OBJECT(object)->sprite_palette = 0x7B0u;
	OBJECT(object)->sprite_palette |= 0x4000u;
	OBJECT(object)->flags |= 2u;
	OBJECT(object)->hitbox_size_x = 0x3Cu;
	OBJECT(object)->hitbox_size_y = 6u;
	OBJECT(object)->hitbox_offset_x = 0x10;
	OBJECT(object)->hitbox_offset_y = -0x22;
	OBJECT(object)->hitbox_height = 0x20u;
	OBJECT(object)->kind = 0x14u;
	OBJECT(object)->spawn_arg_1 = OBJECT(object_cursor)->spawn_arg_1;
	OBJECT(object)->flame_jet.growth_rate = 0x200;
	OBJECT(object)->flame_jet.growth_rate_high = 0u;
	OBJECT(current_object)->direction = OBJECT(object_cursor)->spawn_arg_2;
	OBJECT(object)->flame_jet.angle = 0x158u;
	OBJECT(object)->handler = flame_jet_pilot_ADDRESS;
	if (OBJECT(object)->spawn_arg_1 != 0u)
	{
		OBJECT(object)->flame_jet.angle = 0x350u;
		OBJECT(object)->sprite_flags = 7u;
	}
	r14 = object;
	piece_scale = 0x180u;
	piece_frame = 0u;
	for (;;)
	{
		object = LINKED(object, OBJECT(object)->child);
		if ((uint16_t)object == 0xFFFFu)
			break;
		OBJECT(object)->sprite_frame = 0x6910u;
		OBJECT(object)->sprite_palette = 0x7B0u;
		OBJECT(object)->sprite_palette |= 0x4000u;
		OBJECT(object)->sprite_flags = OBJECT(r14)->sprite_flags;
		piece_frame = (uint8_t)((uint8_t)(piece_frame + 1u) & 3u);
		OBJECT(object)->piece_frame = piece_frame;
		piece_scale -= 0x18u;
		OBJECT(object)->sprite_scale = piece_scale;
	}
}

/* Draws a flame jet's nozzle, removed off screen, and its shadow piece under everything (bucket 0xFF).
   With spawn_arg_1 0 the nozzle is the touchable part, its hitbox_size_y 1.5 times the flame front's
   screen distance; with spawn_arg_1 set it is drawn on top (bucket 0) and the flame carries the touch
   box. */
void flame_jet_nozzle_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		bucket = 0u;
		queue_sprite_in_bucket(bucket);
	}
	else
	{
		queue_sprite(0u);
		OBJECT(current_object)->health = 0x7F;
		current_object = LINKED(current_object, OBJECT(object_cursor)->child);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		r14 = (uint16_t)((OBJECT(current_object)->projected_x - OBJECT(object_cursor)->projected_x) * 3u) >> 1;
		current_object = object_cursor;
		OBJECT(current_object)->hitbox_size_y = r14;
		add_to_touchable_list(deleted);
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->spawn_arg_1 != 0u)
	{
		offset_x = 3u;
		offset_y = 0x19u;
	}
	else
	{
		offset_x = 1u;
		offset_y = 1u;
	}
	place_sprite_near_cursor(offset_x, offset_y);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* The pilot flame: until stage_trigger_bits is set, speed grows to about 0x666 and shrinks to 0 again
   (growth_rate 0x80 and -0x80) through flame_jet_burn. Then sprite_scale = spawn_arg_2 (0x180),
   growth_rate 0x380 and flame_jet_ignite. */
void flame_jet_pilot(void)
{
	if (stage_trigger_bits != 0u)
	{
		OBJECT(current_object)->sprite_scale = OBJECT(current_object)->spawn_arg_2;
		OBJECT(current_object)->flame_jet.growth_rate = 0x380;
		OBJECT(current_object)->handler = flame_jet_ignite_ADDRESS;
		flame_jet_ignite();
		return;
	}
	if (OBJECT(current_object)->flame_jet.growth_rate < 0)
	{
		if (OBJECT(current_object)->speed > 0)
		{
			flame_jet_burn();
			return;
		}
		OBJECT(current_object)->flame_jet.growth_rate = 0x80;
		OBJECT(current_object)->speed = 0;
		flame_jet_burn();
		return;
	}
	if (OBJECT(current_object)->speed <= 0x666)
	{
		flame_jet_burn();
		return;
	}
	OBJECT(current_object)->flame_jet.growth_rate = -0x80;
	if (OBJECT(current_object)->speed > 0)
	{
		flame_jet_burn();
		return;
	}
	OBJECT(current_object)->flame_jet.growth_rate = 0x80;
	OBJECT(current_object)->speed = 0;
	flame_jet_burn();
}

/* The jet fires: flame_jet_burn while speed grows (growth_rate 0x380) up to 0xF000, then growth_rate 0
   and flame_jet_burn as the handler. */
void flame_jet_ignite(void)
{
	if (OBJECT(current_object)->speed <= 0xF000)
	{
		flame_jet_burn();
		return;
	}
	OBJECT(current_object)->flame_jet.growth_rate = 0;
	OBJECT(current_object)->handler = flame_jet_burn_ADDRESS;
	flame_jet_burn();
}

/* The flame of a flame jet: its front sits at the nozzle + speed along angle (speed grows by
   growth_rate), flickering through flame_jet_flame_sprites; with spawn_arg_1 set it is the touchable
   part (hitbox_size_y = its screen distance from the nozzle). Once stage_trigger_bits is set its 9
   pieces fill the line back to the nozzle. */
void flame_jet_burn(void)
{
	uint16_t flame_step_y = 0, offset_x = 0, offset_y = 0, piece_number = 0;
	uint32_t depth = 0, flame_angle = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t projected_x = 0, nozzle_x = 0, projected_y_int = 0, nozzle_y = 0;

	OBJECT(current_object)->speed += OBJECT(current_object)->flame_jet.growth_rate;
	flame_angle = OBJECT(current_object)->flame_jet.angle;
	speed = OBJECT(current_object)->speed;
	direction_to_speed(speed, &flame_angle, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	OBJECT(object_cursor)->projected_x = OBJECT(current_object)->projected_x;
	OBJECT(object_cursor)->projected_y = OBJECT(current_object)->projected_y;
	current_object = object_cursor;
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		OBJECT(current_object)->projected_x += 0xCu;
		OBJECT(current_object)->projected_y_int += 6u;
	}
	else
	{
		OBJECT(current_object)->projected_x -= 0x16u;
		OBJECT(current_object)->projected_y_int += 0xDu;
	}
	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->flame_jet.frame_phase++;
	OBJECT(current_object)->flame_jet.frame_phase &= 3u;
	OBJECT(current_object)->sprite_frame = flame_jet_flame_sprites[(uint8_t)OBJECT(current_object)->flame_jet.frame_index];
	OBJECT(current_object)->flame_jet.frame_index++;
	if (OBJECT(current_object)->flame_jet.frame_index > 4)
		OBJECT(current_object)->flame_jet.frame_index = 0;
	depth = (uint16_t)OBJECT(current_object)->screen_y;
	OBJECT(current_object)->screen_y -= 0x28u;
	queue_sprite_at_depth(&ignored, &depth);
	if (OBJECT(current_object)->spawn_arg_1 != 0u)
	{
		current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
		r14 = (uint16_t)(OBJECT(current_object)->projected_x - OBJECT(object_cursor)->projected_x);
		current_object = object_cursor;
		OBJECT(current_object)->health = 0x7F;
		OBJECT(current_object)->hitbox_size_y = r14;
		add_to_touchable_list(ignored);
	}
	if (stage_trigger_bits == 0u)
		return;
	current_object = LINKED(current_object, OBJECT(object_cursor)->parent);
	r14 = (uint16_t)OBJECT(current_object)->projected_x;
	flame_step_y = (uint16_t)OBJECT(current_object)->projected_y_int;
	current_object = object_cursor;
	if (OBJECT(current_object)->spawn_arg_1 == 0u)
	{
		r14 = (uint16_t)(r14 - 6u);
		flame_step_y += 5u;
	}
	else
		flame_step_y += 0x18u;
	projected_x = OBJECT(object_cursor)->projected_x;
	nozzle_x = r14;
	r14 = (uint16_t)(nozzle_x - projected_x);
	if (nozzle_x != projected_x)
		r14 = (uint16_t)((int16_t)r14 / 0xA);
	projected_y_int = OBJECT(object_cursor)->projected_y_int;
	nozzle_y = flame_step_y;
	flame_step_y = (uint16_t)(nozzle_y - projected_y_int);
	if (nozzle_y != projected_y_int)
		flame_step_y = (uint16_t)((int16_t)flame_step_y / 0xA);
	current_object = LINKED(current_object, OBJECT(object_cursor)->child);
	piece_number = 1u;
	do
	{
		OBJECT(current_object)->sprite_frame = OBJECT(object_cursor)->sprite_frame;
		offset_x = (uint16_t)(r14 * piece_number);
		offset_y = (uint16_t)((uint32_t)flame_step_y * piece_number);
		place_sprite_near_cursor(offset_x, offset_y);
		OBJECT(current_object)->direction++;
		OBJECT(current_object)->direction &= 3u;
		OBJECT(current_object)->sprite_frame = flame_jet_flame_sprites[(uint8_t)OBJECT(current_object)->piece_frame];
		OBJECT(current_object)->piece_frame++;
		if (OBJECT(current_object)->piece_frame > 4)
			OBJECT(current_object)->piece_frame = 0;
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		piece_number++;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
	} while ((uint16_t)current_object != 0xFFFFu);
}

/* Sets up a rock wall (sprite 0x7C18, palette 0x87C0): box 0x18 x 0x4D, height 0xB4, kind 0xC, its two
   sparkle parts in palette 0x8A10; then rock_wall_update. */
void rock_wall_init(void)
{
	OBJECT(current_object)->sprite_frame = 0x7C18u;
	OBJECT(current_object)->sprite_palette = 0x87C0u;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0x18u;
	OBJECT(current_object)->hitbox_size_y = 0x4Du;
	OBJECT(current_object)->hitbox_offset_x = 9;
	OBJECT(current_object)->hitbox_offset_y = -0x39;
	OBJECT(current_object)->hitbox_height = 0xB4u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->rock_wall.frame_count = 2;
	OBJECT(current_object)->handler = rock_wall_update_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8A10u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = 0x8A10u;
}

/* A rock wall: touched by an attacking player (the toucher's flag 0x40) it crumbles open, a
   rock_wall_break_frames frame every 3 frames, dropping spawn_arg_1 rings at angle spawn_arg_2 at the
   fifth. While whole its two parts sparkle and glint, pausing twinkle_delay (0x1E) frames after each
   glint. */
void rock_wall_update(void)
{
	uint8_t bucket = 0;
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t cursor = 0, deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, random_value = 0, ring = 0, slope_height = 0;
	uint16_t twinkle_delay = 0, direction = 0, direction_2 = 0;

	unproject_object(&ignored, &ignored_2, &slope_height, &cursor);
	delete_if_off_screen(&deleted, &ignored_2, &slope_height);
	if ((uint8_t)deleted == 1u)
		return;
	if (OBJECT(current_object)->rock_wall.breaking != 0u)
	{
		if (OBJECT(current_object)->rock_wall.break_frame < 0xD)
		{
			OBJECT(current_object)->rock_wall.frame_count++;
			if (OBJECT(current_object)->rock_wall.frame_count > 2)
			{
				OBJECT(current_object)->rock_wall.break_frame++;
				OBJECT(current_object)->sprite_frame = rock_wall_break_frames[(uint8_t)OBJECT(current_object)->rock_wall.break_frame];
				OBJECT(current_object)->rock_wall.frame_count = 0;
				if ((uint8_t)OBJECT(current_object)->rock_wall.break_frame == 5u && OBJECT(current_object)->spawn_arg_1 != 0u)
				{
					rings_to_release = OBJECT(current_object)->spawn_arg_1;
					released_ring_angle = OBJECT(current_object)->spawn_arg_2;
					released_ring_flip = 0u;
					released_rings_random = 0u;
					drop_rings(slope_height, &cursor, &random_value, &ring);
				}
			}
			add_to_touchable_list(deleted);
		}
	}
	else if ((OBJECT(current_object)->flags & 4u) == 0u || (OBJECT(LINKED(WORK_RAM, OBJECT(current_object)->touched_by))->flags & 0x40u) == 0u)
		add_to_touchable_list(deleted);
	else
	{
		OBJECT(current_object)->rock_wall.breaking = 1u;
		if (OBJECT(current_object)->rock_wall.break_frame < 0xD)
		{
			OBJECT(current_object)->rock_wall.frame_count++;
			if (OBJECT(current_object)->rock_wall.frame_count > 2)
			{
				OBJECT(current_object)->rock_wall.break_frame++;
				OBJECT(current_object)->sprite_frame = rock_wall_break_frames[(uint8_t)OBJECT(current_object)->rock_wall.break_frame];
				OBJECT(current_object)->rock_wall.frame_count = 0;
				if ((uint8_t)OBJECT(current_object)->rock_wall.break_frame == 5u && OBJECT(current_object)->spawn_arg_1 != 0u)
				{
					rings_to_release = OBJECT(current_object)->spawn_arg_1;
					released_ring_angle = OBJECT(current_object)->spawn_arg_2;
					released_ring_flip = 0u;
					released_rings_random = 0u;
					drop_rings(slope_height, &cursor, &random_value, &ring);
				}
			}
			add_to_touchable_list(deleted);
		}
	}
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
	if (OBJECT(current_object)->rock_wall.break_frame < 1)
	{
		twinkle_delay = OBJECT(current_object)->rock_wall.twinkle_delay;
		OBJECT(current_object)->rock_wall.twinkle_delay = twinkle_delay - 1u;
		if (twinkle_delay != 0u)
			return;
		OBJECT(current_object)->rock_wall.twinkle_delay = 0u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 2u;
			OBJECT(current_object)->sprite_frame = rock_wall_sparkle_frames[(uint8_t)OBJECT(current_object)->piece_frame];
			OBJECT(current_object)->piece_frame++;
			if (OBJECT(current_object)->piece_frame > 0x19)
				OBJECT(current_object)->piece_frame = 0;
		}
		offset_x = 8u;
		offset_y = 0xFFEBu;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 1u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		direction_2 = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction_2 - 1u;
		if (direction_2 == 0u)
		{
			OBJECT(current_object)->direction = 4u;
			OBJECT(current_object)->sprite_frame = rock_wall_glint_frames[rock_wall_glint_steps[(uint8_t)OBJECT(current_object)->piece_frame]];
			OBJECT(current_object)->piece_frame++;
			if (OBJECT(current_object)->piece_frame > 0x13)
			{
				OBJECT(current_object)->piece_frame = 0;
				OBJECT(object_cursor)->rock_wall.twinkle_delay = 0x1Eu;
			}
		}
		offset_x = 5u;
		offset_y = 0xFFD1u;
		place_sprite_near_cursor(offset_x, offset_y);
		depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 1u);
		queue_sprite_at_depth(&ignored, &depth);
	}
}

/* State 1: a sagging plank, a platform, until event_1c_riding (plank 0: sound 9); 2: it hangs from the
   previous plank at its angle + bridge_swing_angle (not below 0x200). Plank 0 falls from 0x300 to 0x200
   and swings with decay (sound 0x20 a swing); after 6 it stops, clears event_1c_ride_phase and shows
   sign 0x16. Removed at scroll 0x9000. */
void collapsing_bridge_plank_update(void)
{
	uint8_t sound_id = 0, start_frame = 0;
	uint16_t kind = 0, x = 0, y = 0, z = 0;
	uint32_t angle_whole = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0, plank_index = 0, sag = 0, sag_table = 0, sign = 0, swing_dx = 0, swing_dz = 0;

	switch (OBJECT(current_object)->collapsing_bridge_plank.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->collapsing_bridge_plank.state++;
		OBJECT(current_object)->collapsing_bridge_plank.angle = 0x3000000u;
		OBJECT(current_object)->sprite_offset_x = -4;
		OBJECT(current_object)->sprite_offset_y = -4;
		OBJECT(current_object)->collapsing_bridge_plank.angle_speed = 0;
		OBJECT(current_object)->collapsing_bridge_plank.swing_count = 0u;
		OBJECT(current_object)->collapsing_bridge_plank.plank_index--;
		OBJECT(current_object)->collapsing_bridge_plank.roll_timer = 0u;
		OBJECT(current_object)->collapsing_bridge_plank.roll_frames = OBJECT(current_object)->collapsing_bridge_plank.plank_index * 2u;
		OBJECT(current_object)->animation = collapsing_bridge_plank_animation_ADDRESS;
		start_frame = OBJECT(current_object)->collapsing_bridge_plank.plank_index & 7u;
		OBJECT(current_object)->animation_frame = start_frame;
		OBJECT(current_object)->animation_speed = 3u;
		OBJECT(current_object)->animation_timer = start_frame & 3u;
		if (OBJECT(current_object)->collapsing_bridge_plank.plank_index == 0u)
		{
			bridge_swing_angle = 0;
			bridge_swing_speed = 0;
		}
		project_object(&ignored, &ignored_2);
		return;
	case 1:
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		plank_index = OBJECT(current_object)->collapsing_bridge_plank.plank_index;
		sag_table = bridge_0_sag_ADDRESS;
		if (OBJECT(current_object)->collapsing_bridge_plank.bridge != 0u)
			sag_table = bridge_1_sag_ADDRESS;
		sag = mem_read16(sag_table + plank_index * 2u);
		if ((sag & ~0xFFu) == 0u)
			sag = 0u;
		OBJECT(current_object)->previous_floor_z += sag << 8;
		OBJECT(current_object)->speed_x += (int32_t)((uint32_t)OBJECT(current_object)->previous_floor_z - OBJECT(current_object)->speed_x) >> 2;
		OBJECT(current_object)->z_int = OBJECT(current_object)->speed_x_int;
		update_screen_position(&ignored, &ignored_2);
		if (event_1c_riding != 0u)
		{
			OBJECT(current_object)->collapsing_bridge_plank.state++;
			if (OBJECT(current_object)->collapsing_bridge_plank.plank_index == 0u)
			{
				sound_id = 9u;
				play_sound(sound_id, &ignored_2, &ignored_3);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0x50u) >= 0x1B0u)
			return;
		add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->collapsing_bridge_plank.depth_offset);
		OBJECT(current_object)->draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		if (scroll_position >= 0x9000u)
		{
			delete_current_object();
			return;
		}
		if (OBJECT(current_object)->collapsing_bridge_plank.plank_index != 0u)
		{
			object = LINKED(current_object, OBJECT(current_object)->collapsing_bridge_plank.previous_plank);
			OBJECT(current_object)->collapsing_bridge_plank.angle = OBJECT(object)->collapsing_bridge_plank.angle;
			OBJECT(current_object)->collapsing_bridge_plank.angle += bridge_swing_angle;
			OBJECT(current_object)->collapsing_bridge_plank.angle_whole &= 0x3FFu;
			if (OBJECT(current_object)->collapsing_bridge_plank.angle_whole < 0x200)
				OBJECT(current_object)->collapsing_bridge_plank.angle = 0x2000000u;
			angle_whole = (uint16_t)OBJECT(current_object)->collapsing_bridge_plank.angle_whole;
			swing_dx = sine_table[angle_whole] * 0x40u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->x += swing_dx;
			swing_dz = cosine_table[angle_whole] * 0x40u;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->z -= swing_dz;
			OBJECT(current_object)->flags_low &= ~1u;
			OBJECT(current_object)->flags_low |= OBJECT(object)->flags_low & 1u;
			OBJECT(current_object)->collapsing_bridge_plank.swing_count = OBJECT(object)->collapsing_bridge_plank.swing_count;
		}
		else if ((OBJECT(current_object)->flags & 1u) == 0u)
		{
			if (OBJECT(current_object)->collapsing_bridge_plank.swing_count != 0u)
			{
				bridge_swing_speed -= 0x200u;
				bridge_swing_angle += bridge_swing_speed;
				if (bridge_swing_speed_int < 0 && bridge_swing_angle <= 0)
				{
					OBJECT(current_object)->collapsing_bridge_plank.swing_count++;
					if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count >= 6)
					{
						OBJECT(current_object)->flags |= 1u;
						event_1c_ride_phase = 0u;
						x = 0xBA0u;
						y = 0xEA4u;
						kind = 0x16u;
						z = 0x660u;
						spawn_sign(x, y, kind, z, &ignored_2, &sign);
					}
					bridge_swing_speed = 0u - bridge_swing_speed;
					bridge_swing_speed = (int32_t)(bridge_swing_speed * 0x5Eu) >> 8;
					sound_id = 0x20u;
					play_sound(sound_id, &ignored_2, &ignored_3);
				}
			}
			else
			{
				OBJECT(current_object)->collapsing_bridge_plank.angle_speed -= 0x3000u;
				OBJECT(current_object)->collapsing_bridge_plank.angle += OBJECT(current_object)->collapsing_bridge_plank.angle_speed;
				bridge_head_angle = OBJECT(current_object)->collapsing_bridge_plank.angle_whole;
				if (OBJECT(current_object)->collapsing_bridge_plank.angle_whole <= 0x200)
				{
					OBJECT(current_object)->collapsing_bridge_plank.angle = 0x2000000u;
					OBJECT(current_object)->collapsing_bridge_plank.angle_speed = 0;
					OBJECT(current_object)->collapsing_bridge_plank.swing_count++;
					bridge_swing_speed = -0x10000;
				}
				bridge_swing_angle += 0x2000u;
			}
		}
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 3)
			animate(&ignored_2, &ignored_4, &ignored_5);
		else
		{
			if ((int16_t)OBJECT(current_object)->collapsing_bridge_plank.roll_timer < (int16_t)OBJECT(current_object)->collapsing_bridge_plank.roll_frames)
			{
				OBJECT(current_object)->collapsing_bridge_plank.roll_timer++;
				animate(&ignored_2, &ignored_4, &ignored_5);
				project_object(&ignored, &ignored_2);
				if (OBJECT(current_object)->screen_y > 0xE0)
					return;
				if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
				{
					ignored_2 = queue_sprite(ignored_2);
					return;
				}
				depth = 0u;
				queue_sprite_at_depth(&ignored, &depth);
				return;
			}
			OBJECT(current_object)->sprite_frame = mem_read16(OBJECT(current_object)->animation + 2u);
		}
		project_object(&ignored, &ignored_2);
		if (OBJECT(current_object)->screen_y > 0xE0)
			return;
		if ((int8_t)OBJECT(current_object)->collapsing_bridge_plank.swing_count < 1)
		{
			ignored_2 = queue_sprite(ignored_2);
			return;
		}
		depth = 0u;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	default:
		invalid_jump(0x85F8Eu + OBJECT(current_object)->collapsing_bridge_plank.state * 3u);
	}
}

/* Sets up a burning boulder from burning_boulder_records[sprite_frame]: x, start_delay and path
   (0xFFFFFFFF: the follower, 0x120 wide, spawn_chaser_back_zone 2; else lead_burning_boulder); y 0x3E0 -
   0x60 * sprite_frame, box 0x90 x 0x90, kind 0x14, 0x70 above its floor; four flames chained through
   child, then burning_boulder_update. */
void burning_boulder_start(void)
{
	uint32_t boulder_record = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, object_2 = 0, zone_index = 0;
	uint8_t flames_left_before = 0;

	boulder_record = burning_boulder_records_ADDRESS + OBJECT(current_object)->sprite_frame * 2u * 4u;
	OBJECT(current_object)->x_int = BURNING_BOULDER_RECORD(boulder_record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	boulder_record += 2u;
	OBJECT(current_object)->burning_boulder.start_delay = BURNING_BOULDER_RECORD(boulder_record - 0x2u)->start_delay;
	OBJECT(current_object)->burning_boulder.path = BURNING_BOULDER_RECORD(boulder_record - 0x2u)->path;
	OBJECT(current_object)->y = 0x3E00000;
	OBJECT(current_object)->y_int -= OBJECT(current_object)->sprite_frame * 0x60u;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_frame = 0xAA58u;
	OBJECT(current_object)->sprite_palette = 0x51B0u;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x90u;
	OBJECT(current_object)->hitbox_size_y = 0x90u;
	OBJECT(current_object)->burning_boulder.state = 0u;
	OBJECT(current_object)->burning_boulder.depth_offset = 0x20u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->direction = 0x200u;
	OBJECT(current_object)->hitbox_height = 0xA0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x70;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->burning_boulder.rumble_timer = 0x3Cu;
	burning_boulder_released = 0u;
	burning_boulder_release_timer = 0;
	OBJECT(current_object)->animation = burning_boulder_roll_animation_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	if ((uint16_t)OBJECT(current_object)->burning_boulder.path == 0xFFFFu)
	{
		OBJECT(current_object)->hitbox_size_x = 0x120u;
		zone_index = 2u;
		object = spawn_chaser_back_zone(zone_index, 0u);
	}
	if ((uint16_t)OBJECT(current_object)->burning_boulder.path != 0xFFFFu)
		lead_burning_boulder = current_object;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->z_int -= 0x70u;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->burning_boulder.holding_back = 0u;
	OBJECT(current_object)->burning_boulder.wait_timer = 0;
	if (free_object_stack < 4)
		return;
	boulder_flames_left = 4;
	boulder_flame_number = 0u;
	object_2 = current_object;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x100Au;
		OBJECT(object)->sprite_palette = 0x51C0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->kind = boulder_flame_number;
		boulder_flame_number++;
		OBJECT(object)->handler = burning_boulder_flame_start_ADDRESS;
		OBJECT(object)->burning_boulder_flame.boulder = current_object;
		OBJECT(object_2)->child = object;
		object_2 = object;
		flames_left_before = boulder_flames_left;
		boulder_flames_left = flames_left_before - 1u;
	} while ((int8_t)flames_left_before > 1);
	OBJECT(current_object)->handler = burning_boulder_update_ADDRESS;
}

/* State 0 waits start_delay and falls to its floor (shake, sound 7); 1 the leader rolls along y as a
   chaser (path records, sound 0xD) until it has no floor past scroll 0x3300; 2 the follower rolls at the
   leader's speed; 3 falls away, flames and follower removed; 4 waits for burning_boulder_released; 5
   drops onto the seesaw, seesaw_launched from z 0x9E0. */
void burning_boulder_update(void)
{
	uint32_t zero = 0;
	uint8_t sound_id = 0;
	uint16_t animation_speed = 0, roll_speed = 0;
	uint32_t depth = 0, drop_step = 0, flames_left = 0, floor_slope = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, object = 0, path_cursor = 0, slope_factor = 0, speed_level = 0, step_y = 0;
	uint8_t hold_timer = 0;
	uint16_t start_delay = 0, wait_timer = 0, record_scroll = 0, record_y = 0, next_slow_timer = 0, rumble_before = 0, rumble_after = 0, start_delay_2 = 0;
	uint32_t rumble_timer_halfword = 0;

	switch (OBJECT(current_object)->burning_boulder.state)
	{
	case 0:
	case 6:
		start_delay = OBJECT(current_object)->burning_boulder.start_delay;
		OBJECT(current_object)->burning_boulder.start_delay = start_delay - 1u;
		if ((int16_t)start_delay > 1)
			return;
		OBJECT(current_object)->burning_boulder.start_delay = 0;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
		{
			project_object(&ignored, &ignored_2);
			add_to_zone_list(ignored);
			depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
			if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
				depth = (uint16_t)(depth - 0x20u);
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		OBJECT(current_object)->burning_boulder.state++;
		if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
			OBJECT(current_object)->burning_boulder.state = 2u;
		camera_shake_active = 1u;
		sound_id = 7u;
		play_sound(sound_id, &ignored_2, &ignored_3);
		project_object(&ignored, &ignored_2);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		if (OBJECT(current_object)->burning_boulder.path_high == 0xFFFFu)
			depth = (uint16_t)(depth - 0x20u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		if (OBJECT(current_object)->burning_boulder.holding_back == 0u)
		{
			if (no_player_in_play != 0u || player_mishap != 0u || player_dropping_in != 0u)
			{
				OBJECT(current_object)->burning_boulder.holding_back = 0xFFu;
				OBJECT(current_object)->burning_boulder.saved_base_speed = OBJECT(current_object)->burning_boulder.base_speed;
				OBJECT(current_object)->burning_boulder.base_speed = 0;
				OBJECT(current_object)->burning_boulder.hold_start_position = scroll_position;
				OBJECT(current_object)->burning_boulder.hold_timer = 0xFFu;
				OBJECT(current_object)->burning_boulder.saved_threshold = OBJECT(current_object)->burning_boulder.screen_threshold;
				OBJECT(current_object)->burning_boulder.screen_threshold = -0x20;
			}
			else
			{
				wait_timer = OBJECT(current_object)->burning_boulder.wait_timer;
				OBJECT(current_object)->burning_boulder.wait_timer = wait_timer - 1u;
				if ((int16_t)wait_timer <= 1)
				{
					OBJECT(current_object)->burning_boulder.wait_timer = 0;
					path_cursor = OBJECT(current_object)->burning_boulder.path;
					record_scroll = CHASE_STEP_WITH_LIMIT(path_cursor)->position;
					path_cursor += 2u;
					if (scroll_position < record_scroll)
					{
						record_y = CHASE_STEP_WITH_LIMIT(path_cursor - 0x2u)->limit;
						path_cursor += 2u;
						if ((int16_t)record_y <= OBJECT(current_object)->y_int)
						{
							OBJECT(current_object)->burning_boulder.screen_threshold = CHASE_STEP_WITH_LIMIT(path_cursor - 0x4u)->screen_threshold;
							path_cursor += 2u;
							OBJECT(current_object)->burning_boulder.catch_up_factor = CHASE_STEP_WITH_LIMIT(path_cursor - 0x6u)->catch_up_factor;
							path_cursor += 2u;
							OBJECT(current_object)->burning_boulder.base_speed = CHASE_STEP_WITH_LIMIT(path_cursor - 0x8u)->base_speed;
							path_cursor += 2u;
							OBJECT(current_object)->burning_boulder.wait_timer = CHASE_STEP_WITH_LIMIT(path_cursor - 0xAu)->wait;
							OBJECT(current_object)->burning_boulder.path = path_cursor + 2u;
						}
					}
					else
					{
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.screen_threshold = CHASE_STEP_WITH_LIMIT(path_cursor - 0x4u)->screen_threshold;
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.catch_up_factor = CHASE_STEP_WITH_LIMIT(path_cursor - 0x6u)->catch_up_factor;
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.base_speed = CHASE_STEP_WITH_LIMIT(path_cursor - 0x8u)->base_speed;
						path_cursor += 2u;
						OBJECT(current_object)->burning_boulder.wait_timer = CHASE_STEP_WITH_LIMIT(path_cursor - 0xAu)->wait;
						OBJECT(current_object)->burning_boulder.path = path_cursor + 2u;
					}
				}
			}
		}
		if (OBJECT(current_object)->burning_boulder.holding_back != 0u)
		{
			OBJECT(current_object)->burning_boulder.rumble_timer = 0x3Cu;
			if ((int16_t)(scroll_position - OBJECT(current_object)->burning_boulder.hold_start_position) >= 0x300)
			{
				OBJECT(current_object)->burning_boulder.holding_back = 0u;
				OBJECT(current_object)->burning_boulder.screen_threshold = OBJECT(current_object)->burning_boulder.saved_threshold;
				OBJECT(current_object)->burning_boulder.base_speed = OBJECT(current_object)->burning_boulder.saved_base_speed;
				OBJECT(current_object)->burning_boulder.slow_timer = 0x78;
				OBJECT(current_object)->burning_boulder.rumble_timer = 0u;
			}
			else
			{
				if (no_player_in_play != 0u)
					OBJECT(current_object)->burning_boulder.hold_timer = 0xFFu;
				hold_timer = OBJECT(current_object)->burning_boulder.hold_timer;
				OBJECT(current_object)->burning_boulder.hold_timer = hold_timer - 1u;
				if (hold_timer == 0u)
				{
					OBJECT(current_object)->burning_boulder.holding_back = 0u;
					OBJECT(current_object)->burning_boulder.screen_threshold = OBJECT(current_object)->burning_boulder.saved_threshold;
					OBJECT(current_object)->burning_boulder.base_speed = OBJECT(current_object)->burning_boulder.saved_base_speed;
					OBJECT(current_object)->burning_boulder.slow_timer = 0x78;
					OBJECT(current_object)->burning_boulder.rumble_timer = 0u;
				}
			}
		}
		next_slow_timer = OBJECT(current_object)->burning_boulder.slow_timer - 1u;
		OBJECT(current_object)->burning_boulder.slow_timer = next_slow_timer;
		if ((int16_t)next_slow_timer < 0)
			OBJECT(current_object)->burning_boulder.slow_timer = 0;
		if (OBJECT(current_object)->burning_boulder.wait_timer <= 0)
		{
			if (OBJECT(current_object)->burning_boulder.screen_threshold <= OBJECT(current_object)->screen_y)
			{
				roll_speed = (uint16_t)OBJECT(current_object)->burning_boulder.base_speed;
				if ((uint16_t)OBJECT(current_object)->burning_boulder.slow_timer != 0u)
					roll_speed = 0x10u;
			}
			else
			{
				slope_factor = 0u;
				if (OBJECT(current_object)->floor_slope != 0u)
				{
					floor_slope = OBJECT(current_object)->floor_slope;
					slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
				}
				compute_camera_slope_speed(0u, slope_factor);
				roll_speed = (uint16_t)((uint32_t)camera_slope_speed_int * OBJECT(current_object)->burning_boulder.catch_up_factor) >> 8;
				if ((int16_t)roll_speed <= OBJECT(current_object)->burning_boulder.base_speed)
				{
					roll_speed = (uint16_t)OBJECT(current_object)->burning_boulder.base_speed;
					if ((uint16_t)OBJECT(current_object)->burning_boulder.slow_timer != 0u)
						roll_speed = 0x10u;
				}
			}
			OBJECT(current_object)->speed_fraction = roll_speed;
			step_y = (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
			OBJECT(current_object)->y += step_y;
			OBJECT(current_object)->speed_y = step_y;
		}
		speed_level = OBJECT(current_object)->speed_fraction;
		if (speed_level != 0u)
		{
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level > 5)
				speed_level = 5u;
			OBJECT(current_object)->animation_speed = burning_boulder_roll_rates[speed_level];
			animate(&ignored_2, &ignored_6, &ignored_7);
		}
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		rumble_before = (OBJECT(current_object)->burning_boulder.rumble_timer | ((uint32_t)((uint16_t)OBJECT(current_object)->burning_boulder.start_delay & 0xFFu) << 8u));
		rumble_after = rumble_before - 1u;
		zero = rumble_after == 0u;
		OBJECT(current_object)->burning_boulder.rumble_timer = (uint8_t)rumble_after;
		OBJECT(current_object)->burning_boulder.start_delay = (OBJECT(current_object)->burning_boulder.start_delay & 0xFF00u) | ((rumble_after >> 8u) & 0xFFu);
		if ((int16_t)rumble_before <= 1)
		{
			animation_speed = OBJECT(current_object)->animation_speed;
			if (zero == 0u)
			{
				rumble_timer_halfword = animation_speed * 0xAu;
				OBJECT(current_object)->burning_boulder.rumble_timer = (uint8_t)rumble_timer_halfword;
				OBJECT(current_object)->burning_boulder.start_delay = (OBJECT(current_object)->burning_boulder.start_delay & 0xFF00u) | ((rumble_timer_halfword >> 8u) & 0xFFu);
				sound_id = 0xDu;
				play_sound(sound_id, &ignored_2, &ignored_3);
			}
		}
		if (scroll_position < 0x3300u || OBJECT(current_object)->burning_boulder.floor_block_high >= 0)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		else
			OBJECT(current_object)->burning_boulder.state = 3u;
		project_object(&ignored, &ignored_2);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		OBJECT(current_object)->burning_boulder.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 2:
		object = LINKED(current_object, lead_burning_boulder);
		step_y = (int16_t)OBJECT(object)->speed_fraction;
		if ((int8_t)OBJECT(object)->burning_boulder.state >= 3)
			step_y = (int16_t)OBJECT(current_object)->speed_fraction;
		step_y = step_y << 12;
		OBJECT(current_object)->y += step_y;
		OBJECT(current_object)->speed_y = step_y;
		speed_level = OBJECT(object)->speed_fraction;
		OBJECT(current_object)->speed_fraction = speed_level;
		if (speed_level != 0u)
		{
			speed_level = speed_level >> 4;
			if ((int16_t)speed_level > 5)
				speed_level = 5u;
			OBJECT(current_object)->animation_speed = burning_boulder_roll_rates[speed_level];
			animate(&ignored_2, &ignored_6, &ignored_7);
		}
		find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
		if (OBJECT(current_object)->burning_boulder.floor_block_high >= 0)
			OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
		if (OBJECT(LINKED(current_object, lead_burning_boulder))->burning_boulder.state == 3u)
			OBJECT(current_object)->burning_boulder.state = 3u;
		project_object(&ignored, &ignored_2);
		add_to_zone_list(ignored);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->burning_boulder.depth_offset);
		OBJECT(current_object)->burning_boulder.draw_depth = depth;
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 3:
		drop_step = 0x60000u;
		if (scroll_position >= 0x4800u)
			drop_step = 0u;
		OBJECT(current_object)->z += drop_step;
		OBJECT(current_object)->x += 0x167D2u;
		OBJECT(current_object)->y += 0x1638Eu;
		OBJECT(current_object)->animation_speed = 2u;
		animate(&ignored_2, &ignored_6, &ignored_7);
		project_object(&ignored, &ignored_2);
		if (scroll_position < 0x4C50u && OBJECT(current_object)->screen_y > -0x10)
		{
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		OBJECT(current_object)->burning_boulder.state++;
		OBJECT(current_object)->burning_boulder.start_delay = 0x1E;
		flames_left = 4u;
		do
		{
			current_object = LINKED(current_object, OBJECT(current_object)->child);
			OBJECT(current_object)->flags |= 0x400u;
			flames_left--;
		} while (flames_left != 0u);
		current_object = object_cursor;
		if ((uint16_t)OBJECT(current_object)->burning_boulder.path != 0xFFFFu)
		{
			depth = 0u;
			queue_sprite_at_depth(&ignored, &depth);
			return;
		}
		delete_current_object();
		return;
	case 4:
		burning_boulder_release_timer++;
		if (burning_boulder_release_timer >= 0x258)
			burning_boulder_released = 0xFFu;
		if (burning_boulder_released != 0u)
		{
			start_delay_2 = OBJECT(current_object)->burning_boulder.start_delay;
			OBJECT(current_object)->burning_boulder.start_delay = start_delay_2 - 1u;
			if ((int16_t)start_delay_2 <= 1)
			{
				OBJECT(current_object)->burning_boulder.state++;
				OBJECT(current_object)->flags &= ~1u;
				OBJECT(current_object)->x = 0x1800000;
				OBJECT(current_object)->y = 0xEF00000;
				OBJECT(current_object)->z = 0x9700000;
				OBJECT(current_object)->speed_z = 0xC0000;
				project_object(&ignored, &ignored_2);
				OBJECT(current_object)->sprite_palette = 0x1240u;
			}
		}
		return;
	case 5:
		if (OBJECT(current_object)->screen_y >= 0x150)
		{
			delete_current_object();
			return;
		}
		if ((OBJECT(current_object)->flags & 1u) != 0u)
		{
			if (OBJECT(current_object)->z_int >= 0x9E0)
				seesaw_launched = 0xFFu;
			project_object(&ignored, &ignored_2);
			ignored_2 = queue_sprite(ignored_2);
			return;
		}
		if (OBJECT(current_object)->z_int >= 0xA28)
		{
			OBJECT(current_object)->flags |= 1u;
			camera_shake_active = 1u;
			sound_id = 0xAu;
			play_sound(sound_id, &ignored_2, &ignored_3);
		}
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		if (OBJECT(current_object)->z_int >= 0x9E0)
			seesaw_launched = 0xFFu;
		project_object(&ignored, &ignored_2);
		ignored_2 = queue_sprite(ignored_2);
		return;
	default:
		invalid_jump(0x863F8u + OBJECT(current_object)->burning_boulder.state * 3u);
	}
}

/* Sets up a boulder's flame from burning_boulder_flame_layout[kind] (kind is the flame's number, 0-3):
   offset_x and offset_y from the boulder and its animation (an offset into
   burning_boulder_flame_animations); then burning_boulder_flame_flicker. */
void burning_boulder_flame_start(void)
{
	uint32_t layout_record = 0;

	layout_record = OBJECT(current_object)->kind * 6u + burning_boulder_flame_layout_ADDRESS;
	OBJECT(current_object)->burning_boulder_flame.offset_x = BURNING_BOULDER_FLAME_OFFSET(layout_record)->dx;
	layout_record += 2u;
	OBJECT(current_object)->burning_boulder_flame.offset_y = BURNING_BOULDER_FLAME_OFFSET(layout_record - 0x2u)->dy;
	OBJECT(current_object)->animation = BURNING_BOULDER_FLAME_OFFSET(layout_record - 0x2u)->animation + burning_boulder_flame_animations_ADDRESS;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->animation_speed = 1u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = 0;
	OBJECT(current_object)->handler = burning_boulder_flame_flicker_ADDRESS;
}

/* A flame on a burning boulder: while the boulder rolls (its speed_fraction above 0) it plays its
   animation at the boulder's screen position + offset_x/offset_y, sorted just above it (its draw_depth +
   1); at the end of the animation it waits a random 0-0x3F frames. Removed once it has flag 0x400. */
void burning_boulder_flame_flicker(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0, random_bits = 0, wrapped = 0;
	uint16_t pause_timer = 0;

	if ((OBJECT(current_object)->flags & 0x400u) != 0u)
	{
		delete_current_object();
		return;
	}
	pause_timer = OBJECT(current_object)->burning_boulder_flame.pause_timer;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = pause_timer - 1u;
	if ((int16_t)pause_timer > 1)
		return;
	OBJECT(current_object)->burning_boulder_flame.pause_timer = 0;
	if ((int16_t)OBJECT(LINKED(current_object, OBJECT(current_object)->burning_boulder_flame.boulder))->speed_fraction <= 0)
		return;
	object = LINKED(current_object, OBJECT(current_object)->burning_boulder_flame.boulder);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(current_object)->burning_boulder_flame.offset_x;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->burning_boulder_flame.offset_y;
	animate(&ignored, &wrapped, &ignored_2);
	if ((uint8_t)wrapped == 0u)
	{
		depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->burning_boulder_flame.boulder))->burning_boulder.draw_depth + 1u);
		queue_sprite_at_depth(&ignored_5, &depth);
		return;
	}
	next_random(&random_bits, &ignored_3, &ignored_4);
	OBJECT(current_object)->burning_boulder_flame.pause_timer = random_bits & 0x3Fu;
}

/* Sets up an ember source: end_position from ember_emitter_end_positions[sprite_frame], z on its floor;
   then ember_emitter_move. */
void ember_emitter_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	OBJECT(current_object)->ember_emitter.spawn_timer = 0;
	OBJECT(current_object)->floor_slope = 0u;
	OBJECT(current_object)->floor_type = 0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->ember_emitter.end_position = ember_emitter_end_positions[OBJECT(current_object)->sprite_frame];
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5);
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->handler = ember_emitter_move_ADDRESS;
}

/* An invisible ember source, removed at end_position. It moves at camera_slope_speed >> 7 along byte
   0x15 of its floor block (0 and 4 -y, 1 +x, 2 +y, 3 -x; 0 and 3 while screen_x >= 0x40, the others
   while <= 0x40), on its floor unless sprite_frame is 3; every random 0-0x3F frames a falling ember
   0-0x7F from it. */
void ember_emitter_move(void)
{
	uint8_t random_low = 0;
	uint32_t floor_slope = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, move_step = 0, object = 0, random_bits = 0, slope_factor = 0, speed_entry = 0, travel_direction = 0;
	uint16_t spawn_timer = 0;

	if (OBJECT(current_object)->ember_emitter.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	travel_direction = mem_read8(floor_blocks + 2u + OBJECT(current_object)->floor_block + 0x15u);
	OBJECT(current_object)->ember_emitter.travel_direction = travel_direction;
	switch (travel_direction)
	{
	case 0:
	case 3:
		if (OBJECT(current_object)->screen_x >= 0x40)
		{
			slope_factor = 0u;
			if (OBJECT(current_object)->floor_slope != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			random_bits = compute_camera_slope_speed(0u, slope_factor);
			move_step = camera_slope_speed >> 7;
			switch (OBJECT(current_object)->ember_emitter.travel_direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y -= move_step;
				break;
			case 1:
				OBJECT(current_object)->x += move_step;
				break;
			case 2:
				OBJECT(current_object)->y += move_step;
				break;
			case 3:
				OBJECT(current_object)->x -= move_step;
				break;
			default:
				invalid_jump(0x869ABu + OBJECT(current_object)->ember_emitter.travel_direction * 2u);
			}
		}
		break;
	case 1:
	case 2:
	case 4:
		if (OBJECT(current_object)->screen_x <= 0x40)
		{
			slope_factor = 0u;
			if (OBJECT(current_object)->floor_slope != 0u)
			{
				floor_slope = OBJECT(current_object)->floor_slope;
				slope_factor = floor_slope_factors[(uint16_t)(floor_slope * 2u)];
			}
			random_bits = compute_camera_slope_speed(0u, slope_factor);
			move_step = camera_slope_speed >> 7;
			switch (OBJECT(current_object)->ember_emitter.travel_direction)
			{
			case 0:
			case 4:
				OBJECT(current_object)->y -= move_step;
				break;
			case 1:
				OBJECT(current_object)->x += move_step;
				break;
			case 2:
				OBJECT(current_object)->y += move_step;
				break;
			case 3:
				OBJECT(current_object)->x -= move_step;
				break;
			default:
				invalid_jump(0x869ABu + OBJECT(current_object)->ember_emitter.travel_direction * 2u);
			}
		}
		break;
	default:
		invalid_jump(0x86958u + travel_direction * 2u);
	}
	if (OBJECT(current_object)->sprite_frame != 3u)
	{
		find_floor(&ignored, &ignored_2, &ignored_3, &random_bits, &object);
		OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	}
	project_object(&ignored, &ignored_2);
	spawn_timer = OBJECT(current_object)->ember_emitter.spawn_timer;
	OBJECT(current_object)->ember_emitter.spawn_timer = spawn_timer - 1u;
	if ((int16_t)spawn_timer > 1)
		return;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x51D0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->falling_ember.state = 0u;
		OBJECT(object)->falling_ember.travel_direction = OBJECT(current_object)->ember_emitter.travel_direction;
		OBJECT(object)->handler = falling_ember_update_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		next_random(&random_bits, &ignored_4, &ignored_5);
		random_low = (uint8_t)random_bits;
		random_bits = (random_bits & ~0xFFFFu) | (random_bits & 0x7Fu);
		OBJECT(object)->x_int += random_bits;
		random_bits = random_bits >> 16;
		random_bits = random_bits & 0x7Fu;
		OBJECT(object)->y_int += random_bits;
		speed_entry = falling_ember_speeds_ADDRESS + (random_low & 3u) * 4u;
		OBJECT(object)->speed_z = (uint32_t)FALLING_EMBER_SPEED(speed_entry)->speed_z << 8;
		OBJECT(object)->speed_y = (uint32_t)FALLING_EMBER_SPEED(speed_entry)->speed_y << 8;
		OBJECT(object)->animation = falling_ember_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_speed = 2u;
		OBJECT(object)->animation_timer = 0;
	}
	next_random(&random_bits, &ignored_4, &ignored_5);
	OBJECT(current_object)->ember_emitter.spawn_timer = random_bits & 0x3Fu;
}

/* A falling ember: state 0 raises it 0x70 at a time to above the screen; 1 falls (speed_y, speed_z,
   falling_ember_animation) to its floor (0x530 where none), then bursts (falling_ember_burst_animation,
   or the slope one on a sloped or typed floor); 2 plays the burst, then is removed. Removed when it
   leaves the screen. */
void falling_ember_update(void)
{
	uint32_t burst_animation = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, off_screen = 0, wrapped = 0;

	switch (OBJECT(current_object)->falling_ember.state)
	{
	case 0:
	case 3:
		OBJECT(current_object)->falling_ember.state++;
		do
		{
			OBJECT(current_object)->z_int -= 0x70u;
			project_object(&ignored, &ignored_2);
		} while (OBJECT(current_object)->screen_y > 0);
		return;
	case 1:
		break;
	case 2:
		off_screen = check_left_screen();
		if ((uint8_t)off_screen == 0u)
		{
			animate(&ignored_2, &wrapped, &ignored_4);
			if ((uint8_t)wrapped == 0u)
			{
				update_screen_position(&ignored, &ignored_2);
				ignored_2 = queue_sprite(ignored_2);
				return;
			}
		}
		delete_current_object();
		return;
	default:
		invalid_jump(0x86AA8u + OBJECT(current_object)->falling_ember.state * 3u);
	}
	off_screen = check_left_screen();
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	animate(&ignored_2, &ignored_3, &ignored_4);
	find_floor(&ignored, &ignored_2, &ignored_5, &ignored_6, &ignored_7);
	if (OBJECT(current_object)->falling_ember.floor_block_high < 0)
		OBJECT(current_object)->floor_z = 0x5300000;
	project_object(&ignored, &ignored_2);
	if (OBJECT(current_object)->z_int < OBJECT(current_object)->floor_z_int)
	{
		ignored_2 = queue_sprite(ignored_2);
		return;
	}
	OBJECT(current_object)->falling_ember.state++;
	burst_animation = falling_ember_burst_animation_ADDRESS;
	if (OBJECT(current_object)->falling_ember.floor_block_high >= 0 && (OBJECT(current_object)->floor_slope | OBJECT(current_object)->floor_type) != 0u)
		burst_animation = falling_ember_slope_burst_animation_ADDRESS;
	OBJECT(current_object)->animation = burst_animation;
	OBJECT(current_object)->animation_frame = 0xFFu;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->z = OBJECT(current_object)->floor_z;
	ignored_2 = queue_sprite(ignored_2);
}

/* Sets up a carrier from scene_carrier_records[sprite_frame]: position, 32-bit speed_x/speed_y/speed_z
   and end_position. Number 0 becomes scene_object, 1-3 player_carriers[sprite_frame - 1]; then
   scene_carrier_move. */
void scene_carrier_start(void)
{
	uint32_t carrier_record = 0, sprite_frame = 0;

	carrier_record = scene_carrier_records[OBJECT(current_object)->sprite_frame] + scene_carrier_records_ADDRESS;
	OBJECT(current_object)->x_int = SCENE_CARRIER_RECORD(carrier_record)->x;
	OBJECT(current_object)->x_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->y_int = SCENE_CARRIER_RECORD(carrier_record - 0x2u)->y;
	OBJECT(current_object)->y_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->z_int = SCENE_CARRIER_RECORD(carrier_record - 0x4u)->z;
	OBJECT(current_object)->z_fraction = 0u;
	carrier_record += 2u;
	OBJECT(current_object)->speed_x = SCENE_CARRIER_RECORD(carrier_record - 0x6u)->speed_x;
	carrier_record += 4u;
	OBJECT(current_object)->speed_y = SCENE_CARRIER_RECORD(carrier_record - 0xAu)->speed_y;
	carrier_record += 4u;
	OBJECT(current_object)->speed_z = SCENE_CARRIER_RECORD(carrier_record - 0xEu)->speed_z;
	OBJECT(current_object)->scene_carrier.end_position = SCENE_CARRIER_RECORD(carrier_record - 0xEu)->end_position;
	sprite_frame = OBJECT(current_object)->sprite_frame;
	if ((uint8_t)sprite_frame != 0u)
		player_carriers[(sprite_frame & ~0xFFu) | (uint8_t)(sprite_frame - 1u)] = current_object;
	else
		scene_object = current_object;
	OBJECT(current_object)->handler = scene_carrier_move_ADDRESS;
}

/* An invisible carrier: adds its speeds to its position every frame (sprite_frame 1-3 only once
   seesaw_launched is set); removed at end_position. */
void scene_carrier_move(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	if (OBJECT(current_object)->scene_carrier.end_position <= scroll_position)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->sprite_frame == 0u || seesaw_launched != 0u)
	{
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		OBJECT(current_object)->y += OBJECT(current_object)->speed_y;
		OBJECT(current_object)->x += OBJECT(current_object)->speed_x;
	}
	project_object(&ignored, &ignored_2);
}

/* Sets up the seesaw: a multi-part sprite (seesaw_parts in a second-pool block) at (0x70, 0xF68, 0x9E0)
   with seesaw_animation; then seesaw_tilt (without a free block it tries again next frame). */
void seesaw_start(void)
{
	uint32_t free_count = 0, ignored = 0, ignored_4 = 0, ignored_5 = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, seesaw_parts_ADDRESS, 0xDu);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->x = 0x700000;
		OBJECT(current_object)->y = 0xF680000;
		OBJECT(current_object)->z = 0x9E00000;
		project_object(&ignored_4, &ignored_5);
		OBJECT(current_object)->animation = seesaw_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 2u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->handler = seesaw_tilt_ADDRESS;
	}
}

/* Draws the seesaw; once seesaw_launched is set it plays seesaw_animation once into its part list (the
   word at +0xA). Frees its part list and is removed at scroll 0x4D00. */
void seesaw_tilt(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, next_step = 0, step_list = 0;
	uint8_t animation_timer = 0;
	uint16_t last_step = 0;

	if (scroll_position >= 0x4D00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_speed >= 0 && seesaw_launched != 0u)
	{
		animation_timer = OBJECT(current_object)->animation_timer;
		OBJECT(current_object)->animation_timer = animation_timer - 1u;
		if ((int8_t)animation_timer <= 1)
		{
			OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
			next_step = OBJECT(current_object)->animation_frame;
			next_step = (uint8_t)(next_step + 1u);
			step_list = OBJECT(current_object)->animation;
			last_step = FRAME_ANIMATION(step_list)->last_frame;
			step_list += 2u;
			if (next_step == last_step)
				OBJECT(current_object)->animation_speed = 0xFFu;
			OBJECT(current_object)->animation_frame = next_step;
			mem_write16(OBJECT(current_object)->part_list + 0xAu, SPRITE_FRAME_NUMBERS(step_list)[next_step]);
		}
	}
	update_screen_position(&ignored_2, &ignored_3);
	depth = 0u;
	queue_sprite_at_depth(&ignored_2, &depth);
}

/* Sets up the metal platform's frame (sprite 0xAFB8, palette 0x1230) at z 0x530, then
   metal_platform_base_draw. */
void metal_platform_base_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFB8u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = metal_platform_base_draw_ADDRESS;
	metal_platform_base_draw();
}

/* Draws the metal platform's frame until it leaves the screen (check_left_screen_margins 0x40/0x10),
   then removes it; it stays when the deck flies off. */
void metal_platform_base_draw(void)
{
	uint16_t margin = 0, unseen_margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;

	unseen_margin = 0x10u;
	margin = 0x40u;
	off_screen = check_left_screen_margins(margin, unseen_margin);
	if (off_screen == 0u)
	{
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Sets up the metal platform's deck (sprite 0xAFC0) at z 0x530 with timer 0x12C, and a second object,
   the far railing (sprite 0xAFC8 at 0x131, 0xF24, state 3); both run metal_platform_update (without a
   free object it tries again next frame). */
void metal_platform_start(void)
{
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->sprite_flags = 0x1002u;
	OBJECT(current_object)->sprite_frame = 0xAFC0u;
	OBJECT(current_object)->sprite_palette = 0x1230u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = -6;
	OBJECT(current_object)->sprite_offset_y = 0x1B;
	OBJECT(current_object)->metal_platform.state = 0u;
	OBJECT(current_object)->metal_platform.timer = 0x12C;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored_2);
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1001u;
		OBJECT(object)->sprite_frame = 0xAFC8u;
		OBJECT(object)->sprite_palette = 0x1230u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->x = 0x1310000;
		OBJECT(object)->y = 0xF240000;
		OBJECT(object)->z = 0x5300000;
		OBJECT(object)->handler = metal_platform_update_ADDRESS;
		OBJECT(object)->metal_platform.state = 3u;
		OBJECT(object)->parent = current_object;
		OBJECT(current_object)->handler = metal_platform_update_ADDRESS;
	}
}

/* State 0 waits timer (0x12C) frames; 1 sets metal_platform_lifted, starts the fire wall
   (fire_wall_start) and the deck flies up (speed_z 0x100000, 0x6000 more a frame after 0xA frames); 3
   the railing waits for the deck, then flies up with it (state 2). From screen y 0x28 up it shows
   metal_platform.frame; removed at screen y -0x58. */
void metal_platform_update(void)
{
	uint32_t depth = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0, object_2 = 0;
	uint16_t timer = 0, timer_2 = 0;

	switch (OBJECT(current_object)->metal_platform.state)
	{
	case 0:
	case 4:
		timer = OBJECT(current_object)->metal_platform.timer;
		OBJECT(current_object)->metal_platform.timer = timer - 1u;
		if ((int16_t)timer <= 1)
			OBJECT(current_object)->metal_platform.state++;
		update_screen_position(&ignored, &ignored_2);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x28u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	case 1:
		OBJECT(current_object)->metal_platform.state++;
		metal_platform_lifted = 0xFFu;
		allocate_object(&free_count, &object_2);
		if (free_count != 0u)
			OBJECT(object_2)->handler = fire_wall_start_ADDRESS;
		OBJECT(current_object)->speed_z = 0x100000;
		OBJECT(current_object)->sprite_frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.frame = 0xAFD0u;
		OBJECT(current_object)->metal_platform.timer = 0xA;
		OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
		OBJECT(current_object)->sprite_offset_y = 0xC;
		break;
	case 2:
		break;
	case 3:
		object = LINKED(current_object, OBJECT(current_object)->parent);
		if ((int8_t)OBJECT(object)->metal_platform.state >= 1)
		{
			OBJECT(current_object)->sprite_flags = 0x1002u;
			OBJECT(current_object)->x = OBJECT(object)->x;
			OBJECT(current_object)->y = OBJECT(object)->y;
			OBJECT(current_object)->z = OBJECT(object)->z;
			OBJECT(current_object)->x_int += 0x2Du;
			OBJECT(current_object)->metal_platform.timer = 0xA;
			OBJECT(current_object)->sprite_offset_x = -0x24;
			OBJECT(current_object)->sprite_offset_y = 0x1C;
			OBJECT(current_object)->speed_z = 0x80000;
			OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
			OBJECT(current_object)->sprite_frame = 0xAFD8u;
			OBJECT(current_object)->metal_platform.frame = 0xAFE8u;
			OBJECT(current_object)->metal_platform.state = 2u;
		}
		project_object(&ignored, &ignored_2);
		ignored_2 = queue_sprite(ignored_2);
		return;
	default:
		invalid_jump(0x86DF9u + OBJECT(current_object)->metal_platform.state * 3u);
	}
	timer_2 = OBJECT(current_object)->metal_platform.timer;
	OBJECT(current_object)->metal_platform.timer = timer_2 - 1u;
	if ((int16_t)timer_2 <= 1)
	{
		OBJECT(current_object)->metal_platform.timer = 0;
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z -= OBJECT(current_object)->speed_z;
	}
	if (OBJECT(current_object)->screen_y <= -0x58)
	{
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->screen_y <= 0x28)
		OBJECT(current_object)->sprite_frame = OBJECT(current_object)->metal_platform.frame;
	project_object(&ignored, &ignored_2);
	depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Sets up the fire wall beside the metal platform: a multi-part sprite (fire_wall_parts in a second-pool
   block) at (0x100, 0xE47, 0x610), box 0x5B x 0xF0, height 0x100, kind 0x14, fire_wall_animation,
   speed_z -0x100000; sound 7, then fire_wall_rise (without a free block it tries again next frame). */
void fire_wall_start(void)
{
	uint8_t sound_id = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, part_list = 0;

	OBJECT(current_object)->sprite_flags = 0xD006u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x5Bu;
	OBJECT(current_object)->hitbox_size_y = 0xF0u;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->hitbox_offset_x = -0x2A;
	OBJECT(current_object)->hitbox_offset_y = -1;
	OBJECT(current_object)->kind = 0x14u;
	OBJECT(current_object)->direction = 0x100u;
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	OBJECT(current_object)->x = 0x1000000;
	OBJECT(current_object)->y = 0xE470000;
	OBJECT(current_object)->z = 0x6100000;
	OBJECT(current_object)->animation = fire_wall_animation_ADDRESS;
	OBJECT(current_object)->animation_speed = 2u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->speed_z = -0x100000;
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->child = 0xFFFFu;
	allocate_object_2(&free_count, &ignored_3, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, fire_wall_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		sound_id = 7u;
		play_sound(sound_id, &ignored_2, &ignored_3);
		OBJECT(current_object)->handler = fire_wall_rise_ADDRESS;
	}
}

/* The fire wall rises until z 0x550 (thrown up at speed_z -0x100000, slowed by 0x6000 a frame),
   animating its parts, touchable. Removed with its part list at scroll 0x6800. */
void fire_wall_rise(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	if (scroll_position >= 0x6800u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->z_int > 0x550)
	{
		OBJECT(current_object)->speed_z += 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored_2, &ignored_3);
	animate_all_parts(&ignored_4, &ignored_5);
	add_to_touchable_list(ignored_2);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x70u);
	queue_sprite_at_depth(&ignored_2, &depth);
}

/* Sets up a fire trench (sprite 0xB200, palette 0x51F0) at z 0x530 with flame_timer 0, so the first
   flames come at once; then fire_trench_update. */
void fire_trench_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->sprite_flags = 0x100Au;
	OBJECT(current_object)->sprite_frame = 0xB200u;
	OBJECT(current_object)->sprite_palette = 0x51F0u;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = -0x16;
	OBJECT(current_object)->fire_trench_start.flame_timer = 0;
	OBJECT(current_object)->handler = fire_trench_update_ADDRESS;
	OBJECT(current_object)->z = 0x5300000;
	project_object(&ignored, &ignored_2);
}

/* Draws the fire trench and every 0x12C frames spawn_fire_trench_flames; removed once it has left the
   screen (check_left_screen_margin 0x70). */
void fire_trench_update(void)
{
	uint16_t margin = 0;
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, off_screen = 0;
	uint16_t flame_timer = 0;

	margin = 0x70u;
	off_screen = check_left_screen_margin(margin);
	if (off_screen != 0u)
	{
		delete_current_object();
		return;
	}
	flame_timer = OBJECT(current_object)->fire_trench_update.flame_timer;
	OBJECT(current_object)->fire_trench_update.flame_timer = flame_timer - 1u;
	if ((int16_t)flame_timer <= 1)
	{
		OBJECT(current_object)->fire_trench_update.flame_timer = 0x12C;
		spawn_fire_trench_flames();
	}
	update_screen_position(&ignored, &ignored_2);
	depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y - 0x20u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Spawns three flame columns along the fire trench (y 0xE47, 0xE97, 0xEE7, z 0x530): box 0x40 x 0x80,
   kind 0x14, fire_trench_flame_animation, fire_trench_flame_burn; sound 7 when the trench's screen_x is
   0x140 or less. Nothing without 3 free objects. */
void spawn_fire_trench_flames(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0;
	uint16_t flames_left_before = 0;

	if (free_object_stack < 3)
		return;
	trench_flame_y = 0xE47u;
	trench_flames_left = 3;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->sprite_flags = 0x1006u;
		OBJECT(object)->sprite_palette = 0x5200u;
		OBJECT(object)->flags = 2u;
		OBJECT(object)->sprite_offset_x = -8;
		OBJECT(object)->sprite_offset_y = 0x3E;
		OBJECT(object)->hitbox_offset_x = 0;
		OBJECT(object)->hitbox_offset_y = 0;
		OBJECT(object)->kind = 0x14u;
		OBJECT(object)->direction = 0x100u;
		OBJECT(object)->hitbox_size_x = 0x40u;
		OBJECT(object)->hitbox_size_y = 0x80u;
		OBJECT(object)->handler = fire_trench_flame_burn_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y_int = trench_flame_y;
		OBJECT(object)->y_fraction = 0u;
		OBJECT(object)->z = 0x5300000;
		trench_flame_y += 0x50u;
		OBJECT(object)->animation = fire_trench_flame_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 2u;
		flames_left_before = trench_flames_left;
		trench_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	if (OBJECT(current_object)->screen_x <= 0x140)
	{
		sound_id = 7u;
		play_sound(sound_id, &ignored_2, &ignored_3);
	}
}

/* A flame column of a fire trench: plays its animation once, its hitbox_height following
   fire_trench_flame_heights[animation_frame], touchable; removed at the last step. */
void fire_trench_flame_burn(void)
{
	uint32_t at_last_step = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0;

	animate(&ignored, &ignored_2, &at_last_step);
	if ((uint8_t)at_last_step == 0u)
	{
		OBJECT(current_object)->hitbox_height = fire_trench_flame_heights[OBJECT(current_object)->animation_frame];
		project_object(&ignored_3, &ignored);
		add_to_touchable_list(ignored_3);
		depth = (uint16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->sprite_offset_y);
		queue_sprite_at_depth(&ignored_3, &depth);
		return;
	}
	delete_current_object();
}

/* Sets up a player's ride object (made by spawn_player_ride): box 0x10 x 0x10, it becomes scene_object;
   then ridden_object_fall. */
void ridden_object_start(void)
{
	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x10u;
	OBJECT(current_object)->hitbox_size_y = 0x10u;
	OBJECT(current_object)->handler = ridden_object_fall_ADDRESS;
	scene_object = current_object;
	ridden_object_fall();
}

/* A player's ride object: in the rider list until it stands on a platform (flags 8), then
   ridden_object_follow. */
void ridden_object_fall(void)
{
	if ((OBJECT(current_object)->flags & 8u) != 0u)
		OBJECT(current_object)->handler = ridden_object_follow_ADDRESS;
	add_to_rider_list();
}

/* Keeps the ride object at its platform's (a bridge plank's) x and z every frame; removed at scroll
   0x8D00. */
void ridden_object_follow(void)
{
	uint32_t object = 0;

	if (scroll_position < 0x8D00u)
	{
		object = LINKED(current_object, OBJECT(current_object)->platform);
		OBJECT(current_object)->x = OBJECT(object)->x;
		OBJECT(current_object)->z = OBJECT(object)->z;
		return;
	}
	delete_current_object();
}

/* Sets up the invisible fire under the hanging bridge: box 0x40 x 0x80, height 0x70, kind 0x13, z 0x868,
   rising_fire_object, path bridge_fire_script. With 3 free objects it makes three rising flames
   (rising_flame_update) 0x20, 0x40 and 0x60 along y at its top and becomes bridge_fire_rise; else it
   tries again next frame. */
void bridge_fire_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0;
	uint16_t flames_left_before = 0;

	OBJECT(current_object)->flags = 2u;
	OBJECT(current_object)->hitbox_size_x = 0x40u;
	OBJECT(current_object)->hitbox_size_y = 0x80u;
	OBJECT(current_object)->hitbox_height = 0x70u;
	OBJECT(current_object)->hitbox_offset_x = -0x20;
	OBJECT(current_object)->hitbox_offset_y = -0x10;
	OBJECT(current_object)->kind = 0x13u;
	OBJECT(current_object)->bridge_fire.state = 2u;
	OBJECT(current_object)->bridge_fire.off_screen = 0u;
	OBJECT(current_object)->bridge_fire.sound_timer = 0u;
	OBJECT(current_object)->z = 0x8680000;
	OBJECT(current_object)->floor_z = OBJECT(current_object)->z;
	OBJECT(current_object)->player = 0u;
	rising_fire_object = current_object;
	OBJECT(current_object)->bridge_fire.path = bridge_fire_script_ADDRESS;
	OBJECT(current_object)->bridge_fire.holding_back = 0u;
	OBJECT(current_object)->bridge_fire.wait_timer = 0;
	find_floor(&ignored, &ignored_2, &ignored_3, &ignored_4, &object);
	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->bridge_fire.projected_x_whole = OBJECT(current_object)->projected_x;
	OBJECT(current_object)->bridge_fire.projected_x_fraction = 0u;
	bridge_flames_left = 3;
	if (free_object_stack < bridge_flames_left)
		return;
	bridge_flame_offset_y = 0u;
	do
	{
		allocate_object(&ignored, &object);
		OBJECT(object)->rising_flame.state = 0u;
		OBJECT(object)->handler = rising_flame_update_ADDRESS;
		OBJECT(object)->x = OBJECT(current_object)->x;
		OBJECT(object)->y = OBJECT(current_object)->y;
		bridge_flame_offset_y += 0x200000u;
		OBJECT(object)->y += bridge_flame_offset_y;
		OBJECT(object)->z = OBJECT(current_object)->z;
		OBJECT(object)->z_int -= OBJECT(current_object)->hitbox_height;
		flames_left_before = bridge_flames_left;
		bridge_flames_left = flames_left_before - 1u;
	} while ((int16_t)flames_left_before > 1);
	OBJECT(current_object)->handler = bridge_fire_rise_ADDRESS;
}

/* The bridge fire's handler: bridge_fire_read_script, unless holding back. When no_player_in_play,
   player_mishap or player_dropping_in is set it holds back (base_speed 0, screen_threshold 0xE0, the old
   ones saved, hold_timer 0xFF): bridge_fire_climb_step only, which ends the hold after 0x300 of scroll
   or hold_timer. */
void bridge_fire_rise(void)
{
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		bridge_fire_read_script();
		return;
	}
	if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
	{
		bridge_fire_climb_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		bridge_fire_read_script();
		return;
	}
	OBJECT(current_object)->bridge_fire.holding_back = 0xFFu;
	OBJECT(current_object)->bridge_fire.saved_base_speed = OBJECT(current_object)->bridge_fire.base_speed;
	OBJECT(current_object)->bridge_fire.base_speed = 0;
	OBJECT(current_object)->bridge_fire.hold_start_position = scroll_position;
	OBJECT(current_object)->bridge_fire.hold_timer = 0xFFu;
	OBJECT(current_object)->bridge_fire.saved_threshold = OBJECT(current_object)->bridge_fire.screen_threshold;
	OBJECT(current_object)->bridge_fire.screen_threshold = 0xE0;
	bridge_fire_climb_step();
}

/* Steps through bridge_fire_script: a record sets screen_threshold, catch_up_factor, base_speed and
   wait_timer once the fire's top reaches its height or scroll_position its position less hitbox_height
   (0: height only); a negative factor removes the fire, a negative threshold sets state 7. Then
   bridge_fire_climb_step. */
void bridge_fire_read_script(void)
{
	uint32_t path_cursor = 0;
	uint16_t record_height = 0;

	path_cursor = OBJECT(current_object)->bridge_fire.path;
	record_height = BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor)->height;
	path_cursor += 2u;
	if ((int16_t)record_height < (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) && (BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x2u)->position == 0u || scroll_position < (uint16_t)(BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x2u)->position - OBJECT(current_object)->hitbox_height)))
	{
		bridge_fire_climb_step();
		return;
	}
	path_cursor += 2u;
	if (BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x4u)->catch_up_factor < 0)
	{
		delete_current_object();
		return;
	}
	if (BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x4u)->screen_threshold < 0)
		OBJECT(current_object)->bridge_fire.state = 7u;
	OBJECT(current_object)->bridge_fire.screen_threshold = BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x4u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->bridge_fire.catch_up_factor = BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x6u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->bridge_fire.base_speed = BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0x8u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->bridge_fire.wait_timer = BRIDGE_FIRE_SCRIPT_ENTRY(path_cursor - 0xAu)->wait;
	OBJECT(current_object)->bridge_fire.path = path_cursor + 2u;
	bridge_fire_climb_step();
}

/* The bridge fire's chase: speed_fraction is base_speed while its top (screen_y - hitbox_height) is at
   or above screen_threshold, else camera_ground_speed * catch_up_factor >> 8, at least base_speed; z
   climbs by it after wait_timer. Sound 9 every 0x3C frames on screen; its top reaching z 0x534 sets
   stage_trigger_bits, once (player marks it done). */
void bridge_fire_climb_step(void)
{
	uint16_t climb_speed = 0;
	uint32_t ignored_2 = 0, ignored_3 = 0, point_x = 0, point_y = 0, projected_dx = 0, sound_id = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t wait_timer = 0;

	if (OBJECT(current_object)->bridge_fire.holding_back == 0u)
	{
		if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->bridge_fire.screen_threshold)
			climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
		else
		{
			if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
			{
				OBJECT(current_object)->bridge_fire.holding_back = 0u;
				OBJECT(current_object)->bridge_fire.screen_threshold = OBJECT(current_object)->bridge_fire.saved_threshold;
				OBJECT(current_object)->bridge_fire.base_speed = OBJECT(current_object)->bridge_fire.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->bridge_fire.catch_up_factor) >> 8;
			if ((int16_t)climb_speed <= OBJECT(current_object)->bridge_fire.base_speed)
				climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->bridge_fire.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->bridge_fire.holding_back = 0u;
		OBJECT(current_object)->bridge_fire.screen_threshold = OBJECT(current_object)->bridge_fire.saved_threshold;
		OBJECT(current_object)->bridge_fire.base_speed = OBJECT(current_object)->bridge_fire.saved_base_speed;
		compute_camera_ground_speed(0u);
		climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->bridge_fire.catch_up_factor) >> 8;
		if ((int16_t)climb_speed <= OBJECT(current_object)->bridge_fire.base_speed)
			climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->bridge_fire.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->bridge_fire.hold_timer;
		OBJECT(current_object)->bridge_fire.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->bridge_fire.holding_back = 0u;
			OBJECT(current_object)->bridge_fire.screen_threshold = OBJECT(current_object)->bridge_fire.saved_threshold;
			OBJECT(current_object)->bridge_fire.base_speed = OBJECT(current_object)->bridge_fire.saved_base_speed;
			compute_camera_ground_speed(0u);
			climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->bridge_fire.catch_up_factor) >> 8;
			if ((int16_t)climb_speed <= OBJECT(current_object)->bridge_fire.base_speed)
				climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
		}
		else if ((int16_t)(OBJECT(current_object)->screen_y - OBJECT(current_object)->hitbox_height) <= OBJECT(current_object)->bridge_fire.screen_threshold)
			climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
		else
		{
			if (OBJECT(current_object)->bridge_fire.holding_back != 0u)
			{
				OBJECT(current_object)->bridge_fire.holding_back = 0u;
				OBJECT(current_object)->bridge_fire.screen_threshold = OBJECT(current_object)->bridge_fire.saved_threshold;
				OBJECT(current_object)->bridge_fire.base_speed = OBJECT(current_object)->bridge_fire.saved_base_speed;
			}
			compute_camera_ground_speed(0u);
			climb_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->bridge_fire.catch_up_factor) >> 8;
			if ((int16_t)climb_speed <= OBJECT(current_object)->bridge_fire.base_speed)
				climb_speed = (uint16_t)OBJECT(current_object)->bridge_fire.base_speed;
		}
	}
	OBJECT(current_object)->speed_fraction = climb_speed;
	wait_timer = OBJECT(current_object)->bridge_fire.wait_timer;
	OBJECT(current_object)->bridge_fire.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->bridge_fire.wait_timer = 0;
		OBJECT(current_object)->z -= (uint32_t)(int16_t)OBJECT(current_object)->speed_fraction << 12;
	}
	projected_dx = (int32_t)((uint32_t)FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(current_object)->floor_attribute)->x_per_rise * OBJECT(current_object)->speed_fraction) >> 4;
	OBJECT(current_object)->bridge_fire.fixed_projected_x += projected_dx;
	OBJECT(current_object)->projected_x = OBJECT(current_object)->bridge_fire.projected_x_whole;
	OBJECT(current_object)->projected_y -= (int32_t)projected_dx >> 1;
	update_screen_position(&sound_id, &ignored_2);
	if (OBJECT(current_object)->screen_y > 0x160)
	{
		OBJECT(current_object)->bridge_fire.off_screen = 0xFFu;
		point_x = OBJECT(current_object)->bridge_fire.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
		if (OBJECT(current_object)->player == 0u && (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= 0x534)
		{
			stage_trigger_bits = 0xFFFFFFFFu;
			OBJECT(current_object)->player = 0xFFu;
		}
		sound_id = add_to_touchable_list(sound_id);
		return;
	}
	if (OBJECT(current_object)->bridge_fire.off_screen != 0u)
	{
		OBJECT(current_object)->bridge_fire.off_screen = 0u;
		OBJECT(current_object)->bridge_fire.sound_timer = 0u;
	}
	if (current_game_mode == 7u)
	{
		point_x = OBJECT(current_object)->bridge_fire.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
		if (OBJECT(current_object)->player == 0u && (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= 0x534)
		{
			stage_trigger_bits = 0xFFFFFFFFu;
			OBJECT(current_object)->player = 0xFFu;
		}
		sound_id = add_to_touchable_list(sound_id);
		return;
	}
	sound_timer = OBJECT(current_object)->bridge_fire.sound_timer;
	OBJECT(current_object)->bridge_fire.sound_timer = sound_timer - 1u;
	if ((int8_t)sound_timer > 1)
	{
		point_x = OBJECT(current_object)->bridge_fire.fixed_projected_x;
		point_y = OBJECT(current_object)->projected_y * 2u;
		unproject_point(&point_x, &point_y);
		OBJECT(current_object)->x = point_x;
		OBJECT(current_object)->y = point_y;
		if (OBJECT(current_object)->player == 0u && (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= 0x534)
		{
			stage_trigger_bits = 0xFFFFFFFFu;
			OBJECT(current_object)->player = 0xFFu;
		}
		sound_id = add_to_touchable_list(sound_id);
		return;
	}
	OBJECT(current_object)->bridge_fire.sound_timer = 0x3Cu;
	sound_id = 9u;
	play_sound((uint8_t)sound_id, &ignored_2, &ignored_3);
	point_x = OBJECT(current_object)->bridge_fire.fixed_projected_x;
	point_y = OBJECT(current_object)->projected_y * 2u;
	unproject_point(&point_x, &point_y);
	OBJECT(current_object)->x = point_x;
	OBJECT(current_object)->y = point_y;
	if (OBJECT(current_object)->player == 0u && (int16_t)(OBJECT(current_object)->z_int - OBJECT(current_object)->hitbox_height) <= 0x534)
	{
		stage_trigger_bits = 0xFFFFFFFFu;
		OBJECT(current_object)->player = 0xFFu;
	}
	sound_id = add_to_touchable_list(sound_id);
}

/* Sets up the lava flow (lava_flow_parts in a second-pool block): box 0x60 x 0xE7, height 0x50, kind
   0x14, z 0x530, lava_flow_animation, path lava_flow_script, wait_timer 0x78; clears
   metal_platform_lifted and waits in lava_flow_wait (without a free block it tries again next frame). */
void lava_flow_start(void)
{
	uint32_t free_count = 0, ignored = 0, part_list = 0;

	allocate_object_2(&free_count, &ignored, &part_list);
	if (free_count != 0u)
	{
		move_halfwords_backward(part_list, lava_flow_parts_ADDRESS, 9u);
		OBJECT(current_object)->part_list = part_list;
		OBJECT(current_object)->sprite_flags = 0xD008u;
		OBJECT(current_object)->flags = 2u;
		OBJECT(current_object)->parent = current_object;
		OBJECT(current_object)->child = 0xFFFFu;
		OBJECT(current_object)->lava_flow.path = lava_flow_script_ADDRESS;
		OBJECT(current_object)->lava_flow.holding_back = 0u;
		OBJECT(current_object)->lava_flow.wait_timer = 0x78;
		OBJECT(current_object)->spawn_arg_2 = 2u;
		OBJECT(current_object)->lava_flow.unused_word = 0u;
		OBJECT(current_object)->lava_flow.sound_timer = 0xFFu;
		OBJECT(current_object)->handler = lava_flow_wait_ADDRESS;
		metal_platform_lifted = 0u;
		OBJECT(current_object)->sprite_offset_x = 0;
		OBJECT(current_object)->sprite_offset_y = 0;
		OBJECT(current_object)->hitbox_offset_x = -0x6A;
		OBJECT(current_object)->hitbox_offset_y = 0;
		OBJECT(current_object)->hitbox_size_x = 0x60u;
		OBJECT(current_object)->hitbox_size_y = 0xE7u;
		OBJECT(current_object)->hitbox_height = 0x50u;
		OBJECT(current_object)->kind = 0x14u;
		OBJECT(current_object)->direction = 0x100u;
		OBJECT(current_object)->animation = lava_flow_animation_ADDRESS;
		OBJECT(current_object)->animation_frame = 0u;
		OBJECT(current_object)->animation_speed = 6u;
		OBJECT(current_object)->animation_timer = 0;
		OBJECT(current_object)->z = 0x5300000;
	}
}

/* Waits for metal_platform_lifted, then wait_timer (0x78) frames; then spawn_lava_steam and
   lava_flow_advance as the handler. */
void lava_flow_wait(void)
{
	uint16_t wait_timer = 0;

	if (metal_platform_lifted != 0u)
	{
		wait_timer = OBJECT(current_object)->lava_flow.wait_timer;
		OBJECT(current_object)->lava_flow.wait_timer = wait_timer - 1u;
		if ((int16_t)wait_timer <= 1)
		{
			OBJECT(current_object)->handler = lava_flow_advance_ADDRESS;
			spawn_lava_steam();
		}
	}
}

/* The lava flow's handler: lava_flow_read_script, unless holding back. When no_player_in_play,
   player_mishap or player_dropping_in is set it holds back (base_speed 0, screen_threshold -0x50, the
   old ones saved, hold_timer 0xFF): lava_flow_move_step only, which ends the hold after 0x300 of scroll
   or hold_timer. */
void lava_flow_advance(void)
{
	uint32_t flags = 0;

	flags = OBJECT(current_object)->flags;
	OBJECT(current_object)->flags = flags | 0x80000000u;
	if ((flags & 0x80000000u) == 0u)
	{
		lava_flow_read_script();
		return;
	}
	if (OBJECT(current_object)->lava_flow.holding_back != 0u)
	{
		lava_flow_move_step();
		return;
	}
	if (no_player_in_play == 0u && player_mishap == 0u && player_dropping_in == 0u)
	{
		lava_flow_read_script();
		return;
	}
	OBJECT(current_object)->lava_flow.holding_back = 0xFFu;
	OBJECT(current_object)->lava_flow.saved_base_speed = OBJECT(current_object)->lava_flow.base_speed;
	OBJECT(current_object)->lava_flow.base_speed = 0;
	OBJECT(current_object)->lava_flow.hold_start_position = scroll_position;
	OBJECT(current_object)->lava_flow.hold_timer = 0xFFu;
	OBJECT(current_object)->lava_flow.saved_threshold = OBJECT(current_object)->lava_flow.screen_threshold;
	OBJECT(current_object)->lava_flow.screen_threshold = -0x50;
	lava_flow_move_step();
}

/* Steps through lava_flow_script: the next record (scroll position, x, screen_threshold,
   catch_up_factor, base_speed, wait_timer) is taken once scroll_position reaches its position or x_int
   its x (0: position only); a negative factor removes the flow and its part list. Then
   lava_flow_move_step. */
void lava_flow_read_script(void)
{
	uint16_t part_list = 0;
	uint32_t path_cursor = 0;
	uint16_t record_scroll = 0;

	path_cursor = OBJECT(current_object)->lava_flow.path;
	record_scroll = CHASE_STEP_WITH_LIMIT(path_cursor)->position;
	path_cursor += 2u;
	if (scroll_position < record_scroll && ((uint16_t)CHASE_STEP_WITH_LIMIT(path_cursor - 0x2u)->limit == 0u || OBJECT(current_object)->x_int < CHASE_STEP_WITH_LIMIT(path_cursor - 0x2u)->limit))
	{
		lava_flow_move_step();
		return;
	}
	path_cursor += 2u;
	if (CHASE_STEP_WITH_LIMIT(path_cursor - 0x4u)->catch_up_factor < 0)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->lava_flow.screen_threshold = CHASE_STEP_WITH_LIMIT(path_cursor - 0x4u)->screen_threshold;
	path_cursor += 2u;
	OBJECT(current_object)->lava_flow.catch_up_factor = CHASE_STEP_WITH_LIMIT(path_cursor - 0x6u)->catch_up_factor;
	path_cursor += 2u;
	OBJECT(current_object)->lava_flow.base_speed = CHASE_STEP_WITH_LIMIT(path_cursor - 0x8u)->base_speed;
	path_cursor += 2u;
	OBJECT(current_object)->lava_flow.wait_timer = CHASE_STEP_WITH_LIMIT(path_cursor - 0xAu)->wait;
	OBJECT(current_object)->lava_flow.path = path_cursor + 2u;
	lava_flow_move_step();
}

/* The lava flow's chase along +x: chaser_speed is base_speed while screen_x is at or past
   screen_threshold, else camera_ground_speed * catch_up_factor >> 8, at least base_speed; x moves by it
   after wait_timer; spawn_lava_flame, extend_lava_flow. sound_timer is also its off-screen mark, cleared
   every frame on screen: sound 0x10 plays every frame. */
void lava_flow_move_step(void)
{
	uint16_t flow_speed = 0;
	uint32_t depth = 0, flame = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, octant = 0, x_step_or_sound = 0;
	uint8_t hold_timer = 0, sound_timer = 0;
	uint16_t wait_timer = 0;

	project_object(&x_step_or_sound, &ignored);
	if (OBJECT(current_object)->lava_flow.holding_back == 0u)
	{
		if (OBJECT(current_object)->lava_flow.screen_threshold <= OBJECT(current_object)->screen_x)
			flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
		else
		{
			if (OBJECT(current_object)->lava_flow.holding_back != 0u)
			{
				OBJECT(current_object)->lava_flow.holding_back = 0u;
				OBJECT(current_object)->lava_flow.screen_threshold = OBJECT(current_object)->lava_flow.saved_threshold;
				OBJECT(current_object)->lava_flow.base_speed = OBJECT(current_object)->lava_flow.saved_base_speed;
			}
			octant = compute_camera_ground_speed(0u);
			flow_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->lava_flow.catch_up_factor) >> 8;
			if ((int16_t)flow_speed <= OBJECT(current_object)->lava_flow.base_speed)
				flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
		}
	}
	else if ((int16_t)(scroll_position - OBJECT(current_object)->lava_flow.hold_start_position) >= 0x300)
	{
		OBJECT(current_object)->lava_flow.holding_back = 0u;
		OBJECT(current_object)->lava_flow.screen_threshold = OBJECT(current_object)->lava_flow.saved_threshold;
		OBJECT(current_object)->lava_flow.base_speed = OBJECT(current_object)->lava_flow.saved_base_speed;
		octant = compute_camera_ground_speed(0u);
		flow_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->lava_flow.catch_up_factor) >> 8;
		if ((int16_t)flow_speed <= OBJECT(current_object)->lava_flow.base_speed)
			flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
	}
	else
	{
		if (no_player_in_play != 0u)
			OBJECT(current_object)->lava_flow.hold_timer = 0xFFu;
		hold_timer = OBJECT(current_object)->lava_flow.hold_timer;
		OBJECT(current_object)->lava_flow.hold_timer = hold_timer - 1u;
		if (hold_timer == 0u)
		{
			OBJECT(current_object)->lava_flow.holding_back = 0u;
			OBJECT(current_object)->lava_flow.screen_threshold = OBJECT(current_object)->lava_flow.saved_threshold;
			OBJECT(current_object)->lava_flow.base_speed = OBJECT(current_object)->lava_flow.saved_base_speed;
			octant = compute_camera_ground_speed(0u);
			flow_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->lava_flow.catch_up_factor) >> 8;
			if ((int16_t)flow_speed <= OBJECT(current_object)->lava_flow.base_speed)
				flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
		}
		else if (OBJECT(current_object)->lava_flow.screen_threshold <= OBJECT(current_object)->screen_x)
			flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
		else
		{
			if (OBJECT(current_object)->lava_flow.holding_back != 0u)
			{
				OBJECT(current_object)->lava_flow.holding_back = 0u;
				OBJECT(current_object)->lava_flow.screen_threshold = OBJECT(current_object)->lava_flow.saved_threshold;
				OBJECT(current_object)->lava_flow.base_speed = OBJECT(current_object)->lava_flow.saved_base_speed;
			}
			octant = compute_camera_ground_speed(0u);
			flow_speed = (uint16_t)((uint32_t)camera_ground_speed_int * OBJECT(current_object)->lava_flow.catch_up_factor) >> 8;
			if ((int16_t)flow_speed <= OBJECT(current_object)->lava_flow.base_speed)
				flow_speed = (uint16_t)OBJECT(current_object)->lava_flow.base_speed;
		}
	}
	chaser_speed = flow_speed;
	wait_timer = OBJECT(current_object)->lava_flow.wait_timer;
	OBJECT(current_object)->lava_flow.wait_timer = wait_timer - 1u;
	if ((int16_t)wait_timer <= 1)
	{
		OBJECT(current_object)->lava_flow.wait_timer = 0;
		x_step_or_sound = (uint32_t)chaser_speed << 12;
		OBJECT(current_object)->x += x_step_or_sound;
		OBJECT(current_object)->speed_x = x_step_or_sound;
		OBJECT(current_object)->speed_y = 0;
	}
	animate_all_parts(&ignored_2, &flame);
	if (OBJECT(current_object)->screen_x < -0x40)
	{
		OBJECT(current_object)->lava_flow.sound_timer = 0xFFu;
		flame = spawn_lava_flame(octant, flame);
		extend_lava_flow(flame);
		add_to_touchable_list(x_step_or_sound);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	if (OBJECT(current_object)->lava_flow.sound_timer != 0u)
	{
		OBJECT(current_object)->lava_flow.sound_timer = 0u;
		OBJECT(current_object)->lava_flow.sound_timer = 0u;
	}
	if (current_game_mode == 7u)
	{
		flame = spawn_lava_flame(octant, flame);
		extend_lava_flow(flame);
		add_to_touchable_list(x_step_or_sound);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	sound_timer = OBJECT(current_object)->lava_flow.sound_timer;
	OBJECT(current_object)->lava_flow.sound_timer = sound_timer - 1u;
	if (sound_timer != 0u)
	{
		flame = spawn_lava_flame(octant, flame);
		extend_lava_flow(flame);
		add_to_touchable_list(x_step_or_sound);
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored_4, &depth);
		return;
	}
	OBJECT(current_object)->lava_flow.sound_timer = 0x40u;
	x_step_or_sound = 0x10u;
	play_sound((uint8_t)x_step_or_sound, &ignored, &ignored_3);
	flame = spawn_lava_flame(octant, flame);
	extend_lava_flow(flame);
	add_to_touchable_list(x_step_or_sound);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Adds a trailing sheet of lava behind the object (lava_flow_segment_follow, linked as its child, with
   its own copy of lava_flow_parts in a second-pool block) from scroll_position 0x6180, while it has no
   child yet, its screen_x is -0x50 or more and both pools have a free block. block is only scratch. */
void extend_lava_flow(uint32_t block)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	if ((uint16_t)free_object_stack != 0u && (uint16_t)free_object_stack_2[0] != 0u && (int16_t)OBJECT(current_object)->child < 0 && OBJECT(current_object)->screen_x >= -0x50 && scroll_position >= 0x6180u)
	{
		allocate_object(&ignored, &block);
		OBJECT(block)->sprite_flags = 0xD008u;
		OBJECT(block)->flags = 0u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->child = 0xFFFFu;
		OBJECT(block)->spawn_arg_2 = 0u;
		OBJECT(block)->parent = current_object;
		OBJECT(current_object)->child = block;
		OBJECT(block)->handler = lava_flow_segment_follow_ADDRESS;
		OBJECT(block)->z = OBJECT(current_object)->z;
		object = block;
		allocate_object_2(&ignored, &ignored_2, &block);
		move_halfwords_backward(block, lava_flow_parts_ADDRESS, 9u);
		OBJECT(object)->part_list = block;
	}
}

/* A sheet of lava trailing the one before it (its parent): 0x40 left and 0x20 down on screen, map x 0x50
   less, the parent's frame in both its parts; also spawn_lava_flame and extend_lava_flow. Removed with
   its part list at scroll 0x7E00, or past screen x -0xA0 (clearing the parent's child). */
void lava_flow_segment_follow(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, flame = 0, ignored = 0, ignored_2 = 0, object = 0, part_list_2 = 0, part_list_3 = 0;

	if (scroll_position >= 0x7E00u)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->projected_x = OBJECT(object)->projected_x;
	OBJECT(current_object)->projected_x -= 0x40u;
	OBJECT(current_object)->projected_y = OBJECT(object)->projected_y;
	OBJECT(current_object)->projected_y_int += 0x20u;
	update_screen_position(&ignored, &ignored_2);
	if (OBJECT(current_object)->screen_x <= -0xA0)
	{
		OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->child = 0xFFFFu;
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->x_int -= 0x50u;
	OBJECT(current_object)->y = OBJECT(object)->y;
	part_list_2 = OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->part_list;
	part_list_3 = OBJECT(current_object)->part_list;
	SPRITE_PART_LIST(part_list_3)->part_0_frame = SPRITE_PART_LIST(part_list_2)->part_0_frame;
	SPRITE_PART_LIST(part_list_3)->part_1_frame = SPRITE_PART_LIST(part_list_2)->part_0_frame;
	flame = spawn_lava_flame(0u, 0u);
	extend_lava_flow(flame);
	depth = (uint16_t)(OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->screen_y + 0x48u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Every random 0-0x1F frames (counted in spawn_arg_2) a flame on the lava: lava_flame_animation or
   lava_flame_animation_2 at random, at a random anchor from the object (x -0x30 to -0x12F, y 0x10 to
   0xD0), lava_flame_burn. Hands back the flame (flame as it came when none is made); random_value is
   only scratch. */
uint32_t spawn_lava_flame(uint32_t random_value, uint32_t flame)
{
	uint32_t flame_random = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint32_t saved_current_object = 0;
	uint16_t spawn_arg_2 = 0;

	spawn_arg_2 = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spawn_arg_2 = spawn_arg_2 - 1u;
	if ((int16_t)spawn_arg_2 <= 1)
	{
		next_random(&random_value, &ignored, &ignored_2);
		lava_flame_random = random_value;
		OBJECT(current_object)->spawn_arg_2 = random_value & 0x1Fu;
		allocate_object(&free_count, &flame);
		if (free_count == 0u)
			return flame;
		OBJECT(flame)->sprite_flags = 0x1004u;
		OBJECT(flame)->sprite_palette = 0x50C0u;
		OBJECT(flame)->flags = 0u;
		OBJECT(flame)->sprite_offset_x = 0;
		OBJECT(flame)->sprite_offset_y = 0;
		OBJECT(flame)->handler = lava_flame_burn_ADDRESS;
		OBJECT(flame)->z = OBJECT(current_object)->z;
		OBJECT(flame)->parent = current_object;
		OBJECT(flame)->animation = lava_flame_animation_ADDRESS;
		flame_random = lava_flame_random;
		if ((flame_random & 1u) != 0u)
			OBJECT(flame)->animation = lava_flame_animation_2_ADDRESS;
		OBJECT(flame)->animation_frame = 0u;
		OBJECT(flame)->animation_speed = 4u;
		OBJECT(flame)->animation_timer = 0;
		OBJECT(flame)->anchor_y = (((uint16_t)flame_random & 0xFFu) * 0xC0C1u + 0x100000u) >> 16;
		OBJECT(flame)->anchor_x = 0u - (((flame_random >> 16) & 0xFFu) + 0x30u);
		saved_current_object = current_object;
		current_object = flame;
		project_object(&ignored_3, &ignored_4);
		current_object = saved_current_object;
	}
	return flame;
}

/* A flame on the lava: follows its parent at anchor_x/anchor_y, plays its animation once and is removed. */
void lava_flame_burn(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, wrapped = 0;

	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->x_int += OBJECT(current_object)->anchor_x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int += OBJECT(current_object)->anchor_y;
	project_object(&ignored, &ignored_2);
	animate(&ignored_2, &wrapped, &ignored_3);
	if ((uint8_t)wrapped == 0u)
	{
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	delete_current_object();
}

/* Spawns lava_steam_puff_count steam puffs (lava_steam_puff_update) at lava_steam_puff_offsets around
   the lava flow, while objects are free. */
void spawn_lava_steam(void)
{
	uint32_t free_count = 0, object = 0, offset_cursor = 0, puffs_left = 0;

	puffs_left = lava_steam_puff_count;
	offset_cursor = lava_steam_puff_offsets_ADDRESS;
	do
	{
		allocate_object(&free_count, &object);
		if (free_count == 0u)
			break;
		OBJECT(object)->sprite_flags = 0x1005u;
		OBJECT(object)->sprite_palette = 0x50D0u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = lava_steam_puff_update_ADDRESS;
		OBJECT(object)->spawn_arg_2 = 2u;
		OBJECT(object)->animation = lava_steam_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0xFFu;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->animation_speed = 4u;
		OBJECT(object)->anchor_x = LAVA_STEAM_PUFF_OFFSET(offset_cursor)->dx;
		offset_cursor += 2u;
		OBJECT(object)->anchor_y = LAVA_STEAM_PUFF_OFFSET(offset_cursor - 0x2u)->dy;
		OBJECT(object)->parent = current_object;
		offset_cursor += 2u;
		puffs_left--;
	} while (puffs_left != 0u);
}

/* A steam puff on the lava flow: plays lava_steam_animation at its parent's position +
   anchor_x/anchor_y, waits a random 0-0x3F frames and plays it again. Removed at scroll 0x7E00. */
void lava_steam_puff_update(void)
{
	uint32_t depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, object = 0, random_bits = 0, wrapped = 0;
	uint16_t spawn_arg_2 = 0;

	if (scroll_position >= 0x7E00u)
	{
		delete_current_object();
		return;
	}
	if ((int8_t)OBJECT(current_object)->animation_frame >= 0)
	{
		update_screen_position(&ignored, &ignored_2);
		animate(&ignored_2, &wrapped, &ignored_3);
		if ((uint8_t)wrapped != 0u)
		{
			next_random(&random_bits, &ignored_4, &ignored_5);
			OBJECT(current_object)->spawn_arg_2 = random_bits & 0x3Fu;
			OBJECT(current_object)->animation_frame = 0xFFu;
		}
		depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
		queue_sprite_at_depth(&ignored, &depth);
		return;
	}
	spawn_arg_2 = OBJECT(current_object)->spawn_arg_2;
	OBJECT(current_object)->spawn_arg_2 = spawn_arg_2 - 1u;
	if ((int16_t)spawn_arg_2 > 1)
		return;
	OBJECT(current_object)->animation_frame = 0u;
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->x = OBJECT(object)->x;
	OBJECT(current_object)->x_int += OBJECT(current_object)->anchor_x;
	OBJECT(current_object)->y = OBJECT(object)->y;
	OBJECT(current_object)->y_int += OBJECT(current_object)->anchor_y;
	OBJECT(current_object)->z = OBJECT(object)->z;
	project_object(&ignored, &ignored_2);
	animate(&ignored_2, &wrapped, &ignored_3);
	if ((uint8_t)wrapped != 0u)
	{
		next_random(&random_bits, &ignored_4, &ignored_5);
		OBJECT(current_object)->spawn_arg_2 = random_bits & 0x3Fu;
		OBJECT(current_object)->animation_frame = 0xFFu;
	}
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* A rock slab over the lava (a platform, its glow cycling): 1 waits for the group leader within 0x5A
   along direction, or its group_slots entry; 2 bobs, rises to rise_limit and erupts (lava_rock_erupt)
   once the leader is past it in y or erupt_delay runs out; 3 flies up breaking, removed above the
   screen. Removed while map_screen_active is set. */
void lava_rock_update(void)
{
	uint16_t part_list = 0;
	uint32_t at_last_step = 0, block_or_slot_entry = 0, depth = 0, free_count = 0, height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_6 = 0, ignored_7 = 0, ignored_8 = 0, ignored_9 = 0, parts = 0, random_value = 0, slot_entry = 0, sprite_depth = 0;
	uint16_t erupt_delay = 0;

	if (map_screen_active != 0u)
	{
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			part_list = (uint16_t)OBJECT(current_object)->part_list;
			free_object_2(part_list);
		}
		delete_current_object();
		return;
	}
	switch (OBJECT(current_object)->lava_rock.state)
	{
	case 0:
	case 4:
		OBJECT(current_object)->lava_rock.state++;
		OBJECT(current_object)->lava_rock.palette_list = lava_rock_glow_palettes_ADDRESS;
		OBJECT(current_object)->lava_rock.palette_index = 0u;
		OBJECT(current_object)->lava_rock.palette_timer = 0u;
		next_random(&random_value, &ignored, &ignored_2);
		OBJECT(current_object)->lava_rock.palette_delay = (random_value & 0xFu) + 4u;
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
		{
			allocate_object_2(&free_count, &ignored_3, &parts);
			if ((uint8_t)free_count == 0u)
			{
				delete_current_object();
				return;
			}
			move_halfwords_backward(parts, lava_rock_parts_ADDRESS, 0x10u);
			OBJECT(current_object)->part_list = parts;
		}
		project_object(&ignored_6, &ignored_7);
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	case 1:
		update_screen_position(&ignored_6, &ignored_7);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if ((uint16_t)OBJECT(current_player)->group_leader_move.wait_timer != 0u)
		{
			if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
			{
				slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
				if ((int16_t)mem_read16(slot_entry) < 0)
				{
					OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
					OBJECT(current_object)->lava_rock.rise_limit = 0u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
				}
			}
		}
		else
		{
			switch (OBJECT(current_object)->direction)
			{
			case 0:
			case 4:
				if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x5Au))
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored_6, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored_6, &sprite_depth);
				return;
			case 1:
				if ((int16_t)(OBJECT(current_player)->y_int - 0x5Au) <= OBJECT(current_object)->y_int)
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored_6, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored_6, &sprite_depth);
				return;
			case 2:
				if (OBJECT(current_object)->x_int <= (int16_t)(OBJECT(current_player)->x_int + 0x5Au))
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored_6, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored_6, &sprite_depth);
				return;
			case 3:
				if ((int16_t)(OBJECT(current_player)->x_int - 0x5Au) <= OBJECT(current_object)->x_int)
				{
					OBJECT(current_object)->lava_rock.rise_limit = 0x10u;
					OBJECT(current_object)->lava_rock.state++;
					OBJECT(current_object)->speed_z = -0x10000;
					if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
						return;
					if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
						animate_sign();
					else
						cycle_part_palettes();
					add_to_platform_list();
					sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
					OBJECT(current_object)->draw_depth = sprite_depth;
					queue_sprite_at_depth(&ignored_6, &sprite_depth);
					return;
				}
				if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
				{
					slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
					if ((int16_t)mem_read16(slot_entry) < 0)
					{
						OBJECT(current_object)->lava_rock.erupt_delay = mem_read16(slot_entry) & 0x7FFFu;
						OBJECT(current_object)->lava_rock.rise_limit = 0u;
						OBJECT(current_object)->lava_rock.state++;
						OBJECT(current_object)->speed_z = -0x10000;
					}
				}
				if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
					return;
				if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
					animate_sign();
				else
					cycle_part_palettes();
				add_to_platform_list();
				sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
				OBJECT(current_object)->draw_depth = sprite_depth;
				queue_sprite_at_depth(&ignored_6, &sprite_depth);
				return;
			default:
				invalid_jump(0x919DDu + OBJECT(current_object)->direction * 2u);
			}
		}
		if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA8u) >= 0x1E8u)
			return;
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
			animate_sign();
		else
			cycle_part_palettes();
		add_to_platform_list();
		sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y + OBJECT(current_object)->lava_rock.depth_offset - OBJECT(current_object)->sprite_offset_y);
		OBJECT(current_object)->draw_depth = sprite_depth;
		queue_sprite_at_depth(&ignored_6, &sprite_depth);
		return;
	case 2:
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		height = (uint32_t)OBJECT(current_object)->floor_z - OBJECT(current_object)->z;
		if (height >= 0x20000u)
		{
			OBJECT(current_object)->speed_z = 0u - OBJECT(current_object)->speed_z;
			if ((int32_t)height >= 0 && (int16_t)(OBJECT(current_object)->previous_floor_z_int - OBJECT(current_object)->floor_z_int) <= (int16_t)OBJECT(current_object)->lava_rock.rise_limit)
				OBJECT(current_object)->floor_z_int--;
		}
		update_screen_position(&ignored_6, &ignored_7);
		current_player = current_object;
		current_player = LINKED(current_player, OBJECT(current_object)->parent);
		if (OBJECT(current_object)->y_int <= OBJECT(current_player)->y_int)
		{
			lava_rock_erupt(0u);
			return;
		}
		if (OBJECT(current_object)->y_int <= (int16_t)(OBJECT(current_player)->y_int + 0x30u))
			OBJECT(current_object)->lava_rock.rise_limit = 0x20u;
		if (OBJECT(current_object)->lava_rock.takes_slot != 0u)
		{
			block_or_slot_entry = group_slots_ADDRESS + OBJECT(current_object)->lava_rock.group_slot * 2u;
			if ((int16_t)mem_read16(block_or_slot_entry) < 0)
			{
				erupt_delay = OBJECT(current_object)->lava_rock.erupt_delay;
				OBJECT(current_object)->lava_rock.erupt_delay = erupt_delay - 1u;
				if ((int16_t)erupt_delay <= 1)
				{
					OBJECT(current_object)->lava_rock.takes_slot = 1u;
					lava_rock_erupt(block_or_slot_entry);
					return;
				}
			}
		}
		lava_rock_draw_at_previous_floor();
		return;
	case 3:
		OBJECT(current_object)->speed_z -= 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
		update_screen_position(&ignored_6, &ignored_7);
		if (OBJECT(current_object)->screen_y <= -0x28)
		{
			if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
			{
				part_list = (uint16_t)OBJECT(current_object)->part_list;
				free_object_2(part_list);
			}
			delete_current_object();
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_frame < 0)
		{
			if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
				add_to_platform_list();
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
			queue_sprite_at_depth(&ignored_6, &depth);
			return;
		}
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
		{
			animate(&ignored_7, &ignored_9, &at_last_step);
			if ((uint8_t)at_last_step == 0u)
			{
				if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
					add_to_platform_list();
				depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
				queue_sprite_at_depth(&ignored_6, &depth);
				return;
			}
			OBJECT(current_object)->animation_frame = 0xFFu;
			if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
				add_to_platform_list();
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
			queue_sprite_at_depth(&ignored_6, &depth);
			return;
		}
		animate_all_parts(&at_last_step, &ignored_8);
		if ((uint8_t)at_last_step != 0u)
		{
			OBJECT(current_object)->animation_frame = 0xFFu;
			if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
				add_to_platform_list();
			depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
			queue_sprite_at_depth(&ignored_6, &depth);
			return;
		}
		if ((int8_t)OBJECT(current_object)->animation_frame <= 2)
			add_to_platform_list();
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
		queue_sprite_at_depth(&ignored_6, &depth);
		return;
	default:
		invalid_jump(0x9194Du + OBJECT(current_object)->lava_rock.state * 3u);
	}
}

/* A lava rock erupts: state 3, its break animation from the start, and spawn_lava_column under it (a
   second 0x71 along x for a multi-part rock), with sound 7 when screen_x is -0x90 or more. With
   takes_slot 1 only member 0 does that, and member 2 without the sound. block is only scratch. */
void lava_rock_erupt(uint32_t block)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->lava_rock.state++;
	OBJECT(current_object)->speed_z = 0;
	OBJECT(current_object)->animation_speed = 4u;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_frame = 0u;
	if (OBJECT(current_object)->lava_rock.takes_slot == 1u && OBJECT(current_object)->lava_rock.member != 0u)
	{
		if (OBJECT(current_object)->lava_rock.member == 2u)
		{
			block = spawn_lava_column(block);
			if ((OBJECT(current_object)->sprite_flags & 0xD000u) == 0xD000u)
			{
				OBJECT(current_object)->x_int += 0x71u;
				spawn_lava_column(block);
				OBJECT(current_object)->x_int -= 0x71u;
				lava_rock_draw_at_previous_floor();
				return;
			}
		}
		lava_rock_draw_at_previous_floor();
		return;
	}
	if (OBJECT(current_object)->screen_x < -0x90)
	{
		block = spawn_lava_column(block);
		if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
		{
			lava_rock_draw_at_previous_floor();
			return;
		}
		OBJECT(current_object)->x_int += 0x71u;
		spawn_lava_column(block);
		OBJECT(current_object)->x_int -= 0x71u;
		lava_rock_draw_at_previous_floor();
		return;
	}
	sound_id = 7u;
	play_sound(sound_id, &ignored, &ignored_2);
	block = spawn_lava_column(block);
	if ((OBJECT(current_object)->sprite_flags & 0xD000u) != 0xD000u)
	{
		lava_rock_draw_at_previous_floor();
		return;
	}
	OBJECT(current_object)->x_int += 0x71u;
	spawn_lava_column(block);
	OBJECT(current_object)->x_int -= 0x71u;
	lava_rock_draw_at_previous_floor();
}

/* Draws a lava rock when on screen (screen_x -0xA0 to 0x1E0): a platform, draw_depth the screen y of
   previous_floor_z + depth_offset. */
void lava_rock_draw_at_previous_floor(void)
{
	uint32_t ignored = 0, sprite_depth = 0;

	if ((uint16_t)(OBJECT(current_object)->screen_x + 0xA0u) >= 0x281u)
		return;
	add_to_platform_list();
	sprite_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + OBJECT(current_object)->lava_rock.depth_offset);
	OBJECT(current_object)->draw_depth = sprite_depth;
	queue_sprite_at_depth(&ignored, &sprite_depth);
}

/* Spawns a lava column 0xE0 below the rock: lava_column_parts, box 0x74 x 0x30, height 0xE0, kind 0x14,
   lava_column_animation, side 1 from x 0xC97, lava_column_start. For a rock with takes_slot 1 it is
   tossed instead (lava_column_toss_start, speed_z -0xE0000, box 0x70 x 0x60). Hands back the part list
   block; block is only scratch. */
uint32_t spawn_lava_column(uint32_t block)
{
	uint16_t side = 0;
	uint32_t free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	if ((uint16_t)free_object_stack_2[0] != 0u)
	{
		allocate_object(&free_count, &block);
		if (free_count == 0u)
			return block;
		OBJECT(block)->sprite_flags = 0xD006u;
		OBJECT(block)->flags = 2u;
		OBJECT(block)->sprite_offset_x = 0;
		OBJECT(block)->sprite_offset_y = 0;
		OBJECT(block)->handler = lava_column_start_ADDRESS;
		OBJECT(block)->direction = 0x200u;
		OBJECT(block)->speed_x = 0;
		OBJECT(block)->speed_y = 0;
		OBJECT(block)->speed_z = 0;
		OBJECT(block)->lava_column.standing = 0;
		OBJECT(block)->parent = OBJECT(current_object)->parent;
		OBJECT(block)->x = OBJECT(current_object)->x;
		OBJECT(block)->y = OBJECT(current_object)->y;
		OBJECT(block)->z = OBJECT(current_object)->z;
		OBJECT(block)->z += 0xE00000u;
		OBJECT(block)->kind = 0x14u;
		OBJECT(block)->hitbox_size_x = 0x74u;
		OBJECT(block)->hitbox_size_y = 0x30u;
		OBJECT(block)->hitbox_height = 0xE0u;
		OBJECT(block)->hitbox_offset_x = 0;
		OBJECT(block)->hitbox_offset_y = 0;
		OBJECT(block)->animation = lava_column_animation_ADDRESS;
		OBJECT(block)->animation_speed = 2u;
		OBJECT(block)->animation_timer = 0;
		OBJECT(block)->animation_frame = 0u;
		side = 0u;
		if (OBJECT(block)->x_int >= 0xC97)
			side = 1u;
		OBJECT(block)->lava_column.side = side;
		if (OBJECT(current_object)->spawn_lava_column.takes_slot == 1u)
		{
			OBJECT(block)->handler = lava_column_toss_start_ADDRESS;
			OBJECT(block)->speed_z = -0xE0000;
			OBJECT(block)->lava_column.depth_offset = OBJECT(current_object)->spawn_lava_column.depth_offset;
			OBJECT(block)->floor_z = OBJECT(current_object)->previous_floor_z;
			OBJECT(block)->hitbox_size_x = 0x70u;
			OBJECT(block)->hitbox_size_y = 0x60u;
		}
		object = block;
		allocate_object_2(&ignored, &ignored_2, &block);
		move_halfwords_backward(block, lava_column_parts_ADDRESS, 9u);
		OBJECT(object)->part_list = block;
	}
	return block;
}

/* Projects the new lava column, then lava_column_rise. */
void lava_column_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = lava_column_rise_ADDRESS;
	lava_column_rise();
}

/* A lava column erupting under a rock: rises (speed_z - 0x6000 a frame) to z 0x570, then stands as
   lava_columns[side]. Removed off the left of the screen, while map_screen_active is set or when another
   column has taken its side; animates its parts, touchable. */
void lava_column_rise(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, sprite_depth = 0;

	if (OBJECT(current_object)->screen_x <= -0x88)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (map_screen_active != 0u || (OBJECT(current_object)->lava_column.standing < 0 && lava_columns[OBJECT(current_object)->lava_column.side] != (uint16_t)current_object))
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	if (OBJECT(current_object)->lava_column.standing >= 0)
	{
		OBJECT(current_object)->speed_z -= 0x6000u;
		OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	}
	update_screen_position(&ignored_2, &ignored_3);
	if (OBJECT(current_object)->z_int <= 0x570)
	{
		OBJECT(current_object)->lava_column.standing = -1;
		lava_columns[OBJECT(current_object)->lava_column.side] = current_object;
	}
	animate_all_parts(&ignored_4, &ignored_5);
	add_to_touchable_list(ignored_2);
	if (OBJECT(current_object)->lava_column.standing < 0)
	{
		depth = 0xFFF1u;
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	}
	sprite_depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x128u);
	if ((int16_t)sprite_depth <= -0xF)
		sprite_depth = 0xFFF2u;
	queue_sprite_at_depth(&ignored_2, &sprite_depth);
}

/* Projects the tossed lava column, then lava_column_toss. */
void lava_column_toss_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = lava_column_toss_ADDRESS;
	lava_column_toss();
}

/* A lava column thrown up from a slot-triggered rock (speed_z -0xE0000, 0x6000 a frame back down),
   animating its parts, touchable, sorted by its floor; removed off the left of the screen. Back at z
   0x660 it frees its part list and lands (lava_column_land). */
void lava_column_toss(void)
{
	uint16_t part_list = 0;
	uint32_t depth = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	if (OBJECT(current_object)->screen_x <= -0x88)
	{
		part_list = (uint16_t)OBJECT(current_object)->part_list;
		free_object_2(part_list);
		delete_current_object();
		return;
	}
	OBJECT(current_object)->speed_z += 0x6000u;
	OBJECT(current_object)->z += OBJECT(current_object)->speed_z;
	if (OBJECT(current_object)->z_int < 0x660)
	{
		update_screen_position(&ignored_2, &ignored_3);
		animate_all_parts(&ignored_4, &ignored_5);
		add_to_touchable_list(ignored_2);
		depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16) - view_y + OBJECT(current_object)->lava_column.depth_offset);
		queue_sprite_at_depth(&ignored_2, &depth);
		return;
	}
	OBJECT(current_object)->handler = lava_column_land_ADDRESS;
	part_list = (uint16_t)OBJECT(current_object)->part_list;
	free_object_2(part_list);
	lava_column_land();
}

/* Turns the tossed lava column into a lava bubble 0x40 along x from it (base_x 0x40,
   lava_column_become_bubble). */
void lava_column_land(void)
{
	OBJECT(current_object)->lava_column.base_x = 0x40;
	lava_column_become_bubble();
}

/* Unused: lava_column_land with base_x 0, the bubble at the column's own x. */
void lava_column_land_in_place(void)
{
	OBJECT(current_object)->lava_column.base_x = 0;
	lava_column_become_bubble();
}

/* Turns the landed lava column into a lava bubble: base_x += x_int, base_y = y_int, z 0x540, palette
   0xD000, bubble_animation, handler bubble_near_parent_update. */
void lava_column_become_bubble(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->lava_column.base_x += OBJECT(current_object)->x_int;
	OBJECT(current_object)->lava_column.base_y = OBJECT(current_object)->y_int;
	OBJECT(current_object)->z = 0x5400000;
	OBJECT(current_object)->sprite_flags = 0x1004u;
	OBJECT(current_object)->sprite_palette = 0xD000u;
	OBJECT(current_object)->sprite_offset_x = 0;
	OBJECT(current_object)->sprite_offset_y = 0;
	OBJECT(current_object)->flags = 0u;
	OBJECT(current_object)->handler = bubble_near_parent_update_ADDRESS;
	OBJECT(current_object)->animation_frame = 0u;
	OBJECT(current_object)->animation = bubble_animation_ADDRESS;
	OBJECT(current_object)->animation_timer = 0;
	OBJECT(current_object)->animation_speed = 4u;
	project_object(&ignored, &ignored_2);
}

/* Spawns the object that fires the lava rocks' group slots: next_trigger = group_slot_triggers + the
   record's path, handler group_slot_trigger_update; clears group_slots. Hands back spawned 0xFFFFFFFF (0
   when no object was free), count 0 and the object in trigger. */
void spawn_group_slot_trigger(uint32_t record, uint32_t *p_spawned, uint32_t *p_count, uint32_t *p_trigger)
{
	uint32_t count = *p_count;
	uint32_t fill_word = 0, slot_cursor = 0;

	allocate_object_outside_loop(p_spawned, p_trigger);
	if (*p_spawned == 0u)
	{
		return;
	}
	OBJECT(*p_trigger)->group_slot_trigger.next_trigger = LEADER_SPAWN(record)->path + group_slot_triggers_ADDRESS;
	OBJECT(*p_trigger)->handler = group_slot_trigger_update_ADDRESS;
	OBJECT(*p_trigger)->flags = 0u;
	slot_cursor = group_slots_ADDRESS;
	count = 0x20u;
	fill_word = 0u;
	do
	{
		mem_write32(slot_cursor, fill_word);
		slot_cursor += 4u;
		count--;
	} while (count != 0u);
	*p_spawned = 0xFFFFFFFFu;
	*p_count = count;
}

/* Steps through group_slot_triggers (scroll position, slot, value): once scroll_position reaches the
   entry and group_slots[slot] is taken (not 0) it becomes the value | 0x8000, which makes that row of
   lava rocks erupt after the delay, and the next entry follows. Slot 0x8000 sets goal_unlocked; any
   negative slot removes the object. */
void group_slot_trigger_update(void)
{
	uint32_t cursor = 0, slot = 0;
	uint16_t trigger_scroll = 0;
	uint32_t slot_address = 0;

	cursor = OBJECT(current_object)->group_slot_trigger.next_trigger;
	trigger_scroll = GROUP_SLOT_TRIGGER_RECORD(cursor)->position;
	cursor += 2u;
	if (scroll_position < trigger_scroll)
		return;
	if (GROUP_SLOT_TRIGGER_RECORD(cursor - 0x2u)->slot >= 0)
	{
		slot = (uint16_t)GROUP_SLOT_TRIGGER_RECORD(cursor - 0x2u)->slot;
		cursor += 2u;
		if (group_slots[slot] != 0u)
		{
			group_slots[slot] = GROUP_SLOT_TRIGGER_RECORD(cursor - 0x4u)->delay;
			slot_address = group_slots_ADDRESS + slot * 2u;
			mem_write16(slot_address, mem_read16(slot_address) | 0x8000u);
			OBJECT(current_object)->group_slot_trigger.next_trigger_low = cursor + 2u;
		}
		return;
	}
	if ((uint16_t)GROUP_SLOT_TRIGGER_RECORD(cursor - 0x2u)->slot == 0x8000u)
		goal_unlocked = 0xFFu;
	delete_current_object();
}
