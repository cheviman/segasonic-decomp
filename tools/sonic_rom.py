"""
Name:        sonic_rom.py
Description: MAME 'sonic' ROM set (SegaSonic The Hedgehog, Japan rev. C) and memory region builders
Version:     0.1.0
Created:     11-10-2026
Modified:    14-10-2026
Author:      Sebastian Toyos

Files are identified by CRC32 + SHA1, not by name: dumps circulate with old names
(epr-c-87.ic17) and current ones (epr-15787c.ic17). Accepts a folder or the .zip as is.
Region layouts follow ROM_START( sonic ) in MAME src/mame/sega/segas32.cpp (BSD-3-Clause).
"""
import hashlib
import zipfile
import zlib
from pathlib import Path

SET_NAME = "sonic"    # MAME's name for the set

# MAME name -> (crc32, sha1, size)
ROM_FILES = {
    "epr-15787c.ic17": ("25e3c27e", "8f173cd5c7c817dcccdcad9be5781cfaa081d73e", 0x020000),
    "epr-15786c.ic8": ("efe9524c", "8020e734704a8f989919ee5ad92f70035de717f0", 0x020000),
    "epr-15781c.ic18": ("65b06c25", "9f524012a7adbc71737f90fc556f0ce9adc2bcf8", 0x040000),
    "epr-15780c.ic9": ("2db66fd2", "54582c0d5977649a38fc3a2c0fe4d7b1959abc76", 0x040000),
    "epr-15785.ic36": ("0fe7422e", "b7eaf4736ba155965317bb4ef3b33fc122635151", 0x040000),
    "mpr-15784.ic35": ("42f06714", "30e45bb2d9b492f0c1acc4fbe1e5869f0559300b", 0x100000),
    "mpr-15783.ic34": ("e4220eea", "a546c8bfc24e0695cf79c49e1a867d2595a1ed7f", 0x100000),
    "mpr-15782.ic33": ("cf56b5a0", "5786228aab120c3361524ba93b418b24fd5b8ffb", 0x100000),
    "mpr-15789.ic14": ("4378f12b", "826e0550a3c5f2b6e59c6531ac03658a4f826651", 0x100000),
    "mpr-15788.ic5": ("a6ed5d7a", "d30f26b452d380e7657e044e144f7dbbc4dc13e5", 0x100000),
    "mpr-15790.ic32": ("c69d51b1", "7644fb64457855f9ed87ca25ddc28c21bcb61fd9", 0x200000),
    "mpr-15792.ic30": ("1006bb67", "38c752e634aa94b1a23c09c4dba6388b7d0358af", 0x200000),
    "mpr-15794.ic28": ("8672b480", "61659e3856cdff0b2bca190a7e60c81b86ea2089", 0x200000),
    "mpr-15796.ic26": ("95b8011c", "ebc56ae49a76d04de60b0f81506769219a9885a7", 0x200000),
    "mpr-15791.ic31": ("42217066", "46d14c632da1bed02bc0a637e34ab9cbf356c5de", 0x100000),
    "mpr-15793.ic29": ("75bafe55", "ad33dae062c4bdf8d17d3f6f7c333aa2e7da260e", 0x100000),
    "mpr-15795.ic27": ("7f3dad30", "84be1c31df35e1c7fef77e83d6d135378789a1ef", 0x100000),
    "mpr-15797.ic25": ("013c6cab", "eb9b77d28815d2e225b0882706084a52b11c48ea", 0x100000),
}

# Windows of the V60 program image with unique content; the rest are mirrors (ROM_LOAD_x4 / _x2).
# Each chip is folded to the copy its code refers to: ic8 shows up four times in 0x080000-0x0FFFFF
# and absolute calls use 0x080000-0x09FFFF, even though the hardware reaches the reset vector
# (0xFFFFF0) and the vector table (0xFF000) through the last copy.
PROGRAM_AREAS = [
    (0x000000, 0x020000),  # epr-15787c.ic17
    (0x080000, 0x0A0000),  # epr-15786c.ic8
    (0x100000, 0x180000),  # epr-15781c.ic18 + epr-15780c.ic9, interleaved
]


def read_files(source):
    """Return {original_name: bytes} from a folder or a .zip."""
    source = Path(source)
    contents = {}
    if source.is_file() and source.suffix.lower() == ".zip":
        with zipfile.ZipFile(source) as archive:
            for entry in archive.infolist():
                if not entry.is_dir():
                    contents[Path(entry.filename).name] = archive.read(entry)
        return contents
    for path in sorted(source.iterdir()):
        if path.is_file():
            contents[path.name] = path.read_bytes()
    return contents


def identify_set(source):
    """Return ({mame_name: bytes}, [report lines]), matching every file by its hashes."""
    by_crc = {crc: name for name, (crc, _, _) in ROM_FILES.items()}
    found = {}
    report = []
    for original_name, data in read_files(source).items():
        crc = format(zlib.crc32(data) & 0xFFFFFFFF, "08x")
        mame_name = by_crc.get(crc)
        if mame_name and ROM_FILES[mame_name][1] == hashlib.sha1(data).hexdigest():
            found[mame_name] = data
            report.append(f"OK      {original_name:20} = {mame_name}")
        else:
            report.append(f"--      {original_name:20} not part of the set (crc {crc})")
    for mame_name in sorted(set(ROM_FILES) - set(found)):
        report.append(f"MISSING {mame_name}")
    return found, report


def load_set(source):
    """Like identify_set, but stops when a file is missing."""
    found, _ = identify_set(source)
    missing = sorted(set(ROM_FILES) - set(found))
    if missing:
        raise SystemExit("The 'sonic' ROM set is incomplete. Missing: " + ", ".join(missing))
    return found


def interleave16(even, odd):
    """ROM_LOAD16_BYTE: even bytes from one chip, odd bytes from the other."""
    result = bytearray(len(even) * 2)
    result[0::2] = even
    result[1::2] = odd
    return result


def build_program(rom):
    """mainpcb:maincpu region (2 MB, little endian) as the V60 sees it at 0x000000-0x1FFFFF."""
    image = bytearray(0x200000)
    image[0x000000:0x080000] = rom["epr-15787c.ic17"] * 4
    image[0x080000:0x100000] = rom["epr-15786c.ic8"] * 4
    interleaved = interleave16(rom["epr-15781c.ic18"], rom["epr-15780c.ic9"])
    image[0x100000:0x180000] = interleaved
    image[0x180000:0x200000] = interleaved
    return bytes(image)


def build_tiles(rom):
    """mainpcb:tiles region (2 MB): 16384 tiles of 16x16 pixels at 4 bpp, 128 bytes each."""
    return bytes(interleave16(rom["mpr-15789.ic14"], rom["mpr-15788.ic5"]))


# the sprite chips: four 16-bit lanes of 8-byte groups from each base
SPRITE_CHIPS = [
    (0x000000, ["mpr-15790.ic32", "mpr-15792.ic30", "mpr-15794.ic28", "mpr-15796.ic26"]),
    (0x800000, ["mpr-15791.ic31", "mpr-15793.ic29", "mpr-15795.ic27", "mpr-15797.ic25"]),
]


def build_sprites(rom):
    """mainpcb:sprites region (16 MB) in file byte order: every 32-bit word is big endian.

    ROM_LOAD64_WORD spreads 16-bit words from four chips over 8-byte groups.
    The four chips at 0x800000 are 16 Mbit parts whose upper half is 0xFF; the rest stays 0xFF.
    """
    image = bytearray(b"\xff" * 0x1000000)
    for base, chips in SPRITE_CHIPS:
        for lane, name in enumerate(chips):
            data = rom[name]
            end = base + (len(data) // 2) * 8
            image[base + lane * 2:end:8] = data[0::2]
            image[base + lane * 2 + 1:end:8] = data[1::2]
    return bytes(image)


def build_sound(rom):
    """mainpcb:soundcpu region (4 MB): Z80 program (x4) and three 1 MB banks of PCM samples."""
    image = bytearray(0x400000)
    image[0x000000:0x100000] = rom["epr-15785.ic36"] * 4
    image[0x100000:0x200000] = rom["mpr-15784.ic35"]
    image[0x200000:0x300000] = rom["mpr-15783.ic34"]
    image[0x300000:0x400000] = rom["mpr-15782.ic33"]
    return bytes(image)


def chips_from_regions(regions):
    """The set's chips back from the regions build_program, build_tiles, build_sprites and build_sound
    lay out ({region name: bytes}, every one of the four): {mame_name: bytes}. Each chip is read from
    the copy its region's builder writes first; the mirrors are not read."""
    program, tiles, sprites, sound = (regions[name] for name in ("program", "tiles", "sprites", "sound"))
    chips = {
        "epr-15787c.ic17": program[0x000000:0x020000],
        "epr-15786c.ic8": program[0x080000:0x0A0000],
        "epr-15781c.ic18": program[0x100000:0x180000:2],
        "epr-15780c.ic9": program[0x100001:0x180000:2],
        "mpr-15789.ic14": tiles[0::2],
        "mpr-15788.ic5": tiles[1::2],
        "epr-15785.ic36": sound[0x000000:0x040000],
        "mpr-15784.ic35": sound[0x100000:0x200000],
        "mpr-15783.ic34": sound[0x200000:0x300000],
        "mpr-15782.ic33": sound[0x300000:0x400000],
    }
    for base, names in SPRITE_CHIPS:
        for lane, name in enumerate(names):
            size = ROM_FILES[name][2]
            end = base + (size // 2) * 8
            chip = bytearray(size)
            chip[0::2] = sprites[base + lane * 2:end:8]
            chip[1::2] = sprites[base + lane * 2 + 1:end:8]
            chips[name] = chip
    return {name: bytes(chip) for name, chip in chips.items()}


def canonical_address(address):
    """Map a V60 address to its copy inside PROGRAM_AREAS, or None if it is not ROM.

    The bus is 24 bits wide; 0xF00000-0xFFFFFF mirrors the first MB of the region (MAME's map).
    """
    address &= 0xFFFFFF
    if address >= 0xF00000:
        address -= 0xF00000
    elif address >= 0x200000:
        return None
    if address < 0x080000:
        return address & 0x01FFFF
    if address < 0x100000:
        return 0x080000 | (address & 0x01FFFF)
    return 0x100000 | (address & 0x07FFFF)


class ProgramMemory:
    """V60 view of the program ROM for the disassembler: mirrors folded, anything else reads 0xFF."""

    def __init__(self, image):
        self.image = image

    def r8(self, address):
        offset = canonical_address(address)
        return self.image[offset] if offset is not None else 0xFF

    def r16(self, address):
        return self.r8(address) | (self.r8(address + 1) << 8)

    def r32(self, address):
        return self.r16(address) | (self.r16(address + 2) << 16)
