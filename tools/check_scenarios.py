"""
Name:        check_scenarios.py
Description: runs every control script on the reference and on the translated game, compares them frame by
             frame, and checks that the static analysis alone finds all the code the reference ran
Version:     0.1.0
Created:     11-10-2026
Modified:    13-10-2026
Author:      Sebastian Toyos

Usage: python tools/check_scenarios.py [NAME ...] [--native-only] [--translated] [--video] [--sound] [--jobs N]
                                       [--bin DIR] [--out DIR]
  NAME            scripts of tests/controls to run (file names without .txt); all of them by default
  --native-only   reuse the reference results already in build/scenarios/ (a script without them
                  still runs on the reference)
  --translated    run the translation scaffold (tools/recompile.py) instead of the decompiled game
  --video         compare the frames too: MAME's video in the reference (video_hashes.txt) against the
                  board's video in the game (game_video_hashes.txt), frame by frame
  --sound         compare the sound too: the reference runs MAME's Z80 and RF5C68 and ymfm's YM3438 with the
                  game's timing model (build/sound/timing.txt, tools/decompile_sound.py), the game its own
                  sound program and chips; per frame, the samples and the sound board's memories
                  (sound_hashes.txt against game_sound_hashes.txt)
  --jobs N        run N scripts at a time (half the processors by default)
  --bin DIR       the executables to run (build/bin by default): another build of the game
  --out DIR       where the runs go (build/scenarios by default)

For every script: build/bin/s32ref writes build/scenarios/NAME/{executed,called,hashes}.txt, the
game (build/bin/sonic_game_headless, from tools/decompile.py) writes game_hashes.txt, and the two hash
lists are compared. Then the static analysis (tools/disassemble.py, symbols.txt, no traces) is run once,
and every address the reference executed in any scenario has to be among the instructions it found:
the ones that are not are listed by the place they are entered from, which is where symbols.txt or the
analysis falls short.
"""
import os
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

TOOLS = Path(__file__).resolve().parent
ROOT = TOOLS.parent
sys.path.insert(0, str(TOOLS))
sys.path.insert(0, str(TOOLS / "core"))
import disassemble  # noqa: E402
import sonic_rom  # noqa: E402
from v60 import disassembler as dis  # noqa: E402

CONTROLS = ROOT / "tests" / "controls"
RESULTS = ROOT / "build" / "scenarios"
BIN = ROOT / "build" / "bin"
SOUND_TIMING = ROOT / "build" / "sound" / "timing.txt"


def executable(name):
    path = BIN / (name + ".exe")
    return path if path.exists() else BIN / name


def run_reference(script, out, video, sound=False):
    out.mkdir(parents=True, exist_ok=True)
    extra = ["--video-hashes", str(out / "video_hashes.txt")] if video else []
    if sound:
        extra += ["--sound-timing", str(SOUND_TIMING), "--sound-hashes", str(out / "sound_hashes.txt")]
    result = subprocess.run([str(executable("s32ref")), "--script", str(script),
                             "--trace", str(out / "executed.txt"), "--calls", str(out / "called.txt"),
                             "--hashes", str(out / "hashes.txt")] + extra,
                            capture_output=True, text=True)
    stopped = [line for line in result.stderr.splitlines() if line.startswith("V60 stopped")]
    return stopped[0] if stopped else None


GAMES = {"decompiled": ("sonic_game_headless", "game_hashes.txt"),
         "translated": ("sonic_headless", "native_hashes.txt")}


def run_native(script, out, game, video, sound=False):
    name, hashes = GAMES[game]
    extra = ["--video-hashes", str(out / "game_video_hashes.txt")] if video else []
    if sound:
        extra += ["--sound-hashes", str(out / "game_sound_hashes.txt")]
    try:
        result = subprocess.run([str(executable(name)), "--script", str(script),
                                 "--hashes", str(out / hashes)] + extra, capture_output=True, text=True, timeout=900)
    except subprocess.TimeoutExpired:
        return "did not finish in 15 minutes"
    problems = [line for line in result.stderr.splitlines()
                if not line.startswith("[s32] unmapped write E")]
    return problems[0] if problems else None


def compare(reference_file, native_file):
    """(frames compared, first frame whose hash differs or None)"""
    reference = reference_file.read_text(encoding="ascii").split("\n")
    native = native_file.read_text(encoding="ascii").split("\n")
    reference = [line for line in reference if line]
    native = [line for line in native if line]
    for index, (expected, actual) in enumerate(zip(reference, native)):
        if expected != actual:
            return len(reference), index
    return len(reference), None if len(native) == len(reference) else min(len(native), len(reference))


def static_check(names):
    """Executed addresses of the scenarios that the analysis without traces does not decode"""
    image = sonic_rom.build_program(sonic_rom.load_set(ROOT / "rom"))
    traces = [RESULTS / name / "executed.txt" for name in names if (RESULTS / name / "executed.txt").exists()]
    executed, _ = disassemble.load_trace(traces)
    analysis = disassemble.Analysis(image, disassemble.load_symbols(ROOT / "symbols.txt"))
    analysis.run()
    missing = sorted(address for address in executed if address not in analysis.instructions)
    # where each missing stretch is entered: not the next instruction of another missing one
    entries = []
    previous_end = None
    for address in missing:
        if address != previous_end:
            entries.append(address)
        instruction = dis.decode(analysis.mem, address)
        previous_end = address + instruction.length
    return len(executed), missing, entries


def main():
    global BIN, RESULTS
    arguments = sys.argv[1:]
    native_only = "--native-only" in arguments
    game = "translated" if "--translated" in arguments else "decompiled"
    video = "--video" in arguments
    sound = "--sound" in arguments
    if sound and not SOUND_TIMING.exists():
        raise SystemExit(f"{SOUND_TIMING} not found")
    for option in ("--bin", "--out"):
        if option in arguments:
            position = arguments.index(option)
            folder = Path(arguments[position + 1]).resolve()
            if option == "--bin":
                BIN = folder
            else:
                RESULTS = folder
            del arguments[position:position + 2]
    jobs = max(1, (os.cpu_count() or 2) // 2)
    if "--jobs" in arguments:
        position = arguments.index("--jobs")
        jobs = max(1, int(arguments[position + 1]))
        del arguments[position:position + 2]
    names = [argument for argument in arguments if not argument.startswith("--")]
    if not names:
        names = sorted(path.stem for path in CONTROLS.glob("*.txt"))
    for name in names:
        if not (CONTROLS / f"{name}.txt").exists():
            raise SystemExit(f"{CONTROLS / (name + '.txt')}: no such script")

    def check(name):
        script = CONTROLS / f"{name}.txt"
        out = RESULTS / name
        reused = native_only and (out / "hashes.txt").exists() and (not video or (out / "video_hashes.txt").exists()) \
            and (not sound or (out / "sound_hashes.txt").exists())
        stopped = None if reused else run_reference(script, out, video, sound)
        problem = run_native(script, out, game, video, sound)
        frames, differs = compare(out / "hashes.txt", out / GAMES[game][1])
        video_differs = compare(out / "video_hashes.txt", out / "game_video_hashes.txt")[1] if video else None
        sound_differs = compare(out / "sound_hashes.txt", out / "game_sound_hashes.txt")[1] if sound else None
        if stopped:
            verdict = f"reference: {stopped}"
        elif differs is not None:
            verdict = f"differs from frame {differs}" + (f" ({problem})" if problem else "")
        elif video_differs is not None:
            verdict = f"the picture differs from frame {video_differs}"
        elif sound_differs is not None:
            verdict = f"the sound differs from frame {sound_differs}"
        else:
            verdict = "identical"
        return frames, verdict

    print(f"{'scenario':<16} {'frames':>7}  result")
    failures = 0
    with ThreadPoolExecutor(max_workers=jobs) as pool:
        for name, (frames, verdict) in zip(names, pool.map(check, names)):
            failures += verdict != "identical"
            print(f"{name:<16} {frames:7}  {verdict}", flush=True)

    executed, missing, entries = static_check(names)
    print(f"\nstatic analysis: {executed - len(missing)} of {executed} executed addresses found without traces")
    for address in entries[:40]:
        print(f"  not found: {address:06X}")
    return 1 if failures or missing else 0


if __name__ == "__main__":
    sys.exit(main())
