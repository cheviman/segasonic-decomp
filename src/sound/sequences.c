/**
 *    Name:        sequences.c
 *    Description: the sequence players stepped at every tick (their events into the MIDI queue, the playlist commands) and the V60's sound commands (songs and sequences started on players, volume fades)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

/* The sequencer's work on each timer tick: tick_countdown runs 4 to 1; when it wraps, players 1-7 that
   are playing are stepped (fade_player_volume on player 1 first), and when it reaches 2 the music player
   0 is (fade_music_volume first while music_fade_speed is set). Nothing while midi_queue_count is 0x40
   or more. */
void sequence_tick(void)
{
	uint16_t countdown_at = 0, mailbox_page = 0, player = 0, queue_pointer = 0;
	uint8_t old_countdown = 0;

	z80_block(28);
	countdown_at = tick_countdown_ADDRESS;
	old_countdown = tick_countdown;
	tick_countdown = old_countdown - 1u;
	if (old_countdown == 1u)
	{
		z80_cycles(5);
		z80_block(55);
		tick_countdown = 4u;
		mailbox_page = sound_command_ADDRESS;
		if (midi_queue_count >= 0x40u)
		{
			z80_cycles(6);
			return;
		}
		z80_block(51);
		player = SEQUENCE_PLAYER_ADDRESS(1);
		queue_pointer = 0xE100u | midi_queue_write;
		fade_player_volume(queue_pointer, player, mailbox_page);
		z80_block(24);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(2);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(3);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(4);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(5);
		step_player_if_playing(player, mailbox_page);
		z80_block(27);
		player = SEQUENCE_PLAYER_ADDRESS(6);
		step_player_if_playing(player, mailbox_page);
		z80_block(20);
		player = SEQUENCE_PLAYER_ADDRESS(7);
		step_player_if_playing(player, mailbox_page);
		return;
	}
	z80_block(19);
	if (2u != tick_countdown)
	{
		z80_cycles(6);
		return;
	}
	z80_block(45);
	mailbox_page = sound_command_ADDRESS;
	if (midi_queue_count >= 0x40u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(27);
	if (music_fade_speed != 0u)
	{
		z80_cycles(7);
		fade_music_volume(mailbox_page);
	}
	z80_block(32);
	player = SEQUENCE_PLAYER_ADDRESS(0);
	if (music_player_flags >> 7 != 0u)
	{
		step_player(player, mailbox_page);
		return;
	}
	z80_block(10);
}

/* Playlist bytes F0-FF by their low nibble (playlist_command_table): F0, F3-FD jump to the address that
   follows, taking the song there; F1, F2 take song 0x90 + (playlist_song_choice_1/_2 & 7); FE clears the
   player, FF all eight (spare_players for queue page 0xE2, never passed). Hands back playlist and
   playlist_byte (the song, 0 stops). */
void playlist_command(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page, uint16_t *p_playlist, uint8_t *p_playlist_byte)
{
	uint16_t playlist = (uint16_t)*p_playlist;
	uint8_t playlist_byte = (uint8_t)*p_playlist_byte;
	uint8_t next_song = 0;
	uint16_t cleared_player = 0, handler = 0;
	uint16_t saved_playlist = 0, saved_player = 0;

	z80_block(45);
	saved_playlist = playlist;
	playlist_byte &= 0xFu;
	handler = playlist_command_table_ADDRESS;
	handler = read_table_word(handler, playlist_byte);
	z80_block(4);
	switch (handler)
	{
	case 0x0046B4u:
		z80_block(57);
		playlist = z80_read16(saved_playlist);
		next_song = PLAYLIST_BYTES(playlist)[0];
		playlist++;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046BCu:
		z80_block(53);
		playlist = saved_playlist;
		next_song = (playlist_song_choice_1 & 7u) | 0x90u;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046C5u:
		z80_block(53);
		playlist = saved_playlist;
		next_song = (playlist_song_choice_2 & 7u) | 0x90u;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	case 0x0046CEu:
		break;
	case 0x0046DFu:
		z80_block(32);
		if (queue_pointer >> 8 == 0xE2u)
		{
			z80_block(57);
			next_song = 0u;
			spare_players = 0u;
			move_block(spare_players_ADDRESS, 0xF101u, 0x4FFu, 1u);
		}
		else
		{
			z80_block(67);
			next_song = 0u;
			music_player_flags = 0u;
			move_block(SEQUENCE_PLAYER_ADDRESS(0), 0xE901u, 0x7FFu, 1u);
		}
		z80_block(30);
		playlist = saved_playlist;
		*p_playlist = playlist;
		*p_playlist_byte = next_song;
		return;
	default:
		sound_invalid_jump(handler);
	}
	z80_block(117);
	saved_player = player;
	cleared_player = saved_player;
	next_song = 0u;
	SEQUENCE_PLAYER(cleared_player)->flags = 0u;
	move_block(cleared_player, (uint16_t)(cleared_player + 1u), 0x30u, 1u);
	playlist = saved_playlist;
	*p_playlist = playlist;
	*p_playlist_byte = next_song;
}

/* Runs step_player for the player at player_address when its flags bit 7 (playing) is set. */
void step_player_if_playing(uint16_t player_address, uint16_t mailbox_page)
{
	z80_block(17);
	if (SEQUENCE_PLAYER(player_address)->flags >> 7 == 0u)
	{
		z80_cycles(6);
		return;
	}
	step_player(player_address, mailbox_page);
}

/* Counts delta_time down; at 0 copies the player's events (through the bank window) into the MIDI queue
   at midi_queue_write until one has a delta time after it: note offs get velocity 0x7F, controller 0x10
   goes through fade_volume, sysex is skipped but counted. FF 2F or a status F1-F6/F8-FE ends it:
   playlist (flags bit 5) or stop. */
void step_player(uint16_t player_address, uint16_t mailbox_page)
{
	uint32_t carry = 0;
	uint8_t delta_time_high = 0, delta_time_low = 0, event_byte = 0, meta_bytes_left = 0, new_delta_high = 0, queue_page = 0, queued_byte = 0, sysex_bytes_left = 0, volume_value = 0, write_index = 0;
	uint16_t cursor = 0, player = 0, player_2 = 0, queue_pointer = 0;
	uint8_t saved_queue_page = 0, saved_write_index = 0;
	uint16_t player_then_queue_pointer = 0, saved_player = 0;
	uint8_t controller_number = 0;
	uint16_t delta_time = 0;

	z80_block(64);
	cursor = (player_address & ~0xFFu) | 6u;
	delta_time_low = SEQUENCE_PLAYER(cursor - 0x6u)->delta_time_low;
	cursor = (cursor & ~0xFFu) | (uint8_t)(cursor + 1u);
	delta_time = (((uint32_t)SEQUENCE_PLAYER(cursor - 0x7u)->delta_time_high << 8) | delta_time_low) - 1u;
	delta_time_high = (uint8_t)(delta_time >> 8);
	delta_time_low = (uint8_t)delta_time;
	SEQUENCE_PLAYER(cursor - 0x7u)->delta_time_high = delta_time_high;
	cursor--;
	SEQUENCE_PLAYER(cursor - 0x6u)->delta_time_low = delta_time_low;
	if ((delta_time_high | delta_time_low) != 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(56);
	queue_page = 0xE1u;
	write_index = midi_queue_write;
	player_then_queue_pointer = cursor & ~0xFFu;
	player = player_then_queue_pointer;
	for (;;)
	{
		z80_block(83);
		port_out(ROM_BANK_HIGH, 0u);
		port_out(ROM_BANK_LOW, SEQUENCE_PLAYER(player)->bank);
		cursor = SEQUENCE_PLAYER(player)->position;
		for (;;)
		{
			z80_block(48);
			SEQUENCE_PLAYER(player)->flags &= 0xBFu;
			event_byte = z80_read8(cursor);
			if (event_byte >> 7 == 0u)
			{
				z80_block(35);
				event_byte = SEQUENCE_PLAYER(player)->running_status;
				cursor--;
			}
			else
			{
				z80_block(19);
				SEQUENCE_PLAYER(player)->running_status = event_byte;
			}
			z80_block(17);
			if (event_byte == 0xFFu)
			{
				z80_block(17);
				cursor = next_sequence_byte(cursor, player);
				z80_block(24);
				event_byte = z80_read8(cursor);
				if (event_byte == 0x2Fu)
					break;
				z80_block(17);
				if (event_byte == 0x51u)
				{
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(25);
					event_byte = z80_read8(cursor);
					if (event_byte >> 7)
					{
						z80_block(23);
						event_byte = (0u - event_byte) | 0x80u;
					}
					z80_block(36);
					SEQUENCE_PLAYER(player)->meta_51_value = event_byte;
					cursor = next_sequence_byte(cursor, player);
					z80_block(10);
				}
				else
				{
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(7);
					meta_bytes_left = z80_read8(cursor);
					for (;;)
					{
						z80_block(17);
						cursor = next_sequence_byte(cursor, player);
						z80_block(8);
						meta_bytes_left--;
						if (meta_bytes_left == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(17);
					cursor = next_sequence_byte(cursor, player);
					z80_block(10);
				}
			}
			else
			{
				z80_block(28);
				queued_byte = event_byte;
				event_byte &= ~0xFu;
				if (event_byte == 0xF0u)
				{
					z80_block(21);
					event_byte = queued_byte;
					cursor = next_sequence_byte(cursor, player);
					z80_block(17);
					if (event_byte == 0xF7u)
					{
						z80_block(7);
						sysex_bytes_left = z80_read8(cursor);
						for (;;)
						{
							z80_block(24);
							cursor = next_sequence_byte(cursor, player);
							z80_block(8);
							sysex_bytes_left--;
							if (sysex_bytes_left == 0u)
								break;
							z80_cycles(5);
						}
						z80_block(10);
						z80_block(33);
						midi_queue_count++;
					}
					else
					{
						z80_block(17);
						if (event_byte != 0xF0u)
						{
							z80_block(10);
							break;
						}
						for (;;)
						{
							z80_block(24);
							event_byte = z80_read8(cursor);
							cursor = next_sequence_byte(cursor, player);
							z80_block(17);
							if (event_byte == 0xF7u)
								break;
							z80_block(10);
						}
						z80_block(33);
						midi_queue_count++;
					}
				}
				else
				{
					z80_block(17);
					if (event_byte == 0x80u)
					{
						z80_block(32);
						z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
						write_index++;
						cursor = next_sequence_byte(cursor, player);
						z80_block(25);
						event_byte = z80_read8(cursor);
						if (event_byte >> 7 != 0u)
						{
							z80_cycles(7);
							event_byte = chain_next_event(player, event_byte);
						}
						z80_block(28);
						z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
						write_index++;
						cursor = next_sequence_byte(cursor, player);
						z80_block(51);
						z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x7Fu);
						write_index++;
						midi_queue_count++;
					}
					else
					{
						z80_block(17);
						if (event_byte == 0xB0u)
						{
							z80_block(32);
							z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
							write_index++;
							cursor = next_sequence_byte(cursor, player);
							z80_block(35);
							event_byte = z80_read8(cursor);
							z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
							write_index++;
							cursor = next_sequence_byte(cursor, player);
							z80_block(24);
							controller_number = event_byte;
							event_byte = z80_read8(cursor);
							if (controller_number == 0x10u)
							{
								z80_block(37);
								volume_value = event_byte;
								if (SEQUENCE_PLAYER(player)->fade_speed == 0u)
								{
									z80_block(31);
									event_byte = volume_value;
									SEQUENCE_PLAYER(player)->fade_volume = volume_value & 0x7Fu;
								}
								else
								{
									z80_block(29);
									event_byte = SEQUENCE_PLAYER(player)->fade_volume;
								}
							}
							z80_block(18);
							if (event_byte >> 7 != 0u)
							{
								z80_cycles(7);
								event_byte = chain_next_event(player, event_byte);
							}
							z80_block(28);
							z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
							write_index++;
							cursor = next_sequence_byte(cursor, player);
							z80_block(33);
							midi_queue_count++;
						}
						else
						{
							z80_block(17);
							if (event_byte != 0xC0u)
							{
								z80_block(17);
								if (event_byte != 0xD0u)
								{
									z80_block(10);
									z80_block(32);
									z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
									write_index++;
									cursor = next_sequence_byte(cursor, player);
									z80_block(7);
									queued_byte = z80_read8(cursor);
								}
							}
							z80_block(32);
							z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), queued_byte);
							write_index++;
							cursor = next_sequence_byte(cursor, player);
							z80_block(25);
							event_byte = z80_read8(cursor);
							if (event_byte >> 7 != 0u)
							{
								z80_cycles(7);
								event_byte = chain_next_event(player, event_byte);
							}
							z80_block(28);
							z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), event_byte);
							write_index++;
							cursor = next_sequence_byte(cursor, player);
							z80_block(33);
							midi_queue_count++;
						}
					}
				}
			}
			z80_block(30);
			if (((SEQUENCE_PLAYER(player)->flags >> 6) & 1u) != 0u)
				continue;
			z80_block(24);
			event_byte = z80_read8(cursor);
			cursor = next_sequence_byte(cursor, player);
			z80_block(14);
			if (event_byte == 0u)
				continue;
			z80_block(18);
			if (event_byte >> 7)
			{
				z80_block(37);
				event_byte &= 0x7Fu;
				carry = event_byte & 1u;
				new_delta_high = event_byte >> 1;
				event_byte = z80_read8(cursor);
				if (carry)
				{
					z80_block(8);
					event_byte |= 0x80u;
				}
				z80_block(17);
				cursor = next_sequence_byte(cursor, player);
				z80_block(19);
				SEQUENCE_PLAYER(player)->delta_time_high = new_delta_high;
			}
			z80_block(134);
			SEQUENCE_PLAYER(player)->delta_time_low = event_byte;
			SEQUENCE_PLAYER(player)->position = cursor;
			midi_queue_write = write_index;
			port_out(ROM_BANK_HIGH, rom_bank_high);
			port_out(ROM_BANK_LOW, rom_bank_low);
			return;
		}
		z80_block(30);
		if (((SEQUENCE_PLAYER(player)->flags >> 5) & 1u) == 0u)
		{
			z80_block(30);
			if (((SEQUENCE_PLAYER(player)->flags >> 4) & 1u) != 0u)
			{
				z80_cycles(7);
				clear_player_end_flag(mailbox_page);
			}
			z80_block(100);
			SEQUENCE_PLAYER(player)->flags = 0u;
			midi_queue_write = write_index;
			port_out(ROM_BANK_HIGH, rom_bank_high);
			port_out(ROM_BANK_LOW, rom_bank_low);
			return;
		}
		z80_block(62);
		cursor = SEQUENCE_PLAYER(player)->playlist;
		event_byte = PLAYLIST_BYTES(cursor)[0];
		cursor = next_sequence_byte(cursor, player);
		z80_block(17);
		if (event_byte >= 0xF0u)
		{
			z80_cycles(7);
			queue_pointer = (uint32_t)queue_page << 8;
			playlist_command(queue_pointer, player, mailbox_page, &cursor, &event_byte);
		}
		z80_block(14);
		if (event_byte == 0u)
		{
			z80_block(30);
			if (((SEQUENCE_PLAYER(player)->flags >> 4) & 1u) != 0u)
			{
				z80_cycles(7);
				clear_player_end_flag(mailbox_page);
			}
			z80_block(100);
			SEQUENCE_PLAYER(player)->flags = 0u;
			midi_queue_write = write_index;
			port_out(ROM_BANK_HIGH, rom_bank_high);
			port_out(ROM_BANK_LOW, rom_bank_low);
			return;
		}
		z80_block(91);
		SEQUENCE_PLAYER(player)->playlist = cursor;
		saved_queue_page = queue_page;
		saved_write_index = write_index;
		saved_player = player;
		player_2 = (uint32_t)(saved_player >> 8) << 8;
		start_song(player_2, event_byte);
		z80_block(20);
		queue_page = saved_queue_page;
		write_index = saved_write_index;
	}
}

/* Hands back data_byte without bit 7, which marks it as chained: the player's flags bit 6 is set so
   step_player reads the next event at once, with no delta time between them. */
uint8_t chain_next_event(uint16_t player, uint8_t data_byte)
{
	z80_block(41);
	data_byte &= 0x7Fu;
	SEQUENCE_PLAYER(player)->flags |= 0x40u;
	return data_byte;
}

/* Clears player_end_flag; step_player calls it when a player with flags bit 4 stops. Nothing in the
   sound program sets that bit or reads player_end_flag. */
void clear_player_end_flag(uint16_t mailbox_page)
{
	z80_block(33);
	player_end_flag = 0u;
}

/* Takes the V60's sound command: once sound_command_group is set, clears it and finds sound_command in
   its list (sound_groups). Header flags bit 7 clear: music on player 0 (inline sequence, song header or
   playlist); set: player 2, or with bit 6 players 3-7 by the first event's MIDI channel, if its priority
   is not below the playing sound's. */
void poll_sound_command(void)
{
	uint8_t byte = 0, command_number = 0;
	uint16_t address = 0, header = 0;
	uint16_t exchanged_player = 0;

	z80_block(26);
	address = sound_command_group_ADDRESS;
	byte = sound_command_group;
	if (byte == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(55);
	sound_command_group = 0u;
	command_number = z80_read8((address & ~0xFFu) | (uint8_t)(address - 1u));
	byte &= 0x7Fu;
	address = sound_groups_ADDRESS;
	address = read_table_word(address, byte);
	z80_block(16);
	if (SOUND_LIST(address)->last_number < command_number)
	{
		z80_cycles(6);
		return;
	}
	z80_block(37);
	byte = command_number;
	address++;
	header = SEQUENCE_PLAYER_ADDRESS(0);
	address = read_table_word(address, byte);
	z80_block(29);
	exchanged_player = header;
	header = address;
	address = exchanged_player;
	byte = SOUND_HEADER(header)->flags;
	if (byte >> 7 != 0u)
	{
		z80_block(18);
		if (((byte >> 6) & 1u) != 0u)
		{
			z80_block(29);
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
			select_channel_player(&header, &address);
			z80_block(46);
			byte = SOUND_HEADER(header)->flags & 0xFu;
			if (byte < (SEQUENCE_PLAYER(address)->flags & 0xFu))
			{
				z80_block(10);
				return;
			}
			z80_block(17);
			address = clear_player(address);
			z80_block(87);
			header++;
			SEQUENCE_PLAYER(address)->flags = byte;
			address = (address & ~0xFFu) | 2u;
			SEQUENCE_PLAYER(address - 0x2u)->position_low = (uint8_t)header;
			address = (address & ~0xFFu) | (uint8_t)(address + 1u);
			SEQUENCE_PLAYER(address - 0x3u)->position_high = header >> 8;
			address = (address & ~0xFFu) | 6u;
			SEQUENCE_PLAYER(address - 0x6u)->delta_time_low = 1u;
			address &= ~0xFFu;
			SEQUENCE_PLAYER(address)->flags |= 0x80u;
			return;
		}
		z80_block(10);
		z80_block(54);
		address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
		address = (uint32_t)(uint8_t)((address >> 8) + 1u) << 8;
		byte = SOUND_HEADER(header)->flags & 0xFu;
		if (byte < (SEQUENCE_PLAYER(address)->flags & 0xFu))
		{
			z80_block(10);
			return;
		}
		z80_block(17);
		address = clear_player(address);
		z80_block(7);
		SEQUENCE_PLAYER(address)->flags = byte;
		z80_block(80);
		header++;
		address = (address & ~0xFFu) | 2u;
		SEQUENCE_PLAYER(address - 0x2u)->position_low = (uint8_t)header;
		address = (address & ~0xFFu) | (uint8_t)(address + 1u);
		SEQUENCE_PLAYER(address - 0x3u)->position_high = header >> 8;
		address = (address & ~0xFFu) | 6u;
		SEQUENCE_PLAYER(address - 0x6u)->delta_time_low = 1u;
		address &= ~0xFFu;
		SEQUENCE_PLAYER(address)->flags |= 0x80u;
		return;
	}
	z80_block(18);
	if (((byte >> 6) & 1u) != 0u)
	{
		z80_block(80);
		header++;
		address = (address & ~0xFFu) | 2u;
		SEQUENCE_PLAYER(address - 0x2u)->position_low = (uint8_t)header;
		address = (address & ~0xFFu) | (uint8_t)(address + 1u);
		SEQUENCE_PLAYER(address - 0x3u)->position_high = header >> 8;
		address = (address & ~0xFFu) | 6u;
		SEQUENCE_PLAYER(address - 0x6u)->delta_time_low = 1u;
		address &= ~0xFFu;
		SEQUENCE_PLAYER(address)->flags |= 0x80u;
		return;
	}
	z80_block(18);
	if (((byte >> 5) & 1u) != 0u)
	{
		start_sequence(header, address);
		return;
	}
	z80_block(10);
	z80_block(17);
	address = clear_player(address);
	z80_block(82);
	header++;
	byte = SOUND_HEADER(header - 0x1u)->first_song;
	SEQUENCE_PLAYER(address)->flags |= 0x20u;
	header++;
	address = (address & ~0xFFu) | 0x20u;
	SEQUENCE_PLAYER(address - 0x20u)->playlist_low = (uint8_t)header;
	address++;
	SEQUENCE_PLAYER(address - 0x21u)->playlist_high = header >> 8;
	header = address & ~0xFFu;
	start_song(header, byte);
}

/* Starts song song_number of the songs table on the player at player (start_sequence). */
void start_song(uint16_t player, uint8_t song_number)
{
	uint16_t address = 0, song_header = 0;
	uint16_t exchanged_player = 0;

	z80_block(27);
	address = songs_ADDRESS;
	address = read_table_word(address, song_number);
	z80_block(21);
	exchanged_player = player;
	song_header = address;
	address = exchanged_player;
	start_sequence(song_header, address);
}

/* Starts the sequence of the song header at header on the player at player: its bank into bank, its
   address into position, delta_time 1 (the first event at the next step) and flags bit 7 (playing). */
void start_sequence(uint16_t header, uint16_t player)
{
	uint8_t bank = 0;
	uint16_t player_field = 0;

	z80_block(127);
	header++;
	player_field = (player & ~0xFFu) | 0x1Bu;
	bank = SONG_RECORD(header - 0x1u)->bank;
	header++;
	SEQUENCE_PLAYER(player_field - 0x1Bu)->bank = bank;
	player_field = (player_field & ~0xFFu) | 2u;
	SEQUENCE_PLAYER(player_field - 0x2u)->position_low = SONG_RECORD(header - 0x2u)->sequence_low;
	player_field = (player_field & ~0xFFu) | (uint8_t)(player_field + 1u);
	SEQUENCE_PLAYER(player_field - 0x3u)->position_high = SONG_RECORD(header - 0x2u)->sequence_high;
	player_field = (player_field & ~0xFFu) | 6u;
	SEQUENCE_PLAYER(player_field - 0x6u)->delta_time_low = 1u;
	player_field &= ~0xFFu;
	SEQUENCE_PLAYER(player_field)->flags |= 0x80u;
}

/* Unused: of the byte at player_field and the same byte one and two pages on (the same field of the next
   two sequence players), hands back the address of the lowest, the first of equal ones, also kept in
   lowest_player_field. */
uint16_t find_lowest_player(uint16_t player_field)
{
	uint8_t byte = 0, lowest = 0, players_left = 0;

	z80_block(30);
	players_left = 2u;
	lowest = z80_read8(player_field);
	lowest_player_field = player_field;
	for (;;)
	{
		z80_block(25);
		player_field = (player_field & 0xFFu) | ((uint32_t)(uint8_t)((player_field >> 8) + 1u) << 8);
		byte = z80_read8(player_field);
		if (byte < lowest)
		{
			z80_block(20);
			lowest = byte;
			lowest_player_field = player_field;
		}
		z80_block(8);
		players_left--;
		if (players_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(26);
	player_field = lowest_player_field;
	return player_field;
}

/* Picks the player for a sound by the MIDI channel of its first event (the low nibble of the header's
   second byte): player's page plus that channel - 0xA, so channels 0xA-0xE take players 3-7 from player
   3's page. */
void select_channel_player(uint16_t *p_header, uint16_t *p_player)
{
	uint16_t header = (uint16_t)*p_header;
	uint16_t player = (uint16_t)*p_player;
	uint8_t player_page = 0;

	z80_block(55);
	header++;
	player_page = (EFFECT_HEADER(header - 0x1u)->first_status & 0xFu) - 0xAu + (player >> 8);
	header--;
	player = (uint32_t)player_page << 8;
	*p_header = header;
	*p_player = (*p_player & 0xFFu) | player;
}

/* Zeroes the first 0x31 bytes of the sequence player at player, which stops it, and hands back its
   address. */
uint16_t clear_player(uint16_t player)
{
	uint16_t saved_player = 0;

	z80_block(101);
	saved_player = player;
	SEQUENCE_PLAYER(player)->flags = 0u;
	move_block(player, (uint16_t)(saved_player + 1u), 0x30u, 1u);
	player = (player + 0x30u) & 0xFF00u;
	return player;
}

/* The music player's fade: each time music_fade_wait runs down (reloaded from music_fade_speed) its
   fade_volume drops by one and goes out as controller 0x10 on channel 0xE (BE 10 v); at 0 it stops the
   player, clears music_fade_speed and sends BE 50 7E and BE 79 00. Nothing in the sound program sets
   music_fade_speed. */
void fade_music_volume(uint16_t mailbox_page)
{
	uint8_t queue_page = 0, write_index = 0;
	uint16_t fade_field = 0;

	z80_block(28);
	fade_field = music_fade_wait_ADDRESS;
	if (music_fade_wait != 0u)
	{
		z80_block(21);
		SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait--;
		return;
	}
	z80_cycles(5);
	z80_block(63);
	queue_page = 0xE1u;
	write_index = midi_queue_write;
	fade_field = (fade_field & ~0xFFu) | 0x15u;
	SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume--;
	if (SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume == 0u)
	{
		z80_block(223);
		fade_field &= ~0xFFu;
		SEQUENCE_PLAYER(fade_field)->flags &= 0x7Fu;
		SEQUENCE_PLAYER(fade_field)->fade_speed = 0u;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x50u);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x7Eu);
		write_index++;
		midi_queue_count++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x79u);
		write_index++;
		z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0u);
		midi_queue_count++;
		midi_queue_write = write_index + 1u;
		return;
	}
	z80_block(134);
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0xBEu);
	write_index++;
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), 0x10u);
	write_index++;
	z80_write8((uint16_t)(((uint32_t)queue_page << 8) | write_index), SEQUENCE_PLAYER(fade_field - 0x15u)->fade_volume);
	midi_queue_write = write_index + 1u;
	midi_queue_count++;
	fade_field = (fade_field & ~0xFFu) | 0x18u;
	SEQUENCE_PLAYER(fade_field & ~0xFFu)->fade_wait = SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed;
}

/* The fade of the sequence player at player: each time fade_wait runs down (reloaded from fade_speed)
   fade_channel_volume drops by one and goes out as controller 7 on channel 0xA (BA 07 v); at 0 it stops
   the player and sends BA 7B 00 (all notes off). Only while fade_speed is set, which nothing in the
   sound program does. */
void fade_player_volume(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page)
{
	uint8_t write_index = 0;
	uint16_t fade_field = 0;

	write_index = (uint8_t)queue_pointer;
	z80_block(23);
	fade_field = (player & ~0xFFu) | 0x18u;
	if (SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	fade_field = (fade_field & ~0xFFu) | 0x19u;
	if (SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait != 0u)
	{
		z80_block(21);
		SEQUENCE_PLAYER(fade_field - 0x19u)->fade_wait--;
		return;
	}
	z80_block(39);
	fade_field = (fade_field & ~0xFFu) | 0x1Au;
	SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume--;
	if (SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume == 0u)
	{
		z80_block(138);
		fade_field &= ~0xFFu;
		SEQUENCE_PLAYER(fade_field)->flags = 0u;
		SEQUENCE_PLAYER(fade_field)->fade_speed = 0u;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0xBAu);
		write_index++;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0x7Bu);
		write_index++;
		z80_write8((queue_pointer & ~0xFFu) | write_index, 0u);
		midi_queue_count++;
		midi_queue_write = write_index + 1u;
		return;
	}
	z80_block(134);
	z80_write8((queue_pointer & ~0xFFu) | write_index, 0xBAu);
	write_index++;
	z80_write8((queue_pointer & ~0xFFu) | write_index, 7u);
	write_index++;
	z80_write8((queue_pointer & ~0xFFu) | write_index, SEQUENCE_PLAYER(fade_field - 0x1Au)->fade_channel_volume);
	midi_queue_write = write_index + 1u;
	midi_queue_count++;
	fade_field = (fade_field & ~0xFFu) | 0x18u;
	SEQUENCE_PLAYER(fade_field & ~0xFFu)->fade_wait = SEQUENCE_PLAYER(fade_field - 0x18u)->fade_speed;
}

/* Hands back cursor + 1; past the bank window (0xC000) it maps the player's next bank (bank + 1, out to
   ROM_BANK_LOW) and goes on from 0xA000. */
uint16_t next_sequence_byte(uint16_t cursor, uint16_t player)
{
	z80_block(38);
	cursor++;
	if (cursor >> 8 == 0xC0u)
	{
		z80_block(60);
		SEQUENCE_PLAYER(player)->bank++;
		port_out(ROM_BANK_LOW, SEQUENCE_PLAYER(player)->bank);
		cursor = (cursor & 0xFFu) | 0xA000u;
	}
	z80_block(20);
	return cursor;
}
