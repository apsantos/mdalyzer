##
# \file test_trajectory_gro.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for trajectory.gro

import unittest
from mdalyzer import *

##
# \brief Test suite for GRO trajectory Python commands
class trajectory_gro_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        trajectory.gro()
        trajectory.gro(precision=5)
        
        t = trajectory.gro(files='test/unit/gro/frame.gro.1')
        t.analyze()
        
        t = trajectory.gro(files=['test/unit/gro/frame.gro.1','test/unit/gro/frame.gro.2'])
        t.analyze()
