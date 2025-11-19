//------------------------------------------------------------------------
// Project     : JendrixTuner
// Filename    : scale_quantizer.h
// Created by  : Claude, 01/2025
// Description : Musical scale quantization and pitch correction
//------------------------------------------------------------------------
#pragma once

#include <cmath>
#include <array>
#include <algorithm>

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// ScaleQuantizer - Maps detected pitches to musical scales
//
// Takes a detected pitch and finds the nearest note in a selected
// musical scale (e.g., C Major, A Minor, etc.)
//------------------------------------------------------------------------
class ScaleQuantizer
{
public:
    //--- Scale Types ---
    enum ScaleType
    {
        kScaleMajor = 0,
        kScaleMinor = 1,
        kScaleChromatic = 2  // All 12 notes (no quantization)
    };

    //--- Constructor ---
    ScaleQuantizer()
        : mRootNote(0)  // C
        , mScaleType(kScaleMajor)
    {
    }

    //--- Configuration ---
    void setRootNote(int rootNote)
    {
        // rootNote: 0=C, 1=C#, 2=D, ..., 11=B
        mRootNote = rootNote % 12;
    }

    void setScaleType(ScaleType scaleType)
    {
        mScaleType = scaleType;
    }

    //--- Main Quantization Function ---
    // Input: detected MIDI note (can have fractional part for cents)
    // Output: quantized MIDI note (nearest in-scale note)
    double quantizeNote(double detectedMidiNote) const
    {
        if (mScaleType == kScaleChromatic)
        {
            // No quantization needed - all 12 notes are valid
            return std::round(detectedMidiNote);
        }

        // Get the scale intervals for the current scale type
        const int* intervals = getScaleIntervals(mScaleType);
        int numNotes = getScaleSize(mScaleType);

        // Find the octave and note within octave
        int octave = static_cast<int>(std::floor(detectedMidiNote / 12.0));
        double noteInOctave = detectedMidiNote - (octave * 12.0);

        // Find the nearest in-scale note within this octave
        double nearestNoteInOctave = findNearestInScale(noteInOctave, intervals, numNotes);

        // Reconstruct the full MIDI note
        return (octave * 12.0) + nearestNoteInOctave;
    }

    //--- Quantize from frequency (Hz) ---
    double quantizeFrequency(double frequencyHz) const
    {
        if (frequencyHz <= 0.0)
            return 0.0;

        // Convert frequency to MIDI note
        double midiNote = frequencyToMidi(frequencyHz);

        // Quantize to scale
        double quantizedMidi = quantizeNote(midiNote);

        // Convert back to frequency
        return midiToFrequency(quantizedMidi);
    }

    //--- Calculate pitch correction in cents ---
    // Returns how many cents to shift (positive = sharpen, negative = flatten)
    double calculateCentsCorrection(double detectedMidiNote, double targetMidiNote) const
    {
        // 100 cents = 1 semitone
        return (targetMidiNote - detectedMidiNote) * 100.0;
    }

    //--- Helper: Get scale name ---
    const char* getScaleName() const
    {
        switch (mScaleType)
        {
            case kScaleMajor: return "Major";
            case kScaleMinor: return "Minor";
            case kScaleChromatic: return "Chromatic";
            default: return "Unknown";
        }
    }

    //--- Helper: Get root note name ---
    const char* getRootNoteName() const
    {
        static const char* noteNames[12] = {
            "C", "C#", "D", "D#", "E", "F",
            "F#", "G", "G#", "A", "A#", "B"
        };
        return noteNames[mRootNote];
    }

private:
    //--- Scale Interval Definitions ---
    // These define which notes are in each scale (relative to root)

    // Major scale: Root, M2, M3, P4, P5, M6, M7
    // Example: C Major = C, D, E, F, G, A, B
    static constexpr std::array<int, 7> kMajorScaleIntervals = {0, 2, 4, 5, 7, 9, 11};

    // Minor scale (Natural Minor): Root, M2, m3, P4, P5, m6, m7
    // Example: A Minor = A, B, C, D, E, F, G
    static constexpr std::array<int, 7> kMinorScaleIntervals = {0, 2, 3, 5, 7, 8, 10};

    // Chromatic scale: All 12 notes
    static constexpr std::array<int, 12> kChromaticScaleIntervals = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    //--- Get scale intervals array ---
    const int* getScaleIntervals(ScaleType scaleType) const
    {
        switch (scaleType)
        {
            case kScaleMajor:
                return kMajorScaleIntervals.data();
            case kScaleMinor:
                return kMinorScaleIntervals.data();
            case kScaleChromatic:
                return kChromaticScaleIntervals.data();
            default:
                return kMajorScaleIntervals.data();
        }
    }

    //--- Get number of notes in scale ---
    int getScaleSize(ScaleType scaleType) const
    {
        switch (scaleType)
        {
            case kScaleMajor: return 7;
            case kScaleMinor: return 7;
            case kScaleChromatic: return 12;
            default: return 7;
        }
    }

    //--- Find nearest note in scale ---
    double findNearestInScale(double noteInOctave, const int* intervals, int numNotes) const
    {
        // Adjust note relative to root
        double relativeNote = noteInOctave - mRootNote;
        if (relativeNote < 0.0)
            relativeNote += 12.0;
        if (relativeNote >= 12.0)
            relativeNote -= 12.0;

        // Find the closest scale degree
        double minDistance = 12.0;
        int closestInterval = 0;

        for (int i = 0; i < numNotes; ++i)
        {
            int interval = intervals[i];

            // Check distance to this interval
            double distance = std::abs(relativeNote - interval);

            // Also check distance wrapping around the octave
            double distanceWrap = std::abs(relativeNote - (interval + 12));
            if (distanceWrap < distance)
                distance = distanceWrap;

            distanceWrap = std::abs(relativeNote - (interval - 12));
            if (distanceWrap < distance)
                distance = distanceWrap;

            if (distance < minDistance)
            {
                minDistance = distance;
                closestInterval = interval;
            }
        }

        // Convert back to absolute note in octave
        double result = mRootNote + closestInterval;

        // Handle octave wrapping
        while (result >= 12.0)
            result -= 12.0;
        while (result < 0.0)
            result += 12.0;

        return result;
    }

    //--- Frequency ↔ MIDI conversion ---
    static double frequencyToMidi(double frequency)
    {
        // MIDI note = 69 + 12 * log2(f / 440)
        if (frequency <= 0.0)
            return 0.0;
        return 69.0 + 12.0 * std::log2(frequency / 440.0);
    }

    static double midiToFrequency(double midiNote)
    {
        // f = 440 * 2^((MIDI - 69) / 12)
        return 440.0 * std::pow(2.0, (midiNote - 69.0) / 12.0);
    }

    //--- Member variables ---
    int mRootNote;          // 0-11 (C to B)
    ScaleType mScaleType;   // Major, Minor, or Chromatic
};

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
