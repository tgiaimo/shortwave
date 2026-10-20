# Shortwave — Development Roadmap

This is the living plan and progress record for the project. Tick tasks off as
they're done and add a row to the progress log after each work session.
(Originally drafted in the claude.ai Project "shortwave vst" on 2026-10-19;
this file is now the copy to keep up to date.)

## Vision and scope

Build a creative effect plugin that makes any audio sound like it is being
received over a shortwave radio, then grow it into a playground with delay,
looping and sampling. Thomas writes the code; this doc is the map and the
progress record.

The core effect is a signal chain. Each stage is a self-contained DSP block
that can be built, tested and tuned on its own:

1. **Tuning** — a dial that detunes the station: frequency shift (the "Donald
   Duck" mistuned-SSB sound), heterodyne whistles, and the signal sinking into
   noise when moving off-station.
2. **Bandwidth** — band-pass filtering that squeezes audio into a narrow,
   radio-like band, with a resonance or "IF filter" character control.
3. **Propagation** — slow random fading (QSB), selective fading via a drifting
   notch or comb filter, and pitch/time drift.
4. **Noise and interference** — hiss, static crackle, atmospheric bursts,
   carrier tones and adjacent-station bleed.
5. **Receiver** — AGC pumping, overload and soft-clip saturation, a small
   speaker-style output EQ.
6. **Mix** — dry/wet and output level.

Later modules (delay, looper, sampler) sit around this chain, for example a
delay whose repeats get re-"transmitted" through the radio on every pass.

## Decisions (settled)

| Decision | Choice |
| --- | --- |
| Operating system | Windows only |
| Framework | JUCE 9 (free Starter licence, covers up to $20,000 revenue a year — re-check terms on juce.com before ever selling) |
| Project template | Pamplejuce (CMake + Catch2 tests + pluginval in CI) |
| Plugin formats | VST3 + Standalone (Ableton on Windows loads VST3; CLAP parked) |
| IDE | Visual Studio 2026 Community, CMake 4.2+ |
| DAWs | Ableton Live for producing and phase-end checks; Reaper as the fast dev host |
| Experience | Some programming, little DSP; Phase 1 front-loads the essentials |
| Release goal | Personal/free for now; licence, signing and installer revisited in Phase 6 |

Tool installation and the build/debug setup are in
[SETUP-WINDOWS.md](SETUP-WINDOWS.md).

## Testing in the DAW while building

Compiled C++ plugins can't be swapped while a DAW has them loaded, so each code
change means rebuild, then reload. The setup keeps that cycle short:

- **Loop 1 — seconds, most of the time.** Run the Standalone build or JUCE's
  AudioPluginHost from Visual Studio's debugger with test audio and an
  analyzer. Good for DSP and GUI work.
- **Loop 2 — real DAW context.** F5 in Visual Studio builds, copies the VST3 to
  `%USERPROFILE%\VST3-Dev`, and opens Reaper (with `dev/ShortwaveTest.rpp` if
  saved) with the debugger attached. Use it for automation, state saving,
  sample-rate and buffer-size changes, and offline render.
- **Loop 3 — Ableton.** At the end of each phase. Live locks the plugin file
  while open, so quit Live before rebuilding.

Two habits make the plugin feel live while building:

- **Expose every magic number as a parameter during development** (filter
  cutoffs, fade rates, noise colour) and tune by ear instead of recompiling.
  Hide or fix the ones users shouldn't see later.
- **Tune the GUI live** with Melatonin Inspector (bundled).

True DSP hot-reload is possible with [Cmajor](https://cmajor.dev)'s JIT plugin,
but it's a separate language: a sketchpad for odd experiments only.

## Roadmap

Seven phases, each ending in something loadable in a DAW. Don't move on until
the phase's milestone works.

| Phase | Focus | Ends with |
| --- | --- | --- |
| 0 | Setup | Gain knob in Reaper, debugger attached |
| 1 | Foundations | Parameters, state, smoothing, tests |
| 2 | Radio core | Filter, hiss, fading, generic controls — **first version usable on tracks** |
| 3 | Shortwave character | Tuning, whistles, QSB, crackle, AGC |
| 4 | Interface | Radio-style GUI, tuning dial, meter |
| 5 | Creative modules | Delay, then looper, then sampler |
| 6 | Release | Presets, CPU, DAW tests, signing |

### Phase 0 — Setup and "hello plugin"

Milestone: a plugin with one gain knob loads in Reaper, and a breakpoint in
`processBlock` stops the debugger.

- [x] Install toolchain (see SETUP-WINDOWS.md step 1)
- [x] Create repo from Pamplejuce, rename the plugin, first commit
- [x] Push to GitHub (github.com/tgiaimo/shortwave)
- [x] Build Standalone + VST3 on the Windows PC; open in a host (Reaper)
- [x] Wire Loop 2: save `dev/ShortwaveTest.rpp`, F5 launches Reaper
- [x] Add one gain parameter and knob (Thomas writes this)
- [ ] Pass pluginval (runs automatically in GitHub Actions on every push)

### Phase 1 — Foundations

Milestone: parameters save and recall with the DAW project, change smoothly
without clicks, and are covered by a unit test.

- [ ] Learn the real-time rules: no memory allocation, locks, file or console I/O on the audio thread
- [x] Parameters via `AudioProcessorValueTreeState`; state save/restore (done early, with the Phase 0 gain knob)
- [ ] Parameter smoothing to avoid zipper noise
- [ ] Handle `prepareToPlay`: sample rate, block size, mono/stereo layouts
- [ ] First Catch2 test (e.g. gain of 0 dB passes audio unchanged)

### Phase 2 — Radio core v0.1

Milestone: on a vocal track it clearly reads as "radio", using JUCE's generic
editor for controls.

- [ ] Bandwidth: high-pass + low-pass (or band-pass) with adjustable width and centre
- [ ] Noise: white and pink hiss with level control
- [ ] Simple fading: slow random amplitude modulation
- [ ] Soft-clip saturation stage
- [ ] Dry/wet + output gain

### Phase 3 — Shortwave character

Milestone: sweeping the tuning dial sounds like searching the band — whistles,
detuned voices, a station locking in.

- [ ] Frequency shifter (Hilbert transform) for mistuned-SSB voices
- [ ] Heterodyne whistle tied to tuning offset
- [ ] Signal-to-noise crossfade driven by distance from "on station"
- [ ] Selective fading: slowly drifting notch or comb filter
- [ ] Static crackle and atmospheric bursts
- [ ] AGC with audible pumping
- [ ] Seedable randomness so a preset sounds repeatable

### Phase 4 — Custom interface

Milestone: a radio-style GUI worth using, with a draggable tuning dial and a
signal-strength meter.

- [ ] Sketch the layout before coding
- [ ] Custom `LookAndFeel` for knobs and the tuning dial
- [ ] Meters fed from the audio thread safely (atomics or lock-free FIFO)
- [ ] Resizable window

### Phase 5 — Creative modules

Milestone: one extension shipped inside the plugin, most likely the delay.

- [ ] Delay whose repeats pass back through the radio chain
- [ ] Looper (capture, overdub, feed through the radio)
- [ ] Sampler idea — decide the scope first (see Parking lot)

### Phase 6 — Polish and release

Milestone: a version a friend can install and use without help.

- [ ] Presets and a preset browser
- [ ] CPU profiling; oversampling where saturation aliases
- [ ] Test in at least three DAWs and at 44.1/48/96 kHz
- [ ] Windows code signing via the CI (Azure Trusted Signing)
- [ ] Installer (packaging/installer.iss), README, version 1.0

## Learning resources

| Resource | Use it for | Phase |
| --- | --- | --- |
| JUCE tutorials (juce.com/learn) | Plugin basics, parameters, the DSP module | 0–2 |
| [Pamplejuce docs](https://github.com/sudara/pamplejuce) and Sudara's blog | CMake setup, testing, CI | 0–1 |
| The Audio Programmer (YouTube + Discord) | Walkthroughs and a place to ask questions | All |
| *Designing Audio Effect Plugins in C++* — Will Pirkle | Filters, delays, modulation, with code | 2–5 |
| Julius O. Smith's online books (CCRMA) | Filter and frequency-shifter theory | 3 |
| Ross Bencina, "Real-time audio programming 101" | Why the audio thread rules exist | 1 |
| JUCE forum | Host-specific bugs and workflow tips | All |

## Progress log

Newest first. Add a row after each work session.

| Date | Phase | What happened |
| --- | --- | --- |
| 2026-10-19 | 0 | CI fix: the Windows build had been failing since setup because the runner's clang-cl 20 clashes with newer MSVC intrinsics headers (libwebp in `juce_graphics`). CI now builds with MSVC `cl`, matching local builds. Release build verified locally: zero warnings, 3/3 tests. |
| 2026-10-19 | 0 | Thomas added the gain parameter (APVTS, −48 to +12 dB, state save/restore) and a rotary knob laid out with `juce::Grid`. Debugged three runtime crashes on the way: unassigned `gainParameter` pointer, label attached to itself, mismatched parameter ID (`"VOL"` vs `"gain"`). Builds warning-free, 3/3 tests pass. Pushed for the first pluginval run in CI. |
| 2026-10-19 | 0 | Shortwave loads in Reaper from `VST3-Dev`; saved `dev/shortwaveTest.rpp` and F5 in Visual Studio opens it with the debugger attached. Next: gain parameter + knob. |
| 2026-10-19 | 0 | Moved to Claude Code. Installed VS Community 2026 (18.11, MSVC 19.51), CMake 4.4.4, Git 2.55, Reaper 7.82 via winget. Cloned to `%USERPROFILE%\Projects\shortwave` with submodules. First Windows Debug build: VST3 + Standalone built with zero warnings, VST3 copied to `VST3-Dev`, all 3 tests pass. `SHORTWAVE_DEBUG_HOST` found Reaper. Not yet opened in a host. |
| 2026-10-19 | 0 | Repo scaffolded from Pamplejuce (JUCE 9.0.5): renamed to Shortwave, VST3 + Standalone, VST3-Dev copy folder, F5-launches-Reaper debugging, Windows-only CI, setup guide. Built and all 3 tests passed (verified on Linux). Pushed to GitHub. |
| 2026-10-19 | — | Roadmap drafted; Windows + Ableton confirmed |

## Parking lot

Ideas to revisit, not commitments yet.

- Sampler: as an effect it could capture snippets of incoming audio and replay
  them as "other stations" while tuning; a full MIDI-played sampler would be a
  separate instrument plugin
- Built-in "station" samples (numbers-station tones, Morse, time signals) —
  must be recordings Thomas makes or has rights to
- Tuning dial mapped to MIDI or automation for performance
- CLAP format support
- Stereo tricks: different fading per channel, or a second station in one side
