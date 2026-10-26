/**
 *    Name:        system/coins.c
 *    Description: coins and credits: the coin inputs and counters, the credits and start prompts drawn outside a game
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* From init_system: the settings into credit_coin_cost, credits_per_start, the chute multipliers,
   bonus_adder (1 is free_play), health_setting and starting_health; coin counters and sounds cleared;
   shared_coin_counter_word 0xFC when both multipliers match. The credit blocks get back their credits
   from saved_credits (no coins, at most 9). */
void apply_settings(void)
{
	uint16_t credit_cap = 0;
	uint32_t block_cursor = 0, credit_blocks_left = 0, credits = 0, saved_cursor = 0, vitality = 0;

	credit_coin_cost = setting_coins_per_credit;
	credits_per_start = setting_credits_to_start;
	health_setting = setting_difficulty;
	vitality = setting_initial_vitality;
	starting_health = starting_health_by_vitality[vitality & 3u];
	coin_counter_1_on = 0u;
	coin_counter_1_off = 0u;
	coin_counter_1_pending = 0u;
	chute_1_multiplier = 0u;
	chute_1_multiplier = setting_chute_1_multiplier;
	coin_counter_2_on = 0u;
	coin_counter_2_off = 0u;
	coin_counter_2_pending = 0u;
	chute_2_multiplier = 0u;
	chute_2_multiplier = setting_chute_2_multiplier;
	service_coin_sounds = 0u;
	coin_sounds = 0u;
	shared_coin_counter_word = (uint8_t)((chute_2_multiplier == chute_1_multiplier) * 0xFCu);
	bonus_adder = setting_bonus_adder;
	free_play = 0u;
	if (bonus_adder == 1u)
		free_play = 1u;
	credit_cap = 0x900u;
	saved_cursor = saved_credits;
	block_cursor = p1_coins_ADDRESS;
	credit_blocks_left = 3u;
	do
	{
		credits = mem_read16(saved_cursor);
		credits = credits & ~0xFFu;
		saved_cursor += 2u;
		if ((int16_t)credit_cap < (int16_t)credits)
			credits = credit_cap;
		CREDIT_BLOCK(block_cursor)->counts_long = credits;
		block_cursor += 4u;
		credit_blocks_left--;
	} while (credit_blocks_left != 0u);
}

/* At each VBLANK: clears the blocks' changed flags; unless free_play, plays one pending coin and one
   service sound (0x7F), gives every block in use a credit on a service press (inputs_pressed 0x1001),
   adds the coins of the chutes released (bits 2, 3, 6) to the blocks by cabinet_config (add_coin), with
   the bookkeeping, and pulses the coin counters. */
void update_coins(void)
{
	uint8_t changed_flag = 0, max_credits = 0, sound_id = 0;
	uint32_t block_cursor = 0, bookkeeping_chute = 0, chute_offset = 0, coin_credit_block = 0, coin_credits_added = 0, coin_inputs = 0, counter_bit = 0, counter_state = 0, credit_block = 0, credit_blocks_left = 0, ignored = 0, ignored_2 = 0, pending_offset = 0, service_credits_added = 0;
	uint32_t coins_released = 0;

	block_cursor = p1_coins_ADDRESS;
	changed_flag = 0u;
	credit_blocks_left = 3u;
	do
	{
		CREDIT_BLOCK(block_cursor)->changed = changed_flag;
		block_cursor += 4u;
		credit_blocks_left--;
	} while (credit_blocks_left != 0u);
	if (free_play != 0u)
		return;
	if (coin_sounds != 0u)
	{
		sound_id = 0x7Fu;
		queue_sound(sound_id, &ignored, &ignored_2);
		coin_sounds--;
	}
	if (service_coin_sounds != 0u)
	{
		sound_id = 0x7Fu;
		queue_sound(sound_id, &ignored, &ignored_2);
		service_coin_sounds--;
	}
	max_credits = 9u;
	service_credits_added = 0u;
	if ((inputs_pressed & 0x1001u) != 0u)
	{
		switch (cabinet_config)
		{
		case 0:
		case 1:
			break;
		case 2:
			credit_block = p2_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			break;
		case 3:
		case 4:
			credit_block = p3_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			credit_block = p2_coins_ADDRESS;
			service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
			break;
		default:
			invalid_jump(0xFF7CDu + cabinet_config * 2u);
		}
		credit_block = p1_coins_ADDRESS;
		service_credits_added = add_service_credit(service_credits_added, credit_block, max_credits);
		mem_write16(bookkeeping_service_credits, mem_read16(bookkeeping_service_credits) + service_credits_added);
	}
	coin_credits_added = 0u;
	coins_released = inputs_released & 0x4Cu;
	coin_inputs = coins_released;
	if (coins_released == 0u)
	{
		if (shared_coin_counter == 0u)
		{
			counter_state = coin_counter_2_on_ADDRESS;
			counter_bit = 1u;
			pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
		}
		counter_state = coin_counter_1_on_ADDRESS;
		counter_bit = 0u;
		pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
		return;
	}
	coin_credit_block = p1_coins_ADDRESS;
	pending_offset = 0x7408u;
	chute_offset = 0u;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if ((coin_inputs & 8u) != 0u)
		{
			bookkeeping_chute = bookkeeping_coin_chute_2;
			if (shared_coin_counter == 0u)
			{
				mem_write8(cabinet_events, 0x11u);
				pending_offset = 0x740Cu;
				chute_offset = 4u;
			}
			else
				mem_write8(cabinet_events, 0x11u);
			coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
			pending_offset = 0x7408u;
			chute_offset = 0u;
		}
		break;
	case 2:
		if ((coin_inputs & 8u) != 0u)
		{
			mem_write8(cabinet_events, 0x11u);
			bookkeeping_chute = bookkeeping_coin_chute_2;
			coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
		}
		coin_credit_block += 4u;
		break;
	case 3:
		if ((coin_inputs & 0x40u) != 0u)
		{
			mem_write8(cabinet_events, 0x11u);
			bookkeeping_chute = bookkeeping_coin_chute_3;
			coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
		}
		coin_credit_block += 4u;
		if ((coin_inputs & 8u) != 0u)
		{
			mem_write8(cabinet_events, 0x11u);
			bookkeeping_chute = bookkeeping_coin_chute_2;
			coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
		}
		coin_credit_block += 4u;
		break;
	default:
		invalid_jump(0xFF821u + cabinet_config * 3u);
	}
	if ((coin_inputs & 4u) != 0u)
	{
		mem_write8(cabinet_events, 0x11u);
		bookkeeping_chute = bookkeeping_coin_chute_1;
		coin_credits_added = add_coin(chute_offset, pending_offset, bookkeeping_chute, coin_credits_added, coin_credit_block, max_credits);
	}
	mem_write16(bookkeeping_coin_credits, mem_read16(bookkeeping_coin_credits) + coin_credits_added);
	if (shared_coin_counter == 0u)
	{
		counter_state = coin_counter_2_on_ADDRESS;
		counter_bit = 1u;
		pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
	}
	counter_state = coin_counter_1_on_ADDRESS;
	counter_bit = 0u;
	pulse_coin_counter(counter_bit, counter_state, (uint16_t)service_credits_added, (uint16_t)coin_credits_added);
}

/* Pulses a coin counter (io_port_d bit counter_bit) from its state at counter_state: on for 6 frames,
   off for 6, once per pending coin. Then saves the credits (save_credits) when service_credits or
   coin_credits were added this frame. */
void pulse_coin_counter(uint32_t counter_bit, uint32_t counter_state, uint16_t service_credits, uint16_t coin_credits)
{
	uint32_t pulse_timers = 0;
	uint8_t on_frames = 0;
	uint32_t port_word_before_on = 0, port_word_before_off = 0;

	pulse_timers = COIN_COUNTER(counter_state)->pulse_timers;
	counter_state += 2u;
	if (pulse_timers == 0u)
	{
		if (COIN_COUNTER(counter_state - 0x2u)->pending_coins != 0u)
		{
			COIN_COUNTER(counter_state - 0x2u)->pending_coins--;
			port_word_before_on = mem_read32(io_port_d);
			mem_write32(io_port_d, port_word_before_on | shl32(1u, (int)(counter_bit)));
			COIN_COUNTER(counter_state - 0x2u)->pulse_timers = 0x606u;
		}
	}
	else if ((uint8_t)pulse_timers == 0u)
	{
		counter_state--;
		COIN_COUNTER(counter_state - 0x1u)->off_frames--;
	}
	else
	{
		on_frames = COIN_COUNTER(counter_state - 0x2u)->on_frames;
		COIN_COUNTER(counter_state - 0x2u)->on_frames = on_frames - 1u;
		if (on_frames == 1u)
		{
			port_word_before_off = mem_read32(io_port_d);
			mem_write32(io_port_d, port_word_before_off & ~shl32(1u, (int)(counter_bit)));
		}
	}
	if ((uint16_t)service_credits != 0u)
	{
		save_credits();
		return;
	}
	if ((uint16_t)coin_credits == 0u)
		return;
	save_credits();
}

/* Gives credit_block a service credit unless it already has max_credits: changed flag, credits + 1,
   service_coin_sounds + 1, and its coins and bonus_coins cleared when it reaches the maximum. Hands back
   service_credits + 1, or unchanged when full. */
uint32_t add_service_credit(uint32_t service_credits, uint32_t credit_block, uint8_t max_credits)
{
	if ((uint8_t)max_credits <= CREDIT_BLOCK(credit_block)->credits)
		return service_credits;
	CREDIT_BLOCK(credit_block)->changed = 1u;
	service_credits = (service_credits & ~0xFFFFu) | (uint16_t)(service_credits + 1u);
	CREDIT_BLOCK(credit_block)->credits++;
	service_coin_sounds++;
	if ((uint8_t)max_credits <= CREDIT_BLOCK(credit_block)->credits)
	{
		CREDIT_BLOCK(credit_block)->bonus_coins = 0u;
		CREDIT_BLOCK(credit_block)->coins = 0u;
	}
	return service_credits;
}

/* Counts a coin: bookkeeping_chute + 1, a pulse more pending at pending_offset for the coin counter.
   Unless credit_block has max_credits: changed flag, coin_sounds + 1, and the chute's multiplier
   (chute_offset) of coins, plus a bonus coin every bonus_adder, become credits at credit_coin_cost each,
   up to max_credits. Hands back coin_credits plus those. */
uint32_t add_coin(uint32_t chute_offset, uint32_t pending_offset, uint32_t bookkeeping_chute, uint32_t coin_credits, uint32_t credit_block, uint8_t max_credits)
{
	uint8_t chute_multiplier = 0, coin_cost = 0, coin_tally = 0;
	uint32_t coins_per_bonus = 0;
	uint32_t counter_pending = 0;

	mem_write16(bookkeeping_chute, mem_read16(bookkeeping_chute) + 1u);
	counter_pending = GAME_VARIABLES + pending_offset;
	mem_write8(counter_pending, mem_read8(counter_pending) + 1u);
	if ((uint8_t)max_credits <= CREDIT_BLOCK(credit_block)->credits)
		return coin_credits;
	CREDIT_BLOCK(credit_block)->changed = 1u;
	coin_sounds++;
	chute_multiplier = mem_read8(chute_1_multiplier_ADDRESS + chute_offset);
	coins_per_bonus = bonus_adder;
	if (coins_per_bonus != 0u)
	{
		coin_tally = (uint8_t)(CREDIT_BLOCK(credit_block)->bonus_coins + chute_multiplier);
		while (coins_per_bonus <= coin_tally)
		{
			CREDIT_BLOCK(credit_block)->coins++;
			coin_tally -= coins_per_bonus;
		}
		CREDIT_BLOCK(credit_block)->bonus_coins = coin_tally;
	}
	coin_cost = credit_coin_cost;
	coin_tally = (uint8_t)(CREDIT_BLOCK(credit_block)->coins + chute_multiplier);
	do
	{
		if (coin_tally < coin_cost)
		{
			CREDIT_BLOCK(credit_block)->coins = coin_tally;
			return coin_credits;
		}
		coin_tally -= coin_cost;
		coin_credits = (coin_credits & ~0xFFFFu) | (uint16_t)(coin_credits + 1u);
		CREDIT_BLOCK(credit_block)->credits++;
	} while (CREDIT_BLOCK(credit_block)->credits < (uint8_t)max_credits);
	CREDIT_BLOCK(credit_block)->credits = max_credits;
	CREDIT_BLOCK(credit_block)->bonus_coins = 0u;
	CREDIT_BLOCK(credit_block)->coins = 0u;
	return coin_credits;
}

/* Copies the coins and credits of the three credit blocks to saved_credits, then
   update_bookkeeping_checksum, then copies them to saved_credits_backup. */
void save_credits(void)
{
	uint32_t credit_block = 0, dest = 0;

	dest = saved_credits;
	credit_block = p1_coins_ADDRESS;
	copy_credit_counts(dest, credit_block);
	update_bookkeeping_checksum();
	dest = saved_credits_backup;
	credit_block = p1_coins_ADDRESS;
	copy_credit_counts(dest, credit_block);
}

/* Copies the counts halfword (coins and credits) of three credit blocks, 4 bytes apart from
   credit_block, to dest. */
void copy_credit_counts(uint32_t dest, uint32_t credit_block)
{
	r14 = 3u;
	do
	{
		mem_write16(dest, CREDIT_BLOCK(credit_block)->counts);
		credit_block += 4u;
		r14 = r14 - 1u;
		dest += 2u;
	} while (r14 != 0u);
}

/* Draws the credits outside a game, by cabinet_config: with common chutes (0, 1, 4) block 1's count, the
   big prompt over the demos (blink_demo_prompt) and "2 credits 1 play" when credits_per_start is 2 (not
   in free play); with individual chutes each player's block: its count (draw_credit_count) and start
   prompt (blink_start_prompt). */
void draw_attract_credits(uint32_t *p_credits, uint32_t *p_blink_bit, uint32_t *p_message_text, uint32_t *p_text_position)
{
	uint32_t credit_block = 0, ignored = 0, prompt_texts = 0, text_position = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | common_credit_position;
		draw_credit_count(credit_block, p_credits, p_blink_bit, p_message_text, &text_position);
		*p_credits = blink_demo_prompt(*p_credits, credit_block);
		if (credits_per_start == 2u && free_play == 0u)
		{
			*p_message_text = two_credits_one_play_text_ADDRESS;
			print_text(p_message_text, &text_position);
		}
		*p_text_position = text_position;
		return;
	case 2:
		credit_block = p2_coins_ADDRESS;
		text_position = 0x310000u | credit_position_p3;
		draw_credit_count(credit_block, p_credits, p_blink_bit, &ignored, &text_position);
		text_position = 0x310000u | start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, p_blink_bit, &ignored, &text_position);
		credit_block = p1_coins_ADDRESS;
		text_position = 0x310000u | credit_positions_3p[0];
		draw_credit_count(credit_block, p_credits, p_blink_bit, p_message_text, &text_position);
		text_position = 0x310000u | start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		blink_start_prompt(credit_block, prompt_texts, p_blink_bit, p_message_text, &text_position);
		*p_text_position = text_position;
		return;
	case 3:
		break;
	default:
		invalid_jump(0xFFBA6u + cabinet_config * 3u);
	}
	credit_block = p3_coins_ADDRESS;
	text_position = 0x310000u | credit_position_p3;
	draw_credit_count(credit_block, p_credits, p_blink_bit, p_message_text, &text_position);
	text_position = 0x310000u | start_prompt_position_p3;
	prompt_texts = start_prompt_texts_ADDRESS;
	blink_start_prompt(credit_block, prompt_texts, p_blink_bit, &ignored, &text_position);
	credit_block = p2_coins_ADDRESS;
	text_position = 0x310000u | credit_position_p2;
	draw_credit_count(credit_block, p_credits, p_blink_bit, &ignored, &text_position);
	text_position = 0x310000u | start_prompt_position_p2;
	prompt_texts = start_prompt_texts_ADDRESS;
	blink_start_prompt(credit_block, prompt_texts, p_blink_bit, &ignored, &text_position);
	credit_block = p1_coins_ADDRESS;
	text_position = 0x310000u | credit_positions_3p[0];
	draw_credit_count(credit_block, p_credits, p_blink_bit, p_message_text, &text_position);
	text_position = 0x310000u | start_prompt_positions_3p[0];
	prompt_texts = start_prompt_texts_ADDRESS;
	blink_start_prompt(credit_block, prompt_texts, p_blink_bit, p_message_text, &text_position);
	*p_text_position = text_position;
}

/* Unused: the undo of draw_attract_credits by cabinet_config: erase_credit_count and erase_start_prompt
   at the common or per-player positions. It takes the positions without the 0x310000 base, so its writes
   would miss the text map, and with individual chutes they are not all draw_attract_credits' ones. */
void erase_attract_credits(void)
{
	uint32_t prompt_texts = 0, text_position = 0;

	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		text_position = common_credit_position;
		erase_credit_count(text_position);
		text_position = common_start_prompt_position;
		prompt_texts = common_start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		return;
	case 2:
		text_position = credit_position_p2;
		erase_credit_count(text_position);
		text_position = start_prompt_position_p2;
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		text_position = credit_position_p3;
		erase_credit_count(text_position);
		text_position = start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		return;
	case 3:
		text_position = credit_position_p3;
		erase_credit_count(text_position);
		text_position = start_prompt_position_p3;
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		text_position = credit_position_p2;
		erase_credit_count(text_position);
		text_position = start_prompt_position_p2;
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		text_position = credit_position_p3;
		erase_credit_count(text_position);
		text_position = start_prompt_positions_3p[0];
		prompt_texts = start_prompt_texts_ADDRESS;
		erase_start_prompt(text_position, prompt_texts);
		return;
	default:
		invalid_jump(0x9FC96u + cabinet_config * 2u);
	}
}

/* Every 16th frame redraws credit_block's start prompt at text_position: erased (io_port_d bits 2-3
   cleared) while bit 5 - frames_per_wait of frame_counter is set; else "Push start button" with
   free_play or enough credits, "Insert coin" with nothing in, or "Insert more coin" ("2 credits start"
   by turns when 2 are needed). Its cabinet_config test changes nothing. */
void blink_start_prompt(uint32_t credit_block, uint32_t prompt_texts, uint32_t *p_blink_bit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint16_t credits = 0;
	uint32_t prompt_index = 0;
	uint8_t port_d_bits = 0;

	if ((frame_counter & 0xFu) != 0u)
		return;
	prompt_index = 0u;
	*p_blink_bit = (uint8_t)(5u - frames_per_wait);
	if ((frame_counter_long & shl32(1u, (int)(*p_blink_bit))) != 0u)
	{
		port_d_bits = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits & ~0xCu);
		*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
		erase_text_lines(p_text, p_text_position);
		return;
	}
	if (free_play != 0u)
	{
		if ((cabinet_config & 1u) != 0u || setting_moving_seat != 0u)
		{
			*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
			print_text_lines(p_text, p_text_position);
			return;
		}
		r14 = p1_coins_ADDRESS;
		*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
		print_text_lines(p_text, p_text_position);
		return;
	}
	credits = CREDIT_BLOCK(credit_block)->credits;
	if (credits_per_start <= credits)
	{
		if ((cabinet_config & 1u) != 0u || setting_moving_seat != 0u)
		{
			*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
			print_text_lines(p_text, p_text_position);
			return;
		}
		r14 = p1_coins_ADDRESS;
		*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
		print_text_lines(p_text, p_text_position);
		return;
	}
	prompt_index = 1u;
	if (credits_per_start == 2u)
	{
		(*p_blink_bit)++;
		if ((frame_counter_long & shl32(1u, (int)(*p_blink_bit))) != 0u)
			prompt_index = 3u;
	}
	if (CREDIT_BLOCK(credit_block)->coins != 0u || credits != 0u)
	{
		*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
		print_text_lines(p_text, p_text_position);
		return;
	}
	prompt_index = 2u;
	*p_text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[prompt_index];
	print_text_lines(p_text, p_text_position);
}

/* Unused: clears io_port_d bits 2-3 and erases prompt 1 of prompt_texts ("Insert more coin") at
   text_position (erase_text_lines). */
void erase_start_prompt(uint32_t text_position, uint32_t prompt_texts)
{
	uint32_t text = 0;
	uint8_t port_d_bits = 0;

	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~0xCu);
	text = prompt_texts + START_PROMPT_TEXT_OFFSETS(prompt_texts)[1];
	erase_text_lines(&text, &text_position);
}

/* Redraws credit_block's count at text_position (draw_credit_count_now) the first time, while its bit of
   stage_init_flags (0xF, 0x12, 0x13 for blocks 1 to 3) is clear, or when the block's changed flag is
   set. */
void draw_credit_count(uint32_t credit_block, uint32_t *p_credits, uint32_t *p_coins_per_credit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t drawn_bit = 0;
	uint32_t old_flags = 0;

	drawn_bit = 0xFu;
	*p_text = p1_coins_ADDRESS;
	if (credit_block != p1_coins_ADDRESS)
	{
		drawn_bit = 0x12u;
		*p_text = p2_coins_ADDRESS;
		if (credit_block != p2_coins_ADDRESS)
			drawn_bit = 0x13u;
	}
	old_flags = stage_init_flags;
	stage_init_flags = old_flags | shl32(1u, (int)(drawn_bit));
	if ((old_flags & shl32(1u, (int)(drawn_bit))) == 0u || CREDIT_BLOCK(credit_block)->changed != 0u)
	{
		draw_credit_count_now(credit_block, p_credits, p_coins_per_credit, p_text, p_text_position);
		return;
	}
}

/* Draws credit_block's count at text_position (kept in credit_text_position): "Free play" with
   free_play; else "credit" ("credits" from 2), the count as two BCD digits (one below 10), then the
   coins toward the next credit as a small fraction over credit_coin_cost (draw_fraction), cleared when
   none or when a credit costs 1 coin. */
void draw_credit_count_now(uint32_t credit_block, uint32_t *p_credits, uint32_t *p_coins_per_credit, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t credits = *p_credits;
	uint32_t text_position = *p_text_position;
	uint8_t attribute = 0;
	uint16_t tens = 0;
	uint32_t ignored = 0, plural_cell = 0;
	uint8_t tens_nibble = 0;

	credit_text_position = text_position;
	if (free_play != 0u)
	{
		*p_text = free_play_text_ADDRESS;
		print_text_here(&ignored, p_text, &text_position);
		*p_credits = credits;
		*p_text_position = text_position;
		return;
	}
	*p_text = credits_label_text_ADDRESS;
	print_text_here(&plural_cell, p_text, &text_position);
	credits = (credits & ~0xFFu) | CREDIT_BLOCK(credit_block)->credits;
	if ((uint8_t)credits >= 2u)
	{
		plural_cell = (plural_cell & ~0xFFu) | 0x73u;
		mem_write16(text_position - 4u, plural_cell);
	}
	tens = (uint16_t)((plural_cell & ~0xFFu) | (uint8_t)credits);
	tens = (uint16_t)((tens & ~0xFFu) | ((uint8_t)tens / 0xAu));
	credits = (credits & ~0xFFu) | ((uint8_t)credits % 0xAu);
	tens_nibble = (uint32_t)tens << 4;
	tens = (uint16_t)((tens & ~0xFFu) | tens_nibble);
	if (tens_nibble == 0u)
	{
		mem_write16(text_position + 2u, 0u);
		text_position = (text_position & ~0xFFFFu) | (uint16_t)(text_position - 2u);
	}
	tens = (uint16_t)((tens & ~0xFFu) | (uint8_t)(tens | credits));
	credits = (credits & ~0xFFFFu) | (uint16_t)tens;
	draw_credit_digits(tens, &credits, &text_position);
	text_position = (text_position & ~0xFFFFu) | (uint16_t)(text_position + 4u);
	*p_coins_per_credit = credit_coin_cost;
	if (*p_coins_per_credit != 1u)
	{
		credits = CREDIT_BLOCK(credit_block)->coins;
		if (credits != 0u)
		{
			attribute = 0u;
			draw_fraction(attribute, &credits, p_coins_per_credit, &text_position);
			*p_credits = credits;
			*p_text_position = text_position;
			return;
		}
	}
	mem_write32(text_position, 0u);
	*p_credits = credits;
	*p_text_position = text_position;
}

/* Unused: hands back the pixel rows of a small fraction: numerator_top_rows and numerator_bottom_rows
   from fraction_digit_font for numerator, 3 rows higher, and denominator_top_rows and
   denominator_bottom_rows for denominator ORed with the slash (fraction_slash_top_rows,
   fraction_slash_bottom_rows). */
void build_fraction_glyph(uint32_t numerator, uint32_t denominator, uint32_t *p_numerator_top_rows, uint32_t *p_numerator_bottom_rows, uint32_t *p_denominator_top_rows, uint32_t *p_denominator_bottom_rows)
{
	uint32_t denominator_rows = 0;

	numerator = fraction_digit_font_ADDRESS + 0x3u + numerator * 2u * 4u;
	*p_numerator_top_rows = FRACTION_FONT_ROW_LONGS(numerator)[0];
	*p_numerator_bottom_rows = FRACTION_FONT_ROW_LONGS(numerator)[1];
	denominator_rows = fraction_digit_font_ADDRESS + denominator * 2u * 4u;
	*p_denominator_top_rows = FRACTION_FONT_ROW_LONGS(denominator_rows)[0] | fraction_slash_top_rows;
	*p_denominator_bottom_rows = FRACTION_FONT_ROW_LONGS(denominator_rows)[1] | fraction_slash_bottom_rows;
}

/* Unused: blanks the credit line at text_position: erase_text_here over credits_label_text ("credit "),
   then over the record after it, the 4 cells of the count and fraction. Only erase_attract_credits calls
   it. */
void erase_credit_count(uint32_t text_position)
{
	uint16_t cell = 0;
	uint32_t text = 0;

	text = credits_label_text_ADDRESS;
	cell = 0u;
	erase_text_here(cell, &text, &text_position);
	erase_text_here(cell, &text, &text_position);
}

/* Unused: unless free_play, adds credits_to_add to credit_pool, at most 9 (credit_pool_coins cleared at
   the cap), and copies it to bookkeeping_coin_credits and bookkeeping_backup around
   update_bookkeeping_checksum. Hands back the credits added in its low byte (0 with free play). */
uint32_t add_to_credit_pool(uint32_t credits_to_add)
{
	uint8_t credits_added = 0, excess = 0;
	uint32_t crc = 0, result = 0;
	uint8_t new_total = 0;

	if (free_play != 0u)
	{
		credits_to_add &= ~0xFFu;
		return credits_to_add;
	}
	credits_added = (uint8_t)credits_to_add;
	new_total = credits_to_add + credit_pool;
	excess = (uint8_t)(new_total - 9u);
	if ((int8_t)new_total >= 9)
	{
		credit_pool_coins = 0u;
		credits_added -= excess;
	}
	credit_pool += credits_added;
	mem_write8(bookkeeping_coin_credits, credit_pool);
	crc = update_bookkeeping_checksum();
	mem_write8(bookkeeping_backup, credit_pool);
	result = (crc & ~0xFFFFu) | credits_added;
	return result;
}

/* Unused: unless free_play, takes credits_to_take from credit_pool and copies it to
   bookkeeping_coin_credits and bookkeeping_backup around update_bookkeeping_checksum. */
void take_from_credit_pool(uint8_t credits_to_take)
{
	if (free_play == 0u)
	{
		credit_pool -= credits_to_take;
		mem_write8(bookkeeping_coin_credits, credit_pool);
		update_bookkeeping_checksum();
		mem_write8(bookkeeping_backup, credit_pool);
	}
}
