/**
 *    Name:        video/text.c
 *    Description: text: the text layer's font, printing and erasing text, lines, fractions and digits, and the strings drawn with sprites
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Queues the string of a record at record: x, y, palette, scale, then the characters (queue_string, mode
   0), as the GAME OVER banner does; the y and scale passed in are not used. */
void queue_string_record(uint32_t record, uint16_t y, uint32_t scale, uint32_t *p_palette, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint8_t mode = 0;

	mode = 0u;
	queue_string(record, y, mode, &palette, &scale, p_text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
}

/* Queues the string text at screen x, y with palette and scale (queue_string, mode 1). */
void queue_string_at(uint32_t x, uint16_t y, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint32_t palette = *p_palette;
	uint32_t scale = *p_scale;
	uint8_t mode = 0;

	mode = 1u;
	queue_string(x, y, mode, &palette, &scale, p_text);
	*p_palette = (*p_palette & 0xFFFF0000u) | (palette & 0xFFFFu);
	*p_scale = (*p_scale & 0xFFFF0000u) | (scale & 0xFFFFu);
}

/* Adds a string to string_queue (x, y, palette, scale, text) for draw_queued_strings to draw with
   sprites; mode 0 instead takes x, y, palette and scale from the record at x and the text after them.
   Ignored once 0x2B strings are queued. */
void queue_string(uint32_t x, uint16_t y, uint8_t mode, uint32_t *p_palette, uint32_t *p_scale, uint32_t *p_text)
{
	uint16_t palette = (uint16_t)*p_palette;
	uint16_t scale = (uint16_t)*p_scale;
	uint32_t text = *p_text;
	uint32_t entry = 0;

	if (string_count > 0x2A)
		return;
	if ((uint8_t)mode == 0u)
	{
		text = x;
		x = STRING_RECORD(text)->x;
		text += 2u;
		y = STRING_RECORD(text - 0x2u)->y;
		text += 2u;
		palette = STRING_RECORD(text - 0x4u)->palette;
		text += 2u;
		scale = STRING_RECORD(text - 0x6u)->scale;
		text += 2u;
	}
	entry = (uint16_t)string_count * 0xCu + string_queue_ADDRESS;
	QUEUED_STRING(entry)->x = x;
	entry += 2u;
	QUEUED_STRING(entry - 0x2u)->y = y;
	entry += 2u;
	QUEUED_STRING(entry - 0x4u)->palette = palette;
	entry += 2u;
	QUEUED_STRING(entry - 0x6u)->scale = scale;
	QUEUED_STRING(entry - 0x6u)->text = text;
	string_count++;
	*p_palette = (*p_palette & 0xFFFF0000u) | palette;
	*p_scale = (*p_scale & 0xFFFF0000u) | scale;
	*p_text = text;
}

/* After build_sprite_list: turns string_queue, last string first, into sprite entries at sprite_entry,
   one per character through text_frames, all sized by the string's scale and its first visible
   character's frame (blanks only move x); then sprite_list_end_command, and string_count 0. */
void draw_queued_strings(uint16_t size, uint16_t width, uint32_t sprite_entry, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t scale = *p_scale;
	uint16_t height = (uint16_t)*p_height;
	uint8_t shift = 0;
	uint16_t advance = 0, char_x = 0, palette = 0, string_y = 0;
	uint32_t char_code = 0, cursor = 0, frame_record = 0, leading_blanks = 0, rounding = 0, text = 0;

	for (;;)
	{
		string_count--;
		cursor = (uint16_t)string_count;
		if ((int16_t)cursor < 0)
			break;
		advance = 0u;
		leading_blanks = 0u;
		rounding = 0x80u;
		shift = 0xF8u;
		frame_record = sprite_frames;
		cursor = cursor * 0xCu + string_queue_ADDRESS;
		char_x = (uint16_t)QUEUED_STRING(cursor)->x;
		cursor += 2u;
		string_y = (uint16_t)QUEUED_STRING(cursor - 0x2u)->y;
		cursor += 2u;
		palette = QUEUED_STRING(cursor - 0x4u)->palette;
		cursor += 2u;
		scale = QUEUED_STRING(cursor - 0x6u)->scale;
		text = QUEUED_STRING(cursor - 0x6u)->text;
		char_code = SPRITE_STRING_CHARS(text)[0];
		r14 = 0u;
		text++;
		do
		{
			frame_record = (frame_record & ~0xFFFFu) | text_frames[char_code];
			if ((uint16_t)frame_record != 0u)
			{
				if (r14 == 0u)
				{
					r14 = 1u;
					size = SPRITE_FRAME_RECORD(frame_record)->width;
					frame_record += 2u;
					height = SPRITE_FRAME_RECORD(frame_record - 0x2u)->height;
					width = (uint16_t)shift_by(size * scale + rounding, shift, 32);
					advance = (uint16_t)shift_by((uint8_t)height * scale + rounding, shift, 32);
					size = (uint16_t)(((uint16_t)((uint32_t)size << 6) & ~0xFFu) | (uint8_t)height);
					height = (uint16_t)((height & ~0xFFu) | advance);
					size = rotate_by(size, shift, 16);
					frame_record += 2u;
					width |= SPRITE_FRAME_RECORD(frame_record - 0x4u)->bank_bits;
					frame_record += 2u;
					if (leading_blanks != 0u)
						char_x += leading_blanks * advance;
				}
				else
					frame_record += 6u;
				leading_blanks = 0u;
				mem_write16(sprite_entry, 0xAu);
				sprite_entry += 2u;
				mem_write16(sprite_entry, size);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, string_y);
				sprite_entry += 2u;
				mem_write16(sprite_entry, char_x);
				sprite_entry += 2u;
				mem_write16(sprite_entry, SPRITE_FRAME_RECORD(frame_record - 0x6u)->address);
				sprite_entry += 2u;
				mem_write16(sprite_entry, palette);
				sprite_entry += 2u;
			}
			else
				leading_blanks++;
			char_x += advance;
			char_code = SPRITE_STRING_CHARS(text)[0];
			text++;
		} while (char_code != 0u);
	}
	string_count = 0;
	mem_write16(sprite_entry, sprite_list_end_command);
	*p_scale = scale;
	*p_cursor = cursor;
	*p_height = (*p_height & 0xFFFF0000u) | height;
}

/* Sets up the text layer: video_text_control 0x1F7 (map at 0x31F000, characters at 0x31C000), copies
   0x9A0 halfwords of text_font into the characters, expands the 0x5B 1-bit characters that follow it in
   color 1 (expand_font) and clears the text map. */
void load_text_font(uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint8_t color = 0;
	uint16_t count = 0;
	uint32_t dest = 0, dest_end = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, source = 0, source_end = 0;

	mem_write16(video_text_control, 0x1F7u);
	move_string(text_font_ADDRESS, 0x9A0u, VIDEO_RAM + 0x1C000u, 0x9A0u, 2u, 0u, 0u, 0u, &dest_end, &source_end);
	source = source_end;
	dest = dest_end;
	count = 0x5Bu;
	color = 1u;
	expand_font(color, count, source, dest, p_pixels, p_color_index);
	clear_text_layer(&ignored, &ignored_2, &ignored_3);
}

/* Zeroes the text map: 0xE0 blocks of 16 bytes from its first cell (clear_blocks). */
void clear_text_layer(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	*p_dest = TEXT_CELL(0, 0);
	*p_count = 0xE0u;
	clear_blocks(p_value, p_count, p_dest);
}

/* Expands count 1-bit characters, 8 rows of a byte, from source into 4-bit characters at dest in color &
   0xF; each row's blank pixels take the row above's pixels moved one pixel right, in pen color + 1. */
void expand_font(uint8_t color, uint32_t count, uint32_t source, uint32_t dest, uint32_t *p_pixels, uint32_t *p_color_index)
{
	uint32_t carry = 0;
	uint8_t font_row = 0;
	uint32_t char_rows_left = 0, pairs_left = 0, pixel_mask = 0, pixels = 0, set_pixels = 0, shadow_pixels = 0;

	*p_color_index = (uint8_t)color & 0xFu;
	count = (uint16_t)count;
	do
	{
		char_rows_left = 8u;
		shadow_pixels = 0u;
		do
		{
			font_row = FONT_ROWS_1BPP(source)[0];
			pairs_left = 4u;
			pixels = 0u;
			source++;
			do
			{
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				carry = (uint8_t)(font_row * 2u) < font_row;
				font_row = (uint8_t)(font_row * 2u);
				pixels = rotate_with_carry_by(pixels, 4u, carry, 32);
				pairs_left--;
			} while (pairs_left != 0u);
			pixels = (pixels << 29) | (pixels >> 3);
			set_pixels = pixels;
			pixel_mask = pixels * 0xFu;
			pixels = pixels * *p_color_index;
			mem_write32(dest, byte_swap32((shadow_pixels & ~pixel_mask) | pixels));
			shadow_pixels = (pixels + set_pixels) >> 4;
			char_rows_left--;
			dest += 4u;
		} while (char_rows_left != 0u);
		count--;
	} while (count != 0u);
	*p_pixels = pixels;
}

/* Unused: as expand_font for 2-bit characters: count characters of 8 halfword rows from source, each
   pixel (0-3) plus color & 0xF making a 4-bit pixel of the character at dest. */
void expand_font_2bpp(uint8_t color, uint32_t count, uint32_t source, uint32_t dest)
{
	uint32_t carry = 0;
	uint16_t font_row = 0;
	uint32_t color_nibbles = 0, pairs_left = 0, pixels = 0;

	color_nibbles = ((uint8_t)color & 0xFu) * 0x11111111u;
	count = (uint32_t)(uint16_t)count << 3;
	do
	{
		font_row = FONT_ROWS_2BPP(source)[0];
		pairs_left = 4u;
		pixels = 0u;
		source += 2u;
		do
		{
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = pixels * 2u + carry;
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = rotate_with_carry_by(pixels, 3u, carry, 32);
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = pixels * 2u + carry;
			carry = (uint16_t)(font_row * 2u) < font_row;
			font_row = (uint16_t)(font_row * 2u);
			pixels = rotate_with_carry_by(pixels, 3u, carry, 32);
			pairs_left--;
		} while (pairs_left != 0u);
		mem_write32(dest, byte_swap32(((pixels << 30) | (pixels >> 2)) + color_nibbles));
		count--;
		dest += 4u;
	} while (count != 0u);
}

/* Unused: the same, but pixels that are 0 stay 0 (transparent): color is added only where a pixel is
   set. */
void expand_font_2bpp_transparent(uint8_t color, uint32_t count, uint32_t pixels, uint32_t opaque_mask, uint32_t source, uint32_t dest)
{
	uint8_t pixel = 0;
	uint16_t font_row = 0;
	uint32_t color_nibbles = 0, pairs_left = 0;

	color_nibbles = ((uint8_t)color & 0xFu) * 0x11111111u;
	count = (uint32_t)(uint16_t)count << 3;
	do
	{
		font_row = FONT_ROWS_2BPP(source)[0];
		pairs_left = 4u;
		source += 2u;
		do
		{
			font_row = (uint16_t)(((uint32_t)font_row << 2) | (font_row >> 14));
			pixel = font_row & 3u;
			pixels = pixels << 4;
			pixels = (pixels & ~0xFFu) | (uint8_t)(pixels | pixel);
			opaque_mask = opaque_mask << 4;
			opaque_mask = (opaque_mask & ~0xFFu) | (uint8_t)(opaque_mask | (pixel != 0u));
			font_row = (uint16_t)(((uint32_t)font_row << 2) | (font_row >> 14));
			pixel = font_row & 3u;
			pixels = pixels << 4;
			pixels = (pixels & ~0xFFu) | (uint8_t)(pixels | pixel);
			opaque_mask = opaque_mask << 4;
			opaque_mask = (opaque_mask & ~0xFFu) | (uint8_t)(opaque_mask | (pixel != 0u));
			pairs_left--;
		} while (pairs_left != 0u);
		opaque_mask = opaque_mask * 0xFu;
		pixels = (pixels + color_nibbles) & opaque_mask;
		mem_write32(dest, byte_swap32(pixels));
		count--;
		dest += 4u;
	} while (count != 0u);
}

/* Writes a small fraction in two cells at text_position with attribute: character 0x80 + numerator, then
   0x90 + denominator (each & 0xF): the coins toward a credit over credit_coin_cost. */
void draw_fraction(uint16_t attribute, uint32_t *p_numerator, uint32_t *p_denominator, uint32_t *p_text_position)
{
	uint16_t numerator = (uint16_t)*p_numerator;
	uint16_t denominator = (uint16_t)*p_denominator;
	uint32_t text_position = *p_text_position;

	attribute = (uint16_t)((uint32_t)attribute << 8);
	numerator &= 0xFu;
	denominator &= 0xFu;
	numerator = (uint8_t)(numerator - 0x80u) | attribute;
	denominator = (uint8_t)(denominator - 0x70u) | attribute;
	mem_write16(text_position, numerator);
	text_position += 2u;
	mem_write16(text_position, denominator);
	*p_numerator = (*p_numerator & 0xFFFF0000u) | numerator;
	*p_denominator = (*p_denominator & 0xFFFF0000u) | denominator;
	*p_text_position = text_position;
}

/* Prints each print_text record from text until a 0 halfword. */
void print_text_list(uint32_t *p_text, uint32_t *p_text_position)
{
	while (PLACED_TEXT(*p_text)->position != 0u)
	{
		print_text(p_text, p_text_position);
	}
}

/* Prints a print_text record at text: its first halfword is the text map offset (0x310000 | it), then
   print_text_here writes its attribute byte and characters; hands back text past the record. */
void print_text(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t ignored = 0;

	*p_text_position = 0x310000u | PLACED_TEXT(*p_text)->position;
	*p_text += 2u;
	print_text_here(&ignored, p_text, p_text_position);
}

/* Writes text, an attribute byte then characters up to a 0, at text_position, a cell (attribute << 8 |
   character) each; hands back the last cell in cell, text past the 0 and text_position after the last
   cell. */
void print_text_here(uint32_t *p_cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint16_t cell = 0;
	uint8_t next_char = 0;

	cell = (uint16_t)((uint32_t)TEXT_LAYER_STRING(text)[0] << 8);
	text++;
	cell |= TEXT_LAYER_STRING(text)[0];
	text++;
	if ((uint8_t)cell != 0u)
	{
		do
		{
			mem_write16(*p_text_position, cell);
			next_char = TEXT_LAYER_STRING(text)[0];
			text++;
			cell = (uint16_t)((cell & ~0xFFu) | next_char);
			*p_text_position += 2u;
		} while ((uint8_t)cell != 0u);
	}
	*p_cell = (*p_cell & 0xFFFF0000u) | cell;
	*p_text = text;
}

/* Unused: erase_text for each record from text until a 0 halfword. */
void erase_text_list(uint32_t text)
{
	while (PLACED_TEXT(text)->position != 0u)
	{
		text = erase_text(text);
	}
}

/* Writes 0 over the cells the print_text record at text covers (erase_text_here at 0x310000 | its first
   halfword); hands back text past the record. */
uint32_t erase_text(uint32_t text)
{
	uint16_t cell = 0;
	uint32_t text_position = 0;

	cell = 0u;
	text_position = 0x310000u | PLACED_TEXT(text)->position;
	text += 2u;
	erase_text_here(cell, &text, &text_position);
	return text;
}

/* Writes cell over as many cells from text_position as text has characters (after its attribute byte, up
   to a 0); hands back text past the 0 and text_position after the last cell. */
void erase_text_here(uint16_t cell, uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint8_t first_char = 0, next_char = 0;

	text++;
	first_char = TEXT_LAYER_STRING(text)[0];
	text++;
	if (first_char != 0u)
	{
		do
		{
			mem_write16(*p_text_position, cell);
			next_char = TEXT_LAYER_STRING(text)[0];
			text++;
			*p_text_position += 2u;
		} while (next_char != 0u);
	}
	*p_text = text;
}

/* Writes text like print_text_here over several lines: a byte with bit 7 set starts the next line, one
   text row (0x80 bytes) below the start of the line before, instead of a character. */
void print_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint16_t text_cell = 0;
	uint32_t line_start = 0;

	line_start = text_position;
	text_cell = (uint16_t)((uint32_t)TEXT_LAYER_STRING(text)[0] << 8);
	text++;
	for (;;)
	{
		text_cell = (uint16_t)((text_cell & ~0xFFu) | TEXT_LAYER_STRING(text)[0]);
		text++;
		if ((uint8_t)text_cell == 0u)
			break;
		if ((int8_t)text_cell >= 0)
		{
			mem_write16(text_position, text_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

/* Writes 0 over the cells print_text_lines writes for text at text_position. */
void erase_text_lines(uint32_t *p_text, uint32_t *p_text_position)
{
	uint32_t text = *p_text;
	uint32_t text_position = *p_text_position;
	uint32_t negative = 0;
	uint16_t blank_cell = 0;
	uint32_t line_start = 0;
	uint8_t text_char = 0;

	line_start = text_position;
	blank_cell = 0u;
	text++;
	for (;;)
	{
		text_char = TEXT_LAYER_STRING(text)[0];
		negative = text_char >> 7;
		text++;
		if (text_char == 0u)
			break;
		if (negative == 0u)
		{
			mem_write16(text_position, blank_cell);
			text_position += 2u;
			continue;
		}
		line_start += 0x80u;
		text_position = line_start;
	}
	*p_text = text;
	*p_text_position = text_position;
}

/* Unused: prints a formatted text: a position halfword (0x310000 | it), an attribute byte, then runs of
   characters ending in 0, each followed by a code: 0 ends, 3/6/9 go on at 1, 2 or 3 cells a character,
   0xC a new attribute, 0xF/0x15/0x1B a new position and attribute and 0x12/0x18/0x1E a new attribute at
   width 1, 2 or 3, 0x21/0x24 skip cells. */
void print_formatted_text(uint32_t text)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(0, 0);
	formatted_text_position_single(text, text_position);
}

/* Unused: code 0xF and a formatted text's start: the next halfword is the new position (text_position's
   low half), then formatted_text_attribute_single. */
void formatted_text_position_single(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | PLACED_TEXT(text)->position;
	text += 2u;
	formatted_text_attribute_single(text, text_position);
}

/* Unused: codes 0xC (at width 1) and 0x12: the next byte is the attribute (the cells' high byte), then
   formatted_text_single. */
void formatted_text_attribute_single(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)FORMATTED_TEXT(text)[0] << 8);
	text++;
	formatted_text_single(cell, text, text_position);
}

/* Unused: writes a run of characters at text_position, one cell each (attribute << 8 | character), up to
   its 0, then acts on the code byte after it: 3 another run at this width, 0 ends, the others go to
   their routine. */
void formatted_text_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | FORMATTED_TEXT(text)[0]);
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				next_char = FORMATTED_TEXT(text)[0];
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = FORMATTED_TEXT(text)[0];
		text++;
		switch (code)
		{
		case 0x0u:
			break;
		case 0x3u:
			continue;
		case 0x6u:
			formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_single(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_single(cell, text, text_position);
			return;
		default:
			invalid_jump(0x996A7u + code);
		}
		if (zero)
			return;
	}
}

/* Unused: code 0x21 at width 1: text_position moves by a signed byte of cells, then the next run. */
void formatted_text_skip_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_SKIP(text)->cells * 2u;
	text++;
	formatted_text_single(cell, text, text_position);
}

/* Unused: code 0x24 at width 1: text_position moves by a signed halfword of cells, then the next run. */
void formatted_text_skip_long_single(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_LONG_SKIP(text)->cells * 2u;
	text += 2u;
	formatted_text_single(cell, text, text_position);
}

/* Unused: code 0x15: the next halfword is the new position (text_position's low half), then
   formatted_text_attribute_double. */
void formatted_text_position_double(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | PLACED_TEXT(text)->position;
	text += 2u;
	formatted_text_attribute_double(text, text_position);
}

/* Unused: codes 0xC (at width 2) and 0x18: the next byte is the attribute (the cells' high byte), then
   formatted_text_double. */
void formatted_text_attribute_double(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)FORMATTED_TEXT(text)[0] << 8);
	text++;
	formatted_text_double(cell, text, text_position);
}

/* Unused: writes a run at 2 cells a character (cell, cell + 1) up to its 0, then acts on the code byte:
   6 another run at this width, 0 ends, the others go to their routine. */
void formatted_text_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | FORMATTED_TEXT(text)[0]);
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				next_char = FORMATTED_TEXT(text)[0];
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = FORMATTED_TEXT(text)[0];
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			continue;
		case 0x9u:
			formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_double(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_double(cell, text, text_position);
			return;
		default:
			invalid_jump(0x99703u + code);
		}
	}
}

/* Unused: code 0x21 at width 2: text_position moves by a signed byte of cells, then the next run. */
void formatted_text_skip_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_SKIP(text)->cells * 2u;
	text++;
	formatted_text_double(cell, text, text_position);
}

/* Unused: code 0x24 at width 2: text_position moves by a signed halfword of cells, then the next run. */
void formatted_text_skip_long_double(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_LONG_SKIP(text)->cells * 2u;
	text += 2u;
	formatted_text_double(cell, text, text_position);
}

/* Unused: code 0x1B: the next halfword is the new position (text_position's low half), then
   formatted_text_attribute_triple. */
void formatted_text_position_triple(uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | PLACED_TEXT(text)->position;
	text += 2u;
	formatted_text_attribute_triple(text, text_position);
}

/* Unused: codes 0xC (at width 3) and 0x1E: the next byte is the attribute (the cells' high byte), then
   formatted_text_triple. */
void formatted_text_attribute_triple(uint32_t text, uint32_t text_position)
{
	uint16_t cell = 0;

	cell = (uint16_t)((uint32_t)FORMATTED_TEXT(text)[0] << 8);
	text++;
	formatted_text_triple(cell, text, text_position);
}

/* Unused: writes a run at 3 cells a character (cell, cell + 1, cell + 2) up to its 0, then acts on the
   code byte: 9 another run at this width, 0 ends, the others go to their routine. */
void formatted_text_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t next_char = 0;

	for (;;)
	{
		cell = (uint16_t)((cell & ~0xFFu) | FORMATTED_TEXT(text)[0]);
		zero = (uint8_t)cell == 0u;
		text++;
		if ((uint8_t)cell != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				cell++;
				text_position += 2u;
				mem_write16(text_position, cell);
				next_char = FORMATTED_TEXT(text)[0];
				text++;
				cell = (uint16_t)((cell & ~0xFFu) | next_char);
				zero = (uint8_t)cell == 0u;
				text_position += 2u;
			} while ((uint8_t)cell != 0u);
		}
		code = FORMATTED_TEXT(text)[0];
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			continue;
		case 0xCu:
		case 0x1Eu:
			formatted_text_attribute_triple(text, text_position);
			return;
		case 0xFu:
			formatted_text_position_single(text, text_position);
			return;
		case 0x12u:
			formatted_text_attribute_single(text, text_position);
			return;
		case 0x15u:
			formatted_text_position_double(text, text_position);
			return;
		case 0x18u:
			formatted_text_attribute_double(text, text_position);
			return;
		case 0x1Bu:
			formatted_text_position_triple(text, text_position);
			return;
		case 0x21u:
			formatted_text_skip_triple(cell, text, text_position);
			return;
		case 0x24u:
			formatted_text_skip_long_triple(cell, text, text_position);
			return;
		default:
			invalid_jump(0x99764u + code);
		}
	}
}

/* Unused: code 0x21 at width 3: text_position moves by a signed byte of cells, then the next run. */
void formatted_text_skip_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_SKIP(text)->cells * 2u;
	text++;
	formatted_text_triple(cell, text, text_position);
}

/* Unused: code 0x24 at width 3: text_position moves by a signed halfword of cells, then the next run. */
void formatted_text_skip_long_triple(uint16_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_LONG_SKIP(text)->cells * 2u;
	text += 2u;
	formatted_text_triple(cell, text, text_position);
}

/* Unused: erases what print_formatted_text prints for text, writing cell with its low half cleared over
   the cells, the attribute bytes skipped. Its code tables have no entry for code 0x24. */
void erase_formatted_text(uint32_t cell, uint32_t text)
{
	uint32_t text_position = 0;

	text_position = TEXT_CELL(0, 0);
	cell &= ~0xFFFFu;
	erase_formatted_text_position_single(cell, text, text_position);
}

/* Unused: code 0xF erased (and the text's start): the next halfword replaces text_position's low half,
   then erase_formatted_text_attribute_single. */
void erase_formatted_text_position_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | PLACED_TEXT(text)->position;
	text += 2u;
	erase_formatted_text_attribute_single(cell, text, text_position);
}

/* Unused: codes 0xC (at width 1) and 0x12 erased: skips the attribute byte, then
   erase_formatted_text_single. */
void erase_formatted_text_attribute_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_single(cell, text, text_position);
}

/* Unused: writes cell over a run of characters, one cell each, up to its 0, then acts on the code byte:
   3 another run at this width, 0 ends, the others go to their routine (0x24 has none). */
void erase_formatted_text_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = FORMATTED_TEXT(text)[0];
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write16(text_position, cell);
				next_char = FORMATTED_TEXT(text)[0];
				text++;
				zero = next_char == 0u;
				text_position += 2u;
			} while (next_char != 0u);
		}
		code = FORMATTED_TEXT(text)[0];
		text++;
		switch (code)
		{
		case 0x0u:
			break;
		case 0x3u:
			continue;
		case 0x6u:
			erase_formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			erase_formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_single(cell, text, text_position);
			return;
		default:
			invalid_jump(0x997BCu + code);
		}
		if (zero)
			return;
	}
}

/* Unused: code 0x21 erased at width 1: text_position moves by a signed byte of cells. */
void erase_formatted_text_skip_single(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_SKIP(text)->cells * 2u;
	text++;
	erase_formatted_text_single(cell, text, text_position);
}

/* Unused: code 0x15 erased: a new position halfword, then erase_formatted_text_attribute_double. */
void erase_formatted_text_position_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | PLACED_TEXT(text)->position;
	text += 2u;
	erase_formatted_text_attribute_double(cell, text, text_position);
}

/* Unused: codes 0xC (at width 2) and 0x18 erased: skips the attribute byte, then
   erase_formatted_text_double. */
void erase_formatted_text_attribute_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_double(cell, text, text_position);
}

/* Unused: erases a run at 2 cells a character, writing cell as a long, so the second cell gets cell's
   high half, which erase_formatted_text does not clear; then the code byte: 6 another run at this width,
   0 ends, the others go to their routine. */
void erase_formatted_text_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = FORMATTED_TEXT(text)[0];
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write32(text_position, cell);
				next_char = FORMATTED_TEXT(text)[0];
				text++;
				zero = next_char == 0u;
				text_position += 4u;
			} while (next_char != 0u);
		}
		code = FORMATTED_TEXT(text)[0];
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			continue;
		case 0x9u:
			erase_formatted_text_triple(cell, text, text_position);
			return;
		case 0xCu:
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_double(cell, text, text_position);
			return;
		default:
			invalid_jump(0x997FDu + code);
		}
	}
}

/* Unused: code 0x21 erased at width 2: text_position moves by a signed byte of cells. */
void erase_formatted_text_skip_double(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_SKIP(text)->cells * 2u;
	text++;
	erase_formatted_text_double(cell, text, text_position);
}

/* Unused: code 0x1B erased: a new position halfword, then erase_formatted_text_attribute_triple. */
void erase_formatted_text_position_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position = (text_position & ~0xFFFFu) | PLACED_TEXT(text)->position;
	text += 2u;
	erase_formatted_text_attribute_triple(cell, text, text_position);
}

/* Unused: codes 0xC (at width 3) and 0x1E erased: skips the attribute byte, then
   erase_formatted_text_triple. */
void erase_formatted_text_attribute_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}

/* Unused: erases a run at 3 cells a character, writing cell as a long and a halfword, so the second cell
   gets cell's high half; then the code byte: 9 another run at this width, 0 ends, the others go to their
   routine. */
void erase_formatted_text_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	uint32_t zero = 0;
	uint32_t code = 0;
	uint8_t first_char = 0, next_char = 0;

	for (;;)
	{
		first_char = FORMATTED_TEXT(text)[0];
		zero = first_char == 0u;
		text++;
		if (first_char != 0u)
		{
			do
			{
				mem_write32(text_position, cell);
				text_position += 4u;
				mem_write16(text_position, cell);
				next_char = FORMATTED_TEXT(text)[0];
				text++;
				zero = next_char == 0u;
				text_position += 2u;
			} while (next_char != 0u);
		}
		code = FORMATTED_TEXT(text)[0];
		text++;
		switch (code)
		{
		case 0x0u:
			if (zero)
				return;
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x3u:
			erase_formatted_text_single(cell, text, text_position);
			return;
		case 0x6u:
			erase_formatted_text_double(cell, text, text_position);
			return;
		case 0x9u:
			continue;
		case 0xCu:
		case 0x1Eu:
			erase_formatted_text_attribute_triple(cell, text, text_position);
			return;
		case 0xFu:
			erase_formatted_text_position_single(cell, text, text_position);
			return;
		case 0x12u:
			erase_formatted_text_attribute_single(cell, text, text_position);
			return;
		case 0x15u:
			erase_formatted_text_position_double(cell, text, text_position);
			return;
		case 0x18u:
			erase_formatted_text_attribute_double(cell, text, text_position);
			return;
		case 0x1Bu:
			erase_formatted_text_position_triple(cell, text, text_position);
			return;
		case 0x21u:
			erase_formatted_text_skip_triple(cell, text, text_position);
			return;
		default:
			invalid_jump(0x99841u + code);
		}
	}
}

/* Unused: code 0x21 erased at width 3: text_position moves by a signed byte of cells. */
void erase_formatted_text_skip_triple(uint32_t cell, uint32_t text, uint32_t text_position)
{
	text_position += TILEMAP_SKIP(text)->cells * 2u;
	text++;
	erase_formatted_text_triple(cell, text, text_position);
}

/* Draws value's low byte as 2 hex digits at text_position (draw_hex_digits), a leading 0 cleared. */
uint32_t draw_hex_2(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint8_t)value;
	digit_count = 2u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
	return value;
}

/* Draws value's low halfword as 4 hex digits at text_position (draw_hex_digits), leading zeros cleared. */
void draw_hex_4(uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t attribute = *p_attribute;
	uint32_t digit_count = 0;

	*p_value = (uint16_t)*p_value;
	digit_count = 4u;
	draw_hex_digits(digit_count, p_value, &attribute, p_text_position);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
}

/* Unused: draws value's low 24 bits as 6 hex digits at text_position (draw_hex_digits). */
void draw_hex_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

/* Draws value as 8 hex digits at text_position (draw_hex_digits), leading zeros cleared. */
void draw_hex_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_hex_digits(digit_count, &value, &attribute, &text_position);
}

/* Writes digit_count hex digits of value ('0'-'9', 'A'-'F') right to left into the cells from
   text_position, with attribute's high byte; once the rest of value is 0 the leading cells are cleared
   to 0 (the last digit is always drawn). Hands back text_position at the first cell. */
void draw_hex_digits(uint32_t digit_count, uint32_t *p_value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t value = *p_value;
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		if ((uint8_t)attribute >= 0xAu)
			attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 7u));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 0x30u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = value >> 4;
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count != 0u)
	{
		do
		{
			text_position -= 2u;
			mem_write16(text_position, value);
			digit_count--;
		} while (digit_count != 0u);
	}
	*p_value = value;
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}

/* Draws value's low byte as two BCD digits at text_position (draw_bcd_digits), a leading 0 cleared: the
   credit count of draw_credit_count_now. */
void draw_credit_digits(uint32_t value, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint32_t attribute = *p_attribute;
	uint32_t digits_left = 0;

	value = (uint8_t)value;
	digits_left = 2u;
	draw_bcd_digits(value, digits_left, &attribute, p_text_position);
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
}

/* Unused: draw_bcd_digits with the 4 digits of value's low halfword. */
void draw_bcd_4(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value = (uint16_t)value;
	digit_count = 4u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

/* Unused: draw_bcd_digits with the 6 digits of value's low 24 bits. */
void draw_bcd_6(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	value &= 0xFFFFFFu;
	digit_count = 6u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

/* Unused: draw_bcd_digits with all 8 digits of value. */
void draw_bcd_8(uint32_t value, uint32_t attribute, uint32_t text_position)
{
	uint32_t digit_count = 0;

	digit_count = 8u;
	draw_bcd_digits(value, digit_count, &attribute, &text_position);
}

/* Writes digit_count BCD digits of value right to left into the cells from text_position, each nibble +
   1 as the character with attribute's high byte; once the rest of value is 0 the leading cells are
   cleared to 0 (the last digit is always drawn). Hands back text_position at the first cell. */
void draw_bcd_digits(uint32_t value, uint32_t digit_count, uint32_t *p_attribute, uint32_t *p_text_position)
{
	uint16_t attribute = (uint16_t)*p_attribute;
	uint32_t text_position = *p_text_position;
	uint8_t shift = 0;
	uint32_t remaining = 0;

	text_position += digit_count * 2u;
	shift = 0xFCu;
	do
	{
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)value);
		attribute = (uint16_t)((attribute & ~0xFFu) | (attribute & 0xFu));
		attribute = (uint16_t)((attribute & ~0xFFu) | (uint8_t)(attribute + 1u));
		text_position -= 2u;
		mem_write16(text_position, attribute);
		remaining = shift_by(value, shift, 32);
		value = remaining;
		digit_count--;
	} while ((digit_count != 0u) & (remaining != 0u));
	if (digit_count != 0u)
	{
		do
		{
			text_position -= 2u;
			mem_write16(text_position, value);
			digit_count--;
		} while (digit_count != 0u);
	}
	*p_attribute = (*p_attribute & 0xFFFFFF00u) | (attribute & 0xFFu);
	*p_text_position = text_position;
}
