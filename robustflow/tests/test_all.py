#!/usr/bin/env python
import logging
from pathlib import Path

from robustflow import runblack, loadflow

stem = Path(__file__).parents[2] / "data/pepsi"


def test_black_robust_optical_flow():

    runblack(stem, frameind=[0, 1])

    u, v = loadflow(stem, frameind=[0, 1])
    # %%
    if (u[0, 0] == u).all():
        logging.error(f"all elements of U identical {u[0,0]}")

    if (v[0, 0] == v).all():
        logging.error(f"all elements of V identical {v[0,0]}")

    assert u[15, 25] == -8
    assert v[15, 25] == -60
