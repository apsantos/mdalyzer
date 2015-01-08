##
# \file test_trajectory_hoomd.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for trajectory.hoomd

import unittest
from mdalyzer import *

##
# \brief Test suite for HOOMD XML trajectory Python commands
class trajectory_hoomd_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        trajectory.hoomd()
        trajectory.hoomd(dt=0.005)
