#!/usr/bin/env python
import logging
from robustflow import runblack,loadflow
from matplotlib.pyplot import figure,show

if __name__ == '__main__':
    from argparse import ArgumentParser
    p = ArgumentParser()
    p.add_argument('pgmstem',help='stem of pgm files')
    p.add_argument('-f','--frames',help='start stop frame indices',nargs=2,type=int,default=[0,1])
    p.add_argument('--srcpath',help='path to C code and EXE',default='bin')
    p.add_argument('-o','--outpath',default='results')
    p = p.parse_args()

    runblack(p.pgmstem, p.srcpath, p.frames, p.outpath)

    u,v = loadflow(p.pgmstem, p.outpath, p.frames)
# %%
    if (u[0,0] == u).all():
        logging.error(f'all elements of U identical {u[0,0]}')

    if (v[0,0] == v).all():
        logging.error(f'all elements of V identical {v[0,0]}')

    ax = figure().gca()
    ax.quiver(u,v)

    show()
