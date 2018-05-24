#!/usr/bin/env python
import logging
from pathlib import Path
import numpy as np
from robustflow import runblack,loadflow

stem = Path(__file__).parents[1] / 'data/pepsi'
bindir = Path(__file__).parents[1] / 'bin'

def test_black_robust_optical_flow():

    runblack(stem, bindir, [0,1])

    u,v = loadflow(stem,[0,1])
# %%
    if (u[0,0] == u).all():
        logging.error(f'all elements of U identical {u[0,0]}')

    if (v[0,0] == v).all():
        logging.error(f'all elements of V identical {v[0,0]}')

    assert u[15,25] == -8
    assert v[15,25] == -60

if __name__ == '__main__':
    np.testing.run_module_suite()