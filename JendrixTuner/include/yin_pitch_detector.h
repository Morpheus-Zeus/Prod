//------------------------------------------------------------------------
// Project     : JendrixTuner
// Filename    : yin_pitch_detector.h
// Created by  : Claude, 01/2025
// Description : YIN pitch detection algorithm
//
// Reference: "YIN, a fundamental frequency estimator for speech and music"
//            by Alain de Cheveigné and Hideki Kawahara (2002)
//------------------------------------------------------------------------
#pragma once

#include <vector>
#include <cmath>
#include <algorithm>

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// YinPitchDetector - Monophonic pitch detection using the YIN algorithm
//
// YIN is specifically designed for musical pitch detection and works well
// with vocals. It's more accurate than simple autocorrelation.
//------------------------------------------------------------------------
class YinPitchDetector
{
public:
    YinPitchDetector();
    ~YinPitchDetector();

    // Initialize the detector with buffer size and sample rate
    // bufferSize: typically 1024-2048 samples for low latency
    // sampleRate: audio sample rate (44100 or 48000 Hz)
    void initialize(size_t bufferSize, double sampleRate);

    // Detect pitch from a buffer of audio samples
    // Returns detected frequency in Hz, or 0.0 if no pitch detected
    double detectPitch(const float* audioBuffer, size_t bufferSize);

    // Get the confidence of the last detection (0.0 to 1.0)
    // Higher values = more confident detection
    double getConfidence() const { return mConfidence; }

    // Check if the last detection was valid
    bool isPitchDetected() const { return mPitchDetected; }

    // Get detected frequency as MIDI note number (69 = A4 = 440Hz)
    double getMidiNote() const;

    // Get detected frequency as note name (e.g., "A4", "C#3")
    const char* getNoteName() const;

    // Configuration
    void setThreshold(double threshold) { mThreshold = threshold; }
    double getThreshold() const { return mThreshold; }

private:
    // YIN algorithm steps
    void calculateDifferenceFunction(const float* buffer);
    void calculateCumulativeMeanNormalizedDifference();
    int findAbsoluteThreshold();
    double parabolicInterpolation(int tauEstimate);

    // Helper: Convert frequency to MIDI note
    static double frequencyToMidi(double frequency);
    static const char* midiToNoteName(int midiNote);

    // Configuration
    double mSampleRate;
    size_t mBufferSize;
    double mThreshold;          // YIN threshold (typical: 0.1 to 0.15)

    // Working buffers
    std::vector<double> mDifferenceFunction;        // d(τ)
    std::vector<double> mCumulativeMean;            // d'(τ)

    // Results
    double mDetectedFrequency;
    double mConfidence;
    bool mPitchDetected;
    int mDetectedPeriod;        // Period in samples

    // Constants
    static constexpr double kDefaultThreshold = 0.15;
    static constexpr double kMinFrequency = 60.0;   // ~B1
    static constexpr double kMaxFrequency = 1000.0; // ~B5
};

//------------------------------------------------------------------------
// Implementation
//------------------------------------------------------------------------

inline YinPitchDetector::YinPitchDetector()
    : mSampleRate(44100.0)
    , mBufferSize(2048)
    , mThreshold(kDefaultThreshold)
    , mDetectedFrequency(0.0)
    , mConfidence(0.0)
    , mPitchDetected(false)
    , mDetectedPeriod(0)
{
}

inline YinPitchDetector::~YinPitchDetector()
{
}

inline void YinPitchDetector::initialize(size_t bufferSize, double sampleRate)
{
    mBufferSize = bufferSize;
    mSampleRate = sampleRate;

    // Allocate working buffers
    mDifferenceFunction.resize(bufferSize / 2);
    mCumulativeMean.resize(bufferSize / 2);
}

inline double YinPitchDetector::detectPitch(const float* audioBuffer, size_t bufferSize)
{
    if (bufferSize < mBufferSize)
    {
        mPitchDetected = false;
        mDetectedFrequency = 0.0;
        return 0.0;
    }

    // Step 1: Calculate difference function
    calculateDifferenceFunction(audioBuffer);

    // Step 2: Calculate cumulative mean normalized difference
    calculateCumulativeMeanNormalizedDifference();

    // Step 3: Find the first minimum below threshold
    int tauEstimate = findAbsoluteThreshold();

    if (tauEstimate <= 0)
    {
        // No pitch detected
        mPitchDetected = false;
        mDetectedFrequency = 0.0;
        mConfidence = 0.0;
        return 0.0;
    }

    // Step 4: Parabolic interpolation for better accuracy
    double betterTau = parabolicInterpolation(tauEstimate);

    // Convert period (in samples) to frequency (in Hz)
    mDetectedFrequency = mSampleRate / betterTau;

    // Check if frequency is in valid range
    if (mDetectedFrequency < kMinFrequency || mDetectedFrequency > kMaxFrequency)
    {
        mPitchDetected = false;
        mDetectedFrequency = 0.0;
        mConfidence = 0.0;
        return 0.0;
    }

    // Calculate confidence (inverse of the normalized difference at detected period)
    mConfidence = 1.0 - mCumulativeMean[tauEstimate];
    mPitchDetected = true;
    mDetectedPeriod = tauEstimate;

    return mDetectedFrequency;
}

inline void YinPitchDetector::calculateDifferenceFunction(const float* buffer)
{
    // Calculate d(τ) = Σ[(x[j] - x[j+τ])²]
    // This is the squared difference between the signal and a delayed version

    size_t halfSize = mBufferSize / 2;

    for (size_t tau = 0; tau < halfSize; ++tau)
    {
        double sum = 0.0;

        for (size_t j = 0; j < halfSize; ++j)
        {
            double delta = buffer[j] - buffer[j + tau];
            sum += delta * delta;
        }

        mDifferenceFunction[tau] = sum;
    }
}

inline void YinPitchDetector::calculateCumulativeMeanNormalizedDifference()
{
    // Calculate d'(τ) = d(τ) / [(1/τ) * Σ(d(k)) for k=1 to τ]
    // This normalizes the difference function by its running mean

    mCumulativeMean[0] = 1.0; // Special case: τ=0 is always 1

    double runningSum = 0.0;
    size_t halfSize = mBufferSize / 2;

    for (size_t tau = 1; tau < halfSize; ++tau)
    {
        runningSum += mDifferenceFunction[tau];

        if (runningSum == 0.0)
        {
            mCumulativeMean[tau] = 1.0;
        }
        else
        {
            mCumulativeMean[tau] = mDifferenceFunction[tau] / (runningSum / tau);
        }
    }
}

inline int YinPitchDetector::findAbsoluteThreshold()
{
    // Find the smallest τ where d'(τ) < threshold AND it's a local minimum

    size_t halfSize = mBufferSize / 2;

    // Start search from a minimum period (corresponds to max frequency)
    size_t minPeriod = static_cast<size_t>(mSampleRate / kMaxFrequency);
    size_t maxPeriod = static_cast<size_t>(mSampleRate / kMinFrequency);

    if (maxPeriod >= halfSize)
        maxPeriod = halfSize - 1;

    for (size_t tau = minPeriod; tau < maxPeriod; ++tau)
    {
        // Check if below threshold
        if (mCumulativeMean[tau] < mThreshold)
        {
            // Check if it's a local minimum
            // (value is less than neighbors)
            if (tau == 0 || tau >= halfSize - 1)
                continue;

            if (mCumulativeMean[tau] < mCumulativeMean[tau - 1] &&
                mCumulativeMean[tau] < mCumulativeMean[tau + 1])
            {
                return static_cast<int>(tau);
            }
        }
    }

    // If no minimum found below threshold, find the global minimum
    // This gives us a "best guess" even if confidence is low
    auto minIt = std::min_element(
        mCumulativeMean.begin() + minPeriod,
        mCumulativeMean.begin() + maxPeriod
    );

    if (minIt != mCumulativeMean.begin() + maxPeriod)
    {
        return static_cast<int>(std::distance(mCumulativeMean.begin(), minIt));
    }

    return -1; // No pitch detected
}

inline double YinPitchDetector::parabolicInterpolation(int tauEstimate)
{
    // Use parabolic interpolation to get sub-sample accuracy
    // Fits a parabola through the minimum and its neighbors

    size_t halfSize = mBufferSize / 2;

    if (tauEstimate <= 0 || tauEstimate >= static_cast<int>(halfSize) - 1)
        return static_cast<double>(tauEstimate);

    // Three points: (tau-1, y1), (tau, y2), (tau+1, y3)
    double y1 = mCumulativeMean[tauEstimate - 1];
    double y2 = mCumulativeMean[tauEstimate];
    double y3 = mCumulativeMean[tauEstimate + 1];

    // Parabolic interpolation formula
    double delta = (y3 - y1) / (2.0 * (2.0 * y2 - y1 - y3));

    return tauEstimate + delta;
}

inline double YinPitchDetector::getMidiNote() const
{
    if (!mPitchDetected)
        return 0.0;

    return frequencyToMidi(mDetectedFrequency);
}

inline const char* YinPitchDetector::getNoteName() const
{
    if (!mPitchDetected)
        return "---";

    int midiNote = static_cast<int>(std::round(getMidiNote()));
    return midiToNoteName(midiNote);
}

inline double YinPitchDetector::frequencyToMidi(double frequency)
{
    // MIDI note = 69 + 12 * log2(f / 440)
    if (frequency <= 0.0)
        return 0.0;

    return 69.0 + 12.0 * std::log2(frequency / 440.0);
}

inline const char* YinPitchDetector::midiToNoteName(int midiNote)
{
    static const char* noteNames[12] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    static char noteName[8]; // "C#10" = max 4 chars + null

    int noteClass = midiNote % 12;
    int octave = (midiNote / 12) - 1;

    snprintf(noteName, sizeof(noteName), "%s%d", noteNames[noteClass], octave);

    return noteName;
}

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
