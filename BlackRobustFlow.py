#!/usr/bin/env python3
import logging
from pathlib import Path


import imageio
import numpy as np

from robustflow import runblack, loadflow

from matplotlib.pyplot import figure, show


def main(stem: Path, frames, outpath: Path):
    stem = Path(stem).expanduser()

    runblack(stem, frames, outpath)

    u, v = loadflow(stem, frames, outpath)
    # %%
    if (u[0, 0] == u).all():
        logging.error(f"all elements of U identical {u[0, 0]}")

    if (v[0, 0] == v).all():
        logging.error(f"all elements of V identical {v[0, 0]}")

    imgfn = stem.parent / (stem.name + f"{frames[1]}.pgm")
    img = imageio.imread(imgfn)
    y, x = img.shape

    s = 10
    X = np.arange(0, x, s)
    Y = np.arange(0, y, s)
    X, Y = np.meshgrid(X, Y)

    if figure is None:
        return

    ax = figure().gca()
    ax.imshow(img, cmap="gray", origin="upper")
    ax.quiver(X, Y, u[::s, ::s], v[::s, ::s])

    ax.set_title(f"{imgfn} robust optical flow")

    show()


if __name__ == "__main__":
    from argparse import ArgumentParser

    p = ArgumentParser()
    p.add_argument("pgmstem", help="stem of pgm files")
    p.add_argument(
        "-f",
        "--frames",
        help="start stop frame indices",
        nargs=2,
        type=int,
        default=[0, 1],
    )
    p.add_argument("-o", "--outpath", default="results")
    args = p.parse_args()

    main(args.pgmstem, args.frames, args.outpath)
