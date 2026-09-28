#include "color.hpp"

#include <algorithm>

namespace paint {

ColorRGBA hsv2rgb(const ColorHSV& hsv) {
    float h = hsv.h, s = hsv.s, v = hsv.v;

    if (s == 0.0f) {
        return ColorRGBA(v, v, v);
    }

    // 0~360 -> 0~6
    h *= 6.0f / 360.0f;

    // 保證 h 落在 [0, 6)
    while (h < 0.0f) {
        h += 6.0f;
    }

    while (h >= 6.0f) {
        h -= 6.0f;
    }

    int sector = static_cast<int>(h);

    // sector 內的位置，範圍 [0, 1)
    h -= sector;

    float tab[4];

    tab[0] = v;
    tab[1] = v * (1.0f - s);
    tab[2] = v * (1.0f - s * h);
    tab[3] = v * (1.0f - s * (1.0f - h));

    static const int sectorData[6][3] = {
        {0, 3, 1},
        {2, 0, 1},
        {1, 0, 3},
        {1, 2, 0},
        {3, 1, 0},
        {0, 1, 2}
    };

    float r = tab[sectorData[sector][0]];
    float g = tab[sectorData[sector][1]];
    float b = tab[sectorData[sector][2]];

    return ColorRGBA(r, g, b);
}

ColorHSV rgb2hsv(const ColorRGBA& rgb) {
    float r = rgb.r;
    float g = rgb.g;
    float b = rgb.b;

    float v = std::max({r, g, b});
    float vmin = std::min({r, g, b});
    float diff = v - vmin;

    float h = 0.0f;
    float s = 0.0f;

    // Saturation
    if (v != 0.0f) {
        s = diff / v;
    }

    // Hue
    if (diff != 0.0f) {
        if (v == r) {
            h = (g - b) / diff;
        } else if (v == g) {
            h = 2.0f + (b - r) / diff;
        } else {
            h = 4.0f + (r - g) / diff;
        }

        h *= 60.0f;

        if (h < 0.0f) {
            h += 360.0f;
        }
    }

    return ColorHSV(h, s, v);
}

}  // namespace paint
