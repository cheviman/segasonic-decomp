/**
 *    Name:        system/sound.c
 *    Description: the sound the game asks the Z80 for: init_sound, play_sound and the sound queue
 *    Version:     0.1.0
 *    Created:     12-10-2026
 *    Modified:    25-10-2026
 *    Author:      Sebastian Toyos
 *
 *    SegaSonic The Hedgehog (arcade, Sega System 32): the game's V60 program decompiled into C by
 *    tools/decompile.py, with the names of symbols.txt.
 */
#include "game.h"

/* Starts the sound: sound_queue_read 0, sound_queue_write 1 with sound 0x96 queued first, io_cnt bit 2
   set (the sound CPU runs), io_port_d bit 3 cleared and sound_cpu_hold 0 (the Z80 program goes on). */
uint32_t init_sound(void)
{
	uint32_t sound_hold = 0;
	uint8_t cnt_bits = 0, port_d_bits = 0;

	sound_queue_read = 0u;
	sound_queue_write = 1u;
	sound_queue[0] = 0x96u;
	sound_queue[1] = 0u;
	cnt_bits = mem_read8(io_cnt);
	mem_write8(io_cnt, cnt_bits | 4u);
	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits & ~8u);
	sound_hold = 0u;
	mem_write8(sound_cpu_hold, 0u);
	return sound_hold;
}

/* Unused: sets io_port_d bit 3 when unused_coin_byte, game_in_progress or setting_advertise_sound is
   set; init_sound clears that bit. */
void set_port_d_bit_3_if_sound_allowed(void)
{
	uint8_t port_d_bits = 0;

	if (unused_coin_byte != 0u || game_in_progress != 0u || setting_advertise_sound != 0u)
	{
		port_d_bits = mem_read8(io_port_d);
		mem_write8(io_port_d, port_d_bits | 8u);
	}
}

/* Unused: sets io_port_d bit 3 and queues sound 0x7F, the coin sound update_coins plays. */
void queue_coin_sound(void)
{
	uint8_t sound_id = 0;
	uint32_t ignored = 0, ignored_2 = 0;
	uint8_t port_d_bits = 0;

	port_d_bits = mem_read8(io_port_d);
	mem_write8(io_port_d, port_d_bits | 8u);
	sound_id = 0x7Fu;
	queue_sound(sound_id, &ignored, &ignored_2);
}

/* Asks for sound sound_id: during a game (game_in_progress) always through queue_sound; outside one only
   when setting_advertise_sound is on and sound_id's bit is set in attract_sound_mask. */
void play_sound(uint8_t sound_id, uint32_t *p_write_index, uint32_t *p_write_index_address)
{
	if (game_in_progress != 0u)
	{
		queue_sound(sound_id, p_write_index, p_write_index_address);
		return;
	}
	if (setting_advertise_sound == 0u)
		return;
	*p_write_index_address = (uint8_t)sound_id;
	*p_write_index = *p_write_index_address & 0x1Fu;
	*p_write_index_address = *p_write_index_address >> 5;
	if ((attract_sound_mask[*p_write_index_address] & shl32(1u, (int)(*p_write_index))) == 0u)
	{
		return;
	}
	queue_sound(sound_id, p_write_index, p_write_index_address);
}

/* Puts sound_id in sound_queue at sound_queue_write and moves it on, wrapping at 32, with no filter (the
   test mode and the coins use it directly). */
void queue_sound(uint8_t sound_id, uint32_t *p_write_index, uint32_t *p_write_index_address)
{
	uint32_t write_index = 0;

	write_index = sound_queue_write;
	sound_queue[write_index] = sound_id;
	write_index = (uint8_t)(write_index + 1u);
	write_index = write_index & 0x1Fu;
	*p_write_index_address = sound_queue_write_ADDRESS;
	sound_queue_write = write_index;
	*p_write_index = write_index;
}
