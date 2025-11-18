//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : jendrixtuner_processor.h
// Created by  : Steinberg, 01/2025
// Description : JendrixTuner Audio Processor
//
//------------------------------------------------------------------------
#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// JendrixTunerProcessor - The main audio processing class
// This is where all DSP (Digital Signal Processing) happens
//------------------------------------------------------------------------
class JendrixTunerProcessor : public Vst::AudioEffect
{
public:
    JendrixTunerProcessor();
    ~JendrixTunerProcessor() SMTG_OVERRIDE;

    // Create function used by factory
    static FUnknown* createInstance(void* /*context*/)
    {
        return (Vst::IAudioProcessor*)new JendrixTunerProcessor;
    }

    //--- AudioEffect overrides -------------------------------------------
    tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
    tresult PLUGIN_API terminate() SMTG_OVERRIDE;
    tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;

    // Main processing function - called for each audio buffer
    tresult PLUGIN_API process(Vst::ProcessData& data) SMTG_OVERRIDE;

    // Query processing setup capabilities
    tresult PLUGIN_API setupProcessing(Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
    tresult PLUGIN_API canProcessSampleSize(int32 symbolicSampleSize) SMTG_OVERRIDE;

    // State save/load for DAW projects
    tresult PLUGIN_API setState(IBStream* state) SMTG_OVERRIDE;
    tresult PLUGIN_API getState(IBStream* state) SMTG_OVERRIDE;

protected:
    //--- Parameters (these will be controlled by the user) ---------------
    bool mBypass = false;
    int32 mKey = 0;                  // 0=C, 1=C#, 2=D, etc.
    int32 mScale = 0;                // 0=Major, 1=Minor
    double mRetuneSpeed = 0.5;       // 0.0 to 1.0
    double mHumanize = 0.0;          // 0.0 to 1.0
    double mMix = 1.0;               // 0.0 to 1.0
    bool mFormantPreserve = false;

    double mSampleRate = 44100.0;    // Current sample rate
};

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
