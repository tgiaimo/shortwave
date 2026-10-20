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
- CI trimmed to Windows only: build (MSVC `cl` + Ninja, Release), Catch2
  tests, pluginval, upload zip.
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

- Repo scaffolded, committed and pushed to GitHub (`main`). Cloned on Thomas's
  Windows PC at `%USERPROFILE%\Projects\shortwave`; Debug build there has zero
  warnings and all 3 tests pass (2026-10-19).
- The source code is still the template's pass-through processor and editor.
- Known harmless warning (GCC only): `processBlock(AudioBuffer<double>&)` hidden
  — comes from the template's processor, not seen on MSVC/clang-cl.

## Open items, in order

1. ~~Get the code onto GitHub~~ — done: pushed to github.com/tgiaimo/shortwave
   (branch `main`). Clone it with `git clone --recurse-submodules
   https://github.com/tgiaimo/shortwave.git`.
2. ~~Fetch dependencies~~ — done (cloned with `--recurse-submodules`).
3. ~~Install the toolchain~~ — done: VS Community 2026 18.11, CMake 4.4.4,
   Git 2.55, Reaper 7.82.
4. ~~First build on Windows~~ — done: Debug build, zero warnings, 3/3 tests
   pass. Nothing Windows-specific needed fixing.
5. ~~Point Reaper at `VST3-Dev`, save the .rpp, confirm F5 opens Reaper~~ —
   done. (Ableton's VST3 custom folder still to set when first testing there.)
6. ~~Check the first GitHub Actions run passes~~ — done: run #4 green after
   switching CI to MSVC, pluginval passes.
7. ~~Phase 0 milestone: gain parameter + knob~~ — done by Thomas.

## Suggested first message in Claude Code

> Read docs/HANDOFF.md and docs/ROADMAP.md, then help me with the next open item.
