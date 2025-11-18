cmake_minimum_required(VERSION 3.15)

#-------------------------------------------------------------------------------
# JendrixTuner VST3 Plugin
#-------------------------------------------------------------------------------
project(JendrixTuner VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-------------------------------------------------------------------------------
# VST3 SDK Path
# Adjust this if your SDK is in a different location
#-------------------------------------------------------------------------------
set(vst3sdk_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vst3sdk")
if(NOT EXISTS "${vst3sdk_SOURCE_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "VST3 SDK not found at: ${vst3sdk_SOURCE_DIR}\n"
                        "Please download it from https://github.com/steinbergmedia/vst3sdk")
endif()

#-------------------------------------------------------------------------------
# Add VST3 SDK
#-------------------------------------------------------------------------------
add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

#-------------------------------------------------------------------------------
# Plugin Sources
#-------------------------------------------------------------------------------
set(PLUGIN_SOURCES
    source/jendrixtuner_processor.cpp
    source/jendrixtuner_processor.h
    source/jendrixtuner_controller.cpp
    source/jendrixtuner_controller.h
    source/jendrixtuner_cids.h
    source/version.h
    source/entry.cpp
)

# DSP Algorithm Headers (header-only)
set(PLUGIN_HEADERS
    include/yin_pitch_detector.h
    include/circular_buffer.h
)

#-------------------------------------------------------------------------------
# Create the VST3 plugin target
#-------------------------------------------------------------------------------
smtg_add_vst3plugin(JendrixTuner ${PLUGIN_SOURCES} ${PLUGIN_HEADERS})

#-------------------------------------------------------------------------------
# Set plugin properties
#-------------------------------------------------------------------------------
smtg_target_set_plugin_name(JendrixTuner "JendrixTuner")

target_compile_features(JendrixTuner PUBLIC cxx_std_17)

# Add include directory for our DSP headers
target_include_directories(JendrixTuner
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JendrixTuner
    PRIVATE
        sdk
)

#-------------------------------------------------------------------------------
# Windows-specific settings
#-------------------------------------------------------------------------------
if(MSVC)
    # Enable multi-processor compilation
    target_compile_options(JendrixTuner PRIVATE /MP)

    # Disable specific warnings if needed
    # target_compile_options(JendrixTuner PRIVATE /wd4244 /wd4267)
endif()

#-------------------------------------------------------------------------------
# Output message
#-------------------------------------------------------------------------------
message(STATUS "JendrixTuner VST3 plugin configured")
message(STATUS "VST3 SDK path: ${vst3sdk_SOURCE_DIR}")
