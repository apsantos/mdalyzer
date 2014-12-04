mdalyzer
========

MD trajectory analyzer

## Installation
Code is tested to compile under (and requires):
* gcc
* Boost >= 1.55.0
* Python 2.7

To install, configure the first four lines of the Makefile to set:
* `CC = g++` (compiler)
* `INSTALL_DIR = bin` (installation path)
* `PYTHON_VERSION = 2.7` (compatible version of python, not recommended to change)
* `BOOST_LIB = /path/to/boost` (path to your Boost libraries)

make && make install will compile and install mdalyzer.

## Example
From a Python script or prompt in the same directory as libmdalyzer.so library, a crude Python script
interfacing with the C++ backend is below:

	# load the module
	import libmdalyzer

	# setup the Trajectory and attach Frames
	t = mdalyzer.Trajectory()
	f1 = mdalyzer.HOOMDXMLFrame("frame1.xml")
	f2 = mdalyzer.HOOMDXMLFrame("frame2.xml")
	t.addFrame(f1)
	t.addFrame(f2)

	# setup the Compute
	dens = mdalyzer.DensityProfile(t,"density",mdalyzer.Vector3uint(11,0,0))
	dens.useMassWeighting(True)
	dens.addType('B')
	dens.addType('A')
	t.addCompute(dens,"density")

	# perform the analysis
	t.analyze()

## Overview
All C++ files can be found in `./libmdalyzer`.
