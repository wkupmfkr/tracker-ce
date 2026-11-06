#pragma once

#include "mtStructs.h"

namespace midiInputMap
{
// Project format v1 reserved seven zeroed bytes per MIDI instrument/channel.
constexpr uint8_t kSignature = 0xB3;
constexpr int kLastTarget = kSynthInstrumentsCount + kSampleInstrumentsCount;
constexpr int kFirstSource = 1024;

constexpr int kSourceCount = 2 * 16 * 128;
inline bool isMonitorSource(int source)
{
    return source >= kFirstSource && source < kFirstSource + kSourceCount;
}
inline bool isRecordingSource(int source)
{
    return source >= kFirstSource + kSourceCount;
}

inline int instrument(uint8_t channel)
{
    if (channel < 1 || channel > kMidiInstrumentsCount)
        return -1;
    const auto& bytes = mtProject.values.midiInstrument[channel - 1].reserved;
    const int target = bytes[0];
    if (bytes[1] != kSignature || target == 0 || target > kLastTarget)
        return -1;
    if (target <= kSynthInstrumentsCount)
        return kSampleInstrumentsCount + kMidiInstrumentsCount + target - 1;
    return target - kSynthInstrumentsCount - 1;
}

inline int16_t source(bool usb, uint8_t channel, uint8_t rawNote)
{
    return kFirstSource + ((usb ? 16 : 0) + channel - 1) * 128 + rawNote;
}
}
