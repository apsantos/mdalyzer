mdalyzer
========

MD trajectory analyzer

installation
============
make && make install will install the mdalyzer into path specified in Makefile (default=./bin)

example
=======
From a Python script or prompt in the same directory as mdalyzer.so library, a crude Python script
interfacing with the C++ backend is below:

# load the module
import mdalyzer

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

