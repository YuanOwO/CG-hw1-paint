from pathlib import Path

import matplotlib.pyplot as plt
from matplotlib import font_manager

FONT_PATH = "/Users/yuan/Library/Fonts/NotoSansCJKtc-Regular.otf"
font_manager.fontManager.addfont(FONT_PATH)
plt.rcParams["font.family"] = font_manager.FontProperties(fname=FONT_PATH).get_name()
plt.rcParams["axes.unicode_minus"] = False


OUT = Path(__file__).resolve().parents[1] / "figures" / "stroke-cap.png"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig, ax = plt.subplots(figsize=(6.8, 2.7))
    rows = [
        (2.0, "butt", "平頭（BUTT）"),
        (1.0, "projecting", "方頭（SQUARE）"),
        (0.0, "round", "圓頭（ROUND）"),
    ]
    for y, cap, label in rows:
        ax.plot([0.8, 4.7], [y, y], linewidth=18, color="#bcc7d6", solid_capstyle=cap)
        ax.plot([0.8, 4.7], [y, y], linewidth=1.2, color="#2457a6")
        ax.vlines(
            [0.8, 4.7],
            y - 0.32,
            y + 0.32,
            colors="#b33a3a",
            linestyles="dashed",
            linewidth=0.9,
        )
        ax.text(5.35, y, label, va="center", fontsize=10)
    ax.text(0.8, 2.45, "原始端點", ha="center", fontsize=8, color="#b33a3a")
    ax.set_xlim(0.1, 6.2)
    ax.set_ylim(-0.55, 2.7)
    ax.set_aspect("equal")
    ax.axis("off")
    fig.tight_layout()
    fig.savefig(OUT, bbox_inches="tight", dpi=240)


if __name__ == "__main__":
    main()
