.. image:: https://travis-ci.org/scivision/robust-optical-flow.svg?branch=master
    :target: https://travis-ci.org/scivision/robust-optical-flow

===========
robust-flow
===========

Helper code used with `Michael Black's Robust optical flow code <http://cs.brown.edu/people/black/code.html>`_


Install
=======
You will need:

* C compiler (e.g. ``gcc`` or ``clang``)
* `CMake <https://www.scivision.co/easy-setup-of-cmake/>`_


.. code:: bash

    cd bin
    cmake ../src
    make
    
    make test


Windows: ``cmake -G "MinGW Makefiles" ../src``

Examples
========
You will see this plot:

.. image:: results/quiver_pepsi.jpg


Python
------
::

    python BlackRobustFlow.py data/pepsi

Matlab/Octave
-------------

.. code:: matlab

    [u,v] = BlackRobustFlow('data/pepsi');
    
    
Note, Octave 4.2.2 has a ``quiver()`` plotting bug (in general). 
Matlab R2018a worked fine.


Functions
=========
GNC is the C program used for 
`Robust Estimation of Dense Optical Flow by Michael Black <http://cs.brown.edu/people/black/Papers/cviu.63.1.1996.html>`_.

* ``DemoGNC.sh``:   terminal script running the dense robust optical flow code
* ``BlackRobustFlow.{m,py}``:     Nicer way to call GNC from Matlab with a variety of user-adjustable parameters

