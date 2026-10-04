#include "render/renderer/text_renderer.hpp"

#include <GL/freeglut.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "common/font.hpp"
#include "common/utf8.hpp"
#include "drawing/text_object.hpp"
#include "ui/elements/text.hpp"

namespace paint::render {

namespace {

// FreeGLUT 內建 Bitmap 字型的 raster origin Y。
// 基線位置 = 行頂端 + 行高 - originY。
float bitmapOriginY(BitmapFont font) {
    switch (font) {
    case BitmapFont::BITMAP_8_BY_13:
    case BitmapFont::BITMAP_HELVETICA_10:
        return 3.0f;

    case BitmapFont::BITMAP_9_BY_15:
    case BitmapFont::BITMAP_HELVETICA_12:
    case BitmapFont::BITMAP_TIMES_ROMAN_10:
        return 4.0f;

    case BitmapFont::BITMAP_HELVETICA_18:
        return 5.0f;

    case BitmapFont::BITMAP_TIMES_ROMAN_24:
        return 7.0f;
    }

    throw std::invalid_argument("Unknown bitmap font");
}

void renderBitmap(const std::string& text, const BitmapFontStyle& style, float left, float top) {
    void* font = mapFont(style.font);

    const float lineHeight = static_cast<float>(glutBitmapHeight(font));

    float x = left;
    float y = top + lineHeight - bitmapOriginY(style.font);

    for (unsigned char c : text) {
        if (c == '\n') {
            x = left;
            y += lineHeight;  // 元素座標向下為正。
            continue;
        }

        glRasterPos2f(x, y);
        glutBitmapCharacter(font, c);

        x += static_cast<float>(glutBitmapWidth(font, c));
    }
}

void renderStroke(RenderContext& context, const std::string& text, const StrokeFontStyle& style, float left,
                  float top) {
    void* font = mapFont(style.font);

    // FreeGLUT Roman / Mono Roman 的字型座標上界。
    constexpr float ascent = 119.048f;

    const float scale = style.size;
    const float lineHeight = glutStrokeHeight(font) * scale;

    float x = left;
    float y = top + ascent * scale;

    for (unsigned char c : text) {
        if (c == '\n') {
            x = left;
            y += lineHeight;
            continue;
        }

        // Stroke 字型僅處理 ASCII。
        if (c >= 128) {
            continue;
        }

        context.pushTransform();
        context.translate(x, y);

        // Stroke 字型原本向上為正。
        // 翻轉 Y，配合目前 UI 向下為正的座標系。
        glScalef(scale, -scale, 1.0f);

        // 此函式會移動目前矩陣，因此每個字都恢復矩陣。
        glutStrokeCharacter(font, c);

        context.popTransform();

        // 與目前 getFontWidth() 使用相同的寬度計算。
        x += static_cast<float>(glutStrokeWidth(font, c)) * scale;
    }
}

void renderGfnt(RenderContext& context, const std::string& text, const GfntFontStyle& style, float left,
                float top) {
    // GFNT glyph 直接使用 raster position 繪製，不需要改變 model-view matrix。
    (void)context;

    const GfntFont& font = mapFont(style.font);
    const auto& header = font.header();
    const float lineHeight = static_cast<float>(header.ascender + header.descender);

    float penX = left;
    float baselineY = top + static_cast<float>(header.ascender);

    // glDrawPixels 不會用 glColor 替 alpha bitmap 上色，所以取出目前文字顏色，
    // 再為每個 glyph 產生著色後的 RGBA pixels。
    GLfloat currentColor[4];
    glGetFloatv(GL_CURRENT_COLOR, currentColor);

    const std::uint8_t red = colorToByte(currentColor[0]);
    const std::uint8_t green = colorToByte(currentColor[1]);
    const std::uint8_t blue = colorToByte(currentColor[2]);
    const std::uint8_t colorAlpha = colorToByte(currentColor[3]);

    std::vector<std::uint8_t> rgbaPixels;

    GLint previousUnpackAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousUnpackAlignment);

    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_PIXEL_MODE_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // GFNT 的第一列是 bitmap 頂端；負的 Y zoom 讓 OpenGL 由上往下畫。
    glPixelZoom(1.0f, -1.0f);

    std::size_t offset = 0;
    while (offset < text.size()) {
        const auto decoded = utf8::decodeOne(text, offset);
        if (decoded.bytesConsumed == 0) {
            break;
        }
        offset += decoded.bytesConsumed;

        if (decoded.codepoint == U'\n') {
            penX = left;
            baselineY += lineHeight;
            continue;
        }

        const gfnt::GfntGlyphV1* glyph = font.findGlyphOrFallback(decoded.codepoint);
        if (glyph == nullptr) {
            penX += static_cast<float>(header.pixelSize);
            continue;
        }

        const float glyphLeft = penX + static_cast<float>(glyph->bearingX);
        const float glyphTop = baselineY - static_cast<float>(glyph->bearingY);
        const std::size_t pixelCount =
            static_cast<std::size_t>(glyph->width) * static_cast<std::size_t>(glyph->height);

        if (pixelCount != 0) {
            const std::uint8_t* alphaPixels = font.bitmap(*glyph);
            rgbaPixels.resize(pixelCount * 4);

            for (std::size_t i = 0; i < pixelCount; ++i) {
                rgbaPixels[i * 4] = red;
                rgbaPixels[i * 4 + 1] = green;
                rgbaPixels[i * 4 + 2] = blue;
                rgbaPixels[i * 4 + 3] = static_cast<std::uint8_t>(
                    (static_cast<unsigned int>(alphaPixels[i]) * colorAlpha + 127) / 255);
            }

            glRasterPos2f(glyphLeft, glyphTop);
            glDrawPixels(glyph->width, glyph->height, GL_RGBA, GL_UNSIGNED_BYTE, rgbaPixels.data());
        }

        penX += static_cast<float>(glyph->advanceX);
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, previousUnpackAlignment);
    glPopAttrib();
}

}  // namespace

void TextRenderer::render(RenderContext& context, const ui::TextElement& element) {
    const auto& padding = element.padding();
    const Point origin{static_cast<float>(padding.left), static_cast<float>(padding.top)};

    drawText(context, element.text(), element.fontStyle(), element.color(), origin);
}

void TextRenderer::draw(RenderContext& context, const drawing::TextObject& object) {
    drawText(context, object.text(), object.style().font, object.style().color, object.position());
}

void TextRenderer::drawText(RenderContext& context, const std::string& text, const FontStyle& fontStyle,
                            const ColorRGBA& color, const Point& origin) {
    if (text.empty()) {
        return;
    }

    // 避免影響其他元素的顏色、raster position 與線寬。
    glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
    glColor4f(color.r, color.g, color.b, color.a);

    if (const auto* bitmap = std::get_if<BitmapFontStyle>(&fontStyle)) {
        renderBitmap(text, *bitmap, origin.x(), origin.y());
    } else if (const auto* stroke = std::get_if<StrokeFontStyle>(&fontStyle)) {
        glLineWidth(1.0f);
        renderStroke(context, text, *stroke, origin.x(), origin.y());
    } else if (const auto* gfnt = std::get_if<GfntFontStyle>(&fontStyle)) {
        renderGfnt(context, text, *gfnt, origin.x(), origin.y());
    }

    glPopAttrib();
}

}  // namespace paint::render
