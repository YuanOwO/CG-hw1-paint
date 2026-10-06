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
LIGHT = (220, 225, 232)
PANEL = (247, 249, 252)
PALE_BLUE = (232, 239, 251)
PALE_GREEN = (232, 246, 235)
PALE_RED = (252, 236, 236)
PALE_YELLOW = (255, 248, 222)


def font(size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.truetype(str(FONT_PATH), size)


def canvas(height: int = 1000) -> tuple[Image.Image, ImageDraw.ImageDraw]:
    image = Image.new("RGB", (2400, height), "white")
    return image, ImageDraw.Draw(image)


def save(image: Image.Image, name: str) -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    image.save(OUT / name, dpi=(240, 240), optimize=True)


def panel(
    draw: ImageDraw.ImageDraw, box: tuple[int, int, int, int], title: str
) -> None:
    draw.rounded_rectangle(box, radius=24, fill=PANEL, outline=(184, 192, 203), width=3)
    draw.text((box[0] + 34, box[1] + 24), title, font=font(32), fill=INK)


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


def draw_grid_coordinate() -> None:
    image, draw = canvas(1030)
    draw.text((70, 45), "Canvas 座標轉換與 Grid 模式", font=font(42), fill=INK)
    panel(draw, (60, 130, 1120, 960), "Window 座標轉成 Canvas local 座標")
    panel(draw, (1160, 130, 2340, 960), "Grid 僅作為背景參考")

    window = (150, 270, 1010, 790)
    canvas_box = (330, 395, 920, 710)
    draw.rectangle(window, fill=(255, 255, 255), outline=GRAY, width=4)
    draw.text((820, 292), "Window", font=font(28), fill=INK)
    draw.rectangle(canvas_box, fill=PALE_BLUE, outline=BLUE, width=4)
    draw.text((350, 410), "CanvasElement", font=font(28), fill=BLUE)
    draw.ellipse((665, 535, 683, 553), fill=RED)
    draw.text((690, 520), "滑鼠位置 Pwindow = (520, 360)", font=font(24), fill=RED)
    draw.ellipse((323, 388, 337, 402), fill=GREEN)
    draw.text((345, 360), "Canvas 左上角 = (180, 140)", font=font(23), fill=GREEN)

    arrow(draw, (150, 270), (260, 270), color=GRAY, width=4)
    arrow(draw, (150, 270), (150, 380), color=GRAY, width=4)
    draw.text((266, 254), "X", font=font(23), fill=GRAY)
    draw.text((132, 385), "Y", font=font(23), fill=GRAY)
    draw.text((110, 230), "(0, 0)", font=font(21), fill=GRAY)

    formula_box = (205, 810, 995, 920)
    draw.rounded_rectangle(
        formula_box, radius=14, fill=PALE_GREEN, outline=GREEN, width=2
    )
    centered(
        draw,
        formula_box,
        "Pcanvas = Pwindow - Canvas 左上角\n= (520, 360) - (180, 140) = (340, 220)",
        25,
    )

    modes = [("Lines", 1210), ("Dots", 1585), ("None", 1960)]
    for mode, x0 in modes:
        box = (x0, 285, x0 + 320, 655)
        draw.rounded_rectangle(box, radius=16, fill="white", outline=LIGHT, width=3)
        centered(draw, (x0, 210, x0 + 320, 275), mode, 28)
        if mode == "Lines":
            for x in range(x0 + 55, x0 + 320, 55):
                draw.line((x, 285, x, 655), fill=(220, 220, 220), width=3)
            for y in range(340, 655, 55):
                draw.line((x0, y, x0 + 320, y), fill=(220, 220, 220), width=3)
        elif mode == "Dots":
            for x in range(x0 + 55, x0 + 320, 55):
                for y in range(340, 655, 55):
                    draw.ellipse((x - 3, y - 3, x + 3, y + 3), fill=(190, 190, 190))
        draw.line((x0 + 55, 575, x0 + 250, 390), fill=BLUE, width=12)
        draw.ellipse((x0 + 210, 485, x0 + 245, 520), fill=RED)

    draw.line((1265, 700, 1320, 700), fill=GRAY, width=3)
    draw.line((1265, 688, 1265, 712), fill=GRAY, width=3)
    draw.line((1320, 688, 1320, 712), fill=GRAY, width=3)
    draw.text((1250, 725), "間距 25 px", font=font(23), fill=GRAY)
    draw.text((1580, 704), "Dots 的程式 point size 為 2 px", font=font(23), fill=GRAY)

    order = [("Grid", PALE_YELLOW), ("SceneObject", PALE_BLUE), ("Draft", PALE_RED)]
    x = 1350
    for index, (label, fill) in enumerate(order):
        box = (x, 805, x + 250, 880)
        draw.rounded_rectangle(box, radius=12, fill=fill, outline=GRAY, width=2)
        centered(draw, box, label, 25)
        if index < len(order) - 1:
            arrow(draw, (x + 260, 842), (x + 330, 842), color=GRAY, width=4)
        x += 360
    draw.text(
        (1390, 900), "繪製順序：背景參考 → 正式物件 → 工具預覽", font=font(25), fill=INK
    )
    save(image, "ch04_grid-coordinate.png")


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


def draw_ppm_export() -> None:
    image, draw = canvas(1030)
    draw.text((70, 45), "ColorBuffer 到 PPM P6 的輸出順序", font=font(42), fill=INK)
    panel(draw, (60, 130, 760, 950), "ColorBuffer：RGBA")
    panel(draw, (850, 130, 1550, 950), "PpmExporter")
    panel(draw, (1640, 130, 2340, 950), "PPM P6：RGB")

    colors = [
        [(237, 92, 92), (242, 157, 75), (244, 213, 91), (147, 201, 91)],
        [(88, 176, 129), (72, 176, 190), (90, 145, 220), (126, 112, 204)],
        [(169, 107, 194), (213, 112, 166), (150, 150, 150), (70, 70, 70)],
    ]
    x0, y0, cell = 190, 330, 110
    for row in range(3):
        memory_row = 2 - row
        for col in range(4):
            draw.rectangle(
                (
                    x0 + col * cell,
                    y0 + row * cell,
                    x0 + (col + 1) * cell,
                    y0 + (row + 1) * cell,
                ),
                fill=colors[row][col],
                outline="white",
                width=3,
            )
        draw.text(
            (x0 + 455, y0 + row * cell + 34),
            f"memory row {memory_row}",
            font=font(23),
            fill=INK,
        )
    draw.text((180, 260), "畫面上方", font=font(24), fill=RED)
    draw.text((180, 680), "畫面下方／memory row 0", font=font(24), fill=GREEN)
    draw.text((155, 735), "每個 pixel：R  G  B  A", font=font(27), fill=INK)
    draw.text((155, 785), "每個 channel：1 byte", font=font(24), fill=GRAY)

    loop_box = (930, 290, 1470, 500)
    draw.rounded_rectangle(loop_box, radius=18, fill=PALE_BLUE, outline=BLUE, width=3)
    centered(draw, loop_box, "for i = height - 1 ... 0\n由畫面上方開始寫出", 29)
    arrow(draw, (1200, 530), (1200, 650), color=GREEN, width=5)
    alpha_box = (930, 670, 1470, 825)
    draw.rounded_rectangle(alpha_box, radius=18, fill=PALE_RED, outline=RED, width=3)
    centered(draw, alpha_box, "寫入 RGB 三個 bytes\n忽略 alpha channel", 28)

    arrow(draw, (760, 525), (835, 525), color=BLUE, width=6)
    arrow(draw, (1550, 525), (1625, 525), color=BLUE, width=6)

    header_box = (1740, 250, 2240, 430)
    draw.rounded_rectangle(
        header_box, radius=15, fill=PALE_YELLOW, outline=(150, 126, 40), width=3
    )
    centered(draw, header_box, "P6\nwidth height\n255", 29)
    draw.text((1855, 455), "binary RGB bytes", font=font(27), fill=GRAY)

    out_x, out_y, out_cell = 1770, 535, 105
    for row in range(3):
        for col in range(4):
            draw.rectangle(
                (
                    out_x + col * out_cell,
                    out_y + row * out_cell,
                    out_x + (col + 1) * out_cell,
                    out_y + (row + 1) * out_cell,
                ),
                fill=colors[row][col],
                outline="white",
                width=3,
            )
    draw.text((1765, 880), "輸出結果維持正確的上下方向", font=font(25), fill=GREEN)
    save(image, "ch06_ppm-export-layout.png")


def main() -> None:
    draw_grid_coordinate()
    draw_utf8_caret()
    draw_framebuffer_coordinates()
    draw_ppm_export()


if __name__ == "__main__":
    main()
