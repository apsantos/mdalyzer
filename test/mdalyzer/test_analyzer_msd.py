##
# \file test_analyzer_msd.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for analyzer.msd

import unittest
from mdalyzer import *

##
# \brief Test suite for mean-squared displacement calculator
class analyzer_msd_tests(unittest.TestCase):
    ##
    # \brief reset the analyzer counter to zero to check autonumbering
    def setUp(self):
        analyzer.analyzer.counter = 0
        
    ##
    # \brief check all parameters default initialize correctly
    def test_default_init(self):
        t = trajectory.hoomd()
        a = analyzer.msd(traj=t)
        
        self.assertEqual(a.file_name, 'msd')
        self.assertEqual(a.origins, 1)
        self.assertEqual(a.name, '0')
        self.assertEqual(len(a.types), 0)
    
    ##
    # \brief check all parameters are set correctly    
    def test_full_init(self):
        t = trajectory.hoomd()
        a = analyzer.msd(traj=t, file_name='foo', origins=5, name='bar', types=['A','C'])

        self.assertEqual(a.file_name, 'foo')
        self.assertEqual(a.origins, 5)
        self.assertEqual(a.name, 'bar')
        self.assertEqual(a.types, ['A','C'])
    
    ##
    # \brief check type addition works for strings, lists, and tuples
    def test_add_type(self):
        t = trajectory.hoomd()
        a = analyzer.msd(traj=t)
        
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
        a = analyzer.msd(traj=t, types=['A','B','C','D'])
        
        # string
        a.delete_type('B')
        self.assertEqual(a.types, ['A','C','D'])
        
        # list
        a.delete_type(['A','C'])
        self.assertEqual(a.types,['D'])
        
        # tuple
        a.delete_type(('D'))
        self.assertEqual(len(a.types), 0)
    
        