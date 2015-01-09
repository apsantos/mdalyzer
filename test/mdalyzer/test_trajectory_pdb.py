##
# \file test_trajectory_pdb.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for trajectory.pdb

import unittest
from mdalyzer import *

##
# \brief Test suite for PDB trajectory Python commands
class trajectory_pdb_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        trajectory.pdb()
        trajectory.pdb(time_step=0.1)
        
        t = trajectory.pdb(files='test/unit/pdb/frame.pdb.1')
        t.analyze()
        
        t = trajectory.pdb(files=['test/unit/pdb/frame.pdb.1','test/unit/pdb/frame.pdb.2'])
        t.analyze()
