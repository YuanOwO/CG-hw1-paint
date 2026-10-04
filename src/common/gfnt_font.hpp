#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "common/gfnt.hpp"

namespace paint {

class GfntFont {
   public:
    static GfntFont load(const std::filesystem::path& path);

    const gfnt::GfntHeaderV1& header() const { return _header; }

    const gfnt::GfntGlyphV1* findGlyph(char32_t codepoint) const;
    const gfnt::GfntGlyphV1* findGlyphOrFallback(char32_t codepoint) const;

    const std::uint8_t* bitmap(const gfnt::GfntGlyphV1& glyph) const {
        return _bitmapData.data() + glyph.bitmapOffset;
    }

   private:
    gfnt::GfntHeaderV1 _header{};
    std::vector<gfnt::GfntGlyphV1> _glyphs;
    std::vector<std::uint8_t> _bitmapData;
};

}  // namespace paint
