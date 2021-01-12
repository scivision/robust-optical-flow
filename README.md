# Black Robust Optical Flow


Helper code used with
[Michael Black's Robust optical flow code](http://cs.brown.edu/people/black/code.html)

## Build

These are C programs

```sh
cmake -B build

cmake --build build
```

## Examples

![image](results/quiver_pepsi.jpg)

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
Black](http://cs.brown.edu/people/black/Papers/cviu.63.1.1996.html).

* `DemoGNC.sh`: terminal script running the dense robust optical flow code
* `BlackRobustFlow.{m,py}`: Nicer way to call GNC from Matlab with a variety of user-adjustable parameters
