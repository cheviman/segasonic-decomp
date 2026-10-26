/**
 *    Name:        engine/sprites.c
 *    Description: animation and the sprite list: animation steps (frames, flips, moves, offsets, multi-part sprites), part palette cycles, the depth buckets, build_sprite_list and the buffer swap at VBLANK
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Every palette_delay frames (palette_timer) the next palette of palette_list (its first word is the
   last index; it wraps) goes into every part of the object's part_list. */
void cycle_part_palettes(void)
{
	uint8_t last_index = 0;
	uint16_t palette = 0;
	uint32_t next_index = 0, palettes = 0, part = 0, parts_left = 0;
	uint8_t palette_timer = 0;

	palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_timer;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = palette_timer - 1u;
	if ((int8_t)palette_timer > 1)
		return;
	OBJECT(current_object)->cycle_part_palettes.palette_timer = OBJECT(current_object)->cycle_part_palettes.palette_delay;
	palettes = OBJECT(current_object)->cycle_part_palettes.palette_list;
	last_index = (uint8_t)HUD_PALETTE_CYCLE(palettes)->last_index;
	next_index = OBJECT(current_object)->cycle_part_palettes.palette_index;
	next_index = (uint8_t)(next_index + 1u);
	palettes += 2u;
	if ((int8_t)last_index <= (int8_t)next_index && (int8_t)last_index < (int8_t)next_index)
		next_index = 0u;
	OBJECT(current_object)->cycle_part_palettes.palette_index = next_index;
	palette = HUD_CYCLE_PALETTES(palettes)[next_index];
	part = OBJECT(current_object)->part_list;
	parts_left = SPRITE_PART_LIST(part)->count;
	part += 2u;
	do
	{
		SPRITE_PART(part)->palette = palette;
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
}

/* The animation timer (animation_timer, reloaded from animation_speed when it runs out), then
   animate_step_with_flags. */
void animate_with_flags(uint32_t *p_frame, uint32_t *p_at_last_step)
{
	uint32_t at_last_step = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer <= 1)
	{
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		animate_step_with_flags(p_frame, &at_last_step);
	}
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
}

/* The next step of an animation of two-word steps, always forwards: its flags word goes into
   sprite_flags in place of the flips (bits 6-7) and its frame into sprite_frame. at_last_step is FF on
   the last step. */
void animate_step_with_flags(uint32_t *p_frame, uint32_t *p_at_last_step)
{
	uint8_t at_last_step = (uint8_t)*p_at_last_step;
	uint8_t last_frame = 0;
	uint32_t frame = 0, step_record = 0;

	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)FRAME_ANIMATION(step_record)->last_frame;
	frame = OBJECT(current_object)->animation_frame;
	frame = (uint8_t)(frame + 1u);
	step_record += 2u;
	if ((int8_t)last_frame <= (int8_t)frame)
	{
		if ((int8_t)last_frame < (int8_t)frame)
			frame = 0u;
		else
			at_last_step = 0xFFu;
	}
	OBJECT(current_object)->animation_frame = frame;
	frame = frame * 2u;
	step_record += frame * 2u;
	OBJECT(current_object)->sprite_flags = (OBJECT(current_object)->sprite_flags & ~0xC0u) | FLAGGED_STEP(step_record)->flags;
	OBJECT(current_object)->sprite_frame = FLAGGED_STEP(step_record)->frame;
	*p_frame = frame;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
}

/* Counts animation_timer down; when it runs out it reloads it from animation_speed and moves to the next
   step (animate_step). wrapped and at_last_step come back FF when the step wrapped round or is the last. */
void animate(uint32_t *p_frame, uint32_t *p_wrapped, uint32_t *p_at_last_step)
{
	uint32_t at_last_step = 0, wrapped = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	wrapped = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer <= 1)
	{
		OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
		animate_step(p_frame, &wrapped, &at_last_step);
	}
	*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | (wrapped & 0xFFu);
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | (at_last_step & 0xFFu);
}

/* animation_frame + 1 (- 1 with flags bit 7) and sprite_frame = that step's word. Past the last step
   (its number is the animation's first word) it wraps to 0 (going back, to the last) with wrapped FF;
   reaching the last step (0 going back) sets at_last_step FF. frame is the new step. */
void animate_step(uint32_t *p_frame, uint32_t *p_wrapped, uint32_t *p_at_last_step)
{
	uint8_t wrapped = (uint8_t)*p_wrapped;
	uint8_t at_last_step = (uint8_t)*p_at_last_step;
	uint8_t last_frame = 0;
	uint32_t frame = 0, frame_list = 0;

	frame_list = OBJECT(current_object)->animation;
	last_frame = (uint8_t)FRAME_ANIMATION(frame_list)->last_frame;
	frame = OBJECT(current_object)->animation_frame;
	frame_list += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)last_frame <= (int8_t)frame)
		{
			if ((int8_t)last_frame < (int8_t)frame)
			{
				frame = 0u;
				wrapped = 0xFFu;
			}
			else
				at_last_step = 0xFFu;
		}
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame <= 0)
		{
			if ((int8_t)frame < 0)
			{
				frame = (uint8_t)last_frame;
				wrapped = 0xFFu;
			}
			else
				at_last_step = 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = frame;
	OBJECT(current_object)->sprite_frame = SPRITE_FRAME_NUMBERS(frame_list)[frame];
	*p_frame = frame;
	*p_wrapped = (*p_wrapped & 0xFFFFFF00u) | wrapped;
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
}

/* animate with steps of frame, dx, dy: on each new step sprite_frame takes the frame and projected_x and
   projected_y_int move by dx, dy (negated by the flips). Hands back at_last_step FF on the last step. */
uint32_t animate_and_move(uint32_t at_last_step)
{
	uint8_t last_frame = 0;
	uint16_t delta = 0;
	uint32_t frame = 0, step_record = 0;
	uint8_t animation_timer = 0;

	at_last_step &= ~0xFFu;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return at_last_step;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)FRAME_ANIMATION(step_record)->last_frame;
	frame = OBJECT(current_object)->animation_frame;
	step_record += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)last_frame <= (int8_t)frame)
		{
			if ((int8_t)last_frame < (int8_t)frame)
				frame = 0u;
			else
				at_last_step |= 0xFFu;
		}
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame <= 0)
		{
			if ((int8_t)frame < 0)
				frame = (uint8_t)last_frame;
			else
				at_last_step |= 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = frame;
	step_record += frame * 4u;
	OBJECT(current_object)->sprite_frame = ANIMATION_STEP(step_record)->frame;
	step_record += 2u;
	delta = (uint16_t)ANIMATION_STEP(step_record - 0x2u)->dx;
	step_record++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_x += delta;
	delta = (uint16_t)ANIMATION_STEP(step_record - 0x3u)->dy;
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_y_int += delta;
	return at_last_step;
}

/* The same steps of frame, dx, dy, but placing the object at anchor_x, anchor_y plus dx, dy
   (set_step_around_anchor). Hands back FFFFFFFF on the last step. */
uint32_t animate_around_anchor(void)
{
	uint8_t last_frame = 0;
	uint32_t at_last_step = 0, frame = 0, steps = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return at_last_step;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	steps = OBJECT(current_object)->animation;
	last_frame = (uint8_t)FRAME_ANIMATION(steps)->last_frame;
	frame = OBJECT(current_object)->animation_frame;
	steps += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)last_frame <= (int8_t)frame)
		{
			if ((int8_t)last_frame < (int8_t)frame)
				frame = 0u;
			else
				at_last_step = 0xFFFFFFFFu;
		}
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame <= 0)
		{
			if ((int8_t)frame < 0)
				frame = (uint8_t)last_frame;
			else
				at_last_step = 0xFFFFFFFFu;
		}
	}
	OBJECT(current_object)->animation_frame = frame;
	set_step_around_anchor(frame, steps);
	return at_last_step;
}

/* Puts the current object on step frame of the steps at steps (a frame word, dx, dy): sprite_frame = the
   step's frame, projected_x = anchor_x + dx, projected_y_int = anchor_y + dy, dx and dy negated by the
   flips. */
void set_step_around_anchor(uint32_t frame, uint32_t steps)
{
	uint16_t delta = 0;

	steps += frame * 4u;
	OBJECT(current_object)->sprite_frame = ANIMATION_STEP(steps)->frame;
	steps += 2u;
	delta = (uint16_t)ANIMATION_STEP(steps - 0x2u)->dx;
	steps++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_x = delta + OBJECT(current_object)->anchor_x;
	delta = (uint16_t)ANIMATION_STEP(steps - 0x3u)->dy;
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->projected_y_int = delta + OBJECT(current_object)->anchor_y;
}

/* animate with steps of frame, dx, dy whose dx, dy are added to screen_x and screen_y
   (add_step_screen_offset); between steps the current step's offset is added again
   (apply_step_screen_offset). Hands back at_last_step FF on the last step. */
uint32_t animate_with_screen_offset(uint32_t at_last_step)
{
	uint8_t last_frame = 0;
	uint32_t frame = 0, step_record = 0;
	uint8_t animation_timer = 0;

	at_last_step &= ~0xFFu;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		apply_step_screen_offset();
		return at_last_step;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	step_record = OBJECT(current_object)->animation;
	last_frame = (uint8_t)FRAME_ANIMATION(step_record)->last_frame;
	frame = OBJECT(current_object)->animation_frame;
	step_record += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		frame = (uint8_t)(frame + 1u);
		if ((int8_t)last_frame <= (int8_t)frame)
		{
			if ((int8_t)last_frame < (int8_t)frame)
				frame = 0u;
			else
				at_last_step |= 0xFFu;
		}
	}
	else
	{
		frame = (uint8_t)(frame - 1u);
		if ((int8_t)frame <= 0)
		{
			if ((int8_t)frame < 0)
				frame = (uint8_t)last_frame;
			else
				at_last_step |= 0xFFu;
		}
	}
	OBJECT(current_object)->animation_frame = frame;
	step_record += frame * 4u;
	OBJECT(current_object)->sprite_frame = ANIMATION_STEP(step_record)->frame;
	step_record += 2u;
	add_step_screen_offset(step_record);
	return at_last_step;
}

/* Adds the current animation step's dx, dy to screen_x and screen_y again, without moving on
   (animate_with_screen_offset between steps). */
void apply_step_screen_offset(void)
{
	uint32_t step_offset = 0;

	step_offset = OBJECT(current_object)->animation + 4u + OBJECT(current_object)->animation_frame * 4u;
	add_step_screen_offset(step_offset);
}

/* Adds the dx, dy step_offset points at (an animation step's, after its frame word) to screen_x and
   screen_y, negated by the x and y flips (sprite_flags bits 6 and 7). */
void add_step_screen_offset(uint32_t step_offset)
{
	uint16_t delta = 0;

	delta = (uint16_t)ANIMATION_STEP(step_offset - 0x2u)->dx;
	step_offset++;
	if ((OBJECT(current_object)->sprite_flags & 0x40u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->screen_x += delta;
	delta = (uint16_t)ANIMATION_STEP(step_offset - 0x3u)->dy;
	if ((OBJECT(current_object)->sprite_flags & 0x80u) != 0u)
		delta = (uint16_t)(0u - delta);
	OBJECT(current_object)->screen_y += delta;
}

/* The animation timer of a sprite whose steps are part lists: when animation_timer runs out it reloads
   from animation_speed and animate_part_list_step moves on. */
void animate_part_list(void)
{
	uint8_t animation_timer = 0;

	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
		return;
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	animate_part_list_step();
}

/* animate_step for multi-part sprites: animation_frame moves on (backwards with flags bit 7, wrapping)
   and part_list = the animation's address plus the step's word, the next part list. */
void animate_part_list_step(void)
{
	uint8_t max_frame = 0;
	uint32_t frame_offsets = 0, next_frame = 0;

	frame_offsets = OBJECT(current_object)->animation;
	max_frame = (uint8_t)FRAME_ANIMATION(frame_offsets)->last_frame;
	next_frame = OBJECT(current_object)->animation_frame;
	frame_offsets += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		next_frame = (uint8_t)(next_frame + 1u);
		if ((int8_t)max_frame <= (int8_t)next_frame && (int8_t)max_frame < (int8_t)next_frame)
			next_frame = 0u;
	}
	else
	{
		next_frame = (uint8_t)(next_frame - 1u);
		if ((int8_t)next_frame <= 0 && (int8_t)next_frame < 0)
			next_frame = (uint8_t)max_frame;
	}
	OBJECT(current_object)->animation_frame = next_frame;
	OBJECT(current_object)->part_list = PART_LIST_OFFSETS(frame_offsets)[next_frame] + frame_offsets - 2u;
}

/* animate for a multi-part sprite: on each new step (backwards with flags bit 7, wrapping) the step's
   frame is written into every part of part_list. at_last_step is FF on the last step; part ends past the
   last part. */
void animate_all_parts(uint32_t *p_at_last_step, uint32_t *p_part)
{
	uint32_t part = *p_part;
	uint8_t at_last_step = 0, max_frame = 0;
	uint16_t frame_sprite = 0;
	uint32_t frame_sprites = 0, next_frame = 0, parts_left = 0;
	uint8_t animation_timer = 0;

	at_last_step = 0u;
	animation_timer = OBJECT(current_object)->animation_timer;
	OBJECT(current_object)->animation_timer = animation_timer - 1u;
	if ((int8_t)animation_timer > 1)
	{
		*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
		return;
	}
	OBJECT(current_object)->animation_timer = OBJECT(current_object)->animation_speed;
	frame_sprites = OBJECT(current_object)->animation;
	max_frame = (uint8_t)FRAME_ANIMATION(frame_sprites)->last_frame;
	next_frame = OBJECT(current_object)->animation_frame;
	frame_sprites += 2u;
	if ((OBJECT(current_object)->flags & 0x80u) == 0u)
	{
		next_frame = (uint8_t)(next_frame + 1u);
		if ((int8_t)next_frame < (int8_t)max_frame)
		{
			OBJECT(current_object)->animation_frame = next_frame;
			frame_sprite = SPRITE_FRAME_NUMBERS(frame_sprites)[next_frame];
			part = OBJECT(current_object)->part_list;
			parts_left = SPRITE_PART_LIST(part)->count;
			part += 2u;
			do
			{
				SPRITE_PART(part)->frame = frame_sprite;
				part += 8u;
				parts_left--;
			} while (parts_left != 0u);
			*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
			*p_part = part;
			return;
		}
		if ((int8_t)max_frame < (int8_t)next_frame)
			next_frame = 0u;
		else
			at_last_step = 0xFFu;
	}
	else
	{
		next_frame = (uint8_t)(next_frame - 1u);
		if ((int8_t)next_frame > 0)
		{
			OBJECT(current_object)->animation_frame = next_frame;
			frame_sprite = SPRITE_FRAME_NUMBERS(frame_sprites)[next_frame];
			part = OBJECT(current_object)->part_list;
			parts_left = SPRITE_PART_LIST(part)->count;
			part += 2u;
			do
			{
				SPRITE_PART(part)->frame = frame_sprite;
				part += 8u;
				parts_left--;
			} while (parts_left != 0u);
			*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
			*p_part = part;
			return;
		}
		if ((int8_t)next_frame < 0)
			next_frame = (uint8_t)max_frame;
		else
			at_last_step = 0xFFu;
	}
	OBJECT(current_object)->animation_frame = next_frame;
	frame_sprite = SPRITE_FRAME_NUMBERS(frame_sprites)[next_frame];
	part = OBJECT(current_object)->part_list;
	parts_left = SPRITE_PART_LIST(part)->count;
	part += 2u;
	do
	{
		SPRITE_PART(part)->frame = frame_sprite;
		part += 8u;
		parts_left--;
	} while (parts_left != 0u);
	*p_at_last_step = (*p_at_last_step & 0xFFFFFF00u) | at_last_step;
	*p_part = part;
}

/* Unused: init_sprites with vblank_hook = cancel_sprite_buffer_swap: the VBLANK only clears
   sprite_list_ready, so the sprite buffers never swap. */
void init_sprites_without_swaps(void)
{
	uint32_t end_mark = 0, ignored = 0;

	vblank_hook = cancel_sprite_buffer_swap_ADDRESS;
	init_sprites_from_frame_wait(&end_mark, &ignored);
}

/* Unused: init_sprites with vblank_hook = swap_sprite_buffers_now, a swap at every VBLANK whether the
   list is ready or not, and frames_to_wait = frames_per_wait = 1. */
void init_sprites_swapping_every_frame(void)
{
	uint32_t end_mark = 0, ignored = 0;

	vblank_hook = swap_sprite_buffers_now_ADDRESS;
	frames_to_wait = 1;
	frames_per_wait = 1u;
	init_sprite_hardware(&end_mark, &ignored);
}

/* Sets the sprites up: vblank_hook = swap_sprite_buffers, frames_to_wait = frames_per_wait = 0, then
   init_sprite_hardware (through init_sprites_from_frame_wait). */
void init_sprites(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint32_t end_mark = 0;

	vblank_hook = swap_sprite_buffers_ADDRESS;
	init_sprites_from_frame_wait(&end_mark, p_source);
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | (end_mark & 0xFFFFu);
}

/* init_sprites from its frame wait on: frames_to_wait = frames_per_wait = 0, then init_sprite_hardware.
   init_sprites_without_swaps comes in here. */
void init_sprites_from_frame_wait(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint32_t end_mark = 0;

	frames_to_wait = 0;
	frames_per_wait = 0u;
	init_sprite_hardware(&end_mark, p_source);
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | (end_mark & 0xFFFFu);
}

/* The end of init_sprites: sprite_flip from setting_monitor_flip, the sprite control registers from
   sprite_control_init, entries 0-1 from sprite_list_header, sprite_back_buffer and sprite_list_start
   after them, end marks (0xFFFF) in the buffers, sprite_frames at the frame records,
   clear_sprite_buckets. */
void init_sprite_hardware(uint32_t *p_end_mark, uint32_t *p_source)
{
	uint16_t end_mark = 0;
	uint32_t control_cursor = 0, control_init = 0, dest_end = 0, ignored = 0, ignored_2 = 0, list_start = 0, registers_left = 0;

	control_init = sprite_control_init_ADDRESS;
	mem_write16(sprite_flip, setting_monitor_flip);
	registers_left = 4u;
	control_cursor = SPRITE_CONTROL + 0x6u;
	do
	{
		mem_write16(control_cursor, SPRITE_CONTROL_SETTINGS(control_init)[0]);
		registers_left--;
		control_init++;
		control_cursor += 2u;
	} while (registers_left != 0u);
	move_string(sprite_list_header_ADDRESS, 0x10u, 0x400000u, 0x10u, 2u, 0u, 0u, 0u, &dest_end, &ignored);
	list_start = dest_end;
	sprite_jump_init = 0xA400u;
	sprite_buffer_flip = 0u;
	sprite_back_buffer = list_start;
	sprite_list_start = list_start;
	end_mark = 0xFFFFu;
	mem_write16(list_start, 0xFFFFu);
	mem_write16(list_start + 0x1FF0u, 0xFFFFu);
	mem_write16(list_start + 0x3FE0u, 0xFFFFu);
	mem_write16(list_start + 0x5FD0u, 0xFFFFu);
	sprite_list_end_command = 0xFFFFu;
	sprite_frames = 0x130000u;
	ignored_2 = clear_sprite_buckets();
	*p_end_mark = (*p_end_mark & 0xFFFF0000u) | end_mark;
	*p_source = control_init;
}

/* Empties the 256 depth buckets (each points at itself) and puts the end mark 0xFFFF in
   sprite_buckets_end; hands back the address past the last bucket. */
uint32_t clear_sprite_buckets(void)
{
	uint32_t bucket = 0, buckets_left = 0;

	sprite_buckets_end = 0xFFFFu;
	buckets_left = 0x100u;
	bucket = sprite_buckets_ADDRESS;
	do
	{
		OBJECT(bucket)->draw_next = bucket;
		bucket += 2u;
		buckets_left--;
	} while (buckets_left != 0u);
	return bucket;
}

/* Queues the current object for drawing at the depth of its screen_y (queue_sprite_at_depth); hands back
   the clamped depth. */
uint32_t queue_sprite(uint32_t depth)
{
	uint32_t ignored = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	queue_sprite_at_depth(&ignored, &depth);
	return depth;
}

/* Pushes the current object (through draw_next) onto depth bucket 0xFF - clamp(depth + 0x10, 0, 0xFE):
   the lower on screen, the later it is drawn. bucket_head gets the bucket, depth the clamped value. */
void queue_sprite_at_depth(uint32_t *p_bucket_head, uint32_t *p_depth)
{
	uint32_t depth = *p_depth;
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
	*p_bucket_head = bucket_head;
	*p_depth = depth;
}

/* Pushes the current object onto the bucket in sprite_layer's low byte (the HUD and overlays). */
void queue_sprite_in_layer(void)
{
	uint32_t bucket_head = 0;

	bucket_head = sprite_buckets_ADDRESS + (OBJECT(current_object)->sprite_layer & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
}

/* Queues the current object in bucket 0, drawn last, over everything. */
void queue_sprite_on_top(void)
{
	uint8_t bucket = 0;

	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

/* Pushes the current object onto depth bucket bucket (0xFF is drawn first, under everything; 0 last). */
void queue_sprite_in_bucket(uint32_t bucket)
{
	bucket = sprite_buckets_ADDRESS + (bucket & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket)->draw_next;
	OBJECT(bucket)->draw_next = current_object;
}

/* Queues the current object at the depth of the floor under it, (projected_y + floor_z) >> 16 - view_y,
   for objects in the air; hands back the clamped depth. */
uint32_t queue_sprite_at_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
	return floor_y;
}

/* queue_sprite_at_floor with previous_floor_z. */
uint32_t queue_sprite_at_previous_floor(void)
{
	uint32_t bucket_head = 0, floor_y = 0;

	floor_y = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y - view_y);
	floor_y = (floor_y & ~0xFFFFu) | (uint16_t)(floor_y + 0x10u);
	if ((int16_t)floor_y < 0xFE)
	{
		if ((int16_t)floor_y < 0)
			floor_y = 0u;
	}
	else
		floor_y = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_y) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
	return floor_y;
}

/* Queues the current object at the depth of screen_y - sprite_offset_y, its base before the sprite
   offset; hands back the clamped depth. */
uint32_t queue_sprite_at_base(uint32_t depth)
{
	uint32_t bucket_head = 0;

	depth = (depth & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_y;
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth - OBJECT(current_object)->sprite_offset_y);
	depth = (depth & ~0xFFFFu) | (uint16_t)(depth + 0x10u);
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
	return depth;
}

/* Hands back, in bucket's low word, the bucket queue_sprite_at_depth would use for depth: 0xFF -
   clamp(depth + 0x10, 0, 0xFE). Nothing is queued. */
uint32_t depth_to_bucket(uint32_t bucket, uint16_t depth)
{
	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket = (bucket & ~0xFFFFu) | 0xFFu;
	bucket = (bucket & ~0xFFFFu) | (uint16_t)(bucket - depth);
	return bucket;
}

/* Unused: queue_sprite_at_depth with bucket_offset added to the bucket (& 0xFF): that many buckets
   further back. */
void queue_sprite_at_depth_offset(uint16_t depth, uint8_t bucket_offset)
{
	uint32_t bucket_head = 0;

	depth += 0x10u;
	if ((int16_t)depth < 0xFE)
	{
		if ((int16_t)depth < 0)
			depth = 0u;
	}
	else
		depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - depth + bucket_offset) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
}

/* A second copy of queue_sprite_at_previous_floor that hands nothing back. */
void queue_sprite_at_previous_floor_2(void)
{
	uint16_t floor_depth = 0;
	uint32_t bucket_head = 0;

	floor_depth = (uint16_t)(((int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16) - view_y + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth < 0)
			floor_depth = 0u;
	}
	else
		floor_depth = 0xFEu;
	bucket_head = sprite_buckets_ADDRESS + ((uint16_t)(0xFFu - floor_depth) & 0xFFu) * 2u;
	OBJECT(current_object)->draw_next = OBJECT(bucket_head)->draw_next;
	OBJECT(bucket_head)->draw_next = current_object;
}

/* Unused: the first half of queue_sprite_at_previous_floor_2: floor_depth = (projected_y +
   previous_floor_z) >> 16 - view_y + 0x10 clamped to 0-0xFE and bucket = 0xFF - floor_depth. Nothing is
   queued. */
void get_previous_floor_bucket(uint32_t top_bucket, uint32_t *p_floor_depth, uint32_t *p_bucket)
{
	uint32_t floor_depth = 0;

	floor_depth = (int32_t)((uint32_t)OBJECT(current_object)->projected_y + OBJECT(current_object)->previous_floor_z) >> 16;
	floor_depth = (floor_depth & ~0xFFFFu) | (uint16_t)(floor_depth - view_y);
	floor_depth = (floor_depth & ~0xFFFFu) | (uint16_t)(floor_depth + 0x10u);
	if ((int16_t)floor_depth < 0xFE)
	{
		if ((int16_t)floor_depth < 0)
			floor_depth = 0u;
	}
	else
		floor_depth = 0xFEu;
	top_bucket = (top_bucket & ~0xFFFFu) | 0xFFu;
	*p_bucket = (top_bucket & ~0xFFFFu) | (uint16_t)(top_bucket - floor_depth);
	*p_floor_depth = floor_depth;
}

/* The VBLANK hook init_sprites sets: once sprite_list_ready (which it clears), sprite_buffer_flip + 2
   and swap_sprite_buffers_now (+ 2 more): the finished list is shown and the other buffer is filled
   next. */
void swap_sprite_buffers(void)
{
	if (sprite_list_ready == 0u)
		return;
	sprite_list_ready = 0u;
	sprite_buffer_flip += 2u;
	swap_sprite_buffers_now();
}

/* sprite_buffer_flip + 2, then sprite_back_buffer_offset and the jump in sprite_list_jump from
   sprite_buffer_swaps[flip & 6]; reads sprite_status. As the hook of init_sprites_swapping_every_frame
   (+ 2 a VBLANK) it also reaches the two records the game's + 4 skips. */
void swap_sprite_buffers_now(void)
{
	uint32_t swap_entry = 0;
	uint8_t status = 0;

	sprite_buffer_flip += 2u;
	swap_entry = sprite_buffer_swaps_ADDRESS + (6u & sprite_buffer_flip) * 2u;
	sprite_back_buffer_offset = SPRITE_BUFFER_SWAP(swap_entry)->back_buffer_offset;
	mem_write16(sprite_list_jump, SPRITE_BUFFER_SWAP(swap_entry)->list_jump);
	status = mem_read8(sprite_status);
}

/* Unused: sprite_list_ready = 0, so swap_sprite_buffers skips the next swap. init_sprites_without_swaps
   makes it the vblank_hook, and then the buffers never swap. */
void cancel_sprite_buffer_swap(void)
{
	sprite_list_ready = 0u;
}

/* Unused: sprite_scale (a sized frame's graphics address) = the frame record's address plus row times
   its width >> 2 (the width halved first unless sprite_flags bit 9, 8 bits a pixel): the frame from row
   row down. */
void load_sized_frame_address_at_row(uint16_t row)
{
	uint16_t row_bytes = 0;
	uint32_t frame_record = 0;

	frame_record = LINKED(sprite_frames, OBJECT(current_object)->sprite_frame);
	row_bytes = SPRITE_FRAME_RECORD(frame_record)->width;
	frame_record += 2u;
	if ((OBJECT(current_object)->sprite_flags & 0x200u) == 0u)
		row_bytes = row_bytes >> 1;
	OBJECT(current_object)->sprite_scale = (uint32_t)(row_bytes >> 2) * row + SPRITE_FRAME_RECORD(frame_record - 0x2u)->address;
}

/* Unused: sprite_scale = the address word of the frame's record, which sized frames take as their
   graphics address. */
void load_sized_frame_address(void)
{
	OBJECT(current_object)->sprite_scale = mem_read16(LINKED(sprite_frames, OBJECT(current_object)->sprite_frame) + 6u);
}

/* Unused: sprite_width and sprite_height = the width and height of the frame's record (sprite_frames +
   sprite_frame), the size sized frames are drawn at. */
void load_frame_size(void)
{
	uint32_t frame_record = 0;

	frame_record = LINKED(sprite_frames, OBJECT(current_object)->sprite_frame);
	OBJECT(current_object)->sprite_width = SPRITE_FRAME_RECORD(frame_record)->width;
	OBJECT(current_object)->sprite_height = (uint8_t)SPRITE_FRAME_RECORD(frame_record)->height;
}

/* Writes the sprite list from sprite_list_start, emptying the depth buckets from 0xFF (drawn first) to
   0: an entry per object, by sprite_flags bits 15/14/12: a scaled or unscaled frame, the object's own 16
   bytes (clips), a jump, a sized frame, or one per part (over 16 parts: int_default). */
void build_sprite_list(uint32_t *p_height, uint32_t *p_scale_x, uint32_t *p_scale_y, uint32_t *p_sprite_flags, uint32_t *p_sprite_entry)
{
	uint32_t height = *p_height;
	uint32_t scale_x = *p_scale_x;
	uint32_t scale_y = *p_scale_y;
	uint16_t sprite_flags = (uint16_t)*p_sprite_flags;
	bool write_parts = false;
	uint8_t scale_shift = 0;
	uint16_t dest_height = 0, dest_width = 0, part_dest_width = 0, screen_x = 0, screen_y = 0, sized_width = 0, source_width = 0;
	uint32_t copy_entry = 0, flags_word = 0, frame_data = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, object = 0, part_scale = 0, parts_left = 0, rounding = 0, scaled_dx = 0, sprite_entry = 0, sprite_scale = 0;
	uint16_t part_width = 0, part_height = 0, width_bits = 0;

	sprite_entry = sprite_list_start;
	last_object = 0x20FC02u;
	frame_data = sprite_frames;
	rounding = 0x80u;
	scale_shift = 0xF8u;
	for (;;)
	{
		last_object = last_object - 2u;
		current_object = LINKED(current_object, OBJECT(last_object)->draw_next);
		if ((uint16_t)last_object == (uint16_t)current_object)
			continue;
		if ((uint16_t)last_object < (uint16_t)current_object)
			break;
		do
		{
			sprite_flags = OBJECT(current_object)->sprite_flags;
			flags_word = sprite_flags;
			flags_word = (uint16_t)((flags_word << 6) | (flags_word >> 10));
			switch (flags_word & 0x34u)
			{
			case 0x0u:
			case 0x37u:
				frame_data = LINKED(frame_data, OBJECT(current_object)->sprite_frame);
				sprite_scale = OBJECT(current_object)->sprite_scale;
				source_width = SPRITE_FRAME_RECORD(frame_data)->width;
				frame_data += 2u;
				height = (height & ~0xFFFFu) | SPRITE_FRAME_RECORD(frame_data - 0x2u)->height;
				dest_width = (uint16_t)shift_by(source_width * sprite_scale + rounding, scale_shift, 32);
				dest_height = (uint16_t)shift_by((uint8_t)height * sprite_scale + rounding, scale_shift, 32);
				source_width = (uint16_t)(((uint16_t)((uint32_t)source_width << 6) & ~0xFFu) | (uint8_t)height);
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
				frame_data += 2u;
				dest_width |= SPRITE_FRAME_RECORD(frame_data - 0x4u)->bank_bits;
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, rotate_by(source_width, scale_shift, 16));
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, dest_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, ((uint16_t)OBJECT(current_object)->screen_y | ((uint32_t)(uint16_t)OBJECT(current_object)->screen_x << 16u)));
				sprite_entry += 4u;
				frame_data += 2u;
				mem_write16(sprite_entry, SPRITE_FRAME_RECORD(frame_data - 0x6u)->address);
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				break;
			case 0x3u:
			case 0x4u:
				frame_data = LINKED(frame_data, OBJECT(current_object)->sprite_frame);
				source_width = SPRITE_FRAME_RECORD(frame_data)->width;
				dest_width = (uint16_t)source_width;
				frame_data += 2u;
				height = (height & ~0xFFFFu) | SPRITE_FRAME_RECORD(frame_data - 0x2u)->height;
				source_width = (uint16_t)(((uint16_t)((uint32_t)source_width << 6) & ~0xFFu) | (uint8_t)height);
				sprite_flags &= 0x2FFFu;
				frame_data += 2u;
				dest_width |= SPRITE_FRAME_RECORD(frame_data - 0x4u)->bank_bits;
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, rotate_by(source_width, scale_shift, 16));
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, dest_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, ((uint16_t)OBJECT(current_object)->screen_y | ((uint32_t)(uint16_t)OBJECT(current_object)->screen_x << 16u)));
				sprite_entry += 4u;
				frame_data += 2u;
				mem_write16(sprite_entry, SPRITE_FRAME_RECORD(frame_data - 0x6u)->address);
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				break;
			case 0x7u:
			case 0x8u:
			case 0x9u:
			case 0xAu:
			case 0xBu:
			case 0xCu:
			case 0xDu:
			case 0xEu:
			case 0xFu:
			case 0x10u:
			case 0x13u:
			case 0x14u:
				object = current_object + 2u;
				mem_write32(sprite_entry, (OBJECT(object - 0x2u)->sprite_flags | ((uint32_t)OBJECT(object - 0x2u)->sprite_frame << 16u)));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, (OBJECT(object - 0x6u)->sprite_palette | ((uint32_t)OBJECT(object - 0x6u)->sprite_scale << 16u)));
				sprite_entry += 4u;
				object += 4u;
				mem_write32(sprite_entry, (OBJECT(object - 0xAu)->sprite_layer | ((uint32_t)(uint16_t)OBJECT(object - 0xAu)->screen_y << 16u)));
				sprite_entry += 4u;
				mem_write32(sprite_entry, ((uint16_t)OBJECT(object - 0xAu)->screen_x | ((uint32_t)OBJECT(object - 0xAu)->hitbox_size_x << 16u)));
				sprite_entry += 4u;
				break;
			case 0x17u:
			case 0x18u:
			case 0x19u:
			case 0x1Au:
			case 0x1Bu:
			case 0x1Cu:
			case 0x1Du:
			case 0x1Eu:
			case 0x1Fu:
			case 0x20u:
				mem_write16(sprite_entry, (sprite_entry >> 4) - 0x7FFFu);
				sprite_entry += 2u;
				mem_write32(sprite_entry, ((uint16_t)OBJECT(current_object)->screen_y | ((uint32_t)(uint16_t)OBJECT(current_object)->screen_x << 16u)));
				sprite_entry += 0xEu;
				break;
			case 0x23u:
			case 0x24u:
				sprite_flags &= 0x2FFFu;
				frame_data = LINKED(frame_data, OBJECT(current_object)->sprite_frame);
				source_width = SPRITE_FRAME_RECORD(frame_data)->width;
				frame_data += 2u;
				height = (height & ~0xFFFFu) | SPRITE_FRAME_RECORD(frame_data - 0x2u)->height;
				source_width = rotate_by(((uint16_t)((uint32_t)source_width << 6) & ~0xFFu) | (uint8_t)height, scale_shift, 16);
				height &= ~0xFFu;
				height = (height & ~0xFFFFu) | (uint16_t)(height | OBJECT(current_object)->sprite_height);
				frame_data += 2u;
				sized_width = (uint16_t)(OBJECT(current_object)->sprite_width | SPRITE_FRAME_RECORD(frame_data - 0x4u)->bank_bits);
				mem_write16(sprite_entry, sprite_flags);
				sprite_entry += 2u;
				mem_write16(sprite_entry, source_width);
				sprite_entry += 2u;
				mem_write16(sprite_entry, height);
				sprite_entry += 2u;
				mem_write16(sprite_entry, sized_width);
				sprite_entry += 2u;
				mem_write32(sprite_entry, ((uint16_t)OBJECT(current_object)->screen_y | ((uint32_t)(uint16_t)OBJECT(current_object)->screen_x << 16u)));
				sprite_entry += 4u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_scale);
				sprite_entry += 2u;
				mem_write16(sprite_entry, OBJECT(current_object)->sprite_palette);
				sprite_entry += 2u;
				frame_data += 2u;
				break;
			case 0x27u:
			case 0x28u:
			case 0x29u:
			case 0x2Au:
			case 0x2Bu:
			case 0x2Cu:
			case 0x2Du:
			case 0x2Eu:
			case 0x2Fu:
			case 0x30u:
				part_scale = OBJECT(current_object)->sprite_scale;
				write_parts = true;
				break;
			case 0x33u:
			case 0x34u:
				part_scale = 0x100u;
				write_parts = true;
				break;
			default:
				invalid_jump(0x9A2E0u + (flags_word & 0x34u));
			}
			if (write_parts)
			{
				write_parts = false;
				switch ((sprite_flags >> 5) & 6u)
				{
				case 0x0u:
					scale_x = part_scale;
					scale_y = part_scale;
					break;
				case 0x2u:
					scale_x = 0u - part_scale;
					scale_y = part_scale;
					break;
				case 0x4u:
					scale_x = part_scale;
					scale_y = 0u - part_scale;
					break;
				case 0x6u:
					scale_x = 0u - part_scale;
					scale_y = 0u - part_scale;
					break;
				default:
					invalid_jump(0x9A3C4u + ((sprite_flags >> 5) & 6u));
				}
				sprite_flags &= 0xF0u;
				screen_x = (uint16_t)OBJECT(current_object)->screen_x;
				screen_y = (uint16_t)OBJECT(current_object)->screen_y;
				r14 = OBJECT(current_object)->part_list;
				parts_left = SPRITE_PART_LIST(r14)->count;
				r14 = r14 + 2u;
				if (parts_left >= 0x11u)
				{
					int_default(&ignored, &ignored_2, &ignored_3, &copy_entry);
					*p_height = copy_entry;
					*p_scale_x = scale_x;
					*p_scale_y = scale_y;
					*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
					*p_sprite_entry = sprite_entry;
					return;
				}
				do
				{
					frame_data = (frame_data & ~0xFFFFu) | SPRITE_PART(r14)->frame;
					part_width = SPRITE_FRAME_RECORD(frame_data)->width;
					frame_data += 2u;
					source_width = part_width;
					part_height = SPRITE_FRAME_RECORD(frame_data - 0x2u)->height;
					frame_data += 2u;
					height = (height & ~0xFFFFu) | part_height;
					part_dest_width = (uint16_t)shift_by(source_width * part_scale + rounding, scale_shift, 32);
					dest_height = (uint16_t)shift_by((uint8_t)height * part_scale + rounding, scale_shift, 32);
					source_width = (uint16_t)(((uint16_t)((uint32_t)source_width << 6) & ~0xFFu) | (uint8_t)height);
					height &= ~0xFFu;
					height = (height & ~0xFFFFu) | (uint16_t)(height | dest_height);
					width_bits = SPRITE_FRAME_RECORD(frame_data - 0x4u)->bank_bits;
					frame_data += 2u;
					r14 = r14 + 2u;
					mem_write16(sprite_entry, SPRITE_PART(r14 - 0x2u)->flags ^ sprite_flags);
					sprite_entry += 2u;
					mem_write16(sprite_entry, rotate_by(source_width, scale_shift, 16));
					sprite_entry += 2u;
					mem_write16(sprite_entry, height);
					sprite_entry += 2u;
					mem_write16(sprite_entry, part_dest_width | width_bits);
					sprite_entry += 2u;
					r14 = r14 + 2u;
					scaled_dx = SPRITE_PART(r14 - 0x4u)->dx * scale_x;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, shift_signed_by(SPRITE_PART(r14 - 0x5u)->dy * scale_y, scale_shift, 32) + screen_y);
					sprite_entry += 2u;
					mem_write16(sprite_entry, shift_signed_by(scaled_dx, scale_shift, 32) + screen_x);
					sprite_entry += 2u;
					mem_write16(sprite_entry, SPRITE_FRAME_RECORD(frame_data - 0x6u)->address);
					sprite_entry += 2u;
					r14 = r14 + 1u;
					mem_write16(sprite_entry, SPRITE_PART(r14 - 0x6u)->palette);
					sprite_entry += 2u;
					parts_left--;
					r14 = r14 + 2u;
				} while (parts_left != 0u);
			}
			current_object = LINKED(current_object, OBJECT(current_object)->draw_next);
		} while ((uint16_t)current_object != (uint16_t)last_object);
		OBJECT(last_object)->draw_next = last_object;
	}
	*p_height = height;
	*p_scale_x = scale_x;
	*p_scale_y = scale_y;
	*p_sprite_flags = (*p_sprite_flags & 0xFFFF0000u) | sprite_flags;
	*p_sprite_entry = sprite_entry;
}
