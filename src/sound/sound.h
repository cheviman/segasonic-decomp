/**
 *    Name:        sound.h
 *    Description: the sound program's ports, variables, tables, structures and functions
 *    Version:     0.1.0
 *    Created:     13-10-2026
 *    Modified:    26-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the Z80 sound program decompiled into C by
 *    tools/decompile_sound.py, with the names of sound_symbols.txt.
 */
#ifndef SONIC_SOUND_H
#define SONIC_SOUND_H

#include "sound/support.h"

/* the board's I/O ports the program uses */
#define FM1_ADDRESS 0x80u
#define FM1_DATA 0x81u
#define FM1_PART2_ADDRESS 0x82u
#define FM1_PART2_DATA 0x83u
#define FM2_ADDRESS 0x90u
#define FM2_DATA 0x91u
#define FM2_PART2_ADDRESS 0x92u
#define FM2_PART2_DATA 0x93u
#define ROM_BANK_LOW 0xA0u
#define ROM_BANK_HIGH 0xB0u
#define IRQ_CONTROL 0xC0u
#define IRQ_ACKNOWLEDGE 0xC1u
#define IRQ_VECTOR0_INPUT 0xD0u
#define IRQ_VECTOR1_INPUT 0xD1u
#define IRQ_VECTOR2_INPUT 0xD2u
#define IRQ_MASK 0xD3u
#define IRQ_CONTROL_D8 0xD8u
#define MIDI_USART_DATA 0xF0u
#define MIDI_USART_CONTROL 0xF1u

/* the sound program's variables, in the RAM it shares with the V60 (E000-FFFF) */
#define midi_in_message_length_ADDRESS 0xE000u
#define midi_in_message_length SOUND_RAM8(midi_in_message_length_ADDRESS)
#define midi_in_bytes_left_ADDRESS 0xE001u
#define midi_in_bytes_left SOUND_RAM8(midi_in_bytes_left_ADDRESS)
#define midi_queue_read_ADDRESS 0xE002u
#define midi_queue_read SOUND_RAM8(midi_queue_read_ADDRESS)
#define midi_status_ADDRESS 0xE008u
#define midi_status SOUND_RAM8(midi_status_ADDRESS)
#define sample_or_patch_ADDRESS 0xE00Du
#define sample_or_patch SOUND_RAM8(sample_or_patch_ADDRESS)
#define unread_e010_ADDRESS 0xE010u
#define unread_e010 SOUND_RAM16(unread_e010_ADDRESS)
#define note_code_ADDRESS 0xE012u
#define note_code SOUND_RAM8(note_code_ADDRESS)
#define note_level_ADDRESS 0xE013u
#define note_level SOUND_RAM8(note_level_ADDRESS)
#define pitch_bend_ADDRESS 0xE014u
#define pitch_bend SOUND_RAM16S(pitch_bend_ADDRESS)
#define note_pan_ADDRESS 0xE017u
#define note_pan SOUND_RAM8(note_pan_ADDRESS)
#define fm_pan_modulation_ADDRESS 0xE01Bu
#define fm_pan_modulation SOUND_RAM8(fm_pan_modulation_ADDRESS)
#define music_volume_ADDRESS 0xE01Cu
#define music_volume SOUND_RAM8(music_volume_ADDRESS)
#define kit_pitch_ADDRESS 0xE01Du
#define kit_pitch SOUND_RAM16(kit_pitch_ADDRESS)
#define kit_pitch_low_ADDRESS 0xE01Du
#define kit_pitch_low SOUND_RAM8(kit_pitch_low_ADDRESS)
#define kit_pitch_high_ADDRESS 0xE01Eu
#define kit_pitch_high SOUND_RAM8(kit_pitch_high_ADDRESS)
#define fm_channel_ADDRESS 0xE01Fu
#define fm_channel SOUND_RAM8(fm_channel_ADDRESS)
#define pcm_voices_used_ADDRESS 0xE020u
#define pcm_voices_used SOUND_RAM8(pcm_voices_used_ADDRESS)
#define pcm_note_channel_ADDRESS 0xE025u
#define pcm_note_channel SOUND_RAM8(pcm_note_channel_ADDRESS)
#define pcm_channels_on_ADDRESS 0xE026u
#define pcm_channels_on SOUND_RAM8(pcm_channels_on_ADDRESS)
#define refill_voice_ADDRESS 0xE027u
#define refill_voice SOUND_RAM16(refill_voice_ADDRESS)
#define refill_voices_left_ADDRESS 0xE029u
#define refill_voices_left SOUND_RAM8(refill_voices_left_ADDRESS)
#define rom_bank_low_ADDRESS 0xE02Cu
#define rom_bank_low SOUND_RAM8(rom_bank_low_ADDRESS)
#define rom_bank_high_ADDRESS 0xE02Du
#define rom_bank_high SOUND_RAM8(rom_bank_high_ADDRESS)
#define fm_voices_to_end_ADDRESS 0xE02Eu
#define fm_voices_to_end SOUND_RAM8(fm_voices_to_end_ADDRESS)
#define next_fm_voice_ADDRESS 0xE02Fu
#define next_fm_voice SOUND_RAM16(next_fm_voice_ADDRESS)
#define sample_detune_ADDRESS 0xE031u
#define sample_detune SOUND_RAM8S(sample_detune_ADDRESS)
#define vibrato_voice_ADDRESS 0xE032u
#define vibrato_voice SOUND_RAM16(vibrato_voice_ADDRESS)
#define vibrato_center_ADDRESS 0xE034u
#define vibrato_center SOUND_RAM16(vibrato_center_ADDRESS)
#define new_fm_voice_ADDRESS 0xE036u
#define new_fm_voice SOUND_RAM16(new_fm_voice_ADDRESS)
#define channel_pitch_bends_ADDRESS 0xE040u
#define channel_pitch_bends SOUND_RAM16(channel_pitch_bends_ADDRESS)
#define channel_controls_ADDRESS 0xE060u
#define channel_controls ((uint8_t *)sound_ram_at(channel_controls_ADDRESS))
#define channel_vibrato_presets_ADDRESS 0xE070u
#define channel_vibrato_presets ((uint8_t *)sound_ram_at(channel_vibrato_presets_ADDRESS))
#define midi_queue_ADDRESS 0xE100u
#define midi_queue SOUND_RAM8(midi_queue_ADDRESS)
#define voice_0_allocation_order_ADDRESS 0xE406u
#define voice_0_allocation_order SOUND_RAM8(voice_0_allocation_order_ADDRESS)
#define voice_1_allocation_order_ADDRESS 0xE418u
#define voice_1_allocation_order SOUND_RAM8(voice_1_allocation_order_ADDRESS)
#define voice_2_allocation_order_ADDRESS 0xE42Au
#define voice_2_allocation_order SOUND_RAM8(voice_2_allocation_order_ADDRESS)
#define voice_3_allocation_order_ADDRESS 0xE43Cu
#define voice_3_allocation_order SOUND_RAM8(voice_3_allocation_order_ADDRESS)
#define voice_4_allocation_order_ADDRESS 0xE44Eu
#define voice_4_allocation_order SOUND_RAM8(voice_4_allocation_order_ADDRESS)
#define voice_5_allocation_order_ADDRESS 0xE460u
#define voice_5_allocation_order SOUND_RAM8(voice_5_allocation_order_ADDRESS)
#define voice_6_allocation_order_ADDRESS 0xE472u
#define voice_6_allocation_order SOUND_RAM8(voice_6_allocation_order_ADDRESS)
#define voice_7_allocation_order_ADDRESS 0xE484u
#define voice_7_allocation_order SOUND_RAM8(voice_7_allocation_order_ADDRESS)
#define sound_command_ADDRESS 0xE700u
#define sound_command SOUND_RAM8(sound_command_ADDRESS)
#define sound_command_group_ADDRESS 0xE701u
#define sound_command_group SOUND_RAM8(sound_command_group_ADDRESS)
#define playlist_song_choice_1_ADDRESS 0xE709u
#define playlist_song_choice_1 SOUND_RAM8(playlist_song_choice_1_ADDRESS)
#define playlist_song_choice_2_ADDRESS 0xE70Au
#define playlist_song_choice_2 SOUND_RAM8(playlist_song_choice_2_ADDRESS)
#define player_end_flag_ADDRESS 0xE70Bu
#define player_end_flag SOUND_RAM8(player_end_flag_ADDRESS)
#define tick_countdown_ADDRESS 0xE740u
#define tick_countdown SOUND_RAM8(tick_countdown_ADDRESS)
#define midi_queue_write_ADDRESS 0xE758u
#define midi_queue_write SOUND_RAM8(midi_queue_write_ADDRESS)
#define midi_queue_count_ADDRESS 0xE759u
#define midi_queue_count SOUND_RAM8(midi_queue_count_ADDRESS)
#define lowest_player_field_ADDRESS 0xE770u
#define lowest_player_field SOUND_RAM16(lowest_player_field_ADDRESS)
#define music_player_flags_ADDRESS 0xE900u
#define music_player_flags SOUND_RAM8(music_player_flags_ADDRESS)
#define music_fade_speed_ADDRESS 0xE918u
#define music_fade_speed SOUND_RAM8(music_fade_speed_ADDRESS)
#define music_fade_wait_ADDRESS 0xE919u
#define music_fade_wait SOUND_RAM8(music_fade_wait_ADDRESS)
#define spare_players_ADDRESS 0xF100u
#define spare_players SOUND_RAM8(spare_players_ADDRESS)
#define sound_cpu_hold_ADDRESS 0xFFFFu
#define sound_cpu_hold SOUND_RAM8(sound_cpu_hold_ADDRESS)

/* data in the program ROM the code reads */
#define pcm_channel_bits_ADDRESS 0x06F1u
#define pcm_channel_bits ((const uint8_t *)sound_rom_at(pcm_channel_bits_ADDRESS))
#define part_templates_ADDRESS 0x1594u
#define part_templates SOUND_ROM16(part_templates_ADDRESS)
#define instruments_ADDRESS 0x193Bu
#define instruments SOUND_ROM16(instruments_ADDRESS)
#define note_codes_ADDRESS 0x1A3Eu
#define note_codes ((const uint8_t *)sound_rom_at(note_codes_ADDRESS))
#define pitch_bend_steps_ADDRESS 0x232Eu
#define pitch_bend_steps ((const uint8_t *)sound_rom_at(pitch_bend_steps_ADDRESS))
#define fm_patches_ADDRESS 0x242Eu
#define fm_patches SOUND_ROM16(fm_patches_ADDRESS)
#define drum_kits_ADDRESS 0x3F2Eu
#define drum_kits SOUND_ROM16(drum_kits_ADDRESS)
#define vibrato_presets_ADDRESS 0x45E2u
#define vibrato_presets SOUND_ROM16(vibrato_presets_ADDRESS)
#define sound_groups_ADDRESS 0x4A78u
#define sound_groups SOUND_ROM16(sound_groups_ADDRESS)
#define songs_ADDRESS 0x4C66u
#define songs SOUND_ROM16(songs_ADDRESS)
#define pcm_sample_headers_ADDRESS 0x6429u
#define pcm_sample_headers SOUND_ROM16(pcm_sample_headers_ADDRESS)

/* the sample bank's window and the RF5C68 (read and written through the bus) */
#define ROM_BANK_WINDOW 0xA000u
#define PCM_CONTROL 0xC007u
#define PCM_WAVE_WINDOW 0xD000u
#define PCM_SILENT_LOOP 0xD2BCu

/* data in the program ROM sound_symbols.txt labels */
#define unused_fm_fnum_tables_below_ADDRESS 0x1364u
#define unused_velocity_curve_0_ADDRESS 0x1AFEu
#define unused_velocity_curve_1_ADDRESS 0x1B7Eu
#define unused_velocity_curve_3_ADDRESS 0x1C7Eu
#define unused_velocity_curve_4_ADDRESS 0x1CFEu
#define unused_velocity_curve_5_ADDRESS 0x1D7Eu
#define unused_velocity_curve_6_ADDRESS 0x1DFEu
#define unused_pcm_pitch_tables_below_ADDRESS 0x1E7Eu
#define playlist_command_table_ADDRESS 0x4706u
#define playlist_stages_b_to_f_ADDRESS 0x4D66u
#define playlist_stage_3_ADDRESS 0x4D6Bu
#define playlist_stages_4_8_ADDRESS 0x4D71u
#define playlist_map_ADDRESS 0x4D77u
#define playlist_stages_5_6_ADDRESS 0x4D7Cu
#define playlist_escape_ADDRESS 0x4D81u
#define playlist_stage_clear_ADDRESS 0x4D84u
#define playlist_stage_a_ADDRESS 0x4D87u
#define playlist_stages_9_10_ADDRESS 0x4D8Du
#define playlist_ending_ADDRESS 0x4D93u
#define unplayed_music_playlists_ADDRESS 0x4D96u
#define playlist_reset_music_ADDRESS 0x4DACu
#define unplayed_music_playlists_2_ADDRESS 0x4DB0u
#define playlist_stop_all_ADDRESS 0x4E01u
#define unlisted_music_playlists_ADDRESS 0x4E05u
#define song_records_ADDRESS 0x4E54u
#define stop_music_sequence_ADDRESS 0x4E98u
#define stop_all_voices_sequence_ADDRESS 0x4EA9u
#define sfx_00_ADDRESS 0x4EAFu
#define sfx_crash_ADDRESS 0x4EB7u
#define sfx_explosion_ADDRESS 0x4EBFu
#define sfx_saw_ADDRESS 0x4ECBu
#define sfx_fire_ADDRESS 0x4ED3u
#define sfx_blast_ADDRESS 0x4EDFu
#define sfx_rumble_ADDRESS 0x4EE7u
#define sfx_gate_move_ADDRESS 0x4EEFu
#define sfx_alarm_ADDRESS 0x4EF7u
#define sfx_crumble_ADDRESS 0x4F03u
#define sfx_0a_ADDRESS 0x4F0Bu
#define sfx_quake_ADDRESS 0x4F13u
#define sfx_flow_ADDRESS 0x4F1Bu
#define sfx_0d_ADDRESS 0x4F23u
#define sfx_0e_ADDRESS 0x4F2Bu
#define sfx_ice_block_shake_ADDRESS 0x4F33u
#define sfx_10_ADDRESS 0x4F3Bu
#define sfx_fall_ADDRESS 0x4F43u
#define sfx_12_ADDRESS 0x4F4Bu
#define sfx_electric_shock_ADDRESS 0x4F53u
#define sfx_14_ADDRESS 0x4F5Bu
#define sfx_15_ADDRESS 0x4F63u
#define sfx_16_ADDRESS 0x4F6Bu
#define sfx_launch_ADDRESS 0x4F73u
#define sfx_18_ADDRESS 0x4F7Bu
#define sfx_jump_ADDRESS 0x4F83u
#define sfx_land_ADDRESS 0x4F8Bu
#define sfx_hit_ADDRESS 0x4F93u
#define sfx_1c_ADDRESS 0x4F9Bu
#define sfx_stuck_ADDRESS 0x4FA3u
#define sfx_underwater_explosion_ADDRESS 0x4FABu
#define sfx_bounce_ADDRESS 0x4FB3u
#define sfx_break_free_ADDRESS 0x4FBBu
#define sfx_skid_ADDRESS 0x4FC3u
#define sfx_22_ADDRESS 0x4FCBu
#define sfx_23_ADDRESS 0x4FD3u
#define sfx_ice_block_fall_ADDRESS 0x4FDBu
#define sfx_ring_ADDRESS 0x4FE3u
#define sfx_26_ADDRESS 0x4FEBu
#define sfx_coin_ADDRESS 0x4FF3u
#define sfx_tally_tick_ADDRESS 0x4FFBu
#define sfx_29_ADDRESS 0x5003u
#define sfx_you_failed_ADDRESS 0x500Bu
#define sfx_2b_ADDRESS 0x5013u
#define sfx_2c_ADDRESS 0x501Bu
#define sfx_tally_bonus_ADDRESS 0x5023u
#define sfx_tally_no_bonus_ADDRESS 0x502Bu
#define sfx_2f_ADDRESS 0x5033u
#define unplayed_sfx_32_ADDRESS 0x503Bu
#define unplayed_sfx_33_ADDRESS 0x5043u
#define unplayed_sfx_34_ADDRESS 0x504Bu
#define unplayed_sfx_35_ADDRESS 0x5053u
#define unplayed_sfx_36_ADDRESS 0x505Bu
#define unplayed_sfx_37_ADDRESS 0x5063u
#define unplayed_sfx_38_ADDRESS 0x506Bu
#define unplayed_sfx_39_ADDRESS 0x5073u
#define unplayed_sfx_3a_ADDRESS 0x507Bu
#define unplayed_sfx_3b_ADDRESS 0x5083u
#define unplayed_sfx_3c_ADDRESS 0x508Bu
#define unplayed_sfx_3d_ADDRESS 0x5093u
#define unplayed_sfx_3e_ADDRESS 0x509Bu
#define unplayed_sfx_3f_ADDRESS 0x50A3u
#define unplayed_sfx_40_ADDRESS 0x50ABu
#define unplayed_sfx_41_ADDRESS 0x50B3u
#define unplayed_sfx_42_ADDRESS 0x50BBu
#define unplayed_sfx_43_ADDRESS 0x50C3u
#define unplayed_sfx_44_ADDRESS 0x50CBu
#define unplayed_sfx_45_ADDRESS 0x50D3u
#define unplayed_sfx_46_ADDRESS 0x50DBu
#define unplayed_sfx_47_ADDRESS 0x50E3u
#define unplayed_sfx_48_ADDRESS 0x50EBu
#define unplayed_sfx_49_ADDRESS 0x50F3u
#define unplayed_sfx_4a_ADDRESS 0x50FBu
#define unplayed_sfx_4b_ADDRESS 0x5103u
#define unplayed_sfx_4c_ADDRESS 0x510Bu
#define unplayed_sfx_4d_ADDRESS 0x5113u
#define unplayed_sfx_4e_ADDRESS 0x511Bu
#define unplayed_sfx_4f_ADDRESS 0x5123u
#define unplayed_sfx_50_ADDRESS 0x512Bu
#define unplayed_sfx_51_ADDRESS 0x5133u
#define unplayed_sfx_52_ADDRESS 0x513Bu
#define unplayed_sfx_53_ADDRESS 0x5143u
#define unplayed_sfx_54_ADDRESS 0x514Bu
#define unplayed_sfx_55_ADDRESS 0x5153u
#define unplayed_sfx_56_ADDRESS 0x515Bu
#define unplayed_sfx_57_ADDRESS 0x5163u
#define unplayed_sfx_58_ADDRESS 0x516Bu
#define unplayed_sfx_59_ADDRESS 0x5173u
#define unplayed_sfx_5a_ADDRESS 0x517Bu
#define unplayed_sfx_5b_ADDRESS 0x5183u
#define unplayed_sfx_5c_ADDRESS 0x518Bu
#define unplayed_sfx_5d_ADDRESS 0x5193u
#define unplayed_sfx_5e_ADDRESS 0x519Bu
#define unplayed_sfx_5f_ADDRESS 0x51A3u
#define unplayed_sfx_60_ADDRESS 0x51ABu
#define unplayed_sfx_61_ADDRESS 0x51B3u
#define unplayed_sfx_62_ADDRESS 0x51BBu
#define unplayed_sfx_63_ADDRESS 0x51C3u
#define unplayed_sfx_64_ADDRESS 0x51CBu
#define unplayed_sfx_65_ADDRESS 0x51D3u
#define unplayed_sfx_66_ADDRESS 0x51DBu
#define unplayed_sfx_67_ADDRESS 0x51E3u
#define unplayed_sfx_68_ADDRESS 0x51EBu
#define unplayed_sfx_69_ADDRESS 0x51F3u
#define unplayed_sfx_6a_ADDRESS 0x51FBu
#define unplayed_sfx_6b_ADDRESS 0x5203u
#define unplayed_sfx_6c_ADDRESS 0x520Bu
#define unplayed_sfx_6d_ADDRESS 0x5213u
#define unplayed_sfx_6e_ADDRESS 0x521Bu
#define unplayed_sfx_6f_ADDRESS 0x5223u
#define unplayed_sfx_70_ADDRESS 0x522Bu
#define unplayed_sfx_71_ADDRESS 0x5233u
#define unplayed_load_part_template_00_ADDRESS 0x523Bu
#define unplayed_load_part_template_01_ADDRESS 0x5243u
#define unplayed_load_part_template_02_ADDRESS 0x524Bu
#define unplayed_load_part_template_03_ADDRESS 0x5253u
#define unplayed_load_part_template_04_ADDRESS 0x525Bu
#define unplayed_load_part_template_05_ADDRESS 0x5263u
#define unplayed_load_part_template_06_ADDRESS 0x526Bu
#define unplayed_load_part_template_07_ADDRESS 0x5273u
#define unplayed_load_part_template_08_ADDRESS 0x527Bu
#define unplayed_load_part_template_09_ADDRESS 0x5283u
#define unplayed_load_part_template_0a_ADDRESS 0x528Bu
#define unplayed_load_part_template_0b_ADDRESS 0x5293u
#define unplayed_channel_10_notes_off_ADDRESS 0x529Bu
#define unplayed_channel_10_volume_ADDRESS 0x52A3u
#define control_stop_all_voices_ADDRESS 0x52ABu
#define unsent_stop_all_voices_ADDRESS 0x52B3u
#define voice_sonic_scream_ADDRESS 0x52BBu
#define voice_eggman_01_ADDRESS 0x52C3u
#define voice_ray_struggle_ADDRESS 0x52CFu
#define voice_eggman_03_ADDRESS 0x52D7u
#define voice_04_ADDRESS 0x52E3u
#define voice_05_ADDRESS 0x52EFu
#define voice_sonic_struggle_ADDRESS 0x52FBu
#define voice_sonic_cry_ADDRESS 0x5303u
#define voice_ray_scream_ADDRESS 0x530Bu
#define voice_sonic_teeter_ADDRESS 0x5313u
#define voice_sonic_screen_edge_ADDRESS 0x531Bu
#define voice_sonic_tumble_ADDRESS 0x5323u
#define voice_eggman_0c_ADDRESS 0x532Bu
#define voice_ray_teeter_ADDRESS 0x5337u
#define voice_mighty_struggle_ADDRESS 0x533Fu
#define voice_eggman_0f_ADDRESS 0x5347u
#define voice_sonic_caged_ADDRESS 0x5353u
#define voice_mighty_caged_ADDRESS 0x535Fu
#define voice_eggman_booth_ADDRESS 0x536Bu
#define voice_13_ADDRESS 0x5377u
#define voice_ray_screen_edge_ADDRESS 0x537Fu
#define voice_mighty_teeter_ADDRESS 0x5387u
#define voice_16_ADDRESS 0x538Fu
#define voice_ray_cry_ADDRESS 0x5397u
#define voice_sonic_over_edge_ADDRESS 0x539Fu
#define voice_sonic_19_ADDRESS 0x53A7u
#define voice_mighty_1a_ADDRESS 0x53AFu
#define voice_ray_hurt_ADDRESS 0x53B7u
#define voice_ray_chooser_ADDRESS 0x53BFu
#define voice_mighty_screen_edge_ADDRESS 0x53C7u
#define voice_1e_ADDRESS 0x53CFu
#define voice_ray_stage_b_ADDRESS 0x53D7u
#define voice_ray_over_edge_ADDRESS 0x53DFu
#define voice_sonic_join_ADDRESS 0x53E7u
#define voice_mighty_entry_ADDRESS 0x53EFu
#define voice_mighty_restart_ADDRESS 0x53F7u
#define voice_sonic_chooser_ADDRESS 0x53FFu
#define voice_25_ADDRESS 0x5407u
#define voice_sonic_hurt_ADDRESS 0x540Fu
#define voice_mighty_cry_ADDRESS 0x5417u
#define voice_sonic_28_ADDRESS 0x541Fu
#define voice_sonic_stage_b_ADDRESS 0x5427u
#define voice_2a_ADDRESS 0x542Fu
#define voice_2b_ADDRESS 0x5437u
#define voice_mighty_hurt_ADDRESS 0x543Fu
#define voice_ray_caged_ADDRESS 0x5447u
#define voice_sonic_restart_ADDRESS 0x5453u
#define voice_2f_ADDRESS 0x545Bu
#define voice_30_ADDRESS 0x5463u
#define voice_mighty_stage_b_ADDRESS 0x546Bu
#define voice_ray_tumble_ADDRESS 0x5473u
#define voice_mighty_join_ADDRESS 0x547Bu
#define voice_34_ADDRESS 0x5483u
#define voice_sonic_entry_ADDRESS 0x548Bu
#define voice_ray_36_ADDRESS 0x5493u
#define voice_37_ADDRESS 0x549Bu
#define voice_mighty_over_edge_ADDRESS 0x54A3u
#define voice_ray_restart_ADDRESS 0x54ABu
#define voice_3a_ADDRESS 0x54B3u
#define voice_3b_ADDRESS 0x54BBu
#define voice_ray_entry_ADDRESS 0x54C3u
#define voice_mighty_3d_ADDRESS 0x54CBu
#define voice_ray_3e_ADDRESS 0x54D3u
#define voice_mighty_tumble_ADDRESS 0x54DBu
#define voice_ray_join_ADDRESS 0x54E3u
#define voice_eggman_41_ADDRESS 0x54EBu
#define voice_42_ADDRESS 0x54F7u
#define voice_eggman_43_ADDRESS 0x5503u
#define voice_44_ADDRESS 0x550Fu
#define voice_eggman_45_ADDRESS 0x551Bu
#define voice_eggman_46_ADDRESS 0x5527u
#define voice_47_ADDRESS 0x5533u
#define voice_mighty_scream_ADDRESS 0x553Fu
#define voice_49_ADDRESS 0x5547u
#define voice_4a_ADDRESS 0x5553u
#define voice_eggman_4b_ADDRESS 0x555Bu
#define unlisted_bend_sweep_program_79_ADDRESS 0x5567u
#define unlisted_bend_sweep_program_7a_ADDRESS 0x5689u
#define unlisted_bend_sweep_program_7b_ADDRESS 0x57ABu
#define unlisted_bend_sweep_program_7c_ADDRESS 0x58CDu
#define unlisted_bend_sweep_program_7d_ADDRESS 0x59EFu
#define unlisted_bend_sweep_program_7e_ADDRESS 0x5B11u
#define unlisted_bend_sweep_program_7f_ADDRESS 0x5C33u
#define sfx_30_ADDRESS 0x5D55u
#define sfx_31_ADDRESS 0x5D74u
#define unlisted_sound_5da5_ADDRESS 0x5DA5u
#define unlisted_sound_5dc7_ADDRESS 0x5DC7u
#define unlisted_sound_5de9_ADDRESS 0x5DE9u
#define unlisted_sound_5e0b_ADDRESS 0x5E0Bu
#define unlisted_sound_5e2d_ADDRESS 0x5E2Du
#define unlisted_sound_5e4f_ADDRESS 0x5E4Fu
#define unlisted_sound_5e71_ADDRESS 0x5E71u
#define unlisted_sound_5e89_ADDRESS 0x5E89u
#define unlisted_sound_5ea1_ADDRESS 0x5EA1u
#define unlisted_sound_5eb9_ADDRESS 0x5EB9u
#define unlisted_sound_5ed1_ADDRESS 0x5ED1u
#define unlisted_sound_5ee9_ADDRESS 0x5EE9u
#define unlisted_sound_5f01_ADDRESS 0x5F01u
#define unlisted_sound_5f19_ADDRESS 0x5F19u
#define unlisted_sound_5f31_ADDRESS 0x5F31u
#define unlisted_sound_5f53_ADDRESS 0x5F53u
#define unlisted_sound_5f75_ADDRESS 0x5F75u
#define unlisted_sound_5f97_ADDRESS 0x5F97u
#define unlisted_sound_5fb9_ADDRESS 0x5FB9u
#define unlisted_sound_5fdb_ADDRESS 0x5FDBu
#define unlisted_sound_5ffd_ADDRESS 0x5FFDu
#define unlisted_sound_601f_ADDRESS 0x601Fu
#define unlisted_sound_6041_ADDRESS 0x6041u
#define unlisted_sound_606d_ADDRESS 0x606Du
#define unlisted_sound_6099_ADDRESS 0x6099u
#define unlisted_sound_60c5_ADDRESS 0x60C5u
#define unlisted_sound_60f1_ADDRESS 0x60F1u
#define unlisted_sound_611d_ADDRESS 0x611Du
#define unlisted_sound_6149_ADDRESS 0x6149u
#define unlisted_sound_6175_ADDRESS 0x6175u
#define unlisted_sound_61a1_ADDRESS 0x61A1u
#define unlisted_sound_61c3_ADDRESS 0x61C3u

/* the sound program's structures in the shared RAM */
#pragma pack(push, 1)
struct fm_voice
{
	uint8_t flags;
	uint8_t chip_channel;
	uint8_t note;
	uint8_t midi_channel;
	uint8_t pad_04[0x1];
	uint8_t level;
	uint8_t pad_06[0x1];
	uint8_t vibrato_delay;
	uint8_t vibrato_wait;
	uint8_t vibrato_range;
	uint8_t vibrato_depth;
	uint8_t vibrato_rise;
	uint8_t vibrato_fall;
	uint8_t vibrato_phase;
	uint8_t vibrato_rate;
	union
	{
		uint16_t vibrato_offset;
		uint8_t vibrato_offset_low;
		struct
		{
			uint8_t pad_0F[0x1];
			uint8_t vibrato_offset_high;
		};
	};
	uint8_t vibrato_block;
};

struct midi_part
{
	uint8_t flags;
	uint8_t midi_channel;
	uint8_t volume;
	uint8_t program;
	int8_t transpose;
	int8_t fm_detune;
	uint8_t pad_06[0x2];
	uint8_t effective_volume;
};

struct sample_voice
{
	uint8_t flags;
	uint8_t chip_channel;
	uint8_t note;
	uint8_t midi_channel;
	uint8_t pad_04[0x1];
	uint8_t level;
	uint8_t allocation_order;
	uint8_t sample_bank_high;
	uint8_t sample_bank_low;
	uint8_t wave_page;
	uint8_t source_page;
	uint8_t header_bank_step;
	uint8_t header_address_low;
	uint8_t header_address_high;
	uint16_t pitch_step;
	union
	{
		uint16_t stream_position;
		uint8_t stream_position_low;
		struct
		{
			uint8_t pad_10[0x1];
			uint8_t stream_position_high;
		};
	};
};

struct sequence_player
{
	uint8_t flags;
	uint8_t pad_01[0x1];
	union
	{
		uint16_t position;
		uint8_t position_low;
		struct
		{
			uint8_t pad_02[0x1];
			uint8_t position_high;
		};
	};
	uint8_t pad_04[0x2];
	union
	{
		uint16_t delta_time;
		uint8_t delta_time_low;
		struct
		{
			uint8_t pad_06[0x1];
			uint8_t delta_time_high;
		};
	};
	uint8_t pad_08[0x2];
	uint8_t meta_51_value;
	uint8_t pad_0B[0x9];
	uint8_t running_status;
	uint8_t fade_volume;
	uint8_t pad_16[0x2];
	uint8_t fade_speed;
	uint8_t fade_wait;
	uint8_t fade_channel_volume;
	uint8_t bank;
	uint8_t pad_1C[0x4];
	union
	{
		uint16_t playlist;
		uint8_t playlist_low;
		struct
		{
			uint8_t pad_20[0x1];
			uint8_t playlist_high;
		};
	};
	uint8_t pad_22[0xDE];
};

struct instrument
{
	uint8_t flags;
	uint8_t sample_or_fm_patch;
	uint8_t pad_02[0x1];
	uint8_t pan;
	int8_t detune;
};

struct drum_note
{
	union
	{
		uint16_t pitch;
		uint8_t pitch_low;
		struct
		{
			uint8_t pad_00[0x1];
			uint8_t pitch_high;
		};
	};
	uint8_t pan;
	uint8_t sample;
};

struct vibrato_preset
{
	uint8_t delay;
	uint8_t wait;
	uint8_t range;
	uint8_t depth;
	uint8_t rise;
	uint8_t fall;
	uint8_t phase;
};

struct pcm_sample_header
{
	uint8_t bank_high;
	uint8_t bank_low;
	uint8_t start_page;
	uint8_t bank_step;
	uint8_t address_low;
	uint8_t address_high;
};

struct register_value_pair
{
	uint8_t register_number;
	uint8_t value;
};

struct sound_list
{
	uint8_t last_number;
};

struct sound_header
{
	uint8_t flags;
	uint8_t first_song;
};

struct song_record
{
	uint8_t flags;
	uint8_t bank;
	union
	{
		uint16_t sequence;
		uint8_t sequence_low;
		struct
		{
			uint8_t pad_02[0x1];
			uint8_t sequence_high;
		};
	};
};

struct effect_header
{
	uint8_t flags;
	uint8_t first_status;
};

#pragma pack(pop)

_Static_assert(sizeof(struct fm_voice) == 0x12, "struct fm_voice has the size of the game's blocks");
_Static_assert(sizeof(struct midi_part) == 0x9, "struct midi_part has the size of the game's blocks");
_Static_assert(sizeof(struct sample_voice) == 0x12, "struct sample_voice has the size of the game's blocks");
_Static_assert(sizeof(struct sequence_player) == 0x100, "struct sequence_player has the size of the game's blocks");
_Static_assert(sizeof(struct instrument) == 0x5, "struct instrument has the size of the program ROM's records");
_Static_assert(sizeof(struct drum_note) == 0x4, "struct drum_note has the size of the program ROM's records");
_Static_assert(sizeof(struct vibrato_preset) == 0x7, "struct vibrato_preset has the size of the program ROM's records");
_Static_assert(sizeof(struct pcm_sample_header) == 0x6, "struct pcm_sample_header has the size of the program ROM's records");
_Static_assert(sizeof(struct register_value_pair) == 0x2, "struct register_value_pair has the size of the program ROM's records");
_Static_assert(sizeof(struct sound_list) == 0x1, "struct sound_list has the size of the program ROM's records");
_Static_assert(sizeof(struct sound_header) == 0x2, "struct sound_header has the size of the program ROM's records");
_Static_assert(sizeof(struct song_record) == 0x4, "struct song_record has the size of the program ROM's records");
_Static_assert(sizeof(struct effect_header) == 0x2, "struct effect_header has the size of the program ROM's records");

#define FM_VOICE(address) ((struct fm_voice *)sound_ram_at(address))
#define MIDI_PART(address) ((struct midi_part *)sound_ram_at(address))
#define SAMPLE_VOICE(address) ((struct sample_voice *)sound_ram_at(address))
#define SEQUENCE_PLAYER(address) ((struct sequence_player *)sound_ram_at(address))
#define INSTRUMENT(address) ((const struct instrument *)sound_rom_at(address))
#define DRUM_NOTE(address) ((const struct drum_note *)sound_rom_at(address))
#define VIBRATO_PRESET(address) ((const struct vibrato_preset *)sound_rom_at(address))
#define PCM_SAMPLE_HEADER(address) ((const struct pcm_sample_header *)sound_rom_at(address))
#define REGISTER_VALUE_PAIR(address) ((const struct register_value_pair *)sound_rom_at(address))
#define VOICE_CHIP_CODES(address) ((const uint8_t *)sound_rom_at(address))
#define NOTE_LEVELS(address) ((const uint8_t *)sound_rom_at(address))
#define FM_FNUMBERS(address) ((const uint16_t *)sound_rom_at(address))
#define FM_FNUMBER_RUNS(address) ((const uint16_t *)sound_rom_at(address))
#define PCM_PITCH_STEPS(address) ((const uint16_t *)sound_rom_at(address))
#define PCM_PITCH_RUNS(address) ((const uint16_t *)sound_rom_at(address))
#define FM_REGISTER_NUMBERS(address) ((const uint8_t *)sound_rom_at(address))
#define FM_PATCH_BYTES(address) ((const uint8_t *)sound_rom_at(address))
#define PLAYLIST_BYTES(address) ((const uint8_t *)sound_rom_at(address))
#define TABLE_WORDS(address) ((const uint16_t *)sound_rom_at(address))
#define SOUND_LIST(address) ((const struct sound_list *)sound_rom_at(address))
#define SOUND_HEADER(address) ((const struct sound_header *)sound_rom_at(address))
#define SONG_RECORD(address) ((const struct song_record *)sound_rom_at(address))
#define EFFECT_HEADER(address) ((const struct effect_header *)sound_rom_at(address))

/* the program ROM's tables of records, by name: name[i].field */
#define fm_global_init_ADDRESS 0x0273u
#define fm_global_init ((const struct register_value_pair *)sound_rom_at(fm_global_init_ADDRESS))
#define fm_operator_init_ADDRESS 0x0289u
#define fm_operator_init ((const struct register_value_pair *)sound_rom_at(fm_operator_init_ADDRESS))
#define pcm_register_init_ADDRESS 0x031Bu
#define pcm_register_init ((const struct register_value_pair *)sound_rom_at(pcm_register_init_ADDRESS))
#define voice_chip_channels_ADDRESS 0x0387u
#define voice_chip_channels ((const uint8_t *)sound_rom_at(voice_chip_channels_ADDRESS))
#define fm_patch_registers_ADDRESS 0x134Bu
#define fm_patch_registers ((const uint8_t *)sound_rom_at(fm_patch_registers_ADDRESS))
#define fm_level_registers_ADDRESS 0x1360u
#define fm_level_registers ((const uint8_t *)sound_rom_at(fm_level_registers_ADDRESS))
#define fm_fnum_tables_ADDRESS 0x137Cu
#define fm_fnum_tables ((const uint16_t *)sound_rom_at(fm_fnum_tables_ADDRESS))
#define velocity_levels_ADDRESS 0x1BFEu
#define velocity_levels ((const uint8_t *)sound_rom_at(velocity_levels_ADDRESS))
#define pcm_pitch_tables_ADDRESS 0x1E96u
#define pcm_pitch_tables ((const uint16_t *)sound_rom_at(pcm_pitch_tables_ADDRESS))
#define FM_VOICE_ADDRESS(n) (0xE490u + (n) * 0x12u)    /* the 12 of them, from 0 */
#define MIDI_PART_ADDRESS(n) (0xE300u + (n) * 0x9u)    /* the 13 of them, from 0 */
#define SAMPLE_VOICE_ADDRESS(n) (0xE400u + (n) * 0x12u)    /* the 8 of them, from 0 */
#define SEQUENCE_PLAYER_ADDRESS(n) (0xE900u + (n) * 0x100u)    /* the 8 of them, from 0 */

/* main.c: the sound program's frame: the reset, the timer interrupt (the players' tick), the main loop and sound_init; the two YM3438 and the RF5C68 set up and written; the tracks' and MIDI parts' start values; read_table_word, multiply_bytes, copy_sample_page */
void sound_reset(void);
void sound_irq(void);
void sound_main_loop(void);
void sound_init(uint8_t unused_c);
uint16_t init_fm_chips(void);
uint8_t write_fm_global(uint8_t fm_register, uint8_t value);
void write_fm_channel(uint8_t fm_register, uint8_t value);
void write_fm1(uint8_t fm_register, uint8_t value);
void write_fm1_part2(uint8_t fm_register, uint8_t value);
void write_fm2(uint8_t fm_register, uint8_t value);
void write_fm2_part2(uint8_t fm_register, uint8_t value);
void init_pcm_chip(uint32_t unused_c);
void write_pcm_register(uint8_t pcm_register, uint8_t register_value);
void init_track_channels(uint32_t unused_c);
void init_pcm_tracks(void);
void init_midi_parts(void);
uint16_t read_table_word(uint16_t table, uint8_t index);
uint16_t multiply_bytes(uint16_t factor, uint16_t value);
void copy_sample_page(uint16_t *p_wave_address, uint16_t *p_sample_address);

/* midi.c: the MIDI messages of the queue for each part on their channel: note on and off, pitch bend, control and program changes, picking a sample voice, retuning, FM modulation; the unused MIDI port on an 8251 USART (ports F0 and F1) */
void handle_midi_message(void);
void skip_midi_data(uint8_t first_byte);
void play_midi_message(uint8_t message_byte, uint16_t part);
void scan_drum_note_voices(uint16_t note_and_velocity, uint16_t part);
void allocate_sample_voice(uint8_t note_velocity, uint16_t part);
void note_off(uint16_t part);
uint16_t retune_voice(uint16_t loop_count, uint16_t part, uint16_t voice);
void update_fm_modulation(uint16_t *p_loop_count, uint16_t *p_voice);
void read_midi_byte(uint16_t *p_byte_copy, uint16_t *p_queue_slot, uint8_t *p_byte);
void receive_midi_byte(void);
void receive_midi_byte_spare_return_1(void);
void receive_midi_byte_spare_return_2(void);
void send_midi_byte(uint8_t midi_byte);
void reset_midi_errors(void);
void init_midi_port(void);

/* sequences.c: the sequence players stepped at every tick (their events into the MIDI queue, the playlist commands) and the V60's sound commands (songs and sequences started on players, volume fades) */
void sequence_tick(void);
void playlist_command(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page, uint16_t *p_playlist, uint8_t *p_playlist_byte);
void step_player_if_playing(uint16_t player_address, uint16_t mailbox_page);
void step_player(uint16_t player_address, uint16_t mailbox_page);
uint8_t chain_next_event(uint16_t player, uint8_t data_byte);
void clear_player_end_flag(uint16_t mailbox_page);
void poll_sound_command(void);
void start_song(uint16_t player, uint8_t song_number);
void start_sequence(uint16_t header, uint16_t player);
uint16_t find_lowest_player(uint16_t player_field);
void select_channel_player(uint16_t *p_header, uint16_t *p_player);
uint16_t clear_player(uint16_t player);
void fade_music_volume(uint16_t mailbox_page);
void fade_player_volume(uint16_t queue_pointer, uint16_t player, uint16_t mailbox_page);
uint16_t next_sequence_byte(uint16_t cursor, uint16_t player);

/* voices.c: the voices: the RF5C68's sample voices (starting a sample, refilling its pages, stopping it) and the YM3438's FM voices (note off, release with the sustain pedal, volumes, patches, the vibrato) */
void start_pcm_sample(uint16_t voice);
void refill_pcm_voice(void);
void stop_pcm_voice(uint16_t voice);
void fm_note_off(uint16_t *p_voice_size, uint16_t *p_saved_channel, uint8_t *p_channel);
uint8_t stop_fm_voice(uint16_t voice);
void release_held_fm_voice(uint16_t voice);
void apply_fm_music_volume(uint8_t chip_channel, uint16_t level, uint16_t part);
void apply_fm_channel_volume(uint8_t chip_channel, uint16_t level, uint16_t part);
void load_fm_patch(uint16_t *p_channel_in_part, uint16_t *p_register_list, uint16_t *p_patch_data);
void step_fm_vibrato(uint16_t voice);
uint16_t load_fixed_fm_frequency(void);

#endif
