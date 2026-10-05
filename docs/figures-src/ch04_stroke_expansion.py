import math
from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

OUT = Path(__file__).resolve().parents[1] / "figures" / "ch04_stroke-expansion.png"


def main():
    # 中心線 A -> B
    A = (1.0, 1.0)
    B = (5.0, 2.5)

    # Stroke Width 的一半
    h = 0.45

    # 1. 求 A -> B 的單位方向 u
    dx = B[0] - A[0]
    dy = B[1] - A[1]
    length = math.hypot(dx, dy)

    ux = dx / length
    uy = dy / length

    # 2. 將方向旋轉 90 度，得到垂直方向 n
    nx = -uy
    ny = ux

    # 3. A、B 分別沿 n 的正負方向偏移 h
    A_left = (A[0] + h * nx, A[1] + h * ny)
    A_right = (A[0] - h * nx, A[1] - h * ny)
    B_left = (B[0] + h * nx, B[1] + h * ny)
    B_right = (B[0] - h * nx, B[1] - h * ny)

    fig, ax = plt.subplots(figsize=(7, 3.5))

    # 粗線實際形成的四邊形
    ax.add_patch(
        Polygon(
            [A_left, B_left, B_right, A_right],
            facecolor="#d9e1ec",
            edgecolor="#64748b",
        )
    )

    # 原始中心線
    ax.plot(
        [A[0], B[0]],
        [A[1], B[1]],
        color="#1757ad",
        linewidth=1.5,
    )

    # 左右邊界
    ax.plot(
        [A_left[0], B_left[0]],
        [A_left[1], B_left[1]],
        color="#52677f",
    )
    ax.plot(
        [A_right[0], B_right[0]],
        [A_right[1], B_right[1]],
        color="#52677f",
    )

    # 在中心位置畫出半寬 h
    P = (
        (A[0] + B[0]) / 2,
        (A[1] + B[1]) / 2,
    )
    P_left = (P[0] + h * nx, P[1] + h * ny)
    P_right = (P[0] - h * nx, P[1] - h * ny)

    ax.annotate(
        "",
        xy=P_left,
        xytext=P,
        arrowprops=dict(arrowstyle="->", color="#b42318"),
    )
    ax.annotate(
        "",
        xy=P_right,
        xytext=P,
        arrowprops=dict(arrowstyle="->", color="#b42318"),
    )

    # 標示中心線端點與展開後的四個邊界頂點
    ax.scatter(
        [A_left[0], A_right[0], B_left[0], B_right[0]],
        [A_left[1], A_right[1], B_left[1], B_right[1]],
        color="#52677f",
        s=16,
        zorder=3,
    )
    ax.text(A[0] - 0.15, A[1] - 0.2, "$A$")
    ax.text(B[0] + 0.1, B[1], "$B$")
    ax.text(A_left[0] - 0.35, A_left[1] + 0.05, "$A_L$")
    ax.text(A_right[0] - 0.35, A_right[1] - 0.18, "$A_R$")
    ax.text(B_left[0] + 0.10, B_left[1] + 0.05, "$B_L$")
    ax.text(B_right[0] + 0.10, B_right[1] - 0.18, "$B_R$")
    ax.text(P_left[0] - 0.35, P_left[1] + 0.1, r"$+h\vec{n}$")
    ax.text(P_right[0] + 0.1, P_right[1] - 0.15, r"$-h\vec{n}$")

    ax.set_aspect("equal")
    ax.axis("off")

    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(OUT, bbox_inches="tight", dpi=240)
    plt.close(fig)


if __name__ == "__main__":
    main()
