#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>

// A view over ordinary playlist slots. No serialized repeat metadata.
namespace SongRepeats {
inline int begin(const uint8_t* p, int n, int slot) {
    if (n <= 0) return 0;
    slot = std::max(0, std::min(slot, n - 1));
    while (slot > 0 && p[slot - 1] == p[slot]) --slot;
    return slot;
}
inline int end(const uint8_t* p, int n, int slot) {
    if (n <= 0) return 0;
    slot = std::max(0, std::min(slot, n - 1));
    const auto pattern = p[slot++];
    while (slot < n && p[slot] == pattern) ++slot;
    return slot;
}
inline int count(const uint8_t* p, int n) {
    int result = 0;
    for (int i = 0; i < n; i = end(p, n, i)) ++result;
    return result;
}
inline int row(const uint8_t* p, int n, int slot) {
    int result = 0;
    for (int i = 0; i < n; ++result) {
        i = end(p, n, i);
        if (slot < i) return result;
    }
    return std::max(0, result - 1);
}
inline int slot(const uint8_t* p, int n, int rowIndex) {
    int i = 0;
    while (rowIndex-- > 0 && i < n) i = end(p, n, i);
    return i < n ? i : begin(p, n, n - 1);
}
inline int move(const uint8_t* p, int n, int current, int delta) {
    return slot(p, n, std::max(0, std::min(row(p, n, current) + delta, count(p, n) - 1)));
}
inline int firstRow(const uint8_t* p, int n, int selected, int shown) {
    return std::max(0, std::min(row(p, n, selected) - shown / 2, count(p, n) - shown));
}
// Replace [start, finish) with copies of pattern; preserve the suffix and zero tail.
inline int replace(uint8_t* p, int n, int capacity, int start, int finish, int copies, uint8_t pattern) {
    copies = std::max(0, std::min(copies, capacity - n + finish - start));
    memmove(p + start + copies, p + finish, n - finish);
    memset(p + start, pattern, copies);
    n += copies - (finish - start);
    memset(p + n, 0, capacity - n);
    return n;
}
inline int remap(int position, int start, int finish, int copies, int newLength) {
    if (position >= finish) position += copies - (finish - start);
    else if (position >= start) position = start + std::min(position - start, std::max(0, copies - 1));
    return std::max(0, std::min(position, newLength - 1));
}
}
