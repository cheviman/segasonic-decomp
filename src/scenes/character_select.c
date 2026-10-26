/**
 *    Name:        scenes/character_select.c
 *    Description: choosing a character: the PLAYER SELECT chooser at a new game and the chooser of a player joining
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Unused: clears characters_taken, players_in_join_chooser, free_character_count and
   character_select_timer (the original's overlapping stores clear the timer twice). */
void reset_character_select(void)
{
	characters_taken = 0u;
	players_in_join_chooser = 0u;
	character_select_timer = 0;
}

/* Opens a player's chooser in game mode 0x1B (the first one also shows the background picture and the
   PLAYER SELECT heading with its countdown), character_select_timer 0x258: the first free character from
   the player's number, the next two smaller at its sides, a name, the player label, a panel. right_count
   and left_count start at its screen x and y. */
void start_character_chooser(uint32_t block)
{
	uint16_t chooser_x = 0;
	uint32_t character = 0, frame_index = 0, free_count = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, object = 0, player = 0, x_index = 0;

	count_free_characters(&ignored, &free_count, &object);
	players_choosing_character++;
	if (character_select_timer <= 0)
	{
		mem_write16(video_layer_disable, 0x2Fu);
		mem_write16(video_backdrop_control, 0u);
		show_nbg0_picture(&free_count, &ignored_2, &ignored_3, &block);
		spawn_count = 2u;
		spawn_count = 0xAu;
		spawn_object_with_pieces(0x08F9E2u, &ignored, &ignored_4, &free_count, &object, &block);
		OBJECT(object)->screen_x = 0x118;
		OBJECT(object)->screen_y = 0x1F;
		OBJECT(object)->sprite_palette = 0x8AD0u;
		OBJECT(object)->handler = character_select_countdown_update_ADDRESS;
		object = LINKED(object, OBJECT(object)->child);
		OBJECT(object)->screen_x = 0x90;
		OBJECT(object)->screen_y = 0x20;
		OBJECT(object)->sprite_frame = 0xB708u;
		OBJECT(object)->sprite_palette = 0x8D90u;
		OBJECT(object)->piece_value = 1u;
	}
	character_select_timer = 0x258;
	spawn_count = 8u;
	spawn_object_with_pieces(0x08FA36u, &ignored, &ignored_4, &free_count, &object, &block);
	OBJECT(object)->character_chooser.player_block = current_player;
	player = OBJECT(object_cursor)->player;
	OBJECT(object)->kind = player;
	character = player;
	frame_index = character * 4u;
	while ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) != 0u)
	{
		character = (uint8_t)(character + 1u);
		frame_index = character * 4u;
		if ((int8_t)character <= 2)
			continue;
		character = 0u;
		frame_index &= ~0xFFu;
	}
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	x_index = player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	chooser_x = chooser_x_by_player[x_index];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xA0;
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->character_chooser.character = player;
	OBJECT(object)->character_chooser.turn_direction = 1;
	OBJECT(object)->handler = character_chooser_wait_ADDRESS;
	OBJECT(object)->character_chooser.right_count = OBJECT(object)->screen_x;
	OBJECT(object)->character_chooser.left_count = OBJECT(object)->screen_y;
	frame_index = (frame_index & ~0xFFu) | (uint8_t)(frame_index + 4u);
	if ((int8_t)frame_index > 8)
		frame_index &= ~0xFFu;
	character = (uint8_t)(character + 1u);
	character = character % 3u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_x -= 0x18u;
	OBJECT(object)->screen_y = 0x88;
	OBJECT(object)->piece_value = character;
	OBJECT(object)->sprite_scale = 0xC0u;
	frame_index = (frame_index & ~0xFFu) | (uint8_t)(frame_index + 4u);
	if ((int8_t)frame_index > 8)
		frame_index &= ~0xFFu;
	character = (uint8_t)(character + 1u);
	character = character % 3u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags |= 0x100u;
	OBJECT(object)->sprite_frame = chooser_portrait_frames[frame_index];
	OBJECT(object)->sprite_palette = chooser_portrait_palettes[character];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_x += 0x18u;
	OBJECT(object)->screen_y = 0x88;
	OBJECT(object)->piece_value = character;
	OBJECT(object)->sprite_scale = 0xC0u;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_palette = 0x8DB0u;
	OBJECT(object)->sprite_frame = character_name_sprites[player];
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xB8;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_frame = chooser_player_labels[player];
	OBJECT(object)->sprite_palette = 0x8DA0u;
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0xD0;
	object = LINKED(object, OBJECT(object)->child);
	OBJECT(object)->sprite_flags = 8u;
	OBJECT(object)->sprite_frame = 0xB740u;
	OBJECT(object)->sprite_palette = 0x8DC0u;
	OBJECT(object)->screen_x = chooser_x;
	OBJECT(object)->screen_y = 0x30;
}

/* The chooser's handler: confirms (character_chooser_confirm) on its player's button, with one free
   character left or with character_select_timer below 0x1E. Trackball x adds 8 a frame to right_count or
   left_count; when their high bytes (taken and cleared each frame) differ it turns that way
   (character_chooser_turn), else character_chooser_idle. */
void character_chooser_wait(void)
{
	uint8_t left_turns = 0, right_turns = 0, turn = 0;
	uint16_t trackball_x = 0;
	uint32_t kind = 0;
	uint8_t right_steps = 0;

	if (free_character_count == 1u || character_select_timer < 0x1E)
	{
		character_chooser_confirm();
		return;
	}
	kind = OBJECT(current_object)->kind;
	if ((inputs_pressed & shl32(1u, (int)(chooser_button_bits[kind]))) != 0u)
	{
		character_chooser_confirm();
		return;
	}
	trackball_x = RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
	if (trackball_x == 0u)
	{
		character_chooser_idle();
		return;
	}
	if ((int16_t)trackball_x < 0)
	{
		OBJECT(current_object)->character_chooser.right_count = 0u;
		OBJECT(current_object)->character_chooser.left_count += 8u;
	}
	else
	{
		OBJECT(current_object)->character_chooser.right_count += 8u;
		OBJECT(current_object)->character_chooser.left_count = 0u;
	}
	right_turns = OBJECT(current_object)->character_chooser.right_turns;
	left_turns = OBJECT(current_object)->character_chooser.left_turns;
	OBJECT(current_object)->character_chooser.left_turns = 0u;
	OBJECT(current_object)->character_chooser.right_turns = 0u;
	right_steps = right_turns;
	turn = (uint8_t)(right_steps - left_turns);
	if (right_steps == left_turns)
	{
		character_chooser_idle();
		return;
	}
	OBJECT(current_object)->character_chooser.turn_direction = turn;
	OBJECT(current_object)->character_chooser.step_timer = 0;
	OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
	OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
	character_chooser_turn();
}

/* Takes the first free character from the one shown into the kind of its player object (the parent) and
   characters_taken; free_character_count and players_choosing_character drop (none left choosing:
   character_select_timer 0x3C). The portrait goes back in place in the chosen pose; a random voice of
   the character (chooser_voice_sounds), sound 0x20. */
void character_chooser_confirm(void)
{
	uint8_t sound_id = 0;
	uint32_t character = 0, chosen_character = 0, frame_index = 0, ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, random_value = 0, x_index = 0;

	while ((characters_taken & shl32(1u, (int)(character_taken_bits[OBJECT(current_object)->character_chooser.character]))) != 0u)
	{
		OBJECT(current_object)->character_chooser.character++;
		OBJECT(current_object)->character_chooser.character = OBJECT(current_object)->character_chooser.character % 3u;
	}
	character = OBJECT(current_object)->character_chooser.character;
	OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->kind = character;
	characters_taken |= shl32(1u, (int)(character_taken_bits[character]));
	free_character_count--;
	players_choosing_character--;
	if ((int8_t)players_choosing_character < 1)
		character_select_timer = 0x3C;
	OBJECT(current_object)->sprite_scale = 0x100u;
	x_index = OBJECT(current_object)->kind;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	OBJECT(current_object)->screen_x = chooser_x_by_player[x_index];
	OBJECT(current_object)->screen_y = 0xA0;
	frame_index = OBJECT(current_object)->character_chooser.character;
	chosen_character = frame_index;
	frame_index = (uint8_t)(frame_index * 4u);
	OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(frame_index + 3u)];
	OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[chosen_character];
	OBJECT(current_object)->handler = character_chooser_chosen_ADDRESS;
	next_random(&random_value, &ignored, &ignored_2);
	sound_id = chooser_voice_sounds[OBJECT(current_object)->character_chooser.character].sounds[random_value & 3u];
	play_sound(sound_id, &ignored_3, &ignored_4);
	sound_id = 0x20u;
	play_sound(sound_id, &ignored_3, &ignored_4);
	character_chooser_chosen();
}

/* No turn this frame: a taken character turns the chooser on (character_chooser_turn); otherwise the
   portrait steps through chooser_idle_steps every 3 frames and is drawn with the side portraits (taken
   ones dimmed, palette 0x87A0) and the labels. */
void character_chooser_idle(void)
{
	uint32_t character_bit = 0, idle_frame = 0, piece_value = 0, taken_bit = 0;
	uint16_t step_timer = 0;

	character_bit = character_taken_bits[OBJECT(current_object)->character_chooser.character];
	if ((characters_taken & shl32(1u, (int)(character_bit))) != 0u)
	{
		OBJECT(current_object)->character_chooser.step_timer = 0;
		OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
		OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
		character_chooser_turn();
		return;
	}
	step_timer = OBJECT(current_object)->character_chooser.step_timer;
	OBJECT(current_object)->character_chooser.step_timer = step_timer - 1u;
	if (step_timer == 0u)
	{
		OBJECT(current_object)->character_chooser.step_timer = 3;
		character_bit = OBJECT(current_object)->character_chooser.idle_step;
		idle_frame = chooser_idle_steps[character_bit];
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[(uint8_t)(idle_frame + OBJECT(current_object)->character_chooser.character * 4u)];
		OBJECT(current_object)->character_chooser.idle_step++;
		OBJECT(current_object)->character_chooser.idle_step &= 3u;
	}
	queue_sprite(character_bit);
	r14 = 2u;
	do
	{
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		piece_value = OBJECT(current_object)->piece_value;
		taken_bit = character_taken_bits[piece_value];
		if ((characters_taken & shl32(1u, (int)(taken_bit))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		queue_sprite(taken_bit);
		r14 = r14 - 1u;
	} while (r14 != 0u);
	draw_character_chooser_labels();
}

/* Draws the chooser's three label parts after the portraits: the name, refreshed to
   character_name_sprites[character] when name_changed is set (then cleared), the player label and the
   panel. */
void draw_character_chooser_labels(void)
{
	uint32_t ignored = 0;

	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(object_cursor)->draw_character_chooser_labels.name_changed != 0u)
		OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(object_cursor)->draw_character_chooser_labels.character];
	ignored = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	ignored = queue_sprite(ignored);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite(ignored);
	OBJECT(object_cursor)->draw_character_chooser_labels.name_changed = 0u;
}

/* A turn of 0x18 frames: the middle portrait slides by turn_direction and shrinks while the side ones
   slide and scale toward that side; then the next character that way shows (taken side ones dimmed),
   turning on again while it is taken. Confirms at once with one free character, and keeps
   character_select_timer at 0x1D or more meanwhile. */
void character_chooser_turn(void)
{
	uint16_t turn_step = 0;
	uint32_t character = 0, piece_value = 0, taken_bit = 0, x_index = 0;
	uint8_t next_character = 0;
	uint16_t turn_timer = 0, scale_step = 0;

	for (;;)
	{
		if (free_character_count == 1u)
		{
			character_chooser_confirm();
			return;
		}
		if (character_select_timer < 0x1E)
			character_select_timer = 0x1D;
		turn_timer = OBJECT(current_object)->character_chooser.turn_timer;
		OBJECT(current_object)->character_chooser.turn_timer = turn_timer - 1u;
		if (turn_timer != 0u)
			break;
		x_index = OBJECT(current_object)->kind;
		if ((cabinet_config & 1u) == 0u)
			x_index += 3u;
		OBJECT(current_object)->screen_x = chooser_x_by_player[x_index];
		OBJECT(current_object)->screen_y = 0xA0;
		OBJECT(current_object)->sprite_scale = 0x100u;
		next_character = OBJECT(current_object)->character_chooser.character + OBJECT(current_object)->character_chooser.turn_direction;
		OBJECT(current_object)->character_chooser.character = next_character;
		if ((int8_t)next_character < 0)
			OBJECT(current_object)->character_chooser.character = 2u;
		OBJECT(current_object)->character_chooser.character = OBJECT(current_object)->character_chooser.character % 3u;
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x -= 0x18u;
		OBJECT(current_object)->screen_y = 0x88;
		OBJECT(current_object)->sprite_scale = 0xC0u;
		OBJECT(current_object)->piece_value = OBJECT(object_cursor)->character_chooser.character;
		OBJECT(current_object)->piece_value++;
		OBJECT(current_object)->piece_value = OBJECT(current_object)->piece_value % 3u;
		piece_value = OBJECT(current_object)->piece_value;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[piece_value * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[piece_value]))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		current_object = LINKED(current_object, OBJECT(current_object)->child);
		OBJECT(current_object)->screen_x = OBJECT(object_cursor)->screen_x;
		OBJECT(current_object)->screen_x += 0x18u;
		OBJECT(current_object)->screen_y = 0x88;
		OBJECT(current_object)->sprite_scale = 0xC0u;
		OBJECT(current_object)->piece_value = OBJECT(object_cursor)->character_chooser.character;
		OBJECT(current_object)->piece_value += 2u;
		OBJECT(current_object)->piece_value = OBJECT(current_object)->piece_value % 3u;
		piece_value = OBJECT(current_object)->piece_value;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[piece_value * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[piece_value]))) != 0u)
		{
			OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[piece_value];
			OBJECT(current_object)->sprite_palette = 0x87A0u;
		}
		current_object = object_cursor;
		character = OBJECT(current_object)->character_chooser.character;
		OBJECT(current_object)->sprite_frame = chooser_portrait_frames[character * 4u];
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[character];
		taken_bit = character_taken_bits[character];
		if ((characters_taken & shl32(1u, (int)(taken_bit))) == 0u)
		{
			OBJECT(current_object)->character_chooser.name_changed = 1u;
			OBJECT(current_object)->handler = character_chooser_wait_ADDRESS;
			character_chooser_idle();
			return;
		}
		OBJECT(current_object)->sprite_palette = chooser_portrait_palettes[character];
		OBJECT(current_object)->sprite_palette = 0x87A0u;
		OBJECT(current_object)->character_chooser.step_timer = 0;
		OBJECT(current_object)->character_chooser.turn_timer = 0x18u;
		OBJECT(current_object)->handler = character_chooser_turn_ADDRESS;
	}
	r14 = OBJECT(current_object)->character_chooser.turn_direction;
	turn_step = (uint16_t)r14;
	OBJECT(current_object)->screen_x += turn_step;
	OBJECT(current_object)->screen_y--;
	scale_step = r14 * 2u;
	r14 = scale_step;
	if ((int16_t)scale_step >= 0)
		OBJECT(current_object)->sprite_scale -= r14;
	else
		OBJECT(current_object)->sprite_scale += r14;
	taken_bit = queue_sprite(taken_bit);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if ((uint8_t)turn_step == 1u)
	{
		OBJECT(current_object)->screen_x += turn_step;
		OBJECT(current_object)->screen_y += turn_step;
		OBJECT(current_object)->sprite_scale += r14;
	}
	else
		OBJECT(current_object)->screen_x -= r14;
	taken_bit = queue_sprite(taken_bit);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if ((uint8_t)turn_step == 0xFFu)
	{
		OBJECT(current_object)->screen_x += turn_step;
		OBJECT(current_object)->screen_y -= turn_step;
		OBJECT(current_object)->sprite_scale -= r14;
	}
	else
		OBJECT(current_object)->screen_x -= r14;
	queue_sprite(taken_bit);
	draw_character_chooser_labels();
}

/* After the choice: until character_select_timer runs out (then removed with its parts) draws the chosen
   portrait, the chosen character's name, the player label and the panel; the side portraits are no
   longer drawn. */
void character_chooser_chosen(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0;

	if (character_select_timer <= 0)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	ignored_4 = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(object_cursor)->character_chooser.character];
	ignored_4 = queue_sprite(ignored_4);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	ignored_4 = queue_sprite(ignored_4);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	queue_sprite(ignored_4);
}

/* The PLAYER SELECT countdown: counts character_select_timer down, never below 0, its seconds shown as a
   digit (ranking_letter_sprites 0x20 + n) growing from scale 0x10 to 0x140; the heading's palette
   bounces through player_select_title_palettes. Removed with the heading once nobody is choosing and the
   timer is out. */
void character_select_countdown_update(void)
{
	uint32_t ignored = 0, ignored_2 = 0, ignored_3 = 0, ignored_4 = 0, seconds = 0;
	uint16_t time_left = 0, direction = 0;

	if (players_choosing_character == 0u && character_select_timer <= 0)
	{
		delete_object_and_children(&ignored, &ignored_2, &ignored_3);
		return;
	}
	time_left = character_select_timer;
	character_select_timer = time_left - 1u;
	if (time_left == 0u)
		character_select_timer = 1;
	seconds = (uint16_t)character_select_timer;
	seconds = (uint16_t)((int16_t)seconds / 0x3C);
	if (OBJECT(current_object)->character_select_countdown.shown_seconds != (uint8_t)seconds)
	{
		OBJECT(current_object)->character_select_countdown.shown_seconds = seconds;
		OBJECT(current_object)->sprite_frame = ranking_letter_sprites[(uint16_t)(seconds + 0x20u)];
		OBJECT(current_object)->sprite_scale = 0x10u;
	}
	if ((int16_t)OBJECT(current_object)->sprite_scale < 0x140)
		OBJECT(current_object)->sprite_scale += 8u;
	ignored_4 = queue_sprite(0u);
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	direction = OBJECT(current_object)->direction;
	OBJECT(current_object)->direction = direction - 1u;
	if (direction != 0u)
	{
		queue_sprite(ignored_4);
		return;
	}
	OBJECT(current_object)->direction = 2u;
	OBJECT(current_object)->sprite_palette = player_select_title_palettes[(uint8_t)OBJECT(current_object)->piece_frame];
	OBJECT(current_object)->piece_frame += OBJECT(current_object)->piece_value;
	if (OBJECT(current_object)->piece_frame > 0xC)
	{
		OBJECT(current_object)->direction_low = 8u;
		OBJECT(current_object)->piece_frame = 0xC;
		OBJECT(current_object)->piece_value = 0xFFu;
	}
	if (OBJECT(current_object)->piece_frame < 0)
	{
		OBJECT(current_object)->direction_low = 8u;
		OBJECT(current_object)->piece_frame = 0;
		OBJECT(current_object)->piece_value = 1u;
	}
	queue_sprite(ignored_4);
}

/* A joining player's chooser (player_select_character), when a character is free: in the player's block
   of join_chooser_blocks a small portrait of the first free character from the player object's kind,
   countdown 0x21C, handler join_character_chooser_update, a name block after it; the player's bit set in
   players_in_join_chooser. */
uint32_t start_join_character_chooser(void)
{
	uint16_t chooser_x = 0;
	uint32_t block = 0, character = 0, ignored = 0, ignored_2 = 0, object = 0, player = 0, x_index = 0;

	count_free_characters(&x_index, &ignored, &ignored_2);
	if (free_character_count == 0u)
		return x_index;
	character = OBJECT(object_cursor)->kind;
	player = OBJECT(object_cursor)->player;
	block = join_chooser_blocks[player];
	object = block;
	init_object(object);
	while ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) != 0u)
	{
		character = ((character & ~0xFFu) | (uint8_t)(character + 1u)) % 3u;
	}
	OBJECT(block)->kind = player;
	players_in_join_chooser |= shl32(1u, (int)(player));
	OBJECT(block)->join_character_chooser.player_block = current_player;
	x_index = player;
	if ((cabinet_config & 1u) == 0u)
		x_index += 3u;
	chooser_x = chooser_x_by_player[x_index];
	OBJECT(block)->screen_x = chooser_x;
	OBJECT(block)->screen_y = 0xCC;
	OBJECT(block)->join_character_chooser.countdown = 0x21Cu;
	OBJECT(block)->sprite_flags |= 0x100u;
	OBJECT(block)->sprite_frame = join_chooser_portraits[character];
	OBJECT(block)->sprite_palette = join_chooser_palettes[character];
	OBJECT(block)->join_character_chooser.character = character;
	OBJECT(block)->handler = join_character_chooser_update_ADDRESS;
	OBJECT(block)->join_character_chooser.turn_direction = 1;
	OBJECT(block)->child = block;
	OBJECT(block)->child += 0xA0u;
	block = LINKED(block, OBJECT(block)->child);
	object = block;
	init_object_2(object);
	OBJECT(block)->screen_x = chooser_x;
	OBJECT(block)->screen_y = 0xDD;
	OBJECT(block)->sprite_frame = character_name_sprites[character];
	OBJECT(block)->sprite_palette = 0x8DB0u;
	OBJECT(block)->piece_value = character;
	return x_index;
}

/* A joining player's chooser, while its bit in players_in_join_chooser is set: shows the countdown's
   seconds on both sides of the portrait; the trackball steps through the free characters (taken ones
   skipped), and the button, joining_closed or the end of the countdown takes the one shown
   (join_character_chooser_take_character). Drawn on top. */
void join_character_chooser_update(uint32_t text_x)
{
	uint8_t left_steps = 0, right_turns = 0, turn = 0;
	uint16_t trackball_x = 0, y = 0;
	uint32_t character = 0, kind = 0, palette = 0, player = 0, scale = 0, text = 0;
	uint8_t right_steps = 0, next_character = 0;
	uint16_t time_left = 0;

	player = OBJECT(object_cursor)->player;
	if ((players_in_join_chooser & shl32(1u, (int)(player))) == 0u)
		return;
	current_object = join_chooser_blocks[player];
	time_left = OBJECT(current_object)->join_character_chooser.countdown;
	OBJECT(current_object)->join_character_chooser.countdown = time_left - 1u;
	if (time_left == 0u)
	{
		join_character_chooser_take_character();
		return;
	}
	text_x = (text_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
	text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x - 0x25u);
	y = (uint16_t)(OBJECT(current_object)->screen_y - 0x13u);
	palette = 0x8AD0u;
	scale = 0x140u;
	text = countdown_second_texts_ADDRESS + (int32_t)OBJECT(current_object)->join_character_chooser.countdown / 0x3C * 3u;
	queue_string_at(text_x, y, &palette, &scale, &text);
	text_x = (text_x & ~0xFFFFu) | (uint16_t)OBJECT(current_object)->screen_x;
	text_x = (text_x & ~0xFFFFu) | (uint16_t)(text_x + 9u);
	y = (uint16_t)(OBJECT(current_object)->screen_y - 0x13u);
	queue_string_at(text_x, y, &palette, &scale, &text);
	if (joining_closed != 0u)
	{
		join_character_chooser_take_character();
		return;
	}
	kind = OBJECT(current_object)->kind;
	if ((inputs_pressed & shl32(1u, (int)(chooser_button_bits[kind]))) != 0u)
	{
		join_character_chooser_take_character();
		return;
	}
	trackball_x = RAM16(GAME_VARIABLES + chooser_trackball_offsets[kind]);
	if (trackball_x != 0u)
	{
		if ((int16_t)trackball_x < 0)
		{
			OBJECT(current_object)->spawn_arg_1 = 0u;
			OBJECT(current_object)->spawn_arg_2 += 8u;
		}
		else
		{
			OBJECT(current_object)->spawn_arg_1 += 8u;
			OBJECT(current_object)->spawn_arg_2 = 0u;
		}
		right_turns = OBJECT(current_object)->spawn_arg_1_high;
		left_steps = OBJECT(current_object)->spawn_arg_2_high;
		OBJECT(current_object)->spawn_arg_2_high = 0u;
		OBJECT(current_object)->spawn_arg_1_high = 0u;
		right_steps = right_turns;
		turn = (uint8_t)(right_steps - left_steps);
		if (right_steps != left_steps)
		{
			OBJECT(current_object)->join_character_chooser.turn_direction = turn;
			OBJECT(current_object)->join_character_chooser.portrait_changed = 1u;
			next_character = OBJECT(current_object)->join_character_chooser.character + turn;
			OBJECT(current_object)->join_character_chooser.character = next_character;
			if ((int8_t)next_character >= 0)
				OBJECT(current_object)->join_character_chooser.character = OBJECT(current_object)->join_character_chooser.character % 3u;
			else
				OBJECT(current_object)->join_character_chooser.character = 2u;
		}
	}
	for (;;)
	{
		character = OBJECT(current_object)->join_character_chooser.character;
		if ((characters_taken & shl32(1u, (int)(character_taken_bits[character]))) == 0u)
			break;
		OBJECT(current_object)->join_character_chooser.portrait_changed = 1u;
		OBJECT(current_object)->join_character_chooser.character += OBJECT(current_object)->join_character_chooser.turn_direction;
		if ((int8_t)OBJECT(current_object)->join_character_chooser.character < 0)
			OBJECT(current_object)->join_character_chooser.character = 2u;
		OBJECT(current_object)->join_character_chooser.character = OBJECT(current_object)->join_character_chooser.character % 3u;
	}
	if (OBJECT(current_object)->join_character_chooser.portrait_changed != 0u)
	{
		OBJECT(current_object)->sprite_frame = join_chooser_portraits[character];
		OBJECT(current_object)->sprite_palette = join_chooser_palettes[character];
	}
	queue_sprite_on_top();
	OBJECT(current_object)->join_character_chooser.portrait_changed = 0u;
	r14 = current_object;
	current_object = LINKED(current_object, OBJECT(current_object)->child);
	if (OBJECT(current_object)->piece_value != OBJECT(r14)->join_character_chooser.character)
	{
		OBJECT(current_object)->piece_value = OBJECT(r14)->join_character_chooser.character;
		OBJECT(current_object)->sprite_frame = character_name_sprites[OBJECT(current_object)->piece_value];
	}
	queue_sprite_on_top();
	current_object = object_cursor;
}

/* Ends a join chooser: the first free character from the one shown becomes the kind of the player object
   (object_cursor) and gets its characters_taken bit; the player's bit in players_in_join_chooser is
   cleared. */
void join_character_chooser_take_character(void)
{
	uint32_t character = 0, taken_bit = 0;

	for (;;)
	{
		character = OBJECT(current_object)->join_character_chooser.character;
		OBJECT(object_cursor)->kind = character;
		taken_bit = character_taken_bits[character];
		if ((characters_taken & shl32(1u, (int)(taken_bit))) == 0u)
			break;
		OBJECT(current_object)->join_character_chooser.character++;
		OBJECT(current_object)->join_character_chooser.character = OBJECT(current_object)->join_character_chooser.character % 3u;
	}
	characters_taken |= shl32(1u, (int)(taken_bit));
	players_in_join_chooser &= ~shl32(1u, (int)(OBJECT(current_object)->kind));
	current_object = object_cursor;
}

/* free_character_count = how many of the three characters have their characters_taken bit
   (character_taken_bits) clear. */
void count_free_characters(uint32_t *p_characters_left, uint32_t *p_character, uint32_t *p_taken_bit)
{
	uint8_t free_characters = 0;
	uint32_t characters_left = 0;

	characters_left = 3u;
	free_characters = 0u;
	do
	{
		*p_character = characters_left - 1u;
		*p_taken_bit = character_taken_bits[*p_character];
		if ((characters_taken & shl32(1u, (int)(*p_taken_bit))) == 0u)
			free_characters++;
		characters_left--;
	} while (characters_left != 0u);
	free_character_count = free_characters;
	*p_characters_left = characters_left;
}
