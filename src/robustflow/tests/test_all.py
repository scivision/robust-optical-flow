#!/usr/bin/env python
import logging
from pathlib import Path

from robustflow import runblack, loadflow

stem = Path(__file__).parents[3] / "data/pepsi"


def test_black_robust_optical_flow(tmp_path):

    runblack(stem, [0, 1], tmp_path)

    u, v = loadflow(stem, [0, 1], tmp_path)
    # %%
    if (u[0, 0] == u).all():
        logging.error(f"all elements of U identical {u[0, 0]}")

    if (v[0, 0] == v).all():
        logging.error(f"all elements of V identical {v[0, 0]}")

    assert u[15, 25] == -8
    assert v[15, 25] == -60
