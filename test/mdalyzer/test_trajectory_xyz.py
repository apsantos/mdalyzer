##
# \file test_trajectory_xyz.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for trajectory.xyz

import unittest
from mdalyzer import *

##
# \brief Test suite for XYZ trajectory Python commands
class trajectory_xyz_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        t = trajectory.xyz()
    
        t = trajectory.xyz(files='test/unit/xyz/frame.xyz.1')
        t.analyze()
    
        t = trajectory.xyz(files=['test/unit/xyz/frame.xyz.1','test/unit/xyz/frame.xyz.2'])
        t.analyze()
