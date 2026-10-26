/**
 *    Name:        player/effects.c
 *    Description: around the players: the skid dust, the HUD (gauge, portrait, ring count, start prompt, messages), the ride of game mode 0x1C, joining with a character select, the result bonus, the climb zone splash, the intro marker
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Spawns a skid dust cloud (skid_dust_animation, skid_dust_update) at the player's projected position
   and z, with the player's sprite priority bits. */
void spawn_skid_dust(uint32_t dust)
{
	uint16_t priority_bits = 0;
	uint32_t free_count = 0;

	allocate_object(&free_count, &dust);
	if (free_count != 0u)
	{
		OBJECT(dust)->sprite_flags = 0x1000u;
		OBJECT(dust)->sprite_palette = 0x8260u;
		OBJECT(dust)->animation_frame = 0u;
		OBJECT(dust)->animation_speed = 3u;
		OBJECT(dust)->animation_timer = 3;
		OBJECT(dust)->animation = skid_dust_animation_ADDRESS;
		OBJECT(dust)->sprite_frame = FRAME_ANIMATION(OBJECT(dust)->animation)->frames[0];
		OBJECT(dust)->projected_x = OBJECT(current_object)->projected_x;
		OBJECT(dust)->projected_y = OBJECT(current_object)->projected_y;
		OBJECT(dust)->z = OBJECT(current_object)->z;
		OBJECT(dust)->sprite_offset_x = 0;
		OBJECT(dust)->sprite_offset_y = 0;
		OBJECT(dust)->handler = skid_dust_update_ADDRESS;
		priority_bits = OBJECT(current_object)->sprite_palette & 0x6000u;
		OBJECT(dust)->sprite_palette &= 0x9FFFu;
		OBJECT(dust)->sprite_palette |= priority_bits;
	}
}

/* Plays the skid dust's animation once, drawn meanwhile, then deletes it. */
void skid_dust_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, wrapped = 0;

	animate(&ignored, &wrapped, &ignored_2);
	if ((uint8_t)wrapped == 0u)
	{
		update_screen_position(&ignored_3, &ignored);
		ignored = queue_sprite(ignored);
		return;
	}
	delete_current_object();
}

/* Fills the player's three HUD sprite records in its block (hud_gauge at +0x142, hud_portrait at +0x172,
   hud_ring_effect at +0x1A2): the health gauge and the character's portrait, placed for 2 or 3 players
   by cabinet_config, and the ring sparkle beside the gauge; resets the HUD blink and stage_rings_shown. */
void player_hud_init(void)
{
	uint32_t gauge_position = 0, object = 0, player = 0, player_2 = 0, portrait_anim = 0, portrait_position = 0, portrait_sprite = 0;

	PLAYER(current_player)->hud_blink_on = 0u;
	PLAYER(current_player)->stage_rings_shown = 0xFFFFu;
	PLAYER(current_player)->hud_blink_timer = 0x1E;
	player = current_player + 0x142u;
	PLAYER(player - 0x142u)->hud_gauge_flags = 0x900Au;
	PLAYER(player - 0x142u)->hud_gauge_tile = 0x89E0u;
	PLAYER(player - 0x142u)->hud_gauge_address = SPRITE_FRAME_RECORD(LINKED(sprite_frames, PLAYER(player - 0x142u)->hud_gauge_tile))->address;
	PLAYER(player - 0x142u)->hud_gauge_health = OBJECT(current_object)->health;
	PLAYER(player - 0x142u)->hud_gauge_frame = OBJECT(current_object)->player_view.health_pad;
	player_2 = OBJECT(current_object)->player;
	gauge_position = hud_gauge_positions_3p_ADDRESS + player_2 * 4u;
	if ((cabinet_config & 1u) == 0u)
		gauge_position = hud_gauge_positions_2p_ADDRESS + player_2 * 4u;
	PLAYER(player - 0x142u)->hud_gauge_x = SCREEN_POINT(gauge_position)->x;
	PLAYER(player - 0x142u)->hud_gauge_y = SCREEN_POINT(gauge_position)->y;
	PLAYER(player - 0x142u)->hud_gauge_palette = 0x88E0u;
	PLAYER(player - 0x142u)->hud_gauge_height = 8u;
	PLAYER(current_player)->hud_gauge = player;
	PLAYER(player - 0x142u)->hud_gauge_anim = hud_gauge_hit_palettes_ADDRESS;
	PLAYER(player - 0x142u)->hud_gauge_anim_delay = 3u;
	PLAYER(player - 0x142u)->hud_gauge_frame = 0u;
	PLAYER(player - 0x142u)->hud_gauge_anim_repeats = 0xFFu;
	PLAYER(player - 0x142u)->hud_gauge_reset = 0u;
	player = current_player + 0x172u;
	PLAYER(player - 0x172u)->hud_portrait_flags = 0x100Au;
	player_2 = OBJECT(current_object)->player;
	portrait_position = hud_portrait_positions_3p_ADDRESS + player_2 * 4u;
	if ((cabinet_config & 1u) == 0u)
		portrait_position = hud_portrait_positions_2p_ADDRESS + player_2 * 4u;
	PLAYER(player - 0x172u)->hud_portrait_x = SCREEN_POINT(portrait_position)->x;
	PLAYER(player - 0x172u)->hud_portrait_y = SCREEN_POINT(portrait_position)->y;
	portrait_sprite = hud_portrait_sprites_ADDRESS + OBJECT(current_object)->kind * 4u;
	PLAYER(player - 0x172u)->hud_portrait_tile = HUD_PORTRAIT_SPRITE(portrait_sprite)->frame;
	PLAYER(player - 0x172u)->hud_portrait_palette = HUD_PORTRAIT_SPRITE(portrait_sprite)->palette;
	PLAYER(player - 0x172u)->hud_portrait_height = 0xAu;
	PLAYER(current_player)->hud_portrait = player;
	PLAYER(player - 0x172u)->hud_portrait_anim_delay = 3u;
	PLAYER(player - 0x172u)->hud_portrait_frame = 0u;
	PLAYER(player - 0x172u)->hud_portrait_anim_repeats = 0xFFu;
	portrait_anim = hud_portrait_hurt_palettes_sonic_ADDRESS;
	if (OBJECT(current_object)->kind != 0u)
	{
		portrait_anim = hud_portrait_hurt_palettes_ray_ADDRESS;
		if (OBJECT(current_object)->kind != 1u)
			portrait_anim = hud_portrait_hurt_palettes_mighty_ADDRESS;
	}
	PLAYER(player - 0x172u)->hud_portrait_anim = portrait_anim;
	player = current_player + 0x1A2u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_flags = 0x1000u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_palette = 0x1B0u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim = ring_sparkle_animation_ADDRESS;
	PLAYER(player - 0x1A2u)->hud_ring_effect_frame = 0xFFu;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim_timer = 0u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_anim_delay = 2u;
	object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
	PLAYER(player - 0x1A2u)->hud_ring_effect_x = OBJECT(object)->screen_x;
	PLAYER(player - 0x1A2u)->hud_ring_effect_x += 0x10u;
	PLAYER(player - 0x1A2u)->hud_ring_effect_y = OBJECT(object)->screen_y;
	PLAYER(current_player)->hud_ring_effect = player;
	PLAYER(current_player)->unused_hud_byte = 0u;
	PLAYER(current_player)->unused_hud_word = 0xFFFFu;
}

/* Sets the portrait's frame and palette for the player's character (kind), then player_hud_update. A
   player in play (player_status with bit 7, not 0x82) first gets its HUD message lines erased and, with
   no game in progress, its bit of stage_init_flags (15, 18 or 19) cleared. */
void player_hud_start(void)
{
	uint32_t ignored = 0, init_bit = 0, object = 0, player = 0, sprite_record = 0, text = 0;

	sprite_record = hud_portrait_sprites_ADDRESS + OBJECT(current_object)->kind * 4u;
	object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
	OBJECT(object)->sprite_frame = HUD_PORTRAIT_SPRITE(sprite_record)->frame;
	OBJECT(object)->sprite_palette = HUD_PORTRAIT_SPRITE(sprite_record)->palette;
	player = OBJECT(current_object)->player;
	if ((int8_t)player_status[player] >= 0 || player_status[player] == 0x82u)
	{
		player_hud_update(&ignored, &object);
		return;
	}
	text = hud_message_blank_text_ADDRESS;
	player = OBJECT(current_object)->player;
	object = 0x310000u | hud_text_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		object = (object & ~0xFFFFu) | hud_text_positions_2p[player];
	erase_text_lines(&text, &object);
	if (game_in_progress != 0u)
	{
		player_hud_update(&ignored, &object);
		return;
	}
	init_bit = 0xFu;
	if (OBJECT(current_object)->player != 0u)
	{
		init_bit = 0x12u;
		if (OBJECT(current_object)->player != 1u)
			init_bit = 0x13u;
	}
	stage_init_flags &= ~shl32(1u, (int)(init_bit));
	player_hud_update(&ignored, &object);
}

/* Draws the player's HUD. In play: the gauge (2 x health wide), the portrait and the ring sparkle,
   flashing their palettes when health drops, rings come (block flags bit 6) or health is 8 or less, and
   the ring count; waiting (player_status 0x82) the wait message blinks; out of play the start prompt or
   the no-continue text. */
void player_hud_update(uint32_t *p_digit_attribute, uint32_t *p_text_position)
{
	uint32_t digit = 0, message_position = 0, object = 0, player = 0, player_2 = 0, portrait_anim = 0, text = 0, value = 0, wrapped = 0;
	uint32_t flags = 0;

	player = OBJECT(current_object)->player;
	if (player_status[player] == 0x82u)
	{
		text = hud_wait_text_ADDRESS;
		*p_text_position = blink_player_hud_message(text, *p_text_position);
		OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
		return;
	}
	if (PLAYER(current_player)->state == 0x23u || PLAYER(current_player)->state == 0x2Eu || PLAYER(current_player)->state == 0x34u || PLAYER(current_player)->state == 0x24u || PLAYER(current_player)->state == 0x37u)
	{
		OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
		return;
	}
	if (PLAYER(current_player)->state == 0x2Bu)
	{
		if (PLAYER(current_player)->hud_message_shown == 0u)
		{
			OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
			return;
		}
		PLAYER(current_player)->hud_message_shown = 0u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		*p_text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			*p_text_position = (*p_text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, p_text_position);
		OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
		return;
	}
	if (PLAYER(current_player)->state == 0x39u && game_mode_timer >= 0x3C)
	{
		OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
		return;
	}
	if ((int8_t)player_status[player] < 0 || player_status[player] == 1u)
	{
		object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
		current_object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
		if ((uint8_t)OBJECT(object)->piece_frame != 0u)
		{
			OBJECT(object)->piece_frame = 0;
			OBJECT(object)->direction_low = OBJECT(object_cursor)->health;
			OBJECT(object)->hud_anim_repeats = 0;
			OBJECT(current_object)->hud_anim_repeats = 0;
			OBJECT(object)->sprite_palette = HUD_STEP_LIST(OBJECT(object)->animate_hud.anim)->steps[0];
			portrait_anim = hud_portrait_hurt_palettes_sonic_ADDRESS;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				portrait_anim = hud_portrait_hurt_palettes_ray_ADDRESS;
				if (OBJECT(object_cursor)->kind != 1u)
					portrait_anim = hud_portrait_hurt_palettes_mighty_ADDRESS;
			}
			OBJECT(current_object)->animate_hud.anim = portrait_anim;
			OBJECT(current_object)->sprite_palette = HUD_STEP_LIST(OBJECT(current_object)->animate_hud.anim)->steps[0];
		}
		flags = PLAYER(current_player)->flags;
		PLAYER(current_player)->flags = flags & ~0x40u;
		if ((flags & 0x40u) == 0u)
		{
			if (OBJECT(object_cursor)->health < (int8_t)OBJECT(object)->direction_low)
			{
				OBJECT(object)->animate_hud.anim = hud_gauge_hit_palettes_ADDRESS;
				OBJECT(object)->animate_hud.frame = 0u;
				OBJECT(object)->animate_hud.anim_timer = 0;
				OBJECT(object)->hud_anim_repeats = 3;
				portrait_anim = hud_portrait_hurt_palettes_sonic_ADDRESS;
				if (OBJECT(object_cursor)->kind != 0u)
				{
					portrait_anim = hud_portrait_hurt_palettes_ray_ADDRESS;
					if (OBJECT(object_cursor)->kind != 1u)
						portrait_anim = hud_portrait_hurt_palettes_mighty_ADDRESS;
				}
				OBJECT(current_object)->animate_hud.anim = portrait_anim;
				OBJECT(current_object)->animate_hud.frame = 0u;
				OBJECT(current_object)->animate_hud.anim_timer = 0;
				OBJECT(current_object)->hud_anim_repeats = 3;
			}
		}
		else
		{
			OBJECT(object)->animate_hud.anim = hud_gauge_ring_palettes_ADDRESS;
			OBJECT(object)->animate_hud.frame = 0u;
			OBJECT(object)->animate_hud.anim_timer = 0;
			OBJECT(object)->hud_anim_repeats = 1;
			OBJECT((current_object & ~0xFFFFu) | PLAYER(current_player)->hud_ring_effect)->direction_high = 0u;
		}
		OBJECT(object)->direction_low = OBJECT(object_cursor)->health;
		if (OBJECT(object_cursor)->health <= 8 && OBJECT(object)->hud_anim_repeats <= 0)
		{
			OBJECT(object)->animate_hud.anim = hud_gauge_low_health_palettes_ADDRESS;
			OBJECT(object)->animate_hud.frame = 0u;
			OBJECT(object)->animate_hud.anim_timer = 0;
			OBJECT(object)->hud_anim_repeats = 3;
			portrait_anim = hud_portrait_low_health_palettes_sonic_ADDRESS;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				portrait_anim = hud_portrait_low_health_palettes_ray_ADDRESS;
				if (OBJECT(object_cursor)->kind != 1u)
					portrait_anim = hud_portrait_low_health_palettes_mighty_ADDRESS;
			}
			OBJECT(current_object)->animate_hud.anim = portrait_anim;
			OBJECT(current_object)->animate_hud.frame = 0u;
			OBJECT(current_object)->animate_hud.anim_timer = 0;
			OBJECT(current_object)->hud_anim_repeats = 3;
		}
		if (game_in_progress != 0u)
		{
			value = PLAYER(current_player)->rings;
			convert_to_bcd(&value, &digit);
			*p_digit_attribute = (digit & ~0xFFFFu) | 0x800u;
			if (OBJECT(object_cursor)->kind != 0u)
			{
				*p_digit_attribute = (*p_digit_attribute & ~0xFFFFu) | 0xA00u;
				if (OBJECT(object_cursor)->kind != 1u)
					*p_digit_attribute = (*p_digit_attribute & ~0xFFFFu) | 0xC00u;
			}
			player_2 = OBJECT(object_cursor)->player;
			*p_text_position = 0x310000u | hud_ring_count_positions_3p[player_2];
			if ((cabinet_config & 1u) == 0u)
				*p_text_position = (*p_text_position & ~0xFFFFu) | hud_ring_count_positions_2p[player_2];
			draw_hex_4(&value, p_digit_attribute, p_text_position);
		}
		current_object = (current_object & ~0xFFFFu) | PLAYER(current_player)->hud_ring_effect;
		if ((int8_t)OBJECT(current_object)->direction_high >= 0)
		{
			animate_hud_tiles(p_digit_attribute, &wrapped);
			if ((int8_t)wrapped < 0)
				OBJECT(current_object)->direction_high = 0xFFu;
			queue_sprite_on_top();
		}
		current_object = LINKED(current_object, PLAYER(current_player)->hud_portrait);
		if (OBJECT(current_object)->hud_anim_repeats > 0)
		{
			animate_hud_palette(p_digit_attribute, &wrapped);
			if (wrapped != 0u)
				OBJECT(current_object)->hud_anim_repeats--;
		}
		queue_sprite_on_top();
		current_object = LINKED(current_object, PLAYER(current_player)->hud_gauge);
		if (OBJECT(current_object)->hud_anim_repeats > 0)
		{
			animate_hud_palette(p_digit_attribute, &wrapped);
			if (wrapped != 0u)
				OBJECT(current_object)->hud_anim_repeats--;
		}
		OBJECT(current_object)->sprite_width = (uint8_t)OBJECT(object_cursor)->health * 2u;
		if (OBJECT(current_object)->sprite_width > 0)
		{
			OBJECT(current_object)->sprite_width++;
			queue_sprite_on_top();
		}
		current_object = (current_object & ~0xFFFFu) | (uint16_t)object_cursor;
		return;
	}
	OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
	if (current_game_mode == 0x18u)
	{
		if (PLAYER(current_player)->hud_message_shown == 0u)
		{
			OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
			return;
		}
		PLAYER(current_player)->hud_message_shown = 0u;
		text = hud_message_blank_text_ADDRESS;
		player = OBJECT(current_object)->player;
		*p_text_position = 0x310000u | hud_text_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			*p_text_position = (*p_text_position & ~0xFFFFu) | hud_text_positions_2p[player];
		erase_text_lines(&text, p_text_position);
		OBJECT(LINKED(current_object, PLAYER(current_player)->hud_gauge))->piece_frame = -1;
		return;
	}
	if (joining_closed == 0u)
	{
		*p_text_position = draw_player_start_prompt();
		return;
	}
	if (PLAYER(current_player)->hud_message_shown != 0u)
	{
		return;
	}
	PLAYER(current_player)->hud_message_shown = 0xFFu;
	text = hud_message_blank_text_ADDRESS;
	player = OBJECT(current_object)->player;
	message_position = 0x310000u | hud_text_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		message_position = (message_position & ~0xFFFFu) | hud_text_positions_2p[player];
	erase_text_lines(&text, &message_position);
	player = OBJECT(current_object)->player;
	*p_text_position = 0x310000u | start_prompt_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		*p_text_position = (*p_text_position & ~0xFFFFu) | start_prompt_positions_2p[player];
	text = hud_no_continue_text_ADDRESS;
	print_text_lines(&text, p_text_position);
}

/* Blinks the start prompt (blink_start_prompt, start_prompt_texts) at the player's
   start_prompt_positions entry, with the credits of p1_coins, or of the player's own credit block when
   cabinet_config is 2 or 3; hands back the text position. */
uint32_t draw_player_start_prompt(void)
{
	uint32_t credit_block = 0, ignored = 0, ignored_2 = 0, player = 0, prompt_position = 0, prompt_texts = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		credit_block = p1_coins_ADDRESS;
		player = OBJECT(current_object)->player;
		prompt_position = 0x310000u | start_prompt_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			prompt_position = (prompt_position & ~0xFFFFu) | start_prompt_positions_2p[player];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &ignored, &ignored_2, &prompt_position);
		return prompt_position;
	case 2:
	case 3:
		player = OBJECT(current_object)->player;
		credit_block = p1_coins_ADDRESS + player * 4u;
		prompt_position = 0x310000u | start_prompt_positions_3p[player];
		if ((cabinet_config & 1u) == 0u)
			prompt_position = (prompt_position & ~0xFFFFu) | start_prompt_positions_2p[player];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, &ignored, &ignored_2, &prompt_position);
		return prompt_position;
	default:
		invalid_jump(0xA9E6u + cabinet_config * 2u);
	}
}

/* Every animate_hud.anim_delay frames puts the next palette of the list at animate_hud.anim (its first
   word the last index) into the HUD record's sprite_palette; at the end of the list it starts over and
   wrapped is -1. */
void animate_hud_palette(uint32_t *p_list_index, uint32_t *p_wrapped)
{
	uint32_t list_index = *p_list_index;
	uint8_t last_frame = 0;
	uint32_t palettes = 0;
	uint8_t anim_timer = 0;

	*p_wrapped = 0u;
	anim_timer = OBJECT(current_object)->animate_hud.anim_timer;
	OBJECT(current_object)->animate_hud.anim_timer = anim_timer - 1u;
	if ((int8_t)anim_timer > 1)
	{
		return;
	}
	OBJECT(current_object)->animate_hud.anim_timer = OBJECT(current_object)->animate_hud.anim_delay;
	palettes = OBJECT(current_object)->animate_hud.anim;
	last_frame = (uint8_t)HUD_PALETTE_CYCLE(palettes)->last_index;
	list_index = OBJECT(current_object)->animate_hud.frame;
	list_index = (uint8_t)(list_index + 1u);
	palettes += 2u;
	if ((int8_t)last_frame < (int8_t)list_index)
	{
		*p_wrapped = 0xFFFFFFFFu;
		list_index = 0u;
	}
	OBJECT(current_object)->animate_hud.frame = list_index;
	OBJECT(current_object)->sprite_palette = HUD_CYCLE_PALETTES(palettes)[list_index];
	*p_list_index = list_index;
}

/* As animate_hud_palette for the sprite's frame: every anim_delay frames puts the next frame of the list
   at animate_hud.anim into sprite_frame; past the last it goes back to index 0 without changing the
   frame, and wrapped is -1. */
void animate_hud_tiles(uint32_t *p_list_index, uint32_t *p_wrapped)
{
	uint32_t list_index = *p_list_index;
	uint8_t last_frame = 0;
	uint32_t frame_list = 0;
	uint8_t anim_timer = 0;

	*p_wrapped = 0u;
	anim_timer = OBJECT(current_object)->animate_hud.anim_timer;
	OBJECT(current_object)->animate_hud.anim_timer = anim_timer - 1u;
	if ((int8_t)anim_timer > 1)
	{
		return;
	}
	OBJECT(current_object)->animate_hud.anim_timer = OBJECT(current_object)->animate_hud.anim_delay;
	frame_list = OBJECT(current_object)->animate_hud.anim;
	last_frame = (uint8_t)FRAME_ANIMATION(frame_list)->last_frame;
	list_index = OBJECT(current_object)->animate_hud.frame;
	list_index = (uint8_t)(list_index + 1u);
	frame_list += 2u;
	if ((int8_t)list_index <= (int8_t)last_frame)
		OBJECT(current_object)->sprite_frame = SPRITE_FRAME_NUMBERS(frame_list)[list_index];
	else
	{
		*p_wrapped = 0xFFFFFFFFu;
		list_index = 0u;
	}
	OBJECT(current_object)->animate_hud.frame = list_index;
	*p_list_index = list_index;
}

/* Every 0x1E frames (hud_blink_timer) toggles hud_blink_on and prints or erases text at the player's HUD
   prompt position (hud_prompt_positions_2p/3p); hands back that position, or text_position in between. */
uint32_t blink_player_hud_message(uint32_t text, uint32_t text_position)
{
	uint32_t player = 0, prompt_position = 0;
	uint8_t hud_blink_timer = 0;

	hud_blink_timer = PLAYER(current_player)->hud_blink_timer;
	PLAYER(current_player)->hud_blink_timer = hud_blink_timer - 1u;
	if ((int8_t)hud_blink_timer > 1)
		return text_position;
	PLAYER(current_player)->hud_blink_timer = 0x1E;
	PLAYER(current_player)->hud_blink_on = ~PLAYER(current_player)->hud_blink_on;
	player = OBJECT(current_object)->player;
	prompt_position = 0x310000u | hud_prompt_positions_3p[player];
	if ((cabinet_config & 1u) == 0u)
		prompt_position = (prompt_position & ~0xFFFFu) | hud_prompt_positions_2p[player];
	if (PLAYER(current_player)->hud_blink_on == 0u)
	{
		erase_text_lines(&text, &prompt_position);
		return prompt_position;
	}
	print_text_lines(&text, &prompt_position);
	return prompt_position;
}

/* Unused: when stage_rings differs from stage_rings_shown, copies it there and draws it as two hex
   digits (draw_hex_2, attribute 0) at the player's hud_stage_rings_positions entry. */
void draw_player_stage_rings(void)
{
	uint16_t attribute = 0;
	uint32_t text_position = 0, value = 0;

	if (PLAYER(current_player)->stage_rings_shown != PLAYER(current_player)->stage_rings)
	{
		PLAYER(current_player)->stage_rings_shown = PLAYER(current_player)->stage_rings;
		text_position = 0x310000u | hud_stage_rings_positions[OBJECT(current_object)->player];
		attribute = 0u;
		value = PLAYER(current_player)->stage_rings_shown;
		draw_hex_2(value, attribute, text_position);
	}
}

/* With no ridden_object yet, when the player's x is past 0xAE0 or before 0xA60, spawns the object it
   will ride (ridden_object_start) at that x, y 0xEB2, z 0x530, and keeps it in ridden_object. */
uint32_t spawn_player_ride(uint32_t object)
{
	uint32_t free_count = 0;

	if (PLAYER(current_player)->ridden_object != 0u)
		return object;
	if (OBJECT(current_object)->x_int > 0xAE0)
	{
		ride_start_x = 0xAE00000u;
		allocate_object(&free_count, &object);
		if (free_count != 0u)
		{
			OBJECT(object)->x = ride_start_x;
			OBJECT(object)->y = 0xEB20000;
			OBJECT(object)->z = 0x5300000;
			OBJECT(object)->handler = ridden_object_start_ADDRESS;
			PLAYER(current_player)->ridden_object = object;
		}
		return object;
	}
	if (OBJECT(current_object)->x_int >= 0xA60)
		return object;
	ride_start_x = 0xA600000u;
	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->x = ride_start_x;
		OBJECT(object)->y = 0xEB20000;
		OBJECT(object)->z = 0x5300000;
		OBJECT(object)->handler = ridden_object_start_ADDRESS;
		PLAYER(current_player)->ridden_object = object;
	}
	return object;
}

/* The joining player's character select (state 0x34): the first frame starts the chooser
   (start_join_character_chooser) and notes join_time (game_frames, reset when only one player counts as
   playing); each frame runs join_character_chooser_update and is done (substate and is_joining 0) once
   the player's bit leaves players_in_join_chooser. */
void player_select_character(uint32_t player_number)
{
	uint8_t playing_count = 0;
	uint32_t player_bit = 0, players_left = 0, status_cursor = 0;

	if (PLAYER(current_player)->substate == 0u)
	{
		PLAYER(current_player)->is_joining = 0xFFu;
		PLAYER(current_player)->substate = 0xFFu;
		PLAYER(current_player)->state = 0x34u;
		status_cursor = player_status_ADDRESS;
		players_left = 3u;
		playing_count = 0u;
		do
		{
			if ((int8_t)mem_read8(status_cursor) < 0 || mem_read8(status_cursor) == 1u || mem_read8(status_cursor) == 4u)
				playing_count++;
			status_cursor++;
			players_left--;
		} while (players_left != 0u);
		if (playing_count == 1u)
			game_frames = 0u;
		PLAYER(current_player)->join_time = game_frames;
		player_number = start_join_character_chooser();
	}
	join_character_chooser_update(player_number);
	player_bit = 0u;
	if (OBJECT(current_object)->player != 0u)
	{
		player_bit = 1u;
		if (OBJECT(current_object)->player != 1u)
			player_bit = 2u;
	}
	if ((players_in_join_chooser & shl32(1u, (int)(player_bit))) == 0u)
	{
		PLAYER(current_player)->substate = 0u;
		PLAYER(current_player)->is_joining = 0u;
	}
}

/* The ring tally's bonus for the player block player: health + 8 (at most max_health_by_setting for its
   joined_health_setting), rings + 2 x ring_tally_percent, and block flags bit 6, so the HUD flashes for
   rings. */
void give_result_bonus(uint32_t player)
{
	uint8_t max_health = 0;
	uint32_t joined_setting = 0, player_object = 0;

	player_object = LINKED(player, PLAYER(player)->object);
	OBJECT(player_object)->health += 8u;
	joined_setting = PLAYER(player)->joined_health_setting;
	max_health = (uint8_t)max_health_by_setting[(uint16_t)(joined_setting * 2u)];
	if ((int8_t)max_health < OBJECT(player_object)->health)
		OBJECT(player_object)->health = max_health;
	PLAYER(player)->flags |= 0x40u;
	PLAYER(player)->rings += ring_tally_percent * 2u;
}

/* Spawns the climb zone splash (climb_zone_effect_animation) at x 0x1CC, z 0x4CC and the player's y; its
   parent is the player block (current_player). It starts in climb_zone_effect_start. */
void spawn_climb_zone_effect(uint32_t object)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0x1000u;
		OBJECT(object)->sprite_palette = 0x9130u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_x = 0;
		OBJECT(object)->sprite_offset_y = 0;
		OBJECT(object)->handler = climb_zone_effect_start_ADDRESS;
		OBJECT(object)->animation = climb_zone_effect_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->x = 0x1CC0000;
		OBJECT(object)->y = OBJECT(current_object)->y;
		OBJECT(object)->z = 0x4CC0000;
		OBJECT(object)->parent = current_player;
	}
}

/* Projects the climb zone splash and makes climb_zone_effect_update its handler, running it at once. */
void climb_zone_effect_start(void)
{
	uint32_t ignored = 0, ignored_2 = 0;

	project_object(&ignored, &ignored_2);
	OBJECT(current_object)->handler = climb_zone_effect_update_ADDRESS;
	climb_zone_effect_update();
}

/* The climb zone splash: deleted once its player (the block in parent) stops climbing (state 0xA) or
   leaves the splash_zone; else it follows the player's y, animates and is drawn at depth screen_y +
   0x28. */
void climb_zone_effect_update(void)
{
	uint32_t climbing_player = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	climbing_player = LINKED(current_object, OBJECT(current_object)->parent);
	if (PLAYER(climbing_player)->state != 0xAu || PLAYER(climbing_player)->splash_zone == 0u)
	{
		delete_current_object();
		return;
	}
	OBJECT(current_object)->y = OBJECT(LINKED(current_object, PLAYER(climbing_player)->object))->y;
	animate(&ignored, &ignored_2, &ignored_3);
	project_object(&ignored_4, &ignored);
	depth = (uint16_t)(OBJECT(current_object)->screen_y + 0x28u);
	queue_sprite_at_depth(&ignored_4, &depth);
}

/* Spawns the intro marker (frame 0xA0C0, intro_marker_animation), 0x50 above and 8 right of the player
   object, its parent; intro_marker_update runs it. */
void spawn_intro_marker(uint32_t object)
{
	uint32_t free_count = 0;

	allocate_object(&free_count, &object);
	if (free_count != 0u)
	{
		OBJECT(object)->sprite_flags = 0u;
		OBJECT(object)->sprite_frame = 0xA0C0u;
		OBJECT(object)->sprite_palette = 0xAB0u;
		OBJECT(object)->sprite_scale = 0x100u;
		OBJECT(object)->flags = 0u;
		OBJECT(object)->sprite_offset_y = -0x50;
		OBJECT(object)->sprite_offset_x = 8;
		OBJECT(object)->handler = intro_marker_update_ADDRESS;
		OBJECT(object)->animation = intro_marker_animation_ADDRESS;
		OBJECT(object)->animation_frame = 0u;
		OBJECT(object)->animation_speed = 3u;
		OBJECT(object)->animation_timer = 0;
		OBJECT(object)->parent = current_object;
	}
}

/* Keeps the intro marker above its player (parent): the player's screen position plus its sprite
   offsets, the y one scaled by the player's sprite_scale, which it takes; animates; deleted once
   scene_step reaches 2. */
void intro_marker_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0, scale = 0;

	if ((int8_t)scene_step >= 2)
	{
		delete_current_object();
		return;
	}
	object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->sprite_scale = OBJECT(object)->sprite_scale;
	scale = OBJECT(current_object)->sprite_scale;
	OBJECT(current_object)->screen_y += (OBJECT(current_object)->sprite_offset_y * scale) >> 8;
	OBJECT(current_object)->screen_x += OBJECT(current_object)->sprite_offset_x;
	animate(&scale, &ignored, &ignored_2);
	scale = queue_sprite(scale);
}

/* With setting_moving_seat on, sets the player's bit of io_port_d (2 for player 0, 3 for player 1; never
   player 2) while a game is in progress and it is hurt (states 0xD, 0x20, 0x28, 0x2C, 0x33), and clears
   it otherwise. */
void update_hurt_output(void)
{
	uint8_t outputs = 0, seat_bit = 0;
	uint8_t port_value = 0;

	if (setting_moving_seat == 0u || OBJECT(current_object)->player == 2u)
		return;
	port_value = mem_read8(io_port_d);
	outputs = port_value;
	seat_bit = 4u;
	if (OBJECT(current_object)->player != 0u)
		seat_bit = 8u;
	outputs &= ~(uint32_t)seat_bit;
	if (game_in_progress != 0u && (PLAYER(current_player)->state == 0xDu || PLAYER(current_player)->state == 0x20u || PLAYER(current_player)->state == 0x28u || PLAYER(current_player)->state == 0x2Cu || PLAYER(current_player)->state == 0x33u))
		outputs |= seat_bit;
	mem_write8(io_port_d, outputs);
}

/* Counts the frames scroll_position has not moved (stall_frames; never in stage 5). After 0x1C20 of them
   the player loses all its health and is hurt (state 0xD). */
void check_player_stalled(void)
{
	if (stage == 5u || PLAYER(current_player)->last_scroll != scroll_position)
	{
		PLAYER(current_player)->last_scroll = scroll_position;
		PLAYER(current_player)->stall_frames = 0;
		return;
	}
	PLAYER(current_player)->stall_frames++;
	if (PLAYER(current_player)->stall_frames >= 0x1C20)
	{
		OBJECT(current_object)->health = 0;
		PLAYER(current_player)->state = 0xDu;
		PLAYER(current_player)->substate = 0u;
	}
}
