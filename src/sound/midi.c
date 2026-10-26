/**
 *    Name:        midi.c
 *    Description: the MIDI messages of the queue for each part on their channel: note on and off, pitch bend, control and program changes, picking a sample voice, retuning, FM modulation; the unused MIDI port on an 8251 USART (ports F0 and F1)
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#include "sound.h"

/* Takes one message off the MIDI queue when midi_queue_count is not 0: its status byte, or under running
   status midi_status again, then play_midi_message for each of the 13 parts that is on and listens to
   its channel, each reading from the same place; then skip_midi_data moves past it. */
void handle_midi_message(void)
{
	uint8_t parts_left = 0, status = 0;
	uint16_t channel_and_index = 0, ignored16 = 0, part = 0, parts_left_and_byte = 0;
	uint8_t saved_parts_left = 0, saved_parts_left_and_byte = 0;
	uint16_t saved_channel_and_index = 0;

	z80_block(26);
	channel_and_index = midi_queue_count_ADDRESS;
	if (midi_queue_count == 0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(28);
	midi_queue_count--;
	read_midi_byte(&parts_left_and_byte, &ignored16, &status);
	z80_block(18);
	if (status >> 7 == 0u)
	{
		z80_block(13);
		status = midi_status;
	}
	else
	{
		z80_block(23);
		midi_status = status;
	}
	z80_block(49);
	channel_and_index = ((status & 0xFu) << 8) | midi_queue_read;
	part = MIDI_PART_ADDRESS(0);
	parts_left = 0xDu;
	for (;;)
	{
		z80_block(30);
		if (MIDI_PART(part)->flags >> 7)
		{
			z80_block(33);
			if (channel_and_index >> 8 == MIDI_PART(part)->midi_channel)
			{
				z80_block(39);
				saved_parts_left = parts_left;
				saved_parts_left_and_byte = (uint8_t)parts_left_and_byte;
				saved_channel_and_index = channel_and_index;
				play_midi_message((uint8_t)parts_left_and_byte, part);
				z80_block(37);
				channel_and_index = saved_channel_and_index;
				parts_left_and_byte = saved_parts_left_and_byte;
				parts_left = saved_parts_left;
				midi_queue_read = (uint8_t)channel_and_index;
			}
		}
		z80_block(33);
		part += 9u;
		parts_left--;
		if (parts_left == 0u)
			break;
		z80_cycles(5);
	}
	skip_midi_data((uint8_t)parts_left_and_byte);
}

/* Moves midi_queue_read to the last byte of the message that starts with first_byte: one data byte for
   0xC0 and 0xD0 (program change, channel pressure), two for the others, after the status byte when
   first_byte is one. */
void skip_midi_data(uint8_t first_byte)
{
	uint8_t command = 0;
	uint16_t read_index_address = 0;

	z80_block(28);
	read_index_address = midi_queue_read_ADDRESS;
	if ((uint8_t)first_byte >> 7 != 0u)
	{
		z80_block(22);
		command = first_byte & 0xF0u;
		midi_queue_read++;
	}
	else
	{
		z80_block(30);
		command = midi_status & ~0xFu;
	}
	z80_block(12);
	if (command == 0xC0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(12);
	if (command == 0xD0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(21);
	midi_queue_read++;
}

/* One MIDI message for part, by midi_status's high nibble: note on and off (FM voices, or sample voices
   for sample instruments and drum kits), pitch bend (retune_voice), control change (1, 7, 0x10, 0x11,
   0x20, 0x40, 0x50, 0x51, 0x7B, 0x80) or program change; 0xA0 and 0xD0 only skip their data. The sample
   loops of CC 0x7B and CC 0x50 0x7E never leave the first voice. */
void play_midi_message(uint8_t message_byte, uint16_t part)
{
	uint32_t carry = 0;
	uint8_t bend = 0, bend_high = 0, bend_low = 0, bend_sign = 0, bend_steps = 0, channel = 0, channels_left = 0, chip_channel = 0, control_value = 0, controller = 0, controller_number = 0, controls = 0, depth = 0, detune_low = 0, detune_sign = 0, effective_volume = 0, event = 0, first_byte = 0, first_data_byte = 0, fm_detune = 0, fm_register = 0, ignored8 = 0, level = 0, level_register = 0, modulation_depth = 0, note = 0, note_number = 0, note_to_mark = 0, note_velocity = 0, old_controls = 0, operators_left = 0, part_channel = 0, parts_left = 0, pcm_register = 0, pitch_bend_high = 0, pitch_bend_low = 0, preset = 0, program = 0, register_number = 0, register_value = 0, rounded_volume = 0, sample = 0, scale = 0, setup = 0, total_level = 0, velocity = 0, velocity_then_steal_low = 0, vibrato_preset = 0, voice_channel = 0, voice_chip_channel = 0, voice_note = 0, voices_left = 0, voices_or_bytes_left = 0, voices_unchecked = 0, volume = 0, volume_curve = 0, volume_scale = 0;
	uint16_t bend_entry = 0, bend_index = 0, byte_copy = 0, channel_in_part = 0, channel_offset = 0, controls_entry = 0, each_part = 0, fnum = 0, fnum_entry = 0, fnum_table_entry = 0, free_fm_voice = 0, ignored16 = 0, ignored16_2 = 0, instrument = 0, kept_channel = 0, kit_note = 0, level_product = 0, new_voice = 0, new_voice_field = 0, note_index = 0, patch_levels = 0, preset_address = 0, preset_data = 0, preset_target = 0, register_channel = 0, register_list = 0, scaled_part_volume = 0, setup_data = 0, velocity_index = 0, voice = 0, voice_field = 0, voice_size = 0, voices_left_pair = 0, voices_to_end_at = 0;
	uint8_t saved_channel = 0, saved_count = 0, saved_value = 0, saved_voices_left = 0;
	uint16_t saved_outer = 0, saved_inner = 0;
	uint8_t old_voices_to_end = 0, note_level_byte = 0, level_rotated = 0, fm_detune_byte = 0, negated_bend = 0, negated_steps = 0, modulation_value = 0, modulation_rotated_1 = 0, modulation_rotated_2 = 0, doubled_volume = 0, doubled_music_volume = 0;

	first_data_byte = (uint8_t)message_byte;
	z80_block(37);
	event = midi_status & ~0xFu;
	if (event == 0x90u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&byte_copy, &ignored16, &ignored8);
			first_data_byte = (uint8_t)byte_copy;
		}
		z80_block(96);
		note_number = first_data_byte;
		note_code = note_codes[(uint8_t)(MIDI_PART(part)->transpose + first_data_byte)];
		read_midi_byte(&byte_copy, &ignored16, &velocity);
		velocity_then_steal_low = (uint8_t)byte_copy;
		z80_block(14);
		if (velocity == 0u)
		{
			note_off(part);
			return;
		}
		z80_block(88);
		velocity_index = velocity;
		note_level = velocity_levels[velocity_index];
		program = MIDI_PART(part)->program;
		if (program < 0xF0u)
		{
			z80_block(27);
			instrument = instruments_ADDRESS;
			instrument = read_table_word(instrument, program);
			z80_block(22);
			if (((INSTRUMENT(instrument)->flags >> 1) & 1u) != 0u)
			{
				z80_block(76);
				sample_or_patch = INSTRUMENT(instrument)->sample_or_fm_patch;
				free_fm_voice = next_fm_voice;
				voice_size = 0x12u;
				voices_left = fm_voices_to_end;
				voices_unchecked = 0xCu;
				for (;;)
				{
					z80_block(21);
					if (FM_VOICE(free_fm_voice)->flags == 0u)
						break;
					z80_block(23);
					voices_unchecked--;
					free_fm_voice += voice_size;
					voices_left--;
					if (voices_left != 0u)
					{
						z80_cycles(5);
						continue;
					}
					z80_block(18);
					if (voices_unchecked == 0u)
					{
						z80_block(14);
						return;
					}
					z80_block(24);
					free_fm_voice = FM_VOICE_ADDRESS(0);
					voice_size = 0x12u;
					voices_left = voices_unchecked;
					for (;;)
					{
						z80_block(21);
						if (FM_VOICE(free_fm_voice)->flags == 0u)
							break;
						z80_block(23);
						free_fm_voice += voice_size;
						voices_left--;
						if (voices_left == 0u)
						{
							z80_block(14);
							return;
						}
						z80_cycles(5);
					}
					break;
				}
				z80_block(89);
				new_fm_voice = free_fm_voice;
				vibrato_preset = channel_vibrato_presets[midi_status & 0xFu];
				if (vibrato_preset != 0u)
				{
					z80_block(27);
					preset_address = vibrato_presets_ADDRESS;
					preset_address = read_table_word(preset_address, vibrato_preset);
					z80_block(353);
					preset_data = preset_address;
					preset_target = new_fm_voice + 7u;
					FM_VOICE(preset_target - 0x7u)->vibrato_delay = VIBRATO_PRESET(preset_data)->delay;
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0x8u)->vibrato_wait = VIBRATO_PRESET(preset_data - 0x1u)->wait;
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0x9u)->vibrato_range = VIBRATO_PRESET(preset_data - 0x2u)->range;
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0xAu)->vibrato_depth = VIBRATO_PRESET(preset_data - 0x3u)->depth;
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0xBu)->vibrato_rise = VIBRATO_PRESET(preset_data - 0x4u)->rise;
					preset_data++;
					preset_target++;
					FM_VOICE(preset_target - 0xCu)->vibrato_fall = VIBRATO_PRESET(preset_data - 0x5u)->fall;
					FM_VOICE(preset_target - 0xCu)->vibrato_phase = VIBRATO_PRESET(preset_data - 0x5u)->phase;
					new_voice = new_fm_voice;
					FM_VOICE(new_voice)->vibrato_phase += FM_VOICE(new_voice)->vibrato_range & ~0xFu;
					FM_VOICE(new_voice)->vibrato_rate = FM_VOICE(new_voice)->vibrato_wait;
				}
				z80_block(31);
				voices_to_end_at = fm_voices_to_end_ADDRESS;
				old_voices_to_end = fm_voices_to_end;
				fm_voices_to_end = old_voices_to_end - 1u;
				if (old_voices_to_end != 1u)
				{
					z80_block(53);
					next_fm_voice += 0x12u;
				}
				else
				{
					z80_block(46);
					fm_voices_to_end = 0xCu;
					next_fm_voice = FM_VOICE_ADDRESS(0);
				}
				z80_block(218);
				new_voice_field = new_fm_voice;
				FM_VOICE(new_voice_field)->flags = 0x40u;
				new_voice_field++;
				fm_channel = FM_VOICE(new_voice_field - 0x1u)->chip_channel;
				new_voice_field++;
				note = note_code;
				FM_VOICE(new_voice_field - 0x2u)->note = note;
				voice_note = note;
				new_voice_field++;
				channel = midi_status & 0xFu;
				FM_VOICE(new_voice_field - 0x3u)->midi_channel = channel;
				voice_channel = channel;
				new_voice_field += 2u;
				note_level_byte = note_level;
				carry = note_level_byte & 1u;
				level_rotated = note_level_byte >> 1;
				level = (level_rotated >> 1) | (carry << 7);
				carry = level_rotated & 1u;
				level = (((level >> 1) | (carry << 7)) & 0x1Fu) - 0x20u;
				FM_VOICE(new_voice_field - 0x5u)->level = level;
				level_product = (new_voice_field & 0xFFu) | ((uint32_t)level << 8);
				effective_volume = MIDI_PART(part)->effective_volume;
				volume_curve = effective_volume;
				if (effective_volume != 0u)
				{
					z80_block(20);
					volume_curve += (uint8_t)~effective_volume >> 1;
				}
				z80_block(17);
				level_product = multiply_bytes(volume_curve, level_product);
				z80_block(425);
				note_level = 0x7Fu - ((uint16_t)((uint32_t)((level_product >> 8) >> 1) << 8) >> 8);
				channel_offset = voice_channel;
				fm_pan_modulation = (z80_read8((uint16_t)(channel_controls_ADDRESS + channel_offset)) & 7u) | 0xC0u;
				bend_entry = channel_pitch_bends_ADDRESS + channel_offset + channel_offset;
				fm_detune = MIDI_PART(part)->fm_detune;
				detune_low = fm_detune;
				fm_detune_byte = fm_detune;
				detune_sign = ((uint32_t)fm_detune_byte << 1) | (fm_detune_byte >> 7);
				carry = fm_detune_byte >> 7;
				saved_outer = z80_read16(bend_entry) + (detune_low | ((uint32_t)(uint8_t)(0u - carry) << 8));
				fnum_table_entry = fm_fnum_tables_ADDRESS + (uint8_t)((voice_note & 0xFu) * 2u);
				bend_index = saved_outer;
				fnum_entry = FM_FNUMBER_RUNS(fnum_table_entry)[0] + bend_index;
				carry = (uint16_t)(fnum_entry + bend_index) < fnum_entry;
				fnum_entry += bend_index;
				fnum = FM_FNUMBERS(fnum_entry)[0];
				fnum = (fnum & 0xFFu) | ((uint32_t)(uint8_t)((((voice_note >> 1) | (carry << 7)) & 0x38u) + (fnum >> 8)) << 8);
				register_channel = (fm_channel & 3u) << 8;
				register_value = (uint8_t)(fnum >> 8);
				fm_register = 0xA4u | (register_channel >> 8);
				write_fm_channel(fm_register, register_value);
				z80_block(36);
				register_value = (uint8_t)fnum;
				fm_register = 0xA0u | (register_channel >> 8);
				write_fm_channel(fm_register, register_value);
				z80_block(17);
				load_fm_patch(&channel_in_part, &register_list, &patch_levels);
				part_channel = (uint8_t)channel_in_part;
				z80_block(7);
				operators_left = 4u;
				for (;;)
				{
					z80_block(63);
					saved_count = operators_left;
					saved_value = part_channel;
					register_number = FM_REGISTER_NUMBERS(register_list)[0] + part_channel;
					register_list++;
					level_register = register_number;
					total_level = FM_PATCH_BYTES(patch_levels)[0];
					patch_levels++;
					if (total_level >> 7 == 0u)
					{
						z80_block(21);
						total_level += note_level;
					}
					z80_block(24);
					total_level &= 0x7Fu;
					fm_register = level_register;
					write_fm_channel(fm_register, total_level);
					z80_block(18);
					part_channel = saved_value;
					operators_left = saved_count - 1u;
					if (operators_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(45);
				register_value = fm_pan_modulation;
				fm_register = (uint8_t)(0xB4u + part_channel);
				write_fm_channel(fm_register, register_value);
				z80_block(44);
				register_value = (fm_channel & 7u) | 0xF0u;
				fm_register = 0x28u;
				register_value = write_fm_global(fm_register, register_value);
				return;
			}
			z80_block(84);
			instrument++;
			sample_or_patch = INSTRUMENT(instrument - 0x1u)->sample_or_fm_patch;
			instrument += 2u;
			note_pan = INSTRUMENT(instrument - 0x3u)->pan;
			sample_detune = INSTRUMENT(instrument - 0x3u)->detune;
			note_velocity = velocity_then_steal_low;
			allocate_sample_voice(note_velocity, part);
			return;
		}
		z80_block(34);
		program += 0x10u;
		kit_note = drum_kits_ADDRESS;
		kit_note = read_table_word(kit_note, program);
		z80_block(158);
		note_index = note_number;
		kit_note = kit_note + note_index + note_index + note_index + note_index;
		kit_pitch_low = DRUM_NOTE(kit_note)->pitch_low;
		kit_note++;
		kit_pitch_high = DRUM_NOTE(kit_note - 0x1u)->pitch_high;
		kit_note++;
		note_pan = DRUM_NOTE(kit_note - 0x2u)->pan;
		sample = DRUM_NOTE(kit_note - 0x2u)->sample;
		sample_or_patch = sample;
		if (sample == 0xFFu)
		{
			z80_cycles(6);
			return;
		}
		z80_block(10);
		note_velocity = velocity_then_steal_low;
		allocate_sample_voice(note_velocity, part);
		return;
	}
	z80_block(17);
	if (event == 0x80u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&byte_copy, &ignored16, &ignored8);
			first_data_byte = (uint8_t)byte_copy;
		}
		z80_block(96);
		note_code = note_codes[(uint8_t)(MIDI_PART(part)->transpose + first_data_byte)];
		midi_queue_read++;
		note_off(part);
		return;
	}
	z80_block(17);
	if (event == 0xE0u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&byte_copy, &ignored16, &ignored8);
			first_data_byte = (uint8_t)byte_copy;
		}
		z80_block(21);
		bend_low = first_data_byte;
		read_midi_byte(&ignored16_2, &ignored16, &bend_high);
		z80_block(26);
		carry = bend_high & 1u;
		bend_sign = bend_high >> 1;
		bend = bend_low;
		if (carry)
		{
			z80_block(7);
			bend |= 0x80u;
		}
		z80_block(78);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		bend = (bend >> 1) | (carry << 7);
		if ((bend_sign & 1u) == 0u)
		{
			z80_block(18);
			negated_bend = ~bend + 1u;
			bend = negated_bend;
			if (negated_bend == 0u)
			{
				z80_block(4);
				bend--;
			}
		}
		z80_block(57);
		bend_steps = pitch_bend_steps[bend];
		carry = bend_sign & 1u;
		bend_sign = bend_sign >> 1;
		if (carry == 0u)
		{
			z80_block(22);
			bend_sign--;
			negated_steps = ~bend_steps + 1u;
			bend_steps = negated_steps;
			if (negated_steps == 0u)
			{
				z80_block(4);
				bend_steps--;
			}
		}
		z80_block(145);
		pitch_bend_high = bend_sign;
		pitch_bend_low = bend_steps;
		pitch_bend = ((uint32_t)pitch_bend_high << 8) | pitch_bend_low;
		bend_entry = channel_pitch_bends_ADDRESS + (uint8_t)((midi_status & 0xFu) * 2u);
		z80_write8(bend_entry, pitch_bend_low);
		z80_write8((uint16_t)(bend_entry + 1u), pitch_bend_high);
		voice = SAMPLE_VOICE_ADDRESS(0);
		voices_left = 0x14u;
		channel = midi_status & 0xFu;
		for (;;)
		{
			z80_block(29);
			if (channel == SAMPLE_VOICE(voice)->midi_channel)
			{
				z80_block(47);
				saved_channel = channel;
				if ((SAMPLE_VOICE(voice)->flags & 0x7Fu) != 0u)
				{
					z80_cycles(7);
					voices_left_pair = ((uint32_t)voices_left << 8) | pitch_bend_low;
					voices_left_pair = retune_voice(voices_left_pair, part, voice);
					voices_left = (uint8_t)(voices_left_pair >> 8);
					pitch_bend_low = (uint8_t)voices_left_pair;
				}
				z80_block(10);
				channel = saved_channel;
			}
			z80_block(33);
			voice += 0x12u;
			voices_left--;
			if (voices_left == 0u)
				break;
			z80_cycles(5);
		}
		z80_block(10);
		return;
	}
	z80_block(17);
	if (event == 0xB0u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&byte_copy, &ignored16, &ignored8);
			first_data_byte = (uint8_t)byte_copy;
		}
		z80_block(21);
		controller_number = first_data_byte;
		read_midi_byte(&byte_copy, &voice_field, &ignored8);
		control_value = (uint8_t)byte_copy;
		z80_block(21);
		controller = controller_number;
		carry = controller == 0u;
		if (controller == 1u)
		{
			z80_block(123);
			modulation_value = control_value;
			depth = (modulation_value >> 1) | (carry << 7);
			carry = modulation_value & 1u;
			modulation_rotated_1 = depth;
			depth = (modulation_rotated_1 >> 1) | (carry << 7);
			carry = modulation_rotated_1 & 1u;
			modulation_rotated_2 = depth;
			depth = (modulation_rotated_2 >> 1) | (carry << 7);
			carry = modulation_rotated_2 & 1u;
			depth = ((depth >> 1) | (carry << 7)) & 7u;
			modulation_depth = depth;
			fm_pan_modulation = depth;
			controls_entry = channel_controls_ADDRESS + (midi_status & 0xFu);
			controls = z80_read8(controls_entry);
			old_controls = controls;
			if ((controls & 0x7Fu) == modulation_depth)
			{
				z80_cycles(6);
				return;
			}
			z80_block(59);
			z80_write8(controls_entry, (old_controls & 0x80u) | modulation_depth);
			voice_field = 0xE403u;
			voices_left = 0x14u;
			channel = midi_status & 0xFu;
			for (;;)
			{
				z80_block(17);
				if (channel == z80_read8(voice_field))
				{
					z80_block(46);
					kept_channel = (uint32_t)channel << 8;
					voice_field = voice_field - 1u - 1u - 1u;
					if ((z80_read8(voice_field) & 0x7Fu) != 0u)
					{
						z80_cycles(7);
						voices_left_pair = (uint32_t)voices_left << 8;
						update_fm_modulation(&voices_left_pair, &voice_field);
						voices_left = (uint8_t)(voices_left_pair >> 8);
					}
					z80_block(22);
					voice_field += 3u;
					channel = (uint8_t)(kept_channel >> 8);
				}
				z80_block(29);
				voice_field += 0x12u;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 7u)
		{
			z80_block(18);
			doubled_volume = control_value * 2u;
			volume = doubled_volume;
			if (doubled_volume != 0u)
			{
				z80_block(4);
				volume++;
			}
			z80_block(60);
			MIDI_PART(part)->volume = volume;
			scaled_part_volume = (voice_field & 0xFFu) | ((uint32_t)volume << 8);
			volume_scale = 0xFFu;
			if ((MIDI_PART(part)->flags & 1u) == 0u)
			{
				z80_block(13);
				volume_scale = music_volume;
			}
			z80_block(21);
			scale = volume_scale;
			scaled_part_volume = multiply_bytes(scale, scaled_part_volume);
			z80_block(18);
			rounded_volume = (uint8_t)(scaled_part_volume >> 8);
			if (rounded_volume != 0u)
			{
				z80_block(4);
				rounded_volume++;
			}
			z80_block(36);
			MIDI_PART(part)->effective_volume = rounded_volume;
			voice_field = SAMPLE_VOICE_ADDRESS(0);
			voices_left = 0x14u;
			for (;;)
			{
				z80_block(33);
				saved_outer = voice_field;
				if ((SAMPLE_VOICE(voice_field)->flags >> 6) & 1u)
				{
					z80_block(55);
					voice_field += 3u;
					if ((midi_status & 0xFu) == SAMPLE_VOICE(voice_field - 0x3u)->midi_channel)
					{
						z80_block(37);
						voice_field = voice_field - 1u - 1u;
						voice_chip_channel = SAMPLE_VOICE(voice_field - 0x1u)->chip_channel;
						if (voice_chip_channel >> 7 == 0u)
						{
							z80_block(35);
							register_value = voice_chip_channel | 0xC0u;
							pcm_register = 7u;
							write_pcm_register(pcm_register, register_value);
							z80_block(64);
							voice_field = ((voice_field + 4u) & 0xFFu) | ((uint32_t)SAMPLE_VOICE(voice_field - 0x1u)->level << 8);
							scale = MIDI_PART(part)->effective_volume;
							voice_field = multiply_bytes(scale, voice_field);
							z80_block(28);
							register_value = (uint8_t)(voice_field >> 8);
							pcm_register = 0u;
							write_pcm_register(pcm_register, register_value);
						}
						else
						{
							z80_block(17);
							chip_channel = voice_chip_channel;
							apply_fm_channel_volume(chip_channel, voice_field, part);
							z80_block(10);
						}
					}
				}
				z80_block(39);
				voice_field = saved_outer + 0x12u;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 0x10u)
		{
			z80_block(18);
			doubled_music_volume = control_value * 2u;
			volume = doubled_music_volume;
			if (doubled_music_volume != 0u)
			{
				z80_block(4);
				volume++;
			}
			z80_block(34);
			music_volume = volume;
			each_part = MIDI_PART_ADDRESS(0);
			parts_left = 8u;
			for (;;)
			{
				z80_block(67);
				saved_count = parts_left;
				scaled_part_volume = (voice_field & 0xFFu) | ((uint32_t)MIDI_PART(each_part)->volume << 8);
				volume_scale = 0xFFu;
				if ((MIDI_PART(each_part)->flags & 1u) == 0u)
				{
					z80_block(13);
					volume_scale = music_volume;
				}
				z80_block(21);
				scale = volume_scale;
				scaled_part_volume = multiply_bytes(scale, scaled_part_volume);
				z80_block(18);
				rounded_volume = (uint8_t)(scaled_part_volume >> 8);
				if (rounded_volume != 0u)
				{
					z80_block(4);
					rounded_volume++;
				}
				z80_block(36);
				MIDI_PART(each_part)->effective_volume = rounded_volume;
				voice_field = 0xE403u;
				voices_left = 0x14u;
				for (;;)
				{
					z80_block(51);
					saved_inner = voice_field;
					if (MIDI_PART(each_part)->midi_channel == z80_read8(voice_field))
					{
						z80_block(40);
						voice_field = voice_field - 1u - 1u - 1u;
						if ((z80_read8(voice_field) >> 6) & 1u)
						{
							z80_block(31);
							voice_field++;
							voice_chip_channel = z80_read8(voice_field);
							if (voice_chip_channel >> 7 == 0u)
							{
								z80_block(35);
								register_value = voice_chip_channel | 0xC0u;
								pcm_register = 7u;
								write_pcm_register(pcm_register, register_value);
								z80_block(64);
								voice_field = ((voice_field + 4u) & 0xFFu) | ((uint32_t)z80_read8((uint16_t)(voice_field + 4u)) << 8);
								scale = MIDI_PART(each_part)->effective_volume;
								voice_field = multiply_bytes(scale, voice_field);
								z80_block(28);
								register_value = (uint8_t)(voice_field >> 8);
								pcm_register = 0u;
								write_pcm_register(pcm_register, register_value);
							}
							else
							{
								z80_block(17);
								chip_channel = voice_chip_channel;
								apply_fm_music_volume(chip_channel, voice_field, each_part);
								z80_block(10);
							}
						}
					}
					z80_block(39);
					voice_field = saved_inner + 0x12u;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(43);
				each_part += 9u;
				parts_left = saved_count - 1u;
				if (parts_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(10);
			return;
		}
		z80_block(17);
		if (controller == 0x40u)
		{
			z80_block(21);
			if (control_value >= 0x40u)
			{
				z80_block(77);
				controls_entry = channel_controls_ADDRESS + (midi_status & 0xFu);
				z80_write8(controls_entry, z80_read8(controls_entry) | 0x80u);
				return;
			}
			z80_block(51);
			voice = SAMPLE_VOICE_ADDRESS(0);
			voice_size = 0x12u;
			voices_left = 0x14u;
			channel = midi_status & 0xFu;
			for (;;)
			{
				z80_block(29);
				if (channel == SAMPLE_VOICE(voice)->midi_channel)
				{
					z80_block(41);
					voice_field = (voice_field & ~0xFFu) | channel;
					if ((SAMPLE_VOICE(voice)->flags >> 1) & 1u)
					{
						z80_block(37);
						if (SAMPLE_VOICE(voice)->chip_channel >> 7 == 0u)
						{
							z80_block(39);
							saved_outer = voice_field;
							saved_voices_left = voices_left;
							stop_pcm_voice(voice);
							z80_block(30);
							voices_left = saved_voices_left;
							voice_field = saved_outer;
							voice_size = 0x12u;
						}
						else
						{
							z80_block(17);
							release_held_fm_voice(voice);
							z80_block(10);
						}
					}
					z80_block(4);
					channel = (uint8_t)voice_field;
				}
				z80_block(23);
				voice += voice_size;
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
			z80_block(77);
			controls_entry = channel_controls_ADDRESS + (midi_status & 0xFu);
			z80_write8(controls_entry, z80_read8(controls_entry) & 0x7Fu);
			return;
		}
		z80_block(17);
		if (controller == 0x7Bu)
		{
			z80_block(55);
			control_value = midi_status & 0xFu;
			voice = SAMPLE_VOICE_ADDRESS(0);
			voices_left = 8u;
			for (;;)
			{
				z80_block(30);
				if ((SAMPLE_VOICE(voice)->flags >> 6) & 1u)
				{
					z80_block(33);
					if (control_value == SAMPLE_VOICE(voice)->midi_channel)
					{
						z80_block(28);
						saved_count = voices_left;
						saved_value = control_value;
						stop_pcm_voice(voice);
						z80_block(20);
						voices_left = saved_count;
						control_value = saved_value;
					}
				}
				z80_block(19);
				voices_left--;
				if (voices_left == 0u)
					break;
				z80_cycles(5);
			}
		}
		else
		{
			z80_block(17);
			if (controller == 0x50u)
			{
				z80_block(21);
				if (control_value == 0x7Eu)
				{
					z80_block(31);
					voice = SAMPLE_VOICE_ADDRESS(0);
					voices_left = 8u;
					for (;;)
					{
						z80_block(30);
						if ((SAMPLE_VOICE(voice)->flags >> 6) & 1u)
						{
							z80_block(30);
							if (((SAMPLE_VOICE(voice)->flags >> 4) & 1u) == 0u)
							{
								z80_block(28);
								saved_count = voices_left;
								stop_pcm_voice(voice);
								z80_block(20);
								voices_left = saved_count;
							}
							else
							{
								z80_block(36);
								if (SAMPLE_VOICE(voice)->midi_channel == 0xFu)
								{
									z80_block(28);
									saved_count = voices_left;
									stop_pcm_voice(voice);
									z80_block(20);
									voices_left = saved_count;
								}
							}
						}
						z80_block(19);
						voices_left--;
						if (voices_left == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(10);
				}
				else
				{
					z80_block(31);
					voice = SAMPLE_VOICE_ADDRESS(0);
					voice_size = 0x12u;
					voices_left = 8u;
					for (;;)
					{
						z80_block(30);
						if ((SAMPLE_VOICE(voice)->flags >> 6) & 1u)
						{
							z80_block(28);
							saved_count = voices_left;
							stop_pcm_voice(voice);
							z80_block(20);
							voices_left = saved_count;
							voice_size = 0x12u;
						}
						z80_block(23);
						voice += voice_size;
						voices_left--;
						if (voices_left == 0u)
							break;
						z80_cycles(5);
					}
				}
				z80_block(31);
				voice = FM_VOICE_ADDRESS(0);
				voice_size = 0x12u;
				voices_left = 0xCu;
				for (;;)
				{
					z80_block(30);
					if ((FM_VOICE(voice)->flags >> 6) & 1u)
					{
						z80_block(28);
						saved_count = voices_left;
						stop_fm_voice(voice);
						z80_block(20);
						voices_left = saved_count;
						voice_size = 0x12u;
					}
					z80_block(23);
					voice += voice_size;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(17);
				controls_entry = channel_controls_ADDRESS;
				channels_left = 0x10u;
				for (;;)
				{
					z80_block(24);
					z80_write8(controls_entry, 0u);
					controls_entry++;
					channels_left--;
					if (channels_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(17);
				bend_entry = channel_pitch_bends_ADDRESS;
				channels_left = 0x10u;
				for (;;)
				{
					z80_block(24);
					z80_write8(bend_entry, 0u);
					bend_entry++;
					channels_left--;
					if (channels_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
				return;
			}
			z80_block(17);
			if (controller == 0x20u)
			{
				z80_block(31);
				setup = control_value;
				setup_data = part_templates_ADDRESS;
				setup_data = read_table_word(setup_data, setup);
				z80_block(57);
				move_block(setup_data, MIDI_PART_ADDRESS(0), 0x48u, 1u);
				setup_data += 0x48u;
				each_part = MIDI_PART_ADDRESS(0);
				parts_left = 8u;
				for (;;)
				{
					z80_block(56);
					setup_data = (setup_data & 0xFFu) | ((uint32_t)MIDI_PART(each_part)->volume << 8);
					volume_scale = 0xFFu;
					if ((MIDI_PART(each_part)->flags & 1u) == 0u)
					{
						z80_block(13);
						volume_scale = music_volume;
					}
					z80_block(21);
					scale = volume_scale;
					setup_data = multiply_bytes(scale, setup_data);
					z80_block(18);
					rounded_volume = (uint8_t)(setup_data >> 8);
					if (rounded_volume != 0u)
					{
						z80_block(4);
						rounded_volume++;
					}
					z80_block(52);
					MIDI_PART(each_part)->effective_volume = rounded_volume;
					each_part += 9u;
					parts_left--;
					if (parts_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
				return;
			}
			z80_block(17);
			if (controller == 0x51u)
			{
				z80_block(60);
				note_to_mark = note_codes[control_value];
				voice_field = SAMPLE_VOICE_ADDRESS(0);
				voices_left = 8u;
				for (;;)
				{
					z80_block(22);
					if ((SAMPLE_VOICE(voice_field)->flags >> 6) & 1u)
					{
						z80_block(33);
						voice_field += 2u;
						if (note_to_mark != SAMPLE_VOICE(voice_field - 0x2u)->note)
						{
							z80_block(12);
							voice_field = voice_field - 1u - 1u;
						}
						else
						{
							z80_block(43);
							voice_field++;
							if ((midi_status & 0xFu) != SAMPLE_VOICE(voice_field - 0x3u)->midi_channel)
							{
								z80_block(6);
								voice_field--;
								z80_block(12);
								voice_field = voice_field - 1u - 1u;
							}
							else
							{
								z80_block(43);
								voice_field = voice_field - 1u - 1u - 1u;
								SAMPLE_VOICE(voice_field)->flags |= 1u;
							}
						}
					}
					z80_block(29);
					voice_field += 0x12u;
					voices_left--;
					if (voices_left == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
				return;
			}
			z80_block(17);
			if (controller == 0x11u)
			{
				z80_block(18);
				if (control_value == 0u)
				{
					z80_block(87);
					control_value = 0u;
					channel = midi_status & 0xFu;
					channel_vibrato_presets[channel] = 0u;
					voices_left = 0xCu;
					voice = FM_VOICE_ADDRESS(0);
					for (;;)
					{
						z80_block(29);
						if (channel == FM_VOICE(voice)->midi_channel)
							break;
						z80_block(33);
						voice += 0x12u;
						voices_left--;
						if (voices_left == 0u)
						{
							z80_block(10);
							return;
						}
						z80_cycles(5);
					}
					z80_block(36);
					voice += 4u;
					voices_or_bytes_left = 6u;
					for (;;)
					{
						z80_block(37);
						z80_write8(voice, control_value);
						voice++;
						voices_or_bytes_left--;
						if (voices_or_bytes_left == 0u)
							break;
						z80_cycles(5);
					}
					z80_block(39);
					voice += 8u;
				}
				else
				{
					z80_block(80);
					channel_vibrato_presets[midi_status & 0xFu] = control_value;
					voices_or_bytes_left = 0xCu;
					voice = FM_VOICE_ADDRESS(0);
					for (;;)
					{
						z80_block(36);
						if ((FM_VOICE(voice)->flags & 0x40u) != 0u)
							break;
						z80_block(33);
						voice += 0x12u;
						voices_or_bytes_left--;
						if (voices_or_bytes_left == 0u)
						{
							z80_block(10);
							return;
						}
						z80_cycles(5);
					}
				}
				for (;;)
				{
					z80_block(49);
					if ((midi_status & 0xFu) == FM_VOICE(voice)->midi_channel)
					{
						z80_block(31);
						preset_address = vibrato_presets_ADDRESS;
						preset = control_value;
						preset_address = read_table_word(preset_address, preset);
						z80_block(344);
						preset_data = preset_address;
						FM_VOICE(voice)->vibrato_delay = VIBRATO_PRESET(preset_data)->delay;
						preset_data++;
						FM_VOICE(voice)->vibrato_wait = VIBRATO_PRESET(preset_data - 0x1u)->wait;
						preset_data++;
						FM_VOICE(voice)->vibrato_range = VIBRATO_PRESET(preset_data - 0x2u)->range;
						preset_data++;
						FM_VOICE(voice)->vibrato_depth = VIBRATO_PRESET(preset_data - 0x3u)->depth;
						preset_data++;
						FM_VOICE(voice)->vibrato_rise = VIBRATO_PRESET(preset_data - 0x4u)->rise;
						preset_data++;
						FM_VOICE(voice)->vibrato_fall = VIBRATO_PRESET(preset_data - 0x5u)->fall;
						FM_VOICE(voice)->vibrato_phase = VIBRATO_PRESET(preset_data - 0x5u)->phase;
						FM_VOICE(voice)->vibrato_rate = FM_VOICE(voice)->vibrato_wait;
						FM_VOICE(voice)->vibrato_phase = (FM_VOICE(voice)->vibrato_phase & 1u) + FM_VOICE(voice)->vibrato_range;
					}
					do
					{
						z80_block(33);
						voice += 0x12u;
						voices_or_bytes_left--;
						if (voices_or_bytes_left == 0u)
						{
							z80_block(10);
							return;
						}
						z80_cycles(5);
						z80_block(36);
					} while ((FM_VOICE(voice)->flags & 0x40u) == 0u);
				}
			}
			z80_block(17);
			if (controller != 0x80u)
			{
				z80_block(10);
				return;
			}
		}
		z80_block(47);
		channel = midi_status & 0xFu;
		voice_field = FM_VOICE_ADDRESS(0);
		voice_size = 0x12u;
		voices_left = 0xCu;
		for (;;)
		{
			z80_block(22);
			if ((z80_read8(voice_field) >> 6) & 1u)
			{
				z80_block(35);
				voice_field += 3u;
				if (channel != z80_read8(voice_field))
				{
					z80_block(18);
					voice_field = voice_field - 1u - 1u - 1u;
				}
				else
				{
					z80_block(46);
					voice_field = voice_field - 1u - 1u - 1u;
					saved_count = voices_left;
					saved_value = control_value;
					fm_note_off(&voice_size, &voice_field, &channel);
					z80_block(35);
					voices_left = saved_count;
					control_value = saved_value;
					z80_write8(voice_field, z80_read8(voice_field) & 0xBFu);
				}
			}
			z80_block(19);
			voice_field += voice_size;
			voices_left--;
			if (voices_left == 0u)
				break;
			z80_cycles(5);
		}
		z80_block(118);
		channel_controls[control_value] = 0u;
		bend_entry = channel_pitch_bends_ADDRESS + (uint8_t)(control_value * 2u);
		z80_write8(bend_entry, 0u);
		z80_write8((uint16_t)(bend_entry + 1u), 0u);
		return;
	}
	z80_block(17);
	if (event == 0xC0u)
	{
		z80_block(18);
		if (first_data_byte >> 7)
		{
			z80_block(17);
			read_midi_byte(&byte_copy, &ignored16, &ignored8);
			first_data_byte = (uint8_t)byte_copy;
		}
		z80_block(29);
		MIDI_PART(part)->program = first_data_byte;
		return;
	}
	z80_block(17);
	if (event != 0xD0u)
	{
		z80_block(17);
		if (event != 0xA0u)
		{
			z80_block(10);
			return;
		}
	}
	first_byte = first_data_byte;
	skip_midi_data(first_byte);
}

/* Unused: a drum note on that, for MIDI note 0x2C (note_and_velocity's high byte), walks the 8 sample
   voices for one whose note is 0xCA but changes nothing whichever it finds; then, as for every other
   note, allocate_sample_voice. */
void scan_drum_note_voices(uint16_t note_and_velocity, uint16_t part)
{
	uint8_t voices_left = 0;
	uint16_t voice_field = 0, voice_step = 0;

	z80_block(21);
	if (note_and_velocity >> 8 != 0x2Cu)
	{
		allocate_sample_voice((uint8_t)note_and_velocity, part);
		return;
	}
	z80_block(27);
	voice_field = 0xE402u;
	voice_step = 0x12u;
	voices_left = 8u;
	for (;;)
	{
		z80_block(24);
		if (SAMPLE_VOICE(voice_field - 0x2u)->note == 0xCAu)
		{
			z80_block(34);
			voice_field = voice_field - 1u - 1u;
			z80_block(12);
			voice_field += 2u;
		}
		z80_block(19);
		voice_field += voice_step;
		voices_left--;
		if (voices_left == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	allocate_sample_voice((uint8_t)note_and_velocity, part);
}

/* A sample note on for part: takes a free sample_voice, or keys off and takes the one with the lowest
   allocation_order (every order drops by 1; drum notes off channel 15 get orders from 0xF0, stolen
   last); sets its RF5C68 channel up, starts the sample and writes the pitch (pcm_pitch_tables, bend,
   detune and octave, or kit_pitch), the level and the pan; keys it on. */
void allocate_sample_voice(uint8_t note_velocity, uint16_t part)
{
	uint32_t carry = 0;
	uint8_t channel = 0, control_register = 0, count_or_channel = 0, detune = 0, detune_low = 0, detune_sign = 0, keys_on = 0, lowest_order = 0, note = 0, octave = 0, order = 0, pcm_register = 0, register_value = 0, scale = 0, velocity_then_steal_low = 0, voice_note = 0;
	uint16_t bend_entry = 0, free_voice = 0, level_product = 0, new_voice = 0, order_entry = 0, pitch = 0, pitch_entry = 0, pitch_table_entry = 0, voice_size = 0;
	uint16_t saved_outer = 0;
	uint8_t sample_detune_byte = 0, octave_bits = 0;

	velocity_then_steal_low = (uint8_t)note_velocity;
	z80_block(31);
	free_voice = SAMPLE_VOICE_ADDRESS(0);
	voice_size = 0x12u;
	count_or_channel = 8u;
	lowest_order = 0u;
	for (;;)
	{
		z80_block(17);
		if (lowest_order == SAMPLE_VOICE(free_voice)->flags)
		{
			z80_block(25);
			saved_outer = free_voice;
			new_voice = saved_outer;
			break;
		}
		z80_block(19);
		free_voice += voice_size;
		count_or_channel--;
		if (count_or_channel != 0u)
		{
			z80_cycles(5);
			continue;
		}
		z80_block(21);
		order_entry = voice_0_allocation_order_ADDRESS;
		count_or_channel = 8u;
		lowest_order--;
		for (;;)
		{
			z80_block(17);
			if (SAMPLE_VOICE(order_entry - 0x6u)->allocation_order <= lowest_order)
			{
				z80_block(11);
				lowest_order = SAMPLE_VOICE(order_entry - 0x6u)->allocation_order;
				velocity_then_steal_low = (uint8_t)order_entry;
			}
			z80_block(19);
			order_entry += voice_size;
			count_or_channel--;
			if (count_or_channel == 0u)
				break;
			z80_cycles(5);
		}
		z80_block(333);
		new_voice = 0xFFFAu + ((order_entry & ~0xFFu) | velocity_then_steal_low);
		voice_0_allocation_order--;
		voice_1_allocation_order--;
		voice_2_allocation_order--;
		voice_3_allocation_order--;
		voice_4_allocation_order--;
		voice_5_allocation_order--;
		voice_6_allocation_order--;
		voice_7_allocation_order--;
		pcm_voices_used = 7u;
		keys_on = pcm_channels_on ^ pcm_channel_bits[SAMPLE_VOICE(new_voice)->chip_channel];
		pcm_channels_on = keys_on;
		keys_on = ~keys_on;
		pcm_register = 8u;
		write_pcm_register(pcm_register, keys_on);
		z80_block(67);
		SAMPLE_VOICE(new_voice)->wave_page = 0xD0u;
		SAMPLE_VOICE(new_voice)->stream_position_low = 0xFFu;
		SAMPLE_VOICE(new_voice)->stream_position_high = 7u;
		break;
	}
	z80_block(36);
	if (MIDI_PART(part)->program < 0xF0u)
	{
		z80_block(63);
		register_value = SAMPLE_VOICE(new_voice)->chip_channel;
		pcm_note_channel = register_value;
		register_value |= 0xC0u;
		control_register = 7u;
		pcm_register = 7u;
		write_pcm_register(pcm_register, register_value);
		z80_block(35);
		register_value = ((register_value & 7u) * 2u) | 0x80u;
		pcm_register = control_register;
		write_pcm_register(pcm_register, register_value);
		z80_block(47);
		register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
		register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
		register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
		register_value = (((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u;
		pcm_register = 5u;
		write_pcm_register(pcm_register, register_value);
		z80_block(24);
		pcm_register = 6u;
		write_pcm_register(pcm_register, register_value);
		z80_block(177);
		note = note_code;
		SAMPLE_VOICE(new_voice)->note = note;
		voice_note = note;
		channel = midi_status & 0xFu;
		SAMPLE_VOICE(new_voice)->midi_channel = channel;
		count_or_channel = channel;
		SAMPLE_VOICE(new_voice)->level = note_level;
		order = pcm_voices_used;
		SAMPLE_VOICE(new_voice)->allocation_order = order;
		pcm_voices_used = order + 1u;
		start_pcm_sample(new_voice);
		z80_block(282);
		SAMPLE_VOICE(new_voice)->flags = 0x40u;
		bend_entry = channel_pitch_bends_ADDRESS + (uint8_t)(count_or_channel * 2u);
		detune = sample_detune;
		detune_low = detune;
		sample_detune_byte = detune;
		detune_sign = ((uint32_t)sample_detune_byte << 1) | (sample_detune_byte >> 7);
		carry = sample_detune_byte >> 7;
		saved_outer = z80_read16(bend_entry) + (detune_low | ((uint32_t)(uint8_t)(0u - carry) << 8));
		pitch_table_entry = pcm_pitch_tables_ADDRESS + (uint8_t)((voice_note & 0xFu) * 2u);
		pitch_entry = PCM_PITCH_RUNS(pitch_table_entry)[0] + saved_outer + saved_outer;
		pitch = PCM_PITCH_STEPS(pitch_entry)[0];
		octave_bits = voice_note & ~0xFu;
		octave = octave_bits;
		if (octave_bits != 0u)
		{
			z80_block(33);
			octave = (octave >> 1) | ((uint32_t)octave << 7);
			octave = (octave >> 1) | ((uint32_t)octave << 7);
			octave = (octave >> 1) | ((uint32_t)octave << 7);
			octave = (octave >> 1) | ((uint32_t)octave << 7);
			if (octave < 8u)
			{
				z80_block(4);
				count_or_channel = octave;
				for (;;)
				{
					z80_block(24);
					carry = (uint8_t)pitch >> 7;
					pitch = (pitch & ~0xFFu) | (uint8_t)((uint32_t)pitch << 1);
					pitch = (pitch & 0xFFu) | ((uint32_t)(uint8_t)(((uint32_t)(pitch >> 8) << 1) | carry) << 8);
					count_or_channel--;
					if (count_or_channel == 0u)
						break;
					z80_cycles(5);
				}
			}
			else
			{
				z80_block(19);
				count_or_channel = (~octave + 1u) & 0xFu;
				for (;;)
				{
					z80_block(24);
					carry = (pitch >> 8) & 1u;
					pitch = (pitch & 0xFFu) | ((uint32_t)((pitch >> 8) >> 1) << 8);
					pitch = (pitch & ~0xFFu) | (uint8_t)(((uint8_t)pitch >> 1) | (carry << 7));
					count_or_channel--;
					if (count_or_channel == 0u)
						break;
					z80_cycles(5);
				}
				z80_block(10);
			}
		}
	}
	else
	{
		z80_block(63);
		register_value = SAMPLE_VOICE(new_voice)->chip_channel;
		pcm_note_channel = register_value;
		register_value |= 0xC0u;
		control_register = 7u;
		pcm_register = 7u;
		write_pcm_register(pcm_register, register_value);
		z80_block(35);
		register_value = ((register_value & 7u) * 2u) | 0x80u;
		pcm_register = control_register;
		write_pcm_register(pcm_register, register_value);
		z80_block(47);
		register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
		register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
		register_value = ((uint32_t)register_value << 1) | (register_value >> 7);
		register_value = (((uint32_t)register_value << 1) | (register_value >> 7)) & 0xF0u;
		pcm_register = 5u;
		write_pcm_register(pcm_register, register_value);
		z80_block(24);
		pcm_register = 6u;
		write_pcm_register(pcm_register, register_value);
		z80_block(140);
		SAMPLE_VOICE(new_voice)->note = note_code;
		SAMPLE_VOICE(new_voice)->midi_channel = midi_status & 0xFu;
		SAMPLE_VOICE(new_voice)->level = note_level;
		if ((midi_status & 0xFu) == 0xFu)
		{
			z80_block(49);
			order = pcm_voices_used;
			SAMPLE_VOICE(new_voice)->allocation_order = order;
			pcm_voices_used = order + 1u;
		}
		else
		{
			z80_block(73);
			order = pcm_voices_used | 0xF0u;
			SAMPLE_VOICE(new_voice)->allocation_order = order;
			pcm_voices_used = (order & 0xFu) + 1u;
		}
		z80_block(17);
		start_pcm_sample(new_voice);
		z80_block(63);
		SAMPLE_VOICE(new_voice)->flags = 0x50u;
		pitch = kit_pitch;
	}
	z80_block(28);
	register_value = (uint8_t)(pitch >> 8);
	pcm_register = 3u;
	write_pcm_register(pcm_register, register_value);
	z80_block(28);
	register_value = (uint8_t)pitch;
	pcm_register = 2u;
	write_pcm_register(pcm_register, register_value);
	z80_block(91);
	SAMPLE_VOICE(new_voice)->pitch_step = pitch;
	level_product = (pitch & 0xFFu) | ((uint32_t)note_level << 8);
	scale = MIDI_PART(part)->effective_volume;
	level_product = multiply_bytes(scale, level_product);
	z80_block(28);
	register_value = (uint8_t)(level_product >> 8);
	pcm_register = 0u;
	write_pcm_register(pcm_register, register_value);
	z80_block(37);
	register_value = note_pan;
	pcm_register = 1u;
	write_pcm_register(pcm_register, register_value);
	z80_block(99);
	keys_on = pcm_channels_on | pcm_channel_bits[pcm_note_channel];
	pcm_channels_on = keys_on;
	keys_on = ~keys_on;
	pcm_register = 8u;
	write_pcm_register(pcm_register, keys_on);
}

/* A note off for part (MIDI 0x80, or a note on with velocity 0): nothing for a drum kit (program 0xF0
   and up); an FM instrument's goes to fm_note_off; else the sample voice on midi_status's channel
   playing note_code, if it is sounding and not already released, is stopped (stop_pcm_voice) unless the
   channel's sustain pedal is down (channel_controls bit 7), which only marks it released (flags bit 1). */
void note_off(uint16_t part)
{
	uint8_t channel = 0, channel_index = 0, flags = 0, ignored8 = 0, program = 0, voices_left = 0;
	uint16_t ignored16 = 0, ignored16_2 = 0, instrument = 0, voice = 0, voice_size = 0;

	z80_block(31);
	program = MIDI_PART(part)->program;
	if (program >= 0xF0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(27);
	instrument = instruments_ADDRESS;
	instrument = read_table_word(instrument, program);
	z80_block(22);
	if (((INSTRUMENT(instrument)->flags >> 1) & 1u) != 0u)
	{
		fm_note_off(&ignored16, &ignored16_2, &ignored8);
		return;
	}
	z80_block(51);
	voice = SAMPLE_VOICE_ADDRESS(0);
	voice_size = 0x12u;
	voices_left = 8u;
	channel = midi_status & 0xFu;
	for (;;)
	{
		z80_block(29);
		if (channel == SAMPLE_VOICE(voice)->midi_channel)
		{
			z80_block(46);
			channel_index = channel;
			if (note_code == SAMPLE_VOICE(voice)->note)
			{
				z80_block(37);
				flags = SAMPLE_VOICE(voice)->flags;
				if (((flags >> 1) & 1u) == 0u)
				{
					z80_block(17);
					if ((flags & 0x7Fu) != 0u)
						break;
				}
			}
			z80_block(4);
			channel = (uint8_t)channel_index;
		}
		z80_block(23);
		voice += voice_size;
		voices_left--;
		if (voices_left == 0u)
		{
			z80_block(10);
			return;
		}
		z80_cycles(5);
	}
	z80_block(65);
	if (channel_controls[channel_index] >> 7 == 0u)
	{
		stop_pcm_voice(voice);
		return;
	}
	z80_block(33);
	SAMPLE_VOICE(voice)->flags |= 2u;
}

/* Retunes a sounding voice to pitch_bend: FM (chip_channel bit 7), registers A4/A0 from fm_fnum_tables
   plus part's fm_detune; a sample voice, RF5C68 registers 3/2 from pcm_pitch_tables shifted by its
   octave, pitch_step and stream_position following. A sample voice of a nonzero octave uses up the
   caller's count: loop_count's high byte comes back 0. */
uint16_t retune_voice(uint16_t loop_count, uint16_t part, uint16_t voice)
{
	uint32_t carry = 0;
	uint8_t caller_c = 0, count_or_shifts = 0, detune_low = 0, detune_sign = 0, fm_detune = 0, fm_register = 0, fnum_low_register = 0, note = 0, octave = 0, pcm_register = 0, pitch_low_register = 0, register_value = 0, voice_note = 0;
	uint16_t bend = 0, fnum = 0, fnum_entry = 0, fnum_table_entry = 0, old_pitch = 0, pitch = 0, pitch_entry = 0, pitch_table_entry = 0, register_channel = 0;
	uint8_t saved_count_or_shifts = 0, saved_caller_c = 0;
	uint8_t detune_byte = 0, octave_bits = 0;

	count_or_shifts = (uint8_t)(loop_count >> 8);
	caller_c = (uint8_t)loop_count;
	z80_block(37);
	register_value = SAMPLE_VOICE(voice)->chip_channel;
	if (register_value >> 7 != 0u)
	{
		z80_block(313);
		saved_count_or_shifts = count_or_shifts;
		saved_caller_c = caller_c;
		fm_channel = register_value;
		note = SAMPLE_VOICE(voice)->note;
		voice_note = note;
		fnum_table_entry = fm_fnum_tables_ADDRESS + (uint8_t)((note & 0xFu) * 2u);
		bend = pitch_bend;
		fnum_entry = FM_FNUMBER_RUNS(fnum_table_entry)[0] + bend + bend;
		fm_detune = MIDI_PART(part)->fm_detune;
		detune_low = fm_detune;
		detune_byte = fm_detune;
		detune_sign = ((uint32_t)detune_byte << 1) | (detune_byte >> 7);
		carry = detune_byte >> 7;
		fnum = FM_FNUMBERS(fnum_entry)[0] + (detune_low | ((uint32_t)(uint8_t)(0u - carry) << 8));
		fnum = (fnum & 0xFFu) | ((uint32_t)(uint8_t)(((voice_note & ~0xFu) >> 1) + (fnum >> 8)) << 8);
		register_channel = (fm_channel & 3u) << 8;
		register_value = (uint8_t)(fnum >> 8);
		fm_register = 0xA4u | (register_channel >> 8);
		write_fm_channel(fm_register, register_value);
		z80_block(36);
		register_value = (uint8_t)fnum;
		fnum_low_register = 0xA0u | (register_channel >> 8);
		write_fm_channel(fnum_low_register, register_value);
		z80_block(20);
		loop_count = ((uint32_t)saved_count_or_shifts << 8) | saved_caller_c;
		return loop_count;
	}
	z80_block(31);
	register_value |= 0xC0u;
	pcm_register = 7u;
	write_pcm_register(pcm_register, register_value);
	z80_block(177);
	note = SAMPLE_VOICE(voice)->note;
	pitch_table_entry = pcm_pitch_tables_ADDRESS + (uint8_t)((note & 0xFu) * 2u);
	bend = pitch_bend;
	pitch_entry = PCM_PITCH_RUNS(pitch_table_entry)[0] + bend + bend;
	pitch = PCM_PITCH_STEPS(pitch_entry)[0];
	octave_bits = note & ~0xFu;
	octave = octave_bits;
	if (octave_bits == 0u)
	{
		z80_block(28);
		register_value = (uint8_t)(pitch >> 8);
		pcm_register = 3u;
		write_pcm_register(pcm_register, register_value);
		z80_block(28);
		pitch_low_register = 2u;
		register_value = (uint8_t)pitch;
		pcm_register = 2u;
		write_pcm_register(pcm_register, register_value);
		z80_block(196);
		old_pitch = SAMPLE_VOICE(voice)->pitch_step;
		SAMPLE_VOICE(voice)->pitch_step = pitch;
		pitch = pitch - old_pitch + SAMPLE_VOICE(voice)->stream_position;
		SAMPLE_VOICE(voice)->stream_position = pitch;
		loop_count = ((uint32_t)count_or_shifts << 8) | pitch_low_register;
		return loop_count;
	}
	z80_block(33);
	octave = (octave >> 1) | ((uint32_t)octave << 7);
	octave = (octave >> 1) | ((uint32_t)octave << 7);
	octave = (octave >> 1) | ((uint32_t)octave << 7);
	octave = (octave >> 1) | ((uint32_t)octave << 7);
	if (octave < 8u)
	{
		z80_block(4);
		count_or_shifts = octave;
		for (;;)
		{
			z80_block(24);
			carry = (uint8_t)pitch >> 7;
			pitch = (pitch & ~0xFFu) | (uint8_t)((uint32_t)pitch << 1);
			pitch = (pitch & 0xFFu) | ((uint32_t)(uint8_t)(((uint32_t)(pitch >> 8) << 1) | carry) << 8);
			count_or_shifts--;
			if (count_or_shifts == 0u)
				break;
			z80_cycles(5);
		}
		z80_block(28);
		register_value = (uint8_t)(pitch >> 8);
		pcm_register = 3u;
		write_pcm_register(pcm_register, register_value);
		z80_block(28);
		pitch_low_register = 2u;
		register_value = (uint8_t)pitch;
		pcm_register = 2u;
		write_pcm_register(pcm_register, register_value);
		z80_block(196);
		old_pitch = SAMPLE_VOICE(voice)->pitch_step;
		SAMPLE_VOICE(voice)->pitch_step = pitch;
		pitch = pitch - old_pitch + SAMPLE_VOICE(voice)->stream_position;
		SAMPLE_VOICE(voice)->stream_position = pitch;
		loop_count = ((uint32_t)count_or_shifts << 8) | pitch_low_register;
		return loop_count;
	}
	z80_block(19);
	count_or_shifts = (~octave + 1u) & 0xFu;
	for (;;)
	{
		z80_block(24);
		carry = (pitch >> 8) & 1u;
		pitch = (pitch & 0xFFu) | ((uint32_t)((pitch >> 8) >> 1) << 8);
		pitch = (pitch & ~0xFFu) | (uint8_t)(((uint8_t)pitch >> 1) | (carry << 7));
		count_or_shifts--;
		if (count_or_shifts == 0u)
			break;
		z80_cycles(5);
	}
	z80_block(10);
	z80_block(28);
	register_value = (uint8_t)(pitch >> 8);
	pcm_register = 3u;
	write_pcm_register(pcm_register, register_value);
	z80_block(28);
	pitch_low_register = 2u;
	register_value = (uint8_t)pitch;
	pcm_register = 2u;
	write_pcm_register(pcm_register, register_value);
	z80_block(196);
	old_pitch = SAMPLE_VOICE(voice)->pitch_step;
	SAMPLE_VOICE(voice)->pitch_step = pitch;
	pitch = pitch - old_pitch + SAMPLE_VOICE(voice)->stream_position;
	SAMPLE_VOICE(voice)->stream_position = pitch;
	loop_count = ((uint32_t)count_or_shifts << 8) | pitch_low_register;
	return loop_count;
}

/* CC 1 (modulation) for one sounding voice: an FM voice (chip_channel bit 7) gets register 0xB4 of its
   channel = 0xC0 (both outputs) | fm_pan_modulation's depth, as its frequency modulation sensitivity; a
   sample voice is left alone. */
void update_fm_modulation(uint16_t *p_loop_count, uint16_t *p_voice)
{
	uint16_t loop_count = (uint16_t)*p_loop_count;
	uint16_t voice = (uint16_t)*p_voice;
	uint8_t chip_channel = 0, fm_register = 0, pan_register = 0, register_value = 0, voice_chip_channel = 0;
	uint8_t saved_count = 0, saved_voice_chip_channel = 0;

	z80_block(26);
	voice++;
	voice_chip_channel = SAMPLE_VOICE(voice - 0x1u)->chip_channel;
	if (voice_chip_channel >> 7 == 0u)
	{
		z80_cycles(6);
		loop_count = (loop_count & ~0xFFu) | voice_chip_channel;
	}
	else
	{
		z80_block(10);
		z80_block(90);
		saved_count = loop_count >> 8;
		saved_voice_chip_channel = voice_chip_channel;
		chip_channel = voice_chip_channel;
		fm_channel = chip_channel;
		pan_register = (chip_channel & 3u) - 0x4Cu;
		register_value = (fm_pan_modulation & 7u) | 0xC0u;
		fm_register = pan_register;
		write_fm_channel(fm_register, register_value);
		z80_block(26);
		voice--;
		loop_count = ((uint32_t)saved_count << 8) | saved_voice_chip_channel;
	}
	*p_loop_count = loop_count;
	*p_voice = voice;
}

/* Moves midi_queue_read on by one and hands back the midi_queue byte there in byte (and in byte_copy's
   low byte) and its address in queue_slot. */
void read_midi_byte(uint16_t *p_byte_copy, uint16_t *p_queue_slot, uint8_t *p_byte)
{
	uint8_t byte = 0;
	uint16_t byte_copy = 0, queue_slot = 0;

	z80_block(56);
	midi_queue_read++;
	queue_slot = midi_queue_read | 0xE100u;
	byte = z80_read8(queue_slot);
	byte_copy = byte;
	*p_byte_copy = (*p_byte_copy & 0xFF00u) | byte_copy;
	*p_queue_slot = queue_slot;
	*p_byte = byte;
}

/* Unused: one byte from the MIDI port into midi_queue: waits out (resetting) USART errors, reads without
   checking a byte has come, turns 0xF2 into 0x80 and drops the other 0xF0-0xFF bytes. A status byte
   opens a message of its length (3; 2 for 0xC0-0xDF), a lone data byte reopens the last (running
   status); a full message adds 1 to midi_queue_count. */
void receive_midi_byte(void)
{
	uint8_t message_length = 0, midi_byte = 0, received = 0, write_index = 0;
	uint16_t bytes_left_address = 0;
	uint8_t old_bytes_left = 0;

	for (;;)
	{
		z80_block(25);
		if ((port_in(MIDI_USART_CONTROL) & 0x38u) == 0u)
			break;
		z80_block(17);
		reset_midi_errors();
		z80_block(10);
	}
	z80_cycles(5);
	z80_block(28);
	received = port_in(MIDI_USART_DATA);
	if (received == 0xF2u)
	{
		z80_block(7);
		received = 0x80u;
	}
	z80_block(12);
	if (received >= 0xF0u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(35);
	midi_byte = received;
	bytes_left_address = midi_in_bytes_left_ADDRESS;
	if (midi_in_bytes_left != 0u)
	{
		z80_block(72);
		write_index = midi_queue_write;
		z80_write8(0xE100u | write_index, midi_byte);
		midi_queue_write = write_index + 1u;
		old_bytes_left = midi_in_bytes_left;
		midi_in_bytes_left = old_bytes_left - 1u;
		if (old_bytes_left != 1u)
		{
			z80_cycles(6);
			return;
		}
		z80_block(31);
		midi_queue_count++;
		return;
	}
	z80_block(18);
	if (midi_byte >> 7 == 0u)
	{
		z80_block(34);
		midi_in_bytes_left = midi_in_message_length - 1u;
		z80_block(72);
		write_index = midi_queue_write;
		z80_write8(0xE100u | write_index, midi_byte);
		midi_queue_write = write_index + 1u;
		old_bytes_left = midi_in_bytes_left;
		midi_in_bytes_left = old_bytes_left - 1u;
		if (old_bytes_left != 1u)
		{
			z80_cycles(6);
			return;
		}
		z80_block(31);
		midi_queue_count++;
		return;
	}
	z80_block(28);
	message_length = 2u;
	received = midi_byte;
	if (received < 0xC0u)
	{
		z80_block(4);
		message_length++;
	}
	else
	{
		z80_block(17);
		if (received >= 0xE0u)
		{
			z80_block(4);
			message_length++;
			z80_block(24);
			midi_in_bytes_left = message_length;
			midi_in_message_length = message_length;
			z80_block(72);
			write_index = midi_queue_write;
			z80_write8(0xE100u | write_index, midi_byte);
			midi_queue_write = write_index + 1u;
			old_bytes_left = midi_in_bytes_left;
			midi_in_bytes_left = old_bytes_left - 1u;
			if (old_bytes_left != 1u)
			{
				z80_cycles(6);
				return;
			}
			z80_block(31);
			midi_queue_count++;
			return;
		}
	}
	z80_block(24);
	midi_in_bytes_left = message_length;
	midi_in_message_length = message_length;
	z80_block(72);
	write_index = midi_queue_write;
	z80_write8(0xE100u | write_index, midi_byte);
	midi_queue_write = write_index + 1u;
	old_bytes_left = midi_in_bytes_left;
	midi_in_bytes_left = old_bytes_left - 1u;
	if (old_bytes_left != 1u)
	{
		z80_cycles(6);
		return;
	}
	z80_block(31);
	midi_queue_count++;
}

/* Unused: an empty function, a lone return left after receive_midi_byte's own. */
void receive_midi_byte_spare_return_1(void)
{
	z80_block(10);
}

/* Unused: an empty function, a second lone return left after receive_midi_byte, before send_midi_byte. */
void receive_midi_byte_spare_return_2(void)
{
	z80_block(10);
}

/* Unused: sends midi_byte out of the MIDI port once the USART is ready to transmit; an error it reports
   is reset (reset_midi_errors) and the wait starts over. */
void send_midi_byte(uint8_t midi_byte)
{
	uint32_t carry = 0;
	uint8_t status = 0, status_copy = 0;

	for (;;)
	{
		z80_block(38);
		for (;;)
		{
			z80_block(32);
			status = port_in(MIDI_USART_CONTROL);
			status_copy = status;
			if ((status & 0x38u) != 0u)
				break;
			z80_block(18);
			carry = status_copy & 1u;
			if (carry)
			{
				z80_block(63);
				port_out(MIDI_USART_DATA, (uint8_t)midi_byte);
				return;
			}
		}
		z80_block(17);
		reset_midi_errors();
		z80_block(10);
	}
}

/* Unused: command 0x37 to the MIDI USART (its only callers, receive_midi_byte and send_midi_byte, are
   unused too): resets its error flags and turns the transmitter, receiver, DTR and RTS on. */
void reset_midi_errors(void)
{
	z80_block(70);
	port_out(MIDI_USART_CONTROL, 0x37u);
}

/* Unused: sets the MIDI port's 8251 USART up: three 0 commands and an internal reset (0x40), mode 0x4E
   (asynchronous, clock / 16, 8 data bits, no parity, 1 stop bit: MIDI's frame), then command 0x37 as
   reset_midi_errors. */
void init_midi_port(void)
{
	z80_block(329);
	port_out(MIDI_USART_CONTROL, 0u);
	port_out(MIDI_USART_CONTROL, 0u);
	port_out(MIDI_USART_CONTROL, 0u);
	port_out(MIDI_USART_CONTROL, 0x40u);
	port_out(MIDI_USART_CONTROL, 0x4Eu);
	port_out(MIDI_USART_CONTROL, 0x37u);
}
