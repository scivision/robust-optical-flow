#!/usr/bin/env python
install_requires = ['numpy','pillow','scipy']
tests_require=['nose','coveralls']
#%%
import subprocess
from setuptools import setup,find_packages

setup(name='RobustFlow',
      version= '1.0.0',
      packages=find_packages(),
      author='Michael Hirsch, Ph.D.',
      url='https://github.com/scivision/robust-flow',
      install_requires=install_requires,
      extras_require={'tests':tests_require},
      python_requires='>=3.5',
      tests_require=tests_require,
	  )

subprocess.run(['cmake','../src'],cwd='bin')
subprocess.run(['make'],cwd='bin')
