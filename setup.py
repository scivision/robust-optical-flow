#!/usr/bin/env python
req = ['nose','numpy','pillow','scipy']

import pip
try:
    import conda.cli
    conda.cli.main('install',*req)
except Exception:
    pip.main(['install'] + req)
#%%
import subprocess
from setuptools import setup

setup(name='RobustFlow',
      version= '1.0.0',
      packages=['robustflow'],
      author='Michael Hirsch, Ph.D.',
      url='https://github.com/scivision/robust-flow',
	  )

subprocess.run(['cmake','../src'],cwd='bin')
subprocess.run(['make'],cwd='bin')
