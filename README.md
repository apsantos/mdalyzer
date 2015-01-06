mdalyzer
========

MD trajectory analyzer

## Installation
Code is tested to compile under (and requires):
* gcc
* Boost >= 1.55.0
* Python 2.7
* Doxygen 1.8.2 (optional)

To install, configure the first four lines of the Makefile to set:
* `CC = g++` (compiler)
* `INSTALL_PATH = bin` (installation path)
* `BUILD_PATH = build` (build directory)
* `PYTHON_VERSION = 2.7` (compatible version of python, not recommended to change)
* `BOOST_LIB = /path/to/boost` (path to your Boost libraries)

make && make install will compile and install mdalyzer.

## Example
With the mdalyzer installation path also on the PYTHONPATH, an example script for computing the density profile is:

	from mdalyzer import *

	t = trajectory.hoomd()
	t.add(['frame1.xml','frame2.xml'])

	analyzer.density(traj=t, file_name='density', nx=11, types=['A','B'], weight=False)

	t.analyze()

## Overview
All C++ files can be found in `./libmdalyzer`. The Python wrapper module can be found `./mdalyzer`
