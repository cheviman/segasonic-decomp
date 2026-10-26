/**
 *    Name:        system/main.c
 *    Description: the machine's main program: power-on and boot, the frame loop, the attract mode and the system modes, the frame wait and the controls, the interrupt handlers, the reset vector
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    26-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Power-on: the vector table base, eight byte pairs to 0xE00000 (unmapped on this board) and the
   interrupt controller's set-up to 0xD00000, the privileged stack pointers 0x20FE00, a delay; then for
   ever: init_system, frames (wait_for_frame, run_frame) until test_mode is set, test mode while it
   lasts, then over again from system mode 3, the title. */
void boot(void)
{
	uint32_t bytes_left = 0, count = 0, cursor = 0, dest = 0, height = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, input_cursor = 0, pairs_left = 0, pressed = 0, released = 0, scale = 0, source = 0, trackball_cursor = 0, trackball_port = 0;
	uint32_t initial_psw = 0;
	uint32_t psw = 0;

	load_privileged(5u, vector_table_ADDRESS);
	source = 0x7Eu;
	dest = 0xE00000u;
	pairs_left = 8u;
	do
	{
		mem_write8(dest, BOOT_REGISTER_VALUES(source)[0]);
		dest++;
		source++;
		mem_write8(dest, BOOT_REGISTER_VALUES(source)[0]);
		dest++;
		pairs_left--;
		source++;
	} while (pairs_left != 0u);
	dest = irq_vectors;
	bytes_left = 0xCu;
	do
	{
		mem_write8(dest, BOOT_REGISTER_VALUES(source)[0]);
		dest++;
		bytes_left--;
		source++;
	} while (bytes_left != 0u);
	initial_psw = 0u;
	psw = initial_psw;
	psw_write(psw);
	load_privileged(2u, 0x20FE00u);
	load_privileged(3u, 0x20FE00u);
	load_privileged(4u, 0x20FE00u);
	timer_0_ticks = 0u;
	timer_1_ticks = 0u;
	system_mode_and_step = 0u;
	current_screen = 0u;
	delay_after_reset();
	for (;;)
	{
		init_system(&scale, &cursor);
		for (;;)
		{
			wait_for_frame(&pressed, &released, &input_cursor, &count, &trackball_port, &trackball_cursor);
			if (test_mode != 0u)
				break;
			run_frame(pressed, released, input_cursor, count, trackball_port, trackball_cursor, &scale, &cursor, &height);
		}
		start_test_mode(&scale, &height);
		do
		{
			wait_for_frame(&pressed, &ignored, &ignored_2, &count, &ignored_3, &ignored_4);
			run_test_mode(pressed, (uint16_t)count, &scale, &height);
		} while (test_mode != 0u);
		system_mode_and_step = 0x300u;
	}
}

/* A frame of boot's loop: frame_counter + 1; outside system modes 0 and 6 the attract credits, and a
   start (a player's button with setting_no_start_buttons) pressed with enough credits for cabinet_config
   starts the game (run_frame_start_game; configs 2 and 3 through run_frame_check_p2_start); else
   system_mode's handler, run_objects, build_sprite_list, draw_queued_strings. */
void run_frame(uint32_t pressed, uint32_t released, uint32_t input_cursor, uint32_t count, uint32_t trackball_port, uint32_t trackball_cursor, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t credits_needed = 0, message_text = 0, sprite_entry = 0, start_bit = 0, text_position = 0;

	frame_counter++;
	if (system_mode == 0u || system_mode == 6u)
	{
		switch (system_mode)
		{
		case 0:
			attract_warning(p_scale, &trackball_port);
			break;
		case 1:
			system_mode_idle();
			break;
		case 2:
			attract_ranking(p_scale, &trackball_port);
			break;
		case 3:
			attract_title(p_scale, &count, p_height, &trackball_port);
			break;
		case 4:
			attract_demo_1(pressed, released, input_cursor, (uint16_t)*p_cursor, trackball_cursor, p_scale, &count, p_height, &trackball_port);
			break;
		case 5:
			attract_demo_2(pressed, released, input_cursor, (uint16_t)*p_cursor, trackball_cursor, p_scale, &count, p_height, &trackball_port);
			break;
		case 6:
			run_game_mode(pressed, released, input_cursor, (uint16_t)*p_cursor, trackball_cursor, p_scale, &count, p_height, &trackball_port);
			break;
		case 7:
			wait_attract_timer();
			break;
		default:
			invalid_jump(system_mode_handlers[system_mode]);
		}
		run_objects();
		build_sprite_list(p_scale, &count, p_height, &trackball_port, &sprite_entry);
		draw_queued_strings((uint16_t)count, (uint16_t)trackball_port, sprite_entry, p_scale, p_cursor, p_height);
		return;
	}
	draw_attract_credits(&pressed, &released, &message_text, &text_position);
	credits_needed = (input_cursor & ~0xFFFFu) | credits_per_start;
	if (free_play != 0u)
		credits_needed &= ~0xFFFFu;
	switch (cabinet_config)
	{
	case 0:
	case 1:
	case 4:
		if (p1_credits < (uint8_t)credits_needed)
		{
			switch (system_mode)
			{
			case 0:
				attract_warning(p_scale, &message_text);
				break;
			case 1:
				system_mode_idle();
				break;
			case 2:
				attract_ranking(p_scale, &message_text);
				break;
			case 3:
				attract_title(p_scale, &count, p_height, &message_text);
				break;
			case 4:
				attract_demo_1(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
				break;
			case 5:
				attract_demo_2(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
				break;
			case 6:
				run_game_mode(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
				break;
			case 7:
				wait_attract_timer();
				break;
			default:
				invalid_jump(system_mode_handlers[system_mode]);
			}
			run_objects();
			build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
			draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, p_cursor, p_height);
			return;
		}
		if (setting_no_start_buttons != 0u)
		{
			if ((inputs_pressed & 0x10000u) != 0u || (inputs_pressed & 0x1000000u) != 0u)
			{
				run_frame_start_game(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
				return;
			}
		}
		else if ((inputs_pressed & 0x10u) != 0u || (inputs_pressed & 0x20u) != 0u)
		{
			run_frame_start_game(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
			return;
		}
		if ((cabinet_config & 1u) == 0u)
		{
			switch (system_mode)
			{
			case 0:
				attract_warning(p_scale, &message_text);
				break;
			case 1:
				system_mode_idle();
				break;
			case 2:
				attract_ranking(p_scale, &message_text);
				break;
			case 3:
				attract_title(p_scale, &count, p_height, &message_text);
				break;
			case 4:
				attract_demo_1(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
				break;
			case 5:
				attract_demo_2(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
				break;
			case 6:
				run_game_mode(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
				break;
			case 7:
				wait_attract_timer();
				break;
			default:
				invalid_jump(system_mode_handlers[system_mode]);
			}
			run_objects();
			build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
			draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, p_cursor, p_height);
			return;
		}
		start_bit = 0x12u;
		if (setting_no_start_buttons == 0u)
			start_bit = 7u;
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) != 0u)
		{
			run_frame_start_game(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
			return;
		}
		switch (system_mode)
		{
		case 0:
			attract_warning(p_scale, &message_text);
			break;
		case 1:
			system_mode_idle();
			break;
		case 2:
			attract_ranking(p_scale, &message_text);
			break;
		case 3:
			attract_title(p_scale, &count, p_height, &message_text);
			break;
		case 4:
			attract_demo_1(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 5:
			attract_demo_2(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 6:
			run_game_mode(pressed, released, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 7:
			wait_attract_timer();
			break;
		default:
			invalid_jump(system_mode_handlers[system_mode]);
		}
		run_objects();
		build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
		draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, p_cursor, p_height);
		return;
	case 2:
		run_frame_check_p2_start(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
		return;
	case 3:
		if (p3_credits < (uint8_t)credits_needed)
		{
			run_frame_check_p2_start(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
			return;
		}
		start_bit = 0x12u;
		if (setting_no_start_buttons == 0u)
			start_bit = 7u;
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) != 0u)
		{
			run_frame_start_game(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
			return;
		}
		run_frame_check_p2_start(pressed, released, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
		return;
	default:
		invalid_jump(0x115u + cabinet_config * 3u);
	}
}

/* The start check of run_frame for the individual coin chutes (cabinet_config 2, and 3 after player 3's
   start): player 2's start (bit 5; player 2's button, bit 0x18, with setting_no_start_buttons) with
   enough p2_credits, or player 1's (bit 4 or 0x10) with p1_credits, starts the game
   (run_frame_start_game); else the system mode's handler and the rest of the frame. */
void run_frame_check_p2_start(uint32_t credits, uint32_t blink_bit, uint32_t credits_needed, uint32_t count, uint32_t message_text, uint32_t text_position, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint32_t sprite_entry = 0, start_bit = 0;

	if ((uint8_t)credits_needed <= p2_credits)
	{
		start_bit = 0x18u;
		if (setting_no_start_buttons == 0u)
			start_bit = 5u;
		if ((inputs_pressed & shl32(1u, (int)(start_bit))) != 0u)
		{
			run_frame_start_game(credits, blink_bit, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
			return;
		}
	}
	if (p1_credits < (uint8_t)credits_needed)
	{
		switch (system_mode)
		{
		case 0:
			attract_warning(p_scale, &message_text);
			break;
		case 1:
			system_mode_idle();
			break;
		case 2:
			attract_ranking(p_scale, &message_text);
			break;
		case 3:
			attract_title(p_scale, &count, p_height, &message_text);
			break;
		case 4:
			attract_demo_1(credits, blink_bit, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 5:
			attract_demo_2(credits, blink_bit, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 6:
			run_game_mode(credits, blink_bit, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 7:
			wait_attract_timer();
			break;
		default:
			invalid_jump(system_mode_handlers[system_mode]);
		}
		run_objects();
		build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
		draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, p_cursor, p_height);
		return;
	}
	start_bit = 0x10u;
	if (setting_no_start_buttons == 0u)
		start_bit = 4u;
	if ((inputs_pressed & shl32(1u, (int)(start_bit))) == 0u)
	{
		switch (system_mode)
		{
		case 0:
			attract_warning(p_scale, &message_text);
			break;
		case 1:
			system_mode_idle();
			break;
		case 2:
			attract_ranking(p_scale, &message_text);
			break;
		case 3:
			attract_title(p_scale, &count, p_height, &message_text);
			break;
		case 4:
			attract_demo_1(credits, blink_bit, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 5:
			attract_demo_2(credits, blink_bit, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 6:
			run_game_mode(credits, blink_bit, credits_needed, (uint16_t)*p_cursor, text_position, p_scale, &count, p_height, &message_text);
			break;
		case 7:
			wait_attract_timer();
			break;
		default:
			invalid_jump(system_mode_handlers[system_mode]);
		}
		run_objects();
		build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
		draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, p_cursor, p_height);
		return;
	}
	run_frame_start_game(credits, blink_bit, credits_needed, count, message_text, text_position, p_scale, p_cursor, p_height);
}

/* A start pressed with credits: system_mode 6 (the game), game_mode 0, system_mode_step 0,
   game_in_progress 0xFF; then the rest of the frame: run_game_mode, run_objects, build_sprite_list,
   draw_queued_strings. */
void run_frame_start_game(uint32_t credits, uint32_t blink_bit, uint32_t credits_needed, uint32_t count, uint32_t message_text, uint32_t text_position, uint32_t *p_scale, uint32_t *p_cursor, uint32_t *p_height)
{
	uint16_t cursor = (uint16_t)*p_cursor;
	uint32_t sprite_entry = 0, string_cursor = 0;

	system_mode = 6u;
	game_mode = 0u;
	system_mode_step = 0u;
	game_in_progress = 0xFFu;
	switch (system_mode)
	{
	case 0:
		attract_warning(p_scale, &message_text);
		break;
	case 1:
		system_mode_idle();
		break;
	case 2:
		attract_ranking(p_scale, &message_text);
		break;
	case 3:
		attract_title(p_scale, &count, p_height, &message_text);
		break;
	case 4:
		attract_demo_1(credits, blink_bit, credits_needed, cursor, text_position, p_scale, &count, p_height, &message_text);
		break;
	case 5:
		attract_demo_2(credits, blink_bit, credits_needed, cursor, text_position, p_scale, &count, p_height, &message_text);
		break;
	case 6:
		run_game_mode(credits, blink_bit, credits_needed, cursor, text_position, p_scale, &count, p_height, &message_text);
		break;
	case 7:
		wait_attract_timer();
		break;
	default:
		invalid_jump(system_mode_handlers[system_mode]);
	}
	run_objects();
	build_sprite_list(p_scale, &count, p_height, &message_text, &sprite_entry);
	draw_queued_strings((uint16_t)count, (uint16_t)message_text, sprite_entry, p_scale, &string_cursor, p_height);
	*p_cursor = string_cursor;
}

/* System mode 3, the title: once, the display off for 2 frames, screen mode 0, the text layer and the
   objects cleared, the title's state reset and current_screen 3; then every frame the (C) SEGA 1992 line
   and run_title_screen, until title_done sends it on to the next attract screen. */
void attract_title(uint32_t *p_color, uint32_t *p_row_pairs_left, uint32_t *p_blocks_per_row, uint32_t *p_text)
{
	uint8_t mode = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, text_position = 0;
	uint8_t cnt_bits = 0;

	switch (system_mode_step)
	{
	case 0:
	case 2:
		display_off_frames = 2u;
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits & ~2u);
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&ignored_2, &ignored_3, &ignored);
		reset_objects(&ignored_2, &ignored_4);
		unused_title_byte_3 = 0u;
		sega_logo_step = 0u;
		title_scene_built = 0u;
		scroll_skip_open = 0u;
		unused_title_byte_1 = 0u;
		unused_title_byte_2 = 0u;
		current_screen = 3u;
		system_mode_step++;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x221u + system_mode_step * 2u);
	}
	*p_text = 0x644u;
	print_text(p_text, &text_position);
	run_title_screen(text_position, p_color, p_row_pairs_left, p_blocks_per_row, p_text);
	if (title_done != 0u)
	{
		advance_attract_screen();
		return;
	}
}

/* System mode 2, the ranking: once, screen mode 0, the text layer, page 0 and the objects cleared,
   start_ranking_display and current_screen 2; the next attract screen once ranking_done is set. */
void attract_ranking(uint32_t *p_rows, uint32_t *p_source)
{
	uint8_t mode = 0;
	uint32_t dest = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	switch (system_mode_step)
	{
	case 0:
	case 2:
		mode = 0u;
		set_screen_mode(mode);
		clear_text_layer(&ignored_2, &ignored_3, &ignored);
		dest = clear_page_0_and_scroll();
		reset_objects(&ignored_2, &ignored_4);
		ranking_done = 0u;
		start_ranking_display(dest, p_rows, p_source);
		current_screen = 2u;
		system_mode_step++;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x2B9u + system_mode_step * 3u);
	}
	if (ranking_done != 0u)
	{
		advance_attract_screen();
		return;
	}
}

/* System mode 4, the first demo: once, current_screen 4, game_mode 0, demo_input at
   attract_demo_1_inputs and attract_timer -0x240; then the game runs (run_game_mode) on the recorded
   inputs until attract_timer reaches 0, and the next attract screen follows. */
void attract_demo_1(uint32_t pressed, uint32_t released, uint32_t input_cursor, uint16_t cursor, uint32_t trackball_cursor, uint32_t *p_scale, uint32_t *p_count, uint32_t *p_height, uint32_t *p_trackball_port)
{
	switch (system_mode_step)
	{
	case 0:
	case 2:
		system_mode_step++;
		current_screen = 4u;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		game_mode = 0u;
		attract_timer = -0x240;
		demo_input = attract_demo_1_inputs_ADDRESS;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x31Eu + system_mode_step * 3u);
	}
	if ((uint16_t)attract_timer == 0u)
	{
		advance_attract_screen();
		return;
	}
	run_game_mode(pressed, released, input_cursor, cursor, trackball_cursor, p_scale, p_count, p_height, p_trackball_port);
}

/* System mode 5, the second demo: as attract_demo_1, with current_screen 5 and attract_demo_2_inputs. */
void attract_demo_2(uint32_t pressed, uint32_t released, uint32_t input_cursor, uint16_t cursor, uint32_t trackball_cursor, uint32_t *p_scale, uint32_t *p_count, uint32_t *p_height, uint32_t *p_trackball_port)
{
	switch (system_mode_step)
	{
	case 0:
	case 2:
		system_mode_step++;
		current_screen = 5u;
		stage_init_flags &= ~0x8000u;
		stage_init_flags &= ~0x40000u;
		stage_init_flags &= ~0x80000u;
		game_mode = 0u;
		attract_timer = -0x240;
		demo_input = attract_demo_2_inputs_ADDRESS;
		break;
	case 1:
		break;
	default:
		invalid_jump(0x36Fu + system_mode_step * 2u);
	}
	if ((uint16_t)attract_timer == 0u)
	{
		advance_attract_screen();
		return;
	}
	run_game_mode(pressed, released, input_cursor, cursor, trackball_cursor, p_scale, p_count, p_height, p_trackball_port);
}

/* Moves the attract mode to its next screen: attract_step + 1 (wrapping after 8), system_mode =
   attract_sequence[attract_step], system_mode_step 0. */
void advance_attract_screen(void)
{
	uint32_t next_step = 0;

	system_mode_step = 0u;
	next_step = (uint8_t)(attract_step + 1u) & 7u;
	system_mode = attract_sequence[next_step];
	attract_step = next_step;
}

/* Sets the machine up, at power-on and after test mode: interrupts off, work RAM cleared, the I/O ports,
   the frame and system state, sound, settings and bookkeeping, mixer, palettes, sprites, font, video
   registers and objects, three read_controls, the interrupt controller acknowledged,
   check_moving_seat_dip and init_game_settings; interrupts back on. */
void init_system(uint32_t *p_polynomial, uint32_t *p_words_left)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0, ignored_7 = 0, ignored_8 = 0, inputs = 0, sound_hold = 0;

	interrupts_off();
	clear_work_ram();
	init_io_ports();
	frames_to_wait = 0;
	frames_per_wait = 0u;
	debug_pause = 0u;
	unused_frame_byte = 0u;
	test_mode = 0u;
	vblank_copies_pending = 0u;
	sprite_list_ready = 0u;
	screen_mode = 0u;
	game_in_progress = 0u;
	unused_system_byte = 0u;
	vblank_copy_count = 0u;
	attract_step = 0u;
	stage_init_flags = 0u;
	random_position = 0u;
	sound_hold = init_sound();
	load_settings(sound_hold);
	load_bookkeeping();
	apply_settings();
	init_mixer();
	load_default_palettes();
	init_sprites(&ignored_2, &ignored_3);
	load_text_font(&ignored_4, p_words_left);
	init_video_registers();
	reset_objects(&inputs, &ignored_5);
	read_controls(&inputs, &ignored_6, &ignored_7, &ignored_5, &ignored_8, &ignored_3, &ignored);
	read_controls(&inputs, &ignored_6, &ignored_7, &ignored_5, &ignored_8, &ignored_3, &ignored);
	read_controls(&inputs, &ignored_6, &ignored_7, &ignored_5, &ignored_8, &ignored_3, &ignored);
	mem_write8(irq_acknowledge, 0u);
	mem_write8(irq_control_5, 0u);
	*p_words_left = check_moving_seat_dip(*p_words_left);
	*p_polynomial = init_game_settings();
	interrupts_on();
}

/* Loads the mixer: 0x28 halfwords of defaults into the mixer registers, and their last 0x10 into the
   mixer shadows from mixer_text_blend, which irq_vblank_start copies out every frame. */
void init_mixer(void)
{
	move_halfwords_backward(MIXER_REGISTERS, mixer_defaults_ADDRESS, 0x28u);
	move_halfwords_backward(mixer_text_blend_ADDRESS, 0x5ECu, 0x10u);
}

/* Sets the I/O chip up: port D 0x10, ports G and H 0, CNT 0x80, direction 0xC8 (D, G and H as outputs),
   then D, G and H again; cabinet_events 0x1F. */
void init_io_ports(void)
{
	mem_write8(io_port_d, 0x10u);
	mem_write8(io_port_g, 0u);
	mem_write8(io_port_h, 0u);
	mem_write8(io_cnt, 0x80u);
	mem_write8(io_direction, 0xC8u);
	mem_write8(io_port_d, 0x10u);
	mem_write8(io_port_g, 0u);
	mem_write8(io_port_h, 0u);
	mem_write8(cabinet_events, 0x1Fu);
}

/* Zeroes the work RAM from 0x208000 to 0x20FDFF (0xFC blocks of 0x80 bytes), keeping
   system_mode_and_step. */
void clear_work_ram(void)
{
	uint32_t blocks_left = 0, dest = 0, fill_word = 0;
	uint16_t saved_system_mode_and_step = 0;

	saved_system_mode_and_step = system_mode_and_step;
	dest = GAME_VARIABLES;
	blocks_left = 0xFCu;
	fill_word = 0u;
	do
	{
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		dest += 4u;
		mem_write32(dest, fill_word);
		blocks_left--;
		dest += 4u;
	} while (blocks_left != 0u);
	system_mode_and_step = saved_system_mode_and_step;
}

/* The game's settings at init_system: cabinet_config from setting_cabinet_config, game_in_progress,
   demo_prompt_shown and joining_closed 0, the ranking loaded (load_ranking, whose result it hands back)
   and stage_order copied to stage_order_copy (0x11 halfwords). */
uint32_t init_game_settings(void)
{
	uint32_t polynomial = 0;

	cabinet_config = setting_cabinet_config;
	game_in_progress = 0u;
	demo_prompt_shown = 0u;
	joining_closed = 0u;
	polynomial = load_ranking();
	move_halfwords_backward(stage_order_copy_ADDRESS, stage_order_ADDRESS, 0x11u);
	return polynomial;
}

/* An empty loop of 0x8000 passes that boot runs before the first init_system. */
void delay_after_reset(void)
{
	uint32_t count = 0;

	count = 0x8000u;
	do
	{
		count--;
	} while (count != 0u);
}

/* Sets frames_to_wait = frames_per_wait, then wait_for_frame; frames_per_wait is 0 (only the unused
   init_sprites_swapping_every_frame sets 1), so it waits one frame. Hands back wait_for_frame's count. */
uint32_t wait_for_frames(void)
{
	uint32_t count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0;

	frames_to_wait = frames_per_wait;
	wait_for_frame(&ignored, &ignored_2, &ignored_3, &count, &ignored_4, &ignored_5);
	return count;
}

/* Waits for the next frame: sets sprite_list_ready, polls until irq_vblank_end has counted
   frames_to_wait below 0, reloads it from frames_per_wait, points sprite_list_start_offset at the sprite
   back buffer, attract_timer + 1, and reads the controls (read_controls), handing back what that hands
   back. */
void wait_for_frame(uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t inputs = 0, poll_count = 0;
	uint8_t frames_left = 0;

	poll_count = 0u;
	sprite_list_ready = 0xFFu;
	do
	{
		frame_wait(frames_to_wait_ADDRESS);
		frames_left = frames_to_wait;
		take_interrupts();
		poll_count--;
	} while ((poll_count != 0u) & ((int8_t)frames_left >= 0));
	frame_end();
	inputs = 0u - poll_count;
	frames_to_wait = frames_per_wait;
	sprite_buffer_flip &= 0xFF04u;
	sprite_list_start_offset = sprite_back_buffer_offset;
	attract_timer++;
	read_controls(&inputs, p_pressed, p_released, p_input_cursor, p_count, p_trackball_port, p_trackball_cursor);
}

/* Reads the controls: ports B, A, F, E as one word (B on top) through play_demo_buttons into input_ports
   and input_ports_active, inputs_pressed and inputs_released from the change (active low); the six
   trackball counts into trackball_1_x to trackball_3_y (one beyond +-0x60 keeps the last), the counters
   cleared, x negated; play_demo_trackballs. */
void read_controls(uint32_t *p_inputs, uint32_t *p_pressed, uint32_t *p_released, uint32_t *p_input_cursor, uint32_t *p_count, uint32_t *p_trackball_port, uint32_t *p_trackball_cursor)
{
	uint32_t inputs = *p_inputs;
	uint32_t axes_left = 0, count = 0, pressed = 0, trackball_cursor = 0, trackball_port = 0;

	inputs = (inputs & ~0xFFu) | mem_read8(io_port_b);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_a);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_f);
	inputs = (((inputs << 8) | (inputs >> 24)) & ~0xFFu) | mem_read8(io_port_e);
	inputs = play_demo_buttons(inputs);
	pressed = input_ports;
	input_ports = inputs;
	input_ports_active = ~inputs;
	*p_released = ~pressed & inputs;
	inputs = ~inputs;
	pressed &= inputs;
	inputs_pressed = pressed;
	*p_input_cursor = inputs_released_ADDRESS;
	inputs_released = *p_released;
	trackball_port = IO_PORTS + 0x40u;
	trackball_cursor = trackball_1_x_ADDRESS;
	axes_left = 6u;
	do
	{
		inputs = (inputs & ~0xFFFFu) | (uint16_t)(int8_t)mem_read8(trackball_port);
		if ((int16_t)inputs <= 0x60 && (int16_t)inputs >= -0x60)
			mem_write16(trackball_cursor, inputs);
		trackball_cursor += 2u;
		axes_left--;
		trackball_port += 4u;
	} while (axes_left != 0u);
	trackball_port = IO_PORTS + 0x40u;
	inputs &= ~0xFFFFu;
	count = 3u;
	do
	{
		mem_write16(trackball_port, inputs);
		count--;
		trackball_port += 8u;
	} while (count != 0u);
	trackball_1_x = 0u - trackball_1_x;
	trackball_2_x = 0u - trackball_2_x;
	trackball_3_x = 0u - trackball_3_x;
	play_demo_trackballs(&count, &trackball_port, &trackball_cursor);
	*p_inputs = inputs;
	*p_pressed = pressed;
	*p_count = count;
	*p_trackball_port = trackball_port;
	*p_trackball_cursor = trackball_cursor;
}

/* In a demo (current_screen 4 or 5, current_game_mode 3, no game in progress): puts the 3 low bits of
   the byte at demo_input into bits 25-27 of inputs, the demo players' buttons, and moves demo_input on
   by one. Hands back inputs. */
uint32_t play_demo_buttons(uint32_t inputs)
{
	uint32_t demo_cursor = 0, ignored = 0, ignored_2 = 0;

	if ((int8_t)game_in_progress >= 0 && (int8_t)test_mode >= 0 && (current_screen == 4u || current_screen == 5u) && current_game_mode == 3u)
	{
		demo_patched_inputs = inputs;
		demo_cursor = demo_input;
		move_bits_down(demo_cursor, 0u, temp_0_ADDRESS, 0x19u, 1u, &ignored, &ignored_2);
		move_bits_down(demo_cursor, 1u, temp_0_ADDRESS, 0x1Au, 1u, &ignored, &ignored_2);
		move_bits_down(demo_cursor, 2u, temp_0_ADDRESS, 0x1Bu, 1u, &ignored, &ignored_2);
		inputs = demo_patched_inputs;
		demo_input++;
	}
	return inputs;
}

/* In a demo (current_screen 4 or 5, current_game_mode 3, no game in progress): the next 6 signed bytes
   at demo_input become trackball_1_x to trackball_3_y, and demo_input moves past them. */
void play_demo_trackballs(uint32_t *p_count, uint32_t *p_demo_cursor, uint32_t *p_trackball_cursor)
{
	uint32_t count = *p_count;
	uint32_t demo_cursor = *p_demo_cursor;
	uint32_t trackball_cursor = *p_trackball_cursor;

	if ((int8_t)game_in_progress < 0 || (int8_t)test_mode < 0 || (current_screen != 4u && current_screen != 5u))
		return;
	if (current_game_mode != 3u)
		return;
	demo_cursor = demo_input;
	trackball_cursor = trackball_1_x_ADDRESS;
	count = 6u;
	do
	{
		mem_write16(trackball_cursor, DEMO_TRACKBALL_DELTAS(demo_cursor)[0]);
		trackball_cursor += 2u;
		count--;
		demo_cursor++;
	} while (count != 0u);
	demo_input = demo_cursor;
	*p_count = count;
	*p_demo_cursor = demo_cursor;
	*p_trackball_cursor = trackball_cursor;
}

/* Unused: a debug pause on start 3 (inputs_pressed bit 7): debug_pause + 1 (no vblank_hook), then frames
   go by (wait_for_frame, attract_timer held still) until start 3 is pressed again; debug_pause - 1. */
void pause_on_start_3(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;

	if ((inputs_pressed & 0x80u) == 0u)
		return;
	debug_pause++;
	do
	{
		attract_timer--;
		wait_for_frame(&ignored, &ignored_2, &ignored_3, &ignored_4, &ignored_5, &ignored_6);
	} while ((inputs_pressed & 0x80u) == 0u);
	debug_pause--;
}

/* Splits a frame count (frames_high:frames, a bookkeeping time) into days (/ 0x4E9FA0), hours (/
   0x346A7), minutes (/ 0xDFB) and seconds (/ 0x3C), each from the last remainder: the first three
   divisors take 59.64 frames a second, the last 60. */
void split_frame_count(uint32_t frames, uint32_t frames_high, uint32_t *p_seconds, uint32_t *p_minutes, uint32_t *p_hours, uint32_t *p_days)
{
	uint32_t hours_dividend = 0, hours_dividend_high = 0, minutes_dividend = 0, minutes_dividend_high = 0, seconds_dividend = 0, seconds_dividend_high = 0;

	divx_pair(FRAMES_PER_DAY, &frames, &frames_high);
	*p_days = frames;
	hours_dividend = frames_high;
	hours_dividend_high = 0u;
	divx_pair(FRAMES_PER_HOUR, &hours_dividend, &hours_dividend_high);
	*p_hours = hours_dividend;
	minutes_dividend = hours_dividend_high;
	minutes_dividend_high = 0u;
	divx_pair(FRAMES_PER_MINUTE, &minutes_dividend, &minutes_dividend_high);
	*p_minutes = minutes_dividend;
	seconds_dividend = minutes_dividend_high;
	seconds_dividend_high = 0u;
	divx_pair(FRAMES_PER_SECOND, &seconds_dividend, &seconds_dividend_high);
	*p_seconds = seconds_dividend;
}

/* System mode 0, the warning screen after power-on: step 0 sets attract_timer -0x258, turns the display
   off for 3 frames, loads the font, video registers and sprites, screen mode 1 (wide), init_sound, and
   prints warning_screen_text; step 1 waits for attract_timer (wait_attract_timer); step 2 goes on to the
   next attract screen. */
void attract_warning(uint32_t *p_pixels, uint32_t *p_text)
{
	uint8_t new_mode = 0;
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;
	uint8_t cnt_bits = 0;

	switch (system_mode_step)
	{
	case 0:
	case 4:
		break;
	case 1:
		wait_attract_timer();
		return;
	case 2:
		advance_attract_screen();
		return;
	default:
		invalid_jump(0x9C4EFu + system_mode_step * 3u);
	}
	attract_timer = -0x258;
	system_mode_step++;
	display_off_frames = 3u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits & ~2u);
	load_text_font(p_pixels, &ignored);
	init_video_registers();
	init_sprites(&ignored_2, &ignored_3);
	new_mode = 1u;
	set_screen_mode(new_mode);
	init_sound();
	*p_text = warning_screen_text_ADDRESS;
	print_text_list(p_text, &ignored_4);
	unused_coin_byte = 0u;
	current_screen = 0u;
}

/* System mode 1, which attract_sequence never holds: only clears system_mode_step. */
void system_mode_idle(void)
{
	system_mode_step = 0u;
}

/* System mode 7, and step 1 of attract_warning: once attract_timer reaches 0, system_mode_step + 1 and
   frame_counter 0. */
void wait_attract_timer(void)
{
	if ((uint16_t)attract_timer == 0u)
	{
		system_mode_step++;
		frame_counter = 0u;
	}
}

/* The vblank interrupt (vector 0x40): the mixer shadows out to the mixer, the pending vblank copies,
   vblank_start_hook, a sound command (send_sound_command); outside test mode
   count_frames_and_check_test_switch and, unless free_play, update_coins; display_off_frames counts down
   and at 0 turns the display back on (io_cnt bit 1); acknowledged, vblank_counter + 1. */
void irq_vblank_start(void)
{
	uint32_t zero = 0;
	uint32_t ignored_3 = 0, ignored_4 = 0, ignored_5 = 0, ignored_6 = 0;
	uint32_t saved_r14 = 0;
	uint8_t frames_left = 0, cnt_bits = 0;

	saved_r14 = r14;
	move_halfwords(MIXER_REGISTERS + 0x30u, mixer_text_blend_ADDRESS, 0x10u);
	if (vblank_copies_pending != 0u)
		run_vblank_copies(&ignored_3, &ignored_4, &ignored_5, &ignored_6);
	call_code(vblank_start_hook);
	send_sound_command();
	if (test_mode == 0u)
	{
		count_frames_and_check_test_switch();
		if (free_play == 0u)
			update_coins();
	}
	frames_left = display_off_frames - 1u;
	zero = frames_left == 0u;
	display_off_frames = frames_left;
	if ((int8_t)frames_left < 0)
		display_off_frames = 0xFFu;
	else if (zero)
	{
		cnt_bits = mem_read8(io_cnt);
		mem_write8(io_cnt, cnt_bits | 2u);
		display_off_frames = 0xFFu;
		mem_write8(irq_acknowledge, 0xFEu);
		vblank_counter++;
		r14 = saved_r14;
		return;
	}
	mem_write8(irq_acknowledge, 0xFEu);
	vblank_counter++;
	r14 = saved_r14;
}

/* The vblank end interrupt (vector 0x41): runs vblank_hook (the sprite buffer swap) unless debug_pause
   is set, counts frames_to_wait down for wait_for_frame, and acknowledges (0xFD to irq_acknowledge). */
void irq_vblank_end(void)
{
	if (debug_pause == 0u)
		call_code(vblank_hook);
	frames_to_wait--;
	mem_write8(irq_acknowledge, 0xFDu);
}

/* The sound interrupt (vector 0x42): only acknowledges it (0xFB to irq_acknowledge). */
void irq_sound(void)
{
	mem_write8(irq_acknowledge, 0xFBu);
}

/* Timer 0's interrupt (vector 0x43): timer_0_ticks + 1 (nothing reads it), the timer reloaded with 0x32,
   acknowledged (0xF7 to irq_acknowledge). */
void irq_timer0(void)
{
	timer_0_ticks++;
	mem_write16(timer_0_reload, 0x32u);
	mem_write8(irq_acknowledge, 0xF7u);
}

/* Timer 1's interrupt (vector 0x44): timer_1_ticks + 1 (nothing reads it), the timer reloaded with 0x23,
   acknowledged (0xEF to irq_acknowledge). */
void irq_timer1(void)
{
	timer_1_ticks++;
	mem_write16(timer_1_reload, 0x23u);
	mem_write8(irq_acknowledge, 0xEFu);
}

/* The handler of 247 of the 256 vector table entries: four halts, then on into int_vector_02 (four more
   halts, then run_vblank_copies). */
void int_default(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	halt();
	halt();
	halt();
	halt();
	int_vector_02(p_source, p_dest, p_copies_left, p_copy_entry);
}

/* Vector 2, the NMI: four halts, then run_vblank_copies, which follows it in the program; int_default
   runs into it. */
void int_vector_02(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	halt();
	halt();
	halt();
	halt();
	run_vblank_copies(p_source, p_dest, p_copies_left, p_copy_entry);
}

/* Runs the copies queued for vblank: for each of the vblank_copy_count entries of vblank_copies (source,
   dest, halfwords) the source block is copied to dest and on after it as many times as dest's top byte
   says; vblank_copy_count and vblank_copies_pending are cleared. */
void run_vblank_copies(uint32_t *p_source, uint32_t *p_dest, uint32_t *p_copies_left, uint32_t *p_copy_entry)
{
	uint32_t dest = *p_dest;
	uint8_t repeats_left = 0;
	uint32_t copies_left = 0, copy_entry = 0, copy_halfwords = 0, dest_end = 0, ignored = 0;
	uint8_t repeats_before = 0;

	vblank_copies_pending = 0u;
	copies_left = vblank_copy_count;
	vblank_copy_count = 0u;
	copy_entry = vblank_copies_ADDRESS;
	if ((int16_t)copies_left > 0)
	{
		do
		{
			*p_source = VBLANK_COPY(copy_entry)->source;
			copy_entry += 4u;
			dest = VBLANK_COPY(copy_entry - 0x4u)->dest;
			copy_entry += 4u;
			copy_halfwords = VBLANK_COPY(copy_entry - 0x8u)->halfwords;
			repeats_left = (uint8_t)((dest << 8) | (dest >> 24));
			copy_entry += 2u;
			for (;;)
			{
				move_string(*p_source, copy_halfwords, dest, copy_halfwords, 2u, 0u, 0u, 0u, &dest_end, &ignored);
				repeats_before = repeats_left;
				repeats_left = (uint8_t)(repeats_before - 1u);
				if ((int8_t)repeats_before <= 1)
					break;
				dest = dest_end;
			}
			copies_left--;
		} while (copies_left != 0u);
	}
	*p_dest = dest;
	*p_copies_left = copies_left;
	*p_copy_entry = copy_entry;
}

/* Sends one queued sound a frame (irq_vblank_start): the next id of sound_queue (32 entries) is looked
   up in sound_commands and its two bytes written to the sound CPU's mailbox, sound_command_group and
   sound_command. */
void send_sound_command(void)
{
	uint32_t command_entry = 0, queue_base = 0, read_index = 0;

	read_index = sound_queue_read;
	queue_base = sound_queue_ADDRESS;
	if (read_index != sound_queue_write)
	{
		command_entry = sound_commands_ADDRESS + mem_read8(queue_base + read_index) * 2u;
		mem_write8(sound_command_group, SOUND_COMMAND_BYTES(command_entry)->group);
		mem_write8(sound_command, SOUND_COMMAND_BYTES(command_entry)->number);
		sound_queue_read = (read_index + 1u) & 0x1Fu;
	}
}

/* Unused: ports F and E (service, test, coins, starts and the board's switches) as one halfword in
   service_inputs, the last one in service_inputs_previous, with service_inputs_pressed and
   service_inputs_released from the change (active low): read_controls' edges on 16 bits. */
void read_service_inputs(void)
{
	uint16_t inputs = 0, previous = 0;
	uint8_t port_f_bits = 0, port_e_bits = 0;

	port_f_bits = mem_read8(io_port_f);
	port_e_bits = mem_read8(io_port_e);
	inputs = (uint16_t)((uint16_t)((uint32_t)port_f_bits << 8) | port_e_bits);
	previous = service_inputs;
	service_inputs = inputs;
	service_inputs_previous = previous;
	service_inputs_pressed = previous & ~inputs;
	service_inputs_released = ~previous & inputs;
}

/* From irq_vblank_start outside test mode: power_on_frames + 1 (carrying into power_on_frames_high),
   saved to the bookkeeping and its backup copy every 64 frames; game_frames + 1; test_mode set when the
   test switch or the board's test button (inputs_pressed bits 1, 13) is pressed without service (bit 0). */
void count_frames_and_check_test_switch(void)
{
	uint32_t carry = 0;
	uint8_t old_divider = 0;
	uint16_t old_frames = 0;

	old_frames = power_on_frames;
	carry = (uint16_t)(old_frames + 1u) < old_frames;
	power_on_frames = old_frames + 1u;
	if (carry)
		power_on_frames_high++;
	old_divider = power_time_save_divider;
	power_time_save_divider = old_divider - 4u;
	if (old_divider < 4u)
	{
		mem_write32(bookkeeping_power_on_time, power_on_time);
		mem_write16(bookkeeping_power_on_time_high, power_on_frames_top);
		update_bookkeeping_checksum();
		mem_write32(bookkeeping_backup_power_on_time, power_on_time);
		mem_write16(bookkeeping_backup_power_on_time_high, power_on_frames_top);
	}
	game_frames++;
	if ((inputs_pressed & 0x2002u) != 0u && (inputs_pressed & 1u) == 0u)
		test_mode = 0xFFu;
}

/* The V60's reset vector: jumps to boot. */
void reset(void)
{
	boot();
}
