# Shortwave

A creative audio effect plugin (VST3) that makes anything sound like it's coming
through a shortwave radio: narrow bandwidth, hiss and static, fading signals,
detuned voices and whistles as you tune across the band. Later: delay, looper
and sampler modules built around the radio chain.

**Status:** Phase 0 — project setup. The plugin currently passes audio through
unchanged.

## Building

Windows is the main development platform. Full setup, including installing the
tools and testing in Reaper and Ableton, is in
[docs/SETUP-WINDOWS.md](docs/SETUP-WINDOWS.md). The short version:

```powershell
git clone --recurse-submodules https://github.com/tgiaimo/shortwave.git
cd shortwave
cmake -B Builds -G "Visual Studio 18 2026"
# open Builds\Shortwave.sln and build
```

Builds produce a VST3 plugin and a standalone app. Every push to GitHub is
built, tested and validated with [pluginval](https://github.com/Tracktion/pluginval)
by GitHub Actions; the built plugin can be downloaded from the run's artifacts.

## Project layout

| Folder | What's in it |
| --- | --- |
| `source/` | The plugin: audio processing (`PluginProcessor`) and interface (`PluginEditor`) |
| `tests/` | Unit tests (Catch2) |
| `benchmarks/` | Performance benchmarks |
| `dev/` | Your Reaper test project and test audio |
| `docs/` | Setup and development notes |
| `JUCE/`, `cmake/`, `modules/` | Dependencies (git submodules — don't edit) |
| `packaging/` | Installer config, for later releases |

## Built with

- [JUCE 9](https://juce.com) (Starter licence)
- [Pamplejuce](https://github.com/sudara/pamplejuce) template by Sudara Williams
  (MIT, see `LICENSE-pamplejuce`)
- [Melatonin Inspector](https://github.com/sudara/melatonin_inspector) for UI debugging
