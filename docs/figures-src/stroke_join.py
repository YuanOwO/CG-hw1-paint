from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib import font_manager
from matplotlib.patches import PathPatch
from matplotlib.path import Path as MplPath

FONT_PATH = "/Users/yuan/Library/Fonts/NotoSansCJKtc-Regular.otf"
font_manager.fontManager.addfont(FONT_PATH)
plt.rcParams["font.family"] = font_manager.FontProperties(fname=FONT_PATH).get_name()
plt.rcParams["axes.unicode_minus"] = False


OUT = Path(__file__).resolve().parents[1] / "figures" / "stroke-join.png"


def draw_join(ax, join_style: str, title: str) -> None:
    centerline = np.array([[-1.15, -0.75], [0.0, 0.8], [1.15, -0.75]])
    path = MplPath(centerline)
    patch = PathPatch(
        path,
        fill=False,
        linewidth=18,
        edgecolor="#bcc7d6",
        capstyle="butt",
        joinstyle=join_style,
    )
    ax.add_patch(patch)
    ax.plot(centerline[:, 0], centerline[:, 1], color="#2457a6", linewidth=1.2)
    ax.scatter([0], [0.8], s=13, color="#2457a6", zorder=5)
    ax.set_title(title, fontsize=10, pad=4)
    ax.set_xlim(-1.45, 1.45)
    ax.set_ylim(-1.05, 1.55)
    ax.set_aspect("equal")
    ax.axis("off")


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig, axes = plt.subplots(1, 3, figsize=(7.2, 2.35))
    for ax, style, title in zip(
        axes,
        ("miter", "bevel", "round"),
        ("尖角接合（MITER）", "斜角接合（BEVEL）", "圓角接合（ROUND）"),
    ):
        draw_join(ax, style, title)
    fig.suptitle("線段接合幾何（藍線為中心線）", fontsize=10)
    fig.tight_layout(rect=(0, 0, 1, 0.9), w_pad=1.4)
    fig.savefig(OUT, bbox_inches="tight", dpi=240)


if __name__ == "__main__":
    main()
