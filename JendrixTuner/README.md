# JendrixTuner VST3 Plugin

A real-time vocal pitch correction plugin for Windows, built with the Steinberg VST3 SDK.

## Features

- **Real-time pitch correction** for monophonic vocals
- **Key & Scale selection** (12 keys, Major/Minor scales)
- **Retune Speed control** (instant robotic to slow natural)
- **Humanize control** (preserve natural pitch variations)
- **Formant Preservation** (simple implementation)
- **Wet/Dry Mix** control
- **Low latency** suitable for live monitoring in Ableton Live

## Current Status

✅ **Phase 1 Complete**: Pass-through plugin with parameters
- Audio passes through cleanly
- All parameters defined and automatable
- State saving/loading works

✅ **Phase 2 Complete**: YIN pitch detection
- Real-time monophonic pitch detection
- 60-1000 Hz range (covers vocals)
- ~46ms latency @ 44.1kHz
- Output parameters show detected pitch/note
- See `PHASE2_PITCH_DETECTION.md` for details

🚧 **Next Phases**:
- Phase 3: Note quantization to scale
- Phase 4: Pitch shifting algorithm
- Phase 5: Retune speed & humanization

---

## 📋 Prerequisites

### Required Software

1. **Windows 10/11** (64-bit)
2. **Visual Studio 2019 or 2022** (Community Edition is free)
   - Download: https://visualstudio.microsoft.com/downloads/
   - Required workload: "Desktop development with C++"
3. **CMake 3.15 or later**
   - Download: https://cmake.org/download/
   - ✅ During install, choose "Add CMake to system PATH"
4. **Git** (for cloning VST3 SDK)
   - Download: https://git-scm.com/download/win

---

## 🚀 Build Instructions (Windows)

### Step 1: Verify Prerequisites

Open **Command Prompt** or **PowerShell** and verify installations:

```cmd
cmake --version
git --version
```

Both should print version numbers without errors.

### Step 2: Project Structure

Your directory should look like this:

```
Prod/
├── vst3sdk/              ← VST3 SDK (already downloaded)
└── JendrixTuner/         ← Plugin source code
    ├── source/
    │   ├── jendrixtuner_processor.cpp
    │   ├── jendrixtuner_processor.h
    │   ├── jendrixtuner_controller.cpp
    │   ├── jendrixtuner_controller.h
    │   ├── jendrixtuner_cids.h
    │   ├── version.h
    │   └── entry.cpp
    ├── CMakeLists.txt
    └── README.md (this file)
```

### Step 3: Generate Visual Studio Solution

Open **Command Prompt** or **PowerShell** and navigate to your project:

```cmd
cd C:\path\to\Prod\JendrixTuner
```

Create a build directory and run CMake:

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
```

**Note**: If you have Visual Studio 2019, use:
```cmd
cmake .. -G "Visual Studio 16 2019" -A x64
```

✅ CMake will generate a `JendrixTuner.sln` file in the `build` directory.

### Step 4: Build the Plugin

**Option A: Build from Command Line**

```cmd
cmake --build . --config Release
```

**Option B: Build from Visual Studio**

1. Open `build/JendrixTuner.sln` in Visual Studio
2. In the top toolbar, select **Release** and **x64**
3. Right-click **JendrixTuner** project → **Build**

### Step 5: Locate the Compiled Plugin

After building, the `.vst3` file will be at:

```
Prod/JendrixTuner/build/VST3/Release/JendrixTuner.vst3/
```

This is a **bundle directory** (VST3 plugins are folders on Windows, not single files).

---

## 🎵 Install in Ableton Live 11

### Method 1: Use Default VST3 Path (Recommended)

1. Copy the entire `JendrixTuner.vst3` folder to:
   ```
   C:\Program Files\Common Files\VST3\
   ```

2. Restart Ableton Live 11

3. In Ableton:
   - Create an audio track
   - Click **Audio Effects** → **Plug-ins** → **JendrixTuner**

### Method 2: Custom VST3 Path

1. In Ableton, go to **Options** → **Preferences** → **Plug-Ins**
2. Check **Use VST3 Plug-In System Folders** or add a custom path
3. Click **Rescan** after copying the plugin

---

## 🧪 Testing the Plugin

### Basic Test (Phase 1 - Current)

1. Load JendrixTuner on an audio track in Ableton
2. Play audio or sing into a microphone
3. **Expected behavior**: Audio passes through unchanged
4. Toggle parameters in the Ableton plugin UI:
   - Key (C, C#, D, etc.)
   - Scale (Major/Minor)
   - Retune Speed (0-100%)
   - Humanize (0-100%)
   - Mix (0-100%)
5. Parameters should be automatable (right-click → "Show Automation")

### Debug Build (For Development)

To build a debug version with symbols:

```cmd
cmake --build . --config Debug
```

Debug plugin location: `build/VST3/Debug/JendrixTuner.vst3/`

---

## 📝 Development Workflow

### Rebuild After Code Changes

```cmd
cd Prod/JendrixTuner/build
cmake --build . --config Release
```

### Clean Rebuild

```cmd
cd Prod/JendrixTuner
rmdir /s /q build
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

---

## 🔧 Troubleshooting

### "VST3 SDK not found" Error

- Verify `vst3sdk` folder exists at `Prod/vst3sdk/`
- Check `vst3sdk/CMakeLists.txt` exists
- Re-clone if needed:
  ```cmd
  cd Prod
  git clone --recursive https://github.com/steinbergmedia/vst3sdk.git
  ```

### "Cannot find module" in Visual Studio

- Make sure you selected **Release** or **Debug** configuration
- Right-click project → **Retarget Solution** (updates Windows SDK version)

### Plugin Doesn't Appear in Ableton

- Verify you copied the entire `.vst3` **folder**, not individual files
- Check Ableton's VST3 path in Preferences
- Run Ableton as Administrator once after installing plugin
- Check Ableton's Log.txt for plugin loading errors:
  ```
  C:\Users\YourName\AppData\Roaming\Ableton\Live 11.x\Preferences\Log.txt
  ```

### "Access Denied" When Copying to Program Files

- Run Command Prompt as Administrator
- Or copy to user VST3 folder instead:
  ```
  C:\Users\YourName\AppData\Local\Programs\Common\VST3\
  ```

---

## 📚 Next Steps

Once the basic plugin is working, we'll add:

1. **YIN Pitch Detection** - Detect the current pitch of incoming audio
2. **Scale Quantization** - Map detected pitch to nearest scale note
3. **Pitch Shifting** - Shift audio to corrected pitch
4. **Smoothing & Humanization** - Natural-sounding corrections

Each phase will be added incrementally while keeping the plugin functional.

---

## 📄 License

This plugin uses the Steinberg VST3 SDK, licensed under the GPLv3 or Steinberg VST3 License.

---

## 🆘 Support

If you encounter issues:

1. Check the VST3 SDK documentation: https://steinbergmedia.github.io/vst3_doc/
2. Review CMake output for errors
3. Check Visual Studio build output window for compilation errors

---

**Current Version**: 1.0.0
**Last Updated**: January 2025
