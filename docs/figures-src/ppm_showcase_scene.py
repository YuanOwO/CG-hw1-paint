import math, sys
W, H = 720, 440
objs = []

def style(fill_mode="advanced", ps=1, fill=(0,0,0,0), w=1, color=(0,0,0,1), join="miter", cap="round", ml=4):
    f = " ".join(f"{c:g}" for c in fill); c = " ".join(f"{x:g}" for x in color)
    return f"SHAPE_STYLE {fill_mode} {ps:g}\nFILL {f}\nSTROKE {w:g} {c} {join} {cap} {ml:g}\n"

def two(kind, a, b, **kw):
    objs.append(f"OBJECT shape\nSHAPE {kind}\n{style(**kw)}BOUNDS {a[0]:g} {a[1]:g} {b[0]:g} {b[1]:g}\nEND_OBJECT\n")

def pts(kind, ps_, **kw):
    body = "".join(f"POINT {x:g} {y:g}\n" for x, y in ps_)
    objs.append(f"OBJECT shape\nSHAPE {kind}\n{style(**kw)}POINTS {len(ps_)}\n{body}END_OBJECT\n")

def point(p, size, color):
    objs.append(f"OBJECT shape\nSHAPE point\n{style(ps=size, color=color)}POSITION {p[0]:g} {p[1]:g}\nEND_OBJECT\n")

def text(p, s, color=(0.1,0.1,0.1,1), font="gfnt unifont-16"):
    b = s.encode()
    c = " ".join(f"{x:g}" for x in color)
    objs.append(f"OBJECT text\nPOSITION {p[0]:g} {p[1]:g}\nTEXT_STYLE {c} 1\nFONT {font}\nTEXT {len(b)}\n{s}\nEND_OBJECT\n")

# ---- 左半：插畫（填色 + 外框） ----
two("rectangle", (20, 20), (350, 420), fill=(0.86, 0.93, 0.99, 1), w=2, color=(0.55, 0.68, 0.82, 1))       # 天空
two("ellipse", (250, 50), (320, 120), fill=(1, 0.83, 0.3, 1), w=4, color=(0.95, 0.55, 0.15, 1))             # 太陽
pts("polygon", [(20, 330), (120, 230), (230, 330)], fill=(0.55, 0.78, 0.47, 1), w=3, color=(0.25, 0.5, 0.25, 1), join="round")
two("rectangle", (20, 330), (350, 420), fill=(0.45, 0.7, 0.38, 1), w=2, color=(0.25, 0.5, 0.25, 1))         # 草地
two("rectangle", (175, 250), (305, 345), fill=(0.97, 0.92, 0.82, 1), w=4, color=(0.45, 0.3, 0.2, 1), join="miter")  # 房子
pts("polygon", [(160, 252), (240, 185), (320, 252)], fill=(0.82, 0.28, 0.24, 1), w=4, color=(0.5, 0.15, 0.12, 1), join="miter")  # 屋頂
two("rectangle", (225, 290), (257, 345), fill=(0.55, 0.36, 0.22, 1), w=3, color=(0.4, 0.25, 0.15, 1))       # 門
two("rectangle", (188, 268), (214, 292), fill=(0.7, 0.86, 1, 1), w=3, color=(0.45, 0.3, 0.2, 1))            # 窗
point((250, 318), 5, (0.95, 0.8, 0.3, 1))                                                                     # 門把
# 雲（兩個橢圓，只有填色）
two("ellipse", (50, 60), (130, 100), fill=(1, 1, 1, 1), w=2, color=(0.75, 0.82, 0.9, 1))
two("ellipse", (90, 48), (160, 92), fill=(1, 1, 1, 1), w=2, color=(0.75, 0.82, 0.9, 1))
text((36, 400), "Graphtoria 電腦圖學 HW1", color=(1, 1, 1, 1))

# ---- 右半：Stroke Join / Cap 示範 ----
text((380, 44), "Line Join / Line Cap", font="bitmap helvetica-18")
zig = lambda x0, y0: [(x0, y0 + 50), (x0 + 30, y0), (x0 + 60, y0 + 50), (x0 + 90, y0)]
for i, (join, cap, col, name) in enumerate([
        ("miter", "butt",   (0.16, 0.38, 0.75, 1), "Miter + Butt"),
        ("bevel", "square", (0.55, 0.25, 0.7, 1),  "Bevel + Square"),
        ("round", "round",  (0.1, 0.55, 0.55, 1),  "Round + Round")]):
    x0 = 385 + i * 112
    pts("path", zig(x0, 75), w=14, color=col, join=join, cap=cap)
    pts("path", zig(x0, 75), w=1, color=(1, 0.3, 0.3, 1))      # 中心線
    text((x0 - 4, 158), name, font="bitmap helvetica-12")

# Pencil 風格的曲線
wave = [(385 + t * 3, 230 + 28 * math.sin(t / 8.0)) for t in range(0, 105)]
pts("path", wave, w=8, color=(0.9, 0.45, 0.2, 1), join="round", cap="round")

# 只有外框的橢圓與點
two("ellipse", (385, 290), (505, 380), w=5, color=(0.35, 0.35, 0.8, 1))
for i, (sz, col) in enumerate([(4, (0.9,0.3,0.3,1)), (8, (0.95,0.6,0.2,1)), (12, (0.3,0.7,0.35,1)), (16, (0.25,0.45,0.85,1))]):
    point((540 + i * 40, 335), sz, col)
text((385, 412), "PPM P6 輸出：不含 Grid 與狀態列")

out = sys.argv[1]
with open(out, "w") as f:
    f.write(f"GPTD 1\nCANVAS {W} {H}\nOBJECTS {len(objs)}\n" + "".join(objs) + "END\n")
