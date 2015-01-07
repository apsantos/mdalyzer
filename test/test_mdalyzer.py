##
# \file test_mdalyzer.py
# \author Michael P. Howard
# \date 7 January 2015
# \brief Driver for Python unit tests using auto-discover feature of unittest

import unittest, sys

# validate arguments
if (len(sys.argv) != 3):
    print 'test_mdalyzer.py: arguments must be $(BUILD_PATH) and $(TEST_DIR)'
    print 'FAILURE'
    exit()

# get build and test dir passed in from Makefile
build_dir = sys.argv[1]
test_dir = sys.argv[2]

# put the mdalyzer module on the python path and import
sys.path.append(build_dir)
from mdalyzer import *

# scan the build dir to collect the Python test suite
ts = unittest.TestLoader().discover('%s/%s/mdalyzer/' % (build_dir, test_dir))

# run the unittest suite
unittest.TextTestRunner(verbosity=1).run(ts) 
