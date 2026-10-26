/**
 *    Name:        engine/contacts.c
 *    Description: contacts between objects and players: the zones, the hitter, touchable, rider, platform and wall lists, and the checks that pair them
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Matches each hitter of hitter_list with the first object of zone_list whose box holds its x and y and
   whose height overlaps its own: flags bit 27 on both and anchor_x pointing at each other. Empties
   zone_list (check_hits empties hitter_list). */
uint32_t check_zones(uint32_t box_edge)
{
	uint16_t box_start = 0;
	uint32_t hitter = 0, object = 0;

	hitter = hitter_list_ADDRESS - 0x68u;
	object = hitter_list_ADDRESS - 0x68u;
	if ((int16_t)zone_list < 0)
	{
		zone_list = 0xFFFFu;
		return box_edge;
	}
	while ((int16_t)OBJECT(hitter)->hit_list_next >= 0)
	{
		hitter = LINKED(hitter, OBJECT(hitter)->hit_list_next);
		object = LINKED(object, zone_list);
		for (;;)
		{
			box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + box_edge);
			if ((int16_t)box_start <= OBJECT(hitter)->x_int && OBJECT(hitter)->x_int <= (int16_t)(box_start + OBJECT(object)->hitbox_size_x))
			{
				box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
				box_start = (uint16_t)(OBJECT(object)->y_int + box_edge);
				if ((int16_t)box_start <= OBJECT(hitter)->y_int && OBJECT(hitter)->y_int <= (int16_t)(box_start + OBJECT(object)->hitbox_size_y))
				{
					box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(object)->z_int;
					box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(object)->hitbox_height);
					if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
					{
						box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
						box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
						if ((int16_t)box_edge <= OBJECT(object)->z_int)
						{
							OBJECT(object)->flags |= 0x8000000u;
							OBJECT(hitter)->flags |= 0x8000000u;
							OBJECT(hitter)->anchor_x = object;
							OBJECT(object)->anchor_x = hitter;
							break;
						}
					}
				}
			}
			if ((int16_t)OBJECT(object)->hit_list_next < 0)
				break;
			object = LINKED(object, OBJECT(object)->hit_list_next);
		}
	}
	zone_list = 0xFFFFu;
	return box_edge;
}

/* Puts current_object at the head of zone_list (linked by hit_list_next) and clears its flags bit 27,
   which check_zones sets when a hitter is inside its box. */
uint32_t add_to_zone_list(uint32_t old_head)
{
	old_head = LINKED(old_head, zone_list);
	zone_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x8000000u;
	return old_head;
}

/* Tests each object of hitter_list against the hitters after it (check_hit) and then, unless that has
   already hit it (flags bit 2; the last hitter is always tested), against touchable_list. Empties both
   lists for the next frame. */
void check_hits(uint32_t *p_box_edge, uint32_t *p_distance, uint32_t *p_dy, uint32_t *p_edge_or_centre_y, uint32_t *p_radius, uint32_t *p_centre_offset, uint32_t *p_previous)
{
	uint32_t distance = *p_distance;
	uint32_t dy = *p_dy;
	uint32_t edge_or_centre_y = *p_edge_or_centre_y;
	uint32_t radius = *p_radius;
	uint32_t centre_offset = *p_centre_offset;
	uint32_t previous = *p_previous;
	uint32_t object = 0, other = 0;

	if ((int16_t)hitter_list < 0)
	{
		hitter_list = 0xFFFFu;
		touchable_list = 0xFFFFu;
		return;
	}
	other = 0x200000u;
	previous = 0x200000u;
	object = LINKED(WORK_RAM, hitter_list);
	do
	{
		if ((int16_t)OBJECT(object)->hit_list_next < 0)
		{
			other = LINKED(other, touchable_list);
			if ((int16_t)other >= 0)
			{
				previous = touchable_list_ADDRESS - 0x68u;
				check_hit(other, object, p_box_edge, &distance, &dy, &edge_or_centre_y, &radius, &centre_offset, &previous);
			}
		}
		else
		{
			other = LINKED(other, OBJECT(object)->hit_list_next);
			previous = (previous & ~0xFFFFu) | (uint16_t)object;
			check_hit(other, object, p_box_edge, &distance, &dy, &edge_or_centre_y, &radius, &centre_offset, &previous);
			if ((OBJECT(object)->flags & 4u) == 0u)
			{
				other = LINKED(other, touchable_list);
				if ((int16_t)other >= 0)
				{
					previous = touchable_list_ADDRESS - 0x68u;
					check_hit(other, object, p_box_edge, &distance, &dy, &edge_or_centre_y, &radius, &centre_offset, &previous);
				}
			}
		}
		object = LINKED(object, OBJECT(object)->hit_list_next);
	} while ((int16_t)object >= 0);
	hitter_list = 0xFFFFu;
	touchable_list = 0xFFFFu;
	*p_distance = (*p_distance & 0xFFFF0000u) | (distance & 0xFFFFu);
	*p_dy = (*p_dy & 0xFFFF0000u) | (dy & 0xFFFFu);
	*p_edge_or_centre_y = (*p_edge_or_centre_y & 0xFFFF0000u) | (edge_or_centre_y & 0xFFFFu);
	*p_radius = (*p_radius & 0xFFFF0000u) | (radius & 0xFFFFu);
	*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | (centre_offset & 0xFFFFu);
	*p_previous = previous;
}

/* Tests hitter against the list from other on, skipping pairs whose kinds are both 2 or less: circle
   against circle (skewed on a slope), point against box, box against round or box against box (flags bit
   1: a box) in x and y, then the heights. The first hit sets flags bit 2, touch_distance and touched_by
   (the other) in both. */
void check_hit(uint32_t other, uint32_t hitter, uint32_t *p_box_edge, uint32_t *p_distance, uint32_t *p_dy, uint32_t *p_edge_or_centre_y, uint32_t *p_radius, uint32_t *p_centre_offset, uint32_t *p_previous)
{
	uint32_t box_edge = *p_box_edge;
	uint16_t distance = (uint16_t)*p_distance;
	uint16_t dy = (uint16_t)*p_dy;
	uint16_t edge_or_centre_y = (uint16_t)*p_edge_or_centre_y;
	uint16_t radius = (uint16_t)*p_radius;
	uint16_t centre_offset = (uint16_t)*p_centre_offset;
	uint16_t previous = (uint16_t)*p_previous;
	bool within_box_left = false, within_box_x = false;
	uint16_t box_start = 0, diameter = 0;
	uint32_t floor_slope = 0, quadrant = 0, slope_factor = 0, slope_kind = 0, slope_record = 0;

	for (;;)
	{
		if ((int8_t)OBJECT(other)->kind > 2 || (int8_t)OBJECT(hitter)->kind > 2)
		{
			if ((OBJECT(hitter)->flags & 2u) == 0u)
			{
				if ((OBJECT(other)->flags & 2u) == 0u)
				{
					radius = (uint16_t)(OBJECT(hitter)->hitbox_size_x + OBJECT(other)->hitbox_size_x);
					diameter = (uint16_t)(radius * 2u + 1u);
					centre_offset = (uint16_t)OBJECT(hitter)->hitbox_centre_x;
					dy = (uint16_t)(OBJECT(hitter)->x_int + centre_offset);
					distance = (uint16_t)(OBJECT(other)->x_int + OBJECT(other)->hitbox_centre_x - dy);
					if ((uint16_t)(distance + radius) < diameter)
					{
						centre_offset = (uint16_t)OBJECT(hitter)->hitbox_centre_y;
						edge_or_centre_y = (uint16_t)(OBJECT(hitter)->y_int + centre_offset);
						dy = (uint16_t)(OBJECT(other)->y_int + OBJECT(other)->hitbox_centre_y - edge_or_centre_y);
						if ((uint16_t)(dy + radius) < diameter)
						{
							if (OBJECT(hitter)->floor_slope != 0u)
							{
								floor_slope = OBJECT(hitter)->floor_slope;
								slope_record = floor_slopes_ADDRESS + (uint16_t)(floor_slope * 2u) * 4u;
								slope_kind = FLOOR_SLOPE(slope_record)->flags * 2u;
								slope_record++;
								switch (slope_kind)
								{
								case 0x0u:
								case 0x2u:
								case 0x8u:
									box_edge = (int16_t)dy;
									break;
								case 0x4u:
								case 0x6u:
									box_edge = (int16_t)distance;
									break;
								default:
									invalid_jump(0x1FDEu + slope_kind);
								}
								if ((int16_t)box_edge < 0)
									box_edge = 0u - box_edge;
								slope_factor = FLOOR_SLOPE(slope_record - 0x1u)->factor;
								if ((int32_t)slope_factor < 0)
									slope_factor = 0u - slope_factor;
								box_edge = (((box_edge * slope_factor) >> 8) * 0xB5u) >> 16;
								quadrant = 0u;
								if ((int16_t)distance < 0)
									quadrant = (uint16_t)(quadrant + 2u);
								if ((int16_t)dy < 0)
									quadrant = (uint16_t)(quadrant + 1u);
								switch (quadrant)
								{
								case 0:
									distance += box_edge;
									dy -= box_edge;
									break;
								case 1:
								case 4:
									if ((int16_t)distance <= (int16_t)dy)
									{
										distance -= box_edge;
										dy += box_edge;
									}
									else
									{
										distance += box_edge;
										dy -= box_edge;
									}
									break;
								case 2:
									distance -= box_edge;
									dy += box_edge;
									break;
								case 3:
									if ((int16_t)dy <= (int16_t)distance)
									{
										distance += box_edge;
										dy -= box_edge;
									}
									else
									{
										distance -= box_edge;
										dy += box_edge;
									}
									break;
								default:
									invalid_jump(0x202Eu + quadrant * 2u);
								}
							}
							dy = (uint16_t)((uint32_t)dy * dy);
							distance = (uint16_t)((uint32_t)distance * distance + dy);
							radius = (uint16_t)((uint32_t)radius * radius);
							if ((int16_t)distance <= (int16_t)radius)
							{
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
								if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
								{
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
									if ((int16_t)box_edge <= OBJECT(other)->z_int)
										break;
								}
							}
						}
					}
				}
				else
				{
					box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->hitbox_offset_x;
					box_start = (uint16_t)(OBJECT(other)->x_int + box_edge);
					if ((int16_t)box_start <= OBJECT(hitter)->x_int && OBJECT(hitter)->x_int <= (int16_t)(box_start + OBJECT(other)->hitbox_size_x))
					{
						box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->hitbox_offset_y;
						box_start = (uint16_t)(OBJECT(other)->y_int + box_edge);
						if ((int16_t)box_start <= OBJECT(hitter)->y_int && OBJECT(hitter)->y_int <= (int16_t)(box_start + OBJECT(other)->hitbox_size_y))
						{
							box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
							box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
							if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
							{
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
								if ((int16_t)box_edge <= OBJECT(other)->z_int)
									break;
							}
						}
					}
				}
			}
			else if ((OBJECT(other)->flags & 2u) == 0u)
			{
				dy = (uint16_t)OBJECT(hitter)->hitbox_offset_x;
				distance = (uint16_t)(OBJECT(hitter)->x_int + dy);
				within_box_left = (int16_t)distance <= OBJECT(other)->x_int;
				if (!within_box_left)
				{
					box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->x_int;
					box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge + OBJECT(other)->hitbox_size_x);
					within_box_left = (int16_t)distance <= (int16_t)box_edge;
				}
				if (within_box_left)
				{
					distance += OBJECT(hitter)->hitbox_size_x;
					within_box_x = OBJECT(other)->x_int <= (int16_t)distance;
					if (!within_box_x)
					{
						box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->x_int;
						box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
						within_box_x = (int16_t)box_edge <= (int16_t)distance;
					}
					if (within_box_x)
					{
						dy = (uint16_t)OBJECT(hitter)->hitbox_offset_y;
						distance = (uint16_t)(OBJECT(hitter)->y_int + dy);
						if ((int16_t)distance <= OBJECT(other)->y_int)
						{
							distance += OBJECT(hitter)->hitbox_size_y;
							if (OBJECT(other)->y_int <= (int16_t)distance)
							{
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
								if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
								{
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
									if ((int16_t)box_edge <= OBJECT(other)->z_int)
										break;
								}
							}
							else
							{
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
								if ((int16_t)box_edge <= (int16_t)distance)
								{
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
									if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
									{
										box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
										box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
										if ((int16_t)box_edge <= OBJECT(other)->z_int)
											break;
									}
								}
							}
						}
						else
						{
							box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
							box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge + OBJECT(other)->hitbox_size_x);
							if ((int16_t)distance <= (int16_t)box_edge)
							{
								distance += OBJECT(hitter)->hitbox_size_y;
								if (OBJECT(other)->y_int <= (int16_t)distance)
								{
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
									if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
									{
										box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
										box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
										if ((int16_t)box_edge <= OBJECT(other)->z_int)
											break;
									}
								}
								else
								{
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->y_int;
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_size_x);
									if ((int16_t)box_edge <= (int16_t)distance)
									{
										box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
										box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
										if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
										{
											box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
											box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
											if ((int16_t)box_edge <= OBJECT(other)->z_int)
												break;
										}
									}
								}
							}
						}
					}
				}
			}
			else
			{
				dy = (uint16_t)OBJECT(hitter)->hitbox_offset_x;
				distance = (uint16_t)(OBJECT(hitter)->x_int + dy);
				radius = (uint16_t)OBJECT(other)->hitbox_offset_x;
				edge_or_centre_y = (uint16_t)(OBJECT(other)->x_int + radius);
				if ((int16_t)edge_or_centre_y <= (int16_t)(distance + OBJECT(hitter)->hitbox_size_x))
				{
					edge_or_centre_y += OBJECT(other)->hitbox_size_x;
					if ((int16_t)distance <= (int16_t)edge_or_centre_y)
					{
						dy = (uint16_t)OBJECT(hitter)->hitbox_offset_y;
						distance = (uint16_t)(OBJECT(hitter)->y_int + dy);
						radius = (uint16_t)OBJECT(other)->hitbox_offset_y;
						edge_or_centre_y = (uint16_t)(OBJECT(other)->y_int + radius);
						if ((int16_t)edge_or_centre_y <= (int16_t)(distance + OBJECT(hitter)->hitbox_size_y))
						{
							edge_or_centre_y += OBJECT(other)->hitbox_size_y;
							if ((int16_t)distance <= (int16_t)edge_or_centre_y)
							{
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(other)->z_int;
								box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(other)->hitbox_height);
								if ((int16_t)box_edge <= OBJECT(hitter)->z_int)
								{
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)OBJECT(hitter)->z_int;
									box_edge = (box_edge & ~0xFFFFu) | (uint16_t)(box_edge - OBJECT(hitter)->hitbox_height);
									if ((int16_t)box_edge <= OBJECT(other)->z_int)
										break;
								}
							}
						}
					}
				}
			}
		}
		if ((int16_t)OBJECT(other)->hit_list_next < 0)
		{
			*p_box_edge = box_edge;
			*p_distance = (*p_distance & 0xFFFF0000u) | distance;
			*p_dy = (*p_dy & 0xFFFF0000u) | dy;
			*p_edge_or_centre_y = (*p_edge_or_centre_y & 0xFFFF0000u) | edge_or_centre_y;
			*p_radius = (*p_radius & 0xFFFF0000u) | radius;
			*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | centre_offset;
			*p_previous = (*p_previous & 0xFFFF0000u) | previous;
			return;
		}
		previous = (uint16_t)other;
		other = LINKED(other, OBJECT(other)->hit_list_next);
	}
	OBJECT(other)->flags |= 4u;
	OBJECT(hitter)->flags |= 4u;
	OBJECT(hitter)->touch_distance = distance;
	OBJECT(other)->touch_distance = distance;
	OBJECT(hitter)->touched_by = other;
	OBJECT(other)->touched_by = hitter;
	*p_box_edge = box_edge;
	*p_distance = (*p_distance & 0xFFFF0000u) | distance;
	*p_dy = (*p_dy & 0xFFFF0000u) | dy;
	*p_edge_or_centre_y = (*p_edge_or_centre_y & 0xFFFF0000u) | edge_or_centre_y;
	*p_radius = (*p_radius & 0xFFFF0000u) | radius;
	*p_centre_offset = (*p_centre_offset & 0xFFFF0000u) | centre_offset;
	*p_previous = (*p_previous & 0xFFFF0000u) | previous;
}

/* Puts current_object at the head of hitter_list (linked by hit_list_next) and clears its flags bits 2
   (hit) and 27 (in a zone) for this frame's check_hits and check_zones. */
void add_to_hitter_list(void)
{
	uint16_t old_head = 0;

	old_head = hitter_list;
	hitter_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x8000000u;
}

/* Puts current_object at the head of touchable_list (linked by hit_list_next), which check_hits tests
   the hitters against, and clears its flags bits 2 (hit) and 6 (hit by a player's attack). */
uint32_t add_to_touchable_list(uint32_t old_head)
{
	old_head = LINKED(old_head, touchable_list);
	touchable_list = current_object;
	OBJECT(current_object)->hit_list_next = old_head;
	OBJECT(current_object)->flags &= ~4u;
	OBJECT(current_object)->flags &= ~0x40u;
	return old_head;
}

/* Puts current_object at the head of rider_list (linked by platform_list_next) and clears its flags bits
   3 (on a platform) and 5 (in a wall) for check_platforms and check_walls. */
void add_to_rider_list(void)
{
	uint16_t old_head = 0;

	old_head = rider_list;
	rider_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~8u;
	OBJECT(current_object)->flags &= ~0x20u;
}

/* Puts current_object at the head of platform_list (linked by platform_list_next) and clears its flags
   bit 3, which check_platforms sets when a rider stands on it. */
void add_to_platform_list(void)
{
	uint16_t old_head = 0;

	old_head = platform_list;
	platform_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~8u;
}

/* Matches each rider with the first platform whose box (no offsets) holds its x and y: rider flags bit
   3, platform pointing at each other. A rider not above floor_z sets the platform's bit 3, the first
   adding its member_number to a bridge load list (by spawn_arg_2_high). Then compute_bridge_sag per
   bridge; both lists emptied. */
void check_platforms(uint32_t *p_middle, uint32_t *p_plank, uint32_t *p_span, uint32_t *p_sag, uint32_t *p_sag_step, uint32_t *p_load_count, uint32_t *p_passes_left, uint32_t *p_platform, uint32_t *p_load_list)
{
	uint32_t middle = *p_middle;
	uint32_t sag = *p_sag;
	uint32_t sag_step = *p_sag_step;
	uint32_t load_list = *p_load_list;
	uint16_t plank_count = 0, x_int = 0, y_int = 0;
	uint32_t load_list_2 = 0, object = 0, rider = 0;
	uint32_t flags = 0;

	bridge_0_load = 2u;
	bridge_0_load_first = 0u;
	bridge_0_load_last = (uint8_t)bridge_0_plank_count;
	bridge_1_load = 2u;
	bridge_1_load_first = 0u;
	bridge_1_load_last = (uint8_t)bridge_1_plank_count;
	rider = 0x20F01Eu;
	object = 0x20F01Eu;
	if ((int16_t)platform_list >= 0)
	{
		while ((int16_t)OBJECT(rider)->platform_list_next >= 0)
		{
			rider = LINKED(rider, OBJECT(rider)->platform_list_next);
			object = LINKED(object, platform_list);
			for (;;)
			{
				x_int = (uint16_t)OBJECT(object)->x_int;
				if (OBJECT(rider)->x_int < (int16_t)x_int || (int16_t)(x_int + OBJECT(object)->hitbox_size_x) < OBJECT(rider)->x_int)
				{
					if ((int16_t)OBJECT(object)->platform_list_next < 0)
						break;
					object = LINKED(object, OBJECT(object)->platform_list_next);
					continue;
				}
				y_int = (uint16_t)OBJECT(object)->y_int;
				if (OBJECT(rider)->y_int < (int16_t)y_int || (int16_t)(y_int + OBJECT(object)->hitbox_size_y) < OBJECT(rider)->y_int)
				{
					if ((int16_t)OBJECT(object)->platform_list_next < 0)
						break;
					object = LINKED(object, OBJECT(object)->platform_list_next);
					continue;
				}
				OBJECT(rider)->flags |= 8u;
				OBJECT(rider)->platform = object;
				OBJECT(object)->platform = rider;
				if (OBJECT(rider)->z_int < OBJECT(object)->floor_z_int)
					break;
				flags = OBJECT(object)->flags;
				OBJECT(object)->flags = flags | 8u;
				if ((flags & 8u) != 0u)
					break;
				load_list = bridge_0_load_ADDRESS;
				if (OBJECT(object)->spawn_arg_2_high != 0u)
					load_list = bridge_1_load_ADDRESS;
				BRIDGE_LOAD(load_list)->count++;
				*p_load_count = BRIDGE_LOAD(load_list)->count;
				mem_write8(load_list + *p_load_count, OBJECT(object)->member_number);
				break;
			}
		}
		if (bridge_0_plank_count != 0u)
		{
			load_list_2 = bridge_0_load_ADDRESS;
			current_player = bridge_0_sag_ADDRESS;
			plank_count = bridge_0_plank_count;
			compute_bridge_sag(load_list_2, plank_count, &middle, p_plank, p_span, &sag, &sag_step, p_load_count, p_passes_left, &object, &load_list);
		}
		if (bridge_1_plank_count != 0u)
		{
			load_list_2 = bridge_1_load_ADDRESS;
			current_player = bridge_1_sag_ADDRESS;
			plank_count = bridge_1_plank_count;
			compute_bridge_sag(load_list_2, plank_count, &middle, p_plank, p_span, &sag, &sag_step, p_load_count, p_passes_left, &object, &load_list);
		}
	}
	rider_list = 0xFFFFu;
	platform_list = 0xFFFFu;
	*p_middle = (*p_middle & 0xFFFF0000u) | (middle & 0xFFFFu);
	*p_sag = (*p_sag & 0xFFFF0000u) | (sag & 0xFFFFu);
	*p_sag_step = (*p_sag_step & 0xFFFF0000u) | (sag_step & 0xFFFFu);
	*p_platform = object;
	*p_load_list = load_list;
}

/* Fills a bridge's sag table, which check_platforms passes in current_player, from load_list: with only
   the ends 0 and plank_count (no plank loaded) it is cleared; else the loaded planks are sorted and the
   table gets straight lines between consecutive ones, each sagging bridge_sag_curve[distance to the
   nearer end] << 8. */
void compute_bridge_sag(uint32_t load_list, uint16_t plank_count, uint32_t *p_middle, uint32_t *p_plank, uint32_t *p_span, uint32_t *p_sag, uint32_t *p_sag_step, uint32_t *p_count, uint32_t *p_passes_left, uint32_t *p_load_entry, uint32_t *p_sag_curve)
{
	uint16_t middle = (uint16_t)*p_middle;
	uint32_t plank = *p_plank;
	uint32_t span = *p_span;
	uint16_t sag = (uint16_t)*p_sag;
	uint16_t sag_step = (uint16_t)*p_sag_step;
	uint32_t passes_left = *p_passes_left;
	uint32_t load_entry = *p_load_entry;
	uint8_t smallest = 0, swapped = 0;
	uint16_t plank_total = 0;
	uint32_t clear_dest = 0, compares_left = 0, count = 0, curve_index = 0, fill_word = 0, next_curve_index = 0, sag_cursor = 0, sag_curve = 0, scan = 0, slot = 0;

	if (BRIDGE_LOAD(load_list)->count == 2u)
	{
		count = ((uint16_t)plank_count >> 1) + 1u;
		clear_dest = current_player;
		fill_word = 0u;
		do
		{
			mem_write32(clear_dest, fill_word);
			count--;
			clear_dest += 4u;
		} while (count != 0u);
		*p_count = count;
		*p_sag_curve = clear_dest;
		return;
	}
	slot = load_list;
	passes_left = BRIDGE_LOAD(slot)->count;
	passes_left = (uint8_t)(passes_left - 1u);
	slot++;
	do
	{
		compares_left = passes_left;
		smallest = mem_read8(slot);
		scan = slot + 1u;
		do
		{
			if ((int8_t)mem_read8(scan) < (int8_t)smallest)
			{
				swapped = (uint8_t)smallest;
				smallest = mem_read8(scan);
				mem_write8(scan, swapped);
			}
			scan++;
			compares_left--;
		} while (compares_left != 0u);
		mem_write8(slot, smallest);
		passes_left--;
		slot++;
	} while (passes_left != 0u);
	sag_cursor = current_player;
	load_entry = load_list;
	count = BRIDGE_LOAD(load_entry)->count;
	count = (uint8_t)(count - 1u);
	sag_curve = bridge_sag_curve_ADDRESS;
	load_entry++;
	do
	{
		plank = mem_read8(load_entry);
		curve_index = plank;
		middle = (uint16_t)plank_count;
		plank_total = (uint16_t)middle;
		middle = middle >> 1;
		load_entry++;
		if ((int16_t)middle < (int16_t)curve_index)
		{
			curve_index = (uint16_t)(0u - curve_index);
			curve_index = (uint16_t)(curve_index + plank_total);
		}
		sag = BRIDGE_SAG_DEPTHS(sag_curve)[curve_index];
		span = mem_read8(load_entry);
		next_curve_index = span;
		if ((int16_t)middle < (int16_t)next_curve_index)
		{
			next_curve_index = (uint16_t)(0u - next_curve_index);
			next_curve_index = (uint16_t)(next_curve_index + plank_total);
		}
		sag = (uint16_t)((uint32_t)sag << 8);
		span = (uint16_t)(span - plank);
		sag_step = divide_s16((uint16_t)(((uint32_t)BRIDGE_SAG_DEPTHS(sag_curve)[next_curve_index] << 8) - sag), span);
		do
		{
			mem_write16(sag_cursor, sag);
			sag += sag_step;
			span--;
			sag_cursor += 2u;
		} while (span != 0u);
		count--;
	} while (count != 0u);
	*p_middle = (*p_middle & 0xFFFF0000u) | middle;
	*p_plank = plank;
	*p_span = span;
	*p_sag = (*p_sag & 0xFFFF0000u) | sag;
	*p_sag_step = (*p_sag_step & 0xFFFF0000u) | sag_step;
	*p_count = count;
	*p_passes_left = passes_left;
	*p_load_entry = load_entry;
	*p_sag_curve = sag_curve;
}

/* Puts current_object, a wall, at the head of wall_list (linked by platform_list_next) and clears its
   flags bit 5, which check_walls sets when a rider is inside it. */
void add_to_wall_list(void)
{
	uint16_t old_head = 0;

	old_head = wall_list;
	wall_list = current_object;
	OBJECT(current_object)->platform_list_next = old_head;
	OBJECT(current_object)->flags &= ~0x20u;
}

/* Matches each rider of rider_list with the first wall of wall_list whose box holds its x and y and
   whose height (hitbox_height up from its z) holds the rider's z: flags bit 5 on both and wall pointing
   at each other. Empties wall_list. */
uint32_t check_walls(uint32_t hitbox_offset)
{
	uint16_t box_start = 0;
	uint32_t object = 0, rider = 0;

	rider = 0x20F01Eu;
	object = 0x20F01Eu;
	if ((int16_t)wall_list < 0)
	{
		wall_list = 0xFFFFu;
		return hitbox_offset;
	}
	while ((int16_t)OBJECT(rider)->platform_list_next >= 0)
	{
		rider = LINKED(rider, OBJECT(rider)->platform_list_next);
		object = LINKED(object, wall_list);
		for (;;)
		{
			hitbox_offset = (hitbox_offset & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_x;
			box_start = (uint16_t)(OBJECT(object)->x_int + hitbox_offset);
			if ((int16_t)box_start <= OBJECT(rider)->x_int && OBJECT(rider)->x_int <= (int16_t)(box_start + OBJECT(object)->hitbox_size_x))
			{
				hitbox_offset = (hitbox_offset & ~0xFFFFu) | (uint16_t)OBJECT(object)->hitbox_offset_y;
				box_start = (uint16_t)(OBJECT(object)->y_int + hitbox_offset);
				if ((int16_t)box_start <= OBJECT(rider)->y_int && OBJECT(rider)->y_int <= (int16_t)(box_start + OBJECT(object)->hitbox_size_y) && (uint16_t)(OBJECT(object)->z_int - OBJECT(rider)->z_int) < OBJECT(object)->hitbox_height)
				{
					OBJECT(rider)->flags |= 0x20u;
					OBJECT(object)->flags |= 0x20u;
					OBJECT(rider)->wall = object;
					OBJECT(object)->wall = rider;
					break;
				}
			}
			if ((int16_t)OBJECT(object)->platform_list_next < 0)
				break;
			object = LINKED(object, OBJECT(object)->platform_list_next);
		}
	}
	wall_list = 0xFFFFu;
	return hitbox_offset;
}
