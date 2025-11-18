//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : jendrixtuner_processor.cpp
// Created by  : Steinberg, 01/2025
// Description : JendrixTuner Audio Processor Implementation
//
//------------------------------------------------------------------------

#include "jendrixtuner_processor.h"
#include "jendrixtuner_cids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include <cmath>

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------
JendrixTunerProcessor::JendrixTunerProcessor()
{
    // Register the controller class ID (for communication with UI)
    setControllerClass(kJendrixTunerControllerUID);
}

//------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------
JendrixTunerProcessor::~JendrixTunerProcessor()
{
}

//------------------------------------------------------------------------
// Initialize - Called once when plugin is loaded
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::initialize(FUnknown* context)
{
    // Always call parent initialize first
    tresult result = AudioEffect::initialize(context);
    if (result != kResultOk)
        return result;

    //--- Create Audio IO buses ---
    // We want: 1 input bus (stereo) and 1 output bus (stereo)
    // Even though we process mono internally, stereo IO is standard
    addAudioInput(STR16("Stereo In"), Vst::SpeakerArr::kStereo);
    addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);

    return kResultOk;
}

//------------------------------------------------------------------------
// Terminate - Called when plugin is unloaded
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::terminate()
{
    // Release any resources here
    return AudioEffect::terminate();
}

//------------------------------------------------------------------------
// setActive - Called when plugin is turned on/off
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::setActive(TBool state)
{
    if (state) // Plugin activated
    {
        // Initialize DSP here (e.g., clear buffers, reset state)
    }
    else // Plugin deactivated
    {
        // Clean up DSP resources
    }
    return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
// setupProcessing - Called before processing starts, gives us sample rate
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::setupProcessing(Vst::ProcessSetup& newSetup)
{
    // Store sample rate for DSP calculations
    mSampleRate = newSetup.sampleRate;

    return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
// canProcessSampleSize - Tell the host which sample formats we support
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::canProcessSampleSize(int32 symbolicSampleSize)
{
    // We support 32-bit float processing
    if (symbolicSampleSize == Vst::kSample32)
        return kResultTrue;

    // We could also support 64-bit: Vst::kSample64
    return kResultFalse;
}

//------------------------------------------------------------------------
// process - THE MAIN DSP FUNCTION - called for every audio buffer
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::process(Vst::ProcessData& data)
{
    //--- 1. Read parameter changes from host (automation, UI changes) ---
    if (data.inputParameterChanges)
    {
        int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        for (int32 i = 0; i < numParamsChanged; i++)
        {
            Vst::IParamValueQueue* paramQueue =
                data.inputParameterChanges->getParameterData(i);

            if (paramQueue)
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints = paramQueue->getPointCount();

                // Get the last value in this buffer
                if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) == kResultTrue)
                {
                    switch (paramQueue->getParameterId())
                    {
                        case kParamBypass:
                            mBypass = (value > 0.5);
                            break;
                        case kParamKey:
                            mKey = (int32)(value * 11.0); // 0-11
                            break;
                        case kParamScale:
                            mScale = (int32)(value * 1.0); // 0=Major, 1=Minor
                            break;
                        case kParamRetuneSpeed:
                            mRetuneSpeed = value;
                            break;
                        case kParamHumanize:
                            mHumanize = value;
                            break;
                        case kParamMix:
                            mMix = value;
                            break;
                        case kParamFormantPreserve:
                            mFormantPreserve = (value > 0.5);
                            break;
                    }
                }
            }
        }
    }

    //--- 2. Process audio ---
    if (data.numInputs == 0 || data.numOutputs == 0)
    {
        // No audio to process
        return kResultOk;
    }

    // Get input and output buffers
    Vst::AudioBusBuffers& inputBus = data.inputs[0];
    Vst::AudioBusBuffers& outputBus = data.outputs[0];

    int32 numChannels = inputBus.numChannels;
    int32 numSamples = data.numSamples;

    // For now: PASS-THROUGH mode (copy input to output)
    // We'll add pitch correction in the next phase

    if (mBypass)
    {
        // If bypassed, just copy input to output
        for (int32 channel = 0; channel < numChannels; channel++)
        {
            float* inputChannel = inputBus.channelBuffers32[channel];
            float* outputChannel = outputBus.channelBuffers32[channel];

            // Simple copy
            for (int32 sample = 0; sample < numSamples; sample++)
            {
                outputChannel[sample] = inputChannel[sample];
            }
        }
    }
    else
    {
        // Process each channel
        for (int32 channel = 0; channel < numChannels; channel++)
        {
            float* inputChannel = inputBus.channelBuffers32[channel];
            float* outputChannel = outputBus.channelBuffers32[channel];

            // For now: just pass through
            // TODO: Add pitch detection and correction here
            for (int32 sample = 0; sample < numSamples; sample++)
            {
                float inputSample = inputChannel[sample];

                // *** PITCH CORRECTION WILL GO HERE ***
                float correctedSample = inputSample; // Placeholder

                // Apply wet/dry mix
                float outputSample = inputSample * (1.0f - (float)mMix) +
                                   correctedSample * (float)mMix;

                outputChannel[sample] = outputSample;
            }
        }
    }

    return kResultOk;
}

//------------------------------------------------------------------------
// setState - Load plugin state from DAW project
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::setState(IBStream* state)
{
    if (!state)
        return kResultFalse;

    // Read parameters from stream
    IBStreamer streamer(state, kLittleEndian);

    int32 savedBypass = 0;
    if (streamer.readInt32(savedBypass) == false)
        return kResultFalse;
    mBypass = savedBypass > 0;

    if (streamer.readInt32(mKey) == false)
        return kResultFalse;
    if (streamer.readInt32(mScale) == false)
        return kResultFalse;
    if (streamer.readDouble(mRetuneSpeed) == false)
        return kResultFalse;
    if (streamer.readDouble(mHumanize) == false)
        return kResultFalse;
    if (streamer.readDouble(mMix) == false)
        return kResultFalse;

    int32 savedFormant = 0;
    if (streamer.readInt32(savedFormant) == false)
        return kResultFalse;
    mFormantPreserve = savedFormant > 0;

    return kResultOk;
}

//------------------------------------------------------------------------
// getState - Save plugin state to DAW project
//------------------------------------------------------------------------
tresult PLUGIN_API JendrixTunerProcessor::getState(IBStream* state)
{
    if (!state)
        return kResultFalse;

    // Write parameters to stream
    IBStreamer streamer(state, kLittleEndian);

    streamer.writeInt32(mBypass ? 1 : 0);
    streamer.writeInt32(mKey);
    streamer.writeInt32(mScale);
    streamer.writeDouble(mRetuneSpeed);
    streamer.writeDouble(mHumanize);
    streamer.writeDouble(mMix);
    streamer.writeInt32(mFormantPreserve ? 1 : 0);

    return kResultOk;
}

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
