//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : jendrixtuner_controller.cpp
// Created by  : Steinberg, 01/2025
// Description : JendrixTuner Parameter Controller Implementation
//
//------------------------------------------------------------------------

#include "jendrixtuner_controller.h"
#include "jendrixtuner_cids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
JendrixTunerController::JendrixTunerController()
{
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
JendrixTunerController::~JendrixTunerController()
{
}

//------------------------------------------------------------------------
// initialize - Register all parameters here
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerController::initialize(FUnknown* context)
{
    tresult result = EditControllerEx1::initialize(context);
    if (result != kResultOk)
        return result;

    //--- Define Parameters ---
    // Each parameter needs: ID, title, units, default, flags

    // BYPASS - Standard on/off parameter
    parameters.addParameter(STR16("Bypass"),
                           nullptr,
                           1,  // 1 step (on/off)
                           0,  // default off
                           Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsBypass,
                           kParamBypass);

    // KEY - Musical key selection (C, C#, D, D#, E, F, F#, G, G#, A, A#, B)
    Vst::StringListParameter* keyParam = new Vst::StringListParameter(
        STR16("Key"), kParamKey, nullptr,
        Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsList);

    keyParam->appendString(STR16("C"));
    keyParam->appendString(STR16("C#"));
    keyParam->appendString(STR16("D"));
    keyParam->appendString(STR16("D#"));
    keyParam->appendString(STR16("E"));
    keyParam->appendString(STR16("F"));
    keyParam->appendString(STR16("F#"));
    keyParam->appendString(STR16("G"));
    keyParam->appendString(STR16("G#"));
    keyParam->appendString(STR16("A"));
    keyParam->appendString(STR16("A#"));
    keyParam->appendString(STR16("B"));
    parameters.addParameter(keyParam);

    // SCALE - Major or Minor
    Vst::StringListParameter* scaleParam = new Vst::StringListParameter(
        STR16("Scale"), kParamScale, nullptr,
        Vst::ParameterInfo::kCanAutomate | Vst::ParameterInfo::kIsList);

    scaleParam->appendString(STR16("Major"));
    scaleParam->appendString(STR16("Minor"));
    parameters.addParameter(scaleParam);

    // RETUNE SPEED - How fast to correct pitch (0 = instant, 1 = slow/natural)
    parameters.addParameter(STR16("Retune Speed"),
                           STR16("%"),
                           0,     // continuous
                           0.5,   // default 50%
                           Vst::ParameterInfo::kCanAutomate,
                           kParamRetuneSpeed);

    // HUMANIZE - Amount of natural pitch variation to preserve
    parameters.addParameter(STR16("Humanize"),
                           STR16("%"),
                           0,
                           0.0,   // default 0% (full correction)
                           Vst::ParameterInfo::kCanAutomate,
                           kParamHumanize);

    // MIX - Wet/Dry blend
    parameters.addParameter(STR16("Mix"),
                           STR16("%"),
                           0,
                           1.0,   // default 100% wet
                           Vst::ParameterInfo::kCanAutomate,
                           kParamMix);

    // FORMANT PRESERVE - On/off toggle
    parameters.addParameter(STR16("Formant Preserve"),
                           nullptr,
                           1,
                           0,     // default off
                           Vst::ParameterInfo::kCanAutomate,
                           kParamFormantPreserve);

    //--- Output Parameters (Read-only, for visualization) ---

    // DETECTED PITCH - Shows detected frequency in Hz
    parameters.addParameter(STR16("Detected Pitch"),
                           STR16("Hz"),
                           0,     // continuous
                           0.0,   // default 0
                           Vst::ParameterInfo::kIsReadOnly,
                           kParamDetectedPitch);

    // DETECTED NOTE - Shows detected MIDI note number
    parameters.addParameter(STR16("Detected Note"),
                           STR16(""),
                           0,     // continuous
                           0.0,   // default 0
                           Vst::ParameterInfo::kIsReadOnly,
                           kParamDetectedNote);

    return result;
}

//------------------------------------------------------------------------
// terminate
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerController::terminate()
{
    return EditControllerEx1::terminate();
}

//------------------------------------------------------------------------
// setComponentState - Sync parameters with processor state
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerController::setComponentState(IBStream* state)
{
    if (!state)
        return kResultFalse;

    // Read state from processor
    IBStreamer streamer(state, kLittleEndian);

    int32 savedBypass = 0;
    if (streamer.readInt32(savedBypass) == false)
        return kResultFalse;
    setParamNormalized(kParamBypass, savedBypass ? 1 : 0);

    int32 key = 0;
    if (streamer.readInt32(key) == false)
        return kResultFalse;
    setParamNormalized(kParamKey, key / 11.0);

    int32 scale = 0;
    if (streamer.readInt32(scale) == false)
        return kResultFalse;
    setParamNormalized(kParamScale, scale / 1.0);

    double retuneSpeed = 0.5;
    if (streamer.readDouble(retuneSpeed) == false)
        return kResultFalse;
    setParamNormalized(kParamRetuneSpeed, retuneSpeed);

    double humanize = 0.0;
    if (streamer.readDouble(humanize) == false)
        return kResultFalse;
    setParamNormalized(kParamHumanize, humanize);

    double mix = 1.0;
    if (streamer.readDouble(mix) == false)
        return kResultFalse;
    setParamNormalized(kParamMix, mix);

    int32 savedFormant = 0;
    if (streamer.readInt32(savedFormant) == false)
        return kResultFalse;
    setParamNormalized(kParamFormantPreserve, savedFormant ? 1 : 0);

    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
