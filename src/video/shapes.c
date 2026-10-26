/**
 *    Name:        video/shapes.c
 *    Description: shapes on the bitmap layer, mostly unused: triangles, quadrilaterals, circles, flood fill, saved rectangles, and the clipping the lines and rectangles use
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Unused: fills the triangle top_x, top_y / middle_x, middle_y / bottom_x, bottom_y with color at 8 bpp:
   sorts the points by y, sets current_player to bitmap_address (the base of the span addresses) and the
   polygon_span list at bitmap_draw_buffer, then fill_triangle_sorted. */
void fill_triangle(uint32_t top_x, uint32_t top_y, uint32_t middle_x, uint32_t middle_y, uint32_t bottom_x, uint32_t bottom_y, uint8_t color)
{
	uint32_t span_cursor = 0;
	uint16_t swap_x_12 = 0, swap_y_12 = 0, swap_x_13 = 0, swap_y_13 = 0, swap_x_23 = 0, swap_y_23 = 0;

	if ((int16_t)middle_y < (int16_t)top_y)
	{
		swap_x_12 = top_x;
		top_x = (uint16_t)middle_x;
		middle_x = swap_x_12;
		swap_y_12 = top_y;
		top_y = (uint16_t)middle_y;
		middle_y = swap_y_12;
	}
	if ((int16_t)bottom_y < (int16_t)top_y)
	{
		swap_x_13 = top_x;
		top_x = (uint16_t)bottom_x;
		bottom_x = swap_x_13;
		swap_y_13 = top_y;
		top_y = (uint16_t)bottom_y;
		bottom_y = swap_y_13;
	}
	if ((int16_t)bottom_y < (int16_t)middle_y)
	{
		swap_x_23 = middle_x;
		middle_x = (uint16_t)bottom_x;
		bottom_x = swap_x_23;
		swap_y_23 = middle_y;
		middle_y = (uint16_t)bottom_y;
		bottom_y = swap_y_23;
	}
	span_cursor = bitmap_draw_buffer_ADDRESS;
	current_player = bitmap_address;
	top_x = (int16_t)top_x;
	top_y = (int16_t)top_y;
	middle_x = (int16_t)middle_x;
	middle_y = (int16_t)middle_y;
	bottom_x = (int16_t)bottom_x;
	bottom_y = (int16_t)bottom_y;
	fill_triangle_sorted(top_x, top_y, middle_x, middle_y, bottom_x, bottom_y, color, span_cursor);
}

/* Unused: the points sorted top to bottom: one row (fill_span_8bpp from the leftmost to the rightmost x)
   when all share y, fill_flat_top_triangle when the top two do; else both edges stepped in 16.16, a
   polygon_span (bitmap address, length) a row at span_cursor, turning at the middle point; then
   fill_span_list. */
void fill_triangle_sorted(uint32_t top_x, uint32_t top_y, uint32_t middle_x, uint32_t middle_y, uint32_t bottom_x, uint32_t bottom_y, uint8_t color, uint32_t span_cursor)
{
	uint8_t integer_shift = 0;
	uint32_t bottom_x_fixed = 0, height = 0, ignored = 0, left_x = 0, length = 0, middle_on_right = 0, previous_left_x = 0, right_edge_x = 0, right_x = 0, row_left_x = 0, rows_left = 0, span_address = 0, span_color = 0, span_count = 0;
	uint32_t saved_middle_y = 0, saved_bottom_y = 0, swap_top_x = 0, swap_x_12 = 0, swap_x_13 = 0, swap_slope = 0;

	saved_middle_y = middle_y;
	rows_left = saved_middle_y - top_y;
	if (saved_middle_y == top_y)
	{
		saved_bottom_y = bottom_y;
		height = saved_bottom_y - top_y;
		if (saved_bottom_y != top_y)
		{
			if ((int32_t)top_x <= (int32_t)middle_x)
			{
				fill_flat_top_triangle(top_x, top_y, middle_x, bottom_x, height, color, span_cursor);
				return;
			}
			swap_top_x = top_x;
			left_x = middle_x;
			right_x = swap_top_x;
			fill_flat_top_triangle(left_x, top_y, right_x, bottom_x, height, color, span_cursor);
			return;
		}
		if ((int32_t)middle_x < (int32_t)top_x)
		{
			swap_x_12 = top_x;
			top_x = middle_x;
			middle_x = swap_x_12;
		}
		if ((int32_t)bottom_x < (int32_t)top_x)
		{
			swap_x_13 = top_x;
			top_x = bottom_x;
			bottom_x = swap_x_13;
		}
		if ((int32_t)bottom_x < (int32_t)middle_x)
			bottom_x = middle_x;
		top_y = (top_y << 9) + top_x + current_player;
		length = bottom_x - top_x;
		span_color = (uint8_t)color;
		fill_span_8bpp(length, &ignored, &top_y, &span_color);
		return;
	}
	bottom_y = bottom_y - top_y + 1u;
	span_address = (top_y << 9) + top_x + current_player;
	previous_left_x = top_x;
	top_x = top_x << 16;
	bottom_x = bottom_x << 16;
	bottom_x_fixed = bottom_x;
	integer_shift = 0xF0u;
	rows_left++;
	middle_x = divide_s32((middle_x << 16) - top_x, rows_left);
	bottom_x = divide_s32(bottom_x - top_x, bottom_y);
	middle_on_right = 0u;
	if ((int32_t)bottom_x < (int32_t)middle_x)
	{
		swap_slope = middle_x;
		middle_x = bottom_x;
		bottom_x = swap_slope;
		middle_on_right = 1u;
	}
	right_edge_x = top_x;
	r14 = 0x200u;
	span_count = bottom_y;
	bottom_y -= rows_left;
	for (;;)
	{
		top_x += middle_x;
		right_edge_x += bottom_x;
		row_left_x = shift_signed_by(top_x, integer_shift, 32);
		span_address -= previous_left_x - row_left_x;
		POLYGON_SPAN(span_cursor)->address = span_address;
		span_cursor += 4u;
		POLYGON_SPAN(span_cursor - 0x4u)->length = shift_signed_by(right_edge_x, integer_shift, 32) - row_left_x;
		previous_left_x = row_left_x;
		span_address += r14;
		span_cursor += 2u;
		rows_left--;
		if (rows_left != 0u)
			continue;
		if (bottom_y == 0u)
			break;
		if (middle_on_right == 0u)
		{
			middle_x = divide_s32(bottom_x_fixed - top_x, bottom_y);
			rows_left = bottom_y;
			bottom_y = 0u;
			continue;
		}
		bottom_x = divide_s32(bottom_x_fixed - right_edge_x, bottom_y);
		rows_left = bottom_y;
		bottom_y = 0u;
	}
	fill_span_list(span_count, color);
}

/* Unused: the triangle with its top edge from left_x to right_x on row top_y and its apex at bottom_x,
   height rows lower: both edges stepped in 16.16, one polygon_span a row at span_cursor (addresses from
   current_player, 0x200 a row), then fill_span_list. */
void fill_flat_top_triangle(uint32_t left_x, uint32_t top_y, uint32_t right_x, uint32_t bottom_x, uint32_t height, uint8_t color, uint32_t span_cursor)
{
	uint8_t integer_shift = 0;
	uint32_t left_slope = 0, previous_left_x = 0, row_left_x = 0, span_address = 0, span_count = 0;

	span_address = current_player + ((top_y << 9) + left_x);
	previous_left_x = left_x;
	bottom_x = bottom_x << 16;
	right_x = right_x << 16;
	left_x = left_x << 16;
	height++;
	left_slope = divide_s32(bottom_x - left_x, height);
	bottom_x = divide_s32(bottom_x - right_x, height);
	integer_shift = 0xF0u;
	span_count = height;
	r14 = 0x200u;
	do
	{
		row_left_x = shift_signed_by(left_x, integer_shift, 32);
		POLYGON_SPAN(span_cursor)->address = span_address;
		span_cursor += 4u;
		POLYGON_SPAN(span_cursor - 0x4u)->length = shift_signed_by(right_x, integer_shift, 32) - row_left_x;
		span_address = span_address - (previous_left_x - row_left_x) + r14;
		left_x += left_slope;
		right_x += bottom_x;
		previous_left_x = row_left_x;
		span_cursor += 2u;
		height--;
	} while (height != 0u);
	fill_span_list(span_count, color);
}

/* Unused: fill_span_list with polygon_color, the colour fill_quad keeps. */
void fill_span_list_from_r8(uint32_t span_count, uint8_t polygon_color)
{
	uint8_t color = 0;

	color = (uint8_t)polygon_color;
	fill_span_list(span_count, color);
}

/* Unused: fills span_count polygon_span records from bitmap_draw_buffer (bitmap address and length, a
   negative length skipping the row) with color at 8 bpp: an odd first byte, 16-byte blocks, then 8, 4, 2
   and 1. */
void fill_span_list(uint32_t span_count, uint32_t color)
{
	bool aligned = false;
	uint32_t zero = 0;
	uint8_t last_bytes = 0, tail_bytes = 0;
	uint32_t blocks_left = 0, length = 0, pixel_address = 0, span_record = 0;
	uint16_t block_count = 0;
	uint32_t length_after_odd = 0, odd_bytes = 0;

	span_record = bitmap_draw_buffer_ADDRESS;
	color = (uint8_t)color * 0x1010101u;
	do
	{
		pixel_address = POLYGON_SPAN(span_record)->address;
		span_record += 4u;
		length = (uint16_t)POLYGON_SPAN(span_record - 0x4u)->length;
		span_record += 2u;
		if ((int16_t)length >= 0)
		{
			aligned = (pixel_address & 1u) == 0u;
			if (!aligned)
			{
				mem_write8(pixel_address, color);
				length_after_odd = length - 1u;
				length = length_after_odd;
				pixel_address++;
				aligned = (int32_t)length_after_odd >= 0;
			}
			if (aligned)
			{
				length++;
				tail_bytes = (uint8_t)length;
				block_count = (uint16_t)length >> 4;
				blocks_left = (length & ~0xFFFFu) | block_count;
				if (block_count != 0u)
				{
					do
					{
						mem_write32(pixel_address, color);
						pixel_address += 4u;
						mem_write32(pixel_address, color);
						pixel_address += 4u;
						mem_write32(pixel_address, color);
						pixel_address += 4u;
						mem_write32(pixel_address, color);
						blocks_left--;
						pixel_address += 4u;
					} while (blocks_left != 0u);
				}
				tail_bytes &= 0xFu;
				if (tail_bytes >= 8u)
				{
					mem_write32(pixel_address, color);
					pixel_address += 4u;
					mem_write32(pixel_address, color);
					tail_bytes -= 8u;
					pixel_address += 4u;
				}
				if ((uint8_t)tail_bytes >= 4u)
				{
					mem_write32(pixel_address, color);
					pixel_address += 4u;
				}
				odd_bytes = tail_bytes & 3u;
				last_bytes = (uint8_t)odd_bytes;
				if (odd_bytes != 0u)
				{
					zero = (uint8_t)(last_bytes - 2u) == 0u;
					if (last_bytes < 2u)
						mem_write8(pixel_address, color);
					else
					{
						mem_write16(pixel_address, color);
						pixel_address += 2u;
						if (zero == 0u)
							mem_write8(pixel_address, color);
					}
				}
			}
		}
		span_count--;
	} while (span_count != 0u);
}

/* Unused: fills the convex quadrilateral left_x, top_y / second_x, second_y / third_x, third_y /
   bottom_x, bottom_y (in order round it) with fill_color at 8 bpp: edges walked in 16.16 from the top
   vertex into polygon_span records at bitmap_draw_buffer, then filled; flat rows go to fill_span_8bpp or
   the triangle code. Keeps the vertices on the stack. */
void fill_quad(uint32_t left_x, uint32_t top_y, uint32_t second_x, uint32_t second_y, uint32_t third_x, uint32_t third_y, uint32_t bottom_x, uint32_t bottom_y, uint8_t fill_color)
{
	uint8_t bottom_index = 0, pixel_shift = 0, second_index = 0, third_index = 0, top_index = 0, triangle_color = 0;
	uint16_t right_column = 0;
	uint32_t apex_x = 0, bend_on_right = 0, bottom_right_x = 0, bottom_vertex = 0, color = 0, edge_parts_left = 0, ignored = 0, left_column = 0, left_end = 0, left_pixel = 0, left_slope = 0, length = 0, next_part_rows = 0, previous_left = 0, prior_left = 0, right_side_slope = 0, right_slope = 0, right_x = 0, row_count = 0, rows_after_second = 0, segments_left = 0, single_edge_slope = 0, span_address = 0, span_cursor = 0, triangle_height = 0, triangle_left_x = 0, triangle_right_x = 0, triangle_top_y = 0, vertex_index = 0;
	uint16_t swap_bottom_x = 0;
	uint32_t swap_x_01 = 0, swap_key_01 = 0, swap_x_02 = 0, swap_key_02 = 0, swap_x_03 = 0, swap_key_03 = 0, swap_x_12 = 0, swap_key_12 = 0, swap_x_13 = 0, swap_key_13 = 0, swap_x_23 = 0, swap_key_23 = 0, swap_edge_slope = 0, third_rows = 0, rows_after_left_bend = 0, swap_slope = 0, rows_after_right_bend = 0, swap_top_x = 0, swap_upper_x = 0, swap_lower_x = 0, swap_triangle_x = 0;

	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, bottom_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, bottom_x);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, third_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, third_x);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, second_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, second_x);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, top_y);
	stack_pointer = stack_pointer - 2u;
	mem_write16(stack_pointer, left_x);
	bottom_x = (int16_t)bottom_x;
	third_x = (int16_t)third_x;
	second_x = (int16_t)second_x;
	left_x = (int16_t)left_x;
	second_y = (uint32_t)(int16_t)second_y << 8;
	top_y = (uint32_t)(int16_t)top_y << 8;
	bottom_y = ((uint32_t)(int16_t)bottom_y << 8) | 3u;
	third_y = ((uint32_t)(int16_t)third_y << 8) | 2u;
	second_y |= (uint8_t)(second_y + 1u);
	if ((int32_t)second_y < (int32_t)top_y)
	{
		swap_x_01 = left_x;
		left_x = second_x;
		second_x = swap_x_01;
		swap_key_01 = top_y;
		top_y = second_y;
		second_y = swap_key_01;
	}
	if ((int32_t)third_y < (int32_t)top_y)
	{
		swap_x_02 = left_x;
		left_x = third_x;
		third_x = swap_x_02;
		swap_key_02 = top_y;
		top_y = third_y;
		third_y = swap_key_02;
	}
	if ((int32_t)bottom_y < (int32_t)top_y)
	{
		swap_x_03 = left_x;
		left_x = bottom_x;
		bottom_x = swap_x_03;
		swap_key_03 = top_y;
		top_y = bottom_y;
		bottom_y = swap_key_03;
	}
	if ((int32_t)third_y < (int32_t)second_y)
	{
		swap_x_12 = second_x;
		second_x = third_x;
		third_x = swap_x_12;
		swap_key_12 = second_y;
		second_y = third_y;
		third_y = swap_key_12;
	}
	if ((int32_t)bottom_y < (int32_t)second_y)
	{
		swap_x_13 = second_x;
		second_x = bottom_x;
		bottom_x = swap_x_13;
		swap_key_13 = second_y;
		second_y = bottom_y;
		bottom_y = swap_key_13;
	}
	if ((int32_t)bottom_y < (int32_t)third_y)
	{
		swap_x_23 = third_x;
		third_x = bottom_x;
		bottom_x = swap_x_23;
		swap_key_23 = third_y;
		third_y = bottom_y;
		bottom_y = swap_key_23;
	}
	top_index = (uint8_t)top_y;
	second_index = (uint8_t)second_y;
	third_index = (uint8_t)third_y;
	bottom_index = (uint8_t)bottom_y;
	top_y = (int32_t)top_y >> 8;
	second_y = (int32_t)second_y >> 8;
	third_y = (int32_t)third_y >> 8;
	bottom_y = (int32_t)bottom_y >> 8;
	span_cursor = bitmap_draw_buffer_ADDRESS;
	current_player = bitmap_address;
	r14 = 0x200u;
	if (second_y != top_y)
	{
		if (bottom_y == third_y)
		{
			if ((uint16_t)third_y == (uint16_t)second_y)
			{
				if ((int32_t)third_x < (int32_t)second_x)
					second_x = third_x;
				if ((int32_t)bottom_x < (int32_t)second_x)
					second_x = bottom_x;
				if ((int32_t)bottom_x < (int32_t)third_x)
					bottom_x = third_x;
				bottom_right_x = bottom_x;
				triangle_color = (uint8_t)fill_color;
				stack_pointer = stack_pointer + 0x10u;
				fill_triangle_sorted(left_x, top_y, second_x, second_y, bottom_right_x, third_y, triangle_color, span_cursor);
				return;
			}
			stack_pointer = stack_pointer + 0x10u;
			if ((int16_t)bottom_x < (int16_t)third_x)
			{
				swap_bottom_x = third_x;
				third_x = (uint16_t)bottom_x;
				bottom_x = swap_bottom_x;
				bottom_index = (uint8_t)third_index;
			}
			second_y -= top_y;
			third_y = third_y - top_y + 1u;
			prior_left = left_x;
			top_y = (top_y << 9) + left_x + current_player;
			left_x = left_x << 16;
			right_x = left_x;
			second_x = second_x << 16;
			third_x = third_x << 16;
			bottom_x = bottom_x << 16;
			pixel_shift = 0xF0u;
			segments_left = 2u;
			second_y++;
			if (((uint8_t)(bottom_index - top_index) & 3u) != 2u)
			{
				span_address = top_y;
				row_count = third_y;
				bottom_x = divide_s32(bottom_x - left_x, third_y);
				third_y -= second_y;
				second_x -= left_x;
				do
				{
					second_x = divide_s32(second_x, second_y);
					do
					{
						left_x += second_x;
						right_x += bottom_x;
						left_end = shift_by(left_x, pixel_shift, 32);
						span_address -= prior_left - left_end;
						POLYGON_SPAN(span_cursor)->address = span_address;
						span_cursor += 4u;
						POLYGON_SPAN(span_cursor - 0x4u)->length = shift_by(right_x, pixel_shift, 32) - left_end;
						prior_left = left_end;
						span_address += r14;
						span_cursor += 2u;
						second_y--;
					} while (second_y != 0u);
					second_x = third_x - left_x;
					second_y = third_y;
					segments_left--;
				} while (segments_left != 0u);
				fill_span_list_from_r8(row_count, fill_color);
				return;
			}
			span_address = top_y;
			third_x = divide_s32(third_x - left_x, third_y);
			row_count = third_y;
			third_y -= second_y;
			second_x -= left_x;
			do
			{
				second_x = divide_s32(second_x, second_y);
				do
				{
					left_x += third_x;
					right_x += second_x;
					left_end = shift_by(left_x, pixel_shift, 32);
					span_address -= prior_left - left_end;
					POLYGON_SPAN(span_cursor)->address = span_address;
					span_cursor += 4u;
					POLYGON_SPAN(span_cursor - 0x4u)->length = shift_by(right_x, pixel_shift, 32) - left_end;
					prior_left = left_end;
					span_address += r14;
					span_cursor += 2u;
					second_y--;
				} while (second_y != 0u);
				second_x = bottom_x - right_x;
				second_y = third_y;
				segments_left--;
			} while (segments_left != 0u);
			fill_span_list_from_r8(row_count, fill_color);
			return;
		}
		stack_pointer = stack_pointer + 0x10u;
		if (((uint8_t)(bottom_index - top_index) & 3u) != 2u)
		{
			second_y -= top_y;
			third_y = third_y - top_y + 1u;
			bottom_y = bottom_y - top_y + 1u;
			span_address = (uint32_t)(uint16_t)top_y << 9;
			span_address = ((span_address & ~0xFFFFu) | (uint16_t)(span_address + left_x)) + current_player;
			prior_left = left_x;
			left_x = left_x << 16;
			right_x = left_x;
			third_x = third_x << 16;
			bottom_x = bottom_x << 16;
			pixel_shift = 0xF0u;
			row_count = bottom_y;
			second_y++;
			second_x = divide_s32((second_x << 16) - left_x, second_y);
			right_side_slope = divide_s32(bottom_x - left_x, bottom_y);
			bend_on_right = 0u;
			if ((int32_t)right_side_slope < (int32_t)second_x)
			{
				swap_edge_slope = second_x;
				second_x = right_side_slope;
				right_side_slope = swap_edge_slope;
				bend_on_right++;
			}
			edge_parts_left = 3u;
			bottom_y -= third_y;
			third_rows = third_y;
			next_part_rows = third_rows - second_y;
			if (third_rows == second_y)
			{
				edge_parts_left = 2u;
				next_part_rows = bottom_y;
				third_x = bottom_x;
			}
			for (;;)
			{
				left_x += second_x;
				right_x += right_side_slope;
				current_player = left_x;
				current_player = shift_by(current_player, pixel_shift, 32);
				span_address -= prior_left - current_player;
				POLYGON_SPAN(span_cursor)->address = span_address;
				span_cursor += 4u;
				POLYGON_SPAN(span_cursor - 0x4u)->length = shift_by(right_x, pixel_shift, 32) - current_player;
				prior_left = current_player;
				span_address += r14;
				span_cursor += 2u;
				second_y--;
				if (second_y != 0u)
					continue;
				if (bend_on_right == 0u)
				{
					second_x = divide_s32(third_x - left_x, next_part_rows);
					second_y = next_part_rows;
					third_x = bottom_x;
					next_part_rows = bottom_y;
					edge_parts_left--;
					if (edge_parts_left == 0u)
					{
						fill_span_list_from_r8(row_count, fill_color);
						return;
					}
					continue;
				}
				right_side_slope = divide_s32(third_x - right_x, next_part_rows);
				second_y = next_part_rows;
				third_x = bottom_x;
				next_part_rows = bottom_y;
				edge_parts_left--;
				if (edge_parts_left != 0u)
					continue;
				break;
			}
			fill_span_list_from_r8(row_count, fill_color);
			return;
		}
		second_y -= top_y;
		third_y = third_y - top_y + 1u;
		bottom_y = bottom_y - top_y + 1u;
		span_address = top_y << 9;
		span_address = ((span_address & ~0xFFFFu) | (uint16_t)(span_address + left_x)) + current_player;
		row_count = bottom_y;
		prior_left = left_x;
		left_x = left_x << 16;
		bottom_x = bottom_x << 16;
		right_x = left_x;
		pixel_shift = 0xF0u;
		second_y++;
		second_x = divide_s32((second_x << 16) - left_x, second_y);
		third_x = divide_s32((third_x << 16) - left_x, third_y);
		bottom_y -= third_y;
		rows_after_second = row_count - second_y;
		segments_left = 2u;
		if ((int32_t)second_x <= (int32_t)third_x)
		{
			for (;;)
			{
				left_x += second_x;
				right_x += third_x;
				left_pixel = shift_by(left_x, pixel_shift, 32);
				span_address -= prior_left - left_pixel;
				POLYGON_SPAN(span_cursor)->address = span_address;
				span_cursor += 4u;
				POLYGON_SPAN(span_cursor - 0x4u)->length = shift_by(right_x, pixel_shift, 32) - left_pixel;
				prior_left = left_pixel;
				span_address += r14;
				span_cursor += 2u;
				second_y--;
				if (second_y == 0u)
				{
					segments_left--;
					if (segments_left != 0u)
					{
						second_x = divide_s32(bottom_x - left_x, rows_after_second);
						rows_after_left_bend = rows_after_second;
						second_y = rows_after_left_bend - bottom_y;
						if (rows_after_left_bend == bottom_y)
							break;
						continue;
					}
					break;
				}
			}
			right_slope = divide_s32(bottom_x - right_x, bottom_y);
		}
		else
		{
			swap_slope = second_x;
			left_slope = third_x;
			right_slope = swap_slope;
			for (;;)
			{
				left_x += left_slope;
				right_x += right_slope;
				left_pixel = shift_by(left_x, pixel_shift, 32);
				span_address -= prior_left - left_pixel;
				POLYGON_SPAN(span_cursor)->address = span_address;
				span_cursor += 4u;
				POLYGON_SPAN(span_cursor - 0x4u)->length = shift_by(right_x, pixel_shift, 32) - left_pixel;
				prior_left = left_pixel;
				span_address += r14;
				span_cursor += 2u;
				second_y--;
				if (second_y == 0u)
				{
					segments_left--;
					if (segments_left != 0u)
					{
						right_slope = divide_s32(bottom_x - right_x, rows_after_second);
						rows_after_right_bend = rows_after_second;
						second_y = rows_after_right_bend - bottom_y;
						if (rows_after_right_bend == bottom_y)
							break;
						continue;
					}
					break;
				}
			}
			second_x = divide_s32(bottom_x - left_x, bottom_y);
		}
		do
		{
			left_x += second_x;
			right_x += right_slope;
			left_pixel = shift_by(left_x, pixel_shift, 32);
			span_address -= prior_left - left_pixel;
			POLYGON_SPAN(span_cursor)->address = span_address;
			span_cursor += 4u;
			POLYGON_SPAN(span_cursor - 0x4u)->length = shift_by(right_x, pixel_shift, 32) - left_pixel;
			prior_left = left_pixel;
			span_address += r14;
			span_cursor += 2u;
			bottom_y--;
		} while (bottom_y != 0u);
		fill_span_list_from_r8(row_count, fill_color);
		return;
	}
	if (third_y != second_y)
	{
		if (bottom_y != third_y)
		{
			if ((int32_t)second_x < (int32_t)left_x)
			{
				swap_top_x = left_x;
				left_x = second_x;
				second_x = swap_top_x;
				top_index = (uint8_t)second_index;
			}
			bottom_y -= second_y;
			third_y -= second_y;
			top_y = (top_y << 9) + left_x + current_player;
			previous_left = left_x;
			right_column = (uint16_t)second_x;
			left_x = left_x << 16;
			second_x = second_x << 16;
			third_x = third_x << 16;
			bottom_x = bottom_x << 16;
			pixel_shift = 0xF0u;
			if (((uint8_t)(bottom_index - top_index) & 3u) != 2u)
			{
				span_address = top_y;
				third_x = divide_s32(third_x - second_x, third_y);
				single_edge_slope = divide_s32(bottom_x - left_x, bottom_y);
				row_count = bottom_y;
				segments_left = 2u;
				bottom_y -= third_y;
				for (;;)
				{
					POLYGON_SPAN(span_cursor)->address = span_address;
					span_cursor += 4u;
					POLYGON_SPAN(span_cursor - 0x4u)->length = right_column - previous_left;
					left_x += single_edge_slope;
					second_x += third_x;
					left_column = shift_by(left_x, pixel_shift, 32);
					right_column = (uint16_t)shift_by(second_x, pixel_shift, 32);
					span_address = span_address - (previous_left - left_column) + r14;
					previous_left = left_column;
					span_cursor += 2u;
					third_y--;
					if (third_y != 0u)
						continue;
					third_x = divide_s32(bottom_x - second_x, bottom_y);
					third_y = bottom_y;
					segments_left--;
					if (segments_left == 0u)
						break;
				}
				stack_pointer = stack_pointer + 0x10u;
				fill_span_list_from_r8(row_count, fill_color);
				return;
			}
			span_address = top_y;
			third_x = divide_s32(third_x - left_x, third_y);
			single_edge_slope = divide_s32(bottom_x - second_x, bottom_y);
			third_y++;
			bottom_y++;
			row_count = bottom_y;
			segments_left = 2u;
			bottom_y -= third_y;
			for (;;)
			{
				POLYGON_SPAN(span_cursor)->address = span_address;
				span_cursor += 4u;
				POLYGON_SPAN(span_cursor - 0x4u)->length = right_column - previous_left;
				left_x += third_x;
				second_x += single_edge_slope;
				left_column = shift_by(left_x, pixel_shift, 32);
				right_column = (uint16_t)shift_by(second_x, pixel_shift, 32);
				span_address = span_address - (previous_left - left_column) + r14;
				previous_left = left_column;
				span_cursor += 2u;
				third_y--;
				if (third_y != 0u)
					continue;
				third_x = divide_s32(bottom_x - left_x, bottom_y);
				third_y = bottom_y;
				segments_left--;
				if (segments_left == 0u)
					break;
			}
			stack_pointer = stack_pointer + 0x10u;
			fill_span_list_from_r8(row_count, fill_color);
			return;
		}
		if ((int32_t)second_x < (int32_t)left_x)
		{
			swap_upper_x = left_x;
			left_x = second_x;
			second_x = swap_upper_x;
		}
		if ((int32_t)bottom_x < (int32_t)third_x)
		{
			swap_lower_x = third_x;
			third_x = bottom_x;
			bottom_x = swap_lower_x;
		}
		span_address = (top_y << 9) + left_x + current_player;
		previous_left = left_x;
		right_column = (uint16_t)second_x;
		left_x = left_x << 16;
		second_x = second_x << 16;
		third_y -= second_y;
		third_x = divide_s32((third_x << 16) - left_x, third_y);
		bottom_x = divide_s32((bottom_x << 16) - second_x, third_y);
		pixel_shift = 0xF0u;
		third_y++;
		row_count = third_y;
		stack_pointer = stack_pointer + 0x10u;
		do
		{
			POLYGON_SPAN(span_cursor)->address = span_address;
			span_cursor += 4u;
			POLYGON_SPAN(span_cursor - 0x4u)->length = right_column - previous_left;
			left_x += third_x;
			second_x += bottom_x;
			left_column = shift_by(left_x, pixel_shift, 32);
			right_column = (uint16_t)shift_by(second_x, pixel_shift, 32);
			span_address = span_address - (previous_left - left_column) + r14;
			previous_left = left_column;
			span_cursor += 2u;
			third_y--;
		} while (third_y != 0u);
		fill_span_list_from_r8(row_count, fill_color);
		return;
	}
	if (bottom_y != third_y)
	{
		vertex_index = bottom_index & 3u;
		bottom_vertex = stack_pointer + vertex_index * 4u;
		apex_x = QUAD_VERTEX(bottom_vertex)->x;
		triangle_height = QUAD_VERTEX(bottom_vertex)->y;
		vertex_index = (uint8_t)(vertex_index + 1u);
		vertex_index = vertex_index & 3u;
		triangle_right_x = (int16_t)mem_read16(stack_pointer + vertex_index * 4u);
		vertex_index = (uint8_t)(vertex_index + 2u);
		vertex_index = stack_pointer + (vertex_index & 3u) * 4u;
		triangle_left_x = QUAD_VERTEX(vertex_index)->x;
		triangle_top_y = QUAD_VERTEX(vertex_index)->y;
		triangle_height -= triangle_top_y;
		triangle_color = (uint8_t)fill_color;
		stack_pointer = stack_pointer + 0x10u;
		if ((int32_t)triangle_left_x <= (int32_t)triangle_right_x)
		{
			fill_flat_top_triangle(triangle_left_x, triangle_top_y, triangle_right_x, apex_x, triangle_height, triangle_color, span_cursor);
			return;
		}
		swap_triangle_x = triangle_left_x;
		triangle_left_x = triangle_right_x;
		triangle_right_x = swap_triangle_x;
		fill_flat_top_triangle(triangle_left_x, triangle_top_y, triangle_right_x, apex_x, triangle_height, triangle_color, span_cursor);
		return;
	}
	if ((int32_t)second_x < (int32_t)left_x)
		left_x = second_x;
	if ((int32_t)third_x < (int32_t)left_x)
		left_x = third_x;
	if ((int32_t)bottom_x < (int32_t)left_x)
		left_x = bottom_x;
	if ((int32_t)bottom_x < (int32_t)second_x)
		bottom_x = second_x;
	if ((int32_t)bottom_x < (int32_t)third_x)
		bottom_x = third_x;
	top_y = top_y << 9;
	top_y = ((top_y & ~0xFFFFu) | (uint16_t)(top_y + left_x)) + current_player;
	length = bottom_x - left_x;
	color = (uint8_t)fill_color;
	stack_pointer = stack_pointer + 0x10u;
	fill_span_8bpp(length, &ignored, &top_y, &color);
}

/* Unused: meant to draw a circle of radius |radius| around center_x, center_y in circle_color: 64 points
   (sine_table * radius >> 14 mirrored into eight octants) in bitmap_draw_buffer joined by draw_line. It
   cannot: two mirrored stores are off (mirror_cursor + 0x7E, point_cursor + 0x7F) and the line loop
   steps 2 bytes, joining (x, y) to (y, next x). */
void draw_circle_polygon(uint32_t center_x, uint32_t center_y, uint32_t radius, uint8_t circle_color, uint32_t color)
{
	uint8_t sine_shift = 0;
	uint16_t coordinate = 0, cos_offset = 0, end_x = 0, end_y = 0, sin_offset = 0;
	uint32_t angle = 0, angle_step = 0, ignored = 0, lines_left = 0, mirror_cursor = 0, point_cursor = 0, points = 0, sine_entry = 0, sines = 0, start_x = 0, start_y = 0, steps_left = 0, x_cursor = 0, y_cursor = 0;

	if ((int16_t)radius < 0)
		radius = (uint16_t)(0u - radius);
	radius = (uint16_t)radius;
	color = (color & ~0xFFu) | (uint8_t)circle_color;
	angle = 0x100000u;
	angle_step = 0x200000u;
	sines = sine_table_ADDRESS;
	points = bitmap_draw_buffer_ADDRESS;
	point_cursor = bitmap_draw_buffer_ADDRESS;
	mirror_cursor = 0x20B600u;
	steps_left = 8u;
	sine_shift = 0xF2u;
	do
	{
		sine_entry = angle;
		angle += angle_step;
		sine_entry = sines + (uint8_t)((sine_entry << 15) | (sine_entry >> 17)) * 2u;
		cos_offset = (uint16_t)shift_by(SINE_AND_COSINE(sine_entry)->cosine * radius, sine_shift, 32);
		coordinate = (uint16_t)(center_x + cos_offset);
		mem_write16(point_cursor, coordinate);
		mem_write16(mirror_cursor - 4u, coordinate);
		coordinate = (uint16_t)(center_y + cos_offset);
		point_cursor += 2u;
		mem_write16(point_cursor + 0x40u, coordinate);
		mem_write16(mirror_cursor - 0xC2u, coordinate);
		cos_offset = (uint16_t)(0u - cos_offset);
		coordinate = (uint16_t)(center_x + cos_offset);
		mem_write16(point_cursor + 0x7Eu, coordinate);
		mem_write16(mirror_cursor - 0x84u, coordinate);
		coordinate = (uint16_t)(center_y + cos_offset);
		mem_write16(point_cursor + 0xC0u, coordinate);
		mem_write16(mirror_cursor - 0x42u, coordinate);
		sin_offset = (uint16_t)shift_by(SINE_AND_COSINE(sine_entry)->sine * radius, sine_shift, 32);
		coordinate = (uint16_t)(center_y + sin_offset);
		mem_write16(point_cursor, coordinate);
		mem_write16(mirror_cursor + 0x7Eu, coordinate);
		coordinate = (uint16_t)(center_x + sin_offset);
		point_cursor += 2u;
		mem_write16(point_cursor + 0xBCu, coordinate);
		mem_write16(mirror_cursor - 0xC4u, coordinate);
		sin_offset = (uint16_t)(0u - sin_offset);
		coordinate = (uint16_t)(center_x + sin_offset);
		mem_write16(point_cursor + 0x3Cu, coordinate);
		mem_write16(mirror_cursor - 0x44u, coordinate);
		coordinate = (uint16_t)(center_y + sin_offset);
		mem_write16(point_cursor + 0x7Fu, coordinate);
		mirror_cursor -= 2u;
		mem_write16(mirror_cursor, coordinate);
		steps_left--;
		mirror_cursor -= 2u;
	} while (steps_left != 0u);
	y_cursor = points;
	x_cursor = y_cursor;
	lines_left = 0x3Fu;
	y_cursor += 2u;
	do
	{
		center_x = (center_x & ~0xFFFFu) | mem_read16(x_cursor);
		center_y = (center_y & ~0xFFFFu) | mem_read16(y_cursor);
		x_cursor += 2u;
		end_x = mem_read16(x_cursor);
		y_cursor += 2u;
		end_y = mem_read16(y_cursor);
		draw_line(end_x, end_y, &center_x, &center_y, &color, &ignored);
		lines_left--;
	} while (lines_left != 0u);
	start_x = (center_x & ~0xFFFFu) | (uint16_t)bitmap_draw_buffer;
	start_y = (center_y & ~0xFFFFu) | (uint16_t)(bitmap_draw_buffer >> 16u);
	end_x = mem_read16(x_cursor);
	end_y = mem_read16(y_cursor);
	draw_line(end_x, end_y, &start_x, &start_y, &color, &ignored);
}

/* Unused: a midpoint circle of radius |radius| around x, y in color, eight points a step
   (plot_circle_point_8bpp or _4bpp), none when it is wholly outside bitmap_window. current_player holds
   2 bits a point (plot, or plot if inside), but points 4, 6 and 7 are plotted out of the order they
   assume; at 4 bpp odd pixels get color << 3, not << 4. */
void draw_circle(uint16_t x, uint16_t y, uint16_t radius, uint8_t color)
{
	uint8_t high_color = 0;
	uint16_t circle_bottom = 0, circle_left = 0, circle_right = 0, circle_top = 0, minus_radius = 0, octant_mask = 0, window_bottom = 0, window_right = 0, window_top = 0;
	uint32_t bitmap_base = 0, center_x = 0, center_y = 0, error = 0, error_step = 0, minus_x_offset = 0, minus_y_offset = 0, point_x = 0, point_y = 0, x_offset = 0, y_offset = 0;
	uint32_t error_before_8bpp = 0, error_before_4bpp = 0;

	if ((int16_t)radius < 0)
		radius = (uint16_t)(0u - radius);
	error = (uint16_t)radius;
	x_offset = error;
	y_offset = 0u;
	center_x = (int16_t)x;
	center_y = (int16_t)y;
	bitmap_base = bitmap_address;
	r14 = (uint16_t)bitmap_window_left;
	window_top = (uint16_t)bitmap_window_top;
	window_right = (uint16_t)bitmap_window_right;
	window_bottom = (uint16_t)bitmap_window_bottom;
	octant_mask = 0xFFFFu;
	circle_right = (uint16_t)(center_x + x_offset);
	if ((int16_t)circle_right < (int16_t)r14)
		return;
	minus_radius = (uint16_t)(0u - x_offset);
	circle_left = (uint16_t)(center_x + minus_radius);
	if ((int16_t)window_right < (int16_t)circle_left)
		return;
	if ((int16_t)window_right < (int16_t)circle_right)
		octant_mask &= 0x5FF5u;
	if ((int16_t)circle_left < (int16_t)r14)
		octant_mask &= ~0xAA0u;
	circle_bottom = (uint16_t)(center_y + x_offset);
	if ((int16_t)circle_bottom < (int16_t)window_top)
		return;
	circle_top = (uint16_t)(center_y + minus_radius);
	if ((int16_t)window_bottom < (int16_t)circle_top)
		return;
	if ((int16_t)window_bottom < (int16_t)circle_bottom)
		octant_mask &= 0x55FFu;
	if ((int16_t)circle_top < (int16_t)window_top)
		octant_mask = (uint16_t)((octant_mask & ~0xFFu) | (octant_mask & 0x55u));
	if ((int16_t)center_x < (int16_t)r14)
		octant_mask &= ~0x550u;
	if ((int16_t)window_right < (int16_t)center_x)
		octant_mask &= 0xAFFAu;
	if ((int16_t)center_y < (int16_t)window_top)
		octant_mask = (uint16_t)((octant_mask & ~0xFFu) | (octant_mask & 0xAAu));
	if ((int16_t)window_bottom < (int16_t)center_y)
		octant_mask &= 0xAAFFu;
	if (bitmap_8bpp != 0u)
	{
		while ((int32_t)y_offset <= (int32_t)x_offset)
		{
			current_player = (current_player & ~0xFFFFu) | octant_mask;
			minus_x_offset = 0u - x_offset;
			minus_y_offset = 0u - y_offset;
			point_x = center_x + x_offset;
			point_y = center_y + y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + y_offset;
			point_y = center_y + x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_y_offset;
			point_y = center_y + x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_x_offset;
			point_y = center_y + y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + x_offset;
			point_y = center_y + minus_y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_y_offset;
			point_y = center_y + minus_x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + minus_x_offset;
			point_y = center_y + minus_y_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			point_x = center_x + y_offset;
			point_y = center_y + minus_x_offset;
			plot_circle_point_8bpp(point_x, point_y, color, bitmap_base, window_top, window_right, window_bottom);
			error_step = y_offset;
			y_offset++;
			error_step += y_offset;
			error_before_8bpp = error;
			error = error_before_8bpp - error_step;
			if ((int32_t)error_step <= (int32_t)error_before_8bpp)
				continue;
			x_offset--;
			error += x_offset * 2u;
		}
		return;
	}
	high_color = (uint8_t)color;
	color &= 0xFu;
	high_color = (uint8_t)((uint32_t)high_color << 3);
	while ((int32_t)y_offset <= (int32_t)x_offset)
	{
		current_player = (current_player & ~0xFFFFu) | octant_mask;
		minus_x_offset = 0u - x_offset;
		minus_y_offset = 0u - y_offset;
		point_x = center_x + x_offset;
		point_y = center_y + y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + y_offset;
		point_y = center_y + x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_y_offset;
		point_y = center_y + x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_x_offset;
		point_y = center_y + y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + x_offset;
		point_y = center_y + minus_y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_y_offset;
		point_y = center_y + minus_x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + minus_x_offset;
		point_y = center_y + minus_y_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		point_x = center_x + y_offset;
		point_y = center_y + minus_x_offset;
		plot_circle_point_4bpp(point_x, point_y, high_color, color, bitmap_base, window_top, window_right, window_bottom);
		error_step = y_offset;
		y_offset++;
		error_step += y_offset;
		error_before_4bpp = error;
		error = error_before_4bpp - error_step;
		if ((int32_t)error_step <= (int32_t)error_before_4bpp)
			continue;
		x_offset--;
		error += x_offset * 2u;
	}
}

/* Unused: one of draw_circle's points at 4 bpp: the top two bits of current_player's mask, shifted out:
   the first plots, the second plots only inside the window (r14, window_top, _right, _bottom); the byte
   at (point_y << 8) + (point_x >> 1) + bitmap_base gets high_color in its high nibble at odd x,
   low_color in its low one at even x. */
void plot_circle_point_4bpp(uint32_t point_x, uint32_t point_y, uint8_t high_color, uint8_t low_color, uint32_t bitmap_base, uint16_t window_top, uint16_t window_right, uint16_t window_bottom)
{
	uint32_t negative = 0, carry = 0;
	uint16_t shifted_mask = 0;

	shifted_mask = current_player * 2u;
	carry = (uint16_t)(current_player * 2u) < (uint16_t)current_player;
	negative = shifted_mask >> 15;
	current_player = (current_player & ~0xFFFFu) | shifted_mask;
	if (carry == 0u && (negative == 0u || (int16_t)point_x < (int16_t)r14 || (int16_t)point_y < (int16_t)window_top || (int16_t)window_right < (int16_t)point_x || (int16_t)window_bottom < (int16_t)point_y))
	{
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
		return;
	}
	point_y = (point_y << 8) + ((int32_t)point_x >> 1) + bitmap_base;
	if ((point_x & 1u) != 0u)
	{
		mem_write8(point_y, (mem_read8(point_y) & 0xFu) | high_color);
		current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
		return;
	}
	mem_write8(point_y, (0xF0u & mem_read8(point_y)) | low_color);
	current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
}

/* Unused: one of draw_circle's points at 8 bpp: the same test of current_player's top two bits, then
   color at (point_y << 9) + point_x + bitmap_base. */
void plot_circle_point_8bpp(uint32_t point_x, uint32_t point_y, uint8_t color, uint32_t bitmap_base, uint16_t window_top, uint16_t window_right, uint16_t window_bottom)
{
	uint32_t negative = 0, carry = 0;
	uint16_t shifted_mask = 0;

	shifted_mask = current_player * 2u;
	carry = (uint16_t)(current_player * 2u) < (uint16_t)current_player;
	negative = shifted_mask >> 15;
	current_player = (current_player & ~0xFFFFu) | shifted_mask;
	if (carry || (negative && (int16_t)r14 <= (int16_t)point_x && (int16_t)window_top <= (int16_t)point_y && (int16_t)point_x <= (int16_t)window_right && (int16_t)point_y <= (int16_t)window_bottom))
		mem_write8((point_y << 9) + point_x + bitmap_base, color);
	current_player = (current_player & ~0xFFFFu) | (uint16_t)(current_player * 2u);
}

/* Unused: scanline flood fill from seed_x, seed_y with fill_color up to border_color in bitmap_window
   (none when the seed is outside or either colour): the seed's span, then a ring queue of
   flood_fill_record spans in bitmap_draw_buffer (object_cursor in, last_object out) worked off a row
   above and below each; the queue reaches over string_queue. */
void flood_fill(uint16_t seed_x, uint16_t seed_y, uint8_t fill_color, uint8_t border_color)
{
	uint8_t seed_pixel = 0;
	uint16_t span_y = 0, window_bottom = 0, window_top = 0;
	uint32_t fill_pattern = 0, span_left_x = 0, span_right_address = 0, span_right_x = 0, window_left = 0, window_right = 0;
	uint32_t saved_object_cursor = 0, saved_last_object = 0;

	window_left = (uint16_t)bitmap_window_left;
	if ((int16_t)seed_x < (int16_t)window_left)
		return;
	window_top = (uint16_t)bitmap_window_top;
	if ((int16_t)seed_y < (int16_t)window_top)
		return;
	window_right = (uint16_t)bitmap_window_right;
	if ((int16_t)window_right < (int16_t)seed_x)
		return;
	window_bottom = (uint16_t)bitmap_window_bottom;
	if ((int16_t)window_bottom < (int16_t)seed_y)
		return;
	saved_object_cursor = object_cursor;
	saved_last_object = last_object;
	r14 = (uint32_t)(uint16_t)seed_y << 9;
	r14 = ((r14 & ~0xFFFFu) | (uint16_t)(r14 + seed_x)) + bitmap_address;
	seed_pixel = mem_read8(r14);
	if (seed_pixel == (uint8_t)fill_color || seed_pixel == (uint8_t)border_color)
	{
		last_object = saved_last_object;
		object_cursor = saved_object_cursor;
		return;
	}
	object_cursor = bitmap_draw_buffer_ADDRESS;
	last_object = bitmap_draw_buffer_ADDRESS;
	span_left_x = (uint16_t)seed_x;
	span_y = (uint8_t)seed_y;
	fill_pattern = (uint8_t)fill_color * 0x1010101u;
	flood_fill_span(border_color, window_left, window_right, fill_pattern, &span_left_x, &span_right_address, &span_right_x);
	flood_fill_y = (uint8_t)span_y;
	flood_fill_left_x = span_left_x;
	flood_fill_left_address = r14;
	flood_fill_right_x = span_right_x;
	flood_fill_right_address = span_right_address;
	span_left_x = flood_fill_scan_seed_rows(border_color, window_left, window_top, window_right, window_bottom, span_left_x, span_y, fill_pattern);
	while (last_object != object_cursor)
	{
		flood_fill_direction_and_y = FLOOD_FILL_RECORD(last_object)->direction_and_y;
		last_object = last_object + 2u;
		flood_fill_right_x = FLOOD_FILL_RECORD(last_object - 0x2u)->right_x;
		last_object = last_object + 2u;
		flood_fill_right_address = FLOOD_FILL_RECORD(last_object - 0x4u)->right_address;
		last_object = last_object + 4u;
		flood_fill_left_x = FLOOD_FILL_RECORD(last_object - 0x8u)->left_x;
		last_object = last_object + 2u;
		flood_fill_left_address = FLOOD_FILL_RECORD(last_object - 0xAu)->left_address;
		last_object = last_object + 4u;
		flood_fill_parent_right_x = FLOOD_FILL_RECORD(last_object - 0xEu)->parent_right_x;
		last_object = last_object + 2u;
		flood_fill_parent_right_address = FLOOD_FILL_RECORD(last_object - 0x10u)->parent_right_address;
		last_object = last_object + 4u;
		flood_fill_parent_left_x = FLOOD_FILL_RECORD(last_object - 0x14u)->parent_left_x;
		last_object = last_object + 2u;
		flood_fill_parent_left_address = FLOOD_FILL_RECORD(last_object - 0x16u)->parent_left_address;
		last_object = last_object + 4u;
		if ((uint16_t)last_object >= 0xCCC4u)
			last_object = bitmap_draw_buffer_ADDRESS;
		span_y = flood_fill_y;
		span_left_x = flood_fill_scan_span_rows(border_color, window_left, window_top, window_right, window_bottom, span_left_x, span_y, fill_pattern);
	}
	last_object = saved_last_object;
	object_cursor = saved_object_cursor;
}

/* Unused: fills the run at r14 (x left_x) in the scanned row (flood_fill_span) and queues its
   flood_fill_record at object_cursor: direction and row y + direction, its right and left ends, the
   flood_fill_push_parent_* range; object_cursor wraps from 0xCCC4 to bitmap_draw_buffer. right_x and
   right_address come back at the run's right end. */
void flood_fill_push_span(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint8_t direction, uint32_t left_x, uint8_t y, uint32_t fill_pattern, uint32_t *p_right_address, uint32_t *p_right_x)
{
	flood_fill_span(border_color, window_left, window_right, fill_pattern, &left_x, p_right_address, p_right_x);
	FLOOD_FILL_RECORD(object_cursor)->direction_and_y = (uint16_t)(((uint32_t)y + direction) << 8) | (uint8_t)direction;
	object_cursor = object_cursor + 2u;
	FLOOD_FILL_RECORD(object_cursor - 0x2u)->right_x = *p_right_x;
	object_cursor = object_cursor + 2u;
	FLOOD_FILL_RECORD(object_cursor - 0x4u)->right_address = *p_right_address;
	object_cursor = object_cursor + 4u;
	FLOOD_FILL_RECORD(object_cursor - 0x8u)->left_x = left_x;
	object_cursor = object_cursor + 2u;
	FLOOD_FILL_RECORD(object_cursor - 0xAu)->left_address = r14;
	object_cursor = object_cursor + 4u;
	FLOOD_FILL_RECORD(object_cursor - 0xEu)->parent_right_x = flood_fill_push_parent_right_x;
	object_cursor = object_cursor + 2u;
	FLOOD_FILL_RECORD(object_cursor - 0x10u)->parent_right_address = flood_fill_push_parent_right_address;
	object_cursor = object_cursor + 4u;
	FLOOD_FILL_RECORD(object_cursor - 0x14u)->parent_left_x = flood_fill_push_parent_left_x;
	object_cursor = object_cursor + 2u;
	FLOOD_FILL_RECORD(object_cursor - 0x16u)->parent_left_address = flood_fill_push_parent_left_address;
	object_cursor = object_cursor + 4u;
	if ((uint16_t)object_cursor < 0xCCC4u)
	{
		return;
	}
	object_cursor = bitmap_draw_buffer_ADDRESS;
}

/* Unused: the run through the pixel at r14 (x left_x) at 8 bpp: scans left to window_left and right to
   window_right up to the fill colour (fill_pattern) or border_color, then fills it
   (flood_fill_span_fill_run); left_x and r14 come back at its left end, right_x and right_address at its
   right end. Also fill_span_at_point's 8 bpp path. */
void flood_fill_span(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint32_t fill_pattern, uint32_t *p_left_x, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t left_x = *p_left_x;
	uint8_t pixel_count = 0, pixels = 0;
	uint32_t scan_count = 0, seed_address = 0;
	uint32_t start_x = 0;

	seed_address = r14;
	*p_right_x = left_x;
	start_x = left_x;
	scan_count = start_x - window_left;
	if ((int32_t)start_x <= (int32_t)window_left)
		flood_fill_span_scan_right(border_color, window_right, left_x, fill_pattern, seed_address, p_right_address, p_right_x);
	else
	{
		pixel_count = (uint8_t)(uint16_t)scan_count;
		scan_count = (scan_count & ~0xFFFFu) | ((uint16_t)scan_count >> 1);
		if ((r14 & 1u) == 0u)
			flood_fill_span_left_pair(scan_count, pixel_count, border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, p_right_x);
		else
		{
			pixel_count--;
			scan_count = (scan_count & ~0xFFFFu) | (uint16_t)(scan_count + 1u);
			r14 = r14 - 1u;
			pixels = mem_read8(r14);
			flood_fill_span_left_pixel(pixels, scan_count, pixel_count, border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, p_right_x);
		}
	}
	*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
}

/* Unused: flood_fill_span's left scan: the next two pixels to the left (a halfword before r14), the
   nearer one first: the fill or border colour there ends the run after it (flood_fill_span_set_left_x);
   else flood_fill_span_left_pixel for the farther one. */
void flood_fill_span_left_pair(uint32_t scan_count, uint8_t pixel_count, uint8_t border_color, uint32_t window_right, uint32_t fill_pattern, uint32_t seed_address, uint32_t *p_left_x, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t left_x = *p_left_x;
	uint32_t right_x = *p_right_x;
	uint16_t pixels = 0;

	r14 = r14 - 2u;
	pixels = mem_read16(r14);
	pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
	if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
	{
		r14 = r14 + 2u;
		flood_fill_span_set_left_x(border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
	}
	else
	{
		pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
		flood_fill_span_left_pixel((uint8_t)pixels, scan_count, pixel_count, border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
	}
	*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: flood_fill_span's left scan: the farther pixel of the pair (or an odd first one): the fill
   colour ends the run after it, the border colour or the count's end too (an odd last pixel checked
   first when pixel_count is odd); else the next pair (flood_fill_span_left_pair). */
void flood_fill_span_left_pixel(uint8_t pixel, uint32_t scan_count, uint8_t pixel_count, uint8_t border_color, uint32_t window_right, uint32_t fill_pattern, uint32_t seed_address, uint32_t *p_left_x, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t left_x = *p_left_x;
	uint32_t right_x = *p_right_x;
	uint8_t last_pixel = 0;

	if ((uint8_t)pixel == (uint8_t)fill_pattern)
	{
		r14 = r14 + 1u;
		flood_fill_span_set_left_x(border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
		*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	scan_count--;
	if ((scan_count != 0u) & ((uint8_t)pixel != (uint8_t)border_color))
	{
		flood_fill_span_left_pair(scan_count, pixel_count, border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
		*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	if ((uint8_t)pixel == (uint8_t)border_color)
	{
		r14 = r14 + 1u;
		flood_fill_span_set_left_x(border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
		*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	if ((pixel_count & 1u) == 0u)
	{
		flood_fill_span_set_left_x(border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
		*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	r14 = r14 - 1u;
	last_pixel = mem_read8(r14);
	if (last_pixel == (uint8_t)fill_pattern || last_pixel == (uint8_t)border_color)
	{
		r14 = r14 + 1u;
		flood_fill_span_set_left_x(border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
		*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	flood_fill_span_set_left_x(border_color, window_right, fill_pattern, seed_address, &left_x, p_right_address, &right_x);
	*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: left_x -= seed_address - r14, the run's left end x (r14 on its first pixel), then
   flood_fill_span_scan_right. */
void flood_fill_span_set_left_x(uint8_t border_color, uint32_t window_right, uint32_t fill_pattern, uint32_t seed_address, uint32_t *p_left_x, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t left_x = *p_left_x;
	uint32_t right_x = *p_right_x;

	left_x = (left_x & ~0xFFFFu) | (uint16_t)(left_x - (seed_address - r14));
	flood_fill_span_scan_right(border_color, window_right, left_x, fill_pattern, seed_address, p_right_address, &right_x);
	*p_left_x = (*p_left_x & 0xFFFF0000u) | (left_x & 0xFFFFu);
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: flood_fill_span's right scan from the pixel after the seed (seed_address + 1) toward
   window_right, a pair at a time (flood_fill_span_right_pair, _right_pixel); with right_x already at
   window_right straight to flood_fill_span_fill_run. */
void flood_fill_span_scan_right(uint8_t border_color, uint32_t window_right, uint32_t left_x, uint32_t fill_pattern, uint32_t seed_address, uint32_t *p_right_address, uint32_t *p_right_x)
{
	uint32_t right_x = *p_right_x;
	uint8_t pixel_count = 0, pixels = 0;
	uint32_t right_address = 0, scan_count = 0;
	uint16_t right_limit = 0;

	scan_count = window_right;
	right_limit = scan_count;
	scan_count = (scan_count & ~0xFFFFu) | (uint16_t)(right_limit - right_x);
	if ((int16_t)right_limit <= (int16_t)right_x)
		right_address = flood_fill_span_fill_run(left_x, fill_pattern, right_x);
	else
	{
		right_address = seed_address + 1u;
		pixel_count = (uint8_t)scan_count;
		scan_count = (scan_count & ~0xFFFFu) | ((uint16_t)scan_count >> 1);
		if ((right_address & 1u) == 0u)
			flood_fill_span_right_pair(scan_count, pixel_count, border_color, left_x, fill_pattern, (uint16_t)seed_address, &right_address, &right_x);
		else
		{
			pixel_count--;
			scan_count = (scan_count & ~0xFFFFu) | (uint16_t)(scan_count + 1u);
			pixels = mem_read8(right_address);
			right_address++;
			flood_fill_span_right_pixel(pixels, scan_count, pixel_count, border_color, left_x, fill_pattern, (uint16_t)seed_address, &right_address, &right_x);
		}
	}
	*p_right_address = right_address;
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: flood_fill_span's right scan: the next two pixels (a halfword at right_cursor), the nearer one
   first: the fill or border colour there ends the run before it (flood_fill_span_set_right_x); else
   flood_fill_span_right_pixel for the farther one. */
void flood_fill_span_right_pair(uint32_t scan_count, uint8_t pixel_count, uint8_t border_color, uint32_t left_x, uint32_t fill_pattern, uint16_t seed_address, uint32_t *p_right_cursor, uint32_t *p_right_x)
{
	uint32_t right_cursor = *p_right_cursor;
	uint32_t right_x = *p_right_x;
	uint16_t pixels = 0;

	pixels = mem_read16(right_cursor);
	right_cursor += 2u;
	if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
	{
		right_cursor = right_cursor - 1u - 1u - 1u;
		flood_fill_span_set_right_x(left_x, fill_pattern, seed_address, &right_cursor, &right_x);
	}
	else
	{
		pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
		flood_fill_span_right_pixel((uint8_t)pixels, scan_count, pixel_count, border_color, left_x, fill_pattern, seed_address, &right_cursor, &right_x);
	}
	*p_right_cursor = right_cursor;
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: flood_fill_span's right scan: the farther pixel of the pair (or an odd first one): the fill
   colour ends the run before it, the border colour or the count's end too (an odd last pixel checked
   first when pixel_count is odd); else the next pair (flood_fill_span_right_pair). */
void flood_fill_span_right_pixel(uint8_t pixel, uint32_t scan_count, uint8_t pixel_count, uint8_t border_color, uint32_t left_x, uint32_t fill_pattern, uint16_t seed_address, uint32_t *p_right_cursor, uint32_t *p_right_x)
{
	uint32_t right_x = *p_right_x;
	uint8_t last_pixel = 0;

	if ((uint8_t)pixel == (uint8_t)fill_pattern)
	{
		(*p_right_cursor)--;
		(*p_right_cursor)--;
		flood_fill_span_set_right_x(left_x, fill_pattern, seed_address, p_right_cursor, &right_x);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	scan_count--;
	if ((scan_count != 0u) & ((uint8_t)pixel != (uint8_t)border_color))
	{
		flood_fill_span_right_pair(scan_count, pixel_count, border_color, left_x, fill_pattern, seed_address, p_right_cursor, &right_x);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	if ((uint8_t)pixel == (uint8_t)border_color)
	{
		(*p_right_cursor)--;
		(*p_right_cursor)--;
		flood_fill_span_set_right_x(left_x, fill_pattern, seed_address, p_right_cursor, &right_x);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	if ((pixel_count & 1u) == 0u)
	{
		(*p_right_cursor)--;
		flood_fill_span_set_right_x(left_x, fill_pattern, seed_address, p_right_cursor, &right_x);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	last_pixel = mem_read8(*p_right_cursor);
	if (last_pixel == (uint8_t)fill_pattern || last_pixel == (uint8_t)border_color)
	{
		(*p_right_cursor)--;
		flood_fill_span_set_right_x(left_x, fill_pattern, seed_address, p_right_cursor, &right_x);
		*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
		return;
	}
	flood_fill_span_set_right_x(left_x, fill_pattern, seed_address, p_right_cursor, &right_x);
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: right_x += right_cursor - seed_address, the run's right end x (right_cursor on its last
   pixel), then flood_fill_span_fill_run; right_cursor comes back as the last byte written. */
void flood_fill_span_set_right_x(uint32_t left_x, uint32_t fill_pattern, uint16_t seed_address, uint32_t *p_right_cursor, uint32_t *p_right_x)
{
	uint16_t right_cursor = (uint16_t)*p_right_cursor;
	uint32_t right_x = *p_right_x;
	uint32_t right_address = 0;

	right_x = (right_x & ~0xFFFFu) | (uint16_t)(right_x + ((uint32_t)right_cursor - seed_address));
	right_address = flood_fill_span_fill_run(left_x, fill_pattern, right_x);
	*p_right_cursor = right_address;
	*p_right_x = (*p_right_x & 0xFFFF0000u) | (right_x & 0xFFFFu);
}

/* Unused: fills the run from left_x to right_x with fill_pattern from r14: an odd first byte, 16-byte
   blocks, then 8, 4, 2 and 1; hands back the address of the last byte written. */
uint32_t flood_fill_span_fill_run(uint32_t left_x, uint32_t fill_pattern, uint32_t right_x)
{
	uint32_t zero = 0;
	uint8_t tail_bytes = 0;
	uint32_t blocks_left = 0, bytes_left = 0, fill_count = 0, right_address = 0;
	uint32_t count_after_first = 0, block_count = 0, tail_count = 0;

	right_address = r14;
	fill_count = right_x - left_x;
	if ((right_address & 1u) != 0u)
	{
		mem_write8(right_address, fill_pattern);
		count_after_first = fill_count - 1u;
		fill_count = count_after_first;
		right_address++;
		if ((int32_t)count_after_first < 0)
		{
			right_address--;
			return right_address;
		}
	}
	fill_count++;
	bytes_left = fill_count;
	block_count = fill_count >> 4;
	blocks_left = block_count;
	if (block_count != 0u)
	{
		do
		{
			mem_write32(right_address, fill_pattern);
			right_address += 4u;
			mem_write32(right_address, fill_pattern);
			right_address += 4u;
			mem_write32(right_address, fill_pattern);
			right_address += 4u;
			mem_write32(right_address, fill_pattern);
			blocks_left--;
			right_address += 4u;
		} while (blocks_left != 0u);
	}
	bytes_left &= 0xFu;
	if (bytes_left >= 8u)
	{
		mem_write32(right_address, fill_pattern);
		right_address += 4u;
		mem_write32(right_address, fill_pattern);
		bytes_left -= 8u;
		right_address += 4u;
	}
	if (bytes_left >= 4u)
	{
		mem_write32(right_address, fill_pattern);
		right_address += 4u;
	}
	tail_count = bytes_left & 3u;
	tail_bytes = (uint8_t)tail_count;
	if (tail_count == 0u)
	{
		right_address--;
		return right_address;
	}
	zero = (uint8_t)(tail_bytes - 2u) == 0u;
	if (tail_bytes >= 2u)
	{
		mem_write16(right_address, fill_pattern);
		right_address += 2u;
		if (zero == 0u)
		{
			mem_write8(right_address, fill_pattern);
			right_address++;
		}
		right_address--;
		return right_address;
	}
	mem_write8(right_address, fill_pattern);
	right_address++;
	right_address--;
	return right_address;
}

/* Unused: the seed span's neighbour rows: flood_fill_scan_row above (direction -1) when y is below
   window_top, and below (1) when y is above window_bottom; hands back the x the last scan reached. */
uint32_t flood_fill_scan_seed_rows(uint8_t border_color, uint32_t window_left, uint16_t window_top, uint32_t window_right, uint16_t window_bottom, uint32_t scan_x, uint16_t y, uint32_t fill_pattern)
{
	uint32_t direction = 0;

	if ((int16_t)window_top < (int16_t)y)
	{
		direction = 0xFFFFFFFFu;
		scan_x = flood_fill_scan_row(border_color, window_left, window_right, direction, (uint8_t)y, fill_pattern);
	}
	if ((int16_t)window_bottom <= (int16_t)y)
		return scan_x;
	direction = 1u;
	return flood_fill_scan_row(border_color, window_left, window_right, direction, (uint8_t)y, fill_pattern);
}

/* Unused: scans row y + direction under or over the current span, from flood_fill_left_x to
   flood_fill_right_x (copied into the flood_fill_push_parent_* range): every run not in the fill or
   border colour goes through flood_fill_push_span, the scan going on 2 past its right end; hands back
   the x the scan reached. */
uint32_t flood_fill_scan_row(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint32_t direction, uint8_t y, uint32_t fill_pattern)
{
	uint8_t pixel = 0, pixel_count = 0;
	uint16_t pixels = 0;
	uint32_t right_address = 0, right_x = 0, scan_count = 0, scan_start = 0, scan_x = 0;
	uint16_t count_after_first = 0;

	scan_x = flood_fill_left_x;
	r14 = flood_fill_left_address + (direction << 9);
	flood_fill_push_parent_right_x = flood_fill_right_x;
	scan_count = flood_fill_push_parent_right_x;
	flood_fill_push_parent_right_address = flood_fill_right_address;
	flood_fill_push_parent_left_x = flood_fill_left_x;
	flood_fill_push_parent_left_address = flood_fill_left_address;
	for (;;)
	{
		scan_start = r14;
		scan_count = (uint16_t)(scan_count - scan_x);
		if ((r14 & 1u) != 0u)
		{
			pixel = mem_read8(r14);
			if (pixel != (uint8_t)fill_pattern && pixel != (uint8_t)border_color)
			{
				flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
				scan_count = flood_fill_push_parent_right_x;
				scan_count = (uint16_t)(scan_count - 1u);
				if ((int16_t)scan_count <= (int16_t)right_x)
					break;
				scan_x = right_x + 2u;
				r14 = right_address + 2u;
				flood_fill_push_parent_left_x = scan_x;
				flood_fill_push_parent_left_address = r14;
				scan_count = flood_fill_push_parent_right_x;
				continue;
			}
			count_after_first = scan_count - 1u;
			scan_count = count_after_first;
			if ((int16_t)count_after_first < 0)
				break;
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 1u);
		}
		if (scan_count == 0u)
		{
			pixel = mem_read8(r14);
			if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
				break;
			scan_x = scan_x + r14 - scan_start;
			flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
			scan_count = flood_fill_push_parent_right_x;
			scan_count = (uint16_t)(scan_count - 1u);
			if ((int16_t)scan_count <= (int16_t)right_x)
				break;
			scan_x = right_x + 2u;
			r14 = right_address + 2u;
			flood_fill_push_parent_left_x = scan_x;
			flood_fill_push_parent_left_address = r14;
			scan_count = flood_fill_push_parent_right_x;
			continue;
		}
		scan_count = (uint16_t)(scan_count + 1u);
		pixel_count = (uint8_t)scan_count;
		scan_count = scan_count >> 1;
		for (;;)
		{
			pixels = mem_read16(r14);
			r14 = r14 + 2u;
			if ((uint8_t)pixels != (uint8_t)fill_pattern && (uint8_t)pixels != (uint8_t)border_color)
			{
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				scan_x = scan_x + r14 - scan_start;
				break;
			}
			pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
			if ((uint8_t)pixels != (uint8_t)fill_pattern && (uint8_t)pixels != (uint8_t)border_color)
			{
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				scan_x = scan_x + r14 - scan_start;
				break;
			}
			scan_count--;
			if (scan_count != 0u)
				continue;
			if ((pixel_count & 1u) == 0u)
				return scan_x;
			pixel = mem_read8(r14);
			if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
				return scan_x;
			scan_x = scan_x + r14 - scan_start;
			break;
		}
		flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
		scan_count = flood_fill_push_parent_right_x;
		scan_count = (uint16_t)(scan_count - 1u);
		if ((int16_t)scan_count <= (int16_t)right_x)
			break;
		scan_x = right_x + 2u;
		r14 = right_address + 2u;
		flood_fill_push_parent_left_x = scan_x;
		flood_fill_push_parent_left_address = r14;
		scan_count = flood_fill_push_parent_right_x;
	}
	return scan_x;
}

/* Unused: the same as flood_fill_scan_seed_rows for a span taken from the queue, through
   flood_fill_scan_row_skip_parent. */
uint32_t flood_fill_scan_span_rows(uint8_t border_color, uint32_t window_left, uint16_t window_top, uint32_t window_right, uint16_t window_bottom, uint32_t scan_x, uint16_t y, uint32_t fill_pattern)
{
	uint32_t direction = 0;

	if ((int16_t)window_top < (int16_t)y)
	{
		direction = 0xFFFFFFFFu;
		scan_x = flood_fill_scan_row_skip_parent(border_color, window_left, window_right, direction, scan_x, (uint8_t)y, fill_pattern);
	}
	if ((int16_t)window_bottom <= (int16_t)y)
		return scan_x;
	direction = 1u;
	return flood_fill_scan_row_skip_parent(border_color, window_left, window_right, direction, scan_x, (uint8_t)y, fill_pattern);
}

/* Unused: a queued span's row in direction: toward its own flood_fill_direction the whole row
   (flood_fill_scan_row); back toward its parent only the parts left of flood_fill_parent_left_x and
   right of flood_fill_parent_right_x + 1, each run there through flood_fill_push_span. */
uint32_t flood_fill_scan_row_skip_parent(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint32_t direction, uint32_t scan_x, uint8_t y, uint32_t fill_pattern)
{
	uint8_t pixel = 0, pixel_count = 0;
	uint16_t pixels = 0;
	uint32_t right_address = 0, right_x = 0, scan_count = 0, scan_start = 0;
	uint16_t left_count_before = 0, left_pairs = 0;

	if ((uint8_t)direction == flood_fill_direction)
		return flood_fill_scan_row(border_color, window_left, window_right, direction, y, fill_pattern);
	flood_fill_push_parent_right_x = flood_fill_right_x;
	flood_fill_push_parent_right_address = flood_fill_right_address;
	flood_fill_push_parent_left_x = flood_fill_left_x;
	flood_fill_push_parent_left_address = flood_fill_left_address;
	scan_x = (scan_x & ~0xFFFFu) | flood_fill_left_x;
	if (flood_fill_parent_left_x <= (uint16_t)scan_x)
		return flood_fill_skip_parent_right(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern);
	r14 = flood_fill_left_address + (direction << 9);
	for (;;)
	{
		scan_count = flood_fill_parent_left_x;
		scan_start = r14;
		scan_count = (uint16_t)(scan_count - scan_x);
		if ((r14 & 1u) != 0u)
		{
			pixel = mem_read8(r14);
			if (pixel != (uint8_t)fill_pattern && pixel != (uint8_t)border_color)
			{
				flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
				if ((int16_t)(flood_fill_parent_left_x - 2u) <= (int16_t)right_x)
					break;
				scan_x = right_x + 2u;
				r14 = right_address + 2u;
				continue;
			}
			left_count_before = scan_count;
			scan_count = (uint16_t)(left_count_before - 1u);
			if ((int16_t)left_count_before <= 1)
				break;
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 1u);
		}
		pixel_count = (uint8_t)scan_count;
		left_pairs = scan_count >> 1;
		scan_count = left_pairs;
		if (left_pairs != 0u)
		{
			for (;;)
			{
				pixels = mem_read16(r14);
				r14 = r14 + 2u;
				if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
				{
					pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
					if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
					{
						scan_count--;
						if (scan_count != 0u)
							continue;
						if ((pixel_count & 1u) == 0u)
							return flood_fill_skip_parent_right(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern);
						pixel = mem_read8(r14);
						if (pixel == (uint8_t)fill_pattern)
							return flood_fill_skip_parent_right(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern);
						if (pixel == (uint8_t)border_color)
							return flood_fill_skip_parent_right(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern);
						scan_x = scan_x + r14 - scan_start;
						break;
					}
					r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
					scan_x = scan_x + r14 - scan_start;
					break;
				}
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				scan_x = scan_x + r14 - scan_start;
				break;
			}
			flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
			if ((int16_t)(flood_fill_parent_left_x - 2u) <= (int16_t)right_x)
				break;
			scan_x = right_x + 2u;
			r14 = right_address + 2u;
			continue;
		}
		if ((pixel_count & 1u) == 0u)
			break;
		pixel = mem_read8(r14);
		if (pixel == (uint8_t)fill_pattern)
			break;
		if (pixel == (uint8_t)border_color)
			return flood_fill_skip_parent_right(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern);
		scan_x = scan_x + r14 - scan_start;
		flood_fill_push_span(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern, &right_address, &right_x);
		if ((int16_t)(flood_fill_parent_left_x - 2u) <= (int16_t)right_x)
			break;
		scan_x = right_x + 2u;
		r14 = right_address + 2u;
	}
	return flood_fill_skip_parent_right(border_color, window_left, window_right, (uint8_t)direction, scan_x, y, fill_pattern);
}

/* Unused: flood_fill_scan_row_skip_parent's right part: from flood_fill_parent_right_x + 1 to
   flood_fill_right_x, two pixels at a time, each run of fillable pixels pushed through
   flood_fill_push_span; hands back scan_x. */
uint32_t flood_fill_skip_parent_right(uint8_t border_color, uint32_t window_left, uint32_t window_right, uint8_t direction, uint32_t scan_x, uint8_t y, uint32_t fill_pattern)
{
	uint8_t pixel = 0, pixel_count = 0;
	uint16_t pixels = 0;
	uint32_t right_address = 0, right_x = 0, scan_count = 0, scan_start = 0;
	uint16_t span_right_x = 0, right_limit = 0, right_count_before = 0, right_pairs = 0;

	scan_x = (scan_x & ~0xFFFFu) | flood_fill_parent_right_x;
	scan_count = flood_fill_right_x;
	scan_x = (scan_x & ~0xFFFFu) | (uint16_t)(scan_x + 1u);
	span_right_x = scan_count;
	scan_count = (uint16_t)(span_right_x - scan_x);
	if ((int16_t)span_right_x <= (int16_t)scan_x)
		return scan_x;
	r14 = flood_fill_parent_right_address;
	scan_x = (scan_x & ~0xFFFFu) | (uint16_t)(scan_x + 1u);
	r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 2u);
	for (;;)
	{
		scan_start = r14;
		if ((r14 & 1u) != 0u)
		{
			pixel = mem_read8(r14);
			if (pixel != (uint8_t)fill_pattern && pixel != (uint8_t)border_color)
			{
				flood_fill_push_span(border_color, window_left, window_right, direction, scan_x, y, fill_pattern, &right_address, &right_x);
				scan_count = flood_fill_right_x;
				scan_count = (uint16_t)(scan_count - 1u);
				right_limit = scan_count;
				scan_count = (uint16_t)(right_limit - right_x);
				if ((int16_t)right_limit <= (int16_t)right_x)
					break;
				scan_x = right_x + 2u;
				r14 = right_address + 2u;
				continue;
			}
			right_count_before = scan_count;
			scan_count = (uint16_t)(right_count_before - 1u);
			if ((int16_t)right_count_before <= 1)
				break;
			r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 + 1u);
		}
		pixel_count = (uint8_t)scan_count;
		right_pairs = scan_count >> 1;
		scan_count = right_pairs;
		if (right_pairs != 0u)
		{
			for (;;)
			{
				pixels = mem_read16(r14);
				r14 = r14 + 2u;
				if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
				{
					pixels = (uint16_t)(((uint32_t)pixels << 8) | (pixels >> 8));
					if ((uint8_t)pixels == (uint8_t)fill_pattern || (uint8_t)pixels == (uint8_t)border_color)
					{
						scan_count--;
						if (scan_count != 0u)
							continue;
						if ((pixel_count & 1u) == 0u)
							return scan_x;
						pixel = mem_read8(r14);
						if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
							return scan_x;
						scan_x = scan_x + r14 - scan_start;
						break;
					}
					r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
					scan_x = scan_x + r14 - scan_start;
					break;
				}
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				r14 = (r14 & ~0xFFFFu) | (uint16_t)(r14 - 1u);
				scan_x = scan_x + r14 - scan_start;
				break;
			}
			flood_fill_push_span(border_color, window_left, window_right, direction, scan_x, y, fill_pattern, &right_address, &right_x);
			scan_count = flood_fill_right_x;
			scan_count = (uint16_t)(scan_count - 1u);
			right_limit = scan_count;
			scan_count = (uint16_t)(right_limit - right_x);
			if ((int16_t)right_limit <= (int16_t)right_x)
				break;
			scan_x = right_x + 2u;
			r14 = right_address + 2u;
			continue;
		}
		if ((pixel_count & 1u) == 0u)
			break;
		pixel = mem_read8(r14);
		if (pixel == (uint8_t)fill_pattern || pixel == (uint8_t)border_color)
			break;
		scan_x = scan_x + r14 - scan_start;
		flood_fill_push_span(border_color, window_left, window_right, direction, scan_x, y, fill_pattern, &right_address, &right_x);
		scan_count = flood_fill_right_x;
		scan_count = (uint16_t)(scan_count - 1u);
		right_limit = scan_count;
		scan_count = (uint16_t)(right_limit - right_x);
		if ((int16_t)right_limit <= (int16_t)right_x)
			break;
		scan_x = right_x + 2u;
		r14 = right_address + 2u;
	}
	return scan_x;
}

/* Unused: fills the one run through point_x, point_y up to border_color (nothing outside bitmap_window
   or on fill_color or border_color). At 8 bpp flood_fill_span does it; the 4 bpp nibble scans start from
   outcode (0) instead of point_x and read through right_cursor, which nothing sets: that path cannot
   work as written. */
void fill_span_at_point(uint32_t point_x, uint32_t point_y, uint8_t fill_color, uint8_t border_color, uint32_t right_cursor)
{
	uint8_t pixel_byte = 0, point_pixel = 0;
	uint32_t fill_pattern = 0, ignored = 0, ignored_2 = 0, left_x = 0, outcode = 0, right_x = 0, scan_count = 0, window_left = 0, window_right = 0;
	uint32_t start_x = 0;

	get_point_outcode(&point_x, &point_y, &outcode);
	if ((uint8_t)outcode != 0u)
		return;
	if (bitmap_8bpp != 0u)
	{
		r14 = (point_y << 9) + point_x + bitmap_address;
		point_pixel = mem_read8(r14);
		if (point_pixel == (uint8_t)fill_color || point_pixel == (uint8_t)border_color)
			return;
		left_x = point_x;
		fill_pattern = (uint8_t)fill_color * 0x1010101u;
		window_left = (uint16_t)bitmap_window_left;
		window_right = (uint16_t)bitmap_window_right;
		flood_fill_span(border_color, window_left, window_right, fill_pattern, &left_x, &ignored, &ignored_2);
		return;
	}
	r14 = (point_y << 8) + (point_x >> 1) + bitmap_address;
	point_pixel = mem_read8(r14);
	if ((point_x & 1u) != 0u)
		point_pixel = (uint8_t)(((uint32_t)point_pixel << 4) | (point_pixel >> 4));
	point_pixel &= 0xFu;
	if (point_pixel == (uint8_t)fill_color || point_pixel == (uint8_t)border_color)
		return;
	window_left = bitmap_window_left;
	right_x = outcode;
	start_x = outcode;
	scan_count = start_x - window_left;
	if ((int32_t)start_x <= (int32_t)window_left)
	{
		fill_span_at_point_scan_right(fill_color, border_color, outcode, right_cursor, right_x);
		return;
	}
	if ((outcode & 1u) == 0u)
	{
		fill_span_at_point_left_byte(scan_count, fill_color, border_color, outcode, right_cursor, right_x);
		return;
	}
	outcode--;
	pixel_byte = mem_read8(r14);
	fill_span_at_point_left_even_pixel(pixel_byte, scan_count, fill_color, border_color, outcode, right_cursor, right_x);
}

/* Unused: fill_span_at_point's 4 bpp left scan: the byte to the left (two pixels), its high nibble, the
   nearer odd pixel, first: fill_color or border_color there ends the run after it, as does the count's
   end; else its low nibble (fill_span_at_point_left_even_pixel). */
void fill_span_at_point_left_byte(uint32_t scan_count, uint8_t fill_color, uint8_t border_color, uint32_t left_x, uint32_t right_cursor, uint32_t right_x)
{
	uint8_t nibble = 0, pixel_byte = 0;
	uint32_t left_count_before = 0;

	left_x -= 2u;
	r14 = r14 - 1u;
	pixel_byte = mem_read8(r14);
	nibble = pixel_byte >> 4;
	if (nibble == (uint8_t)fill_color || nibble == (uint8_t)border_color)
	{
		r14 = r14 + 1u;
		left_x++;
		left_x++;
		fill_span_at_point_scan_right(fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	left_count_before = scan_count;
	scan_count = left_count_before - 1u;
	if (left_count_before == 1u)
	{
		left_x++;
		fill_span_at_point_scan_right(fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	fill_span_at_point_left_even_pixel(pixel_byte, scan_count, fill_color, border_color, left_x, right_cursor, right_x);
}

/* Unused: fill_span_at_point's 4 bpp left scan: the low nibble, the farther even pixel: fill_color or
   border_color ends the run after it; else the next byte to the left (fill_span_at_point_left_byte)
   while the count lasts; then fill_span_at_point_scan_right. */
void fill_span_at_point_left_even_pixel(uint8_t pixel_byte, uint32_t scan_count, uint8_t fill_color, uint8_t border_color, uint32_t left_x, uint32_t right_cursor, uint32_t right_x)
{
	pixel_byte &= 0xFu;
	if (pixel_byte == (uint8_t)fill_color)
	{
		left_x++;
		fill_span_at_point_scan_right(fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	scan_count--;
	if ((scan_count != 0u) & (pixel_byte != (uint8_t)border_color))
	{
		fill_span_at_point_left_byte(scan_count, fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	if (pixel_byte == (uint8_t)border_color)
	{
		left_x++;
		fill_span_at_point_scan_right(fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	fill_span_at_point_scan_right(fill_color, border_color, left_x, right_cursor, right_x);
}

/* Unused: fill_span_at_point's 4 bpp right scan from x right_x + 1 to bitmap_window_right, a byte (two
   nibbles) at a time through right_cursor (which nothing sets); with right_x already at the window's
   right edge straight to fill_span_at_point_fill_run. */
void fill_span_at_point_scan_right(uint8_t fill_color, uint8_t border_color, uint32_t left_x, uint32_t right_cursor, uint32_t right_x)
{
	uint8_t pixel_byte = 0;
	uint32_t scan_count = 0;
	uint32_t right_limit = 0;

	right_limit = bitmap_window_right;
	scan_count = right_limit - right_x;
	if ((int32_t)right_limit <= (int32_t)right_x)
	{
		fill_span_at_point_fill_run(fill_color, left_x, right_x);
		return;
	}
	right_x++;
	if ((right_x & 1u) == 0u)
	{
		right_cursor++;
		fill_span_at_point_right_byte(scan_count, fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	pixel_byte = mem_read8(right_cursor);
	right_x++;
	right_cursor++;
	fill_span_at_point_right_odd_pixel(pixel_byte, scan_count, fill_color, border_color, left_x, right_cursor, right_x);
}

/* Unused: fill_span_at_point's 4 bpp right scan: the next byte to the right, its low nibble, the nearer
   even pixel, first: fill_color or border_color there ends the run before it, as does the count's end;
   else its high nibble (fill_span_at_point_right_odd_pixel). */
void fill_span_at_point_right_byte(uint32_t scan_count, uint8_t fill_color, uint8_t border_color, uint32_t left_x, uint32_t right_cursor, uint32_t right_x)
{
	uint8_t nibble = 0, pixel_byte = 0;
	uint32_t right_count_before = 0;

	right_x += 2u;
	pixel_byte = mem_read8(right_cursor);
	nibble = pixel_byte & 0xFu;
	right_cursor++;
	if (nibble == (uint8_t)fill_color || nibble == (uint8_t)border_color)
	{
		right_x--;
		right_x--;
		fill_span_at_point_fill_run(fill_color, left_x, right_x);
		return;
	}
	right_count_before = scan_count;
	scan_count = right_count_before - 1u;
	if (right_count_before == 1u)
	{
		right_x--;
		fill_span_at_point_fill_run(fill_color, left_x, right_x);
		return;
	}
	fill_span_at_point_right_odd_pixel(pixel_byte, scan_count, fill_color, border_color, left_x, right_cursor, right_x);
}

/* Unused: fill_span_at_point's 4 bpp right scan: the high nibble, the farther odd pixel: fill_color or
   border_color ends the run before it; else the next byte (fill_span_at_point_right_byte) while the
   count lasts; then fill_span_at_point_fill_run. */
void fill_span_at_point_right_odd_pixel(uint8_t pixel_byte, uint32_t scan_count, uint8_t fill_color, uint8_t border_color, uint32_t left_x, uint32_t right_cursor, uint32_t right_x)
{
	pixel_byte = (uint8_t)pixel_byte >> 4;
	if (pixel_byte == (uint8_t)fill_color)
	{
		right_x--;
		fill_span_at_point_fill_run(fill_color, left_x, right_x);
		return;
	}
	scan_count--;
	if ((scan_count != 0u) & (pixel_byte != (uint8_t)border_color))
	{
		fill_span_at_point_right_byte(scan_count, fill_color, border_color, left_x, right_cursor, right_x);
		return;
	}
	if (pixel_byte == (uint8_t)border_color)
	{
		right_x--;
		fill_span_at_point_fill_run(fill_color, left_x, right_x);
		return;
	}
	fill_span_at_point_fill_run(fill_color, left_x, right_x);
}

/* Unused: fill_span_at_point's 4 bpp fill of x left_x to right_x from r14 with fill_color's low nibble:
   an odd first pixel (high nibble), an aligning byte, longs, a halfword, a byte, an even last pixel (low
   nibble). With one pixel left after the halfwords (or none after the aligning byte) it still writes the
   byte: 2 pixels too many. */
void fill_span_at_point_fill_run(uint32_t fill_color, uint32_t left_x, uint32_t right_x)
{
	uint8_t pixel_byte = 0;
	uint32_t fill_count = 0, fill_cursor = 0, words_left = 0;
	uint32_t count_after_first = 0, bytes_after_words = 0, bytes_after_half = 0;

	fill_count = right_x - left_x;
	fill_cursor = r14;
	if ((left_x & 1u) != 0u)
	{
		pixel_byte = mem_read8(fill_cursor) & 0xFu;
		pixel_byte = (uint8_t)(((uint32_t)pixel_byte << 4) | (pixel_byte >> 4) | fill_color);
		mem_write8(fill_cursor, ((uint32_t)pixel_byte << 4) | (pixel_byte >> 4));
		count_after_first = fill_count - 1u;
		fill_count = count_after_first;
		fill_cursor++;
		if ((int32_t)count_after_first < 0)
			return;
	}
	if ((fill_cursor & 1u) != 0u)
	{
		if (fill_count == 0u)
		{
			mem_write8(fill_cursor, (0xF0u & mem_read8(fill_cursor)) | (fill_color & 0xFu));
			return;
		}
		fill_color = (uint8_t)(fill_color * 0x11u);
		mem_write8(fill_cursor, fill_color);
		fill_cursor++;
		fill_count -= 2u;
	}
	fill_color = (fill_color & 0xFu) * 0x11111111u;
	fill_count++;
	if (fill_count >= 8u)
	{
		words_left = fill_count >> 3;
		do
		{
			mem_write32(fill_cursor, fill_color);
			fill_cursor += 4u;
			words_left--;
		} while (words_left != 0u);
		bytes_after_words = fill_count & 7u;
		fill_count = bytes_after_words;
		if (bytes_after_words == 0u)
			return;
	}
	if (fill_count >= 4u)
	{
		mem_write16(fill_cursor, fill_color);
		bytes_after_half = fill_count & 3u;
		fill_count = bytes_after_half;
		fill_cursor += 2u;
		if (bytes_after_half == 0u)
			return;
	}
	mem_write8(fill_cursor, fill_color);
	fill_cursor++;
	if ((fill_count & 1u) == 0u)
		return;
	mem_write8(fill_cursor, (0xF0u & mem_read8(fill_cursor)) | (fill_color & 0xFu));
}

/* Unused: meant to save the rectangle left, top to right, bottom of the bitmap (corners sorted) into
   buffer: a saved_bitmap_rect header (width in bits, * 8 at 8 bpp, * 4 kept to a byte at 4 bpp, and
   rows), then each row. The row copy is an instruction the V60 cannot run, so the C stops at
   invalid_operand on the first row. */
void save_bitmap_rect(uint16_t left, uint16_t top, uint32_t right, uint32_t bottom, uint32_t buffer)
{
	uint16_t swap_x = 0, swap_y = 0;

	if ((int16_t)right < (int16_t)left)
	{
		swap_x = left;
		left = (uint16_t)right;
		right = swap_x;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		swap_y = top;
		top = (uint16_t)bottom;
		bottom = swap_y;
	}
	right = (uint32_t)(int16_t)right - (int16_t)left + 1u;
	bottom = (uint32_t)(int16_t)bottom - (int16_t)top + 1u;
	if (bitmap_8bpp != 0u)
		right = right << 3;
	else
	{
		right = (right & ~0xFFu) | (uint8_t)(right * 2u);
		right = (right & ~0xFFu) | (uint8_t)(right * 2u);
	}
	SAVED_BITMAP_RECT(buffer)->row_bits = right;
	SAVED_BITMAP_RECT(buffer)->rows = bottom;
	do
	{
		invalid_operand(0x09C312u);
		bottom--;
	} while (bottom != 0u);
}

/* Unused: meant to copy a save_bitmap_rect buffer back onto the bitmap row by row, from its
   saved_bitmap_rect header (row_bits, rows); the row copy is an instruction the V60 cannot run, so the C
   stops at invalid_operand on the first row. */
void draw_saved_bitmap_rect(uint32_t buffer)
{
	uint32_t rows_left = 0;

	rows_left = SAVED_BITMAP_RECT(buffer)->rows;
	do
	{
		invalid_operand(0x09C372u);
		rows_left--;
	} while (rows_left != 0u);
}

/* Swaps the line's two points (start_x/_y with end_x/_y) and hands back in outcode where the new start
   is outside bitmap_window (get_point_outcode: 1 left, 2 right, 4 above, 8 below). */
void get_window_outcode(uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_end_x, uint32_t *p_end_y, uint32_t *p_outcode)
{
	uint16_t end_x = (uint16_t)*p_end_x;
	uint16_t end_y = (uint16_t)*p_end_y;
	uint32_t old_start_x = 0, old_start_y = 0;
	uint32_t old_end_x = 0, old_end_y = 0;

	old_end_x = end_x;
	old_start_x = *p_start_x;
	*p_start_x = old_end_x;
	old_end_y = end_y;
	old_start_y = *p_start_y;
	*p_start_y = old_end_y;
	get_point_outcode(p_start_x, p_start_y, p_outcode);
	*p_end_x = old_start_x;
	*p_end_y = old_start_y;
}

/* Where point_x, point_y is outside bitmap_window, in outcode: 1 left, 2 right, 4 above, 8 below (0
   inside); both come back sign-extended. */
void get_point_outcode(uint32_t *p_point_x, uint32_t *p_point_y, uint32_t *p_outcode)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t outcode = 0;

	outcode = 0u;
	if ((int16_t)point_x < bitmap_window_left)
		outcode = (uint8_t)(outcode + 1u);
	if (bitmap_window_right < (int16_t)point_x)
		outcode = (uint8_t)(outcode + 2u);
	if ((int16_t)point_y < bitmap_window_top)
		outcode = (uint8_t)(outcode + 4u);
	if (bitmap_window_bottom < (int16_t)point_y)
		outcode = (uint8_t)(outcode + 8u);
	point_x = (int16_t)point_x;
	point_y = (int16_t)point_y;
	*p_point_x = point_x;
	*p_point_y = point_y;
	*p_outcode = outcode;
}

/* Clips a line to bitmap_window: moves its start onto the edge outcode names (top or bottom when a
   corner code also has left or right), then swaps the ends and does the same with end_outcode, so the
   line comes back with its ends swapped; unclipped_y gets the last moved end's y from before the move. */
void clip_line_to_window(uint32_t outcode, uint32_t end_outcode, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_end_x, uint32_t *p_end_y, uint32_t *p_unclipped_y)
{
	uint32_t old_start_x = 0, old_start_y = 0;

	switch (outcode)
	{
	case 0:
		clip_line_end_inside();
		break;
	case 1:
		clip_line_to_left_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 2:
		clip_line_to_right_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 3:
		clip_line_end_inside();
		break;
	case 4:
		clip_line_to_top_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 5:
		clip_line_to_top_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 6:
		clip_line_to_top_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 7:
		clip_line_end_inside();
		break;
	case 8:
		clip_line_to_bottom_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 9:
		clip_line_to_bottom_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 10:
		clip_line_to_bottom_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		break;
	case 11:
		clip_line_end_inside();
		break;
	default:
		invalid_jump(0x9C3D2u + outcode * 3u);
	}
	old_start_x = *p_start_x;
	*p_start_x = *p_end_x;
	*p_end_x = old_start_x;
	old_start_y = *p_start_y;
	*p_start_y = *p_end_y;
	*p_end_y = old_start_y;
	switch (end_outcode)
	{
	case 0:
		clip_line_end_inside();
		return;
	case 1:
		clip_line_to_left_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 2:
		clip_line_to_right_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 3:
		clip_line_end_inside();
		return;
	case 4:
		clip_line_to_top_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 5:
		clip_line_to_top_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 6:
		clip_line_to_top_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 7:
		clip_line_end_inside();
		return;
	case 8:
		clip_line_to_bottom_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 9:
		clip_line_to_bottom_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 10:
		clip_line_to_bottom_edge(*p_end_x, *p_end_y, p_start_x, p_start_y, p_unclipped_y);
		return;
	case 11:
		clip_line_end_inside();
		return;
	default:
		invalid_jump(0x9C3D2u + end_outcode * 3u);
	}
}

/* Nothing to move: the end is inside bitmap_window (outcode 0), or its outcode has both left and right
   (3, 7, 11). */
void clip_line_end_inside(void)
{
}

/* Moves the line's start onto bitmap_window_left (clip_line_at_x by bitmap_window_left - start_x). */
void clip_line_to_left_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t dx = 0;

	dx = bitmap_window_left - *p_start_x;
	clip_line_at_x(end_x, end_y, dx, p_start_x, p_start_y, p_unclipped_y);
}

/* Moves the line's start onto bitmap_window_right (clip_line_at_x by bitmap_window_right - start_x). */
void clip_line_to_right_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t dx = 0;

	dx = bitmap_window_right - *p_start_x;
	clip_line_at_x(end_x, end_y, dx, p_start_x, p_start_y, p_unclipped_y);
}

/* Moves the line's start dx along x: start_x += dx and start_y += (end_y - start_y) * dx / (end_x -
   start_x), start_y kept when the line is vertical; unclipped_y gets start_y before the move. */
void clip_line_at_x(uint32_t end_x, uint32_t end_y, uint32_t dx, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t dy_scaled = 0, line_dx = 0;

	line_dx = end_x - *p_start_x;
	*p_unclipped_y = *p_start_y;
	dy_scaled = (end_y - *p_unclipped_y) * dx;
	*p_start_x += dx;
	if (line_dx != 0u)
	{
		*p_start_y += divide_s32(dy_scaled, line_dx);
		return;
	}
}

/* Moves the line's start onto bitmap_window_top (clip_line_at_y by bitmap_window_top - start_y). */
void clip_line_to_top_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t dy = 0;

	dy = bitmap_window_top - *p_start_y;
	clip_line_at_y(end_x, end_y, dy, p_start_x, p_start_y, p_unclipped_y);
}

/* Moves the line's start onto bitmap_window_bottom (clip_line_at_y by bitmap_window_bottom - start_y). */
void clip_line_to_bottom_edge(uint32_t end_x, uint32_t end_y, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t dy = 0;

	dy = bitmap_window_bottom - *p_start_y;
	clip_line_at_y(end_x, end_y, dy, p_start_x, p_start_y, p_unclipped_y);
}

/* Moves the line's start dy along y: start_y += dy and start_x += (end_x - start_x) * dy / (end_y -
   start_y), start_x kept when the line is level; unclipped_y gets start_y before the move. */
void clip_line_at_y(uint32_t end_x, uint32_t end_y, uint32_t dy, uint32_t *p_start_x, uint32_t *p_start_y, uint32_t *p_unclipped_y)
{
	uint32_t dx_scaled = 0, line_dy = 0;

	*p_unclipped_y = *p_start_y;
	line_dy = end_y - *p_unclipped_y;
	dx_scaled = (end_x - *p_start_x) * dy;
	*p_start_y += dy;
	if (line_dy != 0u)
	{
		*p_start_x += divide_s32(dx_scaled, line_dy);
		return;
	}
}

/* Sorts the rectangle left, top, right, bottom and clips it to bitmap_window: unclipped_edges comes back
   0 when it lies wholly outside, else 0xF less a bit for each edge moved (1 left, 2 right, 4 top, 8
   bottom). The outside tests take left and top unsigned, so a rectangle starting at a negative x or y
   counts as outside. */
void clip_rect_to_window(uint32_t *p_left, uint32_t *p_top, uint32_t *p_right, uint32_t *p_bottom, uint32_t *p_window_min, uint32_t *p_unclipped_edges)
{
	uint16_t left = (uint16_t)*p_left;
	uint16_t top = (uint16_t)*p_top;
	uint16_t right = (uint16_t)*p_right;
	uint16_t bottom = (uint16_t)*p_bottom;
	uint16_t window_max = 0, window_min = 0;
	uint32_t unclipped_edges = 0;
	uint16_t old_left = 0, old_top = 0;

	if ((int16_t)right < (int16_t)left)
	{
		old_left = left;
		left = (uint16_t)right;
		right = old_left;
	}
	if ((int16_t)bottom < (int16_t)top)
	{
		old_top = top;
		top = (uint16_t)bottom;
		bottom = old_top;
	}
	unclipped_edges = 0xFu;
	window_min = (uint16_t)bitmap_window_left;
	window_max = (uint16_t)bitmap_window_right;
	if (window_max < (uint16_t)left || (int16_t)right < (int16_t)window_min)
	{
		unclipped_edges = 0u;
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_window_min = (*p_window_min & 0xFFFF0000u) | window_min;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	if ((int16_t)left < (int16_t)window_min)
	{
		left = (uint16_t)window_min;
		unclipped_edges = 0xEu;
	}
	if ((int16_t)window_max < (int16_t)right)
	{
		right = (uint16_t)window_max;
		unclipped_edges = unclipped_edges & 0xDu;
	}
	window_min = (uint16_t)bitmap_window_top;
	window_max = (uint16_t)bitmap_window_bottom;
	if (window_max < (uint16_t)top || (int16_t)bottom < (int16_t)window_min)
	{
		unclipped_edges = 0u;
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_window_min = (*p_window_min & 0xFFFF0000u) | window_min;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	if ((int16_t)top < (int16_t)window_min)
	{
		top = (uint16_t)window_min;
		unclipped_edges = unclipped_edges & 0xBu;
	}
	if ((int16_t)bottom <= (int16_t)window_max)
	{
		*p_left = (*p_left & 0xFFFF0000u) | left;
		*p_top = (*p_top & 0xFFFF0000u) | top;
		*p_right = (*p_right & 0xFFFF0000u) | right;
		*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
		*p_window_min = (*p_window_min & 0xFFFF0000u) | window_min;
		*p_unclipped_edges = unclipped_edges;
		return;
	}
	bottom = (uint16_t)window_max;
	unclipped_edges = unclipped_edges & 7u;
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_top = (*p_top & 0xFFFF0000u) | top;
	*p_right = (*p_right & 0xFFFF0000u) | right;
	*p_bottom = (*p_bottom & 0xFFFF0000u) | bottom;
	*p_window_min = (*p_window_min & 0xFFFF0000u) | window_min;
	*p_unclipped_edges = unclipped_edges;
}
