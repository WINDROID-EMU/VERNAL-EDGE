# Vernal Edge - Android Port & C Engine Runner

A lightweight, standalone C engine runner and Android port for **Vernal Edge** (built originally with GameMaker YYC).

This project implements an open-source, high-performance clean-room GameMaker YYC runtime layer directly in C99, targeting modern Android devices via **Android NDK**, **NativeActivity**, **OpenGL ES 3.0**, and **AAudio / OpenSL ES**.

---

## ✨ Features

- **Pure C99 Engine Core (`runner/`)**:
  - **Dynamic Typing Runtime**: Full `RValue` implementation supporting reals, strings, arrays, structs, pointers, and undefined values with refcounting.
  - **Data Structures (`ds_*`)**: GameMaker standard lists, maps, grids, and stacks.
  - **Asset Parser (`data_win.c`)**: Native parser for GameMaker `data.win` chunks (FORM, GEN8, OPTN, LANG, EXTN, SOND, AGRP, SPRT, BGND, PATH, SCPT, GLOB, SHDR, FONT, TMLN, OBJT, ROOM, DAUD, TPAG, CODE, VARI, FUNC, STRG, TXTR, AUDO).
  - **Hardware Accelerated Renderer (`graphics.c` & `room.c`)**:
    - OpenGL ES 3.0 batch sprite rendering with texture atlas clipping.
    - Full layer tilemap, asset, and instance drawing with depth ordering.
    - Dynamic viewport camera tracking and aspect-ratio letterboxing/pillarboxing.
    - Room background color clearing and multi-background parallax scrolling.
  - **Audio Pipeline (`audio.c`)**:
    - Native embedded `stb_vorbis` audio stream decoder.
    - Low-latency multi-channel sound effect and background music playback.
  - **Mobile Touch Controls & Virtual Gamepad (`input.c`)**:
    - Floating dynamic analog thumbstick on the left side with visual boundary and knob indicators.
    - Dedicated action buttons on the right side: Jump (A), Attack (X), Parry (B), Pulse (Y), Cast (R1), and Pause (Menu).
    - Intelligent title screen tap-to-select and navigation support.
    - Automatic visibility toggle between menu navigation and in-game exploration.

---

## 📁 Repository Structure

```
.
├── android/
│   ├── app/
│   │   ├── build.gradle
│   │   └── src/main/
│   │       ├── AndroidManifest.xml      # Fullscreen NativeActivity manifest
│   │       ├── cpp/
│   │       │   ├── android_main.c       # NativeActivity entry point & touch handler
│   │       │   └── CMakeLists.txt       # NDK ARM64 build configuration
│   │       └── res/                     # App launcher icons & resources
│   └── build.gradle
├── runner/
│   ├── include/
│   │   ├── runner.h                     # Main runner header & public API
│   │   ├── graphics.h                   # GLES3 batch renderer & shader pipeline
│   │   ├── room.h                       # Room loader & instance manager
│   │   ├── input.h                      # Touch & virtual controller system
│   │   ├── audio.h                      # Audio mixer & Vorbis loader
│   │   ├── data_win.h                   # IFF data.win chunk parser
│   │   ├── ds.h                         # Data structures (ds_map, ds_list)
│   │   ├── rvalue.h                     # Dynamic GML type system
│   │   └── gml_types.h                  # Object and instance representations
│   └── src/
│       ├── main_loop.c                  # Fixed 60 FPS tick loop & event dispatch
│       ├── graphics.c                   # Vertex buffer batching & drawing
│       ├── room.c                       # Room scene graph & layer rendering
│       ├── input.c                      # Touch inputs, floating stick & D-Pad
│       ├── audio.c                      # Audio engine
│       ├── data_win.c                   # Resource loading
│       └── bzip2/                       # Embedded decompression library
├── tools/
│   ├── build_apk.sh                     # Automated build, align, and sign script
│   ├── ghidra_scripts/                  # Reverse engineering analysis scripts
│   └── *.py                             # Dispatcher and global table generators
└── README.md
```

---

## 🛠️ Prerequisites & Building

### Requirements
- **Linux** (Debian/Ubuntu/Arch/Fedora)
- **Android NDK** (version 26+ / r26b or newer)
- **Android SDK** (Build-tools 34.0.0+ / 35.0.0)
- **CMake** (version 3.22.1+)
- **JDK 17+**

### Building the Android APK

1. Configure your Android SDK and NDK paths in `tools/build_apk.sh` or export them in your shell:
   ```bash
   export ANDROID_HOME="/path/to/Android/Sdk"
   export ANDROID_NDK_HOME="${ANDROID_HOME}/ndk/26.1.10909125"
   ```

2. Run the automated build script:
   ```bash
   ./tools/build_apk.sh
   ```

3. The signed APK will be created at:
   ```
   android/VernalEdge-arm64.apk
   ```

### Installing on Device

Deploy to a connected ARM64 device via ADB:
```bash
adb install -r android/VernalEdge-arm64.apk
```

---

## 🎮 Game Assets Setup

This repository contains **only the open-source engine code, tools, and build infrastructure**. No copyrighted game assets, audio files, or proprietary binaries are included.

To run the game:
1. Provide the legitimate `data.win` file and audio `.ogg` files from your owned copy of **Vernal Edge** (GOG or Steam).
2. Place `data.win` and the audio files in the application's external storage directory on your device:
   ```bash
   adb shell mkdir -p /sdcard/VernalEdge/
   adb push data.win /sdcard/VernalEdge/
   adb push *.ogg /sdcard/VernalEdge/
   ```

---

## 🕹️ Controls

- **Left Screen Area**: Dynamic floating thumbstick (tap and drag to move).
- **Right Screen Area**:
  - `A`: Jump / Confirm
  - `X`: Attack
  - `B`: Parry / Cancel
  - `Y`: Pulse / Special
  - `R`: Cast / Magic
- **Top Right Corner**: Pause / In-Game Menu
- **Menus / Title Screen**: Direct touch selection enabled.

---

## 📜 License & Disclaimer

This project is a clean-room reverse-engineered port created for educational, research, and personal preservation purposes. All original game assets, art, audio, and intellectual property belong to their respective copyright holders (**Hello Penguin Games**).
