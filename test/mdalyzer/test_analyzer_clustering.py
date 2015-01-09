##
# \file test_analyzer_clustering.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for analyzer.clustering

import unittest
from mdalyzer import *

##
# \brief Test suite for clustering calculator
class analyzer_clustering_tests(unittest.TestCase):
    ##
    # \brief reset the analyzer counter to zero to check autonumbering
    def setUp(self):
        analyzer.analyzer.counter = 0
        
    ##
    # \brief check all parameters default initialize correctly
    def test_default_init(self):
        t = trajectory.hoomd()
        a = analyzer.clustering(traj=t)
        
        self.assertEqual(a.file_name, 'clustering.dat')
        self.assertEqual(a.distance, 1.0)
        self.assertEqual(a.name, '0')
    
    ##
    # \brief check all parameters are set correctly    
    def test_full_init(self):
        t = trajectory.hoomd()
        a = analyzer.clustering(traj=t, file_name='foo', name='bar', distance=5.0)

        self.assertEqual(a.file_name, 'foo')
        self.assertEqual(a.name, 'bar')
        self.assertEqual(a.distance, 5.0)
    
    ##
    # \brief check distance setter
    def test_set_distance(self):
        t = trajectory.hoomd()
        a = analyzer.clustering(traj=t)
        
        a.set_distance(5.0)
        self.assertEqual(a.distance, 5.0)
        