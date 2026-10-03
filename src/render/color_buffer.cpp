#include "render/color_buffer.hpp"

#include <GL/freeglut.h>

namespace paint {

void ColorBuffer::capture(int x, int y, int width, int height) {
    // 擷取目前 GLUT 視窗的指定區域；呼叫前需先選定視窗並完成繪製。
    // (x, y) 為區域左上角的視窗座標，擷取範圍須位於視窗內。
    if (width <= 0 || height <= 0) {
        invalidate();
        return;
    }

    // 視窗座標以左上角為原點、Y 向下；glReadPixels 以左下角為原點、Y 向上。
    // 將區域左上角的 Y 轉成左下角的 Y，X 不變。
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    const int bottom = windowHeight - y - height;

    // 每個像素儲存 RGBA 四個通道，各占 1 byte。
    _pixels.resize(static_cast<std::size_t>(width) * height * 4);

    // 保存本函式會修改的 OpenGL 狀態。
    GLint oldAlignment;
    GLint oldReadBuffer;
    glGetIntegerv(GL_PACK_ALIGNMENT, &oldAlignment);
    glGetIntegerv(GL_READ_BUFFER, &oldReadBuffer);

    // 目前使用 GLUT_SINGLE，因此從 front buffer 讀取。
    // PACK_ALIGNMENT 設為 1，讓輸出的每列像素緊密排列，不加入對齊填補。
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // 像素由下往上逐列讀回；上述座標轉換不會翻轉像素資料。
    glReadPixels(x, bottom, width, height, GL_RGBA, GL_UNSIGNED_BYTE, _pixels.data());

    // 還原 OpenGL 狀態，避免影響後續操作。
    glPixelStorei(GL_PACK_ALIGNMENT, oldAlignment);
    glReadBuffer(oldReadBuffer);

    // 記錄擷取尺寸，並將緩衝區標記為有效。
    _width = width;
    _height = height;
    _valid = true;
}

void ColorBuffer::restore(int x, int y) const {
    if (!_valid) {
        return;
    }

    // (x, y) 為還原區域左上角的視窗座標。
    // 將 Y 轉成 OpenGL 視窗座標中的區域底部位置。
    const int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    const int bottom = windowHeight - y - _height;

    // UNPACK_ALIGNMENT 屬於 client 狀態，需另外保存。
    GLint oldAlignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldAlignment);

    // 保存接下來會修改的繪圖、測試與像素操作狀態。
    glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_SCISSOR_BIT |
                 GL_PIXEL_MODE_BIT | GL_CURRENT_BIT);

    // 目前使用 GLUT_SINGLE，因此寫入 front buffer。
    // 直接覆寫像素，避免混色、測試或裁切影響還原結果。
    glDrawBuffer(GL_FRONT);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_ALPHA_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // 像素資料緊密排列，並以原尺寸還原。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelZoom(1.0f, 1.0f);

    // 直接指定左下角的 OpenGL 視窗座標，不受投影與模型視圖矩陣影響。
    // capture() 保存的像素由下往上排列，因此不需翻轉。
    glWindowPos2i(x, bottom);
    glDrawPixels(_width, _height, GL_RGBA, GL_UNSIGNED_BYTE, _pixels.data());

    // 還原 OpenGL 狀態，避免影響後續繪製。
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
