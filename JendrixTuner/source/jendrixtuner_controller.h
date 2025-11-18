//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : jendrixtuner_controller.h
// Created by  : Steinberg, 01/2025
// Description : JendrixTuner Parameter Controller
//
//------------------------------------------------------------------------
#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// JendrixTunerController - Handles parameter definitions and UI
// Separate from audio processor for thread safety
//------------------------------------------------------------------------
class JendrixTunerController : public Vst::EditControllerEx1
{
public:
    JendrixTunerController();
    ~JendrixTunerController() SMTG_OVERRIDE;

    // Create function used by factory
    static FUnknown* createInstance(void* /*context*/)
    {
        return (Vst::IEditController*)new JendrixTunerController;
    }

    //--- EditController overrides ----------------------------------------
    tresult PLUGIN_API initialize(FUnknown* context) SMTG_OVERRIDE;
    tresult PLUGIN_API terminate() SMTG_OVERRIDE;

    // State save/load
    tresult PLUGIN_API setComponentState(IBStream* state) SMTG_OVERRIDE;

    //--- EditControllerEx1 overrides -------------------------------------
    // No custom implementations needed for now
};

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
