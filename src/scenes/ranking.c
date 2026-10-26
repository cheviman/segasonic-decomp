/**
 *    Name:        scenes/ranking.c
 *    Description: the ranking: entering initials at game over, loading and resetting the table, the TOP15 display
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Starts a player's ranking entry at game over: rings below every record give OUT OF RANK! for 0x78
   frames (the block's handler field 1); else the player's ranking_entry_blocks block (three letters, an
   end mark) with rank, rings, character, stages_reached, time_left 0x4B0 (handler 0). Sets the player's
   bit in ranking_entry_players. */
void start_ranking_entry(void)
{
	uint8_t ranking_size = 0;
	uint16_t rings = 0;
	uint32_t block = 0, entry_x_index = 0, next_block = 0, object = 0, piece = 0, piece_copy = 0, pieces_left = 0, player = 0, previous_block = 0, record = 0, x_index = 0;

	PLAYER(current_player)->game_over_done = 0u;
	rings = PLAYER(current_player)->rings;
	r14 = 0xFu;
	ranking_size = 0xFu;
	record = ranking_ADDRESS;
	do
	{
		if ((int16_t)rings < RANKING_RECORD(record)->rings)
			break;
		r14 = r14 - 1u;
		record += 8u;
	} while (r14 != 0u);
	if ((int8_t)ranking_size <= (int8_t)r14)
	{
		player = OBJECT(object_cursor)->player;
		ranking_entry_players |= shl32(1u, (int)(player));
		block = ranking_entry_blocks[player];
		x_index = OBJECT(object_cursor)->player;
		if ((cabinet_config & 1u) == 0u)
			x_index += 3u;
		OBJECT(block)->screen_x = ranking_entry_x[x_index];
		OBJECT(block)->ranking_entry.out_of_rank_timer = 0x78u;
		OBJECT(block)->ranking_entry.player_block = current_player;
		OBJECT(block)->handler = 1u;
		return;
	}
	player = OBJECT(object_cursor)->player;
	ranking_entry_players |= shl32(1u, (int)(player));
	block = ranking_entry_blocks[player];
	next_block = (block & ~0xFFFFu) | (uint16_t)(block + 0xA0u);
	OBJECT(block)->child = next_block;
	object = block;
	init_object(object);
	pieces_left = 4u;
	do
	{
		piece = next_block;
		next_block = (next_block & ~0xFFFFu) | (uint16_t)(next_block + 0x30u);
		OBJECT(piece)->child = next_block;
		piece_copy = piece;
		init_object_2(piece_copy);
		pieces_left--;
	} while (pieces_left != 0u);
	OBJECT(piece)->child = 0xFFFFu;
	OBJECT(block)->ranking_entry.player_block = current_player;
	OBJECT(block)->kind = player;
	entry_x_index = player;
	if ((cabinet_config & 1u) == 0u)
		entry_x_index += 3u;
	OBJECT(block)->screen_x = ranking_entry_x[entry_x_index];
	OBJECT(block)->screen_y = 0xD0;
	OBJECT(block)->ranking_entry.trackball = ranking_entry_trackballs[player];
	OBJECT(block)->ranking_entry.button_bit = ranking_entry_buttons[player];
	OBJECT(block)->ranking_entry.time_left = 0x4B0u;
	OBJECT(block)->ranking_entry.rank = r14;
	OBJECT(block)->ranking_entry.character = OBJECT(current_object)->kind;
	OBJECT(block)->ranking_entry.rings = PLAYER(current_player)->rings;
	OBJECT(block)->ranking_entry.stages_reached = PLAYER(current_player)->stages_reached;
	OBJECT(block)->handler = 0u;
	previous_block = (piece_copy & ~0xFFFFu) | (uint16_t)block;
	block = LINKED(block, OBJECT(block)->child);
	OBJECT(block)->sprite_frame = 0xA238u;
	OBJECT(block)->screen_x = OBJECT(previous_block)->screen_x;
	OBJECT(block)->screen_y = OBJECT(previous_block)->screen_y;
	OBJECT(block)->screen_x += 0xDu;
	OBJECT(block)->sprite_palette = 0x8AD0u;
	OBJECT(block)->letter_fraction = 0x80u;
	previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
	block = LINKED(block, OBJECT(block)->child);
	OBJECT(block)->sprite_frame = 0xA238u;
	OBJECT(block)->screen_x = OBJECT(previous_block)->screen_x;
	OBJECT(block)->screen_y = OBJECT(previous_block)->screen_y;
	OBJECT(block)->screen_x += 0xDu;
	OBJECT(block)->sprite_palette = 0x8AD0u;
	OBJECT(block)->letter_fraction = 0x80u;
	previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
	block = LINKED(block, OBJECT(block)->child);
	OBJECT(block)->sprite_frame = 0xA238u;
	OBJECT(block)->screen_x = OBJECT(previous_block)->screen_x;
	OBJECT(block)->screen_y = OBJECT(previous_block)->screen_y;
	OBJECT(block)->screen_x += 0xDu;
	OBJECT(block)->sprite_palette = 0x8AD0u;
	OBJECT(block)->letter_fraction = 0x80u;
	previous_block = (previous_block & ~0xFFFFu) | (uint16_t)block;
	block = LINKED(block, OBJECT(block)->child);
	OBJECT(block)->screen_x = OBJECT(previous_block)->screen_x;
	OBJECT(block)->screen_y = OBJECT(previous_block)->screen_y;
	OBJECT(block)->screen_x += 0xDu;
	OBJECT(block)->piece_frame = 0x22;
	OBJECT(block)->sprite_frame = 0xA230u;
	OBJECT(block)->sprite_palette = 0x8AD0u;
}

/* A player's ranking entry (player state 0x2E): OUT OF RANK! blinks for out_of_rank_timer, then
   game_over_done; else the button goes to the next letter (index spawn_arg_1_high, up to 3) or on the
   end mark saves (enter_ranking_initials_save_record); enter_ranking_initials_count_down does the rest.
   jump_offset is only scratch. */
void enter_ranking_initials(uint32_t jump_offset)
{
	uint16_t y = 0;
	uint32_t blink_timer = 0, button_bit = 0, palette = 0, player = 0, scale = 0, text = 0, x = 0;
	uint16_t out_of_rank_timer = 0;

	player = OBJECT(object_cursor)->player;
	if ((ranking_entry_players & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = ranking_entry_blocks[player];
	if (OBJECT(current_object)->handler != 0u)
	{
		out_of_rank_timer = OBJECT(current_object)->ranking_entry.out_of_rank_timer;
		OBJECT(current_object)->ranking_entry.out_of_rank_timer = out_of_rank_timer - 1u;
		if (out_of_rank_timer == 0u)
		{
			PLAYER(OBJECT(current_object)->ranking_entry.player_block)->game_over_done = 1u;
			ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
			return;
		}
		blink_timer = (jump_offset & ~0xFFFFu) | OBJECT(current_object)->ranking_entry.out_of_rank_timer;
		if ((blink_timer & 3u) != 0u)
		{
			x = (blink_timer & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
			y = 0xC8u;
			palette = 0x8F70u;
			scale = 0x100u;
			text = out_of_rank_text_ADDRESS;
			queue_string_at(x, y, &palette, &scale, &text);
		}
		return;
	}
	r14 = current_object;
	button_bit = OBJECT(current_object)->ranking_entry.button_bit;
	if ((inputs_pressed & shl32(1u, (int)(button_bit))) == 0u)
	{
		enter_ranking_initials_count_down(button_bit);
		return;
	}
	if (OBJECT(current_object)->spawn_arg_1_high == 3u)
	{
		enter_ranking_initials_save_record();
		return;
	}
	OBJECT(current_object)->spawn_arg_1_high++;
	if ((int8_t)OBJECT(current_object)->spawn_arg_1_high <= 3)
	{
		enter_ranking_initials_count_down(button_bit);
		return;
	}
	OBJECT(current_object)->spawn_arg_1_high = 3u;
	enter_ranking_initials_count_down(button_bit);
}

/* The ranking entry's frame: time_left counts down (at 0 the record is saved), with the seconds and the
   rank drawn above; trackball x moves between the letters (spawn_arg_1's high byte), trackball y changes
   the chosen one (codes 0-0x2B, into initials); the letters and the end mark are drawn, the chosen one
   blinking. button_bit is only scratch. */
void enter_ranking_initials_count_down(uint32_t button_bit)
{
	uint16_t abs_dx = 0, abs_dy = 0, dx = 0, dy = 0, y = 0;
	uint32_t countdown_x = 0, letters_left = 0, links_left = 0, object = 0, palette = 0, scale = 0, slot = 0, text = 0, text_x = 0, trackball = 0;
	uint16_t time_left = 0, slot_position = 0, letter_position = 0, direction = 0;

	time_left = OBJECT(current_object)->ranking_entry.time_left;
	OBJECT(current_object)->ranking_entry.time_left = time_left - 1u;
	if (time_left == 0u)
	{
		enter_ranking_initials_save_record();
		return;
	}
	countdown_x = (button_bit & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
	y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	palette = 0x8AD0u;
	scale = 0x100u;
	text = countdown_second_texts_ADDRESS + (int32_t)OBJECT(current_object)->ranking_entry.time_left / 0x3C * 3u;
	queue_string_at(countdown_x, y, &palette, &scale, &text);
	text_x = (countdown_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
	text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x + 0x20u);
	y = (uint16_t)(OBJECT(current_object)->screen_y - 0x20u);
	palette = 0x8AD0u;
	scale = 0x100u;
	text = ranking_rank_texts_ADDRESS + OBJECT(current_object)->ranking_entry.rank * 0xAu;
	queue_string_at(text_x, y, &palette, &scale, &text);
	trackball = OBJECT(current_object)->ranking_entry.trackball;
	abs_dx = (uint16_t)TRACKBALL_MOTION(trackball)->x;
	dx = (uint16_t)abs_dx;
	trackball += 2u;
	if ((int16_t)abs_dx < 0)
		abs_dx = (uint16_t)(0u - abs_dx);
	abs_dy = (uint16_t)TRACKBALL_MOTION(trackball - 0x2u)->y;
	dy = (uint16_t)abs_dy;
	if ((int16_t)abs_dy < 0)
		abs_dy = (uint16_t)(0u - abs_dy);
	if ((int16_t)abs_dy < (int16_t)abs_dx)
	{
		slot_position = OBJECT(current_object)->spawn_arg_1 + dx;
		OBJECT(current_object)->spawn_arg_1 = slot_position;
		if ((int16_t)slot_position < 0)
			OBJECT(current_object)->spawn_arg_1 = 0u;
		if (OBJECT(current_object)->spawn_arg_2_high != OBJECT(current_object)->spawn_arg_1_high)
		{
			OBJECT(current_object)->spawn_arg_1_low = 0x80u;
			OBJECT(current_object)->spawn_arg_2 = OBJECT(current_object)->spawn_arg_1;
		}
	}
	links_left = OBJECT(current_object)->spawn_arg_1_high;
	slot = links_left;
	links_left = (uint8_t)(links_left + 1u);
	if ((int8_t)links_left > 4)
	{
		OBJECT(current_object)->spawn_arg_1_high = 3u;
		links_left = 4u;
		slot = 3u;
	}
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		links_left--;
	} while (links_left != 0u);
	OBJECT(current_object)->piece_value = 1u;
	if (slot != 3u && dy != 0u)
	{
		letter_position = OBJECT(current_object)->projected_x + dy;
		OBJECT(current_object)->projected_x = letter_position;
		if ((int16_t)letter_position < 0)
		{
			OBJECT(current_object)->letter_code = 0x2B;
			OBJECT(current_object)->hud_anim_repeats = 0;
		}
		if (OBJECT(current_object)->letter_code > 0x2B)
			OBJECT(current_object)->letter_code = 0;
		if ((uint8_t)OBJECT(current_object)->piece_frame != (uint8_t)OBJECT(current_object)->letter_code)
			OBJECT(current_object)->letter_fraction = 0x80u;
		OBJECT(current_object)->projected_y_fraction = OBJECT(current_object)->projected_x;
		mem_write8(ranking_initial_fields[slot] + r14, OBJECT(current_object)->letter_code);
	}
	current_object = ranking_entry_blocks[OBJECT(object_cursor)->player];
	letters_left = 3u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		if (OBJECT(current_object)->piece_value == 0u)
			queue_sprite_on_top();
		else
		{
			OBJECT(current_object)->piece_value = 0u;
			OBJECT(current_object)->sprite_frame = ranking_letter_sprites[(uint8_t)OBJECT(current_object)->letter_code];
			OBJECT(current_object)->direction++;
			if ((OBJECT(current_object)->direction & 3u) != 0u)
				queue_sprite_on_top();
		}
		object = current_object;
		letters_left--;
	} while (letters_left != 0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(current_object)->piece_value != 0u)
	{
		OBJECT(current_object)->piece_value = 0u;
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction != 0u)
		{
			OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
			OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
			OBJECT(current_object)->screen_x += 0x18u;
			queue_sprite_on_top();
		}
		else
			OBJECT(current_object)->direction = 3u;
		current_object = object_cursor;
		return;
	}
	OBJECT(current_object)->screen_x = OBJECT(object)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object)->screen_y;
	OBJECT(current_object)->screen_x += 0x18u;
	queue_sprite_on_top();
	current_object = object_cursor;
}

/* Ends the entry (game_over_done): if the rings beat the lowest record, the lower records move down one
   and the new one goes in (initials 'sex', 'xxx', 'x x' and 'xx ' become 'tak'), ranking is copied to
   saved_ranking and ranking_checksum recomputed. Clears the player's bit in ranking_entry_players. */
void enter_ranking_initials_save_record(void)
{
	uint16_t rings = 0;
	uint32_t copy_dest = 0, copy_source = 0, count = 0, crc = 0, data = 0, dest = 0, ignored = 0, initials = 0, read_cursor = 0, record_initials = 0, record_rings = 0, records_left = 0;

	PLAYER(OBJECT(current_object)->ranking_entry.player_block)->game_over_done = 1u;
	rings = (uint16_t)OBJECT(current_object)->ranking_entry.rings;
	read_cursor = ranking_ADDRESS;
	if ((int16_t)rings < (int16_t)ranking)
	{
		current_object = object_cursor;
		ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
		return;
	}
	dest = read_cursor;
	r14 = 0xEu;
	read_cursor += 8u;
	do
	{
		record_rings = RANKING_RECORD(read_cursor)->rings_long;
		read_cursor += 4u;
		if ((int16_t)rings < (int16_t)record_rings)
			break;
		record_initials = RANKING_RECORD(read_cursor - 0x4u)->initials;
		RANKING_RECORD(dest)->rings_long = record_rings;
		dest += 4u;
		RANKING_RECORD(dest - 0x4u)->initials = record_initials;
		r14 = r14 - 1u;
		read_cursor += 4u;
		dest += 4u;
	} while (r14 != 0u);
	initials = OBJECT(current_object)->ranking_entry.initials;
	if (initials == INITIALS_SEX || initials == INITIALS_XXX || initials == INITIALS_X_X || initials == INITIALS_XX_)
		OBJECT(current_object)->ranking_entry.initials = INITIALS_TAK;
	RANKING_RECORD(dest)->rings = OBJECT(current_object)->ranking_entry.rings;
	dest += 2u;
	RANKING_RECORD(dest - 0x2u)->character = OBJECT(current_object)->ranking_entry.character;
	dest++;
	RANKING_RECORD(dest - 0x3u)->stages_reached = OBJECT(current_object)->ranking_entry.stages_reached;
	dest++;
	RANKING_RECORD(dest - 0x4u)->first_initial = OBJECT(current_object)->ranking_entry.first_initial;
	dest++;
	RANKING_RECORD(dest - 0x5u)->second_initial = OBJECT(current_object)->ranking_entry.second_initial;
	dest++;
	RANKING_RECORD(dest - 0x6u)->third_initial = OBJECT(current_object)->ranking_entry.third_initial;
	RANKING_RECORD(dest - 0x6u)->initials_pad = OBJECT(current_object)->ranking_entry.initials_pad;
	copy_source = ranking_ADDRESS;
	copy_dest = saved_ranking;
	records_left = 0xFu;
	do
	{
		mem_write32(copy_dest, RANKING_RECORD(copy_source)->rings_long);
		copy_source += 4u;
		copy_dest += 4u;
		mem_write32(copy_dest, RANKING_RECORD(copy_source - 0x4u)->initials);
		records_left--;
		copy_source += 4u;
		copy_dest += 4u;
	} while (records_left != 0u);
	data = ranking_ADDRESS;
	count = 0x78u;
	compute_crc16(count, &crc, &data, &ignored);
	mem_write16(ranking_checksum, crc);
	current_object = object_cursor;
	ranking_entry_players &= ~shl32(1u, (int)(OBJECT(object_cursor)->player));
}

/* Loads ranking from saved_ranking when compute_crc16 of it (0x78 bytes) matches ranking_checksum, else
   reset_ranking; hands back what copy_ranking_records does. */
uint32_t load_ranking(void)
{
	uint32_t count = 0, count_2 = 0, crc = 0, data = 0, dest = 0, ignored = 0, source = 0;

	data = saved_ranking;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &ignored);
	if (mem_read16(ranking_checksum) != (uint16_t)crc)
		return reset_ranking();
	source = saved_ranking;
	dest = ranking_ADDRESS;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

/* Fills ranking with default_ranking's 15 records (their ASCII initials through
   ascii_to_ranking_letter), then copies it to saved_ranking with ranking_checksum
   (copy_ranking_records). */
uint32_t reset_ranking(void)
{
	uint32_t count = 0, dest = 0, letter = 0, read_cursor = 0, records_left = 0, source = 0, write_cursor = 0;
	uint8_t second_letter = 0, third_letter = 0, last_byte = 0;

	read_cursor = default_ranking_ADDRESS;
	write_cursor = ranking_ADDRESS;
	records_left = 0xFu;
	do
	{
		RANKING_RECORD(write_cursor)->rings = DEFAULT_RANKING_RECORD(read_cursor)->rings;
		write_cursor += 2u;
		read_cursor += 2u;
		RANKING_RECORD(write_cursor - 0x2u)->character_and_stages = DEFAULT_RANKING_RECORD(read_cursor - 0x2u)->character_and_stages;
		write_cursor += 2u;
		read_cursor += 2u;
		letter = DEFAULT_RANKING_RECORD(read_cursor - 0x4u)->first_letter;
		read_cursor++;
		letter = ascii_to_ranking_letter(letter);
		RANKING_RECORD(write_cursor - 0x4u)->first_initial = letter;
		second_letter = DEFAULT_RANKING_RECORD(read_cursor - 0x5u)->second_letter;
		read_cursor++;
		letter = second_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		RANKING_RECORD(write_cursor - 0x5u)->second_initial = letter;
		third_letter = DEFAULT_RANKING_RECORD(read_cursor - 0x6u)->third_letter;
		read_cursor++;
		letter = third_letter;
		write_cursor++;
		letter = ascii_to_ranking_letter(letter);
		RANKING_RECORD(write_cursor - 0x6u)->third_initial = letter;
		last_byte = DEFAULT_RANKING_RECORD(read_cursor - 0x7u)->letters_pad;
		read_cursor++;
		write_cursor++;
		RANKING_RECORD(write_cursor - 0x7u)->initials_pad = last_byte;
		records_left--;
		write_cursor++;
	} while (records_left != 0u);
	source = ranking_ADDRESS;
	dest = saved_ranking;
	count = 0xFu;
	return copy_ranking_records(source, dest, count);
}

/* Copies count 8-byte ranking records from source to dest, then ranking_checksum = compute_crc16 of
   ranking (0x78 bytes); hands back compute_crc16's polynomial. */
uint32_t copy_ranking_records(uint32_t source, uint32_t dest, uint32_t count)
{
	uint32_t count_2 = 0, crc = 0, data = 0, polynomial = 0;

	do
	{
		mem_write32(dest, mem_read32(source));
		source += 4u;
		dest += 4u;
		mem_write32(dest, mem_read32(source));
		count--;
		source += 4u;
		dest += 4u;
	} while (count != 0u);
	data = ranking_ADDRESS;
	count_2 = 0x78u;
	compute_crc16(count_2, &crc, &data, &polynomial);
	mem_write16(ranking_checksum, crc);
	return polynomial;
}

/* Turns an initial of default_ranking into a ranking letter code: 'a'-'z' to 0-0x19, ' ' to 0x2B, '3' to
   0x23; any other character hangs in an endless loop. Hands back letter with its low byte replaced. */
uint32_t ascii_to_ranking_letter(uint32_t letter)
{
	if ((int8_t)letter >= 0x61)
	{
		letter = (letter & ~0xFFu) | (uint8_t)(letter - 0x61u);
		return letter;
	}
	if ((uint8_t)letter == 0x20u)
	{
		letter = (letter & ~0xFFu) | 0x2Bu;
		return letter;
	}
	do
	{
	} while ((uint8_t)letter != 0x33u);
	letter = (letter & ~0xFFu) | 0x23u;
	return letter;
}

/* Starts the TOP15 screen: the display object (allocate_object_or_start_list, display) at (0xA0, -0x1A4)
   with the NBG0 picture shown (show_nbg0_picture, whose rows and source it hands back); handler
   build_ranking_display. */
void start_ranking_display(uint32_t display, uint32_t *p_rows, uint32_t *p_source)
{
	uint32_t ignored = 0, ignored_2 = 0, object = 0;

	display = allocate_object_or_start_list(display);
	OBJECT(display)->parent = object_cursor;
	OBJECT(display)->child = 0xFFFFu;
	init_object(display);
	object = display;
	show_nbg0_picture(&ignored, p_rows, p_source, &ignored_2);
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = -0x1A4;
	OBJECT(object)->handler = build_ranking_display_ADDRESS;
}

/* Builds the TOP15 screen once: for each of the 15 records (lowest first, rows 15 up to 1) a row of 12
   sprites (rank, initials, portrait, name, four score digits, stage) run by ranking_row_update,
   current_player counting the rows; the five title letters (ranking_title_spin_in), ranking_title_layout
   toggled; then ranking_display_scroll_in. */
void build_ranking_display(void)
{
	uint32_t block = 0, character = 0, first_initial = 0, free_count = 0, hundreds_digit = 0, ignored = 0, ignored_2 = 0, label_second_char = 0, object = 0, rank_label = 0, record = 0, remainder = 0, rows_left = 0, second_initial = 0, stages = 0, tens_digit = 0, third_initial = 0, thousands_digit = 0;

	record = ranking_ADDRESS;
	current_player = 0u;
	rows_left = 0xFu;
	OBJECT(current_object)->speed_z = 0x180;
	OBJECT(current_object)->handler = ranking_display_scroll_in_ADDRESS;
	do
	{
		spawn_count = 0xCu;
		spawn_object_with_pieces(0x08F28Bu, &ignored, &ignored_2, &free_count, &object, &block);
		OBJECT(object)->screen_x = 0xC;
		rank_label = ranking_row_numbers_ADDRESS + ((current_player & ~0xFFu) | (uint8_t)(current_player * 2u));
		OBJECT(object)->sprite_frame = ranking_letter_sprites[RANKING_ROW_NUMBER(rank_label)->tens_code];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->ranking_row.row = rows_left;
		OBJECT(object)->handler = ranking_row_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x18;
		label_second_char = RANKING_ROW_NUMBER(rank_label)->units_code;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[label_second_char];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		remainder = (uint16_t)RANKING_RECORD(record)->rings;
		remainder = remainder & 0x7FFFu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xC0;
		thousands_digit = (int32_t)remainder / 0x3E8;
		thousands_digit = (thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[thousands_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		record += 2u;
		if ((uint8_t)thousands_digit != 0x20u)
			remainder -= ((thousands_digit & ~0xFFu) | (uint8_t)(thousands_digit - 0x20u)) * 0x3E8u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xCC;
		hundreds_digit = (int32_t)(uint16_t)remainder / 0x64;
		hundreds_digit = (hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[hundreds_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)hundreds_digit != 0x20u)
			remainder -= ((hundreds_digit & ~0xFFu) | (uint8_t)(hundreds_digit - 0x20u)) * 0x64u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xD8;
		tens_digit = (int32_t)(uint16_t)remainder / 0xA;
		tens_digit = (tens_digit & ~0xFFu) | (uint8_t)(tens_digit + 0x20u);
		OBJECT(object)->sprite_frame = ranking_letter_sprites[tens_digit];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		if ((uint8_t)tens_digit != 0x20u)
			remainder -= ((tens_digit & ~0xFFu) | (uint8_t)(tens_digit - 0x20u)) * 0xAu;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0xE4;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[(remainder & ~0xFFu) | (uint8_t)(remainder + 0x20u)];
		OBJECT(object)->sprite_palette = 0x8AC0u;
		character = RANKING_RECORD(record - 0x2u)->character;
		character = character % 3u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->sprite_flags |= 0x100u;
		OBJECT(object)->screen_x = 0x68;
		OBJECT(object)->sprite_palette = ranking_portrait_palettes[character];
		OBJECT(object)->piece_value = character;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x96;
		OBJECT(object)->sprite_frame = ranking_name_sprites[character];
		OBJECT(object)->sprite_palette = 0x8B10u;
		record++;
		stages = RANKING_RECORD(record - 0x3u)->stages_reached;
		stages = stages % 9u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x114;
		OBJECT(object)->sprite_frame = ranking_stage_sprites[(uint8_t)(stages - 1u)];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x30;
		record++;
		first_initial = RANKING_RECORD(record - 0x4u)->first_initial;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[first_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x3C;
		record++;
		second_initial = RANKING_RECORD(record - 0x5u)->second_initial;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[second_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x48;
		record++;
		third_initial = RANKING_RECORD(record - 0x6u)->third_initial;
		OBJECT(object)->sprite_frame = ranking_letter_sprites[third_initial % 0x2Cu];
		OBJECT(object)->sprite_palette = 0x8AD0u;
		current_player = (current_player & ~0xFFu) | (uint8_t)(current_player + 1u);
		rows_left--;
		record += 2u;
	} while (rows_left != 0u);
	spawn_count = 5u;
	spawn_object_with_pieces(0x08F482u, &ignored, &ignored_2, &free_count, &object, &block);
	ranking_title_layout++;
	ranking_title_layout &= 1u;
	OBJECT(object)->screen_x = 0xA0;
	OBJECT(object)->screen_y = 0x70;
	OBJECT(object)->sprite_scale = 0x10u;
	OBJECT(object)->sprite_frame = 0xA2B0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	OBJECT(object)->handler = ranking_title_spin_in_ADDRESS;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2A8u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA340u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA2D0u;
	OBJECT(object)->sprite_palette = 0x8B20u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = 0xA360u;
	OBJECT(object)->sprite_palette = 0x8B20u;
}

/* The TOP15 letters spin in: scale + 2 up to 0x200, direction + 8 a frame, speed_x/speed_y =
   direction_to_speed(0x2000) of it (their spread); at full scale with direction 0x300 they pause
   (ranking_title_pause, 0x1C frames). */
void ranking_title_spin_in(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;

	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200)
		OBJECT(current_object)->sprite_scale += 2u;
	OBJECT(current_object)->direction += 8u;
	OBJECT(current_object)->direction &= 0x3FFu;
	direction = OBJECT(current_object)->direction;
	speed = 0x2000u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x200 || OBJECT(current_object)->direction != 0x300u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->ranking_title.pause_timer = 0x1C;
	OBJECT(current_object)->handler = ranking_title_pause_ADDRESS;
	ranking_title_pause();
}

/* Holds the TOP15 title for pause_timer (0x1C) frames, then ranking_title_rise. */
void ranking_title_pause(void)
{
	uint16_t pause_timer = 0;

	pause_timer = OBJECT(current_object)->ranking_title.pause_timer;
	OBJECT(current_object)->ranking_title.pause_timer = pause_timer - 1u;
	if (pause_timer != 0u)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->handler = ranking_title_rise_ADDRESS;
	ranking_title_rise();
}

/* Moves the TOP15 title up 1 pixel a frame to screen y 0x23, then leaves it to draw_ranking_title. */
void ranking_title_rise(void)
{
	OBJECT(current_object)->screen_y--;
	if (OBJECT(current_object)->screen_y > 0x23)
	{
		draw_ranking_title();
		return;
	}
	OBJECT(current_object)->screen_y = 0x23;
	OBJECT(current_object)->handler = draw_ranking_title_ADDRESS;
	draw_ranking_title();
}

/* Draws the five TOP15 letters at the first one's scale: the centre one, two at +-(speed_x, speed_y),
   and the outer two at +-2 speed_x with +-2 speed_y added along x (ranking_title_layout 0) or y (1). */
void draw_ranking_title(void)
{
	uint32_t depth = 0, layout_offset = 0;

	depth = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(object_cursor)->speed_x_int;
	OBJECT(current_object)->screen_y += OBJECT(object_cursor)->speed_y_int;
	depth = queue_sprite(depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= OBJECT(object_cursor)->speed_x_int;
	OBJECT(current_object)->screen_y -= OBJECT(object_cursor)->speed_y_int;
	depth = queue_sprite(depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x += OBJECT(object_cursor)->speed_x_int * 2u;
	layout_offset = (depth & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->speed_y_int;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)(layout_offset * 2u);
	if (ranking_title_layout == 0u)
		OBJECT(current_object)->screen_x += layout_offset;
	else
		OBJECT(current_object)->screen_y += layout_offset;
	layout_offset = queue_sprite(layout_offset);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_scale = OBJECT(object_cursor)->sprite_scale;
	OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	OBJECT(current_object)->screen_x -= OBJECT(object_cursor)->speed_x_int * 2u;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)OBJECT(object_cursor)->speed_y_int;
	layout_offset = (layout_offset & ~0xFFFFu) | (uint16_t)(layout_offset * 2u);
	if (ranking_title_layout == 0u)
		OBJECT(current_object)->screen_x -= layout_offset;
	else
		OBJECT(current_object)->screen_y -= layout_offset;
	queue_sprite(layout_offset);
}

/* Brings the TOP15 screen down 1 pixel a frame (screen_y up to 0x24) while the background drifts; then
   the portraits animate (rows_animate) and ranking_display_hold. */
void ranking_display_scroll_in(void)
{
	OBJECT(current_object)->screen_y++;
	if (OBJECT(current_object)->screen_y < 0x24)
	{
		drift_ranking_background();
		return;
	}
	OBJECT(current_object)->ranking_display.rows_animate = 1u;
	OBJECT(current_object)->handler = ranking_display_hold_ADDRESS;
	ranking_display_hold();
}

/* Holds the TOP15 screen for 0x12C frames (hold_timer), the background drifting, then tells the rows to
   remove themselves (rows_remove) and ends with ranking_display_end. */
void ranking_display_hold(void)
{
	OBJECT(current_object)->ranking_display.hold_timer++;
	if (OBJECT(current_object)->ranking_display.hold_timer <= 0x12C)
	{
		drift_ranking_background();
		return;
	}
	OBJECT(current_object)->ranking_display.rows_remove = 1u;
	OBJECT(current_object)->ranking_display.rows_remove_pad = 0u;
	OBJECT(current_object)->handler = ranking_display_end_ADDRESS;
	drift_ranking_background();
}

/* Drifts the TOP15 background: the direction (speed_z's low half) swings between 0x180 and 0x280 by 2 a
   frame, speed_x/speed_y gain direction_to_speed(0x100) of it and NBG0 scrolls to their whole parts.
   speed's low half swings 0-0x300 as well, but the step always uses 0x100. */
void drift_ranking_background(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t speed_fraction = 0;

	if ((uint16_t)OBJECT(current_object)->speed_z_int != 0u)
	{
		OBJECT(current_object)->speed_z_fraction -= 2u;
		if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x180)
			OBJECT(current_object)->speed_z_int = 0;
	}
	else
	{
		OBJECT(current_object)->speed_z_fraction += 2u;
		if ((int16_t)OBJECT(current_object)->speed_z_fraction > 0x280)
		{
			OBJECT(current_object)->speed_z_int = 1;
			OBJECT(current_object)->speed_z_fraction -= 2u;
			if ((int16_t)OBJECT(current_object)->speed_z_fraction < 0x180)
				OBJECT(current_object)->speed_z_int = 0;
		}
	}
	if ((uint16_t)OBJECT(current_object)->speed_int != 0u)
	{
		speed_fraction = OBJECT(current_object)->speed_fraction;
		OBJECT(current_object)->speed_fraction = speed_fraction - 8u;
		if (speed_fraction < 8u)
			OBJECT(current_object)->speed = 0;
		direction = OBJECT(current_object)->speed_z_fraction;
		speed = 0x100u;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
		nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
		return;
	}
	OBJECT(current_object)->speed_fraction += 8u;
	if ((int16_t)OBJECT(current_object)->speed_fraction <= 0x300)
	{
		direction = OBJECT(current_object)->speed_z_fraction;
		speed = 0x100u;
		direction_to_speed(speed, &direction, &speed_x, &speed_y);
		OBJECT(current_object)->speed_x += speed_x;
		OBJECT(current_object)->speed_y += speed_y;
		nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
		nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
		return;
	}
	OBJECT(current_object)->speed_int = 1;
	speed_fraction = OBJECT(current_object)->speed_fraction;
	OBJECT(current_object)->speed_fraction = speed_fraction - 8u;
	if (speed_fraction < 8u)
		OBJECT(current_object)->speed = 0;
	direction = OBJECT(current_object)->speed_z_fraction;
	speed = 0x100u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x += speed_x;
	OBJECT(current_object)->speed_y += speed_y;
	nbg0_scroll_x_int = OBJECT(current_object)->speed_x_int;
	nbg0_scroll_y_int = OBJECT(current_object)->speed_y_int;
}

/* Sets ranking_done and removes the TOP15 display object (delete_object_and_children). */
void ranking_display_end(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;

	ranking_done = 1u;
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* A TOP15 row: at the display's screen_y + row * 0x17, its 12 sprites on top (bucket 0); once
   rows_animate is set the portrait plays its ranking_portrait_frames (the second set of 0x14 for row 1),
   one every 2 frames; removed with its sprites when rows_remove is set. */
void ranking_row_update(void)
{
	uint8_t bucket = 0;
	uint32_t frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, portrait_frames = 0;
	uint16_t direction = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->parent);
	OBJECT(object_cursor)->ranking_row.animate = OBJECT(current_object)->ranking_display.rows_animate;
	if (OBJECT(current_object)->ranking_display.rows_remove != 0u)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	OBJECT(object_cursor)->screen_y = OBJECT(current_object)->screen_y;
	current_object = object_cursor;
	OBJECT(current_object)->screen_y += OBJECT(current_object)->ranking_row.row * 0x17u;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	if (OBJECT(object_cursor)->ranking_row.animate == 0u)
	{
		portrait_frames = ranking_portrait_frames[OBJECT(current_object)->piece_value];
		frame_index = (uint8_t)OBJECT(current_object)->piece_frame;
		if (OBJECT(object_cursor)->ranking_row.row == 1u)
			frame_index += 0x14u;
		OBJECT(current_object)->sprite_frame = RANKING_PORTRAIT_FRAME_LIST(portrait_frames)[frame_index];
		if (OBJECT(object_cursor)->ranking_row.animate != 0u && OBJECT(current_object)->piece_frame < 0x13)
			OBJECT(current_object)->piece_frame++;
	}
	else
	{
		direction = OBJECT(current_object)->direction;
		OBJECT(current_object)->direction = direction - 1u;
		if (direction == 0u)
		{
			OBJECT(current_object)->direction = 1u;
			portrait_frames = ranking_portrait_frames[OBJECT(current_object)->piece_value];
			frame_index = (uint8_t)OBJECT(current_object)->piece_frame;
			if (OBJECT(object_cursor)->ranking_row.row == 1u)
				frame_index += 0x14u;
			OBJECT(current_object)->sprite_frame = RANKING_PORTRAIT_FRAME_LIST(portrait_frames)[frame_index];
			if (OBJECT(object_cursor)->ranking_row.animate != 0u && OBJECT(current_object)->piece_frame < 0x13)
				OBJECT(current_object)->piece_frame++;
		}
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->screen_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}
