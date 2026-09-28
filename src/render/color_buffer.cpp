#include "render/color_buffer.hpp"

#include <GL/freeglut.h>

namespace paint {

void ColorBuffer::capture(int width, int height) {
    if (width <= 0 || height <= 0) {
        invalidate();
        return;
    }

    // 每個像素依序保存 R、G、B、A，各占 1 byte。
    _pixels.resize(static_cast<std::size_t>(width) * height * 4);  // RGBA

    // OpenGL 設定由同一 context 的繪圖程式共用，先保存本函式會修改的設定。
    GLint oldAlignment;
    GLint oldReadBuffer;
    glGetIntegerv(GL_PACK_ALIGNMENT, &oldAlignment);
    glGetIntegerv(GL_READ_BUFFER, &oldReadBuffer);

    // 目前使用 GLUT_SINGLE，顯示與繪製都在 front buffer。
    glReadBuffer(GL_FRONT);
    // PACK 控制 OpenGL 寫入主記憶體時的列對齊；1 表示每列不補齊額外 bytes。
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // 讀取範圍從 framebuffer 左下角 (0, 0) 開始，不受投影矩陣影響。
    // 資料逐列由下往上排列；restore 沿用此順序，因此不需要上下翻轉。
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _pixels.data());

    // 恢復之前的 OpenGL 狀態
    glPixelStorei(GL_PACK_ALIGNMENT, oldAlignment);
    glReadBuffer(oldReadBuffer);

    _width = width;
    _height = height;
    _valid = true;
}

void ColorBuffer::restore() const {
    if (!_valid) {
        return;
    }

    // UNPACK_ALIGNMENT 屬於 client pixel-store 狀態，不由下方 glPushAttrib 保存，
    // 因此需另外記錄並恢復。
    GLint oldAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlignment);

    // 保存接下來會改動的 buffer、測試、像素縮放與光柵位置等狀態。
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_SCISSOR_BIT |
                 GL_PIXEL_MODE_BIT | GL_CURRENT_BIT);

    // 直接覆寫保存的顏色，避免再次混色，或被深度、模板、裁切與 alpha 測試擋住。
    glDrawBuffer(GL_FRONT);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_ALPHA_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);  // 允許寫入所有顏色通道。

    // UNPACK 控制從主記憶體讀取像素的列對齊，需與保存時的資料排列一致。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelZoom(1.0f, 1.0f);  // 原尺寸還原，不縮放也不翻轉。

    // glRasterPos 會經過矩陣轉換。此處依賴 Window 的
    // glOrtho(0, width, height, 0, -1, 1) 與單位 model-view：
    // 畫布座標 (0, height) 對應 framebuffer 左下角，作為像素寫入起點。
    glRasterPos2i(0, _height);
    glDrawPixels(_width, _height, GL_RGBA, GL_UNSIGNED_BYTE, _pixels.data());

    // 恢復狀態，避免影響後續網格、圖形或草稿的繪製。
    glPixelStorei(GL_UNPACK_ALIGNMENT, oldAlignment);
    glPopAttrib();
}

void ColorBuffer::resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        _width = 0;
        _height = 0;
        _pixels.clear();
        invalidate();
        return;
    }

    _pixels.resize(static_cast<std::size_t>(width) * height * 4);  // RGBA
    _width = width;
    _height = height;
    invalidate();  // 調整大小後，像素內容不再有效
}

void ColorBuffer::fill(const ColorRGBA& color) {
    if (_width <= 0 || _height <= 0) {
        return;
    }

    for (int y = 0; y < _height; ++y) {
        for (int x = 0; x < _width; ++x) {
            std::size_t index = static_cast<std::size_t>(y) * _width * 4 + static_cast<std::size_t>(x) * 4;
            // 將浮點色彩 [0, 1] 轉成 unsigned byte [0, 255]。
            _pixels[index] = static_cast<unsigned char>(color.r * 255.0f);
            _pixels[index + 1] = static_cast<unsigned char>(color.g * 255.0f);
            _pixels[index + 2] = static_cast<unsigned char>(color.b * 255.0f);
            _pixels[index + 3] = static_cast<unsigned char>(color.a * 255.0f);
        }
    }

    _valid = true;
}

}  // namespace paint
