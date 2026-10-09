# Black Robust Optical Flow


Helper code used with
[Michael Black's Robust optical flow code](https://cs.brown.edu/people/mjblack/code.html).

We also provide
[updated and corrected Matlab code](./flow_code_2010/)
from the Sun, Roth, and Black
[2010 paper](http://www.cs.brown.edu/people/dqsun/pubs/cvpr_2010_flow.pdf)
"Secrets of optical flow estimation and their principles" *IEEE Conf. on Computer Vision and Pattern Recog., CVPR, June 2010*.

![image](results/quiver_pepsi.jpg)

## Build

These are C programs

```sh
cmake -B build

cmake --build build
```

## Examples


### Python

```sh
python BlackRobustFlow.py data/pepsi
```

### Matlab

```matlab
[u,v] = BlackRobustFlow('data/pepsi');
```

## Functions

GNC is the C program used for [Robust Estimation of Dense Optical Flow
by Michael
[Black](https://cs.brown.edu/people/mjblack/Papers/cviu.63.1.1996.html).

* `DemoGNC.sh`: terminal script running the dense robust optical flow code
* `BlackRobustFlow.{m,py}`: call GNC from Matlab or Python with user-adjustable parameters
