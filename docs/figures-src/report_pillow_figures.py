#!/usr/bin/env python3
"""Generate explanatory raster figures used by the report."""

from __future__ import annotations

from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "docs" / "figures"
FONT_PATH = Path("/Users/yuan/Library/Fonts/NotoSansCJKtc-Regular.otf")

INK = (35, 39, 47)
BLUE = (31, 83, 172)
RED = (181, 44, 44)
GREEN = (34, 126, 65)
GRAY = (113, 121, 132)
PALE_BLUE = (232, 239, 251)
PALE_YELLOW = (255, 248, 222)


def font(size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.truetype(str(FONT_PATH), size)


def canvas(height: int = 1000) -> tuple[Image.Image, ImageDraw.ImageDraw]:
    image = Image.new("RGB", (2400, height), "white")
    return image, ImageDraw.Draw(image)


def save(image: Image.Image, name: str) -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    image.save(OUT / name, dpi=(240, 240), optimize=True)


def arrow(
    draw: ImageDraw.ImageDraw,
    start: tuple[int, int],
    end: tuple[int, int],
    color=BLUE,
    width=5,
) -> None:
    draw.line((start, end), fill=color, width=width)
    x0, y0 = start
    x1, y1 = end
    dx, dy = x1 - x0, y1 - y0
    length = max((dx * dx + dy * dy) ** 0.5, 1)
    ux, uy = dx / length, dy / length
    px, py = -uy, ux
    size = 16
    base_x, base_y = x1 - ux * size, y1 - uy * size
    draw.polygon(
        [
            (x1, y1),
            (base_x + px * 7, base_y + py * 7),
            (base_x - px * 7, base_y - py * 7),
        ],
        fill=color,
    )


def centered(
    draw: ImageDraw.ImageDraw,
    box: tuple[int, int, int, int],
    text: str,
    size: int,
    fill=INK,
) -> None:
    x0, y0, x1, y1 = box
    bounds = draw.multiline_textbbox(
        (0, 0), text, font=font(size), spacing=7, align="center"
    )
    width = bounds[2] - bounds[0]
    height = bounds[3] - bounds[1]
    draw.multiline_text(
        ((x0 + x1 - width) / 2, (y0 + y1 - height) / 2 - bounds[1]),
        text,
        font=font(size),
        fill=fill,
        spacing=7,
        align="center",
    )


def draw_utf8_caret() -> None:
    image, draw = canvas(820)

    # 同一字串「A你B」上下對照：上排是 code point，下排是 UTF-8 bytes。
    # 上排格子的寬度剛好跨過對應的 bytes，讓「一個中文字 = 3 bytes」一眼可見。
    cell = 300
    left = 470
    edges = [left + cell * i for i in range(6)]  # 6 個 byte 邊界
    top_row = (190, 380)
    bottom_row = (520, 660)

    centered(draw, (40, top_row[0], left - 30, top_row[1]), "Code point", 46, fill=BLUE)
    centered(draw, (40, bottom_row[0], left - 30, bottom_row[1]), "UTF-8 bytes", 46, fill=INK)

    characters = [
        (edges[0], edges[1], "A", "U+0041"),
        (edges[1], edges[4], "你", "U+4F60"),
        (edges[4], edges[5], "B", "U+0042"),
    ]
    for x0, x1, glyph, codepoint in characters:
        box = (x0 + 6, top_row[0], x1 - 6, top_row[1])
        draw.rounded_rectangle(box, radius=18, fill=PALE_BLUE, outline=BLUE, width=5)
        centered(draw, (x0, top_row[0] + 10, x1, top_row[1] - 60), glyph, 88)
        centered(draw, (x0, top_row[1] - 70, x1, top_row[1] - 10), codepoint, 38, fill=GRAY)

    for index, byte in enumerate(["41", "E4", "BD", "A0", "42"]):
        box = (edges[index] + 6, bottom_row[0], edges[index + 1] - 6, bottom_row[1])
        draw.rounded_rectangle(box, radius=14, fill=PALE_YELLOW, outline=(150, 126, 40), width=4)
        centered(draw, box, byte, 56)

    # 上下排的對應關係：每個字元邊界往下對到 byte 邊界。
    for x in (edges[0], edges[1], edges[4], edges[5]):
        for y in range(top_row[1] + 8, bottom_row[0] - 4, 22):
            draw.line((x, y, x, min(y + 12, bottom_row[0] - 4)), fill=GRAY, width=4)

    # 綠色：以字元為單位時，游標只會停在字元邊界。
    for x in (edges[0], edges[1], edges[4], edges[5]):
        draw.polygon([(x - 20, 120), (x + 20, 120), (x, 160)], fill=GREEN)
    centered(draw, (edges[5] + 20, top_row[0], 2390, top_row[1]), "游標只停在\n字元邊界", 44, fill=GREEN)

    # 紅色：以 byte 為單位時，游標可能停在「你」的中間。
    cut = edges[3]
    draw.line((cut, bottom_row[0] - 40, cut, bottom_row[1] + 30), fill=RED, width=10)
    draw.polygon([(cut - 22, bottom_row[1] + 72), (cut + 22, bottom_row[1] + 72), (cut, bottom_row[1] + 32)], fill=RED)
    centered(draw, (edges[5] + 20, bottom_row[0], 2390, bottom_row[1]), "以 byte 計算\n可能切斷字元", 44, fill=RED)
    centered(
        draw,
        (left, bottom_row[1] + 80, edges[5], 810),
        "此時按 Backspace 只刪除 A0，留下的 E4 BD 無法解碼",
        44,
        fill=RED,
    )
    save(image, "ch05_utf8-caret.png")


def dimension(
    draw: ImageDraw.ImageDraw, x: int, y0: int, y1: int, label: str, color, side: str = "right"
) -> None:
    """垂直的尺寸標註：兩端有箭頭與短橫線，標籤放在一側。"""
    draw.line((x - 18, y0, x + 18, y0), fill=color, width=4)
    draw.line((x - 18, y1, x + 18, y1), fill=color, width=4)
    mid = (y0 + y1) // 2
    arrow(draw, (x, mid), (x, y0 + 2), color=color, width=5)
    arrow(draw, (x, mid), (x, y1 - 2), color=color, width=5)
    if side == "right":
        draw.text((x + 30, mid - 34), label, font=font(56), fill=color)
    else:
        bounds = draw.textbbox((0, 0), label, font=font(56))
        draw.text((x - 30 - (bounds[2] - bounds[0]), mid - 34), label, font=font(56), fill=color)


def draw_framebuffer_coordinates() -> None:
    image, draw = canvas(1100)

    # 同一個擷取區域，分別從視窗頂端（UI）與底端（OpenGL）量測。
    window = (620, 110, 1700, 970)
    region = (900, 330, 1420, 630)
    draw.rectangle(window, fill="white", outline=GRAY, width=5)

    rows = 3
    row_height = (region[3] - region[1]) // rows
    for i in range(rows):
        y1 = region[3] - i * row_height
        shade = [(96, 146, 220), (148, 184, 236), (200, 220, 246)][i]
        draw.rectangle((region[0], y1 - row_height, region[2], y1), fill=shade)
        centered(draw, (region[0], y1 - row_height, region[2], y1), f"row {i}", 40, fill=INK)
    draw.rectangle(region, outline=BLUE, width=5)
    centered(draw, (region[0], region[1] - 70, region[2], region[1] - 10), "擷取區域", 42, fill=BLUE)

    # UI 原點：左上角，Y 向下。
    ox, oy = window[0], window[1]
    draw.ellipse((ox - 14, oy - 14, ox + 14, oy + 14), fill=RED)
    arrow(draw, (ox, oy), (ox + 150, oy), color=RED, width=6)
    arrow(draw, (ox, oy), (ox, oy + 150), color=RED, width=6)
    draw.text((ox - 300, oy - 30), "UI 原點", font=font(46), fill=RED)
    draw.text((ox + 20, oy + 140), "Y", font=font(40), fill=RED)

    # OpenGL 原點：左下角，Y 向上。
    gx, gy = window[0], window[3]
    draw.ellipse((gx - 14, gy - 14, gx + 14, gy + 14), fill=GREEN)
    arrow(draw, (gx, gy), (gx + 150, gy), color=GREEN, width=6)
    arrow(draw, (gx, gy), (gx, gy - 150), color=GREEN, width=6)
    draw.text((gx - 360, gy - 30), "OpenGL 原點", font=font(46), fill=GREEN)
    draw.text((gx + 20, gy - 190), "Y", font=font(40), fill=GREEN)

    # 尺寸標註：右側為區域相關距離，左側為整個視窗高度。
    dim_x = window[2] + 90
    for y in (region[1], region[3]):
        for x in range(region[2] + 10, dim_x - 20, 24):
            draw.line((x, y, x + 12, y), fill=GRAY, width=3)
    dimension(draw, dim_x, window[1], region[1], "y", RED)
    dimension(draw, dim_x, region[1], region[3], "h", BLUE)
    dimension(draw, dim_x, region[3], window[3], "H − y − h", GREEN)
    dimension(draw, 2250, window[1], window[3], "H", GRAY)
    for y in (window[1], window[3]):
        for x in range(window[2] + 10, 2230, 24):
            draw.line((x, y, x + 12, y), fill=GRAY, width=3)

    centered(
        draw,
        (window[0], window[3] + 45, window[2], 1090),
        "glReadPixels() 從 row 0（區域底部）開始往上讀",
        40,
        fill=GRAY,
    )
    save(image, "ch06_framebuffer-coordinate.png")


def main() -> None:
    draw_utf8_caret()
    draw_framebuffer_coordinates()


if __name__ == "__main__":
    main()
