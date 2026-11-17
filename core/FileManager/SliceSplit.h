#pragma once
#include "mtStructs.h"

// No sample buffers or SDRAM initializers: staging uses only checked free sample RAM.
class SliceSplit {
public:
    const char* begin(int source);
    void update();
    bool busy() const { return phase != 0; }
    int progress() const;
    const char* result = nullptr;
    int count = 0;
private:
    int phase = 0, source = 0, part = 0, channel = 0, channels = 1;
    uint32_t boundary[MAX_SLICE_NUMBER + 1];
    uint8_t slots[MAX_SLICE_NUMBER];
    uint8_t blocked[kSampleInstrumentsCount], noteBlocked[kSampleInstrumentsCount];
    uint32_t frame = 0, staged = 0, total = 0, insertion = 0, words = 0;
    uint32_t reverseLeft = 0, reverseRight = 0;
    void finish(const char* message);
    void startReverse(uint32_t first, uint32_t end);
    void commit();
};
extern SliceSplit sliceSplit;
