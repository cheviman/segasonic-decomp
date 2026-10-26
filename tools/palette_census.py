"""
Name:        palette_census.py
Description: finds where in the program ROM the palette of each sprite frame and each tile is, and which
             frames are drawn opaque, for extract_assets.py
Version:     0.1.0
Created:     14-10-2026
Modified:    19-10-2026
Author:      Sebastian Toyos

Neither the frame records nor the tiles say anything of colors: the object that draws a frame, or the
tilemap cell that shows a tile, picks the palette. So the game is run through the scenarios of
tests/controls with the censuses on (sonic_game_headless --sprite-census: every sprite the list draws;
--tile-census: every tile the tilemaps show, every 30 frames; both with the color each pen shows), and each
set of colors is looked for in the program ROM, where the game keeps its palettes (16-bit entries in the
palette window's alternate format, the V60's byte order). What it writes, sprite_palettes.json and
tile_palettes.json, holds positions only: where each frame's or tile's palette starts in the program ROM;
extract_assets.py reads the colors from the user's ROM. A frame or tile no scenario shows takes the palette
of the nearest one before it (the frames of a character, the tiles of a stage, lie together).

The sprite census also says whether each sprite was drawn opaque (sprite entry bit 8: the end code, all ones
at a word's edges, is drawn as a color, and the row goes on past it): sprite_palettes.json lists those
frames, so that the sheets show what follows an end code where the board draws it (the players, among
others). A frame no scenario draws is drawn the way of the nearest one before it.

Usage:
  py tools/palette_census.py run DIR       the censuses of every scenario into DIR (two files each)
  py tools/palette_census.py resolve DIR   sprite_palettes.json and tile_palettes.json from them
"""
import json
import struct
import subprocess
import sys
from collections import Counter
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
sys.path.insert(0, str(ROOT / "tools" / "core"))
import asset_formats  # noqa: E402
import sonic_rom  # noqa: E402

FRAME_RECORDS = 0x130000          # sprite_frame_records (symbols.txt)
FRAME_COUNT = 0xBFD0 // asset_formats.SPRITE_RECORD
TILE_COUNT = 0x4000               # 16x16 tiles in the tile ROM
JOBS = 8


def run(out_dir):
    out_dir.mkdir(parents=True, exist_ok=True)
    runner = ROOT / "build" / "bin" / ("sonic_game_headless.exe" if sys.platform == "win32" else "sonic_game_headless")
    scripts = sorted((ROOT / "tests" / "controls").glob("*.txt"))

    def one(script):
        result = subprocess.run([str(runner), "--script", str(script),
                                 "--sprite-census", str(out_dir / f"sprites_{script.stem}.txt"),
                                 "--tile-census", str(out_dir / f"tiles_{script.stem}.txt")],
                                capture_output=True, text=True)
        return script.stem, result.returncode
    with ThreadPoolExecutor(JOBS) as pool:
        for name, code in pool.map(one, scripts):
            print(f"{name:<40} {'done' if code == 0 else 'failed'}", flush=True)


def read_census(paths, first):
    """{what was drawn (a sprite's graphics word, a tile): Counter of color tuples}; 'first' is the field the
    colors start at (5 in the sprite census, 3 in the tile census)"""
    seen = {}
    for path in paths:
        for line in path.read_text(encoding="ascii").splitlines():
            parts = line.split()
            colors = tuple(int(part, 16) for part in parts[first:])
            seen.setdefault(int(parts[0], 16), Counter())[colors] += 1
    return seen


def read_opaque(paths):
    """{a sprite's graphics word: whether it was drawn opaque} from the sprite census. A sprite drawn both
    ways counts as opaque: what follows its end codes is part of its picture."""
    opaque = {}
    for path in paths:
        for line in path.read_text(encoding="ascii").splitlines():
            parts = line.split()
            word = int(parts[0], 16)
            opaque[word] = opaque.get(word, False) or parts[4] == "1"
    return opaque


def resolve_opaque(items, opaque):
    """The items drawn opaque, and the items no scenario draws that the nearest item before them makes
    opaque. 'items' are in ROM order."""
    drawn, nearby = [], []
    last = False
    for item in items:
        if item in opaque:
            last = opaque[item]
            if last:
                drawn.append(item)
        elif last:
            nearby.append(item)
    return drawn, nearby


def alternate_format(color):
    """A palette entry as the game writes it through the upper half of the palette window, and keeps it in
    the ROM (runtime/src/machine.c): the low bit of blue, green and red in bits 14-12, the rest 4:4:4."""
    red, green, blue = color & 0x1F, (color >> 5) & 0x1F, (color >> 10) & 0x1F
    return (color & 0x8000) | (blue & 1) << 14 | (green & 1) << 13 | (red & 1) << 12 \
        | (blue & 0x1E) << 7 | (green & 0x1E) << 3 | (red & 0x1E) >> 1


def find_palette(program, colors, index):
    """Where 'colors' (RGB555, as palette RAM holds them) start in the program ROM, or None: the whole set,
    else all but the last pen (the transparent one, whatever color it shows); in the format the game keeps
    them in, else as they are. 'index' maps a 4-byte prefix to its offsets."""
    for convert in (alternate_format, lambda color: color):
        for count in (len(colors), len(colors) - 1):
            wanted = struct.pack(f"<{count}H", *(convert(color) for color in colors[:count]))
            for offset in index.get(wanted[:4], ()):
                if program[offset:offset + len(wanted)] == wanted:
                    return offset
    return None


def resolve_all(items, seen, program, index):
    """{item: palette offset} for the items the census saw with a palette the ROM has, and {item: offset}
    for the rest, from the nearest item before them that has one. 'items' are in ROM order."""
    found = {}
    for item in items:
        for colors, _ in seen.get(item, Counter()).most_common():
            offset = find_palette(program, colors, index)
            if offset is not None:
                found[item] = offset
                break
    guessed = {}
    last = None
    for item in items:
        if item in found:
            last = found[item]
        elif last is not None:
            guessed[item] = last
    return found, guessed


def write_palettes(path, what, found, guessed, digits, opaque=None):
    """The census's palettes into 'path'; the 'chosen' ones the file already has (a palette picked by hand
    where the census's is wrong: a frame shown during a flash, a neighbour's palette that does not fit) are
    kept. 'opaque', for the sprite frames: the frames drawn opaque and those taken to be (two lists)."""
    chosen = json.loads(path.read_text(encoding="ascii")).get("chosen", {}) if path.exists() else {}
    notes = [f"Positions only: where in the program ROM the palette of each {what} starts; "
             "tools/palette_census.py writes it.",
             f"'drawn': the palette the {what} was shown with in the scenarios; 'nearby': one no scenario "
             f"shows, given the palette of the {what} before it; 'chosen': picked by hand where those are "
             "wrong (a frame caught during a flash, a neighbour's palette that is not its own), and used "
             "instead of them."]
    content = {"notes": notes,
               "drawn": {f"{item:0{digits}X}": f"0x{offset:06X}" for item, offset in sorted(found.items())},
               "nearby": {f"{item:0{digits}X}": f"0x{offset:06X}" for item, offset in sorted(guessed.items())},
               "chosen": chosen}
    if opaque is not None:
        notes.append("'opaque': the frames drawn opaque (sprite entry bit 8: the end code is drawn as a color "
                     "and the row goes on past it), 'drawn' so in the scenarios or 'nearby' (a frame no scenario "
                     "draws, drawn the way of the frame before it).")
        content["opaque"] = {source: [f"{item:0{digits}X}" for item in sorted(items)]
                             for source, items in zip(("drawn", "nearby"), opaque)}
    path.write_text(json.dumps(content, indent=1) + "\n", encoding="ascii")
    print(f"written: {path}")


def resolve(census_dir):
    rom = sonic_rom.load_set(ROOT / "rom")
    program = sonic_rom.build_program(rom)
    # every even offset of the program ROM by its first two colors
    index = {}
    for offset in range(0, len(program) - 4, 2):
        index.setdefault(program[offset:offset + 4], []).append(offset)

    frames = asset_formats.sprite_frames(program, FRAME_RECORDS, FRAME_COUNT)
    by_word = {frame["word"]: frame["number"] for frame in frames}
    census = sorted(census_dir.glob("sprites_*.txt"))
    seen = read_census(census, 5)
    words = sorted(by_word)
    found, guessed = resolve_all(words, seen, program, index)
    opaque = resolve_opaque(words, read_opaque(census))
    write_palettes(ROOT / "sprite_palettes.json", "sprite frame (its sprite_frame, the record's offset in "
                   "sprite_frame_records)", {by_word[word]: offset for word, offset in found.items()},
                   {by_word[word]: offset for word, offset in guessed.items()}, 4,
                   [[by_word[word] for word in part] for part in opaque])
    print(f"  {len(frames)} frames: {len(found)} drawn, {len(guessed)} nearby, "
          f"{len(frames) - len(found) - len(guessed)} none; opaque: {len(opaque[0])} drawn, {len(opaque[1])} nearby")

    shown = read_census(sorted(census_dir.glob("tiles_*.txt")), 3)
    seen = {}
    for tile, sets in shown.items():
        # (the tile ROM repeats past its end: the board takes the tile number modulo its size)
        seen.setdefault(tile % TILE_COUNT, Counter()).update(sets)
    found, guessed = resolve_all(list(range(TILE_COUNT)), seen, program, index)
    write_palettes(ROOT / "tile_palettes.json", "16x16 tile (its number in the tile ROM)", found, guessed, 4)
    print(f"  {TILE_COUNT} tiles: {len(found)} shown, {len(guessed)} nearby, {TILE_COUNT - len(found) - len(guessed)} none")


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in ("run", "resolve"):
        raise SystemExit(__doc__.split("Usage:")[1])
    if sys.argv[1] == "run":
        run(Path(sys.argv[2]))
    else:
        resolve(Path(sys.argv[2]))


if __name__ == "__main__":
    main()
