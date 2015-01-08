##
# \file test_trajectory_dcd.py
# \author Andrew P. Santos
# \date 7 January 2015
# \brief Implementation of Python unit tests for trajectory.dcd

import unittest
from mdalyzer import *

##
# \brief Test suite for DCD trajectory Python commands
class trajectory_dcd_tests(unittest.TestCase):
    ##
    # \brief test constructors
    def test_init(self):
        dcd_name='test/unit/dcd/frame.dcd.2'
        i_name='test/unit/hoomd/frame.xml.1'
        trajectory.dcd( dcd_file=dcd_name, i_file=i_name)
        trajectory.dcd( dcd_file=dcd_name, i_file=i_name, i_type='xml')
        trajectory.dcd( dcd_file=dcd_name, i_file=i_name, freq=6)
        trajectory.dcd( dcd_file=dcd_name, i_file=i_name, time_step=3.0)
        trajectory.dcd( dcd_file=dcd_name, i_file=i_name, time_step=1.0, freq=3)
        trajectory.dcd( dcd_file=dcd_name, i_file=i_name, i_type='xml', time_step=1.0, freq=3)
        
        t = trajectory.dcd( dcd_file=dcd_name, i_file=i_name)
        t.analyze()

    ##
    # \brief test that an exception is raised if a nonvalid type is specified
    def test_notAtype(self):
        dcd_name='test/unit/dcd/frame.dcd.2'
        i_name='test/unit/hoomd/frame.wrong_type.1'
        self.assertRaises(RuntimeError, lambda: trajectory.dcd( dcd_file=dcd_name, i_file=i_name, i_type='wrong_type'))
        self.assertRaises(RuntimeError, lambda: trajectory.dcd( dcd_file=dcd_name, i_file=i_name))

    ##
    # \brief test that a file name is parsed correctly for a type
    def test_parseFileName(self):
        dcd_name='test/unit/dcd/frame.dcd.2'
        i_name='test/unit/hoomd/frame.xml.1'
        t = trajectory.dcd( dcd_file=dcd_name, i_file=i_name)
        self.assertEqual('xml', t._parseFileName())

    ##
    # \brief test that the correct pointer to C++ trajectory is made
    # raises an exception if it is not correct
    def test_getTraj(self):
        dcd_name='test/unit/dcd/frame.dcd.2'
        i_name='test/unit/hoomd/frame.xml.1'
        t = trajectory.dcd( dcd_file=dcd_name, i_file=i_name)

        ptr_str = str(t._getTraj('xml', 1.2, 3))
        self.assertEqual('HOOMDXMLTrajectory', ptr_str[13:31])

        ptr_str = str(t._getTraj('gro', 1.2, 3))
        self.assertEqual('GROTrajectory', ptr_str[13:26])

        ptr_str = str(t._getTraj('pdb', 1.2, 3))
        self.assertEqual('PDBTrajectory', ptr_str[13:26])

        ptr_str = str(t._getTraj('xyz', 1.2, 3))
        self.assertEqual('XYZTrajectory', ptr_str[13:26])

        self.assertRaises(RuntimeError, lambda: t._getTraj('wrong_type', 1.2, 3))

