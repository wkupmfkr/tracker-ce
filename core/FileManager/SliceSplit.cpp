#include "SliceSplit.h"
#include "fileManager.h"
#include "VoiceStreamManager.h"
#include "mtAudioEngine.h"
#include <algorithm>
#include <cstdio>
#include <cstring>

SliceSplit sliceSplit;

const char* SliceSplit::begin(int src) {
    if (busy() || newFileManager.getCurrentOperation() != fmNoOperation)
        return "Please wait for file operations";
    if (sequencer.getSeqState() != Sequencer::SEQ_STATE_STOP || sequencer.isRec())
        return "Stop playback and recording first";
    if (src < 0 || src >= kSampleInstrumentsCount) return "Select a sample instrument";
    const auto& original = mtProject.instrument[src];
    if (!original.isActive || original.playMode != playModeBeatSlice || !original.sample.length)
        return "Select a Beat Slice instrument";
    count = original.sliceNumber;
    if (count < 1 || count > MAX_SLICE_NUMBER) return "No valid slices";
    int found = 0;
    for (int i = 0; i < kSampleInstrumentsCount && found < count; ++i)
        if (i != src && !mtProject.instrument[i].isActive) slots[found++] = i;
    if (found != count) return "Not enough empty instrument slots";
    // Marker coordinates are normalized; do not include audio before the first marker.
    for (int i = 0; i < count; ++i)
        boundary[i] = uint64_t(original.slices[i]) * original.sample.length / MAX_16BIT;
    boundary[count] = original.sample.length;
    for (int i = 0; i < count; ++i)
        if (boundary[i] >= boundary[i + 1]) return "Empty or unordered slice boundaries";
    channels = newFileManager.isInstrumentStereo(src) ? 2 : 1;
    uint64_t bytes = uint64_t(boundary[count] - boundary[0]) * channels * sizeof(int16_t);
    uint32_t limit = std::min<uint32_t>(mtProject.max_memory, SAMPLE_MEMORY_SIZE);
    if (mtProject.used_memory > limit || bytes > limit - mtProject.used_memory)
        return "Not enough free sample RAM";
    // Refuse unexpected layouts rather than moving any existing sample blindly.
    uint64_t used = 0;
    for (int i = 0; i < kSampleInstrumentsCount; ++i) {
        const auto& inst = mtProject.instrument[i];
        const auto& mem = mtProject.sampleMemory[i];
        if (!inst.isActive) continue;
        uint32_t frames = mem.isStreamed ? mem.ramFrames :
                         (mem.ramFrames ? mem.ramFrames : inst.sample.length);
        int nch = newFileManager.isInstrumentStereo(i) ? 2 : 1;
        if (used > limit || inst.sample.address != externalRamSampleMemory + used / 2 ||
            mem.address[0] != inst.sample.address ||
            (nch == 2 && mem.address[1] != inst.sample.address + frames))
            return "Sample RAM layout needs reload";
        used += uint64_t(frames) * nch * 2;
    }
    if (used != mtProject.used_memory) return "Sample RAM layout needs reload";
    source = src; part = channel = 0; frame = staged = 0; total = bytes / 2;
    result = nullptr;
    for (int i = 0; i < kSampleInstrumentsCount; ++i) {
        blocked[i] = isBlockedInstrument[i];
        noteBlocked[i] = isBlockedNoteOn[i];
    }
    engine.blockAllNoteOn();
    sequencer.stopManualNotes();
    engine.cutAllVolumeEnvelopes();
    // Staging has at least one main-loop turn before memory is moved.
    phase = 1;
    newFileManager.setCurrentOperation(fmSplitSlices);
    newFileManager.setStatus(fmSplittingSlices);
    return nullptr;
}

int SliceSplit::progress() const {
    return phase == 1 ? int(uint64_t(staged) * 70 / total) : 70 + part * 30 / count;
}

void SliceSplit::finish(const char* message) {
    result = message;
    for (int i = 0; i < kSampleInstrumentsCount; ++i) {
        isBlockedInstrument[i] = blocked[i];
        isBlockedNoteOn[i] = noteBlocked[i];
    }
    phase = 0;
    newFileManager.setStatus(fmIdle);
    newFileManager.setCurrentOperation(fmNoOperation);
}

void SliceSplit::startReverse(uint32_t first, uint32_t end) {
    reverseLeft = first; reverseRight = end;
}

void SliceSplit::update() {
    if (!phase) return;
    if (phase == 1) {
        uint32_t length = boundary[part + 1] - boundary[part];
        uint32_t n = std::min<uint32_t>(4096, length - frame);
        int16_t* dest = externalRamSampleMemory + mtProject.used_memory / 2 + staged;
        if (!voiceStreamManager.readInstrumentFramesBlocking(source, channel,
                boundary[part] + frame, n, dest)) {
            finish("Read failed; no instruments changed"); return;
        }
        frame += n; staged += n;
        if (frame == length) {
            frame = 0;
            if (++channel == channels) { channel = 0; ++part; }
        }
        if (part == count) {
            part = 0; phase = 2;
            for (int i = 0; i < kSampleInstrumentsCount; ++i) engine.setBlockedInstrument(i);
        }
        return;
    }
    uint32_t used = mtProject.used_memory / 2;
    if (phase == 2) {
        insertion = used;
        for (int i = slots[part]; i < kSampleInstrumentsCount; ++i)
            if (mtProject.instrument[i].isActive) {
                insertion = mtProject.instrument[i].sample.address - externalRamSampleMemory;
                break;
            }
        words = (boundary[part + 1] - boundary[part]) * channels;
        // Rotate [old samples][staged slice] without scratch allocation. Subsequent
        // staged slices remain beyond used+words and are never overwritten.
        startReverse(insertion, used); phase = 3;
    }
    if (phase >= 3 && phase <= 5) {
        unsigned budget = 8192;
        while (reverseRight > reverseLeft + 1 && budget--) {
            --reverseRight;
            std::swap(externalRamSampleMemory[reverseLeft++], externalRamSampleMemory[reverseRight]);
        }
        if (reverseRight > reverseLeft + 1) return;
        if (phase == 3) { startReverse(used, used + words); phase = 4; return; }
        if (phase == 4) { startReverse(insertion, used + words); phase = 5; return; }
        commit();
        if (++part == count) { finish(nullptr); return; }
        phase = 2;
    }
}

void SliceSplit::commit() {
    int dst = slots[part];
    for (int i = dst; i < kSampleInstrumentsCount; ++i) {
        if (!mtProject.instrument[i].isActive) continue;
        auto& mem = mtProject.sampleMemory[i];
        mtProject.instrument[i].sample.address += words;
        mem.address[0] += words;
        if (mem.address[1]) mem.address[1] += words;
        voiceStreamManager.invalidateInstrumentVoices(i);
    }
    auto& inst = mtProject.instrument[dst];
    inst = mtProject.instrument[source];
    inst.sample.address = externalRamSampleMemory + insertion;
    inst.sample.length = boundary[part + 1] - boundary[part];
    inst.playMode = playModeSingleShot;
    inst.useC3SampleRoot();
    inst.startPoint = inst.loopPoint1 = 0;
    inst.endPoint = inst.loopPoint2 = MAX_16BIT;
    inst.sliceNumber = inst.selectedSlice = inst.slicePlayThru = 0;
    std::memset(inst.slices, 0, sizeof(inst.slices));
    std::snprintf(inst.sample.file_name, sizeof(inst.sample.file_name), "%.24s_S%02d",
                  mtProject.instrument[source].sample.file_name, part + 1);
    std::snprintf(inst.name, sizeof(inst.name), "%02d", dst + 1);
    voiceStreamManager.clearInstrumentMetadata(dst);
    auto& mem = mtProject.sampleMemory[dst];
    mem = strSampleMemory{};
    mem.isStereo = channels == 2;
    mem.ramFrames = mem.preCacheFrames = inst.sample.length;
    mem.address[0] = inst.sample.address;
    mem.address[1] = inst.sample.address + (channels == 2 ? inst.sample.length : 0);
    mtProject.used_memory += words * 2;
    newFileManager.setInstrumentStructChanged(dst);
    newFileManager.setInstrumentSampleChanged(dst);
    newFileManager.setProjectStructChanged();
}
