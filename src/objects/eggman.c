/**
 *    Name:        objects/eggman.c
 *    Description: Eggman: his booth that slides in during his speeches, the speech scripts (stages 3, 8, A), his scene with the button box in stage B and the sliding door, his teleport (never spawned)
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Spawns Eggman's booth, a two-part object at screen x 0x9F that slides in at the top of the screen
   (eggman_booth_slide_in): Eggman (palette 0x8200, his first step held 0x2F frames) and the backdrop
   (sprite 0x5718, palette 0x8210). Does nothing more when no object is free; hands back free_count and
   block. */
void spawn_eggman_booth(uint32_t spawned, uint32_t piece_count, uint32_t booth, uint32_t *p_free_count, uint32_t *p_block)
{
	uint32_t ignored = 0, object = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x012B2Fu, &spawned, &ignored, p_free_count, &booth, p_block);
	if ((uint8_t)spawned == 0u)
	{
		return;
	}
	OBJECT(booth)->sprite_flags |= 0x100u;
	OBJECT(booth)->eggman_booth.step_timer = 0x2F;
	OBJECT(booth)->sprite_palette = 0x8200u;
	OBJECT(booth)->screen_x = 0x9F;
	OBJECT(booth)->handler = eggman_booth_slide_in_ADDRESS;
	object = LINKED(booth, OBJECT(booth)->child);
	OBJECT(object)->sprite_frame = 0x5718u;
	OBJECT(object)->sprite_palette = 0x8210u;
	OBJECT(object)->screen_x = 0x9F;
	OBJECT(object)->sprite_flags |= 0x100u;
}

/* Slides the booth down 4 lines a frame (slide_y, both parts' screen_y) until slide_y reaches 0x2C, then
   sets it to 0x2F and starts eggman_booth_animate at step 0. */
void eggman_booth_slide_in(void)
{
	OBJECT(current_object)->eggman_booth.slide_y += 4u;
	if (OBJECT(current_object)->eggman_booth.slide_y < 0x2C)
	{
		draw_eggman_booth();
		return;
	}
	OBJECT(current_object)->eggman_booth.slide_y = 0x2F;
	OBJECT(current_object)->handler = eggman_booth_animate_ADDRESS;
	OBJECT(current_object)->eggman_booth.step = 0u;
	eggman_booth_animate();
}

/* Plays the booth's 0x1D steps, each frame of eggman_booth_steps held for its
   eggman_booth_frame_durations (step_timer), then slides the booth back up 4 lines a frame and deletes
   it with its parts once slide_y goes below 0. Drawn by draw_eggman_booth. */
void eggman_booth_animate(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0;
	uint16_t slide_y = 0;

	if (OBJECT(current_object)->eggman_booth.step != 0x1Du)
	{
		OBJECT(current_object)->eggman_booth.step_timer--;
		if (OBJECT(current_object)->eggman_booth.step_timer >= 0)
		{
			draw_eggman_booth();
			return;
		}
		OBJECT(current_object)->eggman_booth.step++;
		OBJECT(current_object)->eggman_booth.step_timer = eggman_booth_frame_durations[eggman_booth_steps[OBJECT(current_object)->eggman_booth.step]];
		draw_eggman_booth();
		return;
	}
	slide_y = OBJECT(current_object)->eggman_booth.slide_y;
	OBJECT(current_object)->eggman_booth.slide_y = slide_y - 4u;
	if (slide_y >= 4u)
	{
		draw_eggman_booth();
		return;
	}
	delete_object_and_children(&ignored, &ignored_2, &ignored_3);
}

/* Draws the booth's two parts at screen_y = slide_y in bucket 0, on top: Eggman with the frame
   eggman_booth_steps gives for step (eggman_booth_frames), then the backdrop. Plays sound 0x41 on every
   frame that shows frame 0xB, except in stages 0 and 5. */
void draw_eggman_booth(void)
{
	uint8_t bucket = 0, sound_id = 0;
	uint32_t booth_frame = 0, ignored = 0, ignored_2 = 0;

	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	booth_frame = eggman_booth_steps[OBJECT(current_object)->draw_eggman_booth.step];
	OBJECT(current_object)->sprite_frame = eggman_booth_frames[booth_frame];
	if (stage != 0u && stage != 5u && booth_frame == 0xBu)
	{
		sound_id = 0x41u;
		play_sound(sound_id, &ignored, &ignored_2);
	}
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->screen_y = OBJECT(object_cursor)->draw_eggman_booth.slide_y;
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
}

/* Unused: spawns a two-part object (spawn_object_with_pieces) and gives it eggman_teleport_init, without
   testing whether one was spawned. */
void spawn_eggman_teleport(uint32_t free_count, uint32_t teleport, uint32_t block)
{
	uint32_t ignored = 0, ignored_2 = 0;

	spawn_count = 2u;
	spawn_object_with_pieces(0x013434u, &ignored, &ignored_2, &free_count, &teleport, &block);
	OBJECT(teleport)->handler = eggman_teleport_init_ADDRESS;
}

/* Sets up Eggman's teleport: sprite_scale 0x180, palette 0x8220, its part a tall beam (sprite 0x79A8),
   then eggman_teleport_animate. Only spawn_eggman_teleport, which nothing calls, spawns it. */
void eggman_teleport_init(void)
{
	OBJECT(current_object)->sprite_scale = 0x180u;
	OBJECT(current_object)->sprite_flags |= 0x100u;
	OBJECT(current_object)->sprite_palette = 0x8220u;
	OBJECT(current_object)->handler = eggman_teleport_animate_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0x79A8u;
}

/* Steps Eggman through eggman_teleport_steps, each frame of eggman_teleport_frames held its
   eggman_teleport_durations: he builds up in stripes, stands, works a remote and fades out; step 0xFF
   deletes him and the beam. The beam part is drawn every other frame at his floor, 8 to the right;
   Eggman 3 frames out of 4. */
void eggman_teleport_animate(void)
{
	uint32_t depth = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint16_t step_timer = 0, odd_frame = 0, flicker_step = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	step_timer = OBJECT(current_object)->eggman_teleport.step_timer;
	OBJECT(current_object)->eggman_teleport.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		frame_index = eggman_teleport_steps[OBJECT(current_object)->eggman_teleport.step];
		OBJECT(current_object)->eggman_teleport.frame_index = frame_index;
		if (frame_index == 0xFFu)
		{
			delete_object_and_children(&ignored, &ignored_2, &ignored_3);
			return;
		}
		OBJECT(current_object)->sprite_frame = eggman_teleport_frames[frame_index];
		OBJECT(current_object)->eggman_teleport.step_timer = eggman_teleport_durations[frame_index];
		OBJECT(current_object)->eggman_teleport.step++;
	}
	OBJECT(current_object)->eggman_teleport.beam_phase++;
	odd_frame = OBJECT(current_object)->eggman_teleport.beam_phase & 1u;
	OBJECT(current_object)->eggman_teleport.beam_phase = odd_frame;
	if (odd_frame != 0u)
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		place_sprite_at_cursor_floor();
		OBJECT(current_object)->sprite_palette = 0u;
		OBJECT(current_object)->screen_x += 8u;
		depth = (uint16_t)(OBJECT(current_object)->screen_y - 2u);
		queue_sprite_at_depth(&ignored, &depth);
		current_object = object_cursor;
	}
	OBJECT(current_object)->eggman_teleport.flicker_phase++;
	flicker_step = OBJECT(current_object)->eggman_teleport.flicker_phase & 3u;
	OBJECT(current_object)->eggman_teleport.flicker_phase = flicker_step;
	if (flicker_step != 0u)
		queue_sprite_at_floor();
}

/* Sets up Eggman's scene in stage B: Eggman (sprite 0x88A8, palette 0x81F0, step_timer 0x3C) with
   eggman_scene_wait, and his shadow part (sprite 0xAA50; the palette 0x250 it writes is at once replaced
   by 0x9FFE). Spawns the button box at his position, projected_x - 0x1E and projected_y + 0x12, with
   button_box_init. */
void eggman_scene_init(void)
{
	uint32_t block = 0, free_count = 0, ignored = 0, ignored_2 = 0, object = 0;

	OBJECT(current_object)->sprite_frame = 0x88A8u;
	OBJECT(current_object)->sprite_palette = 0x81F0u;
	OBJECT(current_object)->sprite_flags |= 0x100u;
	OBJECT(current_object)->eggman_scene.step_timer = 0x3Cu;
	OBJECT(current_object)->handler = eggman_scene_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = 0xAA50u;
	OBJECT(current_object)->sprite_palette = 0x250u;
	OBJECT(current_object)->sprite_palette = 0x9FFEu;
	spawn_count = 1u;
	spawn_object_with_pieces(0x01409Cu, &ignored, &ignored_2, &free_count, &object, &block);
	copy_position_to(object);
	OBJECT(object)->projected_x -= 0x1Eu;
	OBJECT(object)->projected_y_int += 0x12u;
	OBJECT(object)->handler = button_box_init_ADDRESS;
}

/* Draws Eggman until scroll_position passes 0xFC0, then sets scroll_mode 2 (wait) and stage_trigger_bits
   to 1, which the players' stage B sequence waits for, and starts eggman_scene_pause. */
void eggman_scene_wait(void)
{
	if ((int16_t)scroll_position <= 0xFC0)
	{
		eggman_scene_move();
		return;
	}
	scroll_mode = 2u;
	stage_trigger_bits = 1u;
	OBJECT(current_object)->handler = eggman_scene_pause_ADDRESS;
	eggman_scene_pause();
}

/* Draws Eggman for step_timer frames (0x3C from eggman_scene_init), then sets eggman_scene.phase to 1, a
   value nothing tests, and starts eggman_scene_talk. */
void eggman_scene_pause(void)
{
	uint16_t step_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.step_timer = 0u;
	OBJECT(current_object)->eggman_scene.phase = 1u;
	OBJECT(current_object)->handler = eggman_scene_talk_ADDRESS;
	eggman_scene_talk();
}

/* Eggman talks: each time step_timer runs out, the next of three message boxes at (0xA0, 0x80), kind
   eggman_scene_texts[direction] for eggman_scene_text_durations[direction] frames, with sound
   eggman_scene_sounds[direction] (direction counts them); a new talking frame every 7 frames. After the
   third, eggman_scene_press_button. */
void eggman_scene_talk(void)
{
	uint8_t sound_id = 0;
	uint16_t kind = 0, screen_x = 0, screen_y = 0, text_duration = 0;
	uint32_t direction = 0, ignored = 0, ignored_2 = 0;
	uint16_t step_timer = 0, talk_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		talk_timer = OBJECT(current_object)->eggman_scene.talk_timer;
		OBJECT(current_object)->eggman_scene.talk_timer = talk_timer - 1u;
		if (talk_timer != 0u)
		{
			eggman_scene_move();
			return;
		}
		OBJECT(current_object)->eggman_scene.talk_timer = 6u;
		OBJECT(current_object)->sprite_frame = eggman_talk_frames[eggman_talk_steps[(uint8_t)OBJECT(current_object)->eggman_scene.step]];
		OBJECT(current_object)->eggman_scene.step++;
		OBJECT(current_object)->eggman_scene.step = (uint8_t)OBJECT(current_object)->eggman_scene.step % 0xAu;
		eggman_scene_move();
		return;
	}
	if ((int16_t)OBJECT(current_object)->direction > 2)
	{
		OBJECT(current_object)->eggman_scene.step = 0;
		OBJECT(current_object)->eggman_scene.step_timer = 0u;
		OBJECT(current_object)->handler = eggman_scene_press_button_ADDRESS;
		eggman_scene_press_button();
		return;
	}
	direction = OBJECT(current_object)->direction;
	screen_x = 0xA0u;
	screen_y = 0x80u;
	kind = eggman_scene_texts[direction];
	text_duration = eggman_scene_text_durations[direction];
	OBJECT(current_object)->eggman_scene.step_timer = text_duration;
	spawn_message_box(screen_x, screen_y, kind, text_duration, 0u);
	sound_id = (uint8_t)eggman_scene_sounds[OBJECT(current_object)->direction];
	play_sound(sound_id, &ignored, &ignored_2);
	OBJECT(current_object)->direction++;
	talk_timer = OBJECT(current_object)->eggman_scene.talk_timer;
	OBJECT(current_object)->eggman_scene.talk_timer = talk_timer - 1u;
	if (talk_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.talk_timer = 6u;
	OBJECT(current_object)->sprite_frame = eggman_talk_frames[eggman_talk_steps[(uint8_t)OBJECT(current_object)->eggman_scene.step]];
	OBJECT(current_object)->eggman_scene.step++;
	OBJECT(current_object)->eggman_scene.step = (uint8_t)OBJECT(current_object)->eggman_scene.step % 0xAu;
	eggman_scene_move();
}

/* Plays Eggman's 7 button-pressing frames (eggman_press_frames, each held its eggman_press_durations).
   When the fifth shows, eggman_scene.phase = 2 (the box's button goes down) and escape_started = 1;
   after the last, eggman_scene_run_off with a 0x3C-frame wait. */
void eggman_scene_press_button(void)
{
	uint32_t step = 0;
	uint16_t step_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	step = (uint8_t)OBJECT(current_object)->eggman_scene.step;
	OBJECT(current_object)->eggman_scene.step_timer = eggman_press_durations[step];
	OBJECT(current_object)->sprite_frame = eggman_press_frames[step];
	OBJECT(current_object)->eggman_scene.step++;
	if ((uint8_t)OBJECT(current_object)->eggman_scene.step == 5u)
	{
		OBJECT(current_object)->eggman_scene.phase = 2u;
		escape_started = 1u;
	}
	if (OBJECT(current_object)->eggman_scene.step <= 6)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.step = 0;
	OBJECT(current_object)->eggman_scene.step_timer = 0x3Cu;
	OBJECT(current_object)->handler = eggman_scene_run_off_ADDRESS;
	eggman_scene_run_off();
}

/* Waits step_timer frames, then runs: every 3 frames the next of the 8 eggman_run_frames, with
   direction_to_speed(0x20, angle 0x2A0) added to speed_x and speed_y each time, so he speeds up as he
   goes. */
void eggman_scene_run_off(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;
	uint16_t step_timer = 0;

	step_timer = OBJECT(current_object)->eggman_scene.step_timer;
	OBJECT(current_object)->eggman_scene.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		eggman_scene_move();
		return;
	}
	OBJECT(current_object)->eggman_scene.step_timer = 2u;
	OBJECT(current_object)->sprite_frame = eggman_run_frames[(uint8_t)OBJECT(current_object)->eggman_scene.step];
	OBJECT(current_object)->eggman_scene.step++;
	OBJECT(current_object)->eggman_scene.step = (uint8_t)OBJECT(current_object)->eggman_scene.step % 8u;
	direction = 0x2A0u;
	speed = 0x20u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x += speed_x;
	OBJECT(current_object)->speed_y += speed_y;
	eggman_scene_move();
}

/* Moves Eggman (move_object_projected), lets him fall to his floor and queues him in bucket 0. Once
   delete_if_off_screen removes him it sets scroll_mode 7 (follow the players) and clears
   stage_trigger_bits; until then his shadow is drawn every other frame at his floor, 4 lower, in bucket
   0xFF. */
void eggman_scene_move(void)
{
	uint8_t bucket = 0;
	uint32_t bucket_2 = 0, deleted = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;
	uint16_t direction = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	fall_to_floor(&ignored, &ignored_5, &ignored_2);
	bucket = 0u;
	queue_sprite_in_bucket(bucket);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
	{
		scroll_mode = 7u;
		stage_trigger_bits = 0u;
		return;
	}
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction == 0u)
	{
		OBJECT(current_object)->direction = 1u;
		bucket_2 = place_sprite_at_cursor_floor();
		OBJECT(current_object)->screen_y += 4u;
		queue_sprite_in_bucket(bucket_2);
	}
}

/* Sets up the button box Eggman spawns in his stage B scene: sprite 0x8928, palette 0x11D0, step_timer
   0xB4 (its wait before it rises), then button_box_wait. */
void button_box_init(void)
{
	OBJECT(current_object)->button_box.step_timer = 0xB4u;
	OBJECT(current_object)->sprite_frame = 0x8928u;
	OBJECT(current_object)->sprite_palette = 0x11D0u;
	OBJECT(current_object)->handler = button_box_wait_ADDRESS;
}

/* Draws the box until its parent, Eggman, has eggman_scene.step 1, which his first talking frame sets;
   then button_box_rise. */
void button_box_wait(void)
{
	if ((uint8_t)OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->eggman_scene.step != 1u)
	{
		draw_button_box();
		return;
	}
	OBJECT(current_object)->handler = button_box_rise_ADDRESS;
	button_box_rise();
}

/* Waits out step_timer, then plays the box's steps 0 to 0x12 (button_box_steps, button_box_durations,
   button_box_frames): it rises out of the floor and its lid opens on a lit button. Then it waits for
   Eggman's eggman_scene.phase to be 2, when he presses it, and goes on to button_box_press. */
void button_box_rise(void)
{
	uint32_t box_frame = 0, step = 0;
	uint16_t step_timer = 0;

	if (OBJECT(current_object)->button_box.step != 0x13u)
	{
		step_timer = OBJECT(current_object)->button_box.step_timer;
		OBJECT(current_object)->button_box.step_timer = step_timer - 1u;
		if (step_timer != 0u)
		{
			draw_button_box();
			return;
		}
		step = OBJECT(current_object)->button_box.step;
		box_frame = button_box_steps[step];
		OBJECT(current_object)->button_box.step_timer = button_box_durations[step];
		OBJECT(current_object)->sprite_frame = button_box_frames[box_frame];
		OBJECT(current_object)->button_box.step++;
		draw_button_box();
		return;
	}
	if (OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->eggman_scene.phase != 2u)
	{
		draw_button_box();
		return;
	}
	OBJECT(current_object)->handler = button_box_press_ADDRESS;
	button_box_press();
}

/* Plays the box's steps 0x13 to 0x22 (button_box_steps, each frame of button_box_frames held its
   button_box_durations): the button held down, the lid closing and the box sinking back into the floor.
   At step 0x23 it leaves the box to draw_button_box. */
void button_box_press(void)
{
	uint32_t box_frame = 0, step = 0;
	uint16_t step_timer = 0;

	if (OBJECT(current_object)->button_box.step == 0x23u)
	{
		OBJECT(current_object)->handler = draw_button_box_ADDRESS;
		draw_button_box();
		return;
	}
	step_timer = OBJECT(current_object)->button_box.step_timer;
	OBJECT(current_object)->button_box.step_timer = step_timer - 1u;
	if (step_timer != 0u)
	{
		draw_button_box();
		return;
	}
	step = OBJECT(current_object)->button_box.step;
	box_frame = button_box_steps[step];
	OBJECT(current_object)->button_box.step_timer = button_box_durations[step];
	OBJECT(current_object)->sprite_frame = button_box_frames[box_frame];
	OBJECT(current_object)->button_box.step++;
	draw_button_box();
}

/* Places the box on the screen from its projected position (unproject_object), deletes it once it is off
   screen and queues it in bucket 0xFF, under everything, even on the frame it is deleted. */
void draw_button_box(void)
{
	uint8_t bucket = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	unproject_object(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&ignored, &ignored_2, &ignored_3);
	bucket = 0xFFu;
	queue_sprite_in_bucket(bucket);
}

/* Sets up stage B's sliding door: the main object is a sprite clip entry (sprite_flags 0x5000) with a
   box 0xE6 x 0x21, height 0x100, kind 0xC; part 2 is the door (sprite 0x7830, palette 0x9070), part 3 a
   second clip entry. open_delay 0xA, then sliding_door_wait. */
void sliding_door_init(void)
{
	OBJECT(current_object)->sprite_flags |= 0x4000u;
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
	OBJECT(current_object)->sprite_palette = 0u;
	OBJECT(current_object)->sprite_scale = 0x7FFu;
	OBJECT(current_object)->flags |= 2u;
	OBJECT(current_object)->hitbox_size_x = 0xE6u;
	OBJECT(current_object)->hitbox_size_y = 0x21u;
	OBJECT(current_object)->hitbox_offset_x = 9;
	OBJECT(current_object)->hitbox_offset_y = -0xA;
	OBJECT(current_object)->hitbox_height = 0x100u;
	OBJECT(current_object)->kind = 0xCu;
	OBJECT(current_object)->sliding_door.open_delay = 0xAu;
	OBJECT(current_object)->handler = sliding_door_wait_ADDRESS;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 6u;
	OBJECT(current_object)->sprite_frame = 0x7830u;
	OBJECT(current_object)->sprite_palette = 0x9070u;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_flags = 0x5000u;
	OBJECT(current_object)->sprite_frame = 0x7FFu;
}

/* Waits until every playing player (player_status negative) has stage_b_path_done set, with at least one
   playing; then counts down open_delay (0xA) while that holds and starts sliding_door_open. Drawn by
   sliding_door_draw. */
void sliding_door_wait(void)
{
	uint8_t done_players = 0, ready_players = 0;
	uint32_t player_base = 0, players_left = 0, status_cursor = 0;
	uint8_t status = 0;
	uint16_t open_delay = 0;

	status_cursor = player_status_ADDRESS;
	player_base = PLAYER_BLOCK(1);
	players_left = 3u;
	ready_players = 0u;
	done_players = 0u;
	do
	{
		status = mem_read8(status_cursor);
		status_cursor++;
		if ((int8_t)status >= 0)
			ready_players++;
		else if (PLAYER(player_base)->stage_b_path_done != 0u)
		{
			done_players++;
			ready_players++;
		}
		player_base += 0x200u;
		players_left--;
	} while (players_left != 0u);
	if (ready_players == 3u && done_players != 0u)
	{
		open_delay = OBJECT(current_object)->sliding_door.open_delay;
		OBJECT(current_object)->sliding_door.open_delay = open_delay - 1u;
		if (open_delay == 0u)
		{
			OBJECT(current_object)->handler = sliding_door_open_ADDRESS;
			sliding_door_open();
			return;
		}
	}
	sliding_door_draw();
}

/* Slides the door along angle 0xB4 at speed 0x100 until its projected_x reaches 0x3EF, then stops it,
   sets sliding_door_opened to 0xFF and leaves it to sliding_door_draw. */
void sliding_door_open(void)
{
	uint32_t direction = 0, speed = 0, speed_x = 0, speed_y = 0;

	direction = 0xB4u;
	speed = 0x100u;
	direction_to_speed(speed, &direction, &speed_x, &speed_y);
	OBJECT(current_object)->speed_x = speed_x;
	OBJECT(current_object)->speed_y = speed_y;
	if (OBJECT(current_object)->projected_x < 0x3EF)
	{
		sliding_door_draw();
		return;
	}
	OBJECT(current_object)->speed_x = 0;
	OBJECT(current_object)->speed_y = 0;
	sliding_door_opened = 0xFFu;
	OBJECT(current_object)->handler = sliding_door_draw_ADDRESS;
	sliding_door_draw();
}

/* Moves the door (move_object_projected) and deletes it once off screen; else adds it to the touchable
   list and queues its three parts at screen_y - 0x50: the clip entry, the door sprite at its screen
   position, and the second clip entry, whose edges (sprite_palette, sprite_scale) become screen_x and
   0x3FF - projected_x + screen_x. */
void sliding_door_draw(void)
{
	uint16_t offset_x = 0, offset_y = 0;
	uint32_t deleted = 0, depth = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	move_object_projected(&ignored, &ignored_2, &ignored_3, &ignored_4);
	delete_if_off_screen(&deleted, &ignored_2, &ignored_3);
	if ((uint8_t)deleted != 0u)
		return;
	depth = (uint16_t)(OBJECT(current_object)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
	add_to_touchable_list(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	offset_x = 0u;
	offset_y = 0u;
	place_sprite_near_cursor(offset_x, offset_y);
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_palette = OBJECT(object_cursor)->screen_x;
	OBJECT(current_object)->sprite_scale = 0x3FFu - OBJECT(object_cursor)->projected_x + OBJECT(object_cursor)->screen_x;
	depth = (uint16_t)(OBJECT(object_cursor)->screen_y - 0x50u);
	queue_sprite_at_depth(&ignored, &depth);
}

/* Starts one of Eggman's speeches: script = the eggman_speech_scripts record for the object's
   sprite_frame (stage 3 uses 1, stage 8 0, stage A 3), frame_count 0, then eggman_speech_run. */
void eggman_speech_start(void)
{
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	OBJECT(current_object)->eggman_speech.script = eggman_speech_scripts[OBJECT(current_object)->sprite_frame] + eggman_speech_scripts_ADDRESS;
	OBJECT(current_object)->handler = eggman_speech_run_ADDRESS;
	eggman_speech_run();
}

/* Runs a speech script, except while current_game_mode is 0x11 (the screen wipe): when frame_count
   reaches the record's delay it runs its command: 1 Eggman's booth (spawn_eggman_booth), 2 a message box
   (x, y, kind, frames), 3 a sound, each moving script on and restarting frame_count; 7 waits there for
   good; 0 and 4 delete the object. */
void eggman_speech_run(void)
{
	uint8_t sound_id = 0;
	uint16_t frames = 0, kind = 0, screen_x = 0, screen_y = 0;
	uint32_t block = 0, command = 0, free_count = 0, ignored = 0, ignored_2 = 0, script_cursor = 0;
	uint16_t delay = 0;

	if (current_game_mode == 0x11u)
		return;
	OBJECT(current_object)->eggman_speech.frame_count++;
	script_cursor = OBJECT(current_object)->eggman_speech.script;
	delay = EGGMAN_SPEECH_STEP(script_cursor)->delay;
	script_cursor += 2u;
	if (OBJECT(current_object)->eggman_speech.frame_count < (int16_t)delay)
		return;
	command = EGGMAN_SPEECH_STEP(script_cursor - 0x2u)->command * 2u;
	script_cursor += 2u;
	switch (command)
	{
	case 0x0u:
	case 0x8u:
		delete_current_object();
		return;
	case 0x2u:
		OBJECT(current_object)->eggman_speech.script = script_cursor;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		spawn_eggman_booth(command, 0u, 0u, &free_count, &block);
		return;
	case 0x4u:
		break;
	case 0x6u:
		sound_id = (uint8_t)EGGMAN_SPEECH_ARGUMENTS(script_cursor)[0];
		OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
		OBJECT(current_object)->eggman_speech.frame_count = 0;
		play_sound(sound_id, &ignored, &ignored_2);
		return;
	case 0xEu:
		return;
	default:
		invalid_jump(0x85794u + command);
	}
	screen_x = EGGMAN_SPEECH_ARGUMENTS(script_cursor)[0];
	script_cursor += 2u;
	screen_y = EGGMAN_SPEECH_ARGUMENTS(script_cursor)[0];
	script_cursor += 2u;
	kind = EGGMAN_SPEECH_ARGUMENTS(script_cursor)[0];
	script_cursor += 2u;
	frames = EGGMAN_SPEECH_ARGUMENTS(script_cursor)[0];
	OBJECT(current_object)->eggman_speech.script = script_cursor + 2u;
	OBJECT(current_object)->eggman_speech.frame_count = 0;
	block = spawn_message_box(screen_x, screen_y, kind, frames, 0u);
}
