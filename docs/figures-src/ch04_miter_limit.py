from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.patches import Arc, Circle, Polygon


OUT = Path(__file__).resolve().parents[1] / "figures" / "ch04_miter-limit.png"

STROKE_COLOR = "#d9e1ec"
BOUNDARY_COLOR = "#64748b"
CENTER_COLOR = "#1757ad"
MEASURE_COLOR = "#b42318"
WIDTH_COLOR = "#16712d"
GUIDE_COLOR = "#64748b"


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


def stroke_outline(
    vertex_angle_deg: float,
    half_width: float,
    leg_length: float,
    use_miter: bool,
):
    """Return one completed stroke polygon and the join construction points."""
    half_angle = np.deg2rad(vertex_angle_deg / 2.0)

    # A -> B -> C；內角 theta 位於 B 下方。
    B = np.array([0.0, 0.0])
    A = leg_length * np.array([-np.sin(half_angle), -np.cos(half_angle)])
    C = leg_length * np.array([np.sin(half_angle), -np.cos(half_angle)])

    d_ab = unit(B - A)
    d_bc = unit(C - B)
    n_ab = left_normal(d_ab)
    n_bc = left_normal(d_bc)

    # 右轉時，兩條 segment 的 left boundary 是轉角外側。
    O1 = B + half_width * n_ab
    O2 = B + half_width * n_bc
    M = line_intersection(O1, d_ab, O2, d_bc)

    # 內側兩條 offset boundary 的交點。
    inner_ab = B - half_width * n_ab
    inner_bc = B - half_width * n_bc
    I = line_intersection(inner_ab, d_ab, inner_bc, d_bc)

    A_outer = A + half_width * n_ab
    A_inner = A - half_width * n_ab
    C_outer = C + half_width * n_bc
    C_inner = C - half_width * n_bc

    if use_miter:
        # MITER 外輪廓：... -> O1 -> M -> O2 -> ...
        boundary = np.array(
            [A_outer, O1, M, O2, C_outer, C_inner, I, A_inner]
        )
    else:
        # BEVEL 外輪廓：... -> O1 -> O2 -> ...，不包含 O1-M-O2 區域。
        boundary = np.array(
            [A_outer, O1, O2, C_outer, C_inner, I, A_inner]
        )

    return {
        "A": A,
        "B": B,
        "C": C,
        "d_ab": d_ab,
        "n_ab": n_ab,
        "O1": O1,
        "O2": O2,
        "M": M,
        "boundary": boundary,
    }


def draw_case(ax, vertex_angle_deg: float, use_miter: bool) -> None:
    h = 1.0
    miter_limit = 4.0
    geometry = stroke_outline(
        vertex_angle_deg=vertex_angle_deg,
        half_width=h,
        leg_length=7.4,
        use_miter=use_miter,
    )

    A = geometry["A"]
    B = geometry["B"]
    C = geometry["C"]
    O1 = geometry["O1"]
    O2 = geometry["O2"]
    M = geometry["M"]

    # 完成 Join 後的最終 Stroke：只畫一個 polygon 與一個外框。
    ax.add_patch(
        Polygon(
            geometry["boundary"],
            closed=True,
            facecolor=STROKE_COLOR,
            edgecolor=BOUNDARY_COLOR,
            linewidth=1.5,
            zorder=1,
        )
    )

    # 藍線只表示中心線 A-B-C。
    ax.plot(
        [A[0], B[0], C[0]],
        [A[1], B[1], C[1]],
        color=CENTER_COLOR,
        linewidth=1.7,
        zorder=3,
    )

    # Miter limit：以 B 為圓心、hL 為半徑。
    ax.add_patch(
        Circle(
            B,
            h * miter_limit,
            fill=False,
            edgecolor=GUIDE_COLOR,
            linewidth=1.0,
            linestyle=(0, (4, 4)),
            zorder=2,
        )
    )
    ax.text(
        h * miter_limit * 0.72,
        0.18,
        "$hL$",
        color=GUIDE_COLOR,
        ha="center",
        fontsize=10,
    )

    # BEVEL 只保留理論 MITER 的虛線，不填滿 O1-M-O2。
    if not use_miter:
        ax.plot(
            [O1[0], M[0], O2[0]],
            [O1[1], M[1], O2[1]],
            color=MEASURE_COLOR,
            linewidth=1.2,
            linestyle=(0, (4, 3)),
            zorder=4,
        )

    ax.scatter(
        [O1[0], O2[0]],
        [O1[1], O2[1]],
        s=18,
        color=BOUNDARY_COLOR,
        zorder=5,
    )
    ax.scatter([M[0]], [M[1]], s=20, color=MEASURE_COLOR, zorder=5)

    ax.text(O1[0] - 0.15, O1[1] + 0.28, "$O_1$", ha="right", fontsize=9)
    ax.text(O2[0] + 0.15, O2[1] + 0.28, "$O_2$", ha="left", fontsize=9)
    ax.text(M[0] + 0.18, M[1], "$M$", color=MEASURE_COLOR, va="center", fontsize=10)
    ax.text(B[0] + 0.14, B[1] - 0.45, "$B$", fontsize=9)

    # Miter length m：沿角平分線由 B 指向 M。
    ax.annotate(
        "",
        xy=M,
        xytext=B,
        arrowprops=dict(
            arrowstyle="<->",
            color=MEASURE_COLOR,
            linewidth=1.5,
            mutation_scale=9,
        ),
        zorder=6,
    )
    ax.text(
        0.20,
        M[1] * 0.55,
        "$m$",
        color=MEASURE_COLOR,
        fontsize=11,
        va="center",
    )

    # h：從 AB 中的一點沿該 segment 的法向量量到 Stroke boundary。
    sample = A + 0.38 * (B - A)
    sample_boundary = sample + h * geometry["n_ab"]
    ax.annotate(
        "",
        xy=sample_boundary,
        xytext=sample,
        arrowprops=dict(
            arrowstyle="<->",
            color=WIDTH_COLOR,
            linewidth=1.5,
            mutation_scale=9,
        ),
        zorder=6,
    )
    h_midpoint = (sample + sample_boundary) / 2.0
    ax.text(
        h_midpoint[0] - 0.18,
        h_midpoint[1] + 0.18,
        "$h$",
        color=WIDTH_COLOR,
        fontsize=11,
        ha="right",
    )

    # theta：畫在 B 的內側、兩條中心線之間。
    arc_radius = 1.35
    ax.add_patch(
        Arc(
            B,
            2 * arc_radius,
            2 * arc_radius,
            angle=0,
            theta1=270.0 - vertex_angle_deg / 2.0,
            theta2=270.0 + vertex_angle_deg / 2.0,
            color="#333333",
            linewidth=1.0,
            zorder=5,
        )
    )
    ax.text(0.0, -1.58, r"$\theta$", ha="center", va="top", fontsize=11)

    ratio = np.linalg.norm(M - B) / h
    comparison = r"\leq" if use_miter else ">"
    result = "MITER" if use_miter else "BEVEL"
    ax.text(
        0,
        -8.0,
        rf"$\theta={vertex_angle_deg:.0f}^\circ$, $h=1$, $L=4$"
        + "\n"
        + rf"$m/h\approx{ratio:.1f} {comparison} 4$  ({result})",
        ha="center",
        va="top",
        fontsize=9,
    )

    ax.set_xlim(-5.1, 5.1)
    ax.set_ylim(-8.8, 7.0)
    ax.set_aspect("equal")
    ax.axis("off")


def main() -> None:
    OUT.parent.mkdir(parents=True, exist_ok=True)
    fig, axes = plt.subplots(1, 2, figsize=(8.4, 5.4))

    draw_case(axes[0], vertex_angle_deg=36.0, use_miter=True)
    draw_case(axes[1], vertex_angle_deg=18.0, use_miter=False)

    fig.tight_layout(w_pad=1.2)
    fig.savefig(OUT, bbox_inches="tight", dpi=240)
    plt.close(fig)


if __name__ == "__main__":
    main()
