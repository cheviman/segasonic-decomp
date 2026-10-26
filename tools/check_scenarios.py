"""
Name:        check_scenarios.py
Description: runs the shared core's check_scenarios.py (tools/core) for this game: runs every control script on the reference and on the game, frame by frame, and checks the static analysis
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
runpy.run_path(str(TOOLS / "core" / "check_scenarios.py"), run_name="__main__")
