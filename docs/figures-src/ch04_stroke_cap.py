from pathlib import Path

import matplotlib.pyplot as plt


OUT = Path(__file__).resolve().parents[1] / "figures" / "ch04_stroke-cap.png"


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig, ax = plt.subplots(figsize=(7, 3.5))
    rows = [
        (2.0, "butt", "BUTT"),
        (1.0, "projecting", "SQUARE"),
        (0.0, "round", "ROUND"),
    ]
    for y, cap, label in rows:
        # 先畫邊界，再畫內部填色，形成與 Stroke expansion 相同的樣式。
        ax.plot(
            [0.8, 4.7],
            [y, y],
            linewidth=24,
            color="#64748b",
            solid_capstyle=cap,
        )
        ax.plot(
            [0.8, 4.7],
            [y, y],
            linewidth=21,
            color="#d9e1ec",
            solid_capstyle=cap,
        )
        ax.plot([0.8, 4.7], [y, y], linewidth=1.5, color="#1757ad")
        ax.vlines(
            [0.8, 4.7],
            y - 0.30,
            y + 0.30,
            colors="#b42318",
            linestyles="dashed",
            linewidth=0.9,
        )
        ax.text(5.35, y, label, va="center")
    ax.set_xlim(0.1, 6.2)
    ax.set_ylim(-0.55, 2.55)
    ax.set_aspect("equal")
    ax.axis("off")
    fig.tight_layout()
    fig.savefig(OUT, bbox_inches="tight", dpi=240)
    plt.close(fig)


if __name__ == "__main__":
    main()
