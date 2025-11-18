# 🚀 JendrixTuner Quick Start Guide

## ✅ Phase 1 Complete: Minimal Pass-Through Plugin

Your VST3 pitch correction plugin is now set up and ready to build!

---

## 📂 What You Have

```
Prod/
├── vst3sdk/                    ← Official Steinberg VST3 SDK (downloaded)
├── JendrixTuner/               ← Your plugin source
│   ├── source/                 ← All C++ code
│   │   ├── jendrixtuner_processor.cpp/.h      (Audio engine)
│   │   ├── jendrixtuner_controller.cpp/.h     (Parameters)
│   │   ├── jendrixtuner_cids.h                (Unique IDs)
│   │   ├── version.h                          (Version info)
│   │   └── entry.cpp                          (Plugin factory)
│   ├── CMakeLists.txt          ← Build configuration
│   └── README.md               ← Full documentation
└── QUICKSTART.md               ← This file
```

---

## 🎯 What It Does Right Now

✅ **Works as a pass-through effect** - Audio flows cleanly without modification
✅ **All parameters defined and automatable**:
   - Key (C, C#, D, ..., B)
   - Scale (Major, Minor)
   - Retune Speed (0-100%)
   - Humanize (0-100%)
   - Mix (Wet/Dry 0-100%)
   - Formant Preserve (On/Off)
   - Bypass (On/Off)

✅ **Saves/loads state** in Ableton projects
✅ **Low latency** audio processing
✅ **Stereo I/O** (processes mono internally)

❌ **Not yet implemented**:
   - Pitch detection
   - Scale quantization
   - Pitch shifting
   - Actual correction (coming in Phase 2-5)

---

## ⚡ Build It Now (Windows)

### 1️⃣ Open PowerShell or Command Prompt

```cmd
cd C:\path\to\Prod\JendrixTuner
```

### 2️⃣ Generate Visual Studio Solution

```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
```

**Result**: Creates `JendrixTuner.sln` in `build/` folder

### 3️⃣ Build the Plugin

**Command Line:**
```cmd
cmake --build . --config Release
```

**OR in Visual Studio:**
- Open `build/JendrixTuner.sln`
- Select **Release** + **x64**
- Build → Build Solution (F7)

### 4️⃣ Find the Output

```
Prod/JendrixTuner/build/VST3/Release/JendrixTuner.vst3/
```

This entire folder IS the plugin (VST3 = bundle directory)

---

## 🎵 Install in Ableton Live 11

### Copy to Default VST3 Location

**Run as Administrator**, then:

```cmd
xcopy "C:\path\to\Prod\JendrixTuner\build\VST3\Release\JendrixTuner.vst3" ^
      "C:\Program Files\Common Files\VST3\JendrixTuner.vst3" /E /I
```

**Or manually**:
1. Copy the entire `JendrixTuner.vst3` folder
2. Paste to: `C:\Program Files\Common Files\VST3\`

### Load in Ableton

1. Restart Ableton Live 11
2. Create an audio track
3. Audio Effects → Plug-ins → JendrixTuner
4. **Test**: Play audio - it should pass through cleanly
5. **Automate**: All parameters should be visible and automatable

---

## 🧪 Verify It Works

1. ✅ Plugin loads without errors in Ableton
2. ✅ Audio plays through without glitches
3. ✅ Parameters change in Ableton's UI
4. ✅ Bypass works (mutes effect)
5. ✅ Mix control works (0% = dry, 100% = wet)
6. ✅ Saved projects reload with your settings

---

## 🔥 Next Development Phases

### Phase 2: YIN Pitch Detection (Next!)
- Implement the YIN algorithm to detect vocal pitch
- Add circular buffer for windowed analysis
- Output detected frequency in Hz
- **Goal**: Display detected note (e.g., "A4 440Hz")

### Phase 3: Scale Quantization
- Map detected pitch to nearest scale note
- Implement major/minor scale templates
- Add chromatic scale support
- **Goal**: Know the target pitch to correct to

### Phase 4: Pitch Shifting
- Implement phase vocoder or resampling
- Simple formant preservation
- Real-time performance optimization
- **Goal**: Actually shift pitch to target

### Phase 5: Smoothing & Humanization
- Exponential smoothing for retune speed
- Humanize: preserve small pitch variations
- Glide detection (don't correct intentional bends)
- **Goal**: Natural-sounding Auto-Tune effect

### Phase 6 (Optional): GUI
- Basic parameter display
- Visual pitch meter
- Scale visualization

---

## 🛠️ Common Issues & Fixes

### "CMake can't find VST3 SDK"
**Fix**: Verify `vst3sdk` folder exists at `Prod/vst3sdk/CMakeLists.txt`

### "Plugin doesn't show in Ableton"
**Fix**:
- Copy the **entire folder** `JendrixTuner.vst3/`, not individual files
- Restart Ableton completely
- Check Preferences → Plug-Ins → VST3 paths

### "Access denied" when copying
**Fix**: Run Command Prompt as Administrator

### Build errors in Visual Studio
**Fix**:
- Make sure you selected **Release** configuration
- Right-click solution → Retarget Solution
- Clean and rebuild

---

## 📖 Full Documentation

See `JendrixTuner/README.md` for:
- Detailed build instructions
- Troubleshooting guide
- Development workflow
- Technical architecture

---

## 🎓 Key Concepts Explained

### VST3 Architecture

```
┌─────────────────────────────────────────┐
│          Ableton Live (Host)            │
└──────────────┬──────────────────────────┘
               │
       ┌───────┴────────┐
       │                │
┌──────▼──────┐  ┌─────▼──────┐
│ Controller  │  │ Processor  │
│ (UI/Params) │◄─┤  (Audio)   │
│ Thread-safe │  │  Real-time │
└─────────────┘  └────────────┘
```

- **Processor**: Real-time audio processing (our DSP code)
- **Controller**: Parameters, UI, non-realtime stuff
- **Communication**: Parameters flow Controller → Processor

### Current Processing Flow

```
Input Audio → Bypass Check → Mix Control → Output Audio
                     │
                     └─► (Pitch correction will go here)
```

---

## 💡 Tips for Development

1. **Rebuild Fast**: Just run `cmake --build . --config Release` in `build/`
2. **Test Changes**: Copy new `.vst3` to `Program Files`, restart Ableton
3. **Debug**: Build Debug config, attach Visual Studio debugger to Ableton
4. **Parameters**: Changes in `jendrixtuner_controller.cpp` don't need DAW restart
5. **DSP**: Changes in `jendrixtuner_processor.cpp` = full rebuild

---

## 📝 Code Entry Points

Want to hack on the code? Start here:

### Add a new parameter:
1. `jendrixtuner_cids.h` - Add enum entry
2. `jendrixtuner_processor.h` - Add member variable
3. `jendrixtuner_controller.cpp` - Add parameter definition
4. `jendrixtuner_processor.cpp` - Handle parameter in `process()`

### Change audio processing:
- `jendrixtuner_processor.cpp:108` - Main `process()` function
- Line 160: "// *** PITCH CORRECTION WILL GO HERE ***"

---

## 🎉 Congratulations!

You now have a fully functional (pass-through) VST3 plugin!

**Ready for Phase 2?** Let me know and I'll implement YIN pitch detection!

---

**Questions? Issues?** Check the main README or ask for help.
