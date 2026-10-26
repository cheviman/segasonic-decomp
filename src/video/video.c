/**
 *    Name:        video/video.c
 *    Description: the board's video: the video registers and the screen mode, block copies and fills, palette RAM and its copies at VBLANK, the VRAM scripts that build the tilemaps
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Sets up the video chip: vblank_start_hook = apply_video_registers; from video_control, 0x00-0x0F from
   video_register_defaults, 0x10-0x4F cleared, the zoom steps 0x50-0x5B 0x200, 0x5C-0x5F from the
   defaults, five clip rectangles 0,0-0x13F,0xDF, 0x88-0x8F from the defaults; then set_screen_mode(0)
   and clear_page_0_and_scroll. */
void init_video_registers(void)
{
	uint8_t mode = 0;
	uint16_t value = 0;
	uint32_t count = 0, defaults_cursor = 0, ignored = 0, ignored_2 = 0, rects_left = 0, register_cursor = 0, right_bottom = 0;

	vblank_start_hook = apply_video_registers_ADDRESS;
	defaults_cursor = video_register_defaults_ADDRESS;
	register_cursor = video_control;
	count = 1u;
	copy_blocks(count, &defaults_cursor, &register_cursor);
	count = 4u;
	clear_blocks(&ignored, &count, &register_cursor);
	value = 0x200u;
	count = 6u;
	register_cursor = fill_halfwords(value, count, register_cursor);
	mem_write32(register_cursor, VIDEO_REGISTER_DEFAULTS_TAIL(defaults_cursor)->text_backdrop_control);
	register_cursor += 4u;
	right_bottom = 0xDF013Fu;
	rects_left = 5u;
	defaults_cursor += 4u;
	do
	{
		mem_write32(register_cursor, 0u);
		register_cursor += 4u;
		mem_write32(register_cursor, right_bottom);
		rects_left--;
		register_cursor += 4u;
	} while (rects_left != 0u);
	mem_write32(register_cursor, VIDEO_REGISTER_DEFAULTS_TAIL(defaults_cursor - 0x4u)->bitmap_scroll);
	mem_write32(register_cursor + 4u, VIDEO_REGISTER_DEFAULTS_TAIL(defaults_cursor - 0x4u)->bitmap_control);
	mode = 0u;
	set_screen_mode(mode);
	ignored_2 = clear_page_0_and_scroll();
}

/* Zeroes the first 0x400 bytes of video RAM (0x300000) and the 0x80 bytes of scroll and register shadows
   from nbg0_scroll_x. */
uint32_t clear_page_0_and_scroll(void)
{
	uint32_t count = 0, dest = 0, ignored = 0;

	dest = 0x300000u;
	count = 0x40u;
	clear_blocks(&ignored, &count, &dest);
	dest = nbg0_scroll_x_ADDRESS;
	count = 8u;
	clear_blocks(&ignored, &count, &dest);
	return dest;
}

/* Unused: init_video_registers, then the four tile layers off (video_layer_disable | 0xF) and
   empty_vblank_hook as vblank_start_hook, so the register shadows would never reach the chip. */
void init_video_registers_layers_off(void)
{
	init_video_registers();
	mem_write16(video_layer_disable, mem_read16(video_layer_disable) | 0xFu);
	vblank_start_hook = empty_vblank_hook_ADDRESS;
	empty_vblank_hook();
}

/* Unused: an empty function, the vblank_start_hook init_video_registers_layers_off installs. */
void empty_vblank_hook(void)
{
}

/* The VBLANK hook: writes video_registers_latched to the chip's registers 0x31FF10-0x31FF4F and
   video_layer_opaque_latched to video_layer_opaque_flags, then latches the shadows from nbg0_scroll_x on
   (scroll, centres, pages) for the next VBLANK: the chip gets them a frame later. */
void apply_video_registers(void)
{
	move_halfwords(VIDEO_RAM + 0x1FF10u, video_registers_latched_ADDRESS, 0x20u);
	mem_write8(video_layer_opaque_flags, video_layer_opaque_latched);
	move_halfwords(video_registers_latched_ADDRESS, nbg0_scroll_x_ADDRESS, 0x21u);
}

/* Sets the screen: mode bit 0 wide (416 pixels: video_control bit 15, sprite_wide_screen); bit 1,
   flipped once more by setting_monitor_flip, flips the screen (video_control bit 9, sprite_flip 3);
   screen_mode keeps the result. Hands back video_control's address. */
uint32_t set_screen_mode(uint16_t mode)
{
	uint16_t control_bits = 0;
	uint32_t control_register = 0;

	mode = (uint8_t)(mode ^ (setting_monitor_flip & 2u));
	screen_mode = (uint8_t)mode;
	control_register = video_control;
	control_bits = (uint16_t)(mode & 1u);
	mem_write8(sprite_wide_screen, (uint8_t)control_bits);
	mem_write16(video_control, mem_read16(video_control) & 0x7DFFu);
	mode &= 2u;
	mode = (uint16_t)(((uint32_t)mode << 8) | (mode >> 8));
	control_bits = (uint16_t)(((uint32_t)control_bits << 15) | (control_bits >> 1) | mode);
	mode = (uint16_t)(mode * 3u);
	mem_write8(sprite_flip, ((uint32_t)mode << 7) | (mode >> 9));
	mem_write16(video_control, mem_read16(video_control) | control_bits);
	return control_register;
}

/* Copies count blocks of 16 bytes from source to dest; hands back both past them. */
void copy_blocks(uint32_t count, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;

	do
	{
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		source += 4u;
		mem_write32(dest, mem_read32(source));
		dest += 4u;
		count--;
		source += 4u;
	} while (count != 0u);
	*p_source = source;
	*p_dest = dest;
}

/* Writes count blocks of 16 zero bytes from dest; hands back dest past them. */
void clear_blocks(uint32_t *p_value, uint32_t *p_count, uint32_t *p_dest)
{
	uint32_t dest = *p_dest;
	uint32_t value = 0;

	value = 0u;
	do
	{
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		dest += 4u;
		mem_write32(dest, value);
		(*p_count)--;
		dest += 4u;
	} while (*p_count != 0u);
	*p_value = value;
	*p_dest = dest;
}

/* Writes value into count halfwords from dest; hands back dest past them. */
uint32_t fill_halfwords(uint16_t value, uint32_t count, uint32_t dest)
{
	do
	{
		mem_write16(dest, value);
		count--;
		dest += 2u;
	} while (count != 0u);
	return dest;
}

/* Runs a VRAM script from script, writing tilemap cells at dest: a byte 0x00-0x7F writes that word of
   vram_script_dictionary; 0x80-0xA7 are the commands of the vram_script_ functions (address, skips,
   fills, counts, raw bytes and words, page clears, columns, repeats); 0xAA ends. Hands back script and
   dest after it. */
void run_vram_script(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t dest = *p_dest;
	uint32_t dictionary_cursor = 0, script_code = 0;

	dictionary_cursor = vram_script_dictionary_ADDRESS;
	for (;;)
	{
		script_code = VRAM_SCRIPT(*p_script)[0];
		(*p_script)++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, vram_script_dictionary[script_code]);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, p_script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(p_script, &dest);
			continue;
		case 0x6u:
			vram_script_fill(p_count, p_script, &dest);
			continue;
		case 0x9u:
			vram_script_count_up(p_count, p_script, &dest);
			continue;
		case 0xCu:
			vram_script_count_down(p_count, p_script, &dest);
			continue;
		case 0xFu:
			vram_script_bytes(p_count, p_script, &dest);
			continue;
		case 0x12u:
			vram_script_words(p_count, p_script, &dest);
			continue;
		case 0x15u:
			vram_script_words_to_dictionary(p_count, p_script, &dest, &dictionary_cursor);
			continue;
		case 0x18u:
			vram_script_clear_pages(p_script, &dest);
			continue;
		case 0x1Bu:
			vram_script_fill_down(p_count, p_script, &dest);
			continue;
		case 0x1Eu:
			vram_script_fill_down_wide(p_count, p_script, &dest);
			continue;
		case 0x21u:
			vram_script_fill_up(p_count, p_script, &dest);
			continue;
		case 0x24u:
			vram_script_repeat(dest, p_count, p_script);
			continue;
		case 0x27u:
			vram_script_skip_long(p_script, &dest);
			continue;
		case 0x2Au:
			*p_dest = dest;
			return;
		default:
			invalid_jump(0x9988Eu + script_code);
		}
	}
}

/* Command 0xAA's routine, which ends a script; run_vram_script returns by itself on it, so nothing calls
   this empty function. */
void vram_script_end(void)
{
}

/* Command 0x80: dest = 0x300000 (video RAM) + the next halfword * 2. */
void vram_script_set_address(uint32_t script_code, uint32_t *p_script, uint32_t *p_dest)
{
	*p_dest = 0x300000u + ((script_code & ~0xFFFFu) | VRAM_SCRIPT_ADDRESS(*p_script)->cell) * 2u;
	*p_script += 2u;
}

/* Command 0x83: dest moves by a signed byte of cells. */
void vram_script_skip(uint32_t *p_script, uint32_t *p_dest)
{
	*p_dest += TILEMAP_SKIP(*p_script)->cells * 2u;
	(*p_script)++;
}

/* Command 0xA7: dest moves by a signed halfword of cells. */
void vram_script_skip_long(uint32_t *p_script, uint32_t *p_dest)
{
	*p_dest += TILEMAP_LONG_SKIP(*p_script)->cells * 2u;
	*p_script += 2u;
}

/* Command 0x86: a count byte and a word: the word in count cells. */
void vram_script_fill(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(*p_dest, tile);
		count--;
		*p_dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0x89: a count byte and a word: count cells of word, word + 1, word + 2 and so on. */
void vram_script_count_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(*p_dest, tile);
		tile++;
		count--;
		*p_dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0x8C: a count byte and a word: count cells of word, word - 1, word - 2 and so on. */
void vram_script_count_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(*p_dest, tile);
		tile--;
		count--;
		*p_dest += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0x8F: a count byte and a high byte, then count low bytes, each written as a cell with that
   high byte. */
void vram_script_bytes(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_BYTE_CELLS(script)->count;
	script++;
	tile = (uint16_t)((uint32_t)VRAM_SCRIPT_BYTE_CELLS(script - 0x1u)->high_byte << 8);
	script++;
	do
	{
		tile = (uint16_t)((tile & ~0xFFu) | VRAM_SCRIPT_LOW_BYTES(script)[0]);
		mem_write16(*p_dest, tile);
		*p_dest += 2u;
		count--;
		script++;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0x92: a count byte, then count words, one per cell. */
void vram_script_words(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t count = 0;

	count = VRAM_SCRIPT_WORD_CELLS(script)->count;
	script++;
	do
	{
		mem_write16(*p_dest, VRAM_SCRIPT_CELLS(script)[0]);
		*p_dest += 2u;
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0x95: as 0x92, each word also stored in vram_script_dictionary at dictionary_cursor, which
   wraps within 0x100 bytes. */
void vram_script_words_to_dictionary(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest, uint32_t *p_dictionary_cursor)
{
	uint32_t script = *p_script;
	uint32_t dictionary_cursor = *p_dictionary_cursor;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_WORD_CELLS(script)->count;
	script++;
	do
	{
		tile = VRAM_SCRIPT_CELLS(script)[0];
		mem_write16(*p_dest, tile);
		*p_dest += 2u;
		mem_write16(dictionary_cursor, tile);
		dictionary_cursor = (dictionary_cursor & ~0xFFu) | (uint8_t)(dictionary_cursor + 2u);
		count--;
		script += 2u;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
	*p_dictionary_cursor = (*p_dictionary_cursor & 0xFFFFFF00u) | (dictionary_cursor & 0xFFu);
}

/* Command 0x98: an address halfword (dest = 0x300000 + it * 2) and a page count byte: clears count *
   0x200 halfwords from there (fill_value 0). */
void vram_script_clear_pages(uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t halfword_count = 0;

	*p_dest = 0x300000u + VRAM_SCRIPT_PAGE_CLEAR(script)->cell * 2u;
	script += 2u;
	halfword_count = (uint32_t)VRAM_SCRIPT_PAGE_CLEAR(script - 0x2u)->pages << 9;
	fill_value = 0u;
	script++;
	set_halfwords(*p_dest, halfword_count, fill_value);
	*p_script = script;
}

/* Command 0x9B: a count byte and a word: the word in count cells down a column, 0x40 bytes apart. */
void vram_script_fill_down(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(*p_dest, tile);
		*p_dest += 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0xA1: a count byte and a word: the word in count cells up a column, 0x40 bytes apart. */
void vram_script_fill_up(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(*p_dest, tile);
		*p_dest -= 0x40u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0x9E: a count byte and a word: the word in count cells down a column of a 64-cell map, 0x80
   bytes apart. */
void vram_script_fill_down_wide(uint32_t *p_count, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(*p_dest, tile);
		*p_dest += 0x80u;
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Command 0xA4: a count byte and a word: writes the word count times to the same cell; dest does not
   move. */
void vram_script_repeat(uint32_t dest, uint32_t *p_count, uint32_t *p_script)
{
	uint32_t script = *p_script;
	uint16_t tile = 0;
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script++;
	tile = VRAM_SCRIPT_CELL_RUN(script - 0x1u)->cell;
	script += 2u;
	do
	{
		mem_write16(dest, tile);
		count--;
	} while (count != 0u);
	*p_count = count;
	*p_script = script;
}

/* Unused: walks a VRAM script like run_vram_script but writes 0 (cell) to every cell it would draw,
   skipping the data; the address, skip and page-clear commands work as there, and 0xAA ends. */
void erase_vram_script(uint32_t script, uint32_t dest)
{
	uint16_t cell = 0;
	uint32_t script_code = 0;

	cell = 0u;
	for (;;)
	{
		script_code = VRAM_SCRIPT(script)[0];
		script++;
		if ((int8_t)script_code >= 0)
		{
			mem_write16(dest, cell);
			dest += 2u;
			continue;
		}
		script_code = script_code & 0x7Fu;
		switch (script_code)
		{
		case 0x0u:
			vram_script_set_address(script_code, &script, &dest);
			continue;
		case 0x3u:
			vram_script_skip(&script, &dest);
			continue;
		case 0x6u:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0x9u:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0xCu:
			erase_vram_script_fill(cell, &script, &dest);
			continue;
		case 0xFu:
			erase_vram_script_bytes(cell, &script, &dest);
			continue;
		case 0x12u:
			erase_vram_script_words(cell, &script, &dest);
			continue;
		case 0x15u:
			erase_vram_script_words(cell, &script, &dest);
			continue;
		case 0x18u:
			vram_script_clear_pages(&script, &dest);
			continue;
		case 0x1Bu:
			erase_vram_script_fill_down(cell, &script, &dest);
			continue;
		case 0x1Eu:
			erase_vram_script_fill_down_wide(cell, &script, &dest);
			continue;
		case 0x21u:
			erase_vram_script_fill_up(cell, &script, &dest);
			continue;
		case 0x24u:
			script = erase_vram_script_repeat(cell, script, dest);
			continue;
		case 0x27u:
			vram_script_skip_long(&script, &dest);
			continue;
		case 0x2Au:
			return;
		default:
			invalid_jump(0x999B7u + script_code);
		}
	}
}

/* Unused: command 0xAA's routine, which ends an erase_vram_script; erase_vram_script returns by itself
   on it, so this is an empty function. */
void erase_vram_script_end(void)
{
}

/* Unused: commands 0x86, 0x89 and 0x8C erased: a count byte, the word skipped; cell count times. */
void erase_vram_script_fill(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(*p_script)->count;
	*p_script += 3u;
	do
	{
		mem_write16(*p_dest, cell);
		count--;
		*p_dest += 2u;
	} while (count != 0u);
}

/* Unused: command 0x8F erased: a count byte, then the high byte and count bytes skipped; cell count
   times. */
void erase_vram_script_bytes(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t count = 0;

	count = VRAM_SCRIPT_BYTE_CELLS(script)->count;
	script = script + 2u + count;
	do
	{
		mem_write16(*p_dest, cell);
		count--;
		*p_dest += 2u;
	} while (count != 0u);
	*p_script = script;
}

/* Unused: commands 0x92 and 0x95 erased: a count byte, the count words skipped (0x95's dictionary is
   left alone); cell count times. */
void erase_vram_script_words(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t script = *p_script;
	uint32_t count = 0;

	count = VRAM_SCRIPT_WORD_CELLS(script)->count;
	script = script + 1u + count * 2u;
	do
	{
		mem_write16(*p_dest, cell);
		count--;
		*p_dest += 2u;
	} while (count != 0u);
	*p_script = script;
}

/* Unused: command 0x9B erased: a count byte, the word skipped; cell count times down a column, 0x40
   bytes apart. */
void erase_vram_script_fill_down(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(*p_script)->count;
	*p_script += 3u;
	do
	{
		mem_write16(*p_dest, cell);
		*p_dest += 0x40u;
		count--;
	} while (count != 0u);
}

/* Unused: command 0xA1 erased: cell count times up a column, 0x40 bytes apart. */
void erase_vram_script_fill_up(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(*p_script)->count;
	*p_script += 3u;
	do
	{
		mem_write16(*p_dest, cell);
		*p_dest -= 0x40u;
		count--;
	} while (count != 0u);
}

/* Unused: command 0x9E erased: cell count times down a column of a 64-cell map, 0x80 bytes apart. */
void erase_vram_script_fill_down_wide(uint16_t cell, uint32_t *p_script, uint32_t *p_dest)
{
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(*p_script)->count;
	*p_script += 3u;
	do
	{
		mem_write16(*p_dest, cell);
		*p_dest += 0x80u;
		count--;
	} while (count != 0u);
}

/* Unused: command 0xA4 erased: cell count times to the same cell; hands back script past the command. */
uint32_t erase_vram_script_repeat(uint16_t cell, uint32_t script, uint32_t dest)
{
	uint32_t count = 0;

	count = VRAM_SCRIPT_CELL_RUN(script)->count;
	script += 3u;
	do
	{
		mem_write16(dest, cell);
		count--;
	} while (count != 0u);
	return script;
}

/* Empties the VBLANK copies (vblank_copies_pending and vblank_copy_count 0) and loads
   default_palette_list into palette RAM (load_palette_list); hands back where it stopped writing. */
uint32_t load_default_palettes(void)
{
	uint32_t dest = 0, ignored = 0, palette_list = 0;

	vblank_copies_pending = 0u;
	vblank_copy_count = 0u;
	palette_list = default_palette_list_ADDRESS;
	load_palette_list(palette_list, &ignored, &dest);
	return dest;
}

/* Loads palette RAM (0x600000) from a list: a count, then entries of a source, a palette RAM offset and
   a copy mode, each source running to the next entry's: 0 copy_palettes_with_pen_0, 2
   copy_palette_block, 4 repeat_palette_block (6 would be copy_palettes_with_pen_0 with pen 0 = 6; no
   list has it). */
void load_palette_list(uint32_t palette_list, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t copy_mode = 0, dest = 0, entries_left = 0, size = 0;

	entries_left = PALETTE_LOAD_LIST(palette_list)->entry_count;
	dest = 0x600000u;
	palette_list += 2u;
	do
	{
		*p_source = PALETTE_LIST_ENTRY(palette_list)->source;
		palette_list += 4u;
		dest = (dest & ~0xFFFFu) | PALETTE_LIST_ENTRY(palette_list - 0x4u)->palette_offset;
		palette_list += 2u;
		copy_mode = PALETTE_LIST_ENTRY(palette_list - 0x6u)->copy_mode;
		palette_list += 2u;
		size = PALETTE_LIST_ENTRY(palette_list)->source - *p_source;
		switch (copy_mode)
		{
		case 0x0u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, p_source, &dest);
			break;
		case 0x2u:
			copy_palette_block(size, p_source, &dest);
			break;
		case 0x4u:
			repeat_palette_block(size, p_source, &dest, &palette_list);
			break;
		case 0x6u:
			copy_palettes_with_pen_0((uint16_t)copy_mode, size, p_source, &dest);
			break;
		default:
			invalid_jump(0x9A4DFu + copy_mode);
		}
		entries_left--;
	} while (entries_left != 0u);
	*p_dest = dest;
}

/* Writes 16-color palettes at dest from size bytes of source colors: pen 0 is pen_0_color, pens 1-15 the
   next 15 colors of the source. */
void copy_palettes_with_pen_0(uint16_t pen_0_color, uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t source = *p_source;
	uint32_t dest = *p_dest;
	uint16_t pen_15 = 0;
	uint32_t pens_1_2 = 0, pens_3_4 = 0, pens_5_6 = 0, pens_7_8 = 0, pens_9_10 = 0, pens_11_12 = 0, pens_13_14 = 0, halfwords_left = 0;

	size = size >> 1;
	do
	{
		mem_write16(dest, pen_0_color);
		pens_1_2 = PALETTE_PENS_1_15(source)->pens_1_2;
		source += 4u;
		dest += 2u;
		mem_write32(dest, pens_1_2);
		pens_3_4 = PALETTE_PENS_1_15(source - 0x4u)->pens_3_4;
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_3_4);
		pens_5_6 = PALETTE_PENS_1_15(source - 0x8u)->pens_5_6;
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_5_6);
		pens_7_8 = PALETTE_PENS_1_15(source - 0xCu)->pens_7_8;
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_7_8);
		pens_9_10 = PALETTE_PENS_1_15(source - 0x10u)->pens_9_10;
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_9_10);
		pens_11_12 = PALETTE_PENS_1_15(source - 0x14u)->pens_11_12;
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_11_12);
		pens_13_14 = PALETTE_PENS_1_15(source - 0x18u)->pens_13_14;
		source += 4u;
		dest += 4u;
		mem_write32(dest, pens_13_14);
		pen_15 = PALETTE_PENS_1_15(source - 0x1Cu)->pen_15;
		source += 2u;
		dest += 4u;
		mem_write16(dest, pen_15);
		halfwords_left = size;
		size = halfwords_left - 0xFu;
		dest += 2u;
	} while ((int32_t)halfwords_left > 0xF);
	*p_source = source;
	*p_dest = dest;
}

/* Copies size bytes of colors from source to dest in palette RAM (move_string, by halfwords); hands back
   both past them. */
void copy_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest)
{
	uint32_t dest_end = 0, source_end = 0;

	size = size >> 1;
	move_string(*p_source, size, *p_dest, size, 2u, 0u, 0u, 0u, &dest_end, &source_end);
	*p_source = source_end;
	*p_dest = dest_end;
}

/* Writes the 16-color blocks of the palette the list's next long points at, each as many times as its
   count, the counts being the size / 2 halfwords at source; moves palette_list past that long. */
void repeat_palette_block(uint32_t size, uint32_t *p_source, uint32_t *p_dest, uint32_t *p_palette_list)
{
	uint32_t source = *p_source;
	uint32_t dest_end = 0, repeat_counts = 0, repeats_left = 0, source_end = 0;

	repeat_counts = source;
	source = PALETTE_REPEAT_SOURCE(*p_palette_list)->palettes;
	size = size >> 1;
	*p_palette_list += 4u;
	do
	{
		repeats_left = PALETTE_REPEAT_COUNTS(repeat_counts)[0];
		repeat_counts += 2u;
		do
		{
			move_string(source, 0x10u, *p_dest, 0x10u, 2u, 0u, 0u, 0u, &dest_end, &source_end);
			*p_dest = dest_end;
			repeats_left--;
		} while (repeats_left != 0u);
		source = source_end;
		size--;
	} while (size != 0u);
	*p_source = source;
}

/* Adds a copy to vblank_copies for the next VBLANK: source, dest (its top byte is how many times it is
   copied) and halfwords; vblank_copies_pending is cleared while the entry is written, then set. */
void queue_vblank_copy(uint32_t source, uint32_t dest, uint16_t halfwords, uint32_t *p_copy_offset, uint32_t *p_copy_entry)
{
	uint32_t copy_entry = 0, copy_offset = 0;

	vblank_copies_pending = 0u;
	copy_offset = vblank_copy_count;
	vblank_copy_count++;
	copy_offset = copy_offset * 5u;
	copy_entry = vblank_copies_ADDRESS + copy_offset * 2u;
	VBLANK_COPY(copy_entry)->source = source;
	copy_entry += 4u;
	VBLANK_COPY(copy_entry - 0x4u)->dest = dest;
	copy_entry += 4u;
	VBLANK_COPY(copy_entry - 0x8u)->halfwords = halfwords;
	vblank_copies_pending = 1u;
	copy_entry += 2u;
	*p_copy_offset = copy_offset;
	*p_copy_entry = copy_entry;
}
