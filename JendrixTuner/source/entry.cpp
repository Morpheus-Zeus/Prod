//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : entry.cpp
// Created by  : Steinberg, 01/2025
// Description : Plugin Factory Entry Point
//
//------------------------------------------------------------------------

#include "jendrixtuner_processor.h"
#include "jendrixtuner_controller.h"
#include "jendrixtuner_cids.h"
#include "version.h"

#include "public.sdk/source/main/pluginfactory.h"

#define stringPluginName "JendrixTuner"

//------------------------------------------------------------------------
// VST3 Plugin Factory
// This macro creates the plugin entry point that the host will call
//------------------------------------------------------------------------
BEGIN_FACTORY_DEF(stringCompanyName,
                  stringCompanyName,
                  "mailto:your@email.com")

    //--- Register Processor (Audio Engine) ---
    DEF_CLASS2(INLINE_UID_FROM_FUID(Steinberg::JendrixTuner::kJendrixTunerProcessorUID),
               PClassInfo::kManyInstances,
               kVstAudioEffectClass,
               stringPluginName,
               Vst::kDistributable,
               Vst::PlugType::kFx,
               FULL_VERSION_STR,
               kVstVersionString,
               Steinberg::JendrixTuner::JendrixTunerProcessor::createInstance)

    //--- Register Controller (Parameters & UI) ---
    DEF_CLASS2(INLINE_UID_FROM_FUID(Steinberg::JendrixTuner::kJendrixTunerControllerUID),
               PClassInfo::kManyInstances,
               kVstComponentControllerClass,
               stringPluginName "Controller",
               0,
               "",
               FULL_VERSION_STR,
               kVstVersionString,
               Steinberg::JendrixTuner::JendrixTunerController::createInstance)

END_FACTORY
