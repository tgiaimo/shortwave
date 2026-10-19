cmake_minimum_required(VERSION 3.25)

# This tells cmake we have goodies in the /cmake folder
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (PamplejuceVersion)

# Modern concise way to add dependencies to your project
include (CPM)

# Configures universal binaries and decides which version of macOS to support
include(PamplejuceMacOS)

# Couple tweaks that IMO should be JUCE defaults
include(JUCEDefaults)

# Change me!
# This is the internal name of the project and the name of JUCE's shared code target
# Note: This cannot have spaces (it may be 2024, but you can't have it all!)
# Worry not, JUCE's PRODUCT_NAME can have spaces (and is what DAWs display)
set(PROJECT_NAME "Shortwave")

# Worry not, JUCE's PRODUCT_NAME can have spaces (and is what DAWs will display)
# You can also just have it be the same thing as PROJECT_NAME
# You may want to append the major version on the end of this (and PROJECT_NAME) ala:
#   set(PROJECT_NAME "MyPlugin_v${MAJOR_VERSION}")
# Doing so enables major versions to show up in IDEs and DAWs as separate plugins
# allowing you to change parameters and behavior without breaking existing user projects
set(PRODUCT_NAME "Shortwave")

# Change me! Used for the MacOS bundle name and Installers
set(COMPANY_NAME "tgiaimo")

# Change me! Used for the MacOS bundle identifier (and signing)
set(BUNDLE_ID "com.tgiaimo.shortwave")

# Change me! Set the plugin formats you want built
# Valid choices: AAX Unity VST VST3 AU AUv3 Standalone
set(FORMATS Standalone VST3)

# Force JUCE to work with sccache
# instruct MSVC to embed debug info (/Z7) instead of emitting a .pdb
set(CMAKE_POLICY_DEFAULT_CMP0141        NEW      CACHE STRING "" FORCE)
set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT Embedded CACHE STRING "" FORCE)

# For simplicity, the name of the CMake project is also the name of the target
project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# Log build environment info (after project() so compiler info is available)
include(PamplejuceLog)

# Defines the pamplejuce_* functions we call below (also fetches Catch2 at include time)
include(PamplejuceFunctions)

# JUCE is setup as a submodule in the /JUCE folder
# Locally, you must run `git submodule update --init --recursive` once
# and later `git submodule update --remote --merge` to keep it up to date
# On Github Actions, this is done as a part of actions/checkout
add_subdirectory(JUCE)

# Add any other modules you want modules here, before the juce_add_plugin call
# juce_add_module(modules/my_module)

# This adds the melatonin inspector module
add_subdirectory (modules/melatonin_inspector)

# Where the VST3 is copied after every build.
# On Windows the system folder (C:\Program Files\Common Files\VST3) needs admin
# rights, so we copy to a per-user dev folder instead. Add this folder to
# Reaper's and Ableton's VST3 scan paths once (see docs/SETUP-WINDOWS.md).
if (WIN32)
    file(TO_CMAKE_PATH "$ENV{USERPROFILE}/VST3-Dev" SHORTWAVE_VST3_DEV_DIR)
    set(SHORTWAVE_VST3_COPY_ARGS VST3_COPY_DIR "${SHORTWAVE_VST3_DEV_DIR}")
endif ()

# See `docs/CMake API.md` in the JUCE repo for all config options
juce_add_plugin("${PROJECT_NAME}"
    # Icons for the standalone app
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/packaging/icon.png"

    COMPANY_NAME "${COMPANY_NAME}"
    BUNDLE_ID "${BUNDLE_ID}"

    # Copy the built plugin somewhere DAWs can find it, after every build
    COPY_PLUGIN_AFTER_BUILD TRUE
    ${SHORTWAVE_VST3_COPY_ARGS}

    # This is an audio effect (not a synth) with no MIDI in or out
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    VST3_CATEGORIES Fx

    # A four-character manufacturer id
    # First character MUST be uppercase for AU formats
    PLUGIN_MANUFACTURER_CODE Tgmo

    # A unique four-character plugin id
    # Note: this must have at least one upper-case character
    # Never change this after people start saving projects with the plugin
    PLUGIN_CODE Swv1
    FORMATS "${FORMATS}"

    # The name of your final executable
    # This is how it's listed in the DAW
    # This can be different from PROJECT_NAME and can have spaces!
    # You might want to use v${MAJOR_VERSION} here once you go to v2...
    PRODUCT_NAME "${PRODUCT_NAME}")

# This lets us use our code in both the JUCE targets and our Test target
# Without running into ODR violations
add_library(SharedCode INTERFACE)

# Enable fast math, C++23 and a few other target defaults
pamplejuce_shared_code_defaults()


# Manually list all .h and .cpp files for the plugin
# If you are like me, you'll use globs for your sanity.
# Just ensure you employ CONFIGURE_DEPENDS so the build system picks up changes
# If you want to appease the CMake gods and avoid globs, manually add files like so:
# set(SourceFiles Source/PluginEditor.h Source/PluginProcessor.h Source/PluginEditor.cpp Source/PluginProcessor.cpp)
file(GLOB_RECURSE SourceFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h")
target_sources(SharedCode INTERFACE ${SourceFiles})

# Adds a BinaryData target for embedding assets into the binary
pamplejuce_add_assets()

# MacOS only: Cleans up folder and target organization on Xcode.
pamplejuce_xcode_prettify()

# This is where you can set preprocessor definitions for JUCE and your plugin
target_compile_definitions(SharedCode
    INTERFACE

    # JUCE_WEB_BROWSER and JUCE_USE_CURL off by default
    JUCE_WEB_BROWSER=0  # If you set this to 1, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you set this to 1, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0

    # lets the app known if we're Debug or Release
    CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    VERSION="${CURRENT_VERSION}"

    # JucePlugin_Name is for some reason doesn't use the nicer PRODUCT_NAME
    PRODUCT_NAME_WITHOUT_VERSION="Shortwave"
)

# Link to any other modules you added (with juce_add_module) here!
# Usually JUCE modules must have PRIVATE visibility
# See https://github.com/juce-framework/JUCE/blob/master/docs/CMake%20API.md#juce_add_module
# However, with Pamplejuce, you'll link modules to SharedCode with INTERFACE visibility
# This allows the JUCE plugin targets and the Tests target to link against it
target_link_libraries(SharedCode
    INTERFACE
    Assets
    melatonin_inspector
    juce_audio_utils
    juce_audio_processors
    juce_dsp
    juce_gui_basics
    juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)

# Link the JUCE plugin targets our SharedCode target
target_link_libraries("${PROJECT_NAME}" PRIVATE SharedCode)

# Visual Studio: pressing Debug (F5) builds the VST3, copies it, then opens
# Reaper with the debugger attached. If dev/ShortwaveTest.rpp exists, Reaper
# opens that test project. Change the host with:
#   cmake -B Builds -DSHORTWAVE_DEBUG_HOST="C:/path/to/your/daw.exe"
if (MSVC)
    set(SHORTWAVE_DEBUG_HOST "C:/Program Files/REAPER (x64)/reaper.exe"
        CACHE FILEPATH "DAW that Visual Studio launches when debugging the VST3")
    set(SHORTWAVE_TEST_PROJECT "${CMAKE_CURRENT_SOURCE_DIR}/dev/ShortwaveTest.rpp")

    set_target_properties("${PROJECT_NAME}_VST3" PROPERTIES VS_DEBUGGER_COMMAND "${SHORTWAVE_DEBUG_HOST}")
    if (EXISTS "${SHORTWAVE_TEST_PROJECT}")
        set_target_properties("${PROJECT_NAME}_VST3" PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "\"${SHORTWAVE_TEST_PROJECT}\"")
    endif ()

    # Make the VST3 the default project in the Visual Studio solution
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT "${PROJECT_NAME}_VST3")
endif ()

# IPP support (Intel SIMD DSP), off for now
# include(PamplejuceIPP)

# Everything related to the tests target
pamplejuce_add_tests()

# A separate target for Benchmarks (keeps the Tests target fast)
pamplejuce_add_benchmarks()

# Output some config for CI (like our PRODUCT_NAME)
include(GitHubENV)
