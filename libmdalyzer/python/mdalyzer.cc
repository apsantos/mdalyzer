#include <boost/python.hpp>

#include "Frame.h"
#include "HOOMDXMLFrame.h"

int foo()
    {
    return 1;
    }

BOOST_PYTHON_MODULE(mdalyzer)
    {
    using namespace boost::python;
    
    export_Frame();
    export_HOOMDXMLFrame();
    }