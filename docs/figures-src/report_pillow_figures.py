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
    image, draw = canvas(930)
    draw.text(
        (70, 45), "UTF-8 bytes、code point 與 caret 的對應", font=font(42), fill=INK
    )
    panel(draw, (60, 130, 2340, 850), "輸入框以 code point 為編輯單位")

    draw.text((150, 235), "顯示文字", font=font(28), fill=GRAY)
    cells = [("A", 360, 300), ("你", 780, 300), ("B", 1360, 300)]
    widths = [300, 460, 300]
    for (char, x, y), width in zip(cells, widths):
        box = (x, y, x + width, y + 170)
        draw.rounded_rectangle(box, radius=18, fill=PALE_BLUE, outline=BLUE, width=3)
        centered(draw, box, char, 82, fill=BLUE)

    caret_x = [330, 690, 1270, 1690]
    for index, x in enumerate(caret_x):
        draw.line((x, 280, x, 500), fill=RED, width=4)
        draw.text((x - 12, 520), str(index), font=font(25), fill=RED)
    draw.text((1770, 405), "caret index 位於字元之間", font=font(26), fill=RED)

    draw.text((150, 610), "code point", font=font(27), fill=GRAY)
    cp_boxes = [
        ((360, 590, 660, 675), "U+0041"),
        ((780, 590, 1240, 675), "U+4F60"),
        ((1360, 590, 1660, 675), "U+0042"),
    ]
    for box, label in cp_boxes:
        outline = RED if label == "U+4F60" else GREEN
        width = 4 if label == "U+4F60" else 2
        draw.rounded_rectangle(
            box, radius=12, fill=PALE_GREEN, outline=outline, width=width
        )
        centered(draw, box, label, 27)

    draw.text((150, 745), "UTF-8 bytes", font=font(27), fill=GRAY)
    byte_boxes = [
        ((360, 720, 660, 805), "41"),
        ((780, 720, 1240, 805), "E4  BD  A0"),
        ((1360, 720, 1660, 805), "42"),
    ]
    for box, label in byte_boxes:
        draw.rounded_rectangle(
            box, radius=12, fill=PALE_YELLOW, outline=(150, 126, 40), width=2
        )
        centered(draw, box, label, 27)

    draw.multiline_text(
        (1770, 610),
        "Backspace 刪除「你」時\n移除一個 code point，\n對應三個 UTF-8 bytes",
        font=font(26),
        fill=INK,
        spacing=9,
    )
    save(image, "ch05_utf8-caret.png")


def draw_framebuffer_coordinates() -> None:
    image, draw = canvas(1020)
    draw.text((70, 45), "UI 座標與 OpenGL framebuffer 座標", font=font(42), fill=INK)
    panel(draw, (60, 130, 1120, 940), "UI／Window：左上角為原點")
    panel(draw, (1280, 130, 2340, 940), "OpenGL：左下角為原點")

    ui = (210, 285, 940, 790)
    gl = (1430, 285, 2160, 790)
    draw.rectangle(ui, fill="white", outline=GRAY, width=4)
    draw.rectangle(gl, fill="white", outline=GRAY, width=4)

    capture_ui = (410, 410, 790, 650)
    capture_gl = (1630, 425, 2010, 665)
    draw.rectangle(capture_ui, fill=PALE_BLUE, outline=BLUE, width=4)
    draw.rectangle(capture_gl, fill=PALE_BLUE, outline=BLUE, width=4)

    draw.ellipse((203, 278, 217, 292), fill=RED)
    arrow(draw, (210, 285), (320, 285), color=RED, width=4)
    arrow(draw, (210, 285), (210, 395), color=RED, width=4)
    draw.text((325, 268), "X", font=font(24), fill=RED)
    draw.text((190, 398), "Y", font=font(24), fill=RED)
    draw.text((170, 245), "(0, 0)", font=font(23), fill=RED)

    draw.ellipse((1423, 783, 1437, 797), fill=GREEN)
    arrow(draw, (1430, 790), (1540, 790), color=GREEN, width=4)
    arrow(draw, (1430, 790), (1430, 680), color=GREEN, width=4)
    draw.text((1545, 774), "X", font=font(24), fill=GREEN)
    draw.text((1408, 645), "Y", font=font(24), fill=GREEN)
    draw.text((1390, 805), "(0, 0)", font=font(23), fill=GREEN)

    draw.text((425, 430), "擷取區域", font=font(27), fill=BLUE)
    draw.multiline_text(
        (425, 485),
        "左上角：(x, y)\n尺寸：width × height",
        font=font(25),
        fill=INK,
        spacing=8,
    )
    draw.text((1650, 445), "相同的 framebuffer 區域", font=font(25), fill=BLUE)
    draw.text((1645, 600), "底部座標：(x, bottom)", font=font(24), fill=INK)

    arrow(draw, (970, 520), (1245, 520), color=BLUE, width=6)
    formula = (820, 715, 1375, 895)
    draw.rounded_rectangle(
        formula, radius=18, fill=PALE_YELLOW, outline=(150, 126, 40), width=3
    )
    centered(draw, formula, "bottom = windowHeight - y - height", 28)

    for index, color in enumerate(
        [(221, 235, 255), (190, 215, 252), (145, 184, 239), (94, 143, 219)]
    ):
        y0 = capture_gl[1] + index * 60
        draw.rectangle(
            (capture_gl[0], y0, capture_gl[2], y0 + 60),
            fill=color,
            outline="white",
            width=2,
        )
        draw.text((2025, y0 + 13), f"row {3 - index}", font=font(21), fill=INK)
    draw.text(
        (1575, 835), "glReadPixels() 由下往上存入 rows", font=font(25), fill=GREEN
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
