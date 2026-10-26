/**
 *    Name:        voices.c
 *    Description: the voices: the RF5C68's sample voices (starting a sample, refilling its pages, stopping it) and the YM3438's FM voices (note off, release with the sustain pedal, volumes, patches, the vibrato)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    26-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

/* Starts sample sample_or_patch on voice: register 8 with only pcm_channels_on sounding, the sample's
   ROM bank from pcm_sample_headers (kept in the voice, set on the bank ports), its first 256 bytes
   copied to the voice's wave_page; wave_page and source_page move on (past the bank window, into the
   next bank). Header bytes 3-5 are kept, never read. */
void start_pcm_sample(uint16_t voice)
{
	uint8_t bank_high = 0, bank_low = 0, channels_off = 0, next_wave_page = 0, pcm_register = 0, sample = 0, wave_page = 0;
	uint16_t header = 0, sample_address = 0, wave_address = 0;
	uint8_t saved_start_page = 0, saved_start_offset = 0;

	z80_block(52);
	channels_off = ~pcm_channels_on;
	pcm_register = 8u;
	write_pcm_register(pcm_register, channels_off);
	z80_block(40);
	sample = sample_or_patch;
	header = pcm_sample_headers_ADDRESS;
	header = read_table_word(header, sample);
	z80_block(293);
	bank_high = PCM_SAMPLE_HEADER(header)->bank_high;
	SAMPLE_VOICE(voice)->sample_bank_high = bank_high;
	bank_high &= 7u;
	rom_bank_high = bank_high;
	port_out(ROM_BANK_HIGH, bank_high);
	header++;
	bank_low = PCM_SAMPLE_HEADER(header - 0x1u)->bank_low;
	SAMPLE_VOICE(voice)->sample_bank_low = bank_low;
	rom_bank_low = bank_low;
	port_out(ROM_BANK_LOW, bank_low);
	wave_page = SAMPLE_VOICE(voice)->wave_page;
	header++;
	saved_start_page = PCM_SAMPLE_HEADER(header - 0x2u)->start_page;
	saved_start_offset = 0u;
	header++;
	SAMPLE_VOICE(voice)->header_bank_step = PCM_SAMPLE_HEADER(header - 0x3u)->bank_step;
	header++;
	SAMPLE_VOICE(voice)->header_address_low = PCM_SAMPLE_HEADER(header - 0x4u)->address_low;
	SAMPLE_VOICE(voice)->header_address_high = PCM_SAMPLE_HEADER(header - 0x4u)->address_high;
	sample_address = ((uint32_t)saved_start_page << 8) | saved_start_offset;
	wave_address = (uint32_t)wave_page << 8;
	copy_sample_page(&wave_address, &sample_address);
	next_wave_page = (uint8_t)(wave_address >> 8);
	z80_block(40);
	SAMPLE_VOICE(voice)->wave_page = next_wave_page;
	if (0xC0u == sample_address >> 8)
	{
		z80_block(30);
		sample_address = ROM_BANK_WINDOW;
		SAMPLE_VOICE(voice)->sample_bank_low++;
	}
	z80_block(39);
	SAMPLE_VOICE(voice)->source_page = sample_address >> 8;
}

/* Streams one sample voice a pass (refill_voice, the next of the 8 each time): a sounding one adds
   pitch_step to stream_position, and when that passes 0x800 copies 2 pages (256 bytes each) of its
   sample into its wave RAM, past 0x1000 4, wrapping the wave pages at 0xE0 and the bank window at
   0xC000; a page ending in 0xFF ends the sample (stop_pcm_voice). */
void refill_pcm_voice(void)
{
	uint8_t bank_high = 0, bank_low = 0, four_pages = 0, wave_offset = 0, wave_page = 0;
	uint16_t progress = 0, sample_address = 0, voice = 0, voices_left_at = 0, wave_address = 0;
	uint8_t old_voices_left = 0;

	z80_block(50);
	voice = refill_voice;
	if (((SAMPLE_VOICE(voice)->flags >> 6) & 1u) == 0u)
	{
		z80_block(53);
		voice += 0x12u;
		voices_left_at = refill_voices_left_ADDRESS;
		old_voices_left = refill_voices_left;
		refill_voices_left = old_voices_left - 1u;
		if (old_voices_left != 1u)
		{
			z80_block(30);
			refill_voice = voice;
			return;
		}
		z80_cycles(5);
		z80_block(46);
		refill_voices_left = 8u;
		refill_voice = SAMPLE_VOICE_ADDRESS(0);
		return;
	}
	z80_block(105);
	progress = SAMPLE_VOICE(voice)->stream_position + SAMPLE_VOICE(voice)->pitch_step;
	if ((((progress >> 8) >> 4) & 1u) == 0u)
	{
		z80_block(25);
		four_pages = 0u;
		if ((((progress >> 8) >> 3) & 1u) == 0u)
		{
			z80_block(48);
			SAMPLE_VOICE(voice)->stream_position = progress;
			z80_block(53);
			voice += 0x12u;
			voices_left_at = refill_voices_left_ADDRESS;
			old_voices_left = refill_voices_left;
			refill_voices_left = old_voices_left - 1u;
			if (old_voices_left != 1u)
			{
				z80_block(30);
				refill_voice = voice;
				return;
			}
			z80_cycles(5);
			z80_block(46);
			refill_voices_left = 8u;
			refill_voice = SAMPLE_VOICE_ADDRESS(0);
			return;
		}
	}
	else
	{
		z80_block(25);
		progress = (progress & 0xFFu) | (((progress >> 8) & 0xEFu) << 8);
		four_pages = 1u;
	}
	z80_block(252);
	progress = (progress & 0xFFu) | (((progress >> 8) & ~8u) << 8);
	SAMPLE_VOICE(voice)->stream_position = progress;
	z80_write8(PCM_CONTROL, (SAMPLE_VOICE(voice)->chip_channel * 2u) | 0x80u);
	bank_high = SAMPLE_VOICE(voice)->sample_bank_high & 7u;
	rom_bank_high = bank_high;
	port_out(ROM_BANK_HIGH, bank_high);
	bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
	rom_bank_low = bank_low;
	port_out(ROM_BANK_LOW, bank_low);
	wave_page = SAMPLE_VOICE(voice)->wave_page;
	wave_offset = 0u;
	sample_address = (uint32_t)SAMPLE_VOICE(voice)->source_page << 8;
	if (four_pages)
	{
		z80_block(17);
		wave_address = ((uint32_t)wave_page << 8) | wave_offset;
		copy_sample_page(&wave_address, &sample_address);
		wave_page = (uint8_t)(wave_address >> 8);
		wave_offset = (uint8_t)wave_address;
		z80_block(30);
		sample_address--;
		if (z80_read8(sample_address) == 0xFFu)
		{
			stop_pcm_voice(voice);
			return;
		}
		z80_block(27);
		sample_address++;
		if (0xC0u == sample_address >> 8)
		{
			z80_block(73);
			sample_address = (sample_address & 0xFFu) | 0xA000u;
			SAMPLE_VOICE(voice)->sample_bank_low++;
			bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
			rom_bank_low = bank_low;
			port_out(ROM_BANK_LOW, bank_low);
		}
		z80_block(21);
		if (0xE0u == wave_page)
		{
			z80_block(7);
			wave_page = 0xD0u;
		}
		z80_block(17);
		wave_address = ((uint32_t)wave_page << 8) | wave_offset;
		copy_sample_page(&wave_address, &sample_address);
		wave_page = (uint8_t)(wave_address >> 8);
		wave_offset = (uint8_t)wave_address;
		z80_block(27);
		sample_address--;
		if (z80_read8(sample_address) == 0xFFu)
		{
			z80_cycles(5);
			stop_pcm_voice(voice);
			return;
		}
		z80_block(27);
		sample_address++;
		if (0xC0u == sample_address >> 8)
		{
			z80_block(73);
			sample_address = (sample_address & 0xFFu) | 0xA000u;
			SAMPLE_VOICE(voice)->sample_bank_low++;
			bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
			rom_bank_low = bank_low;
			port_out(ROM_BANK_LOW, bank_low);
		}
		z80_block(21);
		if (0xE0u == wave_page)
		{
			z80_block(7);
			wave_page = 0xD0u;
		}
	}
	z80_block(17);
	wave_address = ((uint32_t)wave_page << 8) | wave_offset;
	copy_sample_page(&wave_address, &sample_address);
	wave_page = (uint8_t)(wave_address >> 8);
	wave_offset = (uint8_t)wave_address;
	z80_block(27);
	sample_address--;
	if (z80_read8(sample_address) == 0xFFu)
	{
		z80_cycles(5);
		stop_pcm_voice(voice);
		return;
	}
	z80_block(27);
	sample_address++;
	if (0xC0u == sample_address >> 8)
	{
		z80_block(73);
		sample_address = (sample_address & 0xFFu) | 0xA000u;
		SAMPLE_VOICE(voice)->sample_bank_low++;
		bank_low = SAMPLE_VOICE(voice)->sample_bank_low;
		rom_bank_low = bank_low;
		port_out(ROM_BANK_LOW, bank_low);
	}
	z80_block(21);
	if (0xE0u == wave_page)
	{
		z80_block(7);
		wave_page = 0xD0u;
	}
	z80_block(17);
	wave_address = ((uint32_t)wave_page << 8) | wave_offset;
	copy_sample_page(&wave_address, &sample_address);
	wave_page = (uint8_t)(wave_address >> 8);
	z80_block(27);
	sample_address--;
	if (z80_read8(sample_address) == 0xFFu)
	{
		z80_cycles(5);
		stop_pcm_voice(voice);
		return;
	}
	z80_block(27);
	sample_address++;
	if (0xC0u == sample_address >> 8)
	{
		z80_block(30);
		sample_address = ROM_BANK_WINDOW;
		SAMPLE_VOICE(voice)->sample_bank_low++;
	}
	z80_block(21);
	if (0xE0u == wave_page)
	{
		z80_block(7);
		wave_page = 0xD0u;
	}
	z80_block(38);
	SAMPLE_VOICE(voice)->wave_page = wave_page;
	SAMPLE_VOICE(voice)->source_page = sample_address >> 8;
	z80_block(53);
	voice += 0x12u;
	voices_left_at = refill_voices_left_ADDRESS;
	old_voices_left = refill_voices_left;
	refill_voices_left = old_voices_left - 1u;
	if (old_voices_left != 1u)
	{
		z80_block(30);
		refill_voice = voice;
		return;
	}
	z80_cycles(5);
	z80_block(46);
	refill_voices_left = 8u;
	refill_voice = SAMPLE_VOICE_ADDRESS(0);
}

/* Stops a sample voice: its RF5C68 channel's loop (register 5) to channel * 0x20 + 0x18, a stretch
   init_pcm_chip leaves silent; the voice freed (flags and midi_channel 0, wave_page 0xD0,
   stream_position 0x7FF), its bit out of pcm_channels_on, pcm_voices_used - 1, and the higher
   allocation_orders lowered (unless its own is 0x10 or more). */
void stop_pcm_voice(uint16_t voice)
{
	uint8_t own_order = 0, pcm_register = 0, register_value = 0, voices_left = 0;
	uint16_t other_order = 0, voice_size = 0;

	z80_block(50);
	register_value = SAMPLE_VOICE(voice)->chip_channel | 0xC0u;
	pcm_register = 7u;
	write_pcm_register(pcm_register, register_value);
	z80_block(72);
	register_value = ((register_value & 7u) * 2u) | 0x80u;
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
	register_value = ((((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u) + 0x18u;
	pcm_register = 5u;
	write_pcm_register(pcm_register, register_value);
	z80_block(227);
	SAMPLE_VOICE(voice)->flags = 0u;
	pcm_channels_on ^= pcm_channel_bits[SAMPLE_VOICE(voice)->chip_channel];
	SAMPLE_VOICE(voice)->midi_channel = 0u;
	SAMPLE_VOICE(voice)->wave_page = 0xD0u;
	SAMPLE_VOICE(voice)->stream_position_low = 0xFFu;
	SAMPLE_VOICE(voice)->stream_position_high = 7u;
	pcm_voices_used--;
	if (SAMPLE_VOICE(voice)->allocation_order >= 0x10u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(46);
	other_order = voice_0_allocation_order_ADDRESS;
	voice_size = 0x12u;
	own_order = SAMPLE_VOICE(voice)->allocation_order;
	voices_left = 8u;
	for (;;)
	{
		z80_block(17);
		if (own_order < SAMPLE_VOICE(other_order - 0x6u)->allocation_order)
		{
			z80_block(11);
			SAMPLE_VOICE(other_order - 0x6u)->allocation_order--;
		}
		z80_block(19);
		other_order += voice_size;
		voices_left--;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
}

/* Note off for an FM program: the sounding fm_voice on midi_status's channel playing note_code, unless
   held already, is stopped (stop_fm_voice), or held (flags bit 1) while the channel's sustain pedal is
   down. de, saved_channel and a come back as the original's registers (&channel_controls[channel] after
   a match): CC 0x7B/0x80's loop walks on from them. */
void fm_note_off(uint16_t *p_voice_size, uint16_t *p_saved_channel, uint8_t *p_channel)
{
	uint16_t saved_channel = (uint16_t)*p_saved_channel;
	uint8_t channel = 0, flags = 0, voices_left = 0;
	uint16_t voice = 0, voice_size = 0;

	z80_block(51);
	voice = FM_VOICE_ADDRESS(0);
	voice_size = 0x12u;
	voices_left = 0xCu;
	channel = midi_status & 0xFu;
	for (;;)
	{
		z80_block(29);
		if (channel == FM_VOICE(voice)->midi_channel)
		{
			z80_block(46);
			saved_channel = (saved_channel & ~0xFFu) | channel;
			if (note_code == FM_VOICE(voice)->note)
			{
				z80_block(37);
				flags = FM_VOICE(voice)->flags;
				if (((flags >> 1) & 1u) == 0u)
				{
					z80_block(17);
					if ((flags & 0x7Fu) != 0u)
						break;
				}
			}
			z80_block(4);
			channel = (uint8_t)saved_channel;
		}
		z80_block(23);
		voice += voice_size;
		voices_left--;
		if (voices_left == 0u)
		{
			z80_block(10);
			*p_voice_size = voice_size;
			*p_saved_channel = saved_channel;
			*p_channel = channel;
			return;
		}
		z80_cycles(5);
	}
	z80_block(65);
	channel = (uint8_t)saved_channel;
	voice_size = channel;
	saved_channel = channel_controls_ADDRESS + voice_size;
	if (z80_read8(saved_channel) >> 7 == 0u)
		channel = stop_fm_voice(voice);
	else
	{
		z80_block(33);
		FM_VOICE(voice)->flags |= 2u;
	}
	*p_voice_size = voice_size;
	*p_saved_channel = saved_channel;
	*p_channel = channel;
}

/* Frees the fm_voice voice (flags, midi_channel and its vibrato state 0) and keys its YM3438 channel off
   (register 0x28); hands back what write_fm_global does. */
uint8_t stop_fm_voice(uint16_t voice)
{
	uint8_t fm_register = 0, register_value = 0;

	z80_block(212);
	FM_VOICE(voice)->flags = 0u;
	FM_VOICE(voice)->midi_channel = 0u;
	FM_VOICE(voice)->vibrato_range = 0u;
	FM_VOICE(voice)->vibrato_depth = 0u;
	FM_VOICE(voice)->vibrato_offset_low = 0u;
	FM_VOICE(voice)->vibrato_offset_high = 0u;
	FM_VOICE(voice)->vibrato_block = 0u;
	FM_VOICE(voice)->vibrato_phase = 0u;
	register_value = FM_VOICE(voice)->chip_channel;
	fm_channel = register_value;
	register_value &= 7u;
	fm_register = 0x28u;
	return write_fm_global(fm_register, register_value);
}

/* The sustain pedal let go for an fm_voice held at its note off: flags and midi_channel 0 and its
   channel keyed off (register 0x28 of the YM3438 chip_channel bit 3 picks); unlike stop_fm_voice it
   leaves the vibrato fields as they are. */
void release_held_fm_voice(uint16_t voice)
{
	uint8_t fm_register = 0, key_on_register = 0, register_value = 0;

	z80_block(99);
	FM_VOICE(voice)->flags = 0u;
	FM_VOICE(voice)->midi_channel = 0u;
	key_on_register = 0x28u;
	register_value = FM_VOICE(voice)->chip_channel;
	fm_channel = register_value;
	if (((register_value >> 3) & 1u) == 0u)
	{
		z80_block(17);
		register_value &= 7u;
		fm_register = key_on_register;
		write_fm1(fm_register, register_value);
		return;
	}
	z80_block(17);
	register_value &= 7u;
	fm_register = key_on_register;
	write_fm2(fm_register, register_value);
}

/* CC 0x10 for one sounding FM voice of part (level is the address of the voice's chip_channel byte):
   note_level becomes 0x7F less half of the voice's level scaled by part's effective_volume (taken
   halfway to 0xFF); each of the 4 operator levels of its instrument's fm_patches entry with bit 7 clear
   is added to it and written as that operator's total level. */
void apply_fm_music_volume(uint8_t chip_channel, uint16_t level, uint16_t part)
{
	uint8_t channel_code = 0, effective_volume = 0, fm_register = 0, level_register = 0, operator_level = 0, operators_left = 0, part_channel = 0, patch = 0, program = 0, register_number = 0, volume_curve = 0;
	uint16_t instrument = 0, patch_levels = 0, register_list = 0;
	uint8_t saved_operators_left = 0, saved_part_channel = 0;

	z80_block(104);
	channel_code = (uint8_t)chip_channel;
	fm_channel = channel_code;
	part_channel = channel_code & 3u;
	level = ((level + 4u) & 0xFFu) | ((uint32_t)SAMPLE_VOICE(level - 0x1u)->level << 8);
	effective_volume = MIDI_PART(part)->effective_volume;
	volume_curve = effective_volume;
	if (effective_volume != 0u)
	{
		z80_block(20);
		volume_curve += (uint8_t)~effective_volume >> 1;
	}
	z80_block(17);
	level = multiply_bytes(volume_curve, level);
	z80_block(78);
	note_level = 0x7Fu - ((uint16_t)((uint32_t)((level >> 8) >> 1) << 8) >> 8);
	program = MIDI_PART(part)->program;
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(40);
	patch = INSTRUMENT(instrument)->sample_or_fm_patch;
	patch_levels = fm_patches_ADDRESS;
	patch_levels = read_table_word(patch_levels, patch);
	z80_block(38);
	patch_levels += 0x15u;
	operators_left = 4u;
	register_list = fm_level_registers_ADDRESS;
	for (;;)
	{
		z80_block(63);
		saved_operators_left = operators_left;
		saved_part_channel = part_channel;
		register_number = FM_REGISTER_NUMBERS(register_list)[0] + part_channel;
		register_list++;
		level_register = register_number;
		operator_level = FM_PATCH_BYTES(patch_levels)[0];
		patch_levels++;
		if (operator_level >> 7 == 0u)
		{
			z80_block(38);
			operator_level += note_level;
			fm_register = level_register;
			write_fm_channel(fm_register, operator_level);
		}
		z80_block(18);
		part_channel = saved_part_channel;
		operators_left = saved_operators_left - 1u;
		if (operators_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(20);
}

/* CC 7 for one sounding FM voice of part, the same as apply_fm_music_volume does for CC 0x10: from the
   voice's level (level is the address of its chip_channel byte) and part's effective_volume a new
   note_level, added to the patch's operator levels that have bit 7 clear and written as their total
   levels. */
void apply_fm_channel_volume(uint8_t chip_channel, uint16_t level, uint16_t part)
{
	uint8_t channel_code = 0, effective_volume = 0, fm_register = 0, level_register = 0, operator_level = 0, operators_left = 0, part_channel = 0, patch = 0, program = 0, register_number = 0, volume_curve = 0;
	uint16_t instrument = 0, patch_levels = 0, register_list = 0;
	uint8_t saved_operators_left = 0, saved_part_channel = 0;

	z80_block(104);
	channel_code = (uint8_t)chip_channel;
	fm_channel = channel_code;
	part_channel = channel_code & 3u;
	level = ((level + 4u) & 0xFFu) | ((uint32_t)SAMPLE_VOICE(level - 0x1u)->level << 8);
	effective_volume = MIDI_PART(part)->effective_volume;
	volume_curve = effective_volume;
	if (effective_volume != 0u)
	{
		z80_block(20);
		volume_curve += (uint8_t)~effective_volume >> 1;
	}
	z80_block(17);
	level = multiply_bytes(volume_curve, level);
	z80_block(78);
	note_level = 0x7Fu - ((uint16_t)((uint32_t)((level >> 8) >> 1) << 8) >> 8);
	program = MIDI_PART(part)->program;
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(40);
	patch = INSTRUMENT(instrument)->sample_or_fm_patch;
	patch_levels = fm_patches_ADDRESS;
	patch_levels = read_table_word(patch_levels, patch);
	z80_block(38);
	patch_levels += 0x15u;
	operators_left = 4u;
	register_list = fm_level_registers_ADDRESS;
	for (;;)
	{
		z80_block(63);
		saved_operators_left = operators_left;
		saved_part_channel = part_channel;
		register_number = FM_REGISTER_NUMBERS(register_list)[0] + part_channel;
		register_list++;
		level_register = register_number;
		operator_level = FM_PATCH_BYTES(patch_levels)[0];
		patch_levels++;
		if (operator_level >> 7 == 0u)
		{
			z80_block(38);
			operator_level += note_level;
			fm_register = level_register;
			write_fm_channel(fm_register, operator_level);
		}
		z80_block(18);
		part_channel = saved_part_channel;
		operators_left = saved_operators_left - 1u;
		if (operators_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(20);
}

/* Writes the 21 registers of fm_patches[sample_or_patch] (their numbers from fm_patch_registers plus the
   channel within its part) to the YM3438 channel in fm_channel. Hands back channel_in_part (that channel
   number), register_list (now at fm_level_registers) and patch_data (at the patch's 4 operator levels)
   for the note on's level writes. */
void load_fm_patch(uint16_t *p_channel_in_part, uint16_t *p_register_list, uint16_t *p_patch_data)
{
	uint8_t fm_register = 0, part_channel = 0, patch = 0, patch_byte = 0, patch_register = 0, register_number = 0, registers_left = 0;
	uint16_t channel_in_part = 0, patch_data = 0, register_list = 0;
	uint8_t saved_registers_left = 0, saved_part_channel = 0;

	z80_block(64);
	part_channel = fm_channel & 3u;
	patch = sample_or_patch;
	patch_data = fm_patches_ADDRESS;
	patch_data = read_table_word(patch_data, patch);
	z80_block(17);
	register_list = fm_patch_registers_ADDRESS;
	registers_left = 0x15u;
	for (;;)
	{
		z80_block(62);
		saved_registers_left = registers_left;
		saved_part_channel = part_channel;
		register_number = FM_REGISTER_NUMBERS(register_list)[0] + part_channel;
		register_list++;
		patch_register = register_number;
		patch_byte = FM_PATCH_BYTES(patch_data)[0];
		patch_data++;
		fm_register = patch_register;
		write_fm_channel(fm_register, patch_byte);
		z80_block(18);
		part_channel = saved_part_channel;
		registers_left = saved_registers_left - 1u;
		if (registers_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	channel_in_part = ((uint32_t)registers_left << 8) | part_channel;
	*p_channel_in_part = channel_in_part;
	*p_register_list = register_list;
	*p_patch_data = patch_data;
}

/* One FM voice's software vibrato: vibrato_delay counts down first, then vibrato_wait (from
   vibrato_rate) between moves; each move takes vibrato_offset up by vibrato_rise or down by vibrato_fall
   (vibrato_phase bit 0) around vibrato_center, the note's F-number with pitch_bend, turning at the depth
   (vibrato_range, vibrato_depth), shifting vibrato_block; then registers A4/A0. */
void step_fm_vibrato(uint16_t voice)
{
	bool fall_block_check = false;
	uint8_t center_high = 0, center_low = 0, fall_block = 0, fall_limit = 0, fm_register = 0, rise_block = 0, rise_limit = 0, value = 0;
	uint16_t fnumber = 0, frequency = 0;
	uint8_t saved_center_high = 0, saved_center_low = 0;
	uint16_t word_copy = 0;
	uint8_t fall_limit_bits = 0;
	uint16_t fall_fnumber = 0, center_for_fall = 0, fall_floor = 0, rise_fnumber = 0, center_for_rise = 0, rise_ceiling = 0;

	z80_block(33);
	if (FM_VOICE(voice)->vibrato_delay != 0u)
	{
		z80_block(33);
		FM_VOICE(voice)->vibrato_delay--;
		return;
	}
	z80_block(33);
	if (FM_VOICE(voice)->vibrato_wait != 0u)
	{
		z80_block(33);
		FM_VOICE(voice)->vibrato_wait--;
		return;
	}
	z80_block(305);
	FM_VOICE(voice)->vibrato_wait = FM_VOICE(voice)->vibrato_rate;
	fm_channel = FM_VOICE(voice)->chip_channel;
	value = FM_VOICE(voice)->note;
	fnumber = fm_fnum_tables_ADDRESS + (uint8_t)((value & 0xFu) * 2u);
	frequency = pitch_bend;
	fnumber = FM_FNUMBER_RUNS(fnumber)[0] + frequency + frequency;
	fnumber = FM_FNUMBERS(fnumber)[0];
	vibrato_center = fnumber;
	fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((value & 0x70u) >> 1) + (fnumber >> 8)) << 8);
	if ((FM_VOICE(voice)->vibrato_phase & 1u) != 0u)
	{
		z80_block(214);
		frequency = fnumber;
		fnumber = FM_VOICE(voice)->vibrato_offset - FM_VOICE(voice)->vibrato_fall;
		FM_VOICE(voice)->vibrato_offset = fnumber;
		fnumber += (frequency & 0xFFu) | (((frequency >> 8) & 7u) << 8);
		fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & ~7u) + (fnumber >> 8)) << 8);
		if ((FM_VOICE(voice)->vibrato_range & 0x70u) != 0u)
		{
			z80_block(83);
			word_copy = fnumber;
			fall_fnumber = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = word_copy;
			if (fall_fnumber >= 0x269u)
			{
				z80_block(94);
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
				frequency = (fm_channel & 3u) << 8;
				value = (uint8_t)(fnumber >> 8);
				fm_register = 0xA4u | (frequency >> 8);
				write_fm_channel(fm_register, value);
				z80_block(36);
				value = (uint8_t)fnumber;
				fm_register = 0xA0u | (frequency >> 8);
				write_fm_channel(fm_register, value);
				z80_block(10);
				return;
			}
			z80_block(304);
			frequency = fnumber;
			center_for_fall = vibrato_center;
			center_high = (uint8_t)(center_for_fall >> 8);
			center_low = (uint8_t)center_for_fall;
			fnumber = 0x4CFu - (((uint32_t)center_high << 8) | center_low);
			FM_VOICE(voice)->vibrato_offset = fnumber;
			saved_center_high = center_high;
			saved_center_low = center_low;
			fnumber = (((uint32_t)saved_center_high << 8) | saved_center_low) + FM_VOICE(voice)->vibrato_offset;
			fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & 0x38u) + (fnumber >> 8)) << 8);
			frequency = ((fnumber >> 8) & 0x38u) << 8;
			fall_block = (FM_VOICE(voice)->vibrato_block & ~7u) + (frequency >> 8);
			value = 0u - ((FM_VOICE(voice)->vibrato_range & 0x70u) >> 1) + (frequency >> 8);
			fall_limit = value;
			fall_limit_bits = value & ~7u;
			value = fall_limit_bits;
			fall_block_check = fall_limit_bits == 0u;
			if (!fall_block_check)
			{
				z80_block(17);
				fall_block_check = (value & 0xC0u) != 0u;
				if (!fall_block_check)
				{
					z80_block(18);
					if (fall_limit < fall_block)
					{
						z80_block(10);
						z80_block(59);
						FM_VOICE(voice)->vibrato_block -= 8u;
						z80_block(94);
						fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
						frequency = (fm_channel & 3u) << 8;
						value = (uint8_t)(fnumber >> 8);
						fm_register = 0xA4u | (frequency >> 8);
						write_fm_channel(fm_register, value);
						z80_block(36);
						value = (uint8_t)fnumber;
						fm_register = 0xA0u | (frequency >> 8);
						write_fm_channel(fm_register, value);
						z80_block(10);
						return;
					}
				}
			}
			if (fall_block_check)
			{
				z80_block(21);
				if (fall_block > 8u)
				{
					z80_block(59);
					FM_VOICE(voice)->vibrato_block -= 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			}
			z80_block(49);
			FM_VOICE(voice)->vibrato_block -= 8u;
		}
		z80_block(167);
		FM_VOICE(voice)->vibrato_range &= 0xFu;
		word_copy = fnumber;
		frequency = word_copy;
		fnumber = vibrato_center - (((FM_VOICE(voice)->vibrato_range & 7u) << 8) | FM_VOICE(voice)->vibrato_depth);
		if (((fnumber >> 8) & 0xC0u) == 0u)
		{
			z80_block(69);
			word_copy = frequency;
			fall_floor = fnumber;
			fnumber = frequency;
			if (fall_floor < (uint16_t)((((word_copy >> 8) & 7u) << 8) | (uint8_t)word_copy))
			{
				z80_block(94);
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
				frequency = (fm_channel & 3u) << 8;
				value = (uint8_t)(fnumber >> 8);
				fm_register = 0xA4u | (frequency >> 8);
				write_fm_channel(fm_register, value);
				z80_block(36);
				value = (uint8_t)fnumber;
				fm_register = 0xA0u | (frequency >> 8);
				write_fm_channel(fm_register, value);
				z80_block(10);
				return;
			}
			z80_block(10);
			z80_block(97);
			FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
			FM_VOICE(voice)->vibrato_phase--;
			z80_block(94);
			fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
			frequency = (fm_channel & 3u) << 8;
			value = (uint8_t)(fnumber >> 8);
			fm_register = 0xA4u | (frequency >> 8);
			write_fm_channel(fm_register, value);
			z80_block(36);
			value = (uint8_t)fnumber;
			fm_register = 0xA0u | (frequency >> 8);
			write_fm_channel(fm_register, value);
			z80_block(10);
			return;
		}
		z80_block(83);
		word_copy = frequency;
		fnumber = frequency;
		if (word_copy > 0u)
		{
			z80_block(97);
			FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
			FM_VOICE(voice)->vibrato_phase--;
		}
	}
	else
	{
		z80_block(164);
		frequency = fnumber;
		fnumber = FM_VOICE(voice)->vibrato_offset + FM_VOICE(voice)->vibrato_rise;
		FM_VOICE(voice)->vibrato_offset = fnumber;
		fnumber += frequency;
		if ((FM_VOICE(voice)->vibrato_range & 0x70u) != 0u)
		{
			z80_block(83);
			word_copy = fnumber;
			rise_fnumber = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
			fnumber = word_copy;
			if (rise_fnumber < 0x4CEu)
			{
				z80_block(94);
				fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
				frequency = (fm_channel & 3u) << 8;
				value = (uint8_t)(fnumber >> 8);
				fm_register = 0xA4u | (frequency >> 8);
				write_fm_channel(fm_register, value);
				z80_block(36);
				value = (uint8_t)fnumber;
				fm_register = 0xA0u | (frequency >> 8);
				write_fm_channel(fm_register, value);
				z80_block(10);
				return;
			}
			z80_block(314);
			frequency = fnumber;
			center_for_rise = vibrato_center;
			center_high = (uint8_t)(center_for_rise >> 8);
			center_low = (uint8_t)center_for_rise;
			fnumber = 0x269u - (((uint32_t)center_high << 8) | center_low);
			FM_VOICE(voice)->vibrato_offset = fnumber;
			saved_center_high = center_high;
			saved_center_low = center_low;
			fnumber = (((uint32_t)saved_center_high << 8) | saved_center_low) + FM_VOICE(voice)->vibrato_offset;
			fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)(((frequency >> 8) & 0x38u) + (fnumber >> 8)) << 8);
			frequency = ((fnumber >> 8) & 0x38u) << 8;
			rise_block = ((FM_VOICE(voice)->vibrato_block & ~7u) + (frequency >> 8)) & 0xF8u;
			value = ((FM_VOICE(voice)->vibrato_range & 0x70u) >> 1) + (frequency >> 8) - 8u;
			rise_limit = value;
			if (value >= 0x30u)
			{
				z80_block(7);
				rise_limit = 0x28u;
			}
			z80_block(21);
			if ((rise_limit & 0xC0u) != 0u)
			{
				z80_block(25);
				if (rise_block <= (uint8_t)(rise_limit + 8u))
				{
					z80_block(45);
					FM_VOICE(voice)->vibrato_block += 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			}
			else
			{
				z80_block(18);
				if (rise_block <= rise_limit)
				{
					z80_block(10);
					z80_block(45);
					FM_VOICE(voice)->vibrato_block += 8u;
					z80_block(94);
					fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
					frequency = (fm_channel & 3u) << 8;
					value = (uint8_t)(fnumber >> 8);
					fm_register = 0xA4u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(36);
					value = (uint8_t)fnumber;
					fm_register = 0xA0u | (frequency >> 8);
					write_fm_channel(fm_register, value);
					z80_block(10);
					return;
				}
			}
			z80_block(45);
			FM_VOICE(voice)->vibrato_block += 8u;
		}
		z80_block(170);
		FM_VOICE(voice)->vibrato_range &= 0xFu;
		word_copy = fnumber;
		frequency = word_copy;
		fnumber = vibrato_center + (((FM_VOICE(voice)->vibrato_range & 7u) << 8) | FM_VOICE(voice)->vibrato_depth);
		if (((fnumber >> 8) & 0xFu) >= 4u)
		{
			z80_block(7);
			fnumber = (fnumber & 0xFFu) | 0x500u;
		}
		else
			z80_block(10);
		z80_block(84);
		word_copy = frequency;
		rise_ceiling = (fnumber & 0xFFu) | (((fnumber >> 8) & 7u) << 8);
		fnumber = frequency;
		if (rise_ceiling < (uint16_t)((((word_copy >> 8) & 7u) << 8) | (uint8_t)word_copy))
		{
			z80_block(97);
			FM_VOICE(voice)->vibrato_range += FM_VOICE(voice)->vibrato_phase & 0x70u;
			FM_VOICE(voice)->vibrato_phase++;
		}
	}
	z80_block(94);
	fnumber = (fnumber & 0xFFu) | ((uint32_t)(uint8_t)((FM_VOICE(voice)->vibrato_block & ~7u) + (fnumber >> 8)) << 8);
	frequency = (fm_channel & 3u) << 8;
	value = (uint8_t)(fnumber >> 8);
	fm_register = 0xA4u | (frequency >> 8);
	write_fm_channel(fm_register, value);
	z80_block(36);
	value = (uint8_t)fnumber;
	fm_register = 0xA0u | (frequency >> 8);
	write_fm_channel(fm_register, value);
	z80_block(10);
}

/* Unused: hands back 0x3AE, read as registers A4/A0 take a frequency: block 0, F-number 0x3AE, a value
   of fm_fnum_tables. */
uint16_t load_fixed_fm_frequency(void)
{
	uint16_t fixed_frequency = 0;

	z80_block(24);
	fixed_frequency = 0x3AEu;
	return fixed_frequency;
}
