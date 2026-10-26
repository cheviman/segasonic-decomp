"""
Name:        memory_census.py
Description: runs the shared core's memory_census.py (tools/core) for this game: what memory each mem_read and mem_write of the game's C reaches, over the scenarios
Version:     0.1.0
Created:     26-10-2026
Modified:    26-10-2026
Author:      Sebastian Toyos

The script is the core's (arcade-decomp, which tools/core holds); tools/game_profile.py tells it about this game.
Its usage is the core's.
"""
import runpy
import sys
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
sys.path[:0] = [str(TOOLS / "core"), str(TOOLS)]
runpy.run_path(str(TOOLS / "core" / "memory_census.py"), run_name="__main__")
