##
# \file __init__.py
# \author Michael P. Howard
# \date 6 January 2015
# \brief Implementation of Python wrapper to libmdalyzer

import libmdalyzer

from mdalyzer import trajectory
from mdalyzer import analyzer


##
# \package mdalyzer
# \brief Base module for scripting interaction with libmdalyzer
#
# mdalyzer provides high level wrappers to Python exposed C++ methods.
# It should be imported using
# \code
# from mdalyzer import *
# \endcode
# for most convenient access. 

