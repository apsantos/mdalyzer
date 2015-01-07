##
# \file test_trajectory.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for trajectory.trajectory

import unittest
from mdalyzer import *

##
# \brief Test suite for general trajectory
#
# Uses the HOOMD XML trajectory as the C++ backend
class trajectory_tests(unittest.TestCase):
    ##
    # \brief check add file doesn't throw error using trajectory.hoomd
    def test_add(self):
        traj = trajectory.hoomd()
        traj.add('test/unit/hoomd/frame.xml.1')
        traj.add(['test/unit/hoomd/frame.xml.2','test/unit/hoomd/frame.xml.3'])
        
    ##
    # \brief check analyze doesn't throw error using trajectory.hoomd 
    def test_analyze(self):
        traj = trajectory.hoomd()
        traj.add('test/unit/hoomd/frame.xml.1')
        traj.analyze()
        
    ##
    # \brief check set_box doesn't throw an error using trajectory.hoomd
    def test_set_box(self):
        traj = trajectory.hoomd()
        traj.set_box(lx=11, ly=5, lz=8)
        traj.set_box(lx=11, ly=5, lz=8, xy=0.5, xz=0.0, yz=-2.0)
    