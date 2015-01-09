##
# \file test_analyzer_rdf.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for analyzer.rdf

import unittest
from mdalyzer import *

##
# \brief Test suite for radial distribution function calculator
class analyzer_density_tests(unittest.TestCase):
    ##
    # \brief reset the analyzer counter to zero to check autonumbering
    def setUp(self):
        analyzer.analyzer.counter = 0
        
    ##
    # \brief check all parameters default initialize correctly
    def test_default_init(self):
        t = trajectory.hoomd()
        a = analyzer.rdf(traj=t)
        
        self.assertEqual(a.file_name, 'rdf.dat')
        self.assertEqual(a.origins, 1)
        self.assertEqual(a.bin_size, 1.0)
        self.assertEqual(a.max_radius, 0.0)
        self.assertEqual(a.name, '0')
    
    ##
    # \brief check all parameters are set correctly    
    def test_full_init(self):
        t = trajectory.hoomd()
        a = analyzer.rdf(traj=t, file_name='foo', origins=5, bin_size=0.1, max_radius=5.0, name='bar')

        self.assertEqual(a.file_name, 'foo')
        self.assertEqual(a.origins, 5)
        self.assertEqual(a.bin_size, 0.1)
        self.assertEqual(a.max_radius, 5.0)
        self.assertEqual(a.name, 'bar')
        