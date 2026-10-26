"""
Name:        game_profile.py
Description: what the shared decompiler core (tools/core) needs to know of SegaSonic: where the game is,
             its ROM set, and its settings
Version:     0.1.0
Created:     25-10-2026
Modified:    25-10-2026
Author:      Sebastian Toyos

The core (arcade-decomp: the decompiler, the V60 and Z80 frontends, the analysis, the checks) is the same
for every game; this module gives it SegaSonic's: the repository's root, the ROM set's module
(sonic_rom: the regions as the board has them), the names the generated files carry, and the decompiler's
settings for the V60 program, which it puts in the core's modules when it is imported: the structures
(the objects, 0xA0 bytes, and the three players' blocks from 0x208400), the registers the whole program
shares and what they hold, the game's variables (from R25, 0x208000), the object handler's slots, and the
sound board's chip for the Z80 program (its RF5C68).
"""
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / "core"))

import sonic_rom as rom  # noqa: E402
from decompiler import constants
from decompiler import dataflow
from decompiler import memory
from decompiler import pointers
from decompiler import signatures

ROOT = Path(__file__).resolve().parents[1]
GAME = "SegaSonic The Hedgehog (arcade, Sega System 32)"      # in the generated files' headers
ROM_SET_TITLE = "SegaSonic The Hedgehog (sonic, Japan rev. C)"
CPU = "V60"
BINARY = "sonic"                             # its executables: sonic_game_headless, sonic_headless
GUARD = "SONIC"                              # the generated headers' include guards: SONIC_GAME_H...

# the program's structures, registers and variables (decompiler/pointers.py, memory.py, signatures.py)
pointers.STRUCTS = {"object": 0xA0, "player": 0x200}
pointers.VARIABLES = 0x208000
pointers.PLAYERS = (0x208400, 0x200, 3)
pointers.CONTEXT_KINDS = {20: ("object", 0), 22: ("object", 0), 23: ("object", 0), 24: ("object", 0),
                          18: ("player", 0)}
pointers.OBJECT_REGISTERS = (20, 22, 23, 24)
pointers.OWN_REGISTERS = (20, 23)
pointers.WORK_RAM = (0x200000, 0x210000)
memory.WORK_RAM = (0x200000, 0x210000)
memory.VARIABLES = 0x208000
memory.PLAYER_FIELDS = (0x208400, 0x208A00)
memory.BUS_ONLY = [(0x20E5C4, 0x20E5C6)]
memory.CANONICAL = rom.canonical_address
memory.NOTES = {
    "variables": ["/* the game's variables in work RAM (the original code reaches them from R25, which holds",
                  "   GAME_VARIABLES; some code adds offsets it reads from tables) */"],
    "scoped": ["/* work RAM as one family of functions uses it (their scratch variables) */"],
    "tables": ["/* data in the program ROM the code reads */"],
    "hardware": ["/* the board's registers (read and written through the bus) */"],
}
signatures.CONTEXT = (14, 18, 20, 21, 22, 23, 24, 26, 32)
signatures.INITIAL = {32: 0x20FF00}
signatures.SCRATCH_CONTEXT = {14: 0x0000FFFF}
signatures.CONSTANT = {25: 0x208000}
constants.GLOBALS_BASE = 25
constants.GLOBALS_VALUE = 0x208000
dataflow.HANDLER_SLOTS = (0x1C, 0x9C)

# the sound program's board (z80/emit.py): its banked window and its sound chip; the runtime counts a
# block's cycles alone (z80_block(cycles))
SOUND_BLOCK_FETCHES = False
SOUND_HARDWARE = [(0xA000, 0xC000, "SAMPLE_BANK"), (0xC000, 0xD000, "PCM"), (0xD000, 0xE000, "PCM_RAM")]
SOUND_NOTES = {
    "variables": ["/* the sound program's variables, in the RAM it shares with the V60 (E000-FFFF) */"],
    "scoped": ["/* shared RAM as one family of functions uses it */"],
    "tables": ["/* data in the program ROM the code reads */"],
    "hardware": ["/* the sample bank's window and the RF5C68 (read and written through the bus) */"],
}
