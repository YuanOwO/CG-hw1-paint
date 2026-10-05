from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import PathPatch
from matplotlib.path import Path as MplPath


OUT = Path(__file__).resolve().parents[1] / "figures" / "ch04_stroke-join.png"


def draw_join(ax, join_style: str, label: str) -> None:
    centerline = np.array([[-1.15, -0.75], [0.0, 0.8], [1.15, -0.75]])
    path = MplPath(centerline)

    # 外層邊界與內部填色分開繪製，對應 expansion 圖的 Polygon 樣式。
    border = PathPatch(
        path,
        fill=False,
        linewidth=24,
        edgecolor="#64748b",
        capstyle="butt",
        joinstyle=join_style,
    )
    fill = PathPatch(
        path,
        fill=False,
        linewidth=21,
        edgecolor="#d9e1ec",
        capstyle="butt",
        joinstyle=join_style,
    )
    ax.add_patch(border)
    ax.add_patch(fill)
    ax.plot(centerline[:, 0], centerline[:, 1], color="#1757ad", linewidth=1.5)
    ax.scatter([0], [0.8], s=16, color="#52677f", zorder=5)
    ax.text(0, -1.08, label, ha="center", va="top")
    ax.set_xlim(-1.45, 1.45)
    ax.set_ylim(-1.30, 1.25)
    ax.set_aspect("equal")
    ax.axis("off")


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig, axes = plt.subplots(1, 3, figsize=(7, 3.5))
    for ax, style, label in zip(
        axes,
        ("miter", "bevel", "round"),
        ("MITER", "BEVEL", "ROUND"),
    ):
        draw_join(ax, style, label)
    fig.tight_layout(w_pad=1.4)
    fig.savefig(OUT, bbox_inches="tight", dpi=240)
    plt.close(fig)


if __name__ == "__main__":
    main()
