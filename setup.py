#!/usr/bin/env python
install_requires = ['numpy','imageio']
tests_require=['pytest','nose','coveralls']
#%%
import subprocess
from setuptools import setup,find_packages

setup(name='RobustFlow',
      version= '1.1.0',
      packages=find_packages(),
      author='Michael Hirsch, Ph.D.',
      url='https://github.com/scivision/robust-flow',
      description='Michael Black robust optical flow estimation',
      long_description=open('README.rst').read(),
      install_requires=install_requires,
      extras_require={'tests':tests_require},
      python_requires='>=3.5',
      tests_require=tests_require,
	  )

subprocess.check_call(['cmake','../src'], cwd='bin')
subprocess.check_call(['make'], cwd='bin')
