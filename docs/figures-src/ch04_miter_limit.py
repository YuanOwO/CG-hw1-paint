from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib import font_manager
from matplotlib.patches import Circle, Polygon

FONT_PATH = "/Users/yuan/Library/Fonts/NotoSansCJKtc-Regular.otf"
font_manager.fontManager.addfont(FONT_PATH)
plt.rcParams["font.family"] = font_manager.FontProperties(fname=FONT_PATH).get_name()
plt.rcParams["axes.unicode_minus"] = False


OUT = Path(__file__).resolve().parents[1] / "figures" / "ch04_miter-limit.png"

STROKE_COLOR = "#cbd3df"
CENTER_COLOR = "#1654c0"
MEASURE_COLOR = "#b51f1f"
WIDTH_COLOR = "#16712d"
GUIDE_COLOR = "#777777"
LINE_WIDTH = 1.2


def unit(vector: np.ndarray) -> np.ndarray:
    return vector / np.linalg.norm(vector)


def left_normal(direction: np.ndarray) -> np.ndarray:
    return np.array([-direction[1], direction[0]])


def cross(a: np.ndarray, b: np.ndarray) -> float:
    return float(a[0] * b[1] - a[1] * b[0])


def line_intersection(
    point_a: np.ndarray,
    direction_a: np.ndarray,
    point_b: np.ndarray,
    direction_b: np.ndarray,
) -> np.ndarray:
    denominator = cross(direction_a, direction_b)
    distance = cross(point_b - point_a, direction_b) / denominator
    return point_a + distance * direction_a


def add_segment(ax, start: np.ndarray, end: np.ndarray, half_width: float) -> None:
    direction = unit(end - start)
    normal = left_normal(direction)
    corners = np.array(
        [
            start + half_width * normal,
            end + half_width * normal,
            end - half_width * normal,
            start - half_width * normal,
        ]
    )
    ax.add_patch(
        Polygon(
            corners, closed=True, facecolor=STROKE_COLOR, edgecolor="none", zorder=1
        )
    )


def draw_case(ax, vertex_angle_deg: float, use_miter: bool) -> None:
    half_width = 0.16
    miter_limit = 4.0
    leg_height = 1.32
    leg_half_span = leg_height * np.tan(np.deg2rad(vertex_angle_deg / 2.0))

    left = np.array([-leg_half_span, -leg_height])
    vertex = np.array([0.0, 0.0])
    right = np.array([leg_half_span, -leg_height])

    incoming = unit(vertex - left)
    outgoing = unit(right - vertex)
    incoming_normal = left_normal(incoming)
    outgoing_normal = left_normal(outgoing)

    outer_in = vertex + half_width * incoming_normal
    outer_out = vertex + half_width * outgoing_normal
    miter_tip = line_intersection(outer_in, incoming, outer_out, outgoing)
    miter_length = float(np.linalg.norm(miter_tip - vertex))
    ratio = miter_length / half_width

    add_segment(ax, left, vertex, half_width)
    add_segment(ax, vertex, right, half_width)

    if use_miter:
        join = np.array([outer_in, miter_tip, outer_out])
        ax.add_patch(
            Polygon(
                join, closed=True, facecolor=STROKE_COLOR, edgecolor="none", zorder=1
            )
        )
        ax.plot(
            join[:, 0], join[:, 1], color=GUIDE_COLOR, linewidth=LINE_WIDTH, zorder=2
        )
    else:
        join = np.array([outer_in, vertex, outer_out])
        ax.add_patch(
            Polygon(
                join, closed=True, facecolor=STROKE_COLOR, edgecolor="none", zorder=1
            )
        )
        ax.plot(
            [outer_in[0], outer_out[0]],
            [outer_in[1], outer_out[1]],
            color=GUIDE_COLOR,
            linewidth=LINE_WIDTH,
            zorder=2,
        )
        ax.plot(
            [outer_in[0], miter_tip[0], outer_out[0]],
            [outer_in[1], miter_tip[1], outer_out[1]],
            color=MEASURE_COLOR,
            linewidth=LINE_WIDTH,
            linestyle=(0, (4, 3)),
            zorder=2,
        )

    ax.plot(
        [left[0], vertex[0], right[0]],
        [left[1], vertex[1], right[1]],
        color=CENTER_COLOR,
        linewidth=LINE_WIDTH,
        zorder=3,
    )

    limit_radius = half_width * miter_limit
    ax.add_patch(
        Circle(
            vertex,
            limit_radius,
            fill=False,
            edgecolor="#999999",
            linewidth=0.9,
            linestyle=(0, (4, 4)),
            zorder=2,
        )
    )
    ax.plot(
        [vertex[0], limit_radius],
        [vertex[1], vertex[1]],
        color=GUIDE_COLOR,
        linewidth=LINE_WIDTH,
        zorder=2,
    )
    ax.text(
        limit_radius * 0.60, -0.07, "hL", color=GUIDE_COLOR, fontsize=9, ha="center"
    )

    arrow_style = dict(arrowstyle="<->", linewidth=LINE_WIDTH, mutation_scale=8)
    ax.annotate(
        "",
        xy=miter_tip,
        xytext=vertex,
        arrowprops={**arrow_style, "color": MEASURE_COLOR},
    )
    ax.text(
        0.055, miter_length * 0.52, "m", color=MEASURE_COLOR, fontsize=10, va="center"
    )

    ax.annotate(
        "", xy=outer_in, xytext=vertex, arrowprops={**arrow_style, "color": WIDTH_COLOR}
    )
    half_width_midpoint = (vertex + outer_in) / 2.0
    ax.text(
        half_width_midpoint[0] - 0.055,
        half_width_midpoint[1] + 0.045,
        "h",
        color=WIDTH_COLOR,
        fontsize=10,
        ha="right",
    )

    ax.scatter([vertex[0]], [vertex[1]], s=12, color=CENTER_COLOR, zorder=4)
    ax.text(0.045, -0.10, "B", color="#222222", fontsize=9)

    comparison = "<=" if use_miter else ">"
    result = "使用尖角接合（MITER）" if use_miter else "改用斜角接合（BEVEL）"
    ax.set_title(
        f"頂點夾角 = {vertex_angle_deg:.0f}°\n"
        f"m/h = {ratio:.1f} {comparison} L = 4：{result}",
        fontsize=9,
        pad=5,
    )
    ax.set_xlim(-0.82, 0.82)
    ax.set_ylim(-1.52, 1.24)
    ax.set_aspect("equal")
    ax.axis("off")


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig, axes = plt.subplots(1, 2, figsize=(7.8, 3.25))

    draw_case(axes[0], vertex_angle_deg=36.0, use_miter=True)
    draw_case(axes[1], vertex_angle_deg=18.0, use_miter=False)

    fig.tight_layout(w_pad=2.0)
    fig.savefig(OUT, bbox_inches="tight", dpi=240)
    plt.close(fig)


if __name__ == "__main__":
    main()
