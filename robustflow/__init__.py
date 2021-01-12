from pathlib import Path
import imageio
import shutil
import subprocess
import numpy as np
import typing as T

src = Path(__file__).parents[1]
bindir = src / "build"


def build(src: Path, bindir: Path):
    subprocess.check_call(["cmake", f"-S{src}", f"-B{bindir}"])
    subprocess.check_call(["cmake", "--build", str(bindir)])


def runblack(stem: Path, frameind: T.Tuple[int, int], outpath: Path):
    stem = Path(stem).expanduser()
    outpath = Path(outpath).expanduser()
    stemname = stem.name

    fn = stem.parent / (stemname + str(frameind[0]) + ".pgm")

    rc = imageio.imread(fn).shape

    exe = bindir / "gnc"
    if not shutil.which(exe):
        build(src, bindir)

    for i in range(frameind[0], frameind[1]):
        outstem = outpath / (stemname + str(i + 1) + "-")

        cmd = [
            str(exe),
            str(i),
            str(i + 1),
            str(4),
            str(1),
            str(stem),
            str(outstem),
            "-l1",
            str(10.0),
            "-l2",
            str(1.0),
            "-S1",
            str(10.0),
            "-S2",
            str(1.0),
            "-s1",
            str(10.0),
            "-s2",
            str(0.05),
            "-stages",
            str(5),
            "-nx",
            str(rc[1]),
            "-ny",
            str(rc[0]),
            "-f",
            str(0.5),
            "-F",
            str(0),
            "-w",
            str(1.995),
            "-iters",
            str(20),
            "-end",
            ".pgm",
            "-skip",
            str(15)
            # 15 is # of header bytes for PGM
        ]
        print(" ".join(cmd))
        subprocess.run(
            cmd
        )  # NOTE: not check_call as GNC returns 5 on older C compilers.


def loadflow(stem: Path, frameind: T.Tuple[int, int], outpath: Path):
    stem = Path(stem).expanduser()
    outpath = Path(outpath).expanduser()
    outname = stem.name

    bzero = 128

    u = []
    v = []
    for i in range(frameind[0], frameind[1]):
        outstem = outname + str(i + 1) + "-"
        ufn = outpath / (outstem + "u-4.pgm")
        vfn = outpath / (outstem + "v-4.pgm")
        # NOTE: have to upcast by one size (int16) to account for initial uint8
        # values that would overflow int8. RAM is cheap.
        u.append(imageio.imread(ufn).astype("int16") - bzero)
        v.append(imageio.imread(vfn).astype("int16") - bzero)

    return np.array(u).squeeze(), np.array(v).squeeze()
