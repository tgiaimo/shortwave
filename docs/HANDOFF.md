# Handoff from the claude.ai planning conversation

Written 2026-10-19 at the end of the first planning session in claude.ai, so a
Claude Code session can pick up exactly where it left off.

## Who and how

- **Thomas (GitHub: tgiaimo)** is the main contributor and writes the plugin
  code. Claude is a guide and progress-keeper: explain, suggest, review, help
  debug, keep `docs/ROADMAP.md` current. Don't write DSP/plugin features unless
  Thomas explicitly asks for code.
- Thomas is on **Windows** and makes music in **Ableton Live**. Reaper is the
  dev host because it starts fast and reloads plugins easily.
- Experience: some programming, little DSP so far. Explain DSP concepts from
  first principles when they come up.

## What the project is

Shortwave: a creative VST3 effect emulating shortwave radio reception (narrow
bandwidth, hiss, static, fading, detuning/whistles, AGC), later extended with
delay, looper and maybe a sampler. Full plan in `docs/ROADMAP.md`.

## What was decided in planning

- JUCE 9 + Pamplejuce template, CMake, Visual Studio 2026, CMake 4.2+.
- VST3 + Standalone only. CLAP removed for simplicity (parked).
- CI trimmed to Windows only: build, Catch2 tests, pluginval, upload zip.
  macOS/Linux jobs, installer and code signing removed until Phase 6.
- Intel IPP off.
- After each build the VST3 is copied to `%USERPROFILE%\VST3-Dev` (the system
  VST3 folder needs admin rights). Reaper and Ableton scan that folder.
- In Visual Studio, F5 on the `Shortwave_VST3` project launches Reaper
  (`SHORTWAVE_DEBUG_HOST` cache variable) and opens `dev/ShortwaveTest.rpp` if
  it existed at configure time.
- Plugin IDs: manufacturer `Tgmo`, plugin `Swv1`, bundle `com.tgiaimo.shortwave`.
  `COMPANY_NAME` is a placeholder (`tgiaimo`) — Thomas may want to change it.
- The template's MIT licence was renamed `LICENSE-pamplejuce`; Shortwave's own
  licence is still undecided.

## Current state

- Repo scaffolded and committed as one commit ("Set up Shortwave plugin project
  from the Pamplejuce template"). It configured, built, and passed all 3 tests
  on Linux. It has **not** yet been built on Thomas's Windows PC.
- The source code is still the template's pass-through processor and editor.
- Known harmless warning (GCC only): `processBlock(AudioBuffer<double>&)` hidden
  — comes from the template's processor, not seen on MSVC/clang-cl.

## Open items, in order

1. **Get the code onto GitHub.** The repo github.com/tgiaimo/shortwave exists
   (public, was empty). If this folder came from the zip, push it:
   `git remote set-url origin https://github.com/tgiaimo/shortwave.git` then
   `git push -u origin main`. If GitHub already has the commit, just clone it.
2. **Fetch dependencies**: `git submodule update --init --recursive`
   (JUCE, cmake-includes, melatonin_inspector).
3. **Install the toolchain** (docs/SETUP-WINDOWS.md step 1): Visual Studio 2026
   with the C++ workload, CMake 4.2+, Git, Reaper.
4. **First build on Windows**: `cmake -B Builds -G "Visual Studio 18 2026"`,
   build Debug, run tests. Fix anything Windows-specific that shows up.
5. Point Reaper and Ableton at `%USERPROFILE%\VST3-Dev`, save
   `dev/ShortwaveTest.rpp`, re-run cmake, confirm F5 opens Reaper.
6. Check the first GitHub Actions run passes.
7. Then Phase 0's real milestone: Thomas adds a gain parameter + knob himself,
   with Claude guiding (AudioProcessorValueTreeState is the next concept).

## Suggested first message in Claude Code

> Read docs/HANDOFF.md and docs/ROADMAP.md, then help me with the next open item.
