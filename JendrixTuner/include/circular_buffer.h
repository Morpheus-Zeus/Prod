//------------------------------------------------------------------------
// Project     : JendrixTuner
// Filename    : circular_buffer.h
// Created by  : Claude, 01/2025
// Description : Circular buffer for audio windowing
//------------------------------------------------------------------------
#pragma once

#include <vector>
#include <cstring>

namespace Steinberg {
namespace JendrixTuner {

//------------------------------------------------------------------------
// CircularBuffer - Efficient ring buffer for audio samples
// Used to maintain a sliding window of audio for pitch detection
//------------------------------------------------------------------------
template <typename T>
class CircularBuffer
{
public:
    CircularBuffer() : mSize(0), mWritePos(0) {}

    // Initialize with a specific size
    explicit CircularBuffer(size_t size) : mSize(size), mWritePos(0)
    {
        mBuffer.resize(size, T(0));
    }

    // Resize the buffer (clears existing data)
    void resize(size_t newSize)
    {
        mSize = newSize;
        mWritePos = 0;
        mBuffer.resize(newSize, T(0));
    }

    // Add a single sample to the buffer
    void write(T sample)
    {
        if (mSize == 0) return;

        mBuffer[mWritePos] = sample;
        mWritePos = (mWritePos + 1) % mSize;
    }

    // Add multiple samples to the buffer
    void write(const T* samples, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            write(samples[i]);
        }
    }

    // Read a sample at a specific delay from current write position
    // delay=0 gives the most recent sample
    // delay=1 gives the previous sample, etc.
    T read(size_t delay) const
    {
        if (mSize == 0 || delay >= mSize) return T(0);

        // Calculate read position (going backwards from write position)
        size_t readPos = (mWritePos + mSize - delay - 1) % mSize;
        return mBuffer[readPos];
    }

    // Get the buffer contents in chronological order
    // The most recent sample will be at output[size-1]
    void getOrdered(T* output, size_t count) const
    {
        if (count > mSize) count = mSize;

        for (size_t i = 0; i < count; ++i)
        {
            output[i] = read(count - 1 - i);
        }
    }

    // Clear the buffer (fill with zeros)
    void clear()
    {
        std::fill(mBuffer.begin(), mBuffer.end(), T(0));
        mWritePos = 0;
    }

    size_t size() const { return mSize; }
    bool isEmpty() const { return mSize == 0; }

private:
    std::vector<T> mBuffer;
    size_t mSize;
    size_t mWritePos;
};

//------------------------------------------------------------------------
} // namespace JendrixTuner
} // namespace Steinberg
