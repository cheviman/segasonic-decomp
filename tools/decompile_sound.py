"""
Name:        decompile_sound.py
Description: runs the shared core's decompile_sound.py (tools/core) for this game: the sound program's C from the ROM and sound_symbols.txt
Version:     0.1.0
Created:     25-10-2026
Modified:    25-10-2026
Author:      Sebastian Toyos

The script is the core's (arcade-decomp, which tools/core holds); tools/game_profile.py tells it about this game.
Its usage is the core's.
"""
import runpy
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
sys.path[:0] = [str(TOOLS / "core"), str(TOOLS)]
runpy.run_path(str(TOOLS / "core" / "decompile_sound.py"), run_name="__main__")
