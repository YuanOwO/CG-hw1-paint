#include "render/renderer/text_renderer.hpp"

#include <GL/freeglut.h>

#include <stdexcept>
#include <string>
#include <variant>

#include "common/font.hpp"
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

}  // namespace

void TextRenderer::render(RenderContext& context, const ui::TextElement& element) {
    const auto& padding = element.padding();
    const Point origin{static_cast<float>(padding.left), static_cast<float>(padding.top)};

    drawText(context, element.text(), element.fontStyle(), element.color(), origin);
}

void TextRenderer::draw(RenderContext& context, const drawing::TextObject& object) {
    drawText(context, object.text(), object.style().font, object.style().color, object.position());
}

void TextRenderer::drawText(RenderContext& context, const std::string& text,
                            const FontStyle& fontStyle, const ColorRGBA& color,
                            const Point& origin) {
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
    }

    glPopAttrib();
}

}  // namespace paint::render
