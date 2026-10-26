/**
 *    Name:        engine/geometry.c
 *    Description: between the map and the screen: projecting objects, the floor under a point, unprojecting a screen point; square roots, angles and BCD
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* sides = where the current object's x, y lies around the box of other: 8 before it in x, 4 past it, 2
   before it in y, 1 past it (0 inside). hitbox_offset_y gets other's. */
void side_of_box(uint32_t other, uint32_t *p_hitbox_offset_y, uint32_t *p_sides)
{
	uint16_t box_start = 0, hitbox_offset_y = 0, sides = 0;

	sides = 0u;
	box_start = (uint16_t)(OBJECT(other)->x_int + OBJECT(other)->hitbox_offset_x);
	if ((int16_t)box_start < OBJECT(current_object)->x_int)
	{
		if ((int16_t)(box_start + OBJECT(other)->hitbox_size_x) <= OBJECT(current_object)->x_int)
			sides |= 4u;
	}
	else
		sides |= 8u;
	hitbox_offset_y = (uint16_t)OBJECT(other)->hitbox_offset_y;
	box_start = (uint16_t)(OBJECT(other)->y_int + hitbox_offset_y);
	if ((int16_t)box_start < OBJECT(current_object)->y_int)
	{
		if ((int16_t)(box_start + OBJECT(other)->hitbox_size_y) <= OBJECT(current_object)->y_int)
			sides |= 1u;
	}
	else
		sides |= 2u;
	*p_hitbox_offset_y = (*p_hitbox_offset_y & 0xFFFF0000u) | hitbox_offset_y;
	*p_sides = (*p_sides & 0xFFFF0000u) | sides;
}

/* speed_x = sine_table[direction] * speed_int >> 6 and speed_y = -(cosine_table[direction] * speed_int
   >> 6): speed (its whole part) along direction. Hands back the cosine product. */
uint32_t set_speed_from_direction(void)
{
	uint32_t cos_speed = 0, direction = 0, speed_int = 0;

	direction = (int16_t)OBJECT(current_object)->direction;
	speed_int = (uint16_t)OBJECT(current_object)->speed_int;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * speed_int) >> 6;
	cos_speed = (int32_t)(cosine_table[direction] * speed_int) >> 6;
	OBJECT(current_object)->speed_y = 0u - cos_speed;
	return cos_speed;
}

/* angle becomes dx = sine_table[angle] * length >> 6 and length becomes dy = -(cosine_table[angle] *
   length >> 6); cos_part keeps the cosine product. */
void polar_to_xy(uint32_t *p_cos_part, uint32_t *p_angle, uint32_t *p_length)
{
	uint16_t angle = (uint16_t)*p_angle;
	uint16_t length = (uint16_t)*p_length;
	uint32_t angle_index = 0, dx = 0, dy = 0, magnitude = 0;

	angle_index = (int16_t)angle;
	magnitude = (uint16_t)length;
	dx = (int32_t)(sine_table[angle_index] * magnitude) >> 6;
	*p_cos_part = (int32_t)(cosine_table[angle_index] * magnitude) >> 6;
	dy = 0u - *p_cos_part;
	*p_angle = dx;
	*p_length = dy;
}

/* set_speed_from_direction with current_player's ground_speed_int as the speed. */
uint32_t set_speed_from_player_speed(void)
{
	uint32_t cos_speed = 0, direction = 0, ground_speed_int = 0;

	direction = (int16_t)OBJECT(current_object)->direction;
	ground_speed_int = PLAYER(current_player)->ground_speed_int;
	OBJECT(current_object)->speed_x = (int32_t)(sine_table[direction] * ground_speed_int) >> 6;
	cos_speed = (int32_t)(cosine_table[direction] * ground_speed_int) >> 6;
	OBJECT(current_object)->speed_y = 0u - cos_speed;
	return cos_speed;
}

/* The current object's isometric screen point: projected_x = ((x + y) >> 8) * 0xB5 >> 16, projected_y =
   ((y - x) >> 9) * 0xB5 (16.16; 0xB5 / 0x100 is cos 45 degrees), then update_screen_position. */
void project_object(uint32_t *p_screen_y, uint32_t *p_depth)
{
	uint32_t projected_x = 0, projected_y = 0;

	projected_y = OBJECT(current_object)->y;
	projected_x = ((int32_t)(OBJECT(current_object)->x + projected_y) >> 8) * 0xB5u;
	projected_y = ((int32_t)(projected_y - OBJECT(current_object)->x) >> 9) * 0xB5u;
	OBJECT(current_object)->projected_x = (int32_t)projected_x >> 16;
	OBJECT(current_object)->projected_y = projected_y;
	update_screen_position(p_screen_y, &projected_y);
	*p_depth = projected_y;
}

/* screen_x = projected_x - view_x + sprite_offset_x and screen_y = (projected_y + z) >> 16 - view_y +
   sprite_offset_y; screen_y comes back too. */
void update_screen_position(uint32_t *p_screen_y, uint32_t *p_depth)
{
	uint16_t sprite_offset_y = 0;
	uint32_t screen_y = 0;

	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x + OBJECT(current_object)->sprite_offset_x;
	screen_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16;
	screen_y = (screen_y & ~0xFFFFu) | (uint16_t)(screen_y - view_y);
	sprite_offset_y = (uint16_t)OBJECT(current_object)->sprite_offset_y;
	screen_y = (screen_y & ~0xFFFFu) | (uint16_t)(screen_y + sprite_offset_y);
	OBJECT(current_object)->screen_y = screen_y;
	*p_screen_y = screen_y;
	*p_depth = (*p_depth & 0xFFFF0000u) | sprite_offset_y;
}

/* The screen position of the map point point_x, point_y at height z: projected as in project_object,
   then point_to_screen; point_x and point_y become its screen x and y, projected_y the screen y too. */
void project_point(uint32_t z, uint32_t *p_projected_y, uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t projected_x_fixed = 0, screen_x = 0, screen_y = 0, world_y = 0;

	world_y = point_y;
	projected_x_fixed = ((int32_t)(point_x + world_y) >> 8) * 0xB5u;
	world_y = ((int32_t)(world_y - point_x) >> 9) * 0xB5u;
	projected_x_fixed = (int32_t)projected_x_fixed >> 16;
	point_to_screen((uint16_t)projected_x_fixed, z, &world_y, &screen_x, &screen_y);
	*p_projected_y = world_y;
	*p_point_x = (*p_point_x & 0xFFFF0000u) | (screen_x & 0xFFFFu);
	*p_point_y = (*p_point_y & 0xFFFF0000u) | (screen_y & 0xFFFFu);
}

/* The screen position of a projected point: screen_x = projected_x - view_x, screen_y = (projected_y +
   z) >> 16 - view_y, which projected_y also gets. */
void point_to_screen(uint16_t projected_x, uint32_t z, uint32_t *p_projected_y, uint32_t *p_screen_x, uint32_t *p_screen_y)
{
	uint32_t projected_y = *p_projected_y;
	uint16_t screen_x = 0, screen_y = 0;

	screen_x = (uint16_t)(projected_x - view_x);
	projected_y = (int32_t)(projected_y + z) >> 16;
	projected_y = (projected_y & ~0xFFFFu) | (uint16_t)(projected_y - view_y);
	screen_y = (uint16_t)projected_y;
	*p_projected_y = projected_y;
	*p_screen_x = (*p_screen_x & 0xFFFF0000u) | screen_x;
	*p_screen_y = (*p_screen_y & 0xFFFF0000u) | screen_y;
}

/* project_object with update_screen_position written in, except that once scroll_position reaches 0xA5F0
   screen_y takes current_player's own_view_y in place of view_y. */
uint32_t project_object_2(void)
{
	uint32_t offset_x = 0, offset_y = 0, projected_x = 0, rotated_y = 0;

	rotated_y = OBJECT(current_object)->y;
	projected_x = ((int32_t)(OBJECT(current_object)->x + rotated_y) >> 8) * 0xB5u;
	rotated_y = ((int32_t)(rotated_y - OBJECT(current_object)->x) >> 9) * 0xB5u;
	OBJECT(current_object)->projected_x = (int32_t)projected_x >> 16;
	OBJECT(current_object)->projected_y = rotated_y;
	offset_x = (rotated_y & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_x;
	OBJECT(current_object)->screen_x = OBJECT(current_object)->projected_x - view_x + offset_x;
	if (scroll_position < 0xA5F0u)
	{
		offset_y = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
		OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - view_y + offset_y;
		return offset_y;
	}
	offset_y = (offset_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->sprite_offset_y;
	OBJECT(current_object)->screen_y = ((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->z) >> 16) - PLAYER(current_player)->own_view_y + offset_y;
	return offset_y;
}

/* Lists in nearby_floor_blocks (count in nearby_floor_block_count) the floor blocks with one of the four
   points of their floor_block_bounds in the area view_x - 0x80 .. + 0x1C0, view_y - 0x80 .. + 0x160 (the
   bounds' extremes are tested first). Nothing while nearby_floor_blocks_locked. */
void find_nearby_floor_blocks(uint32_t *p_corner_y, uint32_t *p_floor_block, uint32_t *p_bounds_offset, uint32_t *p_nearby_count, uint32_t *p_corners_left, uint32_t *p_left, uint32_t *p_right)
{
	uint16_t corner_y = (uint16_t)*p_corner_y;
	uint16_t floor_block = (uint16_t)*p_floor_block;
	uint32_t bounds_offset = *p_bounds_offset;
	uint16_t nearby_count = (uint16_t)*p_nearby_count;
	uint32_t corners_left = *p_corners_left;
	uint16_t left = (uint16_t)*p_left;
	uint16_t right = (uint16_t)*p_right;
	uint32_t carry = 0;
	uint16_t bottom = 0, corner_x = 0, top = 0;
	uint32_t blocks_left = 0, bounds_cursor = 0, bounds_list = 0, list_cursor = 0;
	uint16_t box_left = 0, box_right = 0, box_top = 0;

	if (nearby_floor_blocks_locked != 0u)
		return;
	left = (uint16_t)view_x;
	right = (uint16_t)left;
	left -= 0x80u;
	right += 0x1C0u;
	top = (uint16_t)view_y;
	bottom = (uint16_t)top;
	top -= 0x80u;
	bottom += 0x160u;
	bounds_list = floor_block_bounds;
	blocks_left = FLOOR_OUTLINE_LIST(bounds_list)->count;
	list_cursor = nearby_floor_blocks_ADDRESS;
	bounds_offset = 0u;
	nearby_count = 0u;
	floor_block = 0u;
	bounds_list += 2u;
	do
	{
		bounds_cursor = bounds_list + bounds_offset;
		box_left = FLOOR_OUTLINE(bounds_cursor)->left_x;
		bounds_cursor += 2u;
		if ((int16_t)box_left <= (int16_t)right)
		{
			bounds_cursor += 2u;
			box_right = FLOOR_OUTLINE(bounds_cursor - 0x4u)->right_x;
			bounds_cursor += 2u;
			if ((int16_t)left <= (int16_t)box_right)
			{
				bounds_cursor += 4u;
				box_top = FLOOR_OUTLINE(bounds_cursor - 0xAu)->top_y;
				bounds_cursor += 2u;
				if ((int16_t)box_top <= (int16_t)bottom && (int16_t)top <= FLOOR_OUTLINE(bounds_cursor - 0xCu)->bottom_y)
				{
					bounds_cursor = bounds_list + bounds_offset;
					corners_left = 4u;
					do
					{
						corner_x = FLOOR_OUTLINE_CORNER(bounds_cursor)->x;
						bounds_cursor += 2u;
						corner_y = FLOOR_OUTLINE_CORNER(bounds_cursor - 0x2u)->y;
						corner_x -= left;
						carry = corner_x < 0x240u;
						bounds_cursor += 2u;
						if (corner_x < 0x240u)
						{
							corner_y -= top;
							carry = corner_y < 0x1E0u;
						}
						corners_left--;
					} while ((corners_left != 0u) & (carry == 0u));
					if (carry)
					{
						nearby_count++;
						mem_write16(list_cursor, floor_block);
						list_cursor += 2u;
					}
				}
			}
		}
		bounds_offset = (uint16_t)(bounds_offset + 0x10u);
		floor_block += 0x16u;
		blocks_left--;
	} while (blocks_left != 0u);
	nearby_floor_block_count = nearby_count;
	*p_corner_y = (*p_corner_y & 0xFFFF0000u) | corner_y;
	*p_floor_block = (*p_floor_block & 0xFFFF0000u) | floor_block;
	*p_bounds_offset = bounds_offset;
	*p_nearby_count = (*p_nearby_count & 0xFFFF0000u) | nearby_count;
	*p_corners_left = corners_left;
	*p_left = (*p_left & 0xFFFF0000u) | left;
	*p_right = (*p_right & 0xFFFF0000u) | right;
}

/* The floor under the current object, from the nearby_floor_blocks block holding its x, y: floor_block,
   floor_type, floor_attribute, then find_floor_set_floor_z (none: find_floor_no_block); on a platform
   (flags bit 3) its block and z. The old real floor goes to previous_floor_block and previous_floor_z. */
void find_floor(uint32_t *p_x, uint32_t *p_y, uint32_t *p_block_offset, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t block = 0, blocks_left = 0, candidate_offset = 0, object = 0, record = 0, slope_index = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	if ((OBJECT(current_object)->flags & 8u) == 0u)
	{
		blocks_left = nearby_floor_block_count;
		*p_cursor = nearby_floor_blocks_ADDRESS;
		block = floor_blocks + 2u;
		if (blocks_left == 0u)
		{
			find_floor_no_block(p_x, p_slope_height, p_cursor);
			return;
		}
		*p_x = OBJECT(current_object)->x;
		*p_y = OBJECT(current_object)->y;
		for (;;)
		{
			candidate_offset = mem_read16(*p_cursor);
			record = block + candidate_offset;
			x_min = FLOOR_BLOCK(record)->x_min;
			*p_cursor += 2u;
			record += 4u;
			if ((int32_t)x_min <= (int32_t)*p_x)
			{
				x_max = FLOOR_BLOCK(record - 0x4u)->x_max;
				record += 4u;
				if ((int32_t)*p_x <= (int32_t)x_max)
				{
					y_min = FLOOR_BLOCK(record - 0x8u)->y_min;
					record += 4u;
					if ((int32_t)y_min <= (int32_t)*p_y && (int32_t)*p_y <= FLOOR_BLOCK(record - 0xCu)->y_max)
						break;
				}
			}
			blocks_left--;
			if (blocks_left == 0u)
			{
				find_floor_no_block(p_x, p_slope_height, p_cursor);
				*p_block_offset = candidate_offset;
				return;
			}
		}
		if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
		{
			OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
			OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
		}
		OBJECT(current_object)->floor_block = candidate_offset;
		block += candidate_offset;
		*p_x = FLOOR_BLOCK(block)->type_index;
		record = floor_types_ADDRESS + *p_x * 4u;
		OBJECT(current_object)->floor_type = FLOOR_BLOCK_TYPE(record)->floor_type;
		OBJECT(current_object)->floor_attribute = FLOOR_BLOCK_TYPE(record)->floor_attribute;
		find_floor_set_floor_z(block, p_x, p_slope_height, p_cursor);
		*p_block_offset = candidate_offset;
		return;
	}
	if (OBJECT(current_object)->floor_block >= 0 && OBJECT(current_object)->floor_z_int < 0x7FF)
	{
		OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
		OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	}
	object = LINKED(WORK_RAM, OBJECT(current_object)->platform);
	OBJECT(current_object)->floor_block = OBJECT(object)->floor_block;
	OBJECT(current_object)->floor_z = OBJECT(object)->z;
	block = floor_blocks + 2u + OBJECT(current_object)->floor_block;
	*p_x = FLOOR_BLOCK(block)->type_index;
	record = floor_types_ADDRESS + *p_x * 4u;
	OBJECT(current_object)->floor_type = FLOOR_BLOCK_TYPE(record)->floor_type;
	OBJECT(current_object)->floor_attribute = FLOOR_BLOCK_TYPE(record)->floor_attribute;
	OBJECT(current_object)->floor_slope = FLOOR_BLOCK(block)->slope;
	*p_slope_height = 0u;
	if (FLOOR_BLOCK(block)->slope == 0u)
	{
		return;
	}
	slope_index = FLOOR_BLOCK(block)->slope;
	*p_cursor = floor_slopes_ADDRESS + (uint16_t)(slope_index * 2u) * 4u;
	if ((FLOOR_SLOPE(*p_cursor)->flags & 2u) == 0u)
		*p_slope_height = (uint32_t)OBJECT(current_object)->y - FLOOR_BLOCK(block)->y_min;
	else
		*p_slope_height = (uint32_t)OBJECT(current_object)->x - FLOOR_BLOCK(block)->x_min;
	*p_cursor += 2u;
	*p_x = (uint32_t)(uint16_t)FLOOR_BLOCK(block)->height << 16;
	*p_slope_height = (uint32_t)((int32_t)*p_slope_height >> 16) * FLOOR_SLOPE(*p_cursor - 0x2u)->factor + *p_x;
	OBJECT(current_object)->floor_z = *p_slope_height;
	*p_cursor += 4u;
}

/* find_floor when no nearby block holds the object: floor_block = -1 (a real floor is first kept in
   previous_floor_block and previous_floor_z), and floor_z and floor_slope from no_floor_block through
   find_floor_set_floor_z. */
void find_floor_no_block(uint32_t *p_x, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t block = 0;

	if (OBJECT(current_object)->floor_block < 0 || OBJECT(current_object)->floor_z_int >= 0x7FF)
	{
		OBJECT(current_object)->floor_block = -1;
		block = no_floor_block_ADDRESS;
		find_floor_set_floor_z(block, p_x, p_slope_height, p_cursor);
		return;
	}
	OBJECT(current_object)->previous_floor_block = OBJECT(current_object)->floor_block;
	OBJECT(current_object)->previous_floor_z = OBJECT(current_object)->floor_z;
	OBJECT(current_object)->floor_block = -1;
	block = no_floor_block_ADDRESS;
	find_floor_set_floor_z(block, p_x, p_slope_height, p_cursor);
}

/* floor_z = the height of block (whole pixels), floor_slope = its slope; on a slope floor_z adds (y -
   y_min, or x - x_min when the floor_slopes record has bit 1) >> 12, rounded, times the slope's factor
   >> 4. */
void find_floor_set_floor_z(uint32_t block, uint32_t *p_x, uint32_t *p_slope_height, uint32_t *p_cursor)
{
	uint32_t cursor = *p_cursor;
	uint32_t carry = 0;
	uint32_t slope_height = 0, slope_index = 0;

	OBJECT(current_object)->floor_z_int = FLOOR_BLOCK(block)->height;
	OBJECT(current_object)->floor_z_fraction = 0u;
	OBJECT(current_object)->floor_slope = FLOOR_BLOCK(block)->slope;
	slope_height = 0u;
	if (FLOOR_BLOCK(block)->slope != 0u)
	{
		slope_index = FLOOR_BLOCK(block)->slope;
		cursor = floor_slopes_ADDRESS + (uint16_t)(slope_index * 2u) * 4u;
		if ((FLOOR_SLOPE(cursor)->flags & 2u) == 0u)
			slope_height = (uint32_t)OBJECT(current_object)->y - FLOOR_BLOCK(block)->y_min;
		else
			slope_height = (uint32_t)OBJECT(current_object)->x - FLOOR_BLOCK(block)->x_min;
		cursor += 2u;
		*p_x = FLOOR_SLOPE(cursor - 0x2u)->factor;
		carry = (slope_height >> 11) & 1u;
		slope_height = (int32_t)slope_height >> 12;
		cursor += 4u;
		if (carry)
			slope_height++;
		slope_height = (int32_t)(slope_height * *p_x) >> 4;
		OBJECT(current_object)->floor_z += slope_height;
	}
	*p_slope_height = slope_height;
	*p_cursor = cursor;
}

/* Unused: find_floor's floor z for any point: block = the nearby_floor_blocks block whose box holds x, y
   (no_floor_block when none), floor_z = its height << 16 plus, on a slope, (y - y_min, or x - x_min) >>
   16 times the floor_slopes factor. With no block nearby floor_z is left alone. */
void get_floor_z_at_point(uint32_t x, uint32_t y, uint32_t *p_floor_z, uint32_t *p_block)
{
	uint32_t block = 0, block_offset = 0, blocks_left = 0, bounds = 0, cursor = 0, slope = 0, slope_distance = 0, slope_record = 0, test_x = 0, test_y = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	block = floor_blocks + 2u;
	if (blocks_left != 0u)
	{
		test_x = x;
		test_y = y;
		for (;;)
		{
			block_offset = mem_read16(cursor);
			bounds = block + block_offset;
			x_min = FLOOR_BLOCK(bounds)->x_min;
			cursor += 2u;
			bounds += 4u;
			if ((int32_t)x_min <= (int32_t)test_x)
			{
				x_max = FLOOR_BLOCK(bounds - 0x4u)->x_max;
				bounds += 4u;
				if ((int32_t)test_x <= (int32_t)x_max)
				{
					y_min = FLOOR_BLOCK(bounds - 0x8u)->y_min;
					bounds += 4u;
					if ((int32_t)y_min <= (int32_t)test_y && (int32_t)test_y <= FLOOR_BLOCK(bounds - 0xCu)->y_max)
					{
						block += block_offset;
						break;
					}
				}
			}
			blocks_left--;
			if (blocks_left == 0u)
			{
				block = no_floor_block_ADDRESS;
				break;
			}
		}
		*p_floor_z = (uint32_t)(uint16_t)FLOOR_BLOCK(block)->height << 16;
		if (FLOOR_BLOCK(block)->slope != 0u)
		{
			slope = FLOOR_BLOCK(block)->slope;
			slope_record = floor_slopes_ADDRESS + (uint16_t)(slope * 2u) * 4u;
			if ((FLOOR_SLOPE(slope_record)->flags & 2u) == 0u)
				slope_distance = y - FLOOR_BLOCK(block)->y_min;
			else
				slope_distance = x - FLOOR_BLOCK(block)->x_min;
			*p_floor_z += (slope_distance >> 16) * FLOOR_SLOPE(slope_record)->factor;
		}
	}
	*p_block = block;
}

/* Unused: hands back the floor block of nearby_floor_blocks whose box holds the point x, y, or
   no_floor_block when none does (floor_blocks + 2 when no block is nearby). */
uint32_t find_floor_block_at_point(uint32_t x, uint32_t y)
{
	uint32_t block = 0, block_offset = 0, blocks_left = 0, bounds = 0, cursor = 0, test_x = 0, test_y = 0;
	uint32_t x_min = 0, x_max = 0, y_min = 0;

	blocks_left = nearby_floor_block_count;
	cursor = nearby_floor_blocks_ADDRESS;
	block = floor_blocks + 2u;
	if (blocks_left == 0u)
		return block;
	test_x = x;
	test_y = y;
	do
	{
		block_offset = mem_read16(cursor);
		bounds = block + block_offset;
		x_min = FLOOR_BLOCK(bounds)->x_min;
		cursor += 2u;
		bounds += 4u;
		if ((int32_t)x_min <= (int32_t)test_x)
		{
			x_max = FLOOR_BLOCK(bounds - 0x4u)->x_max;
			bounds += 4u;
			if ((int32_t)test_x <= (int32_t)x_max)
			{
				y_min = FLOOR_BLOCK(bounds - 0x8u)->y_min;
				bounds += 4u;
				if ((int32_t)y_min <= (int32_t)test_y && (int32_t)test_y <= FLOOR_BLOCK(bounds - 0xCu)->y_max)
				{
					block += block_offset;
					return block;
				}
			}
		}
		blocks_left--;
	} while (blocks_left != 0u);
	block = no_floor_block_ADDRESS;
	return block;
}

/* The map x, y under the screen point point_x, point_y at height z: adds view_x and view_y, takes z off
   y, doubles it and unprojects (unproject_point) into point_x and point_y. */
void screen_point_to_ground(uint32_t z, uint32_t *p_point_x, uint32_t *p_point_y)
{
	*p_point_x = (uint32_t)(uint16_t)(*p_point_x + view_x) << 16;
	*p_point_y = (((uint32_t)(uint16_t)(*p_point_y + view_y) << 16) - z) * 2u;
	unproject_point(p_point_x, p_point_y);
}

/* The inverse of project_object: from a projected x and twice the projected y (16.16), point_x =
   ((point_x - point_y) >> 8) * 0xB5 and point_y = ((point_y + point_x) >> 8) * 0xB5, the map x and y. */
void unproject_point(uint32_t *p_point_x, uint32_t *p_point_y)
{
	uint32_t point_x = *p_point_x;
	uint32_t point_y = *p_point_y;
	uint32_t new_x = 0;

	new_x = ((int32_t)(point_x - point_y) >> 8) * 0xB5u;
	point_y = ((int32_t)(point_y + point_x) >> 8) * 0xB5u;
	point_x = new_x;
	*p_point_x = point_x;
	*p_point_y = point_y;
}

/* value becomes its integer square root, two bits of value a round for 16 rounds (steps_left counts them
   down to 0). */
void square_root(uint32_t *p_value, uint32_t *p_steps_left)
{
	uint32_t value = *p_value;
	uint32_t carry = 0;
	uint32_t radicand = 0, root_remainder = 0, steps_left = 0;

	radicand = value;
	steps_left = 0x10u;
	root_remainder = 0u;
	value = 0u;
	for (;;)
	{
		carry = radicand * 2u < radicand;
		radicand = radicand * 2u;
		root_remainder = root_remainder * 2u + carry;
		carry = radicand * 2u < radicand;
		radicand = radicand * 2u;
		root_remainder = root_remainder * 2u + carry;
		value = value * 2u + 1u;
		if (root_remainder < value)
		{
			value--;
			steps_left--;
			if (steps_left == 0u)
				break;
			continue;
		}
		root_remainder -= value;
		value++;
		steps_left--;
		if (steps_left == 0u)
			break;
	}
	value = value >> 1;
	*p_value = value;
	*p_steps_left = steps_left;
}

/* The angle (0-0x3FF) of the vector dx, dy, handed back in dx: the smaller of |dx| and |dy| over the
   larger (times 0x100) picks a byte of angle_ratios, added to the base angle angle_octants gives for the
   signs and the larger axis (taken from ~entry when the entry is negative); octant gets the entry. */
void angle_of_vector(uint32_t dy, uint32_t *p_dx, uint32_t *p_octant)
{
	uint32_t dx = *p_dx;
	uint32_t octant = 0, octant_index = 0;
	uint32_t abs_dx = 0;

	octant_index = 0u;
	dx = (uint16_t)dx;
	if ((int16_t)dx < 0)
	{
		dx = (uint16_t)(0u - dx);
		octant_index = (uint8_t)(octant_index + 1u);
	}
	dy = (uint16_t)dy;
	if ((int16_t)dy < 0)
	{
		dy = (uint16_t)(0u - dy);
		octant_index = (uint8_t)(octant_index + 2u);
	}
	if (dy < dx)
		dy = divide_u32(dy << 8, dx);
	else
	{
		abs_dx = dx;
		dx = dy;
		dy = abs_dx;
		octant_index = (uint8_t)(octant_index + 4u);
		if (dx != 0u)
		{
			dy = divide_u32(dy << 8, dx);
			dx = mem_read8(angle_ratios_ADDRESS + dy);
			octant = angle_octants[octant_index];
			if ((int16_t)octant < 0)
			{
				octant = (uint16_t)~octant;
				dx = (uint16_t)(0u - dx);
			}
			dx = (uint16_t)(dx + octant);
			dx = dx & 0x3FFu;
			*p_dx = dx;
			*p_octant = octant;
			return;
		}
	}
	dx = mem_read8(angle_ratios_ADDRESS + dy);
	octant = angle_octants[octant_index];
	if ((int16_t)octant < 0)
	{
		octant = (uint16_t)~octant;
		dx = (uint16_t)(0u - dx);
	}
	dx = (uint16_t)(dx + octant);
	dx = dx & 0x3FFu;
	*p_dx = dx;
	*p_octant = octant;
}

/* Unused: hands back the number the 8 BCD digits of value stand for, from the top digit down (times 10
   plus each nibble). */
uint32_t convert_from_bcd(uint32_t value)
{
	uint32_t digits_left = 0, number = 0;

	digits_left = 8u;
	number = 0u;
	do
	{
		value = (value << 4) | (value >> 28);
		number = number * 0xAu + (0xFu & value);
		digits_left--;
	} while (digits_left != 0u);
	value = number;
	return value;
}

/* value becomes its BCD, up to 8 digits: each division by 10 puts the remainder in the top nibble and
   shifts the rest down, and a shorter number is rotated into place. digit keeps the last remainder (past
   8 digits, the low byte of what is left, in BCD). */
void convert_to_bcd(uint32_t *p_value, uint32_t *p_digit)
{
	uint32_t value = *p_value;
	uint32_t bcd = 0, digit = 0, digits_left = 0;

	digits_left = 8u;
	bcd = 0u;
	do
	{
		digit = 0u;
		divux_pair(0xAu, &value, &digit);
		bcd += digit;
		bcd = (bcd << 28) | (bcd >> 4);
		digits_left--;
	} while ((digits_left != 0u) & (value != 0u));
	if (value != 0u)
	{
		digit = (digit & ~0xFFu) | (uint8_t)value;
		digit = (digit & ~0xFFu) | ((uint8_t)digit % 0xAu);
		digit = (digit & ~0xFFu) | (uint8_t)(digit + ((uint32_t)((uint8_t)value / 0xAu) << 4));
	}
	else if (digits_left != 0u)
	{
		do
		{
			bcd = (bcd << 28) | (bcd >> 4);
			digits_left--;
		} while (digits_left != 0u);
	}
	value = bcd;
	*p_value = value;
	*p_digit = digit;
}

/* Unused: the 64-bit number number_high:number_low in BCD, 24 digits: two divisions by 10 make each
   byte, written backwards below stack_pointer and padded with zeros, then read back most significant
   first into bcd_high, number_high and number_low. */
void convert_64_to_bcd(uint32_t *p_number_low, uint32_t *p_number_high, uint32_t *p_bcd_high)
{
	uint8_t digit_pair = 0;
	uint32_t cursor = 0, dividend = 0, pairs_left = 0, remainder = 0, value_high = 0, value_low = 0;

	value_low = *p_number_low;
	value_high = *p_number_high;
	cursor = stack_pointer;
	pairs_left = 0xAu;
	do
	{
		remainder = 0u;
		dividend = value_high;
		divux_pair(0xAu, &dividend, &remainder);
		value_high = dividend;
		dividend = value_low;
		divux_pair(0xAu, &dividend, &remainder);
		value_low = dividend;
		digit_pair = (uint8_t)remainder;
		remainder = 0u;
		dividend = value_high;
		divux_pair(0xAu, &dividend, &remainder);
		value_high = dividend;
		dividend = value_low;
		divux_pair(0xAu, &dividend, &remainder);
		value_low = dividend;
		digit_pair = (uint8_t)(((uint32_t)digit_pair << 4) | (digit_pair >> 4) | remainder);
		cursor--;
		mem_write8(cursor, ((uint32_t)digit_pair << 4) | (digit_pair >> 4));
		pairs_left--;
	} while ((pairs_left != 0u) & (value_high != 0u));
	do
	{
		remainder = 0u;
		divux_pair(0xAu, &dividend, &remainder);
		digit_pair = (uint8_t)remainder;
		remainder = 0u;
		divux_pair(0xAu, &dividend, &remainder);
		digit_pair = (uint8_t)(((uint32_t)digit_pair << 4) | (digit_pair >> 4) | remainder);
		cursor--;
		mem_write8(cursor, ((uint32_t)digit_pair << 4) | (digit_pair >> 4));
		pairs_left--;
	} while ((pairs_left != 0u) & (dividend != 0u));
	pairs_left += 2u;
	do
	{
		cursor--;
		mem_write8(cursor, dividend);
		pairs_left--;
	} while (pairs_left != 0u);
	*p_bcd_high = byte_swap32(mem_read32(cursor));
	cursor += 4u;
	*p_number_high = byte_swap32(mem_read32(cursor));
	cursor += 4u;
	*p_number_low = byte_swap32(mem_read32(cursor));
	stack_pointer = cursor + 4u;
}
