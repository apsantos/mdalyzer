##
# \file test_trajectory_xtc.py
# \author Sang Beom Kim
# \date 9 January 2015
# \brief Implementation of Python unit tests for trajectory.xtc

import unittest
from mdalyzer import *

##
# \brief Test suite for XTC trajectory Python commands
class trajectory_xtc_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        trajectory.xtc()
        
        t = trajectory.xtc(files='test/unit/xtc/frame.xtc.1')
        t.analyze()
        
        t = trajectory.xtc(files=['test/unit/xtc/frame.xtc.1','test/unit/xtc/frame.xtc.2'])
        t.analyze()
