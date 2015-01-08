##
# \file test_analyzer_temperature.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for analyzer.temperature

import unittest
from mdalyzer import *

##
# \brief Test suite for temperature calculator
class analyzer_temperature_tests(unittest.TestCase):
    ##
    # \brief reset the analyzer counter to zero to check autonumbering
    def setUp(self):
        analyzer.analyzer.counter = 0
        
    ##
    # \brief check all parameters default initialize correctly
    def test_default_init(self):
        t = trajectory.hoomd()
        a = analyzer.temperature(traj=t)
        
        self.assertEqual(a.file_name, 'temperature')
        self.assertEqual(a.bins.x, 0)
        self.assertEqual(a.bins.y, 0)
        self.assertEqual(a.bins.z, 0)
        self.assertEqual(a.name, '0')
        self.assertEqual(len(a.types), 0)
    
    ##
    # \brief check all parameters are set correctly    
    def test_full_init(self):
        t = trajectory.hoomd()
        a = analyzer.temperature(traj=t, file_name='foo', nx=1, ny=2, nz=3, name='bar', types=['A','C'])

        self.assertEqual(a.file_name, 'foo')
        self.assertEqual(a.bins.x, 1)
        self.assertEqual(a.bins.y, 2)
        self.assertEqual(a.bins.z, 3) 
        self.assertEqual(a.name, 'bar')
        self.assertEqual(a.types, ['A','C'])
    
    ##
    # \brief check type addition works for strings, lists, and tuples
    def test_add_type(self):
        t = trajectory.hoomd()
        a = analyzer.temperature(traj=t)
        
        # string
        a.add_type('A')
        self.assertEqual(a.types[0], 'A')
        
        # list
        a.add_type(['B','C'])
        self.assertEqual(a.types, ['A','B','C'])
        
        # tuple
        a.add_type(('D'))
        self.assertEqual(a.types, ['A','B','C','D'])
    
    ##
    # \brief check type deletion works for strings, lists, and tuples
    def test_delete_type(self):
        t = trajectory.hoomd()
        a = analyzer.temperature(traj=t, types=['A','B','C','D'])
        
        # string
        a.delete_type('B')
        self.assertEqual(a.types, ['A','C','D'])
        
        # list
        a.delete_type(['A','C'])
        self.assertEqual(a.types,['D'])
        
        # tuple
        a.delete_type(('D'))
        self.assertEqual(len(a.types), 0)
        