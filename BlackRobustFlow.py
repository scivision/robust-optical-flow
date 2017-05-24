#!/usr/bin/env python
from pathlib import Path
from numpy import int16
from scipy.ndimage import imread
import subprocess as S
from matplotlib.pyplot import figure,draw,pause

def runblack(stem,srcpath,frameind,outpath):
    stem = Path(stem).expanduser()
    stemname = stem.name
    
    srcpath = Path(srcpath).expanduser()

    fn = stem.parent / (stemname + str(frameind[0]) + '.pgm')

    I = imread(fn)
    rc = I.shape

    for i in range(frameind[0],frameind[1]):
        outstem = Path(outpath) / (stemname + str(i+1)+'-')

        cmd = [str(srcpath / 'gnc'),
                   str(i), str(i+1),
                   str(4),str(1),
                   str(stem),str(outstem),
                   '-l1', str(10.),'-l2',str(1.),
                   '-S1', str(10.),'-S2',str(1.),
                   '-s1', str(10.),'-s2',str(.05),
                   '-stages', str(5),
                   '-nx', str(rc[1]), '-ny', str(rc[0]),
                   '-f', str(.5),'-F',str(0),
                   '-w', str(1.995), '-iters', str(20),
                   '-end','.pgm', '-skip', str(15)
                   #15 is # of header bytes for PGM
                   ]
        print(' '.join(cmd))
        S.check_call(cmd)


def loadflow(stem,outpath, frameind):
    stem = Path(stem).expanduser()
    outname = stem.name

    bzero = 128

#    ax = figure().gca()
    for i in range(frameind[0],frameind[1]):
        outstem = str(Path(outpath) / (outname + str(i+1)+'-'))
        ufn = outstem + 'u-4.pgm'
        vfn = outstem + 'v-4.pgm'
# NOTE: have to upcast by one size (int16) to account for initial uint8
# values that would overflow int8. RAM is cheap.
        u = imread(ufn).astype(int16) - bzero;
        v = imread(vfn).astype(int16) - bzero;

    return u,v


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
