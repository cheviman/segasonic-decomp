"""
Name:        make_pack.py
Description: writes the game's assets pack, what the game loads when it starts: every asset of assets/
             (extract_assets.py takes them out of the player's ROM set), by name, where the board's memory
             has it
Version:     0.1.0
Created:     20-10-2026
Modified:    20-10-2026
Author:      Sebastian Toyos

Usage: python tools/make_pack.py [pack]           write the pack (default build/bin/segasonic.assets)
       python tools/make_pack.py --check [rom]   compare what the pack puts in the board's memory with
                                                  the ROM set (default rom/), the code left out, and the
                                                  first run's layout with the pack's
       python tools/make_pack.py --layout [file] write the pack's pieces without their bytes, as C
                                                  (default runtime/src/pack_layout.c)

The pack holds the assets alone: the program's and the sound program's data pieces (layout.json; the
code is in none, the C replaces it), the tile sheets, every sprite frame, the PCM samples, and the fills
the chips have between them. No byte comes from the ROM set: each is built from the asset's files
(data_formats.build_pieces, asset_formats). The game puts each where the board's memory has it, and the
board's mirrors of the chips (runtime/src/game_data.c).

Format (little endian): "SSAP", u16 version, u16 0, u32 entries; each entry: u8 kind (0 bytes, 1 fill),
u8 region (0 program, 1 tiles, 2 sprites, 3 sound), u16 name length, u32 offset, u32 size, the name, then
the bytes (kind 0) or the fill byte (kind 1).

The layout (--layout) is the pack's pieces in its order, each its kind, region, place, size and name:
positions only, as assets.json, with no byte of the ROM. The game's first run builds the same pack from
the player's ROM set with it (runtime/src/game_extract.c), as long as the assets are the ROM set's own.
"""
import json
import struct
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parent
sys.path.insert(0, str(TOOLS))
sys.path.insert(0, str(TOOLS / "core"))
sys.path.insert(0, str(ROOT))
import asset_formats  # noqa: E402
import data_formats  # noqa: E402
import extract_assets  # noqa: E402
import sonic_rom  # noqa: E402

MAGIC = b"SSAP"
VERSION = 1
KIND_BYTES, KIND_FILL = 0, 1
REGIONS = {"program": 0, "tiles": 1, "sprites": 2, "sound": 3}
REGION_SIZES = {"program": 0x200000, "tiles": 0x200000, "sprites": 0x1000000, "sound": 0x400000}
SPRITE_BLANK = 0xFF    # the sprite chips' bytes where no frame is
# the board's mirrors (runtime/src/game_data.c does the same): (region, from, size, to...)
MIRRORS = [
    ("program", 0x000000, 0x020000, (0x020000, 0x040000, 0x060000)),
    ("program", 0x080000, 0x020000, (0x0A0000, 0x0C0000, 0x0E0000)),
    ("program", 0x100000, 0x080000, (0x180000,)),
    ("sound", 0x000000, 0x040000, (0x040000, 0x080000, 0x0C0000)),
]


def data_entries(asset):
    """The data pieces of a program_data or sound_data asset; the code ('kept') is left out."""
    pieces = data_formats.build_pieces(ROOT / asset["path"], asset["size"], bytes(asset["size"]))
    for piece, offset, data in pieces:
        if piece["format"] != "kept":
            yield KIND_BYTES, piece["name"], asset["offset"] + offset, data


def tile_entries(asset):
    rows, _ = asset_formats.read_png_indexed(ROOT / asset["path"])
    data = asset_formats.tiles_from_sheet(rows, asset["columns"], asset["size"] // asset_formats.TILE_BYTES)
    yield KIND_BYTES, Path(asset["path"]).stem, asset["offset"], data


def sprite_entries(asset):
    """Every frame of every sheet (as asset_formats.build_sprite_rom puts them), over the chips' blank."""
    folder = ROOT / asset["path"]
    yield KIND_FILL, "sprites_blank", asset["offset"], (asset["size"], SPRITE_BLANK)
    for described in sorted(folder.rglob("*.json")):
        sheet = json.loads(described.read_text(encoding="ascii"))
        rows, _ = asset_formats.read_png_indexed(described.with_suffix(".png"))
        mask = (1 << sheet["bits"]) - 1
        for frame in sheet["frames"]:
            x, y = frame["x"], frame["y"]
            pens = [[pen & mask for pen in rows[y + line][x:x + frame["width"]]] for line in range(frame["height"])]
            yield (KIND_BYTES, f"{sheet['sheet']}[{frame['index']}]", int(frame["word"], 16) * 4,
                   asset_formats.pens_to_words(pens, sheet["bits"]))


def sample_entries(asset):
    """Every PCM sample (as extract_assets.build_samples puts them), over the end byte."""
    folder = ROOT / asset["path"]
    pieces = json.loads((folder / "samples.json").read_text(encoding="ascii"))["pieces"]
    yield KIND_FILL, "pcm_end", asset["offset"], (asset["size"], asset_formats.PCM_END)
    for piece in pieces:
        samples, _ = asset_formats.read_wav_unsigned(folder / piece["file"])
        yield KIND_BYTES, Path(piece["file"]).stem, int(piece["offset"], 16), asset_formats.unsigned_to_rf5c68(samples)


ENTRIES = {
    "program_data": data_entries,
    "sound_data": data_entries,
    "tiles_16x16x4": tile_entries,
    "sprite_frames": sprite_entries,
    "pcm_samples": sample_entries,
}


def pack_entries(manifest):
    """[(kind, region, name, offset, bytes or (size, fill byte))], the fills first."""
    entries = []
    for asset in manifest["assets"]:
        if asset["type"] not in ENTRIES:
            raise SystemExit(f"assets.json: no pack entries for type '{asset['type']}' ({asset['path']})")
        for kind, name, offset, data in ENTRIES[asset["type"]](asset):
            entries.append((kind, asset["region"], name, offset, data))
    entries.sort(key=lambda entry: entry[0] != KIND_FILL)
    return entries


def regions_from_entries(entries):
    """The board's memory as the game makes it from the pack: zeros, the entries, the mirrors."""
    regions = {name: bytearray(size) for name, size in REGION_SIZES.items()}
    for kind, region, _, offset, data in entries:
        if kind == KIND_FILL:
            size, value = data
            regions[region][offset:offset + size] = bytes([value]) * size
        else:
            regions[region][offset:offset + len(data)] = data
    for region, start, size, targets in MIRRORS:
        for target in targets:
            regions[region][target:target + size] = regions[region][start:start + size]
    return regions


def write_pack(path, entries):
    out = bytearray(MAGIC + struct.pack("<HHI", VERSION, 0, len(entries)))
    for kind, region, name, offset, data in entries:
        encoded = name.encode("ascii")
        size = data[0] if kind == KIND_FILL else len(data)
        if offset + size > REGION_SIZES[region]:
            raise SystemExit(f"{name}: {size} bytes at {offset:06X}, past the {region} region")
        out += struct.pack("<BBHII", kind, REGIONS[region], len(encoded), offset, size) + encoded
        out += bytes([data[1]]) if kind == KIND_FILL else data
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(out)
    return len(out)


def layout_text(name, entries):
    """The pieces of the pack, without their bytes, as C for the game's first run."""
    lines = ["/**",
             f" *    Name:        {name}",
             " *    Description: the assets pack's pieces, without their bytes: what the game's first run takes out",
             " *                 of the player's ROM set (game_extract.c)",
             " *    Version:     0.1.0",
             " *    Created:     20-10-2026",
             " *    Modified:    20-10-2026",
             " *    Author:      Sebastian Toyos",
             " *",
             " *    license:BSD-3-Clause",
             " *    Written by tools/make_pack.py --layout from the assets' layouts: positions only (as assets.json),",
             " *    no byte of the ROM; in the pack's order.",
             " */",
             '#include "game/extract.h"',
             "",
             "const pack_piece pack_pieces[] = {"]
    for kind, region, name, offset, data in entries:
        size, fill = data if kind == KIND_FILL else (len(data), 0)
        lines.append(f'\t{{ {kind}, {REGIONS[region]}, 0x{fill:02X}, 0x{offset:06X}u, 0x{size:X}u, "{name}" }},')
    lines += ["};", "", f"const uint32_t pack_piece_count = {len(entries)}u;", ""]
    return "\n".join(lines)


def write_layout(path, entries):
    path.write_text(layout_text(path.name, entries), encoding="ascii")


def code_ranges(manifest):
    """{region: [(start, end)]} of the code the pack leaves out, and its mirrors."""
    ranges = {name: [] for name in REGION_SIZES}
    for asset in manifest["assets"]:
        if asset["type"] not in ("program_data", "sound_data"):
            continue
        layout = json.loads((ROOT / asset["path"] / "layout.json").read_text(encoding="ascii"))
        base = int(layout["base"], 16)
        for piece in layout["pieces"]:
            if piece["format"] == "kept":
                start = asset["offset"] + int(piece["address"], 16) - base
                ranges[asset["region"]].append((start, start + piece["size"]))
    for region, start, size, targets in MIRRORS:
        for low, high in list(ranges[region]):
            if start <= low < start + size:
                ranges[region] += [(target + low - start, target + high - start) for target in targets]
    return ranges


def check(manifest, source):
    """What the pack puts in the board's memory against the ROM set's regions, the code left out."""
    rom = sonic_rom.load_set(source)
    regions = regions_from_entries(pack_entries(manifest))
    code = code_ranges(manifest)
    different = 0
    for name, builder in extract_assets.REGION_BUILDERS.items():
        original = builder(rom)
        outside = bytearray(len(original))
        for low, high in code[name]:
            outside[low:high] = b"\x01" * (high - low)
        count = sum(1 for index, (one, other) in enumerate(zip(regions[name], original))
                    if one != other and not outside[index])
        left_out = sum(outside)
        print(f"  {name:8} {count:9} bytes differ from the ROM set ({left_out} bytes of code left out)")
        different += count
    layout = ROOT / "runtime" / "src" / "pack_layout.c"
    written = layout.read_text(encoding="ascii").replace("\r\n", "\n")
    if written != layout_text(layout.name, pack_entries(manifest)):
        print(f"  {layout.relative_to(ROOT)} is not this pack's layout (the first run would write another pack)")
        different += 1
    return 1 if different else 0


def main():
    arguments = sys.argv[1:]
    manifest = extract_assets.load_manifest()
    if not extract_assets.assets_ready(manifest):
        raise SystemExit("assets/ does not hold this manifest's extraction")
    if arguments and arguments[0] == "--check":
        return check(manifest, Path(arguments[1]) if len(arguments) > 1 else ROOT / "rom")
    if arguments and arguments[0] == "--layout":
        path = Path(arguments[1]) if len(arguments) > 1 else ROOT / "runtime" / "src" / "pack_layout.c"
        entries = pack_entries(manifest)
        write_layout(path, entries)
        print(f"  {path}  ({len(entries)} pieces)")
        return 0
    path = Path(arguments[0]) if arguments else ROOT / "build" / "bin" / "segasonic.assets"
    entries = pack_entries(manifest)
    size = write_pack(path, entries)
    print(f"  {path}  ({len(entries)} assets, {size} bytes)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
