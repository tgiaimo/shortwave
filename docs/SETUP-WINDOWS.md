# Windows setup

Everything you need to build Shortwave and test it in Reaper and Ableton.
Do the steps in order; each one only needs doing once.

## 1. Install the tools

Open **PowerShell** and run these one at a time. Each pops up a normal installer
or permission prompt; accept it.

```powershell
# Visual Studio 2026 Community with the C++ desktop workload (large download, ~10 GB)
winget install --id Microsoft.VisualStudio.Community --exact --override "--passive --wait --add Microsoft.VisualStudio.Workload.NativeDesktop --includeRecommended"

# CMake (4.2 or newer is needed for Visual Studio 2026)
winget install --id Kitware.CMake --exact

# Git
winget install --id Git.Git --exact

# Reaper, the fast dev DAW (free to evaluate)
winget install --id Cockos.REAPER --exact
```

Close and reopen PowerShell afterwards so it picks up the new tools, then check:

```powershell
cmake --version   # should say 4.2 or higher
git --version
```

Optional: a free spectrum analyzer such as Voxengo SPAN, for looking at what your
filters and noise are doing.

## 2. Get the code

Pick a folder for your projects (avoid OneDrive-synced folders, they slow builds
and can lock files), then:

```powershell
cd $HOME\Projects        # or wherever you keep code
git clone --recurse-submodules https://github.com/tgiaimo/shortwave.git
cd shortwave
```

`--recurse-submodules` matters: it pulls in JUCE and the other dependencies.
If you forgot it, run `git submodule update --init --recursive`.

## 3. Generate the Visual Studio solution and build

```powershell
cmake -B Builds -G "Visual Studio 18 2026"
```

Then open `Builds\Shortwave.sln` in Visual Studio. In the toolbar, keep the
configuration on **Debug** and press **Ctrl+Shift+B** to build everything.

The first build compiles all of JUCE and takes a few minutes. Later builds only
recompile what you changed.

What you get, in `Builds\Shortwave_artefacts\Debug\`:

| Folder | What it is |
| --- | --- |
| `VST3\Shortwave.vst3` | The plugin. Also copied to `%USERPROFILE%\VST3-Dev` after each build |
| `Standalone\Shortwave.exe` | The plugin as its own app, for quick tests |

Run the tests: in Visual Studio, **Test → Run All Tests**, or from PowerShell:

```powershell
ctest --test-dir Builds -C Debug --output-on-failure
```

Re-run the `cmake -B Builds ...` command whenever you add new source files.

## 4. Point your DAWs at the dev folder

The build copies the plugin to `C:\Users\<you>\VST3-Dev` (no admin rights needed).
Tell each DAW to look there:

- **Reaper**: Options → Preferences → Plug-ins → VST → add the folder to the
  VST plug-in paths, then **Re-scan**. Shortwave shows up under VST3 effects.
- **Ableton Live**: Settings → Plug-Ins → turn on **Use VST3 Custom Folder**,
  browse to the folder, then **Rescan**.

## 5. Set up the debug loop (press F5, Reaper opens)

The solution is already configured so that pressing **F5** in Visual Studio
builds the VST3, copies it, and opens Reaper with the debugger attached.

1. Open Reaper, make a track with a looping audio clip (a voice and a drum loop
   are great test material), and add Shortwave as an effect on it.
2. Save the project as `dev\ShortwaveTest.rpp` inside the repo, with the audio
   files next to it.
3. Re-run `cmake -B Builds -G "Visual Studio 18 2026"` so CMake picks up the project.
4. Close Reaper. From now on, F5 opens straight into that project.

The loop is: stop debugging (Reaper closes) → change code → F5. Breakpoints in
`processBlock` will stop Reaper mid-playback so you can inspect values.

If Reaper is installed somewhere else, or you want a different host:

```powershell
cmake -B Builds -DSHORTWAVE_DEBUG_HOST="C:/path/to/host.exe"
```

## 6. Quick-test host: AudioPluginHost (optional)

JUCE's own lightweight host is handy for wiring test signals and analyzers
around the plugin:

```powershell
cmake -B Builds -DJUCE_BUILD_EXTRAS=ON
cmake --build Builds --target AudioPluginHost --config Debug
```

It ends up under `Builds\JUCE\extras\AudioPluginHost\`.

## 7. Testing in Ableton

Ableton keeps a loaded plugin's file locked until you quit Live, so the build
can't overwrite it while Live is open. Use Ableton for checks at the end of a
phase: quit Live → build → reopen Live → rescan if needed.

## Troubleshooting

| Problem | Fix |
| --- | --- |
| `Could not create named generator Visual Studio 18 2026` | CMake is older than 4.2. Run `winget upgrade Kitware.CMake` |
| CMake can't find a compiler | The C++ workload is missing. Open the Visual Studio Installer → Modify → tick "Desktop development with C++" |
| Build fails copying the VST3 | A DAW still has the plugin loaded. Close it and build again |
| Errors about missing JUCE files | Submodules weren't fetched: `git submodule update --init --recursive` |
| Plugin doesn't appear in the DAW | Check the dev folder is in its scan paths and rescan |
