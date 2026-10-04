#pragma once

#include <cstdint>

namespace paint::gfnt {

inline const char MAGIC[4] = {'G', 'F', 'N', 'T'};
inline const uint16_t VERSION = 1;

#pragma pack(push, 1)

struct GfntHeaderV1 {
    char magic[4];

    uint16_t version;
    uint16_t pixelSize;

    int16_t ascender;
    int16_t descender;

    uint32_t glyphCount;
};

struct GfntGlyphV1 {
    uint32_t codepoint;

    uint16_t width;
    uint16_t height;

    int16_t bearingX;
    int16_t bearingY;

    uint16_t advanceX;

    uint32_t bitmapOffset;
};

#pragma pack(pop)

static_assert(sizeof(GfntHeaderV1) == 16);
static_assert(sizeof(GfntGlyphV1) == 18);

}  // namespace paint::gfnt
