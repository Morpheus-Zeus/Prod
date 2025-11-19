//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : jendrixtuner_cids.h
// Created by  : Steinberg, 01/2025
// Description : JendrixTuner VST3 Plugin - Class IDs
//
//------------------------------------------------------------------------
#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// Unique plugin identifiers (GUIDs)
// IMPORTANT: These MUST be unique for your plugin! Generate new ones if forking.
//------------------------------------------------------------------------

// Processor UID: {A1B2C3D4-E5F6-4789-ABCD-EF0123456789}
static const FUID kJendrixTunerProcessorUID(0xA1B2C3D4, 0xE5F64789, 0xABCDEF01, 0x23456789);

// Controller UID: {B2C3D4E5-F6A7-4890-BCDE-F01234567890}
static const FUID kJendrixTunerControllerUID(0xB2C3D4E5, 0xF6A74890, 0xBCDEF012, 0x34567890);

//------------------------------------------------------------------------
// Parameter IDs - these identify each controllable parameter
//------------------------------------------------------------------------
enum JendrixTunerParams : Vst::ParamID
{
    // Input parameters (controlled by user)
    kParamBypass = 0,           // Bypass on/off
    kParamKey = 1,              // Musical key (0-11 = C to B)
    kParamScale = 2,            // Scale type (0=Major, 1=Minor, etc.)
    kParamRetuneSpeed = 3,      // How fast to correct (0=instant/robotic, 1=slow/natural)
    kParamHumanize = 4,         // Amount of pitch variation to preserve
    kParamMix = 5,              // Wet/Dry mix (0=dry, 1=100% wet)
    kParamFormantPreserve = 6,  // Formant preservation on/off

    // Output parameters (read-only, for visualization)
    kParamDetectedPitch = 100,  // Detected pitch in Hz (output only)
    kParamDetectedNote = 101,   // Detected MIDI note (output only)
    kParamTargetPitch = 102,    // Target pitch in Hz after quantization (output only)
    kParamTargetNote = 103,     // Target MIDI note after quantization (output only)
    kParamCentsCorrection = 104 // Pitch correction needed in cents (output only)
};

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
