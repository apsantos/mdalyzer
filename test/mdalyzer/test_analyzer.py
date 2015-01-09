##
# \file test_analyzer.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Implementation of Python unit tests for analyzer.analyzer

import unittest
from mdalyzer import *

##
# \brief Test suite for general analyzer
class analyzer_tests(unittest.TestCase):
    ##
    # \brief reset the analyzer counter in case others have been initialized
    def setUp(self):
        analyzer.analyzer.counter = 0
    
    ##
    # \brief check that unique autonaming using class counter works
    def test_autoname(self):
        # no name means name=0
        a1 = analyzer.analyzer(trajectory=None, file_name=None, name=None)
        self.assertEqual(a1.name, '0')
        
        # name, don't increase counter
        a2 = analyzer.analyzer(trajectory=None, file_name=None, name='foo')
        self.assertEqual(a2.name, 'foo')
        
        # second no name means name=1
        a3 = analyzer.analyzer(trajectory=None, file_name=None, name=None)
        self.assertEqual(a3.name, '1')
        