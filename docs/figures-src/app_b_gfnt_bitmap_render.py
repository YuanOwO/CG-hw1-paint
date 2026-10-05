#!/usr/bin/env python3
"""Draw the GFNT bitmap placement and coloring diagram with real font glyphs."""

from __future__ import annotations

import argparse
import math
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[2]
OUTPUT = ROOT / "docs" / "figures" / "app_b_gfnt-bitmap-render.png"
DEFAULT_CUBIC = Path(
    "/Users/yuan/projects/CG/hw1-font-converter/Cubic-11-1.500/fonts/ttf/Cubic_11.ttf"
)
DEFAULT_UNIFONT = Path("/Users/yuan/projects/CG/hw1-font-converter/unifont-16.0.04.ttf")
LABEL_FONT = Path("/Users/yuan/Library/Fonts/NotoSansCJKtc-Regular.otf")

INK = (35, 39, 47)
BLUE = (31, 83, 172)
RED = (181, 44, 44)
GREEN = (34, 126, 65)
GRAY = (113, 121, 132)
LIGHT_GRID = (214, 219, 226)
PANEL = (247, 249, 252)


def font(size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.truetype(str(LABEL_FONT), size)


def glyph_data(path: Path, pixel_size: int, character: str) -> dict[str, object]:
    face = ImageFont.truetype(str(path), pixel_size)
    bbox = face.getbbox(character, anchor="ls")
    if bbox is None:
        raise RuntimeError(f"No bounding box for {character!r} in {path}")

    left, top, right, bottom = bbox
    mask = Image.frombytes(
        "L",
        face.getmask(character, mode="L").size,
        bytes(face.getmask(character, mode="L")),
    )
    return {
        "mask": mask,
        "width": right - left,
        "height": bottom - top,
        "bearing_x": left,
        "bearing_y": -top,
        "advance_x": round(face.getlength(character)),
        "ascender": face.getmetrics()[0],
        "descender": face.getmetrics()[1],
    }


def dashed_line(
    draw: ImageDraw.ImageDraw, points: tuple[int, int, int, int], fill, width=3, dash=12
) -> None:
    x0, y0, x1, y1 = points
    length = math.hypot(x1 - x0, y1 - y0)
    if length == 0:
        return
    ux = (x1 - x0) / length
    uy = (y1 - y0) / length
    position = 0.0
    while position < length:
        end = min(position + dash, length)
        draw.line(
            (
                round(x0 + ux * position),
                round(y0 + uy * position),
                round(x0 + ux * end),
                round(y0 + uy * end),
            ),
            fill=fill,
            width=width,
        )
        position += dash * 2


def arrow_head(draw: ImageDraw.ImageDraw, tip, direction, fill, size=12) -> None:
    tx, ty = tip
    dx, dy = direction
    length = math.hypot(dx, dy)
    dx /= length
    dy /= length
    px, py = -dy, dx
    base_x = tx - dx * size
    base_y = ty - dy * size
    draw.polygon(
        [
            (tx, ty),
            (base_x + px * size * 0.45, base_y + py * size * 0.45),
            (base_x - px * size * 0.45, base_y - py * size * 0.45),
        ],
        fill=fill,
    )


def double_arrow(draw: ImageDraw.ImageDraw, start, end, fill, width=4) -> None:
    draw.line((start, end), fill=fill, width=width)
    arrow_head(draw, end, (end[0] - start[0], end[1] - start[1]), fill)
    arrow_head(draw, start, (start[0] - end[0], start[1] - end[1]), fill)


def draw_pixel_grid(
    draw: ImageDraw.ImageDraw,
    mask: Image.Image,
    origin: tuple[int, int],
    cell: int,
    color: tuple[int, int, int] | None,
) -> None:
    ox, oy = origin
    pixels = mask.load()
    for y in range(mask.height):
        for x in range(mask.width):
            alpha = pixels[x, y]
            if color is None:
                value = 255 - alpha
                fill = (value, value, value)
            else:
                fill = tuple(
                    round((channel * alpha + 255 * (255 - alpha)) / 255)
                    for channel in color
                )
            draw.rectangle(
                (
                    ox + x * cell,
                    oy + y * cell,
                    ox + (x + 1) * cell,
                    oy + (y + 1) * cell,
                ),
                fill=fill,
            )

    for x in range(mask.width + 1):
        draw.line(
            (ox + x * cell, oy, ox + x * cell, oy + mask.height * cell),
            fill=LIGHT_GRID,
            width=1,
        )
    for y in range(mask.height + 1):
        draw.line(
            (ox, oy + y * cell, ox + mask.width * cell, oy + y * cell),
            fill=LIGHT_GRID,
            width=1,
        )
    draw.rectangle(
        (ox, oy, ox + mask.width * cell, oy + mask.height * cell),
        outline=GRAY,
        width=3,
    )


def draw_placement_panel(draw: ImageDraw.ImageDraw, cubic: dict[str, object]) -> None:
    draw.rounded_rectangle(
        (70, 135, 1160, 970), radius=24, fill=PANEL, outline=(184, 192, 203), width=3
    )
    draw.text((110, 170), "字形定位：Cubic-11，U+4F60", font=font(34), fill=INK)

    mask = cubic["mask"]
    assert isinstance(mask, Image.Image)
    scale = 28
    pen_x = 300
    baseline_y = 700
    glyph_left = pen_x + int(cubic["bearing_x"]) * scale
    glyph_top = baseline_y - int(cubic["bearing_y"]) * scale
    next_pen_x = pen_x + int(cubic["advance_x"]) * scale

    colored = Image.new("RGBA", mask.size, BLUE + (0,))
    colored.putalpha(mask)
    enlarged = colored.resize(
        (mask.width * scale, mask.height * scale), Image.Resampling.NEAREST
    )
    canvas.alpha_composite(enlarged, (glyph_left, glyph_top))

    for x in range(mask.width + 1):
        draw.line(
            (
                glyph_left + x * scale,
                glyph_top,
                glyph_left + x * scale,
                glyph_top + mask.height * scale,
            ),
            fill=(219, 225, 233),
            width=1,
        )
    for y in range(mask.height + 1):
        draw.line(
            (
                glyph_left,
                glyph_top + y * scale,
                glyph_left + mask.width * scale,
                glyph_top + y * scale,
            ),
            fill=(219, 225, 233),
            width=1,
        )

    draw.rectangle(
        (
            glyph_left,
            glyph_top,
            glyph_left + mask.width * scale,
            glyph_top + mask.height * scale,
        ),
        outline=GRAY,
        width=3,
    )
    dashed_line(draw, (130, baseline_y, 1080, baseline_y), RED, width=4)
    draw.text((875, baseline_y - 48), "基線", font=font(26), fill=RED)
    dashed_line(draw, (pen_x, 400, pen_x, 870), GRAY, width=3)
    dashed_line(draw, (next_pen_x, 400, next_pen_x, 870), GRAY, width=3)
    draw.text((pen_x - 70, 875), "目前 penX", font=font(25), fill=INK)
    draw.text((next_pen_x - 65, 875), "下一個 penX", font=font(25), fill=INK)

    double_arrow(
        draw, (pen_x - 55, glyph_top), (pen_x - 55, baseline_y), GREEN, width=4
    )
    draw.text(
        (75, (glyph_top + baseline_y) // 2 - 18),
        f"垂直 bearingY = {cubic['bearing_y']}",
        font=font(24),
        fill=GREEN,
    )
    draw.text(
        (glyph_left + 8, glyph_top - 42),
        f"水平 bearingX = {cubic['bearing_x']}",
        font=font(24),
        fill=GREEN,
    )

    double_arrow(draw, (pen_x, 820), (next_pen_x, 820), BLUE, width=4)
    draw.text(
        (pen_x + 58, 830),
        f"水平推進 advanceX = {cubic['advance_x']}",
        font=font(24),
        fill=BLUE,
    )
    draw.text(
        (680, 455),
        f"bitmap 尺寸：{cubic['width']} × {cubic['height']}\n"
        f"上升部／下降部：{cubic['ascender']}／{cubic['descender']}",
        font=font(26),
        fill=INK,
        spacing=10,
    )


def draw_runtime_panel(draw: ImageDraw.ImageDraw, unifont: dict[str, object]) -> None:
    draw.rounded_rectangle(
        (1240, 135, 2330, 970), radius=24, fill=PANEL, outline=(184, 192, 203), width=3
    )
    draw.text((1280, 170), "點陣圖著色：Unifont 16，U+4F60", font=font(34), fill=INK)

    mask = unifont["mask"]
    assert isinstance(mask, Image.Image)
    cell = 24
    source = (1300, 320)
    colored = (1860, 320)

    draw.text((1300, 265), "GFNT 透明度點陣圖", font=font(28), fill=INK)
    draw_pixel_grid(draw, mask, source, cell, color=None)
    draw.text((1858, 265), "RGBA 像素", font=font(28), fill=INK)
    draw_pixel_grid(draw, mask, colored, cell, color=BLUE)

    arrow_y = source[1] + mask.height * cell // 2
    draw.line((1705, arrow_y, 1820, arrow_y), fill=BLUE, width=5)
    arrow_head(draw, (1820, arrow_y), (1, 0), BLUE, size=16)
    draw.multiline_text(
        (1690, arrow_y - 100),
        "目前顏色\n× 字形透明度",
        font=font(23),
        fill=BLUE,
        spacing=7,
        align="center",
    )

    row_x = 1280
    row_top = source[1]
    row_bottom = source[1] + mask.height * cell
    draw.line((row_x, row_top, row_x, row_bottom), fill=GREEN, width=4)
    arrow_head(draw, (row_x, row_bottom), (0, 1), GREEN, size=15)
    draw.text(
        (row_x - 12, row_top - 2), "第一列", font=font(22), fill=GREEN, anchor="ra"
    )
    draw.text(
        (row_x - 12, row_bottom + 2), "最後一列", font=font(22), fill=GREEN, anchor="ra"
    )

    draw.text(
        (1350, 765),
        "以字形左上角設定繪製位置：glRasterPos2f(glyphLeft, glyphTop)\n"
        "由上往下繪製：glPixelZoom(1, -1)  →  glDrawPixels",
        font=font(27),
        fill=INK,
        spacing=12,
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument("--cubic", type=Path, default=DEFAULT_CUBIC)
    parser.add_argument("--unifont", type=Path, default=DEFAULT_UNIFONT)
    parser.add_argument("--output", type=Path, default=OUTPUT)
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    cubic_data = glyph_data(args.cubic, 12, "你")
    unifont_data = glyph_data(args.unifont, 16, "你")

    canvas = Image.new("RGBA", (2400, 1050), "white")
    drawing = ImageDraw.Draw(canvas)
    drawing.text((80, 45), "GFNT 點陣字形定位與執行期著色", font=font(42), fill=INK)
    draw_placement_panel(drawing, cubic_data)
    draw_runtime_panel(drawing, unifont_data)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    canvas.convert("RGB").save(args.output, dpi=(240, 240), optimize=True)
