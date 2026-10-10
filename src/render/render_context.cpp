#include "render/render_context.hpp"

#include <algorithm>
#include <initializer_list>
#include <limits>
#include <vector>

#include "platform/glut.hpp"

namespace paint::render {
namespace {

// 中點畫圓法：依序回傳第二個八分圓 (0 <= x <= y) 上的整數點，其餘七個八分圓由呼叫端以對稱取得。
template <typename Visit>
void midpointOctant(int radius, Visit visit) {
    int x = 0;
    int y = radius;
    int decision = 1 - radius;
    while (x <= y) {
        visit(x, y);

        // decision 為中點 (x + 1, y - 1/2) 的判別式：小於 0 表示中點在圓內，y 保持不變。
        if (decision < 0) {
            decision += 2 * x + 3;
        } else {
            decision += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
}

// 回傳中點圓在每一列 (以 |dy| 為索引) 的 |x|：outermost 為 true 取最外側，否則取最內側。
// 八分圓的點 (x, y) 同時落在第 y 列 (|x| = x) 與第 x 列 (|x| = y)。
std::vector<int> rowExtents(int radius, bool outermost) {
    std::vector<int> extents(radius + 1, outermost ? -1 : std::numeric_limits<int>::max());
    midpointOctant(radius, [&extents, outermost](int x, int y) {
        if (outermost) {
            extents[y] = std::max(extents[y], x);
            extents[x] = std::max(extents[x], y);
        } else {
            extents[y] = std::min(extents[y], x);
            extents[x] = std::min(extents[x], y);
        }
    });
    return extents;
}

// 開始以 GL_QUADS 逐像素繪製；呼叫端須先 glPushAttrib，結束後 glEnd 與 glPopAttrib。
void beginPixels(const ColorRGBA& color) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);
    glBegin(GL_QUADS);
}

// 在 glBegin(GL_QUADS) 區段內畫出從 (x, y) 開始、寬 width 像素、高 1 像素的水平區段。
// 以方塊而非 GL_POINTS 繪製，才會跟隨目前座標系縮放。
void pixelRect(int x, int y, int width) {
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + 1);
    glVertex2i(x, y + 1);
}

}  // namespace

void RenderContext::pushTransform() {
    glPushMatrix();
}

void RenderContext::popTransform() {
    glPopMatrix();
}

void RenderContext::translate(float x, float y) {
    glTranslatef(x, y, 0.0f);
}

void RenderContext::fillRect(int width, int height, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || color.a <= 0.0f) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor4f(color.r, color.g, color.b, color.a);
    glRecti(0, 0, width, height);
    glPopAttrib();
}

void RenderContext::strokeCircle(int centerX, int centerY, int radius, const ColorRGBA& color) {
    if (radius < 0 || color.a <= 0.0f) {
        return;
    }

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    beginPixels(color);

    // 對稱點重疊時 (x == 0 或 x == y) 只畫一次，避免半透明顏色疊色。
    const auto plot = [centerX, centerY](int dx, int dy) { pixelRect(centerX + dx, centerY + dy, 1); };
    midpointOctant(radius, [&plot](int x, int y) {
        if (x == 0) {
            plot(0, y);
            plot(0, -y);
            if (y != 0) {
                plot(y, 0);
                plot(-y, 0);
            }
            return;
        }

        plot(x, y);
        plot(-x, y);
        plot(x, -y);
        plot(-x, -y);
        if (x != y) {
            plot(y, x);
            plot(-y, x);
            plot(y, -x);
            plot(-y, -x);
        }
    });

    glEnd();
    glPopAttrib();
}

void RenderContext::fillRing(int centerX, int centerY, int innerRadius, int outerRadius,
                             const ColorRGBA& color) {
    if (innerRadius < 0 || outerRadius < innerRadius || color.a <= 0.0f) {
        return;
    }

    // 環形每列從內圓最內側填到外圓最外側。
    const std::vector<int> outerExtent = rowExtents(outerRadius, true);
    const std::vector<int> innerExtent = rowExtents(innerRadius, false);

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    beginPixels(color);

    for (int dy = 0; dy <= outerRadius; dy++) {
        // 超出內圓範圍的列沒有中空部分，整列填滿。
        const int from = dy <= innerRadius ? innerExtent[dy] : 0;
        const int to = outerExtent[dy];

        for (const int y : {centerY + dy, centerY - dy}) {
            if (from == 0) {
                pixelRect(centerX - to, y, 2 * to + 1);
            } else {
                pixelRect(centerX + from, y, to - from + 1);
                pixelRect(centerX - to, y, to - from + 1);
            }

            if (dy == 0) {
                break;  // 中心列只有一列，避免重複繪製
            }
        }
    }

    glEnd();
    glPopAttrib();
}

void RenderContext::fillRoundedRect(int x, int y, int width, int height, int radius, const ColorRGBA& color) {
    if (width <= 0 || height <= 0 || color.a <= 0.0f) {
        return;
    }

    // 圓心落在像素中心，直徑為 2r + 1 像素；限制半徑讓上下、左右兩側的圓角不會交疊。
    radius = std::clamp(radius, 0, (std::min(width, height) - 1) / 2);
    const std::vector<int> extents = rowExtents(radius, true);

    // 左右圓角的圓心欄位；中間的直邊由兩者之間的水平區段自然連接。
    const int leftCenter = x + radius;
    const int rightCenter = x + width - 1 - radius;

    glPushAttrib(GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT | GL_POLYGON_BIT);
    beginPixels(color);

    for (int row = 0; row < height; row++) {
        // dy 為此列到最近圓心列的距離；位於上下圓心之間的列 dy = 0，整列填滿。
        int dy = 0;
        if (row < radius) {
            dy = radius - row;
        } else if (row > height - 1 - radius) {
            dy = row - (height - 1 - radius);
        }

        const int left = leftCenter - extents[dy];
        const int right = rightCenter + extents[dy];
        pixelRect(left, y + row, right - left + 1);
    }

    glEnd();
    glPopAttrib();
}

}  // namespace paint::render
