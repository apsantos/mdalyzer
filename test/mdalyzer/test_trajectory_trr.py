##
# \file test_trajectory_trr.py
# \author Sang Beom Kim
# \date 9 January 2015
# \brief Implementation of Python unit tests for trajectory.trr

import unittest
from mdalyzer import *

##
# \brief Test suite for TRR trajectory Python commands
class trajectory_trr_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        trajectory.trr()
        
        t = trajectory.trr(files='test/unit/trr/frame.trr.1')
        t.analyze()
        
        t = trajectory.trr(files=['test/unit/trr/frame.trr.1','test/unit/trr/frame.trr.2'])
        t.analyze()
