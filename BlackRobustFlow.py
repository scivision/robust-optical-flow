#!/usr/bin/env python
import logging
import imageio
from pathlib import Path
import numpy as np
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

    stem = Path(p.pgmstem).expanduser()
    imgfn = stem.parent / (stem.name+f'{p.frames[1]}.pgm')
    img = imageio.imread(imgfn)
    y,x = img.shape

    s = 10
    X = np.arange(0,x,s)
    Y = np.arange(0,y,s)
    X,Y = np.meshgrid(X,Y)

    ax = figure().gca()
    ax.imshow(img,cmap='gray',origin='upper')
    ax.quiver(X, Y,
             u[::s,::s],v[::s,::s])

    ax.set_title(f'{imgfn} robust optical flow')

    show()
