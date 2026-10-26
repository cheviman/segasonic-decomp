"""
Name:        disassemble.py
Description: runs the shared core's disassemble.py (tools/core) for this game: the program's listing, from the static analysis
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
runpy.run_path(str(TOOLS / "core" / "disassemble.py"), run_name="__main__")
