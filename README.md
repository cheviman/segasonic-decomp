# SegaSonic The Hedgehog — native port

A reconstruction of Sega's 1993 arcade game *SegaSonic The Hedgehog* (Sega System 32) into
platform-agnostic C — not an emulator — with ports to PC, Nintendo Switch and Linux ARM
handhelds (Anbernic RG40XX) on top of it.

**Apart from the screenshots on this page, this repository contains no game data.** The game's program
is here as C, decompiled from the original machine code (`src/`); as with
[n64decomp/sm64](https://github.com/n64decomp/sm64), you bring your own ROM for everything else: the
tools verify it and extract the assets and the data the program reads, which the game loads when it
starts. No executable carries the ROM or anything from it.

<p align="center">
  <img src="docs/screenshots/title.png" width="32%" alt="Title screen">
  <img src="docs/screenshots/intro.png" width="32%" alt="The intro: Mighty, Ray and Sonic in front of Eggman's machine">
  <img src="docs/screenshots/three_players.png" width="32%" alt="Three players on the ice stage">
  <img src="docs/screenshots/island_map.png" width="32%" alt="The island map between stages">
  <img src="docs/screenshots/lava_cage.png" width="32%" alt="A cage over the lava">
  <img src="docs/screenshots/ending.png" width="32%" alt="The ending">
</p>
<p align="center"><sub>Frames drawn by the game's own video library (<code>runtime/src/video.c</code>) while the
decompiled game plays the scenarios of <code>tests/controls/</code>.</sub></p>

## Status

- [x] ROM set verification (MAME set `sonic`, Japan rev. C), by CRC32 and SHA-1 of every file
- [x] NEC V60 disassembler, output identical to MAME's at all 786,432 byte offsets of the program ROM
- [x] Recursive disassembly that follows branch tables, pointer tables and stored code pointers,
      cross-checked against real execution: 541 functions, about 26,000 instructions, no invalid
      or overlapping paths, and every executed address lands on a decoded instruction
- [x] Asset extraction into plain resources, in folders by subject (see "The assets" below): the 6,138
      sprite frames in 291 sheets, an animation family or an object a sheet (`sprites/sonic/walking.png`,
      nine directions of sixteen frames), in the colors the game shows them with, showing what the board
      draws; 16 tile sheets in color; the program's data as text, a file a subject with a section a piece,
      each in a format of its own (palettes, the scripts that build the stages' maps, the characters'
      animations, floor blocks, object placements and spawn lists, restart points and paths, texts, fonts,
      the demos' recorded inputs), naming every sprite frame by its sheet (`sonic/walking[3]`); and the
      sound: the driver's tables as text (instruments, FM patches as an FM editor shows them, drum kits,
      pitch tables), the music and every sound effect and voice as a Standard MIDI File that plays at the
      game's speed, named by where the game plays it, and the 175 PCM samples as WAV. Everything but the
      code builds back from those files byte for byte (`extract_assets.py --check`)
- [x] The game built from its assets: the build makes the game's data from `assets/` (the code, which the
      C replaces, comes from the ROM), so that what is changed there goes into the game: a sprite, a
      palette, a stage's spawn list, an instrument, a sample, a song (the songs may change length: the
      build lays them out anew)
- [x] System 32 board model in C (`runtime/`): memory map, interrupt controller and timers, I/O chip,
      EEPROM, trackballs, palette and mixer registers, and the FD1149 (317-0213) level-load protection
- [x] Reference run of the original program on MAME's V60 core against that model: boots, shows the
      region warning and runs the attract mode for 10 minutes of game time
- [x] V60 to C translation (the scaffold, `tools/recompile.py`): the game runs as native C, and its
      work RAM, video RAM, sprite RAM, palette and mixer match the reference run at the end of every frame
- [x] Decompiler (`tools/decompile.py`, which runs [arcade-decomp](https://github.com/cheviman/arcade-decomp),
      the decompiler and analysis tools this game shares with
      [outrunners-decomp](https://github.com/cheviman/outrunners-decomp), in `tools/core/`): one C function
      per original function, with no CPU left in it:
      registers become named parameters, results and typed local variables, the stack and the condition
      flags are gone, branches become `if`/`while`/`do`/`switch` with `||` and `&&`, a jump through a
      table of code becomes a `switch` on the index, and the game's variables, tables, structures
      (objects, players, and each object type's own variables) and the board's registers are read and
      written by name; addresses the code uses as values print as names (`TEXT_CELL(14, 20)`,
      `PALETTE_RAM + 0x83A0u`, `NAME_ADDRESS`). Code two functions share is written once (the player's
      update runs into the state code of its animation, which takes an `entry_point`; each player state
      the update jumps to is a function of its own). The code nothing in the program reaches (about
      14 KB: drawing routines, a packed picture decoder and an LZ unpacker, object types never spawned,
      earlier versions of live routines) is decompiled too, so the C holds the whole program. It matches
      the reference frame by frame in every scenario of `tests/controls/`
- [x] Names: every function, variable, table, structure field and board register has one in
      `symbols.txt`, with the evidence for it, and so do the parameters and results of the functions,
      the places the C's control variables are named after, and what each object type keeps in the object
      structure
      (`OBJECT(current_object)->fire_front.path`, per family of functions, wherever the code shows which
      type an object is: its own, one it gives a handler, one a typed link leads to); local variables have
      the names `symbols.txt` gives them by the instructions that write them, or take the name of what they
      hold, the field they were read from or the parameter they are passed as. Every byte of the program
      ROM is code or named data. The records the code walks are structures: 201 in the game (181 in the
      program ROM: floor blocks and slopes, spawn lists, the chasers' paths, animations, texts and VRAM
      scripts, the ending's timed frames...; 20 in the work RAM: ranking records, palette cycles, sprite
      part lists...) and 13 in the sound program (instruments, register pairs, songs...), with 59 + 10
      kinds of values the code walks one at a time (a text's characters, a list of sprite frames) and
      48 + 9 tables of records read by index. The C reads the program's data by these names, never by
      its address. The original's quirks found along the way are noted next to the code they are in
- [x] Readable C: no `goto`, a comment above every function (1,783), and what is left of the CPU counted
      by `tools/noise_report.py` and explained under "What is left, and why" below
- [x] The game's source: the decompiled C is in `src/`, and the game builds from it
- [x] Code coverage: the static analysis follows the game's object tables described in `symbols.txt`,
      so the C comes from the ROM alone, and it finds all the code the reference runs. The 98 scenarios
      of `tests/controls/` (attract mode, games for one, two and three players, every stage, the ending,
      the test mode, and pokes that reach what the game itself never does) run 43,042 of its 47,575
      instructions: 3,908 of the others are the code nothing in the program calls (`symbols.txt` says
      what each piece is), and `docs/unreachable.txt` says, for each of the other 625, why no scenario
      runs it (dead code, a hardware fault the board model cannot produce, or a rare situation)
- [x] System 32 video library (`runtime/src/video.c`): the four tilemaps (zoom, row scroll and row
      select, clip windows), the text and bitmap layers, the backdrop, the two sprite frame buffers with
      their list processor, and the mixer (priorities, blending, shadows, color offsets). It draws each
      frame from the board's memories into a 320 or 416 x 224 picture, and matches MAME's System 32
      video pixel for pixel in every frame of the scenarios (`check_scenarios.py --video`)
- [x] Sound board model (`runtime/src/sound.c`): the Z80's memory and ports, its interrupt controller,
      the ROM bank, the shared RAM; a reference run of the sound program on MAME's Z80 with ymfm's YM3438
      and MAME's RF5C68 (`s32ref --sound-wav`), and a Z80 disassembler identical to MAME's
- [x] Sound: the Z80 sound driver decompiled into C (`src/sound/`, written by `tools/decompile_sound.py`
      from the sound ROM and `sound_symbols.txt`), running on the board's time, and the YM3438 and RF5C68
      as libraries (`runtime/src/ym3438.c`, `runtime/src/rf5c68.c`) that make the same samples as ymfm and
      MAME. It matches the reference's samples and sound board memories frame by frame in every scenario
      (`check_scenarios.py --sound`). It reads like the game's C: its functions, parameters, local
      variables, variables, structures (the sequence players, the FM and PCM voices, the MIDI parts) and
      the chips' ports are named in `sound_symbols.txt`, with the evidence for each name, and the bytes the
      Z80 keeps apart in a register pair (a count in B, a register number in C) are variables of their own
- [x] PC port (`ports/sdl2/`): the game in a window, with its sound, the mouse, the keyboard and game
      controllers, on SDL2; its settings in `sonic.ini`. Built and played on Windows; SDL2's own packages
      should build it on Linux and macOS (not tried yet)
- [ ] Ports, in a repository of their own built on this one (`segasonic-port`), so that this one stays the
      faithful reference: Windows and macOS, Android, Nintendo Switch and Linux ARM handhelds (Anbernic);
      released as downloads with no game data in them (the first run takes the player's own ROM set and
      extracts the assets), and listed in [Quiver Launcher](https://quiverlauncher.com)
- [ ] A console version, there too, in a layer of its own beside the arcade one, which stays as it is: no
      coins, a settings menu in the game's own style, higher resolution

## Getting started

Requirements: Python 3.10 or newer, CMake 3.20 or newer, a C11 / C++17 compiler (Visual Studio Build
Tools on Windows) and, for the game's window, the SDL2 development files: on Windows, unpack
`SDL2-devel-2.x.x-VC.zip` from [SDL's releases](https://github.com/libsdl-org/SDL/releases) into
`build/downloads/sdl2/` (or point `SDL2_DIR` at it); elsewhere, the system's package (`libsdl2-dev`).
Without SDL2 only the headless builds are made.

The decompiler, the analysis tools and the asset formats are a project of their own,
[arcade-decomp](https://github.com/cheviman/arcade-decomp), which this repository keeps in `tools/core/` as
a git submodule: clone it with the submodule (`git submodule update --init` in a clone made without it).
The game builds without it; extracting the assets and the decompiler need it:

```bash
git clone --recursive https://github.com/cheviman/segasonic-decomp
```

1. Put your MAME `sonic.zip`, or the files inside it, in `rom/`, and check it:

   ```bash
   python tools/verify_rom.py
   ```

2. Extract the assets into `assets/` (the tile sheets, the sprites, the program's data, the sound
   driver's data, the music and the samples), and check that they build back into your ROM byte for byte:

   ```bash
   python extract_assets.py
   ```

   ```bash
   python extract_assets.py --check
   ```

3. Build the game and the development tools into `build/bin/`. No executable carries the ROM or anything
   from it: the build takes the assets out of your ROM set when `assets/` does not hold them yet, and
   packs them into `build/bin/segasonic.assets` (`tools/make_pack.py`), which the game loads when it
   starts (`runtime/src/game_data.c`). The original code is in no file: the C replaces it. Change an
   asset and build again, and the change is in the game. `python tools/make_pack.py --check` compares
   what the pack puts in the board's memory with your ROM set, byte for byte, the code left out. The
   reference (`s32ref`), which runs the original code as MAME does, reads the ROM set from `rom/` when it
   starts:

   ```bash
   cmake -S . -B build/cmake
   ```

   ```bash
   cmake --build build/cmake --config Release
   ```

4. Play:

   ```bash
   build/bin/sonic
   ```

   The executable can be copied anywhere with SDL2's library beside it (`SDL2.dll` on Windows, which the
   build puts next to it) and the assets pack (`segasonic.assets`), which the game also finds in the folder
   where it keeps what it writes (below). With no pack, the first run makes it there from your ROM set:
   `sonic.zip` beside the game (its `.app` on macOS), or the one you drop on the window it opens (the
   set's 18 files, each checked by its CRC32); from then on the game loads the pack and never opens the
   ROM set again. Built from `rom/`, `build/bin/` has the pack already. The executable holds nothing of
   the game's; the pack is made from your ROM: it is yours, not one to share. On Linux and macOS the build
   needs the system's SDL2 (`libsdl2-dev`, Homebrew's `sdl2`); with no ROM set it makes the executables
   alone.

   Or run the game without a window, here with the controls of a one-player game:

   ```bash
   build/bin/sonic_game_headless --script tests/controls/coin_start.txt
   ```

## Playing

It is the arcade game: put a coin in (5), then start (1). Player 1's controls:

| Control | Keys |
|---|---|
| Trackball | the mouse, or the arrow keys |
| Button | left mouse button, Z or space |
| Coins 1-3, starts 1-3 | 5, 6, 7 and 1, 2, 3 |
| Service, test | 9 and F2 (in the operator's menu, 9 moves the cursor and F2 picks) |
| Full screen | F11 or Alt+Enter |
| Smooth or sharp pixels | F3 |
| The mouse's ball rolls on, or stops with the mouse | F4 |
| The mouse slower, faster | F5, F6 |
| Quit | Escape |

The mouse turns a trackball as the arcade's does: the ball turns with the mouse and, when the mouse stops
or is lifted, rolls on and slows down; the right button holds it. Each game controller is a player: the
left stick is the trackball, A the button, Start the start and Back a coin.

The game keeps what it writes beside the executable on Windows, and elsewhere in the user's folder for it
(`~/.local/share/SegaSonic` on Linux, `~/Library/Application Support/SegaSonic` on macOS). The window's
settings are in `sonic.ini` there, written with every setting and what it
does the first time the game runs: the mouse's speed and how the ball rolls on, the keys' and the
stick's speed, the window's scale, full screen, smooth pixels and the volume. The game reads the file again
whenever it is saved while the game runs, and the keys above write what they change into it. The game's own
settings and bookkeeping (the EEPROM) are kept in `sonic.eeprom`, there too.

### Changing the decompilation

`src/` is written by the decompiler from your ROM and `symbols.txt`. The decompiler is arcade-decomp's,
in `tools/core/`: what is SegaSonic's (its structures, the registers its program shares, its sound chip)
is in `tools/game_profile.py`, and a change to the decompiler itself goes to arcade-decomp, checked on
both games, and comes here as a newer commit of the submodule
(`git submodule update --remote tools/core`). After changing your ROM, `symbols.txt` or the decompiler,
write the C again and check it:

```bash
python tools/decompile.py
```

`src/sound/` is written the same way from the sound ROM and `sound_symbols.txt`:

```bash
python tools/decompile_sound.py
```

`build/bin/s32ref` runs the original program on MAME's V60 core against the same board model. Both
it and the game take the control scripts of `tests/controls/` (format in
`runtime/include/s32/script.h`), and both write a hash of the board state after every frame. This runs
every script on both, compares them frame by frame and checks that the static analysis alone found all
the code the reference executed; `--video` and `--sound` compare every frame's picture and sound too,
and `--jobs N` runs N scripts at a time:

```bash
python tools/check_scenarios.py
```

`python tools/decompile.py --check` reports what the analysis found (functions, parameters and results,
the registers code reached through memory slots or interrupts would hand back), and `--trace` makes
every function report itself when it runs (`--out DIR` writes such a copy elsewhere; CMake builds it
with `-DSONIC_DECOMPILED_DIR=DIR`). `build/decompile/index.txt` lists every function with where its C is,
its callers and callees, what it mentions, and how much of it the scenarios ran. `--variables FILE` lists
every local variable with the instructions that write it, which is what `symbols.txt`'s `local` lines
name them by, and `--split-webs` writes the C with every value of a register as a variable of its own,
to name them.

Other tools:

- `tools/disassemble.py` writes the listing, `build/listing/sonic.s`, with a report beside it.
- `tools/recompile.py` writes the translation scaffold the decompiler grew from (one C statement per
  instruction) into `build/generated/`; CMake builds it with `-DSEGASONIC_SCAFFOLD=ON`, and
  `check_scenarios.py --translated` checks it.
- `tools/coverage.py` classifies every byte of the program ROM and lists what is still unexplained.
- `tools/check_structure.py OUT_DIR [--sound]` decompiles into `OUT_DIR` and checks that the C's `if`,
  loops, `switch` and control variables (`tools/core/decompiler/structure.py` and `gotos.py`) do what the
  program's jumps do: it runs every function's blocks and its C on the same made-up outcomes of its tests
  and compares the blocks each runs, the code no scenario reaches too.
- `tools/noise_report.py [--save]` counts what is left of the CPU and of the decompiler in the C (gotos,
  names after a register, fields without a name, memory reached through computed addresses, results
  written back again before an early return, jumps out of a table, ROM addresses as numbers, functions
  without a comment) and compares
  it with `tools/noise_baseline.txt`: a count that goes up makes it fail.
- `tools/memory_census.py` runs every scenario on a build of the game made with
  `-DSONIC_MEMORY_CENSUS=ON` and says, for each line of `src/` that reads or writes memory through an
  address the code computes, which of the board's memories it reached (the ROM, the work RAM, video RAM,
  the sound board's RAM...): what typing the remaining pointers starts from.
- `tools/make_walks.py` writes the per-stage scripts.
- `tools/reference/compare_with_mame.py` checks our disassembler against MAME's.
- `s32ref --watch ADDRESS` prints a byte whenever it changes.
- `s32ref --dump-frame N --dump-dir DIR` saves the board state at a frame, and
  `tools/render_state.py DIR` draws its text layer and tilemap pages.
- `s32ref --video-hashes FILE` and `--video-dump N --video-dir DIR` draw every frame with MAME's System
  32 video (the hash of each frame, frames as BMP); `sonic_game_headless` takes the same options and draws
  them with the game's own video. `check_scenarios.py --video` compares the two in every scenario.
- `s32ref --sound-hashes FILE`, `--sound-log FILE` and `--sound-wav FILE` run the sound board too, on
  MAME's Z80 and RF5C68 and ymfm's YM3438: per frame, a hash of the samples and of the sound board's
  memories; every write to the sound chips; everything mixed into a WAV file.
- `tools/core/z80/disassembler.py` decodes the sound program; `tools/reference/compare_z80_with_mame.py`
  checks it against MAME's Z80 disassembler at every byte.

## Layout

```
rom/                    your ROM (ignored by git)
assets.json             asset manifest: positions and formats, no data
extract_assets.py       extracts assets/ from your ROM, and checks they build back into it (--check)
sprite_palettes.json    where in the program ROM each sprite frame's palette is (positions only), and
                        which frames the game draws opaque
tile_palettes.json      the same for each tile
sprite_sheets.txt       which frames make each sprite sheet, with the sheets' and their rows' names
data_formats.json       the files of the program's data by subject, and each piece's format
                        (tools/formats/)
sound_formats.json      the same for the sound program's data
src/                    the game's C, decompiled from the program (tools/decompile.py writes it), in
                        files by subject: system/, video/, engine/, stage/, player/, objects/, scenes/
src/sound/              the sound driver's C, decompiled from the sound program
                        (tools/decompile_sound.py writes it)
symbols.txt             names of functions, data, variables, board registers, structures and fields,
                        parameters and local variables, what pointers point at, and the layout of the
                        tables that hold code addresses (names only, no data); its comments explain how
                        each part of the game works, and its "comment" lines are what the C says above
                        each function
sound_symbols.txt       the same for the sound program
runtime/                System 32 board model (memory map, interrupts, I/O, the sound board), the video
                        library, the YM3438 and RF5C68 libraries, and what the decompiled game and sound
                        driver use besides memory (src/game.c, src/sound_cpu.c, include/game/support.h,
                        include/sound/support.h), in C; part of the game
ports/headless/         runs the game with no screen or sound, for comparisons
ports/sdl2/             the game in a window, on SDL2: picture, sound, controls, settings (sonic.ini);
                        first_run.c, the window that asks for the ROM set when there is no assets pack
                        (its letters: font/, Kenney Mini, CC0, drawn by tools/make_font.py)
tests/controls/         control scripts: coins, buttons, trackball moves and pokes per frame
docs/unreachable.txt    the code no scenario runs, and why
tools/sonic_rom.py      ROM set definition and memory region layouts
tools/make_pack.py      packs the assets for the game to load (segasonic.assets): each by name, where
                        the board's memory has it; --check compares it with the ROM set
runtime/src/game_data.c loads the assets pack into the board's memory, and the chips' mirrors
runtime/src/extract.c   the first run: the player's ROM set (rom_set.c: a .zip, inflate.c, or a folder,
                        each file by its CRC32) into the assets pack, the pieces pack_layout.c lists
                        (make_pack.py --layout writes it: positions only)
tools/bisect_code.py    finds the bytes of the code a scenario still reads as data (assets.json's
                        read_as_data takes them), against the reference at a frame
tools/core/             arcade-decomp, the decompiler and analysis tools shared with outrunners-decomp (a
                        git submodule; tools/game_profile.py tells it about SegaSonic, and tools/'s scripts
                        of the same names run its): v60/ (NEC V60 decoder and C translation of each
                        instruction), z80/ (Zilog Z80 decoder, analysis, lifting and timing: the sound
                        program), disassemble.py (recursive disassembler, listing and analysis report),
                        coverage.py (byte map of the program ROM), recompile.py (splits the program into
                        functions and translates them to C: the scaffold), decompile.py (decompiles the
                        program into readable C; decompiler/: lifting to IR, stack, data flow and
                        signatures, conditions, expressions, structure and the control variables that
                        take the gotos out, memory and pointer types, variables, the C printer),
                        decompile_sound.py (the sound program, with the same decompiler set up for the
                        Z80), check_structure.py, noise_report.py, asset_formats.py (the board's data
                        formats to PNG and WAV and back) and data_formats.py (the data as files and back,
                        cut where symbols.txt and sound_symbols.txt name a place, the code left out)
tools/game_profile.py   what the core needs to know of SegaSonic: its ROM set (sonic_rom.py), its
                        structures, the registers the program shares, its board's sound chip
tools/formats/          the data formats, a module a family (palettes, scripts, animations, stages, texts
                        and the rest; the sound driver's tables and the music, as MIDI files)
tools/palette_census.py runs the scenarios with the sprite and tile censuses on (sonic_game_headless
                        --sprite-census, --tile-census) and finds each frame's and tile's palette in the
                        program ROM: writes sprite_palettes.json and tile_palettes.json
tools/check_scenarios.py  runs the control scripts on the reference and on the decompiled game
tools/check_structure.py  checks that the C's control flow runs what the program's jumps run
tools/noise_report.py   counts the leftovers in the C against tools/noise_baseline.txt
tools/reference/        MAME's V60 and Z80 cores and disassemblers, its System 32 video and RF5C68, and
                        ymfm's YM3438, built only into the reference tools (s32ref, the oracles), over
                        stand-ins for the parts of MAME they need
assets/, build/         generated on your machine (ignored by git)
```

## The assets

`python extract_assets.py` writes them from your ROM into `assets/`, by subject:

```
sprites/   the sprite frames in sheets, a PNG and a JSON each (where each frame is in the sheet and goes
           back in the sprite ROM): sonic/, ray/, mighty/ (the players' animations: walking, running,
           tumble, climb...), stage_03/ ... stage_10/, stages_0b_0f/ (each stage's objects), objects/,
           effects/, hud/, text/, eggman/, intro/, title/, map/, character_select/, ranking/,
           ring_tally/, ending/
tiles/     the tile sheets, in color, with each tile's palette
data/      the program's data as text, a file a subject, a section a piece: system/ (the main program,
           the attract demos, the test mode), video/ (fonts, palettes, the frame records), engine/,
           objects/, player/ (sonic, ray, mighty and the players' own), stages/ (each stage: its spawn
           lists, floor blocks, restart points, palettes, tilemaps and its objects' tables), scenes/
sound/     data/: the driver's tables (chips, tuning, instruments, commands) and music/, effects/, voices/,
           a MIDI file each; samples/: the PCM samples, a WAV each
```

Edit them and build again: the game takes them. A piece of data keeps its size (the code reaches it at a
fixed address), but the songs may change length, and a sprite frame is cut from its sheet by its
rectangle. Every file says what it holds; `layout.json` and the sheets' JSON say where each piece goes.

## Reading the C

The game's C lives in `src/`, in files by subject:

```
game.h  ram.h  table.c   the prototypes (file by file); the variables, tables, board registers and
                         structures; the code the program reaches through memory slots, by address
system/   main.c         power-on, the frame loop, the attract mode, the controls, the interrupts
          coins.c        coins, credits and the prompts outside a game
          settings.c     the EEPROM settings and the bookkeeping, with their checksums
          sound.c        the sound the game asks the Z80 for
          test_mode.c    the test mode
video/    video.c        the video registers, palette RAM, the VRAM scripts that build the tilemaps
          text.c         fonts and text, digits, strings drawn with sprites
          bitmap.c       the bitmap layer
          shapes.c       lines, circles, polygons and fills on the bitmap layer
engine/   objects.c      the object list and pools, spawning from the stage lists, what objects share
          contacts.c     contacts between objects and players
          geometry.c     between the map and the screen; square roots, angles, BCD
          sprites.c      animation and the sprite list
stage/    game_modes.c   the game modes and what a stage frame runs
          camera.c       the stage view, the scroll, the camera, the fades
          layers.c       the animated palettes and the tilemap layers
          events.c       the scripted game modes
player/   player.c       the player's update, movement, checks and contacts
          states.c       the states in play
          scene_states.c the states in scenes
          carried_states.c  the states while the stage carries or holds a player
          effects.c      the HUD and the effects around the players
objects/  stage_03.c ... stage_10.c, stages_0b_0f.c   each stage's objects
          intro.c        the opening and the how to play
          eggman.c       Eggman's booth, speeches and scene
          rings.c        rings
          shared.c       the objects several stages use: signs, scenery, walls, the chasers' zones
scenes/   title.c  map.c  character_select.c  ranking.c  ring_tally.c  ending.c
sound/    sound.h  main.c  midi.c  voices.c  sequences.c   the Z80 sound driver
```

Each file's header says what it holds. The original's linker laid the program out in modules, one after
another; `symbols.txt`'s `file` lines give each stretch of it to its file (a stretch ends where a module
of the original ends: its tables after the code, or a byte that aligns the next routine), so a file takes
the stretches of its subject, such as a stage's objects from both code chips, and its functions are in the
order of their addresses. `sound_symbols.txt` does the same for `src/sound/`. What the C uses besides
memory is in `runtime/include/game/support.h`.

- **Memory.** The game's variables are lvalues over the work RAM (`stage`, `scroll_position`), its tables
  read-only arrays over the program ROM (`stage_camera_origins_x[stage]`), and its two structures,
  `struct object` and `struct player`, are reached with `OBJECT(address)` and `PLAYER(address)`: the game
  keeps 24-bit addresses in its variables, as the original does. Each object type's own variables are a
  view of the object (`OBJECT(current_object)->tornado.path`) where the code shows which type the object
  is: its own object, one it gives a handler (`OBJECT(object)->saw_blade.state` after
  `OBJECT(object)->handler = saw_blade_update_ADDRESS`), or one a link leads to that `symbols.txt` types
  with the family (a button box's `parent` is Eggman's scene object:
  `OBJECT(LINKED(current_object, OBJECT(current_object)->parent))->eggman_scene.phase`); elsewhere those
  bytes keep their generic names (`var_80`). The records the code walks through pointers are structures
  too: in the work RAM, the ranking records, the palette cycles, the sprite part lists; in the program
  ROM, read only, every record the code reads through a pointer, from the stage's floor blocks
  (`FLOOR_BLOCK(block)->slope`) and spawn lists to the chasers' paths, the ending's timed frames and the
  sound driver's register pairs. A record by its number from a pointer is `STRUCT(p)[i].field` (the one
  after the pointer's, `[1]`; the one before, `[-1]`), and a table of records at a fixed place is read by
  its name (`hud_gauge_positions_3p[kind].y`), and one a byte offset the game keeps reaches, from where the
  offset counts (a floor type's attribute record,
  `FLOOR_ATTRIBUTE_RECORD(floor_types_ADDRESS + OBJECT(o)->floor_attribute)->height`). A field can be an array, in the record
  (`shocked_palettes[kind].palettes[frame]`, `crawler_walk_steps[variant].steps[step]`) or after it, as an
  animation's frames follow its last frame's number
  (`FRAME_ANIMATION(OBJECT(o)->animation)->frames[frame]`). Values a pointer walks one at a time, a text's
  characters, a list of words, a script's bytes, are arrays of their kind
  (`SPRITE_FRAME_NUMBERS(frame_list)[frame]`, `VRAM_SCRIPT(script)[0]`). What the code reads or writes
  through any other address it computes goes
  through the board's bus: `mem_read16(p)`, `mem_write8(io_cnt, x)` (the board registers are addresses):
  the routines that fill, copy or draw into the board's memories wherever they are told, the memory tests.
  A part of a variable or field with no name of its own
  reads as a cast (`(uint8_t)(OBJECT(o)->flags >> 8u)`) and is written keeping the rest
  (`x = (x & 0xFF00u) | ...`); where the code reads or writes several fields at once (two words copied as
  one long, a halfword from a field's second byte on), the C reads those fields put together
  (`OBJECT(o)->sprite_flags | (uint32_t)OBJECT(o)->sprite_frame << 16u`) and writes each of them its bytes.
  A number the code divides by, counts or compares with has a name where symbols.txt gives it one
  (`divx_pair(FRAMES_PER_HOUR, ...)`, `initials == INITIALS_SEX`). Where the code starts walking a list from an
  address that is no structure, the C says what that address is: `hitter_list_ADDRESS - 0x68u`, where a
  structure would have the list's head as its link field (`hit_list_next`, at 0x68).
- **Links.** The game links its objects by the low half of their addresses: `LINKED(current_object,
  OBJECT(current_object)->child)` is the object `child` names, in the work RAM page.
- **Registers.** `current_object`, `object_cursor`, `current_player`... are the registers the whole program
  keeps (R20, R23, R18): global variables. Everything else a function keeps in registers is a local
  variable or a parameter. What a call hands back and nothing reads goes to a variable of the function kept
  for that, one for each register (`ignored`, `ignored_2`...): the call needs somewhere to write it, and
  never gets one pointer twice (`animate(&ignored, &ignored_2, &at_last_step)` when only the animation's
  last step matters). A value one routine leaves in a register
  and another one, several calls further on, reads has the name of what it holds where it comes from
  (`run_sega_logo` relies on `init_sprites` leaving 0xFFFF in R0, its `end_mark`): the C passes those values
  along explicitly so that it does what the original does; a register the original reads before anything
  sets it starts at 0. `r14` is the one register still called by its name: a global the game uses as
  scratch, which object handlers read where an earlier one left it.
- **Results.** A function hands back one value as its return value, and more through pointers
  (`uint32_t *p_depth`); a caller's variable goes in and comes out. A result the function only passes on
  goes straight to the calls (`check_player_step_against_floor(p_height_diff, p_cursor);`), and one it
  copies back from the same variable at every `return`, and uses little else, is read and set through its
  pointer (`*p_dest += 2u;`). Otherwise, or when some call hands it the same variable as another result,
  it has a variable of its own, written back through the pointer once, at the function's end,
  where its ways back meet; a way out from inside a loop, a `switch` or a branch that cannot reach the end
  without a control variable writes it back before its own `return`.
- **Comments.** Every function has a comment above it that says what it does in the game's terms and the
  C's names (the `comment` lines of `symbols.txt` and `sound_symbols.txt`); code nothing calls starts with
  "Unused:". The rest of `symbols.txt`'s comments keep the evidence: registers, addresses, the listing.
- **Control flow.** Loops, `if`, `switch` (a jump through a table of code is a `switch` on the index) and
  `return` come from the original's branches; there is no `goto`. Code several places jump to is a
  function of its own: each game mode (`run_game_mode` jumps by `game_mode`, and a mode that changes it
  runs the new one at once: `run_game_mode(...); return;`), each player state, the code two states share
  (`player_hurt_fly`). Where the original's jumps cross in a way blocks cannot (a loop entered at two
  places, a way out of two loops at once, two branches whose code joins halfway), a `bool` named after
  the place the jump goes to carries it: `within_box_x = ...; if (!within_box_x) { ...; within_box_x =
  ...; } if (within_box_x) { ... }` in `check_hit`, which is how C writes a test with statements between
  its halves. A loop the code enters at two places gets a copy of the piece from one way in to the other,
  before it (the line drawing's odd and even halves), and a way out of a loop that runs into another one
  is written where it leaves, as if it were inside the loop. Three functions of the game have control
  variables (`check_hit`'s and `fill_span_list`'s tests, the sprite list's multi-part tail) and one of the
  sound driver (`step_fm_vibrato`'s test). `default:
  invalid_jump(address)` is where a jump table's index would leave the table: the original checks
  nothing and would jump into whatever follows it; the C stops there.
- **Interrupts and the frame.** `frame_wait()` is where the game waits for the next frame and the board
  runs its events, and `take_interrupts()` where the interrupts that raised are handled; the interrupt
  handlers are ordinary functions. `halt()` (only in `int_default` and `int_vector_02`) does nothing, as in
  MAME.
- **Unused code.** The functions nothing in the program reaches are in the C beside the rest, in their
  place by address; their entries in `symbols.txt` (at the end) start with "unused:" and say what each one
  would do. With no caller to say what they take and hand back, they take every register they read before
  writing it, and hand back what `symbols.txt` says (`result` lines). The 00 bytes that align the routine
  after them are data, not code. A few of them use addresses of their own stack frame as values: there the
  V60's stack pointer is a global variable, `stack_pointer`, and the stack stays in work RAM, as on the
  board. An instruction the V60 would refuse (a bit operand in a register) is `invalid_operand(address)`,
  which stops the game, as MAME does.
- **The sound driver.** `src/sound/` is the Z80 program the same way. The sound chips are read and
  written through `port_in`/`port_out` by the ports' names; its variables are names over the sound RAM
  (`sound.h`), its structures are reached with `FM_VOICE(voice)->`, `SAMPLE_VOICE(voice)->`,
  `MIDI_PART(part)->`, the records of its ROM with `INSTRUMENT(instrument)->`, `SONG_RECORD(song)->` and
  the like, and what it reads through any other address it
  computes goes through the board's bus (`z80_read8`, `z80_write8`: the bank window, whose contents
  depend on the bank register, and the RF5C68); `z80_block(cycles)` opens each
  stretch of code with the time it takes, so the chips' timers, the interrupt and the samples happen when
  they did on the board. Its unused routines are in the C too, as the game's are (the last section of
  `sound_symbols.txt`): among them a driver for a MIDI port, an 8251 USART at ports F0 and F1 that the
  System 32 board does not have.

### What is left, and why

`tools/noise_report.py` counts these in `src/` and `src/sound/` against `tools/noise_baseline.txt`; none
of them can go without the C doing something the original does not:

| What | Count | Why it stays |
|---|---:|---|
| `goto`, functions or memory named after an address (`sub_`, `unk_`), functions without a comment, data read through its ROM address (`ROM16(...)`: 1,137 of them before the program's data had names), numbers in the ROM's range left as numbers, parameters and locals named after a register (98 before this round), several fields read or written at once (`AS16`, `AT8`: 67), values the decompiler works out once named by a number (`t1`, `stored1`: 7) | 0 | |
| Results written back again before an early return (`*p_x = x; return;`) | 274 | a function writes its results back once, at its end, where its ways back meet (1,500 copies at every `return` before); these are the ways out of a loop, a `switch` case or a branch that could only reach the end through a control variable |
| Memory through a computed address (`mem_read16`, `mem_write32`) | 1199 | the board's memories and registers (video RAM, palette RAM, the I/O ports) and the generic routines that fill, copy and draw wherever they are told |
| Partial writes of a variable (`(x & ~0xFFFFu) \| y`) | 720 | the original writes the low byte or halfword of a register and keeps the rest |
| `default: invalid_jump(...)` | 243 | the original never checks a jump table's index; the C stops where it would leave the table |
| Object fields without a name (`var_80`) | 14 | all in `init_object_fields_tail`, which clears a new object: every object type uses those bytes (0x7F-0x9F) its own way, so where the engine clears them for any type they have no one name |
| Functions with `bool` control variables | 4 | a test with statements between its halves, as C writes it (`check_hit`, `fill_span_list`, `step_fm_vibrato`), and the sprite list's multi-part tail, which only a 60-line copy would take away (see Control flow) |
| Functions the board puts its own in place of (`runtime/replaced.txt`, `BOARD_ORIGINAL`) | 1 | `check_rom_checksums`, the MEMORY TEST's ROM part: it measures ROM chips, and the decompiled game has none (it finds no error, as a board with good chips does) |
| Bytes of the code the game also reads as data (`assets.json`, `read_as_data`) | 999 | the original reads them through a null pointer or past a table's end: a structure or a text at a null pointer (the program's first 512 bytes, and the frames of its characters past `text_frames`), the map trail and marker of a stage with none, a rope bridge's sag. Each piece takes all a pointer or an index can reach there; the assets hold them, and no other byte of the code |

## How the port works

- **Game code.** The V60 machine code is decompiled to C (`src/`), so the game runs natively with no CPU
  emulation. The game is that C and the assets: the build packs them (`tools/make_pack.py`) and the game
  loads the pack when it starts (`runtime/src/game_data.c`), each asset where the board's memory has it,
  so the program reads its data tables and level data, and the video and sound chips their graphics and
  samples, where the original reads them from its ROM chips. The original code is in no file and no
  executable, except the few bytes of it the game also reads as data (`assets.json`, `read_as_data`).
  Only the reference (`s32ref`), which runs the original code as MAME does, reads your ROM set, when it
  starts.
- **Video.** The System 32 video hardware is a library (`runtime/src/video.c`) that draws every frame
  from the board's memories, as the board does, into a 320 or 416 x 224 picture; a platform shows that
  picture as a texture, scaled and filtered as it likes.
- **Sound.** The Z80 sound driver went through the same decompilation (`src/sound/`); the YM3438 and
  RF5C68 chips are libraries that make the samples a platform plays.
- **Platforms.** Each port only provides a window, input, audio output and the presentation of the
  picture, through the board's hooks (`runtime/include/s32/machine.h`); `ports/sdl2/` is the first.

## Legal

- This project is licensed under the BSD 3-Clause License; see `LICENSE`.
- The C in `src/` is the game's program decompiled into a new form: logic only, with names and
  comments written for this project. No ROM contents (graphics, sound, data tables, level data) and no
  extracted assets are distributed by this project; they come from your own ROM. The screenshots in
  `docs/screenshots/` show the game running from this code.
- `runtime/`, `tools/core/v60/disassembler.py` and `tools/core/z80/disassembler.py` follow the behavior of MAME
  source files and, for the YM3438, of ymfm, and `tools/reference/mame/` holds copies of MAME files. Those
  are BSD-3-Clause; see `tools/reference/mame/LICENSE`. `tools/reference/ymfm/` holds a copy of ymfm
  (BSD-3-Clause; see its `LICENSE`).
- SegaSonic The Hedgehog is a trademark of SEGA. This project is not affiliated with or endorsed by SEGA.
