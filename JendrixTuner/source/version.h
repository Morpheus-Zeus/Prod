//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : JendrixTuner
// Filename    : version.h
// Created by  : Steinberg, 01/2025
// Description : Version information
//
//------------------------------------------------------------------------
#pragma once

#define MAJOR_VERSION_STR "1"
#define MAJOR_VERSION_INT 1

#define SUB_VERSION_STR "0"
#define SUB_VERSION_INT 0

#define RELEASE_NUMBER_STR "0"
#define RELEASE_NUMBER_INT 0

#define BUILD_NUMBER_STR "1"
#define BUILD_NUMBER_INT 1

// Version format: 1.0.0.1
#define FULL_VERSION_STR MAJOR_VERSION_STR "." SUB_VERSION_STR "." RELEASE_NUMBER_STR "." BUILD_NUMBER_STR

// Plugin name and vendor
#define stringPluginName "JendrixTuner"
#define stringOriginalFilename "JendrixTuner.vst3"
#define stringFileDescription "JendrixTuner VST3 Pitch Correction Plugin"
#define stringCompanyName "Your Company\0"
#define stringLegalCopyright "Copyright(c) 2025 Your Company"
#define stringLegalTrademarks "VST is a trademark of Steinberg Media Technologies GmbH"
